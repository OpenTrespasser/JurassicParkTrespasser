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
 * $Log:: /JP2_PC/Source/Lib/Groff/Groff.hpp                                                  $
 * 
 * 17    5/18/98 12:53p Mlange
 * Added timing stat for pgonFindObjects().
 * 
 * 16    5/15/98 7:19p Mlange
 * They call me....the Code Nazi!
 * 
 * 15    98.05.08 1:20p Mmouni
 * Added support for loading objects without materials and without textures.
 * 
 * 14    98.05.04 10:27p Mmouni
 * Added support for loading pivot points for each mesh.
 * 
 * 13    98.04.23 6:51p Mmouni
 * Added support for loading version 13 GROFFs (with wrap in GROFF).
 * 
 * 12    10/22/97 1:34a Gstull
 * Changes to support version 12 of the GROFF file structure definitions.  This mainly includes
 * support for text attributes for use with the attribute compiler.
 * 
 * 11    6/09/97 4:39p Agrant
 * bDoneLoading flag to allow delayed loading of some objects.
 * 
 * 10    5/15/97 3:02p Agrant
 * added a value table handle to the groffobjectname class
 *
 *********************************************************************************************/

#ifndef HEADER_LIB_GROFF_GROFF_HPP
#define HEADER_LIB_GROFF_GROFF_HPP


#ifdef USE_MAX_TYPES

#include "StandardTypes.hpp"

#else

#include "Common.hpp"


// Define the floating point 3D vector
typedef struct 
{
	float	X;
	float	Y;
	float	Z;
} fvector3;


// Define the integer 3D vector
typedef struct 
{
	uint	X;
	uint	Y;
	uint	Z;
} uvector3;

// Define the floating point 2D vector
typedef struct 
{
	float	X;
	float	Y;
} fvector2;


// Define the integer 2D vector
typedef struct 
{
	uint	X;
	uint	Y;
} uvector2;

#endif

#include "Lib/Sys/SysLog.hpp"
#include "Lib/Sys/SmartBuffer.hpp"
#include "Lib/Groff/FileIO.hpp"
#include "Lib/Groff/ObjectHandle.hpp"


// Define the Groff file version number.
//
// Version number:
//   10 - Initial release.
//   11 - Removed hack to load bump map files.
//   12 - Added support for attribute section handles.
//   13 - Added wrap to mesh data.
//   14 - Added pivot point to mesh data.
//   15 - Added non-textured materials.
//

#define GROFF_VERSION_ID 15


// Define the flags which describe the contents of a section.
#define	gHEADER			0x00000001		// Groff file header section.
#define	gREGION			0x00000002		// Spatial definition of a region
#define	gOBJECT			0x00000004		// Definition of an object
#define gGEOMETRY		0x00000008		// Geometric definition of an object.
#define gMAPPING		0x00000010		// Mapping definition.
#define gMATERIAL		0x00000020		// Material definition.
#define gBITMAP   		0x00000040		// Bitmap definition.           
#define gPALETTE     	0x00000080		// Palette definition.
#define gPHYSICS		0x00000100		// Physics information.
#define gAI				0x00000200		// AI information.
#define gSOUND			0x00000400		// Sound information.
#define gDESIGN 		0x00000800		// Design daemon information.
#define gTRIGGER		0x00001000		// Trigger information.
#define gATTRIBUTES		0x00002000		// Object attribute information.
#define gSPECIAL		0x80000000		// Special container for whatever


//**********************************************************************************************
//
class CGroffObjectConfig
//
// Define a struct which specifies the position, rotation and scale of an object.
//
// Prefix: goc
//
//**************************************
{
public:
	TSectionHandle sehObject;			// The object definition section handle.
	TSymbolHandle  syhObjectName;		// The object symbol name handle.

	fvector3	   fv3Position;			// The location of the object in the world.
	fvector3	   fv3Rotation;			// The object orientation in Euler angles.
	float		   fScale;				// The scaling factor for the object.

	/* Version 12 - GROFF file format changes. */
	::CHandle		   hAttributeHandle;	// A handle to the object value container.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CGroffObjectConfig();

	~CGroffObjectConfig();
};


