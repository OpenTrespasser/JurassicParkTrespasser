/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The physics command event class and helper classes.
 *
 *
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes/MsgPhysicsReq.hpp                        $
 * 
 * 20    9/05/98 11:16a Agrant
 * Added the go limp flag.  Removed unused tail commands.
 * 
 * 19    8/21/98 2:22p Mlange
 * Additional messaging stat printing.
 * 
 * 18    98/08/21 1:15 Speter
 * Duh...initialise the new field.
 * 
 * 17    98/08/19 23:07 Speter
 * Added subSwing.
 * 
 * 16    8/13/98 6:09p Mlange
 * Now requires registration of recipients.
 * 
 * 15    8/11/98 8:28p Mlange
 * Added #include.
 * 
 * 14    98/04/07 14:57 Speter
 * Removed unnecessary EHandFrame.
 * 
 * 13    98/02/24 21:21 Speter
 * Added flag for various frames for hand orientation.
 * 
 * 12    12/18/97 7:31p Agrant
 * Tail wag amplitude changed to radians rather than 0 to 1
 * 
 * 11    12/17/97 7:25p Agrant
 * Better physics requests from the AI point of view
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_PHYSICSREQ_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_PHYSICSREQ_HPP


#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp"
#include "Lib/Physics/Arms.h"
#include "Game/AI/Rating.hpp"


//**********************************************************************************************
//
template <class DATA>
class CSubRequest
//
// Contains information about particular sub-portions of a physics request.
//
// Prefix: sub
//
// Notes:
//		The data field type varies depending with the type of request.
//
//**************************************
{
public:
	CRating				rtUrgency;		// The urgency of the subrequest.
										// If zero, the subrequest ought not be acknowledged
										// the physics system.

	CRating				rtSpeed;		// The speed at which the request ought to be carried
										// out.  1 is fastest, 0 is slowest.

	DATA				dData;			// An additional data field for the subrequest.


	//******************************************************************************************
	//
	// Member functions.
	//

		//*********************************************************************************
		//
		void Reset
		(
		)
		//
		//	Set data fields to innocuous values.
		//
		//******************************
		{
			rtUrgency = 0;
		}

		//*********************************************************************************
		//
		void Set
		(
			const CRating	rt_urgency,
			const CRating	rt_speed,
			const DATA	&	d_data
		)
		//
		//	Sets data fields.
		//
		//******************************
		{
			rtUrgency	= rt_urgency;
			rtSpeed		= rt_speed;
			dData		= d_data;
		}


};

//**********************************************************************************************
//
class CMessagePhysicsReq : public CMessage, public CRegisteredMessage<CMessagePhysicsReq>
//
// Request movement from physics.
//
// Prefix: msgpr
//
// Notes:
//
//
//**************************************
{
	friend class CRegisteredMessage<CMessagePhysicsReq>;
	static SRecipientsInfo riInfo;

public:
	CSubRequest<CVector2<> >	subMoveBody;			// A subrequest for moving the body.
														//  vector is target pelvis location.

	CSubRequest<CVector3<> >	subMoveHead;			// Move the mouth to new location.
														//  vector is the destination 
														//	of the tip of the nose in world space.
	
	CSubRequest<CVector3<> >	subPointHead;			// A subrequest for changing the facing of the head.
														// Data is vector forward from the head.

	CSubRequest<TReal>			subCockHead;			// A subrequest for changing the facing of the head.
														// Data is angle about PointHead vector to rotate the head.
														// Zero is line through eyes parallel to ground.
														// Angle in radians.

	CSubRequest<CRating>		subOpenMouth;			// Open/Close the mouth.
														// Data-  0 closed, 1 open, in between is in between.

	CSubRequest<CRotate3<> >	subOrientBody;			// A subrequest for changing the facing of the body.
														//  desired world space rotation of the pelvis.

	CSubRequest<bool>			subCrouch;				// True if requesting a crouch.
	CSubRequest<bool>			subJump;				// True if requesting a jump.
	CSubRequest<bool>			subGoLimp;				// True if requesting a tranquilized dino.

	//  User/player specific requests.
	CSubRequest<CRotate3<> >	subUserOrientHead;			// A subrequest for changing the facing of the head.
														// The data field is the object-space rotation of the head.

	CSubRequest<CVector3<> >	subUserMoveHand;		// A subrequest to move the hand.
														// The data field is the object-space position of the hand.
														// This is different to a AI move hand request.
	CSubRequest< CRotate3<> >	subUserOrientHand;		// Rotate the hand.
														// If MoveHand has urgency and OrientHand does not,
														// physics is free to orient hand however it wants.
	CSubRequest<bool>			subPickup;				// Pick up an object with the hand.
	CSubRequest<bool>			subSwing;				// Move arm in "swing" mode.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CMessagePhysicsReq()
		: CMessage(&riInfo.rcRecipients)
	{
	}

	CMessagePhysicsReq(CEntity* pet_sender, CEntity* pet_recipient)
		: CMessage(pet_sender, pet_recipient)
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//


		//*********************************************************************************
		//
		void Reset
		(
		)
		//
		//	Set data fields to innocuous values.
		//
		//******************************
		{
			subMoveBody.Reset();
			subOrientBody.Reset();
			subMoveHead.Reset();
			subPointHead.Reset();
			subCockHead.Reset();
			subOpenMouth.Reset();
//			subTailWagAmplitude.Reset();
//			subTailWagFrequency.Reset();
			
			subUserMoveHand.Reset();
			subUserOrientHand.Reset();
			subUserOrientHead.Reset();
			subPickup.Reset();
			subCrouch.Reset();
			subJump.Reset();
			subGoLimp.Reset();
			subSwing.Reset();
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
		return "Physics Request";
	}

};

//#ifndef HEADER_LIB_ENTITYDBASE_PHYSICSREQ_HPP
#endif
