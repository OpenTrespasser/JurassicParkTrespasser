/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Triangle drawing function. The function is a template based on a scanline type.
 *
 * Bugs:
 *
 * To do:
 *		Add an 'Assert' function to ensure anti-clockwise triangle coordinates are received.
 *		Experiment with tables instead of the compound 'if' statements in the constructor.
 *		If shared edges are rejected, pass only the array of edges to 'RenderFromLeft' and
 *		'RenderFromRight,' and lump functions only called by the constructor into the
 *		constructor. If shared edges are used, create a new constructor.
 *
 *		Change the vertex parameter interpolation to work with half-open ranges.  For example,
 *		when interpolating from a lighting value of 16.0 to 32.0, it should make the last pixel
 *		end up just below 32.0.  Then we won't have to scale the parameter by numbers like
 *		LASTRANGE - 0.01.
 *
 * Notes:
 *		The base edge of the triangle is the edge along which the triangle scanline routine
 *		starts. For a triangle with one left edge and two right edges, it is quicker to
 *		calculate increments for, and scan along, the left edge -- the left edge is therefore
 *		the base edge.
 *
 *		The algorithmic basis for the code implemented here and in reference modules can be
 *		found in "Graphics Gems Vol. III" under "Accurate Polygon Scan Conversion Using
 *		Half-Open Invervals" on page 362. The code is very dissimilar from the example code
 *		in that it uses a faster technique for edge-walking the triangles, and that it is
 *		objected-oriented and templated to work with various scanline types (e.g., flat,
 *		gouraud or linear texture-mapped).
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/DrawTriangle.hpp      $
 * 
 * 113   98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 112   98.09.10 4:05p Mmouni
 * Changed fMAX_NEG_AREA (-2*area where sub-triangle are dropped) to
 * -0.2.
 * 
 * 111   8/27/98 2:23p Rvande
 * Changed to constants to floats.
 * 
 * 110   8/25/98 3:07p Rvande
 * qualified fixed as being in the global scope
 * 
 * 109   98.08.19 7:40p Mmouni
 * Now any polygon that has degenerate sub-triangles will be rendered by
 * triangulation instead of using the trapezoid renderer.
 * 
 * 108   98.07.17 6:36p Mmouni
 * Added  new alpha texture primitive.
 * 
 * 107   98.06.28 4:53p Mmouni
 * Moved pixel coverage stat calculation to before
 * InitializePolygonsData to avoid MMX/FPU switching.
 * 
 * 106   98.06.17 6:46p Mmouni
 * Not counts pixels base on pre-calculated area.
 * 
 * 105   98.06.12 3:36p Mmouni
 * Added optimized versions of planar polygon gradient routines.
 * 
 * 104   98.06.04 8:13p Mmouni
 * Changed method used to compute the texture gradients across polygons
 * with planar values to prevent weird shifting.
 * 
 * 103   98.05.21 11:32p Mmouni
 * Added check for valid texture co-ordinates to polygon drawing
 * routine.
 * 
 * 102   98.03.24 8:17p Mmouni
 * Made changes to support alternate perspective correction settings.
 * 
 * 101   3/05/98 3:38p Pkeet
 * Added the 'bCAT_PIXEL_STAT' macro switch. Removed unused stats.
 * 
 * 100   2/26/98 2:54p Mmouni
 * Added call to done drawing polygon routine for use by the 3dx version
 * of DrawTriangle.
 * 
 * 99    98.01.22 3:14p Mmouni
 * Removed bump-map depth conditional compilation.
 * 
 * 98    1/19/98 7:34p Pkeet
 * Added support for 16 bit bumpmaps.
 * 
 * 97    1/15/98 7:03p Pkeet
 * Added the polygon lock stat.
 * 
 * 96    98.01.14 9:08p Mmouni
 * Made changes for K6 3D polygon setup optimizations.
 * 
 * 95    1/13/98 1:51p Pkeet
 * Replaced the auxilary renderer switch with a screen lock.
 * 
 * 94    1/12/98 11:45a Pkeet
 * Added in the software lock request for the auxilary renderer.
 * 
 * 93    97.11.25 7:02p Mmouni
 * Finished Pentium/Pentium Pro triangle setup optimization.
 * 
 * 92    97.11.14 11:58p Mmouni
 * Removed mis-use of erfFILTER flag.
 * Added specialized versions of InitializePolygonData for terrain
 * texture.
 * 
 * 91    97.11.11 9:50p Mmouni
 * Added iBits to SGlobalRas structure.
 * 
 * 90    97/11/08 3:41p Pkeet
 * Removed Z template parameters and code.
 * 
 * 89    97/10/30 7:05p Pkeet
 * Added a threshold for using a triangle slope for the trapezoids.
 * 
 * 88    97/10/28 13:23 Speter
 * Restored previous SetMinAbs() code, which is needed for proper linear
 * mapping with traps.
 * 
 * 87    97.10.27 1:26p Mmouni
 * Changed to make the pixel coverage stat work correctly.
 * 
 * 86    97.10.15 7:39p Mmouni
 * Added support for inline assembly for parts of CDrawPolygon.
 * 
 * 85    97/10/12 20:34 Speter
 * Removed fClutRampScale, as cvIntensity is now pre-scaled.
 * 
 * 84    10/10/97 1:43p Mmouni
 * All rendering is now left to right.
 * 
 * 83    8/19/97 5:21p Bbell
 * Increased the 'iMAX_RASTER_VERTICES' const.
 * 
 * 82    97/08/07 11:35a Pkeet
 * Added interface support for mipmapping.
 * 
 * 81    97/08/05 11:33 Speter
 * Added psDrawPolygonBegin stat.
 * 
 * 80    97/07/28 11:16a Pkeet
 * Uses the 'erfDRAW_CLIP' to set the constant colour to 0 after polygon
 * data has been set.
 * 
 * 79    97/07/23 18:00 Speter
 * Added psDrawPolygonSolid stat.
 * 
 * 78    97/07/18 3:29p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 77    97/07/16 15:59 Speter
 * Fixed some bugs in trapezoid renderer, and now use trapezoids for
 * non-planar polygons as well.  Added InitGradientData() to calculate
 * gradients for trapezoids.  InitializeTriangleData now takes two
 * params to use as coefficients in derivative calculation rather than
 * f_invdx; also takes b_update parameter to use for multiple-triangle
 * gradient calculations.   Added AssertXRange function to test the
 * validity of a single scanline. Added iNumRasterVertices global.
 * Added additional DrawPolygon sub-stats.  Removed unneeded fInvDX
 * member variable.  
 * 
 * 76    97/07/07 14:02 Speter
 * Now copy vertices for current polygon locally to arvRasterVertices,
 * which is used rather than rpoly.paprvPolyVertices for most
 * DrawPolygon code.  Renamed iY to iYScr, as it's now a union with
 * v3Cam, and set in InitializePolygonData.
 * 
 * 75    97/06/27 15:22 Speter
 * Now renders trapezoids when able to.  Added DrawTrapezoids,
 * InitTriangleData, InitializeTopVertex.  Re-ordered code to allow for
 * multiple base-edges per polygon.  Call new
 * LINE::InitializePolygonData function.  Moved calls to
 * SetNumIntensities() and fClutRampScale to Gouraud templates.  Updated
 * primitive stats (disabled) for trapezoids.  Removed
 * CalculateTrapStats().
 * 
 * 74    97/06/25 1:14p Pkeet
 * Includes code for displaying depth sort clipping.
 * 
 * 73    97/06/24 4:22p Pkeet
 * Made array of edges static to eliminate unnecessary constructor
 * calls.
 * 
 * 72    6/19/97 2:53p Mlange
 * Moved Profile.hpp to Lib/Sys.
 * 
 * 71    97/06/17 2:03p Pkeet
 * Removed the reference counting pointer from prasScreen.
 * 
 * 70    97/06/15 17:27 Speter
 * Put trapezoid stat code inside bPRIM_STATS compile flag.
 * 
 * 69    97/06/14 6:46p Pkeet
 * Fixed compile bug.
 * 
 * 68    6/15/97 1:43a Bbell
 * Fixed final mode build problem.
 * 
 * 67    6/15/97 1:31a Bbell
 * Removed unnecessary include.
 * 
 * 66    97/06/13 7:31p Pkeet
 * Added CDrawPolygonBase and a global raster structure.
 * 
 * 65    97/06/12 15:29 Speter
 * CalculateTrapStats is now iffed out.
 * 
 * 64    97/06/10 15:39 Speter
 * Removed iY calculation from CDrawPolygon.  Now done in pipeline.
 * 
 * 63    97/06/06 17:15 Speter
 * Fixes for Final mode (timing stats).
 * 
 * 62    97/06/05 17:02 Speter
 * Updated for stats.
 * 
 * 61    97/06/04 6:45p Pkeet
 * Moved the profile stats here.
 * 
 * 60    97/06/03 18:50 Speter
 * Added #include FixedP.hpp.  Moved polygon stats here from
 * Profile.cpp, added perspective stats.
 * 
 * 59    97/06/02 13:52 Speter
 * Added primitive element stats for both triangles and trapezoids.
 * 
 * 58    97/05/26 2:19p Pkeet
 * Optimal calculation of the integer Y position.
 * 
 * 57    97/05/25 18:04 Speter
 * Changed CDrawTriangle to CDrawPolygon, which calls member
 * DrawTriangle.  Added faster triangulisation.
 * 
 * 56    97/05/23 17:52 Speter
 * Added CDrawPolygon<> template, which should be used by all rather
 * than CDrawTriangle<>.  For now CDrawPolygon<> invokes
 * CDrawTriangle<>.  The latter now takes vertex and texture arguments
 * directly, rather than via SRenderTriangle.  Removed
 * CRenderPolygon::fScreenArea param, now calculate fInvDx directly.
 * 
 * 55    5/06/97 5:07p Cwalla
 * Stoped fixed point overloading.
 * 
 * 54    97-03-31 22:19 Speter
 * Adjust fInvDx calculation for actual screen area.
 * Now use CEdge<> line variables rather than slower pointers.
 * Moved timing calls to DrawSubtriangle.hpp.
 * 
 * 53    97/02/20 7:19p Pkeet
 * Removed the vertical scanline loop from the 'RenderFromLeft' and
 * 'RenderFromRight' member functions in favour of a function call to a
 * DrawSubtriangle function. Added the necessary includes and changes to
 * use the new DrawSubtriangle function. Removed the '++' member
 * function as the code has been moved to the DrawSubtriangle function.
 * 
 * 52    97/02/19 11:08 Speter
 * Corrected sign of pixel stat.
 * 
 * 51    97/02/07 7:44p Pkeet
 * Removed the 'bValidateHandedness' and 'bSetParialDerivativeRelX' functions in favour of using
 * the pre-calculated area of the triangle.
 * 
 * 50    97/01/26 19:54 Speter
 * Changed valid handedness test to not assert.
 * Moved scanline timing calls from outside scanline call to outside of sub-triangle loop.
 * 
 * 49    97/01/16 11:54 Speter
 * Updated for CProfile changes.
 * 
 * 48    1/15/97 1:03p Pkeet
 * Removed fog template parameters.
 * 
 * 47    1/08/97 7:42p Pkeet
 * Added an include for CEdge. Removed extraneous scope information from the CDrawTriangle
 * constructor.
 * 
 * 46    97/01/07 12:04 Speter
 * Updated for rptr_const.
 * 
 * 45    96/12/31 16:58 Speter
 * Updated for rptr.
 * 
 * 44    96/12/17 19:23 Speter
 * Removed psDrawTriangle stat (subsumed in psDrawTriangles).
 * 
 * 43    96/12/17 13:19 Speter
 * Moved extern fClutRampScale declaration here from DrawTriangle.hpp.
 * Moved includes to bottom of file for now, since they contain code.
 * 
 * 
 * 42    96/12/09 16:18 Speter
 * Updated for new CProfile members.
 * 
 * 41    96/12/06 17:58 Speter
 * Changed profile to use new CCycleTimer class and Add() function.
 * 
 * 40    12/06/96 5:14p Pkeet
 * Commented out seperate left and right drawing routines.
 * 
 * 39    12/06/96 3:51p Pkeet
 * Added the bModified global flag.
 * 
 * 38    96/12/05 16:48 Speter
 * Removed #ifdef on profile calls.
 * 
 * 37    12/05/96 1:17p Pkeet
 * Added profiling from the Profile.hpp module.
 * 
 * 36    11/12/96 2:46p Pkeet
 * Added support for initializing self-modifying code on a per-triangle basis.
 * 
 * 35    10/11/96 10:31a Pkeet
 * Added 'pvClutConversion' as a global variable instead of a member of 'CDrawTriangle.' No clut
 * pointer is therefore passed to the 'DrawLoop' functions.
 * 
 * 34    10/10/96 8:20p Pkeet
 * Moved extern out of this module.
 * 
 * 33    10/10/96 7:58p Pkeet
 * Made statically declared variables externally declared variables.
 * 
 * 32    10/09/96 7:44p Pkeet
 * Changed the 'LineFlatZ.hpp' include to the 'Scanline.hpp' include. Changed references from
 * 'CLineFlatZ' to 'CScanline.' Removed the 'pixPixel' and 'ptexTexture' global variables. Moved
 * the 'iDefaultFog' global to the fog template module. Removed the 'fSCALE_ADJUST' const. Added
 * the 'pdpixClut' and 'ptexTexture' member variables.
 * 
 * 31    96/10/08 19:26 Speter
 * Added stats for scanline timing and number of pixels drawn.
 * 
 * 30    96/10/04 18:39 Speter
 * Added necessary include.
 * 
 * 29    96/10/04 18:02 Speter
 * Removed defunct ObjDef2D.hpp include.  Added RenderDefs.hpp.
 * 
 * 28    10/04/96 3:51p Pkeet
 * Added the CZBuffer class as a template parameter to CLineFlatZ.
 * 
 * 27    10/03/96 6:17p Pkeet
 * Removed pvClutTable in favour of a member variable of the triangle.
 * 
 * 26    10/03/96 3:14p Pkeet
 * Removed constants and global variables associated with Gouraud shading. Removed extraneous
 * includes. Added includes removed from 'ScreenRenderDWI.cpp.'
 * 
 * 25    9/27/96 4:32p Pkeet
 * Removed the 'LineGouraudZ' template class.
 * 
 * 24    9/24/96 2:53p Pkeet
 * Calculates a 'fClutRampScale' value from the number of ramp values in a clut only when a clut
 * is present, otherwise uses the default value. The test for a valid Z-buffer present was moved
 * from this primitive to the 'ScreenRenderDWI' module.
 * 
 * 23    8/21/96 4:44p Pkeet
 * Moved 'pixPixel.'
 * 
 * 22    8/20/96 4:47p Pkeet
 * Added the 'iDefaultFog' variable.
 * 
 * 21    96/08/09 11:05 Speter
 * Changed zbuffer from CRasterMemT<int32> to simple CRaster, because with D3D, z buffer might
 * be different depth.
 * 
 * 20    7/31/96 12:38p Pkeet
 * Added 'fGouraudTolerance' constant.
 * 
 * 19    7/29/96 2:15p Pkeet
 * Made the 'SRenderCoord' struct non-const.
 * 
 * 18    96/07/26 18:22 Speter
 * Changed scaling of vertex parameters again.  Now all increments are scaled by parameter size,
 * actual values are scaled by parameter size minus one, and 0.5 is added.
 * 
 * 17    96/07/26 10:53 Speter
 * Added fSCALE_ADJUST parameter for vertex parameter scaling adjustment.  A value of 1.0 for
 * this prevents crashes, but we must still determine the reallly correct way of scaling.
 * 
 * 16    96/07/24 14:54 Speter
 * Added fClutRampScale global variable, and some comments.
 * 
 * 15    96/07/22 15:24 Speter
 * Moved pixSolid global variable to LineFlatZ.hpp.
 * 
 * 14    7/18/96 5:43p Pkeet
 * Changed iY asserts to match height range.
 * 
 * 13    96/07/18 17:18 Speter
 * Changes affecting zillions of files:
 * Changed SRenderCoord to SRenderVertex.
 * Added SRenderTriangle type, changed drawing primitives to accept it.
 * 
 * 12    7/17/96 12:06p Pkeet
 * Moved the drawing primitives up by 1 so that they clip correctly.
 * 
 * 11    96/07/16 13:14 Speter
 * Changed iPixelSolid to pixSolid.
 * 
 * 10    96/07/15 18:58 Speter
 * Changed u4_data param to CTexture*.
 * Added static globals: ptexTexture, pvClutTable, pixPixel.
 * 
 * 9     7/09/96 3:22p Pkeet
 * Moved 'i4DInvZ' to LineFlatZ.hpp.
 * 
 * 8     7/08/96 3:28p Pkeet
 * Moved 'i4DInvZ.'
 * 
 * 7     6/25/96 2:31p Pkeet
 * Made 'u4Data' a global variable instead of a member variable of 'CDrawTriangle.'
 * 
 * 6     6/17/96 6:42p Pkeet
 * Added 'iFastPosFloatToInt' use.
 * 
 * 5     6/13/96 8:31p Pkeet
 * Eliminated fast float to int conversions for now.
 * 
 * 4     6/13/96 3:23p Pkeet
 * Added comments.
 * 
 * 3     6/13/96 3:08p Pkeet
 * Changed 'CDrawTriangle' prefix. Added to the 'to do' list. Added asserts. Added references to
 * texts.
 * 
 * 2     6/06/96 7:57p Pkeet
 * Changed calls to 'DrawFromLeft' and 'DrawFromRight' to reflect their new status as template
 * functions as opposed to member functions.
 * 
 * 1     6/06/96 5:38p Pkeet
 * Initial implementation. 'CDrawTriangle' now combines aspects of the old 'CPoly' object with
 * the draw functions added as member functions.
 * 
 **********************************************************************************************/

