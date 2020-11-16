/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of ClipRegion2D.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ClipRegion2D.cpp                                         $
 * 
 * 6     9/23/98 9:05p Mmouni
 * Enabled femms instructions in esfSideOf.
 * 
 * 5     9/02/98 3:28p Mlange
 * Increased tolerance of some assert checks.
 * 
 * 4     8/27/98 1:50p Asouth
 * default relative operators are an MS extension not included by MSL by default
 * 
 * 3     2/20/98 7:55p Mlange
 * Defined CClipRegion2D::CClipEdge::esfSideOf() out of line.
 * 
 * 2     11/13/97 1:05p Mlange
 * Inlined some functions.
 * 
 * 1     11/13/97 11:00a Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "ClipRegion2D.hpp"

#include "Lib/Renderer/Camera.hpp"
#include "Lib/View/LineDraw.hpp"


//**********************************************************************************************
//
// Implementation of CClipRegion2D.
//

	//******************************************************************************************
	void CClipRegion2D::Init(const CCamera& cam)
	{
		// Obtain the camera extents in world space.
		CSArray<CVector3<>, 8> sav3_cam_extents;
		cam.WorldExtents(sav3_cam_extents);

		CVector3<> v3_cam_world_pos = cam.v3Pos();

		// Construct the world space definition of the far clipping plane.
		CPlaneDef pl_far(sav3_cam_extents[0], sav3_cam_extents[1], sav3_cam_extents[3]);

		// Construct the side (i.e. left, right, up and down) clipping planes.
		CSCArray<CPlaneDef, 4> scapl_sides;

		for (int i_pl = 0; i_pl < 4; i_pl++)
			scapl_sides[i_pl] = CPlaneDef(v3_cam_world_pos, sav3_cam_extents[i_pl], sav3_cam_extents[(i_pl - 1) & 3]);


		sacleEdges.Reset();

		// Back face cull the parallel projection of the camera's clipping planes. Then, each edge of the camera's
		// volume that is no longer shared between two planes defines one of the edges of the 2d clipping region.

		// Walk around the edges of the far clipping plane and add those that are no longer shared with one of the
		// side clipping planes to the 2d clipping region.
		for (int i = 0; i < 4; i++)
			if ((pl_far.d3Normal.tZ >= 0) != (scapl_sides[i].d3Normal.tZ >= 0))
				sacleEdges << CClipEdge(sav3_cam_extents[(i - 1) & 3], sav3_cam_extents[i]);

		Assert(sacleEdges.uLen != 0);

		// If we do not have a closed clipping region at this point, we close it by adding the edges formed with
		// the camera's origin.
		if (sacleEdges.uLen != 4)
		{
			for (int i_curr = 0; i_curr < sacleEdges.uLen; i_curr++)
			{
				int i_next = (i_curr + 1) % sacleEdges.uLen;

				if (!(sacleEdges[i_curr].v2B == sacleEdges[i_next].v2A))
				{
					sacleEdges << CClipEdge(sacleEdges[i_curr].v2B, v3_cam_world_pos);
					sacleEdges << CClipEdge(v3_cam_world_pos, sacleEdges[i_next].v2A);
					break;
				}
			}
		}

		// If the far clipping plane was back face culled in its projection, we must reverse the order of the
		// edges.
		if (pl_far.d3Normal.tZ < 0)
			for (int i = 0; i < sacleEdges.uLen; i++)
				Swap(sacleEdges[i].v2A, sacleEdges[i].v2B);


		#if VER_DEBUG
			// Make sure that for each edge the inside of the clipping region lies to its left.
			for (int i_curr = 0; i_curr < sacleEdges.uLen; i_curr++)
				for (int i_check = 0; i_check < sacleEdges.uLen; i_check++)
					Assert(Fuzzy(sacleEdges[i_curr].rSignedDist(sacleEdges[i_check].v2A), .02f) <= 0 &&
						   Fuzzy(sacleEdges[i_curr].rSignedDist(sacleEdges[i_check].v2B), .02f) <= 0   );
		#endif
	}


	//******************************************************************************************
	void CClipRegion2D::Draw(const CDraw& draw) const
	{
		for (int i = 0; i < sacleEdges.uLen; i++)
		{
			draw.MoveTo(sacleEdges[i].v2A.tX, sacleEdges[i].v2A.tY);
			draw.LineTo(sacleEdges[i].v2B.tX, sacleEdges[i].v2B.tY);
		}
	}


	//******************************************************************************************
	void CClipRegion2D::operator*= (const CPresence3<>& pr3)
	{
		for (int i = 0; i < sacleEdges.uLen; i++)
		{
			sacleEdges[i].v2A = CVector3<>(sacleEdges[i].v2A) * pr3;
			sacleEdges[i].v2B = CVector3<>(sacleEdges[i].v2B) * pr3;
		}
	}




