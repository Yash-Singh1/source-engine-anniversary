//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI Achievements Panel & Achievement Row Button.
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#include "gamepadui_achievements.h"
#include "gamepadui_interface.h"
#include "gamepadui_basepanel.h"

#include "filesystem.h"
#include "iachievementmgr.h"
#include "tier1/convar.h"
#include "tier1/fmtstr.h"
#include "vgui/ILocalize.h"
#include "vgui/IPanel.h"
#include "vgui/IScheme.h"
#include "vgui/ISurface.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================
// GamepadUIAchievement
//=============================================================================

GamepadUIAchievement::GamepadUIAchievement(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const wchar_t *pText,
	const wchar_t *pDescription,
	const char *pIconImage,
	IAchievement *pAchievement )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription )
	, m_flProgress( 0.0f )
	, m_nCount( 0 )
	, m_nGoal( 0 )
	, m_hProgressFont( vgui::INVALID_FONT )
	, m_colProgressColor( 255, 0, 0, 255 )
	, m_colUnprogressColor( 255, 0, 0, 255 )
	, m_flProgressHeight( 1.0f )
	, m_flIconInset( 0.0f )
	, m_flProgressOffsetX( 0.0f )
{
	m_Icon.SetImage( pIconImage );

	if ( pAchievement )
	{
		m_nGoal = pAchievement->GetGoal();
		m_nCount = pAchievement->GetCount();

		if ( pAchievement->IsAchieved() )
		{
			m_nCount = m_nGoal;
			m_flProgress = 1.0f;
		}
		else if ( m_nGoal > 0 )
		{
			m_flProgress = MIN( (float)m_nCount / (float)m_nGoal, 1.0f );
		}
	}
}

GamepadUIAchievement::~GamepadUIAchievement()
{
}

void GamepadUIAchievement::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	m_hProgressFont = pScheme->GetFont( "Button.Progress.Font", true );
}

void GamepadUIAchievement::Paint()
{
	int x, y, w, h;
	GetBounds( x, y, w, h );

	PaintButton();

	float flTotalHeight = m_flHeight + m_flExtraHeight;
	int nProgressY0 = (int)( flTotalHeight - m_flProgressHeight );
	int nProgressY1 = (int)flTotalHeight;
	int nProgressX = (int)( m_flWidth * m_flProgress );

	// Filled portion of achievement progress bar
	vgui::surface()->DrawSetColor( m_colProgressColor );
	vgui::surface()->DrawFilledRect( 0, nProgressY0, nProgressX, nProgressY1 );

	// Unfilled remainder for multi-step achievements with non-zero progress
	if ( m_nGoal > 1 && m_flProgress > 0.0f )
	{
		vgui::surface()->DrawSetColor( m_colUnprogressColor );
		vgui::surface()->DrawFilledRect( nProgressX, nProgressY0, (int)m_flWidth, nProgressY1 );
	}

	// Achievement icon
	vgui::surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
	vgui::surface()->DrawSetTexture( m_Icon );
	int nIconInset = (int)m_flIconInset;
	int nIconSize = (int)( m_flHeight - ( 2.0f * m_flIconInset ) );
	int nIconEnd = (int)( (float)nIconSize + m_flIconInset );
	vgui::surface()->DrawTexturedRect( nIconInset, nIconInset, nIconEnd, nIconEnd );
	vgui::surface()->DrawSetTexture( 0 );

	PaintText();

	// Progress counter text ("count/goal") on right side for progressive achievements
	if ( m_nGoal > 1 )
	{
		wchar_t wszProgress[32];
		int nLen = V_swprintf_safe( wszProgress, L"%d/%d", m_nCount, m_nGoal );

		vgui::surface()->DrawSetTextColor( m_colTextColor );
		vgui::surface()->DrawSetTextFont( m_hProgressFont );

		int nTextW = 0, nTextH = 0;
		vgui::surface()->GetTextSize( m_hProgressFont, wszProgress, nTextW, nTextH );

		int nPosX = (int)( m_flWidth - (float)nTextW - m_flProgressOffsetX );
		int nPosY = (int)( flTotalHeight * 0.5f - (float)( nTextH / 2 ) );
		vgui::surface()->DrawSetTextPos( nPosX, nPosY );
		vgui::surface()->DrawPrintText( wszProgress, nLen );
	}
}

//=============================================================================
// GamepadUIAchievementsPanel
//=============================================================================

