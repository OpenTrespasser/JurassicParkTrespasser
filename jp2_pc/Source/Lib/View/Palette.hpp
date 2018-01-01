/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		An 8 bit Windows palette object.
 *
 * Bugs:
 *		Read in BI_RLE8 bitmaps correctly.  Currently just assumes no compression.
 *
 * To do:
 *		Change the palette bitmap-based constructor to use the new file system. Build and test
 *		better colour matching systems.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Palette.hpp                                                 $
 * 
 * 20    8/20/98 5:38p Mmouni
 * Changes for additional clut control text props.
 * 
 * 19    98.06.12 3:34p Mmouni
 * Added support for per quantization group CLUT start and stop colors.
 * 
 * 18    1/29/98 7:49p Rwyatt
 * New member to set the hash value to a specific value, used by virtual loader.
 * 
 * 17    97/10/12 20:22 Speter
 * Removed bRefractive flag, clut now takes it from material.  Made u4GetHashValue() const,
 * with "mutable" behaviour.
 * 
 * 16    97/08/04 15:09 Speter
 * Changed iAverage to clrAverage.
 * 
 * 15    97/06/10 21:48 Speter
 * Added iAverage function.
 * 
 * 14    97/02/19 10:32 Speter
 * Added bRefractive flag to palettes.
 * 
 * 13    96/12/31 17:16 Speter
 * Changed palette array to CAArray.
 * Added pplLookup member and associated functions.
 * 
 * 12    7/25/96 10:40a Pkeet
 * Changed 'CPalette' to 'CPal.'
 * 
 * 11    96/07/22 15:37 Speter
 * Commented out assert for BI_RGB format temporarily.
 * 
 * 11    96/07/22 15:37 Speter
 * Commented out assert for BI_RGB format temporarily.
 * 
 * 10    7/13/96 4:05p Pkeet
 * Added greyscale capability for the default constructor.
 * 
 * 9     6/24/96 5:44p Pkeet
 * Added a matching member function that returned the result of the match and the difference
 * between the target and actual colours.
 * 
 * 8     6/24/96 5:14p Pkeet
 * Created the 'GetHashValue' member function and made 'u4HashValue' a protected member
 * variable.
 * 
 * 7     6/24/96 1:03p Pkeet
 * Added 'u4HashValue' member variable and 'SetHash' member function.
 * 
 * 6     6/21/96 6:30p Pkeet
 * Added the 'GetHashValue' member function to CPal.
 * 
 * 5     5/14/96 7:16p Pkeet
 * Moved 'to do' list. Moved 'u1MatchEntry' to .cpp module. Replaced 'pclrPalette' with
 * 'aclrPalette.' Replaced 'u4NoPaletteEntries' with 'uNumPaletteEntries.'
 * 
 * 4     96/05/08 20:39 Speter
 * Added constructor which simply allocates the palette with a specified number of entries.
 * 
 * 3     96/05/08 14:09 Speter
 * Updated for move of View/ files to Lib/View/
 * 
 * 2     5/07/96 1:15p Pkeet
 * 
 * 1     5/03/96 5:34p Pkeet
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_VIEW_PALETTE_HPP
#define HEADER_LIB_VIEW_PALETTE_HPP

//
// Includes and macro definitions.
//

#include "Lib/Std/Ptr.hpp"
#include "Lib/View/Colour.hpp"

class CPaletteLookup;

//*********************************************************************************************
//
struct SPalClutControl
//
// Some values that control how a clut is created from a palette.
//
//**************************************
{
	bool		b_use_clut_colours;	// Flag indicating we should use clut stop/start colours.
	CColour		clrClutStart;		// Clut start colour for this palette.
	CColour		clrClutStop;		// Clut stop colour for this palette.
	CColour		clrClutReflection;	// Clut stop colour for this palette.
};


