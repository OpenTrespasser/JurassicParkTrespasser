/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Definition CTextOverlay
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/TextOverlay.hpp                                      $
 * 
 * 5     9/30/98 2:13a Rwyatt
 * New members
 * 
 * 4     9/29/98 12:59a Rwyatt
 * Added system message
 * 
 * 3     9/23/98 5:52p Rwyatt
 * Added a type field so we can distinguish between tutorial and subtitles
 * 
 * 2     9/18/98 2:04a Rwyatt
 * Added a extern definition for a function to lookup a resource
 * 
 * 1     9/17/98 2:50p Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/

#ifndef HEADER_ENTITYDBASE_OVERLAYTEXT_HPP
#define HEADER_ENTITYDBASE_OVERLAYTEXT_HPP

#include <list.h>
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/EntityDBase/Subsystem.hpp"
#include "Lib/W95/DDFont.hpp"
#include "Lib/View/Colour.hpp"

extern uint32 u4LookupResourceString(int32 id,char* str_buf,uint32 u4_buf_len);

//**********************************************************************************************
#define TEXT_FORMAT_RAW		0x00000100
#define TEXT_FORMAT_SOLID	0x00000200
#define TEXT_FORMAT_IGNORE	0x00000400

//**********************************************************************************************
enum ETextType
// prefix: ett
{
	ettALL,
	ettUNKNOWN,
	ettSUBTITLE,
	ettTUTORIAL
};

//**********************************************************************************************
// prefix: tel
struct STextElement
{
	TSec	sRemove;
	union
	{
		uint8*		pu1FormattedData;			// either a pointer to formatted data
		char*		strString;					// or a bare string
	};
	uint32			u4XPos;
	uint32			u4YPos;
	CColour			clrText;
	uint32			u4Flags;
	STextElement*	ptelNext;					// next piece of text in the sequence
	ETextType		ettType;					// type of this text line
};


//**********************************************************************************************
// prefix: ttl
typedef list<STextElement>		TTextList;


//*********************************************************************************************
//
class CTextOverlay : public CSubsystem
// prefix: tov
{
public:

	CDDFont*	pfntOverlayFont;
	TTextList	ttlTextItems;

	//*****************************************************************************************
	//
	// Constructors and destructors
	//
	CTextOverlay();
	virtual ~CTextOverlay();

	//*****************************************************************************************
	//
	// Overrides
	//
	void Process(const CMessageStep& msg_step);
	void Process(const CMessagePaint& msgpaint);
	void Process(const CMessageSystem& msg);

	
	//*****************************************************************************************
	uint32 u4DisplayFormattedString
	(
		char*		str_text,			// C style text string
		TSec		s_time,				// display time
		uint32		u4_flags,			// formatting flags
		CColour		clr,				// colour of the text
		uint32		u4_prev = 0,
		ETextType	ett = ettUNKNOWN
	);

	//*****************************************************************************************
	uint32 u4DisplayPositionedString
	(
		char*		str_text,			// C style text string
		TSec		s_time,				// display time
		uint32		u4_xpos,			// screen x position
		uint32		u4_ypos,			// screen y position
		CColour		clr,				// colour
		uint32		u4_prev = 0,
		ETextType	ett = ettUNKNOWN
	);

	//*****************************************************************************************
	uint32 u4FindSequenceEnd
	(
		ETextType	ett
	);

	//*****************************************************************************************
	void RemoveText
	(
		ETextType	ett
	);

	//*****************************************************************************************
	void RemoveAll();

	//*****************************************************************************************
	void ResetScreen();

	//*****************************************************************************************
	// The global text system pointer.
	static CTextOverlay* ptovTextSystem;
};

extern CTextOverlay* ptovTextSystem;


#endif