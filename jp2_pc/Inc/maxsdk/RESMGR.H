/**********************************************************************
 *<
	FILE: resmgr.h

	DESCRIPTION: Main include file for the resource manager.

	CREATED BY: Don Brittain

	HISTORY:

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef _RESMGR_H_

#define _RESMGR_H_

#include "export.h"

#define STR_BUF_LENGTH	1024
#define STR_MAX_DLLS	4

class DllExport ResourceManager {
public:
	ResourceManager();
	~ResourceManager();

	HINSTANCE	getHInst(int resID);
	void		setAppHInst(HINSTANCE h);
	HINSTANCE	getAppHInst(void)	{ return hAppInst; }

	HCURSOR		getCursor(int id, HINSTANCE hInst = 0);
	HBITMAP		getBitmap(int id, HINSTANCE hInst = 0);
	HMENU		getMenu(int id, HINSTANCE hInst = 0);
	HICON		getIcon(int id, HINSTANCE hInst = 0);
	HACCEL		getAccel(int id, HINSTANCE hInst = 0);
	TCHAR *		getString(int id, HINSTANCE hInst = 0);
	int			getStringLength(int id, HINSTANCE hInst = 0);

	// help system stuff
	void		setClickHelp(int onOff);
	int			getClickHelp()		{ return clickHelp; }
	void		setHelpHWnd(HWND h)	{ helpHWnd = h; }
	HWND		getHelpHWnd()		{ return helpHWnd; }
	void		setHelpFocus(UINT uCommand, DWORD dwData);
	int			doHelpFocus();
	int			help(UINT uCommand, DWORD dwData);

private:
	HINSTANCE	hAppInst;
	TCHAR *		buf;
	int			bufLen;
	int			clickHelp;
	HWND		helpHWnd;
	HCURSOR		helpCursor;
	HCURSOR		savedCursor;
	UINT		focusCmd;
	DWORD		focusData;
};

struct IDPair {
	DWORD CID;
	DWORD HID;
};

DllExport DWORD CIDtoHID(int CID, IDPair *array);
DllExport void SetDialogHelpIDs(HWND hDlg, IDPair *array);

#define RES_ID_APP			0
#define RES_ID_DS			1
#define RES_ID_DB			2
#define RES_ID_RB			3
#define RES_ID_TH			4
#define RES_ID_GG			5

DllExport ResourceManager &getResMgr(void);

// Win32 already has a GetCursor...
#define GetResCursor(a)	        getResMgr().getCursor(a)
#define GetResBitmap(a)	        getResMgr().getBitmap(a)
#define GetResMenu(a)	        getResMgr().getMenu(a)
#define GetResIcon(a)	        getResMgr().getIcon(a)
#define GetResAccel(a)  	    getResMgr().getAccel(a)
#define GetResString(a)	        getResMgr().getString(a)
#define GetResStringLength(a)   getResMgr().getStringLength(a)

// instances
#define appInst			getResMgr().getAppHInst()
#define dbInst			getResMgr().getHInst(RES_ID_DB)
#define dsInst			getResMgr().getHInst(RES_ID_DS)
#define rbInst			getResMgr().getHInst(RES_ID_RB)
#define thInst			getResMgr().getHInst(RES_ID_TH)
#define ggInst			getResMgr().getHInst(RES_ID_GG)

// help system
#define F1Focus(cmd,data)	getResMgr().setHelpFocus(cmd,data)
#define F1Help()			getResMgr().doHelpFocus()
#define Help(cmd,data)		getResMgr().help(cmd, data)
#define GetClickHelp()		getResMgr().getClickHelp()

#endif // _RESMGR_H_
