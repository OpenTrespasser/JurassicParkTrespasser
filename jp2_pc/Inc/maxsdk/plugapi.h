
/*******************************************************************
 *
 *    DESCRIPTION: DLL Plugin API
 *
 *    AUTHOR: Dan Silva
 *
 *    HISTORY: 11/30/94 Started coding
 *
 *******************************************************************/

#include "maxtypes.h"

#ifndef PLUGAPI_H_DEFINED


#define PLUGAPI_H_DEFINED

// 3DSMAX Version number to be compared against that returned by DLL's
// LibVersion function to detect obsolete DLLs.
//#define VERSION_3DSMAX 0x00006    // changed  4/1/96 -- DB
//#define VERSION_3DSMAX 100      // changed  4/9/96 -- DB
#define VERSION_3DSMAX 110      // changed  7/17/96 -- DS

// Internal super classes, that plug-in developers need not know about
#define GEN_MODAPP_CLASS_ID 		0x00000b
#define MODAPP_CLASS_ID 			0x00000c
#define OBREF_MODAPP_CLASS_ID 		0x00000d
#define BASENODE_CLASS_ID			0x000001
#define GEN_DERIVOB_CLASS_ID 		0x000002
#define DERIVOB_CLASS_ID 			0x000003
#define WSM_DERIVOB_CLASS_ID 		0x000004
#define PARAMETER_BLOCK_CLASS_ID 	0x000008	
#define EASE_LIST_CLASS_ID			0x000009
#define AXIS_DISPLAY_CLASS_ID		0x00000e
#define MULT_LIST_CLASS_ID			0x00000f
#define NOTETRACK_CLASS_ID			0x0000ff
#define TREE_VIEW_CLASS_ID			0xffffff00
#define SCENE_CLASS_ID				0xfffffd00
#define VIEWREF_CLASS_ID			0xffffff01

//-------------------------------------------
// Deformable object psuedo-super-class
#define DEFORM_OBJ_CLASS_ID 		0x000005

//-------------------------------------------
// Mappable object psuedo-super-class
#define MAPPABLE_OBJ_CLASS_ID 		0x000006

//-------------------------------------------
// Shape psuedo-super-class
#define GENERIC_SHAPE_CLASS_ID		0x0000ab

//-------------------------------------------
// Super-classes that are plugabble.
#define GEOMOBJECT_CLASS_ID			0x000010
#define CAMERA_CLASS_ID				0x000020
#define LIGHT_CLASS_ID				0x000030
#define SHAPE_CLASS_ID				0x000040
#define HELPER_CLASS_ID				0x000050
#define SYSTEM_CLASS_ID	 			0x000060 
#define REF_MAKER_CLASS_ID			0x000100 	
#define REF_TARGET_CLASS_ID	 		0x000200
#define OSM_CLASS_ID				0x000810
#define WSM_CLASS_ID				0x000820
#define WSM_OBJECT_CLASS_ID			0x000830
#define SCENE_IMPORT_CLASS_ID		0x000A10
#define SCENE_EXPORT_CLASS_ID		0x000A20
#define BMM_STORAGE_CLASS_ID		0x000B10
#define BMM_FILTER_CLASS_ID			0x000B20
#define BMM_IO_CLASS_ID				0x000B30
#define BMM_DITHER_CLASS_ID			0x000B40
#define BMM_COLORCUT_CLASS_ID		0x000B50
#define USERDATATYPE_CLASS_ID		0x000B60
#define MATERIAL_CLASS_ID			0x000C00    // Materials
#define TEXMAP_CLASS_ID				0x000C10    // Texture maps
#define UVGEN_CLASS_ID				0x0000C20   // UV Generator
#define XYZGEN_CLASS_ID				0x0000C30   // XYZ Generator
#define TEXOUTPUT_CLASS_ID			0x0000C40   // Texture output filter 
#define SOUNDOBJ_CLASS_ID			0x000D00
#define FLT_CLASS_ID				0x000E00
#define RENDERER_CLASS_ID			0x000F00
#define BEZFONT_LOADER_CLASS_ID		0x001000
#define ATMOSPHERIC_CLASS_ID		0x001010
#define UTILITY_CLASS_ID			0x001020	// Utility plug-ins

