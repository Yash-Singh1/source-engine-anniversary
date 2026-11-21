#define _CRT_SECURE_NO_WARNINGS
#define STEAM_API_EXPORTS

#if defined( OSX ) && defined( PLATFORM_64BITS )

#define VERSION_SAFE_STEAM_API_INTERFACES 1
#include "steam/steam_api.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <algorithm>
#include <mutex>
#include <vector>

typedef void *(*CreateInterfaceFn)( const char *pName, int *pReturnCode );
typedef bool (*Steam_BGetCallbackFn)( HSteamPipe hSteamPipe, CallbackMsg_t *pCallbackMsg );
typedef void (*Steam_FreeLastCallbackFn)( HSteamPipe hSteamPipe );
typedef bool (*Steam_GetAPICallResultFn)( HSteamPipe hSteamPipe, SteamAPICall_t hSteamAPICall, void *pCallback, int cubCallback, int iCallbackExpected, bool *pbFailed );
typedef void (*Steam_ReleaseThreadLocalMemoryFn)( int bThreadExit );

static void *g_hSteamClientLib = NULL;
static CreateInterfaceFn g_pCreateInterface = NULL;
static Steam_BGetCallbackFn g_pSteam_BGetCallback = NULL;
static Steam_FreeLastCallbackFn g_pSteam_FreeLastCallback = NULL;
static Steam_GetAPICallResultFn g_pSteam_GetAPICallResult = NULL;
static Steam_ReleaseThreadLocalMemoryFn g_pSteam_ReleaseThreadLocalMemory = NULL;

static ISteamClient *g_pSteamClient = NULL;
static HSteamPipe g_hSteamPipe = 0;
static HSteamUser g_hSteamUser = 0;
static uintptr_t g_nContextCounter = 1;
static char g_szSteamInstallPath[1024] = { 0 };
static bool g_bSteamInitialized = false;

S_API void *g_pSteamClientGameServer;
void *g_pSteamClientGameServer = NULL;

struct CallResultEntry_t
{
	SteamAPICall_t m_hAPICall;
	CCallbackBase *m_pCallback;
};

class CCallbackMgr
{
public:
	static CCallbackMgr &Instance()
	{
		static CCallbackMgr s_Mgr;
		return s_Mgr;
	}

	void RegisterCallback( CCallbackBase *pCallback, int iCallback )
	{
		if ( !pCallback )
			return;
		std::lock_guard<std::recursive_mutex> lock( m_Mutex );
		pCallback->m_iCallback = iCallback;
		pCallback->m_nCallbackFlags |= CCallbackBase::k_ECallbackFlagsRegistered;
		if ( std::find( m_Callbacks.begin(), m_Callbacks.end(), pCallback ) == m_Callbacks.end() )
		{
			m_Callbacks.push_back( pCallback );
		}
	}

	void UnregisterCallback( CCallbackBase *pCallback )
	{
		if ( !pCallback )
			return;
		std::lock_guard<std::recursive_mutex> lock( m_Mutex );
		pCallback->m_nCallbackFlags &= ~CCallbackBase::k_ECallbackFlagsRegistered;
		auto it = std::find( m_Callbacks.begin(), m_Callbacks.end(), pCallback );
		if ( it != m_Callbacks.end() )
		{
			*it = NULL;
		}
	}

	void RegisterCallResult( CCallbackBase *pCallback, SteamAPICall_t hAPICall )
	{
		if ( !pCallback || hAPICall == k_uAPICallInvalid )
			return;
		std::lock_guard<std::recursive_mutex> lock( m_Mutex );
		CallResultEntry_t entry = { hAPICall, pCallback };
		m_CallResults.push_back( entry );
	}

	void UnregisterCallResult( CCallbackBase *pCallback, SteamAPICall_t hAPICall )
	{
		if ( !pCallback )
			return;
		std::lock_guard<std::recursive_mutex> lock( m_Mutex );
		for ( size_t i = 0; i < m_CallResults.size(); ++i )
		{
			if ( m_CallResults[i].m_pCallback == pCallback &&
				 ( hAPICall == k_uAPICallInvalid || m_CallResults[i].m_hAPICall == hAPICall ) )
			{
				m_CallResults[i].m_pCallback = NULL;
				m_CallResults[i].m_hAPICall = k_uAPICallInvalid;
			}
		}
	}

