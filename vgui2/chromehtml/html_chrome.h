//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The CEF thread that owns the browsers, and the command queues
//			between it and the main thread.
//
//=============================================================================//

#ifndef HTML_CHROME_H
#define HTML_CHROME_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/threadtools.h"
#include "tier0/tslist.h"
#include "tier1/utlstring.h"
#include "tier1/utlvector.h"
#include "tier1/utllinkedlist.h"
#include "tier1/utlmap.h"
#include "html/ihtmlchrome.h"
#include "html/htmlprotobuf.h"

class CClientHandler;

bool ChromeInit( const char *pchHTMLCacheDir, const char *pchCookiePath );
void ChromeShutdown();
bool ChromeSetWebCookie( const char *pchHostname, const char *pchName, const char *pchValue, const char *pchPath, RTime32 nExpires );
void ChromeSetClientBuildID( uint64 ulBuildID );
bool ChromeGetWebCookiesForURL( CUtlString *pstrValue, const char *pchURL, const char *pchName );

//-----------------------------------------------------------------------------
// Purpose: the thread CEF runs on; commands go in on one queue and
//			responses come back to the main thread on another
//-----------------------------------------------------------------------------
class CCEFThread : public CThread
{
public:
	CCEFThread();
	~CCEFThread();

	void SetCEFPaths( const char *pchHTMLCacheDir, const char *pchCookiePath );
	void TriggerShutdown();
	void WakeThread();
	virtual int Run();

	bool BHasPendingMessages() { return m_tslResponseBuffers.Count() > 0; }

	HTMLCommandBuffer_t *GetFreeCommandBuffer( EHTMLCommands eCmd, int iBrowser );
	void ReleaseCommandBuffer( HTMLCommandBuffer_t *pBuf );
	void PushCommand( HTMLCommandBuffer_t * );
	void PushResponse( HTMLCommandBuffer_t * );
	bool GetMainThreadCommand( HTMLCommandBuffer_t ** );
	HTMLCommandBuffer_t *BWaitForCommand( EHTMLCommands eCmd, int iBrowser );
	HTMLCommandBuffer_t *BWaitForResponse( EHTMLCommands eCmd, int iBrowser );

	struct SizeChange_t
	{
		int iBrowser;
		int nWide;
		int nTall;
		float flZoom;
	};

private:
	bool m_bExit;
	bool m_bSawUserInputThisFrame;
	CThreadEvent m_WakeEvent;
	CUtlString m_sHTMLCacheDir;
	CUtlString m_sCookiePath;
	bool m_bFullScreenFlashVisible;
	CThreadEvent m_eventDidExit;
	CUtlLinkedList<CClientHandler *> m_listClientHandlers;
	CTSQueue<HTMLCommandBuffer_t *> m_tslCommandBuffers;
	CUtlVector<HTMLCommandBuffer_t *> m_vecQueueCommands;
	CTSQueue<HTMLCommandBuffer_t *> m_tslResponseBuffers;
	CUtlVector<HTMLCommandBuffer_t *> m_vecQueueResponses;
	CTSList<HTMLCommandBuffer_t *> m_tslUnsedBuffers;
	CThreadEvent m_evWaitingForCommand;
	CThreadEvent m_evWaitingForResponse;
	int m_nTargetFrameRate;
	uint16 m_nBrowserSerial;
	uint16 m_nCEFBrowserSerial;
	CUtlMap<int, SizeChange_t, int> m_mapSizeChangesPending;
};

CCEFThread &AccessHTMLWrapper();

#endif // HTML_CHROME_H
