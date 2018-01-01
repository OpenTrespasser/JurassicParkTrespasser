/**********************************************************************
 *<
	FILE: imtl.h

	DESCRIPTION: Renderer materials

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __IMTL__H
#define __IMTL__H


#include <plugapi.h>

//#include "gport.h"	
#include "custcont.h" 

#define AXIS_UV 0
#define AXIS_VW 1
#define AXIS_WU 2

// Values for SymFlags:
#define U_WRAP   (1<<0)
#define V_WRAP   (1<<1)  
#define U_MIRROR (1<<2)
#define V_MIRROR (1<<3)

#define	X_AXIS 0
#define	Y_AXIS 1
#define	Z_AXIS 2

static inline float Intens(const AColor& c) {	return (c.r+c.g+c.b)/3.0f;	}
static inline float Intens(const Color& c) {	return (c.r+c.g+c.b)/3.0f;	}

// Meta-materials post this message to the MtlEditor when user
// clicks on a sub-material button.
#define WM_SUB_MTL_BUTTON	WM_USER + 0x04001

// Materials or Texture maps post this message to the MtlEditor when user
// clicks on a texture map button.
#define WM_TEXMAP_BUTTON	WM_USER + 0x04002

class ShadeContext;
class Bitmap;
class RenderMapsContext; 


class TexHandle {
	public:
		virtual DWORD GetHandle() = 0;
		virtual void DeleteThis() = 0;
	};		

class TexHandleMaker{
	public: 
		// choice of two ways to create a texture handle.
		// From a 3DSMax Bitmap
		virtual TexHandle* CreateHandle(Bitmap *bm, int symflags=0)=0;
		// From a 32 bit DIB
		virtual TexHandle* CreateHandle(BITMAPINFO *bminf, int symflags=0)=0;

		// This tells you the size desired of the bitmap. It ultimately
		// needs a square bitmap that is a power of 2 in width and height.
		// If you already have a bitmap around, just pass it in to CreateHandle
		// and it will be converted.  If you are making a bitmap from scratch
		// (i.e.) a procedural texture, then you should make it Size() in 
		// width in height, and save us an extra step.  In either case
		// You own your bitmap, and are responsible for ultimately freeing it.

		virtual int Size()=0;
	};


// Interface that is passed in to the Mtl or Texture Map when it is in the mtl
// editor.
class IMtlParams {
	public:
	// call after mouse up's in mtls params
	// It causes the viewports to be redrawn.
	virtual void MtlChanged()=0;  

	// Adds rollup pages to the Material Params. Returns the window
	// handle of the dialog that makes up the page.
	virtual HWND AddRollupPage( HINSTANCE hInst, TCHAR *dlgTemplate, 
		DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0 )=0;

	// Removes a rollup page and destroys it.  When a dialog is destroyed
	// it need not delete all its rollup pages: the Mtl Editor will do
	// this for it, and it is more efficient.
	virtual void DeleteRollupPage( HWND hRollup )=0;

	// When the user mouses down in dead area, the plug-in should pass
	// mouse messages to this function which will pass them on to the rollup.
	virtual void RollupMouseMessage( HWND hDlg, UINT message, 
				WPARAM wParam, LPARAM lParam )=0;

	virtual int IsRollupPanelOpen(HWND hwnd)=0;
	virtual int GetRollupScrollPos()=0;
	virtual void SetRollupScrollPos(int spos)=0;

	// Registers a dialog window so IsDlgMesage() gets called for it.
	// This is done automatically for Rollup Pages.
	virtual void RegisterDlgWnd( HWND hDlg )=0;
	virtual int UnRegisterDlgWnd( HWND hDlg )=0;

	// get the current time.
	virtual TimeValue GetTime()=0;	
	};

class ParamDlg;
class Texmap;

class RenderData {
	public:
		virtual void DeleteThis() {delete this;/*should be pure virtual*/}
	};

class LightDesc : public RenderData {
	public:
	// determine color and direction of illumination
    virtual BOOL Illuminate(ShadeContext& sc, Point3& normal, Color& color, Point3 &dir, float &dot_nl) { return 0;}
	};

