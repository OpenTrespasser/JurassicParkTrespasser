/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of DepthSort.hpp.
 *
 * Bugs:
 *		Occasionally P can't be split by Q's plane, and Q can't be split by P's plane. The
 *		temporary fix is to simply render P under these conditions, but this test should be
 *		monitored as optimizations are added, and may be the source of potential future renderer
 *		sort screw-ups.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/DepthSort.cpp                                            $
 * 
 * 119   10/01/98 12:50a Asouth
 * Added missing paren.
 * 
 * 118   10/01/98 12:35a Asouth
 * bool size is 1 in MW build
 * 
 * 117   98.09.24 1:40a Mmouni
 * Shrunk CRenderPolygon by a large amount.
 * Multiple list depth sort stuff is now on a compile switch.
 * 
 * 116   98.09.23 3:01p Mmouni
 * Made polygon pointer array not decommit.
 * 
 * 115   98.09.04 4:30p Mmouni
 * Distance values are now adjusted to remain at the same absolute distance at various quality
 * settings.
 * Distance values are now adjusted by the quality slider.
 * Moved the calculation of plane equations to after we emit all the polygons we are not going
 * to depth sort.
 * Changed binary partition threshold to 1000 since not partitioning is faster.
 * 
 * 114   9/02/98 5:42p Asouth
 * removed a number of 'short' qualifiers from some very long jumps
 * 
 * 113   9/01/98 4:53p Asouth
 * loop vars
 * 
 * 112   98/08/25 18:18 Speter
 * Must increase heap size to avoid crashing TestScene. Needs more debugging.
 * 
 * 111   8/25/98 4:40p Rwyatt
 * Reset heap
 * 
 * 110   8/19/98 1:35p Rwyatt
 * VC6.0 Warning fixes
 * 
 * 109   8/15/98 6:05p Mmouni
 * Changed to be compatible with VC6.0.
 * 
 * 108   98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 107   8/11/98 6:18p Pkeet
 * Removed the settings structure.
 * 
 * 106   8/03/98 4:32p Pkeet
 * Now vertices are jiggled only when hardware is in use and the z buffer is not.
 * 
 * 105   98.08.02 7:23p Mmouni
 * Changed to no longer load max to sort.
 * 
 * 104   98.07.01 6:35p Mmouni
 * Changed rNearZScale.
 * 
 * 103   5/20/98 2:33p Pkeet
 * Commented out an assert.
 * 
 * 102   5/12/98 3:05p Pkeet
 * Added hack to fix crash bug.
 * 
 * 101   98.04.01 7:28p Mmouni
 * Added save/load methods to CPlaneToleranceSetting.
 * 
 * 100   3/16/98 4:18p Pkeet
 * Changed the maximum number of polygons that can be depth sorted to 600.
 * 
 * 99    3/16/98 3:49p Pkeet
 * Added code to not sort distant polygons.
 * 
 * 98    3/10/98 1:21p Pkeet
 * Added include to "LocalArray.hpp."
 * 
 * 97    98/03/05 13:29 Speter
 * Commented out Asserts, which trigger under D3D on my Riva.
 * 
 * 96    98.02.06 7:43p Mmouni
 * Added code to make sure split polygons do not have the bMovedInList flag set.
 * 
 * 95    98.02.05 4:48p Mmouni
 * Made 3dx optimized version of sort.
 * 
 * 94    2/04/98 9:25p Mmouni
 * Fixed incorrect assertion in the depth sort.
 * 
 * 93    98.02.04 4:59p Mmouni
 * Optimized partition building algorithm.
 * Did x86 assembly optimization of most of the sort.
 * Added settable binary/quad partition parameters.
 * 
 * 92    1/25/98 3:41p Pkeet
 * Only polygons that are above a threshold size are sorted. Polygons that do not have the
 * 'bAccept' flag set to true are not pushed onto the render list.
 * 
 * 91    1/24/98 3:20p Pkeet
 * Polygons are removed or just simple sorted based on their screen area.
 * 
 * 90    98/01/16 12:33 Speter
 * Increased heap size.
 * 
 * 89    1/09/98 2:40p Pkeet
 * Added an explicit clip adjust value that can be changed from video card to video card.
 * 
 * 88    1/07/98 7:08p Pkeet
 * Upped the adjust value.
 * 
 * 87    1/07/98 4:05p Pkeet
 * Added the clip region function.
 * 
 * 86    98.01.05 5:59p Mmouni
 * Added fast TreeList from sorted array routine.
 * 
 * 85    12/23/97 2:18p Pkeet
 * Now sets the 'fAdjustOut' parameter.
 * 
 * 84    12/21/97 10:59p Rwyatt
 * Depth sort FastHeap now takes a heapsize and an initial commit size, the depth sort fast heap
 * is passed to the tree list allocator so the block can be extended if required.
 * This saves nearly 4Mb of physical memory
 * 
 * 83    12/16/97 2:57p Mlange
 * Removed assert.
 * 
 * 82    1/23/96 6:23p Pkeet
 * Added comments for the 3DX version.
 * 
 * 81    97/11/16 6:28a Pkeet
 * Changed comments.
 * 
 * 80    11/10/97 10:04p Gfavor
 * Added FEMMS's around bCorrectOrder and bReversePQ calls.
 * 
 * 79    11/09/97 10:04p Gfavor
 * Eliminated special case code for using FastMake3DX.
 * 
 * 78    97/11/07 8:11p Pkeet
 * Fixed bug with render caches.
 * 
 * 77    97/11/07 12:26p Pkeet
 * No sorting is done for polygons that lay in the last few fog bands.
 * 
 * 76    97/11/07 12:01p Pkeet
 * Inlined the call to the 3DX version of 'FastMake.'
 * 
 * 75    11/04/97 7:00p Gfavor
 * Enabled usage of SetPlane3DX
 * 
 * 74    10/29/97 8:05p Gfavor
 * Added 3DX support.
 * 
 * 73    97/10/23 10:57a Pkeet
 * Added a K6 3D switch.
 * 
 * 72    97/08/13 2:40p Pkeet
 * Provided a temporary fix for the mystery infinite loop bug.
 * 
 * 71    97/07/30 1:47p Pkeet
 * Added support in the settings structure for a separate tolerance calculation for terrain
 * polygons. Added default values for terrain tolerance calculations.
 * 
 * 70    97/07/23 19:00 Speter
 * Replaced CCamera argument to clipping functions with b_perspective flag.
 * 
 * 69    6/25/97 7:28p Mlange
 * Now uses the (fast) float to integer conversion functions.
 * 
 * 68    6/19/97 2:53p Mlange
 * Moved Profile.hpp to Lib/Sys.
 * 
 * 67    97/06/11 12:31 Speter
 * Added compile switch for calculating screen extents in pipeline.
 * 
 * 66    97/06/10 15:48 Speter
 * Now takes camera as an argument (for new vertex projection).  
 * 
 * 65    97/05/28 6:20p Pkeet
 * Fixed debug compile bug.
 * 
 * 64    97/05/28 5:17p Pkeet
 * Changed tolerances again.
 * 
 * 63    97/05/28 4:56p Pkeet
 * Changed defaults.
 * 
 * 62    97/05/28 2:54p Pkeet
 * Optimized assembly with VTune.
 * 
 * 61    97/05/27 6:22p Pkeet
 * Changed tolerance defaults.
 * 
 * 60    97/05/27 5:44p Pkeet
 * Added code to build more efficient quad partitions.
 * 
 * 59    97/05/27 3:57p Pkeet
 * Optimized building a tree from an array further.
 * 
 * 58    97/05/27 2:59p Pkeet
 * Fixed minor bug.
 * 
 * 57    97/05/27 2:49p Pkeet
 * Added some asserts, cleaned up code a little.
 * 
 * 56    97/05/27 2:40p Pkeet
 * Made the building of subtrees faster.
 * 
 * 55    97/05/26 5:32p Pkeet
 * Fixed bug in polygon limiter.
 * 
 * 54    97/05/26 4:18p Pkeet
 * Added the 'iMaxToDepthsort' parameter.
 * 
 * 53    97/05/26 3:46p Pkeet
 * Increased tree node memory assigned to depthsorting.
 * 
 * 52    97/05/26 2:57p Pkeet
 * Now uses a fast heap instead of _alloca for memory for tree nodes.
 * 
 * 51    97/05/24 3:58p Pkeet
 * Added the new fpu state object with truncation. Cleaned up code a bit.
 * 
 * 50    97/05/23 6:45p Pkeet
 * Made use of the 'CFPUState' object in the initialization code.
 * 
 * 49    97/05/23 6:26p Pkeet
 * Moved the tolerance object this module to the header. Added code to changed the FPU state.
 * 
 * 48    97/05/22 4:06p Pkeet
 * Fixed bug.
 * 
 * 47    97/05/22 4:00p Pkeet
 * Made 'InitializePolygonForDepthSort' return a boolean value to indicate if the polygon is
 * usable or has been culled.
 * 
 * 46    97/05/21 4:13p Pkeet
 * Added 'fPixelBuffer.'
 * 
 * 45    97/05/21 10:40a Pkeet
 * Added code for using the inverse Z tolerance fall off.
 * 
 * 44    97/05/20 3:15p Pkeet
 * Added the variable for the cutting off depth sorting.
 * 
 * 43    97/05/20 11:41a Pkeet
 * Added variable plane tolerances for depth sorting.
 * 
 * 42    97/05/17 3:23p Pkeet
 * Fixed bug that caused infinite looping.
 * 
 * 41    97/05/16 7:10p Pkeet
 * Optimized tree building.
 * 
 * 40    97/05/16 2:47p Pkeet
 * Further optimizations using the key.
 * 
 * 39    97/05/15 5:42p Pkeet
 * Further optimizations.
 * 
 * 38    97/05/15 5:03p Pkeet
 * Now uses placement information for iterators.
 * 
 * 37    97/05/14 8:12p Pkeet
 * Fixed bug in multiple list use.
 * 
 * 36    97/05/14 4:03p Pkeet
 * Replaced the STL map with the binary tree and list template.
 * 
 * 35    97/05/08 1:01p Pkeet
 * Created the sort list class.
 * 
 * 34    97/05/06 6:59p Pkeet
 * Implemented an STL map version of the depth sort using a key. Added an interface for
 * obtaining the screen width and height.
 * 
 * 33    97/05/06 10:45a Pkeet
 * Moved construction from the constructor.
 * 
 * 32    97/05/05 5:25p Pkeet
 * Removed use of the STL 'set' class for initial sorting.
 * 
 * 31    97/04/30 10:50a Pkeet
 * Optimized reinsertion after polygon splitting.
 * 
 * 30    97/04/29 4:55p Pkeet
 * Added include for the pipeline heap. Moved the test stats counter to the correct location.
 * 
 * 29    97/04/29 2:17p Pkeet
 * Turned depth sorting into an object.
 * 
 * 28    97/04/28 5:49p Pkeet
 * Moved plane test code and statistics reporting code to other modules. Reorganized clipping
 * tests so the clipping code does the intersection test. Now uses the fMinZ and fMaxZ member
 * variables of CRenderPolygon. Now explicitly sets the plane equation for each initialized
 * polygon. The Assert function has been moved to the helper module. Plane tests are now done in
 * ScreenRender.cpp.
 * 
 * 27    97/04/25 6:43p Pkeet
 * Removed degenerate code checking.
 * 
 * 26    97/04/25 6:41p Pkeet
 * Numerous trivial optimizations.
 * 
 * 25    97/04/25 2:20p Pkeet
 * Added a flag for locking the camera. Removed use of the degenerate polygon test. Added some
 * debug stuff to the Assert order function.
 * 
 * 24    97-04-21 17:11 Speter
 * Updated for new plane classes and functions.  Changed optcCam to v3Cam.
 * 
 * 23    97/04/21 10:53a Pkeet
 * Added code to find degenerate polygons.
 * 
 * 22    97/04/17 4:11p Pkeet
 * Added the 'bDegenerate' function.
 * 
 * 21    97/04/17 11:34a Pkeet
 * Added debug tools to the Assert function.
 * 
 * 20    97/04/17 10:19a Pkeet
 * Added a fuzzy overlap test in 2D screen space.
 * 
 * 19    97/04/16 6:25p Pkeet
 * Added the 'bAssertOrder' function.
 * 
 * 18    97/04/16 3:34p Pkeet
 * Fixed bug involving fuzziness values.
 * 
 * 17    97/04/16 2:23p Pkeet
 * Added toggle between old and new clipper.
 * 
 * 16    97/04/15 2:38p Pkeet
 * Added the 'fDEFAULT_PLANE_TOLERANCE' definition.
 * 
 * 15    97/04/15 1:51p Pkeet
 * Added use of the 'u4FromFloat' macro.
 * 
 * 14    97-04-14 20:28 Speter
 * Changed includes to correspond to new module.
 * 
 * 13    97/04/14 3:46p Pkeet
 * Fixed some bugs.
 * 
 * 12    97/04/14 11:56a Pkeet
 * Debugged the 'Watermark' optimization.
 * 
 * 11    97/04/13 4:47p Pkeet
 * Fixed minor bug.
 * 
 * 10    97/04/13 2:33p Pkeet
 * Moved 2D intersection code to "Line.cpp." Changed 'iRectangles' to 'iNumPolygons.' Added
 * 'iNumInitialize.' Removed use of the 'u4FromFloat' macro until the code is stable and
 * debugged. Now using the optcCam.tY value instead of the v3Screen.tZ value because it is
 * easier to work with. Removed the 'BuildPPolyArray' function. Moved the
 * 'InitializePolygonForSort' function to the CRenderPolygon class. Uses 'push_front' instead of
 * 'push_back' to add things to the depth sort list. Changed 'bOverlapBoxes' to
 * 'bOverlapBoxesScreen' and modified the function slightly.  Changed the polygon intersection,
 * opposite side of camera and same side of camera to use the same code. Implemented the
 * 'bSplit' function and a function to reinsert a polygon into the list in an ordered position
 * (this function is currently #ifdef'd disabled). Currently all optimizations are disabled.
 * Added code so that if P can't be split by Q's plane, Q may be split by P's plane.
 * 
 * 9     97/04/09 10:32a Pkeet
 * Added optimization where the 'P' test loop terminates when no more overlaps can occur in the
 * Z extent.
 * 
 * 8     97/04/08 7:05p Pkeet
 * Removed unused esf flag enum types.
 * 
 * 7     97/04/08 2:49p Pkeet
 * Fixed bug in reinsertion code.
 * 
 * 6     97/04/08 2:37p Pkeet
 * Implemented all tests except for the clipping and optimized z extent test.
 * 
 * 5     97/04/08 12:12p Pkeet
 * Added test 5. Added new stats information.
 * 
 * 4     97/04/07 7:59p Pkeet
 * Implemented steps 3 and 4 of the ambiguity resolution tests according to Foley, Van Dam et
 * al.
 * 
 * 3     97/04/07 4:20p Pkeet
 * Added functions to build a sorted STL list and functions to trivially rejected polygons with
 * screen space bounding boxes that don't overlap.
 * 
 * 2     97/04/06 1:52p Pkeet
 * Added function to write depthsort stats.
 * 
 * 1     97/04/05 2:49p Pkeet
 * Initial implementation.
 *
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "DepthSort.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "PipeLineHeap.hpp"
#include "Camera.hpp"
#include "Line.hpp"
#include "DepthSortTools.hpp"
#include "AsmSupport.hpp"
#include "Lib/Renderer/Fog.hpp"
#include "Lib/Renderer/Clip.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/QualitySettings.hpp"

#include "Algo.h"
#include <Malloc.h>

bool bNoCPPSubtriangle = false;

#define DEPTH_SORT_STATS  0

#if (DEPTH_SORT_STATS)

CProfileStat		psSetup			("Setup", &proProfile.psPresort);
CProfileStat		psPlaneSetup	("PlaneSetup", &proProfile.psPresort);
CProfileStat		psQuickSort		("QuickSort", &proProfile.psPresort);
CProfileStat		psBuildQuad		("BuildQuad", &proProfile.psPresort);
CProfileStat		psPushAll		("PushAll", &proProfile.psPresort);
CProfileStat		psDepthSort		("DepthSort", &proProfile.psPresort);
CProfileStat		psSplit			("Split", &psDepthSort);

#endif

//
// Defines.
//

// Type of tree node.
typedef CTreeList<CSortKey, CRenderPolygon*>::TNode TTreeNode;

// The maximum number of polygons that can be allocated.
#define iNUM_MAX_POLYSTOSORTHEAP (1 << 18)	// 256K tree nodes is the reserved heap size
#define iNUM_MAX_POLYSTOSORT (1 << 13)		// poly tree nodes initially allocated = 4096, 2048 polys

// Use polygon area as a threshold for performing the sort or not.
#define bUSE_AREA_P    (1)
#define bUSE_AREA_Q    (0)

#if bUSE_AREA_P
	const float fMinAreaForSort = 2.0f;
#endif // bUSE_AREA_P


//
// Fast heap for depthsorting.
//

// Polygon pointer heap.
CFastHeap fhPPolys(iNUM_MAX_POLYSTOSORTHEAP * sizeof(TTreeNode));


//
// Inline functions.
//

//**********************************************************************************************
//
inline bool operator==
(
	const CSortKey& key_0,
	const CSortKey& key_1
)
//
// Returns 'true' if key 0 equals key 1.
//
//**************************************
{
	return key_0.u4SubKey == key_1.u4SubKey;
}


//
// Macros.
//

// Switch to use ASM in this module.
#define bUSE_ASM_LOCALLY (VER_ASM)

// Switch to include performance counters in this module.
#define bUSE_COUNTERS    (0)


//
// Depth sort object definition.
//

//******************************************************************************************
//
class CDepthSort
//
// Depth sorting object.
//
// Prefix: depth
//
//**************************************
{
public:

	typedef CSortList::iterator TIterator;
	typedef CSortList::binary_iterator TBinaryIterator;
	typedef CSortList::TNode TTNode;

public:

	TReal          rMaxLevel;	// Maximum "watermark" for Z.
	CPipelineHeap* pplhHeap;	// Pointer to the calling heap.
	const CCamera* pcamCamera;	// Pointer to the projecting camera.
	CSortList      slMain;		// Linked list of polygons.
	CSortList      pslParts[iMAX_SORT_LISTS];
	int            iNumSortedLists;
	int            iWidth;
	int            iHeight;

public:

	//**************************************************************************************
	//
	// Constructor.
	//

	// Initializing constructor.
	CDepthSort(CPipelineHeap* pplh_heap, const CCamera* pcam, int i_width, int i_height);


	//**************************************************************************************
	//
	// Public member functions.
	//

	//**************************************************************************************
	//
	void PushFront
	(
		CRenderPolygon* prpoly	// Pointer to the polygon to add.
	)
	//
	// 
	//
	//**************************************
	{
		Assert(prpoly);

		// Make sure the watermark won't produce false results.
		prpoly->rMaxLevel = rMaxLevel;

		// Set key to the end of the list.
		prpoly->keySort.SetKeyDown();

		// Insert polygon at the front of the list.
		prpoly->pvNodeItMain = (void*)slMain.push_front(prpoly->keySort, prpoly).pnodeCurrent;

#if (iMAX_SORT_LISTS > 1)
		for (int i = 0; i < prpoly->iNumSortLists; i++)
			prpoly->pvNodeIt[i] = (void*)prpoly->pslMemberOf[i]->push_front(prpoly->keySort,
			                             prpoly).pnodeCurrent;
#endif
	}

	//**************************************************************************************
	//
	void PushAll
	(
		CRenderPolygon* aprpoly[],	// Pointer to the polygon array to add.
		int i_num
	);
	//
	// 
	//
	//**************************************

	//**************************************************************************************
	//
	void InsertSublist
	(
		CRenderPolygon* prpoly	// Pointer to the polygon to add.
	)
	//
	// 
	//
	//**************************************
	{
		Assert(prpoly);

#if (iMAX_SORT_LISTS > 1)
		for (int i = 0; i < prpoly->iNumSortLists; i++)
			prpoly->pvNodeIt[i] = (void*)prpoly->pslMemberOf[i]->insert(prpoly->keySort,
			                             prpoly).pnodeCurrent;
#endif
	}

	//**************************************************************************************
	//
	void Insert
	(
		CRenderPolygon* prpoly	// Pointer to the polygon to add.
	)
	//
	// 
	//
	//**************************************
	{
		Assert(prpoly);

		// Insert polygon at the front of the list.
		prpoly->pvNodeItMain = (void*)slMain.insert(prpoly->keySort, prpoly).pnodeCurrent;

#if (iMAX_SORT_LISTS > 1)
		for (int i = 0; i < prpoly->iNumSortLists; i++)
			prpoly->pvNodeIt[i] = (void*)prpoly->pslMemberOf[i]->insert(prpoly->keySort,
			                             prpoly).pnodeCurrent;
#endif
	}

	//**************************************************************************************
	//
	void CopyListInfo
	(
		CRenderPolygon* prpoly_to,
		CRenderPolygon* prpoly_from
	)
	//
	// 
	//
	//**************************************
	{
		Assert(prpoly_to);
		Assert(prpoly_from);

		// Copy replacement information.
		prpoly_to->rMaxLevel     = prpoly_from->rMaxLevel;
		prpoly_to->keySort       = prpoly_from->keySort;

		prpoly_to->pvNodeItMain  = prpoly_from->pvNodeItMain;

#if (iMAX_SORT_LISTS > 1)
		prpoly_to->iNumSortLists = prpoly_from->iNumSortLists;
		for (int i = 0; i < prpoly_from->iNumSortLists; i++)
		{
			 prpoly_to->pslMemberOf[i] = prpoly_from->pslMemberOf[i];
			 prpoly_to->pvNodeIt[i]    = prpoly_from->pvNodeIt[i];
		}
#endif
	}

	//**************************************************************************************
	//
	void Replace
	(
		CRenderPolygon* prpoly,			// Pointer to the polygon to add.
		CRenderPolygon* prpoly_replace	// Pointer to the polygon containing placement
										// information.
	)
	//
	// 
	//
	//**************************************
	{
		Assert(prpoly);
		Assert(prpoly_replace);

		CopyListInfo(prpoly, prpoly_replace);

		// Replace the position in the main list.
		((CSortList::TNode*)(prpoly->pvNodeItMain))->Data = prpoly;

#if (iMAX_SORT_LISTS > 1)
		// Insert polygon into lists it is a member of.
		for (int i = 0; i < prpoly->iNumSortLists; i++)
			((CSortList::TNode*)(prpoly->pvNodeIt[i]))->Data = prpoly;
#endif
	}

	//**************************************************************************************
	//
	bool bExists
	(
		CRenderPolygon* prpoly	// Pointer to the polygon to add.
	)
	//
	// 
	//
	//**************************************
	{
		Assert(prpoly);

		if (slMain[prpoly->keySort])
			return true;

		// Insert polygon into lists it is a member of.
		for (int i = 0; i < iNumSortedLists; i++)
			if (pslParts[i][prpoly->keySort])
				return true;
		return false;
	}

	//**************************************************************************************
	//
	void SetMemberData
	(
		CRenderPolygon* prpoly	// Pointer to the polygon to add.
	)
	//
	// Sets the membership data for the polygon.
	//
	//**************************************
	{
#if (iMAX_SORT_LISTS > 1)
		// Initialize the polygon for data.
		prpoly->iNumSortLists = 0;

		// Iterate through all sublists present.
		for (int i = 0; i < iNumSortedLists; i++)
		{
			pslParts[i].SetMemberData(prpoly);
		}
#endif
	}

	//**************************************************************************************
	//
	void AddSublist
	(
		int i_x0, int i_y0, int i_x1, int i_y1
	);
	//
	// Adds a new sublist to the list of sublists.
	// 
	//**************************************

	//**************************************************************************************
	//
	void QuadPartition
	(
		int i_x,
		int i_y_left,
		int i_y_right
	);
	//
	// Adds four new sublists to the list of sublists.
	// 
	//**************************************

	//**************************************************************************************
	//
	void BinaryPartition
	(
		int i_x
	);
	//
	// Adds two new sublists to the list of sublists.
	// 
	//**************************************

	//**************************************************************************************
	//
	void Sort
	(
	);
	//
	// Iterate through the roughly sorted list and push fully sorted polygons back onto
	// the pipeline heap.
	// 
	//**************************************

	//**************************************************************************************
	//
	void SetMaxLevel
	(
		CRenderPolygon* prpoly	// Pointer to the polygon to add.
	)
	//
	// 
	//
	//**************************************
	{
		Assert(prpoly);

		// Get the current watermark for the polygon.
		float f_new_level = prpoly->rMaxLevel;

		// Increase the watermark as required.
		if (u4FromFloat(rMaxLevel) < u4FromFloat(f_new_level))
			rMaxLevel = f_new_level;

		// Make sure the watermark won't produce false results.
		prpoly->rMaxLevel = rMaxLevel;
	}

	//**************************************************************************************
	//
	void InitializePolygon
	(
		CRenderPolygon* prpoly	// Pointers to the polygon to initialize.
	)
	//
	// Sets the membership data, the maximum Z level and the key data for the polygon.
	//
	//**************************************
	{
		Assert(prpoly);

		SetMaxLevel(prpoly);
		prpoly->keySort.SetKeyDown();

#if (iMAX_SORT_LISTS > 1)
		prpoly->iNumSortLists = 0;
#endif
	}

private:

	//**************************************************************************************
	//
	// Private member functions.
	//

	//**************************************************************************************
	//
	void Erase
	(
		CRenderPolygon* prpoly	// Pointer to the polygon to add.
	)
	//
	// 
	//
	//**************************************
	{
		Assert(prpoly);

		// Replace the position in the main list.
		slMain.erase((CSortList::TNode*)prpoly->pvNodeItMain);

#if (iMAX_SORT_LISTS > 1)
		// Remove polygon from lists it is a member of.
		for (int i = 0; i < prpoly->iNumSortLists; i++)
			prpoly->pslMemberOf[i]->erase((CSortList::TNode*)prpoly->pvNodeIt[i]);
#endif
	}

	//**************************************************************************************
	//
	void PushBack
	(
		CRenderPolygon* prpoly	// Pointer to the polygon to add.
	)
	//
	// 
	//
	//**************************************
	{
		Assert(prpoly);

		// Make sure the watermark won't produce false results.
		SetMaxLevel(prpoly);

		// Set key to the end of the list.
		prpoly->keySort.SetKeyUp();

		// Put the polygon into the list.
		Insert(prpoly);
	}

	//******************************************************************************************
	//
	void InsertNew
	(
		CRenderPolygon* prpoly
	)
	//
	// Inserts the polygon into its sorted position in the polygon list.
	//
	//**********************************
	{
		Assert(prpoly);

		// Select an appropriate iterator.
		TIterator it_poly = itSelectQList(prpoly);

		// If the list is empty, just push it to the front.
		if (it_poly)
		{
			// Find the crossover point in Z.
			uint32 u4MaxZ = u4FromFloat(prpoly->fMaxZ);
			for (; it_poly; ++it_poly)
				if (u4FromFloat((*it_poly)->fMaxZ) < u4MaxZ)
					goto POSITION_FOUND;

			// If no Z was found, push back.
			PushBack(prpoly);
			return;

	POSITION_FOUND:

			CRenderPolygon* prpoly_insert = it_poly.pnodeCurrent->Data;
			it_poly.pnodeCurrent = (CSortList::TNode*)prpoly_insert->pvNodeItMain;

			// Move the iterator back to the position before it crossed over in Z.
			--it_poly;
			if (it_poly)
				if (prpoly->keySort.bSetKeyMid((*it_poly)->keySort, prpoly_insert->keySort))
				{
					if (!slMain[prpoly->keySort])
					{
						// For now, rely on the keys.
						prpoly->rMaxLevel = rMaxLevel;
						Insert(prpoly);
						return;
					}
				}
		}
		
		PushFront(prpoly);
	}

	//******************************************************************************************
	//
	bool bSplitPByQ
	(
		CRenderPolygon*& prpoly_p,	// Pointer to render polygon 'P.'
		CRenderPolygon*& prpoly_q	// Pointer to render polygon 'Q.'
	)
	//
	// Returns 'true' if polygon p is split by q or q is split by p.
	//
	//**********************************
	{
		CRenderPolygon* prpoly_pa;
		CRenderPolygon* prpoly_pb;
		//CSortKey key = prpoly_p->keySort;

		// Try and split P by Q's plane first.
		if (bSplit(prpoly_q, prpoly_p, prpoly_pa, prpoly_pb, *pplhHeap, 
			pcamCamera->campropGetProperties().bPerspective))
		{
			// Replace P with one of the new pieces.
			CopyListInfo(prpoly_pa, prpoly_p);
			prpoly_p = prpoly_pa;

			// Set the membership data for the new polygons.
			SetMemberData(prpoly_pb);

			// Sort the pieces of P back into the list.
			InsertNew(prpoly_pb);
			/*
			prpoly_pb->rMaxLevel = rMaxLevel;
			prpoly_pb->keySort   = key;
			PushFront(prpoly_pb);
			*/

			// Indicate that Q is once again moveable.
			prpoly_q->bMovedInList = false;

			// P's should be moveable.
			prpoly_pa->bMovedInList = false;
			prpoly_pb->bMovedInList = false;

			return true;
		}
		return false;
	}

	//******************************************************************************************
	//
	bool bSplitQByP
	(
		CRenderPolygon* prpoly_p,	// Pointer to render polygon 'P.'
		CRenderPolygon* prpoly_q	// Pointer to render polygon 'Q.'
	)
	//
	// Returns 'true' if polygon p is split by q or q is split by p.
	//
	//**********************************
	{
		CRenderPolygon* prpoly_qa;
		CRenderPolygon* prpoly_qb;

		// Try and split Q by P's plane.
		if (bSplit(prpoly_p, prpoly_q, prpoly_qa, prpoly_qb, *pplhHeap, 
			pcamCamera->campropGetProperties().bPerspective))
		{
			CSortKey key_q = prpoly_q->keySort;

			// Iterate to the next key.
			TIterator it_next((CSortList::TNode*)prpoly_q->pvNodeItMain);
			++it_next;

			Replace(prpoly_qa, prpoly_q);

			// Set the membership data for the new polygons.
			SetMemberData(prpoly_qb);

			// Indicate that P is once again moveable.
			prpoly_p->bMovedInList = false;

			// Q's should be moveable.
			prpoly_qa->bMovedInList = false;
			prpoly_qb->bMovedInList = false;

			// Insert the pieces into the list.
			if (!it_next)
			{
				// Insert the pieces into the list.
				PushBack(prpoly_qb);
			}
			else
			{
				if (prpoly_qb->keySort.bSetKeyMid(key_q, (*it_next)->keySort))
				{
					if (!slMain[prpoly_qb->keySort])
					{
						SetMaxLevel(prpoly_qb);
						Insert(prpoly_qb);

						// Indicate success.
						return true;
					}
				}
				PushFront(prpoly_qb);
			}
			return true;
		}
		return false;
	}

	//******************************************************************************************
	//
	bool bReversePQ
	(
		CRenderPolygon* prpoly_p,	// Pointer to render polygon 'P.'
		CRenderPolygon* prpoly_q	// Pointer to render polygon 'Q.'
	)
	//
	// Returns 'true' if Q is reordered after P.
	//
	// Notes:
	//		If all these tests have failed, it is assumed that P obscures Q. Therefore retry
	//		steps 3 and 4 with P and Q reversed (steps 1, 2 and 5 need not be repeated). If the
	//		tests succeed, Q replaces P in the list. To prevent infinite looping, Q is marked
	//		before moving. If Q is already marked, this test is skipped.
	//
	//**********************************
	{
		if (prpoly_q->bMovedInList || !prpoly_p->bInFrontOf(prpoly_q))
			return false;

		// Mark Q.
		prpoly_q->bMovedInList = true;

		// Remove Q from its current position in the list.
		Erase(prpoly_q);

		// Reinsert P and Q into the list, with Q replacing P's position.
		PushFront(prpoly_p);
		PushFront(prpoly_q);

		// Increment stats counter.
		iNumReinserted++;

		// P cannot be scan converted yet.
		return true;
	}

	//******************************************************************************************
	//
	bool bCorrectOrder
	(
		CRenderPolygon* prpoly_p,	// Pointer to render polygon 'P.'
		CRenderPolygon* prpoly_q	// Pointer to render polygon 'Q.'
	)
	//
	// Returns 'true' if P is behind Q or if the order is irrelevant.
	//
	// Notes:
	//		Steps 1 through 5 are incorporated in this function.
	//
	//**********************************
	{
		// Steps 2: trivial rejection.
		if (prpoly_p->bBehind(prpoly_q))
			return true;

		// Step 5: Do the 2D projections of P and Q intersect?
		if (!bIntersect2D(prpoly_p, prpoly_q))
			return true;

		// Order is incorrect.
		return false;
	}

	//**************************************************************************************
	//
	CRenderPolygon* prpolyNextP
	(
	)
	//
	// Returns a pointer to the next polygon in the list after removing the pointer from
	// the list.
	// 
	//**************************************
	{
		// Get the next polygon from the main list.
		CRenderPolygon* prpoly = slMain.pop();

		// Return a null pointer if there are no polygons left.
		if (!prpoly)
			return 0;

#if (iMAX_SORT_LISTS > 1)
		// Remove the polygon from lists it is a member of.
		for (int i = 0; i < prpoly->iNumSortLists; i++)
			prpoly->pslMemberOf[i]->pop();
#endif

		// Return a pointer to the polygon.
		return prpoly;
	}

	//**************************************************************************************
	//
	CSortList::iterator itSelectQList
	(
		CRenderPolygon* prpoly
	)
	//
	// 
	//
	//**************************************
	{
		Assert(prpoly);

#if (iMAX_SORT_LISTS > 1)
		if (prpoly->iNumSortLists == 1)
		{
			// Use the single Q list.
			return prpoly->pslMemberOf[0]->begin();
		}
		else
#endif
		{
			// Use the master list.
			return slMain.begin();
		}
	}

};


//
// Class function implementations.
//

//**********************************************************************************************
//
// CSortList implementation.
//

	//******************************************************************************************
	//
	// CSortList member functions.
	//
	
	//******************************************************************************************
	void CSortList::PushArray(CRenderPolygon* aprpoly[], uint u_num, iterator it)
	{
		switch (u_num)
		{
			case 0:
				return;
			case 1:
				it = insert_fast(aprpoly[0]->keySort, aprpoly[0], it);
				aprpoly[0]->pvNodeItMain = (void*)it.pnodeCurrent;
				return;
			case 2:
				it = insert_fast(aprpoly[0]->keySort, aprpoly[0], it);
				aprpoly[0]->pvNodeItMain = (void*)it.pnodeCurrent;
				it = insert_fast(aprpoly[1]->keySort, aprpoly[1], it);
				aprpoly[1]->pvNodeItMain = (void*)it.pnodeCurrent;
				return;
			case 3:
				// Insert the middle node.
				it = insert_fast(aprpoly[1]->keySort, aprpoly[1], it);
				aprpoly[1]->pvNodeItMain = (void*)it.pnodeCurrent;

				// Create new nodes from the allocator memory.
				TNode* pnode_less    = NewNode(aprpoly[2]->keySort, aprpoly[2]);
				TNode* pnode_greater = NewNode(aprpoly[0]->keySort, aprpoly[0]);
				iNumNodes += 2;

				// Set the main pointers.
				aprpoly[2]->pvNodeItMain = (void*)pnode_less;
				aprpoly[0]->pvNodeItMain = (void*)pnode_greater;

				// Set the parent pointers.
				pnode_less->pnodeParent    = it.pnodeCurrent;
				pnode_greater->pnodeParent = it.pnodeCurrent;

				// Insert the lesser and greater nodes.
				it.pnodeCurrent->pnodeLess = pnode_less;
				it.pnodeCurrent->pnodeGreater = pnode_greater;
				return;
		}

		// Find the midpoint and insert it.
		uint u_mid = u_num >> 1;

		// Insert in the main list.
		it = insert_fast(aprpoly[u_mid]->keySort, aprpoly[u_mid], it);
		aprpoly[u_mid]->pvNodeItMain = (void*)it.pnodeCurrent;

		// Push the array before this point.
		PushArray(aprpoly, u_mid, it);

		// Push the array after this point.
		PushArray(&aprpoly[u_mid + 1], u_num - u_mid - 1, it);
	}
	
	//******************************************************************************************
	void CSortList::PushArray(CRenderPolygon* aprpoly[], uint u_num)
	{
		// Stop the recursive insertion if only a few polygons are left.
		if (u_num < 2)
		{
			for (uint u = 0; u < u_num; u++)
			{
				iterator it_new = insert(aprpoly[u]->keySort, aprpoly[u]);
				aprpoly[u]->pvNodeItMain = (void*)it_new.pnodeCurrent;
			}
			return;
		}

		// Find the midpoint and insert it.
		uint u_mid = u_num >> 1;

		// Insert in the main list.
		iterator it = insert(aprpoly[u_mid]->keySort, aprpoly[u_mid]);
		aprpoly[u_mid]->pvNodeItMain = (void*)it.pnodeCurrent;

		// Push the array before this point.
		PushArray(aprpoly, u_mid, it);

		// Push the array after this point.
		PushArray(&aprpoly[u_mid + 1], u_num - u_mid - 1, it);
	}

	//******************************************************************************************
	void CSortList::BuildFromSortedList(CRenderPolygon* aprpoly[], uint u_num)
	{
		int i;
		TNode *pnode_parent;
		TNode *pnode_lesser;
		TNode *pnode_greater;

		// Don't build from the bottom up if the tree is too small.
		if (u_num < 4 || iNumNodes)
		{
			PushArray(aprpoly, u_num);
			return;
		}

		// Find the height of the tree.
		int levels = 0;
		for (int temp = u_num; temp > 0; temp -= (1 << (levels-1)))
		{
			levels++;
		}

		// Current level.
		int level = 1;

		// Build the bottom two levels.
		for (i = 1; i < u_num-1; i += 4)
		{
			// This node is at i.
			pnode_parent = AllocateNode();
			pnode_parent->Key			= aprpoly[i]->keySort;
			pnode_parent->Data			= aprpoly[i];
			pnode_parent->bSkip			= false;
			aprpoly[i]->pvNodeItMain	= (void*)pnode_parent;

			// Greater child is at [i-1].
			pnode_greater = AllocateNode();
			pnode_greater->Key			= aprpoly[i-1]->keySort;
			pnode_greater->Data			= aprpoly[i-1];
			pnode_greater->bSkip		= false;
			pnode_greater->pnodeParent	= pnode_parent;
			pnode_greater->pnodeLess	= 0;
			pnode_greater->pnodeGreater	= 0;
			aprpoly[i-1]->pvNodeItMain	= (void*)pnode_greater;

			// Lesser child is at [i+1].
			pnode_lesser = AllocateNode();
			pnode_lesser->Key			= aprpoly[i+1]->keySort;
			pnode_lesser->Data			= aprpoly[i+1];
			pnode_lesser->bSkip			= false;
			pnode_lesser->pnodeParent	= pnode_parent;
			pnode_lesser->pnodeLess     = 0;
			pnode_lesser->pnodeGreater  = 0;
			aprpoly[i+1]->pvNodeItMain	= (void*)pnode_lesser;

			pnode_parent->pnodeGreater  = pnode_greater;
			pnode_parent->pnodeLess     = pnode_lesser;
		}

		// Finish any left-overs.  We may have 0, 1 or 2 nodes left.
		if (i < u_num)
		{
			// Two nodes left.

			// This node is at i.
			pnode_parent = AllocateNode();

			pnode_parent->Key			= aprpoly[i]->keySort;
			pnode_parent->Data			= aprpoly[i];
			pnode_parent->bSkip			= false;
			aprpoly[i]->pvNodeItMain	= (void*)pnode_parent;

			// Greater child is at [i-1].
			pnode_greater = AllocateNode();
			pnode_greater->Key			= aprpoly[i-1]->keySort;
			pnode_greater->Data			= aprpoly[i-1];
			pnode_greater->bSkip		= false;
			pnode_greater->pnodeParent	= pnode_parent;
			pnode_greater->pnodeLess	= 0;
			pnode_greater->pnodeGreater	= 0;
			aprpoly[i-1]->pvNodeItMain	= (void*)pnode_greater;

			pnode_parent->pnodeGreater  = pnode_greater;
			pnode_parent->pnodeLess     = 0;
		}
		else if (i-1 < u_num)
		{
			// One node left.

			// This node is at i-1.
			pnode_parent = AllocateNode();
			pnode_parent->Key			= aprpoly[i-1]->keySort;
			pnode_parent->Data			= aprpoly[i-1];
			pnode_parent->bSkip			= false;
			pnode_parent->pnodeLess     = 0;
			pnode_parent->pnodeGreater  = 0;
			aprpoly[i-1]->pvNodeItMain	= (void*)pnode_parent;
		}

		// Build higher levels.
		level++;

		while (level < levels-1)
		{
			// The step for child nodes.
			const int step = (1 << (level-1));

			for (i = (1 << level)-1; i < u_num - step; i += (1 << (level+1)))
			{
				// This node is at i.
				pnode_parent = AllocateNode();
				pnode_parent->Key	= aprpoly[i]->keySort;
				pnode_parent->Data	= aprpoly[i];
				pnode_parent->bSkip	= false;
				aprpoly[i]->pvNodeItMain = (void*)pnode_parent;
	
				// Greater child is at [i-2^(level-1)].
				pnode_greater = (TNode *)aprpoly[i - step]->pvNodeItMain;
				pnode_greater->pnodeParent = pnode_parent;

				// Lesser child is at [i+2^(level-1)].
				pnode_lesser = (TNode *)aprpoly[i + step]->pvNodeItMain;
				pnode_lesser->pnodeParent = pnode_parent;
				
				pnode_parent->pnodeGreater  = pnode_greater;
				pnode_parent->pnodeLess     = pnode_lesser;
			}

			// Finish any left-overs.
			if (i < u_num)
			{
				// Create node at i.
				pnode_parent = AllocateNode();
				pnode_parent->Key	= aprpoly[i]->keySort;
				pnode_parent->Data	= aprpoly[i];
				pnode_parent->bSkip	= false;
				aprpoly[i]->pvNodeItMain = (void*)pnode_parent;

				// Greater child is at [i-2^(level-1)].
				pnode_greater = (TNode *)aprpoly[i - step]->pvNodeItMain;
				pnode_greater->pnodeParent = pnode_parent;

				// Lesser child is found by going down levels until the node exists.
				int sub_step = step >> 1;
				while (i + sub_step >= u_num)
				{
					sub_step >>= 1;
				}

				if (sub_step > 0)
				{
					// Set pointer to lesser sub-tree.
					pnode_lesser = (TNode *)aprpoly[i + sub_step]->pvNodeItMain;
					pnode_lesser->pnodeParent = pnode_parent;
				}
				else
				{
					// No lesser sub-tree.
					pnode_lesser = 0;
				}

				pnode_parent->pnodeGreater  = pnode_greater;
				pnode_parent->pnodeLess     = pnode_lesser;
			}

			// Next higher level of tree.
			level++;
		}

		// Build the root.
		i = (1 << level)-1;
		int step = (1 << (level-1));

		// Create node at i.
		pnode_parent = this;						// Special case for root.
		pnode_parent->Key	= aprpoly[i]->keySort;
		pnode_parent->Data	= aprpoly[i];
		pnode_parent->bSkip	= false;
		aprpoly[i]->pvNodeItMain = (void*)pnode_parent;

		// Greater child is at [i-2^(level-1)].
		pnode_greater = (TNode *)aprpoly[i - step]->pvNodeItMain;
		pnode_greater->pnodeParent = pnode_parent;

		// Lesser child is found by going down levels until the node exists.
		while (i + step >= u_num)
		{
			step >>= 1;
		}

		if (step > 0)
		{
			// Set pointer to lesser sub-tree.
			pnode_lesser = (TNode *)aprpoly[i + step]->pvNodeItMain;
			pnode_lesser->pnodeParent = pnode_parent;
		}
		else
		{
			// No lesser sub-tree.
			pnode_lesser = 0;
		}

		pnode_parent->pnodeParent	= 0;
		pnode_parent->pnodeGreater  = pnode_greater;
		pnode_parent->pnodeLess     = pnode_lesser;

		// Finish setting up the tree.
		iNumNodes = u_num;
	}

	//******************************************************************************************
	void CSortList::Bind(CRenderPolygon* aprpoly[], uint u_num)
	{
		Assert(aprpoly);

		// Do nothing if there are no nodes in this tree.
		if (u_num < 1)
			return;

		CSortList::TNode* pnode_last = (CSortList::TNode*)aprpoly[u_num - 1]->pvNodeItMain;
		pnode_last->pnodePrevious = 0;

		for (int u = (int)u_num - 2; u >= 0; u--)
		{
			CSortList::TNode* pnode = (CSortList::TNode*)aprpoly[u]->pvNodeItMain;
			pnode_last->SetNextPointer(pnode);
			pnode->pnodePrevious = pnode_last;
			pnode_last = pnode;
		}
		pnode_last->SetNextPointer(0);
	}

	//******************************************************************************************
	void CSortList::SetNodeFromMain(CRenderPolygon* aprpoly[], uint u_num)
	{
		Assert(aprpoly);

		for (int i = 0; i < int(u_num); i++)
		{
			Assert(aprpoly[i]);

			CRenderPolygon* prpoly = aprpoly[i];

#if (iMAX_SORT_LISTS > 1)
			prpoly->pslMemberOf[prpoly->iNumSortLists] = this;
			prpoly->pvNodeIt[prpoly->iNumSortLists]    = prpoly->pvNodeItMain;
			prpoly->iNumSortLists++;
#endif
		}
	}

	//******************************************************************************************
	int CSortList::iCopyMatchingElements(CRenderPolygon* aprpoly_master[],
		                                 CRenderPolygon* aprpoly[], uint u_num)
	{
		Assert(aprpoly_master);
		Assert(aprpoly);

		int i_match = 0;
		for (int i = 0; i < int(u_num); i++)
		{
			Assert(aprpoly_master[i]);

			//
			// If the polygon's bounding screen rectangle overlaps the sorting list's screen
			// rectangle, copy a pointer to the polygon to the new list.
			//
			if (bOverlap(aprpoly_master[i]))
			{
				aprpoly[i_match] = aprpoly_master[i];
				i_match++;
			}
		}
		return i_match;
	}


//**********************************************************************************************
//
// CDepthSort member function implementations.
//

	//******************************************************************************************
	//
	// CDepthSort constructor.
	//

	// Initializing constructor.
	CDepthSort::CDepthSort(CPipelineHeap* pplh_heap, const CCamera* pcam, int i_width, int i_height)
		: pplhHeap(pplh_heap), pcamCamera(pcam), rMaxLevel(0.0f), iNumSortedLists(0), iWidth(i_width),
		  iHeight(i_height)
	{
		// Create the main object.
		slMain.Set(CVector2<int>(0, 0), CVector2<int>(i_width, i_height));
	}

	//******************************************************************************************
	//
	// CDepthSort member functions.
	//
	
	//******************************************************************************************
	//
	void CDepthSort::AddSublist(int i_x0, int i_y0, int i_x1, int i_y1)
	{
		Assert(iNumSortedLists < iMAX_SORT_LISTS);
		
		pslParts[iNumSortedLists++].Set(CVector2<int>(i_x0, i_y0), CVector2<int>(i_x1, i_y1));
	}

	//**************************************************************************************
	void CDepthSort::QuadPartition(int i_x, int i_y_left, int i_y_right)
	{
		// Build two lists on the left.
		AddSublist(0,            0, i_x, i_y_left);
		AddSublist(0, i_y_left + 1, i_x, iHeight);

		// Build two lists on the right.
		AddSublist(i_x + 1,             0, iWidth, i_y_right);
		AddSublist(i_x + 1, i_y_right + 1, iWidth, iHeight);
	}

	//**************************************************************************************
	void CDepthSort::BinaryPartition(int i_x)
	{
		AddSublist(      0, 0,    i_x, iHeight);
		AddSublist(i_x + 1, 0, iWidth, iHeight);
	}

#if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)

	//
	// Class members we can't get to using normal means.
	//
	#define CRenderPolygon_paprvPolyVertices_uLen		0
	#define CRenderPolygon_paprvPolyVertices_atArray	4
	#define CRenderPolygon_plPlane_d3Normal_tX			56
	#define CRenderPolygon_plPlane_d3Normal_tY			60
	#define CRenderPolygon_plPlane_d3Normal_tZ			64
	#define CRenderPolygon_plPlane_rD					68

	//******************************************************************************************
	void CDepthSort::Sort()
	{
		CRenderPolygon* prpoly_q;	// Polygon.
		const uint32 u4FP_SIGN_BIT = 0x80000000;

		#if (DEPTH_SORT_STATS)
			CCycleTimer ctmr;
		#endif

		// Make sure these offsets are correct.
		Assert(iDATA_OFFSET == uint32(&((CSortList*)0)->Data));
		Assert(iNEXT_OFFSET == uint32(&((CSortList*)0)->pnodeNext));
		Assert(CRenderPolygon_paprvPolyVertices_uLen  == (uint32)&((CRenderPolygon*)0)->paprvPolyVertices.uLen);
		Assert(CRenderPolygon_paprvPolyVertices_atArray == (uint32)&((CRenderPolygon*)0)->paprvPolyVertices.atArray);
		Assert(CRenderPolygon_plPlane_d3Normal_tX  == (uint32)&((CRenderPolygon*)0)->plPlane.d3Normal.tX);
		Assert(CRenderPolygon_plPlane_d3Normal_tY  == (uint32)&((CRenderPolygon*)0)->plPlane.d3Normal.tY);
		Assert(CRenderPolygon_plPlane_d3Normal_tZ  == (uint32)&((CRenderPolygon*)0)->plPlane.d3Normal.tZ);
		Assert(CRenderPolygon_plPlane_rD  == (uint32)&((CRenderPolygon*)0)->plPlane.rD);

		if (iNumSortedLists == 2)
		{
			iNumA = pslParts[0].size();
			iNumB = pslParts[1].size();
		}
		if (iNumSortedLists == 4)
		{
			iNumA = pslParts[0].size();
			iNumB = pslParts[1].size();
			iNumC = pslParts[2].size();
			iNumD = pslParts[3].size();
		}

		// Get the first polygon.
		CRenderPolygon* prpoly_p = prpolyNextP();

		// While there are still polygons, depth sort them.
		while (prpoly_p)
		{
			// Select the iterator for the list the polygon belongs to.
			TIterator it_poly = itSelectQList(prpoly_p);
	
			#if bUSE_AREA_P
				// Skip the polygon if the area is to small.
				if (u4FromFloat(prpoly_p->fArea) <= u4FromFloat(fMinAreaForSort))
					goto PUSH_P;
			#endif // bUSE_AREA_P

			// Iterate through the polygon list.
			__asm
			{
				mov		esi,[it_poly]

				mov		edi,[prpoly_p]
				jmp		START_Q_LOOP

BEGIN_WHILE_Q:
				// Loaded: edi = prpoly_p
				// Loaded: esi = it_poly for prpoly_q

				mov		edx,[esi + iDATA_OFFSET]						// prpoly_q

				mov		eax,[edi]CRenderPolygon.fMinZ
				mov		[prpoly_q],edx

			#if bUSE_AREA_Q
				mov		ebx,[fMinAreaForSort]

				mov		ecx,[edx]CRenderPolygon.fArea

				cmp		ecx,ebx
				jle		short GET_NEXT_Q_ASM
			#endif // bUSE_AREA_Q

				mov		ebx,[edx]CRenderPolygon.rMaxLevel
				mov		[it_poly],esi

				cmp		eax,ebx
				mov		eax,[edi]CRenderPolygon.v2ScreenMinInt.tX

				jge		short PUSH_P
				mov		ebx,[edx]CRenderPolygon.v2ScreenMaxInt.tX

				// Step 2: screen overlap trivial rejection.
				cmp		eax,ebx
				mov		eax,[edi]CRenderPolygon.v2ScreenMaxInt.tX

				jge		short GET_NEXT_Q_ASM
				mov		ebx,[edx]CRenderPolygon.v2ScreenMinInt.tX

				cmp		eax,ebx
				mov		eax,[edi]CRenderPolygon.v2ScreenMinInt.tY

				jle		short GET_NEXT_Q_ASM
				mov		ebx,[edx]CRenderPolygon.v2ScreenMaxInt.tY

				cmp		eax,ebx
				mov		eax,[edi]CRenderPolygon.v2ScreenMaxInt.tY

				jge		short GET_NEXT_Q_ASM
				mov		ebx,[edx]CRenderPolygon.v2ScreenMinInt.tY

				cmp		eax,ebx
				jle		short GET_NEXT_Q_ASM
			}

CONTINUE_WITH_Q:
			__asm
			{
				femms

				mov		edi,[prpoly_p]

				mov		edx,[prpoly_q]

				//
				// P behind Q?
				//
				// edi = prpoly_p
				// edx = prpoly_q
				//

				//
				// Step 3: Are all points in P entirely behind Q's plane?
				//
				mov		eax,[edi+CRenderPolygon_paprvPolyVertices_atArray]

				mov		ecx,[edi+CRenderPolygon_paprvPolyVertices_uLen]

				mov		esi,[edi]CRenderPolygon.rPlaneTolerance				// Use P's plane tolerance

				movq	mm2,[edx+CRenderPolygon_plPlane_d3Normal_tX]		// N.y | N.x

				movd	mm3,[edx+CRenderPolygon_plPlane_d3Normal_tZ]		// 0 | N.z

				movd	mm4,[edx+CRenderPolygon_plPlane_rD]					// 0 | D

VTX_LOOP_P:
				mov		ebx,[eax]											// De-ref vert
				add		eax,4												// Next vertex

				movq	mm0,[ebx]SRenderCoord.v3Cam.tX						// y | x

				movd	mm1,[ebx]SRenderCoord.v3Cam.tZ						// z

				pfmul	(m0,m2)												// y*y | x*x

				pfmul	(m1,m3)												// z*z

				pfacc	(m0,m0)												// x*x+y*y

				pfadd	(m1,m4)												// z*z + D

				// stall

				pfadd	(m1,m0)												// x*x + y*y + z*z + d

				movd	ebx,mm1

				cmp		ebx,esi												// Point in front of Q?
				jg		P_NOT_BEHIND_Q										// (dist > tol)

				dec		ecx
				jnz		VTX_LOOP_P

				// Order is correct.
				mov		esi,[it_poly]
				femms
				jmp		GET_NEXT_Q_ASM

P_NOT_BEHIND_Q:
				//
				// Step 4: Are all points in Q entirely in front of P's plane?
				//
				mov		eax,[edx+CRenderPolygon_paprvPolyVertices_atArray]

				mov		ecx,[edx+CRenderPolygon_paprvPolyVertices_uLen]
				xor		esi,0x80000000										// Negate plane tolerance.

				movq	mm2,[edi+CRenderPolygon_plPlane_d3Normal_tX]		// N.y | N.x

				movd	mm3,[edi+CRenderPolygon_plPlane_d3Normal_tZ]		// 0 | N.z

				movd	mm4,[edi+CRenderPolygon_plPlane_rD]					// 0 | D

VTX_LOOP_Q:
				mov		ebx,[eax]											// De-ref vert
				add		eax,4												// Next vertex

				movq	mm0,[ebx]SRenderCoord.v3Cam.tX						// y | x

				movd	mm1,[ebx]SRenderCoord.v3Cam.tZ						// z

				pfmul	(m0,m2)												// y*y | x*x

				pfmul	(m1,m3)												// z*z

				pfacc	(m0,m0)												// x*x+y*y

				pfadd	(m1,m4)												// z*z + D

				// stall

				pfadd	(m1,m0)												// x*x + y*y + z*z + d

				movd	ebx,mm1
				
				cmp		ebx,esi												// Point behind P?
				ja		Q_NOT_IN_FRONT_OF_P									// (dist < -tol)

				dec		ecx
				jnz		VTX_LOOP_Q

				// Order is correct.
				mov		esi,[it_poly]
				femms
				jmp		GET_NEXT_Q_ASM

Q_NOT_IN_FRONT_OF_P:
				//
				// Step 5: Do the 2D projections of P and Q intersect?
				//
				
				// 5a) Check all points of P against against all edges of Q.
				mov		ecx,[edx+CRenderPolygon_paprvPolyVertices_uLen]

				mov		ebx,[edx+CRenderPolygon_paprvPolyVertices_atArray]

				pcmpeqd	mm7,mm7									// and_codes = all ones
				mov		edx,[ebx+ecx*4-4]						// First vertex
				
Q_EDGE_LOOP:
				mov		eax,[ebx]								// prv_q2

				mov		edi,[prpoly_p]							// Pointer to p.

				movq	mm2,[edx]SRenderCoord.v3Screen.tX		// q1.Y | q1.X

				movq	mm3,[eax]SRenderCoord.v3Screen.tX		// q2.Y | q2.X

				pfsub	(m3,m2)									// q2.Y-q1.Y | q2.X-q1.X
				movd	mm1,[u4FP_SIGN_BIT]

				//
				// edi = prpoly_p, edx = prv_q1
				// mm2 = q1.Y | q1.X, mm3 = qdx | -qdy
				// ebx is preserved
				//
				mov			esi,[edi+CRenderPolygon_paprvPolyVertices_uLen]
				punpckldq	mm0,mm3								// qdx | X

				mov			edi,[edi+CRenderPolygon_paprvPolyVertices_atArray]
				punpckhdq	mm3,mm0								// qdx | qdy

				pxor	mm6,mm6									// Clear edge codes
				pxor	mm3,mm1									// qdx | -qdy

P_POINT_LOOP:
				mov		eax,[edi]								// Pointer to vertex
				add		edi,4									// Next vertex index
				
				movq	mm0,[eax]SRenderCoord.v3Screen.tX		// p.Y | p.X

				pfsub	(m0,m2)									// p.Y-q1.Y | p.X-q1.X

				pfmul	(m0,m3)									// dy*qdx dx*-qdy

				pfacc	(m0,m0)									// cross product
				psllq	mm6,1									// Shift edge_codes up

				psrld	mm0,31									// Shift sign bit down

				por		mm6,mm0									// Combine with edge_codes

				dec		esi										// Decrement count
				jnz		P_POINT_LOOP							// Loop

				movd	eax,mm6									// get edge codes.
				pand	mm7,mm6									// and_codes &= edge_codes

				mov		edx,[ebx]								// prv_q1 = prv_q2
				add		ebx,4									// Next edge of Q

				test	eax,eax									// Zero edge codes?
				jnz		CONTINUE_Q_LOOP

				mov		esi,[it_poly]							// Restore interator
				femms
				jmp		GET_NEXT_Q_ASM							// No intersection

CONTINUE_Q_LOOP:
				dec		ecx										// Decrement count
				jnz		Q_EDGE_LOOP								// Loop
			
				movd	eax,mm7									// get and codes
				mov		edx,[prpoly_p]							// Load pointer to P

				test	eax,eax									// and_codes != 0
				jnz		SKIP_QP_ISECT_TEST						// Must intersect
				
				// 5b) Check all points of Q against against all edges of P.
				mov		ecx,[edx+CRenderPolygon_paprvPolyVertices_uLen]

				mov		ebx,[edx+CRenderPolygon_paprvPolyVertices_atArray]

				mov		edx,[ebx+ecx*4-4]						// First vertex
				
P_EDGE_LOOP:	
				mov		eax,[ebx]								// prv_p2

				mov		edi,[prpoly_q]							// Load pointer to Q

				movq	mm2,[edx]SRenderCoord.v3Screen.tX		// p1.Y | p1.X

				movq	mm3,[eax]SRenderCoord.v3Screen.tX		// p2.Y | p2.X

				pfsub	(m3,m2)									// p2.Y-p1.Y | p2.X-p1.X
				movd	mm1,[u4FP_SIGN_BIT]

				//
				// edi = prpoly_q, edx = prv_p1
				// mm2 = p1.Y | p1.X, mm3 = pdx | -pdy
				// ebx is preserved
				//
				mov			esi,[edi+CRenderPolygon_paprvPolyVertices_uLen]
				punpckldq	mm0,mm3								// pdx | X

				mov			edi,[edi+CRenderPolygon_paprvPolyVertices_atArray]
				punpckhdq	mm3,mm0								// pdx | pdy

				pxor	mm6,mm6									// Clear inside flag
				pxor	mm3,mm1									// pdx | -pdy

Q_POINT_LOOP:
				mov		eax,[edi]								// Pointer to vertex
				add		edi,4									// Next vertex index
				
				movq	mm0,[eax]SRenderCoord.v3Screen.tX		// q.Y | q.X

				pfsub	(m0,m2)									// q.Y-p1.Y | q.X-p1.X

				pfmul	(m0,m3)									// dy*pdx dx*-pdy

				pfacc	(m0,m0)									// cross product

				por		mm6,mm0									// Or with inside flag

				dec		esi										// Decrement count
				jnz		Q_POINT_LOOP							// Loop

				movd	eax,mm6									// get edge codes.

				mov		edx,[ebx]								// prv_p1 = prv_p2
				add		ebx,4									// Next edge of P

				and		eax,0x80000000							// Zero inside?
				jnz		CONTINUE_P_LOOP

				mov		esi,[it_poly]							// Restore interator
				femms
				jmp		GET_NEXT_Q_ASM							// No intersection

CONTINUE_P_LOOP:
				dec		ecx										// Decrement count
				jnz		P_EDGE_LOOP								// Loop

SKIP_QP_ISECT_TEST:
				//
				// See if we can move Q to the end of the list.
				//
				mov		edi,[prpoly_q]

				mov		edx,[prpoly_p]

#if ((_MSC_VER < 1100) && (!defined(__MWERKS__)))
				mov		eax,[edi]CRenderPolygon.bMovedInList	// prpoly_q->bMovedInList

				test	eax,eax									// if (prpoly_q->bMovedInList)
				jz		P_IN_FRONT_OF_Q_TEST
#else
				mov		al,[edi]CRenderPolygon.bMovedInList		// prpoly_q->bMovedInList

				test	al,al									// if (prpoly_q->bMovedInList)
				jz		P_IN_FRONT_OF_Q_TEST
#endif

				femms
				jmp		ATTEMPT_SPLIT							// goto ATTEMPT_SPLIT;

P_IN_FRONT_OF_Q_TEST:
				//
				// if (!prpoly_p->bInFrontOf(prpoly_q)) goto ATTEMPT_SPLIT;
				//

				//
				// Step 3': Are all points in Q entirely behind P's plane?
				//
				mov		eax,[edi+CRenderPolygon_paprvPolyVertices_atArray]

				mov		ecx,[edi+CRenderPolygon_paprvPolyVertices_uLen]

				mov		esi,[edx]CRenderPolygon.rPlaneTolerance				// Use P's plane tolerance

				movq	mm2,[edx+CRenderPolygon_plPlane_d3Normal_tX]		// N.y | N.x

				movd	mm3,[edx+CRenderPolygon_plPlane_d3Normal_tZ]		// 0 | N.z

				movd	mm4,[edx+CRenderPolygon_plPlane_rD]					// 0 | D

VTX_LOOP_Q2:
				mov		ebx,[eax]											// De-ref vert
				add		eax,4												// Next vertex

				movq	mm0,[ebx]SRenderCoord.v3Cam.tX						// y | x

				movd	mm1,[ebx]SRenderCoord.v3Cam.tZ						// z

				pfmul	(m0,m2)												// y*y | x*x

				pfmul	(m1,m3)												// z*z

				pfacc	(m0,m0)												// x*x+y*y

				pfadd	(m1,m4)												// z*z + D

				// stall

				pfadd	(m1,m0)												// x*x + y*y + z*z + d

				movd	ebx,mm1

				cmp		ebx,esi												// Point in front of P?
				jg		Q_NOT_BEHIND_P										// (dist > tol)

				dec		ecx
				jnz		VTX_LOOP_Q2

				// Order is reversed.
				jmp		P_IN_FRONT_OF_Q

Q_NOT_BEHIND_P:
				//
				// Step 4': Are all points in P entirely in front of Q's plane?
				//
				mov		eax,[edx+CRenderPolygon_paprvPolyVertices_atArray]

				mov		ecx,[edx+CRenderPolygon_paprvPolyVertices_uLen]
				xor		esi,0x80000000										// Negate plane tolerance.

				movq	mm2,[edi+CRenderPolygon_plPlane_d3Normal_tX]		// N.y | N.x

				movd	mm3,[edi+CRenderPolygon_plPlane_d3Normal_tZ]		// 0 | N.z

				movd	mm4,[edi+CRenderPolygon_plPlane_rD]					// 0 | D

VTX_LOOP_P2:
				mov		ebx,[eax]											// De-ref vert
				add		eax,4												// Next vertex

				movq	mm0,[ebx]SRenderCoord.v3Cam.tX						// y | x

				movd	mm1,[ebx]SRenderCoord.v3Cam.tZ						// z

				pfmul	(m0,m2)												// y*y | x*x

				pfmul	(m1,m3)												// z*z

				pfacc	(m0,m0)												// x*x+y*y

				pfadd	(m1,m4)												// z*z + D

				// stall

				pfadd	(m1,m0)												// x*x + y*y + z*z + d

				movd	ebx,mm1
				
				cmp		ebx,esi												// Point behind Q?
				jna		CONT_VTX_LOOP_P2									// !(dist < -tol)

				femms
				jmp		ATTEMPT_SPLIT

CONT_VTX_LOOP_P2:
				dec		ecx
				jnz		VTX_LOOP_P2

				// Order is reversed.

P_IN_FRONT_OF_Q:
				// P is in front of Q.
				femms
			}

//			Assert(prpoly_p->bInFrontOf(prpoly_q) == 1);
//			Assert(bFastIntersect2D(prpoly_p, prpoly_q));

			//
			// Move Q to the end of the list.
			//

			// Mark Q.
			prpoly_q->bMovedInList = true;

			// Remove Q from its current position in the list.
			Erase(prpoly_q);

			// Reinsert P and Q into the list, with Q replacing P's position.
			PushFront(prpoly_p);
			PushFront(prpoly_q);

			// P cannot be scan converted yet.
			goto SKIP_PUSH_P;

ATTEMPT_SPLIT:
//			Assert(prpoly_q->bMovedInList || prpoly_p->bInFrontOf(prpoly_q) == 0);
//			Assert(bFastIntersect2D(prpoly_p, prpoly_q));

			#if (DEPTH_SORT_STATS)
				ctmr.Reset();
			#endif

			// Attempt to split P.
			if (bSplitPByQ(prpoly_p, prpoly_q))
			{
				#if (DEPTH_SORT_STATS)
					psSplit.Add(ctmr(), 1);
				#endif

				goto CONTINUE_WITH_Q;
			}

			// Attempt to split Q.
			if (bSplitQByP(prpoly_p, prpoly_q))
			{
				#if (DEPTH_SORT_STATS)
					psSplit.Add(ctmr(), 1);
				#endif

				goto CONTINUE_WITH_Q;
			}

			#if (DEPTH_SORT_STATS)
				psSplit.Add(ctmr(), 1);
			#endif

			// Increment iterator.
			__asm
			{
				mov		esi,[it_poly]

GET_NEXT_Q_ASM:
				mov		edi,[prpoly_p]

				mov		esi,[esi + iNEXT_OFFSET]

START_Q_LOOP:
				test	esi,esi
				jnz		short BEGIN_WHILE_Q
			}

PUSH_P:
			// Record stat.
			iNumPolygonsEnd++;

			// Add finished polygon to scan conversion list.
			pplhHeap->darppolyPolygons << prpoly_p;

SKIP_PUSH_P:
			// Get the next polygon.
			prpoly_p = prpolyNextP();
		}
	}

#elif (VER_ASM) // if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)

	//
	// Class members we can't get to using normal means.
	//
	#define CRenderPolygon_paprvPolyVertices_uLen		0
	#define CRenderPolygon_paprvPolyVertices_atArray	4
	#define CRenderPolygon_plPlane_d3Normal_tX			56
	#define CRenderPolygon_plPlane_d3Normal_tY			60
	#define CRenderPolygon_plPlane_d3Normal_tZ			64
	#define CRenderPolygon_plPlane_rD					68

	//******************************************************************************************
	void CDepthSort::Sort()
	{
		CRenderPolygon* prpoly_q;	// Polygon.
		float f_temp;
		int i_temp;
//		float f_pixel_tol = ptsTolerances.fPixelBuffer;

		#if (DEPTH_SORT_STATS)
			CCycleTimer ctmr;
		#endif

		// Make sure these offsets are correct.
		Assert(iDATA_OFFSET == uint32(&((CSortList*)0)->Data));
		Assert(iNEXT_OFFSET == uint32(&((CSortList*)0)->pnodeNext));
		Assert(CRenderPolygon_paprvPolyVertices_uLen  == (uint32)&((CRenderPolygon*)0)->paprvPolyVertices.uLen);
		Assert(CRenderPolygon_paprvPolyVertices_atArray == (uint32)&((CRenderPolygon*)0)->paprvPolyVertices.atArray);
		Assert(CRenderPolygon_plPlane_d3Normal_tX  == (uint32)&((CRenderPolygon*)0)->plPlane.d3Normal.tX);
		Assert(CRenderPolygon_plPlane_d3Normal_tY  == (uint32)&((CRenderPolygon*)0)->plPlane.d3Normal.tY);
		Assert(CRenderPolygon_plPlane_d3Normal_tZ  == (uint32)&((CRenderPolygon*)0)->plPlane.d3Normal.tZ);
		Assert(CRenderPolygon_plPlane_rD  == (uint32)&((CRenderPolygon*)0)->plPlane.rD);

		if (iNumSortedLists == 2)
		{
			iNumA = pslParts[0].size();
			iNumB = pslParts[1].size();
		}
		if (iNumSortedLists == 4)
		{
			iNumA = pslParts[0].size();
			iNumB = pslParts[1].size();
			iNumC = pslParts[2].size();
			iNumD = pslParts[3].size();
		}

		// Get the first polygon.
		CRenderPolygon* prpoly_p = prpolyNextP();

		// While there are still polygons, depth sort them.
		while (prpoly_p)
		{
			// Select the iterator for the list the polygon belongs to.
			TIterator it_poly = itSelectQList(prpoly_p);
	
			#if bUSE_AREA_P
				// Skip the polygon if the area is to small.
				if (u4FromFloat(prpoly_p->fArea) <= u4FromFloat(fMinAreaForSort))
					goto PUSH_P;
			#endif // bUSE_AREA_P

			// Iterate through the polygon list.
			__asm
			{
				mov		esi,[it_poly]
				mov		edi,[prpoly_p]

				jmp		START_Q_LOOP

BEGIN_WHILE_Q:
				// Loaded: edi = prpoly_p
				// Loaded: esi = it_poly for prpoly_q

				mov		edx,[esi + iDATA_OFFSET]						// prpoly_q
				mov		eax,[edi]CRenderPolygon.fMinZ

			#if bUSE_AREA_Q
				mov		ebx,[fMinAreaForSort]	
				mov		ecx,[edx]CRenderPolygon.fArea

				cmp		ecx,ebx
				jle		GET_NEXT_Q_ASM
			#endif // bUSE_AREA_Q

				mov		[prpoly_q],edx
				mov		[it_poly],esi

				mov		ebx,[edx]CRenderPolygon.rMaxLevel

				cmp		eax,ebx
				jge		PUSH_P

				// Step 2: screen overlap trivial rejection.
				mov		eax,[edi]CRenderPolygon.v2ScreenMinInt.tX
				mov		ebx,[edx]CRenderPolygon.v2ScreenMaxInt.tX

				cmp		eax,ebx
				jge		GET_NEXT_Q_ASM

				mov		eax,[edi]CRenderPolygon.v2ScreenMaxInt.tX
				mov		ebx,[edx]CRenderPolygon.v2ScreenMinInt.tX

				cmp		eax,ebx
				jle		GET_NEXT_Q_ASM

				mov		eax,[edi]CRenderPolygon.v2ScreenMinInt.tY
				mov		ebx,[edx]CRenderPolygon.v2ScreenMaxInt.tY

				cmp		eax,ebx
				jge		GET_NEXT_Q_ASM

				mov		eax,[edi]CRenderPolygon.v2ScreenMaxInt.tY
				mov		ebx,[edx]CRenderPolygon.v2ScreenMinInt.tY

				cmp		eax,ebx
				jle		GET_NEXT_Q_ASM
			}

			#if (bUSE_COUNTERS)
				iNumStep3++;
			#endif

CONTINUE_WITH_Q:
			__asm
			{
				mov		edi,[prpoly_p]
				mov		edx,[prpoly_q]

				//
				// P behind Q?
				//
				// edi = prpoly_p
				// edx = prpoly_q
				//

				//
				// Step 3: Are all points in P entirely behind Q's plane?
				//
				mov		eax,[edi+CRenderPolygon_paprvPolyVertices_atArray]
				mov		ecx,[edi+CRenderPolygon_paprvPolyVertices_uLen]

				mov		esi,[edi]CRenderPolygon.rPlaneTolerance				// Use P's plane tolerance
				dec		ecx

				mov		ebx,[eax]

				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tX]
				fmul	[ebx]SRenderCoord.v3Cam.tX							// x
				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tY]
				fmul	[ebx]SRenderCoord.v3Cam.tY							// y x
				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tZ]
				fmul	[ebx]SRenderCoord.v3Cam.tZ							// z y x
				fxch	st(2)												// x y z
				faddp	st(1),st(0)											// x+y z

VTX_LOOP_P:
				mov		ebx,[eax+4]											// Next vertex
				add		eax,4

				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tX]
				fmul	[ebx]SRenderCoord.v3Cam.tX							// x1 x+y z
				fxch	st(1)												// x+y x1 z

				faddp	st(2),st(0)											// x1 x+y+z

				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tY]
				fmul	[ebx]SRenderCoord.v3Cam.tY							// y1 x1 x+y+z
				fxch	st(2)												// x+y+z x1 y1

				fadd	dword ptr[edx+CRenderPolygon_plPlane_rD]			// x+y+z+d x1 y1

				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tZ]
				fmul	[ebx]SRenderCoord.v3Cam.tZ							// z1 x+y+z+d x1 y1
				fxch	st(3)												// y1 x+y+z+d x1 z1
				faddp	st(2),st(0)											// x+y+z+d x1+y1 z1

				fstp	[f_temp]											// x1+y1 z1

				mov		ebx,[f_temp]										// Distance from plane
				
				cmp		ebx,esi												// Point in front of Q?
				jng		P_VERT_OK											// !(dist > tol)

				fcompp														// dump stuff off stack
				jmp		P_NOT_BEHIND_Q

