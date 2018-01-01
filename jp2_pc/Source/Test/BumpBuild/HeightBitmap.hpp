/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 ***********************************************************************************************
 *
 * $Log: /JP2_PC/Source/Test/BumpBuild/HeightBitmap.hpp $
 * 
 * 2     9/16/96 3:11p Pkeet
 * Basic construction of a greyscale bitmap implemented.
 * 
 * 1     9/16/96 2:06p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_BUMPBUILD_HEIGHTBITMAP_HPP
#define HEADER_TEST_BUMPBUILD_HEIGHTBITMAP_HPP

#include <Windows.h>


class CGreyBitmap
{

public:

	BITMAPFILEHEADER bmfhFileHeader;
	BITMAPINFOHEADER bmiInfoHeader;
	RGBQUAD          rgbqColors[256];
	unsigned char*   au1HeightArray;

public:

	unsigned char u1GetPixel(int i_x, int i_y)
	{
		return au1HeightArray[i_y * bmiInfoHeader.biWidth + i_x];
	};

	void SetPixel(int i_x, int i_y, unsigned char u1)
	{
		au1HeightArray[i_y * bmiInfoHeader.biWidth + i_x] = u1;
	};

	CGreyBitmap(int i_width, int i_height);

	void WriteToFile(char* str_filename);

};

#endif
