//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI Options Panel and option control widgets
//          (KeyButton, ConvarButton, WheelyWheel, SlideySlide, SkillySkill).
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#include "gamepadui_options.h"
#include "gamepadui_interface.h"
#include "gamepadui_basepanel.h"
#include "gamepadui_checkbutton.h"
#include "gamepadui_genericconfirmation.h"

#include <math.h>
#include <stdlib.h>
#include <wctype.h>

#include "cdll_int.h"
#include "IGameUIFuncs.h"
#include "filesystem.h"
#include "inputsystem/iinputsystem.h"
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/materialsystem_config.h"
#include "modes.h"
#include "tier1/utlbuffer.h"
#include "tier1/strtools.h"
#include "tier2/tier2.h"
#include "tier3/tier3.h"
#include "vgui/IInput.h"
#include "vgui/ILocalize.h"
#include "vgui/IPanel.h"
#include "vgui/IScheme.h"
#include "vgui/ISurface.h"
#include "vgui/IVGui.h"
#include "vgui_controls/AnimationController.h"
#include "vgui_controls/Controls.h"

#if defined( USE_SDL )
#include "SDL.h"
#endif

#if !defined( NO_STEAM )
#include "steam/steam_api.h"
#include "steam/isteaminput.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Static state & retail proxy ConVars
//-----------------------------------------------------------------------------
bool GamepadUIKeyButton::s_bBindingKey = false;
GamepadUIOptionsPanel *GamepadUIOptionsPanel::s_pOptionsPanel = NULL;

static void OnVideoOptionChanged( IConVar *pVar, const char *pOldValue, float flOldValue )
{
	if ( GamepadUIOptionsPanel::s_pOptionsPanel )
	{
		GamepadUIOptionsPanel::s_pOptionsPanel->PopulateResolutions();
	}
}

static ConVar _gamepadui_water_detail( "_gamepadui_water_detail", "0", 0 );
static ConVar _gamepadui_shader_detail( "_gamepadui_shader_detail", "1", 0 );
static ConVar _gamepadui_shadow_detail( "_gamepadui_shadow_detail", "0", 0 );
static ConVar _gamepadui_classic_effects( "_gamepadui_classic_effects", "0", 0 );
static ConVar _gamepadui_antialiasing( "_gamepadui_antialiasing", "0", 0 );
static ConVar _gamepadui_aspectratio( "_gamepadui_aspectratio", "0", 0, "", OnVideoOptionChanged );
static ConVar _gamepadui_displaymode( "_gamepadui_displaymode", "0", 0, "", OnVideoOptionChanged );
static ConVar _gamepadui_resolution( "_gamepadui_resolution", "0", 0 );
static ConVar _gamepadui_sound_quality( "_gamepadui_sound_quality", "0", 0 );
static ConVar _gamepadui_closecaptions( "_gamepadui_closecaptions", "0", 0 );
static ConVar _gamepadui_hudaspect( "_gamepadui_hudaspect", "0", 0 );
static ConVar _gamepadui_skill( "_gamepadui_skill", "0", 0 );
static ConVar gamepadui_last_options_tab( "gamepadui_last_options_tab", "0", FCVAR_ARCHIVE );

//-----------------------------------------------------------------------------
// Aspect ratio table (exact retail table @ 0x2cd6e0)
//-----------------------------------------------------------------------------
struct AspectRatioEntry_t
{
	int		m_nMode;
	float	m_flRatio;
};

static const AspectRatioEntry_t s_AspectRatios[4] =
{
	{ 0, 4.0f / 3.0f },
	{ 1, 16.0f / 9.0f },
	{ 2, 16.0f / 10.0f },
	{ 2, 1.0f },
};

static int GetAspectRatioMode( int nWidth, int nHeight )
{
	float flAspect = ( nHeight > 0 ) ? ( (float)nWidth / (float)nHeight ) : ( 4.0f / 3.0f );
	float flBestDiff = 100000.0f;
	int nBestMode = 0;

	for ( int i = 0; i < (int)ARRAYSIZE( s_AspectRatios ); ++i )
	{
		float flDiff = fabsf( s_AspectRatios[i].m_flRatio - flAspect );
		if ( flDiff < flBestDiff )
		{
			flBestDiff = flDiff;
			nBestMode = s_AspectRatios[i].m_nMode;
		}
	}

	return nBestMode;
}

//-----------------------------------------------------------------------------
// Antialiasing mode table (exact retail table @ 0x2cee00, 16 entries)
//-----------------------------------------------------------------------------
struct AAMode_t
{
	GamepadUIString	m_strName;
	int				m_nNumSamples;
	int				m_nQualityLevel;

	AAMode_t() : m_nNumSamples( 1 ), m_nQualityLevel( 0 ) {}
};

static const int MAX_AA_MODES = 16;
static AAMode_t s_AAModes[MAX_AA_MODES];
static int s_nAAModeCount = 0;
static bool s_bAAModesPopulated = false;

//-----------------------------------------------------------------------------
// Proxy ConVar getters (exact retail logic @ 0x0c1bf0..0x0c208c)
//-----------------------------------------------------------------------------
static int GetWaterDetail()
{
	ConVarRef r_waterforceexpensive( "r_waterforceexpensive" );
	ConVarRef r_waterforcereflectentities( "r_waterforcereflectentities" );
	if ( r_waterforcereflectentities.GetInt() != 0 )
		return 2;
	return ( r_waterforceexpensive.GetInt() != 0 ) ? 1 : 0;
}

static int GetShaderDetail()
{
	ConVarRef mat_reducefillrate( "mat_reducefillrate" );
	ConVarRef r_lightmap_bicubic( "r_lightmap_bicubic" );
	if ( r_lightmap_bicubic.GetInt() != 0 )
		return 2;
	return ( mat_reducefillrate.GetInt() == 0 ) ? 1 : 0;
}

static int GetShadowDetail()
{
	ConVarRef r_shadowrendertotexture( "r_shadowrendertotexture" );
	ConVarRef r_flashlightdepthtexture( "r_flashlightdepthtexture" );
	if ( r_flashlightdepthtexture.GetInt() != 0 )
		return 2;
	return ( r_shadowrendertotexture.GetInt() != 0 ) ? 1 : 0;
}

static int GetClassicEffects()
{
	ConVarRef r_classic_fire( "r_classic_fire" );
	return ( r_classic_fire.GetInt() > 0 ) ? 1 : 0;
}

static int GetAntialiasing()
{
	ConVarRef mat_antialias( "mat_antialias" );
	ConVarRef mat_aaquality( "mat_aaquality" );
	int nSamples = mat_antialias.GetInt();
	int nQuality = mat_aaquality.GetInt();

	for ( int i = 0; i < s_nAAModeCount; ++i )
	{
		if ( s_AAModes[i].m_nNumSamples == nSamples && s_AAModes[i].m_nQualityLevel == nQuality )
			return i;
	}
	return 0;
}

static int GetCurrentAspectRatioMode()
{
	if ( !g_pMaterialSystem )
		return 0;
	const MaterialSystem_Config_t &config = g_pMaterialSystem->GetCurrentConfigForVideoCard();
	return GetAspectRatioMode( config.m_VideoMode.m_Width, config.m_VideoMode.m_Height );
}

static int GetDisplayMode()
{
	if ( !g_pMaterialSystem )
		return 2;
	const MaterialSystem_Config_t &config = g_pMaterialSystem->GetCurrentConfigForVideoCard();
	if ( config.Windowed() )
	{
		// Bit 18 is MATSYS_VIDCFG_FLAGS_NO_WINDOW_BORDER in HL2 Anniversary engine (retail @ 0xc1e8c)
		return ( ( config.m_Flags & ( 1u << 18 ) ) != 0 ) ? 1 : 0;
	}

	static ConVarRef sdl_displayindex( "sdl_displayindex" );
	if ( sdl_displayindex.IsValid() )
	{
		return 2 + sdl_displayindex.GetInt();
	}
	return 2;
}

static int GetSoundQuality()
{
	ConVarRef snd_pitchquality( "snd_pitchquality" );
	ConVarRef dsp_slow_cpu( "dsp_slow_cpu" );
	if ( snd_pitchquality.GetInt() != 0 )
		return 2;
	return ( dsp_slow_cpu.GetInt() == 0 ) ? 1 : 0;
}

static int GetCloseCaptions()
{
	ConVarRef closecaption( "closecaption" );
	ConVarRef cc_subtitles( "cc_subtitles" );
	if ( closecaption.GetInt() == 0 )
		return 0;
	return ( cc_subtitles.GetInt() == 0 ) ? 2 : 1;
}

static int GetHudAspect()
{
	ConVarRef hud_aspect( "hud_aspect" );
	float flVal = hud_aspect.GetFloat();
	if ( flVal <= 0.0f )
		return 0;
	if ( flVal < 1.4f )
		return 1;
	if ( flVal < 1.7f )
		return 3;
	return 2;
}

static int GetSkill()
{
	ConVarRef skill( "skill" );
	return skill.GetInt();
}

//-----------------------------------------------------------------------------
// Proxy ConVar appliers (exact retail logic @ 0x0c2090..0x0c27a9)
//-----------------------------------------------------------------------------
static void ApplyAntialiasing()
{
	ConVarRef mat_antialias( "mat_antialias" );
	ConVarRef mat_aaquality( "mat_aaquality" );
	if ( s_nAAModeCount <= 0 )
		return;

	int nIdx = clamp( _gamepadui_antialiasing.GetInt(), 0, s_nAAModeCount - 1 );
	if ( mat_antialias.GetInt() != s_AAModes[nIdx].m_nNumSamples )
		mat_antialias.SetValue( s_AAModes[nIdx].m_nNumSamples );
	if ( mat_aaquality.GetInt() != s_AAModes[nIdx].m_nQualityLevel )
		mat_aaquality.SetValue( s_AAModes[nIdx].m_nQualityLevel );
}

static void ApplyShaderDetail()
{
	ConVarRef mat_reducefillrate( "mat_reducefillrate" );
	ConVarRef r_lightmap_bicubic( "r_lightmap_bicubic" );
	int nVal = _gamepadui_shader_detail.GetInt();

	bool bReduce = ( nVal == 0 );
	bool bBicubic = ( nVal > 1 );
	if ( ( mat_reducefillrate.GetInt() != 0 ) != bReduce )
		mat_reducefillrate.SetValue( bReduce ? 1 : 0 );
	if ( ( r_lightmap_bicubic.GetInt() != 0 ) != bBicubic )
		r_lightmap_bicubic.SetValue( bBicubic ? 1 : 0 );
}

