/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CMessageCollision
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
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes/MsgCollision.hpp                         $
 * 
 * 23    9/16/98 12:29a Agrant
 * added new energy calculations
 * 
 * 22    8/26/98 2:12p Mlange
 * Collision message now contains ray cast object intersecion list.
 * 
 * 21    98/08/21 18:00 Speter
 * Collisions now store total hit energy in frame as well as max instantaneous energy. Renamed
 * all fields to reflect actual usage. Added functions to return linear and log normalised
 * values for all fields.
 * 
 * 20    8/21/98 2:22p Mlange
 * Additional messaging stat printing.
 * 
 * 19    98/08/19 23:09 Speter
 * Added necessary include.
 * 
 * 18    98/08/19 2:31 Speter
 * Override Send().
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGCOLLISION_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGCOLLISION_HPP

#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp"
#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/Audio/SoundDefs.hpp"

class CRayCast;

//**********************************************************************************************
//
class CMessageCollision : public CMessage, public CRegisteredMessage<CMessageCollision>
//
// Two objects have collided.
//
// Prefix: msgcoll
//
// Notes:
//		If one colliding object is 0, the object has collided with the terrain.
//		Collisions can have a Hit and/or Slide velocity in the same message.
//		Hit velocity will be set every time the objects actually hit one another.
//		Slide velocity will be sent every time they slide against one another.
//		They may both be sent at once.
//
//		No 'release' message is sent.
//
//**************************************
{
	friend class CRegisteredMessage<CMessageCollision>;
	static SRecipientsInfo riInfo;

public:
	CInstance*		pins1;			// One colliding object.
	CInstance*		pins2;			// The other colliding object.
									// This is the only object that can be terrain.
									// If this object is 0, it's a bullet.
	TSoundMaterial	smatSound1;		// The sound material of object 1.
	TSoundMaterial	smatSound2;		// The sound material of object 2.
	float			fEnergyMax;		// Maximum hit energy encountered this frame; 
									// should be used for sounds.
	float			fEnergyHit;		// Total hit energy with which the objects collided this frame;
									// should be used for damage.
	float			fEnergySlide;	// Total slide energy between objects this frame.
	CVector3<>		v3Position;		// The location at which the objects collided, in world space.
	CRayCast*		prcGunHits;		// The object list of all the gun ray intersections, or null if this collision
									// message was not sent from the gun code.
	float			fEnergyDelta1;	// Kinetic energy change of object 1 in the frame containing the collision.
	float			fEnergyDelta2;	// Kinetic energy change of object 2 in the frame containing the collision.

	//******************************************************************************************
	//
	// Constructors.
	//

	//******************************************************************************************
	CMessageCollision			// Sets sound materials through the instances.
	(
		CInstance*	pins_one, 
		CInstance*	pins_two,			// If 0, assumes it's a bullet.
		TSoundMaterial smat_one,		// Specify the 2 material types; if 0, they are inferred.
		TSoundMaterial smat_two,
		float		f_energy_max,
		float		f_energy_hit,
		float		f_energy_slide,
		const CVector3<>& v3,
		float		f_energy_delta_1,
		float		f_energy_delta_2,
		CEntity*	pet_sender = 0,
		CRayCast*   prc_gun_hits = 0
	);

	//******************************************************************************************
	CMessageCollision			// Sets sound materials through the instances.
	(
		CInstance*	pins_one, 
		CInstance*	pins_two,			// If 0, assumes it's a bullet.
		float		f_energy_max,
		float		f_energy_hit,
		float		f_energy_slide,
		const CVector3<>& v3,
		float		f_energy_delta_1,
		float		f_energy_delta_2,
		CEntity*	pet_sender = 0,
		CRayCast*   prc_gun_hits = 0
	);

	//*********************************************************************************************
	//
	float fEnergyMaxNorm() const
	//
	// Returns:
	//		The max energy, normalised linearly so that 1 is the nominal (but not actual) max.
	//
	//**********************************
	{
		return fEnergyMax * fENERGY_MAX_NORM;
	}

	//*********************************************************************************************
	//
	float fEnergyHitNorm() const
	//
	//**********************************
	{
		return fEnergyHit * fENERGY_HIT_NORM;
	}

	//*********************************************************************************************
	//
	float fEnergySlideNorm() const
	//
	//**********************************
	{
		return fEnergySlide * fENERGY_SLIDE_NORM;
	}

	//*********************************************************************************************
	//
	float fEnergyMaxNormLog() const;
	//
	// Returns:
	//		The max energy, clamped and normalised logarithmically to lie strictly between 0..1 inclusive.
	//
	//**********************************

	//*********************************************************************************************
	//
	float fEnergyHitNormLog() const;
	//
	//**********************************

	//*********************************************************************************************
	//
	float fEnergySlideNormLog() const;
	//
	//**********************************


protected:

	// The scaling factors to normalise the energies.
	static const float fENERGY_MAX_NORM;
	static const float fENERGY_HIT_NORM;
	static const float fENERGY_SLIDE_NORM;

	//******************************************************************************************
	//
	// Overrides.
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
		return "Collision";
	}

};



#endif
