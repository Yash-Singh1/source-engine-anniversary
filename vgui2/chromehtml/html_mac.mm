//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Cocoa helpers for the CEF thread
//
//=============================================================================//

#import <Cocoa/Cocoa.h>

extern "C" void *CreateAutoReleasePool()
{
	return [[NSAutoreleasePool alloc] init];
}

extern "C" void ReleaseAutoReleasePool( void *pool )
{
	[(NSAutoreleasePool *)pool release];
}
