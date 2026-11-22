//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI Achievements Panel & Achievement Row Button.
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#ifndef GAMEPADUI_ACHIEVEMENTS_H
#define GAMEPADUI_ACHIEVEMENTS_H

#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_frame.h"
#include "gamepadui_button.h"
#include "gamepadui_util.h"
#include "tier1/utlstring.h"
#include "tier1/utlvector.h"

class IAchievement;

//-----------------------------------------------------------------------------
// Purpose: Single achievement entry with icon, bottom progress bar, and count
//          Retail vtbl: 0x28ba88
//-----------------------------------------------------------------------------
class GamepadUIAchievement : public GamepadUIButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIAchievement, GamepadUIButton );

	GamepadUIAchievement(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const wchar_t *pText,
		const wchar_t *pDescription,
		const char *pIconImage,
		IAchievement *pAchievement );
	virtual ~GamepadUIAchievement();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void Paint() OVERRIDE;

	float GetHeightOut() const { return m_flHeightState[ButtonState::Out]; }
	float GetHeightOver() const { return m_flHeightState[ButtonState::Over]; }

private:
	GamepadUIImage m_Icon;
	float m_flProgress;
	int m_nCount;
	int m_nGoal;
	vgui::HFont m_hProgressFont;

	GAMEPADUI_PANEL_PROPERTY_COLOR( m_colProgressColor, "Button.Background.Progress", "255 0 0 255" );
	GAMEPADUI_PANEL_PROPERTY_COLOR( m_colUnprogressColor, "Button.Background.Unprogress", "255 0 0 255" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flProgressHeight, "Button.Progress.Height", "1" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flIconInset, "Button.Icon.Inset", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flProgressOffsetX, "Button.Progress.OffsetX", "0" );
};

//-----------------------------------------------------------------------------
// Purpose: Vertically scrolling achievements list with top/bottom fade
//          Retail vtbl: 0x28cee0
//-----------------------------------------------------------------------------
class GamepadUIAchievementsPanel : public GamepadUIFrame
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIAchievementsPanel, GamepadUIFrame );

	GamepadUIAchievementsPanel( vgui::Panel *pParent, const char *pPanelName, const char *pOverrideMod = NULL );
	virtual ~GamepadUIAchievementsPanel();

	virtual void UpdateGradients() OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void OnCommand( const char *pCommand ) OVERRIDE;
	virtual void OnMouseWheeled( int nDelta ) OVERRIDE;

	MESSAGE_FUNC_HANDLE( OnGamepadUIButtonNavigatedTo, "OnGamepadUIButtonNavigatedTo", button );

private:
	void LayoutAchievementButtons();

	CUtlVector<GamepadUIAchievement *> m_pAchievementButtons;
	GamepadUIScrollState m_ScrollState;
	int m_nTotalAchievements;
	int m_nUnlocked;
	CUtlString m_strOverrideMod;

	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_AchievementsFade, "Achievements.Fade", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_AchievementsOffsetX, "Achievements.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_AchievementsOffsetY, "Achievements.OffsetY", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flAchievementsSpacing, "Achievements.Spacing", "0" );
};

#endif // GAMEPADUI_ACHIEVEMENTS_H
