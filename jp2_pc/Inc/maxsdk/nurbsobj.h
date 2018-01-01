/**********************************************************************
 *<
	FILE: nurbsobj.h

	DESCRIPTION:  Defines NURBS Mesh Object

	CREATED BY: Steve Anderson, based on nurbsobj by Tom Hudson

	HISTORY: created January 4 1996.

 *>	Copyright (c) 1996, All Rights Reserved.
 **********************************************************************/

#ifndef __NURBSOBJ__ 

#define __NURBSOBJ__

#include "meshlib.h"
#include "nurbslib.h"
#include "snap.h"

#define NURBSOBJ_CLASS_ID	0x4135

extern CoreExport Class_ID nurbsObjectClassID;

// For welding intersections of face & edge tesselations:
#define NMESHWELD 1e-04f

typedef Tab<float> FloatTab;

// MeshingFaces, MeshingEdges, and MeshingVerts are for my own convenience
// in converting NurbsMeshes to Meshes.  Don't worry about them if you don't
// care about how the meshing occurs.

class NurbsMeshingFace {
public:
	int medg[4];	// Four boundary lines: u=0, v=0, u=1, v=1.
	int gnum[2];	// g[0] represents columns, g[1] rows.
	int *gedx[2];	// Indices of intersections of boundary with grid lines
	int vstart;		// Records start vertex of face in final mesh.

	NurbsMeshingFace ();
	~NurbsMeshingFace ();
	void Setup (NurbsFace & fac);
	void AddGridIntersection (int g, int level, int pnt, float endval);
	int CountVerts ();
};

class NurbsMeshingEdge {
public:
	int start, end;		// Meshing verts
	int face1, face2, edge1, edge2;
	FloatTab tvals;		// One t-value per required vertex, including first & last
	int vstart;			// Start of edge-vertices in final mesh.
	BOOL degenerate;

	NurbsMeshingEdge();
	~NurbsMeshingEdge() {}
};

typedef IntTab NurbsMeshingVert;	// (List of incident meshingEdges.)

// NurbsObject: the nexus of Nurbsiness.
// about what you'd expect.  All rendering, etc resorts to meshing.

class NurbsObject: public GeomObject {
		Interval geomValid;
		Interval topoValid;
		DWORD validBits; // for the remaining constant channels
		void CopyValidity(NurbsObject *fromOb, ChannelMask channels);

		// Meshing stuff:
		NurbsMeshingFace *mFaces;
		NurbsMeshingEdge *mEdges;
		NurbsMeshingVert *mVerts;
		int mFnum, mEnum, mVnum;

		void SetupMeshingStructs ();
		void FindMeshingEdges ();
		void FindTVectors ();
		int SortOutMeshingVerts ();
		void SetEdgeVertConnection (int e, int end, int v);
		
	protected:
		//  inherited virtual methods for Reference-management
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message );
	public:
		NurbsMesh nmesh;

		int meshSteps;
		BOOL adaptive;
		Mesh mesh;
		BOOL meshValid;
		BOOL showMesh;
		BOOL showLattice;

		CoreExport NurbsObject();
		CoreExport ~NurbsObject();

		// Inherited virtual methods:

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

		// Deformable object procs	
		int IsDeformable()						{ return 1; }  
		int NumPoints()							{ return nmesh.getNumVerts(); }
		Point3 GetPoint(int i)					{ return nmesh.getVert(i).p; }
		void SetPoint(int i, const Point3& p)	{ nmesh.setVert(i,p); }
		
		void PointsWereChanged()				{ return; }
		CoreExport void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm=NULL,BOOL useSel=FALSE );
		CoreExport void Deform(Deformer *defProc, int useSel);

		CoreExport int CanConvertToType(Class_ID obtype);
		CoreExport Object* ConvertToType(TimeValue t, Class_ID obtype);
		CoreExport void FreeChannels(ChannelMask chan);
		CoreExport Object *MakeShallowCopy(ChannelMask channels);
		CoreExport void ShallowCopy(Object* fromOb, ChannelMask channels);
		CoreExport void NewAndCopyChannels(ChannelMask channels);

		CoreExport DWORD GetSubselState();

		// From GeomObject
		CoreExport int IntersectRay(TimeValue t, Ray& r, float& at);
		CoreExport ObjectHandle CreateTriObjRep(TimeValue t);  // for rendering, also for deformation		
		CoreExport void GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box );
		CoreExport void GetLocalBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box );
		CoreExport int IsInstanceDependent();	// Not view-dependent (yet)
		CoreExport Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);

		NurbsMesh& GetNurbsMesh() { return nmesh; }
		Mesh& GetMesh() { PrepareMesh(); return mesh; }

		// Animatable methods

		void DeleteThis() { delete this; }
		void FreeCaches() { return; }
		Class_ID ClassID() { return Class_ID(NURBSOBJ_CLASS_ID,0); }
		void GetClassName(TSTR& s) { s = TSTR(_T("NurbsObject")); }
		void NotifyMe (Animatable *subAnim, int message) {}
		int IsKeyable() { return 0;}
		int Update(TimeValue t) { return 0; }
		BOOL BypassTreeView() { return TRUE; }

		// IO
		CoreExport IOResult Save(ISave *isave);
		CoreExport IOResult Load(ILoad *iload);

		// NurbsObject-specific methods
		CoreExport void PrepareMesh();
		CoreExport void SetMeshSteps(int steps);
		CoreExport void SetAdaptive(BOOL sw);
		CoreExport void InvalidateMesh();
	};

CoreExport ClassDesc* GetNurbsObjDescriptor();

#endif // __NURBSOBJ__
