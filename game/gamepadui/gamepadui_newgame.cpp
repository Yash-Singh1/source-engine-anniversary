//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI New Game (Chapter Select) Panel & Chapter Button.
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#include "gamepadui_newgame.h"
#include "gamepadui_interface.h"
#include "gamepadui_basepanel.h"
#include "gamepadui_genericconfirmation.h"

#include "cdll_int.h"
#include "filesystem.h"
#include "tier0/icommandline.h"
#include "tier1/convar.h"
#include "vgui/ILocalize.h"
#include "vgui/IPanel.h"
#include "vgui/IScheme.h"
#include "vgui/ISurface.h"

#include <stdio.h>
#include <stdlib.h>
#pragma push_macro( "min" )
#pragma push_macro( "max" )
#undef min
#undef max
#include <string>
#pragma pop_macro( "max" )
#pragma pop_macro( "min" )

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static const int MAX_CHAPTERS = 32;

GamepadUIChapterButton *GamepadUIChapterButton::s_pLastSelectedButton = NULL;

//-----------------------------------------------------------------------------
// Purpose: Sort function used in displaying chapter list
//-----------------------------------------------------------------------------
static int __cdecl ChapterSortFunc( const void *elem1, const void *elem2 )
{
	const chapter_t *c1 = (const chapter_t *)elem1;
	const chapter_t *c2 = (const chapter_t *)elem2;

	// Compare chapter number first
	static const size_t chapterlen = strlen( "chapter" );
	if ( atoi( c1->filename + chapterlen ) > atoi( c2->filename + chapterlen ) )
		return 1;
	else if ( atoi( c1->filename + chapterlen ) < atoi( c2->filename + chapterlen ) )
		return -1;

	// Compare length second (longer string shows up later, e.g. chapter9 before chapter9a)
	if ( strlen( c1->filename ) > strlen( c2->filename ) )
		return 1;
	else if ( strlen( c1->filename ) < strlen( c2->filename ) )
		return -1;

	// Compare strings third
	return strcmp( c1->filename, c2->filename );
}

//-----------------------------------------------------------------------------
// Purpose: Read unlocked chapter count (from convar or override mod's config.cfg)
//-----------------------------------------------------------------------------
static int GetUnlockedChapter( const char *pszOverrideMod )
{
	ConVarRef var( "sv_unlockedchapters" );
	int iUnlockedChapter = atoi( var.IsValid() ? var.GetString() : "1" );

	if ( !g_pFullFileSystem )
		return iUnlockedChapter;

	if ( pszOverrideMod && strlen( pszOverrideMod ) > 0 )
	{
		char szConfigFile[MAX_PATH] = { 0 };
		V_snprintf( szConfigFile, sizeof( szConfigFile ), "%s/cfg/config.cfg", pszOverrideMod );

		FileHandle_t f = g_pFullFileSystem->Open( szConfigFile, "rb", "BASE_PATH" );
		if ( f )
		{
			int nSize = g_pFullFileSystem->Size( f );
			if ( nSize > 0 )
			{
				char *pBuf = new char[nSize + 1];
				g_pFullFileSystem->Read( pBuf, nSize, f );
				pBuf[nSize] = '\0';
				g_pFullFileSystem->Close( f );

				const char *pFound = V_stristr( pBuf, "sv_unlockedchapters" );
				if ( pFound && ( pFound - pBuf ) + 21 < nSize )
				{
					// Skip past `sv_unlockedchapters "` (19 + 2 = 21 bytes)
					const char *pVal = pFound + 21;
					int nRemaining = (int)strlen( pVal );
					int nLen = 0;
					bool bFoundEnd = false;
					while ( nLen < nRemaining )
					{
						char ch = pVal[nLen];
						nLen++;
						if ( ch == '\n' || ch == ' ' || ch == '"' || ch == '\'' )
						{
							bFoundEnd = true;
							break;
						}
					}

					int nCopyLen = bFoundEnd ? MIN( nLen, 32 ) : 1;
					char szNum[33];
					V_strncpy( szNum, pVal, nCopyLen );
					szNum[nCopyLen] = '\0';
					iUnlockedChapter = atoi( szNum );
				}

				delete[] pBuf;
			}
			else
			{
				g_pFullFileSystem->Close( f );
			}
		}
	}

	return iUnlockedChapter;
}

