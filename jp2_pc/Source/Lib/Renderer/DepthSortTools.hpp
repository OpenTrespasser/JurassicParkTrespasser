/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Tools to assist in the debugging of the depth sort routine.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/DepthSortTools.hpp                                       $
 * 
 * 11    12/23/97 2:17p Pkeet
 * Added the 'fAdjustOut' parameter.
 * 
 * 10    97/07/23 18:05 Speter
 * Replaced CCamera argument to clipping functions with b_perspective flag.  
 * 
 * 9     97/06/10 15:48 Speter
 * Now takes camera as an argument (for new vertex projection).  Renamed call to
 * bCalculateScreenExtents.
 * 
 * 8     97/05/22 4:00p Pkeet
 * Added the 'iNumCulled' variable.
 * 
 * 7     97/05/08 1:00p Pkeet
 * Added extra stats. Removed the key less object.
 * 
 * 6     97/05/06 6:52p Pkeet
 * Added map classes. Added an extra stat.
 * 
 * 5     97/04/29 4:54p Pkeet
 * Moved the 'bSplit' function to the body of the code.
 * 
 * 4     97/04/29 2:17p Pkeet
 * Added the 'CPolyFarZ' class and the inline function 'bSplit.'
 * 
 * 3     97/04/28 5:44p Pkeet
 * Added externs for statistics counting.
 * 
 * 2     97/04/23 12:44p Pkeet
 * Added the 'WriteFloat' function.
 * 
 * 1     97/04/17 11:34a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDER3D_DEPTHSORTTOOLS_HPP
#define HEADER_LIB_RENDER3D_DEPTHSORTTOOLS_HPP


#include "ScreenRender.hpp"

//
// Forward declarations.
//
class CPipelineHeap;

//
// Global function prototypes.
//

//**********************************************************************************************
//
void WritePolygon
(
	const CRenderPolygon* prpoly	// Pointer to the render polygon to write the data for.
);
//
// Writes the polygon data to the debug output stream.
//
//**********************************

//**********************************************************************************************
//
void WriteText
(
	const char* str_out	// String to write.
);
//
// Writes the string to the debug output stream.
//
//**********************************

//**********************************************************************************************
//
void WriteFloat
(
	const char* str_out,
	float f
);
//
// Writes the string and the floating point value to the debug output stream. The string
// should not incorporate the float output format characters.
//
//**********************************

//**********************************************************************************************
//
void OpenDepthSortDebug
(
);
//
// Opens the depth sort debug stream.
//
//**********************************

//**********************************************************************************************
//
void CloseDepthSortDebug
(
);
//
// Closes the depth sort debug stream.
//
//**********************************


//
// Externally defined variables.
//
extern int iNumPolygonsStart;
extern int iNumPolygonsEnd;
extern int iNumTests;
extern int iNumStep1and2;
extern int iNumStep3;
extern int iNumStep4;
extern int iNumStep5;
extern int iNumReinserted;
extern int iNumClipped;
extern int iNumInitialize;
extern int iNumCulled;
extern int iNumA;
extern int iNumB;
extern int iNumC;
extern int iNumD;

// Outward adjustment for splitting terrain polygons and using hardware rasterizing.
extern float fAdjustOut;


//
// Functions required by the depthsorting object.
//

//**********************************************************************************************
//
bool bSplit
(
	CRenderPolygon*  prpoly_plane,	// Pointer to the polygon containing the plane to split by.
	CRenderPolygon*  prpoly_split,	// Pointer to the polygon to split.
	CRenderPolygon*& rprpoly_new_a,	// Pointer to the new polygon 'a.'
	CRenderPolygon*& rprpoly_new_b,	// Pointer to the new polygon 'b.'
	CPipelineHeap&   rplh,			// Current pipeline heap object.
	bool			 b_perspective	// Whether projecting camera is perspective.
);
//
// Splits a polygon by a plane. After a valid split, 'prpoly_split' is no longer valid.
//
// Returns 'true' if the polygon could be split by the plane.
//
//**********************************


//
// Internal class definitions.
//

//******************************************************************************************
//
class CPolyFarZ
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
	// Returns 'true' if the farthest (smallest) inverse Z value of the first polygon is
	// closer (larger) than the farthest inverse z value of the second polygon.
	//
	//**************************************
	{
		Assert(prpoly_0->fMaxZ >= 0.0f);
		Assert(prpoly_1->fMaxZ >= 0.0f);

		// Return the results of the comparision.
		return u4FromFloat(prpoly_0->fMaxZ) < u4FromFloat(prpoly_1->fMaxZ);
	}
};


//
// Compare object.
//


#endif // HEADER_LIB_RENDER3D_DEPTHSORTTOOLS_HPP