P_VERT_OK:
				dec		ecx
				jnz		VTX_LOOP_P

				faddp	st(1),st(0)											// x+y+z

				// stall(2)

				fadd	dword ptr[edx+CRenderPolygon_plPlane_rD]			// x+y+z+d

				// stall(3)

				fstp	[f_temp]

				mov		ebx,[f_temp]										// Distance from plane
				
				cmp		ebx,esi												// Point in front of Q?
				jg		P_NOT_BEHIND_Q										// dist > tol

				// Order is correct.
				mov		esi,[it_poly]
				jmp		GET_NEXT_Q_ASM

P_NOT_BEHIND_Q:
				//
				// Step 4: Are all points in Q entirely in front of P's plane?
				//
				mov		eax,[edx+CRenderPolygon_paprvPolyVertices_atArray]
				mov		ecx,[edx+CRenderPolygon_paprvPolyVertices_uLen]

				xor		esi,0x80000000										// Negate plane tolerance.

				mov		ebx,[eax]
				dec		ecx

				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tX]
				fmul	[ebx]SRenderCoord.v3Cam.tX							// x
				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tY]
				fmul	[ebx]SRenderCoord.v3Cam.tY							// y x
				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tZ]
				fmul	[ebx]SRenderCoord.v3Cam.tZ							// z y x
				fxch	st(2)												// x y z
				faddp	st(1),st(0)											// x+y z

