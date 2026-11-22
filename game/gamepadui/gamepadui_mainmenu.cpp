//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Main menu & pause menu frame for HL2 Anniversary GamepadUI.
//
//===========================================================================//

#include "gamepadui_mainmenu.h"
#include "gamepadui_interface.h"

#include "tier0/icommandline.h"
#include "tier1/convar.h"
#include "tier1/fmtstr.h"
#include "tier1/KeyValues.h"
#include "tier1/strtools.h"
#include "filesystem.h"
#include "cdll_int.h"
#include "vgui/ISurface.h"
#include "vgui/IScheme.h"
#include "vgui_controls/Controls.h"

#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Priority comparator matching retail qsort (0xb8630):
//          lowest priority first (drawn at bottom), highest priority last (top)
//-----------------------------------------------------------------------------
static int __cdecl MainMenuButtonPriorityCompare( GamepadUIButton *const *ppA, GamepadUIButton *const *ppB )
{
	int nA = ( *ppA )->GetPriority();
	int nB = ( *ppB )->GetPriority();
	if ( nA > nB )
		return 1;
	if ( nA < nB )
		return -1;
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Constructor & destructor
//-----------------------------------------------------------------------------
GamepadUIMainMenu::GamepadUIMainMenu( vgui::Panel *pParent )
	: BaseClass( pParent, "MainMenu", true, true )
	, m_bLogoTitleValid( false )
	, m_bLogoSubtitleValid( false )
	, m_bIsHL2Complete( false )
	, m_flButtonSpacing( 0.0f )
	, m_flButtonsOffsetX( 0.0f )
	, m_flButtonsOffsetYMenu( 0.0f )
	, m_flButtonsOffsetYInGame( 0.0f )
	, m_flLogoOffsetX( 0.0f )
	, m_flLogoOffsetYMenu( 0.0f )
	, m_flLogoOffsetYInGame( 0.0f )
	, m_colLogoColor( 255, 255, 255, 255 )
	, m_hLogoFont( vgui::INVALID_FONT )
{
	if ( vgui::scheme() )
	{
		vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile( "gamepadui/schememainmenu.res", "SchemeMainMenu" );
		SetScheme( hScheme );
	}

	const char *pszDefaultDir = CommandLine()->ParmValue( "-defaultgamedir", "hl2" );
	m_strCurrentMod = CommandLine()->ParmValue( "-game", pszDefaultDir );
	m_bIsHL2Complete = ( V_stricmp( m_strCurrentMod.String(), "hl2_complete" ) == 0 );

	// Read title / title2 from gameinfo.txt for default logo text
	if ( g_pFullFileSystem )
	{
		KeyValues *pModData = new KeyValues( "ModData" );
		if ( pModData->LoadFromFile( g_pFullFileSystem, "gameinfo.txt", NULL ) ||
			 pModData->LoadFromFile( g_pFullFileSystem, "gameinfo.txt", "MOD" ) )
		{
			const char *pszTitle = pModData->GetString( "title", "" );
			const char *pszTitle2 = pModData->GetString( "title2", "" );
			m_strDefaultLogoTitle.SetString( pszTitle );
			m_strDefaultLogoSubtitle.SetString( pszTitle2 );
			m_bLogoTitleValid = !m_strDefaultLogoTitle.IsEmpty();
			m_bLogoSubtitleValid = !m_strDefaultLogoSubtitle.IsEmpty();
		}
		pModData->deleteThis();
	}

	LoadMainMenuScript();
	SetFooterButtons( FooterButtons::SelectRight, FooterButtons::SelectRight );
	SwitchCategory( "main" );
}

GamepadUIMainMenu::~GamepadUIMainMenu()
{
}

//-----------------------------------------------------------------------------
// Purpose: ApplySchemeSettings
//-----------------------------------------------------------------------------
void GamepadUIMainMenu::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	if ( vgui::Panel *pParent = GetParent() )
	{
		int nWide = 0, nTall = 0;
		pParent->GetSize( nWide, nTall );
		SetBounds( 0, 0, nWide, nTall );
	}

	const char *pszLogoImage = pScheme->GetResourceString( "Logo.Image" );
	if ( pszLogoImage && pszLogoImage[0] )
	{
		m_LogoImage.SetImage( pszLogoImage );
	}

	m_hLogoFont = pScheme->GetFont( "Logo.Font", true );
}

