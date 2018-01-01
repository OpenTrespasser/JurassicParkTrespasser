/**********************************************************************
 *<
	FILE: mesh.h

	DESCRIPTION: Main include file for triangle meshes.

	CREATED BY: Don Brittain

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef _MESH_H_

#define _MESH_H_

#include "channels.h"
#include "snap.h"
#include <ioapi.h>
#include "export.h"
#include "vedge.h"  //DS

typedef unsigned short MtlID;

// This a UV coordinate. These cound be Point2s
typedef Point3 UVVert;


#define MESH_MULTI_PROCESSING	TRUE		// TRUE turns on mp vertex transformation

class ISave;
class ILoad;

#define NEWMESH

class RNormal {
	public:
		RNormal()	{ smGroup = mtlIndex = 0; }
		void		setNormal(const Point3 &nor) { normal = nor; }
		void		addNormal(const Point3 &nor) { normal += nor; }	
		void		normalize(void) 	{ normal = Normalize(normal); }
		Point3 &	getNormal(void) 	{ return normal; }
		void		setSmGroup(DWORD g)	{ smGroup = g; }
		void		addSmGroup(DWORD g) { smGroup |= g; }
		DWORD		getSmGroup(void)	{ return smGroup; }
		void		setMtlIndex(MtlID i){ mtlIndex = i; }
		MtlID		getMtlIndex(void)	{ return mtlIndex; }
		void		setRGB(Point3 &clr)	{ rgb = clr; };
		Point3 &	getRGB(void)		{ return rgb; }
		
	private:	
		Point3		normal;	   
		DWORD		smGroup;    
		MtlID		mtlIndex;   
		Point3		rgb;	   
	};					   



// RVertex flags: contain clip flags, number of normals at the vertex
// and the number of normals that have already been rendered.
// fine PLANE_MASK	0x00003f00UL -- now in gfx.h
#define NORCT_MASK			0x000000ffUL
#define SPECIFIED_NORMAL	0x00004000UL
#define OUT_LEFT			0x00010000UL
#define OUT_RIGHT			0x00020000UL
#define OUT_TOP				0x00040000UL
#define OUT_BOTTOM			0x00080000UL
#define RECT_MASK			0x000f0000UL
#define RND_MASK			0xfff00000UL
#define RND_NOR0			0x00100000UL
#define RND_NOR(n)  		(RND_NOR0 << (n))

class RVertex {
	public:
		RVertex()	{ rFlags = 0; ern = NULL; }
		DllExport ~RVertex();	

		DWORD		rFlags;     
		union {
			int		iPos[3];	
			float	fPos[3];
			};
		RNormal		rn;		   
		RNormal 	*ern;		 
	};					  



// Face Flags:
// 		3 LSBs hold the edge visibility flags
// 		Bit 3 indicates the presence of texture verticies

// if bit is 1, edge is visible
#define EDGE_VIS			1
#define EDGE_INVIS			0

// first edge-visibility bit field
#define VIS_BIT				0x0001
#define VIS_MASK			0x0007

#define EDGE_A		(1<<0)
#define EDGE_B		(1<<1)
#define EDGE_C		(1<<2)
#define EDGE_ALL	(EDGE_A|EDGE_B|EDGE_C)

#define FACE_HIDDEN	(1<<3)
#define HAS_TVERTS	(1<<4)
#define FACE_WORK	(1<<5) // used in various algorithms

// The mat ID is stored in the HIWORD of the face flags
#define FACE_MATID_SHIFT	16
#define FACE_MATID_MASK		0xFFFF


class Face {	
	public:
		DWORD	v[3];
		DWORD	smGroup;
		DWORD	flags;

		Face()	{ flags = 0; }
		MtlID	getMatID() {return (int)((flags>>FACE_MATID_SHIFT)&FACE_MATID_MASK);}
		void    setMatID(MtlID id) {flags &= 0xFFFF; flags |= (DWORD)(id<<FACE_MATID_SHIFT);}
		void	setSmGroup(DWORD i) { smGroup = i; }
		DWORD	getSmGroup(void)	{ return smGroup; }
		void	setVerts(DWORD *vrt){ memcpy(v, vrt, 3*sizeof(DWORD)); }
		void	setVerts(int a, int b, int c)  { v[0]=a; v[1]=b; v[2]=c; }
		DllExport void	setEdgeVis(int edge, int visFlag);
		DllExport void    setEdgeVisFlags(int va, int vb, int vc); 
		int		getEdgeVis(int edge){ return flags & (VIS_BIT << edge); }
		DWORD	getVert(int index)	{ return v[index]; }
		DWORD *	getAllVerts(void)	{ return v; }
		BOOL	Hidden() {return flags&FACE_HIDDEN?TRUE:FALSE;}
		void	Hide() {flags|=FACE_HIDDEN;}
		void	Show() {flags&=~FACE_HIDDEN;}
		void	SetHide(BOOL hide) {if (hide) Hide(); else Show();}
	};



class TVFace {
	public:
		DWORD	t[3];  // indices into tVerts
		
		TVFace() {}
		TVFace(DWORD a, DWORD b, DWORD c) {t[0]=a; t[1]=b; t[2]=c;}
		void	setTVerts(DWORD *vrt){ memcpy(t, vrt, 3*sizeof(DWORD)); }
		void	setTVerts(int a, int b, int c)  { t[0]=a; t[1]=b; t[2]=c; }	
		DWORD	getTVert(int index)	{ return t[index]; }
		DWORD *	getAllTVerts(void)	{ return t; }
	};


		


// Flag definitions
#define MESH_EDGE_LIST 1

#define COMP_TRANSFORM	0x0001	// forces recalc of model->screen transform; else will attempt to use cache
#define COMP_IGN_RECT	0x0002	// forces all polys to be rendered; else only those intersecting the box will be
#define COMP_LIGHTING	0x0004	// forces re-lighting of all verts (as when a light moves); else only relight moved verts

#define COMP_ALL		0x00ff

// If this bit is set then the node being displayed by this mesh is selected.
// Certain display flags only activate when this bit is set.
#define COMP_OBJSELECTED	(1<<8)


typedef int (*INTRFUNC)();

DllExport void setMeshIntrFunc(INTRFUNC fn);


class MeshSubHitRec {
	private:		
		MeshSubHitRec *next;
	public:
		DWORD	dist;
		int		index;
		DWORD	flags;

		MeshSubHitRec(DWORD dist, int index, MeshSubHitRec *next) 
			{this->dist = dist; this->index = index; this->next = next;}
		MeshSubHitRec(DWORD dist, int index, DWORD flags, MeshSubHitRec *next) 
			{this->dist = dist; this->index = index; this->next = next;this->flags = flags;}

		MeshSubHitRec *Next() { return next; }		
	};

class SubObjHitList {
	private:
		MeshSubHitRec *first;
	public:
		SubObjHitList() { first = NULL; }
		~SubObjHitList() {
			MeshSubHitRec *ptr = first, *fptr;
			while (ptr) {
				fptr = ptr;
				ptr = ptr->Next();
				delete fptr;
				}
			first = NULL;
			}	

		MeshSubHitRec *First() { return first; }
		void AddHit( DWORD dist, int index ) {
			first = new MeshSubHitRec(dist,index,first);
			}
	};



// Flags for sub object hit test

// NOTE: these are the same bits used for object level.
#define SUBHIT_SELONLY		(1<<0)
#define SUBHIT_UNSELONLY	(1<<2)
#define SUBHIT_ABORTONHIT	(1<<3)
#define SUBHIT_SELSOLID		(1<<4)

#define SUBHIT_USEFACESEL	(1<<23)   // When this bit is set, the sel only and unsel only tests will use the faces selection when doing a vertex level hit test
#define SUBHIT_VERTS		(1<<24)
#define SUBHIT_FACES		(1<<25)
#define SUBHIT_EDGES		(1<<26)
#define SUBHIT_TYPEMASK		(SUBHIT_VERTS|SUBHIT_FACES|SUBHIT_EDGES)


// Display flags
#define DISP_VERTTICKS		(1<<0)
#define DISP_SELVERTS		(1<<10)
#define DISP_SELFACES		(1<<11)
#define DISP_SELEDGES		(1<<12)
#define DISP_SELPOLYS		(1<<13)

// Selection level bits.
#define MESH_OBJECT		(1<<0)
#define MESH_VERTEX		(1<<1)
#define MESH_FACE		(1<<2)
#define MESH_EDGE		(1<<3)

class MeshOpProgress;

class MeshRenderData {
	public:
	virtual void DeleteThis()=0;
	};

class Mesh {
	friend class Face;

	private:
#if MESH_MULTI_PROCESSING
		static int		refCount;
		static HANDLE	xfmThread;
		static HANDLE	xfmMutex;
		static HANDLE	xfmStartEvent;
		static HANDLE	xfmEndEvent;
		friend DWORD WINAPI xfmFunc(LPVOID ptr);
		static HANDLE	fNorThread;
		static HANDLE	fNorMutex;
		static HANDLE	fNorStartEvent;
		static HANDLE	fNorEndEvent;
		friend DWORD WINAPI fNorFunc(LPVOID ptr);
#endif
		// derived data-- can be regenerated
		RVertex 		*rVerts;		// <<< instance specific.
		GraphicsWindow 	*cacheGW;  		// identifies rVerts cache
		Point3	 		*faceNormal;	// object space--depends on geom+topo
		Box3			bdgBox;			// object space--depends on geom+topo
		int 			numVisEdges;	// depends on topo 
		int				edgeListHasAll;	// depends on topo
		VEdge 			*visEdge;		// depends on topo 	
 
 		// Vertex and face work arrays -- for snap code
		int			snapVCt;
		int			snapFCt;
		char		*snapV;
		char		*snapF;

		// -------------------------------------
		//
		long   		flags;		  	// work flags- 

		float 		norScale;	    // scale of normals -- couldn't this be done
		 							// automatically relative to bdgBox?

		// Rolf: these are instance specific and should be pulled out of here,
		// and just passed in from the Node.
		int			dspNormals;    // display surface normals--- put in flags?
		int			dspAllEdges;   // shows hidden edges  ---- put in flags?

		int 		renderFace(GraphicsWindow *gw, DWORD index, int *custVis=NULL);
		int			renderEdge(GraphicsWindow *gw, DWORD face, DWORD edge);
		void  		calcNormal(int i);
		void		checkRVertsAlloc(void);
		void		setCacheGW(GraphicsWindow *gw)	{ cacheGW = gw; }
		GraphicsWindow *getCacheGW(void)			{ return cacheGW; }

		void		buildFaceNormals();
		void		checkNormals(int illum);

		void 		freeVerts();
		void  		freeFaces();
		void		freeFaceNormals();  
		void  		freeRVerts();
		void  		freeTVerts();
		void 		freeT3DVerts();
		void  		freeTVFaces();
		void		freeSnapData();
		int			buildSnapData(GraphicsWindow *gw,int verts,int edges);

	public:
		// Topology
		int			numVerts;
		int	 		numFaces;
		Face *		faces;

		// Geometry
		Point3 *	verts;

		// Texture Coord assignment 
		int			numTVerts;
		UVVert *	tVerts;
		TVFace *	tvFace;  	 
		Point3 *	t3DVerts;	// Snap shot of regular vertices before a deformation

		// Material assignment
		MtlID		mtlIndex;     // object material
		MtlID	* 	faceMtlIndex; // material index per face. 

		// Selection
		BitArray	vertSel;  		// selected vertices
		BitArray	faceSel;  		// selected faces
		BitArray	edgeSel;		// selected edges, identified as 3*faceIndex + edgeIndex
		BitArray	vertHide;		// Hide flags for vertices

		// Display attribute flags
		DWORD		dispFlags;

		// Selection level
		DWORD		selLevel;

		// true if normals have been built for the current mesh
		int			normalsBuilt;

		MeshRenderData*	 renderData;  // used by the renderer

		DllExport Mesh();
		DllExport Mesh(const Mesh& fromMesh);
		DllExport ~Mesh();
		void 		Init();

		DllExport Mesh& operator=(const Mesh& fromMesh);
		
		DllExport BOOL 	setNumVerts(int ct, BOOL keep=FALSE);
		int				getNumVerts(void) const	{ return numVerts; }
		
		DllExport BOOL	setNumFaces(int ct, BOOL keep=FALSE);
		int				getNumFaces(void) const{ return numFaces; }
		
		DllExport BOOL	setNumTVerts(int ct, BOOL keep=FALSE);
		int				getNumTVerts(void) const { return numTVerts; }
		
		// These are parallel to faces and vertices (repectively)
		// These are called from setNumVets()/Faces() to maintain the same count.
		//
		// If they are NULL and keep = TRUE they stay NULL.
		// If they are NULL and keep = FALSE they are allocated (3D verts also init themselves from the main vert array)
		// If they are non-NULL and ct = 0 they are set to NULL (and freed)
		// NOTE that  T3DVerts are always initialized to either thier previous value (keep=1) or the vertex array
		// The old count is the length of the existing array (usually equal to numVerts but not if verts have just been resized also)
		DllExport BOOL 	setNumTVFaces(int ct, BOOL keep=FALSE, int oldCt=0);
		DllExport BOOL 	setNumT3DVerts(int ct, BOOL keep=FALSE, int oldCt=0);

		void		setVert(int i, const Point3 &xyz)	{ verts[i] = xyz; }
		void		setVert(int i, float x, float y, float z)	{ verts[i].x=x; verts[i].y=y; verts[i].z=z; }
		void		setTVert(int i, const UVVert &xyz)	{ tVerts[i] = xyz; }
		void		setTVert(int i, float x, float y, float z)	{ tVerts[i].x=x; tVerts[i].y=y; tVerts[i].z=z; }
		
		DllExport void		setNormal(int i, const Point3 &xyz); 
		DllExport Point3 &	getNormal(int i);

		void		setFaceNormal(int i, const Point3 &xyz) { faceNormal[i] =  xyz; }
		Point3 &	getFaceNormal(int i) { return faceNormal[i]; }

		Point3 &	getVert(int i)		{ return verts[i];  }
		Point3 *	getVertPtr(int i)	{ return verts+i; }
		UVVert &	getTVert(int i)		{ return tVerts[i];  }
		UVVert *	getTVertPtr(int i)	{ return tVerts+i; }
		RVertex &	getRVert(int i)		{ return rVerts[i]; }
		RVertex *	getRVertPtr(int i)	{ return rVerts+i; }
		
		void		setMtlIndex(MtlID	i)	{ mtlIndex = i; }
		MtlID		getMtlIndex(void) 		{ return mtlIndex; }

		// Face MtlIndex list methods;
		DllExport void		createFaceMtlIndexList();		
		void				freeFaceMtlIndexList();  
	    DllExport MtlID		getFaceMtlIndex(int i);  //	{ return faceMtlIndex? faceMtlIndex[i]: mtlIndex; }
		DllExport void		setFaceMtlIndex(int i, MtlID id); 	
		
		DllExport void		buildNormals();
		DllExport void 		buildRenderNormals(); // like buildNormals, but ignores mtlIndex

		DllExport void		render(GraphicsWindow *gw, Material *ma, RECT *rp, int compFlags, int numMat=1);
		DllExport BOOL		select(GraphicsWindow *gw, Material *ma, HitRegion *hr, int abortOnHit = FALSE, int numMat=1);
		DllExport void		snap(GraphicsWindow *gw, SnapInfo *snap, IPoint2 *p, Matrix3 &tm);
		DllExport BOOL 		SubObjectHitTest(GraphicsWindow *gw, Material *ma, HitRegion *hr,
								DWORD flags, SubObjHitList& hitList, int numMat=1 );

		void		displayNormals(int b, float sc)	{ dspNormals = b; if(sc != (float)0.0) norScale = sc; }
		void		displayAllEdges(int b)	{ dspAllEdges = b; }
		DllExport void		buildBoundingBox(void);
		DllExport Box3 		getBoundingBox(Matrix3 *tm=NULL); // RB: optional TM allows the box to be calculated in any space.
		                                              // NOTE: this will be slower becuase all the points must be transformed.
		
		DllExport void		EnableEdgeList(int e);
		DllExport void     	BuildVisEdgeList();
		DllExport void 		DrawVisEdgeList(GraphicsWindow *gw, DWORD flags);
		DllExport void		HitTestVisEdgeList(GraphicsWindow *gw, int abortOnHit ); // RB
		DllExport void		InvalidateEdgeList(); // RB
		DllExport void 		InvalidateGeomCache();
		DllExport void 		FreeAll(); //DS
				
		// functions for use in data flow evaluation
		DllExport void 		ShallowCopy(Mesh *amesh, unsigned long channels);
		DllExport void 		DeepCopy(Mesh *amesh, unsigned long channels);
		DllExport void		NewAndCopyChannels(unsigned long channels);
		DllExport void 		FreeChannels( unsigned long channels, int zeroOthers=1);

		// Display flags
		void		SetDispFlag(DWORD f) { dispFlags |= f; }
		DWORD		GetDispFlag(DWORD f) { return dispFlags & f; }
		void		ClearDispFlag(DWORD f) { dispFlags &= ~f; }

		// Selection access
		BitArray& 	VertSel() { return vertSel;  }	
		BitArray& 	FaceSel() { return faceSel;  }	

		// Constructs a vertex selection list based on the current selection level.
		DllExport BitArray 	VertexTempSel();

		DllExport IOResult Save(ISave* isave);
		DllExport IOResult Load(ILoad* iload);

		// RB: added so all objects can easily support the GeomObject method of the same name.
		DllExport int IntersectRay(Ray& ray, float& at, Point3& norm);

		// RB: I couldn't resist adding these <g>
		DllExport Mesh operator+(Mesh &mesh);  // Union
		DllExport Mesh operator-(Mesh &mesh);  // Difference
		DllExport Mesh operator*(Mesh &mesh);  // Intersection

		DllExport void WeldCollinear(BitArray &set);

		DllExport void Optimize(
			float normThresh, float edgeThresh, 
			float bias, DWORD flags, MeshOpProgress *prog=NULL);

		DllExport void ApplyUVWMap(int type,
			float utile, float vtile, float wtile,
			int uflip, int vflip, int wflip, int cap,
			const Matrix3 &tm);

		DllExport void FlipNormal(int i);
		DllExport void UnifyNormals(BOOL selOnly);
		DllExport void AutoSmooth(float angle,BOOL useSel);

		DllExport Edge *MakeEdgeList(int *edgeCount, int flagdbls=0);
		DllExport int DeleteFlaggedFaces(); // deletes all faces with FACE_WORK flag set

		// Returns TRUE if an equivalent face already exists.
		DllExport BOOL DoesFaceExist(DWORD v0, DWORD v1, DWORD v2);

		// Removes faces that have two or more equal indices.
		// Returns TRUE if any degenerate faces were found
		DllExport BOOL RemoveDegenerateFaces();

		// Removes faces that have indices that are out of range
		// Returns TRUE if any illegal faces were found
		DllExport BOOL RemoveIllegalFaces();

		DllExport float AngleBetweenFaces(DWORD f0, DWORD f1);

		// Compute the barycentric coords of a point in the plane of
		// a face relative to that face.
		DllExport Point3 BaryCoords(DWORD face, Point3 p);

		// Some edge operations
		DllExport void DivideEdge(DWORD edge);
		DllExport void TurnEdge(DWORD edge);		

		// used by the renderer
		void  SetRenderData(MeshRenderData *p) {renderData = p; } 
		MeshRenderData * GetRenderData() { return renderData; }
	};

// Mapping types passed to ApplyUVWMap()
#define MAP_PLANAR		0
#define MAP_CYLINDRICAL	1
#define MAP_SPHERICAL	2
#define MAP_BALL		3
#define MAP_BOX			4


// Optimize flags
#define OPTIMIZE_SAVEMATBOUNDRIES		(1<<0)
#define OPTIMIZE_SAVESMOOTHBOUNDRIES	(1<<1)
#define OPTIMIZE_AUTOEDGE				(1<<2)


void DllExport setUseVisEdge(int b);
int DllExport getUseVisEdge();

// a callback to update progress UI while doing a
// lengthy operation to a mesh
class MeshOpProgress {
	public:
		// called once with the total increments
		virtual void Init(int total)=0;

		// Called to update progress. % done = p/total
		virtual BOOL Progress(int p)=0;
	};

// Boolean operations for meshes:
#define MESHBOOL_UNION 				1
#define MESHBOOL_INTERSECTION  		2
#define MESHBOOL_DIFFERENCE 		3

//
// mesh = mesh1 op mesh2
// If tm1 or tm2 are non-NULL, the points of the corresponding
// mesh will be transformed by these tm before the bool op
// The mesh will be transformed back by either Inverse(tm1) or
// Inverse(tm2) depending whichInv (0=>tm1, 1=>tm2)
// unless whichInv is -1 in which case it will not be transformed
// back.
//
int DllExport CalcBoolOp(
	Mesh &mesh, Mesh &mesh1, Mesh &mesh2, int op,
	MeshOpProgress *prog = NULL,
	Matrix3 *tm1 = NULL,
	Matrix3 *tm2 = NULL,
	int whichInv = 0,
	int weld = TRUE);



DllExport void SetSubSelColor(Point3 *clr);
DllExport Point3 GetSubSelColor();

#endif // _MESH_H_

