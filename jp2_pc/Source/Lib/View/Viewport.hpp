/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		A viewport object that provides functions for transforming to screen coordinates,
 *		clipping and so on.
 *
 * Bugs:
 *
 * To do:
 *		This module could probably do with some more asserts.
 *
 *		Add virtual coordinate member functions for moving the origin and the position of the
 *		viewport.
 *
 *		Consider changing the viewport to a non-template class with the non-virtual coordinates
 *		represented by a floating point type.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Viewport.hpp                                                $
 * 
 * 15    97/08/11 12:22 Speter
 * Made CTransLinear use the * operator, like other transforms.
 * 
 * 14    97/08/08 15:26 Speter
 * Moved fAspectRatio from viewport to camera properties.
 * 
 * 13    5/30/97 11:05a Agrant
 * Goofy LINT fixes.
 * 
 * 12    97-05-10 0:00 Speter
 * Now use CTransLinear2<> in viewport for transformation.  Added functions taking
 * CRectangle<>.
 * 
 * 11    96/10/31 18:24 Speter
 * Added bSquareScale member, to enforce isotropic scaling.
 * 
 * 10    96/10/18 18:19 Speter
 * Revamped viewport class.  No longer templated.  Now has arbitrary scale for virtual coords.
 * Origin is now in virtual coords.  New translation functions from screen to virtual coords.
 * Renamed all variables and functions appropriately.
 * 
 * 9     7/11/96 4:42p Mlange
 * Updated todo list.
 * 
 * 8     6-06-96 11:40a Mlange
 * The viewport's y axis now increases down the screen.
 * 
 * 7     96/05/31 17:38 Speter
 * Added AspectRatio member to Viewport.
 * 
 * 6     22-05-96 9:39a Mlange
 * Changed the main viewport variable type to a double. Added member functions for moving the
 * origin, resizing and moving the viewport. Updated the to do list. Added data member for the
 * height of the screen. Moved the external declaration for the main viewport to WinShell.hpp.
 * 
 * 5     16-05-96 6:41p Mlange
 * Included "Viewport.cpp" in "Viewport.hpp."
 * 
 * 4     16-05-96 6:37p Mlange
 * Changed global viewport variable to pvMain.
 * 
 * 3     5/16/96 6:02p Pkeet
 * Made class into template. Removed unnecessary #includes.
 * 
 * 2     5/03/96 12:46p Pkeet
 * First version.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_VIEW_VIEWPORT_HPP
#define HEADER_LIB_VIEW_VIEWPORT_HPP

//
// Global includes and #defines.
//

#include "Lib/Transform/TransLinear.hpp"

