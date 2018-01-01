/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		An auxilary D3D renderer to supplement CScreenRenderDWI.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/Renderer/ScreenRenderAuxD3D.hpp                                   $
 * 
 * 34    9/22/98 10:40p Pkeet
 * Forced inline critical Direct3D functions.
 * 
 * 33    9/01/98 12:45a Pkeet
 * Removed the target screen passed as a parameter.
 * 
 * 32    8/31/98 2:58p Pkeet
 * Added the 'FlushBatch' call.
 * 
 * 31    8/19/98 12:37a Pkeet
 * Fixed bug that caused a crash when changing screen resolutions.
 * 
 * 30    8/14/98 1:28p Pkeet
 * Added the 'RenderDummy' member function.
 * 
 * 29    8/10/98 11:28p Pkeet
 * Added the base offsets.
 * 
 * 28    8/07/98 11:46a Pkeet
 * Added flags for tracking whether the hardware is busy or not. Made the 'SetD3DModePriv'
 * member function private.
 * 
 * 27    8/05/98 3:14p Pkeet
 * Added the 'fZBiasPerPrimitive' data member.
 * 
 * 26    8/02/98 3:11p Pkeet
 * Made the offsets and the 'SetD3DModePriv' member function public.
 * 
 * 25    7/28/98 4:25p Pkeet
 * Reduced the default for disabling scheduled texture uploads.
 * 
 * 24    7/27/98 8:44p Pkeet
 * Added code for scheduling uploads.
 * 
 * 23    7/23/98 6:23p Pkeet
 * Added more hardware polygon types.
 * 
 * 22    7/20/98 10:27p Pkeet
 * Upgraded to Direct3D 4's interface. Moved state cache to a separate module.
 * 
 * 21    7/13/98 12:07a Pkeet
 * Added an offset to allow Direct3D to place its polygons in the right place when the size of
 * the viewport is changed.
 * 
 * 20    7/10/98 7:26p Pkeet
 * Changed fog to use alpha test only.
 * 
 * 19    6/21/98 8:03p Pkeet
 * Added the 'SetD3DFlagForPolygon' member function.
 * 
 * 18    6/14/98 2:46p Pkeet
 * Added support for fogging render caches using hardware.
 * 
 * 17    6/09/98 3:52p Pkeet
 * Replaced per polygon tests for hardware polygon type with a batch.
 * 
 * 16    6/08/98 8:08p Pkeet
 * Modified the 'SetD3DFlagForPolygons' parameters to work in the pipeline.
 * 
 * 15    6/02/98 11:12a Pkeet
 * Added the 'SetD3DFlagForPolygons' member function.
 * 
 * 14    4/01/98 5:41p Pkeet
 * Converted functions to 'priv_self' format. Added support for rendering image caches in
 * hardware.
 * 
 * 13    3/04/98 1:26p Pkeet
 * Added the 'AssertEndScene' member function.
 * 
 * 12    1/26/98 3:50p Pkeet
 * D3D renderer only renders flat-filled polygons if they originate from the terrain.
 * 
 * 11    1/26/98 11:50a Pkeet
 * Auxilary renderer now tracks perspective correction state.
 * 
 * 10    1/21/98 1:27p Pkeet
 * Added an assert to ensure clipping polygons are not also terrain polygons.
 * 
 * 9     1/15/98 5:40p Pkeet
 * Added in PowerVR support for fog.
 * 
 * 8     1/13/98 8:32p Pkeet
 * Set 'bDrawPolygon' to inline except if drawing Direct3D primitives.
 * 
 * 7     1/13/98 7:44p Pkeet
 * The screen raster pointer is now stored instead of being passed by each polygon.
 * 
 * 6     1/12/98 11:44a Pkeet
 * Added in the software lock request for the auxilary renderer.
 * 
 * 5     1/07/98 11:25a Pkeet
 * Added fogging for terrain.
 * 
 * 4     12/23/97 3:00p Pkeet
 * Added a flag for clipping split polygons.
 * 
 * 3     12/17/97 7:39p Pkeet
 * Added render state variables for reducing render state switches.
 * 
 * 2     12/15/97 4:09p Pkeet
 * Added a no texture D3D primitive. Also added separate primitives for Gouraud shading and no
 * shading.
 * 
 * 1     12/11/97 1:20p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef LIB_RENDERER_SCREENRENDERAUXD3D_HPP
#define LIB_RENDERER_SCREENRENDERAUXD3D_HPP

//
// Necessary includes.
//
//#include "Lib/W95/Direct3D.hpp"

//
// Forward declarations.
//
class CRenderPolygon;
class CRaster;


//
// Enumerations.
//

// Enumerations for rendering mode.
enum ED3DRenderMode
// Prefix: ed3dr
{
	ed3drSOFTWARE_LOCK,
	ed3drHARDWARE_LOCK
};


//
// Class definitions.
//

