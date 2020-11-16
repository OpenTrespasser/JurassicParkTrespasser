/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Texture.hpp
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Texture.cpp                                              $
 * 
 * 110   98.09.10 2:02p Mmouni
 * Added ComputeSolidColour() method to CTexture.
 * 
 * 
 * 109   9/10/98 12:52a Pkeet
 * Disabled the extra deletion code.
 * 
 * 108   9/10/98 12:41a Pkeet
 * Added destructor functions to the overloaded delete member function.
 * 
 * 107   9/09/98 11:59a Pkeet
 * Made texturing tracking work.
 * 
 * 106   9/08/98 8:54p Rwyatt
 * Added a class specific delete so textures can exist in either normal memory of fast heap
 * memory
 * 
 * 105   9/05/98 10:35p Agrant
 * get rid of symbol length warning
 * 
 * 104   98.08.31 9:34p Mmouni
 * Added support for directly specified alpha colours.
 * 
 * 103   8/29/98 9:33p Pkeet
 * Implemented the 'PurgeD3D' function.
 * 
 * 102   8/27/98 1:52p Asouth
 * loop variable re-scoped
 * 
 * 101   98.08.23 4:03p Mmouni
 * Changed mip-map threshold.
 * 
 * 100   8/21/98 2:25a Rwyatt
 * Added a terminal error in VER_TEST builds for textures missing in the swap file.
 * 
 * 99    8/19/98 2:33p Rwyatt
 * Fixed merge fuck up
 * 
 * 98    8/19/98 1:31p Rwyatt
 * Textures allocated at load are put into a fast heap, therefore texturesr have a ZeroRefs
 * function so they can delete themselves correctly when their ref count gets to zero. 
 * 
 * 97    8/18/98 6:11p Pkeet
 * Added the 'bNoLowRes' flag. Added the 'PurgeD3D' member function.
 * 
 * 96    8/16/98 11:29p Pkeet
 * Enabled tracking only for textures with rasters.
 * 
 * 95    8/16/98 5:48p Pkeet
 * Set necessary features for the default constructor.
 * 
 * 94    8/16/98 4:33p Pkeet
 * Added a default constructor.
 * 
 * 93    8/15/98 6:06p Mmouni
 * Fixed error "==" instead of "=".
 * 
 * 92    8/13/98 1:39p Rwyatt
 * Added memory logs for rasters and textures
 * 
 * 91    98.07.30 6:16p Mmouni
 * Adjusted mip-map threshold to 1.2 (slightly biased towards higher resolution).
 * 
 * 90    98.07.30 11:43a Mmouni
 * Switched to area based mip-mapping.
 * 
 * 89    7/29/98 8:41p Pkeet
 * Added the 'bLargeSizes' member variable.
 * 
 * 88    7/29/98 10:49a Rwyatt
 * Added a render flag for textures that have had mip maps generated.
 * 
 * 87    7/27/98 8:43p Pkeet
 * Added the 'iGetBestLinkedMipLevel' member function.
 * 
 * 86    98.07.27 7:10p Mmouni
 * Made it so that textures that don't get mip-mapped have the flag set that causes them no to
 * get paged out.
 * 
 * 85    7/26/98 7:39p Pkeet
 * Simplified colour conversions to Direct3D format.
 * 
 * 84    7/23/98 10:07p Pkeet
 * Inverted the alpha colour for hardware.
 * 
 * 83    98.07.17 6:42p Mmouni
 * Now sets flags for 16-bit textures based on the pixel format.
 * 
 * 82    98.07.10 4:43p Mmouni
 * Curved bump-map mip-maps are now expanded by one pixel.
 * 
 * 81    7/02/98 7:11p Rwyatt
 * New VM allocation stratergy for curved bump maps
 * 
 * 80    6/08/98 8:00p Mlange
 * Removed unused constructor.
 * 
 * 79    98.05.18 3:00p Mmouni
 * Fixed problem with flat shaded color for 16-bit bump maps.
 * 
 * 78    98.05.17 6:08p Mmouni
 * Fixed assert in GenerateMipLevels for textures without bitmaps when a swap file was
 * available.
 * 
 * 77    5/11/98 12:21p Mlange
 * Added constructor.
 * 
 * 76    4/21/98 2:53p Rwyatt
 * The GenerateMips function now takes a parameter that specifies how many mip maps you want.
 * The lowest three mips, no matter what their size, are in non-pageable memory.
 * 
 * 75    3/19/98 5:03p Mlange
 * Made CTexture::iGetNumMipLevels() an inline function.
 *
 **********************************************************************************************/

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Texture.hpp"

#include "Lib/Renderer/LightBlend.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/FastBump.hpp"
#include "Lib/Renderer/Primitives/FastBumpTable.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/View/RasterFile.hpp"
#include "Lib/Loader/TextureManager.hpp"
#include "Lib/Loader/ImageLoader.hpp"
#include "Lib/Sys/VirtualMem.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#if bTRACK_TEXTURE_USAGE
	#include <memory.h>
	int iTextureCount = 0;	// Count of the total number of textures created.
#endif // bTRACK_TEXTURE_USAGE


#pragma warning(disable: 4786)

// Add this pragma to make sure seterfDEFAULT is initialised before use.
#pragma warning(disable:4073)
#pragma init_seg(lib)


extern CProfileStat psTextureConstruct;
extern CProfileStat psTextureClut;

//
// Module specific functions.
//