static void ApplyWaterDetail()
{
	ConVarRef r_waterforceexpensive( "r_waterforceexpensive" );
	ConVarRef r_waterforcereflectentities( "r_waterforcereflectentities" );
	int nVal = _gamepadui_water_detail.GetInt();

	bool bExpensive = ( nVal > 0 );
	bool bReflectEntities = ( nVal > 1 );
	if ( ( r_waterforceexpensive.GetInt() != 0 ) != bExpensive )
		r_waterforceexpensive.SetValue( bExpensive ? 1 : 0 );
	if ( ( r_waterforcereflectentities.GetInt() != 0 ) != bReflectEntities )
		r_waterforcereflectentities.SetValue( bReflectEntities ? 1 : 0 );
}

static void ApplyClassicEffects()
{
	ConVarRef r_classic_blood( "r_classic_blood" );
	ConVarRef r_classic_fire( "r_classic_fire" );
	int nVal = ( _gamepadui_classic_effects.GetInt() > 0 ) ? 2 : 0;

	if ( r_classic_blood.GetInt() != nVal )
		r_classic_blood.SetValue( nVal );
	if ( r_classic_fire.GetInt() != nVal )
		r_classic_fire.SetValue( nVal );
}

static void ApplyShadowDetail()
{
	ConVarRef r_shadowrendertotexture( "r_shadowrendertotexture" );
	ConVarRef r_flashlightdepthtexture( "r_flashlightdepthtexture" );
	int nVal = _gamepadui_shadow_detail.GetInt();

	bool bRTT = ( nVal > 0 );
	bool bDepth = ( nVal > 1 );
	if ( ( r_shadowrendertotexture.GetInt() != 0 ) != bRTT )
		r_shadowrendertotexture.SetValue( bRTT ? 1 : 0 );
	if ( ( r_flashlightdepthtexture.GetInt() != 0 ) != bDepth )
		r_flashlightdepthtexture.SetValue( bDepth ? 1 : 0 );
}

static void ApplySoundQuality()
{
	ConVarRef snd_pitchquality( "snd_pitchquality" );
	ConVarRef dsp_slow_cpu( "dsp_slow_cpu" );
	ConVarRef dsp_enhance_stereo( "dsp_enhance_stereo" );
	ConVarRef snd_surround_speakers( "snd_surround_speakers" );

	int nQuality = _gamepadui_sound_quality.GetInt();
	if ( nQuality == 0 )
	{
		snd_pitchquality.SetValue( 0 );
		dsp_slow_cpu.SetValue( 1 );
	}
	else if ( nQuality == 1 )
	{
		snd_pitchquality.SetValue( 0 );
		dsp_slow_cpu.SetValue( 0 );
	}
	else
	{
		dsp_enhance_stereo.SetValue( snd_surround_speakers.GetInt() == 0 ? 1 : 0 );
		snd_pitchquality.SetValue( 1 );
		dsp_slow_cpu.SetValue( 0 );
	}
}

static void ApplyCloseCaptions()
{
	ConVarRef closecaption( "closecaption" );
	ConVarRef cc_subtitles( "cc_subtitles" );

	int nMode = _gamepadui_closecaptions.GetInt();
	int nCC = 0;
	if ( nMode == 0 )
	{
		cc_subtitles.SetValue( 0 );
		nCC = 0;
	}
	else if ( nMode == 1 )
	{
		cc_subtitles.SetValue( 1 );
		nCC = 1;
	}
	else
	{
		cc_subtitles.SetValue( 0 );
		nCC = 1;
	}

	char szCmd[64];
	V_snprintf( szCmd, sizeof( szCmd ), "closecaption %i\n", nCC );
	if ( GamepadUI::GetInstance().GetEngineClient() )
	{
		GamepadUI::GetInstance().GetEngineClient()->ClientCmd_Unrestricted( szCmd );
	}
}

static void ApplyHudAspect()
{
	ConVarRef hud_aspect( "hud_aspect" );
	switch ( _gamepadui_hudaspect.GetInt() )
	{
	case 1:
		hud_aspect.SetValue( 4.0f / 3.0f );
		break;
	case 2:
		hud_aspect.SetValue( 16.0f / 9.0f );
		break;
	case 3:
		hud_aspect.SetValue( 1.6f );
		break;
	default:
		hud_aspect.SetValue( 0.0f );
		break;
	}
}

static void ApplySkill()
{
	ConVarRef skill( "skill" );
	skill.SetValue( _gamepadui_skill.GetInt() );
}

//-----------------------------------------------------------------------------
// Helper: parse next token via IVEngineClient::ParseFile (retail @ 0x0c2980)
//-----------------------------------------------------------------------------
static const char *Options_ParseToken( const char *pData, char *pToken, int nMaxLen )
{
	IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient();
	if ( !pEngine || !pData )
		return NULL;
	return pEngine->ParseFile( pData, pToken, nMaxLen );
}

//-----------------------------------------------------------------------------
// GamepadUIKeyButton
//-----------------------------------------------------------------------------
GamepadUIKeyButton::GamepadUIKeyButton(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const char *pText,
	const char *pDescription,
	const char *pAction )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription )
	, m_strAction( pAction ? pAction : "" )
	, m_strKey( "" )
	, m_bBinding( false )
	, m_nSavedCursorX( 0 )
	, m_nSavedCursorY( 0 )
	, m_flBindingWidth( 40.0f )
	, m_flBindingHeight( 40.0f )
	, m_colBinding( 0, 0, 0, 255 )
{
}

GamepadUIKeyButton::~GamepadUIKeyButton()
{
}

ButtonState::State GamepadUIKeyButton::GetCurrentButtonState()
{
	if ( m_bBinding )
		return ButtonState::Pressed;
	if ( s_bBindingKey )
		return ButtonState::Out;
	return BaseClass::GetCurrentButtonState();
}

void GamepadUIKeyButton::BeginKeyBinding()
{
	if ( s_bBindingKey )
		return;

	m_bBinding = true;
	s_bBindingKey = true;

	if ( g_pInputSystem )
		g_pInputSystem->SetNovintPure( true );

	if ( GamepadUI::GetInstance().GetEngineClient() )
		GamepadUI::GetInstance().GetEngineClient()->StartKeyTrapMode();

	vgui::surface()->SetCursor( vgui::dc_none );
	SetCursor( vgui::dc_none );
	vgui::input()->GetCursorPos( m_nSavedCursorX, m_nSavedCursorY );
}

void GamepadUIKeyButton::OnMouseDoublePressed( vgui::MouseCode code )
{
	if ( s_bBindingKey )
		return;
	BeginKeyBinding();
}

void GamepadUIKeyButton::OnKeyCodePressed( vgui::KeyCode code )
{
	// Ignore joystick/gamepad/Steam Controller button codes (retail 0xcdcd6..0xcdd0f)
	if ( IsJoystickButtonCode( code ) || IsSteamControllerCode( code ) )
	{
		BaseClass::OnKeyCodePressed( code );
		return;
	}

	if ( code == KEY_ENTER )
	{
		if ( !s_bBindingKey )
		{
			BeginKeyBinding();
		}
		return;
	}

	// Retail clears keybind on KEY_DELETE (0x49 @ 0xcddb0)
	if ( code == KEY_DELETE )
	{
		if ( !s_bBindingKey )
		{
			m_strKey = "";
		}
		return;
	}

	BaseClass::OnKeyCodePressed( code );
}

void GamepadUIKeyButton::OnThink()
{
	BaseClass::OnThink();

	if ( !m_bBinding )
		return;

	ButtonCode_t trappedCode = BUTTON_CODE_INVALID;
	IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient();
	if ( !pEngine || !pEngine->CheckDoneKeyTrapping( trappedCode ) )
		return;

	m_bBinding = false;
	s_bBindingKey = false;

	vgui::input()->SetMouseCapture( NULL );
	RequestFocus( 0 );

	if ( g_pInputSystem )
		g_pInputSystem->SetNovintPure( false );

	vgui::surface()->SetCursor( vgui::dc_arrow );
	SetCursor( vgui::dc_arrow );
	vgui::input()->SetCursorPos( m_nSavedCursorX, m_nSavedCursorY );

	const char *pszKey = g_pInputSystem ? g_pInputSystem->ButtonCodeToString( trappedCode ) : "";
	if ( GamepadUIOptionsPanel::s_pOptionsPanel && pszKey )
	{
		GamepadUIOptionsPanel::s_pOptionsPanel->ClearDuplicateKeyBind( pszKey );
	}
	m_strKey = pszKey ? pszKey : "";
}

void GamepadUIKeyButton::Paint()
{
	BaseClass::Paint();

	if ( m_bBinding )
	{
		vgui::surface()->DrawSetColor( m_colBinding );
		int nX1 = (int)( m_flWidth - m_flTextOffsetX );
		int nX0 = (int)( ( m_flWidth - m_flTextOffsetX ) - m_flBindingWidth );
		int nY0 = (int)( ( m_flHeight - m_flBindingHeight ) * 0.5f );
		int nY1 = (int)( ( m_flHeight + m_flBindingHeight ) * 0.5f );
		vgui::surface()->DrawFilledRect( nX0, nY0, nX1, nY1 );
		return;
	}

	ButtonState::State eState = GetCurrentButtonState();

	wchar_t wszKey[128];
	wszKey[0] = L'\0';
	V_UTF8ToUnicode( m_strKey.String(), wszKey, sizeof( wszKey ) );
	for ( int i = 0; wszKey[i] != L'\0'; ++i )
	{
		wszKey[i] = (wchar_t)towupper( wszKey[i] );
	}

	vgui::HFont hFont = ( eState != ButtonState::Out && m_hTextFontOver != vgui::INVALID_FONT ) ? m_hTextFontOver : m_hTextFont;
	vgui::surface()->DrawSetTextColor( m_colTextColor );
	vgui::surface()->DrawSetTextFont( hFont );

	int nTextW = 0, nTextH = 0;
	vgui::surface()->GetTextSize( hFont, wszKey, nTextW, nTextH );

	int nX = (int)( m_flWidth - (float)nTextW - m_flTextOffsetX );
	int nY = (int)( m_flHeight * 0.5f - (float)( nTextH / 2 ) );
	vgui::surface()->DrawSetTextPos( nX, nY );
	vgui::surface()->DrawPrintText( wszKey, (int)wcslen( wszKey ) );
}

//-----------------------------------------------------------------------------
// GamepadUIConvarButton
//-----------------------------------------------------------------------------
GamepadUIConvarButton::GamepadUIConvarButton(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const char *pText,
	const char *pDescription,
	const char *pConvarName,
	const char *pDependsOn,
	bool bInstantApply )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription )
	, m_ConVarRef( pConvarName ? pConvarName : "" )
	, m_strDependsOn( pDependsOn ? pDependsOn : "" )
	, m_bInstantApply( bInstantApply )
{
}

GamepadUIConvarButton::~GamepadUIConvarButton()
{
}

const char *GamepadUIConvarButton::GetConvarName() const
{
	return m_ConVarRef.IsValid() ? m_ConVarRef.GetName() : "";
}

