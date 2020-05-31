/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Implementation of 'Render.hpp.'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/W95/Render.cpp                                               $
 * 
 * 96    10/07/98 3:11a Pkeet
 * Fixed non-hardware terrain and cache allocations.
 * 
 * 95    10/02/98 2:58a Pkeet
 * Added different schedule times for Direct3D.
 * 
 * 94    10/01/98 1:28a Pkeet
 * Moved terrain and cache texture allocation from the raster constructor to the general
 * purpose create member function.
 * 
 * 93    9/30/98 2:21a Rwyatt
 * Informs text system when screen mode or size changes
 * 
 * 92    9/04/98 8:59p Shernd
 * Setting Viewport changes
 * 
 * 91    8/28/98 11:56a Asouth
 * conversion cast for delete operator
 * 
 * 90    7/30/98 5:55p Pkeet
 * Added the 'hwnd' parameter.
 * 
 * 89    7/13/98 1:29p Pkeet
 * Rebuilding terrain and caches are too slow, code is disabled. Width is rounded always to a
 * multiple of four.
 * 
 * 88    7/13/98 1:22p Pkeet
 * Increased minimum width size.
 * 
 * 87    7/13/98 12:06a Pkeet
 * Added the new 'AdjustViewport' member functions. The screen viewport is now resized without
 * recreating the screen every time.
 * 
 * 86    6/08/98 5:08p Mlange
 * No longer attaches the default palette to prasMainScreen.
 * 
 * 85    98.04.20 8:47p Mmouni
 * Add support for bigger FPS font with a compile switch.
 * 
 * 84    3/18/98 4:06p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 * 83    3/09/98 7:55p Kmckis
 * Changed the color of the on-screen FPS stat to red for easier viewing against our sky
 * backgrounds
 * 
 * 82    98.03.09 5:50p Mmouni
 * Commented out proProfile.psBeginFrame.Add() statements because they are children of
 * psRender.
 * 
 * 81    3/04/98 1:30p Pkeet
 * Added an assert to make sure the hardware 'EndScene' was called before the flip.
 *
 *********************************************************************************************/

//
// Macros, includes and definitions.
//
#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Render.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/Std/Mem.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/Errors.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/W95/dd.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Std/StringEx.hpp"
#include "Lib/View/Viewport.hpp"
#include "Lib/View/Clut.hpp"
#include "Lib/Renderer/Fog.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/Renderer/Sky.hpp"
#include "Lib/Sys/Timer.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"
#include "Lib/Renderer/Sky.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/Renderer/RenderCache.hpp"
#include "Lib/EntityDBase/TextOverlay.hpp"
#include "Lib/Renderer/RenderCache.hpp"
#include "Lib/GeomDBase/TerrainTexture.hpp"
#include "Lib/Sys/FixedHeap.hpp"
#include "Lib/Sys/Scheduler.hpp"

#include <stdio.h>
#include <stdlib.h>



extern bool bIsTrespasser;



// Change to use bigger font for FPS.
#define bBIGGER_FONT 0

//
// Constants.
//

// Minimum width the screen can go to.
static const int iMinWidth = 240;


//
// Forward declarations of module functions.
//

//*********************************************************************************************
//
int iGetScreenBitdepth();
//
// Returns the pixel bitdepth of the screen.
//
//**************************************


//
// Class implementations.
//

