/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Memory logging implementation
 *
 * This file implements very simple memory logging but it will give us the information that
 * is required.
 *
 * No code will be generated if MEM_LOG is not defined.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/MemoryLog.cpp                                                $
 * 
 * 26    9/08/98 8:57p Rwyatt
 * New memory counters
 * 
 * 25    8/28/98 5:28p Rwyatt
 * Better allocation tracking
 * 
 * 24    8/26/98 11:29a Mmouni
 * Disabled LOG_MEMORY_ALLOCATIONS.
 * 
 * 23    8/25/98 4:38p Rwyatt
 * Updated new and delete operators
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
 * 18    3/27/98 1:14p Mmouni
 * Turned off logging by default.
 * 
 * 17    3/22/98 5:03p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 16    1/29/98 7:47p Rwyatt
 * New memory logs for curved maps
 * 
 * 15    1/09/98 6:56p Rwyatt
 * New counter for wasted memory when rounding textures upto a power of 2
 * 
 * 14    12/21/97 10:53p Rwyatt
 * Added counter for fixed heaps
 * 
 * 13    12/17/97 12:20a Rwyatt
 * New memory logs and a verbose mode with a custom new and delete handler so all allocations
 * can be logged. This feature is not on by default as it is real slow.
 * Can now calculate run time heap fragmentation in debug and release modes.
 * 
 * 12    12/04/97 4:18p Rwyatt
 * New memory logs and a new layout
 * 
 * 11    11/14/97 7:22p Rwyatt
 * More sample memory logs
 * CInstance memory log
 * 
 * 10    9/18/97 3:48p Rwyatt
 * New function/Macro to directly set a counter
 * New counters for system memory (physical, virtual, page)
 * 
 * 9     9/18/97 1:20p Rwyatt
 * Log tables are static and access functions are provided.
 * Keeps max counters
 * 
 * 8     9/02/97 11:23a Rwyatt
 * Zeroed array elements for mip map counters
 * 
 * 7     9/01/97 7:54p Rwyatt
 * New counters for mip maps
 * 
 * 6     97/07/30 12:33p Pkeet
 * Made 'gulMemoryLogTable' non-static so that it can be used externally.
 * 
 * 5     7/29/97 1:54a Rwyatt
 * Logs texture stats
 * 
 * 4     7/24/97 10:15p Rwyatt
 * Removed duplicate header file
 * 
 * 3     7/23/97 8:40p Rwyatt
 * Added Sound Sample log
 * 
 * 2     7/23/97 6:19p Rwyatt
 * Added STL Lists to text table
 * 
 * 1     7/23/97 12:18p Rwyatt
 * Initial implementation
 *
 **********************************************************************************************/
#include "Common.hpp"
#include <stdio.h>
#include <string.h>
#include "MemoryLog.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/sys/DebugConsole.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "crtdbg.h"

//**********************************************************************************************
// define LOG_MEMORY_ALLOCATIONS to enable heavy debugging on memory allocations
//
//#define LOG_MEMORY_ALLOCATIONS


