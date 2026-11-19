/* RAD Telemetry declarations for --retail-win32.

   Written for this tree from the retail Windows binaries (depot 222) and RAD's public
   changelog; no RAD header text. Valve's rel_singleplayer modules were built against
   Telemetry 1.1 build 25: tier0's TelemetryInit calls tmLoadTelemetryEx( 1, 1, 25, 0, 0 ).
   Retail calls every entry through the context's function table at the same offsets as
   later 2.0 releases, with three older call shapes:
   - tmCoreMessage has no thread id: ( cx, flags, &fmt, format, ... );
   - tmCorePlot* have no timestamp: ( cx, type, flags, value, &fmt, name, ... );
   - tmCoreSendCallStack has no frame skip: ( cx, callstack ).
   Entries retail never calls keep the 2.0 shapes. Functions are __stdcall, so the
   variadic ones are caller-cleaned and the others callee-cleaned, as retail shows. */
#ifndef TELEMETRY_H
#define TELEMETRY_H

#define TelemetryMajorVersion	1
#define TelemetryMinorVersion	1
#define TelemetryBuildNumber	25
#define TelemetryCustomization	0

#define RADEXPLINK __stdcall

#ifdef __cplusplus
#define TM_EXTERN_C extern "C"
#else
#define TM_EXTERN_C extern
#endif

typedef signed char			TmI8;
typedef unsigned char		TmU8;
typedef signed short		TmI16;
typedef unsigned short		TmU16;
typedef signed int			TmI32;
typedef unsigned int		TmU32;
typedef float				TmF32;
typedef double				TmF64;
typedef signed __int64		TmI64;
typedef unsigned __int64	TmU64;
typedef unsigned int		TmIntPtr;

/* Per call site; the run-time caches the parsed format here. */
typedef struct _TmFormatCode
{
	TmU32 fc_format;
	TmU32 fc_fbits;
	char const *fc_ptr;
} TmFormatCode;

#define TM_MAX_CALLSTACK_DEPTH 32
typedef struct _TmCallStack
{
	int cs_depth;
	void *cs_stack[ TM_MAX_CALLSTACK_DEPTH ];
} TmCallStack;

typedef struct TmContext *HTELEMETRY;

#define TM_MAX_STRING				( 1024 * 8 )
#define TM_LOCK_MIN_TIME_BUFSIZE	512
#define TELEMETRY_DEFAULT_PORT		0
#define TMOF_DEFAULT				0

typedef enum TmErrorCode
{
	TM_OK = 0,
	TMERR_DISABLED = 1,
	TMERR_INVALID_CONTEXT = 2,
	TMERR_INVALID_PARAM = 3,
	TMERR_OUT_OF_RESOURCES = 4,
	TMERR_UNINITIALIZED = 6,
	TMERR_BAD_HOSTNAME = 7,
	TMERR_COULD_NOT_CONNECT = 8,
	TMERR_UNKNOWN_NETWORK = 9,
	TMERR_ALREADY_SHUTDOWN = 10,
	TMERR_ARENA_TOO_SMALL = 11,
	TMERR_BAD_HANDSHAKE = 12,
	TMERR_UNALIGNED = 13,
	TMERR_NETWORK_NOT_INITIALIZED = 14,
	TMERR_BAD_VERSION = 15,
	TMERR_UNKNOWN = 0xFFFF
} TmErrorCode;

typedef enum TmPrintfToken
{
	TMPRINTF_TOKEN_NONE = 0,
	TMPRINTF_TOKEN_CALLSTACK = 1
} TmPrintfToken;

typedef enum TmConnectionStatus
{
	TMCS_DISCONNECTED = 0,
	TMCS_CONNECTING = 1,
	TMCS_CONNECTED = 2
} TmConnectionStatus;

typedef enum TmZoneFlag
{
	TMZF_NONE = 0x0000,
	TMZF_STALL = 0x0001,
	TMZF_IDLE = 0x0002,
	TMZF_PROFILER_ONLY = 0x0004,
	TMZF_PLOT_TIME = 0x0100
} TmZoneFlag;

