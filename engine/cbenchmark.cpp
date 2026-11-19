//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "client_pch.h"

#ifdef IS_WINDOWS_PC
#include "winlite.h"
#include <winsock2.h> // INADDR_ANY defn
#endif
#include "cbenchmark.h"
#include "tier0/vcrmode.h"
#include "filesystem_engine.h"
#include "sys.h"
#include "KeyValues.h"
#include "sv_uploaddata.h"
#include "FindSteamServers.h"
#include "vstdlib/random.h"
#include "cl_steamauth.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define DEFAULT_RESULTS_FOLDER "results"
#define DEFAULT_RESULTS_FILENAME "results.txt"

CBenchmarkResults g_BenchmarkResults;
extern ConVar host_framerate;
extern void GetMaterialSystemConfigForBenchmarkUpload(KeyValues *dataToUpload);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CBenchmarkResults::CBenchmarkResults()
{
	m_bIsTestRunning = false;
	m_szFilename[0] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBenchmarkResults::IsBenchmarkRunning()
{
	return m_bIsTestRunning;
}
	
//-----------------------------------------------------------------------------
// Purpose: starts recording data
//-----------------------------------------------------------------------------
void CBenchmarkResults::StartBenchmark( const CCommand &args )
{
	const char *pszFilename = DEFAULT_RESULTS_FILENAME;
	
	if ( args.ArgC() > 1 )
	{
		pszFilename = args[1];
	}

	// check path first
	if ( !COM_IsValidPath( pszFilename ) )
	{
		ConMsg( "bench_start %s: invalid path.\n", pszFilename );
		return;
	}

	m_bIsTestRunning = true;

	SetResultsFilename( pszFilename );

	// set any necessary settings
#ifdef BYTEMATCH_RETAIL
	host_framerate.SetValue( (float)(1.0f / host_state.interval_per_tick) );
#else
	//host_framerate.SetValue( (float)(1.0f / host_state.interval_per_tick) );
#endif
#line 75

	// get the current frame and time
	m_iStartFrame = host_framecount;
	m_flStartTime = realtime;
#ifndef BYTEMATCH_RETAIL

	m_flNextSecondTime = realtime + 1.0f;
	m_iNextSecondFrame = host_framecount;
#endif
#line 82
}

//-----------------------------------------------------------------------------
// Purpose: writes out results to file
//-----------------------------------------------------------------------------
void CBenchmarkResults::StopBenchmark()
{
	m_bIsTestRunning = false;

	// reset
#ifdef BYTEMATCH_RETAIL
	host_framerate.SetValue( 0 );
#else
	//host_framerate.SetValue( 0 );
#endif
#line 93

	// print out some stats
	int numticks = host_framecount - m_iStartFrame;
	float framerate = numticks / ( realtime - m_flStartTime );
	Msg( "Average framerate: %.2f\n", framerate );
	
	// work out where to write the file
	g_pFileSystem->CreateDirHierarchy( DEFAULT_RESULTS_FOLDER, "MOD" );
	char szFilename[256];
	Q_snprintf( szFilename, sizeof( szFilename ), "%s\\%s", DEFAULT_RESULTS_FOLDER, m_szFilename );

	// write out the data as keyvalues
	KeyValues *kv = new KeyValues( "benchmark" );
	kv->SetFloat( "framerate", framerate );
	kv->SetInt( "build", build_number() );

#ifndef BYTEMATCH_RETAIL
	CUtlString str;
	for( int i = 0; i < m_FPSInfo.Count(); i++ )
	{
		str += m_FPSInfo[i];
		if( i != m_FPSInfo.Count()-1 )
			str += ',';
	}
	kv->SetString( "framerates", str );
#endif
#line 117

	// get material system info
	GetMaterialSystemConfigForBenchmarkUpload( kv );

	// save
	kv->SaveToFile( g_pFileSystem, szFilename, "MOD" );
	kv->deleteThis();
#ifndef BYTEMATCH_RETAIL

	m_FPSInfo.Purge();
#endif
#line 126
}

//-----------------------------------------------------------------------------
// Purpose: Sets which file the results will be written to
//-----------------------------------------------------------------------------
void CBenchmarkResults::SetResultsFilename( const char *pFilename )
{
	Q_strncpy( m_szFilename, pFilename, sizeof( m_szFilename ) );
	Q_DefaultExtension( m_szFilename, ".txt", sizeof( m_szFilename ) );
}

//-----------------------------------------------------------------------------
// Purpose: uploads the most recent results to Steam
//-----------------------------------------------------------------------------
void CBenchmarkResults::Upload()
{

























}

#ifndef BYTEMATCH_RETAIL
void CBenchmarkResults::Frame()
{
	if( !m_bIsTestRunning )
		return;

	if( m_flNextSecondTime <= realtime )
	{
		m_FPSInfo.AddToTail( host_framecount-m_iNextSecondFrame );
		m_flNextSecondTime += 1.0f;
		m_iNextSecondFrame = host_framecount;
	}
}
#endif


CON_COMMAND_F( bench_start, "Starts gathering of info. Arguments: filename to write results into", FCVAR_CHEAT )
{
	GetBenchResultsMgr()->StartBenchmark( args );
}

CON_COMMAND_F( bench_end, "Ends gathering of info.", FCVAR_CHEAT )
{
	GetBenchResultsMgr()->StopBenchmark();
}

#ifndef BYTEMATCH_RETAIL
CON_COMMAND_F( bench_upload, "Uploads most recent benchmark stats to the Valve servers.", FCVAR_CHEAT )
{
	GetBenchResultsMgr()->Upload();
}
#endif
#line 197

