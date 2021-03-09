/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		The actual message types (derived from the base message classes defined in
 *		'Message.hpp') used for communication.
 *
 *
 * Bugs:
 *
 * Notes:
 *		Queued messages are never deleted.  Their memory is just reclaimed.
 *		Thus, messages should not require fancy delete functions, nor should they include
 *		rptr's.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/EntityDBase/MessageTypes/MsgMagnet.hpp                            $
 * 
 * 1     11/13/97 11:28p Rwyatt
 * Initial Implementation of an empty message for magnets.
 * This will be filled with data when we know what it is.
 * 
 **********************************************************************************************/
 
#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGMAGNET_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGMAGNET_HPP

#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/QueueMessage.hpp"


//
// These messages are placeholders and are currently empty
//


//**********************************************************************************************
//
class CMessageMagnetBreak : public CMessage
//
// This message is sent when a magnet breaks
//
// Prefix: msgmagb
//
// Notes:
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	CMessageMagnetBreak()
	{
	}


protected:

	//******************************************************************************************
	//
	// Overides.
	//
	virtual void Queue() const override;


	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const override
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const override
	{
		return "Magnet Break";
	}


	friend class CQueueMessage;
};







//**********************************************************************************************
//
class CMessageMagnetMove : public CMessage
//
// This message is sent when a magnet moves
//
// Prefix: msgmagm
//
// Notes:
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	CMessageMagnetMove()
	{
	}


protected:

	//******************************************************************************************
	//
	// Overides.
	//
	virtual void Queue() const override;


	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const override
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const override
	{
		return "Magnet Move";
	}


	friend class CQueueMessage;
};



#endif
