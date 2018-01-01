/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Geometry testing code.  Not style-compliant.  Leave me alone.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform5/GeomTest.cpp                                           $
 * 
 * 23    97-04-24 18:40 Speter
 * Folded new changes from 4.2 version into this 5.0 specific version.
 * 
 * 22    97-03-31 22:26 Speter
 * Updated for changes in transforms.
 * 
 * 21    96/12/09 16:07 Speter
 * Removed CTimer calls.
 * 
 * 20    96/11/11 14:44 Speter
 * 
 * 19    96/10/28 15:00 Speter
 * Changed CEventHandler to CAppShell.
 * 
 * 18    96/09/27 11:31 Speter
 * Oops.  Changed <float> in geometry types to <>.
 * 
 * 17    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 16    96/09/23 17:09 Speter
 * Added some test code for transformations to texture surfaces.
 * 
 * 15    96/09/16 12:26 Speter
 * Added test for fixed point Fuzzy.
 * 
 * 14    96/09/09 18:33 Speter
 * Added more test code.
 * 
 * 13    96/08/21 18:58 Speter
 * Updated for new transform functions.
 * 
 * 12    96/08/01 13:44 Speter
 * Changes to test quat renorm.
 * 
 * 11    96/07/31 15:53 Speter
 * Added timing stats.
 * 
 * 10    7/19/96 10:58a Pkeet
 * Changed include files for shell files moved to 'Shell.'
 * 
 * 9     96/07/08 12:40 Speter
 * Changed name of CNormal3 to CDir3 (more general).
 * Added specific functions to transform directions, which do not translate positions.
 * 
 * 8     96/07/03 13:02 Speter
 * Moved several files to new directories, and changed corresponding include statements.
 * 
 * 7     96/06/27 11:05 Speter
 * Fixed quaternion multiplication.  It was backwards.
 * 
 * 6     96/06/26 22:07 Speter
 * Added a bunch of comments, prettied things up nicely.
 * 
 * 5     96/06/26 15:05 Speter
 * Changed CPosition3 to CPlacement3.
 * 
 * 4     96/06/26 13:16 Speter
 * Changed TGReal to TReal and prefix gr to r.
 * 
 * 3     96/06/25 14:35 Speter
 * Finalised design of transform classes, with Position3 and Transform3.
 * 
 * 2     96/06/20 17:12 Speter
 * Converted into templates and made necessary changes.
 * 
 * 1     96/06/20 15:26 Speter
 * First version of new optimised transform library.
 * 
 **********************************************************************************************/

/*								Renorm #		Denorm	Time
								Avg		Max		Max		Debug		Release
	Never renorm													1.6 us
	Always check, renorm		14K				.0001	10.3 us		1.7 us
	Always renorm													2.3 us
	Random (100), renorm		5K		300K	.01		8.4 us		1.9 us
	Check count, renorm			1.4K			.00002	8.1 us		1.5 us
*/

#include "Common.hpp"

#include "Transform.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Shell/AppShell.hpp"

#include <memory.h>

#define TIMIT	0
#define RENORM	0
#define iFACTOR	100

#if RENORM

class CAppShellGeom: public CAppShell
{
public:
	TMSec	msCycle;
	int		iCount;

	CRotate3<> r3a, r3b, r3c;

	CAppShellGeom()
		: r3a(), r3b(), r3c('xyz', 0.01, 0.02, 0.03)
	{
	}

	void Init()
	{
		iCount = msCycle = 0;
		SetContinuous(1);
	}

	void Step()
	{
		CTimer tmr;

		for (int i = 0; i < iFACTOR; i++)
		{
			r3b *= r3c;
			r3a *= r3b;
		}

		msCycle += tmr.msElapsed();
		iCount += iFACTOR;

		conStd.SetCursorPosition(0, 0);
		conStd.Print("Time = %f, Count = %d, Debug = %d   \n", 
			(float) msCycle / iCount, iCount, VER_DEBUG);
#if VER_DEBUG
		conStd.Print("Avg norm count = %d, Max denorm = %f, count = %d   \n", 
			CRotate3<>::iAvgNormalisationCount,
			CRotate3<>::tMaxDenormalisation, 
			CRotate3<>::iMaxDenormalisationCount);
#endif
		conStd.Show();
	}
};

#else

