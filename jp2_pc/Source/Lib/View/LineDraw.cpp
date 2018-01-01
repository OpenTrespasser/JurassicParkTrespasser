/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: Implementation of LineDraw.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/LineDraw.cpp                                                 $
 * 
 * 16    9/10/98 10:10p Shernd
 * added support for clipping to resized viewport
 * 
 * 15    98/07/08 18:34 Speter
 * Added bVER_BONES() switch.
 * 
 * 14    98/06/30 1:19 Speter
 * Changed CoordinateFrame3D to allow brightness up to 2.0 (max desat).
 * 
 * 13    98/04/13 17:58 Speter
 * Fixed bug by clipping Circle() boundary coords.
 * 
 * 12    3/19/98 3:08p Agrant
 * ADDED coordframe drawing to CDraw
 * 
 * 11    98/02/18 21:10 Speter
 * Fixed Line() taking projected CVector3's.
 * 
 * 10    98/02/18 10:42 Speter
 * Added Point3D(), MoveTo3D(), LineTo3D().
 * 
 * 9     98/02/11 15:21 Speter
 * Added Point() functions.  Implemented Clear() for non-raster.
 * 
 * 8     98/02/10 12:52 Speter
 * Extended CDraw class to allow usage in plain HWND, without a CRaster.
 * 
 * 7     98/02/04 14:43 Speter
 * Added 3D draw functions, for lines and boxes.
 * 
 * 6     11/04/97 6:34p Mlange
 * Added CDraw::Circle().
 * 
 * 5     97/08/11 12:22 Speter
 * Made CTransLinear use the * operator, like other transforms.
 * 
 * 4     8/05/97 10:55a Mlange
 * Made many functions const. Added pen style functionality.
 * 
 * 3     97/08/01 15:14 Speter
 * CDraw() and SetVirtualRect() now take a b_square parameter.
 * 
 * 2     97/05/29 15:54 Speter
 * Added Line() taking CVector3<>'s, for negative Z rejection.
 * 
 * 1     97/05/21 17:27 Speter
 * New module allowing GDI drawing into raster.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "LineDraw.hpp"

#include "Lib/View/RasterVid.hpp"