#define SHADELIM_FLAT 1
#define SHADELIM_GOURAUD 2
#define SHADELIM_PHONG 3

// Transform Reference frames: 
enum RefFrame { REF_CAMERA=0, REF_WORLD, REF_OBJECT };

class ShadeOutput {
	public:
		ULONG flags;
		Color c;  // shaded color
		Color t;  // transparency
		float ior;  // index of refraction
		CoreExport void MixIn(ShadeOutput& a, float f);  // (*this) =  (1-f)*(*this) + f*a;
		void Reset() { 
			flags = 0;
			c.Black(); t.Black(); ior = 1.0f; 
			}
	};

#define SCMODE_NORMAL  0
#define SCMODE_SHADOW  1
//
// Shade Context: passed into Mtls and Texmaps
//
class ShadeContext {
	public:
	ULONG mode;							// normal, shadow ...
	BOOL doMaps;						// apply texture maps?
	BOOL filterMaps;					// should texture be filtered		            
	BOOL shadow;						// apply shadows?
	BOOL backFace;						// are we on the back side of a 2-Sided face?
	int mtlNum;							// sub-mtl number for multi-materials
	Color ambientLight;					// 
	int nLights;						// number of lights;
	ShadeOutput out;                    // where the material leaves its results
	void ResetOutput() { out.Reset(); }
	virtual int Antialias() {return 0;}
	virtual int ProjType() { return 0;} // returns: 0: perspective, 1: parallel
	virtual LightDesc* Light(int n)=0;	// get the nth light. 
	virtual TimeValue CurTime()=0;     	// current time value
	virtual int NodeID() { return -1; }
	virtual INode *Node() { return NULL; }
	virtual Point3 BarycentricCoords() { return Point3(0,0,0);}  // coords relative to triangular face 
	virtual int FaceNumber()=0;			// 
	virtual Point3 Normal()=0;  		// interpolated normal
	virtual void SetNormal(Point3 p) {}	// for perturbing normal
	virtual float  Curve() { return 0.0f; }   	    // estimate of dN/dsx, dN/dsy
	virtual Point3 GNormal()=0; 		// geometric (face) normal
	virtual	Point3 ReflectVector()=0;	// reflection vector
	virtual	Point3 RefractVector(float ior)=0;	// refraction vector
	virtual Point3 CamPos()=0;			// camera position
	virtual Point3 V()=0;       		// Unit view vector: from camera towards P 
	virtual Point3 P()=0;				// point to be shaded;
	virtual Point3 DP()=0;    		  	// deriv of P, relative to pixel, for AA
	virtual void DP(Point3& dpdx, Point3& dpdy){};  // deriv of P, relative to pixel
	virtual Point3 PObj()=0;   		  	// point in obj coords
	virtual Point3 DPObj()=0;   	   	// deriv of PObj, rel to pixel, for AA
	virtual Box3 ObjectBox()=0; 	  	// Object extents box in obj coords
	virtual Point3 PObjRelBox()=0;	  	// Point rel to obj box [-1 .. +1 ] 
	virtual Point3 DPObjRelBox()=0;	  	// deriv of Point rel to obj box [-1 .. +1 ] 
	virtual void ScreenUV(Point2& uv, Point2 &duv)=0; // screen relative uv (from lower left)
	virtual IPoint2 ScreenCoord()=0; // integer screen coordinate (from upper left)
	virtual Point3 UVW()=0;  			// return UVW coords for point
	virtual Point3 DUVW()=0; 			// return UVW derivs for point
	virtual void DPdUVW(Point3 dP[3])=0; // Bump vectors for UVW
	virtual AColor EvalEnvironMap(Texmap *map, Point3 view) {return AColor(0,0,0,0);} //evaluate map with given viewDir
	virtual void GetBGColor(Color &bgcol, Color& transp, BOOL fogBG=TRUE)=0;   // returns Background color, bg transparency

	// Camera ranges set by user in camera's UI.
	virtual float CamNearRange() {return 0.0f;}
	virtual float CamFarRange() {return 0.0f;}

