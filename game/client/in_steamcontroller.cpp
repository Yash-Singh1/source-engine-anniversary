//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Steam Input (controller) movement
//
//===========================================================================//

#include "cbase.h"
#include "steam/steam_api.h"
#include "kbutton.h"
#include "usercmd.h"
#include "input.h"
#include "iviewrender.h"
#include "convar.h"
#include "hud.h"
#include "ienginevgui.h"
#include "inputsystem/iinputsystem.h"
#include "tier2/tier2.h"
#include "c_basehlplayer.h"
#include "c_prop_vehicle.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// up / down
#define	PITCH	0
// left / right
#define	YAW		1

extern ConVar cl_forwardspeed;
extern ConVar cl_backspeed;
extern ConVar cl_sidespeed;
extern ConVar cl_yawspeed;
extern ConVar cl_pitchspeed;
extern ConVar cl_pitchdown;
extern ConVar cl_pitchup;
extern ConVar lookspring;
extern ConVar sk_allow_autoaim;
extern ConVar sc_autoaim_mode;
extern ConVar sc_autoaim_scale;

float ResponseCurveLook( int curve, float x, int axis, float otherAxis, float dist, float frametime );

static ConVar sc_enable( "sc_enable", "1", FCVAR_ARCHIVE, "Enable SteamController" );

static ConVar sc_yaw_sensitivity( "sc_yaw_sensitivity_new2", "0.10", FCVAR_ARCHIVE, "SteamController yaw factor." );
static ConVar sc_yaw_sensitivity_default( "sc_yaw_sensitivity_default", "1.0", FCVAR_NONE );

static ConVar sc_pitch_sensitivity( "sc_pitch_sensitivity_new2", "0.10", FCVAR_ARCHIVE, "SteamController pitch factor." );
static ConVar sc_pitch_sensitivity_default( "sc_pitch_sensitivity_default", "0.9", FCVAR_NONE );

NOV2024_ONLY( ConVar sc_autoaimdampen( "sc_autoaimdampen", "0.5", FCVAR_ARCHIVE, "Dampening Factor for Aim Assist" ); )
NOV2024_ONLY( ConVar sc_autoaimdampen_speed_factor( "sc_autoaimdampen_speed_factor", "0.35", FCVAR_ARCHIVE, "Speed Contribution for Dampening Factor for Aim Assist" ); )
NOV2024_ONLY( ConVar sc_autoaimdampen_distance_factor( "sc_autoaimdampen_distance_factor", "0.25", FCVAR_ARCHIVE, "Distance Contribution for Dampening Factor for Aim Assist" ); )
NOV2024_ONLY( ConVar sc_autoaimdampenrange( "sc_autoaimdampenrange", "35", FCVAR_ARCHIVE, "Speed Factor for Applying Aim Assist Dampening" ); )
NOV2024_ONLY( ConVar sc_autoaim_magnetism( "sc_autoaim_magnetism", "0.03", FCVAR_ARCHIVE, "Magnetism Factor for Aim Assist" ); )
NOV2024_ONLY( ConVar sc_sensitivity_scale( "sc_sensitivity_scale", "1.0", FCVAR_ARCHIVE, "Global scale to Steam Input look sensitivity." ); )

#ifdef HL2_ANNIVERSARY_NOV2024
//-----------------------------------------------------------------------------
// Purpose: Aim assist dampening for Steam Input look deltas
//-----------------------------------------------------------------------------
float GetSCAutoAimDampening( float flValue )
{
	if ( sc_autoaim_mode.GetInt() > 0 && sc_autoaimdampen.GetFloat() > 0.0f )
	{
		C_BaseHLPlayer *pLocalPlayer = (C_BaseHLPlayer *)C_BasePlayer::GetLocalPlayer();
		if ( pLocalPlayer )
		{
			float flScale = 1.0f;
			if ( sk_allow_autoaim.GetBool() && sc_autoaim_mode.GetBool() )
			{
				flScale = sc_autoaim_scale.GetFloat();
			}

			if ( pLocalPlayer->m_HL2Local.m_bAutoAimTarget )
			{
				float flAbsValue = fabs( flValue );
				float flRange = sc_autoaimdampenrange.GetFloat() * flScale * sc_sensitivity_scale.GetFloat();
				if ( flAbsValue < flRange )
				{
					float flDistance = 0.0f;
					C_BaseEntity *pTarget = pLocalPlayer->m_HL2Local.m_hAutoAimTarget;
					if ( pTarget )
					{
						flDistance = clamp( pTarget->GetAbsOrigin().DistToSqr( pLocalPlayer->GetAbsOrigin() ) / ( 3000.0f * 3000.0f ), 0.0f, 1.0f ) * sc_autoaimdampen_distance_factor.GetFloat();
					}

					float flSpeed = ( flAbsValue / flRange ) * sc_autoaimdampen_speed_factor.GetFloat();
					float flExponent = flScale + flSpeed + flDistance;
					return MIN( 1.0, pow( sc_autoaimdampen.GetFloat(), flExponent ) );
				}
			}
		}
	}

	return 1.0f;
}
#endif
#line 97

