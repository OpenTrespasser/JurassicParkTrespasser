/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CMessageCreate
 *
 * Bugs:
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/EntityDBase/MessageTypes/MsgCreate.hpp                            $
 * 
 * 1     97-03-27 12:12 Speter
 * 
 * 1     2/09/97 8:11p Agrant
 * initial rev
 * 
 **********************************************************************************************/
 

#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGCREATE_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGCREATE_HPP

#include "Lib/EntityDBase/Message.hpp"
#include "Lib/Sys/Timer.hpp"

//**********************************************************************************************
//
class CMessageCreate : public CMessage
//
// An object has just been created.
//
// Prefix: msgcr
//
// Notes:
//
//**************************************
{
public:
	CInstance*		pinsCreated;		// The object that was created.
	TSec			sWhen;				// When the creation occurred.
	
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CMessageCreate
	(
		CInstance*				pins,	// The new object.
		TSec					s_when	// When the creation occurred.
	) : 
		pinsCreated(pins), sWhen(s_when)
	{}

protected:

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const override
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const override
	{
		return "Create";
	}
};

#endif
