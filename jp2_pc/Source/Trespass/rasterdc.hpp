//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1997.
//
//  File:       rasterdc.hpp
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


#ifndef __RASTERDC_HPP__
#define __RASTERDC_HPP__


class CRasterDC : public CRaster
{
public:
    CRasterDC(HWND hwnd, int iWidth, int iHeight, int iBits);
    CRasterDC(HDC hdcBase, int iWidth, int iHeight, int iBits);
    virtual ~CRasterDC();

    void Lock() override;
    void Unlock() override;
    void Clear(TPixel pix) override;

    void Blit(int idx, 
              int idy, 
              CRaster & rasSrc, 
              SRect* prect_src = 0,
              bool b_clip = true, 
              bool b_colour_key = 0,
              TPixel pix_colour_key = 0) override;

    HDC hdcGet();
    void ReleaseDC(HDC hdc);

    HWND        m_hwnd;
    BOOL        m_fDCSet;
    HDC         m_hdc;
    HDC         m_hDCScreen;
    HBITMAP     m_hBmp;
    HBITMAP     m_hBmpOld;
    HPALETTE    m_hPal;
    int         m_x;
    int         m_y;
    int         m_nLockCount;

protected:
    void  Init(HDC hdcBase, int iWidth, int iHeight, int iBits);
};


#endif // __RASTERDC_HPP__

