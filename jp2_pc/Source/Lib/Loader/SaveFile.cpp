/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of 'SaveFile.hpp.'
 *
 * Bugs:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/SaveFile.cpp                                              $
 * 
 * 45    98/10/07 6:53 Speter
 * Upped for player stow pos.
 * 
 * 44    98/10/04 19:12 Speter
 * Bump version number to make sure dino feet are saved with latest fix.
 * 
 * 43    10/02/98 6:06p Agrant
 * up version number (20) for CEntityAttached
 * 
 * 42    9/29/98 12:05a Agrant
 * bumped animal version to 10
 * 
 * 41    9/28/98 10:21p Mlange
 * Upped version number for saving of blood splats.
 * 
 * 40    9/26/98 9:00p Agrant
 * Upped version number to 18 for player and animation systems
 * 
 * 39    9/24/98 8:38p Mlange
 * Increased version number of save files for partition flags.
 * 
 * 38    9/18/98 3:10p Agrant
 * save/load water disturbances properly
 * 
 * 37    98/09/15 4:00 Speter
 * Upped version for stow orient memory.
 * 
 *********************************************************************************************/


//
// Includes.
//
#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "SaveFile.hpp"

#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"

#include "Lib/Groff/Groff.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include <stdlib.h>

//
// Module variables.
//

#define iVALID_SAVE_VERSION		7
#define iLATEST_SAVE_VERSION	22
#define iLATEST_ANIMAL_VERSION	10

#if VER_DEBUG
//#define VERBOSE(a)  dout << a
#define VERBOSE(a)
#else
//#define VERBOSE(a)  dout << a
#define VERBOSE(a)
#endif

