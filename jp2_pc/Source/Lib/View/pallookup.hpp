/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CPaletteLookup
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/pallookup.hpp                                                $
 * 
 * 1     96/12/31 17:32 Speter
 * New file containing code moved from Clut.cpp.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_PALLOOKUP_HPP
#define HEADER_LIB_VIEW_PALLOOKUP_HPP

#include "Palette.hpp"

//**********************************************************************************************
//
class CPaletteLookup
//
// Prefix: plu
//
// Provides fast colour-matching for palettes.
//
//**************************
{
protected:
	
	#define MAX_LIST 258	

	typedef uint32 TListElem;
	// Prefix:le

	// Our list of list of pointers.
	TListElem*	aleRList[256 * 3];

	#define aleGlist(a) aleRList[a + 256]
	#define aleBList(a) aleRList[a + 256 + 256]

	CPal* ppalTarget;
	CColour clrLastMatch;
	uint8 u1LastMatch;

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CPaletteLookup(CPal *cppal_target);
	
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	uint8 u1MatchColour
	(
		CColour							// Colour to match.
	) const;
	//
	// Returns:
	//		Palette entry that matches the closest to the value passed.		
	//
	//**********************************

protected:

	uint32 u4RGBDiff(uint32 u4_index) const;
};

#endif