#ifndef HEADER_RENDERER_PRIMITIVES_DRAWTRIANGLE_HPP
#define HEADER_RENDERER_PRIMITIVES_DRAWTRIANGLE_HPP


//
// Necessary includes for triangle drawing.
//
#include "Lib/Sys/Profile.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Types/FixedP.hpp"
#include "Edge.hpp"


#define bPRIM_STATS				(0)
#define bPOLY_STATS				(0)
#define bPIXEL_COVERAGE_STAT	(1)
#define bCAT_PIXEL_STAT         (0)
#define bLOCK_STAT              (1)


//
// Global constants  & variables.
//
const int iMAX_RASTER_VERTICES = 64;
const float fMAX_NEG_AREA = -0.2;

extern SRenderVertex arvRasterVertices[iMAX_RASTER_VERTICES];
extern int iNumRasterVertices;
extern bool  bModified;
extern int   iDefaultFog;


// Stats, relative to proProfile.psDrawPolygon.
extern CProfileStat			psDrawPolygonInit;
extern CProfileStat			psDrawPolygonBump;

#if bLOCK_STAT

extern CProfileStat         psDrawPolygonLock;

#endif

#if bPOLY_STATS

extern CProfileStat			psDrawPolygonBegin;
extern CProfileStat			psDrawPolygonData;
extern CProfileStat			psDrawPolygonEdges;
extern CProfileStat			psDrawPolygonWalk;

