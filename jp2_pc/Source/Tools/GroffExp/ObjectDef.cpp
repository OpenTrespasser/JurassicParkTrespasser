/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1996-1997
 *
 * Contents: Implementation of classes for managing object definition information.
 *
 * Bugs:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/ObjectDef.cpp                                          $
 * 
 * 14    7/15/97 6:58p Gstull
 * Made organizational changes to make management of the exporter more reasonable.
 * 
 * 13    7/14/97 3:36p Gstull
 * Made substantial changes to support tasks for the August 18th milestone for attributes,
 * object representation, geometry checking, dialog interfacing, logfile support, string tables
 * and string support.
 * 
 *********************************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <time.h>


#include "StandardTypes.hpp"
#include "Lib/Sys/SysLog.hpp"
#include "ObjectDef.hpp"
#include "Lib/Groff/FileIO.hpp"
#include "Tools/GroffExp/GUIInterface.hpp"

extern CGUIInterface guiInterface;

CSysLog slExportLog;


//**********************************************************************************************
//
CObjectDef::CObjectDef(const char *str_object_name)
{
	// Setup the object name.
	estrObjectName = str_object_name;

	// Set the ID number of this node.
	iID					= -1;

	// Initialize everything to sane values.
	podNext				= 0;
	
	// Set the positional information to the correct default values.
	memset(&uv3DefaultColor, 128, sizeof(uvector3));

	// Initialize the geometric info.
	uVertexCount		= 0;
	afv3Vertex			= 0;

	uFaceCount			= 0;
	auv3Face			= 0;

	uFaceNormalCount	= 0;
	afv3FaceNormal		= 0;

	afv3VertexNormal	= 0;

	// Initialize the mapping info.
	uTextureVertexCount	= 0;
	afv2TextureVertex	= 0;

	uTextureFaceCount	= 0;
	auv3TextureFace		= 0;
	auFaceMaterialIndex = 0;

	uMaterialCount		= 0;
	astrTextureMap		= 0;
	astrOpacityMap		= 0;
	astrBumpMap			= 0;
	auv3MeanColor		= 0;
	alvLightingValues   = 0;
	
	uWarnings			= 0;
	uErrors				= 0;
}


//**********************************************************************************************
//
void CObjectDef::Destroy()
{
	// Deallocate all allocated memory
	estrObjectName = "";

	// Reset the next node pointer.
	podNext = 0;

	// Reset the node ID number.
	iID = -1;

	// Set the positional information to the correct default values.
	memset(&uv3DefaultColor, 128, sizeof(uvector3));
	fUniformScale = 1.0f;

	// Are there any vertices?
	if (afv3Vertex != 0)
	{
		// Deallocate the object vertex list
		delete [] afv3Vertex;

		afv3Vertex = 0;
	}

	// Are there any faces?
	if (auv3Face != 0)
	{
		// Deallocate the object face list
		delete [] auv3Face;

		auv3Face = 0;
	}

	// Are there any face normals.
	if (afv3FaceNormal != 0)
	{
		// Deallocate the normal list
		delete [] afv3FaceNormal;

		afv3FaceNormal = 0;
	}

	// Are there any vertex normals?
	if (afv3VertexNormal != 0)
	{
		// Dealocate the texture vertex list
		delete [] afv3VertexNormal;

		afv3VertexNormal = 0;
	}

	// Are there any texture vertices?
	if (afv2TextureVertex != 0)
	{
		// Dealocate the texture vertex list
		delete [] afv2TextureVertex;

		afv2TextureVertex = 0;
	}

	// Are there any texture faces?
	if (auv3TextureFace != 0)
	{
		// Deallocate the texture face list
		delete [] auv3TextureFace;

		auv3TextureFace = 0;
	}

	// Is there a face/material index list. 
	if (auFaceMaterialIndex != 0)
	{
		// Dealocate the face material index list
		delete [] auFaceMaterialIndex;

		auFaceMaterialIndex = 0;
	}

	// Was a mean color array allocated?
	if (auv3MeanColor != 0)
	{
		// Dealocate the face material index list
		delete [] auv3MeanColor;

		auv3MeanColor = 0;
	}

	// Was a lighting values list allocated?
	if (alvLightingValues != 0)
	{
		// Deallocate the lighting values list.
		delete [] alvLightingValues;

		alvLightingValues = 0;
	}

	// Are there any materials in the list.
	for (uint u_i = 0; u_i < uMaterialCount; u_i++)
	{
		// Were any texture maps allocated?
		if (astrTextureMap != 0)
		{
			// Yes!  Does this material have a texture map?
			if (astrTextureMap[u_i] != 0)
			{
				// Yes!  Deallocate it from the list.
				delete [] astrTextureMap[u_i];

				astrTextureMap[u_i] = 0;
			}
		}

		// Were any opacity maps allocated?
		if (astrOpacityMap != 0)
		{
			// Yes!  Does this material have an opacity map?
			if (astrOpacityMap[u_i] != 0)
			{
				// Yes!  Deallocate it from the list.
				delete [] astrOpacityMap[u_i];

				astrOpacityMap[u_i] = 0;
			}
		}

		// Were any bump maps allocated?
		if (astrBumpMap != 0)
		{
			// Yes!  Does this material have a bump map?
			if (astrBumpMap[u_i] != 0)
			{
				// Yes!  Deallocate it from the list.
				delete [] astrBumpMap[u_i];

				astrBumpMap[u_i] = 0;
			}
		}
	}

	// Now free each of the lists, if they were allocated.
	if (astrTextureMap != 0)
	{
		delete [] astrTextureMap;

		astrTextureMap = 0;
	}
	if (astrOpacityMap != 0)
	{
		delete [] astrOpacityMap;

		astrOpacityMap = 0;
	}
	if (astrBumpMap != 0)
	{
		delete [] astrBumpMap;

		astrBumpMap = 0;
	}

	// Set all the counts to 0.
	uMaterialCount		= 0;
	uVertexCount		= 0;
	uFaceCount			= 0;
	uFaceNormalCount	= 0;
	uVertexNormalCount	= 0;
	uTextureVertexCount	= 0;
	uTextureFaceCount	= 0;

	// Set all the warning and error counters to 0.
	uWarnings			= 0;
	uErrors				= 0;
}


//**********************************************************************************************
//
CObjectDef::~CObjectDef()
{
	Destroy();
}


//**********************************************************************************************
//
void CObjectDef::Position
(
	Point3& p3_position
)
{
	p3Position = p3_position;
}


