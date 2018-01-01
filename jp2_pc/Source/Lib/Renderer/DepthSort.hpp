/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/DepthSort.hpp                                            $
 * 
 * 26    9/24/98 6:06p Asouth
 * changed inline asm ; comments to //
 * 
 * 25    98.09.24 1:40a Mmouni
 * Shrunk CRenderPolygon by a large amount.
 * Multiple list depth sort stuff is now on a compile switch.
 * 
 * 24    98.09.04 4:26p Mmouni
 * Added adjusted distance value support.
 * 
 * 
 * 23    98.04.01 7:28p Mmouni
 * Added save/load methods to CPlaneToleranceSetting.
 * 
 * 22    98.02.04 1:58p Mmouni
 * Added new settings for depth sort partitoning.
 * 
 * 21    98.01.05 5:59p Mmouni
 * Added fast TreeList from sorted array routine.
 * 
 * 20    11/09/97 10:05p Gfavor
 * Simplified SetTolerance code and converted it to 3DX.
 * 
 * 19    97/07/30 2:43p Pkeet
 * Made the tolerance function use the 'bUseSeparateTolerances' flag.
 * 
 * 18    97/07/30 1:47p Pkeet
 * Added support in the settings structure for a separate tolerance calculation for terrain
 * polygons.
 * 
 * 17    97/06/11 12:30 Speter
 * Added compile switch for calculating screen extents in pipeline.
 * 
 * 16    97/06/10 15:48 Speter
 * Now takes camera as an argument (for new vertex projection).  
 * 
 * 15    97/05/27 2:40p Pkeet
 * Made the building of subtrees faster.
 * 
 * 14    97/05/26 4:19p Pkeet
 * Added the 'iMaxToDepthsort' parameter.
 * 
 * 13    97/05/23 6:25p Pkeet
 * Moved the tolerance object here from the implementation module.
 * 
 * 12    97/05/21 4:13p Pkeet
 * Added 'fPixelBuffer.'
 * 
 * 11    97/05/20 3:14p Pkeet
 * Added the variable for the cutting off depth sorting.
 * 
 * 10    97/05/20 11:40a Pkeet
 * Added variable plane tolerances for depth sorting.
 * 
 * 9     97/05/15 5:03p Pkeet
 * Now uses placement information for iterators.
 * 
 * 8     97/05/14 8:12p Pkeet
 * Fixed bug in multiple list use.
 * 
 * 7     97/05/14 4:03p Pkeet
 * Replaced the STL map with the binary tree and list template.
 * 
 * 6     97/05/08 1:01p Pkeet
 * Created the sort list class.
 * 
 * 5     97/05/06 6:59p Pkeet
 * Implemented an STL map version of the depth sort using a key. Added an interface for
 * obtaining the screen width and height.
 * 
 * 4     97/04/28 5:46p Pkeet
 * Moved tolerance definitions here.
 * 
 * 3     97/04/16 6:25p Pkeet
 * Added the 'bVerifyOrder' function.
 * 
 * 2     97/04/06 1:52p Pkeet
 * Added function to write depthsort stats.
 * 
 * 1     97/04/05 2:49p Pkeet
 * Initial implementation.
 *
 **********************************************************************************************/

#ifndef LIB_RENDERER_DEPTHSORT_HPP
#define LIB_RENDERER_DEPTHSORT_HPP


//
// Necessary includes.
//
#include "Lib/Std/TreeList.hpp"
#include "ScreenRender.hpp"
#include "AsmSupport.hpp"

//
// Define a switch to toggle between old clipping and new clipping.
//

#define bEXTENTS_IN_PIPELINE	(0)

// Default plane tolerance for depth sorting.
#define fDEFAULT_PLANE_TOLERANCE (0.000005f)

// Fuzziness permited.
const float fFUZZY_SIDE = fDEFAULT_PLANE_TOLERANCE;


//
// Forward declaration of classes.
//
class CPipelineHeap;


template<class T> bool bIsValidPtr(T* pt)
{
	*pt = *pt;
	return true;
}

//
// Class definitions.
//

