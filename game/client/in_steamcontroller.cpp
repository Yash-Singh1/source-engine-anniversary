//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Steam Input (controller) movement
//
//===========================================================================//

#include "cbase.h"
#include "input.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Steam Input movement and view angles
//-----------------------------------------------------------------------------
void CInput::SteamControllerMove( float frametime, CUserCmd *cmd )
{
}

//-----------------------------------------------------------------------------
// Purpose: Advanced Steam Input frame time accumulator
//-----------------------------------------------------------------------------
void CInput::SteamInput_SetSampleTime( float frametime )
{
	m_flRemainingSteamInputSampleTime = frametime;
}