//**********************************************************************************************
//
// Implementation of CClipRegion2D::CClipEdge.
//

	//******************************************************************************************
	ESideOf CClipRegion2D::CClipEdge::esfSideOf(const CVector2<>& v2_centre, TReal r_radius_sqr) const
	{

	#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		typedef CClipEdge tdCClipEdge;

		TReal r_d;
		TReal r_d_sqr;
		TReal r_temp;

		__asm
		{
			femms

			mov		ecx,[this]					;get ptr to v2A and v2B
			mov		eax,[v2_centre]				;get ptr to v2_centre
			jmp		StartAsm

			align 16
			nop									;establish 2 byte starting code offset
			nop

		StartAsm:
			movq	mm0,[ecx]tdCClipEdge.v2A	;m0= v2A.Y | v2A.X

			movq	mm1,[ecx]tdCClipEdge.v2B	;m1= v2B.Y | v2B.X

			movq	mm2,[eax]					;m2= v2_centre.Y | v2_centre.X

			pfsub	(m1,m0)						;m1=  v2_d.Y | v2_d.X
			test	eax,eax						;2-byte NOOP to avoid degraded predecode

			pfsub	(m2,m0)						;m2= v2_ca.Y | v2_ca.X

			movq	mm0,mm1						;m0=  v2_d.Y | v2_d.X
			psrlq	mm1,32						;m1=     0   | v2_d.Y

			movq	mm3,mm2						;m3= v2_ca.Y | v2_ca.X
			psrlq	mm2,32						;m2=     0   | v2_ca.Y

			pfmul	(m3,m1)						;m3= v2_ca.X*v2_d.Y

			pfmul	(m2,m0)						;m2= v2_ca.Y*v2_d.X

			pfmul	(m0,m0)						;m0= v2_d.Y*v2_d.Y | v2_d.X*v2_d.X

			pfsub	(m3,m2)						;m3= r_d
			movd	mm4,[r_radius_sqr]			;m4= r_radius_sqr

			pfacc	(m0,m0)						;m0= v2_d.tLenSqr()
			movd	[r_d],mm3

			pfmul	(m3,m3)						;m3= r_d*r_d
			movd	[r_d_sqr],mm3

			pfmul	(m0,m4)						;m0= r_radius_sqr*v2_d.tLenSqr()
			movd	[r_temp],mm0

			femms
		}

	#else // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		Assert(esfINSIDE == 1 && esfOUTSIDE == 2);

		CVector2<> v2_ca = v2_centre - v2A;
		CVector2<> v2_d = v2B - v2A;

		// Determine the squared distance from the closest point on the edge to the centre of the
		// circle.
		TReal r_d = v2_ca.tX * v2_d.tY - v2_ca.tY * v2_d.tX;

		TReal r_d_sqr = Sqr(r_d);
		TReal r_temp = r_radius_sqr * v2_d.tLenSqr();

	#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		if (CIntFloat(r_d_sqr).i4Int <= CIntFloat(r_temp).i4Int)
			return esfINTERSECT;
		else
			return !CIntFloat(r_d).bSign() + 1;
	}