//-----------------------------------------------------------------------------
// GamepadUIWheelyWheel
//-----------------------------------------------------------------------------
GamepadUIWheelyWheel::GamepadUIWheelyWheel(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const char *pText,
	const char *pDescription,
	const char *pConvarName,
	const char *pDependsOn,
	bool bInstantApply,
	bool bSignOnly )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription, pConvarName, pDependsOn, bInstantApply )
	, m_bSignOnly( bSignOnly )
	, m_nCurrentOption( 0 )
{
}

GamepadUIWheelyWheel::~GamepadUIWheelyWheel()
{
}

void GamepadUIWheelyWheel::AddOption( const char *pszLocalizedOrRawName, int nValue, int nWidth, int nHeight )
{
	Option_t opt;
	opt.m_strName.SetString( pszLocalizedOrRawName ? pszLocalizedOrRawName : "" );
	opt.m_nValue = nValue;
	opt.m_nWidth = nWidth;
	opt.m_nHeight = nHeight;
	m_Options.AddToTail( opt );
}

void GamepadUIWheelyWheel::AddOptionW( const wchar_t *pwszName, int nValue, int nWidth, int nHeight )
{
	Option_t opt;
	opt.m_strName.SetString( pwszName ? pwszName : L"" );
	opt.m_nValue = nValue;
	opt.m_nWidth = nWidth;
	opt.m_nHeight = nHeight;
	m_Options.AddToTail( opt );
}

const GamepadUIWheelyWheel::Option_t *GamepadUIWheelyWheel::GetOption( int nIndex ) const
{
	if ( nIndex < 0 || nIndex >= m_Options.Count() )
		return NULL;
	return &m_Options[nIndex];
}

bool GamepadUIWheelyWheel::IsConvarDirty()
{
	if ( !m_ConVarRef.IsValid() || m_Options.Count() <= 0 )
		return false;

	int nCurVal = m_bSignOnly
		? ( ( m_ConVarRef.GetFloat() >= 0.0f ) ? 1 : -1 )
		: m_ConVarRef.GetInt();

	int nIdx = clamp( m_nCurrentOption, 0, m_Options.Count() - 1 );
	return m_Options[nIdx].m_nValue != nCurVal;
}

void GamepadUIWheelyWheel::ApplyConvar()
{
	if ( !IsConvarDirty() )
		return;

	int nIdx = clamp( m_nCurrentOption, 0, m_Options.Count() - 1 );
	if ( m_bSignOnly )
	{
		float flMag = fabsf( m_ConVarRef.GetFloat() );
		m_ConVarRef.SetValue( flMag * (float)m_Options[nIdx].m_nValue );
	}
	else
	{
		m_ConVarRef.SetValue( m_Options[nIdx].m_nValue );
	}
}

void GamepadUIWheelyWheel::UpdateConvar()
{
	if ( !m_ConVarRef.IsValid() || m_Options.Count() <= 0 )
		return;

	int nCurVal = m_bSignOnly
		? ( ( m_ConVarRef.GetFloat() >= 0.0f ) ? 1 : -1 )
		: m_ConVarRef.GetInt();

	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		if ( m_Options[i].m_nValue == nCurVal )
		{
			m_nCurrentOption = i;
		}
	}
}

bool GamepadUIWheelyWheel::IsTruthy()
{
	if ( m_nCurrentOption < 0 || m_nCurrentOption >= m_Options.Count() )
		return false;
	return m_Options[m_nCurrentOption].m_nValue != 0;
}

void GamepadUIWheelyWheel::OnKeyCodePressed( vgui::KeyCode code )
{
	ButtonCode_t baseCode = GamepadUI_NormalizeJoystickCode( code );

	bool bLeft = ( baseCode == KEY_LEFT || baseCode == KEY_XBUTTON_LEFT || baseCode == KEY_XSTICK1_LEFT
		|| baseCode == STEAMCONTROLLER_DPAD_LEFT || baseCode == STEAMCONTROLLER_LEFT_PAD_LEFT || baseCode == STEAMCONTROLLER_RIGHT_PAD_LEFT );
	if ( bLeft )
	{
		--m_nCurrentOption;
		if ( m_nCurrentOption < 0 )
		{
			m_nCurrentOption = MAX( 0, m_Options.Count() - 1 );
		}
		if ( m_bInstantApply )
		{
			ApplyConvar();
		}
		return;
	}

	bool bRight = ( baseCode == KEY_RIGHT || baseCode == KEY_XBUTTON_RIGHT || baseCode == KEY_XSTICK1_RIGHT
		|| baseCode == STEAMCONTROLLER_DPAD_RIGHT || baseCode == STEAMCONTROLLER_LEFT_PAD_RIGHT || baseCode == STEAMCONTROLLER_RIGHT_PAD_RIGHT );
	if ( bRight )
	{
		if ( m_Options.Count() > 0 )
		{
			m_nCurrentOption = ( m_nCurrentOption + 1 ) % m_Options.Count();
		}
		if ( m_bInstantApply )
		{
			ApplyConvar();
		}
		return;
	}

	BaseClass::OnKeyCodePressed( code );
}

void GamepadUIWheelyWheel::Paint()
{
	BaseClass::Paint();

	if ( m_nCurrentOption < 0 || m_nCurrentOption >= m_Options.Count() )
		return;

	const Option_t &opt = m_Options[m_nCurrentOption];
	ButtonState::State eState = GetCurrentButtonState();

	int nTextW = 0, nTextH = 0;
	vgui::surface()->GetTextSize( m_hTextFont, opt.m_strName.String(), nTextW, nTextH );

	int nChevronW = vgui::surface()->GetCharacterWidth( m_hTextFont, L'<' )
		+ vgui::surface()->GetCharacterWidth( m_hTextFont, L' ' );

	int nY = (int)( m_flTextOffsetY - (float)( nTextH / 2 ) + m_flHeight * 0.5f );
	vgui::surface()->DrawSetTextColor( m_colTextColor );
	vgui::surface()->DrawSetTextFont( m_hTextFont );

	if ( eState != ButtonState::Out )
	{
		int nLeftX = (int)( m_flWidth - (float)nTextW - ( m_flTextOffsetX + (float)( nChevronW * 2 ) ) );
		vgui::surface()->DrawSetTextPos( nLeftX, nY );
		vgui::surface()->DrawPrintText( L"< ", 2 );
	}

	int nValX = (int)( m_flWidth - (float)nTextW - ( m_flTextOffsetX + (float)nChevronW ) );
	vgui::surface()->DrawSetTextPos( nValX, nY );
	vgui::surface()->DrawPrintText( opt.m_strName.String(), opt.m_strName.Length() );

	if ( eState != ButtonState::Out )
	{
		int nRightX = (int)( m_flWidth - m_flTextOffsetX - (float)nChevronW );
		vgui::surface()->DrawSetTextPos( nRightX, nY );
		vgui::surface()->DrawPrintText( L" >", 2 );
	}
}

//-----------------------------------------------------------------------------
// GamepadUISlideySlide
//-----------------------------------------------------------------------------
GamepadUISlideySlide::GamepadUISlideySlide(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const char *pText,
	const char *pDescription,
	const char *pConvarName,
	const char *pDependsOn,
	bool bInstantApply,
	float flMin,
	float flMax,
	float flStep,
	int nTextPrecision )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription, pConvarName, pDependsOn, bInstantApply )
	, m_flValue( 0.0f )
	, m_flMin( flMin )
	, m_flMax( flMax )
	, m_flStep( flStep )
	, m_nTextPrecision( nTextPrecision )
	, m_flSliderBackingAnimationDuration( 0.2f )
	, m_flSliderFillAnimationDuration( 0.2f )
	, m_flSliderWidth( 160.0f )
	, m_flSliderHeight( 11.0f )
{
	m_colSliderBacking = Color( 255, 255, 255, 22 );
	m_colSliderFill = Color( 255, 255, 255, 255 );
	for ( int i = 0; i < ButtonState::Count; ++i )
	{
		m_colSliderBackings[i] = Color( 255, 255, 255, 22 );
		m_colSliderFills[i] = Color( 255, 255, 255, 255 );
	}

	SCHEME_MEMBER_COLOR( GamepadUISlideySlide, m_colSliderBacking, "Slider.Backing.Out", "255 255 255 22" );
	SCHEME_MEMBER_COLOR( GamepadUISlideySlide, m_colSliderBackings[ButtonState::Out], "Slider.Backing.Out", "255 255 255 22" );
	SCHEME_MEMBER_COLOR( GamepadUISlideySlide, m_colSliderBackings[ButtonState::Over], "Slider.Backing.Over", "255 255 255 22" );
	SCHEME_MEMBER_COLOR( GamepadUISlideySlide, m_colSliderBackings[ButtonState::Pressed], "Slider.Backing.Pressed", "255 255 255 22" );
	SCHEME_MEMBER_FLOAT( GamepadUISlideySlide, m_flSliderBackingAnimationDuration, "Slider.Backing.Animation.Duration", "0.2" );

	SCHEME_MEMBER_COLOR( GamepadUISlideySlide, m_colSliderFill, "Slider.Fill.Out", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUISlideySlide, m_colSliderFills[ButtonState::Out], "Slider.Fill.Out", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUISlideySlide, m_colSliderFills[ButtonState::Over], "Slider.Fill.Over", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUISlideySlide, m_colSliderFills[ButtonState::Pressed], "Slider.Fill.Pressed", "255 255 255 255" );
	SCHEME_MEMBER_FLOAT( GamepadUISlideySlide, m_flSliderFillAnimationDuration, "Slider.Fill.Animation.Duration", "0.2" );

	UpdateConvar();
}

GamepadUISlideySlide::~GamepadUISlideySlide()
{
}

bool GamepadUISlideySlide::IsConvarDirty()
{
	if ( !m_ConVarRef.IsValid() )
		return false;
	return m_ConVarRef.GetFloat() != m_flValue;
}

void GamepadUISlideySlide::ApplyConvar()
{
	if ( !IsConvarDirty() )
		return;
	m_ConVarRef.SetValue( m_flValue );
}

void GamepadUISlideySlide::UpdateConvar()
{
	if ( !m_ConVarRef.IsValid() )
		return;
	m_flValue = m_ConVarRef.GetFloat();
}