#endif

extern CProfileStatParent	psPixels;
#if bCAT_PIXEL_STAT
extern CProfileStat			psPixelsPerspective;
extern CProfileStat			psPixelsC;
#endif

#if bPRIM_STATS

extern CProfileStat psSubpolygons;
extern CProfileStat psTrapezoids;
extern CProfileStat psBaseEdges;
extern CProfileStat psEdges;
extern CProfileStat psLines;

#endif


//
// Forward declarations of classes and functions.
//

//*********************************************************************************************
template <class TPIX, class GOUR, class TRANS, class MAP, class INDEX, class CLU>
class CScanline;

//*********************************************************************************************
template<class LINE> class CDrawPolygon;

//*****************************************************************************************
template<class LINE> void DrawSubtriangle(LINE* pscan, CDrawPolygon<LINE>* pdtriTriangle);


//
// Global templated helper functions used in CDrawPolygon.
//
template <class LINE> void InitializePolygonData(LINE& lineData, CRenderPolygon* rpoly);

template <class LINE> void InitializeEdge(CEdge<LINE>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);

template <class LINE> bool bInitTriangleDataEx(CDrawPolygon<LINE>* poly, SRenderVertex* prv_a, 
									SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);

template <class LINE> bool bInitGradientDataPlanarEx(CDrawPolygon<LINE>* poly);

template <class LINE> void InitializeForWalkEx(CDrawPolygon<LINE>* poly, CEdge<LINE>* pedge);

template <class LINE> void DoneDrawingPolygon(CDrawPolygon<LINE>* poly);

//
// Global functions.
//

//*********************************************************************************************
inline int iPrev
(
	int i
)
{
	return i > 0 ? i-1 : iNumRasterVertices - 1;
}

//*********************************************************************************************
inline int iNext
(
	int i
)
{
	i++;
	return i < iNumRasterVertices ? i : 0;
}

//*********************************************************************************************
//
template<class T> inline void SetMinAbs
(
	T& t_a,								// Variable to set.
	T t_b								// Value to compare with.
)
//
// Set t_a to t_b if the absolute value of t_b is less than t_a.
//
//**************************************
{
//	if (Abs(t_b) < Abs(t_a))
//		t_a = t_b;
	if (t_a >= 0)
	{
		if (t_b < 0)
			t_a = 0;
		else
			SetMin(t_a, t_b);
	}
	else
	{
		if (t_b > 0)
			t_a = 0;
		else
			SetMax(t_a, t_b);
	}
}

//*********************************************************************************************
//
template<class T> inline void SetMaxAbs
(
	T& t_a,								// Variable to set.
	T t_b								// Value to compare with.
)
//
// Set t_a to t_b if the absolute value of t_b is greater than t_a.
//
//**************************************
{
	if (Abs(t_b) > Abs(t_a))
		t_a = t_b;
}

//
// Structure Definitions.
//

//*********************************************************************************************
//
struct SGlobalRas
//
// A global raster description for use by subtriangle asm blocks.
//
// Prefix: gs
//
//**************************************
{
	void*  pvScreen;			// Screen buffer.
	uint32 u4LinePixels;		// Screen buffer stride.
	int iBits;					// Bits per pixel (15 or 16).
};

// Global structure.
extern SGlobalRas gsGlobals;


//
// Class definitions.
//

//*********************************************************************************************
//
class CDrawPolygonBase
//
// Describes a polygon rasterizing structure that is independant of template types.
//
// Prefix: polybase
//
//**************************************
{
public:
	// Force default constructor inline so it is not called.
	forceinline CDrawPolygonBase()
	{
	}

	int   iY;					// Current scanline being rasterized.
	int   iYTo;					// Last scanline to be rasterized along the current
								// pair of left and right edges.
	int   iLineStartIndex;		// Index to the first pixel in the current horizontal line.
	::fixed fxLineLength;		// Length of the current scanline.
	::fixed fxDeltaLineLength;	// Increment of the length of the scanline for the current
								// pair of left and right edges.
};


