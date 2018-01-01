/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of ScreenRender.hpp
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ScreenRender.cpp                                         $
 * 
 * 148   98.09.24 1:40a Mmouni
 * Shrunk CRenderPolygon by a large amount.
 * Multiple list depth sort stuff is now on a compile switch.
 * 
 * 147   98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 146   9/15/98 4:19p Pkeet
 * Reduced problem with objects flashing white after loading a new level.
 * 
 * 145   98.09.10 4:08p Mmouni
 * Removed area culling since polygons are already culled if they do not light any pixels.
 * Disabled turing off texturing for bump map polys since they do not get lighting values that
 * are useable without the bump map.
 * 
 * 144   8/18/98 8:47p Pkeet
 * Removed the 'bFIX_ADJACENT_CRACKS' macro.
 * 
 * 143   8/18/98 3:49p Pkeet
 * Hardware mip levels now take into account the recommended maximum dimension.
 * 
 * 142   8/12/98 6:57p Pkeet
 * Implemented a trial fix for terrain cracking.
 * 
 * 141   8/04/98 6:16p Pkeet
 * Made 'ReduceFeatures' handle hardware polygons correctly.
 * 
 * 140   8/03/98 8:53p Agrant
 * bogus assert fixed per Paul
 * 
 * 139   8/03/98 11:05a Pkeet
 * Commented out asserts.
 * 
 * 138   8/02/98 8:37p Pkeet
 * Added the 'SetHardwareOut' member function.
 * 
 * 137   98.08.02 7:26p Mmouni
 * Changed default alt. perspective error to 2.0
 * 
 * 136   8/01/98 4:42p Pkeet
 * Added the 'pixSetBackground' member function.
 * 
 * 135   7/30/98 4:10p Pkeet
 * Large textures are disabled in hardware if texture create times are slow.
 * 
 * 134   98.07.30 11:43a Mmouni
 * Switched to area based mip-mapping.
 * 
 * 133   7/29/98 2:24p Pkeet
 * Disabled feature reduction for hardware.
 * 
 * 132   7/29/98 1:57p Pkeet
 * Added a parameter to the 'SetMipLevel' member function of 'CRenderPolygon' to reduce the
 * burden on the VM loader by making VM requests only when the Direct3D version is required and
 * is present.
 * 
 * 131   7/28/98 8:26p Pkeet
 * Added the member function 'Grow' to 'CRenderPolygon,' but did not implement any code for it.
 * 
 * 130   98.07.24 8:47p Mmouni
 * Added Gouraud shading threshold area.
 * 
 * 129   7/23/98 6:24p Pkeet
 * Added support for texturing in hardware out of system memory.
 * 
 * 128   6/24/98 3:21p Rwyatt
 * Now longer check if all VM is commited before checking available memory. This is now part of
 * the VM.
 * 
 * 127   6/21/98 8:01p Pkeet
 * Polygons affected by feature reduction now have their hardware flags reset. This prevents a
 * nasty 'flashing' bug.
 * 
 * 126   98.06.18 3:55p Mmouni
 * Added software alpha switch.
 * 
 * 125   6/16/98 7:28p Pkeet
 * Maded 'bCalculateScreenExtents' return false only if the polygon is not terrain.
 * 
 * 124   6/15/98 7:16p Pkeet
 * Fixed bug that caused small terrain polygons to dissappear.
 * 
 * 123   6/14/98 2:48p Pkeet
 * Added the 'fGetFarthestZ' member function.
 * 
 * 122   6/09/98 3:52p Pkeet
 * Added a flag to 'SetD3DFlagForPolygons' to use Direct3D or not.
 * 
 * 121   6/08/98 8:08p Pkeet
 * Modified the 'SetD3DFlagForPolygons' parameters to work in the pipeline.
 * 
 * 120   6/02/98 11:12a Pkeet
 * Added the 'SetD3DFlagForPolygons' member function.
 * 
 * 119   98.05.23 8:10p Mmouni
 * Made bi-linear filter default to on in K63D build.
 * 
 * 118   98.05.21 11:31p Mmouni
 * Added validate function to CRenderPolygon.
 * 
 * 117   4/28/98 4:59p Rwyatt
 * When checking the availability of VM textures we now use the correct length.
 * 
 * 116   98.04.24 6:59p Mmouni
 * Changed alternate perspective correction setting.
 * 
 * 115   4/21/98 3:28p Rwyatt
 * Fixed merge problems
 * 
 * 114   4/21/98 2:51p Rwyatt
 * Added memory present and memory request functions to the mip selection code.
 * 
 * 113   98.04.16 8:00p Mmouni
 * Changed mip-map selection algorithm.
 * 
 * 112   98.04.08 7:45p Mmouni
 * Fixed a potential access violation reading a value off the end of the vertex array.
 * 
 * 111   4/01/98 5:43p Pkeet
 * Added the 'erfD3D_CACHE' render enumeration.
 * 
 * 110   3/30/98 3:10p Agrant
 * Placeholder code for mip selection based on raster availability.
 * 
 * 109   98.03.24 8:16p Mmouni
 * Made changes to support alternate perspective correction settings.
 * 
 * 108   3/17/98 4:42p Pkeet
 * Fixed culling of thin horizontal or vertical polygons.
 * 
 * 107   3/17/98 3:23p Pkeet
 * Fixed bug that caused horizontal gaps in the terrain.
 * 
 * 106   98/03/02 21:42 Speter
 * Combined SetAccept() into ReduceFeatures().  Do not disable texture for software terrain.
 * 
 * 105   98/02/26 15:38 Speter
 * Removed erfALPHA_SHADE and erfALPHA_TEXTURE.
 * 
 * 104   98/02/26 13:52 Speter
 * Moved seterfFeatures to CTexture from SSurface, removing redundant flags.
 * 
 * 103   2/06/98 4:17p Agrant
 * Turn on fogging by default
 * 
 * 102   98.02.05 4:50p Mmouni
 * Removed K6-3D inline assembly.
 * 
 * 101   98.02.04 5:01p Mmouni
 * Changes bInFrontOf() to use a looser in-front-of condition.
 * 
 * 100   98/02/04 14:44 Speter
 * Removed fGetRoughScreenArea().
 * 
 * 99    98/01/30 15:55 Speter
 * Moved transparent poly rejection into texture rejection block; shadows have erfTEXTURE off to
 * start with.
 * 
 * 98    1/26/98 3:50p Pkeet
 * D3D renderer only renders flat-filled polygons if they originate from the terrain.
 * 
 * 97    1/26/98 11:29a Pkeet
 * Adjusted settings.
 * 
 * 96    1/25/98 3:39p Pkeet
 * Added the 'SetAccept' and 'ReduceFeatures' member functions.
 * 
 * 95    1/24/98 4:42p Pkeet
 * Fixed bug in screen area function.
 * 
 * 94    1/24/98 3:18p Pkeet
 * Added the use of screen area for feature reduction.
 * 
 * 93    1/23/98 5:40p Pkeet
 * Added the 'fArea' member variable and the 'SetArea' member function to the screen polygon
 * class.
 * 
 * 92    98.01.15 6:09p Mmouni
 * Optimized mip-map selection.
 * 
 * 91    1/07/98 6:22p Pkeet
 * Tiny terrain polygons for a Direct3D target are no longer culled.
 * 
 * 90    1/23/96 4:37p Pkeet
 * Initializes the 'cvFace' data member to prevent later debug build asserts.
 * 
 * 89    97/11/19 18:11 Speter
 * Adjusted ValidatePolygon() 
 * 
 * 88    11/10/97 10:02p Gfavor
 * Removed FEMMS's from esfSides (to higher level).
 * 
 * 87    11/09/97 10:07p Gfavor
 * Eliminated SetPlane3DX.
 * 
 * 86    97/11/07 12:00p Pkeet
 * Optimized the 'SetPlane' function.
 * 
 * 84    11/05/97 11:09a Gfavor
 * Fixed bInFrontOf to avoid generation of x87 code
 * 
 * 83    11/05/97 11:02a Gfavor
 * Eliminated bIntersect and modified 3DX version of esfSides.
 * 
 * 82    10/29/97 8:09p Gfavor
 * Converted esfSides to 3DX.
 * 
 * 81    97/10/23 10:57a Pkeet
 * Added a K6 3D switch.
 * 
 * 80    10/13/97 8:16p Rwyatt
 * Render settings have a new flag called bDrawSky. This controls if the sky is drawn or not. By
 * default this flag is set to true
 * 
 * 79    97/10/13 2:15p Pkeet
 * Fixed bug with alpha.
 * 
 * 78    97/10/12 21:51 Speter
 * TReflectVal rvIntensity/rvFace changed to TClutVal cvIntensity/cvFace.  Main gamma object
 * moved to CClut.
 * 
 * 77    97/10/01 4:40p Pkeet
 * Added the 'erfOCCLUDE' flag.
 * 
 * 76    97/09/10 11:35 Speter
 * Modified asserts on lighting values.
 * 
 * 75    8/18/97 2:49p Bbell
 * Depth sorting demo stuff.
 * 
 * 74    97/08/17 4:15p Pkeet
 * Added the 'erfSOURCE_WATER' flag.
 * 
 * 73    8/17/97 12:18a Agrant
 * Put texture coord clamping on a VER switch.
 * 
 * 72    97/08/15 12:08 Speter
 * Moved ValidatePolygon and ValidateVertex to CScreenRender base class.  Removed commented
 * code.  Now call ValidatePolygon for every polygon in DrawPolygons.
 * 
 * 71    97/08/09 1:56p Pkeet
 * Added flags for alpha blending.
 * 
 * 70    97/08/08 5:50p Pkeet
 * Fixed bug in mipmap calculations of delta u and delta v. Organized and commented code.
 * 
 * 69    97/08/08 11:51a Pkeet
 * Function now selects mip level. Added the render feature flag for mipmapping.
 * 
 * 68    97/08/07 11:35a Pkeet
 * Added interface support for mipmapping.
 * 
 * 67    97/07/30 2:42p Pkeet
 * Added support for separate tolerances.
 * 
 * 66    97/07/29 6:31p Pkeet
 * Added the 'fGetAverageZ' member function.
 * 
 * 65    97/07/28 11:17a Pkeet
 * Added the 'erfSOURCE_TERRAIN' and 'erfDRAW_CLIP' flags to the default settings constructor.
 * 
 * 64    97/07/25 16:57 Speter
 * SetPlane now uses points distributed around the polygon rather than first 3.
 * 
 * 63    97/07/23 18:11 Speter
 * Replaced CCamera argument in clipping functions with b_perspective flag.  Again use vertex
 * interpolation for screen coords rather than camera reprojection.
 * 
 * 62    97/07/08 20:19 Speter
 * Moved bHalfScanlines and bDoubleVertical here.
 * 
 * 61    97/07/07 13:55 Speter
 * Removed SRenderVertex.iX and .iY.
 * 
 * 60    97/06/27 15:36 Speter
 * Added erfTRAPEZOIDS feature.
 * 
 * 59    97/06/25 1:14p Pkeet
 * Includes code for displaying depth sort clipping.
 * 
 * 58    97/06/23 20:32 Speter
 * Made gcfScreen a static member of CScreenRender, avoiding per-CScreenRender calculation (e.g.
 * in render caching).
 * 
 * 57    97/06/17 2:03p Pkeet
 * Removed the reference counting pointer from prasScreen.
 * 
 * 56    6/16/97 3:00p Mlange
 * Updated for fast float to integer conversion functions' name change.
 * 
 * 55    97/06/11 12:30 Speter
 * Restored b_cull_tiny param to bCalculateScreenExtents.  Made SRenderVertex default
 * constructor null.
 * 
 * 54    97/06/10 15:43 Speter
 * Added iX member to SRenderVertex.  Now pass camera down to interpolation constructor.
 * Changed CRenderPolygon::[b]InitializePolygonForDepthSort() to bCalculateScreenExtents(),
 * which relies on values preset by pipeline.
 * 
 * 53    97/06/03 18:52 Speter
 * Added #include FloatDef.hpp.
 * 
 * 52    97/06/02 15:55 Speter
 * Put back camera projection for v3Screen only, to allow for parallel cameras.
 * 
 * 51    97/05/24 3:56p Pkeet
 * Added the 'InitializePolygonForDepthSort' member function. Utilized optimized integer
 * conversion functions.
 * 
 * 50    97/05/23 6:28p Pkeet
 * Added asserts and the fast make call for setting the plane of the polygon.
 * 
 * 49    97/05/23 17:46 Speter
 * Bye bye SRenderTriangle.  Merged into CRenderPolygon, removed some unused fields and
 * constructors.  Made FindBestSlope and BuildVertexArray member functions.  Added
 * fRoughScreenArea function.
 * 
 * 48    97/05/22 3:59p Pkeet
 * Made 'InitializePolygonForDepthSort' return a boolean value to indicate if the polygon is
 * usable or has been culled.
 * 
 * 47    97/05/21 4:16p Pkeet
 * Added 'fPixelBuffer' tolerances.
 * 
 * 46    97/05/21 10:40a Pkeet
 * Uses the minimum tolerance of two polygons.
 * 
 * 45    97/05/20 11:40a Pkeet
 * Added variable plane tolerances for depth sorting.
 * 
 * 44    97/05/17 4:34p Pkeet
 * Further assembly optimizations.
 * 
 * 43    97/05/17 3:38p Pkeet
 * Improvement in the ASM and debugging of the non-asm plane side test.
 * 
 * 42    97/05/15 7:20p Pkeet
 * Fixed bug in ASM.
 * 
 * 41    97/05/15 6:34p Pkeet
 * Assembly optimization of plane intersection test.
 * 
 * 40    97-05-08 13:19 Speter
 * Moved initialisation of SBumpLighting member from SRenderTriangle, etc. to SBumpLighting
 * constructor.  Made SRenderTriangle constructor inline.
 * 
 * 39    97/05/06 4:19p Pkeet
 * Added a sorting key object.
 * 
 * 38    97-05-06 16:12 Speter
 * Replaced call to AddRenderDescDWI() with explicit add of CRenderDescDWI instance to array.
 * 
 * 37    97/04/28 6:14p Pkeet
 * Fixed conversion bug.
 * 
 * 36    97/04/28 5:43p Pkeet
 * Added the 'bBehind,' 'bInFrontOf' and 'bIntersect' member functions to CRenderPolygon. Moved
 * plane tests here from the depth sort module.
 * 
 * 35    97/04/25 2:51p Pkeet
 * Changed the interpolating SRenderVertex constructor to use the camera projection method
 * instead of the unproject/reproject method. This change results in less errors in the current
 * implementation of the depth sort routine.
 * 
 * 34    97-04-21 16:50 Speter
 * Changed optcCam to v3Cam.  In vertex interpolation, re-enabled direct v3Screen interpolation.
 * Now check for v3Screen.tZ == 0 before doing v3Screen interpolation.
 * 
 * 33    97/04/16 5:15p Pkeet
 * Fixed assert problem in higher screen resolutions.
 * 
 * 32    97/04/16 3:01p Pkeet
 * Fixed scope bug in the interpolation constructor of SRenderVertex.
 * 
 * 31    97-04-16 14:18 Speter
 * Added correct screen vertex interpolation.
 * 
 * 30    97/04/15 1:46p Pkeet
 * Added improved polygon to triangle subdivision code.
 * 
 * 29    97/04/14 11:56a Pkeet
 * Changed 'MinInv' value to 'rMaxLevel.'
 * 
 * 28    97/04/13 2:16p Pkeet
 * Added the 'InitializePolygonForDepthSort' member function to CRenderPolygon. Added the global
 * function 'SetCurrentCamera.' Reworked the SRenderVertex interpolation constructor used by the
 * clipper to use a camera set by SetCurrentCamera. This is a temporary measure to overcome
 * current bugs in the unproject/project code currently in the constructor. Changed the
 * DrawPolygon function so that if the polygon is a triangle the contents of 'aprvVertices' will
 * point to the same render vertices as 'paprvPolyVertices.' Also fixed bug in the
 * retriangulization code.
 * 
 * 27    97/04/10 11:33a Pkeet
 * Added the 'iClipped' member variable to the polygon definition.
 * 
 * 26    97-04-10 11:31 Speter
 * Added v3Screen interpolation in interpolator constructor.  Changed rvIntensity to use 2D
 * interpolation.
 * 
 * 25    97-04-09 16:23 Speter
 * Added SRenderVertex interpolation constructor.
 * 
 * 24    97/04/09 3:13p Pkeet
 * Probable fix to probable bug in the naive polygon triangulation implementation. Added code
 * for a polygon triangulation algorithm which should produce the best triangles for rendering.
 * 
 * 23    97/04/09 12:20p Pkeet
 * The 'DrawPolygon' member function now busts polygons up into triangles.
 * 
 * 22    97/04/07 5:31p Pkeet
 * Added the 'bMovedInList' member variable to CRenderPolygon.
 * 
 * 21    97/04/07 2:28p Pkeet
 * Added an assignment operator and copy constructor to CRenderPolygon that ensures the polygon
 * uses the base class vertex array whenever appropriate.
 * 
 * 20    97-04-04 12:38 Speter
 * A big change: now pipeline uses CRenderPolygon rather than SRenderTriangle (currently always
 * as a triangle).  Changed associated variable names and comments.
 * 
 * 19    97-03-28 17:21 Speter
 * Removed erroneous symbol.
 * 
 * 18    97-03-28 16:00 Speter
 * Renamed SRenderSettings to CScreenRender::SSettings; moved bClip member to new
 * CRenderer::Settings.
 * Removed psrMain, replaced with prenMain.
 * 
 * 17    97/02/25 12:51p Pkeet
 * Added a switch for including and using Direct3D, and set the switch to 'false.'
 * 
 * 16    97/02/21 15:20 Speter
 * Moved prasScreen to CScreenRender from derived classes.
 * 
 * 15    97/02/16 16:59 Speter
 * Turn on specular by default.
 * 
 * 14    97/02/13 4:22p Pkeet
 * Added an init colour for the background. Set the default colour for the background to black.
 * 
 * 13    97/02/13 14:18 Speter
 * Incorporated new SBumpLighting struct into SRenderTriangle.
 * 
 * 12    97/01/29 18:51 Speter
 * Made SRenderTriangle default constructor noninline, as it wasn't being inlined.
 * 
 * 11    97/01/20 18:32 Speter
 * Added bClip to SRenderSettings.
 * 
 * 10    97/01/20 11:50 Speter
 * Moved pixBackground and gcfScreen vars from CScreenRender to derived classes.  Added erfCOPY
 * to default settings.
 * 
 * 9     97/01/16 11:44 Speter
 * Made SRenderSettings set the default state to reasonable values.
 * Removed base class version of CScreenRender::UpdateSettings().
 * 
 * 8     96/12/31 16:54 Speter
 * Updated for rptr.
 * 
 * 7     96/12/17 13:13 Speter
 * Added gamma correction functionality.
 * Moved psrMain global variable here from Render.cpp.
 * 
 * 6     96/12/06 15:12 Speter
 * Added SRenderSettings structure, and UpdateSettings() function for CScreenRender.  
 * 
 * 
 * 5     96/12/02 17:47 Speter
 * Replaced ValidateRenderState() with CheckRenderState() and CorrectRenderState().
 * 
 * 4     96/09/25 15:33 Speter
 * Moved ValidateRenderState to .cpp file.
 * 
 * 3     96/08/22 15:28 Speter
 * Put back D3D support.
 * 
 * 2     96/08/21 16:45 Speter
 * Remove D3D support temporarily to sort out MFC link problem.
 * 
 * 1     96/08/19 12:51 Speter
 * First version, initialises the CArrayRenderDesc variable.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Sys/VirtualMem.hpp"