//*********************************************************************************************
//
inline uint8 uAverage8
(
	rptr<CRaster> pras,			// Raster to sample from.
	int           i_x_corner,	// Corner in units of i_dim.
	int           i_y_corner,
	int           i_dim,			// Sample size.
	int           i_width_source,
	int           i_height_source,
	int           i_stride_source,
	uint8*        pu1_source,
	CColour*      aclr,
	bool		  b_trans
)
//
// Returns a pixel representing the average colour of the pixels at the square defined by
// i_x_corner * i_dim, i_y_corner * i_dim, i_x_corner * i_dim + i_dim and i_y_corner * i_dim +
// i_dim.
//
//**********************************
{
	CColour clr_out;
	CColour clr;
	int i_red             = 0;
	int i_green           = 0;
	int i_blue            = 0;
	int i_num_opaque      = 0;
	int i_num_transparent = 0;

	// Get the real corner.
	i_x_corner *= i_dim;
	i_y_corner *= i_dim;

	// Get the end corners.
	int i_x_corner_end = i_x_corner + i_dim;
	int i_y_corner_end = i_y_corner + i_dim;

	if (i_x_corner_end > i_width_source)
		i_x_corner_end = i_width_source;

	if (i_y_corner_end > i_height_source)
		i_y_corner_end = i_height_source;

	pu1_source += i_y_corner * i_stride_source;

	// Loop.
	for (int i_y = i_y_corner; i_y < i_y_corner_end; ++i_y)
	{
		for (int i_x = i_x_corner; i_x < i_x_corner_end; ++i_x)
		{
			// Extract the pixel index value.
			uint u1 = pu1_source[i_x];

			// If the colour is transparent, don't include it in the average.
			if (u1)
			{
				// Get the rgb colour.
				//clr = pras->pxf.clrFromPixel(u1);
				clr = aclr[u1];

				// Sum rgb values.
				i_red   += clr.u1Red;
				i_green += clr.u1Green;
				i_blue  += clr.u1Blue;
				++i_num_opaque;
			}
			else
			{
				++i_num_transparent;
			}
		}
		pu1_source += i_stride_source;
	}

	// If there are more transparent than opaque pixels, return transparency.
	if ((i_num_transparent > i_num_opaque) && (b_trans))
		return 0;

	if (i_num_opaque == 0)
		return 0;

	// Otherwise return the average colour.
	clr.u1Red   = uint8(i_red   / i_num_opaque);
	clr.u1Green = uint8(i_green / i_num_opaque);
	clr.u1Blue  = uint8(i_blue  / i_num_opaque);

	// Insert averaged value.
	return pras->pxf.pixFromColour(clr);
}


//*********************************************************************************************
inline CBumpAnglePair bangAverage
(
	rptr<CRaster>	pras,			// Raster to sample from.
	int				i_x_corner,		// Corner in units of i_dim.
	int				i_y_corner,		
	int				i_dim,			// Sample size.
	bool			b_transparent	// texture is transparent
)
//
// Returns a pixel representing the average colour of the pixels at the square defined by
// i_x_corner * i_dim, i_y_corner * i_dim, i_x_corner * i_dim + i_dim and i_y_corner * i_dim +
// i_dim as well as the bump angle defined by the average of the normals of that region.
//
//**********************************
{
	CColour clr_out;
	CColour clr;
	int i_red             = 0;
	int i_green           = 0;
	int i_blue            = 0;
	int i_num_opaque      = 0;
	int i_num_transparent = 0;
	CVector3<>              v3_normal(0.0f, 0.0f, 0.0f);

	// Get the real corner.
	i_x_corner *= i_dim;
	i_y_corner *= i_dim;

	// Loop.
	for (int i_x = i_x_corner; i_x < i_x_corner + i_dim; ++i_x)
		for (int i_y = i_y_corner; i_y < i_y_corner + i_dim; ++i_y)
		{
			// Extract the pixel index value.
			CBumpAnglePair bang = pras->pixGet(i_x, i_y);

			// Extract the colour value.
			uint8 u1 = bang.u1GetColour();

			// If the colour is transparent, don't include it in the average.
			if (u1)
			{
				// Get the rgb colour.  
				clr = pras->pxf.clrFromPixel(bang);

				// Sum rgb values.
				i_red   += clr.u1Red;
				i_green += clr.u1Green;
				i_blue  += clr.u1Blue;

				// Get the normal and sum it.
				v3_normal += bang.dir3MakeNormalFast();

				// Increment the opaque count.
				++i_num_opaque;
			}
			else
			{
				++i_num_transparent;
			}
		}

	// If there are more transparent than opaque pixels, return transparency.
	if ((i_num_transparent > i_num_opaque) && (b_transparent))
		return 0;

	if (i_num_opaque == 0)
		return 0;

	// Otherwise return the average colour.
	clr.u1Red   = uint8(i_red   / i_num_opaque);
	clr.u1Green = uint8(i_green / i_num_opaque);
	clr.u1Blue  = uint8(i_blue  / i_num_opaque);

	// Get the average normal.
	if (v3_normal.tX == 0.0f && v3_normal.tY == 0.0f && v3_normal.tZ == 0.0f)
		v3_normal = CVector3<>(0.0f, 1.0f, 0.0f);
	CDir3<> dir3 = CDir3<>(v3_normal);

	// Create a return value.
	CBumpAnglePair bang_ret = pras->pxf.pixFromColour(clr);
	
	// Convert the average normal to a bump angle and add it to the return value.
	bang_ret.SetBump(dir3, false);

	return bang_ret;
}


//*****************************************************************************************
//
TPixel pixAverage
(
	rptr<CRaster> pras				// Any raster.
)
//
// Returns:
//		A pixel representing the average colour of the raster.
//
//**********************************
{
	// Add up all the colours, storing in uint32s for range and precision.
	uint32 u4_red   = 0;
	uint32 u4_green = 0;
	uint32 u4_blue  = 0;
	uint32 u4_count = 0;

	pras->Lock();
	int i_y;
	for (i_y = 0; i_y < pras->iHeight; i_y += 4)
	{
		int i_index = pras->iIndex(0, i_y);
		for (int i_x = i_y & 3; i_x < pras->iWidth; i_x += 2)
		{
			// Get the pixels.
			TPixel pix = pras->pixGet(i_index + i_x);

			// Don't include transparent pixels in the count.
			if (pix)
			{
				CColour clr = pras->clrFromPixel(pix);
				u4_red   += clr.u1Red;
				u4_green += clr.u1Green;
				u4_blue  += clr.u1Blue;
				u4_count++;
			}
		}
	}

	// If no opaque pixels are found, get desperate.
	if (u4_count == 0)
	{
		for (i_y = 0; i_y < pras->iHeight; i_y++)
		{
			int i_index = pras->iIndex(0, i_y);
			for (int i_x = 0; i_x < pras->iWidth; i_x++)
			{
				// Get the pixels.
				TPixel pix = pras->pixGet(i_index + i_x);

				// Don't include transparent pixels in the count.
				if (pix)
				{
					CColour clr = pras->clrFromPixel(pix);
					u4_red   += clr.u1Red;
					u4_green += clr.u1Green;
					u4_blue  += clr.u1Blue;
					u4_count++;
				}
			}
		}
	}
	pras->Unlock();

	if (u4_count)
	{
		// Divide by the number of pixels counted.
		u4_red   /= u4_count;
		u4_green /= u4_count;
		u4_blue  /= u4_count;
	}
	else
	{
		// Otherwise, make a nice, average colour.
		u4_red   = 128;
		u4_green = 128;
		u4_blue  = 128;
	}

	// Convert colour to a pixel in this raster.
	return pras->pixFromColour(CColour((int)u4_red, (int)u4_green, (int)u4_blue));
}


