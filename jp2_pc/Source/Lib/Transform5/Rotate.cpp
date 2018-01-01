/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Rotate.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform5/Rotate.cpp                                             $
 * 
 * 21    97-04-24 18:40 Speter
 * Folded new changes from 4.2 version into this 5.0 specific version.
 * 
 * 23    4/16/97 2:24p Agrant
 * Restored SFrame to allow CRotate --> CMatrix conversion
 * 
 * 22    97/03/24 14:54 Speter
 * Removed constructors of CDirs and CRotates from ASCII chars; use d3ZAxis etc. constants.
 * When we need optimisation for axes, we'll use derived classes which don't need a switch
 * statement.
 * 
 * 21    97/02/07 19:16 Speter
 * Removed SFrame3, associated CRotate3 functions, as they were inefficient and were replaced
 * with equivalent CMatrix functions.  Removed CMatrix3 to CRotate3 conversion, as it used
 * SFrame3 and wasn't used.
 * 
 * 20    97/01/26 20:00 Speter
 * Added operator to interpolate rotation by a scalar.
 * Sped up rotate vector, rotation to matrix, and rotation from vector operations.
 * Aligned code.
 * 
 * 19    12/20/96 3:52p Mlange
 * Moved the CRotate3 constructor that takes a character axis and angle to the class declaration
 * in the header file.
 * 
 * 18    12/20/96 3:45p Mlange
 * Took out unused declaration of static data member.
 * 
 * 17    96/11/20 11:52 Speter
 * Added constructor for CRotate3 from CMatrix3.
 * 
 * 16    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 15    96/09/12 16:27 Speter
 * Replaced bFuzzyEquals with Fuzzy ==.
 * 
 * 14    96/09/10 18:24 Speter
 * Added corrections to constructors which rotate one direction or frame to another.  Now handle
 * cases of vector reversal.
 * 
 * 13    96/09/09 18:33 Speter
 * Added CRotate3 constructor which rotates one frame to another.  Added Assert for proper
 * behavior of constructor rotating one direction to another.
 * 
 * 12    96/08/21 18:42 Speter
 * Changes from code review:
 * Added default template parameter to all classes.
 * Made all in-class friends inline.
 * Added CRotate3() constructor taking multi-byte character.
 * Added casts where necessary.
 * 
 * 11    96/08/06 18:23 Speter
 * Reversed order of d3_to and d3_from in constructor.
 * Added CPlacement3 constructor which takes a CTranslate3.
 * 
 * 10    96/08/01 11:04 Speter
 * Added automatic renormalisation.
 * 
 * 9     96/07/12 17:35 Speter
 * Added Rotate3 constructor taking a rotation vector.
 * 
 * 8     96/07/08 12:40 Speter
 * Changed name of CNormal3 to CDir3 (more general).
 * Added specific functions to transform directions, which do not translate positions.
 * 
 * 7     96/06/27 11:05 Speter
 * Fixed quaternion multiplication.  It was backwards.
 * 
 * 6     96/06/26 22:07 Speter
 * Added a bunch of comments, prettied things up nicely.
 * 
 * 5     96/06/26 17:03 Speter
 * Really fixed rotation.
 * 
 * 4     96/06/26 16:43 Speter
 * Fixed matrix and vector operations.  They were transposed!
 * 
 * 3     96/06/26 13:16 Speter
 * Changed TGReal to TR and prefix gr to r.
 * 
 * 2     96/06/20 17:13 Speter
 * Converted into templates and made necessary changes.
 * 
 * 1     96/06/20 15:26 Speter
 * First version of new optimised transform library.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Rotate.hpp"

//******************************************************************************************
//
// CRotate3<TR> implementation.
//

#if VER_QUATERNION_COUNT
	template<class TR> int CRotate3<TR>::iMaxNormalisationCount = 0;
	template<class TR> int CRotate3<TR>::iMaxCheckCount = 0;
	template<class TR> int CRotate3<TR>::iMaxDenormalisationCount = 0;
	template<class TR> int CRotate3<TR>::iAvgNormalisationCount = 0;

	template<class TR> float CRotate3<TR>::fTotalNormalisationCount = 0.0f;
	template<class TR> float CRotate3<TR>::fTotalNormalisations = 0.0f;

	template<class TR> TR CRotate3<TR>::tMaxDenormalisation = (TR)0;
