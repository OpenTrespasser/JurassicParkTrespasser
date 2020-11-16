/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		Implementation of CDDFont
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/DDFont.cpp                                                    $
 * 
 * 4     9/30/98 2:11a Rwyatt
 * Added 3D effect to text to help contrast
 * 
 * 3     8/21/98 5:00p Rwyatt
 * Now centers subtitles properly on a reduced screen size
 * 
 * 2     2/11/98 4:36p Rwyatt
 * Subtitles now use GDI so it works on all video and 3D cards.
 * 
 * 1     2/06/98 8:22p Rwyatt
 * Initial implementation
 * 
 ***********************************************************************************************/

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/Sys/Profile.hpp"
#include "DDFont.hpp"


extern rptr<CRasterWin> prasMainScreen;


//**********************************************************************************************
//
CDDFont::CDDFont
(
	int32	i4_size
)
//*************************************
{
	uint8	u1;

	u4Colour = 0x00ffffff;
	bFill = false;

	// create the Win32 Font
	hfont = CreateFont(i4_size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
						ANSI_CHARSET,
						OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY,   // NONANTIALIASED_QUALITY - Undocumented
						VARIABLE_PITCH,
						"Arial" );

	// get a DC for the screen
	HDC	hdc_temp = GetDC(NULL);
	Assert(hdc_temp);
	
	// select our selected font into the screen DC
	void* hfont_temp_old = SelectObject(hdc_temp,hfont);

	SIZE	size_ch;

	i4YHeight = 0;

	for (uint32 u4_font_char = 32; u4_font_char<255; u4_font_char++)
	{
		u1 = (uint8)u4_font_char;

		// get the size of the current character
		GetTextExtentPoint(hdc_temp, (char*)&u1, 1, &size_ch);

		u4FontWidth[u4_font_char] = size_ch.cx;

		// get the height of the biggest character...
		if (size_ch.cy>i4YHeight)
		{
			i4YHeight = size_ch.cy;
		}
	}

	// tidy up the DC for the screen
	SelectObject(hdc_temp, hfont_temp_old);

	// release the DC for the screen
	ReleaseDC(NULL,hdc_temp);
}


//**********************************************************************************************
//
CDDFont::~CDDFont
(
)
//*************************************
{
	DeleteObject(hfont);
}


//**********************************************************************************************
//
void CDDFont::PrintOverlayString
(
	int32 i4_xp, 
	int32 i4_yp, 
	const char* str
)
//*************************************
{
	CCycleTimer ctmr;

	prasMainScreen->Unlock();

	HDC	hdc = prasMainScreen->hdcGet();

	SetTextColor(hdc, u4Colour );
	if (bFill)
	{
		SetBkColor(hdc,0);		
		//
		// We must set the background back to opaque as a DD surface has a unique DC whcih is
		// returned every time you ask for a DC (similar to the OWNDC flag in windows). If
		// you cange the state of the DC it remains across DC calls.
		//
		SetBkMode(hdc, OPAQUE);
	}
	else
	{
		SetBkMode(hdc, TRANSPARENT);
	}

	void* hfont_old = SelectObject(hdc,hfont);
	Assert(hfont_old);

	TextOut(hdc,i4_xp,i4_yp,str,strlen(str));

	SelectObject(hdc,hfont_old);

	prasMainScreen->ReleaseDC(hdc);

	proProfile.psOverlayText.Add(ctmr());
}




