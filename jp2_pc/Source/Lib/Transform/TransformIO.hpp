/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		IO routines for transform classes.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/TransformIO.hpp                                        $
 * 
 * 2     98/06/09 21:21 Speter
 * Added ops for CVector2<>.
 * 
 * 1     97/09/17 18:37 Speter
 * Text IO routines for some transform classes.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_TRANSFORMIO_HPP
#define HEADER_LIB_TRANSFORM_TRANSFORMIO_HPP

#include "Transform.hpp"
#include <iostream>

//*********************************************************************************************
//
// CVector2<> operators.
//

	//*********************************************************************************************
	template<class T> std::ostream& operator <<(std::ostream& os, const CVector2<T>& v2)
	{
		return os << v2.tX << ' ' << v2.tY;
	}

	//*********************************************************************************************
	template<class T> std::istream& operator >>(std::istream& is, CVector2<T>& v2)
	{
		return is >> v2.tX >> v2.tY;
	}

//*********************************************************************************************
//
// CVector3<> operators.
//

	//*********************************************************************************************
	template<class T> std::ostream& operator <<(std::ostream& os, const CVector3<T>& v3)
	{
		return os << v3.tX << ' ' << v3.tY << ' ' << v3.tZ;
	}

	//*********************************************************************************************
	template<class T> std::istream& operator >>(std::istream& is, CVector3<T>& v3)
	{
		return is >> v3.tX >> v3.tY >> v3.tZ;
	}

//*********************************************************************************************
//
// CRotate3<> operators.
//

	//*********************************************************************************************
	template<class T> std::ostream& operator <<(std::ostream& os, const CRotate3<T>& r3)
	{
		return os << r3.tC << ' ' << r3.v3S;
	}

	//*********************************************************************************************
	template<class T> std::istream& operator >>(std::istream& is, CRotate3<T>& r3)
	{
		T t_c;
		CVector3<T> v3;
		is >> t_c >> v3;

		new(&r3) CRotate3<T>(t_c, v3);
		return is;
	}


//*********************************************************************************************
//
// CPlacement3<> operators.
//

	//*********************************************************************************************
	template<class T> std::ostream& operator <<(std::ostream& os, const CPlacement3<T>& p3)
	{
		return os << p3.v3Pos << ' ' << p3.r3Rot;
	}

	//*********************************************************************************************
	template<class T> std::istream& operator >>(std::istream& is, CPlacement3<T>& p3)
	{
		return is >> p3.v3Pos >> p3.r3Rot;
	}


#endif
