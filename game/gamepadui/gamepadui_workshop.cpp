//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI Steam Workshop Panel & Workshop Item Buttons
//          (Campaigns horizontal card view + Subscribed addons list view).
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#include "gamepadui_workshop.h"
#include "gamepadui_interface.h"
#include "gamepadui_basepanel.h"

#include <stdlib.h>

#include "cdll_int.h"
#include "tier1/convar.h"
#include "tier1/strtools.h"
#include "tier3/tier3.h"
#include "vgui/ILocalize.h"
#include "vgui/IPanel.h"
#include "vgui/IScheme.h"
#include "vgui/ISurface.h"
#include "vgui/IVGui.h"
#include "vgui_controls/Controls.h"

#if !defined( NO_STEAM )
#include "steam/steam_api.h"
#include "steam/isteamfriends.h"
#include "steam/isteamugc.h"
#include "steam/isteamutils.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Static state & retail ConVar
//-----------------------------------------------------------------------------
GamepadUIWorkshopButton_Square *GamepadUIWorkshopButton_Square::s_pLastSelectedButton = NULL;
GamepadUIWorkshopPanel *GamepadUIWorkshopPanel::s_pWorkshopPanel = NULL;

static ConVar gamepadui_last_workshop_tab( "gamepadui_last_workshop_tab", "0", FCVAR_ARCHIVE );

//-----------------------------------------------------------------------------
// GamepadUIWorkshopButton
//-----------------------------------------------------------------------------
GamepadUIWorkshopButton::GamepadUIWorkshopButton(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const char *pText,
	const char *pDescription,
	WorkshopItem_t *pItem )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription )
	, m_pItem( pItem )
{
}

GamepadUIWorkshopButton::~GamepadUIWorkshopButton()
{
}

//-----------------------------------------------------------------------------
// GamepadUIWorkshopButton_Square
//-----------------------------------------------------------------------------
GamepadUIWorkshopButton_Square::GamepadUIWorkshopButton_Square(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const char *pText,
	const char *pDescription,
	WorkshopItem_t *pItem )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription, pItem )
{
	IWorkshop *pWorkshop = GamepadUI::GetInstance().GetWorkshop();
	if ( pWorkshop && m_pItem )
	{
		IWorkshopImage *pPreview = pWorkshop->GetItemPreviewImage( m_pItem );
		if ( pPreview )
		{
			pPreview->LoadImage( 0 );
			m_Image.SetImage( pPreview );
		}
	}
}

GamepadUIWorkshopButton_Square::~GamepadUIWorkshopButton_Square()
{
	if ( s_pLastSelectedButton == this )
	{
		s_pLastSelectedButton = NULL;
	}
}

ButtonState::State GamepadUIWorkshopButton_Square::GetCurrentButtonState()
{
	if ( s_pLastSelectedButton == this )
		return ButtonState::Over;
	return BaseClass::GetCurrentButtonState();
}

void GamepadUIWorkshopButton_Square::NavigateTo()
{
	BaseClass::NavigateTo();
	s_pLastSelectedButton = this;
}

void GamepadUIWorkshopButton_Square::Paint()
{
	int x = 0, y = 0, w = 0, h = 0;
	GetBounds( x, y, w, h );

	PaintButton();

	float flImageH = (float)w + m_flTextOffsetY - m_flTextOffsetYState[ButtonState::Out];
	int nImageH = (int)flImageH;
	float flV1 = ( w > 0 ) ? ( flImageH / (float)w ) : 1.0f;

	int nTexId = m_Image.GetTextureId();
	if ( nTexId > 0 )
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, 255 );
		vgui::surface()->DrawSetTexture( nTexId );
		vgui::surface()->DrawTexturedSubRect( 0, 0, w, nImageH, 0.0f, 0.0f, 1.0f, flV1 );
		vgui::surface()->DrawSetTexture( 0 );
	}

	if ( !IsEnabled() )
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, 16 );
		vgui::surface()->DrawFilledRect( 0, 0, w, nImageH );
	}

	PaintText();
}

//-----------------------------------------------------------------------------
// GamepadUIWorkshopButton_List
//-----------------------------------------------------------------------------
const GamepadUIString &GamepadUIWorkshopButton_List::GetToggleStateText( bool bEnabled )
{
	static GamepadUIString s_States[2] =
	{
		GamepadUIString( "#GameUI_Disabled" ),
		GamepadUIString( "#GameUI_Enabled" ),
	};
	return s_States[bEnabled ? 1 : 0];
}

