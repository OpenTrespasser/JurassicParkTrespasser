/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of 'Fop.hpp.'
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Fog.cpp                                                  $
 * 
 * 42    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 41    98/09/16 12:15 Speter
 * Removed a redundant check.
 * 
 * 40    98.09.13 7:04p Mmouni
 * Changed the ApplyTerrainFog() method.
 * 
 * 39    8/26/98 5:50p Mmouni
 * If the clipping plane is adjusted to be inside the fog half_y the fog is now adjust to some
 * hard coded values.
 * 
 * 38    98.06.26 12:33p Mmouni
 * Removed some junk that was left in.
 * 
 * 37    98.06.25 8:46p Mmouni
 * Added stuff to support render quality setting.
 * 
 * 36    98/06/18 11:35 Speter
 * Fixed terrain fog clipping bug.
 * 
 * 35    98/01/15 14:14 Speter
 * Moved ApplyTerrainFog here from Pipeline.cpp.  Made ApplyFirst and ApplyLast private.
 * 
 * 34    97.12.08 3:52p Mmouni
 * Removed unused variables/code.
 * 
 * 33    11/16/97 2:34p Sblack
 * Start first fog band at 1/4 instead of 2/10.
 * 
 * 32    97.11.14 11:39a Mmouni
 * Made changes to support smooth foggin into the first fog band.
 * 
 * 31    97/11/13 1:02a Pkeet
 * Added code to cut off last fog band.
 * 
 * 30    11/12/97 7:17a Shernd
 * Added the ability to use flags to not have output files used
 * 
 * 29    97/11/07 10:42a Pkeet
 * Added the 'ApplyLast' member function.
 * 
 * 28    97/11/06 6:31p Pkeet
 * Added the 'ApplyFirst' member function. Setup code initializes the 'fFogTerrainThreshold' and
 * 'fInvFogTerrainFirstBandRange' parameters. Modified the 'bGetFogLevels' to return a boolean
 * value set to 'true' if the polygon is entirely below the terrain threshold value.
 * 
 * 27    97/11/06 4:52p Pkeet
 * Changed maximum allowed value for the floating point fog value.
 * 
 * 26    97/11/05 7:32p Pkeet
 * Implemented the 'SetFogLevelsFloat' member function.
 * 
 * 25    97/11/03 5:52p Pkeet
 * Added the 'fogTerrainFog' global variable.
 * 
 * 24    97/10/06 11:01 Speter
 * Moved fogFog here from Render.hpp.
 * 
 * 23    97/07/23 18:05 Speter
 * Replaced CCamera argument to clipping functions with b_perspective flag.  
 * 
 * 22    97/06/10 15:46 Speter
 * Now takes camera as an argument (for new vertex projection).
 * 
 * 21    97/05/13 13:40 Speter
 * Renamed SRenderTriangle::seterfTri to seterfFace.
 * 
 * 20    97-04-21 17:10 Speter
 * Changed interface of Apply function, to use new CPipelineHeap.  Rewrote Apply() to use new
 * CClipPlane splitting function.
 * 
 * 19    97-04-04 12:38 Speter
 * A big change: now pipeline uses CRenderPolygon rather than SRenderTriangle (currently always
 * as a triangle).  Changed associated variable names and comments.
 * 
 * 18    97/02/03 11:00 Speter
 * Renamed dartTriangles to dartFogTriangles to avoid confusion with Pipeline variable.
 * 
 * 17    12/19/96 1:23p Mlange
 * Removed some redundant functions. Updated the todo list. Updated the fog class interface. It
 * now uses a property structure. Re-implemented the various fog functions. Removed the inverse
 * fog function.
 * 
 * 16    12/14/96 6:25p Mlange
 * Implemented new fog method.
 * 
 * 15    12/14/96 3:19p Pkeet
 * Set 'iPRINT_FOG_TABLE' to 0.
 * 
 * 14    12/09/96 1:41p Mlange
 * Updated for changes to the CCamera interface.
 * 
 * 13    96/12/06 15:09 Speter
 * Removed unneeded pras_screen argument from ConstructTable().
 * 
 * 12    11/27/96 4:55p Pkeet
 * Added extended print functions for the fog table.
 * 
 * 11    11/23/96 5:33p Mlange
 * Made the camera pointer in ConstructTable() const.
 * 
 * 10    96/11/21 18:57 Speter
 * Changed fog table to use hard-coded size, rather than calculating from Z clipping values.
 * 
 * 9     96/10/22 11:27 Speter
 * Changed CPresenceCamera to CCamera, prscam prefix to cam.
 * 
 * 8     96/10/22 11:11 Speter
 * Changed CEntity to CPresence in appropriate contexts.
 * 
 * 7     9/26/96 1:54p Pkeet
 * Changed default fog values.
 * 
 * 6     9/25/96 8:57p Pkeet
 * Made linear fog building the default.
 * 
 * 5     96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 4     96/09/25 15:39 Speter
 * Added efogFunction field to specify function for fog table.
 * Changed fK field to fZMaxDist.
 * Added iShiftFogUp field, removed iSizeofDestPixel.
 * Made u4GetFog() return fog index.  Added u4GetClutOffset to return clut offset for fog index.
 * Implemented bConstantFog function.
 * Changed f_inv_max_depth value to be CCamera::fInvZAdjust.
 * 
 * 3     8/22/96 4:29p Pkeet
 * Made the fog table build according to values for ramp and depth in the main clut.
 * 
 * 2     8/20/96 4:45p Pkeet
 * Initial implementation of fogging.
 * 
 * 1     8/19/96 10:19a Pkeet
 * 
 **********************************************************************************************/

