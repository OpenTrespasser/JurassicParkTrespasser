/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents: 
 *		CGDIBitmap
 *
 * Notes:
 *	This class is Win32 code and does not rely on MFC at all.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/GDIBitmap.cpp                                                 $
 * 
 * 7     8/30/98 4:51p Asouth
 * loop variable
 * 
 * 6     4/21/98 2:57p Rwyatt
 * Modified for the new VM system.
 * 
 * 5     2/11/98 4:35p Rwyatt
 * Can now draw with boolean operators
 * 
 * 4     1/29/98 7:26p Rwyatt
 * Bitmap create is protected by the VM manager.
 * 
 * 3     1/19/98 7:30p Pkeet
 * Added support for 16 bit bumpmaps by adding lower colour resolution and a smaller size for
 * 'CBumpAnglePair.'
 * 
 * 2     1/09/98 7:09p Rwyatt
 * Can make DDBs from rasters that contain textures or bump maps. When a bump map is the source
 * just the texture information is used.
 * DDBs can now be individually clipped to a given rectangle
 * 
 * 1     12/22/97 12:00a Rwyatt
 * Initial implementation
 * 
 **********************************************************************************************/

//
// Includes
//
#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/View/Clut.hpp"
#include "Lib/Renderer/Primitives/FastBump.hpp"
#include "GDIBitmap.hpp"

#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/VirtualMem.hpp"