#include "Lib/Loader/TextureManager.hpp"
#include "ScreenRender.hpp"
#include "Camera.hpp"
#include "ScreenRenderDWI.hpp"
#include "DepthSort.hpp"	
#include "Lib/Math/FloatDef.hpp"
#include "Lib/Math/FastInverse.hpp"
#include "AsmSupport.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3DUtilities.hpp"

#include <Memory.h>

//
// Module defines.
//

// Represents the maximum number of vertices a temporary polygon may have.
#define iMAX_TEMP_VERTICES (40)

// Flag to do some depth sorting hack stuff.
#define bDEMO_BUILD        (0)

// Flag to use screen area for feature reduction.
#define bUSE_SCREEN_AREA   (1)

#if bUSE_SCREEN_AREA
	// Screen area threshold for polygon to use the smallest mip level.
	const float fUseSmallestMipArea = 20.0f;
#endif // bUSE_SCREEN_AREA

// Thresholds for feature reduction.
const float fTextureThresholdArea     =  4.0f;
const float fPerspectiveThresholdArea = 32.0f;
const float fPerspectiveThresholdLen  = 12.0f;
const float fGourShadeThresholdArea	  = 16.0f;
float fPerspectivePixelError          =  1.0;
float fAltPerspectivePixelError       =  2.0;
float fMinZPerspective                =  0.3;


