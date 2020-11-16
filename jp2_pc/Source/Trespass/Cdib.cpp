//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1996.
//
//  File:       cdib.cpp
//
//  Contents:   Dib Reading/Writing/Manipulation Code.  This also handles
//              Conversions from other formats into DIB format
//
//  Classes:    CDib
//
//  Functions:  DetermineFileType
//
//  History:                SHernd   Created
//              13-Jun-96   SHernd   Fixed Targa Reading Code (bgr read).
//                                   Fixed PixelRGB (bgr).
//                                   Added Support For Determning Image File
//                                   Type.
//
//---------------------------------------------------------------------------


#include "precomp.h"
#pragma hdrstop

#include "Cdib.h"


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


typedef struct tagTARGAHEADER
{
    // ID Field Character Length
    BYTE    bIDLength;

    // Color Map Type
    BYTE    bColorMapType;

    // Image Type Code
    BYTE    bImageType;

    // Color Map Specification
    WORD    wColorMapOrigin;
    WORD    wColorMapLength;
    BYTE    bColorMapEntrySize;

    // Image Specification
    WORD    wXOrigin;
    WORD    wYOrigin;
    WORD    wWidth;
    WORD    wHeight;
    BYTE    bPixelSize;
    BYTE    bImageDesc;
} TARGAHEADER;




typedef enum tagIMAGETYPE
{
    IT_NONE,
    IT_BITMAP,
    IT_TARGA,
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
    if (!bRet)
    {
        TraceError(("DetermineFileType() -- Unable to read header info"));
    }

    // Check for BITMAP
    if (ab[0] == 'B' && ab[1] == 'M')
    {
        *pimagetype = IT_BITMAP;
    }
    // PICT file has the first 512 bytes of 0
    else if (dwRead == 512 && memcmp(ab, abPICT, 512) == 0)
    {
        *pimagetype = IT_PICT;
    }
    // we are going to try something different for TARGAs
    else
    {
#if 0
        // The last 26 bytes is the footer information for TARGA
        // the byts 8-23 should contain "TRUEVISION-XFILE"
        SetFilePointer(hfile, -26, NULL, FILE_END);
        bRet = ReadFile(hfile, &ab, 26, &dwRead, NULL);
        if (!bRet || (dwRead != 26))
        {
            TraceError(("DetermineFileType() -- "
                        "Unable to read last 26 bytes"));
            iRet = -2;
            goto Error;
        }

        if (strncmp((char *)&ab[8], "TRUEVISION-XFILE", 16) == 0)
        {
           *pimagetype = IT_TARGA;
        }
        else
        {
            TraceError(("DetermineFileType() -- "
                        "Unable To Determine file type"));
            iRet = -3;
            goto Error;
        }
#endif
		*pimagetype = IT_TARGA;
    }

    iRet = 1;

    SetFilePointer(hfile, 0, NULL, FILE_BEGIN);
    return iRet;
}



BOOL PASCAL DibRead(VOID * pv, VOID * pbDst, DWORD dwSize, DWORD * pdwRead)
{
    return ReadFile((HANDLE)pv, pbDst, dwSize, pdwRead, NULL);
}

BOOL PASCAL DibWrite(VOID * pv, VOID * pbSrc, DWORD dwSize, DWORD * pdwWritten)
{
    return WriteFile((HANDLE)pv, pbSrc, dwSize, pdwWritten, NULL);
}

BOOL PASCAL DibSeek(VOID * pv, DWORD dwMove, DWORD seek, DWORD * pNewOffset)
{
    DWORD   dwOffset;

    dwOffset = SetFilePointer((HANDLE)pv, dwMove, NULL, seek);

    if (pNewOffset)
    {
        *pNewOffset = dwOffset;
    }

    return TRUE;
}



//+--------------------------------------------------------------------------
//----------------           CDib Member Functions           ----------------
//---------------------------------------------------------------------------



CDib::CDib()
{
    m_lpbi = NULL;
}



CDib::~CDib()
{
    delete [] m_lpbi;
}