	// Transform to and from internal space
	virtual Point3 PointTo(const Point3& p, RefFrame ito)=0; 
	virtual Point3 PointFrom(const Point3& p, RefFrame ifrom)=0; 
	virtual Point3 VectorTo(const Point3& p, RefFrame ito)=0; 
	virtual Point3 VectorFrom(const Point3& p, RefFrame ifrom)=0; 

	// After being evaluated, if a map or material has a non-zero GBufID, it should
	// call this routine to store it into the shade context.
	virtual	void SetGBufferID(int gbid) {}

	virtual FILE* DebugFile() { return NULL; }

	ShadeContext() {mode = SCMODE_NORMAL; nLights = 0; shadow = TRUE; }
	};
	
// Material flags values
#define MTL_IN_SCENE          	(1<<0)
#define MTL_BEING_EDITED     	(1<<1)  // mtl params being displayed in medit
#define MTL_SUB_BEING_EDITED 	(1<<2)  // mtl OR sub mtl/tex being displayed in medit
#define MTL_TEX_DISPLAY_ENABLED (1<<3)  // Interactive texture display enabled
#define MTL_MEDIT_BACKGROUND    (1<<8)  // Show background in Mtl Editor
#define MTL_MEDIT_BACKLIGHT		(1<<9)  // Backlight in Mtl Editor

#define MTL_OBJTYPE_SHIFT		10
#define MTL_MEDIT_OBJTYPE		(1<<MTL_OBJTYPE_SHIFT) // Object type displayed in Mtl Editor
#define MTL_MEDIT_OBJTYPE_MASK	((1<<MTL_OBJTYPE_SHIFT)|(1<<MTL_OBJTYPE_SHIFT+1)|(1<<MTL_OBJTYPE_SHIFT+2))

#define MTL_TILING_SHIFT		13
#define MTL_MEDIT_TILING		(1<<MTL_TILING_SHIFT) // Object type displayed in Mtl Editor
#define MTL_MEDIT_TILING_MASK	((1<<MTL_TILING_SHIFT)|(1<<MTL_TILING_SHIFT+1)|(1<<MTL_TILING_SHIFT+2))
#define MTL_MEDIT_VIDCHECK		16
#define MTL_MEDIT_NEXT_AVAIL	17
#define MTL_WORK_FLAG			(1<<31)

// Material Requirements flags: returned by Requirements() function
#define MTLREQ_2SIDE    (1<<0)  // 2-sided material
#define MTLREQ_WIRE     (1<<1)  // Wire frame
#define MTLREQ_WIRE_ABS (1<<2)  // Wire frame, absolute size
#define MTLREQ_TRANSP   (1<<3) 	// transparency
#define MTLREQ_UV		(1<<4)  // requires UVW coords
#define MTLREQ_FACEMAP	(1<<5)  // use "face map" UV coords
#define MTLREQ_XYZ		(1<<6)  // requires object XYZ coords
#define MTLREQ_OXYZ 	(1<<7)  // requires object ORIGINAL XYZ coords
#define MTLREQ_BUMPUV	(1<<8)  // requires UV bump vectors
#define MTLREQ_BGCOL	(1<<9)  // requires background color (e.g. Matte mtl)
#define MTLREQ_PHONG	(1<<10)  // requires interpolated normal
#define MTLREQ_AUTOREFLECT (1<<11) // needs to build auto-reflect map
#define MTLREQ_AUTOMIRROR (1<<12)  // needs to build auto-mirror map
#define MTLREQ_NOATMOS 	(1<<13)  // suppress atmospheric shader (used by Matte mtl)
#define MTLREQ_ADDITIVE_TRANSP	(1<<14)  // composite additively 

#define MAPSLOT_TEXTURE  0	//  texture maps
#define MAPSLOT_ENVIRON  1  //  environment maps

// Base class from which materials and textures are subclassed.
class MtlBase: public ReferenceTarget {
	TSTR name;
	ULONG mtlFlags;
	public:
		ULONG gbufID;
		CoreExport MtlBase();
		TSTR& GetName() { return name; }
		CoreExport void SetName(TSTR s);
		void SetMtlFlag(int mask, BOOL val=TRUE) { 
			if (val) mtlFlags |= mask; else mtlFlags &= ~mask;
			}
		void ClearMtlFlag(int mask) { mtlFlags &= ~mask; }
		int TestMtlFlag(int mask) { return(mtlFlags&mask?1:0); }

