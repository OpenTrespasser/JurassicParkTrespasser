/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CEntity
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Entity.hpp                                            $
 * 
 * 51    98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 50    8/26/98 10:24p Agrant
 * Text prop constructor for CEntityAttached
 * 
 * 49    8/12/98 6:01p Mlange
 * Commented-out CMessageHere - it wasn't used.
 * 
 * 48    8/11/98 8:26p Mlange
 * Entities must now register themselves with each message type in which they are interested.
 * 
 * 47    98/06/21 15:51 Speter
 * Added CEntityAttached argument to specify whether to attach to head.
 * 
 * 46    6/07/98 4:00p Agrant
 * EntityAttached update function now public
 * 
 * 45    2/21/98 5:53p Rwyatt
 * Removed all traces of the audio response message. It is no longer required.
 * 
 * 44    11/13/97 11:35p Rwyatt
 * Default Process functions for the new magnet messages
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_ENTITY_HPP
#define HEADER_LIB_ENTITYDBASE_ENTITY_HPP

#include "Lib/EntityDBase/Instance.hpp"



//**********************************************************************************************
//
// Forward declaration of each message type for class CEntity. See files in MessageTypes.
//
class CMessageAudio;
class CMessageCollision;
class CMessageControl;
class CMessageDeath;
class CMessageWake;
class CMessageCreate;
class CMessageDelete;
//class CMessageHere;
class CMessageInput;
class CMessageMove;
class CMessageMoveReq;
class CMessageMoveTriggerTo;
class CMessagePaint;
class CMessagePickUp;
class CMessageUse;
class CMessagePhysicsReq;
class CMessageSound;
class CMessageStep;
class CMessageSystem;
class CMessageTrigger;
class CMessageNewRaster;
class CMessageDamage;
class CMessageButton;
class CMessageMagnetMove;
class CMessageMagnetBreak;

//**********************************************************************************************
//
class CEntity: public CInstance
//
// Prefix: et
//
// Base class for all objects in the world that process messages.  
//
// Notes:
//		This can include physical objects, non-physical objects, and subsystems. Inactive
//		objects in the world, for example lights, should not be CEntities, but simple CInstances.
//
//		All entities are also a CInstance, even those that don't really need this, such as
//		subsystems. The idea is that such objects will be few in number, and must sacrifice
//		their individual desires for elegance for the greater good.
//
//		Note on message processing:
//
//		When a child class overrides a parent class function with a name such as "Process," 
//		all other functions in the parent class with the same name ("Process") but different 
//		arguments are hidden from that child class.  The child class can no longer call the 
//		hidden functions directly.  
//
//		However, by casting the child class pointer to a parent class pointer, the hidden
//		functions become visible once again and are correctly accessed via the magic of virtual
//		function tables.
//
//**************************************
{

public:
	//******************************************************************************************
	//
	// Constructors.
	//

	CEntity()
	{
	}

	CEntity
	(
		const CInstance::SInit& initins
	)
		: CInstance(initins)
	{
	}

	// The basic CEntity constructor for loading from a GROFF file.
	CEntity
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CInstance(pgon, pload, h_object, pvtable,pinfo)
	{}


	//******************************************************************************************
	//
	// Member functions.
	//


	//
	// Message Processing functions:
	//
	// For each defined message type, we declare a virtual member function that processes that
	// type of message. For this base class, these functions simply do nothing; the derived
	// classes will overide the member functions for the message types they're interested in.
	//
	// See the notes about overriding these functions in the class header.
	//

	//lint -save -e715  // unused arguments!
	virtual void Process(const CMessageStep& msgstep) {}
	virtual void Process(const CMessageControl& mc) {}
	virtual void Process(const CMessageSystem& ms) {}
	virtual void Process(const CMessageInput& msginp) {}
	virtual void Process(const CMessageAudio& ma) {}
	virtual void Process(const CMessagePhysicsReq& msgpr) {}
	virtual void Process(const CMessageMoveReq& msgpr) {}
//	virtual void Process(const CMessageHere& msghere) {}
	virtual void Process(const CMessageCollision& msgcoll) {}
	virtual void Process(const CMessageTrigger& msgtrig) {}
	virtual void Process(const CMessageMoveTriggerTo& msgmtrigto) {}
	virtual void Process(const CMessagePaint& msgpaint) {}
	virtual void Process(const CMessagePickUp& msgpu) {}
	virtual void Process(const CMessageUse& msguse) {}
	virtual void Process(const CMessageDeath& msgtrig) {}
	virtual void Process(const CMessageWake& msgwake) {}
	virtual void Process(const CMessageCreate& msgcr) {}
	virtual void Process(const CMessageDelete& msgtrig) {}
	virtual void Process(const CMessageMove& msgtrig) {}
	virtual void Process(const CMessageNewRaster& msgnewr) {}
	virtual void Process(const CMessageDamage& msgdam) {}
	virtual void Process(const CMessageButton& msgbut) {}
	virtual void Process(const CMessageMagnetMove& msgmagm) {}
	virtual void Process(const CMessageMagnetBreak& msgmagb) {}
	//lint -restore

	//******************************************************************************************
	//
	// Overides.
	//

	//lint -save -e1411
	virtual void Cast(CEntity** ppet)
	{
		*ppet = this;
	}
	//lint -restore

	//*****************************************************************************************
	virtual const char* strPartType() const;
};


//*********************************************************************************************
//
class CEntityAttached: public CEntity
//
// Prefix: eta
//
// An entity possibly attached to another instance.
// The entity always follows the attached instance, and control is bi-directional.
// If the attached instance moves, this entity detects it and follows.
// If this entity is moved explicitly, it tells the attached instance to move.
//
//**************************************
{
protected:
	CInstance*			pinsAttached_;	// The attached instance, if any.
	CAnimate*			paniAttached_;	// The same instance, if a CAnimate, and attaching to head.
	CPlacement3<>		p3Relative_;	// The placement relating this entity to the attached instance.

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	CEntityAttached();

	CEntityAttached
	(
		const SInit& initins,
		      CInstance* pins_attached = 0,
			  bool b_head = false,				// Whether to attach to the head of the object.
		const CPlacement3<>& p3_relative = CPlacement3<>()
	);

	// The basic CEntity constructor for loading from a GROFF file.
	CEntityAttached
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CEntityAttached();

	//******************************************************************************************
	//
	// Member functions.
	//

	CInstance* pinsAttached() const
	{
		return pinsAttached_;
	}

	const CPlacement3<>& p3Relative() const
	{
		return p3Relative_;
	}

	bool bHead() const
	{
		return paniAttached_ != 0;
	}

	//******************************************************************************************
	void SetAttached
	(
		CInstance* pins_attached,
		bool b_head = false				// Whether to attach to the head of the object.
	);
	//
	// Set the attached instance.
	//

	//******************************************************************************************
	void SetRelative
	(
		const CPlacement3<>& p3_relative
	);
	//
	// Set the relative placement to the attached instance.
	//

	//******************************************************************************************
	//
	// Overrides.
	//

	void Move(const CPlacement3<>& p3_new, CEntity* pet_sender = 0);

	// Respond to move messages by moving myself if my parent moves.
	void Process(const CMessageMove& msgmove);

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);


	//*****************************************************************************************
	virtual const char* strPartType() const;

//protected:
	// How can we update attached guys when we aren't in run mode if this is protected?
	//******************************************************************************************
	void UpdateFromAttached();
	//
	// Update from attached instance's position.
	//
};

#endif