//-----------------------------------------------------------------------------
// Purpose: Scan chapter*.cfg files on disk for current or override mod
//-----------------------------------------------------------------------------
static int ScanChapters( chapter_t *pChapters, const char *pszOverrideMod )
{
	if ( !g_pFullFileSystem )
		return 0;

	char szSearchPath[MAX_PATH] = { 0 };
	const char *pszPathID = "MOD";

	if ( pszOverrideMod && strlen( pszOverrideMod ) > 0 )
	{
		V_snprintf( szSearchPath, sizeof( szSearchPath ), "%s/cfg/chapter*.cfg", pszOverrideMod );
		pszPathID = "BASE_PATH";
	}
	else
	{
		V_snprintf( szSearchPath, sizeof( szSearchPath ), "cfg/chapter*.cfg" );
		pszPathID = "MOD";
	}

	int nChapterCount = 0;
	FileFindHandle_t findHandle = FILESYSTEM_INVALID_FIND_HANDLE;
	const char *pszFileName = g_pFullFileSystem->FindFirstEx( szSearchPath, pszPathID, &findHandle );
	while ( pszFileName && nChapterCount < MAX_CHAPTERS )
	{
		if ( pszFileName[0] )
		{
			char szFullFileName[MAX_PATH];
			if ( pszOverrideMod && strlen( pszOverrideMod ) > 0 )
				V_snprintf( szFullFileName, sizeof( szFullFileName ), "%s/cfg/%s", pszOverrideMod, pszFileName );
			else
				V_snprintf( szFullFileName, sizeof( szFullFileName ), "cfg/%s", pszFileName );

			FileHandle_t f = g_pFullFileSystem->Open( szFullFileName, "rb", pszPathID );
			if ( f )
			{
				// Don't load chapter files that are empty (used in the demo)
				if ( g_pFullFileSystem->Size( f ) > 0 )
				{
					V_strncpy( pChapters[nChapterCount].filename, pszFileName, sizeof( pChapters[nChapterCount].filename ) );
					++nChapterCount;
				}
				g_pFullFileSystem->Close( f );
			}
		}
		pszFileName = g_pFullFileSystem->FindNext( findHandle );
	}

	if ( findHandle != FILESYSTEM_INVALID_FIND_HANDLE )
	{
		g_pFullFileSystem->FindClose( findHandle );
	}

	qsort( pChapters, nChapterCount, sizeof( chapter_t ), &ChapterSortFunc );
	return nChapterCount;
}

//-----------------------------------------------------------------------------
// Purpose: Game feature checks for New Game footer buttons
//-----------------------------------------------------------------------------
static bool HasCommentarySupport()
{
	const char *pszGame = CommandLine()->ParmValue( "-game", CommandLine()->ParmValue( "-defaultgamedir", "hl2" ) );
	return !V_strcmp( pszGame, "hl2" ) ||
		!V_strcmp( pszGame, "hl2_complete" ) ||
		!V_strcmp( pszGame, "episodic" ) ||
		!V_strcmp( pszGame, "ep2" ) ||
		!V_strcmp( pszGame, "portal" ) ||
		!V_strcmp( pszGame, "lostcoast" );
}

static bool HasBonusMapsSupport()
{
	const char *pszGame = CommandLine()->ParmValue( "-game", CommandLine()->ParmValue( "-defaultgamedir", "hl2" ) );
	return !V_strcmp( pszGame, "portal" );
}

//=============================================================================
// GamepadUIChapterButton
//=============================================================================

GamepadUIChapterButton::GamepadUIChapterButton(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const wchar_t *pText,
	const wchar_t *pDescription,
	const char *pChapterImage )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription )
{
	m_ChapterImage.SetImage( pChapterImage );
}

GamepadUIChapterButton::~GamepadUIChapterButton()
{
	if ( s_pLastSelectedButton == this )
		s_pLastSelectedButton = NULL;
}

void GamepadUIChapterButton::NavigateTo()
{
	BaseClass::NavigateTo();
	s_pLastSelectedButton = this;
}

