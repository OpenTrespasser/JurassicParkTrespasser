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
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/LightBlend.cpp                                          $
 * 
 * 39    9/22/98 10:40p Pkeet
 * Changed the overloaded '*' operator for CColour to use floats instead of doubles.
 * 
 * 38    98.09.13 7:03p Mmouni
 * Fixed CreatBlend() (for terrain fogging) so that it matches the way the CLUT's for object
 * fogging are created.
 * 
 * 37    9/08/98 8:54p Rwyatt
 * Put alpha CTextures into image loader fast heap
 * 
 * 36    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 35    98.07.24 2:42p Mmouni
 * Dither table memory is now dynamically allocated.
 * Added support for dithering 16-color stuff.
 * 
 * 34    7/23/98 10:03p Pkeet
 * Inverted alpha for hardware.
 * 
 * 33    98.07.17 6:41p Mmouni
 * Added light blend for alpha texture.
 * 
 * 32    98.06.29 5:39p Mmouni
 * Made changes to support non-alpha water settings.
 * 
 * 31    98.06.18 3:55p Mmouni
 * Added support for getting color for stippled alpha.
 * 
 * 30    6/09/98 7:10p Pkeet
 * Changed water alpha creation function to use the D3D water flag.
 * 
 * 29    98/03/27 16:13 Speter
 * Algorithmic settings now ascend evenly.
 * 
 * 28    3/13/98 5:46p Pkeet
 * Added in settings for non-alpha water conversion.
 * 
 * 27    98/02/26 13:59 Speter
 * Removed SSurface as global struct, replaced with straight rptr<CTexture>. Created
 * experimental algorithmic alpha settings.
 * 
 * 26    1/09/98 3:32p Pkeet
 * Added separate settings for a non-translucent version of the water.
 * 
 * 25    12/16/97 7:19p Pkeet
 * Fixed bug that allowed water alpha values to be set outside the settings table.
 * 
 * 24    12/16/97 5:26p Pkeet
 * Added alpha water conversion for Direct3D.
 * 
 * 23    11/16/97 12:05p Rwycko
 * Changed water alpha values for COMDEX
 * 
 * 22    97/11/11 10:25p Pkeet
 * Added a global lightblend object for alpha water.
 * 
 * 21    97.11.11 9:54p Mmouni
 * Made changes to support 2x2 or 4x4 ordered dither based on a #define.
 * 
 * 20    97/11/10 11:28a Pkeet
 * Added code to make the water work temporarily using the terrain alpha.
 * 
 * 19    97.11.07 5:43p Mmouni
 * Added support for a dithering + alpha table.
 * 
 * 18    97/11/06 6:19p Pkeet
 * Set the terrain fog alpha so the first band (band 0) applies no fog.
 * 
 * 17    97.11.05 7:49p Mmouni
 * Added u4TerrainFogMask.
 * 
 *********************************************************************************************/

#include "common.hpp"
#include "LightBlend.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Std/StringEx.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/EntityDBase/Water.hpp"
#include "Lib/Loader/ImageLoader.hpp"

#include <memory.h>
#include <stdio.h>
#include <string.h>

#define bMANUAL_SETTINGS (1)
#define bUSE_CUSTOM_NONALPHA_SETTINGS (1)


//
// Water blend alphas.
//
static float fAbsorb = 0.25;
static CColour clrWater(0, 0, 0);
static CColour clrReflect(102, 150, 164);
static CColour clrLight(232, 254, 252);

static CLightBlend::SLightBlendSettings lbsWater
(
	float f_reflect
)
{
	float f_alpha = 1.0f - (1.0f - fAbsorb) * (1.0f - f_reflect);	// 1 - transmitted (source) fraction.
	return CLightBlend::SLightBlendSettings
	(
		clrWater * (fAbsorb * (1.0f - f_reflect) / f_alpha) + 		// Absorbed (water) component.
			clrReflect * (f_reflect / f_alpha),					// Reflected (sky) component.
		f_alpha
	);
}

