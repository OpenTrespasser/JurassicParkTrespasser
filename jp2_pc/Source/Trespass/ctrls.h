//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1996.
//
//  File:       ctrls.h
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    20-Nov-96   SHernd   Created
//
//---------------------------------------------------------------------------



#ifndef __CTRLS_H__
#define __CTRLS_H__

#include "rasterdc.hpp"

class CRaster;
class CUICtrlCallback;
class CUICtrl;
class CUIButton;
class CUICheckbox;
class CUITextbox;
class CUISlider;
class CUIHotspot;

typedef enum tagBUTTONCMD
{
    BC_UP = 0,
    BC_DOWN,
    BC_OVER,
} BUTTONCMD;


class CUICtrlCallback
{
public:
    CUICtrlCallback(){;}
    virtual ~CUICtrlCallback(){;}

    virtual CUICtrl *   CaptureMouse(CUICtrl * pctrl){return NULL;}
    virtual void        ReleaseMouse(CUICtrl * pctrl){;}
    virtual void        RegisterUIOver(BOOL fEnter, CUICtrl *pctrl) {;}

    virtual CUICtrl *   SetFocus(CUICtrl * pctrl){return NULL;}
    virtual CUICtrl *   GetFocus(){return NULL;}

    virtual void    CtrlInvalidateRect(RECT * prc) {;}

    virtual void    UIButtonUp(CUIButton * pbutton) {;}
    virtual BOOL    UIButtonAudioCmd(CUIButton * pbutton, BUTTONCMD bc) {return TRUE;}
    virtual void    UICheck(CUICheckbox * pcheckbox) {;}
    virtual void    UIDoubleClick(CUICtrl * pctrl) {;}
    virtual void    UIListboxClick(CUICtrl * pctrl, int iIndex) {;}
    virtual void    UIListboxDblClk(CUICtrl * pctrl, int iIndex) {;}
    virtual void    UIEditboxKey(CUICtrl * pctrl, UINT vk, int cRepeat, UINT flags) {;}
    virtual void    UIEditboxMaxText(CUICtrl * pctrl) {;}
    virtual void    UISliderChange(CUISlider * pctrl, int iNewValue) {;}
    virtual void    UIHotspotClick(CUIHotspot * pctrl, BOOL bDown) {;}
};


class CUICtrl
{
public:
    CUICtrl(CUICtrlCallback * pParent);
    virtual ~CUICtrl();

    virtual void    DoFrame(POINT ptMouse);
    virtual void    Draw(CRaster * pRaster, RECT * prcClip);
    virtual void    Draw(LPBYTE pbDst,
                         int iDstWidth,
                         int iDstHeight,
                         int iDstPitch,
                         int iDstBytes,
                         RECT * prcClip) {;}

    virtual BOOL    HitTest(int x, int y);
    virtual BOOL    HitTest(POINT pt);

    virtual BOOL    GetActive();
    virtual BOOL    SetActive(BOOL bActive);

    virtual DWORD   GetID();
    virtual void    SetID(DWORD dwID);

    virtual BOOL    GetVisible();
    virtual BOOL    SetVisible(BOOL bVisible);

    virtual void    GetRect(RECT * prc);
    virtual void    SetRect(RECT * prc);

    virtual void    SetFocus(BOOL bFocus);
    virtual BOOL    GetFocus();

    virtual void    ReleaseCapture() {;}

    virtual void    MouseMove(int x, int y, UINT keyFlags);
    virtual BOOL    LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags);
    virtual BOOL    LButtonUp(int x, int y, UINT keyFlags);

    virtual BOOL    TokenLoad(HANDLE hFile);
protected:
    CUICtrlCallback *   m_pParent;

    BOOL            m_bFocus : 1;
    BOOL            m_bActive : 1;
    BOOL            m_bVisible : 1;
    DWORD           m_dwID;
    RECT            m_rc;

    void InvalidateSelf() { if (m_pParent) m_pParent->CtrlInvalidateRect(&m_rc); }
};

class CUIStatic : public CUICtrl
{
public:
    CUIStatic(CUICtrlCallback * pParent);
    virtual ~CUIStatic();

    BOOL    TokenLoad(HANDLE hFile);

    void    Draw(CRaster * pDst, RECT * prcClip);
    BOOL    SetRaster(CRaster * pRaster, bool bTrans = TRUE);

