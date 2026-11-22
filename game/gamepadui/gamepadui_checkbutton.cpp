//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Checkbox-styled GamepadUIButton subclass.
//
//===========================================================================//

#include "gamepadui_checkbutton.h"

#include "vgui/ISurface.h"
#include "vgui/IScheme.h"

#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructors & destructor
//-----------------------------------------------------------------------------
GamepadUICheckButton::GamepadUICheckButton(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pszSchemeFile,
	const char *pszCommand,
	const char *pszText,
	const char *pszDescription )
	: BaseClass( pParent, pActionSignalTarget, pszSchemeFile, pszCommand, pszText, pszDescription )
	, m_flCheckOffsetX( 10.0f )
	, m_flCheckHeight( 18.0f )
	, m_hCheckFont( vgui::INVALID_FONT )
{
}

GamepadUICheckButton::GamepadUICheckButton(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pszSchemeFile,
	const char *pszCommand,
	const wchar_t *pwszText,
	const wchar_t *pwszDescription )
	: BaseClass( pParent, pActionSignalTarget, pszSchemeFile, pszCommand, pwszText, pwszDescription )
	, m_flCheckOffsetX( 10.0f )
	, m_flCheckHeight( 18.0f )
	, m_hCheckFont( vgui::INVALID_FONT )
{
}

GamepadUICheckButton::~GamepadUICheckButton()
{
}

//-----------------------------------------------------------------------------
// Purpose: ApplySchemeSettings
//-----------------------------------------------------------------------------
void GamepadUICheckButton::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	m_hCheckFont = pScheme->GetFont( "Button.Check.Font", true );
}

//-----------------------------------------------------------------------------
// Purpose: Paint checkbox glyph using Marlett font (matching retail 0xa94c0)
//-----------------------------------------------------------------------------
void GamepadUICheckButton::Paint()
{
	BaseClass::Paint();

	if ( !vgui::surface() || m_hCheckFont == vgui::INVALID_FONT )
		return;

	vgui::surface()->DrawSetTextColor( m_colTextColor );
	vgui::surface()->DrawSetTextFont( m_hCheckFont );

	int nCheckX = static_cast<int>( m_flCheckOffsetX );
	int nCheckY = static_cast<int>( ( m_flHeight - m_flCheckHeight ) * 0.5f );

	// Marlett 'j' and 'k' compose the checkbox frame
	vgui::surface()->DrawSetTextPos( nCheckX, nCheckY );
	vgui::surface()->DrawPrintText( L"j", 1 );

	vgui::surface()->DrawSetTextPos( nCheckX, nCheckY );
	vgui::surface()->DrawPrintText( L"k", 1 );

	// Draw checkmark when selected/checked
	if ( IsSelected() )
	{
		vgui::surface()->DrawSetTextPos( nCheckX, nCheckY );
		vgui::surface()->DrawPrintText( L"b", 1 );
	}
}
