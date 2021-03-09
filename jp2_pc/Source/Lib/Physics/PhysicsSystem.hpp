/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CPhysicsSystem
 *
 * Bugs:
 *
 * To do:
 *   Make a proper header file for PaleoDyne(tm) funtions.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/PhysicsSystem.hpp                                         $
 * 
 * 62    98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 61    98/09/19 14:54 Speter
 * Particles now handled here.
 * 
 * 60    98/09/12 1:08 Speter
 * Made Clear() public.
 * 
 * 59    98/09/07 1:46 Speter
 * Replaced Settle() function with ActivateSettle().
 * 
 * 58    98/07/24 14:00 Speter
 * Moved some box/skeleton specific stuff to appropriate classes.
 * 
 * 57    98/07/20 13:24 Speter
 * Removed obsolete function.
 * 
 * 56    98/06/29 16:26 Speter
 * Removed unused v3GetJointWorldPosition().
 * 
 * 55    98/06/26 20:33 Speter
 * Removed unused parameter.
 * 
 * 54    98/06/09 21:25 Speter
 * Moved default physics param to PhysicsInfo.hpp.
 * 
 * 53    6/01/98 6:08p Agrant
 * get and set internal physics health parameter
 * 
 * 52    98/05/22 13:13 Speter
 * Moved TestIntersect to InfoBox.
 * 
 * 51    98/05/15 16:04 Speter
 * Physics is now informed of moves via CMessageMove, rather than explicitly. Added CPriv.
 * 
 * 50    98/05/05 16:43 Speter
 * Added TestIntersect(), tf3BoxElement(). Updated call to ActivateIntersecting. 
 * 
 * 49    4/06/98 3:44p Agrant
 * Moved some set declarations into .hpp file to allow WDBase reset.
 * 
 * 48    98/03/06 10:50 Speter
 * Moved bones display to CPhysicsSystem, based on MsgPaint.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_PHYSICSSYSTEM_HPP
#define HEADER_LIB_PHYSICS_PHYSICSSYSTEM_HPP

#include "Lib/EntityDBase/Subsystem.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/Sys/Timer.hpp"
#include "Lib/Std/Array2.hpp"

class CCamera;
class CDraw;
class CDialogDraw;

