/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: The implementation of Animal.hpp.
 *
 * Bugs:
 *
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Animate.cpp                                          $
 * 
 * 110   10/05/98 3:57a Sblack
 * Rescaled pushes to work with new gun masters and new dino "hollywood death."
 * 
 * 109   98/10/04 2:36 Speter
 * Added UpdatePhysicsHealth(). Clamped to 100.
 * 
 * 108   10/03/98 6:01a Agrant
 * load dino feet the same way in all versions
 * 
 * 107   10/02/98 11:30p Agrant
 * no blood at all for humans at gore 0 and 1
 * 
 * 106   10/02/98 2:38a Agrant
 * don't take multiplied damage on the player's foot
 * 
 * 105   10/01/98 6:49p Mlange
 * Improved collision message stats.
 * 
 * 104   10/01/98 2:56p Agrant
 * more distinct gore
 * 
 * 103   10/01/98 12:28p Sblack
 * 
 * 102   9/30/98 3:28p Agrant
 * Added commented out version of exciting dino death
 * More blood splats when not using gun
 * 
 * 101   9/29/98 12:06a Agrant
 * save tranq points
 * 
 * 100   98/09/28 2:47 Speter
 * Player no longer bleeds when dead, or on foot damage.
 * 
 *********************************************************************************************/

//
// Includes.
//
#include "Common.hpp"
#include "Animate.hpp"
#include "Game/DesignDaemon/Gun.hpp"
#include "Game/DesignDaemon/BloodSplat.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgCollision.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgTrigger.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPhysicsReq.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/ParticleGen.hpp"
#include "Lib/Physics/PhysicsSystem.hpp"
#include "Lib/Physics/InfoBox.hpp"
#include "Lib/Physics/InfoSkeleton.hpp"
#include "Lib/Physics/pelvis_def.h"
#include "Lib/Physics/Magnet.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "Lib/GeomDBase/Skeleton.hpp"
#include "Lib/GeomDBase/RayCast.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Lib\EntityDBase\Query\QTerrain.hpp"

//
// Constants.
//

// Ratio between force and the number of hitpoints delivered to an animate creature.
// Constants set up to ensure falling damage occurs between 3 and 6 m.
const float fCOLLISION_DAMAGE		= 0.22f;	// Each Joule of collision energy does this much damage.
const float fCOLLISION_BUFFER		= 1.00f;	// Ratio of ignored (harmless) to lethal collision damage.
const float fCOLLISION_REGEN_TIME	= 0.25f;	// Sliding time period over which we integrate collisions.
												// Determines collide point regeneration rate.
const float fBIOMODEL_ADJUST_HACK	= 0.08f;	// Scale applied to damage to/from biomodel boxes.

static const char* astrBoxNames[] =
{
	"Body", "RFoot", "Hand", "Head", "Tail", "LFoot", "FRightFoot", "FLeftFoot",
};

namespace
{
	// Maximum distance from entry wound to terrain for creating blood splat.
	const float fMaxBloodSprayDistance = 3;

	// Scale factors applied to animate damage points to determine size of blood splat/streak;
	const float fBloodSplatSizeDamageScale = 1.0f / 40.0f;
	const float fBloodSplatStreakDamageScale = 1.0f / 40.0f;

	// Random number generator used for blood splats.
	CRandom rndBloodSplats;

	TSoundMaterial smatBlood = matHashIdentifier("Blood");
	TSoundMaterial smatBleed = matHashIdentifier("Bleed");

	//******************************************************************************************
	//
	class CCreateParticlesCycleBleed: public CCreateParticlesCycle
	//
	// Specifies the time parameters for animal bleeding.
	// The basic particle parameters are unspecified, and will be gotten from NParticleMap.
	//
	//**********************************
	{
	public:

		//**************************************************************************************
		CCreateParticlesCycleBleed()
		{
		}

		//**************************************************************************************
		CCreateParticlesCycleBleed(const CCreateParticles& crt)
			: CCreateParticlesCycle(crt)
		{
			// Time parameters.
			sPeriod			= 1.0;
			sDuty			= 0.5;
			fDecay			= 0.6;
			sLifetime		= 6.0;
		}
	};
}

//
// Class implementations.
//

//*********************************************************************************************
//
class CWound
//
// Prefix: wnd
//
// Maintains a raw, bleeding wound, visible on the animal.
//
//**************************************
{
private:

	CParticleCycle	pclBleed;			// The spurting source of blood.
	CBoundaryBox*	pbbBox;				// Which box was hit.
	CPlacement3<>	p3Rel;				// Wound placement relative to box.

public:

	//*********************************************************************************************
	CWound()
	{
	}

	//*********************************************************************************************
	CWound
	(
		float f_damage,					// Amount of damage for this wound.
		CBoundaryBox* pbb,				// Particular boundary box hit.
		const CPlacement3<>& p3_rel,	// Placement of wound relative to box.
		const CCreateParticlesCycle& crtc
	)
		: pclBleed(crtc, f_damage),
		  pbbBox(pbb),
		  p3Rel(p3_rel)
	{
		Assert(pbb);
	}

	//*********************************************************************************************
	//
	void Step
	(
		TSec s_time
	)
	//
	//**********************************
	{
		// Position the wound.
		pclBleed.SetPlacement(p3Rel * pbbBox->p3GetPlacement());

		// Let it spurt.
		pclBleed.Step(s_time);
	}

	//*********************************************************************************************
	//
	bool bIsDone() const
	//
	//
	//**********************************
	{
		return pclBleed.bIsDone();
	}

};

