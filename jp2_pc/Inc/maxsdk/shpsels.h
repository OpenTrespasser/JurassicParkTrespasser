/**********************************************************************
 *<
	FILE: shpsels.h

	DESCRIPTION:  Defines Shape Selection utility objects

	CREATED BY: Tom Hudson

	HISTORY: created 31 October 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __SHPSELS__ 

#define __SHPSELS__

class BezierShape;
class PolyShape;

class ShapeVSel {
	public:
	int polys;
	BitArray *sel;
	CoreExport ShapeVSel();
	CoreExport ~ShapeVSel();
	CoreExport ShapeVSel& operator=(ShapeVSel& from);
	CoreExport void Insert(int where,int count=0);
	CoreExport void Delete(int where);
	CoreExport void SetSize(BezierShape& shape);
	CoreExport void SetSize(PolyShape& shape);
	CoreExport BitArray& operator[](int index);
	CoreExport void ClearAll();
	CoreExport void SetAll();
	CoreExport void Toggle();
	CoreExport void Empty();
	CoreExport BOOL IsCompatible(BezierShape& shape);
	CoreExport BOOL IsCompatible(PolyShape& shape);
	CoreExport IOResult Save(ISave* isave);
	CoreExport IOResult Load(ILoad* iload);
	};

class ShapeSSel {
	public:
	int polys;
	BitArray *sel;
	CoreExport ShapeSSel();
	CoreExport ~ShapeSSel();
	CoreExport ShapeSSel& operator=(ShapeSSel& from);
	CoreExport void Insert(int where,int count=0);
	CoreExport void Delete(int where);
	CoreExport void SetSize(BezierShape& shape);
	CoreExport void SetSize(PolyShape& shape);
	CoreExport BitArray& operator[](int index);
	CoreExport void ClearAll();
	CoreExport void SetAll();
	CoreExport void Toggle();
	CoreExport void Empty();
	CoreExport BOOL IsCompatible(BezierShape& shape);
	CoreExport BOOL IsCompatible(PolyShape& shape);
	CoreExport IOResult Save(ISave* isave);
	CoreExport IOResult Load(ILoad* iload);
	};

class ShapePSel {
	public:
	int polys;
	BitArray sel;
	CoreExport ShapePSel();
	CoreExport ~ShapePSel();
	CoreExport ShapePSel& operator=(ShapePSel& from);
	CoreExport void Insert(int where);
	CoreExport void Delete(int where);
	CoreExport void SetSize(BezierShape& shape);
	CoreExport void SetSize(PolyShape& shape);
	CoreExport void Set(int index);
	CoreExport void Set(int index, int value);
	CoreExport void Clear(int index);
	CoreExport int operator[](int index) const;
	CoreExport void ClearAll();
	CoreExport void SetAll();
	CoreExport void Toggle();
	CoreExport void Empty();
	CoreExport BOOL IsCompatible(BezierShape& shape);
	CoreExport BOOL IsCompatible(PolyShape& shape);
	CoreExport IOResult Save(ISave* isave);
	CoreExport IOResult Load(ILoad* iload);
	};


#endif __SHPSELS__
