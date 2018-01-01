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
 * $Log:: /JP2_PC/Source/Lib/Groff/Groff.cpp                                                  $
 * 
 * 11    8/26/98 4:40p Asouth
 * Moved loop variables into block scope
 * 
 * 10    5/18/98 12:53p Mlange
 * Added timing stat for pgonFindObjects().
 * 
 * 9     98.05.08 1:20p Mmouni
 * Added support for loading objects without materials and without textures.
 * 
 * 8     98.04.23 6:51p Mmouni
 * Added support for loading version 13 GROFFs (with wrap in GROFF).
 * 
 * 7     10/22/97 1:34a Gstull
 * Changes to support version 12 of the GROFF file structure definitions.  This mainly includes
 * support for text attributes for use with the attribute compiler.
 * 
 * 6     96/12/13 17:08 Speter
 * Changed pgonFindObject to const.
 * 
 * 5     96/12/12 20:27 Speter
 * Added CGroffObjectInfo::pgonFindObject() function.
 * 
 * 4     11/20/96 1:13p Gstull
 * Slight modifications to the Groff structures for integration into the GUIApp.
 * 
 * 3     11/15/96 7:29p Gstull
 * File changes to create initial version of the multi-section groff loader.
 * 
 * 2     11/15/96 11:09a Gstull
 * Changes to support multiple object files.
 * 
 * 1     11/14/96 1:50p Gstull
 * 
 * 2     9/16/96 8:24p Gstull
 * Added changes to integrate the new loader stuff with the existing GUI App.
 *
 *********************************************************************************************/

#include "Lib/Sys/SysLog.hpp"
#include "Lib/Sys/SmartBuffer.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Groff/FileIO.hpp"
#include "Lib/Groff/Groff.hpp"


namespace
{
	TCycles cyInFindObject = 0;
};

//******************************************************************************************
CGroffObjectName::CGroffObjectName()
{
	strObjectName    = 0;
	sehObjectSection = 0;

	fv3Position.X = 0.0f;
	fv3Position.Y = 0.0f;
	fv3Position.Z = 0.0f;

	fv3Rotation.X = 0.0f;
	fv3Rotation.Y = 0.0f;
	fv3Rotation.Z = 0.0f;

	fScale = 0.0f;
}


//******************************************************************************************
CGroffObjectName::~CGroffObjectName()
{
	// Was string name allocated?
	if (strObjectName != 0)
	{
		// Delete the string name.
		delete [] strObjectName;
		strObjectName = 0;
	}

	// Set the object section handle to 0.
	sehObjectSection = 0;

	fv3Position.X = 0.0f;
	fv3Position.Y = 0.0f;
	fv3Position.Z = 0.0f;

	fv3Rotation.X = 0.0f;
	fv3Rotation.Y = 0.0f;
	fv3Rotation.Z = 0.0f;

	fScale = 0.0f;
}


//******************************************************************************************
bool CGroffObjectName::bAddObject(
	TSectionHandle	seh_handle, 
	const char*		str_object_name,
	fvector3		fv3_position,
	fvector3		fv3_rotation,
	float			f_scale,
	CHandle			h_handle
)
{
	// Was a string already allocated?
	if (strObjectName != 0)
	{
		delete [] strObjectName;
	}

	// Allocate the string.
	strObjectName = new char[strlen(str_object_name)+1];

	// Were we successful?
	if (strObjectName == 0)
	{
		// No!  Return an error.
		return false;
	}

	// Copy the string.
	strcpy(strObjectName, str_object_name);

	// Assign the section handle.
	sehObjectSection = seh_handle;

	// Assign the object position, rotation and scale.
	fv3Position		 = fv3_position;
	fv3Rotation		 = fv3_rotation;
	fScale			 = f_scale;

	// Copy the attribute handle. 
	hAttributeHandle = h_handle;

	// Return a successful result.
	return true;
}


//******************************************************************************************
CGroffObjectInfo::CGroffObjectInfo() 
{ 
	uObjectCount  = 0; 
	gonObjectList = 0; 
}


//******************************************************************************************
CGroffObjectInfo::~CGroffObjectInfo()
{ 
	Initialize(); 
}

	
//******************************************************************************************
void CGroffObjectInfo::Initialize()
{
	// Has an object section list been allocated?
	if (gonObjectList != 0)
	{
		delete [] gonObjectList;
		gonObjectList = 0;
	}

	uObjectCount = 0;
}