//*********************************************************************************************
//
// CAnimate implementation.
//

	int CAnimate::iGoreLevel = 1;			// Realistic gore.

	//*****************************************************************************************
	CAnimate::CAnimate()
		: fHitPoints(100.0f), fMaxHitPoints(100.0f), fRegenerationRate(1.0f),
		paWounds(10), iNumWounds(0)
	{
		sriBones.bvbVolume = CBoundVolBox(1, 1, 1);
		apbbBoundaryBoxes.Fill(0);

		Init();

		// Register this entity with the message types it needs to receive.
		      CMessageStep::RegisterRecipient(this);
		 CMessageCollision::RegisterRecipient(this);
		CMessagePhysicsReq::RegisterRecipient(this);
	}

	//*****************************************************************************************
	CAnimate::CAnimate(const SInit& initins) 
		: CEntity(initins), fHitPoints(100.0f), fMaxHitPoints(100.0f), fRegenerationRate(1.0f),
		paWounds(10), iNumWounds(0)
	{
		sriBones.bvbVolume = CBoundVolBox(1, 1, 1);
		apbbBoundaryBoxes.Fill(0);

		Init();

		fReallyDead = 0;
		fDieRate = 1.0f;

		// Register this entity with the message types it needs to receive.
		      CMessageStep::RegisterRecipient(this);
		 CMessageCollision::RegisterRecipient(this);
		CMessagePhysicsReq::RegisterRecipient(this);
	}

	//*****************************************************************************************
	CAnimate::CAnimate
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CEntity(pgon, pload, h_object, pvtable, pinfo),
		paWounds(10), iNumWounds(0)
	{
		apbbBoundaryBoxes.Fill(0);

		ParseProps(pgon, pload, h_object, pvtable, pinfo);

		// Register this entity with the message types it needs to receive.
		      CMessageStep::RegisterRecipient(this);
		 CMessageCollision::RegisterRecipient(this);
		CMessagePhysicsReq::RegisterRecipient(this);
	}

	//*****************************************************************************************
	CAnimate::~CAnimate()
	{
		CMessagePhysicsReq::UnregisterRecipient(this);
		 CMessageCollision::UnregisterRecipient(this);
		      CMessageStep::UnregisterRecipient(this);

		delete[] (CWound*)paWounds;
	}

	//*****************************************************************************************
	void CAnimate::Init()
	{
		// Set collide point data from hit point data.
		fCollidePoints = fCollideResetPoints = fMaxCollidePoints = 
			fMaxHitPoints * fCOLLISION_BUFFER;
		fCollideRegenerationRate = fMaxCollidePoints / fCOLLISION_REGEN_TIME;

		// Shared code between constructors.
		// Copy our joints from the biomesh.
		if (!sriBones.patf3JointTransforms.size())
		{
			if (prdtGetRenderInfo())
			{
				rptr_const<CBioMesh> pbm = prdtGetRenderInfo()->rpbmCast();
				if (pbm)
				{
					new(&sriBones) CSkeletonRenderInfo(pbm->patf3JointsOrig.paDup());

					// Set initial bounding volume from the mesh.
					sriBones.bvbVolume = pbm->bvbVolume;
				}
			}
		}
		bSkeletonUpdated = true;

		// Perform additional physics initialisation.
		const CPhysicsInfoSkeleton* pphis = pphiGetPhysicsInfo()->ppisCast();
		if (pphis)
			pphis->Init(this);

		// No one has killed us yet.
		pinsKiller = 0;
	}

	//*****************************************************************************************
	void CAnimate::ParseProps
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	)
	{
		Assert(ebbBODY + esBody == esBody);
		Assert(ebbFOOT + esBody == esFoot);
		Assert(ebbHAND + esBody == esHand);
		Assert(ebbHEAD + esBody == esHead);
		Assert(ebbTAIL + esBody == esTail);

		fHitPoints = 100.0f;
		fMaxHitPoints = 100.0f;
		fRegenerationRate = 1.0f;
		fReallyDead = -20.0f;
		fDieRate = 1.0f;
		fCriticalHit = fMaxHitPoints * .3;

		apbbBoundaryBoxes.Fill(0);

		// Snag the names of the head, body, and tail boxes.
		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_object)
		{
			bFILL_FLOAT(fHitPoints, esHitPoints);

			fMaxHitPoints = fHitPoints;
			bFILL_FLOAT(fMaxHitPoints, esMaxHitPoints);

			bFILL_FLOAT(fRegenerationRate, esRegeneration);
			fMaxTranqPoints = fMaxHitPoints;
			fTranqPoints	= fHitPoints;
			fTranqRegenerationRate = fRegenerationRate;

			fReallyDead = -0.2 * fMaxHitPoints;
			bFILL_FLOAT(fReallyDead, esReallyDie);

			bFILL_FLOAT(fDieRate, esDieRate);

			fCriticalHit = fMaxHitPoints * .3;
			bFILL_FLOAT(fCriticalHit, esCriticalHit);

			bFILL_INT(iTeam, esTeam);

			const CEasyString* pestr = 0;

			int i;
			for (i = ebbBODY; i < ebbEND; ++i)
			{
				ESymbol es = ESymbol(i + esBody);

				if (bFILL_pEASYSTRING(pestr, es))
				{
					CGroffObjectName* pgon_box = pload->goiInfo.pgonFindObject(pestr->strData());
					
					if (pgon_box)
					{
						CBoundaryBox* pbb = new CBoundaryBox(pgon_box, pload, pgon_box->hAttributeHandle, pvtable, 0);

						apbbBoundaryBoxes[i] = pbb;
						pbb->paniAnimate = this;
						pbb->ebbElement = EBoundaryBoxes(i);

						char buffer[256];
						sprintf(buffer, 
							"%s+%s", 
							pgon_box->strObjectName,
							pgon->strObjectName);

						pbb->SetInstanceName(buffer);

						//
						// Move to animal's centre, so that instanced animals have their boxes near them,
						// rather than all near the original animal.
						// The boxes will be moved to the proper place on activation anyway.
						//
						pbb->SetPlacement(p3GetPlacement());
						wWorld.Add(pbb);
					}
					else
					{
#if VER_TEST
					char str_buffer[256];
					sprintf(str_buffer, 
							"%s\n\nMissing dino box:\n (%s needs %s)", 
							__FILE__, 
							pgon->strObjectName,
							pestr->strData());

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
#endif
					}
				}
			}
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		// Check for biomesh again.
		Init();
	}

	//*****************************************************************************************
	void CAnimate::AddBoundaryBox(EBoundaryBoxes ebb, const CVector3<>& v3_size, const CVector3<>& v3_offset,
		float f_mass, TSoundMaterial smat, float f_damage, float f_armour, const char* str_name)
	{
		Assert(ebb < ebbEND);
		if (!apbbBoundaryBoxes[(int)ebb])
		{
			// No longer an option.  It's specified in data or we have an error!
			// AlwaysAssert(false);

			// Generate unique instance name.
			char str_box[256];
			if (!str_name)
			{
				// To make final and release and debug all the same, use the magic 
				// name below....
//				const char* str_ani = strGetInstanceName();
//				if (str_ani)
//					sprintf(str_box, "%s+%s", astrBoxNames[(int)ebb], str_ani);
//				else
					sprintf(str_box, "%s+%X", astrBoxNames[(int)ebb], uHandle);
				str_name = str_box;
			}

			CPhysicsInfoBox phib
			(
				CBoundVolBox(v3_size),
				SPhysicsData(Set(epfTANGIBLE) + epfMOVEABLE, smat),

				// Set mass directly, and pass scale of 1 (CBoundaryBox created at this scale).
				f_mass, 1.0
			);
			phib.fDamage = f_damage;
			phib.fArmour = f_armour;

			apbbBoundaryBoxes[(int)ebb] = new CBoundaryBox
			(
				this,
				ebb,
				v3_offset,
				phib,
				str_name
			);
			wWorld.Add(apbbBoundaryBoxes[(int)ebb]);
		}
	}

	//*****************************************************************************************
	float CAnimate::fWieldDamage(const CInstance* pins_weapon, float f_coll_damage) const
	{
		// Default implementation; no special behaviour.
		return f_coll_damage;
	}

	//*****************************************************************************************
	bool CAnimate::bCanHaveChildren()
	{
		return false;
	}

	//*****************************************************************************************
	const CBoundVol* CAnimate::pbvBoundingVol() const
	{
		return &sriBones.bvbVolume;
	}

	//******************************************************************************************
	void CAnimate::Move(const CPlacement3<>& p3_new, CEntity* pet_sender)
	{
		CPlacement3<> p3_prev = p3GetPlacement();

		CEntity::Move(p3_new, pet_sender);

		// If we are being moved non-physically, drag our bio-boxes to same relative locations.
		if (pet_sender != pphSystem)
		{
			for (int i = 0; i < ebbEND; ++i)
			{
				if (apbbBoundaryBoxes[i])
				{
					CPlacement3<> p3_rel = apbbBoundaryBoxes[i]->p3GetPlacement() / p3_prev;
					apbbBoundaryBoxes[i]->Move(p3_rel * p3GetPlacement(), pet_sender);
				}
			}
		}
	}

	//*****************************************************************************************
	void CAnimate::UpdatePhysicsHealth()
	{
		// Set physics health!
		float f_health = MinMax(100.0f * fHitPoints / fMaxHitPoints, 0.0f, 100.0f);
		pphSystem->SetHealth(this, f_health);
	}

	//*****************************************************************************************
	void CAnimate::Process(const CMessageStep& msgstep)
	{
		// Mainly, animates handle regeneration and degradation.

		// Are we alive and fairly well?
		if (fHitPoints > 0.0)
		{
			// Yes!  Regenerate!
			fHitPoints		+= msgstep.sStep * fRegenerationRate;
			if (fHitPoints > fMaxHitPoints)
				fHitPoints = fMaxHitPoints;

			fTranqPoints	+= msgstep.sStep * fTranqRegenerationRate;
			if (fTranqPoints > fMaxTranqPoints)
				fTranqPoints = fMaxTranqPoints;
		}
		else
		{
			// No!  Degrade!
			fHitPoints -= msgstep.sStep * fDieRate;
			if (fHitPoints < fReallyDead)
			{
				fHitPoints = fReallyDead;
			}
		}

		UpdatePhysicsHealth();

		// Handle collision buffering regardless of livingness.
		fCollideResetPoints	+= msgstep.sStep * fCollideRegenerationRate;
		if (fCollideResetPoints >= fMaxCollidePoints)
		{
			fCollideResetPoints = fMaxCollidePoints;
			fCollidePoints = fMaxCollidePoints;
		}

		// Reset the protection against double duty collide damage 
		fCollideDamageThisFrame = 0.0f;

		// Tend to our wounds.
		int i_valid = 0;
		for (int i = 0; i < iNumWounds; i++)
		{
			// Process each active wound, and purge inactive ones.
			if (!paWounds[i].bIsDone())
			{
				paWounds[i].Step(msgstep.sStep);
				paWounds[i_valid] = paWounds[i];
				i_valid++;
			}
		}

		iNumWounds = i_valid;
	}
	
	extern CProfileStat psCollisionMsgAnimate;

	//******************************************************************************************
	void CAnimate::Process(const CMessageCollision& msgcoll)
	{
		CTimeBlock tmb(&psCollisionMsgAnimate);

		// Do not listen to own collision messages.  Animates only take damage on their bio boxes!
		if (msgcoll.pins1 == this || msgcoll.pins2 == this)
			return;

		// Only process message for this object or its boundary boxes.
		CInstance* pins_us = 0;	// the thing that is "us" for this function call
		CBoundaryBox* pbb_us = 0;

#define bIN_COLLISION(pins, col) (((CInstance*)pins) == col.pins1 || ((CInstance*)pins) == col.pins2)
		if (bIN_COLLISION(this, msgcoll))
		{
			pins_us = this;
		}
		else 
		{
			int i;
			for (i = 0; i < ebbEND; ++i)
			{
				if (apbbBoundaryBoxes[i] && bIN_COLLISION(apbbBoundaryBoxes[i], msgcoll))
				{
					pins_us = pbb_us = apbbBoundaryBoxes[i];
					break;
				}
			}
		}

		// Is it us?
		if (!pins_us)
			// No! Not our problem.  Ignore the collision.
			return;

		// Calculate hit points based on the type and force of the collision.
		float f_damage = fCalculateHitPoints(pins_us, msgcoll);
		float f_tranq = 0.0f;

		// Handle different types of damage differently.
		CInstance *pins_other = (pins_us == msgcoll.pins1) ? msgcoll.pins2 : msgcoll.pins1;
		CGun* pgun = ptCast<CGun>(msgcoll.petGetSender());

		if (!pgun)
		{
			// Check for attack damage.
			// Who is the other guy?
			CBoundaryBox* pbb_other = ptCast<CBoundaryBox>(pins_other);

			if (pbb_other && pbb_other->paniAnimate->bDead())
				pbb_other = 0;

			if (pbb_other)
			{
				// An animate.  Is he on my team?
				if (ptCast<CBoundaryBox>(pins_other))
				{
					if (iTeam == ((CBoundaryBox*)pins_other)->paniAnimate->iTeam)
					{
						// Same team!  No damage.
						f_damage = 0.0f;
					}
				}

				// An animate!  Let's see how bad he's hurt.
				float f_other_damage = fCalculateHitPoints(pins_other, msgcoll);

				// Okay, we calculate this twice for Animate/Animate collisions, but that's pretty rare.
				// Is the other guy worse off than we are?
				if (f_other_damage > f_damage)
				{
					// Yes!  We take no damage.
					f_damage = 0.0f;
				}
			}
			else
			{
				// Have we already felt this pain?
				if (f_damage <= fCollideDamageThisFrame)
					return;
				else
				{
					float f_temp = f_damage;

					// And add the difference.
					f_damage = f_damage - fCollideDamageThisFrame;

					// Remember the new maximum.
					fCollideDamageThisFrame = f_temp;
				}

//				dout << "Pre-buffered damage " << f_damage;


				// Collision from an inanimate non-bullet object.
				// Apply special collision damage here.
				//dout << " Reset pts " <<fCollideResetPoints <<" - " <<f_damage;
				fCollideResetPoints -= f_damage;
				//dout << " = " <<fCollideResetPoints;

				//dout << ", Collide pts " <<fCollidePoints <<" - " <<f_damage;
				fCollidePoints -= f_damage;
				//dout << " = " <<fCollidePoints <<endl;
				if (fCollidePoints < 0.0f)
				{
					// Overran our buffer.  Now we really hurt.
					// Take full damage less any reserve we had.
					SetMin(f_damage, -fCollidePoints);
				}
				else
					// Not strong enough to hurt us.
					f_damage = 0.0f;

//				dout << "\tBuffered: " << f_damage << "\n";
			}
		}

		// Are we already dead?
		if (fHitPoints > fReallyDead)
		{
			// No.  Make it hurt so good.

			// Take tranq damage, if any.
			if (pgun)
			{
				f_tranq = pgun->gdGunData.fTranqDamage;
				fTranqPoints -= f_tranq;
			}

			if (f_damage > 0.0f)
			{
				fHitPoints		-= f_damage;
				dout << pins_us->strGetInstanceName() <<": " << f_damage << " taken from " 
					 << (pins_other ? pins_other->strGetInstanceName() : pgun ? "Gun" : "?")
					 <<" .  Now " << fHitPoints << "\n";

				HandleDamage(f_damage, pins_other, pins_us);

				// Send the damage message!
				CMessageDamage msgd(this, this, f_damage, f_damage > fCriticalHit);
				msgd.Dispatch();

				// Hit points can never be negative, but you can only die once.
				if (fHitPoints < 0.0f && fHitPoints + f_damage > 0.0f)
				{
					// Let physics know right away so we can get dramatic.
					UpdatePhysicsHealth();

					// Save a pointer to the one what done you in.
					pinsKiller = pins_other;

					// Send a death message.
					CMessageDeath msgdth(this);
					msgdth.Dispatch();
				}

				// Was it a gun what done us in?
				//if (false)
				if (pgun)
				{
					float f_gun_death_multiplier = 3.0f;
					CVector3<> v3_arm = msgcoll.v3Position;

					
					//v3_arm.tX += 1.0f * (v3_arm.tX - v3Pos().tX);
					//v3_arm.tY += 1.0f * (v3_arm.tY - v3Pos().tY);
					//v3_arm.tZ += 100.0f * (v3_arm.tZ - v3Pos().tZ);
					

					// Apply the gun's impulse again, scaled up.
					float f_impulse	 = 0.15f * pgun->gdGunData.fImpulse * 0.10f * f_gun_death_multiplier;
					//float f_impulse	 = pgun->gdGunData.fImpulse * 0.10f * f_gun_death_multiplier;

					// Apply hacky scale, as old impulse values were in a different scale.
					CVector3<> v3_impulse = CVector3<>(0, f_impulse, 0) * pgun->pr3Presence().r3Rot;

					pphiGetPhysicsInfo()->ApplyImpulse
					(
						this, ebbBODY, msgcoll.v3Position, v3_impulse
					);
				}
			}
		}

		//
		// Whether dead or alive, create visible wounds from damage.
		//

		if (ptCast<CPlayer>(this))
		{
			if (iGoreLevel < 2)
				// Dead player does not bleed; it's too icky for our family-oriented game.
				f_damage = f_tranq = 0.0f;

			if (pbb_us && pbb_us->ebbElement == ebbFOOT)
				// Player feet do not bleed.
				f_damage = f_tranq = 0.0f;
		}

		if (f_damage || f_tranq)
		{
			// Adjust gore based on user's queasiness.
			if (iGoreLevel <= 0)
				f_damage *= 0.4f;
			else if (iGoreLevel == 1)
				f_damage *= 0.8f;
			else if (iGoreLevel >= 2)
				f_damage *= 2.0f;

			//
			// Determine wound location from collision location.
			//
			CVector3<> v3_wound = msgcoll.v3Position;

			AddWound(v3_wound, pgun, f_damage, f_tranq, pbb_us);

			// Search the gun ray-object intersection list (from this intersection onward) for the first
			// non-animate.  If this is the terrain, consider adding a blood splat.
			if (iGoreLevel > 0)
			{
				SObjectLoc* pobl = 0;
				if (msgcoll.prcGunHits)
				{
					while ((pobl = msgcoll.prcGunHits->poblNextIntersection()) && (ptCast<CAnimate>(pobl->pinsObject) != 0))
						;
				}

				bool b_splat = false;
				if (pobl && ptCast<CTerrain>(pobl->pinsObject))
				{
					// Determine distance from the entry wound to the terrain.
					float f_dist_to_trr = (msgcoll.v3Position - pobl->v3Location).tLen();

					if (f_dist_to_trr <= fMaxBloodSprayDistance)
					{
						// Leave a splat!
  						b_splat = true;

						// The angle at which the bullet hits the terrain determines the probability of creating
						// a blood splat vs a streak.
						CDir3<> d3_trr_hit(pgun->v3Pos() - pobl->v3Location);

						if (rndBloodSplats(0.0, 1.0) < (d3_trr_hit * pobl->d3Face))
						{
							// Add a blood splat. The size of the blood splat depends on the amount of damage done.
							pBloodSplats->CreateSplat(pobl->v3Location.tX, pobl->v3Location.tY, f_damage * fBloodSplatSizeDamageScale);
						}
						else
						{
							// Add a blood streak. Find direction of gun toward wound entry point, in XY plane.
							CVector2<> v2_fire(msgcoll.v3Position - pgun->v3Pos());
							pBloodSplats->CreateStreak(pobl->v3Location.tX, pobl->v3Location.tY, f_damage * fBloodSplatStreakDamageScale, v2_fire);
						}
					}
				}

				// Hack in extra splats if the raycast splat fails.
				// Make this if always false to remove HACK HACK HACK blood splats
				if (!b_splat && CWDbQueryTerrain().tGet())
				{
					// We didn't get a pure raycast splat.  Maybe leave a dropped splat.
 					if (true) //rndBloodSplats() & (1 << 5))
					{
						// 50% shot at a local splat.

						// Add a blood splat. The size of the blood splat depends on the amount of damage done.
						pBloodSplats->CreateSplat(v3_wound.tX, v3_wound.tY, f_damage * fBloodSplatSizeDamageScale * 0.5f);
					}
				}

			}
		}
	}

	//******************************************************************************************
	void CAnimate::AddWound(const CVector3<>& v3_pos, CGun* pgun, 
							float f_damage, float f_tranq, CBoundaryBox* pbb)
	{
		Assert(pbb);

		// HACK: Do not place wounds on any player body part but BODY.
		if (ptCast<CPlayer>(this))
			pbb = apbbBoundaryBoxes[(int)ebbBODY];

		// Find box-relative placement.
		CVector3<> v3_rel = v3_pos / pbb->p3GetPlacement();
		CDir3<> d3_norm = v3_rel;

		// Move wound to surface of ellipsoid.
		const CPhysicsInfoBox* ppib = pbb->pphiGetPhysicsInfo()->ppibCast();
		if (ppib)
		{
			CVector3<> v3_max = ppib->bvbBoundVol.v3GetMax() * pbb->fGetScale();
			CVector3<> v3_cube(v3_rel.tX / v3_max.tX, v3_rel.tY / v3_max.tY, v3_rel.tZ / v3_max.tZ);
			float f_adjust = fInvSqrt(v3_cube.tLenSqr());

			// Move inward toward Y axis.
			v3_rel.tX *= f_adjust;
			v3_rel.tZ *= f_adjust;

			if (ptCast<CPlayer>(this))
				// Move inward along Y axis as well.
				v3_rel.tY *= f_adjust;
			else
				// Clamp to Y axis extents.
				SetMinMax(v3_rel.tY, -v3_max.tY, v3_max.tY);

			// Set normal to ellipsoid surface normal.
			d3_norm = v3_cube;
		}

		// Get new world position based on our adjustments.
		CVector3<> v3_pos_new = v3_rel * pbb->p3GetPlacement();

		CPlacement3<> p3_rel
		(
			CRotate3<>(d3YAxis, d3_norm),
			v3_rel
		);

		// Clamp the blood effect at the capacity of the animal.
		SetMin(f_damage, fMaxHitPoints);
		SetMin(f_tranq, fMaxHitPoints);

		// First do initial splotch.
		CCreateParticles* pcrt = NParticleMap::pcrtFind(smatBlood);
 		if (pcrt && f_damage)
		{
			CCreateParticles crt = *pcrt;
			
			// Customise angle according to this hit.
			CDir3<> d3_wound	= d3_norm * pbb->r3Rot();
			CDir3<> d3_spray	= d3_wound;
			if (pgun)
			{
				CDir3<> d3_fire		= v3_pos_new - pgun->v3Pos();
				CVector3<> v3_spray	= d3_wound + d3_fire;
				CDir3<> d3_spray	= v3_spray.bIsZero() ? d3_wound : CDir3<>(v3_spray);
				crt.ppMax.fSpreadRadians = crt.ppMin.fSpreadRadians = acos(d3_wound * d3_fire) * 0.5;
			}
			else
			{
				// For collision hits, downward spray is kind of useless.
				// Set Z axis positive.
				d3_spray.tZ = Abs(d3_spray.tZ);
				crt.ppMax.fSpreadRadians = 120 * dDEGREES;
			}

			crt.Create(v3_pos_new, d3_spray, 1.0, f_damage);
		}

		//
		// Add a persistent wound (only if alive, and gore allows).
		//
		pcrt = NParticleMap::pcrtFind(smatBleed);
		if (pcrt && !bReallyDead() && iGoreLevel > 0)
		{
			if (iNumWounds == paWounds.size())
			{
				// Delete a wound.
				for (int i = 0; i < iNumWounds-1; i++)
					paWounds[i] = paWounds[i+1];
				iNumWounds--;
			}


			// Create a particle cycler from the specified particle effect.
			CCreateParticlesCycleBleed crtc(*pcrt);

			// Create the wound.
			paWounds[iNumWounds++] = CWound(Max(f_damage, f_tranq * 0.5f), pbb, p3_rel, crtc);
		}
	}

	//******************************************************************************************
	void CAnimate::Process(const CMessagePhysicsReq& msgpr)
	{
		// Pass it on to the physics model.
		Assert(pphiGetPhysicsInfo());
		pphiGetPhysicsInfo()->HandleMessage(msgpr, this);
	}


	//*****************************************************************************************
	void CAnimate::PreRender(CRenderContext& renc)
	{
	}


