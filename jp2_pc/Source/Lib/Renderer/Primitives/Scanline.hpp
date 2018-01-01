/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Defines CScanline, which is used to render triangles of all formats.
 *
 * Notes:
 *		Currently the 'Z' value is calculated without regard to subpixel positioning on either
 *		the 'x' or 'y' axis.
 *
 *		Virtually any mapping applied to rasterized triangles require subpixel calculations
 *		along x, just as edge walking for triangles require subpixel calculations along y.
 *
 *		Left and right subpixel calculations must be handled differently:
 *		
 *					-----------				    -----------
 *				   |           |			   |           |
 *				   |           |			   |     \     |
 *				   |    /      |			   |      \    |
 *				   |   / *     |			   |     * \   |
 *				   |  /        |			   |        \  |
 *				   | /         |			   |         \ |
 *				   |/          |			   |          \|
 *				   /-----------			        -----------\
 *				      |<->|			                 |<->|
 *				     subpixel			            subpixel
 *				    difference			           difference
 *		             on left.				        on right.
 *
 *		For the left, it can be seen that the value of the subpixel position along x is the
 *		distance from the centre of the pixel to the actual starting position of the scanline,
 *		which is:
 *
 *				x difference = (0.5 + the integer value of x) - the actual value of x; or
 *
 *				x difference = 0.5 - the fraction portion of x.
 *
 *		As an optimization, 0.5 has been added to the x value of scanline starting positions
 *		so a truncation can be performed instead of rounding. Thus, for the left scanline
 *		position:
 *
 *				x difference = 0.5 - the fraction portion of x + 0.5; or
 *
 *				x difference = 1.0 - the fraction portion of x.
 *
 *		For the right, the formula for the starting x difference value is:
 *
 *				x difference = the actual value of x - (the integer portion of x + 0.5); or
 *
 *				x difference = the fractional portion of x - 0.5.
 *
 *		Adding 0.5 for the rounding optimization results in:
 *
 *				x difference = the fractional portion of x - 0.5 + 0.5; or
 *
 *				x difference = the fractional portion of x.
 *
 *		Any value that should be subpixel correct with respect to 'x' (e.g. 'u' and 'v' texture
 *		coordinates for texture mapping), can use the x difference value by multiplying the
 *		step value for mapping with respect to x by the x difference and adding it to the
 *		starting value. For example, for a mapping value 'u':
 *
 *				u start = u + (du * x difference).
 *
 *		The fractional difference in x from one line to the next changes by a constant amount
 *		-- in other words a constant value can be added to the initial subpixel x difference. 
 *		If the x subpixel difference creates an integer value, that integer value is simply
 *		removed and ignored. Maintaining a fractional value only for the x difference is
 *		referred to in this, and in dependent modules, as the modulus subtraction.
 *
 *		Whenever and integer value is subtracted (i.e. the modulus subtraction is performed),
 *		all inherited classes must also perform a modulus subtraction (e.g. du is subtracted
 *		from u for texture mapping).
 *
 * Bugs:
 *
 * To do:
 *		Examine data locality issues for the scanline rasterizing functions.
 *		Incorporate the 'fINV_Z_MULTIPLIER' multiply in the camera class.
 *		Add assembly versions of 'DrawFromLeft' and 'DrawFromRight' for specific rasters.
 *		Test faster float to int conversions for positive and negative values.
 *		Reduce the number of tests to find the modularity of the x difference; e.g. make
 *		the fxXDifference value and the increment for fxXDifference both positive.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/Scanline.hpp                                  $
 * 
 * 39    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 38    8/27/98 9:12p Asouth
 * combinatorical relative operators are a library function
 * 
 * 37    8/26/98 11:08a Rvande
 * Added explicit scoping to the fixed data type
 * 
 * 36    98.03.24 8:17p Mmouni
 * Made changes to support alternate perspective correction settings.
 * 
 * 35    97/11/08 4:47p Pkeet
 * Removed 'DrawLoop.hpp' include.
 * 
 * 34    97/11/08 3:41p Pkeet
 * Removed Z template parameters and code.
 * 
 * 33    97.10.15 7:41p Mmouni
 * Removed support for right to left scanlines.
 * 
 * 32    97/10/12 20:46 Speter
 * Changed TReflectVal to TClutVal.
 * 
 * 31    8/18/97 5:46p Bbell
 * Reorganized the 'SetClutAddress' member function to use the CTexture solid colour.
 * 
 * 30    97/08/15 12:05 Speter
 * Now set solid colour differently depending on whether clut lookup is enabled.
 * 
 * 29    97/08/07 11:35a Pkeet
 * Added interface support for mipmapping.
 * 
 * 28    97/08/05 14:19 Speter
 * Slightly optimised polygon vertex setup by adding InitializeVertexData() functions.
 * 
 * 27    97/07/16 15:51 Speter
 * InitializeTriangleData now takes two params to use as coefficients in derivative calculation
 * rather than f_invdx.  Added AssertRange in scanline template.
 * 
 * 26    97/07/07 14:03 Speter
 * Now copy vertices for current polygon locally to arvRasterVertices, which is used rather than
 * rpoly.paprvPolyVertices for most DrawPolygon code.  Renamed iY to iYScr, as it's now a union
 * with v3Cam, and set in InitializePolygonData.
 * 
 * 25    97/06/27 15:35 Speter
 * Moved code to new InitializePolygonData(), added bIsPlanar(). Removed call to
 * SetBaseIntensity.
 * 
 * 24    6/16/97 8:35p Mlange
 * Updated for fixed::fxFromFloat() name change.
 * 
 * 23    97/05/25 18:05 Speter
 * Changed CDrawTriangle to CDrawPolygon.
 * 
 * 22    97-03-31 22:22 Speter
 * Now use CEdge<> line variables rather than slower pointers.
 * Removed unneeded pdtriTriangle member var.
 * 
 * 21    97/02/21 12:47p Pkeet
 * Changed type names for template parameters to meet coding standards.
 * 
 * 20    97/02/20 7:16p Pkeet
 * The 'Draw' member function has been deleted in favour of a DrawSubtriangle function.
 * 
 * 19    97/02/07 3:20p Pkeet
 * Now uses the constant colour fill.
 * 
 * 18    97/01/26 19:54 Speter
 * Moved scanline timing calls from outside scanline call to outside of sub-triangle loop.
 * 
 * 17    1/15/97 1:01p Pkeet
 * Removed fog and b_modify parameters.
 * 
 * 16    97/01/07 12:05 Speter
 * Updated for ptr_const.
 * 
 * 15    96/12/31 17:06 Speter
 * Changed template parameter from raster to pixel type.
 * Updated for rptr.
 * 
 * 14    12/13/96 12:22p Pkeet
 * Added timer code back in.
 * 
 * 13    12/12/96 12:04p Pkeet
 * Added an extra parameter for calling the CIndex constructor on a per scanline basis.
 * 
 * 12    96/12/09 16:17 Speter
 * Renamed tmScanline stat to psPixels.
 * 
 * 11    96/12/06 17:58 Speter
 * Changed profile to use new CCycleTimer class and Add() function.
 * 
 * 10    12/06/96 5:14p Pkeet
 * Commented out seperate left and right drawing routines.
 * 
 * 9     12/06/96 3:52p Pkeet
 * Disabled the self-modifying code portion.
 * 
 * 8     96/12/05 16:48 Speter
 * Removed #ifdef on profile calls.
 * 
 * 7     12/05/96 1:17p Pkeet
 * Added profiling from the Profile.hpp module.
 * 
 * 6     11/12/96 2:46p Pkeet
 * Added support for initializing self-modifying code on a per-triangle basis.
 * 
 * 5     10/11/96 12:26p Pkeet
 * Made the pointer to the texture bitmap a global variable.
 * 
 * 4     10/11/96 10:31a Pkeet
 * Added 'pvClutConversion' as a global variable instead of a member of 'CDrawTriangle.' No clut
 * pointer is therefore passed to the 'DrawLoop' functions.
 * 
 * 3     10/10/96 7:56p Pkeet
 * Modified the 'DrawLoop' parameter list to explicitly pass variables instead of a pointer to
 * the CScanline object.
 * 
 * 2     10/10/96 3:11p Pkeet
 * Modified 'pdpixGetClutAddress' to fall through if no associated clut is present.
 * 
 * 1     10/09/96 7:17p Pkeet
 * Changed 'CLineFlatZ' to 'CScanline' and moved code from 'LineFlatZ.hpp' to this module.
 * 
 * 45    10/04/96 5:34p Pkeet
 * Moved all ZBuffer code from LineFlatZ.hpp to ZBufferT.hpp.
 * 
 * 44    10/03/96 6:19p Pkeet
 * Added the 'pdpixGetClut' member function.
 * 
 * 43    10/03/96 3:18p Pkeet
 * Added the 'INDEX' template parameter and member variable to the CScanline class. Removed
 * redundant includes found in the 'DrawTriangle.hpp' module.
 * 
 * 42    10/02/96 4:40p Pkeet
 * Added the 'INDEX' template parameter. Added full template parameter list to 'CScanline.'
 * 
 * 41    10/01/96 3:19p Pkeet
 * Moved the 'DrawLoop' function to the DrawLoop module.
 * 
 * 40    9/27/96 4:32p Pkeet
 * Removed the 'LineGouraudZ' template class.
 * 
 * 39    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 38    96/09/25 15:48 Speter
 * Updated for new FogT functions.
 * 
 * 37    96/09/12 16:24 Speter
 * Incorporated new TReflectVal usage.
 * 
 * 36    9/12/96 3:52p Pkeet
 * Removed static global variables associated with bumpmapping and moved them to "BumpMapT.hpp."
 * 
 * 35    9/11/96 11:27a Pkeet
 * Added bumpmapping static global variables.
 * 
 * 34    96/09/09 18:30 Speter
 * Made compatible with change of fIntensity to lvIntensity.
 * 
 * 33    8/22/96 2:08p Pkeet
 * Removed the 'Fog' member variable in favour of null pointer casts.
 * 
 * 32    8/21/96 4:45p Pkeet
 * Added fogging.
 * 
 * 31    8/20/96 4:47p Pkeet
 * Initial implementation of fogging.
 * 
 * 30    8/16/96 1:01p Pkeet
 * Now performs subpixel correction for 'z.' The 'LineXDifference' module has been removed and
 * its functionality put into the 'LineFlatZ' module.
 * 
 * 29    96/08/02 11:04 Speter
 * Swapped SRenderVertex .fInvZ  and .v3Screen.tZ values.
 * 
 * 
 * 28    7/29/96 4:18p Pkeet
 * Added asserts to the DrawFromLeft and DrawFromRight subroutines.
 * 
 * 27    7/26/96 6:41p Mlange
 * Updated for CTexture's tpSolid data member name change.
 * 
 * 26    96/07/26 18:22 Speter
 * Changed scaling of vertex parameters again.  Now all increments are scaled by parameter size,
 * actual values are scaled by parameter size minus one, and 0.5 is added.
 * 
 * 25    96/07/24 14:55 Speter
 * Changed rasterising routines to accept lighting and texture parameters in range [0,1], and
 * scale them to the integer/fixed values needed.
 * 
 * 24    96/07/22 15:28 Speter
 * Moved pixPixel global var here from DrawTriangle.hpp.
 * Light intensity is now taken from first vertex rather than assumed maximum.
 * 
 * 23    7/18/96 5:45p Pkeet
 * Changed asserts to match horizontal range.
 * 
 * 22    96/07/18 17:19 Speter
 * Changes affecting zillions of files:
 * Changed SRenderCoord to SRenderVertex.
 * Added SRenderTriangle type, changed drawing primitives to accept it.
 * 
 * 21    96/07/15 18:59 Speter
 * Changed u4Data global to ptexTexture.
 * Moved pvClutTable to DrawTriangle.hpp.
 * Removed iDeltaScan and pu1Bytes from CTexture.  Now these values are retrieved from
 * CTexture::prasTexture.
 * 
 * 20    7/09/96 3:22p Pkeet
 * Moved 'i4DInvZ' to LineFlatZ.hpp.
 * 
 * 19    7/08/96 3:23p Pkeet
 * Set two parameters for InitializeAsBase member function.
 * 
 * 18    96/07/01 21:34 Speter
 * Updated for CRaster's code review changes.
 * 
 * 17    7/01/96 7:51p Pkeet
 * Added 'fX' and 'fXDifference' member variables.
 * 
 * 16    6/26/96 2:14p Pkeet
 * Added include for 'LineFlatZScanline.hpp' for P5.
 * 
 * 15    6/25/96 2:33p Pkeet
 * Changed member functions to utilize 'u4Data' as a global variable.
 * 
 * 14    6/18/96 10:52a Pkeet
 * Added new fast float to fixed conversions. Updated comments in the draw functions.
 * 
 * 13    6/17/96 6:42p Pkeet
 * Added 'iFastFloatToInt' use.
 * 
 * 12    6/13/96 8:31p Pkeet
 * Eliminated fast float to int conversions for now.
 * 
 * 11    6/13/96 3:09p Pkeet
 * Employed 'iIntFromDouble.' Simplified parameter lists for Initialize function. Made
 * 'DrawFromLeft' and 'DrawFromRight' member functions (again). Added 'DrawLoop' function and an
 * assembly version of this function for 8 and 16 bit rasters.
 * 
 * 10    6/06/96 7:56p Pkeet
 * Made 'DrawFromLeft' and 'DrawFromRight' separate functions from CScanline so that separate
 * template functions could be specified for separate rasters.
 * 
 * 9     6/06/96 4:30p Pkeet
 * Made the 'DrawFromLeft' and 'DrawFromRight' functions member functions. Altered structure to
 * allow use by CDrawTriangle as opposed to CPoly.
 * 
 * 8     6/03/96 5:44p Pkeet
 * Updated comments section.
 * 
 * 7     6/03/96 5:22p Pkeet
 * Changed 'Line' member variable of CDrawTriangle to 'lineData.' Added additional notes on data
 * locality issues.
 * 
 * 6     6/03/96 10:24a Pkeet
 * Added more comments.
 * 
 * 5     5/31/96 8:24p Pkeet
 * Added additional comments and asserts.
 * 
 * 4     5/31/96 4:09p Pkeet
 * Set templates up to function of of raster type (RAS) only.
 * 
 * 3     5/29/96 7:54p Pkeet
 * Fixed tear bug.
 * 
 * 2     5/29/96 4:46p Pkeet
 * Initial implementation.
 * 
 * 1     5/29/96 4:44p Pkeet
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_SCANLINE_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_SCANLINE_HPP

