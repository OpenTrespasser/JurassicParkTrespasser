/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Implementation of InfoCompound.hpp
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/InfoCompound.cpp                                         $
 * 
 * 46    98/10/01 16:21 Speter
 * New presence used for tighter fitting collide volumes.
 * 
 * 45    98/09/30 19:04 Speter
 * Set instances in SetupSuperBox.
 * 
 * 44    98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 43    98/09/04 21:59 Speter
 * Added sender to Move().
 * 
 * 42    98/09/02 21:19 Speter
 * Removed i_activity param from Activate().
 * 
 * 41    8/28/98 5:27p Agrant
 * assert for doubly referencing physics submodels
 * 
 * 40    98/08/27 0:53 Speter
 * Added bIsMoving(), made some args const.
 * 
 * 39    98/07/08 18:35 Speter
 * Added bVER_BONES() switch.
 * 
 * 38    98/06/25 17:37 Speter
 * bvbTotal is now derived from both bound vols and collide vols of subobjects.Implemented
 * several suboptions for DrawPhysics (bones). RayIntersect function changed to take explicity
 * subobject index.
 * 
 * 37    6/18/98 4:43p Agrant
 * mass takes a const instance pointer
 * 
 * 36    98/06/09 21:27 Speter
 * Added needed include.
 * 
 *********************************************************************************************/


//
// Includes.
//

#include "common.hpp"
#include "InfoCompound.hpp"

#include "VerBones.hpp"
#include "PhysicsSystem.hpp"
#include "Xob_bc.hpp"
#include "Magnet.hpp"
#include "InfoBox.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Std/Hash.hpp"

//
// Constants.
//

#if VER_TEST
std::set<uint32, std::less<uint32> > setSubmodels;
#endif


//**********************************************************************************************
//
//  CPhysicsInfoCompound implementation
//
	
	//*****************************************************************************************
	CPhysicsInfoCompound::CPhysicsInfoCompound
	(
		const CHandle&				h_obj,				// Handle to the object in the value table.
		const CGroffObjectName*		pgon,		// Object to load.
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				
	)
		: CPhysicsInfo(pgon, h_obj, pvtable, pload),
		bvbBound(0, 0, 0), bvbCollide(0, 0, 0), v3Collide(0, 0, 0)
	{
		// No submodels yet.
		iNumSubmodels = 0;
		iCurrentSubmodel = -1;
	}


	// Just a place to keep the infos in one spot.