GamepadUIWorkshopButton_List::GamepadUIWorkshopButton_List(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const char *pText,
	const char *pDescription,
	WorkshopItem_t *pItem )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription, pItem )
	, m_bEnabled( false )
	, m_flImageOffsetXAnimationDuration( 0.2f )
{
	m_flImageOffsetX = 0.0f;
	for ( int i = 0; i < ButtonState::Count; ++i )
	{
		m_flImageOffsetXs[i] = 0.0f;
	}

	SCHEME_MEMBER_PROPORTIONAL( GamepadUIWorkshopButton_List, m_flImageOffsetX, "Image.OffsetX.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIWorkshopButton_List, m_flImageOffsetXs[ButtonState::Out], "Image.OffsetX.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIWorkshopButton_List, m_flImageOffsetXs[ButtonState::Over], "Image.OffsetX.Over", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIWorkshopButton_List, m_flImageOffsetXs[ButtonState::Pressed], "Image.OffsetX.Pressed", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIWorkshopButton_List, m_flImageOffsetXAnimationDuration, "Image.OffsetX.Animation.Duration", "0.2" );

	IWorkshop *pWorkshop = GamepadUI::GetInstance().GetWorkshop();
	if ( pWorkshop && m_pItem )
	{
		m_bEnabled = pWorkshop->IsItemEnabled( m_pItem->m_Details.m_nPublishedFileId );
		IWorkshopImage *pPreview = pWorkshop->GetItemPreviewImage( m_pItem );
		if ( pPreview )
		{
			pPreview->LoadImage( 0 );
			m_Image.SetImage( pPreview );
		}
	}
}

GamepadUIWorkshopButton_List::~GamepadUIWorkshopButton_List()
{
}

void GamepadUIWorkshopButton_List::OnKeyCodePressed( vgui::KeyCode code )
{
	IWorkshop *pWorkshop = GamepadUI::GetInstance().GetWorkshop();
	ButtonCode_t baseCode = GamepadUI_NormalizeJoystickCode( code );

	switch ( baseCode )
	{
	case KEY_LEFT:
	case KEY_RIGHT:
	case KEY_XBUTTON_LEFT:
	case KEY_XBUTTON_RIGHT:
	case KEY_XSTICK1_LEFT:
	case KEY_XSTICK1_RIGHT:
	case STEAMCONTROLLER_DPAD_LEFT:
	case STEAMCONTROLLER_DPAD_RIGHT:
	case STEAMCONTROLLER_LEFT_PAD_LEFT:
	case STEAMCONTROLLER_LEFT_PAD_RIGHT:
		m_bEnabled = !m_bEnabled;
		if ( pWorkshop && m_pItem )
		{
			pWorkshop->SetItemEnabled( m_pItem->m_Details.m_nPublishedFileId, m_bEnabled );
		}
		return;

	case KEY_UP:
	case KEY_XBUTTON_UP:
	case KEY_XSTICK1_UP:
	case STEAMCONTROLLER_DPAD_UP:
	case STEAMCONTROLLER_LEFT_PAD_UP:
		if ( GamepadUIWorkshopPanel::s_pWorkshopPanel && GamepadUIWorkshopPanel::s_pWorkshopPanel->IsMovingAddon() )
		{
			if ( pWorkshop && m_pItem )
			{
				pWorkshop->MoveItemPriority( m_pItem->m_Details.m_nPublishedFileId, true );
			}
			return;
		}
		break;

	case KEY_DOWN:
	case KEY_XBUTTON_DOWN:
	case KEY_XSTICK1_DOWN:
	case STEAMCONTROLLER_DPAD_DOWN:
	case STEAMCONTROLLER_LEFT_PAD_DOWN:
		if ( GamepadUIWorkshopPanel::s_pWorkshopPanel && GamepadUIWorkshopPanel::s_pWorkshopPanel->IsMovingAddon() )
		{
			if ( pWorkshop && m_pItem )
			{
				pWorkshop->MoveItemPriority( m_pItem->m_Details.m_nPublishedFileId, false );
			}
			return;
		}
		break;

	default:
		break;
	}

	BaseClass::OnKeyCodePressed( code );
}

void GamepadUIWorkshopButton_List::Paint()
{
	BaseClass::Paint();

	const GamepadUIString &strState = GetToggleStateText( m_bEnabled );
	ButtonState::State eState = GetCurrentButtonState();

	int nTextW = 0, nTextH = 0;
	vgui::surface()->GetTextSize( m_hTextFont, strState.String(), nTextW, nTextH );

	int nChevronW = vgui::surface()->GetCharacterWidth( m_hTextFont, L'<' )
		+ vgui::surface()->GetCharacterWidth( m_hTextFont, L' ' );

	int nY = (int)( m_flTextOffsetY - (float)( nTextH / 2 ) + m_flHeight * 0.5f );
	vgui::surface()->DrawSetTextColor( m_colTextColor );
	vgui::surface()->DrawSetTextFont( m_hTextFont );

	if ( eState != ButtonState::Out )
	{
		int nLeftX = (int)( m_flWidth - (float)nTextW - ( m_flTextOffsetX + (float)( nChevronW * 2 ) ) );
		vgui::surface()->DrawSetTextPos( nLeftX, nY );
		vgui::surface()->DrawPrintText( L"< ", 2 );
	}

	int nValX = (int)( m_flWidth - (float)nTextW - ( m_flTextOffsetX + (float)nChevronW ) );
	vgui::surface()->DrawSetTextPos( nValX, nY );
	vgui::surface()->DrawPrintText( strState.String(), strState.Length() );

	if ( eState != ButtonState::Out )
	{
		int nRightX = (int)( m_flWidth - m_flTextOffsetX - (float)nChevronW );
		vgui::surface()->DrawSetTextPos( nRightX, nY );
		vgui::surface()->DrawPrintText( L" >", 2 );
	}

	// Draw square preview thumbnail on the left side of the list row
	int nTexId = m_Image.GetTextureId();
	if ( nTexId > 0 )
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, 255 );
		vgui::surface()->DrawSetTexture( nTexId );
		int nTall = GetTall();
		int nImgX0 = (int)m_flImageOffsetX;
		int nImgX1 = (int)( m_flImageOffsetX + (float)nTall );
		vgui::surface()->DrawTexturedSubRect( nImgX0, 0, nImgX1, nTall, 0.0f, 0.0f, 1.0f, 1.0f );
		vgui::surface()->DrawSetTexture( 0 );
	}
}

