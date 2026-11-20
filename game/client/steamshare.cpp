//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Contains an interface to log some events to the Steam Share 
//			timeline via ISteamTimeline 
//
//===========================================================================//

#if defined( HL2_LOSTCOAST )
// The October 2024 Lost Coast build uses Steamworks 1.60's ISteamTimeline.
#include "steam/steam_api_common.h"
#define ISTEAMTIMELINE_H

enum ETimelineGameMode
{
	k_ETimelineGameMode_Invalid = 0,
	k_ETimelineGameMode_Playing = 1,
	k_ETimelineGameMode_Staging = 2,
	k_ETimelineGameMode_Menus = 3,
	k_ETimelineGameMode_LoadingScreen = 4,

	k_ETimelineGameMode_Max,
};

enum ETimelineEventClipPriority
{
	k_ETimelineEventClipPriority_Invalid = 0,
	k_ETimelineEventClipPriority_None = 1,
	k_ETimelineEventClipPriority_Standard = 2,
	k_ETimelineEventClipPriority_Featured = 3,
};

class ISteamTimeline
{
public:
	virtual void SetTimelineStateDescription( const char *pchDescription, float flTimeDelta ) = 0;
	virtual void ClearTimelineStateDescription( float flTimeDelta ) = 0;
	virtual void AddTimelineEvent( const char *pchIcon, const char *pchTitle, const char *pchDescription, uint32 unPriority, float flStartOffsetSeconds, float flDurationSeconds, ETimelineEventClipPriority ePossibleClip ) = 0;
	virtual void SetTimelineGameMode( ETimelineGameMode eMode ) = 0;
};

#define STEAMTIMELINE_INTERFACE_VERSION "STEAMTIMELINE_INTERFACE_V001"

inline ISteamTimeline *SteamTimeline();
STEAM_DEFINE_USER_INTERFACE_ACCESSOR( ISteamTimeline *, SteamTimeline, STEAMTIMELINE_INTERFACE_VERSION );
#endif

#include "cbase.h"
#include "shareddefs.h"
#include "view.h"
#include "steamshare.h"
#include <vgui/ILocalize.h>
#include <tier1/fmtstr.h>
#include "basecombatweapon_shared.h"
#include "ammodef.h"
#include "c_ai_basenpc.h"
#ifdef HL2_ANNIVERSARY_NOV2024
#include "c_basehlcombatweapon.h"
#endif

#include "steam/isteamtimeline.h"
#include "steam/steam_api.h"

extern vgui::ILocalize *g_pVGuiLocalize;

//-----------------------------------------------------------------------------
// Singleton
//-----------------------------------------------------------------------------
static CSteamShareSystem g_SteamShareSystem;

IGameSystem *SteamShareSystem()
{
	return &g_SteamShareSystem;
}

#ifdef HL2_ANNIVERSARY_NOV2024

CSteamTimelineEventHelper *CSteamTimelineEventHelper::s_pFirst = NULL;

#define REGISTER_STEAM_TIMELINE_EVENT( className, eventName ) \
	static CSteamTimelineEventBase_t *Create_##className() \
	{ \
		className *pEvent = new className(); \
		pEvent->m_pszEventName = eventName; \
		return pEvent; \
	} \
	static CSteamTimelineEventHelper g_##className##_Helper( Create_##className );

bool CSteamShareSystem::Init()
{
	// if we don't have the version of ISteamTimeline we need, don't bother listening for any events
	// and just stub out this whole system.
	if ( !SteamTimeline() )
		return true;

	CSteamTimelineEventHelper *pHelper = CSteamTimelineEventHelper::s_pFirst;
	while ( pHelper )
	{
		CSteamTimelineEventBase_t *pEvent = pHelper->m_pfnCreate();
		m_Events.AddToTail( pEvent );
		pHelper = pHelper->m_pNext;
		ListenForGameEvent( pEvent->GetEventName() );
	}

	ListenForGameEvent( "gameui_activate" );
	ListenForGameEvent( "gameui_hide" );

	m_flChapterStartTime = 0.0f;
	m_bInGame = false;
	m_nChapterDeaths = 0;

	SetTimelineGameMode( k_ETimelineGameMode_Menus );

	return true;
}