bool CSaveFile::bBrief = false;
int CSaveFile::iCurrentVersion = -1;
int iAnimalVersion = -1;

	//*****************************************************************************************
	//
	// CSaveFile constructor and destructor.
	//

	// Constructor requiring a filename.
	CSaveFile::CSaveFile(const char* str_filename, bool b_read, bool b_brief) : bRead(b_read)
	{
		// Set the brief flag.
		bBrief = b_brief;
		shHeader.bBrief = b_brief;

		pcBuffer = (char*)VirtualAlloc(NULL, iPLENTY_BYTES, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

		// Until we set up a file, it isn't valid.
		bValidFile = false;

		// Set dummy header values here.
		shHeader.iVersion = -1;
		int i;
		for (i = 0; i < iNUM_GROFFS; ++i)
			strcpy(shHeader.strGROFF[i], "");
		for (i = 0; i < iSLOP_BYTES; ++i)
			shHeader.acSLOP[i] = 0;


		// Is this a read-only file?
		if (b_read)
		{
			// Yes!  Open the file for reading.
			bValidFile = fioFile.bOpen(str_filename, eRead);

#if VER_TEST
			if (!bValidFile)
			{
					char str_buffer[512];
					sprintf(str_buffer, 
							"%s\n\nMissing .scn file:\n%s\n", 
							__FILE__,
							str_filename);

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
			}
#endif


			if (!fioFile.bReadImage())
			{
				dout << "CSaveFile:  Unable to load Save image into memory.";

				// Error!  Failed to read image!
				AlwaysAssert(false);
			}

			AlwaysAssert(bLoadHeader());
		}
		else
		{
			// No! Open for writing.
			bValidFile = fioFile.bOpen(str_filename, eWrite);
			AlwaysAssert(bValidFile);

		}
	}

 	CSaveFile::~CSaveFile()
	{
		Verify(VirtualFree(pcBuffer,iPLENTY_BYTES,MEM_DECOMMIT));
		Verify(VirtualFree(pcBuffer,0,MEM_RELEASE));
		fioFile.bClose();
	}


	//*****************************************************************************************
	bool CSaveFile::bLoadHeader()
	{
		Assert(bRead);

		// Find the section for the header.
		TSectionHandle seh = 0;
		seh = fioFile.sehLookup("Header");
		AlwaysAssert(seh);
		
		// Read in the header.
		int i_size = fioFile.iRead(seh, &shHeader, sizeof(shHeader));
		bool b;
		AlwaysAssert(b = (i_size == sizeof(shHeader)));

		bBrief = shHeader.bBrief;

		// Get the version number, check its validity, and put it somewhere that loading objects
		// can find it.
		AlwaysAssert(bWithin(shHeader.iVersion, iVALID_SAVE_VERSION, iLATEST_SAVE_VERSION));
		iCurrentVersion = shHeader.iVersion;

		if (shHeader.iVersion < iLATEST_SAVE_VERSION)
			dout <<"Warning: Scene file is version " <<shHeader.iVersion
				 <<", latest version is " <<iLATEST_SAVE_VERSION << std::endl;

		// Put the animal version somewhere that animals can find it.
		iAnimalVersion = shHeader.iAnimalVersion;

		VERBOSE("Header Size: \t" << i_size << '\n');

		return b;
	}

	//*****************************************************************************************
	void CSaveFile::AddGROFF(const char* str_groff)
	{
		// Make sure the file is writeable.
		Assert(!bRead);
		Assert(bValidFile);

		// Cannot have a groff file name that is too long.
		AlwaysAssert(strlen(str_groff) < iGROFF_STRLEN);

		// Find an empty groff slot.
		int i;
		for (i = 0; i < iNUM_GROFFS; ++i)
		{
			if (shHeader.strGROFF[i][0] == '\0')
				break;
		}
		if (i >= iNUM_GROFFS)
		{
			Assert(false);		// Too many groffs!
			return;
		}

		// Set up the header.
		strcpy(shHeader.strGROFF[i], str_groff);
	}

	//*****************************************************************************************
	void CSaveFile::SaveHeader()
	{
		// Make sure the file is writeable.
		Assert(!bRead);
		Assert(bValidFile);

		// Make sure we haven't already done this bit....
		Assert(shHeader.iVersion == -1);

		// Set up the header.
		shHeader.iVersion = iLATEST_SAVE_VERSION;
		shHeader.iAnimalVersion = iLATEST_ANIMAL_VERSION;
		shHeader.sCurrentTime = CMessageStep::sStaticTotal;
		shHeader.sCurrentRealTime = CMessageStep::sElapsedRealTime;

		// Now make the header section.
		TSectionHandle seh = fioFile.sehCreate("Header", gSPECIAL);

		// Save the data, with some extra garbage on the end to allow for changes in header format.
		char str_temp[sizeof(SSaveHeader)];
		*((SSaveHeader*)str_temp) = shHeader;

		int i_size = fioFile.iWrite(seh, &shHeader, sizeof(SSaveHeader));
		AlwaysAssert(i_size == sizeof(SSaveHeader));
		VERBOSE("Save Header: \t" << i_size << '\n');
	}

	//*****************************************************************************************
	bool CSaveFile::bWrite()
	{
		Assert(!bRead);

		// This assert insures that the header has been set up properly.
		Assert(shHeader.iVersion == iLATEST_SAVE_VERSION);

		return fioFile.bWriteImage();
	}

	//*****************************************************************************************
	void CSaveFile::Load(CInstance* pins)
	{
		char str_buf[16];
		Assert(bRead);

		// Make sure that we don't already have one of these!  Assert that the insertion is successful.
		Assert(setHandles.insert(pins->u4GetUniqueHandle()).second);

		// Find the section for the header.
		TSectionHandle seh = 0;
		seh = fioFile.sehLookup(pins->strGetUniqueName(str_buf));

		if (!seh)
		{
			// Can continue from here!  But it is still a bug.
			//AlwaysAssert(seh);
			if (!bBrief)
			{
				dout << "Failed to find " << str_buf << " in save file!\n";
			}
			return;
		}
		
		// How big is this section?
		int i_section_size = fioFile.iCount(seh);
		
		// Read in the header.
		int i_bytes = (fioFile.iRead(seh, pcBuffer, i_section_size));
		AlwaysAssert(i_bytes == i_section_size);

		int i_size = pins->pcLoad(pcBuffer) - pcBuffer;
		AlwaysAssert(i_size == i_section_size);
		VERBOSE("Loading " << i_size << '\t' << pins->strGetInstanceName() << '\n');
	}

	//*****************************************************************************************
	void CSaveFile::Save(const CInstance* pins)
	{
		char str_buf[16];
		Assert(!bRead);

		// Make sure that we don't already have one of these!  Assert that the insertion is successful.
		std::pair < std::set <uint32, std::less<uint32> >::iterator, bool > p = setHandles.insert(pins->u4GetUniqueHandle());
		Assert(p.second);


		// Actually write to the buffer.
		int i_size = pins->pcSave(pcBuffer) - pcBuffer;
		Assert(i_size < iPLENTY_BYTES);

		Assert(i_size >= 0);

		// Do we actually have some data?
		if (i_size != 0)
		{
			// Now make the header section.
			TSectionHandle seh = fioFile.sehCreate(pins->strGetUniqueName(str_buf), gSPECIAL);

			// Now copy into the section.
			AlwaysAssert(fioFile.iWrite(seh, pcBuffer, i_size) == i_size);
			VERBOSE("Saving " << i_size << '\t' << pins->strName.c_str() << '\n');
		}
		else
		{
			// No! Don't save.
			VERBOSE("Skipping " << pins->strName.c_str() << '\n');
		}
	}


	//*****************************************************************************************
	const char*  CSaveFile::pcLoad
	(
		const char* str_section_name, // Name of the section to load.
		int * pi_bytes			// Return value for number of bytes loaded.
	)
	{
		Assert(bRead);
		Assert(pi_bytes);

		*pi_bytes = 0;

		// Find the section for the header.
		TSectionHandle seh = 0;
		seh = fioFile.sehLookup(str_section_name);
		if (!seh)
		{
//			AlwaysAssert(false);
			VERBOSE("Cannot find " << str_section_name  << '\n');
			return 0;
		}
		
		// How big is this section?
		*pi_bytes = fioFile.iCount(seh);
		
		// Read in the header.
		int i_bytes = (fioFile.iRead(seh, pcBuffer, *pi_bytes));

		// Did we read the expected amount of data?
		if (i_bytes == *pi_bytes)
		{
			// Yes!
			VERBOSE("Loading " << i_bytes << '\t' << str_section_name << '\n');
			return pcBuffer;
		}
		else
		{
			// No!
			AlwaysAssert(false);
			return 0;
		}
	}


	//*****************************************************************************************
	bool CSaveFile::bSave
	(
		const char* str_section_name,	// Name of the section to write.
		const char* pc_buffer,			// Data to write.
		int i_bytes						// Number of bytes to write.
	)
	{
		Assert(!bRead);
		Assert(str_section_name);
		Assert(pc_buffer);

		// Now make the header section.
		TSectionHandle seh = fioFile.sehCreate(str_section_name, gSPECIAL);

		// Actually write to the buffer.
		Assert(i_bytes < iPLENTY_BYTES);
		Assert(i_bytes >= 0);

		// Now copy into the section.
		bool b = fioFile.iWrite(seh, (void*)pc_buffer, i_bytes) == i_bytes;
		AlwaysAssert(b);
		VERBOSE("Saving " << i_bytes << '\t' << str_section_name << '\n');
			
		return b;
	}
