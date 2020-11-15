/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of Partition.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/RayCast.cpp                                             $
 * 
 * 24    98/09/12 0:53 Speter
 * Nearest-point raycast now queries objects at 5 times the diameter.
 * 
 * 23    8/25/98 8:37p Rvande
 * Ambiguous access resolved
 * 
 * 22    98/07/08 18:35 Speter
 * Added bVER_BONES() switch.
 * 
 * 21    98/06/29 14:05 Speter
 * Now queries terrain as well.
 * 
 * 20    98/06/25 17:42 Speter
 * RayIntersect function changed to take explicity subobject index.
 * 
 * 19    98/05/22 20:55 Speter
 * Changed raycast construction to let objects insert multiple intersections into list. Now draw
 * intersections in bones.
 * 
 * 18    98/05/06 23:17 Speter
 * If f_length == 0, find nearest point in any direction.
 * 
 * 17    98/04/16 14:23 Speter
 * Now returns SObjectLoc info with precise collision information; utilise CPhysicsInfo
 * functions. Sorting is now correct. Moved container classes into private member pointer; use
 * set<> for automatic sorting on insert.
 * 
 * 16    3/18/98 4:07p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 * 15    98/03/06 10:54 Speter
 * Now store results in vector<>, so it can be sorted. Previous sorting wasn't working, as you
 * can't declare an operator< on pointers. Added DrawPhysics() to show last raycast. Use private
 * anonymous namespace. More accurate collision testing (BVBox intersection test must pass both
 * ways).
 * 
 * 14    98/02/04 14:47 Speter
 * CRayCast now stores and retrieves the querying volume it uses (for debugging).  Now sorts
 * from original raycast origin, not volume centre.
 * 
 * 13    97/12/15 18:54 Speter
 * Fixed several bugs; translation to box centre wasn't done in correct frame, as wasn't used
 * anyway!  ppartNextBoundingCollision() function returned same instance every time.  Changed
 * ppartNextBoundingCollision() to CInstance* pins...  Made DistanceSort() protected; now always
 * sorts.  Made comparison operator more concise.
 * 
 * 12    97/10/30 14:59 Speter
 * Removed #include of dead QSolidObject.hpp.
 * 
 * 11    10/14/97 10:38p Rwyatt
 * Added screen pixel raycast
 * 
 * 10    97/10/14 10:11p Pkeet
 * Changed basic query to a physics query because physics objects are required and the solid
 * object query is broken.
 * 
 * 9     97/09/30 1:27p Pkeet
 * Changed the temporary use of 'CPartitionSpace' to 'CPartitionSpaceQuery' for the query.
 * 
 * 8     97/09/29 16:30 Speter
 * All bounding volumes are now positionless.  Removed v3Pos from Box and Sphere.  
 * 
 * 7     97/09/28 3:08p Pkeet
 * Replace the use of 'CPresence3<>().v3Pos' with 'v3Pos().'
 * 
 * 6     97/09/26 3:34p Pkeet
 * Changed function to use the new form of the CPartitionSpace constructor.
 * 
 * 5     9/03/97 12:27p Pkeet
 * Removed the include for CPartitionChildren.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "RayCast.hpp"

#include "PartitionPriv.hpp"
#include "PartitionSpace.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/EntityDBase/Instance.hpp"
//#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/Query/QPhysics.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/View/LineDraw.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include <set>

