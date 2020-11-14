/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of 'Loader.hpp.'
 *
 * Bugs:
 *
 * To do:
 *		Use debug macro to disable the writing of a log in release mode.
 *		When GROFF file supports more complete information, get rid of:
 *			Use of hack structure for object attributes.
 *			Special check for "Joint" and "Mesh" names to disable loading.
 *		
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/Loader.cpp                                                $
 * 
 * 304   10/07/98 4:42a Agrant
 * Fixed instanced terrain texture sound bug
 * 
 * 303   9/27/98 2:14a Mmouni
 * Fixed load progress to be more consistent.
 * 
 * 302   9/17/98 8:56p Pkeet
 * Clut log is dumped only in GUIApp.
 * 
 * 301   9/17/98 5:37p Pkeet
 * Changed way the main clut is built.
 * 
 * 300   9/17/98 3:52p Rwyatt
 * Removed GRF load filename from the audio system
 * Added text props for the text action
 * 
 * 299   9/13/98 10:55p Kmckis
 * clear current object pointer when done loading
 * 
 * 298   9/11/98 12:38p Agrant
 * new symbols
 * 
 * 297   98.09.09 4:14p Mmouni
 * Added switch to turn off GROFF optimization.
 * 
 * 296   9/08/98 10:24p Pkeet
 * Added a text property for dust.
 * 
 * 295   98.09.07 5:25p Mmouni
 * Changed pmshLoadObject to prdtLoadObject and changed instancing to use CRenderType instead
 * of CMesh.
 * 
 * 
 * 294   9/05/98 11:10p Agrant
 * rotation ratio symbol
 * 
 * 293   9/05/98 4:39p Agrant
 * terrain pathfinding, minor activity changes
 * 
 * 292   9/04/98 7:38p Agrant
 * removed redundant assert- now covered in instance.cpp
 * 
 * 291   9/04/98 4:35p Agrant
 * added the floats text prop
 * 
 * 290   9/03/98 4:30p Shernd
 * Checking for out of disk space
 * 
 * 289   9/01/98 4:16p Asouth
 * Loop variables
 * 
 * 288   9/01/98 2:53p Rwyatt
 * Text props for sound effect action
 * 
 * 287   9/01/98 1:24p Mlange
 * Now AlwaysAsserts (instead of just dout a message) if GROFF class name was not found.
 * 
 * 286   8/31/98 1:05p Mmouni
 * Added esAmmoReallyFull text prop.
 * 
 * 285   8/28/98 6:09p Agrant
 * Query symbol
 * 
 * 284   8/27/98 1:34p Shernd
 * Improved callback updates
 * 
 * 283   98.08.26 6:58p Mmouni
 * Calls FastBumpCleanup when done loading objects.
 * Now closes the GRF file as soon as possible.
 * 
 *********************************************************************************************/


//
// Includes.
//
#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Loader.hpp"

#include "Lib/W95/Errors.h"
#include "Lib/Sys/Errors.hpp"
#include "Lib/Std/StringEx.hpp"
#include "Lib/Std/Hash.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/FileEx.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/Physics/InfoBox.hpp"
#include "Game/AI/AIInfo.hpp"
#include "Lib/GeomDBase/PartitionSpace.hpp"
#include "Lib/Physics/InfoSkeleton.hpp"
#include "Lib/GeomDBase/Skeleton.hpp"

#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"

#include "Lib/Audio/AudioDaemon.hpp"
#include "Lib/View/RasterFile.hpp"

#include "Lib/Loader/LoadTexture.hpp"
#include "Lib/Loader/PlatonicInstance.hpp"

#include "Lib/Renderer/LightBlend.hpp"

#include "TextureManager.hpp"
#include "ImageLoader.hpp"

#include "Game/AI/ActivityEnum.hpp"

#include "Game/AI/ActivityEnum.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"

#include <map>

#include <string.h>
#include <direct.h>
#include "Lib/W95/Direct3D.hpp"


extern bool bIsTrespasser;



#define OPTIMIZE_GROFF (1)


//
//  LOAD PASSES
//

/*
ZERO:	All -00 CInstances and CInstance derivatives, except for triggers
ONE:	Pickup and shoulder magnets
TWO:	All non-00 CInstances and CInstance derivatives, except for triggers
THREE:	All magnets and triggers
*/


//
// Module variables.
//

std::map<uint32, SMeshInstance, std::less<uint32> > mapMeshInstances;
float fDefaultBumpiness = 0.025f;


//*********************************************************************************************
//
// Global variables.
//

CConsoleBuffer		conLoadLog(180, 1);		// Buffer for dumping a load log file.
int					iMaxLoaderSelect = 0;

extern uint g_u_Version_Number;


//*********************************************************************************************
//
// CLoadWorld implementation.
//

bool CLoadWorld::bTextures			= true;
bool CLoadWorld::bBumps				= true;
bool CLoadWorld::bCreateWrap		= true;
bool CLoadWorld::bRebuildPartitions = false;
bool CLoadWorld::bVerbose			= false;
CGroffObjectName* CLoadWorld::pgonCurrentObject = 0;



CProfileStat psLoader("Loader");
CProfileStat psAudio("Audio", &psLoader);
CProfileStat psRAMGROFF("RAM the GROFF", &psLoader);
CProfileStat psImage("Image", &psLoader);
CProfileStat psObjects("Objects", &psLoader);
	CProfileStat psAddToDBase("Add to World", &psObjects);
	CProfileStat psPinsCreate("pinsCreate", &psObjects);
		CProfileStat psMesh("Mesh", &psPinsCreate);
			CProfileStat psPrint("Printing", &psMesh);
			CProfileStat psMiscLoad("Misc.", &psMesh);
			CProfileStat psGroffGeometry("GROFF Geom", &psMesh);
			CProfileStat psCurve("Curve Bumps", &psMesh);
				CProfileStat psBumpMatrix("Matrix", &psCurve);
			CProfileStat psNormalsNVerts("Alloc Verts", &psMesh);
			CProfileStat psFaces("Make Faces", &psMesh);
			CProfileStat psMeshConstruction("Mesh Const.", &psMesh);
				CProfileStat psMeshMerge("Merge", &psMeshConstruction);
				CProfileStat psMeshPolySetup("Poly Setup", &psMeshConstruction);
				CProfileStat psMeshVertNormals("Vert Norms", &psMeshConstruction);
			CProfileStat psTextures("Textures",&psMesh);
				CProfileStat psTextureLookup("Lookup", &psTextures);
				CProfileStat psTextureConstruct("Constructor", &psTextures);
					CProfileStat psTextureClut("Cluts", &psTextureConstruct);
			CProfileStat psMipMap("Mip Map",&psMesh);
				CProfileStat psNewRaster("new CRaster", &psMipMap);
				CProfileStat psPreImageMip("pre image stuff", &psMipMap);
			CProfileStat psShrinkWrap("Shrink Wrap", &psMesh);
			CProfileStat psImageTexture("ptexImageTexture", &psMesh);
				CProfileStat psImagePalette("Image Palette", &psImageTexture);
				CProfileStat psImageRaster("Image Raster", &psImageTexture);
				CProfileStat psImageNewTexture("New Texture", &psImageTexture);

CProfileStat psObjectInfo("Obj Info/Vtable", &psLoader);
	CProfileStat psObjNameLookup("Name Lookup", &psObjectInfo);