	void RunCallbacks( HSteamPipe hPipe, bool bGameServer )
	{
		if ( !hPipe || !g_pSteam_BGetCallback || !g_pSteam_FreeLastCallback )
			return;

		CallbackMsg_t msg;
		while ( g_pSteam_BGetCallback( hPipe, &msg ) )
		{
			if ( msg.m_iCallback == SteamAPICallCompleted_t::k_iCallback &&
				 msg.m_pubParam && msg.m_cubParam >= (int)sizeof( SteamAPICallCompleted_t ) )
			{
				SteamAPICallCompleted_t completed;
				memcpy( &completed, msg.m_pubParam, sizeof( completed ) );

				std::vector<CCallbackBase *> matched;
				{
					std::lock_guard<std::recursive_mutex> lock( m_Mutex );
					for ( size_t i = 0; i < m_CallResults.size(); ++i )
					{
						if ( m_CallResults[i].m_hAPICall == completed.m_hAsyncCall && m_CallResults[i].m_pCallback )
						{
							matched.push_back( m_CallResults[i].m_pCallback );
							m_CallResults[i].m_pCallback = NULL;
							m_CallResults[i].m_hAPICall = k_uAPICallInvalid;
						}
					}
				}

				if ( !matched.empty() && g_pSteam_GetAPICallResult )
				{
					std::vector<uint8> buf( completed.m_cubParam > 0 ? completed.m_cubParam : 1, 0 );
					bool bFailed = false;
					bool bGot = g_pSteam_GetAPICallResult(
						hPipe,
						completed.m_hAsyncCall,
						buf.data(),
						(int)completed.m_cubParam,
						completed.m_iCallback,
						&bFailed );

					if ( bGot )
					{
						for ( size_t i = 0; i < matched.size(); ++i )
						{
							if ( matched[i] )
								matched[i]->Run( buf.data(), bFailed, completed.m_hAsyncCall );
						}
					}
				}
			}
			else
			{
				std::vector<CCallbackBase *> snapshot;
				{
					std::lock_guard<std::recursive_mutex> lock( m_Mutex );
					for ( size_t i = 0; i < m_Callbacks.size(); ++i )
					{
						CCallbackBase *pCB = m_Callbacks[i];
						if ( !pCB )
							continue;
						bool cbIsGS = ( pCB->m_nCallbackFlags & CCallbackBase::k_ECallbackFlagsGameServer ) != 0;
						if ( cbIsGS == bGameServer && pCB->m_iCallback == msg.m_iCallback )
						{
							snapshot.push_back( pCB );
						}
					}
				}

				for ( size_t i = 0; i < snapshot.size(); ++i )
				{
					// Re-check registration in case an earlier callback unregistered this one
					bool bStillRegistered = false;
					{
						std::lock_guard<std::recursive_mutex> lock( m_Mutex );
						bStillRegistered = std::find( m_Callbacks.begin(), m_Callbacks.end(), snapshot[i] ) != m_Callbacks.end();
					}
					if ( bStillRegistered )
					{
						snapshot[i]->Run( msg.m_pubParam );
					}
				}
			}

			g_pSteam_FreeLastCallback( hPipe );
		}

		// Compact tombstones
		{
			std::lock_guard<std::recursive_mutex> lock( m_Mutex );
			m_Callbacks.erase(
				std::remove( m_Callbacks.begin(), m_Callbacks.end(), (CCallbackBase *)NULL ),
				m_Callbacks.end() );
			m_CallResults.erase(
				std::remove_if( m_CallResults.begin(), m_CallResults.end(),
					[]( const CallResultEntry_t &e ) { return e.m_pCallback == NULL; } ),
				m_CallResults.end() );
		}
	}

private:
	std::recursive_mutex m_Mutex;
	std::vector<CCallbackBase *> m_Callbacks;
	std::vector<CallResultEntry_t> m_CallResults;
};

