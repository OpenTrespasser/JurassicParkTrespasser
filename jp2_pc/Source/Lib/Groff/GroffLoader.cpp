/***********************************************************************************************
 * Copyright (c) DreamWorks Interactive 1996
 *
 ***********************************************************************************************
 *
 * $$Log:: /JP2_PC/Source/Lib/Groff/GroffLoader.cpp                                            $
 * 
 * 4     11/21/96 3:02p Gstull
 * Remove default Groff loader section info.
 * 
 * 3     11/15/96 7:29p Gstull
 * File changes to create initial version of the multi-section groff loader.
 * 
 * 2     11/15/96 11:09a Gstull
 * Changes to support multiple object files.
 * 
 **********************************************************************************************/

#include "Lib/Groff/GroffLoader.hpp"
#include "Lib/Sys/SysLog.hpp"
#include "Lib/Groff/FileIO.hpp"
#include "Lib/Groff/Groff.hpp"


// Global variable for the current GROFF file version number.
uint g_u_Version_Number = 0;

// Set the true if the groff has optimized geometry.
bool bOptimizedGROFF = false;


CGroffLoader::CGroffLoader()
{
    pfnWorldNotify = NULL;
    ui32_NotifyParam = 0;
}


CGroffLoader::~CGroffLoader()
{
}


bool CGroffLoader::bReadFromSection(TSectionHandle sh_handle, 
                                    void* data, 
                                    uint u_size, 
                                    const char* str_error_message)
{
    uint        u_remain = u_size;
    bool        b_error = false;
    uint        u_chunk;
    int         i_res;

    while (u_remain && !b_error)
    {
        if (u_remain < 4096)
        {
            u_chunk = u_remain;
        }
        else
        {
            u_chunk = 4096;
        }

        i_res = fioFile.iRead(sh_handle, data, u_chunk);

        if (i_res != (int)u_chunk)
        {
            b_error = true;
#ifdef DEBUG_MODE
		    // Something went wrong so report the error and return an error.
		    slLogfile.Msg(str_error_message);
#endif
        }
        else
        {
            u_remain -= (uint)i_res;
            data = (char*) data + i_res;

			/*
			 * This is unnecessary.
			 *
            if (pfnWorldNotify)
            {
                (pfnWorldNotify)(ui32_NotifyParam, 0, 0, 0);
            }
			*/
        }
    }

    return !b_error;
}


bool CGroffLoader::bLoadBitmap(TSectionHandle sh_handle, CGroffBitmap& gmap_bitmap)
{
	return false;
}


bool CGroffLoader::bLoadMaterial(TSectionHandle seh_handle, CGroffMaterial& gmat_material)
{
	// Attempt to read the material count.
	if (!bReadFromSection(seh_handle, &gmat_material.uTextureCount, sizeof(uint),
		"bLoadMaterial: Unable to read the material count."))
		return false;

	// Attempt to configure the texture to read the material handles.  Were we successful?
	if (!gmat_material.bConfigure())
	{
#ifdef DEBUG_MODE
		// No! Report the error and return an error result.
		slLogfile.Msg("bLoadMaterial: Unable to configure a material object.");
#endif
		// No!  Return an error.
		return false;
	}

	// Now we should be able to load the rest of the material definition.
	if (!bReadFromSection(seh_handle, gmat_material.asyhTextureHandle, sizeof(uint) * gmat_material.uTextureCount,
		"bLoadMaterial: Unable to read the texture map handle list."))
		return false;

	if (!bReadFromSection(seh_handle, gmat_material.asyhOpacityHandle, sizeof(uint) * gmat_material.uTextureCount,
		"bLoadMaterial: Unable to read the opacity map handle list."))
		return false;

	if (!bReadFromSection(seh_handle, gmat_material.asyhBumpHandle, sizeof(uint) * gmat_material.uTextureCount,
		"bLoadMaterial: Unable to read the bumpmap handle list."))
		return false;

	if (15 <= g_u_Version_Number)
	{
		if (!bReadFromSection(seh_handle, gmat_material.auv3MaterialColor, sizeof(uvector3) * gmat_material.uTextureCount,
			"bLoadMaterial: Unable to read the material color list."))
			return false;
	}

	if (!bReadFromSection(seh_handle, &gmat_material.fAmbient, sizeof(float),
		"bLoadMaterial: Unable to read the ambient lighting term."))
		return false;

	if (!bReadFromSection(seh_handle, &gmat_material.fDiffuse, sizeof(float),
		"bLoadMaterial: Unable to read the diffuse lighting term."))
		return false;

	if (!bReadFromSection(seh_handle, &gmat_material.fSpecular, sizeof(float),
		"bLoadMaterial: Unable to read the specular lighting term."))
		return false;

	if (!bReadFromSection(seh_handle, &gmat_material.fOpacity, sizeof(float),
		"bLoadMaterial: Unable to read the opacity value."))
		return false;

	if (!bReadFromSection(seh_handle, &gmat_material.fBumpiness, sizeof(float),
		"bLoadMaterial: Unable to read the bumpiness value."))
		return false;

	// Return a successful result.
	return true;
}


