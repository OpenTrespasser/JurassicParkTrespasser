/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of InfoBox.hpp
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/InfoBox.cpp                                              $
 * 
 * 148   98/10/07 6:27 Speter
 * Fix velocity calc in case of QNAN, so that blowups can be fixed.
 * 
 * 147   98/10/06 18:58 Speter
 * Calling Activate on active object now juices related activity fields as well as Info. This
 * saves floating objects from the sleep Nazi.
 * 
 * 146   98/10/04 23:22 Speter
 * Added safety checks and clamps to all query region functions. Do not sleep driven magnets.
 * 
 * 145   98/10/03 4:42 Speter
 * Removed misleading and useless bvbBoundBox(-1).
 * 
 * 144   10/02/98 11:38p Agrant
 * max out the kinetic energy velocity to avoid extra-damage blow-ups
 * 
 * 143   98/10/02 17:19 Speter
 * Added iTerrainSound and fFriction. Nearby terrain textures now maintained by query regions.
 * Only non-human terrain queries are doubled.
 * 
 * 142   98/10/01 22:16 Speter
 * Fixed crash when activating dead dino box, by arranging for all its boxes to have valid
 * query regions on creation.
 * 
 * 141   98/10/01 16:32 Speter
 * Simpler testing of boxes vs. spheres in wakeup; wakeup always uses boxes; interaction uses
 * spheres based on size only. Use new collision volumes and presences.
 * 
 * 140   98/09/30 19:29 Speter
 * Head/tail orientation now stored in State like other boxes. Now let animal heads/tails
 * maintain separate query regions, for speed. Assert in terrain queries for containment.
 * Utilise fast CBoundVolBox constructors.
 * 
 * 139   98/09/28 2:50 Speter
 * Strictly maintain lspinsNear lists for inactive-only objects. Now check blowups on lower
 * velocity (100) as well as region size. Added f to float constants. Removed sub-stats. Added
 * CheckPelvis.
 * 
 * 138   98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 137   9/22/98 10:40p Pkeet
 * Changed the overloaded '*' operator for CColour to use floats instead of doubles.
 * 
 * 136   98/09/17 3:34 Speter
 * Better Blowup Wizard handling; shouldn't assert.
 * 
 * 135   9/15/98 10:57p Jpaque
 * Moved var to solve init order prob.
 * 
 * 134   98/09/14 1:46 Speter
 * Added the Blowup Wizard, to deactivate errant objects and animals.
 * 
 * 133   98/09/09 23:24 Speter
 * Fixed the damn magnet pickup; wasn't clearing PelvisElem; how did it work before? Turned off
 * immobilisation of boxes when too many.
 * 
 * 132   98.09.08 2:35p Mmouni
 * Made changes to support creation of non CMesh objects for invisible geometry.
 * 
 * 131   98/09/08 13:05 Speter
 * New edge query now intersects with box. Draw'em in Bones (Vertex Attachments, of all
 * things). Bit of a fix for hinge magnets. Store velocity expansion in static array.
 * 
 * 130   98/09/06 19:51 Speter
 * Much improved the sleep Nazi; now properly tracks LastTouched. Draw terrain edges if Vertex
 * Attachment selected!
 * 
 * 129   98/09/04 22:00 Speter
 * Dinos now deactivate and reactivate when dead. Fixed logic of creation of Query regions on
 * object wake-up. Added sender to Move(). Increase sleep threshold faster with # objects.
 * 
 *********************************************************************************************/


//
// Includes.
//

#include "common.hpp"
#include "InfoBox.hpp"

#include <set>
#include "Lib/GeomDBase/PartitionPriv.hpp"

#include "PhysicsSystem.hpp"
#include "InfoCompound.hpp"
#include "InfoSkeleton.hpp"
#include "Xob_bc.hpp"
#include "Magnet.hpp"
#include "PhysicsHelp.hpp"
#include "PhysicsStats.hpp"
#include "PhysicsImport.hpp"
#include "futil.h"
#include "Lib/Sys/Profile.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/Query/QPhysics.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/EntityDBase/Query/QWater.hpp"
#include "Lib/EntityDBase/Query/QTerrainObj.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeQuery.hpp"
#include "Lib/GeomDBase/TerrainObj.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Math/MathUtil.hpp"

#include <algorithm>

// Debugging rendering stuff.
#include "Lib/Renderer/Camera.hpp"
#include "Lib/View/LineDraw.hpp"

#include "Lib/Sys/ConIO.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Transform/TransformIO.hpp"

extern int iPhysFrame;

//**********************************************************************************************
//
// namespace
//
// Private implementation.
//
//**************************************
// {

	CColour clrACTIVE		(1.0, 0.0, 0.0);
	CColour clrIMMOVABLE	(0.8, 0.4, 0.0);
	CColour clrINACTIVE		(0.5, 0.0, 0.5);
	CColour clrATTACH		(0.3, 0.8, 0.0);
	CColour clrIGNORE		(0.0, 0.9, 0.0);
	CColour clrACTIVATE		(0.0, 0.0, 0.5);
	CColour clrQUERY		(0.0, 0.0, 1.0);

	// Query region parameters.
	const TReal rREGION_MIN_SLACK		= 0.5;		// Minimum size region expanded by.
	const TReal rREGION_VELOCITY_SLACK	= 10.0;		// Estimated number of physics steps region expanded by.
	const TReal rSANE_VELOCITY			= 100.0;	// Max sane velocity of object before discipline.
	const TReal rSANE_REGION_SIZE		= 60.0;		// Max sane size of object query region before discipline.


	const TReal rBOX_DIM_WARN	= 0.005;
	const TReal rCOLLIDE_EXPAND	= 0.04;		// Amount collision volumes are grown for safety.
	const TReal rMIN_RADIUS_BOX	= 0.25;		// Minimum radius to use box test for.
	const TReal rMIN_RATIO_BOX	= 2.15;		// Minimum extent ratio to use box test for 
											// (derived from a box with one dimension the golden ratio
											// larger than the others).
	// Sleep constants.
	const int iINFO_REMOVE		= -50;		// Buffer deactivations by many steps, 
											// to preserve lifetime in oscillitory cases.
											// To do: replace with lifetime memory only when near other objects.
	const TSec sFREE_AGE		= 8.0;		// Age an object can have before we get stricter about sleeping.
	const int iFREE_AGE			= sFREE_AGE / MAX_TIMESTEP;	// Number of physics steps.
	const float fSLEEP_GROW		= pow(100.0, 1.0/iFREE_AGE);

#define fMAX_DAMAGE_VEL 20.0f

	const CVector3<> v3CORNFIELD(0, 0, -50000);
	int iCornfield		= 1;				// Whether to punish naughty objects.
											// 1 = reset, 2 = wish to cornfield.
	//*****************************************************************************************
	//
	class CQueryRegion
	//
	// Prefix: qreg
	//
	//**********************************
	{
	public:
		CVector3<>					v3Min, v3Max;	// Limits of current query box.
		aptr< std::list<CInstance*> >	plspinsNear;	// List of inactive objects in this region.
		aptr<CWDbQueryTerrainTopology>	
									pwqttopTerrain;	// The current terrain query region.
		CTerrainObj*				ptoBase;		// The base, or default texture for this region,
													// if any.
		std::list<CTerrainObj*>			lsptoTerrainObjs;// Non-default terrain objects in this query region.
		CAArray<TVectorPair>		pavpEdges;		// Array of terrain edges in this region.
		aptr<CWDbQueryWater>		pwqwtrWater;	// List of water objects intersecting this region.

		//**************************************************************************************
		CQueryRegion()
		{
		}

		//**************************************************************************************
		//
		bool bActive() const
		//
		//**********************************
		{
			return plspinsNear != 0;
		}

		//**************************************************************************************
		//
		void Query
		(
			const CVector3<>& v3_min, 
			const CVector3<>& v3_max, 
			float f_trr_scale = 1.0f
		);
		//
		// Fill this with data for the given region.
		//
		//**********************************

		//**************************************************************************************
		//
		void Query
		(
			int i_index
		);
		//
		// Fill this with data for the given box index, computing the region, and assuming zero velocity.
		//
		//**********************************

		//**************************************************************************************
		//
		void GetEdges();
		//
		// Fills the pavpEdges array with the terrain edges in this region.
		//
		//**********************************

		//**************************************************************************************
		//
		void AddInstance
		(
			CInstance* pins					// New inactive instance to add.
		);
		//
		// Add within query region, add instance to list of inactive nearby objects.
		//
		//**********************************

		//**************************************************************************************
		//
		void RemoveInstance
		(
			CInstance* pins					// Instance to remove.
		)
		//
		// Remove instance from list of nearby inactive objects.
		// Called, e.g. when pins activated.
		//
		//**********************************
		{
			// Remove instance from query list.
			Assert(bActive());
			plspinsNear->remove(pins);
		}

		//**************************************************************************************
		//
		void Clear()
		//
		//**********************************
		{
			plspinsNear = 0;
			pwqttopTerrain = 0;
			erase_all(lsptoTerrainObjs);
			ptoBase = 0;
			pavpEdges = CPArray<TVectorPair>();
			pwqwtrWater = 0;
		}

		//**************************************************************************************
		//
		bool bContains
		(
			float f_x, float f_y
		) const
		//
		//**********************************
		{
			if (bActive())
				return bWithin(f_x, v3Min.tX, v3Max.tX) &&
					   bWithin(f_y, v3Min.tY, v3Max.tY);
			else
				return false;
		}

		//**************************************************************************************
		//
		void Clamp
		(
			float& f_x, float& f_y
		) const
		//
		//**********************************
		{
			if (bActive())
			{
				SetMinMax(f_x, v3Min.tX, v3Max.tX);
				SetMinMax(f_y, v3Min.tY, v3Max.tY);
			}
		}

		//**************************************************************************************
		//
		const CTerrainObj* ptoTerrainObject
		(
			float f_x, float f_y		// World location to query.
		);
		//
		// Returns:
		//		The top terrain object at this 2D location.
		//
		//******************************

	private:

		//**************************************************************************************
		//
		void QueryTrrObjs
		(
			float f_scale = 1.0f		// Scale to expand query volume by;
										// used for dino pelvises.
		);
		//
		// Get the terrain object list.
		//
		//******************************

	};

	CQueryRegion	aqregNear[iMAX_PHYSICS_OBJECTS];
// };


class CJoeXob;

//*****************************************************************************************
//
CJoeXob& JoeXob(int i_index);
//
//	Returns:  
//		The object of this index.
//
//***************************

//*****************************************************************************************
//
class CJoeXob: public CXob
{
public:

	//*****************************************************************************************
	//
	CPlacement3<> p3BoxMain() const
	//
	//	Returns:  
	//		The placement of the main box model in the row indexed by i_index
	//
	//***************************
	{
		CPlacement3<> p3;					// Return value.
		p3 = p3Convert(State, 0);
#if VER_DEBUG
		if (Abs(p3.r3Rot.tLenSqr() - 1.0f) > 0.01f) dout << "Frame " << CMessageStep::u4Frame << ": Box " << Index() << " denormalised!\n";
#endif
		p3.r3Rot.Normalise();
		return p3;
	}

	//*****************************************************************************************
	//
	CPlacement3<> p3BoxElement(int i_elem) const
	//
	//	Returns:
	//		The placement of the box model in (i_index, i_elem) in world space.
	//
	//***************************
	{
		if (Rotated[i_elem])
		{
			return CPlacement3<>
			(
				mx3Convert(SuperOrient[i_elem]), 
				v3Convert(&SuperData[i_elem][3])
			) * 
			p3Convert(State, 0);
		}
		else
		{
			CPlacement3<> p3 = p3Convert(State, 0);
			p3.r3Rot.Normalise();
			p3.v3Pos += v3Convert(&SuperData[i_elem][3]) * p3.r3Rot;
			return p3;
		}
	}

	//*****************************************************************************************
	//
	CTransform3<> tf3BoxElement(int i_elem) const
	//
	//	Returns:
	//		The transform representing this sub-box's placement in world space.
	//
	//***************************
	{
		return CTransform3<>
		(
			mx3Convert(SuperOrient[i_elem]), 
			v3Convert(&SuperData[i_elem][3])
		) * CTransform3<>(p3BoxMain());
	}