//**********************************************************************************************
//
Point3& CObjectDef::p3GetPosition
(
)
{
	return p3Position;
}


//**********************************************************************************************
//
void CObjectDef::Rotation
(
	Point3& p3_rotation
)
{
	p3Rotation = p3_rotation;
	EulerToQuat(&p3_rotation.x, qRotation);
}


//**********************************************************************************************
//
Point3& CObjectDef::p3GetRotation
(
)
{
	return p3Rotation;
}


//**********************************************************************************************
//
void CObjectDef::Rotation
(
	Quat& q_rotation
)
{
	qRotation = q_rotation;
	QuatToEuler(qRotation, &p3Rotation.x);
}


//**********************************************************************************************
//
Quat& CObjectDef::qGetRotation
(
)
{
	return qRotation;
}


//**********************************************************************************************
//
void CObjectDef::Scale
(
	float f_scale_value
)
{
	fUniformScale = f_scale_value;
}


//**********************************************************************************************
//
float CObjectDef::fGetScale
(
)
{
	return fUniformScale;
}


//**********************************************************************************************
//
void CObjectDef::PivotOffset
(
	Point3& p3_pivot_offset
)
{
	p3PivotOffset = p3_pivot_offset;
}


//**********************************************************************************************
//
Point3& CObjectDef::p3GetPivotOffset
(
)
{
	return p3PivotOffset;
}


//**********************************************************************************************
//
void CObjectDef::PivotRotation
(
	Quat& q_pivot_rotation
)
{
	q_pivot_rotation = q_pivot_rotation;
}


//**********************************************************************************************
//
Quat& CObjectDef::qGetPivotRotation
(
)
{
	return qPivotRotation;
}


//**********************************************************************************************
//
void CObjectDef::BoundingBox
(
	Box3& bx3_bounding_box
)
{
	bx3BoundingBox = bx3_bounding_box;
}


//**********************************************************************************************
//
Box3& CObjectDef::bx3GetBoundingBox
(
)
{
	return bx3BoundingBox;
}


//**********************************************************************************************
//
void CObjectDef::ObjectTM
(
	Matrix3& m3_matrix
)
{
	m3ObjectTM = m3_matrix;
}


//**********************************************************************************************
//
Matrix3& CObjectDef::m3GetObjectTM
(
)
{
	return m3ObjectTM;
}


//**********************************************************************************************
//
void CObjectDef::NodeTM
(
	Matrix3& m3_matrix
)
{
	m3NodeTM = m3_matrix;
}


//**********************************************************************************************
//
Matrix3& CObjectDef::m3GetNodeTM
(
)
{
	return m3NodeTM;
}


//**********************************************************************************************
//
void CObjectDef::ParentTM
(
	Matrix3& m3_matrix
)
{
	m3ParentTM = m3_matrix;
}


//**********************************************************************************************
//
Matrix3& CObjectDef::m3GetParentTM
(
)
{
	return m3ParentTM;
}


//**********************************************************************************************
//
void CObjectDef::RelativeTM
(
	Matrix3& m3_matrix
)
{
	m3RelativeTM = m3_matrix;
}


//**********************************************************************************************
//
Matrix3& CObjectDef::m3GetRelativeTM
(
)
{
	return m3RelativeTM;
}


//**********************************************************************************************
//
void CObjectDef::TMBeforeWSM(Matrix3& m3_matrix)
{
	m3TMBeforeWSM = m3_matrix;
}


//**********************************************************************************************
//
Matrix3& CObjectDef::m3GetTMBeforeWSM()
{
	return m3TMBeforeWSM;
}


//**********************************************************************************************
//
void CObjectDef::TMAfterWSM(Matrix3& m3_matrix)
{
	m3TMAfterWSM = m3_matrix;
}


//**********************************************************************************************
//
Matrix3& CObjectDef::m3GetTMAfterWSM()
{
	return m3TMAfterWSM;
}


