
/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CRotate3<TR>
 *		CPlacement3<TR>
 *
 * Bugs:
 *
 * To do:
 *		Optimise conversion from Euler angles.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/Rotate.hpp                                             $
 * 
 * 47    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 46    8/26/98 2:46p Rvande
 * Changed a constant double to a float.
 * 
 * 45    98/08/25 19:03 Speter
 * Now use fInvSqrt function in vectors.
 * 
 * 44    98.08.13 4:27p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 43    98/07/03 19:48 Speter
 * Added identity constants.
 * 
 * 42    98/06/30 1:18 Speter
 * Fixed stupid bug in operator r3 /= r3, which broke magnets and gun recoil.
 * 
 * 41    98/02/10 12:56 Speter
 * Added / operators for faster application of inverse transforms.
 * 
 * 40    97/12/03 18:01 Speter
 * Completely removed VER_QUATERNION_COUNT flag.  CRotate3<> direct constructor from 4 params
 * now takes normalising flag as well.
 * 
 * 39    11/10/97 5:58p Agrant
 * pcSave and pcLoad functions
 * 
 * 38    97/10/08 14:53 Speter
 * Now do quick check for identity rotation before transforming, if VER_SKIP_IDENTITY is on.
 * Turned off VER_QUATERNION_COUNT in all versions, updated #ifs to make it work.  Removed
 * CQuaternion intermediate class, since CRotate3 can be directly initialised with 4 floats.
 * 
 * 37    97/07/07 15:36 Speter
 * A few optimisations by re-organising code.
 * 
 * 36    6/05/97 11:54p Agrant
 * unprotected the data fields of CRotate to expose them for easier debugging.
 * 
 * 35    97/06/03 18:43 Speter
 * Bye-bye #include "*.cpp".  Made .cpp files actual independent files by adding explicit
 * template instantiation.
 * 
 * 34    5/30/97 11:05a Agrant
 * Goofy LINT fixes.
 * 
 * 33    97-04-25 22:58 Speter
 * Now CRotate3<>(CMatrix3<>) performs conversion directly, without SFrame3<>.  Removed
 * SFrame3<>.  Added parameter to conversion which  selects whether matrix is already
 * normalised.
 * 
 * 32    97-04-24 19:09 Speter
 * Added constness to member functions.
 * 
 * 31    97-04-23 14:28 Speter
 * Changed tolerance for normalisation asserts (sorry).
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_ROTATE_HPP
#define HEADER_LIB_TRANSFORM_ROTATE_HPP

#include "Matrix.hpp"
#include "Translate.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/Math/FastSqrt.hpp"


// Whether to check for identity rotations, and skip transformation.
#define VER_SKIP_IDENTITY		(1)

// Threshold at which we renormalise the quaternion.
#define fMAX_QUAT_DENORMALISATION	(fMAX_VECTOR_DENORMALISATION * 0.1)

