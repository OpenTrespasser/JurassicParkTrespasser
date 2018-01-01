/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CTranslate3<TR>
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform5/Translate.hpp                                         $
 * 
 * 11    97-04-24 18:40 Speter
 * Folded new changes from 4.2 version into this 5.0 specific version.
 * 
 * 9     96/12/04 20:18 Speter
 * Changed v3T to v3Pos in all transforms.
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
 * 7     96/08/21 17:50 Speter
 * Changes from code review:
 * Added default template parameter to all classes.
 * Made all in-class friends inline.
 * Added comments.
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

#ifndef HEADER_LIB_TRANSFORM_TRANSLATE_HPP
#define HEADER_LIB_TRANSFORM_TRANSLATE_HPP

#include "Vector.hpp"

//**********************************************************************************************
//
template<class TR = TReal> class CTranslate3
//
// Prefix: tl3
//
// A 3D translation transform.
//
// Note: This is not inherited from CVector3, because a translation behaves like other transforms,
// not like a vector.  For example, CTranslate3 * CVector3 should transform (i.e. translate) the
// vector, whereas CVector3 * CVector3 is the dot product.
//
//**************************************
{
public:
	CVector3<TR>	v3Pos;

public:
	//******************************************************************************************
	//
	// Constructors.
	//

	// Identity transform.
	CTranslate3()
		: v3Pos((TR)0, (TR)0, (TR)0)
	{
	}

	CTranslate3(const CVector3<TR>& v3)
		: v3Pos(v3)
	{
	}

	CTranslate3(TR t_x, TR t_y, TR t_z)
		: v3Pos(t_x, t_y, t_z)
	{
	}

	//******************************************************************************************
	//
	// Conversion operators.
	//

	//
	// CTranslate3 does not convert to any other transform.
	//

	//******************************************************************************************
	//
	// Operators.
	//

	// The inverse is just the negative.
	CTranslate3<TR> operator ~() const
	{
		return CTranslate3<TR>(-v3Pos);
	}

	// Concatenation is just addition.

	CTranslate3<TR> operator *(const CTranslate3<TR>& tl3) const
	{
		return CTranslate3<TR>(v3Pos + tl3.v3Pos);
	}

	CTranslate3<TR>& operator *=(const CTranslate3<TR>& tl3)
	{
		v3Pos += tl3.v3Pos;
		return *this;
	}

};

//******************************************************************************************
//
// Global operators for CTranslate3<>.
//

	// Transformation of a vector is just addition.

	template<class TR> inline CVector3<TR> operator *(const CVector3<TR>& v3, const CTranslate3<TR>& tl3)
	{
		return CVector3<TR>(v3 + tl3.v3Pos);
	}

	template<class TR> inline CVector3<TR>& operator *=(CVector3<TR>& v3, const CTranslate3<TR>& tl3)
	{
		return v3 += tl3.v3Pos;
	}

	// Transformation of a direction, however, is a null operation.

	template<class TR> inline CDir3<TR> operator *(const CDir3<TR>& d3, const CTranslate3<TR>& tl3)
	{
		return d3;
	}

	template<class TR> inline CDir3<TR>& operator *=(CDir3<TR>& d3, const CTranslate3<TR>& tl3)
	{
		return d3;
	}

#endif
