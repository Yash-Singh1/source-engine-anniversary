//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Steam Workshop console commands
//
//=============================================================================//

#include "cdll_int.h"
#include "tier1/convar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern IVEngineClient *engineClient;

CON_COMMAND( workshop_open_manager, "Open workshop manager" )
{
	engineClient->ClientCmd_Unrestricted( "gamemenucommand openworkshopdialog\n" );
}
