//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Steamworks global interface accessors (SteamUser(), SteamUtils(), ...)
//			for game code. Steamworks 1.4x and later headers define them inline;
//			the older headers in public/steam only offer CSteamAPIContext when
//			VERSION_SAFE_STEAM_API_INTERFACES is defined, so provide them here.
//
//===========================================================================//
#ifndef STEAM_ACCESSORS_H
#define STEAM_ACCESSORS_H
#ifdef _WIN32
#pragma once
#endif

#include "steam/steam_api.h"

#if !defined( STEAM_API_COMMON_H ) && defined( VERSION_SAFE_STEAM_API_INTERFACES )

inline CSteamAPIContext *GameSteamAPIContext()
{
	static CSteamAPIContext s_SteamAPIContext;
	if ( !s_SteamAPIContext.SteamUser() )
	{
		s_SteamAPIContext.Init();
	}
	return &s_SteamAPIContext;
}

inline ISteamUser *SteamUser()				{ return GameSteamAPIContext()->SteamUser(); }
inline ISteamFriends *SteamFriends()		{ return GameSteamAPIContext()->SteamFriends(); }
inline ISteamUtils *SteamUtils()			{ return GameSteamAPIContext()->SteamUtils(); }
inline ISteamUserStats *SteamUserStats()	{ return GameSteamAPIContext()->SteamUserStats(); }
inline ISteamApps *SteamApps()				{ return GameSteamAPIContext()->SteamApps(); }
inline ISteamRemoteStorage *SteamRemoteStorage() { return GameSteamAPIContext()->SteamRemoteStorage(); }

#endif

#endif // STEAM_ACCESSORS_H