//-----------------------------------------------------------------------------
// GamepadUIWorkshopPanel
//-----------------------------------------------------------------------------
GamepadUIWorkshopPanel::GamepadUIWorkshopPanel( vgui::Panel *pParent, const char *pPanelName )
	: BaseClass( pParent, pPanelName )
	, m_flTabsOffsetX( 0.0f )
	, m_flTabsOffsetY( 0.0f )
	, m_flOptionsFade( 80.0f )
	, m_flScrollBarOffsetX( 10.0f )
	, m_flScrollBarWidth( 80.0f )
	, m_flScrollBarHeight( 80.0f )
	, m_ChapterOffsetX( 0.0f )
	, m_ChapterOffsetY( 0.0f )
	, m_ChapterSpacing( 0.0f )
	, m_WorkshopOffsetX( 0.0f )
	, m_WorkshopOffsetY( 0.0f )
	, m_WorkshopSpacing( 0.0f )
	, m_nTabCount( 0 )
	, m_pSelectedButton( NULL )
	, m_bMovingAddon( false )
{
	s_pWorkshopPanel = this;

	vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile( "gamepadui/schemepanel.res", "SchemePanel" );
	SetScheme( hScheme );

	m_strTitle.SetString( "#GameUI_Workshop_Title" );

	UpdateFooterButtons();
	Activate();

	BuildTabs();
	LayoutTabs();
	UpdateGradients();

	IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient();
	if ( pEngine && pEngine->GetEventsSubscription() )
	{
		pEngine->GetEventsSubscription()->Subscribe( this );
	}
}

GamepadUIWorkshopPanel::~GamepadUIWorkshopPanel()
{
	IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient();
	if ( pEngine && pEngine->GetEventsSubscription() )
	{
		pEngine->GetEventsSubscription()->Unsubscribe( this );
	}

	GamepadUIButton::s_bSuppressNav = false;

	if ( s_pWorkshopPanel == this )
	{
		s_pWorkshopPanel = NULL;
	}
}

void GamepadUIWorkshopPanel::UpdateGradients()
{
	float flTime = GamepadUI::GetInstance().GetTime();
	GamepadUIGradientHelper &gradients = GamepadUI::GetInstance().GetGradientHelper();

	gradients.ResetTargets( flTime );
	gradients.SetTargetGradient( GamepadUIGradientSides::Up, { 1.0f, 1.0f }, flTime );
	gradients.SetTargetGradient( GamepadUIGradientSides::Left, { 1.0f, 0.6667f }, flTime );
	gradients.SetTargetGradient( GamepadUIGradientSides::Down, { 1.0f, 1.0f }, flTime );
}

int GamepadUIWorkshopPanel::GetCurrentTab() const
{
	int nMaxTab = MAX( 0, m_nTabCount - 1 );
	int nCur = gamepadui_last_workshop_tab.GetInt();
	int nClamped = clamp( nCur, 0, nMaxTab );
	if ( nClamped != nCur )
	{
		gamepadui_last_workshop_tab.SetValue( nClamped );
	}
	return nClamped;
}

void GamepadUIWorkshopPanel::UpdateFooterButtons()
{
	int nMask = FooterButtons::Details | FooterButtons::SelectRight | FooterButtons::Back;
	if ( GetCurrentTab() != 0 )
	{
		nMask = m_bMovingAddon
			? ( FooterButtons::StopMove | FooterButtons::Details | FooterButtons::Back )
			: ( FooterButtons::Move | FooterButtons::Details | FooterButtons::Back );
	}
	SetFooterButtons( nMask, FooterButtons::None );
}

void GamepadUIWorkshopPanel::AddTab( const char *pszTitle )
{
	if ( m_nTabCount >= MAX_WORKSHOP_TABS )
		return;

	char szCmd[64];
	V_snprintf( szCmd, sizeof( szCmd ), "tab %d", m_nTabCount );

	GamepadUIButton *pTabBtn = new GamepadUIButton( this, this, "gamepadui/schemetab.res", szCmd, pszTitle, "" );
	pTabBtn->SetZPos( 50 );
	if ( m_nTabCount == gamepadui_last_workshop_tab.GetInt() )
	{
		pTabBtn->SetSelected( true );
	}

	m_Tabs[m_nTabCount].m_pTabButton = pTabBtn;
	++m_nTabCount;
}

