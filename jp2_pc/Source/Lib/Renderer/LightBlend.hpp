/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *		Currently hardwired for yellow (sun).
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/LightBlend.hpp                                          $
 * 
 * 19    98.07.24 2:42p Mmouni
 * Dither table memory is now dynamically allocated.
 * Added support for dithering 16-color stuff.
 * 
 * 18    98.07.17 6:41p Mmouni
 * Added light blend for alpha texture.
 * 
 * 17    98.06.18 3:55p Mmouni
 * Added support for getting color for stippled alpha.
 * 
 * 16    3/13/98 5:45p Pkeet
 * Added in settings for non-alpha water conversion.
 * 
 * 15    12/15/97 7:16p Pkeet
 * Added the 'CMesh' forward declaration.
 * 
 * 14    97.11.14 11:35a Mmouni
 * Changed dither size to 4.
 * 
 * 13    97/11/11 10:25p Pkeet
 * Added a global lightblend object for alpha water.
 * 
 * 12    97.11.11 9:54p Mmouni
 * Made changes to support 2x2 or 4x4 ordered dither based on a #define.
 * 
 * 11    97.11.07 5:43p Mmouni
 * Added support for a dithering + alpha table.
 * 
 * 10    97/11/05 2:48p Pkeet
 * Added 8 band alpha fog for terrain.
 * 
 * 9     97/11/04 2:20p Pkeet
 * Added floating point access functions for the various colours.
 * 
 * 8     97/11/03 5:54p Pkeet
 * Added the 'CreateBlend' member function. Added the 'lbAlphaTerrain' global variable.
 * 
 * 7     10/28/97 7:17p Agrant
 * Added the indexed ApplyAlphaColour function, as opposed to the name-based one.
 * 
 * 6     8/17/97 12:22a Rwyatt
 * Made the aplha table static rather than allocated so it can be directly referenced,
 * otherwise another register is required in the primitive as a table pointer
 * 
 * 5     97/08/14 6:42p Pkeet
 * Added the 'ApplyAlphaColour' global function.
 * 
 * 4     97/08/13 7:05p Pkeet
 * Cleaned up code. Added support for interleaved alpha colours. Added code for th 565 format,
 * but have not been able to get it to work correctly yet.
 * 
 * 3     97/08/11 12:04p Pkeet
 * Put creation code into the setup function. Made test work with a alpha colour number.
 * 
 * 2     97/08/09 3:11p Pkeet
 * Added a settings structure.
 * 
 * 1     97/08/06 11:53a Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_RENDERER_LIGHTBLEND_HPP
#define HEADER_RENDERER_LIGHTBLEND_HPP


#define DITHER_SIZE 4


//
// Required includes.
//
#include "Lib/View/Clut.hpp"


//
// Constants and macros.
//

// Number of settings for non-alpha water conversion.
#define iNumNonAlphaWaterSettings (32)


//
// Forward declarations.
//
class CRaster;
class CMesh;


//
// Class definitions.
//