    void    Init();

private:
    bool        m_bTrans;
    WORD        m_wTrans;
    CRaster *   m_pRaster;

    WORD        GetTransColor();
};


typedef enum
{
    CUIBUTTON_UP,
    CUIBUTTON_OVER,
    CUIBUTTON_DOWN,
    CUIBUTTON_INACTIVE
};

class CUIButton : public CUICtrl
{
public:
    CUIButton(CUICtrlCallback * pParent);
    virtual ~CUIButton();

    virtual void    DoFrame(POINT ptMouse);
    virtual void    Draw(CRaster * pRaster, RECT * prcClip);

    virtual void    MouseMove(int x, int y, UINT keyFlags);
    virtual BOOL    LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags);
    virtual BOOL    LButtonUp(int x, int y, UINT keyFlags);

    virtual BOOL    TokenLoad(HANDLE hFile);
    virtual BOOL    SetRaster(CRaster * psprite, int iState);
    virtual CRaster * GetRaster();

    virtual BOOL    HitTest(POINT pt);
    virtual void    Init();

    virtual void    ReleaseCapture();

protected:
    UINT             m_uiCmd;
    WORD             m_wTrans;
    BOOL             m_bUseTrans : 1;
    BOOL             m_bOver : 1;
    BOOL             m_bDown : 1;
    BOOL             m_bCaptured : 1;
    CRaster *        m_apRaster[4];
};


#define IDSCROLLUP                      1
#define IDSCROLLDN                      2

#define UILB_TEXTCALLBACK               1

typedef struct tagCUILISTBOXINFO
{
    LPSTR       psz;
    DWORD       dwParam;
    WORD        wFlags;
} CUILISTBOXINFO;


class CUIListbox : public CUICtrl, public CUICtrlCallback
{
public:
    CUIListbox(CUICtrlCallback * pParent);
    virtual ~CUIListbox();

    virtual CUICtrl *   CaptureMouse(CUICtrl * pctrl){return m_pParent->CaptureMouse(pctrl);}
    virtual void        ReleaseMouse(CUICtrl * pctrl){m_pParent->ReleaseMouse(pctrl);}
    virtual void        UIButtonUp(CUIButton * pbutton);
    virtual void        CtrlInvalidateRect(RECT * prc) {m_pParent->CtrlInvalidateRect(prc);}
    virtual void        ReleaseCapture();

    virtual void    DoFrame(POINT ptMouse);
    virtual void    Draw(CRaster * pRaster, RECT * prcClip);

    virtual BOOL    LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags);
    virtual BOOL    LButtonUp(int x, int y, UINT keyFlags);

    virtual BOOL    TokenLoad(HANDLE hFile);

            BOOL    Initialize();

            int     GetNumItems() { return m_vInfo.size(); }

            int     AddItem(LPCSTR pszText, DWORD dwParam, int iIndex, WORD wFlags);
            BOOL    RemoveItem(int iIndex);
            BOOL    RemoveAllItems();

            int     FindItem(LPCSTR pszText);

            int     GetItem(LPSTR psz, int icText, DWORD & dwParam, WORD & wFlags, int iIndex);
            int     SetItem(LPSTR psz, DWORD dwParam, WORD wFlags, int iIndex);

            int     GetCurrSel() { return m_iCurrSel; }
            int     SetCurrSel(int iIndex);

    virtual BOOL    SetFont(HFONT hfont);
    virtual HFONT   GetFont() { return m_hfont; }

    virtual COLORREF GetBkColor() { return m_crBkColor; }
    virtual void     SetBkColor(COLORREF cr) { m_crBkColor = cr; m_bUpdate = TRUE;}

    virtual COLORREF GetFGColor() { return m_crFGColor; }
    virtual void     SetFGColor(COLORREF cr) { m_crFGColor = cr; m_bUpdate = TRUE;}

    virtual BOOL    GetBorder() { return m_bBorder;}
    virtual void    SetBorder(BOOL bBorder) { m_bBorder = bBorder; m_bUpdate = TRUE;}

    virtual BOOL    GetTransparentBk() {return m_bTransBk;}
    virtual void    SetTransparentBk(BOOL bTransBk) { m_bTransBk = bTransBk; m_bUpdate = TRUE;}

    virtual COLORREF GetTransparentColor() { return m_crTrans; }
    virtual void     SetTransparentColor(COLORREF cr) { m_crTrans = cr; m_bUpdate = TRUE; }

    virtual BOOL    GetBackLit() {return m_bBackLit;}
    virtual void    SetBackLit(BOOL bBackLit) { m_bBackLit = bBackLit; m_bUpdate = TRUE; }

    virtual COLORREF GetBackLitColor() { return m_crBackLit; }
    virtual void     SetBackLitColor(COLORREF cr) { m_crBackLit = cr; m_bUpdate = TRUE;}

    virtual BYTE    GetBackLitOffset() { return m_bBackLitOffset;}
    virtual void    SetBackLitOffset(BYTE bOffset) { m_bBackLitOffset = bOffset; m_bUpdate = TRUE; }

    virtual BOOL    GetShowScrollAlways() { return m_bShowScrollAlways; }
    virtual void    SetShowScrollAlways(BOOL bShow) { m_bShowScrollAlways = bShow; m_bUpdate = TRUE; }

    virtual BOOL    GetAllowSelection() { return m_bAllowSelection; }
    virtual void    SetAllowSelection(BOOL bAllowSel) { m_bAllowSelection = bAllowSel; m_bUpdate = TRUE;}

    virtual int     GetFontWeight() { return m_iFontWeight;}
    virtual void    SetFontWeight(int iFontWeight) { m_iFontWeight = iFontWeight; m_bUpdate = TRUE;}

    virtual BOOL    GetFontSize() { return m_bFontSize;}
    virtual void    SetFontSize(BOOL bFontSize) { m_bFontSize = bFontSize; m_bUpdate = TRUE;}
    
