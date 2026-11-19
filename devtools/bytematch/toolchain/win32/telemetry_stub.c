/* Stand-ins for the loader functions of RAD Telemetry's telemetry32.link, which
   tier0 links statically and which isn't available. They report that Telemetry
   can't start, so TelemetryInitialize() gives up before using a context. Built by
   scripts/waifulib/retail_win32.py against hl2_src's telemetry.h. */
#include "telemetry.h"

int RADEXPLINK tmLoadTelemetryEx( int const major, int const minor, int const build, int const cust, int const kUseCheckedDLL )
{
	return 0;
}

TmErrorCode RADEXPLINK tmStartup( void )
{
	return TMERR_DISABLED;
}

TmErrorCode RADEXPLINK tmStartupEx( TMUSERTIMERFNC *fnc )
{
	return TMERR_DISABLED;
}

TmErrorCode RADEXPLINK tmShutdown( void )
{
	return TM_OK;
}

TmU32 RADEXPLINK tmGetVersion( void )
{
	return 0;
}

TmErrorCode RADEXPLINK tmInitializeContext( HTELEMETRY *pcx, void *pArena, TmU32 const kArenaSize )
{
	if ( pcx )
		*pcx = 0;
	return TMERR_DISABLED;
}

TmErrorCode RADEXPLINK tmGetPlatformInformation( void *obj, TmPlatformInformation const kInfo, void *dst, TmU32 const kSize )
{
	return TMERR_DISABLED;
}
