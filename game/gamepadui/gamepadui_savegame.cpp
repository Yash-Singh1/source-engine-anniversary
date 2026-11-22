//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI Save / Load Game Panel & Save Button.
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#include "gamepadui_savegame.h"
#include "gamepadui_interface.h"
#include "gamepadui_basepanel.h"
#include "gamepadui_genericconfirmation.h"

#include "bitmap/tgaloader.h"
#include "cdll_int.h"
#include "filesystem.h"
#include "GameUI/IGameUI.h"
#include "savegame_version.h"
#include "tier1/convar.h"
#include "vgui/IPanel.h"
#include "vgui/IScheme.h"
#include "vgui/ISurface.h"
#include "VGuiMatSurface/IMatSystemSurface.h"

#include <stdlib.h>
#include <string.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MAKEID( d, c, b, a ) ( ((int)(a) << 24) | ((int)(b) << 16) | ((int)(c) << 8) | ((int)(d)) )

//-----------------------------------------------------------------------------
// Purpose: Timestamp sort function for save games (newest first; 0xFFFFFFFF top)
//-----------------------------------------------------------------------------
static int __cdecl SaveGameSortFunc( const void *lhs, const void *rhs )
{
	const SaveGameDescription_t *s1 = (const SaveGameDescription_t *)lhs;
	const SaveGameDescription_t *s2 = (const SaveGameDescription_t *)rhs;

	if ( s1->iTimestamp < s2->iTimestamp )
		return 1;
	else if ( s1->iTimestamp > s2->iTimestamp )
		return -1;

	return strcmp( s1->szFileName, s2->szFileName );
}

//-----------------------------------------------------------------------------
// Purpose: Parses mapName and comment out of a .sav file's GameHeader
//-----------------------------------------------------------------------------
static int SaveReadNameAndComment( FileHandle_t f, OUT_Z_CAP( nameSize ) char *name, int nameSize, OUT_Z_CAP( commentSize ) char *comment, int commentSize )
{
	int i, tag, size, tokenSize, tokenCount;
	char *pSaveData, *pFieldName, **pTokenList;

	name[0] = '\0';
	comment[0] = '\0';

	g_pFullFileSystem->Read( &tag, sizeof( int ), f );
	if ( tag != MAKEID( 'J', 'S', 'A', 'V' ) )
		return 0;

	g_pFullFileSystem->Read( &tag, sizeof( int ), f );
	if ( tag != SAVEGAME_VERSION )
		return 0;

	g_pFullFileSystem->Read( &size, sizeof( int ), f );
	g_pFullFileSystem->Read( &tokenCount, sizeof( int ), f );
	g_pFullFileSystem->Read( &tokenSize, sizeof( int ), f );
	size += tokenSize;

	if ( tokenCount < 0 || tokenCount > 1024 * 1024 * 32 )
		return 0;

	if ( tokenSize < 0 || tokenSize > 1024 * 1024 * 32 )
		return 0;

	if ( size <= 0 )
		return 0;

	pSaveData = new char[size];
	int nBytesRead = g_pFullFileSystem->Read( pSaveData, size, f );
	if ( nBytesRead < size )
	{
		delete[] pSaveData;
		return 0;
	}

	char *pData = pSaveData;
	const char *pEnd = pSaveData + size;

	if ( tokenSize > 0 )
	{
		pTokenList = new char *[tokenCount];
		for ( i = 0; i < tokenCount; i++ )
		{
			if ( pData >= pEnd )
			{
				delete[] pTokenList;
				delete[] pSaveData;
				return 0;
			}
			pTokenList[i] = *pData ? pData : NULL;
			while ( pData < pEnd && *pData++ )
			{
			}
		}
	}
	else
	{
		pTokenList = NULL;
	}

	if ( pData + sizeof( short ) * 2 + sizeof( int ) > pEnd )
	{
		delete[] pTokenList;
		delete[] pSaveData;
		return 0;
	}

	short nFieldSize = 0;
	short nTokenIndex = 0;
	memcpy( &nFieldSize, pData, sizeof( short ) );
	pData += sizeof( short );
	memcpy( &nTokenIndex, pData, sizeof( short ) );
	pFieldName = ( pTokenList && nTokenIndex >= 0 && nTokenIndex < tokenCount ) ? pTokenList[nTokenIndex] : NULL;

	if ( !pFieldName || V_stricmp( pFieldName, "GameHeader" ) != 0 )
	{
		delete[] pTokenList;
		delete[] pSaveData;
		return 0;
	}

	pData += sizeof( short );
	int nNumberOfFields = 0;
	memcpy( &nNumberOfFields, pData, sizeof( int ) );
	if ( nFieldSize < 0 || pData + nFieldSize > pEnd )
	{
		delete[] pTokenList;
		delete[] pSaveData;
		return 0;
	}
	pData += nFieldSize;

	for ( i = 0; i < nNumberOfFields; i++ )
	{
		if ( pData + sizeof( short ) * 2 > pEnd )
			break;

		memcpy( &nFieldSize, pData, sizeof( short ) );
		pData += sizeof( short );
		memcpy( &nTokenIndex, pData, sizeof( short ) );
		pFieldName = ( pTokenList && nTokenIndex >= 0 && nTokenIndex < tokenCount ) ? pTokenList[nTokenIndex] : NULL;
		pData += sizeof( short );

		if ( nFieldSize < 0 || pData + nFieldSize > pEnd )
			break;

		if ( pFieldName && !V_stricmp( pFieldName, "comment" ) )
		{
			int copySize = MIN( commentSize, (int)nFieldSize + 1 );
			V_strncpy( comment, pData, copySize );
		}
		else if ( pFieldName && !V_stricmp( pFieldName, "mapName" ) )
		{
			int copySize = MIN( nameSize, (int)nFieldSize + 1 );
			V_strncpy( name, pData, copySize );
		}

		pData += nFieldSize;
	}

	delete[] pTokenList;
	delete[] pSaveData;

	if ( name[0] != '\0' && comment[0] != '\0' )
		return 1;

	return 0;
}

