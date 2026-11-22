//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Core utility types, scheme loading, glyph rendering, scrolling,
//          and gradient helpers for HL2 Anniversary GamepadUI.
//
//===========================================================================//

#ifndef GAMEPADUI_UTIL_H
#define GAMEPADUI_UTIL_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/dbg.h"
#include "tier1/utlvector.h"
#include "tier1/utlstring.h"
#include "tier1/KeyValues.h"
#include "tier1/strtools.h"
#include "mathlib/mathlib.h"
#include "Color.h"
#include "vgui/VGUI.h"
#include "vgui/IImage.h"
#include "vgui/ILocalize.h"
#include "vgui/IScheme.h"
#include "vgui/ISurface.h"
#include "vgui_controls/Controls.h"
#include "vgui_controls/Panel.h"
#include <wchar.h>

#if !defined( NO_STEAM )
#include "steam/steam_api.h"
#include "steam/isteaminput.h"
#endif

#define GamepadUI_Log( ... ) ConColorMsg( Color( 255, 134, 44, 255 ), "[GamepadUI] " __VA_ARGS__ )

//-----------------------------------------------------------------------------
// Button visual states (matches retail *.Out / *.Over / *.Pressed keys)
//-----------------------------------------------------------------------------
namespace ButtonState
{
	enum State
	{
		Out = 0,
		Over = 1,
		Pressed = 2,
		Count = 3,
	};
}

//-----------------------------------------------------------------------------
// Footer button bitflags (exact retail bit layout for GamepadUIFrame)
//-----------------------------------------------------------------------------
namespace FooterButtons
{
	enum Button
	{
		None		= 0,
		Back		= ( 1 << 0 ),	// 0x001: action_back       / #GameUI_Back
		Cancel		= ( 1 << 1 ),	// 0x002: action_cancel     / #GameUI_Cancel
		Select		= ( 1 << 2 ),	// 0x004: action_select     / #GameUI_Select (left)
		SelectRight	= ( 1 << 3 ),	// 0x008: action_select     / #GameUI_Select (right)
		Apply		= ( 1 << 4 ),	// 0x010: action_apply      / #GameUI_Apply
		Okay		= ( 1 << 5 ),	// 0x020: action_okay       / #GameUI_Ok
		Commentary	= ( 1 << 6 ),	// 0x040: action_commentary / #GameUI_Commentary
		BonusMaps	= ( 1 << 7 ),	// 0x080: action_bonus_maps / #Deck_BonusMaps
		Challenges	= ( 1 << 8 ),	// 0x100: action_challenges / #Deck_Challenges
		Details		= ( 1 << 9 ),	// 0x200: action_details    / #GameUI_Workshop_Details
		Move		= ( 1 << 10 ),	// 0x400: action_move       / #GameUI_Workshop_Move
		StopMove	= ( 1 << 11 ),	// 0x800: action_move       / #GameUI_Workshop_StopMove

		MaxButtons	= 12,
		LeftMask	= Back | Cancel | Select,
	};

	inline const char *GetButtonCommand( int nButton )
	{
		switch ( nButton )
		{
		case FooterButtons::Back:			return "action_back";
		case FooterButtons::Cancel:			return "action_cancel";
		case FooterButtons::Select:
		case FooterButtons::SelectRight:	return "action_select";
		case FooterButtons::Apply:			return "action_apply";
		case FooterButtons::Okay:			return "action_okay";
		case FooterButtons::Commentary:		return "action_commentary";
		case FooterButtons::BonusMaps:		return "action_bonus_maps";
		case FooterButtons::Challenges:		return "action_challenges";
		case FooterButtons::Details:		return "action_details";
		case FooterButtons::Move:
		case FooterButtons::StopMove:		return "action_move";
		default:							return "";
		}
	}

	inline const char *GetButtonText( int nButton )
	{
		switch ( nButton )
		{
		case FooterButtons::Back:			return "#GameUI_Back";
		case FooterButtons::Cancel:			return "#GameUI_Cancel";
		case FooterButtons::Select:
		case FooterButtons::SelectRight:	return "#GameUI_Select";
		case FooterButtons::Apply:			return "#GameUI_Apply";
		case FooterButtons::Okay:			return "#GameUI_Ok";
		case FooterButtons::Commentary:		return "#GameUI_Commentary";
		case FooterButtons::BonusMaps:		return "#Deck_BonusMaps";
		case FooterButtons::Challenges:		return "#Deck_Challenges";
		case FooterButtons::Details:		return "#GameUI_Workshop_Details";
		case FooterButtons::Move:			return "#GameUI_Workshop_Move";
		case FooterButtons::StopMove:		return "#GameUI_Workshop_StopMove";
		default:							return "Unknown";
		}
	}

