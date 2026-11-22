//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Full-screen base frame for HL2 Anniversary GamepadUI panels.
//
//===========================================================================//

#include "gamepadui_frame.h"
#include "gamepadui_interface.h"

#include "tier2/tier2.h"
#include "inputsystem/iinputsystem.h"
#include "vgui/ISurface.h"
#include "vgui/IScheme.h"

#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor & destructor
//-----------------------------------------------------------------------------
GamepadUIFrame::GamepadUIFrame(
	vgui::Panel *pParent,
	const char *pszPanelName,
	bool bShowTaskbarIcon,
	bool bPopup )
	: BaseClass( pParent, pszPanelName, bShowTaskbarIcon, bPopup )
	, m_nFooterButtonsHideOnMouse( 0 )
	, m_colTitleColor( 255, 255, 255, 255 )
	, m_flTitleOffsetX( 0.0f )
	, m_flTitleOffsetY( 0.0f )
	, m_flFooterButtonsOffsetX( 0.0f )
	, m_flFooterButtonsOffsetY( 0.0f )
	, m_flFooterButtonsSpacing( 0.0f )
	, m_hTitleFont( vgui::INVALID_FONT )
	, m_hGenericFont( vgui::INVALID_FONT )
	, m_nFooterButtonWidth( 0 )
	, m_nFooterButtonHeight( 0 )
{
	V_memset( m_pFooterButtons, 0, sizeof( m_pFooterButtons ) );

	SetProportional( true );
	SetTitle( "", false );
}

GamepadUIFrame::~GamepadUIFrame()
{
}

//-----------------------------------------------------------------------------
// Purpose: ApplySchemeSettings
//-----------------------------------------------------------------------------
void GamepadUIFrame::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetMoveable( false );
	SetTitleBarVisible( false );
	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );
	SetMenuButtonResponsive( false );
	SetMenuButtonVisible( false );
	SetSizeable( false );
	SetDeleteSelfOnClose( true );
	SetPaintBackgroundEnabled( false );
	SetPaintBorderEnabled( false );
	SetCloseButtonVisible( false );

	if ( vgui::Panel *pParent = GetParent() )
	{
		int nWide = 0, nTall = 0;
		pParent->GetSize( nWide, nTall );
		SetBounds( 0, 0, nWide, nTall );
	}

	ApplySchemeValues( this, pScheme );

	m_hTitleFont = pScheme->GetFont( "Title.Font", true );
	m_hGenericFont = pScheme->GetFont( "Generic.Text.Font", true );
}

//-----------------------------------------------------------------------------
// Purpose: Virtual hook for panel-specific backdrop gradients
//-----------------------------------------------------------------------------
void GamepadUIFrame::UpdateGradients()
{
}

//-----------------------------------------------------------------------------
// Purpose: Restore parent or main menu gradients when this frame closes
//-----------------------------------------------------------------------------
void GamepadUIFrame::OnClose()
{
	BaseClass::OnClose();

	if ( vgui::Panel *pParent = GetParent() )
	{
		if ( GamepadUIFrame *pParentFrame = dynamic_cast<GamepadUIFrame *>( pParent ) )
		{
			pParentFrame->UpdateGradients();
			return;
		}
	}

	GamepadUI::GetInstance().UpdateGradients();
}

//-----------------------------------------------------------------------------
// Purpose: Footer button creation & layout
//-----------------------------------------------------------------------------
void GamepadUIFrame::SetFooterButtons( int nButtonMask, int nMouseHideMask )
{
	for ( int i = 0; i < FooterButtons::MaxButtons; ++i )
	{
		if ( m_pFooterButtons[i] )
		{
			delete m_pFooterButtons[i];
			m_pFooterButtons[i] = NULL;
		}

		int nBit = ( 1 << i );
		if ( ( nButtonMask & nBit ) == 0 )
			continue;

		const char *pszCommand = FooterButtons::GetButtonCommand( nBit );
		const char *pszText = FooterButtons::GetButtonText( nBit );

		GamepadUIButton *pButton = new GamepadUIButton(
			this,
			this,
			"gamepadui/schemepanel.res",
			pszCommand,
			pszText,
			"" );

		m_pFooterButtons[i] = pButton;
		pButton->SetFooterButton( nBit );
		pButton->SetZPos( 100 );
	}

	m_nFooterButtonsHideOnMouse = nMouseHideMask;
}

