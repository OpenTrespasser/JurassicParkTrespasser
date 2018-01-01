/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Defines a texture.
 *
 * Bugs:
 *
 * To do:
 *		Move pixSolid out of texture, into SSurface structure.  This allows polies to share
 *		texture info, while having different pixSolid values. (Is this a good idea, though? The
 *		CTexture type really contains both texture AND colour information.)
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Texture.hpp                                              $
 * 
 * 56    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 55    98.09.10 2:02p Mmouni
 * Added ComputeSolidColour() method to CTexture.
 * 
 * 
 * 54    9/09/98 11:59a Pkeet
 * Made texturing tracking work.
 * 
 * 53    9/08/98 8:54p Rwyatt
 * Added a class specific delete so textures can exist in either normal memory of fast heap
 * memory
 * 
 * 52    98.08.31 9:34p Mmouni
 * Added support for directly specified alpha colours.
 * 
 * 51    8/19/98 1:31p Rwyatt
 * Textures allocated at load are put into a fast heap, therefore texturesr have a ZeroRefs
 * function so they can delete themselves correctly when their ref count gets to zero. 
 * 
 * 50    8/18/98 6:10p Pkeet
 * Added the 'bNoLowRes' flag. Changed structures to be more memory efficient. Added the
 * 'PurgeD3D' member function.
 * 
 * 49    8/16/98 4:33p Pkeet
 * Added a default constructor.
 * 
 * 48    98.07.30 11:43a Mmouni
 * Switched to area based mip-mapping.
 * 
 * 47    7/29/98 8:41p Pkeet
 * Added the 'bLargeSizes' member variable.
 * 
 * 46    7/27/98 8:42p Pkeet
 * Added the 'iGetBestLinkedMipLevel' member function.
 * 
 * 45    6/08/98 8:00p Mlange
 * Removed unused constructor.
 * 
 * 44    5/11/98 12:21p Mlange
 * Added constructor.
 * 
 * 43    4/21/98 2:54p Rwyatt
 * Generate mips now takes a parameter that specifies the number of mip maps. The default
 * paramater specifies all mips be generated.
 * 
 * 42    3/19/98 5:03p Mlange
 * Made CTexture::iGetNumMipLevels() an inline function.
 * 
 * 41    3/04/98 3:00p Agrant
 * Hold mip levels in a static array in the CTexture class rather than allocating and
 * deallocating all the time.
 * 
 * 40    98/02/26 15:38 Speter
 * Replaced bCurved with erfCURVED.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_TEXTURE_HPP
#define HEADER_LIB_RENDERER_TEXTURE_HPP


//
// Required includes.
//
#include "RenderDefs.hpp"
#include "Lib/Std/Set.hpp"
#include "Lib/View/ColourBase.hpp"
#include "Lib/Transform/Vector.hpp"

//
// Macro definitions.
//

//
// Switch to track usage of textures by name.
//
// Notes:
//		Texture tracking will not work with scenes loaded with an associated swapfile as the
//		swapfile does not have bitmap names in it. Before using texture tracking, delete or
//		rename the existing swapfile.
//
#define bTRACK_TEXTURE_USAGE (0)

#if bTRACK_TEXTURE_USAGE
	#include "Lib/Sys/Textout.hpp"	
#endif // bTRACK_TEXTURE_USAGE


//
// Forward declarations.
//
class CRaster;


//
// Types.
//

// Texture coordinate.
// Prefix: tc
typedef CVector2<> CTexCoord;

// Pixel format of a texture, which can be a variety of resolutions.
typedef TPixel TTexPix;

// D3D pixel format version.
typedef uint32 TD3DPixel;


//
// Enum types.
//

//*********************************************************************************************
//
enum EMipUse
//
// Enumerates different mipmap selection methods.
//
// Prefix: mu
//
// Notes:
//		This type is used for performance testing of mipmapping and texture mapping.
//
//**************************************
{
	emuNORMAL,		// Normal use of mipmaps.
	emuSMALLEST,	// Use the smallest mip level only.
	emuNO_LARGEST	// Do not use the largest mip level.
};


//
// Constants.
//

extern const CSet<ERenderFeature> seterfDEFAULT;

//
// Class definitions.
//

