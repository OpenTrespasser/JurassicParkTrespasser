/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CDaemon definition.
 *
 * Bugs:
 *
 * To do:
 *		The global random functions must be replaced by the CRandom class!
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/Daemon.hpp                                         $
 * 
 * 13    8/13/98 6:10p Mlange
 * Made destructor out of line.
 * 
 * 12    8/13/98 1:48p Mlange
 * Removed unused step message process function.
 * 
 * 11    3/18/98 4:08p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 * 10    11/15/97 9:37p Agrant
 * Overlays now take a centered flag and a throwaway flag.
 * 
 * 9     97/06/23 19:09 Speter
 * Added empty virtual destructor, so list gets freed.
 * 
 *********************************************************************************************/

#ifndef HEADER_GAME_DESIGNDAEMON_DAEMON_HPP
#define HEADER_GAME_DESIGNDAEMON_DAEMON_HPP


//
// Includes.
//
#include <list.h>
#include "Lib/EntityDBase/Subsystem.hpp"
#include "Lib/Sys/Timer.hpp"

class CRaster;

//
// Class definitions.
//

//**********************************************************************************************
//
class CDaemon : public CSubsystem
//
// Prefix: dem
//
//**************************************
{
protected:
public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CDaemon();
	~CDaemon();

	//*****************************************************************************************
	//
	// Member functions.
	//

protected:

	//*****************************************************************************************
	void Dump();

};


//**********************************************************************************************
struct SOverlay
// Prefix: ovl
{
	rptr<CRaster> prasOverlay;		// The raster to display.
	CVector2<int> v2iOverlayPos;	// Position of the raster on the screen.
									// If either coord is negative, the end of
									// the raster is that many pixels from the end
									// of the screen.
		
	bool bThrowaway;				// True if the overlay is only to appear once.
	bool bCentered;					// True if the overlay is to be centered.

	SOverlay()
		: v2iOverlayPos(0, 0), bThrowaway(false), bCentered(false)
	{
	}
};

//**********************************************************************************************
//
class CVideoOverlay: public CSubsystem
//
// Prefix: vidovl
//
// Handles any video overlays on the screen.
// Currently, allows a set of rasters that is cycled through periodically.
//
//**************************************
{
private:
	CContainer< list<SOverlay> >	lsovlOverlays;	// The set of overlays to cycle through.
	TMSec							msPeriod;		// The period of display for each overlay.
	CTimer							tmrCycle;		// The timer used for cycling.
	bool							bEnabled_;		// Whether to enable the overlays.

public:
	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	CVideoOverlay();

	virtual ~CVideoOverlay();

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void AddOverlay
	(
		const SOverlay& ovl				// Overlay spec to add.
	);
	//
	// Adds it to list.
	//
	//**********************************

	//******************************************************************************************
	//
	void SetPeriod
	(
		TMSec ms_period					// Cycle period for overlays.
	);
	//
	//**********************************

	//******************************************************************************************
	//
	void Enable
	(
		bool b_enable					// Flag to enable overlays.
	)
	//
	//**********************************
	{
		bEnabled_ = b_enable;
	}

	//******************************************************************************************
	//
	bool bEnabled() const
	//
	//**********************************
	{
		return bEnabled_;
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	void Process(const CMessagePaint& msgpaint);
};

// Single global instance.
extern CVideoOverlay* pVideoOverlay;

#endif