//******************************************************************************************
bool CGroffObjectInfo::bConfigure()
{
	// Make sure the list is initialized.
	if (gonObjectList != 0)
	{
		delete [] gonObjectList;
		gonObjectList = 0;
	}

	// Attempt to build the info list.
	gonObjectList = new CGroffObjectName[uObjectCount];

	// Were we successful?
	if (gonObjectList == 0)
	{
		// No!  Set the object count to 0.
		uObjectCount = 0;

		return false;
	}

	// Initialize the object info structure.
	for (uint u_i = 0; u_i < uObjectCount; u_i++)
	{
		gonObjectList[u_i].strObjectName    = 0;
		gonObjectList[u_i].sehObjectSection = 0;

		// Set the object's world location to the origin.
		gonObjectList[u_i].fv3Position.X = 0.0f;
		gonObjectList[u_i].fv3Position.Y = 0.0f;
		gonObjectList[u_i].fv3Position.Z = 0.0f;
		
		// Set he object's orientation to as is.
		gonObjectList[u_i].fv3Rotation.X = 0.0f;
		gonObjectList[u_i].fv3Rotation.Y = 0.0f;
		gonObjectList[u_i].fv3Rotation.Z = 0.0f;

		// Set the object scale factor to 1.0.
		gonObjectList[u_i].fScale = 1.0f;

		// Set the attribute handle to NULL.
		gonObjectList[u_i].hAttributeHandle = hNULL_HANDLE;
	}

	// Return a successful result.
	return true;
}


//******************************************************************************************
bool CGroffObjectInfo::bAddObject(
	uint			u_index, 
	TSectionHandle	seh_handle, 
	const char*		str_object_name,
	fvector3		fv3_position,
	fvector3		fv3_rotation,
	float			f_scale,
	CHandle			h_handle
)
{
	// Is the index out of range?
	if (u_index >= uObjectCount)
	{
		// Yes!  Return an error.
		return false;
	}

	// Attempt to allocate the string.  Return the result.
	return gonObjectList[u_index].bAddObject(seh_handle, str_object_name, fv3_position, fv3_rotation, f_scale, h_handle);
}

//******************************************************************************************
CGroffObjectName* CGroffObjectInfo::pgonFindObject(const char* object_name) const
{
	CCycleTimer ctmr;

	CGroffObjectName* pgon_found = 0;

	for (uint u = 0; u < uObjectCount; u++)
	{
		if (!strcmp(gonObjectList[u].strObjectName, object_name))
		{
			pgon_found = &gonObjectList[u];
			break;
		}
	}

	cyInFindObject += ctmr();

	return pgon_found;
}


#if VER_TEST
//******************************************************************************************
float CGroffObjectInfo::fSecsInFindObject()
{
	TCycles cy_t = cyInFindObject;

	cyInFindObject = 0;

	return cy_t * CCycleTimer::fSecondsPerCycle();
}
#endif


//******************************************************************************************
void CGroffObjectInfo::Destroy() 
{ 
	Initialize(); 
}


//******************************************************************************************
CGroffHeader::CGroffHeader()
{ 
	uMagicNumber = 0xACEBABE; 
	uVersion     = GROFF_VERSION_ID;
}


//******************************************************************************************
CGroffHeader::~CGroffHeader() 
{
	uMagicNumber = 0;
	uVersion     = 0;
}


//******************************************************************************************
CGroffObjectConfig::CGroffObjectConfig()
{
	// Initialize the object section handle and symbol handle to 0.
	sehObject = 0;
	syhObjectName = 0;

	// Set the default position to the origin.
	fv3Position.X = 0.0f;
	fv3Position.Y = 0.0f;
	fv3Position.Z = 0.0f;

	// Set the default quaternion orientation of an object.
	fv3Rotation.X = 0.0f;
	fv3Rotation.Y = 0.0f;
	fv3Rotation.Z = 0.0f;

	// Set the default object scale to 1.
	fScale = 1.0f;

	// Set the attribute handle to NULL.
	hAttributeHandle = hNULL_HANDLE;
}


//******************************************************************************************
CGroffObjectConfig::~CGroffObjectConfig()
{
}


//******************************************************************************************
CGroffRegion::CGroffRegion()
{
	uObjectCount   = 0;
	agocObjectList = 0;
}


//******************************************************************************************
CGroffRegion::CGroffRegion(uint u_object_count)
{
	// Attempt to  allocate an object list.
	agocObjectList = new CGroffObjectConfig[u_object_count];

	// Were we successful?
	if (agocObjectList == 0)
	{
		// No!  Set the object count to 0.
		uObjectCount = 0;
	}

	// Setup the object count.
	uObjectCount  = u_object_count;
}


//******************************************************************************************
CGroffRegion::~CGroffRegion()
{
	uObjectCount  = 0;

	// Does an object list exist?
	if (agocObjectList != 0)
	{
		// Delete the list
		delete [] agocObjectList;
		agocObjectList = 0;
	}
}