private:
    vector<CUILISTBOXINFO>     m_vInfo;

    CUIButton  *    m_pScroll[2];
    BOOL            m_bCaptured : 1;
    BOOL            m_bAllowSelection : 1;
    BOOL            m_bScrollShown : 1;
    BOOL            m_bShowScrollAlways : 1;
    int             m_iCurrSel;
    int             m_iTop;
    int             m_iItemsMaxVis;
    int             m_iScrollWidth;
    int             m_iRowHeight;
    int             m_iMouseFromTop;
    WORD            m_wUp;
    WORD            m_wDown;
    WORD            m_wFlat;

    CRasterDC *     m_pRasterDC;
    BOOL            m_bUpdate;
    COLORREF        m_crBkColor;
    COLORREF        m_crFGColor;
    BYTE            m_bFontSize;
    int             m_iFontWeight;
    HFONT           m_hfont;

    COLORREF        m_crBackLit;
    BYTE            m_bBackLitOffset;
    BOOL            m_bBackLit;
    BOOL            m_bBorder;
    COLORREF        m_crTrans;
    BOOL            m_bTransBk;
    WORD            m_wTransColor;

    void Update();
    BOOL InitSurface();
};


typedef enum
{
    CUICHECKBOX_UN,
    CUICHECKBOX_UN_OVER,
    CUICHECKBOX_CHK,
    CUICHECKBOX_CHK_OVER,
    CUICHECKBOX_INACTIVE,
    CUICHECKBOX_INACTIVE_CHK
};


class CUICheckbox : public CUICtrl
{
public:
    CUICheckbox(CUICtrlCallback * pParent);
    virtual ~CUICheckbox();

    virtual void    DoFrame(POINT ptMouse);
    virtual void    Draw(CRaster * pRaster, RECT * prcClip);

    virtual BOOL    LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags);
    virtual BOOL    LButtonUp(int x, int y, UINT keyFlags);

    virtual BOOL    TokenLoad(HANDLE hFile);

    virtual BOOL    GetDown() { return m_bDown; }
    virtual void    SetDown(BOOL bDown) { if (bDown != m_bDown) { m_bDown = bDown; InvalidateSelf(); } }

    virtual void    ReleaseCapture();

private:
    UINT            m_uiCmd;
    BYTE            m_bLastState;
    BOOL            m_bOver : 1;
    BOOL            m_bDown : 1;
    BOOL            m_bCaptured : 1;
    CRasterDC *     m_aprasCheck[6];
};


class CUITextbox : public CUICtrl
{
public:
    CUITextbox(CUICtrlCallback * pParent);
    virtual ~CUITextbox();

    virtual void    Draw(CRaster * pRaster, RECT * prcClip);
    virtual void    Draw(LPBYTE pbDst,
                         int iDstWidth,
                         int iDstHeight,
                         int iDstPitch,
                         int iDstBytes,
                         RECT * prcClip);