CProfileStat psPartitions("Partitions", &psLoader);
CProfileStat psBumpPack("Bumpmap pack", &psLoader);



	//*****************************************************************************************
	//
	// CLoadWorld constructor and destructor.
	//

	// Constructor requiring a filename.
	CLoadWorld::CLoadWorld(const char* str_filename, PFNWORLDLOADNOTIFY pfnWorldNotify, uint32 ui32_NotifyParam) 
	{
        i4Error = 0;

		// Reset D3D memory requirements.
		d3dDriver.Purge();

		// Disable scene saving for asserts.
		bAutoSaveScene = false;

		clrDefStartRamp = clrDefEndDepthDefault;
		clrDefEndRamp = clrDefEndRampDefault;
		clrDefEndDepth = clrDefEndDepthDefault;
		clrDefReflection = clrDefReflectionDefault;

		CCycleTimer ctmr_total;
		CCycleTimer ctmr;
		TCycles	cyc_temp = 0;
		psLoader.Reset();
		psPinsCreate.Add(0,1);
		psMesh.Add(0,1);
		psAddToDBase.Add(0,1);

		psPrint.Add(0,1);
		psMiscLoad.Add(0,1);
		psGroffGeometry.Add(0,1);
		psCurve.Add(0,1);
		psBumpMatrix.Add(0,1);
		psNormalsNVerts.Add(0,1);
		psFaces.Add(0,1);
		psMeshConstruction.Add(0,1);
			psMeshMerge.Add(0,1);
			psMeshPolySetup.Add(0,1);
			psMeshVertNormals.Add(0,1);
		psTextures.Add(0,1);
		psTextureLookup.Add(0,1);
		psTextureConstruct.Add(0,1);
		psTextureClut.Add(0,1);
		psMipMap.Add(0,1);
			psPreImageMip.Add(0,1);
			psNewRaster.Add(0,1);

		psShrinkWrap.Add(0,1);
		psImageTexture.Add(0,1);
			psImagePalette.Add(0,1);
			psImageRaster.Add(0,1);
		psImageNewTexture.Add(0,1);


		// Have not selected anything yet!
		iNumSelected = 0;


		//
		// Attempt to open the 'Groff' file.
		//
		ctmr();
		dout << "Open File: " << str_filename << "\n";

        gioFile.pfnWorldNotify = pfnWorldNotify;
        gioFile.ui32_NotifyParam = ui32_NotifyParam;
		if (!gioFile.bOpenFile(str_filename))
		{
			// Output error message.
			TerminalError(ERROR_UNABLETO_OPENGROFF, true, str_filename);
            i4Error = -1;
			return;
		}
		dout << "Open File: \t";
		cyc_temp = ctmr();
		psRAMGROFF.Add(cyc_temp,1);
		dout << cyc_temp * ctmr.fSecondsPerCycle() << " seconds.\n";


		//
		//  Image loader (.swp file)
		//

		//
		// Cretae the virtual image loader class. If we are using the virtual loader the
		// drive that contains the GRF file is now free as the GRF file is in memory.
		// Therefore we are free the thread the function that creates the local swap file,
		// this is a real advanatge as we hide the decompress and copy time within the
		// groff load time.
		// Create a local class who's scope is valid for the whole load process. This class
		// can be used by any part of the load system trough the classes static members.
		//
		ctmr();
		dout << "Image Loader:  " << str_filename << "\n";
		CLoadImageDirectory		lidImageDir(str_filename, pfnWorldNotify, ui32_NotifyParam);
		dout << "Image Loader: \t";
		cyc_temp = ctmr();
		psImage.Add(cyc_temp,1);
		dout << cyc_temp * ctmr.fSecondsPerCycle() << " seconds.\n";

        if (lidImageDir.i4Error < 0)
        {
            i4Error = lidImageDir.i4Error;
            return;
        }

		//
		//  Object header stuff.
		//
		ctmr();
		dout << "GROFF version: " << g_u_Version_Number << '\n';

		dout << "Load Object Info and vtable: \n";


		// Attempt to load the Groff object information.
		if (!gioFile.bObjectInfo(goiInfo, vtableTable))
		{
			// Output error message.
			TerminalError(ERROR_UNABLETO_READGROFFINFO, true, str_filename);

			return;
		}
		dout << "Load Object Info and vtable: \t";
		cyc_temp = ctmr();
		psObjectInfo.Add(cyc_temp,1);
		dout << cyc_temp * ctmr.fSecondsPerCycle() << " seconds.\n";

		//
		//  A bunch of random debug info and other stuff.
		//
		ctmr();
		dout << "Miscellaneous:\n";

		// Add this to our scene list.
		wWorld.AddGroff(str_filename);

		// Remove current pure spatial partitions and flatten the partition tree.
		wWorld.FlattenPartitions();
		
		// Open a log file with default settings.
		conLoadLog.OpenFileSession("LoadLog.txt", true);
		conLoadLog.SetTabSize(4);
		conLoadLog.Print("Loading file ");
		conLoadLog.Print(str_filename);
		conLoadLog.Print("...\n\n");

		// Cache the file name for error reporting.
		SetCacheText(str_filename);

		//
		// Change to the directory of the file, so that texture loading will work correctly.
		//
		char str_path[_MAX_DRIVE + _MAX_DIR], str_dir[_MAX_DIR], str_name[_MAX_FNAME];

		_splitpath(str_filename, str_path, str_dir, str_name, 0);
		strcat(str_path, str_dir);

		CPushDir pshd(str_path);

		// Set the mipmap directory path.
		// Do this even if we have an image, because then we can snag any mips missing from the .swp file conventionally if we need to.

        bool    bMipCreated;

		bMipCreated = SetMipDirectory(str_name);

#if VER_TEST
		// Make a filename:  "Map\Name.lst"
		char str_no_suffix[_MAX_DIR + _MAX_FNAME];

		strcpy(str_no_suffix, "Map\\");
		strcat(str_no_suffix, str_name);
		strcat(str_no_suffix, ".lst");

		CConsoleBuffer		conListFile(180, 1);		// Buffer for dumping a load log file.

        if (bMipCreated)
        {
		    conListFile.OpenFileSession(str_no_suffix, true);
        }
#endif

		CHandle h_handle;
		// Loop through all the objects and setup their valuetable entries.
		uint u_handle;
		for (u_handle = 0; u_handle < goiInfo.uObjectCount; u_handle++)
		{
			// Is this a version 12 GROFF file?
			if (g_u_Version_Number == 11)
			{	
				// Must use version 12!
				AlwaysAssert(false);
			}

			// HACK HACK HACK
			// Need multiple passes to handle weird stuff like magnets and triggers for now...
			// This needs a better method.
			CGroffObjectName* pgon = &goiInfo.gonObjectList[u_handle];
			pgon->bDoneLoading = false;

#if VER_TEST
			char buffer[256];
			// Is this an ideal object?
			if (bGetPlatonicIdealName(pgon->strObjectName, buffer))
			{
				// Send object name to .lst file.
				conListFile.Print("%s\n", pgon->strObjectName);
			}
#endif
		}
#if VER_TEST
		conListFile.CloseFileSession();
#endif

		// Resolve all symbol handles.
		InitHandleTable();

		dout << "Miscellaneous:\t";
		dout << ctmr() * ctmr.fSecondsPerCycle() << " seconds.\n";

		bool b_done = true;

		//
		//  The actual object loader.
		//
		ctmr();

        int32 icObjects = 0;

        if (pfnWorldNotify)
        {
			// Show completion 15%-90%
			int i_percent = 15 + (icObjects * 75) / goiInfo.uObjectCount;

			(pfnWorldNotify)(ui32_NotifyParam, 1, i_percent, 100);
        }

		// Make sure we don't loop infinitely.
		dout << "Objects: \n";
		iLoadPass = 0;
		do
		{
			// Reset the flag.
			b_done = true;

			char str_dots[] = "..............................";
			int i_max_dots = strlen(str_dots) - 1;

			char *str_end_dots = str_dots + i_max_dots - 1;

			if (i_max_dots > goiInfo.uObjectCount)
				i_max_dots = goiInfo.uObjectCount + 1;
			float f_meshes_per_dot = goiInfo.uObjectCount / (float)i_max_dots;
			int i_dots = i_max_dots;
			float f_meshes = 0.0f;

			// Loop through all the objects and load them.
			for (u_handle = 0; u_handle < goiInfo.uObjectCount; u_handle++)
			{
				// Only try to load if we haven't loaded it already.
				if (!goiInfo.gonObjectList[u_handle].bDoneLoading)
				{
					//CMemCheck	mem("Object Load", MEM_DIFF_STATS|MEM_DIFF_DUMP);
					CCycleTimer ctr_object;

					// Assume that we are going to succeed.  If we fail in a way that a later pass
					// will help, this flag will be unset deep inside the loader.
					goiInfo.gonObjectList[u_handle].bDoneLoading = true;

					// Load the object to the world database.
					LoadObjectToWorld(goiInfo.gonObjectList[u_handle]);

					// If the object load failed, try again next pass.
					if (!goiInfo.gonObjectList[u_handle].bDoneLoading)
					{
						b_done = false;
						AlwaysAssert(iLoadPass < 5);
					}
                    else
                    {
                        icObjects++;

                        if (pfnWorldNotify)
                        {
							// Show completion 15%-90%
							int i_percent = 15 + (icObjects * 75) / goiInfo.uObjectCount;

							(pfnWorldNotify)(ui32_NotifyParam, 1, i_percent, 100);
                        }
                    }

					uint32 u4_cycles = ctr_object();
					if (u4_cycles * ctr_object.fSecondsPerCycle() > 1.0f)
					{
						dout << goiInfo.gonObjectList[u_handle].strObjectName << " is too slow: " << u4_cycles * ctr_object.fSecondsPerCycle() << " seconds.\n";
					}
				}
/*
				// dout a fake status bar.
				f_meshes += 1.0f;
				while (f_meshes > f_meshes_per_dot)
				{
					// Let the viewer know that we're closer.
					i_dots--;
					AlwaysAssert(i_dots >= 0);
					f_meshes -= f_meshes_per_dot;

					char *str_dots_substring = str_end_dots - i_dots;

					AlwaysAssert(str_dots_substring[0] == '.');
					{
						dout << str_dots_substring << '\n';
					}
				}
*/

			}

			dout << "LOAD PASS " << iLoadPass << '\n';
			iLoadPass++;
		}
		while (!b_done);
		dout << "Objects: \t";
		cyc_temp = ctmr();
		psObjects.Add(cyc_temp,1);
		dout << cyc_temp * ctmr.fSecondsPerCycle() << " seconds.\n";

		// Free up any temporary memory allocated by the bump maps.
		FastBumpCleanup();

		#if (OPTIMIZE_GROFF)
		{
			//
			// Attempt to save the optimized GRF under new name.
			//
			char str_newname[_MAX_PATH];
			strcpy(str_newname, str_filename);

			// Strip off file extension && add stuff on.
			int i_ext = strlen(str_newname); 
			while (i_ext > 0 && str_newname[i_ext] != '.')
				i_ext--;
			strcpy(str_newname+i_ext, ".gop");

			// Attemp to save it.
			if (gioFile.bSaveOptimizedFile(str_newname))
			{
				// Close the GRF file.
				gioFile.bCloseFile();

				char str_oldname[_MAX_PATH];
				strcpy(str_oldname, str_filename);
				strcpy(str_oldname+i_ext, ".old");

				// Delete old file if it exists.
				remove(str_oldname);

				// Rename to current groff to .old.
				dout << "Renaming " << str_filename << " to " << str_oldname << "\n";
				rename(str_filename, str_oldname);

				// Rename optimized groff to .grf.
				dout << "Renaming " << str_newname << " to " << str_filename << "\n";
				rename(str_newname, str_filename);
			}
			else
			{
				// Probably we already have an optimzed GRF.  Go ahead and close the
				// file now even thought it would be closed later so that we free up 
				// the memory taken by the GRF image earlier.
				gioFile.bCloseFile();
			}
		}
		#endif

		// Restore original directory.
		// Clear the error cache.
		SetCacheText();

		//
		//  Rebuild the partitions.
		//
		ctmr();
		dout << "Building partitions:\n";

		if (bRebuildPartitions)
		{
			CCycleTimer ctr;

			// Remove current pure spatial partitions and flatten the partition tree.
			dout << "Flattening partitions: \n";
			wWorld.FlattenPartitions();
			dout << "Flattening partitions: \t";
			dout << ctr() * ctr.fSecondsPerCycle() << " seconds.\n";

			// Also reset the pure spatial partition heap.
			CPartitionSpace::ResetHeap();

			// Build the partition tree using a fast system.
			dout << "Building partition: \n";
			wWorld.BuildOptimalPartitions(0);
			dout << "Building partition: \t";
			dout << ctr() * ctr.fSecondsPerCycle() << " seconds.\n";
/*
			dout << "Building terrain partition: \n";
			wWorld.BuildTerrainPartitions();
			dout << "Building terrain partition: \t" << ctr() * ctr.fSecondsPerCycle() << " seconds.\n";

			dout << "Building trigger partition: \n";
			wWorld.BuildTriggerPartitions();
			dout << "Building trigger partition: \t" << ctr() * ctr.fSecondsPerCycle() << " seconds.\n";
*/
		}
		else
		{
			wWorld.InitializePartitions();
		}

		dout << "Partitions:\t";
		cyc_temp = ctmr();
		psPartitions.Add(cyc_temp,1);
		dout << cyc_temp * ctmr.fSecondsPerCycle() << " seconds.\n";

		//
		// pack any bump maps that have just been loaded
		//
		dout << "Pack bump maps:\t";
		gtxmTexMan.PackBumpMaps();
		dout << "Pack bump maps:\t";
		cyc_temp = ctmr();
		psBumpPack.Add(cyc_temp,1);
		dout << cyc_temp * ctmr.fSecondsPerCycle() << " seconds.\n";

		//
		//  Summary info
		//
		dout << "Total:\t";
		cyc_temp = ctmr_total();
		psLoader.Add(cyc_temp,1);
		dout << cyc_temp * ctmr_total.fSecondsPerCycle() << " seconds.\n";

		dout << "# Instances Loaded: "         << wWorld.ppartPartitionList()->iCountInstances() << " \n";
		dout << "# Terrain Instances Loaded: " << wWorld.ppartTerrainPartitionList()->iCountInstances() << " \n";

#if VER_TEST
		dout << "Time in find obj: " << CGroffObjectInfo::fSecsInFindObject() << " secs.\n";
#endif

		// Now dump the profile stats-
		CStrBuffer strbuf(2000);
		psLoader.WriteToBuffer(strbuf);
		dout << strbuf;

		// Enable scene saving for asserts.
		bAutoSaveScene = true;

		strbuf.Reset();
		dout << strbuf;
	}

	// Destructor.
	CLoadWorld::~CLoadWorld()
	{
		// Close the log file.
		conLoadLog.Print("\nFinished loading!");
		conLoadLog.CloseFileSession();

		// Zero the current object pointer for safety.
		pgonCurrentObject = 0;

		// Send out message informing the system a GROFF file has finished loading.
		CMessageSystem msgsys(escGROFF_LOADED);
		msgsys.Send();

		// Insert code to remove unused meshes here.
		bool b_done = false;
		int i_discarded_meshes = 0;

		dout << "Total unique meshes: " << mapMeshInstances.size() << "\n";

		std::map<uint32, SMeshInstance, std::less<uint32> >::iterator i;

		while(!b_done)
		{
			// If nothing happens, we're done.
			b_done = true;

			i = mapMeshInstances.begin();
			for (; i != mapMeshInstances.end(); ++i)
			{
				if ((*i).second.prdtMesh->uRefs == 1)
				{
					mapMeshInstances.erase(i);
					b_done = false;
					i_discarded_meshes++;
					break;
				}
			}
		}

		dout << "Discarded Meshes: " << i_discarded_meshes << "\n";

		extern std::map< uint32, rptr<CTexture>, std::less<uint32> > mapTextures;
		dout << "Total unique textures: " << mapTextures.size() << "\n";
		
		// Write the names of the loaded textures.
		//DumpTextureLogFile();
		
		int i_discarded_textures = 0;

		std::map< uint32, rptr<CTexture>, std::less<uint32> >::iterator i_tex;
		b_done = false;
		
		while(!b_done)
		{
			// If nothing happens, we're done.
			b_done = true;

			i_tex = mapTextures.begin();
			for (; i_tex != mapTextures.end(); ++i_tex)
			{
				if ((*i_tex).second->uRefs == 1)
				{
					mapTextures.erase(i_tex);
					b_done = false;
					i_discarded_textures++;
					break;
				}
			}
		}

		dout << "Discarded Textures: " << i_discarded_textures << "\n";

		if (!bIsTrespasser)
			pcdbMain.PrintDebugInfo();
	}


	//*****************************************************************************************
	//
	// CLoadWorld member functions.
	//
	
	//*****************************************************************************************
	void CLoadWorld::PurgeMeshes()
	{
		mapMeshInstances.erase(mapMeshInstances.begin(), mapMeshInstances.end());
	}


	//*****************************************************************************************
	//
	// CLoadWorld protected member functions.
	//

	//*****************************************************************************************
	//
	void CLoadWorld::LoadObjectToWorld
	(
		CGroffObjectName& gon		// Object to load.
	)
	//
	// Loads an object from the groff file.
	// Notes:
	//		Bug!  Do not copy a CGroffObjectName value; copy references only
	//		Otherwise it will delete stuff twice.
	//
	//**********************************
	{
		CCycleTimer ctmr;

		pgonCurrentObject = &gon;

		// HACK HACK HACK remove me after text prop mesh loading is in.
		// Warning!  This uses hard-coded strings to check for which kinds of meshes to load.
		if (CBeginsWith(gon.strObjectName) == strSUBOBJECT_PREFIX)
		{
			// These are special sub-objects that are loaded when the main object is loaded.
			// Do not load them here.
			return;
		}

		SetCacheText(gon.strObjectName);

		CInstance* pins = 0;

		// First, perform name-based instancing duties.
		char ac_platonic_ideal[256];
		bool b_is_ideal = bGetPlatonicIdealName(gon.strObjectName, ac_platonic_ideal);
		uint32 u4_platonic_hash = 0;

		// If this object has a platonic ideal already in the list, instance it.
		if (ac_platonic_ideal[0] != '\0')
		{
			u4_platonic_hash = u4Hash(ac_platonic_ideal,0,true);

			// Then this object has a platonic ideal!
			TMapHashPins::iterator i = tmPlatonicIdeal.find(u4_platonic_hash);

			if (i != tmPlatonicIdeal.end())
			{
				// Loading a cheap copy.
				// Must wait until pass 2 to allow all pickup magnets to load.
				if (iLoadPass < 2)
				{
					gon.bDoneLoading = false;
					return;
				}

				// Already here!  Instance it.
				pins = 	(*i).second->pinsCopy();

				// Did the copy work?
				if (pins)
				{
					// Yes! Finish the instance by loading PRS.
					CPresence3<> pr3 = ::pr3Presence(gon);

					if (ptCast<CTerrainObj>(pins))
					{
						pr3.v3Pos.tZ = 0;						
					}

					pins->SetPresence(pr3);

					pins->SetInstanceName(gon.strObjectName);
					pins->ClearHierarchyPointers();


#if VER_TEST
					// DEBUG assistance crap to set up marker system.
					extern CInstance *pinsMasterMarker;
					if (pinsMasterMarker == (*i).second)
					{
						// We are copying the marker master.
						extern int iMaxMarker;

						// Get the object number after the dash.
						char *str = gon.strObjectName + strlen(gon.strObjectName);

						int i_num = 0;

						while(str > gon.strObjectName && *str != '-')
						{
							str = str - 1;
						}

						Assert(*str = '-');
						str++;

						sscanf(str, "%d", &i_num);

						AlwaysAssert(i_num > 0);
						AlwaysAssert(i_num < 10000);

						if (iMaxMarker < i_num)
							iMaxMarker = i_num;
					}
#endif

				}
				else
				{
					// No.  Construct an instance.
					pins = 	CInstance::pinsCreate(&gon, this, gon.hAttributeHandle, &vtableTable, (*i).second->pinfoGetInfo());
				}

			}
			else if (!b_is_ideal)
			{
				// If we don't have the base instance yet, complain.
				if (iLoadPass > 1)
				{
					// We cannot wait any longer!

					dout << "Missing " << ac_platonic_ideal << "! Cannot instance " << gon.strObjectName << ".\n";
					// Now fall through and load it.
				}
				else
				{
					// The object's ideal is not yet loaded, and it is not itself ideal.  Try again later.
					gon.bDoneLoading = false;
					return;
				}
			}
		}
		else
		{
			dout << "Cannot instance " << gon.strObjectName << ", no ideal name.\n";
		}

		// Either the object is ideal, or it has no ideal, or it has already been insatnced
		if (!pins)	//if not instanced, make a new one.
		{
			ctmr();

			pins = 	CInstance::pinsCreate(&gon, this, gon.hAttributeHandle, &vtableTable, 0);
			
			psPinsCreate.Add(ctmr());
		}


		// Add the shape to the world database.
		if (pins != pinsINVALID)
		{
			if (pins)
			{
				//
				// If the instance is an object that always faces the camera, adjust its
				// bounding volume.
				//
				pins->SetAlwaysFaceBV();

				// Set a unique value representing the object.

				//
				// Notes: Currently the SetUniqueHandle function generates a handle value
				// automatically from instance data. This will changed to a value from Groff.
				//
				pins->SetUniqueHandle(0);

				// Add to the world database.
				if (pins->bIsBackdrop())
				{
					// Add to the backdrop partition.
					wWorld.AddBackdrop(pins);
				}
				else
				{
					// Add to the object partition.
					ctmr();
					wWorld.Add(pins,true);
					psAddToDBase.Add(ctmr());

					if (iNumSelected < iMaxLoaderSelect)
					{
						wWorld.Select(pins, true);
						++iNumSelected;
					}

					// If ideal, add the object to the ideal list.
					if (b_is_ideal)
						AddToIdealList(u4_platonic_hash, pins);
				}
			}
			return;
		}

		if (g_u_Version_Number > 11)
		{
			// Class or handle not found in the new and improved pinsCreate!  
			dout << "Class Not Found! Cannot create " << gon.strObjectName << '\n';
			return;
		}

		//
		// The rest of this is so that classes which have not been converted will work.
		//
		AlwaysAssert(false);
	}

	//*****************************************************************************************
	//
	rptr<CMesh> CLoadWorld::pmshLoadMesh
	(
		const CGroffObjectName& gon,		// Mesh to load.
		CPresence3<>* ppr3_adjust,			// Adjustment to mesh's presence (due to biomesh uncentering).
		bool b_animating
	)
	//
	// Returns mesh from a 'Groff' file.
	//
	//**************************************
	{
		AlwaysAssert(false);

		return rptr0;
	}

	//*****************************************************************************************
	int CLoadWorld::iFindSymbol(const CEasyString& estr)
	{
		CNewSymbolTable* psym = &vtableTable;
		CHandle h_symbol = (*psym)[estr];
		
		for (int i = esEND - 1; i >= 0; --i)
		{
			if (h_symbol == hSymbol(ESymbol(i)))
				return i;
		}
		return -1;
	}


	//*****************************************************************************************
	void CLoadWorld::InitHandleTable()
	{
#if VER_TEST
		int i;
		for (i = esEND - 1; i>= 0; i--)
		{
			abSymbolInit[i] = false;
		}
#endif


		InitHandle(esClass,			"Class");
		InitHandle(esMesh,			"Mesh");
		InitHandle(esPhysics,		"Physics");

		InitHandle(esMin,			"Min");
		InitHandle(esMax,			"Max");

		InitHandle(esInitialState,	"InitialState");
		InitHandle(esMinHighTime,	"MinHighTime");
		InitHandle(esMaxHighTime,	"MaxHighTime");
		InitHandle(esMinLowTime,	"MinLowTime");
		InitHandle(esMaxLowTime,	"MaxLowTime");
		InitHandle(esToggle,	     "Toggle");


		InitHandle(esClose,			"Close");
		InitHandle(esDecay,			"Decay");
		InitHandle(esSpangCount,	"SpangCount");
		InitHandle(esArmour,			"Armour");
		InitHandle(esDamageMultiplier,	"DamageMultiplier");
		InitHandle(esDamagePerSecond,	"DamagePerSecond");
		InitHandle(esMass,			"Mass");
		InitHandle(esDensity,		"Density");
		InitHandle(esElasticity,	"Elasticity");
		InitHandle(esFloats,		"Floats");
		InitHandle(esFriction,		"Friction");
		InitHandle(esFrozen,		"Frozen");
		InitHandle(esFile,			"File");
		InitHandle(esImpulse,		"Impulse");
		InitHandle(esLocation,		"Location");
		InitHandle(esMaterial,		"Material");
		InitHandle(esMoveable,		"Moveable");
		InitHandle(esPlanted,		"Planted");
		InitHandle(esOpen,			"Open");
		InitHandle(esRatio,			"Ratio");
		InitHandle(esRotationRatio,	"RotationRatio");
		InitHandle(esReset,			"Reset");
		InitHandle(esShadow,		"Shadow");
		InitHandle(esSmall,			"Small");
		InitHandle(esSoundMaterial,	"SoundMaterial");
		InitHandle(esSubstitute,	"Substitute");
		InitHandle(esTangible,		"Tangible");
		InitHandle(esTarget,		"Target");
		InitHandle(esType,			"Type");
		InitHandle(esValue,			"Value");
		InitHandle(esHintID,		"HintID");
		InitHandle(esVolume,		"Volume");

		InitHandle(esScale,			"Scale");
		InitHandle(esMinScale,		"MinScale");
		InitHandle(esMaxScale,		"MaxScale");
		InitHandle(esScaleLimit,	"ScaleLimit");
		InitHandle(esFogNear,		"FogNear");
		InitHandle(esFogFar,		"FogFar");
		InitHandle(esWindSpeedX,	"WindSpeedX");
		InitHandle(esWindSpeedY,	"WindSpeedY");
		InitHandle(esSubDivision,	"SubDivision");
		InitHandle(esFillScreen,	"FillScreen");

		// Animal combat hit point stuff
		InitHandle(esCriticalHit,	"CriticalHit");
		InitHandle(esHitPoints,		"HitPoints");
		InitHandle(esMaxHitPoints,	"MaxHitPoints");
		InitHandle(esRegeneration,	"Regeneration");
		InitHandle(esReallyDie,		"ReallyDie");
		InitHandle(esDieRate,		"DieRate");
		InitHandle(esOuch,			"Ouch");
		InitHandle(esFall,			"Fall");

		//
		// Distance culling parameters.
		//
		InitHandle(esCulling,                "Culling");
		InitHandle(esShadowCulling,          "ShadowCulling");
		InitHandle(esCullingMaxDist,         "CullMaxDist");
		InitHandle(esCullingMaxRadius,       "CullMaxRadius");
		InitHandle(esCullingMaxDistShadow,   "CullMaxDistShadow");
		InitHandle(esCullingMaxRadiusShadow, "CullMaxRadiusShadow");
		InitHandle(esPriority,               "Priority");

		//
		// Direct3D parameters.
		//
		InitHandle(esD3DHardwareOnly,        "HardwareOnly");
		InitHandle(esD3DSoftwareOnly,        "SoftwareOnly");
		InitHandle(esD3DNoLowRes,            "NoLowRes");

		//
		// Rendering paramaters.
		//
		InitHandle(esCameraFOV,			"CameraFOV");
		InitHandle(esPixelError,		"PixelError");
		InitHandle(esSubdivisionLen,	"SubdivisionLen");
		InitHandle(esAltPixelError,		"AltPixelError");
		InitHandle(esAltSubdivisionLen,	"AltSubdivisionLen");

		//
		// Object rendering parameters.
		//
		InitHandle(esAlwaysFace,        "AlwaysFace");


		//
		// Image caching parameters.
		//
		InitHandle(esNoCache,           "NoCache");
		InitHandle(esNoCacheAlone,      "NoCacheAlone");
		InitHandle(esCacheMultiplier,   "CacheMul");

		//
		// Teleport trigger parameters.
		//
		InitHandle(esTeleportDestObjectName, "TeleportDestObjectName");
		InitHandle(esHeightRelative,         "HeightRelative");
		InitHandle(esOnTerrain,				 "OnTerrain");
		InitHandle(esSetPosition,            "SetPosition");
		InitHandle(esSetOrientation,         "SetOrientation");

		//
		// Trigger parameters.
		//
//		InitHandle(esCTrigger,			"CTrigger");
		InitHandle(esTFireDelay,		"FireDelay");
		InitHandle(esTFireCount,		"FireCount");
		InitHandle(esTFireExpression,	"FireExpression");
		InitHandle(esTProb,				"Probability");
		InitHandle(esTFireZero,			"FireAtZero");
		InitHandle(esTResetFire,		"ResetFire");
		InitHandle(esTProcessStyle,		"ProcessStyle");
		InitHandle(esTRepeatPeriod,		"RepeatPeriod");
		InitHandle(esTBoundVol,			"BoundVol");
		InitHandle(esTSequenceDelayMin,	"SequenceDelayMin");
		InitHandle(esTSequenceDelayMax,	"SequenceDelayMax");

		// Collision trigger properties
		InitHandle(esElement1,			"Element1");
		InitHandle(esElement2,			"Element2");
		InitHandle(esSoundMaterial1,	"SoundMaterial1");
		InitHandle(esSoundMaterial2,	"SoundMaterial2");
		InitHandle(esMinVelocity,		"MinVelocity");
		InitHandle(esMaxVelocity,		"MaxVelocity");

		InitHandle(esCreatureDie,		"CreatureDie");
		InitHandle(esCreatureWake,		"CreatureWake");
		InitHandle(esCreatureSleep,		"CreatureSleep");
		InitHandle(esCreatureCriticalDamage, "CreatureCriticalDamage");
		InitHandle(esCreatureDamagePoints,	 "CreatureDamagePoints");
		InitHandle(esEvaluateAll,		"EvaluateAll");

		InitHandle(esTMasterVolumeMin,	"MasterVolumeMin");
		InitHandle(esTMasterVolumeMax,	"MasterVolumeMax");
	
		InitHandle(esTActionType,		"ActionType");
		InitHandle(esTActionSample,		"Sample");
		InitHandle(esTActionSpatial,	"SpatialType");
		InitHandle(esTActionVolume,		"Volume");
		InitHandle(esTActionStopAfterFade, "StopAfterFade");
		InitHandle(esTActionAtten,		"Attenuation");
		InitHandle(esTActionBoundaryVol,"BoundaryVolume");
		InitHandle(esTActionMaxVolDist, "MaxVolDistance");
		InitHandle(esTActionEmitter,	"Emitter");
		InitHandle(esTActionFrustum,	"Frustum");
		InitHandle(esTActionMute,		"Mute");
		InitHandle(esTActionOutsideVol,	"OutsideVolume");
		InitHandle(esTActionSampleLoop,	"Looped");
		InitHandle(esTActionLoopCount,	"LoopCount");
		InitHandle(esTActionMaxDist,	"MaximumDistance");
		InitHandle(esTActionVolFade,	"VolumeFader");
		InitHandle(esTActionAttach,		"Attach");
		InitHandle(esTActionBitmap,		"Bitmap");
		InitHandle(esTActionCenter,		"Center");
		InitHandle(esTActionDiscard,	"Discard");
		InitHandle(esTActionXP,			"XPos");
		InitHandle(esTActionYP,			"YPos");
		InitHandle(esTActionFogType,	"FogType");
		InitHandle(esTActionFogPower,	"FogPower");
		InitHandle(esTActionFogHalf,	"FogHalf");
		InitHandle(esTActionAmbientLight,  "AmbientLight");
		InitHandle(esTActionFarClipPlane,  "FarClipPlane");
		InitHandle(esTActionNearClipPlane, "NearClipPlane");

		InitHandle(esUseObject,				"UseObject");
		InitHandle(esPickUpObject,			"PickUpObject");
		InitHandle(esPutDownObject,			"PutDownObject");
		InitHandle(esTriggerName,			"TriggerName");

		// Terrain parameters.
		InitHandle(esTActionTrrPixelTol,		"TrrPixelTol");
		InitHandle(esTActionTrrPixelTolFar,		"TrrPixelTolFar");
		InitHandle(esTrrNoShadowDist,			"TrrNoShadowDist");
		InitHandle(esTrrNoTextureDist,			"TrrNoTextureDist");
		InitHandle(esTrrNoDynTextureDist,		"TrrNoDynTextureDist");
		InitHandle(esTrrMovingShadows,			"TrrMovingShadows");

		// Depth Sort parameters.
		InitHandle(esTActionNearTolerance,		"NearTolerance");
		InitHandle(esTActionFarTolerance,		"FarTolerance");
		InitHandle(esTActionNearZ,				"NearZ");
		InitHandle(esTActionFarZ,				"FarZ");
		InitHandle(esTActionFarZNo,				"FarZNo");
		InitHandle(esTActionSortPixelTol,		"SortPixelTol");
		InitHandle(esTActionMaxNumToSort,		"MaxNumToSort");
		InitHandle(esTActionSort2PartAt,		"Sort2PartAt");
		InitHandle(esTActionSort4PartAt,		"Sort4PartAt");
		InitHandle(esTActionTerrNearTolerance,	"TerrNearTolerance");
		InitHandle(esTActionTerrFarTolerance,	"TerrFarTolerance");
		InitHandle(esTActionTerrNearZ,			"TerrNearZ");
		InitHandle(esTActionTerrFarZ,			"TerrFarZ");
		InitHandle(esTActionUseSeperateTol,		"UseSeperateTol");

		// Image cache parameters.
		InitHandle(esTActionPixelRatio,	"PixelRatio");
		InitHandle(esTActionMinPixels,	"MinPixels");
		InitHandle(esTActionCacheAge,	"CacheAge");
		InitHandle(esTActionCacheActive,"CacheActive");
		InitHandle(esTActionCacheIntersect,	"CacheIntersect");

//		InitHandle(esCLocationTrigger,	"CLocationTrigger");
		InitHandle(esLTEnterCount,		"CreatureEnterCount");
		InitHandle(esLTLeaveCount,		"CreatureLeaveCount");
		InitHandle(esLTPlayerIn,		"PlayerInTrigger");
		InitHandle(esLTPlayerLeave,		"PlayerLeaveTrigger");
		InitHandle(esLTPlayerEnter,		"PlayerEnterTrigger");
		InitHandle(esLTObjectIn,		"ObjectInTrigger");
		InitHandle(esLTObjectLeave,		"ObjectLeaveTrigger");
		InitHandle(esLTObjectEnter,		"ObjectEnterTrigger");
		InitHandle(esLTCreatureIn,		"CreatureInTrigger");
		InitHandle(esLTCreatureLeave,	"CreatureLeaveTrigger");
		InitHandle(esLTCreatureEnter,	"CreatureEnterTrigger");
		InitHandle(esLTTriggerActivate,	"TriggerActivate");
		InitHandle(esLTPointTrigger,	"PointTrigger");

		// Stuff need for water trigger, plus generally usefull.
		InitHandle(esObjectName,	"ObjectName");
		InitHandle(esEnable,		"Enable");
		InitHandle(esQuery,			"Query");
		InitHandle(esLevelName,		"LevelName");
		InitHandle(esAnimationName,	"AnimationName");

		// For sequence trigger.
		InitHandle(esSequenceListenNames, "SequenceListenNames");
		InitHandle(esSequenceOrderNames, "SequenceOrderNames");
		InitHandle(esSequenceEvalNowNames, "SequenceEvalNowNames");
		InitHandle(esSequenceFalseTriggerName, "SequenceFalseTriggerName");


		// Coordinate symbols.
		InitHandle(esX,	"X");
		InitHandle(esY,	"Y");
		InitHandle(esZ,	"Z");

		// Magnet symbols.
		InitHandle(esBreakable,		"Breakable");
		InitHandle(esBreakStrength,	"BreakStrength");
		InitHandle(esDelta,			"Delta");
		InitHandle(esDrive,			"Drive");
		InitHandle(esDelay,			"Delay");
		InitHandle(esAngleMin,		"AngleMin");
		InitHandle(esAngleMax,		"AngleMax");
		InitHandle(esCMagnet,		"CMagnet");
		InitHandle(esInstance,		"Instance");
		InitHandle(esGender,		"Gender");
		InitHandle(esMagnet,		"Magnet");
		InitHandle(esMasterObject,	"MasterObject");
		InitHandle(esRestoreStrength, "RestoreStrength");
		InitHandle(esSlaveMagnet,	"SlaveMagnet");
		InitHandle(esSlaveObject,	"SlaveObject");
		InitHandle(esXTFree,		"XTFree");
		InitHandle(esYTFree,		"YTFree");
		InitHandle(esZTFree,		"ZTFree");
		InitHandle(esXFree,			"XFree");
		InitHandle(esYFree,			"YFree");
		InitHandle(esZFree,			"ZFree");
		InitHandle(esHandPickup,	"HandPickup");
		InitHandle(esShoulderHold,	"ShoulderHold");
		
		// Water object.
		InitHandle(esResolutionRatio, "ResolutionRatio");

		// Disturbance symbols
		InitHandle(esStrength,		"Strength");
		InitHandle(esInterval,		"Interval");
		InitHandle(esRadius,		"Radius");

		InitHandle(esAudioEnvironment,	"AudioEnvironment");
		InitHandle(esReverbVolume,		"ReverbVolume");
		InitHandle(esReverbDecay,		"ReverbDecay");
		InitHandle(esReverbDamping,		"ReverbDamping");

		// Mesh Symbols
		InitHandle(esBumpiness,		"Bumpiness");
		InitHandle(esBumpmaps,		"Bumpmaps");
		InitHandle(esCacheIntersecting,	"CacheIntersecting");
		InitHandle(esDetail1,		"Detail1");
		InitHandle(esDetail2,		"Detail2");
		InitHandle(esDetail3,		"Detail3");
		InitHandle(esDetail4,		"Detail4");
		InitHandle(esDetailShadow,  "DetailShadow");
		InitHandle(esDiffuse,		"Diffuse");
		InitHandle(esEmissive,		"Emissive");
		InitHandle(esTexture,		"Texture");
		InitHandle(esMerge,			"Merge");
		InitHandle(esNormals,		"Normals");
		InitHandle(esCurved,		"Curved");
		InitHandle(esNumDoubleJoints,		"NumDoubleJoints");
		InitHandle(esNumJoints,		"NumJoints");
		InitHandle(esPRadius,		"PRadius");
		InitHandle(esPVA,			"PVA");
		InitHandle(esReflect,		"Reflect");
		InitHandle(esRefract,		"Refract");
		InitHandle(esRefractIndex,	"RefractIndex");
		InitHandle(esSpecular,		"Specular");
		InitHandle(esSpecularAngle,	"SpecularAngle");
		InitHandle(esSplit,			"Split");
		InitHandle(esVisible,		"Visible");
		InitHandle(esWrap,			"Wrap");
		InitHandle(esUnlit,			"Unlit");
		
		// Clut control.
		InitHandle(esClutStartColor,			"ClutStartColor");
		InitHandle(esClutStopColor,				"ClutStopColor");
		InitHandle(esClutReflectionColor,		"ClutReflectionColor");
		InitHandle(esDefaultClutStartColor,		"DefaultClutStartColor");
		InitHandle(esDefaultClutStopColor,		"DefaultClutStopColor");
		InitHandle(esDefaultClutReflectionColor,"DefaultClutReflectionColor");

		// Alpha water clut values.
		InitHandle(esAlphaWaterProperties,		"AlphaWaterProperties");
		InitHandle(esNonAlphaWaterProperties,	"NonAlphaWaterProperties");

		// Dino helpers
		InitHandle(esBody,			"Body");
		InitHandle(esFoot,			"Foot");
		InitHandle(esLeftFoot,		"LeftFoot");
		InitHandle(esRightFoot,		"RightFoot");
		InitHandle(esLeftRearFoot,	"LeftRearFoot");
		InitHandle(esRightRearFoot,	"RightRearFoot");
		InitHandle(esHand,			"Hand");
		InitHandle(esHead,			"Head");
		InitHandle(esNeck,			"Neck");
		InitHandle(esTail,			"Tail");
		InitHandle(esReverseKnees,	"ReverseKnees");

		// SubMaterial names.
		int i2;
		for (i2 = 0; i2 < iMAX_SUBMATERIALS; ++i2)
		{
			char str_symbol[32];

			sprintf(str_symbol, "SubMaterial%0.2d", i2+1);

			InitHandle(ESymbol(esSubMaterial01+i2), str_symbol);
		}
		InitHandle(esSubMaterialEnd, "SubMaterialEnd");

		// Animation names.
		for (i2 = 0; i2 < iMAX_ANIMATION_FRAMES; ++i2)
		{
			char str_symbol[32];
			sprintf(str_symbol, "Anim%0.2d", i2);
			InitHandle(ESymbol(esAnim00+i2),		str_symbol);

			sprintf(str_symbol, "AnimB%0.2d", i2);
			InitHandle(ESymbol(esAnimB00+i2),		str_symbol);
		}
		InitHandle(esPLACEHOLDER,		"PLACEHOLDER");
		InitHandle(esFrame,				"Frame");
		InitHandle(esFreezeFrame,		"FreezeFrame");
		InitHandle(esTrackTwo,			"TrackTwo");
		InitHandle(esDeltaX,			"DeltaX");
		InitHandle(esDeltaY,			"DeltaY");
		InitHandle(esAnimSubMaterial,	"AnimSubMaterial");

		// Vertex names.
		for (i2 = 0; i2 < iMAX_VERTICES; ++i2)
		{
			char str_symbol[32];
			sprintf(str_symbol, "Vertex%0.3d", i2);
			InitHandle(ESymbol(esVertex000+i2),		str_symbol);
		}
		InitHandle(esVERTEX_PLACEHOLDER, "VERTEX_PLACEHOLDER");

		// Model names.
		for (i2 = 0; i2 < iMAX_MODELS; ++i2)
		{
			char str_symbol[32];
			sprintf(str_symbol, "Model%0.2d", i2);
			InitHandle(ESymbol(esModel00+i2),		str_symbol);
		}
		InitHandle(esMODEL_PLACEHOLDER, "MODEL_PLACEHOLDER");

		// Array names.
		for (i2 = 0; i2 < iMAX_A; ++i2)
		{
			char str_symbol[32];
			sprintf(str_symbol, "A%0.2d", i2);
			InitHandle(ESymbol(esA00+i2),		str_symbol);
		}
		InitHandle(esAPLACEHOLDER, "A_PLACEHOLDER");

		InitHandle(esCTerrainObj, "CTerrainObj");
		InitHandle(esHeight, "Height");

		// Alpha symbols
		InitHandle(esAlpha, "Alpha");
		InitHandle(esR, "R");
		InitHandle(esG, "G");
		InitHandle(esB, "B");
		InitHandle(esAlphaChannel, "AlphaChannel");

		// MipMap symbols
		InitHandle(esMipMap, "MipMap");

		// Sky Symbols
		InitHandle(esSkyFlatColour, "SkyFlatColour");

		// Light symbols
		InitHandle(esIntensity, "Intensity");

		// Gun symbols
		InitHandle(esAmmo, "Ammo");
		InitHandle(esMaxAmmo, "MaxAmmo");
		InitHandle(esROF, "ROF");
		InitHandle(esAutoFire, "AutoFire");
		InitHandle(esAltAmmoCount, "AltAmmoCount");
		InitHandle(esAmmoPickup,  "AmmoPickup");
		InitHandle(esAmmoAlmostEmpty, "AmmoAlmostEmpty");
		InitHandle(esAmmoHalfFull, "AmmoHalfFull");
		InitHandle(esAmmoFull, "AmmoFull");
		InitHandle(esAmmoReallyFull, "AmmoReallyFull");

		InitHandle(esDamage, "Damage");
		InitHandle(esTranqDamage, "TranqDamage");
		InitHandle(esEmptyClipSample, "EmptyClipSample");
		InitHandle(esRingSample, "RingSample");
		InitHandle(esRange, "Range");
		InitHandle(esKick, "Kick");
		InitHandle(esPush, "Push");
		InitHandle(esBarrel, "Barrel");
		InitHandle(esRecoilOrg, "RecoilOrg");
		InitHandle(esRecoilForce, "RecoilForce");
		InitHandle(esMFlashOrg, "MFlashOrg");
		InitHandle(esMFlashDuration, "MFlashDuration");
		InitHandle(esRandomRotate, "RandomRotate");

		for (int imf = 0; imf < iMAX_MUZZLE_FLASH; ++imf)
		{
			char str_symbol[32];
			sprintf(str_symbol, "MFlashObject%d", imf);
			InitHandle(ESymbol(esMFlashObject0+imf), str_symbol);
		}
		InitHandle(esMUZZLE_PLACEHOLDER, "MUZZLE_PLACEHOLDER");


		char* str_activity_table[iMAX_ACTIVITIES];
		int i_act;
		for (i_act = iMAX_ACTIVITIES - 1; i_act >= 0; --i_act)
		{
			str_activity_table[i_act] = "BOGUS";
		}

		str_activity_table[eatEAT] =  "ActEat";
		str_activity_table[eatBITE] = 	"ActBite";
//		str_activity_table[eatCLAW] = 	"ActClaw";
		str_activity_table[eatJUMP_N_BITE] = 	"ActJumpBite";
//		str_activity_table[eatJUMP_N_CLAW] = 	"ActJumpClaw";
		str_activity_table[eatFEINT] = 	"ActFeint";
		str_activity_table[eatHEAD_BUTT] = 	"ActRam";
		str_activity_table[eatDRINK] = 	"ActDrink";
		str_activity_table[eatSHOULDER_CHARGE] = 	"ActShoulderCharge";
		str_activity_table[eatTAIL_SWIPE] = 	"ActTailSwipe";
//		str_activity_table[eatGRAB] = 	"ActGrab";
//		str_activity_table[eatTHROW] = 	"ActThrow";
//		str_activity_table[eatSHAKE] = 	"ActShake";

		str_activity_table[eatOUCH] = 	"ActOuch";
		str_activity_table[eatHELP] = 	"ActHelp";
		str_activity_table[eatHOWL] = 	"ActHowl";
		str_activity_table[eatSNARL] = 	"ActSnarl";
		str_activity_table[eatCROON] = 	"ActCroon";
		str_activity_table[eatDIE] = 	"ActDie";

		str_activity_table[eatLOOK_AT] = 	"ActLookAt";
		str_activity_table[eatTASTE] = 	"ActTaste";
		str_activity_table[eatSNIFF_TARGET] = 	"ActSniffTarget";
		str_activity_table[eatCOCK_HEAD] = 	"ActCockHead";
//		str_activity_table[eatLIFT_TAIL] = 	"ActLiftTail";

		str_activity_table[eatMOVE_TOWARD] = 	"ActMoveToward";
		str_activity_table[eatJUMP] = 	"ActJump";
//		str_activity_table[eatMOVE_HERD] = 	"ActMoveHerd";
		str_activity_table[eatMOVE_AWAY] = 	"ActMoveAway";
		str_activity_table[eatDONT_TOUCH] = 	"ActDontTouch";
		str_activity_table[eatWANDER] = 	"ActWander";
		str_activity_table[eatSTALK] = 	"ActStalk";
		str_activity_table[eatCIRCLE] = 	"ActCircle";
		str_activity_table[eatPURSUE] = 	"ActPursue";
		str_activity_table[eatFLEE] = 	"ActFlee";
		str_activity_table[eatSTAY_NEAR] = 	"ActStayNear";
		str_activity_table[eatSTAY_AWAY] = 	"ActStayAway";
		str_activity_table[eatJUMP_BACK] = 	"ActJumpBack";
		str_activity_table[eatGET_OUT] = 	"ActGetOut";
		str_activity_table[eatDASH] = 	"ActDash";
		str_activity_table[eatAPPROACH] = 	"ActApproach";
		str_activity_table[eatMOVE_BY]	= 	"ActMoveBy";

		str_activity_table[eatREAR_BACK] = 	"ActRearBack";
		str_activity_table[eatBACK_AWAY] = 	"ActBackAway";
		str_activity_table[eatCOWER] = 		"ActCower";

//		str_activity_table[eatLASH_TAIL] = 	"ActLashTail";
		str_activity_table[eatGLARE] = 		"ActGlare";

//		str_activity_table[eatSLEEP] = 	"ActSleep";
//		str_activity_table[eatSCRATCH] = 	"ActScratch";
//		str_activity_table[eatLIE_DOWN] = 	"ActLieDown";
		str_activity_table[eatSNIFF]		= 	"ActSniff";
		str_activity_table[eatLOOK_AROUND]	= 	"ActLookAround";
		str_activity_table[eatNOTHING]		= 	"ActNothing";


		str_activity_table[eatTEST_HEAD_COCK] = 	"ActTestHeadCock";
		str_activity_table[eatTEST_HEAD_ORIENT] = 	"ActTestHeadOrient";
		str_activity_table[eatTEST_WAG_TAIL] = 	"ActTestWagTail";
		str_activity_table[eatTEST_HEAD_POSITION] = 	"ActTestHeadPosition";
		str_activity_table[eatTEST_MOUTH] = 	"ActTestMouth";


		AlwaysAssert(eatEND < iMAX_ACTIVITIES);
		for (i_act = iMAX_ACTIVITIES - 1; i_act >= 0; --i_act)
		{
			InitHandle(ESymbol(esActBASE + i_act), str_activity_table[i_act]);
		}

		// fill the action array
		for (i_act = iMAX_ACTIONS - 1; i_act >= 0; --i_act)
		{
			char str[128];
			wsprintf(str,"Action%0.2d",i_act);
			InitHandle(ESymbol(esTAction00 + i_act), str);
		}


		// Fill the rating feeling symbols for each activity.

		for (i_act = iMAX_ACTIVITIES - 1; i_act >= 0; --i_act)
		{
			char buffer[128];
			sprintf(buffer, "%s%s", str_activity_table[i_act], "Fear");
			InitHandle(ESymbol(esActRATINGS_BASE + i_act * eptEND + eptFEAR), buffer);

			sprintf(buffer, "%s%s", str_activity_table[i_act], "Love");
			InitHandle(ESymbol(esActRATINGS_BASE + i_act * eptEND + eptLOVE), buffer);

			sprintf(buffer, "%s%s", str_activity_table[i_act], "Anger");		
			InitHandle(ESymbol(esActRATINGS_BASE + i_act * eptEND + eptANGER), buffer);

			sprintf(buffer, "%s%s", str_activity_table[i_act], "Curiosity");
			InitHandle(ESymbol(esActRATINGS_BASE + i_act * eptEND + eptCURIOSITY), buffer);

			sprintf(buffer, "%s%s", str_activity_table[i_act], "Hunger");
			InitHandle(ESymbol(esActRATINGS_BASE + i_act * eptEND + eptHUNGER), buffer);

			sprintf(buffer, "%s%s", str_activity_table[i_act], "Thirst");
			InitHandle(ESymbol(esActRATINGS_BASE + i_act * eptEND + eptTHIRST), buffer);

			sprintf(buffer, "%s%s", str_activity_table[i_act], "Fatigue");
			InitHandle(ESymbol(esActRATINGS_BASE + i_act * eptEND + eptFATIGUE), buffer);

			sprintf(buffer, "%s%s", str_activity_table[i_act], "Pain");
			InitHandle(ESymbol(esActRATINGS_BASE + i_act * eptEND + eptPAIN), buffer);

			sprintf(buffer, "%s%s", str_activity_table[i_act], "Solidity");
			InitHandle(ESymbol(esActRATINGS_BASE + i_act * eptEND + eptSOLIDITY), buffer);
		}



		InitHandle(esArchetype,			"Archetype");
		InitHandle(esAIType,			"AIType");
		InitHandle(esAIMass,			"AIMass");
		InitHandle(esDanger,			"Danger");


		InitHandle(ESymbol(esParameter00 + eptFEAR), "Fear");
		InitHandle(ESymbol(esParameter00 + eptLOVE), "Love");
		InitHandle(ESymbol(esParameter00 + eptANGER), "Anger");		
		InitHandle(ESymbol(esParameter00 + eptCURIOSITY), "Curiosity");
		InitHandle(ESymbol(esParameter00 + eptHUNGER), "Hunger");
		InitHandle(ESymbol(esParameter00 + eptTHIRST), "Thirst");
		InitHandle(ESymbol(esParameter00 + eptFATIGUE), "Fatigue");
		InitHandle(ESymbol(esParameter00 + eptPAIN), "Pain");
		InitHandle(ESymbol(esParameter00 + eptSOLIDITY), "Solidity");

		InitHandle(ESymbol(esHumanParameter00 + eptFEAR), "HumanFear");
		InitHandle(ESymbol(esHumanParameter00 + eptLOVE), "HumanLove");
		InitHandle(ESymbol(esHumanParameter00 + eptANGER), "HumanAnger");		
		InitHandle(ESymbol(esHumanParameter00 + eptCURIOSITY), "HumanCuriosity");
		InitHandle(ESymbol(esHumanParameter00 + eptHUNGER), "HumanHunger");
		InitHandle(ESymbol(esHumanParameter00 + eptTHIRST), "HumanThirst");
		InitHandle(ESymbol(esHumanParameter00 + eptFATIGUE), "HumanFatigue");
		InitHandle(ESymbol(esHumanParameter00 + eptPAIN), "HumanPain");
		InitHandle(ESymbol(esHumanParameter00 + eptSOLIDITY), "HumanSolidity");

		InitHandle(ESymbol(esDamageParameter00 + eptFEAR), "DamageFear");
		InitHandle(ESymbol(esDamageParameter00 + eptLOVE), "DamageLove");
		InitHandle(ESymbol(esDamageParameter00 + eptANGER), "DamageAnger");		
		InitHandle(ESymbol(esDamageParameter00 + eptCURIOSITY), "DamageCuriosity");
		InitHandle(ESymbol(esDamageParameter00 + eptHUNGER), "DamageHunger");
		InitHandle(ESymbol(esDamageParameter00 + eptTHIRST), "DamageThirst");
		InitHandle(ESymbol(esDamageParameter00 + eptFATIGUE), "DamageFatigue");
		InitHandle(ESymbol(esDamageParameter00 + eptPAIN), "DamagePain");
		InitHandle(ESymbol(esDamageParameter00 + eptSOLIDITY), "DamageSolidity");

		InitHandle(esBravery,			"Bravery");
		InitHandle(esTeam,				"Team");

		InitHandle(esSpeed,				"Speed");
		InitHandle(esMouth,				"Mouth");

		InitHandle(esAI,				"AI");
		
		InitHandle(esClimb,				"Climb");
		InitHandle(esDinosaur,			"Dinosaur");
		InitHandle(esPathfind,			"Pathfind");
		InitHandle(esVocal,				"Vocal");
 		InitHandle(esTerrainPathfinding,"TerrainPathfinding");
		InitHandle(esSenseTerrain,		"SenseTerrain");
		InitHandle(esSenseObjects,		"SenseObjects");
		InitHandle(esSenseAnimates,		"SenseAnimates");
		InitHandle(esWakeUp,			"WakeUp");
		InitHandle(esSleep,				"Sleep");
		InitHandle(esStayNearMax,		"StayNearMax");
		InitHandle(esStayNearOK,		"StayNearOK");
		InitHandle(esStayNearTarget,	"StayNearTarget");
		InitHandle(esStayAwayMin,		"StayAwayMin");
		InitHandle(esStayAwayOK,		"StayAwayOK");
		InitHandle(esStayAwayTarget,	"StayAwayTarget");
		InitHandle(esBiteTargetDistance,	"BiteTargetDistance");
		InitHandle(esFeintTargetDistance,	"FeintTargetDistance");




		InitHandle(esWidth,				"Width");
		InitHandle(esHeadReach,			"HeadReach");
		InitHandle(esTailReach,			"TailReach");
		InitHandle(esClawReach,			"ClawReach");
		InitHandle(esJumpDistance,		"JumpDistance");
		InitHandle(esJumpUp,			"JumpUp");
		InitHandle(esJumpDown,			"JumpDown");
		InitHandle(esWalkOver,			"WalkOver");
		InitHandle(esWalkUnder,			"WalkUnder");
		InitHandle(esJumpOver,			"JumpOver");
		InitHandle(esCrouchUnder,		"CrouchUnder");
		InitHandle(esMoveableMass,		"MoveableMass");
		InitHandle(esUpAngle,			"UpAngle");
		InitHandle(esDownAngle,			"DownAngle");

		InitHandle(esNodesPerSec,		"NodesPerSec");
		InitHandle(esMaxNodes,			"MaxNodes");
		InitHandle(esTimeToForgetNode,	"TimeToForgetNode");
		InitHandle(esTimeToForgetInfluence,			"TimeToForgetInfluence");
		InitHandle(esMaxPathLength,		"MaxPathLength");
		InitHandle(esMaxAStarSteps,		"MaxAStarSteps");


		InitHandle(esSolid,				"Solid");
		InitHandle(esDust,				"Dust");
		InitHandle(esLog,				"Log");
		InitHandle(esNumber,			"Number");
		InitHandle(esVelocity,			"Velocity");
		InitHandle(esLifespan,			"Lifespan");
		InitHandle(esSize,				"Size");
		InitHandle(esLength,			"Length");
		InitHandle(esGravity,			"Gravity");
		InitHandle(esStuckGravity,		"StuckGravity");
		InitHandle(esSpread,			"Spread");
		InitHandle(esThreshold,			"Threshold");

		InitHandle(esTextPosition,		"TextPosition");
		InitHandle(esTextDisplayTime,	"TextDisplayTime");
		InitHandle(esTextAtTop,			"TextAtTop");
		InitHandle(esOverlayText,		"OverlayText");
		InitHandle(esResourceID,		"ResourceID");

#if VER_TEST
		// Make sure all symbols are defined.
		for (i = esEND - 1; i>= 0; i--)
		{
			ESymbol es = ESymbol(i);
			AlwaysAssert(abSymbolInit[i]);
		}
#endif

	}

	//*****************************************************************************************
	void CLoadWorld::InitHandle(ESymbol es, char *str)
	{
		Assert(es >= 0);
		Assert(es < esEND);
#if VER_TEST
		// Make sure the symbol is not multiply defined.
		AlwaysAssert(abSymbolInit[es] == false);
#endif

		ahSymbols[es] = vtableTable.hSymbol(str);

#if VER_TEST
		abSymbolInit[es] = true;
#endif

	}