//=============================================================================
// GamepadUISaveButton
//=============================================================================

GamepadUISaveButton::GamepadUISaveButton(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const char *pText,
	const char *pDescription,
	SaveGameDescription_t *pSaveGame )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription )
	, m_bCustomImage( false )
	, m_pSaveGame( pSaveGame )
{
	char szTga[4096];
	szTga[0] = '\0';
	if ( pSaveGame )
	{
		V_strncpy( szTga, pSaveGame->szFileName, sizeof( szTga ) );
		char *pExt = strstr( szTga, ".sav" );
		if ( pExt )
		{
			memcpy( pExt, ".tga", 5 );
		}
	}

	if ( szTga[0] && g_pFullFileSystem && g_pFullFileSystem->FileExists( szTga ) )
	{
		CUtlMemory<unsigned char> rgbaBuf;
		int nWidth = 0, nHeight = 0;
		if ( TGALoader::LoadRGBA8888( szTga, rgbaBuf, nWidth, nHeight ) )
		{
			IMatSystemSurface *pMatSurface = GamepadUI::GetInstance().GetMatSystemSurface();
			if ( pMatSurface && vgui::surface() )
			{
				int nTextureId = vgui::surface()->CreateNewTextureID( true );
				pMatSurface->DrawSetTextureRGBAEx2( nTextureId, rgbaBuf.Base(), nWidth, nHeight, IMAGE_FORMAT_RGBA8888, true, false );
				m_SaveImage.SetProceduralTextureId( nTextureId );
			}
		}
		m_bCustomImage = true;
	}
	else
	{
		m_SaveImage.SetImage( "gamepadui/save_game.vmt" );
	}
}

GamepadUISaveButton::~GamepadUISaveButton()
{
}

