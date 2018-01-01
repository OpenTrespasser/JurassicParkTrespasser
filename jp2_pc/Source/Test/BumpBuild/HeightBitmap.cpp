/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 ***********************************************************************************************
 *
 * $Log: /JP2_PC/Source/Test/BumpBuild/HeightBitmap.cpp $
 * 
 * 2     9/16/96 3:11p Pkeet
 * Basic construction of a greyscale bitmap implemented.
 * 
 * 1     9/16/96 2:06p Pkeet
 * Initial implementation.
 * 
 * 1     9/16/96 2:00p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "HeightBitmap.hpp"
#include <stdio.h>


CGreyBitmap::CGreyBitmap(int i_width, int i_height)
{
	//
	// Fill the file header for the windows bitmap.
	//
	bmfhFileHeader.bfType      = 0x4d42;
	bmfhFileHeader.bfSize      = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
		                         256 * sizeof(RGBQUAD) + i_width * i_height;
	bmfhFileHeader.bfReserved1 = 0;
	bmfhFileHeader.bfReserved2 = 0;
	bmfhFileHeader.bfOffBits   = sizeof(BITMAPFILEHEADER) +	sizeof(BITMAPINFOHEADER) +
		                         256 * sizeof(RGBQUAD);

	//
	// Fill the information header for the windows bitmap.
	//
	bmiInfoHeader.biSize          = sizeof(BITMAPINFOHEADER); 
	bmiInfoHeader.biWidth         = i_width; 
	bmiInfoHeader.biHeight        = i_height;
	bmiInfoHeader.biPlanes        = 1; 
	bmiInfoHeader.biBitCount      = 8;
	bmiInfoHeader.biCompression   = BI_RGB; 
	bmiInfoHeader.biSizeImage     = i_width * i_height; 
	bmiInfoHeader.biXPelsPerMeter = 0; 
	bmiInfoHeader.biYPelsPerMeter = 0; 
	bmiInfoHeader.biClrUsed       = 256; 
	bmiInfoHeader.biClrImportant  = 256; 
 
	//
	// Allocate memory for the bitmap.
	//
	au1HeightArray = new unsigned char[bmiInfoHeader.biSize];

	//
	// Fill the palette.
	//
	for (int i_pal = 0; i_pal < 256; i_pal++)
	{
		rgbqColors[i_pal].rgbBlue     = i_pal;
		rgbqColors[i_pal].rgbGreen    = i_pal;
		rgbqColors[i_pal].rgbRed      = i_pal;
		rgbqColors[i_pal].rgbReserved = 0;
	}

}

void CGreyBitmap::WriteToFile(char* str_filename)
{
	HFILE hfile_out;

	//
	// Open file for writing.
	//

	// Attempt to open existing file.
	hfile_out = _lopen(str_filename, OF_WRITE);

	// Create a file if no previous file exists.
	if (hfile_out == HFILE_ERROR)
	{
		hfile_out = _lcreat(str_filename, 0);
	}

	// Error: if (hfile_out == HFILE_ERROR)

	//
	// Write headers and palette.
	//
	int i_header_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
	//_hwrite(hfile_out, (const char *)this, i_header_size);

	_hwrite(hfile_out, (const char *)&bmfhFileHeader, sizeof(BITMAPFILEHEADER));
	_hwrite(hfile_out, (const char *)&bmiInfoHeader,  sizeof(BITMAPINFOHEADER));
	_hwrite(hfile_out, (const char *)rgbqColors,     sizeof(RGBQUAD) * 256);

	//
	// Write the bitmap data.
	//
	_hwrite(hfile_out, (const char *)au1HeightArray, bmiInfoHeader.biSizeImage);
}