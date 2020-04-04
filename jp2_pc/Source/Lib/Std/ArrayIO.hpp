/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		iostream support for array classes
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/Std/ArrayIO.hpp                                                   $
 * 
 * 1     98/07/20 17:10 Speter
 * You $5 bastard file.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_ARRAYIO_HPP
#define HEADER_LIB_STD_ARRAYIO_HPP

#include "Array.hpp"

#include <iostream>

//******************************************************************************************
template<class T, class A> std::ostream& operator <<(std::ostream& os, CArrayT<T, A> a)
{
	for (int i = 0; i < a.size(); i++)
		os << a[i] << ' ';
	return os;
}

#endif