//******************************************************************************************
bool CGroffRegion::bConfigure()
{
	// Does a region list exist?
	if (agocObjectList != 0)
	{
		// It is larger so free this list.
		delete [] agocObjectList;
	}

	// Attempt to allocate the new list.
	agocObjectList = new CGroffObjectConfig[uObjectCount];

	// Were we successful?
	if (agocObjectList == 0)
	{	
		// No! Update the object list count.
		uObjectCount = 0;

		// Return an error.
		return false;
	}

	// Return a successful result.
	return true;
}


//******************************************************************************************
CGroffObject::CGroffObject()
{
	Initialize();
}


//******************************************************************************************
CGroffObject::~CGroffObject()
{
	Initialize();
}


//******************************************************************************************
void CGroffObject::Initialize()
{
	syhObjectName = 0;

	sehGeometry   = 0;
	sehMapping    = 0;
	sehPhysics	  = 0;
	sehAI		  = 0;
	sehSound	  = 0;
	sehSpecial    = 0;
}


//******************************************************************************************
CGroffGeometry::CGroffGeometry()
{
	// Set all the geometry counts to 0.
	uVertexCount       = 0;
	uFaceCount         = 0;
	uVertexNormalCount = 0;
	uWrapVertexCount   = 0;

	// Set all the lists to null.
	afv3Vertex       = 0;
	auv3Face         = 0;
	afv3FaceNormal   = 0;
	afv3VertexNormal = 0;
	afv3WrapVertex   = 0;

	// Setup a default color if no material is present.
	uv3DefaultColor.X  = 128;
	uv3DefaultColor.Y  = 128;
	uv3DefaultColor.Z  = 128;
}


//******************************************************************************************
CGroffGeometry::~CGroffGeometry() 
{ 
	Initialize();
}


//******************************************************************************************
void CGroffGeometry::Initialize()
{
	// Set all the counts to 0.
	uVertexCount       = 0;
	uFaceCount         = 0;
	uVertexNormalCount = 0;
	uWrapVertexCount   = 0;

	// Setup a default color if no material is present.
	uv3DefaultColor.X  = 128;
	uv3DefaultColor.Y  = 128;
	uv3DefaultColor.Z  = 128;
	
	// Is there a vertex list? 
	if (afv3Vertex != 0)
	{
		// Yes! Deallocate it.
		delete [] afv3Vertex;
		afv3Vertex = 0;
	}

	// Is there a face list?
	if (auv3Face != 0)
	{
		// Yes! Deallocate it.
		delete [] auv3Face;
		auv3Face = 0;
	}

	// Is there a face normal list?
	if (afv3FaceNormal != 0)
	{
		// Yes! Deallocate it.
		delete [] afv3FaceNormal;
		afv3FaceNormal = 0;
	}

	// Is there a vertex normal list?
	if (afv3VertexNormal != 0)
	{
		// Yes!  Deallocate it.
		delete [] afv3VertexNormal;
		afv3VertexNormal = 0;
	}

	// Is there a vertex normal list?
	if (afv3WrapVertex != 0)
	{
		// Yes!  Deallocate it.
		delete [] afv3WrapVertex;
		afv3WrapVertex = 0;
	}
}


//******************************************************************************************
bool CGroffGeometry::bConfigure()
{
	// Is there a vertex count?
	if (uVertexCount > 0)
	{
		// Yes! Is there a vertex list? 
		if (afv3Vertex != 0)
		{
			// Yes! Deallocate it.
			delete [] afv3Vertex;
		}

		// Attempt to allocate a new vertex list.
		afv3Vertex = new fvector3[uVertexCount];

		// Were we successful?
		if (afv3Vertex == 0)
		{
			// No! Return an error.
			return false;
		}
	}

	// Is there a face count?
	if (uFaceCount > 0)
	{
		// Yes! Is there a face list?
		if (auv3Face != 0)
		{
			// Yes! Deallocate it.
			delete [] auv3Face;
		}

		// Attempt to allocate a new face list.
		auv3Face = new uvector3[uFaceCount];

		// Were we successful?
		if (auv3Face == 0)
		{
			// No! Return an error.
			return false;
		}

		// Is there a face normal list?
		if (afv3FaceNormal != 0)
		{
			// Yes! Deallocate it.
			delete [] afv3FaceNormal;
		}

		// Attempt to allocate a new face normal list.
		afv3FaceNormal = new fvector3[uFaceCount];

		// Were we successful?
		if (afv3FaceNormal == 0)
		{
			// No!  Return an error.
			return false;
		}
	}

	// Is there a vertex normal count?
	if (uVertexNormalCount > 0)
	{
		// Yes! Is there a vertex normal list? 
		if (afv3VertexNormal != 0)
		{
			// Yes! Deallocate it.
			delete [] afv3VertexNormal;
		}

		// Attempt to allocate a new vertex list.
		afv3VertexNormal = new fvector3[uVertexNormalCount];

		// Were we successful?
		if (afv3VertexNormal == 0)
		{
			// No! Return an error.
			return false;
		}
	}

	// Is there a wrap vertex count?
	if (uWrapVertexCount > 0)
	{
		// Yes! Is there a wrap vertex list? 
		if (afv3WrapVertex != 0)
		{
			// Yes!  Deallocate it.
			delete [] afv3WrapVertex;
		}

		// Attempt to allocate a new vertex list.
		afv3WrapVertex = new fvector3[uWrapVertexCount];

		// Were we successful?
		if (afv3WrapVertex == 0)
		{
			// No! Return an error.
			return false;
		}
	}

	// Return a successful result.
	return true;
}