void GamepadUIFrame::LayoutFooterButtons()
{
	int nParentW = 0, nParentH = 0;
	if ( vgui::Panel *pParent = GetParent() )
	{
		pParent->GetSize( nParentW, nParentH );
	}
	else
	{
		GetSize( nParentW, nParentH );
	}

	bool bControllerActive = g_pInputSystem && g_pInputSystem->IsSteamControllerActive();
	int nLeftAccum = 0;
	int nRightAccum = 0;

	for ( int i = 0; i < FooterButtons::MaxButtons; ++i )
	{
		GamepadUIButton *pButton = m_pFooterButtons[i];
		if ( !pButton )
			continue;

		int nBit = ( 1 << i );
		bool bShouldShow = bControllerActive || ( ( m_nFooterButtonsHideOnMouse & nBit ) == 0 );
		pButton->SetVisible( bShouldShow );
		if ( !bShouldShow )
			continue;

		int nBtnW = pButton->GetWide();
		int nBtnH = pButton->GetTall();
		m_nFooterButtonWidth = nBtnW;
		m_nFooterButtonHeight = nBtnH;

		int nPosY = static_cast<int>( static_cast<float>( nParentH ) - m_flFooterButtonsOffsetY - static_cast<float>( nBtnH ) );

		if ( ( nBit & FooterButtons::LeftMask ) != 0 )
		{
			int nPosX = static_cast<int>( static_cast<float>( nLeftAccum ) + m_flFooterButtonsOffsetX );
			pButton->SetPos( nPosX, nPosY );
			nLeftAccum = static_cast<int>( static_cast<float>( nLeftAccum ) + m_flFooterButtonsSpacing + static_cast<float>( nBtnW ) );
		}
		else
		{
			int nPosX = static_cast<int>( static_cast<float>( nParentW - nBtnW - nRightAccum ) - m_flFooterButtonsOffsetX );
			pButton->SetPos( nPosX, nPosY );
			nRightAccum = static_cast<int>( static_cast<float>( nRightAccum ) + m_flFooterButtonsSpacing + static_cast<float>( nBtnW ) );
		}
	}
}