/*
		// An experiment!
		float f_energy = 0.0f;
		if (pins_target == msgcoll.pins1)
		{
			f_energy = Abs(msgcoll.fEnergyDelta1);
		}
		else
		{
			f_energy = Abs(msgcoll.fEnergyDelta2);
		}

		// Boundary boxes tend to be too massive for accurate energy calculations, so we're
		// trimming them back to match everybody else.
		if (ptCast<CBoundaryBox>((CInstance*)pins_target))
			f_energy *= fBIOMODEL_ADJUST_HACK;

		dout << "Energy: " << f_energy << "\n";


		float f_damage = fCOLLISION_DAMAGE * (f_energy);
 */



	//******************************************************************************************
	float CAnimate::fCalculateHitPoints(const CInstance* pins_target, const CMessageCollision& msgcoll)
	{
 		Assert(pins_target);

		// Calculate base damage from collision intensity.
		// Use Hit energy only; ignore Slide component.
		float f_energy1 = Max(0.0f, - msgcoll.fEnergyDelta1);
		float f_energy2 = Max(0.0f, - msgcoll.fEnergyDelta2);

		// Do we have the player's foot?
		CBoundaryBox* pbb = ptCast<CBoundaryBox>((CInstance*)pins_target);
		bool b_target_is_player_foot = (pbb->paniAnimate == gpPlayer && pbb->ebbElement == ebbFOOT);

		// Boundary boxes tend to be too massive for accurate energy calculations, so we're
		// trimming them back to match everybody else.
		pbb = ptCast<CBoundaryBox>(msgcoll.pins1);
		if (pbb)
		{
			// We have a boundary box.
			f_energy1 *= fBIOMODEL_ADJUST_HACK;

			// Do we have the player's foot?
			if (pbb->paniAnimate == gpPlayer && pbb->ebbElement == ebbFOOT)
			{
				// Yes!  Ignore the other guy.  Foot only takes own damage.
				f_energy2 = 0.0f;
			}
			else
			{
				// No!  Ignore own energy.  Body only takes damage when hit with something,
				// not when it hits something.  This is bogus, but it solves the problem of
				// body hitting the ground before the foot and taking too much damage, and the
				// general problem of the somewhat frantic energy changes of biomodels.
				f_energy1 = 0.0f;
			}
		}

		pbb = ptCast<CBoundaryBox>(msgcoll.pins2);
		if (pbb)
		{
			f_energy2 *= fBIOMODEL_ADJUST_HACK;

			// Do we have the player's foot?
			if (pbb->paniAnimate == gpPlayer && pbb->ebbElement == ebbFOOT)
			{
				// Yes!  Ignore the other guy.  Foot only takes own damage.
				f_energy1 = 0.0f;
			}
			else
			{
				// No!  Ignore own energy.  See above.
				f_energy2 = 0.0f;
			}
		}

		float f_damage = fCOLLISION_DAMAGE * (f_energy1 + f_energy2);

		const CInstance* pins_attacker = 0;
		if (msgcoll.pins1 == pins_target)
		{
			pins_attacker = msgcoll.pins2;
		}
		else
		{
			Assert(msgcoll.pins2 == pins_target);
			pins_attacker = msgcoll.pins1;
		}

		// Attacker is the bad guy!
		CGun* pgun = ptCast<CGun>(msgcoll.petGetSender());
		CBoundaryBox* pbb_attacker = 0;

		if (pins_attacker)
		{
			// Is it a boundary box from a live animal?
			const CBoundaryBox* pbb_attacker = ptCast<CBoundaryBox>((CInstance*)pins_attacker);
			if (pbb_attacker && !pbb_attacker->paniAnimate->bDead())
			{
				// Yes!  Damage from bounding boxes is done very differently.
				f_damage = pbb_attacker->fDamagePerSecond * pbb_attacker->fDamageMultiplier * CMessageStep::sStaticStep;

				// Do not get bitten on the foot!
				// Do we have the player's foot?
				if (b_target_is_player_foot)
					f_damage = 0.0f;
			}
			else
			{
				// No!  Go ahead and apply normal damage.  Otherwise, it's dead and can't hurt much.
				const CPhysicsInfo* pphi = pins_attacker->pphiGetPhysicsInfo();
				if (pphi && !pbb_attacker)
				{
					// We have a physics info, 
					// AND it is not a boundary box.  We thus force BBox multipliers to be zero.
					//     (Dinos are never more collision-dangerous than the standard.)

					// Finally, do we have the player foot?  If so, this is falling damage and should not have the 
					//  multiplier.
					if (!b_target_is_player_foot)
						f_damage *= pphi->fDamageMultiplier(pins_attacker);
				}

				// Further adjust damage if object was wielded.
				CInstance* pins_wielder = NMagnetSystem::pinsFindMaster(const_cast<CInstance*>(pins_attacker));
				CBoundaryBox* pbb_wielder = ptCast<CBoundaryBox>(pins_wielder);
				if (pbb_wielder)
				{
 					f_damage = pbb_wielder->paniAnimate->fWieldDamage(pins_attacker, f_damage);
				}

				pbb_attacker = 0;
			}
		}
		else if (pgun)
		{
			// Hit by a bullet, get bullet damage from message.
			f_damage = pgun->gdGunData.fDamage;
		}

		// Now adjust damage for the defense adjustment.
		const CPhysicsInfo* pphi = pins_target->pphiGetPhysicsInfo();
		if (pphi)
		{
			float f_armour = pphi->fArmourMultiplier(pins_target);

			// Do we have the player's foot?
			
			if (b_target_is_player_foot && f_armour == 0.0f)
				f_armour = 1.0f;

			f_damage *= f_armour;
		}

		return f_damage;
	}

	//*****************************************************************************************
	CPlacement3<> CAnimate::p3HeadPlacement() const
	{
		// Default version returns null placement (head same position as body).
		if (apbbBoundaryBoxes[(int)ebbHEAD])
			return apbbBoundaryBoxes[(int)ebbHEAD]->p3GetPlacement() / p3GetPlacement();
		return CPlacement3<>();
	}

	//*****************************************************************************************
	void CAnimate::Substitute(int i_sub)
	{
		// Do we have a CSubBioMesh?
		rptr<CRenderType> prdt = prdtGetRenderInfoWritable();
		rptr<CSubBioMesh> psbm = rptr_dynamic_cast(CSubBioMesh, prdt);
		if (psbm)
			// Perform substitution in the biomesh.
			psbm->Substitute(i_sub);
	}

	//*****************************************************************************************
	int CAnimate::iGetTeam() const
	{
		return iTeam;
	}
	
	//*****************************************************************************************
	const CAnimate* CAnimate::paniGetOwner() const
	{
		return this;
	}

	//*****************************************************************************************
	CInstance* CAnimate::pinsCopy() const
	{
		AlwaysAssert(false);	
		return 0;
	}