		// Used internally by materials editor.
		int GetMeditObjType() { return (mtlFlags&MTL_MEDIT_OBJTYPE_MASK)>>MTL_OBJTYPE_SHIFT; }	
		void SetMeditObjType(int t) { mtlFlags &= ~MTL_MEDIT_OBJTYPE_MASK; mtlFlags |= t<<MTL_OBJTYPE_SHIFT; }
		int GetMeditTiling() { return (mtlFlags&MTL_MEDIT_TILING_MASK)>>MTL_TILING_SHIFT; }	
		void SetMeditTiling(int t) { mtlFlags &= ~MTL_MEDIT_TILING_MASK; mtlFlags |= t<<MTL_TILING_SHIFT; }

		// recursively determine if there are any multi-materials or texmaps 
		// in tree
		CoreExport BOOL AnyMulti();

		BOOL TextureDisplayEnabled() { return TestMtlFlag(MTL_TEX_DISPLAY_ENABLED); }

		// Return the "className(instance Name)". 
		// The default implementation should be used in most cases.
	    CoreExport virtual TSTR GetFullName();

		// Mtls and Texmaps must use this to copy the common portion of 
		// themselves when cloning
		CoreExport MtlBase& operator=(const MtlBase& m);

		virtual int BuildMaps(TimeValue t, RenderMapsContext &rmc) { return 1; }

		// This gives the cumulative requirements of the mtl and its
		// tree. The default just OR's together the local requirements
		// of the Mtl with the requirements of all its children.
		// For most mtls, all they need to implement is LocalRequirements,
		// if any.
		CoreExport virtual ULONG Requirements(int subMtlNum); 

		// Specifies various requirements for the material: Should NOT
		// include requirements of its sub-mtls and sub-maps.
		virtual ULONG LocalRequirements(int subMtlNum) { return 0; } 

		// This returns true for materials of texmaps that select sub-
		// materials based on mesh faceMtlIndex. Used in 
		// interactive render.
		virtual	BOOL IsMultiMtl() { return FALSE; }

		// Methods to access sub texture maps of material or texmap
		virtual int NumSubTexmaps() { return 0; }
		virtual Texmap* GetSubTexmap(int i) { return NULL; }
		virtual int MapSlotType(int i) { return MAPSLOT_TEXTURE; }
		virtual void SetSubTexmap(int i, Texmap *m) { }

		// query MtlBase about the On/Off state of each sub map.
		virtual int SubTexmapOn(int i) { return 1; } 

		// This must be called on a sub-Mtl or sub-Map when it is removed,
		// in case it or any of its submaps are active in the viewport.
		CoreExport void DeactivateMapsInTree();

		CoreExport virtual TSTR GetSubTexmapSlotName(int i);
		CoreExport TSTR GetSubTexmapTVName(int i);
		// use this for drag-and-drop of texmaps
		CoreExport void CopySubTexmap(HWND hwnd, int ifrom, int ito);

		// To make texture & material evaluation more efficient, this function is
		// called whenever time has changed.  It will be called at the
		// beginning of every frame during rendering.
		virtual	void Update(TimeValue t, Interval& valid)=0;

		// set back to default values
		virtual void Reset()=0;

		// call this to determine the validity interval of the mtl or texture
		virtual Interval Validity(TimeValue t)=0;
		
		// this gets called when the mtl or texture is to be displayed in the
		// mtl editor params area.

		virtual ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)=0;
		// save the common mtlbase stuff.
		// these must be called in a chunk at the beginning of every mtl and
		// texmap.
		CoreExport IOResult Save(ISave *isave);
        CoreExport IOResult Load(ILoad *iload);
		
		// GBuffer functions
		ULONG GetGBufID() { return gbufID; }
		void SetGBufID(ULONG id) { gbufID = id; }
		
		// Default File enumerator.
		CoreExport void EnumAuxFiles(NameEnumCallback& nameEnum, DWORD flags);

	};


