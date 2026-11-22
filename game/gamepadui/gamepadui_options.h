//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: GamepadUI Options Panel and option control widgets
//          (KeyButton, ConvarButton, WheelyWheel, SlideySlide, SkillySkill).
//          Decompiled from retail HL2 20th Anniversary gamepadui.so.
//
//===========================================================================//

#ifndef GAMEPADUI_OPTIONS_H
#define GAMEPADUI_OPTIONS_H

#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_frame.h"
#include "gamepadui_button.h"
#include "gamepadui_util.h"
#include "tier1/convar.h"
#include "tier1/utlstring.h"
#include "tier1/utlsymbol.h"
#include "tier1/utlvector.h"

//-----------------------------------------------------------------------------
// Purpose: Keybinding row button that enters engine key-trap mode on activate
//          Retail vtbl: 0x299ae0 (TI 0x298a3c)
//-----------------------------------------------------------------------------
class GamepadUIKeyButton : public GamepadUIButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIKeyButton, GamepadUIButton );

	GamepadUIKeyButton(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const char *pText,
		const char *pDescription,
		const char *pAction );
	virtual ~GamepadUIKeyButton();

	virtual ButtonState::State GetCurrentButtonState() OVERRIDE;
	virtual void OnMouseDoublePressed( vgui::MouseCode code ) OVERRIDE;
	virtual void OnKeyCodePressed( vgui::KeyCode code ) OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void Paint() OVERRIDE;

	const char *GetAction() const { return m_strAction.String(); }
	const char *GetKey() const { return m_strKey.String(); }
	void SetKey( const char *pszKey ) { m_strKey = pszKey ? pszKey : ""; }
	bool IsBinding() const { return m_bBinding; }

	static bool s_bBindingKey;

private:
	void BeginKeyBinding();

	CUtlString	m_strAction;		// +0x43c
	CUtlString	m_strKey;			// +0x44c
	bool		m_bBinding;			// +0x45c
	int			m_nSavedCursorX;	// +0x460
	int			m_nSavedCursorY;	// +0x464

	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flBindingWidth, "Button.Binding.Width", "40" );	// +0x46c (Button.Binding.Width)
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flBindingHeight, "Button.Binding.Height", "40" );	// +0x474 (Button.Binding.Height)
	GAMEPADUI_PANEL_PROPERTY_COLOR( m_colBinding, "Button.Binding", "0 0 0 255" );				// +0x479 (Button.Binding)
};

//-----------------------------------------------------------------------------
// Purpose: Abstract base class for ConVar-bound option controls
//          Retail vtbl: 0x29afc4 / 0x29c958 (TI 0x298a48)
//-----------------------------------------------------------------------------
class GamepadUIConvarButton : public GamepadUIButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIConvarButton, GamepadUIButton );

	GamepadUIConvarButton(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const char *pText,
		const char *pDescription,
		const char *pConvarName,
		const char *pDependsOn,
		bool bInstantApply );
	virtual ~GamepadUIConvarButton();

	virtual void ApplyConvar() = 0;
	virtual bool IsConvarDirty() = 0;
	virtual void UpdateConvar() = 0;
	virtual bool IsTruthy() { return true; }

	const char *GetConvarName() const;
	const char *GetDependsOn() const { return m_strDependsOn.String(); }
	bool IsInstantApply() const { return m_bInstantApply; }

protected:
	ConVarRef	m_ConVarRef;		// +0x43c
	CUtlString	m_strDependsOn;		// +0x444
	bool		m_bInstantApply;	// +0x454
};