#if VER_TEST
	//*****************************************************************************************
	int CAnimate::iGetDescription(char *buffer, int i_buffer_length)
	{
		int i_len = CInstance::iGetDescription(buffer, i_buffer_length);

		char local_buffer[256];
		
		// Hit points, max hit points, regen rate, really die, die rate
		sprintf(local_buffer, "Hit points: %f of %f\nReal Death: %f\nRegen: %f per second\nDie Rate: %f per second\n", 
			fHitPoints, fMaxHitPoints, fReallyDead, fRegenerationRate, fDieRate);

		strcat(buffer, local_buffer);

		sprintf(local_buffer, "Tranq points: %f of %f\nTrang Regen: %f per second\n", 
			fTranqPoints, fMaxTranqPoints, fTranqRegenerationRate);

		strcat(buffer, local_buffer);
		return strlen(buffer);
	}
#endif

	//*****************************************************************************************
	char *CAnimate::pcSave(char * pc) const
	{
		pc = CInstance::pcSave(pc);

		// Hit points!
		pc = pcSaveT(pc, fHitPoints);
		pc = pcSaveT(pc, fTranqPoints);

		////////  New for animal version 7
		pc = pcSaveT(pc, fMaxHitPoints);
		pc = pcSaveT(pc, fReallyDead);
		pc = pcSaveT(pc, fRegenerationRate);
		pc = pcSaveT(pc, fDieRate);

		pc = pcSaveT(pc, fTranqPoints);
		pc = pcSaveT(pc, fMaxTranqPoints);
		pc = pcSaveT(pc, fTranqRegenerationRate);
		////////


		// The joint transforms, so animals (dead or alive) retain their shape.

		// Save count, for safety.
		pc = pcSaveT(pc, sriBones.patf3JointTransforms.size());
		for (int i = 0; i < sriBones.patf3JointTransforms.size(); i++)
			pc = pcSaveT(pc, sriBones.patf3JointTransforms[i]);

		return pc;
	}

	//*****************************************************************************************
	const char *CAnimate::pcLoad(const char * pc)
	{
		extern int iAnimalVersion;

		pc = CInstance::pcLoad(pc);

		// Hit points!
		pc = pcLoadT(pc, &fHitPoints);
		if (iAnimalVersion >= 10)
		{
			pc = pcLoadT(pc, &fTranqPoints);
		}

		if (iAnimalVersion >= 7)
		{
			////////  New for animal version 7
			pc = pcLoadT(pc, &fMaxHitPoints);
			pc = pcLoadT(pc, &fReallyDead);
			pc = pcLoadT(pc, &fRegenerationRate);
			pc = pcLoadT(pc, &fDieRate);

			pc = pcLoadT(pc, &fTranqPoints);
			pc = pcLoadT(pc, &fMaxTranqPoints);
			pc = pcLoadT(pc, &fTranqRegenerationRate);
			////////
		}
		
		// Reset collide points to max for now.
		fCollidePoints = fCollideResetPoints = fMaxCollidePoints;

		if (CSaveFile::iCurrentVersion >= 8)
		{
			// The joint transforms, so animals (dead or alive) retain their shape.

			// Read count, for safety.
			int i_size;
			pc = pcLoadT(pc, &i_size);
			AlwaysAssert(i_size == sriBones.patf3JointTransforms.size());

			for (int i = 0; i < sriBones.patf3JointTransforms.size(); i++)
				pc = pcLoadT(pc, &sriBones.patf3JointTransforms[i]);
		}

		// We're not saving wound state; sorry.
		iNumWounds = 0;

		return pc;
	}


