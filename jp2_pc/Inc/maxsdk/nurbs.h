/**********************************************************************
 *<
	FILE: NURBS.h

	DESCRIPTION: Main include file for Non Uniform Rational B-Splines

	CREATED BY: Steve Anderson

	HISTORY: Created Dec 31, 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef _NURBS_H_

#define _NURBS_H_

#include <hitdata.h>

// Value for undefined patches and vertices
#define NURBS_UNDEFINED -1

#define MULTI_PROCESSING	TRUE		// TRUE turns on mp vertex transformation

typedef Tab<int> IntTab;

class ISave;
class ILoad;
class NurbsMesh;

// NRVertex flags: contain clip flags, number of normals at the vertex
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

class NRVertex { // NURBS vertex for rendering, like PRVertex in patch.h
	public:
		NRVertex()	{ rFlags = 0; }
		DllExport ~NRVertex() {}	

		DWORD		rFlags;     
		union {
			int		iPos[3];	
			float	fPos[3];
			};
	};					  

// Nurbs vertex
// (Used in NurbsMesh, not in NurbsCurve.)
class NurbsVert {
  public:
	Point3 p;			// Location
	float w;			// p = pw/w
	IntTab faces;		// List of NURBS faces using this vertex
	DWORD flags;		// Currently no flags
		
	// General utilities
	NurbsVert() { p = Point3(0,0,0); w = 1.0f; flags = 0; }
	DllExport NurbsVert(const NurbsVert &from) { (*this) = from; }
	~NurbsVert() { ResetData(); }
	DllExport NurbsVert& operator=(const NurbsVert& from);
	void ResetData() { faces.Delete(0,faces.Count()); }
		
	// Face reference handling
	DllExport int FindFace(int index);
	DllExport void AddFace(int index);
	DllExport void DeleteFace(int index);

	// I/O
	DllExport IOResult Save(ISave* isave);
	DllExport IOResult Load(ILoad* iload);
	};

// NurbsCurve: Not actually used yet; ideally both an independent object
// and a trim curve container in NurbsEdge.
class NurbsCurve {
  public:
	DllExport NurbsCurve() { Init(); }
	NurbsCurve(const NurbsCurve & cv) { Init(); (*this) = cv; }
	DllExport NurbsCurve(int ordd, int numm);
	virtual ~NurbsCurve() { Clear(); }

	// inquiry methods
	DllExport int getOrder()		const { return ord; }
	DllExport int getNumber()		const { return num; }

	// editing methods
	DllExport void		setControlPoint(int ind, const Point3 & p);
	DllExport Point3 &	getControlPoint (int ind) const;
	DllExport void		setWeight (int ind, float w);
	DllExport float		getWeight (int ind) const;

	// evaluation methods
	DllExport void Evaluate(float u, Point3 & p) const;
	DllExport void Evaluate(float u, Point3 & p, Point3& dout) const;
	
	// invert methods
	// int Inverter(const Point3 & p1, float & u, double & dis, double init_value = -1.0) const;
	
	DllExport NurbsCurve & operator = (const NurbsCurve & cv);
	void Init ();
	void Clear();

	// I/O
	DllExport IOResult Save(ISave* isave);
	DllExport IOResult Load(ILoad* iload);

  private:
    int		ord;		// Order of polynomials.  Order 3 = cubic, etc.
    int		num;		// Number of points.
    Point3	*verts;
	float	*wghts;
    float 	*knots;		// Knot vector: length num+ord+1
	char	rational;	// Flag to make computation easier if rational.	 (Ignore weights.)
};

class NurbsEdge {
	public:
		int partner;		// "Joined" edge on adjacent face.
		int parthost;		// host face index of partner
		int rendedge;		// Used in converting to mesh

		DllExport NurbsEdge () { Init(); }
		void Init ();
		NurbsEdge& operator = ( const NurbsEdge& from);

		// I/O
		DllExport IOResult Save(ISave* isave);
		DllExport IOResult Load(ILoad* iload);
};

DllExport void NurbsEdgeEvaluate (int type, float t, Point3 & p);

// NurbsFace: this is what you'd call a Patch if it was Bezier, but
// I'm staying away from non-Nurbs patch terminology.

// NurbsFace flags:
// None yet.

class NurbsFace {
  public:
	int			num;			// == uNum*vNum
	int			uTess, vTess;	// Tesselation resolutions
    int			uOrd, vOrd;		// polynomial orders in each dim.
	int			uNum, vNum;		// Numbers of points in each dim.
    float		*uKnot, *vKnot;	// Knot vectors
    int			*verts;			// Indices of verts in NurbsMesh.  (Weights handled there too.)
    NurbsEdge	edges[4];		// U=0, V=0, U=1, V=1
	DWORD		smGroup;		// Defaults to 1 -- All faces smoothed in a NurbsMesh
	DWORD		flags;			// See flags above.

    DllExport NurbsFace() { Init(); }
	NurbsFace (const NurbsFace & from) { Init(); (*this) = from; }
	DllExport NurbsFace (int uO, int vO, int uN, int vN);
    DllExport ~NurbsFace() { Clear(); }

    // evaluation methods -- need to handle these on higher level...
    DllExport void Evaluate (NurbsVert *vtx, float u, float v, Point3 & p) const;
    DllExport void Evaluate (NurbsVert *vtx, float u, float v, Point3 & p, Point3 & pu, Point3 & pv) const;

    //*************** operators *************************
    DllExport NurbsFace & operator = (const NurbsFace & su);
	DllExport void	Init();
	DllExport void	Clear();
	DllExport void	MakeKnotsBezier (int uO, int vO);
	DllExport void	MakeKnotsUniform (int uO, int vO, int uN, int vN);
	DllExport void	setVert (int row, int col, int vid);
	DllExport void	setVert (int ndex, int vid);
	DllExport int	getVert (int row, int col) const;
	DllExport int	getVert (int ndex) const;
/*
	DllExport int	CountU (int *ucl=NULL) { MeasureKnots(); if (ucl) *ucl=closedu; return distinctu; }
	DllExport int	CountV (int *vcl=NULL) { MeasureKnots(); if (vcl) *vcl=closedv; return distinctv; }
	void MeasureKnots ();
	DllExport void	getDistinctKnotU (float *knot, int *mults=NULL) const;
	DllExport void	getDistinctKnotV (float *knot, int *mults=NULL) const;
*/
    //gives uv values at a point on the surface (input is a Point)
    /*
    int		Inverter(Point & pt, Point & uv, double & dis, const Point & guess_uv) const;
    int		Inverter(Point & pt, Point & uv, double & dis) const { return Inverter(pt, uv, dis, *(Point *) NULL); }
	*/
	DllExport IOResult Save(ISave* isave);
	DllExport IOResult Load(ILoad* iload);
};

