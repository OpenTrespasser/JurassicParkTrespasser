/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of LoadTexture.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/LoadTexture.cpp                                            $
 * 
 * 59    9/10/98 12:22a Pkeet
 * Added an extra assert in the 'EraseTextureMap' function.
 * 
 * 58    9/10/98 12:09a Pkeet
 * Added the 'EraseTextureMap' function.
 * 
 * 57    9/09/98 12:00p Pkeet
 * Made texturing tracking work.
 * 
 * 56    9/08/98 8:53p Rwyatt
 * Put textures into image loader fast heap
 * 
 * 55    8/29/98 10:39p Pkeet
 * Added the 'BreakAllTextureLinks' function.
 * 
 * 54    8/21/98 2:25a Rwyatt
 * Added a terminal error in VER_TEST builds for textures missing in the swap file.
 * 
 * 53    8/20/98 5:38p Mmouni
 * Changes for additional clut control text props.
 * 
 * 52    8/19/98 1:37p Rwyatt
 * VM Textures and rasters allocated at load time are put into a fast heap to help reduce the
 * number of allocations.
 * 
 * 51    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 50    98.07.17 6:33p Mmouni
 * ptexReadTexture() now checks for palette before messing with it.
 * 
 * 49    7/08/98 12:14p Rwyatt
 * changed texture instancing to use name and material. This allows different materials with the
 * same rasters.
 * 
 * 48    98.07.08 12:10p Mmouni
 * Now sets the pixel format correctly when loading bump maps from images.
 * 
 * 47    98.06.12 3:32p Mmouni
 * Added stuff to support CLUT start and stop colors on a per quantization group basis.
 * 
 * 46    4/21/98 3:14p Rwyatt
 * Renamed a enum variable
 * 
 * 45    3/17/98 1:41p Agrant
 * replaced !VER_FINAL with VER_TEST
 * 
 * 44    3/06/98 6:59p Agrant
 * oops.  better assert.
 * 
 * 43    3/06/98 6:52p Agrant
 * Assert and message for maps bigger than 256 by 256
 * 
 * 42    3/04/98 2:58p Agrant
 * more stats
 * 
 * 41    3/02/98 8:19p Rwyatt
 * Mip level colouring
 * 
 * 40    3/02/98 5:50p Agrant
 * Faster texture lookups and more profiling
 * 
 * 39    98/02/26 14:04 Speter
 * Removed obsolete InstanceData.hpp, and bLoadTexture functions. Moved seterfFeatures to
 * CTexture from SSurface, removing redundant flags.
 * 
 * 38    2/19/98 8:43p Agrant
 * made texture instancing map non-static so the loader can see it to remove unused textures.
 * 
 * 37    2/18/98 1:05p Rwyatt
 * Added ImageLoader.hpp header file
 * removed the always assert yet again from the image directory lookup
 * 
 * 36    2/09/98 12:24p Rwyatt
 * Fixed palette instancing bug while using virtual loader
 * 
 * 35    2/03/98 2:24p Rwyatt
 * Removed AlwaysAssert so if a file is not found in the image it will be loaded via the
 * conventional method.
 * 
 * 34    1/29/98 7:35p Rwyatt
 * Occlude textures must be present in the map directory so they get imaged,
 * New load method that tries to image the texure in the virtual image before loading the
 * conventional way.
 * 
 * 33    1/19/98 7:30p Pkeet
 * Added support for 16 bit bumpmaps by adding lower colour resolution and a smaller size for
 * 'CBumpAnglePair.'
 * 
 * 32    97/10/12 22:16 Speter
 * CTexture() now takes CMaterial* parameter.  SSurface() doesn't.
 * 
 * 31    97/10/01 4:42p Pkeet
 * Textures loaded with the name given by the 'strTEXTURE_OCCLUDE' string will be textures for
 * signalling occlusion.
 * 
 * 30    9/04/97 3:56p Pkeet
 * Added texture use tracking data.
 * 
 * 29    9/01/97 8:10p Rwyatt
 * Re-added andrews bump map palette fix
 * 
 ***********************************************************************************************/

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "LoadTexture.hpp"
#include "ImageLoader.hpp"
#include "Loader.hpp"
#include "TextureManager.hpp"
#include "Lib/Std/StringEx.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/Renderer/Primitives/FastBump.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/Sys/VirtualMem.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Std/StringEx.hpp"
#include "Lib/Std/Hash.hpp"
#include <map>
#include <string>

