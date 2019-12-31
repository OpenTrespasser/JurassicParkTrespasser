/***********************************************************************************************
 *
 * Copyright (c) 1997 DreamWorks Interactive
 *
 * Contents: Implementation of the object handle class.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/ObjectHandle.cpp                                            $
 * 
 * 6     98.08.22 11:28p Mmouni
 * Change uWrite to be const.
 * 
 * 5     10/22/97 5:03p Agrant
 * Hack to allow version 12 files to load, for sure this time....
 * 
 * 4     10/22/97 2:14a Gstull
 * Allows version 12 GROFF files to load.
 * 
 * 3     5/27/97 3:16p Agrant
 * Fixed a returning reference to a local bug.
 * 
 * 2     5/20/97 3:04p Gstull
 * Added a new constructor to allow the ranges of object handles to be allocated.
 * 
 * 1     5/12/97 9:56p Gstull
 * Classes for object handle management.
 *
 **********************************************************************************************/

//
// Determine which set of standard types to use based upon the environment.  This is done to
// simplify the use of these classes within both the game and 3D Studio Max.
//

#ifdef USE_MAX_TYPES

#include "StandardTypes.hpp"
#include "EasyString.hpp"
#include "ObjectHandle.hpp"

#else

#include "Common.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "Lib/Groff/ObjectHandle.hpp"

#endif


// Setup the static variable to track the number of classes being managed by the handle manager.
uint			 	 CHandleManager::uClassCount = 0;

// Setup the static variable to track the base value of the handle allocator.
uint				 CHandleManager::uHandleBase = 0;

// Setup the STL list for tracking the object handle records for each registered handle class.
std::deque<CObjectHandle> CHandleManager::aobjhObjectHandle;


//******************************************************************************************
//
CHandle::CHandle
(
) : uHandleID(0), uReferenceCount(0)
{
}


//******************************************************************************************
//
CHandle::CHandle
(
	const uint u_handle
) : uHandleID(u_handle), uReferenceCount(1)
{
}


//******************************************************************************************
//
CHandle::CHandle
(
	const uint u_handle,
	const uint u_ref_count
) : uHandleID(u_handle), uReferenceCount(u_ref_count)
{
}


//******************************************************************************************
//
uint CHandle::uHandle
(
)
{
	// Return the current handle value.
	return uHandleID;
}


//******************************************************************************************
//
uint CHandle::uCount
(
)
{
	// Return the current reference count.
	return uReferenceCount;
}


//******************************************************************************************
//
bool CHandle::operator==
(
	const CHandle& h_handle
) const
{
	return uHandleID == h_handle.uHandleID;
}


//******************************************************************************************
//
bool CHandle::operator!=
(
	const CHandle& h_handle
) const
{
	return uHandleID != h_handle.uHandleID;
}


//******************************************************************************************
//
bool CHandle::operator<
(
	const CHandle& h_handle
) const
{
	return uHandleID < h_handle.uHandleID;
}


//******************************************************************************************
//
bool CHandle::operator<=
(
	const CHandle& h_handle
) const
{
	return uHandleID <= h_handle.uHandleID;
}


//******************************************************************************************
//
bool CHandle::operator>
(
	const CHandle& h_handle
) const
{
	return uHandleID > h_handle.uHandleID;
}


//******************************************************************************************
//
bool CHandle::operator>=
(
	const CHandle& h_handle
) const
{
	return uHandleID >= h_handle.uHandleID;
}


//******************************************************************************************
//
CHandle& CHandle::operator++
(
	int
)
{
	// Increment the reference count.
	uReferenceCount++;

	return *this;
}


//******************************************************************************************
//
CHandle& CHandle::operator--
(
	int
)
{
	// Increment the reference count.
	uReferenceCount--;

	return *this;
}


//******************************************************************************************
//
CHandle& CHandle::operator+=
(
	uint u_reference_count
)
{
	// Increment the reference count.
	uReferenceCount += u_reference_count;

	return *this;
}


//******************************************************************************************
//
CHandle& CHandle::operator-=
(
	uint u_reference_count
)
{
	// Increment the reference count.
	uReferenceCount -= u_reference_count;

	return *this;
}


//**********************************************************************************************
//
std::ostream& operator<<
(
	std::ostream&		os_stream, 
	const CHandle&	h_handle
)
{
	// Display the string.
	os_stream.setf(std::ios::uppercase|std::ios::hex);

#if 1
	os_stream << "h<" << h_handle.uHandleID << "#" << std::dec
		<< h_handle.uReferenceCount << ">" << std::flush;
#else
	cout << "CHandle(0x" << h_handle.uHandleID << ", " << dec << flush;
	cout << h_handle.uReferenceCount << ")" << flush;
		return os_stream;
#endif

	return os_stream;
}


