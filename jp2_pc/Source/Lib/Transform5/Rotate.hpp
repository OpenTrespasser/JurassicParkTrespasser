
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
 *		Incorporate fast sqrt routines when they have enough precision.
 *		Optimise conversion from Euler angles.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform5/Rotate.hpp                                            $
 * 
 * 26    97-04-24 18:40 Speter
 * Folded new changes from 4.2 version into this 5.0 specific version.
 * 
 * 31    97-04-23 14:28 Speter
 * Changed tolerance for normalisation asserts (sorry).
 * 
 * 30    97-04-22 10:50 Speter
 * Sped up Dir * Rotate function by avoiding dir renormalisation.
 * 
 * 29    4/16/97 2:24p Agrant
 * Restored SFrame to allow CRotate --> CMatrix conversion
 * 
 * 28    97/03/24 14:54 Speter
 * Removed constructors of CDirs and CRotates from ASCII chars; use d3ZAxis etc. constants.
 * When we need optimisation for axes, we'll use derived classes which don't need a switch
 * statement.
 * 
 * 27    97/02/19 17:54 Speter
 * Made direct constructors public.
 * 
 * 26    97/02/07 19:15 Speter
 * Removed SFrame3, associated CRotate3 functions, as they were inefficient and were replaced
 * with equivalent CMatrix functions.  Removed CMatrix3 to CRotate3 conversion, as it used
 * SFrame3 and wasn't used.
 * 
 * 25    97/01/26 20:00 Speter
 * Added operator to interpolate rotation by a scalar.
 * Sped up rotate vector, rotation to matrix, and rotation from vector operations.
 * Aligned code.
 * 
 * 24    12/20/96 3:52p Mlange
 * Moved the CRotate3 constructor that takes a character axis and angle to the class
 * declaration in the header file.
 * 
 * 23    96/12/10 11:06 Speter
 * Upped fMAX_DENORMALISATION to .01 from .0001.
 * 
 * 22    12/09/96 8:46p Agrant
 * Added assert in constructor for CRotate taking a CQuaternion
 * 
 * 21    12/09/96 12:43p Agrant
 * Removed the 4 TR constructor, paid 5 bucks.
 * 
 * 20    12/06/96 9:51p Agrant
 * Hacks to CRotate for physics integration.
 * It turns out that we have two different Quaternions.
 * Will fix it up soon.
 * 
 * 19    96/12/04 20:19 Speter
 * Changed v3T to v3Pos in all transforms.  Changed r3T to r3Rot.
 * Changed CPlacement3(CTranslate3) to CPlacement3(CVector3).
 * 
 * 
 * 
 * 18    96/11/20 11:52 Speter
 * Added constructor for CRotate3 from CMatrix3.
 * 
 * 17    96/10/18 19:03 Speter
 * Took out include of <new.h>, now that it's in Common.hpp.
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
 * 14    96/09/09 18:32 Speter
 * Added SFrame3 structure, and CRotate3 constructor which rotates one frame to another.
 * 
 * 13    96/08/21 18:22 Speter
 * Changes from code review:
 * Added default template parameter to all classes.
 * Made all in-class friends inline.
 * Removed CRotate3(char, CAngle) and CRotate3(char*, CAngle, CAngle, CAngle).  Replaced with
 * CRotate3(uint32, CAngle, CAngle = 0, CAngle = 0), where first param is a multi-byte
 * character.  Removed 's' and 'r' specifiers for static and rotating frames.  Rotating frames
 * must now be specified by reversing the order of axes.  See notes.
 * Added CRotate3 * CDir3 operators, to disambiguate between CPlacement3 * CDir3.
 * Changed tLen2() to tLenSqr().
 * Changed calculation slightly in Normalise(), added Asserts.
 * Added SetOrigin() and AdjustOrigin() functions.
 * Added many comments, fixed some prefixes.
 * 
 * 12    96/08/06 18:23 Speter
 * Reversed order of d3_to and d3_from in constructor.
 * Added CPlacement3 constructor which takes a CTranslate3.
 * 
 * 11    96/08/01 11:04 Speter
 * Added automatic renormalisation.
 * 
 * 10    96/07/23 11:01 Speter
 * Added static denormalisation variables for debugging.
 * 
 * 9     96/07/12 17:35 Speter
 * Added Rotate3 constructor taking a rotation vector.
 * 
 * 8     96/07/08 12:40 Speter
 * Changed name of CNormal3 to CDir3 (more general).
 * Added specific functions to transform directions, which do not translate positions.
 * 
 * 7     96/06/26 22:07 Speter
 * Added a bunch of comments, prettied things up nicely.
 * 
 * 6     96/06/26 17:03 Speter
 * Really fixed rotation.
 * 
 * 5     96/06/26 16:43 Speter
 * Fixed matrix and vector operations.  They were transposed!
 * 
 * 4     96/06/26 13:16 Speter
 * Changed TGReal to TR and prefix gr to r.
 * 
 * 3     96/06/25 14:35 Speter
 * Finalised design of transform classes, with Position3 and Transform3.
 * 
 * 2     96/06/20 17:13 Speter
 * Converted into templates and made necessary changes.
 * 
 * 1     96/06/20 15:26 Speter
 * First version of new optimised transform library.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_ROTATE_HPP