	//*****************************************************************************************
	//
	inline TReal rVelocityExpand() const
	//
	// Returns:
	//		The amount to expand volumes due to velocity.
	//
	//**********************************
	{
		if (Info > 0 && Movable)
		{
			// Adjust for velocity. Linear only for now.
			// To do: add rotational velocity if needed.
			// To do: expand only in direction of velocity.
			float f_vel_sqr = Square(State[0][1]) + Square(State[1][1]) + Square(State[2][1]);
			if (!_finite(f_vel_sqr))
				// fSqrtEst doesn't work with infinites, so let's just be really safe.
				return 1e20f;
			return fSqrtEst(f_vel_sqr) * 2*MAX_TIMESTEP;
		}
		else
			return 0;
	}

	//*****************************************************************************************
	//
	void GetCollideBox
	(
		CBoundVolBox* pbvb, CTransform3<>* ptf3, 
		float f_expand
	) const
	//
	//**********************************
	{
		// Use box extents, and expand them.
		// Get box for entire superbox.

		*pbvb = CBoundVolBox
		(
			CVector3<>
			(
				(Extents[3] - Extents[0]) * 0.5f + f_expand,
				(Extents[4] - Extents[1]) * 0.5f + f_expand,
				(Extents[5] - Extents[2]) * 0.5f + f_expand
			),

			// Radius will expand by at most sqrt(3)*f_expand, so this makes it safe.
			Radius + 2.0f*f_expand
		);

		// Get main transform.
		*ptf3 = p3BoxMain();

		// Offset by box centre.
		CVector3<> v3_pos
		(
			(Extents[3] + Extents[0]) * 0.5f,
			(Extents[4] + Extents[1]) * 0.5f,
			(Extents[5] + Extents[2]) * 0.5f
		);
		ptf3->v3Pos = v3_pos * *ptf3;
	}

	//*****************************************************************************************
	//
	inline void GetCollideSphere
	(
		CBoundVolSphere* pbvs, CVector3<>* pv3, 
		float f_expand = 0
	) const
	//
	//**********************************
	{
		// Return sphere data.
		*pbvs = CBoundVolSphere(Radius + f_expand);
		*pv3 = v3Convert(State, 0);
	}

	//*****************************************************************************************
	//
	inline bool bUseBox() const
	//
	//**********************************
	{
		// Base it on both size and extent ratio.
		return Radius >= rMIN_RADIUS_BOX /*||
			   ExtentRatio >= rMIN_RATIO_BOX*/;
	}

	//*****************************************************************************************
	//
	static inline bool bUseBox
	(
		const CBoundVol& bv,
		TReal r_scale
	)
	//
	//**********************************
	{
		return true;
/*		const CBoundVolBox& bvb = *bv.pbvbCast();
		if (!&bvb)
			// Not a box.
			return false;
		return true;

		// Base it on both size and extent ratio.
		TReal r_radius = bvb.fMaxExtent();
		if (r_radius * r_scale >= rMIN_RADIUS_BOX)
			return true;

		// If the max radius is much larger than the min dimension, use the box.
		float f_min = Min(Min(bvb.v3GetMax().tX, bvb.v3GetMax().tY), bvb.v3GetMax().tZ);
		return r_radius >= f_min * rMIN_RATIO_BOX;
*/
	}

	//******************************************************************************************
	inline bool bIgnoreAllCollisions() const
	{
		if (PelvisElem == RIGHT_HAND)
		{
			// See whether the hand will ignore all collisions.
			Assert(PelvisModel >= 0);
			CPlayer* pplay = ptCast<CPlayer>(CPhysicsInfoSkeleton::paniSkeleton(PelvisModel));
			return !pplay->bHandInteract(0);
		}
		return false;
	}

	//******************************************************************************************
	bool bIgnoreCollisions(const CXob& xob, bool b_test_world = true) const
	{
		if (Movable || xob.Movable)
		{
			// Special behaviour for player's hand.
			if (PelvisElem == RIGHT_HAND)
			{
				// Ignore all collisions if hand is inactive.
				Assert(PelvisModel >= 0);
				CPlayer* pplay = ptCast<CPlayer>(CPhysicsInfoSkeleton::paniSkeleton(PelvisModel));
				if (!pplay->bHandInteract(xob.Instances[0]))
					return true;
			}
			else if (xob.PelvisElem == RIGHT_HAND)
			{
				// Ignore all collisions if hand is inactive.
				Assert(xob.PelvisModel >= 0);
				CPlayer* pplay = ptCast<CPlayer>(CPhysicsInfoSkeleton::paniSkeleton(xob.PelvisModel));
				if (!pplay->bHandInteract(Instances[0]))
					return true;
			}
			else if (PelvisModel >= 0 && PelvisModel == xob.PelvisModel)
			{
				// If not the hand, ignore boxes of same skeletal model.
				return true;
			}

			if (b_test_world)
				// Test any ignoring set up outside the physics system.
				return pphSystem->bIgnoreCollisions(Instances[0], xob.Instances[0]);

			return false;
		}
		else
			return true;
	}

	//******************************************************************************************
	inline bool bSharedQuery() const
	{
		// All boundary boxes except head and tail share a query region with element 0.
		return PelvisElem >= 0 && PelvisElem != HEAD_BC && PelvisElem != TAIL_BC;
	}

	//******************************************************************************************
	inline bool bOtherQuery() const
	{
		// All boundary boxes except head and tail share a query region with element 0.
		return PelvisElem > 0 && PelvisElem != HEAD_BC && PelvisElem != TAIL_BC;
	}

	//******************************************************************************************
	inline int iQueryRegion() const
	{
		int i_box = Index();

		if (bOtherQuery())
		{
			// For all central boxes, use the body box region.
			// This should not have its own region.
			Assert(!aqregNear[i_box].plspinsNear);
			Assert(!aqregNear[i_box].pwqttopTerrain);

			Assert(PelvisModel >= 0);
			int i_body = Pel_Box_BC[PelvisModel][0];

			// Make sure this guy's still in the picture.
			Assert(i_body >= 0);
			Assert(Xob[i_body].Instances[0] && Xob[i_body].Movable);

			return i_body;
		}

		// Use this box's own region.
		return i_box;
	}

	//******************************************************************************************
	inline CQueryRegion* pqregNear() const
	{
		return &aqregNear[iQueryRegion()];
	}

	//******************************************************************************************
	float fTrrScale() const
	{
		// Expand the terrain query volume for body boxes of non-humans,
		// to ensure that they encompass possible footfalls.
		float f_trr_scale = 1.0f;
		if (PelvisModel >= 0 && Pel_Usage[PelvisModel] != epelHUMAN	&& PelvisElem == 0)
			return 2.0f;
		else
			return 1.0f;
	}

	//******************************************************************************************
	void TestIntersect(const char* str_when) const
	{
#if VER_TEST
		CTimeBlock tmb(&psTestIntersect);

		CVector3<> v3 = v3Convert(State, 0);

		// Test against all other boxes. No movable box should intersect any other box.
		for (int i_other = 0; i_other < GUYS; i_other++)
		{
			const CJoeXob& xob = JoeXob(i_other);

			if (&xob != this && xob.Instances[0] && !bIgnoreCollisions(xob, true))
			{
				// First see if within radius; we don't want this to be TOO slow.
				CVector3<> v3_other = v3Convert(xob.State, 0);

				// Perform sphere-sphere test first.
				if ((v3 - v3_other).tLenSqr() > Sqr(Radius + xob.Radius))
					continue;

				// Test all elements of all boxes.
				for (int i_elem = 0; Instances[i_elem]; i_elem++)
				{
					// We now use physics extents for penetration testing...much more lenient than world extents.
					CBoundVolBox bvb = bvbBoundBox(i_elem);

					CTransform3<> tf3 = tf3BoxElement(i_elem);
					CTransform3<> tf3_inv = ~tf3;

					for (int i_elem_other = 0; xob.Instances[i_elem_other]; i_elem_other++)
					{
						CBoundVolBox bvb_other = xob.bvbBoundBox(i_elem_other);
						CTransform3<> tf3_other = xob.tf3BoxElement(i_elem_other);
						CTransform3<> tf3_total = tf3_other * tf3_inv;

						if (bvb.esfSideOf(bvb_other.tf3Box(tf3_total)) != esfOUTSIDE &&
							bvb_other.esfSideOf(bvb.tf3Box(~tf3_total)) != esfOUTSIDE)
						{
							CTimeBlock tmb(&psReportIntersect);

							dout <<"Penetrate! " <<str_when <<" "
								<<Instances[i_elem]->strGetInstanceName() <<" "
								<<xob.Instances[i_elem_other]->strGetInstanceName() <<"\n";
						}
					}
				}
			}
		}
#endif
	}

	//*****************************************************************************************
	void UpdateWDBase()
	{
		// Prevent recursion, e.g. from Deactivate.
		if (bUpdating)
			return;
		bUpdating = true;

		// Moved this frame.
		for (int i_element = 0; Instances[i_element]; ++i_element)
		{
			CInstance* pins = Instances[i_element];

			if (i_element == 0 || Instances[i_element-1] != pins)
			{
				if (Moved & 1)
				{
					// Not a compound secondary element.
					if (Moved == 1)
					{
						// Just awoke.
						CMessageMove(CMessageMove::etAWOKE, pins).Dispatch();
					}

					// Update the instance, which will move the object, and send a move message.
					pins->pphiGetPhysicsInfo()->UpdateWDBase(pins, Index(), i_element);
				}
				else if (Moved)
				{
					// Didn't move this frame, but has moved before.
					// Send an ACTIVE message for those who care.
					CMessageMove(CMessageMove::etACTIVE, pins).Dispatch();
				}
			}
		}

		if (Moved & 1)
			// This moved this frame. Reset per-frame flag, and set history flag.
			Moved = 2;

		// Perform extra check for BoxInstance in case it was deactivated from under us.
		if (Info <= iINFO_REMOVE && PelvisElem < 0 && Instances[0])
		{
			// Perform delayed deactivation for non-bio superbox here.
			// Bio superboxes are deactivated only by animal deactivation.
			if (SleepThreshold > 1.0f)
				dout <<"Finally deactivating " <<Instances[0]->strGetInstanceName()
					 <<" at " <<SleepThreshold <<" x normal sleep.\n";
			Deactivate(true);
		}

		// Unset flag for blocking other move commands.
		bUpdating = false;
	}

	//*****************************************************************************************
	void Deactivate(bool b_deactivate)
	{
		Assert(Instances[0]);
		Assert(!bPhysicsIntegrating);

		TestIntersect("Sleep");

		// Give objects final physics position before removing.
		CPhysicsInfoBox::UpdateWDBase(Index());

		conPhysics <<(b_deactivate? "Sleeping" : "Immobilising") <<" object " 
				   <<Index() <<" " <<Instances[0]->strGetInstanceName() <<"\n";

		// Send a sleep message, if the object ever moved.
		if (Moved)
		{
			for (int i_element = 0; Instances[i_element]; ++i_element)
			{
				CInstance* pins = Instances[i_element];
				if (i_element == 0 || Instances[i_element-1] != pins)
					CMessageMove(CMessageMove::etSLEPT, pins).Dispatch();
			}
		}

		Movable = false;
		Moved = 0;

		// Purge query region.
		aqregNear[Index()].Clear();

		if (b_deactivate)
		{
			// Update other objects' list of inactive objects.
			for (int j = 0; j < iMAX_PHYSICS_OBJECTS; j++)
				if (aqregNear[j].bActive())
					aqregNear[j].AddInstance(Instances[0]);

			// Remove from physics system.
			Info = 0;
	
			// Detachable pelvis.
			if (PelvisElem >= 0)
				Pel_Box_BC[ PelvisModel ][ PelvisElem ] = -1;
			PelvisModel = PelvisElem = -1;

			// Zero all the instance elements, to cancel any pending updates.
			memset(Instances, 0, sizeof(Instances));
		}
	}

	//*****************************************************************************************
	CBoundVolBox bvbBoundBox(int i_elem) const
	{
		Assert(i_elem >= 0);

		// Return bounds for individual element.
		return CBoundVolBox(CVector3<>
		(
			SuperData[i_elem][0],
			SuperData[i_elem][1],
			SuperData[i_elem][2]
		));
	}