//*********************************************************************************************
//
class CTexture: public CRefObj
//
// Contains texture and colour information for a surface.
//
// Prefix: tex
//
//**************************************
{
private:

	CMSArray< rptr<CRaster>, 6 > aprasTextures;	// Array of mip pointers.
												// Declared staticly to avoid memory overhead and unnecessary alloc/dealloc

#if bTRACK_TEXTURE_USAGE
	char* strTextureBmpName;		// Texture name for tracking purposes.
	int*  aiMipUseCount;			// Mipmap usage count.
#endif // bTRACK_TEXTURE_USAGE

public:

	CSet<ERenderFeature> seterfFeatures;	// Rendering features enabled for this texture.
	CPalClut*	 ppcePalClut;				// Pointer to the CLUT for the texture.
	struct
	{
		union
		{
			uint16 tpSolid;			// Pixel value to use for solid-colour rendering.
			uint16 iAlphaColour;	// Alpha colour index.
		};
		char bLargeSizes  : 1;		// Contains large rasters for textures (used as an optimization).
		char bNoLowRes    : 1;		// Flag indicates that high resolution textures should not be dropped.
		char bDirectAlpha : 1;		// Alpha is specified directly by tpSolid and d3dpixColour.
	};
	TD3DPixel    d3dpixColour;		// D3D colour.
	uint32		 u4HashValue;		// A unique value for this texture (based on name).

	static float fMipmapThreshold;	// Texel to pixel threshold for selecting mipmaps.
	static EMipUse emuMipUse;		// Mipmap use (see notes for this enum type).

public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CTexture
	(
	);

	// Initialise with a raster and a solid colour.
	CTexture
	(
		rptr<CRaster>		pras,
		const CMaterial*	pmat,
		TTexPix				tp_solid,
		CSet<ERenderFeature> seterf = seterfDEFAULT
	);
	
	// Initialise with a raster; solid colour is calculated automatically.
	CTexture
	(
		rptr<CRaster>		pras,
		const CMaterial*	pmat,
		CSet<ERenderFeature> seterf = seterfDEFAULT
	);

	// Initialise with a palette and a solid colour.
	CTexture
	(
		CPal*				ppal,
		TTexPix				tp_solid,
		const CColour&		clr_d3d,
		const CMaterial*	pmat
	);

	// Initialise with a solid colour (using a default palette).
	CTexture
	(
		CColour				clr_solid,
		const CMaterial*	pmat
	);

	// Initialise with an alpha colour index.
	CTexture
	(
		int i_alpha_color	// Alpha colour.
	);

	// Initialise with colour + alpha.
	CTexture
	(
		int i_red,			// 0-255
		int i_green,
		int i_blue,
		int i_alpha
	);

	~CTexture();


	//******************************************************************************************
	// If this texture is not in the image loader fast heap then delete its memory 
	void operator delete(void* pv);


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	int iGetNumMipLevels() const
	//
	// Returns the number of mip levels present. If 0 is returned, there are no rasters
	// associated with this texture.
	//
	//**************************************
	{
		return (int)aprasTextures.uLen;
	}

	//*****************************************************************************************
	//
	int iGetBestLinkedMipLevel
	(
		int i_miplevel
	) const;
	//
	// Returns a pointer to the best linked raster it can find. If no linked raster can be
	// found, it returns -1.
	//
	//**************************************

	//*****************************************************************************************
	//
	void PurgeD3D
	(
	);
	//
	// Purges all linked Direct3D rasters.
	//
	//**************************************

	//*****************************************************************************************
	//
	forceinline rptr<CRaster> prasGetTexture(int i_miplevel = 0) const
	//
	// Returns a pointer to a raster representing the texture at the given mip level.
	//
	//**************************************
	{
		// Prevent release mode from breaking.
		if (aprasTextures.uLen <= 0)
			return rptr0;
		else if (aprasTextures.uLen <= 1)
			i_miplevel = 0;

		// For now only return the top level.
		return aprasTextures[i_miplevel];
	}

	//*****************************************************************************************
	//
	int iSelectMipLevel(float fScreenArea, float fTextureArea) const;
	//
	// Returns a mip level given the screen area and texture area.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddMipLevel(rptr<CRaster> pras);
	//
	// Adds a raster as the next mip level.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ReassignMipLevel(rptr<CRaster> pras, int i_miplevel = 0);
	//
	// Replaces a raster for a given mip level with a newly supplied one.
	//
	//**************************************

	//*****************************************************************************************
	//
	void GenerateMipLevels
	(
		uint32 u4_smallest = 0xffffffff
	);
	//
	// Automatically generates mip maps down to the smallest level.
	// Set the u4_smallest parameter to the smallest mip you want to be generated
	//
	//**************************************

	//*****************************************************************************************
	//
	void UpdateFeatures();
	//
	// Updates feature flags based on current texture state.
	//
	//**********************************

	//*****************************************************************************************
	//
	void Validate() const;
	//
	// Ensures texture state is consistent.
	//
	//**********************************

	//*****************************************************************************************
	//
	uint32 u4GetConstColour
	(
		int i_ramp = iDefNumRampValues - 1,
		int i_fog = iDefNumDepthValues - 1
	) const;
	//
	// Returns the constant colour associated with this texture based on average pixel colour,
	// clut and pixel format.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetTextureName
	(
		const char* str_texture_bmp_name	// Bitmap file name associated with the texture.
	);
	//
	// Copies the name into the 'strTextureBmpName' member variable if the
	// 'bTRACK_TEXTURE_USAGE' is defined as 'true.'
	//
	//**************************************

	//*****************************************************************************************
	//
	void DestroyMipLevels
	(
	);
	//
	// Destroys mipmap levels, leaving only the top level mipmap.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ComputeSolidColour();
	//
	// Re-computes the solid colour based on the raster.
	//
	//**********************************

#if bTRACK_TEXTURE_USAGE

	//*****************************************************************************************
	//
	void OutputTextureUse
	(
		CConsoleBuffer& con
	) const;
	//
	// Outputs texture use to a console buffer.
	//
	//**************************************
	
#endif // bTRACK_TEXTURE_USAGE
	

private:

	//*****************************************************************************************
	void SetD3DColour();

#if bTRACK_TEXTURE_USAGE

	//*****************************************************************************************
	void CreateTracking();

	//*****************************************************************************************
	void DeleteTracking();

#endif // bTRACK_TEXTURE_USAGE

};

#endif
