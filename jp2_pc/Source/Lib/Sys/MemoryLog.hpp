/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Memory logging definitions and constants.
 *
 * This file should reduce to nothing if the LOG_MEM macro is not defined. Also this file
 * should not include anything else because it is included from within the STL allocator.
 *
 * This file should be included anywhere that memory logging is required. Becareful what is
 * included from within here because our STL headers include this file.
 *
 * Changing this file will cause vector.h list.h and defalloc.h to be rebuilt, in turn this
 * will cause the whole project to be rebuilt.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/MemoryLog.hpp                                                $
 * 
 * 24    9/08/98 8:57p Rwyatt
 * New counters
 * 
 * 23    9/02/98 6:07p Agrant
 * added memory log category
 * 
 * 22    8/19/98 1:29p Rwyatt
 * New memory logs for ImageLoad heap and bump matricies.
 * 
 * 21    8/13/98 1:39p Rwyatt
 * Added memory logs for rasters and textures
 * 
 * 20    8/10/98 5:21p Rwyatt
 * Added mesh and Info memory logs, also split up allocated memory and alloctaed memory with
 * headers.
 * 
 * 19    4/21/98 2:47p Rwyatt
 * New counters for pageable and non pageable memory.
 * New counter for the number of non-pageable sub allocators.
 * 
 * 18    4/06/98 3:41p Agrant
 * LOG_MEM should always be defined here.  To turn off the slower logging of memory
 * allocations, look in the .cpp file.
 * 
 * 17    3/27/98 1:14p Mmouni
 * Turned off logging by default.
 * 
 * 16    3/22/98 5:03p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 15    1/29/98 7:47p Rwyatt
 * New memory logs for curved maps
 * 
 * 14    1/09/98 6:56p Rwyatt
 * New counter for wasted memory when rounding textures upto a power of 2
 * 
 * 13    12/21/97 10:53p Rwyatt
 * Added counter for fixed heaps
 * 
 * 12    12/17/97 12:20a Rwyatt
 * New memory logs and a verbose mode with a custom new and delete handler so all allocations
 * can be logged. This feature is not on by default as it is real slow.
 * Can now calculate run time heap fragmentation in debug and release modes.
 * 
 * 11    12/04/97 4:10p Rwyatt
 * New memory logs for virtual memory and fast heaps
 * 
 * 10    11/14/97 7:17p Rwyatt
 * New memory logs
 * 
 * 9     10/22/97 5:49p Agrant
 * Do not negate an unsigned value, please.
 * 
 * 8     9/18/97 3:48p Rwyatt
 * New function/Macro to directly set a counter
 * New counters for system memory (physical, virtual, page)
 * 
 * 7     9/18/97 1:20p Rwyatt
 * Log tables are static and access functions are provided.
 * Keeps max counters
 * 
 * 6     9/01/97 7:55p Rwyatt
 * New counters for mip maps
 * 
 * 5     7/29/97 1:54a Rwyatt
 * Logs texture stats
 * 
 * 4     7/24/97 10:15p Rwyatt
 * Added macro to increment a log counter from the size of an allocated buffer
 * 
 * 3     7/23/97 8:40p Rwyatt
 * Added Sound Sample log
 * 
 * 2     7/23/97 6:18p Rwyatt
 * Latest Log list
 * 
 * 1     7/23/97 12:19p Rwyatt
 * Initial Implementation
 * Just generic STL is logged
 *
 **********************************************************************************************/


#ifndef HEADER_SYS_MEMORYLOG_HPP
#define HEADER_SYS_MEMORYLOG_HPP

#include <malloc.h>

//**********************************************************************************************
// Uncomment to removed memory logging
//
#define LOG_MEM