#define HEADER_LIB_TRANSFORM_ROTATE_HPP

#include <math.h>

#include "Matrix.hpp"
#include "Translate.hpp"
#include "Lib/Math/FastTrig.hpp"

//
// A version flag to enable counting of quaternion multiplications.
// Set to 1 if we ever want to revisit quaternion renormalisation methods.
// (For now, quaternions are checked for denormalisation after every multiply.)
//
#define VER_QUATERNION_COUNT	VER_DEBUG

// Threshold at which we renormalise the quaternion.
#define fMAX_QUAT_DENORMALISATION	(fMAX_VECTOR_DENORMALISATION * 0.1)

//**********************************************************************************************
//
template<class TR = TReal> struct SFrame3
//
// Prefix: fr3
//
//**************************************
{
	CDir3<TR>	d3Z, d3Y;			// The directions corresponding to the Z and Y axes.

	SFrame3()
		: d3Z(0,0,1), d3Y(0,1,0)
	{
	}

	SFrame3(const CDir3<TR>& d3_z, const CDir3<TR>& d3_y)
		: d3Z(d3_z), d3Y(d3_y)
	{
		// These directions must be perpendicular!
		Assert(Fuzzy(d3Z * d3Y) == 0);
	}
};


template <class TR = TReal>
class CQuaternion
{
public:
	TR tE0;		// Same as tC in CRotate
	TR tE1;		// X
	TR tE2;		// Y 
	TR tE3;		// Z


	CQuaternion(float e0, float e1, float e2, float e3) : tE0(e0), tE1(e1), tE2(e2), tE3(e3)
	{};

	CQuaternion()
	{};
//	CPlacement3(const CRotate3<TR>& r3, const CVector3<TR>& v3)

};



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
	// Several static variables for statistics gathering.
	static int		iMaxNormalisationCount, iAvgNormalisationCount, iMaxCheckCount;
	static float	fTotalNormalisationCount, fTotalNormalisations;
	static int		iMaxDenormalisationCount;
	static TR		tMaxDenormalisation;
#endif

protected:
	TR				tC;				// Scalar part (cosine of half rotation angle).
	CVector3<TR>	v3S;			// Vector part (sine of half rotation angle times vector).

#if VER_QUATERNION_COUNT
	int				iCount;
#endif

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	//******************************************************************************************
	//
	CRotate3()
	//
	// Identity rotation.
	//
	//**********************************
		: tC((TR)1), v3S((TR)0, (TR)0, (TR)0)
	{
		InitCounter();
	}

	//******************************************************************************************
	//
	CRotate3
	(
		const CQuaternion<>& q
	)
	//**********************************
		: v3S(q.tE1, q.tE2, q.tE3), tC(q.tE0)
	{
		Assert(bIsNormalised());
	}
	
	//******************************************************************************************
	//
	CRotate3<TR>(TR t_w, const CVector3<TR>& v3)
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
	CRotate3<TR>
	(
		TR t_w, TR t_x, TR t_y, TR t_z
#if VER_QUATERNION_COUNT
		,
	 	int i_count
#endif
	)
	//
	// Initialise quaternion components even more directly.
	// Invoked by quaternion multiplication operator.
	// Therefore, we do renormalisation here.
	//
	//**********************************
		: tC(t_w), v3S(t_x, t_y, t_z)
#if VER_QUATERNION_COUNT
		,
		iCount(i_count)
