/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Implementation of ImageLoader.hpp
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/ImageLoader.cpp                                            $
 * 
 * 28    10/05/98 6:17a Agrant
 * better? calculation of swap file space management
 * 
 * 27    10/02/98 9:49p Rwyatt
 * Chnaged swpspace registry key to SwapSpaceMb
 * 
 * 26    9/25/98 7:21p Rwyatt
 * Image loader no longer sets the VM to use blocking loads
 * 
 * 25    9/16/98 9:18p Agrant
 * Image loader succeeds unless overridden elsewhere.
 * 
 * 24    98.09.14 12:23p Mmouni
 * Out of date for bad local swap files are now deleted before the image loader tries to do its
 * thing.
 * 
 * 23    9/04/98 4:40p Mmouni
 * Added preliminary assembly version of de-compression (currently disabled).
 * Added support for handling write errors.
 * 
 * 22    9/03/98 4:30p Shernd
 * Checking for out of disk space
 * 
 * 21    98.09.02 10:02p Mmouni
 * Added progress counters to new de-compression.
 * 
 * 20    98.09.01 8:58p Mmouni
 * Added switch to enable/disable loading of new swap file compressed format.
 * 
 * 19    98.09.01 8:43p Mmouni
 * Added the first version (optimized C) of the new image file decompression.
 * 
 * 18    8/31/98 6:46p Mmouni
 * Added optional warning message for no swap file.
 * 
 * 17    8/27/98 1:33p Shernd
 * Improved callback updates
 * 
 * 16    8/25/98 2:13p Rwyatt
 * Made the load heap 16Mb maximum
 * 
 * 15    8/23/98 4:30p Rwyatt
 * Added an assert for zero length PID files
 * 
 * 14    8/23/98 2:45a Agrant
 * Don't wait for the swap thread if we already have a swap file.
 * 
 * 13    8/23/98 12:24a Shernd
 * While waiting for the local Copy to finish we now use an Even instead of a CritialSection.
 * 
 * 12    8/19/98 1:38p Rwyatt
 * New allocation functions to load from the Image Loader fast heap
 * 
 * 11    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 10    8/03/98 6:34p Shernd
 * Changes for Progress Loading Bar
 * 
 * 9     6/24/98 3:22p Rwyatt
 * Use synchronous VM for the first few frames so image cahces and terrain textures are at full
 * resolution.
 * 
 * 8     6/12/98 2:33p Rwyatt
 * If Trespasser is not installed the image loader will default to a non compressed swap file if
 * it can find one. It will assert if a SWP file cannot be found.
 * 
 * 7     6/10/98 4:52p Rwyatt
 * Now loads compressed swap files (.spp). If one of these files cannot be found then the
 * existing swap file is loaded in the usual way,
 * 
 * 6     6/02/98 11:17p Rwyatt
 * Fixed the problem of finding and deleting old swap files if they are read only
 * 
 * 5     4/23/98 7:58p Rwyatt
 * Copy local of swap files now deletes old swap file once the specified quota is reached. The
 * quota is obtained from the registry.
 * Commit all is now idsabled by default.
 * 
 * 4     4/22/98 2:54p Rwyatt
 * The VM thread is only resumed if we do not have the commit all flag set
 * 
 * 3     4/21/98 3:10p Rwyatt
 * New format swap files (110).
 * These support a pageable and non-pageable section.
 * 
 * 2     2/25/98 3:16p Rwyatt
 * Added an extentsion to the swp and pid file so  multiples can exist for different versions of
 * the GUIApp
 * 
 * 1     2/18/98 1:08p Rwyatt
 * Moved from TextureManager.cpp
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "stdio.h"
#include "stdlib.h"
#include "io.h"
#include "Lib/W95/WinInclude.hpp"
#include "ImageLoader.hpp"
#include "Lib/Sys/VirtualMem.hpp"
#include "TextureManager.hpp"
#include "Loader.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/FileEx.hpp"
#include "Lib/sys/Reg.h"
#include "Lib/EntityDBase/RenderDB.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/Sys/Profile.hpp"


//*********************************************************************************************
#define THREADED_COPY (1)

#define NEW_SWAP_COMPRESSION (1)

#if !defined(WARN_ON_NO_SWAP)

#define WARN_ON_NO_SWAP (0)

#endif


//*********************************************************************************************
// Load Image Directory Static Variables
//
CLoadImageDirectory*		CLoadImageDirectory::plidImageDir		= NULL;
bool						CLoadImageDirectory::bValidImage		= false;
bool						CLoadImageDirectory::bImageLoader		= true;
bool						CLoadImageDirectory::bAutoCommit		= false;
HANDLE                      CLoadImageDirectory::hCopyLocal			= NULL;
bool						CLoadImageDirectory::bCopyResult		= false;
bool						CLoadImageDirectory::bCompressed		= false;
bool						CLoadImageDirectory::bNewCompression	= false;
void*						CLoadImageDirectory::pvThreadHandle		= NULL;
CFastHeap					CLoadImageDirectory::fhImageLoad(1 << 24);		// 16Mb


//*********************************************************************************************
//
// A Function that reads the size from one of our newly compressed files.
//
uint32 u4OriginalFileSize(const char *str_filename);
//
//*************************************


