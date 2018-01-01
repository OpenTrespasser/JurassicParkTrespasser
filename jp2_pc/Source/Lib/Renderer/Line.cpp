/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of CLine2D.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Line.cpp                                                    $
 * 
 * 19    98.02.05 4:49p Mmouni
 * Removed bad asserts from bFastIntersect2D().
 * 
 * 18    98.02.04 2:00p Mmouni
 * Added fast 2d intersection function.
 * 
 * 17    11/11/97 4:38p Gfavor
 * Cosmetic change.
 * 
 * 16    11/11/97 3:25p Gfavor
 * Eliminated degraded predecode from 3DX code.
 * 
 * 15    11/10/97 10:08p Gfavor
 * Converted bEncloses2DPoint and bINtersect2D to use 3DX.
 * 
 * 14    97/07/15 12:56p Pkeet
 * Moved 'iLineSide' function out of this module.
 * 
 * 13    6/19/97 2:53p Mlange
 * Moved Profile.hpp to Lib/Sys.
 * 
 * 12    97/05/19 6:47p Pkeet
 * Removed timer counter.
 * 
 * 11    97/05/19 6:40p Pkeet
 * Added assembly optimizations.
 * 
 * 10    97/05/16 11:23a Pkeet
 * Fixed bug in optimizations.
 * 
 * 9     97/05/15 4:22p Pkeet
 * Added a few optimizations.
 * 
 * 8     97/04/28 5:46p Pkeet
 * Put the iSIDE macro definitions into this module instead of the depth sort module.
 * 
 * 7     97/04/25 2:19p Pkeet
 * Altered line intersection code to use the new implementation of CLine2D.
 * 
 * 6     97/04/23 10:28a Pkeet
 * Moved the 'CLine2D' class into a seperate module.
 * 
 * 5     97/04/21 10:54a Pkeet
 * Added the 'bTouch' and 'bShared' functions.
 * 
 * 4     97/04/17 4:13p Pkeet
 * Added alternate line intersection code.
 * 
 * 3     97/04/15 1:50p Pkeet
 * Added use of the 'u4FromFloat' macro.
 * 
 * 2     97/04/14 3:46p Pkeet
 * Fixed some bugs.
 * 
 * 1     97/04/13 1:53p Pkeet
 * Initial implementation. Code moved here from the depth sort module.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Line2D.hpp"
#include "Line.hpp"
#include "LineSide2D.hpp"
#include "AsmSupport.hpp"


