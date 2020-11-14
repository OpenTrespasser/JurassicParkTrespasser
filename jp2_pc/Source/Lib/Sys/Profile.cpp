/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Profile.hpp.
 *
 * To do:
 *		Eliminate the special-purpose arithmetic for certain stats in WriteToBuffer().
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/Profile.cpp                                                   $
 * 
 * 69    98/08/28 12:10 Speter
 * Removed CProfileStatOther, replaced with epfOTHER flag on parent (ensures Other is last).
 * 
 * 68    8/28/98 11:58a Asouth
 * STL differences; loop variable re-scoped
 * 
 * 67    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 66    98/07/27 13:08 Speter
 * Added b_show_hidden args, WriteHeader function; allows 2 levels of detail in stats showing.
 * 
 * 65    98/07/24 14:00 Speter
 * Implemented special MASTER + SEPARATE feature, showing count in absolute terms.
 * 
 * 64    98/07/19 17:52 Speter
 * Added epfOVERLAP flag, CTimeBlock class.
 * 
 * 63    98/04/26 20:17 Speter
 * Commented out BeginFrame/EndFrame stats (too many stats).
 * 
 * 62    4/08/98 12:53p Pkeet
 * Fixed compile bug.
 * 
 * 61    4/01/98 5:46p Pkeet
 * Added profile stats for hardware.
 * 
 * 60    3/05/98 3:37p Pkeet
 * Put the code that collapses unused stats on a hash define that defaults to off.
 * 
 * 59    98.03.04 12:11p Mmouni
 * Changed performance counters for more consistent results.
 * 
 * 58    98/03/02 21:41 Speter
 * New psRender stat incorporates all rendering activity.
 * 
 * 57    2/06/98 8:18p Rwyatt
 * New stats for overlay text - subtitles etc
 * 
 * 56    98/01/21 17:09 Speter
 * psText now subtracted from psFrame time.
 * 
 **********************************************************************************************/

#include "Common.hpp"

// Uses the Windows GetCurrentTime() function.
#include <Lib/W95/WinInclude.hpp>
#include "Profile.hpp"

#include "Lib/Sys/Textout.hpp"

#include <string.h>
#include <math.h>

#ifdef __MWERKS__
 #include <vector.h>
#else
 #include <vector>
 #include <algorithm>
#endif

//
// Module specific variables.
//


//
// Defines.
//

// Flag to determine if the rdtsc preparation is done.
#define bSIMPLE_VXD_TEST 0

// Flag causes unused stats to be removed if set.
#define bCOLLAPSE_STATS  (0)

#define iNAME_WIDTH	16

//
// Extra includes.
//
#if bUSE_SPECIAL_PROFILE
	#include <String.h>
	#include <StdIO.h>
	#include "Lib/W95/WinInclude.hpp"
	#include "Lib/View/Raster.hpp"
	#include "Lib/Sys/W95/Render.hpp"
#endif

#if VER_TIMING_STATS

//*********************************************************************************************
//
class CCycleConvert
//
// Class used by CCycleTimer to convert cycles to seconds.
//
//**************************************
{
private:
	float	fSecondsPerCycle_;			// The result of the calibration.
	int		iCalibrated;				// How many times calibrated.

	// Used to calibrate the timer.
	TCycles cyStart;
	uint32 u4MSStart;

public:
	CCycleConvert()
	{
		bool b = gbUseRDTSC;

		gbUseRDTSC = true;

		// Start dem counters.
		cyStart = cyReadCycles();
		u4MSStart = GetCurrentTime();

		gbUseRDTSC = b;

		iCalibrated = 0;

	#if bSIMPLE_VXD_TEST
		__asm
		{
			xor ecx, ecx
			_emit 0x0F
			_emit 0x33
		}
	#endif
	}

	void UpdateCalibration();

	inline float fSecondsPerCycle()
	{
		// Update calibration the first time, and every so often.
		if (iCalibrated % 64 == 0)
			UpdateCalibration();

		iCalibrated++;

		return fSecondsPerCycle_;
	}
};


void CCycleConvert::UpdateCalibration()
{
	bool b = gbUseRDTSC;

	gbUseRDTSC = true;

	fSecondsPerCycle_ = double(GetCurrentTime() - u4MSStart) * 0.001 /
		                double(cyReadCycles()   - cyStart);

	gbUseRDTSC = b;
}


static CCycleConvert CycleConvert;

