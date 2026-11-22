//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI Bonus Maps Panel & Bonus Map Button.
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#include "gamepadui_bonusmaps.h"
#include "gamepadui_interface.h"
#include "gamepadui_basepanel.h"

#include "cdll_int.h"
#include "GameUI/IGameUI.h"
#include "tier1/convar.h"
#include "vgui/ILocalize.h"
#include "vgui/IScheme.h"
#include "vgui/ISurface.h"

#include <string.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ConVar gamepadui_selected_challenge( "gamepadui_selected_challenge", "0", FCVAR_ARCHIVE );

static const char s_pszMedalNames[4][8] =
{
	"none",
	"bronze",
	"silver",
	"gold"
};

GamepadUIBonusButton *GamepadUIBonusButton::s_pSelectedButton = NULL;

//-----------------------------------------------------------------------------
// Purpose: Compute earned and next challenge medals from a challenge entry
//-----------------------------------------------------------------------------
static void GetChallengeMedalsLocal( ChallengeDescription_t *pChallengeDescription, int &iBest, int &iEarnedMedal, int &iNext, int &iNextMedal )
{
	iBest = pChallengeDescription->iBest;

	if ( iBest == -1 )
		iEarnedMedal = 0;
	else if ( iBest <= pChallengeDescription->iGold )
		iEarnedMedal = 3;
	else if ( iBest <= pChallengeDescription->iSilver )
		iEarnedMedal = 2;
	else if ( iBest <= pChallengeDescription->iBronze )
		iEarnedMedal = 1;
	else
		iEarnedMedal = 0;

	iNext = -1;

	switch ( iEarnedMedal )
	{
	case 0:
		iNext = pChallengeDescription->iBronze;
		iNextMedal = 1;
		break;
	case 1:
		iNext = pChallengeDescription->iSilver;
		iNextMedal = 2;
		break;
	case 2:
		iNext = pChallengeDescription->iGold;
		iNextMedal = 3;
		break;
	case 3:
		iNext = -1;
		iNextMedal = -1;
		break;
	}
}

static void CycleSelectedChallenge()
{
	int nCurrent = clamp( gamepadui_selected_challenge.GetInt(), 0, 2 );
	gamepadui_selected_challenge.SetValue( ( nCurrent + 1 ) % 3 );
}

//=============================================================================
// GamepadUIBonusButton
//=============================================================================

GamepadUIBonusButton::GamepadUIBonusButton(
	vgui::Panel *pParent,
	vgui::Panel *pActionSignalTarget,
	const char *pSchemeFile,
	const char *pCommand,
	const char *pText,
	const char *pDescription,
	const char *pBonusImage )
	: BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription )
	, m_colProgressColor( 255, 0, 0, 255 )
	, m_flProgressHeight( 1.0f )
	, m_flMedalSize( 64.0f )
	, m_flMedalOffsetX( 8.0f )
	, m_flMedalOffsetY( 8.0f )
	, m_flLockSize( 80.0f )
	, m_colLock( 20, 20, 20, 255 )
{
	m_BonusImage.SetImage( pBonusImage );
	m_LockImage.SetImage( "gamepadui/lockylock" );
	m_bChallengeCompleted[0] = false;
	m_bChallengeCompleted[1] = false;
	m_bChallengeCompleted[2] = false;
}

GamepadUIBonusButton::~GamepadUIBonusButton()
{
	if ( s_pSelectedButton == this )
		s_pSelectedButton = NULL;
}

void GamepadUIBonusButton::NavigateTo()
{
	BaseClass::NavigateTo();
	s_pSelectedButton = this;
}

ButtonState::State GamepadUIBonusButton::GetCurrentButtonState()
{
	if ( this == s_pSelectedButton )
		return ButtonState::Over;

	return BaseClass::GetCurrentButtonState();
}

