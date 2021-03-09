/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1996
 *
 * Contents:
 *		This file contains methods for management of a symbol table.
 * 
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/Symtab.cpp                                                   $
 * 
 * 8     6/18/97 7:35p Gstull
 * Added changes to support fast exporting.
 *
 *********************************************************************************************/

#include <string.h>

#ifdef USE_MAX_TYPES
#include "StandardTypes.hpp"
#include "Tools/GroffExp/GUIInterface.hpp"

extern CGUIInterface guiInterface;

#else
#include "common.hpp"
#endif

#include "Lib/Sys/SysLog.hpp"
#include "Lib/Sys/Symtab.hpp"


// Contructor for the symbol table
CSymTab::CSymTab()
{
	// Initialize the symbol table list to empty
	pstHead          = 0;
	SymtabImage      = 0;
	uSymtabImageSize = 0;

#ifdef USE_MAX_TYPES
	// Determine whether the file should be active or not.
	if (guiInterface.bGenerateLogfiles())
	{
		// Open the log file.
		char str_logfile[256];

		// Construct the proper path for the logfile.
		guiInterface.BuildPath(str_logfile, guiInterface.strGetLogfileDirPath(), "Symtab.log");
		slLogfile.Open(str_logfile);

		// Activate the logfile.
		slLogfile.Enable();
	}
	else
	{
		// Deactivate the logfile.
		slLogfile.Disable();
	}
#else
	slLogfile.Disable();
#endif
}


// Contructor for the symbol table
CSymTab::CSymTab(void* symtab, uint u_size)
{
	// Initialize the symbol table list to empty
	pstHead = NULL;

	SymtabImage      = 0;
	uSymtabImageSize = 0;

	// Attempt to build a symbol table from an image.  Were we successful?
	if (!bBuildSymtab(symtab, u_size))
	{
		// No! Reset everything and return.
		Initialize();
	}
}


// Destructor for the symbol table
CSymTab::~CSymTab()
{
	// Destroy the symbol table if it exists.
	Initialize();
}


// Destructor for the symbol table
void CSymTab::Initialize()
{
	// Prepare to destroy the linked list
	SSymtab* pstTmp = pstHead;

	// Loop through the list and deallocate each of the nodes
	while(pstTmp)
	{
		// Advance the head pointer
		pstHead = pstHead->pstNext;

		// Delete the node
		delete pstTmp;

		// Update the temporary pointer;
		pstTmp = pstHead;
	}
}


bool CSymTab::bBuildSymtab(void* symtab, uint u_size)
{
	// Do we already have an existing symbol table?
	if (pstHead != 0)
	{
		// Yes!  Then delete it and start from scratch.
		Initialize();

		// Set the pointer to null.
		pstHead = 0;
	}

	// Setup a check to verify we just end up at the end of the symbol table image
	// once the table has been reconstructed.
	void*	end_of_block = (void *) (((char *) symtab) + u_size);
	void*	data		 = symtab;

	// Now attempt to construct the symbol table.
	uint u_symbol_count = *((uint *) symtab);
	data = (void *) (((char *) data) + 4);

	// Do we have a valid symbol table?
	if (u_size == 0 || u_symbol_count == 0)
	{
		// No! Return and error.
		return false;
	}

	SSymtab* pst_node = pstHead;

	// Build the table.
	for (uint u_i = 0; u_i < u_symbol_count; u_i++)
	{
		//
		// Attempt to add a new symtab node to the list.  
		//

		// Does the list already contain symbols?
		if (pstHead != 0)
		{
			// Yes!  Add it to the last allocated node.
			pst_node->pstNext = new SSymtab;
			pst_node		  = pst_node->pstNext;
		}
		else
		{
			// No!  Then this must be the first node.
			pst_node = new SSymtab;

			// Now setup the head of the list to point to this node.
			pstHead = pst_node;
		}
		
		// Were we successful?
		if (pst_node == 0)
		{
			// No!  Deallocate what we have and return an error.
			Initialize();

			return false;
		}
	
		// Terminate the list.
		pst_node->pstNext = 0;

		// Attempt to allocate a symbol record.
		pst_node->pseSymbol = new SSymbolRecord;

		// Were we successful?
		if (pst_node->pseSymbol == 0)
		{
			// No! Deallocate the list then return an error.
			Initialize();

			return false;
		}

		// Assign the symbol handle. 
		pst_node->pseSymbol->shHandle = *((TSymbolHandle *) data);
		data = (void *) (((char *) data) + 4);

		// Assign the string length.
		pst_node->pseSymbol->uNameLength = *((uint *) data);
		data = (void *) (((char *) data) + 4);

		uint u_strlen = pst_node->pseSymbol->uNameLength;

		// Allocate a string.
		pst_node->pseSymbol->strName = new char[u_strlen];

		// Were we successful?
		if (pst_node->pseSymbol->strName == 0)
		{
			// No! Deallocate the symbol table and return an error.
			Initialize();

			return false;
		}

		// Copy the string from the symbol table into the symbol.
		strcpy(pst_node->pseSymbol->strName, (char *) data);

		// Advance the data pointer.
		data = (void *) (((char *) data) + u_strlen);
	}

	// If everything was OK, pointer 'data' should be equal to 'end_of_block': Is it?
	return data == end_of_block;
}


