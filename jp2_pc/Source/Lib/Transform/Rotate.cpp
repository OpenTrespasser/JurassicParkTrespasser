/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Rotate.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/Rotate.cpp                                              $
 * 
 * 38    98/10/03 22:34 Speter
 * Fixed matrix-quat conversion in 180 turn cases.
 * 
 * 37    8/26/98 6:13p Asouth
 * added the 'class' keyword to explicit template instantiation
 * 
 * 36    98/08/13 14:46 Speter
 * Fixed bug in matirx-to-rotate conversion.
 * 
 * 35    98/07/30 13:42 Speter
 * In slerp operator, replaced fuzzy test for identity with exact bHasRotation().
 * 
 * 34    98/07/03 19:48 Speter
 * Added identity constants.
 * 
 * 33    98/04/21 19:48 Speter
 * Disabled problematic assert.
 * 
 * 32    98/04/14 20:15 Speter
 * Added more fixes for false asserts.
 * 
 * 31    98/04/13 17:59 Speter
 * Added #iffed-out code to perform optimised primary direction transforms.
 * 
 * 30    97/12/09 12:15 Speter
 * Removed temporary vars from v3Rotate; should be just as fast with duplciated multiplies.
 * 
 * 29    97/12/03 18:01 Speter
 * Completely removed VER_QUATERNION_COUNT flag. 
 * 
 * 28    97/10/08 14:54 Speter
 * Now do quick check for identity rotation before transforming, if VER_SKIP_IDENTITY is on.
 * Turned off VER_QUATERNION_COUNT in all versions, updated #ifs to make it work.  
 * 
 * 27    97/07/07 15:36 Speter
 * A few optimisations by re-organising code.
 * 
 * 26    97/06/03 18:43 Speter
 * Bye-bye #include "*.cpp".  Made .cpp files actual independent files by adding explicit
 * template instantiation.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Rotate.hpp"

#include <math.h>

//******************************************************************************************
//
// CRotate3<TR> implementation.
//

#if VER_DEBUG
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

		// Average and normalise the vectors (we don't need to divide by two, because that's
		// redundant when renormalising).
		CVector3<> v3_half_to = d3_from + d3_to;

		if (v3_half_to.tLenSqr() <= 1e-8)
		{
			// Must check the special case of reversal, because d3_to + d3_from can't be normalised.
			// We must construct a rotation of pi about some arbitrary axis perpendicular to d3_to.
			tC = 0;
			if (Fuzzy(d3_to.tX) != 1)
				// d3_to is not x.  Therefore, we can rotate about d3_to ^ x.
				v3S = CDir3<TR>(d3_to ^ d3XAxis);
			else
				// d3_to is almost x.  Therefore, we can rotate about d3_to ^ y.
				v3S = CDir3<TR>(d3_to ^ d3YAxis);
			return;
		}

		CDir3<TR> d3_half_to = v3_half_to;

		tC  = d3_from * d3_half_to;
		v3S = d3_from ^ d3_half_to;

#if 0 && VER_DEBUG
		// Check that the rotation actually rotates d3_from to d3_to.
		// Renormalise all input/output vectors, so as not to trigger asserts.
		CDir3<TR> d3_from_check(d3_from, false);
		CDir3<TR> d3_to_check = d3_from_check * *this;

		// Ignore any denormalisation on input vectors by renormalising.
		d3_to_check.Normalise();
		Assert(Fuzzy(d3_to_check * d3_to) == 1.0);
#endif
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
		// This prevents trying to normalise a zero v3S.
		if (!bHasRotation())
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
	template<class TR> CVector3<TR> CRotate3<TR>::v3Rotate(const CVector3<TR>& v3) const
	{
		//
		// This function could be implemented as "return v3 * CMatrix3(r3)".
		// The code here does essentially that, but performs the multiplications directly
		// rather than storing then reading the matrix elements (because each matrix element
		// is needed only once).
		//

		// Store products we need more than once.
		TR t_cc = tC * tC - 0.5;

		return CVector3<TR>
		(
			2 * (v3.tX * (t_cc + v3S.tX * v3S.tX) +  
			     v3.tY * (v3S.tX * v3S.tY - tC * v3S.tZ) +
			     v3.tZ * (v3S.tX * v3S.tZ + tC * v3S.tY)),

			2 * (v3.tX * (v3S.tX * v3S.tY + tC * v3S.tZ) + 
			     v3.tY * (t_cc + v3S.tY * v3S.tY) +
			     v3.tZ * (v3S.tY * v3S.tZ - tC * v3S.tX)),

			2 * (v3.tX * (v3S.tX * v3S.tZ - tC * v3S.tY) +
			     v3.tY * (v3S.tY * v3S.tZ + tC * v3S.tX) +
			     v3.tZ * (t_cc + v3S.tZ * v3S.tZ))
		);
	}

