/**********************************************************************
 *<
	FILE: patchobj.h

	DESCRIPTION:  Defines Patch Mesh Object

	CREATED BY: Tom Hudson

	HISTORY: created 21 June 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __PATCHOBJ__ 

#define __PATCHOBJ__

#include "meshlib.h"
#include "patchlib.h"
#include "snap.h"

extern CoreExport Class_ID patchObjectClassID;

class PatchObject: public GeomObject {
		Interval geomValid;
		Interval topoValid;
		Interval texmapValid;
		Interval selectValid;
		DWORD validBits; // for the remaining constant channels
		void CopyValidity(PatchObject *fromOb, ChannelMask channels);
		
	protected:
		//  inherited virtual methods for Reference-management
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message );
	public:
		PatchMesh patch;

		// Mesh cache
		Mesh mesh;
		int cacheSteps;		// meshSteps used for the cache
		BOOL cacheAdaptive;	// adaptive switch used for cache
		BOOL meshValid;
		BOOL showMesh;
		BOOL showLattice;

		CoreExport PatchObject();
		CoreExport ~PatchObject();

		//  inherited virtual methods:

		// From BaseObject
		CoreExport int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
		CoreExport int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
		CoreExport void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
		CoreExport CreateMouseCallBack* GetCreateMouseCallBack();
		CoreExport RefTargetHandle Clone(RemapDir& remap = NoRemap());

		// From Object			 
		CoreExport ObjectState Eval(TimeValue time);
		CoreExport Interval ObjectValidity(TimeValue t);

		// get and set the validity interval for the nth channel
	   	CoreExport Interval ChannelValidity(TimeValue t, int nchan);
		CoreExport void SetChannelValidity(int i, Interval v);
		CoreExport void InvalidateChannels(ChannelMask channels);

		// Convert-to-type validity
		CoreExport Interval ConvertValidity(TimeValue t);

		// Deformable object procs	
		int IsDeformable() { return 1; }  
		CoreExport int NumPoints();
		CoreExport Point3 GetPoint(int i);
		CoreExport void SetPoint(int i, const Point3& p);
		
		CoreExport void PointsWereChanged();
		CoreExport void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm=NULL,BOOL useSel=FALSE );
		CoreExport void Deform(Deformer *defProc, int useSel);

		// Mappable object procs
		int IsMappable() { return 1; }
		void ApplyUVWMap(int type, float utile, float vtile, float wtile,
			int uflip, int vflip, int wflip, int cap,const Matrix3 &tm) {
				patch.ApplyUVWMap(type,utile,vtile,wtile,uflip,vflip,wflip,cap,tm); }

		CoreExport int CanConvertToType(Class_ID obtype);
		CoreExport Object* ConvertToType(TimeValue t, Class_ID obtype);
		CoreExport void FreeChannels(ChannelMask chan);
		CoreExport Object *MakeShallowCopy(ChannelMask channels);
		CoreExport void ShallowCopy(Object* fromOb, ChannelMask channels);
		CoreExport void NewAndCopyChannels(ChannelMask channels);

		CoreExport DWORD GetSubselState();

		// From GeomObject
		CoreExport int IntersectRay(TimeValue t, Ray& r, float& at);
		CoreExport void GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box );
		CoreExport void GetLocalBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box );
		CoreExport int IsInstanceDependent();	// Not view-dependent (yet)
		CoreExport Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);

		PatchMesh& GetPatchMesh() { return patch; }
		Mesh& GetMesh() { PrepareMesh(); return mesh; }

		// Animatable methods

		void DeleteThis() { delete this; }
		void FreeCaches() {patch.InvalidateGeomCache(); }
		Class_ID ClassID() { return Class_ID(PATCHOBJ_CLASS_ID,0); }
		void GetClassName(TSTR& s) { s = TSTR(_T("PatchObject")); }
		void NotifyMe(Animatable *subAnim, int message) {}
		int IsKeyable() { return 0;}
		int Update(TimeValue t) { return 0; }
		BOOL BypassTreeView() { return TRUE; }
		// This is the name that will appear in the history browser.
		TCHAR *GetObjectName() { return _T("Patch"); }

		// IO
		CoreExport IOResult Save(ISave *isave);
		CoreExport IOResult Load(ILoad *iload);

		// PatchObject-specific methods
		CoreExport void PrepareMesh();
		CoreExport BOOL ShowLattice() { return patch.GetDispFlag(DISP_LATTICE) ? TRUE : FALSE; }
		CoreExport BOOL ShowVerts() { return patch.GetDispFlag(DISP_VERTS) ? TRUE : FALSE; }
		CoreExport void SetShowLattice(BOOL sw) { if(sw) patch.SetDispFlag(DISP_LATTICE); else patch.ClearDispFlag(DISP_LATTICE); }
		CoreExport void SetShowVerts(BOOL sw) { if(sw) patch.SetDispFlag(DISP_VERTS); else patch.ClearDispFlag(DISP_VERTS); }
		CoreExport void SetMeshSteps(int steps);
		CoreExport int GetMeshSteps();
		CoreExport void SetAdaptive(BOOL sw);
		CoreExport BOOL GetAdaptive();
		CoreExport void InvalidateMesh();
	};

CoreExport ClassDesc* GetPatchObjDescriptor();

#endif // __PATCHOBJ__
