//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include "vgui_controls/AnimationController.h"
#include "vgui_controls/Label.h"
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "text_message.h"
#include "c_baseplayer.h"
#include "IGameUIFuncs.h"
#include "inputsystem/iinputsystem.h"
#include "steam/steam_api.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "bitmap/bitmap.h"
#include "imageutils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Displays hints across the center of the screen
//-----------------------------------------------------------------------------
class CHudHintDisplay : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE( CHudHintDisplay, vgui::Panel );

public:
	CHudHintDisplay( const char *pElementName );

	void Init();
	void Reset();
	void MsgFunc_HintText( bf_read &msg );
	void FireGameEvent( IGameEvent * event);

	bool SetHintText( wchar_t *text );
	void LocalizeAndDisplay( const char *pszHudTxtMsg, const char *szRawString );

	virtual void PerformLayout();

protected:
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void OnThink();

protected:
	vgui::HFont m_hFont;
	Color		m_bgColor;
	vgui::Label *m_pLabel;
	CUtlVector<vgui::Label *> m_Labels;
	CPanelAnimationVarAliasType( int, m_iTextX, "text_xpos", "8", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iTextY, "text_ypos", "8", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iCenterX, "center_x", "0", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iCenterY, "center_y", "0", "proportional_int" );

	bool		m_bLastLabelUpdateHack;
	CPanelAnimationVar( float, m_flLabelSizePercentage, "HintSize", "0" );
};

DECLARE_HUDELEMENT( CHudHintDisplay );
DECLARE_HUD_MESSAGE( CHudHintDisplay, HintText );

