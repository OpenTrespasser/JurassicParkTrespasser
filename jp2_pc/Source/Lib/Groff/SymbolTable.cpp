/***********************************************************************************************
 *
 * Copyright (c) 1997 DreamWorks Interactive
 *
 * Contents: Class for managing a symbol table used during parsing and code generation.
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
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/SymbolTable.cpp                                             $
 * 
 * 3     8/16/98 8:37p Mmouni
 * Fixed problem with returning a reference to a temporary.
 * 
 * 2     5/13/97 1:17p Rwyatt
 * Renamed CSymbolTable to CNewsymbolTable to prevent clashes with the class of the same name
 * defined in SymTab.cpp/hpp
 * 
 * 1     5/12/97 9:58p Gstull
 * Classes for symbol table management,
 *
 **********************************************************************************************/

#include <iostream.h>
#include <iomanip.h>

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
#include "SymbolTable.hpp"
#include "ObjectHandle.hpp"

#else

#include "Common.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "Lib/Groff/SymbolTable.hpp"
#include "Lib/Groff/ObjectHandle.hpp"

#endif


//******************************************************************************************
//
CSymbolEntry::CSymbolEntry
(
) : estrSymbolName("")
{
}


//******************************************************************************************
//
CSymbolEntry::CSymbolEntry
(
	const CEasyString&	estr_symbol_name,
	const CHandle&		h_handle
) : estrSymbolName(estr_symbol_name), hSymbolHandle(h_handle)
{
}


CSymbolEntry& CSymbolEntry::operator=
(
	const CSymbolEntry& se_symbol
)
{
	estrSymbolName = se_symbol.estrSymbolName;
	hSymbolHandle  = se_symbol.hSymbolHandle;

	return *this;
}


//**********************************************************************************************
//
ostream& operator<<
(
	ostream&			os_stream, 
	const CSymbolEntry&	se_symbol
)
{
	// Display the symbol entry record.
	return os_stream << "CSymbolEntry(\"" << se_symbol.estrSymbolName << "\", " << se_symbol.hSymbolHandle << ")";
}


//**********************************************************************************************
//
istream& operator>>
(
	istream&	  is_stream,
	CSymbolEntry& se_symbol
)
{
	// Display the string.
	cout << "CSymbolEntry...\nCEasyString: " << flush; 
	cin >> se_symbol.estrSymbolName;
	cin >> hex >> se_symbol.hSymbolHandle;
	cin.setf(ios::dec);

	return is_stream;
}


//******************************************************************************************
//
CEasyString& CSymbolEntry::estrSymbol
(
)
{
	// Return the current reference count.
	return estrSymbolName;
}


//******************************************************************************************
//
CHandle& CSymbolEntry::hHandle
(
)
{
	// Return the current reference count.
	return hSymbolHandle;
}


//******************************************************************************************
//
uint CSymbolEntry::uCount
(
)
{
	// Return the current reference count.
	return hSymbolHandle.uCount();
}


//******************************************************************************************
//
CSymbolEntry& CSymbolEntry::operator+=
(
   uint u_reference_count
)
{
	// Increment the reference count for this symbol.
	hSymbolHandle += u_reference_count;

	return *this;
}


//******************************************************************************************
//
CSymbolEntry& CSymbolEntry::operator-=
(
	uint u_reference_count
)
{
	// Increment the reference count for this symbol.
	hSymbolHandle -= u_reference_count;

	return *this;
}


//******************************************************************************************
//
CSymbolEntry CSymbolEntry::operator++
(
   int
)
{
	// Increment the reference count for this symbol.
	hSymbolHandle++;

	return *this;
}


//******************************************************************************************
//
CSymbolEntry CSymbolEntry::operator--
(
	int
)
{
	// Increment the reference count for this symbol.
	hSymbolHandle--;

	return *this;
}


//******************************************************************************************
//
bool CSymbolEntry::operator==
(
	const CEasyString& estr_symbol		// Character string containing the symbol.
)
{
	// Is this the symbol we are looking for?
	return estrSymbolName == estr_symbol;
}


//******************************************************************************************
//
bool CSymbolEntry::bDelete
(
	const CEasyString& estr_symbol		// Easystring containing the symbol name.
)
{
	//
	// Make sure that the handle matches the symbol in this entry.  If it doesn't return an
	// unsuccessful result to the caller.
	//
	bool b_result = false;

	// Does the symbol match the symbol in this field?
	if (estrSymbolName == estr_symbol)
	{
		// Yes! Decrement the reference count to this symbol.
		hSymbolHandle--;

		// Setup a successful result.
		b_result = true;
	}

	// Return the result.
	return b_result;
}