//*********************************************************************************************
//
// CGDIBitmap implementation
// NOTE: Source must be 8 bit
//			If the mode is changed all DDBs become void and should be destroyed and recreated
//
CGDIBitmap::CGDIBitmap
(
	rptr<CRaster>	pras_src,				// source raster
	int32			i4_dib_width,			// width of DIB or -1 for same as raster
	int32			i4_dib_height			// height of DIB or -1 for same as raster
)
//**************************************
{
	int32		i4_src_width = pras_src->iWidth;
	int32		i4_src_height = pras_src->iHeight;

	int32		i4_dib_pitch;
	uint32		u4_dib_colours = 256;

	RGBQUAD*	prgbCol;
	BITMAPINFO*	pbmiDib;

	// if the destination width is -1 then use the source width
	if (i4_dib_width == -1)
	{
		i4_dib_width = i4_src_width;
	}

	// if the destination height is -1 then use the source height
	if (i4_dib_height == -1)
	{
		i4_dib_height = i4_src_height;
	}

	// DIB pitch is the width rounded to the next DWORD
	i4_dib_pitch = ( i4_dib_width + 3) &~3;
	
	// the exact size of the bitmap we are about to create.
	uint32		u4_size = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*u4_dib_colours) + 
							(i4_dib_pitch*i4_dib_height);

	pbmiDib = (BITMAPINFO*) new uint8[u4_size];

	//memset(pbmiDib,0x80, u4_size);

	// fill in the header			
	pbmiDib->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmiDib->bmiHeader.biWidth			= i4_dib_width;
	pbmiDib->bmiHeader.biHeight			= -i4_dib_height;		// top down
	pbmiDib->bmiHeader.biPlanes			= 1;
	pbmiDib->bmiHeader.biBitCount		= 8;
	pbmiDib->bmiHeader.biCompression	= BI_RGB;
	pbmiDib->bmiHeader.biSizeImage		= 0;
	pbmiDib->bmiHeader.biXPelsPerMeter	= 0;
	pbmiDib->bmiHeader.biYPelsPerMeter	= 0;
	pbmiDib->bmiHeader.biClrUsed		= 0;					// set by biBitCount
	pbmiDib->bmiHeader.biClrImportant	= 0;

	// point to the palette that we are going to fill from the source
	prgbCol = (RGBQUAD*) ((LPSTR)pbmiDib + (WORD)(pbmiDib->bmiHeader.biSize)); 

	// pointer to the first byte of pixel data..
	uint8*	pu1_bits = (uint8*)(prgbCol + u4_dib_colours);
	uint8*	pu1_src = (uint8*)pras_src->pAddress(0);

	// caclulate the scaling factors
	float	f_src_xp = 0.0f;
	float	f_src_yp = 0.0f;
	float	f_dx = (float)i4_src_width / (float)i4_dib_width;
	float	f_dy = (float)i4_src_height / (float)i4_dib_height;

	//
	// copy the data
	//

	if (pras_src->iPixelBits == 8)
	{
		//
		// Our palette matches the raster palette so we can just copy 8 bit data.
		//
		f_src_yp = 0.0f;
		for (int32 i4_y = 0; i4_y<i4_dib_height;i4_y++)
		{
			f_src_xp = 0.0f;
			for (int32 i4_x = 0; i4_x<i4_dib_width;i4_x++)
			{
				pu1_bits[i4_x] = (uint8)pras_src->pixGet((int)f_src_xp, (int)f_src_yp);
				f_src_xp += f_dx;
			}
			f_src_yp += f_dy;
			pu1_bits += i4_dib_pitch;
		}
	}
	else if (pras_src->iPixelBits == sizeof(CBumpAnglePair) * 8)
	{
		//
		// A CBumpAnglePair raster is a bump map so we need to extract the colour element,
		// this is usually the top byte but we will use the class just to be sure.
		// The resulting data is 8 bits per pixel and uses the palette from the
		// raster.
		//
		CBumpAnglePair	bang;

		f_src_yp = 0.0f;
		for (int32 i4_y = 0; i4_y<i4_dib_height; i4_y++)
		{
			f_src_xp = 0.0f;
			for (int32 i4_x = 0; i4_x<i4_dib_width; i4_x++)
			{
				bang.br = pras_src->pixGet((int)f_src_xp, (int)f_src_yp);
				pu1_bits[i4_x] = bang.u1GetColour();
				f_src_xp += f_dx;
			}
			f_src_yp += f_dy;
			pu1_bits += i4_dib_pitch;
		}
	}
	else
	{
		// not a known bit depth
		Assert(0);
	}

	// we assume no palette
	CPal*	ppal = NULL;

	if (pras_src->pxf)
	{
		// if the raster has a pixel format the check for a palette
		ppal = pras_src->pxf.ppalAttached;
	}

	if (ppal)
	{
		// set the whole bitmap palette to bright red so we can see any unused colours
		uint32 u4;
		for (u4 = 0; u4<u4_dib_colours;u4++)
		{
			prgbCol[u4].rgbRed			= 255;
			prgbCol[u4].rgbGreen		= 0;
			prgbCol[u4].rgbBlue			= 0;
			prgbCol[u4].rgbReserved		= 0;
		}

		if (pras_src->iPixelBits == 8)
		{
			// only read the source colours
			uint32 u4_src_cols = ppal->aclrPalette.uLen;
			for (u4 = 0; u4<u4_src_cols; u4++)
			{
				// CColour and RGBQUAD are the same format
				CColour col = pras_src->clrFromPixel(u4<<24);
				prgbCol[u4] = *(RGBQUAD*)&col;
			}
		}
		else if (pras_src->iPixelBits == sizeof(CBumpAnglePair) * 8)
		{
			// only read the source colours
			uint32 u4_src_cols = ppal->aclrPalette.uLen;
			CBumpAnglePair	bang;

			for (u4 = 0; u4<u4_src_cols; u4++)
			{
				// we need to set the colour through a  bump angle pair class
				bang.SetColour(u4);
				CColour col = pras_src->clrFromPixel(bang.br);
				// CColour and RGBQUAD are the same format
				prgbCol[u4] = *(RGBQUAD*)&col;
			}
		}
	}
	else
	{
		// if the raster has no pixel format make a gray scale palette, but set colour
		// zero to something different so it can be distinguished from colour 1
		for (uint32 u4 = 0; u4<u4_dib_colours;u4++)
		{
			prgbCol[u4].rgbRed			= (uint8)u4;
			prgbCol[u4].rgbGreen		= (uint8)u4;
			prgbCol[u4].rgbBlue			= (uint8)u4;
			prgbCol[u4].rgbReserved		= 0;
		}
	}

	prgbCol[0].rgbRed		= 0;
	prgbCol[0].rgbGreen		= 0;
	prgbCol[0].rgbBlue		= 255;
	prgbCol[0].rgbReserved	= 0;

	// convert the palette set above in the form of a GDI palette
	hpalColours = CGDIBitmap::GetDibPalette(pbmiDib);

	//
	// Now we are going to convert the DIB into a DDB for fast blitting.....
	//

	//get a DC for the screen so we can convert to the correct format
	HDC			hdc = ::GetDC(NULL);
	HPALETTE	hpal_old;
	int32		i4;
	
	//if theres a palette associated with this dib, select it ready for convertion.
	if (hpalColours)					
	{
		hpal_old = ::SelectPalette(hdc,hpalColours,FALSE);
		i4 = ::RealizePalette(hdc);
	}

	// create the DDB and copy the bitmap to the DDB converting the colours to
	// the format speicifed by the hdc and palette
	hddbDeviceMap = ::CreateDIBitmap
					(
						hdc,
						(LPBITMAPINFOHEADER)pbmiDib,
						(LONG)CBM_INIT,
						(LPSTR)pbmiDib + pbmiDib->bmiHeader.biSize + u4_dib_colours*sizeof(RGBQUAD),
						(LPBITMAPINFO)pbmiDib,
						DIB_RGB_COLORS 
					);

	// keep a copy of the width and the height so we can blit it
	i4Width		= pbmiDib->bmiHeader.biWidth;
	i4Height	= abs( pbmiDib->bmiHeader.biHeight );

	// delete the DIB because we have the DDB.
	delete pbmiDib;

	//restore the old palette
	::SelectPalette(hdc,hpal_old,FALSE);

	// cliping area in window area pixels, by default this is the size of the screen as a 
	// window cannot be larger than this, this has the same effect as not clipping.
	rcClip.left		= 0;
	rcClip.top		= 0;
	rcClip.right	= GetSystemMetrics(SM_CXSCREEN);
	rcClip.bottom	= GetSystemMetrics(SM_CYSCREEN);
}



