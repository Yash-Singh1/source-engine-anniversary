//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "OptionsSubGamepad.h"
#include "cvarslider.h"

#include "EngineInterface.h"

#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/Label.h>
#include "tier1/convar.h"
#include "inputsystem/iinputsystem.h"
#include "steam/steam_api.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

COptionsSubGamepad::COptionsSubGamepad( vgui::Panel *parent ) : PropertyPage( parent, NULL )
{
	m_pAutoaimCombo = new ComboBox( this, "Autoaim", 3, false );
	m_pAutoaimCombo->AddItem( "#GameUI_SettingOff", NULL );
	m_pAutoaimCombo->AddItem( "#GameUI_SettingBasic", NULL );
	m_pAutoaimCombo->AddItem( "#GameUI_SettingEnhanced", NULL );
	m_pAutoaimCombo->SetVisible( false );

	m_pAutoaimScaleSlider = new CCvarSlider( this, "AutoaimScale", "#GameUI_AimAssistScale", 0.5f, 2.0f, "sc_autoaim_scale" );
	m_pAutoaimScaleSlider->SetVisible( false );

	m_pJoyScaleSlider = new CCvarSlider( this, "JoyScale", "#GameUI_Gamepad_JoystickSensitivity", 0.5f, 2.0f, "sc_sensitivity_scale" );

	m_pWeaponSelectionCombo = new ComboBox( this, "WeaponSelection", 3, false );
	m_pWeaponSelectionCombo->AddItem( "#GameUI_Gamepad_WeaponSelection_Crossbar", NULL );
	m_pWeaponSelectionCombo->AddItem( "#GameUI_Gamepad_WeaponSelection_Carousel", NULL );
	m_pWeaponSelectionCombo->SetVisible( false );

	m_pAutoaimScaleLabel = new Label( this, "AutoaimScaleLabel", "#GameUI_AimAssistScale" );
	m_pAutoaimScaleLabel->SetVisible( false );

	m_pSteamInputButton = new Button( this, "SteamInput", "#GameUI_Open_SteamInput" );
	m_pSteamInputButton->SetVisible( false );

	LoadControlSettings( "Resource\\OptionsSubGamepad.res" );

	UpdateAutoaimControls();
}

void COptionsSubGamepad::UpdateAutoaimControls()
{
	if ( m_pAutoaimCombo->GetActiveItem() > 0 )
	{
		m_pAutoaimScaleLabel->SetEnabled( true );
		m_pAutoaimScaleSlider->SetEnabled( true );
	}
	else
	{
		m_pAutoaimScaleLabel->SetEnabled( false );
		m_pAutoaimScaleSlider->SetEnabled( false );
	}
}

COptionsSubGamepad::~COptionsSubGamepad()
{
}

void COptionsSubGamepad::OnResetData()
{
	int nAutoaimMode = 0;
	ConVarRef sc_autoaim_mode( "sc_autoaim_mode" );
	if ( sc_autoaim_mode.IsValid() )
	{
		nAutoaimMode = sc_autoaim_mode.GetInt();
	}
	m_pAutoaimCombo->ActivateItem( nAutoaimMode );
	if ( nAutoaimMode > 0 )
	{
		m_pAutoaimScaleSlider->SetEnabled( true );
	}
	else
	{
		m_pAutoaimScaleSlider->SetEnabled( false );
	}

	int nWeaponSelection = 2;
	ConVarRef hud_weaponui_gamepad( "hud_weaponui_gamepad" );
	if ( hud_weaponui_gamepad.IsValid() )
	{
		nWeaponSelection = hud_weaponui_gamepad.GetInt();
	}
	m_pWeaponSelectionCombo->ActivateItem( nWeaponSelection == 2 ? 0 : 1 );
}

void COptionsSubGamepad::OnApplyChanges()
{
	ConVarRef sc_autoaim_mode( "sc_autoaim_mode" );
	if ( sc_autoaim_mode.IsValid() )
	{
		sc_autoaim_mode.SetValue( m_pAutoaimCombo->GetActiveItem() );
	}

	ConVarRef hud_weaponui_gamepad( "hud_weaponui_gamepad" );
	if ( hud_weaponui_gamepad.IsValid() )
	{
		hud_weaponui_gamepad.SetValue( m_pWeaponSelectionCombo->GetActiveItem() ? 3 : 2 );
	}

	m_pAutoaimScaleSlider->ApplyChanges();
	m_pJoyScaleSlider->ApplyChanges();
}

void COptionsSubGamepad::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
}

void COptionsSubGamepad::OnCommand( const char *command )
{
	if ( !stricmp( command, "OpenSteamInput" ) )
	{
#ifdef STEAMINPUT_INTERFACE_VERSION
		InputHandle_t hInputHandle = g_pInputSystem->GetActiveSteamInputHandle();
		if ( !hInputHandle )
		{
			InputHandle_t inputHandles[ STEAM_INPUT_MAX_COUNT ];
			SteamInput()->GetConnectedControllers( inputHandles );
			hInputHandle = inputHandles[ 0 ];
		}

		SteamInput()->ShowBindingPanel( hInputHandle );
#endif
	}

	BaseClass::OnCommand( command );
}

void COptionsSubGamepad::OnControlModified( Panel *panel )
{
	PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );
}

void COptionsSubGamepad::OnTextChanged( Panel *panel )
{
	if ( panel == m_pAutoaimCombo )
	{
		if ( m_pAutoaimCombo->GetActiveItem() > 0 )
		{
			m_pAutoaimScaleLabel->SetEnabled( true );
			m_pAutoaimScaleSlider->SetEnabled( true );
		}
		else
		{
			m_pAutoaimScaleLabel->SetEnabled( false );
			m_pAutoaimScaleSlider->SetEnabled( false );
		}

		PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );
	}
}
