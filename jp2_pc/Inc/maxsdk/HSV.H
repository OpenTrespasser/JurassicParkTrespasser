/**********************************************************************
 *<
	FILE: hsv.h

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __HSV__H
#define __HSV__H


#define MAXCOLORS 16


// This callback proc gets called after every mouse button up to tell you the
// new color, if you want to do interactive update.

class HSVCallback {
	public:
		virtual	void ButtonDown() {}
		virtual	void ButtonUp(BOOL accept) {}
		virtual	void ColorChanged(DWORD col, BOOL buttonUp)=0;	   										
		virtual	void BeingDestroyed(IPoint2 pos)=0;	// gets called when picker is closed: 
	};

// Put up the dialog.
extern CoreExport int HSVDlg_Do(
	HWND hwndOwner, 		// owning window
	DWORD *lpc,				// pointer to color to be edited
    IPoint2 *spos, 			// starting position, set to ending position
    HSVCallback *callBack,	// called when color changes
	TCHAR *name				// name of color being edited
    );

CoreExport void RGBtoHSV (DWORD rgb, int *ho, int *so, int *vo);
CoreExport DWORD HSVtoRGB (int H, int S, int V);
CoreExport void HSVtoHWBt (int h, int s, int v, int *ho, int *w, int *bt);
CoreExport void HWBttoHSV (int h, int w, int bt, int *ho, int *s, int *v);

// RB: Added floating point versions
class Color;
CoreExport Color RGBtoHSV(Color rgb);
CoreExport Color HSVtoRGB(Color hsv);


// MODELESS Version

class ColorPicker {
	public:
		ColorPicker() {}
		virtual	~ColorPicker() {};
		virtual void ModifyColor (DWORD color)=0;
		virtual void SetNewColor (DWORD color, TCHAR *name)=0;  
		virtual DWORD GetColor()=0;
		virtual IPoint2 GetPosition()=0;
		virtual void Destroy()=0;  //when parent is going away.
		virtual void InstallNewCB(DWORD col, HSVCallback *pcb, TCHAR *name)=0;
	};

CoreExport ColorPicker *CreateColorPicker(HWND hwndOwner, DWORD initColor,
	 IPoint2* spos, HSVCallback *pcallback, TCHAR *name, int objClr=0);
	 
CoreExport void SetCPInitPos(IPoint2 &pos);
CoreExport IPoint2 GetCPInitPos(void);	

#define WM_ADD_COLOR	(WM_USER+2321)	// wParam = color

#endif