CON_COMMAND( sc_flashlight, "Toggles the flashlight." )
{
	engine->ClientCmd_Unrestricted( "impulse 100" );
}

CON_COMMAND( sc_squad, "Call/recall squad" )
{
	engine->ClientCmd_Unrestricted( "impulse 50" );
}

ConVar sc_smart_sprint( "sc_smart_sprint", "1", FCVAR_ARCHIVE, "When using a Steam Controller, toggle sprint after sc_smart_sprint_start_bounds analogue joy move until we go under sc_smart_sprint_end_bounds of analogue joy move" );
ConVar sc_smart_sprint_start_bounds( "sc_smart_sprint_start_bounds", "0.50", FCVAR_ARCHIVE, "If we got past this, enable 'smart sprint'." );
ConVar sc_smart_sprint_end_bounds( "sc_smart_sprint_end_bounds", "0.35", FCVAR_ARCHIVE, "If we are in 'smart sprint' and we go under this amount on the joystick, cancel our sprint if sprint is not held." );

static ConVar sc_mouse_sensitivity( "sc_mouse_sensitivity", "0.25", FCVAR_ARCHIVE, "SteamInput global mouse sensitivity factor" );

// Commands issued by the digital actions of the Steam Input manifest. The action name is the command without a leading '+'.
static const char *g_ControllerDigitalGameActions[] =
{
	"+attack",
	"+attack2",
	"+zoom",
	"+reload",
	"+jump",
	"+duck",
	"toggle_duck",
	"toggle_zoom",
	"phys_swap",
	"bug_swap",
	"+use",
	"invnext",
	"invprev",
	"lastinv",
	"+walk",
	"+speed",
	"sc_flashlight",
	"sc_squad",
	"+lookspin",
	"+resetcamera",
	"slot1",
	"slot2",
	"slot3",
	"slot4",
	NOV2024_ONLY( "sc_save_quick", )
	NOV2024_ONLY( "sc_load_quick", )
	NOV2024_ONLY( "smart_sprint", )
};

struct ControllerDigitalActionState
{
	const char *cmd;
	InputDigitalActionHandle_t handle;
	bool bState;
};

int g_ActionSet;

static InputAnalogActionHandle_t g_ControllerMoveHandle;
static ControllerDigitalActionState g_ControllerDigitalActionState[STEAM_INPUT_MAX_COUNT][ARRAYSIZE( g_ControllerDigitalGameActions )];
static InputActionSetHandle_t g_ControllerCurrentActionSet[STEAM_INPUT_MAX_COUNT];
static InputAnalogActionHandle_t g_ControllerSteerHandle;
static InputAnalogActionHandle_t g_ControllerAccelerationHandle;
static InputAnalogActionHandle_t g_ControllerBreakingHandle;
static InputAnalogActionHandle_t g_ControllerJoystickCameraHandle;
static InputAnalogActionHandle_t g_ControllerCameraHandle;
static InputAnalogActionHandle_t g_ControllerGyroAngles;
static InputAnalogActionHandle_t g_ControllerTrackPadAngles;
static InputAnalogActionHandle_t g_ControllerJoystickAngles;
static InputAnalogActionHandle_t g_ControllerFlickStickAngles;

