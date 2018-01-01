/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CTransLinear<>
 *		CTransLinear2<>
 *		CRectangle<>
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/TransLinear.hpp                                         $
 * 
 * 15    98/02/10 18:43 Speter
 * Added rcFlipX() and rcFlipY() members.
 * 
 * 14    9/08/97 12:01p Mlange
 * Added CRectangle default constructor.
 * 
 * 13    8/28/97 4:03p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 14    97/08/22 6:40p Pkeet
 * Added a forward declaration.
 * 
 * 13    97/08/22 13:14 Speter
 * Fixes to make sure CTINVisualise drawing code works with new TIN position.
 * 
 * 12    97/08/20 2:53 Speter
 * Fixed bug by casting 0 to (TR) in CRectangle function.
 * 
 * 11    97/08/19 22:01 Speter
 * Added CTransLinear direct value constructor.  Changed CRectangle<> operator & to clamp size
 * non-negative.  Added bIntersects().  Made some functions const.  Changed some <> to <TR>.
 * 
 * 10    97/08/14 10:29 Speter
 * Fixed mid-point calculations to work with <int> types.
 * 
 * 9     97/08/12 23:40 Speter
 * Added utility rcSubRect() functions.
 * 
 * 8     97/08/11 12:23 Speter
 * Made CTransLinear use the * operator, like other transforms.  Added composition operator.
 * 
 * 7     97/08/08 15:26 Speter
 * Added operators for finding intersections and unions of rectangles and points.   Added
 * bContains() for rectangles. Added tArea().   Added formatting.
 * 
 * 6     97/08/04 15:09 Speter
 * Unprotected tScale and tOffset.  Made square scaling keep offsets.
 * 
 * 5     97/08/01 15:16 Speter
 * Made functions const.  Added bContains().  Changed tXCentre() to tXMid().  Added conversion
 * function between CRectangle<> template types.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_TRANSLINEAR_HPP
#define HEADER_LIB_TRANSFORM_TRANSLINEAR_HPP

#include "Vector.hpp"


//
// Forward declarations.
//
template<class TR = float> class CTransLinear2;

//**********************************************************************************************
//
template<class TR = float> class CTransLinear
//
// Prefix: tlr
//
// Provides a linear transformation of a single coordinate: scale and offset.
//
//**************************************
{
public:
	TR		tScale, tOffset;			// Scale and offset to apply to input coords.

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	CTransLinear(TR t_scale = 1, TR t_offset = 0)
		: tScale(t_scale), tOffset(t_offset)
	{
	}

	CTransLinear(TR t_source_0, TR t_source_range, TR t_dest_0, TR t_dest_range)
	{
		Assert(t_source_range != 0);
		tScale = t_dest_range / t_source_range;
		tOffset = t_dest_0 - t_source_0 * tScale;
	}

	//******************************************************************************************
	//
	// Operators.
	//

	//******************************************************************************************
	CTransLinear<TR> operator ~() const
	// Return the inverse transformation.
	{
		Assert(tScale != TR(0));
		TR t_scale = TR(1) / tScale;
		return CTransLinear<TR>(t_scale, - tOffset * t_scale);
	}

	//******************************************************************************************
	CTransLinear<TR> operator *(const CTransLinear<TR>& tlr) const
	// Concatenate the transformations.
	{
		return CTransLinear<TR>(tScale * tlr.tScale, tOffset * tlr.tScale + tlr.tOffset);
	}

	//******************************************************************************************
	CTransLinear<TR>& operator *=(const CTransLinear<TR>& tlr)
	// Concatenate the transformations.
	{
		tScale *= tlr.tScale;
		tOffset = tOffset * tlr.tScale + tlr.tOffset;
		return *this;
	}

	friend class CTransLinear2<TR>;
};

	//******************************************************************************************
	//
	// Global operators.
	//

	//******************************************************************************************
	template<class TR> inline TR operator *(TR t, const CTransLinear<TR>& tlr)
	// Apply the linear transformation to the coord.
	{
		return t * tlr.tScale + tlr.tOffset;
	}

	//******************************************************************************************
	template<class TR> inline TR& operator *=(TR& t, const CTransLinear<TR>& tlr)
	// Apply the linear transformation to the coord.
	{
		return t = t * tlr.tScale + tlr.tOffset;
	}

