/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		(class CRaster)					// From raster.hpp
 *			class CRasterVid
 *				class CRasterWin
 *
 * Bugs:
 *		In fullscreen mode, switching out of the app via Alt-Esc merely resizes the screen,
 *		and leaves the app in focus, with exclusive screen control.
 *		Access violation in GDI32 during call of SetDisplayMode.
 *		When CRasterVids or CRasterWins are being created, we round up the width to ensure even
 *		pixel counts.  However, this ends up making the *window* bigger!  Also, on some systems,
 *		at some window sizes, all windows on the screen end up invalidated on every paint.
 *		With QVision video, these things don't work.
 *			No surfaces *ever* have the VIDEOMEM flag set!
 *			No screen can be created in a bit-depth other than the current Windows depth (8-bit).
 *		Creating a single-buffered windowed CRasterWin goes kerblooey (should that even be allowed?)
 *		Memory overwrite error for CRasterWin object after running PipelineTest.
 *		When the user switches Windows resolutions, a CRasterWin doesn't know about it and doesn't
 *		  recreate its back buffer.  Blitting to the front still works, but is slower.
 *		The colours in 8-bit window mode are wrong.
 *
 * To do:
 *		Determine whether DDSCAPS_3DDEVICE flag has adverse effect when doing
 *		regular software rendering.
 *		Recreate D3DDevice when raster resized.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/RasterVid.hpp                                                $
 * 
 * 46    9/10/98 2:54p Shernd
 * Added flag to ClearBorder to use back buffer
 * 
 * 45    9/07/98 4:56p Pkeet
 * Fixed bug clearing the background with a viewport smaller than the screen dimensions.
 * 
 * 44    98.08.31 9:34p Mmouni
 * Added bHasGamma() function.
 * 
 * 43    8/20/98 1:29p Pkeet
 * Added a Z buffer. Added an additional function to restore lost surfaces.
 * 
 * 42    98.08.18 7:43p Mmouni
 * Added support for gamma flash for primary video surfaces.
 * 
 * 41    8/07/98 11:42a Pkeet
 * Added the 'bIsLocked' member function.
 * 
 * 40    7/30/98 5:56p Pkeet
 * Added the 'hwnd' parameter for setting the viewport size. Added the 'LockAll' member
 * function. Made blits and clears work correctly with a virtual viewport raster.
 * 
 * 39    7/20/98 10:32p Pkeet
 * Upgraded to Direct3D 4's interface.
 * 
 * 38    7/13/98 12:04a Pkeet
 * Removed stretching. Added code to offset the raster into a smaller area. Replace private
 * member functions with equivalent 'priv_self' member functions.
 * 
 * 37    7/08/98 5:54p Shernd
 * Added function to get the primary direct draw surface
 * 
 * 36    12/11/97 1:33p Pkeet
 * Made the 'CRasterVid' lock and unlock member functions virtual. Added private member
 * functions for use by the 'CRasterWin' constructor.
 * 
 * 35    12/08/97 6:41p Rwyatt
 * Handle resize has been replaced with a similar function that will resize the back buffer if
 * it can. This is more efficient than changing the viewport on a larger screen as no memory is
 * wasted.
 * 
 * 34    97/06/16 20:17 Speter
 * Added flag to prasReadBMP to create CRasterVid (for blitting).
 * 
 * 33    97-04-08 19:05 Speter
 * Fixed wretched bug, due to new arguments not being placed at the end.
 * 
 * 32    97-04-07 12:27 Speter
 * Added ability to create CRasterWin with back buffer of a different size, to allow stretch
 * blitting.
 * 
 * 31    97-03-31 22:27 Speter
 * Updated to use set0 instead of 0.
 * 
 * 30    97/02/19 10:31 Speter
 * Added clear timing test to determine fastest clear method.
 * 
 * 29    96/12/31 17:23 Speter
 * Updated for simplified CRaster hierarchy.  More more virtual inheritance, overloaded new(),
 * or PrivRaster.
 * Updated for rptr.
 * Moved fAspectRatio from CRaster to CRasterWin.
 * 
 * 28    96/11/04 17:01 Speter
 * Reversed bWindowed flag to bFullScreen.
 * 
 * 27    11/01/96 5:44p Pkeet
 * Made the 'FlipToGDISurface' member function non-static.
 * 
 * 26    96/10/25 14:34 Speter
 * Removed commented-out variable bVideoMemFront.
 * 
 * 25    96/10/23 15:41 Speter
 * Updated bugs list.
 * 
 * 24    96/10/22 16:31 Speter
 * Updated Bugs list.
 * 
 * 23    96/10/14 15:30 Speter
 * Updated to do.
 * 
 * 22    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 21    8/23/96 10:38a Gstull
 * Changed prasvReadBMP() back to prasReadBMP().  Now creates a CRasterVid only if D3D support
 * exists.
 * 
 * 20    96/08/15 18:55 Speter
 * Replaced b_system_mem and b_z_buffer flags in CRasterVid() constructor with new
 * CSet<ERasterFlag>.  Now handles textures with erasTexture flag.  Internal DD surface
 * dimensions are power-of-two for textures, external raster dimensions are still original
 * values.  Now takes explicit bit-depth rather than defaulting to 0 (currently handle only 8
 * bits).
 * Moved pddpal and AttachPalette() members from CRasterWin down to CRasterVid.
 * Moved prasReadBMP from Raster.cpp, changed to prasvReadBMP.  Now always reads into
 * CRasterVid, as a texture.
 * Removed CRasterWin::Lock() function, now inherits it.
 * 
 * 19    96/08/14 13:53 Speter
 * Made pddsDraw member public, as D3D stuff needs it.
 * 
 * 18    96/08/09 10:57 Speter
 * Include new CCom functionality.
 * Changed CRasterVid to support z buffers.
 * 
 * 17    96/08/05 12:24 Speter
 * Several improvements in DirectDraw handling:
 *   New b_system_mem argument in constructors.  Replaces compile-time flag.
 *   CRasterWin constructor is much improved.  Now creates flipping surfaces in video mem if it
 * can, else just creates separate surfaces.  Overall faster video.
 *   Replaced Restore() member with bRestore().  All DD operations now detect and restore lost
 * surfaces correctly.
 * Replaced setVideoMem field with bVideoMem (back surface only).  Added u4DDSFlags fields, and
 * GetDDSDesc() functions,  for clients to examine all DDSurface flags (back and front buffers).
 *   
 * 
 * 16    96/07/29 15:01 Speter
 * DD surfaces are now created with widths rounded up  by 8, so that pitch is always a whole
 * pixel count.
 * 
 * 15    7/25/96 10:40a Pkeet
 * Changed 'CPalette' to 'CPal.'
 * 
 * 14    7/16/96 11:29a Mlange
 * Moved several files to different directories, updated includes.
 * 
 * 13    96/07/03 11:34 Speter
 * Initialised setVideoMem to 0 in CRasterWin.
 * Changed GetDC() to hdcGet().
 * 
 * 12    96/07/01 22:29 Speter
 * Code review:
 * Added to do's, fixed comments.
 * 
 * 11    96/06/07 9:58 Speter
 * Changed includes, namespaces to match new Video module.
 * 
 * 10    96/06/06 14:00 Speter
 * Added more comments.
 * 
 * 9     96/06/04 16:12 Speter
 * Moved bugs from Raster module to here.
 * 
 * 8     96/05/30 17:52 Speter
 * Made bWindowed and iBuffers members public.
 * 
 * 7     96/05/30 11:10 Speter
 * Support new CPixelFormat member of CRaster, and CPixelFormat argument of AttachPalette.
 * 
 * 6     96/05/24 12:42 Speter
 * Added HandleResize method, much faster than destroying/recreating raster.
 * 
 * 5     96/05/23 16:59 Speter
 * Now Clear and Blit can be called when surface is Locked.
 * Raster module changes: 
 * Replaced SetInfo member, moved functionality into constructor.
 * Moved HLine and Rect functions into RasterDraw.
 * Set the bFlippable member after ConstructRaster, so it doesn't get trashed.
 * 
 * 4     96/05/09 11:18 Speter
 * Added ppalAttached member to CRaster, which is set by AttachPalette.  Corrected spelling of
 * "AttatchPalette".
 * 
 * 3     96/05/08 14:09 Speter
 * Updated for move of View/ files to Lib/View/
 * 
 * 2     5/07/96 10:09a Pkeet
 * Created the 'AttatchPalette' member function for CRaster and removed the 'SetPalette' member
 * function. Disabled the "inherits via dominance" warning message.
 * 
 * 1     96/05/03 16:23 Speter
 * CRasterVid and CRasterWin classes.  Moved from Raster module
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_RASTERVID_HPP
#define HEADER_LIB_VIEW_RASTERVID_HPP

