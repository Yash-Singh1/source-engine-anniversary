//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Retail HL2 20th Anniversary Steam Workshop UI dialogs
//          Decompiled from retail GameUI.so.
//
//=============================================================================//

#include "WorkshopDialog.h"
#include "BasePanel.h"
#include "EngineInterface.h"
#include "filesystem.h"
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui/ISystem.h"
#include "vgui/IVGui.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/MessageBox.h"
#include "tier1/convar.h"
#include "tier1/strtools.h"
#include "tier1/utlbuffer.h"
#include "bitmap/bitmap.h"
#include "imageutils.h"
#include "steam/steam_api.h"

#if defined( OSX )
#include <unistd.h>
#include <stdio.h>
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

static ConVar workshop_dev_stress_test( "workshop_dev_stress_test", "0", FCVAR_DEVELOPMENTONLY );

static CWorkshopItemPanel *s_pSelectedPanel = NULL;
static KeyValues *s_pGridResKV = NULL;
static KeyValues *s_pListResKV = NULL;

static void OpenURLInOverlayOrBrowser( const char *pszURL )
{
	if ( !pszURL || !pszURL[0] )
		return;

	if ( SteamFriends() && SteamUtils() && SteamUtils()->IsOverlayEnabled() )
	{
		SteamFriends()->ActivateGameOverlayToWebPage( pszURL, k_EActivateGameOverlayToWebPageMode_Default );
		return;
	}

#if defined( OSX )
	pid_t pid = fork();
	if ( pid == 0 )
	{
		execlp( "/usr/bin/open", "open", pszURL, (char *)NULL );
		_exit( 127 );
	}
#endif
}

static bool IsValidWorkshopMapNameChar( char c )
{
	if ( (unsigned char)( c - '0' ) <= 9 )
		return true;
	if ( (unsigned char)( c - 'A' ) <= 25 )
		return true;
	return ( (unsigned char)( c - 'a' ) <= 25 ) || c == '_';
}

#if defined( OSX )
static bool RunOsascriptDialog( const char *pszCommand, char *pszOutPath, size_t outSize )
{
	pszOutPath[0] = '\0';
	FILE *fp = popen( pszCommand, "r" );
	if ( !fp )
		return false;
	bool bGot = ( fgets( pszOutPath, (int)outSize, fp ) != NULL );
	int rc = pclose( fp );
	if ( !bGot || rc != 0 )
	{
		pszOutPath[0] = '\0';
		return false;
	}
	size_t len = strlen( pszOutPath );
	while ( len > 0 && ( pszOutPath[len - 1] == '\n' || pszOutPath[len - 1] == '\r' || pszOutPath[len - 1] == '/' ) )
		pszOutPath[--len] = '\0';
	return len > 0;
}
#endif

//-----------------------------------------------------------------------------
// Local preview bitmap image (CBitmapImage in retail GameUI.so)
//-----------------------------------------------------------------------------
class CWorkshopLocalBitmapImage : public vgui::IImage
{
public:
	CWorkshopLocalBitmapImage()
		: m_iTextureID( -1 ), m_nX( 0 ), m_nY( 0 ), m_nWide( 0 ), m_nTall( 0 ), m_Color( 255, 255, 255, 255 ) {}

	virtual ~CWorkshopLocalBitmapImage()
	{
		if ( m_iTextureID >= 0 && g_pVGuiSurface )
		{
			g_pVGuiSurface->DestroyTextureID( m_iTextureID );
			m_iTextureID = -1;
		}
	}

	void SetRGBA( const unsigned char *pRGBA, int wide, int tall )
	{
		if ( !g_pVGuiSurface || !g_pMatSystemSurface )
			return;
		if ( m_iTextureID >= 0 )
			g_pVGuiSurface->DestroyTextureID( m_iTextureID );
		m_iTextureID = g_pVGuiSurface->CreateNewTextureID( true );
		m_nWide = wide;
		m_nTall = tall;
		g_pMatSystemSurface->DrawSetTextureRGBAEx2( m_iTextureID, pRGBA, wide, tall, IMAGE_FORMAT_RGBA8888, true, false );
	}

	virtual void Paint() OVERRIDE
	{
		if ( m_iTextureID < 0 || !g_pVGuiSurface )
			return;
		g_pVGuiSurface->DrawSetTexture( m_iTextureID );
		g_pVGuiSurface->DrawSetColor( m_Color );
		g_pVGuiSurface->DrawTexturedRect( m_nX, m_nY, m_nX + m_nWide, m_nY + m_nTall );
	}

	virtual void SetPos( int x, int y ) OVERRIDE { m_nX = x; m_nY = y; }
	virtual void GetContentSize( int &wide, int &tall ) OVERRIDE { wide = m_nWide; tall = m_nTall; }
	virtual void GetSize( int &wide, int &tall ) OVERRIDE { wide = m_nWide; tall = m_nTall; }
	virtual void SetSize( int wide, int tall ) OVERRIDE { m_nWide = wide; m_nTall = tall; }
	virtual void SetColor( Color col ) OVERRIDE { m_Color = col; }
	virtual bool Evict() OVERRIDE { return false; }
	virtual int GetNumFrames() OVERRIDE { return 0; }
	virtual void SetFrame( int nFrame ) OVERRIDE {}
	virtual vgui::HTexture GetID() OVERRIDE { return m_iTextureID; }
	virtual void SetRotation( int iRotation ) OVERRIDE {}

private:
	int m_iTextureID;
	int m_nX, m_nY, m_nWide, m_nTall;
	Color m_Color;
};

//-----------------------------------------------------------------------------
// CSteamUGCUserItemsQuery (My Uploads paginated published items query)
//-----------------------------------------------------------------------------
class CSteamUGCUserItemsQuery
{
public:
	explicit CSteamUGCUserItemsQuery( CWorkshopMyUploadsPage *pOwner )
		: m_pOwner( pOwner )
		, m_unCurrentPage( 0 )
		, m_unNumFetched( 0 )
		, m_unTotalMatching( 0 )
		, m_hQuery( k_UGCQueryHandleInvalid )
	{
	}

	~CSteamUGCUserItemsQuery()
	{
		CancelAndRelease();
		for ( int i = 0; i < m_Items.Count(); ++i )
			delete m_Items[i];
		m_Items.RemoveAll();
	}

	void CancelAndRelease()
	{
		m_CallResultUGCQuery.Cancel();
		if ( m_hQuery != k_UGCQueryHandleInvalid && SteamUGC() )
		{
			SteamUGC()->ReleaseQueryUGCRequest( m_hQuery );
		}
		m_hQuery = k_UGCQueryHandleInvalid;
	}

	bool RequestNextPage()
	{
		Msg( "[CSteamUGCUserItems] Requesting page %d of items.\n", m_unCurrentPage + 1 );
		if ( m_hQuery != k_UGCQueryHandleInvalid )
		{
			Msg( "[CSteamUGCUserItems] Aborting... query is already in flight\n" );
			CancelAndRelease();
			return false;
		}

		ISteamUGC *pUGC = SteamUGC();
		if ( !pUGC || !SteamUtils() || !SteamUser() )
		{
			CancelAndRelease();
			return false;
		}

		AppId_t appId = SteamUtils()->GetAppID();
		AccountID_t accountId = SteamUser()->GetSteamID().GetAccountID();
		UGCQueryHandle_t hQuery = pUGC->CreateQueryUserUGCRequest(
			accountId,
			k_EUserUGCList_Published,
			k_EUGCMatchingUGCType_All,
			k_EUserUGCListSortOrder_CreationOrderDesc,
			appId,
			appId,
			m_unCurrentPage + 1 );

		if ( hQuery == k_UGCQueryHandleInvalid )
		{
			Warning( "[CSteamUGCUserItems]Failed to Create Query UGC Request.\n" );
			CancelAndRelease();
			return false;
		}

		pUGC->SetAllowCachedResponse( hQuery, 0 );
		pUGC->SetReturnAdditionalPreviews( hQuery, false );
		pUGC->SetReturnChildren( hQuery, false );
		pUGC->SetReturnKeyValueTags( hQuery, true );
		pUGC->SetReturnLongDescription( hQuery, true );
		pUGC->SetReturnMetadata( hQuery, true );
		pUGC->SetReturnOnlyIDs( hQuery, false );
		pUGC->SetReturnPlaytimeStats( hQuery, 0 );
		pUGC->SetReturnTotalOnly( hQuery, false );

		m_hQuery = hQuery;

		SteamAPICall_t hCall = pUGC->SendQueryUGCRequest( hQuery );
		if ( hCall == k_uAPICallInvalid )
		{
			Warning( "[CSteamUGCUserItems]Failed to Send Query UGC Request.\n" );
			CancelAndRelease();
			return false;
		}

		if ( m_unCurrentPage == 0 )
			Msg( "[CSteamUGCUserItems]Retrieving files from Steam Cloud, please wait..." );
		else
			Msg( "[CSteamUGCUserItems]Requesting %d/%d items from Steam...", m_unNumFetched, m_unTotalMatching );

		m_CallResultUGCQuery.Set( hCall, this, &CSteamUGCUserItemsQuery::OnSteamUGCQueryCompleted );
		return true;
	}

private:
	void OnSteamUGCQueryCompleted( SteamUGCQueryCompleted_t *pResult, bool bIOFailure )
	{
		if ( bIOFailure || !pResult || pResult->m_eResult != k_EResultOK )
		{
			Warning( "[CSteamUGCUserItems]SendQueryUGCRequest() failed. Error=%d Result=%d\n",
				bIOFailure, pResult ? pResult->m_eResult : k_EResultFail );
			CancelAndRelease();
			return;
		}

		if ( !SteamUGC() || !workshop || !m_pOwner || !m_pOwner->m_pList )
		{
			CancelAndRelease();
			return;
		}

		uint32 nNewFetched = m_unNumFetched + pResult->m_unNumResultsReturned;
		m_unTotalMatching = MAX( pResult->m_unTotalMatchingResults, nNewFetched );

		for ( uint32 i = 0; i < pResult->m_unNumResultsReturned; ++i )
		{
			WorkshopItem_t *pItem = new WorkshopItem_t();
			m_Items.AddToTail( pItem );
			if ( workshop->PopulateItemFromQueryResult( pResult->m_handle, i, pItem ) )
			{
				if ( pItem->IsValid() )
				{
					CWorkshopItemPanel *pPanel = new CWorkshopItemPanel( m_pOwner->m_pList, pItem, true, false, true, (int)i );
					m_pOwner->m_pList->AddItem( NULL, pPanel );
				}
			}
		}

		m_unCurrentPage++;
		m_unNumFetched = nNewFetched;

		if ( m_unNumFetched < m_unTotalMatching )
		{
			if ( m_hQuery != k_UGCQueryHandleInvalid && SteamUGC() )
				SteamUGC()->ReleaseQueryUGCRequest( m_hQuery );
			m_hQuery = k_UGCQueryHandleInvalid;
			RequestNextPage();
		}
		else
		{
			CancelAndRelease();
		}
	}

