//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Root VGUI panel for HL2 Anniversary GamepadUI, hosting the main
//          menu and managing startup background music playback.
//
//===========================================================================//

#ifndef GAMEPADUI_BASEPANEL_H
#define GAMEPADUI_BASEPANEL_H

#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/Panel.h"

class GamepadUIMainMenu;

//-----------------------------------------------------------------------------
// Purpose: Top-level GamepadUI root panel (set as GameUI main menu override)
//-----------------------------------------------------------------------------
class GamepadUIBasePanel : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( GamepadUIBasePanel, vgui::Panel );

public:
	explicit GamepadUIBasePanel( vgui::VPANEL parent );
	virtual ~GamepadUIBasePanel();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;

	GamepadUIMainMenu *GetMainMenu() const;

	bool IsBackgroundMusicPlaying() const;
	void StartBackgroundMusic( float flVolume = 1.0f );
	void StopBackgroundMusic();
	void UpdateBackgroundMusic();

private:
	GamepadUIMainMenu	*m_pMainMenu;				// +0x17c
	int					m_nBackgroundMusicGUID;		// +0x180
	bool				m_bPlayBackgroundMusic;		// +0x184
};

#endif // GAMEPADUI_BASEPANEL_H
