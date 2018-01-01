/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Facilities useful for interfacing with DirectDraw:
 *			class CSize<>.
 *			namespace DirectDraw.
 *			class CInitDD.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/DD.hpp                                                       $
 * 
 * 18    8/25/98 6:27p Rvande
 * Removed 4.2ism from MW build
 * 
 * 17    8/25/98 11:50a Rvande
 * removed redundant class scope in class definition
 * 
 * 16    8/22/98 6:40p Pkeet
 * Removed the 4.2 specific externs for 'pdd' and 'pdd4.' Added the 'ReleaseAll' member
 * function to the 'CInitDD' object. Made a single global instance of the  'CInitDD' object.
 * 
 * 15    7/24/98 11:45a Shernd
 * Added IsCertified and Hardware checks for DirectDraw Init
 * 
 * 14    7/20/98 10:32p Pkeet
 * Upgraded to Direct3D 4's interface.
 * 
 * 13    7/05/98 8:08p Pkeet
 * Added a comment.
 * 
 * 12    10/28/97 1:01p Shernd
 * Changed the DirectDraw Video abstraction to load on an explicit location.  This location is
 * now in GUIApp.cpp.  This change allows the ability to display error dialogs if DirectX
 * cannot be initialized.
 * 
 * 11    10/08/96 9:15p Agrant
 * bool is now a typedef,
 * pragma the bool warning off after DirectX turns it on.
 * 
 * 10    96/08/09 10:58 Speter
 * Changed for new CCom functionality.
 * 
 * 9     96/08/05 12:14 Speter
 * Now use new IDirectDraw2 pointer, has minor improvements over IDirectDraw.
 * 
 * 8     96/06/12 20:38 Speter
 * Code review changes:
 * Now includes <DirectX/DDraw.h> in header file, with #undef bool for bug fix.
 * Lists prefix for IDirectDraw.
 * Changed struct CSize to class CDDSize.
 * 
 * 7     96/06/07 18:32 Speter
 * Made DirectDraw::CError::operator= call TerminalError.
 * Moved error strings into resource file.
 * 
 * 6     96/06/07 9:57 Speter
 * Moved video mode stuff to new Video module.
 * 
 * 5     96/05/29 17:10 Speter
 * Changed init classes to have a per-file static instance rather than use inheritance to
 * enforce order.
 * 
 * 4     5/08/96 11:21a Pkeet
 * Upped 'iMAX_MODES' to 32. Suddenly there are more modes... bitchin'.
 * 
 * 3     96/05/01 17:54 Speter
 * Changed CScreenMode to SScreenMode.  Changed location of DDraw.h include
 * 
 * 2     96/04/19 17:55 Speter
 * Moved CSize<> template from .cpp to .hpp, because clients need it.
 * 
 * 1     96/04/18 18:56 Speter
 * First version
 *
 **********************************************************************************************/

#ifndef HEADER_W95_DD_H
#define HEADER_W95_DD_H

// An error occurs at <Oaidl.h> line 524, which is included by DDraw.h.
// At this line, there is a structure member called "bool", which conflicts with STL's <bool.h>,
// as well as all currently accepted notions of sanity.
// The pragma after the include fixes the problem.

#include <DirectX/DDraw.h>

// Get rid of the bool reserved word warning.
#pragma warning(disable:4237)


#include "Com.hpp"

//
// This contains the following type:
//
// struct IDirectDraw2;
// Prefix: dd
//
// Note: IDirectDraw2 is a newer, slightly improved, version of IDirectDraw.
//


//**********************************************************************************************
//
template<class T> class CDDSize: public T 
//	
//	Handy template class for all those DD structs that require a dwSize field to be set properly.
//
//	Example: instead of saying:
//
//		DDSURFACEDESC2	sd;
//		memset(&sd, 0, sizeof(sd));
//		sd.dwSize = sizeof(sd);
//
//	Say:
//
//		CDDSize<DDSURFACEDESC2>	sd;
//
//**************************************
{
public:
	// Constructor just sets everything to 0, then sets the correct dwSize field.
	CDDSize() 
	{
		memset(this, 0, sizeof(*this));
		dwSize = sizeof(*this);
	}
};


#ifndef __MWERKS__
#if _MSC_VER <= 1020
	//
	// Stupid compiler requires this external declaration, otherwise it will not be able to find
	// the struct 'IDirectDraw2.'
	//
	extern CCom<IDirectDraw2>	pdd;		// The pointer through which all functions are accessed.
	extern CCom<IDirectDraw4>	pdd4;		// The pointer through which all functions are accessed.
	#error Bullcrap needed for 4.2. Remove this error message to get 4.2 to compile.
#endif
#endif	// MWERKS


//**********************************************************************************************
//
namespace DirectDraw
//
// Encapsulation of some IDirectDraw items.
// Contains the IDirectDraw2* pointer, and an error variable used to handle errors.
//
// These items are automatically initialised by CInitDD below.
//
//**************************************
{
	extern CCom<IDirectDraw2>	pdd;		// The pointer through which all functions are accessed.
	extern CCom<IDirectDraw4>	pdd4;		// The pointer through which all functions are accessed.

	//**********************************************************************************************
	//
	class CError
	//
	// An error handling class for DirectDraw errors.
	// Uses the assign operation to make error termination easy.
	// You only need the one that's declared below.
	//
	// Example:
	//		#include "dd.h"
	//
	//		IDirectDrawSurface*	pdds;
	//		DirectDraw::err = pdds->Flip();
	//		// If Flip returns anything other than 0, the program will show
	//		// an error dialog with the option to abort.
	//
	//**********************************
	{
	public:

		//**************************************************************************************
		//
		// Assignment operator.
		//

		//**************************************************************************************
		//
		void operator =
		(
			int i_err					// An error code returned by a DirectDraw function.
		);
		//
		// Displays an error message corresponding to i_err if i_err != 0. 
		// Assign a DirectDraw return code to this object to invoke the error handling.
		//
		//******************************
	};

	// Now declare a CError variable.  This is the only one ever needed.
	extern CError		err;
};


//******************************************************************************************
//
class CInitDD
//
// An init class for this module.  See InitSys.hpp for a description of init classes.
//
//**********************************
{
public:
	CInitDD();
	~CInitDD();

	// This actually will perform the DDRAW.DLL demand loading
	int32 Initialize();
    int32 BaseInit();
	void ReleaseAll();

    bool IsCertified();
    bool IsHardwareSupported();

protected:
    HINSTANCE m_hDDraw;         // library handle
};


//
// Create a single global instance of the DirectDraw object.
//
extern CInitDD g_initDD;


#endif

