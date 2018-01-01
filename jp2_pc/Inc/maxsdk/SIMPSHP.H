/**********************************************************************
 *<
	FILE: simpshp.h

	DESCRIPTION:  Defines a simple shape object class to make
		procedural shape primitives easier to create

	CREATED BY: Tom Hudson

	HISTORY: created 30 October 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __SIMPSHP_H__ 

#define __SIMPSHP_H__

class SimpleShape: public ShapeObject {			   
	public:
		IParamBlock *pblock;

		// Shape cache
		PolyShape shape;
		Interval ivalid;

		// Flag to suspend snapping -- Used during creation
		BOOL suspendSnap;

		CoreExport void UpdateShape(TimeValue t);

		static SimpleShape *editOb;

		CoreExport SimpleShape();
		CoreExport ~SimpleShape();

		void ShapeInvalid() { ivalid.SetEmpty(); }

		//  inherited virtual methods:

		// From BaseObject
		CoreExport int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
		CoreExport void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
		CoreExport int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
		CoreExport virtual void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);
		IParamArray *GetParamBlock() {return pblock;}
		CoreExport int GetParamBlockIndex(int id);

		// From Object
		CoreExport ObjectState Eval(TimeValue time);
		CoreExport Interval ObjectValidity(TimeValue t);
		CoreExport int CanConvertToType(Class_ID obtype);
		CoreExport Object* ConvertToType(TimeValue t, Class_ID obtype);
		CoreExport void BuildMesh(TimeValue t, Mesh &mesh);
				
		// From ShapeObject
		CoreExport int IntersectRay(TimeValue t, Ray& r, float& at);
		CoreExport ObjectHandle CreateTriObjRep(TimeValue t);  // for rendering, also for deformation		
		CoreExport void GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );
		CoreExport void GetLocalBoundBox(TimeValue t, INode* inode, ViewExp* vxt, Box3& box );
		CoreExport void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel );
		CoreExport int IsRenderable();
		CoreExport int NumberOfCurves();
		CoreExport BOOL CurveClosed(TimeValue t, int curve);
		BOOL CanMakeBezier() { return TRUE; }			// Return TRUE if can turn into a bezier representation
		CoreExport void MakeBezier(TimeValue t, BezierShape &shape);	// Create the bezier representation
		CoreExport ShapeHierarchy &OrganizeCurves(TimeValue t, ShapeHierarchy *hier=NULL);	// Ready for lofting, extrusion, etc.
		CoreExport void MakePolyShape(TimeValue t, PolyShape &shape, int steps = PSHAPE_BUILTIN_STEPS, BOOL optimize = FALSE);
		CoreExport int MakeCap(TimeValue t, MeshCapInfo &capInfo, int capType);	// Makes a cap out of the shape
		CoreExport int MakeCap(TimeValue t, PatchCapInfo &capInfo);

		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i) {return pblock;}
		void SetReference(int i, RefTargetHandle rtarg) {pblock=(IParamBlock*)rtarg;}		
		CoreExport RefResult NotifyRefChanged(Interval changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);

 		int NumSubs() { return 1; }  
		Animatable* SubAnim(int i) { return pblock; }
		CoreExport TSTR SubAnimName(int i);		

		// Animatable methods
		void DeleteThis() { delete this; }
		CoreExport void FreeCaches(); 

		// IO
		CoreExport IOResult Save(ISave *isave);
		CoreExport IOResult Load(ILoad *iload);

		LRESULT CALLBACK TrackViewWinProc( HWND hwnd,  UINT message, 
	            WPARAM wParam,   LPARAM lParam ){return(0);}

		// Clients of SimpleShape need to implement these methods:
	
		CoreExport virtual TCHAR *GetObjectName() = 0;
		CoreExport virtual void InitNodeName(TSTR& s) = 0;
		CoreExport virtual Class_ID ClassID() = 0;
		CoreExport virtual void GetClassName(TSTR& s) = 0;
		CoreExport virtual void BuildShape(TimeValue t,PolyShape& ashape) = 0;
		CoreExport virtual RefTargetHandle Clone(RemapDir& remap = NoRemap()) = 0;
		CoreExport virtual void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next) = 0;
		CoreExport virtual CreateMouseCallBack* GetCreateMouseCallBack() = 0;
		CoreExport virtual BOOL ValidForDisplay(TimeValue t) = 0;
		CoreExport virtual void InvalidateUI() {}
		CoreExport virtual	ParamDimension *GetParameterDim(int pbIndex) {return defaultDim;}
		CoreExport virtual TSTR GetParameterName(int pbIndex) {return TSTR(_T("Parameter"));}

		// Unlike SimpleSplines, you're probably procedural, so implementing these
		// is a must!
		CoreExport virtual Point3 InterpCurve3D(TimeValue t, int curve, float param) = 0;
		CoreExport virtual Point3 TangentCurve3D(TimeValue t, int curve, float param) = 0;
		CoreExport virtual float LengthOfCurve(TimeValue t, int curve) = 0;
		
		// Here are some optional methods.
		// You should _really_ implement these, because they just do the bare-minimum job
		// (Chopping your curve up into manageable pieces makes things look better)
		CoreExport virtual int NumberOfPieces(TimeValue t, int curve) { return 1; }
		CoreExport virtual Point3 InterpPiece3D(TimeValue t, int curve, int piece, float param) { return InterpCurve3D(t, curve, param); }
		CoreExport virtual Point3 TangentPiece3D(TimeValue t, int curve, int piece, float param) { return TangentCurve3D(t, curve, param); }

	};				


#endif // __SIMPSHP_H__