	inline const char *GetButtonActionName( int nButton )
	{
		switch ( nButton )
		{
		case FooterButtons::Back:
		case FooterButtons::Cancel:			return "menu_cancel";
		case FooterButtons::Select:
		case FooterButtons::SelectRight:
		case FooterButtons::Okay:
		case FooterButtons::Move:
		case FooterButtons::StopMove:		return "menu_select";
		case FooterButtons::Apply:
		case FooterButtons::Commentary:
		case FooterButtons::Challenges:
		case FooterButtons::Details:		return "menu_y";
		case FooterButtons::BonusMaps:		return "menu_x";
		default:							return "";
		}
	}
}

//-----------------------------------------------------------------------------
// Wide-character localized string helper (CCopyableUtlVector<wchar_t>)
//-----------------------------------------------------------------------------
class GamepadUIString : public CCopyableUtlVector<wchar_t>
{
public:
	GamepadUIString() {}
	GamepadUIString( const char *pszText ) { SetString( pszText ); }
	GamepadUIString( const wchar_t *pwszText ) { SetString( pwszText ); }
	GamepadUIString( const GamepadUIString &other ) : CCopyableUtlVector<wchar_t>( other ) {}

	GamepadUIString &operator=( const GamepadUIString &other )
	{
		if ( this != &other )
		{
			CopyArray( other.Base(), other.Count() );
		}
		return *this;
	}

	void SetString( const char *pszText )
	{
		Purge();
		if ( !pszText || !pszText[0] )
			return;

		if ( g_pVGuiLocalize )
		{
			const wchar_t *pwszLocalized = g_pVGuiLocalize->Find( pszText );
			if ( pwszLocalized )
			{
				SetString( pwszLocalized );
				return;
			}
		}

		wchar_t wszBuf[4096];
		V_UTF8ToUnicode( pszText, wszBuf, sizeof( wszBuf ) );
		size_t nLen = wcslen( wszBuf );
		if ( nLen == 0 )
			return;

		CopyArray( wszBuf, static_cast<int>( nLen ) + 1 );
	}

	void SetString( const wchar_t *pwszText )
	{
		Purge();
		if ( !pwszText || !pwszText[0] )
			return;

		size_t nLen = wcslen( pwszText );
		if ( nLen == 0 )
			return;

		CopyArray( pwszText, static_cast<int>( nLen ) + 1 );
	}

	void Clear() { RemoveAll(); }

	const wchar_t *String() const { return Count() > 0 ? Base() : L""; }
	int Length() const { return Count() > 0 ? ( Count() - 1 ) : 0; }
	bool IsEmpty() const { return Count() <= 1; }
};

//-----------------------------------------------------------------------------
// Scheme value binding map (inherited by GamepadUIFrame & GamepadUIButton)
//-----------------------------------------------------------------------------
enum class SchemeValueType_t
{
	Float = 0,
	Bool = 1,
	ProportionalFloat = 2,
	Color = 3,
};

struct SchemeValue_t
{
	const char			*m_pszMemberName;
	const char			*m_pszSchemeKey;
	SchemeValueType_t	m_eType;
	const char			*m_pszDefaultValue;
	void				*(*m_pfnGetMember)( void *pInstance );
};

class SchemeValueMap
{
public:
	SchemeValueMap() {}
	~SchemeValueMap() {}

	inline void AddSchemeValue(
		const char *pszMemberName,
		const char *pszSchemeKey,
		SchemeValueType_t eType,
		const char *pszDefaultValue,
		void *(*pfnGetMember)( void * ) )
	{
		SchemeValue_t val;
		val.m_pszMemberName = pszMemberName;
		val.m_pszSchemeKey = pszSchemeKey;
		val.m_eType = eType;
		val.m_pszDefaultValue = pszDefaultValue;
		val.m_pfnGetMember = pfnGetMember;
		m_SchemeValues.AddToTail( val );
	}

