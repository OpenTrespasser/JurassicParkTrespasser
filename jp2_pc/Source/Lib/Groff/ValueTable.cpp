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
 * $Log:: /JP2_PC/Source/Lib/Groff/ValueTable.cpp                                              $
 * 
 * 7     98.08.22 11:27p Mmouni
 * ObjectValues now use custom CHandleList instead of deque and other misc. changes.
 * 
 * 
 * 6     98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 5     5/27/97 3:16p Agrant
 * Fixed a returning reference to a local bug.
 * 
 * 4     5/15/97 7:26p Agrant
 * Clean up object values on deletion.
 * 
 * 3     5/14/97 7:46p Agrant
 * Fixes a bug where functions returned  a reference to a local.
 * 
 * 2     5/13/97 1:17p Rwyatt
 * Renamed CSymbolTable to CNewsymbolTable to prevent clashes with the class of the same name
 * defined in SymTab.cpp/hpp
 * 
 * 1     5/12/97 9:58p Gstull
 * Classes for intermediate form management.
 *
 **********************************************************************************************/

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
#include "ObjectHandle.hpp"
#include "SymbolTable.hpp"
#include "ValueTable.hpp"

#else

#include "Common.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Groff/SymbolTable.hpp"
#include "Lib/Groff/ValueTable.hpp"

#endif


#if (HANDLE_LIST_STATS)
int CHandleList::iTotalListCount = 0;
int CHandleList::iTotalHandleCount = 0;
#endif


// A null base value for generic use.
CBaseValue bvalNull = CBaseValue();

//******************************************************************************************
//
CBaseValue::CBaseValue
(
)
{
	// Setup the class to be NULL.
	evtVarType    = evtNULL;
	evsVarScope	  = evsNULL;
	ersRefStatus  = ersNULL;
	hValueHandle  = hNULL_HANDLE;
	hSymbolHandle = hNULL_HANDLE;
	hTargetHandle = hNULL_HANDLE;
}


//******************************************************************************************
//
CBaseValue::CBaseValue
(
	const EVarType	evt_var_type,
	const EVarScope evs_var_scope,
	const CHandle	h_symbol_handle
)
{
	// Copy the variable information.
	evtVarType    = evt_var_type;
	evsVarScope   = evs_var_scope;
	ersRefStatus  = ersNULL;
	hValueHandle  = hNULL_HANDLE;
	hSymbolHandle = h_symbol_handle;
	hTargetHandle = hNULL_HANDLE;
}


//******************************************************************************************
//
CBaseValue::CBaseValue
(
	const CBaseValue& basev_value
)
{
	// Copy the variable information.
	evtVarType    = basev_value.evtVarType;
	evsVarScope   = basev_value.evsVarScope;
	ersRefStatus  = basev_value.ersRefStatus;
	hValueHandle  = basev_value.hValueHandle;
	hSymbolHandle = basev_value.hSymbolHandle;
	hTargetHandle = basev_value.hTargetHandle;
}


//******************************************************************************************
//
void CBaseValue::Handle
(
	const CHandle& h_value_handle
)
{
	// Set handle record for this value entry.
	hValueHandle = h_value_handle;
}


//******************************************************************************************
//
CHandle CBaseValue::hHandle
(
) const
{
	// Return the value entry handle.
	return CHandle(hValueHandle);
}

//******************************************************************************************
//
void CBaseValue::Symbol
(
	const CHandle& h_symbol_handle
) 
{
	// Return the 
	hSymbolHandle = h_symbol_handle;
}


//******************************************************************************************
//
const CHandle& CBaseValue::hSymbol
(
) const
{
	// Return the NULL handle.
	return hSymbolHandle;
}


//******************************************************************************************
//
void CBaseValue::RefState
(
	const ERefStatus ers_status
)
{
	// Set the reference state.
	ersRefStatus = ers_status;
}


//******************************************************************************************
//
ERefStatus CBaseValue::ersRefState
(
) const
{
	// Return the reference state.
	return ersRefStatus;
}


//******************************************************************************************
//
void CBaseValue::Target
(
	const CHandle& h_target_handle
)
{
	// Set the target reference handle.
	hTargetHandle = h_target_handle;
}


//******************************************************************************************
//
const CHandle& CBaseValue::hTarget
(
) const
{
	// Return the NULL handle.
	return hTargetHandle;
}


//******************************************************************************************
//
const void CBaseValue::Type 
(
	const EVarType& evt_type
)
{
	// Set the scope of the variable.
	evtVarType = evt_type;
}


//******************************************************************************************
//
const EVarType CBaseValue::evtType 
(
) const
{
	// Return the scope of this variable.
	return evtVarType;
}


//******************************************************************************************
//
const void CBaseValue::Scope 
(
	const EVarScope& evs_scope
)
{
	// Set the scope of the variable.
	evsVarScope = evs_scope;
}