//*********************************************************************************************
//
class CPal
//
// The CPal object contains an 8 bit Windows palette with associated behaviour functions.
//
// Prefix: pal
//
//**************************************
{

public:

	CAArray<CColour> aclrPalette;	// Define the array of palette entries.

	SPalClutControl pccSettings;	/// Controls how the clut is created.

protected:

	uint32   u4HashValue;			// Unique identifier for the palette.
	ptr<CPaletteLookup> pplLookup;	// Table for fast matching.

public:

	//*****************************************************************************************
	//
	// CPal constructors and destructors.
	//

	//*****************************************************************************************
	//
	CPal
	(
		int  i_num_entries = 256,	// Number of entries in the palette.
		bool b_greyscale = false
	);
	//
	// Constructs a palette of the number of entries specified, and sets them all initially to
	// black.  Client must manually set the entries. Will create a greyscale if b_greyscale is
	// set to true.
	//
	//**************************************

	//*****************************************************************************************
	//
	CPal
	(
		const char* str_bitmap_name		// Bitmap file name
	);
	//
	// Constructs a palette by reading in a bitmap file from a file name.
	//
	//**************************************

	//*****************************************************************************************
	//
	~CPal
	(
	);
	//
	// Destroys the palette object and returns previously allocated memory.
	//
	//**************************************


	//*****************************************************************************************
	//
	// CPal member functions.
	//

	//*****************************************************************************************
	//
	void MakeSystemFriendly
	(
	);
	//
	// Makes a palette friendly to the Windows direct draw system by ensuring that
	// aclrPalette[0] is black and aclrPalette[255] is white.
	//
	// Notes:
	//		This function will not alter colours 1 to 9 and 245 to 254, therefore a user's
	//		colour scheme will be affected while in windowed mode.
	//
	//**************************************

	//*****************************************************************************************
	//
	uint32 u4GetHashValue
	(
	) const
	//
	// Gets the hash value by returning the stored hash value or creating a new one.
	//
	// Returns:
	//		Returns value contained in u4HashValue.
	//
	//**************************************
	{
		//
		// If the hash value has not already been generated, generate one.
		if (u4HashValue == 0xFFFFFFFF)
		{
			const_cast<CPal*>(this)->SetHashValue();
		}
		return u4HashValue;
	}


	//*****************************************************************************************
	//
	// CPal colour matching functions.
	//

	//*****************************************************************************************
	//
	uint8 u1MatchEntry
	(
		CColour clr_target	// Target rgb colour to select a match for.
	) const;
	//
	// Matches the target RGB colour to the nearest palette entry.
	//
	// Returns:
	//		Returns the index value of the closest match to 'clr_target.'
	//
	//**************************************

	//*****************************************************************************************
	//
	uint8 u1MatchEntry
	(
		CColour clr_target,		// Target rgb colour to select a match for.
		int&    i_difference	// Matching difference between target and actual colour found.
	) const;
	//
	// Matches the target RGB colour to the nearest palette entry.
	//
	// Returns:
	//		Returns the index value of the closest match to 'clr_target.'
	//
	//**************************************

	//*****************************************************************************************
	//
	void MakeFastLookup();
	//
	// Creates a CPaletteLookup table (if not made already), for fast colour matching.
	//
	// Notes:
	//		When this table is created, u1MatchEntry will execute much faster, but additional
	//		memory will be used.
	//
	//**************************************

	//*****************************************************************************************
	//
	void DestroyFastLookup();
	//
	// Destroys the CPaletteLookup table, if made.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetHashValue
	(
	);
	//
	// Combines all the colour values in the palette with the xor function, and stores the
	// resulting hash value in 'u4HashValue.'
	//
	// Notes:
	//		Call this function after you have altered palette entries.
	//
	//**************************************


	//*****************************************************************************************
	//
	void SetHashValue
	(
		uint32	u4_hash
	)
	{
		u4HashValue = u4_hash;
	}
	//
	// Sets the palette hash value to the specified value
	//
	// Notes:
	//		This should only be called with a value that is correct for this palette. This
	//		intended for use with the loader when the original palette ID is saved in the GRF
	//		file. The saves having to generate it for every new palette loaded.
	//
	//**************************************


	//*****************************************************************************************
	//
	CColour clrAverage() const;
	//
	// Returns:
	//		The average colour of the palette.
	//
	//**********************************
 };

#endif