ButtonState::State GamepadUIChapterButton::GetCurrentButtonState()
{
	if ( this == s_pLastSelectedButton )
		return ButtonState::Over;

	return BaseClass::GetCurrentButtonState();
}

void GamepadUIChapterButton::Paint()
{
	int x, y, w, h;
	GetBounds( x, y, w, h );

	PaintButton();

	vgui::surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
	vgui::surface()->DrawSetTexture( m_ChapterImage );

	// 16:9 thumbnail that smoothly crops upward as text slides up on hover
	float flBaseHeight = (float)( ( w * 9 ) / 16 );
	float flTall = flBaseHeight + ( m_flTextOffsetY - m_flTextOffsetYState[ButtonState::Out] );
	int nImageTall = (int)flTall;
	if ( flBaseHeight > 0.0f )
	{
		vgui::surface()->DrawTexturedSubRect( 0, 0, w, nImageTall, 0.0f, 0.0f, 1.0f, flTall / flBaseHeight );
	}
	vgui::surface()->DrawSetTexture( 0 );

	if ( !IsEnabled() )
	{
		vgui::surface()->DrawSetColor( Color( 255, 255, 255, 16 ) );
		vgui::surface()->DrawFilledRect( 0, 0, w, nImageTall );
	}

	PaintText();
}

//=============================================================================
// GamepadUINewGamePanel
//=============================================================================

GamepadUINewGamePanel::GamepadUINewGamePanel( vgui::Panel *pParent, const char *pPanelName, const char *pOverrideMod )
	: BaseClass( pParent, pPanelName, true, true )
	, m_ChapterOffsetX( 0.0f )
	, m_ChapterOffsetY( 0.0f )
	, m_ChapterSpacing( 0.0f )
	, m_bCommentaryMode( false )
{
	vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile( "gamepadui/schemepanel.res", "SchemePanel" );
	SetScheme( hScheme );

	m_strOverrideMod = pOverrideMod;
	m_strTitle = GamepadUIString( "#GameUI_NewGame" );

	int nFooterButtons = FooterButtons::Back | FooterButtons::SelectRight;
	if ( HasCommentarySupport() )
		nFooterButtons |= FooterButtons::Commentary;
	if ( HasBonusMapsSupport() )
		nFooterButtons |= FooterButtons::BonusMaps;
	SetFooterButtons( nFooterButtons, FooterButtons::SelectRight );

	Activate();

	chapter_t chapters[MAX_CHAPTERS];
	int nChapterCount = ScanChapters( chapters, m_strOverrideMod.Get() );
	int nUnlockedChapter = GetUnlockedChapter( m_strOverrideMod.Get() );

	for ( int i = 0; i < nChapterCount; ++i )
	{
		const char *pszFileName = chapters[i].filename;
		char szChapterID[32] = { 0 };
		sscanf( pszFileName, "chapter%31s", szChapterID );
		char *pExt = V_stristr( szChapterID, ".cfg" );
		if ( pExt )
			*pExt = '\0';

		const char *pGameDir = GamepadUI_GetModName();
		if ( m_strOverrideMod.Length() > 0 )
			pGameDir = m_strOverrideMod.Get();

		char szChapterTitle[64];
		V_snprintf( szChapterTitle, sizeof( szChapterTitle ), "#%s_Chapter%s_Title", pGameDir, szChapterID );

		char szCommand[32];
		V_snprintf( szCommand, sizeof( szCommand ), "chapter %d", i );

		const wchar_t *pwszChapter = g_pVGuiLocalize ? g_pVGuiLocalize->Find( "#GameUI_Chapter" ) : NULL;
		wchar_t wszNum[32];
		V_UTF8ToUnicode( szChapterID, wszNum, sizeof( wszNum ) );

		wchar_t wszChapterDesc[32];
		V_swprintf_safe( wszChapterDesc, L"%ls %ls", pwszChapter ? pwszChapter : L"CHAPTER", wszNum );

		GamepadUIString strChapterTitle( szChapterTitle );

		char szImage[MAX_PATH];
		if ( m_strOverrideMod.Length() > 0 )
			V_snprintf( szImage, sizeof( szImage ), "gamepadui/%s/chapter%s", m_strOverrideMod.Get(), szChapterID );
		else
			V_snprintf( szImage, sizeof( szImage ), "gamepadui/chapter%s", szChapterID );

		char *pDot = strchr( szImage, '.' );
		if ( pDot )
			*pDot = '\0';

		GamepadUIChapterButton *pButton = new GamepadUIChapterButton(
			this,
			this,
			"gamepadui/schemechapterbutton.res",
			szCommand,
			strChapterTitle.String(),
			wszChapterDesc,
			szImage );

		pButton->SetEnabled( nUnlockedChapter > i );
		pButton->SetPriority( i );
		pButton->SetForwardToParent( true );

		m_pChapterButtons.AddToTail( pButton );
		m_Chapters.AddToTail( chapters[i] );
	}

	if ( m_pChapterButtons.Count() > 0 )
	{
		m_pChapterButtons[0]->NavigateTo();
		for ( int i = 1; i < m_pChapterButtons.Count(); ++i )
		{
			m_pChapterButtons[i]->SetNavLeft( m_pChapterButtons[i - 1] );
			m_pChapterButtons[i - 1]->SetNavRight( m_pChapterButtons[i] );
		}
	}

	UpdateGradients();
}

