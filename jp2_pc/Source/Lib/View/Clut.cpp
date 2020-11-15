/*********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The implementation of Clut.hpp.
 *
 * To do:
 *		Speed up cvFromLighting by combining gamma and other operations into single lookup, per clut.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Clut.cpp                                                    $
 * 
 * 74    9/22/98 10:39p Pkeet
 * Forced inline critical Direct3D functions.
 * 
 * 73    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 72    8/28/98 1:08p Asouth
 * Explicit scoping of the fixed class
 * 
 * 71    98.08.18 7:45p Mmouni
 * Increased fCLUT_TOLERANCE to 0.2
 * 
 * 70    8/06/98 6:17p Pkeet
 * Fixed bug with texture formats for clut conversions.
 * 
 * 69    8/01/98 4:46p Pkeet
 * Simplified clut building for Direct3D transparencies.
 * 
 * 68    7/25/98 3:41p Pkeet
 * Changed the 'GetColours' member function and the 'ConvertToD3D' member function to account
 * for gamma already in the clut value.
 * 
 * 67    98.07.24 8:50p Mmouni
 * Increased CLUT tolerance value (error margin).
 * 
 * 66    7/24/98 5:07p Pkeet
 * Added initial code for Direct3D shading.
 * 
 * 65    98/07/22 14:40 Speter
 * CClut now simplified and improved. Surface reflection calculations, including light angles,
 * now done entirely in CMaterial; CClut just converts from TReflectVal to TClutVal (helps
 * D3D). Distinction between shading and hiliting (rv > 1) now entirely distinct from
 * distinction between diffuse and specular reflection. Colours now better allocated to fit
 * material range; use proper gamma-corrected lower range value, allowing denser colour
 * allocation. Bugs fixed which produced wrong ranges for certain materials. 
 * 
 * 64    98/07/20 12:09 Speter
 * Fixed bugs relating to overlit diffuse settings. Set lvDefStart to 0.2, allowing better use
 * of clut range.
 * 
 * 63    7/10/98 4:28p Pkeet
 * Added support for colour keying.
 * 
 * 62    6/09/98 7:16p Pkeet
 * Changed the member function to create the water clut to use the 'bD3DWater' member function
 * of the Direct3D object instead of 'bUseD3D.'
 * 
 * 61    5/13/98 6:45p Pkeet
 * Fixed problem with render cache colours on certain cards.
 * 
 * 60    98/04/22 12:27 Speter
 * Now store pxfDest in clut.
 * 
 * 59    4/01/98 5:44p Pkeet
 * Added support for an independant Direct3D clut.
 * 
 * 58    3/13/98 5:44p Pkeet
 * Added a specialized constructor and member functions for supporting textured, non-alpha
 * water.
 * 
 * 57    98/02/26 13:39 Speter
 * Added safety clamps on clut scaling parameters.  Increased min size used for specular
 * lights.
 * 
 * 56    2/23/98 5:00p Agrant
 * Moved CLUT profiling into CLUT.cpp to avoid any untracked CLUTS.
 * 
 * 55    98/02/10 12:54 Speter
 * Replaced SClutInit with CClu, which can performs ramping functions without requiring a
 * table.
 * 
 * 54    1/19/98 7:30p Pkeet
 * Added support for 16 bit bumpmaps by adding lower colour resolution and a smaller size for
 * 'CBumpAnglePair.'
 * 
 **********************************************************************************************/


//
// Includes.
//

#include "common.hpp"
#include "Clut.hpp"
#include "Raster.hpp"
#include "ColourT.hpp"
#include "Lib/Types/FixedP.hpp"
#include "Lib/Sys/FileEx.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Renderer/LightBlend.hpp"
#include "Lib/Sys/W95/Render.hpp"

#include <memory.h>
#include <stdio.h>

#define bREFRACTIVE_CLUT	0

// Amount to shift clut values inward to prevent range errors in rasteriser.
#define fCLUT_TOLERANCE		0.2f

// Default start and end colours for clut.
const CColour clrDefStartRampDefault(0, 0, 0);		// (.149, .082, .165)
const CColour clrDefEndRampDefault  (255, 255, 255);		// (.996, 1,    .816)
const CColour clrDefEndDepthDefault (215, 230, 255);
const CColour clrDefReflectionDefault(166, 222, 245);