void GamepadUIBonusButton::SetBonusDescription( const BonusMapDescription_t &desc )
{
	for ( int i = 0; i < 3; ++i )
	{
		m_MedalImages[i].Cleanup();
		m_bChallengeCompleted[i] = false;
	}

	m_BonusDescription = desc;

	if ( m_BonusDescription.m_pChallenges )
	{
		for ( int i = 0; i < m_BonusDescription.m_pChallenges->Count(); ++i )
		{
			ChallengeDescription_t *pChallenge = &( (*m_BonusDescription.m_pChallenges)[i] );
			int iType = pChallenge->iType;
			if ( iType == -1 )
				iType = i;

			int iBest, iEarnedMedal, iNext, iNextMedal;
			GetChallengeMedalsLocal( pChallenge, iBest, iEarnedMedal, iNext, iNextMedal );

			if ( iType >= 0 && iType < 3 )
			{
				m_bChallengeCompleted[iType] = ( iNextMedal == -1 );
				int iMedalIdx = clamp( iEarnedMedal, 0, 3 );

				char szMedalPath[256];
				if ( iType < 10 )
					V_snprintf( szMedalPath, sizeof( szMedalPath ), "vgui/medals/medal_0%i_%s", iType, s_pszMedalNames[iMedalIdx] );
				else
					V_snprintf( szMedalPath, sizeof( szMedalPath ), "vgui/medals/medal_%i_%s", iType, s_pszMedalNames[iMedalIdx] );

				m_MedalImages[iType].SetImage( szMedalPath );
			}
		}
	}
}

void GamepadUIBonusButton::Paint()
{
	int x, y, w, h;
	GetBounds( x, y, w, h );

	PaintButton();

	int nImgHeight = ( w * 100 ) / 180;
	float flBaseHeight16x9 = (float)( ( w * 9 ) / 16 );
	float flTallRatio = ( flBaseHeight16x9 > 0.0f )
		? ( ( flBaseHeight16x9 + ( m_flTextOffsetY - m_flTextOffsetYState[ButtonState::Out] ) ) / flBaseHeight16x9 )
		: 1.0f;

	// Bonus map VTF preview is 180x100 inside a 256x128 power-of-two texture
	vgui::surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
	vgui::surface()->DrawSetTexture( m_BonusImage );
	vgui::surface()->DrawTexturedSubRect( 0, 0, w, nImgHeight, 0.0f, 0.0f, 180.0f / 256.0f, ( 100.0f / 128.0f ) * flTallRatio );
	vgui::surface()->DrawSetTexture( 0 );

	if ( !IsEnabled() )
	{
		vgui::surface()->DrawSetColor( Color( 255, 255, 255, 16 ) );
		vgui::surface()->DrawFilledRect( 0, 0, w, nImgHeight );

		vgui::surface()->DrawSetColor( m_colLock );
		vgui::surface()->DrawSetTexture( m_LockImage );
		int nLockX = (int)( ( m_flWidth - m_flLockSize ) * 0.5f );
		int nLockY = (int)( (float)( ( w * 9 ) / 32 ) - ( m_flLockSize * 0.5f ) );
		vgui::surface()->DrawTexturedRect( nLockX, nLockY, (int)( (float)nLockX + m_flLockSize ), (int)( (float)nLockY + m_flLockSize ) );
		vgui::surface()->DrawSetTexture( 0 );
	}

	// Completion accent bar along bottom of preview image
	int nChallenge = clamp( gamepadui_selected_challenge.GetInt(), 0, 2 );
	float flCompletion = 0.0f;
	if ( !m_BonusDescription.bIsFolder && ( m_BonusDescription.bComplete || m_bChallengeCompleted[nChallenge] ) )
	{
		flCompletion = 1.0f;
	}

	vgui::surface()->DrawSetColor( m_colProgressColor );
	vgui::surface()->DrawFilledRect( 0, (int)( (float)nImgHeight - m_flProgressHeight ), (int)( (float)w * flCompletion ), nImgHeight );

	PaintText();

	// Selected challenge medal overlay
	if ( m_MedalImages[nChallenge].GetTextureId() > 0 )
	{
		vgui::surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
		vgui::surface()->DrawSetTexture( m_MedalImages[nChallenge] );
		vgui::surface()->DrawTexturedRect(
			(int)m_flMedalOffsetX,
			(int)m_flMedalOffsetY,
			(int)( m_flMedalOffsetX + m_flMedalSize ),
			(int)( m_flMedalOffsetY + m_flMedalSize ) );
		vgui::surface()->DrawSetTexture( 0 );
	}
}

//=============================================================================
// GamepadUIBonusMapsPanel
//=============================================================================

