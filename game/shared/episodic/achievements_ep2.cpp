//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================


#include "cbase.h"
#include <time.h>
#ifdef GAME_DLL

#include "achievementmgr.h"
#include "baseachievement.h"
#include "iservervehicle.h"
#include "npc_antlion.h"
#include "npc_hunter.h"


class CAchievementEp2KillPoisonAntlion : public CBaseAchievement
{
protected:

	virtual void Init()
	{
		SetVictimFilter( "npc_antlion" );
		SetFlags( ACH_LISTEN_PLAYER_KILL_ENEMY_EVENTS | ACH_SAVE_WITH_GAME );
		SetGameDirFilter( "ep2" );
		SetGoal( 1 );
	}

	virtual void Event_EntityKilled( CBaseEntity *pVictim, CBaseEntity *pAttacker, CBaseEntity *pInflictor, IGameEvent *event ) 
	{
		CNPC_Antlion *pAntlion = dynamic_cast<CNPC_Antlion *>( pVictim );
		if ( pAntlion && pAntlion->IsWorker()  )
		{
			IncrementCount();
		}
	}
};
DECLARE_ACHIEVEMENT_( CAchievementEp2KillPoisonAntlion, ACHIEVEMENT_EP2_KILL_POISONANTLION, "EP2_KILL_POISONANTLION", "ep2", 5, false );

class CAchievementEp2KillAllGrubs : public CBaseAchievement
{
protected:

	virtual void Init()
	{
		SetVictimFilter( "npc_antlion_grub" );
		SetFlags( ACH_LISTEN_KILL_EVENTS | ACH_SAVE_WITH_GAME );
		SetGameDirFilter( "ep2" );
		SetGoal( 333 );
	}
};
DECLARE_ACHIEVEMENT_( CAchievementEp2KillAllGrubs, ACHIEVEMENT_EP2_KILL_ALLGRUBS, "EP2_KILL_ALLGRUBS", "ep2", 20, false );

class CAchievementEp2KillEnemiesWithCar : public CBaseAchievement
{
protected:

	virtual void Init() 
	{
		SetFlags( ACH_LISTEN_KILL_ENEMY_EVENTS | ACH_SAVE_WITH_GAME );
		SetInflictorFilter( "prop_vehicle_jeep" );
		SetGameDirFilter( "ep2" );
		SetGoal( 20 );
	}

	virtual void Event_EntityKilled( CBaseEntity *pVictim, CBaseEntity *pAttacker, CBaseEntity *pInflictor, IGameEvent *event ) 
	{
		// count this if the object is a driveable vehicle and local player is driving
		CBasePlayer *pLocalPlayer = UTIL_GetLocalPlayer();
		IDrivableVehicle *pDriveableVehicle = dynamic_cast<IDrivableVehicle *>( pInflictor );
		if ( pLocalPlayer && pDriveableVehicle && pDriveableVehicle->GetDriver() == pLocalPlayer )
		{
			IncrementCount();
		}
	}
};
DECLARE_ACHIEVEMENT_( CAchievementEp2KillEnemiesWithCar, ACHIEVEMENT_EP2_KILL_ENEMIES_WITHCAR, "EP2_KILL_ENEMIES_WITHCAR", "ep2", 5, false );

class CAchievementEp2KillHunterWithFlechette : public CBaseAchievement
{
protected:
	void Init() 
	{
		SetFlags( ACH_LISTEN_KILL_EVENTS | ACH_SAVE_WITH_GAME );
		SetVictimFilter( "npc_hunter" );
		SetInflictorFilter( "hunter_flechette" );
		SetGameDirFilter( "ep2" );
		SetGoal( 1 );
	}
};
DECLARE_ACHIEVEMENT_( CAchievementEp2KillHunterWithFlechette, ACHIEVEMENT_EP2_KILL_HUNTER_WITHFLECHETTES, "EP2_KILL_HUNTER_WITHFLECHETTES", "ep2", 10, false );

class CAchievementEp2FindAllWebCaches : public CBaseAchievement
{
	virtual void Init()
	{
		static const char *szComponents[] =
		{
			"EP2_WEBCACHE_01", "EP2_WEBCACHE_02", "EP2_WEBCACHE_03", "EP2_WEBCACHE_04",
			"EP2_WEBCACHE_05", "EP2_WEBCACHE_06", "EP2_WEBCACHE_07", "EP2_WEBCACHE_08", "EP2_WEBCACHE_09"
		};		
		SetFlags( ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL );
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE( szComponents );
		SetComponentPrefix( "EP2_WEBCACHE" );
		SetGameDirFilter( "ep2" );
		SetGoal( m_iNumComponents );
	}

	// don't show progress notifications for this achievement, it's distracting
	virtual bool ShouldShowProgressNotification() { return false; }
};
DECLARE_ACHIEVEMENT_( CAchievementEp2FindAllWebCaches, ACHIEVEMENT_EP2_BREAK_ALLWEBS, "EP2_BREAK_ALLWEBS", "ep2", 5, false );

class CAchievementEp2KillChopperNoMisses: public CFailableAchievement
{
protected:

	void Init() 
	{
		SetFlags( ACH_LISTEN_MAP_EVENTS | ACH_SAVE_WITH_GAME );
		SetGameDirFilter( "ep2" );
		SetGoal( 1 );
	}

	virtual void ListenForEvents()
	{
		ListenForGameEvent( "helicopter_grenade_punt_miss" );
	}

	// map event where achievement is activated
	virtual const char *GetActivationEventName() { return "EP2_KILL_CHOPPER_NOMISSES_START"; }
	// map event where achievement is evaluated for success
	virtual const char *GetEvaluationEventName() { return "EP2_KILL_CHOPPER_NOMISSES_END"; }

