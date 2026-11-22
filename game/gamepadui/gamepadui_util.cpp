//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Core utility implementations for HL2 Anniversary GamepadUI.
//
//===========================================================================//

#include "gamepadui_util.h"
#include "gamepadui_interface.h"

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#include "tier0/icommandline.h"
#include "tier1/strtools.h"
#include "tier2/tier2.h"
#include "tier3/tier3.h"
#include "filesystem.h"
#include "inputsystem/iinputsystem.h"
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui/IScheme.h"
#include "vgui/IVGui.h"
#include "vgui_controls/Controls.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "bitmap/bitmap.h"
#include "imageutils.h"

#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// SchemeValueMap
//-----------------------------------------------------------------------------
void SchemeValueMap::ApplySchemeValues( vgui::Panel *pPanel, vgui::IScheme *pScheme )
{
	if ( !pPanel || !pScheme )
		return;

	for ( int i = 0; i < m_SchemeValues.Count(); ++i )
	{
		const SchemeValue_t &entry = m_SchemeValues[i];
		if ( !entry.m_pfnGetMember )
			continue;

		const char *pszVal = pScheme->GetResourceString( entry.m_pszSchemeKey );

		switch ( entry.m_eType )
		{
		case SchemeValueType_t::Float:
		case SchemeValueType_t::ProportionalFloat:
			{
				float flValue = 0.0f;
				if ( pszVal && pszVal[0] )
				{
					flValue = static_cast<float>( strtod( pszVal, NULL ) );
				}
				else if ( entry.m_pszDefaultValue && entry.m_pszDefaultValue[0] )
				{
					flValue = static_cast<float>( strtod( entry.m_pszDefaultValue, NULL ) );
				}

				if ( entry.m_eType == SchemeValueType_t::ProportionalFloat && vgui::scheme() )
				{
					flValue = static_cast<float>( vgui::scheme()->GetProportionalScaledValueEx( pPanel->GetScheme(), static_cast<int>( flValue ) ) );
				}

				*static_cast<float *>( entry.m_pfnGetMember( pPanel ) ) = flValue;
			}
			break;

		case SchemeValueType_t::Bool:
			{
				bool bValue = false;
				if ( pszVal && pszVal[0] )
				{
					bValue = ( strtol( pszVal, NULL, 10 ) != 0 );
				}
				else if ( entry.m_pszDefaultValue && entry.m_pszDefaultValue[0] )
				{
					bValue = ( strtol( entry.m_pszDefaultValue, NULL, 10 ) != 0 );
				}
				*static_cast<bool *>( entry.m_pfnGetMember( pPanel ) ) = bValue;
			}
			break;

		case SchemeValueType_t::Color:
			{
				Color defaultColor( 0, 0, 0, 0 );
				if ( entry.m_pszDefaultValue && entry.m_pszDefaultValue[0] )
				{
					float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
					sscanf( entry.m_pszDefaultValue, "%f %f %f %f", &r, &g, &b, &a );
					defaultColor.SetColor(
						static_cast<int>( r ),
						static_cast<int>( g ),
						static_cast<int>( b ),
						static_cast<int>( a ) );
				}
				Color col = pScheme->GetColor( entry.m_pszSchemeKey, defaultColor );
				*static_cast<Color *>( entry.m_pfnGetMember( pPanel ) ) = col;
			}
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// GamepadUIGlyph
//-----------------------------------------------------------------------------
GamepadUIGlyph::GamepadUIGlyph()
	: m_eOrigin( 0 )
{
	m_nTextureID[0] = -1;
	m_nTextureID[1] = -1;
}

GamepadUIGlyph::~GamepadUIGlyph()
{
	Cleanup();
}

void GamepadUIGlyph::Cleanup()
{
	if ( vgui::surface() )
	{
		for ( int i = 0; i < 2; ++i )
		{
			if ( m_nTextureID[i] > 0 )
			{
				vgui::surface()->DestroyTextureID( m_nTextureID[i] );
			}
			m_nTextureID[i] = -1;
		}
	}
	else
	{
		m_nTextureID[0] = -1;
		m_nTextureID[1] = -1;
	}
	m_eOrigin = 0;
}

bool GamepadUIGlyph::SetupGlyph( int nSize, const char *pszAction, bool bLight )
{
#if !defined( NO_STEAM )
	ISteamInput *pSteamInput = SteamInput();
	if ( !pSteamInput || !pszAction || !pszAction[0] )
		return false;

	InputHandle_t handles[STEAM_INPUT_MAX_COUNT];
	V_memset( handles, 0, sizeof( handles ) );
	pSteamInput->GetConnectedControllers( handles );

	InputHandle_t hController = 0;
	if ( g_pInputSystem )
	{
		hController = static_cast<InputHandle_t>( g_pInputSystem->GetActiveSteamInputHandle() );
	}
	if ( hController == 0 )
	{
		hController = handles[0];
	}
	if ( hController == 0 )
		return false;

	InputActionSetHandle_t hActionSet = pSteamInput->GetCurrentActionSet( hController );
	InputDigitalActionHandle_t hAction = pSteamInput->GetDigitalActionHandle( pszAction );
	if ( hAction == 0 )
	{
		Cleanup();
		return false;
	}

	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	V_memset( origins, 0, sizeof( origins ) );
	int nOrigins = pSteamInput->GetDigitalActionOrigins( hController, hActionSet, hAction, origins );
	if ( nOrigins == 0 || origins[0] == k_EInputActionOrigin_None )
	{
		Cleanup();
		return false;
	}

	if ( static_cast<int>( origins[0] ) == m_eOrigin )
	{
		return m_nTextureID[0] > 0 && m_nTextureID[1] > 0;
	}

	Cleanup();
	m_eOrigin = static_cast<int>( origins[0] );

	if ( nSize <= 0 )
		nSize = 32;

	if ( ( nSize & ( nSize - 1 ) ) != 0 )
	{
		nSize = GamepadUI_NextPowerOfTwo( nSize );
	}

	ESteamInputGlyphSize eGlyphSize = k_ESteamInputGlyphSize_Small;
	if ( nSize > 32 )
	{
		eGlyphSize = ( nSize > 128 ) ? k_ESteamInputGlyphSize_Large : k_ESteamInputGlyphSize_Medium;
	}

	uint32 unStyles[2] =
	{
		bLight ? static_cast<uint32>( ESteamInputGlyphStyle_Light ) : static_cast<uint32>( ESteamInputGlyphStyle_Knockout ),
		static_cast<uint32>( ESteamInputGlyphStyle_Dark ),
	};

	IMatSystemSurface *pMatSurface = GamepadUI::GetInstance().GetMatSystemSurface();
	if ( !vgui::surface() || !pMatSurface )
	{
		Cleanup();
		return false;
	}

	for ( int i = 0; i < 2; ++i )
	{
		const char *pszGlyphPath = pSteamInput->GetGlyphPNGForActionOrigin( origins[0], eGlyphSize, unStyles[i] );
		if ( !pszGlyphPath || !pszGlyphPath[0] )
		{
			Cleanup();
			return false;
		}

		Bitmap_t bitmap;
		if ( ImgUtl_LoadBitmap( pszGlyphPath, bitmap ) != CE_SUCCESS )
		{
			Cleanup();
			return false;
		}

		ImgUtl_ResizeBitmap( bitmap, nSize, nSize, &bitmap );

		m_nTextureID[i] = vgui::surface()->CreateNewTextureID( true );
		if ( m_nTextureID[i] <= 0 )
		{
			Cleanup();
			return false;
		}

		pMatSurface->DrawSetTextureRGBAEx2(
			m_nTextureID[i],
			bitmap.GetBits(),
			bitmap.Width(),
			bitmap.Height(),
			IMAGE_FORMAT_RGBA8888,
			true,
			false );
	}

	return true;
#else
	(void)nSize;
	(void)pszAction;
	(void)bLight;
	return false;
#endif
}

void GamepadUIGlyph::PaintGlyph( int nX, int nY, int nSize, float flFade ) const
{
	if ( !vgui::surface() )
		return;

	int nAlpha0 = static_cast<int>( ( 1.0f - flFade ) * 255.0f );
	int nAlpha1 = 255 - nAlpha0;

	if ( nAlpha0 > 0 && m_nTextureID[0] > 0 )
	{
		vgui::surface()->DrawSetColor( Color( 255, 255, 255, nAlpha0 ) );
		vgui::surface()->DrawSetTexture( m_nTextureID[0] );
		vgui::surface()->DrawTexturedRect( nX, nY, nX + nSize, nY + nSize );
	}

	if ( nAlpha1 > 0 && m_nTextureID[1] > 0 )
	{
		vgui::surface()->DrawSetColor( Color( 255, 255, 255, nAlpha1 ) );
		vgui::surface()->DrawSetTexture( m_nTextureID[1] );
		vgui::surface()->DrawTexturedRect( nX, nY, nX + nSize, nY + nSize );
	}

	vgui::surface()->DrawSetTexture( 0 );
}

//-----------------------------------------------------------------------------
// GamepadUIGradientHelper
//-----------------------------------------------------------------------------
void GamepadUIGradientHelper::PaintGradients( int nWide, int nTall, float flTime ) const
{
	if ( !vgui::surface() )
		return;

	vgui::surface()->DrawSetColor( Color( 0, 0, 0, 255 ) );

	GamepadUIGradientState left = GetCurrentGradient( GamepadUIGradientSides::Left, flTime );
	GamepadUIGradientState right = GetCurrentGradient( GamepadUIGradientSides::Right, flTime );
	GamepadUIGradientState up = GetCurrentGradient( GamepadUIGradientSides::Up, flTime );
	GamepadUIGradientState down = GetCurrentGradient( GamepadUIGradientSides::Down, flTime );

	if ( left.flAmount != 0.0f && left.flExtent != 0.0f )
	{
		unsigned int nAlpha = static_cast<unsigned int>( clamp( left.flAmount, 0.0f, 1.0f ) * 255.0f );
		vgui::surface()->DrawFilledRectFade( 0, 0, static_cast<int>( nWide * left.flExtent ), nTall, nAlpha, 0, true );
	}

	if ( right.flAmount != 0.0f && right.flExtent != 0.0f )
	{
		unsigned int nAlpha = static_cast<unsigned int>( clamp( right.flAmount, 0.0f, 1.0f ) * 255.0f );
		vgui::surface()->DrawFilledRectFade( 0, 0, static_cast<int>( nWide * right.flExtent ), nTall, 0, nAlpha, true );
	}

	if ( down.flAmount != 0.0f && down.flExtent != 0.0f )
	{
		unsigned int nAlpha = static_cast<unsigned int>( clamp( down.flAmount, 0.0f, 1.0f ) * 255.0f );
		vgui::surface()->DrawFilledRectFade( 0, 0, nWide, static_cast<int>( nTall * down.flExtent ), nAlpha, 0, false );
	}

	if ( up.flAmount != 0.0f && up.flExtent != 0.0f )
	{
		unsigned int nAlpha = static_cast<unsigned int>( clamp( up.flAmount, 0.0f, 1.0f ) * 255.0f );
		vgui::surface()->DrawFilledRectFade( 0, 0, nWide, static_cast<int>( nTall * up.flExtent ), 0, nAlpha, false );
	}
}

//-----------------------------------------------------------------------------
// Free utility functions
//-----------------------------------------------------------------------------
bool GamepadUI_LoadKeyValues( const char *pszFileName, KeyValues *pKV )
{
	if ( !pszFileName || !pKV || !g_pFullFileSystem )
		return false;

	if ( g_pFullFileSystem->FileExists( pszFileName, "ROOT_MOD" ) )
	{
		if ( pKV->LoadFromFile( g_pFullFileSystem, pszFileName, "ROOT_MOD" ) )
			return true;
	}

	return pKV->LoadFromFile( g_pFullFileSystem, pszFileName, NULL );
}

const char *GamepadUI_GetModName()
{
	static char s_szModName[MAX_PATH] = "";
	if ( s_szModName[0] != '\0' )
		return s_szModName;

	const char *pszDefaultDir = CommandLine()->ParmValue( "-defaultgamedir", "hl2" );
	const char *pszGame = CommandLine()->ParmValue( "-game", pszDefaultDir );

	char szExpansion[16] = "";
	if ( g_pFullFileSystem && g_pFullFileSystem->GetCurrentGameExpansionId( szExpansion, sizeof( szExpansion ) ) )
	{
		pszGame = szExpansion;
	}

	V_strncpy( s_szModName, pszGame, sizeof( s_szModName ) );

	if ( strchr( s_szModName, '/' ) || strchr( s_szModName, '\\' ) )
	{
		V_StripLastDir( s_szModName, sizeof( s_szModName ) );
		size_t nPrefixLen = strlen( s_szModName );
		V_strncpy( s_szModName, pszGame + nPrefixLen, sizeof( s_szModName ) - nPrefixLen );
	}

	return s_szModName;
}

int GamepadUI_DrawWrappedText( vgui::HFont hFont, int nX, int nY, const wchar_t *pwszText, int nLen, int nMaxWidth, bool bDraw )
{
	if ( !vgui::surface() || !pwszText || nLen <= 0 )
		return 0;

	int nFontTall = vgui::surface()->GetFontTall( hFont );
	const wchar_t *pEnd = pwszText + nLen;
	const wchar_t *pLineStart = pwszText;
	const wchar_t *pLastSpace = NULL;
	const wchar_t *pCur = pwszText;

	int nTotalHeight = 0;
	int nCurY = nY;
	float flLineWidth = 0.0f;
	float flMaxWidth = static_cast<float>( nMaxWidth );

	while ( pCur < pEnd && *pCur != L'\0' )
	{
		wchar_t ch = *pCur;
		bool bNewline = ( ch == L'\n' );

		if ( ch == L' ' || bNewline )
		{
			pLastSpace = pCur;
		}

		if ( !bNewline )
		{
			flLineWidth += static_cast<float>( vgui::surface()->GetCharacterWidth( hFont, ch ) );
		}

		if ( bNewline || flLineWidth >= flMaxWidth )
		{
			const wchar_t *pBreak = pLastSpace ? pLastSpace : pCur;

			if ( bDraw && pBreak > pLineStart )
			{
				vgui::surface()->DrawSetTextPos( nX, nCurY );
				vgui::surface()->DrawPrintText( pLineStart, static_cast<int>( pBreak - pLineStart ) );
			}

			pLineStart = pBreak + 1;
			pCur = ( *pBreak == L'\n' || *pBreak == L' ' ) ? ( pBreak + 1 ) : pBreak;
			pLastSpace = NULL;
			flLineWidth = 0.0f;
			nCurY += nFontTall;
			nTotalHeight += nFontTall;
			continue;
		}

		++pCur;
	}

	if ( pEnd > pLineStart )
	{
		if ( bDraw )
		{
			vgui::surface()->DrawSetTextPos( nX, nCurY );
			vgui::surface()->DrawPrintText( pLineStart, static_cast<int>( pEnd - pLineStart ) );
		}
		nTotalHeight += nFontTall;
	}

	return nTotalHeight;
}

int GamepadUI_NextPowerOfTwo( int nValue )
{
	if ( nValue <= 1 )
		return 1;

	--nValue;
	nValue |= nValue >> 1;
	nValue |= nValue >> 2;
	nValue |= nValue >> 4;
	nValue |= nValue >> 8;
	nValue |= nValue >> 16;
	return nValue + 1;
}

float GamepadUI_ProportionalScale( vgui::HScheme hScheme, float flValue )
{
	if ( !vgui::scheme() )
		return flValue;

	return static_cast<float>( vgui::scheme()->GetProportionalScaledValueEx( hScheme, static_cast<int>( flValue ) ) );
}

ButtonCode_t GamepadUI_NormalizeJoystickCode( ButtonCode_t code )
{
	// Normalize multi-controller joystick button/POV/axis codes to joystick 0,
	// and Steam Controller codes to slot 0 (matching retail ButtonCode_t ranges).
	int nCode = static_cast<int>( code );

	if ( nCode >= JOYSTICK_FIRST_BUTTON && nCode <= JOYSTICK_LAST_BUTTON )
	{
		return static_cast<ButtonCode_t>( JOYSTICK_FIRST_BUTTON + ( ( nCode - JOYSTICK_FIRST_BUTTON ) % JOYSTICK_MAX_BUTTON_COUNT ) );
	}
	if ( nCode >= JOYSTICK_FIRST_POV_BUTTON && nCode <= JOYSTICK_LAST_POV_BUTTON )
	{
		return static_cast<ButtonCode_t>( JOYSTICK_FIRST_POV_BUTTON + ( ( nCode - JOYSTICK_FIRST_POV_BUTTON ) % JOYSTICK_POV_BUTTON_COUNT ) );
	}
	if ( nCode >= JOYSTICK_FIRST_AXIS_BUTTON && nCode <= JOYSTICK_LAST_AXIS_BUTTON )
	{
		return static_cast<ButtonCode_t>( JOYSTICK_FIRST_AXIS_BUTTON + ( ( nCode - JOYSTICK_FIRST_AXIS_BUTTON ) % JOYSTICK_AXIS_BUTTON_COUNT ) );
	}
	if ( nCode >= STEAMCONTROLLER_FIRST_BUTTON && nCode <= STEAMCONTROLLER_LAST_BUTTON )
	{
		return static_cast<ButtonCode_t>( STEAMCONTROLLER_FIRST_BUTTON + ( ( nCode - STEAMCONTROLLER_FIRST_BUTTON ) % STEAMCONTROLLER_MAX_BUTTON_COUNT ) );
	}
	if ( nCode >= STEAMCONTROLLER_FIRST_AXIS_BUTTON && nCode <= STEAMCONTROLLER_LAST_AXIS_BUTTON )
	{
		return static_cast<ButtonCode_t>( STEAMCONTROLLER_FIRST_AXIS_BUTTON + ( ( nCode - STEAMCONTROLLER_FIRST_AXIS_BUTTON ) % STEAMCONTROLLER_AXIS_BUTTON_COUNT ) );
	}
	return code;
}
