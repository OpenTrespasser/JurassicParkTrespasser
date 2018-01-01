/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents: 
 *		CGDIBitmap
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/GDIBitmap.hpp                                                 $
 * 
 * 4     8/30/98 4:50p Asouth
 * removed file scope
 * 
 * 3     2/11/98 4:35p Rwyatt
 * Can now draw with boolean operators
 * 
 * 2     1/09/98 7:10p Rwyatt
 * Can make DDBs from rasters that contain textures or bump maps. When a bump map is the source
 * just the texture information is used.
 * DDBs can now be individually clipped to a given rectangle
 * 
 * 1     12/22/97 12:00a Rwyatt
 * Initial implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_GUIAPP_GDIBITMAP_HPP
#define HEADER_GUIAPP_GDIBITMAP_HPP


//*********************************************************************************************
//
enum EDrawStatus
// prefix: eds
{
	edsNotClipped = 0,		// completely visible
	edsClipped,				// partly clipped
	edsNotVisible,			// completely clipped
};


//*********************************************************************************************
//
// CGDIBitmap definition
//
class CGDIBitmap
// prefix: gdibmp
{
public:
	//*****************************************************************************************
	CGDIBitmap
	(
		rptr<CRaster>	pras_src,
		int32			i4_width = -1,			// width of DIB or -1 for same as raster
		int32			i4_height = -1			// height of DIB or -1 for same as raster
	);

	//*****************************************************************************************
	~CGDIBitmap();

	//*****************************************************************************************
	EDrawStatus Draw
	(
		HDC		hdc,
		int32	i4_x,
		int32	i4_y,
		bool	b_solid = true,
		bool	b_realize = true
	);

	//*****************************************************************************************
	void SetClip
	(
		int32	i4_x,
		int32	i4_y,
		int32	i4_w,
		int32	i4_h
	)
	{
		rcClip.left		= i4_x;
		rcClip.top		= i4_y;
		rcClip.right	= i4_x + i4_w;
		rcClip.bottom	= i4_y + i4_h;
	}

	//*****************************************************************************************
	static HPALETTE GetDibPalette(const BITMAPINFO* pbmi);

	//*****************************************************************************************
	// Class data members
	//
	HBITMAP			hddbDeviceMap;
	HPALETTE		hpalColours;
	int32			i4Width;
	int32			i4Height;
	rptr<CRaster>	prasSource;
	RECT			rcClip;
};

#endif //#ifndef HEADER_GUIAPP_GDIBITMAP_HPP