VTX_LOOP_Q:
				mov		ebx,[eax+4]											// Next vertex
				add		eax,4

				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tX]
				fmul	[ebx]SRenderCoord.v3Cam.tX							// x1 x+y z
				fxch	st(1)												// x+y x1 z

				faddp	st(2),st(0)											// x1 x+y+z

				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tY]
				fmul	[ebx]SRenderCoord.v3Cam.tY							// y1 x1 x+y+z
				fxch	st(2)												// x+y+z x1 y1

				fadd	dword ptr[edi+CRenderPolygon_plPlane_rD]			// x+y+z+d x1 y1

				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tZ]
				fmul	[ebx]SRenderCoord.v3Cam.tZ							// z1 x+y+z+d x1 y1
				fxch	st(3)												// y1 x+y+z+d x1 z1
				faddp	st(2),st(0)											// x+y+z+d x1+y1 z1

				fstp	[f_temp]											// x1+y1 z1

				mov		ebx,[f_temp]										// Distance from plane
				
				cmp		ebx,esi												// Point behind P?
				jna		Q_VERT_OK											// !(dist < -tol)

				fcompp														// dump stuff off stack
				jmp		Q_NOT_IN_FRONT_OF_P

Q_VERT_OK:
				dec		ecx
				jnz		VTX_LOOP_Q

				faddp	st(1),st(0)											// x+y+z

				// stall(2)

				fadd	dword ptr[edi+CRenderPolygon_plPlane_rD]			// x+y+z+d

				// stall(3)

				fstp	[f_temp]

				mov		ebx,[f_temp]										// Distance from plane
				
				cmp		ebx,esi												// Point behind P?
				ja		Q_NOT_IN_FRONT_OF_P									// dist < -tol

				// Order is correct.
				mov		esi,[it_poly]
				jmp		GET_NEXT_Q_ASM