//
// Module specific variables.
//
CColour clrInitColour = CColour(0, 0, 0);


//
// Module specific inline functions.
//

//**********************************************************************************************
//
inline int iMaxDim
(
	rptr<CRaster> pras
)
//
// Returns the maximum dimension of the raster.
//
//**********************************
{
	return Max(pras->iWidth, pras->iHeight);
}

//**********************************************************************************************
//
inline float fGetSlope
(
	const CVector3<>& v3_0,
	const CVector3<>& v3_1
)
//
// Returns:
//		The y/x slope.
//
//**********************************
{
	// Prevent division by zero.
	if (v3_0.tX == v3_1.tX)
		return 99999.0f;

	return Abs((v3_0.tY - v3_1.tY) / (v3_0.tX - v3_1.tX));
}

//**********************************************************************************************
//
inline ESideOf iPlaneDirection
(
	const CPlane& pl	// Plane to test direction.
)
//
// Returns 'esfON, 'esfOUTSIDE,' or 'esfINSIDE.'
//
//**********************************
{
	return (u4FromFloat(pl.rD) & 0x80000000L) ? (esfINSIDE) : (esfOUTSIDE);
}


//**********************************************************************************************
//
inline ESideOf esfSides
(
	const CPlane&         pl,			// Plane to test against.
	const CRenderPolygon* prpoly,		// Pointer to the render polygon.
	int                   esf_side,		// Flags.
	TReal r_plane_tolerance				// Tolerance for plane test.
)
//
// Returns 'true' if the polygon intersects the plane.
//
//**********************************
{
	Assert(prpoly);

	// Iterate through the vertices to get the side the polygon is on.
	for (int i_vertex = 0; i_vertex < prpoly->paprvPolyVertices.uLen; i_vertex++)
	{
		float f_dist = pl.rDistance(prpoly->paprvPolyVertices[i_vertex]->v3Cam);

		#if VER_ASM

			__asm
			{
				mov eax, f_dist
				mov ecx, 0x00000002

				and eax, 0x7FFFFFFF
				mov ebx, [esf_side]

				cmp eax, [r_plane_tolerance]
				jle short CONTINUE_SIDE

				sub eax, f_dist
				sbb ecx, 0

				or ecx, ebx

				cmp ecx, esfINTERSECT
				je short RETURN_INTERSECT

				mov [esf_side], ecx
			CONTINUE_SIDE:
			}

		#else // VER_ASM

			// Is on the negative side?
			if (r_dist < -r_plane_tolerance)
				esf_side |= esfINSIDE;

			// Is on the positive side?
			if (r_dist > r_plane_tolerance)
				esf_side |= esfOUTSIDE;

			// Must lay on the plane, do nothing.

			// Return early if the polygon intersects the plane.
			if (esf_side == esfINTERSECT)
				return esfINTERSECT;

		#endif // VER_ASM
	}
	return esf_side;
RETURN_INTERSECT:
	return esfINTERSECT;
}


