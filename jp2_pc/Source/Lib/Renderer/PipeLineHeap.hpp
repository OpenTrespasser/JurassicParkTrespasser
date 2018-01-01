/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/PipeLineHeap.hpp                                         $
 * 
 * 8     98.09.23 2:58p Mmouni
 * Added Decommit.
 * 
 * 7     98/09/14 1:44 Speter
 * Now save and restore CDArray commit sizes as well as lengths.
 * 
 * 6     97/11/14 17:55 Speter
 * Removed now-unused fhGeneric.
 * 
 * 5     97-04-09 16:16 Speter
 * Added vertex pointers to heap.  Added necessary includes.
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

#ifndef HEADER_LIB_RENDERER_PIPELINEHEAP_HPP
#define HEADER_LIB_RENDERER_PIPELINEHEAP_HPP

#include "Lib/Sys/FastHeap.hpp"
#include "ScreenRender.hpp"

//
// Class definitions.
//

//**********************************************************************************************
//
class CPipelineHeap
//
// Object manages pipeline memory allocation.
//
// Prefix: plh
//
//**************************************
{
private:

	uint	uStartVertices;
	uint	uStartVertexPointers;
	uint	uStartPolygons;
	uint	uStartPolygonPointers;

	uint	uCommitVertices;
	uint	uCommitVertexPointers;
	uint	uCommitPolygons;
	uint	uCommitPolygonPointers;

public:

	static CDArray<SRenderVertex>	darvVertices;
	static CDArray<SRenderVertex*>	daprvVertices;
	static CDArray<CRenderPolygon>	darpolyPolygons;
	static CDArray<CRenderPolygon*>	darppolyPolygons;

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	// Default constructor. Used to construct the static heaps.
	CPipelineHeap();

	// Constructor using already created heap objects.
	~CPipelineHeap();
	

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	static void Decommit
	(
	);
	//
	// Decommits all the memory used by the heap
	//
	//**************************************

	//******************************************************************************************
	//
	CPArray<SRenderVertex> parvVertices
	(
	)
	//
	// Returns an array of vertices associated with the current pipeline heap object.
	//
	//**************************************
	{
		return darvVertices.paSegment(uStartVertices);
	}

	//******************************************************************************************
	//
	CPArray<SRenderVertex*> paprvVertices
	(
	)
	//
	// Returns an array of vertex pointers associated with the current pipeline heap object.
	//
	//**************************************
	{
		return daprvVertices.paSegment(uStartVertexPointers);
	}

	//******************************************************************************************
	//
	CPArray<CRenderPolygon> parpolyPolygons
	(
	)
	//
	// Returns an array of triangles associated with the current pipeline heap object.
	//
	//**************************************
	{
		return darpolyPolygons.paSegment(uStartPolygons);
	}

	//******************************************************************************************
	//
	CPArray<CRenderPolygon*> parppolyPolygons
	(
	)
	//
	// Returns an array of polygon pointers associated with the current pipeline heap object.
	//
	//**************************************
	{
		return darppolyPolygons.paSegment(uStartPolygonPointers);
	}

};


#endif // HEADER_LIB_RENDERER_PIPELINEHEAP_HPP

