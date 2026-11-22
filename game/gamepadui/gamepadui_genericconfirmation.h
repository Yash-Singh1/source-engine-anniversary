//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Generic confirmation modal frame for HL2 Anniversary GamepadUI
//          (Quit confirmation, Return to Main Menu, Overwrite Save, etc.).
//
//===========================================================================//

#ifndef GAMEPADUI_GENERICCONFIRMATION_H
#define GAMEPADUI_GENERICCONFIRMATION_H

#ifdef _WIN32
#pragma once
#endif

#pragma push_macro( "min" )
#pragma push_macro( "max" )
#undef min
#undef max
#include <functional>
#pragma pop_macro( "max" )
#pragma pop_macro( "min" )

#include "gamepadui_frame.h"

//-----------------------------------------------------------------------------
// Purpose: Modal confirmation dialog with localized title, body, and callback
//-----------------------------------------------------------------------------
class GamepadUIGenericConfirmationPanel : public GamepadUIFrame
{
	DECLARE_CLASS_SIMPLE( GamepadUIGenericConfirmationPanel, GamepadUIFrame );

public:
	GamepadUIGenericConfirmationPanel(
		vgui::Panel *pParent,
		const char *pszPanelName,
		const char *pszTitle,
		const char *pszText,
		std::function<void()> fnCallback,
		bool bUseGenericFont = false,
		bool bShowCancel = true );

	GamepadUIGenericConfirmationPanel(
		vgui::Panel *pParent,
		const char *pszPanelName,
		const wchar_t *pwszTitle,
		const wchar_t *pwszText,
		std::function<void()> fnCallback,
		bool bUseGenericFont = false,
		bool bShowCancel = true );

	virtual ~GamepadUIGenericConfirmationPanel();

	// GamepadUIFrame overrides (matching retail vtable)
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void OnCommand( const char *pszCommand ) OVERRIDE;
	virtual void Paint() OVERRIDE;
	virtual void UpdateGradients() OVERRIDE;

	void PaintText();

protected:
	void InitCommon( bool bUseGenericFont, bool bShowCancel );

	GamepadUIString			m_strText;
	std::function<void()>	m_fnCallback;

	GAMEPADUI_PANEL_PROPERTY_COLOR( m_colGenericConfirmationColor, "GenericConfirmation", "255 255 255 255" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flGenericConfirmationOffsetX, "GenericConfirmation.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flGenericConfirmationOffsetY, "GenericConfirmation.OffsetY", "0" );
	vgui::HFont				m_hConfirmationFont;
	const char				*m_pszFontName;
};

#endif // GAMEPADUI_GENERICCONFIRMATION_H