// Every MtlBase sub-class defines a ParamDlg to manage its part of
// the material editor.
class ParamDlg {
	public:
		virtual Class_ID ClassID()=0;
		virtual void SetThing(ReferenceTarget *m)=0;
		virtual ReferenceTarget* GetThing()=0;
		virtual void SetTime(TimeValue t)=0;
		virtual	void ReloadDialog()=0;
		virtual void DeleteThis()=0;
		virtual void ActivateDlg(BOOL onOff)=0;

	};

// Pre-defined categories of texture maps
CoreExport TCHAR TEXMAP_CAT_2D[];  // 2D maps
CoreExport TCHAR TEXMAP_CAT_3D[];  // 3D maps
CoreExport TCHAR TEXMAP_CAT_COMP[]; // Composite
CoreExport TCHAR TEXMAP_CAT_COLMOD[]; // Color modifier
CoreExport TCHAR TEXMAP_CAT_ENV[];  // Environment

class NameAccum {
	public:
		virtual	void AddMapName(TCHAR *name)=0;
	};


// virual texture map interface
class Texmap: public MtlBase {
	int activeCount;
	public:
		
		Texmap() { activeCount = 0; }

		SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
		virtual void GetClassName(TSTR& s) { s= TSTR(_T("Texture")); }  

		// Evaluate the color of map for the context.
		virtual	AColor EvalColor(ShadeContext& sc)=0;
	
		// Evaluate the map for a "mono" channel.
		// this just permits a bit of optimization 
		virtual	float  EvalMono(ShadeContext& sc) {
			return Intens(EvalColor(sc));
			}
		
		// For Bump mapping, need a perturbation to apply to a normal.
		// Leave it up to the Texmap to determine how to do this.
		virtual	Point3 EvalNormalPerturb(ShadeContext& sc)=0;

		// Methods for doing interactive texture display
		CoreExport void IncrActive();
		CoreExport void DecrActive(); 

		int Active() { return activeCount; }
		virtual BOOL SupportTexDisplay() { return FALSE; }
		virtual void ActivateTexDisplay(BOOL onoff) {}
		virtual DWORD GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker) {return 0;}

		virtual	void GetUVTransform(Matrix3 &uvtrans) {}
		virtual int GetTextureTiling() { return  U_WRAP|V_WRAP; }
		virtual void InitSlotType(int sType) {}			   
		
		// System function to set slot type for all subtexmaps in a tree.
		CoreExport void RecursInitSlotType(int sType);			   
		virtual void SetOutputLevel(TimeValue t, float v) {}

		// called prior to render: missing map names should be added to NameAccum.
		// return 1: success,   0:failure. 
		virtual int LoadMapFiles(TimeValue t) { return 1; } 

	};

// virtual material interface
class Mtl: public MtlBase {
	Texmap *activeTexmap; 
	public:
		Mtl(){ activeTexmap=NULL; }
		SClass_ID SuperClassID() { return MATERIAL_CLASS_ID; }
		virtual void GetClassName(TSTR& s) { s= TSTR(_T("Mtl")); }  

		Texmap* GetActiveTexmap() { return activeTexmap; }
		void SetActiveTexmap( Texmap *txm) { activeTexmap = txm; }

		CoreExport void RefDeleted();
		CoreExport void RefAdded(RefMakerHandle rm);  

		// Must call Update(t) before calling these functions!
		// Their purpose is for setting up the material for the
		// GraphicsWindow renderer.
		virtual Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE)=0;
		virtual Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE)=0;	    
		virtual Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE)=0;
		virtual float GetShininess(int mtlNum=0, BOOL backFace=FALSE)=0;
		virtual	float GetShinStr(int mtlNum=0, BOOL backFace=FALSE)=0;		
		virtual float GetXParency(int mtlNum=0, BOOL backFace=FALSE)=0;

		// used by the scanline renderer
		virtual float WireSize(int mtlNum=0, BOOL backFace=FALSE) { return 1.0f; }

		virtual void SetAmbient(Color c, TimeValue t)=0;		
		virtual void SetDiffuse(Color c, TimeValue t)=0;		
		virtual void SetSpecular(Color c, TimeValue t)=0;
		virtual void SetShininess(float v, TimeValue t)=0;		

		virtual ULONG LocalRequirements(int subMtlNum) {  return 0; }

		// The main method: called by the renderer to compute color and transparency
		// output returned in sc.out
		virtual void Shade(ShadeContext& sc)=0;

		// Methods to access sub-materials of meta-materials 
		virtual int NumSubMtls() { return 0; }
		virtual Mtl* GetSubMtl(int i) { return NULL; }
		virtual void SetSubMtl(int i, Mtl *m) { }
		CoreExport virtual TSTR GetSubMtlSlotName(int i);
		CoreExport TSTR GetSubMtlTVName(int i);

	};

