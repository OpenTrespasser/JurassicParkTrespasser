/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: 
 *		CDraw
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/LineDraw.hpp                                                 $
 * 
 * 13    9/10/98 10:10p Shernd
 * added support for clipping to resized viewport
 * 
 * 12    98/07/08 18:34 Speter
 * Added bVER_BONES() switch.
 * 
 * 11    3/19/98 3:08p Agrant
 * ADDED coordframe drawing to CDraw
 * 
 * 10    98/02/18 10:42 Speter
 * Added Point3D(), MoveTo3D(), LineTo3D().
 * 
 * 9     98/02/11 15:20 Speter
 * Added Point() functions.
 * 
 * 8     98/02/10 12:52 Speter
 * Extended CDraw class to allow usage in plain HWND, without a CRaster.
 * 
 * 7     98/02/04 14:43 Speter
 * Added 3D draw functions, for lines and boxes.
 * 
 * 6     11/04/97 6:35p Mlange
 * Added CDraw::Circle().
 * 
 * 5     97/08/18 10:21 Speter
 * Unprotected the Lock functions.
 * 
 * 4     8/05/97 10:55a Mlange
 * Made many functions const. Added pen style functionality.
 * 
 * 3     97/08/01 15:14 Speter
 * CDraw() and SetVirtualRect() now take a b_square parameter.
 * 
 * 2     97/05/29 15:53 Speter
 * Added Line() taking CVector3<>'s, for negative Z rejection.
 * 
 * 1     97/05/21 17:27 Speter
 * New module allows GDI drawing into raster.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_LINEDRAW_HPP
#define HEADER_LIB_RENDERER_LINEDRAW_HPP

#include "Lib/W95/WinAlias.hpp"
#include "Lib/View/Colour.hpp"
#include "Lib/Transform/TransLinear.hpp"
#include "Lib/Transform/Matrix.hpp"

#include "VerBones.hpp"

class CRasterWin;

