/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Main include file for transform library.
 *		Includes all necessary files.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/Transform.hpp                                          $
 * 
 * 9     96/12/04 20:20 Speter
 * Added include of "Presence.hpp".
 * 
 * 8     96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 7     96/08/21 17:38 Speter
 * Changes from code review:
 * Added default template parameter to all classes.
 * Made all in-class friends inline.
 * Updated comments.
 * 
 * 6     96/07/08 12:40 Speter
 * Changed name of CNormal3 to CDir3 (more general).
 * Added specific functions to transform directions, which do not translate positions.
 * 
 * 5     96/06/26 22:07 Speter
 * Added a bunch of comments, prettied things up nicely.
 * 
 * 4     96/06/26 13:16 Speter
 * Changed TGReal to TR and prefix gr to r.
 * 
 * 3     96/06/25 14:36 Speter
 * Finalised design of transform classes, with Position3 and Transform3.
 * 
 * 2     96/06/20 17:13 Speter
 * Converted into templates and made necessary changes.
 * 
 * 1     96/06/20 15:26 Speter
 * First version of new optimised transform library.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_TRANSFORM_HPP
#define HEADER_LIB_TRANSFORM_TRANSFORM_HPP

//******************************************************************************************
//
//	This header includes all headers needed for the transform subsystem.
//
//	This subsystem consists of several classes, each templatised on the basic value parameter
//	(which can be float, fixed, etc.).
//
//   There are vector classes and transform classes.  The vector classes are:
//
//		CVector2				Basic 2D vector.
//			CDir2					Subclass constrained to hold unit vector.
//		CVector3				Basic 3D vector.
//			CDir3					Subclass constrained to hold unit vector.
//
//	The transforms are classes which operate on vectors, transforming them into different vectors.
//	Each transform implements the following interface (although there is in general no inheritance
//	relationship between them):
//

/*
//**********************************************************************************************
//
template<class TR = TReal> class CExample3
//
// Prefix: ex3
//
// This is not a class declaration, but merely an example of the minimum interface
// all transforms implement.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	// The default constructor is an identity transform.
	CExample3();

	// There will be other constructors.
	// ...

	//******************************************************************************************
	//
	// Conversion operators.
	//

	// Supply conversions to more general transforms (see hierarchy below).
	// ...

	//******************************************************************************************
	//
	// Operators.
	//

	// Return the inverse of the transform.
	CExample3<TR> operator ~() const;

	// Concatenate with another transform of same type.
	friend CExample3<TR> operator *(const CExample3<TR>& tf3_a, const CExample3<TR>& tf3_b);

	CExample3<TR>& operator *=(const CExample3<TR>& t3);

	// Concatenate with other transforms if appropriate.
	// ...

	// Operate on a vector.
	friend CVector3 operator *(const CVector3& v3, const CExample3<TR>& t3);

	friend CVector3& operator *=(CVector3& v3, const CExample3<TR>& t3);

	// Operate on a direction.  (Not supported by Scale or Shear.)

	friend CDir3 operator *(const CDir3& d3, const CExample3<TR>& t3);

	friend CDir3& operator *=(CDir3& d3, const CExample3<TR>& t3);
};
*/

