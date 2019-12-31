/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents: Support for a list of locations that we may teleport the player to.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Teleport.cpp                                         $
 * 
 * 10    98.09.10 6:01p Mmouni
 * Changed so that the next teleport is reset on load.
 * 
 * 9     8/04/98 8:37a Shernd
 * Added new teleport functionality
 * 
 * 8     7/02/98 10:19p Pkeet
 * Teleporting purges caches.
 * 
 * 7     6/24/98 3:23p Rwyatt
 * After teleporting VM is synchronous for a few frames.
 * 
 * 6     98.06.05 11:24a Mmouni
 * The iterator is now reset when a teleport is deleted to prevent crashes.
 * 
 * 5     98.05.26 3:05p Mmouni
 * Removed terrain update hack.
 * 
 * 4     5/26/98 12:23p Rwycko
 * don't modify terrain if it does not exist
 * 
 * 3     98/05/23 21:10 Speter
 * Teleport now moves camera attached entity, if any; fixes Anne drunkenness.
 * 
 * 2     98.05.21 9:31p Mmouni
 * Added hack to update terrain after moving so that the pysics won't use the old un-refined
 * terrain.
 * 
 * 1     98.05.08 5:54p Mmouni
 * Initial version, still needs some cleanup.
 * 
 *********************************************************************************************/

#include "Common.hpp"
#include "Instance.hpp"
#include "Gameloop.hpp"

#include "Lib/W95/WinInclude.hpp"
#include "Lib/Renderer/RenderCache.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/Loader/DataDaemon.hpp"
#include "MessageTypes/MsgMove.hpp"
#include "Lib/GeomDBase/Shape.hpp"
#include "PhysicsInfo.hpp"
#include "Game/AI/AIInfo.hpp"
#include "Lib/EntityDBase/Teleport.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib\EntityDBase\Query\QTerrain.hpp"
#include "Lib/EntityDBase/RenderDB.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/Loader/SaveFile.hpp"

#include <algorithm>


//*****************************************************************************************
std::list<CTeleport*> CTeleport::listTeleports;
std::list<CTeleport*>::iterator CTeleport::itCurrent = CTeleport::listTeleports.end();


//*****************************************************************************************
CTeleport* CTeleport::NextTeleport()
{
	if (itCurrent == listTeleports.end())
		itCurrent = listTeleports.begin();

	// Return current teleport.
	if (itCurrent == listTeleports.end())
		return 0;
	else
		return (*itCurrent++);
}


//******************************************************************************************
CTeleport::CTeleport(const CPresence3<>& pr3)
{
	listTeleports.push_front(this);
	itCurrent = listTeleports.begin();
}


//******************************************************************************************
CTeleport::CTeleport
(
	const CGroffObjectName*	pgon,		// Object to load.
	CLoadWorld*				p_load,		// The loader.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
) : CEntity( pgon, p_load, h_object, pvtable, pinfo )
{
	listTeleports.push_front(this);
	itCurrent = listTeleports.begin();
}


//******************************************************************************************
CTeleport::~CTeleport()
{
	// Find ourselves in the teleport list.
	std::list<CTeleport*>::iterator it_me = std::find(listTeleports.begin(), 
											listTeleports.end(), this);

	Assert(it_me != listTeleports.end());

	// Erase this teleport from the list.
	listTeleports.erase(it_me);

	// Reset the current teleport.
	itCurrent = CTeleport::listTeleports.end();
}


//*****************************************************************************************
char* CTeleport::pcSave(char* pc_buffer) const
{
	// Save sequence number.
	int i_seq_num = 0;

	for (list<CTeleport*>::iterator it = listTeleports.begin(); it != listTeleports.end(); it++)
	{
		if (it == itCurrent)
			break;

		i_seq_num++;
	}

	SaveT(&pc_buffer, &i_seq_num);

	return pc_buffer;
}


//*****************************************************************************************
const char* CTeleport::pcLoad(const char*  pc_buffer)
{
	// Reset to first teleport.
	itCurrent = listTeleports.begin();

	if (CSaveFile::iCurrentVersion >= 13)
	{
		// Load sequence number.
		int i_seq_num;

		LoadT(&pc_buffer, &i_seq_num);

		/*
		// Locate the correct teleport.
		while (i_seq_num > 0 && itCurrent != listTeleports.end())
		{
			i_seq_num--;
			itCurrent++;
		}
		*/
	}

	return pc_buffer;
}


//******************************************************************************************
void PlayerTeleportToNextLocation()
{
	CCamera* pcam = CWDbQueryActiveCamera().tGet();
	CTeleport* ptel = CTeleport::NextTeleport();

	if (pcam && ptel)
	{
		// Dump all render caches.
		PurgeRenderCaches();

		// If camera is attached to an instance, teleport it directly.
		CInstance* pins_move = pcam->pinsAttached() ? pcam->pinsAttached() : pcam;

		// Move the mover to the object.
		pins_move->Move(ptel->p3Placement());

		// Use synchronous loads for the next 2 frames to ensure that all of the correct
		// textures are loaded.
		ps_renderDB->SetNoPageFrames(2);
	}
}


void PlayerTeleportToXYZ(float fX, float fY, float fZ)
{
	CCamera* pcam = CWDbQueryActiveCamera().tGet();

	if (pcam)
	{
		// Dump all render caches.
		PurgeRenderCaches();

		// If camera is attached to an instance, teleport it directly.
		CInstance* pins_move = pcam->pinsAttached() ? pcam->pinsAttached() : pcam;

		// Move the mover to the object.
        CVector3<>      location(fX, fY, fZ);
        CPlacement3<>   place = pins_move->pr3Presence();

        place.v3Pos = location;

		pins_move->Move(place);

		// Use synchronous loads for the next 2 frames to ensure that all of the correct
		// textures are loaded.
		ps_renderDB->SetNoPageFrames(2);
	}
}