typedef enum TmPlotType
{
	TMPT_NONE = 0,
	TMPT_MEMORY = 1,
	TMPT_HEX = 2,
	TMPT_INTEGER = 3,
	TMPT_PERCENTAGE_COMPUTED = 4,
	TMPT_PERCENTAGE_DIRECT = 5,
	TMPT_TIME = 6,
	TMPT_TIME_MS = 7,
	TMPT_TIME_US = 8,
	TMPT_TIME_CLOCKS = 9,
	TMPT_TIME_INTERVAL = 10,
	TMPT_USER = 128,
	TMPT_MAX = 255
} TmPlotType;

typedef enum TmMessageFlag
{
	TMMF_SEVERITY_LOG = 0x0001,
	TMMF_SEVERITY_WARNING = 0x0002,
	TMMF_SEVERITY_ERROR = 0x0004,
	TMMF_ZONE_LABEL = 0x0010,
	TMMF_ZONE_SUBLABEL = 0x0020,
	TMMF_ZONE_SHOW_IN_PARENTS = 0x0040,
	TMMF_ICON_EXCLAMATION = 0x1000,
	TMMF_ICON_NOTE = 0x2000,
	TMMF_ICON_WTF = 0x3000,
	TMMF_ICON_QUESTION_MARK = 0x3000
} TmMessageFlag;

typedef enum TmOpenFlag
{
	TMOF_INIT_NETWORKING = 0x0001,
	TMOF_DONT_KILL_OTHER_SESSIONS = 0x0002,
	TMOF_DISABLE_CONTEXT_SWITCHES = 0x0000,
	TMOF_MODERATE_CONTEXT_SWITCHES = 0x0010,
	TMOF_MINIMAL_CONTEXT_SWITCHES = 0x0020,
	TMOF_MAXIMUM_CONTEXT_SWITCHES = 0x0030
} TmOpenFlag;

typedef enum TmConnectionType
{
	TMCT_TCP,
	TMCT_IPC,
	TMCT_FILE
} TmConnectionType;

typedef enum TmOption
{
	TMO_OUTPUT_DEBUG_INFO = 0x0001,
	TMO_RECORD_TELEMETRY_STALLS = 0x0002,
	TMO_RECORD_CALLSTACKS = 0x0004,
	TMO_SUPPORT_ZONES = 0x0100,
	TMO_SUPPORT_MESSAGES = 0x0200,
	TMO_SUPPORT_LOCK_STATES = 0x0400,
	TMO_SUPPORT_MEMORY = 0x0800,
	TMO_SUPPORT_PLOT = 0x1000,
	TMO_SUPPORT_BLOB = 0x2000,
	TMO_SUPPORT_CONTEXT_SWITCHES = 0x8000,
	TMO_NULL_NETWORK = 0x10000,
	TMO_SUPPORT_TIMESPANS = 0x20000
} TmOption;

typedef enum TmParameter
{
	TMP_NONE = 0,
	TMP_LOCK_MIN_TIME = 1,
	TMP_DEBUG_PRINTER = 2,
	TMP_TELEMETRY_PREFERRED_CPU = 3
} TmParameter;

typedef enum TmServerConfiguration
{
	TMSC_MEM_PLOT_THRESHOLD = 0
} TmServerConfiguration;

typedef enum TmLockState
{
	TMLS_RELEASED = 0x0001,
	TMLS_LOCKED = 0x0002,
	TMLS_DESTROYED = 0x0004
} TmLockState;

typedef enum TmLockResult
{
	TMLR_SUCCESS = 0,
	TMLR_FAILED = 1,
	TMLR_TIMEOUT = 2
} TmLockResult;

typedef enum TmStat
{
	TMSTAT_NUM_ALLOCS,
	TMSTAT_NUM_FREES,
	TMSTAT_NUM_TICKS
} TmStat;