#define MAX_HINT_STRINGS 5


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudHintDisplay::CHudHintDisplay( const char *pElementName ) : BaseClass(NULL, "HudHintDisplay"), CHudElement( pElementName )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	SetVisible( false );
	m_pLabel = new vgui::Label( this, "HudHintDisplayLabel", "" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHintDisplay::Init()
{
	HOOK_HUD_MESSAGE( CHudHintDisplay, HintText );

	// listen for client side events
	ListenForGameEvent( "player_hintmessage" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHintDisplay::Reset()
{
	SetHintText( NULL );
	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "HintMessageHide" ); 
	m_bLastLabelUpdateHack = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHintDisplay::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetFgColor( GetSchemeColor("HintMessageFg", pScheme) );
	m_hFont = pScheme->GetFont( "HudHintText", true );
	m_pLabel->SetBgColor( GetSchemeColor("HintMessageBg", pScheme) );
	m_pLabel->SetPaintBackgroundType( 2 );
	m_pLabel->SetSize( 0, GetTall() );		// Start tiny, it'll grow.
}

//-----------------------------------------------------------------------------
// Purpose: Sets the hint text, replacing variables as necessary
//-----------------------------------------------------------------------------
bool CHudHintDisplay::SetHintText( wchar_t *text )
{
	if ( text == NULL || text[0] == L'\0' )
	{
		return false;
	}

	// clear the existing text
	for (int i = 0; i < m_Labels.Count(); i++)
	{
		m_Labels[i]->MarkForDeletion();
	}
	m_Labels.RemoveAll();

	wchar_t *p = text;

	while ( p )
	{
		wchar_t *line = p;
		wchar_t *end = wcschr( p, L'\n' );
		int linelengthbytes = 0;
		if ( end )
		{
			//*end = 0;	//eek
			p = end+1;
			linelengthbytes = ( end - line ) * 2;
		}
		else
		{
			p = NULL;
		}		

		// replace any key references with bound keys
		wchar_t buf[512];
		UTIL_ReplaceKeyBindings( line, linelengthbytes, buf, sizeof( buf ) );

		// put it in a label
		vgui::Label *label = vgui::SETUP_PANEL(new vgui::Label(this, NULL, buf));
		label->SetFont( m_hFont );
		label->SetPaintBackgroundEnabled( false );
		label->SetPaintBorderEnabled( false );
		label->SizeToContents();
		label->SetContentAlignment( vgui::Label::a_west );
		label->SetFgColor( GetFgColor() );
		m_Labels.AddToTail( vgui::SETUP_PANEL(label) );
	}

	InvalidateLayout( true );

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Resizes the label
//-----------------------------------------------------------------------------
void CHudHintDisplay::PerformLayout()
{
	BaseClass::PerformLayout();
	int i;

	int wide, tall;
	GetSize( wide, tall );

	// find the widest line
	int iDesiredLabelWide = 0;
	for ( i=0; i < m_Labels.Count(); ++i )
	{
		iDesiredLabelWide = MAX( iDesiredLabelWide, m_Labels[i]->GetWide() );
	}

	// find the total height
	int fontTall = vgui::surface()->GetFontTall( m_hFont );
	int labelTall = fontTall * m_Labels.Count();

	iDesiredLabelWide += m_iTextX*2;
	labelTall += m_iTextY*2;

	// Now clamp it to our animation size
	iDesiredLabelWide = (iDesiredLabelWide * m_flLabelSizePercentage);

	int x, y;
	if ( m_iCenterX < 0 )
	{
		x = 0;
	}
	else if ( m_iCenterX > 0 )
	{
		x = wide - iDesiredLabelWide;
	}
	else
	{
		x = (wide - iDesiredLabelWide) / 2;
	}

	if ( m_iCenterY > 0 )
	{
		y = 0;
	}
	else if ( m_iCenterY < 0 )
	{
		y = tall - labelTall;
	}
	else
	{
		y = (tall - labelTall) / 2;
	}

	x = MAX(x,0);
	y = MAX(y,0);

	iDesiredLabelWide = MIN(iDesiredLabelWide,wide);
	m_pLabel->SetBounds( x, y, iDesiredLabelWide, labelTall );

	// now lay out the sub-labels
	for ( i=0; i<m_Labels.Count(); ++i )
	{
		int xOffset = (wide - m_Labels[i]->GetWide()) * 0.5;
		m_Labels[i]->SetPos( xOffset, y + m_iTextY + i*fontTall );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates the label color each frame
//-----------------------------------------------------------------------------
void CHudHintDisplay::OnThink()
{
	m_pLabel->SetFgColor(GetFgColor());
	for (int i = 0; i < m_Labels.Count(); i++)
	{
		m_Labels[i]->SetFgColor(GetFgColor());
	}

	// If our label size isn't at the extreme's, we're sliding open / closed
	// This is a hack to get around InvalideLayout() not getting called when
	// m_flLabelSizePercentage is changed via a HudAnimation.
	if ( ( m_flLabelSizePercentage != 0.0 && m_flLabelSizePercentage != 1.0 ) || m_bLastLabelUpdateHack )
	{
		m_bLastLabelUpdateHack = (m_flLabelSizePercentage != 0.0 && m_flLabelSizePercentage != 1.0);
		InvalidateLayout();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Activates the hint display
//-----------------------------------------------------------------------------
void CHudHintDisplay::MsgFunc_HintText( bf_read &msg )
{
	// Read the string(s)
	char szString[255];
	msg.ReadString( szString, sizeof(szString) );

	char *tmpStr = hudtextmessage->LookupString( szString, NULL );
	LocalizeAndDisplay( tmpStr, szString );
}

//-----------------------------------------------------------------------------
// Purpose: Activates the hint display upon receiving a hint
//-----------------------------------------------------------------------------
void CHudHintDisplay::FireGameEvent( IGameEvent * event)
{
	const char *hintmessage = event->GetString( "hintmessage" );
	char *tmpStr = hudtextmessage->LookupString( hintmessage, NULL );
	LocalizeAndDisplay( tmpStr, hintmessage );
}

extern ConVar sv_hudhint_sound;
ConVar cl_hudhint_sound( "cl_hudhint_sound", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE, "Disable hudhint sounds." );

//-----------------------------------------------------------------------------
// Purpose: Localize, display, and animate the hud element
//-----------------------------------------------------------------------------
void CHudHintDisplay::LocalizeAndDisplay( const char *pszHudTxtMsg, const char *szRawString )
{
	static wchar_t szBuf[128];
	wchar_t *pszBuf;

	// init buffers & pointers
	szBuf[0] = 0;
	pszBuf = szBuf;

	// try to localize
	if ( pszHudTxtMsg )
	{
		pszBuf = g_pVGuiLocalize->Find( pszHudTxtMsg );
	}
	else
	{
		pszBuf = g_pVGuiLocalize->Find( szRawString );
	}

	if ( !pszBuf )
	{
		// use plain ASCII string 
		g_pVGuiLocalize->ConvertANSIToUnicode( szRawString, szBuf, sizeof(szBuf) );
		pszBuf = szBuf;
	}

	// make it visible
	if ( SetHintText( pszBuf ) )
	{
		SetVisible( true );
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "HintMessageShow" ); 

		C_BasePlayer *pLocalPlayer = C_BasePlayer::GetLocalPlayer();
		if ( pLocalPlayer )
		{
#ifndef HL2MP
			if ( sv_hudhint_sound.GetBool() && cl_hudhint_sound.GetBool() )
			{
				pLocalPlayer->EmitSound( "Hud.Hint" );
			}
#endif // HL2MP

			if ( pLocalPlayer->Hints() )
			{
				pLocalPlayer->Hints()->PlayedAHint();
			}
		}
	}
	else
	{
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "HintMessageHide" ); 
	}
}




//-----------------------------------------------------------------------------
// Purpose: Displays small key-centric hints on the right hand side of the screen
//-----------------------------------------------------------------------------
// Up to two actions' worth of Steam Input origins per hint variable
#define MAX_HINT_GLYPHS		( STEAM_INPUT_MAX_ORIGINS * 2 )

struct SteamControllerGlyphSet_t
{
	SteamControllerGlyphSet_t()
	{
		m_nX = 0;
		m_nY = 0;
		m_nWide = 0;
		m_nTall = 0;
		m_nFrames = 0;
		for ( int i = 0; i < MAX_HINT_GLYPHS; i++ )
		{
			m_nTextureIDs[i] = -1;
		}
	}

	int m_nTextureIDs[MAX_HINT_GLYPHS];		// one frame per origin, cycled in Paint
	int m_nX, m_nY;
	int m_nWide, m_nTall;
	int m_nFrames;
};

class CHudHintKeyDisplay : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE( CHudHintKeyDisplay, vgui::Panel );

public:
	CHudHintKeyDisplay( const char *pElementName );
	~CHudHintKeyDisplay();
	void Init();
	void Reset();
	void MsgFunc_KeyHintText( bf_read &msg );
	bool ShouldDraw();

	bool SetHintText( const char *text );

protected:
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void Paint();
	virtual void OnThink();

private:
	CUtlVector<vgui::Label *> m_Labels;
	vgui::HFont m_hSmallFont, m_hLargeFont;
	int		m_iBaseY;

	// Steam Input glyphs drawn over the variable labels
	CUtlVector<SteamControllerGlyphSet_t> m_GlyphSets;
	float	m_flNextGlyphFrameTime;
	int		m_nGlyphFrame;

	CPanelAnimationVarAliasType( float, m_iTextX, "text_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_iTextY, "text_ypos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_iTextGapX, "text_xgap", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_iTextGapY, "text_ygap", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_iYOffset, "YOffset", "0", "proportional_float" );
};

DECLARE_HUDELEMENT( CHudHintKeyDisplay );
DECLARE_HUD_MESSAGE( CHudHintKeyDisplay, KeyHintText );

extern int g_ActionSet;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudHintKeyDisplay::CHudHintKeyDisplay( const char *pElementName ) : BaseClass(NULL, "HudHintKeyDisplay"), CHudElement( pElementName )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	SetVisible( false );
	SetAlpha( 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudHintKeyDisplay::~CHudHintKeyDisplay()
{
	for ( int i = 0; i < m_GlyphSets.Count(); i++ )
	{
		SteamControllerGlyphSet_t &glyphSet = m_GlyphSets[i];
		for ( int j = 0; j != MAX_HINT_GLYPHS; j++ )
		{
			if ( glyphSet.m_nTextureIDs[j] > 0 )
			{
				vgui::surface()->DestroyTextureID( glyphSet.m_nTextureIDs[j] );
				glyphSet.m_nTextureIDs[j] = -1;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHintKeyDisplay::Init()
{
	HOOK_HUD_MESSAGE( CHudHintKeyDisplay, KeyHintText );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHintKeyDisplay::Reset()
{
	SetHintText( NULL );
	SetAlpha( 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHintKeyDisplay::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	m_hSmallFont = pScheme->GetFont( "HudHintTextSmall", true );
	m_hLargeFont = pScheme->GetFont( "HudHintTextLarge", true );

	BaseClass::ApplySchemeSettings( pScheme );
}

ConVar sc_use_glyphs( "sc_use_glyphs", "1", 0, "Debug if the string fallbacks display properly." );
NOV2024_ONLY( ConVar sc_prefer_basic_origin_hints( "sc_prefer_basic_origin_hints", "1", 0, "Only show the basic origins if available." ); )
ConVar sc_tint_glyphs( "sc_tint_glyphs", "0", 0, "Whether or not to tint the glyphs or not." );

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CHudHintKeyDisplay::ShouldDraw( void )
{
	return ( ( GetAlpha() > 0 ) && CHudElement::ShouldDraw() );
}

//-----------------------------------------------------------------------------
// Purpose: Draws the current frame of each Steam Input glyph
//-----------------------------------------------------------------------------
void CHudHintKeyDisplay::Paint()
{
	BaseClass::Paint();

	if ( gpGlobals->curtime > m_flNextGlyphFrameTime )
	{
		m_nGlyphFrame++;
		m_flNextGlyphFrameTime = gpGlobals->curtime + 1.0f;
	}

	for ( int i = 0; i < m_GlyphSets.Count(); i++ )
	{
		if ( !m_GlyphSets[i].m_nFrames )
			continue;

		int nFrame = m_nGlyphFrame % m_GlyphSets[i].m_nFrames;
		if ( m_GlyphSets[i].m_nTextureIDs[nFrame] <= 0 )
			continue;

		vgui::surface()->DrawSetColor( Color( 255, 255, 255, GetFgColor().a() ) );
		vgui::surface()->DrawSetTexture( m_GlyphSets[i].m_nTextureIDs[nFrame] );
		vgui::surface()->DrawTexturedRect( m_GlyphSets[i].m_nX, m_GlyphSets[i].m_nY, m_GlyphSets[i].m_nX + m_GlyphSets[i].m_nWide, m_GlyphSets[i].m_nY + m_GlyphSets[i].m_nTall );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates the label color each frame
//-----------------------------------------------------------------------------
void CHudHintKeyDisplay::OnThink()
{
	for (int i = 0; i < m_Labels.Count(); i++)
	{
		if ( IsX360() && ( i & 1 ) == 0 )
		{
			// Don't change the fg color for buttons (even numbered labels)
			m_Labels[i]->SetAlpha( GetFgColor().a() );
		}
		else
		{
			m_Labels[i]->SetFgColor(GetFgColor());
		}
	}

	int ox, oy;
	GetPos(ox, oy);
	SetPos( ox, m_iBaseY + m_iYOffset );
}

//-----------------------------------------------------------------------------
// Purpose: Sets the hint text, replacing variables as necessary
//-----------------------------------------------------------------------------
bool CHudHintKeyDisplay::SetHintText( const char *text )
{
	if ( text == NULL || text[0] == L'\0' )
		return false;

	// clear the existing text
	for (int i = 0; i < m_Labels.Count(); i++)
	{
		m_Labels[i]->MarkForDeletion();
	}
	m_Labels.RemoveAll();

	for ( int i = 0; i < m_GlyphSets.Count(); i++ )
	{
		SteamControllerGlyphSet_t &glyphSet = m_GlyphSets[i];
		for ( int j = 0; j < MAX_HINT_GLYPHS; j++ )
		{
			if ( glyphSet.m_nTextureIDs[j] > 0 )
			{
				vgui::surface()->DestroyTextureID( glyphSet.m_nTextureIDs[j] );
				glyphSet.m_nTextureIDs[j] = -1;
			}
		}
	}
	m_GlyphSets.RemoveAll();

	// look up the text string
	wchar_t *ws = g_pVGuiLocalize->Find( text );

	// Steam Input players get the Deck_ variant of the hint when there is one
	if ( g_pInputSystem->IsSteamControllerActive() && text[0] )
	{
		const char *pszToken = text;
		if ( text[0] == '#' )
		{
			pszToken = text + 1;
		}

		char szDeckToken[256];
		V_snprintf( szDeckToken, sizeof( szDeckToken ), "Deck_%s", pszToken );
		wchar_t *wsDeck = g_pVGuiLocalize->Find( szDeckToken );
		if ( wsDeck )
		{
			ws = wsDeck;
		}

		InputAnalogActionHandle_t hSteer = SteamInput()->GetAnalogActionHandle( "Steer" );
		InputAnalogActionHandle_t hBrake = SteamInput()->GetAnalogActionHandle( "Brake" );
		InputAnalogActionHandle_t hAccel = SteamInput()->GetAnalogActionHandle( "Accelerate" );
		if ( hSteer && ( hBrake || hAccel ) )
		{
			InputHandle_t handles[STEAM_INPUT_MAX_COUNT];
			SteamInput()->GetConnectedControllers( handles );

			InputHandle_t hController = g_pInputSystem->GetActiveSteamInputHandle();
			if ( !hController )
			{
				hController = handles[0];
			}

			InputActionSetHandle_t hActionSet = SteamInput()->GetCurrentActionSet( hController );

			EInputActionOrigin origins[MAX_HINT_GLYPHS] = {};
			int nSteerOrigins = SteamInput()->GetAnalogActionOrigins( hController, hActionSet, hSteer, origins );
			int nBrakeOrigins = SteamInput()->GetAnalogActionOrigins( hController, hActionSet, hBrake, origins );
			int nAccelOrigins = SteamInput()->GetAnalogActionOrigins( hController, hActionSet, hAccel, origins );
			if ( nSteerOrigins && ( nBrakeOrigins || nAccelOrigins ) )
			{
				V_snprintf( szDeckToken, sizeof( szDeckToken ), "Deck_%s_SeparateAccelBrake", pszToken );
				wsDeck = g_pVGuiLocalize->Find( szDeckToken );
				if ( wsDeck )
				{
					ws = wsDeck;
				}
			}
		}
	}

	wchar_t wszBuf[256];
	if ( !ws || wcslen(ws) <= 0)
	{
		if (text[0] == '#')
		{
			// We don't want to display a localization placeholder, do we?
			return false;
		}
		// use plain ASCII string 
		g_pVGuiLocalize->ConvertANSIToUnicode(text, wszBuf, sizeof(wszBuf));
		ws = wszBuf;
	}

	// parse out the text into a label set
	while ( *ws )
	{
		wchar_t token[256];
		bool isVar = false;

		// check for variables
		if ( *ws == '%' )
		{
			isVar = true;
			++ws;
		}

		// parse out the string
		wchar_t *end = wcschr( ws, '%' );
		if ( end )
		{
			wcsncpy( token, ws, MIN( end - ws, ARRAYSIZE(token)) );
			token[end - ws] = L'\0';	// force null termination
		}
		else
		{
			wcsncpy( token, ws, ARRAYSIZE(token) );
			token[ ARRAYSIZE(token) - 1 ] = L'\0';	// force null termination
		}

		ws += wcslen( token );
		if ( isVar )
		{
			// move over the end of the variable
			++ws; 
		}

		// put it in a label
		vgui::Label *label = vgui::SETUP_PANEL(new vgui::Label(this, NULL, token));

		SteamControllerGlyphSet_t glyphSet;
		bool bIsBitmap = false;

		// modify the label if necessary
		if ( isVar )
		{
			label->SetFont( m_hLargeFont );

			// lookup key names
			char binding[64];
			g_pVGuiLocalize->ConvertUnicodeToANSI( token, binding, sizeof(binding) );

			//!! change some key names into better names
			char friendlyName[64];

			if ( g_pInputSystem->IsSteamControllerActive() )
			{
				InputHandle_t handles[STEAM_INPUT_MAX_COUNT];
				SteamInput()->GetConnectedControllers( handles );

				InputHandle_t hController = g_pInputSystem->GetActiveSteamInputHandle();
				if ( !hController )
				{
					hController = handles[0];
				}

				// Steam Input actions are named after the commands, minus the +
				const char *pszAction = ( binding[0] == '+' ) ? binding + 1 : binding;
				if ( !V_strcmp( "impulse 100", pszAction ) )
				{
					pszAction = "sc_flashlight";
				}
				else if ( !V_strcmp( "impulse 50", pszAction ) )
				{
					pszAction = "sc_squad";
				}

				InputDigitalActionHandle_t hDigital = SteamInput()->GetDigitalActionHandle( pszAction );
				EInputActionOrigin origins[MAX_HINT_GLYPHS] = {};
				InputActionSetHandle_t hActionSet = SteamInput()->GetCurrentActionSet( hController );

				if ( hDigital )
				{
					int nOrigins = SteamInput()->GetDigitalActionOrigins( hController, hActionSet, hDigital, origins );

					// The toggle and smart versions are bound separately
					const char *pszAltAction = pszAction;
					if ( !V_strcmp( "duck", pszAltAction ) )
					{
						pszAltAction = "toggle_duck";
					}
					if ( !V_strcmp( "zoom", pszAltAction ) )
					{
						pszAltAction = "toggle_zoom";
					}
#ifdef HL2_ANNIVERSARY_NOV2024
					if ( !V_strcmp( "speed", pszAltAction ) )
					{
						pszAltAction = "smart_sprint";
					}
#endif
#line 729

					InputDigitalActionHandle_t hAltDigital = SteamInput()->GetDigitalActionHandle( pszAltAction );
					if ( hAltDigital && hAltDigital != hDigital )
					{
						nOrigins += SteamInput()->GetDigitalActionOrigins( hController, hActionSet, hAltDigital, &origins[nOrigins] );
					}

#ifdef HL2_ANNIVERSARY_NOV2024
					if ( sc_prefer_basic_origin_hints.GetBool() )
					{
						bool bHasBasicOrigin = false;
						for ( int i = 0; i < nOrigins; i++ )
						{
							if ( SteamInput()->TranslateActionOrigin( k_ESteamInputType_XBoxOneController, origins[i] ) != k_EInputActionOrigin_None )
							{
								bHasBasicOrigin = true;
								break;
							}
						}

						if ( bHasBasicOrigin )
						{
							// Keep only the origins an Xbox controller has, packed to the front
							CUtlVector<int> freeSlots;
							for ( int i = 0; i < nOrigins; i++ )
							{
								if ( SteamInput()->TranslateActionOrigin( k_ESteamInputType_XBoxOneController, origins[i] ) == k_EInputActionOrigin_None )
								{
									origins[i] = k_EInputActionOrigin_None;
									freeSlots.AddToTail( i );
								}
								else if ( freeSlots.Count() )
								{
									origins[freeSlots[0]] = origins[i];
									freeSlots.Remove( 0 );
									freeSlots.AddToTail( i );
									origins[i] = k_EInputActionOrigin_None;
								}
							}
						}
					}
#endif
#line 769
				}
				else
				{
					const char *pszAnalogAction = pszAction;
					if ( !V_strcmp( "xmove", pszAction ) || !V_strcmp( "forward", pszAction ) || !V_strcmp( "moveleft", pszAction ) ||
						 !V_strcmp( "moveright", pszAction ) || !V_strcmp( "back", pszAction ) )
					{
						pszAnalogAction = "Move";
					}
					else if ( !V_strcmp( "xsteer", pszAction ) )
					{
						pszAnalogAction = "Steer";
					}
					else if ( !V_strcmp( "xaccel", pszAction ) )
					{
						pszAnalogAction = "Accelerate";
					}
					else if ( !V_strcmp( "xbrake", pszAction ) )
					{
						pszAnalogAction = "Brake";
					}
					else if ( !V_strcmp( "xlook", pszAction ) )
					{
						pszAnalogAction = "Camera";
					}

					InputAnalogActionHandle_t hAnalog = SteamInput()->GetAnalogActionHandle( pszAnalogAction );
					if ( hAnalog )
					{
						int nOrigins = SteamInput()->GetAnalogActionOrigins( hController, hActionSet, hAnalog, origins );

						const char *pszAltAction = pszAnalogAction;
						if ( g_ActionSet == GAME_ACTION_SET_VEHICLECONTROLS && !V_strcmp( "xmove", pszAnalogAction ) )
						{
							pszAltAction = "Steer";
						}
						else if ( !V_strcmp( "xsteer", pszAnalogAction ) || !V_strcmp( "xaccel", pszAnalogAction ) || !V_strcmp( "xbrake", pszAnalogAction ) )
						{
							pszAltAction = "Move";
						}

						InputAnalogActionHandle_t hAltAnalog = SteamInput()->GetAnalogActionHandle( pszAltAction );
						if ( hAltAnalog && hAnalog != hAltAnalog )
						{
							SteamInput()->GetAnalogActionOrigins( hController, hActionSet, hAltAnalog, &origins[nOrigins] );
						}
					}
				}

				if ( sc_use_glyphs.GetBool() )
				{
					for ( int i = 0; i < MAX_HINT_GLYPHS && origins[i] != k_EInputActionOrigin_None; i++ )
					{
#ifdef HL2_ANNIVERSARY_NOV2024
						// Glyphs are square and as tall as the font; ask Steam for the next size up
						int nGlyphSize = vgui::surface()->GetFontTall( m_hLargeFont );
						nGlyphSize--;
						nGlyphSize |= nGlyphSize >> 1;
						nGlyphSize |= nGlyphSize >> 2;
						nGlyphSize |= nGlyphSize >> 4;
						nGlyphSize |= nGlyphSize >> 8;
						nGlyphSize |= nGlyphSize >> 16;
						nGlyphSize++;

						ESteamInputGlyphSize eGlyphSize;
						if ( nGlyphSize <= 32 )
						{
							eGlyphSize = k_ESteamInputGlyphSize_Small;
						}
						else if ( nGlyphSize <= 128 )
						{
							eGlyphSize = k_ESteamInputGlyphSize_Medium;
						}
						else
						{
							eGlyphSize = k_ESteamInputGlyphSize_Large;
						}

						const char *pszGlyph = SteamInput()->GetGlyphPNGForActionOrigin( origins[i], eGlyphSize, ESteamInputGlyphStyle_Light | ESteamInputGlyphStyle_SolidABXY );
#else
						const char *pszGlyph = SteamInput()->GetGlyphForActionOrigin_Legacy( origins[i] );
#endif
#line 847
						if ( !pszGlyph )
							continue;

						Bitmap_t bitmap;
						if ( ImgUtl_LoadBitmap( pszGlyph, bitmap ) == CE_SUCCESS )
						{
							if ( sc_tint_glyphs.GetBool() )
							{
								for ( int y = 0; y < bitmap.Height(); y++ )
								{
									for ( int x = 0; x < bitmap.Width(); x++ )
									{
										Color c = bitmap.GetColor( x, y );
										Color fg = GetFgColor();

										float r = c.r() * ( 1.0f / 255.0f );
										float g = c.g() * ( 1.0f / 255.0f );
										float b = c.b() * ( 1.0f / 255.0f );
										float fr = fg.r() * ( 1.0f / 255.0f );
										float fgg = fg.g() * ( 1.0f / 255.0f );
										float fb = fg.b() * ( 1.0f / 255.0f );

										// Leave colored parts of the glyph alone, tint the grey ones
										float flSaturation = MAX( MAX( r, g ), b ) - MIN( MIN( r, g ), b ) - ( 2.0f / 255.0f );
										float flColored = 0.0f;
										if ( flSaturation > 0.0f )
										{
											flColored = flSaturation * 2.0f;
										}
										float flTint = 1.0f - MIN( flColored, 1.0f );

										float flTintR = ( fr - 1.0f ) * flTint + 1.0f;
										float flTintG = ( fgg - 1.0f ) * flTint + 1.0f;
										float flTintB = ( fb - 1.0f ) * flTint + 1.0f;
										r *= flTintR;
										g *= flTintG;
										b *= flTintB;

										c.SetColor( r * 255.0f, g * 255.0f, b * 255.0f, c.a() );
										bitmap.SetColor( x, y, c );
									}
								}
							}

#ifndef HL2_ANNIVERSARY_NOV2024
							int nGlyphSize = vgui::surface()->GetFontTall( m_hLargeFont );
							nGlyphSize--;
							nGlyphSize |= nGlyphSize >> 1;
							nGlyphSize |= nGlyphSize >> 2;
							nGlyphSize |= nGlyphSize >> 4;
							nGlyphSize |= nGlyphSize >> 8;
							nGlyphSize |= nGlyphSize >> 16;
							nGlyphSize++;
#endif
#line 891
							ImgUtl_ResizeBitmap( bitmap, nGlyphSize, nGlyphSize, &bitmap );

							glyphSet.m_nTextureIDs[i] = vgui::surface()->CreateNewTextureID( true );
							glyphSet.m_nFrames++;
							g_pMatSystemSurface->DrawSetTextureRGBAEx2( glyphSet.m_nTextureIDs[i], bitmap.GetBits(), bitmap.Width(), bitmap.Height(), IMAGE_FORMAT_RGBA8888, true, true );

							// Make room for the glyph Paint draws
							bIsBitmap = true;
							label->SetText( L"   " );
						}
					}
				}

				if ( !bIsBitmap )
				{
					const char *pszOrigin = "< not bound >";
					if ( origins[0] != k_EInputActionOrigin_None )
					{
						pszOrigin = SteamInput()->GetStringForActionOrigin( origins[0] );
					}

					wchar_t wszOrigin[128];
					V_UTF8ToUnicode( pszOrigin, wszOrigin, sizeof( wszOrigin ) );
					label->SetText( wszOrigin );
				}
			}
			else
			{
				const char *key = engine->Key_LookupBinding( *binding == '+' ? binding + 1 : binding );
				if ( !key )
				{
					key = "< not bound >";
				}

				Q_snprintf( friendlyName, sizeof(friendlyName), "#%s", key );
				Q_strupr( friendlyName );

				// set the variable text - key may need to be localized (button images for example)
				wchar_t *locName = g_pVGuiLocalize->Find( friendlyName );
				if ( !locName || wcslen(locName) <= 0)
				{
					label->SetText( friendlyName + 1 );
				}
				else
				{
					// Assuming localized vars must be using a bitmap image. *May* not be the case, but since
					// keyboard bindings have never been localized in the past, they probably won't in the future either.
					bIsBitmap = true;
					label->SetText( locName );
				}
			}
		}
		else
		{
			label->SetFont( m_hSmallFont );
		}

		label->SetPaintBackgroundEnabled( false );
		label->SetPaintBorderEnabled( false );
		label->SizeToContents();
		label->SetContentAlignment( vgui::Label::a_west );
		if ( bIsBitmap && isVar )
		{
			// Don't change the color of the button art
			label->SetFgColor( Color(255,255,255,255) );
		}
		else
		{
			label->SetFgColor( GetFgColor() );
		}
		m_Labels.AddToTail( vgui::SETUP_PANEL(label) );
		m_GlyphSets.AddToTail( glyphSet );

		m_flNextGlyphFrameTime = gpGlobals->curtime + 1.5f;
	}

	// find the bounds we need to show
	int widest1 = 0, widest2 = 0;
	for (int i = 0; i < m_Labels.Count(); i++)
	{
		vgui::Label *label = m_Labels[i];

		if (i & 1)
		{
			// help text
			if (label->GetWide() > widest2)
			{
				widest2 = label->GetWide();
			}
		}
		else
		{
			// variable
			if (label->GetWide() > widest1)
			{
				widest1 = label->GetWide();
			}
		}
	}

	// position the labels
	int col1_x = m_iTextX;
	int col2_x = m_iTextX + widest1 + m_iTextGapX;
	int col_y = m_iTextY;

	for (int i = 0; i < m_Labels.Count(); i += 2)
	{
		int rowHeight = 0;
		vgui::Label *label0 = m_Labels[i];
		int tall0 = label0->GetTall();
		rowHeight = tall0;

		// glyphs are drawn as tall as the larger font in the row
		int glyphTall = vgui::surface()->GetFontTall( label0->GetFont() );

		if (i + 1 < m_Labels.Count())
		{
			vgui::Label *label1 = m_Labels[i + 1];
			int tall1 = label1->GetTall();
			rowHeight = MAX(tall0, tall1);
			glyphTall = MAX( glyphTall, vgui::surface()->GetFontTall( label1->GetFont() ) );
			label1->SetPos( col2_x, col_y + (rowHeight - tall1) / 2 );
		}

		m_GlyphSets[i].m_nX = col1_x;
		m_GlyphSets[i].m_nY = col_y + (rowHeight - glyphTall) / 2;
		m_GlyphSets[i].m_nWide = glyphTall;
		m_GlyphSets[i].m_nTall = glyphTall;

		if ( i + 1 < m_GlyphSets.Count() )
		{
			m_GlyphSets[i + 1].m_nX = col2_x;
			m_GlyphSets[i + 1].m_nY = col_y + (rowHeight - glyphTall) / 2;
			m_GlyphSets[i + 1].m_nWide = glyphTall;
			m_GlyphSets[i + 1].m_nTall = glyphTall;
		}

		label0->SetPos( col1_x, col_y + (rowHeight - tall0) / 2 );

		col_y += rowHeight + m_iTextGapY;
	}

	// move ourselves relative to our start position
	int newWide = m_iTextX + col2_x + widest2;
	int newTall = col_y;
	int ox, oy;
	GetPos(ox, oy);

	if (IsRightAligned())
	{
		int oldWide = GetWide();
		int diff = newWide - oldWide;
		ox -= diff;
	}

	if (IsBottomAligned())
	{
		int oldTall = GetTall();
		int diff = newTall - oldTall;
		oy -= diff;
	}

	// set the size of the hint panel to fit
 	SetPos( ox, oy );
 	SetSize( newWide, newTall );

	m_iBaseY = oy;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Activates the hint display
//-----------------------------------------------------------------------------
void CHudHintKeyDisplay::MsgFunc_KeyHintText( bf_read &msg )
{
	// how many strings do we receive ?
	int count = msg.ReadByte();

	// here we expect only one string
	if ( count != 1 )
	{
		DevMsg("CHudHintKeyDisplay::MsgFunc_KeyHintText: string count != 1.\n");
		return;
	}

	// read the string
	char szString[2048];
	msg.ReadString( szString, sizeof(szString) );

	// make it visible
	if ( SetHintText( szString ) )
	{
		SetVisible( true );
 		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "KeyHintMessageShow" ); 
	}
	else
	{
		// it's being cleared, hide the panel
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "KeyHintMessageHide" ); 
	}
}
