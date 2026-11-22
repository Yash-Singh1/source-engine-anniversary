//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Animated, scheme-driven GamepadUI button control.
//
//===========================================================================//

#ifndef GAMEPADUI_BUTTON_H
#define GAMEPADUI_BUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_util.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/PanelAnimationVar.h"

//-----------------------------------------------------------------------------
// Purpose: Base button control for all HL2 Anniversary GamepadUI menus/panels
//-----------------------------------------------------------------------------
class GamepadUIButton : public vgui::Button, public SchemeValueMap
{
	DECLARE_CLASS_SIMPLE( GamepadUIButton, vgui::Button );

public:
	GamepadUIButton(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pszSchemeFile,
		const char *pszCommand,
		const char *pszText,
		const char *pszDescription = "" );

	GamepadUIButton(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pszSchemeFile,
		const char *pszCommand,
		const wchar_t *pwszText,
		const wchar_t *pwszDescription = L"" );

	virtual ~GamepadUIButton();

	// vgui::Button / vgui::Panel overrides (matching retail vtable)
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void OnCursorEntered() OVERRIDE;
	virtual void OnKeyCodePressed( vgui::KeyCode code ) OVERRIDE;
	virtual void OnKeyCodeReleased( vgui::KeyCode code ) OVERRIDE;
	virtual void Paint() OVERRIDE;
	virtual void NavigateTo() OVERRIDE;
	virtual void NavigateFrom() OVERRIDE;
	virtual void FireActionSignal() OVERRIDE;

	// Message / new virtual methods matching retail GamepadUIButton slots [260, 286..288]
	MESSAGE_FUNC( OnSiblingGamepadUIButtonOpened, "OnSiblingGamepadUIButtonOpened" );
	MESSAGE_FUNC( OnHotkey, "Hotkey" ) {}
	virtual void RunAnimations( ButtonState::State eState );
	virtual ButtonState::State GetCurrentButtonState();

	// Rendering helpers usable by subclasses
	void PaintButton();
	void PaintBorders();
	int PaintText();
	void DoAnimations( bool bForce = false );

	// Footer / navigation / metadata accessors
	void SetFooterButton( int nFooterButton );
	int GetFooterButton() const { return m_nFooterButton; }
	bool IsFooterButton() const { return m_nFooterButton != 0; }

	void SetForwardToParent( bool bForward ) { m_bForwardToParent = bForward; }
	bool GetForwardToParent() const { return m_bForwardToParent; }

	void SetCategory( const char *pszCategory );
	const char *GetCategory() const;

	void SetPriority( int nPriority ) { m_nPriority = nPriority; }
	int GetPriority() const { return m_nPriority; }

	void SetAllCaps( bool bAllCaps );
	bool IsAllCaps() const { return m_bButtonAllCaps; }

	void SetButtonText( const char *pszText );
	void SetButtonText( const wchar_t *pwszText );
	void SetButtonDescription( const char *pszDescription );
	void SetButtonDescription( const wchar_t *pwszDescription );
	const GamepadUIString &GetButtonText() const { return m_strButtonText; }
	const GamepadUIString &GetButtonDescription() const { return m_strButtonDescription; }

	float GetCurrentWidth() const { return m_flWidth; }
	float GetCurrentHeight() const { return m_flHeight + m_flExtraHeight; }
	float GetExtraHeight() const { return m_flExtraHeight; }
	float GetWrappedDescriptionHeight() const { return m_flWrappedDescriptionHeight; }

	vgui::HFont GetTextFont() const { return m_hTextFont; }
	vgui::HFont GetTextFontOver() const { return m_hTextFontOver; }
	vgui::HFont GetDescriptionFont() const { return m_hDescriptionFont; }

	static bool s_bSuppressNav;

protected:
	void InitCommon( vgui::Panel *pActionSignalTarget, const char *pszSchemeFile, const char *pszCommand );
	void RegisterSchemeValues();

	ButtonState::State	m_ePreviousState;
	bool				m_bButtonAllCaps;
	bool				m_bControllerDepressed;
	bool				m_bNavigated;
	bool				m_bForwardToParent;
	bool				m_bAllowMouseNav;

	// Dynamic extra height animation for wrapped descriptions
	float				m_flExtraHeight;
	float				m_flWrappedDescriptionHeight;
	float				m_flExtraHeightTarget;
	float				m_flExtraHeightStart;
	float				m_flExtraHeightStartTime;