//**********************************************************************************************
//
class CScreenRenderAuxD3D
//
// An auxilary D3D renderer.
//
// Prefix: srd3d
//
// Notes:
//		This is not intended to be a fully-fledged renderer, rather it is intended to supplement
//		the existing software renderer.
//
//**************************************
{
private:

	// Member variables.
	ED3DRenderMode ed3drMode;			// Current rendering mode.
	bool           bOutputDisplay;		// Target is a display output.
	bool           bActive;				// Flag for using D3D or not.
	D3DCOLOR       d3dcolFog;			// Fog colour.
	int            iDisabledCount;		// Frames scheduler is disabled for.
	float          fZBiasPerPrimitive;	// Z bias value set on a per primitive basis.
	bool           bBusy;				// Flag is 'true' between begin and end scenes.

public:

	// Globals set by the system at the beginning of each frame.
	static float   fOffsetX;			// Screen offset in X.
	static float   fOffsetY;			// Screen offset in Y.

	// Globals used always.
	static float   fOffsetXBase;		// Screen offset in X, calculated when D3D is initialized.
	static float   fOffsetYBase;		// Screen offset in X, calculated when D3D is initialized.

public:

	//******************************************************************************************
	//
	// Constructor.
	//

	// Default constructor.
	CScreenRenderAuxD3D();

	// Destructor.
	~CScreenRenderAuxD3D();

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Reset
	(
	);
	//
	// Resets the entire system.
	//
	// Notes:
	//		This should prevent the big bug that caused a crash whenever the screen was resized.
	//
	//**************************************

	//******************************************************************************************
	//
	void SetOutputFlag
	(
		bool b_output_display	// Output display flag.
	)
	//
	// Sets the flag indicating if the target will be a video output raster.
	//
	//**************************************
	{
		bOutputDisplay = b_output_display;
	}

	//******************************************************************************************
	//
	bool bIsBusy
	(
	)
	//
	// Returns 'true' if the card is busy.
	//
	//**************************************
	{
		return bBusy;
	}

	//******************************************************************************************
	//
	forceinline bool bDrawPolygon
	(
		CRenderPolygon& rp			// Polygon to draw.
	)
	//
	// Polygon to render using Direct3D.
	//
	// Returns 'true' if the polygon was rendered by Direct3D.
	//
	// Notes:
	//		If 'false' is returned, no polygon was drawn and the software renderer is
	//		responsible for drawing the polygon.
	//
	//**************************************
	{
		// Return if the polygon is a software polygon.
		if (rp.ehwHardwareFlags == ehwSoftware)
			return false;

		// Use the Direct3D renderer.
		return bDrawPolygonD3D(rp);
	}

	//******************************************************************************************
	//
	bool bBeginScene
	(
	);
	//
	// Sets Direct3D up for drawing a scene.
	//
	//**************************************

	//******************************************************************************************
	//
	void EndScene
	(
	);
	//
	// Finishes Direct3D drawing for a scene.
	//
	//**************************************

	//******************************************************************************************
	//
	void FlushBatch
	(
	);
	//
	// Finishes Direct3D drawing for the most recent batch of polygons.
	//
	//**************************************

	//******************************************************************************************
	//
	void AssertEndScene
	(
	) const;
	//
	// Asserts if the D3D mode is still in the hardware renderer.
	//
	//**************************************
	
	//******************************************************************************************
	//
	forceinline void SetD3DMode
	(
		ED3DRenderMode ed3dr	// New mode.
	)
	//
	// Sets the D3D mode by toggling between software and hardware.
	//
	//**************************************
	{
		// Do nothing if the device is already in the correct mode.
		if (ed3drMode != ed3dr)
			SetD3DModePriv(ed3dr);
	}

	//******************************************************************************************
	//
	void SetD3DFlagForPolygons
	(
		CPArray<CRenderPolygon> parpoly	// Array of polygons to draw.
	);
	//
	// Sets software and hardware flag as required.
	//
	//**************************************

	//******************************************************************************************
	//
	void LoadHardwareTextures
	(
		CPArray<CRenderPolygon> paprpoly,	// Array of polygons to draw.
		bool b_target_hardware
	);
	//
	// Loads Direct3D hardware textures.
	//
	//**************************************

	//******************************************************************************************
	//
	void SetD3DFlagForPolygon
	(
		CRenderPolygon& rpoly	// Polygons to set the hardware flags for.
	);
	//
	// Sets software and hardware flag as required.
	//
	//**************************************

	//******************************************************************************************
	//
	void SetScheduler
	(
		int i_count = 2	// Number of frames to disable the scheduler for.
	)
	//
	// Enables or disables scheduled uploading of textures.
	//
	//**************************************
	{
		iDisabledCount = i_count;
	}

	//******************************************************************************************
	//
	void RenderDummy
	(
		CRasterD3D* prasd3d
	);
	//
	// Renders the texture as a dummy polygon.
	//
	// Notes:
	//		This is an attempt to overcome the "load-on-demand" behaviour of certain cards.
	//
	//**************************************

private:

	//******************************************************************************************
	void SetD3DModePriv(ED3DRenderMode ed3dr);

	//******************************************************************************************
	bool bDrawPolygonD3D(CRenderPolygon& rp);


	class CPriv;
	friend class CPriv;

};


//
// Global variables.
//

// Global instance of this renderer object.
extern CScreenRenderAuxD3D srd3dRenderer;


#endif // LIB_RENDERER_SCREENRENDERAUXD3D_HPP