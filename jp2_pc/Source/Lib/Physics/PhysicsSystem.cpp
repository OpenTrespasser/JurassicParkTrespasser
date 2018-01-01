/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of 'PhysicsSystem.hpp.'
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *		BoxInstances[][] contains the instance pointers of objects that currently have 
 *		a state in the physics system.
 *		Info[] contains 100 for objects that are being actively integrated by the physics.
 *		Info[i] can be 0 even if BoxInstances[i][*] is non-0.  This is all so the physics
 *		can turn objects off and on again very quickly within a frame.  Objects are not removed
 *		from BoxInstances[][] until the end of the frame.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/PhysicsSystem.cpp                                        $
 * 
 * 160   98/10/05 8:10 Speter
 * Fixed load so that velocity of bio-boxes is restored.
 * 
 * 159   98/10/05 7:41 Speter
 * Implemented priority for BoundaryBox's particle-lessness. Generate better position for
 * box-box particle effects.
 * 
 * 158   10/01/98 6:49p Mlange
 * Improved collision message stats.
 * 
 * 157   10/01/98 4:05p Mlange
 * Improved move message stat reporting.
 * 
 * 156   98/09/30 19:12 Speter
 * Phys stats. Fixed SetHealth array overwrite.
 * 
 * 155   98/09/28 2:47 Speter
 * Added stat.
 * 
 * 154   98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 *********************************************************************************************/

#include "Common.hpp"
#include "PhysicsSystem.hpp"

#include "PhysicsImport.hpp"
#include "Xob_bc.hpp"
#include "Magnet.hpp"
#include "PhysicsHelp.hpp"
#include "PhysicsStats.hpp"
#include "InfoBox.hpp"
#include "InfoSkeleton.hpp"
#include "Pelvis.h"
#include "BioModel.h"

#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/EntityDBase/Query/QPhysics.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/EntityDBase/Query/QMessage.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgCollision.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgDelete.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPaint.hpp"
#include "Lib/EntityDBase/ParticleGen.hpp"

#include "Lib/GeomDBase/PartitionSpace.hpp"
#include "Lib/GeomDBase/Skeleton.hpp"
#include "Lib/GeomDBase/RayCast.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/StdDialog.hpp"

#include "Game/DesignDaemon/Player.hpp"

#include "Lib/Renderer/Camera.hpp"
#include "Lib/View/LineDraw.hpp"

#include "Lib/Loader/SaveFile.hpp"
#include "Lib/Std/PrivSelf.hpp"

#include <fstream.h>
#include <stdio.h>
#include <set.h>

//
// Danger! Warning! Do no leave this enabled it can and will cause DirectSound to bag.
//
#define VER_FPEXCEPT	0


CProfileStat 
	psIntegrate("Integrate", &proProfile.psPhysics),
		psBoundary("Box Bound", &psIntegrate, Set(epfHIDDEN)),
			psBoxContact("Contact", &psBoundary, Set(epfHIDDEN)),
			psBoxMoments("Moments", &psBoundary, Set(epfHIDDEN)),
		psIntBox("Int Box", &psIntegrate, Set(epfHIDDEN)),
		psIntPelvis("Int Pelvis", &psIntegrate, Set(epfHIDDEN)),
			//psPelTrans("(Trans)", &psIntPelvis, Set(epfHIDDEN)),
			//psPelNorm("(Norm)", &psIntPelvis, Set(epfHIDDEN)),
			psPelFoot("Foot", &psIntPelvis, Set(epfHIDDEN)),
			psIntBio("Int Bio", &psIntPelvis, Set(epfHIDDEN) + epfSEPARATE),
		psIntFinish("Int Finish", &psIntegrate, Set(epfHIDDEN)),
		psQTrrHeight("(Trr Height)", &psIntegrate, Set(epfHIDDEN) + epfOVERLAP),
			psQTrrHeightPel("(Pel)", &psQTrrHeight, Set(epfHIDDEN) + epfOVERLAP),
			psQTrrHeightFind(" Find", &psQTrrHeight),
			psQTrrHeightInit(" Init", &psQTrrHeight),
			psQTrrHeightEvaluate(" Eval", &psQTrrHeight),
			//psQTrrHeightZ(" Z", &psQTrrHeight),
		psQTrrEdge("(Trr Edge)", &psIntegrate, Set(epfHIDDEN) + epfOVERLAP),
		psQTrrFriction("(Trr Fric)", &psIntegrate, Set(epfHIDDEN) + epfOVERLAP),
		psQWater("(Water Height)", &psIntegrate, Set(epfHIDDEN) + epfOVERLAP),
		psIntersect("Intersect", &psIntegrate, Set(epfSEPARATE)),
			psRegion("Region", &psIntersect, Set(epfHIDDEN)),
			psQuery("Query", &psIntersect, Set(epfHIDDEN)),
			psTQuery("TQuery", &psIntersect, Set(epfHIDDEN)),
			psEvaluate("Evaluate", &psIntersect, Set(epfHIDDEN)),
			psAwaken("Awaken", &psIntersect, Set(epfHIDDEN)),
				psAwakenVol("Vol", &psAwaken, Set(epfHIDDEN)),
				psActivate("Activate", &psAwaken, Set(epfHIDDEN)),
				psDeactivate("Deactivate", &psAwaken, Set(epfHIDDEN)),
			psInteract("Interact", &psIntersect, Set(epfHIDDEN)),
			psInteractVol("Vol", &psInteract, Set(epfHIDDEN)),
	psIgnoramus("Ignore", &proProfile.psPhysics, Set(epfSEPARATE) + epfHIDDEN),
	psTestIntersect("Test Intersect", &proProfile.psPhysics, Set(epfSEPARATE) + epfHIDDEN),
		psReportIntersect("Report Intersect", &psTestIntersect, Set(epfSEPARATE) + epfHIDDEN),
	psUpdate("Phys Update", &proProfile.psPhysics, Set(epfSEPARATE) + epfHIDDEN);

