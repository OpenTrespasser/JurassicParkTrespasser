/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Implementation of DialogDummy.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogDummy.cpp                                               $
 * 
 * 2     3/31/97 3:03p Agrant
 * Bigger window for better font
 * 
 * 1     97/03/27 7:13p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#include "stdafx.h"
#include "guiapp.h"
#include "GUIAppDlg.h"
#include "ConIO.hpp"
#include "DialogDummy.hpp"


//*********************************************************************************************
//
// CDialogDummy implementation.
//

//*********************************************************************************************
CDialogDummy::CDialogDummy(CGUIAppDlg* pdlg, CConsoleEx* pcon)
	: CDialog(CDialogDummy::IDD, 0)
{
	const int i_width  = 332;
	const int i_height = 420;

	//{{AFX_DATA_INIT(CDialogDummy)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pdlgGuiApp  = pdlg;
	pconConsole = pcon;
	Create(IDD_DUMMY, pdlg);
	if (pconConsole)
		pconConsole->SetDialog(pdlg, this);

	// Set the initial position.
	MoveWindow
	(
		GetSystemMetrics(SM_CXSCREEN) - i_width,		// int horizontal position
		GetSystemMetrics(SM_CYSCREEN) - i_height - 32,	// int vertical position
		i_width,										// int width
		i_height,										// int height
		false 											// bool repaint flag
	);
}

//*********************************************************************************************
void CDialogDummy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDummy)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

//*********************************************************************************************
bool CDialogDummy::bIsVisible() const
{
	WINDOWPLACEMENT wndpl;	// Window information.

	// Get the dialog's placement and state information.
	GetWindowPlacement(&wndpl);
	return (wndpl.showCmd == SW_SHOW) ||
		   (wndpl.showCmd == SW_SHOWNORMAL);
}

//*********************************************************************************************
BOOL CDialogDummy::ShowWindow(bool b_show)
{
	if (b_show)
	{
		CDialog::ShowWindow(SW_SHOWNORMAL);

		// Make sure the parent window has the focus.
		SetWindowPos
		(
			(const CWnd*)HWND_TOP,					// Placement-order handle.
			0,										// Horizontal position.
			0,										// Vertical position.
			0,										// Width.
			0,										// Height.
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW	// Window-positioning flags.
		);

		// Make sure the parent window has the focus.
		GetParent()->SetActiveWindow();
	}
	else
		CDialog::ShowWindow(SW_HIDE);
	return 0;
}

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogDummy, CDialog)
	//{{AFX_MSG_MAP(CDialogDummy)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*********************************************************************************************
LRESULT CDialogDummy::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	//
	// Bypass the erase background message.
	//
	if (message == WM_ERASEBKGND)
	{
		// Blit black onto the screen.
		CRect rect;				// Dialog dimensions.
		CDC*  pcdc = GetDC();	// Dialog device context.

		// Get the dialog dimensions.
		GetClientRect(&rect);

		// Blit black.
		pcdc->BitBlt(0, 0, rect.Width(), rect.Height(), 0, 0, 0, BLACKNESS);

		// Release the device context.
		ReleaseDC(pcdc);

		// Display the console.
		if (pconConsole)
			pconConsole->Show();

		return TRUE;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}
