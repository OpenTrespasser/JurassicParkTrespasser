/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of 'PhysicsInfo.hpp.'
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/PhysicsInfo.cpp                                          $
 * 
 * 85    98/09/19 14:53 Speter
 * CPhysicsInfo no longer handles particles; done in canonical manner by CPhysicsSystem.
 * 
 * 84    98/09/17 3:35 Speter
 * Make use of pcrtFind's alternate material type. Provide our own log scale for particles.
 * 
 * 83    98/09/10 20:04 Speter
 * Better direction for particles on gun hits.
 * 
 * 82    9/04/98 4:36p Agrant
 * added floats property
 * 
 * 81    98/08/28 19:50 Speter
 * Added CParticlesCreate:Create() now takes a strength param in event units, rather than an
 * interp param.
 * 
 * 80    98/08/22 2:13 Speter
 * Particles: use actual material passed in message, so terrain works; alter particle direction
 * by gunshot direction.
 * 
 * 79    98/08/21 18:03 Speter
 * Implemented general particle generation from collision event.
 * 
 * 78    8/20/98 1:00p Agrant
 * mouth multiplier
 * mouth and speed multiplier loading
 * 
 * 77    98/08/19 2:32 Speter
 * Added HandleMessage for collision, as yet unimplemented.
 * 
 * 76    8/10/98 5:23p Rwyatt
 * Removed iTotal and replaced it with a memory counter
 * 
 * 75    98/07/09 16:44 Speter
 * Pivot points now stored in CPhysicsInfoBox. 
 * 
 * 74    98/06/25 17:31 Speter
 * Implemented several suboptions for DrawPhysics (bones). RayIntersect function changed to
 * take explicity subobject index.
 * 
 * 73    6/20/98 9:17p Agrant
 * Track physicsinfoskeletons and delete them all at once.  Allows intancing
 * 
 *********************************************************************************************/


//
// Includes.
//

#include "Common.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"

#include "InfoBox.hpp"
#include "InfoSkeleton.hpp"
#include "InfoCompound.hpp"

#include "Lib/GeomDBase/Skeleton.hpp"
#include "Lib/GeomDBase/RayCast.hpp"
#include "Lib/EntityDBase/ParticleGen.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgCollision.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Loader/Loader.hpp"

#include "Lib/Sys/DebugConsole.hpp"

// The constant placement, with all elements 0 (quaternion not unit, obviously).
const CPlacement3<> p3VELOCITY_ZERO(CRotate3<>(0, 0, 0, 0, false), CVector3<>(0, 0, 0));

