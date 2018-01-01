//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1997.
//
//  File:       supportfn.hpp
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    13-Nov-97   SHernd   Created
//
//---------------------------------------------------------------------------


#ifndef __SUPPORTFN_HPP__
#define __SUPPORTFN_HPP__

class CRaster;
class CCamera;
class CUIWnd;
class CDib;

#define RGBFROM555(w555, r, g, b) \
            (r) = (((w555) & 0x7C00) >> 10) * 8;\
            (g) = (((w555) & 0x03E0) >> 5) * 8;\
            (b) = ((w555) & 0x001F) * 8; 


#define RGBFROM565(w565, r, g, b) \
            (r) = (((w565) & 0xF800) >> 11) * 8;\
            (g) = (((w565) & 0x07E0) >> 5) * 4;\
            (b) = ((w565) & 0x001F) * 8; 

#define RGBFROM24(pb, r, g, b) \
            (b) = *(pb)++; \
            (g) = *(pb)++; \
            (r) = *(pb)++;

#define MAKE555(r, g, b) \
    ((WORD)  (((DWORD) ((r) / 8) << 10) | \
              ((WORD)  ((g) / 8) << 5)  | \
              ((BYTE)  ((b) / 8))))

#define MAKE565(r, g, b) \
    ((WORD)  (((DWORD) ((r) / 8) << 11) | \
              ((WORD)  ((g) / 4) << 5)  | \
              ((BYTE)  ((b) / 8))))

#if BUILDVER_MODE != MODE_FINAL
#define TraceError(X) dprintf X ; dprintf("\r\n")
#define Trace(X) dprintf X ; dprintf("\r\n")
#else
#define TraceError(X)
#define Trace(X)
#endif

#define VIEWPORT_INCREMENTS         10
#define VIEWPORT_INCREMENTS_FLOAT   10.0f

void ForceShowCursor(BOOL bShow);

typedef enum
{
    FA_DATADRIVE,
    FA_INSTALLDIR,
} FA_TYPE;

typedef struct tagMEMDIBINFO
{
    LPBYTE  pbBase;
    LPBYTE  pbCurr;
    DWORD   dwSize;
} MEMDIBINFO;

BOOL PASCAL MemDibRead(VOID * pv, VOID * pbDst, DWORD dwSize, DWORD * pdwRead);
BOOL PASCAL MemDibWrite(VOID * pv, VOID * pbSrc, DWORD dwSize, DWORD * pdwWritten);
BOOL PASCAL MemDibSeek(VOID * pv, DWORD dwMove, DWORD seek, DWORD * pNewOffset);

int GetFileLoc(FA_TYPE faFileLoc, char * psz, int icSize);
HANDLE FileAbstraction(LPCSTR psz, FA_TYPE faFileLoc = FA_DATADRIVE);

CRaster * ReadAndConvertBMP(LPSTR pszFile, bool bRasterDC = false, FA_TYPE faFileLoc = FA_INSTALLDIR);
CRaster * ConvertBMP(CDib * pdib, bool bRasterDC = false);
void CenterUIWindow(CUIWnd * puiwnd);
WORD GetColor(BYTE bRed, BYTE bGreen, BYTE bBlue);
void SeperateColor(WORD wColor, BYTE & bRed, BYTE & bGreen, BYTE & bBlue);
void MyFillRect(LPRECT prc,
                WORD wColor,
                LPBYTE pbDst, 
                int iWidth,
                int iHeight,
                int iPitch);
void MyFillRect(RECT * prc, WORD wColor, CRaster * pRaster);
void MyFrameRect(RECT * prc, WORD wColor, LPBYTE pbDst, int iWidth, int iHeight, int iPitch);
void MyFrameRect(RECT * prc, WORD wFill, CRaster * pRaster);
void MySunkenRect(RECT * prc, 
                  WORD wUp, 
                  WORD wDown, 
                  CRaster * pRaster, 
                  RECT * prcClip = NULL);
WORD ChangeIllum(WORD w, double dIllum, bool b555);
void MyBlt(LPBYTE pbSrc,
           int iSrcPitch,
           int iSrcBytes,
           int xSrc,
           int ySrc,
           int iWidth,
           int iHeight,
           LPBYTE pbDst,
           int iDstPitch,
           int iDstBytes,
           int xDst,
           int yDst,
           bool bColorKey,
           int iColorKey,
           int iIllum = 0);
void RasterBlt(CRaster * pRSrc,
               RECT * prcSrc,
               CRaster * pRDst,
               int xDst,
               int yDst,
               bool bColorKey,
               int iColorKey,
               int iIllum = 0);

// Returns the pointer to the Primary Camera
CCamera* pcamGetCamera();
void SetupGameScreen();
void ChangeViewportSize(int iX, int iY, bool bRedraw = true);
void RefreshAudioSettings();
void ScreenCapture();
void MiddleMessage(UINT uiIDS);
void DisplayLoadingFileError(int iErr);
void ClearInputState(bool bCenterMouse = false);


#endif // __SUPPORTFN_H__

