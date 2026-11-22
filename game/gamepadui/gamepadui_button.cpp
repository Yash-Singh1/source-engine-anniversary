//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Animated, scheme-driven GamepadUI button control.
//
//===========================================================================//

#include "gamepadui_button.h"
#include "gamepadui_interface.h"

#include <wctype.h>

#include "tier2/tier2.h"
#include "inputsystem/iinputsystem.h"
#include "vgui/ISurface.h"
#include "vgui/IScheme.h"
#include "vgui/IVGui.h"
#include "vgui_controls/AnimationController.h"

#include "tier0/memdbgon.h"

bool GamepadUIButton::s_bSuppressNav = false;

//-----------------------------------------------------------------------------
// Purpose: Constructors & destructor
//-----------------------------------------------------------------------------
GamepadUIButton::GamepadUIButton(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pszSchemeFile,
	const char *pszCommand,
	const char *pszText,
	const char *pszDescription )
	: BaseClass( pParent, "GamepadUIButton", "", pActionSignalTarget, pszCommand )
	, m_strButtonText( pszText )
	, m_strButtonDescription( pszDescription )
{
	InitCommon( pActionSignalTarget, pszSchemeFile, pszCommand );
}

GamepadUIButton::GamepadUIButton(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pszSchemeFile,
	const char *pszCommand,
	const wchar_t *pwszText,
	const wchar_t *pwszDescription )
	: BaseClass( pParent, "GamepadUIButton", L"", pActionSignalTarget, pszCommand )
	, m_strButtonText( pwszText )
	, m_strButtonDescription( pwszDescription )
{
	InitCommon( pActionSignalTarget, pszSchemeFile, pszCommand );
}

GamepadUIButton::~GamepadUIButton()
{
}

void GamepadUIButton::InitCommon( vgui::Panel *pActionSignalTarget, const char *pszSchemeFile, const char *pszCommand )
{
	(void)pActionSignalTarget;
	(void)pszCommand;

	m_ePreviousState = ButtonState::Out;
	m_bButtonAllCaps = false;
	m_bControllerDepressed = false;
	m_bNavigated = false;
	m_bForwardToParent = false;
	m_bAllowMouseNav = true;

	m_flExtraHeight = 0.0f;
	m_flWrappedDescriptionHeight = 0.0f;
	m_flExtraHeightTarget = 0.0f;
	m_flExtraHeightStart = 0.0f;
	m_flExtraHeightStartTime = 0.0f;

	m_nFooterButton = FooterButtons::None;
	m_nPriority = 0;

	m_CenterX = false;
	m_bDescriptionWrap = true;

	m_hTextFont = vgui::INVALID_FONT;
	m_hTextFontOver = vgui::INVALID_FONT;
	m_hDescriptionFont = vgui::INVALID_FONT;

	for ( int i = 0; i < ButtonState::Count; ++i )
	{
		m_flWidthState[i] = 392.0f;
		m_flHeightState[i] = 40.0f;
		m_flTextOffsetXState[i] = 10.0f;
		m_flTextOffsetYState[i] = 0.0f;
		m_flDescriptionOffsetXState[i] = 0.0f;
		m_flDescriptionOffsetYState[i] = 0.0f;
		m_colBackgroundColorState[i] = Color( 0, 0, 0, 0 );
		m_colTextColorState[i] = Color( 255, 255, 255, 255 );
		m_colDescriptionColorState[i] = Color( 255, 255, 255, 255 );
		m_flBackgroundBlurAmountState[i] = 0.0f;
		m_flGlyphFadeState[i] = 0.0f;
		m_bDescriptionHideState[i] = false;
		m_flTextLeftBorderState[i] = 0.0f;
		m_colLeftBorderState[i] = Color( 0, 0, 0, 0 );
		m_flTextBottomBorderState[i] = 0.0f;
		m_colBottomBorderState[i] = Color( 0, 0, 0, 0 );
	}

	m_flWidthAnimDuration = 0.2f;
	m_flHeightAnimDuration = 0.2f;
	m_flTextOffsetXAnimDuration = 0.2f;
	m_flTextOffsetYAnimDuration = 0.2f;
	m_flDescriptionOffsetXAnimDuration = 0.2f;
	m_flDescriptionOffsetYAnimDuration = 0.2f;
	m_flBackgroundAnimDuration = 0.2f;
	m_flTextAnimDuration = 0.2f;
	m_flDescriptionAnimDuration = 0.2f;
	m_flBackgroundBlurAnimDuration = 0.2f;
	m_flGlyphFadeAnimDuration = 0.2f;
	m_flDescriptionHideAnimDuration = 0.2f;
	m_flTextLeftBorderAnimDuration = 0.2f;
	m_flLeftBorderAnimDuration = 0.2f;
	m_flTextBottomBorderAnimDuration = 0.2f;
	m_flBottomBorderAnimDuration = 0.2f;

	RegisterSchemeValues();

	if ( pszSchemeFile && pszSchemeFile[0] && vgui::scheme() )
	{
		vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile( pszSchemeFile, "SchemePanel" );
		SetScheme( hScheme );
	}
}