void GamepadUISaveButton::Paint()
{
	int x, y, w, h;
	GetBounds( x, y, w, h );

	PaintButton();

	int nImgWidth = ( h * 180 ) / 100;
	int nTextureId = m_SaveImage.GetTextureId();

	if ( nTextureId > 0 )
	{
		vgui::surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
		vgui::surface()->DrawSetTexture( nTextureId );
		if ( m_bCustomImage )
		{
			// Half-texel UV clamp for 180x100 TGA savegame thumbnails
			vgui::surface()->DrawTexturedSubRect( 0, 0, nImgWidth, h, 0.0f, 0.0f, 1.0f - ( 0.5f / 180.0f ), 1.0f - ( 0.5f / 100.0f ) );
		}
		else
		{
			vgui::surface()->DrawTexturedRect( 0, 0, nImgWidth, h );
		}
		vgui::surface()->DrawSetTexture( 0 );
	}
	else
	{
		vgui::surface()->DrawSetColor( Color( 0, 0, 0, 255 ) );
		vgui::surface()->DrawFilledRect( 0, 0, nImgWidth, h );
	}

	PaintText();
}

//=============================================================================
// GamepadUISaveGamePanel
//=============================================================================

GamepadUISaveGamePanel::GamepadUISaveGamePanel( vgui::Panel *pParent, const char *pPanelName, bool bSaveGame, const char *pOverrideMod )
	: BaseClass( pParent, pPanelName, true, true )
	, m_bSaveGame( bSaveGame )
	, m_flSavesFade( 0.0f )
	, m_flSavesOffsetX( 0.0f )
	, m_flSavesOffsetY( 0.0f )
	, m_flSavesSpacing( 0.0f )
{
	vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile( "gamepadui/schemepanel.res", "SchemePanel" );
	SetScheme( hScheme );

	m_strOverrideMod = pOverrideMod;
	m_strTitle = GamepadUIString( m_bSaveGame ? "#GameUI_SaveGame" : "#GameUI_LoadGame" );

	Activate();

	if ( m_bSaveGame )
	{
		SaveGameDescription_t newSave;
		memset( &newSave, 0, sizeof( newSave ) );
		V_strncpy( newSave.szShortName, "NewSavedGame", sizeof( newSave.szShortName ) );
		V_strncpy( newSave.szComment, "#GameUI_NewSaveGame", sizeof( newSave.szComment ) );
		V_strncpy( newSave.szFileTime, "Current", sizeof( newSave.szFileTime ) );
		newSave.iTimestamp = 0xFFFFFFFFu;
		m_SaveGames.AddToTail( newSave );
	}

	ScanSavedGames();

	if ( m_pSaveButtons.Count() > 0 )
	{
		m_pSaveButtons[0]->NavigateTo();
		for ( int i = 1; i < m_pSaveButtons.Count(); ++i )
		{
			m_pSaveButtons[i]->SetNavUp( m_pSaveButtons[i - 1] );
			m_pSaveButtons[i - 1]->SetNavDown( m_pSaveButtons[i] );
		}
	}

	UpdateGradients();
}

GamepadUISaveGamePanel::~GamepadUISaveGamePanel()
{
}

void GamepadUISaveGamePanel::UpdateGradients()
{
	float flTime = GamepadUI::GetInstance().GetTime();
	GamepadUI::GetInstance().GetGradientHelper().ResetTargets( flTime );
	GamepadUI::GetInstance().GetGradientHelper().SetTargetGradient( GamepadUIGradientSides::Up, { 1.0f, 1.0f }, flTime );
	GamepadUI::GetInstance().GetGradientHelper().SetTargetGradient( GamepadUIGradientSides::Down, { 1.0f, 1.0f }, flTime );
}