//*********************************************************************************************
//
template<class LINE> class CDrawPolygon : public CDrawPolygonBase
//
// Describes triangle rasterizing data based on a scanline type. Sets up the basic information
// of the plane, and performs plane data lineIncrements per scanline.
//
// Prefix: tri
//
//**************************************
{

public:
	CRaster*		prasScreen;			// Pointer to the main screen raster.
	ptr_const<CTexture>	ptexTexture;	// Pointer to the source texture.
	LINE            lineData;			// Storage for triangle-wide data based on the
										// scanline type.
	CEdge<LINE>*    pedgeBase;			// Base edge.
	CRenderPolygon* prpolyPoly;			// Render polygon.

public:

	//*****************************************************************************************
	//
	// CDrawPolygon constructors.
	//

	//*****************************************************************************************
	//
	CDrawPolygon
	(
		CRaster*			pras_screen,	// Address of the screen buffer.
		CRenderPolygon&		rpoly			// Info on polygon to render.
	)
	//
	// Draws a polygon by splitting into triangles and calling DrawTriangle.
	//
	// Notes:
	//		This class assumes that:
	//
	//			* The polygon coordinates are provided in an anti-clockwise direction.
	//
	//			* 0.5 has been added to the x and y values of the screen coordinates to cause
	//			  pixels to be lit according to their centres as opposed to their top left
	//			  corners.
	//
	//**************************************
	{
		CCycleTimer ctmr;

		#if (VER_DEBUG)
			rpoly.Validate();
		#endif

		//
		// Copy data to CDrawPolygon member variables.
		//
		bModified = false;
		prpolyPoly = &rpoly;

		Assert(pras_screen);

		prasScreen	= pras_screen;
		ptexTexture	= rpoly.ptexTexture;

#if bPIXEL_COVERAGE_STAT
		// Add timing information.  Area = -0.5 * dx.
		int iArea = iRound(rpoly.fArea);
		psPixels.Add(0, iArea);
#if bCAT_PIXEL_STAT
		if (LINE::TIndex::bIsPerspective())
			psPixelsPerspective.Add(0, iArea);
#endif
#endif

		InitializePolygonData(lineData, &rpoly);

		//
		// Test to see if the polygon is supposed to render black only. If it is, changed
		// the flat colour to black.
		//
		if (rpoly.seterfFace[erfDRAW_CLIP])
		{
			u4ConstColour = 0;
		}

		iNumRasterVertices = rpoly.paprvPolyVertices.uLen;

#if bPOLY_STATS
		psDrawPolygonBegin.Add(ctmr(), 1);
#endif

		if (iNumRasterVertices <= 3)
		{

			// Render the triangle.
			DrawTriangle
			(
				&arvRasterVertices[0],
				&arvRasterVertices[1],
				&arvRasterVertices[2]
			);
		}
		else
		{
			//
			// Subdivide the polygon.
			//

			// Check menu setting before enabling trapezoidalisation.
			if (rpoly.seterfFace[erfTRAPEZOIDS])
			{
				DrawTrapezoids();
			}
			else
			{
				DrawTriangulated();
			}
		}

		DoneDrawingPolygon(this);
	}

	//*****************************************************************************************
	//
	bool bInitTriangleData
	(
		SRenderVertex*			prv_a,				// 3 vertices making up this triangle.	
		SRenderVertex*			prv_b,
		SRenderVertex*			prv_c,
		bool					b_update = false
	)
	//
	// Sets up triangle-wide rasterising info.
	//
	// Returns:
	//		Whether the triangle is large enough to render.
	//
	//**************************************
	{
		Assert(prv_a);
		Assert(prv_b);
		Assert(prv_c);

		//
		// Calculate fInvDx, the partial derivative of x over the triangle, using the following
		// formula:
		//
		//		           1                       1
		//		fInvDx = ---- = ---------------------------------------
		//	              dx    (x2 - x1)(y3 - y1) - (x3 - x1)(y2 - y1)
		//
		// dx also happens to be -2 times the area of the triangle, so we can check for correct
		// orientation here as well.
		//

		float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);

		float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
					  (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;

		if (f_dx >= fMAX_NEG_AREA)
			return false;

		// Calculate coefficients to pass to template InitializeTriangleData functions.
		float f_invdx = 1.0f / f_dx;
		float f_yab_invdx = f_yab * f_invdx;
		float f_yac_invdx = f_yac * f_invdx;

		// Calculate triangle data for the line type.
		lineData.InitializeTriangleData(prv_a, prv_b, prv_c, f_yab_invdx, f_yac_invdx, ptexTexture, b_update, prpolyPoly->seterfFace[erfSOURCE_TERRAIN]);

		return true;
	}

	//*****************************************************************************************
	//
	bool bInitGradientDataPlanar
	(
	)
	//
	// Sets up polygon-wide rasterising info.
	//
	// Returns:
	//		Whether the polygon is large enough to render.
	//
	//**************************************
	{
		SRenderVertex *prv_a = &arvRasterVertices[iNumRasterVertices-2];
		SRenderVertex *prv_b = &arvRasterVertices[iNumRasterVertices-1]; 
		SRenderVertex *prv_c = &arvRasterVertices[0];
		SRenderVertex *prv_a_min = prv_a;
		SRenderVertex *prv_b_min = prv_b; 
		SRenderVertex *prv_c_min = prv_c;

		//
		// Find the first edge pair with an area greater than 25 pixels, or if no 
		// edge pair has an area greater than 25 pixels find the maximum.
		//
		float f_yab_min = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		float f_yac_min = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);

		float f_dx_min  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac_min -
						  (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab_min;

		for (int i = 1; i < iNumRasterVertices && f_dx_min > -50.0f; i++)
		{
			prv_a = prv_b;
			prv_b = prv_c;
			prv_c = &arvRasterVertices[i];

			float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
			float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);

			float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
						  (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;

			if (f_dx < f_dx_min)
			{
				f_yab_min = f_yab;
				f_yac_min = f_yac;
				f_dx_min = f_dx;

				prv_a_min = prv_a;
				prv_b_min = prv_b;
				prv_c_min = prv_c;
			}
		}

		// Is the polygon too small to draw?
		if (f_dx_min >= fMAX_NEG_AREA)
			return false;

#if bPIXEL_COVERAGE_STAT
		// Add timing information.  Area = -0.5 * dx.
		psPixels.Add(0, iRound(f_dx_min * -0.5f));
#if bCAT_PIXEL_STAT
		if (LINE::TIndex::bIsPerspective())
			psPixelsPerspective.Add(0, iRound(f_dx_min * -0.5f));
#endif
#endif

		// Calculate coefficients to pass to template InitializeTriangleData functions.
		float f_invdx = 1.0f / f_dx_min;
		float f_yab_invdx = f_yab_min * f_invdx;
		float f_yac_invdx = f_yac_min * f_invdx;

		// Calculate triangle data for the line type.
		lineData.InitializeTriangleData(prv_a, prv_b, prv_c, f_yab_invdx, f_yac_invdx, ptexTexture, 0, prpolyPoly->seterfFace[erfSOURCE_TERRAIN]);

		return true;
	}

protected:

	//*****************************************************************************************
	//
	// CDrawPolygon member functions.
	//

	//*****************************************************************************************
	//
	void DrawTriangle
	(
		SRenderVertex*			prv_a,			// 3 vertices making up this triangle.	
		SRenderVertex*			prv_b,
		SRenderVertex*			prv_c
	)
	//
	// Draws a triangle by building edges and scanning through those edges either left-to-right
	// or right-to-left. The direction of the scanning depends on which way the triangle points
	// along the horizontal screen axis.
	//
	//**************************************
	{
		CCycleTimer	ctmr;
		int dir;

#if bPRIM_STATS
		psSubpolygons.Add(0, 1);
#endif

		//
		// Determine where to start walking the edges of the triangle, and the base edge of the
		// triangle. There are only six basic patterns for walking the edges of an
		// anti-clockwise triangle. Where 'a', 'b' and 'c' represent the vertices of a triangle,
		// 'ab', 'bc' and 'ca' represent the edges of a triangle and where 'L' stands for left
		// and 'R' stands for right, the decision tree for walking the triangle this is:
		//
		//                              ab is
		//                                /\
		//                             L /  \ R
		//                              /    \
		//              ----------------      ----------------
		//             |                                      |
		//           bc is                                  bc is
		//             /\                                     /\
		//          L /  \ R                               L /  \ R
		//           /    \                                 /    \
		//   -------       ---                           ---       -------
		//  | L | R |         |                         |        | L | R |
		//  | ab| ca|       ca is                     ca is      | ca| bc|
		//  | bc|   |         /\                        /\       |   | ab|
		//   -------       L /  \ R                  L /  \ R     -------
		//                  /    \                    /    \     
		//          -------        -------    -------        -------
		//         | L | R |      | L | R |  | L | R |      | L | R |
		//         | ca| bc|      | ab| ca|  | bc| ab|      | bc| ab|
		//         | ab|   |      |   | bc|  | ca|   |      |   | ca|
		//          -------        -------    -------        -------
		//
		// The order of edge walking will be set to this tree, and the i4ScanlineDirection
		// variable will be set to 1 if the triangle is rasterized from the left to right,
		// otherwise it will be set to -1. If an edge doesn't cross a horizontal scanline,
		// it is assumed to be a right edge.
		//

		CEdge<LINE> pedge[3];	// Edge list.

		// If ab is left:
		if (prv_a->iYScr < prv_b->iYScr)
		{
			// If bc is left:
			if (prv_b->iYScr < prv_c->iYScr)
			{
				dir = -1;
				InitializeEdge(pedge[0], prv_a, prv_b);
				InitializeEdge(pedge[1], prv_b, prv_c);
				InitializeEdge(pedge[2], prv_a, prv_c);
			}
			else
			{
				// If ca is left:
				if (prv_c->iYScr < prv_a->iYScr)
				{
					dir = -1;
					InitializeEdge(pedge[0], prv_c, prv_a);
					InitializeEdge(pedge[1], prv_a, prv_b);
					InitializeEdge(pedge[2], prv_c, prv_b);
				}
				else
				{
					dir = 1;
					InitializeEdge(pedge[0], prv_a, prv_b);
					InitializeEdge(pedge[1], prv_a, prv_c);
					InitializeEdge(pedge[2], prv_c, prv_b);
				}
			}
		}
		else
		{
			// If bc is left:
			if (prv_b->iYScr < prv_c->iYScr)
			{
				// If ca is left:
				if (prv_c->iYScr < prv_a->iYScr)
				{
					dir = -1;
					InitializeEdge(pedge[0], prv_b, prv_c);
					InitializeEdge(pedge[1], prv_c, prv_a);
					InitializeEdge(pedge[2], prv_b, prv_a);
				}
				else
				{
					dir = 1;
					InitializeEdge(pedge[0], prv_b, prv_c);
					InitializeEdge(pedge[1], prv_b, prv_a);
					InitializeEdge(pedge[2], prv_a, prv_c);
				}
			}
			else
			{
				// bc is right.
				dir = 1;
				InitializeEdge(pedge[0], prv_c, prv_a);
				InitializeEdge(pedge[1], prv_c, prv_b);
				InitializeEdge(pedge[2], prv_b, prv_a);
			}
		}

		// Always render left-to-right.
		i4ScanlineDirection = 1;

#if bPOLY_STATS
		psDrawPolygonEdges.Add(ctmr(), 1);
#endif

		if (!bInitTriangleDataEx(this, prv_a, prv_b, prv_c, false))
			return;

#if bPOLY_STATS
		psDrawPolygonData.Add(ctmr(), 1);
#endif

#if bPRIM_STATS
		psEdges.Add(0, 3);
#endif

		//
		// Walk along left or right edges depending on which is faster.
		//
		if (dir > 0)
		{
			RenderFromLeft(&pedge[0], &pedge[1], &pedge[2]);
		}
		else
		{
			RenderFromRight(&pedge[0], &pedge[1], &pedge[2]);
		}
	}

	//*****************************************************************************************
	//
	void DrawTriangulated()
	//
	// Draws a polygon by subdividing into triangles and then dividing the triangles into 
	// sub-trapezoids.
	//
	//**************************************
	{
		// Find top point.
		int i_ymin = iTopIndex();

		// Render the top triangle.
		int i_a = iPrev(i_ymin);
		int i_b = i_ymin;
		int i_c = iNext(i_ymin);

		//
		// From here, proceed to triangulate downwards.
		//

		int i_aa = iPrev(i_a);
		int i_cc = iNext(i_c);

		for (;;)
		{
			DrawTriangle
			(
				&arvRasterVertices[i_a],
				&arvRasterVertices[i_b],
				&arvRasterVertices[i_c]
			);

			if (i_aa == i_cc)
			{
				// Bottom triangle.
				DrawTriangle
				(
					&arvRasterVertices[i_a],
					&arvRasterVertices[i_c],
					&arvRasterVertices[i_cc]
				);
				break;
			}

			// Advance to next lowest vertex.
			if (arvRasterVertices[i_aa].iYScr < arvRasterVertices[i_cc].iYScr)
			{
				i_b = i_a;
				i_a = i_aa;
				i_aa = iPrev(i_a);
			}
			else
			{
				i_b = i_c;
				i_c = i_cc;
				i_cc = iNext(i_c);
			}
		}
	}

	//*****************************************************************************************
	//
	bool bInitGradientData
	(
		int		i_left1,				// Top vertices of polygon.
		int		i_right1,
		bool&	b_bad_subtriangles		// Set to true if any sub-triangles are too small to render.
	)
	//
	//**************************************
	{
		//
		// Iteratate all the trapezoids, finding optimal gradients.
		//
		bool b_update;
		bool b_good;

		// Find next coordinates.
		int i_left2 = iNext(i_left1);
		int i_right2 = iPrev(i_right1);

		// Initialise gradients based on top trapezoid.
		b_good = bInitTriangleDataEx
		(
			this,
			&arvRasterVertices[i_left1], 
			&arvRasterVertices[i_left2], 
			&arvRasterVertices[i_right2],
			false
		);

		b_update = b_good;
		b_bad_subtriangles = !b_good;

		if (i_left1 != i_right1 && i_left2 != i_right2)
		{
			// The top is a trapezoid; update from bottom right triangle.
			b_good = bInitTriangleDataEx
			(
				this,
				&arvRasterVertices[i_left1], 
				&arvRasterVertices[i_right2],
				&arvRasterVertices[i_right1], 
				b_update
			);

			b_update |= b_good;
			b_bad_subtriangles |= !b_good;
		}

		// Iterate through remaining trapezoids.
		for (;;)
		{
			if (i_left2 == i_right2)
				// We just did bottom triangle.
				break;

			int i_diff = arvRasterVertices[i_left2].iYScr - arvRasterVertices[i_right2].iYScr;

			if (i_diff <= 0)
			{
				// New left vertex and edge.
				i_left1 = i_left2;
				i_left2 = iNext(i_left2);

				if (i_diff == 0 && i_left2 == i_right2)
					// We just hit bottom trap.
					break;

				// Update for new left.
				if (i_left2 != i_right2)
				{
					b_good = bInitTriangleDataEx
					(
						this,
						&arvRasterVertices[i_left2], 
						&arvRasterVertices[i_right2],
						&arvRasterVertices[i_left1], 
						b_update
					);

					b_update |= b_good;
					b_bad_subtriangles |= !b_good;
				}
			}

			if (i_diff >= 0)
			{
				// New right vertex and edge.
				i_right1 = i_right2;
				i_right2 = iPrev(i_right2);

				// Update for new right.
				if (i_left2 != i_right2)
				{
					b_good = bInitTriangleDataEx
					(
						this,
						&arvRasterVertices[i_left2], 
						&arvRasterVertices[i_right2],
						&arvRasterVertices[i_right1], 
						b_update
					);

					b_update |= b_good;
					b_bad_subtriangles |= !b_good;
				}
			}
		}

		return b_update;
	}

	//*****************************************************************************************
	//
	void DrawTrapezoids()
	//
	// Draws a polygon by subdividing into trapezoids, and building edges as necessary.
	// Each trapezoid is always rendered left-to-right.
	//
	//**************************************
	{
		CCycleTimer	ctmr;

#if bPRIM_STATS
		psSubpolygons.Add(0, 1);
#endif

		// Find top trapezoid.
		int i_right1 = iTopIndex();

		// May not be unique top point; check adjacent points for same scanline.
		int i_left1 = iNext(i_right1);
		if (arvRasterVertices[i_left1].iYScr > arvRasterVertices[i_right1].iYScr)
		{
			i_left1 = i_right1;

			// Try previous vertex.
			i_right1 = iPrev(i_left1);
			if (arvRasterVertices[i_right1].iYScr > arvRasterVertices[i_left1].iYScr)
				// Pointy top.
				i_right1 = i_left1;
		}

		if (LINE::bIsPlanar())
		{
			// Find gradients.
			if (!bInitGradientDataPlanarEx(this))
				return;
		}
		else 
		{
			// Find gradients.
			bool b_bad_subtriangles = false;
			if (!bInitGradientData(i_left1, i_right1, b_bad_subtriangles))
				return;

			// If one or more sub-triangles were bad, don't use the trapezoid renderer.
			if (b_bad_subtriangles)
			{
				DrawTriangulated();
				return;
			}
		}

#if bPOLY_STATS
		psDrawPolygonData.Add(ctmr(), 1);
#endif

		// Always render left-to-right.
		i4ScanlineDirection = 1;

		// The two edge structures re-used throughout this polygon.
		CEdge<LINE> edge_left, edge_right;
		
		int i_left2 = iNext(i_left1);
		int i_right2 = iPrev(i_right1);

		// Initialise the first two edges.
		InitializeEdge(edge_left, &arvRasterVertices[i_left1], &arvRasterVertices[i_left2]);
		InitializeEdge(edge_right, &arvRasterVertices[i_right1], &arvRasterVertices[i_right2]);

#if bPRIM_STATS
		psEdges.Add(0, 2);
#endif

		fxLineLength	= edge_right.lineStart.fxX - edge_left.lineStart.fxX;

		InitializeTopVertex(edge_left.prvFrom);
		InitializeForWalkEx(this, &edge_left);

#if bPOLY_STATS
		psDrawPolygonEdges.Add(ctmr(), 1);
#endif

#if bLOCK_STAT
		{
			CCycleTimer	ctmr_lock;
			prasScreen->Lock();
			psDrawPolygonLock.Add(ctmr_lock(), 1);
		}
#else
		// Lock the surface for renderering.
		prasScreen->Lock();
#endif

		// Render this and subsequent trapezoids.
		for (;;)
		{
			//
			// Render the current trapezoid.
			//

			// Find the bottom of the current trapezoid.
			iYTo = Min(edge_left.prvTo->iYScr, edge_right.prvTo->iYScr);
			if (iY < iYTo)
			{
#if bPRIM_STATS
				psTrapezoids.Add(0, 1);
#endif
				fxDeltaLineLength = edge_right.lineIncrement.fxX - edge_left.lineIncrement.fxX;

				edge_left.lineStart.AssertXRange(fxLineLength);

#if bPOLY_STATS
				psDrawPolygonWalk.Add(ctmr(), 1);
#endif

				DrawSubtriangle(&edge_left.lineStart, this);
				psPixels.Add(ctmr());
			}

			if (i_left2 == i_right2)
				// We just did bottom triangle.
				break;

			int i_diff = edge_left.prvTo->iYScr - edge_right.prvTo->iYScr;

			if (i_diff <= 0)
			{
				// New left vertex and edge.
				i_left1 = i_left2;
				i_left2 = iNext(i_left2);

				if (i_diff == 0 && i_left2 == i_right2)
					// We just hit bottom trap.
					break;

				// Update line length for more accuracy, adjusting by the difference between the
				// accumulated X of the left edge, and the exact X of the new left vertex.
				fxLineLength += edge_left.lineStart.fxX;

				InitializeEdge(edge_left, &arvRasterVertices[i_left1], &arvRasterVertices[i_left2]);

#if bPRIM_STATS
				psEdges.Add(0, 1);
#endif
				InitializeForWalkEx(this, &edge_left);

				fxLineLength -= edge_left.lineStart.fxX;
			}

			if (i_diff >= 0)
			{
				// New right vertex and edge.
				i_right1 = i_right2;
				i_right2 = iPrev(i_right2);

				InitializeEdge(edge_right, &arvRasterVertices[i_right1], &arvRasterVertices[i_right2]);

#if bPRIM_STATS
				psEdges.Add(0, 1);
#endif

				// Update line length for more accuracy, using exact X of new right vertex,
				// and accumulated X of left edge.
				fxLineLength = edge_right.lineStart.fxX - edge_left.lineStart.fxX;
			}
		}

#if bPOLY_STATS
		psDrawPolygonWalk.Add(ctmr(), 1);
#endif
	}

	//*****************************************************************************************
	//
	forceinline int iTopIndex()
	//
	// Returns:
	//		The index of the topmost (lowest Y) vertex.
	//
	//**********************************
	{
		// Find top point.
		int i_ymin = 0;

		for (int i = 1; i < iNumRasterVertices; i++)
		{
			if (arvRasterVertices[i].iYScr < arvRasterVertices[i_ymin].iYScr)
				i_ymin = i;
		}

		return i_ymin;
	}

	//*****************************************************************************************
	//
	forceinline void InitializeTopVertex
	(
		SRenderVertex* prv_top
	)
	//
	// Sets the line walking variables for the top of any sub-polygon.
	//
	//**************************************
	{
		iY = prv_top->iYScr;
		iLineStartIndex = iY * prasScreen->iLinePixels;
	}

	//*****************************************************************************************
	//
	forceinline void InitializeForWalk
	(
		CEdge<LINE>* pedge				// Base edge.
	)
	//
	// Prepares triangle for edge walking given a base edge.
	//
	//**************************************
	{
		Assert(pedge);

#if bPRIM_STATS
		psBaseEdges.Add(0, 1);
		psLines.Add(0, pedge->prvTo->iYScr - pedge->prvFrom->iYScr);
#endif

		// Initialize edge chosen to be the base edge.
		pedge->lineStart.InitializeAsBase(pedge, this);
	}

	//*****************************************************************************************
	//
	forceinline bool bBeginValidEdge
	(
		const CEdge<LINE>* pedge_opposite	// Edge opposite the base edge.
	)
	//
	// Prepares triangle for edge walking based on a left and right edge.
	//
	// Returns:
	//		Returns 'true' if the edge is visible, otherwise returns 'false.'
	//
	//**************************************
	{
		Assert(pedge_opposite);
		Assert(pedge_opposite->prvTo);

		iYTo = pedge_opposite->prvTo->iYScr;
		return iY < iYTo;
	}

	//*****************************************************************************************
	//
	forceinline void InitializeForEdge
	(
		const CEdge<LINE>* pedge_left,	// Base edge.
		const CEdge<LINE>* pedge_right	// Edge opposite the base edge.
	)
	//
	// Updates triangle information for rasterizing a new edge.
	//
	//**************************************
	{
		Assert(pedge_left);
		Assert(pedge_right);

#if bPRIM_STATS
		psTrapezoids.Add(0, 1);
#endif

		fxLineLength      = (pedge_right->lineStart.fxX - pedge_left->lineStart.fxX);
		fxDeltaLineLength = (pedge_right->lineIncrement.fxX - pedge_left->lineIncrement.fxX);
	}

	//*****************************************************************************************
	//
	forceinline void RenderFromLeft
	(
		CEdge<LINE>* pedge_left,
		CEdge<LINE>* pedge_right0,
		CEdge<LINE>* pedge_right1
	)
	//
	// Scans through triangle edges using the single left edge as the base edge.
	//
	// Notes:
	//		The triangle is subdivided and scanned as follows:
	//
	//                   |\
	//                   | \
	//                   |  \  right edge 0:
	//        left edge  |   \ (top triangle subdivision)
	//       (base edge) |    \
	//                   |     \
	//                   |-------
	//                   |      /
	//                   |     /
	//                   |    /
	//                   |   / right edge 1:
	//                   |  /  (bottom triangle subdivision)
	//                   | /
	//                   |/
	//            
	//**************************************
	{
		CCycleTimer	ctmr;

		Assert(pedge_left);
		Assert(pedge_right0);
		Assert(pedge_right1);

		InitializeTopVertex(pedge_left->prvFrom);

		// Initialize triangle-wide data based on the single left edge.
		InitializeForWalkEx(this, pedge_left);

#if bPOLY_STATS
		psDrawPolygonWalk.Add(ctmr(), 1);
#endif

#if bLOCK_STAT
		{
			CCycleTimer	ctmr_lock;
			prasScreen->Lock();
			psDrawPolygonLock.Add(ctmr_lock(), 1);
		}
#else
		// Lock the surface for renderering.
		prasScreen->Lock();
#endif

		// Scan top sub-triangle of the triangle.
		if (bBeginValidEdge(pedge_right0))
		{
			InitializeForEdge(pedge_left, pedge_right0);
			{
				pedge_left->lineStart.AssertXRange(fxLineLength);

#if bPOLY_STATS
				psDrawPolygonWalk.Add(ctmr(), 1);
#endif

				DrawSubtriangle(&pedge_left->lineStart, this);
				psPixels.Add(ctmr());
			}
		}
		
		// Scan the bottom sub-triangle of the triangle.
		if (bBeginValidEdge(pedge_right1))
		{
			InitializeForEdge(pedge_left, pedge_right1);
			{
				pedge_left->lineStart.AssertXRange(fxLineLength);

#if bPOLY_STATS
				psDrawPolygonWalk.Add(ctmr(), 1);
#endif

				DrawSubtriangle(&pedge_left->lineStart, this);
				psPixels.Add(ctmr());
			}
		}
	}

	//*****************************************************************************************
	//
	forceinline void RenderFromRight
	(
		CEdge<LINE>* pedge_left0,
		CEdge<LINE>* pedge_left1,
		CEdge<LINE>* pedge_right
	)
	//
	// Scans through triangle edges using the two left edges as base edges.
	//
	// Notes:
	//		The triangle is subdivided and scanned as follows:
	//
	//                         /|
	//                        / |
	//       left edge 0:    /  |  
	//      (top triangle   /   | right edge
	//       subdivision)  /    | 
	//                    /     |
	//                   -------|
	//                   \      |
	//                    \     | 
	//      left edge 1:   \    |
	//   (bottom triangle   \   |
	//       subdivision)    \  |
	//                        \ |
	//                         \|
	//
	//**************************************
	{
		CCycleTimer	ctmr;

		Assert(pedge_left0);
		Assert(pedge_left1);
		Assert(pedge_right);

		InitializeTopVertex(pedge_left0->prvFrom);
		InitializeForWalkEx(this, pedge_left0);

		fxLineLength = (pedge_right->lineStart.fxX - pedge_left0->lineStart.fxX);

#if bPOLY_STATS
		psDrawPolygonWalk.Add(ctmr(), 1);
#endif

#if bLOCK_STAT
		{
			CCycleTimer	ctmr_lock;
			prasScreen->Lock();
			psDrawPolygonLock.Add(ctmr_lock(), 1);
		}
#else
		// Lock the surface for renderering.
		prasScreen->Lock();
#endif

		// Scan top sub-triangle of the triangle.
		iYTo = pedge_left0->prvTo->iYScr;
		if (iY < iYTo)
		{
			fxDeltaLineLength = (pedge_right->lineIncrement.fxX - pedge_left0->lineIncrement.fxX);

			pedge_left0->lineStart.AssertXRange(fxLineLength);

#if bPOLY_STATS
			psDrawPolygonWalk.Add(ctmr(), 1);
#endif

			DrawSubtriangle(&pedge_left0->lineStart, this);
			psPixels.Add(ctmr());
		}

		// Scan the bottom sub-triangle of the triangle.
		iYTo = pedge_left1->prvTo->iYScr;
		if (iY < iYTo)
		{
			InitializeForWalkEx(this, pedge_left1);

			// Update line length for more accuracy, adjusting by the difference between the
			// accumulated X of the left edge, and the exact X of the new left vertex.
			fxLineLength += pedge_left0->lineStart.fxX;
			fxLineLength -= pedge_left1->lineStart.fxX;

			fxDeltaLineLength = (pedge_right->lineIncrement.fxX - pedge_left1->lineIncrement.fxX);

			pedge_left1->lineStart.AssertXRange(fxLineLength);

#if bPOLY_STATS
			psDrawPolygonWalk.Add(ctmr(), 1);
#endif

			DrawSubtriangle(&pedge_left1->lineStart, this);
			psPixels.Add(ctmr());
		}
	}

};


