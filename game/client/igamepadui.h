//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Interface to the gamepadui module, which the client loads on Steam Deck.
//
//===========================================================================//

#ifndef IGAMEPADUI_H
#define IGAMEPADUI_H

#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

abstract_class IGamepadUI : public IBaseInterface
{
public:
	virtual void Initialize( CreateInterfaceFn factory ) = 0;
	virtual void Shutdown() = 0;

	virtual void OnUpdate( float flFrametime ) = 0;
	virtual void OnLevelInitializePreEntity() = 0;
	virtual void OnLevelInitializePostEntity() = 0;
	virtual void OnLevelShutdown() = 0;
};

#define GAMEPADUI_INTERFACE_VERSION "GamepadUI001"

extern IGamepadUI *g_pGamepadUI;

#endif // IGAMEPADUI_H
