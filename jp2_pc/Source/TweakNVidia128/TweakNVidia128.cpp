// TweakNVidia128.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include "TweakNVidia128.h"
#include "TweakNVidia128Dlg.h"
#include "ddraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTweakNVidia128App

BEGIN_MESSAGE_MAP(CTweakNVidia128App, CWinApp)
	//{{AFX_MSG_MAP(CTweakNVidia128App)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTweakNVidia128App construction

CTweakNVidia128App::CTweakNVidia128App()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTweakNVidia128App object

CTweakNVidia128App theApp;

/////////////////////////////////////////////////////////////////////////////
// CTweakNVidia128App initialization

BOOL CTweakNVidia128App::InitInstance()
{
	// Standard initialization

	switch (eDetectNVidiaRiva128())
	{
		case eNoDX6:
			::MessageBox(0, "DirectX 6.0 cannot be found. Please install DirectX 6.0.",
						 "Tweak NVidia Riva 128 Error", MB_OK | MB_ICONHAND);
			return FALSE;
			break;

		case eNoRiva128:
			::MessageBox(0, "Tweak compatible NVidia Riva 128 not found. Please consult the 'readme.txt'\n"
						 "file for more information.",
						 "Tweak NVidia Riva 128 Error", MB_OK | MB_ICONHAND);
			return FALSE;
			break;

		default:;
	}

	CTweakNVidia128Dlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

ETweakError eDetectNVidiaRiva128()
{
	GUID               guid;
	LPDIRECTDRAW       pdd  = 0;		// DirectDraw device.
	LPDIRECTDRAW4      pdd4 = 0;		// DirectDraw2 device.
	HRESULT            hres;
	DDDEVICEIDENTIFIER devidIdentifier;	// Device identifier.

	// Use a null guid.
	memset(&guid, 0, sizeof(GUID));

	// Create the direct draw device.
	hres = DirectDrawCreate(&guid, &pdd, 0);
	if (FAILED(hres) || !pdd)
		return eNoDX6;

	// Get the direct draw 2 interface.
    hres = pdd->QueryInterface(IID_IDirectDraw4, (LPVOID*)&pdd4);
	if (FAILED(hres) || !pdd4)
	{
		pdd->Release();
		return eNoDX6;
	}

	// Get the device identifier.
	pdd4->GetDeviceIdentifier(&devidIdentifier, 0);

	// Release the various interfaces and return.
	pdd4->Release();
	pdd->Release();

	// Compare IDs to determine if the device is a NVidia Riva 128.
	if (devidIdentifier.dwVendorId == 0x000012d2 &&
		devidIdentifier.dwDeviceId == 0x00000018)
	{
		return eSuccess;
	}
	
	return eNoRiva128;
}