//*********************************************************************************************
//
// class CDraw implementation.
//

	const float fCOORD_MIN	= -20000.0;
	const float fCOORD_MAX	=  20000.0;
	const float fZ_MIN		=  1.0;

	//*********************************************************************************************
	CDraw::CDraw(HWND hwnd)
		: hwndWin(hwnd)
	{
#if bVER_BONES()
		// Set default colour to white.
		clrCurrent = CColour(1.0, 1.0, 1.0);
		iStyle     = PS_SOLID;
		Lock();
#endif
	}

	//*********************************************************************************************
	CDraw::CDraw(HWND hwnd, const CRectangle<>& rect_virtual, bool b_square)
	{
#if bVER_BONES()
		new(this) CDraw(hwnd);
		SetVirtualRect(rect_virtual, b_square);
#endif
	}

	//*********************************************************************************************
	CDraw::CDraw(rptr<CRasterWin> prasw)
		: hwndWin(0)
	{
#if bVER_BONES()
		Assert(prasw);

		praswWin = prasw;

		// Set default colour to white.
		clrCurrent = CColour(1.0, 1.0, 1.0);
		iStyle     = PS_SOLID;
		Lock();
#endif
	}

	//*********************************************************************************************
	CDraw::CDraw(rptr<CRasterWin> prasw, const CRectangle<>& rect_virtual, bool b_square)
	{
#if bVER_BONES()
		new(this) CDraw(prasw);
		SetVirtualRect(rect_virtual, b_square);
#endif
	}

	//*********************************************************************************************
	CDraw::~CDraw()
	{
#if bVER_BONES()
		Unlock();
#endif
	}

	//*********************************************************************************************
	void CDraw::Lock()
	{
#if bVER_BONES()
		if (praswWin)
		{
			praswWin->Unlock();
			hdcScreen = praswWin->hdcGet();
		}
		else
			hdcScreen = GetDC(hwndWin);

		hpenOld = 0;

        OffsetWindowOrgEx(hdcScreen, -praswWin->iOffsetX, -praswWin->iOffsetY, &ptOrig);
        hrgnClip = CreateRectRgn(praswWin->iOffsetX, 
                                 praswWin->iOffsetY, 
                                 praswWin->iOffsetX + praswWin->iWidth, 
                                 praswWin->iOffsetY + praswWin->iHeight);
        SelectClipRgn(hdcScreen, hrgnClip);

		// Set the brush to null, for no fill.
		HBRUSH hbrush_old = (HBRUSH)SelectObject(hdcScreen, GetStockObject(NULL_BRUSH));

//		if (hbrush_old)
//			DeleteObject(hbrush_old);

		SetPen();
#endif
	}

	//*********************************************************************************************
	void CDraw::Unlock()
	{
#if bVER_BONES()
		// Delete the old pen object.
		if (hpenOld)
		{
			hpenOld = (HPEN)SelectObject(hdcScreen, hpenOld);
			DeleteObject(hpenOld);
		}

        OffsetWindowOrgEx(hdcScreen, praswWin->iOffsetX, praswWin->iOffsetY, &ptOrig);
        SelectClipRgn(hdcScreen, NULL);
        DeleteObject(hrgnClip);

		// Restore the device context for the raster.
		if (praswWin)
			praswWin->ReleaseDC(hdcScreen);
		else
			ReleaseDC(hwndWin, hdcScreen);
#endif
	}

	//*********************************************************************************************
	void CDraw::Clear(CColour clr)
	{
#if bVER_BONES()
		if (praswWin)
		{
			Unlock();
			praswWin->Clear(clr);
			Lock();
		}
		else
		{
			// Use pure GDI.  Inefficient to create/delete brush, but tough.
			HBRUSH hbr = CreateSolidBrush(clr.clrReverseRGB());
			RECT rect;
			GetClientRect(hwndWin, &rect);
			FillRect(hdcScreen, &rect, hbr);
			DeleteObject(hbr);
		}
#endif
	}

	//*********************************************************************************************
	void CDraw::Flip()
	{
#if bVER_BONES()
		if (praswWin)
		{
			Unlock();
			praswWin->Flip();
			Lock();
		}
		// Irrelevant for non-rastered GDI mode.
#endif
	}

	//*********************************************************************************************
	void CDraw::Colour(CColour clr)
	{
#if bVER_BONES()
		clrCurrent = clr;

		SetPen();
#endif
	}

	//*********************************************************************************************
	void CDraw::Style(int i_style)
	{
#if bVER_BONES()
		iStyle = i_style;

		SetPen();
#endif
	}


	//*********************************************************************************************
	void CDraw::SetVirtualRect(const CRectangle<>& rect, bool b_square)
	{
#if bVER_BONES()
		int i_width, i_height;

		if (praswWin)
		{
			i_width = praswWin->iWidth;
			i_height = praswWin->iHeight;
		}
		else
		{
			RECT	rect;
			GetClientRect(hwndWin, &rect);
			i_width = rect.right - rect.left;
			i_height = rect.bottom - rect.top;
		}

		tlr2Mapping = CTransLinear2<>
		(
			rect,

			// Add 0.5 to raster dimensions to implement rounding.
			CRectangle<>(0.5, 0.5, i_width, i_height),
			b_square
		);
/*
		SIZE size_v, size_w;
		POINT point_v, point_w;

		GetViewportExtEx(hdcScreen, &size_v);
		GetViewportOrgEx(hdcScreen, &point_v);
		GetWindowExtEx(hdcScreen, &size_w);
		GetWindowOrgEx(hdcScreen, &point_w);
*/
#endif
	}

	//*********************************************************************************************
	void CDraw::SetVirtualRect()
	{
#if bVER_BONES()
		tlr2Mapping = CTransLinear2<>();
#endif
	}

	//*********************************************************************************************
	void CDraw::Point(float f_sx, float f_sy) const
	{
#if bVER_BONES()
		Assert(hdcScreen)
		::SetPixel(hdcScreen, f_sx * tlr2Mapping.tlrX, f_sy * tlr2Mapping.tlrY, clrCurrent.clrReverseRGB());
#endif
	}

	//*********************************************************************************************
	void CDraw::MoveTo(float f_sx, float f_sy) const
	{
#if bVER_BONES()
		Assert(hdcScreen)

		if (bWithin(f_sx, fCOORD_MIN, fCOORD_MAX) && bWithin(f_sy, fCOORD_MIN, fCOORD_MAX))
			::MoveToEx(hdcScreen, f_sx * tlr2Mapping.tlrX, f_sy * tlr2Mapping.tlrY, 0);
#endif
	}

	//*********************************************************************************************
	void CDraw::LineTo(float f_sx, float f_sy) const
	{
#if bVER_BONES()
		Assert(hdcScreen)

		if (bWithin(f_sx, fCOORD_MIN, fCOORD_MAX) && bWithin(f_sy, fCOORD_MIN, fCOORD_MAX))
			::LineTo(hdcScreen, f_sx * tlr2Mapping.tlrX, f_sy * tlr2Mapping.tlrY);
#endif
	}

	//*********************************************************************************************
	void CDraw::Line(float f_sx0, float f_sy0, float f_sx1, float f_sy1) const
	{
#if bVER_BONES()
		if (bWithin(f_sx0, fCOORD_MIN, fCOORD_MAX) && bWithin(f_sy0, fCOORD_MIN, fCOORD_MAX))
			if (bWithin(f_sx1, fCOORD_MIN, fCOORD_MAX) && bWithin(f_sy1, fCOORD_MIN, fCOORD_MAX))
			{
				// Draw the line.
				MoveTo(f_sx0, f_sy0);
				LineTo(f_sx1, f_sy1);
			}
#endif
	}

	//*********************************************************************************************
	void CDraw::Line(CVector3<> v3_s0, CVector3<> v3_s1) const
	{
#if bVER_BONES()
		// Reject the line if .tZ is non-positive.
		if (v3_s0.tZ <= 0 || v3_s1.tZ <= 0)
		{
			// Unproject the points, and call Line3D.
			TReal r_z0 = 1.0 / v3_s0.tZ;
			TReal r_z1 = 1.0 / v3_s1.tZ;
			
			Line3D
			(
				CVector3<>(v3_s0.tX * r_z0, v3_s0.tY * r_z0, r_z0),
				CVector3<>(v3_s1.tX * r_z1, v3_s1.tY * r_z1, r_z1)
			);
		}
		else
			Line(v3_s0.tX, v3_s0.tY, v3_s1.tX, v3_s1.tY);
#endif
	}

	//*********************************************************************************************
	void CDraw::Point3D(CVector3<> v3) const
	{
#if bVER_BONES()
		// Clip against the fZ_MIN plane.
		if (v3.tZ > fZ_MIN)
			Point(v3.v2Project());
#endif
	}

	//*********************************************************************************************
	void CDraw::Line3D(CVector3<> v3_s0, CVector3<> v3_s1) const
	{
#if bVER_BONES()
		// Clip against the fZ_MIN plane.
		if (v3_s0.tZ < fZ_MIN)
		{
			if (v3_s1.tZ < fZ_MIN)
				// Do not draw line if both are too near.
				return;

			TReal r_t = (fZ_MIN - v3_s0.tZ) / (v3_s1.tZ - v3_s0.tZ);
			Assert(bWithin(r_t, 0.0, 1.0));
			v3_s0 = v3_s0 * (1.0 - r_t) + v3_s1 * r_t;
		}
		else if (v3_s1.tZ < fZ_MIN)
		{
			TReal r_t = (fZ_MIN - v3_s1.tZ) / (v3_s0.tZ - v3_s1.tZ);
			Assert(bWithin(r_t, 0.0, 1.0));
			v3_s1 = v3_s1 * (1.0 - r_t) + v3_s0 * r_t;
		}

		Line(v3_s0.v2Project(), v3_s1.v2Project());

		MoveTo3D(v3_s1);
#endif
	}

	//*********************************************************************************************
	void CDraw::MoveTo3D(CVector3<> v3) const
	{
#if bVER_BONES()
		// v3Current should be 'mutable'.
		NonConst(v3Current) = v3;
#endif
	}

	//*********************************************************************************************
	void CDraw::LineTo3D(CVector3<> v3) const
	{
#if bVER_BONES()
		Line3D(v3Current, v3);
#endif
	}

	//*********************************************************************************************
	void CDraw::Circle(float f_cx, float f_cy, float f_radius) const
	{
#if bVER_BONES()
		Assert(hdcScreen);

		CVector2<> v2_top_left(    f_cx - f_radius, f_cy - f_radius);
		CVector2<> v2_bottom_right(f_cx + f_radius, f_cy + f_radius);

		v2_top_left     *= tlr2Mapping;
		v2_bottom_right *= tlr2Mapping;

		if (bWithin(v2_top_left.tX, fCOORD_MIN, fCOORD_MAX) && 
			bWithin(v2_top_left.tY, fCOORD_MIN, fCOORD_MAX) &&
			bWithin(v2_bottom_right.tX, fCOORD_MIN, fCOORD_MAX) && 
			bWithin(v2_bottom_right.tY, fCOORD_MIN, fCOORD_MAX))
			::Ellipse(hdcScreen, v2_top_left.tX, v2_top_left.tY, v2_bottom_right.tX, v2_bottom_right.tY);
#endif
	}

	//*********************************************************************************************
	void CDraw::Box3D(const CTransform3<>& tf3_box) const
	{
#if bVER_BONES()
		CVector3<> v3_start;

		// Draw X lines.
		v3_start = tf3_box.v3Pos;			Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3X);
		v3_start += tf3_box.mx3Mat.v3Y;		Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3X);
		v3_start += tf3_box.mx3Mat.v3Z;		Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3X);
		v3_start -= tf3_box.mx3Mat.v3Y;		Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3X);

		// Draw Y lines.
		v3_start = tf3_box.v3Pos;			Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3Y);
		v3_start += tf3_box.mx3Mat.v3X;		Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3Y);
		v3_start += tf3_box.mx3Mat.v3Z;		Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3Y);
		v3_start -= tf3_box.mx3Mat.v3X;		Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3Y);

		// Draw Z lines.
		v3_start = tf3_box.v3Pos;			Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3Z);
		v3_start += tf3_box.mx3Mat.v3X;		Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3Z);
		v3_start += tf3_box.mx3Mat.v3Y;		Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3Z);
		v3_start -= tf3_box.mx3Mat.v3X;		Line3D(v3_start, v3_start + tf3_box.mx3Mat.v3Z);
