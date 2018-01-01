/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		CHitSpang
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/HitSpang.hpp                                       $
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
 * 1     7/02/98 1:32a Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_HITSPANG_HPP
#define HEADER_LIB_ENTITYDBASE_HITSPANG_HPP

#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib\GeomDBase\RayCast.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase\WorldDBase.hpp"
#include "list.h"

class CHitSpang;


//*********************************************************************************************
struct SHitSpangList
// prefix: hsl
{
	uint32		u4NextHit;			// Next hit spang in the list to use
	uint32		u4HitCount;			// number of hit spangs of this type
	CHitSpang*	ahpsnList[1];		// an array of hit spang pointers that is u4HitCount long
};

//*********************************************************************************************
// List of hitspangs
typedef list<CHitSpang*>	THitSpangActive;

//*********************************************************************************************
// Map of hit spang lists, referenced by material value
typedef map<uint32, SHitSpangList*, less<uint32> > TSpangMap;
// prefix sm

//*********************************************************************************************
//
class CHitSpang : public CInstance
//
// Hit Spang class and static manager
//
// Prefix hspn
//
//**************************************
{
public:

	//******************************************************************************************
	CHitSpang
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);


	//******************************************************************************************
	~CHitSpang();

	//******************************************************************************************
	SHitSpangList* phslCreate(uint32 u4_hit_count);

	//******************************************************************************************
	bool bVisible()
	{
		return bInWorld;
	}

	//******************************************************************************************
	void AddToWorld()
	{
		if (bInWorld)
			return;

		// add our flash mesh to the world database
		wWorld.Add(this, true);
		bInWorld = true;

		// add this hit spang to the world list
		hsaWorldSpangs.push_back(this);
	}

	//******************************************************************************************
	void RemoveFromWorld()
	{
		if (!bInWorld)
			return;

		// add our flash mesh to the world database
		wWorld.Remove(this);
		bInWorld = false;
	}

	//******************************************************************************************
	// Set while this muzzle flash is in the world and visible
	bool bInWorld;

	//******************************************************************************************
	// The per second scaling factor
	float fScaleDecay;

	//******************************************************************************************
	// The current scale
	float fScale;

	//******************************************************************************************
	// The range of scales for this spang
	float fMinScale;
	float fMaxScale;

	//******************************************************************************************
	// Set to true if the muzzle flash is to rotate randomly around its Y axis
	bool bRandomRotate;

	//******************************************************************************************
	// The biggest this hit spang can get before it is deleted
	float fScaleLimit;

	//******************************************************************************************
	static void Reset();

	//******************************************************************************************
	static void Remove();

	//******************************************************************************************
	static void RemoveDuplicateSpang(SHitSpangList* phsl);

	//******************************************************************************************
	static void RemoveVisible();

	//******************************************************************************************
	// Process a bullet hit and display a hit spang, this is called by the gun
	static void ProcessHit(SObjectLoc* pobl);

	//******************************************************************************************
	// This is called from the CGun step message. The reason we check the frame is because
	// every gun in the level is going to call this function.
	static void ProcessStep
	(
		const CMessageStep& msg
	)
	{
		if (msg.u4Frame != u4SpangFrame)
		{
			u4SpangFrame = msg.u4Frame;
			ProcessHitSpangs(msg);
		}
	}

	//******************************************************************************************
	// This is called every frame by the gun to update any visible hit spangs
	static void ProcessHitSpangs
	(
		const CMessageStep& msg
	);

	//******************************************************************************************
	// This is a global hit spang that is used for every thing that does not have its own
	// specified hit spang.
	static SHitSpangList*	phslDefault;

	//******************************************************************************************
	// The list of hit spangs that are in the world and need to be processed
	static THitSpangActive	hsaWorldSpangs;

	//******************************************************************************************
	// List of all loaded hit spangs, referenced by sound material ID
	static TSpangMap		smSpangs;

	//******************************************************************************************
	static uint32			u4SpangFrame;
};



#endif	//HEADER_LIB_ENTITYDBASE_HITSPANG_HPP