BOOL CDib::Create(int cx, int cy, WORD wBitCount, DWORD dwColorsUsed)
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
    bi.biClrUsed        = dwColors;
    bi.biClrImportant   = dwColors;

    dwSize = bi.biSize + (dwColors * sizeof(RGBQUAD)) + bi.biSizeImage;

    pdib = (LPBITMAPINFOHEADER)new BYTE[dwSize];
    if (pdib == NULL)
    {
        Trace(("ERROR:  CDib::Create() -- OUT OF MEMORY -- "
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
    Trace(("ERROR:  CDib::Create"));
    if (pdib != NULL)
    {
        delete pdib;
    }
    bRet = FALSE;
    goto Cleanup;
}



CDib * CDib::DibFromResource(HINSTANCE hInst, LPCSTR lpName)
{
    HRSRC               hRes;
    HGLOBAL             hresDIB;
    LPBITMAPINFOHEADER  lpbi;
    BITMAPFILEHEADER *  pbmheader;
    CDib *              pdib = NULL;
    DWORD               dwWidthBytes;
    DWORD               dwSize;

    hRes = FindResource (hInst, lpName, (LPCSTR) "DIB");

    if (!hRes)
    {
        goto Error;
    }

    hresDIB = LoadResource (hInst, hRes);

    pbmheader = (LPBITMAPFILEHEADER)LockResource (hresDIB);
    dwSize = pbmheader->bfSize - pbmheader->bfOffBits;

    if (pbmheader == NULL)
    {
        goto Error;
    }

    if (!ISDIB(pbmheader->bfType))
    {
        goto Error;
    }

    lpbi = (LPBITMAPINFOHEADER)((LPBYTE)pbmheader + sizeof(BITMAPFILEHEADER));

    pdib = new CDib;
    if (pdib == NULL)
    {
        goto Error;
    }

    if (!pdib->Create(lpbi->biWidth,
                      lpbi->biHeight,
                      lpbi->biBitCount,
                      lpbi->biClrUsed))
    {
        goto Error;
    }

    if (lpbi->biBitCount == 8)
    {
        int     iColors;

        iColors = lpbi->biClrUsed;
        if (iColors == 0)
        {
            iColors = 256;
        }

        memcpy(pdib->GetColor(0),
               (LPBYTE)lpbi + lpbi->biSize,
               sizeof(RGBQUAD) * iColors);

        dwWidthBytes = (lpbi->biWidth + 3) & ~3;
        memcpy(pdib->GetBits(),
               (LPBYTE)lpbi +
                    sizeof(BITMAPINFOHEADER) +
                    (sizeof(RGBQUAD) * iColors),
               dwWidthBytes * lpbi->biHeight);
        (pdib->GetInfoHeader())->biCompression = lpbi->biCompression;
    }
    else
    if (lpbi->biBitCount == 24)
    {
        dwWidthBytes = ((lpbi->biWidth * 3) + 3) & ~3;
        memcpy(pdib->GetBits(),
               (LPBYTE)lpbi + sizeof(BITMAPINFOHEADER),
               dwWidthBytes * lpbi->biHeight);
        (pdib->GetInfoHeader())->biCompression = lpbi->biCompression;
    }
	else
    {
        Assert(FALSE);
    }

Cleanup:        
    UnlockResource (hresDIB);
    FreeResource (hresDIB);

    return pdib;

Error:
    delete pdib;
    pdib = NULL;
    goto Cleanup;
}



//+--------------------------------------------------------------------------
//
//  Member:     CDib::DibFromFile
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
int CDib::DibFromFile(LPCSTR pszFile, CDib ** ppdib)
{
    int         iRet;
    HANDLE      hfile = INVALID_HANDLE_VALUE;
    CDib *      pdib = NULL;
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
        TraceError(("CDib::DibFromFile() -- "
                    "Unable to OpenFile"));
        iRet = -1;
        goto Error;
    }

    // Try and determine file type
    iRet = DetermineFileType(hfile, &imagetype);
    if (iRet != 1)
    {
        TraceError(("CDib::DibFromFile() -- "
                    "DetermineFileType Failed"));
        goto Error;
    }

    CloseHandle(hfile);
    hfile = INVALID_HANDLE_VALUE;

    switch (imagetype)
    {
        case IT_BITMAP:
            pdib = CDib::DibFromFile(pszFile);
            break;

        case IT_TARGA:
            pdib = CDib::DibFromTarga(pszFile);
            break;

#if 0
        case IT_PICT:
            pdib = DibFromPICT(pszFile);
            break;
#endif
    }

    if (pdib == NULL)
    {
        TraceError(("CDib::DibFromFile() -- "
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
    TraceError(("CDib::DibFromFile"));
    delete pdib;
    pdib = NULL;
    goto Cleanup;
}



CDib * CDib::DibFromFile(LPCSTR pszFile)
{
    CDib * pdib;

    pdib = new CDib();
    if (pdib == NULL)
    {
        Trace(("ERROR:  CDib::DibFromFile() -- "
               "OOM -- Couldn't create CDib"));
        goto Error;
    }

    if (!pdib->LoadFromFile(pszFile))
    {
        Trace(("ERROR:  CDib::DibFromFile() -- LoadFromFile failed"));
        goto Error;
    }

Cleanup:
    return pdib;

Error:
    Trace(("ERROR:  CDib::DibFromFile"));
    if (pdib != NULL)
    {
        delete pdib;
        pdib = NULL;
    }

    goto Cleanup;
}



BOOL CDib::LoadFromStream(VOID * pv, PFNDIBREAD pfnDibRead, PFNDIBSEEK pfnDibSeek)
{
    LPBITMAPINFOHEADER  lpbi = NULL;
    DWORD               dwLen = 0;
    DWORD               dwBits;
    DWORD               dwRead;
    LPBITMAPINFOHEADER  pbtemp = NULL;
    BOOL                bRet;
    BYTE *              pb;

    lpbi = ReadDibBitmapInfo(pv, pfnDibRead, pfnDibSeek);
    if (lpbi == NULL)
    {
        TraceError(("CDib::LoadFromStream() - "
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
        TraceError(("CDib::LoadFromStream() - OOM - "
                    "Couldn't allocate enough memory for size of bitmap"));
        goto Error;
    }

    memcpy(pbtemp,
           lpbi,
           lpbi->biSize + (DWORD)GetPaletteSize(lpbi));
    delete [] lpbi;
    lpbi = (LPBITMAPINFOHEADER)pbtemp;

    //
    // Read in the bits
    //
    pb = (BYTE *)lpbi + (WORD)lpbi->biSize + GetPaletteSize(lpbi);
    (pfnDibRead)(pv, pb, dwBits, &dwRead);
    m_pBits = pb;
    m_wWidthBytes = ((lpbi->biWidth * (lpbi->biBitCount / 8)) + 3) & ~3;

    bRet = TRUE;

Cleanup:
    m_lpbi  = lpbi;

    return bRet;

Error:
    bRet = FALSE;
    delete [] lpbi;
    lpbi = NULL;

    TraceError(("CDib::LoadFromStream"));
    goto Cleanup;
}



BOOL CDib::LoadFromFile(LPCSTR pszFile)
{
    HANDLE              hfile = INVALID_HANDLE_VALUE;
    BOOL                bRet;

    //
    // Open the file and read the DIB information
    //
    hfile = CreateFile(pszFile,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        Trace(("ERROR:  CDib::LoadFromFile() - Couldn't open file %s", pszFile));
        goto Error;
    }

    bRet = LoadFromStream(hfile, (PFNDIBREAD)DibRead, (PFNDIBSEEK)DibSeek);

Cleanup:
    if (hfile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hfile);
    }

    return bRet;

Error:
    bRet = FALSE;

    Trace(("ERROR:  CDib::LoadFromFile"));
    goto Cleanup;
}


LPBITMAPINFOHEADER CDib::ReadDibBitmapInfo(VOID * pv, 
                                           PFNDIBREAD pfnDibRead,
                                           PFNDIBSEEK pfnDibSeek)
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
    DWORD               dwRead;

    //
    // Reset file pointer and read file header
    //
    (pfnDibSeek)(pv, 0L, FILE_CURRENT, &off);
    (pfnDibRead)(pv, &bf, sizeof(BITMAPFILEHEADER), &dwRead);
    if (sizeof(BITMAPFILEHEADER) != dwRead)
    {
        return NULL;
    }

    //
    // Do we have a RC HEADER?
    //
    if (!ISDIB(bf.bfType))
    {
        bf.bfOffBits = 0L;
        (pfnDibSeek)(pv, off, FILE_BEGIN, NULL);
    }

    (pfnDibRead)(pv, &bi, sizeof(bi), &dwRead);
    if (sizeof(bi) != dwRead)
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

            (pfnDibSeek)(pv, 
                         sizeof(BITMAPCOREHEADER) - sizeof(BITMAPINFOHEADER),
                         FILE_CURRENT,
                         NULL);
            break;

        default:
            // Not a DIB!
            Trace(("ERROR:  CDib::ReadDibBitmapInfo() -- "
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
        Trace(("Error:  CDib::ReadDibBitmapInfo() -- OOM -- "
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
            (pfnDibRead)(pv, pRgb, nNumColors * sizeof(RGBTRIPLE), &dwRead);

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
            (pfnDibRead)(pv, pRgb, nNumColors * sizeof(RGBQUAD), &dwRead);
        }
    }

    if (bf.bfOffBits != 0L)
    {
        (pfnDibSeek)(pv, off + bf.bfOffBits, FILE_BEGIN, NULL);
    }

Cleanup:
    return lpbi;

Error:
    Trace(("ERROR:  CDib::ReadDibBitmapInfo"));
    if (lpbi != NULL)
    {
        delete [] lpbi;
        lpbi = NULL;
    }

    goto Cleanup;
}

LPBITMAPINFOHEADER CDib::ReadDibBitmapInfo(HANDLE hfile)
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
    DWORD               dwRead;

    if (hfile == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }

    //
    // Reset file pointer and read file header
    //
    off = SetFilePointer(hfile, 0L, NULL, FILE_CURRENT);
    ReadFile(hfile, &bf, sizeof(BITMAPFILEHEADER), &dwRead, NULL);
    if (sizeof(BITMAPFILEHEADER) != dwRead)
    {
        return NULL;
    }

    //
    // Do we have a RC HEADER?
    //
    if (!ISDIB(bf.bfType))
    {
        bf.bfOffBits = 0L;
        SetFilePointer(hfile, off, NULL, FILE_BEGIN);
    }

    ReadFile(hfile, &bi, sizeof(bi), &dwRead, NULL);
    if (sizeof(bi) != dwRead)
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

            SetFilePointer(hfile, 
                           sizeof(BITMAPCOREHEADER) - sizeof(BITMAPINFOHEADER),
                           NULL,
                           FILE_CURRENT);
            break;

        default:
            // Not a DIB!
            Trace(("ERROR:  CDib::ReadDibBitmapInfo() -- "
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
        Trace(("Error:  CDib::ReadDibBitmapInfo() -- OOM -- "
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
            ReadFile(hfile, pRgb, nNumColors * sizeof(RGBTRIPLE), &dwRead, NULL);

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
            ReadFile(hfile, pRgb, nNumColors * sizeof(RGBQUAD), &dwRead, NULL);
        }
    }

    if (bf.bfOffBits != 0L)
    {
        SetFilePointer(hfile, off + bf.bfOffBits, NULL, FILE_BEGIN);
    }

Cleanup:
    return lpbi;

Error:
    Trace(("ERROR:  CDib::ReadDibBitmapInfo"));
    if (lpbi != NULL)
    {
        delete [] lpbi;
        lpbi = NULL;
    }

    goto Cleanup;
}



WORD CDib::NumColors()
{
    return NumColors((BYTE *)m_lpbi);
}



WORD CDib::NumColors(BYTE * pv)
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



RGBQUAD * CDib::GetColor(int i)
{
    RGBQUAD * prgb;

    if ((NumColors() <= i) || (i < 0))
    {
        return NULL;
    }

    prgb = ((RGBQUAD *)((LPBYTE)(m_lpbi) + (int)(m_lpbi)->biSize));
    return &prgb[i];
}



WORD CDib::GetPaletteSize(LPBITMAPINFOHEADER lpbi)
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



HBITMAP CDib::GetBitmap(HPALETTE * pPal)
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



DWORD CDib::GetSize()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biSize;
    }

    return 0;
}



LONG CDib::GetWidth()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biWidth;
    }

    return 0;
}


WORD CDib::GetWidthBytes()
{
    if (m_lpbi == NULL)
    {
        return (WORD)-1;
    }

    return m_wWidthBytes;
}



LONG CDib::GetHeight()
{
    if (m_lpbi != NULL)
    {
        return abs(m_lpbi->biHeight);
    }

    return 0;
}



int CDib::GetOrientation()
{
    if (m_lpbi != NULL)
    {
        return (m_lpbi->biHeight < 0) ? -1 : 0;
    }

    return 0;
}



WORD CDib::GetPlanes()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biPlanes;
    }

    return 0;
}



WORD CDib::GetBitCount()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biBitCount;
    }

    return 0;
}



