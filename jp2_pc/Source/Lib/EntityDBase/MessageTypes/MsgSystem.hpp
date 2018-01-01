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
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes/MsgSystem.hpp                            $
 * 
 * 8     9/25/98 11:53a Mlange
 * Added a message for a scene file loaded event.
 * 
 * 7     8/26/98 2:13p Mlange
 * Added message for GROFF loaded event.
 * 
 * 6     8/21/98 2:22p Mlange
 * Additional messaging stat printing.
 * 
 * 5     8/13/98 1:40p Mlange
 * The step and system messages now require registration of their recipients.
 * 
 * 4     6/08/98 5:46p Agrant
 * The quality slider variable!
 * 
 **********************************************************************************************/
 
#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGSYSTEM_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGSYSTEM_HPP

#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp"
#include "Lib/EntityDBase/QueueMessage.hpp"


//**********************************************************************************************
enum ESystemCode
// Prefix: esc
{	escSTART_SIM = 1,		// Start the simulation.
	escSTOP_SIM,			// Stop the simulation.
	escKILL_FOCUS,			// The game has lost focus.
	escSET_FOCUS,			// The game has gained focus.
	escQUALITY_CHANGE,		// The game quality setting has changed.
	escGROFF_LOADED,		// Finished loading a GROFF file.
	escSCENE_FILE_LOADED,	// Finished loading a scene file.
};


//**********************************************************************************************
//
class CMessageSystem : public CMessage, public CRegisteredMessage<CMessageSystem>
//
// Prefix: msgsys
//
// Message for system events.
// 
//**************************************
{
	friend class CRegisteredMessage<CMessageSystem>;
	static SRecipientsInfo riInfo;

public:
	ESystemCode escCode;
	
	CMessageSystem(const ESystemCode& esc)
		: CMessage(&riInfo.rcRecipients), escCode(esc)
	{}

	static bool bSimulationGoing()
	{
		return bSimGoing;
	}

	//******************************************************************************************
	//
	// Overrides.
	//


	//******************************************************************************************
	//
	virtual void Send() const;
	//
	// Send this message.
	//
	// Notes:
	//		This function will invoke the Deliver() function for all subsystems.
	//
	//**************************************

protected:
	static bool bSimGoing;

	void DeliverTo(CEntity* pet) const 
	{ 
		pet->Process(*this); 
	}

	const char* strName() const 
	{ 
		return "System"; 
	}
};

#endif