//
// Translucent version of the water.
//
CLightBlend::SLightBlendSettings albsWater[] =
{
#if bMANUAL_SETTINGS
	// Standard settings.
	CLightBlend::SLightBlendSettings(CColour(  0,   0,   0), 0.25f),	// 0
	CLightBlend::SLightBlendSettings(CColour( 10,  16,  17), 0.26f),	// 1
	CLightBlend::SLightBlendSettings(CColour( 18,  26,  28), 0.27f),	// 2
	CLightBlend::SLightBlendSettings(CColour( 28,  42,  45), 0.28f),	// 3
	
	CLightBlend::SLightBlendSettings(CColour( 30,  44,  48), 0.32f),	// 4
	CLightBlend::SLightBlendSettings(CColour( 40,  60,  66), 0.37f),	// 5
	CLightBlend::SLightBlendSettings(CColour( 51,  75,  81), 0.41f),	// 6
	CLightBlend::SLightBlendSettings(CColour( 58,  86,  94), 0.45f),	// 7
	
	CLightBlend::SLightBlendSettings(CColour( 67,  99, 107), 0.50f),	// 8
	CLightBlend::SLightBlendSettings(CColour( 73, 108, 118), 0.54f),	// 9
	CLightBlend::SLightBlendSettings(CColour( 87, 129, 141), 0.58f),	// 10
	CLightBlend::SLightBlendSettings(CColour( 97, 142, 156), 0.63f),	// 11
	
	CLightBlend::SLightBlendSettings(CColour(105, 155, 169), 0.67f),	// 12
	CLightBlend::SLightBlendSettings(CColour(113, 166, 181), 0.72f),	// 13
	CLightBlend::SLightBlendSettings(CColour(128, 188, 205), 0.80f),	// 14
	CLightBlend::SLightBlendSettings(CColour(232, 254, 252), 1.00f)		// 15
#else
	// Algorithmic settings.
	lbsWater(0.5 / 15.0),
	lbsWater(1.5 / 15.0),
	lbsWater(2.5 / 15.0),
	lbsWater(3.5 / 15.0),
	lbsWater(4.5 / 15.0),
	lbsWater(5.5 / 15.0),
	lbsWater(6.5 / 15.0),
	lbsWater(7.5 / 15.0),
	lbsWater(8.5 / 15.0),
	lbsWater(9.5 / 15.0),
	lbsWater(10.5 / 15.0),
	lbsWater(11.5 / 15.0),
	lbsWater(12.5 / 15.0),
	lbsWater(13.5 / 15.0),
	lbsWater(14.5 / 15.0),
	CLightBlend::SLightBlendSettings(clrLight, 15.5 / 16.0)
#endif
};

//
// Stippled version of the water.
//
CLightBlend::SLightBlendSettings lbsNonAlphaWater[32] =
{
#if bMANUAL_SETTINGS
	CLightBlend::SLightBlendSettings(CColour(  0,   0,   0), 0.25f),	// 0
	CLightBlend::SLightBlendSettings(CColour(  0,   0,   0), 0.25f),	// 1
	CLightBlend::SLightBlendSettings(CColour( 10,  16,  17), 0.26f),	// 2
	CLightBlend::SLightBlendSettings(CColour( 10,  16,  17), 0.26f),	// 3
	CLightBlend::SLightBlendSettings(CColour( 18,  26,  28), 0.27f),	// 4
	CLightBlend::SLightBlendSettings(CColour( 18,  26,  28), 0.27f),	// 5
	CLightBlend::SLightBlendSettings(CColour( 28,  42,  45), 0.28f),	// 6
	CLightBlend::SLightBlendSettings(CColour( 28,  42,  45), 0.28f),	// 7
	
	CLightBlend::SLightBlendSettings(CColour( 30,  44,  48), 0.32f),	// 8
	CLightBlend::SLightBlendSettings(CColour( 30,  44,  48), 0.32f),	// 9
	CLightBlend::SLightBlendSettings(CColour( 40,  60,  66), 0.37f),	// 10
	CLightBlend::SLightBlendSettings(CColour( 40,  60,  66), 0.37f),	// 11
	CLightBlend::SLightBlendSettings(CColour( 51,  75,  81), 0.41f),	// 12
	CLightBlend::SLightBlendSettings(CColour( 51,  75,  81), 0.41f),	// 13
	CLightBlend::SLightBlendSettings(CColour( 58,  86,  94), 0.45f),	// 14
	CLightBlend::SLightBlendSettings(CColour( 58,  86,  94), 0.45f),	// 15
	
	CLightBlend::SLightBlendSettings(CColour( 67,  99, 107), 0.50f),	// 16
	CLightBlend::SLightBlendSettings(CColour( 67,  99, 107), 0.50f),	// 17
	CLightBlend::SLightBlendSettings(CColour( 73, 108, 118), 0.54f),	// 18
	CLightBlend::SLightBlendSettings(CColour( 73, 108, 118), 0.54f),	// 19
	CLightBlend::SLightBlendSettings(CColour( 87, 129, 141), 0.58f),	// 20
	CLightBlend::SLightBlendSettings(CColour( 87, 129, 141), 0.58f),	// 21
	CLightBlend::SLightBlendSettings(CColour( 97, 142, 156), 0.63f),	// 22
	CLightBlend::SLightBlendSettings(CColour( 97, 142, 156), 0.63f),	// 23
	
	CLightBlend::SLightBlendSettings(CColour(105, 155, 169), 0.67f),	// 24
	CLightBlend::SLightBlendSettings(CColour(105, 155, 169), 0.67f),	// 25
	CLightBlend::SLightBlendSettings(CColour(113, 166, 181), 0.72f),	// 26
	CLightBlend::SLightBlendSettings(CColour(113, 166, 181), 0.72f),	// 27
	CLightBlend::SLightBlendSettings(CColour(128, 188, 205), 0.80f),	// 28
	CLightBlend::SLightBlendSettings(CColour(128, 188, 205), 0.80f),	// 29
	CLightBlend::SLightBlendSettings(CColour(232, 254, 252), 1.00f),	// 30
	CLightBlend::SLightBlendSettings(CColour(232, 254, 252), 1.00f)		// 31
#else
	// Algorithmic settings.
	lbsWater(0.5 / 31.0),
	lbsWater(1.5 / 31.0),
	lbsWater(2.5 / 31.0),
	lbsWater(3.5 / 31.0),
	lbsWater(4.5 / 31.0),
	lbsWater(5.5 / 31.0),
	lbsWater(6.5 / 31.0),
	lbsWater(7.5 / 31.0),
	lbsWater(8.5 / 31.0),
	lbsWater(9.5 / 31.0),
	lbsWater(10.5 / 31.0),
	lbsWater(11.5 / 31.0),
	lbsWater(12.5 / 31.0),
	lbsWater(13.5 / 31.0),
	lbsWater(15.5 / 31.0),
	lbsWater(16.5 / 31.0),
	lbsWater(17.5 / 31.0),
	lbsWater(18.5 / 31.0),
	lbsWater(19.5 / 31.0),
	lbsWater(20.5 / 31.0),
	lbsWater(21.5 / 31.0),
	lbsWater(22.5 / 31.0),
	lbsWater(23.5 / 31.0),
	lbsWater(24.5 / 31.0),
	lbsWater(25.5 / 31.0),
	lbsWater(26.5 / 31.0),
	lbsWater(27.5 / 31.0),
	lbsWater(28.5 / 31.0),
	lbsWater(29.5 / 31.0),
	lbsWater(30.5 / 31.0),
	CLightBlend::SLightBlendSettings(clrLight, 15.5 / 16.0)
#endif
};


