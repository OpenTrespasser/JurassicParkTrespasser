#pragma once

/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents: Support for a list of locations that we may teleport the player to.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Teleport.hpp                                         $
 * 
 * 1     98.05.08 5:54p Mmouni
 * Initial version, still needs some cleanup.
 * 
 *********************************************************************************************/


class CTeleport : public CEntity
{
private:
	static std::list<CTeleport*> listTeleports;
	static std::list<CTeleport*>::iterator itCurrent;

	CPresence3<> pr3_location;

public:
	//*****************************************************************************************
	static CTeleport* NextTeleport();

	//******************************************************************************************
	// Construct at a given location.
	CTeleport(const CPresence3<>& pr3);

	//******************************************************************************************
	// Constructor for loading from a GROFF.
	CTeleport
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	//******************************************************************************************
	~CTeleport();

	//******************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual char* pcSave(char* pc_buffer) const;

	//*****************************************************************************************
	virtual const char* pcLoad(const char* pc_buffer);
};


//******************************************************************************************
//
// Teleport the player to the next teleport position.
//
void PlayerTeleportToNextLocation();
