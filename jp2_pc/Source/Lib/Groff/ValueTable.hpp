/***********************************************************************************************
 *
 * Copyright (c) 1997 DreamWorks Interactive
 *
 * Contents: Classes for managing value record which are created by the OAL compiler for object
 * attributes.  The CValueTable class maintains a symbol table for the symbols used within the
 * user source code as well as the type information, scope information, relocation information,
 * user data and the compilation unit name (i.e. usually represents the name of the Max scene
 * file from which the attributes were generated).
 *
 * Bugs:
 *
 * To do:
 *
 * 1. Integrate error tracking to the uRead and uWrite classes.
 *
 * 2. Integrate the CFileIO functionality for message and stream IO logging.
 *
 * 3. Add support for CGuiInterface.
 *
 * 4. Add support for the NULL type.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/ValueTable.hpp                                              $
 * 
 * 9     98.08.22 11:24p Mmouni
 * ObjectValues now use custom CHandleList instead of deque.
 * 
 * 8     2/10/98 2:57p Agrant
 * Found the memory leaks-  use a virtual desrtuctor.
 * 
 * 7     10/16/97 1:57p Agrant
 * Moved value table access functions to VTParse.hpp
 * 
 * 6     5/27/97 3:16p Agrant
 * Fixed a returning reference to a local bug.
 * 
 * 5     5/15/97 3:02p Agrant
 * Lots of helper inline functions
 * 
 * 4     5/14/97 7:46p Agrant
 * Fixes a bug where functions returned  a reference to a local.
 * 
 * 3     5/13/97 10:49p Agrant
 * Intermediate check-in of text property loading constructors.
 * Should still support old hack.cpp stuff.
 * 
 * 2     5/13/97 1:17p Rwyatt
 * Renamed CSymbolTable to CNewsymbolTable to prevent clashes with the class of the same name
 * defined in SymTab.cpp/hpp
 * 
 * 1     5/12/97 9:58p Gstull
 * Classes for intermediate form management.
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_GROFF_VALUETABLE_HPP
#define HEADER_LIB_GROFF_VALUETABLE_HPP

//
// Disable a number of annoying warning messages about symbol truncation, and unsigned
// comparisons.
//
#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4786)

#include <iostream>
#include <iomanip>
#include <map>
#include <deque>

//
// Determine which set of standard types to use based upon the environment.  This is done to
// simplify the use of these classes within both the game and 3D Studio Max.
//

#ifdef USE_MAX_TYPES

#include "StandardTypes.hpp"
#include "EasyString.hpp"
#include "SymbolTable.hpp"
#include "ObjectHandle.hpp"

#else

#include "common.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "Lib/Groff/SymbolTable.hpp"
#include "Lib/Groff/ObjectHandle.hpp"

#endif


//**********************************************************************************************
//
enum EVarType
//
// Prefix: evt
//
{
	evtNULL	  = 0,	// Variable type is undefined.
	evtBOOL	  = 1,	// Variable type is boolean.
	evtCHAR	  = 2,	// Variable type is character.
	evtINT	  = 3,	// Variable type is int
	evtFLOAT  = 4,	// Variable type is float.
	evtSTRING = 5,	// Variable type is string.
	evtOBJECT = 6,	// Variable type is object.

	evtNODE	  = 7	// Variable type is scene node (Special).
};

static const char* strVarType[8] = { "?", "BOOL", "CHAR", "INT", "FLOAT", "STRING", "OBJECT", "NODE" };


//**********************************************************************************************
//
enum EVarScope
//
// Prefix: evs
//
{
	evsNULL		= 0,	// Variable scope is inknown.
	evsLOCAL	= 1,	// Variable scope within this file.
	evsPUBLIC	= 2		// Variable scope outside this file.
};


static const char* strVarScope[3] = { "?", "LOCAL", "PUBLIC" };

//**********************************************************************************************
//
enum ERefStatus
//
// Prefix: ers
//
{
	ersNULL			= 0,	// The reference state of this value is unknown.
	ersRESOLVED		= 1,	// The assignment value has not yet been resolved.
	ersUNRESOLVED	= 2		// The assignment value has been resolved. 
};

static const char* strVarReference[3] = { "?", "LINKED", "RELOC" };

static const char* strBoolValue[2] = { "FALSE", "TRUE" };


//**********************************************************************************************
//
class CBaseValue
//
// Prefix: basev
//
{
	EVarType	evtVarType;			// The type of value we are looking at.
	EVarScope	evsVarScope;		// Variable scope classification.
	ERefStatus	ersRefStatus;		// State of the resolution of this variable. 
	::CHandle		hValueHandle;		// Handle to this value entry record.
	::CHandle		hSymbolHandle;		// Handle to this symbol.
	::CHandle		hTargetHandle;		// Handle to the target symbol.

public:

	//******************************************************************************************
	//
	CBaseValue
	(
	);

	//******************************************************************************************
	//
	CBaseValue
	(
		const EVarType	evt_var_type,
		const EVarScope evs_var_scope,
		const ::CHandle	h_symbol_handle
	);


	//******************************************************************************************
	//
	CBaseValue
	(
		const CBaseValue& basev_value
	);

	virtual ~CBaseValue()
	{};

	//******************************************************************************************
	//
	void Handle
	(
		const ::CHandle& h_value_handle
	);


	//******************************************************************************************
	//
	::CHandle hHandle
	(
	) const;


	//******************************************************************************************
	//
	void Symbol
	(
		const ::CHandle& h_symbol_handle
	);


	//******************************************************************************************
	//
	const ::CHandle& hSymbol
	(
	) const;


	//******************************************************************************************
	//
	void RefState
	(
		const ERefStatus ers_status
	);


	//******************************************************************************************
	//
	ERefStatus ersRefState
	(
	) const;


	//******************************************************************************************
	//
	void Target
	(
		const ::CHandle& h_target_handle
	);


	//******************************************************************************************
	//
	const ::CHandle& hTarget
	(
	) const;


	//******************************************************************************************
	//
	const void Type 
	(
		const EVarType& evt_type
	);


	//******************************************************************************************
	//
	const EVarType evtType 
	(
	) const;

	
	//******************************************************************************************
	//
	const void Scope 
	(
		const EVarScope& evs_scope
	);


	//******************************************************************************************
	//
	const EVarScope evsScope 
	(
	) const;

	
	//**********************************************************************************************
	//
	friend std::ostream& operator<<
	(
		std::ostream&		os_stream,
		const CBaseValue&	basev_value
	);

	
	//******************************************************************************************
	//
	virtual uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	virtual uint uWrite
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	virtual uint uRead
	(
		char** ppc_buffer
	);
};


static const CBaseValue basevNULL_VALUE;


//**********************************************************************************************
//
class CBoolValue : public CBaseValue
//
// Prefix: bval
//
{
	//
	// The actual data value.
	//

	bool bBoolValue;

public:

	//******************************************************************************************
	//
	CBoolValue
	(
	);


	//******************************************************************************************
	//
	CBoolValue
	(
		const bool b_value
	);


	//******************************************************************************************
	//
	CBoolValue
	(
		const EVarScope	evs_var_scope,
		const ::CHandle	h_symbol_handle,
		const bool		b_value
	);


	//******************************************************************************************
	//
	CBoolValue
	(
		const CBoolValue& bval_value
	);


	//******************************************************************************************
	//
	CBoolValue& operator=
	(
		const CBoolValue& bval_value
	);


	//******************************************************************************************
	//
	bool bValue
	(
	);


	//******************************************************************************************
	//
	void Value
	(
		bool b_value
	);



	//**********************************************************************************************
	//
	friend std::ostream& operator<<
	(
		std::ostream&	  os_stream,
		const CBoolValue& bval_value
	);

	
	//**********************************************************************************************
	//
	friend std::istream& operator>>
	(
		std::istream&	is_stream,
		CBoolValue&	bval_value
	);


	//******************************************************************************************
	//
	virtual uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	virtual uint uWrite
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	virtual uint uRead
	(
		char** ppc_buffer
	);
};




//**********************************************************************************************
//
class CCharValue : public CBaseValue
//
// Prefix: cval
//
{
	//
	// The actual data value.
	//

	char cCharValue;

public:

	//******************************************************************************************
	//
	CCharValue
	(
	);


	//******************************************************************************************
	//
	CCharValue
	(
		const char c_value
	);


	//******************************************************************************************
	//
	CCharValue
	(
		const EVarScope	evs_var_scope,
		const ::CHandle	h_symbol_handle,
		const char		c_value
	);


	//******************************************************************************************
	//
	CCharValue
	(
		const CCharValue& cval_value
	);


	//******************************************************************************************
	//
	CCharValue& operator=
	(
		const CCharValue& cval_value
	);


	//******************************************************************************************
	//
	char cValue
	(
	);


	//******************************************************************************************
	//
	void Value
	(
		char c_value
	);


	//**********************************************************************************************
	//
	friend std::ostream& operator<<
	(
		std::ostream&	  os_stream,
		const CCharValue& cval_value
	);

	
	//**********************************************************************************************
	//
	friend std::istream& operator>>
	(
		std::istream&	is_stream,
		CCharValue&	cval_value
	);


	//******************************************************************************************
	//
	virtual uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	virtual uint uWrite
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	virtual uint uRead
	(
		char** ppc_buffer
	);
};


//**********************************************************************************************
//
class CIntValue : public CBaseValue
//
// Prefix: ival
//
{
	//
	// The actual data value.
	//

	int	iIntValue;

public:

	//******************************************************************************************
	//
	CIntValue
	(
	);


	//******************************************************************************************
	//
	CIntValue
	(
		const int i_value
	);


	//******************************************************************************************
	//
	CIntValue
	(
		const EVarScope	evs_var_scope,
		const ::CHandle	h_symbol_handle,
		const int		i_value
	);


	//******************************************************************************************
	//
	CIntValue
	(
		const CIntValue& ival_value
	);


	//******************************************************************************************
	//
	CIntValue& operator=
	(
		const CIntValue& ival_value
	);


	//******************************************************************************************
	//
	int iValue
	(
	);


	//******************************************************************************************
	//
	void Value
	(
		int i_value
	);



	//**********************************************************************************************
	//
	friend std::ostream& operator<<
	(
		std::ostream&	 os_stream,
		const CIntValue& ival_value
	);

	
	//**********************************************************************************************
	//
	friend std::istream& operator>>
	(
		std::istream&	is_stream,
		CIntValue&	ival_value
	);


	//******************************************************************************************
	//
	virtual uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	virtual uint uWrite
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	virtual uint uRead
	(
		char** ppc_buffer
	);
};


//**********************************************************************************************
//
class CFloatValue : public CBaseValue
//
// Prefix: sval
//
{
	//
	// The actual data value.
	//

	float	fFloatValue;

public:

	//******************************************************************************************
	//
	CFloatValue
	(
	);


	//******************************************************************************************
	//
	CFloatValue
	(
		const float f_value
	);


	//******************************************************************************************
	//
	CFloatValue
	(
		const EVarScope	evs_var_scope,
		const ::CHandle	h_symbol_handle,
		const float		f_value
	);


	//******************************************************************************************
	//
	CFloatValue
	(
		const CFloatValue& fval_value
	);


	//******************************************************************************************
	//
	CFloatValue& operator=
	(
		const CFloatValue& fval_value
	);


	//******************************************************************************************
	//
	float fValue
	(
	);


	//******************************************************************************************
	//
	void Value
	(
		float f_value
	);



	//**********************************************************************************************
	//
	friend std::ostream& operator<<
	(
		std::ostream&		os_stream,
		const CFloatValue&	fval_value
	);

	
	//**********************************************************************************************
	//
	friend std::istream& operator>>
	(
		std::istream&	is_stream,
		CFloatValue&	fval_value
	);


	//******************************************************************************************
	//
	virtual uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	virtual uint uWrite
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	virtual uint uRead
	(
		char** ppc_buffer
	);
};


//**********************************************************************************************
//
class CStringValue : public CBaseValue
//
// Prefix: estrsval
//
{
	//
	// The actual integer value.
	//

	CEasyString	estrStringValue;	// Actual variable contents.

public:

	//******************************************************************************************
	//
	CStringValue
	(
	);


	//******************************************************************************************
	//
	CStringValue
	(
		const CEasyString& estr_string
	);


	//******************************************************************************************
	//
	CStringValue
	(
		const EVarScope		evs_var_scope,
		const ::CHandle		h_symbol_handle,
		const CEasyString	estr_string
	);


	//******************************************************************************************
	//
	CStringValue
	(
		const CStringValue& estr_string
	);


	//******************************************************************************************
	//
	CStringValue& operator=
	(
		const CStringValue& estr_string_value
	);


	//******************************************************************************************
	//
	CEasyString& estrValue
	(
	);


	//******************************************************************************************
	//
	void Value
	(
		CEasyString& estr_value
	);



	//**********************************************************************************************
	//
	friend std::ostream& operator<<
	(
		std::ostream&		os_stream,
		const CStringValue& sval_value
	);

	
	//**********************************************************************************************
	//
	friend std::istream& operator>>
	(
		std::istream& is_stream,
		CStringValue& sval_value
	);


	//******************************************************************************************
	//
	virtual uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	virtual uint uWrite
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	virtual uint uRead
	(
		char** ppc_buffer
	);
};


const int i_static_size = 4;
const int i_grow_size = 8;

//**********************************************************************************************
//
class CHandleList
//
// A simple very compact list of CHandle objects.
// Prefix: ah
// 
//******************************
{
private:
	int			i_count;					// Number of elements.
	::CHandle		ah_static[i_static_size];	// Statically allocated handles.
	int			i_buf_size;					// Allocated size.
	::CHandle*	ph_array;					// Array of allocated handles.

	// Re-allocate the allocated array.
	void reallocate(int i_alloc_size)
	{
		// Allocate new buffer.
		::CHandle* ph_new = new ::CHandle[i_alloc_size];

		if (ph_array)
		{
			// Copy elemenets.
			for (int i = 0; i < i_count-i_static_size; i++)
				ph_new[i] = ph_array[i];

			// Free old buffer.
			delete[] ph_array;
		}
		
		i_buf_size = i_alloc_size;
		ph_array = ph_new;
	}

	// Make sure we have enought room for i_grow_by more elements.
	void grow(int i_grow_by)
	{
		// See if we need to allocate any more elements.
		if (i_count - i_static_size + i_grow_by > i_buf_size)
		{
			// Need more elements (round up to i_grow_size).
			reallocate(i_count - i_static_size + i_grow_by + i_grow_size);
		}
	}

public:

#if (HANDLE_LIST_STATS)
	// Average handles per list was 6.6 in "lab.grf".
	static int iTotalListCount;
	static int iTotalHandleCount;
#endif

	CHandleList()
	{
		i_count = 0;
		i_buf_size = 0;
		ph_array = 0;
	}

	~CHandleList()
	{
		delete[] ph_array;
	}

	const ::CHandle& operator[] (const int index) const
	{
		if (index < i_static_size)
			return ah_static[index];
		else
			return ph_array[index-i_static_size];
	}

	void SetAllocation(int i_alloc_size)
	{
#if (HANDLE_LIST_STATS)
		iTotalListCount++;
		iTotalHandleCount += i_alloc_size;
#endif

		if (i_alloc_size > i_static_size)
			reallocate(i_alloc_size - i_static_size);
	}

	void clear()
	{
		i_count = 0;
	}

	int size() const
	{
		return i_count;
	}

	void push_back(const ::CHandle& h_handle)
	{
		if (i_count < 4)
		{
			ah_static[i_count++] = h_handle;
		}
		else
		{
			grow(1);
			ph_array[i_count - i_static_size] = h_handle;
			i_count++;
		}
	}
};


//**********************************************************************************************
//
class CObjectValue : public CBaseValue
//
// Prefix: oval
//
{
	//
	// The object entry handles.
	//
	CHandleList ahHandleList;

public:

	//******************************************************************************************
	//
	CObjectValue
	(
	);


	//******************************************************************************************
	//
	CObjectValue
	(
		const EVarScope	evs_var_scope,		// Scope of visibility of this object.
		const ::CHandle	h_symbol_handle		// Handle to the symbol for this object.
	);


	//******************************************************************************************
	//
	~CObjectValue
	(
	);


	//******************************************************************************************
	//
	uint uCount
	(
	);


	//******************************************************************************************
	//
	uint uAddElement
	(
		const ::CHandle& h_handle
	);
	

	//******************************************************************************************
	//
	const ::CHandle& hElementHandle
	(
		const uint u_index
	);
	

	//******************************************************************************************
	//
	CObjectValue& operator=
	(
		const CObjectValue& oval_value
	);


	//**********************************************************************************************
	//
	friend std::ostream& operator<<
	(
		std::ostream&	os_stream,
		CObjectValue&	oval_value
	);

	
	//**********************************************************************************************
	//
	friend std::istream& operator>>
	(
		std::istream& is_stream,
		CObjectValue& estr_string
	);


	//******************************************************************************************
	//
	virtual uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	virtual uint uWrite
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	virtual uint uRead
	(
		char** ppc_buffer
	);
};


//**********************************************************************************************
//
class CValueTable : public CNewSymbolTable
//
// Prefix: vtab
//
{
	CEasyString									estrValueName;
	CObjectHandle								objhHandle;
	CHandleManager								hmgrManager;

	// Provide fast access to the value record through it's handle. 
	std::map< ::CHandle, CBaseValue*, std::less<::CHandle> >	aviValueIndex;

public:

	//******************************************************************************************
	//
	CValueTable
	(
	);


	//******************************************************************************************
	//
	CValueTable
	(
		CEasyString	  estr_module_name
	);


	//******************************************************************************************
	//
	~CValueTable
	(
	);


	//******************************************************************************************
	//
	::CHandle hBaseValue
	(
		CBaseValue* pbasev_value
	);


	//******************************************************************************************
	//

	CBaseValue& operator[]
	(
		const ::CHandle& h_handle
	);


	//******************************************************************************************
	//
	virtual uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	virtual uint uWrite
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	virtual uint uRead
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