//******************************************************************************************
//
inline bool bEncloses2DPoint
(
	const CRenderPolygon* prpoly_a,	// Pointer to the first render polygon.
	const CRenderPolygon* prpoly_b	// Pointer to the second render polygon.
)
//
// Returns 'true' if one of the vertices of polygon b is contained in polygon a in a 2D
// projection of polygons in screen space.
//
//**********************************
{
	Assert(prpoly_a);
	Assert(prpoly_b);

#if (VER_ASM)

	struct SVector
	{
		float fX;
		float fY;
		float fZ;
	};

	SRenderVertex** prv = prpoly_a->paprvPolyVertices.atArray;
	uint32 u_offset;
	{
		SRenderVertex* p = 0;
		u_offset = uint32((void*)&p->v3Screen);
	}

	uint u_len_b = prpoly_b->paprvPolyVertices.uLen;
	
	// Iterate through the vertices of polygon b.
	for (int i_vert = 0; i_vert < u_len_b; i_vert++)
	{
		int i_side = 0;	// Side of line flags.
		uint u_len_a = prpoly_a->paprvPolyVertices.uLen;

		// Get the vertex to test.
		CVector3<>* pv3 = &prpoly_b->paprvPolyVertices[i_vert]->v3Screen;

		__asm
		{
			xor esi, esi
			mov ebx, pv3
			xor ecx, ecx
			mov edx, [i_side]
			inc esi
			mov eax, prv

		START_VERTEX_LOOP:

			cmp esi, u_len_a
			jl short SKIP_MODULUS

			xor esi, esi
		SKIP_MODULUS:

			mov edi, [eax+ecx*4]
			mov esi, [eax+esi*4]

			add edi, u_offset
			add esi, u_offset

			fld  [ebx]SVector.fY
			fsub [edi]SVector.fY
			fld  [esi]SVector.fX
			fsub [edi]SVector.fX
			fmul

			fld  [esi]SVector.fY
			fsub [edi]SVector.fY
			fld  [ebx]SVector.fX
			fsub [edi]SVector.fX
			fmul

			fcompp
			fstsw ax

			sahf

			jz short NEXT_VERTEX
			jnc short POS_SIDE

			or edx, iSIDE_NEG
			jmp short STORE_SIDE_FLAGS

		POS_SIDE:
			or edx, iSIDE_POS

		STORE_SIDE_FLAGS:

			cmp edx, iSIDE_BOTH
			je short BREAK_VERTEX_LOOP

		NEXT_VERTEX:

			mov esi, ecx
			mov eax, prv

			add ecx, 1
			add esi, 2

			cmp ecx, u_len_a
			jl short START_VERTEX_LOOP

		BREAK_VERTEX_LOOP:
			mov [i_side], edx
		}

		// If the point is on the same side of each edge, it is enclosed.
		if (i_side != iSIDE_BOTH)
			return true;
	}

	// The point must be on the same side.
	return false;

#else // if VER_ASM

	// Iterate through the vertices of polygon b.
	for (int i_vert = 0; i_vert < prpoly_b->paprvPolyVertices.uLen; i_vert++)
	{
		int i_side = 0;	// Side of line flags.

		// Get the vertex to test.
		CVector3<>* pv3 = &prpoly_b->paprvPolyVertices[i_vert]->v3Screen;

		for (int i_edge = 0; i_edge < prpoly_a->paprvPolyVertices.uLen; i_edge++)
		{
			// Get the points that make up the edge.
			int i_edge_b = i_edge + 1;
			if (i_edge_b >= prpoly_a->paprvPolyVertices.uLen)
				i_edge_b = 0;

			CVector3<>* pv3_edge_a = &prpoly_a->paprvPolyVertices[i_edge]->v3Screen;
			CVector3<>* pv3_edge_b = &prpoly_a->paprvPolyVertices[i_edge_b]->v3Screen;

			// Accumulate results of side test.
			i_side |= iLineSide(*pv3_edge_a, *pv3_edge_b, *pv3);

			// If the point is not on just one side, the test has failed.
			if (i_side == iSIDE_BOTH)
				break;
		}
		// If the point is on the same side of each edge, it is enclosed.
		if (i_side != iSIDE_BOTH)
			return true;
	}

	// The point must be on the same side.
	return false;

#endif // else

}


//
// Externally defined global functions.
//

//**********************************************************************************************
bool bIntersect2D(const CRenderPolygon* prpoly_a, const CRenderPolygon* prpoly_b)
{
	Assert(prpoly_a);
	Assert(prpoly_b);

	//
	// Trivial acceptance of intersection. This test will cover cases where any vertex of one
	// polygon is inside the other polygon; this includes the case where one polygon is entirely
	// enclosed by another polygon -- this case would not be detected by edge intersection.
	//
	if (bEncloses2DPoint(prpoly_a, prpoly_b) || bEncloses2DPoint(prpoly_b, prpoly_a))
	{
		return true;
	}

	//
	// Test each of the edges of polygon a against each of the edges of polygon b and
	// test if any combination of these intersect.
	//
	for (int i_edge_a = 0; i_edge_a < prpoly_a->paprvPolyVertices.uLen; i_edge_a++)
	{
		// Get the points that make up the edge.
		int i_edge_a_to = (i_edge_a == prpoly_a->paprvPolyVertices.uLen - 1) ?
			              (0) : (i_edge_a + 1);

		CLine2D line_a
		(
			prpoly_a->paprvPolyVertices[i_edge_a]->v3Screen,
			prpoly_a->paprvPolyVertices[i_edge_a_to]->v3Screen
		);
		
		// Iterate through polygon B's line segments.
		for (int i_edge_b = 0; i_edge_b < prpoly_b->paprvPolyVertices.uLen - 1; i_edge_b++)
		{
			// Get the points that make up the edge.
			int i_edge_b_to = i_edge_b + 1;
			if (line_a.bDoesIntersectFast(prpoly_b->paprvPolyVertices[i_edge_b]->v3Screen,
				                          prpoly_b->paprvPolyVertices[i_edge_b_to]->v3Screen))
				return true;
		}
		
		#if DO_FULL_INTERSECT_CHECK
			// Get the points that make up the last edge to check.
			// int i_edge_b = prpoly_b->paprvPolyVertices.uLen - 1;
			int i_edge_b_to = 0;
			if (line_a.bDoesIntersectFast(prpoly_b->paprvPolyVertices[i_edge_b]->v3Screen,
										  prpoly_b->paprvPolyVertices[i_edge_b_to]->v3Screen))
				return true;
		#endif
	}

	// All tests for intersection or containment have failed, the polygons do not intersect.
	return false;
}


