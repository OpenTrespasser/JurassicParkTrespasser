/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/BloodSplat.hpp                                      $
 * 
 * 6     10/02/98 11:09p Mlange
 * Blood splats are now always placed above all other terrain texture objects.
 * 
 * 5     10/01/98 12:24a Asouth
 * added default constructor (as it were) for MW STL
 * 
 * 4     9/28/98 10:21p Mlange
 * Positions and sizes of blood splats are now saved in the scene file.
 * 
 * 3     9/25/98 9:04p Mlange
 * Reworked blood splat system. It now incorporates the slope of the terrain in the placement
 * calculation to correctly handle bloodsplats on steep slopes.
 * 
 * 2     8/26/98 2:11p Mlange
 * Operational.
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_DESIGNDAEMON_BLOODSPLAT_HPP
#define HEADER_GAME_DESIGNDAEMON_BLOODSPLAT_HPP

#include "Lib/EntityDBase/Subsystem.hpp"
#include <list>

class CTerrainObj;

//**********************************************************************************************
//
class CBloodSplats : public CSubsystem
//
// Manager class for terrain texture object bloodsplats.
//
// Prefix: bs
//
//**************************************
{
	struct SBloodSplatObj
	// Prefix: bso
	{
		CTerrainObj* ptrobjShape;
		TReal rDimension;

		SBloodSplatObj(CTerrainObj* ptrobj = 0)
			: ptrobjShape(ptrobj), rDimension(1)
		{
		}
	};

	class CPriv;
	friend class CPriv;

	typedef list<SBloodSplatObj> TContainer;

	TContainer contSplats;
	TContainer contStreaks;

	TReal rDefaultSplatDimension;
	TReal rDefaultStreakDimension;

	int iCurrentTrrObjHeight;

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
public:
	CBloodSplats();

	~CBloodSplats();

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void CreateSplat
	(
		TReal r_world_x,
		TReal r_world_y,
		TReal r_dimension
	);
	//
	// Request a new blood splat on the terrain of the given dimension and at the given location.
	//
	//**************************************


	//******************************************************************************************
	//
	void CreateStreak
	(
		TReal r_world_x,
		TReal r_world_y,
		TReal r_dimension,
		const CDir2<>& d2_direction
	);
	//
	// Request a new blood splat on the terrain of the given dimension and at the given location.
	//
	//**************************************


	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void Process(const CMessageSystem& ms);

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);
};

// Single global instance.
extern CBloodSplats* pBloodSplats;

#endif