//**********************************************************************************************
//
std::istream& operator>>
(
	std::istream& is_stream,
	CHandle& h_handle
)
{
	// Display the string.
	std::cout << "CHandle...\nHandle: " << std::flush;
	std::cin >> std::hex >> h_handle.uHandleID;
	std::cout << "Ref Count: " << std::flush;
	std::cin >> std::dec >> h_handle.uReferenceCount;

	return is_stream;
}


//******************************************************************************************
//
uint CHandle::uWriteCount
(
)
{
	// Return the number of bytes required to represent the string in memory.
	return sizeof(uint) + sizeof(uint);
}


//******************************************************************************************
//
uint CHandle::uWrite
(
	char** ppc_buffer
) const
{
	// Write the object into the buffer.
	uint* pu_value = (uint *) *ppc_buffer;

	// Write the information out to the buffer.
	pu_value[0] = uHandleID;
	pu_value[1] = uReferenceCount;

	// Increment the buffer pointer.
	*ppc_buffer += sizeof(uint) + sizeof(uint);

	// Return the number of bytes placed into the buffer.
	return sizeof(uint) + sizeof(uint);
}


//******************************************************************************************
//
uint CHandle::uRead
(
	char** ppc_buffer
)
{
	// Read the object into memory.
	uint* pu_value = (uint *) *ppc_buffer;
	
	// Read the information in from the buffer.
	uHandleID       = pu_value[0];
	uReferenceCount = pu_value[1];

	// Increment the buffer pointer.
	*ppc_buffer += sizeof(uint) + sizeof(uint);

	// Return the number of bytes placed into the buffer.
	return sizeof(uint) + sizeof(uint);
}


//******************************************************************************************
//
void CObjectHandle::Init
(
)
{
	// Set everything to NULL in case someone is looking at it.
	estrClassName = "";
	uHandleBase	  = 0;
	uNextHandle   = 0;
	uHandleCount  = 0;
	bInUse		  = false;
}


//******************************************************************************************
//
CObjectHandle::CObjectHandle
(
)
{
	// Set the handle to all initial values.
	Init();
}


//******************************************************************************************
//
CObjectHandle::CObjectHandle
(
	const CEasyString&	estr_classname,
	uint				u_handle_base
) : estrClassName(estr_classname), uHandleBase(u_handle_base), uNextHandle(u_handle_base), 
	uHandleCount(0), bInUse(true)
{
}


//******************************************************************************************
//
CObjectHandle::~CObjectHandle
(
) 
{
	// Deactivate this registered handle entry.
	Init();
}


//******************************************************************************************
//
CObjectHandle& CObjectHandle::operator=
(
	const CObjectHandle& hobj_handle
)
{
	// Are we trying to assign to ourself?
	if (this == &hobj_handle)
	{
		// Yes!  Then there is nothing to do.
		return *this;
	}
	
	estrClassName = hobj_handle.estrClassName;
	uHandleBase   = hobj_handle.uHandleBase;

	uNextHandle  = hobj_handle.uNextHandle;
	uHandleCount = hobj_handle.uHandleCount;

	// Pass this object to the next in line.
	return *this;
}


//**********************************************************************************************
//
std::ostream& operator<<
(
	std::ostream&			 os_stream, 
	const CObjectHandle& objh_handle
)
{
	// Display the string.
	os_stream.setf(std::ios::uppercase|std::ios::hex);
	os_stream << "CObjectHandle(\"" << objh_handle.estrClassName << "\", Base:" << std::hex
		<< objh_handle.uHandleBase << ", Next:" << objh_handle.uNextHandle << ", #" 
		<< std::dec << objh_handle.uHandleCount;

	// Is this handle in use?
	if (objh_handle.bInUse)
	{
		std::cout << ", Busy)";
	}
	else
	{
		std::cout << ", Free)";
	}

	// Return the stream.
	return os_stream;
}


//**********************************************************************************************
//
std::istream& operator>>
(
	std::istream&		is_stream,
	CObjectHandle&	objh_handle
)
{
	// Display the string.
	std::cout << "CObjectHandle...\n" << std::flush;
	std::cin >> objh_handle.estrClassName;

	std::cout << "Base: 0x" << std::flush;
	std::cin >> std::hex >> objh_handle.uHandleBase;

	std::cout << "Next: 0x" << std::flush;
	std::cin >> objh_handle.uNextHandle;

	std::cout << "Count: " << std::flush;
	std::cin >> std::dec >> objh_handle.uHandleCount;

	return is_stream;
}