//-----------------------------------------------------------------------------
// Purpose: Looks up the action handles for every connected controller
// Output : number of controllers initialized, 0 on failure
//-----------------------------------------------------------------------------
static int InitializeSteamInputActions()
{
	ControllerHandle_t controllers[STEAM_CONTROLLER_MAX_COUNT];
	int nControllers = SteamController()->GetConnectedControllers( controllers );

	for ( int i = 0; i < ARRAYSIZE( g_ControllerDigitalGameActions ); ++i )
	{
		const char *cmd = g_ControllerDigitalGameActions[i];
		const char *action = ( cmd[0] == '+' ) ? cmd + 1 : cmd;

		for ( int j = 0; j < nControllers; ++j )
		{
			ControllerDigitalActionState &state = g_ControllerDigitalActionState[j][i];
			state.handle = SteamInput()->GetDigitalActionHandle( action );
			if ( i == 0 && state.handle == 0 )
				return 0;

			state.cmd = cmd;
			state.bState = false;
		}
	}

	g_ControllerMoveHandle = SteamInput()->GetAnalogActionHandle( "Move" );
	g_ControllerSteerHandle = SteamInput()->GetAnalogActionHandle( "Steer" );
	g_ControllerAccelerationHandle = SteamInput()->GetAnalogActionHandle( "Accelerate" );
	g_ControllerBreakingHandle = SteamInput()->GetAnalogActionHandle( "Brake" );
	g_ControllerCameraHandle = SteamInput()->GetAnalogActionHandle( "Camera" );
	g_ControllerJoystickCameraHandle = SteamInput()->GetAnalogActionHandle( "JoystickCamera" );
	NOV2024_ONLY( g_ControllerGyroAngles = SteamInput()->GetAnalogActionHandle( "GyroCameraAngles" ); )
	NOV2024_ONLY( g_ControllerTrackPadAngles = SteamInput()->GetAnalogActionHandle( "TouchPadCameraAngles" ); )
	NOV2024_ONLY( g_ControllerJoystickAngles = SteamInput()->GetAnalogActionHandle( "JoystickCameraAngles" ); )
	NOV2024_ONLY( g_ControllerFlickStickAngles = SteamInput()->GetAnalogActionHandle( "FlickStickCameraAngles" ); )

	return nControllers;
}