void GamepadUIWorkshopPanel::BuildTabs()
{
	AddTab( "#GameUI_Workshop_Campaigns" );
	AddTab( "#GameUI_Workshop_Subscribed" );
	PopulateWorkshopButtons();
}

void GamepadUIWorkshopPanel::PopulateWorkshopButtons()
{
	PublishedFileId_t ulPrevSelectedId = 0;
	if ( m_pSelectedButton && m_pSelectedButton->GetItem() )
	{
		ulPrevSelectedId = m_pSelectedButton->GetItem()->m_Details.m_nPublishedFileId;
	}
	m_pSelectedButton = NULL;

	m_pCampaignButtons.PurgeAndDeleteElements();
	m_pAddonButtons.PurgeAndDeleteElements();
	for ( int t = 0; t < m_nTabCount; ++t )
	{
		m_Tabs[t].m_Buttons.RemoveAll();
	}

	m_bMovingAddon = false;
	GamepadUIButton::s_bSuppressNav = false;

	IWorkshop *pWorkshop = GamepadUI::GetInstance().GetWorkshop();
	if ( !pWorkshop )
	{
		UpdateFooterButtons();
		return;
	}

	static ConVarRef workshop_dev_stress_test( "workshop_dev_stress_test" );
	int nLoops = workshop_dev_stress_test.IsValid() ? workshop_dev_stress_test.GetInt() : 0;
	if ( nLoops <= 0 )
		nLoops = 1;

	// Tab 0: Campaigns (horizontal square cards, sorted by time added)
	CUtlVector<PublishedFileId_t> vecCampaigns;
	pWorkshop->GetSubscribedItemsSortedByTime( vecCampaigns );

	int nCampaignIdx = 0;
	for ( int loop = 0; loop < nLoops; ++loop )
	{
		for ( int i = 0; i < vecCampaigns.Count(); ++i )
		{
			WorkshopItem_t *pItem = pWorkshop->GetItem( vecCampaigns[i] );
			if ( !pItem || !pItem->IsCampaign() )
				continue;

			char szCmd[128];
			V_snprintf( szCmd, sizeof( szCmd ), "campaign %llu", (unsigned long long)vecCampaigns[i] );

			GamepadUIWorkshopButton_Square *pCard = new GamepadUIWorkshopButton_Square(
				this, this, "gamepadui/schemeworkshopbutton_grid.res", szCmd, " ", "", pItem );
			pCard->SetPriority( nCampaignIdx );
			pCard->SetForwardToParent( true );

			m_pCampaignButtons.AddToTail( pCard );
			if ( m_nTabCount > 0 )
			{
				m_Tabs[0].m_Buttons.AddToTail( pCard );
			}
			++nCampaignIdx;
		}
	}

	if ( GetCurrentTab() == 0 && m_pCampaignButtons.Count() > 0 )
	{
		m_pCampaignButtons[0]->NavigateTo();
	}

	for ( int i = 1; i < m_pCampaignButtons.Count(); ++i )
	{
		m_pCampaignButtons[i]->SetNavLeft( m_pCampaignButtons[i - 1] );
		m_pCampaignButtons[i - 1]->SetNavRight( m_pCampaignButtons[i] );
	}

	// Tab 1: Subscribed addons (vertical list, in load-priority order)
	CUtlVector<PublishedFileId_t> vecAddons;
	pWorkshop->GetSubscribedItemsInOrder( vecAddons );

	int nAddonIdx = 0;
	for ( int loop = 0; loop < nLoops; ++loop )
	{
		for ( int i = 0; i < vecAddons.Count(); ++i )
		{
			WorkshopItem_t *pItem = pWorkshop->GetItem( vecAddons[i] );
			if ( !pItem || pItem->IsCampaign() )
				continue;

			char szCmd[128];
			V_snprintf( szCmd, sizeof( szCmd ), "toggle %llu", (unsigned long long)vecAddons[i] );

			GamepadUIWorkshopButton_List *pRow = new GamepadUIWorkshopButton_List(
				this, this, "gamepadui/schemeworkshopbutton_list.res", szCmd, pItem->m_Details.m_rgchTitle, "", pItem );
			pRow->SetPriority( nAddonIdx );
			pRow->SetForwardToParent( true );

			m_pAddonButtons.AddToTail( pRow );
			if ( m_nTabCount > 1 )
			{
				m_Tabs[1].m_Buttons.AddToTail( pRow );
			}
			++nAddonIdx;
		}
	}

	if ( GetCurrentTab() == 1 && m_pAddonButtons.Count() > 0 )
	{
		m_pAddonButtons[0]->NavigateTo();
	}

	for ( int i = 1; i < m_pAddonButtons.Count(); ++i )
	{
		m_pAddonButtons[i]->SetNavUp( m_pAddonButtons[i - 1] );
		m_pAddonButtons[i - 1]->SetNavDown( m_pAddonButtons[i] );
	}

	if ( ulPrevSelectedId != 0 )
	{
		for ( int i = 0; i < m_pAddonButtons.Count(); ++i )
		{
			WorkshopItem_t *pItem = m_pAddonButtons[i]->GetItem();
			if ( pItem && pItem->m_Details.m_nPublishedFileId == ulPrevSelectedId )
			{
				m_pSelectedButton = m_pAddonButtons[i];
			}
		}
	}

	UpdateFooterButtons();
}