//**********************************************************************************************
//
inline bool bAllOppositeSide
(
	const CPlane&         pl,		// Plane to test against.
	const CRenderPolygon* prpoly,	// Pointer to the render polygon.
	TReal r_plane_tolerance			// Tolerance for plane test.
)
//
// Returns 'true' if the all the points of a polygon are on the opposite side of the plane from
// the camera.
//
// Notes:
//		The camera is assumed to be at the origin. This is step 3 of the ambiguity resolution.
//
// To do:
//		This routine is inefficient and can be optimized.
//
//**********************************
{
	int esf_side = esfSides(pl, prpoly, 0, r_plane_tolerance);
	if (esf_side == esfINTERSECT)
		return false;
	if (esf_side == esfON)
		return true;
	return esf_side != iPlaneDirection(pl);
}

//**********************************************************************************************
//
inline bool bAllSameSide
(
	const CPlane&         pl,		// Plane to test against.
	const CRenderPolygon* prpoly,	// Pointer to the render polygon.
	TReal r_plane_tolerance			// Tolerance for plane test.
)
//
// Returns 'true' if the all the points of a polygon are on the same side of the plane as the
// camera.
//
// Notes:
//		The camera is assumed to be at the origin. This is step 4 of the ambiguity resolution.
//
// To do:
//		This routine is inefficient and can be optimized.
//
//**********************************
{
	return esfSides(pl, prpoly, iPlaneDirection(pl), r_plane_tolerance) != esfINTERSECT;
}

//*********************************************************************************************
//
inline bool bRequireGouraudShading(const CRenderPolygon& rp)
//
// Returns 'true' if the polygon has to use gouraud shading, otherwise returns 'false.'
//
//**************************************
{
	// Compare all vertex shading values with the first one.
	for (uint u = 1; u < rp.paprvPolyVertices.uLen; u++)
	{
		if (iPosFloatCast(rp.paprvPolyVertices[u]->cvIntensity) != iPosFloatCast(rp.paprvPolyVertices[0]->cvIntensity))
			return true;
	}

	return false;
}


//
// Class definitions.
//

//**********************************************************************************************
// 
// CScreenRender implementation.
//

//**********************************************************************************************
//
struct CVertExtent
//
// Class used for finding vertex extents for mipmapping.
//
// Prefix: ve
//
//**********************************
{
public:
	float	f_value;
	int		i_index;

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Set(float f_v, int i)
	//
	// Sets the internal values.
	//
	//**********************************
	{
		f_value = f_v;
		i_index = i;
	}
};


//
// Class implementations.
//

//**********************************************************************************************
// 
// SRenderVertex implementation.
//

	//******************************************************************************************
	SRenderVertex::SRenderVertex(const SRenderVertex& rv_0, const SRenderVertex& rv_1, TReal r_t,
		bool b_perspective)
	{
		// The intersection must occur along the line segment.
		Assert(bWithin(r_t, 0.0, 1.0));

		const SRenderVertex* prv_0 = &rv_0;
		const SRenderVertex* prv_1 = &rv_1;

		//
		// Keep the direction uniform. This is to prevent two adjacent polygons that are both
		// clipped from generating slightly different vertices. The apparently strange procedure
		// of adding 1 to r_t and immediately subtracting it again is to normalize inaccuracies.
		// If the r_t is very small, adding 1 will normalize the range, e.g.:
		//
		//		Suppose:
		//
		//			float f = 0.000000001;
		//
		//			f == 0.000000001 is true, but if we do this:
		//
		//			f += 1.0f;
		//			f -= 1.0f;
		//
		//			then f == 0.000000001 is not true, f == 0.0.
		//
		// To do:
		//		Remove this code if cracking persists, make it more efficient if it does not.
		//

	#if VER_ASM
		__asm
		{
			fld1
			fadd r_t
			fld1
			fsub
			fstp r_t
		}
	#else
		r_t += 1.0;
		r_t -= 1.0;
	#endif VER_ASM

		if (prv_0->v3Cam.tX < prv_1->v3Cam.tX)
		{
			r_t = 1.0 - r_t;
			prv_0 = &rv_1;
			prv_1 = &rv_0;
		}

		TReal r_s = 1.0 - r_t;

		v3Cam = CVector3<>
		(
			r_s * prv_0->v3Cam.tX  +  r_t * prv_1->v3Cam.tX,
			r_s * prv_0->v3Cam.tY  +  r_t * prv_1->v3Cam.tY,
			r_s * prv_0->v3Cam.tZ  +  r_t * prv_1->v3Cam.tZ
		);

		tcTex = CTexCoord
		(
			r_s * prv_0->tcTex.tX  +  r_t * prv_1->tcTex.tX,
			r_s * prv_0->tcTex.tY  +  r_t * prv_1->tcTex.tY
		);

		//
		// Calculate 2D interpolation parameter as well, for screen coords and shading value.
		//

		TReal r_t2D;

		if (v3Cam.tY <= 0)
			// This vertex will surely be rejected later.
			r_t2D = 0.0;
		else if (prv_0->v3Cam.tY <= 0)
			r_t2D = 1.0;
		else if (prv_1->v3Cam.tY <= 0)
			r_t2D = 0.0;
		else
		{
			r_t2D = r_t * prv_1->v3Cam.tY / v3Cam.tY;
			Assert(bWithin(r_t2D, 0.0, 1.0));
		}
		TReal r_s2D = 1.0f - r_t2D;

		if (b_perspective)
		{
			if (prv_0->v3Screen.tZ && prv_1->v3Screen.tZ)
			{
				// Vertices have been projected, so interpolate screen coordinates.
				v3Screen = CVector3<>
				(
					r_s2D * prv_0->v3Screen.tX  +  r_t2D * prv_1->v3Screen.tX,
					r_s2D * prv_0->v3Screen.tY  +  r_t2D * prv_1->v3Screen.tY,
					r_s2D * prv_0->v3Screen.tZ  +  r_t2D * prv_1->v3Screen.tZ
				);
			}
		}
		else
		{
			// Interpolate the screen coords linearly as well, without projection.
			v3Screen = CVector3<>
			(
				r_s * prv_0->v3Screen.tX  +  r_t * prv_1->v3Screen.tX,
				r_s * prv_0->v3Screen.tY  +  r_t * prv_1->v3Screen.tY,
				r_s * prv_0->v3Screen.tZ  +  r_t * prv_1->v3Screen.tZ
			);
		}

		// Use 2D interpolation for shading as well, because it's not rendered perspective correct.
		cvIntensity = r_s2D * prv_0->cvIntensity  +  r_t2D * prv_1->cvIntensity;
	}