//
// Module specific function prototypes.
//

//*********************************************************************************************
void Average8(rptr<CRaster> pras_dest, rptr<CRaster> pras_source, int i_shift, bool b_trans);

//*********************************************************************************************
void AverageBump(rptr<CRaster> pras_dest, rptr<CRaster> pras_source, int i_shift, bool b_trans);

//*********************************************************************************************
void GrowBumpEdges(rptr<CRaster> pras_new);


//
// Class implementations.
//

//*********************************************************************************************
//
// CTexture implementation.
//

	//
	// The render flags used by default.  Enable only those that are common, and will not be
	// automatically enabled by the texture attributes.
	//

	const CSet<ERenderFeature> seterfDEFAULT =
		Set(erfPERSPECTIVE) + erfZ_BUFFER + erfTRAPEZOIDS + erfSUBPIXEL + 
		erfLIGHT + erfLIGHT_SHADE + erfFOG + erfFOG_SHADE;

	//*****************************************************************************************
	// Default constructor.
	CTexture::CTexture()
	{
		bNoLowRes = false;
		bLargeSizes = false;
		bDirectAlpha = false;
		seterfFeatures = Set(erfTRAPEZOIDS);
		ppcePalClut = 0;
	}

	//*****************************************************************************************
	// Initialise with a raster and a solid colour.
	CTexture::CTexture(rptr<CRaster> pras, const CMaterial* pmat, TTexPix tp_solid, CSet<ERenderFeature> seterf)
		: seterfFeatures(seterf), tpSolid(tp_solid), bLargeSizes(false), bNoLowRes(false), bDirectAlpha(false)
	{
		CCycleTimer ctmr;

		Assert(tpSolid != 0);
		
		CCycleTimer ctmr_clut;
		if (pras->pxf.ppalAttached)
		{
			ppcePalClut = pcdbMain.ppceAddEntry(pras->pxf.ppalAttached, pmat);
			Assert(ppcePalClut);
			pras->pxf.ppalAttached = ppcePalClut->ppalPalette;
			Assert(pras->pxf.ppalAttached);
		}
		else
			ppcePalClut = pcdbMain.pceMainPalClut;

		psTextureClut.Add(ctmr_clut(),0);

		// Create the bump map table here.
		if (seterfFeatures[erfBUMP] && !ppcePalClut->pBumpTable)
			ppcePalClut->pBumpTable = new CBumpTable(ppcePalClut->pclutClut);

		//
		// Attach the texture as a single mip level.
		//
		Assert(aprasTextures.uLen == 0);
		aprasTextures << pras;

		// Set the texture features.
		UpdateFeatures();

		// Set the colour for use by hardware.
		SetD3DColour();

	#if bTRACK_TEXTURE_USAGE
		strTextureBmpName = 0;
		aiMipUseCount     = 0;
		++iTextureCount;
	#endif // bTRACK_TEXTURE_USAGE

		psTextureConstruct.Add(ctmr());

		MEMLOG_ADD_COUNTER(emlCTexture,1);
	}

	//*****************************************************************************************
	// Initialise with a raster; solid colour is calculated automatically.
	CTexture::CTexture(rptr<CRaster> pras, const CMaterial* pmat, CSet<ERenderFeature> seterf)
	{
		new(this) CTexture(pras, pmat, pixAverage(pras), seterf);
	}

	//*****************************************************************************************
	// Initialise with a palette and a solid colour.
	CTexture::CTexture(CPal* ppal, TTexPix tp_solid, const CColour& clr_d3d, const CMaterial* pmat)
		: seterfFeatures(seterfDEFAULT), tpSolid(tp_solid), bLargeSizes(false), 
		  bNoLowRes(false), bDirectAlpha(false)
	{
		CCycleTimer ctmr;
		Assert(tpSolid != 0);
		Assert(ppal);

		CCycleTimer ctmr_clut;
		ppcePalClut = pcdbMain.ppceAddEntry(ppal, pmat);
		Assert(ppcePalClut);
		psTextureClut.Add(ctmr(),0);

		d3dpixColour = clr_d3d.d3dcolGetD3DColour();

	#if bTRACK_TEXTURE_USAGE
		strTextureBmpName = 0;
		aiMipUseCount     = 0;
		++iTextureCount;
	#endif // bTRACK_TEXTURE_USAGE

		seterfFeatures[erfSPECULAR] = pmat->rvSpecular != 0;

		// Check for pre-lit surface.  If no diffuse or specular component, there is no need for vertex lighting.
		if (pmat->rvDiffuse == 0 && pmat->rvSpecular == 0)
			seterfFeatures[erfLIGHT_SHADE] = 0;

		#if VER_DEBUG
			Validate();
		#endif

		psTextureConstruct.Add(ctmr());

		MEMLOG_ADD_COUNTER(emlCTexture,1);
	}

	//*****************************************************************************************
	// Initialise with a solid colour (using a default palette).
	CTexture::CTexture(CColour clr_solid, const CMaterial* pmat)
		: seterfFeatures(seterfDEFAULT), bLargeSizes(false), bNoLowRes(false), bDirectAlpha(false)
	{
		CCycleTimer ctmr;

		// Set the Direct3D colour.
		d3dpixColour = clr_solid.d3dcolGetD3DColour();

		// Use the default palette, adding a new clut for material if necessary.
		Assert(pcdbMain.pceMainPalClut);

		ppcePalClut = pcdbMain.ppceAddEntry(pcdbMain.pceMainPalClut->ppalPalette, pmat);
		Assert(ppcePalClut);

		// And look up the colour therein.
		tpSolid = ppcePalClut->ppalPalette->u1MatchEntry(clr_solid);
		Assert(tpSolid != 0);


		// Set the colour for use by hardware.
		const float fInv255 = 1.0f / 255.0f;

		//
		// To do:
		//		Make this operation faster using a custom macro.
		//

		// For now, set the colour to red.
		d3dpixColour = clr_solid.d3dcolGetD3DColour();

		seterfFeatures[erfSPECULAR] = pmat->rvSpecular != 0;

		// Check for pre-lit surface.  If no diffuse or specular component, there is no need for vertex lighting.
		if (pmat->rvDiffuse == 0 && pmat->rvSpecular == 0)
			seterfFeatures[erfLIGHT_SHADE] = 0;

		#if VER_DEBUG
			Validate();
		#endif

	#if bTRACK_TEXTURE_USAGE
		strTextureBmpName = 0;
		aiMipUseCount     = 0;
		++iTextureCount;
	#endif // bTRACK_TEXTURE_USAGE

		psTextureConstruct.Add(ctmr());

		MEMLOG_ADD_COUNTER(emlCTexture,1);
	}

	//*****************************************************************************************
	// Initialise with an alpha colour index.
	CTexture::CTexture(int i_alpha_color)
		: seterfFeatures(seterfDEFAULT + erfALPHA_COLOUR), iAlphaColour(i_alpha_color),
		  bLargeSizes(false), bNoLowRes(false), bDirectAlpha(false)
	{
		CCycleTimer ctmr;

		CLightBlend::SLightBlendSettings* plbs;	// Pointer to the settings for the alpha.

		// Turn off all lighting for any special surfaces.
		seterfFeatures[erfLIGHT][erfLIGHT_SHADE][erfSPECULAR] = 0;

		#if VER_DEBUG
			Validate();
		#endif

		ppcePalClut = 0;

		//
		// Set the alpha colour value.
		//

		// Get the light blend settings for the alpha colour.
		plbs = &lbAlphaConstant.lpsSettings[iAlphaColour];
		Assert(plbs);

		// Use the D3D colour macro.
		d3dpixColour = D3DRGBA
		(
			plbs->fRed(),
			plbs->fGreen(),
			plbs->fBlue(),
			plbs->fAlpha
		);

	#if bTRACK_TEXTURE_USAGE
		strTextureBmpName = 0;
		aiMipUseCount     = 0;
		++iTextureCount;
	#endif // bTRACK_TEXTURE_USAGE

		psTextureConstruct.Add(ctmr());

		MEMLOG_ADD_COUNTER(emlCTexture,1);
	}

	//*****************************************************************************************
	// Initialise with colour + alpha.
	CTexture::CTexture(int i_red, int i_green, int i_blue, int i_alpha)
		: seterfFeatures(seterfDEFAULT + erfALPHA_COLOUR), bLargeSizes(false), 
		  bNoLowRes(false), bDirectAlpha(true)
	{
		CCycleTimer ctmr;

		// Turn off all lighting for any special surfaces.
		seterfFeatures[erfLIGHT][erfLIGHT_SHADE][erfSPECULAR] = 0;

		#if VER_DEBUG
			Validate();
		#endif

		ppcePalClut = 0;

		// Set the solid version of the alpha colour value.
		tpSolid = prasMainScreen->pixFromColour(CColour(i_red, i_green, i_blue));

		// Use the D3D colour macro.
		d3dpixColour = D3DRGBA
		(
			i_red   / 255.0,
			i_green / 255.0,
			i_blue  / 255.0,
			i_alpha / 255.0
		);

	#if bTRACK_TEXTURE_USAGE
		strTextureBmpName = 0;
		aiMipUseCount     = 0;
		++iTextureCount;
	#endif // bTRACK_TEXTURE_USAGE

		psTextureConstruct.Add(ctmr());

		MEMLOG_ADD_COUNTER(emlCTexture,1);
	}

	//*****************************************************************************************
	CTexture::~CTexture()
	{
		PurgeD3D();
		for (uint u = 0; u < aprasTextures.uLen; ++u)
			aprasTextures[u] = rptr0;
	#if bTRACK_TEXTURE_USAGE
		if (aprasTextures.uLen)
			DeleteTracking();
	#endif // bTRACK_TEXTURE_USAGE
		MEMLOG_SUB_COUNTER(emlCTexture,1);
	}

	//******************************************************************************************
	// If this texture is not in the image loader fast heap then delete its memory 
	void CTexture::operator delete(void* pv)
	{
		if (CLoadImageDirectory::bLoadHeapAllocation(pv))
		{
			/*
			CTexture* ptex = ((CTexture*)pv);
			ptex->PurgeD3D();
			for (uint u = 0; u < ptex->aprasTextures.uLen; ++u)
				ptex->aprasTextures[u] = rptr0;
			*/
		}
		else
		{
			::delete pv;
		}
	}

	//*****************************************************************************************
	void CTexture::PurgeD3D()
	{
		for (int i = 0; i < iGetNumMipLevels(); ++i)
		{
			// Break the raster's link.
			prasGetTexture(i)->Link();
		}
	}
	
	//*****************************************************************************************
	void CTexture::SetD3DColour()
	{
		const float fInv255 = 1.0f / 255.0f;

		Assert(tpSolid != 0);
		Assert(prasMainScreen);

		// Use the texture raster to convert the value.
		if (aprasTextures[0])
		{
			CColour clr = aprasTextures[0]->pxf.clrFromPixel(tpSolid);
			d3dpixColour = clr.d3dcolGetD3DColour();
		}
		else
		{
			// Use bright red to show errors.
			d3dpixColour = D3DRGB(1, 0, 0);
		}

	}

	//*****************************************************************************************
	int CTexture::iSelectMipLevel(float fScreenArea, float fTextureArea) const
	{
		// If there are no mip levels to choose from, just use the top mip level.
		if (aprasTextures.uLen <= 1)
		{
			#if bTRACK_TEXTURE_USAGE
				// Increment tracking information for the selected mip level.
				++aiMipUseCount[0];
			#endif // bTRACK_TEXTURE_USAGE

			return 0;
		}

		// Use the smallest mipmap if required.
		if (emuMipUse == emuSMALLEST)
		{
			#if bTRACK_TEXTURE_USAGE
				// Increment tracking information for the selected mip level.
				++aiMipUseCount[aprasTextures.uLen - 1];
			#endif // bTRACK_TEXTURE_USAGE

			return aprasTextures.uLen - 1;
		}
		
		fScreenArea *= fMipmapThreshold;

		for (uint u = 0; u < aprasTextures.uLen; ++u)
		{
			// Calculate the texture area for this mip level.
			float f_tex_area = fTextureArea * aprasTextures[u]->fWidth * aprasTextures[u]->fHeight;

			// Break if the area of the texels is less than the area of the screen.
			if (f_tex_area < fScreenArea)
			{
				// Do not use the largest mipmap if not allowed.
				if (emuMipUse == emuNO_LARGEST)
				{
					if (u == 0)
						u = 1;
				}

				#if bTRACK_TEXTURE_USAGE
					// Increment tracking information for the selected mip level.
					++aiMipUseCount[u];
				#endif // bTRACK_TEXTURE_USAGE

				return u;
			}
		}

		uint u_retval = aprasTextures.uLen - 1;

		#if bTRACK_TEXTURE_USAGE
			// Increment tracking information for the selected mip level.
			++aiMipUseCount[u_retval];
		#endif // bTRACK_TEXTURE_USAGE

		return u_retval;
	}

	//*****************************************************************************************
	void CTexture::AddMipLevel(rptr<CRaster> pras)
	{
		// Create a new array one larger than the old array.
		aprasTextures << pras;
	}

	//*****************************************************************************************
	void CTexture::DestroyMipLevels()
	{
		// Do nothing if there is only one mip level.
		if (aprasTextures.uLen == 1)
			return;

		for (int i = aprasTextures.uLen - 1; i > 0; --i)
		{
			aprasTextures[i] = rptr0;
		}

		aprasTextures.uLen = 1;
	}

	//*****************************************************************************************
	void CTexture::ReassignMipLevel(rptr<CRaster> pras, int i_miplevel)
	{
		Assert(aprasTextures.uLen > i_miplevel);

		// Reassign the raster.
		aprasTextures[i_miplevel] = pras;
	}


	//*****************************************************************************************
	void CTexture::ComputeSolidColour()
	{
		// Compute solid colour from smalled mip.
		if (aprasTextures.uLen)
			tpSolid = pixAverage(aprasTextures[aprasTextures.uLen-1]);
	}