	CCallResult<CSteamUGCUserItemsQuery, SteamUGCQueryCompleted_t> m_CallResultUGCQuery;
	CWorkshopMyUploadsPage       *m_pOwner;
	CUtlVector<WorkshopItem_t *>  m_Items;
	uint32                        m_unCurrentPage;
	uint32                        m_unNumFetched;
	uint32                        m_unTotalMatching;
	UGCQueryHandle_t              m_hQuery;
};

//-----------------------------------------------------------------------------
// CWorkshopDialog
//-----------------------------------------------------------------------------
CWorkshopDialog::CWorkshopDialog( Panel *parent )
	: BaseClass( parent, "WorkshopDialog" )
{
	m_pCampaignsPage = NULL;
	m_pFeaturedPage = NULL;
	m_pSubscribedPage = NULL;
	m_pMyUploadsPage = NULL;
	m_pBrowseButton = NULL;
	m_pLoadButton = NULL;
	m_pUnusedButton = NULL;
	m_pEnableAllButton = NULL;
	m_pDisableAllButton = NULL;
	m_pNewItemButton = NULL;
	m_hFont = vgui::INVALID_FONT;

	SetDeleteSelfOnClose( true );
	SetSize( QuickPropScale( 640 ), QuickPropScale( 640 ) );
	SetMinimumSize( QuickPropScale( 340 ), QuickPropScale( 320 ) );
	SetSizeable( true );
	SetTitle( "#GameUI_Workshop_Title", true );

	m_pCampaignsPage  = new CWorkshopCampaignsPage( this, "CampaignsPage" );
	m_pSubscribedPage = new CWorkshopSubscribedPage( this, "SubscribedPage" );
	m_pMyUploadsPage  = new CWorkshopMyUploadsPage( this, "MyUploadsPage" );
	m_pMyUploadsPage->SetVisible( false );

	m_pBrowseButton     = new Button( this, "BrowseButton", "#GameUI_Workshop_Browse",     this, "Browse" );
	m_pLoadButton       = new Button( this, "LoadButton",   "#GameUI_Workshop_LoadGame",   this, "LoadWorkshopSave" );
	m_pEnableAllButton  = new Button( this, "EnableAll",    "#GameUI_Workshop_EnableAll",  this, "EnableAll" );
	m_pDisableAllButton = new Button( this, "DisableAll",   "#GameUI_Workshop_DisableAll", this, "DisableAll" );
	m_pNewItemButton    = new Button( this, "NewItem",      "#GameUI_Workshop_NewAddon",   this, "NewItem" );

	m_hFont = scheme()->GetIScheme( GetScheme() )->GetFont( "Default", true );

	AddPage( m_pCampaignsPage,  "#GameUI_Workshop_Campaigns" );
	AddPage( m_pSubscribedPage, "#GameUI_Workshop_Subscribed" );

	UpdateLayout();

	SetApplyButtonVisible( false );
	SetOKButtonVisible( false );
	SetOKButtonText( "#GameUI_Workshop_CloseTab" );
	SetCancelButtonVisible( true );
	SetCancelButtonText( "#GameUI_Close" );

	if ( engine && engine->GetEventsSubscription() )
		engine->GetEventsSubscription()->Subscribe( this );

	if ( workshop && SteamUGC() )
	{
		CUtlVector<PublishedFileId_t> vecCampaigns;
		workshop->GetSubscribedItemsSortedByTime( vecCampaigns );
		for ( int i = 0; i < vecCampaigns.Count(); ++i )
		{
			WorkshopItem_t *pItem = workshop->GetItem( vecCampaigns[i] );
			if ( pItem && !pItem->m_CallResultGetUserItemVote.IsActive() )
			{
				SteamAPICall_t hCall = SteamUGC()->GetUserItemVote( pItem->m_Details.m_nPublishedFileId );
				if ( hCall != k_uAPICallInvalid )
				{
					pItem->m_CallResultGetUserItemVote.Set( hCall, pItem, &WorkshopItem_t::OnGetUserItemVote );
				}
			}
		}
	}
}

CWorkshopDialog::~CWorkshopDialog()
{
	if ( engine && engine->GetEventsSubscription() )
		engine->GetEventsSubscription()->Unsubscribe( this );
}

void CWorkshopDialog::OnThink()
{
	UpdateLayout();
	BaseClass::OnThink();
}

void CWorkshopDialog::PerformLayout()
{
	UpdateLayout();
	BaseClass::PerformLayout();
}

void CWorkshopDialog::UpdateLayout()
{
	if ( m_pMyUploadsPage && GetPropertySheet()->FindPage( m_pMyUploadsPage ) == -1 )
	{
		AddPage( m_pMyUploadsPage, "#GameUI_Workshop_MyUploads" );
	}

	Panel *pActive = GetPropertySheet()->GetActivePage();
	if ( m_pCampaignsPage && pActive == m_pCampaignsPage )
	{
		m_pBrowseButton->SetVisible( true );
		m_pEnableAllButton->SetVisible( false );
		m_pDisableAllButton->SetVisible( false );
		m_pNewItemButton->SetVisible( false );
		m_pLoadButton->SetVisible( true );
		SetOKButtonVisible( false );
		SetCancelButtonVisible( true );
		SetSizeable( true );
	}
	else if ( m_pSubscribedPage && pActive == m_pSubscribedPage )
	{
		m_pBrowseButton->SetVisible( true );
		m_pEnableAllButton->SetVisible( true );
		m_pDisableAllButton->SetVisible( true );
		m_pNewItemButton->SetVisible( false );
		m_pLoadButton->SetVisible( false );
		SetOKButtonVisible( false );
		SetCancelButtonVisible( true );
		SetSizeable( true );
	}
	else if ( m_pMyUploadsPage && pActive == m_pMyUploadsPage )
	{
		m_pBrowseButton->SetVisible( false );
		m_pEnableAllButton->SetVisible( false );
		m_pDisableAllButton->SetVisible( false );
		m_pNewItemButton->SetVisible( true );
		m_pLoadButton->SetVisible( false );
		SetOKButtonVisible( false );
		SetCancelButtonVisible( true );
		SetSizeable( true );
	}
	else
	{
		m_pBrowseButton->SetVisible( false );
		m_pEnableAllButton->SetVisible( false );
		m_pDisableAllButton->SetVisible( false );
		m_pNewItemButton->SetVisible( false );
		m_pLoadButton->SetVisible( false );
		SetOKButtonVisible( true );
		SetCancelButtonVisible( true );
		SetSize( QuickPropScale( 640 ), QuickPropScale( 640 ) );
		SetSizeable( false );
	}

	int x, y, wide, tall;
	GetClientArea( x, y, wide, tall );
	int curX = x + QuickPropScale( 8 );
	int curY = y + tall - QuickPropScale( 28 );

	if ( m_pBrowseButton->IsVisible() )
	{
		m_pBrowseButton->SetBounds( curX, curY, QuickPropScale( 72 ), QuickPropScale( 24 ) );
		curX += QuickPropScale( 80 );
	}
	if ( m_pLoadButton->IsVisible() )
	{
		m_pLoadButton->SetBounds( curX, curY, QuickPropScale( 96 ), QuickPropScale( 24 ) );
		curX += QuickPropScale( 104 );
	}
	if ( m_pEnableAllButton->IsVisible() )
	{
		m_pEnableAllButton->SetBounds( curX, curY, QuickPropScale( 96 ), QuickPropScale( 24 ) );
		curX += QuickPropScale( 104 );
	}
	if ( m_pDisableAllButton->IsVisible() )
	{
		m_pDisableAllButton->SetBounds( curX, curY, QuickPropScale( 96 ), QuickPropScale( 24 ) );
		curX += QuickPropScale( 104 );
	}
	if ( m_pNewItemButton->IsVisible() )
	{
		m_pNewItemButton->SetBounds( curX, curY, QuickPropScale( 88 ), QuickPropScale( 24 ) );
		curX += QuickPropScale( 96 );
	}
}