#endif
	{
		Normalise();
#if VER_DEBUG
		TR t = Abs(tLenSqr() - (TR)1);
		if (t > tMaxDenormalisation)
		{
			tMaxDenormalisation = t;
			iMaxDenormalisationCount = iCount;
		}
#endif
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
		const SFrame3<TR>& fr3_from,	// Starting frame.
		const SFrame3<TR>& fr3_to		// Ending frame.
	);
	//
	// Constructs a rotation that moves the two axes in fr3_from to the corresponding axes
	// in fr3_to.  Unlike the constructor taking two directions, we do not have to choose an
	// arbitrary (great circle) rotation.  This rotation is unique.
	//
	//**********************************

	//******************************************************************************************
	CRotate3
	(
		const CMatrix3<TR>& mx3			// A transformation matrix.
	);
	//
	// Constructs a rotation from a matrix.
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
		TR t_cc = tC     * tC;
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
			2 * (t_cc + t_xx - 0.5),  2 * (t_xy + t_cz      ),  2 * (t_xz - t_cy      ),
			2 * (t_xy - t_cz      ),  2 * (t_cc + t_yy - 0.5),  2 * (t_yz + t_cx      ),
			2 * (t_xz + t_cy      ),  2 * (t_yz - t_cx      ),  2 * (t_cc + t_zz - 0.5)
		);
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Return the inverse of the rotate.
	CRotate3<TR> operator ~() const
	{
		// For unit quaternions, the inverse is equal to the conjugate (negated vector part).
		return CRotate3<TR>(tC, -v3S);
	}

	//
	// Concatenate with another rotation transform.
	//

	CRotate3<TR> operator *(const CRotate3<TR>& r3) const;

	CRotate3<TR>& operator *=(const CRotate3<TR>& r3)
	{
		return *this = *this * r3;
	}

	//
	// Interpolate rotation by a parameter.  Need not be between 0 and 1.
	// Note: For interpolations in a loop, this is slow:
	//
	//		CRotate3<> rot_total;
	//		CDir3<> d3_orig, d3_new;
	//		for (float f = 0; f < 1; f += 0.1)
	//			d3_new = d3_orig * (rot_total * f);
	//
	// and this is fast:
	//
	//		CRotate3<> rot_total;
	//		CDir3<> d3_orig;
	//		CDir3<> d3_new = d3_orig;
	//		CRotate3<> d3_delta = d3_orig * 0.1;
	//
	//		for (int i = 0; i < 10; i++)
	//			d3_new *= d3_delta;
	//
	CRotate3<TR> operator *(TR r_scale);

	CRotate3<TR>& operator *=(TR r_scale)
	{
		return *this = *this * r_scale;
	}

	//
	// Operate on a vector.
	//

	friend CVector3<TR> operator *(const CVector3<TR>& v3, const CRotate3<TR>& r3);

protected:

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	TR tLenSqr() const
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
		return (TR) sqrt((double) tLenSqr());
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
#if VER_DEBUG
		SetMax(iMaxCheckCount, iCount);
#endif
		TR t_lensqr = tLenSqr();

		// If requested, test whether already approximately normalised.
		if (!b_always && bIsNormalised())
			return;
#if VER_DEBUG
		SetMax(iMaxNormalisationCount, iCount);
		fTotalNormalisationCount += iCount;
		fTotalNormalisations ++;
		iAvgNormalisationCount = (int) (fTotalNormalisationCount / fTotalNormalisations);
#endif
#if VER_QUATERNION_COUNT
		iCount = 0;
#endif
		//
		// We want to use a *particular* sqrt approximation, one
		// appropriate to values very close to 1: 
		//
		//		sqrt(x) ~= (x+1)/2.
		//		1/sqrt(x) ~= 2/(x+1).
		//
		Assert(Fuzzy(t_lensqr) != 0);
		TR t_invlen = (TR)2 / (t_lensqr + (TR)1);
		Assert(Fuzzy(t_invlen * t_invlen) == (TR)1 / t_lensqr);

		tC  *= t_invlen;
		v3S *= t_invlen;

		Assert(bIsNormalised());
	}

	//******************************************************************************************
	//
	void InitCounter()
	{
#if VER_QUATERNION_COUNT
		iCount = 0;
#endif
	}
	//
	//**********************************


	//******************************************************************************************
	//
	void BumpCounter()
	{
#if VER_QUATERNION_COUNT
		iCount++;
#endif
	}
	//
	//**********************************


public:
	//******************************************************************************************
	//
	void GetQuaternion
	(
		CQuaternion<TR> *pq
	)
	//
	//**********************************
	{
		pq->tE1 = v3S.tX;
		pq->tE2 = v3S.tY;
		pq->tE3 = v3S.tZ;
		pq->tE0 = tC;
	}
};

