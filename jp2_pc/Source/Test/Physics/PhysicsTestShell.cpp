/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of PhysicsTestShell.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Physics/PhysicsTestShell.cpp                                    $
 * 
 * 4     97-03-28 16:37 Speter
 * Renamed preMain to prnshMain.
 * 
 * 3     96/10/28 15:01 Speter
 * Changed AppShell:: to CAppShell::.
 * 
 * 2     8/15/96 3:51p Pkeet
 * Corrected error in changing line colours.
 * 
 * 1     8/14/96 3:11p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "Lib/W95/WinInclude.hpp"

#include "GblInc/Common.hpp"
#include "Lib/Sys/TextOut.hpp"
#include "Shell/WinShell.hpp"
#include "Shell/AppShell.hpp"
#include "Shell/WinEvent.hpp"
#include "Lib/View/ColourBase.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "PhysicsTestShell.hpp"
#include "PhysicsTest.hpp"
#include "Lib/Loader/Loader.hpp"


//
// Module variables.
//

// Storage for the old pen object.
HPEN hpenOld = 0;

// Screen Device context.
HDC hdcScreen = 0;

// Virtual screen variables.
float fScale   = 0.0f;
int   iHeight  = 0;
int   iXCentre = 0;
int   iYCentre = 0;


//
// Function implementations.
//

//*********************************************************************************************
//
void SetLineViewport
(
	int i_width,	// Width of the raster to draw to.
	int i_height	// Height of the raster to draw to.
)
//
// Sets up the viewport for line drawing.
//
//**************************************
{
	fScale   = (float)Min(i_width, i_height) / 2.0f;
	iHeight  = i_height;
	iXCentre = i_width / 2;
	iYCentre = i_height / 2;
};

//*********************************************************************************************
bool bLeftMouseButton(bool b_is_in_window, bool b_is_in_rect)
{
	Assert(pwnsWindowsShell);
	Assert(pwnsWindowsShell->hwndMain);

	POINT pt;			// Windows 2D coordinate for mouse.
	bool  b_return_val;	// Return value.
	RECT  rect;	// Window position.

	// Get the mouse position.
	GetCursorPos(&pt);

	// Test the left mouse button.
	b_return_val = (GetAsyncKeyState(VK_LBUTTON) & (SHORT)0xFFFE) != 0;
		
	// The mouse is always over the window in full screen mode.
	if (prnshMain->bIsFullScreen)
	{
		return b_return_val;
	}

	// Test if mouse cursor is over a window.
	if (b_is_in_window)
	{
		b_return_val &= WindowFromPoint(pt) == pwnsWindowsShell->hwndMain;
	}

	// Test if mouse cursor is in the rectangle of the window.
	if (b_is_in_rect)
	{
		// Get the window position.
		GetWindowRect(pwnsWindowsShell->hwndMain, &rect);
		rect.top    += GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION) +
					   GetSystemMetrics(SM_CYSIZEFRAME);
		rect.bottom -= GetSystemMetrics(SM_CYSIZEFRAME) * 2;
		rect.left   += GetSystemMetrics(SM_CXSIZEFRAME);
		rect.right  -= GetSystemMetrics(SM_CXSIZEFRAME) * 2;

		// Test position.
		b_return_val &= pt.x > rect.left && pt.x < rect.right &&
		                pt.y > rect.top && pt.y < rect.bottom;
	}

	return b_return_val;
}

//*********************************************************************************************
void GetMousePosition(float& f_x, float& f_y)
{
	Assert(pwnsWindowsShell);
	Assert(pwnsWindowsShell->hwndMain);

	POINT pt;	// Windows 2D coordinate for mouse.
	RECT  rect;	// Window position.

	// Get the mouse position.
	GetCursorPos(&pt);

	// Get the window position.
	GetWindowRect(pwnsWindowsShell->hwndMain, &rect);
	if (!prnshMain->bIsFullScreen)
	{
		rect.top  += GetSystemMetrics(SM_CYMENU);
		rect.top  += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME);
		rect.left += GetSystemMetrics(SM_CXSIZEFRAME);
	}

	// Adjust the cursor position based on the window.
	pt.x -= rect.left;
	pt.y -= rect.top;

	// Convert the coordinates to virtual coordinates.
	f_x = (float)(pt.x - iXCentre) / fScale;
	f_y = (float)(iHeight - pt.y - iYCentre) / fScale;
}