void CWorkshopDialog::OnCommand( const char *command )
{
	if ( !Q_strcmp( command, "OK" ) )
	{
		Panel *pActive = GetPropertySheet()->GetActivePage();
		if ( pActive && dynamic_cast<CWorkshopUploaderPage *>( pActive ) )
		{
			GetPropertySheet()->RemovePage( pActive );
			pActive->MarkForDeletion();
		}
		return;
	}
	if ( !Q_strcmp( command, "Browse" ) )
	{
		char szURL[2048] = {};
		bool bBeta = SteamUtils() && SteamUtils()->GetConnectedUniverse() == k_EUniverseBeta;
		AppId_t appId = workshop ? workshop->GetAppID() : 220;
		Q_snprintf( szURL, sizeof( szURL ),
			bBeta ? "https://beta.steamcommunity.com/app/%d/workshop/"
			      : "https://steamcommunity.com/app/%d/workshop/",
			(int)appId );
		OpenURLInOverlayOrBrowser( szURL );
		return;
	}
	if ( !Q_strcmp( command, "NewItem" ) )
	{
		CWorkshopUploaderPage *pPage = new CWorkshopUploaderPage( this, "UploaderPage", NULL );
		AddPage( pPage, "#GameUI_Workshop_NewAddon" );
		GetPropertySheet()->SetActivePage( GetPropertySheet()->GetPage( GetPropertySheet()->GetNumPages() - 1 ) );
		return;
	}
	if ( !Q_strcmp( command, "LoadWorkshopSave" ) )
	{
		if ( g_pFullFileSystem )
			g_pFullFileSystem->CreateDirHierarchy( "workshop", NULL );
		BasePanel()->OnOpenLoadGameDialog( "workshop" );
		BaseClass::OnCommand( command );
		return;
	}
	if ( !Q_strcmp( command, "DisableAll" ) )
	{
		if ( workshop )
		{
			CUtlVector<PublishedFileId_t> vecItems;
			workshop->GetSubscribedItemsInOrder( vecItems );
			for ( int i = 0; i < vecItems.Count(); ++i )
				workshop->SetItemEnabled( vecItems[i], false );
		}
		BaseClass::OnCommand( command );
		return;
	}
	if ( !Q_strcmp( command, "EnableAll" ) )
	{
		if ( workshop )
		{
			CUtlVector<PublishedFileId_t> vecItems;
			workshop->GetSubscribedItemsInOrder( vecItems );
			for ( int i = 0; i < vecItems.Count(); ++i )
				workshop->SetItemEnabled( vecItems[i], true );
		}
		BaseClass::OnCommand( command );
		return;
	}
	BaseClass::OnCommand( command );
}

void CWorkshopDialog::OnEvent( KeyValues *pEvent )
{
	if ( !pEvent )
		return;

	const char *pszEvent = pEvent->GetName();
	if ( !Q_stricmp( pszEvent, "Workshop_EditItem" ) )
	{
		if ( pEvent->GetUint64( "id", 0 ) == 0 )
			return;
		WorkshopItem_t *pItem = (WorkshopItem_t *)pEvent->GetPtr( "item", NULL );
		if ( !pItem )
			return;

		for ( int i = 0; i < GetPropertySheet()->GetNumPages(); ++i )
		{
			Panel *pPage = GetPropertySheet()->GetPage( i );
			CWorkshopUploaderPage *pUploader = dynamic_cast<CWorkshopUploaderPage *>( pPage );
			if ( pUploader && pUploader->GetPublishedFileId() == pItem->m_Details.m_nPublishedFileId )
			{
				GetPropertySheet()->SetActivePage( pPage );
				return;
			}
		}

		CWorkshopUploaderPage *pUploader = new CWorkshopUploaderPage( this, "UploaderPage", pItem );
		AddPage( pUploader, pItem->m_Details.m_rgchTitle );
		GetPropertySheet()->SetActivePage( GetPropertySheet()->GetPage( GetPropertySheet()->GetNumPages() - 1 ) );
	}
	else if ( !Q_stricmp( pszEvent, "Workshop_LaunchedCampaign" ) )
	{
		OnCommand( "Close" );
	}
}

void CWorkshopDialog::OnKeyCodePressed( KeyCode code )
{
	ButtonCode_t baseCode = GetBaseButtonCode( code );
	if ( baseCode == KEY_XBUTTON_B )
	{
		OnCommand( "Close" );
		return;
	}
	if ( baseCode == KEY_XBUTTON_A ||
		 baseCode == KEY_XBUTTON_UP || baseCode == KEY_XBUTTON_DOWN ||
		 baseCode == KEY_XBUTTON_LEFT || baseCode == KEY_XBUTTON_RIGHT ||
		 baseCode == KEY_XSTICK1_UP || baseCode == KEY_XSTICK1_DOWN ||
		 baseCode == KEY_XSTICK1_LEFT || baseCode == KEY_XSTICK1_RIGHT )
	{
		return;
	}
	BaseClass::OnKeyCodePressed( code );
}

//-----------------------------------------------------------------------------
// CWorkshopCampaignsPage
//-----------------------------------------------------------------------------
CWorkshopCampaignsPage::CWorkshopCampaignsPage( Panel *parent, const char *name )
	: BaseClass( parent, name )
	, m_nItemWidth( 0 )
	, m_nItemHeight( 0 )
	, m_pList( NULL )
{
	SetSize( QuickPropScale( 10 ), QuickPropScale( 10 ) );
	LoadControlSettings( "Resource/WorkshopCampaignsPage.res" );
	if ( engine && engine->GetEventsSubscription() )
		engine->GetEventsSubscription()->Subscribe( this );
}

CWorkshopCampaignsPage::~CWorkshopCampaignsPage()
{
	if ( engine && engine->GetEventsSubscription() )
		engine->GetEventsSubscription()->Unsubscribe( this );
}

void CWorkshopCampaignsPage::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	m_pList = dynamic_cast<PanelListPanel *>( FindChildByName( "ListyList" ) );
	if ( m_pList )
		m_pList->SetFirstColumnWidth( 0 );
	RefreshItems();
}

void CWorkshopCampaignsPage::RefreshItems()
{
	if ( !m_pList || !workshop )
		return;

	m_CampaignPanels.RemoveAll();
	m_pList->DeleteAllItems();

	CUtlVector<PublishedFileId_t> vecCampaigns;
	workshop->GetSubscribedItemsSortedByTime( vecCampaigns );

	int nLoops = workshop_dev_stress_test.GetInt();
	if ( nLoops <= 0 )
		nLoops = 1;

	int nItemsPerRow = 0;
	int nPlaced = 0;
	Panel *pCurrentRow = NULL;

	for ( int loop = 0; loop < nLoops; ++loop )
	{
		for ( int i = 0; i < vecCampaigns.Count(); ++i )
		{
			WorkshopItem_t *pItem = workshop->GetItem( vecCampaigns[i] );
			if ( !pItem || !pItem->IsCampaign() )
				continue;

			CWorkshopItemPanel *pPanel = new CWorkshopItemPanel( this, pItem, false, false, false, i );
			if ( !m_nItemWidth || !m_nItemHeight )
			{
				pPanel->MakeReadyForUse();
				pPanel->InvalidateLayout( true, false );
				m_nItemWidth  = pPanel->GetWide();
				m_nItemHeight = pPanel->GetTall();
			}

			if ( nItemsPerRow == 0 )
			{
				int denom = MAX( 1, m_nItemWidth + 2 * m_nGridSpacing );
				nItemsPerRow = MAX( 1, GetWide() / denom );
			}

			int col = nPlaced % nItemsPerRow;
			++nPlaced;
			if ( col == 0 )
			{
				pCurrentRow = new Panel( this, "CampaignRow" );
				pCurrentRow->SetSize( nItemsPerRow * m_nItemWidth, m_nItemHeight );
				m_pList->AddItem( NULL, pCurrentRow );
			}
			pPanel->SetParent( pCurrentRow );
			pPanel->SetPos( m_nGridSpacing + col * ( m_nItemWidth + m_nGridSpacing ), 0 );
			m_CampaignPanels.AddToTail( pPanel );
		}
	}
}

void CWorkshopCampaignsPage::PerformLayout()
{
	if ( m_CampaignPanels.Count() == 0 )
	{
		RefreshItems();
		BaseClass::PerformLayout();
		return;
	}

	if ( !m_pList || m_nItemWidth <= 0 || m_nItemHeight <= 0 )
	{
		BaseClass::PerformLayout();
		return;
	}

	for ( int i = 0; i < m_CampaignPanels.Count(); ++i )
		m_CampaignPanels[i]->SetParent( (Panel *)NULL );

	int denom = MAX( 1, m_nItemWidth + 2 * m_nGridSpacing );
	int nItemsPerRow = MAX( 1, GetWide() / denom );
	int nRowIndex = 0;
	Panel *pRow = NULL;

	for ( int i = 0; i < m_CampaignPanels.Count(); ++i )
	{
		int col = i % nItemsPerRow;
		if ( col == 0 )
		{
			pRow = ( nRowIndex < m_pList->GetItemCount() ) ? m_pList->GetItemPanel( nRowIndex ) : NULL;
			if ( !pRow )
			{
				pRow = new Panel( this, "CampaignRow" );
				pRow->SetSize( nItemsPerRow * m_nItemWidth, m_nItemHeight );
				m_pList->AddItem( NULL, pRow );
			}
			++nRowIndex;
		}
		m_CampaignPanels[i]->SetParent( pRow );
		m_CampaignPanels[i]->SetPos( m_nGridSpacing + col * ( m_nItemWidth + m_nGridSpacing ), 0 );
	}

	for ( int r = m_pList->GetItemCount() - 1; r >= nRowIndex; --r )
		m_pList->RemoveItem( r );

	BaseClass::PerformLayout();
}