//*********************************************************************************************
// Image Directory Loading
//
// This will create all of the structures for relocating rasters and will start the VM system
// if all goes well.
//
CLoadImageDirectory::CLoadImageDirectory
(
	const char*		str_grf_name,			// name of the GRF file being loaded
    PFNWORLDLOADNOTIFY pfnInWorldNotify,    // Notification callback
    uint32 u4InNotifyParam                  // Notificaiton parameter
)
//*************************************
{
	char	str_dir_name[MAX_PATH];
	HANDLE	h_load;
	uint32	u4_len;
	uint32	u4_load_bytes;

	// Assume success unless set elsewhere.
	i4Error = 0;

	Assert(plidImageDir==NULL);		// there can be only one.

    hCopyLocal = CreateEvent(NULL, TRUE, FALSE, NULL);

    pfnWorldNotify = pfnInWorldNotify;
    u4NotifyParam = u4InNotifyParam;

	strcpy(str_dir_name,str_grf_name);
	strcpy(str_dir_name + strlen(str_dir_name) - 4, strPID_FILE_EXTENTSION);

	plidImageDir = this;			// set the global class pointer to this

	bValidImage		= false;
	pdfhDirHeader	= NULL;

	//
	// if the image loader is disbaled then return without doing anything,
	// if the virtual memory manager is already image based we must use the conventional loader,
	// if the virtual memory manager has allocated any memory we must use the conventional loader,
	// if exit path is taken and something is allocated then we cannot create a swap file later, this
	// happens when two GRF files are loaded. Also, if this exit is taken then make sure that there
	// is no small texture manager. Small textures should be forced to use the normal allocator.
	//
	if ((!bImageLoader) || (gtxmTexMan.pvmeTextures->bFileBased()) || 
									(gtxmTexMan.pvmeTextures->bAnythingAllocated()))
	{
		gtxmTexMan.DestroySmallTextureAllocator();
		bImageLoader = false;
		SetEvent(hCopyLocal);
		return;
	}

	// There must not be a sub allocator at this point or something is really wrong.
	Assert(gtxmTexMan.pvmsaSmallTextures == NULL);

	// process the local swap file.
	if ( bProcessLocalSwapFile
		 (
			str_grf_name			// name of the GRF file to find a swap file for
		 ) == false )
	{
		// We need to create the small texture allocator before we continue with the conventional
		// load.
		gtxmTexMan.CreateSmallTextureAllocator();

		// cannot process the swap file, probably because it does not exist.
		bImageLoader = false;
		return;
	}


	//
	// open the directory file....
	//
	h_load=CreateFile(str_dir_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_READONLY|FILE_FLAG_SEQUENTIAL_SCAN,NULL);

	if (h_load == INVALID_HANDLE_VALUE)
	{
		return;
	}
		
	// size of the file
	u4_len = GetFileSize(h_load, NULL);

	Assert(u4_len > 0);

	pdfhDirHeader = (SDirectoryFileHeader*) new char[u4_len];

	if (ReadFile(h_load, pdfhDirHeader , u4_len ,(DWORD*)&u4_load_bytes, NULL) == false)
	{
		CloseHandle(h_load);
		delete pdfhDirHeader;
		pdfhDirHeader = NULL;
		return;
	}

	// make sure we got the whole file, if not fail on the image file so it is not used.
	if (u4_load_bytes != u4_len)
	{
		CloseHandle(h_load);
		delete pdfhDirHeader;
		pdfhDirHeader = NULL;
		return;
	}

	CloseHandle(h_load);

	// We must have the correct version number
	AlwaysAssert(pdfhDirHeader->u4Version == u4DIRECTORY_FILE_VERSION);

	//
	// check if the bump maps in this file match the bit depth of the current build...
	if (pdfhDirHeader->u4BumpMapBitDepth != iBUMPMAP_RESOLUTION)
	{
		delete pdfhDirHeader;
		pdfhDirHeader = NULL;
		return;
	}

	//
	// Construct an array of CPals
	// 
	uint32					u4_pal_count = pdfhDirHeader->u4PaletteCount;
	SDirectoryPaletteChunk*	pdpc = (SDirectoryPaletteChunk*)( ((uint8*)pdfhDirHeader) +
									pdfhDirHeader->u4PaletteOffset);
	uint32					u4_pal = 0;

	// get the actual palette data from the directory file
	while (u4_pal_count)
	{
		// create a new palette of the correct size and set its hash value to the stored value

		CPal*	ppal = new CPal(pdpc->u4ColoursUsed);
		ppal->SetHashValue(pdpc->u4HashValue);

		// push the palettes into an STL vector.
		CLoadImageDirectory::plidImageDir->appal.push_back(ppal);
		
		// copy the data across
		memcpy(&ppal->aclrPalette[0], &pdpc->clr[0], pdpc->u4ColoursUsed * sizeof(CColour) );

		pdpc = (SDirectoryPaletteChunk*) ((char*)pdpc + pdpc->u4Size);
		u4_pal_count--;
		u4_pal++;
	}


	//
	// add the raster elements to the map for easy locating
	//
	uint32					u4_chunks = pdfhDirHeader->u4RasterChunkCount;
	SDirectoryFileChunk*	pdfc = (SDirectoryFileChunk*)( ((uint8*)pdfhDirHeader) +
									pdfhDirHeader->u4RasterChunkOffset);

	// pdfc points to the first of the nodes...
	while (u4_chunks)
	{
		// check that this ID is not already in directory map.
		Assert(mapChunk[pdfc->u8HashValue] == NULL);

		mapChunk[pdfc->u8HashValue] = pdfc;
		pdfc = (SDirectoryFileChunk*) ((char*)pdfc + pdfc->u4Size);
		u4_chunks--;
	}

	// Reserve enough virtual memory for the image file
	gtxmTexMan.pvmeTextures->SetPagingRegion(u4LocalImageSize);

	bValidImage = true;
}