//
// Template parameter includes.
//
#include "GouraudT.hpp"
#include "TransparencyT.hpp"
#include "MapT.hpp"
#include "IndexT.hpp"
#include "ColLookupT.hpp"
#include "Edge.hpp"
#include "Scanline.hpp"
#include "DrawSubTriangle.hpp"


#if (VER_ASM)

//
// Custom optimized versions of selected global templated polygon setup functions.
//
void InitializePolygonData(TCopyPersp& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TCopyPerspTrans& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TCopyTerrainPersp& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TTexturePersp& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TTexturePerspTrans& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TTexturePerspGour& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TTexturePerspTransGour& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TBumpPersp& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TBumpPerspTrans& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TGFogPersp& lineData, CRenderPolygon* rpoly);

void InitializePolygonData(TCopyLinear& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TCopyLinearTrans& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TCopyTerrainLinear& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TTextureLinear& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TTextureLinearTrans& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TTextureLinearGour& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TTextureLinearTransGour& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TBumpLinear& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TBumpLinearTrans& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TGFogLinear& lineData, CRenderPolygon* rpoly);

void InitializePolygonData(TTexNoClutLinear& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TTexNoClutLinearTrans& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TShadowTrans8& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TShadowTrans32& lineData, CRenderPolygon* rpoly);

