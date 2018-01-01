//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1996.
//
//  File:       SnapDIB.cpp
//
//  Contents:   Dib Reading/Writing/Manipulation Code.  This also handles
//              Conversions from other formats into DIB format
//
//  Classes:    SnapDIB
//
//  Functions:  DetermineFileType
//
//  History:    2-Jan-98            FRoss   Created
//
//---------------------------------------------------------------------------


#include "precomp.h"
#pragma hdrstop

#include "supportfn.hpp"
#include "gdidlgs.h"
#include "grab.hpp"


#define BFT_BITMAP 0x4d42
/* macro to determine if resource is a DIB */
#define ISDIB(bft) ((bft) == BFT_BITMAP)
/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)   ((i+31)/32*4)
//#define WIDTHBYTES(i)   ((unsigned)((i+31)&(~31))/8)
/* flags for _lseek */
#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0




typedef enum tagIMAGETYPE
{
    IT_NONE,
    IT_BITMAP,
    IT_PICT
} IMAGETYPE;



//+--------------------------------------------------------------------------
//
//  Function:   DetermineFileType
//
//  Synopsis:   Takes an open file handle and tries to determine what kind
//              of image file it is.
//
//  Arguments:  [hfile]      -- open image file handle
//              [pimagetype] -- return image file type
//
//  Returns:    int --  1  -- if successful
//                     -2  -- if unable to read from file
//                     -3  -- if unable to determine file type
//
//  History:    13-Jun-96   SHernd   Created
//
//---------------------------------------------------------------------------
int DetermineFileType(HANDLE hfile, IMAGETYPE * pimagetype)
{
    BYTE        ab[512];
    BYTE        abPICT[512];
    DWORD       dwRead;
    BOOL        bRet;
    int         iRet;

    *pimagetype = IT_NONE;
    memset(abPICT, 0, sizeof(abPICT));

    bRet = ReadFile(hfile, &ab, 512, &dwRead, NULL);
    if (!bRet || (dwRead != 512))
    {
        TraceError(("DetermineFileType() -- "
                    "Unable to read 512 bytes from file"));
        iRet = -2;
        goto Error;
    }

    // Check for BITMAP
    if (ab[0] == 'B' && ab[1] == 'M')
    {
        *pimagetype = IT_BITMAP;
    }
    // PICT file has the first 512 bytes of 0
    else if (memcmp(ab, abPICT, 512) == 0)
    {
        *pimagetype = IT_PICT;
    }

    iRet = 1;

Cleanup:
    SetFilePointer(hfile, 0, NULL, FILE_BEGIN);
    return iRet;

Error:
    TraceError(("DetermineFileType"));
    goto Cleanup;
}



//+--------------------------------------------------------------------------
//----------------           SnapDIB Member Functions           ----------------
//---------------------------------------------------------------------------



SnapDIB::SnapDIB()
{
    m_lpbi = NULL;
}



SnapDIB::~SnapDIB()
{
    delete [] m_lpbi;
}



BOOL SnapDIB::Create(int cx, int cy, WORD wBitCount, DWORD dwColorsUsed)
{
    LPBITMAPINFOHEADER  pdib;
    DWORD               dwSize;
    DWORD               dwColors;
    BITMAPINFOHEADER    bi;
    BOOL                bRet;

    // if we have an image that is 16- or 24-bit we won't us the
    // incredibly huge number for a palette
    dwColors = (wBitCount < 16) ? dwColorsUsed : 0;
    switch (wBitCount)
    {
        case 8:
            dwColors = 256;
            break;
        case 4:
            dwColors = 64;
            break;
        case 2:
            dwColors = 2;
    }

    m_wWidthBytes = ((cx * (wBitCount / 8)) + 3) & ~3;

    //
    // Assign the values into the Info Header
    //
    bi.biSize           = sizeof(BITMAPINFOHEADER);
    bi.biWidth          = cx;
    bi.biHeight         = cy;
    bi.biPlanes         = 1;
    bi.biBitCount       = wBitCount;
    bi.biCompression    = (wBitCount < 16) ? 0 : BI_RGB;
    bi.biSizeImage      = m_wWidthBytes * abs(cy);
    bi.biXPelsPerMeter  = 0;
    bi.biYPelsPerMeter  = 0;
    bi.biClrUsed        = dwColorsUsed;
    bi.biClrImportant   = dwColorsUsed;

    dwSize = bi.biSize + (dwColors * sizeof(RGBQUAD)) + bi.biSizeImage;

    pdib = (LPBITMAPINFOHEADER)new BYTE[dwSize];
    if (pdib == NULL)
    {
        Trace(("ERROR:  SnapDIB::Create() -- OUT OF MEMORY -- "
               "Unable to create dib"));
        goto Error;
    }
    memset (pdib, 0, dwSize);
    *(pdib) = bi;

    delete [] m_lpbi;
    m_lpbi = pdib;

    m_pBits = (LPBYTE)m_lpbi + m_lpbi->biSize + (dwColors * sizeof(RGBQUAD));

    // for a 8 bit image create a grey-scale palette
    if (wBitCount == 8)
    {
        RGBQUAD *   prgb = GetColor(0);
        int         i;

        for (i = 0; i < 256; i++)
        {
            prgb[i].rgbRed = i;
            prgb[i].rgbGreen = i;
            prgb[i].rgbBlue = i;
        }
    }

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    Trace(("ERROR:  SnapDIB::Create"));
    if (pdib != NULL)
    {
        delete pdib;
    }
    bRet = FALSE;
    goto Cleanup;
}