void CWorkshopCampaignsPage::OnEvent( KeyValues *pEvent )
{
	if ( !pEvent )
		return;
	const char *pszName = pEvent->GetName();
	if ( !Q_stricmp( pszName, "Workshop_SubscriptionsUpdated" ) )
		RefreshItems();
}

//-----------------------------------------------------------------------------
// CWorkshopSubscribedPage
//-----------------------------------------------------------------------------
CWorkshopSubscribedPage::CWorkshopSubscribedPage( Panel *parent, const char *name )
	: BaseClass( parent, name )
	, m_pList( NULL )
{
	SetSize( QuickPropScale( 10 ), QuickPropScale( 10 ) );
	LoadControlSettings( "Resource/WorkshopSubscribedPage.res" );
	if ( engine && engine->GetEventsSubscription() )
		engine->GetEventsSubscription()->Subscribe( this );
}

CWorkshopSubscribedPage::~CWorkshopSubscribedPage()
{
	if ( engine && engine->GetEventsSubscription() )
		engine->GetEventsSubscription()->Unsubscribe( this );
}

void CWorkshopSubscribedPage::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	m_pList = dynamic_cast<PanelListPanel *>( FindChildByName( "ListyList" ) );
	if ( m_pList )
		m_pList->SetFirstColumnWidth( 0 );
	RefreshItems();
}

void CWorkshopSubscribedPage::RefreshItems()
{
	if ( !m_pList || !workshop )
		return;

	m_pList->DeleteAllItems();

	CUtlVector<PublishedFileId_t> vecItems;
	workshop->GetSubscribedItemsInOrder( vecItems );

	int nLoops = workshop_dev_stress_test.GetInt();
	if ( nLoops <= 0 )
		nLoops = 1;

	int nIndex = 0;
	for ( int loop = 0; loop < nLoops; ++loop )
	{
		for ( int i = 0; i < vecItems.Count(); ++i )
		{
			WorkshopItem_t *pItem = workshop->GetItem( vecItems[i] );
			if ( !pItem || pItem->IsCampaign() )
				continue;

			CWorkshopItemPanel *pPanel = new CWorkshopItemPanel( m_pList, pItem, true, true, false, nIndex++ );
			m_pList->AddItem( NULL, pPanel );
		}
	}
}

