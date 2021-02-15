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
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes/MsgTrigger.hpp                           $
 * 
 * 12    8/21/98 2:22p Mlange
 * Additional messaging stat printing.
 * 
 * 11    98/08/13 17:02 Speter
 * Added CMessagePickUp action types.
 * 
 * 10    8/12/98 7:00p Mlange
 * CMessageDeath and CMessageDamage now require subscription of the recipients.
 * 
 * 9     8/12/98 6:02p Mlange
 * Commented-out CMessageHere - it wasn't used. CMessagePickUp and CMessageUse now require
 * registration of the listeners.
 * 
 * 8     8/12/98 3:39p Mlange
 * The CMessageTrigger and CMessageMoveTriggerTo message types now use the new registration
 * scheme. Removed some commented out code.
 * 
 * 7     7/24/98 12:31a Rwyatt
 * Removed unused messages
 * 
 * 6     6/03/98 4:23p Rwyatt
 * CMessageDamage revamped
 * 
 **********************************************************************************************/
 
#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGTRIGGER_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGTRIGGER_HPP

#include "Lib/Trigger/Trigger.hpp"
#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp"
#include "Lib/EntityDBase/QueueMessage.hpp"


//**********************************************************************************************
//
class CMessageTrigger : public CMessage, public CRegisteredMessage<CMessageTrigger>
//
// The 'trigger' message. This message type is sent by an activated trigger.
//
// Prefix: msgtrig
//
// Notes:
//
//**************************************
{
	friend class CRegisteredMessage<CMessageTrigger>;
	static SRecipientsInfo riInfo;

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	CMessageTrigger(CTrigger* ptr_sender)
		: CMessage(&riInfo.rcRecipients, ptr_sender)
	{
	}

	CTrigger* ptrGetActivatedTrigger() const
	{
		return (CTrigger*)petGetSender();
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
		return "Trigger";
	}

	friend class CQueueMessage;
	friend class CTrigger;
};



//**********************************************************************************************
//
class CMessageMoveTriggerTo : public CMessage, public CRegisteredMessage<CMessageMoveTriggerTo>
//
// This message will move a trigger matching the ID in the constructor.
//
// Prefix: msgmtrigto
//
// Notes:
//
// This message may be broadcast by the sender and it will be picked up by the trigger, but as
// you know the ID of the trigger you may as well get the entity for this trigger and send it
// directly to it, this will save the overhead of all entities processing the message.
// In either case the same functionality is acheived.
//
// The constructor may be changed so you have to pass a destination entity to prevent the
// above happening.
//
//**************************************
{
	friend class CRegisteredMessage<CMessageMoveTriggerTo>;
	static SRecipientsInfo riInfo;

public:

	CPlacement3<>	p3NewPosition;
	CTrigger*		ptrTrigger;

	//******************************************************************************************
	//
	// Constructor.
	//

	//******************************************************************************************
	CMessageMoveTriggerTo(CTrigger* ptr_trig, CPlacement3<> p3)
				: ptrTrigger(ptr_trig), p3NewPosition(p3), CMessage(&riInfo.rcRecipients)
	{
	}



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
		return "Move Trigger To";
	}

	friend class CQueueMessage;
};



/*
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
*/



//**********************************************************************************************
enum EPickup
// Prefix: epu
{
	epuPICKUP,
	epuDROP,
	epuSTOW,
	epuRETRIEVE
};

//**********************************************************************************************
//
class CMessagePickUp : public CMessage, public CRegisteredMessage<CMessagePickUp>
//
// The 'pick up' message. This message type is for informing the game about objects the player
//	has picked up or put down.
//
// Prefix: msgpu
//
// Notes:
//
//**************************************
{
	friend class CRegisteredMessage<CMessagePickUp>;
	static SRecipientsInfo riInfo;

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessagePickUp(CInstance* pins_object, EPickup epu = epuPICKUP)
		: CMessage(&riInfo.rcRecipients), pinsObject(pins_object), epuAction_(epu)
	{
	}

	CInstance* pinsGetMessageObject() const
	{
		return pinsObject;
	}

	EPickup epuAction() const
	{
		return epuAction_;
	}

protected:

	CInstance*	pinsObject;		// The object that has been picked up or put down
	
	EPickup		epuAction_;		// Type of pickup action.

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
		return "Pick Up";
	}
};




//**********************************************************************************************
//
class CMessageUse : public CMessage, public CRegisteredMessage<CMessageUse>
//
// The 'use' message. This message type is for informing the game about carryable objects that
// the player has used.
//
//
// Prefix: msguse
//
// Notes:
//
//**************************************
{
	friend class CRegisteredMessage<CMessageUse>;
	static SRecipientsInfo riInfo;

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessageUse(CInstance* pins_object)
		: CMessage(&riInfo.rcRecipients), pinsObject(pins_object)
	{
	}

	CInstance* pinsGetMessageObject() const
	{
		return pinsObject;
	}

protected:

	CInstance*	pinsObject;		// The object that has been picked up or put down
	

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
		return "Use";
	}
};




//**********************************************************************************************
//
class CMessageDeath : public CMessage, public CRegisteredMessage<CMessageDeath>
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
	friend class CRegisteredMessage<CMessageDeath>;
	static SRecipientsInfo riInfo;

public:

	CAnimate*	paniDyingThing;		// The thing that is about to die.
	CInstance*	pinsCorpse;			// The corpse it will become, or zero if none.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessageDeath(CAnimate* pani_deader, CInstance* pins_corpse = 0, CEntity* pet_sender = 0)
		: CMessage(&riInfo.rcRecipients, pet_sender), paniDyingThing(pani_deader), pinsCorpse(pins_corpse)
	{
	}

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
		return "I am dying!";
	}

};




//**********************************************************************************************
//
class CMessageDamage : public CMessage, public CRegisteredMessage<CMessageDamage>
//
// The 'damage' message. This message type informs the game when something gets injured
//
// Prefix: msgdam
//
// Notes:
//
//**************************************
{
	friend class CRegisteredMessage<CMessageDamage>;
	static SRecipientsInfo riInfo;

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessageDamage
	(
		CEntity*	pet_sender,
		CInstance*	pins_damaged,
		float		f_dam,
		bool b_critical = false
	)
		: CMessage(&riInfo.rcRecipients, pet_sender), fDamage(f_dam), bCriticalHit(b_critical), pinsDamaged(pins_damaged)
	{
	}

	bool bGetDamageType() const
	{
		return bCriticalHit;
	}

	CInstance* pinsGetDamaged() const
	{
		return pinsDamaged;
	}

	float fGetDamage() const
	{
		return fDamage;
	}

protected:
	bool		bCriticalHit;				// was the hit in a critical place
	float		fDamage;					// amount of damage sustained
	CInstance*	pinsDamaged;


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
		return "Damage";
	}

};



#endif