#ifdef LOG_MEM 

	struct SMLName
	// prefix: sml
	{
		EMemoryLog	eml;
		char*		strName;
	};


	static unsigned long gulMemoryLogTable[emlEND];
	static unsigned long gulMaxMemoryLogTable[emlEND];
	static bool bInit = false;

	static SMLName	smlList[] = 
	{
		{emlTotalPhysical,		"Total physical memory",						},
		{emlFreePhysical,		"Free physical memory",							},
		{emlFreePage,			"Free page file memory",						},
		{emlFreeVirtual,		"Free virtual memory",							},

		{emlNewTotal,			"Total RTL allocated memory"					},
		{emlNewMax,				"Total memory used by RTL allocations"			},
		{emlAllocCount,			"Total RTL allocations"							},
		{emlNewCountTotal,		"Number of calls to global new"					},
		{emlDelCountTotal,		"Number of calls to global delete"				},
		{emlDelNull,			"Number of NULL calls to delete"				},
		{emlAlloc0,				"Total number of 0 byte allocations"			},
		{emlAlloc1,				"Total number of 1 byte allocations"			},
		{emlAlloc2,				"Total number of 2 byte allocations"			},
		{emlAlloc3,				"Total number of 3 byte allocations"			},
		{emlAlloc4,				"Total number of 4 byte allocations"			},
		{emlAlloc5,				"Total number of 5 byte allocations"			},
		{emlAlloc6,				"Total number of 6 byte allocations"			},
		{emlAlloc7,				"Total number of 7 byte allocations"			},
		{emlAlloc8,				"Total number of 8 byte allocations"			},
		{emlAlloc9,				"Total number of 9 byte allocations"			},
		{emlAlloc10,			"Total number of 10 byte allocations"			},
		{emlAlloc11,			"Total number of 11 byte allocations"			},
		{emlAlloc12,			"Total number of 12 byte allocations"			},
		{emlAlloc13,			"Total number of 13 byte allocations"			},
		{emlAlloc14,			"Total number of 14 byte allocations"			},
		{emlAlloc15,			"Total number of 15 byte allocations"			},
		{emlAlloc16,			"Total number of 16 byte allocations"			},
		{emlAlloc17,			"Total number of 17 byte allocations"			},
		{emlAlloc18,			"Total number of 18 byte allocations"			},
		{emlAlloc19,			"Total number of 19 byte allocations"			},
		{emlAlloc20,			"Total number of 20 byte allocations"			},
		{emlAlloc21,			"Total number of 21 byte allocations"			},
		{emlAlloc22,			"Total number of 22 byte allocations"			},
		{emlAlloc23,			"Total number of 23 byte allocations"			},
		{emlAlloc24,			"Total number of 24 byte allocations"			},
		{emlAlloc25,			"Total number of 25 byte allocations"			},
		{emlAlloc26,			"Total number of 26 byte allocations"			},
		{emlAlloc27,			"Total number of 27 byte allocations"			},
		{emlAlloc28,			"Total number of 28 byte allocations"			},
		{emlAlloc29,			"Total number of 29 byte allocations"			},
		{emlAlloc30,			"Total number of 30 byte allocations"			},
		{emlAlloc31,			"Total number of 31 byte allocations"			},
		{emlAllocBelow64,		"Total number of allocations 32 to 63 bytes"	},
		{emlAllocBelow128,		"Total number of allocations 64 to 127 bytes"	},
		{emlAllocBelow256,		"Total number of allocations 128 to 255 bytes"	},
		{emlAllocBelow512,		"Total number of allocations 256 to 511 bytes"	},
		{emlAllocBelow1024,		"Total number of allocations 512 to 1023 bytes"	},
		{emlAllocBelow2048,		"Total number of allocations 1024 to 2047 bytes"},
		{emlAllocBelow4096,		"Total number of allocations 2048 to 4095 bytes"},
		{emlAllocBelow8192,		"Total number of allocations 4096 to 8191 bytes"},
		{emlAllocBelow16384,	"Total number of allocations 8192 to 16383 bytes"	},
		{emlAllocAbove16384,	"Total number of allocations above 16384 bytes"		},

		{emlSTL,				"Total memory used by STL"						},
		{emlSTLVector,			"Memory used by STL Vector"						},
		{emlSTLList,			"Memory used by STL List"						},

		{emlSoundSamples,		"DirectSound buffers"							},
		{emlSoundControl,		"Audio System classes/control"					},
		{emlSoundLoader,		"Audio loader/decompression buffers"			},

		{emlTotalInstances,		"Number of Instances"							},
		{emlUniqueInstances,	"Number of unique Instances"					},
		{emlTotalCInfo,			"Number of CInfos"								},
		{emlTotalPhysicsInfo,	"Number of physics infos"						},
		{emlTotalAIInfo,		"Number of AI Infos"							},

		{emlInstance,			"CInstance new operator (fast heap)"			},
		{emlPartition,			"CPartitionSpace new operator (fast heap)"		},
		{emlInstanceNames,		"Instance names (fast heap)"					},
		{emlLoadHeap,			"Load heap allocated (fast heap)"				},
		{emlMeshHeap,			"Mesh heap allocated (fast heap)"				},

		{emlHeap,				"Total reserved virtual heap memory"			},
		{emlCDArray,			"Reserved heap memory in CDArray"				},
		{emlCFastHeap,			"Reserved heap memory in CFastHeap"				},
		{emlCFastHeapCommit,	"Commited heap memory (CDArray and CFastHeap)"	},

		{emlFixedHeap,			"CFixedHeap memory (alloctaed with malloc)"		},

		{emlVirtualPool,		"CVirtualMem address space reserved"			},
		{emlVirtualFixedCommit,	"CVirtualMem address space commited - Non backed"},
		{emlVirtualPagedCommit,	"CVirtualMem address space commited - Page file backed"},
		{emlVirtualSubBlocks,	"Number of CVirtualMem sub blocks"				},
		{emlVirtualSubMem,		"Total memory used by CVirtualMem sub blocks"	},
		{emlVirtualSubUsed,		"Total memory used within sub blocks"			},
		{emlVirtualPages,		"Total memory used by logical page tables"		},

		{emlTotalPolygon,		"Number of unqiue polygons"						},
		{emltotalVertPoint,		"Number of unique vertex points"				},
		{emlTotalVertex,		"Number of unique vertices"						},
		{emlWrapPoints,			"Number of points in wraps"						},
		{emlMeshes,				"Number of meshes constructed"					},
		{emlDeletedMeshes,		"Number of meshes deleted"						},
		{emlDeletedMeshMemory,	"Mesh heap memory wasted by deleted meshes"		},
		{emlOcclude,			"Number of occlusion planes"					},
		{emlBumpMatrix,			"Number of bump map matrices"					},

		{emlCTexture,			"Number of CTexture classes"					},
		{emlCRaster,			"Number of CRasterMem classes"					},

		{emlTextureCount,		"Number of textures loaded"						},
		{emlTextureNoPack,		"Number of textures not in virtual memory"		},
		{emlBumpMapCount,		"Number of bump maps loaded"					},
		{emlBumpCurveCount,		"Number of curved bump maps"					},
		{emlBumpNoPack,			"Number of bump maps not in virtual memory"		},
		{emlTexturePages8,		"Number of 8 bit texture pages"					},
		{emlTexturePages16,		"Number of 16 bit texture pages"				},
		{emlTexturePages32,		"Number of 32 bit texture pages"				},

		{emlTextureMipCount,	"Number of texture mip maps"					},
		{emlBumpMipCount,		"Number of bump mip maps"						},

		{emlTextureManQuad,		"Memory used by texture packer quad tree"		},
		{emlTextureNoPackMem,	"Memory used by textures in system memory"		},
		{emlBumpNoPackMem,		"Memory used by bump maps in system memory"		},
		{emlRoundingWaste,		"Packed bytes wasted due to non powers of 2"	},
		{emlEND, NULL}
	};