static void EnsureSteamAppIdEnv()
{
	const char *pszEnv = getenv( "SteamAppId" );
	if ( pszEnv && pszEnv[0] )
		return;

	FILE *f = fopen( "steam_appid.txt", "r" );
	if ( !f )
		return;

	char szAppId[64] = { 0 };
	if ( fgets( szAppId, sizeof( szAppId ), f ) )
	{
		size_t len = strlen( szAppId );
		while ( len > 0 && ( szAppId[len - 1] == '\n' || szAppId[len - 1] == '\r' || szAppId[len - 1] == ' ' || szAppId[len - 1] == '\t' ) )
			szAppId[--len] = '\0';
		if ( len > 0 )
		{
			setenv( "SteamAppId", szAppId, 1 );
			if ( !getenv( "SteamGameId" ) )
				setenv( "SteamGameId", szAppId, 1 );
		}
	}
	fclose( f );
}

static bool TryLoadSteamClientPath( const char *pszDir )
{
	if ( !pszDir || !pszDir[0] )
		return false;

	char szPath[1024];
	snprintf( szPath, sizeof( szPath ), "%s/steamclient.dylib", pszDir );
	struct stat st;
	if ( stat( szPath, &st ) != 0 )
		return false;

	void *h = dlopen( szPath, RTLD_NOW | RTLD_LOCAL );
	if ( !h )
		return false;

	CreateInterfaceFn pCreate = (CreateInterfaceFn)dlsym( h, "CreateInterface" );
	Steam_BGetCallbackFn pGetCB = (Steam_BGetCallbackFn)dlsym( h, "Steam_BGetCallback" );
	Steam_FreeLastCallbackFn pFreeCB = (Steam_FreeLastCallbackFn)dlsym( h, "Steam_FreeLastCallback" );
	Steam_GetAPICallResultFn pGetResult = (Steam_GetAPICallResultFn)dlsym( h, "Steam_GetAPICallResult" );
	Steam_ReleaseThreadLocalMemoryFn pReleaseMem = (Steam_ReleaseThreadLocalMemoryFn)dlsym( h, "Steam_ReleaseThreadLocalMemory" );

	if ( !pCreate || !pGetCB || !pFreeCB || !pGetResult )
	{
		dlclose( h );
		return false;
	}

	g_hSteamClientLib = h;
	g_pCreateInterface = pCreate;
	g_pSteam_BGetCallback = pGetCB;
	g_pSteam_FreeLastCallback = pFreeCB;
	g_pSteam_GetAPICallResult = pGetResult;
	g_pSteam_ReleaseThreadLocalMemory = pReleaseMem;
	snprintf( g_szSteamInstallPath, sizeof( g_szSteamInstallPath ), "%s", pszDir );
	return true;
}

static bool LoadSteamClientLibrary()
{
	if ( g_hSteamClientLib )
		return true;

	const char *pszInsert = getenv( "STEAM_DYLD_INSERT_LIBRARIES" );
	if ( !pszInsert || !pszInsert[0] )
		pszInsert = getenv( "DYLD_INSERT_LIBRARIES" );
	if ( pszInsert && pszInsert[0] )
	{
		char szBuf[1024];
		snprintf( szBuf, sizeof( szBuf ), "%s", pszInsert );
		char *pColon = strchr( szBuf, ':' );
		if ( pColon )
			*pColon = '\0';
		char *pSlash = strrchr( szBuf, '/' );
		if ( pSlash )
		{
			*pSlash = '\0';
			if ( TryLoadSteamClientPath( szBuf ) )
				return true;
		}
	}

	const char *pszHome = getenv( "HOME" );
	if ( pszHome && pszHome[0] )
	{
		char szDir[1024];
		snprintf( szDir, sizeof( szDir ), "%s/Library/Application Support/Steam/Steam.AppBundle/Steam/Contents/MacOS", pszHome );
		if ( TryLoadSteamClientPath( szDir ) )
			return true;
	}

	if ( TryLoadSteamClientPath( "/Applications/Steam.app/Contents/MacOS" ) )
		return true;

	return false;
}

