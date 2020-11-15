/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Shell to test wave modeling.
 *
 * Bugs:
 *		Sometimes, when drawing the wave with multiple mouse drags, the timer is not paused
 *		for the whole sequence, and the step value for the next render is large.
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Physics/WaveTest.cpp                                            $
 * 
 * 18    97/10/02 12:09 Speter
 * Added boundary condition parameter back into CSimWater2D.
 * 
 * 17    97-05-06 17:16 Speter
 * Resurrected wavetest, updating for changes in lib.
 * 
 * 16    96/12/31 17:08 Speter
 * Updated for rptr.
 * 
 * 15    96/11/14 17:06 Speter
 * Updated for change in mr_gravity parameter.
 * 
 * 14    96/11/11 19:15 Speter
 * Added flags to enable greyscale rendering.
 * 
 * 13    96/11/08 18:32 Speter
 * Added menus to the app.
 * Added a random depth-generation function.
 * Converted separate mouse coords to CVector2<>'s.
 * 
 * 12    96/11/05 20:47 Speter
 * Added depth dependence to 2D sim, with interface.
 * 
 * 11    96/11/05 16:45 Speter
 * Added interface for setting wave height in 2D sim.
 * Now use CAppShell::bKeyState() to keep track of mouse buttons.
 * 
 * 10    96/11/04 16:59 Speter
 * Put variable-velocity sims back in, with real-time integrator.  Added Iterate and
 * ApplyBoundary virtual functions.
 * Changed TMeasure from double to float.
 * 
 * 
 * 9     96/11/01 17:40 Speter
 * Made LineDraw class round to integer screen coords: fixed problem of apparently not damping.
 * Commented out water version until it gets restored in Waves.cpp.
 * Minor changes to timings and initial wave values.
 * 
 * 8     96/10/31 18:26 Speter
 * Added driver for 2D wave sim.
 * 
 * 
 * 6     96/10/28 14:49 Speter
 * Added capability for varying velocity.
 * Added capability and interface for varying water depth.
 * 
 * 5     96/10/25 14:40 Speter
 * Corrected damping parameter in equations, added more parameters.
 * Fixed mouse wave drawing for the most part.
 * 
 * 4     96/10/18 19:02 Speter
 * Now use vpMain to scale coordinates to screen.
 * 
 * 3     96/10/15 20:04 Speter
 * Now we have gravity and damping.
 * 
 * 2     96/10/15 18:18 Speter
 * First working version.  Actually models 1-d waves with rigid endpoints!
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Shell/AppShell.hpp"
#include "Shell/WinRenderTools.hpp"
#include "Lib/Transform/Vector.hpp"
#include "Lib/Physics/Waves.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/View/Viewport.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/Sys/Timer.hpp"
#include "Lib/Sys/Textout.hpp"

static const float fSQRT2 = sqrt(2.0);
static const float fINV_SQRT2 = sqrt(0.5);

class CLineDraw
{
private:
	// Storage for the old pen object.
	HPEN hpenOld;

	// Screen Device context.
	rptr<CRasterWin> praswWin;
	CViewport* pvpScr;
	HDC hdcScreen;

public:

	//*********************************************************************************************
	//
	// Constructor.
	//

	CLineDraw(rptr<CRasterWin> prasw, CViewport* pvp)
	{
		// Get the device contect for the raster.
		praswWin = prasw;
		pvpScr = pvp;
		prasw->Unlock();
		hdcScreen = prasw->hdcGet();

		hpenOld = 0;

		// Create the new pen object and set its colour to white.
		LineColour(CColour(255, 255, 255));
	}

	~CLineDraw()
	{
		// Delete the old pen object.
		if (hpenOld)
		{
			hpenOld = (HPEN)SelectObject(hdcScreen, hpenOld);
			DeleteObject(hpenOld);
		}

		// Restore the device context for the raster.
		praswWin->ReleaseDC(hdcScreen);
	}

	//*********************************************************************************************
	//
	// Member functions.
	//

	//*********************************************************************************************
	void LineColour(CColour clr)
	{
		Assert(hdcScreen)

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
		HPEN hpen_new = CreatePen(PS_SOLID, 1, clr.clrReverseRGB());
		hpenOld = (HPEN)SelectObject(hdcScreen, hpen_new);
	}

