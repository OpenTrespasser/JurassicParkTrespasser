/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/WaterDisturbance.hpp                                      $
 * 
 * 8     9/20/98 7:32p Agrant
 * cast waterdisturbance
 * 
 * 7     9/18/98 3:10p Agrant
 * save/load water disturbances properly
 * 
 * 6     8/13/98 1:41p Mlange
 * Added destructor.
 * 
 * 5     7/20/98 10:14p Rwyatt
 * Removed description text in final mode
 * 
 * 4     12/15/97 3:03p Agrant
 * Water Disturbance descriptions
 * 
 * 3     10/16/97 2:03p Agrant
 * Value table based constructor now allows instancing of CInfo
 * 
 * 2     8/21/97 1:52p Agrant
 * another bones session- tail added
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_WATER_DISTURBANCE_HPP 
#define HEADER_LIB_PHYSICS_WATER_DISTURBANCE_HPP 

#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/EntityDBase/Water.hpp"
#include "Lib/Sys/Timer.hpp"

//**********************************************************************************************
//
class CWaterDisturbance : public CEntity
//
// Prefix: wd
//
//	A class for stimulating a water object.
//
//	Notes:
//		These objects are instances placed in MAX that agitate a water object located 
//		in the same XY position.  Z is ignored.
//
//**************************************
{
public:

	CEntityWater*		pewWater;	// The water affected by this disturbance.  Zero if uninitialized.
	TSec				sInterval;	// Length of time between disturbances.
	TSec				sNextDisturbance;	// Timestamp of next disturbance.
	
	// For now, use the standard round disturbances.
	TReal		rSize;		// How large is this disturbance?
	TReal		rStrength;	// How strong is this disturbance.


	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// The basic constructor for loading from a GROFF file.
	CWaterDisturbance
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CWaterDisturbance();

	//******************************************************************************************
	//
	// Overrides.
	//

	// Step message might disturb some water!
	virtual void Process(const CMessageStep& msgstep);

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

	//*****************************************************************************************
	virtual void Cast(CWaterDisturbance** pwd)
	{
		*pwd = this;
	}


#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};



#endif  //  #ifndef HEADER_LIB_PHYSICS_WATER_DISTURBANCE_HPP 
