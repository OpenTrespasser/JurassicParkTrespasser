/**********************************************************************
 *<
	FILE: shape.h

	DESCRIPTION:  Defines Basic BezierShape Object

	CREATED BY: Tom Hudson

	HISTORY: created 23 February 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __SHAPE__ 

#define __SHAPE__

#include "shphier.h"
#include "spline3d.h"
#include "shpsels.h"	// Shape selection classes

class BezierShape;

class ShapeSubHitRec {
	private:		
		ShapeSubHitRec *next;
	public:
		DWORD	dist;
		BezierShape*	shape;
		int		poly;
		int		index;
		ShapeSubHitRec( DWORD dist, BezierShape *shape, int poly, int index, ShapeSubHitRec *next ) 
			{ this->dist = dist; this->shape = shape; this->poly = poly; this->index = index; this->next = next; }

		ShapeSubHitRec *Next() { return next; }		
	};

class SubShapeHitList {
	private:
		ShapeSubHitRec *first;
	public:
		SubShapeHitList() { first = NULL; }
		~SubShapeHitList() {
			ShapeSubHitRec *ptr = first, *fptr;
			while ( ptr ) {
				fptr = ptr;
				ptr = ptr->Next();
				delete fptr;
				}
			first = NULL;
			}	

		ShapeSubHitRec *First() { return first; }
		void AddHit( DWORD dist, BezierShape *shape, int poly, int index ) {
			first = new ShapeSubHitRec(dist,shape,poly,index,first);
			}
	};

// Special storage class for hit records so we can know which object was hit
class ShapeHitData : public HitData {
	public:
		BezierShape *shape;
		int poly;
		int index;
		ShapeHitData(BezierShape *shape, int poly, int index)
			{ this->shape = shape; this->poly = poly; this->index = index; }
		~ShapeHitData() {}
	};

// Display flags
#define DISP_VERTTICKS		(1<<0)
#define DISP_BEZHANDLES		(1<<1)
#define DISP_SELVERTS		(1<<10)
#define DISP_SELSEGMENTS	(1<<11)
#define DISP_SELPOLYS		(1<<13)
#define DISP_UNSELECTED		(1<<14)		// Used by loft -- Shape unselected
#define DISP_SELECTED		(1<<15)		// Used by loft -- Shape selected
#define DISP_ATSHAPELEVEL	(1<<16)		// Used by loft -- Shape at current level

// Selection level bits.
#define SHAPE_OBJECT		(1<<0)
#define SHAPE_SPLINE		(1<<1)
#define SHAPE_SEGMENT		(1<<2)
#define SHAPE_VERTEX		(1<<3)

// Flags for sub object hit test

// NOTE: these are the same bits used for object level.
#define SUBHIT_SHAPE_SELONLY	(1<<0)
#define SUBHIT_SHAPE_UNSELONLY	(1<<2)
#define SUBHIT_SHAPE_ABORTONHIT	(1<<3)
#define SUBHIT_SHAPE_SELSOLID	(1<<4)

#define SUBHIT_SHAPE_VERTS		(1<<24)
#define SUBHIT_SHAPE_SEGMENTS	(1<<25)
#define SUBHIT_SHAPE_POLYS		(1<<26)
#define SUBHIT_SHAPE_TYPEMASK	(SUBHIT_SHAPE_VERTS|SUBHIT_SHAPE_SEGMENTS|SUBHIT_SHAPE_POLYS)

class ShapeObject;

class BezierShape {
 		Box3			bdgBox;			// object space--depends on geom+topo
		static int shapeCount;			// Number of shape objects in the system!
		PolyShape pShape;				// PolyShape cache
		int pShapeSteps;				// Number of steps in the cache
		BOOL pShapeOptimize;			// TRUE if cache is optimized
		BOOL pShapeCacheValid;			// TRUE if the cache is current
	public:
		ShapeObject *masterObject;		// If this was derived from a ShapeObject, we maintain a pointer to it

		// Patch capping cache (mesh capping and hierarchy caches stored in PolyShape cache)
		PatchCapInfo patchCap;
		BOOL patchCapCacheValid;

		// The list of splines
		Spline3D **splines;
		int splineCount;

		int steps;						// Number of steps (-1 = adaptive)
		BOOL optimize;					// TRUE optimizes linear segments

		// Selection
		ShapeVSel	vertSel;  		// selected vertices
		ShapeSSel	segSel;  		// selected segments
		ShapePSel	polySel;  		// selected polygons

		// If hit bezier vector, this is its info:
		int bezVecPoly;
		int bezVecVert;

		// Selection level
		DWORD		selLevel;

		// Display attribute flags
		DWORD		dispFlags;

		CoreExport BezierShape();
		CoreExport BezierShape(BezierShape& fromShape);

		CoreExport void		Init();

		CoreExport ~BezierShape();

		CoreExport BezierShape& 		operator=(BezierShape& fromShape);
		CoreExport BezierShape& 		operator=(PolyShape& fromShape);
		
		CoreExport Point3&	GetVert(int poly, int i);
		CoreExport void		SetVert(int poly, int i, const Point3 &xyz);
		
		CoreExport void		Render(GraphicsWindow *gw, Material *ma, RECT *rp, int compFlags);
		CoreExport BOOL		Select(GraphicsWindow *gw, Material *ma, HitRegion *hr, int abortOnHit = FALSE);
		CoreExport void		Snap(GraphicsWindow *gw, SnapInfo *snap, IPoint2 *p, Matrix3 &tm);
		// See polyshp.h for snap flags
		CoreExport void		Snap(GraphicsWindow *gw, SnapInfo *snap, IPoint2 *p, Matrix3 &tm, DWORD flags);
		CoreExport BOOL 	SubObjectHitTest(GraphicsWindow *gw, Material *ma, HitRegion *hr,
								DWORD flags, SubShapeHitList& hitList );

		CoreExport void		BuildBoundingBox(void);
		CoreExport Box3		GetBoundingBox(Matrix3 *tm=NULL); // RB: optional TM allows the box to be calculated in any space.
				                                              // NOTE: this will be slower becuase all the points must be transformed.
		
		CoreExport void		InvalidateGeomCache();
		CoreExport void		InvalidateCapCache();
		CoreExport void		FreeAll(); //DS
				
		// functions for use in data flow evaluation
		CoreExport void 	ShallowCopy(BezierShape *ashape, unsigned long channels);
		CoreExport void 	DeepCopy(BezierShape *ashape, unsigned long channels);
		CoreExport void		NewAndCopyChannels(unsigned long channels);
		CoreExport void 	FreeChannels( unsigned long channels, int zeroOthers=1);

		// Display flags
		CoreExport void		SetDispFlag(DWORD f);
		CoreExport DWORD	GetDispFlag(DWORD f);
		CoreExport void		ClearDispFlag(DWORD f);

		// Constructs a vertex selection list based on the current selection level.
		CoreExport BitArray 	VertexTempSel(int poly);

		CoreExport IOResult Save(ISave* isave);
		CoreExport IOResult Load(ILoad* iload);

		// BezierShape-specific methods

		inline int SplineCount() { return splineCount; }
		CoreExport Spline3D* GetSpline(int index);
		CoreExport Spline3D* NewSpline(int itype = KTYPE_CORNER,int dtype = KTYPE_BEZIER,int ptype = PARM_UNIFORM);
		CoreExport Spline3D* AddSpline(Spline3D* spline);
		CoreExport int DeleteSpline(int index);
		CoreExport int InsertSpline(Spline3D* spline, int index);
		CoreExport void NewShape();
		CoreExport int GetNumVerts();
		CoreExport int GetNumSegs();
		CoreExport void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel );
		CoreExport void UpdateSels();
		CoreExport void GetClosures(BitArray& array);
		CoreExport void SetClosures(BitArray& array);
		CoreExport float FindSegmentPoint(int poly, int segment, GraphicsWindow *gw, Material *ma, HitRegion *hr);
		CoreExport void Reverse(int poly);
		CoreExport void Reverse(BitArray &reverse);
		CoreExport ShapeHierarchy &OrganizeCurves(TimeValue t, ShapeHierarchy *hier = NULL);
		CoreExport void MakePolyShape(PolyShape &pshp, int steps = -1, BOOL optimize = FALSE);
		CoreExport void MakeFirst(int poly, int vertex);
		CoreExport void Transform(Matrix3 &tm);
		CoreExport BezierShape& operator+=(BezierShape& from);
		CoreExport void ReadyCachedPolyShape();
		CoreExport int MakeCap(TimeValue t, MeshCapInfo &capInfo, int capType);
		CoreExport int MakeCap(TimeValue t, PatchCapInfo &capInfo);
		CoreExport int ReadyPatchCap();
	};

#endif // __SHAPE__
