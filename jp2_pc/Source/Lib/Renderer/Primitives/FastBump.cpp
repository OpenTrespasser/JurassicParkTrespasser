/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of 'FastBump.hpp.'
 *
 * To do:
 *		Get rid of DrawBump.  Let CDrawPolygon<> do the triangulisation, and CLineBumpMake do
 *		the matrix setup required.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/FastBump.cpp                                  $
 * 
 * 113   10/02/98 3:02p Mmouni
 * Added missing brackets to delete.
 * 
 * 112   98.09.20 12:35a Mmouni
 * Now clears the erfMIPS_CREATED flag for curved bumps.
 * 
 * 111   9/11/98 8:48p Agrant
 * Removed re-calculculation of solid colour since it was causing an assert in TextureManager.
 * 
 * 110   98.09.10 2:01p Mmouni
 * Solid colours for curved bump polys are now computed from their raster after curving.
 * 
 * 109   8/27/98 9:13p Asouth
 * loop variable moved out of block scope
 * 
 * 108   98.08.26 6:57p Mmouni
 * Non-curved bump map maptrices are now hash and kept in a map for quick matching.
 * Added FastBumpCleanup().
 * 
 * 107   98.08.25 4:36p Mmouni
 * Curved bump-mapped objects no longer allocate unique matrices.
 * Remove old commented out cruft.
 * 
 * 106   8/21/98 2:25a Rwyatt
 * Added a terminal error in VER_TEST builds for textures missing in the swap file.
 * 
 * 105   8/19/98 1:30p Rwyatt
 * Removed bump stats and replaced them with mem counters
 * Bump matricies are now allocated from the load heap.
 * 
 * 104   98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 103   98/07/22 14:41 Speter
 * mx3ObjToTexture now scaled down slightly, to guard against lighting value range errors.
 * 
 * 102   7/10/98 5:10p Mlange
 * Poly iterators now take a render context instead of a camera.
 * 
 * 101   98.07.08 12:12p Mmouni
 * Moved CPixelFormatBumpMap to the header file.
 * 
 * 100   7/08/98 12:12p Rwyatt
 * Adjsuted curved bump hash generation to use integers instead of floats.
 * 
 * 99    98/06/29 16:25 Speter
 * Rendering functions now take CInstance*.
 * 
 * 98    98/06/01 18:43 Speter
 * Added new include.
 * 
 * 97    4/27/98 8:54p Pkeet
 * Made an assert more tolerant.
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "FastBump.hpp"

#include "FastBumpMath.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"
#include "Lib/Renderer/Primitives/LineBumpMake.hpp"
#include "Lib/GeomDBase/MeshIterator.hpp"
#include "Lib/EntityDBase/MessageLog.hpp"
#include "Lib/Loader/TextureManager.hpp"
#include "Lib/Loader/ImageLoader.hpp"
#include "Lib/Std/Hash.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <map>

#define bAXIS_SWAP			(1)


//
// Forward declaration of functions used in this module.
//

//*****************************************************************************************
void FatFill(CTexCoord& tc_0, CTexCoord& tc_1, CTexCoord& tc_2, rptr<CBumpMap> pbump);

//*****************************************************************************************
CMatrix3<>* pmx3GetSharedMatrix(const CMatrix3<> &mx3_mat);

//*****************************************************************************************
CMatrix3<>* pmx3GetVerticalAxesSwapMatrix(const CDir3<>& d3_control);

//*********************************************************************************************
void DrawTriangleBump
(
	CBumpMap*			prasDest,
	CRenderPolygon&		rpoly			// Info on polygon (triangle) to render.
);


//*****************************************************************************************
class CVerticalAxisSwapMatrices
{
private:
	CMatrix3<> mx3SwapMatrices[6];

public:
	//*************************************************************************************
	CVerticalAxisSwapMatrices();

	//*************************************************************************************
	CMatrix3<>* pmx3GetMatrix
	(
		const CDir3<>& d3_control
	);

	//*************************************************************************************
	CMatrix3<>* pmx3GetMatrix
	(
		int i_index
	);

	int iNumMatrices() const
	{
		return 6;
	}
};

static CVerticalAxisSwapMatrices SwapMatrices;


//*********************************************************************************************
//
// CBumpAnglePair implementation.
//

	//*****************************************************************************************
	void CBumpAnglePair::SetBump(const CDir3<>& rdir3, bool b_set_light_phi)
	{
		uint u_theta;

		//
		// Get theta. Note that this function relies on an arctan lookup table. To ensure
		// precision and reduce the size of the arctan table, the conversion table only 
		// covers a 45 degree slice. 
		//
		if (Abs(rdir3.tX) >= Abs(rdir3.tY))
		{
			//
			// If there is no x value, theta must be zero (i.e., the normal points along
			// the y axis.
			//
			if (rdir3.tX == 0.0f)
			{
				u_theta = 0;
			}
			else
			{
				// tY divide by tX will always be less than one.
				u_theta = BumpAngleConvert.uArcTan45(rdir3.tY / rdir3.tX);

				// If x is negative, theta must lay 180 degrees opposite of the table's value.
				if (rdir3.tX < 0.0f)
				{
					u_theta -= iTHETA_PI;
				}
			}
		}
		else
		{
			// tX divide by tY will always be less than one.
			u_theta = iTHETA_PI_2 - BumpAngleConvert.uArcTan45(rdir3.tX / rdir3.tY);

			// If x is negative, theta must lay 180 degrees opposite of the table's value.
			if (rdir3.tY < 0.0f)
			{
				u_theta -= iTHETA_PI;
			}
		}

		//
		// Set the Phi angle, which is the arcsine of z.
		//
		if (b_set_light_phi)
			// Set the light phi value.
			SetPhiL(BumpAngleConvert.iArcSinL(rdir3.tZ));
		else
			// Set the bump phi value (clamped to positive).
			SetPhiB(BumpAngleConvert.iArcSinB(rdir3.tZ));

		// Set the theta angle.
		SetTheta(u_theta - iTHETA_PI_2);
	};

	//*********************************************************************************************
	float CBumpAnglePair::fGetPhiB()
	{
		return BumpAngleConvert.fGetPhiB(*this);
	}

	//*********************************************************************************************
	float CBumpAnglePair::fGetPhiL()
	{
		return BumpAngleConvert.fGetPhiL(*this);
	}

	//*****************************************************************************************
	CDir3<> CBumpAnglePair::dir3MakeNormal()
	{
		// Use the transform class to convert the angles in radians to a normal.
		return BumpAngleConvert.dir3AnglesToNormal(fGetPhiB(), fGetTheta());
	}

	//*****************************************************************************************
	CDir3<> CBumpAnglePair::dir3MakeNormalFast()
	{
		// Use the transform class to convert the angles in radians to a normal.
		return BumpAngleConvert.dir3AnglesToNormal(*this);
	}


