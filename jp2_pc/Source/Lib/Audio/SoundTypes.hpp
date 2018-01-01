/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	Standard header file for sound library, this is required so that the audio library
 *  Can be used in other projects without requiring most of the game.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/SoundTypes.hpp                                              $
 * 
 * 4     9/27/98 10:00p Rwyatt
 * Included UAssert
 * 
 * 3     6/02/97 4:49p Rwyatt
 * Only define Assert if it is not already defined
 * 
 * 2     5/29/97 4:20p Rwyatt
 * This file is only used in the sound library, it is used in place of common.hpp so that the
 * audio library can be used outside of the game - suchas the collision editor. If common.hpp is
 * used this is not the case because it declares some global static variables. most of the
 * project has to be included before it links.
 * 
 * 1     5/29/97 2:50p Rwyatt
 * Initial implementation
 * 
 ***********************************************************************************************/

#ifndef _H_SOUNDTYPESHEADER
#define _H_SOUNDTYPESHEADER

#include "windows.h"
#include "crtdbg.h"
#include "Lib/Std/UTypes.hpp"
#include "Lib/Std/UAssert.hpp"

#ifndef Assert
#define		Assert			_ASSERT
#endif

#undef min
#undef max


#endif