#if 0
	//******************************************************************************************
	template<class TR> CDir3<TR> CRotate3<TR>::d3X() const
	{
		that->tX = 2 * (tC * tC - 0.5 + v3S.tX * v3S.tX);
		that->tY = 2 * (v3S.tX * v3S.tY + tC * v3S.tZ);
		that->tZ = 2 * (v3S.tX * v3S.tZ - tC * v3S.tY);

		Assert(that->bIsNormalised());
		return *that;
	}

	//******************************************************************************************
	template<class TR> CDir3<TR> CRotate3<TR>::d3Y() const
	{
		that->tX = 2 * (v3S.tX * v3S.tY - tC * v3S.tZ);
		that->tY = 2 * (tC * tC - 0.5 + v3S.tY * v3S.tY);
		that->tZ = 2 * (v3S.tY * v3S.tZ + tC * v3S.tX);

		Assert(that->bIsNormalised());
		return *that;
	}

	//******************************************************************************************
	template<class TR> CDir3<TR> CRotate3<TR>::d3Z() const
	{
		that->tX = 2 * (v3S.tX * v3S.tZ + tC * v3S.tY);
		that->tY = 2 * (v3S.tY * v3S.tZ - tC * v3S.tX);
		that->tZ = 2 * (tC * tC - 0.5 + v3S.tZ * v3S.tZ);

		Assert(that->bIsNormalised());
		return *that;
	}

	//******************************************************************************************
	friend CDir3<TR> operator *(const CDir3X<TR>& d3x, const CRotate3<TR>& r3)
	{
		if (r3.bDoRotation())
			return r3.d3X();
		else
			return d3x;
	}

	//******************************************************************************************
	friend CDir3<TR> operator *(const CDir3Y<TR>& d3y, const CRotate3<TR>& r3)
	{
		if (r3.bDoRotation())
			return r3.d3Y();
		else
			return d3y;
	}

	//******************************************************************************************
	friend CDir3<TR> operator *(const CDir3Z<TR>& d3z, const CRotate3<TR>& r3)
	{
		if (r3.bDoRotation())
			return r3.d3Z();
		else
			return d3z;
	}

#endif

	//******************************************************************************************
	template<class TR> CRotate3<TR> CRotate3<TR>::r3Rotate(const CRotate3<TR>& r3) const
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
			r3.tC * tC      -  r3.v3S * v3S,
			r3.tC * v3S.tX  +  r3.v3S.tX * tC  -  r3.v3S.tY * v3S.tZ  +  r3.v3S.tZ * v3S.tY,
			r3.tC * v3S.tY  +  r3.v3S.tY * tC  -  r3.v3S.tZ * v3S.tX  +  r3.v3S.tX * v3S.tZ,
			r3.tC * v3S.tZ  +  r3.v3S.tZ * tC  -  r3.v3S.tX * v3S.tY  +  r3.v3S.tY * v3S.tX
		);
	}

	//******************************************************************************************
	template<class TR> CRotate3<TR>::CRotate3(const CMatrix3<TR>& mx3, bool b_normalised)
	{
		// Extract the destination z and y vectors from the matrix.
		CDir3<> d3_z(mx3.v3Z, b_normalised);
		CDir3<> d3_y(mx3.v3Y, b_normalised);

		if (b_normalised)
		{
			// We'll see about that.
			Assert(Fuzzy(mx3.v3X * mx3.v3Y, 0.01f) == 0);
			Assert(Fuzzy(mx3.v3X * mx3.v3Z, 0.01f) == 0);
			Assert(Fuzzy(mx3.v3X ^ mx3.v3Y, 0.01f) == mx3.v3Z);
		}
		else
		{
			//
			// Make Y perpendicular to Z by applying the following formula:
			//		Y => Y - Z * (Y*Z)
			//

			// Avoid composite operator, because we want to renormalise.
			d3_y = CVector3<>(d3_y) - CVector3<>(d3_z) * (d3_y * d3_z);
		}

		//
		// We do this in 2 stages.
		//
		// First, build r3_z, a great-circle rotation which moves d3ZAxis to d3_z.
		//
		CRotate3<TR> r3_z(d3ZAxis, d3_z);

		//
		// d3_y2 is the result of applying this rotation to d3YAxis.
		//
		CDir3<TR> d3_y2 = d3YAxis * r3_z;
		 
		//
		// Now we need to concatenate another rotation which moves d3_y2 to d3_y,
		// about the axis d3_z (thus preserving d3_z).  Fortunately, since both 
		// d3_y2 and d3_y are perpendicular to d3_z, the great circle rotation
		// is precisely the one we want.
		// 

		if ((d3_y2 + d3_y).tLenSqr() <= 1e-4)
		{
			//
			// If the rotation needed is by pi, the dir-to-dir constructor will choose an
			// arbitrary axis about which to rotate, which probably won't be d3_z.
			// So we must manually construct the rotation which does this.
			//
			*this = r3_z * CRotate3<TR>((TR)0, CVector3<TR>(d3_z));
		}
		else
			*this = r3_z * CRotate3<TR>(d3_y2, d3_y);

		// Check correct rotation with a random vector.
		if (b_normalised)
		{
			CVector3<> v3(1.3, -0.5, 0.8);
			CVector3<> v3_r = v3 * *this;
			CVector3<> v3_m = v3 * mx3;
			Assert(Fuzzy(v3_r, 0.1) == v3_m);
		}
	}

#if VER_DEBUG
	// Test.
	static CMatrix3<> mx3Test
	(
		-.953198,  .302348,  .000281,
		 .302348,  .953196,  .001809,
		 .000279,  .001809, -.999998
	);
	static CRotate3<> r3Test(mx3Test, true);

	static CMatrix3<> mx3Test2
	(
		-.999996,  .002942,  0,
		 .002942, -.999996, -.002942,
		 0,		  -.000245, 1
	);
	static CRotate3<> r3Test2(mx3Test, true);
#endif

//
// Explicit instantiation of used template types.
//

template class CRotate3<>;
template class CPlacement3<>;

const CRotate3<> r3Null;
const CPlacement3<> p3Null;
