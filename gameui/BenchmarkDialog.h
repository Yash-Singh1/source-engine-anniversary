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
		switch ( GetBaseButtonCode( code ) )
		{
		case KEY_XBUTTON_B:
		case STEAMCONTROLLER_B:
			Close();
			break;
		case KEY_XBUTTON_A:
		case STEAMCONTROLLER_A:
			RunBenchmark();
			break;
		default:
			BaseClass::OnKeyCodePressed(code);
		}
	}
	
private:
	MESSAGE_FUNC( RunBenchmark, "RunBenchmark" );
};


#endif // BENCHMARKDIALOG_H