//+--------------------------------------------------------------------------
//
//  Member:     SnapDIB::DibFromFile
//
//  Synopsis:   Opens the file.  Determines what kind of image file that it
//              is.  Then reads the image file into a dib.
//
//  Arguments:  [pszFile] -- name of file to load
//              [ppdib]   -- destination pointer for loaded dib
//
//  Returns:    int --   1 -- if successful
//                      -1 -- if file was not able to be opened
//                      -2 -- if error reading from file
//                      -3 -- if unable to determine file type
//                      -5 -- if Error reading image
//
//  History:    13-Jun-96   SHernd   Created
//
//---------------------------------------------------------------------------
/*int SnapDIB::DibFromFile(LPCSTR pszFile, SnapDIB ** ppdib)
{
    int         iRet;
    HANDLE      hfile = INVALID_HANDLE_VALUE;
    SnapDIB *      pdib = NULL;
    IMAGETYPE   imagetype;

    hfile = CreateFile(pszFile,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        TraceError(("SnapDIB::DibFromFile() -- "
                    "Unable to OpenFile"));
        iRet = -1;
        goto Error;
    }

    // Try and determine file type
    iRet = DetermineFileType(hfile, &imagetype);
    if (iRet != 1)
    {
        TraceError(("SnapDIB::DibFromFile() -- "
                    "DetermineFileType Failed"));
        goto Error;
    }

    CloseHandle(hfile);
    hfile = INVALID_HANDLE_VALUE;

    switch (imagetype)
    {
        case IT_BITMAP:
            pdib = SnapDIB::DibFromFile(pszFile);
            break;

        case IT_PICT:
            pdib = DibFromPICT(pszFile);
            break;
    }

    if (pdib == NULL)
    {
        TraceError(("SnapDIB::DibFromFile() -- "
                    "Error loading image %i",
                    imagetype));
        iRet = -5;
    }

    iRet = 1;

Cleanup:
    if (hfile !=INVALID_HANDLE_VALUE)
    {
        CloseHandle(hfile);
    }

    *ppdib = pdib;

    return iRet;

Error:
    TraceError(("SnapDIB::DibFromFile"));
    delete pdib;
    pdib = NULL;
    goto Cleanup;
}

*/

SnapDIB * SnapDIB::DibFromFile(LPCSTR pszFile)
{
    SnapDIB * pdib;

    pdib = new SnapDIB();
    if (pdib == NULL)
    {
        Trace(("ERROR:  SnapDIB::DibFromFile() -- "
               "OOM -- Couldn't create SnapDIB"));
        goto Error;
    }

    if (!pdib->LoadFromFile(pszFile))
    {
        Trace(("ERROR:  SnapDIB::DibFromFile() -- Couldn't LoadFromFIle"));
        goto Error;
    }

Cleanup:
    return pdib;

Error:
    Trace(("ERROR:  SnapDIB::DibFromFile"));
    if (pdib != NULL)
    {
        delete pdib;
        pdib = NULL;
    }

    goto Cleanup;
}