static const char *GetWeaponTimelineIcon( C_BaseCombatWeapon *pWeapon )
{
	if ( !pWeapon )
		return "steam_combat";

	const char *pszIcon = pWeapon->GetName();
	if ( !V_stricmp( "weapon_physcannon", pszIcon ) )
	{
		pszIcon = dynamic_cast< C_BaseHLCombatWeapon * >( pWeapon )->IsUpgraded() ? "weapon_supergravgun" : "weapon_gravgun";
	}
	return pszIcon;
}

class CSteamTimelineEvent_Choreo : public CSteamTimelineEventBase_t
{
public:
	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event )
	{
		struct NPCMeet_t
		{
			const char *m_pszScene;
			const char *m_pszTitle;
			const char *m_pszDescription;
			const char *m_pszIcon;
		};

		NPCMeet_t npcMeets[] =
		{
			{ "scenes/npc/Gman/gman_intro.vcd", "#GameUI_SteamShare_NPCMeet_Gman", "#Trainride.gman_riseshine", "npc_gman" },
			{ "scenes/trainyard/alyx_greet01.vcd", "#GameUI_SteamShare_NPCMeet_Alyx", "#Trainyard.al_presume", "npc_alyx" },
			{ "scenes/trainyard/security01.vcd", "#GameUI_SteamShare_NPCMeet_Barney", "#Trainyard.ba_thatbeer02", "npc_barney" },
			{ "scenes/k_lab/lab01.vcd", "#GameUI_SteamShare_NPCMeet_DrKleiner", "#k_lab.kl_mygoodness02_cc", "npc_kleiner" },
			{ "scenes/k_lab/lab03.vcd", "#GameUI_SteamShare_NPCMeet_Lamarr", "#k_lab.kl_blast_cc", "npc_lamarr" },
			{ "scenes/eli_lab/labtalk01.vcd", "#GameUI_SteamShare_NPCMeet_DrVance", "#eli_lab.eli_greeting", "npc_vance" },
			{ "scenes/eli_lab/airlock01.vcd", "#GameUI_SteamShare_NPCMeet_DrMossman", "#eli_lab.mo_airlock05", "npc_mossman" },
			{ "scenes/eli_lab/dog_intro.vcd", "#GameUI_SteamShare_NPCMeet_Dog", "#eli_lab.al_thisisdog01", "npc_dog" },
			{ "scenes/ravenholm/cartrap_scene.vcd", "#GameUI_SteamShare_NPCMeet_FatherGrigori", "#ravenholm.cartrap_iamgrig", "npc_grigori" },
			{ "scenes/outland_11/silo_entry_04.vcd", "#GameUI_SteamShare_NPCMeet_DrMagnusson", "#ep_02.mag_silo_excuseme01", "npc_magnusson" },
		};

		const char *pszScene = event->GetString( "scene" );

		for ( int i = 0; i < ARRAYSIZE( npcMeets ); i++ )
		{
			if ( !V_stricmp( pszScene, npcMeets[i].m_pszScene ) )
			{
				char szTitle[1024] = { 0 };
				V_strncpy( szTitle, g_pVGuiLocalize->FindAsUTF8( npcMeets[i].m_pszTitle ), sizeof( szTitle ) );

				// skip any leading caption tags
				const char *pszDescription = g_pVGuiLocalize->FindAsUTF8( npcMeets[i].m_pszDescription );
				while ( pszDescription && *pszDescription == '<' )
				{
					while ( *pszDescription && *pszDescription != '>' )
						pszDescription++;
					if ( *pszDescription == '>' )
						pszDescription++;
				}

				SteamTimeline()->AddInstantaneousTimelineEvent( szTitle, pszDescription, npcMeets[i].m_pszIcon, 40, 10.0f, k_ETimelineEventClipPriority_Standard );
				break;
			}
		}

		if ( !V_stricmp( pszScene, "scenes/citadel/br_freemanatlast.vcd" ) )
		{
			char szTitle[1024] = { 0 };
			V_strncpy( szTitle, g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_NPCMeet_DrBreen" ), sizeof( szTitle ) );
			SteamTimeline()->AddInstantaneousTimelineEvent( szTitle, g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_NPCMeet_DrBreen_Desc" ), "npc_breen", 40, 5.0f, k_ETimelineEventClipPriority_Standard );
		}
	}
};
REGISTER_STEAM_TIMELINE_EVENT( CSteamTimelineEvent_Choreo, "choreo_scene" );

class CSteamTimelineEvent_ChapterUpdate : public CSteamTimelineEventBase_t
{
public:
	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event )
	{
		if ( event->GetInt( "old_chapter_index" ) == event->GetInt( "new_chapter_index" ) )
			return;

		const char *pszChapterName = event->GetString( "chapter_name" );
		const char *pszLocalizedChapterName = g_pVGuiLocalize->FindAsUTF8( event->GetString( "chapter_name" ) );
		pSystem->m_strChapterName = pszLocalizedChapterName;

		if ( event->GetInt( "old_chapter_index" ) != -1 )
		{
			pSystem->UpdateChapterDetails();
		}

		SteamTimeline()->StartGamePhase();
		SteamTimeline()->SetGamePhaseID( pszChapterName );

		const char *pszTitle = g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_NewChapter" );
		SteamTimeline()->AddInstantaneousTimelineEvent( pszTitle, pSystem->m_strChapterName.String(), "steam_bookmark", 40, 0.0f, k_ETimelineEventClipPriority_None );
	}
};
REGISTER_STEAM_TIMELINE_EVENT( CSteamTimelineEvent_ChapterUpdate, "new_chapter" );

