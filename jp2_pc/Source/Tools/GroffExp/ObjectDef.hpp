/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1996-1997
 *
 * Contents: Class providing support for nodes which represent individual objects in a scene.
 * 
 * Bugs:
 *
 * To do:
 * 
 *    1. Replace all dynamic memory allocation routines with STL.
 *
 *    2. Remove ObjectDefList class as STL can easily replace it.
 *
 *    3. Convert all internal types to use Max types and provide conversion routines for the
 *       Trespasser data structure types.
 *
 *    4. Replace all string references with CEasyStrings.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/ObjectDef.hpp                                         $
 * 
 * 9     7/15/97 6:58p Gstull
 * Made organizational changes to make management of the exporter more reasonable.
 * 
 * 8     7/14/97 3:36p Gstull
 * Made substantial changes to support tasks for the August 18th milestone for attributes,
 * object representation, geometry checking, dialog interfacing, logfile support, string tables
 * and string support.
 * 
 * 7     6/18/97 7:33p Gstull
 * Added changes to support fast exporting.
 * 
 * 6     3/19/97 6:10p Gstull
 * Added fixes for Bug #18 and bug #19, which resolved the problems of exporting objects without
 * geometry and material ID range errors.
 *  
 * 
 * 5     2/21/97 6:55p Gstull
 * Added code to perform a number of additional checks   
 * on data as well as a new GUI interface class, removal 
 * of the bump map hack.
 * 
 * 4     12/16/96 11:22a Gstull
 * Made changes to support quantization to a single palette.
 * 
 * 3     11/20/96 1:13p Gstull
 * Slight modifications to the Groff structures for integration into the GUIApp.
 * 
 * 2     11/15/96 11:08a Gstull
 * Added substantial changes to the GroffExporter including for support of mutiple section
 * files.
 * 
 * 1     9/16/96 3:53p Gstull
 *
 **********************************************************************************************/

#ifndef HEADER_TOOLS_GROFFEXP_OBJECTDEF_HPP
#define HEADER_TOOLS_GROFFEXP_OBJECTDEF_HPP

#include "Max.h"

#include "StandardTypes.hpp"
#include "Lib/Groff/EasyString.hpp"


//**********************************************************************************************
//
struct SLightingValues
// Prefix: lv
{
	float fAmbient;		// Default ambient value.
	float fDiffuse;		// Default diffuse value.
	float fSpecular;	// Default specular value.
	float fOpacity;		// Default opacity value.
	float fBumpiness;	// Default bumpiness value.
};	