GamepadUIButton *GamepadUIFrame::GetFooterButton( FooterButtons::Button eButton ) const
{
	for ( int i = 0; i < FooterButtons::MaxButtons; ++i )
	{
		if ( ( 1 << i ) == static_cast<int>( eButton ) )
			return m_pFooterButtons[i];
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Per-frame think
//-----------------------------------------------------------------------------
void GamepadUIFrame::OnThink()
{
	BaseClass::OnThink();
	LayoutFooterButtons();
}

//-----------------------------------------------------------------------------
// Purpose: Footer button key dispatch (matching retail bitmasks)
//-----------------------------------------------------------------------------
void GamepadUIFrame::TriggerFooterButtonsPressed( int nMask )
{
	for ( int i = 0; i < FooterButtons::MaxButtons; ++i )
	{
		int nBit = ( 1 << i );
		if ( ( nBit & nMask ) != 0 && m_pFooterButtons[i] )
		{
			m_pFooterButtons[i]->ForceDepressed( true );
		}
	}
}

void GamepadUIFrame::TriggerFooterButtonsReleased( int nMask )
{
	for ( int i = 0; i < FooterButtons::MaxButtons; ++i )
	{
		int nBit = ( 1 << i );
		if ( ( nBit & nMask ) != 0 && m_pFooterButtons[i] && m_pFooterButtons[i]->IsDepressed() )
		{
			m_pFooterButtons[i]->ForceDepressed( false );
			m_pFooterButtons[i]->FireActionSignal();
		}
	}
}

void GamepadUIFrame::OnKeyCodePressed( vgui::KeyCode code )
{
	ButtonCode_t norm = GamepadUI_NormalizeJoystickCode( code );

	// A / Enter -> Select | SelectRight | Okay (0x2c)
	if ( norm == KEY_XBUTTON_A || norm == KEY_ENTER || norm == STEAMCONTROLLER_A )
	{
		TriggerFooterButtonsPressed( FooterButtons::Select | FooterButtons::SelectRight | FooterButtons::Okay );
		return;
	}

	// B / Escape -> Back | Cancel (0x03)
	if ( norm == KEY_XBUTTON_B || norm == KEY_ESCAPE || norm == STEAMCONTROLLER_B )
	{
		TriggerFooterButtonsPressed( FooterButtons::Back | FooterButtons::Cancel );
		return;
	}

	// X -> BonusMaps (0x80)
	if ( norm == KEY_XBUTTON_X || norm == STEAMCONTROLLER_X )
	{
		TriggerFooterButtonsPressed( FooterButtons::BonusMaps );
		return;
	}

	// Y -> Apply | Commentary | Challenges (0x150)
	if ( norm == KEY_XBUTTON_Y || norm == STEAMCONTROLLER_Y )
	{
		TriggerFooterButtonsPressed( FooterButtons::Apply | FooterButtons::Commentary | FooterButtons::Challenges );
		return;
	}

	BaseClass::OnKeyCodePressed( code );
}

void GamepadUIFrame::OnKeyCodeReleased( vgui::KeyCode code )
{
	ButtonCode_t norm = GamepadUI_NormalizeJoystickCode( code );

	if ( norm == KEY_XBUTTON_A || norm == KEY_ENTER || norm == STEAMCONTROLLER_A )
	{
		TriggerFooterButtonsReleased( FooterButtons::Select | FooterButtons::SelectRight | FooterButtons::Okay );
		return;
	}

	if ( norm == KEY_XBUTTON_B || norm == KEY_ESCAPE || norm == STEAMCONTROLLER_B )
	{
		TriggerFooterButtonsReleased( FooterButtons::Back | FooterButtons::Cancel );
		return;
	}

	if ( norm == KEY_XBUTTON_X || norm == STEAMCONTROLLER_X )
	{
		TriggerFooterButtonsReleased( FooterButtons::BonusMaps );
		return;
	}

	if ( norm == KEY_XBUTTON_Y || norm == STEAMCONTROLLER_Y )
	{
		TriggerFooterButtonsReleased( FooterButtons::Apply | FooterButtons::Commentary | FooterButtons::Challenges );
		return;
	}

	BaseClass::OnKeyCodeReleased( code );
}

//-----------------------------------------------------------------------------
// Purpose: Painting
//-----------------------------------------------------------------------------
void GamepadUIFrame::PaintGradients()
{
	GamepadUI &ui = GamepadUI::GetInstance();
	ui.GetGradientHelper().PaintGradients( GetWide(), GetTall(), ui.GetTime() );
}

void GamepadUIFrame::PaintTitle()
{
	if ( m_strTitle.IsEmpty() || !vgui::surface() )
		return;

	vgui::surface()->DrawSetTextColor( m_colTitleColor );
	vgui::surface()->DrawSetTextFont( m_hTitleFont );
	vgui::surface()->DrawSetTextPos( static_cast<int>( m_flTitleOffsetX ), static_cast<int>( m_flTitleOffsetY ) );
	vgui::surface()->DrawPrintText( m_strTitle.String(), m_strTitle.Length() );
}

void GamepadUIFrame::Paint()
{
	BaseClass::Paint();
	PaintGradients();
	if ( !m_strTitle.IsEmpty() )
	{
		PaintTitle();
	}
}

void GamepadUIFrame::SetTitleText( const char *pszTitle )
{
	m_strTitle.SetString( pszTitle );
}

void GamepadUIFrame::SetTitleText( const wchar_t *pwszTitle )
{
	m_strTitle.SetString( pwszTitle );
}