	void FireGameEvent_Internal( IGameEvent *event )
	{
		if ( 0 == Q_strcmp( event->GetName(), "helicopter_grenade_punt_miss" ) )
		{
			SetFailed();
		}
	}
};
DECLARE_ACHIEVEMENT_( CAchievementEp2KillChopperNoMisses, ACHIEVEMENT_EP2_KILL_CHOPPER_NOMISSES, "EP2_KILL_CHOPPER_NOMISSES", "ep2", 15, false );

class CAchievementEp2FindAllRadarCaches : public CBaseAchievement
{
	virtual void Init()
	{
		static const char *szComponents[] =
		{
			"EP2_RADARCACHE_VAN", "EP2_RADARCACHE_SHACK", "EP2_RADARCACHE_RPG", "EP2_RADARCACHE_CAVE", "EP2_RADARCACHE_HANGING"
		};		
		SetFlags( ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL );
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE( szComponents );
		SetComponentPrefix( "EP2_RADARCACHE" );
		SetGameDirFilter( "ep2" );
		SetGoal( m_iNumComponents );
	}
};
DECLARE_ACHIEVEMENT_( CAchievementEp2FindAllRadarCaches, ACHIEVEMENT_EP2_FIND_ALLRADARCACHES, "EP2_FIND_ALLRADARCACHES", "ep2", 10, false );

// Ep2-specific macro that sets game dir filter.  We need this because Ep1/Ep2/... share a binary so we need runtime check against running game.
#define DECLARE_EP2_MAP_EVENT_ACHIEVEMENT( achievementID, achievementName, iPointValue )					\
	DECLARE_MAP_EVENT_ACHIEVEMENT_( achievementID, achievementName, "ep2", iPointValue, false )

#define DECLARE_EP2_MAP_EVENT_ACHIEVEMENT_HIDDEN( achievementID, achievementName, iPointValue )					\
	DECLARE_MAP_EVENT_ACHIEVEMENT_( achievementID, achievementName, "ep2", iPointValue, true )

// achievements which are won by a map event firing once
DECLARE_EP2_MAP_EVENT_ACHIEVEMENT( ACHIEVEMENT_EP2_BEAT_ANTLIONINVASION, "EP2_BEAT_ANTLIONINVASION", 5 );
DECLARE_EP2_MAP_EVENT_ACHIEVEMENT_HIDDEN( ACHIEVEMENT_EP2_BEAT_ANTLIONGUARDS, "EP2_BEAT_ANTLIONGUARDS", 5 );
DECLARE_EP2_MAP_EVENT_ACHIEVEMENT_HIDDEN( ACHIEVEMENT_EP2_BEAT_HUNTERAMBUSH, "EP2_BEAT_HUNTERAMBUSH", 10 );
DECLARE_EP2_MAP_EVENT_ACHIEVEMENT_HIDDEN( ACHIEVEMENT_EP2_KILL_COMBINECANNON, "EP2_KILL_COMBINECANNON", 5 );
DECLARE_EP2_MAP_EVENT_ACHIEVEMENT( ACHIEVEMENT_EP2_BEAT_RACEWITHDOG, "EP2_BEAT_RACEWITHDOG", 5 );
DECLARE_EP2_MAP_EVENT_ACHIEVEMENT( ACHIEVEMENT_EP2_BEAT_ROCKETCACHEPUZZLE, "EP2_BEAT_ROCKETCACHEPUZZLE", 5 );
DECLARE_EP2_MAP_EVENT_ACHIEVEMENT_HIDDEN( ACHIEVEMENT_EP2_BEAT_WHITEFORESTINN, "EP2_BEAT_WHITEFORESTINN", 10 );
DECLARE_EP2_MAP_EVENT_ACHIEVEMENT( ACHIEVEMENT_EP2_PUT_ITEMINROCKET, "EP2_PUT_ITEMINROCKET", 30 );
DECLARE_EP2_MAP_EVENT_ACHIEVEMENT_HIDDEN( ACHIEVEMENT_EP2_BEAT_MISSILESILO2, "EP2_BEAT_MISSILESILO2", 5 );
DECLARE_EP2_MAP_EVENT_ACHIEVEMENT( ACHIEVEMENT_EP2_BEAT_OUTLAND12_NOBUILDINGSDESTROYED, "EP2_BEAT_OUTLAND12_NOBUILDINGSDESTROYED", 35 );
DECLARE_EP2_MAP_EVENT_ACHIEVEMENT_HIDDEN( ACHIEVEMENT_EP2_BEAT_GAME, "EP2_BEAT_GAME", 20 );

//-----------------------------------------------------------------------------
// Purpose: Anniversary achievement, earned by playing on or after Nov 20 2020
//-----------------------------------------------------------------------------
class CAchievementGlobalGnomeAlone : public CBaseAchievement
{
	virtual void Init()
	{
		SetGoal( 1 );
	}

	virtual void ListenForEvents()
	{
		ListenForGameEvent( "game_newmap" );
	}

	virtual void PostRestoreSavedGame()
	{
		if ( !IsAchieved() )
		{
			CheckDate();
		}
	}

protected:
	virtual void FireGameEvent_Internal( IGameEvent *event )
	{
		if ( !IsAchieved() )
		{
			CheckDate();
		}
	}

private:
	void CheckDate()
	{
		if ( IsAchieved() )
			return;

		time_t now;
		time( &now );
		if ( now > 1605850200 )
		{
			IncrementCount();
		}
	}
};
DECLARE_ACHIEVEMENT_( CAchievementGlobalGnomeAlone, ACHIEVEMENT_GLOBAL_GNOME_ALONE, "GLOBAL_GNOME_ALONE", "ep2", 1, false );

#endif // GAME_DLL