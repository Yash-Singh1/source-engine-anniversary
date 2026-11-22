//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI module singleton and IGamepadUI interface implementation.
//
//===========================================================================//

#include "gamepadui_interface.h"
#include "gamepadui_basepanel.h"
#include "gamepadui_frame.h"
#include "gamepadui_mainmenu.h"

#include "tier0/platform.h"
#include "tier1/tier1.h"
#include "tier1/convar.h"
#include "tier1/interface.h"
#include "tier2/tier2.h"
#include "tier3/tier3.h"
#include "filesystem.h"
#include "cdll_int.h"
#include "engine/IEngineSound.h"
#include "ienginevgui.h"
#include "IGameUIFuncs.h"
#include "materialsystem/imaterialsystem.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "ivrenderview.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"
#include "iworkshop.h"
#include "iachievementmgr.h"
#include "GameUI/IGameUI.h"
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui_controls/AnimationController.h"
#include "vgui_controls/Controls.h"

#if !defined( NO_STEAM )
#include "steam/steam_api.h"
#endif

#include "tier0/memdbgon.h"

#if defined( PLATFORM_64BITS )
IVEngineClient *engine = NULL;
#endif

static GamepadUI *s_pGamepadUIInstance = NULL;
static CDllDemandLoader s_GameUIDLL( "GameUI" );

//-----------------------------------------------------------------------------
// Interface factory export matching EXPOSE_SINGLE_INTERFACE("GamepadUI001")
//-----------------------------------------------------------------------------
static void *CreateGamepadUIInterface()
{
	return &GamepadUI::GetInstance();
}

EXPOSE_INTERFACE_FN( CreateGamepadUIInterface, IGamepadUI, GAMEPADUI_INTERFACE_VERSION );

//-----------------------------------------------------------------------------
// GamepadUI
//-----------------------------------------------------------------------------
GamepadUI::GamepadUI()
	: m_pEngineSound( NULL )
	, m_pEngineVGui( NULL )
	, m_pGameUIFuncs( NULL )
	, m_pMaterialSystem( NULL )
	, m_pMatSystemSurface( NULL )
	, m_pSoundEmitterSystem( NULL )
	, m_pEngineClient( NULL )
	, m_pRenderView( NULL )
	, m_pWorkshop( NULL )
	, m_pGameUI( NULL )
	, m_pAchievementMgr( NULL )
	, m_pAnimationController( NULL )
	, m_pBasePanel( NULL )
{
}

GamepadUI::~GamepadUI()
{
}

GamepadUI &GamepadUI::GetInstance()
{
	if ( !s_pGamepadUIInstance )
	{
		s_pGamepadUIInstance = new GamepadUI();
	}
	return *s_pGamepadUIInstance;
}

void GamepadUI::Initialize( CreateInterfaceFn factory )
{
	ConnectTier1Libraries( &factory, 1 );
	ConnectTier2Libraries( &factory, 1 );
	ConVar_Register( FCVAR_CLIENTDLL );
	ConnectTier3Libraries( &factory, 1 );

	m_pEngineClient			= static_cast<IVEngineClient *>( factory( VENGINE_CLIENT_INTERFACE_VERSION, NULL ) );
#if defined( PLATFORM_64BITS )
	engine					= m_pEngineClient;
#endif
	m_pEngineSound			= static_cast<IEngineSound *>( factory( IENGINESOUND_CLIENT_INTERFACE_VERSION, NULL ) );
	m_pEngineVGui			= static_cast<IEngineVGui *>( factory( VENGINE_VGUI_VERSION, NULL ) );
	m_pGameUIFuncs			= static_cast<IGameUIFuncs *>( factory( VENGINE_GAMEUIFUNCS_VERSION, NULL ) );
	m_pMaterialSystem		= static_cast<IMaterialSystem *>( factory( MATERIAL_SYSTEM_INTERFACE_VERSION, NULL ) );
	m_pMatSystemSurface		= static_cast<IMatSystemSurface *>( factory( MAT_SYSTEM_SURFACE_INTERFACE_VERSION, NULL ) );
	m_pRenderView			= static_cast<IVRenderView *>( factory( VENGINE_RENDERVIEW_INTERFACE_VERSION, NULL ) );
	m_pSoundEmitterSystem	= static_cast<ISoundEmitterSystemBase *>( factory( SOUNDEMITTERSYSTEM_INTERFACE_VERSION, NULL ) );
	m_pWorkshop				= static_cast<IWorkshop *>( factory( VWORKSHOP_INTERFACE_VERSION, NULL ) );

	CreateInterfaceFn gameUIFactory = s_GameUIDLL.GetFactory();
#if defined( OSX ) && defined( PLATFORM_64BITS )
	if ( !gameUIFactory && g_pFullFileSystem )
	{
		CSysModule *pGameUIModule = g_pFullFileSystem->LoadModule( "GameUI", "EXECUTABLE_PATH", false );
		if ( pGameUIModule )
		{
			gameUIFactory = Sys_GetFactory( pGameUIModule );
		}
	}
#endif
	if ( gameUIFactory )
	{
		m_pGameUI = static_cast<IGameUI *>( gameUIFactory( GAMEUI_INTERFACE_VERSION, NULL ) );
	}

	m_pAchievementMgr = m_pEngineClient->GetAchievementMgr();

		if ( !m_pEngineClient ||
			 !m_pEngineSound ||
			 !m_pEngineVGui ||
			 !m_pGameUIFuncs ||
			 !m_pMaterialSystem ||
			 !m_pMatSystemSurface ||
			 !m_pRenderView ||
			 !m_pSoundEmitterSystem ||
			 !m_pGameUI ||
			 !m_pAchievementMgr )
		{
			GamepadUI_Log( "GamepadUI::Initialize() failed to get necessary interfaces.\n" );
#if defined( OSX ) && defined( PLATFORM_64BITS )
			GamepadUI_Log( "  engine=%p sound=%p vgui=%p gameuifuncs=%p mat=%p surface=%p render=%p soundemit=%p gameui=%p ach=%p\n",
				m_pEngineClient, m_pEngineSound, m_pEngineVGui, m_pGameUIFuncs, m_pMaterialSystem,
				m_pMatSystemSurface, m_pRenderView, m_pSoundEmitterSystem, m_pGameUI, m_pAchievementMgr );
#endif
			return;
		}

	g_pVGuiLocalize->AddFile( "resource/gameui_%language%.txt", "GAME", true );
	g_pVGuiLocalize->AddFile( "resource/deck_%language%.txt", "GAME", true );

#if !defined( NO_STEAM )
#if defined( VERSION_SAFE_STEAM_API_INTERFACES )
	SteamAPI_InitSafe();
#else
	SteamAPI_Init();
#endif
	SteamAPI_SetTryCatchCallbacks( false );
#endif

	m_pBasePanel = new GamepadUIBasePanel( GetRootVPanel() );
	if ( !m_pBasePanel )
	{
		GamepadUI_Log( "Failed to create BasePanel.\n" );
		return;
	}

	GamepadUI_Log( "Overiding menu.\n" );
	m_pGameUI->SetMainMenuOverride( GetBaseVPanel() );

	m_pAnimationController = new vgui::AnimationController( m_pBasePanel );
	m_pAnimationController->SetProportional( false );

	GetMainMenu()->Activate();
}

