/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of PartitionSpace.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/PartitionSpace.cpp                                      $
 * 
 * 40    9/12/98 4:55p Kmckis
 * Report errors when saving partitions in release mode, so we can figure out which is the bad
 * guy
 * 
 * 39    9/03/98 7:26p Pkeet
 * Equality test asserts when a duplicate handle is found but the spatial information for the
 * partition is not the same.
 * 
 * 38    8/26/98 9:42p Pkeet
 * Amount of memory used for the system is now output to the debugger.
 * 
 * 37    8/26/98 11:59a Rvande
 * Rearranged include order; loop variables re-scoped; a few explicit casts added
 * 
 * 36    8/25/98 4:41p Rwyatt
 * Reset heap
 * 
 * 35    7/04/98 4:43p Pkeet
 * Commented out the random grow for hash collisions.
 * 
 * 34    7/03/98 9:22p Pkeet
 * Added code for finding duplicate partitions.
 * 
 * 33    6/04/98 11:42a Pkeet
 * Implemented 'GetMinMaxDistance' specific for pure spatial partitions.
 * 
 * 32    6/03/98 8:15p Pkeet
 * Added the 'GetMinMaxDistance' member function.
 * 
 * 31    98/05/08 13:49 Speter
 * Slightly faster pr3Presence().
 * 
 * 30    5/01/98 7:45p Pkeet
 * Made the 'fDistanceFromGlobalCameraSqr' 2D.
 * 
 * 29    4/24/98 3:37p Pkeet
 * Added the 'fDistanceFromGlobalCameraSqr' member function.
 * 
 * 28    4/23/98 3:18p Pkeet
 * Gave spatial partitions with infinite bounding volumes a default starting position.
 * 
 * 27    2/25/98 8:07p Pkeet
 * Added the 'GrowRandomly' and 'SetParent' member functions.
 * 
 * 26    98/02/18 21:06 Speter
 * Changed #include SaveFile.hpp to shorter SaveBuffer.hpp.
 * 
 * 25    98/02/18 10:10 Speter
 * Added get/set CPlacement functions to CPartition.
 * 
 * 24    98/02/04 14:47 Speter
 * Corrected include order.
 * 
 * 23    12/18/97 3:08p Agrant
 * Added debugging stuff to savegame and partition building to report handle collisions.
 * 
 * 22    12/17/97 12:21a Rwyatt
 * Logged memory allocated by the CPartitionSpace new operator.
 * 
 * 21    97/12/15 18:51 Speter
 * Changed CPresence3 arguments to references.
 * 
 **********************************************************************************************/

#ifdef __MWERKS__
 #include <windows.h>
#endif

#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <memory.h>

#include "Common.hpp"
#include "PartitionSpace.hpp"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/Sys/FastHeap.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Lib/Std/Hash.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/sys/DebugConsole.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/Renderer/ShapePresence.hpp"


//
// Module specific variables.
//
extern CFastHeap fhPartitionSpace;


//
// Module specific functions.
//

//*********************************************************************************************
//
inline float fSmallRandom()
//
// Returns a random number between 1 and 1.001.
//
//**********************************
{
	int i_rand = rand() & 0x000FFFFF;
	return 1.0f + float(i_rand) / float(0x000FFFFF) * 0.001f;
}


//
// Class implementations.
//