void GamepadUIButton::RegisterSchemeValues()
{
	// Width
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flWidth, "Button.Width.Out", "392" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flWidthState[ButtonState::Out], "Button.Width.Out", "392" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flWidthState[ButtonState::Over], "Button.Width.Over", "392" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flWidthState[ButtonState::Pressed], "Button.Width.Pressed", "392" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flWidthAnimDuration, "Button.Width.Animation.Duration", "0.2" );

	// Height
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flHeight, "Button.Height.Out", "40" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flHeightState[ButtonState::Out], "Button.Height.Out", "40" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flHeightState[ButtonState::Over], "Button.Height.Over", "40" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flHeightState[ButtonState::Pressed], "Button.Height.Pressed", "40" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flHeightAnimDuration, "Button.Height.Animation.Duration", "0.2" );

	// Text OffsetX
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextOffsetX, "Button.Text.OffsetX.Out", "10" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextOffsetXState[ButtonState::Out], "Button.Text.OffsetX.Out", "10" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextOffsetXState[ButtonState::Over], "Button.Text.OffsetX.Over", "10" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextOffsetXState[ButtonState::Pressed], "Button.Text.OffsetX.Pressed", "10" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flTextOffsetXAnimDuration, "Button.Text.OffsetX.Animation.Duration", "0.2" );

	// Text OffsetY
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextOffsetY, "Button.Text.OffsetY.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextOffsetYState[ButtonState::Out], "Button.Text.OffsetY.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextOffsetYState[ButtonState::Over], "Button.Text.OffsetY.Over", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextOffsetYState[ButtonState::Pressed], "Button.Text.OffsetY.Pressed", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flTextOffsetYAnimDuration, "Button.Text.OffsetY.Animation.Duration", "0.2" );

	// Description OffsetX
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flDescriptionOffsetX, "Button.Description.OffsetX.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flDescriptionOffsetXState[ButtonState::Out], "Button.Description.OffsetX.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flDescriptionOffsetXState[ButtonState::Over], "Button.Description.OffsetX.Over", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flDescriptionOffsetXState[ButtonState::Pressed], "Button.Description.OffsetX.Pressed", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flDescriptionOffsetXAnimDuration, "Button.Description.OffsetX.Animation.Duration", "0.2" );

	// Description OffsetY
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flDescriptionOffsetY, "Button.Description.OffsetY.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flDescriptionOffsetYState[ButtonState::Out], "Button.Description.OffsetY.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flDescriptionOffsetYState[ButtonState::Over], "Button.Description.OffsetY.Over", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flDescriptionOffsetYState[ButtonState::Pressed], "Button.Description.OffsetY.Pressed", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flDescriptionOffsetYAnimDuration, "Button.Description.OffsetY.Animation.Duration", "0.2" );

	// Background Color
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colBackgroundColor, "Button.Background.Out", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colBackgroundColorState[ButtonState::Out], "Button.Background.Out", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colBackgroundColorState[ButtonState::Over], "Button.Background.Over", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colBackgroundColorState[ButtonState::Pressed], "Button.Background.Pressed", "0 0 0 0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flBackgroundAnimDuration, "Button.Background.Animation.Duration", "0.2" );

	// Text Color
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colTextColor, "Button.Text.Out", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colTextColorState[ButtonState::Out], "Button.Text.Out", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colTextColorState[ButtonState::Over], "Button.Text.Over", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colTextColorState[ButtonState::Pressed], "Button.Text.Pressed", "255 255 255 255" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flTextAnimDuration, "Button.Text.Animation.Duration", "0.2" );

	// Description Color
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colDescriptionColor, "Button.Description.Out", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colDescriptionColorState[ButtonState::Out], "Button.Description.Out", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colDescriptionColorState[ButtonState::Over], "Button.Description.Over", "255 255 255 255" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colDescriptionColorState[ButtonState::Pressed], "Button.Description.Pressed", "255 255 255 255" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flDescriptionAnimDuration, "Button.Description.Animation.Duration", "0.2" );

	// Background Blur
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flBackgroundBlurAmount, "Button.Background.Blur.Out", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flBackgroundBlurAmountState[ButtonState::Out], "Button.Background.Blur.Out", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flBackgroundBlurAmountState[ButtonState::Over], "Button.Background.Blur.Over", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flBackgroundBlurAmountState[ButtonState::Pressed], "Button.Background.Blur.Pressed", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flBackgroundBlurAnimDuration, "Button.Background.Blur.Animation.Duration", "0.2" );

	// Glyph Fade
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flGlyphFade, "Button.Glyphs.Fade.Out", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flGlyphFadeState[ButtonState::Out], "Button.Glyphs.Fade.Out", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flGlyphFadeState[ButtonState::Over], "Button.Glyphs.Fade.Over", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flGlyphFadeState[ButtonState::Pressed], "Button.Glyphs.Fade.Pressed", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flGlyphFadeAnimDuration, "Button.Glyphs.Fade.Animation.Duration", "0.2" );

	// Description Hide
	SCHEME_MEMBER_BOOL( GamepadUIButton, m_bDescriptionHide, "Button.Description.Hide.Out", "0" );
	SCHEME_MEMBER_BOOL( GamepadUIButton, m_bDescriptionHideState[ButtonState::Out], "Button.Description.Hide.Out", "0" );
	SCHEME_MEMBER_BOOL( GamepadUIButton, m_bDescriptionHideState[ButtonState::Over], "Button.Description.Hide.Over", "0" );
	SCHEME_MEMBER_BOOL( GamepadUIButton, m_bDescriptionHideState[ButtonState::Pressed], "Button.Description.Hide.Pressed", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flDescriptionHideAnimDuration, "Button.Description.Hide.Animation.Duration", "0.2" );

	// Left Border
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextLeftBorder, "Button.Text.LeftBorder.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextLeftBorderState[ButtonState::Out], "Button.Text.LeftBorder.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextLeftBorderState[ButtonState::Over], "Button.Text.LeftBorder.Over", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextLeftBorderState[ButtonState::Pressed], "Button.Text.LeftBorder.Pressed", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flTextLeftBorderAnimDuration, "Button.Text.LeftBorder.Animation.Duration", "0.2" );

	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colLeftBorder, "Button.Background.LeftBorder.Out", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colLeftBorderState[ButtonState::Out], "Button.Background.LeftBorder.Out", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colLeftBorderState[ButtonState::Over], "Button.Background.LeftBorder.Over", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colLeftBorderState[ButtonState::Pressed], "Button.Background.LeftBorder.Pressed", "0 0 0 0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flLeftBorderAnimDuration, "Button.Background.LeftBorder.Animation.Duration", "0.2" );

	// Bottom Border
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextBottomBorder, "Button.Text.BottomBorder.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextBottomBorderState[ButtonState::Out], "Button.Text.BottomBorder.Out", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextBottomBorderState[ButtonState::Over], "Button.Text.BottomBorder.Over", "0" );
	SCHEME_MEMBER_PROPORTIONAL( GamepadUIButton, m_flTextBottomBorderState[ButtonState::Pressed], "Button.Text.BottomBorder.Pressed", "0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flTextBottomBorderAnimDuration, "Button.Text.BottomBorder.Animation.Duration", "0.2" );

	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colBottomBorder, "Button.Background.BottomBorder.Out", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colBottomBorderState[ButtonState::Out], "Button.Background.BottomBorder.Out", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colBottomBorderState[ButtonState::Over], "Button.Background.BottomBorder.Over", "0 0 0 0" );
	SCHEME_MEMBER_COLOR( GamepadUIButton, m_colBottomBorderState[ButtonState::Pressed], "Button.Background.BottomBorder.Pressed", "0 0 0 0" );
	SCHEME_MEMBER_FLOAT( GamepadUIButton, m_flBottomBorderAnimDuration, "Button.Background.BottomBorder.Animation.Duration", "0.2" );
}