//******************************************************************************************
CGroffMapping::CGroffMapping() 
{
	sehMaterial   = 0;
	uTVertexCount = 0;
	uTFaceCount   = 0;

	afv2TVertex = 0;
	auv3TFace = 0;
	auTFaceMaterialID = 0;
}


//******************************************************************************************
CGroffMapping::~CGroffMapping()
{ 
	Initialize(); 
}


//******************************************************************************************
void CGroffMapping::Initialize()
{
	sehMaterial   = 0;
	uTVertexCount = 0;
	uTFaceCount   = 0;

	// Is there a texture vertex list?
	if (afv2TVertex != 0)
	{
		// Deallocate it.
		delete [] afv2TVertex;
		afv2TVertex = 0;
	}

	// Is there a texture face list?
	if (auv3TFace != 0)
	{
		// Deallocate it.
		delete [] auv3TFace;
		auv3TFace = 0;
	}

	// Is there a material ID list?
	if (auTFaceMaterialID != 0)
	{
		// Deallocate it.
		delete [] auTFaceMaterialID;
		auTFaceMaterialID = 0;
	}
}


//******************************************************************************************
bool CGroffMapping::bConfigure()
{
	// Is there a texture vertex count?
	if (uTVertexCount > 0)
	{
		// Is there a texture vertex list?
		if (afv2TVertex != 0)
		{
			// Deallocate it.
			delete [] afv2TVertex;
		}

		// Attempt to allocate a texture vertex list?
		afv2TVertex = new fvector2[uTVertexCount];

		// Were we successful?
		if (afv2TVertex == 0)
		{
			// No! Return an error.
			return false;
		}
	}

	// Is there a texture face count?
	if (uTFaceCount > 0)
	{
		// Is there a texture face list?
		if (auv3TFace != 0)
		{
			// Deallocate it.
			delete [] auv3TFace;
		}

		// Attempt to allocate a texture face list.
		auv3TFace = new uvector3[uTFaceCount];

		// Were we successful?
		if (auv3TFace == 0)
		{
			// No! Return an error.
			return false;
		}

		// Is there a material ID list?
		if (auTFaceMaterialID != 0)
		{
			// Deallocate it.
			delete [] auTFaceMaterialID;
		}

		// Attempt to allocate a material ID list.
		auTFaceMaterialID = new uint[uTFaceCount];

		// Were we successful?
		if (auTFaceMaterialID == 0)
		{
			// No! Return an error.
			return false;
		}
	}

	// Return a successful result.
	return true;
}


//******************************************************************************************
CGroffMaterial::CGroffMaterial() 
{ 
	fAmbient     = 0.0f;
	fDiffuse     = 0.0f;
	fSpecular    = 0.0f;
	fOpacity     = 0.0f;
	fBumpiness   = 0.0f;

	// Set the texture count to 0.
	uTextureCount   = 0;

	// Set the material map handle lists to 0.
	asyhTextureHandle = 0;
	asyhOpacityHandle = 0;
	asyhBumpHandle = 0;
	auv3MaterialColor = 0;
}


//******************************************************************************************
CGroffMaterial::~CGroffMaterial() 
{ 
	Initialize(); 
}
	

//******************************************************************************************
void CGroffMaterial::Initialize()
{
	fAmbient     = 0.0f;
	fDiffuse     = 0.0f;
	fSpecular    = 0.0f;
	fOpacity     = 0.0f;
	fBumpiness   = 0.0f;

	uTextureCount   = 0;

	// Is there a texture map handle list?
	if (asyhTextureHandle != 0)
	{
		// Yes!  Deallocate it.
		delete [] asyhTextureHandle;
		asyhTextureHandle = 0;
	}

	// Is there an opacity map handle list?
	if (asyhOpacityHandle != 0)
	{
		// Yes! Deallocate it.
		delete [] asyhOpacityHandle;
		asyhOpacityHandle = 0;
	}

	// Is there a bump map handle list?
	if (asyhBumpHandle != 0)
	{
		// Deallocate it.
		delete [] asyhBumpHandle;
		asyhBumpHandle = 0;
	}

	if (auv3MaterialColor != 0)
	{
		delete[] auv3MaterialColor;
		auv3MaterialColor = 0;
	}
}


