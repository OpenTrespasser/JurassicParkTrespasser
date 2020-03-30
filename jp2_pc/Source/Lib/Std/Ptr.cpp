/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Ptr.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/Ptr.cpp                                                       $
 * 
 * 12    9/08/98 8:55p Rwyatt
 * Removed the dump of remaining rptrs, some of them will be in deleted fast heaps which causes
 * a crash.
 * 
 * 11    8/27/98 9:32p Asouth
 * #ifdef for different STL implementations
 * 
 * 10    97/10/13 16:17 Speter
 * Commented out debug code that executed a little too often.
 * 
 * 9     97/06/23 20:29 Speter
 * Made CRefObj::uRefs public; rptr classes now manipulate uRefs inline, rather than via funcs,
 * hopefully improving inlining.  Removed CRefPtr base class, moved functionality directly into
 * rptr_const.
 * Replaced CNullObj* with SNullObj variable for null object; it is now initialised staticly,
 * and not destroyed, fixing an obscure memory leak at program end (and now comparisions of
 * rptrs against Null are a bit faster).
 * In special init functions, changed bool param to special enums in overloaded functions,
 * avoiding comparison, and making it a little clearer.
 * 
 * 8     97-05-09 13:46 Speter
 * Converted typeid().name() to strTypeName() calls.
 * 
 * 7     97-05-06 15:13 Speter
 * Now check for pNullObj in CheckPointer() function, rather than inserting pNullObj in the
 * pointer track db.
 * 
 * 6     97/03/03 15:47 Speter
 * CheckPointer() was not evaluating the result of find() correctly, and therefore wasn't
 * catching errors.  CNullObj() wasn't being inserted into CheckPointer() database.
 * 
 * 5     97/02/12 13:11 Speter
 * Compiled out rptr messages with new flag.
 * 
 * 4     97/01/26 19:56 Speter
 * Changed rptrs to point to a special null object rather than 0 when null.  This eliminates a
 * test for 0 when reference counting.
 * 
 * 3     97/01/07 11:06 Speter
 * Changed pSetTracker from ptr<> to raw pointer.  Added comments.
 * 
 * 2     97/01/02 16:36 Speter
 * Moved rptr tracking functions to .cpp file.
 * Added many comments.
 * 
 * 1     96/12/31 17:31 Speter
 * New file to implement aspects of reference tracking.
 * 
 ***********************************************************************************************/



#include "Common.hpp"
#include "Ptr.hpp"
#include "StringEx.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#if VER_TRACK_RPTR

#define bVER_SHOW_RPTR	0
// Flag which displays rptrs as they're added and deleted.  Useful to track down undeleted rptrs,
// but boy is it slow.  Undeleted rptrs are shown regardless of this flag.

#ifdef __MWERKS__
 #include <set.h>
#else
 #include <set>
#endif

//
// Private variables.
//

typedef std::set<CRefObj*, std::less<CRefObj*> > TSetTracker;

//
// Note: I would like to use ptr<TSetTracker> rather than TSetTracker*.  This does not work
// with a pointer init function, because the compiler will init scalar types (including raw
// pointers) to 0 before initing any objects which have constructors.  So a raw pointer will
// be init to 0 before CInitPtrTracker() is called, but a ptr<> might not be.
//
static TSetTracker* pSetTracker;
static uint uRefs = 0;

//**********************************************************************************************
//
// class CInitPtrTracker implementation.
//

	CInitPtrTracker::CInitPtrTracker()
	{
		if (!pSetTracker)
			pSetTracker = new TSetTracker;
		uRefs++;
	}

	CInitPtrTracker::~CInitPtrTracker()
	{
		Assert(pSetTracker);
		Assert(uRefs > 0);
		if (--uRefs == 0)
		{
			// Report currently allocated items.
			dprintf("%d rptrs not deleted: \n", pSetTracker->size());
/*			for (TSetTracker::iterator it = pSetTracker->begin(); it != pSetTracker->end(); it++)
			{
				dprintf("  %8X (%d) %s\n", *it, (*it)->uNumRefs(), strTypeName(**it));
			}*/
	//		Assert(pSetTracker->empty());
			delete pSetTracker;
			pSetTracker = 0;
		}
	}

//**********************************************************************************************
//
// class CRefObj implementation.
//

	//
	// Debug versions of reference-tracking functions, utilising database.
	//

	//******************************************************************************************
	void CRefObj::CheckNewPointer()
	{
		// Insert the new member in the database, and ensure it wasn't there before.
		Assert(uRefs == 0);
		Assert(pSetTracker);
		std::pair<TSetTracker::iterator, bool> pib = pSetTracker->insert(this);
		Assert(pib.second == true);
#if bVER_SHOW_RPTR
		dprintf("  rptr add %8X %s\n", this, strTypeName(*this));
#endif

		// Here is also a good place to add an Assert that our SNullObj hack is valid.
		Assert(&pNullObj->uRefs == &NullObj.uRefs);
	}

	//******************************************************************************************
	void CRefObj::CheckExistingPointer()
	{
/*
		TOO SLOW.
		// Ensure the pointer is already in the database.
		if (this == pNullObj)
			return;
		Assert(uRefs != 0);
		Assert(pSetTracker);
		Assert(pSetTracker->find(this) != pSetTracker->end());
*/
	}

	//******************************************************************************************
	void CRefObj::RemovePointer()
	{
		// Remove from the set, and Assert it was there.
		Assert(pSetTracker);
		Verify(pSetTracker->erase(this) == 1);
#if bVER_SHOW_RPTR
		dprintf("  rptr remove %8X %s\n", this, strTypeName(*this));
#endif
	}

#endif

//**********************************************************************************************
//
// Global NullObj initialisation.
//

// Set refs to a huge amount so it never gets deleted.
SNullObj NullObj = {0, TypeMax(uint) / 2};