#endif
	}


	//*********************************************************************************************
	void CDraw::CoordinateFrame3D(const CTransform3<>& tf3_coord, float f_brightness, TReal r_size)
	{
#if bVER_BONES()
		Assert(bWithin(f_brightness, 0.0, 2.0));

		CVector3<> v3_000 = tf3_coord.v3Pos;

		Colour(CColour(Min(f_brightness, 1.0), Max(f_brightness - 1.0, 0.0), Max(f_brightness - 1.0, 0.0)));
		Line3D
		(
			v3_000,
			CVector3<>(r_size, 0, 0) 	* tf3_coord
		);
		
		Colour(CColour(Max(f_brightness - 1.0, 0.0), Min(f_brightness, 1.0), Max(f_brightness - 1.0, 0.0)));
		Line3D
		(
			v3_000,
			CVector3<>(0, r_size, 0) 	* tf3_coord
		);
		
		Colour(CColour(Max(f_brightness - 1.0, 0.0), Max(f_brightness - 1.0, 0.0), Min(f_brightness, 1.0)));
		Line3D
		(
			v3_000,
			CVector3<>(0, 0, r_size) 	* tf3_coord
		);
#endif
	}

	//*********************************************************************************************
	void CDraw::SetPen()
	{
#if bVER_BONES()
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

		SetBkMode(hdcScreen, TRANSPARENT);
	
		HPEN hpen_new = CreatePen(iStyle, 1, clrCurrent.clrReverseRGB());

		hpenOld = (HPEN)SelectObject(hdcScreen, hpen_new);
#endif
	}

