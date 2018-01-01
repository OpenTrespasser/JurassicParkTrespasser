/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1996
 *
 * Contents:
 * 
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/Export.hpp                                            $
 * 
 * 6     7/15/97 6:58p Gstull
 * Made organizational changes to make management of the exporter more reasonable.
 * 
 * 5     7/14/97 3:36p Gstull
 * Made substantial changes to support tasks for the August 18th milestone for attributes,
 * object representation, geometry checking, dialog interfacing, logfile support, string tables
 * and string support.
 * 
 * 4     12/16/96 11:22a Gstull
 * Made changes to support quantization to a single palette.
 * 
 * 3     11/15/96 7:29p Gstull
 * File changes to create initial version of the multi-section groff loader.
 * 
 * 2     11/15/96 11:08a Gstull
 * Added substantial changes to the GroffExporter including for support of mutiple section
 * files.
 * 
 * 1     11/14/96 1:46p Gstull
 *
 *********************************************************************************************/

#ifndef HEADER_TOOLS_GROFFEXP_EXPORT_HPP
#define HEADER_TOOLS_GROFFEXP_EXPORT_HPP

#include "StandardTypes.hpp"
#include "ObjectDef.hpp"

#include "Lib/Sys/SysLog.hpp"
#include "Lib/Groff/FileIO.hpp"
#include "Lib/Groff/Groff.hpp"


//**********************************************************************************************
//
class CGroffExport
//
// Prefix: ge
//
{
protected:
	CFileIO fioFile;
	CSysLog slLogfile;
	char	str_message[256];


	//******************************************************************************************
	//
	bool bWriteToSection
	(
		CObjectDef*	   pod_node, 
		TSectionHandle seh_handle, 
		void*		   data, 
		uint		   u_size, 
		const char*	   str_error_message
	);


	//******************************************************************************************
	//
	bool bSaveMaterial
	(
		CObjectDef*		pod_node,
		TSectionHandle& seh_handle
	);


	//******************************************************************************************
	//
	bool bSaveMapping
	(
		CObjectDef*		pod_node,
		TSectionHandle& seh_handle
	);


	//******************************************************************************************
	//
	bool bSaveGeometry
	(
		CObjectDef*		pod_node,
		TSectionHandle& seh_handle
	);


	//******************************************************************************************
	//
	bool bSaveObject
	(
		CObjectDef*		pod_node, 
		TSectionHandle& seh_handle,
		TSymbolHandle&	syh_name
	);


	//******************************************************************************************
	//
	bool bSaveRegion
	(
		CObjectDefList*	podl_scene,
		TSectionHandle&	seh_region
	);


	//******************************************************************************************
	//
	bool bSaveHeader
	(
	);


public:
	
	//******************************************************************************************
	//
	bool bSaveScene
	(
		CObjectDefList*	podl_scene, 
		const char*		str_export_filename
	);
};

#endif