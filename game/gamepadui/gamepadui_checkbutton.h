//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Checkbox-styled GamepadUIButton subclass (used for options etc.).
//
//===========================================================================//

#ifndef GAMEPADUI_CHECKBUTTON_H
#define GAMEPADUI_CHECKBUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_button.h"

//-----------------------------------------------------------------------------
// Purpose: GamepadUIButton with a Marlett checkbox indicator on the left
//-----------------------------------------------------------------------------
class GamepadUICheckButton : public GamepadUIButton
{
	DECLARE_CLASS_SIMPLE( GamepadUICheckButton, GamepadUIButton );

public:
	GamepadUICheckButton(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pszSchemeFile,
		const char *pszCommand,
		const char *pszText,
		const char *pszDescription = "" );

	GamepadUICheckButton(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pszSchemeFile,
		const char *pszCommand,
		const wchar_t *pwszText,
		const wchar_t *pwszDescription = L"" );

	virtual ~GamepadUICheckButton();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void Paint() OVERRIDE;

protected:
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flCheckOffsetX, "Button.Check.OffsetX", "10" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flCheckHeight, "Button.Check.Height", "18" );
	vgui::HFont	m_hCheckFont;
};

#endif // GAMEPADUI_CHECKBUTTON_H
