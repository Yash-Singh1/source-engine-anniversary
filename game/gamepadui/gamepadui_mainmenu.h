//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Main menu & pause menu frame for HL2 Anniversary GamepadUI.
//
//===========================================================================//

#ifndef GAMEPADUI_MAINMENU_H
#define GAMEPADUI_MAINMENU_H

#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_frame.h"
#include "tier1/utlvector.h"
#include "tier1/utlstring.h"

//-----------------------------------------------------------------------------
// Purpose: HL2 Anniversary GamepadUI main / in-game pause menu
//-----------------------------------------------------------------------------
class GamepadUIMainMenu : public GamepadUIFrame
{
	DECLARE_CLASS_SIMPLE( GamepadUIMainMenu, GamepadUIFrame );

public:
	explicit GamepadUIMainMenu( vgui::Panel *pParent );
	virtual ~GamepadUIMainMenu();

	// GamepadUIFrame overrides (matching retail vtable)
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void OnCommand( const char *pszCommand ) OVERRIDE;
	virtual void OnSetFocus() OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void OnKeyCodeReleased( vgui::KeyCode code ) OVERRIDE;
	virtual void Paint() OVERRIDE;
	virtual void UpdateGradients() OVERRIDE;

	// Menu state & category/expansion switching
	void OnMenuStateChanged();
	void SwitchCategory( const char *pszCategory );
	void SwitchExpansion( const char *pszExpansion );
	void UpdateMenus();
	void UpdateLogo();
	void LayoutButtons();
	void PaintLogo();

	int GetCurrentFamily() const;
	const CUtlVector<GamepadUIButton *> &GetActiveButtons() const { return m_pActiveButtons; }
	const char *GetCurrentCategory() const { return m_strCurrentCategory.String(); }
	const char *GetCurrentMod() const { return m_strCurrentMod.String(); }
	GamepadUIImage &GetLogoTexture() { return m_LogoImage; }

protected:
	void LoadMainMenuScript();
	void BuildActiveButtonList();
	float GetButtonsOffsetY() const;
	float GetLogoOffsetY() const;

	enum MenuFamily_t
	{
		FAMILY_INGAME = 0,
		FAMILY_MAINMENU = 1,
		FAMILY_COUNT = 2,
	};

	CUtlVector<GamepadUIButton *>	m_pButtons[FAMILY_COUNT];
	CUtlVector<GamepadUIButton *>	m_pActiveButtons;

	GamepadUIString					m_strDefaultLogoTitle;
	GamepadUIString					m_strDefaultLogoSubtitle;
	GamepadUIString					m_strLogoTitle;
	GamepadUIString					m_strLogoSubtitle;
	bool							m_bLogoTitleValid;
	bool							m_bLogoSubtitleValid;

	GamepadUIImage					m_LogoImage;
	bool							m_bIsHL2Complete;

	CUtlString						m_strCurrentMod;
	CUtlString						m_strCurrentCategory;

	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flButtonSpacing, "Buttons.Space", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flButtonsOffsetX, "Buttons.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flButtonsOffsetYMenu, "Buttons.OffsetY.MainMenu", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flButtonsOffsetYInGame, "Buttons.OffsetY.InGame", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flLogoOffsetX, "Logo.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flLogoOffsetYMenu, "Logo.OffsetY.MainMenu", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flLogoOffsetYInGame, "Logo.OffsetY.InGame", "0" );
	GAMEPADUI_PANEL_PROPERTY_COLOR( m_colLogoColor, "Logo", "255 255 255 255" );
	vgui::HFont						m_hLogoFont;
};

#endif // GAMEPADUI_MAINMENU_H