//**********************************************************************************************
//
// Global variables.
//

	// Hacky; this shouldn't be global, but we're lazy.
	// The set of all instance pairs ignoring collisions.
	typedef set<TInstancePair, CLessInstancePair> TSetInstancePair; 
	TSetInstancePair setinsprIgnore;

	int iPhysFrame = 0;							// The current physics frame.
												// Currently tracked by ActivateIntersecting.
	static bool bDump = false,					// Whether to dump phys state every frame.
				bDumpStep = false;				// Whether to dump phys state every step.

	list<CInstance*> lspinsSettle;

//**********************************************************************************************
//
namespace
//
// Private static implementation.
//
//**************************************
{
	//
	// Constants.
	//

	#define VER_TEST_GRAPH	1

	// Version number for load/save.
	const int iPHYS_VERSION	= 2;

	int iMAX_SETTLE_OBJS		= iMAX_PHYSICS_MOVABLE*3/4;	// Maximum objects allowed awake during settling.
	const TReal rBONES_RADIUS	= 50.0;						// Distance up to which we draw bones.

	CColour clrBONES_SELECTED	(1.0, 0.0, 0.0);		// Red.
	CColour clrBONES_UNSELECTED	(0.5, 0.0, 0.0);		// Dark red.

	//*****************************************************************************************
	//
	inline CInstance* pinsBox
	(
		int i_index,					// Superbox index number.
		int i_element = 0				// Superbox element.
	)
	//
	// Returns:
	//		The instance corresponding to the index and element, or 0 if it doesn't exist.
	//
	//**********************************
	{
		return Xob[i_index].Instances[i_element];
	}
};

	//*****************************************************************************************
	void DumpPhys(int i_num)
	{
#if VER_TEST
		// Dump the entire damn state to a file.
		char str_fname[100];

		sprintf(str_fname, "PhyState-%03d.txt", i_num);
		ofstream os(str_fname);

		CXob::DumpStateAll(os);
		DumpPelState(os);
		DumpBioState(os);
#endif
	}

