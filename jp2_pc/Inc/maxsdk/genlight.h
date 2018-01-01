/**********************************************************************
 *<
	FILE: genlight.h

	DESCRIPTION:  Defines General-Purpose lights

	CREATED BY: Tom Hudson

	HISTORY: created 5 December 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __GENLIGHT__H__ 

#define __GENLIGHT__H__

#define OMNI_LIGHT		0	// Omnidirectional
#define TSPOT_LIGHT		1	// Targeted
#define DIR_LIGHT		2	// Directional
#define FSPOT_LIGHT		3	// Free

#define NUM_LIGHT_TYPES	4

// SetAtten types
#define ATTEN_START		0
#define ATTEN_END		1

// Shapes
#define RECT_LIGHT		0
#define CIRCLE_LIGHT	1

class GenLight: public LightObject {			   
public:
	virtual GenLight *NewLight(int type)=0;
	virtual RefResult EvalLightState(TimeValue t, Interval& valid, LightState* cs)=0;
	virtual BOOL IsSpot()=0;
	virtual BOOL IsDir()=0;
	virtual void SetUseLight(int onOff)=0;
	virtual BOOL GetUseLight(void)=0;
	virtual void SetSpotShape(int s)=0;
	virtual int GetSpotShape(void)=0;
	virtual void SetHotspot(TimeValue time, float f)=0;
	virtual float GetHotspot(TimeValue t, Interval& valid = Interval(0,0))=0;
	virtual void SetFallsize(TimeValue time, float f)=0;
	virtual float GetFallsize(TimeValue t, Interval& valid = Interval(0,0))=0;
	virtual void SetAtten(TimeValue time, int which, float f)=0;
	virtual float GetAtten(TimeValue t, int which, Interval& valid = Interval(0,0))=0;
	virtual void SetTDist(TimeValue time, float f)=0;
	virtual float GetTDist(TimeValue t, Interval& valid = Interval(0,0))=0;
	virtual ObjLightDesc *CreateLightDesc(INode *n)=0;
	virtual void SetRGBColor(TimeValue t, Point3& rgb)=0;
	virtual Point3 GetRGBColor(TimeValue t, Interval &valid = Interval(0,0))=0;
	virtual void SetHSVColor(TimeValue t, Point3& hsv)=0;
	virtual Point3 GetHSVColor(TimeValue t, Interval &valid = Interval(0,0))=0;
	virtual void SetIntensity(TimeValue time, float f)=0;
	virtual float GetIntensity(TimeValue t, Interval& valid = Interval(0,0))=0;
	virtual void SetAspect(TimeValue t, float f)=0;
	virtual float GetAspect(TimeValue t, Interval& valid = Interval(0,0))=0;
	virtual void SetConeDisplay(int s, int notify=TRUE)=0;
	virtual BOOL GetConeDisplay(void)=0;
	virtual void SetUseAtten(int s)=0;
	virtual BOOL GetUseAtten(void)=0;
	virtual void SetAttenDisplay(int s)=0;
	virtual BOOL GetAttenDisplay(void)=0;
	virtual void Enable(int enab)=0;
	virtual void SetMapBias(TimeValue t, float f)=0;
	virtual float GetMapBias(TimeValue t, Interval& valid = Interval(0,0))=0;
	virtual void SetMapRange(TimeValue t, float f)=0;
	virtual float GetMapRange(TimeValue t, Interval& valid = Interval(0,0))=0;
	virtual void SetMapSize(TimeValue t, int f)=0;
	virtual int GetMapSize(TimeValue t, Interval& valid = Interval(0,0))=0;
	virtual void SetRayBias(TimeValue t, float f)=0;
	virtual float GetRayBias(TimeValue t, Interval& valid = Interval(0,0))=0;

	virtual int GetUseGlobal()=0;
	virtual void SetUseGlobal(int a)=0;
	virtual int GetShadow()=0;
	virtual void SetShadow(int a)=0;
	virtual int GetShadowType()=0;
	virtual void SetShadowType(int a)=0;
	virtual int GetAbsMapBias()=0;
	virtual void SetAbsMapBias(int a)=0;
	virtual int GetOvershoot()=0;
	virtual void SetOvershoot(int a)=0;

	virtual NameTab& GetExclusionList()=0;
	virtual void SetExclusionList(NameTab &list)=0;

	virtual BOOL SetHotSpotControl(Control *c)=0;
	virtual BOOL SetFalloffControl(Control *c)=0;
	virtual BOOL SetColorControl(Control *c)=0;
	virtual Control* GetHotSpotControl()=0;
	virtual Control* GetFalloffControl()=0;
	virtual Control* GetColorControl()=0;
};


#endif // __GENLIGHT_H__