    virtual void    SetPalette(HPALETTE hpal);
    virtual void    SetPalette(LOGPALETTE * plogpal);

    virtual LPSTR   GetText() { return m_pszText;}
    virtual BOOL    SetText(LPSTR psz);

    virtual COLORREF GetBkColor() { return m_crBkColor; }
    virtual void     SetBkColor(COLORREF cr) { m_crBkColor = cr; m_pParent->CtrlInvalidateRect(&m_rc); m_bUpdate = TRUE;}

    virtual COLORREF GetFGColor() { return m_crFGColor; }
    virtual void     SetFGColor(COLORREF cr) { m_crFGColor = cr; m_pParent->CtrlInvalidateRect(&m_rc); m_bUpdate = TRUE;}

    virtual DWORD    GetDTFormat() { return m_dwDTFormat; }
    virtual void     SetDTFormat(DWORD dw) { m_dwDTFormat = dw; m_pParent->CtrlInvalidateRect(&m_rc);m_bUpdate = TRUE;}

    virtual BOOL    GetBorder() { return m_bBorder;}
    virtual void    SetBorder(BOOL bBorder) { m_bBorder = bBorder; m_pParent->CtrlInvalidateRect(&m_rc); m_bUpdate = TRUE;}

    virtual BOOL    GetTransparentBk() {return m_bTransBk;}
    virtual void    SetTransparentBk(BOOL bTransBk) { m_bTransBk = bTransBk; m_pParent->CtrlInvalidateRect(&m_rc); m_bUpdate = TRUE;}

    virtual BOOL    GetBackLit() {return m_bBackLit;}
    virtual void    SetBackLit(BOOL bBackLit) { m_bBackLit = bBackLit; m_pParent->CtrlInvalidateRect(&m_rc); m_bUpdate = TRUE; }

    virtual COLORREF GetBackLitColor() { return m_crBackLit; }
    virtual void     SetBackLitColor(COLORREF cr) { m_crBackLit = cr; m_pParent->CtrlInvalidateRect(&m_rc); m_bUpdate = TRUE;}

    virtual BYTE    GetBackLitOffset() { return m_bBackLitOffset;}
    virtual void    SetBackLitOffset(BYTE bOffset) { m_bBackLitOffset = bOffset; m_pParent->CtrlInvalidateRect(&m_rc); m_bUpdate = TRUE; }

    virtual BOOL    TokenLoad(HANDLE hFile);

    virtual BOOL InitSurface();
private:
    CRasterDC *     m_pras;
    BOOL            m_bUpdate;
    LPSTR           m_pszText;
    COLORREF        m_crBkColor;
    COLORREF        m_crFGColor;
    COLORREF        m_crBackLit;
    BYTE            m_bBackLitOffset;
    DWORD           m_dwDTFormat;
    BOOL            m_bBackLit;
    BOOL            m_bBorder;
    BOOL            m_bTransBk;
    WORD            m_wTransColor;
    BYTE            m_bFontSize;
    int             m_iFontWeight;
    HFONT           m_hfont;

    void Update();
};


class CUISlider : public CUICtrl
{
public:
    CUISlider(CUICtrlCallback * pParent);
    virtual ~CUISlider();

    virtual void    DoFrame(POINT ptMouse);
    virtual void    Draw(CRaster * pRaster, RECT * prcClip);

    virtual BOOL    GetBorder() { return m_bBorder;}
    virtual void    SetBorder(BOOL bBorder) { m_bBorder = bBorder;}

    virtual int     GetUnits() { return m_iUnits; }
    virtual void    SetUnits(int iUnits) { m_iUnits = iUnits; }

    virtual int     GetCurrUnit() { return m_iCurrUnit; }
    virtual void    SetCurrUnit(int iUnit);

    virtual void    MouseMove(int x, int y, UINT keyFlags);
    virtual BOOL    LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags);
    virtual BOOL    LButtonUp(int x, int y, UINT keyFlags);

    virtual BOOL    TokenLoad(HANDLE hFile);

    virtual void    ReleaseCapture();

private:
    UINT            m_uiCmd;
    BOOL            m_bOver : 1;
    BOOL            m_bDown : 1;
    BOOL            m_bCaptured : 1;
    int             m_x;
    int             m_iUnits;
    int             m_iCurrUnit;
    BOOL            m_bBorder;
    WORD            m_wUpClr;
    WORD            m_wDownClr;
    WORD            m_wFlatClr;
    CRaster *       m_pRaster;
};