#if bSTIPPLE_MIP_LEVELS

//*********************************************************************************************
static void StippleMip(rptr<CRaster> pras_new, int i_mip_level, bool bBumpMap)
{
	uint8*	src;

	// mip non pageable mip levels are in the smallest mip colour
	if ((i_mip_level>=3) || (i_mip_level == (int)eptSMALLEST))
		i_mip_level == 5;

	pras_new->Lock();

	// copy the source raster into the reference raster

	int x, y;

	if (bBumpMap)
	{
		//
		// A CBumpAnglePair raster is a bump map so we need to extract the colour element,
		// this is usually the top byte but we will use the class just to be sure.
		// The resulting data is 8 bits per pixel and uses the palette from the
		// raster.
		//
		CBumpAnglePair	bang;

		for (y = 0; y<pras_new->iHeight; y+=1)
		{
			for (x = (y&1); x<pras_new->iWidth-(y&1); x+=2)
			{
				bang.br = pras_new->pixGet(x, y);
				// (!(b_transparent && br.u1Colour() == 0))
				if (bang.u1GetColour() != 0)
				{
					bang.SetColour(255-i_mip_level);
					pras_new->PutPixel(x,y,bang.br);
				}
			}
		}
	}
	else switch (pras_new->iPixelBits)
	{
	case 8:
		for (y = 0; y<pras_new->iHeight; y+=1)
		{
			src = (uint8*)pras_new->pAddress(0,y);
			for (x = 0 + (y&1); x<pras_new->iWidth-(y&1); x+=2)
			{
				if (src[x] != 0)
					src[x] = 255 - i_mip_level;
			}
			//src = ((uint8*)src) + pras_new->iLineBytes();
		}
		break;
	}

	pras_new->Unlock();
}

