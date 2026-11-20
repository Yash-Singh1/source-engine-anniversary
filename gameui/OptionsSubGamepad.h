//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef OPTIONS_SUB_GAMEPAD_H
#define OPTIONS_SUB_GAMEPAD_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>

class CCvarSlider;

namespace vgui
{
	class Button;
	class ComboBox;
	class Label;
	class Panel;
}

//-----------------------------------------------------------------------------
// Purpose: Gamepad Details, Part of OptionsDialog
//-----------------------------------------------------------------------------
class COptionsSubGamepad : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE( COptionsSubGamepad, vgui::PropertyPage );

public:
	COptionsSubGamepad( vgui::Panel *parent );
	~COptionsSubGamepad();

	virtual void OnResetData();
	virtual void OnApplyChanges();

protected:
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void OnCommand( const char *command );
	void UpdateAutoaimControls();

	MESSAGE_FUNC_PTR( OnControlModified, "ControlModified", panel );
	MESSAGE_FUNC_PTR( OnTextChanged, "TextChanged", panel );
	MESSAGE_FUNC_PTR( OnCheckButtonChecked, "CheckButtonChecked", panel )
	{
		OnControlModified( panel );
	}

private:
	vgui::ComboBox		*m_pAutoaimCombo;
	vgui::Label			*m_pAutoaimScaleLabel;
	CCvarSlider			*m_pAutoaimScaleSlider;
	CCvarSlider			*m_pJoyScaleSlider;
	vgui::ComboBox		*m_pWeaponSelectionCombo;
	vgui::Button		*m_pSteamInputButton;
};

#endif // OPTIONS_SUB_GAMEPAD_H