/* The context starts with this table. Offsets are retail's; the order must not change. */
typedef struct TmApi
{
	TmErrorCode ( RADEXPLINK *tmCoreCheckVersion )( HTELEMETRY cx, TmU32 major, TmU32 minor, TmU32 build, TmU32 cust );	/* 0x00 */
	void ( RADEXPLINK *tmCoreUpdateSymbolData )( HTELEMETRY cx );
	TmU64 ( RADEXPLINK *tmCoreGetLastContextSwitchTime )( HTELEMETRY cx );
	void ( RADEXPLINK *tmCoreTick )( HTELEMETRY cx );													/* 0x0c */
	void ( RADEXPLINK *tmCoreFlush )( HTELEMETRY cx );
	char const *( RADEXPLINK *tmCoreDynamicString )( HTELEMETRY cx, char const *s );						/* 0x14 */
	void ( RADEXPLINK *tmCoreClearStaticString )( HTELEMETRY cx, char const *s );
	void ( RADEXPLINK *tmCoreSetVariable )( HTELEMETRY cx, char const *key, TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCoreGetFormatCode )( TmFormatCode *fc, char const *fmt );
	TmErrorCode ( RADEXPLINK *tmCoreGetSessionName )( HTELEMETRY cx, char *dst, int size );
	TmErrorCode ( RADEXPLINK *tmCoreGetLastError )( HTELEMETRY cx );
	void ( RADEXPLINK *tmCoreShutdownContext )( HTELEMETRY cx );
	TmConnectionStatus ( RADEXPLINK *tmCoreGetConnectionStatus )( HTELEMETRY cx );						/* 0x30 */
	void ( RADEXPLINK *tmCoreSetTimelineSectionName )( HTELEMETRY cx, TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCoreEnable )( HTELEMETRY cx, TmOption option, int value );
	int ( RADEXPLINK *tmCoreIsEnabled )( HTELEMETRY cx, TmOption option );
	TmErrorCode ( RADEXPLINK *tmCoreOpen )( HTELEMETRY cx, char const *app, char const *build, char const *server,
		TmConnectionType type, TmU16 port, TmU32 flags, int timeout_ms );									/* 0x40 */
	void ( RADEXPLINK *tmCoreClose )( HTELEMETRY cx );
	void ( RADEXPLINK *tmCorePause )( HTELEMETRY cx, int pause );											/* 0x48 */
	int ( RADEXPLINK *tmCoreIsPaused )( HTELEMETRY cx );
	void ( RADEXPLINK *tmCoreEnter )( HTELEMETRY cx, TmU64 *matchid, TmU32 tid, TmU64 threshold, TmU32 flags,
		char const *file, TmU32 line, TmFormatCode *fc, char const *fmt, ... );								/* 0x50 */
	void ( RADEXPLINK *tmCoreLeave )( HTELEMETRY cx, TmU64 matchid, TmU32 tid, char const *file, int line );	/* 0x54 */
	void ( RADEXPLINK *tmCoreThreadName )( HTELEMETRY cx, TmU32 tid, TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCoreLockName )( HTELEMETRY cx, void const *ptr, TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCoreTryLock )( HTELEMETRY cx, TmU64 *matchid, TmU64 threshold, char const *file, TmU32 line,
		void const *ptr, TmFormatCode *fc, char const *fmt, ... );											/* 0x60 */
	void ( RADEXPLINK *tmCoreEndTryLock )( HTELEMETRY cx, TmU64 matchid, char const *file, int line, TmFormatCode *fc,
		void const *ptr, TmLockResult result );																/* 0x64 */
	void ( RADEXPLINK *tmCoreSignalLockCount )( HTELEMETRY cx, char const *file, TmU32 line, void const *ptr, TmU32 count,
		TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCoreSetLockState )( HTELEMETRY cx, void const *ptr, TmLockState state, char const *file, TmU32 line,
		TmFormatCode *fc, char const *fmt, ... );															/* 0x6c */
	void ( RADEXPLINK *tmCoreAlloc )( HTELEMETRY cx, void const *ptr, TmU64 size, char const *file, TmU32 line,
		TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCoreFree )( HTELEMETRY cx, void const *ptr, char const *file, int line, TmFormatCode *fc );
	TmI32 ( RADEXPLINK *tmCoreGetStati )( HTELEMETRY cx, TmStat stat );
	void ( RADEXPLINK *tmCoreBeginTimeSpan )( HTELEMETRY cx, TmU64 id, TmU32 flags, TmU64 time, char const *file, TmU32 line,
		TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCoreEndTimeSpan )( HTELEMETRY cx, TmU64 id, TmU32 flags, TmU64 time, char const *file, TmU32 line,
		TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCorePlot )( HTELEMETRY cx, TmPlotType type, TmU32 flags, float value, TmFormatCode *fc, char const *fmt, ... );	/* 0x84 */
	void ( RADEXPLINK *tmCorePlotI32 )( HTELEMETRY cx, TmPlotType type, TmU32 flags, TmI32 value, TmFormatCode *fc, char const *fmt, ... );	/* 0x88 */
	void ( RADEXPLINK *tmCorePlotU32 )( HTELEMETRY cx, TmPlotType type, TmU32 flags, TmU32 value, TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCorePlotI64 )( HTELEMETRY cx, TmPlotType type, TmU32 flags, TmI64 value, TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCorePlotU64 )( HTELEMETRY cx, TmPlotType type, TmU32 flags, TmU64 value, TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCorePlotF64 )( HTELEMETRY cx, TmPlotType type, TmU32 flags, double value, TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCoreBlob )( HTELEMETRY cx, void const *data, int size, char const *plugin, TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCoreDisjointBlob )( HTELEMETRY cx, int pieces, void const **data, int const *sizes, char const *plugin,
		TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCoreMessage )( HTELEMETRY cx, TmU32 flags, TmFormatCode *fc, char const *fmt, ... );	/* 0xa4 */
	int ( RADEXPLINK *tmCoreSendCallStack )( HTELEMETRY cx, TmCallStack const *cs );						/* 0xa8 */
	int ( RADEXPLINK *tmCoreGetCallStack )( HTELEMETRY cx, TmCallStack *cs );
	void ( RADEXPLINK *tmCoreSetDebugZoneLevel )( HTELEMETRY cx, int level );
	void ( RADEXPLINK *tmCoreCheckDebugZoneLevel )( HTELEMETRY cx, int level );
	void ( RADEXPLINK *tmCoreUnwindToDebugZoneLevel )( HTELEMETRY cx, int level );
	void ( RADEXPLINK *tmCoreEmitAccumulationZone )( HTELEMETRY cx, TmU64 *accum, TmU64 total, TmU32 count, TmU32 flags,
		char const *file, TmU32 line, TmFormatCode *fc, char const *fmt, ... );
	int ( RADEXPLINK *tmCoreSetLockStateMinTime )( HTELEMETRY cx, void *buf, void const *ptr, TmLockState state, char const *file,
		TmU32 line, TmFormatCode *fc, char const *fmt, ... );
	void ( RADEXPLINK *tmCoreSetParameter )( HTELEMETRY cx, TmParameter param, void const *value );
} TmApi;

