//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//
#if !defined( PROFILE_H )
#define PROFILE_H
#ifdef _WIN32
#pragma once
#endif

void Host_ReadConfiguration();
#if defined( BYTEMATCH_RETAIL ) || defined( HL2_COMPLETE_ENGINE )
void Host_WriteConfiguration( const char *filename = NULL );
#else
void Host_WriteConfiguration( const char *filename = NULL, bool bAllVars = false );
#endif
#line 21

#endif // PROFILE_H
