/**********************************************************************************************
 *
 * $Source::																			      $
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of AI TestShell.hpp.
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/AITestShell.cpp                                              $
 * 
 * 25    9/23/98 10:19p Agrant
 * more code rot
 * 
 * 24    9/19/98 1:40a Agrant
 * code rot
 * 
 * 23    8/23/98 3:03a Agrant
 * AI Test compile fixes
 * 
 * 22    5/17/98 8:03p Agrant
 * numerous AI test app interface improvements
 * 
 * 21    5/16/98 9:58a Agrant
 * viewport fixes to allow moving the view volume
 * 
 * 20    5/10/98 6:20p Agrant
 * allow loading scenes
 * 
 * 19    5/09/98 11:17a Agrant
 * #include commander
 * 
 * 18    9/11/97 1:49p Agrant
 * Added SetMenuCheck function
 * 
 * 17    5/09/97 12:13p Agrant
 * Mouse player debug option
 * 
 * 16    4/29/97 6:41p Agrant
 * ambiguous float/CRating conversion fixed
 * 
 * 15    97-03-28 16:37 Speter
 * Renamed preMain to prnshMain.
 * 
 * 14    3/19/97 2:20p Agrant
 * Can now load GROFF files.  Now 0,0 is at center of screen.
 * 
 * 13    1/23/97 2:23p Agrant
 * Added step mode to AI test app.
 * 
 * 12    96/10/28 15:01 Speter
 * Changed AppShell:: to CAppShell::.
 * 
 * 11    10/23/96 7:39p Agrant
 * more debugging
 * now using the dialog box for printing some text
 * first pass pathfinder
 * 
 * 10    10/10/96 7:18p Agrant
 * Modified for code spec.
 * 
 * 9     10/02/96 8:10p Agrant
 * Added Sheep and Wolves.
 * Added Influence Debugging.
 * Added AI frame rate counter.
 * Unified all meter/pixel conversions.
 * 
 * 8     9/26/96 5:59p Agrant
 * AI system basic structure implemented
 * AI Test app has basic herding behavior based on two simple activities
 * TReal used as basic world coordinate type.
 * 
 * 7     9/03/96 7:51p Agrant
 * Fixed the bug where dinos would go past the bottom edge of the world.
 * 
 * 6     8/30/96 4:51p Agrant
 * objects now have facing and velocity
 * 
 * 5     8/29/96 5:31p Agrant
 * Major revision
 * Mostly changed code to match coding spec
 * Turned the object list into a class
 * Incorporated a timer
 * Now using TDefReal instead of float
 * 
 * 4     8/28/96 4:14p Agrant
 * Added menu item handler
 * 
 * 3     8/27/96 4:43p Agrant
 * MOdified to allow world coords instead of relative screen coords.
 * Added left mouse click as an event triggering a function call.
 * 
 * 2     8/21/96 3:46p Agrant
 * changed over to AI purposes
 * 
 * 1     8/20/96 5:22p Agrant
 *
 **********************************************************************************************/



#include "common.hpp"
#include "AITestShell.hpp"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/Types/FixedP.hpp"
#include "Lib/View/Colour.hpp"
#include "Lib/View/Palette.hpp"
#include "Lib/View/Viewport.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/View/ColourBase.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "Shell/winshell.hpp"
#include "Shell/AppShell.hpp"
#include "Shell/WinEvent.hpp"
#include "Lib/Sys/W95/Render.hpp"

#include "Game/AI/AITypes.hpp"
#include "Game/AI/Feeling.hpp"

#include "AITest.hpp"




//
// Module variables.
//

// TRUE when a step is requested
bool bStepRequested = FALSE;

// Storage for the old pen object.
HPEN hpenOld		= 0;

// Screen Device context.
HDC hdcScreen		= 0;

