/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of ColourDrawTest.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/FastBumpTest.cpp                                                $
 * 
 * 12    96/12/31 17:08 Speter
 * Updated for rptr and new CRaster funcs.
 * 
 * 11    96/10/28 15:00 Speter
 * Updated for new UpdateCluts interface.
 * 
 * 10    96/10/04 18:07 Speter
 * Removed include of defunct ObjDef*D.hpp files.
 * 
 * 9     9/24/96 3:00p Pkeet
 * Not working, but didn't want them hanging around.
 * 
 * 8     96/09/16 12:26 Speter
 * Updated for simplified CBumpMap structure.
 * 
 * 7     96/09/13 14:49 Speter
 * Corrected variable prefixes.
 * 
 * 6     9/11/96 11:39a Pkeet
 * Changed references of 'pcePalClut' to 'ppcePalClut,' added the 'ObjDef3D.hpp' include.
 * 
 * 5     9/06/96 6:01p Pkeet
 * Moved lookup functions to external assembly routines.
 * 
 * 4     9/05/96 6:34p Pkeet
 * Changed bumpmap to use the 'CRasterSimple' class. Added lighting parameters to the
 * 'MakeBumpmap' Table function.
 * 
 * 3     9/05/96 2:28p Pkeet
 * Added texturing to bumpmapping.
 * 
 * 2     9/04/96 5:16p Pkeet
 * Added file-based bumpmaps.
 * 
 **********************************************************************************************/

#include <math.h>
#include <memory.h>
#include "Common.hpp"
#include "FastBumpTest.hpp"
#include "Lib/Types/FixedP.hpp"
#include "Lib/Renderer/Primitives/FastBump.hpp"
#include "Lib/Renderer/Primitives/FastBumpTable.hpp"
#include "Lib/Renderer/Primitives/P5/FastBumpEx.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Sys/TextOut.hpp"


rptr<CBumpMap> pBump;

CBumpAnglePair Light;

const double dAmbient = 0.1;

	CClut* pclut;
	CPalClut*	PalClut;

void MakeBumpmap()
{
	pBump = new CBumpMap("bindata/bw.bmp", "bindata/col.bmp");

	CTexture tex(pBump);

	PalClut = tex.ppcePalClut;
}

bool b_okay = false;

//*********************************************************************************************
void DrawSpeedBump(CRaster* prasDest, int i_xshift, int i_yshift)
{
	int i, j;
	uint8* au1SubBumpToIntensity;
	uint32 u4LightTheta;

	i_xshift *= iBumpmapLen;
	i_yshift *= iBumpmapLen;

	//
	// Get the clut associated with the bumpmap.
	//
	if (!b_okay)
	{
		pcdbMain.UpdateCluts(prasDest->pxf);
		b_okay = true;
	}
	/*
	if (pBump->ppcePalClut->pclutClut == 0)
	{
		pcdbMain.UpdateCluts(prasDest);
	}
	pclut = pBump->ppcePalClut->pclutClut;

	Assert(pclut);
*/
	pclut = PalClut->pclutClut;
	//
	// Set values for quick table lookup.
	//
	au1SubBumpToIntensity = Light.pu1GetSubBumpTable(au1BumpToIntensity);
	u4LightTheta          = Light.u4IsolateTheta();

	for (i = 0; i < iBumpmapLen; i++)
	{
		for (j = 0; j < iBumpmapLen; j++)
		{
			//int i_intensity = au1SubBumpToIntensity[pBump->pixGet(i, j) - Light];
			int i_intensity = u1GetIntensityFromBumpmap
			(
				pBump->pixGet(i, j),
				u4LightTheta,
				au1SubBumpToIntensity
			);
			uint32 u4_pixel = pclut->Convert(pBump->pixGet(i, j).u1GetColour(), i_intensity, 0);
			prasDest->PutPixel
			(
				i + i_xshift,
				j + i_yshift,
				u4_pixel
			);
		}
	}
}