#include "Lib/Sys/Profile.hpp"


//
// Macros.
//

// Switch to force transparency always off.
#define bALWAYS_TRANSPARENCY     (false)

// Extension for textures associated with bumpmaps.
const char* strTEXTURE_EXTENSION = "bmp";

// Name for textures used to signal occluding polygons.
const char* strTEXTURE_OCCLUDE   = "Soccludet2.bmp";


//
// Module variables.
//

//
// String to bitmap instancer map.
// Since this is a map of rptrs, all objects will be deleted properly upon the map's destruction.
//
typedef std::map< uint32, rptr<CTexture>, std::less<uint32> > TMapTexture;
TMapTexture mapTextures;


//
// Function Implementations.
//

//*********************************************************************************************
//
static rptr<CRaster> prasGetBitmap
(
	const char* str_filename			// File to read.
)
//
// Returns:
//		The raster if found, else 0.
//
// Notes:
//		This function searches in relative directories if the filename as given
//		does not exist.
//
//**************************************
{
	Assert(str_filename);
	Assert(*str_filename);

	SetCacheText(str_filename);

	rptr<CRaster> pras = prasReadBMP(str_filename);

	if (!pras)
	{
		// Try ignoring leading path components.
		const char* str_new;

		while (str_new = strpbrk(str_filename, ":/\\"))
		{
			if (str_new == str_filename)
				str_new++;

			str_filename = str_new;

			pras = prasReadBMP(str_filename);
			
			if (pras)
				break;
		}
	}

	AlwaysAssert(pras);
	Assert(pras->iWidth  > 0);
	Assert(pras->iHeight > 0);

	if (pras->iWidth > 256 || pras->iHeight > 256)
	{
		// Map larger than 256!
		Assert(false);

		dout << str_filename << "  -->  bigger than 256: " << pras->iWidth << " by " << pras->iHeight << "!\n";
	}


	return pras;
}

//*********************************************************************************************
//
bool bTransparent
(
	rptr<CRaster> pras					// Raster to test.
)
//
// Returns:
//		Whether the raster has any transparent (0) pixels.
//
//**************************************
{
	if (bALWAYS_TRANSPARENCY)
		return true;

	if (pras)
	{
		switch (pras->iPixelBits)
		{
			case 8:
			{
				// Search for transparent (zero) pixels.
				for (int i_y = 0; i_y < pras->iHeight; i_y++)
				{
					uint8* pu1 = (uint8*)pras->pAddress(0, i_y);
					for (int i_x = 0; i_x < pras->iWidth; i_x++)
						if (pu1[i_x] == 0)
							return true;
				}
				break;
			}
			case 16:
			{
				// Search for transparent (zero) pixels.
				for (int i_y = 0; i_y < pras->iHeight; i_y++)
				{
					uint16* pu2 = (uint16*)pras->pAddress(0, i_y);
					for (int i_x = 0; i_x < pras->iWidth; i_x++)
						if (pu2[i_x] == 0)
							return true;
				}
				break;
			}
			default:
				Assert(false);
		}
	}

	return false;
}