//*********************************************************************************************
void Line(float f_x0, float f_y0, float f_x1, float f_y1)
{
	// Make sure the screen device context is available.
	if (hdcScreen == 0)
	{
		return;
	}

	// Draw the line.
	MoveToEx
	(
		hdcScreen,
		(int)(f_x0 * fScale) + iXCentre,
		iHeight - ((int)(f_y0 * fScale) + iYCentre),
		0
	);
	LineTo
	(
		hdcScreen,
		(int)(f_x1 * fScale) + iXCentre,
		iHeight - ((int)(f_y1 * fScale) + iYCentre)
	);
}

//*********************************************************************************************
void LineColour(int i_red, int i_green, int i_blue)
{
	// Make sure the screen device context is available.
	if (hdcScreen == 0)
	{
		return;
	}

	// Delete the old pen.
	if (hpenOld)
	{
		hpenOld = (HPEN)SelectObject(hdcScreen, GetStockObject(WHITE_PEN));
		if (hpenOld)
		{
			DeleteObject(hpenOld);
		}
	}

	// Create the new pen with the requested colour.
	HPEN hpen_new = CreatePen(PS_SOLID, 1, RGB(i_red, i_green, i_blue));
	hpenOld = (HPEN)SelectObject(hdcScreen, hpen_new);
}

//*********************************************************************************************
//
namespace TestMain
//
// Creates the test program namespace for integration with WinShell.
//
//**************************************
{
	void NewRaster(CRaster&);	// After window created.
	void Update();				// Update between frames.
	void Paint(CRaster&);		// Draw at current state.
};

//*********************************************************************************************
//
void WindowsEvent(uint u_message, WPARAM wp_param, LPARAM lp_param)
//
// Processes windows and WinEvent messages.
//
//**************************************
{
	switch (u_message)
	{
		// Destroy.
		case AM_DESTROY:
			Destroy();
			
			break;

		// Create.
		case AM_INIT:
			CAppShell::SetContinuous(true);
			Create();
			break;

		// Step.
		case AM_STEP:

			// Call the Step function in the test code.
			Step();

			// Fall through and paint.

		// Paint.
		case WM_PAINT:
			if (prasMainScreen)
			{
				// Clear the screen.
				prasMainScreen->Clear(0);

				// Set up the line drawing viewport.
				SetLineViewport(prasMainScreen->iWidth, prasMainScreen->iHeight);

				// Get the device contect for the raster.
				hdcScreen = prasMainScreen->hdcGet();

				// Create the new pen object and set its colour to white.
				LineColour(255, 255, 255);

				// Call the Paint function in the physics test module.
				Paint();

				// Delete the old pen object.
				if (hpenOld)
				{
					hpenOld = (HPEN)SelectObject(hdcScreen, hpenOld);
					DeleteObject(hpenOld);
				}

				// Restore the device context for the raster.
				prasMainScreen->ReleaseDC(hdcScreen);
				hdcScreen = 0;

				// Display the raster.
				prasMainScreen->Flip();
			}
			break;
	}
}


//Global variables and functions declared elsewhere as extern
//needed by the libraries
bool bIsTrespasser = false;
bool bUseReplayFile = false;
bool bInvertMouse = false;
bool bUseOutputFiles = false;
unsigned int g_u4NotifyParam = 0;
unsigned int u4LookupResourceString(int, char*, unsigned int) { return 0; }
PFNWORLDLOADNOTIFY g_pfnWorldLoadNotify = nullptr;