class CSteamTimelineEvent_NewLevel : public CSteamTimelineEventBase_t
{
public:
	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event )
	{
		if ( event->GetBool( "is_main_menu" ) )
		{
			if ( pSystem->m_bInGame )
			{
				pSystem->UpdateChapterDetails();
				SteamTimeline()->EndGamePhase();
			}

			pSystem->m_eGameMode = k_ETimelineGameMode_Menus;
			pSystem->UpdateSteamTimelineGameMode();
			pSystem->m_bInGame = false;
		}
		else
		{
			pSystem->m_eGameMode = k_ETimelineGameMode_Playing;
			pSystem->UpdateSteamTimelineGameMode();
			pSystem->m_bInGame = true;
		}
	}
};
REGISTER_STEAM_TIMELINE_EVENT( CSteamTimelineEvent_NewLevel, "new_level" );

class CSteamTimelineEvent_LoadingLevel : public CSteamTimelineEventBase_t
{
public:
	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event )
	{
		if ( event->GetBool( "is_loading" ) )
		{
			pSystem->PushTimelineGameMode( k_ETimelineGameMode_Staging );
		}
		else
		{
			pSystem->PopTimelineGameMode();
		}
	}
};
REGISTER_STEAM_TIMELINE_EVENT( CSteamTimelineEvent_LoadingLevel, "loading_level" );

class CSteamTimelineEvent_AchievementUpdate : public CSteamTimelineEventBase_t
{
public:
	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event )
	{
		const char *pszAchievementName = event->GetString( "achievement_name" );
		int nCurVal = event->GetInt( "cur_val" );
		int nMaxVal = event->GetInt( "max_val" );

		char szLocalizedName[256] = { 0 };
		const char *pszLocalizedName = g_pVGuiLocalize->FindAsUTF8( CFmtStr( "#%s_NAME", pszAchievementName ) );
		if ( V_isempty( pszLocalizedName ) )
			return;

		V_strncpy( szLocalizedName, pszLocalizedName, sizeof( szLocalizedName ) );

		char szFmt[128] = { 0 };
		char szProgress[512] = { 0 };
		char szCurVal[16] = { 0 };
		char szMaxVal[16] = { 0 };
		V_snprintf( szCurVal, sizeof( szCurVal ), "%i", nCurVal );
		V_snprintf( szMaxVal, sizeof( szMaxVal ), "%i", nMaxVal );

		const char *pszFmt = g_pVGuiLocalize->FindAsUTF8( "#GameUI_Achievement_Progress_Fmt" );
		if ( V_isempty( pszFmt ) )
			return;

		V_strncpy( szFmt, pszFmt, sizeof( szFmt ) );
		g_pVGuiLocalize->ConstructString( szProgress, sizeof( szProgress ), szFmt, 3, szLocalizedName, szCurVal, szMaxVal );
		SteamTimeline()->AddInstantaneousTimelineEvent( g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_AchievementProgress" ), szProgress, "steam_achievement", 40, 0.0f, k_ETimelineEventClipPriority_Standard );
	}
};
REGISTER_STEAM_TIMELINE_EVENT( CSteamTimelineEvent_AchievementUpdate, "achievement_event" );