/*
	Plans:
		Current:
			Basic transform types.  They all combine.
			Composite types: template, basic + vector.
			Combining all is hairy.

		Union:
			Basic transform types.  They all combine.
			Transform = union of basics, + vector.
			Combining anything with a translation results in a Transform.
			Used only for intermediate calculations.  Base transforms can be stored optimally.
					
		Just matrix:
			Basic transform types.  They all combine.
			Transform = matrix, + vector.
			Combining anything with a translation results in a Transform.
		
Calculations needed:				MT						?T						*T
	Camera:
		Norm = Scale * Shear		Xm = S * Hm				Xm = S * Hm				M = S * Hm
		Camera = ~(RT) * Norm		XmT = MT(~RT) * Xm		XmT = MT(~RT) * Xm		MT = MT(~RT) * M

	Objects:
		RT							
			or SRT
		Camera.T * ~RT				XmT = XmT * MT(~RT)		XmT = XmT * MT(~RT)		MT * MT(~RT)
				or ~SRT				XmT = XmT * MT(~SRT)	XmT = XmT * MT(~SRT)	MT * MT(~SRT)

	Subobjects (skeletons):
		RT = RT*RT					RT = RT * RT			XrT = RT * RT			RT = RT * RT
															or XrT = XrT * RT

	Poly:
		Poly = Object * Camera		XmT = XmT(RT) * XmT		XmT = MT(RT) * XmT		MT = MT(RT) * MT
									XmT = XmT(SRT) * XmT	XmT = XmT(SRT) * XmT	MT = MT(SRT) * MT

	Vertex:
		V * Poly					V * XmT					V * XmT					V * MT
*/

CVector3<>		v3_a(6, -5, 1.5), v3_b(1, 0, 0.4);
CDir3<>			d3_a = v3_a, d3_b = v3_b;

CMatrix3<>		mx3(1, 0, 3,  0, 3, 2,  7, 4, 0);
CRotate3<>		r3(d3_a, d3_b);
CScale3<>		s3(2, 0.5, -1);
CScaleI3<>		si3(4);
CTranslate3<>	tl3(-4, 0, 2);

CPlacement3<>	p3(r3, v3_a);
CTransform3<>	tf3(mx3, v3_b);

template<class T> inline bool operator ==(const T& t1, const T& t2)
{
	return memcmp(&t1, &t2, sizeof(T)) == 0;
}

inline bool operator ==(const CVector3<>& v3_a, const CVector3<>& v3_b)
{
	return bFurryEquals(v3_a.tX, v3_b.tX) &&
		   bFurryEquals(v3_a.tY, v3_b.tY) &&
		   bFurryEquals(v3_a.tZ, v3_b.tZ);
}

template<class T1, class T2> void GeomTest2(const T1& t1, const T2& t2)
{
	CVector3<>	v3(1, 10, 100);
	CVector3<>	v3_2, v3_3;

	if (~(t1*t2) == ~t2*~t1)
		;
	else
	{
		v3_2 = v3 * ~(t1*t2);
		v3_3 = v3 * ~t2*~t1;
		Assert(v3_2 == v3_3);
	}

	v3_2 = v3 * t1 * t2;
	v3 *= t1;
	v3 *= t2;

	Assert(v3_2 == v3);
}

template<class T3> void GeomTest(const T3& t3)
{
	CVector3<>	v3(1, 10, 100);

	T3	t3_identity;
	T3	t3_inv = ~t3;
	
	t3_identity = t3_inv * t3;
	CVector3<>	v3_check = v3 * t3_identity;
	Assert(v3 == v3_check);

	CVector3<> v3_2 = v3 * t3;
	v3 *= t3;
	Assert(v3 == v3_2);

	t3_inv = t3 * t3;
	t3_identity = t3;
	t3_identity *= t3;
	Assert(t3_inv == t3_identity);

	GeomTest2(t3, CMatrix3<>(2, -4, 1,  2, 0, 5,  0, 1, -1));
	GeomTest2(t3, CRotate3<>
	(
		SFrame3<>(CDir3<>(1, 0, 3), d3YAxis),
		SFrame3<>(d3XAxis, CDir3<>(0, 6, -1))
	));

	GeomTest2(t3, CScale3<>(1, 1.04, 0.02));
	GeomTest2(t3, CScaleI3<>(-2.5));
	GeomTest2(t3, CShear3<>(d3ZAxis, 0.1, -0.2));

	GeomTest2(t3, CTranslate3<>(4, 2, -0.4));
	GeomTest2(t3, CPlacement3<>
	(
		CRotate3<>(CDir3<>(1, 0.5, 0.3), CAngle(2.6)),
		CVector3<>(5, 6, 7)
	));
	GeomTest2(t3, CTransform3<>(CMatrix3<>(2, 1, 0, 9, 0, 3, 7, 1, 0), CVector3<>(0.1, 0, -6)));
}

template<class T3> void GeomTestDir(const T3& t3)
{
	CVector3<>	v3(1, 10, 100);
	CDir3<>		d3 = v3;

	CDir3<> d3_2 = d3 * t3;
	d3 *= t3;
	Assert(d3 == d3_2);
}