#include "Raster.hpp"
#include "Video.hpp"
#include "Lib/W95/Com.hpp"
#include "Lib/W95/WinAlias.hpp"
#include "Lib/Std/Set.hpp"


//**********************************************************************************************
//
// Definitions for class CRasterVid.
//

//
// Since CRasterVid privately needs pointers to DirectDraw elements, but we don't want
// CRasterVid's clients to know about it, we declare the DD classes as opaque types.
//

// Prefix: dds
struct IDirectDrawSurface;
struct IDirectDrawSurface4;

// Prefix: ddclip
struct IDirectDrawClipper;

// Prefix: ddpal
struct IDirectDrawPalette;		

enum ERasterFlag
// Prefix: eras
{
	erasVideoMem,					// Try to put the surface in video memory.
									// This is usually faster in rendering,
									// but has the disadvantage that the debugger hangs the system
									// when stepping over calls to Lock() and GetDC().
									// Otherwise, the surface is forced into system memory.
	erasZBuffer,					// Raster is going to be a Z buffer.
	erasTexture						// Raster is going to be a texture.
};

//**********************************************************************************************
//
class CRasterVid: public CRaster
//
// Prefix: rasv
//
// An implementation of CRaster that uses DirectDraw to create and manage the surfaces,
// and provide blitting functions.  This an offscreen surface, though, not
// attached to any window.
//
// As explained in CRaster, you must "new" this class, not declare it staticly or locally.  
// The pointer you get back actually points to a multiply inherited object.  It can be, for 
// example, both a CRasterVid* and a CRaster8*.
//
//**************************************
{
public:
	CCom<IDirectDrawSurface4>	pddsDraw4;	// The DD object that manages the surface.
	uint32						u4DDSFlags;	// DD Surface flags.
	bool						bVideoMem;	// Surface is in videomemory.

protected:
	CCom<IDirectDrawPalette>	pddpal;		// Custom palette if palettised.
	bool						bLocked;	// Surface is currently locked.
											// Needed for interaction between Lock and Clear/Blit.
	enum EClearMethod
	{
		ecmDD,
		ecmDWI,
		ecmTEST
	}
	eClearMethod;
	int32						i4ClearTiming;	// Variable for tracking clear timings.

public:

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	CRasterVid
	(
		int i_width, int i_height, 			// The raster dimensions desired.
		int i_bits,							// The bitdepth desired.
		CSet<ERasterFlag> seteras = set0	// Any special flags for the raster.
	);

	~CRasterVid();

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual void Lock();

	//******************************************************************************************
	virtual bool bIsLocked()
	{
		return bLocked;
	}

	virtual void Unlock();

	void AttachPalette(CPal* ppal, CPixelFormat* ppxf = 0);

	void Clear(TPixel pix);

	void Blit(int i_dx, int i_dy, CRaster& ras_src, SRect* prect_src = 0,			
		bool b_clip = true, bool b_colour_key = 0, TPixel pix_colour_key = 0);

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	HDC hdcGet();
	//
	// Returns:
	//		A Windows DC allowing GDI access to the surface.
	//
	// Notes:
	//		You cannot call this while the raster is Locked.
	//
	//**********************************

	//******************************************************************************************
	//
	void ReleaseDC
	(
		HDC hdc							// The DC that was obtained from GetDC().
	);
	//
	// Releases the DC.  You *must* call ReleaseDC() when done with the DC.
	//
	//**********************************

	//******************************************************************************************
	//
	void GetDDSDesc
	(
		char str_dest[]
	);
	//
	// Fills str_dest with a description of the DD Surface flags.
	//
	//**********************************

protected:

	CRasterVid()
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual bool bRestore
	(
		int i_dderr						// Error code from a DirectDraw operation.
	);
	//
	// Examines the error code, and restores the surface if needed.
	//
	// Returns:
	//		True if i_dderr was an error code, and caller should try the operation
	//		again.  False if there was no error, and caller can proceed.
	//		Also terminates the program if i_dderr is an error code other than DDERR_SURFACELOST, 
	//		or if it cannot restore the surface.
	//
	// Notes:
	//		DirectDraw video surfaces can be lost if, for example, the user switches to another 
	//		application.  Every time a routine attempts a Direct Draw operation on a
	//		surface, it should loop and call bRestore while restoring is needed.
	//
	// Example:
	//		// I'm trying to Lock the surface.
	//		while (bRestore(pddsDraw->Lock(0, &sd, 0, 0)))
	//			;
	//
	// Cross references:
	//		Called by all member functions which access the surface; e.g. Lock(), Blit().
	//
	//**********************************
};


