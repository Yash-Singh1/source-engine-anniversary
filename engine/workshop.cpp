//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Steam Workshop engine subsystem (CWorkshop, CWorkshopImage)
//          Decompiled from retail HL2 20th Anniversary engine.so.
//
//=============================================================================//

#include "cdll_int.h"
#include "tier1/convar.h"
#if !defined( OSX ) || defined( PLATFORM_64BITS )
#include "iworkshop.h"
#endif

#if defined( OSX ) && defined( PLATFORM_64BITS )
#include <algorithm>
#include "tier1/KeyValues.h"
#include "tier1/utlmap.h"
#include "tier1/utlbuffer.h"
#include "filesystem.h"
#include "matchmaking/imatchevents.h"
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "vgui_controls/Controls.h"
#include "bitmap/bitmap.h"
#include "imageutils.h"
#include "steam/steam_api.h"
#include "steam/isteamhttp.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern IVEngineClient *engineClient;

#if defined( OSX ) && defined( PLATFORM_64BITS )

extern IFileSystem *g_pFileSystem;
extern void Host_MapListFlush( void );

//-----------------------------------------------------------------------------
// ConVars (retail defaults & flags)
//-----------------------------------------------------------------------------
ConVar workshop_addon_enable_by_default( "workshop_addon_enable_by_default", "1", 0 );
ConVar workshop_prioritise_campaign_mount( "workshop_prioritise_campaign_mount", "0", FCVAR_ARCHIVE,
	"Whether or not the campaign should be prioritized over existing content overrides that you have mounted." );

//-----------------------------------------------------------------------------
// Tag tables (retail CWorkshop::GetTagList categories 0..4)
//-----------------------------------------------------------------------------
static const WorkshopTag_t s_TypeTags[] =
{
	{ "campaigns", 1ULL << 0 },
	{ "assets",    1ULL << 1 },
	{ NULL, 0 }
};

static const WorkshopTag_t s_AssetTypeTags[] =
{
	{ "maps",      1ULL << 0 },
	{ "materials", 1ULL << 1 },
	{ "models",    1ULL << 2 },
	{ "particles", 1ULL << 3 },
	{ "ui",        1ULL << 4 },
	{ "sounds",    1ULL << 5 },
	{ NULL, 0 }
};

static const WorkshopTag_t s_WeaponTags[] =
{
	{ "crowbar",         1ULL << 0 },
	{ "pistol",          1ULL << 1 },
	{ "smg1",            1ULL << 2 },
	{ "ar2",             1ULL << 3 },
	{ "357",             1ULL << 4 },
	{ "alyxgun",         1ULL << 5 },
	{ "annabelle",       1ULL << 6 },
	{ "bugbait",         1ULL << 7 },
	{ "citizenpackage",  1ULL << 8 },
	{ "citizensuitcase", 1ULL << 9 },
	{ "crossbow",        1ULL << 10 },
	{ "frag",            1ULL << 11 },
	{ "physcannon",      1ULL << 12 },
	{ "rpg",             1ULL << 13 },
	{ "shotgun",         1ULL << 14 },
	{ "stunstick",       1ULL << 15 },
	{ NULL, 0 }
};

static const WorkshopTag_t s_NPCTags[] =
{
	{ "alyx",            1ULL << 0 },
	{ "gman",            1ULL << 1 },
	{ "barney",          1ULL << 2 },
	{ "kleiner",         1ULL << 3 },
	{ "dog",             1ULL << 4 },
	{ "citizen",         1ULL << 5 },
	{ "combine_soldier", 1ULL << 6 },
	{ "combine_elite",   1ULL << 7 },
	{ "metrocop",        1ULL << 8 },
	{ "vortigaunt",      1ULL << 9 },
	{ "antlion",         1ULL << 10 },
	{ "antlion_guard",   1ULL << 11 },
	{ "antlion_worker",  1ULL << 12 },
	{ "antlion_grub",    1ULL << 13 },
	{ "headcrab",        1ULL << 14 },
	{ "lamarr",          1ULL << 15 },
	{ "zombie",          1ULL << 16 },
	{ "fast_zombie",     1ULL << 17 },
	{ "fast_headcrab",   1ULL << 18 },
	{ "poison_zombie",   1ULL << 19 },
	{ "poison_headcrab", 1ULL << 20 },
	{ "advisor",         1ULL << 21 },
	{ "barnacle",        1ULL << 22 },
	{ "hunter",          1ULL << 23 },
	{ "ichthyosaur",     1ULL << 24 },
	{ "leech",           1ULL << 25 },
	{ "mossman",         1ULL << 26 },
	{ "stalker",         1ULL << 27 },
	{ "turrets",         1ULL << 28 },
	{ "grigori",         1ULL << 29 },
	{ "strider",         1ULL << 30 },
	{ "scanner",         1ULL << 31 },
	{ "zombine",         1ULL << 32 },
	{ "rollermine",      1ULL << 33 },
	{ "odessa",          1ULL << 34 },
	{ "pigeon",          1ULL << 35 },
	{ "crow",            1ULL << 36 },
	{ "gunship",         1ULL << 37 },
	{ "dropship",        1ULL << 38 },
	{ "eli",             1ULL << 39 },
	{ "helicopter",      1ULL << 40 },
	{ "magnusson",       1ULL << 41 },
	{ "manhack",         1ULL << 42 },
	{ "breen",           1ULL << 43 },
	{ NULL, 0 }
};

static const WorkshopTag_t s_VehicleTags[] =
{
	{ "jeep",    1ULL << 0 },
	{ "jalopy",  1ULL << 1 },
	{ "airboat", 1ULL << 2 },
	{ "crane",   1ULL << 3 },
	{ "pod",     1ULL << 4 },
	{ "apc",     1ULL << 5 },
	{ NULL, 0 }
};

//-----------------------------------------------------------------------------
// CWorkshopImage: async HTTP preview image implementing IWorkshopImage/IImage
//-----------------------------------------------------------------------------
class CWorkshopImage : public IWorkshopImage
{
public:
	explicit CWorkshopImage( const char *pszURL )
		: m_strURL( pszURL )
		, m_hHTTPRequest( INVALID_HTTPREQUEST_HANDLE )
		, m_bLoadStarted( false )
		, m_Color( 255, 255, 255, 255 )
		, m_iTextureID( -1 )
		, m_nX( 0 )
		, m_nY( 0 )
		, m_nContentWide( 0 )
		, m_nContentTall( 0 )
		, m_nWide( 0 )
		, m_nTall( 0 )
	{
	}