//
//	Here is a list of the specific transform classes, along with the conversion and concatenation 
//	operators they contain (other than self-concatenation).
//
//		CTranslate3				Adds a constant translation to a vector.  
//
//		CMatrix3				Performs any non-translating transformation (scale, rotate, shear).
//
//		CTransform3				General linear 3D transform: any rotation, scale, shear,
//								translation, etc.  
//								Contains a CMatrix3 plus CVector3 translation.
//			CTransform3(CMatrix3)
//			CTransform3(CTranslate3)
//
//			CTransform3 = CTransform3 * CMatrix3
//			CTransform3 *= CMatrix3
//			CTransform3 = CMatrix3 * CTransform3
//
//			CTransform3 = CTransform3 * CTranslate3
//			CTransform3 *= CTranslate3
//			CTransform3 = CTranslate3 * CTransform3
//
//			CTransform3 = CMatrix3 * CTranslate3
//			CTransform3 = CTranslate3 * CMatrix3
//
//		CScale3					Scales by different values in x, y, and z.
//			operator CMatrix3()
//			CMatrix3 = CScale3 * CMatrix3
//			CMatrix3 = CMatrix3 * CScale3
//			CMatrix3 *= CScale3
//
//			CTransform3 = CScale3 * CTransform3
//			CTransform3 = CTransform3 * CScale3
//			CTransform3 *= CScale3
//
//			CTransform3 = CScale3 * CTranslate3
//			CTransform3 = CTranslate3 * CScale3
//
//		CScaleI3				Scales by a constant amount.
//			operator CMatrix3()
//			CMatrix3 = CScaleI3 * CMatrix3
//			CMatrix3 = CMatrix3 * CScaleI3
//			CMatrix3 *= CScaleI3
//
//			CTransform3 = CScaleI3 * CTransform3
//			CTransform3 = CTransform3 * CScaleI3
//			CTransform3 *= CScaleI3
//
//			CTransform3 = CScaleI3 * CTranslate3
//			CTransform3 = CTranslate3 * CScaleI3
//
//			operator CScale3()
//			CScale3 = CScaleI3 * CScale3
//			CScale3 = CScale3 * CScaleI3
//			CScale3 *= CScaleI3
//
//		CShear3					Shears any two axes with respect to the third.
//								Implemented as just a #define to CMatrix3, using a special
//								CMatrix3() constructor.  This is because shear is used too
//								infrequently to bother making an optimised class.
//
//		CRotate3				Performs a rotation.
//								Implemented as a quaternion.
//			operator CMatrix3()
//			CMatrix3 = CRotate3 * CMatrix3
//			CMatrix3 = CMatrix3 * CRotate3
//			CMatrix3 *= CRotate3
//
//		CPlacement3				Performs any rigid transformation, i.e. rotation and translation.
//								Indicates an objects placement (position and orientation) in a 
//								coordinate system.  Contains a CRotate3 and CVector3 translation.
//			CPlacement3(CRotate3)
//			CPlacement3(CTranslate3)
//
//			operator CTransform3();
//
//			CPlacement3 = CPlacement3 * CRotate3
//			CPlacement3 *= CRotate3
//			CPlacement3 = CRotate3 * CPlacement3
//
//			CPlacement3 = CPlacement3 * CTranslate3
//			CPlacement3 *= CTranslate3
//			CPlacement3 = CTranslate3 * CPlacement3
//
//			CPlacement3 = CRotate3 * CTranslate3
//			CPlacement3 = CTranslate3 * CRotate3
//


//
// Include all files necessary for transform subsystem.
//

#include "Vector.hpp"
#include "Translate.hpp"
#include "Matrix.hpp"
#include "Rotate.hpp"
#include "Scale.hpp"
#include "Presence.hpp"
#include "Shear.hpp"

//
// Declare concatenation operators between different transform types to resolve ambiguities.
//
// Compiler bug: these functions should not be needed.  Since there is a conversion from
// CRotate3 to CMatrix3, the compiler is supposed to figure out that an expression like 
// CMatrix3 * CRotate3 should resolve to CMatrix3 * CMatrix3(CRotate3).
// 
// However, when we are dealing with templates, this fails.  (It works for non-templated code.)
//

//
// CRotate3 with CMatrix3.
//

template<class TR> CMatrix3<TR> operator *(const CRotate3<TR>& r3, const CMatrix3<TR>& mx3)
{
	return (CMatrix3<TR>)r3 * mx3;
}

template<class TR> CMatrix3<TR> operator *(const CMatrix3<TR>& mx3, const CRotate3<TR>& r3)
{
	return mx3 * (CMatrix3<TR>)r3;
}

template<class TR> CMatrix3<TR>& operator *=(CMatrix3<TR>& mx3, const CRotate3<TR>& r3)
{
	return mx3 *= (CMatrix3<TR>)r3;
}

//
// CRotate3 with CScale3.
//

template<class TR> CMatrix3<TR> operator *(const CRotate3<TR>& r3, const CScale3<TR>& s3)
{
	return CMatrix3<TR>(r3) * s3;
}

template<class TR> CMatrix3<TR> operator *(const CScale3<TR>& s3, const CRotate3<TR>& r3)
{
	return s3 * CMatrix3<TR>(r3);
}

//
// CRotate3 with CScaleI3.
//

template<class TR> CMatrix3<TR> operator *(const CRotate3<TR>& r3, const CScaleI3<TR>& si3)
{
	return CMatrix3<TR>(r3) * si3;
}

template<class TR> CMatrix3<TR> operator *(const CScaleI3<TR>& si3, const CRotate3<TR>& r3)
{
	return si3 * CMatrix3<TR>(r3);
}


#endif