	int					m_nFooterButton;
	GamepadUIString		m_strButtonText;
	GamepadUIString		m_strButtonDescription;
	GamepadUIGlyph		m_Glyph;
	CUtlString			m_strCategory;
	int					m_nPriority;

	// Animated panel properties (matched to retail CPanelAnimationVar + SchemeValueMap)
	CPanelAnimationVar( float, m_flWidth, "m_flWidth", "392" );
	float m_flWidthState[ButtonState::Count];
	float m_flWidthAnimDuration;

	CPanelAnimationVar( float, m_flHeight, "m_flHeight", "40" );
	float m_flHeightState[ButtonState::Count];
	float m_flHeightAnimDuration;

	CPanelAnimationVar( float, m_flTextOffsetX, "m_flTextOffsetX", "10" );
	float m_flTextOffsetXState[ButtonState::Count];
	float m_flTextOffsetXAnimDuration;

	CPanelAnimationVar( float, m_flTextOffsetY, "m_flTextOffsetY", "0" );
	float m_flTextOffsetYState[ButtonState::Count];
	float m_flTextOffsetYAnimDuration;

	CPanelAnimationVar( float, m_flDescriptionOffsetX, "m_flDescriptionOffsetX", "0" );
	float m_flDescriptionOffsetXState[ButtonState::Count];
	float m_flDescriptionOffsetXAnimDuration;

	CPanelAnimationVar( float, m_flDescriptionOffsetY, "m_flDescriptionOffsetY", "0" );
	float m_flDescriptionOffsetYState[ButtonState::Count];
	float m_flDescriptionOffsetYAnimDuration;

	CPanelAnimationVar( Color, m_colBackgroundColor, "m_colBackgroundColor", "0 0 0 0" );
	Color m_colBackgroundColorState[ButtonState::Count];
	float m_flBackgroundAnimDuration;

	CPanelAnimationVar( Color, m_colTextColor, "m_colTextColor", "255 255 255 255" );
	Color m_colTextColorState[ButtonState::Count];
	float m_flTextAnimDuration;

	CPanelAnimationVar( Color, m_colDescriptionColor, "m_colDescriptionColor", "255 255 255 255" );
	Color m_colDescriptionColorState[ButtonState::Count];
	float m_flDescriptionAnimDuration;

	CPanelAnimationVar( float, m_flBackgroundBlurAmount, "m_flBackgroundBlurAmount", "0" );
	float m_flBackgroundBlurAmountState[ButtonState::Count];
	float m_flBackgroundBlurAnimDuration;

	CPanelAnimationVar( float, m_flGlyphFade, "m_flGlyphFade", "0" );
	float m_flGlyphFadeState[ButtonState::Count];
	float m_flGlyphFadeAnimDuration;

	CPanelAnimationVar( bool, m_bDescriptionHide, "m_bDescriptionHide", "0" );
	bool m_bDescriptionHideState[ButtonState::Count];
	float m_flDescriptionHideAnimDuration;

	CPanelAnimationVar( float, m_flTextLeftBorder, "m_flTextLeftBorder", "0" );
	float m_flTextLeftBorderState[ButtonState::Count];
	float m_flTextLeftBorderAnimDuration;

	CPanelAnimationVar( Color, m_colLeftBorder, "m_colLeftBorder", "0 0 0 0" );
	Color m_colLeftBorderState[ButtonState::Count];
	float m_flLeftBorderAnimDuration;

	CPanelAnimationVar( float, m_flTextBottomBorder, "m_flTextBottomBorder", "0" );
	float m_flTextBottomBorderState[ButtonState::Count];
	float m_flTextBottomBorderAnimDuration;

	CPanelAnimationVar( Color, m_colBottomBorder, "m_colBottomBorder", "0 0 0 0" );
	Color m_colBottomBorderState[ButtonState::Count];
	float m_flBottomBorderAnimDuration;

	GAMEPADUI_PANEL_PROPERTY_BOOL( m_CenterX, "Button.Text.CenterX", "0" );
	GAMEPADUI_PANEL_PROPERTY_BOOL( m_bDescriptionWrap, "Button.Description.Wrap", "1" );

	vgui::HFont m_hTextFont;
	vgui::HFont m_hTextFontOver;
	vgui::HFont m_hDescriptionFont;
};

#endif // GAMEPADUI_BUTTON_H
