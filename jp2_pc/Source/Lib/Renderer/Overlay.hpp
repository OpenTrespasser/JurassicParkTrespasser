/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Screen overlay for flashes and such.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Overlay.hpp                                              $
 * 
 * 1     98.08.31 9:31p Mmouni
 * Initial version.
 * 
 ***********************************************************************************************/

#ifndef LIB_RENDERER_OVERLAY_HPP
#define LIB_RENDERER_OVERLAY_HPP


//
// Includes.
//
#include "Lib/Sys/Timer.hpp"
#include "Lib/Transform/Vector.hpp"
#include "Lib/View/Colour.hpp"


//
// Forward declarations.
//
class CPipelineHeap;
class CCamera;
class CTexture;
class CTextureColours;


//*********************************************************************************************
//
class COverlay
//
// Object that encapsulates stuff for a screen overlay polygon.
//
// Prefix: ovly
//
//**************************
{
private:
	bool				bEnabled;			// Overlay turned on?
	float				fPercent;			// Alpha blend percentage.
	CTexture*			ptexTexture;		// Texture (just alhpa colour) to use.

public:

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	// Default constructor.
	COverlay();

	// Destructor.
	~COverlay();


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Enable
	(
		float f_percent
	);
	//
	// Enables the overlay polygon.
	//
	//**************************

	//*****************************************************************************************
	//
	void Disable
	(
	);
	//
	// Disables the overlay polygon.
	//
	//**************************

	//*****************************************************************************************
	//
	void Add
	(
		const CCamera& cam,
		CPipelineHeap& rplhHeap
	);
	//
	// Adds the overlay polygon to the render.
	//
	//**************************
};


//
// Global variable declarations.
//

// A global instance of a particle system.
extern COverlay Overlay;


#endif LIB_RENDERER_OVERLAY_HPP