//*********************************************************************************************
//
// CBumpMap member functions.
//

	//*****************************************************************************************
	//
	// CBumpMap constructors.
	//
	
	//*****************************************************************************************
	CBumpMap::CBumpMap(int i_width, int i_height, const CPal* ppal, TPixel pix_solid)
		: CRasterMemT<CBumpAnglePair>(i_width, i_height)
	{
		Assert(ppal);

		// Force the pixel format to be 8-bit (paletted), so that colour conversion functions work.
		pxf = CPixelFormatBumpmap();

		// Store the palette pointer in the bump-map palette.
		AttachPalette((CPal*)ppal);

		//
		// Fill bumpmap memory with an up-facing bump of the solid colour.
		//

		CBumpAnglePair	bang;
		bang.SetPhiB(iMAX_PHI_B);
		bang.SetColour(pix_solid);

		Clear(bang);
	}


	//*****************************************************************************************
	CBumpMap::CBumpMap(rptr<CRaster> pras_heightmap, rptr<CRaster> pras_texture, float f_bumpiness)
		: CRasterMemT<CBumpAnglePair>(pras_texture->iWidth, pras_texture->iHeight)
	{
		Assert(pras_texture->iWidth  > 0);
		Assert(pras_texture->iHeight > 0);
		if (pras_heightmap)
		{
			Assert(pras_heightmap->iWidth  == pras_texture->iWidth);
			Assert(pras_heightmap->iHeight == pras_texture->iHeight);
		}
		Assert(f_bumpiness >= 0.0f);

		// Force the pixel format to be 8-bit (paletted), so that colour conversion functions work.
		pxf = CPixelFormatBumpmap();

		// Clear memory for the bumpmap.
		Clear();

		// Add bumps.
		AddHeightField(pras_heightmap, f_bumpiness);

		// Add texture.
		AddTexture(pras_texture);
	}

	//*****************************************************************************************
	CBumpMap::CBumpMap(char* str_height_filename, char* str_texture_filename, float f_bumpiness)
	{
		Assert(str_height_filename);
		Assert(str_texture_filename);
		Assert(f_bumpiness >= 0.0f);

		// Load the texturemap associated with the bumpmap.
		rptr<CRaster> pras_texture = prasReadBMP(str_texture_filename);

		// Load heightmap.
		rptr<CRaster> pras_bumpmap = prasReadBMP(str_height_filename);

		new(this) CBumpMap(pras_texture, pras_bumpmap, f_bumpiness);
	}


	//*****************************************************************************************
	//
	// CBumpMap member functions.
	//

	//*****************************************************************************************
	//
	void CBumpMap::AddHeightField
	(
		rptr<CRaster> pras_heightmap,	// Height field representing bumpmap.  May be rptr0.
		float f_bumpiness				// Value to multiply height by to get vertical value
										// in pixels.
	)
	//
	// Converts a heightfield to a bumpmap.
	//
	//**************************************
	{
		if (pras_heightmap)
		{
			Assert(pras_heightmap->pSurface);
			Assert(iWidth  == pras_heightmap->iWidth);
			Assert(iHeight == pras_heightmap->iHeight);
		}
		else
			f_bumpiness = 0;
		Assert(f_bumpiness >= 0.0f);

		int i_x, i_y;

		//
		// Fill the centre of the bumpmap with angles.
		//
		for (i_y = 1; i_y < iHeight; i_y++)
		{
			CBumpAnglePair* pbang = &tPix(1, i_y);

			for (i_x = 1; i_x < iWidth; i_x++)
			{
				if (f_bumpiness)
					pbang->SetBump(CDir3<>
					(
						(int)pras_heightmap->pixGet(i_x-1, i_y) - (int)pras_heightmap->pixGet(i_x, i_y),
						(int)pras_heightmap->pixGet(i_x, i_y-1) - (int)pras_heightmap->pixGet(i_x, i_y),
						1.0f / f_bumpiness
					));
				else
					pbang->SetBump(d3ZAxis);
				pbang++;
			}
		}

		//
		// Fill the top and left edges by copying from the next inner edges.
		// This continues the curve in the same direction it was going.
		//

		// Fill the top angles.
		for (i_x = 0; i_x < iWidth; i_x++)
		{
			tPix(i_x, 0).SetBump(tPix(i_x, 1));
		}

		// Fill the left angles.
		for (i_y = 0; i_y < iHeight; i_y++)
		{
			tPix(0, i_y).SetBump(tPix(1, i_y));
		}
	}

	//*****************************************************************************************
	//
	void CBumpMap::AddTexture
	(
		rptr<CRaster> pras_texture	// Raster representing bitmap.
	)
	//
	// Adds an 8-bit bitmap to the bumpmap.
	//
	//**************************************
	{
		Assert(pras_texture->pSurface);
		Assert(pras_texture->pxf.ppalAttached);
		Assert(iWidth  == pras_texture->iWidth);
		Assert(iHeight == pras_texture->iHeight);
		Assert(pras_texture->iPixelBits == 8);

		// Store the texture palette in the bump-map palette.
		AttachPalette(pras_texture->pxf.ppalAttached);

		// Copy the texture to the combined bumpmap.
		for (int i_y = 0; i_y < iHeight; i_y++)
		{
			CBumpAnglePair* pbang = &tPix(0, i_y);
			uint8* pu1_tex = (uint8*)pras_texture->pAddress(0, i_y);

			for (int i_x = 0; i_x < iWidth; i_x++)
			{
				pbang->SetColour(*pu1_tex++);
				pbang++;
			}
		}
	}

	//*****************************************************************************************
	void CBumpMap::DrawBump(CTexCoord tc_0, CDir3<> dir3_normal_0,
							CTexCoord tc_1, CDir3<> dir3_normal_1,
							CTexCoord tc_2, CDir3<> dir3_normal_2,
							const CMatrix3<>& mx3_reverse)
	{
		Assert(bWithin(tc_0.tX, 0.0f, 1.0f));
		Assert(bWithin(tc_1.tX, 0.0f, 1.0f));
		Assert(bWithin(tc_2.tX, 0.0f, 1.0f));
		Assert(pSurface);

		// Copy over the reverse transform.
		mx3Reverse = mx3_reverse;

		// Create a CRenderPolygon for CDrawPolygon.
		CRenderPolygon rp;

		rp.cvFace = 0;

		SRenderVertex arvVertices[3];
		SRenderVertex* aprvVertices[3] = { &arvVertices[0], &arvVertices[1], &arvVertices[2] };

		// Assign the polygon's vertex pointer array.
		rp.paprvPolyVertices = PArray(3, aprvVertices);

		// Get a floating point representation of width and height.
		float f_width  = float(iWidth) /* - fTexEdgeTolerance * 2.0f */;
		float f_height = float(iHeight) /* - fTexEdgeTolerance * 2.0f */;

		float f_bias = 0.0f /* + fTexEdgeTolerance */;

		//
		// Convert texture coordinates and normals to the SRenderVertex/SRenderTriangle format.
		//
		arvVertices[0].v3Screen.tX = tc_0.tX * f_width + f_bias;
		arvVertices[0].v3Screen.tY = tc_0.tY * f_height + f_bias;
		arvVertices[0].v3Screen.tZ = 0.001f;
		arvVertices[0].pdir3Normal = &dir3_normal_0;

		arvVertices[1].v3Screen.tX = tc_1.tX * f_width + f_bias;
		arvVertices[1].v3Screen.tY = tc_1.tY * f_height + f_bias;
		arvVertices[1].v3Screen.tZ = 0.001f;
		arvVertices[1].pdir3Normal = &dir3_normal_1;

		arvVertices[2].v3Screen.tX = tc_2.tX * f_width + f_bias;
		arvVertices[2].v3Screen.tY = tc_2.tY * f_height + f_bias;
		arvVertices[2].v3Screen.tZ = 0.001f;
		arvVertices[2].pdir3Normal = &dir3_normal_2;

#define fCONSTANT -0.0001f
#define fLOW 0.0001f

		// No out of boundses, please.
		SetMinMax(arvVertices[0].v3Screen.tX, fLOW, iWidth  + fCONSTANT);
		SetMinMax(arvVertices[1].v3Screen.tX, fLOW, iWidth  + fCONSTANT);
		SetMinMax(arvVertices[2].v3Screen.tX, fLOW, iWidth  + fCONSTANT);
		SetMinMax(arvVertices[0].v3Screen.tY, fLOW, iHeight + fCONSTANT);
		SetMinMax(arvVertices[1].v3Screen.tY, fLOW, iHeight + fCONSTANT);
		SetMinMax(arvVertices[2].v3Screen.tY, fLOW, iHeight + fCONSTANT);

		//
		// Use a standard rasterizing routine to draw a triangle with interpolated curvature on
		// the bumpmap surface.
		//
		//CDrawPolygonWide< CLineBumpMake<CBumpAnglePair> >(this, rp);
		DrawTriangleBump(this , rp);

#if (0)
		//
		// Check the dimension of the triangle.
		//
		float min_x = arvVertices[0].v3Screen.tX;
		float min_y = arvVertices[0].v3Screen.tY;
		float max_x = arvVertices[0].v3Screen.tX;
		float max_y = arvVertices[0].v3Screen.tY;
		for (int i = 1; i < 3; i++)
		{
			if (min_x > arvVertices[i].v3Screen.tX)
				min_x = arvVertices[i].v3Screen.tX;

			if (min_y > arvVertices[i].v3Screen.tY)
				min_y = arvVertices[i].v3Screen.tY;

			if (max_x < arvVertices[i].v3Screen.tX)
				max_x = arvVertices[i].v3Screen.tX;

			if (max_y < arvVertices[i].v3Screen.tY)
				max_y = arvVertices[i].v3Screen.tY;
		}

		if  (max_x - min_x < 1.0f || max_y - min_y < 1.0f)
		{
			// Fill the whole map in x.
			for (int y = 0; y < iHeight; y++)
			{
				CBumpAnglePair* pbang_map = (CBumpAnglePair*)pSurface + y * iLinePixels;

				// Find first pixel in x.
				for (int x = 0; x < iWidth; x++)
					if (pbang_map[x].bGetCurveFlag())
						break;

				if (x < iWidth)
				{
					int start_x = x;

					for (x = start_x-1; x >= 0; x--)
						pbang_map[x] = pbang_map[start_x];
				}

				// Find last pixel in x.
				for (x = iWidth-1; x >= 0; x--)
					if (pbang_map[x].bGetCurveFlag())
						break;

				if (x >= 0)
				{
					int start_x = x;

					for (x = start_x+1; x < iWidth; x++)
						pbang_map[x] = pbang_map[start_x];
				}
			}		

			// Fill the whole map in y.
			for (int x = 0; x < iWidth; x++)
			{
				CBumpAnglePair* pbang_map = (CBumpAnglePair*)pSurface + x;

				// Find first pixel in y.
				for (int y = 0; y < iHeight; y++)
					if (pbang_map[y*iLinePixels].bGetCurveFlag())
						break;

				if (y < iHeight)
				{
					int start_y = y;

					for (y = start_y-1; y >= 0; y--)
						pbang_map[y*iLinePixels] = pbang_map[start_y*iLinePixels];
				}

				// Find last pixel in y.
				for (y = iHeight-1; y >= 0; y--)
					if (pbang_map[y*iLinePixels].bGetCurveFlag())
						break;

				if (y >= 0)
				{
					int start_y = y;

					for (y = start_y+1; y < iHeight; y++)
						pbang_map[y*iLinePixels] = pbang_map[start_y*iLinePixels];
				}
			}		
	
			// Make sure map is filled.
			for (y = 0; y < iHeight; y++)
			{
				CBumpAnglePair* pbang_map = (CBumpAnglePair*)pSurface + y * iLinePixels;

				for (int x = 0; x < iWidth; x++)
					Assert(pbang_map[x].bGetCurveFlag());
			}
		}
#endif
	}

	//*****************************************************************************************
	void CBumpMap::DrawBumps(CTexCoord tc_0, CDir3<> dir3_normal_0,
		                     CTexCoord tc_1, CDir3<> dir3_normal_1,
							 CTexCoord tc_2, CDir3<> dir3_normal_2,
							 CDir3<> dir3_normal_face, const CMatrix3<>& mx3_reverse)
	{
		// Get the coordinates for the newly generated vertex.
		CTexCoord  tc_centre = (tc_0 + tc_1 + tc_2) / 3.0f;

		// Draw the subtriangles.
		DrawBump(tc_0, dir3_normal_0, tc_1, dir3_normal_1, tc_centre,
			     dir3_normal_face, mx3_reverse);
		DrawBump(tc_centre, dir3_normal_face, tc_1, dir3_normal_1, tc_2,
			     dir3_normal_2, mx3_reverse);
		DrawBump(tc_0, dir3_normal_0, tc_centre, dir3_normal_face, tc_2,
			     dir3_normal_2, mx3_reverse);
	}

	//
	// CBumpMap static variables.
	//
	CMatrix3<> CBumpMap::mx3Reverse;


