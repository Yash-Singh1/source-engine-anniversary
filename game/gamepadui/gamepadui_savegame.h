//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI Save / Load Game Panel & Save Button.
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#ifndef GAMEPADUI_SAVEGAME_H
#define GAMEPADUI_SAVEGAME_H

#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_frame.h"
#include "gamepadui_button.h"
#include "gamepadui_util.h"
#include "tier1/utlstring.h"
#include "tier1/utlvector.h"

#define SAVEGAME_MAPNAME_LEN 32
#define SAVEGAME_COMMENT_LEN 80
#define SAVEGAME_ELAPSED_LEN 32

#define MAX_LISTED_SAVE_GAMES 128

struct SaveGameDescription_t
{
	char szShortName[64];
	char szFileName[128];
	char szMapName[SAVEGAME_MAPNAME_LEN];
	char szComment[SAVEGAME_COMMENT_LEN];
	char szType[64];
	char szElapsedTime[SAVEGAME_ELAPSED_LEN];
	char szFileTime[32];
	unsigned int iTimestamp;
	unsigned int iSize;
};

//-----------------------------------------------------------------------------
// Purpose: Save slot list row button with 180x100 TGA screenshot thumbnail
//          Retail vtbl: 0x2a0c1c
//-----------------------------------------------------------------------------
class GamepadUISaveButton : public GamepadUIButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUISaveButton, GamepadUIButton );

	GamepadUISaveButton(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const char *pText,
		const char *pDescription,
		SaveGameDescription_t *pSaveGame );
	virtual ~GamepadUISaveButton();

	virtual void Paint() OVERRIDE;

	SaveGameDescription_t *GetSaveGame() const { return m_pSaveGame; }
	float GetHeightOut() const { return m_flHeightState[ButtonState::Out]; }
	float GetHeightOver() const { return m_flHeightState[ButtonState::Over]; }

private:
	bool m_bCustomImage;
	GamepadUIImage m_SaveImage;
	SaveGameDescription_t *m_pSaveGame;
};

//-----------------------------------------------------------------------------
// Purpose: Save Game and Load Game scrolling list panel (supports override
//          mod for HL2 Anniversary per-expansion saves & overwrite dialog)
//          Retail vtbl: 0x2a2074
//-----------------------------------------------------------------------------
class GamepadUISaveGamePanel : public GamepadUIFrame
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUISaveGamePanel, GamepadUIFrame );

	GamepadUISaveGamePanel( vgui::Panel *pParent, const char *pPanelName, bool bSaveGame, const char *pOverrideMod = NULL );
	virtual ~GamepadUISaveGamePanel();

	virtual void UpdateGradients() OVERRIDE;
	virtual void Paint() OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void OnCommand( const char *pCommand ) OVERRIDE;
	virtual void OnMouseWheeled( int nDelta ) OVERRIDE;

	MESSAGE_FUNC_HANDLE( OnGamepadUIButtonNavigatedTo, "OnGamepadUIButtonNavigatedTo", button );

private:
	void ScanSavedGames();
	bool ParseSaveData( const char *pszFileName, const char *pszShortName, SaveGameDescription_t &save );
	void DeleteSaveGame( const char *pszFileName );
	void FindSaveSlot( char *buffer, int bufsize );
	void LoadSaveGame( const SaveGameDescription_t *pSave );
	void SaveGame( const SaveGameDescription_t *pSave );
	void LayoutSaveButtons();

	GamepadUIString m_strNoSaves;
	CUtlVector<GamepadUISaveButton *> m_pSaveButtons;
	CUtlVector<SaveGameDescription_t> m_SaveGames;
	GamepadUIScrollState m_ScrollState;

	bool m_bSaveGame;
	CUtlString m_strOverrideMod;

	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flSavesFade, "Saves.Fade", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flSavesOffsetX, "Saves.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flSavesOffsetY, "Saves.OffsetY", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flSavesSpacing, "Saves.Spacing", "0" );
};

#endif // GAMEPADUI_SAVEGAME_H