/* Loader functions, linked statically into tier0 (telemetry32.link). */
TM_EXTERN_C int RADEXPLINK tmLoadTelemetryEx( int major, int minor, int build, int cust, int checked );
TM_EXTERN_C TmErrorCode RADEXPLINK tmStartup( void );
TM_EXTERN_C TmErrorCode RADEXPLINK tmShutdown( void );
TM_EXTERN_C TmErrorCode RADEXPLINK tmInitializeContext( HTELEMETRY *pcx, void *arena, TmU32 size );
#define tmLoadTelemetry( checked ) tmLoadTelemetryEx( TelemetryMajorVersion, TelemetryMinorVersion, TelemetryBuildNumber, TelemetryCustomization, checked )

TM_EXTERN_C unsigned __int64 __rdtsc( void );
#pragma intrinsic( __rdtsc )
#define tmFastTime() __rdtsc()

/* Every call checks the context; variadic calls own a format-code static per site. */
#define TM_API_V( cx, fn, args )		{ if ( cx ) { ( ( (TmApi *)( cx ) )->fn args ); } }
#define TM_API_R( cx, fn, args, z )		( ( cx ) ? ( ( (TmApi *)( cx ) )->fn args ) : ( z ) )
#define TM_API_F( cx, fn, args )		{ static TmFormatCode tm_fmt; TM_API_V( cx, fn, args ); }