rptr<CRaster> prasReadBMP
(
	const char* str_bitmap_name,		// BMP file to construct raster from.
	bool b_vid = false					// Whether to make a CRasterVid; otherwise, CRasterMem.
);

//**********************************************************************************************
//
class CRasterWin: public CRasterVid 
//
// Prefix: rasw
//
// A descendent of CRasterVid that adds functionality by being attached to an actual Windows 
// window.  See, first you create an HWND in the standard fashion.  Then, you construct a 
// CRasterWin with the HWND as an argument.  See constructor below.
//
// You can also specify fullscreen mode, and the display mode you want.  And, you can specify 
// double buffering.  In window mode, the back buffer is offscreen, and gets blitted to the 
// front buffer.  In fullscreen mode, both buffers are in video mem, and get page-flipped.
//
//**************************************
{
public:
	bool   bFullScreen;		// True if in full-screen mode, else a window.
	int    iBuffers;		// The number of buffers in the raster.
	float  fAspectRatio;	// Ratio of pixel x size to y size.
	uint32 u4DDSFlagsFront;
	int    iWidthFront;		// True surface dimensions. May be different than the reported
	int    iHeightFront;	// surface.
	bool   bFlippable;		// The double-buffered surface can page flip.
	int    iOffsetX;		// Offset in X.
	int    iOffsetY;		// Offset in Y.
	int    iOffsetPointer;	// Surface offset in bytes.

protected:
	//
	// Additional DirectDraw structures needed for implementation.
	//
	CCom<IDirectDrawSurface4>	pddsPrimary4;	// The front surface, if double-buffered.
	CCom<IDirectDrawClipper>	pddclip;		// Clipping object if windowed.

	CCom<IDirectDrawSurface4>	pddsZBuffer;	// The z buffer if D3D is used.

	// Gamma control info.
	bool bGammaAvailable;		// Gamma control available.
	bool bGammaFlash;			// Gamma flash in progress.
	struct
	{
		WORD red[256];
		WORD green[256];
		WORD blue[256];
	} grSaved;					// Saved gamma settings.
public:

	//******************************************************************************************
	//
	// Constructor.
	//

	//******************************************************************************************
	//
	CRasterWin
	(
		HWND hwnd,							// A previously constructed Windows HWND.
		int i_width, int i_height,			// The desired dimensions of the raster.
		int i_bits = 0,						// How many bits deep for full-screen, 0 for windowed.
		int i_buffers = 1,					// How many buffers to construct.
		CSet<ERasterFlag> seteras = set0	// Any special flags for the raster.
											// Note: only the erasVideoMem flag has effect.
	);
	//
	// Construct a displayed raster surface, attached to the HWND.
	//
	// Notes:
	//		If i_bits is 0, the window remains on the current Windows screen.
	//		i_width and i_height are assumed to be the current size of the hwnd.  The hwnd is not
	//		resized.
	//
	//		If i_bits > 0, change to fullscreen mode.  The display mode is set to 
	//		i_width/i_height/i_bits.  hwnd still functions as an input receiver for the screen.
	//
	//		In either case, if i_buffers is 2, create an extra surface for double-buffering.
	//		All drawing operations take place in the back buffer.  The Flip() method blits (in
	//		windowed mode) or flips (in screen mode) the back buffer to the front.
	//
	//		NB: You should ignore WM_SIZE messages to hwnd during this constructor.
	//
	//**********************************

	//******************************************************************************************
	//
	// Overrides.
	//

	~CRasterWin();

	bool bRestore(int i_dderr);

	bool bRestoreLostSurfaces();

	void AttachPalette(CPal* ppal, CPixelFormat* ppxf = 0);

	//******************************************************************************************
	//
	// Member functions.
	//

	//
	// Functions typically called by the Windows-handling shell.
	//

	//******************************************************************************************
	//
	void Flip();
	//
	// When double-buffered, blit or flip the back surface to front.
	//
	//**********************************

	//******************************************************************************************
	//
	bool bHasGamma
	(
	)
	//
	// Returns true if gamma flash is available.
	//
	//**********************************
	{
		return bGammaAvailable;
	}
 
	//******************************************************************************************
	//
	void GammaFlash
	(
		float fAmount		// Percentage of maximum grayness to use.
	);
	//
	// Change the gamma to gray everything out for a frame.
	//
	//**********************************

	//******************************************************************************************
	//
	void RestoreGamma
	(
	);
	//
	// Restore original gamma settings.
	//
	//**********************************

	//******************************************************************************************
	//
	void HandleActivate
	(
		bool b_active					// Whether window is now active.
	);
	//
	// Handle window being activated or deactivated (WM_ACTIVATE message).
	//
	//**********************************

	//******************************************************************************************
	//
	void FlipToGDISurface();
	//
	// Called by shell before drawing the menu when in fullscreen mode.
	//
	//**********************************

	//******************************************************************************************
	//
    IDirectDrawSurface4 * GetPrimarySurface()
    {
        return pddsPrimary4;
    }
	//
	// Returns a pointer to the primary surface.  This is necessary for Videos.
	//
	//**********************************

	//******************************************************************************************
	//
    IDirectDrawSurface4 * GetPrimarySurface4()
    {
        return pddsPrimary4;
    }
	//
	// Returns a pointer to the primary surface.  This is necessary for Videos.
	//
	//**********************************


	//******************************************************************************************
	//
	void ClearBorder(bool bBackBuffer = false);
	//
	// Called by shell before drawing the menu when in fullscreen mode.
	//
	//**********************************


	//******************************************************************************************
	//
	void GetDDSDescFront
	(
		char str_dest[]
	);
	//
	// Fills str_dest with a description of the front DD Surface flags.
	//
	//**********************************

	//******************************************************************************************
	//
	void SetViewportDim
	(
		int  i_width,	// New width of viewport.
		int  i_height,	// New height of viewport.
		HWND hwnd = 0	// Optional window for windowed mode.
	);
	//
	// Sets the viewport dimensions.
	//
	//**********************************
	
	//******************************************************************************************
	virtual void Lock();
	
	//******************************************************************************************
	//
	virtual void LockAll
	(
	);
	//
	// Use to lock the surface and return a pointer without an offset.
	//
	//**********************************
	
	//******************************************************************************************
	//
	virtual void ClearSubRect
	(
		TPixel pix = 0
	);
	//
	// Clears only the used portion of the rectangle.
	//
	//**********************************

protected:

	CRasterWin()
	{
	}

private:

	class CPriv;
	friend class CPriv;

};

#endif