// Virtual screen variables.
// CViewport requires coordinates in a 0 to 1 scale.  I want meters.
TReal	rScale			= 1;		//	pixels per meter
int		iHeight			= 0;		//  height of screen in pixels
int		iWidth			= 0;		//  height of screen in pixels
int		iXOffset		= 0;		//  screen offset from world coords (in pixels)
int		iYOffset		= 0;			
int		iBorderWidth	= 5;		// A border left empty for clarity.

CVector2<>  v2WorldMax;				// The point with the biggest possible world coords
CVector2<>  v2WorldMin;				// The point with the smallest possible world coords

//
// Function implementations.
//



int iXPixels
(
	TReal r_x_meters
)
//
//	converts meters to pixels (X coordinate)
//
//	Returns:
//		offset in pixels from left side of window
//
//**************************************
{
	return (int)(r_x_meters * rScale - iXOffset);
}

//*********************************************************************************************
//
int iYPixels
(
	TReal r_y_meters
)
//
//	converts meters to pixels (Y coordinate)
//
//	Returns:
//		offset in pixels from top of window
//
//**************************************
{
	return iHeight - (int)(r_y_meters * rScale - iYOffset);
}


//*********************************************************************************************
//
TReal rXMeters
(
	int i_x_pixels
)
//
//	converts pixels to meters (X coordinate)
//
//	Returns:
//		offset in meters from world origin of screen X coordinate iXPixels
//
//**************************************
{
	return (TReal)(i_x_pixels + iXOffset) / rScale;
}

//*********************************************************************************************
//
TReal rYMeters
(
	int i_y_pixels
)
//
//	converts pixels to meters (Y coordinate)
//
//	Returns:
//		offset in meters from world origin of screen Y coordinate iYPixels
//
//**************************************
{
	return (TReal)(iHeight - i_y_pixels + iYOffset) / rScale;
}


//*********************************************************************************************
//
void SetViewCenter
( 
	TReal	r_x,	// X coord in world space
	TReal	r_y		// Y coord in world space
)
//
// Sets up the viewport for line drawing.
//
//**************************************
{								   
	// Want point i_x, i_y to be centered on the screen!
	iXOffset = r_x * rScale - iWidth/2;
	iYOffset = r_y * rScale - iHeight/2;

	SetViewScale(rScale);
}
	
//*********************************************************************************************
//
void SetViewScale
( 
	TReal	r_pixels_per_meter
)
//
// Sets up the viewport for line drawing.
//
//**************************************
{								   
	rScale = r_pixels_per_meter;

	// Find the height in pixels of the viewport.
	RECT rect;
	GetClientRect(pwnsWindowsShell->hwndMain, &rect);

	// Set the height.
	iHeight  = rect.bottom - rect.top;
	iWidth	 = rect.right - rect.left;

	v2WorldMax.tX = rXMeters(iWidth);
	v2WorldMin.tX = rXMeters(0);

	v2WorldMax.tY = rYMeters(0);
	v2WorldMin.tY = rYMeters(iHeight);

//	AlwaysAssert(v2WorldMax.tY > v2WorldMin.tY);
//	AlwaysAssert(v2WorldMax.tX > v2WorldMin.tX);
};

//*********************************************************************************************
bool bLeftMouseButton(bool b_is_in_window, bool b_is_in_rect)
{
	Assert(pwnsWindowsShell);
	Assert(pwnsWindowsShell->hwndMain);

	POINT pt;			// Windows 2D coordinate for mouse.
	bool  b_return_val;	// Return value.
	RECT  rect;			// Window position.

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
		b_return_val &= (WindowFromPoint(pt) == pwnsWindowsShell->hwndMain);
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
		b_return_val &= pt.x > rect.left	&& pt.x < rect.right &&
		                pt.y > rect.top		&& pt.y < rect.bottom	;
	}

	return b_return_val;
}

//*********************************************************************************************
void GetMousePosition(TReal& r_x, TReal& r_y)
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

	// Convert the coordinates to world coordinates.
	r_x = rXMeters(pt.x);
	r_y = rYMeters(pt.y);
}