class CUIEditbox : public CUICtrl
{
public:
    CUIEditbox(CUICtrlCallback * pParent);
    virtual ~CUIEditbox();

    virtual void    DoFrame(POINT ptMouse);
    virtual void    Draw(CRaster * pRaster, RECT * prcClip);

    virtual BOOL    LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags);
    virtual BOOL    LButtonUp(int x, int y, UINT keyFlags);

    virtual void    OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags);
    virtual void    OnChar(TCHAR tch, int cRepeat);

    virtual void    SetPalette(HPALETTE hpal);
    virtual void    SetPalette(LOGPALETTE * plogpal);

    virtual int     SetMaxCharNum(int icMax);
    virtual int     GetMaxCharNum();

    virtual LPSTR   GetText();
    virtual BOOL    SetText(LPSTR psz);

    virtual COLORREF GetBkColor() { return m_crBkColor; }
    virtual void     SetBkColor(COLORREF cr) { m_crBkColor = cr; m_bUpdate = TRUE;}

    virtual COLORREF GetFGColor() { return m_crFGColor; }
    virtual void     SetFGColor(COLORREF cr) { m_crFGColor = cr; m_bUpdate = TRUE;}

    virtual DWORD    GetDTFormat() { return m_dwDTFormat; }
    virtual void     SetDTFormat(DWORD dw) { m_dwDTFormat = dw; m_bUpdate = TRUE;}

    virtual BOOL    GetBorder() { return m_bBorder;}
    virtual void    SetBorder(BOOL bBorder) { m_bBorder = bBorder; m_bUpdate = TRUE;}

    virtual BOOL    TokenLoad(HANDLE hFile);

protected:
    int             m_iMaxSize;
    vector<char>    m_vInfo;
    CRasterDC *     m_pras;
    BOOL            m_bUpdate;
    COLORREF        m_crBkColor;
    COLORREF        m_crFGColor;
    DWORD           m_dwDTFormat;
    BOOL            m_bBorder;
    BYTE            m_bFontSize;
    int             m_iFontWeight;
    HFONT           m_hfont;
    int             m_iLeft;
    COLORREF        m_crBackLit;
    BYTE            m_bBackLitOffset;
    BOOL            m_bBackLit;
    BOOL            m_bTransBk;
    WORD            m_wTransColor;

    void Update();
    BOOL InitSurface();
};


class CUIProgressBar : public CUICtrl
{
public:
    CUIProgressBar(CUICtrlCallback * pParent);
    virtual ~CUIProgressBar();

    BOOL  TokenLoad(HANDLE hFile);
    void  Draw(CRaster * pRaster, RECT * prcClip);
    void  Draw(LPBYTE pbDst,
               int iDstWidth,
               int iDstHeight,
               int iDstPitch,
               int iDstBytes,
               RECT * prcClip);

    int   DeltaPos(int iIncrement);
    int   SetPos(int iNewPos);
    BOOL  SetRange(int iMinRange, int iMaxRange,
                   int * piOldMin, int * piOldMax);
    int   SetStep(int iStep);
    int   StepIt();


protected:
    int         m_iMin;
    int         m_iMax;
    int         m_iCurr;
    int         m_iStep;
    WORD        m_wClrFill;
    bool        m_bFrame;
};


class CUIHotspot : public CUICtrl
{
public:
    CUIHotspot(CUICtrlCallback * pParent);
    virtual ~CUIHotspot();

    BOOL  TokenLoad(HANDLE hFile);
    void  Draw(CRaster * pRaster, RECT * prcClip);

    void    MouseMove(int x, int y, UINT keyFlags);
    BOOL    LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags);
    BOOL    LButtonUp(int x, int y, UINT keyFlags);

protected:
    bool        m_bFrame;
};


#if 0
class CUIXButton : public CUIButton
{
public:
    CUIXButton(CUICtrlCallback * pParent);
    virtual ~CUIXButton();

    virtual void    Draw(CRaster * pRaster, RECT * prcClip);
    virtual BOOL    TokenLoad(CDWIStream * pstm);

protected:
    virtual CRaster * GetRaster();
};
#endif


#endif // __CTRLS_H__


