/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Code to modify the page tables associated with the application.
 *
 * Bugs:
 *
 * To do:
 *		Rewrite to coding standards.
 *
 * Notes:
 *		Currently a lot of this code is borrowed from a Microsoft example, so it has some ugly
 *		Microsoft code thingies in it.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/ExePageModify.hpp                                             $
 * 
 * 1     97/08/28 19:12 Speter
 * Restored to project, 97-8-28
 * 
 * 1     8/25/97 6:50p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_SYS_EXEPAGEMODIFY_HPP
#define HEADER_LIB_SYS_EXEPAGEMODIFY_HPP


//
// Function prototypes.
//

//*********************************************************************************************
//
void SetupForSelfModifyingCode
(
	HINSTANCE hinst	// Application instance.
);
//
// Modify the executable to allow self-modifying code.
//
//**************************************

#endif // HEADER_LIB_SYS_EXEPAGEMODIFY_HPP
