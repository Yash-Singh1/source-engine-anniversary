//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Root VGUI panel for HL2 Anniversary GamepadUI.
//
//===========================================================================//

#include "gamepadui_basepanel.h"
#include "gamepadui_interface.h"
#include "gamepadui_mainmenu.h"

#include "tier0/icommandline.h"
#include "tier1/convar.h"
#include "tier1/strtools.h"
#include "tier1/utlvector.h"
#include "vstdlib/random.h"
#include "filesystem.h"
#include "engine/IEngineSound.h"
#include "vgui/IPanel.h"

#include "tier0/memdbgon.h"

static ConVar gamepadui_background_music_duck( "gamepadui_background_music_duck", "0.35" );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
GamepadUIBasePanel::GamepadUIBasePanel( vgui::VPANEL parent )
	: BaseClass( NULL, "GamepadUIBasePanel" )
	, m_pMainMenu( NULL )
	, m_nBackgroundMusicGUID( 0 )
	, m_bPlayBackgroundMusic( false )
{
	SetParent( parent );
	SetPos( 0, 0 );

	m_nBackgroundMusicGUID = 0;
	m_bPlayBackgroundMusic = ( CommandLine()->FindParm( "-nostartupsound" ) == 0 );

	m_pMainMenu = new GamepadUIMainMenu( this );
	UpdateBackgroundMusic();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
GamepadUIBasePanel::~GamepadUIBasePanel()
{
	StopBackgroundMusic();
}

//-----------------------------------------------------------------------------
// Purpose: Size root panel to parent VPanel and invalidate main menu layout
//-----------------------------------------------------------------------------
void GamepadUIBasePanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	int nWide, nTall;
	vgui::ipanel()->GetSize( GetVParent(), nWide, nTall );
	SetBounds( 0, 0, nWide, nTall );

	m_pMainMenu->InvalidateLayout( false, true );
}

GamepadUIMainMenu *GamepadUIBasePanel::GetMainMenu() const
{
	return m_pMainMenu;
}

//-----------------------------------------------------------------------------
// Purpose: Background music state helpers
//-----------------------------------------------------------------------------
bool GamepadUIBasePanel::IsBackgroundMusicPlaying() const
{
	if ( m_nBackgroundMusicGUID == 0 )
		return false;

	return GamepadUI::GetInstance().GetEngineSound()->IsSoundStillPlaying( m_nBackgroundMusicGUID );
}

void GamepadUIBasePanel::StopBackgroundMusic()
{
	if ( m_nBackgroundMusicGUID != 0 )
	{
		GamepadUI::GetInstance().GetEngineSound()->StopSoundByGuid( m_nBackgroundMusicGUID );
		m_nBackgroundMusicGUID = 0;
	}
}

void GamepadUIBasePanel::StartBackgroundMusic( float flVolume )
{
	if ( IsBackgroundMusicPlaying() )
		return;

	if ( !g_pFullFileSystem )
		return;

	FileFindHandle_t hFind = FILESYSTEM_INVALID_FIND_HANDLE;
	const char *pszFileName = g_pFullFileSystem->FindFirstEx( "sound/ui/gamestartup*.mp3", "MOD", &hFind );
	if ( !pszFileName )
		return;

	CUtlVector<char *> vecTracks;
	while ( pszFileName )
	{
		char szExt[10];
		V_ExtractFileExtension( pszFileName, szExt, sizeof( szExt ) );
		if ( V_stricmp( szExt, "mp3" ) == 0 )
		{
			char szTrack[512];
			V_snprintf( szTrack, sizeof( szTrack ), "ui/%s", pszFileName );
			V_FixSlashes( szTrack, '/' );

			size_t nLen = strlen( szTrack ) + 1;
			char *pszCopy = new char[nLen];
			V_strncpy( pszCopy, szTrack, static_cast<int>( nLen ) );
			vecTracks.AddToTail( pszCopy );
		}
		pszFileName = g_pFullFileSystem->FindNext( hFind );
	}
	g_pFullFileSystem->FindClose( hFind );

	if ( vecTracks.Count() > 0 )
	{
		int nPick = RandomInt( 0, vecTracks.Count() - 1 );
		IEngineSound *pEngineSound = GamepadUI::GetInstance().GetEngineSound();
		if ( pEngineSound )
		{
			float flFinalVol = flVolume * gamepadui_background_music_duck.GetFloat();
			pEngineSound->EmitAmbientSound( vecTracks[nPick], flFinalVol, PITCH_NORM, SND_NOFLAGS, 0.0f );
			m_nBackgroundMusicGUID = pEngineSound->GetGuidForLastSoundEmitted();
		}
	}

	for ( int i = 0; i < vecTracks.Count(); ++i )
	{
		delete[] vecTracks[i];
	}
}

void GamepadUIBasePanel::UpdateBackgroundMusic()
{
	if ( !m_bPlayBackgroundMusic || !GamepadUI::GetInstance().IsInMainMenu() )
	{
		StopBackgroundMusic();
		return;
	}

	if ( !IsBackgroundMusicPlaying() )
	{
		StartBackgroundMusic( 1.0f );
	}
}
