/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Definition of a closed convex polygonal clipping region in the XY plane.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ClipRegion2D.hpp                                         $
 * 
 * 6     2/20/98 7:55p Mlange
 * Defined CClipRegion2D::CClipEdge::esfSideOf() out of line.
 * 
 * 5     98/02/10 13:15 Speter
 * Changed original CSArray to CMSArray.
 * 
 * 4     2/04/98 12:21a Gfavor
 * Converted esfSideOf to 3DX.
 * 
 * 3     11/13/97 1:05p Mlange
 * Inlined some functions.
 * 
 * 2     11/13/97 12:50p Mlange
 * Fixed bug. CClipEdge::esfSideOf() now considers the edge to be infinite for the purposes of
 * the intersection test.
 * 
 * 1     11/13/97 11:01a Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_CLIPREGION2D_HPP
#define HEADER_LIB_RENDERER_CLIPREGION2D_HPP

#include "Lib/Transform/Presence.hpp"
#include "Lib/Transform/Vector.hpp"
#include "Lib/GeomDBase/Plane.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "AsmSupport.hpp"


//**********************************************************************************************
//
// Declarations for CClipRegion2D.
//

#define iMAX_REGION_CLIP_EDGES	5	// Max number of edges defining the clipping region.

class CCamera;
class CDraw;


//**********************************************************************************************
//
class CClipRegion2D
//
// Definition of a closed convex polygonal clipping region in the XY plane.
//
// Prefix: clip2d
//
//**************************************
{
	//**********************************************************************************************
	//
	class CClipEdge
	//
	// Definition of a single edge of the clip region.
	//
	// Prefix: cle
	//
	//**************************************
	{
	public:
		CVector2<> v2A;		// Start vertex.
		CVector2<> v2B;		// End vertex.

		//******************************************************************************************
		//
		// Constructors and destructor.
		//
		CClipEdge()
		{
		}

		// Initialiser constructor.
		CClipEdge(const CVector2<>& v2_a, const CVector2<>& v2_b)
			: v2A(v2_a), v2B(v2_b)
		{
		}


		//******************************************************************************************
		//
		ESideOf esfSideOf
		(
			const CVector2<>& v2_centre,	// Centre of circle to test.
			TReal r_radius_sqr				// Squared radius of circle.
		) const;
		//
		// Returns:
		//		Flag indicating whether the given circle lies to left of, right of, or intersects
		//		the infinite line through this edge.
		//
		//**************************************


		//******************************************************************************************
		//
		TReal rSignedDist
		(
			const CVector2<>& v2_test	// Point to test.
		) const
		//
		// Returns:
		//		The signed distance from the point to this (infinite) edge.
		//
		// Notes:
		//		Considering the directed, infinite line through the start and endpoint of this edge,
		//		the distance returned is negative if the point lies to the left of, positive if it lies
		//		to the right of, and zero if it lies on the (infinite) edge.
		//
		//**************************************
		{
			CVector2<> v2_d = v2B - v2A;

			// Adapted from the FAQ at: http://www.cis.ohio-state.edu/hypertext/faq/usenet/graphics/algorithms-faq/faq.html.
			TReal r_d = (v2_test.tX - v2A.tX) * v2_d.tY - (v2_test.tY - v2A.tY) * v2_d.tX;

			return r_d / v2_d.tLen();
		}
	};



	CMSArray<CClipEdge, iMAX_REGION_CLIP_EDGES> sacleEdges;
										// Array of edges that form the clipping region. The edges may be in any order.
										// The inside of the clipping region lies to the left of each edge.


public:
	//**********************************************************************************************
	//
	class CClipInfo
	//
	// Object describing result of an intersection test.
	//
	// Prefix: cli
	//
	//**************************************
	{
	protected:
		uint uState;	// Result of intersection test. The first two bits hold the ESideOf state. The
						// remaining bits act as bool flags, one for each edge in the clipping region - if
						// set, it indicates the corresponding edge needs to be checked for intersection.

	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//
		CClipInfo()
			: uState(BitMaskRange(0, iMAX_REGION_CLIP_EDGES + 2))
		{
		}


		operator ESideOf() const
		{
			return uState & 3;
		}
	};


private:
	//**********************************************************************************************
	//
	class CClipInfoCreate : public CClipInfo
	//
	// Helper class for creating and accessing CClipInfo types.
	//
	// Prefix: clic
	//
	//**************************************
	{
		uint uMask;		// Bit mask to isolate clip state flag for the current edge.

	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//
		CClipInfoCreate(CClipInfo cli)
			: CClipInfo(cli), uMask(4)
		{
			uState &= ~3;
		}

		// Returns 'true' if the current edge needs to be clipped with.
		operator bool() const
		{
			return uState & uMask;
		}


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		void SetEdgeInfo
		(
			ESideOf esf
		)
		//
		// Set the state info for the current edge.
		//
		//**************************************
		{
			if (esf == esfOUTSIDE)
			{
				uState = esfOUTSIDE;
				return;
			}

			// Accumulate the overall intersection test result.
			uState |= esf;

			Assert(esfINSIDE == 1 && esfOUTSIDE == 2 && esfINTERSECT == 3);

			// Create a mask for the current edge flag bit, that is 1 when esf == esfINTERSECT, 0 otherwise.
			uint u_intersect_mask = ((2 - esf) & ~3) & uMask;

			uState &= ~uMask;
			uState |= u_intersect_mask;
		}


		//******************************************************************************************
		//
		void Next()
		//
		// Advance to the next edge.
		//
		//**************************************
		{
			uMask <<= 1;
		}
	};


public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CClipRegion2D()
	{
	}

	CClipRegion2D(const CCamera& cam)
	{
		Init(cam);
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	CClipInfo cliIntersects
	(
		const CVector2<>& v2_centre,		// Centre of circle to test.
		TReal r_radius_sqr,					// Squared radius of circle.
		CClipInfo cli_mask = CClipInfo()	// Clip info object describing which edges need
											// to be clipped with.
	) const
	//
	// Returns:
	//		Clip info object describing the result of the intersection test.
	//
	//**************************************
	{
		CClipInfoCreate clic_mask(cli_mask);

		for (int i_curr = 0; i_curr < sacleEdges.uLen; i_curr++)
		{
			// Do we need to clip with this edge?
			if (clic_mask)
			{
				ESideOf esf_edge = sacleEdges[i_curr].esfSideOf(v2_centre, r_radius_sqr);

				clic_mask.SetEdgeInfo(esf_edge);

				// If the circle is outside one of the edges, we're done.
				if (esf_edge == esfOUTSIDE)
					break;
			}

			clic_mask.Next();
		}

		return clic_mask;
	}




	//******************************************************************************************
	//
	void Init
	(
		const CCamera& cam
	);
	//
	// Construct the clipping region from a parallel projection of 3d camera's world space volume
	// onto the XY plane.
	//
	// Notes:
	//		The resulting 2d clipping region DOES NOT include the camera's near clipping plane!
	//		That is, we consider the world space convex polyhedron formed by the four points that
	//		define the camera's far  clipping plane and the origin of the camera.
	//
	//**************************************


	//******************************************************************************************
	//
	void Draw
	(
		const CDraw& draw
	) const;
	//
	// Draw the clipping region in wireframe.
	//
	//**************************************


	//******************************************************************************************
	//
	// Operators.
	//

	// Transform the clip region.
	void operator*= (const CPresence3<>& pr3);
};


#endif