GamepadUIBonusMapsPanel::GamepadUIBonusMapsPanel( vgui::Panel *pParent, const char *pPanelName )
	: BaseClass( pParent, pPanelName, true, true )
	, m_BonusOffsetX( 0.0f )
	, m_BonusOffsetY( 0.0f )
	, m_BonusSpacing( 0.0f )
	, m_flFooterMedalSize( 0.0f )
	, m_flFooterMedalNextSize( 0.0f )
	, m_hGoalFont( vgui::INVALID_FONT )
{
	m_szFooterCurrentMedal[0] = '\0';
	m_szFooterNextMedal[0] = '\0';

	vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile( "gamepadui/schemepanel.res", "SchemePanel" );
	SetScheme( hScheme );

	SetFooterButtons( FooterButtons::Back | FooterButtons::SelectRight, FooterButtons::SelectRight );
	BuildMapsList();
	Activate();
	UpdateGradients();
}

GamepadUIBonusMapsPanel::~GamepadUIBonusMapsPanel()
{
}

void GamepadUIBonusMapsPanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	m_hGoalFont = pScheme->GetFont( "Goal.Font", true );
}

void GamepadUIBonusMapsPanel::UpdateGradients()
{
	float flTime = GamepadUI::GetInstance().GetTime();
	GamepadUI::GetInstance().GetGradientHelper().ResetTargets( flTime );
	GamepadUI::GetInstance().GetGradientHelper().SetTargetGradient( GamepadUIGradientSides::Up, { 1.0f, 1.0f }, flTime );
	GamepadUI::GetInstance().GetGradientHelper().SetTargetGradient( GamepadUIGradientSides::Down, { 1.0f, 0.5f }, flTime );
}

void GamepadUIBonusMapsPanel::BuildMapsList()
{
	for ( int i = 0; i < m_pBonusButtons.Count(); ++i )
	{
		delete m_pBonusButtons[i];
	}
	m_pBonusButtons.Purge();
	m_BonusMaps.RemoveAll();

	IGameUI *pGameUI = GamepadUI::GetInstance().GetGameUI();
	CBonusMapsDatabase *pDB = pGameUI ? pGameUI->GetBonusMapsDatabase() : NULL;
	if ( !pDB )
		return;

	pDB->ClearBonusMapsList();
	pDB->ScanBonusMaps();

	char szDisplayPath[4096];
	V_snprintf( szDisplayPath, sizeof( szDisplayPath ), "%s/", pDB->GetPath() );
	(void)szDisplayPath;

	m_strTitle = GamepadUIString( "#GameUI_BonusMaps" );

	int nMapCount = pDB->BonusCount();
	bool bHasChallenges = false;

	for ( int i = 0; i < nMapCount && i < MAX_LISTED_BONUS_MAPS; ++i )
	{
		BonusMapDescription_t *pDesc = pDB->GetBonusData( i );
		if ( !pDesc )
			continue;

		if ( pDesc->m_pChallenges )
			bHasChallenges = true;

		char szImage[MAX_PATH];
		V_snprintf( szImage, sizeof( szImage ), "vgui/%s", pDesc->szImageName );

		GamepadUIBonusButton *pButton = new GamepadUIBonusButton(
			this,
			this,
			"gamepadui/schemebonusbutton.res",
			"action_map",
			pDesc->szMapName,
			NULL,
			szImage );

		pButton->SetPriority( i );
		pButton->SetEnabled( !pDesc->bLocked );
		pButton->SetForwardToParent( true );
		pButton->SetBonusDescription( *pDesc );

		m_pBonusButtons.AddToTail( pButton );
		m_BonusMaps.AddToTail( *pDesc );
	}

	if ( m_pBonusButtons.Count() > 0 )
	{
		m_pBonusButtons[0]->NavigateTo();
		for ( int i = 1; i < m_pBonusButtons.Count(); ++i )
		{
			m_pBonusButtons[i]->SetNavLeft( m_pBonusButtons[i - 1] );
			m_pBonusButtons[i - 1]->SetNavRight( m_pBonusButtons[i] );
		}
	}

	int nFooter = FooterButtons::Back | FooterButtons::SelectRight;
	if ( bHasChallenges )
		nFooter |= FooterButtons::Challenges;
	SetFooterButtons( nFooter, FooterButtons::SelectRight );
}

void GamepadUIBonusMapsPanel::OnThink()
{
	BaseClass::OnThink();
	LayoutBonusButtons();
}