	virtual ~CWorkshopImage()
	{
		m_CallResultHTTPRequest.Cancel();
		if ( m_hHTTPRequest != INVALID_HTTPREQUEST_HANDLE && SteamHTTP() )
		{
			SteamHTTP()->ReleaseHTTPRequest( m_hHTTPRequest );
			m_hHTTPRequest = INVALID_HTTPREQUEST_HANDLE;
		}
		if ( m_iTextureID >= 0 && g_pVGuiSurface )
		{
			g_pVGuiSurface->DestroyTextureID( m_iTextureID );
			m_iTextureID = -1;
		}
	}

	virtual void Paint() OVERRIDE
	{
		if ( m_iTextureID == -1 || !g_pVGuiSurface )
			return;

		int w, h;
		GetSize( w, h );

		g_pVGuiSurface->DrawSetTexture( m_iTextureID );
		g_pVGuiSurface->DrawSetColor( m_Color );
		g_pVGuiSurface->DrawTexturedRect( m_nX, m_nY, m_nX + w, m_nY + h );
	}

	virtual void SetPos( int x, int y ) OVERRIDE
	{
		m_nX = x;
		m_nY = y;
	}

	virtual void GetContentSize( int &wide, int &tall ) OVERRIDE
	{
		wide = m_nWide;
		tall = m_nTall;
	}

	virtual void GetSize( int &wide, int &tall ) OVERRIDE
	{
		wide = m_nWide;
		tall = m_nTall;
		if ( wide == 0 )
			wide = m_nContentWide;
		if ( tall == 0 )
			tall = m_nContentTall;
	}

	virtual void SetSize( int wide, int tall ) OVERRIDE
	{
		m_nWide = wide;
		m_nTall = tall;
	}

	virtual void SetColor( Color col ) OVERRIDE
	{
		m_Color = col;
	}

	virtual bool Evict() OVERRIDE { return false; }
	virtual int GetNumFrames() OVERRIDE { return 0; }
	virtual void SetFrame( int nFrame ) OVERRIDE {}
	virtual vgui::HTexture GetID() OVERRIDE { return m_iTextureID; }
	virtual void SetRotation( int iRotation ) OVERRIDE {}

	virtual void LoadImage( int nPriority = 0 ) OVERRIDE
	{
		if ( m_bLoadStarted || m_iTextureID >= 0 )
			return;

		ISteamHTTP *pHTTP = SteamHTTP();
		if ( !pHTTP || m_strURL.IsEmpty() )
			return;

		m_CallResultHTTPRequest.Cancel();
		if ( m_hHTTPRequest != INVALID_HTTPREQUEST_HANDLE )
		{
			pHTTP->ReleaseHTTPRequest( m_hHTTPRequest );
			m_hHTTPRequest = INVALID_HTTPREQUEST_HANDLE;
		}

		m_hHTTPRequest = pHTTP->CreateHTTPRequest( k_EHTTPMethodGET, m_strURL.Get() );
		if ( m_hHTTPRequest == INVALID_HTTPREQUEST_HANDLE )
			return;

		SteamAPICall_t hCall = k_uAPICallInvalid;
		if ( !pHTTP->SendHTTPRequest( m_hHTTPRequest, &hCall ) )
		{
			m_CallResultHTTPRequest.Cancel();
			pHTTP->ReleaseHTTPRequest( m_hHTTPRequest );
			m_hHTTPRequest = INVALID_HTTPREQUEST_HANDLE;
			return;
		}

		m_CallResultHTTPRequest.Set( hCall, this, &CWorkshopImage::OnHTTPRequestCompleted );
		m_bLoadStarted = true;
	}

private:
	void OnHTTPRequestCompleted( HTTPRequestCompleted_t *pResult, bool bIOFailure )
	{
		if ( bIOFailure || !pResult || !pResult->m_bRequestSuccessful )
		{
			ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshopImage] HTTP request failed...\n" );
			return;
		}

		LoadTextureFromHTTPResponse();
	}

	bool LoadTextureFromHTTPResponse()
	{
		ISteamHTTP *pHTTP = SteamHTTP();
		if ( !pHTTP || m_hHTTPRequest == INVALID_HTTPREQUEST_HANDLE || !g_pVGuiSurface || !g_pMatSystemSurface )
			return false;

		uint32 nBodySize = 0;
		if ( !pHTTP->GetHTTPResponseBodySize( m_hHTTPRequest, &nBodySize ) || nBodySize <= 31 )
			return false;

		CUtlBuffer buf;
		buf.EnsureCapacity( nBodySize );
		buf.SeekPut( CUtlBuffer::SEEK_HEAD, nBodySize );
		if ( !pHTTP->GetHTTPResponseBodyData( m_hHTTPRequest, (uint8 *)buf.Base(), nBodySize ) )
			return false;

		if ( buf.TellMaxPut() <= 31 )
		{
			Warning( "[CWorkshopImage] Unknown image file type... URL=%s.\n", m_strURL.Get() );
			return false;
		}

		const uint8 *pBytes = (const uint8 *)buf.Base();

		// PNG magic: 89 50 4E 47 0D 0A 1A 0A
		static const uint8 s_PNGMagic[8] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
		if ( memcmp( pBytes, s_PNGMagic, 8 ) == 0 )
		{
			Bitmap_t bitmap;
			if ( ImgUtl_LoadPNGBitmapFromBuffer( buf, bitmap ) != CE_SUCCESS )
			{
				Warning( "[CWorkshopImage] Corrupt PNG file, URL=%s.\n", m_strURL.Get() );
				return false;
			}

			if ( m_iTextureID >= 0 )
			{
				g_pVGuiSurface->DestroyTextureID( m_iTextureID );
				m_iTextureID = -1;
			}

			m_iTextureID = g_pVGuiSurface->CreateNewTextureID( true );
			m_nContentWide = bitmap.Width();
			m_nContentTall = bitmap.Height();
			g_pMatSystemSurface->DrawSetTextureRGBAEx2(
				m_iTextureID,
				bitmap.GetBits(),
				bitmap.Width(),
				bitmap.Height(),
				IMAGE_FORMAT_RGBA8888,
				true,
				false );
			return true;
		}

		// JPEG magic: FF D8
		if ( pBytes[0] == 0xFF && pBytes[1] == 0xD8 )
		{
			CUtlBuffer rgbaBuf;
			int nWidth = 0, nHeight = 0;
			if ( ImgUtl_ReadJPEGAsRGBAFromBuffer( buf, rgbaBuf, nWidth, nHeight ) != CE_SUCCESS
				|| nWidth == 0 || nHeight == 0 )
			{
				Warning( "[CWorkshopImage] Corrupt JPG file, URL=%s.\n", m_strURL.Get() );
				return false;
			}

			if ( m_iTextureID >= 0 )
			{
				g_pVGuiSurface->DestroyTextureID( m_iTextureID );
				m_iTextureID = -1;
			}

			m_iTextureID = g_pVGuiSurface->CreateNewTextureID( true );
			m_nContentWide = nWidth;
			m_nContentTall = nHeight;
			g_pMatSystemSurface->DrawSetTextureRGBAEx2(
				m_iTextureID,
				(const unsigned char *)rgbaBuf.Base(),
				nWidth,
				nHeight,
				IMAGE_FORMAT_RGBA8888,
				true,
				false );
			return true;
		}

		Warning( "[CWorkshopImage] Unknown image file type... URL=%s.\n", m_strURL.Get() );
		return false;
	}

	CUtlString m_strURL;
	HTTPRequestHandle m_hHTTPRequest;
	bool m_bLoadStarted;
	CCallResult<CWorkshopImage, HTTPRequestCompleted_t> m_CallResultHTTPRequest;
	Color m_Color;
	int m_iTextureID;
	int m_nX;
	int m_nY;
	int m_nContentWide;
	int m_nContentTall;
	int m_nWide;
	int m_nTall;
};