DWORD CDib::GetCompression()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biCompression;
    }

    return 0;
}



BOOL CDib::Uncompress()
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
    CDib *                  pdib;
    BYTE                    bClr;
    int                     iWidth;
    int                     iHeight;
    LPBITMAPINFOHEADER      lpbi;
    LPBYTE                  pBits;
    WORD                    wWidthBytes;

    //
    // create our destination dib
    //
    pdib = new CDib();
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


DWORD CDib::GetSizeImage()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biSizeImage;
    }

    return 0;
}



DWORD CDib::GetColorsUsed()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biClrUsed;
    }

    return 0;
}



DWORD CDib::GetColorsImportant()
{
    if (m_lpbi != NULL)
    {
        return m_lpbi->biClrImportant;
    }

    return 0;
}



HPALETTE CDib::CreatePalette()
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
    Trace(("ERROR:  CDib::CreatePalette"));
    if (hpal != NULL)
    {
        DeleteObject(hpal);
        hpal = NULL;
    }

    goto Cleanup;
}



LOGPALETTE * CDib::CreateLogPalette()
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
            Trace(("ERROR:  CreateLogPalette() -- OUT OF MEMORY -- "
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
            Trace(("ERROR:  CreateLogPalette() -- OUT OF MEMORY -- "
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

Cleanup:
    return pLogPal;

Error:
    Trace(("ERROR:  CDib::CreateLogPalette"));
    goto Cleanup;
}



RGBQUAD * CDib::GetPalettePtr()
{
    if (m_lpbi)
    {
        return (RGBQUAD *)((BYTE *)m_lpbi + m_lpbi->biSize);
    }
    return NULL;
}


BOOL DecompressTarga (HANDLE hfile, LPBYTE pbBits, TARGAHEADER * pth)
{
    long  lSize;
    BYTE  bLength;
    DWORD dwRead;
    int   iPixelSize;

    iPixelSize = (pth->bPixelSize / 8);
    lSize = pth->wWidth * pth->wHeight;

    while (lSize > 0)
    {
        ReadFile (hfile, &bLength, sizeof(BYTE), &dwRead, NULL);
        if (dwRead==0) return FALSE;
        if (bLength & 0x80)
        {
            bLength &= ~0x80;
			bLength  = (BYTE)__min (lSize, bLength+1);
            lSize   -= bLength;
            // read the RLE color
            ReadFile (hfile, pbBits, iPixelSize, &dwRead, NULL);    
			// we already read one.
			bLength--;
            // copy it
            while (bLength--)
            {
                memcpy (pbBits+iPixelSize, pbBits, iPixelSize);
                pbBits += iPixelSize;
            }
            pbBits += iPixelSize;
        }
        else
        {
            bLength++;
			bLength = (BYTE)__min (lSize, bLength);
            lSize -= bLength;
            // read the raw colors
            ReadFile (hfile, pbBits, iPixelSize*bLength, &dwRead, NULL);    
            pbBits += iPixelSize*bLength;
        }
    }
    return TRUE;
}

BOOL ReadTargaHeader(HANDLE hfile, TARGAHEADER * pth)
{
    DWORD   dwRead;
    BYTE    bIDLength = 0;
    char    sz[200];
    BOOL    bRet;

    //
    // Read Targa file header into global variables
    //

    ReadFile(hfile, &(pth->bIDLength), sizeof(pth->bIDLength), &dwRead, NULL);
    // we can handle this, right?
#if 0
    if (pth->bIDLength != 0)
    {
        wsprintf(sz, "Non-empty identification field");
        goto Failure;
    }
#endif

    // Read color map type
    ReadFile(hfile,
             &(pth->bColorMapType),
             sizeof(pth->bColorMapType),
             &dwRead,
             NULL);
    if (pth->bColorMapType != 0)
    {
        wsprintf(sz, "unexpected color map type (%ld)", pth->bColorMapType);
        goto Failure;
    }

    // Read Image type
    ReadFile(hfile,
             &(pth->bImageType),
             sizeof(pth->bImageType),
             &dwRead, NULL);
    if (pth->bImageType != 2 && pth->bImageType != 10)
    {
        wsprintf(sz, "Unexpected image type (%ld)", pth->bImageType);
        goto Failure;
    }

    //
    // Skip color map specification
    //
    ReadFile(hfile,
             &(pth->wColorMapOrigin),
             sizeof(pth->wColorMapOrigin),
             &dwRead,
             NULL);
    ReadFile(hfile,
             &(pth->wColorMapLength),
             sizeof(pth->wColorMapLength),
             &dwRead,
             NULL);
    ReadFile(hfile,
             &(pth->bColorMapEntrySize),
             sizeof(pth->bColorMapEntrySize),
             &dwRead, NULL);

    //
    // Read image origin
    //
    ReadFile(hfile, &(pth->wXOrigin), sizeof(pth->wXOrigin), &dwRead, NULL);
    ReadFile(hfile, &(pth->wYOrigin), sizeof(pth->wYOrigin), &dwRead, NULL);

    //
    // Read image dimensions
    //
    ReadFile(hfile, &(pth->wWidth), sizeof(pth->wWidth), &dwRead, NULL);
    ReadFile(hfile, &(pth->wHeight), sizeof(pth->wHeight), &dwRead, NULL);

    //
    // Read color depth
    //
    ReadFile(hfile,
             &(pth->bPixelSize),
             sizeof(pth->bPixelSize),
             &dwRead, NULL);

    //
    // Read image descriptor
    //
    ReadFile(hfile,
             &(pth->bImageDesc),
             sizeof(pth->bImageDesc),
             &dwRead, NULL);
    // These bits indicate alpha channel, which
    // we will ignore, but we allow to be here.
#if 0
    if ((pth->bImageDesc & 15) != 0)
    {
        wsprintf(sz,
                 "Unexpected attribute bits (%ld)\n",
                 pth->bImageDesc & 15);
        goto Failure;
    }
#endif

    //
    // Skip the identification field (it will be lost)
    //
    SetFilePointer(hfile, pth->bIDLength, 0, FILE_CURRENT);

    // NOTE: We assume that the color map is empty
    bRet = TRUE;

Cleanup:
    return bRet;

Failure:
    bRet = FALSE;
    TraceError((sz));
    goto Cleanup;
}


#if 0
#define DibWidthBytesN(lpbi, n) (UINT)WIDTHBYTES((UINT)(lpbi)->biWidth * (UINT)(n))
#define DibWidthBytes(lpbi)     DibWidthBytesN(lpbi, (lpbi)->biBitCount)
#define DibColors(lpbi)         ((RGBQUAD FAR *)((LPBYTE)(lpbi) + (int)(lpbi)->biSize))

#ifdef WIN32
    #define DibPtr(lpbi)            ((lpbi)->biCompression == BI_BITFIELDS \
                                       ? (LPVOID)(DibColors(lpbi) + 3) \
                                       : (LPVOID)(DibColors(lpbi) + (UINT)(lpbi)->biClrUsed))
#else
    #define DibPtr(lpbi)            (LPVOID)(DibColors(lpbi) + (UINT)(lpbi)->biClrUsed)
#endif
#define DibXYN(lpbi,pb,x,y,n)   (LPVOID)(                                     \
                                (BYTE *)(pb) +                          \
                                (UINT)((UINT)(x) * (UINT)(n) / 8u) +          \
                                ((DWORD)DibWidthBytesN(lpbi,n) * (DWORD)(UINT)(y)))

#define DibXY(lpbi,x,y)         DibXYN(lpbi,DibPtr(lpbi),x,y,(lpbi)->biBitCount)
#endif



BYTE * CDib::GetPixelPointer(int x, int y)
{
    return (BYTE *)(m_pBits + (x * GetBitCount() / 8)  + (y * m_wWidthBytes));
}



BOOL CDib::GetPixelRGB(int x,
                       int y,
                       BYTE * pbRed,
                       BYTE * pbGreen,
                       BYTE * pbBlue)
{
    BYTE * pb;

    pb = (BYTE *)(m_pBits + (x * GetBitCount() / 8)  + (y * m_wWidthBytes));

    *pbBlue = *pb;
    pb++;
    *pbGreen = *pb;
    pb++;
    *pbRed = *pb;

    return TRUE;
}




BOOL CDib::SetPixelRGB(int x, int y, BYTE bRed, BYTE bGreen, BYTE bBlue)
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
            *pb = 0;
            pb++;
            *pb = bGreen;
            pb++;
            *pb = bRed;
            break;
    }

    return TRUE;
}