void GamepadUIBonusMapsPanel::LayoutBonusButtons()
{
	vgui::Panel *pParent = GetParent();
	if ( !pParent )
		return;

	int nParentW, nParentH;
	pParent->GetSize( nParentW, nParentH );

	float flMaxScroll = 0.0f;
	for ( int i = 0; i < m_pBonusButtons.Count(); ++i )
	{
		int nWide = m_pBonusButtons[0]->GetWide();
		if ( i < m_pBonusButtons.Count() - 2 )
		{
			flMaxScroll += (float)(int)( (float)nWide + m_BonusSpacing );
		}
	}

	m_ScrollState.UpdateScrollBounds( 0.0f, flMaxScroll );

	int nX = (int)m_BonusOffsetX;
	int nY = (int)m_BonusOffsetY;
	int nCol = 0;

	CUtlVector< CUtlVector<GamepadUIBonusButton *> > vecColumns;

	for ( int i = 0; i < m_pBonusButtons.Count(); ++i )
	{
		int nWide = m_pBonusButtons[0]->GetWide();
		int nStepX = (int)( (float)nWide + m_BonusSpacing );

		if ( (float)( nX + nStepX ) > (float)nParentW - m_BonusOffsetX )
		{
			nX = (int)m_BonusOffsetX;
			int nTall = m_pBonusButtons[0]->GetTall();
			nY = (int)( (float)nY + (float)nTall + m_BonusSpacing );
			nCol = 0;
		}

		m_pBonusButtons[i]->SetPos( nX, nY );
		m_pBonusButtons[i]->SetVisible( true );

		nX = (int)( (float)nX + (float)nStepX + m_BonusSpacing );

		int nColIdx = nCol++;
		while ( vecColumns.Count() <= nColIdx )
		{
			vecColumns.AddToTail();
		}
		vecColumns[nColIdx].AddToTail( m_pBonusButtons[i] );
	}

	// Wire up vertical navigation within each grid column
	for ( int c = 0; c < vecColumns.Count(); ++c )
	{
		CUtlVector<GamepadUIBonusButton *> &col = vecColumns[c];
		for ( int r = 1; r < col.Count(); ++r )
		{
			col[r]->SetNavUp( col[r - 1] );
			col[r - 1]->SetNavDown( col[r] );
		}
	}

	m_ScrollState.UpdateScroll( GamepadUI::GetInstance().GetTime(), 0.5f );
}