//**********************************************************************************************
//
class CGroffRegion
//
// Define the structure for a region within the model. 
//
// Prefix: gr
//
//**************************************
{
public:
	uint				uObjectCount;			// The number of objects in this scene.
	CGroffObjectConfig*	agocObjectList;			// The object handle list.

	/* Version 12 - GROFF file format changes. */
	TSectionHandle		sehAttributeSection;	// Handle to the GROFF section containing the
												// attribute value table.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffRegion();

	CGroffRegion(uint u_object_count);

	~CGroffRegion();

	//******************************************************************************************
	//
	// Member functions.
	//

	bool bConfigure();
};


//**********************************************************************************************
//
class CGroffObjectName
//
// Prefix: gon
//
//**************************************
{
public:

	/* Version 11 fields */
	char*		   strObjectName;			// The object name.
	TSectionHandle sehObjectSection;		// Object definition section handle.
	fvector3	   fv3Position;				// Object world position.
	fvector3	   fv3Rotation;				// Object orientation.
	float		   fScale;					// Unit cube scale value.
	
	/* Version 12 - GROFF file format changes. */
	::CHandle		   hAttributeHandle;		// Handle to object base-value container in value table.
	bool		   bDoneLoading;			// True when the object has correctly loaded.  This
											// really does not belong in this structure!!!

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CGroffObjectName();

	~CGroffObjectName();


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	bool bAddObject
	(
		TSectionHandle seh_handle, 
		const char*	   str_object_name,
		fvector3	   fv3_position,
		fvector3	   fv3_rotation,
		float		   f_scale,
		::CHandle		   h_handle				// Attribute handle.	(Version 12 Changes)
	);
	//
	// Returns:
	//
	//**************************************
};


//**********************************************************************************************
//
class CGroffObjectInfo
//
// Prefix: goi
//
//**************************************
{
public:
	uint			  uObjectCount;
	CGroffObjectName* gonObjectList;

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CGroffObjectInfo();

	~CGroffObjectInfo();


	//******************************************************************************************
	//
	// Member functions.
	//

	void Initialize();
	bool bConfigure();
	void Destroy();

	//******************************************************************************************
	//
    bool bAddObject
	(
		uint			u_index, 
		TSectionHandle	seh_handle, 
		const char*		str_object_name,
		fvector3		fv3_position,
		fvector3		fv3_rotation,
		float			f_scale,
		::CHandle			h_handle			// Attribute handle.	(Version 12 Changes)
	);
	//
	// Returns:
	//
	//**************************************


	//******************************************************************************************
	//
	CGroffObjectName* pgonFindObject
	(
		const char* object_name
	) const;
	//
	// Returns:
	//
	//**************************************


#if VER_TEST
	//******************************************************************************************
	//
	static float fSecsInFindObject();
	//
	// Returns:
	//		The number of seconds the processor spent in 'pgonFindObject' since the last call
	//		of this function.
	//
	//**************************************
#endif
};


//**********************************************************************************************
//
class CGroffObject
//
// Define the structure to handle objects.
//
// Prefix: go
//
//**************************************
{
public:
	TSymbolHandle  syhObjectName;		// Handle of the name of the object
	
	TSectionHandle sehGeometry;			// Geometry handle.
	TSectionHandle sehMapping;			// Material mapping handle.
	TSectionHandle sehPhysics;			// Physics handle.
	TSectionHandle sehAI;				// AI handle.
	TSectionHandle sehSound;			// Sound handle.
	TSectionHandle sehSpecial;			// Special handle.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffObject();
	~CGroffObject();

	//******************************************************************************************
	//
	// Member functions.
	//

	void Initialize();
};


//**********************************************************************************************
//
class CGroffHeader
//
// Define a structure which is used to represent a GROFF header.
//
// Prefix: gh
//
//**************************************
{
public:
	uint	uMagicNumber;		// The magic number which identifies the file.
	uint	uVersion;			// The version number of this structure format.

	CGroffHeader();		
	~CGroffHeader();
};


