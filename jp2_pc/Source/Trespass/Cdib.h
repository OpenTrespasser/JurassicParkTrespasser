

#ifndef __CDIB_H_
#define __CDIB_H_


#define PALVERSION	0x300
#define MAXPALETTE	256


class CDib;

#if 0
CDib *  DibFromPICT(LPCSTR pszFileName);
BOOL    ReadLogPalette(LPCSTR pszFileName, LOGPALETTE ** ppLogPal);
CDib *  ReduceColor(CDib *pSrc, 
					HPALETTE hPal, 
					RGBQUAD *pRGBSource, 
					BOOL fDither,
					int      nBitDepth,
					int		 nWidth,
					int		 nHeight);
#endif 
CDib * ResizeImage (CDib *pSrc, int nWidth, int nHeight);


typedef BOOL (FAR PASCAL * PFNDIBREAD)( VOID * pv, VOID * pbDst, DWORD dwSize, DWORD * pdwRead );
typedef BOOL (FAR PASCAL * PFNDIBWRITE)( VOID * pv, VOID * pbSrc, DWORD dwSize, DWORD * pdwWritten );
typedef BOOL (FAR PASCAL * PFNDIBSEEK)( VOID * pv, DWORD dwMove, DWORD seek, DWORD * pNewOffset );



class CDib
{
public:
    CDib();
    ~CDib();

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
    WORD        GetWidthBytes();

    HPALETTE    CreatePalette();
    LOGPALETTE * CreateLogPalette();
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
    BOOL        CopyDibInto(int x, int y, int iWidth, int iHeight, CDib * pdib);
	BOOL        CopyDibFrom(int x, int y, int iWidth, int iHeight, CDib * pdib);


    DWORD       CalculateWriteSize();

    BOOL        Write(LPSTR psz);
    BOOL        Write(HANDLE hfile);
    BOOL        WriteToStream(VOID * pv, PFNDIBWRITE pfnDibWrite);

    BOOL        LoadFromStream(VOID * pv, PFNDIBREAD pfnDibRead, PFNDIBSEEK pfnDibSeek);

    LPBITMAPINFOHEADER GetInfoHeader() { return m_lpbi; }
    LPBYTE      GetBits();

    BOOL        Uncompress();

#if 0
    BOOL        IndexToLogPalette(LOGPALETTE * pLogPal);
    BOOL        ChangeBitDepth(int iBitDepth, LOGPALETTE * pLogPal);
#endif


    //
    // Creation Routines
    //
    static int     DibFromFile(LPCSTR pszFile, CDib ** ppdib);
    static CDib *  DibFromFile(LPCSTR pszFile);
    static CDib *  DibFromResource(HINSTANCE hInst, LPCSTR pszName);
    static CDib *  DibFromTarga(LPCSTR pszFile);
    static CDib *  DibFromBitmap(HBITMAP hbmp, 
                                 DWORD biStyle, 
                                 WORD biBitsCount, 
                                 HPALETTE hpal, 
                                 WORD wUsage);

private:
    LPBITMAPINFOHEADER  m_lpbi;
    LPBYTE              m_pBits;
    WORD                m_wWidthBytes;


    RGBQUAD * GetPalettePtr();

    LPBITMAPINFOHEADER ReadDibBitmapInfo(HANDLE hfile);
    LPBITMAPINFOHEADER ReadDibBitmapInfo(VOID * pv, PFNDIBREAD pfnDibRead, PFNDIBSEEK pfnDibSeek);

};


#endif

