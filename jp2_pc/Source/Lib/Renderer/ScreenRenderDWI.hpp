/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CRenderDescDWI
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/screenrenderdwi.hpp                                      $
 * 
 * 4     97/11/08 3:46p Pkeet
 * Removed Z template parameters and code.
 * 
 * 3     97-05-06 16:10 Speter
 * Replaced AddRenderDescDWI() with CRenderDescDWI declaration and instance.  Now explicitly
 * accessible from elsewhere.
 * 
 * 2     96/08/19 12:58 Speter
 * New interface for CRenderDesc class.
 * 
 * 1     96/08/09 11:04 Speter
 * New files replace ScreenRender.cpp.
 * Implement CScreenRender with our custom rasteriser.
 * 
 **********************************************************************************************/

#include "ScreenRender.hpp"

//**********************************************************************************************
//
class CRenderDescDWI: public CRenderDesc
//
//**************************************
{
public:

	//**********************************************************************************************
	//
	// Constructor.
	//

	//**********************************************************************************************
	CRenderDescDWI()
		: CRenderDesc("3D O'Rama")
	{
	}

	//**********************************************************************************************
	//
	// Overrides.
	//

	virtual CScreenRender* pScreenRenderCreate
	(
		CScreenRender::SSettings* pscrenset,	// Settings.
		rptr<CRaster> pras_screen				// Screen raster.
	);
};

// Global instance of this render descriptor.
extern CRenderDescDWI screndescDWI;