#define tmCheckVersion( cx, ma, mi, b, c )	TM_API_R( cx, tmCoreCheckVersion, ( cx, ma, mi, b, c ), 0 )
#define tmUpdateSymbolData( cx )			TM_API_V( cx, tmCoreUpdateSymbolData, ( cx ) )
#define tmGetLastContextSwitchTime( cx )	TM_API_R( cx, tmCoreGetLastContextSwitchTime, ( cx ), 0 )
#define tmTick( cx )						TM_API_V( cx, tmCoreTick, ( cx ) )
#define tmFlush( cx )						TM_API_V( cx, tmCoreFlush, ( cx ) )
#define tmDynamicString( cx, s )			TM_API_R( cx, tmCoreDynamicString, ( cx, s ), 0 )
#define tmClearStaticString( cx, s )		TM_API_V( cx, tmCoreClearStaticString, ( cx, s ) )
#define tmSetVariable( cx, k, v, ... )		TM_API_F( cx, tmCoreSetVariable, ( cx, k, &tm_fmt, v, ##__VA_ARGS__ ) )
#define tmGetFormatCode( cx, s, t )			TM_API_V( cx, tmCoreGetFormatCode, ( s, t ) )
#define tmGetSessionName( cx, d, n )		TM_API_R( cx, tmCoreGetSessionName, ( cx, d, n ), TMERR_DISABLED )
#define tmGetLastError( cx )				TM_API_R( cx, tmCoreGetLastError, ( cx ), TMERR_INVALID_CONTEXT )
#define tmShutdownContext( cx )				TM_API_V( cx, tmCoreShutdownContext, ( cx ) )
#define tmGetConnectionStatus( cx )			TM_API_R( cx, tmCoreGetConnectionStatus, ( cx ), TMCS_DISCONNECTED )
#define tmSetTimelineSectionName( cx, n, ... )	TM_API_F( cx, tmCoreSetTimelineSectionName, ( cx, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmEnable( cx, o, v )				TM_API_V( cx, tmCoreEnable, ( cx, o, v ) )
#define tmIsEnabled( cx, o )				TM_API_R( cx, tmCoreIsEnabled, ( cx, o ), TMERR_INVALID_CONTEXT )
#define tmOpen( cx, app, b, srv, t, port, f, ms )	TM_API_R( cx, tmCoreOpen, ( cx, app, b, srv, t, port, f, ms ), TMERR_INVALID_CONTEXT )
#define tmClose( cx )						TM_API_V( cx, tmCoreClose, ( cx ) )
#define tmPause( cx, p )					TM_API_V( cx, tmCorePause, ( cx, p ) )
#define tmIsPaused( cx )					TM_API_R( cx, tmCoreIsPaused, ( cx ), 0 )
#define tmGetStati( cx, s )					TM_API_R( cx, tmCoreGetStati, ( cx, s ), TMERR_INVALID_CONTEXT )

#define tmEnter( cx, f, n, ... )			TM_API_F( cx, tmCoreEnter, ( cx, 0, 0, 0, f, __FILE__, __LINE__, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmEnterEx( cx, m, tid, th, file, line, f, n, ... )	TM_API_F( cx, tmCoreEnter, ( cx, m, tid, th, f, file, line, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmLeave( cx )						TM_API_V( cx, tmCoreLeave, ( cx, 0, 0, __FILE__, __LINE__ ) )
#define tmLeaveEx( cx, m, tid, file, line )	TM_API_V( cx, tmCoreLeave, ( cx, m, tid, file, line ) )