//*********************************************************************************************
//
CGDIBitmap::~CGDIBitmap
(
)
//**************************************
{
	DeleteObject(hddbDeviceMap);
	DeleteObject(hpalColours);
}



//*********************************************************************************************
//
EDrawStatus CGDIBitmap::Draw
(
	HDC		hdc,
	int32	i4_x,
	int32	i4_y,
	bool	b_solid,
	bool	b_realize
)
//**************************************
{
	// if we are not visibale at all do nothing...
	if ((i4_x >= rcClip.right) || (i4_y >= rcClip.bottom) || 
		(i4_x+i4Width <= rcClip.left) || (i4_y+i4Height <= rcClip.top) )
	{
		return edsNotVisible;
	}

	// create the source DC because something is visible..
	int32	i4_w	= i4Width;
	int32	i4_h	= i4Height;
	int32	i4_sx	= 0;
	int32	i4_sy	= 0;
	EDrawStatus eds = edsNotClipped;

	HDC			hdc_mem = CreateCompatibleDC(hdc);;

	// only set the palette if we have been told to do so.
	if (b_realize)
	{
		SelectPalette(hdc,hpalColours,FALSE);	
		RealizePalette(hdc);
	}

	SelectObject(hdc_mem,hddbDeviceMap);

	if (i4_x+i4Width > rcClip.right)
	{
		i4_w = rcClip.right - i4_x;
		eds = edsClipped;
	}

	if (i4_x < rcClip.left)
	{
		i4_sx	= rcClip.left - i4_x;
		i4_x	= rcClip.left;
		eds = edsClipped;
	}


	if (i4_y+i4Height > rcClip.bottom)
	{
		i4_h = rcClip.bottom - i4_y;
		eds = edsClipped;
	}

	if (i4_y < rcClip.top)
	{
		i4_sy	= rcClip.top - i4_y;
		i4_y	= rcClip.top;
		eds = edsClipped;
	}

	::BitBlt(hdc, i4_x, i4_y, i4_w,i4_h,hdc_mem,i4_sx,i4_sy,b_solid?SRCCOPY:SRCPAINT);

	DeleteDC(hdc_mem);

	return eds;
}




//*********************************************************************************************
//
HPALETTE CGDIBitmap::GetDibPalette
(
	const BITMAPINFO* pbmi
)
//**************************************
{
	LPLOGPALETTE	plogpal;			// pointer to a logical palette
	uint32			u4;					// loop index
	uint16			u2_colours;			// number of colors in color table
	HPALETTE		hpal = NULL;

	Assert(pbmi);

	// get the number of colors in the DIB (for any bit depth)
	if (pbmi->bmiHeader.biClrUsed!=0)
	{
		u2_colours = (uint16)pbmi->bmiHeader.biClrUsed;
	}
	else
	{
		u2_colours =  (pbmi->bmiHeader.biBitCount<=8) ?  (1 << pbmi->bmiHeader.biBitCount) : 0;
	}

	//if the number of colours is not zero , we need to extract the entries. 0 means its pixel data
	//is RGB data

	if (u2_colours != 0)
	{
		// allocate memory block for logical palette
		plogpal = (LOGPALETTE*)new uint8[sizeof(LOGPALETTE)+sizeof(PALETTEENTRY)*u2_colours];

		//set version and number of palette entries
		plogpal->palVersion = 0x300;
		plogpal->palNumEntries = u2_colours;

		//point to the DIB palette
		RGBQUAD* prgb = (RGBQUAD*) ((LPSTR)pbmi + (WORD)(pbmi->bmiHeader.biSize));

		// copy the colours into the logical palette
		for (u4 = 0; u4 < u2_colours; u4++)
		{
			plogpal->palPalEntry[u4].peRed		= prgb[u4].rgbRed;
			plogpal->palPalEntry[u4].peGreen	= prgb[u4].rgbGreen;
			plogpal->palPalEntry[u4].peBlue		= prgb[u4].rgbBlue;
			plogpal->palPalEntry[u4].peFlags	= 0;
		}

		// create the palette and get handle to it
		hpal=CreatePalette(plogpal);
		delete plogpal;
	}

	return hpal;	// return the handle or NULL for no palette
}