void GamepadUIWorkshopPanel::LayoutTabs()
{
	int nTabX = (int)m_flTabsOffsetX;
	for ( int t = 0; t < m_nTabCount; ++t )
	{
		GamepadUIButton *pTabBtn = m_Tabs[t].m_pTabButton;
		if ( pTabBtn )
		{
			pTabBtn->SetPos( nTabX, (int)m_flTabsOffsetY );
			pTabBtn->SetVisible( true );
			nTabX += pTabBtn->GetWide();
		}

		for ( int i = 0; i < m_Tabs[t].m_Buttons.Count(); ++i )
		{
			m_Tabs[t].m_Buttons[i]->SetVisible( false );
		}
	}

	if ( m_nTabCount <= 0 )
		return;

	int nActiveTab = GetCurrentTab();
	WorkshopTab_t &tab = m_Tabs[nActiveTab];

	int nMaxScroll = 0;
	int nScrollableCount = tab.m_Buttons.Count() - 8;
	for ( int i = 0; i < nScrollableCount; ++i )
	{
		nMaxScroll += tab.m_Buttons[i]->GetTall();
	}

	if ( nActiveTab != 0 )
	{
		tab.m_ScrollState.UpdateScrollBounds( 0.0f, (float)nMaxScroll );
	}
}

void GamepadUIWorkshopPanel::LayoutWorkshopButtons()
{
	int nParentW = 0, nParentH = 0;
	if ( GetParent() )
		GetParent()->GetSize( nParentW, nParentH );
	else
		GetSize( nParentW, nParentH );

	int nActiveTab = GetCurrentTab();

	if ( nActiveTab == 0 )
	{
		// Campaigns tab: horizontal row of square cards
		float flMaxScroll = 0.0f;
		if ( m_pCampaignButtons.Count() > 0 )
		{
			int nCardW = m_pCampaignButtons[0]->GetWide();
			for ( int i = 0; i < m_pCampaignButtons.Count() - 1; ++i )
			{
				flMaxScroll += (float)( (int)( (float)nCardW + m_WorkshopSpacing ) );
			}
		}

		m_Tabs[0].m_ScrollState.UpdateScrollBounds( 0.0f, flMaxScroll );

		if ( m_pCampaignButtons.Count() > 0 )
		{
			int nCardW = m_pCampaignButtons[0]->GetWide();
			int nStride = (int)( (float)nCardW + m_WorkshopSpacing );
			int nY = (int)m_WorkshopOffsetY;
			float flStartX = m_WorkshopOffsetX - m_Tabs[0].m_ScrollState.GetScrollProgress();

			for ( int i = 0; i < m_pCampaignButtons.Count(); ++i )
			{
				m_pCampaignButtons[i]->SetPos( (int)( flStartX + (float)( i * nStride ) ), nY );
				m_pCampaignButtons[i]->SetVisible( true );
			}
		}

		m_Tabs[0].m_ScrollState.UpdateScroll( GamepadUI::GetInstance().GetTime(), 0.5f );

		for ( int i = 0; i < m_pAddonButtons.Count(); ++i )
		{
			m_pAddonButtons[i]->SetVisible( false );
		}
		return;
	}

	// Hide campaign cards when not on Campaigns tab
	for ( int i = 0; i < m_pCampaignButtons.Count(); ++i )
	{
		m_pCampaignButtons[i]->SetVisible( false );
	}

	if ( nActiveTab != 1 )
	{
		for ( int i = 0; i < m_pAddonButtons.Count(); ++i )
		{
			m_pAddonButtons[i]->SetVisible( false );
		}
		return;
	}

	// Subscribed addons tab: vertical list with top/bottom fade
	int nListStartY = ( m_nTabCount > 0 && m_Tabs[0].m_pTabButton )
		? (int)( m_flTabsOffsetY + (float)m_Tabs[0].m_pTabButton->GetTall() )
		: (int)m_flTabsOffsetY;

	float flBottomLimit = (float)nParentH - m_flFooterButtonsOffsetY - (float)m_nFooterButtonHeight;
	int nAccumY = 0;

	for ( int i = 0; i < m_pAddonButtons.Count(); ++i )
	{
		GamepadUIWorkshopButton_List *pRow = m_pAddonButtons[i];
		int nX = (int)m_flTabsOffsetX;
		int nY = (int)( (float)( nListStartY + nAccumY ) - m_Tabs[1].m_ScrollState.GetScrollProgress() );
		int nAlpha = 255;

		if ( nY < nListStartY )
		{
			float flRowH = (float)pRow->GetTall();
			float flDenom = flRowH - m_flOptionsFade;
			if ( flDenom == 0.0f )
			{
				nAlpha = ( (float)( nListStartY - nY ) <= 0.0f ) ? 255 : 0;
			}
			else
			{
				float flFrac = clamp( ( (float)( nListStartY - nY ) + flDenom ) / flDenom, 0.0f, 1.0f );
				nAlpha = (int)( flFrac * 255.0f );
			}
		}
		else if ( (float)nY > flBottomLimit - m_flOptionsFade )
		{
			if ( m_flOptionsFade == 0.0f )
			{
				nAlpha = ( flBottomLimit - (float)( nY + pRow->GetTall() ) >= 0.0f ) ? 255 : 0;
			}
			else
			{
				float flFrac = clamp( ( flBottomLimit - (float)( nY + pRow->GetTall() ) ) / m_flOptionsFade, 0.0f, 1.0f );
				nAlpha = (int)( flFrac * 255.0f );
			}
		}

		if ( pRow->HasFocus() && pRow->IsEnabled() && nAlpha != 0 )
		{
			nAlpha = 255;
		}

		pRow->SetAlpha( nAlpha );
		pRow->SetVisible( true );
		pRow->SetPos( nX, nY );
		nAccumY += pRow->GetTall();
	}

	m_Tabs[1].m_ScrollState.UpdateScroll( GamepadUI::GetInstance().GetTime(), 0.5f );
}