//**********************************************************************************************
//
//  CPhysicsInfo implementation
//

	CSet<EDrawPhysics> CPhysicsInfo::setedfMain = 
		Set(edfBOXES) + edfBOX_CENTRES + edfMAGNETS + edfSKELETONS + edfRAYCASTS;

	//*****************************************************************************************
	CPhysicsInfo::CPhysicsInfo
	(
		const CGroffObjectName*		pgon,
		const CHandle&				h_obj,				// Handle to the object in the value table.
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				// the loader.
	)
	{
		bool b_moveable = false;
		bool b_tangible = false;
		bool b_floats	= false;
		bool b_reverse_knees = false;
		fDamage = 1.0f;
		fArmour = 1.0f;

		TSoundMaterial tmat	= 0;
		const CEasyString* pestr;

		SETUP_TEXT_PROCESSING(pvtable, pload);
		SETUP_OBJECT_HANDLE(h_obj)
		{
			bFILL_BOOL(b_moveable,	esMoveable);
			bFILL_BOOL(b_tangible,	esTangible);
			bFILL_BOOL(b_floats,	esFloats);
			bFILL_BOOL(b_reverse_knees,	esReverseKnees);
			bFILL_FLOAT(fDamage,	esDamage);
			bFILL_FLOAT(fDamage,	esDamageMultiplier);
			bFILL_FLOAT(fArmour,	esArmour);

			AlwaysAssert(fDamage >= 0);
			AlwaysAssert(fArmour >= 0);

			
			// if no sound material is specified at all then assume it is zero
			if (bFILL_pEASYSTRING(pestr,esSoundMaterial))
			{
				if ( (*pestr) == "" )
				{
					// if no string is specified then we have a zero sound material
					tmat = 0;
				}
				else
				{
					tmat = matHashIdentifier( (*pestr).strData() );
				}
			}
			else
			{
				tmat = 0;
			}
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		setFlags[epfTANGIBLE]	= b_tangible;
		setFlags[epfMOVEABLE]	= b_moveable;
		setFlags[epfFLOATS]		= b_floats;
		
		tmatSoundMaterial = tmat;

		MEMLOG_ADD_COUNTER(emlTotalPhysicsInfo,1);
	}


	// Track the skeletons for deletion.
	list<CPhysicsInfoSkeleton*> lppisPhysicsSkeletons;

	//*****************************************************************************************
	const CPhysicsInfo* CPhysicsInfo::pphiFindShared
	(
		const rptr<CRenderType>&	prdt,				// The shape of the object needing the physics data.
		const CGroffObjectName*		pgon,
		const CHandle&				h_obj,				// Handle to the object in the value table.
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				// the loader.
	)
	{
		// If a box is appropriate, use the shape to create it.  Otherwise, do whatever.
		const char *str_type = "Box";

		SETUP_TEXT_PROCESSING(pvtable, pload);
		SETUP_OBJECT_HANDLE(h_obj)
		{
			CPhysicsInfoBox pib
			(
				prdt,				// Pass shape in, to get volume.
				h_obj,				// Handle to the object in the value table.
				pgon,
				pvtable,			// Pointer to the value table.
				pload
			);

			const CEasyString * pestr;

			// Snag the physics type.
			if (bFILL_pEASYSTRING(pestr,esType))
			{
				str_type = (*pestr).strData();
			}

			// Now switch on the type.
			if (!strcmp(str_type, "Box"))
			{
				return (CPhysicsInfoBox*)CPhysicsInfoBox::pphibFindShared(pib);
			}
			else if (!strcmp(str_type, "Compound"))
			{
				return (CPhysicsInfoCompound*)CPhysicsInfoCompound::pphicFindShared
				(
					h_obj,				// Handle to the object in the value table.
					pgon,
					pvtable,			// Pointer to the value table.
					pload				
				);
			}
			else if (!strcmp(str_type, "Raptor") || !strcmp(str_type, "TRex"))
			{
				CPhysicsInfoSkeleton* phis = new CPhysicsInfoBiped(pib);
				phis->ParseProps(h_obj,	pgon, pvtable, pload);

				// Track pointer for deletion.
				lppisPhysicsSkeletons.push_back(phis);
				phis->setFlags[epfHACK_UNIQUE] = false;

				return phis;
			}
			else if (!strcmp(str_type, "Trike"))
			{
				CPhysicsInfoSkeleton* phis = new CPhysicsInfoQuadruped(pib);
				phis->ParseProps(h_obj,	pgon, pvtable, pload);

				// Track pointer for deletion.
				lppisPhysicsSkeletons.push_back(phis);
				phis->setFlags[epfHACK_UNIQUE] = false;
				return phis;
			}
			else if (!strcmp(str_type, "Human"))
			{
				CPhysicsInfoSkeleton* phis = new CPhysicsInfoHuman(pib);
				phis->ParseProps(h_obj,	pgon, pvtable, pload);

				// Track pointer for deletion.
				lppisPhysicsSkeletons.push_back(phis);
				phis->setFlags[epfHACK_UNIQUE] = false;
				return phis;
			}
			else
			{
				dout << "Unknown physics type: " << str_type << "\n";
				AlwaysAssert(false);
			}
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		// Should never get here!
		Assert(false);
		return 0;
	}

	//**********************************************************************************************
	void CPhysicsInfo::RayIntersect(CInstance* pins, int i_subobj, CRayCast& rc,
		const CPlacement3<>& p3, TReal r_length, TReal r_diameter) const
	{
		// Simply call the physics bounding volume's intersect function.
		SObjectLoc obl;

		const CBoundVol* pbv = pbvGetBoundVol();
		Assert(pbv);
		if (pbv->bRayIntersect(&obl, pins->pr3GetPresence(), p3, r_length, r_diameter))
		{
			// Add the instance to the record.
			obl.pinsObject = pins;
			obl.iSubObject = i_subobj;
			rc.InsertIntersection(obl);
		}
	}

//**********************************************************************************************
//
//  Global variable.
//

	// Default is intangible and non-unique (shared).
	CPhysicsInfo phiDEFAULT(0);