//-----------------------------------------------------------------------------
// Purpose: ApplySchemeSettings
//-----------------------------------------------------------------------------
void GamepadUIButton::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetArmedSound( "UI/buttonrollover.wav" );
	SetReleasedSound( "UI/buttonclickrelease.wav" );
	SetDepressedSound( NULL );
	SetPaintBorderEnabled( false );
	SetPaintBackgroundEnabled( false );
	SetProportional( true );

	ApplySchemeValues( this, pScheme );

	m_hTextFont = pScheme->GetFont( "Button.Text.Font", true );
	vgui::HFont hOverFont = pScheme->GetFont( "Button.Text.Font.Over", true );
	m_hTextFontOver = ( hOverFont != vgui::INVALID_FONT ) ? hOverFont : m_hTextFont;
	m_hDescriptionFont = pScheme->GetFont( "Button.Description.Font", true );

	m_ePreviousState = ButtonState::Out;
	m_flWrappedDescriptionHeight = 0.0f;

	if ( !m_strButtonDescription.IsEmpty() && m_bDescriptionWrap )
	{
		float flInset = ( m_flTextOffsetX + m_flDescriptionOffsetX ) * 2.0f;
		int nWrapWidth = static_cast<int>( m_flWidthState[ButtonState::Over] - flInset );
		if ( nWrapWidth > 0 )
		{
			m_flWrappedDescriptionHeight = static_cast<float>(
				GamepadUI_DrawWrappedText(
					m_hDescriptionFont,
					0,
					0,
					m_strButtonDescription.String(),
					m_strButtonDescription.Length(),
					nWrapWidth,
					false ) );
		}
	}

	SetSize( static_cast<int>( m_flWidth ), static_cast<int>( m_flHeight + m_flExtraHeight ) );
	DoAnimations( true );
}

