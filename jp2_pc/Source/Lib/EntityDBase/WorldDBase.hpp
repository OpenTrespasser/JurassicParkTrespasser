/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The world database.
 *
 * Bugs:
 *
 * To do:
 *		Make 'ppartsSpace' private again.
 *		Consider changing the type of the subsystem list to CSubsystem.
 *		Build a separate lock and key class.
 *		The constructor for the world database should *not* add a CDaemon. Rather, this should
 *		be added by the initialisation step of the application itself.
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/WorldDBase.hpp                                        $
 * 
 * 95    98/09/19 14:45 Speter
 * Made 2 bSaveWorlds different names, so one can be called from the debugger.
 * 
 * 94    9/03/98 7:31p Pkeet
 * Added the 'AssertOnDuplicates' member function.
 * 
 * 93    98.09.03 5:02p Mmouni
 * Added game over stuff.
 * 
 * 92    9/03/98 4:30p Shernd
 * Checking for out of disk space
 * 
 * 91    8/03/98 6:34p Shernd
 * Changes for Progress Loading Bar
 * 
 * 90    7/30/98 5:48p Mlange
 * Now creates and adds animation script subsystem to world dbase.
 * 
 * 89    7/11/98 1:38p Shernd
 * Added handler to SaveGame that would help support Trespass better
 * 
 * 88    98.06.25 7:22p Mmouni
 * Changed default quality setting to 3.
 * Added deferred save.
 * 
 * 87    6/24/98 3:23p Rwyatt
 * Externed the RenderDBase class
 * 
 * 86    98.06.19 8:28p Mmouni
 * Added functions to support deferred level loading.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_WORLDDBASE_HPP
#define HEADER_LIB_ENTITYDBASE_WORLDDBASE_HPP

//
// Required includes.
//
#include "Lib/Transform/Rotate.hpp"
#include <bstring.h>


//
// Opaque type declarations.
//
class CPartition;
class CPartitionSpace;
class CScreenRender;
class CCamera;
class CTerrain;
class CInstance;
class CEntity;
class CTerrainObj;
class CSaveFile;
class TMapIntPPart;
class TListInstance;
class TListConstChar;


typedef uint32 (__stdcall * PFNWORLDLOADNOTIFY)(uint32 dwContext, uint32 dwParam1, uint32 dwParam2, uint32 dwParam3);

//
// Class definitions.
//