//*********************************************************************************************
//
CLoadImageDirectory::~CLoadImageDirectory
(
)
//*************************************
{
	dprintf("Waiting for local copy..\n");

    // Spin and wait for mutext to finish
    DWORD   dw;

    do
    {
        dw = WaitForSingleObject(hCopyLocal, 250);
        if (dw == WAIT_TIMEOUT && pfnWorldNotify)
        {
            (pfnWorldNotify)(u4NotifyParam, 0, 0, 0);
        }
    }
    while (dw != WAIT_OBJECT_0);

    CloseHandle(hCopyLocal);

	CloseHandle((HANDLE)pvThreadHandle);

	if (bValidImage)
	{
		if (!bCopyResult)
		{
			// what should we do because the copy has failed....
			AlwaysAssert(0);
		}

		// Start the VM system...
		if (!gtxmTexMan.pvmeTextures->bBeginPaging(strLocalImage,
			pdfhDirHeader->u4PageableOffset,pdfhDirHeader->u4NonPageableCount,false))
		{
			// failed to start the VM system.
			Assert(0);
		}

		// If the auto commit flag is set then load the whole swap file, otherwise
		// start the loader thread so we can begin loading the required data.
		if (bAutoCommit)
		{
			gtxmTexMan.pvmeTextures->CommitAll();
		}
		else
		{
			// Start the thread
			gtxmTexMan.pvmeTextures->ResumeVMLoadThread();
		}
	}
	// go through all the palettes in the STL vector and delete them. This is safe because no
	// raster should reference one of these palettes. All rasters should have created their
	// own palettes.

	for (std::vector<CPal*>::iterator i = appal.begin(); i<appal.end(); ++i)
	{
		delete (*i);
	}

	// delete the memory for the image file..
	delete pdfhDirHeader;
	pdfhDirHeader = NULL;

	plidImageDir = NULL;			// zero out the static class pointer
	bValidImage = false;			// image is not longer valid
}




