/**********************************************************************
 *<
	FILE: ioapi.h

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __IOAPI__H
#define __IOAPI__H

class ILoad;

class PostLoadCallback {
	public:
	virtual void proc(ILoad *iload)=0;
	};

typedef enum {IO_OK=0, IO_END=1, IO_ERROR=2} IOResult; 
typedef enum {NEW_CHUNK=0, CONTAINER_CHUNK=1, DATA_CHUNK=2} ChunkType;

class ISave {
	public:
		virtual ~ISave(){};
		
		// Returns the index of the referenced object in the Scene stream.
		virtual int GetRefID(void *ptarg)=0;

		// Begin a chunk.
		virtual void BeginChunk(USHORT id)=0;

		// End a chunk, and back-patch the length.
		virtual void EndChunk()=0;

		virtual	int CurChunkDepth()=0;  // for checking balanced BeginChunk/EndChunk

		// write a block of bytes to the output stream.
		virtual IOResult Write(const void  *buf, ULONG nbytes, ULONG *nwrit)=0;

		// Write character strings
		virtual IOResult WriteWString(const char *str)=0;
		virtual IOResult WriteWString(const wchar_t *str)=0;
		virtual IOResult WriteCString(const char *str)=0;
		virtual IOResult WriteCString(const wchar_t *str)=0;

		};


class ILoad {
	public:
		virtual ~ILoad(){};
		
		// Returns the memory address of the ith object Scene stream.
		virtual void* GetAddr(int imaker)=0;

		// If GetAddr() returns NULL, then call this to get the address
		// backpatched later, when it is known.  patchThis must point at
		// a valid pointer location. RecordBackpatch will patch the
		// address immediately if it is available.
		virtual void RecordBackpatch(int imaker, void** patchThis)=0;

		// When the root of a reference hierarchy is loaded, its
		// Load() can call this to store away a pointer to itself
		// for later retrieval.
		virtual void SetRootAddr(void *addr)=0;
		virtual void* GetRootAddr()=0;

		// if OpenChunk returns IO_OK, use following 3 function to get the 
		// info about the chunk. IO_END indicates no more chunks at this level
		virtual IOResult OpenChunk()=0;

		// These give info about the most recently opened chunk
		virtual USHORT CurChunkID()=0;
		virtual ChunkType CurChunkType()=0;
		virtual	ULONG CurChunkLength()=0;  // chunk length NOT including header
		virtual	int CurChunkDepth()=0;  // for checking balanced OpenChunk/CloseChunk

		// close the currently opened chunk, and position at the next chunk
		//  return of IO_ERROR indicates there is no open chunk to close
		virtual IOResult CloseChunk()=0;

		// Look at the next chunk ID without opening it.
		// returns 0 if no more chunks
		virtual	USHORT PeekNextChunkID()=0;

		// Read a block of bytes from the output stream.
		virtual IOResult Read(void  *buf, ULONG nbytes, ULONG *nread )=0;

		// Read a string from a string chunk assumes chunk is already open, 
		// it will NOT close the chunk. Sets buf to point
		// to a char string.  Don't delete buf: ILoad will take care of it.

		//   Read a string that was stored as Wide chars. 
		virtual IOResult ReadWStringChunk(char** buf)=0;
		virtual IOResult ReadWStringChunk(wchar_t** buf)=0;

		//   Read a string that was stored as single byte chars
		virtual IOResult ReadCStringChunk(char** buf)=0;
		virtual IOResult ReadCStringChunk(wchar_t** buf)=0;

		// Call this if you encounter obsolete data to cause a
		//  message to be displayed after loading.
		virtual	void SetObsolete()=0;		

		// Register procedure to be called after loading. These will
		// be called in the order that they are registered.
		// It is assumed that if the callback needs to be deleted,
		// the proc will do it.
		virtual void RegisterPostLoadCallback(PostLoadCallback *cb)=0;

		// Gets the various directories. Constants are defined in
		// JAGAPI.H
		virtual TCHAR *GetDir(int which)=0;
	};




#endif
