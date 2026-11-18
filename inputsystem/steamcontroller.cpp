//=========== Copyright Valve Corporation, All rights reserved. ===============//
//
// Purpose: Native Steam Controller Interface
//=============================================================================//
#include "inputsystem.h"
#include "key_translation.h"
#include "filesystem.h"
#include "steam/isteaminput.h"
#include "math.h"

#if !defined( NO_STEAM )
#include "steam/steam_api.h"
#endif //NO_STEAM

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

ConVar sc_joystick_map( "sc_joystick_map", "1", FCVAR_ARCHIVE, "How to map the analog joystick deadzone and extents 0 = Scaled Cross, 1 = Concentric Mapping to Square." );

//-----------------------------------------------------------------------------
// Purpose: Counts the number of active gamepads connected
//-----------------------------------------------------------------------------
uint32 CInputSystem::GetNumSteamControllersConnected()
{
	return m_unNumConnected;
}

struct SDigitalMenuAction
{
	const char *strName;
	ButtonCode_t buttonCode;
	InputDigitalActionHandle_t handle;
	bool bState[STEAM_INPUT_MAX_COUNT];
};

static SDigitalMenuAction g_DigitalMenuActions[] = {
	{ "menu_left", STEAMCONTROLLER_DPAD_LEFT, 0, { false } },
	{ "menu_right", STEAMCONTROLLER_DPAD_RIGHT, 0, { false } },
	{ "menu_up", STEAMCONTROLLER_DPAD_UP, 0, { false } },
	{ "menu_down", STEAMCONTROLLER_DPAD_DOWN, 0, { false } },
	{ "menu_cancel", STEAMCONTROLLER_B, 0, { false } },
	{ "menu_select", STEAMCONTROLLER_A, 0, { false } },
	{ "menu_x", STEAMCONTROLLER_X, 0, { false } },
	{ "menu_y", STEAMCONTROLLER_Y, 0, { false } },
	{ "menu_lb", STEAMCONTROLLER_LEFT_BUMPER, 0, { false } },
	{ "menu_rb", STEAMCONTROLLER_RIGHT_BUMPER, 0, { false } },
	{ "pause_menu", STEAMCONTROLLER_START, 0, { false } },
	{ "menu_4", STEAMCONTROLLER_SELECT, 0, { false } },
};

struct SAnalogAction
{
	const char *strName;
	JoystickAxis_t joystickAxisX, joystickAxisY;
	InputAnalogActionHandle_t handle;
};

static SAnalogAction g_AnalogActions[] = {
	{ "RadialMenuStick", JOY_AXIS_X, JOY_AXIS_R, 0 },
};

struct SGameActionSet
{
	const char *strName;
	GameActionSet_t eGameActionSet;
	InputActionSetHandle_t handle;
};

static SGameActionSet g_GameActionSets[] = {
	{ "GameControls", GAME_ACTION_SET_GAMECONTROLS, 0 },
	{ "VehicleControls", GAME_ACTION_SET_VEHICLECONTROLS, 0 },
	{ "MenuControls", GAME_ACTION_SET_MENUCONTROLS, 0 },
};

bool g_bInitialized = false;

static bool InitDigitalMenuActions()
{
	for ( int i = 0; i != ARRAYSIZE( g_GameActionSets ); ++i )
	{
		g_GameActionSets[i].handle = SteamInput()->GetActionSetHandle( g_GameActionSets[i].strName );
	}

	for ( int i = 0; i != ARRAYSIZE( g_DigitalMenuActions ); ++i )
	{
		g_DigitalMenuActions[i].handle = SteamInput()->GetDigitalActionHandle( g_DigitalMenuActions[i].strName );

		// Init the button state array 
		for ( int j = 0; j < STEAM_INPUT_MAX_COUNT; j++ )
		{
			g_DigitalMenuActions[i].bState[j] = false;
		}
	}

	for ( int i = 0; i != ARRAYSIZE( g_AnalogActions ); ++i )
	{
		g_AnalogActions[i].handle = SteamInput()->GetDigitalActionHandle( g_AnalogActions[i].strName );
	}

	return g_GameActionSets[GAME_ACTION_SET_GAMECONTROLS].handle && g_GameActionSets[GAME_ACTION_SET_MENUCONTROLS].handle;
}