#endif


	//*****************************************************************************************
	void CTexture::GenerateMipLevels
	(
		uint32 u4_smallest
	)
	{
		CCycleTimer ctmr;
		CCycleTimer ctmr_all;
		char str_mip_name[128];
		bool b_imaged;
		int i_mip_level;
		uint32 u4_mips;

		// If there is no raster, or there already are multiple rasters, do nothing.
		if (iGetNumMipLevels() != 1)
			return;

		// This texture already has its mips created.
		if (seterfFeatures[erfMIPS_CREATED])
			return;

		// This texture has been processed for mip maps.
		seterfFeatures[erfMIPS_CREATED] = true;


		b_imaged = CLoadImageDirectory::bImageValid() &&
				   gtxmTexMan.pvmeTextures->bImageAddressValid(aprasTextures[0]->pSurface);

		u4_mips = 0;

		i_mip_level = (int)eptTOP_LEVEL;

		if (seterfFeatures[erfCURVED])
		{
			if ((aprasTextures[0]->iWidth<16) || (aprasTextures[0]->iHeight<16))
			{
				i_mip_level = (int)eptSMALLEST;
			}
		}
		else
		{
			if (u4_smallest < u4NONPAGEABLE_MIPS)
			{
				i_mip_level = (int)eptSMALLEST;
			}
		}
		

/*		if ((aprasTextures[0]->iWidth<16) || (aprasTextures[0]->iHeight<16))
		{
			// width or height are less than 16 which means that no mip maps
			// will get generated for this texture, therefore both of the
			// above checks will fail and the texture will be packed twice
			// and this will generate a hash collision in the image directory.
			// If texture 0 is already packed, do nothing.
			if (gtxmTexMan.pvmeTextures->bVirtualAddressValid(aprasTextures[0]->pSurface))
				return;
		}*/

		// Since we do not mip non-8 bit non-bumpmap textures, make sure that they will not
		// be pageable.
		if (prasGetTexture()->iPixelBits != 8 && !seterfFeatures[erfBUMP])
			i_mip_level = (int)eptSMALLEST;


		//
		// If this texture is not imaged then pack it as usual
		//
		if (!b_imaged)
		{
			// pack the original texture, if it is too big or already packed then this will return
			// what was passed in to it...
			aprasTextures[0] = gtxmTexMan.prasPackTexture(	aprasTextures[0], 
															seterfFeatures[erfCURVED], 
															(ETexturePackTypes)i_mip_level );
#if bSTIPPLE_MIP_LEVELS
			StippleMip(aprasTextures[0], 0, seterfFeatures[erfBUMP]);
#endif
			gtxmTexMan.AddToPackLog(rptr_this(this),
									aprasTextures[0], 
									(uint64)u4HashValue);
		}

		// Get the width and the height for the next mip level.
		int i_width      = prasGetTexture()->iWidth >> 1;
		int i_height     = prasGetTexture()->iHeight >> 1;
		int i_pixel_size = prasGetTexture()->iPixelBits;

		// For the time being, do nothing with non-8 bit non-bumpmap textures.
		if (i_pixel_size != 8 && !seterfFeatures[erfBUMP])
			return;

		// Choose the minimum dimension for determining mip levels.
		int i_dimension = Min(i_width, i_height);

		// Create a unique hash value for this texture.
		// the 64 bit hash value is the parent hash value with the width and height encoded.

		int i_shift = 2;

		// Build mipmaps while they are worth building.
		while (i_dimension >= 8)
		{
			// Create the raster for the new mip level.
			rptr<CRaster>	pras_new;
			bool			b_found;
			uint64			u8_hash = ((uint64)u4HashValue) | 
										(((uint64)i_width)<<32) |		// 10 bits at pos32 for X Size
										(((uint64)i_height)<<42);		// 10 bits at pos42 for y Size

			u4_mips++;
			if (u4_mips>u4_smallest)
				return;

			if (aprasTextures[0]->pxf.ppalAttached)
			{
				// if the parent has a CLUT add convert its unique ID into
				// a byte and merge it with the mips hash value.
				uint32 u4_palid = aprasTextures[0]->pxf.ppalAttached->u4GetHashValue();

				// convert the 32 bit pal ID into a byte
				u4_palid ^= (u4_palid>>15);
				u4_palid &= 0x0000ffff;
				u4_palid ^= (u4_palid>>8);
				u4_palid &= 0x00000fff;
				
				u8_hash |= ((uint64)u4_palid)<<52;						// 12 bits at pos52 for pal ID.				
			}

			// Once we have the packed a smallest texture, all child mips after that are also smallest.
			if (i_mip_level != (int)eptSMALLEST)
			{
				i_mip_level++;

				if (seterfFeatures[erfCURVED])
				{
					if ((aprasTextures[0]->iWidth<16) || (aprasTextures[0]->iHeight<16))
					{
						i_mip_level = (int)eptSMALLEST;
					}
				}
				else
				{
					if ((uint32)i_mip_level >= (uint32)(u4_smallest-3))
					{
						i_mip_level = (int)eptSMALLEST;
					}
				}
			}

			wsprintf(str_mip_name,"Mip/%x%x.mip", (uint32)(u8_hash >> 32), (uint32)(u8_hash & 0xffffffff) );

			b_found = false;

			// if we are using an image file and this texture is in it, try to locate the mip maps
			if (b_imaged)
			{
				ctmr();
				SDirectoryFileChunk*	pdfc;
				pdfc = CLoadImageDirectory::plidImageDir->mapChunk[u8_hash];

				AlwaysAssert(pdfc);

				// check the parameters from the directory file are correct
				if (pdfc)
				{
					Assert((int32)pdfc->u4Width == i_width);
					Assert((int32)pdfc->u4Height == i_height);

					void* pv_raster = CLoadImageDirectory::pvAllocate( sizeof(CRasterMem) );
					pras_new = rptr_cast( CRaster, rptr_new(pv_raster) CRasterMem(
							((char*)gtxmTexMan.pvmeTextures->pvGetBase()) + pdfc->u4VMOffset,
							pdfc->u4Width, 
							pdfc->u4Height, 
							pdfc->u4Bits,
							pdfc->u4Stride,
							NULL,
							emtTexManVirtual) );

					if (pras_new)
					{
						// copy the pxf format of the parent to the mip
						pras_new->pxf = prasGetTexture()->pxf;
						b_found = true;
					}
				}
				extern CProfileStat psNewRaster;
				psNewRaster.Add(ctmr(), 0);
			}

			// if we have not managed to located the mip in the image file load it the usuall way
			if (!b_found)
			{
#if VER_TEST
				if (CLoadImageDirectory::bImageValid())
				{
					char str_buffer[1024];
					sprintf(str_buffer, 
							"%s\n\nWarning: Mip map '%s' Not in swap file..\n", 
							__FILE__, 
							str_mip_name );

					bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__);
				}
#endif
				pras_new = rptr_cast(CRaster, rptr_new CRasterMem
				(
					i_width,
					i_height,
					i_pixel_size,
					0,
					&prasGetTexture()->pxf
				));

				// Try and load a pregenerated mipmap.
				if (!bLoadRasterMip(str_mip_name, pras_new))
				{
					dprintf("Create Mip: %s\n",str_mip_name);

					// Copy the texture over and resample.
					if (seterfFeatures[erfBUMP])
					{
						AverageBump(pras_new, prasGetTexture(), i_shift, seterfFeatures[erfTRANSPARENT]);

						// Grow bump-map edges.
						if (seterfFeatures[erfCURVED])
							GrowBumpEdges(pras_new);
					}
					else switch (i_pixel_size)
					{
						case 8:
							Average8(pras_new, prasGetTexture(), i_shift, seterfFeatures[erfTRANSPARENT]);
							break;

						default:
							Assert(0);
					}

					// Cache the mip level to a file.
					bSaveRasterMip(str_mip_name, pras_new);
				}

#if bSTIPPLE_MIP_LEVELS
				StippleMip(pras_new, i_mip_level, seterfFeatures[erfBUMP]);
#endif

				// pack the mip, this will relocate the raster in the image file log after the
				// raster has been packed.
				pras_new = gtxmTexMan.prasPackTexture(pras_new, 
									seterfFeatures[erfCURVED], (ETexturePackTypes)i_mip_level);

				gtxmTexMan.AddToPackLog(rptr_this(this), pras_new, u8_hash);

				// keep track of the correct mip stat, the check is based on the bit depth...
				MEMLOG_ADD_COUNTER((seterfFeatures[erfBUMP] ? emlBumpMipCount : emlTextureMipCount), 1);
			}

			ctmr_all();
			// Add the mip level to the existing structure.
			AddMipLevel(pras_new);
		extern CProfileStat psPreImageMip;
		psPreImageMip.Add(ctmr_all(),0);

			//
			// Generate values for the next mip level.
			//
			i_width     >>= 1;
			i_height    >>= 1;
			i_dimension >>= 1;
			i_shift     <<= 1;
		}

	#if bTRACK_TEXTURE_USAGE
		// Add tracking information if required.
		CreateTracking();
	#endif // bTRACK_TEXTURE_USAGE

	}

	//******************************************************************************************
	void CTexture::UpdateFeatures()
	{
		if (seterfFeatures[erfOCCLUDE])
		{
			// Occlusion precludes all other features.
			seterfFeatures = Set(erfOCCLUDE);
			return;
		}

		// Set flag if a texture is present.
		bool b_is_texture = iGetNumMipLevels() > 0;

		// Automatically set some flag values.
		seterfFeatures[erfTEXTURE] = b_is_texture;
		if (!b_is_texture)
			seterfFeatures[erfTRANSPARENT][erfBUMP] = false;

		//
		// A bit of a hack here.  Set some flags for any 16-bit texture that is not a bump
		// map based on the pixel format of the map.
		//
		if (b_is_texture && prasGetTexture()->iPixelBits == 16 && !seterfFeatures[erfBUMP])
		{
			if (prasGetTexture()->pxf.cposG == 0x00F0)
				seterfFeatures[erfALPHA_COLOUR] = 1;
			else
				seterfFeatures[erfCOPY] = 1;
		}

		if (seterfFeatures[erfBUMP])
			// Bump mapping is currently imcompatible with light shading.
			seterfFeatures[erfLIGHT_SHADE] = 0;

		if (ppcePalClut)
		{
			const CMaterial* pmat = ppcePalClut->pmatMaterial;

			seterfFeatures[erfSPECULAR]		= pmat->rvSpecular != 0;

			// Check for pre-lit surface.  If no diffuse or specular component, there is no need for vertex lighting.
			if (pmat->rvDiffuse == 0 && pmat->rvSpecular == 0)
				seterfFeatures[erfLIGHT_SHADE] = 0;
		}
		else
			seterfFeatures[erfSPECULAR]		= 0;

		// Turn off all lighting for any special surfaces.
		if (seterfFeatures[erfCOPY][erfALPHA_COLOUR][erfOCCLUDE] ||
		!seterfFeatures[erfLIGHT])
			seterfFeatures[erfLIGHT][erfLIGHT_SHADE][erfSPECULAR] = 0;

		#if VER_DEBUG
			Validate();
		#endif
	}
	
	//******************************************************************************************
	void CTexture::Validate() const
	{
		// To do: further implement this function.

		// If this is a bump map, make sure that the bump bit depth matches the raster.
		if (seterfFeatures[erfBUMP])
		{
			Assert(prasGetTexture()->iPixelBits == iBUMPMAP_RESOLUTION);
		}
	}

	//*****************************************************************************************
	uint32 CTexture::u4GetConstColour(int i_ramp, int i_fog) const
	{
		// If there is no raster, use the average pixel value.
		if (iGetNumMipLevels() < 1)
		{
			// If there is a clut for the texture, use it.
			if (ppcePalClut)
				return ppcePalClut->pclutClut->Convert(tpSolid & 0xFF, i_ramp, i_fog);

			// Default colour.
			return tpSolid;
		}

		// Switch based on the pixel bit depth.
		if (seterfFeatures[erfBUMP])
		{
			if (ppcePalClut)
				return ppcePalClut->pclutClut->Convert(((CBumpAnglePair&)tpSolid).u1GetColour(), i_ramp, i_fog);
		}
		else switch (prasGetTexture()->iPixelBits)
		{
			case 8:
				if (ppcePalClut)
					return ppcePalClut->pclutClut->Convert(tpSolid & 0xFF, i_ramp, i_fog);
			break;
		}

		// Default colour.
		return u2FogSolidCol[i_fog];
	}
	
	//*****************************************************************************************
	void CTexture::SetTextureName(const char* str_texture_bmp_name)
	{
	#if bTRACK_TEXTURE_USAGE

		// Delete any existing strings.
		if (strTextureBmpName)
			free(strTextureBmpName);

		// Do nothing if no string is given.
		if (str_texture_bmp_name == 0)
			return;

		// Allocate memory and copy the string to the new memory location.
		strTextureBmpName = _strdup(str_texture_bmp_name);

	#endif // bTRACK_TEXTURE_USAGE
	}

