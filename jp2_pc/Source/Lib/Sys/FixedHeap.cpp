/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of 'FixedHeap.hpp.'
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/FixedHeap.cpp                                                 $
 * 
 * 9     10/01/98 12:25a Pkeet
 * Upped the default memory allocation.
 * 
 * 8     8/28/98 11:58a Asouth
 * Added another include (MW compiler is more strict about types)
 * 
 * 7     3/10/98 1:20p Pkeet
 * Added include to "LocalArray.hpp."
 * 
 * 6     12/21/97 10:54p Rwyatt
 * Added memory logs for the total memory used by all fixed heaps
 * 
 * 5     12/01/97 12:27p Pkeet
 * 
 * 4     12/01/97 11:25a Pkeet
 * Added some paranoid asserts.
 * 
 * 3     11/26/97 4:42p Pkeet
 * Added numerous asserts. Added the 'DumpState' member function. Fix various bugs.
 * 
 * 2     11/26/97 12:04p Pkeet
 * Added the 'DumpState' member function and the global variable.
 * 
 * 1     11/26/97 11:29a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include <malloc.h>
#include "Set.h"
#include "Multiset.h"
#include "Lib/W95/Direct3D.hpp"
#include "TextOut.hpp"
#include "FixedHeap.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Std/MemLimits.hpp"
#include "Lib/Sys/debugConsole.hpp"
#include "Lib/Renderer/RenderCache.hpp"


//
// Macros.
//

// Default and maximum allocations.
#define iNOMINAL_CACHE_MEMORY_KB (256)
#define iDEFAULT_CACHE_MEMORY_KB (6144)
#define iMAX_CACHE_MEMORY_KB     (iDEFAULT_CACHE_MEMORY_KB * 2)


//
// Enumerations.
//

// Enumeration of the type of memory allocated at a heap node.
enum EMemHeapType
{
	emtUndefined,	// Unknown.
	emtLocalHeap,	// Allocated from a CFixedHeap object.
	emtGlobalHeap	// Allocated from the standard library's malloc.
};

//
// Enumeration types for 'DumpState' only because the compiler will not allow enumerations
// to be defined locally in functions.
//
enum EMemUse { emuUnknown, emuFree, emuUsed };
//
// Used for building a memory map.
//
// Prefix: emu
//


//
// Internal class definitions.
//

//*********************************************************************************************
//
class CHeapNode
//
// Prefix: hn
//
// Node for a fixed heap to track size and types of blocks allocated.
//
//**************************************
{
public:

	EMemHeapType emtMemType;
	void*        pvMem;
	int          iSize;

public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CHeapNode()
		: emtMemType(emtUndefined), pvMem(0), iSize(0)
	{
	}

	// Constructor accepting variable arguements.
	CHeapNode(EMemHeapType emt, void* pv_mem, int i_size)
		: emtMemType(emt), pvMem(pv_mem), iSize(i_size)
	{
	}

};


//*********************************************************************************************
//
class CHeapNodeMemLess
//
// Node for sorting fixed heap nodes by memory address.
//
//**************************************
{
public:

	bool operator()(const CHeapNode& hn_a, const CHeapNode& hn_b) const
	{
		return hn_a.pvMem < hn_b.pvMem;
	}

};


//*********************************************************************************************
//
class CHeapNodeSizeLess
//
// Node for sorting fixed heap nodes by block size.
//
//**************************************
{
public:

	bool operator()(const CHeapNode& hn_a, const CHeapNode& hn_b) const
	{
		return hn_a.iSize < hn_b.iSize;
	}

};


//
// Type definitions.
//

//
// Define internal types. Note that the compiler will not accept typedefs for opaque
// declarations; therefore 'THeapMem' and 'THeapSize' are declared as classes but use
// Hungarian notation for typedefs.
//
class THeapMem  : public set<CHeapNode, CHeapNodeMemLess>{};
class THeapSize : public multiset<CHeapNode, CHeapNodeSizeLess>{};


//
// Class implementations.
//