//-----------------------------------------------------------------------------
// Purpose: Button state machine & animation triggers
//-----------------------------------------------------------------------------
ButtonState::State GamepadUIButton::GetCurrentButtonState()
{
	if ( IsDepressed() )
		return ButtonState::Pressed;

	if ( ( HasFocus() && IsEnabled() ) || IsArmed() || m_bNavigated )
	{
		if ( IsArmed() )
		{
			m_bNavigated = false;
		}
		return ButtonState::Over;
	}

	return ButtonState::Out;
}

void GamepadUIButton::RunAnimations( ButtonState::State eState )
{
	vgui::AnimationController *pAnim = GamepadUI::GetInstance().GetAnimationController();
	if ( !pAnim )
		return;

	pAnim->RunAnimationCommand( this, "m_flWidth", m_flWidthState[eState], 0.0f, m_flWidthAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_flHeight", m_flHeightState[eState], 0.0f, m_flHeightAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_flTextOffsetX", m_flTextOffsetXState[eState], 0.0f, m_flTextOffsetXAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_flTextOffsetY", m_flTextOffsetYState[eState], 0.0f, m_flTextOffsetYAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_flDescriptionOffsetX", m_flDescriptionOffsetXState[eState], 0.0f, m_flDescriptionOffsetXAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_flDescriptionOffsetY", m_flDescriptionOffsetYState[eState], 0.0f, m_flDescriptionOffsetYAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_colBackgroundColor", m_colBackgroundColorState[eState], 0.0f, m_flBackgroundAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_colTextColor", m_colTextColorState[eState], 0.0f, m_flTextAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_colDescriptionColor", m_colDescriptionColorState[eState], 0.0f, m_flDescriptionAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_flBackgroundBlurAmount", m_flBackgroundBlurAmountState[eState], 0.0f, m_flBackgroundBlurAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_flGlyphFade", m_flGlyphFadeState[eState], 0.0f, m_flGlyphFadeAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_bDescriptionHide", m_bDescriptionHideState[eState] ? 1.0f : 0.0f, 0.0f, m_flDescriptionHideAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_flTextLeftBorder", m_flTextLeftBorderState[eState], 0.0f, m_flTextLeftBorderAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_colLeftBorder", m_colLeftBorderState[eState], 0.0f, m_flLeftBorderAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_flTextBottomBorder", m_flTextBottomBorderState[eState], 0.0f, m_flTextBottomBorderAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	pAnim->RunAnimationCommand( this, "m_colBottomBorder", m_colBottomBorderState[eState], 0.0f, m_flBottomBorderAnimDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
}

void GamepadUIButton::DoAnimations( bool bForce )
{
	ButtonState::State eState = GetCurrentButtonState();
	if ( eState != m_ePreviousState || bForce )
	{
		RunAnimations( eState );
		m_ePreviousState = eState;
	}

	SetSize( static_cast<int>( m_flWidth ), static_cast<int>( m_flHeight + m_flExtraHeight ) );
}

void GamepadUIButton::OnThink()
{
	BaseClass::OnThink();
	DoAnimations( false );
}

//-----------------------------------------------------------------------------
// Purpose: Navigation & input handling
//-----------------------------------------------------------------------------
void GamepadUIButton::OnSiblingGamepadUIButtonOpened()
{
	if ( !s_bSuppressNav )
	{
		m_bNavigated = false;
	}
}

void GamepadUIButton::NavigateTo()
{
	BaseClass::NavigateTo();

	if ( m_nFooterButton != FooterButtons::None || s_bSuppressNav )
		return;

	if ( GetVParent() != 0 )
	{
		KeyValues *pMsg = new KeyValues( "OnGamepadUIButtonNavigatedTo" );
		pMsg->SetInt( "button", static_cast<int>( ToHandle() ) );
		vgui::ivgui()->PostMessage( GetVParent(), pMsg, GetVPanel() );
	}

	m_bNavigated = true;
	RequestFocus( 0 );
}

void GamepadUIButton::NavigateFrom()
{
	if ( !s_bSuppressNav )
	{
		BaseClass::NavigateFrom();
		m_bNavigated = false;
	}
}

void GamepadUIButton::OnCursorEntered()
{
	if ( g_pInputSystem && g_pInputSystem->IsSteamControllerActive() )
		return;

	if ( !IsEnabled() || s_bSuppressNav )
		return;

	BaseClass::OnCursorEntered();

	if ( m_nFooterButton == FooterButtons::None && m_bAllowMouseNav )
	{
		if ( vgui::Panel *pParent = GetParent() )
		{
			pParent->NavigateToChild( this );
		}
		else
		{
			NavigateTo();
		}
	}
}

void GamepadUIButton::OnKeyCodePressed( vgui::KeyCode code )
{
	if ( s_bSuppressNav )
		return;

	ButtonCode_t norm = GamepadUI_NormalizeJoystickCode( code );
	bool bIsConfirm = ( norm == KEY_XBUTTON_A || norm == KEY_ENTER || norm == STEAMCONTROLLER_A );

	if ( bIsConfirm )
	{
		if ( IsEnabled() )
		{
			ForceDepressed( true );
			m_bControllerDepressed = true;
		}
		BaseClass::OnKeyCodePressed( code );
		if ( m_bForwardToParent )
		{
			vgui::Panel::OnKeyCodePressed( code );
		}
		return;
	}

	if ( m_bControllerDepressed )
	{
		ForceDepressed( false );
		m_bControllerDepressed = false;
	}

	BaseClass::OnKeyCodePressed( code );
}

void GamepadUIButton::OnKeyCodeReleased( vgui::KeyCode code )
{
	if ( s_bSuppressNav )
		return;

	ButtonCode_t norm = GamepadUI_NormalizeJoystickCode( code );
	bool bIsConfirm = ( norm == KEY_XBUTTON_A || norm == KEY_ENTER || norm == STEAMCONTROLLER_A );

	if ( bIsConfirm )
	{
		if ( IsEnabled() && IsDepressed() && m_bControllerDepressed )
		{
			ForceDepressed( false );
			FireActionSignal();
			m_bControllerDepressed = false;
		}
		if ( m_bForwardToParent )
		{
			vgui::Panel::OnKeyCodeReleased( code );
		}
		return;
	}

	BaseClass::OnKeyCodeReleased( code );
}

void GamepadUIButton::FireActionSignal()
{
	BaseClass::FireActionSignal();

	KeyValues *pMsg = new KeyValues( "OnSiblingGamepadUIButtonOpened" );
	if ( vgui::Panel *pParent = GetParent() )
	{
		int nChildren = pParent->GetChildCount();
		for ( int i = 0; i < nChildren; ++i )
		{
			vgui::Panel *pChild = pParent->GetChild( i );
			if ( !pChild || pChild == this )
				continue;

			if ( dynamic_cast<GamepadUIButton *>( pChild ) )
			{
				PostMessage( pChild->GetVPanel(), pMsg->MakeCopy(), 0.0f );
			}
		}
	}
	pMsg->deleteThis();
}

//-----------------------------------------------------------------------------
// Purpose: Painting
//-----------------------------------------------------------------------------
void GamepadUIButton::PaintBorders()
{
	if ( !vgui::surface() )
		return;

	int nTotalH = static_cast<int>( m_flHeight + m_flExtraHeight );

	if ( m_flTextLeftBorder != 0.0f )
	{
		vgui::surface()->DrawSetColor( m_colLeftBorder );
		vgui::surface()->DrawFilledRect( 0, 0, static_cast<int>( m_flTextLeftBorder ), nTotalH );
	}

	if ( m_flTextBottomBorder != 0.0f )
	{
		vgui::surface()->DrawSetColor( m_colBottomBorder );
		vgui::surface()->DrawFilledRect(
			0,
			static_cast<int>( ( m_flHeight + m_flExtraHeight ) - m_flTextBottomBorder ),
			static_cast<int>( m_flWidth ),
			nTotalH );
	}
}

void GamepadUIButton::PaintButton()
{
	if ( !vgui::surface() )
		return;

	vgui::surface()->DrawSetColor( m_colBackgroundColor );
	vgui::surface()->DrawFilledRect( 0, 0, static_cast<int>( m_flWidth ), static_cast<int>( m_flHeight + m_flExtraHeight ) );
	PaintBorders();
}

int GamepadUIButton::PaintText()
{
	if ( !vgui::surface() )
		return 0;

	ButtonState::State eState = GetCurrentButtonState();
	vgui::HFont hFont = ( eState == ButtonState::Out ) ? m_hTextFont : m_hTextFontOver;

	int nTextW = 0, nTextH = 0;
	int nTextX = 0, nTextY = 0;

	if ( !m_strButtonText.IsEmpty() )
	{
		if ( IsAllCaps() )
		{
			for ( int i = 0; i < m_strButtonText.Count(); ++i )
			{
				m_strButtonText[i] = static_cast<wchar_t>( towupper( m_strButtonText[i] ) );
			}
		}

		vgui::surface()->DrawSetTextFont( hFont );
		vgui::surface()->GetTextSize( hFont, m_strButtonText.String(), nTextW, nTextH );

		if ( m_CenterX )
		{
			nTextX = static_cast<int>( ( m_flWidth * 0.5f ) - ( nTextW * 0.5f ) + m_flTextOffsetX );
		}
		else
		{
			nTextX = static_cast<int>( m_flTextOffsetX );
		}

		nTextY = static_cast<int>( ( m_flHeight * 0.5f ) - ( nTextH * 0.5f ) + m_flTextOffsetY );
	}

	// Render Steam Input glyph on footer buttons when Steam Controller is active
	if ( g_pInputSystem && g_pInputSystem->IsSteamControllerActive() && m_nFooterButton != FooterButtons::None )
	{
		int nGlyphSize = static_cast<int>( m_flHeight * 0.8f );
		const char *pszAction = FooterButtons::GetButtonActionName( m_nFooterButton );
		if ( m_Glyph.SetupGlyph( nGlyphSize, pszAction, false ) )
		{
			int nGlyphX = 0;
			if ( m_CenterX )
			{
				nGlyphX = static_cast<int>( nTextX - ( m_flHeight * 0.5f ) );
				nTextX = static_cast<int>( nTextX + ( m_flHeight * 0.5f ) );
			}
			else
			{
				nGlyphX = static_cast<int>( m_flTextOffsetX );
				nTextX += static_cast<int>( nGlyphSize + ( m_flTextOffsetX * 0.5f ) );
			}

			int nGlyphY = static_cast<int>( ( m_flHeight * 0.5f ) - ( nGlyphSize * 0.5f ) );
			m_Glyph.PaintGlyph( nGlyphX, nGlyphY, nGlyphSize, m_flGlyphFade );
		}
	}

	// Draw main text
	if ( !m_strButtonText.IsEmpty() )
	{
		vgui::surface()->DrawSetTextColor( m_colTextColor );
		vgui::surface()->DrawSetTextFont( hFont );
		vgui::surface()->DrawSetTextPos( nTextX, nTextY );
		vgui::surface()->DrawPrintText( m_strButtonText.String(), m_strButtonText.Length() );
	}

	// Draw description and update extra wrapped height animation
	float flTime = GamepadUI::GetInstance().GetTime();
	float flTargetExtraHeight = 0.0f;

	if ( !m_strButtonDescription.IsEmpty() && !m_bDescriptionHide )
	{
		vgui::surface()->DrawSetTextColor( m_colDescriptionColor );
		vgui::surface()->DrawSetTextFont( m_hDescriptionFont );

		int nDescX = static_cast<int>( nTextX + m_flDescriptionOffsetX );
		int nDescY = static_cast<int>( nTextY + nTextH + m_flDescriptionOffsetY );

		if ( m_bDescriptionWrap )
		{
			int nMaxWidth = static_cast<int>( m_flWidth - ( m_flTextOffsetX + m_flDescriptionOffsetX ) * 2.0f );
			int nWrappedH = GamepadUI_DrawWrappedText(
				m_hDescriptionFont,
				nDescX,
				nDescY,
				m_strButtonDescription.String(),
				m_strButtonDescription.Length(),
				nMaxWidth,
				true );
			flTargetExtraHeight = static_cast<float>( nWrappedH );
		}
		else
		{
			vgui::surface()->DrawSetTextPos( nDescX, nDescY );
			vgui::surface()->DrawPrintText( m_strButtonDescription.String(), m_strButtonDescription.Length() );
		}
	}

	if ( flTargetExtraHeight != m_flExtraHeightTarget )
	{
		m_flExtraHeightTarget = flTargetExtraHeight;
		m_flExtraHeightStart = m_flExtraHeight;
		m_flExtraHeightStartTime = flTime;
	}

	float flFrac = 1.0f;
	if ( m_flHeightAnimDuration > 0.0f )
	{
		flFrac = clamp( ( flTime - m_flExtraHeightStartTime ) / m_flHeightAnimDuration, 0.0f, 1.0f );
	}
	m_flExtraHeight = Lerp( flFrac, m_flExtraHeightStart, m_flExtraHeightTarget );

	return nTextW;
}

void GamepadUIButton::Paint()
{
	BaseClass::Paint();
	PaintButton();
	PaintText();
	m_bNavigated = false;
}

//-----------------------------------------------------------------------------
// Purpose: Metadata setters
//-----------------------------------------------------------------------------
void GamepadUIButton::SetFooterButton( int nFooterButton )
{
	m_nFooterButton = nFooterButton;
	m_Glyph.Cleanup();
}

void GamepadUIButton::SetCategory( const char *pszCategory )
{
	m_strCategory = pszCategory ? pszCategory : "";
}

const char *GamepadUIButton::GetCategory() const
{
	return m_strCategory.IsEmpty() ? NULL : m_strCategory.String();
}

void GamepadUIButton::SetAllCaps( bool bAllCaps )
{
	m_bButtonAllCaps = bAllCaps;
	BaseClass::SetAllCaps( bAllCaps );
}

void GamepadUIButton::SetButtonText( const char *pszText )
{
	m_strButtonText.SetString( pszText );
}

void GamepadUIButton::SetButtonText( const wchar_t *pwszText )
{
	m_strButtonText.SetString( pwszText );
}

void GamepadUIButton::SetButtonDescription( const char *pszDescription )
{
	m_strButtonDescription.SetString( pszDescription );
}

void GamepadUIButton::SetButtonDescription( const wchar_t *pwszDescription )
{
	m_strButtonDescription.SetString( pwszDescription );
}