//  A texture map implements this class and passes it into  EvalUVMap,
//  EvalUVMapMono, and EvalDeriv to evaluate itself with tiling & mirroring
class MapSampler {
	public:
		// required:
		virtual	AColor Sample(ShadeContext& sc, float u,float v)=0;
		virtual	AColor SampleFilter(ShadeContext& sc, float u,float v, float du, float dv)=0;
		// optional:
		virtual	float SampleMono(ShadeContext& sc, float u,float v) { return Intens(Sample(sc,u,v)); }
		virtual	float SampleMonoFilter(ShadeContext& sc, float u,float v, float du, float dv){
			return Intens(SampleFilter(sc,u,v,du,dv)); 
			}
	};


// This class generates UV coordinates based on the results of a UV 
// Source and user specified transformation.
// A reference to one of these is referenced by all 2D texture maps.
class UVGen: public MtlBase {
	public:
		// Get texture coords and derivatives for antialiasing
		virtual void GetUV( ShadeContext& sc, Point2& UV, Point2& dUV)=0;
		
		// This is how a Texmap evaluates itself
		virtual AColor EvalUVMap(ShadeContext &sc, MapSampler* samp,  BOOL filter=TRUE)=0;
		virtual float EvalUVMapMono(ShadeContext &sc, MapSampler* samp, BOOL filter=TRUE)=0;
		virtual	Point2 EvalDeriv( ShadeContext& sc, MapSampler* samp, BOOL filter=TRUE)=0;

		// Get dPdu and dPdv for bump mapping
		virtual	void GetBumpDP( ShadeContext& sc, Point3& dPdu, Point3& dPdv)=0;

		virtual void GetUVTransform(Matrix3 &uvtrans)=0;

		virtual int GetTextureTiling()=0;

		virtual int SymFlags()=0;

		virtual void InitSlotType(int sType)=0; 

		SClass_ID SuperClassID() { return UVGEN_CLASS_ID; }
	};

// This class generates Point3 coordinates based on the ShadeContext.
// A reference to one of these is referenced by all 3D texture maps.
class XYZGen: public MtlBase {
	public:
		// Get texture coords and derivatives for antialiasing
		virtual void GetXYZ( ShadeContext& sc, Point3& p, Point3& dp)=0;
		SClass_ID SuperClassID() { return XYZGEN_CLASS_ID; }

	};

// This class is used by texture maps to put up the output filter 
// rollup, and perform the output filtering.
class TextureOutput: public MtlBase {
	public:
		virtual AColor Filter(AColor c) = 0;
		virtual float Filter(float f) = 0;
		virtual Point3 Filter(Point3 p) = 0;
		virtual float GetOutputLevel(TimeValue t) = 0;
		virtual void SetOutputLevel(TimeValue t, float v) = 0;
		virtual void SetInvert(BOOL onoff)=0;
		virtual BOOL GetInvert()=0;
		SClass_ID SuperClassID() { return TEXOUTPUT_CLASS_ID; }
	};

typedef MtlBase* MtlBaseHandle;
typedef Mtl* MtlHandle;
typedef Texmap* TexmapHandle;


// Simple list of materials
class MtlList: public Tab<MtlHandle> {
	public:
		CoreExport int AddMtl(Mtl *m, BOOL checkUnique=TRUE);
		CoreExport int FindMtl(Mtl *m);
		CoreExport int FindMtlByName(TSTR& name);
		void RemoveEntry(int n) { Delete(n,1); }
		void Empty() { Resize(0); }
	};