//**********************************************************************************************
//
class CGroffGeometry
//
// Define the structure for a geometry section.
//
// Prefix: gg
//
//**************************************
{
public:
	fvector3	fv3PivotOffset;		// Offet (in local space) to pivot point of mesh.

	uint		uVertexCount;		// The number of vertices in this object.
	uint		uFaceCount;			// The number of faces in this object.
	uint		uVertexNormalCount;	// The number of vertex normals in the object.
	uint		uWrapVertexCount;	// The number of wrap vertices for this object.
	uvector3	uv3DefaultColor;	// The default color of an object.

	fvector3*	afv3Vertex;			// List of vertices.
	uvector3*	auv3Face;			// List of faces.
	fvector3*	afv3FaceNormal;		// List of face normals.
	fvector3*   afv3VertexNormal;	// List of vertex normals.
	fvector3*	afv3WrapVertex;		// List of wrap vertices.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffGeometry();	
	~CGroffGeometry();

	//******************************************************************************************
	//
	// Member functions.
	//

	void Initialize();
	bool bConfigure();
};


//**********************************************************************************************
//
class CGroffMapping
//
// Define the structure to handle the mapping information.
//
// Prefix: gmap
//
//**************************************
{
public:
	TSectionHandle  sehMaterial;		// Handle to the material for this object.
	uint			uTVertexCount;		// The number of texture vertices.
	uint			uTFaceCount;		// The number of texture faces.
	
	fvector2*		afv2TVertex;		// List of texture vertices.
	uvector3*		auv3TFace;			// List of texture faces.
	uint*			auTFaceMaterialID;	// List of material indices.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffMapping();
	~CGroffMapping();

	//******************************************************************************************
	//
	// Member functions.
	//

	void Initialize();
	bool bConfigure();
};


//**********************************************************************************************
//
class CGroffMaterial
//
// Define the structure to handle materials.
//
// Prefix: gmat
//
//**************************************
{
public:
	uint			uTextureCount;		// The number of materials.

	TSymbolHandle*	asyhTextureHandle;	// List of texture map handles.
	TSymbolHandle*	asyhOpacityHandle;	// List of opacity map handles.
	TSymbolHandle*	asyhBumpHandle;		// List of bump map handles.

	uvector3*		auv3MaterialColor;	// Flat shaded color for material.

	float			fAmbient;			// The default ambient component.
	float			fDiffuse;			// The default diffuse component.
	float			fSpecular;			// The default specular component.
	float			fOpacity;			// The default opacity component.
	float			fBumpiness;			// The default bumpiness component.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffMaterial();
	~CGroffMaterial();

	//******************************************************************************************
	//
	// Member functions.
	//

	void Initialize();
	bool bConfigure();
};
	

//**********************************************************************************************
//
class CGroffMaterialName
//
// Define the structure to handle material names.
//
// Prefix: gmatn
//
//**************************************
{
public:
	uint		uTextureCount;

	char**		strTextureMap;
	char**		strOpacityMap;
	char**		strBumpMap;

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffMaterialName();
	~CGroffMaterialName();

	//******************************************************************************************
	//
	// Member functions.
	//
	void Initialize();

	bool bBuildNames(CFileIO& fio_file, CGroffMaterial& gmat_material);
	bool bBuildHandles(CFileIO& fio_file, CGroffMaterial& gmat_material);

	const char* strTextureName(uint u_index);
	const char* strOpacityName(uint u_index);
	const char* strBumpName(uint u_index);
};


//**********************************************************************************************
//
class CGroffBitmap
//
// Define the structure to handle bitmaps.
//
// Prefix: gbm
//
//**************************************
{
public:
	TSymbolHandle	syhBitmapName;	// Bitmap name.
	uint			uGroupID;		// Bitmap group.
	uvector3		uv3MeanColor;	// The mean color for the bitmap.
	uint			uWidth;			// Bitmap width in pixels.
	uint			uHeight;		// Bitmap height in pixels.
	uint			uDepth;			// Bitmap depth in bits.
	TSectionHandle	sehPalette;		// Palette handle if a paletted.
	uint			uCRC;			// CRC value for bitmap
	uint8*			au1Data;		// Image data.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffBitmap();
	~CGroffBitmap();

	//******************************************************************************************
	//
	// Member functions.
	//
	void Initialize();
	bool bConfigure();
};
	

