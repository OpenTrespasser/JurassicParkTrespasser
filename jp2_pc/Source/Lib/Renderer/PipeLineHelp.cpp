/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of PipeLineHelp.hpp.
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/PipeLineHelp.cpp                                         $
 * 
 * 19    98.09.24 1:40a Mmouni
 * Shrunk CRenderPolygon by a large amount.
 * Multiple list depth sort stuff is now on a compile switch.
 * 
 * 18    7/29/98 1:57p Pkeet
 * Added a parameter to the 'SetMipLevel' member function of 'CRenderPolygon' to reduce the
 * burden on the VM loader by making VM requests only when the Direct3D version is required and
 * is present.
 * 
 * 17    5/12/98 3:04p Pkeet
 * Added hack to fix crash bug.
 * 
 * 16    1/25/98 3:40p Pkeet
 * 'MakePointerList' only pushes polygons that are accepted onto the polygon list.
 * 
 * 15    97/07/07 14:19 Speter
 * Fixed include file order.  Removed bvboxEnclosing.
 * 
 * 14    97/05/23 17:50 Speter
 * Removed SRenderTriangle, made functions work with CRenderPolygon.  This is experimental code
 * only; now use custom functions for min/max polygon Z, removed from SRenderTriangle.
 * 
 * 13    4/30/97 9:19p Rwyatt
 * Moved esfView member variable from CPartition and put it into the list that the partition
 * functions return. These functions used to return a list of CPartition* now they return a list
 * of structures that contain a CPartition* and esfSideOf.
 * 
 * 12    97-04-14 20:30 Speter
 * Removed unneeded include.
 * 
 * 11    97/04/05 2:48p Pkeet
 * Changed interface to use the pipeline heap.
 * 
 * 10    97/04/04 4:33p Pkeet
 * Made use of the 'u4FromFloat' macro.
 * 
 * 9     97-04-04 12:38 Speter
 * A big change: now pipeline uses CRenderPolygon rather than SRenderTriangle (currently always
 * as a triangle).  Changed associated variable names and comments.
 * 
 * 8     97/04/03 7:52p Pkeet
 * Added the bounding box enclosing function.
 * 
 * 7     97/04/02 6:47p Pkeet
 * Added functions for calling an external sort.
 * 
 * 6     97-04-02 16:07 Speter
 * Removed the call to Set() in the pointer array functions; now takes pre-allocated arrays.
 * 
 * 5     97/04/02 1:34p Pkeet
 * Added an extra assert to ensure the inverse Z values used for comparison are always positive.
 * 
 * 4     97/04/02 12:16p Pkeet
 * Added an assert.
 * 
 * 3     97/04/02 12:13p Pkeet
 * Made 'SetCompareValues' internal to the pipeline help module.
 * 
 * 2     97/04/01 6:56p Pkeet
 * Fixed front-to-back bug.
 * 
 * 1     97/04/01 6:35p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "PipeLineHelp.hpp"
#include "PipeLine.hpp"
#include "RenderDefs.hpp"
#include "Camera.hpp"

#include <list>
#include <set>

//
// Defines.
//

// Switch to use an external sort routine.
#define bUSE_EXTERNAL_SORT (0)

// If an external sort is required, an include for that sort is required.
#if bUSE_EXTERNAL_SORT
	#include "Lib/Std/Sort.hpp"
#endif // bUSE_EXTERNAL_SORT

extern float fAdjustOut;
extern bool bNoCPPSubtriangle;


