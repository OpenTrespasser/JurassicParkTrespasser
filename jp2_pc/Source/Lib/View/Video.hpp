/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Description of hardware screen modes available.
 *
 * Bugs:
 *		DirectDraw currently finds only a subset of the screen modes.
 *		At least on Scott's Compaq system, no 320 pixel modes are available.
 *		The iTotalVideoMemory var isn't set, because DirectDraw returns 0.
 *
 * To do:
 *		Convert ascrmdlist to STL vector, and use STL sorting (too unwieldy?).
 *
 *		Take out the code for hard-wired 320 modes.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Video.hpp                                                    $
 * 
 * 10    8/23/98 3:30p Pkeet
 * Application defaults to 320x240 resolution. If that resolution is not available, it chooses
 * the next lowest resolution. If there are no lower resolutions, it selects the next highest
 * resolution.
 * 
 * 9     8/22/98 6:44p Pkeet
 * Removed the 'CInitVideo' class.
 * 
 * 8     7/05/98 8:04p Pkeet
 * Added code to prevent the enumeration of display modes greater than 640x480.
 * 
 * 7     12/04/97 3:53p Shernd
 * Pumped up the MaxModes up to 60.
 * 
 * 6     10/28/97 1:01p Shernd
 * Changed the DirectDraw Video abstraction to load on an explicit location.  This location is
 * now in GUIApp.cpp.  This change allows the ability to display error dialogs if DirectX cannot
 * be initialized.
 * 
 * 5     7/22/96 6:01p Mlange
 * Increased the maximum number of screen modes to 42. Updated the todo list.
 * 
 * 4     96/06/12 20:42 Speter
 * Code review changes:
 * Changed SScreenMode prefix from scrmode to scrmd.
 * Added comments.
 * 
 * 3     96/06/07 18:32 Speter
 * Added comments.
 * 
 * 2     96/06/07 10:46 Speter
 * Changed iTotalVideoMemory from long to int.
 * 
 * 1     96/06/07 9:57 Speter
 * Moved video mode stuff from DD module.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_VIDEO_HPP
#define HEADER_LIB_VIEW_VIDEO_HPP

struct SScreenMode
// Identify a screen mode.
// Prefix: scrmd
{
	int iW, iH, iBits;

	//******************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	SScreenMode()
		: iW(0), iH(0), iBits(16)
	{
	}

	// Parameter constructor.
	SScreenMode(int i_width, int i_height, int i_bits = 16)
		: iW(i_width), iH(i_height), iBits(i_bits)
	{
	}

	//******************************************************************************************
	//
	// Comparison operators.
	//
	// Notes:
	//		These operators are to be used for matching dimensions, not bit depths.
	//

	bool operator ==(const SScreenMode& scrmd) const
	{
		return iW == scrmd.iW && iH == scrmd.iH;
	}

	bool operator <(const SScreenMode& scrmd) const
	{
		if (iW == scrmd.iW)
			return iH < scrmd.iH;
		return iW < scrmd.iW;
	}

	bool operator >(const SScreenMode& scrmd) const
	{
		if (iW == scrmd.iW)
			return iH > scrmd.iH;
		return iW > scrmd.iW;
	}

};

namespace Video
{
	//
	// Variables describing video hardware in general terms.
	//
	extern int			iTotalVideoMemory;			// Total video card memory in bytes.
	const int			iMAX_MODES = 60;
	extern SScreenMode	ascrmdList[iMAX_MODES];		// The possible screen modes.
	extern int			iModes;						// Count of them.

	//******************************************************************************************
	//
	void EnumerateDisplayModes
	(
		bool b_highres = true	// Enables the enumeration of screen modes higher than 640x480.
	);
	//
	// Enumerates screen resolutions, and stores the results in 'ascrmdList.'
	//
	//**************************************

	//******************************************************************************************
	//
	void SetToValidMode
	(
		int& ri_width,	// Desired width.
		int& ri_height	// Desired height.
	);
	//
	// If the specified dimensions are found within the enumerated modes, this function will
	// do nothing. If the specified dimensions are not found within the enumeration, this function
	// will set the values to the next lowest dimension. If there are no lower dimensions, this
	// function will set the values to the next highest dimension.
	//
	//**************************************

};


#endif
