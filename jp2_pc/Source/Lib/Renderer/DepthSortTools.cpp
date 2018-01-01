/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of DepthSortTools.hpp.
 *
 * Bugs:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/DepthSortTools.cpp                                       $
 * 
 * 34    98.09.24 1:40a Mmouni
 * Shrunk CRenderPolygon by a large amount.
 * Multiple list depth sort stuff is now on a compile switch.
 * 
 * 33    8/03/98 6:13p Pkeet
 * Split polygons will still have their full screen extents calculated.
 * 
 * 32    8/03/98 5:43p Pkeet
 * Split code copies more flags.
 * 
 * 31    7/29/98 11:48a Pkeet
 * The addressing mode for polygons is copied when a new polygon is generated.
 * 
 * 30    6/09/98 3:54p Pkeet
 * Hardware flags are copied to split polygons. The adjust value is given by the terrain
 * hardware flag.
 * 
 * 29    2/13/98 11:18a Pkeet
 * Turned off stats collection in final mode.
 * 
 * 28    1/24/98 3:19p Pkeet
 * Screen areas are now calculated for polygons clipped by the depth sort.
 * 
 * 27    12/23/97 2:17p Pkeet
 * Added the 'fAdjustOut' parameter.
 * 
 * 26    11/10/97 10:05p Gfavor
 * Nulled out body of bVerifyOrder.
 * 
 * 25    97/10/08 11:00a Pkeet
 * Set flag to do normal depth sort reporting.
 * 
 * 24    9/15/97 7:25p Agrant
 * Clear depth sort stats even when in joystick test mode.
 * Fixes split poly stat bug in Art Console.
 * 
 * 23    8/20/97 12:49a Pkeet
 * Added a joystick test function.
 * 
 * 22    97/08/11 12:30p Pkeet
 * Copies mip level to split polygons.
 * 
 * 21    7/27/97 2:16p Agrant
 * Always reset depth sort stats to zero, even if not showing them
 * 
 * 20    97/07/23 18:05 Speter
 * Replaced CCamera argument to clipping functions with b_perspective flag.  
 * 
 * 19    97/06/25 1:15p Pkeet
 * Includes code for displaying depth sort clipping.
 * 
 * 18    6/19/97 2:53p Mlange
 * Moved Profile.hpp to Lib/Sys.
 * 
 * 17    97/06/10 15:48 Speter
 * Now takes camera as an argument (for new vertex projection).  Renamed call to
 * bCalculateScreenExtents.
 * 
 * 16    97/05/24 3:57p Pkeet
 * Added the 'InitializePolygonForDepthSort' member function.
 * 
 * 15    97/05/22 4:00p Pkeet
 * Added the 'iNumCulled' variable.
 * 
 * 14    97/05/21 10:39a Pkeet
 * Uses the minimum tolerance of two polygons.
 * 
 * 13    97/05/20 11:40a Pkeet
 * Added variable plane tolerances for depth sorting.
 * 
 * 12    97/05/16 2:26p Pkeet
 * Added code to look for duplicate polygons in the output list.
 * 
 * 11    97/05/15 2:53p Pkeet
 * Added stat for depthsort clipping.
 * 
 * 10    97/05/08 1:01p Pkeet
 * Added extra stats. Removed the key less object.
 * 
 * 9     97/05/06 6:52p Pkeet
 * Added map classes. Added an extra stat.
 * 
 * 8     97/04/29 4:54p Pkeet
 * Moved the 'bSplit' function to the body of the code.
 * 
 * 7     97/04/28 5:44p Pkeet
 * Moved statistics counting stuff here from the depth sort module.
 * 
 * 6     97/04/23 12:44p Pkeet
 * Added the 'WriteFloat' function.
 * 
 * 5     97-04-21 17:10 Speter
 * Removed old include.
 * 
 * 4     97/04/21 10:54a Pkeet
 * Added output for number of times clipped.
 * 
 * 3     97/04/18 3:02p Pkeet
 * Made the write vertex print out more significant digits for screen coordinates.
 * 
 * 2     97/04/17 4:12p Pkeet
 * Added code to write the values of a polygon and its vertices.
 * 
 * 1     97/04/17 11:34a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

// Define to run the joystick example.
#define bJOYSTICK_TEST (0)

#if bJOYSTICK_TEST
	#include <Lib/W95/WinInclude.hpp>
	#include <windows.h>
	#include <mmsystem.h>
#endif // bJOYSTICK_TEST

