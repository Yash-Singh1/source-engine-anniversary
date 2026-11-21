//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine Steam Workshop interface (HL2 20th Anniversary Edition).
//
//===========================================================================//

#ifndef IWORKSHOP_H
#define IWORKSHOP_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/IAppSystem.h"
#include "tier1/utlvector.h"
#include "tier1/utlstring.h"
#include "tier1/KeyValues.h"
#include "vgui/IImage.h"
#include "steam/steam_api.h"
#include "steam/isteamugc.h"

#define VWORKSHOP_INTERFACE_VERSION "VWorkshop001"

enum WorkshopTagCategory_t
{
	WORKSHOP_TAG_CATEGORY_TYPE = 0,
	WORKSHOP_TAG_CATEGORY_ASSET_TYPE = 1,
	WORKSHOP_TAG_CATEGORY_WEAPON = 2,
	WORKSHOP_TAG_CATEGORY_NPC = 3,
	WORKSHOP_TAG_CATEGORY_VEHICLE = 4,
	WORKSHOP_TAG_CATEGORY_COUNT = 5,
};

struct WorkshopTag_t
{
	const char *m_pszTag;
	uint64      m_ulMask;
};

abstract_class IWorkshopImage : public vgui::IImage
{
public:
	virtual void LoadImage( int nPriority = 0 ) = 0;
};

struct WorkshopItem_t
{
	WorkshopItem_t()
	{
		V_memset( &m_Details, 0, sizeof( m_Details ) );
		m_pPreviewImage = NULL;
		V_memset( m_ulTags, 0, sizeof( m_ulTags ) );
		m_pCampaignData = NULL;
		m_bUnknown_265c = false;
		m_bVotedUp = false;
		m_bVotedDown = false;
	}

	~WorkshopItem_t()
	{
		if ( m_pCampaignData )
		{
			m_pCampaignData->deleteThis();
			m_pCampaignData = NULL;
		}
		delete m_pPreviewImage;
		m_pPreviewImage = NULL;
	}

	bool IsCampaign() const      { return ( m_ulTags[WORKSHOP_TAG_CATEGORY_TYPE] & ( 1ULL << 0 ) ) != 0; }
	bool IsAsset() const         { return ( m_ulTags[WORKSHOP_TAG_CATEGORY_TYPE] & ( 1ULL << 1 ) ) != 0; }
	bool IsValid() const         { return m_Details.m_nPublishedFileId != 0 && m_Details.m_ulTotalFilesSize != 0; }
	bool IsValidAsset() const    { return !IsCampaign() && IsValid() && IsAsset(); }
	bool IsValidCampaign() const { return IsCampaign() && IsValid(); }
	bool IsMountable() const     { return IsValid() && ( ( m_ulTags[WORKSHOP_TAG_CATEGORY_TYPE] & 0x3ULL ) != 0 ); }

	SteamUGCDetails_t   m_Details;                                   // +0x0000 (0x262c on i386)
	IWorkshopImage     *m_pPreviewImage;                             // +0x262c
	uint64              m_ulTags[WORKSHOP_TAG_CATEGORY_COUNT];       // +0x2630 (0x28)
	KeyValues          *m_pCampaignData;                             // +0x2658 ("addoninfo" KV tags)
	bool                m_bUnknown_265c;                             // +0x265c
	bool                m_bVotedUp;                                  // +0x265d
	bool                m_bVotedDown;                                // +0x265e
	CUtlString          m_strInstallFolder;                          // +0x2660
	CUtlString          m_strPreviewURL;                             // +0x2670

	CCallResult<WorkshopItem_t, GetUserItemVoteResult_t> m_CallResultGetUserItemVote; // +0x2680
	void OnGetUserItemVote( GetUserItemVoteResult_t *pResult, bool bIOFailure )
	{
		if ( !bIOFailure && pResult && pResult->m_eResult == k_EResultOK )
		{
			m_bVotedUp = pResult->m_bVotedUp;
			m_bVotedDown = pResult->m_bVotedDown;
		}
	}
};