//******************************************************************************************
bool CGroffMaterial::bConfigure()
{
	// Is there a texture count?
	if (uTextureCount > 0)
	{
		// Is there a texture map handle list?
		if (asyhTextureHandle != 0)
		{
			// Yes!  Deallocate it.
			delete [] asyhTextureHandle;
		}

		// Attempt allocate a texture handle list.
		asyhTextureHandle = new TSymbolHandle[uTextureCount];

		// Were we successful?
		if (asyhTextureHandle == 0)
		{
			// No! return an error.
			return false;
		}

		// Is there an opacity map handle list?
		if (asyhOpacityHandle != 0)
		{
			// Yes! Deallocate it.
			delete [] asyhOpacityHandle;
		}

		// Attempt allocate a texture handle list.
		asyhOpacityHandle = new TSymbolHandle[uTextureCount];

		// Were we successful?
		if (asyhOpacityHandle == 0)
		{
			// No! return an error.
			return false;
		}

		// Is there a bump map handle list?
		if (asyhBumpHandle != 0)
		{
			// Deallocate it.
			delete [] asyhBumpHandle;
		}

		// Attempt allocate a texture handle list.
		asyhBumpHandle = new TSymbolHandle[uTextureCount];

		// Were we successful?
		if (asyhBumpHandle == 0)
		{
			// No! return an error.
			return false;
		}

		// Attempt to allocate material color list.
		auv3MaterialColor = new uvector3[uTextureCount];

		if (auv3MaterialColor == 0)
			return false;
	}

	// Return a successful result.
	return true;
}


//******************************************************************************************
CGroffMaterialName::CGroffMaterialName()
{
	// Set the material count to 0.
	uTextureCount = 0;

	// Set the material lists to 0.
	strTextureMap = 0;
	strOpacityMap = 0;
	strBumpMap = 0;
}


//******************************************************************************************
CGroffMaterialName::~CGroffMaterialName()
{
	// Destroy everything.
	Initialize();
}


//******************************************************************************************
void CGroffMaterialName::Initialize()
{
	// Is there a texture map name list?
	if (strTextureMap != 0)
	{
		// Yes! Look through the name list and deallocate any strings.
		for (uint u_i = 0; u_i < uTextureCount; u_i++)
		{
			// Is there a material name defined?
			if (strTextureMap[u_i] != 0)
			{
				// Yes!  Deallocate it.
				delete [] strTextureMap[u_i];
			}
		}

		// Deallocate the texure map list.
		delete [] strTextureMap;

		// Set the pointer to null.
		strTextureMap = 0;
	}

	// Is there a opacity map name list?
	if (strOpacityMap != 0)
	{
		// Yes! Look through the name list and deallocate any strings.
		for (uint u_i = 0; u_i < uTextureCount; u_i++)
		{
			// Is there a material name defined?
			if (strOpacityMap[u_i] != 0)
			{
				// Yes!  Deallocate it.
				delete [] strOpacityMap[u_i];
			}
		}

		// Deallocate the texure map list.
		delete [] strOpacityMap;

		// Set the pointer to null.
		strOpacityMap = 0;
	}

	// Is there a opacity map name list?
	if (strBumpMap != 0)
	{
		// Yes! Look through the name list and deallocate any strings.
		for (uint u_i = 0; u_i < uTextureCount; u_i++)
		{
			// Is there a material name defined?
			if (strBumpMap[u_i] != 0)
			{
				// Yes!  Deallocate it.
				delete [] strBumpMap[u_i];
			}
		}

		// Deallocate the texure map list.
		delete [] strBumpMap;

		// Set the pointer to null.
		strBumpMap = 0;
	}

	// Set the texture count to 0.
	uTextureCount = 0;
}