	void ApplySchemeValues( vgui::Panel *pPanel, vgui::IScheme *pScheme );

protected:
	CUtlVector<SchemeValue_t> m_SchemeValues;
};

// Helper macro to bind a member variable to a scheme key in a constructor
#define GAMEPADUI_SCHEME_MEMBER( ClassName, Member, SchemeKey, ValueType, DefaultValue ) \
	AddSchemeValue( #Member, SchemeKey, ValueType, DefaultValue, \
		[]( void *pInst ) -> void * { return &static_cast<ClassName *>( pInst )->Member; } )

// Convenience aliases matching retail member types
#define SCHEME_MEMBER_FLOAT( ClassName, Member, SchemeKey, DefaultValue ) \
	GAMEPADUI_SCHEME_MEMBER( ClassName, Member, SchemeKey, SchemeValueType_t::Float, DefaultValue )
#define SCHEME_MEMBER_BOOL( ClassName, Member, SchemeKey, DefaultValue ) \
	GAMEPADUI_SCHEME_MEMBER( ClassName, Member, SchemeKey, SchemeValueType_t::Bool, DefaultValue )
#define SCHEME_MEMBER_PROPORTIONAL( ClassName, Member, SchemeKey, DefaultValue ) \
	GAMEPADUI_SCHEME_MEMBER( ClassName, Member, SchemeKey, SchemeValueType_t::ProportionalFloat, DefaultValue )
#define SCHEME_MEMBER_COLOR( ClassName, Member, SchemeKey, DefaultValue ) \
	GAMEPADUI_SCHEME_MEMBER( ClassName, Member, SchemeKey, SchemeValueType_t::Color, DefaultValue )