#endif

	//******************************************************************************************
	template<class TR> CRotate3<TR>::CRotate3(const CDir3<TR>& d3, CAngle ang)
	{
		//
		// A quaternion consists of a scalar part, which is just the cosine of half the angle of
		// rotation, and a vector part, which is the axis of rotation times the sine of half the
		// angle.
		//

		ang >>= 1;
		tC = ang.fCos();
		v3S = (CVector3<TR>)d3 * ang.fSin();
		InitCounter();
	}

	//******************************************************************************************
	template<class TR> CRotate3<TR>::CRotate3(const CVector3<TR>& v3)
	{
		TR t_len = v3.tLen();

		// Normalise the vector ourselves, since we already have the length.
		new(this) CRotate3<TR>(CDir3<TR>(v3 / t_len, true), CAngle(t_len));
	}

	//******************************************************************************************
	template<class TR> CRotate3<TR>::CRotate3(const CDir3<TR>& d3_from, const CDir3<TR>& d3_to)
	{
		//
		// Well, let's see now.  The quaternion is stored as (cos(a/2), v*sin(a/2).
		//
		// So if we set the scalar part of the quaternion to the dot product of the two vectors,
		// and the vector part to the cross product, then we would have a quaternion that
		// rotated *twice* as much as we wanted!
		//
		// So if we instead use a vector *halfway* between the two, we have the correct encoding.
		// We calculate the half-way vector by averaging the two vectors, and renormalising.
		//

		if (Fuzzy(d3_to * d3_from) == -1)
		{
			// Must check the special case of reversal, because d3_to + d3_from can't be normalised.
			// We must construct a rotation of pi about some arbitrary axis perpendicular to d3_to.
			tC = 0;
			if (Fuzzy(d3_to.tX) != 1)
				// d3_to is not x.  Therefore, we can rotate about d3_to ^ x.
				v3S = CDir3<TR>(d3_to ^ CDir3<TR>(TR(1), 0, 0));
			else
				// d3_to is in fact x.  Therefore, we can rotate about 'y'.
				v3S = CDir3<TR>(0, TR(1), 0);
			return;
		}

		// Average and normalise the vectors (we don't need to divide by two, because that's
		// redundant when renormalising).
		CDir3<TR> d3_half_to = d3_to + d3_from;

		tC  = d3_from * d3_half_to;
		v3S = d3_from ^ d3_half_to;
		InitCounter();

		// Check that the rotation actually rotates d3_from to d3_to
		// (by checking that the dot products are 1).
		Assert(Fuzzy((d3_from * *this) * d3_to) == 1);
	}

	//******************************************************************************************
	template<class TR> CRotate3<TR> operator *(const CRotate3<TR>& r3_a, const CRotate3<TR>& r3_b)
	{
		//
		// Quaternion multiplication is sort of like complex number multiplication, but the
		// imaginary part is a vector, with the following properties:
		//
		//		x*x = y*y = z*z = -1
		//		x*y = -y*x = z
		//		y*z = -z*y = x
		//		z*x = -x*z = y
		//
		// An equivalent way of looking at quaternion multiplication is to split the quaternion
		// q into scalar and vector componenets (c,v).  Then
		//
		//		q1 * q2 = (c1,v1) * (c2,v2) = (c1*c2 - v1*v2, c1*v2 + c2*v1 + v2 x v1)
		//
		// Anyway, either approach leads to the following:
		//

		return CRotate3<TR>
		(
			r3_a.tC * r3_b.tC      -  r3_a.v3S * r3_b.v3S,
			r3_a.tC * r3_b.v3S.tX  +  r3_a.v3S.tX * r3_b.tC  -  r3_a.v3S.tY * r3_b.v3S.tZ  +  r3_a.v3S.tZ * r3_b.v3S.tY,
			r3_a.tC * r3_b.v3S.tY  +  r3_a.v3S.tY * r3_b.tC  -  r3_a.v3S.tZ * r3_b.v3S.tX  +  r3_a.v3S.tX * r3_b.v3S.tZ,
			r3_a.tC * r3_b.v3S.tZ  +  r3_a.v3S.tZ * r3_b.tC  -  r3_a.v3S.tX * r3_b.v3S.tY  +  r3_a.v3S.tY * r3_b.v3S.tX
#if VER_QUATERNION_COUNT
			,
			r3_a.iCount + r3_b.iCount + 1
#endif
		);
	}

	//******************************************************************************************
	template<class TR> CRotate3<TR> CRotate3<TR>::operator *(TR r_scale)
	//
	// I have not found any faster method of quaternion interpolation than using an arccosine
	// to retrieve the angle of rotation from the quaternion, interpolating that, and
	// reconstructing the quaternion.
	//
	{
		// If it's an identity rotation, just return it again.
		// Otherwise, the v3S component should be non-zero.
		if (Fuzzy(tC) == 1)
			return *this;

		TR r_ang_half = acos(tC) * r_scale;
		return CRotate3<TR>
		(
			// Construct from components.
			cos(r_ang_half),
			CVector3<>(CDir3<>(v3S)) * sin(r_ang_half)
		);
	}

	//******************************************************************************************
	template<class TR> CVector3<TR> operator *(const CVector3<TR>& v3, const CRotate3<TR>& r3)
	{
		//
		// This function could be implemented as "return v3 * CMatrix3(r3)".
		// The code here does essentially that, but performs the multiplications directly
		// rather than storing then reading the matrix elements (because each matrix element
		// is needed only once).
		//

		// Store products we need more than once.
		TR t_cc = r3.tC     * r3.tC;
		TR t_cx = r3.tC     * r3.v3S.tX;
		TR t_cy = r3.tC     * r3.v3S.tY;
		TR t_cz = r3.tC     * r3.v3S.tZ;
		TR t_xx = r3.v3S.tX * r3.v3S.tX;
		TR t_xy = r3.v3S.tX * r3.v3S.tY;
		TR t_xz = r3.v3S.tX * r3.v3S.tZ;
		TR t_yy = r3.v3S.tY * r3.v3S.tY;
		TR t_yz = r3.v3S.tY * r3.v3S.tZ;
		TR t_zz = r3.v3S.tZ * r3.v3S.tZ;

		return CVector3<TR>
		(
			2 * (v3.tX * (t_cc + t_xx - 0.5) +  
			     v3.tY * (t_xy - t_cz      ) +
			     v3.tZ * (t_xz + t_cy      )),

			2 * (v3.tX * (t_xy + t_cz      ) + 
			     v3.tY * (t_cc + t_yy - 0.5) +
			     v3.tZ * (t_yz - t_cx      )),

			2 * (v3.tX * (t_xz - t_cy      ) +
			     v3.tY * (t_yz + t_cx      ) +
			     v3.tZ * (t_cc + t_zz - 0.5))
		);
	}

	//******************************************************************************************
	template<class TR> CRotate3<TR>::CRotate3(const SFrame3<TR>& fr3_from, const SFrame3<TR>& fr3_to)
	{
		//
		// We do this in 2 stages.
		//
		// First, build r3_z, a great-circle rotation which moves fr3_from.d3Z to fr3_to.d3Z.
		//
		CRotate3<TR> r3_z(fr3_from.d3Z, fr3_to.d3Z);

		//
		// d3_y2 is the result of applying this rotation to fr3_from.d3Y.
		//
		CDir3<TR> d3_y2 = fr3_from.d3Y * r3_z;
		 
		//
		// Now we need to concatenate another rotation which moves d3_y2 to fr3_to.d3Y,
		// about the axis fr3_to.d3Z (thus preserving fr3_to.d3Z).  Fortunately, since both 
		// d3_y2 and fr3_to.d3Y are perpendicular to fr3_to.d3Z, the great circle rotation
		// is precisely the one we want.
		// 

		if (Fuzzy(d3_y2 * fr3_to.d3Y) == -1)
		{
			//
			// If the rotation needed is by pi, the dir-to-dir constructor will choose an
			// arbitrary axis about which to rotate, which probably won't be fr3_to.d3Z.
			// So we must manually construct the rotation which does this.
			//
			*this = r3_z * CRotate3<TR>((TR)0, CVector3<TR>(fr3_to.d3Z));
		}
		else
			*this = r3_z * CRotate3<TR>(d3_y2, fr3_to.d3Y);

		InitCounter();

		// Check that the rotation actually rotates fr3_from to fr3_to
		// (by checking that the dot products are 1).
		Assert(Fuzzy((fr3_from.d3Z * *this) * fr3_to.d3Z) == 1);
		Assert(Fuzzy((fr3_from.d3Y * *this) * fr3_to.d3Y) == 1);
	}

	//******************************************************************************************
	template<class TR> CRotate3<TR>::CRotate3(const CMatrix3<TR>& mx3)
	{
		// Extract the destination z and y vectors from the matrix.
		CDir3<> d3_z = mx3.v3Z;
		CDir3<> d3_y = mx3.v3Y;

		//
		// Make Y perpendicular to Z by applying the following formula:
		//		Y => Y - Z * (Y*Z)
		//

		// Avoid composite operator, because we want to renormalise.
		d3_y = d3_y - CVector3<>(d3_z) * (d3_y * d3_z);

		// Construct the rotation which converts the default frame to the one for v3_z and v3_y.
		new(this) CRotate3<>(SFrame3<>(), SFrame3<>(d3_z, d3_y));
	}
