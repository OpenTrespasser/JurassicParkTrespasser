/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Implementation of 'Palette.hpp.'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Palette.cpp                                                 $
 * 
 * 27    8/28/98 1:09p Asouth
 * added explicit pointer cast for delete operator
 * 
 * 26    8/20/98 5:37p Mmouni
 * Changed for new clut control stuff and removed hacky palette color match cache stuff.
 * 
 * 25    98.06.12 3:34p Mmouni
 * Added support for per quantization group CLUT start and stop colors.
 * 
 * 24    97/10/12 20:22 Speter
 * Removed bRefractive flag, clut now takes it from material.  Made u4GetHashValue() const,
 * with "mutable" behaviour.
 * 
 * 23    97/08/04 15:09 Speter
 * Changed iAverage to clrAverage.
 * 
 * 22    97/07/23 17:42 Speter
 * Replaced CPArray::Set() with assignment.
 * 
 * 21    97/06/10 21:50 Speter
 * Added iAverage function.
 * 
 * 20    97/02/19 10:32 Speter
 * Added bRefractive flag to palettes.
 * 
 * 19    96/12/31 17:17 Speter
 * Changed palette array to CAArray.
 * Added pplLookup member and associated functions.
 * 
 * 18    10/08/96 9:13p Agrant
 * Now uses WinInclude.hpp
 * 
 * 17    9/27/96 12:17p Cwalla
 * reduced MAX_CLR_CACHE
 * 
 * 16    8/13/96 4:07p Cwalla
 * added a quick hack to make it faster for subsequent
 * lookups, a small cache in fact...
 * 
 * 15    96/07/29 11:09 Speter
 * Now works with palettes less than 256 colours (but cluts are still 256 entries).
 * 
 * 14    7/25/96 10:39a Pkeet
 * Changed 'CPalette' to 'CPal.'
 * 
 * 13    96/07/22 15:37 Speter
 * Commented out assert for BI_RGB format temporarily.
 * 
 * 11    7/15/96 5:43p Pkeet
 * Changed 'delete aclrPalette' to 'delete[] aclrPalette.'
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
 * 5     96/05/31 10:46 Speter
 * Made compatible with new BGR CColour format.
 * Fixed bug: fopen was opening in "text" mode.  Changed to "rb".
 * 
 * 4     5/14/96 7:16p Pkeet
 * Moved 'to do' list. Moved 'u1MatchEntry' to .cpp module. Replaced 'pclrPalette' with
 * 'aclrPalette.' Replaced 'u4NoPaletteEntries' with 'uNumPaletteEntries.'
 * 
 * 3     96/05/08 20:39 Speter
 * Added constructor which simply allocates the palette with a specified number of entries.
 * 
 * 2     5/07/96 1:15p Pkeet
 * 
 * 1     5/03/96 5:32p Pkeet
 * 
 *********************************************************************************************/

#include "common.hpp"
#include "Palette.hpp"

#include "pallookup.hpp"
#include <Lib/W95/WinInclude.hpp>
#include <stdio.h>