//
// Constants.
//
#define uMASK_8_555  (0x8401)
#define uMASK_8_565  (0x0821)
#define uMASK_16_555 (0x8421)
#define uMASK_16_565 (0x0861)


//
// Arrays.
//
uint16 au28555[] =
{
	0x0000,	// 0
	0x0001,	// 1
	0x0400,	// 2
	0x0401,	// 3
	0x8000,	// 4
	0x8001,	// 5
	0x8400,	// 6
	0x8401,	// 7
	0x0000,	// 8
	0x0000,	// 9
	0x0000,	// 10
	0x0000,	// 11
	0x0000,	// 12
	0x0000,	// 13
	0x0000,	// 14
	0x0000	// 15
};

uint16 au28565[] =
{
	0x0000,	// 0
	0x0001,	// 1
	0x0020,	// 2
	0x0021,	// 3
	0x0800,	// 4
	0x0801,	// 5
	0x0820,	// 6
	0x0821,	// 7
	0x0000,	// 8
	0x0000,	// 9
	0x0000,	// 10
	0x0000,	// 11
	0x0000,	// 12
	0x0000,	// 13
	0x0000,	// 14
	0x0000	// 15
};

uint16 au216555[] =
{
	0x0000,	// 0
	0x0001,	// 1
	0x0020,	// 2
	0x0021,	// 3
	0x0400,	// 4
	0x0401,	// 5
	0x0420,	// 6
	0x0421,	// 7
	0x8000,	// 8
	0x8001,	// 9
	0x8020,	// 10
	0x8021,	// 11
	0x8400,	// 12
	0x8401,	// 13
	0x8420,	// 14
	0x8421,	// 15
};

uint16 au216565[] =
{
	0x0000,	// 0
	0x0001,	// 1
	0x0020,	// 2
	0x0021,	// 3
	0x0040,	// 4
	0x0041,	// 5
	0x0060,	// 6
	0x0061,	// 7
	0x0800,	// 8
	0x0801,	// 9
	0x0820,	// 10
	0x0821,	// 11
	0x0840,	// 12
	0x0841,	// 13
	0x0860,	// 14
	0x0861	// 15
};


