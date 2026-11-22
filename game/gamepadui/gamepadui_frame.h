//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Full-screen base frame for HL2 Anniversary GamepadUI panels,
//          providing title rendering, gradient backdrop, and footer buttons.
//
//===========================================================================//

#ifndef GAMEPADUI_FRAME_H
#define GAMEPADUI_FRAME_H

#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_util.h"
#include "gamepadui_button.h"
#include "vgui_controls/Frame.h"

//-----------------------------------------------------------------------------
// Purpose: Base frame inherited by MainMenu, GenericConfirmation, and all
//          6 GamepadUI feature panels.
//-----------------------------------------------------------------------------
class GamepadUIFrame : public vgui::Frame, public SchemeValueMap
{
	DECLARE_CLASS_SIMPLE( GamepadUIFrame, vgui::Frame );

public:
	GamepadUIFrame(
		vgui::Panel *pParent,
		const char *pszPanelName,
		bool bShowTaskbarIcon = true,
		bool bPopup = true );

	virtual ~GamepadUIFrame();

	// vgui::Frame / vgui::Panel overrides (matching retail vtable)
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void OnKeyCodePressed( vgui::KeyCode code ) OVERRIDE;
	virtual void OnKeyCodeReleased( vgui::KeyCode code ) OVERRIDE;
	virtual void Paint() OVERRIDE;
	virtual void OnClose() OVERRIDE;

	// Retail GamepadUIFrame virtual slot [276] (+0x450)
	virtual void UpdateGradients();

	// Footer button management
	void SetFooterButtons( int nButtonMask, int nMouseHideMask = 0 );
	void LayoutFooterButtons();
	GamepadUIButton *GetFooterButton( FooterButtons::Button eButton ) const;
	int GetFooterButtonWidth() const { return m_nFooterButtonWidth; }
	int GetFooterButtonHeight() const { return m_nFooterButtonHeight; }

	// Title / gradient rendering helpers
	void PaintGradients();
	void PaintTitle();
	void SetTitleText( const char *pszTitle );
	void SetTitleText( const wchar_t *pwszTitle );
	const GamepadUIString &GetFrameTitle() const { return m_strTitle; }

	// Font accessors
	vgui::HFont GetTitleFont() const { return m_hTitleFont; }
	vgui::HFont GetGenericFont() const { return m_hGenericFont; }

	// Layout offsets
	float GetTitleOffsetX() const { return m_flTitleOffsetX; }
	float GetTitleOffsetY() const { return m_flTitleOffsetY; }
	float GetFooterButtonsOffsetX() const { return m_flFooterButtonsOffsetX; }
	float GetFooterButtonsOffsetY() const { return m_flFooterButtonsOffsetY; }
	float GetFooterButtonsSpacing() const { return m_flFooterButtonsSpacing; }

protected:
	void TriggerFooterButtonsPressed( int nMask );
	void TriggerFooterButtonsReleased( int nMask );

	GamepadUIString		m_strTitle;
	GamepadUIButton		*m_pFooterButtons[FooterButtons::MaxButtons];
	int					m_nFooterButtonsHideOnMouse;

	GAMEPADUI_PANEL_PROPERTY_COLOR( m_colTitleColor, "Title", "255 255 255 255" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flTitleOffsetX, "Title.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flTitleOffsetY, "Title.OffsetY", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flFooterButtonsOffsetX, "FooterButtons.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flFooterButtonsOffsetY, "FooterButtons.OffsetY", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flFooterButtonsSpacing, "FooterButtons.Spacing", "0" );

	vgui::HFont			m_hTitleFont;
	vgui::HFont			m_hGenericFont;

	int					m_nFooterButtonWidth;
	int					m_nFooterButtonHeight;
};

#endif // GAMEPADUI_FRAME_H
