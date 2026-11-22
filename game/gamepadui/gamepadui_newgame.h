//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI New Game (Chapter Select) Panel & Chapter Button.
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#ifndef GAMEPADUI_NEWGAME_H
#define GAMEPADUI_NEWGAME_H

#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_frame.h"
#include "gamepadui_button.h"
#include "gamepadui_util.h"
#include "tier1/utlstring.h"
#include "tier1/utlvector.h"

struct chapter_t
{
	char filename[32];
};

//-----------------------------------------------------------------------------
// Purpose: Chapter tile button with 16:9 thumbnail that crops up on focus
//          Retail vtbl: 0x297180
//-----------------------------------------------------------------------------
class GamepadUIChapterButton : public GamepadUIButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIChapterButton, GamepadUIButton );

	GamepadUIChapterButton(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const wchar_t *pText,
		const wchar_t *pDescription,
		const char *pChapterImage );
	virtual ~GamepadUIChapterButton();

	virtual void NavigateTo() OVERRIDE;
	virtual ButtonState::State GetCurrentButtonState() OVERRIDE;
	virtual void Paint() OVERRIDE;

	static GamepadUIChapterButton *s_pLastSelectedButton;

private:
	GamepadUIImage m_ChapterImage;
};

//-----------------------------------------------------------------------------
// Purpose: Horizontal scrolling chapter selection screen (supports override
//          mod for HL2 Anniversary Episodes & Lost Coast, plus Commentary mode)
//          Retail vtbl: 0x2985d8
//-----------------------------------------------------------------------------
class GamepadUINewGamePanel : public GamepadUIFrame
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUINewGamePanel, GamepadUIFrame );

	GamepadUINewGamePanel( vgui::Panel *pParent, const char *pPanelName, const char *pOverrideMod = NULL );
	virtual ~GamepadUINewGamePanel();

	virtual void UpdateGradients() OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void OnCommand( const char *pCommand ) OVERRIDE;
	virtual void OnMouseWheeled( int nDelta ) OVERRIDE;

	MESSAGE_FUNC_HANDLE( OnGamepadUIButtonNavigatedTo, "OnGamepadUIButtonNavigatedTo", button );

	void StartGame( int nChapter );

private:
	void LayoutChapterButtons();

	CUtlVector<GamepadUIChapterButton *> m_pChapterButtons;
	CUtlVector<chapter_t> m_Chapters;
	GamepadUIScrollState m_ScrollState;

	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_ChapterOffsetX, "Chapters.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_ChapterOffsetY, "Chapters.OffsetY", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_ChapterSpacing, "Chapters.Spacing", "0" );

	bool m_bCommentaryMode;
	CUtlString m_strOverrideMod;
};

#endif // GAMEPADUI_NEWGAME_H