#endif //LOG_MEM


//**********************************************************************************************
//
#ifdef LOG_MEM
static char* strFindTextName(int i)
{
	static char* str_element = NULL;
	int	t=0;

	while (smlList[t].eml != emlEND)
	{
		if (smlList[t].eml == i)
			return smlList[t].strName;
		t++;
	}

	return str_element;
}
#endif //LOG_MEM




//**********************************************************************************************
// return one of the counters, DO NOT access the counters directly because it will cause
// maintaining across builds difficult.
//
#ifdef LOG_MEM
unsigned long ulGetMemoryLogCounter(EMemoryLog eml)
{
	if (!bInit)
	{
		memset(gulMemoryLogTable,0,sizeof(unsigned long)*emlEND);
		memset(gulMaxMemoryLogTable,0,sizeof(unsigned long)*emlEND);
		bInit = true;
	}

	return gulMemoryLogTable[eml];
}
#endif


//**********************************************************************************************
// return one of the counters, DO NOT access the counters directly because it will cause
// maintaining across builds difficult.
//
#ifdef LOG_MEM
unsigned long ulGetMaxMemoryLogCounter(EMemoryLog eml)
{
	if (!bInit)
	{
		memset(gulMemoryLogTable,0,sizeof(unsigned long)*emlEND);
		memset(gulMaxMemoryLogTable,0,sizeof(unsigned long)*emlEND);
		bInit = true;
	}

	return gulMaxMemoryLogTable[eml];
}
#endif


//**********************************************************************************************
// return the name of a memory counter....DO NOT access the counters directly
#ifdef LOG_MEM
const char* strGetCounterName(EMemoryLog eml)
{
	if (!bInit)
	{
		memset(gulMemoryLogTable,0,sizeof(unsigned long)*emlEND);
		memset(gulMaxMemoryLogTable,0,sizeof(unsigned long)*emlEND);
		bInit = true;
	}

	return strFindTextName(eml);
}
#endif