S_API ESteamAPIInitResult S_CALLTYPE SteamInternal_SteamAPI_Init( const char *pszInternalCheckInterfaceVersions, SteamErrMsg *pOutErrMsg )
{
	if ( g_bSteamInitialized && g_pSteamClient && g_hSteamPipe && g_hSteamUser )
		return k_ESteamAPIInitResult_OK;

	EnsureSteamAppIdEnv();

	if ( !LoadSteamClientLibrary() )
	{
		if ( pOutErrMsg )
			snprintf( *pOutErrMsg, k_cchMaxSteamErrMsg, "Unable to locate Steam client library (steamclient.dylib)" );
		return k_ESteamAPIInitResult_NoSteamClient;
	}

	int rc = 0;
	g_pSteamClient = (ISteamClient *)g_pCreateInterface( STEAMCLIENT_INTERFACE_VERSION, &rc );
	if ( !g_pSteamClient )
		g_pSteamClient = (ISteamClient *)g_pCreateInterface( "SteamClient020", &rc );
	if ( !g_pSteamClient )
		g_pSteamClient = (ISteamClient *)g_pCreateInterface( "SteamClient017", &rc );

	if ( !g_pSteamClient )
	{
		if ( pOutErrMsg )
			snprintf( *pOutErrMsg, k_cchMaxSteamErrMsg, "Failed to acquire ISteamClient interface" );
		return k_ESteamAPIInitResult_VersionMismatch;
	}

	g_hSteamPipe = g_pSteamClient->CreateSteamPipe();
	if ( !g_hSteamPipe )
	{
		if ( pOutErrMsg )
			snprintf( *pOutErrMsg, k_cchMaxSteamErrMsg, "Steam is not running (CreateSteamPipe failed)" );
		return k_ESteamAPIInitResult_NoSteamClient;
	}

	g_hSteamUser = g_pSteamClient->ConnectToGlobalUser( g_hSteamPipe );
	if ( !g_hSteamUser )
	{
		g_pSteamClient->BReleaseSteamPipe( g_hSteamPipe );
		g_hSteamPipe = 0;
		if ( pOutErrMsg )
			snprintf( *pOutErrMsg, k_cchMaxSteamErrMsg, "Steam is not running (ConnectToGlobalUser failed)" );
		return k_ESteamAPIInitResult_NoSteamClient;
	}

	g_bSteamInitialized = true;
	++g_nContextCounter;
	return k_ESteamAPIInitResult_OK;
}

S_API ESteamAPIInitResult S_CALLTYPE SteamAPI_InitFlat( SteamErrMsg *pOutErrMsg )
{
	return SteamInternal_SteamAPI_Init( NULL, pOutErrMsg );
}

S_API bool S_CALLTYPE SteamAPI_InitSafe()
{
	return SteamInternal_SteamAPI_Init( NULL, NULL ) == k_ESteamAPIInitResult_OK;
}

S_API void S_CALLTYPE SteamAPI_Shutdown()
{
	if ( g_pSteamClient && g_hSteamPipe )
	{
		if ( g_hSteamUser )
		{
			g_pSteamClient->ReleaseUser( g_hSteamPipe, g_hSteamUser );
			g_hSteamUser = 0;
		}
		g_pSteamClient->BReleaseSteamPipe( g_hSteamPipe );
		g_hSteamPipe = 0;
	}
	g_bSteamInitialized = false;
	++g_nContextCounter;
}

S_API bool S_CALLTYPE SteamAPI_RestartAppIfNecessary( uint32 )
{
	return false;
}

S_API void S_CALLTYPE SteamAPI_ReleaseCurrentThreadMemory()
{
	if ( g_pSteam_ReleaseThreadLocalMemory )
		g_pSteam_ReleaseThreadLocalMemory( 0 );
}

S_API void S_CALLTYPE SteamAPI_WriteMiniDump( uint32, void *, uint32 ) {}
S_API void S_CALLTYPE SteamAPI_SetMiniDumpComment( const char * ) {}

S_API void S_CALLTYPE SteamAPI_RunCallbacks()
{
	if ( g_bSteamInitialized && g_hSteamPipe )
		CCallbackMgr::Instance().RunCallbacks( g_hSteamPipe, false );
}

S_API void S_CALLTYPE SteamAPI_RegisterCallback( CCallbackBase *pCallback, int iCallback )
{
	CCallbackMgr::Instance().RegisterCallback( pCallback, iCallback );
}

