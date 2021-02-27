/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
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
 * To do:
 *		Add more variables and constructors to the 'CMessageSound' class to accomodate
 *		additional types of sound processing (currently the message functions only for
 *		voice overs).
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes.hpp                                      $
 * 
 * 31    3/11/97 3:21p Blee
 * Revamped trigger system.
 * 
 * 30    2/03/97 10:35p Agrant
 * Query.hpp and MessageTypes.hpp have been split into
 * myriad parts so that they may have friends.
 * Or rather, so compile times go down.
 * Look for your favorite query in Lib/EntityDBase/Query/
 * Look for messages in                Lib/EntityDBase/MessageTypes/
 * 
 * 29    1/28/97 4:34p Blee
 * 
 * 28    1/21/97 1:35p Agrant
 * MessageDelete now deletes an instance instead of a Entity.
 * Uses Trash list in gameloop to actually perform the deletion.
 * 
 * 27    12/30/96 4:23p Mlange
 * Removed unused constructor from CMessageStep(). It was dangerous because it did not update
 * the timer.
 * 
 * 26    12/17/96 5:37p Pkeet
 * Added the 'CMessageDelete' class.
 * 
 * 25    12/14/96 4:52p Agrant
 * Added message types: death and pickup
 * 
 * 24    12/11/96 3:24p Agrant
 * Added sound materials as separate fields in a collision message
 * to allow terrain collsions sounds.
 * 
 * 23    12/09/96 7:52p Agrant
 * Added velocity and position for collisions.
 * 
 * 22    11/21/96 1:53p Blee
 * Take out SoundMessage.
 * 
 * 21    11/21/96 1:03p Blee
 * Change constructor for audio message.
 * 
 * 20    11/20/96 5:59p Blee
 * Add Collision Daemon.
 * Add Audio Daemon.
 * Change Audio to not use STRICT.
 * Added comments to Collision Daemon.
 * 
 * 19    11/20/96 1:23p Agrant
 * Added a total simulation time stamp to the step message.
 * 
 * 18    11/19/96 6:58p Blee
 * Add Control.
 * 
 * 17    96/11/18 20:57 Speter
 * Added strData() function for CMessageStep.
 * 
 * 16    96/11/18 13:49 Speter
 * Added temporary CMessageInput type.
 * 
 * 15    11/18/96 1:51p Pkeet
 * Modified the CMessageSound class to do voice overs.
 * 
 * 14    11/16/96 7:27p Pkeet
 * Added the 'CMessageMoveTriggerTo' class.
 * 
 * 13    11/15/96 6:38p Pkeet
 * Added the 'CMessageTrigger' classes. Moved the 'Send' member function for CMessageStep to the
 * implementation file.
 * 
 * 12    11/14/96 11:13p Agrant
 * Added collision message type
 * 
 * 11    11/14/96 7:31p Pkeet
 * Added class 'CMessageHere.'
 * 
 * 10    96/11/11 17:01 Speter
 * Added sStep field to CMessageStep.  Added code to calculate time step for each message.
 * 
 * 9     11/02/96 7:18p Agrant
 * moved CMessagePhysicsReq to a different file
 * 
 * 8     11/01/96 3:44p Agrant
 * Added CMessagePhysicsReq class.
 * 
 * 7     96/10/24 6:04p Mlange
 * Removed 'const' from all CEntity*.
 * 
 * 6     96/10/24 4:30p Mlange
 * Added default constructors to all classes.
 * 
 * 5     96/10/23 6:44p Mlange
 * Shortened the strMessageName() and strMessageData functions to strName() and strData().
 * Defined the 'step' message type. Added some comments.
 * 
 * 4     96/10/22 11:09 Speter
 * Changed CBase to CEntity, and prefixes correspondingly.
 * 
 * 3     10/21/96 7:30p Pkeet
 * Added 'strMessageName' and 'strMessageData' functions. Added a parameter to the 'Deliver'
 * function.
 * 
 * 2     96/10/21 6:04p Mlange
 * Messages now have a member function for obtaining the sender. Added CMessagePersonal.
 * 
 * 1     96/10/21 4:03p Mlange
 * Initial version.
 * 
 **********************************************************************************************/


