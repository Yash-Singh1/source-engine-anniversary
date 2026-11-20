//===== Copyright Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#include "mm_events.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CMatchEventsSubscription::CMatchEventsSubscription() :
	m_bBroadcasting( false ),
	m_bAllowNestedBroadcasts( false )
{
}

CMatchEventsSubscription::~CMatchEventsSubscription()
{
}

static CMatchEventsSubscription g_MatchEventsSubscription;
CMatchEventsSubscription *g_pMatchEventsSubscription = &g_MatchEventsSubscription;

void CMatchEventsSubscription::Subscribe( IMatchEventsSink *pSink )
{
	if ( !pSink )
		return;

	int idx = m_arrSinks.Find( pSink );
	if ( m_arrSinks.IsValidIndex( idx ) )
	{
		Assert( m_arrRefCount.IsValidIndex( idx ) );
		Assert( m_arrRefCount[ idx ] > 0 );

		++ m_arrRefCount[ idx ];
	}
	else
	{
		m_arrSinks.AddToTail( pSink );
		m_arrRefCount.AddToTail( 1 );

		Assert( m_arrSinks.Count() == m_arrRefCount.Count() );
	}
}

void CMatchEventsSubscription::Unsubscribe( IMatchEventsSink *pSink )
{
	if ( !pSink )
		return;

	int idx = m_arrSinks.Find( pSink );
	Assert( m_arrSinks.IsValidIndex( idx ) );
	if ( !m_arrSinks.IsValidIndex( idx ) )
		return;

	Assert( m_arrRefCount.IsValidIndex( idx ) );
	Assert( m_arrRefCount[ idx ] > 0 );

	-- m_arrRefCount[ idx ];

	if ( m_arrRefCount[ idx ] <= 0 )
	{
		m_arrSinks.Remove( idx );
		m_arrRefCount.Remove( idx );

		Assert( m_arrSinks.Count() == m_arrRefCount.Count() );

		// Update outstanding iterators that are beyond removal point
		for ( int k = 0; k < m_arrIteratorsOutstanding.Count(); ++ k )
		{
			if ( m_arrIteratorsOutstanding[k] >= idx )
				-- m_arrIteratorsOutstanding[k];
		}
	}
}

#ifdef BYTEMATCH_RETAIL
void _bm_dead_mm_events_0() {}
#endif
#line 70

void CMatchEventsSubscription::BroadcastEvent( KeyValues *pEvent )
{
	// Events broadcast while sinks are being notified are queued
	if ( m_bBroadcasting && !m_bAllowNestedBroadcasts )
	{
		m_arrQueuedEvents.AddToTail( pEvent );
		return;
	}

	m_bBroadcasting = true;

	KeyValues::AutoDelete autoDeleteEvent( pEvent );
	m_arrSentEvents.AddToHead( pEvent );

	// Iterate through sinks; Unsubscribe adjusts the outstanding iterators
	m_arrIteratorsOutstanding.AddToTail( 0 );
	for ( ; m_arrIteratorsOutstanding.Tail() < m_arrSinks.Count(); ++ m_arrIteratorsOutstanding.Tail() )
	{
		IMatchEventsSink *pSink = m_arrSinks[ m_arrIteratorsOutstanding.Tail() ];
		pSink->OnEvent( pEvent );
	}
	m_arrIteratorsOutstanding.RemoveMultipleFromTail( 1 );

	m_bBroadcasting = false;

	// Broadcast the next queued event
	if ( m_arrQueuedEvents.Count() > 0 )
	{
		KeyValues *pQueued = m_arrQueuedEvents.Head();
		m_arrQueuedEvents.RemoveMultipleFromHead( 1 );
		BroadcastEvent( pQueued );
		return;
	}

	// No more queued events, clean up registered event data
	for ( int k = 0; k < m_arrEventData.Count(); ++ k )
	{
		KeyValues *pRegisteredData = m_arrEventData[k];
		pRegisteredData->deleteThis();
	}
	m_arrEventData.Purge();

	// Sent events delete themselves once broadcast
	m_arrSentEvents.Purge();
}

void CMatchEventsSubscription::RegisterEventData( KeyValues *pEventData )
{
	Assert( pEventData );
	if ( !pEventData )
		return;

	char const *szName = pEventData->GetName();
	Assert( szName && *szName );
	if ( !szName || !*szName )
		return;

	for ( int k = 0; k < m_arrEventData.Count(); ++ k )
	{
		KeyValues *&kv = m_arrEventData[k];
		if ( !Q_stricmp( szName, kv->GetName() ) )
		{
			// Override the existing event data
			kv->deleteThis();
			kv = pEventData;
			return;
		}
	}

	m_arrEventData.AddToTail( pEventData );
}

KeyValues * CMatchEventsSubscription::GetEventData( char const *szEventDataKey )
{
	for ( int k = 0; k < m_arrEventData.Count(); ++ k )
	{
		KeyValues *kv = m_arrEventData[k];
		if ( !Q_stricmp( szEventDataKey, kv->GetName() ) )
			return kv;
	}

	for ( int k = 0; k < m_arrSentEvents.Count(); ++ k )
	{
		KeyValues *kv = m_arrSentEvents[k];
		if ( !Q_stricmp( szEventDataKey, kv->GetName() ) )
			return kv;
	}

	return NULL;
}