void GamepadUI::Shutdown()
{
	if ( m_pGameUI )
	{
		m_pGameUI->SetMainMenuOverride( 0 );
	}

	if ( m_pBasePanel )
	{
		m_pBasePanel->DeletePanel();
	}

	ConVar_Unregister();
	DisconnectTier3Libraries();
	DisconnectTier2Libraries();
	DisconnectTier1Libraries();
}

void GamepadUI::OnUpdate( float flFrametime )
{
	(void)flFrametime;
	if ( m_pAnimationController )
	{
		m_pAnimationController->UpdateAnimations( static_cast<float>( Plat_FloatTime() ) );
	}
}

void GamepadUI::OnLevelInitializePreEntity()
{
}

void GamepadUI::OnLevelInitializePostEntity()
{
#if defined( OSX ) && defined( PLATFORM_64BITS )
	if ( !m_pBasePanel )
		return;
#endif
	m_pBasePanel->UpdateBackgroundMusic();
	if ( GamepadUIMainMenu *pMainMenu = GetMainMenu() )
	{
		pMainMenu->OnMenuStateChanged();
	}
}

void GamepadUI::OnLevelShutdown()
{
	if ( m_pAnimationController )
	{
		m_pAnimationController->UpdateAnimations( static_cast<float>( Plat_FloatTime() ) );
		m_pAnimationController->CancelAllAnimations();
	}

#if defined( OSX ) && defined( PLATFORM_64BITS )
	if ( !m_pBasePanel )
		return;
#endif
	m_pBasePanel->UpdateBackgroundMusic();
	if ( GamepadUIMainMenu *pMainMenu = GetMainMenu() )
	{
		pMainMenu->OnMenuStateChanged();
	}
}

IFileSystem *GamepadUI::GetFileSystem() const
{
	return g_pFullFileSystem;
}

vgui::VPANEL GamepadUI::GetRootVPanel() const
{
	return m_pEngineVGui->GetPanel( PANEL_GAMEUIDLL );
}

vgui::VPANEL GamepadUI::GetBaseVPanel() const
{
	return m_pBasePanel ? m_pBasePanel->GetVPanel() : 0;
}

GamepadUIMainMenu *GamepadUI::GetMainMenu() const
{
	return m_pBasePanel ? m_pBasePanel->GetMainMenu() : NULL;
}

vgui::VPANEL GamepadUI::GetMainMenuVPanel() const
{
	return GetMainMenu() ? GetMainMenu()->GetVPanel() : 0;
}

GamepadUIFrame *GamepadUI::GetMainMenuFrame() const
{
	return GetMainMenu();
}

bool GamepadUI::IsInLevel() const
{
	const char *pszLevelName = m_pEngineClient->GetLevelName();
	if ( !pszLevelName || !pszLevelName[0] )
		return false;

	return !m_pEngineClient->IsLevelMainMenuBackground();
}

bool GamepadUI::IsInBackgroundLevel() const
{
	const char *pszLevelName = m_pEngineClient->GetLevelName();
	if ( !pszLevelName || !pszLevelName[0] )
		return false;

	return m_pEngineClient->IsLevelMainMenuBackground();
}

bool GamepadUI::IsInMultiplayer() const
{
	return IsInLevel() && ( m_pEngineClient->GetMaxClients() > 1 );
}

bool GamepadUI::IsInMainMenu() const
{
	if ( !IsInLevel() )
		return true;

	return IsInBackgroundLevel();
}

void GamepadUI::UpdateGradients()
{
	GetMainMenu()->UpdateGradients();
}

GamepadUIImage *GamepadUI::GetLogoTexture() const
{
	return &GetMainMenu()->GetLogoTexture();
}

float GamepadUI::GetTime() const
{
	return static_cast<float>( Plat_FloatTime() );
}

void GamepadUI::GetScreenSize( int &nWide, int &nTall ) const
{
	vgui::surface()->GetScreenSize( nWide, nTall );
}

float GamepadUI::GetScreenRatio() const
{
	int nWide, nTall;
	GetScreenSize( nWide, nTall );
	return static_cast<float>( nWide ) / static_cast<float>( nTall );
}
