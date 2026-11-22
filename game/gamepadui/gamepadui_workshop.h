//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI Steam Workshop Panel & Workshop Item Buttons
//          (Campaigns horizontal card view + Subscribed addons list view).
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#ifndef GAMEPADUI_WORKSHOP_H
#define GAMEPADUI_WORKSHOP_H

#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_frame.h"
#include "gamepadui_button.h"
#include "gamepadui_util.h"
#include "iworkshop.h"
#include "matchmaking/imatchevents.h"
#include "tier1/utlvector.h"

//-----------------------------------------------------------------------------
// Purpose: Base Workshop item button holding WorkshopItem_t* + preview image
//          Retail vtbl: 0x2a3578 / 0x2a4eec (TI 0x2a24f4)
//-----------------------------------------------------------------------------
class GamepadUIWorkshopButton : public GamepadUIButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIWorkshopButton, GamepadUIButton );

	GamepadUIWorkshopButton(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const char *pText,
		const char *pDescription,
		WorkshopItem_t *pItem );
	virtual ~GamepadUIWorkshopButton();

	WorkshopItem_t *GetItem() const { return m_pItem; }
	GamepadUIImage &GetImage() { return m_Image; }

	void ResetButtonInputState()
	{
		SetArmed( false );
		SetSelected( false );
		m_bControllerDepressed = false;
		m_bNavigated = false;
	}

protected:
	WorkshopItem_t	*m_pItem;	// +0x43c
	GamepadUIImage	m_Image;	// +0x440
};

//-----------------------------------------------------------------------------
// Purpose: Square campaign card button (Campaigns tab, horizontal carousel)
//          Retail vtbl: 0x2a3a08 (TI 0x2a2500), size 0x44c on i386
//-----------------------------------------------------------------------------
class GamepadUIWorkshopButton_Square : public GamepadUIWorkshopButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIWorkshopButton_Square, GamepadUIWorkshopButton );

	GamepadUIWorkshopButton_Square(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const char *pText,
		const char *pDescription,
		WorkshopItem_t *pItem );
	virtual ~GamepadUIWorkshopButton_Square();

	virtual ButtonState::State GetCurrentButtonState() OVERRIDE;
	virtual void NavigateTo() OVERRIDE;
	virtual void Paint() OVERRIDE;

	static GamepadUIWorkshopButton_Square *s_pLastSelectedButton;
};

//-----------------------------------------------------------------------------
// Purpose: Subscribed addon row button (Subscribed tab, vertical list with
//          enable/disable toggle and priority reorder support)
//          Retail vtbl: 0x2a537c (TI 0x2a250c), size 0x464 on i386
//-----------------------------------------------------------------------------
class GamepadUIWorkshopButton_List : public GamepadUIWorkshopButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIWorkshopButton_List, GamepadUIWorkshopButton );

	GamepadUIWorkshopButton_List(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const char *pText,
		const char *pDescription,
		WorkshopItem_t *pItem );
	virtual ~GamepadUIWorkshopButton_List();

	virtual void OnKeyCodePressed( vgui::KeyCode code ) OVERRIDE;
	virtual void Paint() OVERRIDE;

	bool IsAddonEnabled() const { return m_bEnabled; }
	void SetAddonEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }

private:
	static const GamepadUIString &GetToggleStateText( bool bEnabled );

	bool	m_bEnabled;	// +0x44b

	CPanelAnimationVar( float, m_flImageOffsetX, "m_flImageOffsetX", "0" );
	float	m_flImageOffsetXs[ButtonState::Count];
	float	m_flImageOffsetXAnimationDuration;
};

//-----------------------------------------------------------------------------
// Purpose: Tabbed Workshop browser panel (Campaigns + Subscribed addons)
//          Retail vtbl: 0x2a67d4 (GamepadUIFrame), 0x2a6c38 (IMatchEventsSink @ +0x2dc)
//          Retail size: 0x3e0 on i386
//-----------------------------------------------------------------------------
class GamepadUIWorkshopPanel : public GamepadUIFrame, public IMatchEventsSink
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIWorkshopPanel, GamepadUIFrame );

	static const int MAX_WORKSHOP_TABS = 2;

	GamepadUIWorkshopPanel( vgui::Panel *pParent, const char *pPanelName );
	virtual ~GamepadUIWorkshopPanel();

	// GamepadUIFrame overrides
	virtual void UpdateGradients() OVERRIDE;
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void Paint() OVERRIDE;
	virtual void OnMouseWheeled( int nDelta ) OVERRIDE;
	virtual void OnKeyCodePressed( vgui::KeyCode code ) OVERRIDE;
	virtual void OnCommand( const char *pCommand ) OVERRIDE;

	// IMatchEventsSink override
	virtual void OnEvent( KeyValues *pEvent ) OVERRIDE;

	MESSAGE_FUNC_INT( OnGamepadUIButtonNavigatedTo, "OnGamepadUIButtonNavigatedTo", button );

	bool IsMovingAddon() const { return m_bMovingAddon; }

	static GamepadUIWorkshopPanel *s_pWorkshopPanel;

private:
	struct WorkshopTab_t
	{
		GamepadUIButton					*m_pTabButton;	// +0x00
		CUtlVector<GamepadUIButton *>	m_Buttons;		// +0x04
		GamepadUIScrollState			m_ScrollState;	// +0x18

		WorkshopTab_t() : m_pTabButton( NULL ) {}
	};

	void AddTab( const char *pszTitle );
	void BuildTabs();
	void PopulateWorkshopButtons();
	void LayoutTabs();
	void LayoutWorkshopButtons();
	void ChangeTab( int nTab );
	int GetCurrentTab() const;
	void UpdateFooterButtons();

	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flTabsOffsetX, "Tabs.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flTabsOffsetY, "Tabs.OffsetY", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flOptionsFade, "Options.Fade", "80" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flScrollBarOffsetX, "Scrollbar.OffsetX", "10" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flScrollBarWidth, "Scrollbar.Width", "80" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flScrollBarHeight, "Scrollbar.Height", "80" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_ChapterOffsetX, "Chapters.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_ChapterOffsetY, "Chapters.OffsetY", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_ChapterSpacing, "Chapters.Spacing", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_WorkshopOffsetX, "Workshop.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_WorkshopOffsetY, "Workshop.OffsetY", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_WorkshopSpacing, "Workshop.Spacing", "0" );

	WorkshopTab_t								m_Tabs[MAX_WORKSHOP_TABS];	// +0x344
	int											m_nTabCount;				// +0x394
	GamepadUIGlyph								m_GlyphLB;					// +0x398
	GamepadUIGlyph								m_GlyphRB;					// +0x3a4
	CUtlVector<GamepadUIWorkshopButton_Square *> m_pCampaignButtons;		// +0x3b0
	CUtlVector<GamepadUIWorkshopButton_List *>	m_pAddonButtons;			// +0x3c4
	GamepadUIWorkshopButton						*m_pSelectedButton;			// +0x3d8
	bool										m_bMovingAddon;				// +0x3dc
};

#endif // GAMEPADUI_WORKSHOP_H