//-----------------------------------------------------------------------------
// Purpose: Horizontal option carousel (< Value >) bound to a ConVar
//          Retail vtbl: 0x29b464 (TI 0x298a54)
//-----------------------------------------------------------------------------
class GamepadUIWheelyWheel : public GamepadUIConvarButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIWheelyWheel, GamepadUIConvarButton );

	struct Option_t
	{
		GamepadUIString	m_strName;	// +0x00 (0x18 bytes on i386)
		int				m_nValue;	// +0x18
		int				m_nWidth;	// +0x1c (video resolution width)
		int				m_nHeight;	// +0x20 (video resolution height)

		Option_t() : m_nValue( 0 ), m_nWidth( 0 ), m_nHeight( 0 ) {}
	};

	GamepadUIWheelyWheel(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const char *pText,
		const char *pDescription,
		const char *pConvarName,
		const char *pDependsOn,
		bool bInstantApply,
		bool bSignOnly );
	virtual ~GamepadUIWheelyWheel();

	virtual void OnKeyCodePressed( vgui::KeyCode code ) OVERRIDE;
	virtual void Paint() OVERRIDE;

	virtual void ApplyConvar() OVERRIDE;
	virtual bool IsConvarDirty() OVERRIDE;
	virtual void UpdateConvar() OVERRIDE;
	virtual bool IsTruthy() OVERRIDE;

	void AddOption( const char *pszLocalizedOrRawName, int nValue, int nWidth = 0, int nHeight = 0 );
	void AddOptionW( const wchar_t *pwszName, int nValue, int nWidth = 0, int nHeight = 0 );
	void ClearOptions() { m_Options.RemoveAll(); }
	int GetOptionCount() const { return m_Options.Count(); }
	const Option_t *GetOption( int nIndex ) const;
	int GetCurrentOptionIndex() const { return m_nCurrentOption; }

private:
	bool					m_bSignOnly;		// +0x458
	int						m_nCurrentOption;	// +0x45c
	CUtlVector<Option_t>	m_Options;			// +0x460
};

//-----------------------------------------------------------------------------
// Purpose: Horizontal slider bar control bound to a float ConVar
//          Retail vtbl: 0x29cdf8 (TI 0x298a60)
//-----------------------------------------------------------------------------
class GamepadUISlideySlide : public GamepadUIConvarButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUISlideySlide, GamepadUIConvarButton );

	GamepadUISlideySlide(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const char *pText,
		const char *pDescription,
		const char *pConvarName,
		const char *pDependsOn,
		bool bInstantApply,
		float flMin,
		float flMax,
		float flStep,
		int nTextPrecision );
	virtual ~GamepadUISlideySlide();

	virtual void OnKeyCodePressed( vgui::KeyCode code ) OVERRIDE;
	virtual void Paint() OVERRIDE;
	virtual void RunAnimations( ButtonState::State state ) OVERRIDE;

	virtual void ApplyConvar() OVERRIDE;
	virtual bool IsConvarDirty() OVERRIDE;
	virtual void UpdateConvar() OVERRIDE;

private:
	float	m_flValue;			// +0x458
	float	m_flMin;			// +0x45c
	float	m_flMax;			// +0x460
	float	m_flStep;			// +0x464
	int		m_nTextPrecision;	// +0x468

	CPanelAnimationVar( Color, m_colSliderBacking, "m_colSliderBacking", "255 255 255 22" );
	Color	m_colSliderBackings[ButtonState::Count];
	float	m_flSliderBackingAnimationDuration;

	CPanelAnimationVar( Color, m_colSliderFill, "m_colSliderFill", "255 255 255 255" );
	Color	m_colSliderFills[ButtonState::Count];
	float	m_flSliderFillAnimationDuration;

	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flSliderWidth, "Slider.Width", "160" );	// +0x4a0 (Slider.Width)
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flSliderHeight, "Slider.Height", "11" );	// +0x4a8 (Slider.Height)
};

//-----------------------------------------------------------------------------
// Purpose: Horizontal difficulty card button (Easy / Medium / Hard)
//          Retail vtbl: 0x29e2ec (TI 0x298a6c)
//-----------------------------------------------------------------------------
class GamepadUISkillySkill : public GamepadUIButton
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUISkillySkill, GamepadUIButton );

	GamepadUISkillySkill(
		vgui::Panel *pParent,
		vgui::Panel *pActionSignalTarget,
		const char *pSchemeFile,
		const char *pCommand,
		const char *pText,
		const char *pDescription,
		const char *pImage,
		int nSkill );
	virtual ~GamepadUISkillySkill();

	virtual ButtonState::State GetCurrentButtonState() OVERRIDE;
	virtual void NavigateTo() OVERRIDE;
	virtual void DoClick() OVERRIDE;
	virtual void RunAnimations( ButtonState::State state ) OVERRIDE;
	virtual void Paint() OVERRIDE;

