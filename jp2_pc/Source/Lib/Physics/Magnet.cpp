/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/Magnet.cpp                                               $
 * 
 * 74    10/02/98 3:24p Mlange
 * Added GetAttachedMagnetsSlaves() function.
 * 
 * 73    98/09/25 1:33 Speter
 * No longer deactivate/reactivate objects when using intangible magnets; fixes stow bugs.
 * 
 * 72    9/05/98 5:18p Agrant
 * save memory in final mode-  magnets do not descend from CPartition in final
 * 
 * 71    98/09/04 21:58 Speter
 * Added sender to Move().
 * 
 * 70    98/09/02 21:21 Speter
 * Removed i_activity param from Activate().
 * 
 *********************************************************************************************/


//
// Includes.
//

#include "Common.hpp"
#include "Magnet.hpp"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Query/QPhysics.hpp"
#include "Lib/GeomDBase/PartitionSpace.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/View/LineDraw.hpp"
#include "Lib/Std/StringEx.hpp"
#include "Lib/Loader/PlatonicInstance.hpp"

#include <set>
#include <map>

//**********************************************************************************************
//
// CMagnet implementation
//

	// Static variables.  They could be in the class, but that would just slow down the compile times.
	typedef std::set<CMagnet, std::less<CMagnet> > TSMagnet;
	TSMagnet tsmMagnet;	// A set containing all shared magnets, for instancing.

	// Set this to load magnets as instances, to see where they are.
	#define bIGNORE_COLLISIONS	(0)
	static bool bDebugMagnets = false;

	//*****************************************************************************************
	CMagnet::CMagnet(CSet<EMagnetFlag> setemf, float f_break,
		float f_restore, float f_friction, float f_drive, float f_ang_min, float f_ang_max) :

		// Default magnet is static, unbreakable.
		setFlags(setemf), fBreakStrength(f_break), 
		fRestore(f_restore), fFriction(f_friction), fDrive(f_drive), fAngleMin(f_ang_min), fAngleMax(f_ang_max)
	{
		Assert(fBreakStrength >= 0);
		Assert(fRestore >= 0);
		Assert(fFriction >= 0);
		Assert(bWithin(fAngleMin, float(-dPI), float(+dPI)));
		Assert(bWithin(fAngleMax, float(-dPI), float(+dPI)));

		if (fDrive || bool(setFlags[emfLIMITS]))
		{
			// Only allow on Z axis for now.
			Assert(setFlags[emfZFREE]);
			Assert(!setFlags[emfXFREE][emfYFREE]);
		}
	}

	//*****************************************************************************************
	const CMagnet* CMagnet::pmagFindShared(const CMagnet& mag)
	{
//		CMagnet* pmag_ret = new CMagnet(mag);

		// Insert or find, please.
		std::pair<TSMagnet::iterator, bool> p = tsmMagnet.insert(mag);

		// If we found a duplicate, it will do.
		// If we inserted a new one, the new one will do.
		const CMagnet* pmag_ret = &(*p.first);
		return pmag_ret;
	}


	//*****************************************************************************************
	const CMagnet* CMagnet::pmagFindShared
	(
		const CHandle&	h, 
		CObjectValue*	poval_cmagnet, 
		CValueTable*	pvt, 
		CLoadWorld*		pload
	)
	{
		// Create magnet with defaults.

		CMagnet mag;
		
		// Process the text props if there are any.
		SETUP_TEXT_PROCESSING(pvt, pload);
		SETUP_OBJECT_POINTER(poval_cmagnet)
		{
			bool b_temp;

			if (bFILL_BOOL(b_temp, esBreakable))
				mag.setFlags[emfBREAKABLE] = b_temp;

			if (bFILL_BOOL(b_temp, esXTFree))
				mag.setFlags[emfXTFREE] = b_temp;
			if (bFILL_BOOL(b_temp, esYTFree))
				mag.setFlags[emfYTFREE] = b_temp;
			if (bFILL_BOOL(b_temp, esZTFree))
				mag.setFlags[emfZTFREE] = b_temp;
			if (bFILL_BOOL(b_temp, esXFree))
				mag.setFlags[emfXFREE] = b_temp;
			if (bFILL_BOOL(b_temp, esYFree))
				mag.setFlags[emfYFREE] = b_temp;
			if (bFILL_BOOL(b_temp, esZFree))
				mag.setFlags[emfZFREE] = b_temp;
			if (bFILL_BOOL(b_temp, esHandPickup))
			{
				mag.setFlags[emfHAND_HOLD] = b_temp;
				int i_sub = 0;
				bFILL_INT(i_sub, esSubstitute);
				mag.u1SubstituteMesh = i_sub;
			}
			if (bFILL_BOOL(b_temp, esShoulderHold))
				mag.setFlags[emfSHOULDER_HOLD] = b_temp;

			bFILL_FLOAT(mag.fBreakStrength, esBreakStrength);
			bFILL_FLOAT(mag.fRestore,  esRestoreStrength);
			bFILL_FLOAT(mag.fFriction, esFriction);
			bFILL_FLOAT(mag.fDrive,    esDrive);

			// Angles must be specified together.
			if (bFILL_FLOAT(mag.fAngleMin, esAngleMin))
			{
				Verify(bFILL_FLOAT(mag.fAngleMax, esAngleMax));
				mag.setFlags += emfLIMITS;
			}
			else
				Assert(!bFILL_FLOAT(mag.fAngleMax, esAngleMax));
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		return pmagFindShared(mag);
	}

	//*****************************************************************************************
	CInstance* CMagnet::pinsLoadMasterMagnet
	(
		CGroffObjectName* pgon,
		CLoadWorld*	pload,
		const CHandle& h,
		CValueTable* pvt
	)
	{
		if (bDebugMagnets)
			return new CInstance(pgon, pload, h, pvt, 0);


		// Wait for instances and slave magnets to load.
		if (pload->iLoadPass < 2)
		{
			pgon->bDoneLoading = false;
			return 0;
		}


		// Load them text props!
		Assert(h != hNULL_HANDLE);

		// Attachment names.
		const CEasyString*  pestr_master = 0;
		const CEasyString*  pestr_slave = 0;
		const CEasyString*  pestr_slave_magnet = 0;
		bool b_instance_magnet = false;	// True if the magnet should be instanced rather than
										// specified on a per-object basis (pickup and shoulder magnets)
		SETUP_TEXT_PROCESSING(pvt, pload)
		SETUP_OBJECT_HANDLE(h)
		{
			if (pload->iLoadPass < 3)
			{
				// Must check to see if we want to instance this magnet.
				// If so, we can't wait.  Must parse them now.
				// We must parse now so that we can instance the magnets.
				// We can afford to do so because only -00 objects should have instancing magnets.
				{
					// Are we instancing this magnet?
					bFILL_BOOL(b_instance_magnet, esInstance);

					// Are we loading a pickup or shoulder magnet?
					if (!b_instance_magnet)
					{
						// No!  Wait for later.
						pgon->bDoneLoading = false;
						return 0;
					}
				}
			}

			if (CLoadWorld::bVerbose)
			{
				dout << "Magnet " << pgon->strObjectName << std::endl;
			}

			//
			// We support 2 methods of attachment.
			// The MasterObject and SlaveObject parameters can be specified.
			// Or, if empty, then the 2 objects the magnet intersects will be attached;
			// in this case, the master/slave order is arbitrary.  (We may need to fix this).
			//

			// Get magnet names.
			bFILL_pEASYSTRING(pestr_master,			esMasterObject);
			bFILL_pEASYSTRING(pestr_slave_magnet,	esSlaveMagnet);
			bFILL_pEASYSTRING(pestr_slave,			esSlaveObject);


			CInstance* pins_master = 0;
			CInstance* pins_slave = 0;
			CInstance* pins_slave_magnet = 0;

			CPresence3<> pr3_master_magnet = ::pr3Presence(*pgon);

			if (pestr_slave_magnet)
			{
				pins_slave_magnet = wWorld.ppartPartitions->pinsFindNamedInstance(pestr_slave_magnet->strData());

#if VER_TEST
				if (!pins_slave_magnet)
				{
					// Verify that we are applying instancable magnets only to -00 objects.
					char buffer[512];

					sprintf(buffer, "%s:  Cannot find slave magnet (%s).\n", pgon->strObjectName, pestr_slave_magnet->strData());
					if (bTerminalError(ERROR_ASSERTFAIL, true, buffer, __LINE__))
						DebuggerBreak();
				}
#endif

			}
	
			const char* str_master = pestr_master ? pestr_master->strData() : "";
			const char* str_slave  = pestr_slave  ? pestr_slave->strData()  : "";


#if VER_TEST
			// Verify that we are applying instancable magnets only to -00 objects.
			char buffer[512];
			const char * str_error = str_master[0] != '\0' ? str_master : str_slave;
			
			if (b_instance_magnet && !bGetPlatonicIdealName(str_error, buffer))
			{
				// Cannot apply pickup, shoulder, or text prop world magnet to non-00 object!
				sprintf(buffer, "%s:  Cannot apply instanced magnet to non -00 object (%s).\n", pgon->strObjectName, str_error);
				if (bTerminalError(ERROR_ASSERTFAIL, true, buffer, __LINE__))
					DebuggerBreak();
			}
#endif


			// Have we specified the magnet master or slave objects?
			if (*str_master || *str_slave)
			{
				// Yes!  Find them by name.
				if (*str_master)
					pins_master	= wWorld.ppartPartitions->pinsFindNamedInstance(str_master);
				if (*str_slave)
					pins_slave	= wWorld.ppartPartitions->pinsFindNamedInstance(str_slave);

				if (pins_slave && !pins_master)
				{
					// For some backwards compatibility, switch these when only one instance specified.
					pins_master = pins_slave;
					pins_slave = 0;
				}


				if (pins_master == 0)
				{
					#if VER_TEST
					const char *str_target = (*str_master) ? str_master : str_slave;
					sprintf(buffer, "%s: Cannot find target object (%s).\n", pgon->strObjectName, str_target);
					if (bTerminalError(ERROR_ASSERTFAIL, true, buffer, __LINE__))
						DebuggerBreak();
					#endif
					pgon->bDoneLoading = false;
					return 0;
				}

			}
			else
			{
				// No!  Find them by location.
#if VER_TEST
				sprintf(buffer, "%s: Magnet specified by location only..\nDid you misspell Object again, Rich?\n", pgon->strObjectName);
				if (bTerminalError(ERROR_ASSERTFAIL, true, buffer, __LINE__))
					DebuggerBreak();
#endif
				// Find 1 or 2 intersecting instances.
				// Construct a bounding box with which to search the world.
				CBoundVolBox bvb(2, 2, 2);
				CWDbQueryPhysics wqph(bvb, pr3_master_magnet);

				// Get'em.
				foreach (wqph)
				{
					if (*wqph == pins_slave_magnet)
						continue;
					CInstance* pins = static_cast<CInstance*>(*wqph);

					if (!pins_master)
						// There must always be a master attachment.
						pins_master = pins;
					else if (!pins_slave)
						// If there is a slave, attach it; otherwise, master is magneted to space.
						pins_master = pins;
					else
					{
						dout << "  !Extra attachment " << pins->strGetInstanceName() << std::endl;
						Assert(false);
						return 0;
					}
				}
			}

#if VER_TEST
			if (pins_master == 0)
			{
				sprintf(buffer, "%s: Nothing attached to magnet.\n", pgon->strObjectName);
				if (bTerminalError(ERROR_ASSERTFAIL, true, buffer, __LINE__))
					DebuggerBreak();
			}
#endif

			if (CLoadWorld::bVerbose)
			{
				dout << "  Attaches " << pins_master->strGetInstanceName();
				if (pins_slave)
					dout << " and " << pins_slave->strGetInstanceName();
				dout << std::endl;
			}

			// Make a CMagnet out of the text props.
			CObjectValue* poval_cmagnet = 0;

			// Get the actual CMagnet.
			if (!bFILL_OBJECT(poval_cmagnet, esCMagnet))
			{
				poval_cmagnet = __pov;
			}

			const CMagnet* pmag;

			pmag = pmagFindShared(h, poval_cmagnet, pvt, pload);

			// Make the magnet presence relative to the master object.
			pr3_master_magnet /= pins_master->pr3Presence();

			// When displacements are supported, pass this information as well...
/*
			CVector3<> v3_mag_master(0, 0, 0);
			if (pins_slave_magnet)
				 v3_mag_master = pins_slave_magnet->v3Pos() * ~pins_master->pr3Presence();
*/

			// Pass the master's original rotation as the hinge limit space.
			NMagnetSystem::AddMagnetPair(pins_master, pins_slave, pmag, 
				pr3_master_magnet, pins_master->r3Rot());

			// And finally, remove the now defunct slave magnet.
			if (pins_slave_magnet)
				pins_slave_magnet->Kill();
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		// No instance created.
		return 0;
	}


	//*****************************************************************************************
	CInstance* CMagnet::pinsLoadSlaveMagnet
	(
		CGroffObjectName* pgon,
		CLoadWorld*	pload,
		const CHandle& h,
		CValueTable* pvt
	)
	{
		// HACK HACK HACK

		// Make an instance so we can root out it's position later.
		return new CInstance(pgon, pload, h, pvt, 0);
	}
	
	//*****************************************************************************************
	CVector3<> CMagnet::d3Min() const
	{
		return d3XAxis * CRotate3<>(d3ZAxis, fAngleMin);
	}

	//*****************************************************************************************
	CVector3<> CMagnet::d3Max() const
	{
		return d3XAxis * CRotate3<>(d3ZAxis, fAngleMax);
	}

	//*****************************************************************************************
	CDir3<> CMagnet::d3Axis() const
	{
		// Must always return some direction, so just return Z by default.
		return setFlags[emfXFREE] ? d3XAxis :
			   setFlags[emfYFREE] ? d3YAxis :
			   d3ZAxis;
	}

//**********************************************************************************************
//
// CMagnetPair implementation
//

	//*****************************************************************************************
	CMagnetPair::CMagnetPair
	(
		CInstance* pins_master, CInstance* pins_slave, 
		const CMagnet* pmag, const CPresence3<>& pr3_mag_master, const CRotate3<>& r3_hinge_rel
	)
		: pinsMaster(pins_master), pinsSlave(pins_slave), pmagData(pmag), 
		pr3MagMaster(pr3_mag_master), r3Orig(r3_hinge_rel)
	{
	}

	//*****************************************************************************************
	CMagnetPair::~CMagnetPair()
	{
	}

	//*****************************************************************************************
	CSet<EMagnetFlag> CMagnetPair::setemfFlags() const
	{
		if (!pmagData)
			return set0;

		CSet<EMagnetFlag> setemf = pmagData->setFlags;

		// Add some extra flags that depend on world magneting.
		if (pinsSlave == 0 && !setemf[emfHAND_HOLD][emfSHOULDER_HOLD])
		{
			// This is a world magnet, but may not be flagged as such.
			if (setemf[emfXFREE][emfYFREE][emfZFREE])
				setemf += emfHINGE;
			if (setemf[emfXTFREE][emfYTFREE][emfZTFREE])
				setemf += emfSLIDE;
			if (!setemf[emfHINGE][emfSLIDE])
				setemf += emfFREEZE;
		}

		return setemf;
	}

	//*****************************************************************************************
	CVector3<> CMagnetPair::d3Zero() const
	{
		return d3XAxis * r3Orig;
	}

	//*****************************************************************************************
	CVector3<> CMagnetPair::d3Min() const
	{
		return pmagData->d3Min() * r3Orig;
	}

	//*****************************************************************************************
	CVector3<> CMagnetPair::d3Max() const
	{
		return pmagData->d3Max() * r3Orig;
	}

	//*****************************************************************************************
	const CBoundVol* CMagnetPair::pbvBoundingVol() const
	{
		static CBoundVolPoint bvpt;
		return &bvpt;
	}

	//*****************************************************************************************
	CPresence3<> CMagnetPair::pr3Presence() const
	{
		// Return world presence.
		Assert(pinsMaster);
		return pr3MagMaster * pinsMaster->pr3GetPresence();
	}

	//*****************************************************************************************
	void CMagnetPair::SetPresence(const CPresence3<> &pr3)
	{
		// Convert to slave-relative.
		Assert(pinsMaster);
		pr3MagMaster = pr3 / pinsMaster->pr3GetPresence();
	}

	//*****************************************************************************************
	CPlacement3<> CMagnetPair::p3Placement() const
	{
		// Return world placement.
		Assert(pinsMaster);
		return pr3MagMaster * pinsMaster->pr3GetPresence();
	}

	//*****************************************************************************************
	void CMagnetPair::SetPlacement(const CPlacement3<>& p3)
	{
		SetPresence(p3);
	}

	//*****************************************************************************************
	CVector3<> CMagnetPair::v3Pos() const
	{
		Assert(pinsMaster);
		return pr3MagMaster.v3Pos * pinsMaster->pr3GetPresence();
	}
	
	//*****************************************************************************************
	void CMagnetPair::SetPos(const CVector3<>& v3_pos)
	{
		Assert(pinsMaster);
		pr3MagMaster.v3Pos = v3_pos / pinsMaster->pr3GetPresence();
	}

	//*****************************************************************************************
	CRotate3<> CMagnetPair::r3Rot() const
	{
		Assert(pinsMaster);
		return pr3MagMaster.r3Rot * pinsMaster->pr3GetPresence().r3Rot;
	}
	
	//*****************************************************************************************
	void CMagnetPair::SetRot(const CRotate3<>& r3_rot)
	{
		Assert(pinsMaster);
		pr3MagMaster.r3Rot = r3_rot / pinsMaster->pr3GetPresence().r3Rot;
	}

	//*****************************************************************************************
	float CMagnetPair::fGetScale() const
	{
		Assert(pinsMaster);
		return pr3MagMaster.rScale * pinsMaster->fGetScale();
	}

	//*****************************************************************************************
	void CMagnetPair::SetScale(float f_new_scale)
	{
		Assert(pinsMaster);
		pr3MagMaster.rScale = f_new_scale / pinsMaster->fGetScale();
	}

	//*****************************************************************************************
	void CMagnetPair::Move(const CPlacement3<>& p3_new, CEntity*)
	{
		// Simply move the presence, relative to master.
		// No interaction with world dbase.
		Assert(pinsMaster);
		static_cast<CPlacement3<>&>(pr3MagMaster) = CPresence3<>(p3_new) / pinsMaster->pr3GetPresence();
	}

	//*****************************************************************************************
	int CMagnetPair::iSizeOf() const
	{
		return sizeof(*this);
	}

	//*****************************************************************************************
	char* CMagnetPair::pcSave(char* pc) const
	{
		// Save pointers.
		pc = pcSaveInstancePointer(pc, pinsMaster);
		pc = pcSaveInstancePointer(pc, pinsSlave);

		// Save copy of magnet data.
		pc = pcSaveT(pc, *pmagData);

		// Save the master-relative presence.
		pc = pr3MagMaster.pcSave(pc);

		// And the world hinge rotation.
		pc = r3Orig.pcSave(pc);

		return pc;
	}

	//*****************************************************************************************
	const char* CMagnetPair::pcLoad(const char* pc)
	{
		// Load pointers.
		pc = pcLoadInstancePointer(pc, &pinsMaster);
		pc = pcLoadInstancePointer(pc, &pinsSlave);

		// Load copy of magnet data.
		CMagnet mag;
		pc = pcLoadT(pc, &mag);

		// Still instancing...
		pmagData = CMagnet::pmagFindShared(mag);

		// Load the magnet presence.
		pc = pr3MagMaster.pcLoad(pc);

		// Load the world hinge rotation.
		pc = r3Orig.pcLoad(pc);

		return pc;
	}

//
// Typedefs: cannot be placed in NMagnetSystem, or VC 4.2 barfs.
//
typedef std::multimap<const CInstance* const, CMagnetPair*, std::less<const CInstance* const> >   TMagnetTable;
typedef std::pair<const CInstance* const, CMagnetPair*>							TMagnetTableEntry;
typedef std::pair<TMagnetTable::iterator, TMagnetTable::iterator>	TMagnetTableMatches;

//**********************************************************************************************
//
namespace NMagnetSystem
{
	TMagnetTable	mtSlaveLookup;		// A table to find magnets pairs from the slave side.
										// There can be only one master per slave.
	TMagnetTable	mtMasterLookup;		// A table to find magnets pairs from the master side.
	
	//*****************************************************************************************
	void AddMagnetPair
	(
		CInstance* pins_master, CInstance* pins_slave, const CMagnet* pmag, 
		const CPresence3<>& par3_mag_master, const CRotate3<>& r3_hinge_rel
	)
	{
		Assert(pins_master);

		// Convert magneting to immobile objects to magneting to space (null attachment instance).
		if (pins_slave && !pins_slave->pphiGetPhysicsInfo()->bIsMovable())
			pins_slave = 0;
		if (!pins_master->pphiGetPhysicsInfo()->bIsMovable())
		{
			if (!pins_slave)
				// If both are immobile, it's pointless to magnet them.
				return;

			// Swap them, as the master is always the movable instance.
			pins_master = pins_slave;
			pins_slave = 0;
		}

		//
		// Enforce a single-master model.  We magnet to the ultimate master of the slave,
		// so that the slave doesn't end up with more than one master.
		//
		
		pins_slave = pinsFindMaster(pins_slave);

		CMagnetPair* pmp = new CMagnetPair(pins_master, pins_slave, pmag, par3_mag_master, r3_hinge_rel);

		// Add to lookup tables.
		if (pins_slave)
			mtSlaveLookup.insert( TMagnetTableEntry(pins_slave, pmp) );
		mtMasterLookup.insert( TMagnetTableEntry(pins_master, pmp) );

#if VER_DEBUG
		// Get current list of attached magnets, to check for looping.
		std::list<CMagnetPair*> lspmp;
		GetAttachedMagnets(pins_master, &lspmp);
#endif

		if (pmag->setFlags[emfINTANGIBLE])
		{
			// If the magnet is intangible, merely deactivate the slave, to
			// avoid wasteful deact/act calls, which do not preserve velocity on magneting.
			if (pins_slave)
				pins_slave->PhysicsDeactivate();
		}
		else if (pins_master->pphiGetPhysicsInfo()->bIsActive(pins_master) ||
				(pins_slave && pins_slave->pphiGetPhysicsInfo()->bIsActive(pins_slave)))
		{
			// If either is active, deactivate both, then reactivate the compound master.
			pins_master->PhysicsDeactivate();
			if (pins_slave)
				pins_slave->PhysicsDeactivate();

			pins_master->PhysicsActivate();
		}
	}

	//*****************************************************************************************
	void RemoveMagnetPair(CInstance* pins_master, CInstance* pins_slave, CSet<EMagnetFlag> setemf)
	{
		Assert(pins_master);

		// Find master of the slave, up to but not including pins_master.
		pins_slave = pinsFindMaster(pins_slave, pins_master);

		// Get all master magnet entries for pins_master.
		// Go through the matches and find the magnet pair that includes pins_slave.
		TMagnetTableMatches matches = mtMasterLookup.equal_range(pins_master);
		for (TMagnetTable::iterator itpmp = matches.first; itpmp != matches.second; itpmp++)
		{
			// (*itpmp) is a pair, <key, value>   OR  <CInstance*, CMagnetPair*>
			CMagnetPair* pmp = (*itpmp).second;

			Assert(pmp->pinsMaster == pins_master);

			// If slave matches, and all requested flags are present.
			if (pmp->pinsSlave == pins_slave && (!setemf || pmp->setemfFlags() & setemf))
			{
				// Found it!

				// Get master's current velocity, and deactivate.
				CPlacement3<> p3_vel = pins_master->pphiGetPhysicsInfo()->p3GetVelocity(pins_master);
				bool b_active = pins_master->pphiGetPhysicsInfo()->bIsActive(pins_master);

				if (b_active)
				{
					// If the magnet is intangible, then so is the slave, and we don't need
					// to deactivate or reactivate either object.
					if (pmp->setemfFlags()[emfINTANGIBLE])
						b_active = false;
					else
						pins_master->PhysicsDeactivate();
				}

				// Ditch the entry.
				mtMasterLookup.erase(itpmp);

				// Get the entry in slave table as well.
				if (pins_slave)
				{
					Assert(mtSlaveLookup.count(pins_slave) == 1);

					// Ditch the entry.
					mtSlaveLookup.erase(pins_slave);
				}
				
				// Kill the magnet pair.
				delete pmp;

				if (b_active)
				{
					// Re-activate parts, with original velocity.
					pins_master->pphiGetPhysicsInfo()->Activate(pins_master, false, p3_vel);
					if (pins_slave)
					{
						pins_slave->pphiGetPhysicsInfo()->Activate(pins_slave, false, p3_vel);
#if bIGNORE_COLLISIONS
						// Ignore immediate collisions between these newly freed objects.
						pphSystem->IgnoreCollisions(pins_master, pins_slave, true);
#endif
					}
				}

				// If we found it, remove any further world magnets.
				// Must re-invoke rather than looping, because multi-set has changed.
				if (!pins_slave)
					RemoveMagnetPair(pins_master, pins_slave, setemf);
				break;
			}
		}
	}

	//*****************************************************************************************
	void RemoveMagnets(CInstance* pins, bool b_remove_masters, bool b_remove_slaves)
	{
		Assert(pins);

		// Deactivate from physics.
		CPlacement3<> p3_vel = pins->pphiGetPhysicsInfo()->p3GetVelocity(pins);
		bool b_active = pins->pphiGetPhysicsInfo()->bIsActive(pins);
		if (b_active)
			pins->PhysicsDeactivate();

		TMagnetTable::iterator i;

#if VER_DEBUG
		bool b_found_it = false;
#endif

		if (b_remove_slaves)
		{
			// Find any master attachments.
			// We need to re-find the entire way through
			// because the range method becomes invalid as soon as we erase a node in the tree.
			while ((i = mtMasterLookup.find(pins)) != mtMasterLookup.end())
			{
				// Get the partner, and find in other list.
				CInstance* pins_slave = (*i).second->pinsSlave;

				if (pins_slave)
				{
					TMagnetTableMatches matches2 = mtSlaveLookup.equal_range(pins_slave);
					for (TMagnetTable::iterator i2 = matches2.first; i2 != matches2.second; ++i2)
					{
						if ((*i2).second == (*i).second)
						{
							// Kill the magnet pair.
							delete (*i).second;
							mtSlaveLookup.erase(i2);
					#if VER_DEBUG
							b_found_it = true;
					#endif
							break;
						}
					}

					Assert(b_found_it);
				}

				mtMasterLookup.erase(i);

				// Reactivate partner, with old velocity.
				if (b_active && pins_slave)
				{
					pins_slave->pphiGetPhysicsInfo()->Activate(pins_slave, false, p3_vel);
#if bIGNORE_COLLISIONS

					// Ignore immediate collisions between these newly freed objects.
					pphSystem->IgnoreCollisions(pins, pins_slave, true);
#endif
				}
			}
		}

#if VER_DEBUG
		b_found_it = true;
#endif

		if (b_remove_masters)
		{
			// Find any slave attachments.
			while ((i = mtSlaveLookup.find(pins)) != mtSlaveLookup.end())
			{
				// Get the partner, and find in other list.
				CInstance* pins_master = (*i).second->pinsMaster;
				
				TMagnetTableMatches matches2 = mtMasterLookup.equal_range(pins_master);
				for (TMagnetTable::iterator i2 = matches2.first; i2 != matches2.second; ++i2)
				{
					if ((*i2).second == (*i).second)
					{
						// Kill the magnet pair.
						delete (*i).second;
						mtMasterLookup.erase(i2);
				#if VER_DEBUG
						b_found_it = true;
				#endif
						break;
					}
				}

				Assert(b_found_it);
				mtSlaveLookup.erase(i);

				// Reactivate partner.
				if (b_active)
				{
					pins_master->pphiGetPhysicsInfo()->Activate(pins_master, false, p3_vel);
#if bIGNORE_COLLISIONS

					// Ignore immediate collisions between these newly freed objects.
					pphSystem->IgnoreCollisions(pins, pins_master, true);
#endif
				}
			}
		}

		// Reactivate original.
		if (b_active)
			pins->pphiGetPhysicsInfo()->Activate(pins, false, p3_vel);
	}

	//*****************************************************************************************
	void RemoveAllMagnets()
	{
		// Clean up the magnet pairs!
		// Iterate master table (it contains all magnet pairs).
		TMagnetTable::const_iterator i;
		for (i = mtMasterLookup.begin(); i != mtMasterLookup.end(); ++i)
		{
			delete (*i).second;
		}

		mtSlaveLookup.erase(mtSlaveLookup.begin(), mtSlaveLookup.end());
		mtMasterLookup.erase(mtMasterLookup.begin(), mtMasterLookup.end());
	}

	//*****************************************************************************************
	void SetFrozen(CInstance* pins, bool b_frozen)
	{
		if (b_frozen)
		{
			// Magnet to world, if there isn't one already.
			if (!bIsFrozen(pins))
				NMagnetSystem::AddMagnetPair(pins, 0, CMagnet::pmagFindShared(CMagnet()));
		}
		else
		{
			// Demagnet from world.
			NMagnetSystem::RemoveMagnetPair(pins, 0, Set(emfFREEZE));
		}
	}

	//*****************************************************************************************
	bool bIsFrozen(const CInstance* pins)
	{
		// Search for a freeze magnet (world magnet with no freedom).
		return NMagnetSystem::pmpFindMagnet(pins, Set(emfFREEZE)) != 0;
	}

	//*****************************************************************************************
	bool bIsIntangible(const CInstance* pins)
	{
		// Search master magnets.
		TMagnetTableMatches matches = mtSlaveLookup.equal_range(pins);

		for (TMagnetTable::iterator i = matches.first; i != matches.second; i++)
		{
			if ((*i).second->setemfFlags()[emfINTANGIBLE])
				// Was enslaved to an intangible magnet.
				return true;
		}

		return false;
	}

	//*****************************************************************************************
	float fAngleRotated(const CInstance* pins)
	{
		// Search for a hinge magnet.
		const CMagnetPair* pmp = NMagnetSystem::pmpFindMagnet(pins, Set(emfHINGE));
		if (!pmp)
			return 0.0;

		// Find this object's rotation relative to original.
		CRotate3<> r3_rel = pins->p3GetPlacement().r3Rot / pmp->r3Orig;

		// We find the sin of half this angle by dotting the quaternion vector with
		// the axis of rotation.
		float f_sin_half = r3_rel.v3S * pmp->pmagData->d3Axis();
		return 2.0 * asin(f_sin_half);
	}

	//*****************************************************************************************
	CInstance* pinsFindMaster(CInstance* pins, CInstance* pins_super_master)
	{
		// Infinite loop detection.
		int i_count_dracula = 0;

		while (pins)
		{
			// Search for master instance.
			CInstance* pins_master = 0;
			TMagnetTableMatches matches = mtSlaveLookup.equal_range(pins);

			for (TMagnetTable::iterator i = matches.first; i != matches.second; i++)
			{
				// Found a master magnet.  There can be only one.
				Assert((*i).second->pinsMaster);
				Assert(!pins_master);
				pins_master = (*i).second->pinsMaster;
				#if !VER_DEBUG
					// Don't bother checking for extras.
					break;
				#endif
			}

			if (!pins_master)
				break;
			if (pins_master == pins_super_master)
				break;
			pins = pins_master;

			// Detect infinite recursion.
			Assert(++i_count_dracula < 50);
		}

		return pins;
	}

	//*****************************************************************************************
	const CMagnetPair* pmpFindMagnet(const CInstance* pins_master, const CInstance* pins_slave, CSet<EMagnetFlag> setemf)
	{
		TMagnetTableMatches matches = mtMasterLookup.equal_range(pins_master);
		for (TMagnetTable::iterator i = matches.first; i != matches.second; i++)
		{
			if ((*i).second->pinsSlave == pins_slave)
			{
				// See if all flags match.
				if (!setemf || ((*i).second->setemfFlags() & setemf))
				{
					return (*i).second;
				}
			}
		}
		return 0;
	}

	//*****************************************************************************************
	void GetAttachedMagnets(CInstance* pins, std::list<CMagnetPair*>* plsmp)
	{
		static CMagnetPair mpMaster(0, 0, 0);

		if (!plsmp->size())
		{
			// This is the top call of this function.
			// Find the instance's ultimate master, and insert it in the dummy magnet pair.
			mpMaster.pinsSlave = pins = pinsFindMaster(pins);
			plsmp->push_back(&mpMaster);
		}

		// Attach slaves, recursively.
		TMagnetTableMatches matches = mtMasterLookup.equal_range(pins);
		for (TMagnetTable::iterator i = matches.first; i != matches.second; i++)
		{
			// Add magnet to list.
			plsmp->push_back((*i).second);

			// Sanity checking (infinite recursion).
			Assert(plsmp->size() < 50);

			// Add this slave's slaves, if any.
			GetAttachedMagnets((*i).second->pinsSlave, plsmp);
		}
	}

	//*****************************************************************************************
	void GetAttachedMagnetsSlaves(CInstance* pins, CDArray<CMagnetPair*>* pdamp)
	{
		static CMagnetPair mpMaster(0, 0, 0);

		if (!pdamp->size())
		{
			// This is the top call of this function.
			// Find the instance's ultimate master, and insert it in the dummy magnet pair.
			mpMaster.pinsSlave = pins;
			(*pdamp) << &mpMaster;
		}

		// Attach slaves, recursively.
		TMagnetTableMatches matches = mtMasterLookup.equal_range(pins);
		for (TMagnetTable::iterator i = matches.first; i != matches.second; i++)
		{
			// Add magnet to list.
			(*pdamp) << (*i).second;

			// Sanity checking (infinite recursion).
			Assert(pdamp->size() < 50);

			// Add this slave's slaves, if any.
			GetAttachedMagnetsSlaves((*i).second->pinsSlave, pdamp);
		}
	}

	//*****************************************************************************************
	void QueryMagnets(TPartitionList& rlsple, const CPartition* ppart_query)
	{
#if VER_PARTITION_MAGNETS
		// Iterate all magnet pairs, add those in ppart to list.
		forall (mtMasterLookup, TMagnetTable, itpmp)
		{
			if (!ppart_query || ppart_query->esfSideOf((*itpmp).second) & esfINSIDE)
			{
				SPartitionListElement ple = {(*itpmp).second, esfINSIDE};
				rlsple.push_back(ple);
			}
		}
#endif
	}

	//*****************************************************************************************
	void DrawMagnets(CInstance* pins, CDraw& draw, CCamera& cam)
	{
		// Set up some coordinate frame vectors.
		CPresence3<> pr3 = pins->pr3Presence();

		// Set up some coordinate frame vectors.
		const TReal r_frame_length = .03;
		const TReal r_limit_length = .10;
		const CVector3<> v3_x = CVector3<>(r_frame_length, 0, 0);
		const CVector3<> v3_y = CVector3<>(0, r_frame_length, 0);		
		const CVector3<> v3_z = CVector3<>(0, 0, r_frame_length);

		CTransform3<> tf3_screen = cam.tf3ToHomogeneousScreen();

		// Draw just magnets that attach to pins as master.
		// Lookup pins in master list.
		TMagnetTableMatches matches = mtMasterLookup.equal_range(pins);

		TMagnetTable::iterator i;

		for (i = matches.first; i != matches.second; i++)
		{
			const CMagnetPair* pmp = (*i).second;

			// Make sure that we have a good magnet pair.
			Assert(pmp->pinsMaster == pins);

			// Transform the origin, please.
			CTransform3<> tf3_mag_screen = CPlacement3<>(pmp->pr3Presence()) * tf3_screen;
			CTransform3<> tf3_mag_pos_screen = CPlacement3<>(pmp->v3Pos()) * tf3_screen;

			// Draw the frame for the master magnet.

			// X axis.
			draw.Colour(CColour(1.0, 0.0, 0.0));
			draw.Line3D((v3_x * -0.5) * tf3_mag_screen, v3_x * tf3_mag_screen);

			// Y axis.
			draw.Colour(CColour(0.0, 1.0, 0.0));
			draw.Line3D((v3_y * -0.5) * tf3_mag_screen, v3_y * tf3_mag_screen);

			// Z axis.
			draw.Colour(CColour(0.0, 0.0, 1.0));
			draw.Line3D((v3_z * -0.5) * tf3_mag_screen, v3_z * tf3_mag_screen);

			if (pmp->setemfFlags()[emfLIMITS])
			{
				// Draw limits of rotation.
				CVector3<> v3_min = pmp->pmagData->d3Min() * r_limit_length;
				CVector3<> v3_max = pmp->pmagData->d3Max() * r_limit_length;

				draw.Colour(CColour(1.0, 1.0, 0.0));
				draw.Line3D(tf3_mag_screen.v3Pos, v3_min * tf3_mag_pos_screen);

				draw.Colour(CColour(0.7, 0.7, 0.0));
				draw.Line3D(tf3_mag_screen.v3Pos, v3_max * tf3_mag_pos_screen);
			}
		}
	}

	//*****************************************************************************************
	char* pcSave(char* pc)
	{
		// Save a spot in the buffer for the number of magnets.
		int * pi_num_magnets = (int*) pc;
		pc += sizeof(int);
		
		// Now save the magnets.  Go through one of the maps.
		int i_num_magnets = 0;

		// Iterate master table (it contains all magnet pairs).
		TMagnetTable::const_iterator i;
		for (i = mtMasterLookup.begin(); i != mtMasterLookup.end(); ++i)
		{
			++i_num_magnets;
			pc = (*i).second->pcSave(pc);
		}

		// Store the table size.
		*pi_num_magnets = i_num_magnets;

		return pc;
	}

	//*****************************************************************************************
	const char* pcLoad(const char* pc)
	{
		int i_num_magnets;
		pc = pcLoadT(pc, &i_num_magnets);

		CMagnetPair mp_temp(0, 0, 0);

		// Kill all old magnet pairs.
		RemoveAllMagnets();
	
		// For each magnet pair, load it and compare it to the existing tables.
		for (--i_num_magnets ; i_num_magnets >= 0; --i_num_magnets)
		{
			pc = mp_temp.pcLoad(pc);
			CMagnetPair* pmp = new CMagnetPair(mp_temp);

			// We have a magnet.  
			// Add to lookup tables.
			Assert(mp_temp.pinsMaster);
			mtMasterLookup.insert( TMagnetTableEntry(mp_temp.pinsMaster, pmp ));

			if (mp_temp.pinsSlave)
				mtSlaveLookup.insert ( TMagnetTableEntry(mp_temp.pinsSlave,  pmp ));
		}

		return pc;
	}
};