//*********************************************************************************************
inline void LineDirect(int i_x0, int i_y0, int i_x1, int i_y1)
{
	// No safety checks.  Just do it.

	// Make sure the screen device context is available.
	if (hdcScreen == 0)
	{
		return;
	}

	if (i_x0 < 0 && i_x1 < 0)
		return;

	if (i_y0 < 0 && i_y1 < 0)
		return;

	if (i_y0 > iHeight && i_y1 > iHeight)
		return;

	if (i_x0 > iWidth && i_x1 > iWidth)
		return;

	// Draw the line.
	MoveToEx
	(
		hdcScreen,
		i_x0,
		i_y0,
		0
	);
	LineTo
	(
		hdcScreen,
		i_x1,
		i_y1
	);
}


//*********************************************************************************************
void Line(TReal r_x0, TReal r_y0, TReal r_x1, TReal r_y1)
{
	// Make sure the screen device context is available.
	if (hdcScreen == 0)
	{
		return;
	}

	LineDirect(int(iXPixels(r_x0)), int(iYPixels(r_y0)), int(iXPixels(r_x1)),	int(iYPixels(r_y1)));

}

//*********************************************************************************************
void Line(CVector2<> v2_from, CVector2<> v2_to)
{
	Line(v2_from.tX, v2_from.tY, v2_to.tX, v2_to.tY);
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
void Text
(
	TReal r_x,
	TReal r_y,
	char *pc_buffer
)
//
// Draws the given text at location r_x, r_y.  Coords are in meters from the lower left of the world.
//
//**************************************
{
	// Make sure the screen device context is available.
	if (hdcScreen == 0)
	{
		return;
	}


	SetTextColor
	(
		hdcScreen,	// handle of device context  
		RGB(255,255,255) 	// text color 
	);	
 
	SetBkMode
	(
		hdcScreen,	// handle of device context
		TRANSPARENT 	// flag specifying background mode
	);	
 
	// Set up start location of rectangle.
	RECT rect;
	rect.left	= iXPixels(r_x);
	rect.top	= iYPixels(r_y);

	rect.right	= rect.left + 100;
	rect.bottom	= rect.top  + 100;

	// Draw the text.
//	DrawText
	//(
		//hdcScreen,
		//pc_buffer,
		//strlen(pc_buffer),
		//&rect,
		//DT_LEFT
	//);

	TextOut(
		hdcScreen,	// handle of device context 
		rect.left,	// x-coordinate of starting position  
		rect.top,	// y-coordinate of starting position  
		pc_buffer,	// address of string 
		strlen(pc_buffer) 	// number of characters in string 
   );

/*	ExtTextOut(
		hdcScreen,	// handle to device context 
		rect.left,
		rect.top,
		0,
		ETO_TRANSPARENT,
		pc_buffer,
		strlen(pc_buffer),
		0
   );	
 */

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
// Processes windows and WinEvent messages.  Dispatches messages to Application-specific 
// functions.
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
			

			// Create the main palette if there isn't one.
			if (!pcdbMain.ppalMain)
			{
				pcdbMain.ppalMain = new CPal(256,true);
			}
		
			// Create the main clut if there isn't one.
		    pcdbMain.CreateMainClut();


			Create();

			break;

		// Step.
		case AM_STEP:

			// Call the Step function in the test code.
			Step();

			// Fall through to paint.
			// This will refresh the screen completely every frame.  Ick.

		// Paint.
		case WM_PAINT:
			if (prasMainScreen)
			{
				// Clear the screen. 
				prasMainScreen->Clear(0);

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
		
		// Window size has changed
		case WM_SIZE:
			// Update the viewport
			SetViewScale(rScale);
			break;
		
		// Left Mouse Button Pressed
		case WM_LBUTTONDOWN:
			// Notify the app
			LeftButtonClicked(	rXMeters(LOWORD(lp_param)),
								rYMeters(HIWORD(lp_param)));
			break;

		// Left Mouse Button Released
		case WM_LBUTTONUP:
			// Notify the app
			LeftButtonReleased(	rXMeters(LOWORD(lp_param)),
								rYMeters(HIWORD(lp_param)));
			break;

		
		// Right Mouse Button Pressed.
		case WM_RBUTTONDOWN:
			// Notify the app
			RightButtonClicked(	rXMeters(LOWORD(lp_param)),
								rYMeters(HIWORD(lp_param)));
			break;

		// Menu Item selected.
		case WM_COMMAND:
			// If we have an accelerator or menu selection
			if (HIWORD(wp_param) == 0	||
				HIWORD(wp_param) == 1)
			{
				// Call the command handler
				Command(LOWORD(wp_param));				
			}
			break;

		case WM_KEYDOWN:
			KeyPressed(wp_param);
			break;

		// All other events come here.
		default:
			// And are ignored.
			break;
	}
}


//*********************************************************************************
//
void DrawFeeling
(
	const CFeeling		&feel,
	const CVector2<>	&v2_upper_left
)
//
//	Draws the feeling at the specified location.
//
//******************************
{
	// Make sure the screen device context is available.
	if (hdcScreen == 0)
	{
		return;
	}

	// Let's convert to pixels so that our size is not dependent upon world scale.
	int i_left		= iXPixels(v2_upper_left.tX);
	int i_top		= iYPixels(v2_upper_left.tY);
	int i_width		= 20;
	float f_width	= i_width;
	

	// Draw a guide bar on the top and bottom.
//	LineColour(255,255,255);	
	//LineDirect(i_left, i_top, i_left + i_width, i_top);
	//LineDirect(	i_left, i_top + eptEND, 
				//i_left + i_width, i_top + eptEND);

	// Now draw each rating.
	int i_my_width;
	char c_delta = 255 / eptEND;
	for (int i = eptEND - 1; i >= 0; i--)
	{	
		i_my_width = feel[i] * f_width;

		LineColour( 128 + c_delta * 7 * i,
					128 + c_delta * 13 * i,
					128 + c_delta * 3 * i);

		LineDirect(	i_left, i_top + i, 
					i_left + i_my_width, i_top + i);
	}
}



//*********************************************************************************************
//
bool IsMenuItemChecked
(
	uint u4_id	
)
//
//  Returns:
//		true if menu item is checked, else false.
//		Menu item is specified by ID.
//
//**************************************
{
	return GetMenuState(GetMenu(pwnsWindowsShell->hwndMain), 
						u4_id,  MF_BYCOMMAND) &
								MF_CHECKED;
}

//*********************************************************************************************
//
void ToggleMenuCheck
(
	uint u4_id	
)
//
// Toggles the check state of the menu item.  Menu item is specified by its ID.
//
//**************************************
{
	uint u4_flags;

	if (IsMenuItemChecked(u4_id))
		u4_flags = MF_UNCHECKED;
	else
		u4_flags = MF_CHECKED;

	CheckMenuItem(	GetMenu(pwnsWindowsShell->hwndMain),
					u4_id, u4_flags | MF_BYCOMMAND);
}


//*********************************************************************************************
//
void SetMenuCheck
(
	uint u4_id,
	bool b_checked
)
//
// Toggles the check state of the menu item.  Menu item is specified by its ID.
//
//**************************************
{
	uint u4_flags;

	if (b_checked)
		u4_flags = MF_UNCHECKED;
	else
		u4_flags = MF_CHECKED;

	CheckMenuItem(	GetMenu(pwnsWindowsShell->hwndMain),
					u4_id, u4_flags | MF_BYCOMMAND);
}


//*********************************************************************************************
uint32 u4LookupResourceString(int32 i4_id,char* str_buf,uint32 u4_buf_len)
{
	return 0;
}

uint32 g_u4NotifyParam = 0;
PFNWORLDLOADNOTIFY          g_pfnWorldLoadNotify;