//*********************************************************************************************
//
class CWorld
//
// World database object.
//
// Prefix: w
//
//**************************************
{
private:

	bool			 bHasBeenPurged;	// True if the database has already been purged, and is ready for deletion.
	CPartition*      ppartPartitions;	// List of all spatial partitionings in the world.
	CPartition*      ppartTriggers;		// List of all triggers in the world.
	CPartition*      ppartBackdrops;	// List of all backdrop objects in the world.
	CPartition*		 ppartTerrainObjs;	// List of terrain objects in the world.
										// This list is disjoint from the main partition list, because
										// they are queried and maintained totally separately, and it's
										// speedier.
	CCamera*		 pcamActive;		// The camera that is currently active.
	TListInstance*   psubList;			// List of all subsystems active in the world.
	CEntity*         petShell;			// Pointer to the shell of this application.

	CTerrain*		 ptrrTerrain;		// Pointer to the terrain subsystem. Note that this also appears on the subsystem list<>.

	int iNumInstances;					// Current count of objects in the world.
	int iNumSubsystems;					// Current count of subsystems.
	int iLockLevel;						// Recursive lock counter.

	TListConstChar*	 plsstrGroffsLoaded;	// Remember all Groffs loaded (for scene save).

	TMapIntPPart*	 pmapHandlePointer;		// A map between handles and partition/instance pointers.

	bool			 bLoadPending;			// We want to load a new level.
	string			 strLoadName;			// Level to load before the next frame.

	bool			 bSavePending;			// We want to save.
	string			 strSaveName;			// Level to save before the next frame.

	bool			 bGameOver;				// Game over man, game over.

public:
	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	CWorld();

	~CWorld();
	
	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Init
	(
	);
	//
	// Sets up the world database for future use.
	//
	//**********************************

	//*****************************************************************************************
	//
	void Reset
	(
	);
	//
	// Deletes everything in the database and sets it up for future use.
	//
	//**********************************

	//*****************************************************************************************
	//
	void Add
	(
		CInstance* pins,		// The instance to add.
		bool b_fast = false		// True if we just want it fast, false if we need to propagate down the tree
	);
	//
	// Add an instance to the database. This function will fail if the lock is on.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddDefaultLight
	(
	);
	//
	// Creates and adds a default light.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddBackdrop
	(
		CInstance* pins
	);
	//
	// Add the instance to the backdrop database.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddWithNoLock
	(
		CInstance* pins,
		bool b_fast = false
	);
	//
	// Add an instance to the database. This function is the same as the add function, but
	// ignores the lock mechanism.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddShell
	(
		CEntity* pet
	);
	//
	// Add a pointer to the application's shell.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddGroff
	(
		const char* str_groff				// Groff file that was loaded.
	);
	//
	// Add a copy of the Groff file name string to the list.  Doesn't actually load the Groff.
	//
	//**************************************

	//*****************************************************************************************
	//
	const char* strGetGroff
	(
		uint32 u4_index
	);
	//
	// Returns the file name of the requested groff file. Groff files are added to this list
	// in order so the first file loaded will be index 0.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Remove
	(
		CInstance* pins
	);
	//
	// Remove an instance from the database.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Lock
	(
	) const
	//
	// Increments the recursive lock counter.
	//
	// Notes:
	//		For convenience the 'const' is cast away.
	//
	//**************************************
	{
		Assert(iLockLevel >= 0);

		(*((int*)&iLockLevel))++;
	}

	//*****************************************************************************************
	//
	void Unlock
	(
	) const
	//
	// Decrements the recursive lock counter.
	//
	// Notes:
	//		For convenience the 'const' is cast away.
	//
	//**************************************
	{
		Assert(iLockLevel > 0);

		(*((int*)&iLockLevel))--;
	}

	//*****************************************************************************************
	//
	bool bIsLocked
	(
	) const
	//
	// Returns 'true' if the world database is locked.
	//
	//**************************************
	{
		return iLockLevel != 0;
	}

	//*****************************************************************************************
	//
	void DumpWorld
	(
		const char* str_fileout = "World.txt"
	);
	//
	// Write the contents of the world database to a textfile for debugging purposes.
	//
	//**************************************

	//*****************************************************************************************
	//
	void DumpSpatial
	(
	);
	//
	// Writes the spatial hierarchy to a textfile for debugging purposes.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bSaveWorld
	(
		const char* str_filename		// File name to save.
	);
	//
	// Saves the scene description to the given file.
	// 
	// Returns:
	//		Success.
	//
	//**********************************

	//*****************************************************************************************
	//
	bool bSaveTheWorld
	(
		CSaveFile* pSF           // Opened Save File to place save game
	);
	//
	// Saves the scene description to the given file.
	// 
	// Returns:
	//		Success.
	//
	//**********************************

	//*****************************************************************************************
	//
	void LoadHierarchyInfo
	(
		CSaveFile* psf,				// The save file to load from.
		const char* str_section,	// The section name to load from.
		CPartition* ppart,			// The top level partition to load to.
		bool b_attempt_error_correction = false	// true if we should try to fake the partition structure where the parent is not present.
	);
	//
	// Loads the hierarchy info in the given section to the given partition.
	//
	//**********************************

	//*****************************************************************************************
	//
	void SaveHierarchyInfo
	(
		CSaveFile* psf,				// The save file to save to.
		const char* str_section,	// The section name to save to.
		CPartition* ppart			// The top level partition to save from.
	);
	//
	// Saves the hierarchy info in the partition to the given section.
	//
	//**********************************

	//*****************************************************************************************
	//
	int32 bLoadScene
	(
		const char* str_filename,		// File name of scene to load.
        PFNWORLDLOADNOTIFY pfnLoadNotify = NULL, // Notification callback
        uint32 u32_NotifyParam = 0      // Parameter to Notification callback
	);
	//
	// Loads the scene file.  Determines if this is a full reset or partial based on the GROFFs
	// currently loaded.
	// 
	// Returns:
	//		 1 - Success
    //      -1 - SceneFile Does not exist
    //      -2 - If not enough space on Drive for SwapFile
	//
	//**********************************

	//*****************************************************************************************
	//
	int32 bLoadWorld
	(
		const char* str_filename,		// File name of scene to load.
		bool b_reset = false,			// Reset positions only, do not load.
        PFNWORLDLOADNOTIFY pfnLoadNotify = NULL, // Notification callback
        uint32 u32_NotifyParam = 0      // Parameter to notification callback
	);
	//
	// Loads the scene file.  If !b_reset, loads specified Groffs.  Positions instances.
	// 
	// Returns:
	//		 1 - Success
    //      -1 - SceneFile Does not exist
    //      -2 - If not enough space on Drive for SwapFile
	//
	//**********************************

	//*****************************************************************************************
	//
	bool bLoadPartitionFromScene
	(
		const char* str_filename		// File name of scene to load.
	);
	//
	// Loads the scene file.  If !b_reset, loads specified Groffs.  Positions instances.
	// 
	// Returns:
	//		Success.
	//
	//**********************************

	//*****************************************************************************************
	//
	bool bSaveAsText
	(
		const char* str_filename		// File name to save.
	);
	//
	// Saves the scene description to the given file in a text format.
	// 
	// Returns:
	//		Success.
	//
	//	Notes:
	//		Saves PRS information only.
	//
	//**********************************

	//*****************************************************************************************
	//
	void PrunePartitions
	(
	);
	//
	// Removes empty partition nodes and optimizes the partition tree.
	//
	//**************************************

	//*****************************************************************************************
	//
	void FlattenPartitions
	(
	);
	//
	// Flatten the existing partitioning structure by removing pure spatial partitions.
	//
	//**************************************

	//*****************************************************************************************
	//
	void BuildOptimalPartitions
	(
		void (__cdecl *partcallback)(int i)	// Callback for monitoring partition building.
	);
	//
	// Builds and optimizes the partition tree.
	//
	//**************************************

	//*****************************************************************************************
	//
	void BuildTerrainPartitions
	(
	);
	//
	// Builds and optimizes the partition tree.
	//
	//**************************************

	//*****************************************************************************************
	//
	void BuildTriggerPartitions
	(
	);
	//
	// Builds and optimizes the trigger partition tree.
	//
	//**************************************

	//*****************************************************************************************
	//
	CPartition *ppartGetWorldSpace
	(
	) const;
	//
	// Returns a pure spatial partition that bounds the entire physical world.
	//
	//**************************************


	//*********************************************************************************************
	//
	CCamera* pcamGetActiveCamera() const
	//
	// Returns
	//		The current active camera.
	//
	//******************
	{
		return pcamActive;
	}


	//*****************************************************************************************
	//
	void GetWorldExtents
	(
		CVector3<>& v3_min, 
		CVector3<>& v3_max
	) const;
	//
	// Return the combined extents of all world objects, including terrain.
	//
	//**********************************

	//*****************************************************************************************
	//
	CPartition* ppartPartitionList() const
	//
	// Returns:
	//		The top-level partition of the regular world.
	//
	//**********************************
	{
		return ppartPartitions;
	}

	//*****************************************************************************************
	//
	CPartition* ppartBackdropsList() const
	//
	// Returns the top-level partition for backdrop objects.
	//
	//**********************************
	{
		return ppartBackdrops;
	}

	//*****************************************************************************************
	//
	CPartition* ppartTriggerPartitionList() const
	//
	// Returns the top-level partition of the trigger spatial hierarchy.
	//
	//**********************************
	{
		return ppartTriggers;
	}

	//*****************************************************************************************
	//
	CPartition* ppartTerrainPartitionList() const
	//
	// Returns:
	//		The top-level partition of the terrain object world.
	//
	//**********************************
	{
		return ppartTerrainObjs;
	}

	//*****************************************************************************************
	//
	void Purge
	(
	);
	//
	// Deletes everything in the database.
	//
	//**********************************

	//*****************************************************************************************
	//
	void Preload
	(
	);
	//
	// Preloads data for upcoming scenes.
	//
	//**********************************

	//*****************************************************************************************
	//
	char* pcSaveSpatialPartitions
	(
		char* pc_buffer  // Buffer to save spatial partitions to.
	);
	//
	// Saves pure spatial partitions into a buffer.
	//
	//	Returns:
	//		Pointer 
	//
	//**********************************

	//*****************************************************************************************
	//
	const char* pcLoadSpatialPartitions
	(
		const char* pc_buffer,	// Buffer to load partitions from.
		int i_size				// Number of bytes in buffer.
	);
	//
	// Loads pure spatial partitions from a buffer.
	//
	//**********************************


	//
	//	These functions allow the world library to access certain debugging functions.
	//	They have null implementations unless overridden by the application using the world
	//	library.  The functions are implemented in WorldPort.cpp
	//

	//*********************************************************************************************
	//
	bool bSelected
	(
		CInstance* pins
	);
	//
	// Returns:
	//		Whether pins is currently selected.
	//
	//**********************************

	//*********************************************************************************************
	//
	int iSelectedCount() const;
	//
	// Returns:
	//		How many objects selected.
	//
	//**********************************

	//*********************************************************************************************
	void ClearHandleMap
	(
	);
	//
	//	Empties the handle map of a all handle info.
	//
	//******************

	//*********************************************************************************************
	void AddToHandleMap
	(
		uint32 u4_handle,	// The handle to add.
		CPartition* ppart	// The partition with the given handle.
	);
	//
	//	Adds a handle/pointer pair to the handle map.
	//
	//******************

	//*********************************************************************************************
	CInstance* pinsFindInstance
	(
		uint32 u4_handle		// Unique handle of the instance to find.
	);
	//
	//	Returns a pointer to the instance with the specified handle, if it exists in the world database.
	//
	//******************

	//*********************************************************************************************
	CPartition* ppartFindPartition
	(
		uint32 u4_handle			// Unique handle of the instance to find.
	);
	//
	//	Returns a pointer to the partition with the specified handle, if it exists in the world database.
	//
	//******************

	//*********************************************************************************************
	CPartition* ppartFindPartition
	(
		uint32 u4_handle,			// Unique handle of the instance to find.
		CPartition* ppart_parent	// The partition to use as a backup if the map find fails.
	);
	//
	//	Returns a pointer to the partition with the specified handle, if it exists in the world database.
	//
	//******************

	//*********************************************************************************************
	CInstance* pinsLastSelected
	(
		int i_index = 0					// Which object to get from the list.
										// 0 means last selected, 1 means next last, etc.
	);
	//
	// Returns:
	//		The asked-for instance, or 0 if it existsn't.
	//
	//**********************************

	//*********************************************************************************************
	//
	void Select
	(
		CInstance* pins,				// Object to select.
		bool b_augment = false			// Adds it to list; otherwise, selects exclusively.
	);
	//
	// Makes pins selected.
	//
	//**********************************

	//*****************************************************************************************
	//
	bool bSelect
	(
		int i_x, int i_y,					// Position of mouse cursor on screen.
		bool b_augment = false				// As above.
	);
	//
	// Select a shape based on the position of the mouse.
	//
	// Returns 'true' if a shape is found near the mouse cursor.
	//
	//**************************************

	//*****************************************************************************************
	//
	int iDeleteSelected
	(
	);
	//
	// Removes the selected shapes from the world.
	//
	// Returns:
	//		How many removed.
	//
	//**************************************

	//*****************************************************************************************
	//
	const CTerrainObj* ptobjGetTopTerrainObjAt
	(
		TReal r_x,		// World X coord
		TReal r_y		// World Y coord
	);
	//
	//  Returns:
	//		A pointer to the highest terrain obj covering point X,Y on the terrain.
	//
	//**************************************

	//*****************************************************************************************
	//
	void InitializePartitions
	(
	);
	//
	// Initializes partitioning system after it is built for use.
	//
	//**************************************

	//*****************************************************************************************
	//
	void RestoreSubsystemDefaults
	(
	);
	//
	// Restore the default settings of the subsystems.
	//
	//**********************************

	//*****************************************************************************************
	//
	void DeferredLoad
	(
		const char *str_name
	);
	//
	// Set the level we want to load (before next frame).
	//
	//**********************************

	//*****************************************************************************************
	//
	bool bIsLoadPending
	(
	);
	//
	// Returns true if a deferred load is pending.
	//
	//**********************************

	//*****************************************************************************************
	//
	const string& strGetPendingLoad
	(
	);
	//
	// Get the pending load level name and clear it.
	//
	//**********************************

	//*****************************************************************************************
	//
	void DeferredSave
	(
		const char *str_name
	);
	//
	// Set the level we want to save to (before next frame).
	//
	//**********************************

	//*****************************************************************************************
	//
	bool bIsSavePending
	(
	);
	//
	// Returns true if a deferred save is pending.
	//
	//**********************************

	//*****************************************************************************************
	//
	const string& strGetPendingSave
	(
	);
	//
	// Get the pending save level name and clear it.
	//
	//**********************************

	//*****************************************************************************************
	//
	bool bIsGameOver()
	//
	// Returns true if the game is over is set.
	// Then, clears the game over flag if it is set.
	//
	//**********************************
	{
		if (bGameOver)
		{
			bGameOver = false;
			return true;
		}

		return false;
	}

	//*****************************************************************************************
	//
	void GameIsOver()
	//
	// Sets the game over flag to true.
	//
	//**********************************
	{
		bGameOver = true;
	}


	//*****************************************************************************************
	//
	void AssertOnDuplicates
	(
	);
	//
	// Asserts when duplicate handles are found.
	//
	//**********************************

private:

	//
	// The following are utilised by world objects to move themselves in the world.
	//

	//*****************************************************************************************
	//
	void Move
	(
	 	CPartition* ppart,					// Partition to move in the world database.
		const CPlacement3<>& p3_new			// The object's new placement.
											// Note: its scale is unchanged.
	);
	//
	// Moves a partition from in the world database so that it maintains a valid position
	// in the spatial hierarchy.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Move
	(
	 	CPartition* ppart,					// Partition to move in the world database.
		const CPlacement3<>& p3_new,		// The object's new placement.
											// Note: its scale is unchanged.
		CPartition* ppart_top				// The top level partition into which the object is moved.
	);
	//
	// Moves a partition from in the world database so that it maintains a valid position
	// in the spatial hierarchy.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ResetFrameCounts
	(
	);
	//
	// Resets the frame counts for all partitions.
	//
	//**************************************

	//*****************************************************************************************
	//
//	void Move
//	(
//	 	CInstance* pins,					// Instance to move in the world database.
//		const CPlacement3<>& p3_new,		// The object's new placement.
//											// Note: its scale is unchanged.
//		TSec s_when = 0						// When it is consider ed moved (if needed).
//	);
	//
	// Moves an instance, handles partition management, and sends appropriate messages.
	//
	//**************************************


	//*****************************************************************************************
	//
	// The friend query classes.
	//
	friend class CWDbQueryActiveEntities;

	friend class CWDbQueryTerrain;
	friend class CWDbQueryTerrainMesh;
	friend class CWDbQueryTerrainTopology;

	friend class CWDbQuerySubsystem;
	friend class CWDbQueryActiveDaemon;
	friend class CWDbQueryActivePlayer;

	// friends needed in AI Test App only.
//	friend class CWDbQueryTestShape;
//	friend class CWDbQueryTest;

	friend class CRenderDB;

	// Friends for rendering.
//	friend class CWDbQueryShapes;
//	friend class CWDbQueryAllParts;
//	friend class CWDbQueryLights;
	friend class CWDbQueryActiveCamera;
//	friend class CWDbQueryRenderAtom;
//	friend class CWDbQueryTerrainObj;
//	friend class CWDbQuerySolidObject;
	friend class CMessageNewRaster;

	// Object friends.
	friend class CPartition;
	friend class CInstance;
	friend class CMagnet;
	friend class CTrigger;
	friend class CTerrainObj;
	friend class CGUIAppDlg;

private:
	//
	// Prevent copying of the world.
	//
	CWorld(const CWorld&);  //lint !e1704   // private constructor.

	CWorld& operator =(const CWorld&);

};


