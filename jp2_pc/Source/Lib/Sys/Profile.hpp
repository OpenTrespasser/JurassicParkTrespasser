/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 *	CProfileStat
 *		CProfileStatMain
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/Profile.hpp                                                   $
 * 
 * 62    9/02/98 2:38p Rwyatt
 * Removed constructor in final mode to prevent it from being called.
 * 
 * 61    98/08/28 12:10 Speter
 * Removed CProfileStatOther, replaced with epfOTHER flag on parent (ensures Other is last).
 * 
 * 60    8/25/98 11:44a Rvande
 * Forward declaration of vector scoped for STL usage
 * 
 * 59    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 58    98/07/27 13:08 Speter
 * Added b_show_hidden args, WriteHeader function; allows 2 levels of detail in stats showing.
 * 
 * 57    98/07/19 17:52 Speter
 * Added epfOVERLAP flag, CTimeBlock class.
 * 
 * 56    4/01/98 5:46p Pkeet
 * Added profile stats for hardware.
 * 
 * 55    98.03.04 12:12p Mmouni
 * Changed performance counters for more consistent results.
 * 
 * 54    98/03/02 21:41 Speter
 * New psRender stat incorporates all rendering activity.
 * 
 * 53    2/06/98 8:18p Rwyatt
 * New stats for overlay text - subtitles etc
 * 
 * 52    98/01/19 21:08 Speter
 * Replaced bShow member with epfHIDDEN flag.
 * 
 * 51    98/01/08 12:57 Speter
 * proProfile is again an actual variable.  pragma init_seg controls initialisation.
 * 
 * 50    98/01/06 15:40 Speter
 * Moved texture stats to proProfile.
 * 
 * 49    97/10/07 5:34p Pkeet
 * Added the 'psOcclusion' stat.
 * 
 * 48    8/28/97 4:05p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 49    8/26/97 2:00a Rwyatt
 * A new global for normalizing stats to ms or leaving them in raw ticks
 * 
 * 48    8/25/97 5:21p Rwyatt
 * Modified to allow access to performance counters, also added some global variables.
 * 
 * 47    7/29/97 2:03p Agrant
 * #if VER_TIMING_STATS, not #ifdef VER_TIMING_STATS
 * 
 * 46    7/27/97 2:15p Agrant
 * Accessor functions for profile stat variables
 * 
 * 45    97/07/07 20:50 Speter
 * Made members protected again.  Placed proper macro on destructor to fix Final build.
 * 
 * 44    97/07/07 13:45 Speter
 * Made CProfileStat members public.  Removed psWorldInstances.
 * 
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_W95_PROFILE_HPP
#define HEADER_LIB_W95_PROFILE_HPP

#include "Lib/Std/Set.hpp"
#include "Lib/Std/StringEx.hpp"
#include "Lib/Sys/P5/Msr.hpp"

//*********************************************************************************************
//
// Opaque declarations for CProfile.
//

#ifdef __MWERKS__
#include <vector.h>
#else
template<class T> class vector;
#endif

class CConsoleBuffer;

//
// Defines.
//

// Macro to conditionally define function as empty.
#if VER_TIMING_STATS
	// Function defined in .cpp file.
	#define FN_TIMING_STATS		;
	#define FN_TIMING_STATS_RETURN_0	;
#else
	// Function does nothing.
	#define FN_TIMING_STATS		{}
	#define FN_TIMING_STATS_RETURN_0	{ return 0; }
#endif

// Flag to determine if special profiling is active.
#define bUSE_SPECIAL_PROFILE 0

extern uint32	gu4MSRProfileTimerSelect;
extern bool		gbMSRProfile;
extern bool		gbUseRDTSC;
extern bool		gbNormalizeStats;