class CSteamTimelineEvent_PlayerDeath : public CSteamTimelineEventBase_t
{
public:
	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event )
	{
		pSystem->m_nChapterDeaths++;
		SteamTimeline()->AddInstantaneousTimelineEvent( g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_PlayerKilled" ), NULL, "steam_death", 40, 0.0f, k_ETimelineEventClipPriority_Standard );
	}
};
REGISTER_STEAM_TIMELINE_EVENT( CSteamTimelineEvent_PlayerDeath, "single_player_death" );

class CSteamTimelineEvent_InventoryUpdated : public CSteamTimelineEventBase_t
{
public:
	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event )
	{
		AddCurrentInventoryToGamePhase( event );
	}

	void AddCurrentInventoryToGamePhase( IGameEvent *event )
	{
		int nOwner = event->GetInt( "owner" );
		IClientEntity *pOwnerEntity = cl_entitylist->GetBaseEntity( nOwner );

		C_BasePlayer *pPlayer = dynamic_cast< C_BasePlayer * >( pOwnerEntity );
		if ( !pPlayer )
			return;



		for ( int i = 0; i < MAX_WEAPONS; i++ )
		{
			C_BaseCombatWeapon *pWeapon = pPlayer->GetWeapon( i );
			if ( !pWeapon )
				continue;

			const char *pszWeaponName = "weapon";
			if ( const char *pszLocalizedName = g_pVGuiLocalize->FindAsUTF8( pWeapon->GetPrintName() ) )
				pszWeaponName = pszLocalizedName;

			SteamTimeline()->AddGamePhaseTag( pszWeaponName, GetWeaponTimelineIcon( pWeapon ), g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_Tooltip_ChapterInventory" ), 0 );
		}
	}
};
REGISTER_STEAM_TIMELINE_EVENT( CSteamTimelineEvent_InventoryUpdated, "weapon_inventory_updated" );

class CSteamTimelineEvent_WeaponEquipped : public CSteamTimelineEventBase_t
{
public:
	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event )
	{
		int nWeaponEntindex = event->GetInt( "entindex" );
		IClientEntity *pWeaponEntity = cl_entitylist->GetBaseEntity( nWeaponEntindex );
		if ( !pWeaponEntity )
			return;

		C_BaseCombatWeapon *pWeapon = dynamic_cast< C_BaseCombatWeapon * >( pWeaponEntity );
		const char *pszWeaponName = "weapon";
		if ( const char *pszLocalizedName = pWeapon ? g_pVGuiLocalize->FindAsUTF8( pWeapon->GetPrintName() ) : NULL )
			pszWeaponName = pszLocalizedName;

		char szWeaponName[1024] = { 0 };
		V_strncpy( szWeaponName, pszWeaponName, sizeof( szWeaponName ) );

		char szTitle[1024] = { 0 };
		g_pVGuiLocalize->ConstructString( szTitle, sizeof( szTitle ), g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_WeaponPickup" ), 1, szWeaponName );

		SteamTimeline()->AddInstantaneousTimelineEvent( szTitle, NULL, GetWeaponTimelineIcon( pWeapon ), 30, 0.0f, k_ETimelineEventClipPriority_None );
	}
};
REGISTER_STEAM_TIMELINE_EVENT( CSteamTimelineEvent_WeaponEquipped, "weapon_equipped" );

