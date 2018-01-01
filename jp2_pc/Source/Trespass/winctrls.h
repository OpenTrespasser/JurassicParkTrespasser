//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       winctrls.h
//
//  Contents:   Code for overriding the windows control.
//
//  Classes:    
//
//  Functions:
//
//  History:    05-Jan-98   SHernd   Created
//
//---------------------------------------------------------------------------


#ifndef __WINCTRLS_H__
#define __WINCTRLS_H__

#include "dialogs.h"

#define  BORDER_OFFSET	4

#define GAMEFONTNAME            "Tahoma"
#define DEFAULT_GAMEFONTHEIGHT  12
#define DEFUALT_GAMEFONTWEIGHT  FW_BOLD

#define COLOR_SELECTED_FG       RGB(255, 255, 255)
#define COLOR_SELECTED_BK       RGB(0, 0, 0)
#define COLOR_NORMAL_FG         RGB(0, 128, 192)
#define COLOR_NORMAL_BK         RGB(0, 0, 0)
#define COLOR_DISABLED_FG       RGB(128, 128, 128)
#define COLOR_DISABLED_BK       RGB(0, 0, 0)

#define SELETED_OUTLINE			3
#define CHANGE_SYS_COLOUR_COUNT 7

class CRasterDC;

inline void SetReturnResult(HWND hwnd, LPARAM lParam)
{
    SetWindowLong(hwnd, DWL_MSGRESULT, lParam);
}

void EraseParentBkgndSegment(HWND hwndChild, HDC hdc);
void EraseParentBkgnd(HWND hwndChild, HDC hdc);
BOOL MyDrawEdge(HDC hdc, RECT * prc, UINT uiEdge, UINT grfFlags);
BOOL MyDrawDlgFrame(HDC hdc, RECT * prc);

void DrawTab(const DRAWITEMSTRUCT * lpDrawItem);
HFONT CreateGameFont(int iHeight = -1, int iWeight = -1);
BOOL OnEraseBkgndSegment(HWND hwnd, HDC hdcDst, RECT *rcValues);

extern int g_SelectListbox;

class COwnerSubclassBase : public CMultiWnd
{
public:
    COwnerSubclassBase();
    virtual ~COwnerSubclassBase();

    void Subclass(HWND hwnd);

protected:
    LRESULT BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void OnDestroy(HWND hwnd);

    WNDPROC     m_wndproc;
};

class COwnerCheckbox : public COwnerSubclassBase
{
public:
    COwnerCheckbox();
    virtual ~COwnerCheckbox();

    void Subclass(HWND hwnd);

protected:
    LRESULT BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
    UINT OnNCHitTest(HWND hwnd, int x, int y);
    void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
    void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
    void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
    void OnPaint(HWND hwnd);
    void OnSetFocus(HWND hwnd, HWND hwndOldFocus);
    void OnKillFocus(HWND hwnd, HWND hwndNewFocus);

private:
    bool    m_bTextRect;
    RECT    m_rc;
    RECT    m_rcText;
    HBITMAP m_hbm;
    BITMAP  m_bm;
    int     m_iCheckWidth;
    int     m_iCheckHeight;
    DWORD   m_dwState;
    DWORD   m_dwStyle;
    DWORD   m_dwTextFlags;
    bool    m_bKeyDown;

    void DrawCheckPart(HDC hdc, BOOL bChecked, BOOL bDown);
    void SetupTextRect(HDC hdc);
    void DrawFocusRect(HDC hdc);
};


class COwnerButton : public COwnerSubclassBase
{
public:
    COwnerButton();
    virtual ~COwnerButton();

    BOOL Init(CRasterMem * pRaster);
    void Draw(const DRAWITEMSTRUCT * lpDrawItem);

    UINT OnNCHitTest(HWND hwnd, int x, int y);

protected:

    CRasterMem *    m_prasImg;
    CRasterDC *     m_prasBkgnd;
    int             m_iHeight;
    int             m_iWidth;
    WORD            m_wTrans;
};


class COwnerTab : public COwnerSubclassBase
{
public:
    COwnerTab();
    virtual ~COwnerTab();

    void Subclass(HWND hwnd);

protected:
    LRESULT BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void OnPaint(HWND hwnd);
    BOOL OnEraseBkgnd(HWND hwnd, HDC hdcDst);

private:
    int     m_iAdjustSize;

    void DrawTab(int iTab, BOOL bSel, BOOL bFocus, HDC hdc, RECT * prcBounding);
    void DrawTabEdge(RECT * prc, HDC hdc);
};


class COwnerSlider : public COwnerSubclassBase
{
public:
    COwnerSlider();
    virtual ~COwnerSlider();

protected:
    BOOL    OnEraseBkgnd(HWND hwnd, HDC hdcDst);
    void    OnPaint(HWND hwnd);

private:
    BOOL    ValidDrawStyle();
};


class COwnerEdit : public COwnerSubclassBase
{
public:
    COwnerEdit();
    virtual ~COwnerEdit();
private:
    char        pBuffer[100];
    DWORD       dwParam;
    WORD        wFlags;
	int			cxChar, cxClient, cxBuffer, xCaret;
	int			iCaretHeight;
	SIZE		CaretPos;
	SIZE		HilitePos;
	int			iEndHilite, iStartHilite; // These are really counts
	int			iLastMouseX;
	BOOL		bHiliteFlag;
    DWORD		m_dwState;
protected:
	LRESULT BaseHandler(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
    BOOL OnEraseBkgnd(HWND hwnd, HDC hdcDst);
    void OnPaint(HWND hwnd);
	void ProcessText(HDC hdc, COLORREF crFg, COLORREF crBk, RECT *rcWindow, char *sz);
	void SwapEnds(int *StartHiliteTmp, int *iEndHiliteTmp);
	void OnMouseMove(HWND hwnd, int MouseX, int MouseY, UINT keyFlags);
    void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
    void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
    void OnChar(HWND hwnd, TCHAR ch, int cRepeat);
    void OnSetFocus(HWND hwnd, HWND hwndOldFocus);
    void OnKillFocus(HWND hwnd, HWND hwndNewFocus);
};

class COwnerListBox : public COwnerSubclassBase
{
public:
    COwnerListBox(HWND hwnd);
    virtual ~COwnerListBox();
	LRESULT BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int MouseX, int MouseY, UINT keyFlags);
//+	void OnMouseMove(HWND hwnd, int MouseX, int MouseY, UINT keyFlags);
	void SetUpOwnersList(HWND hwnd);
    void OnSetFocus(HWND hwnd, HWND hwndOldFocus);
    void OnKillFocus(HWND hwnd, HWND hwndNewFocus);

protected:
	int  nLastIndex;
	int	 nSelectedItem;
	BOOL bKeyDownFlag;
    DWORD m_dwState;
	DWORD SaveSysColor[CHANGE_SYS_COLOUR_COUNT];

};

class COwnerScrollBar : public COwnerSubclassBase
{
public:
    COwnerScrollBar();
    virtual ~COwnerScrollBar();
	LRESULT BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int MouseX, int MouseY, UINT keyFlags);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
  //  void OnSetFocus(HWND hwnd, HWND hwndOldFocus);
 //   void OnKillFocus(HWND hwnd, HWND hwndNewFocus);

protected:
    DWORD   m_dwState;
};


class COwnerComboBox : public COwnerSubclassBase
{
public:
    COwnerComboBox();
    virtual ~COwnerComboBox();
	LRESULT BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	HBRUSH OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type);

protected:
	DWORD SaveSysColor[CHANGE_SYS_COLOUR_COUNT];
};


#endif