	//*********************************************************************************************
	void MoveTo(float f_x, float f_y)
	{
		Assert(hdcScreen)

		// Draw the line.
		::MoveToEx
		(
			hdcScreen, 
			pvpScr->scScreenX(f_x) + 0.5,
			pvpScr->scScreenY(f_y) + 0.5,
			0
		);
	}

	//*********************************************************************************************
	void LineTo(float f_x, float f_y)
	{
		Assert(hdcScreen)

		::LineTo
		(
			hdcScreen,
			pvpScr->scScreenX(f_x) + 0.5,
			pvpScr->scScreenY(f_y) + 0.5
		);
	}

	//*********************************************************************************************
	void Line(float f_x0, float f_y0, float f_x1, float f_y1)
	{
		// Draw the line.
		MoveTo(f_x0, f_y0);
		LineTo(f_x1, f_y1);
	}

};


//*********************************************************************************************
//
class CAppShellWave: public CAppShell, public CSimWater
//
// Implement the CAppShell interface.
//
//**************************************
{
	typedef CSimWater parent;

public:

	CViewport vpScr;	
	CTimer tmrStep;
	bool bFirst;
	CVector2<> v2Mouse;

	//*********************************************************************************************
	//
	// Constructor.
	//

	CAppShellWave()
		: CSimWater
		(
			128,				// Number of elements along length.
			100.0,				// Length of wave medium.
			10.0,				// Default depth.
			.2,					// Damping. (alpha)
			25,					// Restoring. (beta)
			0					// Gravity.
		)
	{

/*
		// Set the waves to an initial value.  
		for (int u = 0; u < pamAmplitude.uLen; u++)
			pamAmplitude[u] = 25 * mrFunk((float)u / pamAmplitude.uLen);
		CopyPrev();
*/
//		pamAmplitude[pamAmplitude.uLen/2] = -0.5;

		// Stick a sine wave in the middle.  
		for (int u = 0; u < pamAmplitude.uLen; u++)
			pamAmplitude[u] = 10 * mrFunk((float)u / pamAmplitude.uLen);
		CopyPrev();

		// Set the new wave based on the velocity of propagation rightward, and time step.
//		for (u = 1; u < pamAmplitude.uLen; u++)
//			pamAmplitude[u] = 0.25 * mrFunk((float)u / pamAmplitude.uLen * 2 - 0.5 - (mrVelocity * sStep / mLength));

/*
		for (int u = 0; u < pamAmplitude.uLen/10; u++)
			pamAmplitude[pamAmplitude.uLen/2 + u] = - 0.49 * CAngle(dPI * 10 * (float)u / pamAmplitude.uLen).fSin();
		SwapPrev();
		for (u = 0; u < pamAmplitude.uLen/10; u++)
			pamAmplitude[pamAmplitude.uLen/2 + u] = - 0.50 * CAngle(dPI * 10 * (float)u / pamAmplitude.uLen).fSin();
*/

		bFirst = true;
	}

	//*********************************************************************************************
	//
	// Member functions.
	//


	//*********************************************************************************************
	static TMeasure mrFunk(TMeasure mr_pos)
	{
		if (!bWithin(mr_pos, 0, 1))
			return 0;
		CAngle ang = d2_PI * mr_pos;
		return ang.fSin();

/*		
		float ang_r = asin(sin(ang) / 1.33);
		float f_sin1 = sin(ang - ang_r);
		float f_sin2 = sin(ang + ang_r);
		float f_tan1 = tan(ang - ang_r);
		float f_tan2 = tan(ang + ang_r);

		return f_sin1 * f_sin1 / 2.0 / f_sin2 / f_sin2
		     + f_tan1 * f_tan1 / 2.0 / f_tan2 / f_tan2;
*/
	}

	//*********************************************************************************************
	void ModifyWave(CVector2<> v2_mouse)
	{
		//
		// Set the wave values according to line between previous and current mouse positions.
		//

		int i_index0 = v2Mouse.tX / mInterval;
		int i_index = v2_mouse.tX / mInterval;

		float f_slope = i_index != i_index0 ? (v2Mouse.tY - v2_mouse.tY) / (i_index0 - i_index) : 0;

		while (i_index < i_index0)
		{
			if (i_index < pamAmplitude.uLen)
			{
				if (bKeyState(VK_LBUTTON))
					pamAmplitude[i_index] = v2_mouse.tY;
				else if (bKeyState(VK_RBUTTON))
					pamDepth[i_index] = -v2_mouse.tY;
			}
			v2_mouse.tY += f_slope;
			i_index++;
		}
		while (i_index >= i_index0)
		{
			if (i_index < pamAmplitude.uLen)
			{
				if (bKeyState(VK_LBUTTON))
					pamAmplitude[i_index] = v2_mouse.tY;
				else if (bKeyState(VK_RBUTTON))
					pamDepth[i_index] = -v2_mouse.tY;
			}
			v2_mouse.tY -= f_slope;
			i_index--;
		}
		if (!bContinuous())
			Repaint();
	}

	void MouseMove(CVector2<> v2_mouse)
	{
		// Handle virtual coords.
		if (bKeyState(VK_LBUTTON) || bKeyState(VK_RBUTTON))
			ModifyWave(v2_mouse);
		v2Mouse = v2_mouse;
	}

	//*********************************************************************************************
	//
	// Overrides.
	//

	void Init()
	{
		vpScr.SetVirtualSize(mLength, -mLength);
		vpScr.SetVirtualOrigin(0, -mLength/2);

		SetContinuous(true);
	}

	void NewRaster()
	{
		vpScr.SetSize(prasMainScreen->iWidth, prasMainScreen->iHeight);
	}

	void Paint()
	{ 
		prasMainScreen->Clear(CColour(0, 0, 0));

		CLineDraw ld(prasMainScreen, &vpScr);

		// First draw base line, in dark green.
		ld.LineColour(CColour(0.0, 0.5, 0.0));
		ld.Line(0, 0, mLength, 0);

		// Now draw wave, in green.
		ld.LineColour(CColour(0.0, 1.0, 0.0));
		ld.MoveTo(0, pamAmplitude[0]);

		for (int u = 0; u < pamAmplitude.uLen; u++)
		{
			ld.LineTo(u * mInterval, pamAmplitude[u]);
		}

		// Now draw bottom, in yellow.
		ld.LineColour(CColour(1.0, 1.0, 0.0));
		ld.MoveTo(0, -pamDepth[0]);
		for (u = 0; u < pamDepth.uLen; u++)
		{
			ld.LineTo(u * mInterval, -pamDepth[u]);
		}
	}

	void Step()
	{
		if (bFirst)
		{
			tmrStep.sElapsed();
			bFirst = false;
		}
		else
			parent::Step(tmrStep.sElapsed());
		Repaint();

		conStd.ClearScreen();
//		ConShowStats();
		conStd.Print("Step quantum: %.2f ms\n", sStepMax * 1000.0);
		conStd.Show();
	}

	void MouseMove(int i_x, int i_y)
	{
		// Call function handling virtual coordinates.
		MouseMove(CVector2<>(vpScr.vcVirtualX(i_x), vpScr.vcVirtualY(i_y)));
	}

	void KeyPress(int i_key)
	{
		switch (i_key)
		{
			// Left button draws the wave.
			case VK_LBUTTON:
				CTimer::Pause(true);
				SetContinuous(false);
				ModifyWave(v2Mouse);
				break;
			case -VK_LBUTTON:
				CopyPrev();
				break;

			// Right button draws the bottom.
			case VK_RBUTTON:
				CTimer::Pause(true);
				SetContinuous(false);
				ModifyWave(v2Mouse);
				break;
			case -VK_RBUTTON:
				CopyPrev();
				UpdateDepth();
				break;

			case ' ':
				CTimer::Pause(false);
				SetContinuous(true);
				break;
		}
	}
};