//-----------------------------------------------------------------------------
// Purpose: SteamControllerMove -- main entry point for applying Steam Input movements
// Input  : *cmd - 
//-----------------------------------------------------------------------------
void CInput::SteamControllerMove( float flFrametime, CUserCmd *cmd )
{
	if ( enginevgui->IsGameUIVisible() || !SteamController() )
		return;

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	C_BaseHLPlayer *pHLPlayer = NULL;
	bool bInVehicle = false;
	bool bSprinting = false;
	if ( pPlayer )
	{
		bInVehicle = dynamic_cast< C_PropVehicleDriveable * >( pPlayer->GetVehicleEntity() ) != NULL;
		pHLPlayer = dynamic_cast< C_BaseHLPlayer * >( pPlayer );
		if ( pHLPlayer )
		{
			bSprinting = pHLPlayer->IsSprinting();
		}
	}

	g_ActionSet = bInVehicle ? GAME_ACTION_SET_VEHICLECONTROLS : GAME_ACTION_SET_GAMECONTROLS;
	g_pInputSystem->ActivateSteamControllerActionSetForSlot( STEAM_INPUT_HANDLE_ALL_CONTROLLERS, (GameActionSet_t)g_ActionSet );

	InputHandle_t controllers[STEAM_INPUT_MAX_COUNT];
	int nControllers = SteamInput()->GetConnectedControllers( controllers );

	OCT2024_ONLY( if ( nControllers <= 0 ) return; ) static int nControllersInitialized = 0;
	if ( nControllers != nControllersInitialized )
	{
		nControllersInitialized = InitializeSteamInputActions();
		NOV2024_ONLY( Joystick_Advanced(); )
	}

	if ( !nControllersInitialized NOV2024_ONLY( || nControllers <= 0 ) )
		return;

	QAngle viewangles;
	engine->GetViewAngles( viewangles );

	view->StopPitchDrift();

	bool bSmartSprintHeld = false;
	Vector2D vecMoveTotal( 0.0f, 0.0f );

	for ( int i = 0; i < nControllers && i < STEAM_INPUT_MAX_COUNT; ++i )
	{
		InputHandle_t controller = controllers[i];

		InputAnalogActionData_t moveData = SteamInput()->GetAnalogActionData( controller, g_ControllerMoveHandle );
		vecMoveTotal.x += moveData.x;

		InputActionSetHandle_t actionSet = SteamInput()->GetCurrentActionSet( controller );

		for ( int j = 0; j < ARRAYSIZE( g_ControllerDigitalGameActions ); ++j )
		{
			ControllerDigitalActionState &state = g_ControllerDigitalActionState[i][j];
			InputDigitalActionData_t data = SteamInput()->GetDigitalActionData( controller, state.handle );

			bool bActionSetChanged = g_ControllerCurrentActionSet[i] != actionSet;
			bool bSmartSprint = !V_strcmp( state.cmd, NOV2024_ONLY( "smart_sprint" ) OCT2024_ONLY( "+speed" ) );

			if ( bActionSetChanged )
			{
#ifdef HL2_ANNIVERSARY_NOV2024
				// Don't carry buttons over from the previous action set
				int bits = ::input->GetButtonBits( 0 );
				if ( bits > 0 )
				{
					::input->ClearInputButton( bits );
				}
#endif
#line 280
				state.bState = data.bState;
			}
			else if ( data.bState != state.bState )
			{
				if ( data.bState || state.cmd[0] == '+' )
				{
					sc_controller_mode.SetValue( 1 );

					if ( !bSmartSprint || !sc_smart_sprint.GetBool() || !( !bSmartSprint || bSprinting ) )
					{
						char cmdbuf[128];
						Q_snprintf( cmdbuf, sizeof( cmdbuf ), "%s", state.cmd );
						if ( !data.bState )
						{
							cmdbuf[0] = '-';
						}

						engine->ClientCmd_Unrestricted( cmdbuf );
					}

					g_pInputSystem->SetActiveInputType( ACTIVE_INPUT_TYPE_STEAM_INPUT, controller );
				}

				state.bState = data.bState;
			}

			if ( bSmartSprint )
			{
				bSmartSprintHeld |= state.bState;
			}
		}

		vecMoveTotal.y += moveData.y;

		InputAnalogActionData_t steerData = SteamInput()->GetAnalogActionData( controller, g_ControllerSteerHandle );
		InputAnalogActionData_t accelerationData = SteamInput()->GetAnalogActionData( controller, g_ControllerAccelerationHandle );
		InputAnalogActionData_t brakeData = SteamInput()->GetAnalogActionData( controller, g_ControllerBreakingHandle );

		// Apply forward/back movement
		if ( moveData.y > 0.0f )
		{
			cmd->forwardmove += moveData.y * cl_forwardspeed.GetFloat();
		}
		else
		{
			cmd->forwardmove += moveData.y * cl_backspeed.GetFloat();
		}

		if ( accelerationData.x > 0.0f )
		{
			cmd->forwardmove += accelerationData.x * cl_forwardspeed.GetFloat();
		}

		if ( brakeData.x > 0.0f )
		{
			cmd->forwardmove -= brakeData.x * cl_backspeed.GetFloat();
		}

		// Apply sidestep movement
		cmd->sidemove += ( moveData.x + steerData.x ) * cl_sidespeed.GetFloat();

		InputAnalogActionData_t joystickCameraData = SteamInput()->GetAnalogActionData( controller, g_ControllerJoystickCameraHandle );
		InputAnalogActionData_t cameraData = SteamInput()->GetAnalogActionData( controller, g_ControllerCameraHandle );

		float flMouseX = cameraData.x * sc_mouse_sensitivity.GetFloat();
		float flMouseY = sc_mouse_sensitivity.GetFloat() * cameraData.y;
		float flMouseLength = Vector2D( flMouseX, flMouseY ).Length();
		if ( flMouseLength > 2.0f )
		{
			g_pInputSystem->SetActiveInputType( ACTIVE_INPUT_TYPE_STEAM_INPUT, controller );
		}

		QAngle angDelta( 0.0f, 0.0f, 0.0f );

		if ( m_flRemainingSteamInputSampleTime > 0.0f )
		{
			float frametime = MIN( m_flRemainingSteamInputSampleTime, flFrametime );
			m_flRemainingSteamInputSampleTime -= frametime;

			if ( joystickCameraData.bActive )
			{
				static ConVarRef joy_variable_frametime( "joy_variable_frametime" );
				bool bVariableFrametime = joy_variable_frametime.GetBool();
				float flSampleTime = bVariableFrametime ? frametime : gpGlobals->frametime;
				float aspeed = flSampleTime * gHUD.GetFOVSensitivityAdjust();

				float flJoystickYaw = joystickCameraData.x;
				float flJoystickPitch = joystickCameraData.y;

				static ConVarRef s_joy_yawsensitivity( "joy_yawsensitivity" );
				static ConVarRef s_joy_pitchsensitivity( "joy_pitchsensitivity" );
				ConVarRef joy_response_look( "joy_response_look" );

				float dist = Vector2D( flJoystickYaw, flJoystickPitch ).Length();

				angDelta[YAW] += ( bVariableFrametime || frametime != gpGlobals->frametime ) ? ResponseCurveLook( joy_response_look.GetInt(), flJoystickYaw, YAW, flJoystickPitch, dist, flSampleTime ) * s_joy_yawsensitivity.GetFloat() * aspeed * cl_yawspeed.GetFloat() : 0.0f;
				angDelta[PITCH] += ( bVariableFrametime || frametime != gpGlobals->frametime ) ? -( ResponseCurveLook( joy_response_look.GetInt(), flJoystickPitch, PITCH, flJoystickYaw, dist, flSampleTime ) * s_joy_pitchsensitivity.GetFloat() * aspeed * cl_pitchspeed.GetFloat() ) : 0.0f;

				view->StopPitchDrift();
				if ( flJoystickPitch == 0.f && lookspring.GetFloat() == 0.f )
				{
					// no pitch movement
					// disable pitch return-to-center unless requested by user
					view->StopPitchDrift();
				}

				if ( fabs( flJoystickPitch ) > 0.5f || fabs( flJoystickYaw ) > 0.5f )
				{
					g_pInputSystem->SetActiveInputType( ACTIVE_INPUT_TYPE_STEAM_INPUT, 0 );
				}
			}
		}

		if ( flMouseLength > 0.0f )
		{
			sc_controller_mode.SetValue( 1 );

			if ( !m_fCameraInterceptingMouse )
			{
#ifdef HL2_ANNIVERSARY_NOV2024
				flMouseY = -flMouseY;
				angDelta[YAW] -= CAM_CapYaw( gHUD.GetFOVSensitivityAdjust() * sc_yaw_sensitivity.GetFloat() * flMouseX * GetSCAutoAimDampening( flMouseX ) * sc_sensitivity_scale.GetFloat() );
				angDelta[PITCH] -= gHUD.GetFOVSensitivityAdjust() * flMouseY * sc_pitch_sensitivity.GetFloat() * GetSCAutoAimDampening( flMouseY ) * sc_sensitivity_scale.GetFloat();
#else
				angDelta[YAW] -= CAM_CapYaw( gHUD.GetFOVSensitivityAdjust() * sc_yaw_sensitivity.GetFloat() * flMouseX );
				angDelta[PITCH] += gHUD.GetFOVSensitivityAdjust() * flMouseY * sc_pitch_sensitivity.GetFloat();
#endif
#line 402
			}
		}

#ifdef HL2_ANNIVERSARY_NOV2024
		UpdateQuickTurnAndCameraReset( angDelta, flFrametime );
		UpdateSteamInputCameraAngles( controller, angDelta );
#else
		angDelta[YAW] = UpdateAndGetQuickTurnYaw( flFrametime, angDelta[YAW] );
		angDelta[PITCH] = UpdateAndGetCameraReset( viewangles, flFrametime, angDelta[PITCH] );
#endif
#line 407

		cmd->mousedx += angDelta[YAW];
		cmd->mousedy += angDelta[PITCH];

		viewangles += angDelta;

#ifdef HL2_ANNIVERSARY_NOV2024
		if ( sk_allow_autoaim.GetBool() && g_pInputSystem->IsSteamControllerActive() )
		{
			viewangles = DoAutoAimMagnetism( viewangles, sc_autoaim_mode.GetInt(), sc_autoaim_magnetism.GetFloat(), sc_autoaim_scale.GetFloat() );
		}
#endif
#line 417

		viewangles[PITCH] = clamp( viewangles[PITCH], -cl_pitchdown.GetFloat(), cl_pitchup.GetFloat() );

		g_ControllerCurrentActionSet[i] = actionSet;
	}

	bool bSmartSprintActive = pHLPlayer->m_bSmartSprintActive;
	if ( g_pInputSystem->IsSteamControllerActive() )
	{
		if ( sc_smart_sprint.GetBool() && ( bSmartSprintHeld || bSmartSprintActive OCT2024_ONLY( || bSprinting ) ) )
		{
			float flMoveLength = vecMoveTotal.Length();
			bool bInBounds = bSmartSprintActive ? ( flMoveLength >= sc_smart_sprint_end_bounds.GetFloat() ) : ( flMoveLength >= sc_smart_sprint_start_bounds.GetFloat() );
			pHLPlayer->m_bSmartSprintActive = ( bSprinting || bSmartSprintHeld ) && bInBounds;
			if ( !pHLPlayer->m_bSmartSprintActive && !bSmartSprintHeld )
			{
				engine->ClientCmd_Unrestricted( "-speed" );
			}
		}
	}
	else
	{
		if ( sc_smart_sprint.GetBool() && bSmartSprintActive )
		{
			engine->ClientCmd_Unrestricted( "-speed" );
		}
		pHLPlayer->m_bSmartSprintActive = false;
	}

	engine->SetViewAngles( viewangles );
}

