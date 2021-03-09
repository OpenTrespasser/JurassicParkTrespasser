/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CMessageMove
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
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes/MsgMove.hpp                              $
 * 
 * 10    98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 9     98/09/01 13:09 Speter
 * Added handy shortcut constructor.
 * 
 * 8     8/22/98 5:39p Mlange
 * Override send function.
 * 
 * 7     8/21/98 2:22p Mlange
 * Additional messaging stat printing.
 * 
 * 6     8/11/98 8:28p Mlange
 * Is now a message type requiring registration.
 * 
 * 5     98/05/15 16:12 Speter
 * Added bSignificant() function.
 * 
 * 4     98/04/23 10:37 Speter
 * Added etACTIVE enum.
 * 
 **********************************************************************************************/
 

#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGMOVE_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGMOVE_HPP

#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp"
#include "Lib/Sys/Timer.hpp"

//**********************************************************************************************
//
class CMessageMove : public CMessage, public CRegisteredMessage<CMessageMove>
//
// An object has moved, awakened, or slept.
//
// Prefix: msgmv
//
//**************************************
{
	friend class CRegisteredMessage<CMessageMove>;
	static SRecipientsInfo riInfo;

public:
	enum EType
	// Prefix: et
	{
		etAWOKE,				// Movable object has just awakened (not moved yet).
		etACTIVE,				// Object still awake, but has not moved.
		etMOVED,				// Object still awake, has moved.
		etSLEPT					// Movable object just went sleepy-bye (didn't move).
	}
	etType;

	CInstance*		pinsMover;	// The object in question.
	CPlacement3<>	p3Prev;		// The old placement on the instance, in world space.
								// If etType != etMOVED, this will be the same as the object's
								// current placement.
	TSec			sWhen;		// When the activity occurred.
	
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessageMove
	(
		EType					et,			// What kind of activity.
		CInstance*				pins,		// The object that moved.
		const CPlacement3<>&	p3_prev,	// Its old location.
		CEntity*				pet_sender = 0,
		TSec					s_when = 0	// When the move occurred.
											// If 0, the current CMessageStep total is used.
	) : 
		CMessage(&riInfo.rcRecipients, pet_sender),
		etType(et), pinsMover(pins), p3Prev(p3_prev), 
		sWhen(s_when ? s_when : CMessageStep::sStaticTotal)
	{
	}

	CMessageMove
	(
		EType					et,			// What kind of activity.
		CInstance*				pins,		// The object that moved.
		CEntity*				pet_sender = 0
	) : 
		CMessage(&riInfo.rcRecipients, pet_sender),
		etType(et), pinsMover(pins), p3Prev(pins->p3GetPlacement()), 
		sWhen(CMessageStep::sStaticTotal)
	{
	}

	static const TReal rTHRESHOLD_DEFAULT;

	//******************************************************************************************
	//
	bool bSignificant
	(
		TReal r_threshold = rTHRESHOLD_DEFAULT		// Measure of movement, roughly in world scale.
	) const;
	//
	// Returns:
	//		Whether this movement is significant, to the given threshold.
	//
	//**********************************

protected:

	//******************************************************************************************
	//
	// Overides.
	//

	//******************************************************************************************
	virtual void Send() const override;

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const override
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const override
	{
		return "Move";
	}
};



#endif
