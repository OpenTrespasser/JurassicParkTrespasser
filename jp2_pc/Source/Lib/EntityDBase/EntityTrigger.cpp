/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: The implementation of EntityTrigger.hpp.
 *
 * Bugs:
 *
 * To do:
 *		Replace the code in CActivatePlayerView that checks if an object is in the player's
 *		camera's view volume with the bounding volume of a polyhedron.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/EntityTrigger.cpp                                    $
 * 
 * 16    97/09/29 4:42p Pkeet
 * Implemented 'fGetScale()' instead of '.'
 * 
 * 15    97/09/28 3:11p Pkeet
 * Replace the use of 'CPresence3<>().v3Pos' with 'v3Pos().'
 * 
 * 14    97/03/04 12:43p Pkeet
 * Implemented the use of the 'pbvGetBoundingVolume' member function.
 * 
 * 13    2/03/97 10:35p Agrant
 * Query.hpp and MessageTypes.hpp have been split into
 * myriad parts so that they may have friends.
 * Or rather, so compile times go down.
 * Look for your favorite query in Lib/EntityDBase/Query/
 * Look for messages in                Lib/EntityDBase/MessageTypes/
 * 
 * 12    97/01/07 11:25 Speter
 * Put all CRenderTypes in rptr<>.
 * 
 * 11    12/14/96 3:29p Pkeet
 * Added the 'CTriggerTime' class.
 * 
 * 10    96/12/11 16:59 Speter
 * Updated for changed CEntity constructors.
 * 
 * 9     12/09/96 1:40p Mlange
 * Commented-out bIsVisible(). It needs to be re-written to use bounding volume intersection
 * tests directly instead of using camera member functions.
 * 
 * 8     96/12/04 20:33 Speter
 * A big change: Updated for new CPresence3<>, and CInstance functions.
 * 
 * 7     11/23/96 5:36p Mlange
 * Updated for changes to the camera query.
 * 
 * 6     11/21/96 11:55a Mlange
 * Updated for changes to CPresence.
 * 
 * 5     11/17/96 3:15p Pkeet
 * Added the 'bIsVisible' function. Added numerous comments. Modified code requiring strings to
 * use the 'StringEx.hpp' library.
 * 
 * 4     11/16/96 7:30p Pkeet
 * Added the 'CEntityTrigger*' parameter to the bActivate member function. Added a process
 * function for 'CMessageMoveTriggerTo.' Change an init constructor to work without a
 * placement. Added a 'Bounding Sphere' test to the bActivate member function.
 * 
 * 3     11/16/96 4:12p Mlange
 * Updated for CInstance's 'dynamic_cast' method.
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


//
// Includes.
//

#include "GblInc/Common.hpp"
#include "EntityTrigger.hpp"

#include "Lib/Std/StringEx.hpp"
#include "Lib/Std/Hash.hpp"
#include "Entity.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "WorldDBase.hpp"
#include "MessageLog.hpp"
#include "MessageTypes/MsgTrigger.hpp"
#include "MessageTypes/MsgStep.hpp"
#include "Game/DesignDaemon/Player.hpp"


//
// Forward declarations of module-specific functions.
//

//*********************************************************************************************
//
bool bIsVisible
(
	const CCamera&       cam,				// The camera.
	const CTransform3<>& tf3_shape_to_world	// The transform from the shape's local position
											// to the world position.
);
//
// Returns true if the shape is visible in the camera, otherwise returns false.
//
// Notes:
//		This function is temporary, and should be removed when polyhedral bounding volumes
//		are supported.
//
//**************************************


//
// Class implementations.
//

//*********************************************************************************************
//
// CEntityTrigger implementation.
//

	//*****************************************************************************************
	//
	// CEntityTrigger constructor and destructor.
	//

	//*****************************************************************************************
	CEntityTrigger::CEntityTrigger(SInit initrig)
		: petSubject(0), CEntity(CInstance::SInit(initrig.p3Placement))
	{
		// Copy the trigger name.
		strTriggerName = strDuplicate(initrig.strTriggerName);
		
		// Set the hash value.
		u4Key = u4Hash(initrig.strTriggerName);

		// Copy the subject activation object's pointer.
		pactSubject = initrig.pactSubject;

		// Copy the bounding volume's pointer.
		pbvBoundingVolume = initrig.pbvTriggerVolume;
	}

	//*****************************************************************************************
	CEntityTrigger::~CEntityTrigger()
	{
		delete strTriggerName;
		delete pactSubject;
		delete pbvBoundingVolume;
	}


	//*****************************************************************************************
	//
	// CEntityTrigger message processing member function implementations.
	//

	//*****************************************************************************************
	void CEntityTrigger::Process(const CMessageHere& msghere)
	{
		// Check to see if the trigger meets activation conditions.
		if (pactSubject->bActivate(this, msghere.petGetSender()))
		{
			// Send out a trigger message.
			CMessageTrigger msgtrig(this);

			// Make sure this trigger know's who tripped it.
			petSubject = msghere.petGetSender();

			// Okay, now actually send it out.
			msgtrig.Dispatch();
		}
	}
	
	//*****************************************************************************************
	void CEntityTrigger::Process(const CMessageMoveTriggerTo& msgmtrigto)
	{
		// Update placement.
		pr3Presence() = CPresence3<>(msgmtrigto.p3NewPosition, fGetScale());

		// Debug information only. Can be removed.
		conMessageLog.Print("\nTrigger '");
		conMessageLog.Print(strTriggerName);
		conMessageLog.Print("' responding to an MoveTriggerTo message.\n");
		conMessageLog.Print("New location: %1.2f, %1.2f, %1.2f\n\n", 
							v3Pos().tX, v3Pos().tY, v3Pos().tZ);
	}


