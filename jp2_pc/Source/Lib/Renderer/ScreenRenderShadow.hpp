/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ScreenRenderShadow.hpp                                   $
 * 
 * 1     97-03-28 17:14 Speter
 * Rasteriser specialised/simplified for shadow buffering.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_SCREENRENDERSHADOW_HPP
#define HEADER_LIB_RENDERER_SCREENRENDERSHADOW_HPP

#include "ScreenRender.hpp"

//******************************************************************************************
//
CScreenRender* pscrenShadow
(
	CScreenRender::SSettings* pscrenset,	// Render settings to use.
	rptr<CRaster> pras_screen				// The screen to render to.
);
//
// Returns:
//		A new ScreenRender object usable for shadow buffering.
//
//**************************************

#endif