template<class T> T Sqr
(
	T t
)
{
	return t * t;
}

#define GRID		1
#define GREY		0
#define DRAW_SPHERE	0

//*********************************************************************************************
//
class CAppShellWave2D: public CAppShell, public CSimWater2D
//
// Implement the CAppShell interface.
//
//**************************************
{
	typedef CSimWater2D	parent;

private:

	TMeasure mrDepthDefault;

	CViewport vpScr;	
	CTimer tmrStep;
	bool bFirst;
	TPixel pixClut[256];

	CVector2<>	v2Mouse;

	CVector2<>	v2Start;
	CVector2<>	v2Mod;

	CAArray<TMeasure> pamDepth1, pamDepth2;

	int iPrevDepthIndex;
	bool bShowWater, bShowBottom;

public:

	//*********************************************************************************************
	//
	// Constructor.
	//

	CAppShellWave2D()
		: CSimWater2D
		(
			48,					// Number of elements along length.
/*
			1000.0, 1000.0,		// Length and width of wave medium.
			100.0,				// Default depth.
			.0,					// Damping. (alpha)
*/
			50.0, 50.0,			// Length and width of wave medium.
			1,					// Boundary condition.
			5.0,				// Default depth.
			.05,				// Damping (alpha).
			25					// Restoring (beta).
		),
		pamDepth1(pa2mDepth.iHeight), pamDepth2(pa2mDepth.iWidth),
		mrDepthDefault(2.0)
	{
		pamDepth1.Fill(mrDepthDefault);
		pamDepth2.Fill(mrDepthDefault);

		// Set the waves to an initial value.  

/*
		// Stick a sine wave in the middle.
		for (int i_y = 0; i_y < pa2mAmplitude.iHeight; i_y++)
			for (int i_x = 0; i_x < pa2mAmplitude.iWidth; i_x++)
				pa2mAmplitude[i_y][i_x] = 15 * mrFunk(CVector2<>
				(
					8 * ((float)i_y / pa2mAmplitude.iHeight - 0.5), 
					8 * ((float)i_x / pa2mAmplitude.iWidth - 0.5)
				).tLenSqr());
		CopyPrev();
*/
/*
		// Make a depth barrier with a double slit.
		for (int i_x = 0; i_x < pa2mDepth.iWidth; i_x++)
		{
			if (!bWithin(float(i_x) / pa2mDepth.iWidth, 0.3, 0.4) &&
				!bWithin(float(i_x) / pa2mDepth.iWidth, 0.6, 0.7))
				pa2mDepth[pa2mAmplitude.iHeight / 2][i_x] = 0;
		}
		UpdateDepth();
*/
		bFirst = true;
		iPrevDepthIndex = -1;
		bShowWater = bShowBottom = true;
	}

	//*********************************************************************************************
	//
	// Member functions.
	//

	//*********************************************************************************************
	static TMeasure mrFunk(TMeasure mr_pos)
	{
		return bWithin(mr_pos, -1, 1) ? CAngle(dPI_2 * mr_pos).fCos() : 0;
	}

#if GRID
	//*********************************************************************************************
	//
	CVector2<> v2Project
	(
		CVector3<> v3					// 3D position in sim coords.
	)
	//
	// Returns:
	//		Projection of v3 onto virtual viewport coords.
	//
	// Uses an isometric projection, as follows:
	//		X => X
	//		Z => Y
	//		Y => 45 degrees, between X and Y
	//
	// Thus:
	//		sx = x + y/sqrt(2)
	//		sy = z + y/sqrt(2)
	//
	//**********************************
	{
		return CVector2<>(v3.tX + fINV_SQRT2 * v3.tY, v3.tZ + fINV_SQRT2 * v3.tY);
	}	

	//*********************************************************************************************
	//
	CVector3<> v3ProjectXY
	(
		CVector2<> v2_scr,					// 2D screen position.
		TMeasure mr_z						// Z position to project to.
	)
	//
	// Returns:
	//		The location on the xy plane corresponding to v2_scr.
	//
	// Since:
	//		sx = x + y/sqrt(2)
	//		sy = z + y/sqrt(2)
	//
	// We find that:
	//		y = (sy - z)*sqrt(2)
	//		x = sx - y/sqrt(2)
	//		  = sx - sy + z
	//
	//**********************************
	{
		return CVector3<>(v2_scr.tX - v2_scr.tY + mr_z, (v2_scr.tY - mr_z) * fSQRT2, mr_z);
	}	

	//*********************************************************************************************
	//
	CVector3<> v3ProjectXZ
	(
		CVector2<> v2_scr,					// 2D screen position.
		TMeasure mr_y						// Y position to project to.
	)
	//
	// Returns:
	//		The location on the xz plane corresponding to v2_scr.
	//
	// We find that:
	//		x = sx - y/sqrt(2)
	//		z = sy - y/sqrt(2)
	//
	//**********************************
	{
		mr_y *= fINV_SQRT2;
		return CVector3<>(v2_scr.tX - mr_y, mr_y, v2_scr.tY - mr_y);
	}

	//*********************************************************************************************
	//
	CVector3<> v3ProjectYZ
	(
		CVector2<> v2_scr,					// 2D screen position.
		TMeasure mr_x						// X position to project to.
	)
	//
	// Returns:
	//		The location on the yz plane corresponding to v2_scr.
	//
	//		sx = x + y/sqrt(2)
	//		sy = z + y/sqrt(2)
	// We find that:
	//		y = (sx - x)*sqrt(2)
	//		z = sy - y/sqrt(2)
	//		  = sy - sx + x
	//
	//**********************************
	{
		return CVector3<>(0, (v2_scr.tX - mr_x) * fSQRT2, v2_scr.tY - v2_scr.tX + mr_x);
	}
#endif

#if DRAW_SPHERE
	//*********************************************************************************************
	//
	void SetModifyPoint
	(
		CVector2<> v2_mouse
	)
	//
	//**********************************
	{
		v2Start = v2_mouse;
		v2Mod = v3ProjectXY(v2_mouse, 0);
	}
#endif

	//*********************************************************************************************
	//
	void ModifyWave
	(
		CVector2<> v2_mouse
	)
	//
	// Modify the wave height, based on current mouse position.  This works as follows:
	// The y distance from the starting mouse position indicates height.
	// The x distance indicates radius.
	//
	//**********************************
	{
#if DRAW_SPHERE
		TMeasure mr_height = v2_mouse.tY - v2Start.tY;
		TMeasure mr_radius = Abs(v2_mouse.tX - v2Start.tX);

		// Create a hump of the given height and radius.
		for (int i_y = 0; i_y < pa2mAmplitude.iHeight; i_y++)
		{
			for (int i_x = 0; i_x < pa2mAmplitude.iWidth; i_x++)
			{
				TMeasure mr_dist = CVector2<>(i_y * mInterval - v2Mod.tX, i_x * mInterval - v2Mod.tY).tLen();
				if (mr_dist <= mr_radius)
				{
					TMeasure mr_val = CAngle(mr_dist / mr_radius).fCos();
					if (bKeyState(VK_LBUTTON))
						pa2mAmplitude[i_y][i_x] = mr_height * mr_val * mr_val;
					else if (bKeyState(VK_RBUTTON))
						pa2mDepth[i_y][i_x] = - mr_height * mr_val * mr_val;
				}
			}
		}
#else
		const TMeasure mr_mod_radius = mInterval/2;

		// Implement the Klay-Doodle: Depress the current water height with a small hemisphere.
#if GRID
		CVector2<> v2_plane = v3ProjectXY(v2_mouse, 0);
#else
		CVector2<> v2_plane = v2_mouse;
#endif
		CVector2<> v2_start = (v2_plane - CVector2<>(mr_mod_radius, mr_mod_radius)) / mInterval;
		CVector2<> v2_end =   (v2_plane + CVector2<>(mr_mod_radius, mr_mod_radius)) / mInterval;

		for (int i_y = v2_start.tX; i_y <= v2_end.tX; i_y++)
			for (int i_x = v2_start.tY; i_x <= v2_end.tY; i_x++)
				if (i_y < pa2mAmplitude.iHeight && i_x < pa2mAmplitude.iWidth)
				{
					TMeasure mr_dist = (CVector2<>(i_y * mInterval, i_x * mInterval) - v2_plane).tLen();
					pa2mAmplitude[i_y][i_x] -= mr_dist;
				}
#endif
		if (!bContinuous())
			Repaint();
	}

#if !DRAW_SPHERE
	//*********************************************************************************************
	//
	void ModifyDepth
	(
		CVector2<> v2_mouse
	)
	//
	// Modify the wave depth, based on current mouse position.
	// If the mouse is in front of the water, draw the depth along the x axis.
	// If the mouse is to the right of the water, draw the depth along the y axis.
	//
	//**********************************
	{
		CVector3<> v3_draw, v3_draw_prev;

		v3_draw = v3ProjectXZ(v2_mouse, 0);

		if (bWithin(v3_draw.tX, 0, mLength) && v3_draw.tZ < 0)
		{
			// We're drawing X depth.
			int i1 = v3_draw.tX / mInterval;
			if (iPrevDepthIndex < 0)
				iPrevDepthIndex = i1;
			Swap(i1, iPrevDepthIndex);
			for (;;)
			{
				pamDepth1[i1] = -v3_draw.tZ;

				// Update depth.
				for (int i2 = 0; i2 < pa2mDepth.iWidth; i2++)
					pa2mDepth[i1][i2] = Min(pamDepth1[i1], pamDepth2[i2]);

				if (i1 < iPrevDepthIndex)
					i1++;
				else if (i1 > iPrevDepthIndex)
					i1--;
				else
					break;
			}
		}
		else 
		{
			v3_draw = v3ProjectYZ(v2_mouse, mLength);
			if (bWithin(v3_draw.tY, 0, mWidth) && v3_draw.tZ < 0)
			{
				// We're drawing Y depth.
				int i2 = v3_draw.tY / mInterval;
				if (iPrevDepthIndex < 0)
					iPrevDepthIndex = i2;
				Swap(i2, iPrevDepthIndex);
				for (;;)
				{
					pamDepth2[i2] = -v3_draw.tZ;

					// Update depth.
					for (int i1 = 0; i1 < pa2mDepth.iHeight; i1++)
						pa2mDepth[i1][i2] = Min(pamDepth1[i1], pamDepth2[i2]);

					if (i2 < iPrevDepthIndex)
						i2++;
					else if (i2 > iPrevDepthIndex)
						i2--;
					else
						break;
				}
			}
			else
				return;
		}

		if (!bContinuous())
			Repaint();
	}
#endif

	//*********************************************************************************************
	//
	void MakeRandomDepth()
	//
	// Create a random depth array.
	//
	// Method:
	//		Travel around the perimeter of the rectangle.  At every point, calculate a random
	//		radius (adjusted from radius at previous point).  The point at this radius from the
	//		centre becomes the edge of the pond, and the depth decreases linearly from the centre
	//		to the edge.
	//
	//**********************************
	{
		//
		// First create an array of radius values for every angle.
		// Let's use as many angle steps as there are points on the perimter.
		//
		int i_steps = (pa2mDepth.iHeight + pa2mDepth.iWidth) * 2;
		CAArray<TMeasure> pamr_radius(i_steps);

		//
		// The starting radius is the maximum that will fit within the sim.
		// The radius is allowed to become larger or smaller than that.
		//
		TMeasure mr_radius = Min(mLength, mWidth) / 2;
		TMeasure mr_radius_mod = mr_radius * 0.1;

		static CRandom rand;

		for (int i = 0; i < i_steps; i++)
		{
			mr_radius += rand(-mr_radius_mod, +mr_radius_mod);

			// Can't get too low.
			SetMax(mr_radius, mr_radius_mod);
			pamr_radius[i] = mr_radius;
		}

		// Now go through the entire area, and set the depth based on polar coordinates.
		for (int i_x = 0; i_x < pa2mDepth.iHeight; i_x++)
			for (int i_y = 0; i_y < pa2mDepth.iWidth; i_y++)
			{
				// Figure out the polar coordinates for this element.
				TMeasure mr_x = i_x * mInterval - mLength/2;
				TMeasure mr_y = i_y * mInterval - mWidth/2;

				mr_radius = CVector2<>(mr_x, mr_y).tLen();
				float f_ang = atan2(mr_x, mr_y);
				if (f_ang < 0)
					f_ang += d2_PI;
				int i_ang = f_ang / d2_PI * (i_steps-1);

				//
				// The depth is proportional to the percentage distance the point is
				// from the radius for this angle to the centre.
				//
				pa2mDepth[i_x][i_y] = mrDepthDefault * (pamr_radius[i_ang] - mr_radius) / pamr_radius[i_ang];
			}

		UpdateDepth();
	}

	//********************************************************************************************
	//
	// Overrides.
	//

	//********************************************************************************************
	enum
	{
		iMENU_QUELL	= iMENU_ID_START,
		iMENU_DEPTH,
		iMENU_SHOW_WATER,
		iMENU_SHOW_BOTTOM
	};

	//********************************************************************************************
	void Init()
	{
#if GREY
		vpScr.SetVirtualSize(mLength, mWidth, true);
		vpScr.SetVirtualOrigin(0, 0);
#else
		CVector2<> v2 = v2Project(CVector3<>(mLength, mWidth, 0));
		vpScr.SetVirtualSize(v2.tX, -v2.tY * 1.2, true);
		vpScr.SetVirtualOrigin(0, -v2.tY * 1.1);
#endif

		SetContinuous(true);

		// Add some menus here.
		HMENU hmenu = hmenuAddSubMenu(0, "&Wave");

		AddMenuItem(hmenu, "&Quell",		iMENU_QUELL);
		AddMenuItem(hmenu, "Make &Depth",	iMENU_DEPTH);
		AddMenuItem(hmenu, "Show &Water",	iMENU_SHOW_WATER, &bShowWater);
		AddMenuItem(hmenu, "Show &Bottom",	iMENU_SHOW_BOTTOM, &bShowBottom);
	}

	//********************************************************************************************
	void NewRaster()
	{
		vpScr.SetSize(prasMainScreen->iWidth, prasMainScreen->iHeight);
#if GREY
		// Build my own mini-clut.
		CColour clrWater(0.0, 1.0, 1.0);
		for (int i = 0; i < 256; i++)
			pixClut[i] = prasMainScreen->pixFromColour(CColour(0, i, i));
#endif
	}

	//********************************************************************************************
	void Paint()
	{ 
#if GREY
		// Fill the screen with the water map, showing height via colour.
		int i_x_prev = 0;
		for (int i_1 = 0; i_1 < pa2mAmplitude.iHeight; i_1++)
		{
			int i_x = vpScr.scScreenX((i_1+1) * mInterval);

			int i_y_prev = 0;
			for (int i_2 = 0; i_2 < pa2mAmplitude.iWidth; i_2++)
			{
				int i_y = vpScr.scScreenY((i_2+1) * mInterval);
				// Get the colour associated with the height here.
				TMeasure mr_height = pa2mAmplitude[i_1][i_2];
				int i_index = MinMax(int(mr_height / (mLength/8) * 256) + 128, 0, 255);

				prasMainScreen->Rect(pixClut[i_index], SRect
				(
					i_x_prev, i_y_prev,
					i_x - i_x_prev, i_y - i_y_prev
				));
				i_y_prev = i_y;
			}
			i_x_prev = i_x;
		}
#else
		prasMainScreen->Clear(CColour(0, 0, 0));

		CLineDraw ld(prasMainScreen, &vpScr);

		if (bShowBottom)
		{
			// Draw the depth in brown.
			ld.LineColour(CColour(0.5, 0.2, 0.0));

			for (int i_x = 0; i_x < pa2mDepth.iWidth; i_x++)
				for (int i_y = 0; i_y < pa2mDepth.iHeight; i_y++)
				{
					CVector2<> v2 = v2Project(CVector3<>
					(
						i_y * mInterval, i_x * mInterval, -pa2mDepth[i_y][i_x]
					));
					if (i_y == 0)
						ld.MoveTo(v2.tX, v2.tY);
					else
						ld.LineTo(v2.tX, v2.tY);
				}

	/*
			// Draw the 2 controlling depth curves.
			for (int i_y = 0; i_y < pamDepth1.uLen; i_y++)
			{
				CVector2<> v2 = v2Project(CVector3<>
				(
					i_y * mInterval, 0, -pamDepth1[i_y]
				));
				if (i_y == 0)
					ld.MoveTo(v2.tX, v2.tY);
				else
					ld.LineTo(v2.tX, v2.tY);
			}

			for (i_x = 0; i_x < pamDepth2.uLen; i_x++)
			{
				CVector2<> v2 = v2Project(CVector3<>
				(
					mLength, i_x * mInterval, -pamDepth2[i_x]
				));
				if (i_y == 0)
					ld.MoveTo(v2.tX, v2.tY);
				else
					ld.LineTo(v2.tX, v2.tY);
			}
	*/
		}

		if (bShowWater)
		{
			// Now draw wave, in green.
			ld.LineColour(CColour(0.0, 1.0, 0.0));

			// Draw the vertical lines.
			for (int i_y = 0; i_y < pa2mAmplitude.iHeight; i_y++)
			{
				bool b_drew = false;
				for (int i_x = 0; i_x < pa2mAmplitude.iWidth; i_x++)
				{
					if (pa2mDepth[i_y][i_x] > 0)
					{
						CVector2<> v2 = v2Project(CVector3<>
						(
							i_y * mInterval, i_x * mInterval, pa2mAmplitude[i_y][i_x]
						));
						if (!b_drew)
							ld.MoveTo(v2.tX, v2.tY);
						else
							ld.LineTo(v2.tX, v2.tY);
						b_drew = true;
					}
					else
						b_drew = false;
				}
			}
	/*
			// Draw the horizontal lines.
			for (i_x = 0; i_x < pa2mAmplitude.iWidth; i_x++)
				for (int i_y = 0; i_y < pa2mAmplitude.iHeight; i_y++)
				{
					CVector2<> v2 = v2Project(CVector3<>
					(
						i_y * mInterval, i_x * mInterval, pa2mAmplitude[i_y][i_x]
					));
					if (i_y == 0)
						ld.MoveTo(v2.tX, v2.tY);
					else
						ld.LineTo(v2.tX, v2.tY);
				}
	*/
		}
#endif

	}

	//********************************************************************************************
	void Step()
	{
		if (bFirst)
		{
			tmrStep.sElapsed();
			bFirst = false;
		}
		else
			parent::Step(tmrStep.sElapsed());
	
		Repaint();

		conStd.ClearScreen();
//		ConShowStats();
		conStd.Print("Step quantum: %.2f ms\n", sStepMax * 1000.0);
		conStd.Show();
	}

	//********************************************************************************************
	void KeyPress(int i_key)
	{
		switch (i_key)
		{
#if DRAW_SPHERE
			case VK_LBUTTON:
			case VK_RBUTTON:
				CTimer::Pause(true);
				SetContinuous(false);
				SetModifyPoint(v2Mouse);
				break;
			case -VK_LBUTTON:
				CopyPrev();
				CTimer::Pause(false);
				SetContinuous(true);
				break;
			case -VK_RBUTTON:
				UpdateDepth ();
				break;

			case ' ':
				CTimer::Pause(false);
				SetContinuous(true);
				break;
#else
			case VK_LBUTTON:
				ModifyWave(v2Mouse);
				break;

			case VK_RBUTTON:
				SetContinuous(false);
				ModifyDepth(v2Mouse);
				break;

			case -VK_RBUTTON:
				SetContinuous(true);
				UpdateDepth ();
				iPrevDepthIndex = -1;
				break;
#endif

		}
	}

	//********************************************************************************************
	void MouseMove(CVector2<> v2_mouse)
	{
		// Handle virtual coords.
#if DRAW_SPHERE
		if (bKeyState(VK_LBUTTON) || bKeyState(VK_RBUTTON))
			ModifyWave(v2_mouse);
#else
		if (bKeyState(VK_LBUTTON))
			ModifyWave(v2_mouse);
		if (bKeyState(VK_RBUTTON))
			ModifyDepth(v2_mouse);
#endif
		v2Mouse = v2_mouse;
	}

	//********************************************************************************************
	void MouseMove(int i_x, int i_y)
	{
		// Call function handling virtual coordinates.
		MouseMove(CVector2<>(vpScr.vcVirtualX(i_x), vpScr.vcVirtualY(i_y)));
	}

	//********************************************************************************************
	void MenuCommand(int i_command, bool*)
	{
		switch (i_command)
		{
			case iMENU_QUELL:
			{
				// Reset the waves.
				for (int u_1 = 0; u_1 < pa2mAmplitude.iHeight; u_1++)
					for (int u_2 = 0; u_2 < pa2mAmplitude.iWidth; u_2++)
						pa2mAmplitude[u_1][u_2] = Max(0, -pa2mDepth[u_1][u_2]);
				CopyPrev();
				break;
			}
			case iMENU_DEPTH:
				MakeRandomDepth();
				CopyPrev();
				break;
		}
		if (!bContinuous())
			Repaint();
	}
};

CAppShell* pappMain = new CAppShellWave2D;