#if bTRACK_TEXTURE_USAGE

	//*****************************************************************************************
	//
	void CTexture::CreateTracking
	(
	)
	//
	// Sets up required tracking structures and initializes them.
	//
	//**************************************
	{
		// Create and zero memory for mip use count.
		aiMipUseCount = new int[aprasTextures.uLen];
		memset(aiMipUseCount, 0, sizeof(int) * aprasTextures.uLen);
	}

	//*****************************************************************************************
	//
	void CTexture::DeleteTracking
	(
	)
	//
	// If a the name exists in the 'strTextureBmpName' member variable, the name and texture
	// usage data will be output to a log. If the log does not exist, this member function
	// will generate one.
	//
	//**************************************
	{
		// Delete any existing strings.
		if (strTextureBmpName)
			free(strTextureBmpName);

		// Delete count array.
		delete[] aiMipUseCount;

		// Decrement the texture count, and close the output file if it is zero.
		--iTextureCount;
	}

	//*****************************************************************************************
	void CTexture::OutputTextureUse(CConsoleBuffer& con) const
	{
		// Dump information about the texture.
		if (strTextureBmpName && aiMipUseCount)
		{
			// Count the total mip use.
			int i_count = 0;
			for (uint u = 0; u < aprasTextures.uLen; ++u)
			{
				i_count += aiMipUseCount[u];
			}
			i_count = Max(i_count, 1);

			// Print the mip use.
			con.Print
			(
				"\n%s: %ld, %ld\n\n",
				strTextureBmpName,
				aprasTextures.uLen,
				i_count
			);
			for (u = 0; u < aprasTextures.uLen; ++u)
			{
				float f_percent = float(aiMipUseCount[u]) / float(i_count) * 100.0f;
				con.Print
				(
					"\t%ldx%ld\t%1.1f\t%ld\n",
					aprasTextures[u]->iWidth,
					aprasTextures[u]->iHeight,
					f_percent,
					aiMipUseCount[u]
				);
			}
		}
		else
		{
			//AlwaysAssert(0);
		}
	}