#if (BILINEAR_FILTER)

void InitializePolygonData(TCopyPerspFilter& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TCopyLinearFilter& lineData, CRenderPolygon* rpoly);

#endif

void InitializePolygonData(TStippleTexPersp& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TStippleTexLinear& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TWaterPersp& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TWaterLinear& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TAlphaTexPersp& lineData, CRenderPolygon* rpoly);
void InitializePolygonData(TAlphaTexLinear& lineData, CRenderPolygon* rpoly);


void InitializeEdge(CEdge<TCopyPersp>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TCopyPerspTrans>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TCopyTerrainPersp>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TTexturePersp>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TTexturePerspTrans>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TTexturePerspGour>& edge,	SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TTexturePerspTransGour>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TBumpPersp>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TBumpPerspTrans>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TGFogPersp>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);

void InitializeEdge(CEdge<TCopyLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TCopyLinearTrans>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TCopyTerrainLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TTextureLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TTextureLinearTrans>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TTextureLinearGour>& edge,	SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TTextureLinearTransGour>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TBumpLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TBumpLinearTrans>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TGFogLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);

void InitializeEdge(CEdge<TTexNoClutLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TTexNoClutLinearTrans>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TShadowTrans8>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TShadowTrans32>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);

#if (BILINEAR_FILTER)