//*********************************************************************************************
//
// Class CRenderShell implementation.
//

	//*********************************************************************************************
	CRenderShell::CRenderShell(HWND hwnd, HINSTANCE hins, bool b_show_textdialog)
	{
		Assert(hwnd);
		Assert(hins);
		Assert(prnshMain == 0);

		//
		// Copy windows handles.
		//
		hwndMain			= hwnd;
		hinsAppInstance		= hins;
		iIgnoreWinCommands	= 0;
		bIsFullScreen		= false;
		bActiveApp			= true;
		uCurrentDriver		= 0;

		// Set appropriate palette use.
		SetPaletteUse(true);

		//
		// Load the main system palette.
		//
		//pcdbMain.ppalAddPalette(new CPal(256, true), true);
		//pcdbMain.ppalAddPalette("BinData\\Palettes\\Standard.bmp", true);

		// Make sure the system palette is suited for direct draw output.
		//Assert(pcdbMain.pceMainPalClut);
		//Assert(pcdbMain.pceMainPalClut->ppalPalette);
		//pcdbMain.pceMainPalClut->ppalPalette->MakeSystemFriendly();
	}

	//*********************************************************************************************
	CRenderShell::~CRenderShell()
	{
		// Undo palette use.
		SetPaletteUse(false);

		// Destroy the Renderer.
		delete (CRenderer*)prenMain;
		prenMain = 0;

		// Repaint all windows regardless of which application owns them.
		InvalidateRect(0, 0, true);
	}

	//*********************************************************************************************
	bool CRenderShell::bCreateScreen
	(
		int		i_screen_width, 
		int		i_screen_height, 
		int		i_screen_bits, 
		bool	b_system_mem
	)
	{
		bool b_resize_only = false;
		
		if (iIgnoreWinCommands)
			return false;
		iIgnoreWinCommands++;
	
		iLastScreenBits = i_screen_bits;
		bLastSystemMem  = b_system_mem;

		// Delete the current renderer.  We must do this before calling bChangeRenderer below,
		// because by then, the rasters will be invalid.
		delete (CRenderer*)prenMain;
		prenMain = 0;

		//
		// Determine b_resize_only setting.
		//
		if (prasMainScreen)
		{
			if (i_screen_bits == 0)
				b_resize_only = !bIsFullScreen && (b_system_mem != !!prasMainScreen->bVideoMem);
		}

		bIsFullScreen = i_screen_bits != 0;

		// if we have a sky remove its reference to the main screen
		if (gpskyRender)
		{
			gpskyRender->RemoveRenderSurface();
		}

		// We must destroy the current screen before creating a new one, or DirectDraw complains.
		std::destroy_at(&prasMainScreen);

		// Create a new instance of prasMainScreen.
		prasMainScreen = rptr_new CRasterWin(hwndMain, i_screen_width, i_screen_height, i_screen_bits, 
			                                 3, Set(erasVideoMem) * !b_system_mem);

		// Clear the render cache heap.
		fxhHeap.Reset();

		// Allocate scheduler time.
		if (d3dDriver.bUseD3D())
		{
			shcScheduler.SetTimeSlice(7);
			shcSchedulerTerrainTextures.SetTimeSlice(4);
		}
		else
		{
			shcScheduler.SetTimeSlice(10);
			shcSchedulerTerrainTextures.SetTimeSlice(5);

			// Allocate system memory correctly for caches and terrain.
			CRenderCache::SetTextureMemSize();
			NMultiResolution::CTextureNode::SetTextureMemSize();
		}

		// Set the palette usage to work with 254 colours instead of 236 colours.

		// Attatch the standard palette to prasMainScreen.
//		Assert(pcdbMain.pceMainPalClut);
//		prasMainScreen->AttachPalette(pcdbMain.pceMainPalClut->ppalPalette);

		// Update cluts if required.
		pcdbMain.UpdateCluts(prasMainScreen->pxf);

		// Re-create the renderer for the new screen.
		bChangeRenderer(uCurrentDriver);

		// Get a pointer to the main camera.
		CCamera* pcam = CWDbQueryActiveCamera().tGet();

		// Get the current properties of the camera.
		CCamera::SProperties camprop = pcam->campropGetProperties();

		// Setup the main viewport object.
		camprop.vpViewport.SetSize(prasMainScreen->iWidth, prasMainScreen->iHeight);

		// Set the physical aspect ratio to the product of the raster and pixel aspect ratios.
		camprop.fAspectRatio = prasMainScreen->GetAspectRatio();

		pcam->SetProperties(camprop);

		if (gpskyRender)
		{
			gpskyRender->NewRenderSurface(rptr_cast(CRaster,prasMainScreen));
		}

		CTextOverlay::ptovTextSystem->ResetScreen();
		//
		// Set flags.
		//
		iIgnoreWinCommands--;
		return true;
	}

	//******************************************************************************************
	bool CRenderShell::bChangeRenderer(uint u_driver)
	{
		if (u_driver >= sapRenderDesc.uLen)
			return false;

		iIgnoreWinCommands++;

		// Delete the current renderer, and create a new one.
		delete (CRenderer*)prenMain;

		// To do: Z buffers aren't compatible.  If we want them to work
		// together, the DWI rasteriser needs access to D3D.

		CScreenRender* psr = sapRenderDesc[u_driver]->pScreenRenderCreate
		(
			&rensetSettings,
			rptr_cast(CRaster, prasMainScreen)
		);
		prenMain = new CRenderer(psr, &rensetSettings);

		uCurrentDriver = u_driver;

		proProfile.psFrame.Reset();
		iIgnoreWinCommands--;
		return true;
	}

	//*********************************************************************************************
	void CRenderShell::ProcessMessage(MSG& msg)
	{
		switch (msg.message)
		{

			case WM_ACTIVATE:
				if (!iIgnoreWinCommands)
				{
					if (LOWORD(msg.wParam) && !HIWORD(msg.lParam))
					{
						if (prasMainScreen != 0)
						{
							// Activated and not minimised.
							prasMainScreen->HandleActivate(true);
						}
						CTimer::Pause(false);
					}
					else
						CTimer::Pause(true);
				}
				break;

			case WM_ACTIVATEAPP:
				bActiveApp = msg.wParam != 0;
				break;

			case WM_ENTERMENULOOP:
			case WM_SYSKEYDOWN:
				// Ensure the menu is drawable.
				if (bIsFullScreen)
				{
					prasMainScreen->FlipToGDISurface();
//					DrawMenuBar(hwndMain);
//					RedrawWindow(hwndMain, 0, 0, RDW_FRAME);
				}
				break;
		}
	}

	static PAINTSTRUCT	ps;

	//*********************************************************************************************
	bool CRenderShell::bBeginPaint()
	{
		CCycleTimer ctmr;

		WINDOWPLACEMENT wplcPlacement;

		if (!prasMainScreen || iIgnoreWinCommands || !bActiveApp)
		{
			return false;
		}
		
		//
		// Don't paint the window if it is minimized.
		//
		wplcPlacement.length = sizeof(WINDOWPLACEMENT);
		AlwaysVerify(GetWindowPlacement
		(
			hwndMain,		// Handle of window.
			&wplcPlacement 	// Address of structure for position data.
		));

		bool bPaintModeSet = !(wplcPlacement.showCmd == SW_MINIMIZE ||
							   wplcPlacement.showCmd == SW_SHOWMINIMIZED ||
							   wplcPlacement.showCmd == SW_SHOWMINNOACTIVE);

		if (!bPaintModeSet)
		{
//			proProfile.psBeginFrame.Add(ctmr());
			return false;
		}

		iIgnoreWinCommands++;

        ::BeginPaint(hwndMain, &ps);

		specProfile.StartAll();

//		proProfile.psBeginFrame.Add(ctmr());

		return true;
	}

	//*********************************************************************************************
	void CRenderShell::EndPaint()
	{
		CCycleTimer ctmr;

		prenMain->pScreenRender->EndFrame();

        ::EndPaint(hwndMain, &ps);

		specProfile.StopAll();
		specProfile.WriteToMainWindow();

		CCycleTimer ctmr2;
		srd3dRenderer.AssertEndScene();
		prasMainScreen->Flip();
		proProfile.psFlip.Add(ctmr2());

		iIgnoreWinCommands--;

		proProfile.psEndFrame.Add(ctmr());
	}

	//*********************************************************************************************
	void CRenderShell::ShowConsoleOnScreen(CConsoleBuffer& con)
	{
		// Show it right on the screen.
		prasMainScreen->Unlock();
		HDC hdc = prasMainScreen->hdcGet();
		con.SetTransparency(true);
		con.Show(hdc, prasMainScreen->iHeight, 1);
		con.SetTransparency(false);
		prasMainScreen->ReleaseDC(hdc);
	}

	//*********************************************************************************************
	void CRenderShell::PrintString(char *cstr)
	{
		// Show it right on the screen.
		prasMainScreen->Unlock();
		HDC hdc = prasMainScreen->hdcGet();

#if (bBIGGER_FONT)
		static HFONT hfont;

		// create the Win32 Font
		if (!hfont)
		{
			hfont = CreateFont(32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
								ANSI_CHARSET,
								OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY,   // NONANTIALIASED_QUALITY - Undocumented
								VARIABLE_PITCH,
								"Arial" );
		}

		// Select our font into the DC.
		void *hfont_old = SelectObject(hdc, hfont);
#endif

		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(200,20,20));
		TextOut(hdc, 0, 0, cstr, strlen(cstr));