BOOL CDib::SetPixel(int x, int y, BYTE bColorIndex)
{
    BYTE * pb;

    pb = (BYTE *)(m_pBits + (x * GetBitCount() / 8)  + (y * m_wWidthBytes));

    *pb = bColorIndex;

    return TRUE;
}


CDib * CDib::DibFromTarga(LPCSTR pszFile)
{
    CDib *          pdib = NULL;
    HANDLE          hfile = INVALID_HANDLE_VALUE;
    TARGAHEADER     th;
    int             x;
    int             y;
    DWORD           dwRead;
    BYTE            bRed;
    BYTE            bGreen;
    BYTE            bBlue;
    DWORD           dwSize;
    BYTE *          pbTargaBits = NULL;
    BYTE *          pBits;

    hfile = CreateFile(pszFile,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        Trace(("Couldn't open file %s", pszFile));
        goto Error;
    }

    if (!ReadTargaHeader(hfile, &th))
    {
        Trace(("Couldn't read targa header for %s", pszFile));
        goto Error;
    }

    pdib = new CDib();
    if (pdib == NULL)
    {
        Trace(("Couldn't Create Dib for Targa - OOM"));
        goto Error;
    }

    if (pdib->Create(th.wWidth, th.wHeight, 24, 0) == FALSE)
    {
        Trace(("Couldnt' create destinaton dib for Targa %s", pszFile));
        goto Error;
    }

    dwSize = th.wWidth * th.wHeight * (th.bPixelSize / 8);
    pbTargaBits = new BYTE[dwSize];
    if (pbTargaBits == NULL)
    {
        Trace(("ERROR:  Insufficent memory to decompress Targa."));
    }

    if (th.bImageType==10)
    {
        DecompressTarga (hfile, pbTargaBits, &th);
    }
    else
    {
        ReadFile(hfile, pbTargaBits, dwSize, &dwRead, NULL);
    }

    pBits = pbTargaBits;

    if (!(th.bImageDesc & 32))
    {
        for (y = 0; y < th.wHeight; y++)
        {
            for (x = 0; x < th.wWidth; x++)
            {
                switch (th.bPixelSize)
                {
                    case 32:
                        {
                            bBlue = *pBits++;
                            bGreen = *pBits++;
                            bRed = *pBits++;

                            pBits++;
                        }
                        break;

                    case 24:
                        {
                            bBlue = *pBits++;
                            bGreen = *pBits++;
                            bRed = *pBits++;
                        }
                        break;

                    case 16:
                        {
                            bRed = (((*(WORD*)pBits) & 0x7C00) >> 10) * 8;
                            bGreen = (((*(WORD*)pBits) & 0x03E0) >> 5) * 8;
                            bBlue = ((*(WORD*)pBits) & 0x001F) * 8;

                            pBits += sizeof(WORD);
                        }
                        break;
                }

                pdib->SetPixelRGB(x, y, bRed, bGreen, bBlue);
            }
        }
    }
    else
    {
        for (y = th.wHeight - 1; y >= 0; y--)
        {
            for (x = 0; x < th.wWidth; x++)
            {
                switch (th.bPixelSize)
                {
                    case 32:
                        {
                            bBlue = *pBits++;
                            bGreen = *pBits++;
                            bRed = *pBits++;

                            pBits++;
                        }
                        break;

                    case 24:
                        {
                            bBlue = *pBits++;
                            bGreen = *pBits++;
                            bRed = *pBits++;
                        }
                        break;

                    case 16:
                        {
                            bRed = (((*(WORD*)pBits) & 0x7C00) >> 10) * 8;
                            bGreen = (((*(WORD*)pBits) & 0x03E0) >> 5) * 8;
                            bBlue = ((*(WORD*)pBits) & 0x001F) * 8;

                            pBits += sizeof(WORD);
                        }
                        break;
                }

                pdib->SetPixelRGB(x, y, bRed, bGreen, bBlue);
            }
        }
    }

Cleanup:
    if (hfile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hfile);
    }

    if (pbTargaBits != NULL)
    {
        delete [] pbTargaBits;
    }
    return pdib;