BOOL SnapDIB::LoadFromFile(LPCSTR pszFile)
{
    HFILE               hfile;
    LPBITMAPINFOHEADER  lpbi = NULL;
    DWORD               dwLen = 0;
    DWORD               dwBits;
    OFSTRUCT            of;
    LPBITMAPINFOHEADER  pbtemp = NULL;
    BOOL                bRet;
    BYTE *              pb;

    //
    // Open the file and read the DIB information
    //
    hfile = OpenFile(pszFile, &of, OF_READ);
    if (hfile == HFILE_ERROR)
    {
        Trace(("ERROR:  SnapDIB::LoadFromFile() - Couldn't open file"));
        goto Error;
    }

    lpbi = ReadDibBitmapInfo(hfile);
    if (lpbi == NULL)
    {
        Trace(("Error:  SnapDIB::LoadFromFile() - "
               "Read dib bitmap info failed"));
        goto Error;
    }

    //
    // Calculate the memory needed to hold the DIB
    //
    dwBits = lpbi->biSizeImage;
    dwLen  = lpbi->biSize + (DWORD)GetPaletteSize(lpbi) + dwBits;

    //
    // Try to increase the size of the bitmap info. buffer to hold the DIB
    //
    pbtemp = (LPBITMAPINFOHEADER) new BYTE[dwLen];

    if (pbtemp == NULL)
    {
        Trace(("ERROR:  SnapDIB::LoadFromFile() - OOM - "
               "Couldn't allocate enough memory for size of bitmap"));
        goto Error;
    }
    else
    {
        memcpy(pbtemp,
               lpbi,
               lpbi->biSize + (DWORD)GetPaletteSize(lpbi));
        delete [] lpbi;
        lpbi = (LPBITMAPINFOHEADER)pbtemp;
    }

    //
    // Read in the bits
    //
    pb = (BYTE *)lpbi + (WORD)lpbi->biSize + GetPaletteSize(lpbi);
    _lread(hfile, pb, dwBits);
    m_pBits = pb;
    m_wWidthBytes = ((lpbi->biWidth * (lpbi->biBitCount / 8)) + 3) & ~3;

    bRet = TRUE;

Cleanup:
    if (hfile != HFILE_ERROR)
    {
        _lclose(hfile);
    }

    m_lpbi  = lpbi;


    return bRet;

Error:
    bRet = FALSE;
    if (lpbi != NULL)
    {
        delete [] lpbi;
        lpbi = NULL;
    }

    Trace(("ERROR:  SnapDIB::LoadFromFile"));
    goto Cleanup;
}

LPBITMAPINFOHEADER SnapDIB::ReadDibBitmapInfo(HFILE hfile)
{
    DWORD               off;
    HANDLE              hbi = NULL;
    INT                 size;
    INT                 i;
    WORD                nNumColors;

    RGBQUAD FAR       * pRgb;
    BITMAPINFOHEADER    bi;
    BITMAPCOREHEADER    bc;
    LPBITMAPINFOHEADER  lpbi;
    BITMAPFILEHEADER    bf;
    DWORD               dwWidth = 0;
    DWORD               dwHeight = 0;
    WORD                wPlanes, wBitCount;
    UINT                uResult;

    if (hfile == HFILE_ERROR)
    {
        return NULL;
    }

    //
    // Reset file pointer and read file header
    //
    off = _llseek(hfile, 0L, FILE_CURRENT);
    uResult = _lread(hfile, &bf, sizeof(BITMAPFILEHEADER));
    if (sizeof(BITMAPFILEHEADER) != uResult)
    {
        return NULL;
    }

    //
    // Do we have a RC HEADER?
    //
    if (!ISDIB(bf.bfType))
    {
        bf.bfOffBits = 0L;
        _llseek(hfile, off, FILE_BEGIN);
    }

    uResult = _lread(hfile, (LPSTR)&bi, (UINT)sizeof(bi));
    if (sizeof(bi) != uResult)
    {
        return NULL;
    }

    nNumColors = NumColors((BYTE *)&bi);

    //
    // Check the nature (BITMAPINFO or BITMAPCORE) of the info. block
    // and extract the field information accordingly. If a BITMAPCOREHEADER,
    // transfer it's field information to a BITMAPINFOHEADER-style block
    //
    switch (size = (INT)bi.biSize)
    {
        case sizeof(BITMAPINFOHEADER):
            break;

        case sizeof(BITMAPCOREHEADER):

            bc = *(BITMAPCOREHEADER*)&bi;

            dwWidth                 = (DWORD)bc.bcWidth;
            dwHeight                = (DWORD)bc.bcHeight;
            wPlanes                 = bc.bcPlanes;
            wBitCount               = bc.bcBitCount;

            bi.biSize               = sizeof(BITMAPINFOHEADER);
            bi.biWidth              = dwWidth;
            bi.biHeight             = dwHeight;
            bi.biPlanes             = wPlanes;
            bi.biBitCount           = wBitCount;

            bi.biCompression        = BI_RGB;
            bi.biSizeImage          = 0;
            bi.biXPelsPerMeter      = 0;
            bi.biYPelsPerMeter      = 0;
            bi.biClrUsed            = nNumColors;
            bi.biClrImportant       = nNumColors;

            _llseek(hfile,
                    sizeof(BITMAPCOREHEADER) - sizeof(BITMAPINFOHEADER),
                    SEEK_CUR);
            break;

        default:
            // Not a DIB!
            Trace(("ERROR:  SnapDIB::ReadDibBitmapInfo() -- "
                   "This is not a dib"));
            goto Error;
    }

    //
    //  Fill in some default values if they are zero
    //
    if (bi.biSizeImage == 0)
    {
        bi.biSizeImage = WIDTHBYTES((DWORD)bi.biWidth * bi.biBitCount)
                         * bi.biHeight;
    }

    if (bi.biClrUsed == 0)
    {
        bi.biClrUsed = NumColors((BYTE *)&bi);
    }

    //
    // Allocate for the BITMAPINFO structure and the color table
    //
    lpbi = (LPBITMAPINFOHEADER)new BYTE[bi.biSize +
                                        nNumColors * sizeof(RGBQUAD)];
    if (lpbi == NULL)
    {
        Trace(("Error:  SnapDIB::ReadDibBitmapInfo() -- OOM -- "
               "Couldn't allocate new bitmap location"));
        goto Error;
    }

    *lpbi = bi;

    //
    // Get a pointer to the color table
    //
    pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + bi.biSize);
    if (nNumColors)
    {
        if (size == sizeof(BITMAPCOREHEADER))
        {
            //
            // Convert a old color table (3 byte RGBTRIPLEs) to a new
            // color table (4 byte RGBQUADs)
            //
            _lread(hfile, (LPSTR)pRgb, (UINT)nNumColors * sizeof(RGBTRIPLE));

            for (i = nNumColors - 1; i >= 0; i--)
            {
                RGBQUAD rgb;

                rgb.rgbRed      = ((RGBTRIPLE FAR *)pRgb)[i].rgbtRed;
                rgb.rgbBlue     = ((RGBTRIPLE FAR *)pRgb)[i].rgbtBlue;
                rgb.rgbGreen    = ((RGBTRIPLE FAR *)pRgb)[i].rgbtGreen;
                rgb.rgbReserved = (BYTE)0;

                pRgb[i] = rgb;
            }
        }
        else
        {
            _lread(hfile, (LPSTR)pRgb, (UINT)nNumColors * sizeof(RGBQUAD));
        }
    }

    if (bf.bfOffBits != 0L)
    {
        _llseek(hfile, off + bf.bfOffBits, FILE_BEGIN);
    }

