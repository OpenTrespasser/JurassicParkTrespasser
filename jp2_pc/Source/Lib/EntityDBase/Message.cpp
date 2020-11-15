/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Message.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Message.cpp                                           $
 * 
 * 23    98/08/29 18:14 Speter
 * Added OVERLAP flag to stats, and hid them anyway.
 * 
 * 22    98/08/28 12:03 Speter
 * Put proper epfOVERLAP flags on stats.
 * 
 * 21    8/25/98 2:46p Rvande
 * Fixed loop scope bug
 * 
 * 20    98/08/13 17:04 Speter
 * Added stats for broadcast vs. subscribed messages.
 * 
 * 19    8/11/98 8:31p Mlange
 * Implemented message-entity registration scheme.
 * 
 * 18    98/07/14 20:14 Speter
 * Disabled messages stat, as it was confusing.
 * 
 * 17    98/06/02 23:32 Speter
 * Added messaging stat.
 * 
 * 16    12/02/97 7:52a Shernd
 * Stopped writing of files that weren't necessary
 * 
 * 15    4/09/97 11:33a Rwyatt
 * Bases messages are not recorded in the replay file
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "Message.hpp"

#include "WorldDBase.hpp"
#include "Query/QMessage.hpp"
#include "MessageLog.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/EntityDBase/Entity.hpp"


//**********************************************************************************************
//
// CMessage implementation.
//
	bool CMessage::bIgnoreRegisteredRecipients = false;

	CProfileStat	psMsgBroadcast("(Msg Broadcast)", &proProfile.psStep, Set(epfOVERLAP) + epfHIDDEN),
					psMsgSubscribe("(Msg Subscribe)", &proProfile.psStep, Set(epfOVERLAP) + epfHIDDEN);

	//******************************************************************************************
	void CMessage::Send() const
	{

		// This base class does not have an implementation of ExtractReplayData because it would
		// no purpose because we have no idea what the message is.
		// Any message that is required to be accurately recorded in the reply file has to
		// provide an implementation of the Send member so that the correct data can be
		// extracted and saved. The Send Member needs to call ExtractReplyData which must also
		// be implemented for that particular message.

		CCycleTimer ctmr;

		// Write message to log.
		#if VER_LOG_MESSAGES
			conMessageLog.Print("Sending: ");
			WriteToLog();
		#endif

		// Deliver the message.
		if (petGetRecipient() != 0)
		{
			// Dispatch this message to the single recipient.
			DeliverTo(petGetRecipient());
			psMsgSubscribe.Add(ctmr(), 1);
		}
		else if (prcRecipients != 0 && !CMessage::bIgnoreRegisteredRecipients)
		{
			// Dispatch this message to the registered recipients.
			for (TRecipientsContainer::iterator it = prcRecipients->begin(); it != prcRecipients->end(); ++it)
				DeliverTo(*it);
			psMsgSubscribe.Add(ctmr(), prcRecipients->size());
		}
		else
		{
			// Dispatch this message to all the recipients.
			CWDbQueryActiveEntities wqaet;
			for (; wqaet.bIsNotEnd(); wqaet++)
				DeliverTo(wqaet.tGet());
			psMsgBroadcast.Add(ctmr(), wqaet.size());
		}
	}



#if VER_LOG_MESSAGES

	//******************************************************************************************
	void CMessage::WriteToLog() const
	{
        if (!bUseOutputFiles)
        {
            return;
        }

		// Write the name.
		conMessageLog.Print(strName());

		// Write additional data if there is any.
		const char* str_data = strData();

		if (str_data)
		{
			conMessageLog.Print(": ");
			conMessageLog.Print(str_data);
		}

		conMessageLog.Print("\n");
	}

//#if VER_LOG_MESSAGES
#endif