//*********************************************************************************************
// globals to control which of the MSRs are used....
// Cannot make static members of a class because the assembler seems to have real trouble
// with them? I will make them static when I figure out how to do it!
//
uint32	gu4MSRProfileTimerSelect = 0;				// event timer we are using
bool	gbMSRProfile = true;						// can we use MSR profiling
bool	gbUseRDTSC = true;							// are we using RDTSC or RDMSR
bool	gbNormalizeStats = true;					// set to true if stat is required in actual time
//*********************************************************************************************
//
// CCycleTimer implementation.
//

	float CCycleTimer::fSecondsPerCycle()
	{
		return CycleConvert.fSecondsPerCycle();
	}

//*********************************************************************************************
//
// CProfileStat implementation.
//

// 'this' : used in base member initializer list
// #pragma warning(disable: 4355)

	//*********************************************************************************************
	CProfileStat::CProfileStat(const char* str, CProfileStat* pps_parent, CSet<EProfileFlag> setepf)
		: strName(str), ppsParent(pps_parent), cyCycles(0), iCount(0), 
		setepfFlags(setepf),
		pvcList(0)
	{
		if (pps_parent)
			// Add to the parent profile's list.
			pps_parent->AddStat(this);
	}

	//*********************************************************************************************
	CProfileStat::~CProfileStat()
	{
		delete pvcList;

		// Remove from parent.
		if (ppsParent)
		{
			Assert(ppsParent->pvcList);

			// Remove from list.
			std::vector<CProfileStat*>::iterator vci = std::find(ppsParent->pvcList->begin(), ppsParent->pvcList->end(), this);
			if (vci != ppsParent->pvcList->end())
				ppsParent->pvcList->erase(vci);
		}
	}

	//*********************************************************************************************
	float CProfileStat::fSeconds() const
	{
		if (gbNormalizeStats)		
			return cyCycles * CCycleTimer::fSecondsPerCycle();
		else
			return cyCycles;
	}

	//*********************************************************************************************
	float CProfileStat::fSecondsPerCount() const
	{
		if (ppsParent && ppsParent->iCount)
			return fSeconds() / ppsParent->iCount;
		else if (iCount)
			return fSeconds() / iCount;
		else
			return 0;
	}

	//*********************************************************************************************
	float CProfileStat::fCountPer() const
	{
		if (ppsParent && ppsParent->iCount)
			return float(iCount) / ppsParent->iCount;
		else
			return iCount;
	}

	//*********************************************************************************************
	void CProfileStat::WriteToBuffer(CStrBuffer& strbuf, int i_indent, bool b_show_hidden)
	{
		Assert(strName);

		if (*strName)
		{
			// Subtract separate children from this stat before printing it.
			TCycles cy_original = cyCycles;
			if (pvcList)
				for (std::vector<CProfileStat*>::iterator it = pvcList->begin(); it != pvcList->end(); it++)
					if ((*it)->setepfFlags[epfSEPARATE])
						cyCycles -= (*it)->cyCycles;

			//
			// Write this stat.
			//

		#if bCOLLAPSE_STATS
			if (cyCycles <= 0 && !iCount && !pvcList)
				// This stat has accumulated nothing so far.
				return;
		#endif // bCOLLAPSE_STATS

			strbuf.Print("%*s%-*s:", i_indent, "", iNAME_WIDTH-i_indent, strName);

			// ms per count.
			if (cyCycles > 0)
			{
				float f_count;


				if (gbNormalizeStats)
				{
					f_count = fSecondsPerCount() * 1000.0;
					strbuf.Print(" %8.1f", f_count);
				}
				else
				{
					f_count = fSecondsPerCount();

					if (f_count>1000.0f)
					{
						strbuf.Print(" %7.1fK", f_count*0.001);
					}
					else
					{
						strbuf.Print(" %8.1f", f_count);
					}
				}
			}
			else
			{
				strbuf.Print(" %8s", "");
			}

			// Percent time, relative to parent.
			if (ppsParent && ppsParent->cyCycles > 0)
				strbuf.Print(" %5.1f", float(cyCycles) / float(ppsParent->cyCycles) * 100.0f);
			else
				strbuf.Print(" %5s", "");

			if (gbNormalizeStats)
			{
				// Count, relative to parent.
				if (iCount)
				{
					float f_count_per = fCountPer();
					if (f_count_per >= 1000.0)
						strbuf.Print(" %7.2fK", f_count_per * 0.001);
					else
						strbuf.Print(" %8.2f", f_count_per);

					// Count/sec
					if (cyCycles)
					{
						float f_cps = iCount / fSeconds();
						if (f_cps >= 1000.0)
							strbuf.Print(" %7.1fK", f_cps * 0.001);
						else
							strbuf.Print(" %8.1f", f_cps);
					}
				}
			}
			strbuf.Print("\n");

			cyCycles = cy_original;
		}

		if (pvcList)
		{
			// Construct "other" stat, print if requested.
			TCycles cy_other = cyCycles;

			// Write non-separate children first, indented.
			std::vector<CProfileStat*>::iterator it;
			for (it = pvcList->begin(); it != pvcList->end(); it++)
			{
				if (b_show_hidden || !(*it)->setepfFlags[epfHIDDEN])
					if (!(*it)->setepfFlags[epfSEPARATE])
						(*it)->WriteToBuffer(strbuf, i_indent+1, b_show_hidden);

				if (!(*it)->setepfFlags[epfOVERLAP])
					cy_other -= (*it)->cyCycles;
			}

			if (setepfFlags[epfOTHER])
			{
				// Print the remainder.
				CProfileStat ps_other("Other", this, Set(epfRELATIVE));
				ps_other.cyCycles = cy_other;

				ps_other.WriteToBuffer(strbuf, i_indent+1, b_show_hidden);
			}

			// Write separate children, not indented.
			for (it = pvcList->begin(); it != pvcList->end(); it++)
				if (b_show_hidden || !(*it)->setepfFlags[epfHIDDEN])
					if ((*it)->setepfFlags[epfSEPARATE])
						(*it)->WriteToBuffer(strbuf, i_indent, b_show_hidden);
		}
	}

	//*********************************************************************************************
	void CProfileStat::WriteToConsole(CConsoleBuffer& con, bool b_show_hidden)
	{
		if (con.bIsActive())
		{
			CStrBuffer strbuf(4000);
			WriteToBuffer(strbuf, 0, b_show_hidden);
			con.PutString(strbuf);
		}
	}

	//*****************************************************************************************
	void CProfileStat::Reset()
	{
		// Reset self and all children.
		cyCycles = 0;
		iCount = 0;

		if (pvcList)
			for (std::vector<CProfileStat*>::iterator it = pvcList->begin(); it != pvcList->end(); it++)
				(*it)->Reset();
	}

	//*********************************************************************************************
	void CProfileStat::AddStat(CProfileStat* pps)
	{
		// Create list first time.
		if (!pvcList)
			pvcList = new std::vector<CProfileStat*>;

		// Add to list.
		pvcList->push_back(pps);

		if (pps->setepfFlags[epfSEPARATE][epfMASTER].bAll())
			pps->ppsParent = 0;
		else if (setepfFlags[epfMASTER] || pps->setepfFlags[epfRELATIVE])
			// Set the stat's parent pointer.
			pps->ppsParent = this;
		else
			// Set the stat's parent pointer to our parent.
			pps->ppsParent = ppsParent;
	}

	//*********************************************************************************************
	void CProfileStat::WriteHeader(CStrBuffer& strbuf)
	{
		if (gbNormalizeStats)
		{
			// Write header.
			strbuf.Print("%-*s: %7s %7s %8s %8s (%.1f)\n", 
				iNAME_WIDTH, "Item",
				"MS",
				"%Tot",
				"Count",
				"Count/s",
				1e-6 / CCycleTimer::fSecondsPerCycle()
			);
		}
		else
		{
			// Write header.
			strbuf.Print("%-*s: %7s %7s\n", 
				iNAME_WIDTH, "Item",
				"Count",
				"%Tot"
			);

		}
	}

	//*********************************************************************************************
	void CProfileStat::WriteHeader(CConsoleBuffer& con)
	{
		if (con.bIsActive())
		{
			CStrBuffer strbuf(4000);
			WriteHeader(strbuf);
			con.PutString(strbuf);
		}
	}

