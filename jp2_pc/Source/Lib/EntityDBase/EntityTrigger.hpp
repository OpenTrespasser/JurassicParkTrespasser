/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CEntityTrigger definition.
 *
 *		Also definitions of various activation objects:
 *
 *			CActivate
 *				CActivatePlayer
 *					CActivatePlayerView
 *
 * Notes:
 *
 *		Triggers are designed to send out a message that the trigger has been activated on
 *		conditions like the player entering within a certain proximity of the trigger, or
 *		the trigger first becoming visible to the player. Other trigger activation conditions
 *		can be added by extending the "CActivate" class.
 *
 *		Triggers are identified by a unique integer key, 'u4Key.' This key is generated from
 *		a hash value based on the character string representing the name of the trigger. The
 *		hash value is preferable to the string as it requires less memory; the string name is
 *		currently only kept around for debugging purposes (see the 'To Do' list). Provided the
 *		same hashing function is used ('u4Hash' in 'StdLibEx.hpp'), the design daemon or other
 *		modules can identify a trigger through its key value. The hash function is case
 *		insensitive when used by strings.
 *
 *		Triggers have a bounding volume specified by the code that instantiates a trigger.
 *		Any bounding volume in the 'GeomTypes' library can be used. Currently one of the
 *		SInit constructors builds a spherical bounding volume from a proximity, representing
 *		the sphere's radius.
 *
 *		Triggers also have an activation object described by class 'CActivate' and its
 *		descendants. These objects allow triggers to work with any kind of 'subject' (e.g.
 *		the player) and any condition without requiring modifications to the basic trigger
 *		code.
 *
 *		Various SInit constructors have been created to ease the creation of triggers.
 *		Additional constructors can be added as a greater variety of triggers become possible.
 *
 *		The following is an example of how a trigger can be added to the world:
 *
 *			// Instantiate the trigger.
 *			wWorld.Add(new CEntityTrigger(CEntityTrigger::SInit("Bomb 2", 10.0, true)));
 *
 *		The first parameter of the 'CEntityTrigger::SInit' constructor is a string representing
 *		the name of the trigger, which is converted into an integer 'key' for identifying the
 *		trigger. The second parameter of the constructor represents the distance to the
 *		trigger the player must be to activate the trigger. The final parameter (true)
 *		constructs the trigger with the activation object 'CActivatePlayerView,' which
 *		indicates that the trigger must be within the player's viewing range.
 *
 *		Note that player activation is implicit within this constructor, but additional
 *		constructors can easily add other ways of activating the trigger in conjunction with
 *		extension to the 'CActivate' class. Also note that no placement information is given
 *		in the example constructor, and the 'CMessageMoveTriggerTo' message should be used
 *		after the trigger is instantiated.
 *
 * Bugs:
 *
 * To do:
 *		Support additional trigger features as required. Remove the string identifier.
 *		Make the current CEntityTrigger into CTriggerLocation, and create a CTrigger as an
 *		abstract base class for all triggers.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/EntityDBase/EntityTrigger.hpp                                    $
 * 
 * 9     97/03/10 5:17p Pkeet
 * Added an overloaded partition type function.
 * 
 * 8     97/03/04 12:43p Pkeet
 * Implemented the use of the 'pbvGetBoundingVolume' member function.
 * 
 * 7     12/14/96 3:29p Pkeet
 * Added the 'CTriggerTime' class.
 * 
 * 6     11/23/96 4:35p Pkeet
 * Made the destructor for CEntityTrigger virtual.
 * 
 * 5     11/17/96 3:12p Pkeet
 * Added notes and comments.
 * 
 * 4     11/16/96 7:29p Pkeet
 * Added the 'CEntityTrigger*' parameter to the bActivate member function. Added a process
 * function for 'CMessageMoveTriggerTo.' Change an init constructor to work without a
 * placement.
 * 
 * 3     11/16/96 4:16p Mlange
 * Added identifier function.
 * 
 * 2     11/15/96 6:36p Pkeet
 * Added the 'CActivatePlayerView' class. Added the 'b_also_view' flag in the constructor init
 * structure. Added the 'pactSubject' and 'petSubject' member variables. Added the
 * 'petGetSubject' member function and a destructor.
 * 
 * 1     11/14/96 7:27p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_ENTITYTRIGGER_HPP