//*********************************************************************************************
// process the local swap file, if there is not a swap file locally copy it
// across.
// This will return true if all is OK. str_local_swap_image is the name returned by this
// function as the name of the swap file to load. If we are loading the non-local swap file
// copy its filename into this buffer.
//
bool CLoadImageDirectory::bProcessLocalSwapFile
(
	const char*		str_grf_name			// name of the GRF swap file
)
//*************************************
{
	char	str_local_swap_dir[MAX_PATH];

	bCopyResult = false;
	bCompressed = true;
	bNewCompression = true;

    i4Error = 0;

	GetRegString("Installed Directory", str_local_swap_dir, MAX_PATH, "");

	// Check the last character of the install path.
	char ch_last = *(str_local_swap_dir+strlen(str_local_swap_dir)-1);
	if ((ch_last == '\\') || (ch_last == '/'))
	{
		*(str_local_swap_dir+strlen(str_local_swap_dir)-1) = 0;
	}

	// Make the name of the compressed file
	strcpy(strSourceImage, str_grf_name);
	strcpy(strSourceImage + strlen(strSourceImage) - 4, strCOMP_FILE_EXTENTSION);

	//
	// check if the GRF file has a swap file with it, if it does not then
	// do not bother looking any futher
	//
	if (!NEW_SWAP_COMPRESSION || !bFileExists(strSourceImage))
	{
		dprintf("No SPZ file (compressed swap file) with GRF file. (%s)\n", strSourceImage);
		bNewCompression = false;

		// Modify the pathname to look for a old style compressed swap file.
		strcpy(strSourceImage + strlen(strSourceImage) - strlen(strCOMP_FILE_EXTENTSION), strCOMP_FILE_EXTENTSION_OLD);
		if (!bFileExists(strSourceImage))
		{
			dprintf("No SPP file (old compressed swap file) with GRF file. (%s)\n", strSourceImage);
			bCompressed = false;

			// Modify the pathname to look for a uncompressed swap file.
			strcpy(strSourceImage + strlen(strSourceImage) - strlen(strCOMP_FILE_EXTENTSION), strSWP_FILE_EXTENTSION);
			if (!bFileExists(strSourceImage))
			{
				dprintf("No SWP file (uncompressed swap file) with GRF file. (%s)\n", strSourceImage);

				#if (WARN_ON_NO_SWAP)
					Warning("No swap file");
				#endif

				SetEvent(hCopyLocal);
				return false;
			}
		}
	}

	// get the last written time of the swp file with the GRF file and its size
	u8SourceImageTime = u8FileTimeLastWritten(strSourceImage);
	u4SourceSize = u4FileSize(strSourceImage);

	if (bNewCompression)
		u4LocalImageSize = u4OriginalFileSize(strSourceImage);
	else
		u4LocalImageSize = u4FileSize(strSourceImage, bCompressed);

	Assert(u8SourceImageTime>0);
	Assert(u8SourceImageTime!=0xffffffff);

	//
	// The GRF file has a swap file so check if we have a local version of
	// the same swp file.
	//
	// This is the name of the local swap file if one exists..
	//
	strcpy(strLocalImage, str_local_swap_dir);
	strcat(strLocalImage, "\\");
	strcat(strLocalImage, strLeafName(strSourceImage) );
	// Make sure the local image ends in SWP
	strcpy(strLocalImage + strlen(strLocalImage) - strlen(strSWP_FILE_EXTENTSION), strSWP_FILE_EXTENTSION);

	if (bFileExists(strLocalImage))
	{
		// There is a local version of this swap file, does it have the same last accessed date
		// and the same size as the version with the GRF file??
		uint64 u8_local_time = u8FileTimeLastWritten(strLocalImage);

		if ((u8_local_time == u8SourceImageTime) && (u4LocalImageSize == u4FileSize(strLocalImage)))
		{
			// We have a match for the swap file so copy the name of the file to load.
			dprintf("Using local swap file..\n");

            if (pfnWorldNotify)
            {
                (pfnWorldNotify)(u4NotifyParam, 2, 99, 0);
            }

            SetEvent(hCopyLocal);

			bCopyResult = true;
			return true;
		}
		else
		{
			// Local swap file is out of date or bad, delete it.
			SetFileAttributes(strLocalImage, FILE_ATTRIBUTE_NORMAL);
			DeleteFile(strLocalImage);
		}
	}

	//
	// We cannot create a temp file called the same as local swap file so we must assume
	// that the copy will fail, or the local directory path is NULL.
	// In this case we can use the swap file that is with the GRF file..
	//
	if ((*str_local_swap_dir == 0) || !bCanCreateFile(strLocalImage))
	{
		dprintf("Cannot create local swap file - Is Trespasser installed??\n");
		if (bCompressed)
		{
			// We have found a compressed swap file and this cannot be used so we need to look for
			// a uncompressed version or fail the load and force it to use the conventional loader.
			strcpy(strSourceImage + strlen(strSourceImage) - strlen(strCOMP_FILE_EXTENTSION), strSWP_FILE_EXTENTSION);
			if (!bFileExists(strSourceImage))
			{
				//
				// There is only a compressed swap file and this machine does not have Trespasser installed so
				// no registry key can be found, hence we have no idea where to decompress the file to. If the
				// uncompressed swap file had been present it would have been used in place without being copied
				// locally. Compressed swap files must be copied locally in order to decompress them and therefore
				// Trespasser must be installed.
				//
				dprintf("No uncompressed swap file with GRF file. Using conventional loader\n", strSourceImage);
				AlwaysAssert(0);
                SetEvent(hCopyLocal);
				return false;
			}
		}
		dprintf("Using GRF swap file - Cannot create local.\n");
		strcpy(strLocalImage,strSourceImage);
		bCopyResult = true;
        SetEvent(hCopyLocal);
		return true;
	}

	//
	// If we manage to get to here we need to create a local swap file....
	//


	// Before we copy local ensure that we have not reached our quota for used disk space
	char					str_wildcard[MAX_PATH];
	_finddata_t				fnd;
	std::vector<SDiskSwapFile>	vdsf;

	strcpy(str_wildcard,str_local_swap_dir);
	strcat(str_wildcard,"\\*.swp");

	int32 i4_ffhandle = _findfirst(str_wildcard, &fnd);

	//
	// If FindFirst reports an error there is probably no files that match so copy the
	// swap file without any other regards.
	//
	if (i4_ffhandle!=-1)
	{
		uint32 u4_count = 0;
		uint32 u4_totalsize = 0;
		int32 i4_res = 0;

		while (i4_res==0)
		{
			SDiskSwapFile	dsf;
			
			strcpy(dsf.strFilename,str_local_swap_dir);
			strcat(dsf.strFilename,"\\");
			strcat(dsf.strFilename,fnd.name);

			// FAT system do not support a last access time stamp, if this time is -1 then use
			// the write time stamp. This effectively changes the caching system from an LRU to
			// FIFO.
			if (fnd.time_access == -1)
			{
				dsf.u4Time = fnd.time_write;
			}
			else
			{
				dsf.u4Time = fnd.time_access;
			}

			dsf.u4Size = fnd.size;
			u4_totalsize += fnd.size;

			dsf.bDeleted = false;

			vdsf.push_back(dsf);
			u4_count++;

			// find the next matching file
			i4_res = _findnext(i4_ffhandle, &fnd);
		}
		_findclose(i4_ffhandle);

		// If the SwpSpace registry key is not present then assume the max size is 0,
		// This is effect means that there will only ever be a single swap file locally.
		uint32 u4_swap_disk_space = (uint32)GetRegValue("SwapSpaceMb", 0) * 1024 * 1024;
		int32 i4_space_free = int(u4_swap_disk_space) - int(u4_totalsize);

		// Have a guess at the decompressed size
		int32 i4_space_required = int(u4LocalImageSize) - i4_space_free;

		if (i4_space_required>0)
		{
			uint32 u4_files = u4_count;
			uint32 u4_oldest = 0xffffffff;
			uint32 u4_oldtime = 0xffffffff;

			// we need to clean out some old swap files to make our new one fit.
			dprintf("Cleaning out old swap files.\n");

			// If we have any swap files left in the directory and we still need to find more room
			// go and delete another file.
			while ((u4_files>0) && (i4_space_required>0))
			{
				for (int i=0; i<u4_count; i++)
				{
					if ( vdsf[i].bDeleted == false)
					{
						if ( vdsf[i].u4Time<u4_oldtime)
						{
							u4_oldest = i;
							u4_oldtime = vdsf[i].u4Time;
						}
					}
				}

				Assert(u4_oldest!=0xffffffff);

				// delete the element u4_oldest in the vector
				Assert(vdsf[u4_oldest].bDeleted == false);
				vdsf[u4_oldest].bDeleted = true;

				// Switch off any flags, such as read only, so it can be deleted.
				char* str_delete = vdsf[u4_oldest].strFilename;
				SetFileAttributes(str_delete,FILE_ATTRIBUTE_NORMAL);
				if (DeleteFile(str_delete))
				{
					// if we successfully deleted the file, subtract its size from the amount required.
					i4_space_required-=vdsf[u4_oldest].u4Size;
				}

				u4_oldest = 0xffffffff;
				u4_oldtime = 0xffffffff;

				u4_files--;
			}
		}
	}

    //
    // At this point we need to check to see if there is enough space 
    // on the local drive to copy the swap file.
    //
    {
        char    szDrive[50];
        uint32  u4SectorsPerCluster;
        uint32  u4BytesPerSector;
        uint32  u4FreeClusters;
        uint32  u4TotalClusters;

        _splitpath(strLocalImage, szDrive, NULL, NULL, NULL);
        strcat(szDrive, "\\");

        GetDiskFreeSpace(szDrive,
                         (unsigned long *)&u4SectorsPerCluster,
                         (unsigned long *)&u4BytesPerSector,
                         (unsigned long *)&u4FreeClusters,
                         (unsigned long *)&u4TotalClusters);
        // Check for enough free space for a save
        if (u4SectorsPerCluster * u4BytesPerSector * u4FreeClusters < u4LocalImageSize)
        {
            i4Error = -2;
			SetEvent(hCopyLocal);
            return false;
        }
    }


#if (THREADED_COPY)
	uint32	u4_thread_id;

	// Create the thread that will do the processing.....
	pvThreadHandle = (void*)CreateThread( 
				NULL,							// no security attributes 
				0,								// use default stack size  
				(LPTHREAD_START_ROUTINE) u4CopyLocalThread,
				NULL,							// argument to thread function
				0,								// create running
				(ULONG*)&u4_thread_id);			// returns the thread identifier

	dprintf("Thread Create: CopyLocalThread [Thread handle 0x%x]\n", u4_thread_id);
#else
	// if we are not threaded, just call the thread function..
	u4CopyLocalThread(0);
	pvThreadHandle = NULL;
#endif

	return true;
}