//*********************************************************************************************
//
// CPal Member Functions.
//
//

	//*****************************************************************************************
	//
	// CPal constructors and destructors.
	//

	//*****************************************************************************************
	CPal::CPal(int i_num_entries, bool b_greyscale)
		: aclrPalette(i_num_entries)
	{
		Assert(i_num_entries <= 256);

		if (b_greyscale)
		{
			// Create a greyscale from black to white.
			for (uint u_entry = 0; u_entry < aclrPalette.uLen; u_entry++)
				aclrPalette[u_entry] = CColour((int)u_entry, (int)u_entry, (int)u_entry);
		}
		else
		{
			// Set all entries to black.
			for (uint u_entry = 0; u_entry < aclrPalette.uLen; u_entry++)
				aclrPalette[u_entry] = clrBlack;
		}

		u4HashValue = 0xFFFFFFFF;
		pccSettings.b_use_clut_colours = false;
	}

	//*****************************************************************************************
	CPal::CPal(const char* str_bitmap_name)
	{
		const uint32     u_bmp_signature = 0x4d42;	// Bitmap (.bmp) file signature.
		FILE*            pfl_file_in;				// Bitmap file stream.
		BITMAPFILEHEADER bfh_bitmap_file_header;	// Bitmap file header.
		BITMAPINFOHEADER binh_bitmap_info_header;	// Bitmap information.

		//
		// Open bitmap. Note that the file operations will be replaced when the file i/o
		// module is developed.
		//

		// Open file.
		pfl_file_in = fopen(str_bitmap_name, "rb");
		Assert(pfl_file_in);

		// Read in bitmap file header.
		fread(&bfh_bitmap_file_header, sizeof(bfh_bitmap_file_header), 1, pfl_file_in);
		Assert(bfh_bitmap_file_header.bfType == u_bmp_signature);

		// Read in bitmap information.
		fread(&binh_bitmap_info_header, sizeof(binh_bitmap_info_header), 1, pfl_file_in);
		Assert(binh_bitmap_info_header.biBitCount == sizeof(uint8) * 8);
//		Assert(binh_bitmap_info_header.biCompression == BI_RGB);

		int i_entries;

		// Get number of palette entries.
		if (binh_bitmap_info_header.biClrUsed == 0)
		{
			i_entries = 1 << binh_bitmap_info_header.biBitCount;
		}
		else
		{
			i_entries = binh_bitmap_info_header.biClrUsed;
		}

		// Create palette of appropriate size.
		aclrPalette = CPArray<CColour>(i_entries);

		// Read in the palette.  
		// Note that since CColour is compatible with PALETTEENTRY, no conversion is necessary.
		Verify(fread(aclrPalette, sizeof(CColour), aclrPalette.uLen, pfl_file_in) == aclrPalette.uLen);

		// Close the file.
		Verify(fclose(pfl_file_in) == 0);

		u4HashValue = 0xFFFFFFFF;
		pccSettings.b_use_clut_colours = false;
	}

	//*****************************************************************************************
	CPal::~CPal()
	{
		delete (CPaletteLookup*)pplLookup;
	}


	//*****************************************************************************************
	//
	// CPal member functions.
	//

	//*****************************************************************************************
	void CPal::MakeSystemFriendly()
	{
		Assert(aclrPalette);
		Assert(aclrPalette.uLen <= 256);

		uint8 u1_best_match;

		//
		// Make sure first palette entry is black.
		//
		if (aclrPalette[0].u4Value != clrBlack.u4Value)
		{
			// Get palette entry closest to black.
			u1_best_match = u1MatchEntry(clrBlack);

			// Swap palette entry with the first palette entry.
			Swap(aclrPalette[0], aclrPalette[(uint)u1_best_match]);

			// Set the first palette entry to black.
			aclrPalette[0] = clrBlack;
		}

		//
		// Make sure last palette entry is white.
		//
		if (aclrPalette[aclrPalette.uLen - 1].u4Value != clrWhite.u4Value)
		{
			// Get palette entry closest to white.
			u1_best_match = u1MatchEntry(clrWhite);

			// Swap palette entry with the first palette entry.
			Swap(aclrPalette[aclrPalette.uLen - 1], aclrPalette[(uint)u1_best_match]);

			// Set the first palette entry to white.
			aclrPalette[aclrPalette.uLen - 1] = clrWhite;
		}

		u4HashValue = 0xFFFFFFFF;
	}

	//*****************************************************************************************
	uint8 CPal::u1MatchEntry(CColour clr_target) const
	{
		int i_min_diff;

		return u1MatchEntry(clr_target, i_min_diff);
	}

	//*****************************************************************************************
	uint8 CPal::u1MatchEntry(CColour clr_target, int& i_difference) const
	{
		if ((const CPaletteLookup*)pplLookup)
		{
			// We have a fast lookup table.
			return pplLookup->u1MatchColour(clr_target);
		}

		uint u_test_index;
		uint u_best_index;
		int  i_current_diff;
		
		// Get the maximum difference of any two colours.
		i_difference = iDifference(aclrPalette[0], clr_target);
		u_best_index = 0;

		//
		// Search for the colour with the lowest difference to the target.
		//
		for (u_test_index = 1; u_test_index < aclrPalette.uLen; u_test_index++)
		{
			// Calculate the simple difference between the source and destination RGB colours.
			i_current_diff = iDifference(aclrPalette[u_test_index], clr_target);

			// Bail if difference is zero.
			if (i_current_diff == 0)
			{
				return (uint8)u_test_index;
			}

			// Choose lowest difference.
			if (i_current_diff < i_difference)
			{
				// Select best colour palette index.
				i_difference = i_current_diff;
				u_best_index = u_test_index;
			}
		}

		return (uint8)u_best_index;
	}

	//*****************************************************************************************
	void CPal::SetHashValue()
	{
		Assert(aclrPalette != 0);
		Assert(aclrPalette.uLen > 0);

		// Iterate through palette.
		NonConst(u4HashValue) = 0;
		for (uint u_pal_entry = 0; u_pal_entry < aclrPalette.uLen; u_pal_entry++)
		{
			u4HashValue ^= aclrPalette[u_pal_entry].u4Value << (u4HashValue & 2);
		}
	}

	//*****************************************************************************************
	void CPal::MakeFastLookup()
	{
		if (!pplLookup)
			pplLookup = new CPaletteLookup(this);
	}

	//*****************************************************************************************
	void CPal::DestroyFastLookup()
	{
		delete (CPaletteLookup*)pplLookup;
		pplLookup = 0;
	}

	//*****************************************************************************************
	CColour CPal::clrAverage() const
	{
		Assert(aclrPalette.uLen);

		// Add up all the colours, storing in uint32s for range and precision.
		uint32 u4_red   = 0;
		uint32 u4_green = 0;
		uint32 u4_blue  = 0;

		for (int i = 0; i < aclrPalette.uLen; i++)
		{
			u4_red   += aclrPalette[i].u1Red;
			u4_green += aclrPalette[i].u1Green;
			u4_blue  += aclrPalette[i].u1Blue;
		}

		// Divide by the number of colours.
		u4_red   /= aclrPalette.uLen;
		u4_green /= aclrPalette.uLen;
		u4_blue  /= aclrPalette.uLen;

		return CColour((int)u4_red, (int)u4_green, (int)u4_blue);
	}