//
// Global variables.
//

// Object for render dBase
extern CRenderDB* ps_renderDB;

// Object representing the world database.
extern CWorld* pwWorld;
#define	wWorld (*pwWorld)

// Minimum is always zero!
#define iMAX_QUALITY_SETTING 4
#define iDEFAULT_QUALITY_SETTING (3)


	//*****************************************************************************************
	//
	int iGetQualitySetting();
	//
	//  Gets a value, 0 to 4, indicating the overall quality of the simulation.
	//
	//	Notes:
	//		0 is lowest quality and top speed.  Really low quality.  Really really low.
	//		4 is the top quality.
	//		The quality we usually look at for our frame rate tests is 3 or 4.
	//
	//**************************************

	//*****************************************************************************************
	//
	float fGetQualitySetting();
	//
	//  Gets a value, 0 to 1, indicating the overall quality of the simulation.
	//
	//	Notes:
	//		0 is lowest quality and top speed.  Really low quality.  Really really low.
	//		1 is the top quality.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetQualitySetting(int i_quality);
	//
	//  Gets a value, 0 to 4, indicating the overall quality of the simulation.
	//
	//	Notes:
	//		0 is lowest quality and top speed.  Really low quality.  Really really low.
	//		4 is the top quality.
	//		The quality we usually look at for our frame rate tests is 3 or 4.
	//
	//**************************************



// Save helper functions.

	//*****************************************************************************************
	//
	char * pcSaveInstancePointer
	(
		char * pc,						// Buffer to save in.
		const CInstance* pins			// Pointer to save.
	);
	//
	// Saves the instance pointer.  Returns the next free byte in the buffer (pc)
	//
	//	Notes:
	//		Uses the instance's handle to save its info.
	//
	//**************************************

	//*****************************************************************************************
	//
	const char * pcLoadInstancePointer
	(
		const char* pc,		// The buffer to read from.
		CInstance** ppins	// Pointer to the pins to be filled.
	);
	//
	// Loads the instance pointer.  Returns the next unused byte in the buffer (pc)
	//
	//	Notes:
	//		Uses the instance's handle to save its info.
	//		Does not alter *ppins if no instance is found, but does assert.
	//
	//**************************************



#endif
