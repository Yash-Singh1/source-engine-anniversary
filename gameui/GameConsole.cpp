//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//


#include <stdio.h>

#include "GameConsole.h"
#include "GameConsoleDialog.h"
#include "LoadingDialog.h"
#include "vgui/ISurface.h"

#include "KeyValues.h"
#include "vgui/VGUI.h"
#include "vgui/IVGui.h"
#include "vgui/IPanel.h"
#include "vgui_controls/Panel.h"
#include "convar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CGameConsole g_GameConsole;
//-----------------------------------------------------------------------------
// Purpose: singleton accessor
//-----------------------------------------------------------------------------
CGameConsole &GameConsole()
{
	return g_GameConsole;
}
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameConsole, IGameConsole, GAMECONSOLE_INTERFACE_VERSION, g_GameConsole);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CGameConsole::CGameConsole()
{
	m_bInitialized = false;
	m_bConsoleLaidOut = false;
	m_pConsole = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CGameConsole::~CGameConsole()
{
}

//-----------------------------------------------------------------------------
// Purpose: sets up the console for use
//-----------------------------------------------------------------------------
void CGameConsole::Initialize()
{
#ifndef _XBOX
	// the dialog is created once the console gets its parent
	if ( m_pConsole )
	{
		m_pConsole->Close();
		m_pConsole->DeletePanel();
		m_pConsole = NULL;
	}

	m_bInitialized = true;
	m_bConsoleLaidOut = false;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: activates the console, makes it visible and brings it to the foreground
//-----------------------------------------------------------------------------
void CGameConsole::Activate()
{
#ifndef _XBOX
	if (!m_bInitialized)
		return;

	vgui::surface()->RestrictPaintToSinglePanel(NULL);
	m_pConsole->Activate();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: hides the console
//-----------------------------------------------------------------------------
void CGameConsole::Hide()
{
#ifndef _XBOX
	if (!m_bInitialized)
		return;

	m_pConsole->Hide();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: clears the console
//-----------------------------------------------------------------------------
void CGameConsole::Clear()
{
#ifndef _XBOX
	if (!m_bInitialized)
		return;

	m_pConsole->Clear();
#endif
}


//-----------------------------------------------------------------------------
// Purpose: returns true if the console is currently in focus
//-----------------------------------------------------------------------------
bool CGameConsole::IsConsoleVisible()
{
#ifndef _XBOX
	if (!m_bInitialized)
		return false;
	
	return m_pConsole->IsVisible();
#else
	return false;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: activates the console after a delay
//-----------------------------------------------------------------------------
void CGameConsole::ActivateDelayed(float time)
{
#ifndef _XBOX
	if (!m_bInitialized)
		return;

	m_pConsole->PostMessage(m_pConsole, new KeyValues("Activate"), time);
#endif
}

void CGameConsole::SetParent( intp parent )
{	
#ifndef _XBOX
	if (!m_bInitialized)
		return;

	if ( !m_pConsole )
	{
		vgui::Panel *pParent = vgui::ipanel()->GetPanel( static_cast<vgui::VPANEL>( parent ), "BaseUI" );
		m_pConsole = new CGameConsoleDialog( pParent );
	}
	else
	{
		m_pConsole->SetParent( static_cast<vgui::VPANEL>( parent ));
	}

	if ( m_bConsoleLaidOut )
		return;

	m_pConsole->MakeReadyForUse(); // we add text before displaying this so set it up now!

	// set the console to taking up most of the right-half of the screen
	int swide, stall;
	vgui::surface()->GetScreenSize(swide, stall);
	int offsetx = vgui::scheme()->GetProportionalScaledValue(16);
	int offsety = vgui::scheme()->GetProportionalScaledValue(64);

	m_pConsole->SetBounds(
		swide / 2 - offsetx,
		offsety,
		swide / 2,
#ifdef BYTEMATCH_RETAIL
		stall - (offsety * 2));
#else
		(IsAndroid() ? 0.5f : 1.f )*(stall - (offsety * 2)));
#endif

	m_bConsoleLaidOut = true;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: static command handler
//-----------------------------------------------------------------------------
void CGameConsole::OnCmdCondump()
{
#ifndef _XBOX
	g_GameConsole.m_pConsole->DumpConsoleTextToFile();
#endif
}

#ifndef _XBOX
CON_COMMAND( condump, "dump the text currently in the console to condumpXX.log" )
{
	g_GameConsole.OnCmdCondump();
}
#endif