private:
	GamepadUIImage	m_Image;	// +0x43c
	int				m_nSkill;	// +0x444

	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flImageWidth, "Button.Image.Width", "40" );	// +0x44c (Button.Image.Width)
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flImageHeight, "Button.Image.Height", "40" );	// +0x454 (Button.Image.Height)

	CPanelAnimationVar( Color, m_colImage, "m_colImage", "255 255 255 255" );
	Color	m_colImages[ButtonState::Count];
	float	m_flImageAnimationDuration;

	CPanelAnimationVar( Color, m_colTitleBackground, "m_colTitleBackground", "0 0 0 0" );
	Color	m_colTitleBackgrounds[ButtonState::Count];
	float	m_flTitleBackgroundAnimationDuration;

	CPanelAnimationVar( Color, m_colDescriptionBackground, "m_colDescriptionBackground", "0 0 0 0" );
	Color	m_colDescriptionBackgrounds[ButtonState::Count];
	float	m_flDescriptionBackgroundAnimationDuration;
};

//-----------------------------------------------------------------------------
// Purpose: Tabbed Options Panel reading gamepadui/options.res
//          Retail vtbl: 0x29f744 (TI 0x298a30), size 0x530 on i386
//-----------------------------------------------------------------------------
class GamepadUIOptionsPanel : public GamepadUIFrame
{
public:
	DECLARE_CLASS_SIMPLE( GamepadUIOptionsPanel, GamepadUIFrame );

	static const int MAX_OPTION_TABS = 8;

	GamepadUIOptionsPanel( vgui::Panel *pParent, const char *pPanelName );
	virtual ~GamepadUIOptionsPanel();

	virtual void UpdateGradients() OVERRIDE;
	virtual void Paint() OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void OnMouseWheeled( int nDelta ) OVERRIDE;
	virtual void OnKeyCodePressed( vgui::KeyCode code ) OVERRIDE;
	virtual void OnCommand( const char *pCommand ) OVERRIDE;

	MESSAGE_FUNC_INT( OnGamepadUIButtonNavigatedTo, "OnGamepadUIButtonNavigatedTo", button );

	void PopulateResolutions();
	void ClearDuplicateKeyBind( const char *pszKey );

	static GamepadUIOptionsPanel *s_pOptionsPanel;

private:
	struct OptionTab_t
	{
		GamepadUIButton					*m_pTabButton;		// +0x00
		CUtlVector<GamepadUIButton *>	m_Buttons;			// +0x04
		GamepadUIScrollState			m_ScrollState;		// +0x18
		bool							m_bAlternating;		// +0x28
		bool							m_bHorizontal;		// +0x29
		CUtlVector<CUtlSymbol>			m_BoundKeys;		// +0x2c

		OptionTab_t()
			: m_pTabButton( NULL )
			, m_bAlternating( false )
			, m_bHorizontal( false )
		{
		}
	};

	void BuildOptions( const char *pszOptionsFile );
	void PopulateKeyBindings();
	void ApplyKeyBindings();
	void LayoutOptions();
	void PaintOptionsDecorations();
	void ChangeTab( int nTab );
	int GetCurrentTab() const;

	static void PopulateAAModes();
	static void UpdateAllProxyConvars();
	static void ApplyAllProxyConvars();

	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flTabsOffsetX, "Tabs.OffsetX", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flTabsOffsetY, "Tabs.OffsetY", "0" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flOptionsFade, "Options.Fade", "80" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flScrollBarOffsetX, "Scrollbar.OffsetX", "10" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flScrollBarWidth, "Scrollbar.Width", "80" );
	GAMEPADUI_PANEL_PROPERTY_PROPORTIONAL( m_flScrollBarHeight, "Scrollbar.Height", "80" );

	OptionTab_t				m_Tabs[MAX_OPTION_TABS];	// +0x310
	int						m_nTabCount;				// +0x510
	GamepadUIWheelyWheel	*m_pResolutionButton;		// +0x514
	GamepadUIGlyph			m_GlyphLB;					// +0x518
	GamepadUIGlyph			m_GlyphRB;					// +0x524
};

#endif // GAMEPADUI_OPTIONS_H
