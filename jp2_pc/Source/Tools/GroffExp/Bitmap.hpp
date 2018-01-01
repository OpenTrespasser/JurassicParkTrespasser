/***********************************************************************************************
 *
 * Copyright (c) 1996 DreamWorks Interactive, 1996
 *
 * Contents: Class definitions for managing bitmaps in a general way.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/Bitmap.hpp                                             $
 * 
 * 4     12/16/96 11:22a Gstull
 * Made changes to support quantization to a single palette.
 * 
 * 3     11/15/96 11:08a Gstull
 * Added substantial changes to the GroffExporter including for support of mutiple section
 * files.
 * 
 * 2     11/06/96 7:29p Gstull
 * Added new location for the logfiles.
 * 
 * 1     11/04/96 7:02p Gstull
 *
 **********************************************************************************************/

#ifndef HEADER_TOOLS_BITMAP_HPP
#define HEADER_TOOLS_BITMAP_HPP

#include <fstream.h>
#include <stdio.h>


#include "StandardTypes.hpp"

#define MAX_BITMAP_WIDTH	100000
#define MAX_BITMAP_HEIGHT	100000
 
class CBitmapInfo;
class CBitmapImage;
class CBitmapIO;
class CBitmap;


//**********************************************************************************************
//
struct SColor24
//
// Prefix: c24
//
// This class provides the functions and internal data structures to manage and object which 
// exists in the scene.
//
{
	uint8 u1Blue;
	uint8 u1Green;
	uint8 u1Red;
};


//**********************************************************************************************
// 
// Prefix: fmt
enum EBitmapFormat {
	EFMT_UNDEFINED,				// No file format has been selected.
	EFMT_BMP,					// Windows BMP bitmap format.
	EFMT_GROFF					// GROFF bitmap format.
};


//**********************************************************************************************
//
class CBitmapInfo
//
// Prefix: bmi
//
// This class provides the functions and internal data structures to manage and object which 
// exists in the scene.
//
{
#define MAX_NAME_LENGTH 128

	EBitmapFormat	fmtFileFormat;
	uint			uBitmapWidth;
	uint			uBitmapHeight;
	uint			uBitmapDepth;
	float			fBitmapAspect;
	float			fBitmapGamma;

	// For paletted bitmaps there is the following additional information.
	uint			uBitmapPaletteSize;
	uint			uBitmapPaletteEntries;

	// Information on the name of the bitmap and any user specified information.
	char			strBitmapName[MAX_NAME_LENGTH];
	char*			strBitmapDescription;

public:
					// Constructor/Destructor functions.
					CBitmapInfo();
					~CBitmapInfo();

	void			Name(const char* str_bitmap_name);
	bool			bDescription(const char* str_description);
	void			BitmapFormat(EBitmapFormat fmt_file_format);

	void			Width(uint u_width);
	void			Height(uint u_height);
	void			Depth(uint u_depth);
	void			PaletteEntries(uint u_palette_entries);

	void			Gamma(float f_gamma);
	void			Aspect(float f_aspect);

	const char*		strName() const;
	const char*		strDescription() const;
	EBitmapFormat	fmtBitmapFormat() const;

	uint			uWidth() const;
	uint			uHeight() const;
	uint			uDepth() const;
	uint			uPaletteSize() const;
	uint			uPaletteEntries() const;

	float			fGamma() const;
	float			fAspect() const;

	void			Initialize();

	bool			bIsValidDefinition() const;
	bool			bIsValidRepresentation() const;

	CBitmapInfo&	operator =(CBitmapInfo& bi_src);
};
	

