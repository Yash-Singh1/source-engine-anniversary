/* Stand-ins for the loader functions of RAD Telemetry's telemetry32.link, which
   tier0 links statically and which isn't available. They report that Telemetry
   can't start, so TelemetryInitialize() gives up before using a context. Built by
   scripts/waifulib/retail_win32.py against telemetry/include/telemetry.h. */
#include "telemetry.h"

int RADEXPLINK tmLoadTelemetryEx( int major, int minor, int build, int cust, int checked )
{
	return 0;
}

TmErrorCode RADEXPLINK tmStartup( void )
{
	return TMERR_DISABLED;
}

TmErrorCode RADEXPLINK tmShutdown( void )
{
	return TM_OK;
}

TmErrorCode RADEXPLINK tmInitializeContext( HTELEMETRY *pcx, void *arena, TmU32 size )
{
	if ( pcx )
		*pcx = 0;
	return TMERR_DISABLED;
}