bool CGroffLoader::bLoadMapping(TSectionHandle seh_handle, CGroffMapping& gmap_mapping)
{
	// Attempt to write the class info to the image.
	if (!bReadFromSection(seh_handle, &gmap_mapping.sehMaterial, sizeof(TSectionHandle),
		"bLoadMapping: Unable to write the material handle."))
		return false;

	if (!bReadFromSection(seh_handle, &gmap_mapping.uTVertexCount, sizeof(uint),
		"bLoadMapping: Unable to write the texture vertex count."))
		return false;

	if (!bReadFromSection(seh_handle, &gmap_mapping.uTFaceCount, sizeof(uint),
		"bLoadMapping: Unable to write the texture face count."))
		return false;

	// Attempt to construct the mapping object. Were we successful?
	if (!gmap_mapping.bConfigure())
	{
#ifdef DEBUG_MODE
		// No! Report the error and return an error result.
		slLogfile.Msg("bLoadMapping: Unable to configure a mapping object.");
#endif
		// No! Return an error;
		return false;
	}

	if (!bReadFromSection(seh_handle, gmap_mapping.afv2TVertex, sizeof(fvector2) * gmap_mapping.uTVertexCount,
		"bLoadMapping: Unable to write the material handle."))
		return false;

	if (!bReadFromSection(seh_handle, gmap_mapping.auv3TFace, sizeof(uvector3) * gmap_mapping.uTFaceCount,
		"bLoadMapping: Unable to write the material handle."))
		return false;

	if (!bReadFromSection(seh_handle, gmap_mapping.auTFaceMaterialID, sizeof(uint) * gmap_mapping.uTFaceCount,
		"bLoadMapping: Unable to write the material handle."))
		return false;

	// Return a successful result.
	return true;
}


bool CGroffLoader::bLoadGeometry(TSectionHandle seh_handle, CGroffGeometry& gg_geometry)
{
	// Start out with a fresh object.
	gg_geometry.Initialize();

	//
	// Read the data from the image.
	//

	// Is this a version 14 or later GROFF file?
	if (14 <= g_u_Version_Number)
	{
		if (!bReadFromSection(seh_handle, &gg_geometry.fv3PivotOffset, sizeof(fvector3),
			"bLoadGeometry: Unable to read pivot offset."))
				return false;
	}
	else
	{
		// Zero pivot offset.
		gg_geometry.fv3PivotOffset.X = 0.0f;
		gg_geometry.fv3PivotOffset.Y = 0.0f;
		gg_geometry.fv3PivotOffset.Z = 0.0f;
	}

	if (!bReadFromSection(seh_handle, &gg_geometry.uVertexCount, sizeof(uint),
		"bLoadGeometry: Unable to read vertex count."))
		return false;

	if (!bReadFromSection(seh_handle, &gg_geometry.uFaceCount, sizeof(uint), 
		"bLoadGeometry: Unable to read face count."))
		return false;

	if (!bReadFromSection(seh_handle, &gg_geometry.uVertexNormalCount, sizeof(uint),
		"bLoadGeometry: Unable to read vertex normal count."))
		return false;

	// Is this a version 13 or later GROFF file?
	if (13 <= g_u_Version_Number)
	{
		if (!bReadFromSection(seh_handle, &gg_geometry.uWrapVertexCount, sizeof(uint),
			"bLoadGeometry: Unable to read wrap vertex count."))
			return false;
	}

	if (!bReadFromSection(seh_handle, &gg_geometry.uv3DefaultColor, sizeof(uvector3),
		"bLoadGeometry: Unable to read the default color."))
		return false;

	// Attempt to configure the geometry object.  Were we successful?
	if (!gg_geometry.bConfigure())
	{
#ifdef DEBUG_MODE
		// Something went wrong so return an error.
		slLogfile.Msg("bLoadGeometry: Unable to configure a geometry object.");
#endif
		// No! Return an error.
		return false;
	}

	// Now read in the rest of the geometry definition.
	if (!bReadFromSection(seh_handle, gg_geometry.afv3Vertex, sizeof(fvector3) * gg_geometry.uVertexCount,
		"bLoadGeometry: Unable to read vertices."))
		return false;

	if (!bReadFromSection(seh_handle, gg_geometry.auv3Face, sizeof(uvector3) * gg_geometry.uFaceCount,
		"bLoadGeometry: Unable to read faces."))
		return false;

	if (!bReadFromSection(seh_handle, gg_geometry.afv3FaceNormal, sizeof(fvector3) * gg_geometry.uFaceCount,
		"bLoadGeometry: Unable to read face normals."))
		return false;

	if (!bReadFromSection(seh_handle, gg_geometry.afv3VertexNormal, sizeof(fvector3) * gg_geometry.uVertexNormalCount,
		"bLoadGeometry: Unable to read vertex normals."))
		return false;

	// Is this a version 13 or later GROFF file?
	if (13 <= g_u_Version_Number)
	{
		if (!bReadFromSection(seh_handle, gg_geometry.afv3WrapVertex, sizeof(fvector3) * gg_geometry.uWrapVertexCount,
			"bLoadGeometry: Unable to read wrap vertices."))
			return false;
	}

	// Return a successful result.
	return true;
}


