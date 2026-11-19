//===== Copyright Valve Corporation, All rights reserved. ======//
//
// Purpose: Match events: sinks subscribe to events broadcast as KeyValues.
//
//===========================================================================//

#ifndef IMATCHEVENTS_H
#define IMATCHEVENTS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class KeyValues;

abstract_class IMatchEventsSink
{
public:
	virtual void OnEvent( KeyValues *pEvent ) {}
};

abstract_class IMatchEventsSubscription
{
public:
	virtual void Subscribe( IMatchEventsSink *pSink ) = 0;
	virtual void Unsubscribe( IMatchEventsSink *pSink ) = 0;

	virtual void BroadcastEvent( KeyValues *pEvent ) = 0;

	virtual void RegisterEventData( KeyValues *pEventData ) = 0;
	virtual KeyValues * GetEventData( char const *szEventDataKey ) = 0;
};

#endif // IMATCHEVENTS_H