void InitializeEdge(CEdge<TCopyPerspFilter>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TCopyLinearFilter>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);

#endif

void InitializeEdge(CEdge<TStippleTexPersp>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TStippleTexLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TWaterPersp>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TWaterLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TAlphaTexPersp>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);
void InitializeEdge(CEdge<TAlphaTexLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to);


bool bInitTriangleDataEx(CDrawPolygon<TCopyPersp>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TCopyPerspTrans>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TCopyTerrainPersp>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TTexturePersp>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TTexturePerspTrans>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TTexturePerspGour>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TTexturePerspTransGour>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TBumpPersp>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TBumpPerspTrans>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TGFogPersp>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);

bool bInitTriangleDataEx(CDrawPolygon<TCopyLinear>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TCopyLinearTrans>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TCopyTerrainLinear>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TTextureLinear>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TTextureLinearTrans>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TTextureLinearGour>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TTextureLinearTransGour>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TBumpLinear>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TBumpLinearTrans>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TGFogLinear>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);

bool bInitTriangleDataEx(CDrawPolygon<TTexNoClutLinear>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TTexNoClutLinearTrans>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TShadowTrans8>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TShadowTrans32>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);

#if (BILINEAR_FILTER)

bool bInitTriangleDataEx(CDrawPolygon<TCopyPerspFilter>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TCopyLinearFilter>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);

#endif

bool bInitTriangleDataEx(CDrawPolygon<TStippleTexPersp>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TStippleTexLinear>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TWaterPersp>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TWaterLinear>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TAlphaTexPersp>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);
bool bInitTriangleDataEx(CDrawPolygon<TAlphaTexLinear>* poly, SRenderVertex* prv_a, 
							SRenderVertex* prv_b, SRenderVertex* prv_c, bool b_update);


bool bInitGradientDataPlanarEx(CDrawPolygon<TCopyPersp>* poly);
bool bInitGradientDataPlanarEx(CDrawPolygon<TCopyPerspTrans>* poly);
bool bInitGradientDataPlanarEx(CDrawPolygon<TCopyTerrainPersp>* poly);
bool bInitGradientDataPlanarEx(CDrawPolygon<TTexturePersp>* poly);
bool bInitGradientDataPlanarEx(CDrawPolygon<TTexturePerspTrans>* poly);
bool bInitGradientDataPlanarEx(CDrawPolygon<TBumpPersp>* poly);
bool bInitGradientDataPlanarEx(CDrawPolygon<TBumpPerspTrans>* poly);

#if (BILINEAR_FILTER)

bool bInitGradientDataPlanarEx(CDrawPolygon<TCopyPerspFilter>* poly);

#endif

bool bInitGradientDataPlanarEx(CDrawPolygon<TStippleTexPersp>* poly);
bool bInitGradientDataPlanarEx(CDrawPolygon<TWaterPersp>* poly);
bool bInitGradientDataPlanarEx(CDrawPolygon<TAlphaTexPersp>* poly);


#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

//
// We can't beat the compiled code on the Pentium for this routine so
// only K6_3D version exist.
//
void InitializeForWalkEx(CDrawPolygon<TCopyPersp>* poly, CEdge<TCopyPersp>* pedge);
void InitializeForWalkEx(CDrawPolygon<TCopyPerspTrans>* poly, CEdge<TCopyPerspTrans>* pedge);
void InitializeForWalkEx(CDrawPolygon<TCopyTerrainPersp>* poly, CEdge<TCopyTerrainPersp>* pedge);
void InitializeForWalkEx(CDrawPolygon<TTexturePersp>* poly, CEdge<TTexturePersp>* pedge);
void InitializeForWalkEx(CDrawPolygon<TTexturePerspTrans>* poly, CEdge<TTexturePerspTrans>* pedge);
void InitializeForWalkEx(CDrawPolygon<TTexturePerspGour>* poly, CEdge<TTexturePerspGour>* pedge);
void InitializeForWalkEx(CDrawPolygon<TTexturePerspTransGour>* poly, CEdge<TTexturePerspTransGour>* pedge);
void InitializeForWalkEx(CDrawPolygon<TBumpPersp>* poly, CEdge<TBumpPersp>* pedge);
void InitializeForWalkEx(CDrawPolygon<TBumpPerspTrans>* poly, CEdge<TBumpPerspTrans>* pedge);
void InitializeForWalkEx(CDrawPolygon<TGFogPersp>* poly, CEdge<TGFogPersp>* pedge);