#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_HPP

#error MessageTypes.hpp is now obsolete! Use MessageTypes/foo.hpp

#if 0

#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/EntityDBase/Message.hpp"

#include "Lib/EntityDBase/QueueMessage.hpp"

#include "Game/AI/Rating.hpp"
#include "Lib/Transform/Vector.hpp"
#include "Lib/Transform/Rotate.hpp"
#include "Lib/Sys/Timer.hpp"

//#include "Lib/Audio/Material.hpp"

// Just to keep all messages defined by including MessageTypes.hpp
// Could be replaced with "class CMessagePhysicsReq" to reduce dependencies.
#include "MessagePhysicsReq.hpp"

//**********************************************************************************************
//
class CMessageStep : public CMessage
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
public:
	TSec	sStep;					// The time since the last step.
	TSec	sTotal;					// The total elapsed simulation time since game start.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessageStep()
	{
		static CTimer tmrStep;				// The simulation timer.
		static const TSec sSTEP_MAX = 0.2;	// Maximum time step.
		static TSec s_total			= 0;	// Total elapsed simulation time.

		// The first step time will probably be very large, since it's the time elapsed since
		// creation.  However, we clamp the step value, so we'll be all right.

		sStep	= Min(tmrStep.sElapsed(), sSTEP_MAX);
		s_total += sStep;
		sTotal	= s_total;
	}

protected:

	//******************************************************************************************
	//
	// Overides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const
	{
		return "Step";
	}

	virtual const char* strData() const;

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

	friend class CQueueMessage;
};


//**********************************************************************************************

enum ESystemCode
// Prefix: sc
{	sc_start_sim = 1,		// Start the simulation.
	sc_stop_sim,			// Stop the simulation.
	sc_kill_focus,			// The game has lost focus.
	sc_set_focus,			// The game has gained focus.
};

class CMessageSystem : public CMessage
// Prefix: ms
// Message for system messages.
//**************************************
{
public:
	ESystemCode code;

	CMessageSystem(const ESystemCode& sc)
		: code(sc)
	{}
protected:
	void DeliverTo(CEntity* pet) const { pet->Process(*this); }
	const char* strName() const { return "System"; }
};

//**********************************************************************************************

#include "lib/control/Control.hpp"

class CMessageControl : public CMessage
// Prefix: mc
// Message for user control.
//**************************************
{
public:
	TInput input;

	CMessageControl(const TInput& in)
		: input(in)
	{}
protected:
	void DeliverTo(CEntity* pet) const { pet->Process(*this); }
	const char* strName() const { return "Control"; }
};

//**********************************************************************************************
//
class CMessageInput : public CMessage
//
// Passes along a user input.
//
// Prefix: msginp
//
// Notes:
//		This is a temporary message type, that lets entities directly handle Windows input.
//		This will go away when the real input handling system is created.
//
//**************************************
{
public:
	int		iKey;						// Which key was pressed (VK_...).
	float	fMouseX, fMouseY;			// The mouse position during the key press 
										// (virtual [-1,1] coords).

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessageInput(int i_key, float f_mouse_x = 0, float f_mouse_y = 0)
		: iKey(i_key), fMouseX(f_mouse_x), fMouseY(f_mouse_y)
	{
	}

protected:

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const
	{
		return "Input";
	}
};

//**********************************************************************************************
//
class CMessageTrigger : public CMessage
//
// The 'trigger' message. This message type is sent by an activated trigger.
//
// Prefix: msgtrig
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
	CMessageTrigger(CEntity* pet_sender)
		: CMessage(pet_sender)
	{
	}

protected:

	//******************************************************************************************
	//
	// Overides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const
	{
		return "Trigger";
	}

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

	friend class CQueueMessage;
};