//*********************************************************************************************
//
// CPartitionSpace implementation.
//

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	//*****************************************************************************************
	CPartitionSpace::CPartitionSpace()
		: v3Position(CVector3<>(0.0f, 0.0f, 0.0f)), CPartition()
	{
		// Create a bounding volume.
		new(&bviInfinite) CBoundVolInfinite();
	}

	//*****************************************************************************************
	CPartitionSpace::CPartitionSpace(const CBoundVol& bv)
		: v3Position(CVector3<>(0.0f, 0.0f, 0.0f)), CPartition()
	{
		// Use the bounding volume pointer passed as a parameter.
		Assert(bv.iSizeOf() <= sizeof(CBoundVolBox));
		memcpy(&bviInfinite, &bv, bv.iSizeOf());
	}

	//*****************************************************************************************
	CPartitionSpace::CPartitionSpace(int i_handle, TPartSpaceMap& rpsm)
		: CPartition(i_handle)
	{
		int i_num_read;
		int i_spatial_children;
		int i_instance_children;

		CBoundVol* pbv = pbvLoad(i_handle);
		Assert(pbv->iSizeOf() <= sizeof(CBoundVolBox));
		memcpy(&bviInfinite, pbv, pbv->iSizeOf());
		delete pbv;

		// Store the presence.
		i_num_read = _read(i_handle, (char*)&v3Position, (int)sizeof(v3Position));
		AlwaysAssert(i_num_read == sizeof(v3Position));

		// Load in a count of the number of pure spatial children.
		i_num_read = _read(i_handle, (char*)&i_spatial_children, (int)sizeof(i_spatial_children));
		Assert(i_num_read == sizeof(i_spatial_children));

		// Load in a count of the number of not pure spatial children.
		i_num_read = _read(i_handle, (char*)&i_instance_children, (int)sizeof(i_instance_children));
		Assert(i_num_read == sizeof(i_instance_children));

		// Add non-spatial children to the map.
		int i;
		for (i = 0; i < i_instance_children; ++i)
		{
			uint32 u4_handle_id;

			// Load in the child's handle.
			i_num_read = _read(i_handle, (char*)&u4_handle_id, (int)sizeof(u4_handle_id));
			Assert(i_num_read == sizeof(u4_handle_id));

			// Only add the handle if it is not zero.
			if (u4_handle_id)
			{
				rpsm[u4_handle_id] = this;
			}
		}

		// Load the spatial children and add them to the list.
		for (i = 0; i < i_spatial_children; ++i)
		{
			CPartitionSpace* ppartspace = new CPartitionSpace(i_handle, rpsm);
			ppartspace->SetParent(this);
		}
	}

	//*****************************************************************************************
	CPartitionSpace::~CPartitionSpace()
	{
	}

	//*****************************************************************************************
	//
	// CPartitionSpace member functions.
	//
	
	//*****************************************************************************************
	void* CPartitionSpace::operator new(uint  u_size_type)
	{
#ifdef LOG_MEM

		// if we are logging memory allocate 8 bytes more that required, the first DWORD  is
		// a check value so we can verify that a freed block was allocated by this operator.
		// The second DWORD is the size of the block so we can log memory usage.
		void* pv = new (fhPartitionSpace) uint8[u_size_type+8];
		*((uint32*)pv)	= MEM_LOG_PARTITION_CHECKWORD;
		*(((uint32*)pv)+1) = u_size_type;

		MEMLOG_ADD_COUNTER(emlPartition, u_size_type);

		// return 8 bytes after the allocated address to the caller.
		return (void*) (((uint8*)pv)+8);
#else
		return new (fhPartitionSpace) uint8[u_size_type];
#endif
	}

	//*****************************************************************************************
	void CPartitionSpace::operator delete(void *pv_mem)
	{
#ifdef LOG_MEM
		uint32*	pu4 = (uint32*) (((uint8*)pv_mem)-8);

		if (pu4[0] == MEM_LOG_PARTITION_CHECKWORD)
		{
			MEMLOG_SUB_COUNTER(emlPartition, pu4[1]);
		}
		else
		{
			dprintf("CPartitionSpace delete: Adress %x not allocated by CPartitionSpace new\n",
					pv_mem);
		}
#endif
	}

	//*****************************************************************************************
	void CPartitionSpace::ResetHeap()
	{
		dprintf("Memory used for spatial partitions: %ldKb\n",fhPartitionSpace.uNumBytesUsed() / 1024);
		MEMLOG_SET_COUNTER(emlPartition, 0);
		fhPartitionSpace.Reset(0,0);
	}
	
	//*****************************************************************************************
	const char* CPartitionSpace::strPartType() const
	{
		return "Space";
	}
	
	//*****************************************************************************************
	int CPartitionSpace::iSizeOf() const
	{
		return sizeof(CPartitionSpace);
	}

	//*****************************************************************************************
	bool CPartitionSpace::bIsPureSpatial() const
	{
		return true;
	}

	//*****************************************************************************************
	char* CPartitionSpace::pcSave(char* pc) const
	{
		// If the bounding volume is infinite, save nothing.  We're at the top level.
		if (pbvBoundingVol()->ebvGetType() == ebvINFINITE)
			return pc;

		// Save base class information.
		pc = CPartition::pcSave(pc);

		// Store the bounding volume represented.
		// Store center.
		pc = pcSaveT(pc,v3Position);

		// Verify that the volume is a box.
		Assert(pbvBoundingVol()->ebvGetType() == ebvBOX);

		// Save the first corner.
		CVector3<> v3 = (*((CBoundVolBox*)pbvBoundingVol()))[0];
		pc = pcSaveT(pc, v3);

		return pc;
	}

	const char* CPartitionSpace::pcLoad(const char *  pc)
	{
		// Load base class info.
		pc = CPartition::pcLoad(pc);

		// Load the bounding volume represented.
		// Load center.
		pc = pcLoadT(pc,&v3Position);

		// Get the first corner.
		CVector3<> v3;
		pc = pcLoadT(pc, &v3);
		new (&bvbBox) CBoundVolBox(v3);

		return pc;
	}


	//*****************************************************************************************
	char* CPartitionSpace::pcSaveSpatial(char* pc) const
	{
		// Save base class information.
		pc = pcSave(pc);

#if VER_TEST
		// For debugging, map the handle to find collisions.
		wWorld.AddToHandleMap(u4GetUniqueHandle(), (CPartition*)this);
#endif

		// Save all children.
		pc = CPartition::pcSaveSpatial(pc);

		return pc;
	}
	
	//*****************************************************************************************
	const CBoundVol* CPartitionSpace::pbvBoundingVol() const
	{
		return &bvbBox;
	}

	//*****************************************************************************************
	CVector3<> CPartitionSpace::v3Pos() const
	{
		return v3Position;
	}
	
	//*****************************************************************************************
	void CPartitionSpace::SetPos(const CVector3<>& v3_pos)
	{
		v3Position = v3_pos;
	}

	//*****************************************************************************************
	CRotate3<> CPartitionSpace::r3Rot() const
	{
		return CRotate3<>();
	}
	
	//*****************************************************************************************
	void CPartitionSpace::SetRot(const CRotate3<>& r3_rot)
	{
		Assert(false);
	}

	//*****************************************************************************************
	float CPartitionSpace::fGetScale() const
	{
		return 1.0f;
	}

	//*****************************************************************************************
	void CPartitionSpace::SetScale(float f_new_scale)
	{
		// To do: rescale the bounding volume.
	}

	//*****************************************************************************************
	CPresence3<> CPartitionSpace::pr3Presence() const
	{
		return CPresence3<>(v3Position);
	}

	//*****************************************************************************************
	void CPartitionSpace::SetPresence(const CPresence3<> &pr3)
	{
		v3Position = pr3.v3Pos;
	}

	//*****************************************************************************************
	CPlacement3<> CPartitionSpace::p3Placement() const
	{
		return CPlacement3<>(v3Position);
	}

	//*****************************************************************************************
	void CPartitionSpace::SetPlacement(const CPlacement3<>& p3)
	{
		v3Position = p3.v3Pos;
	}

	//*****************************************************************************************
	uint32 CPartitionSpace::u4GetUniqueHandle() const
	{
		if (pbvBoundingVol()->ebvGetType() != ebvBOX)
			return 0;

		uint32 u4_hashpos = u4Hash(&v3Position, sizeof(CVector3<>));
		CVector3<> v3_box = bvbBox.v3GetMax();
		uint32 u4_hashcorner = u4Hash(&v3_box, sizeof(v3_box));

		return u4_hashpos + u4_hashcorner;
	}

	//*****************************************************************************************
	void CPartitionSpace::GrowRandomly()
	{
		AlwaysAssert(pbvBoundingVol()->ebvGetType() == ebvBOX);

		// Grow the box.
		CVector3<> v3_box = bvbBox.v3GetMax();
		v3_box.tX *= fSmallRandom();
		v3_box.tY *= fSmallRandom();
		v3_box.tZ *= fSmallRandom();

		// Replace the box with the grown box.
		bvbBox = CBoundVolBox(v3_box);
	}

	//*****************************************************************************************
	void CPartitionSpace::SetParent(CPartition* ppart)
	{
		/*
		// Do nothing if the current parent is the same as the target parent.
		//if (ppart == ppartParent)
			//return;

		//
		// If the two handles are not the same, use the default member function code. If the
		// two handles are the same, handle the case by removing one of the objects from the
		// world database.
		//
		if (!ppart)
		{
			CPartition::SetParent(ppart);
			return;
		}
		if (ppart->u4GetUniqueHandle() != u4GetUniqueHandle())
		{
			CPartition::SetParent(ppart);
			return;
		}

		// If the parent is not a spatial partition, there is no hope.
		AlwaysAssert(ppart);
		AlwaysAssert(ppart->bIsPureSpatial());

		//
		// Hack Warning:
		//
		// Currently the only location in the code where hash collisions will occur for
		// pure spatial partitions is in the 'SetParent' call from the 'SubdividePartition'
		// function in the partition building code. While improbable, it is possible for
		// this collision to occur for this algorithm; it also does not greatly affect
		// performance as this collision does not occur often. Therefore fixing the symptom
		// seems justified, and this will be done by slightly changing the size of the parent
		// which will therefore change its hashing value.
		//
		// To do:
		//
		// Implement a member function which will remove children partitions that are almost
		// the same size as the parent.
		//

		// Grow the parent slightly by some random amounts.
		(static_cast<CPartitionSpace*>(ppart))->GrowRandomly();
		AlwaysAssert(ppart->u4GetUniqueHandle() != u4GetUniqueHandle());
		*/

		// Call the base class handler for this member function.
		CPartition::SetParent(ppart);
	}

	//*****************************************************************************************
	float CPartitionSpace::fDistanceFromGlobalCameraSqr() const
	{
		float f_x = SPartitionSettings::v3GlobalCameraPosition.tX - v3Position.tX;
		float f_y = SPartitionSettings::v3GlobalCameraPosition.tY - v3Position.tY;
		return f_x * f_x + f_y * f_y;
	}

	//*****************************************************************************************
	void CPartitionSpace::GetMinMaxDistance(float& rf_min, float& rf_max, CShapePresence& rsp)
	{
		CVector3<> av3[8];

		bGetBoundingBoxVertices(av3);

		// Get the camera's position in object space.
		CVector3<> v3_cam = rsp.pr3GetCamWorld().v3Pos - v3Position;

		// Get the near and far corners.
		uint u_near_corner = uGetBoxNearestCorner(v3_cam);
		uint u_far_corner  = 0x7 - u_near_corner;

		rf_min = (av3[u_near_corner] - v3_cam).tManhattanDistance();
		rf_max = (av3[u_far_corner]  - v3_cam).tManhattanDistance();

		//
		// Note:
		//		It is not required to rescale as pure spatial partitions are always in world
		//		coordinates.
		//

	#if 0
		// Test results against known results.
		{
			float f_min;
			float f_max;
			CPartition::GetMinMaxDistance(f_min, f_max, rsp);
			AlwaysAssert(bFurryEquals(rf_min, f_min, 0.01f));
			AlwaysAssert(bFurryEquals(rf_max, f_max, 0.01f));
		}
	#endif
	}

	//*****************************************************************************************
	bool CPartitionSpace::bIsEqualToSpatial(const CPartition* ppart) const
	{
		Assert(ppart);

		const CPartitionSpace* parts;	// Pure spatial version of the partition.

		// If the other partition is not a spatial partition, they cannot be equal.
		if (!ppart->bIsPureSpatial())
			goto FAIL_EQUAL;

		// Safe to cast.
		parts = (const CPartitionSpace*)ppart;

		//
		// Match bounding volumes.
		//

		// Match boxes only, for now.
		if (pbvBoundingVol()->ebvGetType() != ebvBOX)
			goto FAIL_EQUAL;

		if (parts->pbvBoundingVol()->ebvGetType() != ebvBOX)
			goto FAIL_EQUAL;

		// Test box extents.
		if (!(bvbBox.v3GetMax() == parts->bvbBox.v3GetMax()))
			goto FAIL_EQUAL;

		// Test box positions.
		if (!(v3Position == parts->v3Position))
			goto FAIL_EQUAL;

		// These partitions are equal.
		AlwaysAssert(u4GetUniqueHandle() == ppart->u4GetUniqueHandle());
		return true;

	FAIL_EQUAL:

		// These partitions are not equal.
		AlwaysAssert(u4GetUniqueHandle() != ppart->u4GetUniqueHandle());
		return false;
	}