//******************************************************************************************
bool CGroffMaterialName::bBuildNames(CFileIO& fio_file, CGroffMaterial& gmat_material)
{
	// Initialize the name list structure.
	Initialize();

	// Attempt to construct the texture map list.
	strTextureMap = new char *[gmat_material.uTextureCount];

	// Were we successful?
	if (strTextureMap == 0)
	{
		// No! Return an error.
		return false;
	}

	// Attempt to construct the texture map list.
	strOpacityMap = new char *[gmat_material.uTextureCount];

	// Were we successful?
	if (strOpacityMap == 0)
	{
		// No! Initialize the structure.
		Initialize();

		// Return an error.
		return false;
	}

	// Attempt to construct the texture map list.
	strBumpMap = new char *[gmat_material.uTextureCount];

	// Were we successful?
	if (strBumpMap == 0)
	{
		// No! Initialize the structure.
		Initialize();

		// No! Return an error.
		return false;
	}

	// Set the material count.
	uTextureCount = gmat_material.uTextureCount;

	uint u_i;
	for (u_i = 0; u_i < uTextureCount; u_i++)
	{
		// Set all the material lists to null.
		strTextureMap[u_i] = 0;
		strOpacityMap[u_i] = 0;
		strBumpMap[u_i] = 0;
	}

	// Loop 
	for (u_i = 0; u_i < uTextureCount; u_i++)
	{
		// Does a material name exist?
		if (gmat_material.asyhTextureHandle[u_i] != 0)
		{
			// Yes! Attempt to lookup the texture name.
			const char* str_map_name = fio_file.strLookup(gmat_material.asyhTextureHandle[u_i]);

			// Were we successful?
			if (str_map_name != 0)
			{
				// Yes! Attempt to allocate a string.
				strTextureMap[u_i] = new char[strlen(str_map_name)+1];

				// Were we successful?
				if (strTextureMap[u_i] == 0)
				{
					// No!  Initialize the structure and return an error.
					Initialize();

					return false;
				}

				// Copy the string.
				strcpy(strTextureMap[u_i], str_map_name);
			}
			else
			{
				// No! Initialize the structure and return an error.
				Initialize();

				return false;
			}
		}

		// Does a material name exist?
		if (gmat_material.asyhOpacityHandle[u_i] != 0)
		{
			// Yes! Attempt to lookup the texture name.
			const char* str_map_name = fio_file.strLookup(gmat_material.asyhOpacityHandle[u_i]);

			// Were we successful?
			if (str_map_name != 0)
			{
				// Yes! Attempt to allocate a string.
				strOpacityMap[u_i] = new char[strlen(str_map_name)+1];

				// Were we successful?
				if (strOpacityMap[u_i] == 0)
				{
					// No!  Initialize the structure and return an error.
					Initialize();

					return false;
				}

				// Copy the string.
				strcpy(strOpacityMap[u_i], str_map_name);
			}
			else
			{
				// No! Initialize the structure and return an error.
				Initialize();

				return false;
			}
		}

		// Does a material name exist?
		if (gmat_material.asyhBumpHandle[u_i] != 0)
		{
			// Yes! Attempt to lookup the texture name.
			const char* str_map_name = fio_file.strLookup(gmat_material.asyhBumpHandle[u_i]);

			// Were we successful?
			if (str_map_name != 0)
			{
				// Yes! Attempt to allocate a string.
				strBumpMap[u_i] = new char[strlen(str_map_name)+1];

				// Were we successful?
				if (strBumpMap[u_i] == 0)
				{
					// No!  Initialize the structure and return an error.
					Initialize();

					return false;
				}

				// Copy the string.
				strcpy(strBumpMap[u_i], str_map_name);
			}
			else
			{
				// No! Initialize the structure and return an error.
				Initialize();

				return false;
			}
		}
	}

	// Return a successful result.
	return true;
}


//******************************************************************************************
bool CGroffMaterialName::bBuildHandles(CFileIO& fio_file, CGroffMaterial& gmat_material)
{
	return false;
}


//******************************************************************************************
const char* CGroffMaterialName::strTextureName(uint u_index)
{
	// Is the index in range?
	if (u_index < uTextureCount)
	{
		// Yes!  Return the string.
		return strTextureMap[u_index];
	}

	// Return an error.
	return 0;
}


//******************************************************************************************
const char* CGroffMaterialName::strOpacityName(uint u_index)
{
	// Is the index in range?
	if (u_index < uTextureCount)
	{
		// Yes!  Return the string.
		return strOpacityMap[u_index];
	}

	// Return an error.
	return 0;
}


//******************************************************************************************
const char* CGroffMaterialName::strBumpName(uint u_index)
{
	// Is the index in range?
	if (u_index < uTextureCount)
	{
		// Yes!  Return the string.
		return strBumpMap[u_index];
	}

	// Return an error.
	return 0;
}


//******************************************************************************************
CGroffBitmap::CGroffBitmap()
{
	syhBitmapName = 0;
	uv3MeanColor.X = 0;
	uv3MeanColor.Y = 0;
	uv3MeanColor.Z = 0;

	uWidth  = 0;
	uHeight = 0;
	uDepth  = 0;

	sehPalette = 0;
	
	au1Data = 0;
}


//******************************************************************************************
CGroffBitmap::~CGroffBitmap() 
{ 
	Initialize(); 
}
	

