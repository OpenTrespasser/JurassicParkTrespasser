/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of TestMath.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/TestMath.cpp                                                     $
 * 
 * 12    96/10/28 15:00 Speter
 * Changed CEventHandler to CAppShell.
 * 
 * 11    7/25/96 9:35a Mlange
 * Updated random test code.
 * 
 * 10    7/24/96 2:22p Mlange
 * Added test code for the random number generator.
 * 
 * 9     7/19/96 10:58a Pkeet
 * Changed include files for shell files moved to 'Shell.'
 * 
 * 8     96/07/03 13:02 Speter
 * Moved several files to new directories, and changed corresponding include statements.
 * 
 * 7     96/06/20 6:27p Mlange
 * Updated for changes to the CIntFloat class. Reduced the mantissa step value so more tests are
 * done.
 * 
 * 6     3-06-96 3:42p Mlange
 * Reorganised code such that single values of the square root functions are calculated on each
 * iteration of the Step() function. This ensures that windows' messages are processed on a
 * timely basis.
 * 
 * 5     29-05-96 2:39p Mlange
 * Now no longer calls TerminateShell().
 * 
 * 4     16-05-96 5:17p Mlange
 * Updated for changes to app main interface.
 * 
 * 3     16-05-96 4:17p Mlange
 * Now calls TerminateShell.
 * 
 * 2     10-05-96 3:18p Mlange
 * Test code operational.
 * 
 * 1     5/09/96 5:18p Mlange
 * Empty initial version.
 * 
 **********************************************************************************************/

#include <float.h>
#include <math.h>
#include "common.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/Math/FastSqrt.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Shell/AppShell.hpp"
#include "Shell/AppShell.hpp"
#include "TestMath.hpp"


#define iREFRESH_TIME	100
#define fRANGE_MIN		.005f
#define fRANGE_MAX		16.0f
#define i4MANT_STEP		0x20


CRandom rndTest;

class CAppShellTestMath: public CAppShell
{
public:
	CIntFloat ifVal;
	CIntFloat ifMaxDiffValSqrt, ifMaxDiffValInvSqrt;
	float fMaxDiffSqrt, fMaxDiffInvSqrt;
	double dPctDiffSqrt, dPctDiffInvSqrt;

	int iRefreshCount;

	void Init()
	{
		iRefreshCount = 0;

		ifMaxDiffValSqrt    = 0.0f;
		ifMaxDiffValInvSqrt = 0.0f;

		dPctDiffSqrt    = 0.0;
		dPctDiffInvSqrt = 0.0;

		ifVal = fRANGE_MIN;

		SetContinuous(true);
	}

	void Step()
	{
		float f_approx_sqrt, f_accurate_sqrt, f_diff_sqrt;
		float f_approx_inv_sqrt, f_accurate_inv_sqrt, f_diff_inv_sqrt;
	
		if (ifVal.fFloat <= fRANGE_MAX)
		{
			f_approx_sqrt   = fSqrt(ifVal);
			f_accurate_sqrt = (float)sqrt(ifVal);

			f_diff_sqrt = (float)fabs(f_approx_sqrt - f_accurate_sqrt);

			if (f_diff_sqrt > fMaxDiffSqrt)
			{
				fMaxDiffSqrt     = f_diff_sqrt;
				ifMaxDiffValSqrt = ifVal;

				dPctDiffSqrt = (double)f_diff_sqrt / f_accurate_sqrt * 100.0;
			}


			f_approx_inv_sqrt   = fInvSqrt(ifVal);
			f_accurate_inv_sqrt = 1.0f / (float)sqrt(ifVal);

			f_diff_inv_sqrt = (float)fabs(f_approx_inv_sqrt - f_accurate_inv_sqrt);

			if (f_diff_inv_sqrt > fMaxDiffInvSqrt)
			{
				fMaxDiffInvSqrt     = f_diff_inv_sqrt;
				ifMaxDiffValInvSqrt = ifVal;

				dPctDiffInvSqrt = (double)f_diff_inv_sqrt / f_accurate_inv_sqrt * 100.0;
			}


			if (iRefreshCount == 0)
			{
				iRefreshCount = iREFRESH_TIME;

				conStd.ClearScreen();

				conStd.Print("Testing fast square root functions from: %.3f to: %.3f\n", (double)fRANGE_MIN, (double)fRANGE_MAX);
				conStd.Print("Current num : %f\n", (double)ifVal );
				conStd.Print("\nSquare root:\n" );
				conStd.Print("Max diff pct: %f\n", (double)dPctDiffSqrt );
				conStd.Print("At num      : %f\n", (double)ifMaxDiffValSqrt );
				conStd.Print("\nInverse square root:\n" );
				conStd.Print("Max diff pct: %f\n", (double)dPctDiffInvSqrt );
				conStd.Print("At num      : %f\n", (double)ifMaxDiffValInvSqrt );

				conStd.Show();
			}

			iRefreshCount -= 1;

			ifVal.i4Int += i4MANT_STEP;
		}

		Repaint();
	}


	//******************************************************************************************
	void Paint()
	{
		//
		// Test random number generator.
		//
		for (int i = 0; i < 64; i++)
		{
			int i_x = rndTest((uint32)0, (uint32)prasMainScreen->iWidth);
			int i_y = rndTest((uint32)0, (uint32)prasMainScreen->iHeight);

			CColour clr
			(
				(uint8)rndTest(),
				(uint8)rndTest(),
				(uint8)rndTest()
			);

			TPixel pix = prasMainScreen->pixFromColour(clr);

			prasMainScreen->PutPixel(i_x, i_y, pix);
		}

	}
};



CAppShell* pappMain = new CAppShellTestMath;


//Global variables and functions declared elsewhere as extern
//needed by the libraries
bool bIsTrespasser = false;
bool bUseReplayFile = false;
bool bInvertMouse = false;
bool bUseOutputFiles = false;
unsigned int g_u4NotifyParam = 0;
unsigned int u4LookupResourceString(int, char*, unsigned int) { return 0; }
void LineColour(int,int,int) {}
PFNWORLDLOADNOTIFY g_pfnWorldLoadNotify = nullptr;