template<class T3> void GeomTestPlusDir(const T3& t3)
{
	GeomTest(t3);
	GeomTestDir(t3);
}

class CAppShellGeom: public CAppShell
{
public:
	void Init()
	{
#if TIMIT
		SetContinuous(1);
	}

	void Step()
	{
		CTimer tmr;
#endif
		for (int i = 0; i < iFACTOR; i++)
		{
			/*
				Find the transformation which maps a 3-D triangle onto a 2-D texture.
				!! This is not determinate !!

				Given:
					Polygon vertices v0, v1, v2;
					Texture vertices t0, t1, t2; each with Z value arbitrary

				Find:
					Matrix M, such that

						v * M = t

						v:0:x,y,z,1 * M:x:x,y,0,0 = t:0:x,y,0,1
						  1:x,y,z,1     y:x,y,0,0     1:x,y,0,1
						  2:x,y,z,1     z:x,y,0,0     2:x,y,0,1
						  ?:?,?,?,?     t:x,y,0,1     ?:?,?,?,?


						v:0:x,y,z,1 * M:x:x,y = t:0:x,y
						  1:x,y,z,1     y:x,y	  1:x,y
						  2:x,y,z,1     z:x,y	  2:x,y
										t:x,y

						3.4 * 4.2 = 3.2
				Then:
					
					M = ~v * t
					
						4.3 * 3.2 = 4.2

				3x4:
					~(M+T) = (~M - T*~M)

			*/

/*
			CVector3<>	poly[3] = {CVector3<>(2, 4, 7), CVector3<>(0, 0, 0), CVector3<>(9, 3, 2)};
			CVector3<>	tex[3]	= {CVector3<>(4, 6, 0), CVector3<>(7, 3, 0), CVector3<>(1, 5, 0)};

			CTransform3<>	xform =
				~CTransform3<>(CMatrix3<>(poly[0], poly[1], poly[2])) *
				 CTransform3<>(CMatrix3<>(tex[0], tex[1], tex[2]));

			CVector3<>	tex2[3] = {poly[0] * xform, poly[1] * xform, poly[2] * xform};

			Assert(tex2[0] == tex[0]);
			Assert(tex2[1] == tex[1]);
			Assert(tex2[2] == tex[2]);
*/

			fixed a = 3.00, b = 3.01;

			if (Fuzzy(a) != b)
				a = b;
				
			GeomTestPlusDir(CMatrix3<>(1, 0, 3,  0, 3, 2,  7, 4, 0));
			GeomTestPlusDir(CRotate3<>(d3XAxis, 0.2) * CRotate3<>(d3YAxis, 0.4) * CRotate3<>(d3ZAxis, 0.5));
			GeomTest(CScale3<>(3, 0.5, -1));
			GeomTest(CScaleI3<>(4));
			GeomTest(CShear3<>('y', 0, -0.4));

			GeomTest(CTranslate3<>(6, -5, 1.5));
			GeomTest(CPlacement3<>(CRotate3<>(d3YAxis, -2.22), CVector3<>(0.5, 3.2, -6.7)));
			GeomTest(CTransform3<>(CMatrix3<>(0, 9, 1, 8, 5, 0, 2, 0, 6), CVector3<>(-9.1, 6, 0)));

			Assert(CRotate3<>(CDir3<>(1, 0, 0), CAngle(1)) == CRotate3<>(d3XAxis, 1));
			Assert(CRotate3<>(CDir3<>(-1, 4, 3), 2) == 
				CRotate3<>(d3XAxis, 1) * CRotate3<>(d3ZAxis, 2) * CRotate3<>(d3YAxis, 3));

			Assert(CVector3<>(d3XAxis * r3) == CVector3<>(1, 0, 0) * r3);
			Assert(CVector3<>(d3YAxis * r3) == CVector3<>(0, 1, 0) * r3);
			Assert(CVector3<>(d3ZAxis * r3) == CVector3<>(0, 0, 1) * r3);

			tf3 = p3;
			tf3 = mx3 = r3;
			tf3 = mx3;
			tf3 = mx3 = s3;
			tf3 = mx3 = si3;
//			tf3 = tl3.v3T;

			p3 = r3;

			mx3 = r3;
			mx3 = s3;
			mx3 = si3;

			s3 = si3;
		}

#if TIMIT
		msStep += tmr.msElapsed();
		StatDB.iCount++;
		conStd.SetCursorPosition(0, 0);
		conStd.Print("Time = %f   ", (float) msStep / StatDB.iCount, iFACTOR);
		conStd.Show();
#else
		TerminateShell();
#endif
	}

	void NewRaster()
	{
	}

	void Resize()
	{
	}

	void Paused(bool b_paused)
	{
	}

	void Paint()
	{
	}
};

#endif

CAppShell* pappMain = new CAppShellGeom;

