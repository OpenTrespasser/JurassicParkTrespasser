/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		A colour look-up table for any pixel format. This clut has three dimensions:
 *			1) The palette index (typically 8 bits);
 *			2) A ramp value representing intensity; and
 *			3) A depth value representing distance.
 *		The clut is constructed from a source CPal and a destination CPixelFormat.
 *
 *		If '1' is specified for the number of ramp values, the single ramp (intensity) will be
 *		filtered only by the 'clrDefEndRamp' value.
 *
 *		If '1' is specified for the number of depth values, the single ramp (intensity) will be
 *		filtered only by the 'clrDefStartDepth' value.
 *
 * To do:
 *		Add HSL colour matching, possibly using octrees. 
 *		Search for a faster method than a 'switch...case' statement for 'u4GetGouraudAddress.'
 *		Make a correct non-black value for black for 8 bit.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Clut.hpp                                                     $
 * 
 * 40    9/22/98 10:39p Pkeet
 * Forced inline critical Direct3D functions.
 * 
 * 39    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 38    7/25/98 4:14p Pkeet
 * Changed parameter in 'GetColours' to match it implementation.
 * 
 * 37    7/24/98 5:07p Pkeet
 * Added initial code for Direct3D shading.
 * 
 * 36    98/07/22 14:39 Speter
 * CClut now simplified and improved. Surface reflection calculations now done entirely in
 * CMaterial; CClut just converts from TReflectVal to TClutVal (helps D3D). Distinction between
 * shading and hiliting (rv > 1) now entirely distinct from distinction between diffuse and
 * specular reflection. Colours now better allocated to fit material range; use proper
 * gamma-corrected lower range value, allowing denser colour allocation. Bugs fixed which
 * produced wrong ranges for certain materials. 
 * 
 * 35    7/13/98 3:03p Mlange
 * Added CClut::u4GetMemSize().
 * 
 * 34    98/04/22 12:27 Speter
 * Now store pxfDest in clut.
 * 
 * 33    4/01/98 6:56p Pkeet
 * Fixed an assert.
 * 
 * 32    4/01/98 5:44p Pkeet
 * Added support for an independant Direct3D clut.
 * 
 * 31    3/13/98 5:44p Pkeet
 * Added a specialized constructor and member functions for supporting textured, non-alpha
 * water.
 * 
 * 30    98/02/26 13:40 Speter
 * Added safety clamps on clut scaling parameters. Moved TClutVal to Material.hpp.
 * 
 * 29    98/02/10 12:53 Speter
 * Replaced SClutInit with CClu, which can performs ramping functions without requiring a table.
 * 
 * 28    1/07/98 3:18p Pkeet
 * Added a new clut constructor.
 * 
 * 27    97/11/07 10:43a Pkeet
 * Added the 'u2FogSolidCol' global array.
 * 
 * 26    97/11/04 7:18p Pkeet
 * Added the 'SetAsTerrainClut' member function.
 * 
 * 25    97/10/12 20:25 Speter
 * Cluts are now customised to a CMaterial, making optimum use of diffuse and specular ranges.
 * Added several functions to CClut which convert lighting values to clut indices, with gamma
 * correction (replaces some lighting code).  Added lvDefStart to SClutInit, removed fMaxShaded
 * (calculated from material properties).
 * 
 * 24    9/02/97 6:50p Agrant
 * Made some colors non-const to allow option file specification
 * 
 * 23    97/06/03 18:39 Speter
 * Added Draw() function, added iNumEntries member, made all members protected.
 * 
 * 22    97-04-21 16:41 Speter
 * Added useful assert.
 * 
 * 21    97/02/19 10:33 Speter
 * Added clrReflection field to SClutInit.  Clut now builds refractive ramp based on palette
 * flag.
 * 
 * 20    1/20/97 2:56p Pkeet
 * Made black a very dark shade of blue for all colour bitdepths except 8 bit.
 * 
 * 19    96/12/17 13:16 Speter
 * Removed gamma correction code (now in separate module).
 * Changed iNumHiliteValues variable to fMaxShaded ratio.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_CLUT_HPP
#define HEADER_LIB_VIEW_CLUT_HPP


//
// Includes.
//