//**********************************************************************************************
//
class CMessageMoveTriggerTo : public CMessage
//
// This message will move a trigger matching the name specified in the constructor.
//
// Prefix: msgmtrigto
//
// Notes:
//
//**************************************
{
public:

	CPlacement3<> p3NewPosition;
	uint32        u4Key;

	//******************************************************************************************
	//
	// Constructor.
	//

	//******************************************************************************************
	CMessageMoveTriggerTo(const char* str_name, CPlacement3<> p3);

protected:

	//******************************************************************************************
	//
	// Overides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const
	{
		return "Move Trigger To";
	}

	friend class CQueueMessage;
};

/*
//**********************************************************************************************
//
class CMessageCollision : public CMessage
//
// Two objects have collided.
//
// Prefix: msgcoll
//
// Notes:
//
//**************************************
{
public:
	CInstance*	pins1;		// One colliding object.
	CInstance*	pins2;		// The other colliding object.
	TMaterial	tmatSound1;	// The sound material of object 1.
	TMaterial	tmatSound2;	// The sound material of object 2.
	TReal		rVelocity;	// The velocity with which the objects collided.
	TReal		rForce;		// The velocity with which the objects collided.
	CVector3<>	v3Position;	// The location at which the objects collided, in world space.


	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CMessageCollision			// Sets sound materials through the instances.
	(
		CInstance *	pins_one, 
		CInstance*	pins_two, 
		TReal		r_force,
		TReal		r_velocity, 
		const CVector3<>& v3,
		CEntity*	pet_sender = 0, 
		CEntity*	pet_recipient = 0
	);

	CMessageCollision			// Sets sound materials explicitly.
	(
		CInstance *	pins_one, 
		CInstance*	pins_two, 
		TReal		r_force,
		TReal		r_velocity, 
		const CVector3<>& v3,
		TMaterial	tmat1,
		TMaterial   tmat2,
		CEntity*	pet_sender = 0, 
		CEntity*	pet_recipient = 0
	)
		:	CMessage(pet_sender, pet_recipient), 
			pins1(pins_one),
			pins2(pins_two),
			rVelocity(r_velocity),
			v3Position(v3),
			tmatSound1(tmat1),
			tmatSound2(tmat2)
	{
	}
protected:

	//******************************************************************************************
	//
	// Overides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const
	{
		return "Collision";
	}

};
*/

//**********************************************************************************************
//
class CMessageHere : public CMessage
//
// The 'here' message. This message type for informing triggers and subsystems that "I am here."
//
// Prefix: msghere
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

	CMessageHere(CEntity* pet_sender)
		: CMessage(pet_sender)
	{
	}

protected:

	//******************************************************************************************
	//
	// Overides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const
	{
		return "I am here";
	}

};

//**********************************************************************************************
//
class CMessagePickUp : public CMessage
//
// The 'pick up' message. This message type is for informing the game about objects the player
//	has picked up.
//
// Prefix: msgpu
//
// Notes:
//
//**************************************
{
public:

	CInstance* pinsObject;		// The object that has been picked up, or 0 if the message indicates a drop.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessagePickUp(CInstance* pins_object)
		: CMessage(), pinsObject(pins_object)
	{

	}

protected:

	//******************************************************************************************
	//
	// Overides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const
	{
		return "Pick Up";
	}

};

//**********************************************************************************************
//
class CMessageDeath : public CMessage
//
// The 'death' message. This message type informs the game when something dies.
//
// Prefix: msgdeth
//
// Notes:
//		The dying entity ought not be deleted until the next frame if possible!  That way any
//		data structures referencing the dying entity can fix themselves up before their pointers
//		become invalid.  
//
//**************************************
{
public:

	CAnimate*	paniDyingThing;		// The thing that is about to die.
	CInstance*	pinsCorpse;			// The corpse it will become, or zero if none.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessageDeath(CAnimate* pani_deader, CInstance* pins_corpse = 0, CEntity* pet_sender = 0)
		: CMessage(pet_sender), paniDyingThing(pani_deader), pinsCorpse(pins_corpse)
	{
	}

protected:

	//******************************************************************************************
	//
	// Overides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const
	{
		return "I am dying!";
	}

};


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

#endif