//******************************************************************************************
//
class CSortList : public CTreeList<CSortKey, CRenderPolygon*>
//
// Depth sorting list.
//
// Prefix: sl
//
//**************************************
{
public:

	CVector2<int> v2ScreenMinInt;	// Minimum screen coordinates as an integer.
	CVector2<int> v2ScreenMaxInt;	// Maximum screen coordinates as an integer.

public:

	//**************************************************************************************
	//
	// Constructor.
	//

	// Default constructor.
	CSortList()
	{
	}

	//**************************************************************************************
	//
	// Member functions.
	//

	// Constructor with a size.
	void Set(const CVector2<int>& v2_min, const CVector2<int>& v2_max)
	{
		v2ScreenMinInt = v2_min;
		v2ScreenMaxInt = v2_max;
	}

	//**************************************************************************************
	//
	bool bOverlap
	(
		const CRenderPolygon* prpoly	// Pointer to the polygon to add.
	) const
	//
	// Returns 'true' if the polygon's screen space rectangle overlaps this list's screen
	// space rectangle.
	//
	//**************************************
	{
		Assert(prpoly);

		// X Overlap.
		if (v2ScreenMinInt.tX > prpoly->v2ScreenMaxInt.tX)
			return false;
		if (v2ScreenMaxInt.tX < prpoly->v2ScreenMinInt.tX)
			return false;

		// Y Overlap.
		if (v2ScreenMinInt.tY > prpoly->v2ScreenMaxInt.tY)
			return false;
		if (v2ScreenMaxInt.tY < prpoly->v2ScreenMinInt.tY)
			return false;

		// Success.
		return true;
	}

	//**************************************************************************************
	//
	bool bGetLast
	(
		const CSortKey& key,
		      CSortKey& rkey_next
	)
	//
	// Erases the entry with the specified key.
	//
	//**************************************
	{
		iterator it = find(key);
		if (!it)
			return false;
		--it;
		if (!it)
			return false;
		rkey_next = (*it)->keySort;
		return true;
	}

#if (iMAX_SORT_LISTS > 1)
	//**************************************************************************************
	//
	void SetMemberData
	(
		CRenderPolygon* prpoly	// Pointer to the polygon to add.
	)
	//
	// Adds a polygon to this list.
	//
	//**************************************
	{
		Assert(prpoly);

		// Do nothing if the polygon cannot be a member.
		if (!bOverlap(prpoly))
			return;

		// Add a pointer to this list to the polygon.
		prpoly->pslMemberOf[prpoly->iNumSortLists++] = this;
	}
#endif
	
	//******************************************************************************************
	//
	void PushArray
	(
		CRenderPolygon* aprpoly[],
		uint u_num,
		iterator it
	);
	//
	// 
	//
	//**************************************
	
	//******************************************************************************************
	//
	void PushArray
	(
		CRenderPolygon* aprpoly[],
		uint u_num
	);
	//
	// 
	//
	//**************************************

	//******************************************************************************************
	//
	void BuildFromSortedList
	(
		CRenderPolygon* aprpoly[],		// Sorted input array.
		uint u_num						// Number of elements in the input array.
	);
	//
	// Quickly build the list from a sorted array.
	//
	//**************************************


	//******************************************************************************************
	//
	void Bind
	(
		CRenderPolygon* aprpoly[],	// Array of pointers to sorted polygons.
		uint u_num					// Number of elements in the array.
	);
	//
	// 
	//
	//**************************************

	//******************************************************************************************
	//
	void SetNodeFromMain
	(
		CRenderPolygon* aprpoly[],	// Array of pointers to sorted polygons.
		uint u_num					// Number of elements in the array.
	);
	//
	// 
	//
	//**************************************

	//******************************************************************************************
	//
	int iCopyMatchingElements
	(
		CRenderPolygon* aprpoly_master[],	// Array of pointers to sorted polygons.
		CRenderPolygon* aprpoly[],			// Array of pointers to sorted polygons.
		uint u_num							// Number of elements in the array.
	);
	//
	// 
	//
	//**************************************

};


//**********************************************************************************************
//
enum EToleranceScale
//
// Prefix: ets
//
// Determines the type of scale plane tolerances will be calculated on.
//
//**************************************
{
	etsNONE,	// Tolerances are always set to the start value.
	etsLINEAR,	// Tolerances are linear with Z.
	etsINVERSE	// Tolerances are based on the inverse Z value.
};


