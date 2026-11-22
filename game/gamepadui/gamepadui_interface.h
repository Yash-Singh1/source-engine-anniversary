//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI module singleton and IGamepadUI interface implementation.
//
//===========================================================================//

#ifndef GAMEPADUI_INTERFACE_H
#define GAMEPADUI_INTERFACE_H

#ifdef _WIN32
#pragma once
#endif

#include "igamepadui.h"
#include "gamepadui_util.h"
#include "tier1/interface.h"

class IEngineSound;
class IEngineVGui;
class IGameUIFuncs;
class IMaterialSystem;
class IMatSystemSurface;
class ISoundEmitterSystemBase;
class IVEngineClient;
class IVRenderView;
class IWorkshop;
class IGameUI;
class IAchievementMgr;
class IFileSystem;

namespace vgui
{
	class AnimationController;
	class Panel;
}

class GamepadUIBasePanel;
class GamepadUIFrame;
class GamepadUIMainMenu;

// Forward declarations for the 6 feature panels (implemented in separate translation units)
class GamepadUINewGamePanel;
class GamepadUISaveGamePanel;
class GamepadUIAchievementsPanel;
class GamepadUIBonusMapsPanel;
class GamepadUIOptionsPanel;
class GamepadUIWorkshopPanel;

//-----------------------------------------------------------------------------
// Purpose: Main GamepadUI singleton implementing IGamepadUI ("GamepadUI001")
//-----------------------------------------------------------------------------
class GamepadUI : public IGamepadUI
{
public:
	GamepadUI();
	virtual ~GamepadUI();

	// IGamepadUI overrides
	virtual void Initialize( CreateInterfaceFn factory ) OVERRIDE;
	virtual void Shutdown() OVERRIDE;
	virtual void OnUpdate( float flFrametime ) OVERRIDE;
	virtual void OnLevelInitializePreEntity() OVERRIDE;
	virtual void OnLevelInitializePostEntity() OVERRIDE;
	virtual void OnLevelShutdown() OVERRIDE;

	// Singleton accessor
	static GamepadUI &GetInstance();

	// Engine / VGUI / Steam interface accessors
	IEngineSound			*GetEngineSound() const { return m_pEngineSound; }
	IEngineVGui				*GetEngineVGui() const { return m_pEngineVGui; }
	IGameUIFuncs			*GetGameUIFuncs() const { return m_pGameUIFuncs; }
	IMaterialSystem			*GetMaterialSystem() const { return m_pMaterialSystem; }
	IMatSystemSurface		*GetMatSystemSurface() const { return m_pMatSystemSurface; }
	ISoundEmitterSystemBase	*GetSoundEmitterSystem() const { return m_pSoundEmitterSystem; }
	IVEngineClient			*GetEngineClient() const { return m_pEngineClient; }
	IVRenderView			*GetRenderView() const { return m_pRenderView; }
	IWorkshop				*GetWorkshop() const { return m_pWorkshop; }
	IGameUI					*GetGameUI() const { return m_pGameUI; }
	IAchievementMgr			*GetAchievementMgr() const { return m_pAchievementMgr; }
	vgui::AnimationController *GetAnimationController() const { return m_pAnimationController; }
	IFileSystem				*GetFileSystem() const;

	// Panel accessors
	vgui::VPANEL			GetRootVPanel() const;
	GamepadUIBasePanel		*GetBasePanel() const { return m_pBasePanel; }
	vgui::VPANEL			GetBaseVPanel() const;
	GamepadUIMainMenu		*GetMainMenu() const;
	vgui::VPANEL			GetMainMenuVPanel() const;
	GamepadUIFrame			*GetMainMenuFrame() const;

	// Game state queries (exact retail behavior)
	bool					IsInLevel() const;
	bool					IsInBackgroundLevel() const;
	bool					IsInMultiplayer() const;
	bool					IsInMainMenu() const;

	// Rendering / timing / layout helpers
	void					UpdateGradients();
	GamepadUIGradientHelper	&GetGradientHelper() { return m_GradientHelper; }
	const GamepadUIGradientHelper &GetGradientHelper() const { return m_GradientHelper; }
	GamepadUIImage			*GetLogoTexture() const;
	float					GetTime() const;
	void					GetScreenSize( int &nWide, int &nTall ) const;
	float					GetScreenRatio() const;

private:
	IEngineSound				*m_pEngineSound;			// +0x04
	IEngineVGui					*m_pEngineVGui;				// +0x08
	IGameUIFuncs				*m_pGameUIFuncs;			// +0x0c
	IMaterialSystem				*m_pMaterialSystem;			// +0x10
	IMatSystemSurface			*m_pMatSystemSurface;		// +0x14
	ISoundEmitterSystemBase		*m_pSoundEmitterSystem;		// +0x18
	IVEngineClient				*m_pEngineClient;			// +0x1c
	IVRenderView				*m_pRenderView;				// +0x20
	IWorkshop					*m_pWorkshop;				// +0x24
	IGameUI						*m_pGameUI;					// +0x28
	IAchievementMgr				*m_pAchievementMgr;			// +0x2c
	vgui::AnimationController	*m_pAnimationController;	// +0x30
	GamepadUIBasePanel			*m_pBasePanel;				// +0x34
	GamepadUIGradientHelper		m_GradientHelper;			// +0x38..+0x88
};

#endif // GAMEPADUI_INTERFACE_H