// Super-class ID's of various controls
#define	CTRL_SHORT_CLASS_ID 	   	0x9001
#define	CTRL_INTEGER_CLASS_ID		0x9002
#define	CTRL_FLOAT_CLASS_ID			0x9003
#define	CTRL_POINT2_CLASS_ID	   	0x9004
#define	CTRL_POINT3_CLASS_ID	   	0x9005
#define	CTRL_POS_CLASS_ID		   	0x9006
#define	CTRL_QUAT_CLASS_ID			0x9007
#define	CTRL_MATRIX3_CLASS_ID		0x9008
#define	CTRL_COLOR_CLASS_ID     	0x9009	// float color
#define	CTRL_COLOR24_CLASS_ID   	0x900A   // 24 bit color
#define	CTRL_POSITION_CLASS_ID		0x900B
#define	CTRL_ROTATION_CLASS_ID		0x900C
#define	CTRL_SCALE_CLASS_ID			0x900D
#define CTRL_MORPH_CLASS_ID			0x900E
#define CTRL_USERTYPE_CLASS_ID		0x900F  // User defined type

// Class ID's of built-in classes. The value is the first ULONG of the 
// 8 byte Class ID: the second ULONG is 0 for all built-in classes.
// NOTE: Only built-in classes should have the second ULONG == 0.

//--------------------- subclasses of GEOMOBJECT_CLASS_ID:

// Built into CORE
#define TRIOBJ_CLASS_ID 	 	0x0009	  
#define PATCHOBJ_CLASS_ID  		0x1030
#define NURBSOBJ_CLASS_ID		0x4135

// Primitives
#define BOXOBJ_CLASS_ID 		0x0010
#define SPHERE_CLASS_ID 		0x0011 
#define CYLINDER_CLASS_ID 		0x0012
#define CONE_CLASS_ID			0xa86c23dd
#define TORUS_CLASS_ID			0x0020
#define TUBE_CLASS_ID			0x7B21
#define HEDRA_CLASS_ID			0xf21c5e23

// The teapot is unique in that it uses both DWORDs in its class IDs
// Note that this is what 3rd party plug-ins SHOULD do.
#define TEAPOT_CLASS_ID1		0xACAD13D3
#define TEAPOT_CLASS_ID2		0xACAD26D9

#define PATCHGRID_CLASS_ID  	0x1070

// Particles
#define RAIN_CLASS_ID			0x9bd61aa0
#define SNOW_CLASS_ID			0x9bd61aa1

// Space Warp Objects
#define WAVEOBJ_CLASS_ID 		0x0013

// The basic lofter class
#define LOFTOBJ_CLASS_ID		0x1035
#define LOFT_DEFCURVE_CLASS_ID	0x1036

// Our implementation of the lofter
#define LOFT_GENERIC_CLASS_ID	0x10B0

#define TARGET_CLASS_ID  		0x1020  // should this be a helper?
#define MORPHOBJ_CLASS_ID		0x1021

// Subclasses of SHAPE_CLASS_ID
#define SPLINESHAPE_CLASS_ID 		0x00000a
#define LINEARSHAPE_CLASS_ID 		0x0000aa
#define SPLINE3D_CLASS_ID  		0x1040
#define NGON_CLASS_ID  			0x1050
#define DONUT_CLASS_ID  		0x1060
#define STAR_CLASS_ID			0x1995
#define RECTANGLE_CLASS_ID		0x1065
#define HELIX_CLASS_ID			0x1994
#define ELLIPSE_CLASS_ID		0x1097
#define CIRCLE_CLASS_ID			0x1999
#define TEXT_CLASS_ID			0x1993
#define ARC_CLASS_ID			0x1996