// If this defines a value, print out the most recently created fog table.
#define bPRINT_FOG_TABLE	1

//
// Includes.
//
#include "Common.hpp"
#include "Fog.hpp"

#include "ScreenRender.hpp"
#include "Clip.hpp"
#include "Lib/GeomDBase/PlaneAxis.hpp"
#include "Camera.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"

#if bPRINT_FOG_TABLE
	#include <fstream.h>
	#include <iomanip.h>

	ofstream stream;
#endif

extern bool        bUseOutputFiles;

#define rMIN_FOG_BAND_WIDTH		TReal(.005)		// Minimum width of a fog band.

//*********************************************************************************************
//
// CFog implemenation.
//

	//******************************************************************************************
	void CFog::SetProperties(const CFog::SProperties& fogprop)
	{
		// Set the new properties.
		fogpropCurrent = fogprop;

		// Compute adjusted fog values based on adjusted far clip.
		TReal rAdjustedHalfFogY = fogpropCurrent.rHalfFogY * fogpropCurrent.rHalfFogYAdjusment;
		TReal rAdjustedPower = fogpropCurrent.rPower;

		// Check if we have pulled the clipping plane in past the middle of the fog.
		if (rAdjustedHalfFogY > 1.0f)
		{
			// If so, use these hard coded settings.
			rAdjustedHalfFogY = 0.8f;
			rAdjustedPower = 5.0f;
		}

		#if bPRINT_FOG_TABLE
            
            if (bUseOutputFiles)
            {
			    stream.open("fog.txt");

			    stream << setiosflags(ios::left) << setprecision(5);
 			    stream << "Fog Creation...\n\n" << "Level to Y\n";
            }
		#endif

		int i_curr_index = 0;
		TReal r_last_fog_end_y = 0;

		for (int i_fog_level = 0; i_fog_level < (int)parLevelToY.uLen; i_fog_level++)
		{
			//
			// First, calculate the end y value for the current fog level. (Actually,
			// the start y value of the next fog level.)
			//

			// Get the next fog level normalised for the range: [0,1].
			TReal r_fog_level = TReal(i_fog_level + 1) / parLevelToY.uLen;

			Assert(bWithin(r_fog_level, 0, 1));

			TReal r_fog_end_y;

			switch (fogpropCurrent.efogFunction)
			{
				case efogLINEAR :
				{
					//
					// The fog level is linear with y raised to some power:
					//
					//		       p             1/p
					//		fog = y   =>  y = fog
					//
					// We want the fog level to be half of maximum at some distance h_y, thus we need some
					// constant k:
					//
					//		       1/p                    1/p                     1/p
					//		y = fog    * k, with: h_y = .5    * k  =>  k = h_y * 2
					//
					float f_k = rAdjustedHalfFogY * pow(2.0, 1.0 / rAdjustedPower);

					r_fog_end_y = pow(r_fog_level, 1 / rAdjustedPower) * f_k;
					break;
				}

				case efogEXPONENTIAL :
				{
					//
					// The fog level calculated with the exponential decay function:
					//
					//		             -ky
					//		fog = A(1 - e   ) (in our case: A = 1)  =>  y = ln(-fog + 1) / -k
					//
					// We want the fog level to be half of maximum at some distance h_y, thus the
					// constant k is calculated with:
					//
					//           -kh_y
					//		1 - e      = .5  =>  k = -ln(.5) / h_y
					//
					float f_k = -log(.5) / rAdjustedHalfFogY;

					if (r_fog_level < 1 - FLT_EPSILON * 10)
						r_fog_end_y = log(-r_fog_level + 1) / -f_k;
					else
						r_fog_end_y = FLT_MAX;

					break;
				}

				default :
				{
					Assert(false);
					break;
				}
			}


			#if bPRINT_FOG_TABLE
                if (bUseOutputFiles)
                {
				    stream << setw(4) << i_fog_level << " -> " << setw(5) << r_fog_end_y << "  ";
                }
			#endif

			// Ensure the fog band has the minimum witdh.
			if (r_fog_end_y < r_last_fog_end_y + rMIN_FOG_BAND_WIDTH)
				r_fog_end_y = r_last_fog_end_y + rMIN_FOG_BAND_WIDTH;

			r_last_fog_end_y = r_fog_end_y;

			// We want maximum fog to extend to the far clipping plane, so if this is the last fog level,
			// force its y distance to be beyond the far clipping plane. Also, if this fog band ends
			// after or sufficiently close to the far clipping plane, extend it to be beyond it as well to
			// avoid unnessary clipping operations.
			if (i_fog_level == (int)parLevelToY.uLen - 1 || r_fog_end_y + rMIN_FOG_BAND_WIDTH / 2 > 1)
				r_fog_end_y = FLT_MAX;


			// Now fill in the y to fog level conversion table.
			for (; i_curr_index < (int)paiYToLevel.uLen; i_curr_index++)
			{
				// Convert the table index to a y value in the range [0,1].
				TReal r_y = (TReal)i_curr_index / (TReal)paiYToLevel.uLen;

				// Fill the table with the current fog level until the y value is greater than the y value
				// of the end of the fog band.
				paiYToLevel[i_curr_index] = i_fog_level;

				if (r_y >= r_fog_end_y)
				{
					// Now set the end y value of the current fog band to the y value calculated for this
					// table index. This way, the two conversion tables will match.
					r_fog_end_y = r_y;
					break;
				}
			}

			#if bPRINT_FOG_TABLE
                if (bUseOutputFiles)
                {
				    stream << r_fog_end_y << endl;
                }
			#endif

			parLevelToY[i_fog_level] = r_fog_end_y;
		}

		// Ensure the entire table is initialised.
		Assert(i_curr_index == paiYToLevel.uLen);
		Assert(pafYToLevel.uLen == paiYToLevel.uLen);

		// Initialize the floating point table.
		SetFogLevelsFloat();

		#if bPRINT_FOG_TABLE
            if (bUseOutputFiles)
            {
			    stream << "\n\n\nY to level\n";
            }

			for (i_curr_index = 0; i_curr_index < (int)paiYToLevel.uLen; i_curr_index++)
			{
				TReal r_range_start = (TReal)i_curr_index       / (TReal)paiYToLevel.uLen;
				TReal r_range_end   = (TReal)(i_curr_index + 1) / (TReal)paiYToLevel.uLen;

                if (bUseOutputFiles)
                {
				    stream << setw(4) << i_curr_index << " [" << setw(9) << r_range_start << ", " << setw(9) << r_range_end << ")";
				    stream << " -> " << paiYToLevel[i_curr_index] << ", " << pafYToLevel[i_curr_index] << endl;
                }
			}

            if (bUseOutputFiles)
            {
			    stream.close();
            }
		#endif
	}
	
	//******************************************************************************************
	//
	void CFog::SetQualityAdjustment
	(
		float f_actual_farclip,		// Actual far clipping value.
		float f_desired_farclip		// Far clipping value before adjustment.
	)
	//
	// Update the properties based on quality adjustment to the camera.
	//
	//**************************************
	{
		TReal rHalfFogYAdjusment = f_desired_farclip / f_actual_farclip;

		// Avoid doing anything if we can.
		if (rHalfFogYAdjusment != fogpropCurrent.rHalfFogYAdjusment)
		{
			fogpropCurrent.rHalfFogYAdjusment = f_desired_farclip / f_actual_farclip;
			SetProperties(fogpropCurrent);
		}
	}

	//*****************************************************************************************
	void CFog::SetFogLevelsFloat()
	{
		float f_k;
		float f_inv_k;
		float f_num_bands = float(iNumBands);
		float f_fog_max   = float(iNumBands) - 1.001f;

		// Compute adjusted fog values based on adjusted far clip.
		TReal rAdjustedHalfFogY = fogpropCurrent.rHalfFogY * fogpropCurrent.rHalfFogYAdjusment;
		TReal rAdjustedPower = fogpropCurrent.rPower;

		// Check if we have pulled the clipping plane in past the middle of the fog.
		if (rAdjustedHalfFogY > 1.0f)
		{
			// If so, use these hard coded settings.
			rAdjustedHalfFogY = 0.8f;
			rAdjustedPower = 5.0f;
		}

		// Set the fog half power constant.
		switch (fogpropCurrent.efogFunction)
		{
			case efogLINEAR:
			{
				f_inv_k = 1.0f / (rAdjustedHalfFogY * pow(2.0, 1.0 / rAdjustedPower));
				break;
			}
			case efogEXPONENTIAL:
			{
				f_k = log(0.5) / rAdjustedHalfFogY;
				break;
			}
			default:
			{
				Assert(false);
				break;
			}
		}

		fFogTerrainThreshold = 0.0f;

		for (int i_curr_index = 0; i_curr_index < (int)pafYToLevel.uLen; i_curr_index++)
		{
			TReal r_fog;	// Storage for the fog value.

			// Calculate the normalized 'y' value.
			TReal r_y = TReal(i_curr_index) / TReal(pafYToLevel.uLen);

			switch (fogpropCurrent.efogFunction)
			{
				case efogLINEAR:
				{
					//
					// The fog level is linear with y raised to some power:
					//
					//		        1/p                       p
					//		 y = fog   * k  qed  fog = (y / k)
					//
					r_fog = pow(r_y * f_inv_k, rAdjustedPower) * f_num_bands;
					break;
				}

				case efogEXPONENTIAL:
				{
					//
					// The fog level calculated with the exponential decay function:
					//
					//		             -ky
					//		fog = A(1 - e   ) where A = 1
					//
					r_fog = (1.0f - exp(r_y * f_k)) * f_num_bands;
					break;
				}
			}

			SetMinMax(r_fog, 0.0f, f_fog_max);

			if (r_fog > 0.25f && fFogTerrainThreshold == 0.0f)
			{
				// Set the threshold to near clip at to the Y at which the fog value is > 1/5.
				fFogTerrainThreshold = r_y;
			}

			pafYToLevel[i_curr_index] = r_fog;
		}

		// Set the threshold to far clip at.
		fFogLastBand = parLevelToY[iNumBands - 2];
	}


//*********************************************************************************************
//
// Main fog table.
//

CFog fogFog;
CFog fogTerrainFog(iNUM_TERRAIN_FOG_BANDS);

