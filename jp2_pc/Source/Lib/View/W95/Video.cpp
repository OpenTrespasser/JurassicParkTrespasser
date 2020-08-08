/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Video.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/W95/Video.cpp                                                $
 * 
 * 13    9/14/98 7:59p Pkeet
 * 512x384 screen resolution is suppressed for Rendition V1000 based cards.
 * 
 * 12    8/23/98 3:30p Pkeet
 * Application defaults to 320x240 resolution. If that resolution is not available, it chooses
 * the next lowest resolution. If there are no lower resolutions, it selects the next highest
 * resolution.
 * 
 * 11    8/22/98 6:44p Pkeet
 * Removed the 'CInitVideo' class.
 * 
 * 10    7/05/98 8:05p Pkeet
 * Added code to prevent the enumeration of display modes greater than 640x480.
 * 
 * 9     98/01/20 13:18 Speter
 * Now only allow 16-bit video modes.
 * 
 * 8     10/28/97 1:01p Shernd
 * Changed the DirectDraw Video abstraction to load on an explicit location.  This location is
 * now in GUIApp.cpp.  This change allows the ability to display error dialogs if DirectX cannot
 * be initialized.
 * 
 * 7     5/13/97 3:55p Cwalla
 * made EnumDisplayModes user DDEDM_REFRESHRATES because its not
 * clear if 0 is correct. It probably is, but having
 * the flag doesn`t hurt.
 * 
 * 
 * 6     8/23/96 10:39a Gstull
 * Removed hard-code adding lo-res modes.
 * 
 * 5     96/08/19 13:01 Speter
 * Reversed sorting comparison, so low-res modes now first.
 * 
 * 4     7/16/96 11:29a Mlange
 * Moved several files to different directories, updated includes.
 * 
 * 3     96/06/12 20:42 Speter
 * Code review changes:
 * Removed #include <DirectX/DDraw.h> because it's now in DD.hpp.
 * Fixed source code alignment.
 * Added comments.
 * 
 * 2     96/06/07 10:46 Speter
 * Changed iTotalVideoMemory from long to int.
 * 
 * 1     96/06/07 9:57 Speter
 * Moved video mode stuff from DD module.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/W95/DD.hpp"
#include "../Video.hpp"
#include "Lib/Sys/DWSizeStruct.hpp"
#include "Lib/W95/Direct3DCards.hpp"


//
// Local module variables.
//
bool bSuppress512x384 = false;


//**********************************************************************************************
//
namespace Video
//
//**************************************
{
	int				iTotalVideoMemory;
	SScreenMode		ascrmdList[iMAX_MODES];
	int				iModes;
	bool bEnumHighResolutions = true;

	//
	// Functions needed by CInitVideo.
	//

	//******************************************************************************************
	//
	static void AddScreenMode
	(
		int i_w, int i_h, int i_bits	// The screen mode dimensions.
	)
	// 
	// Adds the specified screenmode to ascrmdList.
	//
	//**********************************
	{
		// We now only support 16-bit rendering.
		if (i_bits != 16)
			return;

		// Search list to see if already there.
		for (int i = 0; i < iModes; i++)
		{
			if (ascrmdList[i].iW	== i_w &&
				ascrmdList[i].iH	== i_h &&
				ascrmdList[i].iBits == i_bits)
				return;
		}

		Assert(iModes < iMAX_MODES);

		// Not already there.
		ascrmdList[iModes].iW    = i_w;
		ascrmdList[iModes].iH    = i_h;
		ascrmdList[iModes].iBits = i_bits;
		iModes++;
	}

	//******************************************************************************************
	//
	static HRESULT CALLBACK EnumDisplayModesCallback
	(
		DDSURFACEDESC2* pddsd,			// Surface descriptor for a screen mode.
		void*							// Ignored.
	)
	// 
	// Callback for IDirectDraw::EnumDisplayModes().  Adds the display mode 
	// in ascrmdList via AddScreenMode().
	//
	// Returns:
	//		DDENUMRET_OK to indicate success.
	//
	//**********************************
	{
		if (!bEnumHighResolutions)
			if (pddsd->dwWidth > 800)
				return DDENUMRET_OK;

		// Suppress resolutions if required.
		if (bSuppress512x384 && pddsd->dwWidth == 512)
			return DDENUMRET_OK;

		AddScreenMode(pddsd->dwWidth, pddsd->dwHeight, 
			pddsd->ddpfPixelFormat.dwRGBBitCount);

		return DDENUMRET_OK;
	}

	//******************************************************************************************
	//
	static int _cdecl CompareModes
	(
		const void* p_1, const void* p_2
									// Pointers to SScreenMode structures.
	)
	//
	// Callback for qsort().  Compares two SScreenModes.
	// Modes sort in ascending order based first on bit depth, then width, then height.
	//
	// Returns:
	//		An int > 0 if p_1 sorts after p_2, < 0 if vice versa, 0 if equal.
	//
	//**********************************
	{
		SScreenMode* pscrmd_1 = (SScreenMode*) p_1;
		SScreenMode* pscrmd_2 = (SScreenMode*) p_2;
		int cmp;

		if (cmp = pscrmd_1->iBits - pscrmd_2->iBits)
			return cmp;
		if (cmp = pscrmd_1->iW - pscrmd_2->iW)
			return cmp;
		return pscrmd_1->iH - pscrmd_2->iH;
	}

	//******************************************************************************************
	//
	void EnumerateDisplayModes(bool b_highres)
	// 
	// Generate the list of available display modes.  Store it sorted in ascrmdList.
	//
	// Notes:
	//		Bug: We have discovered that DirectDraw's EnumDisplayModes function does not 
	//		enumerate the 320/?/8 modes on many cards, but that they work anyway.  
	//		So at least for now, we hard code these modes.
	//
	// Cross-references:
	//		Called by CModule below.
	//
	//**********************************
	{
		CDDSize<DDCAPS> ddcaps_hw, ddcaps_sw;
		bEnumHighResolutions = b_highres;
		EVideoCard evc;

		// Get the video card type.
		AlwaysAssert(DirectDraw::pdd4);
		if (DirectDraw::pdd4)
			evc = evcGetCard(DirectDraw::pdd4);
		else
			return;
		
		// Suppress resolutions based on the video card type.
		bSuppress512x384 = false;
		switch (evc)
		{
			case evcRenditionV1000:
				bSuppress512x384 = true;
				break;
		}

		// Find out the total video memory for this hardware.
		DirectDraw::err = DirectDraw::pdd4->GetCaps(&ddcaps_hw, &ddcaps_sw);

		// Bug: the following is always 0.
		iTotalVideoMemory = ddcaps_hw.dwVidMemTotal;

		// Use DirectDraw and our callback functions to list the available screen modes.
		iModes = 0;
		DirectDraw::err = DirectDraw::pdd4->EnumDisplayModes(DDEDM_REFRESHRATES, 0, 0, EnumDisplayModesCallback);

		qsort(ascrmdList, iModes, sizeof(*ascrmdList), CompareModes);
	}

	//******************************************************************************************
	void SetToValidMode(int& ri_width, int& ri_height)
	{
		int i_mode;
		SScreenMode scrmd(ri_width, ri_height);

		// Find the matching resolution if it is available.
		for (i_mode = 0; i_mode < iModes; ++i_mode)
		{
			if (scrmd == ascrmdList[i_mode])
				return;
		}

		// Find the next smallest dimension.
		for (i_mode = iModes - 1; i_mode >= 0; --i_mode)
		{
			if (scrmd > ascrmdList[i_mode])
			{
				ri_width  = ascrmdList[i_mode].iW;
				ri_height = ascrmdList[i_mode].iH;
				return;
			}
		}

		// Find the next largest dimension.
		for (i_mode = 0; i_mode < iModes; ++i_mode)
		{
			if (scrmd < ascrmdList[i_mode])
			{
				ri_width  = ascrmdList[i_mode].iW;
				ri_height = ascrmdList[i_mode].iH;
				return;
			}
		}

		//
		// The situation is dire if this point is reached. Use 640x480 as it is the most
		// universally supported dimension.
		//
		ri_width  = 640;
		ri_height = 480;
	}

};