// Retail-accurate inline scheme property macro (mirrors CPanelAnimationVar layout:
// a 1-byte registration helper member precedes the field and registers with SchemeValueMap).
#define GAMEPADUI_PANEL_PROPERTY( type, name, schemeKey, defaultValue, valueType ) \
	static void *GetSchemeVar_##name( void *pPanel ) \
	{ \
		return &( (ThisClass *)pPanel )->name; \
	} \
	class SchemeVar_##name \
	{ \
	public: \
		SchemeVar_##name( SchemeValueMap *pMap ) \
		{ \
			pMap->AddSchemeValue( #name, schemeKey, valueType, defaultValue, ThisClass::GetSchemeVar_##name ); \
		} \
	}; \
	SchemeVar_##name m_##name##_register = this; \
	type name

#define GAMEPADUI_PANEL_PROPERTY_FLOAT( name, schemeKey, defaultValue ) \
	GAMEPADUI_PANEL_PROPERTY( float, name, schemeKey, defaultValue, SchemeValueType_t::Float )
#define GAMEPADUI_PANEL_PROPERTY_BOOL( name, schemeKey, defaultValue ) \
	GAMEPADUI_PANEL_PROPERTY( bool, name, schemeKey, defaultValue, SchemeValueType_t::Bool )
#define GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( name, schemeKey, defaultValue ) \
	GAMEPADUI_PANEL_PROPERTY( float, name, schemeKey, defaultValue, SchemeValueType_t::ProportionalFloat )
#define GAMEPADUI_PANEL_PROPERTY_COLOR( name, schemeKey, defaultValue ) \
	GAMEPADUI_PANEL_PROPERTY( Color, name, schemeKey, defaultValue, SchemeValueType_t::Color )

//-----------------------------------------------------------------------------
// Texture / VGUI IImage wrapper
//-----------------------------------------------------------------------------
class GamepadUIImage
{
public:
	GamepadUIImage() : m_nTextureId( -1 ), m_pImage( NULL ) {}
	explicit GamepadUIImage( const char *pszImage ) : m_nTextureId( -1 ), m_pImage( NULL ) { SetImage( pszImage ); }
	explicit GamepadUIImage( vgui::IImage *pImage ) : m_nTextureId( -1 ), m_pImage( NULL ) { SetImage( pImage ); }
	~GamepadUIImage() { Cleanup(); }

	void SetImage( const char *pszImage )
	{
		Cleanup();
		if ( !pszImage || !pszImage[0] || !vgui::surface() )
			return;

		m_nTextureId = vgui::surface()->CreateNewTextureID( false );
		vgui::surface()->DrawSetTextureFile( m_nTextureId, pszImage, true, false );
	}

	void SetImage( vgui::IImage *pImage )
	{
		Cleanup();
		m_pImage = pImage;
		if ( m_pImage )
		{
			m_nTextureId = static_cast<int>( m_pImage->GetID() );
		}
	}

	void SetProceduralTextureId( int nTextureId ) { Cleanup(); m_nTextureId = nTextureId; }

	void Cleanup()
	{
		if ( !m_pImage && m_nTextureId > 0 && vgui::surface() )
		{
			vgui::surface()->DestroyTextureID( m_nTextureId );
		}
		m_nTextureId = -1;
		m_pImage = NULL;
	}

	int GetTextureId()
	{
		if ( m_pImage )
		{
			m_nTextureId = static_cast<int>( m_pImage->GetID() );
		}
		return m_nTextureId;
	}
	operator int() { return GetTextureId(); }
	bool IsValid() { return GetTextureId() > 0; }

	vgui::IImage *GetVGuiImage() const { return m_pImage; }

private:
	GamepadUIImage( const GamepadUIImage & ) = delete;
	GamepadUIImage &operator=( const GamepadUIImage & ) = delete;

	int				m_nTextureId;
	vgui::IImage	*m_pImage;
};

//-----------------------------------------------------------------------------
// Steam Input action glyph texture cache & renderer
//-----------------------------------------------------------------------------
class GamepadUIGlyph
{
public:
	GamepadUIGlyph();
	~GamepadUIGlyph();

	bool SetupGlyph( int nSize, const char *pszAction, bool bLight = false );
	void PaintGlyph( int nX, int nY, int nSize, float flFade = 0.0f ) const;
	void Cleanup();

	bool IsValid() const { return m_nTextureID[0] > 0 && m_nTextureID[1] > 0; }
	int GetTextureID( int nIndex ) const { return ( nIndex >= 0 && nIndex < 2 ) ? m_nTextureID[nIndex] : -1; }

private:
	GamepadUIGlyph( const GamepadUIGlyph & ) = delete;
	GamepadUIGlyph &operator=( const GamepadUIGlyph & ) = delete;

	int m_eOrigin;
	int m_nTextureID[2];
};

//-----------------------------------------------------------------------------
// Smooth scrolling state used by scrollable list panels
//-----------------------------------------------------------------------------
class GamepadUIScrollState
{
public:
	GamepadUIScrollState()
		: m_flScrollTarget( 0.0f )
		, m_flScrollProgress( 0.0f )
		, m_flScrollStart( 0.0f )
		, m_flScrollStartTime( 0.0f )
	{
	}

	void OnMouseWheeled( int nDelta, float flTime, float flStep = 160.0f )
	{
		m_flScrollTarget -= static_cast<float>( static_cast<int>( static_cast<float>( nDelta ) * flStep ) );
		m_flScrollStart = m_flScrollProgress;
		m_flScrollStartTime = flTime;
	}

	void SetScrollTarget( float flTarget, float flTime )
	{
		if ( flTarget != m_flScrollTarget )
		{
			m_flScrollTarget = flTarget;
			m_flScrollStart = m_flScrollProgress;
			m_flScrollStartTime = flTime;
		}
	}

	void UpdateScrollBounds( float flMin, float flMax )
	{
		if ( flMax < flMin )
			flMax = flMin;

		m_flScrollTarget = clamp( m_flScrollTarget, flMin, flMax );
	}

	float UpdateScroll( float flTime, float flDuration = 0.25f )
	{
		float flT = 1.0f;
		if ( flDuration > 0.0f )
		{
			flT = clamp( ( flTime - m_flScrollStartTime ) / flDuration, 0.0f, 1.0f );
			flT = SimpleSpline( flT );
		}

		m_flScrollProgress = Lerp( flT, m_flScrollStart, m_flScrollTarget );
		return m_flScrollProgress;
	}

	float GetScrollTarget() const { return m_flScrollTarget; }
	float GetScrollProgress() const { return m_flScrollProgress; }

	void SetScrollProgressImmediate( float flValue )
	{
		m_flScrollTarget = flValue;
		m_flScrollProgress = flValue;
		m_flScrollStart = flValue;
	}

	void Reset()
	{
		m_flScrollTarget = 0.0f;
		m_flScrollProgress = 0.0f;
		m_flScrollStart = 0.0f;
		m_flScrollStartTime = 0.0f;
	}

public:
	float m_flScrollTarget;
	float m_flScrollProgress;
	float m_flScrollStart;
	float m_flScrollStartTime;
};

//-----------------------------------------------------------------------------
// Animated 4-side screen gradient backdrop helper (stored in GamepadUI)
//-----------------------------------------------------------------------------
namespace GamepadUIGradientSides
{
	enum Side
	{
		Left = 0,
		Right = 1,
		Up = 2,
		Down = 3,
		Count = 4,
	};
}

struct GamepadUIGradientState
{
	float flAmount;
	float flExtent;
};

class GamepadUIGradientHelper
{
public:
	static constexpr float k_flTransitionDuration = 0.5f;

	GamepadUIGradientHelper()
	{
		V_memset( m_flTime, 0, sizeof( m_flTime ) );
		V_memset( m_Previous, 0, sizeof( m_Previous ) );
		V_memset( m_Target, 0, sizeof( m_Target ) );
	}

	void ResetTargets( float flTime )
	{
		for ( int i = 0; i < GamepadUIGradientSides::Count; ++i )
		{
			GamepadUIGradientSides::Side eSide = static_cast<GamepadUIGradientSides::Side>( i );
			m_Previous[i] = GetCurrentGradient( eSide, flTime );
			m_Target[i].flAmount = 0.0f;
			m_Target[i].flExtent = 0.0f;
			m_flTime[i] = flTime;
		}
	}

	void SetTargetGradient( GamepadUIGradientSides::Side eSide, GamepadUIGradientState target, float flTime )
	{
		int idx = static_cast<int>( eSide );
		if ( idx < 0 || idx >= GamepadUIGradientSides::Count )
			return;

		m_Previous[idx] = GetCurrentGradient( eSide, flTime );
		m_Target[idx] = target;
		m_flTime[idx] = flTime;
	}

	GamepadUIGradientState GetCurrentGradient( GamepadUIGradientSides::Side eSide, float flTime ) const
	{
		int idx = static_cast<int>( eSide );
		if ( idx < 0 || idx >= GamepadUIGradientSides::Count )
			return { 0.0f, 0.0f };

		float flStart = m_flTime[idx];
		float flEnd = flStart + k_flTransitionDuration;
		float flFrac = 0.0f;
		if ( flStart == flEnd )
		{
			flFrac = ( flTime >= flEnd ) ? 1.0f : 0.0f;
		}
		else
		{
			float flNorm = clamp( ( flTime - flStart ) / k_flTransitionDuration, 0.0f, 1.0f );
			flFrac = SimpleSpline( flNorm );
		}

		GamepadUIGradientState result;
		result.flAmount = Lerp( flFrac, m_Previous[idx].flAmount, m_Target[idx].flAmount );

		result.flExtent = m_Target[idx].flExtent;
		if ( m_Target[idx].flAmount != 0.0f && m_Previous[idx].flAmount != 0.0f && m_Previous[idx].flExtent > m_Target[idx].flExtent )
		{
			result.flExtent = Lerp( flFrac, m_Previous[idx].flExtent, m_Target[idx].flExtent );
		}

		return result;
	}

	void PaintGradients( int nWide, int nTall, float flTime ) const;

public:
	float					m_flTime[GamepadUIGradientSides::Count];
	GamepadUIGradientState	m_Previous[GamepadUIGradientSides::Count];
	GamepadUIGradientState	m_Target[GamepadUIGradientSides::Count];
};

//-----------------------------------------------------------------------------
// Free utility functions matching retail gamepadui_util
//-----------------------------------------------------------------------------
bool GamepadUI_LoadKeyValues( const char *pszFileName, KeyValues *pKV );
const char *GamepadUI_GetModName();
int GamepadUI_DrawWrappedText( vgui::HFont hFont, int nX, int nY, const wchar_t *pwszText, int nLen, int nMaxWidth, bool bDraw = true );
int GamepadUI_NextPowerOfTwo( int nValue );
float GamepadUI_ProportionalScale( vgui::HScheme hScheme, float flValue );
ButtonCode_t GamepadUI_NormalizeJoystickCode( ButtonCode_t code );

#endif // GAMEPADUI_UTIL_H