//*********************************************************************************************
//
// CRayCast implementation.
//

	//*****************************************************************************************
	//
	namespace
	//
	// Private static implementation.
	//
	//**********************************
	{
#if bVER_BONES()
		CPartitionSpaceQuery	partsqLast(CPresence3<>(), CBoundVolBox(1.0, 1.0, 1.0));
		aptr<CRayCast>			prcLast;
#endif
	};

	//**************************************************************************************
	//
	inline bool operator <(const SObjectLoc& obl1, const SObjectLoc& obl2)
	//
	// Comparision operator for SObjectLoc, for inserting into our set.
	// Sorts by distance from origin.
	//
	//******************************
	{
		return obl1.rDist < obl2.rDist;
	}

	//**************************************************************************************
	//
	class CRayCast::CPrivate: public CRefObj
	//
	// Private implementation class.
	//
	//**********************************
	{
	public:
		// The result list.
		CContainer< std::set<SObjectLoc, std::less<SObjectLoc> > >	setoblResults;
	};

	//*****************************************************************************************
	CRayCast::CRayCast(const CPresence3<>& pr3, float f_diameter, float f_length)
	{
		ppriv = rptr_new CPrivate();

		// The volume for the query.
		// Construct a centred box for the nearest point query when f_length == 0.
		CPartitionSpaceQuery	partsq(pr3, 
			f_length ? CBoundVolBox(f_diameter, f_length, f_diameter)
			: CBoundVolBox(f_diameter*5, f_diameter*5, f_diameter*5));

		// Move the origin to the centre of the box in Y.  It is already centred in XZ.
		partsq.SetPos(pr3.v3Pos + d3YAxis * (f_length/2 * pr3.rScale) * pr3.r3Rot);

		//
		// Create a query solid objects class constructed with the above partition.
		// Use this to find physics objects in the world which MAY intersect the ray.
		// (this also calls begin for its container)
		//
		CWDbQueryPhysics wqsolobj(&partsq);

		// Also do terrain.
		if (CWDbQueryTerrain().tGet())
			wqsolobj.push_back(CWDbQueryTerrain().tGet());
		wqsolobj.Begin();

		//
		// Iterate through these objects, finding their actual points of intersection,
		// if any, and copy to list. The object's physics info is responsible for this.
		//
		// After this operation the list is ordered by distance to object collisions.
		// The terrain is an intangible object and will not be in the list.
		//

		foreach (wqsolobj)
		{
			CInstance* pins = *wqsolobj;
			pins->pphiGetPhysicsInfo()->RayIntersect(pins, 0, *this, pr3, f_length, f_diameter);
		}

		// Put the next pointer back to the start
		ppriv->setoblResults.Begin();

#if bVER_BONES()
		// Copy result to static member, for physics debugging.
		prcLast = new CRayCast(*this);
		partsqLast = partsq;
#endif
	}

	//*****************************************************************************************
	CRayCast::~CRayCast()
	{
	}

	//******************************************************************************************
	void CRayCast::InsertIntersection(SObjectLoc& obl)
	{
		// Just add it to the set, automatically sorted by distance.
		ppriv->setoblResults.insert(obl);
	}

	//******************************************************************************************
	void CRayCast::ResetIntersectionList()
	{
		ppriv->setoblResults.Begin();
	}

	//*****************************************************************************************
	CInstance* CRayCast::pinsNextObject()
	//
	// Give the next object in the ray collision list, the list is used in its current state.
	// This will continually return NULL once the last element has been reached; this may be
	// changed later to Assert if it is called at the end.
	//
	{
		// Are there any more in our container list?
		if (ppriv->setoblResults)
		{
			SObjectLoc* pobl = &*ppriv->setoblResults;
			++ppriv->setoblResults;
			return pobl->pinsObject;
		}

		return 0;
	}

	//*****************************************************************************************
	SObjectLoc* CRayCast::poblNextIntersection()
	//
	{
		// Are there any more in our container list?
		if (ppriv->setoblResults)
		{
			SObjectLoc* pobl = &*ppriv->setoblResults;
			++ppriv->setoblResults;
			return pobl;
		}

		return 0;
	}

	//******************************************************************************************
	void CRayCast::DrawPhysics(CDraw& draw, CCamera& cam)
	{
#if bVER_BONES()
		// Draw the last raycast if it exists.
		if ((const CRayCast*)prcLast)
		{
			const CBoundVolBox* pbvb = partsqLast.pbvBoundingVol()->pbvbCast();
			if (pbvb)
			{
				// Draw ray-cast box.
				draw.Colour(CColour(0.5, 0.5, 0.5));

				CTransform3<> tf3_screen = cam.tf3ToHomogeneousScreen();
				draw.Box3D(pbvb->tf3Box(partsqLast.pr3Presence() * tf3_screen));

				// And all collisions.
				draw.Colour(CColour(1.0, 1.0, 1.0));
				prcLast->ResetIntersectionList();
				SObjectLoc* pobl;
				while (pobl = prcLast->poblNextIntersection())
				{
					// Draw a small vector from the collision location in the direction of the face normal.
					draw.Line3D
					(
						pobl->v3Location * tf3_screen,
						(pobl->v3Location + pobl->d3Face * 0.1) * tf3_screen
					);
				}
			}
		}
#endif
	}

/*
//*********************************************************************************************
//
CRayCast* rcCastScreenRay
(
	float	f_xpos,			// normalized screen co-ord, -1,-1 top left -> 1,1 bottom right
	float	f_ypos,
	float	f_ray_length,
	float	f_ray_diam
)
//
//**************************************
{
	CVector3<>		v3_ray;
	CVector3<>		v3_cam;
	CDir3<>			d3_ray;
	CDir3<>			d3_cam;
	CPresence3<>	pr3;		// Presence of the camera

	// Query the world database to get the current active camera.
	CWDbQueryActiveCamera wqcam(wWorld);
	CCamera* pcam = wqcam.tGet();
	Assert(pcam);

	// Construct an inverse transform for the camera.
	CTransform3<> tf3_invcam = ~pcam->tf3ToNormalisedCamera();

	// Get the presence of the camera.
	pr3 = pcam->pr3Presence();

	// the required vectors in normalized camera space
	v3_ray = CVector3<>(  f_xpos, 1.0f,  f_ypos);	// vector from given screen pos
	v3_cam = CVector3<>(  0.0f,   1.0f,  0.0f);		// camera Y

	// Make direction vectors in world space for the two vectors above
	d3_ray		= CDir3<>( ((v3_ray * tf3_invcam) - pr3.v3Pos) , false);
	d3_cam		= CDir3<>( ((v3_cam * tf3_invcam) - pr3.v3Pos), false);

	// make a rotation from the two vectors and
	// concatenate with the orientation of the local presence (the camera)
	pr3*=CRotate3<>(d3_cam, d3_ray);

	//
	// pr3 in world space is based at the camera's origin and points in the direction of d3_ray.
	// Now cast a ray as usuall
	//
	return new CRayCast(pr3, f_ray_diam, f_ray_length);
}
*/