S_API void S_CALLTYPE SteamAPI_UnregisterCallback( CCallbackBase *pCallback )
{
	CCallbackMgr::Instance().UnregisterCallback( pCallback );
}

S_API void S_CALLTYPE SteamAPI_RegisterCallResult( CCallbackBase *pCallback, SteamAPICall_t hAPICall )
{
	CCallbackMgr::Instance().RegisterCallResult( pCallback, hAPICall );
}

S_API void S_CALLTYPE SteamAPI_UnregisterCallResult( CCallbackBase *pCallback, SteamAPICall_t hAPICall )
{
	CCallbackMgr::Instance().UnregisterCallResult( pCallback, hAPICall );
}

S_API bool S_CALLTYPE SteamAPI_IsSteamRunning()
{
	return g_bSteamInitialized && g_hSteamPipe != 0 && g_hSteamUser != 0;
}

S_API void S_CALLTYPE Steam_RunCallbacks( HSteamPipe hSteamPipe, bool bGameServerCallbacks )
{
	CCallbackMgr::Instance().RunCallbacks( hSteamPipe, bGameServerCallbacks );
}

S_API void S_CALLTYPE Steam_RegisterInterfaceFuncs( void * ) {}

S_API HSteamUser S_CALLTYPE Steam_GetHSteamUserCurrent()
{
	return g_hSteamUser;
}

S_API const char *SteamAPI_GetSteamInstallPath()
{
	return g_szSteamInstallPath[0] ? g_szSteamInstallPath : NULL;
}

S_API HSteamPipe S_CALLTYPE SteamAPI_GetHSteamPipe()
{
	return g_hSteamPipe;
}

S_API HSteamUser S_CALLTYPE SteamAPI_GetHSteamUser()
{
	return g_hSteamUser;
}

S_API HSteamPipe S_CALLTYPE GetHSteamPipe()
{
	return g_hSteamPipe;
}

S_API HSteamUser S_CALLTYPE GetHSteamUser()
{
	return g_hSteamUser;
}

S_API void S_CALLTYPE SteamAPI_SetTryCatchCallbacks( bool ) {}
S_API void S_CALLTYPE SteamAPI_SetBreakpadAppID( uint32 ) {}
S_API void S_CALLTYPE SteamAPI_UseBreakpadCrashHandler( const char *, const char *, const char *, bool, void *, PFNPreMinidumpCallback ) {}

S_API void *S_CALLTYPE SteamInternal_ContextInit( void *pContextInitData )
{
	struct ContextInitData_t
	{
		void (*pFn)( void *pCtx );
		uintptr_t counter;
		void *ptr;
	};

	ContextInitData_t *p = (ContextInitData_t *)pContextInitData;
	if ( p->counter != g_nContextCounter )
	{
		p->ptr = NULL;
		if ( g_bSteamInitialized && g_pSteamClient && g_hSteamPipe && g_hSteamUser && p->pFn )
		{
			p->pFn( &p->ptr );
		}
		p->counter = g_nContextCounter;
	}
	return &p->ptr;
}