//*********************************************************************************************
//
class CViewport
//
// The viewport is an object that provides data and translation services for using screen
// coordinates from other coordinate systems.
//
// Prefix: vp
//
// Notes:
//		The viewport works with screen and virtual coordinates. For screen coordinates,
//		the Y axis moves in a positive direction while going 'down' on the screen. The
//		viewport has a position and extent on the screen.  For virtual coordinates, the
//		viewport width and height are mapped to an arbitrary virtual width and height,
//		and the virtual origin can be anyplace in the viewport.  If virtual height is
//		negative, the virtual Y axis moves upward from the virtual origin.
//
//          Position x, y
//			    +--------------------------------------------------   -+
//		  |     |                                                  |   |
//		  |     |                                                  |   |
//		  |     |            |                                     |   |
//		  | +y  |           -+- Virtual Origin                     |   |
//		  |     |            |                                     |    ]- Height
//		  |     |                                                  |   |
//		  |     |                                                  |   |
//		 \|/    |                                                  |   |
//		        |                                                  |   |
//			     --------------------------------------------------   -/
//		                            +x ------>
//
//			    |                                                  |
//              \------------------------\/------------------------/
//                                      Width
//
//**************************************
{
public:
	// Typedef the coordinate types, for ease of self-documentation.

	// Prefix: sc
	// A value in screen (pixel) coordinates.
	typedef float	TScreen;		

	// Prefix: vc
	// A value in virtual coordinates.
	typedef float	TVirtual;

	// Screen dimensions.
	TScreen scWidth, scHeight;			// Extent of viewport on the screen.
	TScreen	scPositionX, scPositionY;	// Position of upper left corner of viewport on the screen.

	// Virtual dimensions.
	TVirtual vcWidth, vcHeight;			// Virtual viewport width and height.
	TVirtual vcOriginX, vcOriginY;		// Virtual coord origin, relative to scPosition.

protected:

	bool bSquareScale;					// Force scaling to be same in X and Y dimensions.
	CTransLinear2<> tlr2Trans;			// Used to actually transform coords.

public:

	//*****************************************************************************************
	//
	// CViewport constructors and destructors.
	//

	//*****************************************************************************************
	CViewport
	(
	);

	//*****************************************************************************************
	//
	// CViewport member functions.
	//

	//*****************************************************************************************
	//
	void SetScreen
	(
		const CRectangle<>& rc_screen		// The extents of the screen rectangle.
	);
	//
	// Sets the extents of the screen in pixel coordinates.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetPosition
	(
		TScreen sc_x,
		TScreen sc_y		// The new position of the viewport in pixel coordinates.
	);
	//
	// Sets the position of the viewport in pixel coordinates.
	//
	//**************************************


	//*****************************************************************************************
	//
	void MovePosition
	(
		TScreen sc_delta_x,
		TScreen sc_delta_y		// The amount to move.
	)
	//
	// Moves the viewport by the given amount in pixel coordinates.
	//
	//**************************************
	{
		SetPosition(scPositionX + sc_delta_x, scPositionY + sc_delta_y);
	}

	//*****************************************************************************************
	//
	void SetSize
	(
		TScreen sc_width,
		TScreen sc_height			// The screen height of the viewport.
	);
	//
	// Sets the size of the viewport in pixel coordinates.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ReSize
	(
		TScreen sc_delta_width, 
		TScreen sc_delta_height		// The amount to adjust the size of the viewport by.
	)
	//
	// Resizes the viewport by the given amount in pixels.
	//
	//**************************************
	{
		SetSize(scWidth + sc_delta_width, scHeight + sc_delta_height);
	}

	//*****************************************************************************************
	//
	void SetVirtual
	(
		const CRectangle<>& rc_virtual,		// The extents of the virtual rectangle.
		bool b_square_scale = false			// Force scaling the same in X and Y dimensions.
	);
	//
	// Sets the extents of the screen in virtual coordinates.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetVirtualOrigin
	(
		TVirtual v_x,
		TVirtual v_y				// The new viewport origin in virtual coordinates.
	);
	//
	// Sets the origin of the viewport in virtual coordinates.
	//
	//**************************************

	//*****************************************************************************************
	//
	void MoveVirtualOrigin
	(
		TVirtual vc_delta_x,
		TVirtual vc_delta_y			// The amount to move the origin.
	)
	//
	// Moves the origin by the given amount in virtual coordinates.
	//
	//**************************************
	{
		SetVirtualOrigin(vcOriginX + vc_delta_x, vcOriginY + vc_delta_y);
	}

	//*****************************************************************************************
	//
	void SetVirtualSize
	(
		TVirtual vc_width,
		TVirtual vc_height,				// The new viewport size in virtual coordinates.
		bool b_square_scale = false		// Force scaling the same in X and Y dimensions.
	);
	//
	// Sets the viewport size.
	//
	//**************************************

	//*****************************************************************************************
	//
	// CViewport coordinate translation member functions.
	//

	//*****************************************************************************************
	//
	TScreen scScreenX
	(
		TVirtual vc_x		// Virtual X viewport coordinate.
	) const
	//
	// Returns.
	//		The screen coordinate corresponding to vc_x.
	//
	//**************************************
	{
		return vc_x * tlr2Trans.tlrY;
	}

	//*****************************************************************************************
	//
	TScreen scScreenY
	(
		TVirtual vc_y		// Virtual Y viewport coordinate.
	) const
	//
	// Returns.
	//		The screen coordinate corresponding to vc_y.
	//
	//**************************************
	{
		return vc_y * tlr2Trans.tlrY;
	}

	//*****************************************************************************************
	//
	TVirtual vcVirtualX
	(
		TScreen sc_x		// Screen X viewport coordinate.
	) const
	//
	// Returns.
	//		The virtual coordinate corresponding to sc_x.
	//
	//**************************************
	{
		return sc_x * ~tlr2Trans.tlrX;
	}

	//*****************************************************************************************
	//
	TVirtual vcVirtualY
	(
		TScreen sc_y		// Screen Y viewport coordinate.
	) const
	//
	// Returns.
	//		The virtual coordinate corresponding to sc_y.
	//
	//**************************************
	{
		return sc_y * ~tlr2Trans.tlrY;
	}

protected:

	void CalculateNewAdjust();
};

#endif
