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
 * $Log:: /JP2_PC/Source/Lib/Groff/GroffLoader.hpp                                            $
 * 
 * 5     8/27/98 1:33p Shernd
 * Improved callback updates
 * 
 * 4     10/22/97 1:34a Gstull
 * Changes to support version 12 of the GROFF file structure definitions.  This mainly includes
 * support for text attributes for use with the attribute compiler.
 * 
 * 3     11/15/96 7:29p Gstull
 * File changes to create initial version of the multi-section groff loader.
 * 
 * 2     11/15/96 11:09a Gstull
 * Changes to support multiple object files.
 * 
 * 1     11/13/96 5:36p Gstull
 *
 *********************************************************************************************/

#ifndef HEADER_LIB_GROFF_GROFFLOADER_HPP
#define HEADER_LIB_GROFF_GROFFLOADER_HPP


#include "Lib/Sys/SysLog.hpp"
#include "Lib/Groff/FileIO.hpp"
#include "Lib/Groff/Groff.hpp"
#include "Lib/Groff/ValueTable.hpp"


#ifndef PFNWORLDLOADNOTIFY
typedef uint32 (__stdcall * PFNWORLDLOADNOTIFY)(uint32 dwContext, uint32 dwParam1, uint32 dwParam2, uint32 dwParam3);
#endif

class CGroffLoader
{
protected:
	CFileIO	 fioFile;
	CSysLog	 slLogfile;
	char	 str_message[256];

	bool bReadFromSection(TSectionHandle seh_handle, 
                          void* data, 
                          uint u_size, 
                          const char* str_error_message);

public:
		 CGroffLoader();
		 ~CGroffLoader();

	bool bLoadBitmap(TSectionHandle seh_handle, CGroffBitmap& gbm_bitmap);
	bool bLoadMaterial(TSectionHandle seh_handle, CGroffMaterial& gmat_material);
	bool bLoadMapping(TSectionHandle seh_handle, CGroffMapping& gmap_mapping);
	bool bLoadGeometry(TSectionHandle seh_handle, CGroffGeometry& gg_geometry);
	bool bLoadObject(TSectionHandle seh_handle, CGroffObject& go_object);
	bool bLoadAttributes(TSectionHandle seh_attributes, CValueTable& vtab_value_table);
	bool bLoadRegion(TSectionHandle seh_region, CGroffRegion& gr_region);
	bool bLoadHeader(uint& u_version_number);

    PFNWORLDLOADNOTIFY      pfnWorldNotify;
    uint32                  ui32_NotifyParam;
};

#endif