//**********************************************************************************************
//
template<class TR = TReal> class CRectangle
//
// Prefix: rc
//
//**************************************
{
protected:
	CVector2<TR> v2Start_, v2Size_;

public:
	//******************************************************************************************
	//
	// Constructors.
	//
	CRectangle()
	{
	}


	CRectangle(CVector2<TR> v2_start, CVector2<TR> v2_size)
		: v2Start_(v2_start), v2Size_(v2_size)
	{
	}

	CRectangle(TR t_x, TR t_y, TR t_width, TR t_height)
		: v2Start_(t_x, t_y), v2Size_(t_width, t_height)
	{
	}

	CRectangle(CVector2<TR> v2_size)
		: v2Start_(v2_size * -0.5), v2Size_(v2_size)
	{
	}

	CRectangle(TR t_width, TR t_height)
		: v2Start_(-t_width/2, -t_height/2), v2Size_(t_width, t_height)
	{
	}

	//******************************************************************************************
	//
	// Operators.
	//

	//******************************************************************************************
	CRectangle<TR>& operator *=(TR r_scale)
	{
		v2Start_ *= r_scale;
		v2Size_ *= r_scale;
		return *this;
	}

	//******************************************************************************************
	CRectangle<TR> operator *(TR r_scale) const
	{
		return CRectangle<TR>(v2Start_ * r_scale, v2Size_ * r_scale);
	}

	//******************************************************************************************
	CRectangle<TR>& operator +=(CVector2<TR> v2_offset)
	{
		v2Start_ += v2_offset;
		return *this;
	}

	//******************************************************************************************
	CRectangle<TR> operator +(CVector2<TR> v2_offset) const
	{
		return CRectangle<TR>(v2Start_ + v2_offset, v2Size_);
	}

	//******************************************************************************************
	CRectangle<TR>& operator &=(const CRectangle<TR>& rc)
	{
		// Find intersection of the two rectangles.
		CVector2<TR> v2_end = v2End();
		v2_end.SetMin(rc.v2End());

		v2Start_.SetMax(rc.v2Start_);
		v2Size_.tX = Max(TR(0), v2_end.tX - v2Start_.tX);
		v2Size_.tY = Max(TR(0), v2_end.tY - v2Start_.tY);
		return *this;
	}

	//******************************************************************************************
	CRectangle<TR> operator &(const CRectangle<TR>& rc) const
	{
		CRectangle<TR> rc_return = *this;
		return rc_return &= rc;
	}

	//******************************************************************************************
	CRectangle<TR>& operator |=(const CRectangle<TR>& rc)
	{
		// Find union of the two rectangles (rectangle enclosing both).
		if (rc.tX0() < tX0())
		{
			v2Size_.tX += tX0() - rc.tX0();
			v2Start_.tX = rc.tX0();
		}
		if (rc.tX1() > tX1())
			v2Size_.tX = rc.tX1() - v2Start_.tX;

		if (rc.tY0() < tY0())
		{
			v2Size_.tY += tY0() - rc.tY0();
			v2Start_.tY = rc.tY0();
		}
		if (rc.tY1() > tY1())
			v2Size_.tY = rc.tY1() - v2Start_.tY;
		return *this;
	}

	//******************************************************************************************
	CRectangle<TR> operator |(const CRectangle<TR>& rc) const
	{
		CRectangle<TR> rc_return = *this;
		return rc_return |= rc;
	}

	//******************************************************************************************
	CRectangle<TR>& operator |=(const CVector2<TR>& v2)
	// Grows the rectangle if needed to contain the point.
	{
		// Grow the rectangle to contain the point.
		if (v2.tX < tX0())
		{
			v2Size_.tX += tX0() - v2.tX;
			v2Start_.tX = v2.tX;
		}
		else if (v2.tX > tX1())
			v2Size_.tX = v2.tX - v2Start_.tX;

		if (v2.tY < tY0())
		{
			v2Size_.tY += tY0() - v2.tY;
			v2Start_.tY = v2.tY;
		}
		if (v2.tY > tY1())
			v2Size_.tY = v2.tY - v2Start_.tY;
		return *this;
	}

	//******************************************************************************************
	CRectangle<TR> operator |(const CVector2<TR>& v2)
	{
		CRectangle<TR> rc_return = *this;
		return rc_return |= v2;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	bool bContains
	(
		CVector2<TR> v2					// A point to test.
	) const
	//
	// Returns:
	//		Whether the point is contained (inclusively) in the rectangle.
	//
	//**********************************
	{
		return bWithin(v2.tX, tX0(), tX1()) &&
			   bWithin(v2.tY, tY0(), tY1());
	}

	//******************************************************************************************
	//
	bool bContains
	(
		CRectangle<TR> rc				// A rectangle to test.
	) const
	//
	// Returns:
	//		Whether the rectangle is entirely contained (inclusively) in this rectangle.
	//
	//**********************************
	{
		return rc.tX0() >= tX0() && rc.tX1() <= tX1() &&
			   rc.tY0() >= tY0() && rc.tY1() <= tY1();
	}

	//******************************************************************************************
	//
	bool bIntersects
	(
		CRectangle<TR> rc				// A rectangle to test.
	) const
	//
	// Returns:
	//		Whether the rectangle overlaps at all with this rectangle.
	//
	//**********************************
	{
		CRectangle<TR> rc_intersect = *this & rc;
		return rc_intersect.tWidth() > 0 && rc_intersect.tHeight() > 0;
	}

	//******************************************************************************************
	//
	CRectangle<TR> rcSubRect
	(
		CRectangle<TR> rc_rel			// A rectangle relative to this one.
	) const
	//
	// Returns:
	//		A rectangle describing the sub-rectangle in absolute coords.
	//
	// Notes:
	//		Asserts for containment.
	//
	//**********************************
	{
		Assert(bContains(rc_rel));
		return CRectangle<TR>(tX0() + rc_rel.tX0(), tY0() + rc_rel.tY0(), rc_rel.tWidth(), rc_rel.tHeight());
	}

	//******************************************************************************************
	//
	CRectangle<TR> rcSubRect
	(
		TR t_x, TR t_y, TR t_w, TR t_h	// Dimensions of relative rectangle.
	) const
	//
	// Returns:
	//		A rectangle describing the sub-rectangle in absolute coords.
	//
	//**********************************
	{
		return rcSubRect(CRectangle<TR>(t_x, t_y, t_w, t_h));
	}

	//******************************************************************************************
	//
	CRectangle<TR> rcFlipX() const
	//
	// Returns:
	//		A equivalent rectangle with the x axis flipped.
	//
	//**********************************
	{
		return CRectangle<TR>(tX1(), tY0(), -tWidth(), tHeight());
	}

	//******************************************************************************************
	//
	CRectangle<TR> rcFlipY() const
	//
	// Returns:
	//		A equivalent rectangle with the y axis flipped.
	//
	//**********************************
	{
		return CRectangle<TR>(tX0(), tY1(), tWidth(), -tHeight());
	}

	//
	// Access functions.
	//

	const CVector2<TR>& v2Start() const
	{
		return v2Start_;
	}

	CVector2<TR> v2End() const
	{
		return v2Start_ + v2Size_;
	}

	const CVector2<TR>& v2Extent() const
	{
		return v2Size_;
	}

	const CVector2<TR> v2Mid() const
	{
		return CVector2<TR>(tXMid(), tYMid());
	}

	TR tX0() const
	{
		return v2Start_.tX;
	}

	TR tY0() const
	{
		return v2Start_.tY;
	}

	TR tX1() const
	{
		return v2Start_.tX + v2Size_.tX;
	}

	TR tY1() const
	{
		return v2Start_.tY + v2Size_.tY;
	}

	TR tWidth() const
	{
		return v2Size_.tX;
	}

	TR tHeight() const
	{
		return v2Size_.tY;
	}

	TR tXMid() const
	{
		return v2Start_.tX + v2Size_.tX / 2;
	}

	TR tYMid() const
	{
		return v2Start_.tY + v2Size_.tY / 2;
	}

	TR tArea() const
	{
		return v2Size_.tX * v2Size_.tY;
	}
};

	//**********************************************************************************************
	//
	// Global functions for CRectangle<TR>.
	//

	//**********************************************************************************************
	//
	template<class S, class D> inline CRectangle<D> rcConvert
	(
		D,								// Dummy parameter indicating type of returned rectangle.
		const CRectangle<S>& rcs		// Source rectangle to convert.
	)
	//
	// Returns:
	//		An equivalent rectangle of type CRectangle<D>.
	//
	//**********************************
	{
		return CRectangle<D>(rcs.tX0(), rcs.tY0(), rcs.tWidth(), rcs.tHeight());
	}

//**********************************************************************************************
//
template<class TR = float> class CTransLinear2
//
// Prefix: tlr2
//
// Groups linear transformations for 2 independent coordinates, thus defining a viewport.
//
//**************************************
{
public:
	CTransLinear<TR>	tlrX, tlrY;		// Translations for individual coordinates.
										// May by publicly accessed individually, 
										// or via operator functions taking CVector2<TR>.

	//******************************************************************************************
	//
	// Constructors.
	//

	CTransLinear2()
	{
	}

	CTransLinear2(CTransLinear<TR> tlr_x, CTransLinear<TR> tlr_y)
		: tlrX(tlr_x), tlrY(tlr_y)
	{
	}

	CTransLinear2(const CRectangle<TR>& rc_source, const CRectangle<TR>& rc_dest, bool b_square_scale = false)
		: tlrX(rc_source.tX0(), rc_source.tWidth(),  rc_dest.tX0(), rc_dest.tWidth()),
		  tlrY(rc_source.tY0(), rc_source.tHeight(), rc_dest.tY0(), rc_dest.tHeight())
	{
		if (b_square_scale)
		{
			// Set both scales to the minimum of the two, but preserve the signs.
			// Also keep the same offset.
			if (Abs(tlrX.tScale) < Abs(tlrY.tScale))
			{
				TR t_scale = Abs(tlrX.tScale / tlrY.tScale);
				tlrY.tOffset *= t_scale;
				tlrY.tScale *= t_scale;
			}
			else
			{
				TR t_scale = Abs(tlrY.tScale / tlrX.tScale);
				tlrX.tOffset *= t_scale;
				tlrX.tScale *= t_scale;
			}
		}
	}

	//******************************************************************************************
	//
	// Operators.
	//

	//******************************************************************************************
	CTransLinear2<TR> operator ~() const
	{
		return CTransLinear2<TR>(~tlrX, ~tlrY);
	}

	//******************************************************************************************
	CTransLinear2<TR> operator *(const CTransLinear2<TR>& tlr2) const
	// Concatenate the transformations.
	{
		return CTransLinear2<TR>(tlrX * tlr2.tlrX, tlrY * tlr2.tlrY);
	}

	//******************************************************************************************
	CTransLinear2<TR>& operator *=(const CTransLinear2<TR>& tlr2)
	// Concatenate the transformations.
	{
		tlrX *= tlr2.tlrX;
		tlrY *= tlr2.tlrY;
		return *this;
	}
};

	//******************************************************************************************
	//
	// Global operators.
	//

	//******************************************************************************************
	template<class TR> inline CVector2<TR> operator *(const CVector2<TR>& v2, const CTransLinear2<TR>& tlr2)
	// Apply the linear transformation to the vector.
	{
		return CVector2<TR>(v2.tX * tlr2.tlrX, v2.tY * tlr2.tlrY);
	}

	//******************************************************************************************
	template<class TR> inline CVector2<TR>& operator *=(CVector2<TR>& v2, const CTransLinear2<TR>& tlr2)
	// Apply the linear transformation to the vector.
	{
		v2.tX *= tlr2.tlrX;
		v2.tY *= tlr2.tlrY;
		return v2;
	}

	//******************************************************************************************
	template<class TR> inline CRectangle<TR> operator *(const CRectangle<TR>& rc, const CTransLinear2<TR>& tlr2)
	// Apply the linear transformation to the rectangle.
	{
		return CRectangle<TR>
		(
			rc.tX0() * tlr2.tlrX, rc.tY0() * tlr2.tlrY,						// Position linearly transformed.
			rc.tWidth() * tlr2.tlrX.tScale, rc.tHeight() * tlr2.tlrY.tScale	// Size scaled only.
		);
	}

	//******************************************************************************************
	template<class TR> inline CRectangle<TR>& operator *=(CRectangle<TR>& rc, const CTransLinear2<TR>& tlr2)
	// Apply the linear transformation to the rectangle.
	{
		return rc = rc * tlr2;
	}


template<class X> class BecauseOfMicrosoftIMustPutThisHere {};

#endif