//******************************************************************************************
//
CEasyString& CObjectHandle::estrName
(
)
{
	// Return the name of this handle entry.
	return estrClassName;
}


//******************************************************************************************
//
CHandle CObjectHandle::hNext
(
)
{
	// Return the next valid handle.
	return CHandle(uNextHandle++, 1);
}


//******************************************************************************************
//
uint CObjectHandle::uNextRange
(
)
{
	// Return the lasy valid handle.
	return uNextHandle + uGROUP_OFFSET & uGROUP_OFFSET-1;
}


//******************************************************************************************
//
bool CObjectHandle::bIsActive
(
)
{
	// Is this class entry active?
	return bInUse;
}


//******************************************************************************************
//
bool CObjectHandle::bActivate
(
	const CEasyString&  estr_classname,
	uint				u_handle_base
)
{
	// Is this class currently inactive?
	if (bInUse)
	{
		// Yes! Then don't allow the user to destroy it.  Return an error.
		return false;
	}

	// The class was not found, so return an error.
	estrClassName = estr_classname;

	uHandleBase	  = u_handle_base;
	uNextHandle   = u_handle_base;
	uHandleCount  = 0;
	bInUse		  = true;

	// Return a successful result.
	return true;
}


//******************************************************************************************
//
bool CObjectHandle::bDeactivate
(
	const CEasyString& estr_classname
)
{
	// Is this the correct class?
	if (estrClassName == estr_classname )
	{
		// Yes! Then destroy this handle class.
		Init();

		// Let the user know the class was destroyed.
		return true;
	}

	// The class was not found, so return an error.
	return false;
}


//******************************************************************************************
//
uint CObjectHandle::uWriteCount
(
)
{
	// Return the number of bytes required to represent the string in memory.
	return estrClassName.uWriteCount() + sizeof(uint) + sizeof(uint) + sizeof(uint) + sizeof(char);
}


//******************************************************************************************
//
uint CObjectHandle::uWrite
(
	char** ppc_buffer
)
{
	// Write classname into the buffer.
	uint u_count = estrClassName.uWrite(ppc_buffer);

	// Write the handle parameters.
	*(*((uint **) ppc_buffer))++ = uHandleBase;
	*(*((uint **) ppc_buffer))++ = uNextHandle;
	*(*((uint **) ppc_buffer))++ = uHandleCount;

	// Write out the booleain use variable.
	*(*ppc_buffer)++ = bInUse ? 1 : 0;
	
	// Increment the byte counter.
	u_count += sizeof(uint) + sizeof(uint) + sizeof(uint) + sizeof(char);

	// Return the number of bytes placed into the buffer.
	return u_count;
}


//******************************************************************************************
//
uint CObjectHandle::uRead
(
	char** ppc_buffer
)
{
	// Write classname into the buffer.
	uint u_count = estrClassName.uRead(ppc_buffer);

	// Write the handle parameters.
	uHandleBase  = *(*((uint **) ppc_buffer))++;
	uNextHandle  = *(*((uint **) ppc_buffer))++;
	uHandleCount = *(*((uint **) ppc_buffer))++;

	// Read in the boolean value and increment the pointer.
	bInUse = 1 == *(*ppc_buffer)++;

	// Increment the byte counter.
	u_count += sizeof(uint) + sizeof(uint) + sizeof(uint) + sizeof(char);

	// Return the number of bytes placed into the buffer.
	return u_count;
}


//**********************************************************************************************
//
std::ostream& operator<<
(
	std::ostream&			  os_stream, 
	const CHandleManager& hmgr_handle
)
{
	// Display the string.
	return os_stream << "CHandleManager(Count: " << hmgr_handle.uClassCount << std::hex << ", Base: 0x" 
		<< hmgr_handle.uHandleBase << ")" << std::dec;
}


//******************************************************************************************
//
bool CHandleManager::bSetup
(
	CObjectHandle& objh_handle
)
{
	// Look through the list to make sure that this object handle record doesn't exist.
	for (uint u_index = 0; u_index < aobjhObjectHandle.size(); u_index++)
	{
		// Is this class entry here?
		// HACK HACK HACK
		// This isn't safe, but here it is...  This allows version 12 files to load without all of the error
		//	checking that they really ought to have.  
		//  Made the following if conditional always evaluate to false.
		if (aobjhObjectHandle[u_index].estrName() == objh_handle.estrName() && false)
		{
			// Yes!  It is already here so return an error.
			return false;
		}
	}

	// This record is object handle group is not here, so add it to the object handle manager.
	uClassCount++;

	// Create the handle class entry.
	aobjhObjectHandle.push_back(objh_handle);

	// Return a successful result.
	return true;
}