#include "Pixel.hpp"
#include "Gamma.hpp"
#include "Lib/Renderer/Material.hpp"
#include "Lib/Transform/TransLinear.hpp"


//
// Forward declarations.
//
class CRaster;


//
// Enumerations.
//

enum EClutState
// Enumeration of clut states
// Prefix: ecs
{
	ecsSCREEN,			// Screen format clut.
	ecsD3D_TRANSPARENT,	// Transparent D3D textures.
	ecsD3D_OPAQUE,		// Opaque D3D textures.
	ecsUNKNOWN			// Unknown state.
};

//
// Constants.
//

//
// The number of bits in the palette index. This must be a power of 2, and should always be
// set to 8 representing 8 bits, or a 256 colour palette.
//
const int iBitsInIndex = 8;

// Default parameters for number of ramp and depth values for clut.
#define iRAMP_SHIFT_BITS  (5)
#define iDEPTH_SHIFT_BITS (3)
const int iDefNumRampValues		= 1 << iRAMP_SHIFT_BITS;
const int iDefNumDepthValues	= 1 << iDEPTH_SHIFT_BITS;

// Conversion table size for Direct3D.
#define iD3D_TABLE_SIZE (256)

// Default start and end colours for clut.
extern CColour clrDefStartRamp;
extern CColour clrDefEndRamp;
extern CColour clrDefEndDepth;
extern CColour clrDefReflection;
extern uint16  u2FogSolidCol[iDefNumDepthValues];

extern TLightVal lvDefStart;

// Black defined for the specific colour depth.
extern uint32 u4BlackPixel;


//
// Structures.
//

//*********************************************************************************************
//
struct SD3DTable
//
// Table for D3D colours.
//
// Prefix: d3dt
//
//**************************************
{
	uint32 u4Colour;
	uint32 u4Specular;
};


//
// Class definitions.
//

//*********************************************************************************************
//
class CClu
//
// Prefix: clu
//
// Colour LookUp (a clut without the t).  Specifies parameters for clut construction.
//
//**************************************
{
public:
	const CMaterial* pmatMaterial;		// The material the clut is based on.

	CColour clrStartRamp, clrEndRamp;	// Colour limits for ramp modulation.
	CColour clrEndDepth;				// Colour limit for depth fading.
	CColour clrReflection;				// Colour for reflections (when material is bRefractive).
	int iNumRampValues;					// Number of ramp values.
	int iNumDepthValues;				// Number of depth values.

	// The single gamma-correcting lighting-to-clut translation table.
	static CGammaCorrection<float> gcfMain;

protected:
	CTransLinear<>	tlrClutShaded, tlrClutHilite;
										// Translation parameters for lighting to clut index values.
	int iNumShadedValues;				// Number of values used for shading.
	TReflectVal	rvStart;				// Min reflect value used in clut allocation.
	TReflectVal rvMax;					// Cached maximum reflectance in material.

public:

	//*****************************************************************************************
	CClu
	(
		const CMaterial* pmat,
		int     i_num_ramp      = iDefNumRampValues,
		int     i_num_depth     = iDefNumDepthValues,
		TLightVal lv_start		= lvDefStart,
		CColour clr_start_ramp  = clrDefStartRamp,
		CColour clr_end_ramp    = clrDefEndRamp,
		CColour clr_end_depth   = clrDefEndDepth,
		CColour clr_reflection	= clrDefReflection
	);

	//*****************************************************************************************
	//
	forceinline TClutVal cvFromReflect
	(
		TReflectVal rv					// Total reflectance off a surface,
										// combining any diffuse and specular components
										// into a single value from 0..rvMAX_WHITE.
	) const
	//
	// Returns:
	//		The corresponding clut index, in floating point.
	//		Input (rvStart..1) is gamma-corrected, then transformed to (0..<iNumRampValues).
	//
	//**********************************
	{
		Assert(bWithin(rv, 0.0, rvMAX_WHITE));

		TClutVal cv;

		if (rv <= rvMAX_COLOURED)
		{
			// Gamma-correct reflectance, and convert to shaded clut value.
			// To do: put this all in a single LUT.
			rv = gcfMain(rv);
			SetMax(rv, rvStart);
			cv = rv * tlrClutShaded;
		}
		else
			// Convert to hilited clut value.
			cv = rv * tlrClutHilite;
		Assert(cv >= 0.0 && cv < float(iNumRampValues));
		return cv;
	}


	//*****************************************************************************************
	//
	TReflectVal rvFromClut
	(
		TClutVal cv						// Clut index.
	) const;
	//
	// Returns:
	//		The corresponding reflectance value.
	//
	//**********************************

	//*****************************************************************************************
	//
	forceinline void GetColours
	(
		TClutVal cv,					// A reflectance value.
		CColour* pclr_mod,				// Returned colours: pclr_mod modulates texture colour,
		CColour* pclr_add				// pclr_add adds to it.
	) const
	//
	// Returns the colours used for shading a texture, corresponding to this reflectance.
	//
	//**********************************
	{
		Assert(cv >= 0 && cv < float(iNumRampValues));

		if (cv <= float(iNumShadedValues))
		{
			// Scale to the corresponding reflect range of the clut.
			cv /= float(iNumShadedValues);
			cv = rvStart + cv * (1.0 - rvStart);

			// Calculate the modulating (shading) colour. There is no add (hilite).
			*pclr_mod = clrStartRamp.ScaleTrunc(1.0f - cv) + clrEndRamp.ScaleTrunc(cv);
			pclr_add->u4Value = 0;
		}
		else
		{
			AlwaysAssert(iNumRampValues != iNumShadedValues);

			// In the hilite range. Fade between the source colour and the hilite.
			cv -= float(iNumShadedValues);
			cv /= float(iNumRampValues - iNumShadedValues);
			*pclr_mod = clrEndRamp.ScaleTrunc(1.0f - cv);
			*pclr_add = clrEndRamp.ScaleTrunc(cv);
		}
	}

	//*****************************************************************************************
	bool operator == (const CClu& clu) const
	{
		return !memcmp(this, &clu, sizeof(*this));
	}
};