SSymtab* CSymTab::NewNode( 
	char*		str_name,			// The actual symbol
	SSymtab*	pst_next)			// The pointer to the next node in the list
{
	// Attempt to allocate a symbol table
	SSymtab* pst_node = new SSymtab;

	// Were we successful?
	if (pst_node == 0)
	{
		// No! Return an error.
		return 0;
	}

	// Attempt to allocate a symbol record.
	SSymbolRecord* pse_symbol = new SSymbolRecord;

	// Were we successful?
	if (pse_symbol == 0)
	{
		// No! Deallocate the symbol list node, then return an error.
		delete pst_node;

		return 0;
	}

	// Allocate a symbol entry node
	pst_node->pseSymbol = pse_symbol;

	// Allocate memory for the string
	pse_symbol->uNameLength = strlen(str_name)+1;
	pse_symbol->strName		= new char[pse_symbol->uNameLength];

	// Setup the node.
	strcpy(pse_symbol->strName, str_name);
	pse_symbol->shHandle = (TSymbolHandle) pse_symbol;
	pst_node->pstNext	 = pst_next;


	// Return this node
	return pst_node;
}


// Insert a new symbol into the list in sorted order.
TSymbolHandle CSymTab::shInsert(char* str_name)
{
	SSymtab* pst_node = pstHead;
	
	// Is the list empty?
	if (pstHead == NULL)
	{
		// Setup the node
		pstHead = NewNode(str_name, NULL);

		// Return the handle to this symbol
		return pstHead->pseSymbol->shHandle;
	}
	// Is this symbol to be placed at the front of the list?
	else if (strcmp(str_name, pst_node->pseSymbol->strName) < 0)
	{
		// Setup the node
		pstHead = NewNode(str_name, pstHead);

		// return the handle to this symbol
		return pstHead->pseSymbol->shHandle;
	}
	else
	{
		// Look through the list to determine the best location for this symbol.
		while(pst_node->pstNext != NULL)
		{
			// Should the new symbol be inserted here?
			if (strcmp(str_name, pst_node->pstNext->pseSymbol->strName) < 0 )
			{
				// Yes!  So create a new node here ...
				pst_node->pstNext = NewNode(str_name, pst_node->pstNext);
				
				// Return the handle to this symbol
				return pst_node->pstNext->pseSymbol->shHandle;
			}

			// Advance to the next node in the list
			pst_node = pst_node->pstNext;
		}

		// Well, it isn't in the list so add it to the end.
		pst_node->pstNext = NewNode(str_name, NULL);
	}

	// Return the handle to the symbol
	return pst_node->pstNext->pseSymbol->shHandle;
}
	
	
// Delete a symbol from the table
bool CSymTab::bDelete(char* str_name)
{
	int		 i_result;
	SSymtab* pst_tmp;
	SSymtab* pst_node = pstHead;


	// Start looking for this node in the list.  Is the list empty?
	if (pstHead == NULL)
	{
		// Yes!  This symbol is not in the table so return an error.
		return false;
	}

	// Compare the two strings
	i_result = strcmp(pst_node->pseSymbol->strName, str_name);

	// Is the symbol at the head of the list?
	if (i_result == 0)
	{
		// Yes! Get rid of this node.
		pstHead = pstHead->pstNext;

		// Now delete the node
		delete pst_node->pseSymbol;
		delete pst_node;

		// Return to the caller with a successful result.
		return true;
	}
	// Have we already passed this node?
	else if (i_result > 0)
	{
		// Yes!  It is not in the list, so return an error.
		return false;
	}

	// Look through the symbol table for the node
	while(pst_node->pstNext != NULL)
	{
		// Compare the two strings
		i_result = strcmp(pst_node->pstNext->pseSymbol->strName, str_name);

		// Did we find a match?
		if (i_result == 0)
		{
			// Setup a pointer to the node.
			pst_tmp = pst_node->pstNext;

			// Delete the node from the list.
			pst_node->pstNext = pst_tmp->pstNext;

			// Deallocate the node
			delete pst_tmp->pseSymbol;
			delete pst_tmp;

			// Return a successful result.
			return true;
		}

		// Advance to the next node
		pst_node = pst_node->pstNext;
	}

	// the symbol was not found so return an error
	return false;
}


