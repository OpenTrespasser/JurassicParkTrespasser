/***********************************************************************************************
 *
 * Copyright (c) 1996 DreamWorks Interactive, 1996-1997
 *
 * Contents: Class for performing validation of object characteristic information including 
 * the geometry, face definitions, normals and texture coordinates.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/Geometry.hpp                                           $
 * 
 * 5     7/15/97 6:58p Gstull
 * Made organizational changes to make management of the exporter more reasonable.
 * 
 * 4     7/14/97 3:36p Gstull
 * Made substantial changes to support tasks for the August 18th milestone for attributes,
 * object representation, geometry checking, dialog interfacing, logfile support, string tables
 * and string support.
 * 
 * 3     11/15/96 11:08a Gstull
 * Added substantial changes to the GroffExporter including for support of mutiple section
 * files.
 * 
 * 2     11/06/96 7:29p Gstull
 * Added new location for the logfiles.
 * 
 * 1     9/16/96 3:53p Gstull
 *
 **********************************************************************************************/

#ifndef HEADER_TOOLS_GROFFEXP_GEOMETRY_HPP
#define HEADER_TOOLS_GROFFEXP_GEOMETRY_HPP

#include "StandardTypes.hpp"
#include "ObjectDef.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "Lib/Sys/SysLog.hpp"
#include "Lib/Groff/Groff.hpp"


//**********************************************************************************************
// Class which performs various checks on the structural components, and tranforms the object
// into the viewing cube (i.e. [-1.0, 1.0] in R3) prior to exporting the object.
//
class CGeometry
//
// Prefix: geo 
// 
{
private:

	CEasyString	estrFilename;		// Filename of output logfile.

	CSysLog		slLogfile;			// Geometry logfile name.

	int			iDegenerateFaces;	// Degenerate polygons per object.
	int			iRemovedVertices;	// Vertices removed per object.
	int			iClampUVCoords;		// UV Texture coords clamped.
	int			iClampMaterialIDs;	// Material ID's clamped in a single material object.


	//*****************************************************************************************
	// Function to reset the statistics.
	void ResetStatistics
	(
	);
	// 
	//*****************************************************************************************
	

	//*****************************************************************************************
	// Function to display the statistics.
	//
	void PrintStatistics
	(
	);
	//
	//*****************************************************************************************


	//*****************************************************************************************
	// Function to validate the objects geometry.
	//
	bool bCheckGeometry
	( 
		CObjectDef* pod_object
	);
	//
	// Returns a boolean which indicates if the geometry is valid.
	// 
	//*****************************************************************************************


	//*****************************************************************************************
	// FUnction to validate the face definitions.
	//
	bool bCheckFaces
	( 
		CObjectDef* pod_object
	);
	//
	// Returns a boolean which indicates if the face definition is valid.
	// 
	//*****************************************************************************************
	

	//*****************************************************************************************
	// Function to validate the texture coordinates.
	//
	bool bCheckTexCoords
	( 
		CObjectDef* pod_object
	);
	//
	// Returns a boolean which indicates if the texture coordinate definition is valid.
	// 
	//*****************************************************************************************
	

	//*****************************************************************************************
	// Function to validate the material definition.
	//
	bool bCheckMaterials
	( 
		CObjectDef* pod_object
	);
	//
	// Returns a boolean which indicates if the material definition is valid.
	// 
	//*****************************************************************************************


public:

	//*****************************************************************************************
	// Constructor.
	//
	CGeometry
	(
	) 
	{
	}
	//
	//*****************************************************************************************


	//*****************************************************************************************
	// Destructor.
	//
	~CGeometry
	(
	)
	{
	}
	//
	//*****************************************************************************************


	//*****************************************************************************************
	// Take a scene description and prepare each of the objects in the scene for exporting.
	//
	bool bCheckScene
	(
		CObjectDefList*	   podl_scene,				// The scene list.
		const CEasyString& estr_export_filename		// The name of the export file.
	);
	//
	// Returns a boolean which indicates if the entire scene was successfully processed.
	//
	//*****************************************************************************************
};

#endif