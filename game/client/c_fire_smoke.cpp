//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "iviewrender.h"
#include "clienteffectprecachesystem.h"
#include "studio.h"
#include "bone_setup.h"
#include "engine/ivmodelinfo.h"
#include "c_fire_smoke.h"
#include "engine/IEngineSound.h"
#include "iefx.h"
#include "dlight.h"
#include "tier0/icommandline.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


CLIENTEFFECT_REGISTER_BEGIN( SmokeStackMaterials )
	CLIENTEFFECT_MATERIAL( "particle/SmokeStack" )
	CLIENTEFFECT_MATERIAL( "particle/fire" )
	CLIENTEFFECT_MATERIAL( "sprites/flamefromabove" )
	CLIENTEFFECT_MATERIAL( "sprites/flamelet1" )
	CLIENTEFFECT_MATERIAL( "sprites/flamelet2" )
	CLIENTEFFECT_MATERIAL( "sprites/flamelet3" )
	CLIENTEFFECT_MATERIAL( "sprites/flamelet4" )
	CLIENTEFFECT_MATERIAL( "sprites/flamelet5" )
	CLIENTEFFECT_MATERIAL( "sprites/fire1" )
CLIENTEFFECT_REGISTER_END()

ConVar r_classic_fire( "r_classic_fire", NOV2024_ONLY( "0", FCVAR_ARCHIVE | FCVAR_REPLICATED, "Use Classic Effects for Fire (Disabled/Classic/Release)" ) OCT2024_ONLY( "2", FCVAR_REPLICATED, "Use Classic Effects for Fire (New/Classic/Release)" ) );