//**********************************************************************************************
// 
// CRenderPolygon implementation.
//

	//******************************************************************************************
	CRenderPolygon::CRenderPolygon()
		: bMovedInList(false), iMipLevel(0),
		  cvFace(0.0f)
	{
#if (VER_DEBUG)
		bClippedByDepthSort = false;
		iClipped = 0;
#endif
	}

	//******************************************************************************************
	void CRenderPolygon::SetPlane()
	{
		// Choose points for the plane evenly distributed around the polygon.
		plPlane.FastMake
		(
			paprvPolyVertices[0]->v3Cam,
			paprvPolyVertices[1]->v3Cam,
			paprvPolyVertices[(paprvPolyVertices.uLen >> 1) + 1]->v3Cam
		);
		ptsTolerances.SetTolerance(this);
	}

	//******************************************************************************************
	float CRenderPolygon::fGetFarthestZ() const
	{
		float f_z = 0.0f;

		// Sum the z values of the vertices.
		for (int i = 0; i < paprvPolyVertices.size(); ++i)
			if (paprvPolyVertices[i]->v3Cam.tY > f_z)
			 f_z = paprvPolyVertices[i]->v3Cam.tY;

		// Return the average value.
		return f_z;
	}

	//******************************************************************************************
	bool CRenderPolygon::bCalculateScreenExtents(bool b_cull_tiny)
	{
		Assert(paprvPolyVertices);
		Assert(paprvPolyVertices[0]);

#if (VER_DEBUG)
		bClippedByDepthSort = false;
#endif

		//
		// Build the bounding box in screen space.
		//
		float f_min_x, f_max_x;
		float f_min_y, f_max_y;
		float f_min_z, f_max_z;

		// Set initial min and max values for the screen.
		f_min_x = f_max_x = paprvPolyVertices[0]->v3Screen.tX;
		f_min_y = f_max_y = paprvPolyVertices[0]->v3Screen.tY;
		f_min_z = f_max_z = paprvPolyVertices[0]->v3Cam.tY;
		Assert(f_min_x >= 0.0f);
		Assert(f_min_y >= 0.0f);
		Assert(f_min_z >= 0.0f);

		//
		// Iterate through remaining vertices setting min and max values.
		//
		for (int i_vert = 1; i_vert < paprvPolyVertices.uLen; i_vert++)
		{
			Assert(paprvPolyVertices[i_vert]);

			// Set x range.
			{
				// Get the screen value.
				float f_x = paprvPolyVertices[i_vert]->v3Screen.tX;
				Assert(f_x >= 0.0f);

				// Set the minimum and maximum x values.
				if (u4FromFloat(f_x) < u4FromFloat(f_min_x))
					f_min_x = f_x;
				if (u4FromFloat(f_x) > u4FromFloat(f_max_x))
					f_max_x = f_x;
			}

			// Set y range.
			{
				// Get the screen value.
				float f_y = paprvPolyVertices[i_vert]->v3Screen.tY;
				Assert(f_y >= 0.0f);

				// Set the minimum and maximum x values.
				if (u4FromFloat(f_y) < u4FromFloat(f_min_y))
					f_min_y = f_y;
				if (u4FromFloat(f_y) > u4FromFloat(f_max_y))
					f_max_y = f_y;
			}

			// Set z range.
			{
				// Get the screen value.
				float f_z = paprvPolyVertices[i_vert]->v3Cam.tY;
				Assert(f_z >= 0.0f);

				// Set the minimum and maximum x values.
				if (u4FromFloat(f_z) < u4FromFloat(f_min_z))
					f_min_z = f_z;
				if (u4FromFloat(f_z) > u4FromFloat(f_max_z))
					f_max_z = f_z;
			}
		}

		if (b_cull_tiny && ehwHardwareFlags != ehwTerrain)
			if ((iPosFloatCast(f_max_x) == iPosFloatCast(f_min_x)) ||
			    (iPosFloatCast(f_max_y) == iPosFloatCast(f_min_y)))
				return false;

		f_min_x += ptsTolerances.fPixelBuffer;
		f_min_y += ptsTolerances.fPixelBuffer;
		if (u4FromFloat(f_max_x) >= u4FromFloat(ptsTolerances.fPixelBuffer))
			f_max_x -= ptsTolerances.fPixelBuffer;
		if (u4FromFloat(f_max_y) >= u4FromFloat(ptsTolerances.fPixelBuffer))
			f_max_y -= ptsTolerances.fPixelBuffer;
		
		// Copy the minimum and maximum values to the class structure.
		v2ScreenMinInt.tX = Fist(f_min_x);
		v2ScreenMinInt.tY = Fist(f_min_y);
		v2ScreenMaxInt.tX = Fist(f_max_x);
		v2ScreenMaxInt.tY = Fist(f_max_y);

		fMinZ = f_min_z;
		fMaxZ = rMaxLevel = f_max_z;
		bMovedInList = false;

		return true;
		//return (v2ScreenMaxInt.tX == v2ScreenMinInt.tX) || (v2ScreenMaxInt.tY == v2ScreenMinInt.tY);
	}

	//******************************************************************************************
	bool CRenderPolygon::bBehind(const CRenderPolygon* prpoly) const
	{
		// This polygon is 'P,' prpoly is 'Q.'

		#if bDEMO_BUILD
			//
			// Hack! If P is a terrain polygon, and Q is not, bypass this test.
			//
			if (seterfFace[erfSOURCE_TERRAIN] && !prpoly->seterfFace[erfSOURCE_TERRAIN])
				return true;
		#endif // bDEMO_BUILD

		// Use P's tolerance.

		// Step 4: Is Q entirely on the same side of P's plane from the viewpoint?
		if (bAllSameSide(plPlane, prpoly, rPlaneTolerance))
			return true;

		// Step 3: Is P entirely on the opposite side of Q's plane from the viewpoint?
		if (bAllOppositeSide(prpoly->plPlane, this, rPlaneTolerance))
			return true;

		return false;
	}
	
	//******************************************************************************************
	bool CRenderPolygon::bInFrontOf(const CRenderPolygon* prpoly) const
	{
		// This polygon is 'P,' prpoly is 'Q.'

		#if bDEMO_BUILD
			//
			// Hack! If P is a terrain polygon, and Q is not, bypass this test.
			//
			if (prpoly->seterfFace[erfSOURCE_TERRAIN] && !seterfFace[erfSOURCE_TERRAIN])
				return true;
		#endif // bDEMO_BUILD

		// Use P's tolerance.
		
#if (BROKEN_DEPTH_SORT)

		// Used when the depth sort can't handle reversing polygons in complex
		// situations.

		// Step 3': Is Q entirely on the opposite side of P's plane from the viewpoint?
		if (bAllOppositeSide(plPlane, prpoly, rPlaneTolerance))
		{
			// Step 4': Is P entirely on the same side of Q's plane from the viewpoint?
			if (bAllSameSide(prpoly->plPlane, this, rPlaneTolerance))
				return true;
		}
	
#else

		// Step 3': Is Q entirely on the opposite side of P's plane from the viewpoint?
		if (bAllOppositeSide(plPlane, prpoly, rPlaneTolerance))
			return true;

		// Step 4': Is P entirely on the same side of Q's plane from the viewpoint?
		if (bAllSameSide(prpoly->plPlane, this, rPlaneTolerance))
			return true;

#endif
		
		return false;
	}

	//******************************************************************************************
	void CRenderPolygon::ReduceFeatures()
	{
		bAccept = false;
		bool b_hardware = bFullHardware || ehwHardwareFlags != ehwSoftware;

		// Cull based on screen extents.
		if (!bCalculateScreenExtents(!b_hardware))
			return;

		// Compute screen area.
		SetArea();

		// Don't feature reduce or cull hardware polygons.
		if (b_hardware)
		{
			bAccept = true;
			return;
		}

		// Cull if the polygons is zero or negative area.
		if (fArea <= 0.0f)
			return;

		// Accept the polygon.
		bAccept = true;

		// Do not do feature reduction if the polygon is water.
		if (seterfFace[erfSOURCE_WATER])
			return;

		Assert(fArea > 0.0f);

		// Turn off texture if possible.
		if (seterfFace[erfTEXTURE])
		{
			// Do not kill texture for terrain or for bump maps.
			if (!seterfFace[erfSOURCE_TERRAIN] && !seterfFace[erfBUMP] &&
				u4FromFloat(fArea) <= u4FromFloat(fTextureThresholdArea))
			{
				seterfFace[erfTEXTURE][erfBUMP][erfPERSPECTIVE] = 0;

				// Do not render the polygon if it is transparent.
				if (seterfFace[erfTRANSPARENT])
				{
					bAccept = false;
					return;
				}
				srd3dRenderer.SetD3DFlagForPolygon(*this);
			}
			else
			{
				if (seterfFace[erfPERSPECTIVE])
				{
					// Turn off perspective correction if possible.
					if (u4FromFloat(fArea) <= u4FromFloat(fPerspectiveThresholdArea))
					{
						seterfFace[erfPERSPECTIVE] = 0;
					}
					else
					{
						TReal r_len = Max
						(
							v2ScreenMaxInt.tX - v2ScreenMinInt.tX, 
							v2ScreenMaxInt.tY - v2ScreenMinInt.tY
						);
						if (u4FromFloat(r_len) <= u4FromFloat(fPerspectiveThresholdLen))
						{
							seterfFace[erfPERSPECTIVE] = 0;
						}
						else
						{
							//
							// Pixel error is (zmax/zmin - 1)/4 * scr-length.
							// So, perspective correct if (zmax/zmin - 1)/4 * scr-length >= ERR.
							// (zmax - zmin) / (4 * zmin) * scr-length >= ERR.
							// (zmax - zmin) * scr-length >= ERR * 4 * zmin.
							//
							TReal r_err;

							if (seterfFace[erfSOURCE_TERRAIN])
								r_err = fAltPerspectivePixelError * 4.0 * fMinZ;
							else
								r_err = fPerspectivePixelError * 4.0 * fMinZ;
								
							if ((fMaxZ - fMinZ) * r_len < r_err)
							{
								seterfFace[erfPERSPECTIVE] = 0;
							}
						}
					}
				}
			}
		}

		// Remove Gouraud shading if possible.
		if (seterfFace[erfLIGHT_SHADE])
		{
			if (u4FromFloat(fArea) <= u4FromFloat(fGourShadeThresholdArea) || !bRequireGouraudShading(*this))
			{
				cvFace = paprvPolyVertices[0]->cvIntensity;
				seterfFace[erfLIGHT_SHADE] = 0;
			}
		}
	}
	
	//**********************************************************************************************
	void CRenderPolygon::FindBestSlope(int& ri_best_vert_a, int& ri_best_vert_b) const
	{
		bool  b_found_first = false;	// Flag to find first slope.
		float f_best_slope;				// Best (smallest) slope found.

		//
		// Iterate through all combinations of vertices to find the line with the smallest
		// slope.
		//
		for (int i_vert_0 = 0; i_vert_0 < paprvPolyVertices.uLen - 2; i_vert_0++)
		{
			for (int i_vert_1 = i_vert_0 + 2; i_vert_1 < paprvPolyVertices.uLen; i_vert_1++)
			{
				// Make sure that vertex 0 and the last vertex are not on the same edge.
				if (i_vert_0 == 0 && i_vert_1 == paprvPolyVertices.uLen - 1)
					continue;

				// Get the slope of the subdividing line.
				float f_new_slope = fGetSlope
				(
					paprvPolyVertices[i_vert_0]->v3Screen,
					paprvPolyVertices[i_vert_1]->v3Screen
				);

				// Record the best subdivision.
				if (!b_found_first)
				{
					b_found_first  = true;
					ri_best_vert_a = i_vert_0;
					ri_best_vert_b = i_vert_1;
					f_best_slope   = f_new_slope;
				}
				else
				{
					if (f_new_slope < f_best_slope)
					{
						ri_best_vert_a = i_vert_0;
						ri_best_vert_b = i_vert_1;
						f_best_slope   = f_new_slope;
					}
				}
			}
		}
		if (ri_best_vert_a > ri_best_vert_b)
			Swap(ri_best_vert_a, ri_best_vert_b);
	}

	//**********************************************************************************************
	void CRenderPolygon::BuildVertexArray(CRenderPolygon& rpoly_new, int i_from, int i_to,
		CPArray<SRenderVertex*> paprv_scratch) const
	{
		int i_len = i_to + 1 - i_from;
		if (i_len < 0)
			i_len += paprvPolyVertices.uLen;
		rpoly_new.paprvPolyVertices.uLen = i_len;

		// Use passed temporary array.
		rpoly_new.paprvPolyVertices.atArray = paprv_scratch;
		Assert(paprv_scratch.uLen >= i_len);

		// Iterate from the first vertex to the final.

		int i_vert_master = i_from;				// Vertex index for the master polygon.
		for (int i_vert_new = 0; i_vert_new < rpoly_new.paprvPolyVertices.uLen; i_vert_new++)
		{
			rpoly_new.paprvPolyVertices[i_vert_new] = paprvPolyVertices[i_vert_master++];

			// Wrap.
			if (i_vert_master == paprvPolyVertices.uLen)
				i_vert_master = 0;
		}
	}
	
	//******************************************************************************************
	void CRenderPolygon::SetMipLevel(bool b_target_hardware)
	{
		// Do nothing if the polygon is to be rejected.
		if (!bAccept)
		{
			// Why am I calling a function for every rejected polygon?
			iMipLevel = 0;
			return;
		}

		// Do nothing if there is no texture or only one mip level.
		if (!ptexTexture || ptexTexture->iGetNumMipLevels() <= 1)
		{
			iMipLevel = 0;
			return;
		}

	#if bUSE_SCREEN_AREA
		// Use the smallest mip level if possible.
		if (u4FromFloat(fArea) <= u4FromFloat(fUseSmallestMipArea))
		{
			iMipLevel = ptexTexture->iGetNumMipLevels() - 1;
			return;
		}
	#endif // bUSE_SCREEN_AREA

		//
		// Compute texture area of this polygon.
		//
		float fTexArea = 0.0f;

		// Break the polygon into triangles and sum their area.
		for (uint u_3 = 2; u_3 < paprvPolyVertices.uLen; ++u_3)
		{
			uint u_1 = 0;
			uint u_2 = u_3 - 1;

			//
			// Formula:
			//
			//		Area = (u2 - u1) * (v3 - v1) - (u3 - u1) * (v2 - v1)
			//             ---------------------------------------------
			//                                   2
			//
			// Note the reverse order of the screen vertices. Also note that the CVector2D cross
			// product could be used here.
			//
			float f_sub_area = (paprvPolyVertices[u_2]->tcTex.tX - paprvPolyVertices[u_3]->tcTex.tX) *
				               (paprvPolyVertices[u_1]->tcTex.tY - paprvPolyVertices[u_3]->tcTex.tY) -
					           (paprvPolyVertices[u_1]->tcTex.tX - paprvPolyVertices[u_3]->tcTex.tX) *
							   (paprvPolyVertices[u_2]->tcTex.tY - paprvPolyVertices[u_3]->tcTex.tY);

			// Sum the areas.
			fTexArea += fabs(f_sub_area);
		}

		// Divide by two.
		fTexArea *= 0.5f;

		// Modify the target hardware flag to determine if polygon is to be rendered using hardware.
		b_target_hardware = b_target_hardware && (bFullHardware || ehwHardwareFlags != ehwSoftware);

		// Set the mip level.
		int i_desired_mip = ptexTexture->iSelectMipLevel(fArea, fTexArea);

		//
		// Nothing further is required if the polygons uses hardware and the mip level is present for
		// hardware.
		//
		if (b_target_hardware)
		{
			if (bAvailableD3DRaster((CTexture*)ptexTexture.ptGet(), i_desired_mip))
			{
				iMipLevel = i_desired_mip;
				return;
			}
		}

		// Prevent large mip levels from being loaded if the hardware is slow at creating textures.
		if (b_target_hardware)
		{
			for (;;)
			{
				if (iMaxDim(ptexTexture->prasGetTexture(i_desired_mip)) <= d3dDriver.iGetRecommendedMaxDim())
					break;
				if (i_desired_mip >= ptexTexture->iGetNumMipLevels() - 1)
					break;
				++i_desired_mip;
			}
			if (bAvailableD3DRaster((CTexture*)ptexTexture.ptGet(), i_desired_mip))
			{
				iMipLevel = i_desired_mip;
				return;
			}
		}

#if (0)
		// If the texture manager has all the textures loaded then there is no need to substitute anothe level or
		// request a load.
		if (gtxmTexMan.bAllPresent())
		{
			iMipLevel = i_desired_mip;
			return;
		}
#endif

		//
		// We are paging textures and therefore need to ensure that the texture we want to use exists.
		//
		if (gtxmTexMan.bIsAvailable(ptexTexture->prasGetTexture(i_desired_mip)->pSurface, 
			ptexTexture->prasGetTexture(i_desired_mip)->iByteSpan()) )
		{
			iMipLevel = i_desired_mip;
			return;
		}
		else
		{
			gtxmTexMan.RequestMemory(ptexTexture->prasGetTexture(i_desired_mip)->pSurface, 
				ptexTexture->prasGetTexture(i_desired_mip)->iByteSpan());
		}

		// The following test is safe because the lowest mip is always available.  
		// We will never add one past the end of the array because the previous test will cause a return.
		if (gtxmTexMan.bIsAvailable(ptexTexture->prasGetTexture(i_desired_mip + 1)->pSurface, 
			ptexTexture->prasGetTexture(i_desired_mip + 1)->iByteSpan()) )
		{
			// Use the next smaller mip.
			iMipLevel = i_desired_mip + 1;
		}
		else if (i_desired_mip > 0 && gtxmTexMan.bIsAvailable( ptexTexture->prasGetTexture(i_desired_mip - 1)->pSurface, 
			ptexTexture->prasGetTexture(i_desired_mip - 1)->iByteSpan()) )
		{
			// Use the next bigger mip.
			iMipLevel = i_desired_mip - 1;
		}
		else
		{
			// The last mip to check for validity.  Sub 2 or 3 instead of 1 to skip the lowest 1 or 2 levels..
			int i_smallest_mip = ptexTexture->iGetNumMipLevels() - 1; 

			// Already tried the next smaller, so skip it.
			++i_desired_mip;

			// As long as there's something we haven't tried....
			while (i_desired_mip <= i_smallest_mip)
			{	
				// Try each mip.
				if (gtxmTexMan.bIsAvailable(ptexTexture->prasGetTexture(i_desired_mip)->pSurface, 
					ptexTexture->prasGetTexture(i_desired_mip)->iByteSpan()) )
				{
					iMipLevel = i_desired_mip;
					return;
				}

				++i_desired_mip;
			}

			// Didn't find anything.  Use smallest.
			iMipLevel = i_smallest_mip;
			AlwaysAssert(0);
		}
	}

	//******************************************************************************************
	void CRenderPolygon::Validate()
	{
		// Verify that the screen co-ordinates are in range.
		for (int i = 0; i < paprvPolyVertices.uLen; i++)
		{
			float tX = paprvPolyVertices[i]->v3Screen.tX;
			float tY = paprvPolyVertices[i]->v3Screen.tY;

			AlwaysAssert(tX < 2048.0f && tX >= 0.0f);
			AlwaysAssert(tY < 2048.0f && tY >= 0.0f);
		}

		// Verify that the texture co-ordinates are in range.
		if (seterfFace[erfTEXTURE])
		{
			rptr<CRaster> pras_texture = ptexTexture->prasGetTexture(iMipLevel);

			if (pras_texture->bNotTileable)
			{
				for (int i = 0; i < paprvPolyVertices.uLen; i++)
				{
					float tX = paprvPolyVertices[i]->tcTex.tX * pras_texture->fWidth + fTexEdgeTolerance;
					float tY = paprvPolyVertices[i]->tcTex.tY * pras_texture->fHeight + fTexEdgeTolerance;

					AlwaysAssert(tX < 32767.0f && tX > -32767.0f);
					AlwaysAssert(tY < 32767.0f && tY > -32767.0f);
				}
			}
			else
			{
				for (int i = 0; i < paprvPolyVertices.uLen; i++)
				{
					float tX = paprvPolyVertices[i]->tcTex.tX * pras_texture->fWidth;
					float tY = paprvPolyVertices[i]->tcTex.tY * pras_texture->fHeight;

					AlwaysAssert(tX < 32767.0f && tX > -32767.0f);
					AlwaysAssert(tY < 32767.0f && tY > -32767.0f);
				}
			}
		}

		// Verify that the shading values are in range.
		if (seterfFace[erfLIGHT_SHADE])
		{
			for (int i = 0; i < paprvPolyVertices.uLen; i++)
			{
				AlwaysAssert(paprvPolyVertices[i]->cvIntensity >= 0.05 && 
							 paprvPolyVertices[i]->cvIntensity <= ptexTexture->ppcePalClut->pclutClut->iNumRampValues-0.05);
			}
		}
	}

	//******************************************************************************************
	void CRenderPolygon::Grow(float f_amount)
	{
	}