//*********************************************************************************************
//
class CDraw
//
// Prefix: draw
//
// A class allowing line drawing into a raster window.  Implemented via GDI.  This is transparent
// to most clients, but the HDC is publicly accessible if needed for a facility not provided
// by this class.
//
// All coordinates are floating point.  The mapping by default is identical with raster (pixel) 
// coords, but this can be changed via SetVirtualRect().
//
//**************************************
{
private:
	// Storage for the old pen object.
	HWND			hwndWin;			// The underlying OS window, from raster or elsewhere.
	HPEN			hpenOld;
	rptr<CRasterWin> praswWin;
	CTransLinear2<>	tlr2Mapping;		// The object mapping virtual to screen coords.
	CColour			clrCurrent;			// Remember across unlocks.
	int			    iStyle;
	CVector3<>		v3Current;			// Current moved-to point.
    POINT           ptOrig;
    HRGN            hrgnClip;

public:
	// Screen Device context.
	HDC hdcScreen;

public:

	//*********************************************************************************************
	//
	// Constructor and destructor.
	//

	//*********************************************************************************************
	CDraw
	(
		rptr<CRasterWin> prasw				// Window raster to draw on.
	);

	//*********************************************************************************************
	CDraw
	(
		rptr<CRasterWin> prasw,				// Window raster to draw on.
		const CRectangle<>& rect_virtual,	// Rectangle indicating extents of virtual coordinate system.
		bool b_square = false				// Whether to make scaling equal in both dimensions.
	);

	//*********************************************************************************************
	CDraw
	(
		HWND hwnd							// Non-raster constructor.  Just give an HWND.
	);

	//*********************************************************************************************
	CDraw
	(
		HWND hwnd,							// Non-raster constructor.  Just give an HWND.
		const CRectangle<>& rect_virtual,	// Rectangle indicating extents of virtual coordinate system.
		bool b_square = false				// Whether to make scaling equal in both dimensions.
	);

	//*********************************************************************************************
	~CDraw();

	//*********************************************************************************************
	//
	// Member functions.
	//

	//*********************************************************************************************
	//
	void Style
	(
		int i_style
	);
	//
	// Sets the current drawing style.
	//
	//**************************************


	//*********************************************************************************************
	//
	void Colour
	(
		CColour clr		// Colour to set.
	);
	//
	// Sets the current drawing colour.
	//
	//**************************************


	//*********************************************************************************************
	//
	CColour clrGetColour() const
	//
	// Returns:
	//		The current drawing colour.
	//
	//**************************************
	{
		return clrCurrent;
	}


	//*********************************************************************************************
	//
	void SetVirtualRect
	(
		const CRectangle<>& rect_virtual,	// Rectangle indicating extents of virtual coordinate system.
		bool b_square = false				// Whether to make scaling equal in both dimensions.
	);
	//
	// Sets the coordinate mapping scheme.
	// 
	// Input coordinate are given in virtual coords, which are defined by the rectangle passed to
	// this function.  This rectangle maps onto the raster extents.
	//
	//**************************************


	//*********************************************************************************************
	//
	void SetVirtualRect();
	//
	// Resets the coordinate mapping scheme to identity.
	//
	//**************************************


	//*********************************************************************************************
	//
	void Point
	(
		float f_x, float f_y			// Virtual coords.
	) const;
	//
	// Sets the point to the current colour.
	//
	//**************************************

	//*********************************************************************************************
	//
	void Point(CVector2<> v2) const
	//
	//**************************************
	{
		Point(v2.tX, v2.tY);
	}


	//*********************************************************************************************
	//
	void MoveTo
	(
		float f_x, float f_y			// Virtual coords.
	) const;
	//
	// Moves the current line cursor to the given coords.
	//
	//**************************************


	//*********************************************************************************************
	//
	void MoveTo(CVector2<> v2_s) const
	//
	//
	//**************************************
	{
		MoveTo(v2_s.tX, v2_s.tY);
	}


	//*********************************************************************************************
	//
	void LineTo
	(
		float f_x, float f_y			// Virtual coords.
	) const;
	// 
	// Draws a line from the current position to the given position, in the current colour.
	//
	//**************************************


	//*********************************************************************************************
	//
	void LineTo(CVector2<> v2_s) const
	//
	//
	//**************************************
	{
		LineTo(v2_s.tX, v2_s.tY);
	}


	//*********************************************************************************************
	//
	void Line
	(
		float f_x0, float f_y0,		// Line starting coords.
		float f_x1, float f_y1		// Line ending coords.
	) const;
	// 
	// Draws a line in between the given coords, in the current colour.
	//
	//
	//**************************************


	//*********************************************************************************************
	//
	void Line
	(
		CVector2<> v2_s0, CVector2<> v2_s1
	) const
	//
	//
	//**************************************
	{
		Line(v2_s0.tX, v2_s0.tY, v2_s1.tX, v2_s1.tY);
	}


	//*********************************************************************************************
	//
	void Circle
	(
		float f_cx, float f_cy,	// Centre of circle.
		float f_radius			// Radius.
	) const;
	//
	// Draws a circle in the current colour.
	//
	//**************************************

	//
	// 3D functions.  Take CVector3's, respresenting unprojected screen-space coords.
	//

	//*********************************************************************************************
	//
	void Point3D
	(
		CVector3<> v3						// Unprojected point.
	) const;
	//
	// Projects and draws the point.
	//
	//**************************************

	//*********************************************************************************************
	//
	void Line
	(
		CVector3<> v3_s0, CVector3<> v3_s1		// Projected CVector3<>'s defining endpoints.
	) const;
	//
	// Draws a line between the given coords, but avoids out-of-range points.
	// Each vector's .tZ coord is assumed to be an inverse Z value.  
	// If this is negative, the line is not drawn.
	//
	//**************************************

	//*********************************************************************************************
	//
	void MoveTo3D
	(
		CVector3<> v3					// Unprojected point.
	) const;
	//
	//**************************************

	//*********************************************************************************************
	//
	void LineTo3D
	(
		CVector3<> v3					// Unprojected CVector3<>'s defining endpoints.
	) const;
	//
	//**************************************

	//*********************************************************************************************
	//
	void Line3D
	(
		CVector3<> v3_s0, CVector3<> v3_s1		// Unprojected CVector3<>'s defining endpoints.
	) const;
	//
	// Draws a line between the given coords, but avoids out-of-range points.
	//
	//**************************************


	//*********************************************************************************************
	//
	void Box3D
	(
		const CTransform3<>& tf3_box		// Transform defining unit cube's placement in 3D screen space.
	) const;
	//
	//**************************************

	//*********************************************************************************************
	//
	void CoordinateFrame3D
	(
		const CTransform3<>& tf3_box,		// Transform defining unit cube's placement in 3D screen space.
		float f_brightness = 1.0f,			// Brightness of the coordinate frame  (0.0 - 1.0)
		TReal r_size = 0.1f					// Size of the coordinate frame, in some unknown kind of units.
	) ;
	//
	//**************************************

	//
	// Convenience functions for the raster, which handle unlocking/locking.
	//

	//*********************************************************************************************
	//
	void Clear
	(
		CColour clr						// Colour to clear to.
	);
	//
	// Clears the entire raster.
	//
	//**************************************


	//*********************************************************************************************
	//
	void Flip();
	//
	// Flips the raster, making it visible.
	//
	//**************************************


	//*********************************************************************************************
	//
	void Unlock();
	//
	// Release this object's hold on the raster, to allow other operations such as flipping.
	//
	//**************************************


	//*********************************************************************************************
	//
	void Lock();
	//
	// Restore this object's hold on the raster.  Must be done after Unlocking, or drawing will fail.
	//
	//**************************************

private:
	//*********************************************************************************************
	//
	void SetPen();
	//
	// Create a HPEN for this drawing object in the current colour and style..
	//
	//**************************************

};

#endif