void GamepadUISaveGamePanel::Paint()
{
	BaseClass::Paint();

	if ( !m_strNoSaves.IsEmpty() )
	{
		vgui::Panel *pParent = GetParent();
		if ( !pParent )
			return;

		int nParentW, nParentH;
		pParent->GetSize( nParentW, nParentH );

		vgui::surface()->DrawSetTextFont( m_hGenericFont );
		vgui::surface()->DrawSetTextColor( Color( 255, 255, 255, 255 ) );

		int nTextW = 0, nTextH = 0;
		vgui::surface()->GetTextSize( m_hGenericFont, m_strNoSaves.String(), nTextW, nTextH );

		vgui::surface()->DrawSetTextPos( ( nParentW / 2 ) - ( nTextW / 2 ), (int)( m_flSavesOffsetY + (float)nTextH ) );
		vgui::surface()->DrawPrintText( m_strNoSaves.String(), m_strNoSaves.Length() );
	}
}

void GamepadUISaveGamePanel::OnThink()
{
	BaseClass::OnThink();
	LayoutSaveButtons();
}

void GamepadUISaveGamePanel::LayoutSaveButtons()
{
	vgui::Panel *pParent = GetParent();
	if ( !pParent )
		return;

	int nParentW, nParentH;
	pParent->GetSize( nParentW, nParentH );

	float flMaxScroll = 0.0f;
	for ( int i = 0; i < m_pSaveButtons.Count(); ++i )
	{
		int nTall = m_pSaveButtons[i]->GetTall();
		if ( i < m_pSaveButtons.Count() - 3 )
		{
			flMaxScroll += (float)(int)( (float)nTall + m_flSavesSpacing );
		}
	}

	m_ScrollState.UpdateScrollBounds( 0.0f, flMaxScroll );

	int nOffsetY = 0;
	for ( int i = 0; i < m_pSaveButtons.Count(); ++i )
	{
		int nTall = m_pSaveButtons[i]->GetTall();
		int nStep = (int)( (float)nTall + m_flSavesSpacing );
		int nY = (int)( m_flSavesOffsetY - m_ScrollState.GetScrollProgress() + (float)nOffsetY );

		int nAlpha = 255;
		if ( m_flSavesFade > 0.0f )
		{
			if ( (float)nY < m_flSavesOffsetY )
			{
				float flFade = clamp( ( m_flSavesOffsetY - (float)nY ) / m_flSavesFade, 0.0f, 1.0f );
				nAlpha = (int)( ( 1.0f - flFade ) * 255.0f );
			}
			if ( (float)nY > (float)nParentH - m_flSavesFade )
			{
				float flFade = clamp( ( (float)nY + (float)nStep - ( (float)nParentH - m_flSavesFade ) ) / m_flSavesFade, 0.0f, 1.0f );
				nAlpha = (int)( ( 1.0f - flFade ) * 255.0f );
			}
		}

		if ( nAlpha != 0 && m_pSaveButtons[i]->HasFocus() )
		{
			nAlpha = 255;
		}

		m_pSaveButtons[i]->SetAlpha( nAlpha );
		m_pSaveButtons[i]->SetPos( (int)m_flSavesOffsetX, nY );
		m_pSaveButtons[i]->SetVisible( true );

		nOffsetY += nStep;
	}

	m_ScrollState.UpdateScroll( GamepadUI::GetInstance().GetTime(), 0.5f );
}

void GamepadUISaveGamePanel::OnMouseWheeled( int nDelta )
{
	m_ScrollState.OnMouseWheeled( nDelta, GamepadUI::GetInstance().GetTime(), 200.0f );
}

