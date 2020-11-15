/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 * Application side of the processor detection system.
 *
 * Bugs:
 *
 * To do:
 * Have some method of locating the DLL if it is no where we expected it to be.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/ProcessorDetect.cpp                                           $
 * 
 * 3     7/09/98 1:11a Pkeet
 * Added the 'u4GetCPUSpeed' function.
 * 
 * 2     7/08/97 5:16p Rwyatt
 * Now loads the DLL from the current Dir
 * 
 * 1     7/08/97 4:47p Rwyatt
 * Class wrapper for the processor detection DLL
 * 
 **********************************************************************************************/


//*********************************************************************************************
//
#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "ProcessorDetect.hpp"



//*********************************************************************************************
// Class Implementation
//*********************************************************************************************

//*********************************************************************************************
// Constructor loads the DLL, does the stuff and then unloads it. We do not keep the DLL
// around until the destructor is called. This is because the DLL only returns a 100 byte
// struture which can be copied into some local storage.
//
// If the DLL proves to be a problem then meybe we could move the DLL code into this class
// but that would mean keeping it around for the whole game which is pointless.
//
CCPUDetect::CCPUDetect()
{
	HINSTANCE	h_lib = NULL;
	CPUFUNC		func;
	bool		b_res;
	
	bLoaded = false;

	// load from the current directory
	h_lib = LoadLibrary("processor.dll");

	if (h_lib)
	{
		func=(CPUFUNC)GetProcAddress(h_lib,"bGetProcessorInfo");
		if (func)
		{
			b_res = func(&cpuCPUInfo);
			if (b_res)
			{
				bLoaded = true;
			}
		}
	}

	// unload the DLL if we managed to load it
	if (h_lib)
	{
		FreeLibrary(h_lib);
	}
}



//*********************************************************************************************
//
CCPUDetect::~CCPUDetect()
{
	// do nothing at the moment
}


//*********************************************************************************************
uint32 u4GetCPUSpeed()
{
	CCPUDetect det;

	// If the cpu is not reliably detected, return 0.
	if (!det.bLoaded)
		return 0;

	// Return the detected speed.
	return det.cpuCPUInfo.u4CPUSpeed;
}
