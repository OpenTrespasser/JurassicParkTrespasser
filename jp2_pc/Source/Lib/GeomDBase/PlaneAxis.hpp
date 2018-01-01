/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/PlaneAxis.hpp                                           $
 * 
 * 4     97/08/28 18:30 Speter
 * CPlaneDefAxis no longer wastefully inherited from CPlaneDef.  Implemented
 * CPlaneDef::rDistanceRel() function
 * 
 * 3     97/06/30 19:19 Speter
 * Made rPos public.
 * 
 * 2     97/05/23 6:24p Pkeet
 * Untemplatized CPlaneDef.
 * 
 * 1     97-04-21 17:14 Speter
 * New file containing axis-aligned plane definitions.
 * 
 * 1     97-04-14 20:41 Speter
 * Contains code moved from GeomTypes.hpp.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_PLANEAXIS_HPP
#define HEADER_LIB_GEOMDBASE_PLANEAXIS_HPP

#include "Plane.hpp"

//
// Plane classes specialised for simple cases.
//

//**********************************************************************************************
//
class CPlaneDefAxis
//
// Prefix: pldax
//
// Base class for any axis-aligned plane.
//
//**************************************
{
public:
	TReal	rPos;						// Position on the given axis.

	//******************************************************************************************
	//
	// Constructors
	//

	CPlaneDefAxis(TReal r_pos = 0)
		: rPos(r_pos)
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	TReal rTolerance(const CVector3<>& v3) const
	{
		return fPLANE_TOLERANCE;
	}

	//******************************************************************************************
	void operator *=(TReal r_scale)
	{
		rPos *= r_scale;
	}
};

//
// Specific axis-aligned plane classes.
//

//**********************************************************************************************
//
class CPlaneDefPosX: public CPlaneDefAxis
//
//**************************************
{
public:
	CPlaneDefPosX(TReal r_pos = 0)
		: CPlaneDefAxis(r_pos)
	{
	}

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		return v3.tX - rPos;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return v3_rel.tX;
	}

	//******************************************************************************************
	operator CPlaneDef() const
	{
		return CPlaneDef(d3XAxis, -rPos);
	}
};


//**********************************************************************************************
//
class CPlaneDefNegX: public CPlaneDefAxis
//
//**************************************
{
public:
	CPlaneDefNegX(TReal r_pos = 0)
		: CPlaneDefAxis(r_pos)
	{
	}

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		return rPos - v3.tX;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return -v3_rel.tX;
	}

	//******************************************************************************************
	operator CPlaneDef() const
	{
		return CPlaneDef(-d3XAxis, rPos);
	}
};



//**********************************************************************************************
//
class CPlaneDefPosY: public CPlaneDefAxis
//
//**************************************
{
public:
	CPlaneDefPosY(TReal r_pos = 0)
		: CPlaneDefAxis(r_pos)
	{
	}

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		return v3.tY - rPos;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return v3_rel.tY;
	}

	//******************************************************************************************
	operator CPlaneDef() const
	{
		return CPlaneDef(d3YAxis, -rPos);
	}
};



//**********************************************************************************************
//
class CPlaneDefNegY: public CPlaneDefAxis
//
//**************************************
{
public:
	CPlaneDefNegY(TReal r_pos = 0)
		: CPlaneDefAxis(r_pos)
	{
	}

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		return rPos - v3.tY;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return -v3_rel.tY;
	}

	//******************************************************************************************
	operator CPlaneDef() const
	{
		return CPlaneDef(-d3YAxis, rPos);
	}
};



//**********************************************************************************************
//
class CPlaneDefPosZ: public CPlaneDefAxis
//
//**************************************
{
public:
	CPlaneDefPosZ(TReal r_pos = 0)
		: CPlaneDefAxis(r_pos)
	{
	}

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		return v3.tZ - rPos;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return v3_rel.tZ;
	}

	//******************************************************************************************
	operator CPlaneDef() const
	{
		return CPlaneDef(d3ZAxis, -rPos);
	}
};


//**********************************************************************************************
//
class CPlaneDefNegZ: public CPlaneDefAxis
//
//**************************************
{
public:
	CPlaneDefNegZ(TReal r_pos = 0)
		: CPlaneDefAxis(r_pos)
	{
	}

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		return rPos - v3.tZ;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return -v3_rel.tZ;
	}

	//******************************************************************************************
	operator CPlaneDef() const
	{
		return CPlaneDef(-d3ZAxis, rPos);
	}
};



#endif
