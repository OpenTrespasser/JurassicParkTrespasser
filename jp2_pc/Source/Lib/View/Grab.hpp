/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		CRasterGrab
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Grab.hpp                                                     $
 * 
 * 6     8/05/98 5:28p Mlange
 * Added auto frame grab capability.
 * 
 * 5     2/10/98 8:45p Pkeet
 * Added functions and data members to create borders for grabs that cannot be sized exactly
 * correctly.
 * 
 * 4     2/10/98 8:21p Pkeet
 * Added the invert grab macro.
 * 
 * 3     2/10/98 7:05p Pkeet
 * Changed prefix. Added notes.
 * 
 * 2     2/10/98 6:22p Pkeet
 * Made the grab constants 'const.' Changed their values.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_GRAB_HPP
#define HEADER_LIB_VIEW_GRAB_HPP

#include "Lib/EntityDBase/Subsystem.hpp"
#include <string.h>

//
// Macros.
//

// Macro to invert the DIB image.
#define bINVERT_GRAB (0)


//
// Constants.
//

// Default sizes.
const int iGrabDefaultWidth  = 80;
const int iGrabDefaultHeight = 60;


//
// Forward declarations.
//
class CColour24;


//
// Class definitions.
//

//**********************************************************************************************
//
class CGrabRaster
//
// Class for grabbing, shrinking and filtering the main view raster.
//
// Prefix: grab
//
// Notes:
//		The following screen sizes are strongly recommended, as they match the most common
//		aspect ratios:
//
//			160 x 120
//			80 x 60
//
//**************************************
{
private:

	int        iWidth;		// Width of the grabbed image.
	int        iHeight;		// Height of the grabbed image.
	CColour24* pcol3Data;	// Raw bitmap data.
	CColour24* pcol3Border;	// Border colour.

public:

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	// Default constructor.
	CGrabRaster();

	// Destructor deletes memory associated with the shared surface.
	~CGrabRaster();


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SetDimensions
	(
		int i_width  = iGrabDefaultWidth,	// Width of the bitmap.
		int i_height = iGrabDefaultHeight	// Height of the bitmap.
	);
	//
	// Sets the dimensions of the bitmap to hold the grabbed image.
	//
	// Notes:
	//		To conform to Windows requirements, the width must always be a multiple of 16.
	//
	//**********************************

	//******************************************************************************************
	//
	int iGetWidth
	(
	) const
	//
	// Returns the width of the grabbed image.
	//
	//**********************************
	{
		return iWidth;
	}

	//******************************************************************************************
	//
	int iGetHeight
	(
	) const
	//
	// Returns the height of the grabbed image.
	//
	//**********************************
	{
		return iHeight;
	}

	//******************************************************************************************
	//
	int iGetSizeBytes
	(
	) const
	//
	// Returns the size of the grabbed image in bytes.
	//
	//**********************************
	{
		return iWidth * iHeight * 3;
	}

	//******************************************************************************************
	//
	void SetBorderColour
	(
		int i_red   = 0,	// RGB colour values, defaults to black.
		int i_green = 0,
		int i_blue  = 0
	);
	//
	// Sets the border colour.
	//
	//**********************************

	//******************************************************************************************
	//
	void GrabBackbuffer
	(
	);
	//
	// Grabs the backbuffer from system memory.
	//
	//**********************************

	//******************************************************************************************
	//
	void* pvGetRawData
	(
	) const;
	//
	// Returns the raw bitmap from the last grab. If no grab was performed prior to calling
	// this function, it will return a null pointer.
	//
	// Notes:
	//		This function will return pixel data for a 24-bit windows bitmap in the form
	//		blue-green-red. This is consistent with the Windows DIB format for 24-bit bitmaps.
	//
	//**********************************

	//******************************************************************************************
	//
	void ReleaseLastGrab
	(
	);
	//
	// Releases the result of the last memory grab.
	//
	// Notes:
	//		This function should always be called after 'GrabBackbuffer.'
	//
	//**********************************

	//******************************************************************************************
	//
	void DumpBitmap
	(
		const char* str = "Grab.bmp"
	) const;
	//
	// Dumps a screen grab to a windows bitmap file of the given name.
	//
	//**********************************

};



class CMessagePaint;

//**********************************************************************************************
//
class CAutoGrabber : public CSubsystem
//
// Continously grabs frame buffer on every paint.
//
// Prefix: ag
//
//**************************************
{
	CGrabRaster* pgrabActive;

	int iFrameCount;

	string strBaseName;

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
public:
	CAutoGrabber();

	~CAutoGrabber();

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	bool bIsActive() const
	//
	// Returns:
	//		'true' if auto grabber is currently active.
	//
	//**************************************
	{
		return pgrabActive != 0;
	}


	//******************************************************************************************
	//
	void Start
	(
		const char* str_output_name,
		int i_width, int i_height
	);
	//
	// Start grabbing.
	//
	//**************************************

	//******************************************************************************************
	//
	void Stop();
	//
	// Stop grabbing.
	//
	//**************************************

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void Process(const CMessagePaint& msgpaint);
};


//
// Global variables.
//

// Global instance of the grab object.
extern CGrabRaster grabRaster;

extern CAutoGrabber* pagAutoGrabber;

#endif