#ifdef HL2_ANNIVERSARY_NOV2024
//-----------------------------------------------------------------------------
// Purpose: Adds the gyro, touch pad, joystick and flick stick camera deltas
//-----------------------------------------------------------------------------
void CInput::UpdateSteamInputCameraAngles( uint64 controller, QAngle &angles )
{
	InputAnalogActionHandle_t handles[] = { g_ControllerGyroAngles, g_ControllerTrackPadAngles, g_ControllerJoystickAngles, g_ControllerFlickStickAngles };

	Vector2D vecDelta = vec2_origin;
	for ( int i = 0; i < (int)ARRAYSIZE( handles ); ++i )
	{
		if ( !handles[i] )
			continue;

		InputAnalogActionData_t data = SteamInput()->GetAnalogActionData( controller, handles[i] );
		if ( !data.bActive )
			continue;

		Vector2D vecInput( data.x, data.y );
		if ( g_ControllerFlickStickAngles != handles[i] )
		{
			vecInput *= gHUD.GetFOVSensitivityAdjust();
			vecInput.x *= GetSCAutoAimDampening( vecInput.x ); vecInput.y *= GetSCAutoAimDampening( vecInput.y );
			vecInput *= sc_sensitivity_scale.GetFloat();
		}
		vecDelta += vecInput;
	}

	angles[YAW] -= vecDelta.x;
	angles[PITCH] += vecDelta.y;
}
#endif
#line 479