//-----------------------------------------------------------------------------
// Purpose: Steam reloaded the controller bindings; re-fetch the action handles
//-----------------------------------------------------------------------------
void CInputSystem::OnSteamInputConfigurationLoaded( SteamInputConfigurationLoaded_t *pParam )
{
	if ( m_hConfigurationDevice == pParam->m_ulDeviceHandle &&
		 m_unConfigurationMajorRevision == pParam->m_unMajorRevision &&
		 m_unConfigurationMinorRevision == pParam->m_unMinorRevision )
	{
		return;
	}

	InitDigitalMenuActions();

	for ( int i = 0; i != ARRAYSIZE( g_GameActionSets ); ++i )
	{
		if ( !g_GameActionSets[i].handle )
		{
			return;
		}
	}

	m_hConfigurationDevice = pParam->m_ulDeviceHandle;
	m_unConfigurationMajorRevision = pParam->m_unMajorRevision;
	m_unConfigurationMinorRevision = pParam->m_unMinorRevision;
}

ConVar sc_debug_sets( "sc_debug_sets", "0", FCVAR_ARCHIVE, "Debugging" );

void CInputSystem::PollSteamControllers( void )
{
	// Make sure we've got the appropriate connections to Steam
	if ( !SteamInput() )
	{
		return;
	}

	SteamInput()->RunFrame();

	InputHandle_t nControllerHandles[STEAM_INPUT_MAX_COUNT];
	m_unNumConnected = SteamInput()->GetConnectedControllers( nControllerHandles );

	// Forget the active controller if it was unplugged
	if ( m_hActiveSteamInput )
	{
		bool bFound = false;
		for ( uint32 i = 0; i < m_unNumConnected; ++i )
		{
			if ( nControllerHandles[i] == m_hActiveSteamInput )
			{
				bFound = true;
				break;
			}
		}

		if ( !bFound )
		{
			m_hActiveSteamInput = 0;
		}
	}

	if ( m_unNumConnected > 0 && !g_bInitialized )
	{
		if ( InitDigitalMenuActions() )
		{
			g_pInputSystem->ActivateSteamControllerActionSetForSlot( STEAM_INPUT_HANDLE_ALL_CONTROLLERS, GAME_ACTION_SET_MENUCONTROLS );
			m_hActiveSteamInput = 0;
			g_bInitialized = true;
		}
	}

	if ( m_unNumConnected <= 0 || !g_bInitialized )
	{
		return;
	}

	// For each digital action
	for ( int i = 0; i != ARRAYSIZE( g_DigitalMenuActions ); ++i )
	{
		SDigitalMenuAction& action = g_DigitalMenuActions[i];

		// and for each controller
		for ( uint64 j = 0; j < m_unNumConnected; ++j )
		{
			// Get the action's current state
			InputDigitalActionData_t data = SteamInput()->GetDigitalActionData( nControllerHandles[j], action.handle );

			// We only care if the action is active
			if ( data.bActive )
			{
				// If the action's state has changed
				if ( data.bState != action.bState[j] )
				{
					action.bState[j] = data.bState;
					m_eActiveInputType = ACTIVE_INPUT_TYPE_STEAM_INPUT;

					// Press the key for the correct controller
					ButtonCode_t buttonCode = ButtonCodeToJoystickButtonCode( action.buttonCode, j );

					if ( action.bState[j] )
					{
						PostButtonPressedEvent( IE_ButtonPressed, m_nLastSampleTick, buttonCode, buttonCode );
						m_hActiveSteamInput = nControllerHandles[j];
					}
					else
					{
						PostButtonReleasedEvent( IE_ButtonReleased, m_nLastSampleTick, buttonCode, buttonCode );
					}
				}
			}
		}
	}

	// Radial menu stick, per controller
	for ( uint64 i = 0; i < m_unNumConnected; ++i )
	{
		InputAnalogActionData_t data = SteamInput()->GetAnalogActionData( nControllerHandles[i], g_AnalogActions[0].handle );
		if ( data.bActive )
		{
			m_pRadialMenuStickVal[i][0] = data.x;
			m_pRadialMenuStickVal[i][1] = data.y;

			if ( abs( data.x ) > 2 || abs( data.y ) > 2 )
			{
				m_eActiveInputType = ACTIVE_INPUT_TYPE_STEAM_INPUT;
			}
		}
		else
		{
			m_pRadialMenuStickVal[i][0] = 0.0f;
			m_pRadialMenuStickVal[i][1] = 0.0f;
		}
	}
}

bool CInputSystem::GetRadialMenuStickValues( int nSlot, float &fX, float &fY )
{
	fX = m_pRadialMenuStickVal[nSlot][0];
	fY = m_pRadialMenuStickVal[nSlot][1];

	return true;
}

bool CInputSystem::IsSteamControllerActive( void )
{
	return m_eActiveInputType == ACTIVE_INPUT_TYPE_STEAM_INPUT;
}

bool CInputSystem::IsXinputControllerActive( void )
{
	return m_eActiveInputType == ACTIVE_INPUT_TYPE_XINPUT;
}

