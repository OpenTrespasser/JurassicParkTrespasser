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
 * $Log:: /JP2_PC/Source/Test/FastBumpTestShell.cpp                                           $
 * 
 * 7     96/10/28 14:53 Speter
 * Changed AppShell:: to CAppShell::.
 * 
 * 6     96/09/27 11:31 Speter
 * Oops.  Changed <float> in geometry types to <>.
 * 
 * 5     9/24/96 3:00p Pkeet
 * Not working, but didn't want them hanging around.
 * 
 * 4     9/12/96 3:16p Pkeet
 * Reduced size of bumpmap to intensity table by half because bumpmap phi values can be
 * positive only.
 * 
 * 3     9/05/96 6:34p Pkeet
 * Changed bumpmap to use the 'CRasterSimple' class. Added lighting parameters to the
 * 'MakeBumpmap' Table function.
 * 
 * 2     9/04/96 5:17p Pkeet
 * Added file-based bumpmaps.
 * 
 **********************************************************************************************/

#include "Lib/W95/WinInclude.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "gblinc/common.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Shell/winshell.hpp"
#include "Shell/AppShell.hpp"
#include "Shell/WinEvent.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/View/ColourBase.hpp"
#include "Lib/Renderer/Primitives/FastBump.hpp"
#include "Lib/Renderer/Primitives/FastBumpTable.hpp"
#include "Lib/Renderer/Primitives/FastBumpMath.hpp"
#include "FastBumpTest.hpp"
#include "FastBumpTestShell.hpp"


float LightPhi   = 0.0f;
float LightTheta = 0.0f;
const float fMoveLightBy = 0.05f;


void SetLight()
{
	CDir3<> dir = dir3AnglesToNormal(LightPhi, LightTheta);
	Light = dir;
}

void MoveLightPhi(float f_rad)
{
	LightPhi += f_rad;
	SetLight();
}

void MoveLightTheta(float f_rad)
{
	LightTheta += f_rad;
	SetLight();
}

//*********************************************************************************************
void WindowsEvent(uint u_message, WPARAM wp_param, LPARAM lp_param)
{
	switch (u_message)
	{
		case WM_DESTROY:
			MessageBeep(0);
			break;

		case AM_INIT:
			CAppShell::SetContinuous(true);
			break;

		case AM_NEWRASTER:


			conStd.OpenFileSession("bumpangle.txt");
			MakeBumpPrimaryTable();
			TestMath();
			conStd.CloseFileSession();

			SetLight();
			MakeBumpmap();

			prasMainScreen->Clear(0);
			GetAsyncKeyState(VK_SHIFT);
			preMain->SetConsoleState(CRender::econsWINDOW);
			conStd.ClearScreen();
			Light.PrintToCon();
			conStd.Show();
			break;

		case WM_KEYDOWN:
			switch (wp_param)
			{

				case VK_F4:
					break;

				case VK_F1:
					prasMainScreen->Clear(0);
					break;
				case VK_F2:
					prasMainScreen->Clear(0);
					break;
				case VK_F3:
					prasMainScreen->Clear(0);
					break;

				case VK_UP:
					MoveLightPhi(fMoveLightBy);
					break;
				case VK_DOWN:
					MoveLightPhi(-fMoveLightBy);
					break;
				case VK_LEFT:
					MoveLightTheta(fMoveLightBy * 2);
					break;
				case VK_RIGHT:
					MoveLightTheta(-fMoveLightBy * 2);
					break;

				case VK_F11:
					break;
				case VK_F12:
					break;
			}
			/*
			conStd.ClearScreen();
			Light.PrintToCon();
			conStd.Show();
			CAppShell::Repaint();
			break;
			*/

		case AM_STEP:
			conStd.ClearScreen();
			Light.PrintToCon();
			conStd.Print("Phi, Theta: %1.3f, %1.3f\n", fRadToDeg(LightPhi), fRadToDeg(LightTheta));
			{
				CDir3<> dir = dir3AnglesToNormal(LightPhi, LightTheta);
				conStd.Print("X, Y, Z:  %1.3f, %1.3f, %1.3f\n", dir.tX, dir.tY, dir.tZ);
			}
			conStd.Print("Bumpmap Table Size: %ld\n", iBUMP_TABLE_SIZE);
			conStd.Show();
			CAppShell::Repaint();
			break;

		case AM_PAINT:
			//prasMainScreen->Clear(0);
			{
				/*
				int no_w = min(prasMainScreen->iWidth / iBumpmapLen, 3);
				int no_h = min(prasMainScreen->iHeight / iBumpmapLen, 2);
				for (int i = 0; i < no_w; i++)
					for (int j = 0; j < no_h; j++)
				*/
						DrawSpeedBump(prasMainScreen.ptGet(), 0, 0);
			}
			break;
	}
}