GamepadUINewGamePanel::~GamepadUINewGamePanel()
{
}

void GamepadUINewGamePanel::UpdateGradients()
{
	float flTime = GamepadUI::GetInstance().GetTime();
	GamepadUI::GetInstance().GetGradientHelper().ResetTargets( flTime );
	GamepadUI::GetInstance().GetGradientHelper().SetTargetGradient( GamepadUIGradientSides::Up, { 1.0f, 1.0f }, flTime );
	GamepadUI::GetInstance().GetGradientHelper().SetTargetGradient( GamepadUIGradientSides::Down, { 1.0f, 0.5f }, flTime );
}

void GamepadUINewGamePanel::OnThink()
{
	BaseClass::OnThink();
	LayoutChapterButtons();
}

void GamepadUINewGamePanel::LayoutChapterButtons()
{
	vgui::Panel *pParent = GetParent();
	if ( !pParent )
		return;

	int nParentW, nParentH;
	pParent->GetSize( nParentW, nParentH );

	float flMaxScroll = 0.0f;
	for ( int i = 0; i < m_pChapterButtons.Count(); ++i )
	{
		int nWide = m_pChapterButtons[0]->GetWide();
		if ( i < m_pChapterButtons.Count() - 2 )
		{
			flMaxScroll += (float)(int)( (float)nWide + m_ChapterSpacing );
		}
	}

	m_ScrollState.UpdateScrollBounds( 0.0f, flMaxScroll );

	for ( int i = 0; i < m_pChapterButtons.Count(); ++i )
	{
		int nWide = m_pChapterButtons[0]->GetWide();
		int nStep = (int)( (float)nWide + m_ChapterSpacing );
		int nX = (int)( ( m_ChapterOffsetX - m_ScrollState.GetScrollProgress() ) + (float)( i * nStep ) );
		m_pChapterButtons[i]->SetPos( nX, (int)m_ChapterOffsetY );
		m_pChapterButtons[i]->SetVisible( true );
	}

	m_ScrollState.UpdateScroll( GamepadUI::GetInstance().GetTime(), 0.5f );
}

void GamepadUINewGamePanel::OnMouseWheeled( int nDelta )
{
	m_ScrollState.OnMouseWheeled( nDelta, GamepadUI::GetInstance().GetTime(), 20.0f * m_ChapterSpacing );
}