//
// Global functions.
//

//*****************************************************************************************
inline TReal rSignedArea
(
	const CVector2<>& v2_0, const CVector2<>& v2_1, const CVector2<>& v2_2
)
{
	return (v2_1 - v2_0) ^ (v2_2 - v2_0);
}


//
// Fuzzy specialisation for matrix.
//

//**********************************************************************************************
inline TReal Difference(const CMatrix3<>& mx3_a, const CMatrix3<>& mx3_b)
//
// Specialise the Difference function used by CFuzzy<>.
//
// Note: this version does not allow ordering comparisions, just [in]equality.
//
//**************************************
{
	return Difference(mx3_a.v3X, mx3_b.v3X) +
		   Difference(mx3_a.v3Y, mx3_b.v3Y) +
		   Difference(mx3_a.v3Z, mx3_b.v3Z);
}


//**********************************************************************************************
inline CFuzzy<CMatrix3<>, TReal> Fuzzy(const CMatrix3<>& mx3_value, TReal r_tolerance = 0.01)
// Specialise the Fuzzy function.
{
	return CFuzzy<CMatrix3<>, TReal>(mx3_value, r_tolerance);
}


//*****************************************************************************************
CMatrix3<> mx3ObjToTexture
(
	const CVector3<>& v3_0, const  CVector3<>& v3_1, const CVector3<>& v3_2, const CDir3<>& d3,
	const CTexCoord&  tc_0, const  CTexCoord&  tc_1, const CTexCoord&  tc_2
)
{
	//
	// The direction of the SDirReflectData is in object space.  We need to convert
	// that to texture space.  
	// We need a transformation which converts a frame representing the object triangle
	// to a frame representing the texture triangle.
	// Each frame consists of a vector in the plane of the triangle, and a vector
	// perpendicular to it.
	//

	// First construct the inverse object frame matrix, from the triangle edge vector and normal.
	CMatrix3<> mx3_obj = CMatrix3<>
	(
		// The first direction is the one from point 0 to the midpoint of 1 and 2.
		(v3_1 + v3_2) * 0.5 - v3_0,

		// The second direction is the object-space normal of the triangle.
		d3,

		// These are perpendicular.
		true
	);

	// Shrink it a bit to ensure we don't generate normals that are too large.
	mx3_obj *= CScaleI3<>(0.999f);

	Assert(Fuzzy(~mx3_obj, 0.1f) == mx3_obj.mx3Transpose());

	TReal r_sign = rSignedArea(tc_0, tc_1, tc_2);
	if (r_sign == 0)
		// Degenerate texture triangle; just return the object transform.
		return mx3_obj.mx3Transpose();

	// The direction from point 0 to the midpoint of 1 and 2.
	CDir2<> d2_tex = (tc_1 + tc_2) * 0.5 - tc_0;

	//
	// Create a similar matrix representing the texture frame.
	// Then concatenate the inverse of mx3_obj with the texture matrix, thus creating
	// a matrix which converts from the object frame to the texture frame.  Finally,
	// if the texture frame's Z axis is below the XY plane (its orientation is clockwise),
	// reverse the Z axis of the resulting transform.
	//
	// We actually do a shortcut version of this process.  The texture matrix has d2_tex
	// as its X, the positive or negative Z axis as its Y, and their cross-product as its Z:
	//
	//		d2_tex.tX	d3_tex.tY	0
	//		0			0			S
	//		S*d3_tex.tY	-S*d2_tex.tX 0
	//		
	// Also, the inverse of mx3_obj is its transpose.  Thus, the concatenation of ~mx3_obj
	// and the texture matrix is given by the code below:
	//

	if (r_sign < 0)
		mx3_obj.v3Z *= -1;

	return CMatrix3<>
	(
		CVector2<>(mx3_obj.v3X.tX, mx3_obj.v3Z.tX) * d2_tex,
		CVector2<>(mx3_obj.v3X.tX, mx3_obj.v3Z.tX) ^ d2_tex,
		mx3_obj.v3Y.tX,

		CVector2<>(mx3_obj.v3X.tY, mx3_obj.v3Z.tY) * d2_tex,
		CVector2<>(mx3_obj.v3X.tY, mx3_obj.v3Z.tY) ^ d2_tex,
		mx3_obj.v3Y.tY,

		CVector2<>(mx3_obj.v3X.tZ, mx3_obj.v3Z.tZ) * d2_tex,
		CVector2<>(mx3_obj.v3X.tZ, mx3_obj.v3Z.tZ) ^ d2_tex,
		mx3_obj.v3Y.tZ
	);
}


//*****************************************************************************************
inline float fGetX(CTexCoord tc, rptr<CRaster> pras)
{
	return tc.tX * float(pras->iWidth);
}


//*****************************************************************************************
inline float fGetY(CTexCoord tc, rptr<CRaster> pras)
{
	return tc.tY * float(pras->iHeight);
}


//*****************************************************************************************
uint32 u4GenerateUniqueBumpHashValue
(
	uint32				u4_old_hash,
	float				f_xmin,
	float				f_ymin,
	float				f_xmax,
	float				f_ymax,
	const CVector3<>&	v3_p1,
	const CVector3<>&	v3_p2,
	const CVector3<>&	v3_p3,
	uint32				u4_poly,
	uint32				u4_vertices
)
//*************************************
{
	struct SGenHash
	{
		uint32		u4HashValue;
		int32		i4MinX;
		int32		i4MinY;
		int32		i4MaxX;
		int32		i4MaxY;
		uint32		u4PolyNumber;
		uint32		u4Verts;
		int32		i4Vert1p1;
		int32		i4Vert1p2;
		int32		i4Vert1p3;
		int32		i4Vert2p1;
		int32		i4Vert2p2;
		int32		i4Vert2p3;
		int32		i4Vert3p1;
		int32		i4Vert3p2;
		int32		i4Vert3p3;
	};

	SGenHash		gen_hash;

	//
	// The curved bump hash value is calculated with ints because we had problems with debug and release
	// mode generating different hash values. All I can think of that could cause this is the differences
	// is code generation generating slightly different float values and therefore a different hash value.
	// I now just use 2 decimal places of precison, if we are generating floating point values that differ
	// by more than 0.01 we have a serious problem.
	//

	// multiple the poly number by the old hash in an attempt to make the hash value more unique.
	gen_hash.u4PolyNumber = u4_poly * u4_old_hash;
	gen_hash.u4Verts = u4_vertices * u4_old_hash;
	gen_hash.i4MinX = (int32)(f_xmin*100.0f);
	gen_hash.i4MinY = (int32)(f_ymin*100.0f);
	gen_hash.i4MaxX = (int32)(f_xmax*100.0f);
	gen_hash.i4MaxY = (int32)(f_ymax*100.0f);
	gen_hash.u4HashValue = u4_old_hash;
	gen_hash.i4Vert1p1 = (int32)(v3_p1.tX*100.0f);
	gen_hash.i4Vert1p2 = (int32)(v3_p1.tY*100.0f);
	gen_hash.i4Vert1p3 = (int32)(v3_p1.tZ*100.0f);
	gen_hash.i4Vert2p1 = (int32)(v3_p2.tX*100.0f);
	gen_hash.i4Vert2p2 = (int32)(v3_p2.tY*100.0f);
	gen_hash.i4Vert2p3 = (int32)(v3_p2.tZ*100.0f);
	gen_hash.i4Vert3p1 = (int32)(v3_p3.tX*100.0f);
	gen_hash.i4Vert3p2 = (int32)(v3_p3.tY*100.0f);
	gen_hash.i4Vert3p3 = (int32)(v3_p3.tZ*100.0f);
	
	return u4Hash(&gen_hash, sizeof(gen_hash), false);
}


#pragma optimize("p", on)	// Imrove float cosistency for generation of hash values.

//*****************************************************************************************
void GetUniqueTextureSize
(
	CMesh::CPolyIterator& pi,	// Polygon iterator containing current polygon.
	CTexCoord& tc_min_src,
	CTexCoord& tc_max_src
)
{
	for (int i = 0; i < pi.iNumVertices(); i++)
	{
		CTexCoord tc = pi.tcTexCoord(i);
		if (i == 0)
			tc_min_src = tc_max_src = tc;
		else
		{
			SetMin(tc_min_src.tX, tc.tX);
			SetMin(tc_min_src.tY, tc.tY);
			SetMax(tc_max_src.tX, tc.tX);
			SetMax(tc_max_src.tY, tc.tY);
		}
	}
}

#pragma optimize("p", off)