GamepadUIAchievementsPanel::GamepadUIAchievementsPanel( vgui::Panel *pParent, const char *pPanelName, const char *pOverrideMod )
	: BaseClass( pParent, pPanelName, true, true )
	, m_nTotalAchievements( 0 )
	, m_nUnlocked( 0 )
	, m_AchievementsFade( 0.0f )
	, m_AchievementsOffsetX( 0.0f )
	, m_AchievementsOffsetY( 0.0f )
	, m_flAchievementsSpacing( 0.0f )
{
	vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile( "gamepadui/schemepanel.res", "SchemePanel" );
	SetScheme( hScheme );

	m_strTitle = GamepadUIString( "#GameUI_Achievements_Title" );
	SetFooterButtons( FooterButtons::Back, FooterButtons::None );

	Activate();

	m_strOverrideMod = pOverrideMod;

	IAchievementMgr *pAchievementMgr = GamepadUI::GetInstance().GetAchievementMgr();
	if ( pAchievementMgr )
	{
		pAchievementMgr->EnsureGlobalStateLoaded();

		int nAllAchievements = pAchievementMgr->GetAchievementCount();
		for ( int i = 0; i < nAllAchievements; ++i )
		{
			IAchievement *pAchievement = pAchievementMgr->GetAchievementByIndex( i );
			if ( !pAchievement )
				continue;

			const char *pszAchName = pAchievement->GetName();
			if ( !pszAchName || !pszAchName[0] )
				continue;

#ifndef HL2_LOSTCOAST
			int nID = pAchievement->GetAchievementID();
			bool bCanEarn = m_strOverrideMod.IsEmpty()
				? pAchievementMgr->CanEarnForCurrentExpansion( nID )
				: pAchievementMgr->CanEarnForExpansion( nID, m_strOverrideMod.Get() );
			if ( !bCanEarn )
				continue;
#endif

			if ( pAchievement->IsAchieved() )
				++m_nUnlocked;

			// Do not show hidden achievements until earned
			if ( pAchievement->ShouldHideUntilAchieved() && !pAchievement->IsAchieved() )
				continue;

			char szCheckFile[MAX_PATH] = { 0 };
			char szImagePath[MAX_PATH] = { 0 };
			const char *pszSuffix = pAchievement->IsAchieved() ? "" : "_bw";

			if ( g_pFullFileSystem && !m_strOverrideMod.IsEmpty() )
			{
				V_sprintf_safe( szImagePath, "gamepadui/%s/achievements/%s%s", m_strOverrideMod.Get(), pszAchName, pszSuffix );
				V_sprintf_safe( szCheckFile, "materials/%s.vmt", szImagePath );
				if ( !g_pFullFileSystem->FileExists( szCheckFile ) )
				{
					V_sprintf_safe( szImagePath, "vgui/%s/achievements/%s%s", m_strOverrideMod.Get(), pszAchName, pszSuffix );
					V_sprintf_safe( szCheckFile, "materials/%s.vmt", szImagePath );
					if ( !g_pFullFileSystem->FileExists( szCheckFile ) )
					{
						szCheckFile[0] = '\0';
					}
				}
			}

			if ( g_pFullFileSystem && !szCheckFile[0] )
			{
				V_sprintf_safe( szImagePath, "gamepadui/achievements/%s%s", pszAchName, pszSuffix );
				V_sprintf_safe( szCheckFile, "materials/%s.vmt", szImagePath );
				if ( !g_pFullFileSystem->FileExists( szCheckFile ) )
				{
					szCheckFile[0] = '\0';
				}
			}

			if ( g_pFullFileSystem && !szCheckFile[0] )
			{
				V_sprintf_safe( szImagePath, "vgui/achievements/%s%s", pszAchName, pszSuffix );
				V_sprintf_safe( szCheckFile, "materials/%s.vmt", szImagePath );
				if ( !g_pFullFileSystem->FileExists( szCheckFile ) )
				{
					szCheckFile[0] = '\0';
				}
			}

			if ( !szCheckFile[0] )
			{
				V_strncpy( szCheckFile, "vgui/hud/icon_locked.vmt", sizeof( szCheckFile ) );
				V_strncpy( szImagePath, "vgui/hud/icon_locked", sizeof( szImagePath ) );
			}

			const wchar_t *pwszName = g_pVGuiLocalize ? g_pVGuiLocalize->Find( CFmtStr( "#%s_NAME", pszAchName ) ) : NULL;
			const wchar_t *pwszDesc = g_pVGuiLocalize ? g_pVGuiLocalize->Find( CFmtStr( "#%s_DESC", pszAchName ) ) : NULL;

			GamepadUIAchievement *pButton = new GamepadUIAchievement(
				this,
				this,
				"gamepadui/schemeachievement.res",
				"",
				pwszName,
				pwszDesc,
				szImagePath,
				pAchievement );

			pButton->SetPriority( m_nTotalAchievements );
			m_pAchievementButtons.AddToTail( pButton );
			++m_nTotalAchievements;
		}
	}

	if ( m_pAchievementButtons.Count() > 0 )
	{
		m_pAchievementButtons[0]->NavigateTo();
		for ( int i = 1; i < m_pAchievementButtons.Count(); ++i )
		{
			m_pAchievementButtons[i]->SetNavUp( m_pAchievementButtons[i - 1] );
			m_pAchievementButtons[i - 1]->SetNavDown( m_pAchievementButtons[i] );
		}
	}

	UpdateGradients();
}

GamepadUIAchievementsPanel::~GamepadUIAchievementsPanel()
{
}

void GamepadUIAchievementsPanel::UpdateGradients()
{
	float flTime = GamepadUI::GetInstance().GetTime();
	GamepadUI::GetInstance().GetGradientHelper().ResetTargets( flTime );
	GamepadUI::GetInstance().GetGradientHelper().SetTargetGradient( GamepadUIGradientSides::Up, { 1.0f, 1.0f }, flTime );
	GamepadUI::GetInstance().GetGradientHelper().SetTargetGradient( GamepadUIGradientSides::Down, { 1.0f, 0.5f }, flTime );
}