#endif // bTRACK_TEXTURE_USAGE

	//*****************************************************************************************
	int CTexture::iGetBestLinkedMipLevel(int i_miplevel) const
	{
		int i_test_level;

		// Prevent bad requests.
		if (i_miplevel < 0 || i_miplevel >= iGetNumMipLevels())
			i_miplevel = 0;

		// Is the mip level requested actually available?
		if (aprasTextures[i_miplevel]->prasLink)
			return i_miplevel;

		// Look for higher resolution mip levels.
		for (i_test_level = i_miplevel - 1; i_test_level >= 0; --i_test_level)
		{
			if (aprasTextures[i_test_level]->prasLink)
				return i_test_level;
		}

		// Look for lower resolution mip levels.
		for (i_test_level = i_miplevel + 1; i_test_level < iGetNumMipLevels(); ++i_test_level)
		{
			if (aprasTextures[i_test_level]->prasLink)
				return i_test_level;
		}

		// Return a code indicating failure.
		return -1;
	}


//
// Implementation of module specific functions.
//

//*********************************************************************************************
void Average8(rptr<CRaster> pras_dest, rptr<CRaster> pras_source, int i_shift, bool b_trans)
{
	// Store the pixel format.
	CPixelFormat pxf = pras_source->pxf;

	// Get the attached palette.
	CColour* aclr = pras_source->pxf.ppalAttached->aclrPalette.atArray;

	// Obtain the width, height, stride and a pointer to the surface.
	int    i_width_source  = pras_source->iWidth;
	int    i_height_source = pras_source->iHeight;
	int    i_stride_source = pras_source->iLinePixels;
	uint8* pu1_source      = (uint8*)pras_source->pSurface;

	// Loop.
	for (int i_dest_x = 0; i_dest_x < pras_dest->iWidth; ++i_dest_x)
		for (int i_dest_y = 0; i_dest_y < pras_dest->iHeight; ++i_dest_y)
		{
			// Insert averaged value.
			uint8 u1 = uAverage8
			(
				pras_source,
				i_dest_x,
				i_dest_y,
				i_shift,
				i_width_source,
				i_height_source,
				i_stride_source,
				pu1_source,
				aclr,
				b_trans
			);
			pras_dest->PutPixel(i_dest_x, i_dest_y, u1);
		}
}

