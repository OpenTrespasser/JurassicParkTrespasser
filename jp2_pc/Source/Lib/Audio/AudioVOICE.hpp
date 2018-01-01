/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CAudioVOICE - VOICE audio loading and decompression class
 *
 *		Voice compression is only defined for 8bit mono data, if any other format of data is
 *		decompressed this class will assert
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *		The code contained within this file should be 100% thread safe, this is because
 *		streamed audio will be loaded from within a thread.
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/AudioVOICE.hpp                                              $
 * 
 * 4     8/04/98 3:58p Rwyatt
 * New constructors for loaders for samples created from databases as the cau header is in the
 * database index so does not need to be loaded
 * 
 * 3     6/08/98 12:56p Rwyatt
 * The database of a sample is now passed all the way down to the loader. This enables the
 * loader to close the file correctly for databases with static file handles.
 * 
 * 2     3/09/98 10:51p Rwyatt
 * Uses the new constructor of the base class CCAULoad. This is to enable support of handled
 * based sounds.
 * 
 * 1     11/14/97 12:01a Rwyatt
 * Loader for VOICE compressed samples
 * 
 ***********************************************************************************************/

#ifndef HEADER_AUDIO_VOICE_HPP
#define HEADER_AUDIO_VOICE_HPP




#endif