//*********************************************************************************************
//
// CBoundaryBox implementation.
//

	//******************************************************************************************
	CBoundaryBox::CBoundaryBox
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CInstance(pgon, pload, h_object, pvtable, pinfo), paniAnimate(0)
	{
		paniAnimate = 0;

		AlwaysAssert(pphiGetPhysicsInfo()->bIsTangible() && pphiGetPhysicsInfo()->bIsMovable());

		// Attempt to fix if re-trying.
		if (!pphiGetPhysicsInfo()->bIsTangible() || !pphiGetPhysicsInfo()->bIsMovable())
		{
			CPhysicsInfo* pphi = pphiGetPhysicsInfoWritable();

			// Force these flags on.
			pphi->setFlags[epfTANGIBLE][epfMOVEABLE] = true;

			// Replace the info.
			SetPhysicsInfo(pphi);
		}

		// HACK HACK HACK
		fDamagePerSecond = pphiGetPhysicsInfo()->fDamage * 10.0f;
		fDamageMultiplier = 0.0f;

		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_object)
		{
			bFILL_FLOAT(fDamagePerSecond, esDamagePerSecond);
		}
		END_OBJECT;
		END_TEXT_PROCESSING;
	}

	//******************************************************************************************
	CBoundaryBox::CBoundaryBox
	(
		CAnimate* pani,
		EBoundaryBoxes ebb,
		const CVector3<>& v3_offset,
		const CPhysicsInfoBox& phib,
		const char* str_name
	) : 
		CInstance(CInstance::SInit
		(
			pani->p3GetPlacement(),		// Set to animate placement initially; scale = 1.
			rptr0, 
			const_cast<CPhysicsInfoBox*>(CPhysicsInfoBox::pphibFindShared(phib)), 
			0, 
			str_name
		)),
		paniAnimate(pani),
		ebbElement(ebb)
	{
		// Can't handle offset here yet.
//		Assert(v3_offset.bIsZero());
	}

	//******************************************************************************************
	int CBoundaryBox::iPhysicsElem() const
	{
		// Simply use physics to translate our box number to physics element.
		return CPhysicsInfoSkeleton::iPhysicsElem(ebbElement);
	}

	//*****************************************************************************************
	int CBoundaryBox::iGetTeam() const
	{
		return paniAnimate->iTeam;
	}

	//*****************************************************************************************
	const CAnimate* CBoundaryBox::paniGetOwner() const
	{
		return paniAnimate;
	}

	//******************************************************************************************
	CVector3<> CBoundaryBox::v3GetOffset() const
	{
		// Get the pivot point from our physics box, and scale by our size.
		const CPhysicsInfoBox* pphib = pphiGetPhysicsInfo()->ppibCast();
		Assert(pphib);
		return pphib->v3Pivot * pr3GetPresence().rScale;
	}

	//*****************************************************************************************
	CInstance* CBoundaryBox::pinsCopy() const
	{
		AlwaysAssert(false);	
		return 0;
	}

