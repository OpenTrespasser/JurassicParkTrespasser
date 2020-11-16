/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	 	Implementation of Hash.hpp
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/Hash.cpp                                                     $
 * 
 * 16    8/27/98 9:31p Asouth
 * MSL's map class requires the index to be const
 * 
 * 15    7/08/98 12:43a Rwyatt
 * New function to enable hashing across multiple blocks/strings
 * 
 * 14    3/17/98 1:41p Agrant
 * replaced !VER_FINAL with VER_TEST
 * 
 * 13    3/09/98 10:48p Rwyatt
 * New function to CRC a string without first getting its length
 * 
 * 12    3/02/98 5:48p Agrant
 * Added a verify hashing flag, true when we want to take a speed hit in order to verify that
 * we have no string hash collisions
 * 
 * 11    1/29/98 7:30p Rwyatt
 * u4Hash now uses a CCITT32 crc for more reliable results.
 * This file is initialized with the run time library so the hash function is available for
 * static constructors.
 * 
 * 10    12/02/97 12:41a Agrant
 * Improved hashing function for strings.
 * 
 * 9     12/01/97 5:02p Agrant
 * Better handling of hash failures in release mode.
 * 
 * 8     12/01/97 3:24p Agrant
 * Hash now remains the same across GUIApp sessions, and is not guaranteed to be unique for
 * non-strings.  Much error-checking exists to make verify uniqueness for strings.  Improved
 * hash function for fewer hash collisions.
 * 
 * 7     11/10/97 5:59p Agrant
 * Added debug code to check for hash collisions on strings.
 * 
 * 6     97/02/03 3:32p Pkeet
 * Added the 'bALWAYS_UNIQUE' define.
 * 
 * 5     12/16/96 10:42p Pkeet
 * Temporarily restored old hash function.
 * 
 * 4     12/16/96 10:15p Pkeet
 * Made the hash function return unique values.
 * 
 * 3     12/16/96 8:12p Pkeet
 * Updated hash function.
 * 
 * 2     12/16/96 7:44p Pkeet
 * Fixed bug.
 * 
 * 1     96/12/15 18:16 Speter
 * Moved code from Hash.hpp.  Changed hash function to shift a bit at a time rather than a
 * byte, providing more random results.
 * 
 * 1     11/17/96 3:09p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

//
// Includes.
//

#include <string>
#include <map>
#include "common.hpp"
#include "Hash.hpp"
#include "CRC.hpp"
#include "Lib/Sys/DebugConsole.hpp"


// init constructors in this file when the library is initialized
#pragma warning(disable:4073)
#pragma init_seg(lib)

//
// Type definition.
//

// Definition of a string to raster map.
typedef std::map<std::string, uint32, std::less<std::string> > TMapHash;

// Definition of a hash to a string map.
typedef std::map<uint32, std::string, std::less<uint32> > TMapHashString;
//
// Module variables.
//

// String to bitmap map.
TMapHash* mapHash = 0;
// Count for hashing.
uint32 u4HashCount = 1;

// By default, do not verify hashing.  It is very slow.
bool bVerifyHashing = false;

class CHashInit
{
public:
	static	CCRC*	pcrcCCITT32;

	CHashInit()
	{
		if (pcrcCCITT32 == NULL)
			pcrcCCITT32 = CCRC::pcrcCreateCCITT32();
	}

	~CHashInit()
	{
		if (mapHash)
		{
			mapHash->erase(mapHash->begin(), mapHash->end());
			delete mapHash;
		}

		if (pcrcCCITT32)
		{
			delete pcrcCCITT32;
			pcrcCCITT32 = NULL;
		}
	}
};


CHashInit	HashInit;
CCRC*		CHashInit::pcrcCCITT32 = NULL;


#if VER_TEST
TMapHashString mapHashString;
#endif

//**********************************************************************************************
uint32 u4Hash(const void* pv, int i_size_bytes,	bool b_string)
{
	Assert(pv);
	Assert(CHashInit::pcrcCCITT32);

	// generate the CRC value for the block....
	CHashInit::pcrcCCITT32->Reset();

	if (b_string)
	{
		CHashInit::pcrcCCITT32->CRCString((const char*)pv);
	}
	else
	{
		Assert(i_size_bytes > 0);
		CHashInit::pcrcCCITT32->CRCBlock(pv, i_size_bytes);
	}

	uint32 u4_hash = (*CHashInit::pcrcCCITT32)();

#if VER_TEST
	if (b_string && bVerifyHashing) 
	{
		const char* str = (const char*)pv;

		Assert(b_string);
	#ifdef __MWERKS__
		pair<TMapHashString::iterator, bool> pib = mapHashString.insert(pair<const uint32, string>(u4_hash,string(str)));
	#else
		std::pair<TMapHashString::iterator, bool> pib = mapHashString.insert(std::pair<uint32, std::string>(u4_hash, std::string(str)));
	#endif

		// Did the insertion succeed?
		if (pib.second)
		{
			// Yes!  Return the valid hash value.
			Assert(u4_hash);
			return u4_hash;
		}
		else
		{
			// No!  Make sure that we have the same string.
			if (strcmpi((*pib.first).second.c_str(), str))
			{
				// If this assert fails, then we have a hash collision between *it and str
				dout << "Hash collision between \"" << (*pib.first).second.c_str();
				dout << "\"\t\tand \"" << str << "\"\n";
				AlwaysAssert(0);
			}
			return u4_hash;
		}
	}
#endif  // !VER_FINAL

	// Return the hash value.
	return u4_hash;
}


//**********************************************************************************************
uint32 u4HashContinue(const void* pv, int i_size_bytes,	bool b_string)
{
	Assert(pv);
	Assert(CHashInit::pcrcCCITT32);

	// generate the CRC value for the block but do not reset the CRC so this data will be combined
	// with the previous data.
	// This data is not checked for being unique as that would required us to know what data was
	// used for the previous hash value..
	if (b_string)
	{
		CHashInit::pcrcCCITT32->CRCString((const char*)pv);
	}
	else
	{
		Assert(i_size_bytes > 0);
		CHashInit::pcrcCCITT32->CRCBlock(pv, i_size_bytes);
	}

	return (*CHashInit::pcrcCCITT32)();
}