bool CInputSystem::IsMouseKeyboardActive( void )
{
	return m_eActiveInputType == ACTIVE_INPUT_TYPE_MOUSE_KEYBOARD;
}

bool CInputSystem::InitializeSteamControllers()
{
	m_flLastSteamControllerInput = -FLT_MAX;

	SteamAPI_Init();
	if ( SteamInput() )
	{
		SteamInput()->Init( true );
	}

	if ( !m_bOnDiscManifestLoaded )
	{
		m_bOnDiscManifestLoaded = BLoadOnDiscManifest();
	}

	for( int i=0; i<STEAM_INPUT_MAX_COUNT; i++ )
	{
		m_pRadialMenuStickVal[i][0] = 0.0f;
		m_pRadialMenuStickVal[i][1] = 0.0f;
	}
	
	// We have to account for other joysticks prior to adding steam controllers
	// So we get the baseline number here when first initializing
	m_nJoystickBaseline = m_nJoystickCount;
	if ( SteamInput() )
	{
		InputHandle_t nControllerHandles[STEAM_INPUT_MAX_COUNT];
		m_unNumConnected = SteamInput()->GetConnectedControllers( nControllerHandles );

		if ( m_unNumConnected > 0 )
		{
			for ( uint32 i = 0; i < m_unNumConnected; i++ )
			{
				if ( m_Device[i].m_nJoystickIndex == INVALID_USER_ID )
				{
					int nJoystickIndex = i;
					m_Device[i].m_nJoystickIndex = nJoystickIndex;
					m_Device[i].m_nHardwareIndex = i;
				}
				m_nControllerType[m_Device[i].m_nJoystickIndex] = INPUT_TYPE_STEAMCONTROLLER;
			}

			SteamInput()->RunFrame();
			return true;
		}
	}

	return false;	
}

bool CInputSystem::BLoadOnDiscManifest()
{
	if ( !SteamInput() )
	{
		return false;
	}

	char szCurrentDir[MAX_PATH + 1];
	if ( !g_pFullFileSystem->GetCurrentDirectory( szCurrentDir, sizeof( szCurrentDir ) ) )
	{
		Msg( "Couldn't get current directory.\n" );
	}
	V_StripTrailingSlash( szCurrentDir );

	const char *pszActionManifest = "steam_input\\action_manifest_hl2.vdf";
	char szManifestPath[MAX_PATH + 1];
	V_snprintf( szManifestPath, sizeof( szManifestPath ), "%s\\%s", szCurrentDir, pszActionManifest );
	Msg( "Loading action manifest: %s\n", szManifestPath );

	if ( g_pFullFileSystem->FileExists( szManifestPath ) )
	{
		Msg( "[SteamInput] Loading action manifest from console: %s\n", szManifestPath );
		SteamInput()->SetInputActionManifestFilePath( szManifestPath );
		if ( SteamInput()->SetInputActionManifestFilePath( szManifestPath ) )
		{
			DevMsg( "- [SteamInput] Local IGA file exists for this Mod. \"%s\".\n It will be used for canonical inputs instead of the Steam Workshop.\n Be sure to set \"Custom Configuration (Bundled with Game)\" on Steam Partner site for this App ID.\n", szManifestPath );
			return true;
		}
		else
		{
			DevMsg( "- [SteamInput] Local IGA file exists for this Mod \"%s\".\n, but it could not be parsed successfully.]\n", szManifestPath );
			return false;
		}
	}
	else
	{
		DevMsg( "- [SteamInput ]Local IGA file does not exist for this Mod\"%s\".\n. Partner site should be set to \"Custom Configuration (Bundled with Game)\" for this app for cloud settings to work.\n", szManifestPath );
		return false;
	}
}

ControllerActionSetHandle_t CInputSystem::GetActionSetHandle( GameActionSet_t eActionSet )
{
	return g_GameActionSets[eActionSet].handle;
}

void CInputSystem::ActivateSteamControllerActionSetForSlot( uint64 nSlot, GameActionSet_t eActionSet )
{
	if ( !SteamInput() )
	{
		return;
	}

	InputActionSetHandle_t hActionSet = g_GameActionSets[eActionSet].handle;

	InputHandle_t nControllerHandles[STEAM_INPUT_MAX_COUNT];
	int nNumConnected = SteamInput()->GetConnectedControllers( nControllerHandles );

	if ( nSlot == STEAM_INPUT_HANDLE_ALL_CONTROLLERS || nSlot >= nNumConnected )
	{
		SteamInput()->ActivateActionSet( STEAM_INPUT_HANDLE_ALL_CONTROLLERS, g_GameActionSets[eActionSet].handle );
	}
	else
	{
		InputHandle_t hController = nControllerHandles[nSlot];
		if ( SteamInput()->GetCurrentActionSet( hController ) != hActionSet )
		{
			SteamInput()->ActivateActionSet( hController, g_GameActionSets[eActionSet].handle );
		}
	}
}