//*********************************************************************************************
//
// A Function that reads the size from one of our newly compressed files.
//
uint32 u4OriginalFileSize(const char *str_filename)
//
//*************************************
{
	HANDLE hFileSrc;
	DWORD dwRead;
	BOOL bRet;

	uint32 u4_file_size = 0;

	// Open the source file.
	hFileSrc = CreateFile(str_filename,
					   GENERIC_READ,
					   0,
					   NULL,
					   OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL);

	if (hFileSrc == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	// Read the un-compressed size from the file.
	bRet = ReadFile(hFileSrc, &u4_file_size, sizeof(u4_file_size), &dwRead, NULL);
	Assert(bRet && dwRead == sizeof(u4_file_size));

	// Close the file.
	CloseHandle(hFileSrc);

	return u4_file_size;
}


//*********************************************************************************************
// Function actually does the copy operation, either in a thread or by being called directly.
//
uint32 CLoadImageDirectory::u4CopyLocalThread
(
	uint32 u4_user
)
//*************************************
{
	// get the data passed in..
	CLoadImageDirectory* plid = CLoadImageDirectory::plidImageDir;

#if VER_TEST
	dprintf("Creating local swap file (%s)...\n", bCompressed ? "Compressed" : "Copy");
#endif

	CCycleTimer ctmr;

	if (bNewCompression)
	{
		//
		// Constants defining how the file was compressed.
		//
		const int N			= 4096;		// Size of ring buffer.
		const int F			= 18;		// Upper limit for match_length.
		const int THRESHOLD = 2;		// Encode string into position and length
										//	if match_length is greater than this.

		unsigned char text_buf[N];		// Ring buffer of size N.
		int r;							// Index into ring buffer.
		int i;
		unsigned int flags;				// Decompression flags.

		// File I/O stuff.
		HANDLE hFileSrc;					// Input file handle.
		HANDLE hFileDest;					// Output file handle.
		const int i_bufsize = 4096;			// Read buffer size.
		unsigned char src_buf[i_bufsize];	// Buffer for reading.
		unsigned char *src_base;			// Points to one past the end of the data.
		int i_srccnt = 0;					// Source offset.
		int i_dest_start;					// Where we started filling the ring buffer at.
		DWORD dwWritten, dwRead;
		BOOL bRet;
		uint32 u4_file_size;
		uint32 u4_bytes_written = 0;
		int iPercent;
		int iLastPercent;
		
		// Open the source file.
		hFileSrc = CreateFile((char*)plid->strSourceImage,
						   GENERIC_READ,
						   0,
						   NULL,
						   OPEN_EXISTING,
						   FILE_ATTRIBUTE_NORMAL + FILE_FLAG_SEQUENTIAL_SCAN,
						   NULL);

		if (hFileSrc == INVALID_HANDLE_VALUE)
		{
			goto ThreadError;
		}

		// Open the destination file.
		hFileDest = CreateFile((char*)plid->strLocalImage,
						   GENERIC_WRITE,
						   0,
						   NULL,
						   CREATE_ALWAYS,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL);

		if (hFileDest == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFileSrc);
			goto ThreadError;
		}

		// Initialize progress bar.
		if (plid->pfnWorldNotify)
		{
			(plid->pfnWorldNotify)(plid->u4NotifyParam, 3, 0, 100);
		}
		iLastPercent = 0;

		// Read the un-compressed size from the file.
		bRet = ReadFile(hFileSrc, &u4_file_size, sizeof(u4_file_size), &dwRead, NULL);
		Assert(bRet);

		// Clear the ring buffer.
		for (i = 0; i < N - F; i++) 
			text_buf[i] = '\0';
		
		r = N - F;
		i_dest_start = r;
		flags = 0;

#if (0 && VER_ASM && TARGET_PROCESSOR == PROCESSOR_PENTIUM)

		// This doesn't seem to be any faster than the non-asm version.
		__asm
		{
			mov		eax,[flags]			// eax = flags
			mov		esi,[i_srccnt]		// esi = i_srccnt
			mov		ecx,[r]				// ecx = r
			mov		ebx,[src_base]		// ebx = src_base
			lea		edi,[text_buf]		// edi = text_buf

		FOREVER_LOOP:
			shl		ah,1				// mask & count
			jnz		KEEP_DECODING

			// Check if we need to read.
			cmp		esi,0
			jne		NO_READ1

			// Read.
			call	READ_BUFFER

			test	esi,esi
			jnz		NO_READ1

			// EOF.
			mov		[r],ecx				// ecx = r
			jmp		DONE

		NO_READ1:
			// Load new flags.
			mov		al,[ebx+esi]		// src_base[i_srccnt]
			inc		esi					// i_srccnt++

			mov		ah,1				// use ah as bit mask and to count 8.
			
		KEEP_DECODING:
			test	al,ah				// test encoding bit.
			jz		ENCODED

			// Not encoded.

			// Check if we need to read.
			cmp		esi,0
			jne		NO_READ2

			// Read.
			call	READ_BUFFER

			test	esi,esi
			jnz		NO_READ2

			// EOF.
			mov		[r],ecx				// ecx = r
			jmp		DONE

		NO_READ2:
			// Copy character.
			mov		dl,[ebx+esi]
			inc		esi

			mov		[edi+ecx],dl
			inc		ecx

			// Check if we need to write.
			cmp		ecx,4096
			jne		FOREVER_LOOP

			// Write.
			call	WRITE_BUFFER

			jmp		FOREVER_LOOP

		ENCODED:
			// Check if we have 2 bytes.
			cmp		esi,-2
			jle		HAVE_TWO

			// Check if we need to read.
			cmp		esi,0
			jne		NO_READ3

			// Read.
			call	READ_BUFFER

			test	esi,esi
			jnz		NO_READ3

			// EOF.
			mov		[r],ecx				// ecx = r
			jmp		DONE

		NO_READ3:
			mov		dl,[ebx+esi]		// lower 8 bits of index
			inc		esi

			// Check if we need to read.
			cmp		esi,0
			jne		NO_READ4

			push	edx

			// Read.
			call	READ_BUFFER

			pop		edx

			test	esi,esi
			jnz		NO_READ4

			// EOF.
			mov		[r],ecx				// ecx = r
			jmp		DONE

		NO_READ4:
			mov		bl,[ebx+esi]		// upper 4 bits of index + count
			inc		esi

			jmp		DONE_WITH_READ

		HAVE_TWO:
			mov		dl,[ebx+esi]		// lower 8 bits of index

			mov		bl,[ebx+esi+1]		// upper 4 bits of index + count
			add		esi,2				// i_srccnt += 2

		DONE_WITH_READ:
			mov		dh,bl				// copy count
			and		bl,0x0f				// mask bits of count

			shr		dh,4				// shift bits of index down
			add		bl,3				// add THRESHOLD+1

			and		edx,4095			// and with N-1

		DECODE_LOOP:
			mov		bh,[edi+edx]		// read byte from ring buffer.
			inc		edx

			mov		[edi+ecx],bh		// write byte to ring buffer.
			inc		ecx

			// Check if we need to write.
			cmp		ecx,4096
			jne		NO_WRITE

			push	edx
			push	ebx

			// Write.
			call	WRITE_BUFFER

			pop		ebx
			pop		edx

		NO_WRITE:
			and		edx,4095				// and with N-1

			dec		bl
			jnz		DECODE_LOOP

			mov		ebx,[src_base]
			jmp		FOREVER_LOOP
		}

		// Write a block of data to the output file.
WRITE_BUFFER:
		__asm
		{
			mov		[flags],eax			// eax = flags
			mov		[i_srccnt],esi		// esi = i_srccnt
		}

		{
			// Write from ring buffer.
			bRet = WriteFile(hFileDest, text_buf+i_dest_start, N-i_dest_start, &dwWritten, NULL);
			if (!bRet || (iRead != dwWritten))
			{
				plid->i4Error = -2;
				CloseHandle(hFileSrc);
				CloseHandle(hFileDest);
				// TODO: return error code here.
			}

			// Show progress.
			u4_bytes_written += N-i_dest_start;
			iPercent = (u4_bytes_written * 100 / u4_file_size);
			if (iPercent > iLastPercent)
			{
				if (plid->pfnWorldNotify)
				{
					(plid->pfnWorldNotify)(plid->u4NotifyParam, 2, iPercent, 0);
				}

				iLastPercent = iPercent;
			}

			r = 0;
			i_dest_start = 0;

		}

		__asm
		{
			mov		eax,[flags]			// eax = flags
			mov		esi,[i_srccnt]		// esi = i_srccnt
			mov		ecx,[r]				// ecx = r
			mov		ebx,[src_base]		// ebx = src_base
			lea		edi,[text_buf]		// edi = text_buf

			ret
		}

		// Read a block of data from the input file.
READ_BUFFER:
		__asm
		{
			mov		[flags],eax			// eax = flags
			mov		[r],ecx				// ecx = r
		}

		{
			// Read buffer.
			bRet = ReadFile(hFileSrc, src_buf, i_bufsize, &dwRead, NULL);
			Assert(bRet);

			// EOF if (dwRead == 0).

			src_base = src_buf + dwRead;
			i_srccnt = -dwRead;
		}

		__asm
		{
			mov		eax,[flags]			// eax = flags
			mov		esi,[i_srccnt]		// esi = i_srccnt
			mov		ecx,[r]				// ecx = r
			mov		ebx,[src_base]		// ebx = src_base
			lea		edi,[text_buf]		// edi = text_buf

			ret
		}

DONE:

#else
		int j, k;

		for (;;)
		{
			// Shift encoding flags & decrement count.
			flags >>= 1;

			// Check count to see if we need new flags.
			if ((flags & 256) == 0)
			{
				// Read a character.
				if (i_srccnt == 0)
				{
					// Read buffer.
					bRet = ReadFile(hFileSrc, src_buf, i_bufsize, &dwRead, NULL);
					Assert(bRet);

					// EOF.
					if (dwRead == 0)
						break;

					src_base = src_buf + dwRead;
					i_srccnt = -dwRead;
				}

				flags = src_base[i_srccnt++];

				flags |= 0xff00;			// Uses higher byte cleverly to count eight.
			}

			// Check encoding flags.
			if (flags & 1)
			{
				// Read a character.
				if (i_srccnt == 0)
				{
					// Read buffer.
					bRet = ReadFile(hFileSrc, src_buf, i_bufsize, &dwRead, NULL);
					Assert(bRet);

					// EOF.
					if (dwRead == 0)
						break;

					src_base = src_buf + dwRead;
					i_srccnt = -dwRead;
				}

				// Copy character.
				text_buf[r++] = src_base[i_srccnt++];

				// See if we need to write.
				if (r == N)
				{
					// Write from ring buffer.
					bRet = WriteFile(hFileDest, text_buf+i_dest_start, r-i_dest_start, &dwWritten, NULL);
					if (!bRet || (r-i_dest_start != dwWritten))
					{
						plid->i4Error = -2;
						CloseHandle(hFileSrc);
						CloseHandle(hFileDest);
						goto ThreadError;
					}

					// Show progress.
					u4_bytes_written += r-i_dest_start;
					iPercent = (u4_bytes_written * 100 / u4_file_size);
					if (iPercent > iLastPercent)
					{
						if (plid->pfnWorldNotify)
						{
							(plid->pfnWorldNotify)(plid->u4NotifyParam, 2, iPercent, 0);
						}

						iLastPercent = iPercent;
					}

					r = 0;
					i_dest_start = 0;
				}
			}
			else
			{
				// Read a 2 characters.
				if (i_srccnt > -2)
				{
					// Read buffer.
					if (i_srccnt == 0)
					{
						bRet = ReadFile(hFileSrc, src_buf, i_bufsize, &dwRead, NULL);
						Assert(bRet);

						// EOF.
						if (dwRead == 0)
							break;

						src_base = src_buf + dwRead;
						i_srccnt = -dwRead;
					}

					i = src_base[i_srccnt++];

					// Read a character.
					if (i_srccnt == 0)
					{
						// Read buffer.
						bRet = ReadFile(hFileSrc, src_buf, i_bufsize, &dwRead, NULL);
						Assert(bRet);

						// EOF.
						if (dwRead == 0)
							break;

						src_base = src_buf + dwRead;
						i_srccnt = -dwRead;
					}

					j = src_base[i_srccnt++];
				}
				else
				{
					i = src_base[i_srccnt++];
					j = src_base[i_srccnt++];
				}
				
				// Convert to index & count.
				i |= ((j & 0xf0) << 4);	 
				j = (j & 0x0f) + THRESHOLD;
						
				for (k = i; k <= i+j; k++)
				{
					// Store in ring buffer.
					text_buf[r++] = text_buf[k & (N - 1)];

					// See if we need to write.
					if (r == N)
					{
						// Write from ring buffer.
						bRet = WriteFile(hFileDest, text_buf+i_dest_start, r-i_dest_start, &dwWritten, NULL);
						if (!bRet || (r-i_dest_start != dwWritten))
						{
							plid->i4Error = -2;
							CloseHandle(hFileSrc);
							CloseHandle(hFileDest);
							goto ThreadError;
						}

						// Show progress.
						u4_bytes_written += r-i_dest_start;
						iPercent = (u4_bytes_written * 100 / u4_file_size);
						if (iPercent > iLastPercent)
						{
							if (plid->pfnWorldNotify)
							{
								(plid->pfnWorldNotify)(plid->u4NotifyParam, 2, iPercent, 0);
							}

							iLastPercent = iPercent;
						}

						r = 0;
						i_dest_start = 0;
					}
				}
			}
		}
#endif

		// Flush buffer.
		bRet = WriteFile(hFileDest, text_buf+i_dest_start, r-i_dest_start, &dwWritten, NULL);
		if (!bRet || (r-i_dest_start != dwWritten))
		{
			plid->i4Error = -2;
			CloseHandle(hFileSrc);
			CloseHandle(hFileDest);
			goto ThreadError;
		}

		// Show progress as done.
		if (plid->pfnWorldNotify)
		{
			(plid->pfnWorldNotify)(plid->u4NotifyParam, 2, 100, 0);
		}

		// Close the files.
		CloseHandle(hFileSrc);
		CloseHandle(hFileDest);
	}
	else
	{
		int			i_file_s;
		OFSTRUCT	ofs_s;
		BYTE        ab[4096];
		int         iRead;
		HANDLE      hFile;
		DWORD       dwWritten;
		BOOL        bRet;
		int         iLen;
		int         iChunk;
		int			iPercent;
		int			iLastPercent;

		// Open the source file.
		i_file_s = LZOpenFile((char*)plid->strSourceImage,&ofs_s,OF_READ);
		if (i_file_s < 0)
		{
			goto ThreadError;
		}

		// Open the destination file.
		hFile = CreateFile((char*)plid->strLocalImage,
						   GENERIC_WRITE,
						   0,
						   NULL,
						   CREATE_ALWAYS,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			LZClose(i_file_s);
			goto ThreadError;
		}

		iLen = LZSeek(i_file_s, 0, 2);
		LZSeek(i_file_s, 0, 0);

		iLen /= 4096;
		iChunk = 0;
		iLastPercent = 0;

		if (plid->pfnWorldNotify)
		{
			(plid->pfnWorldNotify)(plid->u4NotifyParam, 3, 0, 100);
		}

		while ((iRead = LZRead(i_file_s, (LPSTR)&ab, sizeof(ab))) > 0)
		{
			iChunk++;
			iPercent = (iChunk * 100 / iLen);
			if (iPercent > iLastPercent)
			{
				if (plid->pfnWorldNotify)
				{
					(plid->pfnWorldNotify)(plid->u4NotifyParam, 2, iPercent, 0);
				}

				iLastPercent = iPercent;
			}

			bRet = WriteFile(hFile, ab, iRead, &dwWritten, NULL);
            if (!bRet || (iRead != dwWritten))
            {
                plid->i4Error = -2;
		        LZClose(i_file_s);
		        CloseHandle(hFile);
                goto ThreadError;
            }
		}

		if (plid->pfnWorldNotify)
		{
			(plid->pfnWorldNotify)(plid->u4NotifyParam, 2, 100, 0);
		}

		// close the files and clean up
		LZClose(i_file_s);
		CloseHandle(hFile);
	}

	dprintf("done creating local swap file...\n");
	dprintf("in %f seconds.\n", (double)ctmr() * ctmr.fSecondsPerCycle());
	bCopyResult = true;

	// Now set the time stamp of the swap file to be the same as the swap file source
	bFileSetTimeStamp(plid->strLocalImage, plid->u8SourceImageTime);

    // Signal that the CopyLocal event is complete
    SetEvent(hCopyLocal);

	return 1;

	//******************************************************************************************
ThreadError:
	dprintf("Failed to create local swap file..\n");
	AlwaysAssert(0);
	bCopyResult = false;
    SetEvent(hCopyLocal);
	return 0;
}
