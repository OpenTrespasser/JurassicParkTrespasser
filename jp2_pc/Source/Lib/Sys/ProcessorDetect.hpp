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
 * Add members to return all the processor info
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/ProcessorDetect.hpp                                           $
 * 
 * 4     7/09/98 1:11a Pkeet
 * Added the 'u4GetCPUSpeed' function.
 * 
 * 3     8/28/97 4:05p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 3     8/25/97 5:18p Rwyatt
 * New functions to return processor make and model
 * 
 * 2     7/08/97 5:16p Rwyatt
 * Added members to return CPU Info
 * 
 * 1     7/08/97 4:40p Rwyatt
 * Wrapper class for the processor detection DLL
 * 
 **********************************************************************************************/


#ifndef HEADER_LIB_SYS_PROCESSORDETECT_HPP
#define HEADER_LIB_SYS_PROCESSORDETECT_HPP


//*********************************************************************************************
// Includes
//
#include "Processor.hpp"


//*********************************************************************************************
// This class relies on the processor DLL being in the current directory.
//
class CCPUDetect
// prefix: det
{
public:

	CCPUDetect();
	~CCPUDetect();

	bool bProcessorDetected()
	{
		return bLoaded;
	}

	const char* strLocalProcessorName()
	{
		return &cpuCPUInfo.strProcessor[0];
	}


	const ECPUMan cpumanProcessorMake()
	{
		return cpuCPUInfo.cpumanProcessorManufacture;
	}

	const ECPUFamily cpufamProcessorModel()
	{
		return cpuCPUInfo.cpufamProcessorFamily;
	}

	const uint32 u4ProcessorFlags()
	{
		return cpuCPUInfo.u4CPUFlags;
	}

private:
	CPUInfo		cpuCPUInfo;
	bool		bLoaded;

	//*****************************************************************************************
	//
	friend uint32 u4GetCPUSpeed
	//
	// Returns the processor speed in Mhz.
	//
	//**************************************
	(
	);
};


#endif //HEADER_LIB_SYS_PROCESSORDETECT_HPP