// The following #defines and Hit-related classes have been cut/pasted
// from patch code.  Not even sure if it's used, offhand.

// Render flag definitions
#define COMP_TRANSFORM	0x0001	// forces recalc of model->screen transform; else will attempt to use cache
#define COMP_IGN_RECT	0x0002	// forces all polys to be rendered; else only those intersecting the box will be
#define COMP_LIGHTING	0x0004	// forces re-lighting of all verts (as when a light moves); else only relight moved verts

#define COMP_ALL		0x00ff

// If this bit is set then the node being displayed by this mesh is selected.
// Certain display flags only activate when this bit is set.
#define COMP_OBJSELECTED	(1<<8)
#define COMP_OBJFROZEN		(1<<9)


// Special types for patch vertex hits -- Allows us to distinguish what they hit on a pick
#define NURBS_HIT_FACE		0
#define NURBS_HIT_VERTEX	1

class NurbsSubHitRec {
	private:		
		NurbsSubHitRec *next;
	public:
		DWORD	dist;
		NurbsMesh *nmesh;
		int		index;
		int		type;

		NurbsSubHitRec( DWORD dist, NurbsMesh *nmesh, int index, int type, NurbsSubHitRec *next ) 
			{ this->dist = dist; this->nmesh = nmesh; this->index = index; this->type = type; this->next = next; }