//
// Variables.
//
uint16  u2TerrainFogMask;
uint32  u4TerrainFogMask;
uint16  u2WaterMask;
uint16* pu2WaterAlpha;


//
// Pre-defined dither matrices.
//
#if (DITHER_SIZE == 2)

static uint8 dither_matrix[4] = {	0, 192, 
								  128,  64 };

#elif (DITHER_SIZE == 4)

static uint8 dither_matrix[16] = {	0, 192,  48, 240, 
								  128,  64, 176, 112, 
								   32, 224,  16, 208, 
								  160,  96, 144,  80 };
#endif


//*********************************************************************************************
//
uint32 *CLightBlend::au4MakeDitherTable
(
	int shades,					// Number of shaded supported by the alpha table.
	uint16 *au2AlphaReference	// Intensiy to alpha conversion table.
)
//
// Create a table for that combines ordered dither and intensity to alpha bits
// conversion. The table is organized as a (size x shades x size)
// table.  The fractional bits of the dither size...
//
//**************************************
{
	uint32 mem_addr;
	uint32 *table_ptr;

#if (DITHER_SIZE == 2)
	
	const int size = 2*2;

	if (!au4AlphaRefDitherBuf)
	{
		// Allocate buffer for alpha + dither.
		// Note: we allocate twice as much as we need so that we can
		// align on a table size boundry.
		au4AlphaRefDitherBuf = new uint32[16*shades*2];
	}

	AlwaysAssert(au4AlphaRefDitherBuf);

	mem_addr = (uint32)au4AlphaRefDitherBuf;

	//
	// Insure alignment on a table sized boundry.
	// The lower bits of the address will be in the form: YXSSSFF or
	// YXSSSSFF.
	//
	int table_size = size*shades*size*sizeof(uint32);
	int align_mask = table_size-1;

	// Table size must be a power of 2.
	Assert((table_size & align_mask) == 0);

	mem_addr = (mem_addr + align_mask)& ~align_mask;

	table_ptr = (uint32 *)mem_addr;
	int index = 0;

	// Fill table.
	for (int entry = 0; entry < size; entry++)
	{
		for (int shade = 0; shade < shades; shade++)
		{
			for (int frac = 0; frac < size; frac++)
			{
				int dither_add = (dither_matrix[entry] + frac*(256/size) > 255);

				table_ptr[index++] = au2AlphaReference[shade + dither_add];
			}
		}
	}

#elif (DITHER_SIZE == 4)

	const int size = 4*4;

	if (!au4AlphaRefDitherBuf)
	{
		// Allocate buffer for alpha + dither.
		// Note: we allocate an additional amount equal to 4*width*shades*entries
		// so that we can align on this boundry.
		au4AlphaRefDitherBuf = new uint32[shades*1024+shades*256];

		// Clear it.
		memset(au4AlphaRefDitherBuf, 0, shades*1024+shades*256*sizeof(uint32));
	}

	AlwaysAssert(au4AlphaRefDitherBuf);

	mem_addr = (uint32)au4AlphaRefDitherBuf;

	//
	// Insure alignment on an 4*width*shades*entries boundry.
	// The lower bits of the address will be in the form: YY0XX0SSSFFFF
	// or YY0XX0SSSSFFFF.
	// 
	int align_size = 16*shades*size*sizeof(uint32);

	int align_mask = align_size-1;

	// Table size must be a power of 2.
	Assert((align_size & align_mask) == 0);

	mem_addr = (mem_addr + align_mask)& ~align_mask;

	table_ptr = (uint32 *)mem_addr;
	int index = 0;

	// Fill table.
	for (int entry = 0; entry < size; entry++)
	{
		// Skip every other column and every other row.
		index = (entry / 4) * 16 * shades * size + (entry % 4) * 2 * shades * size;

		for (int shade = 0; shade < shades; shade++)
		{
			for (int frac = 0; frac < size; frac++)
			{
				int dither_add = (dither_matrix[entry] + frac*(256/size) > 255);

				table_ptr[index++] = au2AlphaReference[shade + dither_add];
			}
		}
	}

#endif
	
	return table_ptr;
}


//
// Local classes.
//