#ifdef __MWERKS__
 // for >= if only given < and ==
 #include <utility>
 using namespace std::rel_ops;
#endif

//
// Class definitions.
//

//*********************************************************************************************
//
template
<
	class TPIX,		// Destination raster pixel type.
	class GOUR,		// Intensity shading format.
	class TRANS,	// Transparency switch.
	class MAP,		// Source map format.
	class INDEX,	// Source index coordinates.
	class CLU		// Colour lookup table switch.
> class CScanline
//
// Defines a scanline for a triangle.
//
// Prefix: line (uses a generic prefix because it is a template class).
//
// Notes:
//		This class is used by CEdge and CDrawPolygon template classes as a type for defining
//		scanline type and raster type.
//
//**************************************
{

public:

	::fixed fxX;				// Position of coordinate along the X axis.
	::fixed fxXDifference;	// Fixed point version of the difference between the first pixel
							// lit along x and the subpixel position of the raster scanline.
							// This value is used for edge walking.
	float fXDifference;		// The floating point version of 'fxXDifference.' This value is
							// used only in initialization.
	GOUR  gourIntensity;	// Intensity template element.
	INDEX indCoord;			// Source texture index coordinates template element.

public:

	//*****************************************************************************************
	//
	// CScanline template access types.
	//

	typedef TPIX  TPixel;	// Destination pixel type.
	typedef GOUR  TGouraud;	// Gouraud shading type.
	typedef TRANS TTrans;	// Transparency type.
	typedef MAP   TMap;		// Mapping type.
	typedef INDEX TIndex;	// Indexing type.
	typedef CLU   TClu;		// Colour lookup type.

public:

	//*****************************************************************************************
	//
	// CScanline member functions.
	//

	// Force default constructor inline so it is not called.
	forceinline CScanline()
	{
	}

	//*****************************************************************************************
	//
	forceinline void Initialize
	(
		CEdge< CScanline<TPIX, GOUR, TRANS, MAP, INDEX, CLU> >* pedge
							// Pointer to the edge requiring start and increment values. 
	)
	//
	// Initializes the X components of the edge, including the starting position and the edge
	// walking increment.
	//
	// Notes:
	//		The subpixel calculation made in CEdge is applied here.
	//
	//**************************************
	{
		Assert(pedge);
		Assert(pedge->prvFrom);
		Assert(pedge->prvTo);

		//
		// Find the x increment value. The value is calculated from:
		//
		//		              dx    (x1 - x0)
		//	   x increment = ---- = ---------
		//		              dy    (y1 - y0)
		//
		// but 1 / (y1 - y0) has already been calculated for the edge.
		//
		float f_increment_x = pedge->fInvDY * (pedge->prvTo->v3Screen.tX - pedge->prvFrom->v3Screen.tX);
		pedge->lineIncrement.fxX.fxFromFloat(f_increment_x);

		//
		// Adjust X to the position where it would intersect the first horizontal line.
		//
		fxX.fxFromPosFloat(pedge->prvFrom->v3Screen.tX + f_increment_x * pedge->fStartYDiff);
	}

	//*****************************************************************************************
	//
	forceinline void InitializeAsBase
	(
		CEdge< CScanline<TPIX, GOUR, TRANS, MAP, INDEX, CLU> >* pedge,
								// Edge requiring start and increment values.
		const CDrawPolygon< CScanline<TPIX, GOUR, TRANS, MAP, INDEX, CLU> >* pdtri
								// Back pointer to the triangle structure.
	)
	//
	// Initializes the Z components of the edge, including starting position and edge walking
	// increment.
	//
	// Notes:
	//		The subpixel calculation made in CEdge is applied here.
	//
	//**************************************
	{
		Assert(pedge);
		Assert(pedge->prvFrom);
		Assert(pedge->prvTo);
		Assert(pdtri);

		bool b_swap_modsub_sign = false;

		//
		// Get the fractional portions of the starting x difference and the increment x
		// difference. See the 'Notes' section of this module for an explanation of this
		// implementation.
		//
		fxXDifference = ::fixed(1) - fxX.fxFractional();
		pedge->lineIncrement.fxXDifference = -pedge->lineIncrement.fxX.fxFractional();

		// Convert x difference and increment to floating point.
		fXDifference = (float)fxXDifference;
		pedge->lineIncrement.fXDifference = (float)pedge->lineIncrement.fxXDifference;

		// Make sure the modulus subtraction values are set to the right sign.
		b_swap_modsub_sign = pedge->lineIncrement.fXDifference >= 0.0f;

		// Use the setup routine for Gouraud-shaded base edges.
		gourIntensity.InitializeAsBase
		(
			pedge->prvFrom,
			pedge->prvTo,
			pedge->fInvDY,
			pedge->fStartYDiff,
			fXDifference,
			&pedge->lineIncrement.gourIntensity,
			pedge->lineIncrement.fXDifference
		);
		
		// Use the setup routine for indexed coordinates.
		indCoord.InitializeAsBase
		(
			pdtri->ptexTexture,
			pedge->prvFrom,
			pedge->prvTo,
			pedge->lineIncrement.indCoord,
			pedge->fInvDY,
			pedge->lineIncrement.fXDifference,
			fXDifference,
			pedge->fStartYDiff,
			pdtri->prpolyPoly->iMipLevel
		);

		//
		// If the negate modulus subraction sign is set, negate the modulus subraction sign
		// for all related variables.
		//
		if (b_swap_modsub_sign)
		{
			-gourIntensity;
			-indCoord;
		}
	}

	//*****************************************************************************************
	//
	forceinline bool operator +=
	(
		const CScanline& line	// Increment to the current scanline.
	)
	//
	// Adds two CScanline objects together and accumulates it in the calling object.
	//
	// Returns 'true' if a modulus subtraction must be performed.
	//
	//**************************************
	{
		bool b_mod_sub = false;	// Modulus subtraction return flag.

		//
		// Increment the line edge variables by the edgewalking amount.
		fxX           += line.fxX;
		fxXDifference += line.fxXDifference;
		gourIntensity += line.gourIntensity;
		indCoord      += line.indCoord;

		// Check if fxXDifference overflows.
		if (fxXDifference >= (::fixed)1)
		{
			fxXDifference -= (::fixed)1;
			b_mod_sub = true;
		}
		else
		{
			// Check if fxXDifference underflows.
			if (fxXDifference < (::fixed)0)
			{
				fxXDifference += (::fixed)1;
				b_mod_sub = true;
			}
		}

		// Perform modulus subtractions.
		if (b_mod_sub)
		{
			gourIntensity.ModulusSubtract();
			indCoord.ModulusSubtract();
		}

		// No modulus subtraction.
		return b_mod_sub;
	}

	//*****************************************************************************************
	//
	void AssertRange
	(
		ptr_const<CTexture> ptex,
		int                 i_mip_level
	)
	//
	// Tests that the current values of scanline variables are within range.
	// Call this at the end of a scanline.
	//
	//**************************************
	{
		indCoord.AssertRange(ptex, i_mip_level);
		gourIntensity.AssertRange();
	}

	//*********************************************************************************************
	//
	void AssertXRange
	(
		::fixed fx_linelength				// Length of current scanline.
	)
	//
	// Assert that all incremented variables are within range.
	//
	//**********************************
	{
#if VER_DEBUG
		int i_xrange = int(fxX + fx_linelength) - int(fxX) - 1;
		if (i_xrange >= 0)
		{
			indCoord.AssertXRange(i_xrange);
			gourIntensity.AssertXRange(i_xrange);
		}
#endif
	}

	//*********************************************************************************************
	//
	static forceinline bool bIsPlanar()
	//
	// Returns:
	//		Whether this feature has planar gradients across any polygon.
	//
	//**********************************
	{
		//
		// Currently, linear indexing and Gouraud shading can create gradients which are not
		// planar.  This is specified by the individual template classes.
		//
		return INDEX::bIsPlanar() && GOUR::bIsPlanar();
	}

	//*********************************************************************************************
	//
	forceinline void InitializePolygonData
	(
		CPArray<SRenderVertex*> parv_all,	// List of all polygon vertices.
		ptr_const<CTexture>     ptex,		// Texture for polygon.
		TClutVal				cv_face,	// Face-wide lighting.
		int                     i_mip_level	// Mipmap level.
	)
	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	//
	//**************************************
	{
		// Copy screen coords to our local vertices.
		Assert(parv_all.uLen <= iMAX_RASTER_VERTICES);

		// Initialize polygon-wide variables for index coordinates.
		indCoord.InitializePolygonData(ptex, i_mip_level);

		// Initialize polygon-wide variables for Gouraud shading.
		gourIntensity.InitializePolygonData(ptex, cv_face);

		// Set the base address for the clut.
		SetClutAddress(ptex);

		for (int i = 0; i < parv_all.uLen; i++)
		{
			arvRasterVertices[i].v3Screen = parv_all[i]->v3Screen;
			arvRasterVertices[i].iYScr = iPosFloatCast(arvRasterVertices[i].v3Screen.tY);

			indCoord.InitializeVertexData(parv_all[i], &arvRasterVertices[i]);
			gourIntensity.InitializeVertexData(parv_all[i], &arvRasterVertices[i]);
		}
	}

	//*****************************************************************************************
	//
	forceinline void InitializeTriangleData
	(
		SRenderVertex*  prv_1,		// First of three non-colinear coordinates describing the
									// plane of the triangle in screen coordinates.
		SRenderVertex*  prv_2,		// Second coordinate describing the plane.
		SRenderVertex*  prv_3,		// Third coordinate describing the plane.
		float           f_yab_invdx,		// Multipliers for gradients.
		float           f_yac_invdx,
		ptr_const<CTexture> ptex,	// Pointer to source texture.
		bool			b_update = false,	// Modify the gradients rather than setting them.
		bool			b_altpersp = false	// Use alternate perspecive settings.
	)
	//
	// Use the class storage structure to for triangle-wide data. This member function
	// should only be called by CDrawPolygon.
	//
	//**************************************
	{
		// Initialize triangle-wide variables for index coordinates.
		indCoord.InitializeTriangleData(prv_1, prv_2, prv_3, ptex, f_yab_invdx, f_yac_invdx, b_update, b_altpersp);

		// Initialize triangle-wide variables for Gouraud shading.
		gourIntensity.InitializeTriangleData(prv_1, prv_2, prv_3, f_yab_invdx, f_yac_invdx, b_update);
	}

	//*****************************************************************************************
	//
	forceinline void SetClutAddress
	(
		ptr_const<CTexture> ptex	// Pointer to the source texture.
	)
	//
	// Returns a pointer to a position within a clut for use by the source texture.
	//
	//**************************************
	{
		// Set a default value of null for the clut pointer.
		pvClutConversion = 0;
		u4ConstColour = 0;


		if (CLU::bUseClut())
		{
			//
			// Check to see if a texture with an associated clut is present. If it isn't present,
			// fall through.
			//
			if (ptex && ptex->ppcePalClut && ptex->ppcePalClut->pclutClut)
			{
				// Call the get address function for the clut.
				pvClutConversion = ptex->ppcePalClut->pclutClut->pvGetConversionAddress
				(
					MAP::u4GetClutBaseColour(ptex),
					iBaseIntensity,
					iDefaultFog
				);
				Assert(pvClutConversion);
			}
		}
		else
		{
			// If a constant colour can be set, do nothing else.
			MAP::bSetConstColour(ptex, iBaseIntensity, iDefaultFog);
		}
	}

};


#endif
