/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Implementation of CHitSpang
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/HitSpang.cpp                                        $
 * 
 * 9     8/25/98 4:49p Rvande
 * Loop variable moved into block scope
 * 
 * 8     98/08/21 17:51 Speter
 * Moved particle system update to physics system step.
 * 
 * 7     8/21/98 1:11a Rwyatt
 * Added the default hit spang to the hit spang map under a null key so the map reset code will
 * remove it but the hit spang material search will not find it.
 * 
 * 6     8/16/98 3:10p Pkeet
 * Added particles to the hit spang system.
 * 
 * 5     7/08/98 11:03p Rwyatt
 * Hit spangs can now  randomly rotate around their Y axis...
 * 
 * 4     7/06/98 1:17a Rwyatt
 * Hit spangs can now grow
 * 
 * 3     7/02/98 7:10p Rwyatt
 * Hit spangs now have a random scale factor
 * 
 * 2     7/02/98 2:02a Rwyatt
 * First working version
 * 
 * 1     7/02/98 1:31a Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/
#include "Common.hpp"
#include "HitSpang.hpp"
#include "Lib/Trigger/Trigger.hpp"
#include "Lib/Sys/DebugConsole.hpp"

//*********************************************************************************************
//
// CHitSpang implementation.
//
	SHitSpangList*	CHitSpang::phslDefault = NULL;
	THitSpangActive	CHitSpang::hsaWorldSpangs;
	TSpangMap		CHitSpang::smSpangs;
	uint32			CHitSpang::u4SpangFrame = 0;


	//*****************************************************************************************
	//
	// Constructors.
	//
	CHitSpang::CHitSpang
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CInstance(pgon, pload, h_object, pvtable, pinfo)
	//*********************************
	{
		const CEasyString* pestr_material = NULL;

		fScaleDecay = 200.0f;		// 200% per second (disappear in 0.5s)
		bInWorld = false;

		// By default the muzzle flash is not scaled..
		fMinScale = 1.0f;
		fMaxScale = 1.0f;
		fScaleLimit = 1.0f;

		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_object)
		{
			uint32 u4_count = 0;		// the current material index

			// How many of this type of hit spang should we create
			int32 i4_hit_count = 4;
			bFILL_INT( i4_hit_count, esSpangCount);
			Assert(i4_hit_count>0);

			// What is the scale decay of this type of hit spang
			bFILL_FLOAT(fScaleDecay, esDecay);

			// What is the minimum initial scale of this hit spang
			bFILL_FLOAT(fMinScale, esMinScale);
			Assert(fMinScale>=0.0f);

			// What is the maximum initial scale of this hit spang
			bFILL_FLOAT(fMaxScale, esMaxScale);
			Assert(fMaxScale>=0.0f);

			bFILL_FLOAT(fScaleLimit, esScaleLimit);
			Assert(fScaleLimit>0.0f);

			// Max scale must be greater than the min scale (or equal)
			Assert(fMaxScale>=fMinScale);
			Assert(fMaxScale<=fScaleLimit);

			// Should we rotate the hit spangs??
			bRandomRotate = false;
			bFILL_BOOL(bRandomRotate, esRandomRotate);

			// Create the hit spang list
			SHitSpangList*	phsl = phslCreate((uint32)i4_hit_count);
			Assert(phsl);

			// Process the list of materials that this object should be used for....
			while ( bFILL_pEASYSTRING(pestr_material, (ESymbol)((uint32)esA00+u4_count)) )
			{
				uint32 u4_hash = matHashIdentifier( pestr_material->strData() );
				Assert(smSpangs[u4_hash] == NULL);
				smSpangs[u4_hash] = phsl;
				u4_count++;
			}

			if (u4_count == 0)
			{
				//
				// If the count is zero then no materials were specified so this must be the
				// default hit spang.
				//

				// Make sure we do not already have a default
				Assert(phslDefault == NULL);

				phslDefault = phsl;

				// Material 0 is the default, this is just so it is in the map and therefore
				// will get deleted in the world reset. The find function will never find this
				// because zero is an invalid hash value.
				smSpangs[0] = phsl;
			}
		}
		END_OBJECT_HANDLE;
		END_TEXT_PROCESSING;
	}

	//*****************************************************************************************
	//
	// Destructors.
	//
	CHitSpang::~CHitSpang
	(
	)
	//*********************************
	{
		Assert(bInWorld == false);
	}


	//*****************************************************************************************
	// CReate a list of identical hit spangs
	SHitSpangList* CHitSpang::phslCreate
	(
		uint32 u4_hit_count
	)
	//*********************************
	{
		SHitSpangList* phsl;

		phsl = (SHitSpangList*) new char[ sizeof(SHitSpangList)+(u4_hit_count-1)*sizeof(CHitSpang*) ];
		phsl->u4HitCount = u4_hit_count;
		phsl->u4NextHit = 0;
		phsl->ahpsnList[0] = this;

		u4_hit_count--;
		uint32 u4 = 1;
		while (u4_hit_count)
		{
			// Construct identical copies for the rest of the hit spang list

			// Must return an identical copy
			AlwaysAssert(!pinfoProperties->setUnique[CInfo::eitTHIS]);

			phsl->ahpsnList[u4++] = new CHitSpang(*this);
			u4_hit_count--;
		}

		return phsl;
	}


	//*****************************************************************************************
	//
	// Static functions to cotrol and process hit spangs
	//

	//******************************************************************************************
	// This is called at the start and before any new level is loaded.
	// Set everything to a default state and assert that any previous hit spangs have been removed
	void CHitSpang::Reset()
	{
		// The default shoul dhave been removed by now
		Assert(phslDefault == NULL);
		// set it to NULL for good measure
		phslDefault = NULL;

		// There should be no active hit spangs and no hit spang lists
		Assert(hsaWorldSpangs.size() == 0);
		Assert(smSpangs.size() == 0);

		// Reset the frame
		u4SpangFrame = 0;
	}


	//******************************************************************************************
	// Go through the material map and null out any materials that have the speicifed
	// spang list pointer.
	void CHitSpang::RemoveDuplicateSpang
	(
		SHitSpangList* phsl
	)
	//*********************************
	{
		for (TSpangMap::iterator i = smSpangs.begin(); i!=smSpangs.end() ; ++i)
		{
			if ( (*i).second == phsl )
			{
				// All materials that point to the spang list passed in now point to NULL
				(*i).second = NULL;
			}
		}
	}


	//******************************************************************************************
	// Called at the end to remove any hit spangs and delete them from the world
	void CHitSpang::Remove
	(
	)
	//*********************************
	{
		RemoveVisible();

		// Now delete all the hit spangs in the material map......
		for (TSpangMap::iterator i = smSpangs.begin(); i!=smSpangs.end() ; ++i)
		{
			SHitSpangList* phsl = (*i).second;
			if ( phsl )
			{
				// Delete all of the instances, none of which should be visible
				for (uint32 u4=0;u4< phsl->u4HitCount; u4++)
				{
					delete phsl->ahpsnList[u4];
				}

				// Is this the default??
				if ( phsl == phslDefault)
					phslDefault = NULL;

				// There could be other materials pointing to this hit spang list so we must remove
				// them as well.
				RemoveDuplicateSpang( phsl );

				// delete the hit spang list structure....
				delete phsl;
			}
		}

		// Remove all the pointers from the map
		smSpangs.erase( smSpangs.begin(), smSpangs.end());

		// ensure that there is no spangs left behind
		Assert(smSpangs.size() == 0);
	}


	//******************************************************************************************
	// Remove all visible hit spangs from the world
	void CHitSpang::RemoveVisible
	(
	)
	//*********************************
	{
		bool b_remove = true;

		while (b_remove)
		{
			b_remove = false;

			for (THitSpangActive::iterator i = hsaWorldSpangs.begin(); i!=hsaWorldSpangs.end() ; ++i)
			{
				// Make sure that our spang is still marked as visible
				Assert( (*i)->bVisible() );
				wWorld.Remove( (*i) );
				(*i)->bInWorld = false;

				hsaWorldSpangs.erase(i);
				b_remove = true;
				break;
			}
		}
	}

	//*****************************************************************************************
	// Called by the gun when something is hit. This should display the correct hit spang
	void CHitSpang::ProcessHit
	(
		SObjectLoc* pobl
	)
	//*********************************
	{
		TSoundMaterial	smat;
		CPlacement3<>	p3;
		SHitSpangList*	phsl;

		//
		// From the obejct structure figure out what hit spang mesh to use....
		//
		if (pobl->pinsObject)
		{
			smat = pobl->pinsObject->pphiGetPhysicsInfo()->smatGetMaterialType(pobl->v3Location);

			TSpangMap::iterator	i;

			i = smSpangs.find((uint32)smat);
			if (i != smSpangs.end())
			{
				phsl = (*i).second;
			}
			else
			{
				// We have not found a spang for this material so use the default....
				phsl = phslDefault;
			}
		}
		else
		{
			// There was no object specified, just use the default if there is one.
			phsl = phslDefault;
		}


		// Did not find an hit spang so just do nothing
		if (phsl == NULL)
			return;

		// Get the next hit spang mesh of this type
		CHitSpang*	phspn = phsl->ahpsnList[phsl->u4NextHit];
		phsl->u4NextHit++;
		if (phsl->u4NextHit>=phsl->u4HitCount)
			phsl->u4NextHit = 0;

		// add this hit spang to the world if it is not already present and put it in the
		// hit spang list so it will get processed.
		phspn->AddToWorld();

		// Set the default scale
		if (phspn->fMinScale == phspn->fMaxScale)
		{
			phspn->fScale = phspn->fMinScale;
		}
		else
		{
			phspn->fScale = CTrigger::rndRand(phspn->fMinScale,phspn->fMaxScale);
		}

		phspn->SetScale(phspn->fScale);

		// position the hit spang at the collision location with the Y axis of the hit spang
		// pointing down the face normal.
		p3.v3Pos = pobl->v3Location;
		if (phspn->bRandomRotate)
		{
			p3.r3Rot = CRotate3<>( CDir3<>(0.0f,1.0f,0.0f), CAngle(CTrigger::rndRand(0,2*dPI)));
			p3.r3Rot *= CRotate3<>( CDir3<>(0.0f,1.0f,0.0f), pobl->d3Face);
		}
		else
		{
			p3.r3Rot = CRotate3<>( CDir3<>(0.0f,1.0f,0.0f), pobl->d3Face);
		}
		phspn->Move(p3);
	}


	//*****************************************************************************************
	// Called from the gun step message every frame
	//
	void CHitSpang::ProcessHitSpangs
	(
		const CMessageStep& msg
	)
	//*********************************
	{
		bool b_remove = false;

		// Iterate through the list of active hit spangs
		THitSpangActive::iterator i = hsaWorldSpangs.begin();
		for (; i!=hsaWorldSpangs.end() ; ++i)
		{
			// Set the new scale of this hit spang
			(*i)->fScale -= ((msg.sStaticStep * (*i)->fScaleDecay) / 100.0f);

			// If we still have a scale that is above zero and below our limit: Set it
			if ( ((*i)->fScale>0.0f) && ((*i)->fScale<(*i)->fScaleLimit) )
			{
				(*i)->SetScale( (*i)->fScale );
			}
			else
			{
				b_remove = true;
			}
		}

		//
		// Now go through the list and remove any that we no longer require. If nothing
		// needs to be removed b_remove will be false and nothing will be checked.
		//
		while (b_remove)
		{
			b_remove = false;

			for (i = hsaWorldSpangs.begin(); i!=hsaWorldSpangs.end() ; ++i)
			{
				if ( ((*i)->fScale<=0.0f) || ((*i)->fScale>=(*i)->fScaleLimit) )
				{
					// Make sure that our spang is still marked as visible
					Assert( (*i)->bVisible() );
					wWorld.Remove( (*i) );
					(*i)->bInWorld = false;

					hsaWorldSpangs.erase(i);
					b_remove = true;
					break;
				}
			}
		}
	}
