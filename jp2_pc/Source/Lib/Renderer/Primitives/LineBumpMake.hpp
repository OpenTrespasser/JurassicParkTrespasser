/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Primitive to "draw" interpolated normals onto a pre-existing bumpmap.
 *
 * Bugs:
 *
 * Notes:
 *		This code works in exactly the same fashion as any other of the rendering primitives,
 *		except that the Z-buffer is not implemented.
 *
 * To do:
 *		Add a faster function for rotating one normal by another normal.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/LineBumpMake.hpp                              $
 * 
 * 35    98.03.07 7:40p Mmouni
 * Removed DrawSubTriangle routine.
 * 
 * 34    98/03/04 2:12p Mmouni
 * Made the DrawSubTriangle routine draw an extra pixel in x and y to reduce the number of
 * non-drawn pixels in curved bump-maps.
 * 
 * 33    97/11/08 3:41p Pkeet
 * Removed Z template parameters and code.
 * 
 * 32    97.10.15 7:41p Mmouni
 * Removed support for right to left scanlines.
 * 
 * 31    97/10/12 20:42 Speter
 * Changed rv_face to cv_face.
 * 
 * 30    97/08/07 11:35a Pkeet
 * Added interface support for mipmapping.
 * 
 * 29    97/07/18 3:30p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 28    97/07/16 15:51 Speter
 * InitializeTriangleData now takes two params to use as coefficients in derivative calculation
 * rather than f_invdx. 
 * 
 * 27    97/07/07 14:03 Speter
 * Now copy vertices for current polygon locally to arvRasterVertices, which is used rather than
 * rpoly.paprvPolyVertices for most DrawPolygon code.  Renamed iY to iYScr, as it's now a union
 * with v3Cam, and set in InitializePolygonData.
 * 
 * 26    97/06/27 15:29 Speter
 * Changed CGouraudOff to new CGouraudNone where appropriate.  Added bIsPlanar() function.
 * 
 * 25    6/16/97 8:36p Mlange
 * Updated for fixed::fxFromFloat() name change.
 * 
 * 24    97/05/25 18:05 Speter
 * Changed CDrawTriangle to CDrawPolygon.
 * 
 * 23    97-05-06 15:53 Speter
 * Now store bump curves using optimised axis-swap bump transform  
 * 
 * 22    97-03-31 22:25 Speter
 * Now use CEdge<> line variables rather than slower pointers.
 * Due to this, copied necessary code from parent class Initialize() and InitializeAsBaseEdge();
 * can't call parent's versions, because they have a different idea of what CEdge<LINE> looks
 * like.
 * Changed CZBufferInv parameter to CZBufferOff.
 * 
 * 21    97/03/18 19:22 Speter
 * Replaced inefficient CDir3<>('z') with d3ZAxis.
 * 
 * 20    97/02/20 8:15p Pkeet
 * Fixed bug found on an assert fail when loaded a curved bumpmapped object.
 * 
 * 19    97/02/20 8:00p Pkeet
 * Added a specialized version of 'DrawSubtriangle.'
 * 
 * 18    97/02/07 19:11 Speter
 * Replaced r3ObjToTexture with faster, better mx3ObjToTexture, propagated change.
 * 
 * 17    1/29/97 12:17p Pkeet
 * Added code for using object space for bumpmap normals.
 * 
 * 16    1/15/97 1:05p Pkeet
 * Removed the fog template parameter.
 * 
 * 15    97/01/07 12:05 Speter
 * Updated for ptr_const.
 * 
 * 14    96/12/31 17:06 Speter
 * Changed template parameter from raster to pixel type.
 * Updated for rptr.
 * 
 * 13    12/16/96 7:52p Pkeet
 * Fixed bug.
 * 
 * 12    12/16/96 6:43p Pkeet
 * Quick bumpmap fix.
 * 
 * 11    12/06/96 5:14p Pkeet
 * Commented out seperate left and right drawing routines.
 * 
 * 10    11/12/96 2:46p Pkeet
 * Added support for initializing self-modifying code on a per-triangle basis.
 * 
 * 9     96/11/11 18:53 Speter
 * Updated for new CBumpAnglePair.SetBump() function.
 * 
 * 8     10/14/96 11:46a Pkeet
 * Added a template parameter for the CMap template parameter.
 * 
 * 7     10/09/96 7:32p Pkeet
 * Changed class to use the new 'CScanline' base class.
 * 
 * 6     10/04/96 3:51p Pkeet
 * Added the CZBuffer class as a template parameter to CScanline.
 * 
 * 5     10/03/96 6:18p Pkeet
 * Added the 'pdpixGetClut' member function.
 * 
 * 4     10/02/96 4:40p Pkeet
 * Added the 'INDEX' template parameter. Added full template parameter list to 'CScanline.'
 * 
 * 3     9/27/96 4:32p Pkeet
 * Removed the 'LineGouraudZ' template class.
 * 
 * 2     96/09/27 11:31 Speter
 * Oops.  Changed <float> in geometry types to <>.
 * 
 * 1     9/18/96 2:45p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_LINEBUMPMAKE_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_LINEBUMPMAKE_HPP

// Make sure this file is being included after 'FastBump.hpp.'
#ifndef HEADER_LIB_RENDERER_PRIMITIVES_FASTBUMP_HPP
	#error The header file FastBump.hpp has not been included!
#endif

#include "Lib/Renderer/Primitives/DrawTriangle.hpp"
#include "Lib/Renderer/Primitives/Scanline.hpp"


//
// Static global variables.
//

CVector3<> vec3DeltaX;		// Vector increment for one step along the horizontal axis.


//*****************************************************************************************
//
template<class PIXELFORMAT> void DrawLoopBumpMake
(
	int          i_pixel,		// Offset into the destination raster.
	PIXELFORMAT* ptpix_screen,	// Address of the first screen surface pixel.
	CVector3<>   vec3			// Starting vector for interpolation.
)
//
// Scanline draw function draws left to right or right to left depending on
// i4ScanlineDirection. Version specific to bumpmap angle pairs.
//
// Notes:
//		This function is the inner scanline loop for the raster routine.
//
//**************************************
{

	//
	// Iterate left or right along the destination scanline depending on the sign of
	// 'i4ScanlineDirection.'
	//
	do
	{
		//
		// Do nothing if curvature already added. This allows multiple triangles to modify
		// an existing bumpmap without creating artifacts.
		//
		if (!ptpix_screen[i_pixel].bGetCurveFlag())
		{
			//
			// Get normals for the interpolated curve and the existing bumpmap normal.
			//
			CDir3<> dir3_curve(vec3);
			CDir3<> dir3_bump = ptpix_screen[i_pixel].dir3MakeNormal();

			// Rotate the bumpmap normal by the interpolated curve's normal.
			dir3_bump = dir3_bump * CRotate3<>(d3ZAxis, dir3_curve);

			// Reverse the transformation.
			dir3_bump    *= CBumpMap::mx3Reverse;

			// Store the combined curvature and bump.
			ptpix_screen[i_pixel].SetBump(dir3_bump);

			// Mark this pixel as already having curvature.
			ptpix_screen[i_pixel].SetCurveFlag();
		}

		// Increment vector and scanline count.
		vec3    += vec3DeltaX;
		i_pixel++;
	}
	while (i_pixel != 0);
}

#endif	// ifndef HEADER_LIB_RENDERER_PRIMITIVES_LINEBUMPMAKE_HPP