//*********************************************************************************************
//
// CPartitionSpaceQuery implementation.
//

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	//*****************************************************************************************
	CPartitionSpaceQuery::CPartitionSpaceQuery(const CPresence3<>& pr3, TReal r_radius)
		: CPartitionSpace(CBoundVolSphere(r_radius)), pr3Pres(pr3)
	{
	}

	//*****************************************************************************************
	CPartitionSpaceQuery::CPartitionSpaceQuery(const CPresence3<>& pr3, const CBoundVol& bv)
		: CPartitionSpace(bv), pr3Pres(pr3)
	{
	}

	//*****************************************************************************************
	CPresence3<> CPartitionSpaceQuery::pr3Presence() const
	{
		return pr3Pres;
	}

	//*****************************************************************************************
	void CPartitionSpaceQuery::SetPresence(const CPresence3<> &pr3)
	{
		pr3Pres = pr3;
	}

	//*****************************************************************************************
	CPlacement3<> CPartitionSpaceQuery::p3Placement() const
	{
		return pr3Pres;
	}

	//*****************************************************************************************
	void CPartitionSpaceQuery::SetPlacement(const CPlacement3<>& p3)
	{
		static_cast<CPlacement3<>&>(pr3Pres) = p3;
	}

	//*****************************************************************************************
	CVector3<> CPartitionSpaceQuery::v3Pos() const
	{
		return pr3Pres.v3Pos;
	}
	
	//*****************************************************************************************
	void CPartitionSpaceQuery::SetPos(const CVector3<>& v3_pos)
	{
		pr3Pres.v3Pos = v3_pos;
	}


// Add this pragma to make sure fhPartitionSpace is initialised before use.
#pragma warning(disable:4073)
#pragma init_seg(lib)

// Virtual memory for allocating the partitioning system.
CFastHeap fhPartitionSpace(1 << 24);