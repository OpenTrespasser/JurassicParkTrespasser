/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CSubsystem
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Subsystem.hpp                                        $
 * 
 * 9     8/13/98 1:44p Mlange
 * Removed dummy message step process function. Added assert.
 * 
 * 8     98.05.14 7:00p Mmouni
 * Added save/restore of default settings.
 * 
 * 7     3/22/98 5:03p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 6     11/11/97 2:57p Agrant
 * Defualt subsystem name is now "Subsystem"
 * 
 * 5     11/10/97 6:04p Agrant
 * Default subsystem save/load does nothing.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_SUBSYTEM_HPP
#define HEADER_LIB_ENTITYDBASE_SUBSYTEM_HPP

#include "Lib/EntityDBase/Entity.hpp"


//*********************************************************************************************
//
class CSubsystem: public CEntity
//
// Prefix: sub
//
// Base class for all subsystems.  
//
// Notes:
//
//**************************************
{

public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CSubsystem()
	{
		SetInstanceName("Subsystem");
		//strName = "Subsystem";
	}

	CSubsystem
	(
		const CInstance::SInit& initins
	)
		: CEntity(initins)
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const override
	{ return pc_buffer; }

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer) override
	{ return pc_buffer; }

	//*****************************************************************************************
	//
	virtual void SaveDefaults()
	{
		return;
	}
	//
	// Save the default values of the user modifiable settings.
	//
	//**********************************

	//*****************************************************************************************
	//
	virtual void RestoreDefaults()
	{
		return;
	}
	//
	// Restore the default values of the user modifiable settings.
	//
	//**********************************

	//******************************************************************************************
	virtual void Move(const CPlacement3<>&)
	{
		// Subsystems cannot move.
		Assert(false);
	}

	//******************************************************************************************
	virtual void Cast(CSubsystem** ppsub) override
	{
		*ppsub = this;
	}
};



#endif
