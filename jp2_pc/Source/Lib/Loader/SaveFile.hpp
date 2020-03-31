/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Save class for Trespasser CInstances (could be a CSaveable at some point).
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/SaveFile.hpp                                              $
 * 
 * 17    8/25/98 11:39a Rvande
 * Removed redundant access specifier
 * 
 * 16    7/07/98 11:47a Agrant
 * Better slop byte handling
 * 
 * 15    7/06/98 10:23p Agrant
 * save/load real elapsed time in scene file
 * 
 * 14    98/06/28 17:24 Speter
 * Now expose version of currently opened save file as a public static.
 * 
 * 13    6/13/98 9:17p Agrant
 * Animal version numbers for save files
 * 
 * 12    5/26/98 12:23p Rwycko
 * make save buffer bigger for partition hierarchy
 * 
 * 11    5/13/98 7:03p Agrant
 * Add current step time to save file
 * 
 * 10    5/01/98 9:10p Rwyatt
 * Allocates pcBuffer with virtualAlloc rather that it being a member array as the class is
 * constructed on the stack.
 * Saved games cannot be above 512K.
 * 
 * 9     2/23/98 5:01p Agrant
 * Added the brief save file format.
 * 
 * 8     98/02/18 21:06 Speter
 * Moved pcSaveT and pcLoadT macros to new SaveBuffer.hpp.
 * 
 * 7     12/02/97 12:42a Agrant
 * Increased save buffer from 64 to 512K.
 * 
 * 6     12/01/97 3:25p Agrant
 * CSaveFile includes its own buffers.
 * Allows non-instances to save and load themselves.
 * 
 * 5     11/11/97 2:56p Agrant
 * Save/Load template functions for classes with no virtual function and simple data fields.
 * 
 * 4     11/10/97 6:01p Agrant
 * Added a set to verify that there are no handle collisions at save/load time.
 * 
 * 3     11/09/97 4:45p Agrant
 * Save file working!
 * 
 * 2     11/07/97 9:15p Agrant
 * Closer to saving games
 * 
 * 1     11/07/97 7:36p Agrant
 * initial rev
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_LOADER_SAVER_HPP
#define HEADER_GUIAPP_LOADER_SAVER_HPP


//
// Includes.
//

#include "SaveBuffer.hpp"
#include "Lib/GROFF/FileIO.hpp"
#include "Lib/Sys/Timer.hpp"
#include <set>

class CInstance;

// The number of groffs that must be loaded.
#define iNUM_GROFFS 5
#define iGROFF_STRLEN 256

// A number of bytes that is large enough to hold any section in the save file.
#define iPLENTY_BYTES			(1024 * 1024)
#define iSLOP_BYTES	48

//*********************************************************************************************
//
struct SSaveHeader
//
// Data that every save game needs.
//
// Prefix: sh
//
{
public:
	int		iVersion;			// The version number of the save game.
	char	strGROFF[iNUM_GROFFS][iGROFF_STRLEN];		// The GROFF file for the level used in the saved game.
	bool	bBrief;				// Was this file saved as a brief save file?
	TSec	sCurrentTime;
	int		iAnimalVersion;		// Another version number, used for determining which animal and brain save formats to use
	TSec	sCurrentRealTime;	
	char	acSLOP[iSLOP_BYTES];
};





//*********************************************************************************************
//
class CSaveFile
//
// A save file for reading OR writing, but not both.
//
// Prefix: sf
//
//**************************************
{
public:
	bool			bRead;		// True when the file is open for reading.
	SSaveHeader		shHeader;	// The save header.

	CFileIO			fioFile;	// The file for saving/loading.
	bool			bValidFile;	// True if the file is valid.

	std::set<uint32, std::less<uint32> > setHandles;	// The handles of instances that have been saved or loaded.


	static bool		bBrief;		// True if we are to conserve space.
								// For example, sometimes we can skip the immovable stuff.  This is going to cause bugs.
	static int		iCurrentVersion;	// A duplicate of shHeader.iVersion for the currently loading file.
										// A sad hack to provide due to the fact that object pcLoad gives
										// no access to the CSaveFile.

	char*			pcBuffer;

	//
	// Constructor and destructor.
	//

	// Constructor requiring a filename.
	CSaveFile(const char* str_filename, bool b_read, bool b_brief);

	// Destructor.
	~CSaveFile();


	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	bool bLoadHeader
	(
	);
	//
	// Loads the header from the save file.
	//
	//	Returns:  true if successful.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SaveHeader
	(
	);
	//
	// Saves the header.
	//
	//**************************************

	//*****************************************************************************************
	void AddGROFF
	(
		const char* str_groff
	);
	//
	//	Adds a groff file to the save file header.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bWrite
	(
	);
	//
	// Saves the save file to disk.
	//
	//	Returns:  true if successful.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Load
	(
		CInstance* pins
	);
	//
	// Loads the instance's saved data, if any exists.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Save
	(
		const CInstance* pins
	);
	//
	// Saves the instance's data.
	//
	//**************************************

	//*****************************************************************************************
	//
	const char*  pcLoad
	(
		const char* str_section_name, // Name of the section to load.
		int * pi_bytes			// Return value for number of bytes loaded.
	);
	//
	// Loads the specified section into a buffer.
	//
	//	Returns:
	//		Pointer to the loaded buffer. Memory from the buffer is supplied by the 
	//		CSaveFile class.  Pointer is zero if the operation fails.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bSave
	(
		const char* str_section_name,	// Name of the section to write.
		const char* pc_buffer,			// Data to write.
		int i_bytes						// Number of bytes to write.
	);
	//
	// Saves the specified section.
	//
	//	Returns:
	//		true if successful, else false.
	//
	//**************************************
};


#endif