//******************************************************************************************
//
CObjectHandle CHandleManager::objhCreate
(
	const CEasyString& estr_classname
)
{
	// Look through the list to make sure the Search the list to see if this class is here.
	for (uint u_index = 0; u_index < aobjhObjectHandle.size(); u_index++)
	{
		// Is this class entry here?
		if (estr_classname == aobjhObjectHandle[u_index].estrName())
		{
			// Yes!  It is already here so return a true result.
			return aobjhObjectHandle[u_index];
		}
	}

	// No! There were no available class entries available.  So, add it to the list.
	uHandleBase += uGROUP_OFFSET;
	uClassCount++;

	// Create the handle class entry.
	aobjhObjectHandle.push_back(CObjectHandle(estr_classname, uHandleBase));

	// Return a successful result.
	return aobjhObjectHandle[aobjhObjectHandle.size() - 1];
}


//******************************************************************************************
//
bool CHandleManager::bDelete
(
	const CEasyString& estr_classname
)
{
	std::deque<CObjectHandle>::iterator i = aobjhObjectHandle.begin();

	// Look through the list to make sure the Search the list to see if the class is active.
	for (; i != aobjhObjectHandle.end(); i++)
	{
		// Is this class entry here?
		if ((*i).estrName() == estr_classname)
		{
			int i_size = aobjhObjectHandle.size();

			// Yes!  Then delete it from the list.
			aobjhObjectHandle.erase(i);

			// Return a successful result.
			return aobjhObjectHandle.size() != i_size;
		}
	}

	// We were not able to find the class entry, so return an error.
	return false;
};


//******************************************************************************************
//
bool CHandleManager::bDestroyHandleManager
(
)
{
	// Get rid of all the class entries.
	aobjhObjectHandle.erase(aobjhObjectHandle.begin(), aobjhObjectHandle.end());

	return aobjhObjectHandle.size() == 0;
};


//******************************************************************************************
//
uint CHandleManager::uWriteCount
(
)
{
	// Return the number of bytes required to represent the string in memory.
	uint u_count = 0;
	for (uint u_index = 0; u_index < aobjhObjectHandle.size(); u_index++)
	{
		// Dump the handle entry records.
		u_count += aobjhObjectHandle[u_index].uWriteCount();
	}

	// Return the total number of bytes required by this object.
	return sizeof(uint) + sizeof(uint) + sizeof(uint) + u_count;
}


//******************************************************************************************
//
uint CHandleManager::uWrite
(
	char** ppc_buffer
)
{
	//
	// Write the class count, handle base and object handle record count.
	//

	*(*((uint **) ppc_buffer))++ = uClassCount;
	*(*((uint **) ppc_buffer))++ = uHandleBase;
	*(*((uint **) ppc_buffer))++ = aobjhObjectHandle.size();

	// Write classname into the buffer.
	uint u_count = sizeof(uint) + sizeof(uint) + sizeof(uint);

	// Loop through the object list and write them all out.
	for (uint u_index = 0; u_index < aobjhObjectHandle.size(); u_index++)
	{
		// Dump the handle entry records.
		u_count += aobjhObjectHandle[u_index].uWrite(ppc_buffer);
	}

	// Return the number of bytes placed into the buffer.
	return u_count;
}


//******************************************************************************************
//
uint CHandleManager::uRead
(
	char** ppc_buffer
)
{
	// Write the handle parameters.
	uClassCount		= *(*((uint **) ppc_buffer))++;
	uHandleBase		= *(*((uint **) ppc_buffer))++;
	uint u_entries	= *(*((uint **) ppc_buffer))++;

	// Advance the buffer pointer.
	uint u_count = sizeof(uint) + sizeof(uint) + sizeof(uint);

	// Loop through the object list and write them all out.
	for (uint u_index = 0; u_index < u_entries; u_index++)
	{
		// Load the object handle entry from the buffer.
		CObjectHandle objh_handle;
		u_count += objh_handle.uRead(ppc_buffer);

		// Add this object handle record to the back of the list.
		aobjhObjectHandle.push_back(objh_handle);
	}

	// Return the number of bytes placed into the buffer.
	return u_count;
}


//******************************************************************************************
//
void CHandleManager::Dump
(
)
{
	// Display the banner.
	std::cout << *this << std::endl;

	// Loop through each of the entries.
	for (uint u_index = 0; u_index < aobjhObjectHandle.size(); u_index++)
	{
		// Dump the handle entry records.
		std::cout << "Object Handle(" << std::dec << u_index << "), " << aobjhObjectHandle[u_index] << std::endl;
	}

	// Add a carriage return and spill the output to the console.
	std::cout << std::endl;
}