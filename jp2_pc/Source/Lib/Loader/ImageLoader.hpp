/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		ImageLoader is responsible for relocating all objects as they are loaded and for getting
 *		the swap file locally on the users hard-drive...
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/ImageLoader.hpp                                            $
 * 
 * 15    9/08/98 8:51p Rwyatt
 * Removed MAX_PATH so windows,h is not required by this file.
 * 
 * 14    9/03/98 4:30p Shernd
 * Checking for out of disk space
 * 
 * 13    98.09.01 8:41p Mmouni
 * Added support for new compression method for image files.
 * 
 * 12    8/25/98 4:40p Rwyatt
 * Rset memory counter for load heap in reset code
 * 
 * 11    8/23/98 12:24a Shernd
 * While waiting for the local Copy to finish we now use an Even instead of a CritialSection.
 * 
 * 10    8/19/98 1:38p Rwyatt
 * New allocation functions to load from the Image Loader fast heap
 * 
 * 9     98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 8     8/03/98 6:35p Shernd
 * Changes for Progress Loading Bar
 * 
 * 7     7/08/98 12:13p Rwyatt
 * Adjsuted swap and pid file versions. This is for changes in texture hash generation,
 * 
 * 6     6/10/98 4:52p Rwyatt
 * Now loads compressed swap files (.spp). If one of these files cannot be found then the
 * existing swap file is loaded in the usual way,
 * 
 * 5     98.05.17 8:01p Mmouni
 * Changed swapfile version (to coincide with 16-bit bump changeover).
 * 
 * 4     4/23/98 7:58p Rwyatt
 * Copy local of swap files now deletes old swap file once the specified quota is reached. The
 * quota is obtained from the registry.
 * Commit all is now idsabled by default.
 * 
 * 3     4/21/98 3:10p Rwyatt
 * New format swap files (110).
 * These support a pageable and non-pageable section.
 * 
 * 2     2/25/98 3:16p Rwyatt
 * Added an extentsion to the swp and pid file so  multiples can exist for different versions of
 * the GUIApp
 * 
 * 1     2/18/98 1:09p Rwyatt
 * Moved from TextureManager.hpp
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_LOADER_IMAGELOADER_HPP
#define HEADER_LIB_LOADER_IMAGELOADER_HPP

#include "Lib/Sys/FastHeap.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/View/Colour.hpp"
#include "Lib/View/Palette.hpp"
#include <vector>
#include <map>
#include <set>


//**********************************************************************************************
//
// Directory File Structures
//
//**********************************************************************************************
//
#define u4DIRECTORY_FILE_VERSION	0x130
#define strSWP_FILE_EXTENTSION		"-130.swp"
#define strPID_FILE_EXTENTSION		"-130.pid"
#define strCOMP_FILE_EXTENTSION_OLD	"-130.spp"
#define strCOMP_FILE_EXTENTSION		"-130.spz"


#ifndef PFNWORLDLOADNOTIFY
typedef uint32 (__stdcall * PFNWORLDLOADNOTIFY)(uint32 dwContext, uint32 dwParam1, uint32 dwParam2, uint32 dwParam3);
#endif

//**********************************************************************************************
//
struct SDirectoryFileHeader
{
	uint32	u4Version;
	uint32	u4BumpMapBitDepth;			// number of bits in the bump maps ithin the swap file
	uint32	u4RasterChunkOffset;		// file offset from start of file to first Raster chunk
	uint32	u4RasterChunkCount;			// number of raster chunks
	uint32	u4PaletteOffset;			// offset to the palettes
	uint32	u4PaletteCount;				// number of palettes
	uint32	u4PageableOffset;			// Offset of the first byte that can be dynamically paged
	uint32	u4NonPageableCount;			// Number of bytes from the start of the swap file that 
										// cannot be paged. This can be less than u4PageableOffset
};


//**********************************************************************************************
//
struct SDirectoryFileChunk
{
	uint32	u4Size;						// size of this chunk (offset to next chunk)
	uint32	u4VMOffset;					// Pack file (Virtual memory block) offset
	uint32	u4Width;
	uint32	u4Height;					// dimensions of the raster
	uint32	u4Stride;
	uint32	u4Bits;						// bits per pixel pf the raster
	CColour	clrConstCol;				// constant colour of the surface
	int		iTransparent;				// true for transparent
	int		iBumpMap;					// texture is a bump map
	int		iOcclusion;					// texture is an occlusion map
	uint32	u4Palette;					// index into palette array of 0xffffffff for no palette
	uint64	u8HashValue;				// ID of the raster
};