//**********************************************************************************************
//
class CBitmapImage 
//
// Prefix: bmi
//
// This class provides the functions and internal data structures to manage the storage 
// of palette and raw image data associated with a bitmap.
//
{
#define MAX_PALETTE_SIZE 256

	CBitmapInfo		biImageBitmapInfo;

	uint			uBufferSize;		// The size of the buffer containing the pixel data.

	// For representation of 24 bit, true color bitmaps.
	SColor24*		ac24TruePixels;

	// For representation of 8 bit paletted bitmaps.
	uint8*			au1PalettedPixels;
	SColor24		ac24Palette[MAX_PALETTE_SIZE];

	bool			bRangeCheckAndClip(uint u_width, uint u_x, uint& u_length);
public:
					CBitmapImage();
					~CBitmapImage();

	void			Initialize();

	bool			bCreate(CBitmapInfo& pbi_bitmap_info, bool b_initialize);
	void			bDestroy();

	uint			uGetTruePixels(uint u_x, uint u_y, uint u_count, SColor24* ac24_pixels);
	uint			uSetTruePixels(uint u_x, uint u_y, uint u_count, SColor24* ac24_pixels);
					

	uint			uGetPalettedPixels(uint u_x, uint u_y, uint u_count, uint8* au1_pixels);
	uint			uSetPalettedPixels(uint u_x, uint u_y, uint u_count, uint8* au1_pixels);

	uint			uGetPaletteEntries(uint u_start, uint u_count, SColor24* ac24_user_palette);
	uint			uSetPaletteEntries(uint u_start, uint u_count, SColor24* ac24_user_palette);

	// Allow the user to modify a couple of the bitmap properties. (Name, Description, Format).
	bool			bBitmapInfo(CBitmapInfo& bi_bitmap_info);
	CBitmapInfo&	biBitmapInfo();		
	
	
	bool			bIsPaletted() const;	// Is the current image a paletted bitmap?
	bool			bIsValidImage();		// Is the current image a valid image?

	void*			Data() const;
	
	CBitmapImage&	operator =(CBitmapImage& bmi_src);
};


//**********************************************************************************************
//
class CBitmapIO 
//
// Prefix: bio
//
// This class provides the functions and internal data structures to manage the storage 
// of palette and raw image data associated with a bitmap.
//
{
public:
			CBitmapIO() {};
			~CBitmapIO() {};

	bool	bLoad(const char* str_bitmap_name, CBitmapImage& bmi_bitmap_image);
	bool	bSave(CBitmapImage& bmi_bitmap_image);
	bool	bCopy(const char* str_dst, const char* str_src);
};

/*
//**********************************************************************************************
//
class CBitmapUtil 
//
// Prefix: butil
//
// This class provides the functions and internal data structures to perform general purpose 
// operations on the bitmaps.
{
	Quantizer*		pqQuantizer;		// Quantization structures.
	uint			uPaletteSize;		// The maximum user palette size.
	ColorPacker*	*pcpColorPacker;	// The pixel color packing structure.
	SColor24*		ac24Palette;		// Local palette for quantization purposes.
	bool			bTransparency;		// Boolean indicating whether transparency is in use. 

public:
			CBitmapUtil();
			~CBitmapUtil();

			// Method to determine whether a bitmap exists or not.
    bool	bExists(const char* str_bitmap_name);

			// Method to copy the bitmap.
	bool	bCopy(const char* str_dst, const char* str_src);

			// Methods for quantizing and converting bitmaps from 24 bit to 8 bit paletted bitmaps.
	bool	bConfigureQuantizer(uint u_maximum_palette_entries);
	bool	bQuantizePixels(CBitmapImage& bmi_bitmap_image);
	bool	bPalettizeBitmap(CBitmapImage& bmi_dst, CBitmapImage& bmi_dst& u1_palette_size); 

			// Method to merge a texture map and an opacity map.  This method requires an
			// 8 bit bitmap. 
	bool	bMerge(CBitmapImage& bmi_dst, CBitmapImage& bmi_texture, CBitmap bmi_opacity);

			// Method to flip a bitmap in upto two directions simultaneously.
	bool	bFlip(CBitmapImage& bmi_bitmap_image, bool b_vertical, bool b_horizontal);

			// Method to return the mean color of a bitmap.
	bool	bMeanColor(CBitmapImage& bmi_bitmap_image, SColor24& c24_mean_color);
};
*/

