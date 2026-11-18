//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The subset of the Miles Sound System API (MSS 9, 32-bit) that
//			vaudio_miles uses. This is not RAD's header; the Miles libraries
//			themselves are user-provided (libMilesX86.dylib, mss32.dll).
//
//=============================================================================//

#ifndef MSS_H
#define MSS_H
#ifdef _WIN32
#pragma once
#endif

typedef char				C8;
typedef signed int			S32;
typedef unsigned int		U32;
typedef signed int			SINTa;
typedef unsigned int		UINTa;

#ifdef _WIN32
#define AILCALL				__stdcall
#define AILCALLBACK			__stdcall
#define AILEXPORT			__declspec( dllimport ) __stdcall
#else
#define AILCALL
#define AILCALLBACK
#define AILEXPORT
#endif

#define MSS_REDIST_DIR_NAME	"redist"

typedef SINTa				HPROVIDER;
typedef UINTa				HPROPERTY;
typedef SINTa				HASISTREAM;
typedef S32					RIBRESULT;
typedef S32					ASIRESULT;
typedef struct _DIG_DRIVER	*HDIGDRIVER;

#define RIB_NOERR			0

typedef enum
{
	RIB_FUNCTION = 0,
	RIB_PROPERTY
} RIB_ENTRY_TYPE;

typedef enum
{
	RIB_NONE = 0,
	RIB_CUSTOM,
	RIB_DEC,
	RIB_HEX,
	RIB_FLOAT,
	RIB_PERCENT,
	RIB_BOOL,
	RIB_STRING,
	RIB_READONLY = 0x80000000
} RIB_DATA_SUBTYPE;

typedef struct
{
	RIB_ENTRY_TYPE		type;
	const C8			*entry_name;
	UINTa				token;
	RIB_DATA_SUBTYPE	subtype;
} RIB_INTERFACE_ENTRY;

#define FN( entry_name )		{ RIB_FUNCTION, #entry_name, (UINTa) &(entry_name), RIB_NONE }
#define PR( entry_name, ID )	{ RIB_PROPERTY, (entry_name), (UINTa) &(ID), RIB_NONE }

#define RIB_request( provider, name, list ) \
	RIB_request_interface( provider, name, sizeof( list ) / sizeof( *list ), list )

typedef void * AILCALLBACK MSS_ALLOC_TYPE( UINTa size, UINTa user, char const *filename, U32 line );
typedef void AILCALLBACK MSS_FREE_TYPE( void *ptr, UINTa user, char const *filename, U32 line );

typedef S32 ( AILCALLBACK *AILASIFETCHCB )( UINTa user, void *dest, S32 bytes_requested, S32 offset );

typedef HASISTREAM ( AILCALL *ASI_STREAM_OPEN )( MSS_ALLOC_TYPE *palloc, MSS_FREE_TYPE *pfree, UINTa user,
	AILASIFETCHCB fetch_CB, U32 total_size );
typedef S32 ( AILCALL *ASI_STREAM_PROCESS )( HASISTREAM stream, void *buffer, S32 buffer_size );
typedef ASIRESULT ( AILCALL *ASI_STREAM_SEEK )( HASISTREAM stream, S32 stream_offset );
typedef ASIRESULT ( AILCALL *ASI_STREAM_CLOSE )( HASISTREAM stream );
typedef S32 ( AILCALL *ASI_STREAM_PROPERTY )( HASISTREAM stream, HPROPERTY property, void *before_value,
	void const *new_value, void *after_value );

// On 32-bit OS X, Miles calls back with a 4-byte aligned stack. Callbacks are
// registered through a thunk that realigns it to 16 bytes and forwards the four
// stack arguments.
//
// Like RAD's macro, MSS_DEF_CB_STACK_ALIGN defines two out-of-line functions per
// callback: name##_fixup, the realigning thunk that is handed to Miles, and
// name##_fixup_noalign, an entry for callers whose stack is already aligned,
// which jumps straight to the callback. Nothing here takes the second one, so
// the linker strips it, but it must still be defined: retail's
// voice_codec_miles.o numbers its GCC_except_table labels with one function per
// use beyond the thunk. RAD's own name and body for it are not known.
#if defined( __APPLE__ ) && defined( __i386__ )
#define MSS_CALLBACK_ALIGNED_NAME( name )	name##_fixup
#define MSS_DEC_CB_STACK_ALIGN( name )		void * name##_fixup( void );
#define MSS_DEF_CB_STACK_ALIGN( name )										\
	void * name##_fixup( void )												\
	{																		\
		__asm__ __volatile__ (												\
			"movl $16, %%eax\n\t"											\
			"subl $64, %%esp\n\t"											\
			"addl %%esp, %%eax\n\t"											\
			"andl $-16, %%eax\n\t"											\
			"movups 8(%%ebp), %%xmm0\n\t"									\
			"movaps %%xmm0, (%%eax)\n\t"									\
			"movl %%esp, 32(%%eax)\n\t"										\
			"movl %%eax, %%esp\n\t"											\
			"call *%0\n\t"													\
			"movl 32(%%esp), %%esp\n\t"										\
			"addl $64, %%esp\n\t"											\
			"popl %%ebp\n\t"												\
			"ret\n\t"														\
			: : "c" ( name ) );												\
		__builtin_unreachable();											\
	}																		\
	void * name##_fixup_noalign( void )										\
	{																		\
		__asm__ __volatile__ (												\
			"popl %%ebp\n\t"												\
			"jmp *%0\n\t"													\
			: : "c" ( name ) );												\
		__builtin_unreachable();											\
	}
#else
#define MSS_CALLBACK_ALIGNED_NAME( name )	name
#define MSS_DEC_CB_STACK_ALIGN( name )
#define MSS_DEF_CB_STACK_ALIGN( name )
#endif

#ifdef __cplusplus
extern "C" {
#endif

MSS_DEC_CB_STACK_ALIGN( MSS_alloc_info )
MSS_DEC_CB_STACK_ALIGN( MSS_free_info )
void * AILCALLBACK MSS_alloc_info( UINTa size, UINTa user, char const *filename, U32 line );
void AILCALLBACK MSS_free_info( void *ptr, UINTa user, char const *filename, U32 line );

S32 AILEXPORT AIL_startup( void );
void AILEXPORT AIL_shutdown( void );
char * AILEXPORT AIL_set_redist_directory( char const *dir );

HPROVIDER AILEXPORT RIB_find_files_provider( C8 const *interface_name, C8 const *property_name_1,
	C8 const *file_suffix_1, C8 const *property_name_2, C8 const *file_suffix_2 );
RIBRESULT AILEXPORT RIB_request_interface( HPROVIDER provider, C8 const *interface_name, S32 entry_count,
	RIB_INTERFACE_ENTRY *list );
void AILEXPORT RIB_free_provider_library( HPROVIDER provider );

#ifdef __cplusplus
}
#endif

#endif // MSS_H
