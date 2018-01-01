/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Com.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/Com.cpp                                                      $
 * 
 * 3     97/01/07 11:07 Speter
 * Reordered includes.
 * 
 * 2     11/11/96 8:57p Pkeet
 * Added '#undef' 's for min and max.
 * 
 * 1     96/08/09 10:55 Speter
 * Wrap COM functionality, w/ automatic AddRef/Release.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Com.hpp"

#undef min
#undef max
#include <unknwn.h>


void ComAddRef(void* p)
{
	if (p)
		((IUnknown*)p)->AddRef();
}

void ComRelease(void* p)
{
	if (p)
		((IUnknown*)p)->Release();
}