class CSteamTimelineEvent_EntityKilled : public CSteamTimelineEventBase_t
{
public:
	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event )
	{
		const char *pszClassName = event->GetString( "ent_class" );
		Vector vecPosition;
		vecPosition.x = event->GetInt( "ent_last_position_x" );
		vecPosition.y = event->GetInt( "ent_last_position_y" );
		vecPosition.z = event->GetInt( "ent_last_position_z" );
		if ( !pszClassName )
			return;



		if ( !V_stricmp( "class C_BaseHelicopter", pszClassName ) )
		{
			SteamTimeline()->AddInstantaneousTimelineEvent( g_pVGuiLocalize->FindAsUTF8( "GameUI_SteamShare_KilledEnemyChopper" ), NULL, "enemy_chopper", 200, 0.0f, k_ETimelineEventClipPriority_Featured );
		}
		else if ( !V_stricmp( "class C_Strider", pszClassName ) )
		{
			SteamTimeline()->AddInstantaneousTimelineEvent( g_pVGuiLocalize->FindAsUTF8( "GameUI_SteamShare_KilledEnemyStrider" ), NULL, "enemy_strider", 200, 0.0f, k_ETimelineEventClipPriority_Featured );
		}
		else if ( !V_stricmp( "class C_CombineGunship", pszClassName ) )
		{
			SteamTimeline()->AddInstantaneousTimelineEvent( g_pVGuiLocalize->FindAsUTF8( "GameUI_SteamShare_KilledEnemyGunship" ), NULL, "enemy_gunship", 200, 0.0f, k_ETimelineEventClipPriority_Featured );
		}

		C_BaseEntity *pInflictor = cl_entitylist->GetBaseEntity( event->GetInt( "entindex_inflictor" ) );
		const char *pszModelName = pInflictor ? modelinfo->GetModelName( modelinfo->GetModel( pInflictor->GetModelIndex() ) ) : "";
		if ( V_stristr( pszModelName, "sawblade" ) )
		{
			SteamTimeline()->AddInstantaneousTimelineEvent( g_pVGuiLocalize->FindAsUTF8( "GameUI_SteamShare_SawbladeKill" ), NULL, "sawblade_kill", 50, 0.0f, k_ETimelineEventClipPriority_Standard );
		}

		C_BaseEntity *pAttacker = cl_entitylist->GetBaseEntity( event->GetInt( "entindex_attacker" ) );
		if ( pAttacker && pAttacker->IsPlayer() )
		{
			if ( ( vecPosition - pAttacker->GetAbsOrigin() ).Length() > 1200.0f )
			{
				SteamTimeline()->AddInstantaneousTimelineEvent( g_pVGuiLocalize->FindAsUTF8( "GameUI_SteamShare_LongDistanceKill" ), NULL, "longdistance_kill", 50, 0.0f, k_ETimelineEventClipPriority_Standard );
			}
		}

		if ( event->GetInt( "damagebits" ) & DMG_BLAST )
		{
			if ( m_pLastExplosionInflictor && pInflictor != m_pLastExplosionInflictor )
			{
				m_nExplosionKills = 1;
			}
			else
			{
				m_nExplosionKills++;
			}
			m_pLastExplosionInflictor = pInflictor;

			if ( m_nExplosionKills == 2 )
			{
				SteamTimeline()->AddInstantaneousTimelineEvent( g_pVGuiLocalize->FindAsUTF8( "GameUI_SteamShare_ExplosionKill" ), NULL, "explosion_kill", 50, 0.0f, k_ETimelineEventClipPriority_Standard );
			}
		}
	}

	int m_nExplosionKills = 0;
	C_BaseEntity *m_pLastExplosionInflictor = NULL;
};
REGISTER_STEAM_TIMELINE_EVENT( CSteamTimelineEvent_EntityKilled, "entity_killed" );

class CSteamTimelineEvent_CommentaryEvent : public CSteamTimelineEventBase_t
{
public:
	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event )
	{
		char szTitle[1024] = { 0 };
		g_pVGuiLocalize->ConstructString( szTitle, sizeof( szTitle ), g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_CommentaryTitle" ), 1, event->GetString( "speaker_name" ) );
		SteamTimeline()->AddRangeTimelineEvent( szTitle, pSystem->m_strChapterName.String(), "steam_chat", 200, 0.0f, event->GetFloat( "duration" ), k_ETimelineEventClipPriority_Featured );
	}
};
REGISTER_STEAM_TIMELINE_EVENT( CSteamTimelineEvent_CommentaryEvent, "commentary_event" );

class CSteamTimelineEvent_CombatEvent : public CSteamTimelineEventBase_t
{
public:
	CSteamTimelineEvent_CombatEvent()
	{
		m_bInCombat = false;
	}