//-----------------------------------------------------------------------------
// Purpose: 0 uses the particle fire, 1 the classic sprite fire, 2 the classic
//			fire everywhere except in the episodes
//-----------------------------------------------------------------------------
bool BIsCurrentFireTypeClassic()
{
	int nFireType = r_classic_fire.GetInt();
	if ( nFireType == 2 )
		return !IsHL2Episodic();

	return nFireType == 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pRecvProp - 
//			*pStruct - 
//			*pVarData - 
//			*pIn - 
//			objectID - 
//-----------------------------------------------------------------------------
void RecvProxy_Scale( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	C_FireSmoke	*pFireSmoke	= (C_FireSmoke	*) pStruct;
	float scale				= pData->m_Value.m_Float;

	//If changed, update our internal information
	if ( ( pFireSmoke->m_flScale != scale ) && ( pFireSmoke->m_flScaleEnd != scale ) )
	{
		pFireSmoke->m_flScaleStart		= pFireSmoke->m_flScaleRegister;
		pFireSmoke->m_flScaleEnd		= scale;			
	}

	pFireSmoke->m_flScale = scale;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pRecvProp - 
//			*pStruct - 
//			*pVarData - 
//			*pIn - 
//			objectID - 
//-----------------------------------------------------------------------------
void RecvProxy_ScaleTime( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	C_FireSmoke	*pFireSmoke	= (C_FireSmoke	*) pStruct;
	float time				= pData->m_Value.m_Float;

	//If changed, update our internal information
	//if ( pFireSmoke->m_flScaleTime != time )
	{
		if ( time == -1.0f )
		{
			pFireSmoke->m_flScaleTimeStart	= Helper_GetTime()-1.0f;
			pFireSmoke->m_flScaleTimeEnd	= pFireSmoke->m_flScaleTimeStart;
		}
		else
		{
			pFireSmoke->m_flScaleTimeStart	= Helper_GetTime();
			pFireSmoke->m_flScaleTimeEnd	= Helper_GetTime() + time;
		}
	}

	pFireSmoke->m_flScaleTime = time;
}

//Receive datatable
IMPLEMENT_CLIENTCLASS_DT( C_FireSmoke, DT_FireSmoke, CFireSmoke )
	RecvPropFloat( RECVINFO( m_flStartScale )),
	NOV2024_ONLY( RecvPropFloat( RECVINFO( m_flOriginalStartScale )), )
	RecvPropFloat( RECVINFO( m_flScale ), 0, RecvProxy_Scale ),
	RecvPropFloat( RECVINFO( m_flScaleTime ), 0, RecvProxy_ScaleTime ),
	RecvPropInt( RECVINFO( m_nFlags ) ),
	RecvPropInt( RECVINFO( m_nFlameModelIndex ) ),
	RecvPropInt( RECVINFO( m_nFlameFromAboveModelIndex ) ),
END_RECV_TABLE()

//==================================================
// C_FireSmoke
//==================================================

C_FireSmoke::C_FireSmoke()
{
	//Server-side
	m_flStartScale			= 0.0f;
	m_flOriginalStartScale	= 0.0f;
	m_flScale				= 0.0f;
	m_flScaleTime			= 0.0f;
	m_nFlags				= bitsFIRESMOKE_NONE;
	m_nFlameModelIndex		= 0;
	m_nFlameFromAboveModelIndex = 0;

	//Client-side
	m_flScaleRegister		= 0.0f;
	m_flScaleStart			= 0.0f;
	m_flScaleEnd			= 0.0f;
	m_flScaleTimeStart		= 0.0f;
	m_flScaleTimeEnd		= 0.0f;
	m_bClipTested			= false;
	m_flChildFlameSpread	= FLAME_CHILD_SPREAD;

	for ( int i = 0; i < NUM_CHILD_FLAMES; i++ )
	{
		m_entFlames[i].Clear();
		m_entFlamesFromAbove[i].Clear();
	}

	m_pSmokeEmitter = NULL;
	m_bClassicFire = BIsCurrentFireTypeClassic();
}

C_FireSmoke::~C_FireSmoke()
{
	if ( m_pFireOverlay != NULL )
	{
		delete m_pFireOverlay;
		m_pFireOverlay = NULL;
	}

	// Shut down our effect if we have it
	ParticleProp()->StopEmissionAndDestroyImmediately( m_hEffect );
}

//-----------------------------------------------------------------------------
// Purpose: FIXME: what's the right way to do this?
//-----------------------------------------------------------------------------
void C_FireSmoke::StartClientOnly( void )
{
	Start();

	ClientEntityList().AddNonNetworkableEntity(	this );
	CollisionProp()->CreatePartitionHandle();
	AddEffects( EF_NORECEIVESHADOW | EF_NOSHADOW );
	AddToLeafSystem();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_FireSmoke::RemoveClientOnly(void)
{
	ClientThinkList()->RemoveThinkable( GetClientHandle() );

	// Remove from the client entity list.
	ClientEntityList().RemoveEntity( GetClientHandle() );

	partition->Remove( PARTITION_CLIENT_SOLID_EDICTS | PARTITION_CLIENT_RESPONSIVE_EDICTS | PARTITION_CLIENT_NON_STATIC_EDICTS, CollisionProp()->GetPartitionHandle() );

	RemoveFromLeafSystem();

	CollisionProp()->DestroyPartitionHandle();
	ParticleProp()->StopEmissionAndDestroyImmediately( m_hEffect );
}

//-----------------------------------------------------------------------------
// Purpose: Restart the fire with the effect type r_classic_fire now asks for
//-----------------------------------------------------------------------------
void C_FireSmoke::ChangeFireEffectType( void )
{
	m_bClassicFire = BIsCurrentFireTypeClassic();

	RemoveClientOnly();
	StartClientOnly();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_FireSmoke::Simulate( void )
{
	if ( m_bClassicFire != BIsCurrentFireTypeClassic() )
	{
		ChangeFireEffectType();
	}

	if ( !ShouldDraw() )
	{
		for ( int i = 0; i < NUM_CHILD_FLAMES; i++ )
		{
			m_entFlames[i].SetRenderColor( 0, 0, 0, 0 );
			m_entFlames[i].SetBrightness( 0 );
		}

		if ( m_nFlags & bitsFIRESMOKE_VISIBLE_FROM_ABOVE )
		{
			for ( int i = 0; i < NUM_CHILD_FLAMES; i++ )
			{
				m_entFlamesFromAbove[i].SetRenderColor( 0, 0, 0, 0 );
				m_entFlamesFromAbove[i].SetBrightness( 0 );
			}
		}

		m_nFlags &= ~bitsFIRESMOKE_SMOKE;
	}

#ifdef HL2_ANNIVERSARY_NOV2024
	if ( !BIsCurrentFireTypeClassic() )
	{
		float flScaleTime = m_flScaleTimeEnd - m_flScaleTimeStart;
		float flTimeLeft = m_flScaleTimeEnd - Helper_GetTime();

		// Stop emitting once the fire has almost finished shrinking away
		if ( m_hEffect && flScaleTime > 0.0f && m_flScaleEnd == 0.0f && flTimeLeft <= 1.5f )
		{
			m_hEffect->StopEmission( false, false, true );
			m_hEffect = NULL;
		}
	}
	else
#else
	if ( BIsCurrentFireTypeClassic() )
#endif
	{
		//Only do this if we're active
		if ( m_nFlags & bitsFIRESMOKE_ACTIVE )
		{
			Update();
			AddFlames();
		}
	}
#line 257
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline void C_FireSmoke::UpdateEffects( void )
{
	if ( !BIsCurrentFireTypeClassic() )
		return;

	if ( m_pSmokeEmitter.IsValid() )
	{
		m_pSmokeEmitter->SetSortOrigin( GetAbsOrigin() );
	}

	if ( m_pFireOverlay != NULL )
	{
		m_pFireOverlay->m_vPos = GetAbsOrigin();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline void C_FireSmoke::UpdateScale( void )
{
	if ( !BIsCurrentFireTypeClassic() )
		return;

	float	time = Helper_GetTime();

	if ( m_flScaleRegister != m_flScaleEnd )
	{
		//See if we're done scaling
		if ( time > m_flScaleTimeEnd )
		{
			m_flScaleRegister = m_flStartScale = m_flScaleEnd;
		}
		else
		{
			//Lerp the scale and set it 
			float	timeFraction = 1.0f - ( m_flScaleTimeEnd - time ) / ( m_flScaleTimeEnd - m_flScaleTimeStart );
			float	newScale = m_flScaleStart + ( ( m_flScaleEnd - m_flScaleStart ) * timeFraction );

			m_flScaleRegister = m_flStartScale = newScale;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_FireSmoke::Update( void )
{
	if ( !BIsCurrentFireTypeClassic() )
		return;

	//If we haven't already, find the clip plane for smoke effects
	if ( ( m_nFlags & bitsFIRESMOKE_SMOKE ) && ( m_bClipTested == false ) )
	{
		FindClipPlane();
	}

	//Update all our parts
	UpdateEffects();
	UpdateScale();
	UpdateAnimation();
	UpdateFlames();

	//See if we should emit smoke
	if ( m_nFlags & bitsFIRESMOKE_SMOKE )
	{
		float tempDelta = Helper_GetFrameTime();

		while( m_tParticleSpawn.NextEvent( tempDelta ) )
		{
			SpawnSmoke();
		}
	}
}

#define	FLAME_ALPHA_START	0.9f
#define FLAME_ALPHA_END		1.0f

#define	FLAME_TRANS_START	0.75f

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_FireSmoke::AddFlames( void )
{
	if ( !BIsCurrentFireTypeClassic() )
		return;

	for ( int i = 0; i < NUM_CHILD_FLAMES; i++ )
	{
		if ( m_entFlames[i].GetScale() > 1e-3f )
		{
			m_entFlames[i].SetRenderColor( 255, 255, 255 );
			m_entFlames[i].SetBrightness( 255 );
			m_entFlames[i].AddToLeafSystem();
		}
	}

	if ( m_nFlags & bitsFIRESMOKE_VISIBLE_FROM_ABOVE )
	{
		for ( int i = 0; i < NUM_CHILD_FLAMES; i++ )
		{
			if ( m_entFlamesFromAbove[i].GetScale() > 1e-3f )
			{
				m_entFlamesFromAbove[i].SetRenderColor( 255, 255, 255 );
				m_entFlamesFromAbove[i].SetBrightness( 255 );
				m_entFlamesFromAbove[i].AddToLeafSystem();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : bnewentity - 
//-----------------------------------------------------------------------------
void C_FireSmoke::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	UpdateEffects();

	if ( updateType == DATA_UPDATE_CREATED )
	{
		m_bClassicFire = BIsCurrentFireTypeClassic();
		Start();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_FireSmoke::Start( void )
{
	if ( m_flOriginalStartScale == 0.0f )
	{
		m_flOriginalStartScale = m_flStartScale;
	}

	if ( BIsCurrentFireTypeClassic() )
	{
		// Setup the flame sprites
		bool bTools = CommandLine()->CheckParm( "-tools" ) != NULL;

		int i;
		for ( i = 0; i < NUM_CHILD_FLAMES; i++ )
		{
			if ( bTools )
			{
				ClientEntityList().AddNonNetworkableEntity(	&m_entFlames[i] );
			}
			m_entFlames[i].AddToLeafSystem( RENDER_GROUP_TRANSLUCENT_ENTITY );
		}

		if ( m_nFlags & bitsFIRESMOKE_VISIBLE_FROM_ABOVE )
		{
			for ( i = 0; i < NUM_CHILD_FLAMES; i++ )
			{
				if ( bTools )
				{
					ClientEntityList().AddNonNetworkableEntity(	&m_entFlamesFromAbove[i] );
				}
				m_entFlamesFromAbove[i].AddToLeafSystem( RENDER_GROUP_TRANSLUCENT_ENTITY );
			}
		}

		//Various setup info
		m_tParticleSpawn.Init( 2.0f );

		QAngle	offset;
		int		maxFrames;

		//Setup the child flames
		for ( i = 0; i < NUM_CHILD_FLAMES; i++ )
		{
			//Setup our offset angles
			offset[0] = 0.0f;
			offset[1] = Helper_RandomFloat( 0, 360 );
			offset[2] = 0.0f;

			AngleVectors( offset, &m_entFlames[i].m_vecMoveDir );
			m_entFlames[i].m_bFadeFromAbove = ( m_nFlags & bitsFIRESMOKE_VISIBLE_FROM_ABOVE );

			model_t *pModel = (model_t *) modelinfo->GetModel( m_nFlameModelIndex );
			maxFrames = modelinfo->GetModelFrameCount( pModel );

			//Setup all the information for the client entity
			m_entFlames[i].SetModelByIndex( m_nFlameModelIndex );
			m_entFlames[i].SetLocalOrigin( GetLocalOrigin() );
			m_entFlames[i].m_flFrame			= Helper_RandomInt( 0, maxFrames - 1 );
			m_entFlames[i].m_flSpriteFramerate	= (float) Helper_RandomInt( 15, 30 );
			m_entFlames[i].SetScale( m_flStartScale );
			m_entFlames[i].SetRenderMode( kRenderTransAddFrameBlend );
			m_entFlames[i].m_nRenderFX			= kRenderFxNone;
			m_entFlames[i].SetRenderColor( 255, 255, 255, 255 );
			m_entFlames[i].SetBrightness( 255 );
			m_entFlames[i].AddEffects( EF_NORECEIVESHADOW | EF_NOSHADOW );

			m_entFlames[i].index				= -1;

			if ( i == 0 )
			{
				m_entFlameScales[i] = 1.0f;
			}
			else
			{
				//Keep a scale offset
				m_entFlameScales[i] = random->RandomFloat( 0.5f, 1.0f );
			}
		}

		if ( m_nFlags & bitsFIRESMOKE_VISIBLE_FROM_ABOVE )
		{
			for ( i = 0; i < NUM_CHILD_FLAMES; i++ )
			{
				model_t *pModel = (model_t *) modelinfo->GetModel( m_nFlameFromAboveModelIndex );
				maxFrames = modelinfo->GetModelFrameCount( pModel );

				//Setup all the information for the client entity
				m_entFlamesFromAbove[i].SetModelByIndex( m_nFlameFromAboveModelIndex );
				m_entFlamesFromAbove[i].SetLocalOrigin( GetLocalOrigin() );
				m_entFlamesFromAbove[i].m_flFrame			= Helper_RandomInt( 0, maxFrames - 1 );
				m_entFlamesFromAbove[i].m_flSpriteFramerate	= (float) Helper_RandomInt( 15, 30 );
				m_entFlamesFromAbove[i].SetScale( m_flStartScale );
				m_entFlamesFromAbove[i].SetRenderMode( kRenderTransAddFrameBlend );
				m_entFlamesFromAbove[i].m_nRenderFX			= kRenderFxNone;
				m_entFlamesFromAbove[i].SetRenderColor( 255, 255, 255, 255 );
				m_entFlamesFromAbove[i].SetBrightness( 255 );
				m_entFlamesFromAbove[i].AddEffects( EF_NORECEIVESHADOW | EF_NOSHADOW );

				m_entFlamesFromAbove[i].index				= -1;

				if ( i == 0 )
				{
					m_entFlameScales[i] = 1.0f;
				}
				else
				{
					//Keep a scale offset
					m_entFlameScales[i] = random->RandomFloat( 0.5f, 1.0f );
				}
			}
		}

		//Only make the smoke if we've requested it
		if ( m_nFlags & bitsFIRESMOKE_SMOKE )
		{
			m_pSmokeEmitter = CLitSmokeEmitter::Create( "C_FireSmoke::m_pSmokeEmitter" );
			m_pSmokeEmitter->Init( "particle/SmokeStack", GetAbsOrigin() );
		}
	}
	else
	{
		if ( !m_hEffect )
		{
			const char *lpszEffectName;
			int nSize = (int) floor( m_flOriginalStartScale / 36.0f );
			switch ( nSize )
			{
			case 0:
				lpszEffectName = ( m_nFlags & bitsFIRESMOKE_SMOKE ) ? "env_fire_tiny_smoke" : "env_fire_tiny";
				break;

			case 1:
				lpszEffectName = ( m_nFlags & bitsFIRESMOKE_SMOKE ) ? "env_fire_small_smoke" : "env_fire_small";
				break;

			case 2:
				lpszEffectName = ( m_nFlags & bitsFIRESMOKE_SMOKE ) ? "env_fire_medium_smoke" : "env_fire_medium";
				break;

			case 3:
			default:
				lpszEffectName = ( m_nFlags & bitsFIRESMOKE_SMOKE ) ? "env_fire_large_smoke" : "env_fire_large";
				break;
			}

			// Create the effect of the correct size
			m_hEffect = ParticleProp()->Create( lpszEffectName, PATTACH_ABSORIGIN );
			m_hEffect->SetControlPoint( 0, GetAbsOrigin() );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool C_FireSmoke::ShouldDraw()
{
	C_BaseEntity *pOwner = GetOwnerEntity();
	if ( pOwner && pOwner->GetRenderColor().a == 0 )
	{
		// Don't draw if our owner is invisible
		return false;
	}

	return BIsCurrentFireTypeClassic();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_FireSmoke::UpdateAnimation( void )
{
	if ( !BIsCurrentFireTypeClassic() )
		return;

	int		numFrames;
	float	frametime = Helper_GetFrameTime();

	for ( int i = 0; i < NUM_CHILD_FLAMES; i++ )
	{
		m_entFlames[i].m_flFrame += m_entFlames[i].m_flSpriteFramerate * frametime;

		numFrames = modelinfo->GetModelFrameCount( m_entFlames[i].GetModel() );

		if ( m_entFlames[i].m_flFrame >= numFrames )
		{
			m_entFlames[i].m_flFrame = m_entFlames[i].m_flFrame - (int)(m_entFlames[i].m_flFrame);
		}
	}

	if ( m_nFlags & bitsFIRESMOKE_VISIBLE_FROM_ABOVE )
	{
		for ( int i = 0; i < NUM_CHILD_FLAMES; i++ )
		{
			m_entFlamesFromAbove[i].m_flFrame += m_entFlamesFromAbove[i].m_flSpriteFramerate * frametime;

			numFrames = modelinfo->GetModelFrameCount( m_entFlamesFromAbove[i].GetModel() );

			if ( m_entFlamesFromAbove[i].m_flFrame >= numFrames )
			{
				m_entFlamesFromAbove[i].m_flFrame = m_entFlamesFromAbove[i].m_flFrame - (int)(m_entFlamesFromAbove[i].m_flFrame);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_FireSmoke::UpdateFlames( void )
{
	if ( !BIsCurrentFireTypeClassic() )
		return;

	for ( int i = 0; i < NUM_CHILD_FLAMES; i++ )
	{
		float	newScale = m_flScaleRegister * m_entFlameScales[i];

		Vector	dir;
		dir[2] = 0.0f;
		VectorNormalize( dir );

		Vector	offset = GetAbsOrigin();
		offset[2] += m_entFlames[i].GetScale() * FLAME_SOURCE_HEIGHT;

		if ( !m_bFadingOut )
		{
			// Note: Sprite render assumes 0 scale means 1.0
			m_entFlames[i].SetScale( clamp( newScale, 0.000001f, 1.0f ) );
		}
		else
		{
			m_entFlames[i].SetScale( newScale );
		}

		if ( i != 0 )
		{
			m_entFlames[i].SetLocalOrigin( offset + ( m_entFlames[i].m_vecMoveDir * ( m_entFlames[i].GetScale() * m_flChildFlameSpread ) ) );
		}
		else
		{
			m_entFlames[i].SetLocalOrigin( offset );
		}
	}

	if ( m_nFlags & bitsFIRESMOKE_VISIBLE_FROM_ABOVE )
	{
		for ( int i = 0; i < NUM_CHILD_FLAMES; i++ )
		{
			float	newScale = m_flScaleRegister * m_entFlameScales[i];

			Vector	dir;
			dir[2] = 0.0f;
			VectorNormalize( dir );

			Vector	offset = GetAbsOrigin();
			offset[2] += m_entFlamesFromAbove[i].GetScale() * FLAME_FROM_ABOVE_SOURCE_HEIGHT;

			if ( !m_bFadingOut )
			{
				// Note: Sprite render assumes 0 scale means 1.0
				m_entFlamesFromAbove[i].SetScale( MAX( 0.000001, newScale ) );
			}
			else
			{
				m_entFlamesFromAbove[i].SetScale( newScale );
			}

			if ( i != 0 )
			{
				m_entFlamesFromAbove[i].SetLocalOrigin( offset + ( m_entFlames[i].m_vecMoveDir * ( m_entFlamesFromAbove[i].GetScale() * m_flChildFlameSpread ) ) );
			}
			else
			{
				m_entFlamesFromAbove[i].SetLocalOrigin( offset );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_FireSmoke::FindClipPlane( void )
{
	m_bClipTested = true;
	m_flClipPerc = 1.0f;

	trace_t	tr;
	Vector	end( 0.0f, 0.0f, SMOKE_RISE_RATE*SMOKE_LIFETIME );

	UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin()+end, MASK_SOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &tr );

	//If the trace hit something, setup our clip plane
	if ( tr.fraction < 0.5f )
	{
		m_nFlags &= ~bitsFIRESMOKE_SMOKE;
	}
	else if ( tr.fraction < 1.0f )
	{
		m_planeClip.Init( tr.plane.normal, tr.plane.dist );
		m_nFlags |= bitsFIRESMOKE_SMOKE_COLLISION;
		m_flClipPerc = tr.fraction * 0.5f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Spawn smoke (...duh)
//-----------------------------------------------------------------------------

void C_FireSmoke::SpawnSmoke( void )
{
	if ( m_pSmokeEmitter.IsValid() == false )
		return;

	float	scalar = m_flScaleRegister * 32.0f;
	Vector	offset;

	offset[0] = Helper_RandomFloat( -scalar, scalar );
	offset[1] = Helper_RandomFloat( -scalar, scalar );
	offset[2] = scalar + Helper_RandomFloat( -0.25f * scalar, 0.25f * scalar );
	offset[2] += 100.0f;

	m_pSmokeEmitter->SetDirectionalLight( GetAbsOrigin(), Vector( 1.0f, 0.5f, 0.2f ), 2500 );

	CLitSmokeEmitter::LitSmokeParticle *pParticle = (CLitSmokeEmitter::LitSmokeParticle *) m_pSmokeEmitter->AddParticle( sizeof( CLitSmokeEmitter::LitSmokeParticle ), m_pSmokeEmitter->GetSmokeMaterial(), offset + GetAbsOrigin() );

	if ( pParticle == NULL )
		return;

	float flDieTime = m_flClipPerc * SMOKE_LIFETIME;
	pParticle->m_flLifetime	= 0.0f;
	pParticle->m_flDieTime	= random->RandomFloat( flDieTime * 0.75f, flDieTime );

	pParticle->m_vecVelocity.Init( random->RandomFloat( -16.0f, 16.0f ), random->RandomFloat( -16.0f, 16.0f ), random->RandomFloat( 2.0f, SMOKE_RISE_RATE ) );

	int color = random->RandomInt( 8, 64 );
	pParticle->m_uchColor[0] = color;
	pParticle->m_uchColor[1] = color;
	pParticle->m_uchColor[2] = color;
	pParticle->m_uchColor[3] = random->RandomInt( 128, 256 );

	pParticle->m_uchStartSize	= random->RandomFloat( 32.0f, 48.0f );
	pParticle->m_uchEndSize		= pParticle->m_uchStartSize * 3.0f;
}


IMPLEMENT_CLIENTCLASS_DT( C_EntityFlame, DT_EntityFlame, CEntityFlame )
	RecvPropFloat(RECVINFO(m_flSize)),
	RecvPropEHandle(RECVINFO(m_hEntAttached)),
	RecvPropBool(RECVINFO(m_bUseHitboxes)),
	RecvPropInt(RECVINFO(m_iNumHitboxFires)),
	RecvPropFloat(RECVINFO(m_flHitboxFireScale)),
	RecvPropTime(RECVINFO(m_flLifetime)),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_EntityFlame::C_EntityFlame( void ) :
m_hEffect( NULL )
{
	m_bClassicFire			= false;
	m_hOldAttached			= NULL;
	m_flSize				= 4.0f;
	m_pEmitter				= NULL;
	m_flLifetime			= 0;
	m_bStartedFading		= false;
	m_bCreatedClientside	= false;

	m_bClassicFire = BIsCurrentFireTypeClassic();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_EntityFlame::~C_EntityFlame( void )
{
	ParticleProp()->StopEmission( NULL, true, true );
	CleanUpRagdollOnRemove();

	if ( m_bAttachedToHitboxes )
	{
		for ( int i = 0; i < NUM_HITBOX_FIRES; i++ )
		{
			if ( m_pFireSmoke[i] )
			{
				delete m_pFireSmoke[i];
				m_pFireSmoke[i] = NULL;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Stop the burning sounds and effects on the entity we're attached to
//-----------------------------------------------------------------------------
void C_EntityFlame::CleanUpRagdollOnRemove( void )
{
	if ( m_hEntAttached )
	{
		m_hEntAttached->RemoveFlag( FL_ONFIRE );
		m_hEntAttached->SetEffectEntity( NULL );
		m_hEntAttached->StopSound( "General.BurningFlesh" );
		m_hEntAttached->StopSound( "General.BurningObject" );

		m_hEntAttached = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_EntityFlame::UpdateOnRemove( void )
{
	ParticleProp()->StopEmission( NULL, true, true );
	CleanUpRagdollOnRemove();

	BaseClass::UpdateOnRemove();
}

void C_EntityFlame::CreateEffect( void )
{
	ParticleProp()->StopEmissionAndDestroyImmediately( m_hEffect );

#ifdef TF_CLIENT_DLL
	m_hEffect = ParticleProp()->Create( "burningplayer_red", PATTACH_ABSORIGIN_FOLLOW );
#else
	m_hEffect = ParticleProp()->Create( "burning_character", PATTACH_ABSORIGIN_FOLLOW );
#endif

	if ( m_hEffect )
	{
		C_BaseEntity *pEntity = m_hEntAttached;
		m_hOldAttached = m_hEntAttached;

		ParticleProp()->AddControlPoint( m_hEffect, 1, pEntity, PATTACH_ABSORIGIN_FOLLOW );
		m_hEffect->SetControlPoint( 0, GetAbsOrigin() );
		m_hEffect->SetControlPoint( 1, GetAbsOrigin() );
		m_hEffect->SetControlPointEntity( 0, pEntity );
		m_hEffect->SetControlPointEntity( 1, pEntity );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_EntityFlame::OnDataChanged( DataUpdateType_t updateType )
{
	if ( updateType == DATA_UPDATE_CREATED )
	{
		if ( BIsCurrentFireTypeClassic() )
		{
			CreateClassicEffect();
		}
		else
		{
			CreateEffect();
		}
	}

	// FIXME: This is a bit of a shady path
	if ( updateType == DATA_UPDATE_DATATABLE_CHANGED )
	{
		if ( BIsCurrentFireTypeClassic() )
		{
			// Rebuild the hitbox flames if their settings changed
			if ( m_iOldNumHitboxFires != m_iNumHitboxFires || m_flOldHitboxFireScale != m_flHitboxFireScale )
			{
				m_iOldNumHitboxFires = m_iNumHitboxFires;
				m_flOldHitboxFireScale = m_flHitboxFireScale;

				if ( !m_hEntAttached )
					return;

				DeleteHitBoxFlames();
				AttachToHitBoxes();
			}
		}
		else
		{
			// If our owner changed, then recreate the effect
			if ( m_hEntAttached != m_hOldAttached )
			{
				CreateEffect();
			}
		}
	}

	BaseClass::OnDataChanged( updateType );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_EntityFlame::CreateClassicEffect( void )
{
	m_iOldNumHitboxFires = m_iNumHitboxFires;
	m_flOldHitboxFireScale = m_flHitboxFireScale;

	C_BaseEntity *pEnt = m_hEntAttached;

	if ( pEnt )
	{
		if ( m_bUseHitboxes && pEnt->GetBaseAnimating() != NULL )
		{
			AttachToHitBoxes();
		}
		else
		{
			m_vecLastPosition = GetRenderOrigin();

			m_ParticleSpawn.Init( 60 ); //Events per second

			m_pEmitter = CEmberEffect::Create("C_EntityFlame::Create");

			if ( m_pEmitter.IsValid() )
			{
				for ( int i = 1; i < NUM_FLAMELETS+1; i++ )
				{
					m_MaterialHandle[i-1] = m_pEmitter->GetPMaterial( VarArgs( "sprites/flamelet%d", i ) );
				}

				m_pEmitter->SetSortOrigin( GetAbsOrigin() );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
struct HitboxVolume_t
{
	int nIndex;
	float flVolume;
};

//-----------------------------------------------------------------------------
// Purpose: Sorts the hitboxes by volume.
//-----------------------------------------------------------------------------
int __cdecl SortHitboxVolumes( HitboxVolume_t *elem1, HitboxVolume_t *elem2 )
{
	if ( elem1->flVolume > elem2->flVolume + 50 ) return -1;
	if ( elem1->flVolume < elem2->flVolume + 50 ) return 1;
	if ( elem1->flVolume != elem2->flVolume ) return random->RandomInt( -1, 1 );
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Attaches fire to the hitboxes of an animating character. The fire
//			is distributed based on hitbox volumes -- it attaches to the larger
//			hitboxes first.
//-----------------------------------------------------------------------------
void C_EntityFlame::AttachToHitBoxes( void )
{
	m_pCachedModel = NULL;

	C_BaseCombatCharacter *pAnimating = (C_BaseCombatCharacter *)m_hEntAttached.Get();

	if ( !pAnimating || !pAnimating->GetModel() )
	{
		return;
	}

	CStudioHdr *pStudioHdr = pAnimating->GetModelPtr();
	if ( !pStudioHdr )
	{
		return;
	}

	mstudiohitboxset_t *set = pStudioHdr->pHitboxSet( pAnimating->m_nHitboxSet );
	if ( !set )
	{
		return;
	}

	if ( !set->numhitboxes )
	{
		return;
	}

	m_pCachedModel = pAnimating->GetModel();

	CBoneCache *pCache = pAnimating->GetBoneCache( pStudioHdr );
	matrix3x4_t	*hitboxbones[MAXSTUDIOBONES];
	pCache->ReadCachedBonePointers( hitboxbones, pStudioHdr->numbones() );

	//
	// Sort the hitboxes by volume.
	//
	HitboxVolume_t hitboxvolume[MAXSTUDIOBONES];
	for ( int i = 0; i < set->numhitboxes; i++ )
	{
		mstudiobbox_t *pBox = set->pHitbox(i);
		hitboxvolume[i].nIndex = i;
		hitboxvolume[i].flVolume = ( pBox->bbmax.x - pBox->bbmin.x ) * ( pBox->bbmax.y - pBox->bbmin.y ) * ( pBox->bbmax.z - pBox->bbmin.z );
	}
	qsort( hitboxvolume, set->numhitboxes, sizeof( hitboxvolume[0] ), (int (__cdecl *)(const void *, const void *))SortHitboxVolumes );

	//
	// Attach fire to the hitboxes.
	//
	for ( int i = 0; i < m_iNumHitboxFires; i++ )
	{
		int hitboxindex;
		//
		// Pick the 5 biggest hitboxes, or random ones if there are less than 5 hitboxes,
		// then pick random ones after that.
		//
		if ( ( i < 5 ) && ( i < set->numhitboxes ) )
		{
			hitboxindex = i;
		}
		else
		{
			hitboxindex = random->RandomInt( 0, set->numhitboxes - 1 );
		}
		
		mstudiobbox_t *pBox = set->pHitbox( hitboxvolume[hitboxindex].nIndex );

		Assert( hitboxbones[pBox->bone] );

		m_nHitbox[i] = hitboxvolume[hitboxindex].nIndex;
		m_pFireSmoke[i] = new C_FireSmoke;

		//
		// Calculate a position within the hitbox to place the fire.
		//
		m_vecFireOrigin[i] = Vector( random->RandomFloat( pBox->bbmin.x, pBox->bbmax.x ), random->RandomFloat( pBox->bbmin.y, pBox->bbmax.y ), random->RandomFloat( pBox->bbmin.z, pBox->bbmax.z ) );
		Vector vecAbsOrigin;
		VectorTransform( m_vecFireOrigin[i], *hitboxbones[pBox->bone], vecAbsOrigin );
		m_pFireSmoke[i]->SetLocalOrigin( vecAbsOrigin );

		//
		// The first fire emits smoke, the rest do not.
		//
		m_pFireSmoke[i]->m_nFlags = bitsFIRESMOKE_ACTIVE;

		m_pFireSmoke[i]->m_nFlameModelIndex	= modelinfo->GetModelIndex( "sprites/fire1.vmt" );
		m_pFireSmoke[i]->m_nFlameFromAboveModelIndex = modelinfo->GetModelIndex( "sprites/flamefromabove.vmt" );
		m_pFireSmoke[i]->m_flScale = 0;
		m_pFireSmoke[i]->m_flStartScale = 0;
		m_pFireSmoke[i]->m_flScaleTime = 1.5;
		m_pFireSmoke[i]->m_flScaleRegister = 0.1;
		m_pFireSmoke[i]->m_flChildFlameSpread = 20.0;
		m_pFireSmoke[i]->m_flScaleStart = 0;
		m_pFireSmoke[i]->SetOwnerEntity( this );
		
		// Do a simple approximation of the area the fire is covering based on the hitbox volume.
		float flVolume = hitboxvolume[hitboxindex].flVolume;
		if ( flVolume < 1000.0f )
		{
			flVolume = 1000.0f;
		}
		else if ( flVolume > 4000.0f )
		{
			flVolume = 4000.0f;
		}
		m_pFireSmoke[i]->m_flScaleEnd = flVolume * 0.00012f * m_flHitboxFireScale;
		m_pFireSmoke[i]->m_flScaleTimeStart = Helper_GetTime();
		m_pFireSmoke[i]->m_flScaleTimeEnd = Helper_GetTime() + 2.0;

		m_pFireSmoke[i]->StartClientOnly();
	}

	m_bAttachedToHitboxes = true;
}

//-----------------------------------------------------------------------------
// Purpose: Restart the flame with the effect type r_classic_fire now asks for
//-----------------------------------------------------------------------------
void C_EntityFlame::ChangeFireEffectType( void )
{
	m_bClassicFire = BIsCurrentFireTypeClassic();

	if ( m_bClassicFire )
	{
		ParticleProp()->StopEmission( NULL, true, true );
		CreateClassicEffect();
	}
	else
	{
		DeleteHitBoxFlames();
		CreateEffect();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_EntityFlame::Simulate( void )
{
	if ( gpGlobals->frametime <= 0.0f )
		return;

	if ( m_bClassicFire != BIsCurrentFireTypeClassic() )
	{
		SetNextClientThink( gpGlobals->curtime );
	}

	if ( HL2_EPISODIC_ACTIVE() )
	{
#ifdef HL2_EPISODIC

		if ( BIsCurrentFireTypeClassic() && !m_bCreatedClientside && !m_bStartedFading && ( m_flLifetime - gpGlobals->curtime ) < 2.0f )
		{
			// Shrink the hitbox flames away before we expire
			for ( int i = 0; i < NUM_HITBOX_FIRES; i++ )
			{
				if ( m_pFireSmoke[i] != NULL )
				{
					m_pFireSmoke[i]->m_flScaleStart = m_pFireSmoke[i]->m_flScaleEnd;
					m_pFireSmoke[i]->m_flScaleEnd = 0.00001;
					m_pFireSmoke[i]->m_flScaleTimeStart = gpGlobals->curtime;
					m_pFireSmoke[i]->m_flScaleTimeEnd = m_flLifetime;
					m_pFireSmoke[i]->m_flScaleRegister = -1;
				}
			}

			m_bStartedFading = true;
		}

		if ( IsEffectActive(EF_BRIGHTLIGHT) || IsEffectActive(EF_DIMLIGHT) )
		{
			dlight_t *dl = effects->CL_AllocDlight ( index );
			dl->origin = GetAbsOrigin();
	 		dl->origin[2] += 16;
			dl->color.r = 254;
			dl->color.g = 174;
			dl->color.b = 10;
			dl->radius = random->RandomFloat(400,431);
			dl->die = gpGlobals->curtime + 0.001;

			if ( BIsCurrentFireTypeClassic() && m_pFireSmoke[0] )
			{
				if ( m_pFireSmoke[0]->m_flScaleRegister == -1 )
				{
					// We have to use the previous scale since the new scale isn't set up yet
					dl->radius *= m_pFireSmoke[0]->m_flScaleStart;
				}
				else
				{
					dl->radius *= m_pFireSmoke[0]->m_flScaleRegister;
				}
			}
		}

#endif
	}

	if ( !BIsCurrentFireTypeClassic() )
		return;

	if ( m_bAttachedToHitboxes )
	{
		UpdateHitBoxFlames();
	}
	else if ( m_pEmitter.IsValid() )
	{
		m_pEmitter->SetSortOrigin( GetAbsOrigin() );

		SimpleParticle	*pParticle;
		Vector			offset;

		Vector	moveDiff	= GetAbsOrigin() - m_vecLastPosition;
		float	moveLength	= VectorNormalize( moveDiff );

		int	numPuffs = moveLength / ( m_flSize / 2.0f );

		//FIXME: More rational cap here, perhaps
		numPuffs = clamp( numPuffs, 1, 8 );

		float			step = moveLength / numPuffs;

		//Fill in the gaps
		for ( int i = 1; i < numPuffs+1; i++ )
		{
			offset = m_vecLastPosition + ( moveDiff * step * i );

			pParticle = (SimpleParticle *) m_pEmitter->AddParticle( sizeof(SimpleParticle), m_MaterialHandle[random->RandomInt( 0, NUM_FLAMELETS-1 )], offset );

			if ( pParticle )
			{
				pParticle->m_flDieTime	= 0.4f;
				pParticle->m_flLifetime	= 0.0f;
				
				pParticle->m_flRoll		= random->RandomInt( 0, 360 );
				pParticle->m_flRollDelta= random->RandomFloat( -2.0f, 2.0f );
				
				pParticle->m_uchStartSize	= random->RandomInt( m_flSize*0.25f, m_flSize*0.5f );
				pParticle->m_uchEndSize		= pParticle->m_uchStartSize * 2.0f;
				pParticle->m_uchStartAlpha	= 255;
				pParticle->m_uchEndAlpha	= 0;
				
				pParticle->m_uchColor[0] = pParticle->m_uchColor[1] = pParticle->m_uchColor[2] = 255;
				
				Vector dir;

				dir.x = random->RandomFloat( -1.0f, 1.0f );
				dir.y = random->RandomFloat( -1.0f, 1.0f );
				dir.z = random->RandomFloat( 0.5f, 1.0f );
			
				pParticle->m_vecVelocity = dir * random->RandomInt( 4, 32 );
				pParticle->m_vecVelocity[2] = random->RandomInt( 32, 64 );
			}
		}
	}

	m_vecLastPosition = GetRenderOrigin();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_EntityFlame::UpdateHitBoxFlames( void )
{
	C_BaseCombatCharacter *pAnimating = (C_BaseCombatCharacter *)m_hEntAttached.Get();
	if ( !pAnimating )
	{
		return;
	}

	if ( pAnimating->GetModel() != m_pCachedModel )
	{
		if ( m_pCachedModel != NULL )
		{
			// The model changed, so rebuild the flames
			DeleteHitBoxFlames();
			AttachToHitBoxes();
		}

		if ( m_pCachedModel == NULL )
		{
			return;
		}
	}

	studiohdr_t *pStudioHdr = modelinfo->GetStudiomodel( pAnimating->GetModel() );
	if ( !pStudioHdr )
	{
		return;
	}

	mstudiohitboxset_t *set = pStudioHdr->pHitboxSet( pAnimating->m_nHitboxSet );
	if ( !set )
	{
		return;
	}

	if ( !set->numhitboxes )
	{
		return;
	}

	pAnimating->SetupBones( NULL, -1, BONE_USED_BY_ANYTHING, gpGlobals->curtime );

	for ( int i = 0; i < NUM_HITBOX_FIRES; i++ )
	{
		Vector vecAbsOrigin;
		mstudiobbox_t *pBox = NULL;

		if ( m_pFireSmoke[i] != NULL )
		{
			pBox = set->pHitbox( m_nHitbox[i] );

			VectorTransform( m_vecFireOrigin[i], pAnimating->GetBone( pBox->bone ), vecAbsOrigin );
			m_pFireSmoke[i]->SetLocalOrigin( vecAbsOrigin );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_EntityFlame::ClientThink( void )
{
	if ( m_bClassicFire != BIsCurrentFireTypeClassic() )
	{
		ChangeFireEffectType();
		return;
	}

	if ( BIsCurrentFireTypeClassic() )
	{
		for ( int i = 0; i < NUM_HITBOX_FIRES; i++ )
		{
			if ( m_pFireSmoke[i] != NULL )
			{
				if ( m_pFireSmoke[i]->m_bFadingOut == false )
				{
					m_pFireSmoke[i]->m_flScaleStart = m_pFireSmoke[i]->m_flScaleEnd;
					m_pFireSmoke[i]->m_flScaleEnd = 0.00001;
					m_pFireSmoke[i]->m_flScaleTimeStart = Helper_GetTime();
					m_pFireSmoke[i]->m_flScaleTimeEnd = Helper_GetTime() + 2.0;
					m_pFireSmoke[i]->m_flScaleRegister = -1;
					m_pFireSmoke[i]->m_bFadingOut = true;
				}
				else if ( m_pFireSmoke[i]->m_flScaleTimeEnd <= Helper_GetTime() )
				{
					if ( m_hEntAttached )
					{
						CPASAttenuationFilter filter( m_hEntAttached );
						m_hEntAttached->EmitSound( filter, m_hEntAttached->GetSoundSourceIndex(), "General.StopBurning" );
					}

					CleanUpRagdollOnRemove();
					Release();
					return;
				}
			}
		}

		SetNextClientThink( gpGlobals->curtime + 0.1f );
		return;
	}

	ParticleProp()->StopEmission( NULL, true, true );
	CleanUpRagdollOnRemove();
	Release();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_EntityFlame::DeleteHitBoxFlames( void )
{
	for ( int i = 0; i < NUM_HITBOX_FIRES; i++ )
	{
		if ( m_pFireSmoke[i] )
		{
			m_pFireSmoke[i]->RemoveClientOnly();
			delete m_pFireSmoke[i];
			m_pFireSmoke[i] = NULL;
		}
	}
}