void GamepadUISaveGamePanel::OnGamepadUIButtonNavigatedTo( vgui::VPANEL button )
{
	vgui::Panel *pPanel = vgui::ipanel()->GetPanel( button, GetModuleName() );
	if ( !pPanel )
		return;

	GamepadUISaveButton *pButton = dynamic_cast<GamepadUISaveButton *>( pPanel );
	if ( !pButton || pButton->GetAlpha() == 255 )
		return;

	vgui::Panel *pParent = GetParent();
	if ( !pParent )
		return;

	int nParentW, nParentH;
	pParent->GetSize( nParentW, nParentH );

	int nX, nY;
	pButton->GetPos( nX, nY );

	int nBaseY = (int)( (float)pButton->GetPriority() * ( pButton->GetHeightOut() + m_flSavesSpacing ) );
	int nTargetScroll;
	if ( nY < nParentH / 2 )
	{
		nTargetScroll = (int)( (float)(int)( (float)nBaseY + ( (float)nParentH - m_flSavesOffsetY ) ) - m_flSavesSpacing );
	}
	else
	{
		float flOverH = pButton->GetHeightOver();
		nTargetScroll = (int)( (float)(int)( (float)nBaseY + flOverH ) + ( flOverH * 0.5f + m_flSavesSpacing ) );
	}

	m_ScrollState.SetScrollTarget( (float)nTargetScroll - ( (float)nParentH - m_flSavesOffsetY ), GamepadUI::GetInstance().GetTime() );
}

void GamepadUISaveGamePanel::ScanSavedGames()
{
	if ( !g_pFullFileSystem )
		return;

	char szDirectory[4096];
	const char *pszPathID;
	if ( m_strOverrideMod.IsEmpty() )
	{
		V_snprintf( szDirectory, sizeof( szDirectory ), "save/*.sav" );
		pszPathID = "MOD";
	}
	else
	{
		V_snprintf( szDirectory, sizeof( szDirectory ), "%s/save/*.sav", m_strOverrideMod.Get() );
		pszPathID = "BASE_PATH";
	}

	FileFindHandle_t handle = FILESYSTEM_INVALID_FIND_HANDLE;
	const char *pFileName = g_pFullFileSystem->FindFirst( szDirectory, &handle );
	while ( pFileName )
	{
		if ( !V_strnicmp( pFileName, "HLSave", 6 ) )
		{
			pFileName = g_pFullFileSystem->FindNext( handle );
			continue;
		}

		char szFileName[4096];
		if ( m_strOverrideMod.IsEmpty() )
			V_snprintf( szFileName, sizeof( szFileName ), "save/%s", pFileName );
		else
			V_snprintf( szFileName, sizeof( szFileName ), "%s/save/%s", m_strOverrideMod.Get(), pFileName );

		if ( g_pFullFileSystem->FileExists( szFileName, pszPathID ) )
		{
			SaveGameDescription_t save;
			memset( &save, 0, sizeof( save ) );
			if ( ParseSaveData( szFileName, pFileName, save ) )
			{
				m_SaveGames.AddToTail( save );
			}
		}

		pFileName = g_pFullFileSystem->FindNext( handle );
	}

	if ( handle != FILESYSTEM_INVALID_FIND_HANDLE )
	{
		g_pFullFileSystem->FindClose( handle );
	}

	qsort( m_SaveGames.Base(), m_SaveGames.Count(), sizeof( SaveGameDescription_t ), &SaveGameSortFunc );

	for ( int i = 0; i < m_SaveGames.Count() && i < MAX_LISTED_SAVE_GAMES; ++i )
	{
		SaveGameDescription_t &save = m_SaveGames[i];
		GamepadUISaveButton *pButton = new GamepadUISaveButton(
			this,
			this,
			"gamepadui/schemesavebutton.res",
			"load_save",
			save.szComment,
			save.szFileTime,
			&save );

		pButton->SetPriority( i );
		pButton->SetForwardToParent( true );
		m_pSaveButtons.AddToTail( pButton );
	}

	if ( m_SaveGames.Count() == 0 )
	{
		m_strNoSaves = GamepadUIString( "#GameUI_NoSaveGamesToDisplay" );
		SetFooterButtons( FooterButtons::Back, FooterButtons::None );
	}
	else
	{
		SetFooterButtons( FooterButtons::Back | FooterButtons::SelectRight, FooterButtons::SelectRight );
	}

	SetControlEnabled( "loadsave", false );
	SetControlEnabled( "delete", false );
}

