//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine Workshop interface (anniversary update).
//
//			Shape taken from the retail binaries: CWorkshop derives from
//			CBaseAppSystem<IWorkshop> (Linux engine.so RTTI) and has 23 virtuals
//			after the IAppSystem ones. Only the slots the retail GameUI calls are
//			named; the others keep their position until the engine is written.
//
//===========================================================================//

#ifndef IWORKSHOP_H
#define IWORKSHOP_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/IAppSystem.h"
#include "tier1/utlvector.h"
#include "steam/isteamugc.h"

class KeyValues;

#define VWORKSHOP_INTERFACE_VERSION "VWorkshop001"

struct WorkshopItem_t
{
	SteamUGCDetails_t	m_Details;
	uint8				m_Reserved[0x2c];
	KeyValues			*m_pCampaignData;
};

abstract_class IWorkshop : public IAppSystem
{
public:
	virtual void			Unknown05() = 0;
	virtual void			Unknown06() = 0;
	virtual void			Unknown07() = 0;
	virtual void			Unknown08() = 0;
	virtual WorkshopItem_t	*GetItem( PublishedFileId_t nItemID ) = 0;
	virtual void			Unknown10() = 0;
	virtual void			Unknown11() = 0;
	virtual void			Unknown12() = 0;
	virtual void			Unknown13() = 0;
	virtual void			Unknown14() = 0;
	virtual void			Unknown15() = 0;
	virtual void			Unknown16() = 0;
	virtual void			Unknown17() = 0;
	virtual void			Unknown18() = 0;
	virtual void			GetCurrentCampaign( CUtlVector< PublishedFileId_t > &items, PublishedFileId_t &nCampaignID ) = 0;
	virtual void			Unknown20() = 0;
	virtual void			Unknown21() = 0;
	virtual void			Unknown22() = 0;
	virtual void			Unknown23() = 0;
	virtual void			Unknown24() = 0;
	virtual void			Unknown25() = 0;
	virtual void			Unknown26() = 0;
	virtual void			Unknown27() = 0;
};

extern IWorkshop *workshop;

#endif // IWORKSHOP_H