void GamepadUIWorkshopPanel::ChangeTab( int nTab )
{
	if ( m_nTabCount <= 0 )
		return;

	int nOldTab = GetCurrentTab();
	int nClamped = clamp( nTab, 0, m_nTabCount - 1 );
	gamepadui_last_workshop_tab.SetValue( nClamped );
	int nNewTab = GetCurrentTab();

	if ( nNewTab == nOldTab )
		return;

	for ( int i = 0; i < m_nTabCount; ++i )
	{
		if ( m_Tabs[i].m_pTabButton )
		{
			m_Tabs[i].m_pTabButton->SetSelected( i == nNewTab );
		}
	}

	WorkshopTab_t &tab = m_Tabs[nNewTab];
	GamepadUIButton *pFocusTarget = NULL;

	for ( int i = 0; i < tab.m_Buttons.Count(); ++i )
	{
		ButtonState::State st = tab.m_Buttons[i]->GetCurrentButtonState();
		if ( st == ButtonState::Pressed || st == ButtonState::Over )
		{
			pFocusTarget = tab.m_Buttons[i];
			break;
		}
	}

	if ( !pFocusTarget )
	{
		for ( int i = 0; i < tab.m_Buttons.Count(); ++i )
		{
			if ( tab.m_Buttons[i]->IsEnabled() )
			{
				pFocusTarget = tab.m_Buttons[i];
				break;
			}
		}
	}

	for ( int i = 0; i < tab.m_Buttons.Count(); ++i )
	{
		if ( tab.m_Buttons[i] != pFocusTarget )
		{
			tab.m_Buttons[i]->SetSelected( false );
		}
	}

	if ( pFocusTarget )
	{
		pFocusTarget->NavigateTo();
	}

	m_bMovingAddon = false;
	GamepadUIButton::s_bSuppressNav = false;

	if ( m_pSelectedButton )
	{
		m_pSelectedButton->ResetButtonInputState();
	}

	UpdateFooterButtons();
}

void GamepadUIWorkshopPanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	LayoutTabs();
	LayoutWorkshopButtons();
}

void GamepadUIWorkshopPanel::OnThink()
{
	BaseClass::OnThink();
	LayoutTabs();
	LayoutWorkshopButtons();
}

void GamepadUIWorkshopPanel::Paint()
{
	BaseClass::Paint();

	if ( m_nTabCount <= 0 || !m_Tabs[0].m_pTabButton )
		return;

	int nTabW = m_Tabs[0].m_pTabButton->GetWide();
	int nTabH = m_Tabs[0].m_pTabButton->GetTall();
	int nGlyphSize = (int)( (float)nTabH * 0.9f );
	int nGlyphMargin = (int)( (float)nGlyphSize * 0.25f );
	int nGlyphY = (int)( m_flTabsOffsetY + (float)( ( nTabH - nGlyphSize ) / 2 ) );

	if ( m_GlyphLB.SetupGlyph( nGlyphSize, "menu_lb", true ) )
	{
		int nGlyphX = (int)( m_flTabsOffsetX - (float)nGlyphSize - (float)nGlyphMargin );
		vgui::surface()->DrawSetColor( 255, 255, 255, 255 );
		vgui::surface()->DrawSetTexture( m_GlyphLB.GetTextureID( 0 ) );
		vgui::surface()->DrawTexturedRect( nGlyphX, nGlyphY, nGlyphX + nGlyphSize, nGlyphY + nGlyphSize );
		vgui::surface()->DrawSetTexture( 0 );
	}

	if ( m_GlyphRB.SetupGlyph( nGlyphSize, "menu_rb", true ) )
	{
		int nGlyphX = (int)( m_flTabsOffsetX + (float)( m_nTabCount * nTabW ) ) + nGlyphMargin;
		vgui::surface()->DrawSetColor( 255, 255, 255, 255 );
		vgui::surface()->DrawSetTexture( m_GlyphRB.GetTextureID( 0 ) );
		vgui::surface()->DrawTexturedRect( nGlyphX, nGlyphY, nGlyphX + nGlyphSize, nGlyphY + nGlyphSize );
		vgui::surface()->DrawSetTexture( 0 );
	}

	UpdateGradients();
}

