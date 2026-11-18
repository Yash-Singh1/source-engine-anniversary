//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Implements the IHTMLChromeController interface: the main thread
//			side of the CEF thread's command and response queues.
//
//=============================================================================//

#include "html_chrome.h"
#include "html/ichromehtmlwrapper.h"
#include "html/htmlprotobuf.h"
#include "tier0/vprof.h"
#include "tier1/interface.h"
#include "tier1/utlmap.h"
#include "tier0/valve_minmax_off.h"

#include "tier0/memdbgoff.h"
#include "htmlmessages.pb.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: owns the browser handles on the main thread and pumps the
//			responses from the CEF thread to them
//-----------------------------------------------------------------------------
class CHTMLController : public IHTMLChromeController
{
public:
	CHTMLController()
	{
		m_BrowserSerial = 0;
		m_nCefTargetFrameRate = 0;
		SetDefLessFunc( m_mapBrowserRequests );
		SetDefLessFunc( m_mapBrowsers );
	}
	~CHTMLController() {}

	bool Init( const char *pchHTMLCacheDir, const char *pchCookiePath );
	void Shutdown();
	bool RunFrame();

	void SetWebCookie( const char *pchHostname, const char *pchKey, const char *pchValue, const char *pchPath, RTime32 nExpires );
	void GetWebCookiesForURL( CUtlString *pstrValue, const char *pchURL, const char *pchName );
	void SetClientBuildID( uint64 ulBuildID );

	bool BHasPendingMessages();

	void CreateBrowser( IHTMLResponses *pBrowser, bool bPopupWindow, const char *pchUserAgentIdentifier );
	void RemoveBrowser( IHTMLResponses *pBrowser );

	void WakeThread() { AccessHTMLWrapper().WakeThread(); }
	HTMLCommandBuffer_t *GetFreeCommandBuffer( EHTMLCommands eCmd, int iBrowser ) { return AccessHTMLWrapper().GetFreeCommandBuffer( eCmd, iBrowser ); }
	void PushCommand( HTMLCommandBuffer_t *pCmd ) { AccessHTMLWrapper().PushCommand( pCmd ); }

	void SetCefThreadTargetFrameRate( uint32 nFPS );

private:
	// browsers we have asked the CEF thread to create, keyed by request id
	CUtlMap<uint32, IHTMLResponses *, int> m_mapBrowserRequests;
	uint32 m_BrowserSerial;
	// live browsers, keyed by the CEF thread's browser handle
	CUtlMap<uint32, IHTMLResponses *, int> m_mapBrowsers;
	uint32 m_nCefTargetFrameRate;
};

static CHTMLController s_HTMLController;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CHTMLController, IHTMLChromeController, CHROMEHTML_CONTROLLER_INTERFACE_VERSION, s_HTMLController );


//-----------------------------------------------------------------------------
// Purpose: request a new browser; the CEF thread answers with a
//			BrowserCreateResponse carrying the browser handle
//-----------------------------------------------------------------------------
void CHTMLController::CreateBrowser( IHTMLResponses *pBrowser, bool bPopupWindow, const char *pchUserAgentIdentifier )
{
	++m_BrowserSerial;
	m_mapBrowserRequests.Insert( m_BrowserSerial, pBrowser );

	CHTMLProtoBufMsg<CMsgBrowserCreate> cmd( eHTMLCommands_BrowserCreate );
	cmd.Body().set_request_id( m_BrowserSerial );
	cmd.Body().set_popup( bPopupWindow );
	cmd.Body().set_useragent( pchUserAgentIdentifier );
	HTMLCommandBuffer_t *pBuf = AccessHTMLWrapper().GetFreeCommandBuffer( eHTMLCommands_BrowserCreate, -1 );
	cmd.SerializeCrossProc( &pBuf->m_Buffer );
	AccessHTMLWrapper().PushCommand( pBuf );
}