//******************************************************************************************
void CGroffBitmap::Initialize()
{
	// Set the bitmap name handle to 0.
	syhBitmapName = 0;

	// Set the mean color to 0.
	uv3MeanColor.X = 0;
	uv3MeanColor.Y = 0;
	uv3MeanColor.Z = 0;

	// Set the bitmaps dimensions to 0.
	uWidth  = 0;
	uHeight = 0;
	uDepth  = 0;

	// Set the palette section handle to 0.
	sehPalette = 0;

	// Was a bitmap allocated?
	if (au1Data != 0)
	{
		// Yes! Then deallocate it.
		delete [] au1Data;
		au1Data = 0;
	}
}


//******************************************************************************************
bool CGroffBitmap::bConfigure()
{
	// Is the bitmap ready to be configured?
	if (uWidth == 0 || uHeight == 0)
	{
		// No! Return an error.
		return false;
	}

	// Is the depth reasonable?
	uint u_size;
	switch (uDepth)
	{
		case 8:
			u_size = 1;
			break;
		case 24:
			u_size = 3;
			break;
		default:
			// The depth is not a reasonable value.  Return an error.
			return false;
	}

	// Was a bitmap allocated?
	if (au1Data != 0)
	{
		// Yes! Then deallocate it.
		delete [] au1Data;
	}

	// Attempt to allocate the bitmap.
	au1Data = new uint8[uWidth * uHeight * u_size];

	// Were we successful?
	if (au1Data == 0)
	{
		// No!  Return an error.
		return false;
	}

	// Return a succesful result.
	return true;
}


//******************************************************************************************
CGroffPalette::CGroffPalette()
{
	// Set the palette name handle and count to 0.
	syhPaletteName = 0;
	uPaletteCount  = 0;

	// Set the palette list to null.
	auv3Palette = 0;
}



//******************************************************************************************
CGroffPalette::~CGroffPalette() 
{ 
	Initialize(); 
}


//******************************************************************************************
void CGroffPalette::Initialize()
{
	syhPaletteName = 0;
	uPaletteCount  = 0;

	// Was a palette allocated?
	if (auv3Palette != 0)
	{
		// Yes! Then deallocate it.
		delete [] auv3Palette;
		auv3Palette = 0;
	}
}


//******************************************************************************************
bool CGroffPalette::bConfigure()
{
	// Is there a requested buffer size?
	if (uPaletteCount > 0 && uPaletteCount < 256)
	{
		// Was a palette allocated?
		if (auv3Palette != 0)
		{
			// Yes! Then deallocate it.
			delete [] auv3Palette;
		}

		// Attempt to allocate a palette.
		auv3Palette = new uvector3[uPaletteCount];

		// Were we successful?
		if (auv3Palette == 0)
		{
			// No! return an error.
			return false;
		}

		// Initialize the palette to known values.
		for (uint u_i = 0; u_i < uPaletteCount; u_i++)
		{
			// Set the palette colors to 0.
			auv3Palette[u_i].X = 0;
			auv3Palette[u_i].Y = 0;
			auv3Palette[u_i].Z = 0;
		}

		// Return a successful result.
		return true;
	}

	// Return an error.
	return false;
}


//******************************************************************************************
CGroffPhysics::CGroffPhysics() 
{
	uByteCount = 0;
	pu1Buffer = 0;
}


//******************************************************************************************
CGroffPhysics::~CGroffPhysics() 
{ 
	Initialize(); 
}


//******************************************************************************************
void CGroffPhysics::Initialize()
{
	uByteCount = 0;

	// Was a buffer allocated?
	if (pu1Buffer != 0)
	{
		// Yes! Then deallocate it.
		delete [] pu1Buffer;
		pu1Buffer = 0;
	}
}


//******************************************************************************************
bool CGroffPhysics::bConfigure()
{
	// Was a buffer size specified?
	if (uByteCount > 0)
	{
		// Was a buffer allocated?
		if (pu1Buffer != 0)
		{
			// Yes! Then deallocate it.
			delete [] pu1Buffer;
		}

		// Attempt to allocate a new buffer.
		pu1Buffer = new uint8[uByteCount];

		// Were we successful?
		if (pu1Buffer == 0)
		{
			// No!  Return an error.
			return false;
		}
	}

	// Return a successful result.
	return true;
}


//******************************************************************************************
CGroffAI::CGroffAI() 
{
	uByteCount = 0;
	pu1Buffer = 0;
}


//******************************************************************************************
CGroffAI::~CGroffAI() 
{ 
	Initialize(); 
}


//******************************************************************************************
void CGroffAI::Initialize()
{
	uByteCount = 0;

	// Was a buffer allocated?
	if (pu1Buffer != 0)
	{
		// Yes! Then deallocate it.
		delete [] pu1Buffer;
		pu1Buffer = 0;
	}
}


