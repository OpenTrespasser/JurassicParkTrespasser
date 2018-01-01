/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Line2D.hpp                                               $
 * 
 * 9     9/24/98 6:07p Asouth
 * changed inline asm ; comments to //
 * 
 * 8     11/11/97 4:37p Gfavor
 * Eliminated degraded predecode from 3DX code.
 * 
 * 7     11/10/97 10:09p Gfavor
 * Converted bDoesIntersect to use 3DX.
 * 
 * 6     97/05/22 17:25 Speter
 * Added Asserts to ensure positive values in CLine2D.
 * 
 * 5     97/05/16 10:30a Pkeet
 * Fixed slight bug in the optimizations.
 * 
 * 4     97/05/15 4:22p Pkeet
 * Added a few optimizations.
 * 
 * 3     97/04/25 6:40p Pkeet
 * Changed some compares to use the '(u4FromFloat' macro.
 * 
 * 2     97/04/25 2:22p Pkeet
 * Implemented a new version of CLine2D based on an article in Graphics Gems III.
 * 
 * 1     97/04/23 10:23a Pkeet
 * Initial implementation. Code moved here from 'Line.cpp.'
 * 
 **********************************************************************************************/

#ifndef LIB_RENDERER_LINE2D_HPP
#define LIB_RENDERER_LINE2D_HPP

//
// Necessary includes.
//
#include "AsmSupport.hpp"


//
// Global function prototypes.
//

//**********************************************************************************************
//
inline bool bIntersectTrivial
(
	const CVector2<>& p1,
	const CVector2<>& p2,
	const CVector2<>& p3,
	const CVector2<>& p4
)
//
// Returns 'true' if the bounding rectangles of the line segments intersect.
//
//**********************************
{
	if (Min(p1.tX, p2.tX) > Max(p3.tX, p4.tX))
		return false;
	if (Max(p1.tX, p2.tX) < Min(p3.tX, p4.tX))
		return false;
	if (Min(p1.tY, p2.tY) > Max(p3.tY, p4.tY))
		return false;
	if (Max(p1.tY, p2.tY) < Min(p3.tY, p4.tY))
		return false;
	return true;
}


//
// Class definitions.
//

