/**********************************************************************
 *<
	FILE: simpmod.h

	DESCRIPTION:  Simple modifier base class

	CREATED BY: Dan Silva & Rolf Berteig

	HISTORY: created 30 Jauary, 1995

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __SIMPMOD__
#define __SIMPMOD__


//--- SimpleMod -----------------------------------------------------------

class SimpleMod: public Modifier {
	public:	
		Control *tmControl;
		Control *posControl;
		IParamBlock *pblock;
				
		CoreExport static IObjParam *ip;
		static MoveModBoxCMode *moveMode;
		static RotateModBoxCMode *rotMode;
		static UScaleModBoxCMode *uscaleMode;
		static NUScaleModBoxCMode *nuscaleMode;
		static SquashModBoxCMode *squashMode;		
		static SimpleMod *editMod;
			
		CoreExport SimpleMod();
		CoreExport virtual ~SimpleMod();

		ChannelMask ChannelsUsed()  { return PART_GEOM|PART_TOPO|SELECT_CHANNEL|SUBSEL_TYPE_CHANNEL; }
		ChannelMask ChannelsChanged() { return PART_GEOM; }
		CoreExport void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Class_ID InputType() {return defObjectClassID;}
		CoreExport Interval LocalValidity(TimeValue t);
		CoreExport Matrix3 CompMatrix(TimeValue t, ModContext& mc, Matrix3& ntm, 
			Interval& valid, BOOL needOffset);
		CoreExport void CompOffset( TimeValue t, Matrix3& offset, Matrix3& invoffset);

		// From BaseObject
		CoreExport int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
		CoreExport int Display(TimeValue t, INode* inode, ViewExp *vpt, int flagst, ModContext *mc);
		CoreExport void GetWorldBoundBox(TimeValue t,INode* inode, ViewExp *vpt, Box3& box, ModContext *mc);
		
		CoreExport void GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		CoreExport void GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		BOOL ChangeTopology() {return FALSE;}

		CoreExport IParamArray *GetParamBlock();
		CoreExport int GetParamBlockIndex(int id);

		// Affine transform methods
		CoreExport void Move( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE );
		CoreExport void Rotate( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin=FALSE );
		CoreExport void Scale( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE );
		
		int NumRefs() {return 3;}
		CoreExport RefTargetHandle GetReference(int i);
		CoreExport void SetReference(int i, RefTargetHandle rtarg);

		int NumSubs() {return 3;}
		CoreExport Animatable* SubAnim(int i);
		CoreExport TSTR SubAnimName(int i);

		CoreExport RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);

		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 
		CoreExport void ActivateSubobjSel(int level, XFormModes& modes);

		// When clients are cloning themselves, they should call this 
		// method on the clone to copy SimpleMod's data.
		CoreExport void SimpleModClone(SimpleMod *smodSource);

		// Clients of simpmod probably want to override these. If they do
		// the should call these from within thier methods.
		CoreExport void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		CoreExport void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

		// Clients of simpmod need to implement this method
		virtual Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)=0;
		virtual void InvalidateUI() {}
		virtual Interval GetValidity(TimeValue t) {return FOREVER;}
		virtual	ParamDimension *GetParameterDim(int pbIndex) {return defaultDim;}
		virtual TSTR GetParameterName(int pbIndex) {return TSTR(_T("Parameter"));}
		virtual BOOL GetModLimits(TimeValue t,float &zmin, float &zmax, int &axis) {return FALSE;}
	};


// This is the ref ID of the parameter block
#define SIMPMOD_PBLOCKREF	2


//--- SimpleWSMMod -----------------------------------------------------------

class SimpleWSMMod: public Modifier {	
	public:
		WSMObject  	*obRef;
		INode       *nodeRef;
		IParamBlock *pblock;
				
		CoreExport static IObjParam *ip;
		static SimpleWSMMod *editMod;
	
		CoreExport SimpleWSMMod();
		CoreExport virtual ~SimpleWSMMod();

		ChannelMask ChannelsUsed()  { return PART_GEOM|PART_TOPO; }
		ChannelMask ChannelsChanged() { return PART_GEOM; }
		CoreExport void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Class_ID InputType() {return defObjectClassID;}
		CoreExport Interval LocalValidity(TimeValue t);		
		BOOL ChangeTopology() {return FALSE;}
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}

		int NumRefs() {return 3;}
		CoreExport RefTargetHandle GetReference(int i);
		CoreExport void SetReference(int i, RefTargetHandle rtarg);

		int NumSubs() {return 1;}
		CoreExport Animatable* SubAnim(int i);
		CoreExport TSTR SubAnimName(int i);

		CoreExport RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);
		
		CoreExport IParamArray *GetParamBlock();
		CoreExport int GetParamBlockIndex(int id);

		// Evaluates the node reference and returns the WSM object.
		CoreExport WSMObject *GetWSMObject(TimeValue t);
				
		// When clients are cloning themselves, they should call this 
		// method on the clone to copy SimpleMod's data.
		CoreExport void SimpleWSMModClone(SimpleWSMMod *smodSource);

		// Clients of simpmod probably want to override these. If they do
		// the should call these from within thier methods.
		CoreExport void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		CoreExport void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

		// Clients of simpmod need to implement this method
		virtual Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)=0;
		virtual void InvalidateUI() {}
		virtual Interval GetValidity(TimeValue t) {return FOREVER;}
		virtual	ParamDimension *GetParameterDim(int pbIndex) {return defaultDim;}
		virtual TSTR GetParameterName(int pbIndex) {return TSTR(_T("Parameter"));}
		virtual void InvalidateParamMap() {}
	};


#define SIMPWSMMOD_OBREF		0
#define SIMPWSMMOD_NODEREF		1
#define SIMPWSMMOD_PBLOCKREF	2


#endif

