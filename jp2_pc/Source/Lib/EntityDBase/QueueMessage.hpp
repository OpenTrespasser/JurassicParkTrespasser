/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Classes for queue'ing messages until the end of the current frame.
 *
 * Bugs:
 *
 * To do:
 *		Consider having the CQueueMessage class simply contain instances of the STL queue type
 *		instead of pointers to new'ed instances of that type.
 *
 *		When the compiler supports templated class member functions, implement the global
 *		overloaded << operator as a member function of the CQueueMessage class. Then it is also
 *		possible to make the queue member variables private to the class. They are now
 *		necessarily public for access by the global << operator function. It does not seem to be
 *		possible to declare templated functions as friends.
 *
 *		Implement a delete queue for messages about to be freed in a Flip()
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/QueueMessage.hpp                                      $
 * 
 * 11    9/30/98 9:36p Rwyatt
 * Added reset function
 * 
 * 10    8/25/98 2:47p Rvande
 * Fix required for MSL (instead of HP STL)
 * 
 * 9     8/25/98 11:35a Rvande
 * MSL defines queue in a separate file
 * 
 * 8     97/12/02 15:01 Speter
 * operator << now uses copy constructor rather than default constructor plus assign.
 * 
 * 7     97/09/11 20:54 Speter
 * .Play() now optionally takes a step time.  
 * 
 * 6     4/09/97 11:42a Rwyatt
 * Added default bool parameter to Play member, this
 * control if step messages are generated or not
 * 
 * 5     1/21/97 1:35p Agrant
 * updated todo list
 * 
 * 4     11/01/96 3:45p Agrant
 * Added include for FrameHeap.hpp
 * 
 * 3     96/10/24 4:30p Mlange
 * The overloaded << operator now takes a reference to the message to queue instead of a
 * pointer. Also, moved the body of this function from the cpp file to this header file.
 * 
 * 2     96/10/23 6:41p Mlange
 * Declared several things as const. Removed the CWrapMessage class and the PushNext() member
 * function. Replaced the PushNext function by an overloaded templated << operator. Added some
 * comments.
 * 
 * 1     10/21/96 4:12p Pkeet
 * Initial implementation.
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUEUEMESSAGE_HPP
#define HEADER_LIB_ENTITYDBASE_QUEUEMESSAGE_HPP


//
// Includes.
//
#ifdef __MWERKS__
#include <queue.h>
#endif
#include <deque.h>
#include <stack.h>

#include "FrameHeap.hpp"
#include "Lib/Sys/Timer.hpp"

//*********************************************************************************************
//
// Forward declaration for class CQueueMessage.
//
class CMessage;


//*********************************************************************************************
//
class CQueueMessage
//
// Maintains a queue of messages for the current and the next frame.
//
// Prefix: qm
//
//**************************************
{

public:
#ifdef __MWERKS__
	// MSL's queue is a slightly different implementation than HP STL
	queue< const CMessage* >* pdqwmCurrentMessages;			// Current frame's event message queue.
	queue< const CMessage* >* pdqwmNextMessages;			// Next frame's event message queue.
#else
	queue< deque<const CMessage*> >* pdqwmCurrentMessages;	// Current frame's event message queue.
	queue< deque<const CMessage*> >* pdqwmNextMessages;		// Next frame's event message queue.
#endif

public:

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	CQueueMessage();

	~CQueueMessage();


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Flip
	(
	);
	//
	// Swaps the current message queue with the next message queue. Also performs a flip on the
	// FrameHeap.
	//
	//**************************************


	//*****************************************************************************************
	//
	void Reset
	(
	);
	//
	// Empties and resets the message queue.
	//
	//**************************************

	
	//*****************************************************************************************
	//
	void Play
	(
		bool b_step = true,					// Defaults to true, set to false if no step is required.
		TSec s_step = -1.0					// Amount to step; or default for real-time.
	) const;
	//
	// Dispatches all messages in the current message queue.
	//
	//**************************************
};


//*****************************************************************************************
//
template<class T_MSG> inline void operator <<
(
	CQueueMessage& qm,
	const T_MSG& msg
)
//
// Places a message at the end of the next frame's message queue.
//
// Notes:
//		The message is copied and placed on the current frame's heap.
//
//**************************************
{
	// Allocate and copy the message onto the frameheap.
	T_MSG* pmsg_new = new(frhFrameHeap) T_MSG(msg);

	qm.pdqwmNextMessages->push(pmsg_new);
}




//
// External declarations.
//

// Object to maintain event message queues for the current and the next frame.
extern CQueueMessage qmQueueMessage;

#endif
