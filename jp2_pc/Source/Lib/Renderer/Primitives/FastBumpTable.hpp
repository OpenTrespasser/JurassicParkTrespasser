/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Fast bumpmapping tables.
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/FastBumpTable.hpp                             $
 * 
 * 18    10/03/98 4:38a Rwyatt
 * Extern'ed bump table fill function.
 * 
 * 17    98/02/10 12:57 Speter
 * CBumpTable now requires just a CClu for conversions, not a CClut or CPalClut.
 * 
 * 16    97/10/12 20:38 Speter
 * Bump tables are now tailored to particular materials and cluts.  Translated bump mapping is
 * now used only for lighting changes.  Changed CBumpLookup to CBumpTable.  Now just one routine
 * is required for bump setup: bSetupBump.  Added Update() to force update.  Removed
 * au1BumpToCos primary table; now new primary table is calculated for each material.  Replaced
 * au1CosToIntensity with pau1IntensityTrans.  Removed au1BumpFirst.
 * 
 * 15    97/09/16 15:33 Speter
 * Removed old code for separate specular bump-mapping.  Changed SetupBump to return only
 * 'translate' flag.
 * 
 * 14    97/06/23 20:33 Speter
 * Gamma correction is now incorporated into bump-mapping.
 * 
 * 13    97-04-23 17:52 Speter
 * Removed specular structures by compile flag.
 * 
 * 12    97-04-23 14:40 Speter
 * Moved all bump lookup table setup into CBumpLookup::SetupBump(), from ScreenRenderDWI.  Now
 * fold specular lighting into translated bump-mapping, eliminating separate, slow specular
 * primitive.
 * Moved bangLight and bangSpecular variables here from MapT.hpp.
 * Removed code in disabled bREFLECTION_PRIMARY section.
 * 
 * 11    97/02/13 14:22 Speter
 * Incorporated new TAngleWidth into specular table building.
 * 
 * 10    1/09/97 8:07p Pkeet
 * Changed 'true' to 1 and 'false' to 0 for #defines used in #if statements.
 * 
 * 9     96/12/17 13:20 Speter
 * Set bREFLECTION_PRIMARY to false, reducing table size needed for water.
 * 
 * 8     96/12/05 14:32 Speter
 * Added bREFLECTION_PRIMARY flag to control method of reflection bump-mapping.  Moved
 * au1BumpFirst pointer into CBumpLookup, set automatically.
 * 
 * 7     96/12/04 11:46 Speter
 * Large reorganisation.  Put all bumpmap tables and functions into new CBumpLookup class.
 * Renamed tables and functions.
 * Added MakeBumpReflection() to make new angle-to-intensity table for water mapping.  Changed
 * MakeSpecularTable to take a flag indicating whether to calculate specular for water or
 * regular.
 * Made FillBumpTable take a function pointer for calculating values; reordered loop to reduce
 * calculations; took advantage of symmetry in theta.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_FASTBUMPTABLE_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_FASTBUMPTABLE_HPP


//
// Includes.
//
#include "FastBump.hpp"
#include "Lib/View/Clut.hpp"

//
// Defines.
//

// Maximum number of intensities that can be handled by the bumpmapper.
#define bFOLD_SPECULAR			1

//*********************************************************************************************
//
class CBumpTable
//
// Provides lookup tables for bump-angle to intensity conversions.
//
//**************************************
{
public:
	// Location of the light relative to the polygon's texture coordinates.
	static CBumpAnglePair bangLight;

	// Table for direct conversion of bumpmap angles to clut intensities.
	uint8 au1BumpToIntensity[iBUMP_TABLE_SIZE];

	// Scale table for translated bumpmap table lookups.
	CPArray<uint8> pau1IntensityTrans;
	
	const CClu* pcluCLU;				// Colour lookup this table is based on.
	float fLightTolerance;				// How much variance allowed before translation.
	SBumpLighting bltMain;				// Lighting values embodied in main table.	
	SBumpLighting bltTrans;				// Lighting values embodied in current translation tables.

public:

	//*********************************************************************************************
	//
	// Constructor.
	//

	CBumpTable(const CClu* pclu);

	//*********************************************************************************************
	//
	// Member functions.
	//

	//*********************************************************************************************
	//
	bool bSetupBump
	(
		const SBumpLighting& blt_main,		// Bump lighting for main scene lighting.
		const SBumpLighting& blt_poly		// Bump lighting for the current polygon.
	);
	//
	// Sets up all structures necessary for bump map primitives for this lighting.
	//
	// Returns:
	//		Whether to use translated bump primitive.
	//
	//**********************************

	//*********************************************************************************************
	//
	void Update();
	//
	// Causes entire table to be rebuilt.
	//
	//**********************************

protected:

	//*********************************************************************************************
	//
	void MakeTranslationTable
	(
		const SBumpLighting& blt			// Structure containing all bump lighting info.
	);
	//
	//
	//**************************************
};

//*********************************************************************************************
//
extern void FillBumpTable
(
	uint8* au1_bump,				// Table to fill.
	const SBumpLighting& blt,		// Lighting conditions.
	const CClu& clu					// Output clu.
);


#endif