//*********************************************************************************************
//
class CCycleTimer
//
// Class to count clock cycles.
//
// Prefix ctmr
//
//**************************************
{
private:
#if VER_TIMING_STATS
	TCycles cyStart;
#endif

public:

	//*****************************************************************************************
	CCycleTimer()
	{
#if VER_TIMING_STATS
		// Initialise the timer.
		Reset();
#endif
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Reset()
	//
	// Resets the timer to 0.
	//
	//**********************************
	{
#if VER_TIMING_STATS
		cyStart = cyReadCycles();
#endif
	}

	//******************************************************************************************
	//
	TCycles operator ()()
	//
	// Returns:
	//		The number of cycles elapsed since last call to this function or Reset().
	//
	//**********************************
	{
#if VER_TIMING_STATS
		TCycles cy_elapsed = cyReadCycles() - cyStart;
		cyStart += cy_elapsed;
		return cy_elapsed;
#else
		return 0;
#endif
	}

	//******************************************************************************************
	//
	static float fSecondsPerCycle()
	//
	// Returns:
	//		A factor relating the cycle count to seconds on this machine.
	//
	//**********************************
		FN_TIMING_STATS_RETURN_0
};

//*********************************************************************************************
//
enum EProfileFlag
//
// Prefix: epf
//
// Various display features of stats.
//
{
	epfSEPARATE,		// Stat is shown separate from, and subtracted from, parent.
	epfOTHER,			// Shows time unaccounted by children as "Other".
	epfOVERLAP,			// Stat overlaps main set of stats in this group; doesn't subtract from "Other".
	epfRELATIVE,		// Stat count is shown relative to parent; else relative to master.
	epfMASTER,			// A stat whose count all descendents' counts are relative to.
	epfHIDDEN			// Stat is not shown automatically by parent, but may be shown on its own.
};


const CSet<EProfileFlag> setepf_empty = CSet<EProfileFlag>(set0);

//*********************************************************************************************
//
class CProfileStat
//
// Prefix: ps
//
// Class to keep track of clock cycles and other counts.
//
//**************************************
{
#if VER_TIMING_STATS
protected:
	const char*		strName;
	TCycles			cyCycles;			// Total cycles taken.
	int				iCount;				// Number of items/iterations.

	CProfileStat*	ppsParent;			// Stat enclosing this stat's timings (used as denominator).
	CSet<EProfileFlag> setepfFlags;		// Various flags affecting display.

	// A list of references to individual stats, for automatic printing, resetting, etc.
	vector<CProfileStat*>* pvcList;
#endif

public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CProfileStat(const char* str = "", CProfileStat* pps_parent = 0, CSet<EProfileFlag> setepf = setepf_empty)
		FN_TIMING_STATS

/*
	CProfileStat() 
	{
		//new(this) CProfileStat("", 0, set0);
	}
*/

	virtual ~CProfileStat()
		FN_TIMING_STATS

	//*****************************************************************************************
	//
	// Operators.
	//

	CProfileStat& operator +=(const CProfileStat& ps)
	{
#if VER_TIMING_STATS
		Add(ps.cyCycles, ps.iCount);
#endif
		return *this;
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Add
	(
		TCycles cy_cycles,				// Cycles to accumulate.
		int i_count = 0					// Count to accumulate.
	)
	//
	// Adds the given numbers to the stat.
	//
	//**********************************
	{
#if VER_TIMING_STATS
		cyCycles += cy_cycles;
		iCount += i_count;
#endif
	}

	//*****************************************************************************************
	//
	float fSeconds() const
	//
	// Returns:
	//		Total seconds accumulated for this stat.
	//
	//**********************************
		FN_TIMING_STATS_RETURN_0


	//*****************************************************************************************
	//
	float fSecondsPerCount() const
	//
	// Returns:
	//		Seconds taken by this stat, per unit count referenced by stat's denom.
	//
	//**********************************
		FN_TIMING_STATS_RETURN_0

	//*****************************************************************************************
	//
	float fCountPer() const
	//
	// Returns:
	//		Counts for this stat, relative to its parent.
	//
	//**********************************
		FN_TIMING_STATS_RETURN_0

	//*****************************************************************************************
	//
	int iGetCount() const
	//
	// Returns:
	//		iCount.
	//
	//**********************************
	{
#if VER_TIMING_STATS
		return iCount;
#else
		return 0;
#endif
	}

	//*****************************************************************************************
	//
	TCycles cyGetCycles() const
	//
	// Returns:
	//		cyCycles.
	//
	//**********************************
	{
#if VER_TIMING_STATS
		return cyCycles;
#else
		return 0;
#endif
	}

	//*****************************************************************************************
	//
	virtual void Reset()
	//
	// Resets stats to 0.
	//
	//**********************************
		FN_TIMING_STATS

	//*****************************************************************************************
	//
	virtual void WriteToBuffer
	(
		CStrBuffer& strbuf,				// Buffer to append to.
		int i_indent = 0,				// Number of spaces to indent.
		bool b_show_hidden = false		// Show all children, even if HIDDEN.
	)
	//
	// Outputs stat to the buffer.
	//
	//**********************************
		FN_TIMING_STATS

	//*****************************************************************************************
	//
	void WriteToConsole
	(
		CConsoleBuffer& con,
		bool b_show_hidden = false		// Show all children, even if HIDDEN.
	)
	//
	// Outputs stat to the console.
	//
	// Cross-references:
	//		Utilises WriteToBuffer.
	//
	//**************************************
		FN_TIMING_STATS

	//*****************************************************************************************
	//
	static void WriteHeader
	(
		CStrBuffer& strbuf
	)
	//
	// Writes a stat header to the buffer.
	//
	//**********************************
		FN_TIMING_STATS

	//*****************************************************************************************
	//
	static void WriteHeader
	(
		CConsoleBuffer& con
	)
	//
	// Writes a stat header to the console.
	//
	//**********************************
		FN_TIMING_STATS

protected:
	
	//*****************************************************************************************
	//
	virtual void AddStat
	(
		CProfileStat* pps
	)
	//
	// Adds pps to this stat's child list.
	//
	// Called by child stat upon its construction.
	//
	//**********************************
		FN_TIMING_STATS
};

//*********************************************************************************************
//
class CProfileStatMain: public CProfileStat
//
// Prefix: ps
//
// CProfileStat which prints out a header.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	CProfileStatMain(const char* str)
#if VER_TIMING_STATS
		;
#else
		: CProfileStat(str, 0 , Set(epfMASTER)) {}
#endif

	CProfileStatMain()
	{
		new(this) CProfileStatMain("");
	}

	//*****************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual void WriteToBuffer(CStrBuffer& strbuf, int i_indent = 0, bool b_show_hidden = false)
	//
	// Outputs self and children to the buffer, with header.
	//
	//**************************************
		FN_TIMING_STATS
};

typedef CProfileStat CProfileStatParent;

//*********************************************************************************************
//
struct SProfileMain
//
// Global individual profile stats.
//
//**************************************
{
	CProfileStatMain psMain;
		CProfileStat psFrame;					// Time for an entire frame.
			CProfileStat psStep;				// World update portion.
				CProfileStat psAI;
				CProfileStat psPhysics;

			// Pipeline stages.
			CProfileStat psRender;
				CProfileStat psTerrainUpdate;	// Terrrain update.
				CProfileStat psOcclusion;		// Occlusion setup.
				CProfileStat psRenderShape;		// Visibility/Geometry processing.
				CProfileStat psPresort;

				CProfileStat psDrawPolygon;
					CProfileStat psDrawPolygonInit;
					CProfileStat psDrawPolygonBump;

				CProfileStat psBeginFrame;
				CProfileStat psClearScreen;
				CProfileStat psEndFrame;
					CProfileStat psFlip;

			CProfileStat psText;
			CProfileStat psOverlayText;

	CProfileStat psTextures;
	CProfileStat psTextureKB;

	//******************************************************************************************
	SProfileMain()
		FN_TIMING_STATS
};

extern SProfileMain proProfile;

//*********************************************************************************************
//
struct SProfileHardware
//
// Global individual profile stats.
//
//**************************************
{
	CProfileStatMain psHardware;
		CProfileStat psHardwareFrame;
			CProfileStat psUploads;

	//******************************************************************************************
	SProfileHardware()
		FN_TIMING_STATS
};

extern SProfileHardware proHardware;

//*********************************************************************************************
//
class CSpecialProfile
//
// Prefix spec
//
// Special profiler.
//
//**************************************
{
public:

	int     iFrame;
	int     iCount;
	TCycles cySpecial;
	TCycles cyLastSpecial;
	TCycles cyAll;
	TCycles cyLastAll;

public:

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	CSpecialProfile()
	{
		Reset();
	}


	//*****************************************************************************************
	//
	// Member functions.
	//
	
	//*****************************************************************************************
	//
	void Start
	(
		int i_count
	)
	//
	// Starts count.
	//
	//**************************************
	{
		iCount += i_count;
		cyLastSpecial = cyReadCycles();
	}
	
	//*****************************************************************************************
	//
	void Stop
	(
	)
	//
	// Stops count.
	//
	//**************************************
	{
		cySpecial += cyReadCycles() - cyLastSpecial;
	}
	
	//*****************************************************************************************
	//
	void StartAll
	(
	);
	//
	// Starts count.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void StopAll
	(
	);
	//
	// Stops count.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void WriteToMainWindow
	(
	) const;
	//
	// Outputs profile to the main window raster.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void Reset
	(
	)
	//
	// Resets all 
	//
	//**************************************
	{
		iFrame        = 0;
		iCount        = 0;
		cySpecial     = 0;
		cyLastSpecial = 0;
		cyAll         = 0;
		cyLastAll     = 0;
	}

};

//**********************************************************************************************
//
class CTimeBlock
//
// Prefix: tmb
//
// Automatically times the block of code it's declared in, cleverly using ctor and dtor.
// Uses 1 as count value for the block.
// Useful in functions with multiple returns.
//
// Example:
//
//		void Func()
//		{
//			CTimeBlock	tmb(&psBarf);		// Times entire function, even with multiple returns.
//			...
//			if (b_done)
//				return;
//			...
//		}
//
//
//**************************************
{
	CProfileStat*	ppsStat;
	CCycleTimer		ctmrBlock;

public:
	CTimeBlock(CProfileStat* pps)
		: ppsStat(pps)
	{
		Assert(pps);
	}

	~CTimeBlock()
	{
		ppsStat->Add(ctmrBlock(), 1);
	}
};



// The main profile DB.
extern CSpecialProfile specProfile;

#endif
