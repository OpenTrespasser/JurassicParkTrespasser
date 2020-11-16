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
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes/MsgControl.hpp                           $
 * 
 * 9     8/21/98 2:22p Mlange
 * Additional messaging stat printing.
 * 
 * 8     8/13/98 7:15p Mlange
 * Control messages now require registration.
 * 
 * 7     98/03/31 17:08 Speter
 * Updated for Control changes. CMessageControl now has SInput as a base. Removed unused
 * CMessageInput.
 * 
 * 6     97/04/15 6:35p Pkeet
 * Changed new raster message to use settings values instead of raster values.
 * 
 * 5     97/04/15 5:37p Pkeet
 * Added the 'CMessageNewRaster' class.
 * 
 * 4     4/09/97 11:25p Rwyatt
 * fixed typo (sorry)
 * 
 **********************************************************************************************/
 
#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGCONTROL_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGCONTROL_HPP

#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp"
#include "Lib/EntityDBase/QueueMessage.hpp"
#include "Lib/Control/Control.hpp"


//**********************************************************************************************
//
class CMessageControl : public CMessage, public SInput, public CRegisteredMessage<CMessageControl>
//
// Prefix: mc
//
// Message for user control.
//
//**************************************
{
	friend class CRegisteredMessage<CMessageControl>;
	static SRecipientsInfo riInfo;

public:
	CMessageControl(const SInput& in)
		: SInput(in), CMessage(&riInfo.rcRecipients)
	{}

protected:
	//******************************************************************************************
	//
	// Overrides.
	//

	void DeliverTo(CEntity* pet) const { pet->Process(*this); }
	const char* strName() const { return "Control"; }

	//******************************************************************************************
	//
	virtual void Send() const;
	//
	// Send this message.
	//
	// Notes:
	//		This function will invoke the Deliver() function either for just the single
	//		recipient or for all recipients.
	//


	//******************************************************************************************
	//
	virtual void ExtractReplayData() const;
	//
	// Extract replay data specific to this message
	//
	//**************************************


	//******************************************************************************************
	//
	// we need to be a friend with the replay class because it can fire off messages while
	// playing back a replay.
	//
	// all mesages that are recorded should in a replay be friends with CReplay
	//
	friend class CReplay;
};


//**********************************************************************************************
//
class CMessageNewRaster : public CMessage
//
// Indicates a change in raster format.
//
// Prefix: msgnewr
//
// Notes:
//
//**************************************
{
public:
	int  iMode;			// Raster mode.
	bool bSystemMem;	// System memory flag.
	bool bForce;		// Force flag.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMessageNewRaster(int i_mode, bool b_system_mem, bool b_force)
		: iMode(i_mode), bSystemMem(b_system_mem), bForce(b_force)
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
		return "NewRaster";
	}

	//******************************************************************************************
	//
	virtual void Send() const;
	//
	// Send this message.
	//
	// Notes:
	//		The Input message is only a temp. message to allow access to the Windows controls.
	//		In a later version this may be removed
	//


	//******************************************************************************************
	//
	virtual void ExtractReplayData() const;
	//
	// Extract replay data specific to this message
	//
	//**************************************

	//******************************************************************************************
	//
	// we need to be a friend with the replay class because it can fire off messages while
	// playing back a replay.
	//
	// all mesages that are recorded should in a replay be friends with CReplay
	//
	friend class CReplay;
};

#endif