//**********************************************************************************************
//
class CPhysicsSystem::CPriv: public CPhysicsSystem
//
// Private implementation.
//
{
public:

	//*****************************************************************************************
	//
	int iActiveBoxes() const;
	//
	// Returns:
	//		Number of active boxes.
	//
	//**********************************

	//*****************************************************************************************
	//
	int iActiveBoxes
	(
		bool b_movable					// Which kind we're interested in.
	) const;
	//
	// Returns:
	//		Number of active (im)movable boxes.
	//
	//**********************************

	//*****************************************************************************************
	//
	int iActiveNonBioBoxes() const;
	//
	// Returns:
	//		Number of active boxes on inanimate objects.
	//
	//**********************************

	//*****************************************************************************************
	//
	int iActiveSkeletons() const;
	//
	// Returns:
	//		Number of active skeletons.
	//
	//**********************************

	//*****************************************************************************************
	//
	void Unignore();
	//
	// Traverses ignore list, and turns off ignoring for those objects no longer intersecting.
	//
	//**********************************

	//*****************************************************************************************
	//
	void DoSettle();
	//
	// Feeds waiting objects into the meat grinder.
	//
	//**********************************

	//*****************************************************************************************
	//
	void ApplyImpulses
	(
		const CVector3<>& v3_centre,	// World origin.
		TReal r_radius,					// Radius of effect.
		TReal r_impulse_max,			// Amount of momentum to apply at centre.
										// Decays linearly with radius.
		bool b_ground					// If true, applies only to objects on ground,
										// with a predominantly upward vector.
										// Otherwise, applies to all objects in a sphere,
										// with an outward vector.
	);
	//
	//**********************************
};

	//*****************************************************************************************
	int CPhysicsSystem::CPriv::iActiveBoxes() const
	{
		// Count boxes.
		int i_active = 0;
		for (int i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
			if (pinsBox(i))
				i_active++;

		return i_active;
	}

	//*****************************************************************************************
	int CPhysicsSystem::CPriv::iActiveBoxes(bool b_movable) const
	{
		// Count boxes.
		int i_active = 0;
		for (int i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
			if (pinsBox(i) && Xob[i].Movable == b_movable)
				i_active++;

		return i_active;
	}

	//*****************************************************************************************
	int CPhysicsSystem::CPriv::iActiveNonBioBoxes() const
	{
		// Count just boxes which belong to inanimate objects.
		int i_active = 0;
		for (int i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
			if (pinsBox(i) && Xob[i].PelvisModel < 0)
				i_active++;

		return i_active;
	}

	//*****************************************************************************************
	int CPhysicsSystem::CPriv::iActiveSkeletons() const
	{
		// Count skeletons.
		int i_active = 0;
		for (int i = 0; i < NUM_PELVISES; i++)
			if (CPhysicsInfoSkeleton::paniSkeleton(i))
				i_active++;

		return i_active;
	}

	//*****************************************************************************************
	void CPhysicsSystem::CPriv::Unignore()
	{
		CCycleTimer	ctmr;

		// Traverse entire set, find which objects still active and intersecting.
		TSetInstancePair setinspr_new;

		for (TSetInstancePair::iterator itinspr = setinsprIgnore.begin(); itinspr != setinsprIgnore.end(); ++itinspr)
		{
			CInstance* pins_me = (*itinspr).first;
			CInstance* pins_you = (*itinspr).second;

			int i_me = iGetIndex(pins_me);
			if (i_me < 0)
				continue;
			int i_you = iGetIndex(pins_you);
			if (i_you < 0)
				continue;

			if (!CPhysicsInfoBox::bIntersect(i_me, i_you))
				continue;

			// Add to the new set.
			TInstancePair inspr(pins_me, pins_you);
			setinspr_new.insert(inspr);
		}

		// Replace old with new.
		setinsprIgnore = setinspr_new;

		psIgnoramus.Add(ctmr(), 1);
	}

	//*****************************************************************************************
	void CPhysicsSystem::CPriv::DoSettle()
	{
		// Automatically activate objects from the settle list.
		while (lspinsSettle.size() && iActiveBoxes(true) < iMAX_SETTLE_OBJS)
		{
			CInstance* pins = *lspinsSettle.begin();
			lspinsSettle.pop_front();

			// Activate it, baby.
			pins->PhysicsActivate();

			if (lspinsSettle.size() % 20 == 0)
				dout << "Delayed settle activation: " <<lspinsSettle.size() <<" objects to go...\n";
		}
	}

	//*****************************************************************************************
	void CPhysicsSystem::CPriv::ApplyImpulses(const CVector3<>& v3_centre, TReal r_radius,
		TReal r_impulse_max, bool b_ground)
	{
		dout <<"Impulses " <<r_impulse_max <<" radius " <<r_radius <<endl;

		// Create a query sphere.
		CPartitionSpaceQuery partsq(CPresence3<>(v3_centre), r_radius);

		// Query movable objects in the region.
		TReal r_inv_rad_sqr = 1.0f / Sqr(r_radius);
		for (CWDbQueryPhysicsMovable wqph(&partsq); wqph; ++wqph)
		{
			CInstance* pins = *wqph;
			if (ptCast<CBoundaryBox>(pins))
				// Skip boundary boxes; for skeletons, apply to ebbFOOT only.
				continue;

			CVector3<> v3_dist = pins->p3GetPlacement().v3Pos - v3_centre;
			TReal r_impulse = r_impulse_max * (1.0f - v3_dist.tLenSqr() * r_inv_rad_sqr);

			// Detect animals and player, and increase impulse to simulate destabilisation.
			if (r_impulse > 0.0f)
			{
				if (ptCast<CPlayer>(pins))
					// Arbitrarily increase the impulse on player, to simulate foot instability.
					r_impulse *= 10.0f;

				CDir3<> d3_impulse = b_ground ? d3ZAxis : CDir3<>(v3_dist);

				// Apply impulse to the object's centre. If it's a skeleton, apply to foot.
				pins->pphiGetPhysicsInfo()->ApplyImpulse(pins, ebbFOOT, pins->p3GetPlacement().v3Pos, d3_impulse * r_impulse);
			}
		}
	}

//**********************************************************************************************
//
// CPhysicsSystem implementation.
//

	CPhysicsSystem* pphSystem = 0;
	CDialogDraw*	pdldrPhysics = 0;

	//*****************************************************************************************
	CPhysicsSystem::CPhysicsSystem()
	{
		SetInstanceName("Joe");

		conLog.OpenFileSession("PhysicsLog.txt", true);
		conLog.SetTabSize(4);
		conLog.Print("Starting physics log.\n");

		bActive = true;
		bAllowSleep = true;
		bShowBones = false;

		// Clear out all state, making nothing active.
		priv_self.Clear();

		// Register this entity with the message types it needs to receive.
		     CMessageStep::RegisterRecipient(this);
		    CMessagePaint::RegisterRecipient(this);
		     CMessageMove::RegisterRecipient(this);
		CMessageCollision::RegisterRecipient(this);
		   CMessageSystem::RegisterRecipient(this);
	}

	//*****************************************************************************************
	CPhysicsSystem::~CPhysicsSystem()
	{
		   CMessageSystem::UnregisterRecipient(this);
		CMessageCollision::UnregisterRecipient(this);
		     CMessageMove::UnregisterRecipient(this);
		    CMessagePaint::UnregisterRecipient(this);
		     CMessageStep::UnregisterRecipient(this);

		// For good measure.
		priv_self.Clear();

//		DeactivateAll();

		conLog.Print("\nEnd log.");
		conLog.CloseFileSession();
	}

	//*****************************************************************************************
	void CPhysicsSystem::DeactivateAll()
	{
		// Deactivate all boxes.
		int i;
		for (i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
			if (pinsBox(i))
				pinsBox(i)->PhysicsDeactivate();

		// Deactivate the skeletons.
		for (i = 0; i < NUM_PELVISES; i++)
			if (CPhysicsInfoSkeleton::paniSkeleton(i))
				CPhysicsInfoSkeleton::paniSkeleton(i)->PhysicsDeactivate();
	}

	//*****************************************************************************************
	int CPhysicsSystem::iGetIndex(const CInstance* pins) const
	{
		return v2iGetIndices(pins).tX;
	}

	//*****************************************************************************************
	CVector2<int> CPhysicsSystem::v2iGetIndices(const CInstance* pins, int i_skip) const
	{
		CVector2<int> v2;

		Assert(pins);

		for (v2.tX = 0; v2.tX < iMAX_PHYSICS_OBJECTS; v2.tX++)
		{
			for (v2.tY = 0; v2.tY < iMAX_BOX_ELEMENTS && pinsBox(v2.tX, v2.tY); v2.tY++)
			{
				if (pinsBox(v2.tX, v2.tY) == pins)
				{
					// Are we at the desired occurrence?
					if (i_skip == 0)
						// Yes!  Return indices.
						return v2;
					else
						// No!  Wait a little longer.
						--i_skip;
				}
			}
		}

		return CVector2<int>(-1, -1);
	}

	//*****************************************************************************************
	int CPhysicsSystem::iGetSkeletonIndex(const CInstance* pins) const
	{
		for (int i = 0; i < NUM_PELVISES; i++)
		{
			if (pins == CPhysicsInfoSkeleton::paniSkeleton(i))
				return i;
		}
		return -1;
	}

	//*****************************************************************************************
	float CPhysicsSystem::fGetHealth(const CInstance* pins) const
	{
		int i_index = iGetSkeletonIndex(pins);
		if (i_index >= 0)
			return GetBioHealth(i_index);
		return 0.0f;
	}

	//*****************************************************************************************
	void CPhysicsSystem::SetHealth(const CInstance* pins, float f_health) const
	{
		int i_index = iGetSkeletonIndex(pins);
		if (i_index >= 0)
		{
			AlwaysAssert(f_health >= 0.0f && f_health <= 100.0f);
			SetBioHealth(i_index, f_health);
		}
	}

	//**********************************************************************************************
	void CPhysicsSystem::IgnoreCollisions(CInstance* pins_me, CInstance* pins_you, bool b_ignore) const
	{
		CTimeBlock	tmb(&psIgnoramus);

		// Optimise for common case.
		if (!b_ignore && setinsprIgnore.size() == 0)
			return;

		if (!pins_you)
		{
			// Unignore all collisions for pins_me.
			Assert(pins_me);
			Assert(!b_ignore);

			TSetInstancePair::iterator itinspr;

			do
			{
				for (itinspr = setinsprIgnore.begin(); itinspr != setinsprIgnore.end(); ++itinspr)
				{
					if ((*itinspr).first == pins_me || (*itinspr).second == pins_me)
					{
						setinsprIgnore.erase(itinspr);
						break;
					}
				}
			}
			while (itinspr != setinsprIgnore.end());
		}
		else
		{
			Assert(pins_me);

			// Replace each instance into with any master magnet instance.
			pins_me  = NMagnetSystem::pinsFindMaster(pins_me);
			pins_you = NMagnetSystem::pinsFindMaster(pins_you);

			// Blow off this anomolous case.
			if (pins_me == pins_you)
				return;

			if (pins_me > pins_you)
				// Make a repeatable order.
				Swap(pins_me, pins_you);
			TInstancePair inspr(pins_me, pins_you);

			if (b_ignore)
			{
				// Add to the set.
				setinsprIgnore.insert(inspr);
			}
			else
			{
				// Remove from the set.
				setinsprIgnore.erase(inspr);
			}
		}
	}

	//**********************************************************************************************
	bool CPhysicsSystem::bIgnoreCollisions(CInstance* pins_me, CInstance* pins_you) const
	{
		CTimeBlock	tmb(&psIgnoramus);

		//
		// Optimise for common cases.
		//

		//
		// An instance should always ignore collisions with itself.
		// This check can also occur in physics code, which may store several bounding boxes on a given
		// biomodel instance. These boxes should ignore each other.
		//
		if (pins_me == pins_you)
			return true;

		if (setinsprIgnore.size() == 0)
			return false;

		if (!pins_you)
		{
			// Return whether any collisions are being ignored for pins_me.
			// This should be used for debugging only.
			Assert(pins_me);

			forall (setinsprIgnore, TSetInstancePair, itinspr)
				if ((*itinspr).first == pins_me || (*itinspr).second == pins_me)
					return true;
			return false;
		}

		Assert(pins_me);

		// Make consistent order.
		if (pins_me > pins_you)
			Swap(pins_me, pins_you);
		TInstancePair inspr(pins_me, pins_you);

		return setinsprIgnore.find(inspr) != setinsprIgnore.end();
	}

	//*****************************************************************************************
	void CPhysicsSystem::Integrate
	(
		float f_elapsed_seconds		// Time elapsed since the last integration.
	)
	{
		if (f_elapsed_seconds <= 0)
			return;

		if (f_elapsed_seconds > .25) 
		{
//			Assert(false);
			f_elapsed_seconds = .25;
		}

		//float f_max_system_step = .001;//.0075;//`.01;
		//float f_max_bio_step    = .001;
		float f_system_seconds = f_elapsed_seconds;

		CPhysicsInfoBox::UpdateQueries();

		// Set the FPU to single precision.
		CFPUState fpus;
		fpus.SetLowRes();

#if (VER_FPEXCEPT)
		//
		// Turn on floating point exceptions.
		//
		fpus.EnableExceptions
		(
			CFPUState::fpuINVALID_OPERATION |
			CFPUState::fpuZERO_DIVIDE		|
			CFPUState::fpuOVERFLOW
		);
			
#endif

		CXob::PD_Integrate(f_elapsed_seconds);
		
		// Restore FPU state.
		fpus.Restore();

		// Clear out ignore list.
		priv_self.Unignore();

#if VER_TIMING_STATS

	#if VER_TEST_GRAPH
		{
			// Graph the number of objects active.
			const int iX_ELEMS = 200;
			const int iY_ELEMS = 20;

			static int i_x = 0;

			if (pdldrPhysics)
			{
				if (i_x == 0 || i_x >= iX_ELEMS)
				{
					pdldrPhysics->SetVirtualRect(CRectangle<>(0.0, 0.0, iX_ELEMS, iY_ELEMS).rcFlipY());
					pdldrPhysics->Clear(CColour(0.0, 0.0, 0.3));
					pdldrPhysics->Colour(CColour(0.0, 0.5, 1.0));
					//pdldrPhysics->MoveTo(0, 0);
					i_x = 0;
				}

				pdldrPhysics->Point(i_x++, iActiveObjects());
				//pdldrPhysics->Point(i_x++ % iX_ELEMS, i_x % iY_ELEMS);
			}
		}
	#endif

#endif
	}

	//*****************************************************************************************
	void CPhysicsSystem::Clear()
	{
		CPhysicsInfoBox::Reset();
		CPhysicsInfoSkeleton::Reset();

		// Clear ignore list.
		erase_all(setinsprIgnore);
		erase_all(lspinsSettle);

		iPhysFrame = 0;
	}

	//*****************************************************************************************
	void CPhysicsSystem::Process(const CMessageSystem& msgsys)
	{
	}

	//*****************************************************************************************
	void CPhysicsSystem::ActivateIntersecting() const
	{
		iPhysFrame++;

		if (bDumpStep)
			DumpPhys(iPhysFrame);
		CPhysicsInfoBox::ActivateIntersecting();
	}
	
	//*****************************************************************************************
	void CPhysicsSystem::Process(const CMessageStep& msgstep)
	{
		// Don't run if physics is off.
		if (!bActive)
			return;

		CCycleTimer ctmr;

		// sStep is capped at .2 seconds in MessageTypes.hpp
		//		msgstep.sStep;					// The time since the last step.
		//		msgstep.sTotal;					// The total elapsed simulation time since game start.

		priv_self.DoSettle();

		// Take a velocity snapshot.
		int i;
		for (i = 0; i < iMAX_PHYSICS_OBJECTS; ++i)
		{
			if (pinsBox(i))
				Xob[i].fInitialEnergy = Xob[i].fGetKineticEnergy();
			else
				Xob[i].fInitialEnergy = 0;
		}

		// Call the integrator here.
		Integrate(msgstep.sStep);

		// Tell the objects where they are now.
		UpdateWDBase();

		proProfile.psPhysics.Add(ctmr(), iActiveObjects());

		if (bDump)
			DumpPhys(iPhysFrame);

		// Process particles. Done here to avoid the bother of making Particles an entity.
		Particles.Update(msgstep.sStep);
	}

	//*****************************************************************************************
	TSoundMaterial smatAlternate(CInstance* pins)
	{
		static TSoundMaterial mat_terrain	= matHashIdentifier("Terrain"),
							  mat_animate	= matHashIdentifier("Animate");

		// Determine alternate sound material by class. To do: make it a virtual function.
		return	ptCast<CTerrain>(pins) ? mat_terrain :
				ptCast<CAnimate>(pins) ? mat_animate :
				0;
	}

	const float fENERGY_MAX_INV = 1.0f / 1000.0f;
	const float fENERGY_MIN_INV	= 1.0f / 0.1f;
	const float fENERGY_LOG_INV	= 1.0f / log(fENERGY_MIN_INV / fENERGY_MAX_INV);

	const int	iPRIORITY_BIO	= 42;		// Hard-coded priority for disabling collisions on animals.

	extern CProfileStat psCollisionMsgPhysics;

	//*****************************************************************************************
	void CPhysicsSystem::Process(const CMessageCollision& msgcoll)
	{
		CTimeBlock tmb(&psCollisionMsgPhysics);

		if (!msgcoll.fEnergyMax)
			return;

		CCreateParticles* pcrt1;
		CCreateParticles* pcrt2;
		int i_pri_1, i_pri_2;

		//
		// We want to disable most effects on collisions with CBoundaryBoxes.
		// (Footsteps are generated via a special CAnimate/CTerrain collision, and blood is
		// an entirely different system). For this, we have a hard-coded priority for
		// boundary boxes which requests no effect at all. For other materials with no
		// specified effect, the priority is -1, so that any effect will override it.
		//

		// Find effect for both materials.
		if (ptCast<CBoundaryBox>(msgcoll.pins1))
		{
			pcrt1 = 0;
			i_pri_1 = iPRIORITY_BIO;
		}
		else
		{
			pcrt1 = msgcoll.pins1 ? NParticleMap::pcrtFind(msgcoll.smatSound1, smatAlternate(msgcoll.pins1)) : 0;
			i_pri_1 = pcrt1 ? pcrt1->iPriority : -1;
		}

		if (ptCast<CBoundaryBox>(msgcoll.pins2))
		{
			pcrt2 = 0;
			i_pri_2 = iPRIORITY_BIO;
		}
		else
		{
			pcrt2 = msgcoll.pins2 ? NParticleMap::pcrtFind(msgcoll.smatSound2, smatAlternate(msgcoll.pins2)) : 0;
			i_pri_2 = pcrt2 ? pcrt2->iPriority : -1;
		}

		CInstance* pins;
		CCreateParticles* pcrt;

		// Choose whichever one exists; or if both, whichever one has higher priority.
		if (i_pri_1 > i_pri_2)
		{
			pins = msgcoll.pins1;
			pcrt = pcrt1;
		}
		else
		{
			pins = msgcoll.pins2;
			pcrt = pcrt2;
		}

		if (!pcrt)
			// No effect was specified.
			return;

		// Normalise the collision strength.
		float f_strength;
		if (pcrt->bLogScale)
			// Logarithmic conversion.  Scale min..max to 0..1.
			f_strength = log(msgcoll.fEnergyMax * fENERGY_MIN_INV) * fENERGY_LOG_INV;
		else
			// Linear conversion.
			f_strength = msgcoll.fEnergyMax * fENERGY_MAX_INV;

		if (f_strength >= pcrt->ppMin.fThreshold)
		{
			Assert(f_strength >= 0.0f);

			// Find face normal at this location.
			SObjectLoc obl;
			const CBoundVol* pbv = pins->pphiGetPhysicsInfo()->pbvGetBoundVol();
			Assert(pbv);
			if (!pbv->bRayIntersect(&obl, pins->pr3GetPresence(), CPlacement3<>(msgcoll.v3Position), 0, 0))
			{
				obl.v3Location = msgcoll.v3Position;
				obl.d3Face = d3ZAxis;
			}

			if (msgcoll.pins2 == 0 && msgcoll.petGetSender())
			{
				// Collision came from a bullet. Average direction with bullet reflection direction.
				CDir3<> d3_gun = msgcoll.petGetSender()->p3GetPlacement().v3Pos - msgcoll.v3Position;
				obl.d3Face = obl.d3Face * (obl.d3Face * d3_gun * 2.0f + 1.0f) - d3_gun;
			}

  			pcrt->Create(obl.v3Location, obl.d3Face, 1.0, f_strength);
		}
	}

	extern CProfileStat psMoveMsgPhysics;

	//*****************************************************************************************
	void CPhysicsSystem::Process(const CMessageMove& msgmv)
	{
		CTimeBlock tmb(&psMoveMsgPhysics);

		// Update physics system if the object is active and was moved by another entity.
		if (msgmv.etType == CMessageMove::etMOVED && msgmv.petGetSender() != this)
			msgmv.pinsMover->pphiGetPhysicsInfo()->Activate(msgmv.pinsMover, true);
	}

	//*****************************************************************************************
	void CPhysicsSystem::Process(const CMessageDelete& msgdel)
	{
		msgdel.pinsDeleteMe->PhysicsDeactivate();
		CPhysicsInfoBox::ProcessCreateDelete();
	}

	//*****************************************************************************************
	void CPhysicsSystem::Process(const CMessagePaint& msgpaint)
	{
#if VER_TEST
		// Update physics stats.
		if (conPhysicsStats.bIsActive() && proProfile.psPhysics.iGetCount() % 5 == 0)
		{
			// Write all physics stats to console, even "hidden".
			conPhysicsStats.ClearScreen();
			CProfileStat::WriteHeader(conPhysicsStats);
			proProfile.psPhysics.WriteToConsole(conPhysicsStats, true);
			conPhysicsStats.Show();
		}
#endif

#if bVER_BONES()
		if (!bShowBones)
			return;

		// Construct draw object for target raster.
		rptr<CRaster> pras = rptr_this(msgpaint.renContext.pScreenRender->prasScreen);
		rptr<CRasterWin> prasw = rptr_dynamic_cast(CRasterWin, pras);
		if (!prasw)
			return;

		CDraw draw(prasw);

		// Show da bones.

		// Iterate through physics objects, whether rendered or not.
		for (CWDbQueryPhysics wqph(&msgpaint.camCurrent); wqph.bIsNotEnd(); wqph++)
		{
			if (!(*wqph)->pphiGetPhysicsInfo()->bIsActive(*wqph))
				if (((*wqph)->v3Pos() - msgpaint.camCurrent.v3Pos()).tLenSqr() > Sqr(rBONES_RADIUS))
					continue;

			// Draw any physics info.
			draw.Colour(1 ? clrBONES_SELECTED : clrBONES_UNSELECTED);
			(*wqph)->pphiGetPhysicsInfo()->DrawPhysics(*wqph, draw, msgpaint.camCurrent);
		}

		// Here we draw any ray-casts as well.
		if (CPhysicsInfo::setedfMain[edfRAYCASTS])
			CRayCast::DrawPhysics(draw, msgpaint.camCurrent);

		// Display some funny textual data.
		CConsoleBuffer con;
		con.SetTransparency(true);

		draw.Colour(CColour(1.0, 1.0, 0.0));
		con.Print("Boxes: %d mov, %d imm\nSkels: %d\n", 
			priv_selfc.iActiveBoxes(true), priv_selfc.iActiveBoxes(false), priv_selfc.iActiveSkeletons());
		con.Show(draw.hdcScreen, prasw->iHeight, true);
#endif
	}

	//*****************************************************************************************
	void CPhysicsSystem::UpdateWDBase
	(
	)
	{
		CCycleTimer ctmr;

		//  Grab box model data
		int i;
		for (i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
		{
			// Update if active.
			if (pinsBox(i))
			{
				CPhysicsInfoBox::UpdateWDBase(i);
				psUpdate.Add(ctmr(), 1);
			}
		}

		//  Grab bio model data
		for (i = NUM_PELVISES-1; i >= 0; i--)
		{
			if (CPhysicsInfoSkeleton::paniSkeleton(i))
			{
				const CPhysicsInfo* pphi = CPhysicsInfoSkeleton::paniSkeleton(i)->pphiGetPhysicsInfo();
				pphi->UpdateWDBase(CPhysicsInfoSkeleton::paniSkeleton(i), i);
				psUpdate.Add(ctmr(), 1);
			}
		}
	}

	//*****************************************************************************************
	int CPhysicsSystem::iActiveObjects() const
	{
		return priv_selfc.iActiveBoxes() + priv_selfc.iActiveSkeletons();
	}

	//*****************************************************************************************
	void CPhysicsSystem::ActivateSettle(CInstance* pins)
	{
		if (priv_selfc.iActiveBoxes(true) < iMAX_SETTLE_OBJS)
			// Activate this now.
			pins->PhysicsActivate();
		else
			// Append this to the settle list, for later activation.
			lspinsSettle.push_back(pins);
	}

	//*****************************************************************************************
	char* CPhysicsSystem::pcSave(char* pc) const
	{
		//
		//	JOE
		//

		// Version
		*pc++ = iPHYS_VERSION;

		//
		// Save the magnet table.
		// Must be done before objects so they are activated correctly.
		//
		pc = NMagnetSystem::pcSave(pc);

		//
		// Menu-settable flags.
		//
		SaveT(&pc, bActive);
		SaveT(&pc, bAllowSleep);

		//
		// Save skeletons before boxes, as this ensures that bio-linking is done correctly.
		//
		int i;
		for (i = 0; i < NUM_PELVISES; i++)
		{
			if (CPhysicsInfoSkeleton::paniSkeleton(i))
			{
				//
				// Save the instance pointer, activity, and velocity.
				// The velocity is taken from the pelvic state for the body.
				//
				pc = pcSaveInstancePointer(pc, CPhysicsInfoSkeleton::paniSkeleton(i));
				SaveT(&pc, 100);
				SaveT(&pc, CPlacement3<>());

				pc = pcSavePelvis(pc, i);
			}
		}

		// Save boxes.
		for (i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
		{
			if (pinsBox(i))
			{
				//
				// Save the instance pointer, activity, and velocity.
				// We only need save the primary instance of each superbox; the rest
				// will be activated automatically.
				//
				pc = pcSaveInstancePointer(pc, pinsBox(i));
				SaveT(&pc, Xob[i].Info);
				SaveT(&pc, p3Convert(Xob[i].State, 1));
			}
		}

		// Don't forget end-of-instances flag.
		pc = pcSaveInstancePointer(pc, 0);

		return pc;
	}

	//*****************************************************************************************
	const char* CPhysicsSystem::pcLoad(const char* pc)
	{
		//
		//	JOE
		//

		// Check version number.
		uint8 u1_version = *pc++;
		AlwaysAssert(u1_version == iPHYS_VERSION);

		// Make us squeaky-clean.
		priv_self.Clear();

		//
		// Load the magnet table.
		// Must be done before objects so they are activated correctly.
		//
		pc = NMagnetSystem::pcLoad(pc);

		//
		// Menu-settable flags.
		//
		LoadT(&pc, &bActive);
		LoadT(&pc, &bAllowSleep);

		//
		// Load active instances.
		// Loop till we get a 0 instance.
		//

		CInstance* pins;
		while (pc = pcLoadInstancePointer(pc, &pins), pins)
		{
			// Load activity level and velocity.
			int i_activity;
			CPlacement3<> p3_vel;

			LoadT(&pc, &i_activity);
			LoadT(&pc, &p3_vel);

			// Add to physics system.
			Assert(pins);

			// First make sure active. Then reset to desired velocity.
			pins->pphiGetPhysicsInfo()->Activate(pins);
			pins->pphiGetPhysicsInfo()->Activate(pins, true, p3_vel);

			// Hacky. Should pass info to Activate, but this will do.
			if (CSaveFile::iCurrentVersion >= 8)
			{
				int i_pel = iGetSkeletonIndex(pins);
				if (i_pel >= 0)
					// It was a skeleton model.
					pc = pcLoadPelvis(pc, i_pel);
			}
		}

		DumpPhys(0);

		return pc;
	}


#if 0
	inline void DrawLine
	(
		const CVector3<>&		v3_world1,	// The instance whose magnets we draw.
		const CVector3<>&		v3_world2,	// The instance whose magnets we draw.
		CDraw&					draw,		// The drawing class.
		CCamera&				cam			// The view camera.
	)
	{
		// Another plus sign
		draw.Line
		(
			cam.ProjectPoint(v3_world1 * cam.tf3ToNormalisedCamera() ),
			cam.ProjectPoint(v3_world2 * cam.tf3ToNormalisedCamera() )
		);
	}


	static void DrawPoint
	(
		const CVector3<>&		v3_world,	// The instance whose magnets we draw.
		CDraw&					draw,		// The drawing class.
		CCamera&				cam			// The view camera.
	)
	{
		CVector3<> v3_draw_me = v3_world * cam.tf3ToNormalisedCamera();

#define POINT_RADIUS TReal(.03)

		CVector3<> v3_a = v3_world;
		CVector3<> v3_b = v3_world;
		v3_a.tX += POINT_RADIUS;
		v3_b.tX -= POINT_RADIUS;

		DrawLine(v3_a, v3_b, draw, cam);

		v3_a = v3_world;
		v3_b = v3_world;
		v3_a.tY += POINT_RADIUS;
		v3_b.tY -= POINT_RADIUS;

		DrawLine(v3_a, v3_b, draw, cam);

		v3_a = v3_world;
		v3_b = v3_world;
		v3_a.tZ += POINT_RADIUS;
		v3_b.tZ -= POINT_RADIUS;

		DrawLine(v3_a, v3_b, draw, cam);
	}
#endif