//**********************************************************************************************
//
class CPhysicsSystem : public CSubsystem
//
// Prefix: ph
//
//	Notes:
//		Also known as 'Joe'.  Handles all time-based physics functionality.  Interfaces to
//		real physics modules elsewhere in this directory.
//
//**************************************
{
public:

	bool		bActive;									// True is the physics system runs
															// on a step message.
	bool		bAllowSleep;								// True if sleep is checked for.
	bool		bShowBones;									// Flag to draw debug bones.

	CConsoleBuffer		conLog;								// Buffer for dumping a log file.

	//******************************************************************************************
	//
	// Constructors.
	//
public:

	// default constructor
	CPhysicsSystem();
	
	~CPhysicsSystem();
	
	//******************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void DeactivateAll();
	//
	// Deactivates all objects.
	//
	//**********************************

	//*****************************************************************************************
	//
	int iGetIndex
	(
		const CInstance* pins		// The object whose index has been requested.
	) const;
	//
	// Gets the index of the object in the State physics variable, or -1 if not active.
	//
	//**************************

	//*****************************************************************************************
	//
	CVector2<int> v2iGetIndices
	(
		const CInstance* pins,	// The object whose index has been requested.
		int i_skip=0			// The number of occurrences of the instance to skip.
	) const;
	//
	// Returns:
	//		Two integers in the form of a CVector2<int>.
	//		.tX is the index of the instance (or -1 if inactive), .tY is the sub-element
	//		of the compound box.
	//
	//**************************

	//*****************************************************************************************
	//
	void IgnoreCollisions
	(
		CInstance* pins_me, CInstance* pins_you,
		bool b_ignore					// True to ignore collisions, false to notice them.
	) const;
	//
	// Causes collisions between these two objects to be ignored, or noticed.
	// 
	// If pins_you is 0 and b_ignore is false, then all no collisions with pins_me will be ignored.
	//
	//**********************************

	//*****************************************************************************************
	//
	bool bIgnoreCollisions
	(
		CInstance* pins_me, CInstance* pins_you		// The instances to test.
	) const;
	//
	// Returns:
	//		Whether collisions between these two instances are ignored.
	//
	//**********************************

	//*****************************************************************************************
	//
	int iGetSkeletonIndex
	(
		const CInstance* pins	// The object whose index has been requested.
	) const;
	//
	// Returns:
	//		The index of the object in the State physics variable, or -1 if not active.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual float fGetHealth
	(
		const CInstance* pins
	) const;
	//
	//	For completeness, query the physics system's notion of the animal's health
	//
	//*****************

	//*****************************************************************************************
	//
	virtual void SetHealth
	(
		const CInstance*	pins,
		float				f_health
	) const;
	//
	//	Set the physics system's notion of health for this.  0-100, a percentage of health.
	//	100 is healthy.
	//	0 is dead.
	//
	//*****************

	//*****************************************************************************************
	//
	void Integrate
	(
		float f_elapsed_seconds		// Time elapsed since the last integration.
	);
	//
	// Gives the physics system a few clock cycles to process object motion.
	//
	//	Notes:
	//		Performs integration for an entire frame in one go.
	//
	//**************************
	
	//*****************************************************************************************
	//
	void UpdateWDBase
	(
	);
	//
	// Update the CInstance data.
	//
	//**************************
	
	//*****************************************************************************************
	//
	int iActiveObjects() const;
	//
	// Returns:
	//		Total number of objects currently active in physics.
	//
	//**********************************

	//*****************************************************************************************
	//
	void ActivateSettle
	(
		CInstance* pins			// Object to activate for settling.
	);
	//
	// Sets the given object up for settling. Will be activated either immediately or when
	// there's room.
	//
	//**********************************

	//*****************************************************************************************
	//
	void ActivateIntersecting() const;
	//
	// Wakes up all objects whose physics bounding volumes intersect the given object.
	//
	//**************************

	//*****************************************************************************************
	//
	void Clear();
	//
	// Clears physics state, deactivating all objects without updating them.
	//
	//**********************************

	//*****************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual void Process(const CMessageSystem& msgsys) override;

	//*****************************************************************************************
	//
	virtual void Process(const CMessageStep& msgstep) override;
	//
	// Gives the physics system a few clock cycles to process object motion.
	//
	//**************************

	//*****************************************************************************************
	//
	void Process(const CMessageMove& msgmv) override;
	//
	// Updates active objects which are moved elsewhere.
	//
	//**********************************

	//*****************************************************************************************
	//
	void Process(const CMessageCollision& msgcoll) override;
	//
	// Provides auxilliary effects for collisions.
	//
	//**********************************

	//*****************************************************************************************
	//
	virtual void Process(const CMessageDelete& msgdel) override;
	//
	// Handles removal of world objects.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void Process(const CMessagePaint& msgpaint) override;
	//
	// If bShowBones is on, draws physics boxes, bones, other debugging info.
	//
	//**********************************

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const override;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer) override;

	class CPriv;
};

	//**********************************************************************************************
	//
	// Definitions for implementing IgnoreCollisions.
	//

	typedef std::pair<CInstance*, CInstance*> TInstancePair;
	// Prefix: inspr.
	// These are always stored in ascending address order, for unambiguity.

	//**********************************************************************************************
	class CLessInstancePair
	{
	public:
		bool operator ()(const TInstancePair& inspr1, const TInstancePair& inspr2) const
		{
			return inspr1.first != inspr2.first ? inspr1.first < inspr2.first
												: inspr1.second < inspr2.second;
		}
	};


extern CPhysicsSystem* pphSystem;

extern CDialogDraw*	pdldrPhysics;						// Special draw dialog.

//#ifndef HEADER_LIB_PHYSICS_PHYSICSSYSTEM_HPP
#endif
