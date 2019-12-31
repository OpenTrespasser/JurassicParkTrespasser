/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998
 *
 * Contents: Implementation of 'FindDuplicates.hpp'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/FindDuplicates.cpp                                            $
 * 
 * 8     98/09/17 16:38 Speter
 * Update due to DebugConsole.hpp.
 * 
 * 7     8/30/98 4:52p Asouth
 * added default constructor for MW build
 * 
 * 6     8/19/98 1:43p Rwyatt
 * Platonic instances are hash based
 * 
 * 5     8/16/98 3:10p Agrant
 * Make text out scale relative to master
 * 
 * 4     98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 3     7/31/98 4:16p Agrant
 * only share if you have the same materials, friction, density, etc
 * 
 * 2     7/31/98 3:16p Agrant
 * real implementation
 * 
 * 1     7/30/98 5:52p Agrant
 * initial rev
 * 
 *********************************************************************************************/


//
// Includes.
//
#include "Common.hpp"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Loader/PlatonicInstance.hpp"
#include "Game/AI/AIInfo.hpp"
#include "Lib/Physics/InfoBox.hpp"
#include "Lib/Transform/TransformIO.hpp"

#include <fstream>
#include <set>

float fTolerance = 0.01f;

class CDescription {
public:
	// default constructor added so MSL can construct the class
	CDescription();
	CDescription(const CInstance* pins);

	const CInstance* pinsBase;

	CVector3<> v3Extent;
	TSoundMaterial	matMaterial;
	float			fDensity;
	float			fFriction;
	float			fElasticity;
	bool bPathfind;

	bool operator<(const CDescription&) const;
};

extern TMapHashPins tmPlatonicIdeal;

typedef std::set<CDescription, std::less<CDescription> > TSetDescPins;
TSetDescPins tsdpDescriptions;


bool bIsCandidate(const CInstance* pins)
{
	const CPhysicsInfo* pphi = pins->pphiGetPhysicsInfo();

	// Must have physics info.  Must be immovable.
	if (!pphi || pphi->bIsMovable() || !pphi->bIsTangible())
		return false;

	rptr_const<CMesh> pmsh = rptr_const_dynamic_cast(CMesh, pins->prdtGetRenderInfo());

	if (!pmsh || pmsh->pampPolygons.uLen == 0)
	{
		const CBoundVol* pbv = pins->pphiGetPhysicsInfo()->pbvGetBoundVol();
		const CBoundVolBox* pbvb = pbv->pbvbCast();
		
		if (pbvb)
		{
			if (!((CPhysicsInfoBox*)pphi)->v3Pivot.bIsZero())
			{
				dout << pins->strGetInstanceName() << " --off center pivot point! " << ((CPhysicsInfoBox*)pphi)->v3Pivot << "\n";
				return false;
			}
			return true;
		}
	}

	return false;
}


void FindDuplicates()
{
	std::ofstream ofs("Duplicates.txt");
	if (!ofs.is_open())
	{
		// Some sort of failure.
		return;
	}


	// The error allowed in matching boxes.
	float f_tolerance = .001;

	// First, make a list of all instances.
	TMapHashPins::iterator i = tmPlatonicIdeal.begin();

	for ( ; i != tmPlatonicIdeal.end(); ++i)
	{
		const CInstance* pins = (*i).second;

		if (pins)
		{
			// Is it a good candidate?
			if (bIsCandidate(pins))
			{
				// Yes!
				std::pair<TSetDescPins::iterator, bool> p = tsdpDescriptions.insert(CDescription(pins));

				if (!p.second)
				{
					// Match!
					void DuplicateTextOut(std::ofstream&, const CInstance*, const CInstance*);
					DuplicateTextOut(ofs, (*p.first).pinsBase, pins);
				}
			}
		}
	}
}



CVector3<> v3MakePositive(CVector3<> v3)
{
	v3.tX = abs(v3.tX);
	v3.tY = abs(v3.tY);	
	v3.tZ = abs(v3.tZ);
	return v3;
}

bool bVectorsMatch(const CVector3<>& v3_a, const CVector3<>& v3_b)
{
	CVector3<> v3_diff = v3_a - v3_b;

	return (v3_diff.tLenSqr() < 3 * fTolerance * fTolerance);
}