//******************************************************************************************
//
const EVarScope CBaseValue::evsScope 
(
) const
{
	// Return the scope of this variable.
	return evsVarScope;
}


//**********************************************************************************************
//
std::ostream& operator<<
(
	std::ostream&			os_stream, 
	const CBaseValue&	basev_value
)
{
	// Display the string.
	os_stream.setf(std::ios::uppercase|std::ios::hex);

	os_stream << "basev(val" << basev_value.hValueHandle << ", sym" 
		<< basev_value.hSymbolHandle << ", " << strVarType[basev_value.evtVarType] << ", " 
		<< strVarScope[basev_value.evsVarScope] << ", target" << basev_value.hTargetHandle
		<< ", " << strVarReference[basev_value.ersRefStatus] << ")" << std::dec << std::flush;

	// Return the stream.
	return os_stream;
}


//******************************************************************************************
//
uint CBaseValue::uWriteCount
(
)
{
	// Start out by getting the size of the value handle and the symbol handle.
	uint u_count = sizeof(char) + sizeof(char) + sizeof(char);

	// Now add the size of the variable type, scope and the value reference state.
	u_count += hValueHandle.uWriteCount() + hSymbolHandle.uWriteCount() + hTargetHandle.uWriteCount();

	// Return the number of bytes required.
	return u_count;
}