bool GamepadUISaveGamePanel::ParseSaveData( const char *pszFileName, const char *pszShortName, SaveGameDescription_t &save )
{
	char szMapName[SAVEGAME_MAPNAME_LEN];
	char szComment[SAVEGAME_COMMENT_LEN];
	char szElapsedTime[SAVEGAME_ELAPSED_LEN];

	const char *pszPathID = m_strOverrideMod.IsEmpty() ? "MOD" : "BASE_PATH";

	V_strncpy( save.szShortName, pszShortName, sizeof( save.szShortName ) );
	V_strncpy( save.szFileName, pszFileName, sizeof( save.szFileName ) );

	FileHandle_t fh = g_pFullFileSystem->Open( pszFileName, "rb", pszPathID );
	if ( fh == FILESYSTEM_INVALID_HANDLE )
		return false;

	int nReadOk = SaveReadNameAndComment( fh, szMapName, ARRAYSIZE( szMapName ), szComment, ARRAYSIZE( szComment ) );
	g_pFullFileSystem->Close( fh );

	if ( !nReadOk )
		return false;

	V_strncpy( save.szMapName, szMapName, sizeof( save.szMapName ) );

	// Elapsed time is the last 6 characters in comment (mmm:ss)
	size_t nLen = strlen( szComment );
	V_strncpy( szElapsedTime, "??", sizeof( szElapsedTime ) );
	if ( nLen >= 6 )
	{
		V_strncpy( szElapsedTime, &szComment[nLen - 6], 7 );
		szElapsedTime[6] = '\0';

		int nMinutes = atoi( szElapsedTime );
		int nSeconds = atoi( szElapsedTime + 4 );

		if ( nMinutes )
			V_snprintf( szElapsedTime, sizeof( szElapsedTime ), "%d %s %d seconds", nMinutes, nMinutes > 1 ? "minutes" : "minute", nSeconds );
		else
			V_snprintf( szElapsedTime, sizeof( szElapsedTime ), "%d seconds", nSeconds );

		int n = (int)nLen - 6;
		szComment[n] = '\0';
		n--;
		while ( n >= 1 && szComment[n] == ' ' )
		{
			szComment[n--] = '\0';
		}
	}

	const char *pszType = "";
	if ( strstr( pszFileName, "quick" ) )
		pszType = "#GameUI_QuickSave";
	else if ( strstr( pszFileName, "autosave" ) )
		pszType = "#GameUI_AutoSave";

	V_strncpy( save.szType, pszType, sizeof( save.szType ) );
	V_strncpy( save.szComment, szComment, sizeof( save.szComment ) );
	V_strncpy( save.szElapsedTime, szElapsedTime, sizeof( save.szElapsedTime ) );

	long fileTime = g_pFullFileSystem->GetFileTime( pszFileName, NULL );
	char szFileTime[32];
	g_pFullFileSystem->FileTimeToString( szFileTime, sizeof( szFileTime ), fileTime );
	char *pNewline = strchr( szFileTime, '\n' );
	if ( pNewline )
		*pNewline = '\0';

	V_strncpy( save.szFileTime, szFileTime, sizeof( save.szFileTime ) );
	save.iTimestamp = (unsigned int)fileTime;
	return true;
}

void GamepadUISaveGamePanel::DeleteSaveGame( const char *pszFileName )
{
	const char *pszPathID = m_strOverrideMod.IsEmpty() ? "MOD" : "BASE_PATH";

	g_pFullFileSystem->RemoveFile( pszFileName, pszPathID );

	char szTga[4096];
	V_strncpy( szTga, pszFileName, sizeof( szTga ) );
	char *pExt = strstr( szTga, ".sav" );
	if ( pExt )
	{
		memcpy( pExt, ".tga", 5 );
	}
	g_pFullFileSystem->RemoveFile( szTga, pszPathID );
}