bool CGroffLoader::bLoadObject(TSectionHandle seh_handle, CGroffObject& go_object)
{
	// Start out with a fresh object.
	go_object.Initialize();

	// Attempt to read the object into memory.
	if (!bReadFromSection(seh_handle, &go_object.syhObjectName, sizeof(TSymbolHandle),
		"bReadObject: Unable to read object symbol handle."))
		return false;

	if (!bReadFromSection(seh_handle, &go_object.sehGeometry, sizeof(TSectionHandle),
		"bReadObject: Unable to read geometry section handle."))
		return false;

	if (!bReadFromSection(seh_handle, &go_object.sehMapping, sizeof(TSectionHandle),
		"bReadObject: Unable to read mapping section handle."))
		return false;

	if (!bReadFromSection(seh_handle, &go_object.sehPhysics, sizeof(TSectionHandle),
		"bReadObject: Unable to read physics section handle."))
		return false;

	if (!bReadFromSection(seh_handle, &go_object.sehAI, sizeof(TSectionHandle),
		"bReadObject: Unable to read AI section handle."))
		return false;

	if (!bReadFromSection(seh_handle, &go_object.sehSound, sizeof(TSectionHandle),
		"bReadObject: Unable to read sound section handle."))
		return false;

	if (!bReadFromSection(seh_handle, &go_object.sehSpecial, sizeof(TSectionHandle),
		"bReadObject: Unable to read special section handle."))
		return false;

	// Return a successful result.
	return true;
}


bool CGroffLoader::bLoadAttributes(TSectionHandle seh_attributes, 
                                   CValueTable& vtab_value_table)
{
	// Does the section handle refer to a valid section?
	if (seh_attributes == 0)
	{
		// No!  Report the error and the return an error result.
		slLogfile.Msg("bLoadAttributes: Unable to load value table since section handle is NULL.");

		return false;
	}

	// Determine the size of the section.
	uint u_buffer_size = fioFile.iCount(seh_attributes);

	// Were we successful?
	if (u_buffer_size == 0xFFFFFFFF)
	{
		// No!  Then return an error.
		return false;
	}

	// Allocate a temporary buffer for the data, then attempt to build the image.
	char* buffer = new char[u_buffer_size];

	// Were we successful at allocating the buffer?
	if (buffer == 0)
	{
		// No!  Report the error and return a failure result.
		slLogfile.Msg("bLoadAttributes: Unable to allocate a temporary buffer for the value table.");

		delete [] buffer;
		return false;
	}

	// Attempt to read the value table image into the temporary buffer.  Were we successful?
	if (!bReadFromSection(seh_attributes, 
                          buffer, 
                          u_buffer_size, 
                          "bLoadAttributes: Unable to read value table image."))
	{
		// No!  Report the error and return a failure result.
		slLogfile.Msg("bLoadAttributes: Unable to read the value table attribute image from the file.");

		delete [] buffer;
		return false;
	}

	// Attempt to construct the value table from the image.
	char *tmp = buffer;

	// Were we successful?
	if (vtab_value_table.uRead(&tmp) != u_buffer_size)
	{
		// No!  Report the error 
		slLogfile.Msg("bLoadAttributes: Image size does not match the buffer size.");

		delete [] buffer;
		return false;
	}
	
	// Yes!  Return a successful result.
	delete [] buffer;
	return true;
}