//******************************************************************************************
//
uint CBaseValue::uWrite
(
	char** ppc_buffer
)
{
	// Write out the variable type, scope and resolution status.
	*(*ppc_buffer)++ = (char) evtVarType;
	*(*ppc_buffer)++ = (char) evsVarScope;
	*(*ppc_buffer)++ = (char) ersRefStatus;

	// Increment the counter.
	uint u_count = sizeof(char) + sizeof(char) + sizeof(char);

	// Write out the value, symbol and target handles
	u_count += hValueHandle.uWrite(ppc_buffer);
	u_count += hSymbolHandle.uWrite(ppc_buffer);
	u_count += hTargetHandle.uWrite(ppc_buffer);

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
uint CBaseValue::uRead
(
	char** ppc_buffer
)
{
	// Read in the variable type, scope and resolution status.
	evtVarType   = (EVarType)   *(*ppc_buffer)++;
	evsVarScope  = (EVarScope)  *(*ppc_buffer)++;
	ersRefStatus = (ERefStatus) *(*ppc_buffer)++;
	
	// Increment the counter.
	uint u_count = sizeof(char) + sizeof(char) + sizeof(char);

	// Read in the value, symbol and target handles
	u_count += hValueHandle.uRead(ppc_buffer);
	u_count += hSymbolHandle.uRead(ppc_buffer);
	u_count += hTargetHandle.uRead(ppc_buffer);

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
CBoolValue::CBoolValue
(
) : CBaseValue() 
{ 
	// Set the value to false.
	bBoolValue = false;
}


//******************************************************************************************
//
CBoolValue::CBoolValue
(
	const bool b_value
) : CBaseValue()
{ 
	// Setup the user specified value.
	bBoolValue = b_value;
}


//******************************************************************************************
//
CBoolValue::CBoolValue
(
	const EVarScope	evs_var_scope,
	const CHandle	h_symbol_handle,
	const bool		b_value
) : CBaseValue(evtBOOL, evs_var_scope, h_symbol_handle)
{ 
	// Setup the user specified value.
	bBoolValue = b_value; 
}


//******************************************************************************************
//
CBoolValue::CBoolValue
(
	const CBoolValue& bval_value
) 
{ 
	// Setup this value entry.
	bBoolValue = bval_value.bBoolValue;
}


//******************************************************************************************
//
CBoolValue& CBoolValue::operator=
(
	const CBoolValue& bval_value
) 
{ 
	// Setup this value entry.
	if (this == &bval_value)
	{
		// We are assigning to ourselves.
		return *this;
	}

	// Copy all the fields in the base class.
	Type(evtBOOL);
	Scope(bval_value.evsScope());
	RefState(bval_value.ersRefState());

	Handle(bval_value.hHandle());
	Symbol(bval_value.hSymbol());
	Target(bval_value.hTarget());

	// Now copy the local parameter.
	bBoolValue = bval_value.bBoolValue;

	// Return this record.
	return *this;
}


//******************************************************************************************
//
bool CBoolValue::bValue
(
)
{
	// Return the value.
	return bBoolValue;
}


//******************************************************************************************
//
void CBoolValue::Value
(
	bool b_value
)
{
	// Setup the integer value. 
	bBoolValue = b_value;
}



//**********************************************************************************************
//
std::ostream& operator<<
(
	std::ostream&		  os_stream, 
	const CBoolValue& bval_value
)
{
	// Display the string.
	return os_stream << std::dec << strBoolValue[bval_value.bBoolValue] << " -> " << (CBaseValue) bval_value;
}


//**********************************************************************************************
//
std::istream& operator>>
(
	std::istream&	is_stream,
	CBoolValue&	bval_value
)
{
	int i_temp;

	// Read in the string.
	is_stream >> std::dec >> i_temp;

	bval_value.bBoolValue = i_temp;

	// Pass the input stream along.
	return is_stream;
}


//******************************************************************************************
//
uint CBoolValue::uWriteCount
(
)
{
	// Return the number of bytes required.
	return CBaseValue::uWriteCount() + sizeof(char);
}


//******************************************************************************************
//
uint CBoolValue::uWrite
(
	char** ppc_buffer
)
{
	// Start by writing the base value record.
	uint u_count = CBaseValue::uWrite(ppc_buffer);

	// Write the bool value.
	*(*ppc_buffer)++ = bBoolValue ? 1 : 0;

	// Increment the byte pointer.
	u_count += sizeof(char);

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
 uint CBoolValue::uRead
(
	char** ppc_buffer
)
{
	// Start by reading the base value record.
	uint u_count = CBaseValue::uRead(ppc_buffer);

	// Read in the bool value.
	bBoolValue = *(*ppc_buffer)++;

	// Increment the byte pointer.
	u_count += sizeof(char);

	// Return the number of bytes read in.
	return u_count;
}


//******************************************************************************************
//
CCharValue::CCharValue
(
) : CBaseValue() 
{ 
	// Set the value to false.
	cCharValue = '\0';
}


//******************************************************************************************
//
CCharValue::CCharValue
(
	const char c_value
) : CBaseValue()
{ 
	// Setup the user specified value.
	cCharValue = c_value;
}


//******************************************************************************************
//
CCharValue::CCharValue
(
	const EVarScope	evs_var_scope,
	const CHandle	h_symbol_handle,
	const char		c_value
) : CBaseValue(evtCHAR, evs_var_scope, h_symbol_handle)
{ 
	// Setup the user specified value.
	cCharValue = c_value; 
}


//******************************************************************************************
//
CCharValue::CCharValue
(
	const CCharValue& cval_value
) 
{ 
	// Setup this value entry.
	cCharValue = cval_value.cCharValue;
}


//******************************************************************************************
//
CCharValue& CCharValue::operator=
(
	const CCharValue& cval_value
) 
{ 
	// Setup this value entry.
	if (this == &cval_value)
	{
		// We are assigning to ourselves.
		return *this;
	}

	// Copy all the fields in the base class.
	Type(evtCHAR);
	Scope(cval_value.evsScope());
	RefState(cval_value.ersRefState());

	Handle(cval_value.hHandle());
	Symbol(cval_value.hSymbol());
	Target(cval_value.hTarget());

	// Now copy the local parameter.
	cCharValue = cval_value.cCharValue;

	// Return this record.
	return *this;
}


//******************************************************************************************
//
char CCharValue::cValue
(
)
{
	// Return the value.
	return cCharValue;
}


//******************************************************************************************
//
void CCharValue::Value
(
	char c_value
)
{
	// Setup the integer value. 
	cCharValue = c_value;
}



//**********************************************************************************************
//
std::ostream& operator<<
(
	std::ostream&		  os_stream, 
	const CCharValue& cval_value
)
{
	// Display the string.
	return os_stream << std::dec << cval_value.cCharValue << " -> " << (CBaseValue) cval_value;
}


//**********************************************************************************************
//
std::istream& operator>>
(
	std::istream&	is_stream,
	CCharValue&	cval_value
)
{
	// Read in the string.
	is_stream >> std::dec >> cval_value.cCharValue;

	// Pass the input stream along.
	return is_stream;
}


//******************************************************************************************
//
uint CCharValue::uWriteCount
(
)
{
	// Return the number of bytes required.
	return CBaseValue::uWriteCount() + sizeof(char);
}


//******************************************************************************************
//
uint CCharValue::uWrite
(
	char** ppc_buffer
)
{
	// Start by writing the base value record.
	uint u_count = CBaseValue::uWrite(ppc_buffer);

	// Write the char value.
	*(*ppc_buffer)++ = cCharValue;

	// Increment the byte pointer.
	u_count += sizeof(char);

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
 uint CCharValue::uRead
(
	char** ppc_buffer
)
{
	// Start by reading the base value record.
	uint u_count = CBaseValue::uRead(ppc_buffer);

	// Read in the char value.
	cCharValue = *(*ppc_buffer)++;

	// Increment the byte pointer.
	u_count += sizeof(char);

	// Return the number of bytes read in.
	return u_count;
}


//******************************************************************************************
//
CIntValue::CIntValue
(
) : CBaseValue() 
{ 
	// Set the value to 0.
	iIntValue = 0;
}


//******************************************************************************************
//
CIntValue::CIntValue
(
	const int i_value
) : CBaseValue()
{ 
	// Setup the user specified value.
	iIntValue = i_value;
}


//******************************************************************************************
//
CIntValue::CIntValue
(
	const EVarScope	evs_var_scope,
	const CHandle	h_symbol_handle,
	const int		i_value
) : CBaseValue(evtINT, evs_var_scope, h_symbol_handle)
{ 
	// Setup the user specified value.
	iIntValue = i_value; 
}


//******************************************************************************************
//
CIntValue::CIntValue
(
	const CIntValue& ival_value
) 
{ 
	// Setup this value entry.
	iIntValue = ival_value.iIntValue;
}


//******************************************************************************************
//
CIntValue& CIntValue::operator=
(
	const CIntValue& ival_value
) 
{ 
	// Setup this value entry.
	if (this == &ival_value)
	{
		// We are assigning to ourselves.
		return *this;
	}

	// Copy all the fields in the base class.
	Type(evtINT);
	Scope(ival_value.evsScope());
	RefState(ival_value.ersRefState());

	Handle(ival_value.hHandle());
	Symbol(ival_value.hSymbol());
	Target(ival_value.hTarget());

	// Now copy the local parameter.
	iIntValue = ival_value.iIntValue;

	// Return this record.
	return *this;
}


//******************************************************************************************
//
int CIntValue::iValue
(
)
{
	// Return the integer value.
	return iIntValue;
}


//******************************************************************************************
//
void CIntValue::Value
(
	int i_value
)
{
	// Setup the integer value. 
	iIntValue = i_value;
}



//**********************************************************************************************
//
ostream& operator<<
(
	ostream&		 os_stream, 
	const CIntValue& ival_value
)
{
	// Display the string.
	return os_stream << dec << ival_value.iIntValue << " -> " << (CBaseValue) ival_value;
}


//**********************************************************************************************
//
istream& operator>>
(
	istream&	is_stream,
	CIntValue&	ival_value
)
{
	// Read in the string.
	is_stream >> dec >> ival_value.iIntValue;

	// Pass the input stream along.
	return is_stream;
}


//******************************************************************************************
//
uint CIntValue::uWriteCount
(
)
{
	// Return the number of bytes required.
	return CBaseValue::uWriteCount() + sizeof(int);
}


//******************************************************************************************
//
uint CIntValue::uWrite
(
	char** ppc_buffer
)
{
	// Start by writing the base value record.
	uint u_count = CBaseValue::uWrite(ppc_buffer);

	// Write the integer value.
	*(*((int **) ppc_buffer))++ = iIntValue;

	// Increment the byte pointer.
	u_count += sizeof(uint);

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
uint CIntValue::uRead
(
	char** ppc_buffer
)
{
	// Start by reading the base value record.
	uint u_count = CBaseValue::uRead(ppc_buffer);

	// Read in the integer value.
	iIntValue = *(*((int **) ppc_buffer))++;

	// Increment the byte pointer.
	u_count += sizeof(uint);

	// Return the number of bytes read in.
	return u_count;
}


//******************************************************************************************
//
CFloatValue::CFloatValue
(
) : CBaseValue() 
{ 
	// Set the value to 0.
	fFloatValue = 0.0f;
}


//******************************************************************************************
//
CFloatValue::CFloatValue
(
	const float f_value
) : CBaseValue()
{ 
	// Setup the user specified value.
	fFloatValue = f_value;
}


//******************************************************************************************
//
CFloatValue::CFloatValue
(
	const EVarScope	evs_var_scope,
	const CHandle	h_symbol_handle,
	const float		f_value
) : CBaseValue(evtFLOAT, evs_var_scope, h_symbol_handle)
{ 
	// Setup the user specified value.
	fFloatValue = f_value; 
}


//******************************************************************************************
//
CFloatValue::CFloatValue
(
	const CFloatValue& fval_value
) 
{ 
	// Setup this value entry.
	fFloatValue = fval_value.fFloatValue;
}


//******************************************************************************************
//
CFloatValue& CFloatValue::operator=
(
	const CFloatValue& fval_value
) 
{ 
	// Setup this value entry.
	if (this == &fval_value)
	{
		// We are assigning to ourselves.
		return *this;
	}

	// Copy all the fields in the base class.
	Type(evtFLOAT);
	Scope(fval_value.evsScope());
	RefState(fval_value.ersRefState());

	Handle(fval_value.hHandle());
	Symbol(fval_value.hSymbol());
	Target(fval_value.hTarget());

	// Now copy the local parameter.
	fFloatValue = fval_value.fFloatValue;

	// Return this record.
	return *this;
}


//******************************************************************************************
//
float CFloatValue::fValue
(
)
{
	// Return the float value.
	return fFloatValue;
}


//******************************************************************************************
//
void CFloatValue::Value
(
	float f_value
)
{
	// Setup the integer value. 
	fFloatValue = f_value;
}



//**********************************************************************************************
//
ostream& operator<<
(
	ostream&			os_stream, 
	const CFloatValue&	fval_value
)
{
	// Display the string.
	return os_stream << fval_value.fFloatValue << " -> " << (CBaseValue) fval_value;
}


//**********************************************************************************************
//
istream& operator>>
(
	istream&		is_stream,
	CFloatValue&	fval_value
)
{
	// Read in the string.
	is_stream >> fval_value.fFloatValue;

	// Pass the input stream along.
	return is_stream;
}


//******************************************************************************************
//
uint CFloatValue::uWriteCount
(
)
{
	// Return the number of bytes required.
	return CBaseValue::uWriteCount() + sizeof(float);
}


//******************************************************************************************
//
uint CFloatValue::uWrite
(
	char** ppc_buffer
)
{
	// Start by writing the base value record.
	uint u_count = CBaseValue::uWrite(ppc_buffer);

	// Write the float value.
	*(*((float **) ppc_buffer))++ = fFloatValue;

	// Increment the byte pointer.
	u_count += sizeof(float);

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
 uint CFloatValue::uRead
(
	char** ppc_buffer
)
{
	// Start by reading the base value record.
	uint u_count = CBaseValue::uRead(ppc_buffer);

	// Read in the float value.
	fFloatValue = *(*((float **) ppc_buffer))++;

	// Increment the byte pointer.
	u_count += sizeof(float);

	// Return the number of bytes read in.
	return u_count;
}


//******************************************************************************************
//
CStringValue::CStringValue
(
) : CBaseValue() 
{ 
}


//******************************************************************************************
//
CStringValue::CStringValue
(
	const CEasyString& estr_string
) : CBaseValue()
{ 
	// Setup the user specified value.
	estrStringValue = estr_string;
}


//******************************************************************************************
//
CStringValue::CStringValue
(
	const EVarScope		evs_var_scope,
	const CHandle		h_symbol_handle,
	const CEasyString	estr_string
) : CBaseValue(evtSTRING, evs_var_scope, h_symbol_handle)
{ 
	// Setup the user specified value.
	estrStringValue = estr_string; 
}


//******************************************************************************************
//
CStringValue::CStringValue
(
	const CStringValue& estr_string
) 
{ 
	// Setup this value entry.
	estrStringValue = estr_string.estrStringValue;
}


//******************************************************************************************
//
CStringValue& CStringValue::operator=
(
	const CStringValue& estr_string_value
) 
{ 
	// Setup this value entry.
	if (this == &estr_string_value)
	{
		// We are assigning to ourselves.
		return *this;
	}

	// Copy all the fields in the base class.
	Type(evtSTRING);
	Scope(estr_string_value.evsScope());
	RefState(estr_string_value.ersRefState());

	Handle(estr_string_value.hHandle());
	Symbol(estr_string_value.hSymbol());
	Target(estr_string_value.hTarget());

	// Now copy the local parameter.
	estrStringValue = estr_string_value.estrStringValue;

	// Return this record.
	return *this;
}


//******************************************************************************************
//
CEasyString& CStringValue::estrValue
(
)
{
	// Return the integer value.
	return estrStringValue;
}


//******************************************************************************************
//
void CStringValue::Value
(
	CEasyString& estr_value
)
{
	// Setup the integer value. 
	estrStringValue = estr_value;
}



//**********************************************************************************************
//
ostream& operator<<
(
	ostream&		    os_stream, 
	const CStringValue& sval_value
)
{
	// Display the string.
	return os_stream << sval_value.estrStringValue << " -> " << (CBaseValue) sval_value;
}


//**********************************************************************************************
//
istream& operator>>
(
	istream&	  is_stream,
	CStringValue& sval_value
)
{
	// Read in the string.
	is_stream >> sval_value.estrStringValue;

	// Pass the input stream along.
	return is_stream;
}


//******************************************************************************************
//
uint CStringValue::uWriteCount
(
)
{
	// Return the number of bytes required.
	return CBaseValue::uWriteCount() + estrStringValue.uWriteCount();
}


//******************************************************************************************
//
uint CStringValue::uWrite
(
	char** ppc_buffer
)
{
	// Start by writing the base value record.
	uint u_count = CBaseValue::uWrite(ppc_buffer);

	// Write the string value.
	u_count += estrStringValue.uWrite(ppc_buffer);

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
uint CStringValue::uRead
(
	char** ppc_buffer
)
{
	// Start by reading the base value record.
	uint u_count = CBaseValue::uRead(ppc_buffer);

	// Read in the string value.
	u_count += estrStringValue.uRead(ppc_buffer);

	// Return the number of bytes read in.
	return u_count;
}


//******************************************************************************************
//
CObjectValue::CObjectValue
(
) : CBaseValue() 
{ 
}


//******************************************************************************************
//
CObjectValue::CObjectValue
(
	const EVarScope	evs_var_scope,		// Scope of visibility of this object.
	const CHandle	h_symbol_handle		// Handle to the symbol for this object.
) : CBaseValue(evtOBJECT, evs_var_scope, h_symbol_handle)
{ 
}


//******************************************************************************************
//
CObjectValue::~CObjectValue
(
) 
{ 
	// Remove all the entries from the object handle list.
	ahHandleList.clear();
}


//******************************************************************************************
//
uint CObjectValue::uCount
(
) 
{ 
	// Return the number of handles in the list.
	return ahHandleList.size();
}


//******************************************************************************************
//
uint CObjectValue::uAddElement
(
	const CHandle& h_handle
)
{
	// Add this handle to the end of the list.
	ahHandleList.push_back(h_handle);

	// Return the number of handles in the list.
	return ahHandleList.size();
}


//******************************************************************************************
//
const CHandle& CObjectValue::hElementHandle
(
	const uint u_index
)
{
	// Is this a valid index?
	if (u_index < ahHandleList.size())
	{
		// Yes! Return the handle.
		return ahHandleList[u_index];
	}

	// Our handle is invalid so return an invalid handle.
	return hNULL_HANDLE;
}


//******************************************************************************************
//
CObjectValue& CObjectValue::operator=
(
	const CObjectValue& oval_value
) 
{ 
	// Setup this value entry.
	if (this == &oval_value)
	{
		// We are assigning to ourselves.
		return *this;
	}

	// Copy all the fields in the base class.
	Type(oval_value.evtType());
	Scope(oval_value.evsScope());
	RefState(oval_value.ersRefState());
	Handle(oval_value.hHandle());
	Symbol(oval_value.hSymbol());
	Target(oval_value.hTarget());

	//
	// we need to copy all the handles from the source object to the dest object.  Start out
	// by removing any handles from the destination list.
	//
	ahHandleList.clear();

	// Copy the handle list.
	for (uint u_index = 0; u_index < oval_value.ahHandleList.size(); u_index++)
	{
		ahHandleList.push_back(oval_value.ahHandleList[u_index]);
	}

	// Return this record.
	return *this;
}


//**********************************************************************************************
//
ostream& operator<<
(
	ostream&		os_stream, 
	CObjectValue&	oval_value
)
{
	// Display the string.
	os_stream << "(" << oval_value.ahHandleList.size();

	// Display all of the handles.
	for (uint u_index = 0; u_index < oval_value.ahHandleList.size(); u_index++)
	{
		// Display the handle.
		os_stream << ", " << oval_value.ahHandleList[u_index];
	}

	return os_stream << ")" << " -> " << (CBaseValue) oval_value;
}


//**********************************************************************************************
//
istream& operator>>
(
	istream&	 is_stream,
	CObjectValue& estr_string
)
{
	// Pass the input stream along.
	return is_stream;
}


//******************************************************************************************
//
uint CObjectValue::uWriteCount
(
)
{
	// Determine the byte count for the base class and the handle count.
	uint u_count = CBaseValue::uWriteCount() + sizeof(uint);
	
	// Add the size of all the handles.
	return u_count += ahHandleList.size() * CHandle::uWriteCount();
}


//******************************************************************************************
//
uint CObjectValue::uWrite
(
	char** ppc_buffer
)
{
	// Start by writing the base value record.
	uint u_count = CBaseValue::uWrite(ppc_buffer);

	// Write the handle count and update the byte count.
	*(*((int **) ppc_buffer))++ = ahHandleList.size();
	u_count += sizeof(uint);

	// Write each of the handles.
	for (uint u_index = 0; u_index < ahHandleList.size(); u_index++)
	{
		// Write out the handle.
		u_count += ahHandleList[u_index].uWrite(ppc_buffer);
	}

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
uint CObjectValue::uRead
(
	char** ppc_buffer
)
{
	// Start by writing the base value record.
	uint u_count = CBaseValue::uRead(ppc_buffer);

	// Read the handle count and update the byte count.
	uint u_handle_count = *(*((int **)ppc_buffer))++;
	u_count += sizeof(uint);

	// Make sure the handle list is empty.  Is it?
	if (ahHandleList.size())
	{
		// No! Then empty it.
		ahHandleList.clear();
	}

	// Set the allocation.
	ahHandleList.SetAllocation(u_handle_count);

	// Read each of the handles.
	for (uint u_index = 0; u_index < u_handle_count; u_index++)
	{
		// Read in the handle.
		CHandle h_handle;
		u_count += h_handle.uRead(ppc_buffer);

		// Add it to the list.
		ahHandleList.push_back(h_handle);
	}

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
CValueTable::CValueTable
(
)
{
}


//******************************************************************************************
//
CValueTable::CValueTable
(
	CEasyString	  estr_module_name
	) : CNewSymbolTable(estr_module_name+".Sym"), estrValueName(estr_module_name)
{
	// Setup the handle manager for the value table entries.
	objhHandle = hmgrManager.objhCreate(estrValueName+".Val");
}


//******************************************************************************************
//
CValueTable::~CValueTable
(
)
{
	// Delete the symbol table manager.  Were we successful?
	if (!hmgrManager.bDelete(estrValueName+".Val"))
	{
		// No! Reoprt an error for now.
		printf("Unable to delete the handle manager for the value table.\n");
	}

	// Clean up all those value table values.
	map< CHandle, CBaseValue*, less<CHandle> >::iterator i = aviValueIndex.begin();
	for ( ; i != aviValueIndex.end(); i++)
	{
		delete (*i).second;
	}

	// Free up the symbol table index and handle index.
	aviValueIndex.erase(aviValueIndex.begin(), aviValueIndex.end());

	// Erase the symbol table name.
	estrValueName = "";
}


//******************************************************************************************
//
CHandle CValueTable::hBaseValue
(
	CBaseValue* pbasev_value
)
{
	// Setup a handle to the value table entry.
	CHandle h_handle = objhHandle.hNext();
	
	// Setup a handle to this value record.
	pbasev_value->Handle(h_handle);

	// Add this new value record to the end of the list.
	aviValueIndex[h_handle] = pbasev_value;
	
	// Were we able to add the new element to the list?
	if (aviValueIndex[h_handle] == pbasev_value)
	{
		// Yes! Aetup the pointers to the elements.
		return h_handle;
	}

	// No! Return an invalid handle.
	return hNULL_HANDLE;
}


//******************************************************************************************
//
CBaseValue& CValueTable::operator[]
(
	const CHandle& h_handle
)
{
	//
	// Search the table for the symbol.
	//

	// Is this symbol in the table yet?
	CBaseValue* pvb_value_rec = aviValueIndex[h_handle];

	// Was the symbol in the table?
	if (pvb_value_rec)
	{
		// Yes! Return the handle.
		return *pvb_value_rec;
	}
	
	// We didn't find our handle in the table, so return a null.
	return bvalNull;
}


//******************************************************************************************
//
uint CValueTable::uWriteCount
(
)
{
	// Add the value table name length to the count.
	uint u_count = estrValueName.uWriteCount();

	// Add in the size of the symbol table.
	u_count += CNewSymbolTable::uWriteCount();

	// Add in the object handle manager.
	u_count += objhHandle.uWriteCount();


	// Add in the count for represent the number of value entries.
	u_count += sizeof(uint);

	// Loop through all the value records and records their amounts.
	map< CHandle, CBaseValue*, less<CHandle> >::iterator i;
	for (i = aviValueIndex.begin(); i != aviValueIndex.end(); i++)
	{
		//
		// First determine if a base value entry exists for this handle.  For now, we will
		// delete any value handle map entries which do not contains valid base value
		// records since this would be indicative of a compiler problem.
		//
		CBaseValue* pbasev_value = (*i).second;

		// Were we successful?
		if (pbasev_value)
		{
			// Add in the amount for the handle.
			CHandle h_handle = (*i).first;
			u_count += h_handle.uWriteCount();

			// Add in the amount for the DERIVED value record.
			u_count += pbasev_value->uWriteCount();
		}
		else
		{
			// No! There is not base value entry here.  Remove this mapping entry.
			aviValueIndex.erase(i);
		}
	}

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
uint CValueTable::uWrite
(
	char** ppc_buffer
)
{
	// Add the value table name length to the count.
	uint u_count = estrValueName.uWrite(ppc_buffer);

	// Write out the symbol table.
	u_count += CNewSymbolTable::uWrite(ppc_buffer);

	// Add in the object handle manager.
	u_count += objhHandle.uWrite(ppc_buffer);

	// Loop through all the value records and remove any degenerate records.
	uint u_record_count = 0;
	map< CHandle, CBaseValue*, less<CHandle> >::iterator i;
	for (i = aviValueIndex.begin(); i != aviValueIndex.end(); i++)
	{
		//
		// First determine if a base value entry exists for this handle.  For now, we will
		// delete any value handle map entries which do not contains valid base value
		// records since this would be indicative of a compiler problem.
		//
		CBaseValue* pbasev_value = (*i).second;

		// Were we successful?
		if (pbasev_value)
		{
			// Increment the valid record count.
			u_record_count++;
		}
		else
		{
			// No! There is not base value entry here.  Remove this value index.
			aviValueIndex.erase(i);
		}
	}

	// Write out the value count and increment the counter.
	*(*((uint **) ppc_buffer))++ = u_record_count;
	u_count += sizeof(uint);

	// Loop through all the value records and write each one out.
	for (i = aviValueIndex.begin(); i != aviValueIndex.end(); i++)
	{
		//
		// First determine if a base value entry exists for this handle.  For now, we will
		// delete any value handle map entries which do not contains valid base value
		// records since this would be indicative of a compiler problem.
		//
		CBaseValue* pbasev_value = (*i).second;

		// Were we successful?
		if (pbasev_value)
		{
			// Add in the amount for the DERIVED value record.
			u_count += pbasev_value->uWrite(ppc_buffer);

			// Add in the amount for the handle.
			CHandle h_handle = (*i).first;
			u_count += h_handle.uWrite(ppc_buffer);
		}
	}

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
 uint CValueTable::uRead
(
	char** ppc_buffer
)
{
	// Start by loading the value table name.
	uint u_count = estrValueName.uRead(ppc_buffer);

	// Read in the symbol table.
	u_count += CNewSymbolTable::uRead(ppc_buffer);

	// Read in the object handle manager.
	u_count += objhHandle.uRead(ppc_buffer);

	// Attempt to add it to the handle manager.  Were we successful?
	if (!hmgrManager.bSetup(objhHandle))
	{
		// No!  Return the current count and the error will be caught.
		return u_count;
	}

	// Read in the value record count and increment the counter.
	uint u_record_count = *(*((uint **) ppc_buffer))++;
	u_count += sizeof(uint);

	// Loop through all the value records and write each one out.
	for (uint u_index = 0; u_index < u_record_count; u_index++)
	{
		// We will need to setup a pointer to the base class for insertion into the handle index.
		CBaseValue* pbasev_value = 0;

		// Now read in the value record variable type so we know what to construct.
		switch(**ppc_buffer)
		{
			case evtBOOL:
				// Allocate and integer value record.
				pbasev_value = new CBoolValue;

				break;

			case evtCHAR:
				// Allocate and integer value record.
				pbasev_value = new CCharValue;

				break;

			case evtINT:
				
				// Allocate and integer value record.
				pbasev_value = new CIntValue;

				break;

			case evtFLOAT:
				// Allocate and integer value record.
				pbasev_value = new CFloatValue;

				break;

			case evtSTRING:
				// Allocate and integer value record.
				pbasev_value = new CStringValue;

				break;

			case evtOBJECT:
				// Allocate and integer value record.
				pbasev_value = new CObjectValue;

				break;

			case evtNULL:
			case evtNODE:
				break;

			default:
				// This is bad.  This will be caught above us since the counts will not match.
				return u_count;
		}

		// Were we successful?
		if (pbasev_value == 0)
		{
			// No! Return the current count.
			return u_count;
		}

		// Read in the value record.
		u_count += pbasev_value->uRead(ppc_buffer);

		// Start out by reading in the handle record.
		CHandle h_handle;
		u_count += h_handle.uRead(ppc_buffer);

		// Now insert the value record into the list.
		aviValueIndex[h_handle] = pbasev_value;
	}

	// Return the number of bytes written.
	return u_count;
}


//******************************************************************************************
//
void CValueTable::Dump
(
)
{
	// Print out a banner.
	cout << "CValueTable(\"" << estrValueName << "\", hobj:" << objhHandle << endl;

	// Dump the symbol table.
	CNewSymbolTable::Dump();

	// Are there any entries in the value table?

	cout << "\nValue table entries: " << aviValueIndex.size() << endl;

	// Dump the value table.
	uint u_index;
	map< CHandle, CBaseValue*, less<CHandle> >::iterator i;
	for (u_index = 0, i = aviValueIndex.begin(); i != aviValueIndex.end(); i++, u_index++)
	{
		// Get a local copy of the symbol entry.
		CBaseValue* pbasev_value = (*i).second;

		// Were we successful?
		if (pbasev_value)
		{
			// Yes! Then dump the record.
			switch(pbasev_value->evtType())
			{
				case evtBOOL:
					// Allocate and integer value record.
					cout << setw(4) << u_index << ": " << *((CBoolValue *) pbasev_value) << endl;

					break;

				case evtCHAR:
					// Allocate and integer value record.
					cout << setw(4) << u_index << ": " << *((CCharValue *) pbasev_value) << endl;

					break;

				case evtINT:
					
					// Allocate and integer value record.
					cout << setw(4) << u_index << ": " << *((CIntValue *) pbasev_value) << endl;

					break;

				case evtFLOAT:
					// Allocate and integer value record.
					cout << setw(4) << u_index << ": " << *((CFloatValue *) pbasev_value) << endl;

					break;

				case evtSTRING:
					// Allocate and integer value record.
					cout << setw(4) << u_index << ": " << *((CStringValue *) pbasev_value) << endl;

					break;

				case evtOBJECT:
					// Allocate and integer value record.
					cout << setw(4) << u_index << ": " << *((CObjectValue *) pbasev_value) << endl;

					break;

				default:
					break;
			}
		}
	}

	// Close off the opening banner.
	cout << ")\n" << endl;
}