//*********************************************************************************************
void AdjustUniqueTextureCoordinates
(
	CMesh::CPolyIterator&	pi,				// Polygon iterator containing current polygon.
	rptr<CRaster>			pbump,			// original raster
	rptr<CRaster>			pbump_new,		// new raster
	CTexCoord				tc_min			// minimum texture co-ordinates
)
//*************************************
{
	//
	// Set new texture coordinates.
	//
	int i_x_src_0 = iTrunc(fGetX(tc_min, rptr_cast(CRaster, pbump))) - 2;
	int i_y_src_0 = iTrunc(fGetY(tc_min, rptr_cast(CRaster, pbump))) - 2;


	float f_x_src_0 = float(i_x_src_0) / float(pbump->iWidth);
	float f_y_src_0 = float(i_y_src_0) / float(pbump->iHeight);

	float f_x_src_to_dest = float(pbump->iWidth) / float(pbump_new->iWidth);
	float f_y_src_to_dest = float(pbump->iHeight) / float(pbump_new->iHeight);

	for (int i = 0; i < pi.iNumVertices(); i++)
	{
		pi.pmvVertex(i)->tcTex.tX = (pi.pmvVertex(i)->tcTex.tX - f_x_src_0) * f_x_src_to_dest;
		pi.pmvVertex(i)->tcTex.tY = (pi.pmvVertex(i)->tcTex.tY - f_y_src_0) * f_y_src_to_dest;

		// Check texture coordinate ranges.
	//	Assert(bWithin(pi.pmvVertex(i)->tcTex.tX, 0.0, 1.0));
	//	Assert(bWithin(pi.pmvVertex(i)->tcTex.tX, 0.0, 1.0));
	}
}


//*****************************************************************************************
rptr<CBumpMap> pbumpCreateUniqueBumpap
(
	CMesh::CPolyIterator&	pi,				// Polygon iterator containing current polygon.
	uint32					u4_new_hash,	// hash value of the new texture
	rptr<CBumpMap>			pbump			// The source bumpmap.
)
{
	Assert(pbump);

	uint32	u4_parent_hash = pi.ptexTexture()->u4HashValue;

	// add this parent bump map to the list of parents. The parents do not have rasters
	// attached to them when the image is saved. If the bump map is only a curved bump
	// parent then it will not be in any other list, but if the parent map is used elsewhere
	// as a normal bump map then the map will also be in the usual pack image directory. In
	// this case the item from the parent list should be removed.
	// A parent map is only added to the list for every curved map that uses but each parent
	// will only be present once in the list.
	gtxmTexMan.AddCurvedBumpParent
	( 
		rptr_cast(CRaster,pbump),
		u4_parent_hash,
		pi.ptexTexture()->seterfFeatures[erfTRANSPARENT]
	);

	// Get the minimum and maximum dimensions for the texture.
	CTexCoord tc_max_src;
	CTexCoord tc_min_src;

	GetUniqueTextureSize(pi,tc_min_src,tc_max_src);

	// Get the source copy area, rounding up/down and adding another pixel just for good measure.
	int i_x_src_0 = iTrunc(fGetX(tc_min_src, rptr_cast(CRaster, pbump))) - 2;
	int i_y_src_0 = iTrunc(fGetY(tc_min_src, rptr_cast(CRaster, pbump))) - 2;
	int i_x_src_1 = iTrunc(fGetX(tc_max_src, rptr_cast(CRaster, pbump))) + 3;
	int i_y_src_1 = iTrunc(fGetY(tc_max_src, rptr_cast(CRaster, pbump))) + 3;

	if (i_y_src_0 > i_y_src_1)
		Swap(i_y_src_0, i_y_src_1);

	if (i_x_src_0 > i_x_src_1)
		Swap(i_x_src_0, i_x_src_1);

	Assert(i_x_src_0 < i_x_src_1);
	Assert(i_y_src_0 < i_y_src_1);

	// if the size of the new map, in any dimension, is equal to the grow amount then
	// we have generated a curved texture that would have zero height. This will
	// produce large amounts of blue!.
//	Assert( (i_x_src_1 - i_x_src_0)>5);		// ZERO PIXEL BUMP MAP-IDENTICAL U,V??
//	Assert( (i_y_src_1 - i_y_src_0)>5);		// ZERO PIXEL BUMP MAP-IDENTICAL U,V??

	rptr<CBumpMap> pbump_new;

	// if the size of the new bump map is more than 5 bigger than the source bump
	// map then the UV co-ords of the cuvred section must be tiled and the delta UV
	// must be above 1.0. Tiled UV co-ords are not illegal, for example 0.4 to 1.4 is
	// tiled but valid as there is no overlap. A co-ords of 0.4 to 2.0 are illegal as
	// there is overlap. Overlapping tiled co-ords work perfectly it just makes new 
	// rasters which are huge, the same as the tiling factor larger than the source.
	// For example, a tiled curved map which goes from 0.0 to 2.0 in both directions
	// with a source map of 256x256 would make a curved map of 512x512.

	// Try 8 pixels to allow a little overlap
	Assert( ((i_x_src_1 - i_x_src_0)-8)<=pbump->iWidth);		//TILED CURVED MAP
	Assert( ((i_y_src_1 - i_y_src_0)-8)<=pbump->iHeight);		//TILED CURVED MAP


	// Create a bumpmap of the width and height of the source rectangle.
	pbump_new = rptr_new CBumpMap
	(
		i_x_src_1 - i_x_src_0,
		i_y_src_1 - i_y_src_0,
		pbump->pxf.ppalAttached
	);

	
	//
	// Copy the source bumpmap rectangle to the new raster.
	//
	for (int i_y = 0; i_y < pbump_new->iHeight; i_y++)
	{
		int i_y_src = i_y + i_y_src_0;
		if (i_y_src < 0)
		{
			i_y_src += (-i_y_src*pbump->iHeight);
		}

		CBumpAnglePair* pbang = &pbump_new->tPix(0, i_y);
		CBumpAnglePair* pbang_src = &pbump->tPix(0, i_y_src % pbump->iHeight);

		for (int i_x = 0; i_x < pbump_new->iWidth; i_x++)
		{
			int i_x_src = i_x + i_x_src_0;
			if (i_x_src < 0)
			{
				// add on some multiple of the width to ensure that the X co-ord
				// is positive. 
				// The line below will give massive X co-ords but this is OK because
				// the modulus in the array index will keep the actual co-ords in
				// the range 0..iWidth
				i_x_src += (-i_x_src*pbump->iWidth);
			}
			pbang[i_x].br = pbang_src[i_x_src % pbump->iWidth].br;
		}
	}

	AdjustUniqueTextureCoordinates
	(
		pi,
		rptr_cast(CRaster,pbump),
		rptr_cast(CRaster,pbump_new),
		tc_min_src
	);

	// Copy the polygon's current texture, change its raster to the new one,
	// and reassign it to the polygon's unique surface.

	CMesh::SSurface& sf = *pi.pmpPolygon()->pSurface;

	// Create the curved bump map
	sf.ptexTexture = rptr_new CTexture
	(
		rptr_cast(CRaster, pbump_new), 
		sf.ptexTexture->ppcePalClut->pmatMaterial, 
		sf.ptexTexture->tpSolid, 
		sf.ptexTexture->seterfFeatures
	);

	// Set the curved flag.
	sf.ptexTexture->seterfFeatures += erfCURVED;

	// Clear the mips-created flag.
	sf.ptexTexture->seterfFeatures -= erfMIPS_CREATED;

	//
	// set the new hash value for the texture
	//
	sf.ptexTexture->u4HashValue = u4_new_hash;

#if VER_DEBUG
	sf.ptexTexture->Validate();
#endif

	return pbump_new;
}


extern rptr<CTexture> ptexImageTexture
(
	SDirectoryFileChunk*	pdfc,	// Structure that specified virtual memory location
	const CMaterial* pmat			// Material for clut construction.
);


const int iMAX_VERTICES	= 200;