// subclasses of CAMERA_CLASS_ID:
#define SIMPLE_CAM_CLASS_ID  	0x1001
#define LOOKAT_CAM_CLASS_ID  	0x1002

// subclasses of LIGHT_CLASS_ID:
#define OMNI_LIGHT_CLASS_ID  	0x1011
#define SPOT_LIGHT_CLASS_ID  	0x1012
#define DIR_LIGHT_CLASS_ID  	0x1013
#define FSPOT_LIGHT_CLASS_ID  	0x1014

// subclasses of HELPER_CLASS_ID
#define DUMMY_CLASS_ID 			0x876234
#define BONE_CLASS_ID 			0x8a63c0
#define TAPEHELP_CLASS_ID 		0x02011
#define GRIDHELP_CLASS_ID		0x02010
#define POINTHELP_CLASS_ID		0x02013

//subclasses of UVGEN_CLASS_ID
#define STDUV_CLASS_ID 			0x0000100

//subclasses of XYZGEN_CLASS_ID
#define STDXYZ_CLASS_ID 		0x0000100

//subclasses of TEXOUT_CLASS_ID
#define STDTEXOUT_CLASS_ID 		0x0000100

// subclasses of MATERIAL_CLASS_ID	
#define CMTL_CLASS_ID 			0x0000100  // top-bottom material 
#define MULTI_CLASS_ID 			0x0000200  // multi material
#define DOUBLESIDED_CLASS_ID 	0x0000210  // double-sided mtl
#define MIXMAT_CLASS_ID 		0x0000250  // blend mtl

// subclasses of TEXMAP_CLASS_ID	
#define CHECKER_CLASS_ID 		0x0000200
#define MARBLE_CLASS_ID 		0x0000210
#define MASK_CLASS_ID 			0x0000220  // mask texture
#define MIX_CLASS_ID 			0x0000230
#define NOISE_CLASS_ID 			0x0000234
#define GRADIENT_CLASS_ID 		0x0000270
#define TINT_CLASS_ID 			0x0000224  // Tint texture
#define BMTEX_CLASS_ID 			0x0000240  // Bitmap texture
#define ACUBIC_CLASS_ID 		0x0000250  // Reflect/refract
#define MIRROR_CLASS_ID 		0x0000260  // Flat mirror
#define COMPOSITE_CLASS_ID 		0x0000280   // Composite texture

// subclasses of RENDERER_CLASS_ID		  
#define SREND_CLASS_ID 			0x000001 // default scan-line renderer

// subclasses of REF_MAKER_CLASS_ID			
#define MTL_LIB_CLASS_ID 		0x001111
#define MTLBASE_LIB_CLASS_ID 	0x003333
#define THE_SCENE_CLASS_ID   	0x002222
#define MEDIT_CLASS_ID 	 		0x000C80

// subclass of all classes
#define STANDIN_CLASS_ID   		0xffffffff  // subclass of all super classes

// Default material class
#define DMTL_CLASS_ID  		0x000002

// Default sound object
#define DEF_SOUNDOBJ_CLASS_ID	0x0000001

// Default atmosphere
#define FOG_CLASS_ID 0x10000001

//------------------ Modifier sub classes --------
#define SKEWOSM_CLASS_ID		0x6f3cc2aa
#define BENDOSM_CLASS_ID 			0x00010
#define TAPEROSM_CLASS_ID 			0x00020
#define TWISTOSM_CLASS_ID 			0x00090

#define UVWMAPOSM_CLASS_ID			0xf72b1
#define SELECTOSM_CLASS_ID			0xf8611
#define MATERIALOSM_CLASS_ID		0xf8612
#define SMOOTHOSM_CLASS_ID			0xf8613
#define NORMALOSM_CLASS_ID			0xf8614
#define OPTIMIZEOSM_CLASS_ID		0xc4d31