S_API void *S_CALLTYPE SteamInternal_FindOrCreateUserInterface( HSteamUser hSteamUser, const char *pszVersion )
{
	if ( !g_pSteamClient || !g_hSteamPipe || !pszVersion )
		return NULL;

	if ( strncmp( pszVersion, "SteamUtils", 10 ) == 0 )
		return g_pSteamClient->GetISteamUtils( g_hSteamPipe, pszVersion );

	if ( !hSteamUser )
		return NULL;

	if ( strncmp( pszVersion, "STEAMAPPS_INTERFACE_VERSION", 27 ) == 0 )
		return g_pSteamClient->GetISteamApps( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "STEAMUSERSTATS_INTERFACE_VERSION", 32 ) == 0 )
		return g_pSteamClient->GetISteamUserStats( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "STEAMUGC_INTERFACE_VERSION", 26 ) == 0 )
		return g_pSteamClient->GetISteamUGC( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "SteamUser", 9 ) == 0 )
		return g_pSteamClient->GetISteamUser( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "SteamFriends", 12 ) == 0 )
		return g_pSteamClient->GetISteamFriends( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "STEAMREMOTESTORAGE_INTERFACE_VERSION", 36 ) == 0 )
		return g_pSteamClient->GetISteamRemoteStorage( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "STEAMSCREENSHOTS_INTERFACE_VERSION", 34 ) == 0 )
		return g_pSteamClient->GetISteamScreenshots( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "STEAMHTTP_INTERFACE_VERSION", 27 ) == 0 )
		return g_pSteamClient->GetISteamHTTP( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "SteamMatchMakingServers", 23 ) == 0 )
		return g_pSteamClient->GetISteamMatchmakingServers( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "SteamMatchMaking", 16 ) == 0 )
		return g_pSteamClient->GetISteamMatchmaking( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "SteamNetworking", 15 ) == 0 && strncmp( pszVersion, "SteamNetworking", 15 ) == 0 && pszVersion[15] >= '0' && pszVersion[15] <= '9' )
		return g_pSteamClient->GetISteamNetworking( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "SteamInput", 10 ) == 0 )
		return g_pSteamClient->GetISteamInput( hSteamUser, g_hSteamPipe, pszVersion );
	if ( strncmp( pszVersion, "SteamController", 15 ) == 0 )
		return g_pSteamClient->GetISteamController( hSteamUser, g_hSteamPipe, pszVersion );

	return g_pSteamClient->GetISteamGenericInterface( hSteamUser, g_hSteamPipe, pszVersion );
}

S_API void *S_CALLTYPE SteamInternal_FindOrCreateGameServerInterface( HSteamUser, const char * )
{
	return NULL;
}

S_API ESteamAPIInitResult S_CALLTYPE SteamInternal_GameServer_Init_V2( uint32, uint16, uint16, int, const char *, const char *, SteamErrMsg * )
{
	return k_ESteamAPIInitResult_FailedGeneric;
}

S_API void *S_CALLTYPE SteamInternal_CreateInterface( const char *ver )
{
	if ( !ver )
		return NULL;
	if ( !g_pCreateInterface && !LoadSteamClientLibrary() )
		return NULL;
	return g_pCreateInterface ? g_pCreateInterface( ver, NULL ) : NULL;
}

S_API void S_CALLTYPE SteamAPI_ManualDispatch_Init() {}
S_API void S_CALLTYPE SteamAPI_ManualDispatch_RunFrame( HSteamPipe ) {}
S_API bool S_CALLTYPE SteamAPI_ManualDispatch_GetNextCallback( HSteamPipe hSteamPipe, CallbackMsg_t *pCallbackMsg )
{
	return ( g_pSteam_BGetCallback && hSteamPipe ) ? g_pSteam_BGetCallback( hSteamPipe, pCallbackMsg ) : false;
}
S_API void S_CALLTYPE SteamAPI_ManualDispatch_FreeLastCallback( HSteamPipe hSteamPipe )
{
	if ( g_pSteam_FreeLastCallback && hSteamPipe )
		g_pSteam_FreeLastCallback( hSteamPipe );
}
S_API bool S_CALLTYPE SteamAPI_ManualDispatch_GetAPICallResult( HSteamPipe hSteamPipe, SteamAPICall_t hSteamAPICall, void *pCallback, int cubCallback, int iCallbackExpected, bool *pbFailed )
{
	return ( g_pSteam_GetAPICallResult && hSteamPipe ) ? g_pSteam_GetAPICallResult( hSteamPipe, hSteamAPICall, pCallback, cubCallback, iCallbackExpected, pbFailed ) : false;
}

S_API HSteamPipe S_CALLTYPE SteamGameServer_GetHSteamPipe() { return 0; }
S_API HSteamUser S_CALLTYPE SteamGameServer_GetHSteamUser() { return 0; }
S_API uint32 S_CALLTYPE SteamGameServer_GetIPCCallCount() { return 0; }
S_API bool S_CALLTYPE SteamGameServer_InitSafe() { return false; }
S_API void S_CALLTYPE SteamGameServer_RunCallbacks() {}
S_API void S_CALLTYPE SteamGameServer_Shutdown() {}

#else // !(OSX && PLATFORM_64BITS)

#if defined __GNUC__
 #define S_API extern "C" __attribute__ ((visibility("default"))) 
#elif defined _MSC_VER
#define S_API extern "C" __declspec(dllexport)
#endif
#define NULL 0