		NurbsSubHitRec *Next() { return next; }		
	};

class SubNurbsHitList {
	private:
		NurbsSubHitRec *first;
	public:
		SubNurbsHitList() { first = NULL; }
		~SubNurbsHitList() {
			NurbsSubHitRec *ptr = first, *fptr;
			while ( ptr ) {
				fptr = ptr;
				ptr = ptr->Next();
				delete fptr;
				}
			first = NULL;
			}	

		NurbsSubHitRec *First() { return first; }
		void AddHit( DWORD dist, NurbsMesh *patch, int index, int type ) {
			first = new NurbsSubHitRec(dist,patch,index,type,first);
			}
	};


// Special storage class for hit records so we can know which object was hit
class NurbsHitData : public HitData {
	public:
		NurbsMesh *nmesh;
		int index;
		int type;
		NurbsHitData(NurbsMesh *nmesh, int index, int type)
			{ this->nmesh = nmesh; this->index = index; this->type = type; }
	};

// Flags for sub object hit test

// NOTE: these are the same bits used for object level.
#define SUBHIT_NURBS_SELONLY	(1<<0)
#define SUBHIT_NURBS_UNSELONLY	(1<<2)
#define SUBHIT_NURBS_ABORTONHIT	(1<<3)
#define SUBHIT_NURBS_SELSOLID	(1<<4)

#define SUBHIT_NURBS_VERTS		(1<<24)
#define SUBHIT_NURBS_FACES		(1<<25)
#define SUBHIT_NURBS_TYPEMASK	(SUBHIT_NURBS_VERTS|SUBHIT_NURBS_FACES)


// Display flags -- not used.
#define DISP_VERTTICKS		(1<<0)

#define DISP_SELVERTS		(1<<10)
#define DISP_SELFACES		(1<<11)

#define DISP_LATTICE		(1<<16)
#define DISP_VERTS			(1<<17)

// Selection level bits.
#define NURBS_OBJECT		(1<<0)
#define NURBS_VERTEX		(1<<1)
#define NURBS_FACE			(1<<2)

// NurbsMesh: contains faces, edges, verts just like regular mesh.

class NurbsMesh {
	private:
		// derived data-- can be regenerated
		NRVertex 		*rVerts;		// <<< instance specific.
		GraphicsWindow 	*cacheGW;  		// identifies rVerts cache
		Box3			bdgBox;			// object space--depends on geom+topo

		int			snapVCt;
		char		*snapV;
 
		DWORD  		flags;	// work flags- 	None used just yet.

		int 		renderFace (GraphicsWindow *gw, int index);
		void		checkRVertsAlloc(void);
		void		setCacheGW(GraphicsWindow *gw)	{ cacheGW = gw; }
		GraphicsWindow *getCacheGW(void)			{ return cacheGW; }

		void 		freeVerts();
		void  		freeFaces();
		void  		freeRVerts();
		void		freeFaceMtlIndexList();  
		void		freeSnapData();
		int			buildSnapData(GraphicsWindow *gw);

	public:
		// Topology
		int			numVerts;
		int	 		numFaces;
		NurbsVert *	verts;
		NurbsFace *	faces;

		DWORD		mtlIndex;		// object material
		DWORD *		faceMtlIndex;	// material per face
		BitArray	vertSel;  		// selected vertices
		BitArray	faceSel;  		// selected patches
		DWORD		dispFlags;		// Display attribute flags
		DWORD		selLevel;		// Selection level