//*********************************************************************************************
//
// CFixedHeap implementation.
//

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	//*****************************************************************************************
	CFixedHeap::CFixedHeap()
		: iSize(0), phsetAllocated(0), phsetFreeSize(0), phsetFreeMem(0)
	{
		Reset();
	}

	//*****************************************************************************************
	CFixedHeap::~CFixedHeap()
	{
		Dealloc();
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	void* CFixedHeap::pvMalloc(int i_size)
	{
		// Round size request to match granularity.
		int i_mask = iGranularity - 1;
		int i_round_size = (i_size + i_mask) & (~i_mask);

		// Construct a dummy insertion heap node.
		CHeapNode hn(emtUndefined, 0, i_round_size);

		//
		// Attempt to locate a free location the size of the block.
		//
		{
			THeapSize::iterator it_free = phsetFreeSize->find(hn);

			// If a node of the right size is found, use it.
			if (it_free != phsetFreeSize->end())
			{
				hn = *it_free;
				Assert(hn.pvMem);
				Assert(hn.iSize);
				Assert(hn.emtMemType == emtLocalHeap);

				// Remove the block from the free list.
				phsetFreeSize->erase(it_free);
				phsetFreeMem->erase(hn);

				// Add the block to the used list.
				phsetAllocated->insert(hn);

				// Return the allocated memory.
				Assert(hn.pvMem);
				return hn.pvMem;
			}
		}

		//
		// Attempt to find the smallest free block in which the size will fit.
		//
		{
			// Perform a dummy insertion and get an iterator for its position.
			phsetFreeSize->insert(hn);
			THeapSize::iterator it_dummy = phsetFreeSize->find(hn);
			THeapSize::iterator it_free  = it_dummy;
			++it_free;

			// If there are nodes larger than the requested size, use them.
			if (it_dummy != phsetFreeSize->end() && it_free != phsetFreeSize->end())
			{
				Assert(it_dummy != it_free);

				// Store the heap node information.
				CHeapNode hn_mem = *it_free;
				Assert(hn_mem.pvMem);
				Assert(hn_mem.iSize);
				Assert(hn_mem.emtMemType == emtLocalHeap);

				// Remove the dummy and current node from the free list.
				phsetFreeSize->erase(it_free);
				phsetFreeMem->erase(hn_mem);
				phsetFreeSize->erase(hn);
				Assert(phsetFreeSize->find(hn) == phsetFreeSize->end());

				// Allocate memory from the free node.
				hn            = hn_mem;
				hn.iSize      = i_round_size;
				hn_mem.pvMem  = (void*)(((char*)hn_mem.pvMem) + i_round_size);
				hn_mem.iSize -= i_round_size;

				// Insert the remaining piece of the node back into the free list.
				Assert(hn_mem.pvMem);
				Assert(hn_mem.iSize);
				Assert(hn_mem.emtMemType == emtLocalHeap);
				phsetFreeSize->insert(hn_mem);
				phsetFreeMem->insert(hn_mem);

				// Add the block to the used list.
				Assert(hn.pvMem);
				Assert(hn.iSize);
				Assert(hn.emtMemType == emtLocalHeap);
				phsetAllocated->insert(hn);

				// Return the allocated memory.
				Assert(hn.pvMem);
				return hn.pvMem;
			}
			else
			{
				phsetFreeSize->erase(hn);
				Assert(phsetFreeSize->find(hn) == phsetFreeSize->end());
			}
		}

		//
		// As a last resort, use malloc.
		//
		hn = CHeapNode(emtGlobalHeap, malloc(i_size), i_size);
		Assert(hn.pvMem);

		// Add the block to the used list.
		Assert(hn.pvMem);
		Assert(hn.iSize);
		Assert(hn.emtMemType == emtGlobalHeap);
		phsetAllocated->insert(hn);

		// Return the address of the malloc block.
		return hn.pvMem;
	}

	//*****************************************************************************************
	void CFixedHeap::Free(void* pv)
	{
		CHeapNode hn;
		int i_num_nodes = phsetAllocated->size();

		//
		// Locate the node associated with this block of memory and remove it.
		//
		{
			CHeapNode hn_dummy(emtUndefined, pv, 0);

			// Locate the node.
			THeapMem::iterator it_mem = phsetAllocated->find(hn_dummy);
			hn = *it_mem;
			if (it_mem == phsetAllocated->end())
			{
				DumpState();
				Assert(0);
			}

			// Remove the node.
			phsetAllocated->erase(it_mem);
			AlwaysAssert(phsetAllocated->find(hn_dummy) == phsetAllocated->end());
		}

		//
		// Free the node.
		//
		switch (hn.emtMemType)
		{
			case emtLocalHeap:
				{
					// Return the memory to the free list.
					Assert(hn.pvMem);
					Assert(hn.iSize);
					Assert(hn.emtMemType == emtLocalHeap);
					phsetFreeSize->insert(hn);
					phsetFreeMem->insert(hn);
				}
				break;
			case emtGlobalHeap:
				{
					// Call the free function associated with 'malloc.'
					free(hn.pvMem);
				}
				break;
			default:
				AlwaysAssert(0);
		}
		AlwaysAssert((i_num_nodes - 1) == phsetAllocated->size());
	}

	//*****************************************************************************************
	bool CFixedHeap::bConglomerate()
	{
		// Do nothing if the heap as too few allocations.
		if (phsetFreeMem->size() < 2)
			return false;

		// Set flag indicating free blocks were merged.
		bool b_retval = false;

		// Iterate through each node until the next node is the last node.
		THeapMem::iterator it = phsetFreeMem->begin();
		for (; it != phsetFreeMem->end(); ++it)
		{
			// Get the next node.
			THeapMem::iterator it_next = it;
			++it_next;
			if (it_next == phsetFreeMem->end())
				return b_retval;

			//
			// If the difference in the pointers of the current node and the next node is
			// equal to the size of the current node, the nodes can be merged.
			//
			int i_diff = int((*it_next).pvMem) - int((*it).pvMem);
			Assert(i_diff > 0);
			//Assert(i_diff <= (*it).iSize);

			if (i_diff == (*it).iSize)
			{
				// Set flag indicating free blocks were merged.
				b_retval = true;

				// Merge the two nodes.
				CHeapNode hn_base = *it;
				CHeapNode hn_next = *it_next;

				// Remove the nodes from the lists.
				phsetFreeMem->erase(hn_base);
				phsetFreeMem->erase(hn_next);

				// Remove the base node from the size list.
				THeapSize::iterator it_lower = phsetFreeSize->lower_bound(hn_base);
				THeapSize::iterator it_upper = phsetFreeSize->upper_bound(hn_base);
				{
					THeapSize::iterator it_size = it_lower;
					for (; it_size != it_upper; ++it_size)
					{
						if ((*it_size).pvMem == hn_base.pvMem)
						{
							phsetFreeSize->erase(it_size);
							break;
						}
					}
				}

				// Remove the next node from the size list.
				it_lower = phsetFreeSize->lower_bound(hn_next);
				it_upper = phsetFreeSize->upper_bound(hn_next);
				{
					THeapSize::iterator it_size = it_lower;
					for (; it_size != it_upper; ++it_size)
					{
						if ((*it_size).pvMem == hn_next.pvMem)
						{
							phsetFreeSize->erase(it_size);
							break;
						}
					}
				}

				// Merge the next node into the current node.
				hn_base.iSize += hn_next.iSize;

				// Add the base node back into the free lists.
				Assert(hn_base.pvMem);
				Assert(hn_base.iSize);
				Assert(hn_base.emtMemType == emtLocalHeap);
				phsetFreeMem->insert(hn_base);
				phsetFreeSize->insert(hn_base);

				// Set the current iterator.
				it = phsetFreeMem->find(hn_base);
			}
		}
		return b_retval;
	}
	
	//*****************************************************************************************
	void CFixedHeap::GetInfo(int& ri_num_nodes, int& ri_mem_used) const
	{
		// Get the number of nodes in the used list.
		ri_num_nodes = phsetAllocated->size();

		// Sum the used blocks.
		ri_mem_used = 0;
		{
			THeapMem::iterator it = phsetAllocated->begin();
			for (; it != phsetAllocated->end(); ++it)
				ri_mem_used += (*it).iSize;
		}
	}

	//*****************************************************************************************
	void CFixedHeap::DumpState() const
	{
		int  i_num_blocks = iSize / iGranularity;
		int  i_num_nodes          = 0;
		int  i_num_external_nodes = 0;
		int  i_external_size      = 0;
		int  i_total_size         = 0;

		// Allocate memory for a memory map and fill with a value representing unknown.
		CLArray(EMemUse, aemu, i_num_blocks);
		{
			for (int i_block = 0; i_block < i_num_blocks; ++i_block)
				aemu[i_block] = emuUnknown;
		}

		// Write used blocks to the memory map.
		{
			THeapMem::iterator it = phsetAllocated->begin();
			for (; it != phsetAllocated->end(); ++it, ++i_num_nodes)
			{
				i_total_size += (*it).iSize;
				switch ((*it).emtMemType)
				{
					case emtLocalHeap:
						{
							// Get the starting block.
							int i_block_start = (int((*it).pvMem) - int(pvHeap)) / iGranularity;
							Assert(i_block_start >= 0 && i_block_start < i_num_blocks);

							// Get the number of blocks.
							int i_num_local_blocks = (*it).iSize / iGranularity;
							Assert(i_num_local_blocks > 0);

							// Fill the map.
							for (int i_block = 0; i_block < i_num_local_blocks; ++i_block)
								aemu[i_block + i_block_start] = emuUsed;
						}
						break;
					case emtGlobalHeap:
						++i_num_external_nodes;
						i_external_size += (*it).iSize;
						break;
					default:
						Assert(0);
				}
			}
		}

		// Write free blocks to the memory map.
		{
			THeapMem::iterator it = phsetFreeMem->begin();
			for (; it != phsetFreeMem->end(); ++it)
			{
				switch ((*it).emtMemType)
				{
					case emtLocalHeap:
						{
							// Get the starting block.
							int i_block_start = (int((*it).pvMem) - int(pvHeap)) / iGranularity;
							Assert(i_block_start >= 0 && i_block_start < i_num_blocks);

							// Get the number of blocks.
							int i_num_local_blocks = (*it).iSize / iGranularity;
							Assert(i_num_local_blocks > 0);

							// Fill the map.
							for (int i_block = 0; i_block < i_num_local_blocks; ++i_block)
								aemu[i_block + i_block_start] = emuFree;
						}
						break;
					case emtGlobalHeap:
						break;
					default:
						Assert(0);
				}
			}
		}

		CConsoleBuffer con_out;	// Buffer to write to.

		// Open the file for writing text out.
		con_out.OpenFileSession("FixedHeap.txt");

		// Write out the number of externally allocated blocks.
		con_out.Print("Number of allocation made total:               %ld\n",
			          i_num_nodes);
		con_out.Print("Number of kbytes allocated total:              %ld\n",
		              i_total_size >> 10);
		con_out.Print("Number of allocation made by the Windows heap: %ld\n",
			          i_num_external_nodes);
		con_out.Print("Number kbytes allocated by the Windows heap:   %ld\n",
			          i_external_size >> 10);

		// Write the blocks to the buffer.
		con_out.Print("\nMemoryMap\n");
		{
			for (int i_block = 0; i_block < i_num_blocks; ++i_block)
			{
				// Go to the next line if required.
				if ((i_block & 63) == 0)
					con_out.Print("\n\t");
				
				// Write a character based on the type.
				switch (aemu[i_block])
				{
					case emuUnknown:
						con_out.Print(" ! ");
						break;
					case emuFree:
						con_out.Print(".");
						break;
					case emuUsed:
						con_out.Print("*");
						break;
					default:
						Assert(0);
				}
			}
		}

		// Close the file for text.
		con_out.CloseFileSession();
	}
	
	//*****************************************************************************************
	void CFixedHeap::Reset()
	{
		// Hardware usually supplies its own buffers.
		if (d3dDriver.bUseD3D())
		{
			// Allocate a nominal amount of memory.
			Alloc(iNOMINAL_CACHE_MEMORY_KB * 1024, 128);
			return;
		}

		// Normal allocation for 32 Mb machines.
		if (u4TotalPhysicalMemory() < (1 << 20) * 60)
		{
			// Allocate the default amount of memory.
			Alloc(iDEFAULT_CACHE_MEMORY_KB * 1024, 128);
			return;
		}

		// Allocate a large block of memory.
		Alloc(iMAX_CACHE_MEMORY_KB * 1024, 128);
	}

	//*****************************************************************************************
	//
	void CFixedHeap::Alloc
	(
		int i_size,			// Size of the memory block for use by the heap.
		int i_granularity	// Mimimum amount of memory allocated at a time.
	)
	//
	// Allocates a new fixed heap.
	//
	//**************************************
	{
		// Do nothing if the correct size is already allocated.
		if (iSize == i_size)
			return;
		PurgeRenderCaches();
		Dealloc();

		iSize        = i_size;
		pvHeap       = malloc(i_size);
		iGranularity = i_granularity;

		Assert(iSize > 0);
		Assert(pvHeap);

		MEMLOG_ADD_COUNTER(emlFixedHeap, iSize);

		// Create the allocated and free lists.
		phsetAllocated = new THeapMem();
		phsetFreeSize  = new THeapSize();
		phsetFreeMem   = new THeapMem();

		// Insert the entire heap as one free block.
		CHeapNode hn(emtLocalHeap, pvHeap, iSize);
		phsetFreeSize->insert(hn);
		phsetFreeMem->insert(hn);

		dprintf("Cache heap size: %ldKb\n", iGetSizeKB());
	}

	//*****************************************************************************************
	//
	void CFixedHeap::Dealloc
	(
	)
	//
	// Deallocates all the memory in the fixed heap.
	//
	//**************************************
	{
		// Release memory.
		if (pvHeap)
		{
			free(pvHeap);
			MEMLOG_SUB_COUNTER(emlFixedHeap, iSize);
		}

		// Delete the allocated and free lists.
		delete phsetAllocated;
		delete phsetFreeSize;
		delete phsetFreeMem;

		// Set pointers to zero.
		pvHeap         = 0;
		phsetAllocated = 0;
		phsetFreeSize  = 0;
		phsetFreeMem   = 0;
	}


//
// Global variables.
//
CFixedHeap fxhHeap;