/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		The actual message types (derived from the base message classes defined in
 *		'Message.hpp') used for communication.
 *
 *		New message types will be added to this header file if and when they are required.
 *
 * Bugs:
 *
 * Notes:
 *		Queued messages are never deleted.  Their memory is just reclaimed.
 *		Thus, messages should not require fancy delete functions, nor should they include
 *		rptr's.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes/MsgPaint.hpp                             $
 * 
 * 7     8/21/98 2:22p Mlange
 * Additional messaging stat printing.
 * 
 * 6     8/13/98 6:08p Mlange
 * Now requires registration of recipients.
 * 
 * 5     98/03/06 10:54 Speter
 * Added CCamera& arg to CMessagePaint.
 * 
 * 4     5/30/97 11:05a Agrant
 * Goofy LINT fixes.
 * 
 * 3     97-03-28 16:36 Speter
 * Replaced rsrContext member with renContext.
 * 
 * 2     3/14/97 4:11p Mlange
 * Now contains a rendering context. Made the Send() function public.
 * 
 **********************************************************************************************/
 
#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGPAINT_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGPAINT_HPP

#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp"
#include "Lib/Renderer/Pipeline.hpp"

//**********************************************************************************************
//
class CMessagePaint : public CMessage, public CRegisteredMessage<CMessagePaint>
//
// The 'paint' message. This message type is sent to indicate that the screen needs to be
// updated.
//
// Prefix: msgpaint
//
// Notes:
//		The order in which this message is delivered to the subsystems determines the order in
//		which they update the screen. Currently, this order is determined by the iteration of
//		the subsystems in the entity database. This behaviour may be modified by overiding the
//		Dispatch() function for this class.
//
//**************************************
{
	friend class CRegisteredMessage<CMessagePaint>;
	static SRecipientsInfo riInfo;

public:
	CRenderer& renContext;				//lint !e1725  // The rendering context to use.
	CCamera& camCurrent;				// The camera specifying current view.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessagePaint(CRenderer& ren_context, CCamera& cam)
		: CMessage(&riInfo.rcRecipients), renContext(ren_context), camCurrent(cam)
	{
	}

protected:

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void DeliverTo(CEntity* pet) const
	{
		pet->Process(*this);
	}

	//******************************************************************************************
	virtual const char* strName() const
	{
		return "Paint";
	}

public:
	//******************************************************************************************
	//
	virtual void Send() const;
	//
	// Send this message.
	//
	// Notes:
	//		This function will invoke the Deliver() function for all subsystems.
	//
	//**************************************
};


#endif