//******************************************************************************************
//
// Global functions for per-polygon coarse sorting (for testing purposes only).
// These sort functions borrow the CRenderPolygon.fMaxZ variable, which is normally used in
// sorting.
//

	//******************************************************************************************
	//
	void SetCompareInvZMin
	(
		CRenderPolygon& rp
	)
	//
	// Sets rp.fMaxZ to the minimum inv z value for the polygon for doing rough triangle sorting.
	//
	//**************************************
	{
		rp.fMaxZ = rp.paprvPolyVertices[0]->v3Screen.tZ;

		for (uint u = 1; u < rp.paprvPolyVertices.uLen; u++)
			SetMin(rp.fMaxZ, rp.paprvPolyVertices[u]->v3Screen.tZ);

		// Other initialization.
		rp.SetArea();
		rp.SetPlane();
		rp.bCalculateScreenExtents();
		rp.ReduceFeatures();
		rp.keySort.SetKeyDown();

#if (iMAX_SORT_LISTS > 1)
		rp.iNumSortLists = 0;
#endif

		rp.SetMipLevel(false);
	}

	//******************************************************************************************
	//
	void SetCompareInvZMax
	(
		CRenderPolygon& rp
	)
	//
	// Sets rp.fMaxZ to the maximum inv z value for the polygon for doing rough triangle sorting.
	//
	//**************************************
	{
		rp.fMaxZ = rp.paprvPolyVertices[0]->v3Screen.tZ;

		for (uint u = 1; u < rp.paprvPolyVertices.uLen; u++)
			SetMax(rp.fMaxZ, rp.paprvPolyVertices[u]->v3Screen.tZ);

		// Other initialization.
		rp.SetArea();
		rp.SetPlane();
		rp.bCalculateScreenExtents();
		rp.ReduceFeatures();
		rp.keySort.SetKeyDown();

#if (iMAX_SORT_LISTS > 1)
		rp.iNumSortLists = 0;
#endif

		rp.SetMipLevel(false);
	}

//
// Internal class definitions.
//

//******************************************************************************************
//
class CPolyCompLess
//
// Less than comparison class for CRenderPolygon pointers.
//
//**************************************
{
public:

	//**************************************************************************************
	//
	bool operator()(const CRenderPolygon* prpoly_0, const CRenderPolygon* prpoly_1) const
	//
	// Returns 'true' if the first triangle has a lower inverse z value than the second
	// triangle.
	//
	// Notes:
	//		Member function version for STL.
	//
	//**************************************
	{
		// The aliased floating point values should never be negative.
		Assert(((u4FromFloat(prpoly_0->fMaxZ) | u4FromFloat(prpoly_1->fMaxZ)) & 0x80000000L) == 0);
		return u4FromFloat(prpoly_0->fMaxZ) > u4FromFloat(prpoly_1->fMaxZ);
	}

	//**************************************************************************************
	//
	static bool bCompare(const CRenderPolygon* prpoly_0, const CRenderPolygon* prpoly_1)
	//
	// Returns 'true' if the first triangle has a lower inverse z value than the second
	// triangle.
	//
	// Notes:
	//		Member function version for templated sort function.
	//
	//**************************************
	{
		// The aliased floating point values should never be negative.
		Assert(((u4FromFloat(prpoly_0->fMaxZ) | u4FromFloat(prpoly_1->fMaxZ)) & 0x80000000L) == 0);
		return u4FromFloat(prpoly_0->fMaxZ) > u4FromFloat(prpoly_1->fMaxZ);
	}

};

//******************************************************************************************
//
class CPolyCompGT
//
// Greater than comparison class for CRenderPolygon pointers.
//
//**************************************
{
public:

	//**************************************************************************************
	//
	bool operator()(const CRenderPolygon* prpoly_0, const CRenderPolygon* prpoly_1) const
	//
	// Returns 'true' if the first triangle has a lower inverse z value than the second
	// triangle.
	//
	// Notes:
	//		Member function version for STL.
	//
	//**************************************
	{
		// The aliased floating point values should never be negative.
		Assert(((u4FromFloat(prpoly_0->fMaxZ) | u4FromFloat(prpoly_1->fMaxZ)) & 0x80000000L) == 0);
		return u4FromFloat(prpoly_0->fMaxZ) < u4FromFloat(prpoly_1->fMaxZ);
	}

	//**************************************************************************************
	//
	static bool bCompare(const CRenderPolygon* prpoly_0, const CRenderPolygon* prpoly_1)
	//
	// Returns 'true' if the first triangle has a lower inverse z value than the second
	// triangle.
	//
	// Notes:
	//		Member function version for templated sort function.
	//
	//**************************************
	{
		// The aliased floating point values should never be negative.
		Assert(((u4FromFloat(prpoly_0->fMaxZ) | u4FromFloat(prpoly_1->fMaxZ)) & 0x80000000L) == 0);
		return u4FromFloat(prpoly_0->fMaxZ) < u4FromFloat(prpoly_1->fMaxZ);
	}

};


