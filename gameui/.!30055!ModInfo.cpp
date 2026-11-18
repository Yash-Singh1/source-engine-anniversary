//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//


#include "ModInfo.h"
#include "KeyValues.h"
#include "vgui_controls/Controls.h"
#include "filesystem.h"
#include "EngineInterface.h"
#include "tier0/icommandline.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

//-----------------------------------------------------------------------------
// Purpose: singleton accessor
//-----------------------------------------------------------------------------
CModInfo &ModInfo()
{
	static CModInfo s_ModInfo;
	return s_ModInfo;
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CModInfo::CModInfo()
{
	m_pModData = new KeyValues("ModData");
	m_wcsGameTitle[0] = 0;
	m_bHL2Complete = false;

	if ( !V_stricmp( CommandLine()->ParmValue( "-game", CommandLine()->ParmValue( "-defaultgamedir", "hl2" ) ), "hl2_complete" ) )
	{
		m_bHL2Complete = true;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CModInfo::~CModInfo()
{
	FreeModInfo();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CModInfo::FreeModInfo()
{
	if (m_pModData)
	{
		m_pModData->deleteThis();
		m_pModData = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
bool CModInfo::IsMultiplayerOnly()
{
	return (stricmp(m_pModData->GetString("type", ""), "multiplayer_only") == 0);
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
bool CModInfo::IsSinglePlayerOnly()
{
#ifndef _XBOX
	return (stricmp(m_pModData->GetString("type", ""), "singleplayer_only") == 0);
#else
	// xboxissue - no support for disparate mounted content
	return true;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
const char *CModInfo::GetFallbackDir()
{
	return m_pModData->GetString("fallback_dir", "");
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
const wchar_t *CModInfo::GetGameTitle()
{
	if (!m_wcsGameTitle[0])
	{
		// for some reason, the standard ILocalize::ConvertANSIToUnicode() strips off