//-----------------------------------------------------------------------------
// CWorkshop: Retail VWorkshop001 AppSystem implementation
//-----------------------------------------------------------------------------
static bool IsValidMapNameChar( char c )
{
	if ( (unsigned char)( c - '0' ) <= 9 )
		return true;
	if ( (unsigned char)( c - 'A' ) <= 25 )
		return true;
	return ( (unsigned char)( c - 'a' ) <= 25 ) || c == '_';
}

static bool LessPublishedFileId( const PublishedFileId_t &lhs, const PublishedFileId_t &rhs )
{
	return lhs < rhs;
}

class CWorkshop : public CBaseAppSystem<IWorkshop>, public IMatchEventsSink
{
public:
	CWorkshop()
		: m_bInitialized( false )
		, m_bPaused( true )
		, m_bLaunchingWorkshopCampaign( false )
		, m_ulCurrentWorkshopCampaign( 0 )
		, m_CallbackSubscriptionsChanged( this, &CWorkshop::OnUserSubscribedItemsListChanged )
		, m_CallbackItemDownloaded( this, &CWorkshop::OnDownloadItemResult )
		, m_mapEnabled( LessPublishedFileId )
		, m_bNeedsRemount( false )
		, m_mapItems( LessPublishedFileId )
		, m_mapTempItems( LessPublishedFileId )
	{
	}

	~CWorkshop()
	{
		m_CallResultUGCQuery.Cancel();
		FOR_EACH_MAP_FAST( m_mapTempItems, i )
		{
			delete m_mapTempItems[i];
		}
		m_mapTempItems.Purge();
		FOR_EACH_MAP_FAST( m_mapItems, i )
		{
			delete m_mapItems[i];
		}
		m_mapItems.Purge();
	}

	//-------------------------------------------------------------------------
	// IAppSystem
	//-------------------------------------------------------------------------
	virtual bool Connect( CreateInterfaceFn factory ) OVERRIDE
	{
		return factory( VWORKSHOP_INTERFACE_VERSION, NULL ) != NULL;
	}

	virtual void Disconnect() OVERRIDE
	{
	}

	virtual void *QueryInterface( const char *pInterfaceName ) OVERRIDE
	{
		if ( !V_stricmp( pInterfaceName, VWORKSHOP_INTERFACE_VERSION ) )
			return static_cast<IWorkshop *>( this );
		return NULL;
	}