//******************************************************************************************
//
uint CSymbolEntry::uWriteCount
(
)
{
	// Return the number of bytes required to represent the string in memory.
	return estrSymbolName.uWriteCount() + hSymbolHandle.uWriteCount();
}


//******************************************************************************************
//
uint CSymbolEntry::uWrite
(
	char** ppc_buffer
)
{
	// Write the object into the buffer.
	uint u_count = estrSymbolName.uWrite(ppc_buffer);

	// Write the symbol handle out.
	u_count += hSymbolHandle.uWrite(ppc_buffer);

	// Return the number of bytes placed into the buffer.
	return u_count;
}


//******************************************************************************************
//
uint CSymbolEntry::uRead
(
	char** ppc_buffer
)
{
	// Write the object into the buffer.
	uint u_count = estrSymbolName.uRead(ppc_buffer);

	// Write the symbol handle out.
	u_count += hSymbolHandle.uRead(ppc_buffer);

	// Return the number of bytes placed into the buffer.
	return u_count;
}


//******************************************************************************************
//
void CSymbolEntry::Dump
(
)
{
	// Dump the CHandle to stdout.
	cout << *this << endl;
}


//******************************************************************************************
//
CNewSymbolTable::CNewSymbolTable
(
)
{
}


//******************************************************************************************
//
CNewSymbolTable::CNewSymbolTable
(
	const CEasyString& estr_tablename
)
{
	// Setup the handle manager.
	objhHandle = hmgrManager.objhCreate(estr_tablename);

	// Setup the symbol table name.
	estrTableName = estr_tablename;
}


//******************************************************************************************
//
CNewSymbolTable::~CNewSymbolTable
(
)
{
	// Delete the symbol table manager.
	if (!hmgrManager.bDelete(estrTableName))
	{
		printf("Unable to delete the handle manager for the symbol table.\n");
	}

	// Free up the symbol table index, handle table index and the symbol table itself.
	asiSymbolIndex.erase(asiSymbolIndex.begin(), asiSymbolIndex.end());
	asiHandleIndex.erase(asiHandleIndex.begin(), asiHandleIndex.end());
	aseSymbolTable.erase(aseSymbolTable.begin(), aseSymbolTable.end());

	// Erase the symbol table name.
	estrTableName = "";
}


//******************************************************************************************
//
const CHandle& CNewSymbolTable::operator[]
(
	const CEasyString& estr_symbol
)
{
	// Is this symbol in the table yet?
	CSymbolEntry* pse_symbol_rec = asiSymbolIndex[estr_symbol];

	// Was the symbol in the table?
	if (pse_symbol_rec)
	{
		// Yes! Return the handle.
		return pse_symbol_rec->hHandle();
	}
	
	// We didn't find our handle in the table, so return an invalid address.
	return hNULL_HANDLE;
}


//******************************************************************************************
//
const CEasyString& CNewSymbolTable::operator[]
(
	CHandle h_handle
)
{
	//
	// Search the table for the symbol.
	//

	// Is this symbol in the table yet?
	CSymbolEntry* pse_symbol_rec = asiHandleIndex[h_handle];

	// Was the symbol in the table?
	if (pse_symbol_rec)
	{
		// Yes! Return the handle.
		return pse_symbol_rec->estrSymbol();
	}
	
	// We didn't find our handle in the table, so return an invalid address.
	return estrNULL_STRING;
}


//******************************************************************************************
//
CHandle CNewSymbolTable::hSymbol
(
	const CEasyString& estr_symbol
)
{
	// Is this symbol in the table yet?
	CSymbolEntry* pse_symbol_rec = asiSymbolIndex[estr_symbol];

	// Was the symbol in the table?
	if (pse_symbol_rec)
	{
		// Yes! Then increment the reference counter and return the handle.
		(*pse_symbol_rec)++;

		return pse_symbol_rec->hHandle();
	}

	// Setup the pointers to the entries in the list.
	uint u_index = aseSymbolTable.size();
	
	// Get a new handle.
	CHandle h_handle = objhHandle.hNext();

	// Add this new symbol record to the end of the list.
	aseSymbolTable.push_back(CSymbolEntry(estr_symbol, h_handle));
	
	// Were we able to add the new element to the list?
	if (u_index < aseSymbolTable.size())
	{
		// Yes! Aetup the pointers to the elements.
		asiHandleIndex[h_handle]    = &aseSymbolTable[u_index];
		asiSymbolIndex[estr_symbol] = &aseSymbolTable[u_index];
	}
	else
	{
		// No! Return an invalid handle.
		h_handle = hNULL_HANDLE;
	}

	// Return the handle to the symbol entry.
	return h_handle;
}