//**********************************************************************************************
//
class CPlaneToleranceSetting
//
// Global object determines plane tolerances for depth sorting.
//
// Prefix: pts
//
//**************************************
{
private:

	EToleranceScale etsScale;

public:

	TReal rNearZScale;				// Plane tolerance at and before rNearZ.
	TReal rFarZScale;				// Plane tolerance at and after rFarZ.
	TReal rNearZ;
	TReal rFarZ;
	TReal rNearZScaleTerrain;
	TReal rFarZScaleTerrain;
	TReal rNearZTerrain;
	TReal rFarZTerrain;
	TReal rFarZNoDepthSort;			// Distance at which we no longer depth sort.
	float fPixelBuffer;
	int   iMaxToDepthsort;			// Maximum number of polygons that will be depthsorted.
	int	  iBinaryPartitionAt;		// Number of polygons to start using a binary partition at.
	int   iQuadPartitionAt;			// Number of polygons to start using a quad. partition at.
	bool  bUseSeparateTolerances;	// Flag indicates if separate tolerances are to be used
									// for terrain.

	TReal rNearZAdj;				// Adjusted version of the above value.
	TReal rFarZAdj;					// Adjusted version of the above value.
	TReal rFarZNoDepthSortAdj;		// Adjusted version of the above value.

private:

	TReal rInvDeltaZ;				// 1 / (rFarZ-rNearZ)
	TReal rInvDeltaZTerrain;

	TReal rInvDeltaZAdj;			// Adjusted version of the above value.
	
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CPlaneToleranceSetting();


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SetQualityAdjustment
	(
		float f_adjusted_farclip,		// Adjusted far clipping value.
		float f_desired_farclip			// Far clipping value before adjustment.
	);
	//
	// Set adjusted depth sort values based on adjusted far clipping plane to keep
	// the depths sort values at the same absolute distance.
	//
	//**************************************

	//******************************************************************************************
	//
	EToleranceScale etsGet
	(
	) const
	//
	// Returns the current scale type.
	//
	//**************************************
	{
		return etsScale;
	}

	//******************************************************************************************
	//
	void SetLinearScale
	(
	);
	//
	// Performs the necessary calculations to use the scale linearly.
	//
	//**************************************

	//******************************************************************************************
	//
	void SetInverseScale
	(
	);
	//
	// Performs the necessary calculations to use the scale using inverse Z.
	//
	//**************************************

	//******************************************************************************************
	//
	void SetTolerance
	(
		CRenderPolygon* prpoly	// Polygon to set the tolerance for.
	) const
	//
	// Determines the plane equation tolerance for the given polygon.
	//
	//**************************************
	{
	#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		typedef CPlaneToleranceSetting tdCPlaneToleranceSetting;

		__asm
		{
		jmp		StartAsm

		align	16
			nop									// establish 3 byte starting code offset
			nop

		StartAsm:		

		// femms								// caller responsible for this

		mov		ecx,[this]						// ecx= This ptr

		mov		eax,[prpoly]					// eax= prpoly ptr

		movd	mm2,[ecx]tdCPlaneToleranceSetting.rNearZAdj	// m2= rNearZ

		movd	mm0,[eax]CRenderPolygon.fMinZ				// m0= fMinZ

		movd	mm3,[ecx]tdCPlaneToleranceSetting.rFarZAdj	// m3= rFarZ

		movd	mm4,[ecx]tdCPlaneToleranceSetting.rInvDeltaZAdj
		;										// m4= rInvDeltaZ
		pfmax	(m0,m2)							// m0= (fMinZ <= rNearZ) ? rNearZ : fMinZ
	
		movd	mm6,[ecx]tdCPlaneToleranceSetting.rNearZScale
		;										// m6= rNearZScale
		pfmin	(m0,m3)							// m0= (fMinZ >= rFarZ)  ? rFarZ  : fMinZ

		movd	mm7,[ecx]tdCPlaneToleranceSetting.rFarZScale
		;										// m7= rFarZScale
		pfsub	(m0,m2)							// m0= ("fMinZ"-rNearZ)

		
		pfmul	(m0,m4)							// m0= f_position
		pfsub	(m7,m6)							// m7= rFarZScale-rNearZScale

		pfmul	(m0,m7)							// m0= f_position*(rFarZScale-rNearZScale)

		pfadd	(m0,m6)							// m0= rPlaneTolerance
		movd	[eax]CRenderPolygon.rPlaneTolerance,mm0

		// femms								;caller responsible for this
		}

	#else // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		if (prpoly->fMinZ <= rNearZAdj)
		{
			prpoly->rPlaneTolerance = rNearZScale;
		}
		else if (prpoly->fMinZ >= rFarZAdj)
		{
			prpoly->rPlaneTolerance = rFarZScale;
		}
		else
		{
			float f_position = (prpoly->fMinZ - rNearZAdj) * rInvDeltaZAdj;
			prpoly->rPlaneTolerance = f_position * (rFarZScale - rNearZScale) +
				                      rNearZScale;
		}

	#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		Assert(prpoly);
	}

	//*****************************************************************************************
	//
	char* pcSave
	(
		char* pc
	) const;
	//
	// Saves the plane tolerance settings to a buffer.
	//
	//**************************************

	//*****************************************************************************************
	//
	const char* pcLoad
	(
		const char* pc
	);
	//
	// Loads the plane tolerance settings from a buffer.
	//
	//**************************************
};

//
// Global function prototypes.
//

//**********************************************************************************************
//
void DepthSortPolygons
(
	CPipelineHeap& rplh,	// Current pipeline heap.
	const CCamera& cam		// Current projecting camera.
);
//
// Returns an array of pointers to polygons depth sorted in a back-to-front order.
//
//**************************************

//**********************************************************************************************
//
bool bVerifyOrder
(
	CPArray<CRenderPolygon*> paprpoly	// Array of pointers to the ordered list of polygons.
);
//
// Returns 'true' if the depth-sorted order is correct.
//
//**************************************

//**********************************************************************************************
//
void DepthSortStatsWriteAndClear
(
);
//
// Writes the depthsort stats to the depthsort console window and clears the stats.
//
//**************************************


//
// Global variables.
//

// Object to determine global plane tolerances for depth sorting.
extern CPlaneToleranceSetting ptsTolerances;
extern uint32 u4ConvexHullUniqueMembership;
extern uint32 u4ConvexHullTestMembership;


#endif // LIB_RENDERER_DEPTHSORT_HPP