Cleanup:
    return lpbi;

Error:
    Trace(("ERROR:  SnapDIB::ReadDibBitmapInfo"));
    if (lpbi != NULL)
    {
        delete [] lpbi;
        lpbi = NULL;
    }

    goto Cleanup;
}



WORD SnapDIB::NumColors()
{
    return NumColors((BYTE *)m_lpbi);
}



WORD SnapDIB::NumColors(BYTE * pv)
{
    int                 bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;
    WORD                wRet;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    //
    //  With the BITMAPINFO format headers, the size of the palette
    //  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
    //  is dependent on the bits per pixel ( = 2 raised to the power of
    //  bits/pixel).
    //
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
    {
        if (lpbi->biClrUsed != 0)
        {
            return (WORD)lpbi->biClrUsed;
        }

        bits = lpbi->biBitCount;
    }
    else
    {
        bits = lpbc->bcBitCount;
    }

    switch (bits)
    {
        case 1:
            wRet = 2;
            break;
        case 4:
            wRet = 16;
            break;
        case 8:
            wRet = 256;
            break;
        default:
            // A 24 bitcount DIB has no color table
            wRet = 0;
            break;
    }

    return wRet;
}



RGBQUAD * SnapDIB::GetColor(int i)
{
    RGBQUAD * prgb;

    if ((NumColors() <= i) || (i < 0))
    {
        return NULL;
    }

    prgb = ((RGBQUAD *)((LPBYTE)(m_lpbi) + (int)(m_lpbi)->biSize));
    return &prgb[i];
}



WORD SnapDIB::GetPaletteSize(LPBITMAPINFOHEADER lpbi)
{
    WORD    wNumColors;
    WORD    wSize;

    wNumColors = NumColors((BYTE *)lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
    {
        wSize = wNumColors * sizeof(RGBTRIPLE);
    }
    else
    {
        wSize = wNumColors * sizeof(RGBQUAD);
    }

    return wSize;
}



HBITMAP SnapDIB::GetBitmap(HPALETTE * pPal)
{
    HPALETTE            hpalT;
    HDC                 hdc;
    HBITMAP             hbm;
    HPALETTE            hpal;

    if (pPal == NULL)
    {
        hpal = CreatePalette();
    }
    else
    {
        hpal = *pPal;
    }

    hdc = GetDC(NULL);

    if (hpal)
    {
        hpalT = SelectPalette(hdc,hpal,FALSE);
        RealizePalette(hdc);     // GDI Bug...????
    }

    hbm = CreateDIBitmap(hdc,
                m_lpbi,
                CBM_INIT,
                (BYTE *)m_lpbi + (WORD)m_lpbi->biSize + GetPaletteSize(m_lpbi),
                (LPBITMAPINFO)m_lpbi,
                DIB_RGB_COLORS );

    if (hpal)
        SelectPalette(hdc,hpalT,FALSE);

    ReleaseDC(NULL,hdc);

    return hbm;
}



DWORD SnapDIB::GetSize()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biSize;
    }

    return 0;
}



LONG SnapDIB::GetWidth()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biWidth;
    }

    return 0;
}


LONG SnapDIB::GetHeight()
{
    if (m_lpbi != NULL)
    {
        return abs(m_lpbi->biHeight);
    }

    return 0;
}