//*********************************************************************************************
// Textures are not put into the packed surfaces by this function. This is because when loading
// a bump map we have no knowledge of its use or if it is going to be curved.
// All packing is done within the GenerateMipLevels function, this will pack the original
// texture, the generate the mip map in the correct method for the type of texture and then
// pack the new mip-map texture.
//
// Bump maps cannot be packed until AFTER they have been curved. This is because packed rasters
// are of type CRasterMem which is fine for textures and non curved bump maps. Bump maps that
// are going to be curved MUST exist as CRasterBump so the curvature code can get access to
// the matrix contained within the bump map. Once the map has been curved and all the mip
// levels have been generated they can be packed as CRasterMem structures as the matrix is no
// longer needed.
//
static rptr<CTexture> ptexReadTexture
(
	const char* str_texture_name,		// File name of texture to read.
	const char* str_bumpmap_name,		// File name of bumpmap to read.
	float f_bumpiness,					// Bumpiness to apply if a bump map.
	const CMaterial* pmat,				// Material for clut construction.
	uint32 u4_hash,						// hash id
	const SPalClutControl& pccSettings	// Clut start/stop and reflection settings.
)
//
// Returns:
//		The texture if found, else 0.
//
// Notes:
//		Does processing necessary to convert CRaster to CTexture:
//		Determines whether transparent, and reads additional bumpmap raster if present.
//
//**************************************
{
	Assert(str_texture_name);

	// Determine if the texture is a special case for use in signaling occluding polygons.
	bool b_occlude = _strcmpi(strFindLastSlash(str_texture_name), strTEXTURE_OCCLUDE) == 0;

	//
	// Check to see if the file uses a heightfield extension, and load it as a bumpmap if
	// it does.
	//
	rptr<CRaster> pras;
	bool b_transparent;

	rptr<CTexture> ptex;

	// Set the bumpmapping flag based on having a valid filename.
	bool b_bumpmapped = CLoadWorld::bBumps && str_bumpmap_name != 0;
	b_bumpmapped = b_bumpmapped && *str_bumpmap_name;

	if (b_bumpmapped)
	{
		conLoadLog.Print("BUMP_LOAD: [bump map = %s] [texture = %s]\n", str_bumpmap_name, str_texture_name);

		// Read in the bitmap representing the bumpmap heightfield.
		rptr<CRaster> pras_height = prasGetBitmap(str_bumpmap_name);

		Assert(pras_height);

		// Bumpmaps don't need palettes.
		delete pras_height->pxf.ppalAttached;
		pras_height->pxf.ppalAttached = 0;

		// Read in the texture map.
		rptr<CRaster> pras_texture = prasGetBitmap(str_texture_name);
		Assert(pras_texture);

		b_transparent = bTransparent(pras_texture);

		pras = rptr_cast(CRaster, rptr_new CBumpMap(pras_height, pras_texture, f_bumpiness));

		if (pras->pxf.ppalAttached)
		{
			// Add CLUT start, stop, and reflection colors to Palette.
			pras->pxf.ppalAttached->pccSettings = pccSettings;
		}

		MEMLOG_ADD_COUNTER(emlBumpMapCount, 1);
	}
	else
	{
		// Read regular texture map.
		conLoadLog.Print("TEXTURE_LOAD: [texture = %s]\n", str_texture_name);

		pras = prasGetBitmap(str_texture_name);
		Assert(pras);

		if (pras->pxf.ppalAttached)
		{
			// Add CLUT start, stop, and reflection colors to Palette.
			pras->pxf.ppalAttached->pccSettings = pccSettings;
		}

		MEMLOG_ADD_COUNTER(emlTextureCount, 1);

		b_transparent = bTransparent(pras);
	}

	// Set the features for this texture.
	CSet<ERenderFeature> seterf = seterfDEFAULT;

	seterf[erfOCCLUDE]		= b_occlude;
	seterf[erfTRANSPARENT]	= b_transparent;
	seterf[erfBUMP]			= b_bumpmapped;
	
	ptex = rptr_new CTexture(pras, pmat, seterf);

	// set the unique hash value for this texture
	ptex->u4HashValue = u4_hash;

	// Set texture name for texture tracking.
	ptex->SetTextureName(str_texture_name);

	return ptex;
}