CColour clrDefStartRamp = clrDefEndDepthDefault;
CColour clrDefEndRamp = clrDefEndRampDefault;
CColour clrDefEndDepth = clrDefEndDepthDefault;
CColour clrDefReflection = clrDefReflectionDefault;
//CColour clrDefReflection = clrDefEndDepth;

TLightVal lvDefStart = 0.2;				// Minimum expected ambient value.

const float fGAMMA_DEFAULT = 0.6;

uint32 u4BlackPixel;
const char* strClutFileCache = "BinData/Cluts";

//**********************************************************************************************
//
// CClu implementation.
//

	CGammaCorrection<float> CClu::gcfMain(rvMAX_WHITE, fGAMMA_DEFAULT);

	//******************************************************************************************
	CClu::CClu
	(
		const CMaterial* pmat,
		int i_num_ramp,
		int i_num_depth,
		TLightVal lv_start,
		CColour clr_start_ramp,
		CColour clr_end_ramp,
		CColour clr_end_depth,
		CColour clr_reflection
	) :
		pmatMaterial(pmat),
		iNumRampValues(i_num_ramp), iNumDepthValues(i_num_depth),
		clrStartRamp(clr_start_ramp), clrEndRamp(clr_end_ramp), 
		clrEndDepth(clr_end_depth), clrReflection(clr_reflection)
	{
		Assert(pmat);
		Assert(bWithin(lv_start, 0, 1));
		Assert(iNumRampValues > 0);

		// Set minimum reflectance value.
		if (pmat->bRefractive)
			lv_start = 0;

		rvStart = gcfMain(lv_start * pmat->rvDiffuse);

		rvMax = Max(Max(pmat->rvDiffuse, pmat->rvEmissive), pmat->rvSpecular);

		if (pmat->rvDiffuse == 0.0 && pmat->rvSpecular == 0.0)
			// Optimise for pre-lit materials by creating only 1 shade value.
			iNumRampValues = iNumShadedValues = 1;
		else
		{
			// Find max shading needed.
			if (rvMax > 1.0)
			{
				// Reduce the range allocated to specular hilites.
				float f_max = 1.0 + (rvMax - 1.0) * 0.75;
				float f_hilite_frac = (f_max - 1.0) / (f_max - rvStart);
				int i_num_hilite_values = Max(iRound(f_hilite_frac * iNumRampValues), 1);
				iNumShadedValues = iNumRampValues - i_num_hilite_values;
			}
			else
				// No hilite component.
				iNumShadedValues = iNumRampValues;
		}

		// Set up parameters for quick translation between lighting value and clut index,
		// scaled down slightly to never exceed max clut index.
		tlrClutShaded = CTransLinear<>
		(
			rvStart, 1.0 - rvStart,
			fCLUT_TOLERANCE, Max(iNumShadedValues - 2*fCLUT_TOLERANCE, 0)
		);

		if (iNumShadedValues < iNumRampValues)
			tlrClutHilite = CTransLinear<>
			(
				// Isolated specular lighting translation.
				1.0, rvMax - 1.0,
				iNumShadedValues + fCLUT_TOLERANCE, 
				Max(iNumRampValues - iNumShadedValues - 2*fCLUT_TOLERANCE, 0)
			);
		else
			tlrClutHilite = CTransLinear<>
			(
				1.0, 1.0,
				iNumRampValues - fCLUT_TOLERANCE, 0.0
			);
	}

	//*****************************************************************************************
	TReflectVal CClu::rvFromClut(TClutVal cv) const
	{
		// Go backwards, converting cv to rv.
		if (cv >= float(iNumShadedValues))
			// It's in the hilite range.
			return cv * ~tlrClutHilite;
		else
			// It's in the shaded range.
			// To do: account for gamma.
			return cv * ~tlrClutShaded;
	}