	//*****************************************************************************************
	void DrawPhysicsBoxes(int i_elem, CDraw& draw, CCamera& cam, CSet<EDrawPhysics> setedf)
	{
#if bVER_BONES()
		CPlacement3<> p3 = p3BoxElement(i_elem);
		CTransform3<> tf3_screen = cam.tf3ToHomogeneousScreen();
		CTransform3<> tf3_shape_screen = p3 * tf3_screen;

		if (setedf[edfBOXES_PHYSICS])
		{
			if (bIgnoreAllCollisions() ||
				pphSystem->bIgnoreCollisions(Instances[i_elem], 0))
				draw.Colour(clrIGNORE);
			else if (Moved)
				draw.Colour(clrACTIVE);
			else
				draw.Colour(clrIMMOVABLE);

			// Use bvbBoundBox for the box extents, as that's what Physics uses.
			CBoundVolBox bvb = bvbBoundBox(i_elem);

			// Draw the box.
			draw.Box3D(bvb.tf3Box(tf3_shape_screen));
		}

		// For each superbox, draw CM and wake-up volumes.
		if (i_elem == 0)
		{
			if (setedf[edfBOXES])
			{
				// Draw a 3-cross at the CM.
				CPlacement3<> p3_super = p3BoxMain();
				tf3_shape_screen = p3_super * tf3_screen;

				draw.CoordinateFrame3D(tf3_shape_screen, 0.5f, 0.1f);
			}

			if (setedf[edfBOXES_WAKE])
			{
				// Draw activation volume, dark blue.
				draw.Colour(clrACTIVATE);

				// Draw activation volume.
				if (bUseBox())
				{
					// Draw a box.
					CBoundVolBox bvb;
					CTransform3<> tf3;

					GetCollideBox(&bvb, &tf3, rCOLLIDE_EXPAND + rVelocityExpand());
					CTransform3<> tf3_shape_screen = tf3 * tf3_screen;
					draw.Box3D(bvb.tf3Box(tf3_shape_screen));
				}
				else
				{
					// Draw a sphere.
					CBoundVolSphere bvs;
					CVector3<> v3;

					GetCollideSphere(&bvs, &v3, rCOLLIDE_EXPAND + rVelocityExpand());

					// Find radius offset in world space.
					CVector3<> v3_radius = CVector3<>(bvs.rRadius, 0, 0) * cam.pr3Presence().r3Rot;
					CVector2<> v2_cm = tf3_shape_screen.v3Pos.v2Project();
					CVector3<> v2_border = ((v3 + v3_radius) * tf3_screen).v2Project();

					// Get radius in camera space.
					draw.Circle(v2_cm.tX, v2_cm.tY, (v2_border - v2_cm).tLen());
				}
			}

			int i_index = Index();

			if (setedf[edfBOXES_QUERY] && aqregNear[i_index].bActive())
			{
				// It has a region.
				draw.Colour(clrQUERY);

				// Create transform for axis-aligned box.
				CVector3<> v3_ext = aqregNear[i_index].v3Max - aqregNear[i_index].v3Min;
				CTransform3<> tf3_box
				(
					CMatrix3<>(v3_ext.tX, 0, 0,  0, v3_ext.tY, 0,  0, 0, v3_ext.tZ),
					aqregNear[i_index].v3Min
				);

				draw.Box3D(tf3_box * tf3_screen);
			}

			if (setedf[edfSKELETONS])
			{
				// Draw interaction connections: one way only, so check only earlier boxes.
				draw.Colour(clrQUERY);

				for (int i = 0; i < i_index; i++)
				{
					if (Xob[i_index].Do_It[i])
					{
						// Draw line between centres.
						CVector3<> v3_pos = v3Convert(&Xob[i].State[0], 0);
						draw.Line3D(tf3_shape_screen.v3Pos, v3_pos * tf3_screen);
					}
				}
			}

			if (setedf[edfATTACHMENTS])
			{
				// First draw all edges, duller.
				CQueryRegion* pqreg = pqregNear();
				if (pqreg->pavpEdges.size())
				{
					draw.Colour(clrATTACH * 0.5f);

					forall (pqreg->pavpEdges, CPArray<TVectorPair>, ped)
					{
						draw.Line3D
						(
							v3Convert((*ped)[0]) * tf3_screen, 
							v3Convert((*ped)[1]) * tf3_screen
						);
					}

					// Draw terrain edges, if any.
					draw.Colour(clrATTACH);

					TVectorPair* apvp_edges[200];
					int i_count = iTerrainEdges(apvp_edges, 200);
					for (int i = 0; i < i_count; i++)
					{
						draw.Line3D
						(
							v3Convert((*apvp_edges[i])[0]) * tf3_screen, 
							v3Convert((*apvp_edges[i])[1]) * tf3_screen
						);
					}
				}
			}
		}
#endif
	}
};

//*****************************************************************************************
inline CJoeXob& JoeXob(int i_index)
{
	Assert(i_index >= 0);
	return static_cast<CJoeXob&>(Xob[i_index]);
}

#define JoeXobSelf		static_cast<CJoeXob&>(*this)

//**************************************************************************************
//
// CQueryRegion implementation.
//

	//**************************************************************************************
	void CQueryRegion::QueryTrrObjs(float f_scale)
	{
		// Construct an equivalent bounding box for this.
		CVector3<> v3_extent = (v3Max - v3Min) * (0.5f * f_scale);
		v3_extent.tZ = 1e-6f;

		CVector3<> v3_centre = (v3Min + v3Max) * 0.5f;
		v3_centre.tZ = 0.0f;

		CPartitionSpace parts = CBoundVolBox(v3_extent);
		parts.SetPos(v3_centre);

		// Find all intersecting terrain objects, sorted.
		CWDbQueryTerrainObj qto(&parts, true);

		//
		// Find highest encompassing texture.
		//
		ptoBase = 0;
		std::list<SPartitionListElement>::iterator it = qto.end();
		if (it != qto.begin())
		{
			do
			{
				--it;

				// See whether it entirely contains the partition.
				if ((*it).esfView == esfINTERSECT)
				{
					// It is not contained in the partition.
					if ((*it).ppart->esfSideOf(&parts) == esfINSIDE)
					{
						// This is the highest encompassing texture.
						ptoBase = static_cast<CTerrainObj*>((*it).ppart);
						++it;
						break;
					}
				}
			}
			while (it != qto.begin());
		}

		//
		// Store only objects above the base which have different materials.
		//

		// Make sure previous list erased.
		erase_all(lsptoTerrainObjs);

		for (; it != qto.end(); ++it)
		{
			CTerrainObj* pto = static_cast<CTerrainObj*>((*it).ppart);
			if (!ptoBase ||
				pto->matSoundMaterial != ptoBase->matSoundMaterial ||
				pto->fFriction != ptoBase->fFriction)
			{
				// It's different. Stick it on the front of the list.
				// This puts makes higher textures at the front, for efficient searching.
  				lsptoTerrainObjs.push_front(pto);
			}
		}
	}

	//**************************************************************************************
	void CQueryRegion::Query(const CVector3<>& v3_min, const CVector3<>& v3_max, float f_trr_scale)
	{
		CCycleTimer ctmr;

		v3Min = v3_min;
		v3Max = v3_max;

		// Construct an equivalent bounding box for this.
		CBoundVolBox bvb((v3Max - v3Min) * 0.5f);
		CPresence3<> pr3((v3Min + v3Max) * 0.5f);

		Assert(v3Max.tX - v3Min.tX <= rSANE_REGION_SIZE);
		Assert(v3Max.tY - v3Min.tY <= rSANE_REGION_SIZE);
		Assert(v3Max.tZ - v3Min.tZ <= rSANE_REGION_SIZE);

		plspinsNear = new std::list<CInstance*>;

		// Query nearby boxes. Add inactive ones to list.
		CWDbQueryPhysicsBoxFast wqph(bvb, pr3);
		foreach (wqph)
		{
			CInstance* pins_near = *wqph;
			if (pphSystem->iGetIndex(pins_near) < 0)
				// Also skip objects magneted as intangible.
				if (!NMagnetSystem::bIsIntangible(pins_near))
					plspinsNear->push_back(pins_near);
		}

		// Query contained terrain texture objects.
		QueryTrrObjs(f_trr_scale);

		psQuery.Add(ctmr(), 1);

		// Create terrain query region for this box as well.
		CTerrain* ptrr = CWDbQueryTerrain().tGet();
		if (ptrr)
		{
			// Expand the terrain query scale by the requested amount.
			pr3.rScale *= f_trr_scale;
			if (pwqttopTerrain)
				pwqttopTerrain->SetVolume(bvb, pr3);
			else
				pwqttopTerrain = new CWDbQueryTerrainTopology(bvb, pr3);

			pwqttopTerrain->Evaluate();

			psTQuery.Add(ctmr(), 1);
		}
		else
		{
			Assert(!pwqttopTerrain);
		}

		// Also find any water objects in this region.
		pwqwtrWater = new CWDbQueryWater(bvb, pr3);

		// Clear edges.
		pavpEdges = CPArray<TVectorPair>();
	}

	//**************************************************************************************
	void CQueryRegion::Query(int i_index)
	{
		Assert(i_index >= 0);
		CJoeXob& xob = JoeXob(i_index);

		if (!xob.Movable)
			return;

		// Find box's region of influence.
		TReal r_radius = xob.Radius + rCOLLIDE_EXPAND + rREGION_MIN_SLACK;
		CVector3<> v3_min
		(
			xob.State[0][0] - r_radius, xob.State[1][0] - r_radius, xob.State[2][0] - r_radius
		);
		CVector3<> v3_max
		(
			xob.State[0][0] + r_radius, xob.State[1][0] + r_radius, xob.State[2][0] + r_radius
		);

		if (xob.bSharedQuery())
		{
			// This is a central bio box. Merge all boxes of same creature into one region.
			Assert(xob.PelvisModel >= 0);

			// Find and merge regions of other bio boxes.
			for (int i_bio = 0; i_bio < iMAX_PHYSICS_OBJECTS; i_bio++)
			{
				CJoeXob& xob_bio = JoeXob(i_bio);
				if (i_bio != i_index && xob_bio.PelvisModel == xob.PelvisModel && xob_bio.bSharedQuery())
				{
					TReal r_radius = xob_bio.Radius + rCOLLIDE_EXPAND + rREGION_MIN_SLACK;

					SetMin(v3_min.tX, xob_bio.State[0][0] - r_radius);
					SetMin(v3_min.tY, xob_bio.State[1][0] - r_radius);
					SetMin(v3_min.tZ, xob_bio.State[2][0] - r_radius);

					SetMax(v3_max.tX, xob_bio.State[0][0] + r_radius);
					SetMax(v3_max.tY, xob_bio.State[1][0] + r_radius);
					SetMax(v3_max.tZ, xob_bio.State[2][0] + r_radius);
				}
			}
		}

		// Perform the query.
		Query(v3_min, v3_max, xob.fTrrScale());
	}

	//**************************************************************************************
	const CTerrainObj* CQueryRegion::ptoTerrainObject(float f_x, float f_y)
	{
		// Search in order for objects containing this point.
		CVector3<> v3(f_x, f_y, 0.0f);

		// The list contains all non-base significant textures to check,
		// with higher textures first.
		forall (lsptoTerrainObjs, std::list<CTerrainObj*>, itpto)
		{
			if ((*itpto)->pbvBoundingVol()->bContains(v3 / (*itpto)->pr3Presence()))
			{
				return *itpto;
			}
		}

		return ptoBase;
	}


	struct SVectorPair
	{
		TVectorPair vp;
	};

	//**************************************************************************************
	void CQueryRegion::GetEdges()
	{
		if (!pwqttopTerrain)
			return;

		// Iterate and store edges. Put them in a large temporary array at first.
		CMLArray(SVectorPair, mavp_edges, 300);

		for 
		(
			NMultiResolution::CQueryRect::CEdgeIterator eit = pwqttopTerrain->eitBegin(); 
			eit && mavp_edges.size() < mavp_edges.capacity();
			++eit
		)
		{
			if (eit.esfEdgeIntersects() & esfINSIDE)
			{
				// Allocate.
				mavp_edges.paAlloc(1);
				SVectorPair& vp = mavp_edges(-1);
				eit.GetEdge(v3Convert(vp.vp[0]), v3Convert(vp.vp[1]));
			}
		}

		// Allocate and copy array.
		CPArray<SVectorPair> pavp = mavp_edges.paDup();
		pavpEdges = CPArray<TVectorPair>(pavp.size(), reinterpret_cast<TVectorPair*>(pavp.atArray));
	}

	//**************************************************************************************
	void CQueryRegion::AddInstance(CInstance* pins)
	{
		Assert(bActive());

		// Add pins to query list if intersecting.
		// See if it's in the list.
		if (find(plspinsNear->begin(), plspinsNear->end(), pins) != plspinsNear->end())
			return;

		// Add to the list if it's in the region.
		// Construct an equivalent bounding box for this.
		CBoundVolBox bvb((v3Max - v3Min) * 0.5f);
		CPresence3<> pr3((v3Min + v3Max) * 0.5f);

		if (bvb.esfSideOf(*pins->pbvBoundingVol(), &pr3, &pins->pr3GetPresence()) & esfINSIDE)
		{
			plspinsNear->push_back(pins);
		}
	}