	virtual InitReturnVal_t Init() OVERRIDE
	{
		ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] Init\n" );
		LoadConfig();
		return INIT_OK;
	}

	virtual void Shutdown() OVERRIDE
	{
		if ( engineClient && engineClient->GetEventsSubscription() )
		{
			engineClient->GetEventsSubscription()->Unsubscribe( static_cast<IMatchEventsSink *>( this ) );
		}
	}

	//-------------------------------------------------------------------------
	// IWorkshop
	//-------------------------------------------------------------------------
	virtual void Frame() OVERRIDE
	{
		if ( !m_bInitialized )
		{
			if ( engineClient && engineClient->GetEventsSubscription() )
			{
				engineClient->GetEventsSubscription()->Subscribe( static_cast<IMatchEventsSink *>( this ) );
				m_bInitialized = true;
			}
			RefreshSubscribedItems();
		}

		if ( engineClient )
		{
			bool bPaused = engineClient->IsPaused();
			if ( bPaused != m_bPaused )
			{
				if ( bPaused )
					StopPlaytimeTracking();
				else
					UpdatePlaytimeTracking();
				m_bPaused = bPaused;
			}
		}
	}

	virtual bool GetSubscribedItemsInOrder( CUtlVector<PublishedFileId_t> &items ) OVERRIDE
	{
		items.RemoveAll();
		int nCount = m_vecAddonOrder.Count();
		if ( nCount != 0 )
		{
			items.SetCount( nCount );
			for ( int i = 0; i < nCount; ++i )
			{
				items[i] = m_vecAddonOrder[i];
			}
		}
		return true;
	}

	virtual bool GetSubscribedItemsSortedByTime( CUtlVector<PublishedFileId_t> &items ) OVERRIDE
	{
		GetRawSubscribedItems( items );

		std::stable_sort( items.Base(), items.Base() + items.Count(),
			[this]( PublishedFileId_t a, PublishedFileId_t b ) -> bool
			{
				WorkshopItem_t *pA = GetItem( a );
				if ( !pA )
					return false;
				WorkshopItem_t *pB = GetItem( b );
				if ( !pB )
					return false;
				return pA->m_Details.m_rtimeAddedToUserList < pB->m_Details.m_rtimeAddedToUserList;
			} );

		return true;
	}

	virtual bool RefreshSubscribedItems() OVERRIDE
	{
		ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] Querying for subscribed files...\n" );
		if ( !SteamUGC() )
			return false;

		CUtlVector<PublishedFileId_t> vecSubscribed;
		GetRawSubscribedItems( vecSubscribed );

		ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] Subscribed item count: %d\n", vecSubscribed.Count() );
		if ( vecSubscribed.Count() == 0 )
		{
			// Handle transition to 0 subscribed items cleanly
			if ( m_mapItems.Count() > 0 )
			{
				FOR_EACH_MAP_FAST( m_mapItems, i )
				{
					delete m_mapItems[i];
				}
				m_mapItems.Purge();
				m_mapEnabled.Purge();
				m_vecAddonOrder.RemoveAll();
				m_bNeedsRemount = true;
				if ( engineClient && engineClient->GetEventsSubscription() )
				{
					KeyValues *pEvent = new KeyValues( "Workshop_SubscriptionsUpdated" );
					engineClient->GetEventsSubscription()->BroadcastEvent( pEvent );
				}
				SaveConfig();
			}
			return true;
		}

		UGCQueryHandle_t hQuery = SteamUGC()->CreateQueryUGCDetailsRequest( vecSubscribed.Base(), vecSubscribed.Count() );
		if ( hQuery == k_UGCQueryHandleInvalid )
		{
			ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] CreateQueryUGCDetailsRequest failed.\n" );
			return false;
		}

		SteamUGC()->SetReturnLongDescription( hQuery, true );
		SteamUGC()->SetReturnKeyValueTags( hQuery, true );

		SteamAPICall_t hCall = SteamUGC()->SendQueryUGCRequest( hQuery );
		if ( hCall == k_uAPICallInvalid )
		{
			ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] SendQueryUGCRequest failed.\n" );
			SteamUGC()->ReleaseQueryUGCRequest( hQuery );
			return false;
		}

		m_CallResultUGCQuery.Set( hCall, this, &CWorkshop::OnSteamUGCQueryCompleted );
		return true;
	}

	virtual WorkshopItem_t *GetItem( PublishedFileId_t nItemID ) OVERRIDE
	{
		int idx = m_mapItems.Find( nItemID );
		if ( idx == m_mapItems.InvalidIndex() )
			return NULL;
		return m_mapItems[idx];
	}

	virtual const char *GetItemInstallFolder( PublishedFileId_t nItemID ) OVERRIDE
	{
		WorkshopItem_t *pItem = GetItem( nItemID );
		if ( !pItem )
			return NULL;
		return GetItemInstallFolderInternal( pItem );
	}

	virtual bool RemountAddons( bool bForce ) OVERRIDE
	{
		if ( bForce )
			m_bNeedsRemount = true;

		CheckForNewlyInstalledItems();

		bool bDidRemount = m_bNeedsRemount;
		if ( !bDidRemount )
			return false;

		m_bNeedsRemount = false;

		if ( !g_pFileSystem )
			return false;

		g_pFileSystem->PushNoFinishAllOnSearchPaths();
		UnmountAll();

		if ( !workshop_prioritise_campaign_mount.GetBool() )
		{
			if ( m_ulCurrentWorkshopCampaign != 0 )
			{
				WorkshopItem_t *pCampaign = GetItem( m_ulCurrentWorkshopCampaign );
				if ( pCampaign && pCampaign->IsValidCampaign() )
					MountItem( pCampaign );
			}
		}

		for ( int i = m_vecAddonOrder.Count() - 1; i >= 0; --i )
		{
			PublishedFileId_t id = m_vecAddonOrder[i];
			if ( id == 0 )
				continue;

			WorkshopItem_t *pItem = GetItem( id );
			if ( !pItem || !pItem->IsValidAsset() )
				continue;

			if ( IsItemEnabled( id ) )
				MountItem( pItem );
		}

		if ( workshop_prioritise_campaign_mount.GetBool() )
		{
			if ( m_ulCurrentWorkshopCampaign != 0 )
			{
				WorkshopItem_t *pCampaign = GetItem( m_ulCurrentWorkshopCampaign );
				if ( pCampaign && pCampaign->IsValidCampaign() )
					MountItem( pCampaign );
			}
		}

		UpdatePlaytimeTracking();
		ReloadLocalizationAndMapList();
		g_pFileSystem->PopNoFinishAllOnSearchPaths();

		return bDidRemount;
	}

	virtual void UnmountAll() OVERRIDE
	{
		if ( g_pFileSystem )
		{
			g_pFileSystem->RemoveSearchPaths( "WORKSHOP_ASSET" );
			g_pFileSystem->RemoveSearchPaths( "WORKSHOP_ASSET_TRACKING" );
		}
		m_vecMountedAddons.RemoveAll();
		StopPlaytimeTracking();
	}

	virtual void ReloadLocalizationAndMapList() OVERRIDE
	{
		if ( g_pFileSystem && g_pVGuiLocalize )
		{
			FileFindHandle_t hFind;
			const char *pszFile = g_pFileSystem->FindFirstEx( "resource/*_english.txt", "WORKSHOP_ASSET_TRACKING", &hFind );
			while ( pszFile )
			{
				size_t len = strlen( pszFile );
				if ( len > 12 )
				{
					char szLocFile[4096];
					V_sprintf_safe( szLocFile, "Resource/%.*s_%%language%%.txt", (int)( len - 12 ), pszFile );
					ConColorMsg( 1, Color( 0, 255, 255, 255 ), "[CWorkshop] Added localization file: %s\n", szLocFile );
					g_pVGuiLocalize->AddFile( szLocFile, "GAME", true );
				}
				pszFile = g_pFileSystem->FindNext( hFind );
			}
			g_pFileSystem->FindClose( hFind );
		}

		Host_MapListFlush();
	}

	virtual AppId_t GetAppID() OVERRIDE
	{
		if ( SteamUtils() )
			return SteamUtils()->GetAppID();
		return 220;
	}

	virtual void SetItemEnabled( PublishedFileId_t nItemID, bool bEnabled ) OVERRIDE
	{
		WorkshopItem_t *pItem = GetItem( nItemID );
		if ( pItem && !pItem->IsValidAsset() )
		{
			m_mapEnabled.Remove( nItemID );
			return;
		}

		int idx = m_mapEnabled.Find( nItemID );
		if ( idx != m_mapEnabled.InvalidIndex() )
		{
			if ( m_mapEnabled[idx] == bEnabled )
				return;
			m_mapEnabled[idx] = bEnabled;
		}
		else
		{
			m_mapEnabled.Insert( nItemID, bEnabled );
		}

		m_bNeedsRemount = true;
		SaveConfig();

		if ( engineClient && engineClient->GetEventsSubscription() )
		{
			KeyValues *pEvent = new KeyValues( "Workshop_EnableChange" );
			pEvent->SetUint64( "id", nItemID );
			engineClient->GetEventsSubscription()->BroadcastEvent( pEvent );
		}
	}

	virtual void MoveItemPriority( PublishedFileId_t nItemID, bool bMoveUp ) OVERRIDE
	{
		int nCount = m_vecAddonOrder.Count();
		int idx = m_vecAddonOrder.Find( nItemID );
		if ( idx == -1 || nCount <= 1 )
			return;

		if ( bMoveUp )
		{
			if ( idx == 0 )
				return;
			std::swap( m_vecAddonOrder[idx], m_vecAddonOrder[idx - 1] );
		}
		else
		{
			if ( idx == nCount - 1 )
				return;
			std::swap( m_vecAddonOrder[idx], m_vecAddonOrder[idx + 1] );
		}

		m_bNeedsRemount = true;
		SaveConfig();

		if ( engineClient && engineClient->GetEventsSubscription() )
		{
			KeyValues *pEvent = new KeyValues( "Workshop_PriorityChange" );
			pEvent->SetUint64( "id", nItemID );
			engineClient->GetEventsSubscription()->BroadcastEvent( pEvent );
		}
	}

	virtual bool IsItemEnabled( PublishedFileId_t nItemID ) OVERRIDE
	{
		int idx = m_mapEnabled.Find( nItemID );
		if ( idx != m_mapEnabled.InvalidIndex() )
			return m_mapEnabled[idx];
		return workshop_addon_enable_by_default.GetBool();
	}

	virtual bool LoadCampaign( PublishedFileId_t nCampaignID, bool bStartMap ) OVERRIDE
	{
		if ( nCampaignID == m_ulCurrentWorkshopCampaign && m_bLaunchingWorkshopCampaign )
			return true;

		WorkshopItem_t *pItem = GetItem( nCampaignID );
		if ( !pItem || !pItem->IsValidCampaign() || !pItem->m_pCampaignData )
			return false;

		const char *pszStartingMap = pItem->m_pCampaignData->GetString( "starting_map", "" );
		if ( !pszStartingMap || !pszStartingMap[0] )
		{
			ConColorMsg( 0, Color( 0, 255, 255, 255 ), "Campaign with no map name!\n" );
			return false;
		}

		size_t nLen = strlen( pszStartingMap );
		for ( size_t i = 0; i < nLen; ++i )
		{
			if ( !IsValidMapNameChar( pszStartingMap[i] ) )
			{
				ConColorMsg( 0, Color( 0, 255, 255, 255 ),
					"Couldn't start campaign as it had a naughty map name: %s\n", pszStartingMap );
				return false;
			}
		}

		m_bLaunchingWorkshopCampaign = true;
		m_ulCurrentWorkshopCampaign = nCampaignID;
		m_bNeedsRemount = true;

		ConVarRef workshop_playing_campaign( "workshop_playing_campaign" );
		if ( workshop_playing_campaign.IsValid() )
			workshop_playing_campaign.SetValue( 1 );

		const char *pszExpansion = pItem->m_pCampaignData->GetString( "expansion", "ep2" );
		const char *pszMod = "workshop_ep2";
		if ( !strcmp( pszExpansion, "hl2" ) )
			pszMod = "workshop_hl2";
		else if ( !strcmp( pszExpansion, "episodic" ) || !strcmp( pszExpansion, "ep1" ) )
			pszMod = "workshop_episodic";

		char szLaunchMod[512];
		V_sprintf_safe( szLaunchMod, "launchmod %s\n", pszMod );

		if ( engineClient )
		{
			if ( bStartMap )
			{
				engineClient->ClientCmd_Unrestricted( "disconnect\n" );
				engineClient->ClientCmd_Unrestricted( szLaunchMod );

				char szMapCmd[1024];
				V_snprintf( szMapCmd, sizeof( szMapCmd ), "map %s\n", pszStartingMap );
				engineClient->ClientCmd_Unrestricted( szMapCmd );
			}
			else
			{
				engineClient->ExecuteClientCmd( "disconnect\n" );
				engineClient->ExecuteClientCmd( szLaunchMod );
			}

			if ( engineClient->GetEventsSubscription() )
			{
				KeyValues *pEvent = new KeyValues( "Workshop_LaunchedCampaign" );
				engineClient->GetEventsSubscription()->BroadcastEvent( pEvent );
			}
		}

		return true;
	}

	virtual void GetCurrentCampaign( CUtlVector<PublishedFileId_t> &outEnabledAddons, PublishedFileId_t &outCampaignID ) OVERRIDE
	{
		outEnabledAddons.RemoveAll();
		outCampaignID = 0;

		FOR_EACH_MAP( m_mapItems, i )
		{
			PublishedFileId_t id = m_mapItems.Key( i );
			WorkshopItem_t *pItem = m_mapItems.Element( i );
			if ( pItem && pItem->IsValidAsset() && IsItemEnabled( id ) )
			{
				outEnabledAddons.AddToTail( id );
			}
		}

		outCampaignID = m_ulCurrentWorkshopCampaign;
	}

	virtual bool PopulateItemFromQueryResult( UGCQueryHandle_t hQuery, uint32 uIndex, WorkshopItem_t *pOutItem ) OVERRIDE
	{
		if ( !SteamUGC() || !pOutItem )
			return false;

		V_memset( pOutItem->m_ulTags, 0, sizeof( pOutItem->m_ulTags ) );

		uint32 nNumTags = SteamUGC()->GetQueryUGCNumTags( hQuery, uIndex );
		for ( uint32 i = 0; i < nNumTags; ++i )
		{
			char szTag[256];
			SteamUGC()->GetQueryUGCTag( hQuery, uIndex, i, szTag, sizeof( szTag ) );
			szTag[sizeof( szTag ) - 1] = '\0';

			for ( int iCat = 0; iCat < WORKSHOP_TAG_CATEGORY_COUNT; ++iCat )
			{
				const WorkshopTag_t *pTag = GetTagList( (WorkshopTagCategory_t)iCat );
				while ( pTag && pTag->m_pszTag != NULL )
				{
					if ( !V_stricmp( szTag, pTag->m_pszTag ) )
						break;
					++pTag;
				}
				if ( pTag )
					pOutItem->m_ulTags[iCat] |= pTag->m_ulMask;
			}
		}

		bool bResult = SteamUGC()->GetQueryUGCResult( hQuery, uIndex, &pOutItem->m_Details );
		if ( !bResult )
			return false;

		char szPreviewURL[2048];
		if ( SteamUGC()->GetQueryUGCPreviewURL( hQuery, uIndex, szPreviewURL, sizeof( szPreviewURL ) ) )
		{
			pOutItem->m_strPreviewURL = szPreviewURL;
		}

		uint32 nNumKVTags = SteamUGC()->GetQueryUGCNumKeyValueTags( hQuery, uIndex );
		if ( pOutItem->m_pCampaignData )
			pOutItem->m_pCampaignData->deleteThis();
		pOutItem->m_pCampaignData = new KeyValues( "addoninfo" );
		for ( uint32 i = 0; i < nNumKVTags; ++i )
		{
			char szKey[2048];
			char szValue[2048];
			if ( SteamUGC()->GetQueryUGCKeyValueTag( hQuery, uIndex, i, szKey, sizeof( szKey ), szValue, sizeof( szValue ) ) )
			{
				szKey[sizeof( szKey ) - 1] = '\0';
				szValue[sizeof( szValue ) - 1] = '\0';
				pOutItem->m_pCampaignData->SetString( szKey, szValue );
			}
			else
			{
				ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] Skipping UGC KeyValue tag...\n" );
			}
		}

		return bResult;
	}

	virtual IWorkshopImage *GetItemPreviewImage( WorkshopItem_t *pItem ) OVERRIDE
	{
		if ( !pItem )
			return NULL;

		if ( pItem->m_pPreviewImage )
			return pItem->m_pPreviewImage;

		if ( pItem->m_strPreviewURL.IsEmpty() )
			return NULL;

		pItem->m_pPreviewImage = new CWorkshopImage( pItem->m_strPreviewURL.Get() );
		return pItem->m_pPreviewImage;
	}

	virtual const WorkshopTag_t *GetTagList( WorkshopTagCategory_t eCategory ) OVERRIDE
	{
		switch ( eCategory )
		{
		case WORKSHOP_TAG_CATEGORY_TYPE:       return s_TypeTags;
		case WORKSHOP_TAG_CATEGORY_ASSET_TYPE: return s_AssetTypeTags;
		case WORKSHOP_TAG_CATEGORY_WEAPON:     return s_WeaponTags;
		case WORKSHOP_TAG_CATEGORY_NPC:        return s_NPCTags;
		case WORKSHOP_TAG_CATEGORY_VEHICLE:    return s_VehicleTags;
		default:                               return NULL;
		}
	}

	virtual bool DownloadItem( PublishedFileId_t nItemID ) OVERRIDE
	{
		return SteamUGC() ? SteamUGC()->DownloadItem( nItemID, true ) : false;
	}

	virtual void OnLevelChange() OVERRIDE
	{
		if ( m_bLaunchingWorkshopCampaign )
		{
			m_bLaunchingWorkshopCampaign = false;
			return;
		}

		if ( m_ulCurrentWorkshopCampaign != 0 )
		{
			m_ulCurrentWorkshopCampaign = 0;
			ConVarRef workshop_playing_campaign( "workshop_playing_campaign" );
			if ( workshop_playing_campaign.IsValid() )
				workshop_playing_campaign.SetValue( 0 );
			m_bNeedsRemount = true;
		}
	}

	virtual void Dump() OVERRIDE
	{
		ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] m_ulCurrentWorkshopCampaign: %llu\n", (unsigned long long)m_ulCurrentWorkshopCampaign );
		ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] m_bPaused: %s\n", m_bPaused ? "true" : "false" );
		ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] m_bLaunchingWorkshopCampaign: %s\n", m_bLaunchingWorkshopCampaign ? "true" : "false" );
		ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] m_bNeedsRemount: %s\n", m_bNeedsRemount ? "true" : "false" );
		ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] Mounted Addons:\n" );

		for ( int i = m_vecMountedAddons.Count() - 1; i >= 0; --i )
		{
			PublishedFileId_t id = m_vecMountedAddons[i];
			WorkshopItem_t *pItem = GetItem( id );
			const char *pszTitle = pItem ? pItem->m_Details.m_rgchTitle : "Unknown (Unsubscribed now?)";
			ConColorMsg( 0, Color( 0, 255, 255, 255 ), "    %s (%llu)\n", pszTitle, (unsigned long long)id );
		}
	}

	virtual void SetItemVote( WorkshopItem_t *pItem, bool bVoteUp ) OVERRIDE
	{
		if ( !pItem )
			return;
		pItem->m_bVotedUp = bVoteUp;
		pItem->m_bVotedDown = !bVoteUp;
		if ( SteamUGC() )
			SteamUGC()->SetUserItemVote( pItem->m_Details.m_nPublishedFileId, bVoteUp );
	}

	//-------------------------------------------------------------------------
	// IMatchEventsSink
	//-------------------------------------------------------------------------
	virtual void OnEvent( KeyValues *pEvent ) OVERRIDE
	{
		if ( pEvent )
			pEvent->GetName();
	}