void GamepadUISlideySlide::RunAnimations( ButtonState::State state )
{
	BaseClass::RunAnimations( state );

	vgui::AnimationController *pAnim = GamepadUI::GetInstance().GetAnimationController();
	if ( pAnim )
	{
		pAnim->RunAnimationCommand( this, "m_colSliderBacking", m_colSliderBackings[state], 0.0f, m_flSliderBackingAnimationDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
		pAnim->RunAnimationCommand( this, "m_colSliderFill", m_colSliderFills[state], 0.0f, m_flSliderFillAnimationDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	}
}

void GamepadUISlideySlide::OnKeyCodePressed( vgui::KeyCode code )
{
	ButtonCode_t baseCode = GamepadUI_NormalizeJoystickCode( code );

	bool bLeft = ( baseCode == KEY_LEFT || baseCode == KEY_XBUTTON_LEFT || baseCode == KEY_XSTICK1_LEFT
		|| baseCode == STEAMCONTROLLER_DPAD_LEFT || baseCode == STEAMCONTROLLER_LEFT_PAD_LEFT || baseCode == STEAMCONTROLLER_RIGHT_PAD_LEFT );
	if ( bLeft )
	{
		m_flValue = clamp( m_flValue - m_flStep, m_flMin, m_flMax );
		if ( m_bInstantApply )
		{
			ApplyConvar();
		}
		return;
	}

	bool bRight = ( baseCode == KEY_RIGHT || baseCode == KEY_XBUTTON_RIGHT || baseCode == KEY_XSTICK1_RIGHT
		|| baseCode == STEAMCONTROLLER_DPAD_RIGHT || baseCode == STEAMCONTROLLER_LEFT_PAD_RIGHT || baseCode == STEAMCONTROLLER_RIGHT_PAD_RIGHT );
	if ( bRight )
	{
		m_flValue = clamp( m_flValue + m_flStep, m_flMin, m_flMax );
		if ( m_bInstantApply )
		{
			ApplyConvar();
		}
		return;
	}

	BaseClass::OnKeyCodePressed( code );
}

void GamepadUISlideySlide::Paint()
{
	BaseClass::Paint();

	if ( m_nTextPrecision >= 0 )
	{
		wchar_t wszVal[1024];
		V_snwprintf( wszVal, ARRAYSIZE( wszVal ), L"%.*f", m_nTextPrecision, (double)m_flValue );

		int nTextW = 0, nTextH = 0;
		vgui::surface()->GetTextSize( m_hTextFont, wszVal, nTextW, nTextH );

		int nTextX = (int)( m_flWidth - (float)nTextW - ( m_flTextOffsetX * 2.0f + m_flSliderWidth ) );
		int nTextY = (int)( m_flHeight * 0.5f - (float)( nTextH / 2 ) );
		vgui::surface()->DrawSetTextColor( m_colTextColor );
		vgui::surface()->DrawSetTextFont( m_hTextFont );
		vgui::surface()->DrawSetTextPos( nTextX, nTextY );
		vgui::surface()->DrawPrintText( wszVal, (int)wcslen( wszVal ) );
	}

	float flRight = m_flWidth - m_flTextOffsetX;
	float flLeft = flRight - m_flSliderWidth;
	float flTop = ( m_flHeight - m_flSliderHeight ) * 0.5f;
	float flBottom = ( m_flHeight + m_flSliderHeight ) * 0.5f;

	vgui::surface()->DrawSetColor( m_colSliderBacking );
	vgui::surface()->DrawFilledRect( (int)flLeft, (int)flTop, (int)flRight, (int)flBottom );

	float flRange = m_flMax - m_flMin;
	float flFrac = ( flRange > 0.0f ) ? ( ( m_flValue - m_flMin ) / flRange ) : 0.0f;
	float flFillRight = flRight - ( 1.0f - flFrac ) * m_flSliderWidth;

	vgui::surface()->DrawSetColor( m_colSliderFill );
	vgui::surface()->DrawFilledRect( (int)flLeft, (int)flTop, (int)flFillRight, (int)flBottom );
}

//-----------------------------------------------------------------------------
// GamepadUISkillySkill
//-----------------------------------------------------------------------------
GamepadUISkillySkill::GamepadUISkillySkill(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const char *pText,
	const char *pDescription,
	const char *pImage,
	int nSkill )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription )
	, m_Image( pImage )
	, m_nSkill( nSkill )
	, m_flImageWidth( 40.0f )
	, m_flImageHeight( 40.0f )
	, m_flImageAnimationDuration( 0.2f )
	, m_flTitleBackgroundAnimationDuration( 0.2f )
	, m_flDescriptionBackgroundAnimationDuration( 0.2f )
{
	m_colImage = Color( 255, 255, 255, 255 );
	m_colTitleBackground = Color( 0, 0, 0, 0 );
	m_colDescriptionBackground = Color( 0, 0, 0, 0 );
	for ( int i = 0; i < ButtonState::Count; ++i )
	{
		m_colImages[i] = Color( 255, 255, 255, 255 );
		m_colTitleBackgrounds[i] = Color( 0, 0, 0, 0 );
		m_colDescriptionBackgrounds[i] = Color( 0, 0, 0, 0 );
	}

	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colImage, "Button.Image.Out", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colImages[ButtonState::Out], "Button.Image.Out", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colImages[ButtonState::Over], "Button.Image.Over", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colImages[ButtonState::Pressed], "Button.Image.Pressed", "255 255 255 255" );
	SCHEME_MEMBER_FLOAT( GamepadUISkillySkill, m_flImageAnimationDuration, "Button.Image.Animation.Duration", "0.2" );

	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colTitleBackground, "Button.Title.Background.Out", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colTitleBackgrounds[ButtonState::Out], "Button.Title.Background.Out", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colTitleBackgrounds[ButtonState::Over], "Button.Title.Background.Over", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colTitleBackgrounds[ButtonState::Pressed], "Button.Title.Background.Pressed", "0 0 0 0" );
	SCHEME_MEMBER_FLOAT( GamepadUISkillySkill, m_flTitleBackgroundAnimationDuration, "Button.Title.Background.Animation.Duration", "0.2" );

	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colDescriptionBackground, "Button.Description.Background.Out", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colDescriptionBackgrounds[ButtonState::Out], "Button.Description.Background.Out", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colDescriptionBackgrounds[ButtonState::Over], "Button.Description.Background.Over", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUISkillySkill, m_colDescriptionBackgrounds[ButtonState::Pressed], "Button.Description.Background.Pressed", "0 0 0 0" );
	SCHEME_MEMBER_FLOAT( GamepadUISkillySkill, m_flDescriptionBackgroundAnimationDuration, "Button.Description.Background.Animation.Duration", "0.2" );

	// Retail disables mouse-hover navigation on skill cards (0x0c70f7)
	m_bAllowMouseNav = false;
}

GamepadUISkillySkill::~GamepadUISkillySkill()
{
}

ButtonState::State GamepadUISkillySkill::GetCurrentButtonState()
{
	if ( _gamepadui_skill.GetInt() == m_nSkill )
		return ButtonState::Pressed;
	return BaseClass::GetCurrentButtonState();
}

void GamepadUISkillySkill::NavigateTo()
{
	BaseClass::NavigateTo();
	_gamepadui_skill.SetValue( m_nSkill );
}

void GamepadUISkillySkill::DoClick()
{
	BaseClass::DoClick();
	_gamepadui_skill.SetValue( m_nSkill );
}