// Delete a symbol from the table
bool CSymTab::bDelete(TSymbolHandle sh_handle)
{
	SSymtab* pst_tmp;
	SSymtab* pst_node = pstHead;

	// Start looking for this node in the list.  Is the list empty?
	if (pstHead == NULL)
	{
		// Yes!  This symbol is not in the table so return an error.
		return false;
	}

	// Is the symbol at the head of the list?
	if (pstHead->pseSymbol->shHandle == sh_handle)
	{
		// Yes! Get rid of this node.
		pstHead = pstHead->pstNext;

		// Now delete the node, First do the symbol entry, then the node
		delete pst_node->pseSymbol;
		delete pst_node;

		// Return to the caller with a successful result.
		return true;
	}

	// Look through the symbol table for the node
	while(pst_node->pstNext != NULL)
	{
		// Did we find a match?
		if (pst_node->pstNext->pseSymbol->shHandle == sh_handle)
		{
			// Setup a pointer to the node.
			pst_tmp = pst_node->pstNext;

			// Delete the node from the list.
			pst_node->pstNext = pst_tmp->pstNext;

			// Deallocate the node
			delete pst_tmp->pseSymbol;
			delete pst_tmp;

			// Return a successful result.
			return true;
		}

		// Advance to the next node
		pst_node = pst_node->pstNext;
	}

	// the symbol was not found so return an error
	return false;
}


TSymbolHandle CSymTab::shLookup(char* str_name)
{
	SSymtab* pst_node = pstHead;


	// Start looking for this node in the list.  Is the list empty?
	if (pstHead == NULL)
	{
		// Yes!  This symbol is not in the table so return an error.
		return 0;
	}

	// Is the symbol at the head of the list?
	if (strcmp(pst_node->pseSymbol->strName, str_name) == 0)
	{
		// Yes! Return a handle to this symbol.
		return pstHead->pseSymbol->shHandle;
	}

	// Look through the symbol table for the node
	while(pst_node->pstNext != NULL)
	{
		// Did we find a match?
		if (strcmp(pst_node->pstNext->pseSymbol->strName, str_name) == 0)
		{
			// Return the symbols handle.
			return pst_node->pstNext->pseSymbol->shHandle;
		}

		// Advance to the next node
		pst_node = pst_node->pstNext;
	}


	// the symbol was not found so return an error
	return 0;
}


const char* CSymTab::strLookup(TSymbolHandle sh_handle)
{
	SSymtab* pst_node = pstHead;


	// Start looking for this node in the list.  Is the list empty?
	if (pstHead == NULL)
	{
		// Yes!  This symbol is not in the table so return an error.
		return 0;
	}

	// Is the symbol at the head of the list?
	if (pst_node->pseSymbol->shHandle == sh_handle)
	{
		// Yes! Return a handle to this symbol.
		return pstHead->pseSymbol->strName;
	}

	// Look through the symbol table for the node
	while(pst_node != NULL)
	{
		// Did we find a match?
		if (pst_node->pseSymbol->shHandle == sh_handle)
		{
			// Return the symbols handle.
			return pst_node->pseSymbol->strName;
		}

		// Advance to the next node
		pst_node = pst_node->pstNext;
	}

	// the symbol was not found so return an error
	return NULL;
}