	virtual void HandleEvent( CSteamShareSystem *pSystem, IGameEvent *event )
	{
		if ( !pSystem->m_bInGame )
		{
			if ( m_bInCombat )
			{
				m_bInCombat = false;
				SteamTimeline()->EndRangeTimelineEvent( m_hCombatEvent, 0.0f );
			}
			return;
		}

		bool bInCombat = event->GetBool( "in_combat" );
		if ( bInCombat == m_bInCombat )
			return;

		if ( bInCombat )
		{
			m_hCombatEvent = SteamTimeline()->StartRangeTimelineEvent( g_pVGuiLocalize->FindAsUTF8( "GameUI_SteamShare_CombatEvent" ), NULL, "steam_attack", 50, 0.0f, k_ETimelineEventClipPriority_Standard );
		}
		else
		{
			SteamTimeline()->EndRangeTimelineEvent( m_hCombatEvent, 0.0f );

			int nPlayer = event->GetInt( "player" );
			IClientEntity *pPlayerEntity = cl_entitylist->GetBaseEntity( nPlayer );

			C_BasePlayer *pPlayer = dynamic_cast< C_BasePlayer * >( pPlayerEntity );
			if ( pPlayer && pPlayer->GetHealth() <= 10 )
			{
				SteamTimeline()->AddInstantaneousTimelineEvent( g_pVGuiLocalize->FindAsUTF8( "GameUI_SteamShare_SurvivedCombatLowHealth" ), NULL, "steam_heart", 50, 0.0f, k_ETimelineEventClipPriority_Standard );
			}


		}

		m_bInCombat = bInCombat;
	}

	bool m_bInCombat;
	TimelineEventHandle_t m_hCombatEvent;
};
REGISTER_STEAM_TIMELINE_EVENT( CSteamTimelineEvent_CombatEvent, "combat_event" );

void CSteamShareSystem::FireGameEvent( IGameEvent *event )
{
	for ( int i = 0; i < m_Events.Count(); i++ )
	{
		if ( !V_stricmp( event->GetName(), m_Events[i]->GetEventName() ) )
		{
			m_Events[i]->HandleEvent( this, event );
			break;
		}
	}
}

const char *FormatChapterTimeString( float flSeconds )
{
	static int nWhichStr = 0;
	static char s_szResult[2][32];

	if ( flSeconds > 0.0f )
	{
		int nTime = (int)flSeconds;
		int nHours = nTime / 3600;
		int nRemainder = nTime - nHours * 3600;
		int nSeconds = nTime % 60;
		int nMinutes = nRemainder / 60;
		char *pszResult = s_szResult[nWhichStr];
		V_snprintf( pszResult, sizeof( s_szResult[0] ), "%02i:%02i", nMinutes, nSeconds );
		nWhichStr = ( nWhichStr + 1 ) % 2;
		return pszResult;
	}

	return "00:00:00";
}