//**********************************************************************************************
//
struct SDirectoryPaletteChunk
{
	uint32	u4Size;						// size of this chunk (offset to next chunk)
	uint32	u4ColoursUsed;				// number of colours used in this palette
	uint32	u4HashValue;				// unique hash value for this palette
	CColour	clr[1];						// binary colour data in CColour format
	//
	// followed by u4ColoursUsed entries, this data must be reflected in the u4Size parameter.
	//
};


//**********************************************************************************************
//
struct SDiskSwapFile
{
	char	strFilename[256];			// Used to use MAX_PATH but it is not possible to include wininclude.hpp in all files
	uint32	u4Time;
	uint32	u4Size;
	bool	bDeleted;
};


//**********************************************************************************************
// This class is responsible for loading the Image file directory and relocating all the rasters
// when they are loaded fom the groff file.
//
class CLoadImageDirectory
// prefix: lid
//
//*************************************
{
public:

	static CLoadImageDirectory*			plidImageDir;
	static bool							bValidImage;
	static bool							bImageLoader;
	static bool							bAutoCommit;
    static HANDLE                       hCopyLocal;
	static bool							bCopyResult;
	static void*						pvThreadHandle;
	static bool							bCompressed;
	static bool							bNewCompression;

	std::map<uint64,SDirectoryFileChunk*, std::less<uint64> >	mapChunk;
	std::vector<CPal*>									appal;
	char											strLocalImage[264];
	char											strSourceImage[264];
	uint64											u8SourceImageTime;
	uint32											u4SourceSize;
	uint32											u4LocalImageSize;
    int32                                           i4Error;
    PFNWORLDLOADNOTIFY                              pfnWorldNotify;
    uint32                                          u4NotifyParam;

	//*****************************************************************************************
	// Allocate the requested number of bytes from the image loader fast heap, The memory 
	// allocated by this function should not be deleted with the delete member.
	//
	static void* pvAllocate
	(
		uint32 u4_size
	)
	//*************************************
	{
		MEMLOG_ADD_COUNTER(emlLoadHeap,u4_size);
		return fhImageLoad.pvAllocate( u4_size );
	}

	//*********************************************************************************************
	//
	static bool bLoadHeapAllocation
	(
		void* pv
	)
	//*************************************
	{
		return ( ((uint32)pv < ((uint32)fhImageLoad.uNumBytesUsed() + (uint32)fhImageLoad.pvGetBase())) &&
		((uint32)pv >= (uint32)fhImageLoad.pvGetBase()) );
	}


	//*********************************************************************************************
	//
	static void ResetLoadHeap
	(
	)
	//*************************************
	{
		// Reset the load heap and remove all committed memory!
		fhImageLoad.Reset(0,0);
		MEMLOG_SET_COUNTER(emlLoadHeap,0);
	}


	//******************************************************************************************
	// Constructors and Destructor
	//
	CLoadImageDirectory
	(
		const char*	str_grf_filename,			// the GRF file being loaded
        PFNWORLDLOADNOTIFY pfnInWorldNotify = NULL, 
        uint32 u4InNotifyParam = 0
	);


	~CLoadImageDirectory();


	//******************************************************************************************
	bool bProcessLocalSwapFile
	(
		const char*		str_grf_name			// name of the GRF swap file
	);


	//******************************************************************************************
	// Member functions
	//
	CPal*	ppalGetPalette(uint32 u4_pal)
	{
		return appal[u4_pal];
	}
	
	//******************************************************************************************
	// Static Member functions
	//
	static bool bImageValid()
	{
		return CLoadImageDirectory::bValidImage;
	}

	//******************************************************************************************
	// Disable or enable the image loader
	//
	static void Enable
	(
		bool	b_state
	)
	//*********************************
	{
		bImageLoader = b_state;
	}

	//******************************************************************************************
	// returns a bool with the enabled status of the image loader
	//
	static bool bEnabled()
	{
		return bImageLoader;
	}

	//******************************************************************************************
	// thread function for background copy and decompress
	//
	static uint32 u4CopyLocalThread(uint32 u4_user);

protected:
	SDirectoryFileHeader*	pdfhDirHeader;
	static CFastHeap		fhImageLoad;
};



#endif //HEADER_LIB_LOADER_IMAGELOADER_HPP
