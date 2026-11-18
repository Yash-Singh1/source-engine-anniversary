//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "LoadGameDialog.h"
#include "EngineInterface.h"
#include "iworkshop.h"
#include "vgui/ISystem.h"
#include "vgui/ISurface.h"
#include "vgui/IVGui.h"
#include "KeyValues.h"
#include "filesystem.h"
#include "steam/steam_api.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/PanelListPanel.h"
#include "vgui_controls/QueryBox.h"
#include "vgui_controls/MessageBox.h"
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose:Constructor
//-----------------------------------------------------------------------------
CLoadGameDialog::CLoadGameDialog(vgui::Panel *parent) : BaseClass(parent, "LoadGameDialog")
{
	m_nWorkshopItemID = 0;

	SetDeleteSelfOnClose(true);
	SetBounds(0, 0, 512, 384);
	SetMinimumSize( 256, 300 );
	SetSizeable( true );

	SetTitle("#GameUI_LoadGame", true);

	vgui::Button *cancel = new vgui::Button( this, "Cancel", "#GameUI_Cancel" );
	cancel->SetCommand( "Close" );

	LoadControlSettings("resource/LoadGameDialog.res");

	SetControlEnabled( "delete", false );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CLoadGameDialog::~CLoadGameDialog()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLoadGameDialog::OnCommand( const char *command )
{
	if ( !stricmp( command, "loadsave" ) )
	{
		int saveIndex = GetSelectedItemSaveIndex();
		if ( m_SaveGames.IsValidIndex(saveIndex) )
		{
			const char *shortName = m_SaveGames[saveIndex].szShortName;
			if ( shortName && shortName[ 0 ] )
			{
				// Load the game, return to top and switch to engine
				char sz[ 256 ];
				if ( m_strOverrideMod.Length() )
				{
					// saves made in a Workshop campaign remember the campaign in their appendix
					KeyValues *pAppendix = m_SaveGames[saveIndex].pAppendix;
					KeyValues *pWorkshop = pAppendix ? pAppendix->FindKey( "Workshop" ) : NULL;
					PublishedFileId_t nCampaignID = pWorkshop ? pWorkshop->GetUint64( "Campaign" ) : 0;
					if ( nCampaignID )
					{
						m_nWorkshopItemID = nCampaignID;

						if ( !( SteamUGC()->GetItemState( nCampaignID ) & k_EItemStateSubscribed ) )
						{
							vgui::MessageBox *pMessageBox = new vgui::MessageBox( "#GameUI_Workshop_MissingCampaign_Title", "#GameUI_Workshop_MissingCampaign_Desc", this );
							pMessageBox->SetOKButtonVisible( true );
							pMessageBox->SetOKButtonText( "#GameUI_Workshop_ViewOnWorkshop" );
							pMessageBox->SetCommand( "viewonworkshop" );
							pMessageBox->SetCloseButtonVisible( true );
							pMessageBox->SetCancelButtonVisible( true );
							pMessageBox->SetCancelButtonText( "#GameUI_Close" );
							pMessageBox->DoModal();
							return;
						}

						if ( SteamUGC()->GetItemState( nCampaignID ) & k_EItemStateNeedsUpdate )
						{
							Msg( "[WorkshopSaveLoad] Item %llu needs an update...\n", nCampaignID );
							return;
						}

						if ( !( SteamUGC()->GetItemState( nCampaignID ) & k_EItemStateInstalled ) )
						{
							Msg( "[WorkshopSaveLoad] Item %llu not installed yet...\n", nCampaignID );
							return;
						}

						if ( !workshop->GetItem( nCampaignID ) )
						{
							Msg( "[WorkshopSaveLoad] Item %llu haven't got UGC details yet!\n", nCampaignID );
							return;
						}

						if ( !workshop->LoadCampaign( nCampaignID, false ) )
						{
							Msg( "[WorkshopSaveLoad] Failed to launch campaign: %llu\n", nCampaignID );
							return;
						}

						Q_snprintf( sz, sizeof( sz ), "disconnect\nprogress_enable\nload %s\n", shortName );
					}
					else
					{
						Q_snprintf( sz, sizeof( sz ), "disconnect\nlaunchmod %s\nprogress_enable\nload %s\n", m_strOverrideMod.Get(), shortName );
					}
				}
				else
				{
					Q_snprintf( sz, sizeof( sz ), "progress_enable\nload %s\n", shortName );
				}
				
				engine->ClientCmd_Unrestricted( sz );
				
				// Close this dialog
				OnClose();
			}
		}
	}
	else if ( !stricmp( command, "viewonworkshop" ) )
	{
		char szURL[2048] = { 0 };
		if ( SteamUtils() && SteamUtils()->GetConnectedUniverse() == k_EUniverseBeta )
		{
			V_sprintf_safe( szURL, "http://beta.steamcommunity.com/sharedfiles/filedetails/?id=%llu&requirelogin=true", m_nWorkshopItemID );
		}
		else
		{
			V_sprintf_safe( szURL, "http://steamcommunity.com/sharedfiles/filedetails/?id=%llu&requirelogin=true", m_nWorkshopItemID );
		}

		SteamFriends()->ActivateGameOverlayToWebPage( szURL );
	}
	else if ( !stricmp( command, "Delete" ) )
	{
		int saveIndex = GetSelectedItemSaveIndex();
		if ( m_SaveGames.IsValidIndex(saveIndex) )
		{
			// confirm the deletion
			QueryBox *box = new QueryBox( "#GameUI_ConfirmDeleteSaveGame_Title", "#GameUI_ConfirmDeleteSaveGame_Info" );
			box->AddActionSignalTarget(this);
			box->SetOKButtonText("#GameUI_ConfirmDeleteSaveGame_OK");
			box->SetOKCommand(new KeyValues("Command", "command", "DeleteConfirmed"));
			box->DoModal();
		}
	}
	else if ( !stricmp( command, "DeleteConfirmed" ) )
	{
		int saveIndex = GetSelectedItemSaveIndex();
		if ( m_SaveGames.IsValidIndex(saveIndex) )
		{
			DeleteSaveGame( m_SaveGames[saveIndex].szFileName );

			// reset the list
			ScanSavedGames();
			m_pGameList->MoveScrollBarToTop();
		}
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