// Materials library
class MtlLib: public ReferenceTarget, public MtlList {
	public:
		SClass_ID SuperClassID() { return REF_MAKER_CLASS_ID; }
		CoreExport Class_ID ClassID();
		CoreExport void DeleteAll();
		void GetClassName(TSTR& s) { s= TSTR(_T("MtlLib")); }  
		CoreExport ~MtlLib();

		int NumSubs() { 
			return Count(); 
			}  
		Animatable* SubAnim(int i) { 
			return (*this)[i]; 
			}
		CoreExport TSTR SubAnimName(int i);
		CoreExport virtual void Remove(Mtl *m);
		CoreExport virtual void Add(Mtl *m);

		// From ref
		RefResult AutoDelete() { return REF_SUCCEED; }
		CoreExport void DeleteThis();
		int NumRefs() { return Count();}
		RefTargetHandle GetReference(int i) { return (RefTargetHandle)(*this)[i];}
		CoreExport void SetReference(int i, RefTargetHandle rtarg);
		CoreExport RefTargetHandle Clone(RemapDir &remap = NoRemap());
		CoreExport RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );

		// IO
		CoreExport IOResult Save(ISave *isave);
        CoreExport IOResult Load(ILoad *iload);

	};


// A MtlBase Version of the above
class MtlBaseList: public Tab<MtlBaseHandle> {
	public:
		CoreExport int AddMtl(MtlBase *m, BOOL checkUnique=TRUE);
		CoreExport int FindMtl(MtlBase *m);
		CoreExport int FindMtlByName(TSTR& name);
		void RemoveEntry(int n) { Delete(n,1); }
		void Empty() { Resize(0); }
	};

class MtlBaseLib : public ReferenceTarget, public MtlBaseList {
	public:
		SClass_ID SuperClassID() { return REF_MAKER_CLASS_ID; }
		CoreExport Class_ID ClassID();
		CoreExport void DeleteAll();
		void GetClassName(TSTR& s) { s= TSTR(_T("MtlBaseLib")); }  
		CoreExport ~MtlBaseLib();

		int NumSubs() {return Count();}		
		Animatable* SubAnim(int i) {return (*this)[i];}
		CoreExport TSTR SubAnimName(int i);
		
		CoreExport virtual void Remove(MtlBase *m);
		CoreExport virtual void Add(MtlBase *m);
		CoreExport virtual void RemoveDuplicates();

		// From ref
		RefResult AutoDelete() {return REF_SUCCEED;}
		CoreExport void DeleteThis();
		int NumRefs() { return Count();}
		RefTargetHandle GetReference(int i) { return (RefTargetHandle)(*this)[i];}
		CoreExport void SetReference(int i, RefTargetHandle rtarg);
		CoreExport RefTargetHandle Clone(RemapDir &remap = NoRemap());
		CoreExport RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );

		// IO
		CoreExport IOResult Save(ISave *isave);
        CoreExport IOResult Load(ILoad *iload);
	};


// Simple list of numbers
class NumList: public Tab<int> {
	public:
		CoreExport int Add(int j, BOOL checkUnique=TRUE);
		CoreExport int Find(int j);
	};

class MtlRemap {
	public:
		virtual Mtl* Map(Mtl *oldAddr)=0;
	};

CoreExport void SetLoadingMtlLib(MtlLib *ml);
CoreExport void SetLoadingMtlBaseLib(MtlBaseLib *ml);

CoreExport ClassDesc* GetMtlLibCD();
CoreExport ClassDesc* GetMtlBaseLibCD();

CoreExport UVGen* GetNewDefaultUVGen();
CoreExport XYZGen* GetNewDefaultXYZGen();
CoreExport TextureOutput* GetNewDefaultTextureOutput();
inline int IsMtl(MtlBase *m) { return m->SuperClassID()==MATERIAL_CLASS_ID; }
inline int IsTex(MtlBase *m) { return m->SuperClassID()==TEXMAP_CLASS_ID; }

// Combines the two materials into a multi-material.
// Either of the two input materials can themselves be multis.
// c1 and c2 will be set to the mat count for mat1 and mat2.
CoreExport Mtl *CombineMaterials(Mtl *mat1, Mtl *mat2, int &c1, int &c2);

#endif
