//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef BENCHMARKDIALOG_H
#define BENCHMARKDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/Frame.h"

//-----------------------------------------------------------------------------
// Purpose: Benchmark launch dialog
//-----------------------------------------------------------------------------
class CBenchmarkDialog : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE( CBenchmarkDialog, vgui::Frame );
public:
	CBenchmarkDialog(vgui::Panel *parent, const char *name);

	void OnKeyCodePressed( vgui::KeyCode code )
	{
		vgui::KeyCode baseCode = GetBaseButtonCode( code );
		if ( baseCode == KEY_XBUTTON_B || baseCode == STEAMCONTROLLER_B )
		{
			Close();
		}
		else if ( baseCode == KEY_XBUTTON_A || baseCode == STEAMCONTROLLER_A )
		{
			RunBenchmark();
		}
		else
		{
			BaseClass::OnKeyCodePressed(code);
		}
	}
	
private:
	MESSAGE_FUNC( RunBenchmark, "RunBenchmark" );
};


#endif // BENCHMARKDIALOG_H