//**********************************************************************************************
//
// CProfileStatMain implementation.
//

	//*********************************************************************************************
	CProfileStatMain::CProfileStatMain(const char* str)
		: CProfileStat(str, 0, Set(epfMASTER))
	{
	}

	//*********************************************************************************************
	void CProfileStatMain::WriteToBuffer(CStrBuffer& strbuf, int i_indent, bool b_show_hidden)
	{
		WriteHeader(strbuf);

		// Write rest.
		CProfileStat::WriteToBuffer(strbuf, i_indent, b_show_hidden);
	}

//**********************************************************************************************
//
// SProfileMain implementation.
//

	//******************************************************************************************
	SProfileMain::SProfileMain()
		:
		//
		// Initialise the stats with their names.  Those that have no name don't get shown.
		//
		psFrame			("Frame", &psMain, Set(epfMASTER) + epfOTHER),
			psStep			("Step", &psFrame, Set(epfOTHER)),
				psAI			("AI Step", &psStep),
				psPhysics		("Physics Step", &psStep),
			psOcclusion		("Occlusion", &psFrame),
			psTerrainUpdate	("Terrain Upd.", &psFrame),
			psRender		("Render", &psFrame, Set(epfOTHER)),
				psRenderShape	("RenderShape", &psRender),
				psPresort		("Presort", &psRender),
				psDrawPolygon	("DrawPolygon", &psRender),