void GamepadUIAchievementsPanel::OnThink()
{
	BaseClass::OnThink();
	LayoutAchievementButtons();
}

void GamepadUIAchievementsPanel::LayoutAchievementButtons()
{
	vgui::Panel *pParent = GetParent();
	if ( !pParent )
		return;

	int nParentW, nParentH;
	pParent->GetSize( nParentW, nParentH );

	float flMaxScroll = 0.0f;
	for ( int i = 0; i < m_pAchievementButtons.Count(); ++i )
	{
		int nTall = m_pAchievementButtons[i]->GetTall();
		if ( i < m_pAchievementButtons.Count() - 2 )
		{
			flMaxScroll += (float)(int)( (float)nTall + m_flAchievementsSpacing );
		}
	}

	m_ScrollState.UpdateScrollBounds( 0.0f, flMaxScroll );

	int nOffsetY = 0;
	for ( int i = 0; i < m_pAchievementButtons.Count(); ++i )
	{
		int nTall = m_pAchievementButtons[i]->GetTall();
		int nStep = (int)( (float)nTall + m_flAchievementsSpacing );
		int nY = (int)( m_AchievementsOffsetY - m_ScrollState.GetScrollProgress() + (float)nOffsetY );

		int nAlpha = 255;
		if ( m_AchievementsFade > 0.0f )
		{
			if ( (float)nY < m_AchievementsOffsetY )
			{
				float flFade = clamp( ( m_AchievementsOffsetY - (float)nY ) / m_AchievementsFade, 0.0f, 1.0f );
				nAlpha = (int)( ( 1.0f - flFade ) * 255.0f );
			}
			if ( (float)nY > (float)nParentH - m_AchievementsFade )
			{
				float flFade = clamp( ( (float)nY + (float)nStep - ( (float)nParentH - m_AchievementsFade ) ) / m_AchievementsFade, 0.0f, 1.0f );
				nAlpha = (int)( ( 1.0f - flFade ) * 255.0f );
			}
		}

		if ( nAlpha != 0 && m_pAchievementButtons[i]->HasFocus() )
		{
			nAlpha = 255;
		}

		m_pAchievementButtons[i]->SetAlpha( nAlpha );
		m_pAchievementButtons[i]->SetPos( (int)m_AchievementsOffsetX, nY );
		m_pAchievementButtons[i]->SetVisible( true );

		nOffsetY += nStep;
	}

	m_ScrollState.UpdateScroll( GamepadUI::GetInstance().GetTime(), 0.5f );
}

void GamepadUIAchievementsPanel::OnMouseWheeled( int nDelta )
{
	m_ScrollState.OnMouseWheeled( nDelta, GamepadUI::GetInstance().GetTime(), 160.0f );
}

void GamepadUIAchievementsPanel::OnGamepadUIButtonNavigatedTo( vgui::VPANEL button )
{
	vgui::Panel *pPanel = vgui::ipanel()->GetPanel( button, GetModuleName() );
	if ( !pPanel )
		return;

	GamepadUIAchievement *pButton = dynamic_cast<GamepadUIAchievement *>( pPanel );
	if ( !pButton || pButton->GetAlpha() == 255 )
		return;

	vgui::Panel *pParent = GetParent();
	if ( !pParent )
		return;

	int nParentW, nParentH;
	pParent->GetSize( nParentW, nParentH );

	int nX, nY;
	pButton->GetPos( nX, nY );

	int nBaseY = (int)( (float)pButton->GetPriority() * ( pButton->GetHeightOut() + m_flAchievementsSpacing ) );
	int nTargetScroll;
	if ( nY < nParentH / 2 )
	{
		nTargetScroll = (int)( (float)(int)( (float)nBaseY + ( (float)nParentH - m_AchievementsOffsetY ) ) - m_flAchievementsSpacing );
	}
	else
	{
		float flExpandedH = pButton->GetWrappedDescriptionHeight() + pButton->GetHeightOver();
		nTargetScroll = (int)( (float)(int)( (float)nBaseY + flExpandedH ) + ( flExpandedH * 0.5f + 2.0f * m_flAchievementsSpacing ) );
	}

	m_ScrollState.SetScrollTarget( (float)nTargetScroll - ( (float)nParentH - m_AchievementsOffsetY ), GamepadUI::GetInstance().GetTime() );
}

void GamepadUIAchievementsPanel::OnCommand( const char *pCommand )
{
	if ( !V_strcmp( pCommand, "action_back" ) )
	{
		Close();
	}
	else
	{
		BaseClass::OnCommand( pCommand );
	}
}

CON_COMMAND( gamepadui_openachievementsdialog, "" )
{
	const char *pszOverrideMod = args.ArgC() > 1 ? args.Arg( 1 ) : NULL;
	new GamepadUIAchievementsPanel( GamepadUI::GetInstance().GetBasePanel(), "AchievementsPanel", pszOverrideMod );
}