#include "Common.hpp"
#include "DepthSortTools.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/TextOut.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Renderer/Clip.hpp"
#include "PipeLineHeap.hpp"
#include "Line.hpp"
#include "DepthSort.hpp"

#if bJOYSTICK_TEST
	#include "Lib/Control/Control.hpp"
#endif // bJOYSTICK_TEST


//
// Module specific variables.
//

// Text output buffer.
CConsoleBuffer conDepthDebug;

bool bOpened     = false;
bool bCameraMove = true;

// Outward adjustment for splitting terrain polygons and using hardware rasterizing.
float fAdjustOut;


//
// Function implementations.
//

//**********************************************************************************************
//
bool bSplit
(
	CRenderPolygon*  prpoly_plane,	// Pointer to the polygon containing the plane to split by.
	CRenderPolygon*  prpoly_split,	// Pointer to the polygon to split.
	CRenderPolygon*& rprpoly_new_a,	// Pointer to the new polygon 'a.'
	CRenderPolygon*& rprpoly_new_b,	// Pointer to the new polygon 'b.'
	CPipelineHeap&   rplh,			// Current pipeline heap object.
	bool			 b_perspective	// Whether projecting camera is perspective.
)
//
// Splits a polygon by a plane. After a valid split, 'prpoly_split' is no longer valid.
//
// Returns 'true' if the polygon could be split by the plane.
//
//**********************************
{
	// Store the farthest (maximum) Z value.
	TReal r_min_level = Max(prpoly_plane->rMaxLevel, prpoly_split->rMaxLevel);
	TReal r_min_tol   = Min(prpoly_split->rPlaneTolerance, prpoly_plane->rPlaneTolerance);
	int   i_mip_level = prpoly_split->iMipLevel;
	EHWRenderType ehw = prpoly_split->ehwHardwareFlags;
	EAddressMode eam  = prpoly_split->eamAddressMode;
	bool b_preraster  = prpoly_split->bPrerasterized;

	//CCycleTimer ctmr;	// Timer object.

	// Create a plane from Q and split P.
		

	CClipPlaneTolerance clpt(prpoly_plane->plPlane, r_min_tol);
	rprpoly_new_a = prpoly_split;
	bool b_split = clpt.esfSplitPolygon
	(
		*prpoly_split, 
		rplh, 
		b_perspective,
		&rprpoly_new_b,
		(ehw == ehwTerrain) ? (fAdjustOut) : (0.0f)
	) == esfINTERSECT;

	//proProfile.psClipping.Add(ctmr(), 1);
	if (!b_split)
	{
		//proProfile.psClipping.Add(ctmr(), 1);
		return false;
	}

	Assert(rprpoly_new_a);
	Assert(rprpoly_new_b);

	// Initialize polygons for sorting.
	rprpoly_new_a->bCalculateScreenExtents(false);
	rprpoly_new_b->bCalculateScreenExtents(false);
	rprpoly_new_a->rMaxLevel = rprpoly_new_b->rMaxLevel = r_min_level;
	rprpoly_new_a->SetArea();
	rprpoly_new_b->SetArea();

	// Copy hardware flags.
	rprpoly_new_a->ehwHardwareFlags = ehw;
	rprpoly_new_b->ehwHardwareFlags = ehw;
	rprpoly_new_a->eamAddressMode   = eam;
	rprpoly_new_b->eamAddressMode   = eam;
	rprpoly_new_a->bPrerasterized   = b_preraster;
	rprpoly_new_b->bPrerasterized   = b_preraster;
	rprpoly_new_a->bAccept          = true;
	rprpoly_new_b->bAccept          = true;

	// Prevent plane degeneration.
	rprpoly_new_a->plPlane = rprpoly_new_b->plPlane = prpoly_split->plPlane;
	rprpoly_new_a->rPlaneTolerance = rprpoly_new_b->rPlaneTolerance =
		                             prpoly_split->rPlaneTolerance;

	// Get correct sort for new polygons.
	if (u4FromFloat(rprpoly_new_a->fMaxZ) < u4FromFloat(rprpoly_new_b->fMaxZ))
		Swap(rprpoly_new_a, rprpoly_new_b);

	// Test for incorrect clipping.
	//Assert(!rprpoly_new_a->bIntersect(prpoly_plane->plPlane));
	//Assert(!rprpoly_new_b->bIntersect(prpoly_plane->plPlane));

#if (BUILDVER_MODE != MODE_FINAL)
	iNumClipped++;
	iNumInitialize += 2;
#endif // (BUILDVER_MODE != MODE_FINAL)

#if (VER_DEBUG)
	// Indicate that the polygon was clipped by the depth sort routine.
	rprpoly_new_a->bClippedByDepthSort = true;
	rprpoly_new_b->bClippedByDepthSort = true;

	// Increment statistics counter.
	rprpoly_new_a->iClipped++;
	rprpoly_new_b->iClipped++;
#endif

	// Copy mip level over.
	rprpoly_new_a->iMipLevel = i_mip_level;
	rprpoly_new_b->iMipLevel = i_mip_level;
	
	//proProfile.psClipping.Add(ctmr(), 1);

	// Indicate success.
	return true;
}