//*****************************************************************************************
void ApplyCurves(rptr<CMesh> pmsh)
{
	CCycleTimer ctmr;

	bool b_made_unique = false;
	uint32	u4_poly_count = 0;

#if VER_DEBUG
	pmsh->Validate();
#endif


	// Iterate through the triangles of the mesh.
	// OK to use 0 for mesh instance and render context, as it doesn't need them.
	for (CMesh::CPolyIterator pi(*pmsh, 0, 0); pi.bNext(); )
	{
		// u4_poly_count is only used for an ID so it does not matter that it is 1 based
		u4_poly_count++;

		// Act only on bumpmaps.
		if (pi.ptexTexture()->seterfFeatures[erfBUMP])
		{
			// Get the transformation from world to texture space.
			CMatrix3<> mx3_obj_to_tex = mx3ObjToTexture
			(
				pi.v3Point(0),
				pi.v3Point(1),
				pi.v3Point(2),
				pi.d3Normal(),
				pi.tcTexCoord(0),
				pi.tcTexCoord(1),
				pi.tcTexCoord(2)
			);

#if !bAXIS_SWAP
			// Save the object-to-texture matrix.
			pi.pmpPolygon()->pmx3ObjToTexture = pmx3GetSharedMatrix(mx3_obj_to_tex);
#endif

			if (!pi.bCurved())
			{
#if bAXIS_SWAP
				// Do not apply curves.  However, save the object-to-texture matrix.
				pi.pmpPolygon()->pmx3ObjToTexture = pmx3GetSharedMatrix(mx3_obj_to_tex);
#endif
				continue;
			}

			MEMLOG_ADD_COUNTER(emlBumpCurveCount,1);

			// Ensure that every polygon has unique vertices and surfaces.
			if (!b_made_unique)
			{
				b_made_unique = true;
				pmsh->MakeVerticesUnique();
				pmsh->MakeSurfacesUnique();
			}

			//
			// Generate the hash value for this section of the curved map and check
			// if we already have one.
			//

			// Get the minimum and maximum dimensions for the texture.
			CTexCoord tc_max_src;
			CTexCoord tc_min_src;

			GetUniqueTextureSize(pi,tc_min_src,tc_max_src);

			rptr<CRaster>	pras = rptr_nonconst(pi.ptexTexture()->prasGetTexture());

			uint32 u4_hash = u4GenerateUniqueBumpHashValue(pi.ptexTexture()->u4HashValue,
					fGetX(tc_min_src, rptr_cast(CRaster, pras )),
					fGetY(tc_min_src, rptr_cast(CRaster, pras )),
					fGetX(tc_max_src, rptr_cast(CRaster, pras )),
					fGetY(tc_max_src, rptr_cast(CRaster, pras )),
					pi.v3Point(0),
					pi.v3Point(1),
					pi.v3Point(2),
					u4_poly_count,
					pi.iNumVertices());

			// Detect zero area texture polygons.
			bool bBadTextureCoords = false;
			if (fabs(rSignedArea(pi.tcTexCoord(0), pi.tcTexCoord(1), pi.tcTexCoord(2))) < 0.001)
				bBadTextureCoords = true;

			//
			// If the polygon we are attempting to curve has zero area, set all the texture
			// values the same.
			//
			if (bBadTextureCoords)
			{
				CTexCoord tc_avg = pi.pmvVertex(0)->tcTex;

				// Compute averages.
				int i;
				for (i = 1; i < pi.iNumVertices(); i++)
				{
					tc_avg += pi.pmvVertex(i)->tcTex;
				}
				tc_avg /= float(pi.iNumVertices());

				// Loop through the triangles of this polygon.
				for (i = 0; i < pi.iNumVertices(); i++)
				{
					pi.pmvVertex(i)->tcTex = tc_avg;
					pi.pmvVertex(i)->d3Normal = pi.d3Normal();
				}

				// Update thest values since we have modified the texture co-ordinates.
				GetUniqueTextureSize(pi,tc_min_src,tc_max_src);
			}

			// is there a valid VM image file open??
			if (CLoadImageDirectory::bImageValid())
			{
				// yes there is a valid image, is our map in it
				SDirectoryFileChunk*	pdfc;
				pdfc = CLoadImageDirectory::plidImageDir->mapChunk[(uint64)u4_hash];

				if (pdfc)
				{
					CMesh::SSurface& sf = *pi.pmpPolygon()->pSurface;
					sf.ptexTexture = ptexImageTexture(pdfc, sf.ptexTexture->ppcePalClut->pmatMaterial );
					sf.ptexTexture->seterfFeatures += erfCURVED;

					AdjustUniqueTextureCoordinates
					(
						pi,
						rptr_cast(CRaster,pras),
						rptr_cast(CRaster,sf.ptexTexture->prasGetTexture()),
						tc_min_src
					);

#if bAXIS_SWAP
					// Store the matrix necessary to transform a light direction from object to the
					// axis-swapped texture space.
					pi.pmpPolygon()->pmx3ObjToTexture = SwapMatrices.pmx3GetMatrix(pi.d3Normal());
#endif

					continue;
				}
				else
				{
#if VER_TEST
					char str_buffer[1024];
					sprintf(str_buffer, 
							"%s\n\nWarning: Curved bump map not in swap file..\n", 
							__FILE__);

					bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__);
#endif
				}
			}

			SVirtualImageElement* vie = gtxmTexMan.vieFindInPackedLog(u4_hash, true, 
																pi.ptexTexture()->tpSolid);

			if (vie)
			{
				dprintf("Instancing curved bump map for polygon %d\n", u4_poly_count-1);

				CMesh::SSurface& sf = *pi.pmpPolygon()->pSurface;
				sf.ptexTexture = vie->ptexTexture;

				AdjustUniqueTextureCoordinates
				(
					pi,
					rptr_cast(CRaster,pras),
					rptr_cast(CRaster,sf.ptexTexture->prasGetTexture()),
					tc_min_src
				);


#if bAXIS_SWAP
				// Store the matrix necessary to transform a light direction from object to the
				// axis-swapped texture space.
				pi.pmpPolygon()->pmx3ObjToTexture = SwapMatrices.pmx3GetMatrix(pi.d3Normal());
#endif

				continue;
			}

			//
			// If we get to here we did not find the curved map in the virtual image, or there is
			// no virtual image.
			//

			// Get a pointer to the texture surface as a bump map.
			// This dynamic cast will alwyas fail if the parent is within the virtual image
			// but the curved section cannot be found. The imaged parent cannot be cast to a
			// bump.
			rptr<CBumpMap> pbump = rptr_dynamic_cast(CBumpMap, pras);

			// did we get a bump map from the above cast
			Assert(pbump);

			// Make a unique raster for the segement of the bumpmap to be curved.
			rptr<CBumpMap> pbump_new = pbumpCreateUniqueBumpap
			(
				pi,
				u4_hash,
				pbump
			);
			//Assert(rptr_cast(CRaster, pbump_new) == pi.ptexTexture->prasTexture);
			Assert(pbump_new != pbump);

			//
			// Store the colour values.
			//

			// Create a temporary array for storage.
			uint u_pixels = pbump_new->iLinePixels * pbump_new->iHeight;
			uint u_width = pbump_new->iLinePixels;
			uint u_height = pbump_new->iHeight;
			Assert(u_pixels);
			Assert(u_width);
			Assert(u_height);

			CBumpAnglePair* pbang_store = new CBumpAnglePair[u_pixels];
			CBumpAnglePair* pbang_map   = (CBumpAnglePair*)pbump_new->pSurface;
			Assert(pbang_map);
			Assert(pbang_store);

			// Copy.
			{
				pbump_new->Lock();
				for (uint u = 0; u < u_pixels; ++u)
				{
					CBumpAnglePair bang_temp = pbang_map[u];
					pbang_map[u].SetColour(0);
					pbang_store[u] = bang_temp;
				}
				pbump_new->Unlock();
			}

#if (0)
			// Create a CRenderPolygon for CDrawPolygon.
			CRenderPolygon rp;

			Assert(pi.iNumVertices() <= iMAX_VERTICES);
			SRenderVertex arvVertices[iMAX_VERTICES];
			SRenderVertex* aprvVertices[iMAX_VERTICES];

			// Assign the polygon's vertex pointer array.
			rp.paprvPolyVertices = PArray(pi.iNumVertices(), aprvVertices);
			rp.ptexTexture = rptr_new CTexture(pbump_new);

			for (int i = 0; i < pi.iNumVertices(); i++)
			{
				aprvVertices[i] = &arvVertices[i];

				// Extend the texture coordinates to do a "Fat Fill."
				FatFill(tc_0, tc_1, tc_2, pbump_new);

				arvVertices[i].tcTex = pi.tcTexCoord(i);
				arvVertices[i].v3Screen.tZ = 0.001;
				arvVertices[i].pdir3Normal = &pi.d3Normal(i);
			}

			//
			// Use a standard rasterizing routine to draw a triangle with interpolated curvature on
			// the bumpmap surface. Assume that there is no Z-buffer.
			//
			CDrawPolygon< CLineBumpMake<CBumpAnglePair> >
			(
				pbump_new,
				ptr<CRaster>(),
				rp
			);
#else
			// Loop through the triangles of this polygon.
			for (int i_tri = 0; i_tri < pi.iNumVertices() - 2; i_tri++)
			{
				CTexCoord tc_0 = pi.tcTexCoord(0);
				CTexCoord tc_1 = pi.tcTexCoord(1 + i_tri);
				CTexCoord tc_2 = pi.tcTexCoord(2 + i_tri);

				// Get the vertex normals.
				CDir3<> d3_0    = pi.d3Normal(0);
				CDir3<> d3_1    = pi.d3Normal(1 + i_tri);
				CDir3<> d3_2    = pi.d3Normal(2 + i_tri);
				CDir3<> d3_face = pi.d3Normal();

				// Transform the vertex normals to texture space.
				d3_0    *= mx3_obj_to_tex;
				d3_1    *= mx3_obj_to_tex;
				d3_2    *= mx3_obj_to_tex;
				d3_face *= mx3_obj_to_tex;

				if (!bBadTextureCoords)
				{
					// Extend the texture coordinates to do a "Fat Fill."
					FatFill(tc_0, tc_1, tc_2, pbump_new);
				}

#if bAXIS_SWAP
				//
				// Calculate final transformation matrix.  This transforms bumps from flat texture
				// space into object space, then swaps any axes necessary to get it pointing mostly
				// vertical.
				//
				CMatrix3<> mx3_reverse = mx3_obj_to_tex.mx3Transpose();
				SwapAxesVertical(mx3_reverse, pi.d3Normal());
#else
				// Do not transform bumps.
				CMatrix3<> mx3_reverse;
#endif

				#if bSUBDIVIDE_BUMPS

					// Render the curve.
					if (rSignedArea(tc_0, tc_1, tc_2) < 0)
						pbump_new->DrawBumps(tc_0, d3_0, tc_1, d3_1, tc_2, d3_2, d3_face,
											 mx3_reverse);
					else
						pbump_new->DrawBumps(tc_2, d3_2, tc_1, d3_1, tc_0, d3_0, d3_face,
											 mx3_reverse);

				#else

					// Render the curve.
					if (rSignedArea(tc_0, tc_1, tc_2) < 0)
						pbump_new->DrawBump(tc_0, d3_0, tc_1, d3_1, tc_2, d3_2,
											mx3_reverse);
					else
						pbump_new->DrawBump(tc_2, d3_2, tc_1, d3_1, tc_0, d3_0,
											mx3_reverse);
				#endif // bSUBDIVIDE_BUMPS
			}
#endif

			pbump_new->Lock();

#if (0)
			//
			// Restore the colour values and grow the curved area by 1 pixel in X and Y.
			//
			{
				bool	b_col;
				uint	u_offset;

				//
				// PASS 1 of the colour restore restores just the curved pixels in the source
				// map and grows by 1 pixel on the X axis. In doing this the bCurvedFlag gets
				// destroyed as this flag is stored in place of the colour information.
				//
				u_offset = 0;
				for (uint y = 0; y<u_height; y++)
				{
					b_col = false;
					for (uint x = 0; x<u_width; x++)
					{
						if (b_col)
						{
							//
							// we are in a curved section...
							//

							if (pbang_map[u_offset].bGetCurveFlag())
							{
								// we are in a curved section and this pixel is curved so just
								// copy it as is...
								pbang_map[u_offset].SetColour(pbang_store[u_offset].u1GetColour());
							}
							else
							{
								// we are in a curved section and thsi pixel is not curved, must be
								// the end, copy the previous pixel to get the angle correct and then
								// set teh colour form the source.
								if (x<u_width)
								{
									pbang_map[u_offset] = pbang_map[u_offset-1];
									pbang_map[u_offset].SetColour(pbang_store[u_offset].u1GetColour());
								}

								b_col = false;
							}
						}
						else
						{
							//
							// we are not currently in a curved section..
							//

							if (pbang_map[u_offset].bGetCurveFlag())
							{
								pbang_map[u_offset].SetColour(pbang_store[u_offset].u1GetColour());

								// the current pixel curved so we need to also copy to the pixel before
								// as long as we do not go out of the raster...
								// We copy the whole bump pixel so we get curvature that is a duplicate of
								// the previous pixel, this is better than no curvature which what causes
								// the bump map errors at the joins...We copy the colour data for the
								// new pixel from the original map.
								if (x>0)
								{
									pbang_map[u_offset-1] = pbang_map[u_offset];
									pbang_map[u_offset-1].SetColour(pbang_store[u_offset-1].u1GetColour());
								}

								b_col = true;
							}
						}

						u_offset++;
					}
				}

				//
				// PASS 2: Scans the bit map vertically growing the pixels out by 1. Angle data
				// for the new pixels is obtained from the neighbour bur the colour data is
				// obtained from the same pixel in the source map.
				//
				for (uint x = 0; x<u_width; x++)
				{
					b_col = false;

					for (uint y = 0; y<u_height; y++)	
					{
						u_offset = (y*u_width) + x;

						if (b_col)
						{
							//
							// we are in a coloured section...
							//

							if (pbang_map[u_offset].u1GetColour() == 0)
							{
								// we are in a coloured section and this pixel is not coloured, must be
								// the end, copy the previous pixel to get the angle correct and then
								// set the colour form the source.
								if (y<u_height)
								{
									pbang_map[u_offset] = pbang_map[u_offset-u_width];
									pbang_map[u_offset].SetColour(pbang_store[u_offset].u1GetColour());
								}

								b_col = false;
							}
						}
						else
						{
							//
							// we are not currently in a coloured section..
							//
							if (pbang_map[u_offset].u1GetColour())
							{
								// the current pixel is coloured so we need to copy to the pixel before
								// as long as we do not go out of the raster...
								// We copy the whole bump pixel so we get curvature that is a duplicate of
								// the previous pixel, this is better than no curvature which what causes
								// the bump map errors at the joins...We copy the colour data for the
								// new pixel from the original map.
								if (y>0)
								{
									pbang_map[u_offset-u_width] = pbang_map[u_offset];
									pbang_map[u_offset-u_width].
										SetColour(pbang_store[u_offset-u_width].u1GetColour());
								}

								b_col = true;
							}
						}
					}
				}
			}
#else
			{
				for (uint u = 0; u < u_pixels; ++u)
				{
					if (pbang_map[u].bGetCurveFlag())
					{
						//Assert(pbang_store[u].u1GetColour());
						pbang_map[u].SetColour(pbang_store[u].u1GetColour());
					}
				}
			}
#endif

			pbump_new->Unlock();
			delete[] pbang_store;

#if bAXIS_SWAP
			// Store the matrix necessary to transform a light direction from object to the
			// axis-swapped texture space.
			pi.pmpPolygon()->pmx3ObjToTexture = SwapMatrices.pmx3GetMatrix(pi.d3Normal());
#endif

		}
	}