//*********************************************************************************************
//
template<int iBitsColour, int iFirstBit, int iBitsAlpha> class CAlphaTranslate
//
// Table translation object.
//
// Prefix: at
//
//**************************************
{
	int   iShiftBy;
	int   iMask;
	float fMinusAlpha;
	float fAlphaCol;
	float fDivisor;
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	// Constructor.
	CAlphaTranslate(float f_alpha, float f_col)
	{
		iShiftBy    = iFirstBit + iBitsAlpha;
		iMask       = ((1 << (iBitsColour - iBitsAlpha)) - 1) << iShiftBy;
		fDivisor    = 1.0f / float((1 << (iBitsColour - iBitsAlpha)) - 1);
		float f_multiplier = float(1 << iBitsColour);
		
		fAlphaCol = f_col * f_alpha * f_multiplier + 0.01f;
		fMinusAlpha = (1.0f - f_alpha) * f_multiplier;
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	uint32 u4Translate
	(
		uint32 u4_colour	// Packed RGB value.
	) const
	//
	// Returns bits representing the alpha of a given colour.
	//
	//**************************************
	{
		float     f_col = float((u4_colour & iMask) >> iShiftBy) * fDivisor;
		float f_new_col = f_col * fMinusAlpha + fAlphaCol;

		uint32 u4_col = uint32(f_new_col);
		SetMinMax(u4_col, 0, (1 << iBitsColour) - 1);
		return u4_col << iFirstBit;
	}
};


//
// Class implementations.
//

//*********************************************************************************************
//
// Implementation of class CLightBlend
//

	//*****************************************************************************************
	CLightBlend::CLightBlend(int i_num_colours)
		: iNumColours(i_num_colours)
	{
		// Default settings.
		SLightBlendSettings albs[] =
		{
			CLightBlend::SLightBlendSettings(CColour(201, 221, 255), 0.4f),		// 0
			CLightBlend::SLightBlendSettings(CColour(201, 221, 255), 0.2f),		// 1
			CLightBlend::SLightBlendSettings(CColour(201, 221, 255), 0.1f),		// 2
			CLightBlend::SLightBlendSettings(CColour(201, 221, 255), 0.05f),	// 3
			
			CLightBlend::SLightBlendSettings(CColour(255, 255, 237), 0.2f),		// 4
			CLightBlend::SLightBlendSettings(CColour(255, 255, 237), 0.1f),		// 5
			CLightBlend::SLightBlendSettings(CColour(255, 255, 237), 0.05f),	// 6
			
			CLightBlend::SLightBlendSettings(CColour(224, 255, 255), 0.2f),		// 7
			CLightBlend::SLightBlendSettings(CColour(224, 255, 255), 0.1f),		// 8
			CLightBlend::SLightBlendSettings(CColour(224, 255, 255), 0.05f),	// 9
			
			CLightBlend::SLightBlendSettings(CColour(255, 255, 255), 0.2f),		// 10
			CLightBlend::SLightBlendSettings(CColour(255, 255, 255), 0.1f),		// 11
			CLightBlend::SLightBlendSettings(CColour(255, 255, 255), 0.05f),	// 12
			
			CLightBlend::SLightBlendSettings(CColour(128, 128, 0), 0.5f),		// 13
			CLightBlend::SLightBlendSettings(CColour(128, 128, 0), 0.5f),		// 14
			CLightBlend::SLightBlendSettings(CColour(128, 128, 0), 0.5f)		// 15
		};
		memcpy(lpsSettings, albs, sizeof(SLightBlendSettings) * 16);

		uLen = 1 << 16;
		au4AlphaRefDitherBuf = 0;
	}

	//*****************************************************************************************
	CLightBlend::~CLightBlend()
	{
		delete au4AlphaRefDitherBuf;
	}

	//*****************************************************************************************
	void CLightBlend::Setup(CRaster* pras, int i_num_colour, bool b_create_dither)
	{
		int i_bits = (pras->pxf.cposG == 0x03E0) ? (15) : (16);

		// Save for later use.
		iBitDepth = i_bits;

		// Setup for 8 colours.
		if (iNumColours == 8)
		{
			//
			// Set the appropriate alpha mask and reference structures.
			//
			if (i_bits == 15)
			{
				u2AlphaMask       = uMASK_8_555;
				au2AlphaReference = au28555;

				if (b_create_dither)
					au4AlphaRefDither = au4MakeDitherTable(8, au28555);
			}
			else
			{
				u2AlphaMask       = uMASK_8_565;
				au2AlphaReference = au28565;

				if (b_create_dither)
					au4AlphaRefDither = au4MakeDitherTable(8, au28565);
			}

			//
			// Fill in values for the table.
			//
			if (i_num_colour >= 0 && i_num_colour < iNumColours)
			{
				if (i_bits == 15)
					Build555_8(i_num_colour);
				else
					Build565_8(i_num_colour);
			}
			else
				for (int i = 0; i < iNumColours; ++i)
				{
					if (i_bits == 15)
						Build555_8(i);
					else
						Build565_8(i);
				}
			return;
		}

		// Setup for 16 colours.
		if (iNumColours == 16)
		{
			//
			// Set the appropriate alpha mask and reference structures.
			//
			if (i_bits == 15)
			{
				u2AlphaMask       = uMASK_16_555;
				au2AlphaReference = au216555;

				if (b_create_dither)
					au4AlphaRefDither = au4MakeDitherTable(16, au216555);
			}
			else
			{
				u2AlphaMask       = uMASK_16_565;
				au2AlphaReference = au216565;

				if (b_create_dither)
					au4AlphaRefDither = au4MakeDitherTable(16, au216565);
			}

			//
			// Fill in values for the table.
			//
			if (i_num_colour >= 0 && i_num_colour < iNumColours)
			{
				if (i_bits == 15)
					Build555(i_num_colour);
				else
					Build565(i_num_colour);
			}
			else
				for (int i = 0; i < iNumColours; ++i)
				{
					if (i_bits == 15)
						Build555(i);
					else
						Build565(i);
				}
			return;
		}

		Assert(0);
	}

	//*****************************************************************************************
	void CLightBlend::Build555_8(int i_alpha_num)
	{
		// Get the settings for the light.
		SLightBlendSettings* plbs = lpsSettings + i_alpha_num;
	
		CAlphaTranslate<5,  0, 1> at_blue(plbs->fAlpha, plbs->fBlue());
		CAlphaTranslate<5,  5, 0> at_green(plbs->fAlpha, plbs->fGreen());
		CAlphaTranslate<5, 10, 1> at_red(plbs->fAlpha, plbs->fRed());

		uint16 u2_reference = u2GetAlphaReference(i_alpha_num);

		// Get the mask and reference values.
		for (uint u = i_alpha_num; u < uLen; ++u)
		{
			// Check the mask.
			if ((u & u2AlphaMask) != u2_reference)
				continue;

			// Insert converted colour.
			au2Colour[u] = at_blue.u4Translate(u) | at_green.u4Translate(u) |
				           at_red.u4Translate(u);
		}
	}

	//*****************************************************************************************
	void CLightBlend::Build565_8(int i_alpha_num)
	{
		// Get the settings for the light.
		SLightBlendSettings* plbs = lpsSettings + i_alpha_num;
	
		CAlphaTranslate<5,  0, 1> at_blue(plbs->fAlpha, plbs->fBlue());
		CAlphaTranslate<6,  5, 1> at_green(plbs->fAlpha, plbs->fGreen());
		CAlphaTranslate<5, 11, 1> at_red(plbs->fAlpha, plbs->fRed());

		uint16 u2_reference = u2GetAlphaReference(i_alpha_num);

		// Get the mask and reference values.
		for (uint u = i_alpha_num; u < uLen; ++u)
		{
			// Check the mask.
			if ((u & u2AlphaMask) != u2_reference)
				continue;

			// Insert converted colour.
			au2Colour[u] = at_blue.u4Translate(u) | at_green.u4Translate(u) |
				           at_red.u4Translate(u);
		}
	}

	//*****************************************************************************************
	void CLightBlend::Build555(int i_alpha_num)
	{
		// Get the settings for the light.
		SLightBlendSettings* plbs = lpsSettings + i_alpha_num;
	
		CAlphaTranslate<5,  0, 1> at_blue(plbs->fAlpha, plbs->fBlue());
		CAlphaTranslate<5,  5, 0> at_green(plbs->fAlpha, plbs->fGreen());
		CAlphaTranslate<5, 10, 1> at_red(plbs->fAlpha, plbs->fRed());

		uint16 u2_reference = u2GetAlphaReference(i_alpha_num);

		// Get the mask and reference values.
		for (uint u = i_alpha_num; u < uLen; ++u)
		{
			// Check the mask.
			if ((u & u2AlphaMask) != u2_reference)
				continue;

			// Insert converted colour.
			au2Colour[u] = at_blue.u4Translate(u) | at_green.u4Translate(u) |
				           at_red.u4Translate(u);
		}
	}

	//*****************************************************************************************
	void CLightBlend::Build565(int i_alpha_num)
	{
		// Get the settings for the light.
		SLightBlendSettings* plbs = lpsSettings + i_alpha_num;
	
		CAlphaTranslate<5,  0, 1> at_blue(plbs->fAlpha, plbs->fBlue());
		CAlphaTranslate<6,  5, 2> at_green(plbs->fAlpha, plbs->fGreen());
		CAlphaTranslate<5, 11, 1> at_red(plbs->fAlpha, plbs->fRed());

		uint16 u2_reference = u2GetAlphaReference(i_alpha_num);

		// Get the mask and reference values.
		for (uint u = i_alpha_num; u < uLen; ++u)
		{
			// Check the mask.
			if ((u & u2AlphaMask) != u2_reference)
				continue;

			// Insert converted colour.
			au2Colour[u] = at_blue.u4Translate(u) | at_green.u4Translate(u) |
				           at_red.u4Translate(u);
		}
	}

	//*****************************************************************************************
	void CLightBlend::Test(CRaster* pras, int i_alpha_num)
	{
		int i_x    = (2 * pras->iWidth) / 3;
		int i_xlen = pras->iWidth  / 6;
		int i_y    = 0;
		int i_ylen = pras->iHeight;
		int i_line = pras->iLinePixels;
		uint16* pu2 = (uint16*)pras->pSurface;
		uint16* pu2_pixel;

		uint16  u2_mask      = u2GetAlphaMask();
		uint16  u2_reference = u2GetAlphaReference(i_alpha_num);

		pu2 += i_x + i_line * i_y;
		for (int i_y_line = 0; i_y_line < i_ylen; ++i_y_line)
		{
			for (int i_x_line = 0; i_x_line < i_xlen; ++i_x_line)
			{
				// Get the pixel.
				pu2_pixel = pu2 + i_x_line;
				uint16 u2 = *pu2_pixel;

				// Mask the pixel.
				u2 &= u2_mask;
				u2 |= u2_reference;

				// Transform and set the pixel.
				*pu2_pixel = au2Colour[u2];
				
			}
			pu2 += i_line;
		}
	}

	//*****************************************************************************************
	void CLightBlend::CreateBlend(CRaster* pras, CColour clr)
	{
		float f_alpha_step = 1.0f / float(iNumColours);
		float f_alpha = 0.0f;

		// Fog blend.
		for (int i = 0; i < iNumColours; ++i, f_alpha += f_alpha_step)
		{
			SetMinMax(f_alpha, 0.0f, 1.0f);
			lpsSettings[i] = CLightBlend::SLightBlendSettings(clr, f_alpha);
		}

		// Setup the alpha object, with dither table.
		Setup(pras, -1, true);

		// Set the terrain mask.
		u2TerrainFogMask = u2GetAlphaMask();

		// Get the zero extend DWORD version of the mask.
		u4TerrainFogMask = u2TerrainFogMask;
	}

	//*****************************************************************************************
	void CLightBlend::CreateBlendForWater(CRaster* pras)
	{
		const int i_num_intensities = 1 << iRAMP_SHIFT_BITS;

		// Copy the water settings to the object.
		memcpy(lpsSettings, albsWater, sizeof(albsWater));

		// Setup the alpha object.
		Setup(pras);

		// Set up the water mask and colour table globals.
		u2WaterMask   = u2GetAlphaMask();
		pu2WaterAlpha = &au2Colour[0];

		// Setup the intensity to alpha table.
		if (d3dDriver.bD3DWater())
		{
			// Convert for D3D format colors.
			for (int i = 0; i < i_num_intensities; ++i)
			{
				int j = (i * iNumColours) / i_num_intensities;
				au2IntensityToAlpha[i] = d3dDriver.u4ConvertToAlpha
				(
					lpsSettings[j].fRed(),
					lpsSettings[j].fGreen(),
					lpsSettings[j].fBlue(),
					lpsSettings[j].fAlpha
				);
			}
		}
		else if (CEntityWater::bAlpha)
		{
			// Software alpha format colors.
			for (int i = 0; i < i_num_intensities; ++i)
			{
				int j = (i * iNumColours) / i_num_intensities;
				au2IntensityToAlpha[i] = u2GetAlphaReference(j);
			}
		}
		else
		{
			// Screen format colors.
			for (int i = 0; i < i_num_intensities; ++i)
			{
#if (bUSE_CUSTOM_NONALPHA_SETTINGS)
				// Just use the colour from special light blend settings.
				int j = (i * 32) / i_num_intensities;

				if (iBitDepth == 15)
				{
					au2IntensityToAlpha[i] = 
						((lbsNonAlphaWater[j].clrBlendColour.u1Red << 7)   & 0x7c00) |
						((lbsNonAlphaWater[j].clrBlendColour.u1Green << 2) & 0x03e0) |
						((lbsNonAlphaWater[j].clrBlendColour.u1Blue >> 3)  & 0x001f);
				}
				else
				{
					au2IntensityToAlpha[i] = 
						((lbsNonAlphaWater[j].clrBlendColour.u1Red << 8)   & 0xf800) |
						((lbsNonAlphaWater[j].clrBlendColour.u1Green << 3) & 0x07e0) |
						((lbsNonAlphaWater[j].clrBlendColour.u1Blue >> 3)  & 0x001f);
				}
#else
				// Using same alpha settings as the alpha water.
				int j = (i * iNumColours) / i_num_intensities;
				au2IntensityToAlpha[i] = u2GetAlphaSolidColour(j);
#endif
			}
		}
	}

	//*****************************************************************************************
	//
	uint16 CLightBlend::u2GetAlphaSolidColour
	(
		int i_alpha_num
	)
	//
	// Returns the solid colour that when 50% stippled will approximate the color that 
	// would result using the normal alpha blend.
	//
	//**************************
	{
		if (iBitDepth == 15)
		{
			// Blend with light gray.
			uint16 u2_blend = ((128 << 7) & 0x7c00) |
							  ((128 << 2) & 0x03e0) |
							  ((128 >> 3) & 0x001f);

			u2_blend &= ~u2AlphaMask;
			u2_blend |= au2AlphaReference[i_alpha_num];
			
			return au2Colour[u2_blend];
		}
		else
		{
			// Blend with light gray.
			uint16 u2_blend = ((128 << 8) & 0xf800) |
							  ((128 << 3) & 0x07e0) |
							  ((128 >> 3) & 0x001f);

			u2_blend &= ~u2AlphaMask;
			u2_blend |= au2AlphaReference[i_alpha_num];
			
			return au2Colour[u2_blend];
		}
	}

	// 12-bit color to alhpa conversion table.
	uint16 *u2ColorToAlpha;

	//*****************************************************************************************
	CAlphaBlend::CAlphaBlend()
	{
		u2ColorToAlpha = 0;
	}

	//*****************************************************************************************
	CAlphaBlend::~CAlphaBlend()
	{
		delete[] u2ColorToAlpha;
	}

	//*****************************************************************************************
	void CAlphaBlend::Setup(CRaster* pras)
	{
		delete[] u2ColorToAlpha;
		u2ColorToAlpha = new uint16[4096];

		AlwaysAssert(u2ColorToAlpha);

		// Build table.
		if (pras->pxf.cposG == 0x03E0)
		{
			// 555 colour.
			for (int c = 0; c < 4096; c++)
			{
				u2ColorToAlpha[c] = ((c << 3) & 0x7800) |
									((c << 2) & 0x03c0) |
									((c << 1) & 0x001e);
			}
		}
		else
		{
			// 565 colour.
			for (int c = 0; c < 4096; c++)
			{
				u2ColorToAlpha[c] = ((c << 4) & 0xf000) |
									((c << 3) & 0x0780) |
									((c << 1) & 0x001e);
			}
		}
	}

//
// Global function implementations.
//

//*********************************************************************************************
void ApplyAlphaColour(CMesh& msh, const char* str_name)
{
	Assert(str_name);

	const char* strAlphaColourName = "AlphaCol";
	const char* strNumerals        = "0123456789";
	CBeginsWith bw_alpha_col(str_name);
	int i_alpha_num = 0;

	//
	// See if the name contains the string 'alphacol.' If it does, obtain the value after
	// it and apply the colour. If it does not, do nothing.
	//
	if (!(bw_alpha_col == strAlphaColourName))
		return;

	// Look for the first numeral.
	int i_pos = strcspn(str_name, strNumerals);

	// If there is a first numeral, read it in.
	if (i_pos > 0 && i_pos < strlen(str_name))
	{
		sscanf(str_name + i_pos, "%ld", &i_alpha_num);
		SetMinMax(i_alpha_num, 0, 15);
	}

	ApplyAlphaColour(msh, i_alpha_num);
}


//*********************************************************************************************
void ApplyAlphaColour(CMesh& msh, int i_alpha_num)
{
	//
	// Iterate through all the surfaces on the mesh and apply the alpha colour.
	//

	Assert(i_alpha_num >= 0);
	Assert(i_alpha_num < 16);

	// Create an alpha colour texture.
	void* pv_tex = CLoadImageDirectory::pvAllocate( sizeof(CTexture) );
	rptr<CTexture> ptex_alpha = rptr_new(pv_tex) CTexture(i_alpha_num);

	// Assign the texture to the mesh.
	for (uint u_surface = 0; u_surface < msh.pasfSurfaces.uLen; ++u_surface)
	{
		msh.pasfSurfaces[u_surface].ptexTexture = ptex_alpha;
	}
}


//
// Global Variables.
//

// Sun beam light table.
CLightBlend lbAlphaConstant;

// Terrain fog table.
CLightBlend lbAlphaTerrain(8);

// Water translucency table.
CLightBlend lbAlphaWater;

// Lookup for water conversion.
uint16 au2IntensityToAlpha[1 << iRAMP_SHIFT_BITS];

// Texture alpha blend.
CAlphaBlend abAlphaTexture;