//*****************************************************************************************
//
rptr<CTexture> ptexImageTexture
(
	SDirectoryFileChunk*	pdfc,	// Structure that specified virtual memory location
	const CMaterial* pmat			// Material for clut construction.
)
//
//*************************************
{
	CCycleTimer ctmr;
	CCycleTimer ctmr_all;

	rptr<CRasterMem>	prasmem;

	// Allocate space in the loader fast heap for this raster.
	void* pv_raster = CLoadImageDirectory::pvAllocate( sizeof(CRasterMem) );

	Assert( gtxmTexMan.pvmeTextures->pvGetBase() );

	//
	// Create a raster that points to the existing texture in the VM image
	//
	prasmem = rptr_cast( CRasterMem, rptr_new(pv_raster) CRasterMem(
														((char*)gtxmTexMan.pvmeTextures->pvGetBase()) + pdfc->u4VMOffset,
														pdfc->u4Width, 
														pdfc->u4Height, 
														pdfc->u4Bits,
														pdfc->u4Stride,
														NULL,
														emtTexManVirtual
													   ) );

	if (pdfc->iBumpMap && pdfc->u4Palette != 0xffffffff)
	{
		// Force the pixel format to be 8-bit (paletted), so that colour conversion functions work.
		prasmem->pxf = CPixelFormatBumpmap();
	}

	extern CProfileStat psImageRaster;
	psImageRaster.Add(ctmr(), 0);
	if (pdfc->u4Palette != 0xffffffff)
	{
		//
		// Do not attach the instanced palette, recreate it
		//
		CPal*	ppal_inst = CLoadImageDirectory::plidImageDir->ppalGetPalette(pdfc->u4Palette);

		CPal*	ppal = new CPal(ppal_inst->aclrPalette.uLen);

		// set the hash value of the new palette
		ppal->SetHashValue( ppal_inst->u4GetHashValue() );

		// copy the palette entries to the new palette
		for (int i = 0; i<ppal_inst->aclrPalette.uLen; i++)
		{
			ppal->aclrPalette[i] = ppal_inst->aclrPalette[i];
		}

		// attach the new palette to the raster
		prasmem->AttachPalette(ppal);
	}
	extern CProfileStat psImagePalette;
	psImagePalette.Add(ctmr(), 0);

	// convert the CColour in the pdfc structure into a screen specific pixel value
	TPixel pix = prasmem->pixFromColour(pdfc->clrConstCol);

	CSet<ERenderFeature> seterf = seterfDEFAULT;
	seterf[erfTRANSPARENT] = pdfc->iTransparent;
	seterf[erfBUMP] = pdfc->iBumpMap;

	ctmr();

	void* pv_tex = CLoadImageDirectory::pvAllocate( sizeof(CTexture) );
	rptr<CTexture> ptex = rptr_new(pv_tex) CTexture(rptr_static_cast(CRaster, prasmem), 
												pmat, pix, seterf);

	extern CProfileStat psImageNewTexture;
	psImageNewTexture.Add(ctmr(), 0);

	// set the unique hash value for this texture from the file directory, a textures Id is the bottom
	// 32 bits of the directory hash value. The top 8 bit are used to identify mip rasters of the same
	// texture.
	ptex->u4HashValue = (uint32)(pdfc->u8HashValue);

	extern CProfileStat psImageTexture;
	psImageTexture.Add(ctmr_all(), 0);


	return ptex;
}


//*****************************************************************************************
//
uint32 u4HashTextureData
(
	const char* str_inst_name,
	const CMaterial* pmat
)
//*************************************
{	
	u4Hash(str_inst_name);

	return u4HashContinue(pmat, sizeof(CMaterial*), false);
}