//**********************************************************************************************
//
class CGroffPalette
//
// Define the structure to handle bitmap palettes.
//
// Prefix: gpal
//
//**************************************
{
public:
	TSymbolHandle	syhPaletteName;	// Palette name.
	
	uint			uPaletteCount;	// Number of bytes in this section.
	uvector3*		auv3Palette;	// List of raw data.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffPalette();
	~CGroffPalette();

	//******************************************************************************************
	//
	// Member functions.
	//
	void Initialize();
	bool bConfigure();
};


//**********************************************************************************************
//
class CGroffPhysics
//
// Define the structure to handle the physics system information.
//
// Prefix: gph
//
//**************************************
{
	TSectionHandle	sehSkeleton;	// Handle to the skeleton object.

	uint			uByteCount;		// Physics attribute list size.
	uint8*			pu1Buffer;		// Physics model parameters.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffPhysics(); 
	~CGroffPhysics();

	void Initialize();
	bool bConfigure();
};


//**********************************************************************************************
//
class CGroffSkeleton
//
// Define the structure to handle the physics system information.
//
// Prefix: gsk
//
//**************************************
{
	TSectionHandle	sehGeometry;		// Handle to the geometry section.
	uint			uJointCount;		// The number of joints in the model.
	fvector3*		afv3Joints;			// The absolute joint location.
	uint			uVertexCount;		// The number of vertices in the model. 
	uint*			uVertexJointList;	// Mapping list of vertices and joints.

	uint			uByteCount;			// Number of bytes in this section.
	uint8*			pu1Buffer;			// List of raw data.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffSkeleton(); 
	~CGroffSkeleton();

	//******************************************************************************************
	//
	// Member functions.
	//
	void Initialize();
	bool bConfigure();
};


//**********************************************************************************************
//
class CGroffAI
//
// Define the structure to handle the AI system information.
//
// Prefix: gai
//
//**************************************
{
public:
	uint	uByteCount;	// Number of bytes in this section.
	uint8*	pu1Buffer;	// List of raw data.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffAI();
	~CGroffAI();
			
	//******************************************************************************************
	//
	// Member functions.
	//
	void Initialize();
	bool bConfigure();
};


//**********************************************************************************************
//
class CGroffSound
//
// Define the structure to handle the sound system information.
//
// Prefix: snd
//
//**************************************
{
public:
	uint	uByteCount;	// Number of bytes in this section.
	uint8*	pu1Buffer;	// List of raw data.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffSound(); 
	~CGroffSound();

	//******************************************************************************************
	//
	// Member functions.
	//
	void Initialize();
	bool bConfigure();
};


//**********************************************************************************************
//
class CGroffDesign
//
// Define the structure to handle the game system information.
//
// Prefix: gdd
//
//**************************************
{
public:
	uint	uByteCount;	// Number of bytes in this section.
	uint8*	pu1Buffer;	// List of raw data.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffDesign();
	~CGroffDesign();

	//******************************************************************************************
	//
	// Member functions.
	//
	void Initialize();
	bool bConfigure();
};


//**********************************************************************************************
//
class CGroffTrigger
//
// Define the structure to handle the trigger system information.
//
// Prefix: tg
//
//**************************************
{
public:
	uint	uByteCount;	// Number of bytes in this section.
	uint8*	pu1Buffer;	// List of raw data.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffTrigger(); 
	~CGroffTrigger();

	//******************************************************************************************
	//
	// Member functions.
	//
	void Initialize();
	bool bConfigure();
};


//**********************************************************************************************
//
class CGroffSpecial
//
// Define the structure to handle the user specific information.
//
// Prefix: gs
//
//**************************************
{
public:
	uint	uByteCount;	// Number of bytes in this section.
	uint8*	pu1Buffer;	// List of raw data.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CGroffSpecial();
	~CGroffSpecial();

	//******************************************************************************************
	//
	// Member functions.
	//
	void Initialize();
	bool bConfigure();
};

#endif
