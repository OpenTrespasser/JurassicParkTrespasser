/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *	CTextOverlay
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/TextOverlay.cpp                                       $
 * 
 * 8     10/07/98 4:00p Rwyatt
 * Subtitles are sequenced using real time and not simulation time which is clamped at 100ms per
 * frame
 * 
 * 7     9/30/98 2:14a Rwyatt
 * Font size changes with screen size
 * 
 * 6     9/29/98 1:00a Rwyatt
 * Removed pending text on a scene or groff load
 * 
 * 5     9/24/98 6:25p Rwyatt
 * Format buffer size is now calculated and allocated on the fly so is always big enough.
 * 
 * 4     9/23/98 5:52p Rwyatt
 * Added a type field so we can distinguish between tutorial and subtitles
 * 
 * 3     9/18/98 2:03a Rwyatt
 * Removed debug code
 * 
 * 2     98/09/17 18:38 Speter
 * Give it a unique name.
 * 
 * 1     9/17/98 2:48p Rwyatt
 * Initial implementation
 * 
 ***********************************************************************************************/

#include "common.hpp"
#include "Lib/W95\winInclude.hpp"
#include "TextOverlay.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/W95/DDFont.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPaint.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"


//**********************************************************************************************
// A static member pointing one of its own type, the global text system
CTextOverlay* CTextOverlay::ptovTextSystem = NULL;



//**********************************************************************************************
CTextOverlay::CTextOverlay
(
)
//*************************************
{
	Assert(ptovTextSystem == NULL);
	ptovTextSystem = this;

	pfntOverlayFont = new CDDFont(24);

	SetInstanceName("TextOverlay");

	// Register this entity with the message types it needs to receive.
	 CMessageStep::RegisterRecipient(this);
	CMessagePaint::RegisterRecipient(this);
   CMessageSystem::RegisterRecipient(this);
}



//**********************************************************************************************
CTextOverlay::~CTextOverlay
(
)
//*************************************
{
	// Unregister this entity with the message types it needs to receive.
	 CMessageStep::UnregisterRecipient(this);
	CMessagePaint::UnregisterRecipient(this);
   CMessageSystem::UnregisterRecipient(this);

	RemoveAll();

	delete pfntOverlayFont;
	ptovTextSystem = NULL;
}


#define u4MAX_FORMAT_BUFFER		1024

//**********************************************************************************************
uint32 CTextOverlay::u4DisplayFormattedString
(
	char*		str_text,			// C style text string
	TSec		s_time,				// display time
	uint32		u4_flags,			// formatting flags
	CColour		clr,				// colour
	uint32		u4_prev,			// previous line of text to attach to
	ETextType	ett					// type of the text
)
//*************************************
{
	STextElement	tel;
	uint8			pu1_buf[u4MAX_FORMAT_BUFFER];	// 1K local buffer on the stack

	// we cannot be a raw string
	u4_flags &= ~TEXT_FORMAT_RAW;

	tel.sRemove				= CMessageStep::sElapsedRealTime + s_time;
	tel.clrText				= clr;
	tel.u4Flags				= u4_flags;
	tel.ptelNext			= NULL;
	tel.ettType				= ett;


	//
	// Format the string in the local buffer and then allocate the memory for it.
	//
	pfntOverlayFont->pu1FormatOverlayString(str_text,pu1_buf,u4_flags);

	uint32 u4_bytes = pfntOverlayFont->u4FormattedStringBytes(pu1_buf);
	Assert(u4_bytes<u4MAX_FORMAT_BUFFER);

	tel.pu1FormattedData	= new uint8[ u4_bytes ];
	memcpy(tel.pu1FormattedData,pu1_buf,u4_bytes);


	// Add this item to the draw list
	ttlTextItems.push_front(tel);

	// get the address of the item we have just added
	STextElement*	ptel = &(*(ttlTextItems.begin()));

	if (u4_prev != 0)
	{
		// we are waiting for a previous text item to finish
		ptel->u4Flags |= TEXT_FORMAT_IGNORE;
		ptel->sRemove = s_time;					// store the display time
		STextElement*	ptel_next= (STextElement*)u4_prev;

		// Make sure that this text element does not already have a next pointer
		Assert(ptel_next->ptelNext == NULL);
		ptel_next->ptelNext = ptel;
	}

	return (uint32)ptel;
}