//*********************************************************************************************
//
class CLine2D
//
// A 2D line.
//
// Prefix: line
//
//****************
{
public:

	//*****************************************************************************************
	//
	// Object variables.
	//

	CVector2<> v2From;			// Starting coordinate of the line (p1).
	CVector2<> v2To;			// End coordinate of the line (p2).
	CVector2<> v2A;				// Parametric delta.
	CVector2<> v2Min;			// Minimum coordinates of the bounding rectangle.
	CVector2<> v2Max;			// Minimum coordinates of the bounding rectangle.
	float      fDenominator;	// Intersection denominator.
	float      fNumeratorAlpha;	// Numerator for alpha (this line segment).
	float      fNumeratorBeta;	// Numerator for beta (other line segment).

public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	//*****************************************************************************************
	//
	CLine2D
	(
	)
	//
	// Default constructor.
	//
	//****************
	{
	}

	//*****************************************************************************************
	//
	CLine2D
	(
		float f_x0,	// Coordinates of starting point.
		float f_y0,
		float f_x1,	// Coordinates of ending point.
		float f_y1
	)
	//
	// Constructor.
	//
	//****************
	{
		Initialize(f_x0, f_y0, f_x1, f_y1);
	}

	//*****************************************************************************************
	//
	CLine2D
	(
		const CVector3<>& v3_0,	// Coordinates of starting point.
		const CVector3<>& v3_1	// Coordinates of ending point.
	)
	//
	// Constructor.
	//
	//****************
	{
		Initialize(v3_0.tX, v3_0.tY, v3_1.tX, v3_1.tY);
	}


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Initialize
	(
		float f_x0,	// Coordinates of starting point.
		float f_y0,
		float f_x1,	// Coordinates of ending point.
		float f_y1
	)
	//
	// Constructor.
	//
	//****************
	{
		Assert(f_x0 >= 0 && f_y0 >= 0);
		Assert(f_x1 >= 0 && f_y1 >= 0);

		// Set the endpoints.
		v2From = CVector2<>(f_x0, f_y0);
		v2To   = CVector2<>(f_x1, f_y1);

		// Set the parametric delta.
		v2A    = v2To - v2From;

		// Set the bounding rectangle.
		if (u4FromFloat(v2From.tX) > u4FromFloat(v2To.tX))
		{
			v2Min.tX = v2To.tX;
			v2Max.tX = v2From.tX;
		}
		else
		{
			v2Min.tX = v2From.tX;
			v2Max.tX = v2To.tX;
		}
		if (u4FromFloat(v2From.tY) > u4FromFloat(v2To.tY))
		{
			v2Min.tY = v2To.tY;
			v2Max.tY = v2From.tY;
		}
		else
		{
			v2Min.tY = v2From.tY;
			v2Max.tY = v2To.tY;
		}
	}

	//*****************************************************************************************
	//
	bool bLeft()
	//
	// Returns 'true' if the line segment belongs to the left side of the polygon.
	//
	//****************
	{
		return v2From.tY < v2To.tY;
	}

	//*****************************************************************************************
	//
	bool bHorizontal()
	//
	// Returns 'true' if the line segment is horizontal.
	//
	//****************
	{
		return v2From.tY == v2To.tY;
	}

	//*****************************************************************************************
	//
	float fMinY
	(
	) const
	//
	// Returns the lowest value for Y.
	//
	//****************
	{
		return v2Min.tY;
	}

	//*****************************************************************************************
	//
	float fMaxY
	(
	) const
	//
	// Returns the hightest value for Y.
	//
	//****************
	{
		return v2Max.tY;
	}

	//*****************************************************************************************
	//
	bool bBoundingRect
	(
		const CVector2<>& p3,
		const CVector2<>& p4
	)
	//
	// Returns 'true' if the bounding rectangles of the line segments intersect.
	//
	//****************
	{
		Assert(p3.tX >= 0 && p3.tY >= 0);
		Assert(p4.tX >= 0 && p4.tY >= 0);

		uint32 u4_max = u4FromFloat(p3.tX);
		uint32 u4_min = u4FromFloat(p4.tX);

		if (u4_min > u4_max)
			Swap(u4_min, u4_max);

		if (u4FromFloat(v2Min.tX) > u4_max)
			return false;
		if (u4FromFloat(v2Max.tX) < u4_min)
			return false;

		u4_max = u4FromFloat(p3.tY);
		u4_min = u4FromFloat(p4.tY);

		if (u4_min > u4_max)
			Swap(u4_min, u4_max);

		if (u4FromFloat(v2Min.tY) > u4_max)
			return false;
		if (u4FromFloat(v2Max.tY) < u4_min)
			return false;

		return true;
	}

	//*****************************************************************************************
	//
	bool bDoesIntersect
	(
		const CLine2D& line
	)
	//
	// Returns a line representing the remaining portion of the original line after an
	// intersection. If there is no intersection, the original line is returned.
	//
	//****************
	{
		return bDoesIntersect(line.v2From, line.v2To);
	}

	//*****************************************************************************************
	//
	bool bDoesIntersect
	(
		CVector2<> p3,
		CVector2<> p4
	)
	//
	// Returns 'true' if this line segment and the line segment described by p3 and p4
	// intersect.
	//
	// Notes:
	//		This code is based on the algorithm described in the article "Faster Line Segment
	//		Intersection" from page 199 to 202 in Graphics Gems III. The algorithm works by
	//		simplifying the parametric representations of the line segments to determine the
	//		point of intersection.
	//
	//****************
	{
		// Attempt trivial rejection.
		if (!bBoundingRect(p3, p4))
			return false;

		CVector2<> v2_b = p3 - p4;
		CVector2<> v2_c = v2From - p3;

		float fDenominator = v2A.tY * v2_b.tX - v2A.tX * v2_b.tY;

		// If the fDenominatorinator is zero, the lines are colinear and 
		// therefore do not intersect.
		if (fDenominator == 0.0f)
			return false;

		//
		// Get alpha's numerator and check if its in bounds.
		//
		fNumeratorAlpha = v2_b.tY * v2_c.tX - v2_b.tX * v2_c.tY;

		if (u4FromFloat(fDenominator) & 0x80000000L)
		{
			if (fNumeratorAlpha > 0.0f)
				return false;
			if (fNumeratorAlpha < fDenominator)
				return false;
		}
		else
		{
			if (u4FromFloat(fNumeratorAlpha) & 0x80000000L)
				return false;
			if (u4FromFloat(fNumeratorAlpha) > u4FromFloat(fDenominator))
				return false;
		}

		//
		// Get beta's numerator and check if its in bounds.
		//
		fNumeratorBeta = v2A.tX * v2_c.tY - v2A.tY * v2_c.tX;

		// Test if the denominator is negative.
		if (u4FromFloat(fDenominator) & 0x80000000L)
		{
			if (fNumeratorBeta > 0.0f)
			//if (!(u4FromFloat(fNumeratorBeta) & 0x80000000L))
				return false;
			if (fNumeratorBeta < fDenominator)
				return false;
		}
		else
		{
			if (u4FromFloat(fNumeratorBeta) & 0x80000000L)
				return false;
			if (u4FromFloat(fNumeratorBeta) > u4FromFloat(fDenominator))
				return false;
		}

		// There must be intersection.
		return true;
	}

	//*****************************************************************************************
	//
	bool bDoesIntersectFast
	(
		CVector2<> p3,
		CVector2<> p4
	)
	//
	// Returns 'true' if this line segment and the line segment described by p3 and p4
	// intersect.
	//
	// Notes:
	//		This code is based on the algorithm described in the article "Faster Line Segment
	//		Intersection" from page 199 to 202 in Graphics Gems III. The algorithm works by
	//		simplifying the parametric representations of the line segments to determine the
	//		point of intersection.
	//
	//****************
	{
	#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		Assert((char *)&v2A.tX - (char *)&v2A == 0);
		Assert((char *)&v2A.tY - (char *)&v2A == 4);

		typedef CLine2D tdCLine2D;

		struct SVec2
		{
			float fX;
			float fY;
		};

		uint u_one = 1;
		bool b_cmp_results;

		__asm
		{
			jmp		StartAsm

			align	16
			nop									// establish 8 byte starting code offset
			nop
			nop
			nop
			nop
			nop
			nop
			nop

		StartAsm:		
			// femms							;caller responsible for this

			mov		ecx,[this]					// ecx= This ptr

			movq	mm0,[p3]					// m0= p3.Y | p3.X

			movq	mm2,[p4]					// m2= p4.Y | p4.X

			movq	mm4,[ecx]tdCLine2D.v2Min	// m4= v2Min.Y | v2Min.X
			movq	mm6,mm0						// m6= p3.Y | p3.X

			movq	mm5,[ecx]tdCLine2D.v2Max	// m5= v2Max.Y | v2Max.X
			nop									// 1-byte NOOP to avoid degraded predecode
			movq	mm7,mm2						// m7= p4.Y | p4.X

			pfmin	(m6,m2)						// m6= u4_min.Y | u4_min.X
			movq	mm1,[ecx]tdCLine2D.v2From	// m1= v2From.Y | v2From.X

			pfmax	(m7,m0)						// m7= u4_max.Y | u4_max.X
			pfsubr	(m2,m0)						// m2= v2_b.Y | v2_b.X

			pfcmpgt	(m6,m5)						// m6= check if u4_min > v2Max
			pfsubr	(m0,m1)						// m0= v2_c.Y | v2_c.X

			pfcmpgt	(m4,m7)						// m4= check if v2Min > u4_max
			movq	mm3,mm2						// m3= v2_b.X

			punpckhdq mm2,mm2					// m2= v2_b.Y
			movd	mm5,[ecx+0]tdCLine2D.v2A	// m5= v2A.X

			por		mm6,mm4						// m6= combine compare results
			movd	mm7,[ecx+4]tdCLine2D.v2A	// m7= v2A.Y

			movq	mm4,mm6						// m4= compare results - low
			punpckhdq mm6,mm6					// m6= compare results - high

			por		mm6,mm4						// m6= all compare results combined
			movd	eax,mm6						// eax!=0 if no possible X or Y overlap

			pfmul	(m5,m2)						// m5= v2A.X*v2_b.Y
			movq	mm1,mm0						// m1= v2_c.X

			pfmul	(m7,m3)						// m7= v2A.Y*v2_b.X
			punpckhdq mm0,mm0					// m0= v2_c.Y

			test	eax,eax
			jnz		SHORT ReturnFalse			// go return false if no possible overlap

			pfsub	(m7,m5)						// m7= fDenominator
			pfmul	(m2,m1)						// m2= v2_b.Y*v2_c.X

			movd	eax,mm7						// eax= fDenominator
			test	ebx,ebx						// 2-byte NOOP to avoid degraded predecode
			pfmul	(m3,m0)						// m3= v2_b.X*v2_c.Y

			movd	mm4,[ecx+0]tdCLine2D.v2A	// m4= v2A.X

			movd	mm5,[ecx+4]tdCLine2D.v2A	// m5= v2A.Y
			pfsub	(m2,m3)						// m2= fNumeratorAlpha

			test	eax,0x7FFFFFFF				// check if fDenominator is zero
			jz		SHORT ReturnFalse			// if so, go return false

			pfmul	(m0,m4)						// m0= v2A.X*v2_c.Y
			test	eax,eax

			pfmul	(m1,m5)						// m1= v2A.Y*v2_c.X
			movq	mm6,mm7						// m6= fDenominator

			pxor	mm4,mm4						// m4= 0.0
			pxor	mm5,mm5						// m5= 0.0

			pfsub	(m0,m1)						// m0= fNumeratorBeta
			jns		SHORT CheckPosDen			// go do checks based on fDenominator > 0

			align	16							// establish 0 byte starting code offset
		// CheckNegDen:
			pfcmpgt	(m6,m2)						// check if fDenominator > fNumeratorAlpha

			pfcmpgt	(m2,m4)						// check if fNumeratorAlpha > 0.0

			pfcmpgt	(m7,m0)						// check if fDenominator > fNumeratorBeta

			pfcmpgt	(m0,m4)						// check if fNumeratorBeta > 0.0
			por		mm2,mm6						// combine first two compare results

			movd	mm1,[u_one]					// m1= 0x1
			por		mm2,mm7						// combine in third compare result

			por		mm0,mm2						// m0= all compare results combined
												//  = all 1's if should return false
			jmp		SHORT ReturnResults

			align	16							// establish 0 byte starting code offset
		CheckPosDen:
			pfcmpgt	(m4,m2)						// check if 0.0 > fNumeratorAlpha

			pfcmpgt	(m2,m7)						// check if fNumeratorAlpha > fDenominator

			pfcmpgt	(m5,m0)						// check if 0.0 > fNumeratorBeta

			pfcmpgt	(m0,m7)						// check if fNumeratorBeta > fDenominator
			por		mm2,mm4						// combine first two compare results

			movd	mm1,[u_one]					// m1= 0x1
			por		mm2,mm5						// combine in third compare result

			por		mm0,mm2						// m0= all compare results combined
												//  = all 1's if should return false
		ReturnResults:
			pandn	mm0,mm1						// m0= 0x1 if should return true, else = 0x0
			movd	[b_cmp_results],mm0

			// femms							// caller responsible for this
		}
		return b_cmp_results;
	
	ReturnFalse:
		return false;

	#else (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		// Attempt trivial rejection.
		if (!bBoundingRect(p3, p4))
			return false;

		CVector2<> v2_b = p3 - p4;
		CVector2<> v2_c = v2From - p3;

		float fDenominator = v2A.tY * v2_b.tX - v2A.tX * v2_b.tY;

		// If the fDenominatorinator is zero, the lines are colinear and 
		// therefore do not intersect.
		if (fDenominator == 0.0f)
			return false;

		//
		// Get alpha's numerator and check if its in bounds.
		//
		fNumeratorAlpha = v2_b.tY * v2_c.tX - v2_b.tX * v2_c.tY;

		if (u4FromFloat(fDenominator) & 0x80000000L)
		{
			if (fNumeratorAlpha > 0.0f)
				return false;
			if (fNumeratorAlpha < fDenominator)
				return false;
		}
		else
		{
			if (u4FromFloat(fNumeratorAlpha) & 0x80000000L)
				return false;
			if (u4FromFloat(fNumeratorAlpha) > u4FromFloat(fDenominator))
				return false;
		}

		//
		// Get beta's numerator and check if its in bounds.
		//
		fNumeratorBeta = v2A.tX * v2_c.tY - v2A.tY * v2_c.tX;

		// Test if the denominator is negative.
		if (u4FromFloat(fDenominator) & 0x80000000L)
		{
			if (fNumeratorBeta > 0.0f)
			//if (!(u4FromFloat(fNumeratorBeta) & 0x80000000L))
				return false;
			if (fNumeratorBeta < fDenominator)
				return false;
		}
		else
		{
			if (u4FromFloat(fNumeratorBeta) & 0x80000000L)
				return false;
			if (u4FromFloat(fNumeratorBeta) > u4FromFloat(fDenominator))
				return false;
		}

		// There must be intersection.
		return true;

	#endif (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
	}

	//*****************************************************************************************
	//
	CVector2<> v2GetIntersection
	(
	)
	//
	// Returns the intersection point given the last intersection test.
	//
	//****************
	{
		Assert(fDenominator != 0.0f);
		return v2A * fNumeratorAlpha / fDenominator + v2From;
	}

	//*****************************************************************************************
	//
	float fGetIntercept
	(
		float f_y
	)
	//
	// Returns the x coordinate of the point of intersection with a horizontal line y.
	//
	//****************
	{
		CVector2<> p3( -10.0f, f_y);
		CVector2<> p4(2038.0f, f_y);

		CVector2<> v2_b = p3 - p4;
		CVector2<> v2_c = v2From - p3;

		float fDenominator = v2A.tY * v2_b.tX - v2A.tX * v2_b.tY;

		//
		// If the fDenominatorinator is zero, the lines are colinear and therefore do not
		// intersect.
		//
		Assert(fDenominator != 0.0f);

		//
		// Get alpha's numerator and check if its in bounds.
		//
		fNumeratorAlpha = v2_b.tY * v2_c.tX - v2_b.tX * v2_c.tY;
		return (v2A * fNumeratorAlpha / fDenominator + v2From).tX;
	}
};


#endif // LIB_RENDERER_LINE2D_HPP
