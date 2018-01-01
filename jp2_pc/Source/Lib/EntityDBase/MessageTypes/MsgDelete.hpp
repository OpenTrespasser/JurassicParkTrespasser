/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		The actual message types (derived from the base message classes defined in
 *		'Message.hpp') used for communication.
 *
 *		New message types will be added to this header file if and when they are required.
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
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes/MsgDelete.hpp                            $
 * 
 * 1     2/03/97 10:37p Agrant
 * split up MessageTypes.hpp
 * 
 * 1     2/03/97 10:36p Agrant
 * Message types files
 *
 **********************************************************************************************/
 
#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGDELETE_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGDELETE_HPP

#include "Lib/EntityDBase/Message.hpp"

#include "Lib/EntityDBase/QueueMessage.hpp"


//**********************************************************************************************
//
class CMessageDelete : public CMessage
//
// This message notifies entities that they must remove all references to the instance passed 
// in the constructor.  
//
// Prefix: msgmdelsend
//
// Notes:
//
//**************************************
{
public:
	CInstance* pinsDeleteMe;	// Instance to be deleted.


	//******************************************************************************************
	//
	// Constructor.
	//

	//******************************************************************************************
	CMessageDelete(CInstance* pins);
	CMessageDelete(){ }

protected:

	//******************************************************************************************
	//
	// Overides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const;

	//******************************************************************************************
	virtual const char* strName() const
	{
		return "Delete";
	}

	//******************************************************************************************
	//
	virtual void Queue
	(
	) const;
	//
	// Put this message on the queue.
	//
	//**************************************

	//******************************************************************************************
	//
	virtual void Send
	(
	) const;
	//
	// Send this message.
	//
	// Notes:
	//		This function will actually call delete on its sender.
	//
	//**************************************

	friend class CQueueMessage;
};


#endif
