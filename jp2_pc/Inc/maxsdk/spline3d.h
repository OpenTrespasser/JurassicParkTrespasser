/**********************************************************************
 *<
	FILE: spline3d.cpp

	DESCRIPTION: General-purpose 3D spline class

	CREATED BY: Tom Hudson & Dan Silva

	HISTORY: created 2/23/95

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __SPLINE3D_H__

#define __SPLINE3D_H__

#include "polyshp.h"	// Need this for PolyLine class

// Point flags for PolyShape representation
#define BEZ_SHAPE_KNOT			(1<<0)	// It's a knot point
#define BEZ_SHAPE_INTERPOLATED	(1<<1)	// It's an interpolated point between two knots

// Line types:
#define LTYPE_CURVE 0
#define LTYPE_LINE 1

// Compound line types
#define CURVE_CURVE (LTYPE_CURVE | (LTYPE_CURVE<<2))
#define LINE_CURVE (LTYPE_LINE | (LTYPE_CURVE<<2))
#define CURVE_LINE (LTYPE_CURVE | (LTYPE_LINE<<2))
#define LINE_LINE (LTYPE_LINE | (LTYPE_LINE<<2))

// Knot types
#define KTYPE_AUTO 0
#define KTYPE_CORNER 1
#define KTYPE_BEZIER 2
#define KTYPE_BEZIER_CORNER (KTYPE_BEZIER | KTYPE_CORNER)

// Parameter types
#define PARM_UNIFORM		0
#define PARM_ARCLENGTH		1
#define PARM_CENTRIPETAL	2
#define PARM_CUSTOM			3

class Spline3D;

class SplineKnot {
	int ktype;
	int ltype;
	Point3 point;
	Point3 inVec;
	Point3 outVec;
public:
	SplineKnot(int k, int l, Point3 p, Point3 in, Point3 out) { ktype=k; ltype=l; point=p; inVec=in; outVec=out; }
	inline	int		Ktype() { return ktype; }
	inline	int		Ltype() { return ltype; }
	friend class Spline3D;
	};

typedef struct {
	int ktype;			// Knot type
	int ltype;			// Line type
	float du;			// Parameter value
	int aux;			// Used in capping
	} Knot;

// Private spline flags
#define SPLINE_CLOSED	(1<<0)

class Spline3D {
private:
	static	int			splineCount;	// Number of splines in the system
			int			parmType;		// Interpolation parameter type	(needed?)
			int			knotCount;		// Number of points in spline

			int			flags;			// Private flags
//			HMENU		hMenuPType;
			int			iCur;			// Current editing point
			int			Interact(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3* mat );	// Handle mouse interaction
			float		cachedLength;
			BOOL		cacheValid;
public:
			// Should consolidate all of these into one knot structure -- Was this way for simple Windows GDI version
			Knot *		knots;		// Knot attributes array
			Point3 *	bezp;		// Bezier point array

			int			drawPhase;		// Drawing phase
			int			editMode;		// 1 if editing, 0 otherwise

			// Creation settings
			int			initialType;	// Knot type at initial click
			int			dragType;		// Knot type at drag

	CoreExport		Spline3D(int itype = KTYPE_CORNER,int dtype = KTYPE_BEZIER,int ptype = PARM_UNIFORM);		// Constructor	
	CoreExport		~Spline3D();	// Destructor
	CoreExport		Spline3D& 	operator=(Spline3D& fromSpline);
	CoreExport		Spline3D& 	operator=(PolyLine& fromLine);
	CoreExport		void		NewSpline();
	CoreExport		void		Allocate(int count);
	inline			int			ParmType() { return parmType; };
	inline			int			KnotCount() { return knotCount; }							// Point (knot) count
	inline			int			Flags() { return flags; }
	inline			int			Segments() { return knotCount + Closed() - 1; }				// Segment count
	inline			int			Closed() { return (flags & SPLINE_CLOSED) ? 1:0; }		// Returns closed status
	CoreExport		int			ShiftKnot(int where,int direction);						// Shove array left or right 1,
																						// starting at given point
	CoreExport		int			AddKnot(SplineKnot &k,int where = -1);						// Add a knot to the spline
	CoreExport		int			DeleteKnot(int where);									// Delete the specified knot
	CoreExport		int			Create(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3* mat);	// Create the spline
	CoreExport		int			StartInsert(ViewExp *vpt,int msg, int point, int flags, IPoint2 theP, Matrix3* mat, int where );	// Start an insertion operation on the spline
	CoreExport		void		ChordParams();							// Compute chord length params
	CoreExport		void		UniformParams();						// Compute uniform params
	CoreExport		void		CentripetalParams();					// Compute centripetal params
	CoreExport		int			SetParam(int index,float param);		// Set custom param value
	CoreExport		float		GetParam(int index);					// Get param value
	inline			int			GetKnotType(int index) { return knots[index].ktype; }
	CoreExport		int			SetKnotType(int index,int type);		// Set the knot type
	inline			int			GetLineType(int index) { return knots[index].ltype; }
	CoreExport		int			SetLineType(int index,int type);		// Set the line type
	virtual			void		CustomParams() { UniformParams(); }		// Replace this as needed
	CoreExport		void		CompParams();							// Compute param values
	CoreExport		void		ComputeBezPoints();
	CoreExport		void		LinearFwd(int i);
	CoreExport		void		LinearBack(int i);
	CoreExport		void		ContinFwd(int i);
	CoreExport		void		ContinBack(int i);
	CoreExport		void		HybridPoint(int i);
	CoreExport		void		CompCornerBezPoints(int n);
	CoreExport		void		CompAdjBesselBezPoints(int i);
	CoreExport		void		BesselStart(int i);
	CoreExport		void		BesselEnd(int i);
	CoreExport		void		NaturalFwd(int i);
	CoreExport		void		NaturalBack(int i);
	CoreExport		Point2		InterpBezier(IPoint2 *bez, float t);
	CoreExport		Point3		InterpBezier3D(int segment, float t);
	CoreExport		Point3		InterpCurve3D(float u);
	CoreExport		Point3		TangentBezier3D(int segment, float t);
	CoreExport		Point3		TangentCurve3D(float u);
	CoreExport		void		BoundingRect(RECT *r, IPoint2 *p, int npoints);
	CoreExport		Point3		AverageTangent(int i);
	CoreExport		void		MakeBezCont(int i);
	CoreExport		void		RedistTangents(int i, Point3 d);
	CoreExport		void		FixAdjBezTangents(int i);
	CoreExport		void		DrawCurve(GraphicsWindow *gw, Material *mtl);
	inline			void		SetEditMode(int mode) { editMode = mode ? 1:0; }
	CoreExport		int			IsAuto(int i);
	CoreExport		int			IsBezierPt(int i);
	CoreExport		int			IsCorner(int i);
	CoreExport		Point3		GetDragVector(ViewExp *vpt,IPoint2 p,int i,Matrix3* mat);
	CoreExport		int			InsertPoint(ViewExp *vpt,int where, IPoint2& p, float t);
	CoreExport		int			AppendPoint(ViewExp *vpt,const Point3& p, int where = -1);
	CoreExport		void		ComputeAdjBezPts(int n);
	CoreExport		int			DrawPhase() { return drawPhase; }
	CoreExport		int			GetiCur() { return iCur; }
	CoreExport		void		CheckMenu(HMENU hMenu, int idfirst, int idlast, int idcheck);
	CoreExport		void		RedrawCurves();
	CoreExport		void		GetBBox(TimeValue t,  Matrix3& tm, Box3& box);
	CoreExport		IPoint2		ProjectPoint(ViewExp *vpt, Point3 fp, Matrix3 *mat);
	CoreExport		Point3		UnProjectPoint(ViewExp *vpt, IPoint2 p, Matrix3 *mat);
	CoreExport		void		Snap(GraphicsWindow *gw, SnapInfo *snap, IPoint2 *p, Matrix3 &tm);
	CoreExport		IOResult 	Save(ISave *isave);
	CoreExport		IOResult	Load(ILoad *iload);
	CoreExport		int			SetClosed(int flag = 1);
	CoreExport		int			SetOpen();
	CoreExport		void		Dump(int where);
	inline			Point3&		InVec(int i) { return bezp[i*3]; }
	inline			Point3&		KnotPoint(int i) { return bezp[i*3+1]; }
	inline			Point3&		OutVec(int i) { return bezp[i*3+2]; }
	inline			Point3&		GetVert(int i) { return bezp[i]; }
	inline			void		SetVert(int i, const Point3& p) { bezp[i] = p; }
	inline			int			Verts() { return knotCount*3; }
	CoreExport		float		SplineLength();
	CoreExport		void		Transform(Matrix3 *tm);
	CoreExport		void		Reverse();
	CoreExport		void		Append(Spline3D *spline);
	CoreExport		void		Prepend(Spline3D *spline);
	CoreExport		BOOL		IsClockWise();			// 2D!
	CoreExport		BOOL		SelfIntersects();		// 2D!
	CoreExport		BOOL		IntersectsSpline(Spline3D *spline);		// 2D!
	CoreExport		BOOL		SurroundsPoint(Point2 p);	// 2D!
	CoreExport		void		MakePolyLine(PolyLine &line, int steps = -1, BOOL optimize = FALSE);
	CoreExport		void		InvalidateGeomCache();
	};

#endif // __SPLINE3D_H__