//**********************************************************************************************
//
// Global operators for CRotate3<>.
//

	template<class TR> inline CVector3<TR>& operator *=(CVector3<TR>& v3, const CRotate3<TR>& r3)
	{
		return v3 = v3 * r3;
	}

	//
	// Define transformation of CDir3 as well, to return CDir3.
	//

	template<class TR> CDir3<TR> operator *(const CDir3<TR>& d3, const CRotate3<TR>& r3)
	{
		// Transform dir like a vector, then copy to a direction, bypassing renormalisation.
		return CDir3<TR>((CVector3<TR>&)d3 * r3, true);
	}

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

	CPlacement3()
		: v3Pos(0, 0, 0)
	{
	}

	//
	// Provide constructors for all combinations of rotation and translation.
	//

	CPlacement3(const CRotate3<TR>& r3, const CVector3<TR>& v3)
		: r3Rot(r3), v3Pos(v3)
	{
	}

	CPlacement3(const CRotate3<TR>& r3)
		: r3Rot(r3), v3Pos(0, 0, 0)
	{
	}

	CPlacement3(const CVector3<TR>& v3)
		: v3Pos(v3)
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//


	//******************************************************************************************
	//
	void SetOrigin
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
	void AdjustOrigin
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

	CPlacement3<TR> operator *(const CPlacement3<TR>& p3) const
	{
		return CPlacement3<TR>(r3Rot * p3.r3Rot, v3Pos * p3);
	}

	CPlacement3<TR>& operator *=(const CPlacement3<TR>& p3)
	{
		// Concatenate base transform and translation separately.
		r3Rot *= p3.r3Rot;
		v3Pos *= p3;
		return *this;
	}

	//
	// Concatenate with simple rotation of same type.
	//

	CPlacement3<TR> operator *(const CRotate3<TR>& r3) const
	{
		return CPlacement3<TR>(r3Rot * r3, v3Pos * r3);
	}

	CPlacement3<TR>& operator *=(const CRotate3<TR>& r3)
	{
		// Concatenate base transform and translation separately.
		r3Rot *= r3;
		v3Pos *= r3;
		return *this;
	}

	//
	// Concatenate with a translation.
	//

	CPlacement3<TR> operator *(const CTranslate3<TR>& tl3) const
	{
		return CPlacement3<TR>(r3Rot, v3Pos + tl3.v3Pos);
	}

	CPlacement3<TR>& operator *=(const CTranslate3<TR>& tl3)
	{
		v3Pos += tl3.v3Pos;
		return *this;
	}
};


//**********************************************************************************************
//
// Global operators for CRotate3<>.
//

	template<class TR> inline CPlacement3<TR> operator *(const CRotate3<TR>& r3, const CPlacement3<TR>& p3)
	{
		return CPlacement3<TR>(r3 * p3.r3Rot, p3.v3Pos);
	}

	template<class TR> inline CPlacement3<TR> operator *(const CTranslate3<TR>& tl3, const CPlacement3<TR>& p3)
	{
		return CPlacement3<TR>(p3.r3Rot, tl3.v3Pos * p3);
	}

	//
	// Combine a rotation and translation into a placement.
	//

	template<class TR> inline CPlacement3<TR> operator *(const CRotate3<TR>& r3, const CTranslate3<TR>& tl3)
	{
		return CPlacement3<TR>(r3, tl3.v3Pos);
	}

	template<class TR> inline CPlacement3<TR> operator *(const CTranslate3<TR>& tl3, const CRotate3<TR>& r3)
	{
		return CPlacement3<TR>(r3, tl3.v3Pos * r3);
	}

	//
	// Operate on a vector.
	//

	template<class TR> inline CVector3<TR> operator *(const CVector3<TR>& v3, const CPlacement3<TR>& p3)
	{
		// Perform the transform, then add the translation.
		return v3 * p3.r3Rot + p3.v3Pos;
	}

	template<class TR> inline CVector3<TR>& operator *=(CVector3<TR>& v3, const CPlacement3<TR>& p3)
	{
		v3 *= p3.r3Rot;
		v3 += p3.v3Pos;
		return v3;
	}

	//
	// Operate on a direction by skipping the translation step.
	//

	template<class TR> inline CDir3<TR> operator *(const CDir3<TR>& d3, const CPlacement3<TR>& p3)
	{
		return d3 * p3.r3Rot;
	}

	template<class TR> inline CDir3<TR>& operator *=(CDir3<TR>& d3, const CPlacement3<TR>& p3)
	{
		return d3 *= p3.r3Rot;
	}

//**********************************************************************************************
//
// Global functions for CRotate3<>.
//

	//******************************************************************************************
	//
	template<class TR> inline CPlacement3<TR> TransformAt(const CRotate3<TR>& r3, const CVector3<TR>& v3_at)
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

//******************************************************************************************
//
// We must include all the implementation code because this is a template class.
//

#include "Rotate.cpp"


#endif