#define tmThreadName( cx, tid, n, ... )		TM_API_F( cx, tmCoreThreadName, ( cx, tid, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmLockName( cx, p, n, ... )			TM_API_F( cx, tmCoreLockName, ( cx, p, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmTryLock( cx, p, n, ... )			TM_API_F( cx, tmCoreTryLock, ( cx, 0, 0, __FILE__, __LINE__, p, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmTryLockEx( cx, m, th, file, line, p, n, ... )	TM_API_F( cx, tmCoreTryLock, ( cx, m, th, file, line, p, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmEndTryLock( cx, p, r )			TM_API_F( cx, tmCoreEndTryLock, ( cx, 0, __FILE__, __LINE__, &tm_fmt, p, r ) )
#define tmEndTryLockEx( cx, m, file, line, p, r )	TM_API_F( cx, tmCoreEndTryLock, ( cx, m, file, line, &tm_fmt, p, r ) )
#define tmSignalLockCount( cx, p, c, n, ... )	TM_API_F( cx, tmCoreSignalLockCount, ( cx, __FILE__, __LINE__, p, c, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmSetLockState( cx, p, s, n, ... )	TM_API_F( cx, tmCoreSetLockState, ( cx, p, s, __FILE__, __LINE__, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmSetLockStateEx( cx, file, line, p, s, n, ... )	TM_API_F( cx, tmCoreSetLockState, ( cx, p, s, file, line, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmSetLockStateMinTime( cx, b, p, s, n, ... )	TM_API_F( cx, tmCoreSetLockStateMinTime, ( cx, b, p, s, __FILE__, __LINE__, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmSetLockStateMinTimeEx( cx, b, file, line, p, s, n, ... )	TM_API_F( cx, tmCoreSetLockStateMinTime, ( cx, b, p, s, file, line, &tm_fmt, n, ##__VA_ARGS__ ) )

#define tmAlloc( cx, p, sz, n, ... )		TM_API_F( cx, tmCoreAlloc, ( cx, p, sz, __FILE__, __LINE__, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmAllocEx( cx, file, line, p, sz, n, ... )	TM_API_F( cx, tmCoreAlloc, ( cx, p, sz, file, line, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmFree( cx, p )						TM_API_F( cx, tmCoreFree, ( cx, p, __FILE__, __LINE__, &tm_fmt ) )

#define tmBeginTimeSpan( cx, id, f, n, ... )	TM_API_F( cx, tmCoreBeginTimeSpan, ( cx, id, f, 0, __FILE__, __LINE__, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmEndTimeSpan( cx, id, f, n, ... )		TM_API_F( cx, tmCoreEndTimeSpan, ( cx, id, f, 0, __FILE__, __LINE__, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmBeginTimeSpanAt( cx, id, f, t, n, ... )	TM_API_F( cx, tmCoreBeginTimeSpan, ( cx, id, f, t, __FILE__, __LINE__, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmEndTimeSpanAt( cx, id, f, t, n, ... )		TM_API_F( cx, tmCoreEndTimeSpan, ( cx, id, f, t, __FILE__, __LINE__, &tm_fmt, n, ##__VA_ARGS__ ) )

#define tmPlot( cx, t, f, v, n, ... )		TM_API_F( cx, tmCorePlot, ( cx, t, f, v, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmPlotF32( cx, t, f, v, n, ... )	TM_API_F( cx, tmCorePlot, ( cx, t, f, v, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmPlotF64( cx, t, f, v, n, ... )	TM_API_F( cx, tmCorePlotF64, ( cx, t, f, v, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmPlotI32( cx, t, f, v, n, ... )	TM_API_F( cx, tmCorePlotI32, ( cx, t, f, v, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmPlotU32( cx, t, f, v, n, ... )	TM_API_F( cx, tmCorePlotU32, ( cx, t, f, v, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmPlotI64( cx, t, f, v, n, ... )	TM_API_F( cx, tmCorePlotI64, ( cx, t, f, v, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmPlotU64( cx, t, f, v, n, ... )	TM_API_F( cx, tmCorePlotU64, ( cx, t, f, v, &tm_fmt, n, ##__VA_ARGS__ ) )