#if VER_DEBUG
	pmsh->Validate();
#endif
}


//*****************************************************************************************
double dGetRadians(CTexCoord tc_a, CTexCoord tc_b)
{
	float dx = tc_b.tX - tc_a.tX;
	float dy = tc_b.tY - tc_a.tY;

	return fmod((atan2(dy, dx) + d2_PI), d2_PI);
}


//*****************************************************************************************
double dAverageAngles(double d_a, double d_b)
{
	if (d_a > d_b)
		Swap(d_a, d_b);
	
	double d_average = (d_a + d_b) / 2.0;

	if (d_b - d_a > dPI)
		return fmod(d_average + dPI, d2_PI);

	return d_average;
}


//*****************************************************************************************
void Extend(CTexCoord& tc_0, CTexCoord tc_1, CTexCoord tc_2, double d_radius = 2.0)
{
	double d_ang = dAverageAngles
	(
		dGetRadians(tc_1, tc_0),
		dGetRadians(tc_2, tc_0)
	);
	tc_0.tX += float(cos(d_ang) * d_radius);
	tc_0.tY += float(sin(d_ang) * d_radius);
}


//*****************************************************************************************
void FatFill(CTexCoord& tc_0, CTexCoord& tc_1, CTexCoord& tc_2, rptr<CBumpMap> pbump)
{
	CTexCoord tc_d_0,  tc_d_1,  tc_d_2;		// Texture coordinates in destination coordinates.
	CTexCoord tc_dp_0, tc_dp_1, tc_dp_2;	// New texture coordinates in destination coordinates.
	
	// Create coordinates using destination dimensions.
	tc_d_0.tX = fGetX(tc_0, rptr_cast(CRaster, pbump));
	tc_d_0.tY = fGetY(tc_0, rptr_cast(CRaster, pbump));
	tc_d_1.tX = fGetX(tc_1, rptr_cast(CRaster, pbump));
	tc_d_1.tY = fGetY(tc_1, rptr_cast(CRaster, pbump));
	tc_d_2.tX = fGetX(tc_2, rptr_cast(CRaster, pbump));
	tc_d_2.tY = fGetY(tc_2, rptr_cast(CRaster, pbump));

	tc_dp_0 = tc_d_0;
	tc_dp_1 = tc_d_1;
	tc_dp_2 = tc_d_2;

	// Extend coordinates.
	Extend(tc_dp_0, tc_d_1, tc_d_2);
	Extend(tc_dp_1, tc_d_0, tc_d_2);
	Extend(tc_dp_2, tc_d_1, tc_d_0);

	// Convert texture coordinates back to virtual coordinates.
	float f_dx = 1.0f / pbump->iWidth;
	float f_dy = 1.0f / pbump->iHeight;
	tc_0.tX = tc_dp_0.tX * f_dx;
	tc_0.tY = tc_dp_0.tY * f_dy;
	tc_1.tX = tc_dp_1.tX * f_dx;
	tc_1.tY = tc_dp_1.tY * f_dy;
	tc_2.tX = tc_dp_2.tX * f_dx;
	tc_2.tY = tc_dp_2.tY * f_dy;

	// Make sure texture coordinates are within the bounds of the raster.
	SetMinMax(tc_0.tX, 0.0f, 1.0f);
	SetMinMax(tc_0.tY, 0.0f, 1.0f);
	SetMinMax(tc_1.tX, 0.0f, 1.0f);
	SetMinMax(tc_1.tY, 0.0f, 1.0f);
	SetMinMax(tc_2.tX, 0.0f, 1.0f);
	SetMinMax(tc_2.tY, 0.0f, 1.0f);
}


//*****************************************************************************************
void SwapAxesVertical(CVector3<>& v3, const CDir3<>& d3_control)
{
	if (Abs(d3_control.tX) >= Abs(d3_control.tY))
	{
		if (Abs(d3_control.tX) >= Abs(d3_control.tZ))
		{
			// X is main axis; swap with Z.
			Swap(v3.tX, v3.tZ);
			if (d3_control.tX < 0)
			{
				// And it's negative, so negate the two.
				v3.tX = -v3.tX;
				v3.tZ = -v3.tZ;
			}
		}
		else if (d3_control.tZ < 0)
		{
			// Z is main axis, and negative; so negate Z.
			v3.tZ = -v3.tZ;
		}
	}
	else 
	{
		if (Abs(d3_control.tY) >= Abs(d3_control.tZ))
		{
			// Y is main axis; swap with Z.
			Swap(v3.tY, v3.tZ);
			if (d3_control.tY < 0)
			{
				// And it's negative, so negate the two.
				v3.tY = -v3.tY;
				v3.tZ = -v3.tZ;
			}
		}
		else if (d3_control.tZ < 0)
		{
			// Z is main axis, and negative; so negate Z.
			v3.tZ = -v3.tZ;
		}
	}

#if VER_DEBUG
	static bool b_flag = 0;
	if (!b_flag)
	{
		b_flag = true;
		CDir3<> d3_test = d3_control;
		SwapAxesVertical(d3_test, d3_control);
		Assert(d3_test.tZ >= Abs(d3_test.tX) && d3_test.tZ >= Abs(d3_test.tY));
		SwapAxesVertical(d3_test, d3_control);
		Assert(Fuzzy(d3_test, FLT_EPSILON) == d3_control);
		b_flag = false;
	}
#endif
}


//*****************************************************************************************
void SwapAxesVertical(CMatrix3<>& mx3, const CDir3<>& d3_control)
{
	SwapAxesVertical(mx3.v3X, d3_control);
	SwapAxesVertical(mx3.v3Y, d3_control);
	SwapAxesVertical(mx3.v3Z, d3_control);
}