#if (bBIGGER_FONT)
		// Select the old font back into the device context.
		SelectObject(hdc, hfont_old);
#endif

		prasMainScreen->ReleaseDC(hdc);
	}

	//*********************************************************************************************
	void CRenderShell::SetPaletteUse(bool b_use_full_palette)
	{
		Assert(hwndMain);

		// Get the bitdepth for the screen.
		int i_bitdepth = iGetScreenBitdepth();

		// Get the device context for the window.
		HDC hdc_win_dc = GetDC(0);

		// Test the screen capabilities to see if it is 8 bits per pixel.
		if (i_bitdepth == 8)
		{
			SetSystemPaletteUse
			(
				hdc_win_dc,
				(b_use_full_palette) ? (SYSPAL_NOSTATIC) : (SYSPAL_STATIC)
			);
		}
		
		// Release the device context for the window.
		ReleaseDC(0, hdc_win_dc);
	}

	//*****************************************************************************************
	void CRenderShell::AdjustViewport(int i_view_width, 
                                      int i_view_height, 
                                      HWND hwnd, 
                                      bool bRedraw /* = true */)
	{
		// Do nothing if no screen is present.
		if (!prasMainScreen)
			return;


		//
		// Remove overlay text
		//
		CTextOverlay::ptovTextSystem->RemoveAll();

		// Constants for minimum dimensions that preserve the screen's ratio.
		int iMinHeight = iMinWidth * prasMainScreen->iHeightFront / prasMainScreen->iWidthFront;

		// Always make the new width a multiple of four.
		i_view_width += 2;
		i_view_width &= 0xFFFFFFFC;

        if (!bIsTrespasser)
        {
		    // Clamp the minimum and maximum values.
		    SetMinMax(i_view_width,  iMinWidth,  prasMainScreen->iWidthFront);
		    SetMinMax(i_view_height, iMinHeight, prasMainScreen->iHeightFront);
        }

		// Get the main camera's properties.
		CCamera* pcam = CWDbQueryActiveCamera().tGet();
		Assert(pcam);
		CCamera::SProperties camprop = pcam->campropGetProperties();

		// Adjust the viewport for the main raster and the camera.
		prasMainScreen->SetViewportDim(i_view_width, i_view_height, hwnd);
		camprop.vpViewport.SetSize(prasMainScreen->iWidth, prasMainScreen->iHeight);

		// Set the new renderer aspect ratio.
		camprop.fAspectRatio = prasMainScreen->GetAspectRatio();

		// Set the main camera's properties.
		pcam->SetProperties(camprop);

		// Set new sky values.
		if (gpskyRender)
			gpskyRender->ResetScreenSize();

		// Redraw terrain and caches.
		/*
		if (CWDbQueryTerrain().tGet() != 0)
			CWDbQueryTerrain().tGet()->Rebuild(true);
		PurgeRenderCaches();
		*/

        if (bRedraw)
        {
		    // Clear and repaint the screen without flickering.
		    prasMainScreen->Clear(0);
		    if (bBeginPaint())
		    {
			    gmlGameLoop.Paint();
			    EndPaint();
			    if (bBeginPaint())
			    {
				    prasMainScreen->Clear(0);
				    gmlGameLoop.Paint();
				    EndPaint();
			    }
		    }
		    else
		    {
			    prasMainScreen->Flip();
			    prasMainScreen->Clear(0);
		    }
        }

		CTextOverlay::ptovTextSystem->ResetScreen();
	}

	//*****************************************************************************************
	void CRenderShell::AdjustViewportRelative(float f_resize_amount, HWND hwnd)
	{
		// Do nothing if no screen is present.
		if (!prasMainScreen)
			return;

		// Adjust width and then calculate the relative value for height.
		int i_new_width  = int(float(prasMainScreen->iWidth) * f_resize_amount + 0.5f);
		SetMinMax(i_new_width,  iMinWidth,  prasMainScreen->iWidthFront);
		int i_new_height = i_new_width * prasMainScreen->iHeight / prasMainScreen->iWidth;

		// Resize the viewport.
		AdjustViewport(i_new_width, i_new_height, hwnd);
	}

	//*****************************************************************************************
	void CRenderShell::AdjustViewportRelativeHorizontal(float f_resize_amount, HWND hwnd)
	{
		// Do nothing if no screen is present.
		if (!prasMainScreen)
			return;

		// Adjust width and then calculate the relative value for height.
		int i_new_width  = int(float(prasMainScreen->iWidth) * f_resize_amount + 0.5f);

		// Resize the viewport.
		AdjustViewport(i_new_width, prasMainScreen->iHeight, hwnd);
	}

	//*****************************************************************************************
	void CRenderShell::AdjustViewportRelativeVertical(float f_resize_amount, HWND hwnd)
	{
		// Do nothing if no screen is present.
		if (!prasMainScreen)
			return;

		// Adjust height.
		int i_new_height = int(float(prasMainScreen->iHeight) * f_resize_amount + 0.5f);

		// Resize the viewport.
		AdjustViewport(prasMainScreen->iWidth, i_new_height, hwnd);
	}


//
// Global functions.
//

//*********************************************************************************************
int iGetScreenBitdepth()
{
	// Get the device context for the screen.
	HDC hdc_win_dc = GetDC(0);

	// Get the bitdepth.
	int i_bitdepth = GetDeviceCaps(hdc_win_dc, BITSPIXEL);

	// Release the device context for the screen.
	ReleaseDC(0, hdc_win_dc);

	return i_bitdepth;
}

//*********************************************************************************************
void TerminateShell(bool b_terminal_error)
{
	// Destroy "pwnsWindowsShell" if it is still valid.
	//if (pwnsWindowsShell)
	//{
		// Send a "WM_DESTROY" message to the main window.
		//Verify(DestroyWindow(pwnsWindowsShell->hwndMain));

		// Delete the windows shell object.
		//delete pwnsWindowsShell;
		//pwnsWindowsShell = 0;
	//}

	// Repaint all windows regardless of which application owns them.
	InvalidateRect(0, 0, true);

	// Exit application for sure.
	exit(0);
}

//
// Global Variables.
//
CRenderShell*		prnshMain = 0;
rptr<CRasterWin>	prasMainScreen;
