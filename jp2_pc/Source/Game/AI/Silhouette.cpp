/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 *		Implementation of Silhouette.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/Silhouette.cpp                                               $
 * 
 * 23    9/03/98 6:04p Agrant
 * don't merge silhouettes that will total more than 8 edges.
 * 
 * 22    9/02/98 5:20p Agrant
 * New silhouette simplification functions
 * 
 * 21    7/14/98 4:11p Agrant
 * some silhouette improvements
 * 
 * 20    7/11/98 8:07p Agrant
 * Added asserts to help track down silhouette errors
 * 
 * 19    6/08/98 9:45p Agrant
 * Assume simpler silhouette cases more often
 * 
 * 18    3/20/98 4:48p Agrant
 * Okay, when you find a boundvol you don't like, fake a boundvol rather than AlwaysAsserting
 * and then crashing.
 * 
 * 17    3/12/98 7:54p Agrant
 * New silhouette internals (array instead of vector)
 * 2d pathfinding handles compound physics objects
 * 
 * 16    3/06/98 9:34p Agrant
 * for safety, make this always assert
 * 
 * 15    97/09/29 16:34 Speter
 * All bounding volumes are now positionless.  Removed v3Pos from Box and Sphere.  
 * 
 * 14    7/31/97 4:46p Agrant
 * #if not #ifdef for ver_debug
 * 
 * 13    97/06/30 19:22 Speter
 * Removed casts on pbvbCast().  Now returns const*.
 * 
 * 12    5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 11    97/05/21 17:35 Speter
 * Updated CFuzzy for new template parameters.
 * 
 * 10    5/18/97 5:06p Agrant
 * Reworked PUSH(x,x,x) macro in hopes of better inlining for
 * STL vector< CVector2<> > push_back.  
 * 
 * 9     5/04/97 9:30p Agrant
 * Reworked the way activities are handled-  each now operates on an
 * InfluenceList.
 * 
 * 8     97-04-14 12:10 Speter
 * Changed access of removed CBoundVolBox::v3Pos to v3GetOrigin().
 * 
 * 7     3/19/97 2:28p Agrant
 * Added better asserts for the silhouette constructor.
 * 
 * 6     2/10/97 4:15p Agrant
 * Intermediate pathfinding check-in
 * 
 * 5     2/09/97 8:17p Agrant
 * Generalized silhouette code to take arbitrary boundvols-
 * only box is implemented, though.
 * 
 * 4     2/06/97 7:18p Agrant
 * Increased robustness of silhouette validation function.
 * 
 * 3     1/30/97 2:39p Agrant
 * Silhouettes added and pathfinder revision.
 * 
 * 2     1/28/97 6:05p Agrant
 * Intermediate pathfinding revision check in
 * 
 * 1     1/28/97 2:59p Agrant
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Silhouette.hpp"

#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/GeomDBase/LineSegment.hpp"

#include "Lib/Std/stdlibex.hpp"
#include "Lib/Types/FixedP.hpp"