Q_NOT_IN_FRONT_OF_P:
				//
				// Step 5: Do the 2D projections of P and Q intersect?
				//
				
				// 5a) Check all points of P against against all edges of Q.
				mov		ecx,[edx+CRenderPolygon_paprvPolyVertices_uLen]
				mov		ebx,[edx+CRenderPolygon_paprvPolyVertices_atArray]

				mov		eax,0xffffffff							// and_codes

				mov		[i_temp],eax
				mov		edx,[ebx+ecx*4-4]						// First vertex
				
Q_EDGE_LOOP:	
				mov		eax,[ebx]								// prv_q2
				mov		edi,[prpoly_p]							// Pointer to p.

				fld		[edx]SRenderCoord.v3Screen.tX			// q1.X
				fsubr	[eax]SRenderCoord.v3Screen.tX			// q2.X-q1.X
				fld		[edx]SRenderCoord.v3Screen.tY			// q1.Y
				fsubr	[eax]SRenderCoord.v3Screen.tY			// q2.Y-q1.Y q2.X-q1.X

				// edi = prpoly_p, edx = prv_q1, st(0) = f_qdy, st(1) = f_qdx, ebx is preserved
				mov		ch,[edi+CRenderPolygon_paprvPolyVertices_uLen]
				mov		edi,[edi+CRenderPolygon_paprvPolyVertices_atArray]

				xor		esi,esi									// Clear edge codes
				dec		ch										// Decrement count

				mov		eax,[edi]								// Pointer to vertex

				fld		[edx]SRenderCoord.v3Screen.tY			// q1.Y
				fsubr	[eax]SRenderCoord.v3Screen.tY			// p.Y-q1.Y
				fld		[edx]SRenderCoord.v3Screen.tX			// q1.X
				fsubr	[eax]SRenderCoord.v3Screen.tX			// p.X-q1.X p.Y-q1.Y qdy qdx
				fxch	st(1)									// dy dx qdy qdx
				fmul	st(0),st(3)								// dy*qdx dx qdy qdx
				fxch	st(1)									// dx dy*qdx qdy qdx
				fmul	st(0),st(2)								// dx*qdy dy*qdx qdy qdx