void GamepadUISkillySkill::RunAnimations( ButtonState::State state )
{
	BaseClass::RunAnimations( state );

	vgui::AnimationController *pAnim = GamepadUI::GetInstance().GetAnimationController();
	if ( pAnim )
	{
		pAnim->RunAnimationCommand( this, "m_colTitleBackground", m_colTitleBackgrounds[state], 0.0f, m_flTitleBackgroundAnimationDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
		pAnim->RunAnimationCommand( this, "m_colDescriptionBackground", m_colDescriptionBackgrounds[state], 0.0f, m_flDescriptionBackgroundAnimationDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
		pAnim->RunAnimationCommand( this, "m_colImage", m_colImages[state], 0.0f, m_flImageAnimationDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	}
}

void GamepadUISkillySkill::Paint()
{
	PaintButton();

	ButtonState::State eState = GetCurrentButtonState();
	vgui::HFont hFont = ( eState != ButtonState::Out && m_hTextFontOver != vgui::INVALID_FONT ) ? m_hTextFontOver : m_hTextFont;

	int nTextW = 0, nTextH = 0;
	vgui::surface()->GetTextSize( hFont, m_strButtonText.String(), nTextW, nTextH );

	int nTitleBottom = (int)( m_flHeight * 0.5f + m_flTextOffsetY + (float)( nTextH / 2 ) );
	int nTitleTop = (int)( m_flHeight * 0.5f + m_flTextOffsetY - (float)( nTextH / 2 ) );
	int nTitleRight = (int)( m_flTextOffsetX * 2.0f + (float)nTextW );

	// Title background band
	vgui::surface()->DrawSetColor( m_colTitleBackground );
	vgui::surface()->DrawFilledRect( 0, nTitleTop, nTitleRight, nTitleBottom );

	// Description background band
	vgui::surface()->DrawSetColor( m_colDescriptionBackground );
	vgui::surface()->DrawFilledRect( 0, nTitleBottom, (int)m_flWidth, (int)( m_flHeight + m_flExtraHeight ) );

	// Centered skill icon image
	vgui::surface()->DrawSetColor( m_colImage );
	vgui::surface()->DrawSetTexture( m_Image.GetTextureId() );
	int nImgX0 = (int)( ( m_flWidth - m_flImageWidth ) * 0.5f );
	int nImgX1 = (int)( ( m_flWidth + m_flImageWidth ) * 0.5f );
	vgui::surface()->DrawTexturedRect( nImgX0, 0, nImgX1, (int)m_flImageHeight );
	vgui::surface()->DrawSetTexture( 0 );

	PaintText();
	PaintBorders();
}

//-----------------------------------------------------------------------------
// GamepadUIOptionsPanel
//-----------------------------------------------------------------------------
GamepadUIOptionsPanel::GamepadUIOptionsPanel( vgui::Panel *pParent, const char *pPanelName )
	: BaseClass( pParent, pPanelName )
	, m_flTabsOffsetX( 0.0f )
	, m_flTabsOffsetY( 0.0f )
	, m_flOptionsFade( 80.0f )
	, m_flScrollBarOffsetX( 10.0f )
	, m_flScrollBarWidth( 80.0f )
	, m_flScrollBarHeight( 80.0f )
	, m_nTabCount( 0 )
	, m_pResolutionButton( NULL )
{
	s_pOptionsPanel = this;

	vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile( "gamepadui/schemepanel.res", "SchemePanel" );
	SetScheme( hScheme );

	m_strTitle.SetString( "#GameUI_Options" );

	SetFooterButtons( FooterButtons::Back | FooterButtons::Apply, FooterButtons::None );
	Activate();

	if ( !s_bAAModesPopulated )
	{
		PopulateAAModes();
	}

	UpdateAllProxyConvars();
	BuildOptions( "gamepadui/options.res" );
	PopulateKeyBindings();
	ChangeTab( GetCurrentTab() );
	UpdateGradients();
}

GamepadUIOptionsPanel::~GamepadUIOptionsPanel()
{
	if ( s_pOptionsPanel == this )
	{
		s_pOptionsPanel = NULL;
	}
}

void GamepadUIOptionsPanel::UpdateGradients()
{
	float flTime = GamepadUI::GetInstance().GetTime();
	GamepadUIGradientHelper &gradients = GamepadUI::GetInstance().GetGradientHelper();

	gradients.ResetTargets( flTime );
	gradients.SetTargetGradient( GamepadUIGradientSides::Up, { 1.0f, 1.0f }, flTime );
	gradients.SetTargetGradient( GamepadUIGradientSides::Down, { 1.0f, 0.5f }, flTime );
}

int GamepadUIOptionsPanel::GetCurrentTab() const
{
	int nMaxTab = MAX( 0, m_nTabCount - 1 );
	int nCur = gamepadui_last_options_tab.GetInt();
	int nClamped = clamp( nCur, 0, nMaxTab );
	if ( nClamped != nCur )
	{
		gamepadui_last_options_tab.SetValue( nClamped );
	}
	return nClamped;
}

void GamepadUIOptionsPanel::PopulateAAModes()
{
	s_nAAModeCount = 0;
	s_bAAModesPopulated = true;

	auto AddAA = []( const char *pszToken, int nSamples, int nQuality )
	{
		if ( s_nAAModeCount >= MAX_AA_MODES )
			return;
		s_AAModes[s_nAAModeCount].m_strName.SetString( pszToken );
		s_AAModes[s_nAAModeCount].m_nNumSamples = nSamples;
		s_AAModes[s_nAAModeCount].m_nQualityLevel = nQuality;
		++s_nAAModeCount;
	};

	AddAA( "#GameUI_None", 1, 0 );

	if ( !g_pMaterialSystem )
		return;

	if ( g_pMaterialSystem->SupportsMSAAMode( 2 ) )
		AddAA( "#GameUI_2X", 2, 0 );
	if ( g_pMaterialSystem->SupportsMSAAMode( 4 ) )
		AddAA( "#GameUI_4X", 4, 0 );
	if ( g_pMaterialSystem->SupportsMSAAMode( 6 ) )
		AddAA( "#GameUI_6X", 6, 0 );
	if ( g_pMaterialSystem->SupportsCSAAMode( 4, 2 ) )
		AddAA( "#GameUI_8X_CSAA", 4, 2 );
	if ( g_pMaterialSystem->SupportsCSAAMode( 4, 4 ) )
		AddAA( "#GameUI_16X_CSAA", 4, 4 );
	if ( g_pMaterialSystem->SupportsMSAAMode( 8 ) )
		AddAA( "#GameUI_8X", 8, 0 );
	if ( g_pMaterialSystem->SupportsCSAAMode( 8, 2 ) )
		AddAA( "#GameUI_16XQ_CSAA", 8, 2 );
}

void GamepadUIOptionsPanel::UpdateAllProxyConvars()
{
	_gamepadui_water_detail.SetValue( GetWaterDetail() );
	_gamepadui_shader_detail.SetValue( GetShaderDetail() );
	_gamepadui_shadow_detail.SetValue( GetShadowDetail() );
	_gamepadui_classic_effects.SetValue( GetClassicEffects() );
	_gamepadui_antialiasing.SetValue( GetAntialiasing() );
	_gamepadui_aspectratio.SetValue( GetCurrentAspectRatioMode() );
	_gamepadui_displaymode.SetValue( GetDisplayMode() );
	_gamepadui_sound_quality.SetValue( GetSoundQuality() );
	_gamepadui_closecaptions.SetValue( GetCloseCaptions() );
	_gamepadui_hudaspect.SetValue( GetHudAspect() );
	_gamepadui_skill.SetValue( GetSkill() );
}

void GamepadUIOptionsPanel::ApplyAllProxyConvars()
{
	ApplyShaderDetail();
	ApplyWaterDetail();
	ApplyShadowDetail();
	ApplyAntialiasing();

	// Apply video mode (exact retail @ 0x0c23a0)
	if ( s_pOptionsPanel && s_pOptionsPanel->m_pResolutionButton && g_pMaterialSystem )
	{
		const GamepadUIWheelyWheel::Option_t *pOpt = s_pOptionsPanel->m_pResolutionButton->GetOption( _gamepadui_resolution.GetInt() );
		if ( pOpt )
		{
			const MaterialSystem_Config_t &config = g_pMaterialSystem->GetCurrentConfigForVideoCard();
			int nCurDisplayMode = GetDisplayMode();
			int nNewDisplayMode = _gamepadui_displaymode.GetInt();

			if ( config.m_VideoMode.m_Width != pOpt->m_nWidth ||
				 config.m_VideoMode.m_Height != pOpt->m_nHeight ||
				 nCurDisplayMode != nNewDisplayMode )
			{
				int nWindowed = ( nNewDisplayMode <= 1 ) ? 1 : 0;
				int nNoBorder = ( nNewDisplayMode == 1 ) ? 1 : 0;

#if defined( OSX ) && defined( PLATFORM_64BITS )
				if ( nNewDisplayMode > 1 )
				{
					ConVarRef sdl_displayindex( "sdl_displayindex" );
					if ( sdl_displayindex.IsValid() )
					{
						sdl_displayindex.SetValue( nNewDisplayMode - 2 );
					}
				}
#endif

				char szCmd[256];
				V_snprintf( szCmd, sizeof( szCmd ), "mat_setvideomode %i %i %i %i\n",
					pOpt->m_nWidth, pOpt->m_nHeight, nWindowed, nNoBorder );
				if ( GamepadUI::GetInstance().GetEngineClient() )
				{
					GamepadUI::GetInstance().GetEngineClient()->ClientCmd_Unrestricted( szCmd );
				}
			}
		}
	}

	ApplySoundQuality();
	ApplyCloseCaptions();
	ApplyHudAspect();
	ApplySkill();
	ApplyClassicEffects();
}

void GamepadUIOptionsPanel::PopulateResolutions()
{
	if ( !m_pResolutionButton || !g_pMaterialSystem )
		return;

	m_pResolutionButton->ClearOptions();

	const MaterialSystem_Config_t &config = g_pMaterialSystem->GetCurrentConfigForVideoCard();
	int nDesiredWidth = config.m_VideoMode.m_Width;
	int nDesiredHeight = config.m_VideoMode.m_Height;

	int nDesktopW = 0, nDesktopH = 0;
	IGameUIFuncs *pGameUIFuncs = GamepadUI::GetInstance().GetGameUIFuncs();
	if ( pGameUIFuncs )
	{
		pGameUIFuncs->GetDesktopResolution( nDesktopW, nDesktopH );
	}

	int nDisplayMode = _gamepadui_displaymode.GetInt();
	if ( nDisplayMode > 1 )
	{
		int nDisplayIndex = nDisplayMode - 2;
#if defined( USE_SDL )
		SDL_Rect bounds;
		if ( SDL_GetDisplayBounds( nDisplayIndex, &bounds ) == 0 )
		{
			nDesktopW = bounds.w;
			nDesktopH = bounds.h;
		}
#endif
		static ConVarRef sdl_displayindex_fullscreen( "sdl_displayindex_fullscreen" );
		if ( !sdl_displayindex_fullscreen.IsValid() || sdl_displayindex_fullscreen.GetInt() != nDisplayIndex )
		{
			nDesiredWidth = nDesktopW;
			nDesiredHeight = nDesktopH;
		}
	}

	vmode_t *pModes = NULL;
	int nModeCount = 0;
	if ( pGameUIFuncs )
	{
#if defined( OSX ) && defined( PLATFORM_64BITS )
		// The engine lists the modes of the sdl_displayindex display; preview the one picked here.
		static ConVarRef sdl_displayindex_preview( "sdl_displayindex" );
		int nSavedDisplayIndex = sdl_displayindex_preview.IsValid() ? sdl_displayindex_preview.GetInt() : 0;
		if ( sdl_displayindex_preview.IsValid() && nDisplayMode > 1 )
		{
			sdl_displayindex_preview.SetValue( nDisplayMode - 2 );
		}
		pGameUIFuncs->GetVideoModes( &pModes, &nModeCount );
		if ( sdl_displayindex_preview.IsValid() )
		{
			sdl_displayindex_preview.SetValue( nSavedDisplayIndex );
		}
#else
		pGameUIFuncs->GetVideoModes( &pModes, &nModeCount );
#endif
	}

	int nTargetAspect = _gamepadui_aspectratio.GetInt();
	int nSelectedIdx = -1;
	int nAdded = 0;

	for ( int i = 0; i < nModeCount; ++i )
	{
		int w = pModes[i].width;
		int h = pModes[i].height;

		if ( GetAspectRatioMode( w, h ) != nTargetAspect )
			continue;

		wchar_t wszLabel[1024];
		if ( w == nDesktopW && h == nDesktopH )
		{
			const wchar_t *pwszNative = g_pVGuiLocalize ? g_pVGuiLocalize->Find( "#GameUI_Native" ) : L"Native";
			V_snwprintf( wszLabel, ARRAYSIZE( wszLabel ), L"%i x %i (%ls)", w, h, pwszNative ? pwszNative : L"Native" );
		}
		else
		{
			V_snwprintf( wszLabel, ARRAYSIZE( wszLabel ), L"%i x %i", w, h );
		}

		if ( w == nDesiredWidth && h == nDesiredHeight )
		{
			nSelectedIdx = nAdded;
		}
		else if ( nSelectedIdx == -1 && w == config.m_VideoMode.m_Width && h == config.m_VideoMode.m_Height )
		{
			nSelectedIdx = nAdded;
		}

		m_pResolutionButton->AddOptionW( wszLabel, nAdded, w, h );
		++nAdded;
	}

	if ( nSelectedIdx == -1 )
	{
		nSelectedIdx = MAX( 0, m_pResolutionButton->GetOptionCount() - 1 );
	}

	_gamepadui_resolution.SetValue( nSelectedIdx );
	m_pResolutionButton->UpdateConvar();
}

void GamepadUIOptionsPanel::BuildOptions( const char *pszOptionsFile )
{
	KeyValues *pKV = new KeyValues( "Options" );
	if ( !GamepadUI_LoadKeyValues( pszOptionsFile, pKV ) )
	{
		pKV->deleteThis();
		return;
	}

	for ( KeyValues *pTabKV = pKV->GetFirstSubKey(); pTabKV != NULL && m_nTabCount < MAX_OPTION_TABS; pTabKV = pTabKV->GetNextKey() )
	{
		int nTabIdx = m_nTabCount;
		OptionTab_t &tab = m_Tabs[nTabIdx];

		char szTabCmd[64];
		V_snprintf( szTabCmd, sizeof( szTabCmd ), "tab %d", nTabIdx );

		const char *pszTitle = pTabKV->GetString( "title", "" );
		GamepadUIButton *pTabBtn = new GamepadUIButton( this, this, "gamepadui/schemetab.res", szTabCmd, pszTitle, "" );
		pTabBtn->SetZPos( 50 );
		if ( nTabIdx == gamepadui_last_options_tab.GetInt() )
		{
			pTabBtn->SetSelected( true );
		}

		tab.m_pTabButton = pTabBtn;
		tab.m_bAlternating = pTabKV->GetBool( "alternating", false );
		tab.m_bHorizontal = pTabKV->GetBool( "horizontal", false );

		KeyValues *pItemsKV = pTabKV->FindKey( "items" );
		if ( pItemsKV )
		{
			for ( KeyValues *pItemKV = pItemsKV->GetFirstSubKey(); pItemKV != NULL; pItemKV = pItemKV->GetNextKey() )
			{
				const char *pszType = pItemKV->GetString( "type", "droppydown" );
				const char *pszText = pItemKV->GetString( "text", "" );
				const char *pszDesc = pItemKV->GetString( "description", "" );

				if ( !V_strcmp( pszType, "checkybox" ) )
				{
					GamepadUICheckButton *pCheck = new GamepadUICheckButton(
						this, this, "gamepadui/schemeoptions_checkybox.res", "button_pressed", pszText, pszDesc );
					tab.m_Buttons.AddToTail( pCheck );
				}
				else if ( !V_strcmp( pszType, "skillyskill" ) )
				{
					const char *pszImage = pItemKV->GetString( "image", "" );
					int nSkill = V_atoi( pItemKV->GetString( "skill", "" ) );
					GamepadUISkillySkill *pSkill = new GamepadUISkillySkill(
						this, this, "gamepadui/schemeoptions_skillyskill.res", "button_pressed", pszText, pszDesc, pszImage, nSkill );
					tab.m_Buttons.AddToTail( pSkill );
				}
				else if ( !V_strcmp( pszType, "slideyslide" ) )
				{
					const char *pszConvar = pItemKV->GetString( "convar", "" );
					const char *pszDepends = pItemKV->GetString( "depends_on", "" );
					bool bInstant = pItemKV->GetBool( "instantapply", false );
					float flMin = pItemKV->GetFloat( "min", 0.0f );
					float flMax = pItemKV->GetFloat( "max", 1.0f );
					float flStep = pItemKV->GetFloat( "step", 0.1f );
					int nPrecision = pItemKV->GetInt( "textprecision", -1 );

					GamepadUISlideySlide *pSlide = new GamepadUISlideySlide(
						this, this, "gamepadui/schemeoptions_slideyslide.res", "button_pressed",
						pszText, pszDesc, pszConvar, pszDepends, bInstant, flMin, flMax, flStep, nPrecision );
					tab.m_Buttons.AddToTail( pSlide );
				}
				else if ( !V_strcmp( pszType, "headeryheader" ) )
				{
					GamepadUIButton *pHeader = new GamepadUIButton(
						this, this, "gamepadui/schemeoptions_sectiontitle.res", "button_pressed", pszText, pszDesc );
					pHeader->SetEnabled( false );
					tab.m_Buttons.AddToTail( pHeader );
				}
				else if ( !V_strcmp( pszType, "wheelywheel" ) )
				{
					const char *pszConvar = pItemKV->GetString( "convar", "" );
					const char *pszDepends = pItemKV->GetString( "depends_on", "" );
					bool bInstant = pItemKV->GetBool( "instantapply", false );
					bool bSignOnly = pItemKV->GetBool( "signonly", false );

					GamepadUIWheelyWheel *pWheel = new GamepadUIWheelyWheel(
						this, this, "gamepadui/schemeoptions_wheelywheel.res", "button_pressed",
						pszText, pszDesc, pszConvar, pszDepends, bInstant, bSignOnly );

					const char *pszOptionsFrom = pItemKV->GetString( "options_from", "" );
					KeyValues *pOptionsKV = pItemKV->FindKey( "options" );

					if ( pOptionsKV )
					{
						for ( KeyValues *pOptKV = pOptionsKV->GetFirstSubKey(); pOptKV != NULL; pOptKV = pOptKV->GetNextKey() )
						{
							int nOptVal = V_atoi( pOptKV->GetName() );
							const char *pszOptLabel = pOptKV->GetString( (const char *)NULL, "" );
							pWheel->AddOption( pszOptLabel, nOptVal );
						}
					}
					else if ( pszOptionsFrom && pszOptionsFrom[0] )
					{
						if ( !V_strcmp( pszOptionsFrom, "antialiasing" ) )
						{
							for ( int i = 0; i < s_nAAModeCount; ++i )
							{
								pWheel->AddOptionW( s_AAModes[i].m_strName.String(), i );
							}
						}
						else if ( !V_strcmp( pszOptionsFrom, "displaymode" ) )
						{
							GamepadUIString strWindowed( "#GameUI_Windowed" );
							pWheel->AddOptionW( strWindowed.String(), 0 );

							const wchar_t *pwszNoBorder = g_pVGuiLocalize ? g_pVGuiLocalize->Find( "#GameUI_NoBorder" ) : L"No Border";
							wchar_t wszNoBorder[256];
							V_snwprintf( wszNoBorder, ARRAYSIZE( wszNoBorder ), L"%ls (%ls)",
								strWindowed.String(), pwszNoBorder ? pwszNoBorder : L"No Border" );
							pWheel->AddOptionW( wszNoBorder, 1 );

							GamepadUIString strFullscreen( "#GameUI_Fullscreen" );
							int nDisplays = 1;
#if defined( USE_SDL )
							nDisplays = SDL_GetNumVideoDisplays();
#endif
							if ( nDisplays > 1 )
							{
								for ( int d = 0; d < nDisplays; ++d )
								{
									wchar_t wszDisp[256];
									V_snwprintf( wszDisp, ARRAYSIZE( wszDisp ), L"%ls (%d)", strFullscreen.String(), d );
									pWheel->AddOptionW( wszDisp, 2 + d );
								}
							}
							else
							{
								pWheel->AddOptionW( strFullscreen.String(), 2 );
							}
						}
						else if ( !V_strcmp( pszOptionsFrom, "resolutions" ) )
						{
							m_pResolutionButton = pWheel;
							PopulateResolutions();
						}
					}

					pWheel->UpdateConvar();
					tab.m_Buttons.AddToTail( pWheel );
				}
				else if ( !V_strcmp( pszType, "button" ) )
				{
					const char *pszCmd = pItemKV->GetString( "command", "button_pressed" );
					GamepadUIButton *pBtn = new GamepadUIButton(
						this, this, "gamepadui/schemeoptions_wheelywheel.res", pszCmd, pszText, pszDesc );
					tab.m_Buttons.AddToTail( pBtn );
				}
			}
		}

		// Dynamic keyboard bindings from scripts/kb_act.lst
		const char *pszItemsFrom = pTabKV->GetString( "items_from", "" );
		if ( !V_strcmp( pszItemsFrom, "keyboard" ) && g_pFullFileSystem )
		{
			CUtlBuffer buf( 0, 0, CUtlBuffer::TEXT_BUFFER );
			if ( g_pFullFileSystem->ReadFile( "scripts/kb_act.lst", NULL, buf ) )
			{
				const char *pCursor = (const char *)buf.Base();
				char szToken[512];
				char szAction[256];
				char szLabel[256];

				while ( pCursor )
				{
					pCursor = Options_ParseToken( pCursor, szToken, sizeof( szToken ) );
					if ( !szToken[0] )
						break;
					V_strncpy( szAction, szToken, sizeof( szAction ) );

					pCursor = Options_ParseToken( pCursor, szToken, sizeof( szToken ) );
					if ( !szToken[0] )
						break;
					V_strncpy( szLabel, szToken, sizeof( szLabel ) );

					// Retail skips divider lines starting with '='
					if ( szLabel[0] == '=' )
						continue;

					if ( !V_stricmp( szAction, "blank" ) )
					{
						GamepadUIButton *pSection = new GamepadUIButton(
							this, this, "gamepadui/schemeoptions_sectiontitle.res", "button_pressed", szLabel, "" );
						pSection->SetEnabled( false );
						tab.m_Buttons.AddToTail( pSection );
					}
					else
					{
						GamepadUIKeyButton *pKeyBtn = new GamepadUIKeyButton(
							this, this, "gamepadui/schemeoptions_wheelywheel.res", "button_pressed", szLabel, "", szAction );
						tab.m_Buttons.AddToTail( pKeyBtn );
					}
				}
				PopulateKeyBindings();
			}
		}

		// Wire up focus navigation links between buttons on this tab
		for ( int i = 1; i < tab.m_Buttons.Count(); ++i )
		{
			GamepadUIButton *pPrev = tab.m_Buttons[i - 1];
			GamepadUIButton *pCurr = tab.m_Buttons[i];
			if ( tab.m_bHorizontal )
			{
				pCurr->SetNavLeft( pPrev );
				pPrev->SetNavRight( pCurr );
			}
			else
			{
				pCurr->SetNavUp( pPrev );
				pPrev->SetNavDown( pCurr );
			}
		}

		++m_nTabCount;
	}

	pKV->deleteThis();
}

void GamepadUIOptionsPanel::PopulateKeyBindings()
{
	for ( int t = 0; t < m_nTabCount; ++t )
	{
		m_Tabs[t].m_BoundKeys.RemoveAll();
	}

	ConVarRef joystick( "joystick" );
	bool bJoystick = joystick.IsValid() && ( joystick.GetInt() != 0 );

	ConVarRef hap_HasDevice( "hap_HasDevice" );
	bool bHaptics = hap_HasDevice.IsValid() && ( hap_HasDevice.GetInt() != 0 );

	IGameUIFuncs *pFuncs = GamepadUI::GetInstance().GetGameUIFuncs();
	if ( !pFuncs || !g_pInputSystem )
		return;

	for ( int code = 0; code < BUTTON_CODE_LAST; ++code )
	{
		const char *pszBind = pFuncs->GetBindingForButtonCode( (ButtonCode_t)code );
		if ( !pszBind )
			continue;

		GamepadUIKeyButton *pMatch = NULL;
		int nMatchTab = 0;

		for ( int t = 0; t < m_nTabCount; ++t )
		{
			for ( int i = 0; i < m_Tabs[t].m_Buttons.Count(); ++i )
			{
				GamepadUIKeyButton *pKeyBtn = dynamic_cast<GamepadUIKeyButton *>( m_Tabs[t].m_Buttons[i] );
				if ( pKeyBtn && !V_strcmp( pKeyBtn->GetAction(), pszBind ) )
				{
					pMatch = pKeyBtn;
					nMatchTab = t;
					break;
				}
			}
		}

		if ( !pMatch )
			continue;

		const char *pszKeyName = g_pInputSystem->ButtonCodeToString( (ButtonCode_t)code );
		const char *pszExisting = pMatch->GetKey();

		if ( pszExisting && pszExisting[0] )
		{
			ButtonCode_t existingCode = pFuncs->GetButtonCodeForBind( pszExisting );
			bool bReplace = false;
			if ( bJoystick && IsJoystickCode( (ButtonCode_t)code ) && !IsJoystickCode( existingCode ) )
			{
				bReplace = true;
			}
			else if ( bHaptics && ( code >= NOVINT_FIRST && code <= NOVINT_LAST ) && !( existingCode >= NOVINT_FIRST && existingCode <= NOVINT_LAST ) )
			{
				bReplace = true;
			}

			if ( !bReplace )
				continue;

			CUtlSymbol oldSym( pszExisting );
			m_Tabs[nMatchTab].m_BoundKeys.FindAndRemove( oldSym );
		}

		pMatch->SetKey( pszKeyName );
		m_Tabs[nMatchTab].m_BoundKeys.AddToTail( CUtlSymbol( pszKeyName ) );
	}
}

void GamepadUIOptionsPanel::ClearDuplicateKeyBind( const char *pszKey )
{
	if ( !pszKey )
		return;

	for ( int t = 0; t < m_nTabCount; ++t )
	{
		for ( int i = 0; i < m_Tabs[t].m_Buttons.Count(); ++i )
		{
			GamepadUIKeyButton *pKeyBtn = dynamic_cast<GamepadUIKeyButton *>( m_Tabs[t].m_Buttons[i] );
			if ( pKeyBtn && !V_strcmp( pKeyBtn->GetKey(), pszKey ) )
			{
				pKeyBtn->SetKey( "" );
			}
		}
	}
}

void GamepadUIOptionsPanel::ApplyKeyBindings()
{
	IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient();
	if ( !pEngine || m_nTabCount <= 0 )
		return;

	int nTab = GetCurrentTab();
	OptionTab_t &tab = m_Tabs[nTab];

	char szCmd[256];
	for ( int i = 0; i < tab.m_BoundKeys.Count(); ++i )
	{
		V_snprintf( szCmd, sizeof( szCmd ), "unbind \"%s\"\n", tab.m_BoundKeys[i].String() );
		pEngine->ClientCmd_Unrestricted( szCmd );
	}
	tab.m_BoundKeys.RemoveAll();

	for ( int i = 0; i < tab.m_Buttons.Count(); ++i )
	{
		GamepadUIKeyButton *pKeyBtn = dynamic_cast<GamepadUIKeyButton *>( tab.m_Buttons[i] );
		if ( !pKeyBtn )
			continue;

		const char *pszKey = pKeyBtn->GetKey();
		if ( !pszKey || !pszKey[0] )
			continue;

		V_snprintf( szCmd, sizeof( szCmd ), "bind \"%s\" \"%s\"\n", pszKey, pKeyBtn->GetAction() );
		pEngine->ClientCmd_Unrestricted( szCmd );
	}
}

void GamepadUIOptionsPanel::ChangeTab( int nTab )
{
	if ( m_nTabCount <= 0 )
		return;

	int nClamped = clamp( nTab, 0, m_nTabCount - 1 );
	gamepadui_last_options_tab.SetValue( nClamped );
	int nActive = GetCurrentTab();

	for ( int i = 0; i < m_nTabCount; ++i )
	{
		if ( m_Tabs[i].m_pTabButton )
		{
			m_Tabs[i].m_pTabButton->SetSelected( i == nActive );
		}
	}

	OptionTab_t &tab = m_Tabs[nActive];
	GamepadUIButton *pFocusTarget = NULL;

	// Prefer a button already in Pressed or Over state
	for ( int i = 0; i < tab.m_Buttons.Count(); ++i )
	{
		ButtonState::State st = tab.m_Buttons[i]->GetCurrentButtonState();
		if ( st == ButtonState::Pressed || st == ButtonState::Over )
		{
			pFocusTarget = tab.m_Buttons[i];
			break;
		}
	}

	// Fallback to first enabled button
	if ( !pFocusTarget )
	{
		for ( int i = 0; i < tab.m_Buttons.Count(); ++i )
		{
			if ( tab.m_Buttons[i]->IsEnabled() )
			{
				pFocusTarget = tab.m_Buttons[i];
				break;
			}
		}
	}

	for ( int i = 0; i < tab.m_Buttons.Count(); ++i )
	{
		if ( tab.m_Buttons[i] != pFocusTarget )
		{
			tab.m_Buttons[i]->SetSelected( false );
		}
	}

	if ( pFocusTarget )
	{
		pFocusTarget->NavigateTo();
	}
}

void GamepadUIOptionsPanel::LayoutOptions()
{
	int nParentW = 0, nParentH = 0;
	if ( GetParent() )
		GetParent()->GetSize( nParentW, nParentH );
	else
		GetSize( nParentW, nParentH );

	int nTabX = (int)m_flTabsOffsetX;
	int nListStartY = 0;

	for ( int t = 0; t < m_nTabCount; ++t )
	{
		GamepadUIButton *pTabBtn = m_Tabs[t].m_pTabButton;
		if ( pTabBtn )
		{
			pTabBtn->SetPos( nTabX, (int)m_flTabsOffsetY );
			pTabBtn->SetVisible( true );
			nTabX += pTabBtn->GetWide();
			nListStartY = (int)( m_flTabsOffsetY + (float)pTabBtn->GetTall() );
		}

		for ( int i = 0; i < m_Tabs[t].m_Buttons.Count(); ++i )
		{
			m_Tabs[t].m_Buttons[i]->SetVisible( false );
		}
	}

	if ( m_nTabCount <= 0 )
		return;

	int nActiveTab = GetCurrentTab();
	OptionTab_t &tab = m_Tabs[nActiveTab];

	int nMaxScroll = 0;
	int nScrollableCount = tab.m_Buttons.Count() - 8;
	for ( int i = 0; i < nScrollableCount; ++i )
	{
		nMaxScroll += tab.m_Buttons[i]->GetTall();
	}

	tab.m_ScrollState.UpdateScrollBounds( 0.0f, (float)nMaxScroll );

	int nOffset = 0;
	float flBottomLimit = (float)nParentH - m_flFooterButtonsOffsetY - (float)m_nFooterButtonHeight;

	for ( int i = 0; i < tab.m_Buttons.Count(); ++i )
	{
		GamepadUIButton *pBtn = tab.m_Buttons[i];
		int nX = (int)m_flTabsOffsetX;
		int nY = nListStartY;
		int nAlpha = 255;

		if ( tab.m_bHorizontal )
		{
			nX += nOffset;
		}
		else
		{
			nY = (int)( (float)( nListStartY + nOffset ) - tab.m_ScrollState.GetScrollProgress() );

			if ( nY < nListStartY )
			{
				float flBtnTall = (float)pBtn->GetTall();
				float flDenom = flBtnTall - m_flOptionsFade;
				if ( flDenom == 0.0f )
				{
					nAlpha = ( (float)( nListStartY - nY ) <= 0.0f ) ? 255 : 0;
				}
				else
				{
					float flFrac = clamp( ( (float)( nListStartY - nY ) + flDenom ) / flDenom, 0.0f, 1.0f );
					nAlpha = (int)( flFrac * 255.0f );
				}
			}
			else if ( (float)nY > flBottomLimit - m_flOptionsFade )
			{
				if ( m_flOptionsFade == 0.0f )
				{
					nAlpha = ( flBottomLimit - (float)( nY + pBtn->GetTall() ) >= 0.0f ) ? 255 : 0;
				}
				else
				{
					float flFrac = clamp( ( flBottomLimit - (float)( nY + pBtn->GetTall() ) ) / m_flOptionsFade, 0.0f, 1.0f );
					nAlpha = (int)( flFrac * 255.0f );
				}
			}

			if ( pBtn->HasFocus() && pBtn->IsEnabled() && nAlpha != 0 )
			{
				nAlpha = 255;
			}
		}

		// Evaluate depends_on visibility for ConVar-bound options
		bool bVisible = true;
		GamepadUIConvarButton *pCvarBtn = dynamic_cast<GamepadUIConvarButton *>( pBtn );
		if ( pCvarBtn )
		{
			const char *pszDep = pCvarBtn->GetDependsOn();
			if ( pszDep && pszDep[0] )
			{
				bool bFoundDep = false;
				for ( int j = 0; j < tab.m_Buttons.Count(); ++j )
				{
					GamepadUIConvarButton *pOther = dynamic_cast<GamepadUIConvarButton *>( tab.m_Buttons[j] );
					if ( pOther && pOther->GetConvarName()[0] && !V_strcmp( pszDep, pOther->GetConvarName() ) )
					{
						bVisible = pOther->IsTruthy();
						bFoundDep = true;
						break;
					}
				}
				if ( !bFoundDep )
				{
					ConVarRef depRef( pszDep );
					bVisible = ( depRef.GetInt() != 0 );
				}
			}
		}

		pBtn->SetAlpha( nAlpha );
		pBtn->SetVisible( bVisible );
		pBtn->SetPos( nX, nY );

		if ( tab.m_bHorizontal )
			nOffset += pBtn->GetWide();
		else
			nOffset += pBtn->GetTall();
	}

	tab.m_ScrollState.UpdateScroll( GamepadUI::GetInstance().GetTime(), 0.5f );
}

void GamepadUIOptionsPanel::PaintOptionsDecorations()
{
	if ( m_nTabCount <= 0 )
		return;

	int nParentW = 0, nParentH = 0;
	if ( GetParent() )
		GetParent()->GetSize( nParentW, nParentH );
	else
		GetSize( nParentW, nParentH );

	int nListStartY = 0;
	for ( int t = 0; t < m_nTabCount; ++t )
	{
		if ( GamepadUIButton *pTabBtn = m_Tabs[t].m_pTabButton )
		{
			nListStartY = (int)( m_flTabsOffsetY + (float)pTabBtn->GetTall() );
		}
	}

	int nActiveTab = GetCurrentTab();
	OptionTab_t &tab = m_Tabs[nActiveTab];

	// Alternating row background shading (matches retail @ 0xc4d20..0xc4fa1, drawn inside Paint())
	if ( tab.m_bAlternating )
	{
		for ( int i = 0; i < tab.m_Buttons.Count(); ++i )
		{
			GamepadUIButton *pBtn = tab.m_Buttons[i];
			int nAlpha = pBtn->GetAlpha();
			if ( pBtn->IsEnabled() && pBtn->IsVisible() && nAlpha > 0 )
			{
				int nX = 0, nY = 0;
				pBtn->GetPos( nX, nY );
				int nBtnW = pBtn->GetWide();
				int nBtnH = pBtn->GetTall();
				if ( ( i & 1 ) == 0 )
				{
					int a = ( nAlpha > 64 ) ? 1 : 0;
					vgui::surface()->DrawSetColor( nAlpha, nAlpha, nAlpha, a );
				}
				else
				{
					int nShade = MIN( 255, nAlpha + 127 );
					int a = ( nShade * 20 ) / 255;
					vgui::surface()->DrawSetColor( 0, 0, 0, a );
				}
				vgui::surface()->DrawFilledRect( nX, nY, nX + nBtnW, nY + nBtnH );
			}
		}
	}

	// Draw scrollbar thumb when tab content overflows (matches retail @ 0xc4b1f..0xc4c14)
	int nMaxScroll = 0;
	int nScrollableCount = tab.m_Buttons.Count() - 8;
	for ( int i = 0; i < nScrollableCount; ++i )
	{
		nMaxScroll += tab.m_Buttons[i]->GetTall();
	}

	if ( nMaxScroll > 0 )
	{
		float flBottomLimit = (float)nParentH - m_flFooterButtonsOffsetY - (float)m_nFooterButtonHeight;
		vgui::surface()->DrawSetColor( 255, 255, 255, 200 );
		float flTrackTop = (float)nListStartY;
		float flTrackSpan = ( flBottomLimit - m_flScrollBarHeight ) - flTrackTop;
		float flScrollFrac = clamp( tab.m_ScrollState.GetScrollProgress() / (float)nMaxScroll, 0.0f, 1.0f );
		int nThumbY = (int)( flTrackTop + flScrollFrac * flTrackSpan );
		int nThumbX = (int)( m_flTabsOffsetX + m_flScrollBarOffsetX );
		vgui::surface()->DrawFilledRect(
			nThumbX,
			nThumbY,
			(int)( (float)nThumbX + m_flScrollBarWidth ),
			(int)( (float)nThumbY + m_flScrollBarHeight ) );
	}
}

void GamepadUIOptionsPanel::Paint()
{
	BaseClass::Paint();
	PaintOptionsDecorations();

	if ( m_nTabCount <= 0 || !m_Tabs[0].m_pTabButton )
		return;

	int nTabW = m_Tabs[0].m_pTabButton->GetWide();
	int nTabH = m_Tabs[0].m_pTabButton->GetTall();
	int nGlyphSize = (int)( (float)nTabH * 0.9f );
	int nGlyphMargin = (int)( (float)nGlyphSize * 0.25f );
	int nGlyphY = (int)( m_flTabsOffsetY + (float)( ( nTabH - nGlyphSize ) / 2 ) );

	if ( m_GlyphLB.SetupGlyph( nGlyphSize, "menu_lb", true ) )
	{
		int nGlyphX = (int)( m_flTabsOffsetX - (float)nGlyphSize - (float)nGlyphMargin );
		vgui::surface()->DrawSetColor( 255, 255, 255, 255 );
		vgui::surface()->DrawSetTexture( m_GlyphLB.GetTextureID( 0 ) );
		vgui::surface()->DrawTexturedRect( nGlyphX, nGlyphY, nGlyphX + nGlyphSize, nGlyphY + nGlyphSize );
		vgui::surface()->DrawSetTexture( 0 );
	}

	if ( m_GlyphRB.SetupGlyph( nGlyphSize, "menu_rb", true ) )
	{
		int nGlyphX = (int)( m_flTabsOffsetX + (float)( m_nTabCount * nTabW ) ) + nGlyphMargin;
		vgui::surface()->DrawSetColor( 255, 255, 255, 255 );
		vgui::surface()->DrawSetTexture( m_GlyphRB.GetTextureID( 0 ) );
		vgui::surface()->DrawTexturedRect( nGlyphX, nGlyphY, nGlyphX + nGlyphSize, nGlyphY + nGlyphSize );
		vgui::surface()->DrawSetTexture( 0 );
	}
}

void GamepadUIOptionsPanel::OnThink()
{
	BaseClass::OnThink();
	LayoutOptions();
}

void GamepadUIOptionsPanel::OnMouseWheeled( int nDelta )
{
	if ( m_nTabCount <= 0 )
		return;

	int nActiveTab = GetCurrentTab();
	m_Tabs[nActiveTab].m_ScrollState.OnMouseWheeled( nDelta, GamepadUI::GetInstance().GetTime(), 100.0f );
}

void GamepadUIOptionsPanel::OnKeyCodePressed( vgui::KeyCode code )
{
	ButtonCode_t baseCode = GamepadUI_NormalizeJoystickCode( code );

	bool bPrevTab = ( baseCode == KEY_XBUTTON_LEFT_SHOULDER || baseCode == STEAMCONTROLLER_LEFT_BUMPER );
	if ( bPrevTab )
	{
		ChangeTab( GetCurrentTab() - 1 );
		return;
	}

	bool bNextTab = ( baseCode == KEY_XBUTTON_RIGHT_SHOULDER || baseCode == STEAMCONTROLLER_RIGHT_BUMPER );
	if ( bNextTab )
	{
		ChangeTab( GetCurrentTab() + 1 );
		return;
	}

	BaseClass::OnKeyCodePressed( code );
}

void GamepadUIOptionsPanel::OnCommand( const char *pCommand )
{
	if ( !V_strcmp( pCommand, "action_back" ) )
	{
		Close();
		return;
	}

	if ( !V_strcmp( pCommand, "action_apply" ) )
	{
		if ( m_nTabCount > 0 )
		{
			OptionTab_t &tab = m_Tabs[GetCurrentTab()];
			for ( int i = 0; i < tab.m_Buttons.Count(); ++i )
			{
				GamepadUIConvarButton *pCvarBtn = dynamic_cast<GamepadUIConvarButton *>( tab.m_Buttons[i] );
				if ( pCvarBtn )
				{
					pCvarBtn->ApplyConvar();
				}
			}
		}

		ApplyAllProxyConvars();
		ApplyKeyBindings();

		if ( GamepadUI::GetInstance().GetEngineClient() )
		{
			GamepadUI::GetInstance().GetEngineClient()->ClientCmd_Unrestricted(
				"exec userconfig.cfg\nhost_writeconfig\nmat_savechanges\n" );
		}
		return;
	}

	if ( StringHasPrefix( pCommand, "tab " ) )
	{
		if ( pCommand[4] != '\0' )
		{
			int nTab = (int)strtol( pCommand + 4, NULL, 10 );
			ChangeTab( nTab );
		}
		return;
	}

	if ( !V_strcmp( pCommand, "open_steaminput" ) )
	{
#if !defined( NO_STEAM )
		if ( SteamInput() )
		{
			InputHandle_t handle = g_pInputSystem ? (InputHandle_t)g_pInputSystem->GetActiveSteamInputHandle() : 0;
			if ( handle == 0 )
			{
				InputHandle_t handles[STEAM_INPUT_MAX_COUNT];
				V_memset( handles, 0, sizeof( handles ) );
				if ( SteamInput()->GetConnectedControllers( handles ) > 0 )
				{
					handle = handles[0];
				}
			}
			if ( handle != 0 )
			{
				SteamInput()->ShowBindingPanel( handle );
			}
		}
#endif
		return;
	}

	if ( !V_strcmp( pCommand, "open_techcredits" ) )
	{
		GamepadUIString strTitle( "#GameUI_ThirdPartyTechCredits" );
		GamepadUIString strBink( "#GameUI_Bink" );
		GamepadUIString strMilesAudio( "#GameUI_Miles_Audio" );
		GamepadUIString strMilesVoice( "#GameUI_Miles_Voice" );

		wchar_t wszBody[4096];
		V_snwprintf( wszBody, ARRAYSIZE( wszBody ), L"%ls\n\n%ls\n\n%ls",
			strBink.String(), strMilesAudio.String(), strMilesVoice.String() );

		new GamepadUIGenericConfirmationPanel(
			GamepadUI::GetInstance().GetBasePanel(),
			"TechCredits",
			strTitle.String(),
			wszBody,
			[]() {},
			true,
			false );
		return;
	}

	BaseClass::OnCommand( pCommand );
}

void GamepadUIOptionsPanel::OnGamepadUIButtonNavigatedTo( int button )
{
	vgui::VPANEL hVPanel = vgui::ivgui()->HandleToPanel( (vgui::HPanel)button );
	vgui::Panel *pPanel = vgui::ipanel()->GetPanel( hVPanel, GetModuleName() );
	if ( !pPanel )
		return;

	GamepadUIButton *pBtn = dynamic_cast<GamepadUIButton *>( pPanel );
	if ( !pBtn || m_nTabCount <= 0 )
		return;

	int nParentW = 0, nParentH = 0;
	if ( GetParent() )
		GetParent()->GetSize( nParentW, nParentH );
	else
		GetSize( nParentW, nParentH );

	int nActiveTab = GetCurrentTab();
	OptionTab_t &tab = m_Tabs[nActiveTab];
	float flTabHeight = tab.m_pTabButton ? tab.m_pTabButton->GetCurrentHeight() : 0.0f;

	int nBtnX = 0, nBtnY = 0;
	pBtn->GetPos( nBtnX, nBtnY );
	int nBtnTall = pBtn->GetTall();

	float flBottomEdge = (float)nBtnY + (float)nBtnTall + m_flFooterButtonsOffsetY + (float)m_nFooterButtonHeight;
	float flTopEdge = m_flTabsOffsetY + flTabHeight;

	if ( flBottomEdge <= (float)nParentH && (float)nBtnY >= flTopEdge )
		return;

	int nAccumY = 0;
	for ( int i = 0; i < tab.m_Buttons.Count(); ++i )
	{
		if ( tab.m_Buttons[i] == pBtn )
			break;
		nAccumY += (int)tab.m_Buttons[i]->GetCurrentHeight();
	}

	// Retail special case: if navigating to the 2nd item on a tab, snap top to 0
	if ( tab.m_Buttons.Count() > 1 && tab.m_Buttons[1] == pBtn )
	{
		nAccumY = 0;
	}

	float flHalfBtn = pBtn->GetCurrentHeight() * 0.5f;
	int nNewTarget = 0;
	if ( nBtnY < nParentH / 2 )
	{
		nNewTarget = (int)( (float)nAccumY - flHalfBtn );
	}
	else
	{
		float flScrollOffset = (float)m_nFooterButtonHeight - (float)nParentH + ( m_flTabsOffsetY + m_flFooterButtonsOffsetY ) + flTabHeight + (float)nAccumY;
		nNewTarget = (int)( (float)( (int)flScrollOffset ) + pBtn->GetCurrentHeight() + flHalfBtn );
	}

	tab.m_ScrollState.SetScrollTarget( (float)nNewTarget, GamepadUI::GetInstance().GetTime() );
}

//-----------------------------------------------------------------------------
// Console command to open Options panel
//-----------------------------------------------------------------------------
CON_COMMAND( gamepadui_openoptionsdialog, "" )
{
	new GamepadUIOptionsPanel( GamepadUI::GetInstance().GetBasePanel(), "" );
}
