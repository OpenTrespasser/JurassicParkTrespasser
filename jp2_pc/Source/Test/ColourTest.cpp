/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of TestMath.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/ColourTest.cpp                                                  $
 * 
 * 18    96/10/28 14:52 Speter
 * Changed AppShell:: to CAppShell::.
 * 
 * 17    7/19/96 10:58a Pkeet
 * Changed include files for shell files moved to 'Shell.'
 * 
 * 16    7/18/96 3:30p Pkeet
 * Converted to 'CRender' style shell.
 * 
 * 15    7/14/96 10:41a Pkeet
 * 
 * 14    7/13/96 11:35p Pkeet
 * 
 * 13    7/13/96 11:31p Pkeet
 * 
 * 12    7/13/96 11:07p Pkeet
 * Finished test.
 * 
 * 11    7/13/96 11:00p Pkeet
 * 
 * 10    7/13/96 10:59p Pkeet
 * Finished test.
 * 
 * 9     7/13/96 5:51p Pkeet
 * Speedbump test.
 * 
 * 8     7/12/96 5:08p Pkeet
 * Finished test.
 * 
 * 7     96/07/03 13:02 Speter
 * Moved several files to new directories, and changed corresponding include statements.
 * 
 * 6     6/24/96 6:25p Pkeet
 * Finished series of tests.
 * 
 * 5     6/21/96 2:59p Pkeet
 * Finished with clut tests.
 * 
 * 4     6/19/96 8:45p Pkeet
 * Fixed log problem. Added array prefix for pre-built array.
 * 
 * 3     6/19/96 8:37p Pkeet
 * Initial implementation.
 * 
 * 2     6/18/96 4:26p Pkeet
 * 
 **********************************************************************************************/

#include "Lib/W95/WinInclude.hpp"
#include <StdIO.h>
#include <StdLib.h>
#include "GblInc/Common.hpp"
#include "Lib/Sys/TextOut.hpp"
#include "Shell/WinShell.hpp"
#include "Shell/AppShell.hpp"
#include "Shell/WinEvent.hpp"
#include "Lib/View/ColourBase.hpp"
#include "Lib/Loader/Loader.hpp"
#include "ColourDrawTest.hpp"
#include "ColourTest.hpp"

#include <algorithm>


void MoveLight(int& iLight, int iAmount)
{
	if (GetAsyncKeyState(VK_SHIFT) != 0)
		iAmount /= 4;
	iLight += 256;
	iLight += iAmount;
	iLight &= 0xFF;
}

float fAngle(int i)
{
	float f = (float)i;
	f *= 360.0f / 256.0f;
	return f;
}


//*********************************************************************************************
void WindowsEvent(uint u_message, WPARAM wp_param, LPARAM lp_param)
{
	switch (u_message)
	{
		case WM_DESTROY:
			break;

		case AM_INIT:
			//AppShell::SetContinuous(false);
			CAppShell::SetContinuous(true);
			break;

		case AM_NEWRASTER:
			prasMainScreen->Clear(0);
			MakeBumpmap(cone);
			GetAsyncKeyState(VK_SHIFT);
			break;

		case WM_KEYDOWN:
			switch (wp_param)
			{

				case VK_F1:
					prasMainScreen->Clear(0);
					MakeBumpmap(cone);
					break;
				case VK_F2:
					prasMainScreen->Clear(0);
					MakeBumpmap(hemi);
					break;
				case VK_F3:
					prasMainScreen->Clear(0);
					MakeBumpmap(ripple);
					break;

				case VK_UP:
					MoveLight(iIncidence, 8);
					break;
				case VK_DOWN:
					MoveLight(iIncidence, -8);
					break;
				case VK_LEFT:
					MoveLight(iBearing, 8);
					break;
				case VK_RIGHT:
					MoveLight(iBearing, -8);
					break;

				case VK_F11:
					MoveLight(iConeHeight, 4);
					MakeBumpmap(cone);
					break;
				case VK_F12:
					MoveLight(iConeHeight, -4);
					MakeBumpmap(cone);
					break;
			}
			CAppShell::Repaint();
			break;

		case AM_STEP:
			CAppShell::Repaint();
			break;

		case AM_PAINT:
			//prasMainScreen->Clear(0);
			{
				int no_w = std::min(prasMainScreen->iWidth / iBumpmapLen, 3);
				int no_h = std::min(prasMainScreen->iHeight / iBumpmapLen, 2);
				for (int i = 0; i < no_w; i++)
					for (int j = 0; j < no_h; j++)
						DrawSpeedBump(prasMainScreen.ptGet(), i, j);
			}
			/*
			DrawWater(prasMainScreen);
			*/
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
void LineColour(int, int, int) {}
PFNWORLDLOADNOTIFY g_pfnWorldLoadNotify = nullptr;