CRotate3<> r3FindRot(const CVector3<>& v3_master, const CVector3<>& v3_copy)
{
	CRotate3<> r3;

	// Returns the rotation that takes only 90 degree turns to get from master to copy
	if (bVectorsMatch(v3_master, v3_copy))	// XYZ
	{
		// Natural instance.  Just take the copy's presence.
		return r3;
	}

#define PID2 (0.5 * 3.14159265)
	// Jump through hoops.
	CRotate3<> r3_x = CRotate3<>(CVector3<>(1,0,0), PID2);
	CRotate3<> r3_y = CRotate3<>(CVector3<>(0,1,0), PID2);
	CRotate3<> r3_z = CRotate3<>(CVector3<>(0,0,1), PID2);

	r3 = r3_x;
	CVector3<> v3 = v3_copy * r3;	// XZY
	v3 = v3MakePositive(v3);
	if (bVectorsMatch(v3_master, v3))
	{
		return r3;
	}

	r3 = r3 * r3_z;
	v3 = v3_copy * r3;	// ZXY
	v3 = v3MakePositive(v3);
	if (bVectorsMatch(v3_master, v3))
	{
		return r3;
	}
						  
	r3 = r3 * r3_x;
	v3 = v3_copy * r3;	// ZYX
	v3 = v3MakePositive(v3);
	if (bVectorsMatch(v3_master, v3))
	{
		return r3;
	}
	
	r3 = r3 * r3_z;
	v3 = v3_copy * r3;	// YZX
	v3 = v3MakePositive(v3);
	if (bVectorsMatch(v3_master, v3))
	{
		return r3;
	}

	r3 = r3 * r3_x;
	v3 = v3_copy * r3;	// YXZ
	v3 = v3MakePositive(v3);
	if (bVectorsMatch(v3_master, v3))
	{
		return r3;
	}

	AlwaysAssert(false);
	return r3;
}





void DuplicateTextOut(std::ofstream& ofs, const CInstance* pins_master, const CInstance* pins_copy)
{
	// Get the physics infos:
	const CPhysicsInfoBox* pphib_master = (CPhysicsInfoBox*)pins_master->pphiGetPhysicsInfo();
	const CPhysicsInfoBox* pphib_copy	= (CPhysicsInfoBox*)pins_copy->pphiGetPhysicsInfo();

	// Assume that the pivot points are centered!
	AlwaysAssert(pphib_master->v3Pivot.bIsZero());
	AlwaysAssert(pphib_copy->v3Pivot.bIsZero());

	// Calculate a presence for the master such that if set, it will exactly coincide with the copy.
	CPresence3<> pr3_copy = pins_copy->pr3Presence();

	CVector3<> v3_master = pphib_master->bvbBoundVol.v3GetMax();
	CVector3<> v3_copy	 = pphib_copy->bvbBoundVol.v3GetMax();

	pr3_copy.r3Rot = pr3_copy.r3Rot * r3FindRot(v3_master, v3_copy);


	///  WRITING!
	ofs << pins_master->strGetInstanceName() << ' ' << pins_copy->strGetInstanceName();

	// Write its placement.  
	ofs << ' ' << pr3_copy;

	ofs << ' ' << pins_copy->fGetScale() / pins_master->fGetScale();

	ofs << std::endl;
}



CDescription::CDescription(const CInstance* pins)
{
	pinsBase = pins;

	const CPhysicsInfoBox* pphib = (CPhysicsInfoBox*)pins->pphiGetPhysicsInfo();
	const CBoundVol* pbv = pphib->pbvGetBoundVol();
	const CBoundVolBox* pbvb = pbv->pbvbCast();
	AlwaysAssert(pbvb);

	bPathfind = pins->paiiGetAIInfo()->bPathfinding();

	matMaterial = pphib->smatGetMaterialType();
	fDensity	= pphib->fDensity;
	fFriction	= pphib->fFriction;
	fElasticity = pphib->fElasticity;

	// v3Extent should be the physical bounding volume extents, in descending order of magnitude.
	CVector3<> v3 = pbvb->v3GetMax();

	if (v3.tY < v3.tZ)
	{
		Swap(v3.tY, v3.tZ);
	}

	if (v3.tX < v3.tY)
	{
		Swap(v3.tX, v3.tY);
	}

	if (v3.tY < v3.tZ)
	{
		Swap(v3.tY, v3.tZ);
	}

	v3Extent = v3;

	AlwaysAssert(v3.tX >= v3.tY);
	AlwaysAssert(v3.tY >= v3.tZ);
	AlwaysAssert(v3.tX >  0.99f);
}


bool CDescription::operator<(const CDescription& desc) const
{
	float f_low = 1.0 - fTolerance;

	if (!bPathfind && desc.bPathfind)
		return true;

	if (matMaterial < desc.matMaterial)
		return true;
	if (fDensity < desc.fDensity)
		return true;
	if (fFriction < desc.fFriction)
		return true;
	if (fElasticity < desc.fElasticity)
		return true;
	if (v3Extent.tX < desc.v3Extent.tY * f_low)
		return true;
	else if (v3Extent.tY < desc.v3Extent.tY * f_low)
		return true;
	else if (v3Extent.tZ < desc.v3Extent.tZ * f_low)
		return true;
	else
		return false;
}