		DllExport NurbsMesh() { Init(); }
		DllExport NurbsMesh(const NurbsMesh& from) { Init(); (*this) = from; }
		DllExport ~NurbsMesh() { FreeAll(); }

		DllExport void Init();
		DllExport void FreeAll ();
		DllExport NurbsMesh& operator=(const NurbsMesh& from);
		
		DllExport BOOL		setNumVerts(int ct, BOOL keep = FALSE);
		int					getNumVerts(void) const	{ return numVerts; }
		
		DllExport BOOL		setNumFaces(int ct, BOOL keep = FALSE);
		int					getNumFaces(void) const	{ return numFaces; }
		
		void		setVert(int i, const Point3 &xyz)			{ verts[i].p = xyz; }
		void		setVert(int i, float x, float y, float z)	{ verts[i].p.x=x; verts[i].p.y=y; verts[i].p.z=z; }
		void		setWeight (int i, float w)					{ verts[i].w = w; }
		void		setVert(int i, float x, float y, float z, float w) { verts[i].p = Point3(x,y,z); verts[i].w = w; }
		void		setVert(int i, const Point3 &xyz, float w)	{ verts[i].p = xyz; verts[i].w = w; }

		NurbsVert &	getVert(int i)		const	{ return verts[i];  }
		NurbsVert *	getVertPtr(int i)	const	{ return verts+i; }
		NRVertex &	getRVert(int i)		const	{ return rVerts[i]; }
		NRVertex *	getRVertPtr(int i)	const	{ return rVerts+i; }
		
		void		setMtlIndex(DWORD i)			{ mtlIndex = i; }
		DWORD		getMtlIndex(void)		const	{ return mtlIndex; }
		DWORD		getFaceMtlIndex(int i)	const	{ return faceMtlIndex? faceMtlIndex[i]: mtlIndex; }

		// Automatically update all the adjacency info, etc.
		// Returns TRUE if patch mesh is valid, FALSE if it's not!
		DllExport BOOL		buildLinkages();
		
		DllExport void		InvalidateGeomCache();
		DllExport void		render(GraphicsWindow *gw, Material *ma, RECT *rp, int compFlags);
		DllExport BOOL		select(GraphicsWindow *gw, Material *ma, HitRegion *hr, int abortOnHit = FALSE);
		DllExport void		snap(GraphicsWindow *gw, SnapInfo *snap, IPoint2 *p, Matrix3 &tm);
		DllExport BOOL 		SubObjectHitTest(GraphicsWindow *gw, Material *ma, HitRegion *hr,
								DWORD flags, SubNurbsHitList& hitList );

		DllExport void		buildBoundingBox(void);
		DllExport Box3		getBoundingBox(Matrix3 *tm=NULL); // RB: optional TM allows the box to be calculated in any space.
		                                              // NOTE: this will be slower becuase all the points must be transformed.
		
		// Join two faces along one of the four standard edges.
		DllExport void		joinFaces (int f1, int f2, int edgetype);
		DllExport BOOL		EdgeDegenerate (int f, int e, float tolerance);

		// For meshing functions:
		//DllExport void	CountTesselatedParams (int *tvct, int *tfct, int steps);
		DllExport void		Evaluate (int f, float u, float v, Point3& p);
		DllExport void		Evaluate (int f, float u, float v, Point3& p, Point3& du, Point3& dv);

		// functions for use in data flow evaluation
		DllExport void 		ShallowCopy(NurbsMesh *amesh, unsigned long channels);
		DllExport void 		DeepCopy(NurbsMesh *amesh, unsigned long channels);
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
	};

// profiling constants
//#define BUILD_NORMALS	0
#define TRANSFORM_VERTS	1
#define VISEDGE_LIST	2
#define RENDER_PATCHES	3
#define BACK_CULL		4
#define NURBS_PROF_PARTS	5	// always last


#endif // _NURBS_H_