int SnapDIB::GetOrientation()
{
    if (m_lpbi != NULL)
    {
        return (m_lpbi->biHeight < 0) ? -1 : 0;
    }

    return 0;
}



WORD SnapDIB::GetPlanes()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biPlanes;
    }

    return 0;
}



WORD SnapDIB::GetBitCount()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biBitCount;
    }

    return 0;
}



DWORD SnapDIB::GetCompression()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biCompression;
    }

    return 0;
}



BOOL SnapDIB::Uncompress()
{
    BOOL                    bRet;
    BYTE *                  pb;
    BYTE *                  pbDest;
    int                     x;
    int                     y;
    int                     i;
    BYTE                    byte;
    int                     iCount;
    BOOL                    bEnd = FALSE;
    SnapDIB *                  pdib;
    BYTE                    bClr;
    int                     iWidth;
    int                     iHeight;
    LPBITMAPINFOHEADER      lpbi;
    LPBYTE                  pBits;
    WORD                    wWidthBytes;

    //
    // create our destination dib
    //
    pdib = new SnapDIB();
    if (pdib == NULL)
    {
        goto Error;
    }

    pdib->Create(GetWidth(),
                 GetHeight(),
                 8,
                 256);
    if (pdib == NULL)
    {
        goto Error;
    }


    //
    // Copy the bitmap color table information
    //
    memcpy(pdib->GetColor(0), GetColor(0), 256 * sizeof(RGBQUAD));

    pb = GetBits();
    x = 0;
    y = 0;
    iWidth = pdib->GetWidth();
    iHeight = pdib->GetHeight();
    pbDest = pdib->GetPixelPointer(x, y);

    while (!bEnd)
    {
        byte = *pb;

        if (byte == 0)
        {
            byte = *++pb;
            switch (byte)
            {
                case 0:
                    //
                    // End of line in bitmap
                    //
                    y++;
                    x = 0;
                    pbDest = pdib->GetPixelPointer(x, y);
                    pb++;
                    break;

                case 1:
                    //
                    // End of Bitmap
                    //
                    bEnd = TRUE;
                    break;

                case 2:
                    //
                    // Move to a new X, Y coordinate
                    //
                    byte = *pb++;
                    x += (int)byte;
                    byte = *pb++;
                    y += (int)byte;

                    pbDest = pdib->GetPixelPointer(x, y);
                    break;

                default:
                    //
                    // Absolute Mode (must align on word boundries)
                    //
                    pb++;
                    iCount = 0;

                    for (i = 0; i < (int)byte; i++)
                    {
                        iCount++;

                        bClr = *pb++;
                        *pbDest++ = bClr;
                        x++;
                    }

                    // Assuring word alignment
                    if ((iCount % 2) != 0)
                    {
                        pb++;
                    }
                    break;
            }
        }
        else
        {
            //
            // Here would be where the main optimizations would occure
            //
            bClr = *++pb;

            for (i = 0; i < (int)byte; i++)
            {
                *pbDest++ = bClr;
                x++;

                Assert(x < pdib->GetWidth());
            }

            pb++;
        }
    }


    lpbi = pdib->m_lpbi;
    pBits = pdib->m_pBits;
    wWidthBytes = pdib->m_wWidthBytes;

    pdib->m_lpbi = m_lpbi;
    pdib->m_pBits = m_pBits;
    pdib->m_wWidthBytes = m_wWidthBytes;

    m_lpbi = lpbi;
    m_pBits = pBits;
    m_wWidthBytes = wWidthBytes;


Cleanup:
    delete pdib;
    return bRet;

Error:
    Trace(("ERROR:  RLEDibToDib\r\n"));
    bRet = FALSE;
    goto Cleanup;
}


DWORD SnapDIB::GetSizeImage()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biSizeImage;
    }

    return 0;
}



DWORD SnapDIB::GetColorsUsed()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biClrUsed;
    }

    return 0;
}



DWORD SnapDIB::GetColorsImportant()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biClrImportant;
    }

    return 0;
}



