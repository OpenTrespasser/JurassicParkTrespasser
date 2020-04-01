/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Implementation of Grab.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Grab.cpp                                                     $
 * 
 * 8     8/13/98 6:09p Mlange
 * Paint message now requires registration of the recipients.
 * 
 * 7     8/05/98 5:28p Mlange
 * Added auto frame grab capability.
 * 
 * 6     2/10/98 8:45p Pkeet
 * Added functions and data members to create borders for grabs that cannot be sized exactly
 * correctly.
 * 
 * 5     2/10/98 8:21p Pkeet
 * Added the invert grab macro.
 * 
 * 4     2/10/98 8:14p Pkeet
 * Initial implementation of the grab code performs a grab.
 * 
 **********************************************************************************************/

#include <stdio.h>
#include "Common.hpp"
#include "Grab.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/View/ColourT.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPaint.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"


//
// Internal class definitions.
//

//**********************************************************************************************
//
class CColour24 : public CColourT<uint8>
//
// Class for handling 24-bit colour pixels.
//
// Prefix: col3
//
//**************************************
{
public:
};


//
// Class implementation.
//

//**********************************************************************************************
//
// CGrabRaster implementation.
//

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	//******************************************************************************************
	CGrabRaster::CGrabRaster()
		: pcol3Data(0), pcol3Border(new CColour24())
	{
		// Initialize iWidth and iHeight.
		SetDimensions();

		// Initialize with a default colour.
		SetBorderColour();
	}

	//******************************************************************************************
	CGrabRaster::~CGrabRaster()
	{
		ReleaseLastGrab();
		delete pcol3Border;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	void CGrabRaster::SetDimensions(int i_width, int i_height)
	{
		Assert((i_width & 0xF) == 0);

		iWidth  = i_width;
		iHeight = i_height;
	}

	//******************************************************************************************
	void CGrabRaster::SetBorderColour(int i_red, int i_green, int i_blue)
	{
		pcol3Border->tRed   = i_red;
		pcol3Border->tGreen = i_green;
		pcol3Border->tBlue  = i_blue;
	}

	//******************************************************************************************
	void CGrabRaster::GrabBackbuffer()
	{
		Assert(prasMainScreen);

		int i_width;
		int i_height;
		int i_sample_size = 0;
		int i_sample_area;

		// Release the memory for an existing buffer.
		ReleaseLastGrab();

		// Get an appropriate width and height for a temporary buffer.
		do
		{
			++i_sample_size;
			i_width  = prasMainScreen->iWidth  / i_sample_size;
			i_height = prasMainScreen->iHeight / i_sample_size;
		}
		while (i_width > iWidth && i_height > iHeight);
		i_sample_area = i_sample_size * i_sample_size;

		// Create a temporary buffer.
		CColour24* pcol3 = new CColour24[i_width * iHeight];

		// Average a sample area for each pixel.
		prasMainScreen->Lock();
		{
			CColour24* pcol3_pos = pcol3;
		#if bINVERT_GRAB
			for (int j = 0; j < i_height; ++j)
		#else // bINVERT_GRAB
			for (int j = i_height - 1; j >= 0; --j)
		#endif // bINVERT_GRAB
				for (int i = 0; i < i_width; ++i, ++pcol3_pos)
				{
					// Initialize the pixel.
					int i_red   = 0;
					int i_green = 0;
					int i_blue  = 0;

					int i_x_start = i * i_sample_size;
					int i_y_start = j * i_sample_size;
					for (int i_sample = 0; i_sample < i_sample_size; ++i_sample)
						for (int j_sample = 0; j_sample < i_sample_size; ++j_sample)
						{
							// Sum the sample area colours.
							CColour col = prasMainScreen->clrFromPixel(prasMainScreen->pixGet
							(
								i_x_start + i_sample,
								i_y_start + j_sample
							));
							i_red   += int(col.u1Red);
							i_green += int(col.u1Green);
							i_blue  += int(col.u1Blue);
						}
					pcol3_pos->tRed   = i_red / i_sample_area;
					pcol3_pos->tGreen = i_green / i_sample_area;
					pcol3_pos->tBlue  = i_blue / i_sample_area;
				}
		}
		prasMainScreen->Unlock();

		//
		// Return the DIB data pointer without reformatting if it is already of the required
		// size.
		//
		if (i_width == iWidth && i_height == iHeight)
		{
			pcol3Data = pcol3;
			return;
		}

		//
		// Create a DIB data block of the right size, and centre and copy the temporary DIB
		// block into it.
		//
		Assert(iWidth  > i_width);
		Assert(iHeight > i_height);
		pcol3Data = new CColour24[iWidth * iHeight];

		// Clear the block.
		{
			CColour24* pcol3_pos = pcol3Data;
			for (int j = 0; j < iHeight; ++j)
				for (int i = 0; i < iWidth; ++i, ++pcol3_pos)
					*pcol3_pos = *pcol3Border;
		}

		// Copy the block.
		{
			int i_x_offset = (iWidth - i_width) / 2;
			int i_y_offset = (iHeight - i_height) / 2;
			CColour24* pcol3_src  = pcol3;
			CColour24* pcol3_dest = pcol3Data + i_x_offset + i_y_offset * iWidth;

			for (int j = 0; j < i_height; ++j, pcol3_dest += iWidth)
			{
				CColour24* pcol3_dest_line = pcol3_dest;
				for (int i = 0; i < i_width; ++i, ++pcol3_src, ++pcol3_dest_line)
					*pcol3_dest_line = *pcol3_src;
			}
		}

		// Delete the memory for the temporary block.
		delete[] pcol3;
	}

	//******************************************************************************************
	void* CGrabRaster::pvGetRawData() const
	{
		Assert(pcol3Data);
		return (void*)pcol3Data;
	}

	//******************************************************************************************
	void CGrabRaster::ReleaseLastGrab()
	{
		// Do nothing if no data is present.
		if (!pcol3Data)
			return;

		delete[] pcol3Data;
		pcol3Data = 0;
	}

	//******************************************************************************************
	void CGrabRaster::DumpBitmap(const char* str) const
	{
		// Do nothing if no data is present.
		if (!pcol3Data)
			return;
		Assert(str);

		// Get the size of the data.
		int i_size = iWidth * iHeight * 3;

		// Open the file for writing.
		FILE* pfile = fopen(str, "wb");
		Assert(pfile);

		// Create a windows bitmap file header.
		BITMAPFILEHEADER bfh;
		bfh.bfType      = 0x4D42;
        bfh.bfSize      = sizeof(BITMAPFILEHEADER);
        bfh.bfReserved1 = 0;
        bfh.bfReserved2 = 0;
        bfh.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		// Create a windows bitmap header.
		BITMAPINFOHEADER bih;

		bih.biSize          = sizeof(BITMAPINFOHEADER);
		bih.biWidth         = iWidth;
		bih.biHeight        = iHeight;
		bih.biPlanes        = 1;
		bih.biBitCount      = 24;
		bih.biCompression   = BI_RGB;
		bih.biSizeImage     = i_size;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed       = 0;
		bih.biClrImportant  = 0;

		// Write all information.
		Verify(fwrite(&bfh, bfh.bfSize, 1, pfile));
		Verify(fwrite(&bih, bih.biSize, 1, pfile));
		Verify(fwrite(pcol3Data, i_size, 1, pfile));

		// Close the file.
		fclose(pfile);
	}


//**********************************************************************************************
//
// CAutoGrabber implementation.
//

	//******************************************************************************************
	CAutoGrabber::CAutoGrabber()
		: pgrabActive(0), iFrameCount(0)
	{
		SetInstanceName("CAutoGrabber");

		// Register this entity with the message types it needs to receive.
		CMessagePaint::RegisterRecipient(this);
	}

	//******************************************************************************************
	CAutoGrabber::~CAutoGrabber()
	{
		CMessagePaint::UnregisterRecipient(this);

		Stop();
	}

	//******************************************************************************************
	void CAutoGrabber::Start(const char* str_output_name, int i_width, int i_height)
	{
		Stop();

		pgrabActive = new CGrabRaster;
		pgrabActive->SetDimensions(i_width, i_height);

		strBaseName = str_output_name;
	
		// Strip of extension and digits, if any.
		strBaseName = strBaseName.substr(0, strBaseName.find_last_of('.'));

		while (isdigit(strBaseName[strBaseName.length() - 1]))
			strBaseName = strBaseName.substr(0, strBaseName.length() - 1);
	}

	//******************************************************************************************
	void CAutoGrabber::Stop()
	{
		if (bIsActive())
		{
			delete pgrabActive;
			pgrabActive = 0;

			iFrameCount = 0;
			strBaseName = "";
		}
	}

	//******************************************************************************************
	void CAutoGrabber::Process(const CMessagePaint& msgpaint)
	{
		if (!pgrabActive || !gmlGameLoop.bCanStep())
			return;

		char str_num[5];
		sprintf(str_num, "%.4d", iFrameCount);

		std::string str_name = strBaseName + str_num + ".bmp";

		pgrabActive->GrabBackbuffer();
		pgrabActive->DumpBitmap(str_name.c_str());
		pgrabActive->ReleaseLastGrab();

		iFrameCount++;
	}


//
// Global variables.
//
CGrabRaster grabRaster;

CAutoGrabber* pagAutoGrabber;
