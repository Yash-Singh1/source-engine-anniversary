//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Contains an interface to log some events to the Steam Share 
//			timeline via ISteamVideo 
//
// $NoKeywords: $
//
//===========================================================================//

#ifndef STEAMSHARE_H
#define STEAMSHARE_H

#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "GameEventListener.h"

#ifdef HL2_ANNIVERSARY_NOV2024
#include "utlvector.h"
#include "utlstring.h"
#include "steam/isteamtimeline.h"

class CSteamShareSystem;

class CSteamTimelineEventBase_t
{
public:
	virtual const char *GetEventName() { return m_pszEventName; }
	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event ) = 0;
	virtual void Init() {}

	const char *m_pszEventName;
};

typedef CSteamTimelineEventBase_t *( *SteamTimelineEventCreateFn_t )();

class CSteamTimelineEventHelper
{
public:
	CSteamTimelineEventHelper( SteamTimelineEventCreateFn_t pfnCreate );

	SteamTimelineEventCreateFn_t m_pfnCreate;
	CSteamTimelineEventHelper *m_pNext;

	static CSteamTimelineEventHelper *s_pFirst;
};
#endif

class CSteamShareSystem : public CBaseGameSystemPerFrame, public CGameEventListener
{
public:
	// Methods of IGameSystem
	virtual bool	Init();

	// Methods of IGameEventListener2
	virtual void FireGameEvent( IGameEvent *event );

#ifdef HL2_ANNIVERSARY_NOV2024
	void UpdateChapterDetails();
	void UpdateSteamTimelineGameMode();
	void SetTimelineGameMode( ETimelineGameMode eMode )
	{
		m_eGameMode = eMode;
		UpdateSteamTimelineGameMode();
	}
	void PushTimelineGameMode( ETimelineGameMode eMode );
	void PopTimelineGameMode();

	ETimelineGameMode m_eGameMode;
	CUtlVector< ETimelineGameMode > m_GameModeStack;
	CUtlVector< CSteamTimelineEventBase_t * > m_Events;
	CUtlString m_strChapterName;
	float m_flChapterStartTime;
	int m_nChapterDeaths;
	bool m_bInGame;
#endif

private:
};

extern IGameSystem *SteamShareSystem();

#endif // STEAMSHARE_H
