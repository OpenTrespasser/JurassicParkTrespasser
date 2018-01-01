/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		CAudioSubtitle
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/Subtitle.hpp                                                $
 * 
 * 3     8/21/98 5:59p Rwyatt
 * Added an init flag to the subtitle class
 * 
 * 2     2/06/98 8:19p Rwyatt
 * CAU Subtitle control basics
 * 
 * 1     2/03/98 3:03p Rwyatt
 * Initial implementation
 *
 ***********************************************************************************************/

#ifndef HEADER_LIB_AUDIO_SUBTITLE
#define HEADER_LIB_AUDIO_SUBTITLE



//**********************************************************************************************
//
struct SSubtitleHeader
// prefix: sth
{
	uint32		u4SubtitleLen;			// length of the subtitle data block, NOT INCLUDING this DWORD
	uint32		u4SubtitleSections;

	//
	// Followed by data for the first section.
	//
};


//**********************************************************************************************
//
class CAudioSubtitle
{
public:

	//******************************************************************************************
	CAudioSubtitle
	(
		SSubtitleHeader* psth,				// subtitle header
		bool b_delete						// delete the memory on destruction?
	)
	//*********************************
	{
		bSetup			= false;
		bDeleteMemory	= b_delete;
		psthTitle		= psth;
	}


	//******************************************************************************************
	~CAudioSubtitle
	(
	)
	//*********************************
	{
		if (bDeleteMemory)
		{
			delete	psthTitle;
		}
	}

	//******************************************************************************************
	SSubtitleHeader* psthGetSubtitleData
	(
	)
	//*********************************
	{
		return psthTitle;
	}

	//******************************************************************************************
	bool bSubtitleSetup
	(
	)
	//*********************************
	{
		return bSetup;
	}

	//******************************************************************************************
	void SubtitleSetup
	(
	)
	//*********************************
	{
		bSetup = true;
	}

protected:
	bool				bSetup;
	bool				bDeleteMemory;
	SSubtitleHeader*	psthTitle;
};


#endif //HEADER_LIB_AUDIO_SUBTITLE