/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of PipeLineHeap.hpp.
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/PipeLineHeap.cpp                                         $
 * 
 * 11    98.09.23 2:59p Mmouni
 * Added Decommit.
 * 
 * 10    98/09/16 15:00 Speter
 * PipelineHeap reset now does not decommit. Improves the pipeline speed by over 30%. Like that?
 * Good enough for ya?
 * 
 * 9     98/09/14 1:44 Speter
 * Now save and restore CDArray commit sizes as well as lengths.
 * 
 * 8     8/25/98 4:40p Rwyatt
 * Reset heap
 * 
 * 7     97/11/14 17:55 Speter
 * Removed now-unused fhGeneric.  Now has 4 points per polygon rather than 3, for safety.
 * 
 * 6     97-05-06 16:36 Speter
 * Lowered heap limits to reasonable values, to save VM.
 * 
 * 5     97-04-09 16:17 Speter
 * Added vertex pointers to heap.  Removed unnecessary includes.
 * 
 * 4     97/04/05 2:48p Pkeet
 * Added a static array of 'CRenderPolygon' pointers.
 * 
 * 3     97/04/04 4:32p Pkeet
 * Fixed heap bug by removing the static CPipelineHeap object and replacing it with references
 * to locally created objects.
 * 
 * 2     97-04-04 12:38 Speter
 * A big change: now pipeline uses CRenderPolygon rather than SRenderTriangle (currently always
 * as a triangle).  Changed associated variable names and comments.
 * 
 * 1     97/04/03 7:52p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "PipeLineHeap.hpp"
#include "Lib/Sys/DebugConsole.hpp"


//
// Constants.
//

//
// Maximum sizes of the renderer's dynamic arrays and heaps, per scene (not per object). 
// These are some arbitrary large values, because the fast heap allocater only commits 
// physical memory that is actually used.
// (However, it does reserve virtual memory from the maximum available on the swap file).
//

// Maximum number of polygons to render.
#define uMAX_NUM_POLYGONS	(20000)

// Maximum number of transformed points.
#define uMAX_NUM_POINTS		(uMAX_NUM_POLYGONS * 4)

//
// Class implementations.
//

//**********************************************************************************************
//
// CPipelineHeap Implementation.
//

	//******************************************************************************************
	//
	// Constructors.
	//

	//******************************************************************************************
	CPipelineHeap::CPipelineHeap()
	{
		uStartVertices        = darvVertices.uLen;
		uStartVertexPointers  = daprvVertices.uLen;
		uStartPolygons        = darpolyPolygons.uLen;
		uStartPolygonPointers = darppolyPolygons.uLen;

		uCommitVertices        = darvVertices.uCommit;
		uCommitVertexPointers  = daprvVertices.uCommit;
		uCommitPolygons        = darpolyPolygons.uCommit;
		uCommitPolygonPointers = darppolyPolygons.uCommit;
	}

	//******************************************************************************************
	CPipelineHeap::~CPipelineHeap()
	{
#if (0)
		dprintf("Committed Pipeline Heap = %dK+%dK+%dK+%dK\n",
						darvVertices.uCommit * sizeof(SRenderVertex)/1024, 
						daprvVertices.uCommit * sizeof(SRenderVertex*)/1024, 
						darpolyPolygons.uCommit * sizeof(CRenderPolygon)/1024, 
						darppolyPolygons.uCommit * sizeof(CRenderPolygon*)/1024);
#endif

		darvVertices.Reset(uStartVertices, false, -1);
		daprvVertices.Reset(uStartVertexPointers, false, -1);
		darpolyPolygons.Reset(uStartPolygons, false, -1);
		darppolyPolygons.Reset(uStartPolygonPointers, false, -1);
	}

	//******************************************************************************************
	void CPipelineHeap::Decommit()
	{
		// Decommit (and empty) the arrays.
		darvVertices.Reset(0, false, 0);
		daprvVertices.Reset(0, false, 0);
		darpolyPolygons.Reset(0, false, 0);
		darppolyPolygons.Reset(0, false, 0);
	}

//
// Module variable declarations.
//

// Static heaps.
CDArray<SRenderVertex>	 CPipelineHeap::darvVertices(uMAX_NUM_POINTS);
CDArray<SRenderVertex*>	 CPipelineHeap::daprvVertices(uMAX_NUM_POINTS);
CDArray<CRenderPolygon>  CPipelineHeap::darpolyPolygons(uMAX_NUM_POLYGONS);
CDArray<CRenderPolygon*> CPipelineHeap::darppolyPolygons(uMAX_NUM_POLYGONS);