//**********************************************************************************************
//
bool bFastIntersect2D
(
	const CRenderPolygon* prpoly_p,		// Pointer to the first render polygon.
	const CRenderPolygon* prpoly_q		// Pointer to the second render polygon.
)
//
// Returns 'true' if the 2D projections of the polygon intersect in screen space.
//
// Notes:	The polygons must be in the standard winding order.
//			Polygons with more than 32 vertices may not work correctly.
//
//**********************************
{
	// Check all points of P againts all edges of Q.
	SRenderVertex *prv_q1 = prpoly_q->paprvPolyVertices.atArray[prpoly_q->paprvPolyVertices.uLen-1];
	uint32 u4_and_codes = 0xffffffff;
	for (int i = 0; i < prpoly_q->paprvPolyVertices.uLen; i++)
	{
		SRenderVertex *prv_q2 = prpoly_q->paprvPolyVertices.atArray[i];

		float f_qdx = prv_q2->v3Screen.tX - prv_q1->v3Screen.tX;
		float f_qdy = prv_q2->v3Screen.tY - prv_q1->v3Screen.tY;

		uint32 u4_edge_codes = 0;

		// For each point of P.
		for (int j = 0; j < prpoly_p->paprvPolyVertices.uLen; j++)
		{
			SRenderVertex *prv_p = prpoly_p->paprvPolyVertices.atArray[j];

			float f_cross = f_qdx * (prv_p->v3Screen.tY - prv_q1->v3Screen.tY) -
							f_qdy * (prv_p->v3Screen.tX - prv_q1->v3Screen.tX);

			u4_edge_codes = (u4_edge_codes << 1) | (f_cross < 0.0f);
		}

		// All points outside this edge?
		if (u4_edge_codes == 0)
		{
			// No intersection.
			return false;
		}

		u4_and_codes &= u4_edge_codes;
		prv_q1 = prv_q2;
	}

	// Any point inside all edges?
	if (u4_and_codes == 0)
	{
		// No, polygons may or may not intersect, need to do further tests.

		// Check all points of Q against all edges of P.
		SRenderVertex *prv_p1 = prpoly_p->paprvPolyVertices.atArray[prpoly_p->paprvPolyVertices.uLen-1];
		uint32 u4_and_codes = 0xffffffff;
		for (int i = 0; i < prpoly_p->paprvPolyVertices.uLen; i++)
		{
			SRenderVertex *prv_p2 = prpoly_p->paprvPolyVertices.atArray[i];

			float f_pdx = prv_p2->v3Screen.tX - prv_p1->v3Screen.tX;
			float f_pdy = prv_p2->v3Screen.tY - prv_p1->v3Screen.tY;

			int inside = 0;

			// For each point of Q.
			for (int j = 0; j < prpoly_q->paprvPolyVertices.uLen; j++)
			{
				SRenderVertex *prv_q = prpoly_q->paprvPolyVertices.atArray[j];

				float f_cross = f_pdx * (prv_q->v3Screen.tY - prv_p1->v3Screen.tY) -
								f_pdy * (prv_q->v3Screen.tX - prv_p1->v3Screen.tX);

				inside |= (f_cross < 0.0f);
			}

			// All points outside this edge?
			if (inside == 0)
			{
				// No intersection.
				return false;
			}

			prv_p1 = prv_p2;
		}
	}

	// Polygons must intersect.
	return(true);
}