//******************************************************************************************
bool CGroffAI::bConfigure()
{
	// Was a buffer size specified?
	if (uByteCount > 0)
	{
		// Was a buffer allocated?
		if (pu1Buffer != 0)
		{
			// Yes! Then deallocate it.
			delete [] pu1Buffer;
		}

		// Attempt to allocate a new buffer.
		pu1Buffer = new uint8[uByteCount];

		// Were we successful?
		if (pu1Buffer == 0)
		{
			// No!  Return an error.
			return false;
		}
	}

	// Return a successful result.
	return true;
}


//******************************************************************************************
CGroffSound::CGroffSound() 
{
	uByteCount = 0;
	pu1Buffer = 0;
}


//******************************************************************************************
CGroffSound::~CGroffSound() 
{ 
	Initialize(); 
}


//******************************************************************************************
void CGroffSound::Initialize()
{
	uByteCount = 0;

	// Was a buffer allocated?
	if (pu1Buffer != 0)
	{
		// Yes! Then deallocate it.
		delete [] pu1Buffer;
		pu1Buffer = 0;
	}
}


//******************************************************************************************
bool CGroffSound::bConfigure()
{
	// Was a buffer size specified?
	if (uByteCount > 0)
	{
		// Was a buffer allocated?
		if (pu1Buffer != 0)
		{
			// Yes! Then deallocate it.
			delete [] pu1Buffer;
		}

		// Attempt to allocate a new buffer.
		pu1Buffer = new uint8[uByteCount];

		// Were we successful?
		if (pu1Buffer == 0)
		{
			// No!  Return an error.
			return false;
		}
	}

	// Return a successful result.
	return true;
}


//******************************************************************************************
CGroffDesign::CGroffDesign() 
{
	uByteCount = 0;
	pu1Buffer = 0;
}


//******************************************************************************************
CGroffDesign::~CGroffDesign() 
{ 
	Initialize(); 
}


//******************************************************************************************
void CGroffDesign::Initialize()
{
	uByteCount = 0;

	// Was a buffer allocated?
	if (pu1Buffer != 0)
	{
		// Yes! Then deallocate it.
		delete [] pu1Buffer;
		pu1Buffer = 0;
	}
}


//******************************************************************************************
bool CGroffDesign::bConfigure()
{
	// Was a buffer size specified?
	if (uByteCount > 0)
	{
		// Was a buffer allocated?
		if (pu1Buffer != 0)
		{
			// Yes! Then deallocate it.
			delete [] pu1Buffer;
		}

		// Attempt to allocate a new buffer.
		pu1Buffer = new uint8[uByteCount];

		// Were we successful?
		if (pu1Buffer == 0)
		{
			// No!  Return an error.
			return false;
		}
	}

	// Return a successful result.
	return true;
}


//******************************************************************************************
CGroffTrigger::CGroffTrigger() 
{
	uByteCount = 0;
	pu1Buffer = 0;
}


//******************************************************************************************
CGroffTrigger::~CGroffTrigger() 
{ 
	Initialize(); 
}


//******************************************************************************************
void CGroffTrigger::Initialize()
{
	uByteCount = 0;

	// Was a buffer allocated?
	if (pu1Buffer != 0)
	{
		// Yes! Then deallocate it.
		delete [] pu1Buffer;
		pu1Buffer = 0;
	}
}


//******************************************************************************************
bool CGroffTrigger::bConfigure()
{
	// Was a buffer size specified?
	if (uByteCount > 0)
	{
		// Was a buffer allocated?
		if (pu1Buffer != 0)
		{
			// Yes! Then deallocate it.
			delete [] pu1Buffer;
		}

		// Attempt to allocate a new buffer.
		pu1Buffer = new uint8[uByteCount];

		// Were we successful?
		if (pu1Buffer == 0)
		{
			// No!  Return an error.
			return false;
		}
	}

	// Return a successful result.
	return true;
}


//******************************************************************************************
CGroffSpecial::CGroffSpecial() 
{
	uByteCount = 0;
	pu1Buffer = 0;
}


//******************************************************************************************
CGroffSpecial::~CGroffSpecial() 
{ 
	Initialize(); 
}


//******************************************************************************************
void CGroffSpecial::Initialize()
{
	uByteCount = 0;

	// Was a buffer allocated?
	if (pu1Buffer != 0)
	{
		// Yes! Then deallocate it.
		delete [] pu1Buffer;
		pu1Buffer = 0;
	}
}


//******************************************************************************************
bool CGroffSpecial::bConfigure()
{
	// Was a buffer size specified?
	if (uByteCount > 0)
	{
		// Was a buffer allocated?
		if (pu1Buffer != 0)
		{
			// Yes! Then deallocate it.
			delete [] pu1Buffer;
		}

		// Attempt to allocate a new buffer.
		pu1Buffer = new uint8[uByteCount];

		// Were we successful?
		if (pu1Buffer == 0)
		{
			// No!  Return an error.
			return false;
		}
	}

	// Return a successful result.
	return true;
}