void CInputSystem::SetSteamInputRumble( float fLeftMotor, float fRightMotor, int userId )
{
	if ( userId >= STEAM_INPUT_MAX_COUNT )
	{
		return;
	}

#ifndef BYTEMATCH_RETAIL
	// stub_steam has no Steam Input
	if ( !SteamInput() )
		return;
#else



#endif

	// Only stop rumble on a controller the player isn't using
	if ( m_eActiveInputType != ACTIVE_INPUT_TYPE_STEAM_INPUT && ( fLeftMotor != 0.0f || fRightMotor != 0.0f ) )
	{
		return;
	}

	if ( m_hActiveSteamInput )
	{
		SteamInput()->TriggerVibration( m_hActiveSteamInput, (unsigned short)( fLeftMotor * 32767.0f ), (unsigned short)( fRightMotor * 32767.0f ) );
		return;
	}

	InputHandle_t hController = STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	if ( userId > 0 )
	{
		InputHandle_t nControllerHandles[STEAM_INPUT_MAX_COUNT];
		SteamInput()->GetConnectedControllers( nControllerHandles );
		hController = nControllerHandles[userId];
		if ( !hController )
		{
			return;
		}
	}

	SteamInput()->TriggerVibration( hController, (unsigned short)( fLeftMotor * 32767.0f ), (unsigned short)( fRightMotor * 32767.0f ) );
}

//-----------------------------------------------------------------------------
// Purpose: Which physical input the menu action bound to a Steam Controller
//			button code maps to, for button glyphs
//-----------------------------------------------------------------------------
EControllerActionOrigin CInputSystem::GetActionOriginForSlotAndCode( int nSlot, ButtonCode_t code )
{
#ifndef BYTEMATCH_RETAIL
	// stub_steam has no Steam Input
	if ( !SteamInput() )
		return k_EControllerActionOrigin_None;
#else



#endif

	InputHandle_t hController;
	if ( nSlot == -1 && m_hActiveSteamInput )
	{
		hController = m_hActiveSteamInput;
	}
	else
	{
		InputHandle_t nControllerHandles[STEAM_INPUT_MAX_COUNT];
		SteamInput()->GetConnectedControllers( nControllerHandles );
		hController = nControllerHandles[0];
	}

	InputDigitalActionHandle_t hAction;
	switch ( code )
	{
	case STEAMCONTROLLER_A:
		hAction = g_DigitalMenuActions[5].handle;
		break;
	case STEAMCONTROLLER_B:
		hAction = g_DigitalMenuActions[4].handle;
		break;
	case STEAMCONTROLLER_X:
		hAction = g_DigitalMenuActions[6].handle;
		break;
	case STEAMCONTROLLER_Y:
		hAction = g_DigitalMenuActions[7].handle;
		break;
	case STEAMCONTROLLER_LEFT_BUMPER:
		hAction = g_DigitalMenuActions[9].handle;
		break;
	case STEAMCONTROLLER_SELECT:
		hAction = g_DigitalMenuActions[10].handle;
		break;
	case STEAMCONTROLLER_START:
		hAction = g_DigitalMenuActions[8].handle;
		break;
	default:
		return k_EControllerActionOrigin_None;
	}

	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS] = {};
	SteamInput()->GetDigitalActionOrigins( hController, g_pInputSystem->GetActionSetHandle( GAME_ACTION_SET_MENUCONTROLS ), hAction, origins );
	return (EControllerActionOrigin)origins[0];
}

//-----------------------------------------------------------------------------
//	Purpose: Pulse haptic feedback
//-----------------------------------------------------------------------------
void CInputSystem::PulseHapticOnSteamController( uint32 nControllerIndex, ESteamControllerPad ePad, unsigned short usDurationMicroSec )
{
	if ( SteamInput() )
	{
		SteamInput()->Legacy_TriggerHapticPulse( nControllerIndex, ePad, usDurationMicroSec );
	}
}

int CInputSystem::GetSteamControllerIndexForSlot( int nSlot )
{
	for ( int i = 0; i < Q_ARRAYSIZE( m_Device ); i++ )
	{
		if ( m_Device[i].active && (int)m_Device[i].m_nJoystickIndex == nSlot )
		{
			return m_Device[i].m_nHardwareIndex;
		}
	}
	return -1;
}
