/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of dd.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/DD.cpp                                                        $
 * 
 * 24    8/22/98 8:53p Pkeet
 * The DirectDraw library is freed and reloaded whenever 'ReleaseAll' is called as a safety
 * precaution.
 * 
 * 23    8/22/98 6:41p Pkeet
 * Made the global Direct3D object initialization and uninitialization occur only from the
 * 'CInitDD' object.  Added the 'ReleaseAll' member function to the 'CInitDD' object. Made a
 * single global instance of the  'CInitDD' object.
 * 
 * 22    7/24/98 11:45a Shernd
 * Added IsCertified and Hardware checks for DirectDraw Init
 * 
 * 21    7/20/98 10:32p Pkeet
 * Upgraded to Direct3D 4's interface.
 * 
 * 20    12/09/97 4:56p Pkeet
 * Changed initialization to support multiple video cards and Direct3D.
 * 
 * 19    10/28/97 1:01p Shernd
 * Changed the DirectDraw Video abstraction to load on an explicit location.  This location is
 * now in GUIApp.cpp.  This change allows the ability to display error dialogs if DirectX cannot
 * be initialized.
 * 
 * 1     96/04/18 16:49 Speter
 *
 **********************************************************************************************/

#include "Common.hpp"

#include "Dd.hpp"
#include "Lib/Sys/RegInit.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/DWSizeStruct.hpp"

//**********************************************************************************************
//
// namespace DirectDraw implementation.
//

namespace DirectDraw
{
	CCom<IDirectDraw4>	pdd4;

	//******************************************************************************************
	//
	// class CError implementation.
	//

		//**************************************************************************************
		void CError::operator =(int i_err)
		{
			//
			// Convert i_err to a resource symbol, and call the standard TerminalError function.
			// The app resources contain strings for all DirectDraw error codes.
			// The conversion consists of taking the low 16 bits of the error code, and adding
			// it to ERROR_DD_BASE.
			//
			if (i_err) 
				TerminalError(ERROR_DD_BASE + (uint16) i_err, true, "DirectDraw Error");
		}

		CError	err;
};


//**********************************************************************************************
//
// class CInitDD implementation.
//

CInitDD::CInitDD()
{
    m_hDDraw = NULL;
}


CInitDD::~CInitDD()
{
	ReleaseAll();
    if (m_hDDraw) 
    {
        FreeLibrary(m_hDDraw);
    }
}


//+--------------------------------------------------------------------------
//
//  Function:   CInitDD::Initialize
//
//  Synopsis:   Performs an explict load on the Direct Draw library.  Then
//              it QIs for the DirectDraw2 interface.
//
//  Returns:    int32 : -1 ddraw.dll was not loaded/located
//                      -2 DirectDrawCreate Address was not found
//                      -3 IDirectDraw interface was not created
//                      -4 IDirectDraw2 interface was not created
//
//  History:    23-Oct-97    SHernd  Created
//
//---------------------------------------------------------------------------
int32 CInitDD::Initialize()
{
	bool          b_use_reg    = bGetInitFlag();		// Flag for using the registry.
	GUID          guid         = ReadDDGUID();		// The GUID for direct draw.
	GUID*         pguid        = pConvertGUID(guid);	// Pointer to the GUID for direct draw.
	bool          b_fullscreen = bGetFullScreen();	// Flag indicating the application
													// should be run in fullscreen.
    int32       iRet;


    iRet = BaseInit();

	if (!b_use_reg || !b_fullscreen)
	{
		DirectDraw::err = DirectDraw::pdd4->SetCooperativeLevel(0, DDSCL_NORMAL);
	}

    return iRet;
}


int32 CInitDD::BaseInit()
{
	HRESULT      hres;
	IDirectDraw* pdd1;
	bool         b_use_reg    = bGetInitFlag();		// Flag for using the registry.
	GUID         guid         = ReadDDGUID();		// The GUID for direct draw.
	GUID*        pguid        = pConvertGUID(guid);	// Pointer to the GUID for direct draw.
	bool         b_fullscreen = bGetFullScreen();	// Flag indicating the application
													// should be run in fullscreen.

	// Force a dynamic load on DDRAW.DLL
    if (m_hDDraw == NULL)
    {
        m_hDDraw = LoadLibrary("ddraw.dll");
    }

	// If The direct Draw Library is not found on the user's
	// system then we'll want to bomb out.  The caller will have
	// to display an appropriate error dialog saying that ddraw.dll
	// could not be found.
	if (m_hDDraw == NULL)
	{
		return -1;
	}

    // Extract the DirectDrawCreate address out of ddraw.dll
    if (!DirectDraw::pdd4)
    {
		ReleaseAll();
    	
        const auto fp_DDrw = reinterpret_cast<decltype(&::DirectDrawCreate)>(GetProcAddress(m_hDDraw, "DirectDrawCreate"));
        if (!fp_DDrw)
		{
            return -2;
        }

        DirectDraw::err = (fp_DDrw)(pguid, &pdd1, 0);

        if (!pdd1)
        {
            return -3;
        }
		PrintD3D(">>>>>>New direct draw created\n");

        // Query for the DirectDraw4 interface.
        DirectDraw::err = pdd1->QueryInterface(IID_IDirectDraw4, (LPVOID*)&DirectDraw::pdd4);
        if (!DirectDraw::pdd4)
        {
            return -4;
        }
		PrintD3D(">>>>>>New direct draw 4 interface created\n");

		// Fail this function if Direct3D use is not set in the registry.
		if (bGetD3D() && IsHardwareSupported())
		{

			// Start up Direct3D.
			if (!d3dDriver.bInitializeD3D())
			{
				DirectDraw::pdd4.SafeRelease();
				PrintD3D(">>>>>>Direct3D interface not set up.\n");
				PrintD3D(">>>>>>DirectDraw4 interface released.\n");
				return 0;
			}
		}

        pdd1->Release();
		PrintD3D(">>>>>>DirectDraw1 interface released\n");
    }

    return 0;
}

void CInitDD::ReleaseAll()
{
	if (!m_hDDraw)
		return;

	if (DirectDraw::pdd4)
		PrintD3D(">>>>>>DirectDraw4 interface released.\n");

	d3dDriver.ReleaseD3D();
	DirectDraw::pdd4.SafeRelease();

	PrintD3D(">>>>>>Reloading DirectDraw library.\n");
    FreeLibrary(m_hDDraw);
    m_hDDraw = LoadLibrary("ddraw.dll");
}


bool CInitDD::IsCertified()
{
    CDDSize<DDCAPS> ddcaps;
    HRESULT ddrval;

    if (BaseInit() < 0)
    {
        return false;
    }

    ddrval = DirectDraw::pdd4->GetCaps(&ddcaps, NULL);
    if (ddrval != DD_OK)
    {
        return false;
    }
    
    if (ddcaps.dwCaps2 & DDCAPS2_CERTIFIED)
    {
        return true;
    }

    return false;
}


bool CInitDD::IsHardwareSupported()
{
    CDDSize<DDCAPS> ddcaps;
    HRESULT ddrval;

    if (BaseInit() < 0)
    {
        return false;
    }

    ddrval = DirectDraw::pdd4->GetCaps(&ddcaps, NULL);
    if (ddrval != DD_OK)
    {
        return false;
    }
    
    if (ddcaps.dwCaps & DDCAPS_NOHARDWARE)
    {
        return false;
    }

    return true;
}


//
// Create a single global instance of the DirectDraw object.
//
CInitDD g_initDD;

