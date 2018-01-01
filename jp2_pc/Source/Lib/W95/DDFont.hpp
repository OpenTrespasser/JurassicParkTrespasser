/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		CDDFont
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/DDFont.hpp                                                    $
 * 
 * 3     9/17/98 3:53p Rwyatt
 * Added a macro to make RGB values in windows format
 * 
 * 2     2/11/98 4:36p Rwyatt
 * Subtitles now use GDI so it works on all video and 3D cards.
 * 
 * 1     2/06/98 8:22p Rwyatt
 * Initial implementation
 * 
 ***********************************************************************************************/
#ifndef HEADER_LIB_W95_DDFONT_HPP
#define HEADER_LIB_W95_DDFONT_HPP

#include "Lib/View/Colour.hpp"

#define MAKERGB(r, g ,b)  ((DWORD) (((BYTE) (r) | ((WORD) (g) << 8)) | (((DWORD) (BYTE) (b)) << 16))) 

//**********************************************************************************************
//
#define TEXT_FORMAT_CENTER		0x00000001
#define TEXT_FORMAT_LEFT		0x00000002
#define TEXT_FORMAT_RIGHT		0x00000004
#define TEXT_FORMAT_TOP			0x00000008
#define TEXT_FORMAT_BOTTOM		0x00000010

#define TEXT_BORDER_X			8
#define TEXT_BORDER_Y			8


//**********************************************************************************************
//
struct SFormattedString
{
	 uint32		u4Size;
	 uint32		u4Lines;

	 //
	 // followed by u4Line  SformattedLine structures
	 //
};


//**********************************************************************************************
//
struct SFormattedLine
//prefix: fmtl
{
	uint32		u4XPos;
	uint32		u4YPos;
	uint32		u4TextLen;		// not including the NULL
	//
	// Followed by u4TextLen characters, followed by a 0
	//
};


//**********************************************************************************************
//
class CDDFont
// prefix: fnt
{
public:

	//******************************************************************************************
	//
	CDDFont
	(
		int32	i4_size
	);

	//******************************************************************************************
	//
	~CDDFont(void);


	//******************************************************************************************
	// returns the address after the NULL byte in the buffer passed in. This is in
	// effect the next address that can be used for another formatted string in the
	// same buffer.
	uint8* pu1FormatOverlayString
	(
		const char*	str_string,
		uint8*		str_buff,
		uint32		u4_flags
	);


	//******************************************************************************************
	//
	void PrintOverlayString
	(
		int32 i4_xp, 
		int32 i4_yp, 
		const char* str
	);


	//******************************************************************************************
	//
	void PrintFormattedOverlayString
	(
		uint8*	pu1_data
	);


	//******************************************************************************************
	uint32 u4FormattedStringBytes
	(
		uint8*	pu1_data
	)
	{
		// return the first DWORD...
		return *(uint32*)pu1_data;
	}

	//******************************************************************************************
	void SetFill
	(
		bool	b_fill
	)
	{
		bFill = b_fill;
	}

	//******************************************************************************************
	void SetColour
	(
		CColour	clr
	)
	{
		u4Colour = MAKERGB(clr.u1Red, clr.u1Green, clr.u1Blue);
	}

protected:
	void*			hfont;
	bool			bFill;
	uint32			u4Colour;			// same as a COLORREF not a CColour
	int32			i4YHeight;
	uint32			u4FontWidth[256];
};

#endif //HEADER_LIB_W95_DDFONT_HPP