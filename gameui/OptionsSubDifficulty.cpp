//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "OptionsSubDifficulty.h"
#include "tier1/convar.h"
#include "EngineInterface.h"
#include "tier1/KeyValues.h"
#include "ModInfo.h"

#include "vgui_controls/RadioButton.h"
#include "vgui_controls/Button.h"

using namespace vgui;


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
COptionsSubDifficulty::COptionsSubDifficulty(vgui::Panel *parent) : BaseClass(parent, NULL)
{
	m_pEasyRadio = new RadioButton(this, "Skill1Radio", "#GameUI_SkillEasy");
	m_pNormalRadio = new RadioButton(this, "Skill2Radio", "#GameUI_SkillNormal");
	m_pHardRadio = new RadioButton(this, "Skill3Radio", "#GameUI_SkillHard");

	m_pMigrateButton = new Button( this, "MigrateButton", "#GameUI_MigrateEpisodicData" );
	m_pMigrateButton->SizeToContents();
	m_pMigrateButton->SetEnabled( false );
	m_pMigrateButton->SetVisible( false );

	LoadControlSettings("Resource/OptionsSubDifficulty.res");

	m_pMigrateButton->SetEnabled( false );
	m_pMigrateButton->SetVisible( false );
}

//-----------------------------------------------------------------------------
// Purpose: resets controls
//-----------------------------------------------------------------------------
void COptionsSubDifficulty::OnResetData()
{
	ConVarRef var( "skill" );

	if (var.GetInt() == 1)
	{
		m_pEasyRadio->SetSelected(true);
	}
	else if (var.GetInt() == 3)
	{
		m_pHardRadio->SetSelected(true);
	}
	else
	{
		m_pNormalRadio->SetSelected(true);
	}

	if ( m_pMigrateButton )
	{
		// episodic data migration is not offered; the button stays hidden
		ModInfo();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubDifficulty::OnCommand( const char *command )
{
	if ( ModInfo().IsHL2Complete() && !stricmp( command, "MigrateData" ) )
	{
		engine->ClientCmd_Unrestricted( "migrate_episodic_data" );
	}

	BaseClass::OnCommand( command );
}

//-----------------------------------------------------------------------------
// Purpose: sets data based on control settings
//-----------------------------------------------------------------------------
void COptionsSubDifficulty::OnApplyChanges()
{
	ConVarRef var( "skill" );

	if ( m_pEasyRadio->IsSelected() )
	{
		var.SetValue( 1 );
	}
	else if ( m_pHardRadio->IsSelected() )
	{
		var.SetValue( 3 );
	}
	else
	{
		var.SetValue( 2 );
	}
}


//-----------------------------------------------------------------------------
// Purpose: enables apply button on radio buttons being pressed
//-----------------------------------------------------------------------------
void COptionsSubDifficulty::OnRadioButtonChecked()
{
	PostActionSignal(new KeyValues("ApplyButtonEnable"));
}