HPALETTE SnapDIB::CreatePalette()
{
    LOGPALETTE *    pLogPal = NULL;
    HPALETTE        hpal = NULL;
    RGBQUAD *       pRgb;
    WORD            nNumColors;
    WORD            i;
    BYTE            red;
    BYTE            green;
    BYTE            blue;
    BOOL            fCoreHeader;
    BYTE *          pb;
                

    if (m_lpbi == NULL)
    {
        return NULL;
    }

    fCoreHeader = (m_lpbi->biSize == sizeof(BITMAPCOREHEADER));

    pRgb = (RGBQUAD *)((BYTE *)m_lpbi + (WORD)m_lpbi->biSize);
    nNumColors = NumColors();

    if (nNumColors)
    {
        pLogPal = (LOGPALETTE *)new BYTE[sizeof(LOGPALETTE) +
                        (nNumColors * sizeof(PALETTEENTRY))];

        if (!pLogPal)
        {
            Trace(("ERROR:  CreatePalette() -- OUT OF MEMORY -- "
                   "Couldn't create log Palette"));
            goto Error;
        }

        pLogPal->palNumEntries  = nNumColors;
        pLogPal->palVersion = PALVERSION;

        for( i = 0; i < nNumColors; i++ )
        {
            pLogPal->palPalEntry[i].peRed   = pRgb->rgbRed;
            pLogPal->palPalEntry[i].peGreen = pRgb->rgbGreen;
            pLogPal->palPalEntry[i].peBlue  = pRgb->rgbBlue;
            pLogPal->palPalEntry[i].peFlags = (BYTE)0;
            
            // If this was an old style bitmap advance only
            if (fCoreHeader)
            {
                pb = (BYTE *)pRgb;

                pb += sizeof(RGBTRIPLE);
                pRgb = (RGBQUAD *)pb;
            }
            else
            {
                pRgb++;
            }
        }
    }
    else if (GetBitCount() == 24)
    {
        nNumColors = MAXPALETTE;
        pLogPal = (LOGPALETTE *) new BYTE[sizeof (LOGPALETTE) +
                                (nNumColors * sizeof (PALETTEENTRY))];

        if (!pLogPal)
        {
            Trace(("ERROR:  CreatePalette() -- OUT OF MEMORY -- "
                   "Couldn't create log Palette (24-bit)"));
            goto Error;
        }

        pLogPal->palNumEntries = nNumColors;
        pLogPal->palVersion    = PALVERSION;

        red = green = blue = 0;

        for( i = 0; i < pLogPal->palNumEntries; i++ )
        {
            pLogPal->palPalEntry[i].peRed   = red;
            pLogPal->palPalEntry[i].peGreen = green;
            pLogPal->palPalEntry[i].peBlue  = blue;
            pLogPal->palPalEntry[i].peFlags = (BYTE)0;
            
            if (!(red += 32))
            {
                if (!(green += 32))
                {
                    blue += 64;
                }
            }
        }
    }

    hpal = ::CreatePalette(pLogPal);

Cleanup:
    if (pLogPal != NULL)
    {
        delete [] pLogPal;
    }

    return hpal;

Error:
    Trace(("ERROR:  SnapDIB::CreatePalette"));
    if (hpal != NULL)
    {
        DeleteObject(hpal);
        hpal = NULL;
    }

    goto Cleanup;
}



RGBQUAD * SnapDIB::GetPalettePtr()
{
    if (m_lpbi)
    {
        return (RGBQUAD *)((BYTE *)m_lpbi + m_lpbi->biSize);
    }
    return NULL;
}

BYTE * SnapDIB::GetPixelPointer(int x, int y)
{
    return (BYTE *)(m_pBits + (x * GetBitCount() / 8)  + (y * m_wWidthBytes));
}



BOOL SnapDIB::GetPixelRGB(int x,
                       int y,
                       BYTE * pbRed,
                       BYTE * pbGreen,
                       BYTE * pbBlue)
{
    BYTE * pb;

    pb = (BYTE *)(m_pBits + (x * GetBitCount() / 8)  + (y * m_wWidthBytes));

    *pbBlue  = *pb;
    pb++;
    *pbGreen = *pb;
    pb++;
    *pbRed   = *pb;

    return TRUE;
}




BOOL SnapDIB::SetPixelRGB(int x, int y, BYTE bRed, BYTE bGreen, BYTE bBlue)
{
    BYTE *  pb;

    pb = (BYTE *)(m_pBits + (x * GetBitCount() / 8)  + (y * m_wWidthBytes));

    switch (m_lpbi->biBitCount)
    {
        case 8:
        case 16:
            break;

        case 24:
            *pb = bBlue;
            pb++;
            *pb = bGreen;
            pb++;
            *pb = bRed;
            break;

        case 32:  // BUGBUG: This isn't how it works???
            *pb = bBlue;
            pb++;
            *pb = bGreen;
            pb++;
            *pb = bRed;
            pb++;
            *pb = 0;
            break;
    }

    return TRUE;
}



BOOL SnapDIB::SetPixel(int x, int y, BYTE bColorIndex)
{
    BYTE * pb;

    pb = (BYTE *)(m_pBits + (x * GetBitCount() / 8)  + (y * m_wWidthBytes));

    *pb = bColorIndex;

    return TRUE;
}