void InitializeForWalkEx(CDrawPolygon<TCopyLinear>* poly, CEdge<TCopyLinear>* pedge);
void InitializeForWalkEx(CDrawPolygon<TCopyLinearTrans>* poly, CEdge<TCopyLinearTrans>* pedge);
void InitializeForWalkEx(CDrawPolygon<TCopyTerrainLinear>* poly, CEdge<TCopyTerrainLinear>* pedge);
void InitializeForWalkEx(CDrawPolygon<TTextureLinear>* poly, CEdge<TTextureLinear>* pedge);
void InitializeForWalkEx(CDrawPolygon<TTextureLinearTrans>* poly, CEdge<TTextureLinearTrans>* pedge);
void InitializeForWalkEx(CDrawPolygon<TTextureLinearGour>* poly, CEdge<TTextureLinearGour>* pedge);
void InitializeForWalkEx(CDrawPolygon<TTextureLinearTransGour>* poly, CEdge<TTextureLinearTransGour>* pedge);
void InitializeForWalkEx(CDrawPolygon<TBumpLinear>* poly, CEdge<TBumpLinear>* pedge);
void InitializeForWalkEx(CDrawPolygon<TBumpLinearTrans>* poly, CEdge<TBumpLinearTrans>* pedge);
void InitializeForWalkEx(CDrawPolygon<TGFogLinear>* poly, CEdge<TGFogLinear>* pedge);

void InitializeForWalkEx(CDrawPolygon<TTexNoClutLinear>* poly, CEdge<TTexNoClutLinear>* pedge);
void InitializeForWalkEx(CDrawPolygon<TTexNoClutLinearTrans>* poly, CEdge<TTexNoClutLinearTrans>* pedge);
void InitializeForWalkEx(CDrawPolygon<TShadowTrans8>* poly, CEdge<TShadowTrans8>* pedge);
void InitializeForWalkEx(CDrawPolygon<TShadowTrans32>* poly, CEdge<TShadowTrans32>* pedge);

#if (BILINEAR_FILTER)

void InitializeForWalkEx(CDrawPolygon<TCopyPerspFilter>* poly, CEdge<TCopyPerspFilter>* pedge);
void InitializeForWalkEx(CDrawPolygon<TCopyLinearFilter>* poly, CEdge<TCopyLinearFilter>* pedge);

#endif

void InitializeForWalkEx(CDrawPolygon<TStippleTexPersp>* poly, CEdge<TStippleTexPersp>* pedge);
void InitializeForWalkEx(CDrawPolygon<TStippleTexLinear>* poly, CEdge<TStippleTexLinear>* pedge);
void InitializeForWalkEx(CDrawPolygon<TWaterPersp>* poly, CEdge<TWaterPersp>* pedge);
void InitializeForWalkEx(CDrawPolygon<TWaterLinear>* poly, CEdge<TWaterLinear>* pedge);
void InitializeForWalkEx(CDrawPolygon<TAlphaTexPersp>* poly, CEdge<TAlphaTexPersp>* pedge);
void InitializeForWalkEx(CDrawPolygon<TAlphaTexLinear>* poly, CEdge<TAlphaTexLinear>* pedge);


//
// This routine is used on the K6_3D to empty the MMX state.
//
void DoneDrawingPolygon(CDrawPolygon<TCopyPersp>* poly);
void DoneDrawingPolygon(CDrawPolygon<TCopyPerspTrans>* poly);
void DoneDrawingPolygon(CDrawPolygon<TCopyTerrainPersp>* poly);
void DoneDrawingPolygon(CDrawPolygon<TTexturePersp>* poly);
void DoneDrawingPolygon(CDrawPolygon<TTexturePerspTrans>* poly);
void DoneDrawingPolygon(CDrawPolygon<TTexturePerspGour>* poly);
void DoneDrawingPolygon(CDrawPolygon<TTexturePerspTransGour>* poly);
void DoneDrawingPolygon(CDrawPolygon<TBumpPersp>* poly);
void DoneDrawingPolygon(CDrawPolygon<TBumpPerspTrans>* poly);
void DoneDrawingPolygon(CDrawPolygon<TGFogPersp>* poly);

void DoneDrawingPolygon(CDrawPolygon<TCopyLinear>* poly);
void DoneDrawingPolygon(CDrawPolygon<TCopyLinearTrans>* poly);
void DoneDrawingPolygon(CDrawPolygon<TCopyTerrainLinear>* poly);
void DoneDrawingPolygon(CDrawPolygon<TTextureLinear>* poly);
void DoneDrawingPolygon(CDrawPolygon<TTextureLinearTrans>* poly);
void DoneDrawingPolygon(CDrawPolygon<TTextureLinearGour>* poly);
void DoneDrawingPolygon(CDrawPolygon<TTextureLinearTransGour>* poly);
void DoneDrawingPolygon(CDrawPolygon<TBumpLinear>* poly);
void DoneDrawingPolygon(CDrawPolygon<TBumpLinearTrans>* poly);
void DoneDrawingPolygon(CDrawPolygon<TGFogLinear>* poly);

void DoneDrawingPolygon(CDrawPolygon<TTexNoClutLinear>* poly);
void DoneDrawingPolygon(CDrawPolygon<TTexNoClutLinearTrans>* poly);
void DoneDrawingPolygon(CDrawPolygon<TShadowTrans8>* poly);
void DoneDrawingPolygon(CDrawPolygon<TShadowTrans32>* poly);

#if (BILINEAR_FILTER)

void DoneDrawingPolygon(CDrawPolygon<TCopyPerspFilter>* poly);
void DoneDrawingPolygon(CDrawPolygon<TCopyLinearFilter>* poly);

#endif

void DoneDrawingPolygon(CDrawPolygon<TStippleTexPersp>* poly);
void DoneDrawingPolygon(CDrawPolygon<TStippleTexLinear>* poly);
void DoneDrawingPolygon(CDrawPolygon<TWaterPersp>* poly);
void DoneDrawingPolygon(CDrawPolygon<TWaterLinear>* poly);
void DoneDrawingPolygon(CDrawPolygon<TAlphaTexPersp>* poly);
void DoneDrawingPolygon(CDrawPolygon<TAlphaTexLinear>* poly);


#endif // if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

#endif // if (VER_ASM)


//
// Generic versions of global templated polygon setup functions.
//
template <class LINE> forceinline void InitializePolygonData
(
	LINE& lineData, 
	CRenderPolygon* rpoly
)
{
	lineData.InitializePolygonData(rpoly->paprvPolyVertices, rpoly->ptexTexture, rpoly->cvFace,
		                           rpoly->iMipLevel);
}

template <class LINE> forceinline void InitializeEdge
(
	CEdge<LINE>& edge,			// The edge to initialize.
	SRenderVertex* prv_from,	// Starting coordinate of edge.
	SRenderVertex* prv_to		// End coordinate of edge.
)
{
	edge.InitializeEdge(prv_from, prv_to);
}

template <class LINE> forceinline bool bInitTriangleDataEx
(
	CDrawPolygon<LINE>*		poly,
	SRenderVertex*			prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*			prv_b,
	SRenderVertex*			prv_c,
	bool					b_update
)
{
	return poly->bInitTriangleData(prv_a, prv_b, prv_c, b_update);
}

template <class LINE> forceinline bool bInitGradientDataPlanarEx
(
	CDrawPolygon<LINE>* poly
)
{
	return poly->bInitGradientDataPlanar();
}

template <class LINE> forceinline void InitializeForWalkEx
(
	CDrawPolygon<LINE>* poly, 
	CEdge<LINE>* pedge
)
{
		Assert(pedge);

#if bPRIM_STATS
		psBaseEdges.Add(0, 1);
		psLines.Add(0, pedge->prvTo->iYScr - pedge->prvFrom->iYScr);
#endif

		// Set base edge.
		poly->pedgeBase = pedge;

		// Initialize edge chosen to be the base edge.
		pedge->lineStart.InitializeAsBase(pedge, poly);
}

template <class LINE> forceinline void DoneDrawingPolygon
(
	CDrawPolygon<LINE>* poly
)
{
	// Do nothing.
	return;
}


#endif	// if HEADER_RENDERER_PRIMITIVES_DRAWTRIANGLE_HPP