//*********************************************************************************************
//
//	Class CSilhouette
//

	//*********************************************************************************
	CSilhouette::CSilhouette 
	(
		const CBoundVol*		pbv,	// The bounding volume to be projected.
		const CPresence3<>&		pr3,	// The orientation and scale of the bounding box.
		TReal					r_slop
	)
	{
		// Init.
		// for some reason, LINT thinks that this constructor is static, and therefore cannot 
		// perform this init call.  Weird, huh?
		Init(pbv, pr3, r_slop);  //lint !e1032  
	}


	//*********************************************************************************
	void CSilhouette::Init	
	(
		const CBoundVol*        pbv,	// The bounding volume to be projected.
		const CPresence3<>&		pr3,	// The orientation and scale of the bounding box.
		TReal					r_slop
	)
	{
		const CBoundVolBox* pbvb = pbv->pbvbCast();

		// Now, add the right amount of slop to each dimension.
		r_slop /= pr3.rScale;

		// Create a new corner for the local box.
		CVector3<> v3_corner;
		if (pbvb)
		{
			v3_corner = (*pbvb)[0] + CVector3<>(r_slop, r_slop, r_slop);
		}
		else
		{
			v3_corner = CVector3<>(r_slop, r_slop, r_slop);
		}

		// Create the local box.
		CBoundVolBox bvb_local(v3_corner);

		// Init.
		Init(bvb_local, pr3);

		Assert(uLen > 2);
	}

	
	//*********************************************************************************
	void CSilhouette::Init	
	(
		const CBoundVolBox&		bvb,	// The bounding box to be projected.
		const CPresence3<>&		pr3		// The orientation and scale of the bounding box.
	)
	{
		Clear();

		// Okay, first we transform the basic X, Y, and Z axis vectors of the box in to world
		// coordinates.  With these, we can quickly calculate any corner of the box.

		// Throughout this function, the notation (+,+,+) means a vector that is positive in 
		// all of X, Y, and Z.
		// (+,-,0) means a vector that is positive in X, negative in Y, and zero in Z.
		// A corner of a box can be uniquely descibed in local coordinates by one of the eight 
		// vectors that consist only of +'s and -'s.

		// Additionally, the X faces of a box are thes face that are perpendicular to the X axis in
		// local coords.  Y and Z faces are similarly defined.


		// Make a non-translating presence.
		CPresence3<> pr3_no_trans = pr3;
		pr3_no_trans.v3Pos = CVector3<>(0,0,0);


		Assert(bvb[0].tX > 0);
		Assert(bvb[0].tY > 0);
		Assert(bvb[0].tZ > 0);

		
		// Transform the vectors, rotation and scale only.
		CVector3<>	v3_x = CVector3<>(bvb[0].tX, 0, 0) * pr3_no_trans;
		CVector3<>	v3_y = CVector3<>(0, bvb[0].tY, 0) * pr3_no_trans;
		CVector3<>	v3_z = CVector3<>(0, 0, bvb[0].tZ) * pr3_no_trans;
		// With these vectors, the box corner (+,-,-) can be located in world space as
		// bvb.v3Pos + v3_x - v3_y - v3_z

		CVector2<>	v2_x	= v3_x;
		CVector2<>	v2_y	= v3_y;
		CVector2<>	v2_z	= v3_z;


//		Assert(!v2_x.bIsZero());
//		Assert(!v2_y.bIsZero());
//		Assert(!v2_z.bIsZero());



		// The basic concept here is that there are three basic cases of box orientations:
		// Case 0 is none of the axis vectors have a zero Z component in 
		// world space.  When this is the case, you can throw out the highest and lowest corners.
		// All corners that remain are projected onto the XY plane to create a corner of the
		// silhouette polygon.

		// Case 1
		// When one axis vectors has a zero Z component, then you can throw out the two highest
		// corners or the two lowest corners and project the rest onto XY.

		// Case 2
		// When two axes have a zero Z component, you can throw out either the four highest or
		// the four lowest corners.
		//


		// Lets look at case 0, where no axis vectors have zero Z.
		// The highest point is the one described by (sgn(xz), sgn(yz), sgn(zz)) where
		// "?z" is the z component of axis ?.  The lowest is the opposite corner.  Therefore, we
		// want to ditch the two corners whose signs match sgn(?z) in all three positions or in 
		// no positions at all (ditch the ones that match 0 or 3 axes, keep the ones matching
		// 1 or 2).

		// How about Case 1, where 1 axis has zero Z.  Here, there are two highest corners
		// and two lowest corners, and we want none of them.  The two highest corners are
		// the ones whose signs match the non-zero axis Z signs.  The lowest are the ones that don't
		// match either non-zero axis.  This means ditching the corners matching 2 or 0 axes and keeping
		// the ones that match 1 axis.  We can't match all 3, because of the zero.

		// In case 2, we don't care if we throw out high or low, as long as we throw out one or the
		// other consistently.  We can only match 0 or 1 axes since there are two zeros.
		// If we keep ones matching one axis, we can fold this into case 1. 
		

		CFuzzy<TReal, TReal> fuzzy_zero(0, 0.01);

		// returns -1, 0, or 1 depending on sign of "f"
#define iSGN(f) ((fuzzy_zero == f) ? 0 : ((f > 0) ? 1 : -1))

		// Find the sign of the Z component of each axis vector.
		int i_xz_sgn	= iSGN(v3_x.tZ);
		int i_yz_sgn	= iSGN(v3_y.tZ);
		int i_zz_sgn	= iSGN(v3_z.tZ);

		int i_num_zeros	= (!i_xz_sgn) + (!i_yz_sgn) + (!i_zz_sgn);  //lint !e514  // unusual use of a boolean

		// true if sgn1 and sgn2 are both > 0 or both < 0 or one == 0
//#define bMATCH(sgn1, sgn2) ((sgn1) * (sgn2) >= 0)

		// true if the point's projection belongs in the silhouette.
		// macro arguments are + or - 
//#define bINCLUDE(c1, c2, c3, modnum) ((bMATCH(i_xz_sgn, c1 1) + bMATCH(i_yz_sgn, c2 1) + bMATCH(i_zz_sgn, c3 1)) % modnum)


CVector2<>	v2(pr3.v3Pos);

// Pushes the specified corner.
CVector2<>	v2_temp;
#define PUSH(c1, c2, c3) { v2_temp = v2 c1 v2_x c2 v2_y c3 v2_z;  push_back(v2_temp); }

// Adds the corner to the silhouette if the corner helps define the silhouette.
//#define PUSH_IF_GOOD(c1, c2, c3, modnum) if (bINCLUDE(c1, c2, c3, modnum)) push_back(v2 c1 v2_x c2 v2_y c3 v2_z )



// The case analysis method.  Fast, inelegant, and hard to debug.

		if (i_num_zeros != 0)
		{
			if (i_num_zeros == 2)
			{
				// Case 2- one axis points straight up or down.
				// Six cases.

				// The push statements could be a bit more efficient, as we're adding a zero vector.

				if (i_xz_sgn == 1)
				{
					// X points up.
					PUSH(+,+,+);
					PUSH(+,+,-);
					PUSH(+,-,-);
					PUSH(+,-,+);

					Assert(bIsValid());
				}
				else if (i_xz_sgn == -1)
				{
					// X points down.
					PUSH(-,+,+);
					PUSH(-,-,+);
					PUSH(-,-,-);
					PUSH(-,+,-);
					Assert(bIsValid());
				}
				else if (i_yz_sgn == 1)
				{
					// Y points up.
					PUSH(+,+,+);
					PUSH(-,+,+);
					PUSH(-,+,-);
					PUSH(+,+,-);
					Assert(bIsValid());
				}
				else if (i_yz_sgn == -1)
				{
					// Y points down.
					PUSH(-,-,+);
					PUSH(+,-,+);
					PUSH(+,-,-);
					PUSH(-,-,-);
					Assert(bIsValid());
				}
				else if (i_zz_sgn == 1)
				{
					// Z points up.
					PUSH(+,+,+);
					PUSH(+,-,+);
					PUSH(-,-,+);
					PUSH(-,+,+);
					Assert(bIsValid());
				}
				else
				{
					// The only option left....
					Assert(i_zz_sgn == -1);

					// Z points down.
					PUSH(+,+,-);
					PUSH(-,+,-);
					PUSH(-,-,-);
					PUSH(+,-,-);
					Assert(bIsValid());
				}
			}  // Ends case 2.
			else if (i_num_zeros == 1)
			{
				// Case 1
				// This covers twelve cases.
				if (i_xz_sgn == 0)
				{
					// (0,?,?)
					if (i_yz_sgn > 0)
					{	
						// (0,+,?)
						if (i_zz_sgn > 0)
						{
							// (0,+,+)
							PUSH(+,-,+);
							PUSH(-,-,+);
							PUSH(-,+,-);
							PUSH(+,+,-);
							Assert(bIsValid());
						}
						else
						{
							// (0,+,-)
							PUSH(+,-,-);
							PUSH(+,+,+);
							PUSH(-,+,+);
							PUSH(-,-,-);
							Assert(bIsValid());
						}
					}
					else
					{
						// (0,-,?)
						if (i_zz_sgn > 0)
						{
							// (0,-,+)
							PUSH(+,-,-);
							PUSH(-,-,-);
							PUSH(-,+,+);
							PUSH(+,+,+);
							Assert(bIsValid());
						}
						else
						{
							// (0,-,-)
							PUSH(+,-,+);
							PUSH(+,+,-);
							PUSH(-,+,-);
							PUSH(-,-,+);
							Assert(bIsValid());
						}
					}
				}
				else if (i_yz_sgn == 0)
				{
					// (?,0,?)
					if (i_xz_sgn > 0)
					{	
						// (+,0,?)
						if (i_zz_sgn > 0)
						{
							// (+,0,+)
							PUSH(+,+,-);
							PUSH(+,-,-);
							PUSH(-,-,+);
							PUSH(-,+,+);
							Assert(bIsValid());
						}
						else
						{
							// (+,0,-)
							PUSH(+,+,+);
							PUSH(-,+,-);
							PUSH(-,-,-);
							PUSH(+,-,+);
							Assert(bIsValid());
						}
					}
					else
					{
						// (-,0,?)
						if (i_zz_sgn > 0)
						{
							// (-,0,+)
							PUSH(+,+,+);
							PUSH(+,-,+);
							PUSH(-,-,-);
							PUSH(-,+,-);
							Assert(bIsValid());
						}
						else
						{
							// (-,0,-)
							PUSH(+,+,-);
							PUSH(-,+,+);
							PUSH(-,-,+);
							PUSH(+,-,-);
							Assert(bIsValid());
						}
					}
				}
				else
				{
					// (?,?,0)
					if (i_xz_sgn > 0)
					{
						// (+, ?, 0)
						if (i_yz_sgn > 0)
						{
							// (+,+,0)
							PUSH(+,-,+);
							PUSH(-,+,+);
							PUSH(-,+,-);
							PUSH(+,-,-);
							Assert(bIsValid());
						}
						else
						{
							// (+,-,0)
							PUSH(+,+,+);
							PUSH(+,+,-);
							PUSH(-,-,-);
							PUSH(-,-,+);
							Assert(bIsValid());
						}

					}
					else
					{
						// (-,?,0)
						if (i_yz_sgn > 0)
						{
							// (-,+,0)
							PUSH(+,+,+);
							PUSH(-,-,+);
							PUSH(-,-,-);
							PUSH(+,+,-);

							Assert(bIsValid());
						
						}
						else
						{
							// (-,-,0)
							PUSH(+,-,+);
							PUSH(+,-,-);
							PUSH(-,+,-);
							PUSH(-,+,+);

							Assert(bIsValid());
						}
					}

				}
			}
			else
			{
				// 0 zeroes.  What do we do now?
				push_back(v2 + CVector2<>(0.1,0.0));
				push_back(v2 + CVector2<>(0.0,0.0));
				push_back(v2 + CVector2<>(0.0,0.1));
				Assert(bIsValid());

			}

			Assert(bIsValid());
			return;
		}
		else
		{
			// This covers only eight cases.
			// Case 0:
			// Keep those points that match 1 or 2 axes.

			if (i_xz_sgn > 0)
			{
				// (+,?,?)
				if (i_yz_sgn > 0)
				{
					// (+,+,?)
					if (i_zz_sgn > 0)
					{
						// (+,+,+)
						PUSH(+,-,-);
						PUSH(+,-,+);
						PUSH(-,-,+);
						PUSH(-,+,+);
						PUSH(-,+,-);
						PUSH(+,+,-);
						Assert(bIsValid());
					}
					else
					{
					// (+,+,-)
						PUSH(+,+,+);
						PUSH(-,+,+);
						PUSH(-,+,-);
						PUSH(-,-,-);
						PUSH(+,-,-);
						PUSH(+,-,+);
						Assert(bIsValid());
					}
				}
				else
				{
					// (+,-,?)
					if (i_zz_sgn > 0)
					{
						// (+,-,+)
						PUSH(+,-,-);
						PUSH(-,-,-);
						PUSH(-,-,+);
						PUSH(-,+,+);
						PUSH(+,+,+);
						PUSH(+,+,-);
						Assert(bIsValid());
					}
					else
					{
						// (+,-,-)
						PUSH(+,-,+);
						PUSH(+,+,+);
						PUSH(+,+,-);
						PUSH(-,+,-);
						PUSH(-,-,-);
						PUSH(-,-,+);
						Assert(bIsValid());
					}
				}
			}
			else
			{
				// (-,?,?)
				if (i_yz_sgn > 0)
				{
					// (-,+,?)
					if (i_zz_sgn > 0)
					{
						// (-,+,+)
						PUSH(+,+,+);
						PUSH(+,-,+);
						PUSH(-,-,+);
						PUSH(-,-,-);
						PUSH(-,+,-);
						PUSH(+,+,-);
						Assert(bIsValid());
					}
					else
					{
						// (-,+,-)
						PUSH(+,+,+);
						PUSH(-,+,+);
						PUSH(-,-,+);
						PUSH(-,-,-);
						PUSH(+,-,-);
						PUSH(+,+,-);
						Assert(bIsValid());
					}
				}
				else
				{
					// (-,-,?)
					if (i_zz_sgn > 0)
					{
						// (-,-,+)
						PUSH(+,+,+);
						PUSH(+,-,+);
						PUSH(+,-,-);
						PUSH(-,-,-);
						PUSH(-,+,-);
						PUSH(-,+,+);
						Assert(bIsValid());
					}
					else
					{
						// (-,-,-)
						PUSH(-,-,+);
						PUSH(+,-,+);
						PUSH(+,-,-);
						PUSH(+,+,-);
						PUSH(-,+,-);
						PUSH(-,+,+);
						Assert(bIsValid());
					}
				}
			}

		}

		Assert(bIsValid());


/*
// The elegant, two case solution, that does not order the points.


		if (i_num_zeros != 0)
		{
			// Cases 1 and 2:
			//  Keep those points that match exactly one axis sign.
			PUSH_IF_GOOD(+,+,+, 2);
			PUSH_IF_GOOD(+,+,-, 2);
			PUSH_IF_GOOD(+,-,+, 2);
			PUSH_IF_GOOD(+,-,-, 2);
			PUSH_IF_GOOD(-,+,+, 2);
			PUSH_IF_GOOD(-,+,-, 2);
			PUSH_IF_GOOD(-,-,+, 2);
			PUSH_IF_GOOD(-,-,-, 2);
		}
		else
		{
			// Case 0:
			// Keep those points that match 1 or 2 axes.

			PUSH_IF_GOOD(+,+,+,3);
			PUSH_IF_GOOD(+,+,-,3);
			PUSH_IF_GOOD(+,-,+,3);
			PUSH_IF_GOOD(+,-,-,3);
			PUSH_IF_GOOD(-,+,+,3);
			PUSH_IF_GOOD(-,+,-,3);
			PUSH_IF_GOOD(-,-,+,3);
			PUSH_IF_GOOD(-,-,-,3);
		}
		*/
	}

	//*********************************************************************************
	bool CSilhouette::bIsValid()
	{
#if VER_DEBUG
		// Make sure that the silhoutte describes a convex polygon's point moving clockwise.
		int i_size = size();	

		CVector2<> v2_to_C	;
		CVector2<> v2_from_C;

		// For corner C of the poly, make sure that the next corner lies to the right of the
		// edge leading to C.
		for (int i = i_size - 2; i >= 1; i--)
		{
			v2_to_C		= (*this)[i] - (*this)[i - 1];
			v2_from_C	= (*this)[i + 1] - (*this)[i];

			if ((v2_to_C ^ v2_from_C) >= 0)
			{
				Assert(false);
				return false;
			}
		}

		// Now for the 'round the corner tests.
		if (i_size > 1)
		{
			v2_to_C		= (*this)[0] - (*this)[i_size - 1];
			v2_from_C	= (*this)[1] - (*this)[0];

			if ((v2_to_C ^ v2_from_C) >= 0)
			{
				Assert(false);
				return false;
			}

			v2_to_C		= (*this)[i_size - 1] - (*this)[i_size - 2];
			v2_from_C	= (*this)[0] - (*this)[i_size - 1];

			if ((v2_to_C ^ v2_from_C) >= 0)
			{
//				Assert(false);
				return false;
			}
		}
#endif

		return size() > 2;
	}


	//******************************************************************************************
	bool CSilhouette::bEdgeIntersects
	(
		const CLineSegment2<>& ls
	) const
	{
		CLineSegment2<> ls_local((*this)[0], (*this)[size() - 1]);
		if (ls_local.bIntersects(ls))
			return true;
		
		for (int i = size() - 1; i > 0; i--)
		{
			CLineSegment2<> ls_local2((*this)[i], (*this)[i-1]);
			
			if (ls_local2.bIntersects(ls))
				return true;
		}

		// Tested all edges.
		return false;
	}

	//******************************************************************************************
	bool CSilhouette::bMerge
	(
		const CSilhouette* psil_A,
		const CSilhouette* psil_B
	)
	{
		AlwaysAssert(size() == 0);

		// Tries to merge psil_A and psil_B edges into this

		// First, try to find common edge.
		int i_A_vert = 1;
		int i_B_vert = 0;
		int i_A_size = psil_A->size();
		int i_B_size = psil_B->size();
		if (i_A_size + i_B_size > 8)
			return false;

		// Check all but the first of A verts, because if we don't have a match before then, we will never get two matches.
		for ( ; i_A_vert < i_A_size; ++i_A_vert)
		{
			CVector2<> v2_a = (*psil_A)[i_A_vert];
			for (i_B_vert = 0; i_B_vert < i_B_size; ++i_B_vert)
			{
				// Are the verts the same?
				CVector2<> v2_b = (*psil_B)[i_B_vert];

				if (bFuzzyEquals(v2_a.tX, v2_b.tX) && bFuzzyEquals(v2_a.tY, v2_b.tY))
				{
					// Yes!  Check for a second duplicate!
					int i_A_other_vert = (i_A_vert + 1) % i_A_size;
					int i_B_other_vert = (i_B_size + i_B_vert - 1) % i_B_size;

					CVector2<> v2_a_other = (*psil_A)[i_A_other_vert];
					CVector2<> v2_b_other = (*psil_B)[i_B_other_vert];

					// CHECK THESE VERTS
					if (bFuzzyEquals(v2_a_other.tX, v2_b_other.tX) && 
						bFuzzyEquals(v2_a_other.tY, v2_b_other.tY))
					{
						// IF THEY MATCHED, MERGE.

						// Do the merge thing.
						int i_A;
						int i_B;
		
						// Push back all A verts, ending with i_A_vert
						for (i_A = i_A_other_vert; (i_A % i_A_size) != i_A_vert; ++i_A)
						{
							i_A = i_A % i_A_size;
							push_back((*psil_A)[i_A]);
						}

						Assert(size() < i_A_size);

						for (i_B = i_B_vert; (i_B % i_B_size) != i_B_other_vert; ++i_B)
						{
							i_B = i_B % i_B_size;
							push_back((*psil_B)[i_B]);
						}

						return true;
					}
				}
			}
		}

		// Tested all edges.
		return false;
	}

	//******************************************************************************************
	void CSilhouette::RemoveColinearPoints()
	{
		// From last to zero.
		CVector2<> v2_prev;
		CVector2<> v2_next = (*this)[0] - (*this)[size() - 1];
		v2_next.Normalise();

		for (int i = 0; i < size(); ++i)
		{
			v2_prev = v2_next;

			int i_next		= (i + 1) % size();
			v2_next = (*this)[i_next] - (*this)[i];
			v2_next.Normalise();

			TReal r_cross = v2_prev ^ v2_next;
			if (bFuzzyEquals(r_cross,0))
			{
				// Colinear!  Remove this point!
				Remove(i);

				// Pretend we are one back, and continue.
				--i;

				// This works with prev and next because they are... colinear.
			}
		}
	}




	void TestSilhouette()
	{
		CSilhouette sil;
		CBoundVolBox bvb(1,1,1);
		CPresence3<> pr3;

		float f_rot_x;
		float f_rot_y = 0.0f;

		for (; f_rot_y < 1.0; f_rot_y += 0.0001)
		{
			CRotate3<> r3_y = CRotate3<>(d3YAxis, f_rot_y);


			for (f_rot_x = 0.0f; f_rot_x < 1.0; f_rot_x += 0.0001)
			{
				sil.Reset();
				pr3.r3Rot = CRotate3<>(d3XAxis, f_rot_x) * r3_y;

				sil.Init(bvb, pr3);
			}
		}
	}