void GamepadUIBonusMapsPanel::Paint()
{
	BaseClass::Paint();

	GamepadUIBonusButton *pSelected = GamepadUIBonusButton::s_pSelectedButton;
	if ( !pSelected )
		return;

	CUtlVector<ChallengeDescription_t> *pChallenges = pSelected->GetBonusDescription().m_pChallenges;
	if ( !pChallenges )
		return;

	int nChallenge = clamp( gamepadui_selected_challenge.GetInt(), 0, 2 );
	ChallengeDescription_t *pChallengeDesc = NULL;
	int nMatchedType = -1;

	for ( int i = 0; i < pChallenges->Count(); ++i )
	{
		int iType = (*pChallenges)[i].iType;
		if ( iType == -1 )
			iType = i;

		if ( iType == nChallenge )
		{
			pChallengeDesc = &( (*pChallenges)[i] );
			nMatchedType = iType;
			break;
		}
	}

	if ( !pChallengeDesc )
		return;

	int iBest = -1, iEarnedMedal = -1, iNext = -1, iNextMedal = -1;
	GetChallengeMedalsLocal( pChallengeDesc, iBest, iEarnedMedal, iNext, iNextMedal );

	vgui::Panel *pParent = GetParent();
	if ( !pParent )
		return;

	int nParentW, nParentH;
	pParent->GetSize( nParentW, nParentH );

	// Update cached current earned medal texture
	if ( iEarnedMedal >= 0 && iEarnedMedal < 4 )
	{
		char szMedal[256];
		V_snprintf( szMedal, sizeof( szMedal ), "vgui/medals/medal_0%i_%s", nMatchedType, s_pszMedalNames[iEarnedMedal] );
		if ( strcmp( m_szFooterCurrentMedal, szMedal ) != 0 )
		{
			m_FooterCurrentMedal.SetImage( szMedal );
			V_strncpy( m_szFooterCurrentMedal, szMedal, sizeof( m_szFooterCurrentMedal ) );
		}
	}
	else
	{
		m_FooterCurrentMedal.Cleanup();
		m_szFooterCurrentMedal[0] = '\0';
	}

	// Update cached next goal medal texture
	if ( iNextMedal >= 0 && iNextMedal < 4 )
	{
		char szMedal[256];
		V_snprintf( szMedal, sizeof( szMedal ), "vgui/medals/medal_0%i_%s", nMatchedType, s_pszMedalNames[iNextMedal] );
		if ( strcmp( m_szFooterNextMedal, szMedal ) != 0 )
		{
			m_FooterNextMedal.SetImage( szMedal );
			V_strncpy( m_szFooterNextMedal, szMedal, sizeof( m_szFooterNextMedal ) );
		}
	}
	else
	{
		m_FooterNextMedal.Cleanup();
		m_szFooterNextMedal[0] = '\0';
	}

	float flBaseX = m_flFooterButtonsOffsetX + m_flFooterButtonsSpacing + (float)m_nFooterButtonWidth;
	float flBaseY = (float)nParentH - m_flFooterButtonsOffsetY - (float)m_nFooterButtonHeight;

	int nBestW = 0, nBestH = 0;
	if ( iBest != -1 && g_pVGuiLocalize )
	{
		char szNum[256];
		V_snprintf( szNum, sizeof( szNum ), "%i", iBest );
		wchar_t wszNum[256];
		V_UTF8ToUnicode( szNum, wszNum, sizeof( wszNum ) );

		const wchar_t *pwszBestFmt = g_pVGuiLocalize->Find( "#GameUI_BonusMapsBest" );
		wchar_t wszBest[256];
		g_pVGuiLocalize->ConstructString( wszBest, sizeof( wszBest ), pwszBestFmt ? pwszBestFmt : L"Best: %s1", 1, wszNum );

		if ( m_FooterCurrentMedal.GetTextureId() > 0 )
		{
			vgui::surface()->DrawSetTexture( m_FooterCurrentMedal );
			vgui::surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
			vgui::surface()->DrawTexturedRect(
				(int)flBaseX,
				(int)flBaseY,
				(int)( flBaseX + m_flFooterMedalSize ),
				(int)( flBaseY + m_flFooterMedalSize ) );
		}

		vgui::surface()->DrawSetTextColor( Color( 255, 255, 255, 255 ) );
		vgui::surface()->DrawSetTextFont( m_hGoalFont );
		vgui::surface()->DrawSetTextPos( (int)( flBaseX + m_flFooterMedalSize + m_flFooterButtonsSpacing ), (int)flBaseY );
		vgui::surface()->GetTextSize( m_hGoalFont, wszBest, nBestW, nBestH );
		vgui::surface()->DrawPrintText( wszBest, (int)wcslen( wszBest ) );
	}

	int nGoalW = 0, nGoalH = 0;
	if ( iNext != -1 && g_pVGuiLocalize )
	{
		char szNum[256];
		V_snprintf( szNum, sizeof( szNum ), "%i", iNext );
		wchar_t wszNum[256];
		V_UTF8ToUnicode( szNum, wszNum, sizeof( wszNum ) );

		const wchar_t *pwszGoalFmt = g_pVGuiLocalize->Find( "#GameUI_BonusMapsGoal" );
		wchar_t wszGoal[256];
		g_pVGuiLocalize->ConstructString( wszGoal, sizeof( wszGoal ), pwszGoalFmt ? pwszGoalFmt : L"Goal: %s1", 1, wszNum );

		vgui::surface()->DrawSetTextColor( Color( 255, 255, 255, 255 ) );
		vgui::surface()->DrawSetTextFont( m_hGoalFont );
		vgui::surface()->DrawSetTextPos( (int)( flBaseX + m_flFooterMedalSize + m_flFooterButtonsSpacing ), (int)( flBaseY + (float)nBestH ) );
		vgui::surface()->GetTextSize( m_hGoalFont, wszGoal, nGoalW, nGoalH );
		vgui::surface()->DrawPrintText( wszGoal, (int)wcslen( wszGoal ) );

		if ( m_FooterNextMedal.GetTextureId() > 0 )
		{
			vgui::surface()->DrawSetTexture( m_FooterNextMedal );
			vgui::surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
			float flMedalX = flBaseX + m_flFooterMedalSize + ( 2.0f * m_flFooterButtonsSpacing ) + (float)MAX( nBestW, nGoalW );
			float flMedalY = flBaseY + (float)nBestH;
			vgui::surface()->DrawTexturedRect(
				(int)flMedalX,
				(int)flMedalY,
				(int)( flMedalX + m_flFooterMedalNextSize ),
				(int)( flMedalY + m_flFooterMedalNextSize ) );
		}
	}
}

void GamepadUIBonusMapsPanel::OnMouseWheeled( int nDelta )
{
}

