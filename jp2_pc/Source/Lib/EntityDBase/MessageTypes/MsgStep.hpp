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
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes/MsgStep.hpp                              $
 * 
 * 13    9/25/98 12:47p Agrant
 * simulation speed multiplier, mostly for SLOMO
 * 
 * 12    8/21/98 2:22p Mlange
 * Additional messaging stat printing.
 * 
 * 11    8/13/98 1:40p Mlange
 * The step and system messages now require registration of their recipients.
 * 
 * 10    8/11/98 8:28p Mlange
 * Added #include.
 * 
 * 9     2/06/98 5:37p Mlange
 * Added frame count to step message.
 * 
 * 8     12/04/97 4:21p Agrant
 * Added a static "last frame in game time" variable- 
 * sStaticStep
 * 
 * 7     97/09/11 20:53 Speter
 * CMessageStep() now optionally takes a step time.  Moved constructor code to .cpp file.
 * 
 **********************************************************************************************/
 
#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGSTEP_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGSTEP_HPP

#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp"
#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/Sys/Timer.hpp"

//**********************************************************************************************
//
class CMessageStep : public CMessage, public CRegisteredMessage<CMessageStep>
//
// The 'step' message. This message type is for executing one step of the entity's loop.
//
// Prefix: msgstep
//
// Notes:
//		This message type replaces the traditional 'main loop' or 'message pump' construct. It
//		is used by the entity types that need to perform one step of their general processing
//		loop each frame.
//
//		The order in which this message is delivered to the entities determines the order in
//		which their step functions are executed. Currently, this order is determined by the
//		iteration of the entities in the entity database. This behaviour may be modified by
//		overiding the Dispatch() function for this class.
//
//**************************************
{
	friend class CRegisteredMessage<CMessageStep>;
	static SRecipientsInfo riInfo;

public:
	TSec		sStep;					// The time since the last step.
	TSec		sTotal;					// The total elapsed simulation time since game start 
										//  as of this message.
	static TSec	sElapsedRealTime;		// elapsed time to this message, REAL UNCLAMPED TIME

	static TSec	sStaticTotal;			// The total elapsed simulation time since game start
										//  as of the most recent message.
	static TSec sStaticStep;			// Step time of the most recent step.

	static TSec sMultiplier;			// Game speed in seconds per second.  Lower is slower.  1.0 is real time.
	
	static uint32 u4Frame;				// Frame number of this message (step frames, not paint frames).

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessageStep
	(
		TSec s_step = -1.0				// Explicit step value.
										// If default, uses actual elapsed time.
	);

protected:

	//******************************************************************************************
	//
	// Overides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const override
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const override
	{
		return "Step";
	}

	virtual const char* strData() const override;

	//******************************************************************************************
	//
	virtual void Send() const override;
	//
	// Send this message.
	//
	// Notes:
	//		This function will invoke the Deliver() function either for just the single
	//		recipient or for all recipients.
	//


	//******************************************************************************************
	//
	virtual void ExtractReplayData() const override;
	//
	// Extract replay data specific to this message
	//
	//**************************************


	friend class CQueueMessage;

	//
	// we need to be a friend with the replay class because it can fire off messages while
	// playing back a replay.
	//
	// all mesages that are recorded should in a replay be friends with CReplay
	//
	friend class CReplay;
};


#endif