#define HEADER_LIB_ENTITYDBASE_ENTITYTRIGGER_HPP

#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/Transform/Rotate.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/Sys/Timer.hpp"


//
// Forward class declarations.
//
class CEntityTrigger;


//
// Class definitions.
//

//*****************************************************************************************
//
// Classes to determine subject's line of sight.
//

//*****************************************************************************************
//
class CActivate
//
// A base class for all types of trigger activation conditions.
//
// Prefix: act
//
// Notes:
//		This class will always return true when the 'Is Here' message is sent to the
//		trigger. It is primarily intended as an abstract class that models how descendant
//		activation classes should work.
//
//**************************************
{
public:

	//*************************************************************************************
	//
	virtual bool bActivate
	(
		CEntityTrigger* pettrig,	// Pointer to the trigger.
		const CEntity*  pet_subject	// Pointer to the subject attempting to cause activation.
	) const
	//
	// Returns 'true' always. See the notes associated with the class.
	//
	//**************************************
	{
		return true;
	}

};

//*****************************************************************************************
//
class CActivatePlayer : public CActivate
//
// A class to determine if the player is in the proximity of the trigger.
//
// Prefix: actp
//
//**************************************
{
public:

	//*************************************************************************************
	//
	virtual bool bActivate
	(
		CEntityTrigger* pettrig,	// Pointer to the trigger.
		const CEntity*  pet_subject	// Pointer to the subject attempting to cause activation.
	) const;
	//
	// Returns true if the player is within the bounding volume of the trigger.
	//
	//**************************************

};

//*****************************************************************************************
//
class CActivatePlayerView : public CActivatePlayer
//
// A class to determine if the player  in the proximity of the trigger and can see the
// trigger.
//
// Prefix: actpv
//
//**************************************
{
public:

	//*************************************************************************************
	//
	virtual bool bActivate
	(
		CEntityTrigger* pettrig,	// Pointer to the trigger.
		const CEntity* pet_subject	// Pointer to the subject attempting to cause activation.
	) const;
	//
	// Returns true if the player is within the bounding volume of the trigger and the
	// trigger is visible to the player.
	//
	//**************************************

};


//**********************************************************************************************
//
class CEntityTrigger : public CEntity
//
// An entity that describes a trigger.
//
// Prefix: ettrig
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Classes for initializing the trigger.
	//

	//*****************************************************************************************
	//
	struct SInit
	//
	// A base class for all types of seeing classes.
	//
	// Prefix: initrig
	//
	//**************************************
	{
		char const*   strTriggerName;	// A pointer to a string representing the name of the
										// trigger.
		CPlacement3<> p3Placement;		// The position and orientation of the trigger in the
										// world.
		CBoundVol*    pbvTriggerVolume;	// The object representing the trigger's bounding
										// volume.
		CActivate*    pactSubject;		// The activation object.

		//*************************************************************************************
		//
		// Constructors.
		//

		// Default constructor.
		SInit()
			: strTriggerName(0), pbvTriggerVolume(0), pactSubject(0)
		{
		}

		// Constructor just using the name.
		SInit
		(
			const char const* str_trigger_name	// Unique name for the trigger.
		)
			: strTriggerName(str_trigger_name),  pbvTriggerVolume(0), pactSubject(0)
		{
		}

		// Constructor with full description.
		SInit
		(
			const char const*    str_trigger_name,	// Unique name for the trigger.
			const CPlacement3<>& p3,				// Position of the trigger in the world.
			const CBoundVol*     pbv,				// Trigger bounding volume.
			const CActivate*     pact				// Class describing conditions for seeing.
		)
			: strTriggerName(str_trigger_name),  p3Placement(p3),
			  pbvTriggerVolume((CBoundVol*)pbv), pactSubject((CActivate*)pact)
		{
		}

		// Constructor for a player seeing the object for the first time. Note that the
		// position for the trigger must still be set after the trigger is created.
		SInit
		(
			const char const* str_trigger_name,		// Unique name for the trigger.
			float             f_radius,				// Radius from subject to trigger.
			bool              b_also_view = false	// If false, the trigger is activated
													// by proximity only, if true the trigger
													// must be in the player's view cone.
		)
			: strTriggerName(str_trigger_name)
		{
			// Create a sphere around the trigger.
			pbvTriggerVolume = new CBoundVolSphere(CVector3<>(), f_radius);

			// Look for a player.
			pactSubject = (b_also_view) ? (new CActivatePlayerView()) : (new CActivatePlayer());
		}

	};