void GamepadUIWorkshopPanel::OnMouseWheeled( int nDelta )
{
	if ( m_nTabCount <= 0 )
		return;

	int nActiveTab = GetCurrentTab();
	m_Tabs[nActiveTab].m_ScrollState.OnMouseWheeled( nDelta, GamepadUI::GetInstance().GetTime(), 20.0f * m_WorkshopSpacing );
}

void GamepadUIWorkshopPanel::OnKeyCodePressed( vgui::KeyCode code )
{
	ButtonCode_t baseCode = GamepadUI_NormalizeJoystickCode( code );

	bool bPrevTab = ( baseCode == KEY_XBUTTON_LEFT_SHOULDER || baseCode == STEAMCONTROLLER_LEFT_BUMPER );
	if ( bPrevTab )
	{
		ChangeTab( GetCurrentTab() - 1 );
		return;
	}

	bool bNextTab = ( baseCode == KEY_XBUTTON_RIGHT_SHOULDER || baseCode == STEAMCONTROLLER_RIGHT_BUMPER );
	if ( bNextTab )
	{
		ChangeTab( GetCurrentTab() + 1 );
		return;
	}

	BaseClass::OnKeyCodePressed( code );
}

void GamepadUIWorkshopPanel::OnCommand( const char *pCommand )
{
	if ( !V_strcmp( pCommand, "action_back" ) )
	{
		Close();
		return;
	}

	if ( !V_strcmp( pCommand, "action_move" ) )
	{
		m_bMovingAddon = !m_bMovingAddon;
		if ( !m_pSelectedButton )
		{
			m_bMovingAddon = false;
			UpdateFooterButtons();
			return;
		}

		if ( m_bMovingAddon )
		{
			GamepadUIButton::s_bSuppressNav = true;
			m_pSelectedButton->NavigateTo();

			for ( int i = 0; i < m_pAddonButtons.Count(); ++i )
			{
				m_pAddonButtons[i]->SetSelected( false );
			}

			m_pSelectedButton->NavigateTo();
			m_pSelectedButton->RequestFocus( 0 );
			m_pSelectedButton->SetSelected( m_bMovingAddon );
		}
		else
		{
			m_pSelectedButton->NavigateTo();

			for ( int i = 0; i < m_pAddonButtons.Count(); ++i )
			{
				m_pAddonButtons[i]->SetSelected( false );
			}

			m_pSelectedButton->NavigateTo();
			GamepadUIButton::s_bSuppressNav = false;
		}

		UpdateFooterButtons();
		return;
	}

	if ( !V_strcmp( pCommand, "action_details" ) )
	{
#if !defined( NO_STEAM )
		if ( m_pSelectedButton && m_pSelectedButton->GetItem() && SteamFriends() )
		{
			bool bBeta = ( SteamUtils() && SteamUtils()->GetConnectedUniverse() == k_EUniverseBeta );
			char szURL[2048];
			V_snprintf( szURL, sizeof( szURL ),
				bBeta ? "http://beta.steamcommunity.com/sharedfiles/filedetails/?id=%llu&requirelogin=true"
				      : "http://steamcommunity.com/sharedfiles/filedetails/?id=%llu&requirelogin=true",
				(unsigned long long)m_pSelectedButton->GetItem()->m_Details.m_nPublishedFileId );
			SteamFriends()->ActivateGameOverlayToWebPage( szURL, k_EActivateGameOverlayToWebPageMode_Default );
		}
#endif
		return;
	}

	if ( StringHasPrefix( pCommand, "campaign " ) )
	{
		PublishedFileId_t ulItemId = (PublishedFileId_t)strtoull( pCommand + 9, NULL, 10 );
		if ( ulItemId != 0 )
		{
			IWorkshop *pWorkshop = GamepadUI::GetInstance().GetWorkshop();
			WorkshopItem_t *pItem = pWorkshop ? pWorkshop->GetItem( ulItemId ) : NULL;
			if ( pItem && pItem->IsCampaign() )
			{
#if !defined( NO_STEAM )
				if ( SteamUGC() )
				{
					uint32 unState = SteamUGC()->GetItemState( ulItemId );
					if ( ( unState & k_EItemStateInstalled ) != 0 && ( unState & k_EItemStateNeedsUpdate ) == 0 )
					{
						pWorkshop->LoadCampaign( ulItemId, true );
						Close();
					}
				}
#endif
			}
		}
		return;
	}

	if ( StringHasPrefix( pCommand, "tab " ) )
	{
		if ( pCommand[4] != '\0' )
		{
			int nTab = (int)strtol( pCommand + 4, NULL, 10 );
			ChangeTab( nTab );
		}
		return;
	}

	if ( StringHasPrefix( pCommand, "cmd " ) )
	{
		if ( pCommand[4] != '\0' && GamepadUI::GetInstance().GetEngineClient() )
		{
			GamepadUI::GetInstance().GetEngineClient()->ClientCmd_Unrestricted( pCommand + 4 );
		}
		return;
	}

	BaseClass::OnCommand( pCommand );
}