bool CSymTab::bData(void** symtab, uint* u_size)
{
	// Does this table have any symbols in it?
	if (pstHead == 0)
	{
		// No! Allocate a small node with a zero in it, to represent 0 symbols.
		SymtabImage = new char[4];

		// Were we successful?
		if (SymtabImage == 0)
		{
			// No! Set the size to 0 since we were unsuccessful.
			uSymtabImageSize = 0;

			return false;
		}

		// Yes! Set the size to 4 to contain the value 0.
		uSymtabImageSize = 4;
		*((uint *) SymtabImage) = 0;

		// Setup the return variables.
		*symtab = SymtabImage;
		*u_size  = uSymtabImageSize;

		// Return to the caller.
		return true;
	}

	// Yes!  Determine how much memory we need to store the symbol table.
	SSymtab* pst_node			 = pstHead;
	uint     u_symtab_image_size = 4;			// Include memory for the symbol table count.
	uint	 u_symbol_count		 = 0;

	// Loop through the symbol table list.
	while (pst_node != 0)
	{
		// Add the amount of memory required for the handle, length and string + null character.
		u_symtab_image_size += pst_node->pseSymbol->uNameLength + 8;

		// Increment the symbol counter.
		u_symbol_count++;

		// Advance to the next node in the list.
		pst_node = pst_node->pstNext;
	}

	// If there was an old image, and it is big enough reuse it otherwise allocate a new image buffer.
	if (u_symtab_image_size <= uSymtabImageSize)
	{
		// The existing buffer can be reused so reuse it.
		uSymtabImageSize = u_symtab_image_size;
	}
	else
	{
		// The current buffer is either too small or 0 in length, requiring a new buffer to be built.
		if (SymtabImage != 0)
		{
			// Return this buffer to the memory manager.
			delete [] SymtabImage;
			SymtabImage = 0;

			// Set the image size to 0.
			uSymtabImageSize = 0;
		}

		// Attempt to allocate the memory for the symbol table image.
		uSymtabImageSize = u_symtab_image_size;
		SymtabImage = new char[uSymtabImageSize];

		// Were we successful?
		if (SymtabImage == 0)
		{
			// No! Set the symbol table image size to 0.
			uSymtabImageSize = 0;

			return false;
		}
	}

	// Setup a data pointer to the symbol table image.
	void* data = SymtabImage;

	// Save the symbol count and advance the symbol pointer.
	*((uint *) data) = u_symbol_count;
	data = (void *) (((char *) data) + 4);

	// Reset the symbol list pointer.
	pst_node = pstHead;

	// Loop through the symbol table list and construct the image.
	while (pst_node != 0)
	{
		// Save the symbol handle.
		*((TSymbolHandle *) data) = pst_node->pseSymbol->shHandle;
		data = (void *) (((char *) data) + 4);

		// Save the string length.
		*((uint *) data) = pst_node->pseSymbol->uNameLength;
		data = (void *) (((char *) data) + 4);

		// Save the string itself.
		strcpy((char *) data, pst_node->pseSymbol->strName);
		data = (void *) (((char *) data) + pst_node->pseSymbol->uNameLength);
		
		// Advance to the next node in the list.
		pst_node = pst_node->pstNext;
	}

	// Perform a sanity check to verify that we were successful.
	if (((char *) SymtabImage) + uSymtabImageSize == data)
	{
		// Yes, things worked out correctly.
		*symtab = SymtabImage;
		*u_size  = uSymtabImageSize;

		return true;
	}
	else
	{
		// No! Something went wrong.
		*symtab = 0;
		*u_size  = 0;

		return false;
	}
}


uint CSymTab::uCount()
{
	// Yes!  Determine how much memory we need to store the symbol table.
	SSymtab* pst_node			 = pstHead;
	uint	 u_symbol_count		 = 0;

	// Loop through the symbol table list.
	while (pst_node != 0)
	{
		// Increment the symbol counter.
		u_symbol_count++;

		// Advance to the next node in the list.
		pst_node = pst_node->pstNext;
	}

	// Return the symbol table.
	return u_symbol_count;
}


const char* CSymTab::operator[](uint u_index)
{
	// Yes!  Determine how much memory we need to store the symbol table.
	SSymtab* pst_node			 = pstHead;
	uint	 u_symbol_count		 = 0;

	// Loop through the symbol table list.
	while (pst_node != 0)
	{
		// Are we at the correct node yet?
		if (u_index == u_symbol_count)
		{
			// Yes!  So return this string.
			return pst_node->pseSymbol->strName;
		}

		// Increment the symbol counter.
		u_symbol_count++;


		// Advance to the next node in the list.
		pst_node = pst_node->pstNext;
	}

	// We have run off of the end of the table, so return null.
	return 0;
}


void CSymTab::Dump()
{
	int			   i = 0;
	char		   str_message[256];
	SSymtab*	   pst_node = pstHead;
	SSymbolRecord* pse_symbol;
	

	// Loop through the symbol table and dump all the symbols
	slLogfile.Msg("Dumping symbol table...");
	slLogfile.Msg("");

	// Is the table empty?
	if (pst_node == NULL)
	{
		// Report this to the log
		slLogfile.Msg("<EMPTY>");
	}
	else
	{
		// Begin dumping the symbol table.
		while(pst_node != NULL)
		{
			// Dump the symbol information to the logfile.
			pse_symbol = pst_node->pseSymbol;
			sprintf(str_message, "Table entry %d: Handle: 0x%X, Name :%s:", i++, pse_symbol->shHandle, pse_symbol->strName);
			slLogfile.Msg(str_message);

			// Move to the next node in the list.
			pst_node = pst_node->pstNext;
		}

		// Dump the symbol table trailer
		slLogfile.Msg("");
		slLogfile.Msg("----- Symbol Table dump complete -----");
	}
}