//-----------------------------------------------------------------------------
// Purpose: tell the CEF thread to close this browser and forget about it
//-----------------------------------------------------------------------------
void CHTMLController::RemoveBrowser( IHTMLResponses *pBrowser )
{
	FOR_EACH_MAP_FAST( m_mapBrowsers, i )
	{
		if ( m_mapBrowsers[i] == pBrowser )
		{
			CHTMLProtoBufMsg<CMsgBrowserRemove> cmd( eHTMLCommands_BrowserRemove );
			cmd.Body().set_browser_handle( pBrowser->BrowserGetIndex() );
			HTMLCommandBuffer_t *pBuf = AccessHTMLWrapper().GetFreeCommandBuffer( eHTMLCommands_BrowserRemove, pBrowser->BrowserGetIndex() );
			cmd.SerializeCrossProc( &pBuf->m_Buffer );
			AccessHTMLWrapper().PushCommand( pBuf );
			m_mapBrowsers.RemoveAt( i );
		}
	}

	FOR_EACH_MAP_FAST( m_mapBrowserRequests, i )
	{
		if ( m_mapBrowserRequests[i] == pBrowser )
		{
			m_mapBrowserRequests.RemoveAt( i );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: start the CEF thread
//-----------------------------------------------------------------------------
bool CHTMLController::Init( const char *pchHTMLCacheDir, const char *pchCookiePath )
{
	ChromeInit( pchHTMLCacheDir, pchCookiePath );
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: stop the CEF thread
//-----------------------------------------------------------------------------
void CHTMLController::Shutdown()
{
	ChromeShutdown();
}


//-----------------------------------------------------------------------------
// Purpose: union a paint's update rect into the dirty region; no update rect
//			means the whole page
//-----------------------------------------------------------------------------
static inline void AccumulateDirtyRect( const CMsgNeedsPaint &paint, int &nMinX, int &nMinY, int &nMaxX, int &nMaxY )
{
	if ( !paint.updatewide() || !paint.updatetall() )
	{
		nMinX = 0;
		nMinY = 0;
		nMaxX = INT_MAX;
		nMaxY = INT_MAX;
		return;
	}

	int x0 = paint.updatex();
	int x1 = x0 + (int)paint.updatewide();
	if ( x0 >= x1 )
		return;

	int y0 = paint.updatey();
	int y1 = y0 + (int)paint.updatetall();
	if ( y0 >= y1 )
		return;

	nMinX = MIN( nMinX, x0 );
	nMinY = MIN( nMinY, y0 );
	nMaxX = MAX( nMaxX, x1 );
	nMaxY = MAX( nMaxY, y1 );
}


#define HTML_MSG_FUNC( eHTMLCommand, bodyType, commandFunc ) \
	case eHTMLCommand: \
	{ \
		CHTMLProtoBufMsg< bodyType > cmd( pCmd->m_eCmd ); \
		if ( !cmd.BDeserializeCrossProc( &pCmd->m_Buffer ) ) \
		{ \
			bError = true; \
		} \
		else \
		{ \
			int iClient = m_mapBrowsers.Find( cmd.BodyConst().browser_handle() ); \
			if ( iClient != m_mapBrowsers.InvalidIndex() ) \
			{ \
				if ( m_mapBrowsers[iClient] ) \
					m_mapBrowsers[iClient]->commandFunc( &cmd.BodyConst() ); \
			} \
		} \
	} \
	break; \


//-----------------------------------------------------------------------------
// Purpose: dispatch the CEF thread's responses to their browsers
//-----------------------------------------------------------------------------
bool CHTMLController::RunFrame()
{
	VPROF_BUDGET( "CHTMLController::RunFrame", VPROF_BUDGETGROUP_OTHER_VGUI );

	HTMLCommandBuffer_t *pCmd;
	bool bError = false;
	bool bDidWork = false;
	CUtlVector<HTMLCommandBuffer_t *> vecNeedsPaint;
	while ( AccessHTMLWrapper().GetMainThreadCommand( &pCmd ) )
	{
		bool bRelease = true;
		switch ( pCmd->m_eCmd )
		{
			case eHTMLCommands_BrowserCreateResponse:
			{
				CHTMLProtoBufMsg< CMsgBrowserCreateResponse > cmd( pCmd->m_eCmd );
				if ( !cmd.BDeserializeCrossProc( &pCmd->m_Buffer ) )
				{
					bError = true;
				}
				else
				{
					int iRequest = m_mapBrowserRequests.Find( cmd.BodyConst().request_id() );
					if ( iRequest != m_mapBrowserRequests.InvalidIndex() )
					{
						m_mapBrowsers.Insert( cmd.BodyConst().browser_handle(), m_mapBrowserRequests[iRequest] );
						m_mapBrowserRequests[iRequest]->BrowserSetIndex( cmd.BodyConst().browser_handle() );
						m_mapBrowserRequests.RemoveAt( iRequest );
					}
				}
			}
			break;

			HTML_MSG_FUNC( eHTMLCommands_BrowserReady, CMsgBrowserReady, BrowserReady );
			case eHTMLCommands_NeedsPaint:
				// paints are coalesced below, once the queue is drained
				vecNeedsPaint.AddToTail( pCmd );
				bRelease = false;
				break;

			HTML_MSG_FUNC( eHTMLCommands_StartRequest, CMsgStartRequest, BrowserStartRequest );
			HTML_MSG_FUNC( eHTMLCommands_URLChanged, CMsgURLChanged, BrowserURLChanged );
			HTML_MSG_FUNC( eHTMLCommands_FinishedRequest, CMsgFinishedRequest, BrowserFinishedRequest );
			HTML_MSG_FUNC( eHTMLCommands_ShowPopup, CMsgShowPopup, BrowserShowPopup );
			HTML_MSG_FUNC( eHTMLCommands_HidePopup, CMsgHidePopup, BrowserHidePopup );
			HTML_MSG_FUNC( eHTMLCommands_OpenNewTab, CMsgOpenNewTab, BrowserOpenNewTab );
			HTML_MSG_FUNC( eHTMLCommands_PopupHTMLWindow, CMsgPopupHTMLWindow, BrowserPopupHTMLWindow );
			HTML_MSG_FUNC( eHTMLCommands_SetHTMLTitle, CMsgSetHTMLTitle, BrowserSetHTMLTitle );
			HTML_MSG_FUNC( eHTMLCommands_LoadingResource, CMsgLoadingResource, BrowserLoadingResource );
			HTML_MSG_FUNC( eHTMLCommands_StatusText, CMsgStatusText, BrowserStatusText );
			HTML_MSG_FUNC( eHTMLCommands_SetCursor, CMsgSetCursor, BrowserSetCursor );
			HTML_MSG_FUNC( eHTMLCommands_FileLoadDialog, CMsgFileLoadDialog, BrowserFileLoadDialog );
			HTML_MSG_FUNC( eHTMLCommands_ShowToolTip, CMsgShowToolTip, BrowserShowToolTip );
			HTML_MSG_FUNC( eHTMLCommands_UpdateToolTip, CMsgUpdateToolTip, BrowserUpdateToolTip );
			HTML_MSG_FUNC( eHTMLCommands_HideToolTip, CMsgHideToolTip, BrowserHideToolTip );
			HTML_MSG_FUNC( eHTMLCommands_SearchResults, CMsgSearchResults, BrowserSearchResults );
			HTML_MSG_FUNC( eHTMLCommands_Close, CMsgClose, BrowserClose );
			HTML_MSG_FUNC( eHTMLCommands_GetZoomResponse, CMsgGetZoomResponse, BrowserGetZoomResponse );
			HTML_MSG_FUNC( eHTMLCommands_HorizontalScrollBarSizeResponse, CMsgHorizontalScrollBarSizeResponse, BrowserHorizontalScrollBarSizeResponse );
			HTML_MSG_FUNC( eHTMLCommands_VerticalScrollBarSizeResponse, CMsgVerticalScrollBarSizeResponse, BrowserVerticalScrollBarSizeResponse );
			HTML_MSG_FUNC( eHTMLCommands_LinkAtPositionResponse, CMsgLinkAtPositionResponse, BrowserLinkAtPositionResponse );
			HTML_MSG_FUNC( eHTMLCommands_ZoomToElementAtPositionResponse, CMsgZoomToElementAtPositionResponse, BrowserZoomToElementAtPositionResponse );
			HTML_MSG_FUNC( eHTMLCommands_JSAlert, CMsgJSAlert, BrowserJSAlert );
			HTML_MSG_FUNC( eHTMLCommands_JSConfirm, CMsgJSConfirm, BrowserJSConfirm );
			HTML_MSG_FUNC( eHTMLCommands_OpenSteamURL, CMsgOpenSteamURL, BrowserOpenSteamURL );
			HTML_MSG_FUNC( eHTMLCommands_CanGoBackandForward, CMsgCanGoBackAndForward, BrowserCanGoBackandForward );
			HTML_MSG_FUNC( eHTMLCommands_SizePopup, CMsgSizePopup, BrowserSizePopup );
			HTML_MSG_FUNC( eHTMLCommands_ScaleToValueResponse, CMsgScalePageToValueResponse, BrowserScalePageToValueResponse );
			HTML_MSG_FUNC( eHTMLCommands_RequestFullScreen, CMsgRequestFullScreen, BrowserRequestFullScreen );
			HTML_MSG_FUNC( eHTMLCommands_ExitFullScreen, CMsgExitFullScreen, BrowserExitFullScreen );
			HTML_MSG_FUNC( eHTMLCommands_GetCookiesForURLResponse, CMsgGetCookiesForURLResponse, BrowserGetCookiesForURLResponse );
			HTML_MSG_FUNC( eHTMLCommands_NodeGotFocus, CMsgNodeHasFocus, BrowserNodeGotFocus );
			HTML_MSG_FUNC( eHTMLCommands_SavePageToJPEGResponse, CMsgSavePageToJPEGResponse, BrowserSavePageToJPEGResponse );
			HTML_MSG_FUNC( eHTMLCommands_GetFocusedNodeValueResponse, CMsgFocusedNodeTextResponse, BrowserFocusedNodeValueResponse );

			default:
				break;
		}

		if ( bError )
		{
			Warning( "Failed to parse command %d", pCmd->m_eCmd );
		}
		bDidWork = true;

		if ( bRelease )
			AccessHTMLWrapper().ReleaseCommandBuffer( pCmd );
	}

	// only the newest paint for a browser gets drawn; older ones are merged into
	// its dirty region and handed straight back so their textures can be reused
	while ( vecNeedsPaint.Count() )
	{
		HTMLCommandBuffer_t *pPaintCmd = vecNeedsPaint[ vecNeedsPaint.Count() - 1 ];
		int iBrowser = pPaintCmd->m_iBrowser;
		CHTMLProtoBufMsg< CMsgNeedsPaint > cmd( eHTMLCommands_NeedsPaint );
		cmd.BDeserializeCrossProc( &pPaintCmd->m_Buffer );
		AccessHTMLWrapper().ReleaseCommandBuffer( pPaintCmd );

		int nMinX = INT_MAX, nMinY = INT_MAX, nMaxX = 0, nMaxY = 0;
		AccumulateDirtyRect( cmd.BodyConst(), nMinX, nMinY, nMaxX, nMaxY );
		vecNeedsPaint.Remove( vecNeedsPaint.Count() - 1 );

		for ( int i = vecNeedsPaint.Count() - 1; i >= 0; i-- )
		{
			if ( vecNeedsPaint[i]->m_iBrowser != iBrowser )
				continue;

			CHTMLProtoBufMsg< CMsgNeedsPaint > cmdOld( eHTMLCommands_NeedsPaint );
			cmdOld.BDeserializeCrossProc( &vecNeedsPaint[i]->m_Buffer );
			if ( cmd.BodyConst().browser_handle() != cmdOld.BodyConst().browser_handle() )
				continue;

			AccessHTMLWrapper().ReleaseCommandBuffer( vecNeedsPaint[i] );
			vecNeedsPaint.Remove( i );
			AccumulateDirtyRect( cmdOld.BodyConst(), nMinX, nMinY, nMaxX, nMaxY );

			HTMLCommandBuffer_t *pBuf = GetFreeCommandBuffer( eHTMLCommands_NeedsPaintResponse, cmdOld.BodyConst().browser_handle() );
			CHTMLProtoBufMsg< CMsgNeedsPaintResponse > cmdResponse( eHTMLCommands_NeedsPaintResponse );
			cmdResponse.Body().set_browser_handle( cmdOld.BodyConst().browser_handle() );
			cmdResponse.Body().set_textureid( cmdOld.BodyConst().textureid() );
			cmdResponse.SerializeCrossProc( &pBuf->m_Buffer );
			PushCommand( pBuf );
		}

		int iClient = m_mapBrowsers.Find( cmd.BodyConst().browser_handle() );
		if ( iClient != m_mapBrowsers.InvalidIndex() && m_mapBrowsers[iClient] )
		{
			int nWide = cmd.BodyConst().wide();
			int nTall = cmd.BodyConst().tall();
			int x0 = clamp( nMinX, 0, nWide );
			int x1 = clamp( nMaxX, 0, nWide );
			int y0 = clamp( nMinY, 0, nTall );
			int y1 = clamp( nMaxY, 0, nTall );
			if ( x1 - x0 == nWide && y1 - y0 == nTall )
			{
				cmd.Body().clear_updatex();
				cmd.Body().clear_updatey();
				cmd.Body().clear_updatewide();
				cmd.Body().clear_updatetall();
			}
			else
			{
				cmd.Body().set_updatex( clamp( nMinX, 0, nWide ) );
				cmd.Body().set_updatey( clamp( nMinY, 0, nTall ) );
				cmd.Body().set_updatewide( x1 - x0 );
				cmd.Body().set_updatetall( y1 - y0 );
			}
			m_mapBrowsers[iClient]->BrowserNeedsPaint( &cmd.BodyConst() );
		}
	}

	return bDidWork;
}


//-----------------------------------------------------------------------------
// Purpose: set a cookie in the CEF cookie store
//-----------------------------------------------------------------------------
void CHTMLController::SetWebCookie( const char *pchHostname, const char *pchKey, const char *pchValue, const char *pchPath, RTime32 nExpires )
{
	ChromeSetWebCookie( pchHostname, pchKey, pchValue, pchPath, nExpires );
}


//-----------------------------------------------------------------------------
// Purpose: the steam client build id, for the user agent
//-----------------------------------------------------------------------------
void CHTMLController::SetClientBuildID( uint64 ulBuildID )
{
	ChromeSetClientBuildID( ulBuildID );
}


//-----------------------------------------------------------------------------
// Purpose: read a cookie from the CEF cookie store
//-----------------------------------------------------------------------------
void CHTMLController::GetWebCookiesForURL( CUtlString *pstrValue, const char *pchURL, const char *pchName )
{
	ChromeGetWebCookiesForURL( pstrValue, pchURL, pchName );
}


//-----------------------------------------------------------------------------
// Purpose: are there responses waiting for RunFrame?
//-----------------------------------------------------------------------------
bool CHTMLController::BHasPendingMessages()
{
	return AccessHTMLWrapper().BHasPendingMessages();
}


//-----------------------------------------------------------------------------
// Purpose: how often the CEF thread should pump CEF
//-----------------------------------------------------------------------------
void CHTMLController::SetCefThreadTargetFrameRate( uint32 nFPS )
{
	if ( nFPS != m_nCefTargetFrameRate )
	{
		m_nCefTargetFrameRate = nFPS;
		CHTMLProtoBufMsg<CMsgSetTargetFrameRate> cmd( eHTMLCommands_SetTargetFrameRate );
		cmd.Body().set_ntargetframerate( nFPS );
		HTMLCommandBuffer_t *pBuf = AccessHTMLWrapper().GetFreeCommandBuffer( eHTMLCommands_SetTargetFrameRate, -1 );
		cmd.SerializeCrossProc( &pBuf->m_Buffer );
		AccessHTMLWrapper().PushCommand( pBuf );
	}
}
