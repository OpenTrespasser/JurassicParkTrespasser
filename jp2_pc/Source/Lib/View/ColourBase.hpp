/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		A database to manage palettes and associated cluts.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/ColourBase.hpp                                               $
 * 
 * 19    10/03/98 4:36a Rwyatt
 * Remove protected
 * 
 * 18    9/17/98 5:36p Pkeet
 * Changed way the main clut is built.
 * 
 * 17    8/28/98 1:08p Asouth
 * added constructor to meet C++ restrictions on template instantiation
 * 
 * 16    8/25/98 11:49a Rvande
 * Added #ifdef for forward declaration of class set (MSL uses std::)
 * 
 * 15    7/13/98 3:05p Mlange
 * Added debug info printing functionality to the CLUT dbase.
 * 
 * 14    98.05.08 1:22p Mmouni
 * Added code so that palettes shared between materials are not deleted twice.
 * 
 * 13    4/06/98 3:41p Agrant
 * Opaque set declaration for compile times (removed CContainer)
 * Added Clear function for resetting WDBase.
 * 
 * 12    98/02/10 12:54 Speter
 * Removed never-used SClutInit() argument from UpdateClut().
 * 
 * 11    97/10/12 20:31 Speter
 * Cluts and bump-tables are now customised for CMaterials.  CPalClut contains pointers to
 * pmatMaterial and pBumpTable.  Added ppceAddEntry(), removed ppalAddPalette() and
 * pclutFindClut().  Added UpdateCluts(), which forces an update.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_COLOURBASE_HPP
#define HEADER_LIB_VIEW_COLOURBASE_HPP

//
// Includes.
//

#include "Lib/View/Clut.hpp"
//#include "Lib/EntityDBase/Container.hpp"
//#include "Stl/Set.h"

#ifdef __MWERKS__
#include <set.h>
#else
// Note that this declares ::set (not std::set).
template <class ITEM, class COMPARE> class set;
#endif

class CBumpTable;

//
// Class definitions.
//

//*********************************************************************************************
//
class CPalClut
//
// An object to combine a palette and a clut.
//
// Prefix: pce
//
//**************************************
{

public:

	CPal*				ppalPalette;		// Source palette for clut.
	const CMaterial*	pmatMaterial;		// Source material for clut.
	CClut*				pclutClut;			// Clut created from palette nad material.
	CBumpTable*			pBumpTable;			// Custom bump-mapping table for this clut.

public:

	//*****************************************************************************************
	//
	// CPalClut constructor and destructor.
	//

	//*****************************************************************************************
	//
	CPalClut();
	//
	// Default constructor provided to ensure class can be instantiated
	// within a template (e.g. rb_tree).
	//
	//**************************************

	//*****************************************************************************************
	//
	CPalClut
	(
		CPal* ppal,							// Source palette.
		const CMaterial* pmat				// Source material.
	);
	//
	// Constructs the CPalClut object from a palette.
	//
	//**************************************

	//*****************************************************************************************
	~CPalClut();

	//*****************************************************************************************
	//
	// CPalClut member functions.
	//

	//*****************************************************************************************
	//
	void UpdateClut
	(
		const CPixelFormat& pxf_dest		// New destination pixel format.
	);
	//
	// Creates or updates the clut to match the source palette and the destination format.
	//
	//**************************************
};


//*********************************************************************************************
//
class CPalClutLess
//
// An object to compare two CPalClut objects.
//
// Prefix: Not required.
//
//**************************************
{

public:

	bool operator()
	(
		const CPalClut& pce_0,
		const CPalClut& pce_1
	) const;
	//
	// Returns a comparison between the two hash values of the two palettes in
	// CPalClut.
	//
	//**************************************
};


//*********************************************************************************************
//
class CPalPtrLess
//
// An object to compare two CPal* objects.
//
// Prefix: Not required.
//
//**************************************
{
public:
	bool operator()
	(
		CPal* const& ppal_0,
		CPal* const& ppal_1
	) const
	//
	// Returns a comparison between the addresses of the two palettes.
	//
	//**************************************
	{
		return ppal_0 < ppal_1;
	}
};


