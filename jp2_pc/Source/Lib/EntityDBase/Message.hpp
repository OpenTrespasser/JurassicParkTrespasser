/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CMessage
 *
 *		Defines the base class for the message hierarchy. The actual message types used for
 *		communication must be derived from this class.
 *
 *		See also the files in MessageTypes and 'Entity.hpp'.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Message.hpp                                           $
 * 
 * 15    8/21/98 2:22p Mlange
 * Additional messaging stat printing.
 * 
 * 14    8/13/98 7:16p Mlange
 * Made a member protected.
 * 
 * 13    8/11/98 8:31p Mlange
 * Implemented message-entity registration scheme.
 * 
 * 12    12/02/97 7:52a Shernd
 * Stopped writing of unwanted files
 * 
 * 11    4/09/97 11:41a Rwyatt
 * Added ExtractReplayData member function
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_MESSAGE_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGE_HPP

#include "Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp"

extern bool bUseOutputFiles;

class CEntity;
class CConsoleBuffer;

//**********************************************************************************************
//
class CMessage
//
// Abstract base class for message type class hierarchy.
//
// Prefix: msg
//
// Notes:
//		Communication through the use of this message class (and all message types derived from
//		it) is an 'active' process. That is, delivering this message to a recipient actually
//		diverts the flow of control to a member function of that recipient (unless the message
//		is queued). The flow of control will not return to the sender until the recipient has
//		finished processing the message. In other words, a recipient does not 'poll' a queue for
//		any pending messages, but processes the message as and when it is delivered.
//
//**************************************
{
	CEntity* petSender;		// The sender of this message, null if the sender is unidentifiable.
	CEntity* petRecipient;	// The intended recipient of this message, null if intended for all.

protected:
	TRecipientsContainer* prcRecipients;

public:
	static bool bIgnoreRegisteredRecipients;

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor. Defines message from unidentifiable sender to all recipients.
	CMessage()
		: petSender(0), petRecipient(0), prcRecipients(0)
	{
	}

	// Set the sender and intended recipient of the message to the ones specified.
	CMessage(CEntity* pet_sender, CEntity* pet_recipient = 0)
		: petSender(pet_sender), petRecipient(pet_recipient), prcRecipients(0)
	{
		// Don't Assert the sender, because you may want to send a message to a particular entity
		// from some non-entity object.
	}

	CMessage(TRecipientsContainer* prc_recipients, CEntity* pet_sender = 0)
		: petSender(pet_sender), petRecipient(0), prcRecipients(prc_recipients)
	{
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual CEntity* petGetSender() const
	//
	// Obtain the sender of this message.
	//
	// Returns:
	//		The sender of this message, or null if the sender is unidentifiable.
	//
	//**************************************
	{
		return petSender;
	}


	//******************************************************************************************
	//
	virtual CEntity* petGetRecipient() const
	//
	// Obtain the recipient for this message.
	//
	// Returns:
	//		The recipient for this message, or null if it is intended for all recipients.
	//
	//**************************************
	{
		return petRecipient;
	}


	//******************************************************************************************
	//
	virtual void Dispatch() const
	//
	// First record this message in the log and then queue it.
	//
	//**************************************
	{
		#if VER_LOG_MESSAGES
            if (bUseOutputFiles)
            {
			    WriteToLog();
            }
		#endif

		Queue();
	}


	//******************************************************************************************
	//
	static void PrintStats
	(
		CConsoleBuffer& con
	);
	//
	// Print messaging info to the given console.
	//
	//**************************************


protected:

	//******************************************************************************************
	//
	virtual void Queue() const
	//
	// Queue this message.
	//
	// Notes:
	//		The default behaviour of this function is not to queue, but to immediately send the
	//		message. This function can be overidden by the derived classes to implement actual
	//		queueing of the message if required.
	//
	//**************************************
	{
		Send();
	}

	//******************************************************************************************
	//
	virtual void ExtractReplayData() const
	//
	// Extract replay data for this message.
	//
	// Notes:
	//		The default behaviour of this function is to do nothing, if this message is required
	//		in the replay file then the message must overload this function which should save
	//		important information relating to this message to enable it to be reproduced.
	//		This function should be called from the Send function of the respective message.
	//
	//**************************************
	{}


	//******************************************************************************************
	//
	virtual void Send() const;
	//
	// Send this message.
	//
	// Notes:
	//		This function will invoke the Deliver() function either for just the single
	//		recipient or for all recipients.
	//
	//**************************************


	//******************************************************************************************
	//
	virtual void DeliverTo
	(
		CEntity* pet	// The recipient for this message.
	) const = 0;
	//
	// A pure virtual function, private to this class, that performs the actions necessary to
	// 'deliver' this message to the given recipient.
	//
	// Notes:
	//		Each message is required to know how to deliver itself. The derived classes must
	//		implement this function depending on the requirements for that message. Generally,
	//		this means simply calling the entity's Process() function.
	//
	//**************************************


#if VER_LOG_MESSAGES

	//******************************************************************************************
	//
	virtual const char* strName
	(
	) const = 0;
	//
	// A pure virtual function that returns a string representing the name of the message.
	//
	// Notes:
	//		Each message must overide this member function to return a name about the class
	//		relevant to debugging.
	//
	//**************************************

	//******************************************************************************************
	//
	virtual const char* strData
	(
	) const
	//
	// A virtual function that returns a string representing the data specific to the
	// message.
	//
	// Notes:
	//		Each message may overide this member function to return text information about the
	//		class relevant to debugging, such as the contents of data member variables.
	//
	//**************************************
	{
		return 0;
	}

	//******************************************************************************************
	//
	virtual void WriteToLog
	(
	) const;
	//
	// Write this message to the message log.
	//
	//**************************************

//#if VER_LOG_MESSAGES
#endif


	friend class CQueueMessage;
};



#endif