//*********************************************************************************************
void AverageBump(rptr<CRaster> pras_dest, rptr<CRaster> pras_source, int i_shift, bool b_trans)
{
	// Store the pixel format.
	CPixelFormat pxf = pras_source->pxf;

	int i_line_dest    = pras_dest->iLinePixels;
	int i_line_source  = pras_source->iLinePixels;
	TBumpRes* pbr_dest   = (TBumpRes*)pras_dest->pSurface;

	uint32 i_index_dest;

	// Loop.
	for (int i_dest_x = 0; i_dest_x < pras_dest->iWidth; ++i_dest_x)
		for (int i_dest_y = 0; i_dest_y < pras_dest->iHeight; ++i_dest_y)
		{
			// Insert averaged value.
			CBumpAnglePair bang = bangAverage(pras_source, i_dest_x, i_dest_y, i_shift, b_trans);

			// For some reason, put pixel did not work.
			i_index_dest = i_dest_y * i_line_dest + i_dest_x;
			pbr_dest[i_index_dest] = bang.br;
		}
}

//*********************************************************************************************
void GrowBumpEdges(rptr<CRaster> pras_new)
{
	CBumpAnglePair	bang;

	//
	// Grow the curved area by 1 pixel in X and Y.
	//
	pras_new->Lock();

	//
	// PASS 1: Scans the bit map horizontally growing the pixels out by 1.
	//
	for (int y = 0; y < pras_new->iHeight; y++)
	{
		bool b_col = false;
		for (int x = 0; x < pras_new->iWidth; x++)
		{
			// Get pixel.
			bang.br = pras_new->pixGet(x,y);

			if (b_col)
			{
				if (bang.u1GetColour() == 0)
				{
					// Colour to no colour.

					// Copy previous pixel to this pixel.
					bang.br = pras_new->pixGet(x-1,y);
					pras_new->PutPixel(x,y,bang.br);

					b_col = false;
				}
			}
			else
			{
				if (bang.u1GetColour() != 0)
				{
					// No colour to colour.
					if (x > 0)
					{
						// Copy this pixel to previous pixel.
						pras_new->PutPixel(x-1,y,bang.br);
					}

					b_col = true;
				}
			}
		}
	}

	//
	// PASS 2: Scans the bit map vertically growing the pixels out by 1.
	//
	for (int x = 0; x < pras_new->iWidth; x++)
	{
		bool b_col = false;

		for (int y = 0; y < pras_new->iHeight; y++)	
		{
			// Get pixel.
			bang.br = pras_new->pixGet(x,y);

			if (b_col)
			{
				if (bang.u1GetColour() == 0)
				{
					// Colour to no colour.

					// Copy previous pixel to this pixel.
					bang.br = pras_new->pixGet(x,y-1);
					pras_new->PutPixel(x,y,bang.br);

					b_col = false;
				}
			}
			else
			{
				if (bang.u1GetColour() != 0)
				{
					// No colour to colour.
					if (y > 0)
					{
						// Copy this pixel to previous pixel.
						pras_new->PutPixel(x,y-1,bang.br);
					}

					b_col = true;
				}
			}
		}
	}

	pras_new->Unlock();
}


//
// Static variables.
//
float   CTexture::fMipmapThreshold = 1.44f;		// Default multiplier (higher gives higher resolution textures).
EMipUse CTexture::emuMipUse        = emuNORMAL;	// Mipmap use type.