//**********************************************************************************************
//
bool CObjectDef::VertexCount(uint u_vertex_count)
{
	// Allocate memory for the vertex list
	afv3Vertex = new fvector3[u_vertex_count];

	// Were we successful?
	if (afv3Vertex == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate vertex list of size %d for object: %s\n", u_vertex_count,
			estrObjectName.strData());

		// No!  Return an error.
		return false;
	}

	// Setup the vertex count.
	uVertexCount = u_vertex_count;

	// Initialize the vertex list to known values;
	for (uint u_i = 0; u_i < u_vertex_count; u_i++)
	{
		// Set each component to 0.0
		afv3Vertex[u_i].X = 0.0f;
		afv3Vertex[u_i].Y = 0.0f;
		afv3Vertex[u_i].Z = 0.0f;
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::Vertex(uint u_vertex_index, float f_x, float f_y, float f_z) 
{
	// Is the specified index in range?
	if (u_vertex_index >= uVertexCount || afv3Vertex == 0)
	{
		// Range error.  Return an error.
		return false;
	}

	// The index is in range, so setup the vector
	afv3Vertex[u_vertex_index].X = f_x;
	afv3Vertex[u_vertex_index].Y = f_y;
	afv3Vertex[u_vertex_index].Z = f_z;

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::FaceCount(uint u_face_count)
{
	// Allocate memory for the face list.
	auv3Face = new uvector3[u_face_count];

	// Were we successful?
	if (auv3Face == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate face list of size %d for object: %s\n", u_face_count,
			estrObjectName.strData());

		// No!  Return an error.
		return false;
	}

	// Setup the face count.
	uFaceCount = u_face_count;

	// Initialize all the face list
	for (uint u_i = 0; u_i < u_face_count; u_i++)
	{
		// Set each component to 0.
		auv3Face[u_i].X = 0;
		auv3Face[u_i].Y = 0;
		auv3Face[u_i].Z = 0;
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::Face(uint u_face_index, uint u_v0, uint u_v1, uint u_v2)
{
	// Is the specified index in range?
	if (u_face_index >= uFaceCount || auv3Face == 0)
	{
		// Range error.  Return an error.
		return false;
	}

	// Check and make sure there are no degenerate faces
	if (u_v0 == u_v1 || u_v1 == u_v2 || u_v0 == u_v2)
	{
		// Bummer!  This should not happen.  3DS Max is generating degenerate
		// polygons where at least two of the three vertices are the same.

		// Log an error to the export log
		slExportLog.Printf("Error: Degenerate polygon detected in object: %s\n", estrObjectName.strData());
		slExportLog.Printf("Polygon: %d, Vertices: (%d, %d, %d)\n\n", u_face_index, u_v0, u_v1, u_v2);

		// Increment the counters for keeping track of the errors.
		uErrors++;
	}

	// The index is in range, so setup the vector.
	auv3Face[u_face_index].X = u_v0;
	auv3Face[u_face_index].Y = u_v1;
	auv3Face[u_face_index].Z = u_v2;

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::VertexNormalCount(uint u_face_count)
{
	// Allocate memory for the face list
	afv3VertexNormal = new fvector3[u_face_count * 3];

	// Were we successful?
	if (afv3VertexNormal == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate vertex normal list of size %d for object: %s\n", 
			u_face_count, estrObjectName.strData());

		// No!  Return an error.
		return false;
	}

	// Setup the number of normals. The number of faces * 3 vertices per face.
	uVertexNormalCount = u_face_count * 3;

	// Initialize all the face list
	for (uint u_i = 0; u_i < u_face_count * 3; u_i++)
	{
		// Set each component to 0
		afv3VertexNormal[u_i].X = 0.0f;
		afv3VertexNormal[u_i].Y = 0.0f;
		afv3VertexNormal[u_i].Z = 0.0f;
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::VertexNormal(uint u_face_index, uint u_vertex_index, float f_x, float f_y, float f_z)
{
	// Setup a variable which defines the into the vertex normal array.
	const uint u_offset = u_face_index * 3 + u_vertex_index;

	// Is the specified index in range?
	if (u_offset >= uVertexNormalCount || afv3VertexNormal == 0)
	{
		// Range error.  Return an error.
		return false;
	}

	// The index is in range, so setup the vector
	afv3VertexNormal[u_offset].X = f_x;
	afv3VertexNormal[u_offset].Y = f_y;
	afv3VertexNormal[u_offset].Z = f_z;

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::FaceNormalCount(uint u_face_count)
{
	// Allocate memory for the face list
	afv3FaceNormal = new fvector3[u_face_count];

	// Were we successful?
	if (afv3FaceNormal == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate face normal list of size %d for object: %s\n", 
			u_face_count, estrObjectName.strData());

		// No!  Return an error.
		return false;
	}

	// Setup the number of normals.
	uFaceNormalCount = u_face_count;

	// Initialize all the face list
	for (uint u_i = 0; u_i < u_face_count; u_i++)
	{
		// Set each component to 0
		afv3FaceNormal[u_i].X = 0.0f;
		afv3FaceNormal[u_i].Y = 0.0f;
		afv3FaceNormal[u_i].Z = 0.0f;
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::FaceNormal(uint u_face_index, float f_x, float f_y, float f_z)
{
	// Is the specified index in range?
	if (u_face_index >= uFaceCount || afv3FaceNormal == 0)
	{
		// Range error.  Return an error.
		return false;
	}

	// The index is in range, so setup the vector
	afv3FaceNormal[u_face_index].X = f_x;
	afv3FaceNormal[u_face_index].Y = f_y;
	afv3FaceNormal[u_face_index].Z = f_z;

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::TextureVertexCount(uint u_texture_vertex_count)
{
	// Allocate memory for the texture vertex list
	afv2TextureVertex = new fvector2[u_texture_vertex_count];

	// Were we successful?
	if (afv2TextureVertex == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate texture vertex list of size %d for object: %s\n", 
			u_texture_vertex_count, estrObjectName.strData());

		// No!  Return an error.
		return false;
	}

	uTextureVertexCount = u_texture_vertex_count;

	// Initialize the texture vertex list
	for (uint u_i = 0; u_i < u_texture_vertex_count; u_i++)
	{
		// Set each component to 0.0
		afv2TextureVertex[u_i].X = 0.0f;
		afv2TextureVertex[u_i].Y = 0.0f;
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::TextureVertex(uint u_texture_vertex_index, float f_u, float f_v) 
{
	// Is the specified index in range?
	if (u_texture_vertex_index >= uTextureVertexCount || afv2TextureVertex == 0) 
	{
		// Range error.  Return an error.
		return false;
	}

	// The index is in range, so setup the vector
	afv2TextureVertex[u_texture_vertex_index].X = f_u;
	afv2TextureVertex[u_texture_vertex_index].Y = f_v;

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::TextureFaceCount(uint u_face_count)
{
	// Allocate memory for the texture vertex list
	auv3TextureFace = new uvector3[u_face_count];

	// Were we successful?
	if (auv3TextureFace == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate texture face list of size %d for object: %s\n", 
			u_face_count, estrObjectName.strData());

		// No!  Return an error.
		return false;
	}

	uTextureFaceCount = u_face_count;

	// Setup an array to keep track of the material index for each face.
	auFaceMaterialIndex = new uint[u_face_count];

	// Were we successful?
	if (auFaceMaterialIndex == 0)
	{
		// No!  Return an error.
		return false;
	}

	// Initialize the texture face list and the material index list.
	for (uint u_i = 0; u_i < u_face_count; u_i++)
	{
		// Set each component to 0.0
		auv3TextureFace[u_i].X = 0;
		auv3TextureFace[u_i].Y = 0;
		auv3TextureFace[u_i].Z = 0;

		// Initialize this material index to an invalid value.
		auFaceMaterialIndex[u_i] = 0xFFFFFFFF;
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::TextureFace(uint u_face_index, uint u_v0, uint u_v1, uint u_v2)
{
	bool bResult = true;

	// Is the specified index in range?
	if (u_face_index >= uTextureFaceCount || auv3TextureFace == 0)
	{
		// Range error.  Return an error.
		return false;
	}

	// Check and make sure there are no degenerate texture faces
	if (u_v0 == u_v1 || u_v1 == u_v2 || u_v0 == u_v2)
	{
		// Bummer!  This should not happen.  3DS Max is generating degenerate
		// polygons where at least two of the three vertices are the same.

		// Log an error to the export log
		slExportLog.Printf("Error: Degenerate texture face detected in object: %s\n", estrObjectName.strData());
		slExportLog.Printf("Face: %d, Texture vertices: (%d, %d, %d)\n", u_face_index, u_v0, u_v1, u_v2);

		// Increment the counters for keeping track of the errors.
		uErrors++;

		// Return an error
		bResult = false;
	}

	// The index is in range, so setup the vector
	auv3TextureFace[u_face_index].X = u_v0;
	auv3TextureFace[u_face_index].Y = u_v1;
	auv3TextureFace[u_face_index].Z = u_v2;

	// Return the result.
	return bResult;
}


//**********************************************************************************************
//
void CObjectDef::LightingValues(uint u_material_index, SLightingValues* plv_lighting_values)
{
	// Was a lighting value array allocated?
	if (alvLightingValues != 0)
	{
		// Is the material index value in range?
		if (u_material_index < uMaterialCount)
		{
			alvLightingValues[u_material_index].fAmbient   = plv_lighting_values->fAmbient;
			alvLightingValues[u_material_index].fDiffuse   = plv_lighting_values->fDiffuse;
			alvLightingValues[u_material_index].fSpecular  = plv_lighting_values->fSpecular;
			alvLightingValues[u_material_index].fOpacity   = plv_lighting_values->fOpacity;
			alvLightingValues[u_material_index].fBumpiness = plv_lighting_values->fBumpiness;
		}
	}
}


//**********************************************************************************************
//
void CObjectDef::MeanColor(uint u_material_index, uint8 u1_blue, uint8 u1_green, uint8 u1_red)
{
	// Was a mean color array allocated?
	if (auv3MeanColor != 0)
	{
		// Is the material index value in range?
		if (u_material_index < uMaterialCount)
		{
			auv3MeanColor[u_material_index].X = u1_blue;
			auv3MeanColor[u_material_index].Y = u1_green;
			auv3MeanColor[u_material_index].Z = u1_red;
		}
	}
	else
	{
		// Log an error to the export log
		slExportLog.Printf("Warning: Attempted to assign mean color with index %d to null list for object: %s\n", 
			u_material_index, estrObjectName.strData());
	}
}


//**********************************************************************************************
//
void CObjectDef::DefaultColor(uint8 u1_blue, uint8 u1_green, uint8 u1_red)
{
	uv3DefaultColor.X = u1_blue;
	uv3DefaultColor.Y = u1_green;
	uv3DefaultColor.Z = u1_red;
}


//**********************************************************************************************
//
bool CObjectDef::FaceMaterialIndex(uint u_face_index, uint u_material_index)
{
	// Is the specified index in range?
	if (u_face_index >= uTextureFaceCount || auFaceMaterialIndex == 0)
	{
		// Range error.  Return an error.
		return false;
	}

	//
	// Is the material index in range?
	// 
	// 11/05/96 - Greg Stull
	// 
	// Removed the code to range check the materials because single materials are allowed to 
	// have materials ID's which are out of range, as absurd as this might sound.  This 
	// condition is later resolved by the validation code in the geomety class.
	//

	// Add the index into the list
	auFaceMaterialIndex[u_face_index] = u_material_index;

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::MaterialCount(uint u_material_count)
{
	// Allocate memory for the texture map list.
	astrTextureMap = new char *[u_material_count];

	// Were we successful?
	if (astrTextureMap == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate texture map list of size %d for object: %s\n",
			u_material_count, estrObjectName.strData());

		// No!  Return an error.
		return false;
	}

	// Allocate memory for the texture map list.
	astrOpacityMap = new char *[u_material_count];

	// Were we successful?
	if (astrOpacityMap == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate opacity map list of size %d for object: %s\n",
			u_material_count, estrObjectName.strData());

		// No! Deallocate the texture map list.
		delete [] astrTextureMap;

		astrTextureMap = 0;

		// Return an error.
		return false;
	}

	// Allocate memory for the texture map list.
	astrBumpMap = new char *[u_material_count];

	// Were we successful?
	if (astrBumpMap == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate bump map list of size %d for object: %s\n",
			u_material_count, estrObjectName.strData());

		// No! Deallocate the texture map list.
		delete [] astrTextureMap;

		astrTextureMap = 0;

		// No! Deallocate the texture map list.
		delete [] astrOpacityMap;

		astrOpacityMap = 0;

		// No!  Return an error.
		return false;
	}

	// Allocate memory for the texture map list.
	alvLightingValues = new SLightingValues[u_material_count];

	// Were we successful?
	if (alvLightingValues == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate lighting values list of size %d for object: %s\n",
			u_material_count, estrObjectName.strData());

		// No! Deallocate the texture map list.
		delete [] astrTextureMap;

		astrTextureMap = 0;

		// No! Deallocate the texture map list.
		delete [] astrOpacityMap;

		astrOpacityMap = 0;

		// No! Deallocate the texture map list.
		delete [] astrBumpMap;

		astrBumpMap = 0;

		// No!  Return an error.
		return false;
	}

	// Allocate memory for the mean colors.
	auv3MeanColor = new uvector3[u_material_count];

	// Were we successful?
	if (auv3MeanColor == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate mean color list of size %d for object: %s\n",
			u_material_count, estrObjectName.strData());

		// No! Deallocate the texture map list.
		delete [] astrTextureMap;

		astrTextureMap = 0;

		// No! Deallocate the texture map list.
		delete [] astrOpacityMap;

		astrOpacityMap = 0;

		// No! Deallocate the texture map list.
		delete [] alvLightingValues;

		alvLightingValues = 0;

		// No! Deallocate the lighting values list.
		delete [] alvLightingValues;

		// No!  Return an error.
		return false;
	}

	uMaterialCount = u_material_count;

	// Initialize the normal list
	for (uint u_i = 0; u_i < u_material_count; u_i++)
	{
		// Set each of the indices in the bitmap name lists to null.
		astrTextureMap[u_i] = 0;
		astrOpacityMap[u_i] = 0;
		astrBumpMap[u_i]    = 0;

		alvLightingValues[u_i].fAmbient   = -1.0f;
		alvLightingValues[u_i].fDiffuse   = -1.0f;
		alvLightingValues[u_i].fSpecular  = -1.0f;
		alvLightingValues[u_i].fOpacity   = -1.0f;
		alvLightingValues[u_i].fBumpiness = -1.0f;

		auv3MeanColor[u_i].X = 0;
		auv3MeanColor[u_i].Y = 0;
		auv3MeanColor[u_i].Z = 0;
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::TextureName(uint u_material_index, const char* str_mapname) 
{
	// Is the specified index in range?
	if (u_material_index >= uMaterialCount)
	{
		// Range error.  Return an error.
		return false;
	}

	// The index is in range, so setup the string.  If memory for a string already
	// exists, deallocate it and add a new string since the lengths could be off.
	if (astrTextureMap[u_material_index] != 0)
	{
		// Free the previous string.
		delete [] astrTextureMap[u_material_index];

		astrTextureMap[u_material_index] = 0;
	}

	// Allocate a new string
	astrTextureMap[u_material_index] = new char[strlen(str_mapname) + 1];

	// Were we successful?
	if (astrTextureMap[u_material_index] == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate texture name %s at index %d for object: %s\n",
			str_mapname, u_material_index, estrObjectName.strData());

		// Return an error
		return false;
	}

	strcpy(astrTextureMap[u_material_index], str_mapname);

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::OpacityName(uint u_material_index, const char* str_mapname) 
{
	// Is the specified index in range?
	if (u_material_index >= uMaterialCount)
	{
		// Range error.  Return an error.
		return false;
	}

	// The index is in range, so setup the string.  If memory for a string already
	// exists, deallocate it and add a new string since the lengths could be off.
	if (astrOpacityMap[u_material_index] != 0)
	{
		// Free the previous string.
		delete [] astrOpacityMap[u_material_index];

		astrOpacityMap[u_material_index] = 0;
	}

	// Allocate a new string
	astrOpacityMap[u_material_index] = new char[strlen(str_mapname) + 1];

	// Were we successful?
	if (astrOpacityMap[u_material_index] == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate opacity name %s at index %d for object: %s\n",
			str_mapname, u_material_index, estrObjectName.strData());

		// Return an error
		return false;
	}

	strcpy(astrOpacityMap[u_material_index], str_mapname);

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CObjectDef::BumpmapName(uint u_material_index, const char* str_mapname) 
{
	// Is the specified index in range?
	if (u_material_index >= uMaterialCount)
	{
		// Range error.  Return an error.
		return false;
	}

	// The index is in range, so setup the string.  If memory for a string already
	// exists, deallocate it and add a new string since the lengths could be off.
	if (astrBumpMap[u_material_index] != 0)
	{
		// Free the previous string.
		delete [] astrBumpMap[u_material_index];

		astrBumpMap[u_material_index] = 0;
	}

	// Allocate a new string
	astrBumpMap[u_material_index] = new char[strlen(str_mapname) + 1];

	// Were we successful?
	if (astrBumpMap[u_material_index] == 0)
	{
		// Log an error to the export log
		slExportLog.Printf("Error: Unable to allocate bump map name %s at index %d for object: %s\n",
			str_mapname, u_material_index, estrObjectName.strData());

		// Return an error
		return false;
	}

	strcpy(astrBumpMap[u_material_index], str_mapname);

	// Return a successful result.
	return true;
}


/*

 //**********************************************************************************************
//
// Methods for adjusting the list.
//

//**********************************************************************************************
//
bool CObjectDef::OrderVertices()
{
	fvector3*	pfv3_v0;
	fvector3*	pfv3_v1;
	fvector3*	pfv3_v2;

	fvector3	fv3_v0;
	fvector3	fv3_v1;
	fvector3	fv3_cross_product;

	fvector3*	pfv3_normal;
	
	uint		u_tmp;
	float		f_dir;

	// In order to generate these we need both the vertices and the normals
	if (afv3FaceNormal == 0 || afv3Vertex == 0)
	{
		// Both structures are not present, so return and error.
		return false;
	}


	// The rendering engine prefers to have all polygons ordered in so that
	// when facing the polygon (i.e. the normal is pointing towards me), the 
	// vertices are ordered counter clockwise .
	//
	// This can asily be checked by generating to vectors, generating a cross
	// product, then taking the inner product of the cross product of the two
	// vectors and the polygon normal.  If the result is positive, then the
	// vertices are properly ordered.  If the the result is negative, then the
	// vertices are ordered incorrectly.  This condition can be easily resolved 
	// by switching the first two vertices.  This only works because we are
	// working with three sided polygons.
	

	// Process each polygon in the object
	for (uint u_i = 0; u_i < uFaceCount; u_i++)
	{
		// Get the polygon normal
		pfv3_normal = &afv3FaceNormal[u_i];

		// First build the direction vectors for the cross product.. Start by
		// getting all three vertices.
		pfv3_v0 = &afv3Vertex[auv3Face[u_i].X];
		pfv3_v1 = &afv3Vertex[auv3Face[u_i].Y];
		pfv3_v2 = &afv3Vertex[auv3Face[u_i].Z];

		// Now build the direction vectors
		fv3_v0.X = pfv3_v1->X - pfv3_v0->X;
		fv3_v0.Y = pfv3_v1->Y - pfv3_v0->Y;
		fv3_v0.Z = pfv3_v1->Z - pfv3_v0->Z;

		fv3_v1.X = pfv3_v2->X - pfv3_v1->X;
		fv3_v1.Y = pfv3_v2->Y - pfv3_v1->Y;
		fv3_v1.Z = pfv3_v2->Z - pfv3_v1->Z;

		// Calculate the cross product
		fv3_cross_product.X = fv3_v0.Y * fv3_v1.Z - fv3_v0.Z * fv3_v1.Y;
		fv3_cross_product.Y = fv3_v0.Z * fv3_v1.X - fv3_v0.X * fv3_v1.Z;
		fv3_cross_product.Z = fv3_v0.X * fv3_v1.Y - fv3_v0.Y * fv3_v1.X;

		// Now calculate the dot product.
		f_dir = fv3_cross_product.X * pfv3_normal->X + 
			    fv3_cross_product.Y * pfv3_normal->Y + 
				fv3_cross_product.Z * pfv3_normal->Z;

		// Do these vertics need to be reordered?
		if (f_dir < 0.0f) {

			// Log an error to the export log
			slExportLog.Printf("Warning: Object: %s has polygon with vertices in clockwise order.\n", estrObjectName.strData());
			slExportLog.Printf("Polygon: %d, Vertices: (%d, %d, %d)\n", u_i, auv3Face[u_i].X, auv3Face[u_i].Y, auv3Face[u_i].Z);

			// Yes! So reorder the polygon
			u_tmp		    = auv3Face[u_i].Y;
			auv3Face[u_i].Y = auv3Face[u_i].Z;
			auv3Face[u_i].Z = u_tmp;

			// Keep track of the number of reordered polygons. FYI
			uWarnings++;
		}
	}


	// Return a successful result
	return true;
}

 
//**********************************************************************************************
//
bool CObjectDef::BuildNormals()
{
	fvector3*	pfv3_v0;
	fvector3*	pfv3_v1;
	fvector3*	pfv3_v2;

	fvector3	fv3_v0;
	fvector3	fv3_v1;

	fvector3*	pfv3_normal;
	float		f_length;


	// In order to generate these we need both the vertices and the normals

	if (afv3FaceNormal == 0 || afv3Vertex == 0)
	{
		// Both structures are not present, so return and error(true)
		return false;
	}


	// The rendering engine prefers to have all polygons ordered in so that
	// when facing the polygon (i.e. the normal is pointing towards me), the 
	// vertices are ordered counter clockwise .
	//
	// This can asily be checked by generating to vectors, generating a cross
	// product, then taking the inner product of the cross product of the two
	// vectors and the polygon normal.  If the result is positive, then the
	// vertices are properly ordered.  If the the result is negative, then the
	// vertices are ordered incorrectly.  This condition can be easily resolved 
	// by switching the first two vertices.  This only works because we are
	// working with three sided polygons.
	

	// Process each polygon in the object
	for (uint u_i = 0; u_i < uFaceCount; u_i++)
	{
		// Get the polygon normal
		pfv3_normal = &afv3FaceNormal[u_i];

		// First build the direction vectors for the cross product.. Start by
		// getting all three vertices.
		pfv3_v0 = &afv3Vertex[auv3Face[u_i].X];
		pfv3_v1 = &afv3Vertex[auv3Face[u_i].Y];
		pfv3_v2 = &afv3Vertex[auv3Face[u_i].Z];

		// Now build the direction vectors
		fv3_v0.X = pfv3_v1->X - pfv3_v0->X;
		fv3_v0.Y = pfv3_v1->Y - pfv3_v0->Y;
		fv3_v0.Z = pfv3_v1->Z - pfv3_v0->Z;

		fv3_v1.X = pfv3_v2->X - pfv3_v1->X;
		fv3_v1.Y = pfv3_v2->Y - pfv3_v1->Y;
		fv3_v1.Z = pfv3_v2->Z - pfv3_v1->Z;

		// Calculate the cross product
		pfv3_normal->X = fv3_v0.Y * fv3_v1.Z - fv3_v0.Z * fv3_v1.Y;
		pfv3_normal->Y = fv3_v0.Z * fv3_v1.X - fv3_v0.X * fv3_v1.Z;
		pfv3_normal->Z = fv3_v0.X * fv3_v1.Y - fv3_v0.Y * fv3_v1.X;

		// Calculate the length of the vectors
		f_length = (float) sqrt((double) pfv3_normal->X * pfv3_normal->X + pfv3_normal->Y * pfv3_normal->Y + pfv3_normal->Z * pfv3_normal->Z);

		// Now, make the normals unit length
		pfv3_normal->X /= f_length;
		pfv3_normal->Y /= f_length;
		pfv3_normal->Z /= f_length;
	}


	// Return a successful result
	return true;
}
*/


static bool b_open = false;

//**********************************************************************************************
//
CObjectDefList::CObjectDefList()
{
	// Set the list pointers to 0 
	podHead = 0; 
    podTail = 0; 

	// Set the total object node count to 0
    uNodeCount = 0;

	b_open = false;
}


//**********************************************************************************************
//
CObjectDefList::~CObjectDefList()
{
	// Log a final message to the export logfile
	slExportLog.Printf("Export complete:\n");

	// Close the export logfile
	slExportLog.Close();
}


//**********************************************************************************************
//
CObjectDef* CObjectDefList::podAddObject(const char* str_object_name)
{
	CObjectDef*  pod_new_node = new CObjectDef(str_object_name);
	CObjectDef*	pod_last = podTail;

	// Is the logfile open?
	if (!b_open)
	{
		// Indicate that the file is now open.
		b_open = true;

		// Determine whether the file should be active or not.
		if (guiInterface.bGenerateLogfiles())
		{
			// Open a logfile.
			char str_logfile[256];
			guiInterface.BuildPath(str_logfile, guiInterface.strGetLogfileDirPath(), "ObjectDefRaw.log");
			slExportLog.Open(str_logfile);

			// Activate the logfile.
			slExportLog.Enable();
		}
		else
		{
			// Deactivate the logfile.
			slExportLog.Disable();
		}
	}

	// Were we able to allocate a new node?
	if (pod_new_node == false)
	{
		// Log a final message to the export logfile
		slExportLog.Printf("Error: Unable to allocate a node for object: %s", str_object_name);
	}

	// Set the ID number of the new node.
	pod_new_node->iID = uNodeCount++;

	// Insert the node into the list
	if (podHead == false)
	{
		// This is easy since the list is empty
		podHead = pod_new_node;
		podTail = pod_new_node;
	}
	else
	{
		// Append this object to the end of the list;
		podTail->podNext = pod_new_node;
		podTail			 = pod_new_node;
	}

	// Return this new node to the caller
	return pod_new_node;
}


//**********************************************************************************************
//
void CObjectDefList::DeleteObjects()
{
	CObjectDef*	pod_next = podHead;

	// Delete all memobers from the list
	while(podHead)
	{
		// Setup a pointer to the next node
		pod_next = podHead->podNext;

		// Deallocate the contents of the node.
		podHead->Destroy();

		// Delete this node
		delete podHead;

		// Update the pointer to the next node
		podHead = pod_next;
	}

	// Reset the pointers to reasonable initial values
	podHead	   = 0;
	podTail	   = 0;
	uNodeCount = 0;
}


//**********************************************************************************************
//
CObjectDef* CObjectDefList::podLookup(uint u_object_index)
{
	// Perform a simple range check
	if (u_object_index >= uNodeCount)
		// Return an invalid pointer
		return 0;

	// We now know there is a correcsponding node in the list.  So start looking...
	CObjectDef* pod_node = podHead;

	// Walk the node list
	for (; pod_node != 0 && u_object_index; u_object_index--)
		pod_node = pod_node->podNext;
		
	// This must be the node we are looking for, so return it
	return pod_node;
}


//**********************************************************************************************
//
CObjectDef*	CObjectDefList::podLookup(const char* str_object_name)
{	
	// Search until we are at the end of the list or have foudn a match.
	CObjectDef* pod_node = podHead;

	while(pod_node) 
	{
		// Is this the Node we are looking for?
		if (pod_node->estrObjectName != str_object_name)
		{
			// Aha!  We have found the node.  Return it!
			return pod_node;
		}

		// Move onto the next node
		pod_node = pod_node->podNext;
	}

	// Was unable to locate a matching node.  Return an invalid pointer
	return 0;
}


//**********************************************************************************************
//
uint CObjectDefList::uWarnings()
{
	uint		uWarnings = 0;
	CObjectDef*	pod_node  = podHead;


	// Determine the total number of warnings.
	while(pod_node)
	{
		uWarnings += pod_node->uWarnings;

		// Advance to the next node.
		pod_node = pod_node->podNext;
	}

	// Return the total number of warnings.
	return uWarnings;
}


//**********************************************************************************************
//
uint CObjectDefList::uErrors()
{
	uint		uErrors  = 0;
	CObjectDef*	pod_node = podHead;


	// Determine the total number of Normals
	while(pod_node)
	{
		uErrors += pod_node->uErrors;

		// Advance to the next node
		pod_node = pod_node->podNext;
	}

	// Return the total number of errors.
	return uErrors;
}


//**********************************************************************************************
//
void CObjectDefList::Dump()
{
	// Open the log file.
	CSysLog		sl_glog;
	CObjectDef*	pod_node = podHead;
	struct tm*	tm_current_time;
	time_t		tt_clock;

	// Is the user generating log_files?
	if (!guiInterface.bGenerateLogfiles())
	{
		// No! The return immediately.
		return;
	}

	// Get the currrent time
	time(&tt_clock);
	tm_current_time = localtime(&tt_clock);

	// Determine whether the file should be active or not.
	if (guiInterface.bGenerateLogfiles())
	{
		// Construct the proper path for the logfile.
		char str_logfile[256];
		guiInterface.BuildPath(str_logfile, guiInterface.strGetLogfileDirPath(), "ObjectDefDump.log");
		sl_glog.Open(str_logfile);

		// Activate the logfile.
		sl_glog.Enable();
	}
	else
	{
		// Deactivate the logfile.
		sl_glog.Disable();
	}

	// Display a banner
	sl_glog.Printf("----- Geometry logfile: -----\nFile creation date: %s \n", asctime(tm_current_time));

	// Are there any nodes to display?
	if (pod_node == 0) 
	{
		// No. Log completion message, close file and return
		sl_glog.Printf("Geometry list is <EMPTY>.\n");

		return;
	}

	while(pod_node != 0)
	{
		// First display the name of the object
		sl_glog.Printf("\nObject name: \"%s\", ID %d\n\n", pod_node->estrObjectName.strData(), pod_node->iID);

		// Record the object's position.
		Point3 p3_position = pod_node->p3GetPosition();
		sl_glog.Printf("Position: x -> %.3f, y -> %.3f, z -> %.3f\n", p3_position[0], p3_position[1], p3_position[2]);

		// Record the object's orientation.
		Point3 p3_rotation(pod_node->p3GetRotation());
		sl_glog.Printf("Rotation: x -> %.3f, y -> %.3f, z -> %.3f\n", p3_rotation[0], p3_rotation[1], p3_rotation[2]);

		// Record the object's uniform scaling constant. 
		sl_glog.Printf("Uniform Scaling value: %3f\n", pod_node->fGetScale());

		// Display the vertices for this object
		sl_glog.Printf("Object vertices: %d\n", pod_node->uVertexCount);

		// Dump the object vertices
		for (uint u_i = 0; u_i < pod_node->uVertexCount; u_i++)
		{
			// Display the vertex
			sl_glog.Printf("Vertex %4d: x -> %.3f, y -> %.3f, z -> %.3f\n", u_i, 
				pod_node->afv3Vertex[u_i].X, pod_node->afv3Vertex[u_i].Y, pod_node->afv3Vertex[u_i].Z);
		}

		// Display the polygon face definitions
		sl_glog.Printf("\nObject faces: %d\n", pod_node->uFaceCount);

		// Dump the object faces
		for (u_i = 0; u_i < pod_node->uFaceCount; u_i++)
		{
			// Display the vertex
			sl_glog.Printf("Face %4d: x -> %4d, y -> %4d, z -> %4d\n", u_i, 
				pod_node->auv3Face[u_i].X, pod_node->auv3Face[u_i].Y, pod_node->auv3Face[u_i].Z);
		}

		sl_glog.Printf("\nObject face normals: %d\n", pod_node->uFaceNormalCount);

		// Dump the object normals
		for (u_i = 0; u_i < pod_node->uFaceNormalCount; u_i++)
		{
			// Display the normals
			sl_glog.Printf("Normal %4d: x -> %.3f, y -> %3.f, z -> %.3f\n", u_i, 
				pod_node->afv3FaceNormal[u_i].X, pod_node->afv3FaceNormal[u_i].Y, pod_node->afv3FaceNormal[u_i].Z);
		}

		sl_glog.Printf("\nObject face vertex normals: %d\n", pod_node->uVertexNormalCount);

		// Dump the object normals
		for (u_i = 0; u_i < pod_node->uVertexNormalCount; u_i+=3)
		{
			// Display the vertex normals.
			sl_glog.Printf("Face %4d: Vertex 0: x -> %.3f, y -> %.3f, z -> %.3f\n", u_i/3, 
				pod_node->afv3VertexNormal[u_i].X, pod_node->afv3VertexNormal[u_i].Y, pod_node->afv3VertexNormal[u_i].Z);

			sl_glog.Printf("Face %4d: Vertex 1: x -> %.3f, y -> %.3f, z -> %.3f\n", u_i/3, 
				pod_node->afv3VertexNormal[u_i+1].X, pod_node->afv3VertexNormal[u_i+1].Y, pod_node->afv3VertexNormal[u_i+1].Z);
			
			sl_glog.Printf("Face %4d: Vertex 2: x -> %.3f, y -> %.3f, z -> %.3f\n", u_i/3, 
				pod_node->afv3VertexNormal[u_i+2].X, pod_node->afv3VertexNormal[u_i+2].Y, pod_node->afv3VertexNormal[u_i+2].Z);
		}

		// Optional stuff which may be associated with an object
		if (pod_node->uTextureVertexCount > 0)
		{
			sl_glog.Printf("\nObject texture vertices: %d\n", pod_node->uTextureVertexCount);

			// Dump the object vertices
			for (u_i = 0; u_i < pod_node->uTextureVertexCount; u_i++)
			{
				// Display the texture vertices
				sl_glog.Printf("Texture vertex %4d: u -> %.3f, v -> %.3f\n", u_i, 
					pod_node->afv2TextureVertex[u_i].X, pod_node->afv2TextureVertex[u_i].Y);
			}

			// Display the polygon face definitions
			sl_glog.Printf("\nObject texture faces: %d\n", pod_node->uTextureFaceCount);

			// Is this a single-material object?
			if (pod_node->uMaterialCount == 1)
			{
				// Dump the object faces
				for (u_i = 0; u_i < pod_node->uTextureFaceCount; u_i++)
				{
					// Display the texture faces
					sl_glog.Printf("Texture face %4d: x -> %4d, y -> %4d, z -> %4d\n", u_i, 
						pod_node->auv3TextureFace[u_i].X, pod_node->auv3TextureFace[u_i].Y, pod_node->auv3TextureFace[u_i].Z);
				}

				sl_glog.Printf("\n");

				// Display any materials which may be present.
				if (pod_node->astrTextureMap != 0)
				{
					if (pod_node->astrTextureMap[0] != 0)
					{
						sl_glog.Printf("Diffuse map: %s\n", pod_node->astrTextureMap[0]);
					}

					// Display the mean color.
					if (pod_node->auv3MeanColor != 0)
					{
						sl_glog.Printf("Mean color : R -> %d, G -> %d, B -> %d\n", 
							pod_node->auv3MeanColor[0].Z, pod_node->auv3MeanColor[0].Y, pod_node->auv3MeanColor[0].X);
					}
				}

				if (pod_node->astrOpacityMap != 0)
				{
					if (pod_node->astrOpacityMap[0] != 0)
					{
						sl_glog.Printf("Opacity map: %s\n", pod_node->astrOpacityMap[0]);
					}
				}

				if (pod_node->astrBumpMap != 0)
				{
					if (pod_node->astrBumpMap[0] != 0)
					{
						sl_glog.Printf("Bumpmap    : %s\n", pod_node->astrBumpMap[0]);
					}
				}

				if (pod_node->alvLightingValues != 0)
				{
					sl_glog.Printf("\n");
					if (pod_node->alvLightingValues[0].fAmbient != -1.0f)
					{
						sl_glog.Printf("Ambient    : %f\n", pod_node->alvLightingValues[0].fAmbient);
					}

					if (pod_node->alvLightingValues[0].fDiffuse != -1.0f)
					{
						sl_glog.Printf("Diffuse    : %f\n", pod_node->alvLightingValues[0].fDiffuse);
					}

					if (pod_node->alvLightingValues[0].fSpecular != -1.0f)
					{
						sl_glog.Printf("Specular   : %f\n", pod_node->alvLightingValues[0].fSpecular);
					}

					if (pod_node->alvLightingValues[0].fOpacity != -1.0f)
					{
						sl_glog.Printf("Opacity    : %f\n", pod_node->alvLightingValues[0].fOpacity);
					}

					if (pod_node->alvLightingValues[0].fBumpiness != -1.0f)
					{
						sl_glog.Printf("Bumpiness  : %f\n\n\n", pod_node->alvLightingValues[0].fBumpiness);
					}
				}
			}
			else
			{
				// Dump the object faces
				for (u_i = 0; u_i < pod_node->uTextureFaceCount; u_i++)
				{
					// Display the texture faces
					sl_glog.Printf("Texture face %4d: x -> %4d, y -> %4d, z -> %4d\n", u_i, 
						pod_node->auv3TextureFace[u_i].X, pod_node->auv3TextureFace[u_i].Y, pod_node->auv3TextureFace[u_i].Z);
				}

				// Display the polygon face definitions
				sl_glog.Printf("Object materials: %d\n", (int) pod_node->uMaterialCount);

				// Dump the object faces
				for (u_i = 0; u_i < pod_node->uMaterialCount; u_i++)
				{
					// Display any materials which may be present.
					if (pod_node->astrTextureMap != 0)
					{
						if (pod_node->astrTextureMap[u_i] != 0)
						{
							sl_glog.Printf("Diffuse map: %s\n", pod_node->astrTextureMap[u_i]);

							// Display the mean color.
							if (pod_node->auv3MeanColor != 0)
							{
								sl_glog.Printf("Mean color : R -> %d, G -> %d, B -> %d\n", 
									pod_node->auv3MeanColor[u_i].Z, pod_node->auv3MeanColor[u_i].Y, pod_node->auv3MeanColor[u_i].X);
							}
						}
					}

					if (pod_node->astrOpacityMap != 0)
					{
						if (pod_node->astrOpacityMap[u_i] != 0)
						{
							sl_glog.Printf("Opacity map: %s\n", pod_node->astrOpacityMap[u_i]);
						}
					}

					if (pod_node->astrBumpMap != 0)
					{
						if (pod_node->astrBumpMap[u_i] != 0)
						{
							sl_glog.Printf("Bumpmap    : %s\n", pod_node->astrBumpMap[u_i]);
						}
					}

					if (pod_node->alvLightingValues != 0)
					{
						sl_glog.Printf("\n");
						if (pod_node->alvLightingValues[u_i].fAmbient != -1.0f)
						{
							sl_glog.Printf("Ambient    : %.3f\n", pod_node->alvLightingValues[u_i].fAmbient);
						}

						if (pod_node->alvLightingValues[u_i].fDiffuse != -1.0f)
						{
							sl_glog.Printf("Diffuse    : %.3f\n", pod_node->alvLightingValues[u_i].fDiffuse);
						}

						if (pod_node->alvLightingValues[u_i].fSpecular != -1.0f)
						{
							sl_glog.Printf("Specular   : %.3f\n", pod_node->alvLightingValues[u_i].fSpecular);
						}

						if (pod_node->alvLightingValues[u_i].fOpacity != -1.0f)
						{
							sl_glog.Printf("Opacity    : %.3f\n", pod_node->alvLightingValues[u_i].fOpacity);
						}

						if (pod_node->alvLightingValues[u_i].fBumpiness != -1.0f)
						{
							sl_glog.Printf("Bumpiness  : %.3f\n", pod_node->alvLightingValues[u_i].fBumpiness);
						}

						sl_glog.Printf("\n");
					}
				}
				sl_glog.Printf("\n");
			}
		}


		// Advance to the next node in the list
		pod_node = pod_node->podNext;
	}

	// All done.  Display the completion message and return. 
	sl_glog.Printf("----- Geometry file complete -----\n");
}