//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Generic confirmation modal frame for HL2 Anniversary GamepadUI.
//
//===========================================================================//

#include "gamepadui_genericconfirmation.h"
#include "gamepadui_interface.h"
#include "gamepadui_basepanel.h"

#include "tier1/convar.h"
#include "cdll_int.h"
#include "vgui/ISurface.h"
#include "vgui/IScheme.h"
#include "vgui_controls/Controls.h"

#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructors & destructor
//-----------------------------------------------------------------------------
GamepadUIGenericConfirmationPanel::GamepadUIGenericConfirmationPanel(
	vgui::Panel *pParent,
	const char *pszPanelName,
	const char *pszTitle,
	const char *pszText,
	std::function<void()> fnCallback,
	bool bUseGenericFont,
	bool bShowCancel )
	: BaseClass( pParent, pszPanelName, true, true )
	, m_fnCallback( std::move( fnCallback ) )
	, m_colGenericConfirmationColor( 255, 255, 255, 255 )
	, m_flGenericConfirmationOffsetX( 0.0f )
	, m_flGenericConfirmationOffsetY( 0.0f )
	, m_hConfirmationFont( vgui::INVALID_FONT )
	, m_pszFontName( bUseGenericFont ? "Generic.Text.Font" : "GenericConfirmation.Font" )
{
	InitCommon( bUseGenericFont, bShowCancel );
	SetTitleText( pszTitle );
	m_strText.SetString( pszText );
}

GamepadUIGenericConfirmationPanel::GamepadUIGenericConfirmationPanel(
	vgui::Panel *pParent,
	const char *pszPanelName,
	const wchar_t *pwszTitle,
	const wchar_t *pwszText,
	std::function<void()> fnCallback,
	bool bUseGenericFont,
	bool bShowCancel )
	: BaseClass( pParent, pszPanelName, true, true )
	, m_fnCallback( std::move( fnCallback ) )
	, m_colGenericConfirmationColor( 255, 255, 255, 255 )
	, m_flGenericConfirmationOffsetX( 0.0f )
	, m_flGenericConfirmationOffsetY( 0.0f )
	, m_hConfirmationFont( vgui::INVALID_FONT )
	, m_pszFontName( bUseGenericFont ? "Generic.Text.Font" : "GenericConfirmation.Font" )
{
	InitCommon( bUseGenericFont, bShowCancel );
	SetTitleText( pwszTitle );
	m_strText.SetString( pwszText );
}

GamepadUIGenericConfirmationPanel::~GamepadUIGenericConfirmationPanel()
{
}

void GamepadUIGenericConfirmationPanel::InitCommon( bool bUseGenericFont, bool bShowCancel )
{
	m_pszFontName = bUseGenericFont ? "Generic.Text.Font" : "GenericConfirmation.Font";

	if ( vgui::scheme() )
	{
		vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile( "gamepadui/schemepanel.res", "SchemePanel" );
		SetScheme( hScheme );
	}

	int nFooterMask = FooterButtons::Okay;
	if ( bShowCancel )
	{
		nFooterMask |= FooterButtons::Cancel;
	}
	SetFooterButtons( nFooterMask );

	Activate();
	UpdateGradients();
}

//-----------------------------------------------------------------------------
// Purpose: ApplySchemeSettings
//-----------------------------------------------------------------------------
void GamepadUIGenericConfirmationPanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	m_hConfirmationFont = pScheme->GetFont( m_pszFontName, true );
}

//-----------------------------------------------------------------------------
// Purpose: Command dispatch
//-----------------------------------------------------------------------------
void GamepadUIGenericConfirmationPanel::OnCommand( const char *pszCommand )
{
	if ( !strcmp( pszCommand, "action_cancel" ) )
	{
		Close();
	}
	else if ( !strcmp( pszCommand, "action_okay" ) )
	{
		m_fnCallback();
		Close();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Paint wrapped confirmation body text
//-----------------------------------------------------------------------------
void GamepadUIGenericConfirmationPanel::PaintText()
{
	vgui::surface()->DrawSetTextColor( m_colGenericConfirmationColor );
	vgui::surface()->DrawSetTextFont( m_hConfirmationFont );

	int nX = static_cast<int>( m_flGenericConfirmationOffsetX );
	int nY = static_cast<int>( m_flGenericConfirmationOffsetY );
	vgui::surface()->DrawSetTextPos( nX, nY );

	int nMaxWidth = static_cast<int>( static_cast<float>( GetWide() ) - ( m_flGenericConfirmationOffsetX * 2.0f ) );
	GamepadUI_DrawWrappedText(
		m_hConfirmationFont,
		nX,
		nY,
		m_strText.String(),
		m_strText.Length(),
		nMaxWidth,
		true );
}

void GamepadUIGenericConfirmationPanel::Paint()
{
	BaseClass::Paint();
	if ( !m_strText.IsEmpty() )
	{
		PaintText();
	}
	UpdateGradients();
}

void GamepadUIGenericConfirmationPanel::UpdateGradients()
{
	GamepadUI &ui = GamepadUI::GetInstance();
	float flTime = ui.GetTime();
	GamepadUIGradientHelper &grad = ui.GetGradientHelper();

	grad.ResetTargets( flTime );
	grad.SetTargetGradient( GamepadUIGradientSides::Up, { 1.0f, 1.0f }, flTime );
	grad.SetTargetGradient( GamepadUIGradientSides::Left, { 1.0f, 1.0f }, flTime );
}

//-----------------------------------------------------------------------------
// Retail ConCommands for Quit & Return to Main Menu confirmation dialogs
//-----------------------------------------------------------------------------
CON_COMMAND( gamepadui_openquitgamedialog, "" )
{
	new GamepadUIGenericConfirmationPanel(
		GamepadUI::GetInstance().GetBasePanel(),
		"QuitConfirmationPanel",
		"#GameUI_QuitConfirmationTitle",
		"#GameUI_QuitConfirmationText",
		[]()
		{
			GamepadUI::GetInstance().GetEngineClient()->ClientCmd_Unrestricted( "quit" );
		},
		false,
		true );
}

CON_COMMAND( gamepadui_openmainmenudialog, "" )
{
	new GamepadUIGenericConfirmationPanel(
		GamepadUI::GetInstance().GetBasePanel(),
		"QuitConfirmationPanel",
		"#GameUI_ConfirmMainMenu_Title",
		"#GameUI_MainMenuWarning",
		[]()
		{
			GamepadUI::GetInstance().GetEngineClient()->ClientCmd_Unrestricted( "startupmenu force" );
		},
		false,
		true );
}