//*********************************************************************************************
//
class CPalClutDatabase
//
// Creates a database consisting of CPalClut objects.
//
// Prefix: pcdb
//
//**************************************
{
public:
	CPalClut* pceMainPalClut;					// Pointer to the CPalClut object for the main screen.
	CPal*     ppalMain;							// Main palette.

	set<CPalClut, CPalClutLess>* psetPalClut;	// STL Associate Container for CPalClut.
										
	set<CPal*, CPalPtrLess>*	 psetOwnedPals;	// Palettes we own (and need to delete).

	CPixelFormat pxfDest;						// The current pixel format that cluts convert to.

public:

	//*****************************************************************************************
	//
	// CPalClutDatabase constructor and destructor.
	//

	//*****************************************************************************************
	//
	CPalClutDatabase
	(
	);
	//
	// Initializes CPalClutDatabase.
	//
	//**************************************

	//*****************************************************************************************
	//
	~CPalClutDatabase
	(
	);
	//
	// Destroys all the memory associated with palettes and cluts.
	//
	//**************************************


	//*****************************************************************************************
	//
	// CPalClutDatabase member functions.
	//

	//*****************************************************************************************
	//
	CPalClut* ppceFindEntry
	(
		const CPal* ppal,					// Palette to find match for.
		const CMaterial* pmat				// Source material.
	) const;
	//
	// Finds a palette based on its hash value.
	//
	// Returns.
	//		The database entry containing the palette.
	//
	//**************************************

	//*****************************************************************************************
	//
	CPalClut* ppceAddEntry
	(
		CPal* ppal,							// Pointer to an already created palette.
		const CMaterial* pmat,				// Source material.
		bool b_is_main_palette = false		// Flag indicating whether the palette is to be
											// the main palette.
	);
	//
	// Adds a palette entry to the list of palettes, if not already there, and creates a clut 
	// for it if prasForCluts is set.
	//
	// Returns:
	//		A pointer to the CPalClut entry.
	//
	//**************************************

	//*****************************************************************************************
	//
	CPalClut* ppceAddEntry
	(
		const char* str_palette_filename,	// String of the filename containing the palette.
		const CMaterial* pmat,				// Source material.
		bool b_is_main_palette = false		// Flag indicating whether the palette is to be
											// the main palette.
	);
	//
	// Adds a palette from a file to the list of palettes. If the palette is already loaded,
	// this function will do nothing.
	//
	// Returns:
	//		A pointer to the CPalClut entry.
	//
	//
	//**************************************

	//*****************************************************************************************
	//
	void UpdateCluts
	(
		const CPixelFormat& pxf_dest		// New target pixel format.
	);
	//
	// Creates or updates cluts associated with source palettes.
	// Stores pxf_dest as the new pxfDest.
	//
	//**************************************

	//*****************************************************************************************
	//
	void UpdateCluts();
	//
	// Updates cluts associated with source palettes.
	//
	//**************************************

	//*****************************************************************************************
	//
	CPalClut* ppceFindColourMatch
	(
		CColour clr,	// Target colour.
		int&    i_index	// Palette index number of found colour.
	) const;
	//
	// Finds the palette and palette entry with the closest colour to the target colour.
	//
	// Returns.
	//		The database entry containing the palette with the closest colour match.
	//
	//**************************************

	//*****************************************************************************************
	//
	CClut* pclutGet(int i_index);
	//
	// Gets the indexed clut.
	//
	// Notes:
	//		Returns 0 if out of range.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Clear
	(
	);
	//
	// Clears the database of all palette and CLUT information.
	//
	//**************************************

	//*****************************************************************************************
	//
	void CreateMainClut
	(
	);
	//
	// Creates the main clut if there is not one already.
	//
	//**************************************

	//*****************************************************************************************
	//
	void PrintDebugInfo() const;
	//
	// Prints stats to the debug output stream, in VER_TEST builds only.
	//
	//**************************************
};


//*********************************************************************************************
//
// Main global variable.
//

extern CPalClutDatabase pcdbMain;


#endif