S_API void *g_pSteamClientGameServer;
void *g_pSteamClientGameServer = NULL;

//steam_api.h
S_API bool SteamAPI_Init() {
	return true;
}

S_API bool SteamAPI_InitSafe() {
	return true;
}

S_API void SteamAPI_Shutdown() {

}

S_API bool SteamAPI_RestartAppIfNecessary() {
	return false;
}

S_API void SteamAPI_ReleaseCurrentThreadMemory() {

}

S_API void SteamAPI_WriteMiniDump() {

}

S_API void SteamAPI_SetMiniDumpComment() {

}

S_API void SteamAPI_RunCallbacks() {
}

S_API void SteamAPI_RegisterCallback() {

}

S_API void SteamAPI_UnregisterCallback() {

}

S_API void SteamAPI_RegisterCallResult() {

}

S_API void SteamAPI_UnregisterCallResult() {

}

S_API bool SteamAPI_IsSteamRunning() {
	return false;
}

S_API void Steam_RunCallbacks() {

}

S_API void Steam_RegisterInterfaceFuncs() {
}

S_API int Steam_GetHSteamUserCurrent() {
	return 0;
}

S_API const char *SteamAPI_GetSteamInstallPath() {
	return NULL;
}

S_API int SteamAPI_GetHSteamPipe() {
	return 0;
}

S_API void SteamAPI_SetTryCatchCallbacks() {

}

S_API void SteamAPI_SetBreakpadAppID() {

}

S_API void SteamAPI_UseBreakpadCrashHandler() {

}

S_API int GetHSteamPipe() {
	return 0;
}

S_API int GetHSteamUser() {
	return 0;
}

S_API int SteamAPI_GetHSteamUser() {
	return 0;
}

// Steamworks 1.4x+ accessors read the interface pointer from slot 2 of the
// caller's context; it stays NULL here, so SteamInput() and friends return NULL.
S_API void *SteamInternal_ContextInit( void *pContextInitData ) {
	return (void **)pContextInitData + 2;
}

S_API void *SteamInternal_FindOrCreateUserInterface( int hSteamUser, const char *pszVersion ) {
	return NULL;
}

// ESteamAPIInitResult: k_ESteamAPIInitResult_FailedGeneric
S_API int SteamInternal_SteamAPI_Init( const char *pszInternalCheckInterfaceVersions, void *pOutErrMsg ) {
	return 1;
}

S_API void *SteamInternal_FindOrCreateGameServerInterface( int hSteamUser, const char *pszVersion ) {
	return NULL;
}

// ESteamAPIInitResult: k_ESteamAPIInitResult_FailedGeneric
S_API int SteamInternal_GameServer_Init_V2( unsigned int unIP, unsigned short usGamePort, unsigned short usQueryPort, int eServerMode, const char *pchVersionString, const char *pszInternalCheckInterfaceVersions, void *pOutErrMsg ) {
	return 1;
}

S_API void *SteamInternal_CreateInterface() {
	return NULL;
}

S_API void *SteamApps() {
	return NULL;
}

S_API void *SteamClient() {
	return NULL;
}

S_API void *SteamFriends() {
	return NULL;
}

S_API void *SteamHTTP() {
	return NULL;
}

S_API void *SteamMatchmaking() {
	return NULL;
}

S_API void *SteamMatchmakingServers() {
	return NULL;
}

S_API void *SteamNetworking() {
	return NULL;
}

S_API void *SteamRemoteStorage() {
	return NULL;
}

S_API void *SteamScreenshots() {
	return NULL;
}

S_API void *SteamUser() {
	return NULL;
}

S_API void *SteamUserStats() {
	return NULL;
}

S_API void *SteamUtils() {
	return NULL;
}

S_API int SteamGameServer_GetHSteamPipe() {
	return 0;
}

S_API int SteamGameServer_GetHSteamUser() {
	return 0;
}

S_API int SteamGameServer_GetIPCCallCount() {
	return 0;
}

S_API int SteamGameServer_InitSafe() {
	return 0;
}

S_API void SteamGameServer_RunCallbacks() {
}

S_API void SteamGameServer_Shutdown() {
}

#endif // !(OSX && PLATFORM_64BITS)