//**********************************************************************************************
//
class CBmpBitmap
//
// Prefix: bmpbm
//
// This class provides the functions and internal data structures to load and store the various 
// kinds of windows DIB bitmaps.
//
{
#define WINDOWS_BMP_MAGIC_NUMBER 0x4D42	// 'BM'

// Make sure all fields are packed together even though it is slower to access.
#pragma pack(push, 1)

	//
	// Define the structural representations for Windows DIB files.
	//

	// Prefix: fh
	struct SFileHeader
	{
		uint16	u2FileType;		// Windows DIB files.
		uint	uFileSize;		// The size of the file.
		uint16	uReserved1;		// Reserved.
		uint16	uReserved2;		// Reserved.
		uint	uBitmapOffset;	// The offset to the bitmap data in bytes.
	};

	// Prefix: bmh
	struct SBitmapHeader
	{
		uint	uHeaderSize;	// The size of this head in bytes.
		long	lWidth;			// The width of the header in pixels.
		long	lHeight;		// The height of the header in pixels.
		uint16	u2Planes;		// The number of planes in the image.
		uint16	u2BitsPerPixel;	// The number of bits per pixel.

		uint	uCompression;	// Compression methods used.
		uint	uBitmapSize;	// The size of the bitmap when decompressed.
		uint	uHorizRes;		// Horizontal resolution in pixels per meter.
		uint	uVerticalRes;	// Vertical resolution in pixels per meter.
		uint	uColorsUsed;	// The number of colors in the palette.
		uint	uColorsReqd;	// The number of pixels required for reasonable image quality.
	};

	// Prefix: bple
	struct SPaletteEntry
	{
		uint8	u1Blue;
		uint8	u1Green;
		uint8	u1Red;
		uint8	u1Reserved;
	};

	// Prefix: bpxe
	struct SPixelEntry
	{
		uint8	u1Blue;
		uint8	u1Green;
		uint8	u1Red;
	};

#pragma pack(pop)

public:
			CBmpBitmap() {};
			~CBmpBitmap() {};

	bool	bDetectBitmap(const char* str_bitmap_name);

	bool	bRead(const char* str_bitmap_name, CBitmapImage& bmi_bitmap_image);
	bool	bWrite(CBitmapImage& bmi_bitmap_image);
};


//**********************************************************************************************
//
class CGrfBitmap
//
// Prefix: groffbm
//
// This class provides the functions and internal data structures to load and store the various 
// kinds of windows DIB bitmaps.
//
{
#pragma pack(push, 1)
#pragma pack(pop)

public:
			CGrfBitmap() {};
			~CGrfBitmap() {};

	bool	bDetectBitmap(const char* str_bitmap_name);

	bool	bRead(const char* str_bitmap_name, CBitmapImage& bmi_bitmap_image);
	bool	bWrite(CBitmapImage& bmi_bitmap_image);
};


/*
//**********************************************************************************************
//
class CBitmapUtil
//
// Prefix: bu
//
// This class provides methods which perform logical operations on bitmaps.
//
{
public:
	CBitmapUtil();
	~CBitmapUtil();


	// bool	bResize(int	i_width, int i_height);
	// bool	bPalettize(int i_palette_size);
};


//**********************************************************************************************
//
class CQuantize
//
// Prefix: qn
//
// This class provides the functions and internal data structures for quantizing 24 bit color 
// images to paletted images.
//
{
	uint		u_palette;

public:
				CQuantize();
				~CQuantize();

	void		ColorTableSize(uint u_count);

	void		AddPixels(const SColor24* pc24_buffer, const uint u_count);
	uint8*		pu1Quantize(uint8* pu1_dstconst SColor24* pc24_buffer, const uint u_count);

	CQuantize&	operator =(CQuantize& src);	
};
*/

#endif