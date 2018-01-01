/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		A console output for logging messages.
 *
 * Bugs:
 *
 * To do:
 *		Interface log with an MFC dialog box for realtime message debugging.
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageLog.hpp                                        $
 * 
 * 1     10/21/96 6:20p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_MESSAGELOG_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGELOG_HPP


//
// Includes.
//
#include "Lib/Sys/Textout.hpp"


//
// External declarations of global variables.
//

// Object for sending event log messages to.
extern CConsoleBuffer conMessageLog;

#endif