//*****************************************************************************************
uint32 CTextOverlay::u4DisplayPositionedString
(
	char*		str_text,			// C style text string
	TSec		s_time,				// display time
	uint32		u4_xpos,			// screen x position
	uint32		u4_ypos,			// screen y position
	CColour		clr,				// colour
	uint32		u4_prev,			// previous line to attach to
	ETextType	ett				// type of the text
)
//*************************************
{
	STextElement	tel;

	tel.sRemove		= CMessageStep::sElapsedRealTime + s_time;
	tel.strString	= new char[ strlen(str_text)+1 ];
	strcpy(tel.strString, str_text);
	tel.u4XPos		= u4_xpos;
	tel.u4YPos		= u4_ypos;
	tel.clrText		= clr;
	tel.u4Flags		= TEXT_FORMAT_RAW;
	tel.ptelNext	= NULL;
	tel.ettType		= ett;

	// Add this item to the draw list
	ttlTextItems.push_front(tel);

	// get the address of the item we have just added
	STextElement*	ptel = &(*(ttlTextItems.begin()));

	if (u4_prev != 0)
	{
		// we are waiting for a previous text item to finish
		ptel->u4Flags |= TEXT_FORMAT_IGNORE;
		ptel->sRemove = s_time;
		STextElement*	ptel_next= (STextElement*)u4_prev;

		// Make sure that this text element does not already have a next pointer
		Assert(ptel_next->ptelNext == NULL);
		ptel_next->ptelNext = ptel;
	}

	return (uint32)ptel;
}


//**********************************************************************************************
void CTextOverlay::Process
(
	const CMessageSystem& msg_system
)
//*************************************
{
	if ((msg_system.escCode == escGROFF_LOADED) || (msg_system.escCode == escSCENE_FILE_LOADED))
	{
		RemoveAll();
	}
}


//**********************************************************************************************
void CTextOverlay::Process
(
	const CMessageStep& msg_step
)
//*************************************
{
	bool b_done = false;

	while (!b_done)
	{
		for (TTextList::iterator i = ttlTextItems.begin(); i != ttlTextItems.end(); ++i)
		{
			if ( (*i).u4Flags & TEXT_FORMAT_IGNORE)
				continue;

			if (CMessageStep::sElapsedRealTime >=(*i).sRemove)
			{
				if ( (*i).ptelNext)
				{
					// This element has a next pointer..
					(*i).ptelNext->u4Flags &=~TEXT_FORMAT_IGNORE;		// make visible
					(*i).ptelNext->sRemove+=CMessageStep::sElapsedRealTime;	// set the remove time
				}
				delete (void*) ((*i).strString);		// delete the memory as a naked pointer
				ttlTextItems.erase(i);
				break;
			}
		}
		b_done = true;
	}
}



//**********************************************************************************************
void CTextOverlay::Process
(
	const CMessagePaint& msgpaint
)
//*************************************
{
	for (TTextList::iterator i = ttlTextItems.begin(); i != ttlTextItems.end(); ++i)
	{
		// Is the ignore flag set??
		if ( (*i).u4Flags & TEXT_FORMAT_IGNORE)
			continue;

		pfntOverlayFont->SetFill( ((*i).u4Flags & TEXT_FORMAT_SOLID) );
		pfntOverlayFont->SetColour( (*i).clrText );

		if ((*i).u4Flags & TEXT_FORMAT_RAW)
		{
			// print the text at the specified position
			pfntOverlayFont->PrintOverlayString(  (int32)((*i).u4XPos),(int32)((*i).u4YPos), (*i).strString);
		}
		else
		{
			// print the formatted string
			pfntOverlayFont->PrintFormattedOverlayString( (*i).pu1FormattedData );
		}
	}
}


//**********************************************************************************************
// This will return the handle of the earliest sequence in the list, this is the one you should
// attach to if you want your test to appear in order.
uint32 CTextOverlay::u4FindSequenceEnd
(
	ETextType	ett
)
//*************************************
{
	for (TTextList::iterator i = ttlTextItems.begin(); i != ttlTextItems.end(); ++i)
	{
		if ( (*i).ettType == ett)
			return (uint32)&(*(i));
	}

	return 0;
}


//**********************************************************************************************
// This will return the handle of the earliest sequence in the list, this is the one you should
// attach to if you want your test to appear in order.
void CTextOverlay::RemoveText
(
	ETextType	ett
)
//*************************************
{
	if (ett == ettALL)
	{
		ttlTextItems.erase(ttlTextItems.begin(),ttlTextItems.end());
	}
	else
	{
		bool b_done = false;

		while (!b_done)
		{
			b_done = true;
			for (TTextList::iterator i = ttlTextItems.begin(); i != ttlTextItems.end(); ++i)
			{
				if ( (*i).ettType == ett)
				{
					ttlTextItems.erase(i);
					b_done = false;
					break;
				}
			}
		}
	}
}


//**********************************************************************************************
void CTextOverlay::RemoveAll
(
)
//*************************************
{
	// tidy up the memory used by the list but do not free the list
	for (TTextList::iterator i = ttlTextItems.begin(); i != ttlTextItems.end(); ++i)
	{
		// delete the memory as a naked pointer
		delete (void*) ((*i).strString);
	}

	// erase the actual items items in the list
	ttlTextItems.erase(ttlTextItems.begin(),ttlTextItems.end());
}


//**********************************************************************************************
void CTextOverlay::ResetScreen
(
)
//*************************************
{
	RemoveAll();

	delete pfntOverlayFont;

	uint32 u4_size = prasMainScreen->iWidth/20;

	if (u4_size>32)
		u4_size = 32;

	if (u4_size<12)
		u4_size = 12;

	pfntOverlayFont = new CDDFont(u4_size);
}