//*********************************************************************************************
//
// CActivatePlayer implementations.
//

	//*****************************************************************************************
	bool CActivatePlayer::bActivate(CEntityTrigger* pettrig, const CEntity* pet_subject) const
	{
		// Return false if the base class returns false.
		if (!CActivate::bActivate(pettrig, pet_subject))
			return false;

		Assert(pettrig);
		Assert(pet_subject);
		Assert(pettrig->pbvBoundingVol());

		// Return false if the subject is not a player.
		if (ptCastInstance<CPlayer>((CInstance*)pet_subject) == 0)
			return false;

		//
		// Check to see that the player is in the trigger's bounding volume.
		//

		// Transform the subject's position to the trigger's coordinate frame.
		CVector3<> v3 = pet_subject->v3Pos() - pettrig->v3Pos();

		// Return the result of testing if the point is in the bounding volume..
		return pettrig->pbvBoundingVol()->bContains(v3);
	}


//*********************************************************************************************
//
// CActivatePlayerView implementations.
//

	//*****************************************************************************************
	bool CActivatePlayerView::bActivate(CEntityTrigger* pettrig, const CEntity* pet_subject) const
	{
		// Return false if the base class returns false.
		if (!CActivatePlayer::bActivate(pettrig, pet_subject))
			return false;

		Assert(pettrig);
		Assert(pet_subject);

		//*************************************************************************************
		// Start of temporary code: to be replaced by polyhedral representation of the camera's
		// view volume.
		//*************************************************************************************

		//
		// Get the player's camera and ensure that it is in the correct position.
		//

		// Get a pointer to the player.
		CPlayer* pplay = ptCastInstance<CPlayer>((CInstance*)pet_subject);
		Assert(pplay);

		// Get a pointer to the camera.
		pplay->PositionCamera();
		rptr<CCamera> pcam = ptCastRenderType<CCamera>(pplay->pinsGetCamera()->prdtGetRenderInfo());
		Assert(pcam);

		// Check to see that the trigger is in the player's view pyramid.
		return bIsVisible(*pcam, pettrig->pr3Presence());

		//*************************************************************************************
		// End of temporary code.
		//*************************************************************************************
	}


//
// Module functions.
//

//*********************************************************************************************
bool bIsVisible(const CCamera& cam, const CTransform3<>& tf3_shape_to_world)
{
/*
	// Transform origin of the shape to camera space and clip.
	CObjPointClip optc
	(
		cam.tf3ShapeToCameraSpace(tf3_shape_to_world).v3Pos,
		cam.campropGetProperties().rNearClipPlaneDist,
		cam.campropGetProperties().rFarClipPlaneDist
	);

	// If any of the clipping codes are set, the point falls outside of the view of the camera.
	if (optc.seteoc3OutCode)
		return false;
*/
	return true;
}


//*********************************************************************************************
//
// CTriggerTime implementation.
//

	//*****************************************************************************************
	//
	// CTriggerTime message processing member function implementations.
	//

	//*****************************************************************************************
	void CTriggerTime::Process(const CMessageStep& msgstep)
	{
		// Check to see if the alarm has already been set. If it has not, set it.
		if (!bFirstStep)
		{
			// Indicate that the first step message has been received.
			bFirstStep = true;

			// Set the alarm to go off at the current time plus the duration.
			sAlarm += msgstep.sTotal;
			return;
		}

		// Check to see if the alarm should go off!
		if (sAlarm <= msgstep.sTotal)
		{
			// Create a trigger message.
			CMessageTrigger msgtrig(this);

			// Now actually send it out.
			msgtrig.Dispatch();
		}
	}