//**********************************************************************************************
//
uint8* CDDFont::pu1FormatOverlayString
(
	const char*	str_string,
	uint8*		str_buff,
	uint32		u4_flags
)
//*************************************
{
	char*	str = (char*)str_buff;
	int32*	pi4_lines;
	int32*	pi4_bytes;

	SFormattedLine*	apfmtl_lines[32];
	SFormattedLine* pfmtl;
	int32	i4_count;

	// width and height of the screen area we are going to draw to...
	int32	i4_scr_width	= prasMainScreen->iWidth - (TEXT_BORDER_X*2);
	int32	i4_x_size;
	int32	i4_y_pos = TEXT_BORDER_Y;// - prasMainScreen->iOffsetY;
	char	ch;
	int32	i4_lines = 0;

	char*	str_dst_break;
	char*	str_src_break;
	bool	b_break;
	int32	i4_break_size;
	int32	i4_break_count;

	pi4_bytes = (int32*)(str);
	str += sizeof(int32);

	pi4_lines = (int32*)(str);
	str += sizeof(int32);

	while (*str_string)
	{
		pfmtl = (SFormattedLine*)str;
		str+=sizeof(SFormattedLine);

		i4_x_size = 0;

		b_break		  = false;
		str_dst_break = NULL;
		str_src_break = NULL;
		i4_break_size = 0;
		i4_count = 0;
		i4_break_count = 0;

		while (*str_string)
		{
			ch = *str_string;

			if (ch==13)
			{
				str_string++;
				break;
			}

			if (i4_x_size+u4FontWidth[ch]>=i4_scr_width)
			{
				break;
			}

			i4_x_size += u4FontWidth[ch];

			*str = ch;
			i4_count ++;

			if (*str == ' ' || *str == ',' || *str == ';' || *str == ':' || *str == '-')
			{
				str_dst_break = (char*)str+1;
				str_src_break = (char*)str_string+1;
				i4_break_size = i4_x_size;
				i4_break_count = i4_count;
				b_break = true;
			}
			else
			{
				b_break = false;
			}

			str++;
			str_string++;
		}

		if (b_break || (str_dst_break == NULL) || (*str_string == NULL))
		{
			// the last charatcer we emmited was a character suitable for a break character,
			// terminate the current string
			*str = 0;
			*str++;
		}
		else
		{
			// we must go back to the last break position.
			*str_dst_break = 0;
			str = str_dst_break+1;
			str_string = str_src_break;
			i4_x_size = i4_break_size;
			i4_count = i4_break_count;
		}

		//
		// fill in the position formatting the string in the correct way
		//
		if (u4_flags & TEXT_FORMAT_LEFT)
		{
			pfmtl->u4XPos = TEXT_BORDER_X + prasMainScreen->iOffsetX;
		}
		else if (u4_flags & TEXT_FORMAT_CENTER)
		{
			pfmtl->u4XPos = TEXT_BORDER_X + prasMainScreen->iOffsetX + (i4_scr_width - i4_x_size)/2;
		}
		else if (u4_flags & TEXT_FORMAT_RIGHT)
		{
			pfmtl->u4XPos = prasMainScreen->iWidth - i4_x_size - TEXT_BORDER_X + prasMainScreen->iOffsetX;
		}
		else
		{
			Assert(0);
		}

		pfmtl->u4TextLen = (uint32)i4_count;
		pfmtl->u4YPos = i4_y_pos + prasMainScreen->iOffsetY;
		apfmtl_lines[i4_lines] = pfmtl;
		i4_lines++;
		i4_y_pos+=i4YHeight;
	}

	// if we are formatting at the bottom of the screen, move all the subtitles down..
	if (u4_flags & TEXT_FORMAT_BOTTOM)
	{
		// text_y_pos is the bottom of the last scan line drawn....
		// adjust all the Y positions
		for (int i = 0; i<i4_lines; i++)
		{
			apfmtl_lines[i]->u4YPos += (prasMainScreen->iHeight-i4_y_pos - TEXT_BORDER_Y);
		}
	}

	// fill in the number of lines
	*pi4_lines = i4_lines;

	// number of bytes taken is the difference between the start and end pointers
	*pi4_bytes = (int32)((uint32)str-(uint32)str_buff);

	// return the current address as the next address that can be used...
	return (uint8*)str;
}




//**********************************************************************************************
//
void CDDFont::PrintFormattedOverlayString
(
	uint8*	pu1_data
)
//*************************************
{
	int32			i4_lines;
	SFormattedLine* pfmtl;

	CCycleTimer		ctmr;

	//skip the byte count dword - we do not need it.
	pu1_data+=sizeof(int32);

	// lines is the second dword
	i4_lines = *(int32*)(pu1_data);
	pu1_data+=sizeof(int32);


	//
	// Setup the device context for drawing to
	//
	prasMainScreen->Unlock();
	HDC	hdc = prasMainScreen->hdcGet();

	if (bFill)
	{
		SetBkColor(hdc,0);		
		SetBkMode(hdc, OPAQUE);
	}
	else
	{
		SetBkMode(hdc, TRANSPARENT);
	}

	void* hfont_old = SelectObject(hdc,hfont);
	Assert(hfont_old);

	//
	// After the string header is a formatted section..
	//
	while (i4_lines)
	{
		pfmtl = (SFormattedLine*)pu1_data;
		pu1_data+=sizeof(SFormattedLine);

		SetTextColor(hdc, 0);
		TextOut(hdc,pfmtl->u4XPos-1,pfmtl->u4YPos-1,(char*)pu1_data,pfmtl->u4TextLen);

		SetTextColor(hdc, u4Colour );
		TextOut(hdc,pfmtl->u4XPos,pfmtl->u4YPos,(char*)pu1_data,pfmtl->u4TextLen);

		// point to the next line header, skip the text len + 1 for the 0
		pu1_data+=pfmtl->u4TextLen+1;
		i4_lines--;
	}


	//
	// Put the DD device context back to how it was
	//
	SelectObject(hdc,hfont_old);
	prasMainScreen->ReleaseDC(hdc);

	proProfile.psOverlayText.Add(ctmr());
}