protected:

	char*      strTriggerName;		// Name of this trigger.
	uint32     u4Key;				// Unique value identifying this trigger.
	CBoundVol* pbvBoundingVolume;	// Bounding volume for triggering this trigger.
	CActivate* pactSubject;			// Subject information for determining activation.
	CEntity*   petSubject;			// Pointer to subject causing the trigger.

public:

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	// Constructor with initialization information.
	CEntityTrigger(SInit initrig);

	// Destructor.
	virtual ~CEntityTrigger();


	//*****************************************************************************************
	//
	// Message processing member functions.
	//

	//*****************************************************************************************
	//
	virtual void Process
	(
		const CMessageHere& msghere
	);
	//
	// Responds to an "Is Here" message.
	//
	// Notes:
	//		This function will test the object sending the "Is Here" message to see if it meets
	//		the necessary conditions for causing the trigger's activation. If the trigger is
	//		activated, this function will send out a "CMessageTrigger" message.
	//
	//**************************************

	//******************************************************************************************
	//
	virtual void Process
	(
		const CMessageMoveTriggerTo& msgmtrigto
	);
	//
	// Responds to an "Move Trigger To" message by moving the trigger to the location specified
	// in the message.
	//
	//**************************************


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	const char* strGetTriggerName
	(
	) const
	//
	// Returns the name of the trigger.
	//
	//**************************************
	{
		Assert(strTriggerName);

		return strTriggerName;
	}

	//*****************************************************************************************
	//
	uint32 u4GetKey
	(
	) const
	//
	// Returns a unique key value for the trigger.
	//
	// Notes:
	//		The key for the trigger is a hash value based on its name.
	//
	//**************************************
	{
		return u4Key;
	}

	//*****************************************************************************************
	//
	virtual const CBoundVol* pbvBoundingVol
	(
	) const
	//
	// Returns the bounding volume for the object.
	//
	//**************************
	{
		return pbvBoundingVolume;
	}

	//*****************************************************************************************
	//
	virtual const CEntity* petGetSubject
	(
	) const
	//
	// Returns a pointer to the subject causing this trigger.
	//
	//**************************************
	{
		return petSubject;
	}


	//*****************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual void Cast(CEntityTrigger** ppettrig)
	{
		*ppettrig = this;
	}

	//*****************************************************************************************
	//
	virtual const char* strPartType
	(
	) const
	//
	// Returns a partition type string.
	//
	//**************************
	{
		return "Trigger";
	}

};


//**********************************************************************************************
//
class CTriggerTime : public CEntityTrigger
//
// A trigger that sends out a message after a given period of time.
//
// Prefix: trtime
//
//**************************************
{
protected:

	TSec   sAlarm;		// Time to set the trigger off.
	bool   bFirstStep;	// Flag to indicate the first step message has been received.

public:

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	// Standard constructor.
	CTriggerTime
	(
		const char const* str_trigger_name,	// Unique name for the trigger.
		double            d_duration		// Wait period for activating the trigger in
											// seconds.
	)
		: sAlarm(TSec(d_duration)), bFirstStep(false), CEntityTrigger(SInit(str_trigger_name))
	{
	}


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual void Process
	(
		const CMessageStep& msgstep
	);
	//
	// Responds to an "Step" message by either setting, checking or responding to this
	// trigger's internal alarm.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void Process
	(
		const CMessageHere& msghere
	)
	//
	// Responds to an "Is Here" message. Do nothing!
	//
	// Notes:
	//		Remove override when an abstract trigger base class is implemented.
	//
	//**************************************
	{
	}

	//******************************************************************************************
	//
	virtual void Process
	(
		const CMessageMoveTriggerTo& msgmtrigto
	)
	//
	// Responds to an "Move Trigger To" message. Do nothing!
	//
	// Notes:
	//		Remove override when an abstract trigger base class is implemented.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	virtual const char* strPartType
	(
	) const
	//
	// Returns a partition type string.
	//
	//**************************
	{
		return "TimeTrigger";
	}

};


#endif