//-----------------------------------------------------------------------------
// Purpose: Load menu items from gamepadui/mainmenu.res
//-----------------------------------------------------------------------------
void GamepadUIMainMenu::LoadMainMenuScript()
{
	KeyValues *pKV = new KeyValues( "MainMenuScript" );
	if ( !pKV )
	{
		UpdateMenus();
		return;
	}

	if ( GamepadUI_LoadKeyValues( "gamepadui/mainmenu.res", pKV ) )
	{
		for ( KeyValues *pSub = pKV->GetFirstSubKey(); pSub != NULL; pSub = pSub->GetNextKey() )
		{
			const char *pszCommand = pSub->GetString( "command", "" );
			const char *pszText = pSub->GetString( "text", "Sample Text" );
			const char *pszDesc = pSub->GetString( "description", "" );

			GamepadUIButton *pButton = new GamepadUIButton(
				this,
				this,
				"gamepadui/schememainmenu.res",
				pszCommand,
				pszText,
				pszDesc );

			pButton->SetPriority( V_atoi( pSub->GetString( "priority", "0" ) ) );

			if ( pSub->GetBool( "caps", false ) )
			{
				pButton->SetAllCaps( true );
			}

			const char *pszCategory = pSub->GetString( "category", NULL );
			if ( pszCategory && pszCategory[0] )
			{
				pButton->SetCategory( pszCategory );
			}

			pButton->SetVisible( false );

			const char *pszFamily = pSub->GetString( "family", "all" );
			bool bInGame = ( V_strcmp( pszFamily, "ingame" ) == 0 || V_strcmp( pszFamily, "all" ) == 0 );
			bool bMainMenu = ( V_strcmp( pszFamily, "mainmenu" ) == 0 || V_strcmp( pszFamily, "all" ) == 0 );

			if ( bInGame )
			{
				m_pButtons[FAMILY_INGAME].AddToTail( pButton );
			}
			if ( bMainMenu )
			{
				m_pButtons[FAMILY_MAINMENU].AddToTail( pButton );
			}
		}
	}

	pKV->deleteThis();
	UpdateMenus();
}

//-----------------------------------------------------------------------------
// Purpose: Active menu list & vertical layout
//-----------------------------------------------------------------------------
int GamepadUIMainMenu::GetCurrentFamily() const
{
	return GamepadUI::GetInstance().IsInLevel() ? FAMILY_INGAME : FAMILY_MAINMENU;
}

void GamepadUIMainMenu::BuildActiveButtonList()
{
	m_pActiveButtons.RemoveAll();

	const CUtlVector<GamepadUIButton *> &vecFamily = m_pButtons[GetCurrentFamily()];
	const char *pszActiveCategory = m_strCurrentCategory.String();

	for ( int i = 0; i < vecFamily.Count(); ++i )
	{
		GamepadUIButton *pButton = vecFamily[i];
		const char *pszBtnCat = pButton->GetCategory();
		if ( !pszBtnCat || !pszBtnCat[0] || V_stricmp( pszBtnCat, pszActiveCategory ) == 0 )
		{
			m_pActiveButtons.AddToTail( pButton );
		}
	}

	if ( m_pActiveButtons.Count() > 1 )
	{
		m_pActiveButtons.Sort( MainMenuButtonPriorityCompare );
	}
}

void GamepadUIMainMenu::UpdateMenus()
{
	for ( int f = 0; f < FAMILY_COUNT; ++f )
	{
		for ( int i = 0; i < m_pButtons[f].Count(); ++i )
		{
			m_pButtons[f][i]->NavigateFrom();
			m_pButtons[f][i]->SetVisible( false );
		}
	}

	BuildActiveButtonList();

	int nCount = m_pActiveButtons.Count();
	if ( nCount == 0 )
		return;

	for ( int i = 1; i < nCount; ++i )
	{
		m_pActiveButtons[i]->SetNavDown( m_pActiveButtons[i - 1] );
		m_pActiveButtons[i - 1]->SetNavUp( m_pActiveButtons[i] );
	}

	for ( int i = 0; i < nCount; ++i )
	{
		m_pActiveButtons[i]->SetVisible( true );
	}

	// Focus the top-most (highest priority) button
	m_pActiveButtons[nCount - 1]->NavigateTo();
}

float GamepadUIMainMenu::GetButtonsOffsetY() const
{
	return GamepadUI::GetInstance().IsInLevel() ? m_flButtonsOffsetYInGame : m_flButtonsOffsetYMenu;
}

float GamepadUIMainMenu::GetLogoOffsetY() const
{
	return GamepadUI::GetInstance().IsInLevel() ? m_flLogoOffsetYInGame : m_flLogoOffsetYMenu;
}