Error:
    Trace(("ERROR:  DibFromTarga"));
    if (pdib != NULL)
    {
        GlobalFreePtr(pdib);
        pdib = NULL;
    }

    goto Cleanup;
}



DWORD CDib::CalculateWriteSize()
{
    DWORD   dwRet;

    dwRet = sizeof(BITMAPFILEHEADER);
    dwRet += GetSize() + GetPaletteSize(m_lpbi) + GetSizeImage();

    return dwRet;
}


BOOL CDib::WriteToStream(VOID * pv, PFNDIBWRITE pfnDibWrite)
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

    (pfnDibWrite)(pv, &bf, sizeof(bf), &dwWritten);

    dwSize = GetSize() + GetPaletteSize(m_lpbi) + GetSizeImage();
    (pfnDibWrite)(pv, m_lpbi, dwSize, &dwWritten);

    return TRUE;
}


BOOL CDib::Write(LPSTR psz)
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



BOOL CDib::Write(HANDLE hfile)
{
    return WriteToStream(hfile, (PFNDIBWRITE)DibWrite);
}



CDib * CDib::DibFromBitmap(HBITMAP hbm,
                           DWORD biStyle,
                           WORD biBits,
                           HPALETTE hpal,
                           WORD wUsage)
{
    BITMAP              bm;
    CDib *              pdib = NULL;
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

    pdib = new CDib();
    if (pdib == NULL)
    {
        Trace(("ERROR:  CDib::DibFromBitmap -- OOM -- "
               "Couldn't create dib"));
        goto Error;
    }

    dwLen = bi.biSize + pdib->GetPaletteSize(&bi) + bi.biSizeImage;

    pdib->m_lpbi = (LPBITMAPINFOHEADER) new BYTE[dwLen];
    if (pdib->m_lpbi)
    {
        Trace(("ERROR:  CDib::DibFromBitmap -- OOM -- "
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
    Trace(("ERROR:  CDib::DibFromBitmap"));

    if (pdib != NULL)
    {
        delete pdib;
        pdib = NULL;
    }

    goto Cleanup;
}



BOOL CDib::SetColorRect(int x, int y, int iWidth, int iHeight, COLORREF cr)
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



BOOL CDib::CopyDibInto(int x, int y, int iWidth, int iHeight, CDib * pdib)
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


BOOL CDib::CopyDibFrom (int x, int y, int iWidth, int iHeight, CDib * pdib)
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



LPBYTE CDib::GetBits()
{
      return (LPBYTE)m_lpbi + (WORD)m_lpbi->biSize + GetPaletteSize(m_lpbi);
}


#if 0
BOOL CDib::IndexToLogPalette(LOGPALETTE * pLogPal)
{
    BOOL        bRet;

    if ((GetBitCount() == 8) &&
        !ChangeBitDepth(24, NULL))
    {
        TraceError(("CDib::IndexToLogPalette() -- "
                    "Changing bit depth from 8 to 24 bit failed"));
        goto Error;
    }

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CDib::ChangeBitDepth"));
    bRet = FALSE;
    goto Cleanup;
}

BOOL CDib::ChangeBitDepth(int iBitDepth, LOGPALETTE * pLogPal)
{
    BOOL                    bRet;
    CDib *                  pdib;
    HPALETTE                hpal = NULL;
    LPBITMAPINFOHEADER      lpbi;
    LPBYTE                  pb;
    WORD                    w;

    if (pLogPal)
    {
        hpal = ::CreatePalette(pLogPal);
    }

    pdib = ReduceColor(this, 
                       hpal, 
                       NULL, 
                       FALSE, 
                       iBitDepth,
                       GetWidth(),
                       GetHeight());
    if (pdib == NULL)
    {
        TraceError(("CDib::ChangeBitDepth() -- Unable to reduce color"));
        goto Error;
    }

    // flip-flop values
    lpbi = m_lpbi;
    pb = m_pBits;
    w = m_wWidthBytes;

    m_lpbi = pdib->m_lpbi;
    m_pBits = pdib->m_pBits;
    m_wWidthBytes = pdib->m_wWidthBytes;

    pdib->m_lpbi = lpbi;
    pdib->m_pBits = pb;
    pdib->m_wWidthBytes = w;

    // deleting the "old bitdepth"
    delete pdib;

    bRet = TRUE;

Cleanup:
    if (hpal != NULL)
    {
        DeleteObject(hpal);
    }
    return bRet;

Error:
    TraceError(("CDib::ChangeBitDepth"));
    bRet = FALSE;
    goto Cleanup;
}


//
// This method uses GDI to speed up the conversion
// to 8bit.
//
//
CDib *  ReduceColor(CDib *pSrc, 
					HPALETTE hPal, 
					RGBQUAD *pRGBSource, 
					BOOL fDither,
					int      nBitDepth,
					int		 nWidth,
					int		 nHeight)
{
	CDib *          pdib;
	HDC             hdc;
	HBITMAP         hbitmap;
    HBITMAP         hbmpold;
	LPBYTE          pBits;
	HDRAWDIB        hdd;
    PALETTEENTRY    palentry;
    RGBQUAD *       rgb;
    int             i;


	//
    // create our destination dib
    //
    pdib = new CDib();
    if (pdib == NULL)
    {
        TraceError(("ReduceColor() -- OOM "));
        return NULL;
    }

	if (nHeight == -1) nHeight = pSrc->GetHeight();
	if (nWidth == -1) nWidth = pSrc->GetWidth();
    pdib->Create(nWidth,
                 nHeight,
                 nBitDepth,
                 pRGBSource ? 256 : pSrc->GetColorsUsed());

    if (nBitDepth < 16)
    {
	    if (pRGBSource)
        {
		    memcpy (pdib->GetColor(0), pRGBSource, sizeof(RGBQUAD)*256);
        }
	    else if (pSrc->GetBitCount() < 16)
        {
            memcpy (pdib->GetColor(0), pSrc->GetColor(0), sizeof (RGBQUAD)*pSrc->GetColorsUsed());
        }
        else
        {
            // based off of the destination hpal create the destination 
            rgb = pdib->GetColor(0);
            for (i = 0; i < (int)pdib->GetColorsUsed(); i ++)
            {
                GetPaletteEntries(hPal, i, 1, &palentry);
                rgb[i].rgbBlue = palentry.peBlue;
                rgb[i].rgbGreen = palentry.peGreen;
                rgb[i].rgbRed = palentry.peRed;
                rgb[i].rgbReserved = 0;
            }
        }
    }

	hdc     = CreateCompatibleDC(NULL);

	if (hPal) SelectPalette (hdc, hPal, FALSE);
	RealizePalette (hdc);
	hbitmap = CreateDIBSection (hdc,
			(LPBITMAPINFO)pdib->GetInfoHeader(),
			DIB_RGB_COLORS,
			(void **)&pBits,
			NULL,0);
	hbmpold = (HBITMAP)SelectObject (hdc, hbitmap);
	if (hPal) SelectPalette (hdc, hPal, FALSE);
	RealizePalette (hdc);

	if (fDither)
	{
		hdd = DrawDibOpen();
		if (hPal) DrawDibSetPalette (hdd, hPal);
		DrawDibDraw (hdd, hdc,
			0,0,pdib->GetWidth(),pdib->GetHeight(),
			pSrc->GetInfoHeader(),pSrc->GetBits(),
			0,0,pSrc->GetWidth(),pSrc->GetHeight(),
			0);
		DrawDibClose (hdd);
	}
	else
	{
		StretchDIBits (hdc,
			0,0,pdib->GetWidth(),pdib->GetHeight(),
			0,0,pSrc->GetWidth(),pSrc->GetHeight(),
			pSrc->GetBits(),
			(LPBITMAPINFO)pSrc->GetInfoHeader(),
			DIB_RGB_COLORS,
			SRCCOPY);
	}

	memcpy (pdib->GetBits(), pBits, pdib->GetSizeImage());
	SelectObject (hdc, hbmpold);
	DeleteObject (hbitmap);
	SelectObject (hdc, GetStockObject (DEFAULT_PALETTE));
	DeleteDC (hdc);

	return pdib;
}

#endif