P_POINT_LOOP:
				mov		eax,[edi+4]								// Pointer to vertex
				add		edi,4									// Next vertex index
				
				fld		[edx]SRenderCoord.v3Screen.tY			// q1.Y' dx*qdy dy*qdx qdy qdx
				fsubr	[eax]SRenderCoord.v3Screen.tY			// p.Y'-q1.Y' dx*qdy dy*qdx qdy qdx
				fxch	st(1)									// dx*qdy p.Y'-q1.Y' dy*qdx qdy qdx

				fsubp	st(2),st(0)								// p.Y'-q1.Y' cross qdy qdx
				
				fld		[edx]SRenderCoord.v3Screen.tX			// q1.X' p.Y'-q1.Y' cross qdy qdx
				fsubr	[eax]SRenderCoord.v3Screen.tX			// p.X'-q1.X' p.Y'-q1.Y' cross qdy qdx
				fxch	st(1)									// dy' dx' cross qdy qdx
				fmul	st(0),st(4)								// dy'*qdx' dx' cross qdy qdx
				fxch	st(2)									// cross dx' dy'*qdx' qdy qdx

//				fadd	[f_pixel_tol]
				fstp	[f_temp]								// dx' dy'*qdx' qdy qdx

				fmul	st(0),st(2)								// dx'*qdy' dy'*qdx' qdy qdx

				shl		esi,1									// Shift edge_codes up
				mov		eax,[f_temp]							// Load cross product

				shr		eax,31									// Shift sign bit down

				or		esi,eax									// Combine with edge_codes
				dec		ch										// Decrement count

				jnz		P_POINT_LOOP							// Loop

				fsubp	st(1),st(0)								// cross qdy qdx
				fxch	st(2)									// qdx qdy cross
				fcompp											// cross

				mov		edx,[ebx]								// prv_q1 = prv_q2
				add		ebx,4									// Next edge of Q