void CSteamShareSystem::UpdateChapterDetails()
{
	char szPlaytime[1024] = { 0 };
	g_pVGuiLocalize->ConstructString( szPlaytime, sizeof( szPlaytime ), g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_ChapterPlaytime" ), 1, FormatChapterTimeString( gpGlobals->curtime - m_flChapterStartTime ) );

	SteamTimeline()->SetGamePhaseAttribute( g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_Tooltip_ChapterTitle" ), CFmtStr( "%s", m_strChapterName.String() ), 0 );
	SteamTimeline()->SetGamePhaseAttribute( g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_Tooltip_ChapterPlaytime" ), szPlaytime, 0 );

	char szDeathCount[4];
	V_snprintf( szDeathCount, sizeof( szDeathCount ), "%d", m_nChapterDeaths );
	char szDeaths[1024] = { 0 };
	g_pVGuiLocalize->ConstructString( szDeaths, sizeof( szDeaths ), g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_ChapterDeaths" ), 1, szDeathCount );
	SteamTimeline()->SetGamePhaseAttribute( g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_Tooltip_ChapterDeaths" ), szDeaths, 0 );

	m_flChapterStartTime = 0.0f;
	m_nChapterDeaths = 0;
}

void CSteamShareSystem::UpdateSteamTimelineGameMode()
{
	ETimelineGameMode eGameMode = m_GameModeStack.IsEmpty() ? m_eGameMode : m_GameModeStack.Tail();
	SteamTimeline()->SetTimelineGameMode( eGameMode );

	switch ( eGameMode )
	{
	default:
		SteamTimeline()->SetTimelineTooltip( "", 0.0f );
		break;
	case k_ETimelineGameMode_Playing:
		SteamTimeline()->SetTimelineTooltip( m_strChapterName.String(), 0.0f );
		break;
	case k_ETimelineGameMode_Staging:
		SteamTimeline()->SetTimelineTooltip( g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_Tooltip_GameModeLoading" ), 0.0f );
		break;
	case k_ETimelineGameMode_Menus:
		SteamTimeline()->SetTimelineTooltip( g_pVGuiLocalize->FindAsUTF8( "#GameUI_SteamShare_Tooltip_GameModeMenu" ), 0.0f );
		break;
	}
}

void CSteamShareSystem::PushTimelineGameMode( ETimelineGameMode eMode )
{
	m_GameModeStack.AddToTail( eMode );
	UpdateSteamTimelineGameMode();
}

void CSteamShareSystem::PopTimelineGameMode()
{
	if ( m_GameModeStack.Count() )
	{
		m_GameModeStack.Remove( m_GameModeStack.Count() - 1 );
	}
	UpdateSteamTimelineGameMode();
}

#else // HL2_ANNIVERSARY_NOV2024

bool CSteamShareSystem::Init()
{
	// if we don't have the version of ISteamTimeline we need, don't bother listening for any events
	// and just stub out this whole system.
	if ( !SteamTimeline() )
		return true;

	ListenForGameEvent( "gameui_activate" );
	ListenForGameEvent( "gameui_hide" );
	ListenForGameEvent( "server_spawn" );
	ListenForGameEvent( "single_player_death" );
	ListenForGameEvent( "weapon_equipped" );
	ListenForGameEvent( "ammo_pickup" );
	ListenForGameEvent( "take_health" );
	ListenForGameEvent( "take_armor" );
	ListenForGameEvent( "enter_vehicle" );
	ListenForGameEvent( "leave_vehicle" );
	ListenForGameEvent( "entity_killed" );
	SteamTimeline()->SetTimelineGameMode( k_ETimelineGameMode_Menus );

	return true;
}

void CSteamShareSystem::FireGameEvent( IGameEvent *event )
{
	if ( !V_stricmp( event->GetName(), "gameui_activate" ) )
	{
		SteamTimeline()->SetTimelineGameMode( k_ETimelineGameMode_Menus );
	}
	else if ( !V_stricmp( event->GetName(), "gameui_hide" ) )
	{
		SteamTimeline()->SetTimelineGameMode( k_ETimelineGameMode_Playing );
	}
	else if ( !V_stricmp( event->GetName(), "server_spawn" ) )
	{
		char buf[ 1024 ];
		engine->GetChapterName( buf, sizeof( buf ) );
		const char *pszText = nullptr;
		if ( V_isempty( buf ) )
		{
			pszText = g_pVGuiLocalize->FindAsUTF8( buf );
		}
		else
		{
			pszText = event->GetString( "mapname" );
		}

		if ( V_isempty( pszText ) )
		{
			char buf2[ 1024 ];
			V_strcpy_safe( buf2, pszText );
			SteamTimeline()->SetTimelineStateDescription( buf2, 0 );
		}
	}
	else if ( !V_stricmp( event->GetName(), "single_player_death" ) )
	{
		SteamTimeline()->AddTimelineEvent( "steam_death", "Player was killed", nullptr, 40, 0, 0, k_ETimelineEventClipPriority_None );
	}
	else if ( !V_stricmp( event->GetName(), "weapon_equipped" ) )
	{
		int nWeaponEntindex = event->GetInt( "entindex" );
		IClientEntity *pWeaponEntity = cl_entitylist->GetBaseEntity( nWeaponEntindex );
		if ( pWeaponEntity )
		{
			C_BaseCombatWeapon *pWeapon = dynamic_cast<C_BaseCombatWeapon *>( pWeaponEntity );
			const char *pchWeaponName = "weapon";
			const char *pchIcon = "steam_combat";
			if ( pWeapon )
			{
				const char *pchActualName = g_pVGuiLocalize->FindAsUTF8( pWeapon->GetPrintName() );
				if ( pchActualName )
				{
					pchWeaponName = pchActualName;
				}

				if ( !V_stricmp( "weapon_crowbar", pWeapon->GetName() ) )
				{
					pchIcon = "weapon_crowbar";
				}
				else if ( !V_stricmp( "weapon_pistol", pWeapon->GetName() ) )
				{
					pchIcon = "weapon_pistol";
				}
				else if ( !V_stricmp( "weapon_smg1", pWeapon->GetName() ) || !V_stricmp( "weapon_smg2", pWeapon->GetName() ) )
				{
					pchIcon = "weapon_smg";
				}
			}

			SteamTimeline()->AddTimelineEvent( pchIcon, CFmtStr( "Picked up %s", pchWeaponName ), nullptr, 50, 0, 0, k_ETimelineEventClipPriority_Standard );
		}
	}
	else if ( !V_stricmp( event->GetName(), "ammo_pickup" ) )
	{
		const Ammo_t *pAmmo = GetAmmoDef()->GetAmmoOfIndex( event->GetInt( "ammo_index" ) );
		if ( pAmmo )
		{
			const char *pchIcon = "steam_chest";
			if ( !V_stricmp( pAmmo->pName, "pistol" ) )
			{
				pchIcon = "ammo_pistol";
			}
			else if ( !V_stricmp( pAmmo->pName, "smg1" ) || !V_stricmp( pAmmo->pName, "smg2" ) )
			{
				pchIcon = "ammo_smg";
			}

			SteamTimeline()->AddTimelineEvent( pchIcon, CFmtStr( "Picked up %d %s ammo", event->GetInt( "amount" ), pAmmo->pName ), CFmtStr( "%d now available", event->GetInt( "total" ) ), 0, 0, 0, k_ETimelineEventClipPriority_None );
		}
	}
	else if ( !V_stricmp( event->GetName(), "take_health" ) )
	{
		int nAmount = event->GetInt( "amount" );
		int nTotal = event->GetInt( "total" );
		uint32 unPriority = ( ( nAmount > 10 ) | ( nTotal == 100 ) ) ? 3 : 0;
		SteamTimeline()->AddTimelineEvent( "steam_heart", CFmtStr( "Picked up %d health", nAmount ), CFmtStr( "Health is now %d", nTotal ), unPriority, 0, 0, k_ETimelineEventClipPriority_None );
	}
	else if ( !V_stricmp( event->GetName(), "take_armor" ) )
	{
		int nAmount = event->GetInt( "amount" );
		int nTotal = event->GetInt( "total" );
		uint32 unPriority = ( ( nAmount > 10 ) | ( nTotal == 100 ) ) ? 3 : 0;
		SteamTimeline()->AddTimelineEvent( "steam_bolt", CFmtStr( "Picked up %d energy", nAmount ), CFmtStr( "Energy is now %d", nTotal ), unPriority, 0, 0, k_ETimelineEventClipPriority_None );
	}
	else if ( !V_stricmp( event->GetName(), "entity_killed" ) )
	{
		C_AI_BaseNPC *pKilledEntity = dynamic_cast<C_AI_BaseNPC *>( cl_entitylist->GetBaseEntity( event->GetInt( "entindex_killed", 0 ) ) );
		if ( pKilledEntity )
		{
			const char *pchClassName = pKilledEntity->GetClassname();
			if ( !V_stricmp( "class C_BaseHelicopter", pchClassName ) )
			{
				SteamTimeline()->AddTimelineEvent( "enemy_chopper", "Killed attack chopper", nullptr, 200, -30.f, 35.f, k_ETimelineEventClipPriority_Featured );
			}

			C_BaseEntity *pInflictor = cl_entitylist->GetBaseEntity( event->GetInt( "entindex_inflictor", 0 ) );

			const char *pchModelName = pInflictor ? modelinfo->GetModelName( modelinfo->GetModel( pInflictor->GetModelIndex() ) ) : "";
			if ( nullptr != V_stristr( pchModelName, "sawblade" ) )
			{
				SteamTimeline()->AddTimelineEvent( "sawblade_kill", "Chopped something up with a sawblade", nullptr, 50, 0, 0, k_ETimelineEventClipPriority_None );
			}
		}
	}
}

#endif // HL2_ANNIVERSARY_NOV2024