//**********************************************************************************************
// adjust one of the counters, this should be called through a macro...
//
#ifdef LOG_MEM
void AddToCounter(EMemoryLog eml, long l_amount)
{
	if (!bInit)
	{
		memset(gulMemoryLogTable,0,sizeof(unsigned long)*emlEND);
		memset(gulMaxMemoryLogTable,0,sizeof(unsigned long)*emlEND);
		bInit = true;
	}

	gulMemoryLogTable[eml] += l_amount;

	if (gulMemoryLogTable[eml] > gulMaxMemoryLogTable[eml])
		gulMaxMemoryLogTable[eml] = gulMemoryLogTable[eml];
}
#endif //LOG_MEM


//**********************************************************************************************
// set one of the counters, this should be called through a macro...
//
#ifdef LOG_MEM
void SetMemLogCounter(EMemoryLog eml, unsigned long l_amount)
{
	if (!bInit)
	{
		memset(gulMemoryLogTable,0,sizeof(unsigned long)*emlEND);
		memset(gulMaxMemoryLogTable,0,sizeof(unsigned long)*emlEND);
		bInit = true;
	}

	gulMemoryLogTable[eml] = l_amount;

	if (gulMemoryLogTable[eml] > gulMaxMemoryLogTable[eml])
		gulMaxMemoryLogTable[eml] = gulMemoryLogTable[eml];
}
#endif //LOG_MEM


//**********************************************************************************************
//
// Code to debug new and delete operations
//
//**********************************************************************************************


#ifdef LOG_MEMORY_ALLOCATIONS

#if VER_DEBUG
#define nNoMansLandSize 4

//
// the MSVC debug heap header....
//

typedef struct _CrtMemBlockHeader
{
        struct _CrtMemBlockHeader * pBlockHeaderNext;
        struct _CrtMemBlockHeader * pBlockHeaderPrev;
        char *                      szFileName;
        int                         nLine;
        size_t                      nDataSize;
        int                         nBlockUse;
        long                        lRequest;
        unsigned char               gap[nNoMansLandSize];
        /* followed by:
         *  unsigned char           data[nDataSize];
         *  unsigned char           anotherGap[nNoMansLandSize];
         */
} _CrtMemBlockHeader;
#endif



void CountAllocationSizes(size_t nSize, int i_inc)
{
	if (nSize<32)
	{
		MEMLOG_ADD_COUNTER(emlAlloc0+nSize, i_inc);
	}
	else if (nSize<64)
	{
		MEMLOG_ADD_COUNTER(emlAllocBelow64, i_inc);
	}
	else if (nSize<128)
	{
		MEMLOG_ADD_COUNTER(emlAllocBelow128, i_inc);
	}
	else if (nSize<256)
	{
		MEMLOG_ADD_COUNTER(emlAllocBelow256, i_inc);
	}
	else if (nSize<512)
	{
		MEMLOG_ADD_COUNTER(emlAllocBelow512, i_inc);
	}
	else if (nSize<1024)
	{
		MEMLOG_ADD_COUNTER(emlAllocBelow1024, i_inc);
	}
	else if (nSize<2048)
	{
		MEMLOG_ADD_COUNTER(emlAllocBelow2048, i_inc);
	}
	else if (nSize<4096)
	{
		MEMLOG_ADD_COUNTER(emlAllocBelow4096, i_inc);
	}
	else if (nSize<8192)
	{
		MEMLOG_ADD_COUNTER(emlAllocBelow8192, i_inc);
	}
	else if (nSize<16384)
	{
		MEMLOG_ADD_COUNTER(emlAllocBelow16384, i_inc);
	}
	else
	{
		MEMLOG_ADD_COUNTER(emlAllocAbove16384, i_inc);
	}
}



#define MAX_LOGGED_ALLOCATIONS	256000		// 2 megs

struct SAllocTag
{
	void*	pvBase;
	size_t	nSize;
};

static SAllocTag AllocList[MAX_LOGGED_ALLOCATIONS];
static bool bBlockInit = false;
void AddBlock(void* pv, size_t nSize)
{
	if (!bBlockInit)
	{
		memset(AllocList,0,sizeof(SAllocTag)*MAX_LOGGED_ALLOCATIONS);
		bBlockInit = true;
	}
	for (int t=0;t<MAX_LOGGED_ALLOCATIONS;t++)
	{
		if (AllocList[t].pvBase == NULL)
		{
			AllocList[t].pvBase = pv;
			AllocList[t].nSize = nSize;
			return;
		}
	}
}

