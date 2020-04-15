/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Polygon drawing module.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/PolyData.hpp                                                     $
 * 
 * 7     96/10/04 18:07 Speter
 * Removed include of defunct ObjDef*D.hpp files.
 * 
 * 6     96/07/18 17:21 Speter
 * Changed SRenderCoord to SRenderVertex.
 * 
 * 5     96/07/03 13:02 Speter
 * Moved several files to new directories, and changed corresponding include statements.
 * 
 * 4     5/29/96 4:36p Pkeet
 * Initial implementation.
 * 
 * 3     5/23/96 9:30a Pkeet
 * 
 * 2     5/17/96 10:22a Pkeet
 * 
 * 1     5/10/96 1:52p Pkeet
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_POLYDATA_HPP
#define HEADER_TEST_POLYDATA_HPP

#include "Lib/Renderer/ScreenRender.hpp"
#include "Lib/Sys/TextOut.hpp"

extern SRenderVertex arvTestVertices[];

extern int aiPoly0[]; 
extern int aiPoly1[]; 
extern int aiPoly2[]; 
extern int aiPoly3[]; 
extern int aiPoly4[]; 

void RollTestVertices();

#endif