//
// Function implementations.
//

//**********************************************************************************************
void MakePointerList(CPArray<CRenderPolygon> parpoly, CPipelineHeap& rplh)
{
	// Set the pointer values in the array.
	CRenderPolygon* prpoly     = parpoly;
	CRenderPolygon* prpoly_end = prpoly + parpoly.uLen;

	for (; prpoly < prpoly_end; prpoly++)
		if (prpoly->bAccept)
			rplh.darppolyPolygons << prpoly;
}

//**********************************************************************************************
//
void SetCompareValues
(
	CPArray<CRenderPolygon> parpoly	// Pointer to an array of render triangles.
)
//
// Generates an inverse Z value for comparison for sorts and Z buffer rasterizing.
//
//**************************************
{
	for (uint u_tri = 0; u_tri < parpoly.uLen; u_tri++)
		SetCompareInvZMax(parpoly[u_tri]);
}


//
// Alternative functions to build a sorted list.
//

#if 0

	//
	// Externally defined global function.
	//

	//******************************************************************************************
	void MakeSortedPointerList(CPArray<CRenderPolygon> parpoly, CPipelineHeap& rplh,
							   bool bFrontToBack)
	{
		CFPUState fpus;
		fpus.SetLowRes();
		fpus.SetTruncate();

		// Build the pointer list and compare values separately.
		MakePointerList(parpoly, rpaprpoly);
		SetCompareValues(parpoly);

		//
		// Feed the array to the external sort.
		//

		// Choose front-to-back or back-to-front compares.
		if (bFrontToBack)
		{
			SortLargeList(rpaprpoly, rpaprpoly.uLen, (CPolyCompLess*)0);
		}
		else
		{
			SortLargeList(rpaprpoly, rpaprpoly.uLen, (CPolyCompGT*)0);
		}
	}

#else // bUSE_EXTERNAL_SORT

	//
	// Externally defined global function.
	//

	//******************************************************************************************
	void MakeSortedPointerList(CPArray<CRenderPolygon> parpoly, CPipelineHeap& rplh,
							   bool bFrontToBack)
	{
		// A basic assumption.
		Assert(sizeof(TReal) == sizeof(uint32));
		bNoCPPSubtriangle = true;

		CFPUState fpus;
		fpus.SetLowRes();
		fpus.SetTruncate();

		// Set the outward ajust value for hardware.
		fAdjustOut = 0.0f;

		if (bFrontToBack)
		{
			std::multiset<CRenderPolygon*, CPolyCompLess> mset_rpoly;
			std::multiset<CRenderPolygon*, CPolyCompLess>::iterator itmset;

			// Set the pointer values in the array.
			int i_start = rplh.darppolyPolygons.uLen;
			CRenderPolygon* prpoly     = parpoly;
			CRenderPolygon* prpoly_end = prpoly + parpoly.uLen;

			for (; prpoly < prpoly_end; prpoly++)
			{
				SetCompareInvZMax(*prpoly);
				if (prpoly->bAccept)
					mset_rpoly.insert(prpoly);
			}

			//rplh.darppolyPolygons.Reset(0);
			for (itmset = mset_rpoly.begin(); itmset != mset_rpoly.end(); itmset++)
			{
				rplh.darppolyPolygons << *itmset;
			}
		}
		else
		{
			std::multiset<CRenderPolygon*, CPolyCompGT> mset_rpoly;
			std::multiset<CRenderPolygon*, CPolyCompGT>::iterator itmset;

			// Set the pointer values in the array.
			int i_start = rplh.darppolyPolygons.uLen;
			CRenderPolygon* prpoly     = parpoly;
			CRenderPolygon* prpoly_end = prpoly + parpoly.uLen;

			for (; prpoly < prpoly_end; prpoly++)
			{
				SetCompareInvZMax(*prpoly);
				if (prpoly->bAccept)
					mset_rpoly.insert(prpoly);
			}

			//rplh.darppolyPolygons.Reset(0);
			for (itmset = mset_rpoly.begin(); itmset != mset_rpoly.end(); itmset++)
			{
				rplh.darppolyPolygons << *itmset;
			}
		}
	}


#endif // bUSE_EXTERNAL_SORT