//				fadd	[f_pixel_tol]
				fstp	[f_temp]

				shl		esi,1									// Shift edge_codes up
				mov		eax,[f_temp]							// Load cross product

				shr		eax,31									// Shift sign bit down
				mov		edi,[i_temp]							// Load and_codes

				or		eax,esi									// Combine with edge_codes
				mov		esi,[it_poly]							// Restore interator

				test	eax,eax									// Zero edge codes?
				jz		GET_NEXT_Q_ASM							// No intersection

				and		edi,eax									// and_codes &= edge_codes
				dec		cl										// Decrement count

				mov		[i_temp],edi							// Save and_codes
				jnz		Q_EDGE_LOOP								// Loop

				mov		edx,[prpoly_p]							// Load pointer to P

				test	edi,edi									// and_codes != 0
				jnz		SKIP_QP_ISECT_TEST						// Must intersect
				
				// 5b) Check all points of Q against against all edges of P.
				mov		ecx,[edx+CRenderPolygon_paprvPolyVertices_uLen]
				mov		ebx,[edx+CRenderPolygon_paprvPolyVertices_atArray]

				mov		edx,[ebx+ecx*4-4]						// First vertex
				
P_EDGE_LOOP:	
				mov		eax,[ebx]								// prv_p2
				mov		edi,[prpoly_q]							// Load pointer to Q

				fld		[edx]SRenderCoord.v3Screen.tX			// p1.X
				fsubr	[eax]SRenderCoord.v3Screen.tX			// p2.X-p1.X
				fld		[edx]SRenderCoord.v3Screen.tY			// p1.Y
				fsubr	[eax]SRenderCoord.v3Screen.tY			// p2.Y-p1.Y p2.X-p1.X

				// edi = prpoly_q, edx = prv_p1, st(0) = f_pdy, st(1) = f_pdx, ebx is preserved
				mov		ch,[edi+CRenderPolygon_paprvPolyVertices_uLen]
				mov		edi,[edi+CRenderPolygon_paprvPolyVertices_atArray]

				xor		esi,esi									// Clear inside flag
				dec		ch										// Decrement count

				mov		eax,[edi]								// Pointer to vertex

				fld		[edx]SRenderCoord.v3Screen.tY			// p1.Y
				fsubr	[eax]SRenderCoord.v3Screen.tY			// q.Y-p1.Y
				fld		[edx]SRenderCoord.v3Screen.tX			// p1.X
				fsubr	[eax]SRenderCoord.v3Screen.tX			// q.X-p1.X q.Y-p1.Y pdy pdx
				fxch	st(1)									// dy dx pdy pdx
				fmul	st(0),st(3)								// dy*pdx dx pdy pdx
				fxch	st(1)									// dx dy*px pdy pdx
				fmul	st(0),st(2)								// dx*pdy dy*pdx pdy pdx

Q_POINT_LOOP:
				mov		eax,[edi+4]								// Pointer to vertex
				add		edi,4									// Next vertex index
				
				fld		[edx]SRenderCoord.v3Screen.tY			// p1.Y' dx*pdy dy*pdx pdy pdx
				fsubr	[eax]SRenderCoord.v3Screen.tY			// q.Y'-p1.Y' dx*pdy dy*pdx pdy pdx
				fxch	st(1)									// dx*pdy q.Y'-p1.Y' dy*pdx pdy pdx

				fsubp	st(2),st(0)								// q.Y'-p1.Y' cross pdy pdx
				
				fld		[edx]SRenderCoord.v3Screen.tX			// p1.X' q.Y'-p1.Y' cross pdy pdx
				fsubr	[eax]SRenderCoord.v3Screen.tX			// q.X'-p1.X' q.Y'-p1.Y' cross pdy pdx
				fxch	st(1)									// dy' dx' cross pdy pdx
				fmul	st(0),st(4)								// dy'*pdx' dx' cross pdy pdx
				fxch	st(2)									// cross dx' dy'*pdx' pdy pdx

//				fadd	[f_pixel_tol]
				fstp	[f_temp]								// dx' dy'*pdx' pdy pdx

				fmul	st(0),st(2)								// dx'*pdy' dy'*pdx' pdy pdx

				mov		eax,[f_temp]							// Load cross product

				or		esi,eax									// Or with inside flag

				dec		ch										// Decrement count
				jnz		Q_POINT_LOOP							// Loop

				fsubp	st(1),st(0)								// cross pdy pdx
				fxch	st(2)									// pdx pdy cross
				fcompp											// cross

				mov		edx,[ebx]								// prv_p1 = prv_p2
				add		ebx,4									// Next edge of P

//				fadd	[f_pixel_tol]
				fstp	[f_temp]

				mov		eax,[f_temp]							// Load cross product

				or		eax,esi									// Or with inside flag
				mov		esi,[it_poly]							// Restore interator

				and		eax,0x80000000							// Zero inside?
				jz		GET_NEXT_Q_ASM							// No intersection

				dec		cl										// Decrement count
				jnz		P_EDGE_LOOP								// Loop

SKIP_QP_ISECT_TEST:
				// See if we can move Q to the end of the list.
				mov		edi,[prpoly_q]
				mov		edx,[prpoly_p]

#if ((_MSC_VER < 1100) && (!defined(__MWERKS__)))
				mov		eax,[edi]CRenderPolygon.bMovedInList	// prpoly_q->bMovedInList

				test	eax,eax									// if (prpoly_q->bMovedInList)
				jnz		ATTEMPT_SPLIT							//     goto ATTEMPT_SPLIT;
#else
				mov		al,[edi]CRenderPolygon.bMovedInList		// prpoly_q->bMovedInList

				test	al,al									// if (prpoly_q->bMovedInList)
				jnz		ATTEMPT_SPLIT							//     goto ATTEMPT_SPLIT;
#endif
				//
				// if (!prpoly_p->bInFrontOf(prpoly_q)) goto ATTEMPT_SPLIT;
				//

				//
				// Are all points in Q entirely behind P's plane?
				//
				mov		eax,[edi+CRenderPolygon_paprvPolyVertices_atArray]
				mov		ecx,[edi+CRenderPolygon_paprvPolyVertices_uLen]

				mov		esi,[edx]CRenderPolygon.rPlaneTolerance				// Use P's plane tolerance
				dec		ecx

				mov		ebx,[eax]

				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tX]
				fmul	[ebx]SRenderCoord.v3Cam.tX							// x
				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tY]
				fmul	[ebx]SRenderCoord.v3Cam.tY							// y x
				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tZ]
				fmul	[ebx]SRenderCoord.v3Cam.tZ							// z y x
				fxch	st(2)												// x y z
				faddp	st(1),st(0)											// x+y z

VTX_LOOP_Q2:
				mov		ebx,[eax+4]											// Next vertex
				add		eax,4

				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tX]
				fmul	[ebx]SRenderCoord.v3Cam.tX							// x1 x+y z
				fxch	st(1)												// x+y x1 z

				faddp	st(2),st(0)											// x1 x+y+z

				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tY]
				fmul	[ebx]SRenderCoord.v3Cam.tY							// y1 x1 x+y+z
				fxch	st(2)												// x+y+z x1 y1

				fadd	dword ptr[edx+CRenderPolygon_plPlane_rD]			// x+y+z+d x1 y1

				fld		dword ptr[edx+CRenderPolygon_plPlane_d3Normal_tZ]
				fmul	[ebx]SRenderCoord.v3Cam.tZ							// z1 x+y+z+d x1 y1
				fxch	st(3)												// y1 x+y+z+d x1 z1
				faddp	st(2),st(0)											// x+y+z+d x1+y1 z1

				fstp	[f_temp]											// x1+y1 z1

				mov		ebx,[f_temp]										// Distance from plane
				
				cmp		ebx,esi												// Point in front of Q?
				jng		Q_VERT_OK2											// !(dist > tol)

				fcompp														// dump stuff off stack
				jmp		Q_NOT_BEHIND_P

Q_VERT_OK2:
				dec		ecx
				jnz		VTX_LOOP_Q2

				faddp	st(1),st(0)											// x+y+z

				// stall(2)

				fadd	dword ptr[edx+CRenderPolygon_plPlane_rD]			// x+y+z+d

				// stall(3)

				fstp	[f_temp]

				mov		ebx,[f_temp]										// Distance from plane
				
				cmp		ebx,esi												// Point in front of Q?
				jg		Q_NOT_BEHIND_P										// dist > tol

				// Q is behind P.
				jmp		P_IN_FRONT_OF_Q

