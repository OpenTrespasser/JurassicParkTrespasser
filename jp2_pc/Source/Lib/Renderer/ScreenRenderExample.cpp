/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Example file for CScreenRender driver.  Change the word "Example" to a descriptive term for
 * your driver, and fill in the appropriate function bodies.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ScreenRenderExample.cpp                                  $
 * 
 * 4     97-04-14 20:37 Speter
 * Updated for new settings conventions.
 * 
 * 3     97/01/16 12:06 Speter
 * Updated for changes in CScreenRender.
 * 
 * 2     97/01/10 17:39 Speter
 * Fixed some syntax errors.
 * 
 * 1     97/01/09 14:37 Speter
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "ScreenRender.hpp"

//**********************************************************************************************
//
class CScreenRenderExample: public CScreenRender
//
// Prefix: screndex
//
// Implementation of CScreenRender interface for Example driver.
//
//**************************************
{
private:

	//
	// Any data needed for the driver should go right here.
	//

public:

	//******************************************************************************************
	CScreenRenderExample(SSettings* pscrenset, rptr<CRaster> pras_screen, rptr<CRaster> pras_zbuffer, ...)
		: CScreenRender(pscrenset, pras_screen, pras_zbuffer)
	{
		//
		// A constructor for the driver, taking the CRasterVid to output to, and any additional
		// arguments needed.  This is called when the program decides to render using this driver
		// (for example, the user selects it from the menu).
		//
		// If this driver outputs to a custom surface, raather than prasv_screen, 
		// then just ignore this argument, but still copy it to the CScreenRender structure, as
		// shown.  This lets other parts of the program write to that raster, even if the renderer
		// doesn't.
		//

		// Perform any driver initialisation here.

	}

	~CScreenRenderExample()
	{
		// Any destruction code needed.
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual CSet<ERenderFeature> seterfModify()
	{
		// Return which features can be toggled.
		static const CSet<ERenderFeature> seterf_modify = Set
			 (erfZ_BUFFER)
			+ erfZ_WRITE
			+ erfZ_TEST

			+ erfCOPY
			+ erfLIGHT_SHADE
			+ erfFOG_SHADE

			+ erfTEXTURE
			+ erfTRANSPARENT
			+ erfBUMP

			+ erfSUBPIXEL
			+ erfPERSPECTIVE
			+ erfDITHER
			+ erfFILTER
			+ erfFILTER_EDGES

			+ erfRASTER_CLIP
			+ erfRASTER_CULL

			+ erfLIGHT
			+ erfFOG
			+ erfSPECULAR
			+ erfCOLOURED_LIGHTS;

		return seterf_modify;
	}

	//******************************************************************************************
	virtual CSet<ERenderFeature> seterfDefault()
	{
		// Return which features are on by default.
		static const CSet<ERenderFeature> seterf_default = Set
			 (erfZ_BUFFER)
			+ erfZ_WRITE
			+ erfZ_TEST

			+ erfCOPY
			+ erfLIGHT_SHADE
			+ erfFOG_SHADE

			+ erfTEXTURE
			+ erfTRANSPARENT
			+ erfBUMP

			+ erfSUBPIXEL
			+ erfPERSPECTIVE
			+ erfDITHER
			+ erfFILTER
			+ erfFILTER_EDGES

			+ erfRASTER_CLIP
			+ erfRASTER_CULL

			+ erfLIGHT
			+ erfFOG
			+ erfSPECULAR
			+ erfCOLOURED_LIGHTS;

		return seterf_default;
	}

	//******************************************************************************************
	virtual void BeginFrame()
	{
		// Call this function to ensure that the render state set by the application is valid.
		CorrectRenderState(pSettings->seterfState);

		if (pSettings->seterfState[erfZ_BUFFER])
		{
			// Clear the Z buffer if needed.
		}

		if (pSettings->bClearBackground)
		{
			// Clear the screen if told to by the app.
		}

		// Other pre-rendering setup.
	}

	//******************************************************************************************
	virtual void EndFrame()
	{
		// Any post-rendering cleanup needed.
	}

	//******************************************************************************************
	virtual void DrawPolygons
	(
		CPArray<CRenderPolygon> parpoly
	)
	{
		// Here is the main render code.
	}
};

//**********************************************************************************************
//
class CRenderDescExample: public CRenderDesc
//
// This class describes to the app how to create a particular renderer driver.  There may be
// more than one driver for a given renderer; for example the D3D renderer provides Ramp, RGB,
// and HAL drivers.
//
//**************************************
{
private:

	// If the renderer supports more than one driver, then particular driver information 
	// may be stored here.

public:

	//**********************************************************************************************
	//
	// Constructor.
	//

	CRenderDescExample(...)
		// Initialise the CRenderDesc with the string describing the driver on the menu.
		: CRenderDesc("Example")
	{
		// Set up any particular driver information.
	}

	virtual CScreenRender* pScreenRenderCreate(SSettings* pscrenset, rptr<CRaster> pras_screen, rptr<CRaster> pras_zbuffer)
	{
		//
		// This function is called by the app to actually create the driver.
		// Just call the driver constructor here, passing the prasv_screen arg, and any other
		// args needed for the particular driver.
		//
		return new CScreenRenderExample(pscrenset, pras_screen, pras_zbuffer);
	}
};


//**********************************************************************************************
//
void AddRenderDescExample()
//
// This function is called by the app to add all the drivers supported by this renderer to the
// driver list.
//
//**************************************
{
	// Add all the drivers.  If there is only one driver for this renderer, you don't need the loop.
	// for (...)
	{
		sapRenderDesc << new CRenderDescExample(/*...*/);
	}
}