BOOL SnapDIB::Write(LPSTR psz)
{
    HANDLE hfile;

    hfile = CreateFile(psz,
                       GENERIC_WRITE,
                       0,
                       NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    Write(hfile);

    CloseHandle(hfile);

    return TRUE;
}

BOOL SnapDIB::Write(HANDLE hfile)
{
    BITMAPFILEHEADER    bf;
    DWORD               dwSize;
    DWORD               dwWritten;

    //
    // Write out bitmap file header
    //
    dwSize = sizeof(bf) + GetSize() + GetPaletteSize(m_lpbi) + GetSizeImage();

    bf.bfType = BFT_BITMAP;
    bf.bfSize = dwSize;
    bf.bfReserved1 = 0;
    bf.bfReserved2 = 0;
    bf.bfOffBits = GetSize() + GetPaletteSize(m_lpbi) + sizeof(bf);

    WriteFile(hfile, &bf, sizeof(bf), &dwWritten, NULL);

    dwSize = GetSize() + GetPaletteSize(m_lpbi) + GetSizeImage();
    WriteFile(hfile, m_lpbi, dwSize, &dwWritten, NULL);

    return TRUE;
}

SnapDIB * SnapDIB::DibFromBitmap(HBITMAP hbm,
                           DWORD biStyle,
                           WORD biBits,
                           HPALETTE hpal,
                           WORD wUsage)
{
    BITMAP              bm;
    SnapDIB *              pdib = NULL;
    BITMAPINFOHEADER    bi;
    BITMAPINFOHEADER *  lpbi = NULL;
    DWORD               dwLen;
    int                 nColors;
    HDC                 hdc;

    if (wUsage == 0)
    {
        wUsage = DIB_RGB_COLORS;
    }

    if (!hbm)
    {
        return NULL;
    }

    if (hpal == NULL)
    {
        hpal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
    }

    GetObject(hbm,sizeof(bm),(LPSTR)&bm);
    GetObject(hpal,sizeof(nColors),(LPSTR)&nColors);

    if (biBits == 0)
    {
        biBits = bm.bmPlanes * bm.bmBitsPixel;
    }

    nColors = (biBits < 16) ? nColors : 0;

    bi.biSize               = sizeof(BITMAPINFOHEADER);
    bi.biWidth              = bm.bmWidth;
    bi.biHeight             = bm.bmHeight;
    bi.biPlanes             = 1;
    bi.biBitCount           = biBits;
    bi.biCompression        = biStyle;
    bi.biSizeImage          = bm.bmWidth * bm.bmHeight * (biBits / 8);
    bi.biXPelsPerMeter      = 0;
    bi.biYPelsPerMeter      = 0;
    bi.biClrUsed            = nColors;
    bi.biClrImportant       = nColors;

    hdc = CreateCompatibleDC(NULL);
    hpal = SelectPalette(hdc, hpal, FALSE);
    RealizePalette(hdc);  // why is this needed on a MEMORY DC? GDI bug??

    pdib = new SnapDIB();
    if (pdib == NULL)
    {
        Trace(("ERROR:  SnapDIB::DibFromBitmap -- OOM -- "
               "Couldn't create dib"));
        goto Error;
    }

    dwLen = bi.biSize + pdib->GetPaletteSize(&bi) + bi.biSizeImage;

    pdib->m_lpbi = (LPBITMAPINFOHEADER) new BYTE[dwLen];
    if (pdib->m_lpbi)
    {
        Trace(("ERROR:  SnapDIB::DibFromBitmap -- OOM -- "
               "Couldn't create bitmap info headr"));
        goto Error;
    }

    *(pdib->m_lpbi) = bi;

    /*
     *  call GetDIBits with a NON-NULL lpBits param, and actualy get the
     *  bits this time
     */
    GetDIBits(hdc,
              hbm,
              0,
              (WORD)bi.biHeight,
              (LPSTR)pdib->m_lpbi +
              (WORD)pdib->m_lpbi->biSize +
              pdib->GetPaletteSize(pdib->m_lpbi),
              (LPBITMAPINFO)(pdib->m_lpbi),
              wUsage);

    bi = *lpbi;

Cleanup:
    SelectPalette(hdc,hpal,FALSE);
    DeleteDC(hdc);
    return pdib;

Error:
    Trace(("ERROR:  SnapDIB::DibFromBitmap"));

    if (pdib != NULL)
    {
        delete pdib;
        pdib = NULL;
    }

    goto Cleanup;
}



BOOL SnapDIB::SetColorRect(int x, int y, int iWidth, int iHeight, COLORREF cr)
{
    int     iy;
    int     ix;
    BYTE    bRed;
    BYTE    bGreen;
    BYTE    bBlue;

    if (GetBitCount() < 24)
        return FALSE;

    if (x < 0)
    {
        x = 0;
    }

    if (x + iWidth > GetWidth())
    {
        iWidth = GetWidth() - x;
    }

    if (y < 0)
    {
        y = 0;
    }

    if (y + iHeight > GetHeight())
    {
        iHeight = GetHeight() - y;
    }

    bRed = GetRValue(cr);
    bGreen = GetGValue(cr);
    bBlue = GetBValue(cr);

    for (iy = y; iy < y + iHeight; iy++)
    {
        for (ix = x; ix < x + iWidth; ix++)
        {
            SetPixelRGB(ix, iy, bRed, bGreen, bBlue);
        }
    }

    return TRUE;
}



BOOL SnapDIB::CopyDibInto(int x, int y, int iWidth, int iHeight, SnapDIB * pdib)
{
    int     iy;

    if (GetBitCount() != pdib->GetBitCount())
        return FALSE;

    if (x < 0)
    {
        x = 0;
    }

    if (x + iWidth > GetWidth())
    {
        iWidth = GetWidth() - x;
    }

    if (y < 0)
    {
        y = 0;
    }

    if (y + iHeight > GetHeight())
    {
        iHeight = GetHeight() - y;
    }

    // convert to width in bytes.
    iWidth  *= (GetBitCount() / 8);
    for (iy = y; iy < y + iHeight; iy++)
    {
        memcpy (GetPixelPointer (x, iy),
                pdib->GetPixelPointer (0, iy-y),
                iWidth);
#if 0
        // this doesn't work for 8bit images
        for (ix = x; ix < x + iWidth; ix++)
        {
            pdib->GetPixelRGB(ix - x, iy - y, &bRed, &bGreen, &bBlue);
            SetPixelRGB(ix, iy, bRed, bGreen, bBlue);
        }
#endif
    }

    return TRUE;
}


BOOL SnapDIB::CopyDibFrom (int x, int y, int iWidth, int iHeight, SnapDIB * pdib)
{
    int     iy;
    int     ixDest;
    int     iyDest;

    if (GetBitCount() != pdib->GetBitCount() ||
        pdib->GetColorsUsed() > GetColorsUsed())
        return FALSE;

    // get the color table.
    memcpy (GetColor(0), pdib->GetColor(0), pdib->GetColorsUsed()*sizeof(RGBQUAD));

    ixDest = 0;
    if (x < 0)
    {
        ixDest = -x;
        x = 0;
    }

    if (x + iWidth > pdib->GetWidth())
    {
        iWidth = pdib->GetWidth() - x;
    }

    iyDest = 0;
    if (y < 0)
    {
        iyDest = -y;
        y = 0;
    }

    if (y + iHeight > pdib->GetHeight())
    {
        iHeight = pdib->GetHeight() - y;
    }

    // convert to width in bytes.
    iWidth  *= (GetBitCount() / 8);
    for (iy = y; iy < y + iHeight; iy++)
    {
        memcpy (GetPixelPointer (ixDest, iyDest+iy-y),
                pdib->GetPixelPointer (x, iy),
                iWidth);
#if 0
        // this doesn't work for 8bit images
        for (ix = x; ix < x + iWidth; ix++)
        {
            pdib->GetPixelRGB(ix - x, iy - y, &bRed, &bGreen, &bBlue);
            SetPixelRGB(ix, iy, bRed, bGreen, bBlue);
        }
#endif
    }

    return TRUE;
}



LPBYTE SnapDIB::GetBits()
{
      return (LPBYTE)m_lpbi + (WORD)m_lpbi->biSize + GetPaletteSize(m_lpbi);
}



void SnapDIB::GrabGameScreen()
{
		Assert(prasMainScreen);

		int			i, j;
		HDC			icon_hdc;	
		HBITMAP		iconHBitmap, oldHBitmap;
		int			tmpW, tmpH;
		HDC			ImageDC;
		LPBYTE		ptrData;
		LPBYTE		ptrData2;

		prasMainScreen->FlipToGDISurface();
		prasMainScreen->Unlock();
		ImageDC = prasMainScreen->hdcGet();

		icon_hdc = CreateCompatibleDC(ImageDC);

		// Create the Dib header info
		Create(FILEICON_WIDTH, FILEICON_HEIGHT, 24, 0);

		ptrData = m_pBits;
		iconHBitmap =  CreateDIBSection(icon_hdc, (LPBITMAPINFO)m_lpbi, DIB_RGB_COLORS, 
			(void **)&ptrData, NULL, 0);

		oldHBitmap = (HBITMAP )SelectObject(icon_hdc, iconHBitmap);
		tmpW = prasMainScreen->iWidth;
		tmpH = prasMainScreen->iHeight;

//		prasMainScreen->Lock();

		SetStretchBltMode(icon_hdc, COLORONCOLOR);
		StretchBlt(icon_hdc,
                0, 0, GetWidth(), GetHeight(),
				ImageDC,
                0, 0, tmpW, tmpH,
                SRCCOPY);
		{
		ptrData2 = m_pBits;
		for (i=0; i<FILEICON_HEIGHT; i++)
			for (j=0; j<FILEICON_WIDTH; j++)
			{
				*ptrData2++   = *ptrData;
				*ptrData2++	  = *(ptrData+2);
				*ptrData2++   = *(ptrData+1);
				ptrData += 3;
			}
		}

		DeleteDC(icon_hdc);
		prasMainScreen->Unlock();
		prasMainScreen->FlipToGDISurface();
		prasMainScreen->ReleaseDC(ImageDC);
}
