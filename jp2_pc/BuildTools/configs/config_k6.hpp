/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 * This file contains global compile time control variables.
 *
 * Bugs:
 *
 * To do:
 *
 * NOTE:
 * This file must be explicitly included where it is required. This is a little strange but
 * it keeps the dependencies under control. If this was file was included from within common.hpp
 * then changing this would cause the whole project to be rebuilt whereas the specific include
 * method only causes individual files to be rebuilt.
 *
 * This should be included straight after common.hpp
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/BuildTools/configs/config_k6.hpp                                             $
 * 
 * 1     9/07/98 5:51p Kidsbuild
 * 
 * 17    4/24/98 11:04a Mlange
 * Target processor set to Pentium. Next time you do a check in take care in what you are doing!
 * Thank you!
 * 
 * 16    4/24/98 10:20a Shernd
 * new version information
 * 
 * 15    2/23/98 11:29a Rwycko
 * Set default build back to Pentium...
 * 
 * 14    2/21/98 5:52p Rwyatt
 * Removed all traces of the audio response message. It is no longer required.
 *
 **********************************************************************************************/


#ifndef HEADER_GBLINC_CONFIG_HPP
#define HEADER_GBLINC_CONFIG_HPP


//**********************************************************************************************
// Processor types do not change these.
//
#define PROCESSOR_PENTIUM			1
#define PROCESSOR_PENTIUM_MMX		2
#define PROCESSOR_PENTIUMPRO		3
#define PROCESSOR_PENTIUMPRO_MMX	4
#define PROCESSOR_K6_3D				5



//**********************************************************************************************
// Select a target processor, this list can be extended to one of the above processor types if
// or when required.
// Only 1 processor should be defined at a time.
//
// Only define if a processor was not specified on the command line
//
#ifndef TARGET_PROCESSOR

//#define TARGET_PROCESSOR		PROCESSOR_PENTIUM
//#define TARGET_PROCESSOR		PROCESSOR_PENTIUMPRO
#define TARGET_PROCESSOR		PROCESSOR_K6_3D

//#define TARGET_PROCESSOR		PROCESSOR_PENTIUM_MMX
//#define TARGET_PROCESSOR		PROCESSOR_PENTIUMPRO_MMX
#endif //#ifndef TARGET_PROCESSOR


//**********************************************************************************************
// Enable bi-linear filtering for the terrain copies, only supported on K6-3D right now.
//
#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

#define BILINEAR_FILTER 1

#endif  // if (TARGET_PROCESSOR == PROCESSOR_K6_3D)


//**********************************************************************************************
// if a processor is still not defined it is an error!
//
#ifndef TARGET_PROCESSOR
#error Config.hpp is incorrect, no processor has been defined.
#endif



#endif //#ifndef HEADER_GBLINC_CONFIG_HPP