Q_NOT_BEHIND_P:
				//
				// Are all points in P entirely in front of Q's plane?
				//
				mov		eax,[edx+CRenderPolygon_paprvPolyVertices_atArray]
				mov		ecx,[edx+CRenderPolygon_paprvPolyVertices_uLen]

				xor		esi,0x80000000										// Negate plane tolerance.

				mov		ebx,[eax]
				dec		ecx

				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tX]
				fmul	[ebx]SRenderCoord.v3Cam.tX							// x
				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tY]
				fmul	[ebx]SRenderCoord.v3Cam.tY							// y x
				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tZ]
				fmul	[ebx]SRenderCoord.v3Cam.tZ							// z y x
				fxch	st(2)												// x y z
				faddp	st(1),st(0)											// x+y z

VTX_LOOP_P2:
				mov		ebx,[eax+4]											// Next vertex
				add		eax,4

				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tX]
				fmul	[ebx]SRenderCoord.v3Cam.tX							// x1 x+y z
				fxch	st(1)												// x+y x1 z

				faddp	st(2),st(0)											// x1 x+y+z

				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tY]
				fmul	[ebx]SRenderCoord.v3Cam.tY							// y1 x1 x+y+z
				fxch	st(2)												// x+y+z x1 y1

				fadd	dword ptr[edi+CRenderPolygon_plPlane_rD]			// x+y+z+d x1 y1

				fld		dword ptr[edi+CRenderPolygon_plPlane_d3Normal_tZ]
				fmul	[ebx]SRenderCoord.v3Cam.tZ							// z1 x+y+z+d x1 y1
				fxch	st(3)												// y1 x+y+z+d x1 z1
				faddp	st(2),st(0)											// x+y+z+d x1+y1 z1

				fstp	[f_temp]											// x1+y1 z1

				mov		ebx,[f_temp]										// Distance from plane
				
				cmp		ebx,esi												// Point behind Q?
				jna		P_VERT_OK2											// !(dist < -tol)

				fcompp														// dump stuff off stack
				jmp		ATTEMPT_SPLIT

P_VERT_OK2:
				dec		ecx
				jnz		VTX_LOOP_P2

				faddp	st(1),st(0)											// x+y+z

				// stall(2)

				fadd	dword ptr[edi+CRenderPolygon_plPlane_rD]			// x+y+z+d

				// stall(3)

				fstp	[f_temp]

				mov		ebx,[f_temp]										// Distance from plane
				
				cmp		ebx,esi												// Point behind Q?
				ja		ATTEMPT_SPLIT										// dist < -tol

P_IN_FRONT_OF_Q:
				// P is in front of Q.

			}

//			Assert(prpoly_p->bInFrontOf(prpoly_q) == 1);
//			Assert(bFastIntersect2D(prpoly_p, prpoly_q));

			//
			// Move Q to the end of the list.
			//

			// Mark Q.
			prpoly_q->bMovedInList = true;

			// Remove Q from its current position in the list.
			Erase(prpoly_q);

			// Reinsert P and Q into the list, with Q replacing P's position.
			PushFront(prpoly_p);
			PushFront(prpoly_q);

			// P cannot be scan converted yet.
			goto SKIP_PUSH_P;

ATTEMPT_SPLIT:
//			Assert(prpoly_q->bMovedInList || prpoly_p->bInFrontOf(prpoly_q) == 0);
//			Assert(bFastIntersect2D(prpoly_p, prpoly_q));

			#if (DEPTH_SORT_STATS)
				ctmr.Reset();
			#endif

			// Attempt to split P.
			if (bSplitPByQ(prpoly_p, prpoly_q))
			{
				#if (DEPTH_SORT_STATS)
					psSplit.Add(ctmr(), 1);
				#endif

				goto CONTINUE_WITH_Q;
			}

			// Attempt to split Q.
			if (bSplitQByP(prpoly_p, prpoly_q))
			{
				#if (DEPTH_SORT_STATS)
					psSplit.Add(ctmr(), 1);
				#endif

				goto CONTINUE_WITH_Q;
			}

			#if (DEPTH_SORT_STATS)
				psSplit.Add(ctmr(), 1);
			#endif

			// Increment iterator.
			__asm
			{
				mov		esi,[it_poly]

GET_NEXT_Q_ASM:
				mov		edi,[prpoly_p]
				mov		esi,[esi + iNEXT_OFFSET]

START_Q_LOOP:
				test	esi,esi
				jnz		BEGIN_WHILE_Q
			}

PUSH_P:
			// Record stat.
			iNumPolygonsEnd++;

			// Add finished polygon to scan conversion list.
			pplhHeap->darppolyPolygons << prpoly_p;

SKIP_PUSH_P:
			// Get the next polygon.
			prpoly_p = prpolyNextP();
		}
	}

#else // elif (VER_ASM)

	//******************************************************************************************
	void CDepthSort::Sort()
	{
		CRenderPolygon* prpoly_q;	// Polygon.

		if (iNumSortedLists == 2)
		{
			iNumA = pslParts[0].size();
			iNumB = pslParts[1].size();
		}
		if (iNumSortedLists == 4)
		{
			iNumA = pslParts[0].size();
			iNumB = pslParts[1].size();
			iNumC = pslParts[2].size();
			iNumD = pslParts[3].size();
		}

		// Get the first polygon.
		CRenderPolygon* prpoly_p = prpolyNextP();

		// Check for duplicate keys.
		//Assert(!bExists(prpoly_p));

		// While there are still polygons, depth sort them.
		while (prpoly_p)
		{
			// Select the iterator for the list the polygon belongs to.
			TIterator it_poly = itSelectQList(prpoly_p);
	
			#if bUSE_AREA_P
				// Skip the polygon if the area is to small.
				if (u4FromFloat(prpoly_p->fArea) <= u4FromFloat(fMinAreaForSort))
					goto PUSH_P;
			#endif // bUSE_AREA_P

			// Iterate through the polygon list.
			while (it_poly)
			{
				prpoly_q = *it_poly;

				#if bUSE_AREA_Q
					// Skip the polygon if the area is to small.
					if (u4FromFloat(prpoly_q->fArea) <= u4FromFloat(fMinAreaForSort))
						goto GET_NEXT_Q;
				#endif // bUSE_AREA_Q

				//
				// At the point where P's closest (largest) inverse Z position is farther
				// (smaller) than Q's farthest (smallest) Z position, terminate the loop
				// and indicate success.
				//
				if (u4FromFloat(prpoly_p->fMinZ) >= u4FromFloat(prpoly_q->rMaxLevel))
					break;

				// Steps 1 and 2: trivial rejection.
				if (prpoly_p->bScreenNoOverlap(prpoly_q))
					goto GET_NEXT_Q;

CONTINUE_WITH_Q:
				// Test if order is already correct or inconsequential.
				if (bCorrectOrder(prpoly_p, prpoly_q))
				{
					goto GET_NEXT_Q;
				}

				// Attempt to move Q to the end of the list.
				if (bReversePQ(prpoly_p, prpoly_q))
				{
					goto SKIP_PUSH_P;
				}

				// Attempt to split P.
				if (bSplitPByQ(prpoly_p, prpoly_q))
					goto CONTINUE_WITH_Q;

				// Attempt to split Q.
				if (bSplitQByP(prpoly_p, prpoly_q))
					goto CONTINUE_WITH_Q;

GET_NEXT_Q:
				++it_poly;
			}

PUSH_P:
			// Record stat.
			iNumPolygonsEnd++;

			// Add finished polygon to scan conversion list.
			pplhHeap->darppolyPolygons << prpoly_p;

SKIP_PUSH_P:
			// Get the next polygon.
			prpoly_p = prpolyNextP();
		}
	}

#endif // else

	//******************************************************************************************
	void CDepthSort::PushAll(CRenderPolygon* aprpoly[], int i_num)
	{
		//
		// Create all the child trees.
		//
		if (iNumSortedLists == 2)
		{
			// Worst case allocations.
			CRenderPolygon** aprpoly_new_a = (CRenderPolygon**)_alloca(i_num * sizeof(CRenderPolygon*));
			CRenderPolygon** aprpoly_new_b = (CRenderPolygon**)_alloca(i_num * sizeof(CRenderPolygon*));

			//
			// Optimized special case for quad-partition.
			//
			int i_sublist_size_a = 0;
			int i_sublist_size_b = 0;

			// It is more efficient to walk through the source list once.
			for (int i = 0; i < i_num; i++)
			{
				Assert(aprpoly[i]);

				//
				// If the polygon's bounding screen rectangle overlaps the sorting list's screen
				// rectangle, copy a pointer to the polygon to the new list.
				//
				if (pslParts[0].bOverlap(aprpoly[i]))
				{
					aprpoly_new_a[i_sublist_size_a++] = aprpoly[i];
				}

				if (pslParts[1].bOverlap(aprpoly[i]))
				{
					aprpoly_new_b[i_sublist_size_b++] = aprpoly[i];
				}
			}

			pslParts[0].BuildFromSortedList(aprpoly_new_a, i_sublist_size_a);
			pslParts[0].Bind(aprpoly_new_a, i_sublist_size_a);
			pslParts[0].SetNodeFromMain(aprpoly_new_a, i_sublist_size_a);
			pslParts[0].SetBeginEnd();

			pslParts[1].BuildFromSortedList(aprpoly_new_b, i_sublist_size_b);
			pslParts[1].Bind(aprpoly_new_b, i_sublist_size_b);
			pslParts[1].SetNodeFromMain(aprpoly_new_b, i_sublist_size_b);
			pslParts[1].SetBeginEnd();
		}
		else if (iNumSortedLists == 4)
		{
			// Worst case allocations.
			CRenderPolygon** aprpoly_new_a = (CRenderPolygon**)_alloca(i_num * sizeof(CRenderPolygon*));
			CRenderPolygon** aprpoly_new_b = (CRenderPolygon**)_alloca(i_num * sizeof(CRenderPolygon*));
			CRenderPolygon** aprpoly_new_c = (CRenderPolygon**)_alloca(i_num * sizeof(CRenderPolygon*));
			CRenderPolygon** aprpoly_new_d = (CRenderPolygon**)_alloca(i_num * sizeof(CRenderPolygon*));

			//
			// Optimized special case for quad-partition.
			//
			int i_sublist_size_a = 0;
			int i_sublist_size_b = 0;
			int i_sublist_size_c = 0;
			int i_sublist_size_d = 0;

			// It is more efficient to walk through the source list once.
			for (int i = 0; i < i_num; i++)
			{
				Assert(aprpoly[i]);

				//
				// If the polygon's bounding screen rectangle overlaps the sorting list's screen
				// rectangle, copy a pointer to the polygon to the new list.
				//
				if (pslParts[0].bOverlap(aprpoly[i]))
				{
					aprpoly_new_a[i_sublist_size_a++] = aprpoly[i];
				}

				if (pslParts[1].bOverlap(aprpoly[i]))
				{
					aprpoly_new_b[i_sublist_size_b++] = aprpoly[i];
				}

				if (pslParts[2].bOverlap(aprpoly[i]))
				{
					aprpoly_new_c[i_sublist_size_c++] = aprpoly[i];
				}

				if (pslParts[3].bOverlap(aprpoly[i]))
				{
					aprpoly_new_d[i_sublist_size_d++] = aprpoly[i];
				}
			}

			pslParts[0].BuildFromSortedList(aprpoly_new_a, i_sublist_size_a);
			pslParts[0].Bind(aprpoly_new_a, i_sublist_size_a);
			pslParts[0].SetNodeFromMain(aprpoly_new_a, i_sublist_size_a);
			pslParts[0].SetBeginEnd();

			pslParts[1].BuildFromSortedList(aprpoly_new_b, i_sublist_size_b);
			pslParts[1].Bind(aprpoly_new_b, i_sublist_size_b);
			pslParts[1].SetNodeFromMain(aprpoly_new_b, i_sublist_size_b);
			pslParts[1].SetBeginEnd();

			pslParts[2].BuildFromSortedList(aprpoly_new_c, i_sublist_size_c);
			pslParts[2].Bind(aprpoly_new_c, i_sublist_size_c);
			pslParts[2].SetNodeFromMain(aprpoly_new_c, i_sublist_size_c);
			pslParts[2].SetBeginEnd();

			pslParts[3].BuildFromSortedList(aprpoly_new_d, i_sublist_size_d);
			pslParts[3].Bind(aprpoly_new_d, i_sublist_size_d);
			pslParts[3].SetNodeFromMain(aprpoly_new_d, i_sublist_size_d);
			pslParts[3].SetBeginEnd();
		}
		else
		{
			CRenderPolygon** aprpoly_new = (CRenderPolygon**)_alloca(i_num * sizeof(CRenderPolygon*));

			// General case for any number of partitions.
			for (int i_list = 0; i_list < iNumSortedLists; i_list++)
			{
				int i_sublist_size = pslParts[i_list].iCopyMatchingElements
				(
					aprpoly,
					aprpoly_new,
					i_num
				);

				pslParts[i_list].BuildFromSortedList(aprpoly_new, i_sublist_size);
				pslParts[i_list].Bind(aprpoly_new, i_sublist_size);
				pslParts[i_list].SetNodeFromMain(aprpoly_new, i_sublist_size);
				pslParts[i_list].SetBeginEnd();
			}
		}

		// Create the main tree.
		slMain.BuildFromSortedList(aprpoly, i_num);
		slMain.Bind(aprpoly, i_num);
		slMain.SetBeginEnd();
	}