void GamepadUIBonusMapsPanel::OnGamepadUIButtonNavigatedTo( vgui::VPANEL button )
{
}

void GamepadUIBonusMapsPanel::OnCommand( const char *pCommand )
{
	if ( !V_strcmp( pCommand, "action_back" ) )
	{
		IGameUI *pGameUI = GamepadUI::GetInstance().GetGameUI();
		CBonusMapsDatabase *pDB = pGameUI ? pGameUI->GetBonusMapsDatabase() : NULL;
		if ( !pDB || !V_strcmp( pDB->GetPath(), "." ) )
		{
			Close();
		}
		else
		{
			pDB->BackPath();
			BuildMapsList();
		}
	}
	else if ( !V_strcmp( pCommand, "action_challenges" ) )
	{
		CycleSelectedChallenge();
	}
	else if ( !V_strcmp( pCommand, "action_map" ) )
	{
		GamepadUIBonusButton *pSelected = GamepadUIBonusButton::s_pSelectedButton;
		if ( !pSelected )
			return;

		IGameUI *pGameUI = GamepadUI::GetInstance().GetGameUI();
		CBonusMapsDatabase *pDB = pGameUI ? pGameUI->GetBonusMapsDatabase() : NULL;
		if ( !pDB )
			return;

		int nMapIndex = pSelected->GetPriority();
		if ( !pDB->IsValidIndex( nMapIndex ) )
			return;

		BonusMapDescription_t *pBonusMap = pDB->GetBonusData( nMapIndex );
		if ( !pBonusMap || pBonusMap->bLocked || !pBonusMap->szShortName[0] )
			return;

		if ( pBonusMap->bIsFolder )
		{
			pDB->AppendPath( pBonusMap->szShortName );
			BuildMapsList();
		}
		else
		{
			char szCmd[256];
			szCmd[0] = '\0';

			ConVarRef commentary( "commentary" );
			if ( commentary.IsValid() )
				commentary.SetValue( 0 );

			ConVarRef sv_cheats( "sv_cheats" );
			if ( sv_cheats.IsValid() )
				sv_cheats.SetValue( 0 );

			IVEngineClient *pEngine = GamepadUI::GetInstance().GetEngineClient();

			if ( pSelected->GetBonusDescription().m_pChallenges )
			{
				int iChallenge = clamp( gamepadui_selected_challenge.GetInt(), 0, 2 );
				V_snprintf( szCmd, sizeof( szCmd ), "sv_bonus_challenge %i\n", iChallenge + 1 );
				if ( pEngine )
				{
					pEngine->ClientCmd_Unrestricted( szCmd );
				}

				if ( pBonusMap->m_pChallenges && pBonusMap->m_pChallenges->IsValidIndex( iChallenge ) )
				{
					ChallengeDescription_t *pChallengeDesc = &( (*pBonusMap->m_pChallenges)[iChallenge] );
					pDB->SetCurrentChallengeObjectives( pChallengeDesc->iBronze, pChallengeDesc->iSilver, pChallengeDesc->iGold );
					pDB->SetCurrentChallengeNames( pBonusMap->szFileName, pBonusMap->szMapName, pChallengeDesc->szName );
				}
			}

			if ( pBonusMap->szMapFileName[0] != '.' )
			{
				V_snprintf( szCmd, sizeof( szCmd ), "map %s\n", pBonusMap->szMapFileName );
			}
			else
			{
				const char *pchPath = pDB->GetPath();
				const char *pchSubDir = V_strnchr( pchPath, '/', (int)strlen( pchPath ) );
				if ( pchSubDir )
				{
					pchSubDir = V_strnchr( pchSubDir + 1, '/', (int)strlen( pchSubDir + 1 ) );
					if ( pchSubDir )
					{
						++pchSubDir;
						const char *pchMapFileName = pBonusMap->szMapFileName + 2;
						V_snprintf( szCmd, sizeof( szCmd ), "map %s/%s\n", pchSubDir, pchMapFileName );
					}
				}
			}

			if ( pEngine )
			{
				pEngine->ClientCmd_Unrestricted( szCmd );
			}
			OnClose();
		}
	}
	else
	{
		BaseClass::OnCommand( pCommand );
	}
}

CON_COMMAND( gamepadui_openbonusmapsdialog, "" )
{
	new GamepadUIBonusMapsPanel( GamepadUI::GetInstance().GetBasePanel(), "" );
}
