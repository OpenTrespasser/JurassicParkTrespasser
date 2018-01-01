/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Implementation of 'Viewport.hpp.'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Viewport.cpp                                                $
 * 
 * 18    97/08/08 15:26 Speter
 * Moved fAspectRatio from viewport to camera properties.
 * 
 * 17    97/06/06 12:59 Speter
 * Changes for CRectangle<>.
 * 
 * 16    97-05-10 0:18 Speter
 * Oops.
 * 
 * 15    97-05-10 0:00 Speter
 * Now use CTransLinear2<> in viewport for transformation.  Added functions taking
 * CRectangle<>.
 * 
 * 14    96/11/05 20:24 Speter
 * Fixed bug in sqiare scale calculation.
 * 
 * 13    96/10/31 18:24 Speter
 * Added bSquareScale member, to enforce isotropic scaling.
 * 
 * 12    96/10/25 14:36 Speter
 * Changed default virtual origin to be centre of screen.
 * 
 * 11    96/10/18 18:19 Speter
 * Revamped viewport class.  No longer templated.  Now has arbitrary scale for virtual coords.
 * Origin is now in virtual coords.  New translation functions from screen to virtual coords.
 * Renamed all variables and functions appropriately.
 * 
 * 10    7/03/96 2:08p Pkeet
 * Added Asserts.
 * 
 * 9     6-06-96 11:40a Mlange
 * The viewport's y axis now increases down the screen.
 * 
 * 8     96/05/31 17:38 Speter
 * Added AspectRatio member to Viewport.
 * 
 * 7     22-05-96 9:40a Mlange
 * Added implementation for new member functions. Setting the size of the viewport now no
 * longer causes the position to be rescaled. The adjust Y variable is now based on the screen
 * height data member.
 * 
 * 6     16-05-96 6:37p Mlange
 * Changed global viewport variable to pvMain.
 * 
 * 5     5/16/96 6:02p Pkeet
 * Made class into template. Removed unnecessary #includes.
 * 
 * 4     96/05/14 10:53 Speter
 * Removed include of "Gameloop.hpp".
 * 
 * 3     96/05/08 14:09 Speter
 * Updated for move of View/ files to Lib/View/
 * 
 * 2     5/03/96 12:46p Pkeet
 * First version.
 * 
 *********************************************************************************************/

//
// Macros, includes and definitions.
//

#include "Common.hpp"
#include "Viewport.hpp"


//*********************************************************************************************
//
// CViewport implementation.
//
//

	//*****************************************************************************************
	//
	// CViewport constructors and destructors.
	//

	//*****************************************************************************************
	CViewport::CViewport()
	{
		// Initialize all values to defaults.
		scWidth     = 1;
		scHeight    = 1;
		scPositionX = 0;
		scPositionY = 0;

		// The default virtual system has extents of 1, and has its origin at the centre.
		vcWidth		= 1;
		vcHeight	= 1;
		vcOriginX   = 0.5;
		vcOriginY   = 0.5;

		bSquareScale = false;

		CalculateNewAdjust();
	}

	//*****************************************************************************************
	//
	// CViewport member functions.
	//

	//*****************************************************************************************
	void CViewport::SetScreen(const CRectangle<TScreen>& rc_screen)
	{
		scPositionX = rc_screen.tX0();
		scPositionY = rc_screen.tY0();
		scWidth     = rc_screen.tWidth();
		scHeight    = rc_screen.tHeight();
		CalculateNewAdjust();
	}

	//*****************************************************************************************
	void CViewport::SetSize(TScreen sc_width, TScreen sc_height)
	{
		//
		// Set the new width and height.
		//
		scWidth  = sc_width;
		scHeight = sc_height;
		CalculateNewAdjust();
	}

	//*****************************************************************************************
	void CViewport::SetPosition(TScreen sc_x, TScreen sc_y)
	{
		scPositionX = sc_x;
		scPositionY = sc_y;
		CalculateNewAdjust();
	}

	//*****************************************************************************************
	void CViewport::SetVirtual(const CRectangle<TVirtual>& rc_virtual, bool b_square_scale)
	{
		// Because of the definition of vcOrigin{X,Y}, it is the negative of the starting
		// coordinate of the rectangle.
		vcOriginX	= -rc_virtual.tX0();
		vcOriginY	= -rc_virtual.tY0();
		vcWidth     = rc_virtual.tWidth();
		vcHeight    = rc_virtual.tHeight();
		bSquareScale = b_square_scale;
		CalculateNewAdjust();
	}

	//*****************************************************************************************
	void CViewport::SetVirtualOrigin(TVirtual vc_x, TVirtual vc_y)
	{
		vcOriginX = vc_x;
		vcOriginY = vc_y;
		CalculateNewAdjust();
	}

	//*****************************************************************************************
	void CViewport::SetVirtualSize(TVirtual vc_width, TVirtual vc_height, bool b_square_scale)
	{
		vcWidth = vc_width;
		vcHeight = vc_height;
		bSquareScale = b_square_scale;
		CalculateNewAdjust();
	}

	//*****************************************************************************************
	//
	// Protected member functions.

	//*****************************************************************************************
	void CViewport::CalculateNewAdjust()
	{
		Assert(scWidth != 0);
		Assert(scHeight != 0);

		Assert(vcWidth != 0);
		Assert(vcHeight != 0);

		tlr2Trans = CTransLinear2<>
		(
			CRectangle<>(-vcOriginX, -vcOriginY, vcWidth, vcHeight),
			CRectangle<>(scPositionX, scPositionY, scWidth, scHeight),
			bSquareScale
		);
	}