private:
	STEAM_CALLBACK( CWorkshop, OnUserSubscribedItemsListChanged, UserSubscribedItemsListChanged_t, m_CallbackSubscriptionsChanged )
	{
		if ( !pParam || pParam->m_nAppID != GetAppID() )
			return;

		ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] Subscriptions changed\n" );
		RefreshSubscribedItems();
	}

	STEAM_CALLBACK( CWorkshop, OnDownloadItemResult, DownloadItemResult_t, m_CallbackItemDownloaded )
	{
		if ( !pParam || pParam->m_eResult != k_EResultOK || pParam->m_unAppID != GetAppID() )
			return;

		ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] Item %llu downloaded.\n", (unsigned long long)pParam->m_nPublishedFileId );

		WorkshopItem_t *pItem = GetItem( pParam->m_nPublishedFileId );
		if ( pItem )
			UpdateItemInstallFolder( pItem );
		else
			RefreshSubscribedItems();
	}

	void OnSteamUGCQueryCompleted( SteamUGCQueryCompleted_t *pResult, bool bIOFailure )
	{
		if ( !SteamUGC() || bIOFailure || !pResult || pResult->m_eResult != k_EResultOK )
		{
			ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] EnumerateSubscribedFiles failed...\n" );
			if ( pResult && SteamUGC() && pResult->m_handle != k_UGCQueryHandleInvalid )
				SteamUGC()->ReleaseQueryUGCRequest( pResult->m_handle );
			return;
		}

		FOR_EACH_MAP_FAST( m_mapTempItems, i )
		{
			delete m_mapTempItems[i];
		}
		m_mapTempItems.Purge();

		for ( uint32 i = 0; i < pResult->m_unNumResultsReturned; ++i )
		{
			WorkshopItem_t *pItem = new WorkshopItem_t();
			if ( !PopulateItemFromQueryResult( pResult->m_handle, i, pItem ) )
			{
				delete pItem;
				continue;
			}

			PublishedFileId_t id = pItem->m_Details.m_nPublishedFileId;
			WorkshopItem_t *pOldItem = GetItem( id );
			if ( !pOldItem )
			{
				ConColorMsg( 1, Color( 0, 255, 255, 255 ), "[CWorkshop] Subscribed to item: %s (%llu)\n",
					pItem->m_Details.m_rgchTitle, (unsigned long long)id );
			}
			else if ( pOldItem->m_Details.m_hPreviewFile == pItem->m_Details.m_hPreviewFile )
			{
				pItem->m_pPreviewImage = pOldItem->m_pPreviewImage;
				pOldItem->m_pPreviewImage = NULL;
			}

			uint32 uState = SteamUGC()->GetItemState( id );
			bool bInstalled = ( uState & k_EItemStateInstalled ) != 0;

			if ( ( uState & k_EItemStateNeedsUpdate ) ||
				 !( uState & ( k_EItemStateInstalled | k_EItemStateDownloading | k_EItemStateDownloadPending ) ) )
			{
				if ( SteamUGC()->DownloadItem( id, false ) )
				{
					ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] Set item %s (%llu) off downloading...\n",
						pItem->m_Details.m_rgchTitle, (unsigned long long)id );
				}
				else
				{
					ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] DownloadItem failed for file %s (%llu).\n",
						pItem->m_Details.m_rgchTitle, (unsigned long long)id );
					delete pItem;
					continue;
				}
			}

			m_mapTempItems.InsertOrReplace( id, pItem );

			if ( !pItem->IsCampaign() )
			{
				if ( m_vecAddonOrder.Find( id ) == -1 )
				{
					m_vecAddonOrder.AddToHead( id );
				}
			}

			if ( m_mapEnabled.Find( id ) == m_mapEnabled.InvalidIndex() )
			{
				if ( pItem->IsValidAsset() )
				{
					m_mapEnabled.Insert( id, workshop_addon_enable_by_default.GetBool() );
				}
			}

			if ( !pItem->IsValidAsset() )
			{
				m_mapEnabled.Remove( id );
			}

			if ( bInstalled )
			{
				UpdateItemInstallFolder( pItem );
			}

			if ( IsItemEnabled( id ) )
			{
				m_bNeedsRemount = true;
			}

			if ( !pItem->m_CallResultGetUserItemVote.IsActive() )
			{
				SteamAPICall_t hVoteCall = SteamUGC()->GetUserItemVote( id );
				if ( hVoteCall != k_uAPICallInvalid )
				{
					pItem->m_CallResultGetUserItemVote.Set( hVoteCall, pItem, &WorkshopItem_t::OnGetUserItemVote );
				}
			}

			IWorkshopImage *pImg = GetItemPreviewImage( pItem );
			if ( pImg )
			{
				pImg->LoadImage( 1000 + (int)i );
			}
		}

		FOR_EACH_MAP_FAST( m_mapItems, i )
		{
			WorkshopItem_t *pOldItem = m_mapItems[i];
			PublishedFileId_t id = pOldItem->m_Details.m_nPublishedFileId;
			int newIdx = m_mapTempItems.Find( id );
			if ( newIdx == m_mapTempItems.InvalidIndex() || m_mapTempItems[newIdx] == NULL )
			{
				ConColorMsg( 0, Color( 0, 255, 255, 255 ), "[CWorkshop] Item %s (%llu) was unsubscribed from.\n",
					pOldItem->m_Details.m_rgchTitle, (unsigned long long)id );
				m_mapEnabled.Remove( id );
				m_vecAddonOrder.FindAndRemove( id );
			}
		}

		FOR_EACH_MAP_FAST( m_mapItems, i )
		{
			delete m_mapItems[i];
		}
		m_mapItems.Purge();
		FOR_EACH_MAP_FAST( m_mapTempItems, i )
		{
			m_mapItems.InsertOrReplace( m_mapTempItems.Key( i ), m_mapTempItems[i] );
		}
		m_mapTempItems.Purge();

		for ( int i = m_vecAddonOrder.Count() - 1; i >= 0; --i )
		{
			WorkshopItem_t *pItem = GetItem( m_vecAddonOrder[i] );
			if ( !pItem || pItem->IsCampaign() )
			{
				m_vecAddonOrder.Remove( i );
			}
		}

		if ( engineClient && engineClient->GetEventsSubscription() )
		{
			KeyValues *pEvent = new KeyValues( "Workshop_SubscriptionsUpdated" );
			engineClient->GetEventsSubscription()->BroadcastEvent( pEvent );
		}

		SaveConfig();
		SteamUGC()->ReleaseQueryUGCRequest( pResult->m_handle );
	}

	bool UpdateItemInstallFolder( WorkshopItem_t *pItem )
	{
		if ( !pItem || !SteamUGC() )
			return false;

		pItem->m_strInstallFolder.Set( NULL );

		uint64 ulSizeOnDisk = 0;
		uint32 uTimeStamp = 0;
		char szFolder[4096];
		szFolder[0] = '\0';

		bool bOK = SteamUGC()->GetItemInstallInfo(
			pItem->m_Details.m_nPublishedFileId,
			&ulSizeOnDisk,
			szFolder,
			sizeof( szFolder ),
			&uTimeStamp );

		if ( bOK )
			pItem->m_strInstallFolder = szFolder;

		return bOK;
	}

	const char *GetItemInstallFolderInternal( WorkshopItem_t *pItem )
	{
		if ( !pItem )
			return NULL;

		if ( pItem->m_strInstallFolder.IsEmpty() )
			UpdateItemInstallFolder( pItem );
		if ( pItem->m_strInstallFolder.IsEmpty() )
			return NULL;

		return pItem->m_strInstallFolder.Get();
	}

	bool CheckForNewlyInstalledItems()
	{
		bool bAnyUpdated = false;
		for ( int i = 0; i < m_vecAddonOrder.Count(); ++i )
		{
			PublishedFileId_t id = m_vecAddonOrder[i];
			if ( id == 0 )
				continue;

			WorkshopItem_t *pItem = GetItem( id );
			if ( !pItem || !pItem->IsValidAsset() )
				continue;

			if ( pItem->m_strInstallFolder.IsEmpty() )
			{
				if ( GetItemInstallFolderInternal( pItem ) != NULL )
				{
					bAnyUpdated = true;
					m_bNeedsRemount = true;
				}
			}
		}
		return bAnyUpdated;
	}

	void StopPlaytimeTracking()
	{
		if ( SteamUGC() )
			SteamUGC()->StopPlaytimeTrackingForAllItems();
	}

	void UpdatePlaytimeTracking()
	{
		if ( !SteamUGC() )
			return;

		StopPlaytimeTracking();
		if ( engineClient && engineClient->IsPaused() )
			return;

		if ( m_vecMountedAddons.Count() > 0 )
			SteamUGC()->StartPlaytimeTracking( m_vecMountedAddons.Base(), m_vecMountedAddons.Count() );
	}

	void SaveConfig()
	{
		if ( !g_pFileSystem )
			return;

		KeyValues *pKV = new KeyValues( "AddonInfo" );
		for ( int i = 0; i < m_vecAddonOrder.Count(); ++i )
		{
			PublishedFileId_t id = m_vecAddonOrder[i];
			char szID[64];
			V_sprintf_safe( szID, "%llu", (unsigned long long)id );
			pKV->SetInt( szID, IsItemEnabled( id ) ? 1 : 0 );
		}
		pKV->SaveToFile( g_pFileSystem, "cfg/workshop.txt", "ROOT_MOD" );
		pKV->deleteThis();
	}

	void LoadConfig()
	{
		if ( !g_pFileSystem )
			return;

		KeyValues *pKV = new KeyValues( "AddonInfo" );
		if ( pKV->LoadFromFile( g_pFileSystem, "cfg/workshop.txt", "ROOT_MOD" ) )
		{
			for ( KeyValues *pSub = pKV->GetFirstSubKey(); pSub != NULL; pSub = pSub->GetNextKey() )
			{
				PublishedFileId_t id = 0;
				sscanf( pSub->GetName(), "%llu", (unsigned long long *)&id );
				if ( id != 0 )
				{
					m_vecAddonOrder.FindAndRemove( id );
					m_vecAddonOrder.AddToTail( id );
					m_mapEnabled.InsertOrReplace( id, pSub->GetBool() );
				}
			}
		}
		pKV->deleteThis();
	}

	void MountItem( WorkshopItem_t *pItem )
	{
		const char *pszFolder = GetItemInstallFolderInternal( pItem );
		if ( !pszFolder || !pszFolder[0] )
			return;

		if ( !pItem->IsMountable() )
			return;

		char szVPK[4096];
		V_snprintf( szVPK, sizeof( szVPK ), "%s/workshop.vpk", pszFolder );

		ConColorMsg( 0, Color( 0, 255, 255, 255 ), "Mounting workshop item %s (%llu)\n",
			pItem->m_Details.m_rgchTitle, (unsigned long long)pItem->m_Details.m_nPublishedFileId );

		g_pFileSystem->AddSearchPath( szVPK, "WORKSHOP_ASSET", PATH_ADD_TO_HEAD );
		g_pFileSystem->AddSearchPath( szVPK, "WORKSHOP_ASSET_TRACKING", PATH_ADD_TO_HEAD );

		m_vecMountedAddons.AddToTail( pItem->m_Details.m_nPublishedFileId );
	}

	bool GetRawSubscribedItems( CUtlVector<PublishedFileId_t> &out )
	{
		out.RemoveAll();
		if ( !SteamUGC() )
			return false;

		uint32 nCount = SteamUGC()->GetNumSubscribedItems();
		if ( nCount > 0 )
		{
			out.SetCount( nCount );
			uint32 nGot = SteamUGC()->GetSubscribedItems( out.Base(), out.Count() );
			out.SetCount( nGot );
		}
		return true;
	}

	bool m_bInitialized;
	bool m_bPaused;
	bool m_bLaunchingWorkshopCampaign;
	PublishedFileId_t m_ulCurrentWorkshopCampaign;

	CCallResult<CWorkshop, SteamUGCQueryCompleted_t> m_CallResultUGCQuery;

	CUtlMap<PublishedFileId_t, bool, int> m_mapEnabled;
	CUtlVector<PublishedFileId_t>         m_vecAddonOrder;
	CUtlVector<PublishedFileId_t>         m_vecMountedAddons;
	bool                                  m_bNeedsRemount;
	CUtlMap<PublishedFileId_t, WorkshopItem_t *, int> m_mapItems;
	CUtlMap<PublishedFileId_t, WorkshopItem_t *, int> m_mapTempItems;
};

static CWorkshop g_Workshop;
IWorkshop *workshop = &g_Workshop;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CWorkshop, IWorkshop, VWORKSHOP_INTERFACE_VERSION, g_Workshop );

CON_COMMAND( workshop_dump, "" )
{
	workshop->Dump();
}

CON_COMMAND( workshop_refresh_subscribed, "Force a refresh of all our subscribed items" )
{
	workshop->RefreshSubscribedItems();
}

CON_COMMAND( workshop_unmount_all, "Unmount all workshop content" )
{
	workshop->UnmountAll();
}

CON_COMMAND( workshop_remount_all, "Remount all workshop content" )
{
	workshop->RemountAddons( false );
}

#elif !defined( OSX )
IWorkshop *workshop = NULL;
#endif

CON_COMMAND( workshop_open_manager, "Open workshop manager" )
{
	engineClient->ClientCmd_Unrestricted( "gamemenucommand openworkshopdialog\n" );
}
