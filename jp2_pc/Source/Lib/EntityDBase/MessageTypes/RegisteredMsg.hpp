/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp                        $
 * 
 * 3     8/21/98 2:22p Mlange
 * Additional messaging stat printing.
 * 
 * 2     8/12/98 1:53p Mlange
 * Now skips registration if the recipient is already registered. Added functions for accessing
 * the number of registered recipients and for purging all recipients for a message type.
 * 
 * 1     8/11/98 8:32p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_REGISTEREDMSG_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_REGISTEREDMSG_HPP

#include <list.h>

class CEntity;



// Prefix: rc
typedef list<CEntity*> TRecipientsContainer;

struct SRecipientsInfo
// Prefix: ri
{
	TRecipientsContainer rcRecipients;
	int iCreateCount;

	SRecipientsInfo()
		: iCreateCount(0)
	{
	}
};


//**********************************************************************************************
//
template<class T_DERIVED> class CRegisteredMessage
//
//
// Prefix: rmsg
//
// Notes:
//		Each class inherited from this must declare a static data member named 'riInfo' of
//		type 'SRecipientsInfo'. This static data member must be accessable by this base class.
//
// Example:
//		class CFoo : public CRegisteredMessage<CFoo>
//		{
//			friend class CRegisteredMessage<CFoo>;
//			static SRecipientsInfo riInfo;
//		};
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CRegisteredMessage()
	{
		++T_DERIVED::riInfo.iCreateCount;
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	static void RegisterRecipient
	(
		CEntity* pet
	)
	//
	// Add the given entity to the list of recipients for this message type.
	//
	//**********************************
	{
		// Do nothing if already registered.
		if (!bIsRegistered(pet))
			T_DERIVED::riInfo.rcRecipients.push_back(pet);
	}

	//******************************************************************************************
	//
	static void UnregisterRecipient
	(
		CEntity* pet
	)
	//
	// Remove the given entity from the list of recipients for this message type.
	//
	//**********************************
	{
		// Do nothing if not registered.
		if (bIsRegistered(pet))
			T_DERIVED::riInfo.rcRecipients.remove(pet);
	}

	//******************************************************************************************
	//
	static void UnregisterAll()
	//
	// Clear out the list of recipients for this message type.
	//
	//**********************************
	{
		T_DERIVED::riInfo.rcRecipients.erase(T_DERIVED::riInfo.rcRecipients.begin(), T_DERIVED::riInfo.rcRecipients.end());
	}

	//******************************************************************************************
	//
	static int iGetNumRegisteredRecipients()
	//
	// Returns:
	//		The number of recipients that are registered with this message type.
	//
	//**********************************
	{
		return T_DERIVED::riInfo.rcRecipients.size();
	}

	//******************************************************************************************
	//
	static bool bIsRegistered
	(
		CEntity* pet
	)
	//
	// Returns:
	//		'true' if the given entity is registered as a recipient for this message type.
	//
	//**********************************
	{
		for (TRecipientsContainer::iterator it = T_DERIVED::riInfo.rcRecipients.begin(); it != T_DERIVED::riInfo.rcRecipients.end(); ++it)
			if ((*it) == pet)
				return true;

		return false;
	}

	//******************************************************************************************
	//
	static int iGetNumCreated()
	//
	// Returns:
	//		The number of messages created of this type since the last time this function was
	//		called.
	//
	//**********************************
	{
		int i_ret = T_DERIVED::riInfo.iCreateCount;

		T_DERIVED::riInfo.iCreateCount = 0;

		return i_ret;
	}

};

#endif