//**********************************************************************************************
//
//  CPhysicsInfoBox implementation
//


	// It could be in the class, but that would just slow down the compile times.
	typedef std::set<CPhysicsInfoBox, std::less<CPhysicsInfoBox> > TSPB;
	TSPB tspbPhysicsInfoBox;	// A set containing all shared box infos, for instancing.

	//*****************************************************************************************
	CPhysicsInfoBox::CPhysicsInfoBox() 
	{
		SPhysicsData phd;
		CBoundVolBox bvb(1, 1, 1);

		new (this) CPhysicsInfoBox(bvb, phd);
	}
	
	//*****************************************************************************************
	CPhysicsInfoBox::CPhysicsInfoBox
	(
		const rptr<CRenderType>&	prdt,				// The rendering shape.
		const CHandle&				h_obj,				// Handle to the object in the value table.
		const CGroffObjectName*		pgon,
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				// the loader.
	) : CPhysicsInfo(pgon, h_obj, pvtable, pload)
	{
		fDensity =		fDEFAULT_DENSITY;
		fFriction =		fDEFAULT_FRICTION;
		fElasticity =	fDEFAULT_ELASTICITY;

		float f_mass = 0.0f;

		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_obj)
		{
			bFILL_FLOAT(f_mass,     esMass);
			bFILL_FLOAT(fDensity,	esDensity);
			bFILL_FLOAT(fElasticity,esElasticity);
			bFILL_FLOAT(fFriction,	esFriction);

			bool b = false;
			bFILL_BOOL(b, esSmall);
			setFlags[epfSMALL] = b;
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		// Friction must be in the 0-10 range, inclusive.
		Assert(fFriction >= 0.0f);
		Assert(fFriction <= 10.0f);
/*
		// Slam density for immovable objects.
		if (!bIsMovable())
			fDensity = 10.0f;
*/

		// Use v3Max to construct bounding box, as it represents the actual mesh extents.
		// Do not use bvbBoundVol, as that is grown to encompass wrap.
		bvbBoundVol = CBoundVolBox(prdt->v3GetPhysicsBox());

		// Create the collide box, allowing physics to resize if needed.
		CVector3<> v3_corner = bvbBoundVol.v3GetMax() * pgon->fScale;
		if (bIsTangible())
			if (Min(Min(v3_corner.tX, v3_corner.tY), v3_corner.tZ) <= rBOX_DIM_WARN)
				dout <<"Warning: Physics box " <<pgon->strObjectName <<" is smaller than " <<(rBOX_DIM_WARN*2.0f) <<" m: "
					 <<(v3_corner*2.0f) << std::endl;

		CXob::XOBResize(afConvert(v3_corner));
		bvbCollideVol = CBoundVolBox(v3_corner / pgon->fScale);

		// Copy pivot from mesh as well.
		v3Pivot = prdt->v3GetPivot();

		if (f_mass > 0.0f)
			// This determines density, overriding any previous value.
			fDensity = f_mass / bvbBoundVol.rGetVolume(pgon->fScale) * 0.001f;
	}

	//*****************************************************************************************
	CPhysicsInfoBox::CPhysicsInfoBox
	(
		const CBoundVolBox &bvb,
		const SPhysicsData& phd,
		float f_mass_override,
		float f_scale,
		const CVector3<>& v3_pivot
	) : 
		CPhysicsInfo(phd),
		fDensity(phd.fDensity), fFriction(phd.fFriction), fElasticity(phd.fElasticity), v3Pivot(v3_pivot)
	{
		// Friction must be in the 0-10 range, inclusive.
		Assert(fFriction >= 0.0f);
		Assert(fFriction <= 10.0f);
/*
		// Slam density for immovable objects.
		if (!bIsMovable())
			fDensity = 10.0f;
*/
		bvbBoundVol = bvb;

		// Create the collide box, allowing physics to resize if needed.
		CVector3<> v3_corner = bvb[0] * f_scale;
		CXob::XOBResize(afConvert(v3_corner));
		bvbCollideVol = CBoundVolBox(v3_corner / f_scale);

		if (f_mass_override > 0.0f)
		{
			// Set density from this.
			fDensity = f_mass_override / (bvbBoundVol.rGetVolume(f_scale)) * 0.001f;
		}
	}

	//*****************************************************************************************
	bool CPhysicsInfoBox::operator< 
	(
		const CPhysicsInfoBox& pib
	) const
	{
		return memcmp(this, &pib, sizeof pib) < 0;
	};

	
	//*****************************************************************************************
	CPhysicsInfo* CPhysicsInfoBox::pphiCopy()
	{
		CPhysicsInfoBox* pphib_ret = new CPhysicsInfoBox(*this);
		pphib_ret->setFlags[epfHACK_UNIQUE] = true;
		return pphib_ret;
	}

	//*****************************************************************************************
	const CPhysicsInfoBox* CPhysicsInfoBox::pphibFindShared
	(
		const CPhysicsInfoBox& phib
	)
	{
		CPhysicsInfoBox phib_copy = phib;

		// It's not unique, because we're about to stick it in the instancer.
		phib_copy.setFlags[epfHACK_UNIQUE] = false;
		phib_copy.setFlags[epfMANAGED] = true;

		// Blow off this optimisation. It saves only a few boxes, and gets in the way
		// of some things like the intangible hand box fix.
/*
		if (!phib_copy.bIsTangible())
		{
			// If it isn't tangible, make a standard box so that we can instance better.
			phib_copy.bvbCollideVol = phib_copy.bvbBoundVol = CBoundVolBox(CVector3<>(0.1f, 0.1f, 0.1f));
		}
*/
		// Insert or find, please.
		std::pair<TSPB::iterator, bool> p = tspbPhysicsInfoBox.insert(phib_copy);

		// If we found a duplicate, it will do.
		// If we inserted a new one, the new one will do.
		return &(*p.first);
	}

	//*****************************************************************************************
	void CPhysicsInfoBox::ZeroRefs()
	{
		if (setFlags[epfMANAGED])
		{
			// Then we're in the instancer!
			// We could remove "this" from tspbPhysicsInfoBox, but we won't bother for now.
		}
		else
		{
			// Someone allocates "this" with "new"
			delete this;
		}
	}
	
	//*****************************************************************************************
	TReal CPhysicsInfoBox::fVolume(CInstance* pins) const
	{
		// Return the box's volume, scaled by the instance.
		return bvbBoundVol.rGetVolume(pins->fGetScale());
	}

	//*****************************************************************************************
	TReal CPhysicsInfoBox::fMass(const CInstance* pins) const
	{
		// Get the partition's box volume, multiply by its density (converted from cgs to mks).
		float f_density = NMagnetSystem::bIsFrozen(pins) ? 10.0f : fDensity;
		return bvbBoundVol.rGetVolume(pins->fGetScale()) * f_density * 1000.0f;
	}

	//*****************************************************************************************
	void CPhysicsInfoBox::Activate
	(
		CInstance* pins, bool b_just_update, const CPlacement3<>& p3_vel
	) const
	{
		CTimeBlock tmb(&psActivate);

		static bool bFailed = false;				// Set to warn about overflow first time only.

		Assert(pphSystem);

		// Only wake up tangible objects!
		if (!bIsTangible())
			return;

		bool b_floats = bFloats();

		// Check whether it's already there.
		CVector2<int> v2i = pphSystem->v2iGetIndices(pins);

		int i_index;

		if (v2i.tX >= 0)
		{
			CJoeXob& xob = JoeXob(v2i.tX);

			// Object already active.
			if (b_just_update)
			{
				if (xob.bUpdating)
					// This comes from physics move command, so needn't update physics.
					return;

				AlwaysAssert(!bPhysicsIntegrating);

				// Re-use same slot, but update position, etc.
				i_index = v2i.tX;

				// Start it from scratch.
				memset(xob.Instances, 0, sizeof(xob.Instances));
				xob.Movable = 0;
				xob.Moved = 0;

				// Purge query regions.
				CQueryRegion* pqreg = xob.pqregNear();
				pqreg->Clear();
			}
			else
			{
				// Make sure integration is on, and activity is full.
				SetMax(xob.Info, INFO_MAX);
				xob.iLastTouched = iPhysFrame;
				xob.SleepThreshold = 1.0f;
				return;
			}
		}
		else
		{
			if (b_just_update)
				return;

			AlwaysAssert(!bPhysicsIntegrating);

			// Find a slot.  
			for (i_index = 0; i_index < iMAX_PHYSICS_OBJECTS; i_index++)
			{
				if (Xob[i_index].Instances[0] == 0)
					break;
			}
			if (i_index >= iMAX_PHYSICS_OBJECTS)
			{
				// Sorry, no room.
				if (!bFailed)
				{
					dout <<"Too many boxes! Cannot wake up " <<pins->strGetInstanceName() << std::endl;
					bFailed = true;
				}
				return;
			}
		}

		if (!b_just_update)
			bFailed = false;

		//
		// Add object to integrator.
		//

		CJoeXob& xob = JoeXob(i_index);

		// Get attachment list.
		std::list<CMagnetPair*> lspmp;
		NMagnetSystem::GetAttachedMagnets(pins, &lspmp);

		// Get master of this group.
		pins = (*lspmp.begin())->pinsSlave;
		Assert(pins);

		// Set initial state.
		float aaf_init[7][3];
		CInstance* apins[iMAX_BOX_ELEMENTS];				// Instances.
		float aaf_extents[iMAX_BOX_ELEMENTS][6];			// Offsets and sizes.
		float aaaf_orient[iMAX_BOX_ELEMENTS][3][3];			// Orientation matrices.
		float af_mass[iMAX_BOX_ELEMENTS];					// Masses.
		float af_friction[iMAX_BOX_ELEMENTS];				// Frictions.
		int ai_sound[iMAX_BOX_ELEMENTS];					// Sounds.
		float aaf_magnet_pos[iMAX_BOX_ELEMENTS][3];			// Magnet positions.
		float af_breaking[iMAX_BOX_ELEMENTS];				// Magnet strengths.
		bool b_movable = true;
		bool  ab_live_dof[6] = 
			{true, true, true, true, true, true};			// Which degrees (linear, rot) are free to move.
		float f_hinge_restore = 0,							// Additional hinge params.
			  f_hinge_friction = 0,
			  f_hinge_drive = 0;
		bool b_hinge = false;								// Whether it's hinged.
		bool b_hinge_limits = false;						// Whether hinge has limits.
		float af_hinge_xzero[3],							// Zero-point and limits of X axis in world space.
			  af_hinge_xmin[3],
			  af_hinge_xmax[3];
		int i_pelvis = -1, i_pelvis_elem = -1;

		conPhysics << "Waking object " << i_index << " " << pins->strGetInstanceName() << "\n";

		// Set all to unbreakable by default.
		int i;
		for (i = 0; i < iMAX_BOX_ELEMENTS; i++)
			af_breaking[i] = FLT_MAX;

		// World placement of element 0.
		CPlacement3<> p3_0 = pins->pphiGetPhysicsInfo()->p3Base(pins);
		Assert(p3_0.r3Rot.bIsNormalised());

		// Find any bio-linkage (for first element).
		CBoundaryBox* pbb = ptCast<CBoundaryBox>(pins);
		if (pbb)
		{
			Assert(pbb->paniAnimate);
			i_pelvis = pphSystem->iGetSkeletonIndex(pbb->paniAnimate);
			if (i_pelvis < 0)
			{
				// Animal is inactive. Activate it.
				pbb->paniAnimate->PhysicsActivate();
				i_pelvis = pphSystem->iGetSkeletonIndex(pbb->paniAnimate);
				if (i_pelvis >= 0)
				{
					if (xob.bSharedQuery())
					{
						// Create the animal's shared query region.
						int i_query = xob.iQueryRegion();
						aqregNear[i_query].Query(i_query);
					}
				}

				// Now entire animal, and its boxes have been activated.
				return;
			}

			if (i_pelvis >= 0)
			{
				// HACK. Detect obsolete Anne head box.
				if (Pel_Usage[i_pelvis] == epelHUMAN && pbb->ebbElement == ebbHEAD)
					return;

				i_pelvis_elem = pbb->iPhysicsElem();

				// Adjust base placement for bio-offset.
				// Get the local offset for this bio-box.
				CVector3<> v3_offset = pbb->v3GetOffset();

				// Set base placement to refer back to actual joint.
				p3_0.v3Pos -= v3_offset * p3_0.r3Rot;
			}
			else
				// Somehow it didn't activate. Just make the box immovable.
				b_movable = false;
		}

		// Make a pass through the list to see what kind of magnets we have.
		if (b_movable)
		{
			forall (lspmp, std::list<CMagnetPair*>, itpmp)
			{
				const CMagnetPair& mp = **itpmp;
				CSet<EMagnetFlag> set = mp.setemfFlags();

				if (set[emfINTANGIBLE])
					// Special magnet makes slave intangible.
					return;

				if (!mp.pinsSlave)
				{
					if (set[emfFREEZE][emfHINGE][emfSLIDE])
					{
						// Only interested in world magnets.

						// Set breaking strength.
						af_breaking[0] = set[emfBREAKABLE] ? mp.pmagData->fBreakStrength : FLT_MAX;

						// Set master-relative position on element 0.  Magnet orientation ignored.
						Copy(aaf_magnet_pos[0], mp.v3Pos() / p3_0);

						if (set[emfFREEZE])
						{
							b_movable = false;

							// Once we hit a frozen magnet, we aren't interested in any others.
							break;
						}

						// An element is magneted to the world.
						// Change the degrees of freedom.
						ab_live_dof[0] = set[emfXTFREE];
						ab_live_dof[1] = set[emfYTFREE];
						ab_live_dof[2] = set[emfZTFREE];
						ab_live_dof[3] = set[emfXFREE];
						ab_live_dof[4] = set[emfYFREE];
						ab_live_dof[5] = set[emfZFREE];

						if (set[emfHINGE])
						{
							b_hinge = true;

							// Set base placement to this element's rotation,
							// so hinge axis is correct.
							p3_0.r3Rot = mp.pinsMaster->pphiGetPhysicsInfo()->p3Base(mp.pinsMaster).r3Rot;

							// Set master-relative position on element 0.  Magnet orientation ignored.
							Copy(aaf_magnet_pos[0], mp.v3Pos() / p3_0);

							f_hinge_drive = mp.pmagData->fDrive;
							f_hinge_restore = mp.pmagData->fRestore;
							f_hinge_friction = mp.pmagData->fFriction;
							b_hinge_limits = set[emfLIMITS];

							if (b_hinge_limits)
							{
								//
								// Calculate hinge limits relative to object's current position.
								// These are in the form of X axis world directions.
								//

								Copy(af_hinge_xzero, mp.d3Zero());
								Copy(af_hinge_xmin, mp.d3Min());
								Copy(af_hinge_xmax, mp.d3Max());
							}
						}
					}
				}
				else
				{
					// Check each instance for mobility.
					if (!mp.pinsSlave->pphiGetPhysicsInfo()->bIsMovable())
					{
						b_movable = false;
						break;
					}
				}
			}
		}

		// Do all the attached boxes.
		int i_elems = 0;

		// Fill the a2pinsActiveBoxes array with all attached box instances.
		forall (lspmp, std::list<CMagnetPair*>, itpmp)
		{
			const CMagnetPair& mp = **itpmp;
			CInstance* pins_sub = mp.pinsSlave;
			if (!pins_sub)
				continue;

			// Make sure slave is not currently active.
			Assert(pphSystem->iGetIndex(pins_sub) < 0);

			const CPhysicsInfo* pphi = pins_sub->pphiGetPhysicsInfo();
			Assert(pphi);

			pphi->SetupSuperBox(p3_0,
								pins_sub->pr3GetPresence(),
								(*itpmp),
								&i_elems,
								apins,
								aaf_extents,
								aaaf_orient,
								af_mass,
								af_friction,
								ai_sound,
								aaf_magnet_pos,
								af_breaking);
		}

		float aaaf_null_orient[1][3][3] = {{{1,0,0}, {0,1,0}, {0,0,1}}};

		if (!b_hinge)
		{
			// Set orient[0] to exactly unity.
			Assert(Fuzzy(aaaf_orient[0][0][0]) == 1.0f);
			Assert(Fuzzy(aaaf_orient[0][1][1]) == 1.0f);
			Assert(Fuzzy(aaaf_orient[0][2][2]) == 1.0f);

			aaaf_orient[0][0][0] = aaaf_orient[0][1][1] = aaaf_orient[0][2][2] = 1.0f;
			aaaf_orient[0][0][1] = aaaf_orient[0][0][2] = 0.0f;
			aaaf_orient[0][1][0] = aaaf_orient[0][1][2] = 0.0f;
			aaaf_orient[0][2][0] = aaaf_orient[0][2][1] = 0.0f;
		}

		if (!b_movable)
		{
			for (int i = 0; i < 6; i++)
				ab_live_dof[i] = false;
		}

		// Set initial state.
		Copy(aaf_init, p3_0);

		// World velocity.
		Copy(aaf_init, p3_vel, 1);

		// Acceleration is 0.
		for (i = 0; i < 7; i++)
			aaf_init[i][2] = 0;

		xob.Create(	   i_elems,
					   aaf_magnet_pos,
					   af_breaking,
					   aaf_extents,
					   aaaf_orient,
					   af_mass,
					   af_friction,
					   ai_sound,
					   b_movable,
					   b_floats,
					   ab_live_dof,
					   f_hinge_restore,
					   f_hinge_friction,
					   f_hinge_drive,
					   b_hinge_limits, af_hinge_xzero, af_hinge_xmin, af_hinge_xmax,
					   aaf_init,
					   i_pelvis, i_pelvis_elem);

		xob.iActivatedFrame = iPhysFrame;
		xob.iLastTouched = iPhysFrame;
		xob.fVelocityExpand = 0;

		// Clear all the instances, then fill them.
		memset(xob.Instances, 0, sizeof(xob.Instances));
		for (i = 0; i < i_elems; i++)
			xob.Instances[i] = apins[i];

		// Initially set Intersected flag on all boxes, so ignoring won't turn off.
		memset(xob.Intersected, -1, sizeof(xob.Intersected));

		// Remove from other objects' list of inactive objects.
		for (int j = 0; j < iMAX_PHYSICS_OBJECTS; j++)
			if (aqregNear[j].bActive())
			{
				for (int k = 0; xob.Instances[k]; k++)
					aqregNear[j].RemoveInstance(xob.Instances[k]);
			}

		if (!xob.bSharedQuery())
			// Create this box's unique query region.
			// A shared query region will be created above, when the animal is activated.
			aqregNear[i_index].Query(i_index);

		// Test newly activated objects for intersection.
		// Skip if it's a biobox, or the result of a world Move.
		if (!b_just_update)
			xob.TestIntersect("Wake");
	}

	//******************************************************************************************
	int CPhysicsInfoBox::bIntersect(int i_me, int i_you)
	{
		Assert(i_me >= 0 && i_you >= 0);
		return Xob[i_me].Intersected[i_you];
	}

	//******************************************************************************************
	void CPhysicsInfoBox::SetupSuperBox
	(
		const CPresence3<>& pr3_master,
		const CPresence3<>& pr3_slave,							// presence of element, in world coords
		CMagnetPair* pmp,
		int* pi_elem,
		CInstance* apins[],										// Instances.
		float aaf_extents[iMAX_BOX_ELEMENTS][6],				// Offsets and sizes.
		float aaaf_orient[iMAX_BOX_ELEMENTS][3][3],				// Orientation matrices.
		float af_mass[iMAX_BOX_ELEMENTS],						// Masses.
		float af_friction[iMAX_BOX_ELEMENTS],					// Frictions.
		int ai_sound[iMAX_BOX_ELEMENTS],						// Sounds.
		float aaf_magnet_pos[iMAX_BOX_ELEMENTS][3],				// Magnet positions.
		float af_breaking[iMAX_BOX_ELEMENTS]					// Magnet strengths.
	) const
	{
		// Make sure slave is not currently active.
		CInstance* pins_sub = pmp->pinsSlave;

		// Put the box in its superbox slot.
		apins[*pi_elem] = pins_sub;

		TReal r_scale = pr3_slave.rScale;

		// Set box extents.
		CVector3<> v3_extent = bvbCollideVol[0] * r_scale;

		Copy(&aaf_extents[*pi_elem][0], v3_extent);

		// Get the placement of the slave relative to the master.
		CPlacement3<> p3_rel = pr3_slave.p3Placement() / pr3_master.p3Placement();

		// Set box offset from master.  
		Copy(&aaf_extents[*pi_elem][3], p3_rel.v3Pos);

		// Set box orientation.
		p3_rel.r3Rot.Normalise(true);
		CMatrix3<> mx3_rel = p3_rel.r3Rot;
		Copy(aaaf_orient[*pi_elem], mx3_rel);

		// Set mass from volume and density.
		af_mass[*pi_elem] = fMass(pins_sub);

		af_friction[*pi_elem] = fFriction;
		ai_sound[*pi_elem] = tmatSoundMaterial;

		// Set magnet position, relative element 0.  Ignore its orientation.
		if (!pmp->pmagData)
		{
			// No magnet, OR not magneted to another object,
			// OR  submodel of a compound physics model which cannot break.
			// Set breakage to max to avoid trouble.
			Assert(*pi_elem == 0 || pins_sub->pphiGetPhysicsInfo()->pphicCast());
			//Copy(aaf_magnet_pos[*pi_elem], CVector3<>(0, 0, 0));
			//af_breaking[*pi_elem] = FLT_MAX;
		}
		else
		{
			// Set master-relative position.
			Copy(aaf_magnet_pos[*pi_elem], pmp->v3Pos() / pr3_master.p3Placement());

			// Is this magnet breakable?
			if (pmp->pmagData->setFlags[emfBREAKABLE])
			{
				// Yes!  Set breaking strength.
				af_breaking[*pi_elem] = pmp->pmagData->fBreakStrength;
			}
			else
			{
				// No!  Set breaking strength to max.
				af_breaking[*pi_elem] = FLT_MAX;
			}
		}

		// Use the next element next time.
		++(*pi_elem);
	}

	//*****************************************************************************************
	void CPhysicsInfoBox::Deactivate(CInstance* pins) const
	{
		// Find object in system.  Deactivate all elements.
		int i = pphSystem->iGetIndex(pins);
		if (i >= 0)
			JoeXob(i).Deactivate(true);
	}

	//*****************************************************************************************
	bool CPhysicsInfoBox::bIsActive(const CInstance* pins) const
	{
		return pphSystem->iGetIndex(pins) >= 0;
	}

	//*****************************************************************************************
	bool CPhysicsInfoBox::bIsMoving(const CInstance* pins) const
	{
		// Return true only if the object is active, and has moved.
		int i_index = pphSystem->iGetIndex(pins);
		return i_index >= 0 && Xob[i_index].Moved;
	}

	//*****************************************************************************************
	CPlacement3<> CPhysicsInfoBox::p3GetVelocity(const CInstance* pins) const
	{
		int i_index = pphSystem->iGetIndex(pins);
		if (i_index >= 0)
		{
			// Get the velocity from State; it's the same for all sub-elements.
			return p3Convert(Xob[i_index].State, 1);
		}
		else
			return p3Null;
	}

	//*****************************************************************************************
	void CPhysicsInfoBox::UpdateWDBase(int i_index)
	{
		Assert(i_index >= 0);
		JoeXob(i_index).UpdateWDBase();
	}

	//*****************************************************************************************
	void CPhysicsInfoBox::UpdateWDBase
	(
		CInstance*	pins,		// The instance to update
		int			i_index,	// The index of that instance in the appropriate physics
								// system array.
		int			i_element
	) const
	{
		// Check whether this group as a whole is movable (may be staticly magneted to world).
		Assert(i_index >= 0);
		pins->Move(JoeXob(i_index).p3BoxElement(i_element), pphSystem);
	}

	//**********************************************************************************************
	void CPhysicsInfoBox::ApplyImpulse(CInstance* pins, int, const CVector3<>& v3_pos, const CVector3<>& v3_impulse) const
	{
		// Activate the thing.
		Activate(pins);
		int i_index = pphSystem->iGetIndex(pins);
		Assert(i_index >= 0);

		// Copy the vectors, as they're modified.
		float af_pos[3], af_impulse[3];

		Copy(af_pos, v3_pos);
		Copy(af_impulse, v3_impulse);

		// Call the raw physics function.
		Xob[i_index].ApplyImpulse(af_pos, af_impulse);
	}

	//*****************************************************************************************
	void CPhysicsInfoBox::ForceVelocity(CInstance* pins_target, const CVector3<>& v3_new_velocity) const
	{
		Assert(pins_target);

		// Activate the thing.
		Activate(pins_target);

		// Find the thing in the physics arrays.
		int i_index = pphSystem->iGetIndex(pins_target);
		Assert(i_index >= 0);

		// Hack the state.
		Xob[i_index].State[0][1] = v3_new_velocity.tX;
		Xob[i_index].State[1][1] = v3_new_velocity.tY;
		Xob[i_index].State[2][1] = v3_new_velocity.tZ;
	}


	//*****************************************************************************************
	void CPhysicsInfoBox::UpdateQueries()
	{
		CCycleTimer ctmr;

		for (int i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
		{
			if (aqregNear[i].pwqttopTerrain)
			{
				// Juice this region so it stays updated.
				aqregNear[i].pwqttopTerrain->Evaluate();
				psEvaluate.Add(ctmr(), 1);
			}
		}
	}

	//*****************************************************************************************
	void CPhysicsInfoBox::ActivateIntersecting()
	{
		//
		// Wake up objects near those that are actually moving.
		//

		CCycleTimer ctmr, ctmr2;

		psIntegrate.Add(0, 1);

		int i;
		for (i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
		{
			CJoeXob& xob = JoeXob(i);

			// Set all LastTouched records of bio boxes to current frame.
			if (xob.PelvisModel >= 0)
			{
				//
				// Provide life-support to bio-boxes if the animal is alive, preventing them
				// from ever sleeping.
				//
				CAnimate* pani = CPhysicsInfoSkeleton::paniSkeleton(xob.PelvisModel);
				Assert(pani);
				if (!pani->bReallyDead())
				{
					// Keep boxes fully active.
					xob.Info = INFO_MAX;
					xob.iLastTouched = iPhysFrame;
					xob.SleepThreshold = 1.0f;
				}
				else
				{
					// Keep boxes a little bit active, so they are still processed,
					// but mark them as wanting to sleep.
					SetMax(xob.Info, INFO_MAX-1);
				}
			}
			else if (xob.Instances[0] && xob.Data[14])
			{
				// Test for driven magneted object (Data[14] = Drive).
				// Prevent sleep on this entirely.
				xob.Info = INFO_MAX;
				xob.SleepThreshold = 0.0f;
			}

			xob.fVelocityExpand = xob.rVelocityExpand();

			memset(xob.Do_It, 0, sizeof(xob.Do_It));
			memset(xob.Intersected, 0, sizeof(xob.Intersected));
		}

		CBoundVolBox	abvb_boxes[iMAX_PHYSICS_OBJECTS];
		CTransform3<>	atf3_boxes[iMAX_PHYSICS_OBJECTS];
		bool			ab_boxes[iMAX_PHYSICS_OBJECTS];

		memset(ab_boxes, 0, sizeof(ab_boxes));

		//
		// Procedure for constructing and maintaining query regions and objects:
		//
		// Every active movable object has a query region.
		// For animals, query regions for all boxes are combined into one.
		//
		// Each frame:
		//		For each moving object:
		//			Determine whether it is still inside its region.
		//			If not, re-create the region, with extra slack, and re-query it for objects.
		//
		//			For each object in the region:
		//				If asleep, perform a proximity test, and if near enough, wake.
		//				If sleepy and moving object is more active, do the same.
		// 

		//
		// Check and update each object's query region.
		//
		for (i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
		{
			CJoeXob& xob = JoeXob(i);

			// Only fully awake objects can awaken slept objects.
			if (xob.Movable && (xob.Info == INFO_MAX || xob.PelvisElem >= 0))
			{
				if (xob.bOtherQuery())
				{
					// Not interested in central body boxes. They are handled
					// by being merged into body-box region.
					Assert(xob.PelvisModel >= 0);
					continue;
				}

				// Find box's region of influence.
				TReal r_vel_expand = xob.fVelocityExpand;
				TReal r_radius = xob.Radius + rCOLLIDE_EXPAND + r_vel_expand;

				CVector3<> v3_min
				(
					xob.State[0][0] - r_radius, xob.State[1][0] - r_radius, xob.State[2][0] - r_radius
				);
				CVector3<> v3_max
				(
					xob.State[0][0] + r_radius, xob.State[1][0] + r_radius, xob.State[2][0] + r_radius
				);

				if (xob.PelvisElem == 0)
				{
					// This is a bio body box. Merge all central boxes of same creature into one region.
					Assert(xob.PelvisModel >= 0);

					// Find and merge regions of other bio boxes.
					for (int i_bio = 0; i_bio < iMAX_PHYSICS_OBJECTS; i_bio++)
					{
						CJoeXob& xob_bio = JoeXob(i_bio);

						if (i_bio != i && xob_bio.PelvisModel == xob.PelvisModel && xob_bio.bSharedQuery())
						{
							TReal r_radius = xob_bio.Radius + rCOLLIDE_EXPAND + xob_bio.fVelocityExpand;

							SetMin(v3_min.tX, xob_bio.State[0][0] - r_radius);
							SetMin(v3_min.tY, xob_bio.State[1][0] - r_radius);
							SetMin(v3_min.tZ, xob_bio.State[2][0] - r_radius);

							SetMax(v3_max.tX, xob_bio.State[0][0] + r_radius);
							SetMax(v3_max.tY, xob_bio.State[1][0] + r_radius);
							SetMax(v3_max.tZ, xob_bio.State[2][0] + r_radius);

							SetMax(r_vel_expand, xob_bio.fVelocityExpand);
						}
					}
				}

				CQueryRegion* pqreg = &aqregNear[i];

				if (pqreg->bActive())
				{
					// Currently active region. Check against limits.
					if (v3_min.tX < pqreg->v3Min.tX || v3_max.tX > pqreg->v3Max.tX ||
						v3_min.tY < pqreg->v3Min.tY || v3_max.tY > pqreg->v3Max.tY ||
						v3_min.tZ < pqreg->v3Min.tZ || v3_max.tZ > pqreg->v3Max.tZ)
					{
						// No longer inside this region. 
						// Destroy box query, but not terrain query.
						pqreg->plspinsNear = 0;
					}
				}

				psRegion.Add(ctmr2(), 1);

				if (!pqreg->bActive())
				{
					// Must [re]create the query region.
					// Add some slack.
					TReal r_slack = rREGION_MIN_SLACK + rREGION_VELOCITY_SLACK * r_vel_expand;
					v3_min.tX -= r_slack;
					v3_min.tY -= r_slack;
					v3_min.tZ -= r_slack;

					v3_max.tX += r_slack;
					v3_max.tY += r_slack;
					v3_max.tZ += r_slack;

					//
					// Now is a great time to check whether the object is blowing up.
					// We can simply check the size of the query region, as that grows
					// linearly with velocity, and will also catch animals flying apart.
					//
					if (r_vel_expand >= rSANE_VELOCITY*2*MAX_TIMESTEP ||
						v3_max.tX - v3_min.tX >= rSANE_REGION_SIZE ||
						v3_max.tY - v3_min.tY >= rSANE_REGION_SIZE ||
						v3_max.tZ - v3_min.tZ >= rSANE_REGION_SIZE)
					{
						CInstance* pins = xob.PelvisModel < 0 ? 
							xob.Instances[0] :
							CPhysicsInfoSkeleton::paniSkeleton(xob.PelvisModel);

						AlwaysAssert(pins);
						xob.fVelocityExpand = 0;

						xob.TestIntersect("Discipline");

						if (iCornfield == 2)
						{
							dout <<"WARNING! Object " <<pins->strGetInstanceName()
								 <<" has been put in the cornfield:\n";
							dout <<" Velocity = " <<(r_vel_expand / (2*MAX_TIMESTEP))
								<<", query size = " <<(v3_max - v3_min) <<".\n";

							// Remove object object from game.
							pins->PhysicsDeactivate();
							pins->Move(v3CORNFIELD);
							continue;
						}
						else if (iCornfield == 1)
						{
							dout <<"WARNING! Object " <<pins->strGetInstanceName()
								 <<" has been reprimanded and has promised to behave:\n";
							dout <<" Velocity = " <<(r_vel_expand / (2*MAX_TIMESTEP))
								<<", query size = " <<(v3_max - v3_min) <<".\n";

							// Reset the object.
							if (xob.PelvisModel >= 0)
							{
								// Deactivate the animal, in expectation that AI will shortly reactivate it.
								// If player, it will stay inactive.
								pins->PhysicsDeactivate();
								continue;
							}
							else
							{
								// Simply zero the velocity and accel, leaving it active.
								for (int j = 0; j < 7; j++)
									xob.State[j][1] = xob.State[j][2] = 0;

								// Create the query region anew.
								TReal r_adjust = rREGION_VELOCITY_SLACK * r_vel_expand;
								v3_min.tX += r_adjust;
								v3_min.tY += r_adjust;
								v3_min.tZ += r_adjust;

								v3_max.tX -= r_adjust;
								v3_max.tY -= r_adjust;
								v3_max.tZ -= r_adjust;
							}
						}
						else
						{
							// Simply reduce the query region size.
							for (EAxis ea = eX; ea <= eZ; ea = EAxis(ea+1))
							{
								v3_max[ea] = (v3_max[ea] + v3_min[ea] + rSANE_REGION_SIZE) * 0.5f;
								v3_min[ea] = (v3_max[ea] + v3_min[ea] - rSANE_REGION_SIZE) * 0.5f;
							}
						}
					}

					// Re-create the region.
					pqreg->Query(v3_min, v3_max, xob.fTrrScale());
				}

				ctmr2.Reset();
			} // if movable and originally awake
		} // for all objects

		//
		// Check for new objects to awaken in each region.
		//

		// Store objects' original activity.
		int ai_orig_info[iMAX_PHYSICS_OBJECTS];
		for (i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
			ai_orig_info[i] = Xob[i].Info;

		// Also count movable objects.
		int i_movable = 0;
		for (i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
		{
			CJoeXob& xob = JoeXob(i);

			// Only originally fully awake objects can awaken slept objects.
			if (xob.Movable)
			{
				i_movable++;
				if (ai_orig_info[i] < INFO_MAX)
					continue;

				// If a bio-box is ignoring everything, continue.
				if (xob.bIgnoreAllCollisions())
					continue;
				
				CQueryRegion* pqreg = xob.pqregNear();

				//
				// Test nearby objects for wake-up.
				//

				// Expand wake-up volume by 1.75 * our threshold (once for moving object, once for sleeping,
				// but subtract a bit so that wake-up is more conservative than sleep).
				// Expand more for velocity.
				TReal r_expand = 1.75f * rCOLLIDE_EXPAND + xob.fVelocityExpand;

				// Get moving object's activation sphere info.
				CBoundVolSphere bvs;
				CVector3<> v3;
				xob.GetCollideSphere(&bvs, &v3, r_expand);

				for 
				(
					std::list<CInstance*>::iterator itpins = pqreg->plspinsNear->begin();
					itpins != pqreg->plspinsNear->end();
				)
				{
					CInstance* pins_near = *itpins;

					if (pphSystem->bIgnoreCollisions(pins_near, xob.Instances[0]))
					{
						++itpins;
						continue;
					}

					// A box that is inactive or sleepy. 
					// Get its physics bounding volume; do not adjust for velocity as it has none. 
					// Use its current world presence.
					const CPresence3<>& pr3_near = pins_near->pphiGetPhysicsInfo()->pr3Collide(pins_near);
					const CBoundVol* pbv_near = pins_near->pphiGetPhysicsInfo()->pbvGetCollideVol();
					Assert(pbv_near);

					// Get equivalent sphere for preliminary intersection.
					TReal r_radius_near = pbv_near->fMaxExtent() * pr3_near.rScale;

					// Perform sphere-sphere test first.
					if ((v3 - pr3_near.v3Pos).tLenSqr() < Sqr(bvs.rRadius + r_radius_near))
					{
						CTimeBlock tmb(&psAwakenVol);

						// Right. Now do full BV intersection if necessary.
						ESideOf esf;

						if (xob.bUseBox())
						{
							// Get the moving box volume.
							if (!ab_boxes[i])
							{
								xob.GetCollideBox(&abvb_boxes[i], &atf3_boxes[i], r_expand);
								ab_boxes[i] = true;
							}

							if (CJoeXob::bUseBox(*pbv_near, pr3_near.rScale))
							{
								// Box-box intersection.
								CTransform3<> tf3_to_near = atf3_boxes[i] * CTransform3<>(~pr3_near);
								esf = pbv_near->esfSideOf(abvb_boxes[i].tf3Box(tf3_to_near));
								if (esf == esfINTERSECT)
									esf = abvb_boxes[i].esfSideOf(pbv_near->tf3Box(~tf3_to_near));
							}
							else
							{
								// Sphere-box intersection.
								CPresence3<> pr3_sph = pr3_near;
								pr3_sph.v3Pos /= atf3_boxes[i];
								esf = abvb_boxes[i].esfSideOf(pbv_near->bvsGetBoundingSphere(), 0, &pr3_sph);
							}
						}
						else
						{
							// Using the sphere.
							if (CJoeXob::bUseBox(*pbv_near, pr3_near.rScale))
								// Box-sphere intersection.
								esf = pbv_near->esfSideOf(bvs, &pr3_near, &CPresence3<>(v3));
							else
								// Sphere-sphere; already did it.
								esf = esfINSIDE;
						}

						if (esf & esfINSIDE)
						{
							// Got the intersection.

							// The list should contain only inactive objects.
							AlwaysAssert(pphSystem->iGetIndex(pins_near) < 0);

							pins_near->PhysicsActivate();
							int i_near = pphSystem->iGetIndex(pins_near);
							if (i_near >= 0)
							{
								CJoeXob& xob_near = JoeXob(i_near);

								// We succeeded in activating.
								// Mark these objects as interacting, so we don't test again.
								xob.Do_It[i_near] = xob_near.Do_It[i] = true;
								xob.Intersected[i_near] = xob_near.Intersected[i] = true;

								// Propagate last-touched time, for sleep Nazi.
								xob_near.iLastTouched = xob.iLastTouched;

								if (xob_near.Movable)
									i_movable++;

								// Reset our iteratation, as activation has removed this object
								// (and all its magneted attachments) from our list.
								itpins = pqreg->plspinsNear->begin();
								continue;
							}
						}
					} // if spheres intersect

					++itpins;

					psAwaken.Add(0, 1);
				} // foreach (*pqreg->plspinsNear)
			} // if (xob.Movable)
			else
			{
				Assert(!aqregNear[i].plspinsNear);
				Assert(!aqregNear[i].pwqttopTerrain);
			}
		} // for all awake objects

		psAwaken.Add(ctmr2());

		// Set the global sleep threshold for the coming frame based on number of moving boxes.
		PHY_sleep = fSLEEP_DEFAULT * Sqr(Max((float)i_movable / iMAX_PHYSICS_MOVABLE * 5.0f, 1.0f));

		//
		// Iterate through all awake pairs, finding those near enough to interact.
		// Store interaction info in Do_It, for physics system, and bump activities if needed.
		//

		for (i = 1; i < iMAX_PHYSICS_OBJECTS; i++)  if (Xob[i].Instances[0])
		{
			CJoeXob& xob = JoeXob(i);

			// Get moving object's activation sphere info.
			CBoundVolSphere bvs;
			CVector3<> v3;

			// Expand interact volume by our threshold, plus velocity.
			TReal r_expand = rCOLLIDE_EXPAND + xob.fVelocityExpand;
			xob.GetCollideSphere(&bvs, &v3, r_expand);

			// Iterate through each pair once, treating them symmetrically.
			for (int i_2 = 0; i_2 < i; i_2++)  if (Xob[i_2].Instances[0])
			{
				CJoeXob& xob2 = JoeXob(i_2);

				if ((xob.Movable && ai_orig_info[i] > 0) || (xob2.Movable && ai_orig_info[i_2] > 0))
				{
					// Quick test here for bio-box ignoring.
					// Don't check world ignore list unless we get an intersection.
					if (xob.bIgnoreCollisions(xob2, false))
						continue;

					if (xob.Do_It[i_2])
						// Already marked by wake-up.
						continue;
/*
					if (!pphSystem->bAllowSleep)
					{
						// Always interact all waking objects.
						aabIntersected[i][i_2] = aabIntersected[i_2][i] = true;
						if (!pphSystem->bIgnoreCollisions(Instances[i][0], Instances[i_2][0]))
							Do_It[i][i_2] = Do_It[i_2][i] = true;
						continue;
					}
*/
					CBoundVolSphere bvs_2;
					CVector3<> v3_2;

					// Expand interact volume by our threshold, plus velocity.
					TReal r_expand_2 = rCOLLIDE_EXPAND + xob2.fVelocityExpand;
					xob2.GetCollideSphere(&bvs_2, &v3_2, r_expand_2);

					// Perform sphere-sphere test first.
					if ((v3 - v3_2).tLenSqr() < Sqr(bvs.rRadius + bvs_2.rRadius))
					{
						// Right. Now do full BV intersection if necessary.
						CTimeBlock tmb(&psInteractVol);
						ESideOf esf;

						if (xob.bUseBox() && !ab_boxes[i])
						{
							xob.GetCollideBox(&abvb_boxes[i], &atf3_boxes[i], r_expand);
							ab_boxes[i] = true;
						}

						if (xob2.bUseBox())
						{
							// Box-box intersection.
							if (!ab_boxes[i_2])
							{
								xob2.GetCollideBox(&abvb_boxes[i_2], &atf3_boxes[i_2], r_expand_2);
								ab_boxes[i_2] = true;
							}

							if (xob.bUseBox())
							{
								// Box-box intersection.
								CTransform3<> tf3_21 = atf3_boxes[i_2] / atf3_boxes[i];
								esf = abvb_boxes[i].esfSideOf(abvb_boxes[i_2].tf3Box(tf3_21));
								if (esf == esfINTERSECT)
									esf = abvb_boxes[i_2].esfSideOf(abvb_boxes[i].tf3Box(~tf3_21));
							}
							else
								// Box-sphere intersection.
								esf = abvb_boxes[i_2].esfSideOf(bvs, 0, &CPresence3<>(v3 / atf3_boxes[i_2]));
						}
						else
						{
							if (xob.bUseBox())
								// Sphere-box intersection.
								esf = abvb_boxes[i].esfSideOf(bvs_2, 0, &CPresence3<>(v3_2 / atf3_boxes[i]));
							else
								// Sphere-sphere; already did it.
								esf = esfINSIDE;
						}

						if (esf & esfINSIDE)
						{
							xob.Intersected[i_2] = xob2.Intersected[i] = true;

							if (pphSystem->bIgnoreCollisions(xob.Instances[0], xob2.Instances[0]))
								continue;

							// Got the intersection.
							xob.Do_It[i_2] = xob2.Do_It[i] = true;

							// Moved objects up the activities of other objects.
							if (xob.Movable)
							{
								if (ai_orig_info[i] == INFO_MAX)
									xob2.Info = INFO_MAX;
								if (xob.iLastTouched > Max(xob2.iLastTouched, xob2.iActivatedFrame))
									xob2.SleepThreshold = xob.SleepThreshold;
								SetMax(xob2.iLastTouched, xob.iLastTouched);
							}
							if (xob2.Movable)
							{
								if (ai_orig_info[i_2] == INFO_MAX)
									xob.Info = INFO_MAX;
								if (xob2.iLastTouched > Max(xob.iLastTouched, xob.iActivatedFrame))
									xob.SleepThreshold = xob2.SleepThreshold;
								SetMax(xob.iLastTouched, xob2.iLastTouched);
							}

							continue;
						}
					} // if spheres intersect

					psInteract.Add(0, 1);
				} // if either Info == INFO_MAX
			} // for i_2
		} // for i_1

		// Based on lifetime of objects, manage sleep thresholds, to prevent objects staying awake forever.
		for (i = 0; i < iMAX_PHYSICS_OBJECTS; i++)  if (Xob[i].Info > 0 && Xob[i].Movable)
		{
			CJoeXob& xob = JoeXob(i);

			int i_age = iPhysFrame - Max(xob.iLastTouched, xob.iActivatedFrame);
			if (i_age >= iFREE_AGE)
			{
				// Start increasing object's sleep threshold.
				if (i_age % iFREE_AGE == 0)
				{
					dout <<"Object " <<xob.Instances[0]->strGetInstanceName()
						 <<" has been awake for " <<(i_age * MAX_TIMESTEP)
						 << " s; invoking sleep Nazi.\n";
				}
				xob.SleepThreshold *= fSLEEP_GROW;
			}
		}

		psInteract.Add(ctmr2());

		psIntersect.Add(ctmr());
	}

	//*****************************************************************************************
	void CPhysicsInfoBox::ProcessCreateDelete()
	{
		// Since instances are being added/deleted to world, purge all queries.
//		Purge();
	}

	//*****************************************************************************************
	void CPhysicsInfoBox::Purge()
	{
		for (int i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
		{
			// Clean up query regions.
			aqregNear[i].Clear();
		}
	}

	//*****************************************************************************************
	void CPhysicsInfoBox::Reset()
	{
		// Clear our internal arrays.
		Purge();

		// Clear out physics system stuff.
		CXob::Reset();
	}

	//*****************************************************************************************
	CBoundVolBox CPhysicsInfoBox::bvbBoundBox(int i_index, int i_elem)
	{
		return JoeXob(i_index).bvbBoundBox(i_elem);
	}

	//*****************************************************************************************
	CPlacement3<> CPhysicsInfoBox::p3BoxElement(int i_index, int i_elem)
	{
		return JoeXob(i_index).p3BoxElement(i_elem);
	}

	//*****************************************************************************************
	void CPhysicsInfoBox::DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const
	{
#if bVER_BONES()
		// Find this instance in the physics system.
		const CPhysicsInfoCompound* pphic = pins->pphiGetPhysicsInfo()->pphicCast();
		int i_skip = pphic ? pphic->iCurrentSubmodel : 0;
		CVector2<int> v2i_index = pphSystem->v2iGetIndices(pins, i_skip);
		if (v2i_index.tX >= 0)
		{
			// Draw physics system's internal extents.
			JoeXob(v2i_index.tX).DrawPhysicsBoxes(v2i_index.tY, draw, cam, setedfMain);
		}

		if (setedfMain[edfBOXES] && (v2i_index.tX < 0 || !setedfMain[edfBOXES_PHYSICS]))
		{
			//
			// Draw world collide boxes.
			//

			// Don't box intangibles.
			if (!setFlags[epfTANGIBLE])
				return;

			// Get the compound physics info if it exists.  If it exists, then this box is a submodel
			// of the compound model.
			const CPhysicsInfoCompound* pphic = pins->pphiGetPhysicsInfo()->pphicCast();

			CPresence3<> pr3 = pins->pr3Presence();

			// Do we have a submodel?
			if (pphic)
			{
				// Yes! Apply the extra presence from the compound model.
				Assert(pphic->apphiSubmodels[pphic->iCurrentSubmodel] == this);
				pr3 = pphic->apr3SubmodelPresences[pphic->iCurrentSubmodel] * pr3;
			}

			// Get placement and box size (world scale).
			CPlacement3<> p3 = pr3.p3Placement();
			CBoundVolBox bvb = bvbCollideVol;
			bvb *= pr3.rScale;

			if ((v2i_index.tX >= 0 && JoeXob(v2i_index.tX).bIgnoreAllCollisions()) || 
				pphSystem->bIgnoreCollisions(pins, 0))
				draw.Colour(clrIGNORE);
			else if (v2i_index.tX >= 0)
			{
				if (Xob[v2i_index.tX].Moved)
					draw.Colour(clrACTIVE);
				else
					draw.Colour(clrIMMOVABLE);
			}
			else
				draw.Colour(clrINACTIVE);

			// Draw the box.
			CTransform3<> tf3_shape_screen = p3 * cam.tf3ToHomogeneousScreen();
			draw.Box3D(bvb.tf3Box(tf3_shape_screen));
		}

		if (setedfMain[edfBOXES_WAKE] && v2i_index.tX < 0)
		{
			// Draw wakeup volume, dark blue.
			draw.Colour(clrACTIVATE);

			const CPresence3<>& pr3 = pins->pphiGetPhysicsInfo()->pr3Collide(pins);
			CTransform3<> tf3_screen = cam.tf3ToHomogeneousScreen();
			CTransform3<> tf3_shape_screen = CTransform3<>(pr3) * tf3_screen;

			const CBoundVol* pbv = pins->pphiGetPhysicsInfo()->pbvGetCollideVol();
			Assert(pbv);

			if (CJoeXob::bUseBox(*pbv, pr3.rScale))
			{
				const CBoundVolBox* pbvb = pbv->pbvbCast();
				if (pbvb)
					draw.Box3D(pbvb->tf3Box(tf3_shape_screen));
			}
			else
			{
				// Draw the sphere.
				TReal r_radius = pbv->fMaxExtent() * pr3.rScale;

				// Find radius offset in world space.
				CVector3<> v3_radius = CVector3<>(r_radius, 0, 0) * cam.pr3GetPresence().r3Rot;
				CVector2<> v2_cm = tf3_shape_screen.v3Pos.v2Project();
				CVector3<> v2_border = ((pr3.v3Pos + v3_radius) * tf3_screen).v2Project();

				// Get radius in camera space.
				draw.Circle(v2_cm.tX, v2_cm.tY, (v2_border - v2_cm).tLen());
			}
		}

		// Now draw the magnets for the box.
		if (setedfMain[edfMAGNETS])
			NMagnetSystem::DrawMagnets(pins, draw, cam);
#endif
	}

//*****************************************************************************************
//
// class CXob implementation.
//
// The following functions are implemented here instead of Xob_bc.cpp,
// to provide separation between physics and world code.
//

	//*****************************************************************************************
	float CXob::fGetKineticEnergy()
	{
		CVector3<> v3_vel;

		// Always use Z velocity for energy calculation.


		// MASSIVE HACK!  HACK HACK HACK 
		//  HOOOO BOY.
		CBoundaryBox* pbb = ptCast<CBoundaryBox>(Instances[0]);

		// Do we have the player's foot?
		if (pbb && pbb->paniAnimate == gpPlayer && pbb->ebbElement == ebbFOOT)
		{
			// Yes!  Only record Z component of energy to make falling damage as simple as possible.
			v3_vel.tX = 0;
			v3_vel.tY = 0;
			v3_vel.tZ = State[2][1];
		}
		else
		{
			// No!  Calculate full energy for box.
			v3_vel.tX = State[0][1];
			v3_vel.tY = State[1][1];
			v3_vel.tZ = State[2][1];
		}

		float f_hack_mass = Data[19];


		if (!pbb)
		{
//			float f_physical_mass = 1.0f / Data[0];
//			dout << f_physical_mass << "\t";
	#define fHIGH_MASS	20.0f
	#define fFALLOFF 	0.01f
			if (f_hack_mass > fHIGH_MASS)
			{
				f_hack_mass = sqrt(fHIGH_MASS * f_hack_mass);
//				f_hack_mass = fHIGH_MASS + fFALLOFF * (f_hack_mass - fHIGH_MASS);
//				dout << f_hack_mass << "\n";
			}
		}

		float f_vel_sqr = v3_vel.tLenSqr();

		// Clamp velocity to 30m/s for damage calculations.
		if (f_vel_sqr > fMAX_DAMAGE_VEL * fMAX_DAMAGE_VEL)
			f_vel_sqr = fMAX_DAMAGE_VEL * fMAX_DAMAGE_VEL;

		float f_energy = f_hack_mass * f_vel_sqr;
		return f_energy;
	}

	//******************************************************************************************
	float CXob::fTerrainMaxHeight()
	{
		CQueryRegion* pqreg = JoeXobSelf.pqregNear();

		if (CWDbQueryTerrain().tGet())
			Assert(pqreg->pwqttopTerrain);

		if (pqreg->pwqttopTerrain)
			// There is an active terrain query.
			return pqreg->pwqttopTerrain->rMaxHeight();
		else
			return 0;
	}

	//******************************************************************************************
	float CXob::fTerrainHeight(float f_x, float f_y)
	{
		CTimeBlock tmb(&psQTrrHeight);

		// Index must refer to a moving box's query region.
		CQueryRegion* pqreg = JoeXobSelf.pqregNear();

		if (CWDbQueryTerrain().tGet())
			Assert(pqreg->pwqttopTerrain);

		if (pqreg->pwqttopTerrain)
		{
			// There is an active terrain query.
			pqreg->Clamp(f_x, f_y);
			return pqreg->pwqttopTerrain->rHeight(f_x, f_y);
		}
		else
			return 0;
	}

	//******************************************************************************************
	float CXob::fTerrainHeight(float f_x, float f_y, float af_normal[3])
	{
		CTimeBlock tmb(&psQTrrHeight);

		// Index must refer to a moving box's query region.
		CQueryRegion* pqreg = JoeXobSelf.pqregNear();

		if (CWDbQueryTerrain().tGet())
			Assert(pqreg->pwqttopTerrain);

		if (pqreg->pwqttopTerrain)
		{
			// There is an active terrain query.
			pqreg->Clamp(f_x, f_y);
			const CPlane* ppl;
			TReal r_height = pqreg->pwqttopTerrain->rHeight(f_x, f_y, &ppl);
			Copy(af_normal, ppl->d3Normal);
			return r_height;
		}
		else
		{
			af_normal[0] = af_normal[1] = 0.0f;
			af_normal[2] = 1.0f;
			return 0;
		}
	}

	//******************************************************************************************
	float CXob::fTerrainHeight(float f_x, float f_y, float f_zmin, float af_normal[3])
	{
		CTimeBlock tmb(&psQTrrHeight);

		// Index must refer to a moving box's query region.
		CQueryRegion* pqreg = JoeXobSelf.pqregNear();

		if (CWDbQueryTerrain().tGet())
			Assert(pqreg->pwqttopTerrain);

		if (pqreg->pwqttopTerrain)
		{
			pqreg->Clamp(f_x, f_y);

			// There is an active terrain query.
			if (pqreg->pwqttopTerrain->rMaxHeight() >= f_zmin)
			{
				// The terrain is potentially as high as the minimum. Find exact height here.
				const CPlane* ppl;
				TReal r_height = pqreg->pwqttopTerrain->rHeight(f_x, f_y, &ppl);
				Copy(af_normal, ppl->d3Normal);
				return r_height;
			}
			else
			{
				af_normal[0] = af_normal[1] = 0.0f;
				af_normal[2] = 1.0f;
				return pqreg->pwqttopTerrain->rMaxHeight();
			}
		}
		else
		{
			af_normal[0] = af_normal[1] = 0.0f;
			af_normal[2] = 1.0f;
			return 0;
		}
	}

	//*****************************************************************************************
	inline void Interp(CVector3<>& v3_a, const CVector3<>& v3_b, TReal r_t)
	{
		v3_a = v3_a * (1.0f - r_t) + v3_b * r_t;
	}

	//*****************************************************************************************
	bool bClip(CVector3<>& v3_a, const CVector3<>& v3_b, TReal r_min, TReal r_max, TReal r_a, TReal r_b)
	{
		TReal r_d = r_b - r_a;

		if (r_a < r_min)
		{
			// Outside the plane.
			if (r_b < r_min)
				// Does not intersect.
				return false;

			// Crosses the plane. Interpolate point A to lie on the plane.
			if (r_d)
				Interp(v3_a, v3_b, (r_min - r_a) / r_d);
		}
		else if (r_a > r_max)
		{
			// Outside the plane.
			if (r_b > r_max)
				// Does not intersect.
				return false;

			// Crosses the plane. Interpolate point A to lie on the plane.
			if (r_d)
				Interp(v3_a, v3_b, (r_max - r_a) / r_d);
		}

		return true;
	}

	//*****************************************************************************************
	bool bClip(CVector3<>& v3_a, const CVector3<>& v3_b, TReal r_a, TReal r_b)
	{
		if (r_a < 0.0f)
		{
			// Outside the plane.
			if (r_b < 0.0f)
				// Does not intersect.
				return false;

			// Crosses the plane. Interpolate point A to lie on the plane.
			TReal r_d = r_b - r_a;
			if (r_d)
				Interp(v3_a, v3_b, - r_a / r_d);
		}
		else if (r_a > 1.0f)
		{
			// Outside the plane.
			if (r_b > 1.0f)
				// Does not intersect.
				return false;

			// Crosses the plane. Interpolate point A to lie on the plane.
			TReal r_d = r_b - r_a;
			if (r_d)
				Interp(v3_a, v3_b, (1.0f - r_a) / r_d);
		}

		return true;
	}

	const TReal rEDGE_TOL	= 0.01f;

	//*****************************************************************************************
	int CXob::iTerrainEdges(TVectorPair* apvp_edges[], int i_max)
	{
		Assert(i_max > 0);
		CTimeBlock tmb(&psQTrrEdge);

		// Index must refer to a moving box's query region.
		CQueryRegion* pqreg = JoeXobSelf.pqregNear();
		if (!Verify(pqreg->bActive()))
			return 0;

		// Get edges the first time requested.
		if (!pqreg->pavpEdges.size())
			pqreg->GetEdges();

		//
		// Fill the array with pointers to actual intersecting edges.
		//

		// Get a box definition from the Extents.
		CBoundVolBox bvb;
		CTransform3<> tf3;
		JoeXobSelf.GetCollideBox(&bvb, &tf3, 0);

		// Construct transform from world space to unit cube.
		CTransform3<> tf3_inv = ~bvb.tf3Box(tf3);

		int i_count = 0;
		forall (pqreg->pavpEdges, CPArray<TVectorPair>, ped)
		{
			// Intersect the edge with the box.

			// Convert points to box space.
			CVector3<> v3_a = v3Convert((*ped)[0]) * tf3_inv;
			CVector3<> v3_b = v3Convert((*ped)[1]) * tf3_inv;

			if (bClip(v3_a, v3_b, v3_a.tX, v3_b.tX) &&
				bClip(v3_a, v3_b, v3_a.tY, v3_b.tY) &&
				bClip(v3_a, v3_b, v3_a.tZ, v3_b.tZ))
			{
				if (bWithin(v3_a.tX, -rEDGE_TOL, 1.0f + rEDGE_TOL) &&
					bWithin(v3_a.tY, -rEDGE_TOL, 1.0f + rEDGE_TOL))
				{
					apvp_edges[i_count++] = ped;
					if (i_count >= i_max)
						break;
				}
			}
		}

		psQTrrEdge.Add(0, i_count);
		return i_count;
	}

	//*****************************************************************************************
	float CXob::fWaterHeight(float f_x, float f_y, CEntityWater** ppetw)
	{
		CTimeBlock tmb(&psQWater);

		// Index must refer to a moving box's query region.
		Assert(Movable);
		CQueryRegion* pqreg = JoeXobSelf.pqregNear();

		if (Verify(pqreg->pwqwtrWater)) 
		{
			foreach ((*pqreg->pwqwtrWater))
			{
				// Ask the water its height.
				float f_z = (**pqreg->pwqwtrWater)->rWaterHeight(CVector2<>(f_x, f_y));
				if (f_z > rWATER_NONE)
				{
					// There is water here, and it is above the terrain.
					*ppetw = **pqreg->pwqwtrWater;
					return f_z;
				}
			}
		}

		*ppetw = 0;
		return rWATER_NONE;
	}

	//*****************************************************************************************
	int CXob::iTerrainSound(float f_x, float f_y)
	{
		CQueryRegion* pqreg = JoeXobSelf.pqregNear();
		if (!Verify(pqreg->bActive()))
			return 0;
		const CTerrainObj* pto = pqreg->ptoTerrainObject(f_x, f_y);
		return pto ? pto->matSoundMaterial : 0;
	}

	//*****************************************************************************************
	float CXob::fTerrainFriction(float f_x, float f_y)
	{
		CQueryRegion* pqreg = JoeXobSelf.pqregNear();
		if (!Verify(pqreg->bActive()))
			return fDEFAULT_FRICTION;
		const CTerrainObj* pto = pqreg->ptoTerrainObject(f_x, f_y);
		return pto ? pto->fFriction : fDEFAULT_FRICTION;
	}

	//*****************************************************************************************
	void CXob::CheckPelvis()
	{
		Assert(PelvisElem == 0);
		CPlayer* pplay = ptCast<CPlayer>(CPhysicsInfoSkeleton::paniSkeleton(PelvisModel));
		if (pplay)
			pplay->CheckForDrop();
	}