//********************************************************************************************
//
// CClut implementation.
//

	//*****************************************************************************************
	CClut::CClut(const CClu& clu, const CPal* ppal_source, const CPixelFormat& pxf_dest)
		: CClu(clu), ppalPalette(ppal_source), pxfDest(pxf_dest), bTerrainClut(false), aTableD3DTrans(0),
		  ad3dtTable(0)
	{
		extern CProfileStat psCluts;
		CCycleTimer ctmr;


		char         str_filecache[256];	// File cache name.
		int          i_depth;				// Depth iteration.
		int          i_ramp;				// Ramp iteration.
		int          i_col_index;			// Colour iteration.
		
		Assert(ppal_source);
		Assert(pxf_dest);
		
		Assert(bPowerOfTwo(iNumRampValues));
		Assert(iNumDepthValues > 0);

	#if iBUMPMAP_RESOLUTION == 16

		// Get the number of colours granular to the power of two.
		iNumEntries = (int)ppal_source->aclrPalette.uLen;
		if (!bPowerOfTwo(iNumEntries))
			iNumEntries = NextPowerOfTwo(iNumEntries);

		// Currently only allow a non-256 size for bumpmaps, which require a 64 entry size.
		if (iNumEntries != 64)
			iNumEntries = 256;

	#else // iBUMPMAP_RESOLUTION == 16

		iNumEntries = 256;

	#endif // iBUMPMAP_RESOLUTION == 16


		iSizeofPixel = pxf_dest.u1Bits / 8;

		if (iSizeofPixel == 1)
		{
			Assert(pxf_dest.ppalAttached);
			pxf_dest.ppalAttached->MakeFastLookup();
		}

		Assert(bWithin(iSizeofPixel, 1, 4));

		//
		// Define black.
		//
		switch (iSizeofPixel)
		{
			case 1:
				// To do: Change this line!
				u4BlackPixel = 1;
				break;
			default:
				u4BlackPixel = 1;
		}

		// Set the values to shift the ramp and depth amounts by.
		iShiftRamp  = uLog2(iNumEntries);
		iShiftDepth = uLog2(iNumRampValues) + iShiftRamp;

		uint32 u4_clut_size = iNumEntries * iNumRampValues * iNumDepthValues * iSizeofPixel;

		// Allocate memory for the clut table.
		aTable = new char[u4_clut_size];
		
		//
		// If the clut is base on a destination size of eight bits, use file caching.
		//
		if ((pxf_dest.u1Bits / 8) == 1)
		{
			Assert(pxf_dest.ppalAttached);

			// Generate a file name from the palette hash values.
			sprintf(str_filecache, "%x%x.cl8", ((CPal*)ppal_source)->u4GetHashValue(),
				    pxf_dest.ppalAttached->u4GetHashValue());

			// Set the file paths to the local BinData directory.
			CPushDir pshd(strStartupPath());
			CPushDir pshd2(strClutFileCache);

			// If the file exists, load it instead of calculating it.
			if (bFileExists(str_filecache))
				ReadImage(str_filecache, aTable, u4_clut_size);
		}

		int i_num_shaded_values = iNumShadedValues;
		int i_num_hilite_values = iNumRampValues - i_num_shaded_values;

		// Initialize the ramp start and end values.
		CColourT<float> fcl_shade_start	= CColourT<float>(clrStartRamp) * Min(rvMax, 1.0);
		CColourT<float> fcl_shade_end	= CColourT<float>(clrEndRamp)   * Min(rvMax, 1.0);

		// Ramp up the start value by rvStart.
		fcl_shade_start = fcl_shade_start * (1.0 - rvStart) + fcl_shade_end * rvStart;

		CColourT<float> fcl_light			= clrEndRamp;
		CColourT<float> fcl_hilite_end		= fcl_shade_end;
		CColourT<float> fcl_hilite_mod;
		CColourT<float> fcl_hilite_add(0, 0, 0);

		if (rvMax <= 1.0)
		{
			// No hilite range.
			fcl_hilite_end  = fcl_shade_end;
		}
		else
		{
			// There is a hilite range.
			float f_hilite = rvMax - 1.0;
			fcl_hilite_mod = CColourT<float>(clrEndRamp) * (1.0 - f_hilite);
			fcl_hilite_add = CColourT<float>(clrEndRamp) * f_hilite;
		}

		CColourT<float> fcl_depth			= clrEndDepth;
		CColourT<float> fcl_reflection		= clrReflection;

		TPixel	pix_ramp_end, pix_depth;

		pix_ramp_end = pxf_dest.pixFromColour(clrEndRamp);
		pix_depth = pxf_dest.pixFromColour(clrEndDepth);

		// Variables for remembering the last translation done.
		// By initialising them both to 0, we assume that black always maps to pixel 0.
		CColour	clr_last(0, 0, 0);
		TPixel	pix_last = 0;

		//
		// Iterate over colour range.
		//
		int i_max_entry = (int)ppal_source->aclrPalette.uLen - 1;
		for (i_col_index = 0; i_col_index < iNumEntries; i_col_index++)
		{
			int i_pal_index = i_col_index;
			if (i_pal_index > i_max_entry)
				i_pal_index = i_max_entry;
			CColourT<float>	fcl_pal = ppal_source->aclrPalette[i_pal_index];
												// Source palette colour, in float 0..1 range.
			CColourT<float>	fcl_ramp;			// Colour for current ramp value.
			CColourT<float>	fcl_delta_diffuse;	// Delta for each ramp increment in diffuse range.
			CColourT<float>	fcl_delta_specular;	// Delta for each ramp increment in specular range.

			//
			// Set up for iteration over the ramp range.
			//
#if bREFRACTIVE_CLUT
			if (pmatMaterial->bRefractive)
			{
				//
				// From [0, iNumShadedValues) we fade from fcl_pal up to fcl_reflection,
				// to show various proportions of reflection.
				// From [iNumShadedValues, iNumRampValues) we fade from fcl_reflection
				// to fcl_light, to show specular highlights on top of reflection.
				//
				if (iNumShadedValues == 1)
				{
					// Only 1 ramp value, so set to end value.
					fcl_ramp = fcl_reflection;
				}
				else
				{
					// More than 1 ramp value, so modulate by a range from the start ramp colour
					// to the end ramp colour.
					fcl_ramp			= fcl_pal * 0.5;
					fcl_delta_diffuse	= (fcl_reflection - fcl_ramp) / float(iNumShadedValues - 1);
				}

				// If there are any hilite values, generate the delta to fade the start ramp colour
				// to the end ramp colour.
				if (i_num_hilite_values > 0)
					fcl_delta_specular = (fcl_light - fcl_reflection) / float(i_num_hilite_values);
			}
			else
#endif
			{
				//
				// From [0, iNumShadedValues) we modulate fcl_pal by a colour ranging 
				// from fcl_shade_start to fcl_shade_end.
				// From [iNumShadedValues, iNumRampValues) we fade the modulated colour
				// up to a calculated fcl_specular_end.
				//
				if (i_num_shaded_values == 1)
				{
					// Only 1 ramp value, so set to the end value.
					fcl_ramp = fcl_pal * fcl_shade_end;
				}
				else
				{
					// More than 1 ramp value, so modulate by a range from the start ramp colour
					// to the end ramp colour.
					fcl_ramp = fcl_pal * fcl_shade_start;
					fcl_delta_diffuse = (fcl_pal * (fcl_shade_end - fcl_shade_start)) /
										float(i_num_shaded_values - 1);
				}

				// If there are any hilite values, generate the delta to fade the end diffuse colour
				// to the actual end colour.
				if (i_num_hilite_values > 0)
				{
					// Calculate the endpoint specular colour, and delta thereto.
					CColourT<float> fcl_specular_end = (fcl_pal * fcl_hilite_mod) + fcl_hilite_add;
					fcl_delta_specular = (fcl_specular_end - fcl_pal * fcl_shade_end) / float(i_num_hilite_values);
				}
			}

			//
			// Iterate over the depth range.
			//
			for (i_depth = 0; i_depth < iNumDepthValues; i_depth++)
			{	
				//
				// Set the solid fog colour.
				//
				{
					CColour clr_fog;
					float f_depth   = float(i_depth) / float(iNumDepthValues);
					float f_depth_1 = 1.0f - f_depth;
					float f_red     = float(clrDefEndDepth.u1Red) * f_depth + 128.0f * f_depth_1;
					float f_green   = float(clrDefEndDepth.u1Green) * f_depth + 96.0f * f_depth_1;
					float f_blue    = float(clrDefEndDepth.u1Blue) * f_depth + 32.0f * f_depth_1;

					// Fill colour structure.
					clr_fog.u1Red   = uint8(f_red);
					clr_fog.u1Green = uint8(f_green);
					clr_fog.u1Blue  = uint8(f_blue);

					// Convert colour.
					u2FogSolidCol[i_depth] = pxf_dest.pixFromColour(clr_fog);
				}

				//
				// For a given depth, we modify the fcl_ramp values to fade toward fcl_depth.
				//
				float f_depth = (iNumDepthValues > 1) ? float(i_depth) / float(iNumDepthValues - 0) : 0.0f;

				// The starting colour fades toward depth.
				CColourT<::fixed> fxcl_ramp = clrConvert(::fixed(0), fcl_ramp * (1 - f_depth) + fcl_depth * f_depth);

				// The ramp deltas are diminished by 1 - f_depth.
				CColourT<::fixed> fxcl_delta_diffuse = clrConvert(::fixed(0), fcl_delta_diffuse * (1 - f_depth));
				CColourT<::fixed> fxcl_delta_specular= clrConvert(::fixed(0), fcl_delta_specular * (1 - f_depth));

				//
				// Iterate over the ramp range.
				//
				int i_tbl_index = iGetIndex(i_col_index, 0, i_depth);
				for (i_ramp = 0; i_ramp < iNumRampValues; i_ramp++)
				{

					// Convert the CColourFixed to CColour.
					CColour clr_pixel = fxcl_ramp;

					if (clr_pixel != clr_last)
					{
						// New colour.
						pix_last = pxf_dest.pixFromColour(clr_pixel);
						clr_last = clr_pixel;
					}
/*
					pix_last = pxf_dest.pixFromColour(fxcl_ramp);
*/
					SetValue(i_tbl_index, pix_last);

					// Increment the ramp value.
					if (i_ramp < i_num_shaded_values - 1)
						fxcl_ramp += fxcl_delta_diffuse;
					else
						fxcl_ramp += fxcl_delta_specular;

					i_tbl_index += 1 << iShiftRamp;
				}
			}
		}
		
		//
		// If the clut is base on a destination size of eight bits, write file cache.
		//
		if ((pxf_dest.u1Bits / 8) == 1)
		{
			// Set the file paths to the local BinData directory.
			CPushDir pshd(strStartupPath());
			CPushDir pshd2(strClutFileCache);

			// Save cache file.
			SaveImage(str_filecache, aTable, u4_clut_size);
		}

		psCluts.Add(ctmr(),1);
	}
	
	//*****************************************************************************************
	CClut::CClut(CClut* pclut, bool b_water)
		: ppalPalette(0), bDirect3DClut(false), aTable(0), CClu(*pclut), aTableD3DTrans(0), ad3dtTable(0)
	{
		Assert(pclut);

		if (b_water)
		{
			//
			// Use specialized construction if the clut is intended for use for non-alpha
			// water.
			//
			ppalPalette = pclut->ppalPalette;
			Assert(ppalPalette);
			SetAsWaterClut(true);
			return;
		}

		extern CProfileStat psCluts;
		CCycleTimer ctmr;

		// Copy all data.
		*this = *pclut;

		//
		// Create a new table.
		//

		// Get the memory requirements for the new clut.
		uint32 u4_clut_size = iNumEntries * iNumRampValues * iNumDepthValues * iSizeofPixel;

		// Allocate memory for the clut table.
		aTable = new char[u4_clut_size];

		// Copy the clut.
		memcpy(aTable, pclut->aTable, u4_clut_size);


		psCluts.Add(ctmr(),1);
	}

	///****************************************************************************************
	CClut::~CClut()
	{
		delete[] aTable;
		delete[] aTableD3DTrans;
		delete[] ad3dtTable;
		extern CProfileStat psCluts;
		psCluts.Add(0,-1);
	}

	//*****************************************************************************************
	void CClut::Draw(rptr<CRaster> pras_screen, int i_fog) const
	{
		int i_clr_repeat = Max(pras_screen->iWidth/ iNumEntries, 1);
		int i_ramp_repeat = Max(pras_screen->iHeight / iNumRampValues, 1);

		// Draw the colours going across the screen.
		int i_x = 0;
		for (int i_clr = 0; i_clr < iNumEntries; i_clr++)
		{
			for (int i = 0; i < i_clr_repeat; i++, i_x++)
			if (i_x < pras_screen->iWidth)
			{
				int i_y = 0;
				for (int i_ramp = 0; i_ramp < iNumRampValues; i_ramp++)
				{
					TPixel pix = Convert(i_clr, i_ramp, i_fog);

					for (int i = 0; i < i_ramp_repeat; i++, i_y++)
					if (i_y < pras_screen->iHeight)
					{
						pras_screen->PutPixel(i_x, i_y, pix);
					}
				}
			}
		}
	}

	//*****************************************************************************************
	void CClut::SetAsTerrainClut()
	{
		CColour* aclr = ppalPalette->aclrPalette.atArray;
		uint16*  pu2  = (uint16*)aTable;

		// Do nothing if the clut has already been set for terrain use.
		if (bTerrainClut)
			return;

		bTerrainClut = true;

		uint u_entry = 0;

		// Remake clut for the terrain.
		for (uint u_intensity = 0; u_intensity < (1 << 5); ++u_intensity)
		{
			for (uint u_pal = 0; u_pal < (1 << 8); ++u_pal, ++u_entry)
			{
				float f_intensity = float(u_intensity) / float((1 << 5) - 1);
				float f_red   = float(aclr[u_pal].u1Red) / 255.0f;
				float f_green = float(aclr[u_pal].u1Green) / 255.0f;
				float f_blue  = float(aclr[u_pal].u1Blue) / 255.0f;

				f_red   *= f_intensity;
				f_green *= f_intensity;
				f_blue  *= f_intensity;

				uint32 u4_red   = uint32(f_red * 16.0f + 0.5f) << 11;
				uint32 u4_green = uint32(f_green * 16.0f + 0.5f) << 6;
				uint32 u4_blue  = uint32(f_blue * 16.0f + 0.5f) << 1;

				pu2[u_entry] = u4_red | u4_green | u4_blue;
			}
		}
	}

	//*****************************************************************************************
	void CClut::SetAsWaterClut(bool b_force)
	{
		typedef CLightBlend::SLightBlendSettings TLbs;

		Assert(ppalPalette);
		Assert(ppalPalette->aclrPalette.uLen == 256);

		// Rebuild the clut only if required.
		if (!b_force)
		{
			if (bDirect3DClut == d3dDriver.bD3DWater())
				return;
		}

		// Set new clut states.
		bDirect3DClut   = d3dDriver.bD3DWater();
		bTerrainClut    = false;
		iNumRampValues  = iDefNumRampValues;
		iNumDepthValues = 1;
		iSizeofPixel    = sizeof(uint16);
		iShiftRamp      = iBitsInIndex;
		iShiftDepth     = iBitsInIndex + iDEPTH_SHIFT_BITS;
		iNumEntries     = 256;
		Assert(iNumRampValues == iNumNonAlphaWaterSettings);

		// Get a pixel conversion object.
		CPixelFormat pxf;

		if (bDirect3DClut)
		{
			// Get the Direct3D format.
			DDPIXELFORMAT ddpf = d3dDriver.ddpfGetPixelFormat(ed3dtexSCREEN_OPAQUE);
			pxf = CPixelFormat
			(
				(int)ddpf.dwRGBBitCount,
				ddpf.dwRBitMask,
				ddpf.dwGBitMask,
				ddpf.dwBBitMask
			);
		}
		else
		{
			// Get the main screen format.
			Assert(prasMainScreen);
			pxf = prasMainScreen->pxf;
		}

		// Create a new clut table of the appropriate size.
		{
			delete[] aTable;
			uint32 u4_clut_size = iNumEntries * iNumRampValues * iNumDepthValues * iSizeofPixel;
			aTable = new char[u4_clut_size];
		}

		uint16* au2_table = (uint16*)aTable;
		
		// For each alpha setting, blend the table.
		for (int i_band = 0; i_band < iNumRampValues; ++i_band)
		{
			uint16* au2           = &au2_table[i_band * iNumEntries];
			TLbs&   lbs           = lbsNonAlphaWater[i_band];
			float   f_minus_alpha = 1.0f - lbs.fAlpha;
			float   f_red_alpha   = lbs.fRed()   * lbs.fAlpha;
			float   f_green_alpha = lbs.fGreen() * lbs.fAlpha;
			float   f_blue_alpha  = lbs.fBlue()  * lbs.fAlpha;

			Assert(bWithin(lbs.fAlpha,    0.0f, 1.0f));
			Assert(bWithin(f_minus_alpha, 0.0f, 1.0f));
			Assert(bWithin(f_red_alpha,   0.0f, 1.0f));
			Assert(bWithin(f_green_alpha, 0.0f, 1.0f));
			Assert(bWithin(f_blue_alpha,  0.0f, 1.0f));

			// For each colour in the palette, blend with the alpha and set the table.
			for (int i_col = 0; i_col < iNumEntries; ++i_col)
			{
				// Get the colour.
				CColour clr     = ppalPalette->aclrPalette[i_col];
				float   f_red   = float(clr.u1Red)   / 255.0f;
				float   f_green = float(clr.u1Green) / 255.0f;
				float   f_blue  = float(clr.u1Blue)  / 255.0f;

				// Blend the colour.
				f_red   = f_red   * f_minus_alpha + f_red_alpha;
				f_green = f_green * f_minus_alpha + f_green_alpha;
				f_blue  = f_blue  * f_minus_alpha + f_blue_alpha;

				// Convert the colour back.
				clr.u1Red   = uint8(f_red   * 255.0f);
				clr.u1Green = uint8(f_green * 255.0f);
				clr.u1Blue  = uint8(f_blue  * 255.0f);
				au2[i_col] = (uint16)pxf.pixFromColour(clr);
			}
		}
	}

	//*****************************************************************************************
	void CClut::SetClutState(EClutState ecs)
	{
		ecsClutState = ecs;
	}

	//*****************************************************************************************
	void CClut::BuildD3DTransparentClut()
	{
		Assert(aTable);
		Assert(iSizeofPixel == 2);

		// Get the size of the clut in bytes.
		uint32 u4_num_entries = iNumEntries * iNumRampValues * iNumDepthValues;
		uint32 u4_clut_size   = u4_num_entries * iSizeofPixel;

		// Create a new clut table.
		if (aTableD3DTrans)
			delete[] aTableD3DTrans;
		aTableD3DTrans = new uint8[u4_clut_size];

		//
		// Repack pixels.
		//
		uint16* pu2_screen  = (uint16*)aTable;
		uint16* pu2_texture = (uint16*)aTableD3DTrans;

		switch (d3dDriver.d3dcomGetCommonFormat(ed3dtexSCREEN_TRANSPARENT))
		{
			case ed3dcom5551_BRGA:
			{
				switch (d3dDriver.esfGetScreenFormat())
				{
					case esf555:
					{
						// Convert 555 to 1555.
						for (uint32 u4 = 0; u4 < u4_num_entries; ++u4)
							pu2_texture[u4] = (pu2_screen[u4] << 1) | 0x0001;
						break;
					}

					case esfOther:
						Assert(0);
					case esf565:
					{
						for (uint32 u4 = 0; u4 < u4_num_entries; ++u4)
						{
							uint16 u2_blue      = pu2_screen[u4] & 0x001F;
							uint16 u2_red_green = pu2_screen[u4] & 0xFFC0;

							pu2_texture[u4] = u2_red_green | (u2_blue << 1) | 0x0001;
						}
						break;
					}

					default:
						Assert(0);
				}
				break;
			}

			case ed3dcom1555_ARGB:

				switch (d3dDriver.esfGetScreenFormat())
				{
					case esf555:
					{
						// Convert 555 to 1555.
						for (uint32 u4 = 0; u4 < u4_num_entries; ++u4)
							pu2_texture[u4] = pu2_screen[u4] | 0x8000;
						break;
					}

					case esfOther:
						Assert(0);
					case esf565:
					{
						// Convert 565 to 1555.
						for (uint32 u4 = 0; u4 < u4_num_entries; ++u4)
						{
							uint16 u2_blue      = pu2_screen[u4] & 0x001F;
							uint16 u2_red_green = pu2_screen[u4] & 0xFFC0;

							pu2_texture[u4] = (u2_red_green >> 1) | u2_blue | 0x8000;
						}
						break;
					}

					default:
						Assert(0);
				}

				break;

			default:;
		}
	}

	//*****************************************************************************************
	//
	void CClut::BuildD3DColourTable
	(
	)
	//
	//**************************************
	{
		// Do not build a conversion table if one already exists.
		if (ad3dtTable)
			return;

		// Allocate memory for the conversion table.
		ad3dtTable = new SD3DTable[iD3D_TABLE_SIZE];
	}


//
// Global variables.
//
uint16 u2FogSolidCol[iDefNumDepthValues];
EClutState CClut::ecsClutState = ecsSCREEN;

