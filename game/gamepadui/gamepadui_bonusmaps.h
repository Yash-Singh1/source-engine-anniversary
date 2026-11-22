//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI Bonus Maps Panel & Bonus Map Button.
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#ifndef GAMEPADUI_BONUSMAPS_H
#define GAMEPADUI_BONUSMAPS_H

#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_frame.h"
#include "gamepadui_button.h"
#include "gamepadui_util.h"
#include "gameui/BonusMapsDatabase.h"
#include "tier1/utlvector.h"

#define MAX_LISTED_BONUS_MAPS 128

//-----------------------------------------------------------------------------
// Purpose: Bonus map / folder tile button with lock icon, progress accent,
//          and challenge medal overlay
//          Retail vtbl: 0x28ea64
//-----------------------------------------------------------------------------
class GamepadUIBonusButton : public GamepadUIButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIBonusButton, GamepadUIButton );

	GamepadUIBonusButton(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const char *pText,
		const char *pDescription,
		const char *pBonusImage );
	virtual ~GamepadUIBonusButton();

	virtual void NavigateTo() OVERRIDE;
	virtual ButtonState::State GetCurrentButtonState() OVERRIDE;
	virtual void Paint() OVERRIDE;

	void SetBonusDescription( const BonusMapDescription_t &desc );
	const BonusMapDescription_t &GetBonusDescription() const { return m_BonusDescription; }

	static GamepadUIBonusButton *s_pSelectedButton;

private:
	GamepadUIImage m_BonusImage;
	GamepadUIImage m_MedalImages[3];
	GamepadUIImage m_LockImage;
	bool m_bChallengeCompleted[3];
	BonusMapDescription_t m_BonusDescription;

	GAMEPADUI_PANEL_PROPERTY_COLOR( m_colProgressColor, "Button.Background.Progress", "255 0 0 255" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flProgressHeight, "Button.Progress.Height", "1" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flMedalSize, "Button.Medal.Size", "64" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flMedalOffsetX, "Button.Medal.OffsetX", "8" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flMedalOffsetY, "Button.Medal.OffsetY", "8" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flLockSize, "Button.Lock.Size", "80" );
	GAMEPADUI_PANEL_PROPERTY_COLOR( m_colLock, "Button.Lock", "20 20 20 255" );
};

//-----------------------------------------------------------------------------
// Purpose: Wrapping grid browser for bonus map directories, maps, and
//          challenge modes with footer medal / best / goal display
//          Retail vtbl: 0x28febc
//-----------------------------------------------------------------------------
class GamepadUIBonusMapsPanel : public GamepadUIFrame
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIBonusMapsPanel, GamepadUIFrame );

	GamepadUIBonusMapsPanel( vgui::Panel *pParent, const char *pPanelName );
	virtual ~GamepadUIBonusMapsPanel();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void UpdateGradients() OVERRIDE;
	virtual void Paint() OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void OnCommand( const char *pCommand ) OVERRIDE;
	virtual void OnMouseWheeled( int nDelta ) OVERRIDE;

	MESSAGE_FUNC_HANDLE( OnGamepadUIButtonNavigatedTo, "OnGamepadUIButtonNavigatedTo", button );

private:
	void BuildMapsList();
	void LayoutBonusButtons();

	CUtlVector<GamepadUIBonusButton *> m_pBonusButtons;
	CUtlVector<BonusMapDescription_t> m_BonusMaps;
	GamepadUIScrollState m_ScrollState;

	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_BonusOffsetX, "Bonus.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_BonusOffsetY, "Bonus.OffsetY", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_BonusSpacing, "Bonus.Spacing", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flFooterMedalSize, "FooterMedal.Current.Size", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flFooterMedalNextSize, "FooterMedal.Next.Size", "0" );

	vgui::HFont m_hGoalFont;
	GamepadUIImage m_FooterCurrentMedal;
	GamepadUIImage m_FooterNextMedal;
	char m_szFooterCurrentMedal[256];
	char m_szFooterNextMedal[256];
};

#endif // GAMEPADUI_BONUSMAPS_H
