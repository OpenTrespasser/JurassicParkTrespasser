/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Helper class for testing intersections with boxes. Contains the following classes:
 *
 *			CExtents		Contains minimum and maximum vector values.
 *			CLineSegment	Describes a 3D line segment.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/GeomLines.hpp                                            $
 * 
 * 1     97/03/31 6:04p Pkeet
 * Initial implementation.
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_GEOMLINES_HPP
#define HEADER_LIB_RENDERER_GEOMLINES_HPP


//**********************************************************************************************
//
class CExtents
//
// Describes the minimum and maximum values for 3D vectors.
//
// Prefix: ext
//
// Notes:
//		Note that the two points store represent the diagonally opposite corners of an axis
//		aligned box.
//
//**************************************
{
public:

	CVector3<> v3Min;	// Minimum values for the three axis's.
	CVector3<> v3Max;	// Maximum values for the three axis's.

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CExtents()
	{
	}

	// Constructor with intial values.
	CExtents(const CVector3<>& v3_a, const CVector3<>& v3_b)
	{
		// Set min and max values for X.
		if (v3_a.tX < v3_b.tX)
		{
			v3Min.tX = v3_a.tX;
			v3Max.tX = v3_b.tX;
		}
		else
		{
			v3Min.tX = v3_b.tX;
			v3Max.tX = v3_a.tX;
		}
		
		// Set min and max values for Y.
		if (v3_a.tY < v3_b.tY)
		{
			v3Min.tY = v3_a.tY;
			v3Max.tY = v3_b.tY;
		}
		else
		{
			v3Min.tY = v3_b.tY;
			v3Max.tY = v3_a.tY;
		}

		// Set min and max values for Z.
		if (v3_a.tZ < v3_b.tZ)
		{
			v3Min.tZ = v3_a.tZ;
			v3Max.tZ = v3_b.tZ;
		}
		else
		{
			v3Min.tZ = v3_b.tZ;
			v3Max.tZ = v3_a.tZ;
		};
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	bool bWithin
	(
		const CVector3<>& v3	// Specified point.
	) const
	//
	// Returns 'true' if the specified point is within.
	//
	//**************************************
	{
		// X axis.
		if (v3.tX <= v3Min.tX)
			return false;
		if (v3.tX >= v3Max.tX)
			return false;
		
		// Y axis.
		if (v3.tY <= v3Min.tY)
			return false;
		if (v3.tY >= v3Max.tY)
			return false;

		// X axis.
		if (v3.tZ <= v3Min.tZ)
			return false;
		if (v3.tZ >= v3Max.tZ)
			return false;

		// Success.
		return true;
	}

	//******************************************************************************************
	//
	bool bTrivialReject
	(
		const CExtents& ext	// Specified extent to test with.
	) const
	//
	// Returns 'true' if the specified extent can be trivially rejected. This is equivalent
	// to testing for intersection of two axis-aligned boxes (see the notes associate with
	// the class description.
	//
	//**************************************
	{
		// X axis.
		if (ext.v3Min.tX > v3Max.tX)
			return true;
		if (ext.v3Max.tX < v3Min.tX)
			return true;

		// X axis.
		if (ext.v3Min.tY > v3Max.tY)
			return true;
		if (ext.v3Max.tY < v3Min.tY)
			return true;

		// X axis.
		if (ext.v3Min.tZ > v3Max.tZ)
			return true;
		if (ext.v3Max.tZ < v3Min.tZ)
			return true;

		return false;
	}

	//******************************************************************************************
	//
	bool bWithinX
	(
		TReal r_x	// Value to test for.
	) const
	//
	// Returns 'true' if the specified value is within the minimum and maximum X extents.
	//
	//**************************************
	{
		return ::bWithin(r_x, v3Min.tX, v3Max.tX);
	}

	//******************************************************************************************
	//
	bool bWithinY
	(
		TReal r_y	// Value to test for.
	) const
	//
	// Returns 'true' if the specified value is within the minimum and maximum Y extents.
	//
	//**************************************
	{
		return ::bWithin(r_y, v3Min.tY, v3Max.tY);
	}

	//******************************************************************************************
	//
	bool bWithinZ
	(
		TReal r_z	// Value to test for.
	) const
	//
	// Returns 'true' if the specified value is within the minimum and maximum Z extents.
	//
	//**************************************
	{
		return ::bWithin(r_z, v3Min.tZ, v3Max.tZ);
	}

};


//**********************************************************************************************
//
class CLineSegment : public CExtents
//
// Describes a 3D line segment.
//
// Prefix: lseg
//
//**************************************
{
public:

	CVector3<> v3From;	// Starting point of the line segment.
	CVector3<> v3To;	// End point.
	CVector3<> v3Delta;	// Deltas for x, y and z between the start and end points.

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	// Constructor.
	CLineSegment
	(
		const CVector3<>& v3_from,	// Starting point of the line segment.
		const CVector3<>& v3_to		// End point of the line segment.
	)
		: CExtents(v3_from, v3_to)
	{
		v3From  = v3_from;
		v3To    = v3_to;

		// Calculate the offset between the start and end points.
		v3Delta = v3_to - v3_from;
	}

};


#endif // HEADER_LIB_RENDERER_GEOMLINES_HPP
