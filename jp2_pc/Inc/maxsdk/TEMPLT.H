
/**********************************************************************
 *<
	FILE: templt.h

	DESCRIPTION:  Defines 2D Template Object

	CREATED BY: Tom Hudson

	HISTORY: created 31 October 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __TEMPLT_H__ 

#define __TEMPLT_H__

class PolyLine;
class Spline3D;

// A handy 2D floating-point box class

class Box2D {
	public:
		BOOL empty;
		Point2 min, max;
		Box2D() { empty = TRUE; }
		void SetEmpty() { empty = TRUE; }
		CoreExport Box2D& operator+=(const Point2& p);	// expand this box to include p
	};

// This object is used to test shapes for self-intersection, clockwise status, point
// surrounding and intersection with other templates.  The last and first points will be the
// same if it is closed.

class Template {
	public:
		int points;
		BOOL closed;
		Point2 *pts;
		Template(Spline3D *spline);
		Template(PolyLine *line);
		void Create(PolyLine *line);
		~Template();
		Points() { return points; }
		BOOL SurroundsPoint(Point2& point);
		BOOL IsClockWise();
		BOOL SelfIntersects();
		BOOL Intersects(Template &t);
		Box2D Bound();
	};

#endif // __TEMPLT_H__