//**********************************************************************************************
// The enum below should be used as an index into the memory log array to find how much memory
// the particular section/system/library is using.
// If you add enteries to this table, also insert description strings into the MemoryLog.cpp
// file.
//
enum EMemoryLog
// prefix eml
{
	emlTotalPhysical=0, // Total physical memory in the machine
	emlFreePhysical,	// Free physical memory
	emlFreePage,		// Free page file memory
	emlFreeVirtual,		// Free virtual memory

	eml0000,

	emlNewTotal,		// current amount of memory allocated with new
	emlNewMax,			// estimate of amount of memory used by new including headers..
	emlAllocCount,		// current number of allocations
	emlNewCountTotal,	// total number of calls to new
	emlDelCountTotal,	// total number of calls to delete
	emlDelNull,			// total number of null deletes
	emlAlloc0,
	emlAlloc1,
	emlAlloc2,
	emlAlloc3,
	emlAlloc4,
	emlAlloc5,
	emlAlloc6,
	emlAlloc7,			// total allocations of a particular size
	emlAlloc8,
	emlAlloc9,
	emlAlloc10,
	emlAlloc11,
	emlAlloc12,
	emlAlloc13,
	emlAlloc14,
	emlAlloc15,
	emlAlloc16,
	emlAlloc17,
	emlAlloc18,
	emlAlloc19,			// total allocations of a particular size
	emlAlloc20,
	emlAlloc21,
	emlAlloc22,
	emlAlloc23,
	emlAlloc24,
	emlAlloc25,
	emlAlloc26,
	emlAlloc27,
	emlAlloc28,
	emlAlloc29,
	emlAlloc30,
	emlAlloc31,
	emlAllocBelow64,	// total allocations of a particular size
	emlAllocBelow128,
	emlAllocBelow256,
	emlAllocBelow512,
	emlAllocBelow1024,
	emlAllocBelow2048,
	emlAllocBelow4096,
	emlAllocBelow8192,
	emlAllocBelow16384,
	emlAllocAbove16384,

	eml0002,

	emlTotalInstances,	// Number of instances in the whole scene
	emlUniqueInstances, // Number of unique instances
	emlTotalCInfo,		// Number of CInfo's
	emlTotalPhysicsInfo,// Number of physics infos
	emlTotalAIInfo,		// Number of AI Infos

	eml0005,

	emlInstance,		// memory allocated by CInstance new operator
	emlPartition,		// memory allocated by CPartitionSpace new operator
	emlAI,				// memory allocated for AI in fast heap
	emlInstanceNames,	// memory used by the instance names
	emlLoadHeap,		// memory used by the load heap
	emlMeshHeap,		// memory used by the global mesh heap

	eml0004,

	emlHeap,			// amount of virtual memory in the private heaps
	emlCDArray,			// amount of heap space within CDArray's
	emlCFastHeap,		// amount of heap space within CFastHeap
	emlCFastHeapCommit,	// amount of commited fast heap space

	eml0008,

	emlFixedHeap,		// amount of memory allocated within fixed heaps

	eml0003,

	emlVirtualPool,		// amount of virtual memory we have allocated
	emlVirtualFixedCommit,	// amount of virtual memory we have committed
	emlVirtualPagedCommit,	// amount of virtual memory we have committed in page file backed memory
	emlVirtualSubBlocks,// Number of CVirtualMem sub blocks
	emlVirtualSubMem,	// Total memory used by CVirtualMem sub blocks
	emlVirtualSubUsed,	// Total memory used within sub blocks
	emlVirtualPages,	// memory used by the VM system logical memory pages

	eml0007,

	emlSTL,				// Total memory currently allocated by STL
	emlSTLVector,		// Total memory currently allocated by STL Vector
	emlSTLList,			// Total memory currently allocated by STL List

	eml0001,

	emlSoundSamples,	// total memory used by just sample data
	emlSoundControl,	// total control memory used by the sound system
	emlSoundLoader,		// total memory used by the sound loader

	eml0009,

	emlMeshes,			// Number of meshes constructed
	emlDeletedMeshes,	// of the above, how many have been deleted
	emlDeletedMeshMemory,	// Total memory wasted by deleted meshes
	emlTotalPolygon,	// Number of polygons in the scene
	emltotalVertPoint,	// Number of unique vertex points
	emlTotalVertex,		// Number of unique vertices
	emlWrapPoints,		// Number of points in the wrap
	emlOcclude,			// Number of occlusion planes
	emlBumpMatrix,		// Number of bump map matrices

	eml0006,

	emlCTexture,		// number of CTexture classes
	emlCRaster,			// number of CRasters
	emlTextureCount,	// number of textures loaded
	emlTextureNoPack,	// number of textures that we failed to pack
	emlBumpMapCount,	// number of bump maps loaded
	emlBumpCurveCount,	// number of curved bump maps
	emlBumpNoPack,		// number of bump maps that we failed to pack
	emlTexturePages8,	// number of 8 bit pages
	emlTexturePages16,	// number of 16 bit pages
	emlTexturePages32,	// number of 32 bit pages
	emlTextureMipCount,	// number of texture mip maps
	emlBumpMipCount,	// number of texture mip maps
	emlTextureManQuad,	// memory used by the texture managers quad trees
	emlTextureNoPackMem,// memory used by textures that would not pack
	emlBumpNoPackMem,	// memory used by bump maps that would not pack
	emlRoundingWaste,	// bytes wasted because of power of 2 rounding
	emlEND				// do not insert anything after here...
};