//******************************************************************************************
void Write(const SRenderVertex* prv)
{
	Assert(prv);

	conDepthDebug.Print("    Screen: %1.20f, %1.20f, %1.5f\n", prv->v3Screen.tX, prv->v3Screen.tY, prv->v3Screen.tZ);
	conDepthDebug.Print("    Camera: %1.5f, %1.5f, %1.5f\n", prv->v3Cam.tX, prv->v3Cam.tY, prv->v3Cam.tZ);
	conDepthDebug.Print("\n");
}

//**********************************************************************************************
void WritePolygon(const CRenderPolygon* prpoly)
{
	Assert(prpoly);

	conDepthDebug.Print("\nPolygon Information: %ld\n", prpoly);
	conDepthDebug.Print("    Plane: %1.4f, %1.4f, %1.4f, %1.4f\n", prpoly->plPlane.d3Normal.tX,
					prpoly->plPlane.d3Normal.tY, prpoly->plPlane.d3Normal.tZ, prpoly->plPlane.rD);

#if (VER_DEBUG)
	conDepthDebug << "    Number of times clipped: " << prpoly->iClipped << "\n";
#endif

	// Write out the vertex information
	conDepthDebug << "    Number of vertices: " << prpoly->paprvPolyVertices.uLen << "\n\n";
	for (int i_vert = 0; i_vert < prpoly->paprvPolyVertices.uLen; i_vert++)
		Write(prpoly->paprvPolyVertices[i_vert]);
	conDepthDebug.Print("\n");
}

//**********************************************************************************************
void WriteText(const char* str_out)
{
	conDepthDebug.Print(str_out);
}

//**********************************************************************************************
void WriteFloat(const char* str_out, float f)
{
	conDepthDebug.Print(str_out);
	conDepthDebug.Print("%1.5f\n", f);
}

//**********************************************************************************************
void OpenDepthSortDebug()
{
	conDepthDebug.OpenFileSession("DepthSortDebug.txt");
}

//**********************************************************************************************
void CloseDepthSortDebug()
{
	conDepthDebug.CloseFileSession();
}

//**********************************************************************************************
void DepthSortStatsWriteAndClear()
{
#if bJOYSTICK_TEST
	// Do nothing if the console is not being displayed.
	if (conDepthSort.bIsActive())
	{
		float f_x;
		float f_y;
		float f_z;
		bool  b_trigger;
		bool  b_thumb;
		bool  b_joystick_present = bReadJoystickSimple(f_x, f_y, f_z, b_trigger, b_thumb);

		if (b_joystick_present)
		{
			conDepthSort.Print("\n\nx: %1.2f\ny: %1.2f\nz: %1.2f\n", f_x, f_y, f_z);
			if (b_trigger)
				conDepthSort.Print("Trigger button pressed.\n");
			if (b_thumb)
				conDepthSort.Print("Thumb button pressed.\n");
		}
		else
		{
			conDepthSort.Print("Joystick not working.\n");
		}

		// Display console.
		conDepthSort.Show();
		conDepthSort.ClearScreen();
	}
#else
	// Do nothing if the console is not being displayed.
	if (conDepthSort.bIsActive())
	{
		conDepthSort.Print("Stats:\n\n");
		conDepthSort.Print("Number of polygons at start:     %ld\n", iNumPolygonsStart);	
		conDepthSort.Print("Number of tests:                 %ld\n", iNumTests);
		conDepthSort.Print("Number left after steps 1 and 2: %ld\n", iNumStep1and2);
		conDepthSort.Print("Number left after step 3:        %ld\n", iNumStep3);
		conDepthSort.Print("Number left after step 4:        %ld\n", iNumStep4);
		conDepthSort.Print("Number left after step 5:        %ld\n", iNumStep5);
		conDepthSort.Print("Number polygons reinserted:      %ld\n", iNumReinserted);
		conDepthSort.Print("Number polygons clipped:         %ld\n", iNumClipped);
		conDepthSort.Print("Number polygons initialized:     %ld\n", iNumInitialize);
		conDepthSort.Print("Number of polygons at end:       %ld\n", iNumPolygonsEnd);
		conDepthSort.Print("Number of polygons culled:       %ld\n", iNumCulled);
		conDepthSort.Print("Number of polygons that a:       %ld\n", iNumA);
		conDepthSort.Print("Number of polygons that b:       %ld\n", iNumB);
		conDepthSort.Print("Number of polygons that c:       %ld\n", iNumC);
		conDepthSort.Print("Number of polygons that d:       %ld\n", iNumD);

		// Display console.
		conDepthSort.Show();
		conDepthSort.ClearScreen();
	}
#endif // bJOYSTICK_TEST

	// Clear.
	iNumPolygonsStart = 0;
	iNumPolygonsEnd   = 0;
	iNumTests         = 0;
	iNumStep1and2     = 0;
	iNumStep3         = 0;
	iNumStep4         = 0;
	iNumStep5         = 0;
	iNumReinserted    = 0;
	iNumClipped       = 0;
	iNumInitialize    = 0;
	iNumCulled        = 0;
	iNumA             = 0;
	iNumB             = 0;
	iNumC             = 0;
	iNumD             = 0;
}

