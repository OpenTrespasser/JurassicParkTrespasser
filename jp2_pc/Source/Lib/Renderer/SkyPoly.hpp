/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Structures and code for supporting the sky as a polygon.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/SkyPoly.hpp                                              $
 * 
 * 1     1/05/98 6:26p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_SKYPOLY_HPP
#define HEADER_LIB_RENDERER_SKYPOLY_HPP

//
// Necessary includes.
//
#include "Lib/W95/Direct3D.hpp"
#include "Lib/View/RasterD3D.hpp"


//
// Structure definitions.
//

//*********************************************************************************************
//
struct SSkyClip
//
// Encapsulates sky polygons for D3D use.
//
// Prefix: skc
//
//**************************************
{
	bool bValid;
	SSkyClip*   pskcBefore;
	SSkyClip*   pskcAfter;
	CVector3<>* pv3From;
	CVector3<>* pv3To;
	CVector3<>  v3IntersectSky;
	CVector2<>  v2Screen;
	CVector2<>  v2UVSky;
	float       fLen;
	float       fZ;

	//*****************************************************************************************
	//
	// Constructor.
	//

	// Default constructor.
	SSkyClip()
	{
		bValid     = false;
		pskcBefore = 0;
		pskcAfter  = 0;
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Set(CVector3<>* pv3_from, CVector3<>* pv3_to, float f_screen_x, float f_screen_y)
	//
	// Sets up the sky polygon.
	//
	//**************************************
	{
		Assert(pv3_from);
		Assert(pv3_to);

		pv3From = pv3_from;
		pv3To   = pv3_to;
		v2Screen.tX = f_screen_x;
		v2Screen.tY = f_screen_y;
	}

	//*****************************************************************************************
	//
	void SetValid(SSkyClip* pskc_before, SSkyClip* pskc_after)
	//
	// Determines if this edge is valid for rendering.
	//
	//**************************************
	{
		Assert(pskc_before);
		Assert(pskc_after);

		pskcBefore = pskc_before;
		pskcAfter  = pskc_after;
		if (pskcBefore->bValid)
		{
			bValid = !pskcAfter->bValid;
			return;
		}
		if (pskcAfter->bValid)
		{
			bValid = !pskcBefore->bValid;
			return;
		}
		bValid = false;
	}

	//*****************************************************************************************
	//
	void SetIntersection(float f_height, float f_scale)
	//
	// Parametrically determines the intersection between the sky view frustum and the sky.
	//
	//**************************************
	{
		// Parametrically determine at what point z intersects the sky.
		fLen = (f_height - pv3From->tZ) / (pv3To->tZ - pv3From->tZ);

		// Apply the parametric to x and y.
		v3IntersectSky.tX = (pv3To->tX - pv3From->tX) * fLen + pv3From->tX;
		v3IntersectSky.tY = (pv3To->tY - pv3From->tY) * fLen + pv3From->tY;
		v3IntersectSky.tZ = f_height;

		// Determine the UV values.
		v2UVSky.tX = v3IntersectSky.tX * f_scale + 1.50f;
		v2UVSky.tY = v3IntersectSky.tY * f_scale;
	}

	//*****************************************************************************************
	//
	void SetScreenLeft()
	//
	// Sets up clipping against the left side of the screen.
	//
	//**************************************
	{
		v2Screen.tX = 0.0f;
		v2Screen.tY = v2Screen.tY * (1.0f - fLen);
	}

	//*****************************************************************************************
	//
	void SetScreenRight()
	//
	// Sets up clipping against the right side of the screen.
	//
	//**************************************
	{
		v2Screen.tY = v2Screen.tY * fLen;
	}

	//*****************************************************************************************
	//
	void SetScreenTop()
	//
	// Sets up clipping against the top of the screen.
	//
	//**************************************
	{
		v2Screen.tX = v2Screen.tX * fLen;
		v2Screen.tY = 0.0f;
	}

	//*****************************************************************************************
	//
	void SetScreenBottom()
	//
	// Sets up clipping against the bottom of the screen.
	//
	//**************************************
	{
		v2Screen.tX = v2Screen.tX * (1.0f - fLen);
	}

};

#endif // HEADER_LIB_RENDERER_SKYPOLY_HPP