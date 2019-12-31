/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The implementation for ColourBase.hpp.
 *
 * To do:
 *		Rearrange several clut-related classes:
 *			Place CClut parameters and lighting code into CMaterial.  Clut points to CMaterial,
 *			CPal, and specific lookup table for the combination.
 *			Place the bump table pointer in CMaterial as well.
 *			Get rid of SSurface.  Place seterfFeatures into CTexture, and remove redundant
 *			CTexture flags.
 *			Get rid of CPalClut intermediate class.  Point directly to CClut from CTexture and
 *			CClutDB.
 *		Remove pceMain; takes unnecessary room.  Use a unique 1-entry palette for
 *		 solid-coloured objects.  Create ppalScreen8 for 8-bit support.
 *		Implement palette uniqueness in CPalette constructor rather than CClutDB.
 *
 * Bugs:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/ColourBase.cpp                                               $
 * 
 * 43    98.09.20 7:18p Mmouni
 * Fixed clut memory management.
 * 
 * 42    9/18/98 7:35p Pkeet
 * Pointers are now nulled after they are deleted.
 * 
 * 41    9/17/98 5:36p Pkeet
 * Changed way the main clut is built.
 * 
 * 40    8/20/98 5:38p Mmouni
 * Changes for additional clut control text props.
 * 
 * 39    98/08/07 0:11 Speter
 * Now show pre-lit clut stats as well.
 * 
 * 38    98.08.03 6:39p Mmouni
 * Now print object name and palette size when adding a palette.
 * 
 * 37    98/07/24 13:59 Speter
 * No longer show cluts in stats window, as sizes are shown in debug window.
 * 
 * 36    98/07/22 14:40 Speter
 * Fixed bug in pclutGet, causing Ctrl-F9 freeze
 * 
 * 35    7/13/98 3:05p Mlange
 * Added debug info printing functionality to the CLUT dbase.
 * 
 * 34    98.06.12 3:34p Mmouni
 * Added support for per quantization group CLUT start and stop colors.
 * 
 * 33    98.05.08 1:22p Mmouni
 * Added code so that palettes shared between materials are not deleted twice.
 * 
 * 32    5/04/98 9:01p Agrant
 * Fixed palette reset bug
 * 
 * 31    98/04/22 12:28 Speter
 * Now update clut in place if it exists, rather than deleting and re-creating (works better
 * with cached clut pointers, e.g. terrain texture).
 * 
 **********************************************************************************************/

// Flag to force all palettes to use the same clut, thereby increasing speed??
#define bUSE_ONE_CLUT	0

//
// Includes.
//

#include "Common.hpp"
#include "ColourBase.hpp"

#include "Lib/Renderer/Primitives/FastBumpTable.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Loader/Loader.hpp"

#include <set>

extern bool bIsTrespasser;

CProfileStat	psCluts/*("Cluts", &proProfile.psMain)*/;

//*********************************************************************************************
//
// CPalClut implementation.
//

	//*****************************************************************************************
	//
	// CPalClut constructor and destructor.
	//

	//*****************************************************************************************
	CPalClut::CPalClut(CPal* ppal, const CMaterial* pmat)
	{
		Assert(ppal);
		Assert(pmat);

		ppalPalette  = ppal;
		pmatMaterial = pmat;
		pclutClut    = 0;
		pBumpTable	 = 0;
	}

	//*****************************************************************************************
	CPalClut::~CPalClut()
	{
		if (pclutClut)
		{
			//dout << "Deleting pclutClut.\n";

			// We own the palette, clut, and bump tables.
			delete pBumpTable;
			pBumpTable = 0;
			delete pclutClut;
			pclutClut = 0;
		}
	}

	//*****************************************************************************************
	//
	// CPalClut member functions.
	//

	//*****************************************************************************************
	void CPalClut::UpdateClut(const CPixelFormat& pxf_dest)
	{
		Assert(pxf_dest);

		if (pclutClut)
		{
			if (ppalPalette->pccSettings.b_use_clut_colours)
			{
				// Update clut in place.
				pclutClut->Update(pxf_dest, CClu(pmatMaterial, iDefNumRampValues, 
								  iDefNumDepthValues, lvDefStart, 
								  ppalPalette->pccSettings.clrClutStart, 
								  ppalPalette->pccSettings.clrClutStop,
								  clrDefEndDepth,
								  ppalPalette->pccSettings.clrClutReflection));
			}
			else
			{
				// Update clut in place.
				pclutClut->Update(pxf_dest, CClu(pmatMaterial));
			}
		}
		else
		{
			//dout << "Allocating pclutClut.\n";
			if (ppalPalette->pccSettings.b_use_clut_colours)
			{
				// Create the new clut with the associated palette and target format.
				pclutClut = new CClut(CClu(pmatMaterial, iDefNumRampValues, 
									  iDefNumDepthValues, lvDefStart, 
									  ppalPalette->pccSettings.clrClutStart, 
									  ppalPalette->pccSettings.clrClutStop,
									  clrDefEndDepth,
									  ppalPalette->pccSettings.clrClutReflection), 
									  ppalPalette, pxf_dest);
			}
			else
			{
				// Create the new clut with the associated palette and target format.
				pclutClut = new CClut(CClu(pmatMaterial), ppalPalette, pxf_dest);
			}
		}

		// Update any associated bump table as well.
		if (pBumpTable)
			pBumpTable->Update();
	}