//**********************************************************************************************
bool bVerifyOrder(CPArray<CRenderPolygon*> paprpoly)
{
	Assert(0);				// calls of bBehind and bIntersect2D require surrounding FEMMS's

/*	if (!bOpened)
	{
		OpenDepthSortDebug();
		bOpened = true;
	}

	WriteText("\nNew Frame:\n");
	//
	// Iterate through all the polygons and ensure that the order of the first polygon
	// is behind the second polygon in the array.
	//

	// Iterate through the "behind" polygons.
	for (int i_back = 0; i_back < paprpoly.uLen; i_back++)
	{
		// Iterate through looking for duplicate polygons.
		for (int i_front = i_back + 1; i_front < paprpoly.uLen; i_front++)
			if (paprpoly[i_back] == paprpoly[i_front])
				WriteText("Duplicate polygons found!\n");

		// Iterate through the "in front" polygons.
		for (i_front = i_back + 1; i_front < paprpoly.uLen; i_front++)
		{
			CRenderPolygon* prpoly_back  = paprpoly[i_back];
			CRenderPolygon* prpoly_front = paprpoly[i_front];

			// Ignore polygons that don't overlap in screen space.
			//if (!bOverlapBoxesScreen(prpoly_back, prpoly_front))
			if (prpoly_back->bScreenNoOverlap(prpoly_front))
				continue;

			if (!bIntersect2D(prpoly_back, prpoly_front))
				continue;

			//
			// Test if the back polygon lays entirely behind the front polygon or if the front
			// polygon lays entirely to the front of the back polygon. In either case, the
			// sort is correct.
			//
			if (prpoly_back->bBehind(prpoly_front))
				continue;

			// Find the overlapping area.
			float f_area_intersection = fAreaIntersection(prpoly_back, prpoly_front);
			Assert(f_area_intersection >= 0.0f);

			// If the area is below a threshold value, ignore.
			if (f_area_intersection < 10.0f)
				continue;

			//bCameraMove = false;
			// The test should never reach this point, indicate failure.
			//OpenDepthSortDebug();
			WriteFloat("Overlapping area: ", f_area_intersection);
			//WriteText("\nMismatch polygon #1 (Back):\n\n");
			//WritePolygon(prpoly_back);
			//WriteText("\nMismatch polygon #2 (Front):\n\n");
			//WritePolygon(prpoly_front);
			//CloseDepthSortDebug();
			//bIntersect2D(prpoly_back, prpoly_front);
			//Assert(0);
			//return false;
		}
	}

	// If a failure has occured yet, the test succeeded.
*/
	return true;
}


//
// Performance counters.
//
int iNumPolygonsStart = 0;
int iNumPolygonsEnd   = 0;
int iNumTests         = 0;
int iNumStep1and2     = 0;
int iNumStep3         = 0;
int iNumStep4         = 0;
int iNumStep5         = 0;
int iNumReinserted    = 0;
int iNumClipped       = 0;
int iNumInitialize    = 0;
int iNumCulled        = 0;
int iNumA             = 0;
int iNumB             = 0;
int iNumC             = 0;
int iNumD             = 0;


//
// Temporary printing class.
//

class CPrintAll
{
public:
	CPrintAll()
	{
	}
	~CPrintAll()
	{
		CloseDepthSortDebug();
	}
};

CPrintAll printall;