//**********************************************************************************************
//
class CObjectDef
//
// Prefix: od
//
// This class provides the functions and internal data structures to manage and object which 
// exists in the scene.
//
{
private:

	//******************************************************************************************
	// Placement information.
	//******************************************************************************************
	//
	Point3				p3Position;					// Object position.
	Quat				qRotation;					// Object orientation quaternion.   (Master)
	Point3				p3Rotation;					// Object orientation Euler angles. (Slave)
	float				fUniformScale;				// Object uniform scaling factor.
	Point3				p3PivotOffset;				// Object center of mass.
	Quat				qPivotRotation;				// Object object-offset orientation.
	Box3				bx3BoundingBox;				// Object bounding box;

	Matrix3				m3ObjectTM;					// Object TM.
	Matrix3				m3NodeTM;					// Node TM.
	Matrix3				m3ParentTM;					// Parent TM.
	Matrix3				m3RelativeTM;				// Node-parent relative TM.
	Matrix3				m3TMBeforeWSM;				// Object TM before WSM.
	Matrix3				m3TMAfterWSM;				// Object TM after WSM.

public:
	
	//******************************************************************************************
	//
	CObjectDef*			podNext;					// Link to the next object.


	//******************************************************************************************
	// 
	int					iID;						// The ID number of this node.
	CEasyString			estrObjectName;				// Object name.


	//******************************************************************************************
	// Geometric information.
	uint				uVertexCount;				// Number of vertices.
	fvector3*			afv3Vertex;					// Vertex list.

	uint				uFaceCount;					// Number of faces.
	uvector3*			auv3Face;					// Face list.
	uint*				auFaceMaterialIndex;		// Texture face material index

	uint				uFaceNormalCount;			// Number of face normals.
	fvector3*			afv3FaceNormal;				// Face normal list.

	uint				uVertexNormalCount;			// Number of vertex normals.
	fvector3*			afv3VertexNormal;			// Vertex normal list.


	//******************************************************************************************
	// Material mapping information.
	uint				uTextureVertexCount;		// Number of texture vertices
	fvector2*			afv2TextureVertex;			// Texture vertex list

	uint				uTextureFaceCount;			// Number of texture faces
	uvector3*			auv3TextureFace;			// Texture face list


	//******************************************************************************************
	// Material information.
	uint				uMaterialCount;				// Number of materials in object

	char**				astrTextureMap;				// Texture map list.
	char**				astrOpacityMap;				// Opacity map list.
	char**				astrBumpMap;				// Bump map list.
	uvector3			uv3DefaultColor;			// Wire color for a geometry.
	uvector3*			auv3MeanColor;				// Mean color of each texture map.
	SLightingValues*	alvLightingValues;			// Per texture lighting values.


	//******************************************************************************************
	// Physics information.
    char				strSkeletonName[256];		// Name of the physics skeleton.
	uint				uJointCount;				// The number of joints in the skeleton.
	fvector3			afv3Joints;					// The location of the joints.
	uint*				auVertexJointList;			// Vertex joints mapping list.
	

	//******************************************************************************************
	// Error reporting information.
	uint				uWarnings;					// Number of warnings generated
	uint				uErrors;					// Number of errors generated


	//******************************************************************************************
	//	
	// Member functions.
	//


	//******************************************************************************************
	//
	CObjectDef
	(
		const char *str_object_name
	);

	
	//******************************************************************************************
	//
	~CObjectDef
	(
	);
	

	//******************************************************************************************
	//
	void Destroy
	(
	);


	//******************************************************************************************
	//
	// Methods for accessing object placement and TM information.
	//

	//******************************************************************************************
	//
	void Position
	(
		Point3& p3_position
	);


	//******************************************************************************************
	//
	Point3& p3GetPosition
	(
	);


	//******************************************************************************************
	//
	void Rotation
	(
		Point3& p3_rotation
	);


	//******************************************************************************************
	//
	Point3& p3GetRotation
	(
	);


	//******************************************************************************************
	//
	void Rotation
	(
		Quat& q_rotation
	);


	//******************************************************************************************
	//
	Quat& qGetRotation
	(
	);


	//******************************************************************************************
	//
	void Scale
	(
		float f_scale_value
	);


	//******************************************************************************************
	//
	float fGetScale
	(
	);


	//******************************************************************************************
	//
	void BoundingBox
	(
		Box3& bx3_bounding_box
	);


	//******************************************************************************************
	//
	Box3& bx3GetBoundingBox
	(
	);


	//******************************************************************************************
	//
	void TMBeforeWSM
	(
		Matrix3& m3_matrix
	);


	//******************************************************************************************
	//
	Matrix3& m3GetTMBeforeWSM
	(
	);


	//******************************************************************************************
	//
	void TMAfterWSM
	(
		Matrix3& m3_matrix
	);


	//******************************************************************************************
	//
	Matrix3& m3GetTMAfterWSM
	(
	);


	//******************************************************************************************
	//
	void ObjectTM
	(
		Matrix3& m3_matrix
	);


	//******************************************************************************************
	//
	Matrix3& m3GetObjectTM
	(
	);


	//******************************************************************************************
	//
	void NodeTM
	(
		Matrix3& m3_matrix
	);


	//******************************************************************************************
	//
	Matrix3& m3GetNodeTM
	(
	);


	//******************************************************************************************
	//
	void ParentTM
	(
		Matrix3& m3_matrix
	);


	//******************************************************************************************
	//
	Matrix3& m3GetParentTM
	(
	);


	//******************************************************************************************
	//
	void RelativeTM
	(
		Matrix3& m3_matrix
	);


	//******************************************************************************************
	//
	Matrix3& m3GetRelativeTM
	(
	);


	//******************************************************************************************
	//
	void PivotOffset
	(
		Point3& p3_pivot_offset
	);


	//******************************************************************************************
	//
	Point3& p3GetPivotOffset
	(
	);


	//******************************************************************************************
	//
	void PivotRotation
	(
		Quat& q_pivot_rotation
	);


	//******************************************************************************************
	//
	Quat& qGetPivotRotation
	(
	);


	//******************************************************************************************
	//
	// Methods for geometric information.
	//


	//******************************************************************************************
	//
	bool VertexCount
	(
		uint u_vertex_count
	);


	//******************************************************************************************
	//
	bool Vertex
	(
		uint  u_vertex_index, 
		float f_x,
		float f_y,
		float f_z
	);


	//******************************************************************************************
	//
	bool FaceCount
	(
		uint u_face_count
	);


	//******************************************************************************************
	//
	bool Face
	(
		uint u_face_index, 
		uint u_v0, 
		uint u_v1,
		uint u_v2
	);


	//******************************************************************************************
	//
	bool VertexNormalCount
	(
		uint u_face_count
	);


	//******************************************************************************************
	//
	bool VertexNormal
	(
		uint  u_face_index, 
		uint  u_vertex_index, 
		float f_x, 
		float f_y, 
		float f_z
	);


	//******************************************************************************************
	//
	bool FaceNormalCount
	(
		uint u_face_count
	);


	//******************************************************************************************
	//
	bool FaceNormal
	(
		uint  u_face_index, 
		float f_x, 
		float f_y,
		float f_z
	);


	//******************************************************************************************
	//
	// Methods for material mapping information.
	//


	//******************************************************************************************
	//
	bool TextureVertexCount
	(
		uint u_texture_vertex_count
	);


	//******************************************************************************************
	//
	bool TextureVertex
	(
		uint u_texture_vertex_index, 
		float f_u, 
		float f_v
	);


	//******************************************************************************************
	//
	bool TextureFaceCount
	(
		uint u_face_count
	);


	//******************************************************************************************
	//
	bool TextureFace
	(
		uint u_face_index, 
		uint u_v0, 
		uint u_v1, 
		uint u_v2
	);


	//******************************************************************************************
	//
	void LightingValues
	(
		uint			 u_material_index, 
		SLightingValues* plv_lighting_values
	);


	//******************************************************************************************
	//
	void MeanColor
	(
		uint  u_material_index, 
		uint8 u1_blue, 
		uint8 u1_green,
		uint8 u1_red
	);


	//******************************************************************************************
	//
	void DefaultColor(uint8 u1_blue, uint8 u1_green, uint8 u1_red);


	//******************************************************************************************
	//
	// Methods for material definition information.
	//


	//******************************************************************************************
	//
	bool FaceMaterialIndex
	(
		uint u_face_index, 
		uint u_material_index
	);


	//******************************************************************************************
	//
	bool MaterialCount
	(
		uint u_material_count
	);


	//******************************************************************************************
	//
	bool TextureName
	(
		uint		u_material_index, 
		const char* str_mapname
	);


	//******************************************************************************************
	//
	bool OpacityName
	(
		uint		u_material_index, 
		const char* str_mapname
	);


	//******************************************************************************************
	//
	bool BumpmapName
	(
		uint u_material_index, 
		const char* str_mapname
	);


	//******************************************************************************************
	//
	// Methods for skeletal information.
	//

	//******************************************************************************************
	//
	bool bSkeletonName
	(
		const char* str_skeleton_name
	);


	//******************************************************************************************
	//
	bool bJointCount
	(
		uint u_joint_count
	);


	//******************************************************************************************
	//
	bool bJoint
	(
		uint u_index, 
		float f_x, 
		float f_y, 
		float f_z
	);


	//******************************************************************************************
	//
	bool bVertexJoint
	(
		uint u_vertex_index, 
		uint u_joint_index
	);


	//******************************************************************************************
	//
	void Dump
	(
	);
};


//**********************************************************************************************
// This class keeps track of the list of all object definitions in the scene.
//
class CObjectDefList
//
// Prefix: odl
//
{
public:

	CObjectDef*	podHead;
	CObjectDef*	podTail;

	uint		uNodeCount;


	//******************************************************************************************
	//
	// Constructors and destructors.
	//

	//******************************************************************************************
	//
	CObjectDefList
	(
	);
	
	
	//******************************************************************************************
	//
	~CObjectDefList
	(
	);


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	CObjectDef* podAddObject
	(
		const char* str_object_name
	);


	//******************************************************************************************
	void DeleteObjects
	(
	);

	
	//******************************************************************************************
	//
	CObjectDef* podLookup
	(
		uint u_object_index
	);
	
	
	//******************************************************************************************
	//
	CObjectDef* podLookup
	(
		const char* str_object_name
	);
	

	//******************************************************************************************
	//
	uint uObjectCount
	(
	)
	{
		return uNodeCount; 
	};


	//******************************************************************************************
	//
	uint uWarnings
	(
	);
	

	//******************************************************************************************
	//
	uint uErrors
	(
	);


	//******************************************************************************************
	void Dump
	(
	);
};

#endif