void GamepadUIWorkshopPanel::OnEvent( KeyValues *pEvent )
{
	if ( !pEvent )
		return;

	const char *pszName = pEvent->GetName();
	bool bSubUpdated = ( V_stricmp( pszName, "Workshop_SubscriptionsUpdated" ) == 0 );
	bool bPriorityChanged = ( V_stricmp( pszName, "Workshop_PriorityChange" ) == 0 );

	if ( bSubUpdated || bPriorityChanged )
	{
		bool bWasMoving = m_bMovingAddon;
		PopulateWorkshopButtons();

		if ( bPriorityChanged )
		{
			if ( GetCurrentTab() == 1 )
			{
				PublishedFileId_t ulId = pEvent->GetUint64( "id", 0 );
				for ( int i = 0; i < m_pAddonButtons.Count(); ++i )
				{
					WorkshopItem_t *pItem = m_pAddonButtons[i]->GetItem();
					if ( pItem && pItem->m_Details.m_nPublishedFileId == ulId )
					{
						m_pSelectedButton = m_pAddonButtons[i];
						m_pSelectedButton->NavigateTo();
						m_pSelectedButton->SetSelected( false );
						if ( bWasMoving )
						{
							GamepadUIButton::s_bSuppressNav = true;
							m_pSelectedButton->SetSelected( true );
							m_pSelectedButton->RequestFocus( 0 );
							m_bMovingAddon = true;
						}
					}
				}
			}
			UpdateFooterButtons();
		}
		return;
	}

	if ( V_stricmp( pszName, "Workshop_EnableChange" ) == 0 )
	{
		IWorkshop *pWorkshop = GamepadUI::GetInstance().GetWorkshop();
		PublishedFileId_t ulId = pEvent->GetUint64( "id", 0 );
		if ( !pWorkshop || ulId == 0 )
			return;

		for ( int i = 0; i < m_pAddonButtons.Count(); ++i )
		{
			WorkshopItem_t *pItem = m_pAddonButtons[i]->GetItem();
			if ( pItem && pItem->m_Details.m_nPublishedFileId == ulId )
			{
				m_pAddonButtons[i]->SetAddonEnabled( pWorkshop->IsItemEnabled( ulId ) );
			}
		}
	}
}

void GamepadUIWorkshopPanel::OnGamepadUIButtonNavigatedTo( int button )
{
	vgui::VPANEL hVPanel = vgui::ivgui()->HandleToPanel( (vgui::HPanel)button );
	vgui::Panel *pPanel = vgui::ipanel()->GetPanel( hVPanel, GetModuleName() );
	if ( !pPanel )
		return;

	GamepadUIWorkshopButton *pWsBtn = dynamic_cast<GamepadUIWorkshopButton *>( pPanel );
	if ( !pWsBtn )
		return;

	if ( m_bMovingAddon )
	{
		pWsBtn->ResetButtonInputState();
	}
	else
	{
		m_pSelectedButton = pWsBtn;
	}

	GamepadUIWorkshopButton_Square *pCard = dynamic_cast<GamepadUIWorkshopButton_Square *>( pWsBtn );
	if ( !pCard )
		return;

	int nParentW = 0, nParentH = 0;
	if ( GetParent() )
		GetParent()->GetSize( nParentW, nParentH );
	else
		GetSize( nParentW, nParentH );

	int nCardX = 0, nCardY = 0;
	pCard->GetPos( nCardX, nCardY );
	float flCardW = pCard->GetCurrentWidth();

	if ( (float)nCardX + flCardW <= (float)nParentW && nCardX >= 0 )
		return;

	float flStride = flCardW + m_ChapterSpacing;
	float flCardPos = (float)( (int)( (float)pCard->GetPriority() * flStride ) );

	int nTargetOffset = 0;
	if ( nCardX < nParentW / 2 )
	{
		nTargetOffset = (int)( (float)( (int)( ( (float)nParentW - m_ChapterOffsetX ) + flCardPos ) ) - m_ChapterSpacing );
	}
	else
	{
		nTargetOffset = (int)( (float)( (int)( flCardPos + flCardW ) ) + m_ChapterSpacing + flCardW * 0.5f );
	}

	int nActiveTab = GetCurrentTab();
	float flTarget = ( m_ChapterOffsetX - (float)nParentW ) + (float)nTargetOffset;
	m_Tabs[nActiveTab].m_ScrollState.SetScrollTarget( flTarget, GamepadUI::GetInstance().GetTime() );
}

//-----------------------------------------------------------------------------
// Console command to open Workshop panel
//-----------------------------------------------------------------------------
CON_COMMAND( gamepadui_openworkshopdialog, "" )
{
	new GamepadUIWorkshopPanel( GamepadUI::GetInstance().GetBasePanel(), "" );
}
