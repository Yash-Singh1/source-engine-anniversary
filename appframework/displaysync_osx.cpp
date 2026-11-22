//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Vsync pacing for the OpenGL game window on macOS, locked to the
//			display the window is on.
//
//===========================================================================//

#include <CoreVideo/CoreVideo.h>
#include <ApplicationServices/ApplicationServices.h>
#include <pthread.h>
#include <time.h>
#include "SDL.h"

static pthread_mutex_t s_DisplaySyncMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_DisplaySyncCond = PTHREAD_COND_INITIALIZER;
static unsigned int s_nDisplaySyncTicks = 0;
static CVDisplayLinkRef s_DisplayLink = NULL;
static CGDirectDisplayID s_DisplayLinkDisplay = kCGNullDirectDisplay;
static int s_nDisplayLinkSDLDisplay = -1;

static CVReturn DisplaySyncCallback( CVDisplayLinkRef displayLink, const CVTimeStamp *pNow, const CVTimeStamp *pOutputTime, CVOptionFlags flagsIn, CVOptionFlags *pFlagsOut, void *pContext )
{
	pthread_mutex_lock( &s_DisplaySyncMutex );
	s_nDisplaySyncTicks++;
	pthread_cond_broadcast( &s_DisplaySyncCond );
	pthread_mutex_unlock( &s_DisplaySyncMutex );
	return kCVReturnSuccess;
}

static CGDirectDisplayID DisplaySync_FindCGDisplay( int nSDLDisplay )
{
	SDL_Rect rect;
	if ( SDL_GetDisplayBounds( nSDLDisplay, &rect ) == 0 )
	{
		CGDirectDisplayID displays[32];
		uint32_t nDisplays = 0;
		if ( CGGetActiveDisplayList( 32, displays, &nDisplays ) == kCGErrorSuccess )
		{
			for ( uint32_t i = 0; i < nDisplays; i++ )
			{
				CGRect bounds = CGDisplayBounds( displays[i] );
				if ( (int)bounds.origin.x == rect.x && (int)bounds.origin.y == rect.y )
					return displays[i];
			}
		}
	}
	return CGMainDisplayID();
}

// Blocks until the display the window is on has refreshed nInterval more times.
void DisplaySync_WaitForVBlank( SDL_Window *pWindow, int nInterval )
{
	int nSDLDisplay = SDL_GetWindowDisplayIndex( pWindow );
	if ( !s_DisplayLink || nSDLDisplay != s_nDisplayLinkSDLDisplay )
	{
		s_nDisplayLinkSDLDisplay = nSDLDisplay;
		CGDirectDisplayID display = DisplaySync_FindCGDisplay( nSDLDisplay );

		if ( !s_DisplayLink )
		{
			if ( CVDisplayLinkCreateWithCGDisplay( display, &s_DisplayLink ) != kCVReturnSuccess )
			{
				s_DisplayLink = NULL;
				return;
			}
			CVDisplayLinkSetOutputCallback( s_DisplayLink, DisplaySyncCallback, NULL );
			CVDisplayLinkStart( s_DisplayLink );
		}
		else if ( display != s_DisplayLinkDisplay )
		{
			CVDisplayLinkSetCurrentCGDisplay( s_DisplayLink, display );
		}
		s_DisplayLinkDisplay = display;
	}

	// The timeout keeps a stalled link (display asleep, window hidden) from hanging the frame.
	struct timespec deadline;
	clock_gettime( CLOCK_REALTIME, &deadline );
	deadline.tv_nsec += 100 * 1000 * 1000;
	if ( deadline.tv_nsec >= 1000 * 1000 * 1000 )
	{
		deadline.tv_sec++;
		deadline.tv_nsec -= 1000 * 1000 * 1000;
	}

	pthread_mutex_lock( &s_DisplaySyncMutex );
	unsigned int nTarget = s_nDisplaySyncTicks + ( nInterval > 0 ? nInterval : 1 );
	while ( (int)( s_nDisplaySyncTicks - nTarget ) < 0 )
	{
		if ( pthread_cond_timedwait( &s_DisplaySyncCond, &s_DisplaySyncMutex, &deadline ) != 0 )
			break;
	}
	pthread_mutex_unlock( &s_DisplaySyncMutex );
}

void DisplaySync_Shutdown()
{
	if ( s_DisplayLink )
	{
		CVDisplayLinkStop( s_DisplayLink );
		CVDisplayLinkRelease( s_DisplayLink );
		s_DisplayLink = NULL;
	}
	s_DisplayLinkDisplay = kCGNullDirectDisplay;
	s_nDisplayLinkSDLDisplay = -1;
}