#define EXTRUDEOSM_CLASS_ID 		0x000A0
#define SURFREVOSM_CLASS_ID 		0x000B0

#define DISPLACEOSM_CLASS_ID		0xc4d32
#define DISPLACE_OBJECT_CLASS_ID	0xe5240
#define DISPLACE_WSM_CLASS_ID		0xe5241

#define SINEWAVE_OBJECT_CLASS_ID 	0x00030
#define SINEWAVE_CLASS_ID 			0x00040
#define SINEWAVE_OMOD_CLASS_ID 		0x00045
#define LINWAVE_OBJECT_CLASS_ID 	0x00035
#define LINWAVE_CLASS_ID 			0x00042
#define LINWAVE_OMOD_CLASS_ID 		0x00047

#define GRAVITYOBJECT_CLASS_ID		0xe523c
#define GRAVITYMOD_CLASS_ID			0xe523d
#define WINDOBJECT_CLASS_ID			0xe523e
#define WINDMOD_CLASS_ID			0xe523f

#define DEFLECTOBJECT_CLASS_ID		0xe5242
#define DEFLECTMOD_CLASS_ID			0xe5243

#define BOMB_OBJECT_CLASS_ID 		0xf2e32
#define BOMB_CLASS_ID 				0xf2e42


//------------------ Controller sub classes --------
#define LININTERP_FLOAT_CLASS_ID 			0x2001
#define LININTERP_POSITION_CLASS_ID 		0x2002
#define LININTERP_ROTATION_CLASS_ID 		0x2003
#define LININTERP_SCALE_CLASS_ID			0x2004
#define PRS_CONTROL_CLASS_ID				0x2005
#define LOOKAT_CONTROL_CLASS_ID				0x2006				

#define HYBRIDINTERP_FLOAT_CLASS_ID 		0x2007
#define HYBRIDINTERP_POSITION_CLASS_ID 		0x2008
#define HYBRIDINTERP_ROTATION_CLASS_ID 		0x2009
#define HYBRIDINTERP_POINT3_CLASS_ID		0x200A
#define HYBRIDINTERP_SCALE_CLASS_ID			0x2010
#define HYBRIDINTERP_COLOR_CLASS_ID			0x2011

#define TCBINTERP_FLOAT_CLASS_ID 			0x442311
#define TCBINTERP_POSITION_CLASS_ID 		0x442312
#define TCBINTERP_ROTATION_CLASS_ID 		0x442313
#define TCBINTERP_POINT3_CLASS_ID			0x442314
#define TCBINTERP_SCALE_CLASS_ID			0x442315


//--------------------------------------------------


class ISave;
class ILoad;
class Interface;

// System keepsa list of the DLL's found on startup.
// This is the interface to a single class
class ClassDesc {
	public:
		virtual int				IsPublic()=0;  // Show this in create branch?
		virtual void *			Create(BOOL loading=FALSE)=0;   // return a pointer to an instance of the class.
		virtual	int 			BeginCreate(Interface *i) {return 0;}
		virtual int 			EndCreate(Interface *i) {return 0;};
		virtual const TCHAR* 	ClassName()=0;
		virtual SClass_ID		SuperClassID()=0;
		virtual Class_ID		ClassID()=0;
		virtual const TCHAR* 	Category()=0;   // primitive/spline/loft/ etc
		virtual BOOL			OkToCreate(Interface *i) { return TRUE; }	// return FALSE to disable create button		
		virtual BOOL			HasClassParams() {return FALSE;}
		virtual void			EditClassParams(HWND hParent) {}
		virtual void			ResetClassParams(BOOL fileReset=FALSE) {}
		// Class IO
		virtual BOOL			NeedsToSave() { return FALSE; }
		virtual IOResult 		Save(ISave *isave) { return IO_OK; }
		virtual IOResult 		Load(ILoad *iload) { return IO_OK; }

	};

// Create instance of the specified class
CoreExport void *CreateInstance(SClass_ID superID, Class_ID classID);

#endif