//*********************************************************************************************
//
class CClut: public CClu
//
// A 3 dimensional colour look-up table object. The table is based on a source palette and a
// target pixel format.
//
// Prefix: clut
//
// Notes:
//		The table is built with 3 dimensions:
//			1) Index.	This value represents an index into the source palette.
//			2) Ramp.	This value represents a range of intensity.
//			3) Depth.	This value represents a range of colour adjustment.
//
//		The table's size depends on the range of ramp and depth values desired. The number of
//		index values are set to 256, representing an 8-bit source palette.
//
//		The ramp and depth ranges are values interpolated between two colour values, one pair
//		of colour values for ramp and one pair for depth.
//
//**************************************
{
private:
	static EClutState ecsClutState;	// The pixel output format of the clut.

	char* aTable;			// The colour lookup table in screen format.
	uint8* aTableD3DTrans;	// The colour lookup table in D3D transparent texture format.
	int   iSizeofPixel;		// The number of bytes in the destination pixels.
	int   iShiftRamp;		// The number of bits to shift the ramp value by to get an index
							// into the table.
	int   iShiftDepth;		// The number of bits to shift the depth value by to get an index
							// into the table.
	int	  iNumEntries;		// Number of colour entries contained.
							// Similar parameters for combined specular lighting to clut index values.
	bool  bTerrainClut;		// Set to 'true' if the clut is used for terrain.
	bool  bDirect3DClut;	// Set to 'true' if the clut is used for Direct3D.
	SD3DTable* ad3dtTable;	// D3D table.

public:

	const CPal* ppalPalette;	// The palette the clut is based on.
	CPixelFormat	pxfDest;	// The destination pixel format for the clut.

public:

	//*****************************************************************************************
	//
	forceinline void ConvertToD3D
	(
		SD3DTable& rd3dt,	// Direct3D colour and specular value.
		TClutVal   cv		// Clut index.
	)
	//
	// Converts the cv value to a colour and specular value for use by Direct3D.
	//
	//**********************************
	{
		// Build a conversion table if one does not already exist.
		if (!ad3dtTable)
			BuildD3DColourTable();

		CColour clr_colour;
		CColour clr_specular;

		GetColours(cv, &clr_colour, &clr_specular);

		rd3dt.u4Colour   = clr_colour.u4Value;
		rd3dt.u4Specular = clr_specular.u4Value;
	}

	//*****************************************************************************************
	//
	// CClut constructor and destructor.
	//

	//*****************************************************************************************
	//
	CClut
	(
		const CClu& clu,					// The colour information.
		const CPal* ppal_source,			// Pointer to the source palette.
		const CPixelFormat& pxf_dest		// The destination pixel format.
	);
	//
	// Constructs the colour lookup table based on the number of ramp and depth values, and
	// start and end colours for the ramp and depth.
	//
	//**************************************

	//*****************************************************************************************
	//
	CClut
	(
		const CClu& clu,					// The colour information.
		const CPal* ppal_source				// Pointer to the source palette.
	)
	//
	// Constructs an empty clut with just the source pointers, useful for searching in containers.
	//
	//**************************************
		: CClu(clu), ppalPalette(ppal_source), ad3dtTable(0)
	{
	}

	//*****************************************************************************************
	//
	CClut
	(
		CClut* pclut,			// Clut to construct the current clut from.
		bool   b_water = false	// Constructs a water clut if set.
	);
	//
	// Constructs a clut using an existing clut as a template.
	//
	//**************************************

	//*****************************************************************************************
	//
	~CClut();
	//
	// Frees memory allocated for the lookup table.
	//
	//**************************************


	//*****************************************************************************************
	//
	// CClut member functions.
	//

	//*****************************************************************************************
	//
	static void SetClutState
	(
		EClutState ecs	// New clut state.
	);
	// 
	// Sets the state of the clut system.
	//
	//**********************************

	//*****************************************************************************************
	//
	uint32 u4GetMemSize() const
	// 
	// Returns:
	//		The number of bytes required to hold the CLUT table (excluding any additional D3D
	//		table).
	//
	//**********************************
	{
		return iNumEntries * iNumRampValues * iNumDepthValues * iSizeofPixel;
	}

	//*****************************************************************************************
	//
	void SetAsTerrainClut
	(
	);
	// 
	// Remakes the clut for the terrain.
	//
	//**********************************

	//*****************************************************************************************
	//
	void SetAsWaterClut
	(
		bool b_force = false	// Forces rebuilding of the clut.
	);
	// 
	// Remakes the clut for the water.
	//
	//**********************************

	//*****************************************************************************************
	//
	void Update
	(
		const CPixelFormat& pxf_dest,	// The destination pixel format.
		const CClu& clu					// The colour information.
	)
	// 
	// Remakes the clut with the given parameters.
	//
	//**********************************
	{
		delete[] aTable;
		delete[] aTableD3DTrans;
		delete[] ad3dtTable;
		new(this) CClut(clu, ppalPalette, pxf_dest);
	}

	//*****************************************************************************************
	//
	forceinline int iGetIndex
	(
		int i_index,	// Index into the source palette.
		int i_ramp,		// Intensity value.
		int i_depth		// Depth value.
	) const
	//
	// Finds the index into the current lookup table given the source palette index value,
	// the ramp value and the depth value.
	//
	// Returns.
	//		The index into the lookup table.
	//
	//**************************************
	{
		Assert(bWithin(i_ramp, 0, iNumRampValues-1));
		Assert(bWithin(i_depth, 0, iNumDepthValues-1));
		return i_index | (i_ramp << iShiftRamp) | (i_depth << iShiftDepth);
	}

	//*****************************************************************************************
	//
	forceinline uint32 Convert
	(
		int i_index,	// Index into the source palette.
		int i_ramp,		// Intensity value.
		int i_depth		// Depth value.
	) const
	//
	// Converts an index value representing an index into the source palette, a ramp value and
	// a depth value into a destination pixel formatted for the destination pixel format.
	//
	// Returns.
	//		The converted pixel value.
	//
	//**************************************
	{
		Assert(avGetTable());
		switch (iSizeofPixel)
		{
			case 1:
				return ((uint8*)avGetTable())[iGetIndex(i_index, i_ramp, i_depth)];
			case 2:
				return ((uint16*)avGetTable())[iGetIndex(i_index, i_ramp, i_depth)];
			case 3:
				return ((uint24*)avGetTable())[iGetIndex(i_index, i_ramp, i_depth)];
			case 4:
				return ((uint32*)avGetTable())[iGetIndex(i_index, i_ramp, i_depth)];
		}
		Assert(false);
		return 0;
	}

	//*****************************************************************************************
	//
	forceinline void* pvGetConversionAddress
	(
		int i_index,	// Index into the source palette.
		int i_ramp,		// Intensity value.
		int i_depth		// Depth value.
	) const
	//
	// Converts an index value representing an index into the source palette, a ramp value and
	// a depth value into a destination pixel formatted for the destination pixel format.
	//
	// Returns.
	//		The converted pixel value.
	//
	//**************************************
	{
		Assert(avGetTable());
		switch (iSizeofPixel)
		{
			case 1:
				return (void*)&((uint8*)avGetTable())[iGetIndex(i_index, i_ramp, i_depth)];
			case 2:
				return (void*)&((uint16*)avGetTable())[iGetIndex(i_index, i_ramp, i_depth)];
			case 3:
				return (void*)&((uint24*)avGetTable())[iGetIndex(i_index, i_ramp, i_depth)];
			case 4:
				return (void*)&((uint32*)avGetTable())[iGetIndex(i_index, i_ramp, i_depth)];
		}
		Assert(false);
		return 0;
	}

	//*****************************************************************************************
	//
	forceinline uint32 u4GetGouraudAddress
	(
		int i_index,	// Index into the source palette.
		int i_depth		// Depth value.
	) const
	//
	// Converts an index value representing an index into the source palette and a depth value
	// into a pointer-value into the clut for Gouraud shading by only interpolating
	// intensities.
	//
	// Returns.
	//		The pointer to the location of the clut 'page' in memory converted to a uint32.
	//
	//**************************************
	{
		Assert(avGetTable());

		// Get the index into the table.
		int i_table_index = i_index | (i_depth << iShiftDepth);

		// Switch to the appropriate conversion.
		switch (iSizeofPixel)
		{
			case 1:
				return (uint32)&((uint8*)avGetTable())[i_table_index];
			case 2:
				return (uint32)&((uint16*)avGetTable())[i_table_index];
			case 3:
				return (uint32)&((uint24*)avGetTable())[i_table_index];
			case 4:
				return (uint32)&((uint32*)avGetTable())[i_table_index];
		}

		// Pixel format not currently supported.
		Assert(false);
		return 0;
	}

	//*****************************************************************************************
	//
	void Draw
	(
		rptr<CRaster> pras_screen,
		int i_fog = 0						// Fog index to draw (can only do one at a time).
	) const;
	//
	// Draws the clut values on the screen.
	//
	//**************************************

	//*****************************************************************************************
	//
	void BuildD3DTransparentClut
	(
	);
	//
	// Builds a D3D transparent clut.
	//
	//**************************************

private:

	//*****************************************************************************************
	//
	forceinline void* avGetTable
	(
	) const
	//
	// Returns a pointer to the clut as determined by the clut state.
	//
	//**************************************
	{
		switch (ecsClutState)
		{
			// Return the screen format clut.
			case ecsSCREEN:
				Assert(aTable);
				return aTable;

			// Return the D3D transparent texture clut, building one if necessary.
			case ecsD3D_TRANSPARENT:
				if (!aTableD3DTrans)
					((CClut*)this)->BuildD3DTransparentClut();
				Assert(aTableD3DTrans);
				return aTableD3DTrans;

			// Error.
			default:
				Assert(0);
				return aTable;
		};
	}

	//*****************************************************************************************
	//
	void SetValue
	(
		int i_clut_index,	// Index into the clut.
		uint32 u4_pixel		// Value to set in the clut.
	) const
	//
	//**************************************
	{
		// Make sure the pixel won't be re-rendered as a transparency.
		if (u4_pixel == 0)
			u4_pixel = u4BlackPixel;

		switch (iSizeofPixel)
		{
			case 1:
				((uint8*)avGetTable())[i_clut_index]  = u4_pixel;
				break;
			case 2:
				((uint16*)avGetTable())[i_clut_index] = u4_pixel;
				break;
			case 3:
				((uint24*)avGetTable())[i_clut_index] = u4_pixel;
				break;
			case 4:
				((uint32*)avGetTable())[i_clut_index] = u4_pixel;
				break;
			default:
				Assert(false);
		}
	}
	
	//*****************************************************************************************
	void BuildD3DColourTable();

};

#endif // HEADER_LIB_VIEW_CLUT_HPP