//*****************************************************************************************
//
rptr<CTexture> ptexGetTexture
(
	const char* str_texture_name,		// Name of texture to read or get.
	const char* str_bumpmap_file,		// Bumpmap file name.
	float f_bumpiness,					// Bumpiness to apply if a bump map.
	const CMaterial* pmat,				// Material used by this texture.
	const SPalClutControl& pccSettings	// Clut start/stop and reflection settings.
)
//
// Returns:
//		The texture, previously read or read from disk.
//
// Notes:
//		Makes use of the mapTextures map to instance the textures.
//
//**************************************
{
	//
	// Build a unique name for the texture by combining the texture name and bumpmap name.
	//

	// Allocate memory for the new name.
	char str_inst_mapname[1024];
	Assert(strlen(str_texture_name) + strlen(str_bumpmap_file) + 1 < 1024);

	// Combine the texture and bumpmap names.
	strcpy(str_inst_mapname, str_texture_name);
	strcat(str_inst_mapname, str_bumpmap_file);
	Assert(*str_inst_mapname);

	// Remove case sensitivity and forward/backward slash sensitivity.
	for (int i = strlen(str_inst_mapname) - 1; i >= 0; --i)
	{
		if (str_inst_mapname[i] == '\\')
			str_inst_mapname[i] = '/';
		else
		{
			str_inst_mapname[i] = tolower(str_inst_mapname[i]);
		}
	}

	CCycleTimer ctmr;
	// Try and find the texture in the map.

	// generate the unique hash value for this texture, texture IDs are only 32 bits
	// the top 32 bits stored within the hash value in the image directory are for
	// mip levels. All mip levels have an identical bottom 32 bits so it is easy
	// to identify them as the same texture.
	uint32 u4_hash = u4HashTextureData(str_inst_mapname, pmat);

	rptr<CTexture> ptex = mapTextures[u4_hash];

	extern CProfileStat psTextureLookup;
	psTextureLookup.Add(ctmr(),0);


	if (!ptex)
	{
		bool b_imaged = false;


		// is there a valid VM image file open??
		if (CLoadImageDirectory::bImageValid())
		{
			// yes there is a valid image, is our map in it
			SDirectoryFileChunk*	pdfc;
			pdfc = CLoadImageDirectory::plidImageDir->mapChunk[(uint64)u4_hash];

			if (pdfc)
			{
				// we have found the map in the image file
				ptex = ptexImageTexture(pdfc, pmat);

				// if we successfully constructed a texture from the image then do
				// not load by conventional means.
				if (ptex)
				{
					ptex->seterfFeatures[erfOCCLUDE] = pdfc->iOcclusion;
					b_imaged = true;
				}
			}
		}

		if (!b_imaged)
		{
#if VER_TEST
			if (CLoadImageDirectory::bImageValid())
			{
				char str_buffer[1024];
				sprintf(str_buffer, 
						"%s\n\nWarning: Texture '%s' Not in swap file..\n", 
						__FILE__, 
						str_texture_name );

				bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__);
			}
#endif

			// Load a bitmap(s) through conventional means..
			ptex = ptexReadTexture(str_texture_name, str_bumpmap_file, f_bumpiness, pmat, 
								   u4_hash, pccSettings);
		}

		// Add it to the map.
		mapTextures[u4_hash] = ptex;
		//mapTextureNames[u4_hash] = str_texture_name;
	}

	Assert(ptex);
	return ptex;
}

//*********************************************************************************************
//
// Global functions.
//

//*********************************************************************************************
void LoadColour(rptr<CMesh> pmsh, CColour clr, const CMaterial* pmat)
{
	Assert(pmsh->pasfSurfaces.uLen == 1);

	void* pv_tex = CLoadImageDirectory::pvAllocate( sizeof(CTexture) );
	pmsh->pasfSurfaces[0] = rptr_new(pv_tex) CTexture(clr, pmat);
}

//*********************************************************************************************
void DumpTextureLogFile()
{
/*	CConsoleBuffer con_texture_log;	// Text log.
	map< uint32, string, less<uint32> >::iterator it_maptex;	// Iterator.

	//
	// Write the texture map names to a console buffer.
	//

	// Open a texture log file with default settings.
	con_texture_log.OpenFileSession("TextureLog.txt");
	con_texture_log.SetTabSize(4);
	con_texture_log.Print("Loaded textures...\n\n");

	for (it_maptex = mapTextureNames.begin(); it_maptex != mapTextureNames.end(); it_maptex++)
	{
		con_texture_log.Print("%d\t",(*it_maptex).first);
		con_texture_log.Print((*it_maptex).second.c_str());
		con_texture_log.Print("\n");
	}

	// Close and write the file session.
	con_texture_log.Print("\nFinished!");
	con_texture_log.CloseFileSession();*/
}

//*********************************************************************************************
void BreakAllTextureLinks()
{
	TMapTexture::iterator it = mapTextures.begin();
	for (; it != mapTextures.end(); ++it)
		(*it).second->PurgeD3D();
}

//*****************************************************************************************
void DumpTextureUse()
{
#if bTRACK_TEXTURE_USAGE

	CConsoleBuffer conTextureTracker;

	conTextureTracker.SetActive(true);
	conTextureTracker.OpenFileSession("TextureUse.txt");

	TMapTexture::iterator it = mapTextures.begin();
	for (; it != mapTextures.end(); ++it)
		(*it).second->OutputTextureUse(conTextureTracker);

	conTextureTracker.CloseFileSession();

#endif // bTRACK_TEXTURE_USAGE
}

//*********************************************************************************************
void EraseTextureMap()
{
	TMapTexture::iterator it = mapTextures.begin();
	for (; it != mapTextures.end(); ++it)
		Assert((*it).second->uRefs == 1);
	mapTextures.erase(mapTextures.begin(), mapTextures.end());
}