//******************************************************************************************
//
uint CNewSymbolTable::uWriteCount
(
)
{
	//
	// Calculate the storage requirements for the symbol table by looping through the symbol
	// table and determining the size of symbol entry in the table.
	//
	
	// Add the symbol table object handle record size.
	uint u_count = objhHandle.uWriteCount();

	// Add the size of this symbol table name.
	u_count += estrTableName.uWriteCount();
	
	// Add the size of the symbol count.
	u_count += sizeof(uint);

	// Add up the storage requirements for each symbol table entry.
	uint u_symbol_count = aseSymbolTable.size();
	for (uint u_index = 0; u_index < u_symbol_count; u_index++)
	{
		// Determine how big is this symbol table entry is.
		u_count += aseSymbolTable[u_index].uWriteCount();
	}

	// Return the number of bytes required to represent the string in memory.
	return u_count;
}


//******************************************************************************************
//
uint CNewSymbolTable::uWrite
(
	char** ppc_buffer
)
{
	// Write out the object handle record for this table and increment the pointer.
	uint u_count = objhHandle.uWrite(ppc_buffer);

	// Write out the object handle record for this table and increment the pointer.
	u_count += estrTableName.uWrite(ppc_buffer);

	// Write out the symbol count. 
	uint u_entries = aseSymbolTable.size();
	*(*((uint **) ppc_buffer))++ = u_entries;

	// Write the number of symbol entries out to the image and advance the counter.
	uint u_symbol_count = aseSymbolTable.size();
	u_count += sizeof(uint);

	// Loop through the symbol table and write out each entry.
	for (uint u_index = 0; u_index < u_entries; u_index++)
	{
		// Determine how big is this symbol table entry is.
		u_count += aseSymbolTable[u_index].uWrite(ppc_buffer);
	}

	// Return the number of bytes required to represent the string in memory.
	return u_count;
}


//******************************************************************************************
//
uint CNewSymbolTable::uRead
(
	char** ppc_buffer
)
{
	// Write out the object handle record for this table and increment the pointer.
	uint u_count = objhHandle.uRead(ppc_buffer);

	// Attempt to install the symbol table's profile in the handle manager.  Were we successful?
	if (!hmgrManager.bSetup(objhHandle))
	{
		// No! return the current count and the error will be detected later.
		return u_count;
	}

	// Write out the object handle record for this table and increment the pointer.
	u_count += estrTableName.uRead(ppc_buffer);

	// Read in the symbol count.
	uint u_symbol_count = *(*((uint **) ppc_buffer))++;

	// Increment the counter.
	u_count += sizeof(uint);

	// Loop through the symbol table and write out each entry.
	for (uint u_index = 0; u_index < u_symbol_count; u_index++)
	{
		//
		// We need to construct a symbol entry record then insert it into the list.
		//
		CSymbolEntry se_entry;

		// Load the entry from the buffer.
		u_count += se_entry.uRead(ppc_buffer);

		// Is this symbol already in the table?
		CSymbolEntry* pse_symbol_rec = asiSymbolIndex[se_entry.estrSymbol()];

		// Was the symbol in the table?
		if (pse_symbol_rec)
		{
			// Yes! Then increment the reference counter and return the handle.
			*pse_symbol_rec += se_entry.uCount();
		}
		else
		{
			// No! Then we need to add it to the table
			uint u_index = aseSymbolTable.size();

			// Add this new symbol record to the end of the list.
			aseSymbolTable.push_back(se_entry);
			
			// Were we able to add the new element to the list?
			if (u_index < aseSymbolTable.size())
			{
				// Yes! Aetup the pointers to the elements.
				asiHandleIndex[se_entry.hHandle()]    = &aseSymbolTable[u_index];
				asiSymbolIndex[se_entry.estrSymbol()] = &aseSymbolTable[u_index];
			}
			else
			{
				// No! Assertion!  Report a big error.
				cout << "Unable to add symbol to list.  Aborting.";

				// Halt the program in a user break point.
				_asm
				{
					// Halt the program here.
					int	3;
				}
			}
		}
	}

	// Return the number of bytes required to represent the string in memory.
	return u_count;
}


//******************************************************************************************
//
void CNewSymbolTable::Dump()
{
	// Print out a banner.
	cout << "\nCNewSymbolTable(Name: \"" << estrTableName << "\", Count: " << (int) aseSymbolTable.size() << ")\n{";

	// Dump the table.
	for (uint u_index = 0; u_index < aseSymbolTable.size(); u_index++)
	{
		// Get a local copy of the symbol entry.
		cout << endl << setw(4) << u_index << ": " << aseSymbolTable[u_index];
	}

	cout << "\n}" << endl;
}
