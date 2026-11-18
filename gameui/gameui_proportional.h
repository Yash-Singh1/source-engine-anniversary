//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: PROPORTIONAL_VALUE for trees whose vgui_controls/Panel.h lacks it
//
//=============================================================================//

#ifndef GAMEUI_PROPORTIONAL_H
#define GAMEUI_PROPORTIONAL_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>

#ifndef PROPORTIONAL_VALUE
namespace vgui
{
inline int GetProportionalScaledValueIf( ISchemeManager *pSchemeManager, bool bProportional, HScheme hScheme, int nValue )
{
	return bProportional ? pSchemeManager->GetProportionalScaledValueEx( hScheme, nValue ) : nValue;
}
}

#define PROPORTIONAL_VALUE( nValue ) vgui::GetProportionalScaledValueIf( vgui::scheme(), IsProportional(), GetScheme(), ( nValue ) )
#endif

#endif // GAMEUI_PROPORTIONAL_H