//**********************************************************************************************
// 
// CScreenRender implementation.
//

	//******************************************************************************************
	// 
	// SSettings implementation.
	//

		//**************************************************************************************
		CScreenRender::SSettings::SSettings()
			: 
			bClearBackground(true),
			bDrawSky(true),
			//clrBackground(clrDefEndDepth),	// Default BG colour.
			clrBackground(clrInitColour),		// Default BG colour.
			bTestGamma(false),
			bHalfScanlines(false),
			bDoubleVertical(false),
			bSoftwareAlpha(false)
		{
			seterfState = Set
				 (erfZ_BUFFER)
				+ erfLIGHT
				+ erfLIGHT_SHADE
				+ erfFOG
				+ erfFOG_SHADE
				+ erfSPECULAR
				+ erfCOPY
				+ erfTEXTURE
				+ erfTRANSPARENT
				+ erfBUMP
				+ erfSUBPIXEL
				+ erfPERSPECTIVE
				+ erfTRAPEZOIDS
				+ erfSOURCE_TERRAIN
				+ erfSOURCE_WATER
				+ erfOCCLUDE
				+ erfDRAW_CLIP
				+ erfMIPMAP
				+ erfALPHA_COLOUR
				+ erfD3D_CACHE
			#if (BILINEAR_FILTER)
				+ erfFILTER
			#endif
				;
		}

	//******************************************************************************************
	CScreenRender::CScreenRender(SSettings* pscrenset, rptr<CRaster> pras_screen, rptr<CRaster> pras_zbuffer)
		: prasScreen(pras_screen.ptGet()), pSettings(pscrenset)
	{
		Assert(pscrenset);
	}

	//******************************************************************************************
	void CScreenRender::CorrectRenderState(CSet<ERenderFeature>& seterf)
	{
		// Enforce a few dependencies.
		if (seterf[erfLIGHT_SHADE])
			seterf[erfLIGHT] = 1;
		if (seterf[erfFOG_SHADE])
			seterf[erfFOG] = 1;

		// Ensure nothing was changed that cannot be.
		seterf = (seterf & seterfModify()) + (seterfDefault() - seterfModify());
	}

	//******************************************************************************************
	void CScreenRender::DrawPolygons(CPArray<CRenderPolygon*> paprpoly)
	{
		// Default version calls DrawPolygon.
		for (uint u_poly = 0; u_poly < paprpoly.uLen; u_poly++)
		{
#if VER_DEBUG
			ValidatePolygon(*paprpoly[u_poly]);
#endif
			DrawPolygon(*paprpoly[u_poly]);
		}
	}

	//******************************************************************************************
	void CScreenRender::SetD3DFlagForPolygons(CPArray<CRenderPolygon> parpoly, bool b_d3d)
	{
		//
		// Set D3D flags.
		//
		if (d3dDriver.bUseD3D() && b_d3d)
		{
			srd3dRenderer.SetD3DFlagForPolygons(parpoly);
			return;
		}

		//
		// Set flag default.
		//
		for (uint u_poly = 0; u_poly < parpoly.uLen; u_poly++)
		{
			parpoly[u_poly].bFullHardware    = false;;
			parpoly[u_poly].ehwHardwareFlags = ehwSoftware;
		}
	}

	//**********************************************************************************************
	void CScreenRender::ValidatePolygon(const CRenderPolygon& rpoly)
	{
		// There must be a texture pointer; if we are mapping, it must have a raster.
		Assert(rpoly.ptexTexture);

		if (rpoly.seterfFace[erfTEXTURE][erfTRANSPARENT][erfBUMP][erfCOPY])
		{
			Assert(rpoly.ptexTexture->prasGetTexture(rpoly.iMipLevel));

			if (rpoly.seterfFace[erfTRANSPARENT])
				Assert(rpoly.ptexTexture->seterfFeatures[erfTRANSPARENT]);

			if (rpoly.seterfFace[erfCOPY])
			{
				// Make sure it's screen format.
				Assert(rpoly.ptexTexture->prasGetTexture(rpoly.iMipLevel)->iPixelBits == prasScreen->iPixelBits);
			}
			else if (rpoly.seterfFace[erfBUMP])
			{
				Assert(rpoly.ptexTexture->seterfFeatures[erfBUMP]);
			}
		}

		if (rpoly.seterfFace[erfLIGHT])
		{
			Assert(rpoly.ptexTexture && rpoly.ptexTexture->ppcePalClut && rpoly.ptexTexture->ppcePalClut->pclutClut);
			if (!rpoly.seterfFace[erfLIGHT_SHADE][erfBUMP])
				// If not bump-mapping or vertex lighting, we need valid face lighting here.
				Assert(rpoly.cvFace >= 0 && rpoly.cvFace < rpoly.ptexTexture->ppcePalClut->pclutClut->iNumRampValues);
		}

		for (int i = 0; i < rpoly.paprvPolyVertices.uLen; i++)
		{
			ValidateVertex(*rpoly.paprvPolyVertices[i]);
			if (rpoly.seterfFace[erfLIGHT_SHADE])
				// We need valid vertex lighting.
				Assert(rpoly.paprvPolyVertices[i]->cvIntensity >= 0 && 
					rpoly.paprvPolyVertices[i]->cvIntensity < rpoly.ptexTexture->ppcePalClut->pclutClut->iNumRampValues);
		}
	}

	//**********************************************************************************************
	void CScreenRender::ValidateVertex(const SRenderVertex& rv)
	{
		const float fTEXTURE_TOLERANCE = 0.001;

		Assert(bWithin(int(rv.v3Screen.tX), 0, prasScreen->iWidth));
		Assert(bWithin(int(rv.v3Screen.tY), 0, prasScreen->iHeight));

		Assert(rv.v3Screen.tZ > 0.0f);

		// No, we are tiling now!  Do not clamp the nice texture coordinate.
#if VER_CLAMP_UV_TILE
		SetMinMax(NonConst(rv.tcTex.tX), 0, 1);
		SetMinMax(NonConst(rv.tcTex.tY), 0, 1);
#endif
//		Assert(bWithin(rv.tcTex.tX, -fTEXTURE_TOLERANCE, 1.0 + fTEXTURE_TOLERANCE));
//		Assert(bWithin(rv.tcTex.tY, -fTEXTURE_TOLERANCE, 1.0 + fTEXTURE_TOLERANCE));
	}

	//******************************************************************************************
	TPixel CScreenRender::pixSetBackground(TPixel pix_background)
	{
		// Function should never be called.
		//Assert(0);
		return 0;
	}

	//******************************************************************************************
	void CScreenRender::SetHardwareOut(bool b_allow_hardware)
	{
		// Function should never be called.
		//Assert(0);
	}


//**********************************************************************************************
//
// CArrayRenderDesc implementation.
//

	CArrayRenderDesc::CArrayRenderDesc()
	{
		//
		// Add the known render descs.
		// This is the only place in the code that must specifically know of all RenderDescs,
		// and must include the ScreenRender*.hpp files.
		//
		sapRenderDesc << &screndescDWI;
	}


//**********************************************************************************************
//
// CArrayRenderDesc global variable.
//

CArrayRenderDesc sapRenderDesc;

// Static key variable.
uint32 CSortKey::u4UniqueSubKeyUp;
uint32 CSortKey::u4UniqueSubKeyDown;
uint32 CSortKey::u4UniqueSubKeyStep;

float fMaxAreaCull = 0.05f;
float fMaxAreaCullTerrain;