void GamepadUINewGamePanel::OnGamepadUIButtonNavigatedTo( vgui::VPANEL button )
{
	vgui::Panel *pPanel = vgui::ipanel()->GetPanel( button, GetModuleName() );
	if ( !pPanel )
		return;

	GamepadUIButton *pButton = dynamic_cast<GamepadUIButton *>( pPanel );
	if ( !pButton )
		return;

	vgui::Panel *pParent = GetParent();
	if ( !pParent )
		return;

	int nParentW, nParentH;
	pParent->GetSize( nParentW, nParentH );

	int nX, nY;
	pButton->GetPos( nX, nY );

	float flButtonWidth = pButton->GetCurrentWidth();
	if ( (float)nX + flButtonWidth > (float)nParentW || nX < 0 )
	{
		int nBaseX = (int)( (float)pButton->GetPriority() * ( flButtonWidth + m_ChapterSpacing ) );
		int nTargetScroll;
		if ( nX < nParentW / 2 )
		{
			nTargetScroll = (int)( (float)(int)( (float)nBaseX + ( (float)nParentW - m_ChapterOffsetX ) ) - m_ChapterSpacing );
		}
		else
		{
			nTargetScroll = (int)( (float)(int)( (float)nBaseX + flButtonWidth ) + ( flButtonWidth * 0.5f + m_ChapterSpacing ) );
		}

		m_ScrollState.SetScrollTarget( (float)nTargetScroll - ( (float)nParentW - m_ChapterOffsetX ), GamepadUI::GetInstance().GetTime() );
	}
}

void GamepadUINewGamePanel::StartGame( int nChapter )
{
	if ( !m_Chapters.IsValidIndex( nChapter ) )
		return;

	char szCommand[512];
	szCommand[0] = '\0';

	if ( m_strOverrideMod.Length() > 0 )
	{
		V_snprintf( szCommand, sizeof( szCommand ),
			"disconnect\ndeathmatch 0\nlaunchmod %s\nprogress_enable\nexec %s\n",
			m_strOverrideMod.Get(), m_Chapters[nChapter].filename );
	}
	else
	{
		V_snprintf( szCommand, sizeof( szCommand ),
			"disconnect\ndeathmatch 0\nprogress_enable\nexec %s\n",
			m_Chapters[nChapter].filename );
	}

	ConVarRef commentary( "commentary" );
	if ( commentary.IsValid() )
		commentary.SetValue( m_bCommentaryMode );

	ConVarRef sv_cheats( "sv_cheats" );
	if ( sv_cheats.IsValid() )
		sv_cheats.SetValue( m_bCommentaryMode );

	IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient();

	if ( m_bCommentaryMode )
	{
		if ( GamepadUI::GetInstance().IsInMultiplayer() && pEngine )
		{
			pEngine->ExecuteClientCmd( "disconnect" );
		}

		std::string strCommand( szCommand );
		new GamepadUIGenericConfirmationPanel(
			GamepadUI::GetInstance().GetBasePanel(),
			"SaveOverwriteConfirmationPanel",
			"#GameUI_LoadCommentary",
			"#GAMEUI_Commentary_Console_Explanation",
			[strCommand]()
			{
				if ( IVEngineClient *pEng = GamepadUI::GetInstance().GetEngineClient() )
				{
					pEng->ClientCmd_Unrestricted( strCommand.c_str() );
				}
			},
			true,
			true );
	}
	else if ( pEngine )
	{
		pEngine->ClientCmd_Unrestricted( szCommand );
	}
}

void GamepadUINewGamePanel::OnCommand( const char *pCommand )
{
	if ( !V_strcmp( pCommand, "action_back" ) )
	{
		Close();
	}
	else if ( !V_strcmp( pCommand, "action_commentary" ) )
	{
		if ( GamepadUIChapterButton::s_pLastSelectedButton )
		{
			m_bCommentaryMode = true;
			GamepadUIChapterButton::s_pLastSelectedButton->DoClick();
		}
	}
	else if ( !V_strcmp( pCommand, "action_bonus_maps" ) )
	{
		if ( IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient() )
		{
			pEngine->ClientCmd_Unrestricted( "gamepadui_openbonusmapsdialog\n" );
		}
	}
	else if ( V_stristr( pCommand, "chapter " ) )
	{
		if ( pCommand[8] != '\0' )
		{
			StartGame( atoi( pCommand + 8 ) );
		}
		Close();
	}
	else
	{
		BaseClass::OnCommand( pCommand );
	}
}

CON_COMMAND( gamepadui_opennewgamedialog, "" )
{
	const char *pszOverrideMod = args.ArgC() > 1 ? args.Arg( 1 ) : NULL;
	new GamepadUINewGamePanel( GamepadUI::GetInstance().GetBasePanel(), "NewGame", pszOverrideMod );
}