abstract_class IWorkshop : public IAppSystem
{
public:
	// [5]  (0x14) Called every engine frame (_Host_RunFrame)
	virtual void            Frame() = 0;
	// [6]  (0x18) Returns addon order vector (m_vecAddonOrder)
	virtual bool            GetSubscribedItemsInOrder( CUtlVector<PublishedFileId_t> &items ) = 0;
	// [7]  (0x1c) Returns all subscribed items sorted by m_rtimeAddedToUserList ascending
	virtual bool            GetSubscribedItemsSortedByTime( CUtlVector<PublishedFileId_t> &items ) = 0;
	// [8]  (0x20) Queries SteamUGC for details of all subscribed items
	virtual bool            RefreshSubscribedItems() = 0;
	// [9]  (0x24) Looks up item in m_mapItems
	virtual WorkshopItem_t *GetItem( PublishedFileId_t nItemID ) = 0;
	// [10] (0x28) Returns item install folder string (or NULL)
	virtual const char     *GetItemInstallFolder( PublishedFileId_t nItemID ) = 0;
	// [11] (0x2c) Remounts campaign + enabled addons if needed or bForce
	virtual bool            RemountAddons( bool bForce ) = 0;
	// [12] (0x30) Removes WORKSHOP_ASSET / WORKSHOP_ASSET_TRACKING search paths & stops playtime tracking
	virtual void            UnmountAll() = 0;
	// [13] (0x34) Loads Resource/*_%language%.txt from WORKSHOP_ASSET_TRACKING and refreshes map list
	virtual void            ReloadLocalizationAndMapList() = 0;
	// [14] (0x38) Returns SteamUtils()->GetAppID(), or 220 if SteamUtils() is NULL
	virtual AppId_t         GetAppID() = 0;
	// [15] (0x3c) Enables/disables an asset addon, saves cfg/workshop.txt, broadcasts Workshop_EnableChange
	virtual void            SetItemEnabled( PublishedFileId_t nItemID, bool bEnabled ) = 0;
	// [16] (0x40) Swaps item one step up (bMoveUp=true) or down in m_vecAddonOrder, saves & broadcasts Workshop_PriorityChange
	virtual void            MoveItemPriority( PublishedFileId_t nItemID, bool bMoveUp ) = 0;
	// [17] (0x44) Returns enabled state from m_mapEnabled, falling back to workshop_addon_enable_by_default
	virtual bool            IsItemEnabled( PublishedFileId_t nItemID ) = 0;
	// [18] (0x48) Launches a Workshop campaign (disconnect + launchmod + optional map)
	virtual bool            LoadCampaign( PublishedFileId_t nCampaignID, bool bStartMap ) = 0;
	// [19] (0x4c) Fills outEnabledAddons with enabled asset items and sets outCampaignID = m_ulCurrentWorkshopCampaign
	virtual void            GetCurrentCampaign( CUtlVector<PublishedFileId_t> &items, PublishedFileId_t &nCampaignID ) = 0;
	// [20] (0x50) Populates a WorkshopItem_t from a SteamUGC query result entry
	virtual bool            PopulateItemFromQueryResult( UGCQueryHandle_t hQuery, uint32 uIndex, WorkshopItem_t *pOutItem ) = 0;
	// [21] (0x54) Returns (or lazily creates) CWorkshopImage for pItem->m_strPreviewURL
	virtual IWorkshopImage *GetItemPreviewImage( WorkshopItem_t *pItem ) = 0;
	// [22] (0x58) Returns NULL-terminated WorkshopTag_t table for category 0..4, or NULL
	virtual const WorkshopTag_t *GetTagList( WorkshopTagCategory_t eCategory ) = 0;
	// [23] (0x5c) Calls SteamUGC()->DownloadItem(nItemID, true)
	virtual bool            DownloadItem( PublishedFileId_t nItemID ) = 0;
	// [24] (0x60) Called on disconnect/level change; consumes m_bLaunchingWorkshopCampaign or clears active campaign
	virtual void            OnLevelChange() = 0;
	// [25] (0x64) Prints state and mounted addons to console (workshop_dump)
	virtual void            Dump() = 0;
	// [26] (0x68) Updates pItem->m_bVotedUp/Down and calls SteamUGC()->SetUserItemVote
	virtual void            SetItemVote( WorkshopItem_t *pItem, bool bVoteUp ) = 0;

	// Compatibility aliases for existing call sites in host/client code
	inline void Unknown05() { Frame(); }
	inline bool Unknown11( bool bArg ) { return RemountAddons( bArg ); }
	inline void Unknown24() { OnLevelChange(); }
};

extern IWorkshop *workshop;

#endif // IWORKSHOP_H