//*************************************************************************************
CVerticalAxisSwapMatrices::CVerticalAxisSwapMatrices()
{
	for (int i = 0; i < 6; i++)
	{
		mx3SwapMatrices[i] = CMatrix3<>();

		switch (i)
		{
		case 0:
			// Identity.
			break;
		case 1:
			// Z is main axis, and negative; so negate Z.
			mx3SwapMatrices[i].v3X.tZ = -mx3SwapMatrices[i].v3X.tZ;
			mx3SwapMatrices[i].v3Y.tZ = -mx3SwapMatrices[i].v3Y.tZ;
			mx3SwapMatrices[i].v3Z.tZ = -mx3SwapMatrices[i].v3Z.tZ;
			break;
		case 2:
			// X is main axis; swap with Z.
			Swap(mx3SwapMatrices[i].v3X.tX, mx3SwapMatrices[i].v3X.tZ);
			Swap(mx3SwapMatrices[i].v3Y.tX, mx3SwapMatrices[i].v3Y.tZ);
			Swap(mx3SwapMatrices[i].v3Z.tX, mx3SwapMatrices[i].v3Z.tZ);
			break;
		case 3:
			// X is main axis; swap with Z.
			Swap(mx3SwapMatrices[i].v3X.tX, mx3SwapMatrices[i].v3X.tZ);
			Swap(mx3SwapMatrices[i].v3Y.tX, mx3SwapMatrices[i].v3Y.tZ);
			Swap(mx3SwapMatrices[i].v3Z.tX, mx3SwapMatrices[i].v3Z.tZ);

			// And it's negative, so negate the two.
			mx3SwapMatrices[i].v3X.tX = -mx3SwapMatrices[i].v3X.tX;
			mx3SwapMatrices[i].v3Y.tX = -mx3SwapMatrices[i].v3Y.tX;
			mx3SwapMatrices[i].v3Z.tX = -mx3SwapMatrices[i].v3Z.tX;
			mx3SwapMatrices[i].v3X.tZ = -mx3SwapMatrices[i].v3X.tZ;
			mx3SwapMatrices[i].v3Y.tZ = -mx3SwapMatrices[i].v3Y.tZ;
			mx3SwapMatrices[i].v3Z.tZ = -mx3SwapMatrices[i].v3Z.tZ;
			break;
		case 4:
			// Y is main axis; swap with Z.
			Swap(mx3SwapMatrices[i].v3X.tY, mx3SwapMatrices[i].v3X.tZ);
			Swap(mx3SwapMatrices[i].v3Y.tY, mx3SwapMatrices[i].v3Y.tZ);
			Swap(mx3SwapMatrices[i].v3Z.tY, mx3SwapMatrices[i].v3Z.tZ);
			break;
		case 5:
			// Y is main axis; swap with Z.
			Swap(mx3SwapMatrices[i].v3X.tY, mx3SwapMatrices[i].v3X.tZ);
			Swap(mx3SwapMatrices[i].v3Y.tY, mx3SwapMatrices[i].v3Y.tZ);
			Swap(mx3SwapMatrices[i].v3Z.tY, mx3SwapMatrices[i].v3Z.tZ);

			// And it's negative, so negate the two.
			mx3SwapMatrices[i].v3X.tY = -mx3SwapMatrices[i].v3X.tY;
			mx3SwapMatrices[i].v3Y.tY = -mx3SwapMatrices[i].v3Y.tY;
			mx3SwapMatrices[i].v3Z.tY = -mx3SwapMatrices[i].v3Z.tY;
			mx3SwapMatrices[i].v3X.tZ = -mx3SwapMatrices[i].v3X.tZ;
			mx3SwapMatrices[i].v3Y.tZ = -mx3SwapMatrices[i].v3Y.tZ;
			mx3SwapMatrices[i].v3Z.tZ = -mx3SwapMatrices[i].v3Z.tZ;
			break;
		}
	}
}


//*************************************************************************************
CMatrix3<>* CVerticalAxisSwapMatrices::pmx3GetMatrix(const CDir3<>& d3_control)
{
	int i_matrix = 0;

	if (Abs(d3_control.tX) >= Abs(d3_control.tY))
	{
		if (Abs(d3_control.tX) >= Abs(d3_control.tZ))
		{
			// X is main axis; swap with Z.
			i_matrix = 2;

			if (d3_control.tX < 0)
			{
				// And it's negative, so negate the two.
				i_matrix = 3;
			}
		}
		else if (d3_control.tZ < 0)
		{
			// Z is main axis, and negative; so negate Z.
			i_matrix = 1;
		}
	}
	else 
	{
		if (Abs(d3_control.tY) >= Abs(d3_control.tZ))
		{
			// Y is main axis; swap with Z.
			i_matrix = 4;

			if (d3_control.tY < 0)
			{
				// And it's negative, so negate the two.
				i_matrix = 5;
			}
		}
		else if (d3_control.tZ < 0)
		{
			// Z is main axis, and negative; so negate Z.
			i_matrix = 1;
		}
	}

	return &mx3SwapMatrices[i_matrix];
}


//*************************************************************************************
CMatrix3<>* CVerticalAxisSwapMatrices::pmx3GetMatrix(int i_index)
{
	return &mx3SwapMatrices[i_index];
}



//*****************************************************************************************
//
int iHashMatrix
(
	const CMatrix3<> &mx3_mat
)
//
// Compute a unique value for a given matrix.
//
//**************************************
{
	const float f_radians = 3.14159f*2.0f;
	const int i_ang_bits = 8;
	const int i_range = 1 << i_ang_bits;
	float xrot, yrot, zrot;

	// Convert to Euler angles.
	yrot = asin(-mx3_mat.v3X.tZ);

	if (fabs(cos(yrot)) > FLT_EPSILON)
	{
		xrot = atan2(mx3_mat.v3Y.tZ, mx3_mat.v3Z.tZ);
		zrot = atan2(mx3_mat.v3X.tY, mx3_mat.v3X.tX);
	}
	else
	{
		xrot = atan2(mx3_mat.v3Y.tX, mx3_mat.v3Y.tY);
		zrot = 0.0f;
	}
	
	// Make sure they are all positive.
	if (xrot < 0.0f) xrot += f_radians;
	if (yrot < 0.0f) yrot += f_radians;
	if (zrot < 0.0f) zrot += f_radians;

	Assert(xrot >= 0.0f && xrot <= f_radians)
	Assert(yrot >= 0.0f && yrot <= f_radians)
	Assert(zrot >= 0.0f && zrot <= f_radians)

	// Cut down precision of angles.
	int i_xrot = iRound(xrot*(i_range/f_radians)) & (i_range-1);
	int i_yrot = iRound(yrot*(i_range/f_radians)) & (i_range-1);
	int i_zrot = iRound(zrot*(i_range/f_radians)) & (i_range-1);

	// Assembly into an integer.
	return (i_xrot << i_ang_bits*2) | (i_yrot << i_ang_bits) | i_zrot;
}


//*****************************************************************************************
//
void MatrixFromHash
(
	int i_hash,
	CMatrix3<> &mx3_mat
)
//
// Re-construct a matrix from a hash identifier.
//
//**************************************
{
	const float f_radians = 3.14159f*2.0f;
	const int i_ang_bits = 8;
	const int i_range = 1 << i_ang_bits;

	// Get angles from hash.
	float xrot = ((i_hash >> i_ang_bits*2) & i_range-1) * f_radians/i_range;
	float yrot = ((i_hash >> i_ang_bits) & i_range-1) * f_radians/i_range;
	float zrot = (i_hash & i_range-1) * f_radians/i_range;

	// Compute sines and cosines.
	float f_sin_x = sin(xrot);
	float f_cos_x = cos(xrot);
	float f_sin_y = sin(yrot);
	float f_cos_y = cos(yrot);
	float f_sin_z = sin(zrot);
	float f_cos_z = cos(zrot);

	// Compute matrix.
	mx3_mat.v3X.tX = f_cos_y*f_cos_z;
	mx3_mat.v3X.tY = f_cos_y*f_sin_z;
	mx3_mat.v3X.tZ = -f_sin_y;

	mx3_mat.v3Y.tX = f_sin_x*f_sin_y*f_cos_z - f_cos_x*f_sin_z;
	mx3_mat.v3Y.tY = f_sin_x*f_sin_y*f_sin_z + f_cos_x*f_cos_z;
	mx3_mat.v3Y.tZ = f_sin_x*f_cos_y;

	mx3_mat.v3Z.tX = f_cos_x*f_sin_y*f_cos_z + f_sin_x*f_sin_z;
	mx3_mat.v3Z.tY = f_cos_x*f_sin_y*f_sin_z - f_sin_x*f_cos_z;
	mx3_mat.v3Z.tZ = f_cos_x*f_cos_y;
}


//
// Set of bump map matrices.
//
typedef std::map< int, CMatrix3<>*, std::less<int> > TMapMatrixHash;
TMapMatrixHash* pmapBumpMatrices = 0;


//*****************************************************************************************
//
void FastBumpCleanup()
//
// Cleanup anything that has been allocated by bump map creation.
//
//**************************************
{
	delete pmapBumpMatrices;
	pmapBumpMatrices = 0;
}