//**********************************************************************************************
//
// CPlaneToleranceSetting implementation.
//
	
	//******************************************************************************************
	CPlaneToleranceSetting::CPlaneToleranceSetting()
	{
		rNearZScale			= 2.5e-6f;
		rFarZScale			= 0.0078f;
		rNearZ				= 6.44e-4f;
		rFarZ				= 0.40f;
		rFarZNoDepthSort	= 0.5f;
		fPixelBuffer		= 0.65f;
		iMaxToDepthsort		= 900;
		iBinaryPartitionAt	= 1000;
		iQuadPartitionAt	= 1500;

		// Terrain tolerance values.		
		rNearZScaleTerrain = 1.0e-4f;
		rFarZScaleTerrain  = 0.02f;
		rNearZTerrain      = 6.44e-4f;
		rFarZTerrain       = 0.30f;
		bUseSeparateTolerances = false;

		// Set up the scale.
		SetLinearScale();
	}

	//******************************************************************************************
	void CPlaneToleranceSetting::SetQualityAdjustment
	(
		float f_adjusted_farclip, 
		float f_desired_farclip
	)
	{
		// Scale by original / adjusted.
		float f_scale = (f_desired_farclip / f_adjusted_farclip) *
					qdQualitySettings[iGetQualitySetting()].fSortDistanceScale;

		rNearZAdj			= Min(rNearZ * f_scale, 1.0f);
		rFarZAdj			= Min(rFarZ * f_scale, 1.0f);
		rFarZNoDepthSortAdj = Min(rFarZNoDepthSort * f_scale, 1.0f);

		rInvDeltaZAdj = 1.0f / (rFarZAdj - rNearZAdj);
	}

	//******************************************************************************************
	void CPlaneToleranceSetting::SetLinearScale()
	{
		Assert(rFarZ > rNearZ);

		etsScale = etsLINEAR;
		rInvDeltaZ = 1.0f / (rFarZ - rNearZ);
		rInvDeltaZTerrain = 1.0f / (rFarZTerrain - rNearZTerrain);
	}

	//******************************************************************************************
	void CPlaneToleranceSetting::SetInverseScale()
	{
		Assert(rFarZ > rNearZ);

		etsScale = etsINVERSE;
		rInvDeltaZ = 1.0f / (rFarZ - rNearZ);
		rInvDeltaZTerrain = 1.0f / (rFarZTerrain - rNearZTerrain);
	}

	//*****************************************************************************************
	//
	char* CPlaneToleranceSetting::pcSave
	(
		char* pc
	) const
	//
	// Saves the plane tolerance settings to a buffer.
	//
	//**************************************
	{
		// Save version number.
		pc = pcSaveT(pc, 1);

		pc = pcSaveT(pc, rNearZScale);
		pc = pcSaveT(pc, rFarZScale);
		pc = pcSaveT(pc, rNearZ);
		pc = pcSaveT(pc, rFarZ);
		pc = pcSaveT(pc, rFarZNoDepthSort);
		pc = pcSaveT(pc, fPixelBuffer);
		pc = pcSaveT(pc, rNearZScaleTerrain);
		pc = pcSaveT(pc, rFarZScaleTerrain);
		pc = pcSaveT(pc, rNearZTerrain);
		pc = pcSaveT(pc, rFarZTerrain);
		pc = pcSaveT(pc, iMaxToDepthsort);
		pc = pcSaveT(pc, iBinaryPartitionAt);
		pc = pcSaveT(pc, iQuadPartitionAt);
		pc = pcSaveT(pc, bUseSeparateTolerances);
		pc = pcSaveT(pc, etsScale);

		return pc;
	}

	//*****************************************************************************************
	//
	const char* CPlaneToleranceSetting::pcLoad
	(
		const char* pc
	)
	//
	// Loads the plane tolerance settings settings.
	//
	//**************************************
	{
		int iVersion, iDummy;

		pc = pcLoadT(pc, &iVersion);

		if (iVersion == 1)
		{
			pc = pcLoadT(pc, &rNearZScale);
			pc = pcLoadT(pc, &rFarZScale);
			pc = pcLoadT(pc, &rNearZ);
			pc = pcLoadT(pc, &rFarZ);
			pc = pcLoadT(pc, &rFarZNoDepthSort);
			pc = pcLoadT(pc, &fPixelBuffer);
			pc = pcLoadT(pc, &rNearZScaleTerrain);
			pc = pcLoadT(pc, &rFarZScaleTerrain);
			pc = pcLoadT(pc, &rNearZTerrain);
			pc = pcLoadT(pc, &rFarZTerrain);
			pc = pcLoadT(pc, &iDummy);
			pc = pcLoadT(pc, &iBinaryPartitionAt);
			pc = pcLoadT(pc, &iQuadPartitionAt);
			pc = pcLoadT(pc, &bUseSeparateTolerances);
			pc = pcLoadT(pc, &etsScale);

			switch (etsScale)
			{
				case etsLINEAR:
					SetLinearScale();
					break;
				case etsINVERSE:
					SetInverseScale();
					break;
			}
		}
		else
		{
			AlwaysAssert("Unknown version of plane tolerance settings");
		}

		return pc;
	}

//
// Global function implementations.
//

//**********************************************************************************************
//
int iScoreX(CRenderPolygon* aprpoly[], int i_num, int i_x)
//
// Returns an efficiency score for the given horizontal division.
//
//**********************************
{
	int i_score = 0;
	for (int i = 0; i < i_num; i += 4)
	{
		if (aprpoly[i]->v2ScreenMaxInt.tX < i_x)
			i_score--;
		if (aprpoly[i]->v2ScreenMinInt.tX >= i_x)
			i_score++;
	}
	return i_score;
}

//**********************************************************************************************
//
int iScoreY(CRenderPolygon* aprpoly[], int i_num, int i_y)
//
// Returns an efficiency score for the given vertical division.
//
//**********************************
{
	int i_score = 0;
	for (int i = 0; i < i_num; i++)
	{
		if (aprpoly[i]->v2ScreenMaxInt.tY < i_y)
			i_score--;
		if (aprpoly[i]->v2ScreenMinInt.tY >= i_y)
			i_score++;
	}
	return i_score;
}

//**********************************************************************************************
//
void BuildQuad(CDepthSort& rdepth, CRenderPolygon* aprpoly[], int i_num_polys)
//
// Builds an efficient quad partition.
//
//**********************************
{
	const int i_ok_score = 5;
	int i_mid_x = rdepth.iWidth >> 1;
	int i_bin = i_mid_x >> 1;

	// Calculate the initial score.
	int i_score = iScoreX(aprpoly, i_num_polys, i_mid_x);

	// Binary search for the best score.
	while (Abs(i_score) > 5 && i_bin > 0)
	{
		if (i_score < 0)
			i_mid_x -= i_bin;
		else
			i_mid_x += i_bin;
		i_bin = i_bin >> 1;
		i_score = iScoreX(aprpoly, i_num_polys, i_mid_x);
	}

	// Create a left and a right list based on the horizontal midpoint split.
	int i_num_left  = 0;
	int i_num_right = 0;
	int i_size_alloc = i_num_polys * sizeof(CRenderPolygon*) / 4;
	CRenderPolygon** aprpoly_left  = (CRenderPolygon**)_alloca(i_size_alloc);
	CRenderPolygon** aprpoly_right = (CRenderPolygon**)_alloca(i_size_alloc);

	for (int i = 0; i < i_num_polys; i += 4)
	{
		// Push into the left or right list.
		if (aprpoly[i]->v2ScreenMaxInt.tX < i_mid_x)
			aprpoly_left[i_num_left++] = aprpoly[i];
		if (aprpoly[i]->v2ScreenMinInt.tX >= i_mid_x)
			aprpoly_right[i_num_right++] = aprpoly[i];
	}

	// Initialize the left and right midpoint variables.
	int i_mid_y_left  = rdepth.iHeight >> 1;
	int i_mid_y_right = rdepth.iHeight >> 1;

	// Build an efficient left list.
	i_bin = i_mid_y_left >> 1;
	i_score = iScoreY(aprpoly_left, i_num_left, i_mid_y_left);
	while (Abs(i_score) > 3 && i_bin > 0)
	{
		if (i_score < 0)
			i_mid_y_left -= i_bin;
		else
			i_mid_y_left += i_bin;
		i_bin = i_bin >> 1;
		i_score = iScoreY(aprpoly_left, i_num_left, i_mid_y_left);
	}

	// Build an efficient right list.
	i_bin = i_mid_y_right >> 1;
	i_score = iScoreY(aprpoly_right, i_num_right, i_mid_y_right);
	while (Abs(i_score) > 3 && i_bin > 0)
	{
		if (i_score < 0)
			i_mid_y_right -= i_bin;
		else
			i_mid_y_right += i_bin;
		i_bin = i_bin >> 1;
		i_score = iScoreY(aprpoly_right, i_num_right, i_mid_y_right);
	}

	// Create the quad partition.
	rdepth.QuadPartition(i_mid_x, i_mid_y_left, i_mid_y_right);
}

//**********************************************************************************************
//
void BuildBinary(CDepthSort& rdepth, CRenderPolygon* aprpoly[], int i_num_polys)
//
// Builds an efficient binary partition.
//
//**********************************
{
	const int i_ok_score = 5;
	int i_mid_x = rdepth.iWidth >> 1;
	int i_bin = i_mid_x >> 1;

	// Calculate the initial score.
	int i_score = iScoreX(aprpoly, i_num_polys, i_mid_x);

	// Binary search for the best score.
	while (Abs(i_score) > 5 && i_bin > 0)
	{
		if (i_score < 0)
			i_mid_x -= i_bin;
		else
			i_mid_x += i_bin;
		i_bin = i_bin >> 1;
		i_score = iScoreX(aprpoly, i_num_polys, i_mid_x);
	}

	// Create the quad partition.
	rdepth.BinaryPartition(i_mid_x);
}

//**********************************************************************************************
//
void DepthSortPolygons(CPipelineHeap& rplh, const CCamera& cam)
//
// Implementation of the Depth Sort algorithm given in pages 673 to 675 in "Computer Graphics:
// "Principles and Practices" by Foley, Van Dam et al.
//
//**********************************
{
	CFPUState fpus;
	fpus.SetLowRes();
	fpus.SetTruncate();
	
	bNoCPPSubtriangle = false;

#if (DEPTH_SORT_STATS)
	CCycleTimer ctmr;
#endif

	// Compute adjusted values based on adjusted far clip.
	ptsTolerances.SetQualityAdjustment(cam.campropGetProperties().rFarClipPlaneDist, 
									cam.campropGetProperties().rDesiredFarClipPlaneDist);

	// Set the outward ajust value for hardware.
	fAdjustOut = 0.0f;

	// Ensure TScreen is a floating point typedef.
	Assert(CIntFloat(CViewport::TScreen(1)).i4Int == CIntFloat(1.0f).i4Int);

	int i_width  = iPosFloatCast(cam.campropGetProperties().vpViewport.scWidth);
	int i_height = iPosFloatCast(cam.campropGetProperties().vpViewport.scHeight);
	SetClipRegion(0.0f, 0.0f, float(i_width), float(i_height));

	int i_hmid = i_width >> 1;
	int i_vmid = i_height >> 1;

	CDepthSort depth(&rplh, &cam, i_width, i_height);	// Depth sort object.

	// Allocate memory from the fast heap for depth sorting.
	TTreeNode* panodes = new (fhPPolys) TTreeNode[iNUM_MAX_POLYSTOSORT];
	depth.slMain.SetAllocator((void*)panodes, iNUM_MAX_POLYSTOSORT);

	// Reset the key values for the current frame.
	CSortKey::Reset();

	//
	// Create a temporary pointer array and sort the array.
	//

	// Set the pointer values in the array.
	int             i_num_start = (int)rplh.parpolyPolygons().uLen;
	CRenderPolygon* prpoly      = rplh.parpolyPolygons();
	CRenderPolygon* prpoly_end  = prpoly + i_num_start;
	CLArray(CRenderPolygon*, aprpoly_over, i_num_start);
	aprpoly_over.uLen = 0;

	// Create the temporary array.
	uint             u_start_polys = rplh.darppolyPolygons.uLen;
	CRenderPolygon** aprpoly       = (CRenderPolygon**)_alloca(i_num_start * sizeof(CRenderPolygon*));

	CVector2<int> v2_av(0, 0);

	//
	// Build a trivially sorted polygon set.
	//
	int i_num_polys;
	for (i_num_polys = 0; prpoly < prpoly_end; prpoly++)
	{
		// Initialize member variables used for sorting.
		if (prpoly->bAccept)
		{
			// Insert polygon.
			aprpoly[i_num_polys] = prpoly;

			// Get average position for building an efficient quad.
			v2_av += prpoly->v2ScreenMinInt + prpoly->v2ScreenMaxInt;

			// Increment the polygon count.
			i_num_polys++;
		}
	}

#if (DEPTH_SORT_STATS)
	psSetup.Add(ctmr(), i_num_polys);
#endif

#if (DEPTH_SORT_STATS)
	psPlaneSetup.Add(ctmr(), i_num_polys);
#endif
	
	// Sort the polygons using STL's QSort routine.
	sort(aprpoly, aprpoly + i_num_polys, CPolyFarZ());

#if (DEPTH_SORT_STATS)
	psQuickSort.Add(ctmr(), i_num_polys);
#endif

	// Set statistics.
	iNumPolygonsStart += i_num_start;
	iNumInitialize    += i_num_start;
	iNumCulled        += i_num_start - i_num_polys;
	
	// Restore the temporary array (don't de-commit).
	rplh.darppolyPolygons.Reset(u_start_polys, false, -1);

	// Push unsorted polygons to the render queue.
	int i;
	for (i = 0; i < int(aprpoly_over.uLen); ++i)
	{
		// Record stat.
		iNumPolygonsEnd++;
		rplh.darppolyPolygons << aprpoly_over[i];
	}

	// Push polygons over the limit to the render queue.
	for (i = i_num_polys - 1; i >= ptsTolerances.iMaxToDepthsort; i--)
	{
		// Record stat.
		iNumPolygonsEnd++;

		// Add finished polygon to scan conversion list.
		rplh.darppolyPolygons << aprpoly[i];
	}
	i_num_polys = min(i_num_polys, ptsTolerances.iMaxToDepthsort);

	// Push as many far away polygons as possible to the render list.
	{
		while (i_num_polys > 0 && aprpoly[i_num_polys - 1]->fMaxZ > ptsTolerances.rFarZNoDepthSortAdj)
		{
			rplh.darppolyPolygons << aprpoly[i_num_polys - 1];
			--i_num_polys;
		}
	}

	// Do nothing if there are no polygons to render.
	if (i_num_polys <= 0)
	{
		fhPPolys.Reset( 0, iNUM_MAX_POLYSTOSORT * sizeof(TTreeNode) );
		return;
	}

	//
	// Create the planes for the polygons.
	//
#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
	__asm femms
#endif

	for (int i_poly = 0; i_poly < i_num_polys; i_poly++)
		aprpoly[i_poly]->SetPlane();

#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
	__asm femms
#endif


#if (DEPTH_SORT_STATS)
	ctmr.Reset();
#endif

#if (iMAX_SORT_LISTS > 1)
	// Build an optimal quad partition.
	if (i_num_polys > ptsTolerances.iBinaryPartitionAt)
	{
		if (i_num_polys > ptsTolerances.iQuadPartitionAt)
			BuildQuad(depth, aprpoly, i_num_polys);
		else
			BuildBinary(depth, aprpoly, i_num_polys);
	}

#if (DEPTH_SORT_STATS)
	psBuildQuad.Add(ctmr(), i_num_polys);
#endif

#endif

	// Establish keys and set the watermark.
	for (i = 0; i < i_num_polys; i++)
		depth.InitializePolygon(aprpoly[i]);

#if (DEPTH_SORT_STATS)
	ctmr.Reset();
#endif

	// Copy pointers into the STL list.
	depth.PushAll(aprpoly, i_num_polys);

#if (DEPTH_SORT_STATS)
	psPushAll.Add(ctmr(), i_num_polys);
#endif

	// Resolve sorting ambiguities.
	depth.Sort();

#if (DEPTH_SORT_STATS)
	psDepthSort.Add(ctmr(), i_num_polys);
#endif

	// Reset the fast heap for use for the next frame, leaving enough memory commited
	// for the default sorted poly count, set above.
	// Any additional memory allocated by the TreeList will be decommited and returned
	// to the free pool
	fhPPolys.Reset(0, iNUM_MAX_POLYSTOSORT * sizeof(TTreeNode));
}


//
// Global variables.
//
CPlaneToleranceSetting ptsTolerances;
