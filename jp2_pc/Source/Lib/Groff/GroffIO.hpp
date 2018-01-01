/***********************************************************************************************
 *
 * Copyright (c) 1996 DreamWorks Interactive.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/GroffIO.hpp                                                 $
 * 
 * 31    98.09.07 5:25p Mmouni
 * Changed pmshLoadObject to prdtLoadObject and changed instancing to use CRenderType instead of
 * CMesh.
 * 
 * 
 * 30    8/27/98 1:33p Shernd
 * Improved callback updates
 * 
 * 29    8/25/98 9:17a Shernd
 * Added callback functionality
 * 
 * 28    98.08.21 7:25p Mmouni
 * Added bSaveOptimizedFile().
 * 
 * 27    5/14/98 8:07p Agrant
 * removed poval_renderer foolishness; no longer used
 * 
 * 26    98/02/26 14:05 Speter
 * Removed obsolete InstanceData.hpp. 
 * 
 * 25    2/10/98 1:42p Pkeet
 * Added the 'Texture' text property.
 * 
 * 24    1/26/98 11:21a Agrant
 * CVersion 11 groff files invalidated, old code removed (or commented out)
 * 
 * 23    12/10/97 10:21p Agrant
 * Detail reduction models scale fix
 * 
 * 22    12/03/97 7:54p Agrant
 * Version 12 detail reduction using text props.
 * 
 * 21    10/27/97 4:37p Agrant
 * Added a text-prop based LoadLowerMeshes function
 * 
 * 20    10/24/97 7:27p Agrant
 * Added a text prop based mesh loader to the GroffIO
 * 
 * 19    10/22/97 1:34a Gstull
 * Changes to support version 12 of the GROFF file structure definitions.  This mainly includes
 * support for text attributes for use with the attribute compiler.
 * 
 * 18    8/13/97 6:46p Agrant
 * Allows loading of animating meshes.
 * 
 * 17    97/06/14 0:22 Speter
 * Removed bLoadTextures function (incorporated into pmshLoadObject).  Removed SMaterialInfo
 * structure, temporary interface struct no longer needed.  Added LoadLowerMeshes, for detail
 * reduction.
 * 
 * 16    97/05/28 17:11 Speter
 * Now retransform all biomeshes so that their origin is Joint 0.
 * 
 * 15    97/05/27 18:09 Speter
 * Added constant for "$" literal.  Moved pr3Presence(CGroffObjectName) to .cpp file.
 * 
 * 14    97/03/24 15:13 Speter
 * Replaced deleted CRotate-from-char constructor with explicit concatenation.
 * 
 * 13    2/25/97 11:12a Gstull
 * Added changes to support new GROFF file format, vertex normals and better error reporting.
 * 
 * 12    2/24/97 7:31p Gstull
 * Added fixes to the GUIApp which result in the removal of the .BMF bump map loading hack.
 * Bump maps will not load correctly until the .BMF hack has been removed.
 * 
 * 11    97/01/07 12:06 Speter
 * Made all CRenderTypes use rptr<>.
 * 
 * 10    96/12/16 11:45 Speter
 * Put object rotation back to default, as the specified rotation is invalid.
 * 
 * 9     96/12/15 18:23 Speter
 * Added a lot of code for loading skeletal meshes in a hacky way.  Not quite working yet, but
 * nothing else broken.  Added pr3Presence(CGroffObjectName) function, and now set loaded
 * objects' orientation.
 * 
 * 8     96/12/11 16:47 Speter
 * Added b_curved parameter to pmshLoadObject.
 * 
 * 7     11/20/96 1:14p Gstull
 * Minor changes to the Groff loader to support integration into the GUIApp.
 * 
 * 6     11/15/96 11:09a Gstull
 * Changes to support multiple object files.
 * 
 * 5     96/10/25 14:40 Speter
 * Updated for new include file location.
 * 
 * 4     96/10/04 18:51 Speter
 * Changed SMesh to CMesh, like it's sposta be.
 * 
 * 3     96/10/04 18:04 Speter
 * Removed include of defunct ObjDef3D.hpp.
 * 
 * 2     9/16/96 8:18p Gstull
 * Added project relative path to the include statements.
 *
 **********************************************************************************************/


#ifndef HEADER_COMMON_GROFFIO_HPP
#define HEADER_COMMON_GROFFIO_HPP

#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/Sys/SysLog.hpp"
#include "Lib/Sys/SmartBuffer.hpp"
#include "Lib/Groff/FileIO.hpp"
#include "Lib/Groff/Groff.hpp"
#include "Lib/Groff/GroffLoader.hpp"

#define strSUBOBJECT_PREFIX	"$"
#define strJOINT_PREFIX		"$J"
#define strMESH_PREFIX		"$M"
#define strLEVEL_PREFIX		"$L"


#ifndef PFNWORLDLOADNOTIFY
typedef uint32 (__stdcall * PFNWORLDLOADNOTIFY)(uint32 dwContext, uint32 dwParam1, uint32 dwParam2, uint32 dwParam3);
#endif


//*********************************************************************************************
//
class CGroffIO : public CGroffLoader
//
// Prefix: gl
//
// This class is intended to be used as an I/O module for the various datasets which form the
// environment of the Jurassic Park II PC game.  Included in this class are all the necessary
// member functions for loading/saving a GROFF file to/from disk as well as accessing specific
// sections within the file.
//
// Example:
//
//*********************************************************************************************
{
public:
	CGroffIO
	(
	);
	
	~CGroffIO
	(
	);

	bool bOpenFile
	(
		const char* str_filename
	);

	bool bObjectInfo
	(
		CGroffObjectInfo& goi_object_info,
		CValueTable&	  vtab_value_table
	);

	bool bSaveOptimizedFile
	(
		const char *str_filename
	);
	
	bool bCloseFile
	(
	);

	/*
	rptr<CMesh> pmshLoadObject
	(
		const CGroffObjectInfo& goi,
		const CGroffObjectName& gon, 
		CPresence3<>* ppr3_adjust,
		const SInstanceData& insd,
		bool b_textures,
		bool b_animating = false
	);
	*/

	rptr<CRenderType> prdtLoadObject
	(
		const CGroffObjectName*	pgon,				// Pointer to GROFF name.
		CLoadWorld*				pload,				// Pointer to loader.
		const CHandle&			h_obj,				// Handle to the base object in the value table.
		CValueTable*			pvtable				// Pointer to the value table.
	);

protected:

	/*
	void LoadLowerMeshes
	(
		rptr<CMesh> pmsh, 
		const CGroffObjectInfo& goi, 
		const char* str_object_name, 
		const SInstanceData& insd, 
		bool b_textures
	);
	*/

	void LoadLowerMeshes
	(
		rptr<CMesh> pmsh, 
		const CGroffObjectName* pgon,
		CLoadWorld*	pload,
		CValueTable*	pvt,
		TReal			r_instance_scale
	);

	int iNumJoints
	(
		const CGroffObjectInfo& goi,
		const CGroffObjectName& gon
	);

};

CPresence3<> pr3Presence
(
	const CGroffObjectName& gon
);

#endif