//*****************************************************************************************
//
CMatrix3<>* pmx3GetSharedMatrix
(
	const CMatrix3<> &mx3_mat
)
//
// Either allocate or find a matrix for bump-mapping.
//
//**************************************
{
	// Create the bump matrix map if we haven't already.
	if (!pmapBumpMatrices)
	{
		pmapBumpMatrices = new TMapMatrixHash;
		AlwaysAssert(pmapBumpMatrices);
	}

	// Compute the matrix hash.
	int i_hash = iHashMatrix(mx3_mat);

	// See if we already have this matrix.
	CMatrix3<> *pmx3 = (*pmapBumpMatrices)[i_hash];

	if (!pmx3)
	{
		// Just create the damn thing.
		void *pv_mat = CLoadImageDirectory::pvAllocate(sizeof(CMatrix3<>));
		MEMLOG_ADD_COUNTER(emlBumpMatrix,1);
		pmx3 = new(pv_mat) CMatrix3<>(mx3_mat);

		#if (0)
			// Should we re-construct the matrix from the hash value?
			MatrixFromHash(i_hash, *pmx3);
		#endif

		// Add to map.
		(*pmapBumpMatrices)[i_hash] = pmx3;
	}

	return pmx3;
}


//*********************************************************************************************
//
void DrawTriangleBump
(
	CBumpMap*			prasDest,
	CRenderPolygon&		rpoly			// Info on polygon (triangle) to render.
)
//
// Routine to render a polygon so that any pixel that is overlapped by the polygon
// is drawn.  The bump map is drawn by interpolating normals.
//
// Note that this routine does not sub-pixel correct the normals.  It could compute the
// area of intersection of the polygon with the pixel to get correct normal values.  In
// practice it doesn't seem necessary.
//
//**************************************
{
	int i, cnt;
	int topi, li, ri;		// Top, current left, and current right vertex indices.
	int iy;					// Current integer y value.
	float fy;				// Current floating point y value.
	int ly, ry;				// End of left and right edges.

	int ix1, ix2;
	float recip_x;

	// Edge values.
	float x1, xslope1;
	CVector3<> v1, vslope1;
	float x2, xslope2;
	CVector3<> v2, vslope2;

	cnt = rpoly.paprvPolyVertices.uLen;

	// We are limited to triangle right now.
	AlwaysAssert(cnt == 3);
	
	// Find top vertex of polygon.
	topi = 0;
	float ymin = rpoly.paprvPolyVertices[0]->v3Screen.tY;
	for (i = 1; i < cnt; i++)
	{
		if (rpoly.paprvPolyVertices[i]->v3Screen.tY < ymin)
		{
			topi = i;
			ymin = rpoly.paprvPolyVertices[i]->v3Screen.tY;
		}
	}

	fy = ymin;
	iy = floor(ymin);

	li = ri = topi;
	ly = ry = iy-1;

	CBumpAnglePair* ptpix_screen = (CBumpAnglePair*)prasDest->pSurface + iy * prasDest->iLinePixels;

	// Draw.
	while (cnt > 0)
	{
		// Advance left edge?
		while (ly <= iy && cnt)
		{
			cnt--;
			i = li + 1;
			if (i >= rpoly.paprvPolyVertices.uLen) i = 0;

			float recip_y = 1.0f / (rpoly.paprvPolyVertices[i]->v3Screen.tY - rpoly.paprvPolyVertices[li]->v3Screen.tY);

			x1 = rpoly.paprvPolyVertices[li]->v3Screen.tX;
			xslope1 = (rpoly.paprvPolyVertices[i]->v3Screen.tX - rpoly.paprvPolyVertices[li]->v3Screen.tX) * recip_y;

			v1 = *rpoly.paprvPolyVertices[li]->pdir3Normal;
			vslope1 = ((CVector3<>)*rpoly.paprvPolyVertices[i]->pdir3Normal - 
					   (CVector3<>)*rpoly.paprvPolyVertices[li]->pdir3Normal) * recip_y;

			ly = floor(rpoly.paprvPolyVertices[i]->v3Screen.tY);
			li = i;
		}

		// Advance right edge?
		while (ry <= iy && cnt)
		{
			cnt--;
			i = ri - 1;
			if (i < 0) i = rpoly.paprvPolyVertices.uLen - 1;
	
			float recip_y = 1.0f / (rpoly.paprvPolyVertices[i]->v3Screen.tY - rpoly.paprvPolyVertices[ri]->v3Screen.tY);

			x2 = rpoly.paprvPolyVertices[ri]->v3Screen.tX;
			xslope2 = (rpoly.paprvPolyVertices[i]->v3Screen.tX - rpoly.paprvPolyVertices[ri]->v3Screen.tX) * recip_y;

			v2 = *rpoly.paprvPolyVertices[ri]->pdir3Normal;
			vslope2 = ((CVector3<>)*rpoly.paprvPolyVertices[i]->pdir3Normal - 
					   (CVector3<>)*rpoly.paprvPolyVertices[ri]->pdir3Normal) * recip_y;

			ry = floor(rpoly.paprvPolyVertices[i]->v3Screen.tY);
			ri = i;
		}

		// Fill until end of left or right edge.
		while (iy < ly && iy < ry)
		{
			// Determine the x positions where the left and right edges cross into the 
			// next scanline.
			float y_step = (iy+1) - fy;
			float next_x1 = x1 + xslope1 * y_step;
			float next_x2 = x2 + xslope2 * y_step;

			// Take the min/max of the current x value and the next x values.
			if (next_x1 < x1)
				ix1 = floor(next_x1);
			else
				ix1 = floor(x1);

			if (next_x2 > x2)
				ix2 = floor(next_x2);
			else
				ix2 = floor(x2);
	
			if (ix1 < ix2)
				recip_x = 1.0f / float(ix2 - ix1);
			else
				recip_x = 0.0f;

			vec3DeltaX = (v2 - v1) * recip_x;

			Assert(ix1 >= 0);
			Assert(ix2 < prasDest->iWidth);

			DrawLoopBumpMake(ix1 - ix2 - 1, ptpix_screen + ix2 + 1, v1);

			// Advance edge values by y_step.
			x1 += xslope1 * y_step;
			x2 += xslope2 * y_step;
			v1 += vslope1 * y_step;
			v2 += vslope2 * y_step;

			// Integer y and floating point y now match up.
			iy++;
			fy = iy;

			ptpix_screen += prasDest->iLinePixels;
		}

		// Check for zero height polygon.
		if (ri == topi)
		{
			// Left edge has consumed all the edges.
			Assert(cnt == 0);

			// Find min/max x co-ordinates.
			x1 = rpoly.paprvPolyVertices[0]->v3Screen.tX;
			x2 = rpoly.paprvPolyVertices[0]->v3Screen.tX;
			v1 = *rpoly.paprvPolyVertices[0]->pdir3Normal;
			v2 = *rpoly.paprvPolyVertices[0]->pdir3Normal;
			for (i = 1; i < rpoly.paprvPolyVertices.uLen; i++)
			{
				if (rpoly.paprvPolyVertices[i]->v3Screen.tX < x1)
				{
					x1 = rpoly.paprvPolyVertices[i]->v3Screen.tX;
					v1 = *rpoly.paprvPolyVertices[i]->pdir3Normal;
				}

				if (rpoly.paprvPolyVertices[i]->v3Screen.tX > x2)
				{
					x2 = rpoly.paprvPolyVertices[i]->v3Screen.tX;
					v2 = *rpoly.paprvPolyVertices[i]->pdir3Normal;
				}
			}

			// Draw a line between them.
			ix1 = floor(x1);
			ix2 = floor(x2);

			if (ix1 < ix2)
				recip_x = 1.0f / float(ix2 - ix1);
			else
				recip_x = 0.0f;

			vec3DeltaX = (v2 - v1) * recip_x;

			Assert(ix1 >= 0);
			Assert(ix2 < prasDest->iWidth);

			DrawLoopBumpMake(ix1 - ix2 - 1, ptpix_screen + ix2 + 1, v1);
		}
		else
		{
			float alt_x1, alt_x2;

			// Finish left over part of edge, could also be zero height edge.
			if (ly == iy && ry == iy)
			{
				// End of both edges.
				// Determine the x positions where the left and right edges end.
				alt_x1 = rpoly.paprvPolyVertices[li]->v3Screen.tX;
				alt_x2 = rpoly.paprvPolyVertices[ri]->v3Screen.tX;

				// This would need to take into account the way "fy" is treated
				// to work for N-sided polygons since the left and right edges 
				// may end at different points.
			}
			else if (ly == iy)
			{
				// End of left edge.
				fy = rpoly.paprvPolyVertices[li]->v3Screen.tY;
				alt_x1 = rpoly.paprvPolyVertices[li]->v3Screen.tX;

				// Step other edge to fy.
				alt_x2 = x2;
				x2 += xslope2 * (fy - iy);
			}
			else if (ry == iy)
			{
				// End of right edge.
				fy = rpoly.paprvPolyVertices[ri]->v3Screen.tY;
				alt_x2 = rpoly.paprvPolyVertices[ri]->v3Screen.tX;

				// Step other edge to fy.
				alt_x1 = x1;
				x1 += xslope1 * (fy - iy);
			}

			// Take the min/max of the current x value and the alternate x values.
			if (alt_x1 < x1)
				ix1 = floor(alt_x1);
			else
				ix1 = floor(x1);

			if (alt_x2 > x2)
				ix2 = floor(alt_x2);
			else
				ix2 = floor(x2);
			
			if (ix1 < ix2)
				recip_x = 1.0f / float(ix2 - ix1);
			else
				recip_x = 0.0f;

			vec3DeltaX = (v2 - v1) * recip_x;

			Assert(ix1 >= 0);
			Assert(ix2 < prasDest->iWidth);

			DrawLoopBumpMake(ix1 - ix2 - 1, ptpix_screen + ix2 + 1, v1);
		}
	}
}