bool CGroffLoader::bLoadRegion(TSectionHandle seh_region, CGroffRegion& gr_region)
{
	// Attempt to read the write the object to the image.
	if (!bReadFromSection(seh_region, &gr_region.uObjectCount, sizeof(uint),
		"bLoadRegion: Unable to read object count."))
		return false;

	// Attempt to construct a region of the correct size.
	if (!gr_region.bConfigure())
	{
#ifdef DEBUG_MODE
		// Something went wrong so flush the image and return an error.
		sprintf(str_message, "bLoadRegion: Unable to configure a region object containing %d objects.", gr_region.uObjectCount); 
		slLogfile.Msg(str_message);
#endif
		// No!  Return an error.
		return false;
	}

	//
	// Attempt to read the object handles from the section and return the result;
	//

	// Setup the record offset as a function of the file version number.
	uint u_offset = 0;
	if (g_u_Version_Number == 11)
	{
		// Offset for the newly added hAttributeHandle in the CGroffObjectConfig class.
		u_offset = sizeof(CHandle);
	}
	for (uint u_index = 0; u_index < gr_region.uObjectCount; u_index++)
	{
		// Read the record in one at a time.
		if (!bReadFromSection(seh_region, &gr_region.agocObjectList[u_index], sizeof(CGroffObjectConfig)- u_offset,
			"bLoadRegion: Unable to read object handle from section."))
		{
			return false;
		}
		
		// Is this a version 11 file?
		if (g_u_Version_Number == 11)
		{
			// Yes!  Then set the handle to NULL.
			gr_region.agocObjectList[u_index].hAttributeHandle = hNULL_HANDLE;
		}
	}

	// Is this a version 12 or later GROFF file?
	if (12 <= g_u_Version_Number)
	{
		// Attempt to read the GROFF section handle for the value table image.
		return bReadFromSection(seh_region, &gr_region.sehAttributeSection, sizeof(TSectionHandle),
			"bLoadRegion: Unable to read attribute section handle.");
	}
	else
	{
		// Set the GROFF section handle to null.
		gr_region.sehAttributeSection = 0;
	}

	// Return a successful result.
	return true;
}


bool CGroffLoader::bLoadHeader(uint& u_version_number)
{
	// Initialize the file version number to 0.
	g_u_Version_Number = 0;
	u_version_number = 0;

	// Attempt to locate the header in the file.  Was the section found?
	TSectionHandle seh_handle = fioFile.sehLookup(".header");

	// Were we successful?
	if (seh_handle == 0)
	{
#ifdef DEBUG_MODE
		// Something went wrong so flush the image and return an error.
		slLogfile.Msg("bLoadHeader: Unable to locate section '.header'.");
#endif
		// No! Return an error.
		return false;
	}

	// Write out the file header.
	CGroffHeader gh_header;

	// Read in the file header.
	if (!bReadFromSection(seh_handle, &gh_header, sizeof(CGroffHeader), "bLoadHeader: Unable to read file header."))
	{
		// Return an error.
		return false;
	}

	// Is this file a groff file?
	if (gh_header.uMagicNumber != MAGIC_NUMBER)
	{
		// No!  Return an error.
		return false;
	}

	if (gh_header.uVersion & 0x80000000)
	{
		// Groff hase been re-written with optimized geometry.
		bOptimizedGROFF = true;
	}
	else
	{
		// Not an optimized Groff.
		bOptimizedGROFF = false;
	}

	u_version_number   = gh_header.uVersion & 0x7fffffff;
	g_u_Version_Number = gh_header.uVersion & 0x7fffffff;

	// Return a successful result.
	return true;
}