//*********************************************************************************************
//
class CLightBlend
//
// A light blend object.
//
// Prefix: lb
//
//**************************
{
	uint uLen;
	int  iNumColours;

public:

	//*****************************************************************************************
	//
	struct SLightBlendSettings
	//
	// Settings for an alpha colour.
	//
	// Prefix: lbs
	//
	//**************************
	{
		CColour clrBlendColour;
		float   fAlpha;

		//*************************************************************************************
		//
		// Constructors.
		//

		// Default constructor.
		SLightBlendSettings()
		{
		}

		// Constructor with data.
		SLightBlendSettings(CColour clr, float f_alpha)
		{
			clrBlendColour = clr;
			fAlpha         = f_alpha;
		}

		//*************************************************************************************
		//
		// Member functions.
		//

		//*************************************************************************************
		//
		float fRed
		(
		) const
		//
		// Returns the red component of the colour setting in the range 0 to 1.
		//
		//**************************
		{
			return float(clrBlendColour.u1Red) / 255.0f;
		}

		//*************************************************************************************
		//
		float fGreen
		(
		) const
		//
		// Returns the green component of the colour setting in the range 0 to 1.
		//
		//**************************
		{
			return float(clrBlendColour.u1Green) / 255.0f;
		}

		//*************************************************************************************
		//
		float fBlue
		(
		) const
		//
		// Returns the blue component of the colour setting in the range 0 to 1.
		//
		//**************************
		{
			return float(clrBlendColour.u1Blue) / 255.0f;
		}

	};

	uint16  au2Colour[1 << 16];				// Alpha colour table.
	uint16  u2AlphaMask;					// Alpha mask.
	uint16* au2AlphaReference;				// Alpha reference.
	SLightBlendSettings lpsSettings[16];	// Settings.
	int		iBitDepth;						// The bit depth the blend is set up for.

	uint32*	au4AlphaRefDitherBuf;			// Memory for dither+alpha table.
	uint32* au4AlphaRefDither;				// Special table for conversion and dither.

public:
	
	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	// Default constructor.
	CLightBlend(int i_num_colours = 16);

	// Destructor.
	~CLightBlend();

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	uint16 u2GetAlphaMask
	(
	)
	//
	// Returns a mask that can be used for getting the valid bit pattern to use with an alpha
	// reference.
	//
	//**************************
	{
		return ~u2AlphaMask;
	}

	//*****************************************************************************************
	//
	uint16 u2GetAlphaReference
	(
		int i_alpha_num
	)
	//
	// Returns the reference bits for a given alpha number.
	//
	//**************************
	{
		return au2AlphaReference[i_alpha_num];
	}

	//*****************************************************************************************
	//
	uint16 u2GetAlphaSolidColour
	(
		int i_alpha_num
	);
	//
	// Returns the solid colour that when 50% stippled will approximate the color that 
	// would result using the normal alpha blend.
	//
	//**************************

	//*****************************************************************************************
	//
	uint32 u4GetAlphaRefDither
	(
		int x,				// Screen x.
		int y,				// Screen y.
		int fx_alpha_num	// 16.16 fixed point alhpa intensity.
	)
	//
	// Returns the reference bits for a given fractional alpha number.
	//
	//**************************
	{
		// These assume 8 shades.
		Assert(iNumColours == 8);

#if (DITHER_SIZE == 2)
		// Preserve two bits of fractional precision.
		int index = (y & 1)*64 + (x & 1)*32 + (fx_alpha_num >> 14);
#elif (DITHER_SIZE == 4)
		// Preserve four bits of fractional precision.
		int index = (y & 3)*2048 + (x & 3)*256 + (fx_alpha_num >> 12);
#endif

		return au4AlphaRefDither[index];
	}

	//*****************************************************************************************
	//
	void Setup
	(
		CRaster* pras,					// Destination raster.
		int i_num_colour = -1,			// Setup a specific colour only (if non-zero).
		bool b_create_dither = false	// Set to true to create dither table.
	);
	//
	// Constructs the alpha table for a given raster.
	//
	//**************************

	//*****************************************************************************************
	//
	void CreateBlend(CRaster* pras, CColour clr);
	//
	// Creates an alpha blend from no alpha to full alpha over the range of alpha entries
	// and using the supplied colour.
	//
	//**************************

	//*****************************************************************************************
	//
	void CreateBlendForWater(CRaster* pras);
	//
	// Creates an alpha blend from no alpha to full alpha over the range of alpha entries
	// and using the supplied colour.
	//
	//**************************

	//*****************************************************************************************
	void Test(CRaster* pras, int i_alpha_num);

private:

	//*****************************************************************************************
	void Build555_8(int i_alpha_num);

	//*****************************************************************************************
	void Build565_8(int i_alpha_num);

	//*****************************************************************************************
	void Build555(int i_alpha_num);

	//*****************************************************************************************
	void Build565(int i_alpha_num);

	//*********************************************************************************************
	//
	uint32 *au4MakeDitherTable
	(
		int shades,					// Number of shaded supported by the alpha table.
		uint16 *au2AlphaReference	// Intensiy to alpha conversion table.
	);
	//
	// Create a table for that combines ordered dither and intensity to alpha bits
	// conversion. The table is organized as a (size x shades x size)
	// table.  The fractional bits of the dither size...
	//
	//**************************************
};


//*********************************************************************************************
//
class CAlphaBlend
//
// Builds the table for conversion of 4444 alhpa textures to screen format
// for use in stipple software alpha rendering.
//
// Prefix: ab
//
//**************************
{
public:
	// 12-bit color to alhpa conversion table.
	uint16 *u2ColorToAlpha;

	// Default constructor.
	CAlphaBlend();

	// Destructor.
	~CAlphaBlend();

	//*****************************************************************************************
	//
	void Setup(CRaster* pras);
	//
	// Constructs the alpha to color table.
	//
	//**************************
};


//
// Global function prototypes.
//

//*********************************************************************************************
//
void ApplyAlphaColour
(
	CMesh& msh,				// Mesh to apply alpha colour to.
	const char* str_name	// Object's name.
);
//
// If the string starts with 'alphacol' an alpha colour will be applied to all the surfaces in
// the mesh.
//
// Note:
//		Although most of the function's contents can be cannibalized by the loader when the
//		exporter can properly handle alpha stuff, the use of this function can be considered
//		a hack and for temporary use only.
//
//**************************

//*********************************************************************************************
//
void ApplyAlphaColour
(
	CMesh& msh,				// Mesh to apply alpha colour to.
	int i_alpha_channel		// Alpha channel to apply.
);
//
//	Turns the mesh into an alpha object of the given channel.
//
//**************************

//
// Global Variables.
//

// Sun beam light table.
extern CLightBlend lbAlphaConstant;

// Terrain fog table.
extern CLightBlend lbAlphaTerrain;

// Water translucency table.
extern CLightBlend lbAlphaWater;

// Water non-alpha table.
extern CLightBlend::SLightBlendSettings lbsNonAlphaWater[iNumNonAlphaWaterSettings];

// Lookup for water conversion.
extern uint16 au2IntensityToAlpha[1 << iRAMP_SHIFT_BITS];

// Texture alpha blend.
extern CAlphaBlend abAlphaTexture;


#endif // HEADER_RENDERER_LIGHTBLEND_HPP