#define tmBlob( cx, d, sz, plugin, n, ... )	TM_API_F( cx, tmCoreBlob, ( cx, d, sz, plugin, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmDisjointBlob( cx, k, d, sz, plugin, n, ... )	TM_API_F( cx, tmCoreDisjointBlob, ( cx, k, d, sz, plugin, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmMessage( cx, f, s, ... )			TM_API_F( cx, tmCoreMessage, ( cx, f, &tm_fmt, s, ##__VA_ARGS__ ) )

#define tmSendCallStack( cx, cs )			TM_API_R( cx, tmCoreSendCallStack, ( cx, cs ), TMPRINTF_TOKEN_NONE )
#define tmGetCallStack( cx, cs )			TM_API_R( cx, tmCoreGetCallStack, ( cx, cs ), 0 )

#define tmSetDebugZoneLevel( cx, n )		TM_API_V( cx, tmCoreSetDebugZoneLevel, ( cx, n ) )
#define tmCheckDebugZoneLevel( cx, n )		TM_API_V( cx, tmCoreCheckDebugZoneLevel, ( cx, n ) )
#define tmUnwindToDebugZoneLevel( cx, n )	TM_API_V( cx, tmCoreUnwindToDebugZoneLevel, ( cx, n ) )

#define tmGetAccumulationStart( cx )		( ( cx ) ? tmFastTime() : 0 )
#define tmEnterAccumulationZone( cx, z )	{ if ( cx ) ( z ) -= tmFastTime(); }
#define tmLeaveAccumulationZone( cx, z )	{ if ( cx ) ( z ) += tmFastTime(); }
#define tmEmitAccumulationZone( cx, f, acc, cnt, total, n, ... )	TM_API_F( cx, tmCoreEmitAccumulationZone, ( cx, acc, total, cnt, f, __FILE__, __LINE__, &tm_fmt, n, ##__VA_ARGS__ ) )
#define tmSetParameter( cx, p, v )			TM_API_V( cx, tmCoreSetParameter, ( cx, p, v ) )

#ifdef __cplusplus

#define TM_LINE_NAME3( name, line )		name##line
#define TM_LINE_NAME2( name, line )		TM_LINE_NAME3( name, line )
#define TM_LINE_NAME( name )			TM_LINE_NAME2( name, __LINE__ )

/* Scoped zone: enters now, leaves with the matching id when the scope ends. */
class Tm_Zone
{
public:
	inline Tm_Zone( HTELEMETRY cx, TmU64 const id ) { z_cx = cx; z_id = id; }
	inline ~Tm_Zone() { TM_API_V( z_cx, tmCoreLeave, ( z_cx, z_id, 0, 0, 0 ) ); }

private:
	TmU64 z_id;
	HTELEMETRY z_cx;
};

#define tmZoneFiltered( cx, threshold, f, n, ... ) TmU64 TM_LINE_NAME( tm_id ) = 0; \
	TM_API_F( cx, tmCoreEnter, ( cx, &( TM_LINE_NAME( tm_id ) ), 0, threshold, ( f ), __FILE__, __LINE__, &tm_fmt, n, ##__VA_ARGS__ ) ); \
	Tm_Zone TM_LINE_NAME( tm_zone )( cx, TM_LINE_NAME( tm_id ) );
#define tmZone( cx, f, n, ... )			tmZoneFiltered( cx, 0, f, n, ##__VA_ARGS__ )

#endif /* __cplusplus */

#endif /* TELEMETRY_H */
