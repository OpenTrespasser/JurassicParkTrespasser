/***********************************************************************************************
 *
 * Copyright (c) 1997 DreamWorks Interactive
 *
 * Contents: Set of classes for managing handle profiles for use with symbol tables, value
 * tables, file images, smart buffers, easy string, etc.  The intent is to provide a structural
 * component that can be reused, but which designed to be unique to the systems which use them
 * so that uniqueness is maintained.  For example a symbol handle and a value handle are both
 * declared from the same type CHandle but, the resulting handles are incompatible in each
 * others entry points, meaning thet an attempt to use one handle in the others subsystem will
 * result in an invalid handle error.
 *
 * Bugs:
 *
 * To do: 
 *
 * 1. Modify the base address allocator to verify the base uniqueness.
 *
 * 2. Modify the handle manager to take #1 into account when calling bSetup(...).
 *
 * 3. Add FileIO logging capability to the stream IO methods.
 *
 * 4. Integrate the CGuiInterface class into this code.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/ObjectHandle.hpp                                            $
 * 
 * 5     98.08.22 11:28p Mmouni
 * Change uWrite to be const.
 * 
 * 4     5/27/97 3:16p Agrant
 * Fixed a returning reference to a local bug.
 * 
 * 3     5/26/97 1:38p Agrant
 * LINT minor syntax error fixed.
 * 
 * 2     5/20/97 3:04p Gstull
 * Added a new constructor to allow the ranges of object handles to be allocated.
 * 
 * 1     5/12/97 9:56p Gstull
 * Classes for object handle management.
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_GROFF_OBJECTHANDLE_HPP
#define HEADER_LIB_GROFF_OBJECTHANDLE_HPP

#include <iostream.h>

//
// Disable a number of annoying warning messages about symbol truncation, and unsigned
// comparisons.
//

#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4786)

#include <map.h>
#include <deque.h>

//
// Determine which set of standard types to use based upon the environment.  This is done to
// simplify the use of these classes within both the game and 3D Studio Max.
//

#ifdef USE_MAX_TYPES

#include "StandardTypes.hpp"
#include "EasyString.hpp"

#else

#include "Common.hpp"
#include "Lib/Groff/EasyString.hpp"

#endif

//
// Define the offset of the handle groups.
//

#ifndef uGROUP_OFFSET
#define uGROUP_OFFSET 0x100000
#endif


//**********************************************************************************************
//
class CHandle
//
// Prefix: h
//
// Class for managing the information associated with a handle.
//
{
	uint uHandleID;
	uint uReferenceCount;

public:

	//******************************************************************************************
	//
	CHandle
	(
	);


	//******************************************************************************************
	//
	CHandle
	(
		const uint u_handle
	);


	//******************************************************************************************
	//
	CHandle
	(
		const uint u_handle,
		const uint u_ref_count
	);


	//******************************************************************************************
	//
	uint uHandle
	(
	);


	//******************************************************************************************
	//
	uint uCount
	(
	);


	//******************************************************************************************
	//
	bool operator==
	(
		const CHandle& h_handle
	) const;


	//******************************************************************************************
	//
	bool operator!=
	(
		const CHandle& h_handle
	) const;


	//******************************************************************************************
	//
	bool operator<
	(
		const CHandle& h_handle
	) const;


	//******************************************************************************************
	//
	bool operator<=
	(
		const CHandle& h_handle
	) const;


	//******************************************************************************************
	//
	bool operator>
	(
		const CHandle& h_handle
	) const;


	//******************************************************************************************
	//
	bool operator>=
	(
		const CHandle& h_handle
	) const;


	//******************************************************************************************
	//
	CHandle& operator++
	(
		int
	);


	//******************************************************************************************
	//
	CHandle& operator--
	(
		int
	);


	//******************************************************************************************
	//
	CHandle& operator+=
	(
		uint u_reference_count
	);


	//******************************************************************************************
	//
	CHandle& operator-=
	(
		uint u_reference_count
	);


	//**********************************************************************************************
	//
	friend ostream& operator<<
	(
		ostream&		os_stream, 
		const CHandle&	h_handle
	);

	
	//**********************************************************************************************
	//
	friend istream& operator>>
	(
		istream& is_stream,
		CHandle& h_handle
	);


	//******************************************************************************************
	//
	static uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	uint uWrite
	(
		char** ppc_buffer
	) const;


	//******************************************************************************************
	//
	uint uRead
	(
		char** ppc_buffer
	);
};


//
// Define the "NULL" handle.
//

static const CHandle hNULL_HANDLE;


//**********************************************************************************************
//
class CObjectHandle
//
// Prefix: objh
//
// Class for managing a set or group of handles.
//
{
	CEasyString	estrClassName;
	uint		uHandleBase;
	uint		uNextHandle;
	uint		uHandleCount;
	bool		bInUse;


	//******************************************************************************************
	//
	void Init
	(
	);

public:

	//******************************************************************************************
	//
	CObjectHandle
	(
	);
	

	//******************************************************************************************
	//
	CObjectHandle
	(
		const CEasyString&	estr_classname,
		uint				u_handle_base
	);

	
	//******************************************************************************************
	//
	~CObjectHandle
	(
	);

	
	//******************************************************************************************
	//
	CObjectHandle& operator=
	(
		const CObjectHandle& hobj_handle
	);


	//**********************************************************************************************
	//
	friend ostream& operator<<
	(
		ostream&			 os_stream, 
		const CObjectHandle& objh_handle
	);


	//**********************************************************************************************
	//
	friend istream& operator>>
	(
		istream&		is_stream,
		CObjectHandle&	objh_handle
	);

	
	//******************************************************************************************
	//
	CEasyString& estrName
	(
	);


	//******************************************************************************************
	//
	CHandle hNext
	(
	);


	//******************************************************************************************
	//
	uint uNextRange
	(
	);


	//******************************************************************************************
	//
	bool bIsActive
	(
	);


	//******************************************************************************************
	//
	bool bActivate
	(
		const CEasyString&  estr_classname,
		uint				u_handle_base
	);


	//******************************************************************************************
	//
	bool bDeactivate
	(
		const CEasyString& estr_classname
	);


	//******************************************************************************************
	//
	uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	uint uWrite
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	uint uRead
	(
		char** ppc_buffer
	);
};


//**********************************************************************************************
//
class CHandleManager
//
// Prefix: hmgr
//
// Class for managing all the handle "groups".
//
{
	static uint					uClassCount;
	static uint					uHandleBase;
	static deque<CObjectHandle>	aobjhObjectHandle;

public:

	//**********************************************************************************************
	//
	friend ostream& operator<<
	(
		ostream&			  os_stream, 
		const CHandleManager& hmgr_handle
	);

	
	//******************************************************************************************
	//
	bool bSetup
	(
		CObjectHandle& objh_handle
	);


	//******************************************************************************************
	//
	CObjectHandle objhCreate
	(
		const CEasyString& estr_classname
	);


	//******************************************************************************************
	//
	bool bDelete
	(
		const CEasyString& estr_classname
	);


	//******************************************************************************************
	//
	bool bDestroyHandleManager
	(
	);


	//******************************************************************************************
	//
	uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	uint uWrite
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	uint uRead
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	void Dump
	(
	);
};

#endif