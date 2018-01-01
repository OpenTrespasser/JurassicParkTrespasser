/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	Sound Material implementation
 *
 * Bugs:
 *
 * To do:
 * Add a last hit member to every Collision, this will save searching the second list.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/Material.cpp                                                $
 * 
 * 19    3/22/98 5:02p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 18    11/24/97 2:21p Rwyatt
 * Text file reading functions are static so they can be called from elewhere.
 * 
 * 17    11/18/97 8:10p Rwyatt
 * Will only load version 2.01 files.
 * Asserts that hash ID of material types are correct
 * 
 * 16    11/13/97 11:51p Rwyatt
 * Version number has gone to 2.01 because attenuation is now in dBs and not distance. The
 * loader will convert the old format to the new.
 * 
 * 15    6/10/97 1:23p Rwyatt
 * Fixed a reference to un-initialised data
 * 
 * 14    6/10/97 1:07a Rwyatt
 * Added check for zero sound materials to save searching whole database
 * 
 * 13    6/10/97 12:59a Rwyatt
 * Added Query collision function
 * 
 * 12    6/09/97 12:09p Rwyatt
 * New format material database for matrix collisions.
 * 
 * 11    6/04/97 7:02p Rwyatt
 * Calling save with a null name will save over the COL file last loaded. It will assert if no
 * file has been loaded. This is for the save  materials menu option in the GUIApp.
 * 
 * 10    6/02/97 4:47p Rwyatt
 * Material loader does not create the col file that is being asked for if it does not exist, it
 * just returns.
 * Also added support for NULL material searches.
 * 
 * 9     5/29/97 4:18p Rwyatt
 * Sound Material databse initial implementation
 * 
 * 8     5/25/97 8:31p Rwyatt
 * Removed all of Brandon's RSX code and renamed AudioDeemone to AudioDaemon.
 * 
 * 7     5/07/97 7:15p Rwyatt
 * New material collision editor.
 * New volume,pitch transfer functions
 *
 ***********************************************************************************************/