void GamepadUIMainMenu::LayoutButtons()
{
	int nOffsetY = static_cast<int>( GetButtonsOffsetY() );
	int nPanelTall = GetTall();
	int nPosX = static_cast<int>( m_flButtonsOffsetX );

	for ( int i = 0; i < m_pActiveButtons.Count(); ++i )
	{
		GamepadUIButton *pButton = m_pActiveButtons[i];
		if ( !pButton || !pButton->IsVisible() )
			continue;

		nOffsetY += pButton->GetTall();
		pButton->SetPos( nPosX, nPanelTall - nOffsetY );
		nOffsetY = static_cast<int>( static_cast<float>( nOffsetY ) + m_flButtonSpacing );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Logo state & rendering
//-----------------------------------------------------------------------------
void GamepadUIMainMenu::UpdateLogo()
{
	m_strLogoTitle = m_strDefaultLogoTitle;
	m_strLogoSubtitle = m_strDefaultLogoSubtitle;

	char szExpansion[16] = "";
	if ( !g_pFullFileSystem || !g_pFullFileSystem->GetCurrentGameExpansionId( szExpansion, sizeof( szExpansion ) ) )
		return;

	m_bLogoTitleValid = true;
	m_bLogoSubtitleValid = true;

	const char *pszCategory = m_strCurrentCategory.String();
	if ( !pszCategory || !pszCategory[0] || V_stricmp( pszCategory, "main" ) == 0 )
		return;

	if ( V_stricmp( pszCategory, "episodic" ) == 0 )
	{
		m_strLogoTitle.SetString( "HALF-LIFE'" );
		m_strLogoSubtitle.SetString( "== episode one ==" );
	}
	else if ( V_stricmp( pszCategory, "ep2" ) == 0 )
	{
		m_strLogoTitle.SetString( "HALF-LIFE'" );
		m_strLogoSubtitle.SetString( "== episode two ==" );
	}
	else
	{
		m_strLogoTitle.SetString( "HALF-LIFE'" );
		m_strLogoSubtitle.Clear();
	}
}

void GamepadUIMainMenu::PaintLogo()
{
	if ( !vgui::surface() )
		return;

	vgui::surface()->DrawSetTextColor( m_colLogoColor );
	vgui::surface()->DrawSetTextFont( m_hLogoFont );

	int nTitleW = 0, nTitleH = 0;
	int nSubW = 0, nSubH = 0;

	if ( !m_strLogoTitle.IsEmpty() )
	{
		vgui::surface()->GetTextSize( m_hLogoFont, m_strLogoTitle.String(), nTitleW, nTitleH );
	}
	else if ( m_bLogoTitleValid )
	{
		vgui::surface()->GetTextSize( m_hLogoFont, L"A", nTitleW, nTitleH );
	}

	int nTotalH = nTitleH;

	if ( !m_strLogoSubtitle.IsEmpty() )
	{
		vgui::surface()->GetTextSize( m_hLogoFont, m_strLogoSubtitle.String(), nSubW, nSubH );
		nTotalH += nSubH;
	}
	else if ( m_bLogoSubtitleValid )
	{
		vgui::surface()->GetTextSize( m_hLogoFont, L"A", nSubW, nSubH );
		nTotalH += nSubH;
	}

	int nLogoY = static_cast<int>( static_cast<float>( GetTall() ) - GetLogoOffsetY() - static_cast<float>( nTotalH ) );
	int nLogoX = static_cast<int>( m_flLogoOffsetX );

	int nTexId = m_LogoImage.GetTextureId();
	if ( nTexId > 0 )
	{
		vgui::surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
		vgui::surface()->DrawSetTexture( nTexId );
		vgui::surface()->DrawTexturedRect( nLogoX, nLogoY, nLogoX + ( nTitleH * 3 ), nLogoY + nTitleH );
		vgui::surface()->DrawSetTexture( 0 );
		return;
	}

	// Draw subtitle line and title line
	vgui::surface()->DrawSetTextPos( nLogoX, nLogoY );
	if ( !m_strLogoSubtitle.IsEmpty() )
	{
		vgui::surface()->DrawPrintText( m_strLogoSubtitle.String(), m_strLogoSubtitle.Length() );
	}

	vgui::surface()->DrawSetTextPos( nLogoX, nLogoY - nSubH );
	if ( !m_strLogoTitle.IsEmpty() )
	{
		vgui::surface()->DrawPrintText( m_strLogoTitle.String(), m_strLogoTitle.Length() );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Category & expansion switching (retail commands)
//-----------------------------------------------------------------------------
void GamepadUIMainMenu::SwitchCategory( const char *pszCategory )
{
	if ( pszCategory && V_stricmp( m_strCurrentCategory.String(), pszCategory ) != 0 )
	{
		m_strCurrentCategory = pszCategory;
		UpdateMenus();

		int nFocusIdx = -1;
		for ( int i = m_pActiveButtons.Count() - 1; i >= 0; --i )
		{
			GamepadUIButton *pBtn = m_pActiveButtons[i];
			pBtn->NavigateFrom();
			if ( !pBtn->IsVisible() )
				continue;

			if ( nFocusIdx == -1 )
			{
				const char *pszBtnCat = pBtn->GetCategory();
				if ( !pszBtnCat || V_stricmp( pszBtnCat, pszCategory ) == 0 )
				{
					nFocusIdx = i;
				}
			}
		}

		if ( nFocusIdx != -1 )
		{
			m_pActiveButtons[nFocusIdx]->NavigateTo();
		}
	}

	UpdateLogo();
}

void GamepadUIMainMenu::SwitchExpansion( const char *pszExpansion )
{
	if ( !pszExpansion )
		return;

	const char *pszMod = ( V_stricmp( pszExpansion, "main" ) == 0 ) ? "hl2" : pszExpansion;
	CFmtStr fmtCmd( "launchmod %s\nstartupmenu force", pszMod );

	if ( IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient() )
	{
		pEngine->ClientCmd_Unrestricted( fmtCmd.String() );
	}

	SwitchCategory( pszExpansion );
}

//-----------------------------------------------------------------------------
// Purpose: Panel event overrides
//-----------------------------------------------------------------------------
void GamepadUIMainMenu::OnCommand( const char *pszCommand )
{
	if ( pszCommand && StringHasPrefix( pszCommand, "cmd " ) )
	{
		if ( pszCommand[4] != '\0' )
		{
			if ( IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient() )
			{
				pEngine->ClientCmd_Unrestricted( pszCommand + 4 );
			}
		}
		return;
	}

	BaseClass::OnCommand( pszCommand );
}

void GamepadUIMainMenu::OnKeyCodeReleased( vgui::KeyCode code )
{
	ButtonCode_t norm = GamepadUI_NormalizeJoystickCode( code );
	if ( norm == KEY_XBUTTON_B || norm == STEAMCONTROLLER_B )
	{
		if ( GamepadUI::GetInstance().IsInLevel() )
		{
			if ( IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient() )
			{
				pEngine->ClientCmd_Unrestricted( "gamemenucommand resumegame" );
			}
		}
		return;
	}

	BaseClass::OnKeyCodeReleased( code );
}

void GamepadUIMainMenu::OnSetFocus()
{
	BaseClass::OnSetFocus();
	UpdateGradients();
	UpdateMenus();
}

void GamepadUIMainMenu::OnMenuStateChanged()
{
	UpdateGradients();
	UpdateMenus();
}

void GamepadUIMainMenu::OnThink()
{
	BaseClass::OnThink();
	LayoutButtons();
}

void GamepadUIMainMenu::Paint()
{
	BaseClass::Paint();
	PaintLogo();
}

void GamepadUIMainMenu::UpdateGradients()
{
	GamepadUI &ui = GamepadUI::GetInstance();
	float flTime = ui.GetTime();
	GamepadUIGradientHelper &grad = ui.GetGradientHelper();

	grad.ResetTargets( flTime );
	grad.SetTargetGradient( GamepadUIGradientSides::Left, { 1.0f, 0.666f }, flTime );
}

//-----------------------------------------------------------------------------
// Retail ConCommands for category & expansion switching
//-----------------------------------------------------------------------------
CON_COMMAND( gamepadui_switchcategory, "" )
{
	if ( args.ArgC() <= 1 )
		return;

	if ( GamepadUIMainMenu *pMainMenu = GamepadUI::GetInstance().GetMainMenu() )
	{
		pMainMenu->SwitchCategory( args.Arg( 1 ) );
	}
}

CON_COMMAND( gamepadui_switchexpansion, "" )
{
	if ( args.ArgC() <= 1 )
		return;

	if ( GamepadUIMainMenu *pMainMenu = GamepadUI::GetInstance().GetMainMenu() )
	{
		pMainMenu->SwitchExpansion( args.Arg( 1 ) );
	}
}