//*********************************************************************************************
//
// CPalClutLess implementation.
//

	inline bool CPalClutLess::operator()
	(
		const CPalClut& pce_0,
		const CPalClut& pce_1
	) const
	//
	// Returns a comparison between the two hash values of the two palettes in
	// CPalClut.
	//
	//**************************************
	{
		Assert(pce_0.ppalPalette);
		Assert(pce_1.ppalPalette);

#if bUSE_ONE_CLUT
		// Pretend all palettes are the same.
		return false;
#else
		int i = pce_0.ppalPalette->u4GetHashValue() - pce_1.ppalPalette->u4GetHashValue();
		if (i != 0)
			return i < 0;
		else
			// Compare material addresses rather than values, assuming they are unique.
			return pce_0.pmatMaterial < pce_1.pmatMaterial;
#endif
	}


//*********************************************************************************************
//
// CPalClutDatabase implementation.
//

	//*****************************************************************************************
	//
	// CPalClutDatabase constructor and destructor.
	//

	//*****************************************************************************************
	CPalClutDatabase::CPalClutDatabase()
		: pceMainPalClut(0), ppalMain(0)
	{
		psetPalClut = new std::set<class CPalClut,class CPalClutLess>;
		psetOwnedPals = new std::set<class CPal*,class CPalPtrLess>;
	}

	//*****************************************************************************************
	CPalClutDatabase::~CPalClutDatabase()
	{
		// Delete the palettes we own.
		std::set<class CPal*,class CPalPtrLess>::iterator it_palptr;
		for (it_palptr = psetOwnedPals->begin(); it_palptr != psetOwnedPals->end(); it_palptr++)
			delete *it_palptr;
	
		// Delete the main palette as well.
		delete ppalMain;
		
		// Delete the palette list.
		delete psetOwnedPals;

		// Delete the CLUTS.
		delete psetPalClut;

		// Null pointers.
		psetOwnedPals = 0;
		psetPalClut   = 0;
		ppalMain      = 0;
	}

	//*****************************************************************************************
	//
	// CPalClutDatabase member functions.
	//

	//*****************************************************************************************
	CPalClut* CPalClutDatabase::ppceFindEntry(const CPal* ppal, const CMaterial* pmat) const
	{
		Assert(ppal);
		Assert(pmat);

		std::set<CPalClut, CPalClutLess>::iterator it_palclut_db;

		//
		// Search for CPalClut in the set.
		//
		it_palclut_db = psetPalClut->find(CPalClut(const_cast<CPal*>(ppal), pmat));
		if (it_palclut_db == psetPalClut->end())
		{
			// Palette not found.
			return (CPalClut*)0;
		}
		else
		{
			// Palette found, return pointer.
			return const_cast<CPalClut*>(&*it_palclut_db);
		}
	}

	//*****************************************************************************************
	CPalClut* CPalClutDatabase::ppceAddEntry(CPal* ppal, const CMaterial* pmat, bool b_is_main_palette)
	{
		Assert(ppal);
		Assert(pmat);

		//
		// Search for palette & material in the array.
		//
		CPalClut* ppce = ppceFindEntry(ppal, pmat);

		if (ppce)
		{
			//
			// CPalClut found, destroy duplicate palettes.
			//
			if (ppal != ppce->ppalPalette)
			{
				// We already have this palette.
				delete ppal;
			}

			// Otherwise, we've already added this very same CPalClut object before.  We're done.
		}
		else
		{
			#if VER_TEST
				if (CLoadWorld::pgonCurrentObject)
					dout << CLoadWorld::pgonCurrentObject->strObjectName << " - new palette/material pair (" << ppal->aclrPalette.size() << ").\n";
				else
					dout << "Global - new palette/material pair (" << ppal->aclrPalette.size() << ").\n";
			#endif

			// Palette not found, add to list.
			psetPalClut->insert(CPalClut(ppal, pmat));
			ppce = ppceFindEntry(ppal, pmat);

			// Check that a valid entry was returned.
			Assert(ppce);

			// If the palette is the main palette, make it so!
			if (b_is_main_palette)
			{
				Assert(pceMainPalClut == 0);
				pceMainPalClut = ppce;
				Assert(pceMainPalClut);
			}
			else
			{
				// Add the CPal to the set of palettes we own.
				if (psetOwnedPals->find(ppal) == psetOwnedPals->end())
					psetOwnedPals->insert(ppal);
			}

			if (pxfDest)
			{
				// Update the clut.
				ppce->UpdateClut(pxfDest);
			}
		}	

		return ppce;
	}

	//*****************************************************************************************
	CPalClut* CPalClutDatabase::ppceAddEntry(const char* str_palette_filename, const CMaterial* pmat,
		                                     bool b_is_main_palette)
	{
		Assert(str_palette_filename);
		Assert(*str_palette_filename);

		// Create the palette from the file.
		CPal* ppal = new CPal(str_palette_filename);

		return ppceAddEntry(ppal, pmat, b_is_main_palette);
	}

	//*****************************************************************************************
	void CPalClutDatabase::UpdateCluts(const CPixelFormat& pxf)
	{
		// Reset the clut count for stats.
		psCluts.Reset();

		std::set<CPalClut, CPalClutLess>::iterator it_palclut_db;

		// Iterate through list of palette-clut objects.
		for (it_palclut_db = psetPalClut->begin(); it_palclut_db != psetPalClut->end(); it_palclut_db++)
		{
			//
			// Update the clut for the palette-clut entry. The access to the member function
			// must go through a pointer because the STL "set" does not seem to access the set
			// element directly.
			//
			CPalClut& ppalclut = const_cast<CPalClut&>(*it_palclut_db);

			// Update only if the pixel format has changed.
			if (pxfDest != pxf)
				ppalclut.UpdateClut(pxf);
		}

		pxfDest = pxf;
	}

	//*****************************************************************************************
	void CPalClutDatabase::UpdateCluts()
	{
		// We must already have previously created cluts.
		Assert(pxfDest);

		// Reset the clut count for stats.
		psCluts.Reset();

		std::set<CPalClut, CPalClutLess>::iterator it_palclut_db;

		// Iterate through list of palette-clut objects.
		for (it_palclut_db = psetPalClut->begin(); it_palclut_db != psetPalClut->end(); it_palclut_db++)
		{
			//
			// Update the clut for the palette-clut entry. The access to the member function
			// must go through a pointer because the STL "set" does not seem to access the set
			// element directly.
			//
			CPalClut& ppalclut = const_cast<CPalClut&>(*it_palclut_db);

			Assert(ppalclut.pclutClut);

			ppalclut.UpdateClut(pxfDest);
		}
	}

	//*****************************************************************************************
	CPalClut* CPalClutDatabase::ppceFindColourMatch(CColour clr, int& i_index) const
	{
		std::set<CPalClut, CPalClutLess>::iterator it_palclut_db;
		CPalClut* ppce_current;
		CPalClut* ppce_best;
		int       i_current_difference;
		int       i_best_difference;
		
		//
		// Get the first palette and test for a match. Because it is the first palette tested,
		// it will be the best so far.
		//
		it_palclut_db = psetPalClut->begin();
		ppce_best     = ppce_current = (CPalClut*)&(*it_palclut_db);

		Assert(ppce_best);
		Assert(ppce_best->ppalPalette);

		i_index = ppce_best->ppalPalette->u1MatchEntry(clr, i_best_difference);

		// Return if the match is perfect.
		if (i_best_difference == 0)
		{
			return ppce_best;
		}

		it_palclut_db++;

		//
		// Iterate through the rest of the palettes looking for a better match.
		//
		while (it_palclut_db != psetPalClut->end())
		{
			ppce_current = (CPalClut*)&(*it_palclut_db);

			Assert(ppce_current);
			Assert(ppce_current->ppalPalette);

			int i_new_index = ppce_current->ppalPalette->u1MatchEntry(clr, i_current_difference);

			// If the match is the best so far, store.
			if (i_current_difference < i_best_difference)
			{
				i_index           = i_new_index;
				ppce_best         = ppce_current;
				i_best_difference = i_current_difference;
			}

			// Return if the match is perfect.
			if (i_best_difference == 0)
			{
				return ppce_best;
			}

			// Iterate through palette-clut entries.
			it_palclut_db++;
		}

		// Return best match.
		return ppce_best;
	}

	//*****************************************************************************************
	CClut* CPalClutDatabase::pclutGet(int i_index)
	{
		if (i_index < 0 || i_index >= psetPalClut->size())
			return 0;

		std::set<CPalClut, CPalClutLess>::iterator it_palclut_db;
		it_palclut_db = psetPalClut->begin();

		while (i_index > 0)
		{
			++it_palclut_db;
			--i_index;
		}

		return (*it_palclut_db).pclutClut;
	}

	//*****************************************************************************************
	void CPalClutDatabase::Clear()
	{
		AlwaysAssert(ppalMain);

		// Delete the palettes we own.
		std::set<class CPal*,class CPalPtrLess>::iterator it_palptr;
		for (it_palptr = psetOwnedPals->begin(); it_palptr != psetOwnedPals->end(); it_palptr++)
			delete *it_palptr;

		//
		// Regenerate the main palette.
		//
		pceMainPalClut = 0;
		delete psetPalClut;
		delete psetOwnedPals;
		psetPalClut = new std::set<class CPalClut,class CPalClutLess>;
		psetOwnedPals = new std::set<class CPal*,class CPalPtrLess>;
		CreateMainClut();
	}

	//*****************************************************************************************
	void CPalClutDatabase::CreateMainClut()
	{
		// Do nothing if the main clut is already created.
		if (pceMainPalClut)
			return;

		// Add the main clut using the main palette and the default material type.
		ppceAddEntry(ppalMain, &matDEFAULT, true);
	}

	//*****************************************************************************************
	void CPalClutDatabase::PrintDebugInfo() const
	{
		// We must already have previously created cluts.
		Assert(pxfDest);

		uint32 u4_num_clut_bytes        = 0;
		uint32 u4_num_bump_clut_bytes   = 0;
		int i_num_bump_clut_entries     = 0;
		uint32 u4_num_prelit_clut_bytes = 0;
		int i_num_prelit_clut_entries   = 0;

		// Iterate through list of palette-clut objects.
		for (std::set<CPalClut, CPalClutLess>::iterator it_palclut_db = psetPalClut->begin(); it_palclut_db != psetPalClut->end(); ++it_palclut_db)
		{
			uint32 u4_size = (*it_palclut_db).pclutClut->u4GetMemSize();

			u4_num_clut_bytes += u4_size;

			// Recognise a CLUT for bumpmapped material by virtue of its smaller size.
			if (u4_size <= 4*1024)
			{
				i_num_prelit_clut_entries++;
				u4_num_prelit_clut_bytes += u4_size;
			}
			else if (u4_size <= 64 * 1024)
			{
				i_num_bump_clut_entries++;
				u4_num_bump_clut_bytes += u4_size;
			}
		}

		#if VER_TEST
			dout << "Total CLUT dbase entries    : " << psetPalClut->size() << '\n';
			dout << "Total CLUT memory           : " << (u4_num_clut_bytes      + 512) / 1024 << "KB\n";
			dout << "Total bumpmapped entries    : " << i_num_bump_clut_entries << '\n';
			dout << "Total bumpmapped CLUT memory: " << (u4_num_bump_clut_bytes + 512) / 1024 << "KB\n";
			dout << "Total prelit entries        : " << i_num_prelit_clut_entries << '\n';
			dout << "Total prelit CLUT memory    : " << (u4_num_prelit_clut_bytes + 512) / 1024 << "KB\n";
		#endif
	}

	//*****************************************************************************************
	//
	// Main global variable.
	//

	CPalClutDatabase pcdbMain;