//**********************************************************************************************
//
template<class TR = TReal> class CRotate3
//
// Prefix: r3
//
// An arbitrary rotation (any axis and angle).
// Implemented as a quaternion.
//
// Note: Since we use a right-handed coordinate system, an angle of rotation about a vector
// specifies amount of clockwise rotation when looking in the direction of the vector.
//
//**************************************
{
public:
#if VER_DEBUG
	static TR		tMaxDenormalisation;
#endif

//protected:  
	// We all know by now not to change these guys without a really good reason.
	// For debugging, it's really nice to have access.

	TR				tC;				// Scalar part (cosine of half rotation angle).
	CVector3<TR>	v3S;			// Vector part (sine of half rotation angle times vector).

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	//******************************************************************************************
	//
	forceinline CRotate3()
	//
	// Identity rotation.
	//
	//**********************************
		: tC((TR)1), v3S((TR)0, (TR)0, (TR)0)
	{
	}

	//******************************************************************************************
	//
	forceinline CRotate3<TR>(TR t_w, const CVector3<TR>& v3)
	//
	// Initialise quaternion components directly.
	// Invoked by operator~ and .r3Rotate.
	//
	//**********************************
		: tC(t_w), v3S(v3)
	{
		Assert(bIsNormalised());
	}

	//******************************************************************************************
	//
	forceinline CRotate3<TR>
	(
		TR t_w, TR t_x, TR t_y, TR t_z,	// The raw components.
		bool b_normalise = true			// Whether to normalise the quaternion.
										// May be false if this is a velocity quaternion.
	)
	//
	// Initialise quaternion components even more directly.
	// Invoked by quaternion multiplication operator.
	// Therefore, we do renormalisation here.
	//
	//**********************************
		: tC(t_w), v3S(t_x, t_y, t_z)
	{
		if (b_normalise)
		{
			Normalise();
	#if VER_DEBUG
			TR t = Abs(tLenSqr() - (TR)1);
			if (t > tMaxDenormalisation)
			{
				tMaxDenormalisation = t;
			}
	#endif
		}
	}

	//******************************************************************************************
	CRotate3
	(
		const CDir3<TR>& d3,			// Axis about which to rotate.
		CAngle ang						// Angle of rotation (clockwise when looking down vector).
	);
	//**********************************

	//******************************************************************************************
	CRotate3
	(
		const CVector3<TR>& v3			// Rotation vector to apply.
										// This is a vector whose direction is the axis of rotation,
										// and whose magnitude specifies the amount of rotation,
										// in radians.
	);
	//**********************************

	//******************************************************************************************
	CRotate3
	(
		const CDir3<TR>&	d3_from,	// Starting direction.
		const CDir3<TR>&	d3_to		// Destination direction.
	);
	//
	// Constructs a rotation that moves d3_from to d3_to, via the shortest path (great circle).
	//
	//**********************************

	//******************************************************************************************
	CRotate3
	(
		const CMatrix3<TR>& mx3,		// A transformation matrix.
		bool b_is_normalised = false	// Whether matrix is assumed to be orthonormal.
	);
	//
	// Constructs a rotation from a matrix.  If matrix is not normalised, approximates it.
	//
	//**********************************

	//******************************************************************************************
	//
	// Conversion operators.
	//

	// Bug: Due to a confirmed Microsoft bug, conversion operators in template classes
	// must be declared inline in the class.  That's why the following function is here
	// (even though I'd rather put it outside the class).

	operator CMatrix3<TR> () const
	{
		// From Graphics Gems II, Sec VII.6.

		// Store products we need more than once.
		TR t_cc = tC     * tC     - 0.5f;
		TR t_cx = tC     * v3S.tX;
		TR t_cy = tC     * v3S.tY;
		TR t_cz = tC     * v3S.tZ;
		TR t_xx = v3S.tX * v3S.tX;
		TR t_xy = v3S.tX * v3S.tY;
		TR t_xz = v3S.tX * v3S.tZ;
		TR t_yy = v3S.tY * v3S.tY;
		TR t_yz = v3S.tY * v3S.tZ;
		TR t_zz = v3S.tZ * v3S.tZ;

		return CMatrix3<TR>
		(
			2 * (t_cc + t_xx),  2 * (t_xy + t_cz),  2 * (t_xz - t_cy),
			2 * (t_xy - t_cz),  2 * (t_cc + t_yy),  2 * (t_yz + t_cx),
			2 * (t_xz + t_cy),  2 * (t_yz - t_cx),  2 * (t_cc + t_zz)
		);
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Return the inverse of the rotate.
	forceinline CRotate3<TR> operator ~() const
	{
		// For unit quaternions, the inverse is equal to the conjugate (negated vector part).
		return CRotate3<TR>(tC, -v3S);
	}

	//
	// Concatenate with another rotation transform.
	//

	// BUG: When this guy is inlined in VC6.0 and one of the rotations is unit, the result
	// is bad.
	friend  CRotate3<TR> operator *(const CRotate3<TR>& r3_a, const CRotate3<TR>& r3_b)
	{
		if (!r3_a.bDoRotation())
			return r3_b;
		else if (!r3_b.bDoRotation())
			return r3_a;
		else
			return r3_b.r3Rotate(r3_a);
	}

	CRotate3<TR>& operator *=(const CRotate3<TR>& r3)
	{
		if (r3.bDoRotation())
			*this = r3.r3Rotate(*this);
		return *this;
	}

	//
	// Concatenate with an inverse rotation transform.
	//

	friend CRotate3<TR> operator /(const CRotate3<TR>& r3_a, const CRotate3<TR>& r3_b)
	{
		if (!r3_a.bDoRotation())
			return ~r3_b;
		else if (!r3_b.bDoRotation())
			return r3_a;
		else
			return (~r3_b).r3Rotate(r3_a);
	}

	CRotate3<TR>& operator /=(const CRotate3<TR>& r3)
	{
		if (r3.bDoRotation())
			*this = (~r3).r3Rotate(*this);
		return *this;
	}

	//
	// Interpolate rotation by a parameter.  Need not be between 0 and 1.
	// Note: For interpolations in a loop, this is slow:
	//
	//		CRotate3<> r3_total;
	//		CDir3<> d3_orig, d3_new;
	//		for (float f = 0; f < 1; f += 0.1)
	//			d3_new = d3_orig * (r3_total * f);
	//
	// and this is fast:
	//
	//		CRotate3<> r3_total;
	//		CDir3<> d3_orig;
	//		CDir3<> d3_new = d3_orig;
	//		CRotate3<> r3_delta = r3_total * 0.1;
	//
	//		for (int i = 0; i < 10; i++)
	//			d3_new *= r3_delta;
	//
	CRotate3<TR> operator *(TR r_scale);

	forceinline CRotate3<TR>& operator *=(TR r_scale)
	{
		return *this = *this * r_scale;
	}

	//
	// Operate on a vector.
	//

	friend forceinline CVector3<TR> operator *(const CVector3<TR>& v3, const CRotate3<TR>& r3)
	{
		if (r3.bDoRotation())
			return r3.v3Rotate(v3);
		else
			return v3;
	}

	friend forceinline CVector3<TR>& operator *=(CVector3<TR>& v3, const CRotate3<TR>& r3)
	{
		if (r3.bDoRotation())
			v3 = r3.v3Rotate(v3);
		return v3;
	}

	friend forceinline CVector3<TR> operator /(const CVector3<TR>& v3, const CRotate3<TR>& r3)
	{
		if (r3.bDoRotation())
			return (~r3).v3Rotate(v3);
		else
			return v3;
	}

	friend forceinline CVector3<TR>& operator /=(CVector3<TR>& v3, const CRotate3<TR>& r3)
	{
		if (r3.bDoRotation())
			v3 = (~r3).v3Rotate(v3);
		return v3;
	}

	//
	// Do directions as well to avoid ambiguities with CPlacement3 below.
	//

	friend forceinline CDir3<TR> operator *(const CDir3<TR>& d3, const CRotate3<TR>& r3)
	{
		// Transform dir like a vector, then copy to a direction, bypassing renormalisation.
		if (r3.bDoRotation())
			return CDir3<TR>(r3.v3Rotate(d3), true);
		else
			return d3;
	}

	friend forceinline CDir3<TR>& operator *=(CDir3<TR>& d3, const CRotate3<TR>& r3)
	{
		if (r3.bDoRotation())
			d3 = CDir3<>(r3.v3Rotate(d3), true);
		return d3;
	}

	friend forceinline CDir3<TR> operator /(const CDir3<TR>& d3, const CRotate3<TR>& r3)
	{
		// Transform dir like a vector, then copy to a direction, bypassing renormalisation.
		if (r3.bDoRotation())
			return CDir3<TR>((~r3).v3Rotate(d3), true);
		else
			return d3;
	}

	friend forceinline CDir3<TR>& operator /=(CDir3<TR>& d3, const CRotate3<TR>& r3)
	{
		if (r3.bDoRotation())
			d3 = CDir3<>((~r3).v3Rotate(d3), true);
		return d3;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	bool bHasRotation() const
	//
	// Returns:
	//		Whether this object constitutes a non-identity rotation.
	//
	//**********************************
	{
		return tC != TR(1);
	}

	//******************************************************************************************
	//
	bool bDoRotation() const
	//
	// Returns:
	//		Whether we should rotate with this object.
	//
	//**********************************
	{
#if VER_SKIP_IDENTITY
		return tC != TR(1);
#else
		return true;
#endif
	}

	//*****************************************************************************************
	char * pcSave(char * pc_buffer) const
	// Same as the CInstance function of the same name...
	{
		*((CRotate3<TR>*)pc_buffer) = *this;
		return pc_buffer + sizeof(CRotate3<TR>);
	}

	//*****************************************************************************************
	const char* pcLoad(const char * pc_buffer)
	// Same as the CInstance function of the same name...
	{
		*this = *((CRotate3<TR>*)pc_buffer);
		return pc_buffer + sizeof(CRotate3<TR>);
	}


	//******************************************************************************************
	//
	forceinline TR tLenSqr() const
	//
	// Returns:
	//		The square of the length of the quaternion, as if a 4D vector.
	//
	//**********************************
	{
		return tC * tC + v3S * v3S;
	}

	//******************************************************************************************
	//
	TR tLen() const
	//
	// Returns:
	//		Length of the quaternion, as if a 4D vector.
	//
	//**********************************
	{
		return (TR) fSqrt(tLenSqr());
	}

	//******************************************************************************************
	//
	bool bIsNormalised() const
	//
	// Returns:
	//		Whether the square of the length of the quaternion is fuzzily equal to 1.
	//
	//**********************************
	{
		return Fuzzy(tLenSqr(), (TR)fMAX_QUAT_DENORMALISATION) == 1;
	}

	//******************************************************************************************
	//
	void Normalise
	(
		bool b_always = false			// Always renormalise without checking.
	)
	//
	// Sets the 4-dimensional quaternion length to 1.
	// If b_always is false, check first.  This will generally be more efficient,
	// as the checking is much faster than the renormalising.
	//
	//**********************************
	{
		TR t_lensqr = tLenSqr();

		// If requested, test whether already approximately normalised.
		if (!b_always && Fuzzy(t_lensqr, (TR)fMAX_QUAT_DENORMALISATION) == 1)
			return;

		Assert(Fuzzy(t_lensqr) != 0);
		TR t_invlen = fInvSqrt(t_lensqr);

		tC  *= t_invlen;
		v3S *= t_invlen;

		Assert(bIsNormalised());
	}

protected:

	//******************************************************************************************
	//
	// Member functions.
	//

	//
	// Public operator helper functions.
	//

	//******************************************************************************************
	//
	CVector3<TR> v3Rotate
	(
		const CVector3<TR>& v3
	) const;
	//
	// Returns:
	//		The vector rotated by this rotation.
	//
	//**********************************

	//******************************************************************************************
	//
	CRotate3<TR> r3Rotate
	(
		const CRotate3<TR>& r3
	) const;
	//
	// Returns:
	//		The rotation rotated by this rotation.
	//
	//**********************************

};

extern const CRotate3<> r3Null;


//**********************************************************************************************
//
template<class TR = TReal> class CPlacement3
//
// Prefix: p3
//
// A rigid transform, capable of describing an object's placement in a coordinate system.
// Contains a rotation and a translation vector.
//
//**************************************
{
public:
	CVector3<TR>	v3Pos;				// The translation to add.
	CRotate3<TR>	r3Rot;				// The non-translating transform.

public:
	
	//******************************************************************************************
	//
	// Constructors.
	//

	forceinline CPlacement3()
		: v3Pos(0, 0, 0)
	{
	}

	//
	// Provide constructors for all combinations of rotation and translation.
	//

	forceinline CPlacement3(const CRotate3<TR>& r3, const CVector3<TR>& v3)
		: r3Rot(r3), v3Pos(v3)
	{
	}

	forceinline CPlacement3(const CRotate3<TR>& r3)
		: r3Rot(r3), v3Pos(0, 0, 0)
	{
	}

	forceinline CPlacement3(const CVector3<TR>& v3)
		: v3Pos(v3)
	{
	}

	forceinline CPlacement3(const CPlacement3<TR>& p3)
		: v3Pos(p3.v3Pos), r3Rot(p3.r3Rot)
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//


	//******************************************************************************************
	//
	forceinline void SetOrigin
	(
		const CVector3<TR>& v3_origin	// Point acting as origin of rotation.
	)
	//
	// Adjusts the transform so that the rotation is centred on v3_origin.
	//
	//**********************************
	{
		//
		// To do the operation, first translate the object from v3_at back to the origin, 
		// do the rotation, then put the object back at v3_at.
		//
		// This is the same as just setting the translation to -v3_at * mx3 + v3_at.
		//
		v3Pos = -v3_origin * r3Rot + v3_origin;
	}

	//******************************************************************************************
	//
	forceinline void AdjustOrigin
	(
		const CVector3<TR>& v3_origin	// Point acting as origin of transformation.
	)
	//
	// Adjusts the placement so that it is centred on v3_origin.  This is similar
	// to SetOrigin() above, except that the placement's current translation is kept, and added
	// to the new translation.
	//
	//**********************************
	{
		v3Pos += -v3_origin * r3Rot + v3_origin;
	}

	//******************************************************************************************
	//
	friend forceinline CPlacement3<TR> TransformAt(const CRotate3<TR>& r3, const CVector3<TR>& v3_at)
	//
	// Returns:
	//		A CPlacement3 which performs the rotation r3 as if the point v3_at
	//		were the origin.
	//
	// Notes:
	//		This function does not have a type prefix because it is meant to be generic.
	//		There is another version for CMatrix3 and CTransform3.
	//
	//**********************************
	{
		//
		// To do the operation, first translate the object from v3_at back to the origin, 
		// do the rotation, then put the object back at v3_at.
		//
		// This is the same as just setting the translation to -v3_at * r3 + v3_at.
		//
		return CPlacement3<TR>(r3, (-v3_at) * r3 + v3_at);
	}

	//*****************************************************************************************
	char * pcSave(char * pc_buffer) const
	// Same as the CInstance function of the same name...
	{
		*((CPlacement3<TR>*)pc_buffer) = *this;
		return pc_buffer + sizeof(CPlacement3<TR>);
	}

	//*****************************************************************************************
	const char* pcLoad(const char *pc_buffer)
	// Same as the CInstance function of the same name...
	{
		*this = *((CPlacement3<TR>*)pc_buffer);
		return pc_buffer + sizeof(CPlacement3<TR>);
	}

	//******************************************************************************************
	//
	// Conversion operators.
	//

	operator CTransform3<TR> () const
	{
		return CTransform3<TR> ((CMatrix3<TR>)r3Rot, v3Pos);
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Return the inverse of the transform.
	CPlacement3<TR> operator ~() const
	{
		// The inverse of a composite operation S*TR is ~TR * ~S, which is CPlacement3(~S, -TR * ~S).
		CRotate3<TR> r3_inverse = ~r3Rot;
		return CPlacement3<TR>(r3_inverse, -v3Pos * r3_inverse);
	}

	//
	// Concatenate with another CPlacement3.
	//

	friend forceinline CPlacement3<TR> operator *(const CPlacement3<TR>& p3_a, const CPlacement3<TR>& p3_b)
	{
		return CPlacement3<TR>(p3_a.r3Rot * p3_b.r3Rot, p3_a.v3Pos * p3_b);
	}

	forceinline CPlacement3<TR>& operator *=(const CPlacement3<TR>& p3)
	{
		// Concatenate base transform and translation separately.
		r3Rot *= p3.r3Rot;
		v3Pos *= p3;
		return *this;
	}

	//
	// Concatenate with an inverse CPlacement3.
	//

	friend forceinline CPlacement3<TR> operator /(const CPlacement3<TR>& p3_a, const CPlacement3<TR>& p3_b)
	{
		return CPlacement3<TR>(p3_a.r3Rot / p3_b.r3Rot, p3_a.v3Pos / p3_b);
	}

	forceinline CPlacement3<TR>& operator /=(const CPlacement3<TR>& p3)
	{
		// Concatenate base transform and translation separately.
		r3Rot /= p3.r3Rot;
		v3Pos /= p3;
		return *this;
	}

	forceinline CTransform3<TR> operator *(const CTransform3<TR>& tf3) const
	{
		return CTransform3<TR>(*this) * tf3;
	}

	//
	// Concatenate with simple rotation of same type.
	//

	friend forceinline CPlacement3<TR> operator *(const CPlacement3<TR>& p3, const CRotate3<TR>& r3)
	{
		return CPlacement3<TR>(p3.r3Rot * r3, p3.v3Pos * r3);
	}

	forceinline CPlacement3<TR>& operator *=(const CRotate3<TR>& r3)
	{
		// Concatenate base transform and translation separately.
		r3Rot *= r3;
		v3Pos *= r3;
		return *this;
	}

	friend forceinline CPlacement3<TR> operator *(const CRotate3<TR>& r3, const CPlacement3<TR>& p3)
	{
		return CPlacement3<TR>(r3 * p3.r3Rot, p3.v3Pos);
	}

	friend forceinline CPlacement3<TR> operator /(const CPlacement3<TR>& p3, const CRotate3<TR>& r3)
	{
		return CPlacement3<TR>(p3.r3Rot / r3, p3.v3Pos / r3);
	}

	CPlacement3<TR>& operator /=(const CRotate3<TR>& r3)
	{
		// Concatenate base transform and translation separately.
		r3Rot /= r3;
		v3Pos /= r3;
		return *this;
	}

	friend forceinline CPlacement3<TR> operator /(const CRotate3<TR>& r3, const CPlacement3<TR>& p3)
	{
		return CPlacement3<TR>(r3 / p3.r3Rot, -p3.v3Pos / p3.r3Rot);
	}

	//
	// Concatenate with a translation.
	//

	friend forceinline CPlacement3<TR> operator *(const CPlacement3<TR>& p3, const CTranslate3<TR>& tl3)
	{
		return CPlacement3<TR>(p3.r3Rot, p3.v3Pos + tl3.v3Pos);
	}

	forceinline CPlacement3<TR>& operator *=(const CTranslate3<TR>& tl3)
	{
		v3Pos += tl3.v3Pos;
		return *this;
	}

	friend forceinline CPlacement3<TR> operator *(const CTranslate3<TR>& tl3, const CPlacement3<TR>& p3)
	{
		return CPlacement3<TR>(p3.r3Rot, tl3.v3Pos * p3);
	}

	//
	// Combine a rotation and translation into a position.
	//

	friend forceinline CPlacement3<TR> operator *(const CRotate3<TR>& r3, const CTranslate3<TR>& tl3)
	{
		return CPlacement3<TR>(r3, tl3.v3Pos);
	}

	friend forceinline CPlacement3<TR> operator *(const CTranslate3<TR>& tl3, const CRotate3<TR>& r3)
	{
		return CPlacement3<TR>(r3, tl3.v3Pos * r3);
	}

	//
	// Operate on a vector.
	//

	friend forceinline CVector3<TR> operator *(const CVector3<TR>& v3, const CPlacement3<TR>& p3)
	{
		// Perform the transform, then add the translation.
		return v3 * p3.r3Rot + p3.v3Pos;
	}

	friend forceinline CVector3<TR>& operator *=(CVector3<TR>& v3, const CPlacement3<TR>& p3)
	{
		v3 *= p3.r3Rot;
		v3 += p3.v3Pos;
		return v3;
	}

	friend forceinline CVector3<TR> operator /(const CVector3<TR>& v3, const CPlacement3<TR>& p3)
	{
		// Subtract the translation, then perform the inverse rotation.
		return (v3 - p3.v3Pos) / p3.r3Rot;
	}

	friend forceinline CVector3<TR>& operator /=(CVector3<TR>& v3, const CPlacement3<TR>& p3)
	{
		v3 -= p3.v3Pos;
		v3 /= p3.r3Rot;
		return v3;
	}

	//
	// Operate on a direction by skipping the translation step.
	//

	friend forceinline CDir3<TR> operator *(const CDir3<TR>& d3, const CPlacement3<TR>& p3)
	{
		return d3 * p3.r3Rot;
	}

	friend forceinline CDir3<TR>& operator *=(CDir3<TR>& d3, const CPlacement3<TR>& p3)
	{
		return d3 *= p3.r3Rot;
	}

	friend forceinline CDir3<TR> operator /(const CDir3<TR>& d3, const CPlacement3<TR>& p3)
	{
		return d3 / p3.r3Rot;
	}

	friend forceinline CDir3<TR>& operator /=(CDir3<TR>& d3, const CPlacement3<TR>& p3)
	{
		return d3 /= p3.r3Rot;
	}
};

extern const CPlacement3<> p3Null;

#if _MSC_VER < 1100
template<class X> class CMicrosoftsCompilerIsStinky
{
};
#endif

#endif