//				psBeginFrame	("Begin Frame", &psRender),
				psClearScreen	("Clear Screen", &psRender),
//			psEndFrame		("End Frame", &psFrame),
//			psFlip			("Flip", &psEndFrame, Set(epfSEPARATE)),
			psFlip			("Flip", &psFrame),
			psText			("Text", &psFrame, Set(epfSEPARATE)),
			psOverlayText	("Overlay text", &psFrame)
			
//	psTextures				("Textures", &psMain),
//	psTextureKB				("Texture KB", &psMain)


//		psDynamicYes	("dynamic_cast true", &psMain),
//		psDynamicNo		("dynamic_cast false", &psMain)
	{
/*
		// Hacky test of dynamic_cast<> speed.
		const int iCOUNT = 100;
		CProfileStat* pps = new CProfileStat();

		CCycleTimer ctmr;
		for (int i = 0; i < iCOUNT*1000; i++)
		{
			dynamic_cast<CProfileStat*>(pps);
		}
		psDynamicYes.Add(ctmr(), iCOUNT);

		for (i = 0; i < iCOUNT*1000; i++)
		{
			dynamic_cast<CProfileStatExclusive*>(pps);
		}
		psDynamicNo.Add(ctmr(), iCOUNT);
*/
	}

	//******************************************************************************************
	SProfileHardware::SProfileHardware()
		: psHardwareFrame("Hardware Stats", &psHardware, Set(epfMASTER)),
		  psUploads ("Uploads", &psHardware)
	{
	}

// #if VER_TIMING_STATS
#else
		// GUIApp uses this guy even in a final build, so we'll include it and disable MSR.
bool	gbMSRProfile = false;						// can we use MSR profiling
#endif


// Ensure proProfile is initialised early.
#pragma warning(disable:4073)
#pragma init_seg(lib)

SProfileMain proProfile;



#if bUSE_SPECIAL_PROFILE
	int iFrame = 0;
#endif
	

//*********************************************************************************************
//
// CSpecialProfile implementation.
//

	//*****************************************************************************************
	//
	// CSpecialProfile member functions.
	//

	//*****************************************************************************************
	void CSpecialProfile::StartAll()
	{
	#if bUSE_SPECIAL_PROFILE

		// Reset the all counters.
		if (iFrame++ >= 10)
		{
			//Reset();
		}
		
		// Get the current cycle count.
		cyLastAll = cyReadCycles();

	#endif
	}

	//*****************************************************************************************
	void CSpecialProfile::StopAll()
	{
	#if bUSE_SPECIAL_PROFILE

		// Do nothing if nothing has been done.
		if (iCount <= 0)
			return;

		// Get the current cycle count.
		cyAll += cyReadCycles() - cyLastAll;

	#endif
	}

	//*****************************************************************************************
	void CSpecialProfile::WriteToMainWindow() const
	{
	#if bUSE_SPECIAL_PROFILE

		char str_out[256];	// Output string.

		//
		// Calculate cycles per special item and the percentage of the frame time spent on the
		// special item.
		//

		int i_count = iCount;

		if (i_count <= 0)
			i_count = 1;

		double d_cycles_per_item = double(int64(cySpecial)) / double(i_count);
		double d_percent         = double(int64(cySpecial)) / double(int64(cyAll)) * 100.0f;

		// Write the string.
		sprintf(str_out, "%1.1f : %1.3f", (float)d_cycles_per_item, (float)d_percent);

		// Unlock the main screen and get the handle for its device context.
		prasMainScreen->Unlock();
		HDC hdc          = prasMainScreen->hdcGet();
		COLORREF colref  = SetTextColor(hdc, RGB(255, 255, 0));
		int i_old_bkmode = SetBkMode(hdc, TRANSPARENT);

		// Write the text to the main screen.
		TextOut
		(
			hdc,
			5,
			5,
			str_out,
			strlen(str_out)
		);
		
		// Release the device context for the screen.
		SetBkMode(hdc, i_old_bkmode);
		SetTextColor(hdc, colref);
		prasMainScreen->ReleaseDC(hdc);

	#endif
	}


//*********************************************************************************************
//
// Externally defined global variables.
//

SProfileHardware proHardware;
CSpecialProfile specProfile;