uint32 RemoveBlock(void* pv)
{
	if (!bBlockInit)
	{
		memset(AllocList,0,sizeof(SAllocTag)*MAX_LOGGED_ALLOCATIONS);
		bBlockInit = true;
	}

	for (int t=0;t<MAX_LOGGED_ALLOCATIONS;t++)
	{
		if (AllocList[t].pvBase == pv)
		{
			uint32 u4_size = AllocList[t].nSize;
			AllocList[t].pvBase = NULL;
			AllocList[t].nSize = 0;
			return u4_size;
		}
	}

	// a block has been freed that was never allocated
	return 0xffffffff;
}


char* CalculateHeapFragmentationText()
{
	static char str[256];

	uint32	u4_total = 0;
	uint32	u4_pages = 0;
	uint32	au4PageBase[MAX_LOGGED_ALLOCATIONS];

	for (int i=0;i<MAX_LOGGED_ALLOCATIONS;i++)
	{
		u4_total += AllocList[i].nSize;
		
		int32 i4_size = AllocList[i].nSize;
		uint32 u4_adr = ((uint32)AllocList[i].pvBase) & 0xfffff000;
		while (i4_size>0)
		{
			au4PageBase[u4_pages] = u4_adr;
			u4_pages++;
			Assert(u4_pages < MAX_LOGGED_ALLOCATIONS);
			i4_size -= 4096;
			u4_adr += 4096;
		}
	}

	uint32 u4_unique = 0;

	for (i=0 ; i<u4_pages; i++)
	{
		uint32 u4_base = au4PageBase[i];
		if (u4_base != 0)
			u4_unique++;

		for (int j=0 ; j<u4_pages; j++)
		{
			if (au4PageBase[j] == u4_base)
			{
				au4PageBase[j] = 0;
			}
		}
	}

	sprintf(str,"%d bytes allocated (including headers and padding)\nConsuming %d partial pages [%d Kb]. Fragmentation = %d%%\n",
		u4_total,u4_unique,u4_unique*4,(uint32) (100.0f - (( ((float)u4_total) / ((float)(u4_unique*4096)) ) *100.0f)) );

	return str;
}


//**********************************************************************************************
//
void* __cdecl operator new(size_t nSize)
{
	uint32 u4_consume;

	MEMLOG_ADD_COUNTER(emlAllocCount,1);
	MEMLOG_ADD_COUNTER(emlNewCountTotal,1);
	CountAllocationSizes(nSize,1);

#if VER_DEBUG
	u4_consume = ((nSize+sizeof(_CrtMemBlockHeader)+4) + 15) & 0xfffffff0;
#else
	u4_consume = ((nSize+16) + 15) & 0xfffffff0;
#endif

	MEMLOG_ADD_COUNTER(emlNewTotal, nSize);
	MEMLOG_ADD_COUNTER(emlNewMax, u4_consume);
	void* pv;

#ifdef VER_DEBUG
	pv = _malloc_dbg(nSize, _NORMAL_BLOCK, NULL, 0);
#else
	pv = malloc(nSize);
#endif

	AddBlock(pv, nSize);

	return pv;
}


//**********************************************************************************************
//
void __cdecl operator delete(void* p)
{
	MEMLOG_ADD_COUNTER(emlDelCountTotal,1);
	if (!p)
	{
		MEMLOG_ADD_COUNTER(emlDelNull,1);
		return;
	}

	uint32 u4_consume;
	uint32 u4_size = RemoveBlock(p);

	if (u4_size == 0xffffffff)
	{
		dprintf("Address '%x' deleted but not allocated.\n",p);
		return;
	}

	MEMLOG_SUB_COUNTER(emlAllocCount,1);
	CountAllocationSizes(u4_size,-1);

	MEMLOG_SUB_COUNTER(emlNewTotal,u4_size);

#if VER_DEBUG
	u4_consume = ((u4_size+sizeof(_CrtMemBlockHeader)+4) + 15) & 0xfffffff0;
#else
	u4_consume = ((u4_size+16) + 15) & 0xfffffff0;
#endif

	MEMLOG_SUB_COUNTER(emlNewMax,u4_consume);


#if VER_DEBUG
	_free_dbg(p, _NORMAL_BLOCK);
#else
	free(p);
#endif

}


#else //#ifdef LOG_MEMORY_ALLOCATIONS

char* CalculateHeapFragmentationText()
{
	return NULL;
}

#endif  //#ifdef LOG_MEMORY_ALLOCATIONS