void GamepadUISaveGamePanel::FindSaveSlot( char *buffer, int bufsize )
{
	buffer[0] = '\0';
	char szFileName[4096];
	for ( int i = 0; i < 1000; ++i )
	{
		V_snprintf( szFileName, sizeof( szFileName ), "save/half-life-%03i.sav", i );

		FileHandle_t fp = g_pFullFileSystem->Open( szFileName, "rb", NULL );
		if ( !fp )
		{
			// Strip leading "save/" and trailing ".sav"
			V_strncpy( buffer, szFileName + 5, bufsize );
			char *pExt = strstr( buffer, ".sav" );
			if ( pExt )
				*pExt = '\0';
			return;
		}
		g_pFullFileSystem->Close( fp );
	}
}

void GamepadUISaveGamePanel::LoadSaveGame( const SaveGameDescription_t *pSave )
{
	if ( !pSave || !pSave->szShortName[0] )
		return;

	char szCmd[256];
	if ( m_strOverrideMod.Length() > 0 )
	{
		V_snprintf( szCmd, sizeof( szCmd ), "disconnect\nlaunchmod %s\nprogress_enable\nload %s\n", m_strOverrideMod.Get(), pSave->szShortName );
	}
	else
	{
		V_snprintf( szCmd, sizeof( szCmd ), "progress_enable\nload %s\n", pSave->szShortName );
	}

	if ( IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient() )
	{
		pEngine->ClientCmd_Unrestricted( szCmd );
	}
	Close();
}

void GamepadUISaveGamePanel::SaveGame( const SaveGameDescription_t *pSave )
{
	if ( pSave && pSave->szFileName[0] )
	{
		DeleteSaveGame( pSave->szFileName );
	}

	char szSaveName[128];
	FindSaveSlot( szSaveName, sizeof( szSaveName ) );
	if ( !szSaveName[0] )
		return;

	char szCmd[256];
	V_snprintf( szCmd, sizeof( szCmd ), "save %s\n", szSaveName );

	if ( IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient() )
	{
		pEngine->ClientCmd_Unrestricted( szCmd );
	}
	Close();
	if ( IGameUI *pGameUI = GamepadUI::GetInstance().GetGameUI() )
	{
		pGameUI->SendMainMenuCommand( "resumegame" );
	}
}

void GamepadUISaveGamePanel::OnCommand( const char *pCommand )
{
	if ( !V_strcmp( pCommand, "action_back" ) )
	{
		Close();
	}
	else if ( !V_strcmp( pCommand, "load_save" ) )
	{
		for ( int i = 0; i < m_pSaveButtons.Count(); ++i )
		{
			if ( m_pSaveButtons[i]->HasFocus() )
			{
				SaveGameDescription_t *pSave = m_pSaveButtons[i]->GetSaveGame();
				if ( !pSave )
					break;

				if ( !m_bSaveGame )
				{
					LoadSaveGame( pSave );
				}
				else if ( pSave->iTimestamp == 0xFFFFFFFFu )
				{
					SaveGame( pSave );
				}
				else
				{
					new GamepadUIGenericConfirmationPanel(
						GamepadUI::GetInstance().GetBasePanel(),
						"SaveOverwriteConfirmationPanel",
						"#GameUI_ConfirmOverwriteSaveGame_Title",
						"#GameUI_ConfirmOverwriteSaveGame_Info",
						[this, pSave]()
						{
							SaveGame( pSave );
						},
						false,
						true );
				}
				break;
			}
		}
	}
	else
	{
		BaseClass::OnCommand( pCommand );
	}
}

CON_COMMAND( gamepadui_opensavegamedialog, "" )
{
	const char *pszOverrideMod = args.ArgC() > 1 ? args.Arg( 1 ) : NULL;
	new GamepadUISaveGamePanel( GamepadUI::GetInstance().GetBasePanel(), "", true, pszOverrideMod );
}

CON_COMMAND( gamepadui_openloadgamedialog, "" )
{
	const char *pszOverrideMod = args.ArgC() > 1 ? args.Arg( 1 ) : NULL;
	new GamepadUISaveGamePanel( GamepadUI::GetInstance().GetBasePanel(), "", false, pszOverrideMod );
}
