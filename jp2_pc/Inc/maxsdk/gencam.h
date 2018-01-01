/**********************************************************************
 *<
	FILE: gencamera.h

	DESCRIPTION:  Defines General-Purpose cameras

	CREATED BY: Tom Hudson

	HISTORY: created 5 December 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __GENCAM__H__ 

#define __GENCAM__H__

// Camera types
#define FREE_CAMERA 0
#define TARGETED_CAMERA 1

class GenCamera: public CameraObject {			   
	public:
		virtual GenCamera *NewCamera(int type)=0;
		virtual void SetFOV(TimeValue t, float f)=0;
		virtual float GetFOV(TimeValue t, Interval& valid = Interval(0,0))=0;
		virtual void SetTDist(TimeValue t, float f)=0;
		virtual float GetTDist(TimeValue t, Interval& valid = Interval(0,0))=0;
		virtual void SetConeState(int s)=0;
		virtual void SetHorzLineState(int s)=0;
		virtual int GetManualClip()=0;
		virtual void SetManualClip(int onOff)=0;
		virtual float GetClipDist(TimeValue t, int which, Interval &valid = Interval(0,0))=0;
		virtual void SetClipDist(TimeValue t, int which, float f)=0;
		virtual void SetEnvRange(TimeValue time, int which, float f)=0;
		virtual float GetEnvRange(TimeValue t, int which, Interval& valid = Interval(0,0))=0;
		virtual void SetEnvDisplay(BOOL b, int notify=TRUE)=0;
		virtual BOOL GetEnvDisplay(void)=0;
		virtual void Enable(int enab)=0;
		virtual BOOL SetFOVControl(Control *c)=0;

		virtual Control *GetFOVControl()=0;
	};



#endif // __GENCAM__H__