//**********************************************************************************************
// Defines to identify different types of class specific new memory blocks. The requested blocks
// are extened by 8 bytes, the first dword is the check DWORD and the second DWORD is the size
// of the block.
// 
#define MEM_LOG_INSTANCE_CHECKWORD	(0xDEADC0D0)
#define MEM_LOG_PARTITION_CHECKWORD	(0xDEADC0D1)


//**********************************************************************************************
// 
#ifdef LOG_MEM
extern const char* strGetCounterName(EMemoryLog eml);
#define MEMLOG_COUNTER_NAME(x) strGetCounterName((EMemoryLog)x)
#else
#define MEMLOG_COUNTER_NAME(x) ""
#endif

//**********************************************************************************************
// 
#ifdef LOG_MEM
unsigned long ulGetMaxMemoryLogCounter(EMemoryLog eml);
#else
#define ulGetMaxMemoryLogCounter(x) (0)
#endif


//**********************************************************************************************
// 
#ifdef LOG_MEM
unsigned long ulGetMemoryLogCounter(EMemoryLog eml);
#else
#define ulGetMemoryLogCounter(x) (0)
#endif




//**********************************************************************************************
// Adjust one of the counters, if not logging memory thsi function is NULL
//
#ifdef LOG_MEM
void AddToCounter(EMemoryLog eml, long l_amount);
#else
#define AddToCounter(x,y)
#endif


//**********************************************************************************************
// Set one of the counters, if not logging memory thsi function is NULL
//
#ifdef LOG_MEM
void SetMemLogCounter(EMemoryLog eml, unsigned long l_amount);
#else
#define SetMemLogCounter(x,y)
#endif



//**********************************************************************************************
// define all of the macros that do the work
//
#ifdef LOG_MEM

//**********************************************************************************************
// If we are memory logging direct the macros to the memory functions.
//**********************************************************************************************
#define	MEMLOG_ADD_COUNTER(count, mem) AddToCounter((EMemoryLog)(count), (long)(mem))
#define	MEMLOG_SUB_COUNTER(count, mem) AddToCounter((EMemoryLog)(count), -((long)(mem)))
#define MEMLOG_ADD_ADRSIZE(count, buffer) if (buffer) AddToCounter((EMemoryLog)(count), _msize( (buffer) ) )
#define MEMLOG_SUB_ADRSIZE(count, buffer) if (buffer) AddToCounter((EMemoryLog)(count), -(int)_msize( (buffer) ) )
#define MEMLOG_SET_COUNTER(count, size) SetMemLogCounter((EMemoryLog)(count), (unsigned long)size)
#else

//**********************************************************************************************
// If we are not memory logging the macros reduce to nothing at all
//**********************************************************************************************
#define	MEMLOG_ADD_COUNTER(count, mem)
#define	MEMLOG_SUB_COUNTER(count, mem)
#define MEMLOG_ADD_ADRSIZE(count, mem)
#define MEMLOG_SUB_ADRSIZE(count, mem)
#define MEMLOG_SET_COUNTER(count, size)
#endif // LOG_MEM



#endif //HEADER_SYS_MEMORYLOG_HPP


 