void CWorkshopSubscribedPage::OnEvent( KeyValues *pEvent )
{
	if ( !pEvent )
		return;

	const char *pszName = pEvent->GetName();
	if ( !Q_stricmp( pszName, "Workshop_SubscriptionsUpdated" ) ||
		 !Q_stricmp( pszName, "Workshop_PriorityChange" ) )
	{
		RefreshItems();
	}
	else if ( !Q_stricmp( pszName, "Workshop_EnableChange" ) )
	{
		if ( m_pList && pEvent->GetUint64( "id", 0 ) != 0 )
		{
			for ( int i = 0; i < m_pList->GetItemCount(); ++i )
			{
				Panel *pRow = m_pList->GetItemPanel( i );
				if ( pRow )
					pRow->PerformLayout();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// CWorkshopFeaturedPage
//-----------------------------------------------------------------------------
CWorkshopFeaturedPage::CWorkshopFeaturedPage( Panel *parent, const char *name )
	: BaseClass( parent, name )
{
	SetSize( QuickPropScale( 10 ), QuickPropScale( 10 ) );
	LoadControlSettings( "Resource/WorkshopFeaturedPage.res" );
}

//-----------------------------------------------------------------------------
// CWorkshopItemPanel
//-----------------------------------------------------------------------------
CWorkshopItemPanel::CWorkshopItemPanel( Panel *parent, WorkshopItem_t *pItem,
                                        bool bListMode, bool bAllowReorder, bool bEditMode, int nIndex )
	: BaseClass( parent, "WorkshopItemPanel" )
	, m_pItem( pItem )
	, m_nIndex( nIndex )
	, m_bAllowReorder( bAllowReorder )
	, m_bEditMode( bEditMode )
	, m_bListMode( bListMode )
	, m_unItemState( 0 )
	, m_pPreviewImage( NULL )
	, m_pImagePanel( NULL )
	, m_pTitleBacking( NULL )
	, m_pTitleLabel( NULL )
	, m_pLoadingLabel( NULL )
	, m_pDescLabel( NULL )
	, m_pPlayButton( NULL )
	, m_pEditButton( NULL )
	, m_pUpButton( NULL )
	, m_pDownButton( NULL )
	, m_pCheckButton( NULL )
	, m_pNormalBorder( NULL )
	, m_pSelectedBorder( NULL )
	, m_SelectionColor( 255, 255, 255, 255 )
{
	if ( workshop && pItem )
	{
		m_pPreviewImage = workshop->GetItemPreviewImage( pItem );
		if ( m_pPreviewImage )
			m_pPreviewImage->LoadImage( 0 );
	}

	SetSize( QuickPropScale( 624 ), QuickPropScale( 64 ) );

	const char *pszRes = bListMode ? "Resource/WorkshopItemPanel_List.res"
	                               : "Resource/WorkshopItemPanel_Grid.res";
	KeyValues *&pCachedKV = bListMode ? s_pListResKV : s_pGridResKV;
	if ( !pCachedKV )
	{
		pCachedKV = new KeyValues( pszRes );
		pCachedKV->LoadFromFile( g_pFullFileSystem, pszRes, NULL );
	}
	LoadControlSettings( pszRes, NULL, pCachedKV, NULL );
}

CWorkshopItemPanel::~CWorkshopItemPanel()
{
	if ( s_pSelectedPanel == this )
		s_pSelectedPanel = NULL;
}

void CWorkshopItemPanel::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	if ( m_pItem && !m_pItem->IsMountable() )
	{
		SetBorder( pScheme->GetBorder( "WorkshopBrokenBorder" ) );
	}
	else if ( !m_bListMode )
	{
		m_pNormalBorder = GetBorder();
		m_pSelectedBorder = pScheme->GetBorder( "WorkshopSelectedBorder" );
	}

	m_pImagePanel = dynamic_cast<ImagePanel *>( FindChildByName( "WorkshopImagePanel" ) );
	if ( m_pImagePanel )
	{
		m_pImagePanel->SetMouseInputEnabled( false );
		m_pImagePanel->SetImage( m_pPreviewImage );
	}

	m_pTitleLabel = dynamic_cast<Label *>( FindChildByName( "WorkshopItemTitle" ) );
	if ( m_pTitleLabel )
	{
		if ( m_pItem )
		{
			wchar_t wszTitle[k_cchPublishedDocumentTitleMax];
			V_UTF8ToUnicode( m_pItem->m_Details.m_rgchTitle, wszTitle, sizeof( wszTitle ) );
			wszTitle[ARRAYSIZE( wszTitle ) - 1] = 0;
			m_pTitleLabel->SetText( wszTitle );
		}
		m_pTitleLabel->SetMouseInputEnabled( false );
	}

	m_pLoadingLabel = dynamic_cast<Label *>( FindChildByName( "WorkshopItemLoading" ) );
	if ( m_pLoadingLabel )
	{
		m_pLoadingLabel->SetVisible( false );
		m_pLoadingLabel->SetMouseInputEnabled( false );
	}

	m_pDescLabel = dynamic_cast<Label *>( FindChildByName( "WorkshopItemDesc" ) );
	if ( m_pDescLabel )
		m_pDescLabel->SetMouseInputEnabled( false );

	m_pPlayButton  = dynamic_cast<Button *>( FindChildByName( "WorkshopPlayButton" ) );
	m_pEditButton  = dynamic_cast<Button *>( FindChildByName( "WorkshopEditButton" ) );
	m_pCheckButton = dynamic_cast<CheckButton *>( FindChildByName( "WorkshopCheckButton" ) );
	m_pUpButton    = dynamic_cast<Button *>( FindChildByName( "UpButton" ) );
	m_pDownButton  = dynamic_cast<Button *>( FindChildByName( "DownButton" ) );

	m_pTitleBacking = dynamic_cast<ImagePanel *>( FindChildByName( "WorkshopTitleBacking" ) );
	if ( m_pTitleBacking )
		m_pTitleBacking->SetMouseInputEnabled( false );

	m_SelectionColor = pScheme->GetColor( "NewGame.SelectionColor", Color( 255, 255, 255, 255 ) );

	UpdateDownloadState();
}

void CWorkshopItemPanel::PerformLayout()
{
	BaseClass::PerformLayout();

	if ( !m_pItem )
		return;

	if ( m_pDescLabel )
	{
		wchar_t wszDesc[512];
		V_UTF8ToUnicode( m_pItem->m_Details.m_rgchDescription, wszDesc, sizeof( wszDesc ) );
		wszDesc[ARRAYSIZE( wszDesc ) - 1] = 0;
		m_pDescLabel->SetText( wszDesc );
	}

	if ( m_pUpButton && m_pDownButton )
	{
		PanelListPanel *pList = NULL;
		if ( GetParent() && GetParent()->GetParent() )
			pList = dynamic_cast<PanelListPanel *>( GetParent()->GetParent() );

		int nCount = pList ? pList->GetItemCount() : 0;
		if ( nCount == 0 )
		{
			m_pUpButton->SetVisible( false );
			m_pDownButton->SetVisible( false );
		}
		else
		{
			m_pUpButton->SetEnabled( m_nIndex > 0 && nCount > 1 );
			m_pDownButton->SetEnabled( m_nIndex < nCount - 1 && nCount > 1 );
		}

		if ( !m_bAllowReorder || !m_pItem->IsMountable() )
		{
			m_pUpButton->SetEnabled( false );
			m_pDownButton->SetEnabled( false );
			m_pUpButton->SetVisible( false );
			m_pDownButton->SetVisible( false );
		}
	}

	if ( m_bEditMode )
	{
		if ( m_pEditButton )
			m_pEditButton->SetVisible( true );
		if ( m_pPlayButton )
			m_pPlayButton->SetVisible( false );
		if ( m_pCheckButton )
			m_pCheckButton->SetVisible( false );
	}
	else
	{
		if ( m_pEditButton )
			m_pEditButton->SetVisible( false );
		if ( m_pPlayButton )
			m_pPlayButton->SetVisible( m_pItem->IsCampaign() );
		if ( m_pCheckButton )
			m_pCheckButton->SetVisible( !m_pItem->IsCampaign() );
	}

	if ( !m_pItem->IsMountable() )
	{
		if ( m_pPlayButton )
			m_pPlayButton->SetVisible( false );
		if ( m_pCheckButton )
			m_pCheckButton->SetVisible( false );
	}

	if ( m_pCheckButton && workshop )
	{
		m_pCheckButton->SetSelected( workshop->IsItemEnabled( m_pItem->m_Details.m_nPublishedFileId ) );
	}
}

void CWorkshopItemPanel::OnThink()
{
	UpdateDownloadState();
	BaseClass::OnThink();
}

void CWorkshopItemPanel::UpdateDownloadState()
{
	if ( !SteamUGC() || !m_pItem )
		return;

	uint32 state = SteamUGC()->GetItemState( m_pItem->m_Details.m_nPublishedFileId );
	if ( state == m_unItemState )
		return;

	if ( ( state & ( k_EItemStateSubscribed | k_EItemStateNeedsUpdate | k_EItemStateDownloading ) )
	     == ( k_EItemStateSubscribed | k_EItemStateNeedsUpdate ) )
	{
		SteamUGC()->DownloadItem( m_pItem->m_Details.m_nPublishedFileId, false );
	}

	if ( m_pCheckButton )
	{
		bool bCanToggle = ( state & ( k_EItemStateSubscribed | k_EItemStateInstalled ) )
		                  == ( k_EItemStateSubscribed | k_EItemStateInstalled );
		m_pCheckButton->SetEnabled( bCanToggle );
	}

	if ( m_pLoadingLabel && m_pTitleBacking && m_pImagePanel && ( state & k_EItemStateSubscribed ) )
	{
		bool bBusy = ( state & ( k_EItemStateNeedsUpdate | k_EItemStateDownloading ) ) != 0
		             || ( state & k_EItemStateInstalled ) == 0;
		if ( bBusy )
		{
			m_pLoadingLabel->SetText( "#GameUI_Workshop_Downloading" );
			m_pLoadingLabel->SetVisible( true );
			m_pTitleBacking->SetVisible( true );
			m_pImagePanel->SetDrawColor( Color( 100, 100, 100, 255 ) );
			SetEnabled( false );
		}
		else
		{
			m_pLoadingLabel->SetVisible( false );
			m_pTitleBacking->SetVisible( false );
			m_pImagePanel->SetDrawColor( Color( 255, 255, 255, 255 ) );
			SetEnabled( true );
		}
	}

	m_unItemState = state;
}

void CWorkshopItemPanel::OnMousePressed( MouseCode code )
{
	if ( !m_pItem )
		return;

	if ( GetParent() && GetParent()->IsEnabled() && !m_bListMode && m_pItem->IsMountable() )
	{
		if ( s_pSelectedPanel )
			s_pSelectedPanel->SetBorder( s_pSelectedPanel->m_pNormalBorder );
		s_pSelectedPanel = this;
		if ( m_pSelectedBorder )
			SetBorder( m_pSelectedBorder );
	}

	if ( code == MOUSE_RIGHT )
	{
		if ( m_hActionMenu.Get() )
		{
			delete m_hActionMenu.Get();
			m_hActionMenu = NULL;
		}
		m_hActionMenu = new Menu( this, "ActionMenu" );
		m_hActionMenu->SetProportional( false );

		wchar_t wTitle[k_cchPublishedDocumentTitleMax];
		V_UTF8ToUnicode( m_pItem->m_Details.m_rgchTitle, wTitle, sizeof( wTitle ) );
		wTitle[ARRAYSIZE( wTitle ) - 1] = 0;
		int nTitleIdx = m_hActionMenu->AddMenuItem( "Title", wTitle, (KeyValues *)NULL, this );
		m_hActionMenu->SetItemEnabled( nTitleIdx, false );

		if ( !m_pItem->IsMountable() )
		{
			int nBrokenIdx = m_hActionMenu->AddMenuItem( "Broken", L"Broken Item!", (KeyValues *)NULL, this );
			m_hActionMenu->SetItemEnabled( nBrokenIdx, false );
		}

		m_hActionMenu->AddMenuItem( "#GameUI_Workshop_ViewOnWorkshop", new KeyValues( "Command", "command", "addon_online" ), this );
		m_hActionMenu->AddMenuItem( "#GameUI_Workshop_CopyToClipboard", new KeyValues( "Command", "command", "addon_copy" ), this );

		if ( !m_pItem->m_bVotedUp )
			m_hActionMenu->AddMenuItem( "#GameUI_Workshop_VoteUp", new KeyValues( "Command", "command", "addon_voteup" ), this );
		if ( !m_pItem->m_bVotedDown )
			m_hActionMenu->AddMenuItem( "#GameUI_Workshop_VoteDown", new KeyValues( "Command", "command", "addon_votedown" ), this );

		m_hActionMenu->AddMenuItem( "#GameUI_Workshop_Unsubscribe", new KeyValues( "Command", "command", "addon_unsubscribe" ), this );
		Menu::PlaceContextMenu( this, m_hActionMenu.Get() );
	}
}

void CWorkshopItemPanel::OnMouseDoublePressed( MouseCode code )
{
	OnMousePressed( code );
	if ( !m_pItem || !m_pItem->IsMountable() || !workshop )
		return;
	if ( ( m_unItemState & ( k_EItemStateInstalled | k_EItemStateNeedsUpdate ) ) != k_EItemStateInstalled )
		return;
	if ( code == MOUSE_LEFT && GetParent() && GetParent()->IsEnabled() && m_pItem->IsCampaign() )
	{
		workshop->LoadCampaign( m_pItem->m_Details.m_nPublishedFileId, true );
	}
}

void CWorkshopItemPanel::OnCommand( const char *command )
{
	if ( !m_pItem )
	{
		BaseClass::OnCommand( command );
		return;
	}

	if ( !Q_strcmp( command, "addon_enabled" ) )
	{
		if ( workshop && m_pCheckButton && ( m_unItemState & k_EItemStateInstalled ) )
			workshop->SetItemEnabled( m_pItem->m_Details.m_nPublishedFileId, m_pCheckButton->IsSelected() );
		return;
	}
	if ( !Q_strcmp( command, "addon_up" ) || !Q_strcmp( command, "addon_down" ) )
	{
		bool bMoveUp = ( Q_strcmp( command, "addon_up" ) == 0 );
		if ( workshop )
			workshop->MoveItemPriority( m_pItem->m_Details.m_nPublishedFileId, bMoveUp );
		return;
	}
	if ( !Q_strcmp( command, "edit" ) )
	{
		if ( engine && engine->GetEventsSubscription() )
		{
			KeyValues *kv = new KeyValues( "Workshop_EditItem" );
			kv->SetPtr( "item", m_pItem );
			kv->SetUint64( "id", m_pItem->m_Details.m_nPublishedFileId );
			engine->GetEventsSubscription()->BroadcastEvent( kv );
		}
		return;
	}
	if ( !Q_strcmp( command, "addon_online" ) )
	{
		char szURL[2048] = {};
		bool bBeta = SteamUtils() && SteamUtils()->GetConnectedUniverse() == k_EUniverseBeta;
		Q_snprintf( szURL, sizeof( szURL ),
			bBeta ? "http://beta.steamcommunity.com/sharedfiles/filedetails/?id=%llu&requirelogin=true"
			      : "http://steamcommunity.com/sharedfiles/filedetails/?id=%llu&requirelogin=true",
			(unsigned long long)m_pItem->m_Details.m_nPublishedFileId );
		OpenURLInOverlayOrBrowser( szURL );
		return;
	}
	if ( !Q_strcmp( command, "addon_unsubscribe" ) )
	{
		if ( SteamUGC() )
			SteamUGC()->UnsubscribeItem( m_pItem->m_Details.m_nPublishedFileId );
		return;
	}
	if ( !Q_strcmp( command, "addon_voteup" ) )
	{
		if ( workshop )
			workshop->SetItemVote( m_pItem, true );
		return;
	}
	if ( !Q_strcmp( command, "addon_votedown" ) )
	{
		if ( workshop )
			workshop->SetItemVote( m_pItem, false );
		return;
	}
	if ( !Q_strcmp( command, "addon_copy" ) )
	{
		char szURL[2048] = {};
		bool bBeta = SteamUtils() && SteamUtils()->GetConnectedUniverse() == k_EUniverseBeta;
		Q_snprintf( szURL, sizeof( szURL ),
			bBeta ? "http://beta.steamcommunity.com/sharedfiles/filedetails/?id=%llu"
			      : "http://steamcommunity.com/sharedfiles/filedetails/?id=%llu",
			(unsigned long long)m_pItem->m_Details.m_nPublishedFileId );
		system()->SetClipboardText( szURL, Q_strlen( szURL ) );
		return;
	}
	BaseClass::OnCommand( command );
}

//-----------------------------------------------------------------------------
// CWorkshopMyUploadsPage
//-----------------------------------------------------------------------------
CWorkshopMyUploadsPage::CWorkshopMyUploadsPage( Panel *parent, const char *name )
	: BaseClass( parent, name )
	, m_pList( NULL )
	, m_pQuery( NULL )
{
	SetSize( QuickPropScale( 10 ), QuickPropScale( 10 ) );
	LoadControlSettings( "Resource/WorkshopMyUploadsPage.res" );
	if ( engine && engine->GetEventsSubscription() )
		engine->GetEventsSubscription()->Subscribe( this );
}

CWorkshopMyUploadsPage::~CWorkshopMyUploadsPage()
{
	if ( engine && engine->GetEventsSubscription() )
		engine->GetEventsSubscription()->Unsubscribe( this );
	delete m_pQuery;
	m_pQuery = NULL;
}

void CWorkshopMyUploadsPage::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	m_pList = dynamic_cast<PanelListPanel *>( FindChildByName( "ListyList" ) );
	if ( m_pList )
		m_pList->SetFirstColumnWidth( 0 );
	if ( !m_pQuery )
		RefreshItems();
}

void CWorkshopMyUploadsPage::RefreshItems()
{
	delete m_pQuery;
	m_pQuery = NULL;
	if ( !m_pList )
		return;
	m_pList->DeleteAllItems();
	m_pQuery = new CSteamUGCUserItemsQuery( this );
	m_pQuery->RequestNextPage();
}

void CWorkshopMyUploadsPage::OnEvent( KeyValues *pEvent )
{
	if ( !pEvent )
		return;
	const char *pszName = pEvent->GetName();
	if ( !Q_stricmp( pszName, "Workshop_CreatedItem" ) )
		RefreshItems();
}

//-----------------------------------------------------------------------------
// CWorkshopUploaderPage
//-----------------------------------------------------------------------------
CWorkshopUploaderPage::CWorkshopUploaderPage( Panel *parent, const char *name, WorkshopItem_t *pEditItem )
	: BaseClass( parent, name )
	, m_pInitialEditItem( pEditItem )
	, m_pPreviewImagePanel( NULL )
	, m_pLocalPreviewBitmap( NULL )
	, m_pTitleEntry( NULL )
	, m_pDescEntry( NULL )
	, m_pStartingMapEntry( NULL )
	, m_pStartingMapLabel( NULL )
	, m_pExpansionLabel( NULL )
	, m_pExpansionCombo( NULL )
	, m_pContentEntry( NULL )
	, m_pAddonTypeCombo( NULL )
	, m_pVisibilityCombo( NULL )
	, m_pUploadButton( NULL )
	, m_pProgressBar( NULL )
	, m_bUploading( false )
	, m_bTitleDirty( false )
	, m_bDescDirty( false )
	, m_bStartingMapDirty( false )
	, m_nPublishedFileId( 0 )
	, m_hUpdateHandle( k_UGCUpdateHandleInvalid )
{
	V_memset( m_ulTags, 0, sizeof( m_ulTags ) );
	if ( m_pInitialEditItem && m_pInitialEditItem->m_Details.m_nPublishedFileId == 0 )
		m_pInitialEditItem = NULL;

	SetSize( QuickPropScale( 10 ), QuickPropScale( 10 ) );
	LoadControlSettings( "Resource/WorkshopUploaderPage.res" );
	if ( engine && engine->GetEventsSubscription() )
		engine->GetEventsSubscription()->Subscribe( this );
	ivgui()->AddTickSignal( GetVPanel(), 0 );
}

CWorkshopUploaderPage::~CWorkshopUploaderPage()
{
	ClearPreviewImage();
	if ( engine && engine->GetEventsSubscription() )
		engine->GetEventsSubscription()->Unsubscribe( this );
	m_CallResultCreateItem.Cancel();
	m_CallResultSubmitUpdate.Cancel();
}

void CWorkshopUploaderPage::ClearPreviewImage()
{
	if ( m_pPreviewImagePanel )
		m_pPreviewImagePanel->SetImage( (IImage *)NULL );
	delete m_pLocalPreviewBitmap;
	m_pLocalPreviewBitmap = NULL;
}

int CWorkshopUploaderPage::GetSelectedAddonType()
{
	if ( m_pAddonTypeCombo && m_pAddonTypeCombo->GetActiveItem() == 0 )
		return 1; // Campaign
	return 2; // Content
}

void CWorkshopUploaderPage::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_pPreviewImagePanel = dynamic_cast<ImagePanel *>( FindChildByName( "PreviewImage" ) );
	if ( m_pPreviewImagePanel )
		m_pPreviewImagePanel->SetShouldScaleImage( true );

	m_pTitleEntry = dynamic_cast<TextEntry *>( FindChildByName( "Title" ) );
	if ( m_pTitleEntry )
		m_pTitleEntry->AddActionSignalTarget( this );

	m_pDescEntry = dynamic_cast<TextEntry *>( FindChildByName( "Description" ) );
	if ( m_pDescEntry )
	{
		m_pDescEntry->AddActionSignalTarget( this );
		m_pDescEntry->SetMultiline( true );
		m_pDescEntry->SetVerticalScrollbar( true );
		m_pDescEntry->SetCatchEnterKey( true );
		m_pDescEntry->SendNewLine( false );
	}

	m_pStartingMapEntry = dynamic_cast<TextEntry *>( FindChildByName( "StartingMap" ) );
	if ( m_pStartingMapEntry )
		m_pStartingMapEntry->AddActionSignalTarget( this );

	m_pStartingMapLabel = dynamic_cast<Label *>( FindChildByName( "StartingMapLabel" ) );
	m_pExpansionLabel = dynamic_cast<Label *>( FindChildByName( "ExpansionLabel" ) );

	m_pExpansionCombo = dynamic_cast<ComboBox *>( FindChildByName( "ExpansionButton" ) );
	if ( m_pExpansionCombo && m_pExpansionCombo->GetItemCount() == 0 )
	{
		m_pExpansionCombo->AddItem( "#GameUI_Workshop_Expansion_HL2", NULL );
		m_pExpansionCombo->AddItem( "#GameUI_Workshop_Expansion_Episode1", NULL );
		m_pExpansionCombo->AddItem( "#GameUI_Workshop_Expansion_Episode2", NULL );
		m_pExpansionCombo->ActivateItem( 2 );
	}

	m_pContentEntry = dynamic_cast<TextEntry *>( FindChildByName( "Content" ) );
	if ( m_pContentEntry )
		m_pContentEntry->AddActionSignalTarget( this );

	m_pAddonTypeCombo = dynamic_cast<ComboBox *>( FindChildByName( "AddonType" ) );
	if ( m_pAddonTypeCombo && m_pAddonTypeCombo->GetItemCount() == 0 )
	{
		m_pAddonTypeCombo->AddItem( "#GameUI_Workshop_Upload_Campaign", NULL );
		m_pAddonTypeCombo->AddItem( "#GameUI_Workshop_Upload_Content", NULL );
		m_pAddonTypeCombo->ActivateItem( 0 );
		m_pAddonTypeCombo->AddActionSignalTarget( this );
	}

	m_pVisibilityCombo = dynamic_cast<ComboBox *>( FindChildByName( "Visibility" ) );
	if ( m_pVisibilityCombo && m_pVisibilityCombo->GetItemCount() == 0 )
	{
		m_pVisibilityCombo->AddItem( "#GameUI_Workshop_Upload_Visibility_Public", NULL );
		m_pVisibilityCombo->AddItem( "#GameUI_Workshop_Upload_Visibility_FriendsOnly", NULL );
		m_pVisibilityCombo->AddItem( "#GameUI_Workshop_Upload_Visibility_Private", NULL );
		m_pVisibilityCombo->AddItem( "#GameUI_Workshop_Upload_Visibility_Unlisted", NULL );
		m_pVisibilityCombo->AddItem( "#GameUI_Workshop_Upload_Visibility_Unknown", NULL );
		m_pVisibilityCombo->ActivateItem( 2 );
	}

	m_pUploadButton = dynamic_cast<Button *>( FindChildByName( "UploadButton" ) );
	m_pProgressBar = dynamic_cast<ProgressBar *>( FindChildByName( "ProgressBar" ) );

	if ( m_pInitialEditItem )
	{
		m_nPublishedFileId = m_pInitialEditItem->m_Details.m_nPublishedFileId;
		m_szTitle = m_pInitialEditItem->m_Details.m_rgchTitle;
		m_szDescription = m_pInitialEditItem->m_Details.m_rgchDescription;
		m_szStartingMap = m_pInitialEditItem->m_pCampaignData ? m_pInitialEditItem->m_pCampaignData->GetString( "starting_map", "" ) : "";

		if ( m_pTitleEntry ) m_pTitleEntry->SetText( m_szTitle.Get() );
		if ( m_pDescEntry ) m_pDescEntry->SetText( m_szDescription.Get() );
		if ( m_pStartingMapEntry ) m_pStartingMapEntry->SetText( m_szStartingMap.Get() );

		if ( m_pAddonTypeCombo )
			m_pAddonTypeCombo->ActivateItem( m_pInitialEditItem->IsCampaign() ? 0 : 1 );
		if ( m_pVisibilityCombo )
			m_pVisibilityCombo->ActivateItem( clamp( (int)m_pInitialEditItem->m_Details.m_eVisibility, 0, 4 ) );
		if ( m_pExpansionCombo )
		{
			const char *pszExp = m_pInitialEditItem->m_pCampaignData ? m_pInitialEditItem->m_pCampaignData->GetString( "expansion", "ep2" ) : "ep2";
			if ( !V_stricmp( pszExp, "hl2" ) )
				m_pExpansionCombo->ActivateItem( 0 );
			else if ( !V_stricmp( pszExp, "episodic" ) || !V_stricmp( pszExp, "ep1" ) )
				m_pExpansionCombo->ActivateItem( 1 );
			else
				m_pExpansionCombo->ActivateItem( 2 );
		}
		if ( m_pPreviewImagePanel && workshop )
			m_pPreviewImagePanel->SetImage( workshop->GetItemPreviewImage( m_pInitialEditItem ) );

		m_bTitleDirty = false;
		m_bDescDirty = false;
		m_bStartingMapDirty = false;
		V_memcpy( m_ulTags, m_pInitialEditItem->m_ulTags, sizeof( m_ulTags ) );
		m_pInitialEditItem = NULL;
	}

	UpdateButtonStates();
}

void CWorkshopUploaderPage::OnTextChanged( Panel *panel )
{
	if ( panel == m_pTitleEntry ) m_bTitleDirty = true;
	if ( panel == m_pDescEntry ) m_bDescDirty = true;
	if ( panel == m_pStartingMapEntry ) m_bStartingMapDirty = true;
	UpdateButtonStates();
}

void CWorkshopUploaderPage::UpdateButtonStates()
{
	bool bCampaign = ( m_pAddonTypeCombo && m_pAddonTypeCombo->GetActiveItem() == 0 );
	bool bCanUpload = !m_bUploading;

	if ( !m_pTitleEntry || m_pTitleEntry->GetTextLength() <= 0 )
		bCanUpload = false;
	if ( !m_pDescEntry || m_pDescEntry->GetTextLength() <= 0 )
		bCanUpload = false;

	if ( bCampaign )
	{
		if ( !m_pStartingMapEntry || m_pStartingMapEntry->GetTextLength() <= 0 )
		{
			bCanUpload = false;
		}
		else
		{
			char szMap[256] = {};
			m_pStartingMapEntry->GetText( szMap, sizeof( szMap ) );
			for ( size_t i = 0; szMap[i]; ++i )
			{
				if ( !IsValidWorkshopMapNameChar( szMap[i] ) )
				{
					bCanUpload = false;
					break;
				}
			}
		}
	}

	if ( m_nPublishedFileId == 0 )
	{
		if ( m_szPreviewImagePath.IsEmpty() || !m_pContentEntry || m_pContentEntry->GetTextLength() <= 0 )
			bCanUpload = false;
	}

	if ( m_pContentEntry && m_pContentEntry->GetTextLength() > 0 )
	{
		char szDir[1024] = {};
		m_pContentEntry->GetText( szDir, sizeof( szDir ) );
		m_szContentFolder = szDir;
		if ( !g_pFullFileSystem || !g_pFullFileSystem->IsDirectory( m_szContentFolder.Get(), NULL ) )
			bCanUpload = false;
	}
	else
	{
		m_szContentFolder.Clear();
	}

	if ( m_pUploadButton )
	{
		m_pUploadButton->SetVisible( !m_bUploading );
		m_pUploadButton->SetEnabled( bCanUpload );
		m_pUploadButton->SetText( m_nPublishedFileId == 0 ? "#GameUI_Workshop_Upload_Upload" : "#GameUI_Workshop_Upload_Update" );
	}
	if ( m_pProgressBar )
		m_pProgressBar->SetVisible( m_bUploading );
	if ( m_pStartingMapEntry )
		m_pStartingMapEntry->SetVisible( bCampaign );
	if ( m_pStartingMapLabel )
		m_pStartingMapLabel->SetVisible( bCampaign );
	if ( m_pExpansionCombo )
		m_pExpansionCombo->SetVisible( bCampaign );
	if ( m_pExpansionLabel )
		m_pExpansionLabel->SetVisible( bCampaign );
	if ( m_pAddonTypeCombo )
		m_pAddonTypeCombo->SetEnabled( m_nPublishedFileId == 0 );
}

void CWorkshopUploaderPage::UpdateTabTitle()
{
	PropertySheet *pSheet = dynamic_cast<PropertySheet *>( GetParent() );
	if ( !pSheet )
		return;
	int idx = pSheet->FindPage( this );
	if ( idx >= 0 && !m_szTitle.IsEmpty() )
		pSheet->SetTabTitle( idx, m_szTitle.Get() );
}

void CWorkshopUploaderPage::BringTabToFront()
{
	PropertySheet *pSheet = dynamic_cast<PropertySheet *>( GetParent() );
	if ( pSheet )
		pSheet->SetActivePage( this );
}

void CWorkshopUploaderPage::OnCommand( const char *command )
{
	if ( !Q_strcmp( command, "BrowseImage" ) )
	{
#if defined( OSX )
		char szPath[1024] = {};
		if ( RunOsascriptDialog( "osascript -e 'POSIX path of (choose file of type {\"public.png\", \"public.jpeg\"})' 2>/dev/null", szPath, sizeof( szPath ) ) )
		{
			CUtlBuffer buf;
			if ( g_pFullFileSystem && g_pFullFileSystem->ReadFile( szPath, NULL, buf ) && buf.TellMaxPut() > 8 )
			{
				const uint8 *pBytes = (const uint8 *)buf.Base();
				static const uint8 s_PNGMagic[8] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
				ClearPreviewImage();
				if ( memcmp( pBytes, s_PNGMagic, 8 ) == 0 )
				{
					Bitmap_t bmp;
					if ( ImgUtl_LoadPNGBitmapFromBuffer( buf, bmp ) == CE_SUCCESS )
					{
						m_pLocalPreviewBitmap = new CWorkshopLocalBitmapImage();
						m_pLocalPreviewBitmap->SetRGBA( bmp.GetBits(), bmp.Width(), bmp.Height() );
						if ( m_pPreviewImagePanel )
							m_pPreviewImagePanel->SetImage( m_pLocalPreviewBitmap );
						m_szPreviewImagePath = szPath;
						Msg( "[CWorkshopUploaderPage] Loaded image: %s\n", szPath );
					}
				}
				else if ( pBytes[0] == 0xFF && pBytes[1] == 0xD8 )
				{
					CUtlBuffer rgba;
					int w = 0, h = 0;
					if ( ImgUtl_ReadJPEGAsRGBAFromBuffer( buf, rgba, w, h ) == CE_SUCCESS && w > 0 && h > 0 )
					{
						m_pLocalPreviewBitmap = new CWorkshopLocalBitmapImage();
						m_pLocalPreviewBitmap->SetRGBA( (const unsigned char *)rgba.Base(), w, h );
						if ( m_pPreviewImagePanel )
							m_pPreviewImagePanel->SetImage( m_pLocalPreviewBitmap );
						m_szPreviewImagePath = szPath;
						Msg( "[CWorkshopUploaderPage] Loaded image: %s\n", szPath );
					}
				}
				UpdateButtonStates();
			}
		}
#endif
		return;
	}
	if ( !Q_strcmp( command, "BrowseContentFolder" ) )
	{
#if defined( OSX )
		char szDir[1024] = {};
		if ( RunOsascriptDialog( "osascript -e 'POSIX path of (choose folder)' 2>/dev/null", szDir, sizeof( szDir ) ) )
		{
			if ( m_pContentEntry )
				m_pContentEntry->SetText( szDir );
			UpdateButtonStates();
			Msg( "[CWorkshopUploaderPage] Selecting content dir %s\n", szDir );
		}
#endif
		return;
	}
	if ( !Q_strcmp( command, "DoUpload" ) )
	{
		StartUpload();
		return;
	}
	BaseClass::OnCommand( command );
}

void CWorkshopUploaderPage::StartUpload()
{
	if ( !SteamUGC() || !workshop )
	{
		OnUploadFailed( "Steam is not running." );
		return;
	}

	m_bUploading = true;
	if ( m_nPublishedFileId == 0 )
	{
		if ( m_pProgressBar ) m_pProgressBar->SetProgress( 0.0f );
		UpdateButtonStates();
		SteamAPICall_t hCall = SteamUGC()->CreateItem( workshop->GetAppID(), k_EWorkshopFileTypeCommunity );
		if ( hCall == k_uAPICallInvalid )
		{
			OnUploadFailed( "Failed to create new workshop item." );
			return;
		}
		m_CallResultCreateItem.Set( hCall, this, &CWorkshopUploaderPage::OnCreateItem );
	}
	else
	{
		if ( m_pProgressBar ) m_pProgressBar->SetProgress( 0.125f );
		UpdateButtonStates();
		UpdateUploadParams();
	}
}

void CWorkshopUploaderPage::OnCreateItem( CreateItemResult_t *pResult, bool bIOFailure )
{
	if ( bIOFailure || !pResult || pResult->m_eResult != k_EResultOK )
	{
		OnUploadFailed( "Failed to create new workshop item." );
		return;
	}
	m_nPublishedFileId = pResult->m_nPublishedFileId;
	UpdateButtonStates();
	UpdateUploadParams();
}

void CWorkshopUploaderPage::UpdateUploadParams()
{
	if ( !SteamUGC() || !workshop )
	{
		OnUploadFailed( "Failed to start item update." );
		return;
	}

	m_hUpdateHandle = SteamUGC()->StartItemUpdate( workshop->GetAppID(), m_nPublishedFileId );
	if ( m_hUpdateHandle == k_UGCUpdateHandleInvalid )
	{
		OnUploadFailed( "Failed to start item update." );
		return;
	}

	if ( m_pProgressBar )
		m_pProgressBar->SetProgress( 0.25f );

	bool bOK = true;
	if ( m_pTitleEntry )
	{
		char szBuf[256] = {};
		m_pTitleEntry->GetText( szBuf, sizeof( szBuf ) );
		m_szTitle = szBuf;
		if ( m_bTitleDirty || m_nPublishedFileId == 0 )
			bOK = bOK && SteamUGC()->SetItemTitle( m_hUpdateHandle, m_szTitle.Get() );
	}
	if ( m_pDescEntry )
	{
		char szBuf[8000] = {};
		m_pDescEntry->GetText( szBuf, sizeof( szBuf ) );
		m_szDescription = szBuf;
		if ( m_bDescDirty || m_nPublishedFileId == 0 )
			bOK = bOK && SteamUGC()->SetItemDescription( m_hUpdateHandle, m_szDescription.Get() );
	}
	if ( !m_szPreviewImagePath.IsEmpty() )
	{
		bOK = bOK && SteamUGC()->SetItemPreview( m_hUpdateHandle, m_szPreviewImagePath.Get() );
	}
	if ( !m_szContentFolder.IsEmpty() )
	{
		bOK = bOK && SteamUGC()->SetItemContent( m_hUpdateHandle, m_szContentFolder.Get() );
		if ( SteamApps() )
		{
			char szBuildId[32];
			V_snprintf( szBuildId, sizeof( szBuildId ), "%d", SteamApps()->GetAppBuildId() );
			SteamUGC()->RemoveItemKeyValueTags( m_hUpdateHandle, "buildid" );
			SteamUGC()->AddItemKeyValueTag( m_hUpdateHandle, "buildid", szBuildId );
		}
	}

	bool bCampaign = ( GetSelectedAddonType() == 1 );
	m_ulTags[WORKSHOP_TAG_CATEGORY_TYPE] = bCampaign ? ( 1ULL << 0 ) : ( 1ULL << 1 );
	if ( bCampaign )
	{
		if ( m_pStartingMapEntry )
		{
			char szMap[256] = {};
			m_pStartingMapEntry->GetText( szMap, sizeof( szMap ) );
			m_szStartingMap = szMap;
			SteamUGC()->RemoveItemKeyValueTags( m_hUpdateHandle, "starting_map" );
			bOK = bOK && SteamUGC()->AddItemKeyValueTag( m_hUpdateHandle, "starting_map", m_szStartingMap.Get() );
		}
		const char *pszExp = "ep2";
		if ( m_pExpansionCombo )
		{
			int sel = m_pExpansionCombo->GetActiveItem();
			if ( sel == 0 ) pszExp = "hl2";
			else if ( sel == 1 ) pszExp = "episodic";
		}
		SteamUGC()->RemoveItemKeyValueTags( m_hUpdateHandle, "expansion" );
		bOK = bOK && SteamUGC()->AddItemKeyValueTag( m_hUpdateHandle, "expansion", pszExp );
	}
	else
	{
		SteamUGC()->RemoveItemKeyValueTags( m_hUpdateHandle, "starting_map" );
		SteamUGC()->RemoveItemKeyValueTags( m_hUpdateHandle, "expansion" );
	}

	CUtlVector<const char *> vecTagNames;
	for ( int iCat = 0; iCat < WORKSHOP_TAG_CATEGORY_COUNT; ++iCat )
	{
		const WorkshopTag_t *pTable = workshop->GetTagList( (WorkshopTagCategory_t)iCat );
		for ( int i = 0; pTable && pTable[i].m_pszTag; ++i )
		{
			if ( m_ulTags[iCat] & pTable[i].m_ulMask )
				vecTagNames.AddToTail( pTable[i].m_pszTag );
		}
	}
	if ( vecTagNames.Count() > 0 )
	{
		SteamParamStringArray_t arr;
		arr.m_ppStrings = vecTagNames.Base();
		arr.m_nNumStrings = vecTagNames.Count();
		bOK = bOK && SteamUGC()->SetItemTags( m_hUpdateHandle, &arr, false );
	}

	if ( !bOK )
	{
		OnUploadFailed( "Failed to update item paramaters." );
		return;
	}

	SteamAPICall_t hCall = SteamUGC()->SubmitItemUpdate( m_hUpdateHandle, NULL );
	if ( hCall == k_uAPICallInvalid )
	{
		OnUploadFailed( "Failed to submit item update." );
		return;
	}
	m_CallResultSubmitUpdate.Set( hCall, this, &CWorkshopUploaderPage::OnSubmitItemUpdate );
}

void CWorkshopUploaderPage::OnSubmitItemUpdate( SubmitItemUpdateResult_t *pResult, bool bIOFailure )
{
	if ( bIOFailure || !pResult || pResult->m_eResult != k_EResultOK )
	{
		OnUploadFailed( "Failed to submit item update." );
		return;
	}
	OnUploadSucceeded( pResult );
}

void CWorkshopUploaderPage::OnUploadSucceeded( SubmitItemUpdateResult_t *pResult )
{
	UpdateTabTitle();
	m_hUpdateHandle = k_UGCUpdateHandleInvalid;
	m_bUploading = false;
	m_bTitleDirty = false;
	m_bDescDirty = false;
	m_bStartingMapDirty = false;
	V_memset( m_ulTags, 0, sizeof( m_ulTags ) );
	m_szPreviewImagePath.Clear();
	m_szContentFolder.Clear();
	if ( m_pContentEntry )
		m_pContentEntry->SetText( "" );
	UpdateButtonStates();

	char szURL[2048] = {};
	bool bBeta = SteamUtils() && SteamUtils()->GetConnectedUniverse() == k_EUniverseBeta;
	Q_snprintf( szURL, sizeof( szURL ),
		bBeta ? "http://beta.steamcommunity.com/sharedfiles/filedetails/?id=%llu&requirelogin=true"
		      : "http://steamcommunity.com/sharedfiles/filedetails/?id=%llu&requirelogin=true",
		(unsigned long long)pResult->m_nPublishedFileId );
	Msg( "[CWorkshopUploaderPage] Uploaded file! %s\n", szURL );
	OpenURLInOverlayOrBrowser( szURL );

	if ( engine && engine->GetEventsSubscription() )
	{
		KeyValues *pEvent = new KeyValues( "Workshop_CreatedItem" );
		engine->GetEventsSubscription()->BroadcastEvent( pEvent );
	}

	if ( workshop )
	{
		WorkshopItem_t *pSub = workshop->GetItem( m_nPublishedFileId );
		if ( pSub )
		{
			workshop->DownloadItem( pSub->m_Details.m_nPublishedFileId );
			workshop->RefreshSubscribedItems();
		}
	}
}

void CWorkshopUploaderPage::OnUploadFailed( const char *pszReason )
{
	m_bUploading = false;
	m_hUpdateHandle = k_UGCUpdateHandleInvalid;
	UpdateButtonStates();

	MessageBox *pBox = new MessageBox( "Failed to upload to workshop", pszReason, BasePanel() );
	pBox->DoModal();
	BringTabToFront();
}

void CWorkshopUploaderPage::OnTick()
{
	if ( m_hUpdateHandle != k_UGCUpdateHandleInvalid && SteamUGC() && m_pProgressBar )
	{
		uint64 processed = 0, total = 0;
		EItemUpdateStatus status = SteamUGC()->GetItemUpdateProgress( m_hUpdateHandle, &processed, &total );
		float frac = ( total > 0 ) ? clamp( (float)( (double)processed / (double)total ), 0.0f, 1.0f ) : 0.0f;
		switch ( status )
		{
		case k_EItemUpdateStatusPreparingConfig:      m_pProgressBar->SetProgress( 0.28f ); break;
		case k_EItemUpdateStatusPreparingContent:     m_pProgressBar->SetProgress( 0.35f ); break;
		case k_EItemUpdateStatusUploadingContent:     m_pProgressBar->SetProgress( 0.35f + frac * 0.50f ); break;
		case k_EItemUpdateStatusUploadingPreviewFile: m_pProgressBar->SetProgress( 0.85f + frac * 0.05f ); break;
		case k_EItemUpdateStatusCommittingChanges:    m_pProgressBar->SetProgress( 0.95f ); break;
		default: break;
		}
	}
}
