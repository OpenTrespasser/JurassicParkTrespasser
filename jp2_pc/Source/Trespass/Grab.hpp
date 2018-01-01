

#ifndef __SnapDIB_H_
#define __SnapDIB_H_


#define PALVERSION	0x300
#define MAXPALETTE	256


class SnapDIB;


SnapDIB *  DibFromPICT(LPCSTR pszFileName);


class SnapDIB
{
public:
    SnapDIB();
    ~SnapDIB();

    BOOL        Create(int cx, int cy, WORD wBitCount, DWORD dwColorsUsed);

    HANDLE      LoadFromResource(HINSTANCE hinstance, LPCSTR res);
    BOOL        LoadFromFile(LPCSTR pszFile);

    DWORD       GetSize();
    LONG        GetWidth();
    LONG        GetHeight();
    int         GetOrientation();
    WORD        GetPlanes();
    WORD        GetBitCount();
    DWORD       GetCompression();
    DWORD       GetSizeImage();
    DWORD       GetColorsUsed();
    DWORD       GetColorsImportant();

    HPALETTE    CreatePalette();
    WORD        GetPaletteSize(LPBITMAPINFOHEADER lpbi);

    HBITMAP     GetBitmap(HPALETTE * pPal);
    WORD        NumColors();
    WORD        NumColors(BYTE * pb);
    RGBQUAD *   GetColor(int i);

    BYTE *      GetPixelPointer(int x, int y);
    BOOL        GetPixelRGB(int x, 
                            int y, 
                            BYTE * pbRed, 
                            BYTE * pbGreen, 
                            BYTE * pbBlue);
    BOOL        SetPixelRGB(int x, int y, BYTE bRed, BYTE bGreen, BYTE bBlue);
    BOOL        SetPixel(int x, int y, BYTE bColorIndex);
    BOOL        SetColorRect(int x, int y, int iWidth, int iHeight, COLORREF cr);
    BOOL        CopyDibInto(int x, int y, int iWidth, int iHeight, SnapDIB * pdib);
	BOOL        CopyDibFrom(int x, int y, int iWidth, int iHeight, SnapDIB * pdib);

    BOOL        Write(LPSTR psz);
    BOOL        Write(HANDLE hfile);

    LPBITMAPINFOHEADER GetInfoHeader() { return m_lpbi; }
    LPBYTE      GetBits();

    BOOL        Uncompress();


    //
    // Creation Routines
    //
    static int     DibFromFile(LPCSTR pszFile, SnapDIB ** ppdib);
    static SnapDIB *  DibFromFile(LPCSTR pszFile);
    static SnapDIB *  DibFromBitmap(HBITMAP hbmp, 
                                 DWORD biStyle, 
                                 WORD biBitsCount, 
                                 HPALETTE hpal, 
								 WORD wUsage);
    void GrabGameScreen();

private:
	// Remember the order of m_lpbi and m_pBits is important simulates BITMAPINFO
    LPBITMAPINFOHEADER  m_lpbi;
    LPBYTE              m_pBits;
    WORD                m_wWidthBytes;


    RGBQUAD * GetPalettePtr();

    LPBITMAPINFOHEADER ReadDibBitmapInfo(HFILE hfile);

};


#endif