std::list<CPhysicsInfoCompound> lphicCompoundInfos;

	//*****************************************************************************************
	const CPhysicsInfoCompound* CPhysicsInfoCompound::pphicFindShared
	(
		const CHandle&				h_obj,				// Handle to the object in the value table.
		const CGroffObjectName*		pgon,		// Object to load.
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				
	)
	{
		// Don't instance for now.

		// Create the compound info.
		CPhysicsInfoCompound phic(h_obj, pgon, pvtable, pload);
		CPhysicsInfoCompound* pphic = &phic;

		// Find each sub-model, and add it.
		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_obj)
		{
			int i_index = 0;

			const CEasyString* pestr;
			while(bFILL_pEASYSTRING(pestr, ESymbol(esModel00 + i_index)))
			{
				const CGroffObjectName* pgon_submodel = pload->goiInfo.pgonFindObject(pestr->strData());

#if VER_TEST
				char str_buffer[512];

				// Make sure we don't use the same compound submodel twice!
				if (!setSubmodels.insert(u4Hash(pestr->strData())).second)
				{
					sprintf(str_buffer, "%s\n\nDouble reference of physics submodel:\n%s", __FILE__, pestr->strData());

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}

				// Make sure we found the submodel!
				if (!pgon_submodel)
				{
					sprintf(str_buffer, "%s\n\nMissing part of compound physics object:\n%s", __FILE__, pestr->strData());

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}

				// Make sure that the submodel has some defining text props!
				if (pgon_submodel->hAttributeHandle == hNULL_HANDLE)
				{
					sprintf(str_buffer, "%s\n\nPart of compound physics object has no text props:\n%s", __FILE__, pestr->strData());

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}
#endif

				SETUP_OBJECT_HANDLE(pgon_submodel->hAttributeHandle)
				{
					// First, snag the mesh  (will replace with bbox when supported in GROFF)
					rptr<CRenderType> prdt = CRenderType::prdtFindShared(
						pgon_submodel, pload, pgon_submodel->hAttributeHandle, pvtable);

					// Construct an infobox out of the mesh and the props.
					const CPhysicsInfo* pphi = pphiFindShared(prdt, pgon_submodel, pgon_submodel->hAttributeHandle, pvtable, pload);

					CPresence3<> pr3 = ::pr3Presence(*pgon_submodel) * (~::pr3Presence(*pgon));

					// Inherit sound material from compound.
					if (pphi->smatGetMaterialType() == 0)
					{
						const_cast<CPhysicsInfo*>(pphi)->SetMaterialType(pphic->smatGetMaterialType());
					}

					// Add the box to the compound
					pphic->AddSubmodel(pphi, pr3);

					// Delete the mesh
					// Done by rptr magic!
				}
				END_OBJECT;

				// Increment the counter.
				++i_index;
			}

			// Make sure that we have at least one submodel.
			AlwaysAssert(i_index > 0);
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		lphicCompoundInfos.push_front(phic);

		pphic = &(*lphicCompoundInfos.begin());

		Assert(pphic);

		return pphic;
	}

	//*****************************************************************************************
	TReal CPhysicsInfoCompound::fVolume(CInstance* pins) const
	{
		float f_vol = 0.0;
		for (int i = 0; i < iNumSubmodels; i++)
		{
			TReal r_s = apr3SubmodelPresences[i].rScale;
			f_vol += apphiSubmodels[i]->fVolume(pins) * r_s * r_s * r_s;
		}
		return f_vol;
	}

	//*****************************************************************************************
	TReal CPhysicsInfoCompound::fMass(const CInstance* pins) const
	{
		// Sum the submodel masses, scaling each by relative scale.
		float f_mass = 0.0;
		for (int i = 0; i < iNumSubmodels; i++)
		{
			TReal r_s = apr3SubmodelPresences[i].rScale;
			f_mass += apphiSubmodels[i]->fMass(pins) * r_s * r_s * r_s;
		}
		return f_mass;
	}

	//*****************************************************************************************
	void CPhysicsInfoCompound::AddSubmodel(const CPhysicsInfo* pphi, const CPresence3<>& pr3)
	{
		// Ought to have an array entry for each submodel.
		Assert(iNumSubmodels == apphiSubmodels.uLen);

		apphiSubmodels << pphi;
		apr3SubmodelPresences << pr3;
		iNumSubmodels++;

		// Expand bvbBound by new volume.
		CVector3<> v3_box = bvbBound.v3GetMax();

		// Find bounding box in terms of extents.
		// This is in object space, despite function name.
		CVector3<> v3_min, v3_max;
		pphi->pbvGetBoundVol()->GetWorldExtents(pr3, v3_min, v3_max);

		// For now, we only have a symmetrical bounding box; so expand max.
		// Also, we store only one box, used for both bounding and collide volume.
		SetMax(v3_box.tX, Max(Abs(v3_min.tX), Abs(v3_max.tX)));
		SetMax(v3_box.tY, Max(Abs(v3_min.tY), Abs(v3_max.tY)));
		SetMax(v3_box.tZ, Max(Abs(v3_min.tZ), Abs(v3_max.tZ)));

		bvbBound = CBoundVolBox(v3_box);

		//
		// Collide box is more flexible, as it comes with its own placement.
		//

		v3_box = bvbCollide.v3GetMax();

		// Expand bvbCollide by new volume.
		v3Collide /= apr3SubmodelPresences[0].r3Rot;
		CVector3<> v3_min_total = v3Collide - bvbCollide.v3GetMax(), 
				   v3_max_total = v3Collide + bvbCollide.v3GetMax();

		// Find bounding box in terms of extents.
		// Volume is stored in element 0's rotation.
		CPresence3<> pr3_sub = pr3;
		pr3_sub /= apr3SubmodelPresences[0].r3Rot;
		pphi->pbvGetCollideVol()->GetWorldExtents(pr3_sub, v3_min, v3_max);

		// Grow collide extents by submodel.
		v3_min_total.SetMin(v3_min);
		v3_max_total.SetMax(v3_max);

		bvbCollide = CBoundVolBox((v3_max_total - v3_min_total) * 0.5f);
		v3Collide = (v3_max_total + v3_min_total) * 0.5f * apr3SubmodelPresences[0].r3Rot;
	}

	//*****************************************************************************************
	void CPhysicsInfoCompound::Activate
	(
		CInstance *pins, bool b_just_update, const CPlacement3<>& p3_vel
	) const
	{
		apphiSubmodels[0]->Activate(pins, b_just_update, p3_vel);
	}

	//*****************************************************************************************
	void CPhysicsInfoCompound::Deactivate(CInstance *pins) const
	{
		apphiSubmodels[0]->Deactivate(pins);
	}

	//*****************************************************************************************
	bool CPhysicsInfoCompound::bIsActive(const CInstance* pins) const
	{
		return pphSystem->iGetIndex(pins) >= 0;
	}

	//*****************************************************************************************
	bool CPhysicsInfoCompound::bIsMoving(const CInstance* pins) const
	{
		// Return true only if the object is active, and has moved.
		int i_index = pphSystem->iGetIndex(pins);
		return i_index >= 0 && Xob[i_index].Moved;
	}

	//*****************************************************************************************
	CPlacement3<> CPhysicsInfoCompound::p3GetVelocity(const CInstance* pins) const
	{
		return apphiSubmodels[0]->p3GetVelocity(pins);
	}

	//*****************************************************************************************
	void CPhysicsInfoCompound::UpdateWDBase(CInstance* pins, int i_index, int i_element) const
	{
		Assert(Xob[i_index].Instances[i_element] == pins);
		Assert(i_element == 0 || Xob[i_index].Instances[i_element-1] != pins);

		// Update, converting the submodel's presence to the main instance's presence.
		CPlacement3<> p3_main = ~(CPlacement3<>&)apr3SubmodelPresences[0];
		p3_main.v3Pos *= pins->fGetScale();
		p3_main *= CPhysicsInfoBox::p3BoxElement(i_index, i_element);
										  
		pins->Move(p3_main, pphSystem);
	}

	//**********************************************************************************************
	void CPhysicsInfoCompound::ApplyImpulse(CInstance* pins, int, const CVector3<>& v3_pos, const CVector3<>& v3_impulse) const
	{
		// Ignore subobject, as physics system handles it at superbox level.
		apphiSubmodels[0]->ApplyImpulse(pins, 0, v3_pos, v3_impulse);
	}

	//*****************************************************************************************
	void CPhysicsInfoCompound::ForceVelocity(CInstance* pins_target, const CVector3<>& v3_new_velocity) const
	{
		apphiSubmodels[0]->ForceVelocity(pins_target, v3_new_velocity);
	}

	//**********************************************************************************************
	void CPhysicsInfoCompound::RayIntersect(CInstance* pins, int i_subobj, CRayCast& rc,
		const CPlacement3<>& p3_orig, TReal r_length, TReal r_diameter) const
	{
		//
		// Insert all intersections with sub-volumes.
		//

		// Save original presence.
		CPresence3<> pr3_orig = pins->pr3GetPresence();

		// Recursively check sub-volumes, adding all relevant intersections to the raycast.
		for (int i = 0; i < apphiSubmodels.size(); i++)
		{
			//
			// Temporarily alter the instance's presence to refer to that of the sub-model.
			// Hacky, but will work in this limited context.
			// Pass along the given i_subobj, as that may theoretically refer to a bio box.
			// Do not use compound element index for this, as no such differentiation need be made.
			//
			pins->SetPresence(apr3SubmodelPresences[i] * pr3_orig);
			apphiSubmodels[i]->RayIntersect(pins, i_subobj, rc, CPresence3<>(p3_orig), r_length, r_diameter);
		}

		// Restore presence.
		pins->SetPresence(pr3_orig);
	}

	//*****************************************************************************************
	CPlacement3<> CPhysicsInfoCompound::p3Base(CInstance* pins) const
	{
		// Adjust the instance's placement by the relative placement of element 0.
		// The physics system stores all element orientations relative to element 0.
		return apr3SubmodelPresences[0] * pins->pr3GetPresence();
	}

	//*****************************************************************************************
	CPresence3<> CPhysicsInfoCompound::pr3Collide(CInstance* pins) const
	{
		return CPresence3<>(CPlacement3<>(apr3SubmodelPresences[0].r3Rot, v3Collide)) * 
			pins->pr3GetPresence();
	}

	//*****************************************************************************************
	void CPhysicsInfoCompound::HandleMessage(const CMessagePhysicsReq& msgpr, CInstance *pins) const
	{
		Assert(0);
	}

	//*****************************************************************************************
	void CPhysicsInfoCompound::DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const
	{
#if bVER_BONES()
		// Draw each submodel.
		for (int i = iNumSubmodels - 1; i >= 0; --i)
		{
			((CPhysicsInfoCompound*)this)->iCurrentSubmodel = i;
			apphiSubmodels[i]->DrawPhysics(pins, draw, cam);
		}
#endif
	}


	//**********************************************************************************************
	void CPhysicsInfoCompound::SetupSuperBox
	(
		const CPresence3<>& pr3_master,
		const CPresence3<>& pr3_slave,
		CMagnetPair* pmp,
		int* pi_elem,
		CInstance* apins[],										// Instances.
		float aaf_extents[iMAX_BOX_ELEMENTS][6],				// Offsets and sizes.
		float aaf_orient[iMAX_BOX_ELEMENTS][3][3],				// Orientation matrices.
		float af_mass[iMAX_BOX_ELEMENTS],						// Masses.
		float af_friction[iMAX_BOX_ELEMENTS],					// Frictions.
		int ai_sound[iMAX_BOX_ELEMENTS],						// Sounds.
		float aaf_magnet_pos[iMAX_BOX_ELEMENTS][3],				// Magnet positions.
		float af_breaking[iMAX_BOX_ELEMENTS]					// Magnet strengths.
	) const
	{
		// Make sure slave is not currently active.
		CInstance* pins_sub = pmp->pinsSlave;

		const CPhysicsInfoCompound* pphic = pins_sub->pphiGetPhysicsInfo()->pphicCast();
		Assert(pphic);
		Assert(pphSystem->iGetIndex(pins_sub) < 0);

		// Put each submodel in its place.
		for (int i = 0; i < iNumSubmodels; ++i)
		{
			//iCurrentSubmodel = i;
			CPresence3<> pr3_submodel = apr3SubmodelPresences[i] * pr3_slave;

			apphiSubmodels[i]->SetupSuperBox
			(
				pr3_master,
				pr3_submodel,
				pmp,
				pi_elem,
				apins,
				aaf_extents,
				aaf_orient,
				af_mass,
				af_friction,
				ai_sound,
				aaf_magnet_pos,
				af_breaking
			);

			// Now correct the mass!
			TReal r_scale = apr3SubmodelPresences[i].rScale;
			af_mass[*pi_elem - 1] *= r_scale * r_scale * r_scale;
		}
	}


