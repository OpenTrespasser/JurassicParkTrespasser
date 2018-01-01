/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Functions for loading texture into meshes.
 *
 * Bugs:
 *
 * To do:
 *		Collect these into a class, or make them part of another class.
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/lib/loader/LoadTexture.hpp                                           $
 * 
 * 16    9/10/98 12:08a Pkeet
 * Added the 'EraseTextureMap' function.
 * 
 * 15    9/09/98 11:59a Pkeet
 * Made texturing tracking work.
 * 
 * 14    8/29/98 10:39p Pkeet
 * Added the 'BreakAllTextureLinks' function.
 * 
 * 13    8/20/98 5:38p Mmouni
 * Changes for additional clut control text props.
 * 
 * 12    98.06.12 3:32p Mmouni
 * Added stuff to support CLUT start and stop colors on a per quantization group basis.
 * 
 * 11    98/02/26 14:02 Speter
 * Removed obsolete InstanceData.hpp, and bLoadTexture functions.
 * 
 * 10    97/10/12 22:15 Speter
 * ptexGetTexture now takes CMaterial* parameter.  SSurface() doesn't.
 * 
 * 9     9/01/97 7:41p Rwyatt
 * 
 * 8     97/06/14 0:12 Speter
 * Removed bLoadTextures function (incorporated into pmshLoadObject).  Made ptexGetTexture
 * public.
 * 
 * 7     97/02/27 21:20 Speter
 * Changed hack structure and loading routines to include render-flag settings, with new
 * SSurfaceData structure.  
 * 
 * 6     2/25/97 11:16a Gstull
 * Somehow, source safe screwed up my checkin, so I rechecked in the file.  Weird
 * 
 * 5     2/25/97 11:12a Gstull
 * Added changes to support new GROFF file format, vertex normals and better error reporting.
 * 
 * 4     97/02/13 13:37 Speter
 * Use new bumpiness attribute from SInstanceData.
 * 
 * 3     97/01/07 12:07 Speter
 * Made all CRenderTypes use rptr<>.
 * Simplified usage of instancer map.
 * 
 * 2     96/12/12 13:28 Speter
 * Removed matDefault variable.  Now default material is just matDEFAULT.
 * 
 * 1     96/12/12 12:20 Speter
 * New module containing code moved from PredefineShapes.cpp.
 * 
 *
 ***********************************************************************************************/

#ifndef HEADER_LIB_LOADER_LOADTEXTURE_HPP
#define HEADER_LIB_LOADER_LOADTEXTURE_HPP

#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Renderer/Primitives/FastBump.hpp"

//*********************************************************************************************
//
// Global functions.
//

//
// Functions for operating on pre-defined shapes.
//

//*********************************************************************************************
//
void LoadColour
(
	rptr<CMesh> pmsh,							// Mesh to associate colour with.
	CColour    clr = CColour(255, 255, 255),	// Colour to associate with mesh.
	const CMaterial* pmat = &matDEFAULT			// Material to associate with mesh.
);
//
// Associates a colour with a mesh.
//
//**************************************

//*****************************************************************************************
//
rptr<CTexture> ptexGetTexture
(
	const char* str_texture_name,		// Name of texture to read or get.
	const char* str_bumpmap_file,		// Bumpmap file name.
	float f_bumpiness,					// Bumpiness to apply if a bump map.
	const CMaterial* pmat,				// Material used by texture.
	const SPalClutControl& pccSettings	// Clut start/stop and reflection settings.
);
//
// Returns:
//		The texture, previously read or read from disk.
//
// Notes:
//		Makes use of the mapTextures map to instance the textures.
//
//**************************************

//*****************************************************************************************
//
void DumpTextureLogFile
(
);
//
// Writes the names of the textures in 'mapTextures' to the "TextureLog.txt" text file.
//
//**************************************

//*****************************************************************************************
//
void BreakAllTextureLinks
(
);
//
// Breaks all linked rasters for loaded textures.
//
//**************************************

//*****************************************************************************************
//
void DumpTextureUse
(
);
//
// Dumps texture use information to a log file.
//
//**************************************

//*********************************************************************************************
//
void EraseTextureMap
(
);
//
// Erases the STL map of reference counted texture pointers.
//
//**************************************


#endif