//-----------------------------------------------------------------------------
// Purpose: Advanced Steam Input frame time accumulator
//-----------------------------------------------------------------------------
void CInput::SteamInput_SetSampleTime( float frametime )
{
	m_flRemainingSteamInputSampleTime = frametime;
}

CON_COMMAND( sc_force_action_manifest, "Forces an action manifest from the app install dir. Arg 0 is the game name, so you get steam_input/action_manifest_[game name].vdf loaded" )
{
	if ( args.ArgC() != 2 )
	{
		Msg( "Invalid arg count.\n" );
		return;
	}

	char szInstallDir[MAX_PATH + 1];
	SteamApps()->GetAppInstallDir( SteamUtils()->GetAppID(), szInstallDir, MAX_PATH );
	if ( !szInstallDir[0] )
	{
		Msg( "Couldn't get app install dir.\n" );
		return;
	}

	V_StripTrailingSlash( szInstallDir );

	char szManifest[MAX_PATH + 1];
	V_snprintf( szManifest, sizeof( szManifest ), "%s/steam_input/action_manifest_%s.vdf", szInstallDir, args[1] );
	Msg( NOV2024_ONLY( "[SteamInput] Loading action manifest from console: %s\n" ) OCT2024_ONLY( "Loading action manifest: %s\n" ), szManifest );
	SteamInput()->SetInputActionManifestFilePath( szManifest );
}

CON_COMMAND( sc_save_quick, "Helper binding for quick saving in Steam Input" )
{
	engine->ClientCmd_Unrestricted( "save quick" );
}

CON_COMMAND( sc_load_quick, "Helper binding for quick loading in Steam Input" )
{
	engine->ClientCmd_Unrestricted( NOV2024_ONLY( "load quick" ) OCT2024_ONLY( "save load" ) );
}

#ifdef HL2_ANNIVERSARY_NOV2024
CON_COMMAND( smart_sprint, "Helper binding for smart sprint in Steam Input" )
{
	engine->ClientCmd_Unrestricted( "+speed" );
}
#endif
