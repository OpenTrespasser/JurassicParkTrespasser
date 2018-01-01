/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		A class for using fogging in raster primitives.
 *
 * Bugs:
 *		Currently, fog only depends on the y coordinate of the distance from the camera. This is
 *		obviously wrong, since it must depend on the true distance from the camera.
 *
 * To Do:
 *		The fog band end y calculation must take the position of the near clipping plane into
 *		account as well. I.e. if the end of a fog band is behind or sufficiently close to the
 *		near clipping plane, it should be moved far behind it. (So the current camera must be
 *		passed to the SetProperties() function to find the position of the near clipping plane.
 *		This creates an uncomfortable interdependancy: what if the near clipping plane moves...
 *		the fog tables need to be recalculated!)
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Fog.hpp                                                  $
 * 
 * 33    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 32    98.09.13 7:03p Mmouni
 * Tweaked the fGetFogLevel() method.
 * 
 * 31    98.06.25 8:46p Mmouni
 * Added stuff to support render quality setting.
 * 
 * 30    3/18/98 6:46p Pkeet
 * Changed default settings as per Terry's request.
 * 
 * 29    98/01/15 14:14 Speter
 * Moved ApplyTerrainFog here from Pipeline.cpp.  Made ApplyFirst and ApplyLast private.
 * 
 * 28    11/16/97 2:33p Sblack
 * Made fGetFogLevel interpolate.
 * 
 * 27    97.11.14 11:37a Mmouni
 * Changes for smooth fogging from into the first fog band.
 * 
 * 26    97/11/13 1:02a Pkeet
 * Added code to cut off last fog band.
 * 
 * 25    97/11/07 12:06p Pkeet
 * Added the 'fNoSortThreshold' member function.
 * 
 * 24    97/11/07 10:42a Pkeet
 * Added the 'ApplyLast' member function.
 * 
 * 23    97/11/06 6:30p Pkeet
 * Added the 'fFogTerrainThreshold' and 'fInvFogTerrainFirstBandRange' parameters. Added the
 * 'ApplyFirst' member function. Modified the 'fGetFogLevel' function to normalize values
 * falling after the terrain threshold value but before the first fog band. Modified the
 * 'bGetFogLevels' to return a boolean value set to 'true' if the polygon is entirely below the
 * terrain threshold value.
 * 
 * 22    97/11/05 7:32p Pkeet
 * Added the 'pafYToLevel' array. Added the 'fGetFogLevel' and 'SetFogLevelsFloat' member
 * functions. Increased the fog table to 1024.
 * 
 * 21    97/11/05 2:48p Pkeet
 * Added 8 band alpha fog for terrain.
 * 
 * 20    97/11/03 5:52p Pkeet
 * Added the 'fogTerrainFog' global variable.
 * 
 * 19    97/08/17 18:26 Speter
 * Changed int cast to iPosFloatCast().
 * 
 * 18    8/16/97 9:56p Bbell
 * Changed defaults.
 * 
 * 17    97/07/30 11:48a Pkeet
 * Made the 'GetFogLevels' member function public.
 * 
 * 16    97/07/23 18:05 Speter
 * Replaced CCamera argument to clipping functions with b_perspective flag.  
 * 
 * 15    97/06/10 21:51 Speter
 * Changed fog default to exponential.
 * 
 * 14    97/06/10 15:46 Speter
 * Now takes camera as an argument (for new vertex projection).
 * 
 * 13    97-04-21 17:09 Speter
 * Changed interface of Apply function, to use new CPipelineHeap.
 * 
 * 12    97-04-04 12:38 Speter
 * A big change: now pipeline uses CRenderPolygon rather than SRenderTriangle (currently always
 * as a triangle).  Changed associated variable names and comments.
 * 
 * 11    1/08/97 7:10p Pkeet
 * Removed scope information from parameter type to avoid an error with Visual C++ 5.0.
 * 
 * 10    12/20/96 12:10p Mlange
 * Moved fog source files to 'Renderer' directory.
 * 
 * 9     12/19/96 1:23p Mlange
 * Removed some redundant functions. Updated the todo list. Updated the fog class interface. It
 * now uses a property structure. Re-implemented the various fog functions. Removed the inverse
 * fog function.
 * 
 * 8     12/14/96 6:19p Mlange
 * Implemented a new method for fogging. Triangles are now partitioned along fog band boundaries
 * so that each part lies in a single fog band.
 * 
 * 7     96/12/06 15:09 Speter
 * Removed unneeded pras_screen argument from ConstructTable().
 * 
 * 6     11/23/96 5:33p Mlange
 * Made the camera pointer in ConstructTable() const.
 * 
 * 5     96/10/22 11:27 Speter
 * Changed CPresenceCamera to CCamera, prscam prefix to cam.
 * 
 * 4     96/10/22 11:11 Speter
 * Changed CEntity to CPresence in appropriate contexts.
 * 
 * 3     96/09/25 15:39 Speter
 * Added efogFunction field to specify function for fog table.
 * Changed fK field to fZMaxDist.
 * Added iShiftFogUp field, removed iSizeofDestPixel.
 * Made u4GetFog() return fog index.  Added u4GetClutOffset to return clut offset for fog index.
 * Implemented bConstantFog function.
 * Changed f_inv_max_depth value to be CCamera::fInvZAdjust.
 * 
 * 2     8/20/96 4:45p Pkeet
 * Initial implementation of fogging.
 * 
 * 1     8/19/96 10:21a Pkeet
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_FOG_HPP
#define HEADER_LIB_RENDERER_FOG_HPP

#include "Lib/Transform/Vector.hpp"
#include "Lib/View/Clut.hpp"
#include "Lib/Renderer/Clip.hpp"
#include "Lib/GeomDBase/PlaneAxis.hpp"


//*********************************************************************************************
//
// Definitions for CFog.
//

// Size of the y value to fog level conversion table.
#define iY_TO_FOG_TABLE_SIZE   TReal(1024)

// Number of fog bands for terrain.
#define iNUM_TERRAIN_FOG_BANDS (8)

// Threshold band for no sorting.
#define iNO_SORT_FOG_BAND      (2)

class CCamera;
class CRenderPolygon;
class CPipelineHeap;

//*********************************************************************************************
//
class CFog
//
// Prefix: fog
//
// Defines fogging information for rasterizing.
//
//**************************************
{
public:
	enum EFogFunction
	// Describes the various fog functions that are available.
	// Prefix: efog
	{
		efogLINEAR,
		efogEXPONENTIAL
	};

	//******************************************************************************************
	//
	struct SProperties
	//
	// Structure for holding the fog properties.
	//
	// Prefix: fogprop
	//
	//**************************************
	{
		EFogFunction efogFunction;	// The fog function to use.

		TReal rHalfFogY;			// Distance from camera (in normalised camera space) at which fog level is half of maximum.
		TReal rPower;				// Power value for the linear fog function.
		TReal rHalfFogYAdjusment;	// Adjustment based on camera adjustment.
	
		//**************************************************************************************
		//
		// Constructors.
		//
		SProperties()
			: efogFunction(efogLINEAR), rHalfFogY(0.28), rPower(1.20), rHalfFogYAdjusment(1.0)
		{
		};

		SProperties(EFogFunction efog, TReal r_half_fog_y, TReal r_power)
			: efogFunction(efog), rHalfFogY(r_half_fog_y), rPower(r_power), rHalfFogYAdjusment(1.0)
		{
			Assert(rHalfFogY > 0);
			Assert(rPower    > 0);
		};
	};

	// Where to clip to end per-pixel fogging and begin flat shading.
	float          fFogLastBand;

private:
	SProperties fogpropCurrent;		// Current properties of this fog.

	CPArray<TReal> parLevelToY;		// Table that converts a fog level to a distance that defines the end of that fog band.
	CPArray<int>   paiYToLevel;		// Table that converts a distance to a fog level (integer).
	CPArray<float> pafYToLevel;		// Table that converts a distance to a fog level (float).
	int            iNumBands;		// Number of fog bands.

	// Where to clip to start per-pixel fogging.
	float          fFogTerrainThreshold;

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CFog(int i_num_bands = iDefNumDepthValues)
		: paiYToLevel(iY_TO_FOG_TABLE_SIZE), pafYToLevel(iY_TO_FOG_TABLE_SIZE),
		  parLevelToY(i_num_bands), iNumBands(i_num_bands)
	{
		SetProperties(CFog::SProperties());
	}

	// Initialiser constructor.
	CFog(const SProperties& fogprop, int i_num_bands = iDefNumDepthValues)
		: paiYToLevel(iY_TO_FOG_TABLE_SIZE), pafYToLevel(iY_TO_FOG_TABLE_SIZE),
		  parLevelToY(i_num_bands), iNumBands(i_num_bands)
	{
		SetProperties(fogprop);
	}

	// Copy constructor.
	CFog(const CFog& fog)
	{
		// Not implemented yet!!
		Assert(false);
	}

	// Destructor.
	~CFog()
	{
		// Delete the lookup tables.
		delete[] parLevelToY.atArray;
		delete[] paiYToLevel.atArray;
		delete[] pafYToLevel.atArray;
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SetProperties
	(
		const CFog::SProperties& fogprop		// New properties for this fog.
	);
	//
	// Update the properties of this fog.
	//
	//**************************************

	//******************************************************************************************
	//
	void SetQualityAdjustment
	(
		float f_actual_farclip,		// Actual far clipping value.
		float f_desired_farclip		// Far clipping value before adjustment.
	);
	//
	// Update the properties based on quality adjustment to the camera.
	//
	//**************************************

	//******************************************************************************************
	//
	const CFog::SProperties& fogpropGetProperties
	(
	) const
	//
	// Obtain the current properties of the fog.
	//
	// Returns:
	//		The current properties of the fog.
	//
	//**************************************
	{
		return fogpropCurrent;
	}


	//*****************************************************************************************
	//
	void Apply
	(
		CRenderPolygon& rpoly,					// Polygon requiring fogging.
		CPipelineHeap& plh,						// Heap on which to add polygons and vertices.
		bool b_perspective						// Whether screen coords need perspective 
												// interpolation.
	);
	//
	// Apply fogging to a polygon, possibly splitting it, and place all polygons on the heap array.
	//
	// Notes:
	//		If the polygon crosses one or more fog bands (and fog shading is enabled) it is
	//		paritioned along the fog band boundaries into several new polygons. Each of these
	//		new polygons has a constant fog value.
	//
	//		If fogging is disabled, the resulting polygon has a zero fog band index.
	//
	//**************************************

	//**********************************************************************************************
	//
	void ApplyTerrainFog
	(
		CRenderPolygon& rpoly,
		CPipelineHeap&  plh,
		bool            b_perspective
	);
	//
	// Applies terrain fogging to a terrain polygon.
	//
	//**********************************

	//*****************************************************************************************
	//
	forceinline int iGetFogLevel
	(
		TReal r_y		// The y coordinate to find the fog level for.
	)
	//
	// Returns:
	//		The fog band the given y coordinate falls in.
	//
	//**************************************
	{
		int i_index = iPosFloatCast(r_y * paiYToLevel.uLen);

		// SetMinMax(i_index, 0, paiYToLevel.uLen - 1)
		if (i_index < 0)
			i_index = 0;
		else if (i_index > paiYToLevel.uLen - 1)
			i_index = paiYToLevel.uLen - 1;

		int i_level = paiYToLevel[i_index];

		Assert(bWithin(i_level, 0, iNumBands - 1));

		return i_level;
	}


	//*****************************************************************************************
	//
	forceinline float fGetFogLevel
	(
		TReal r_y		// The y coordinate to find the fog level for.
	)
	//
	// Returns:
	//		The fog band the given y coordinate falls in.
	//
	//**************************************
	{
		// Compute floating point index.
		float f_index = r_y * pafYToLevel.uLen;
		
		// No negative values.
		if (f_index < 0.0f)
			f_index = 0.0f;

		// Integer truncated version.
		int i_index = iPosFloatCast(f_index);

		if (i_index >= pafYToLevel.uLen - 1)
		{
			// We are at the end of the table, don't interpolate.
			return pafYToLevel[pafYToLevel.uLen-1];
		}
		else
		{
			// Fractional remainder.
			float t = f_index - (float)i_index;

			// Interpolate between levels.
			return pafYToLevel[i_index] + t*(pafYToLevel[i_index+1] - pafYToLevel[i_index]);
		}
	}


	//*****************************************************************************************
	//
	float fNoSortThreshold
	(
	) const
	//
	// Returns the 'y' value in normalized camera space beyond which polygons are not sorted.
	//
	//**************************************
	{
		return parLevelToY[iNumBands - iNO_SORT_FOG_BAND];
	}

private:

	//*****************************************************************************************
	//
	bool bGetFogLevels
	(
		CRenderPolygon& rpoly,	// Polygon to apply fogging to.
		int& i_fog_level_near,	// Return types for near and far fog levels
		int& i_fog_level_far
	);
	//
	// Obtain the fog band(s) the given triangle lies in.
	//
	// Returns 'true' if the polygon is below the terrain threshold.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetFogLevelsFloat
	(
	);
	//
	// Sets the fog levels in 'pafYToLevel.'
	//
	//**************************************

};


//*********************************************************************************************
//
// Fogging functions inlined for speed.
//

	//*********************************************************************************************
	forceinline bool CFog::bGetFogLevels
	(
		CRenderPolygon& rpoly,
		int& ri_fog_level_near,
		int& ri_fog_level_far
	)
	{
		// Only if fogging is enabled, calculate the fog bands of the closest and furthest vertex.
		TReal r_near, r_far;

		r_near = r_far = rpoly.paprvPolyVertices[0]->v3Cam.tY;

		for (int i = 1; i < rpoly.paprvPolyVertices.uLen; i++)
		{
			SetMin(r_near, rpoly.paprvPolyVertices[i]->v3Cam.tY);
			SetMax(r_far,  rpoly.paprvPolyVertices[i]->v3Cam.tY);
		}

		if (rpoly.seterfFace[erfFOG_SHADE])
		{
			ri_fog_level_near = iGetFogLevel(r_near);
			ri_fog_level_far  = iGetFogLevel(r_far);
		}
		else
		{
			// If fog shading is disabled, calculate the fog band for the average the y coordinates.
			ri_fog_level_near = iGetFogLevel((r_near + r_far) * 0.5f);
			ri_fog_level_far  = ri_fog_level_near;
		}
		return r_far <= fFogTerrainThreshold;
	}

	//*********************************************************************************************
	forceinline void CFog::Apply(CRenderPolygon& rpoly, CPipelineHeap& plh, bool b_perspective)
	{
		// If the polygon crosses one or more fog bands, it needs to be partitioned. Otherwise,
		// apply the fog level to the whole polygon.
		if (!rpoly.seterfFace[erfFOG])
		{
			rpoly.iFogBand = 0;
			return;
		}

		int i_fog_near, i_fog_far;

		bGetFogLevels(rpoly, i_fog_near, i_fog_far);
		Assert(i_fog_near <= i_fog_far);

		for (;;)
		{
			rpoly.iFogBand = i_fog_near;

			if (i_fog_near == i_fog_far)
				break;

			//
			// Construct a plane for splitting the polygon at this boundary.
			// The splitting plane leaves the input polygon on the inside of the plane.
			// Since we want to use the same polygon as we traverse the planes from near to far, 
			// we make the plane pointing in the near direction (-Y).
			//
			CClipPlaneT<CPlaneDefNegY> clp_fog(parLevelToY[i_fog_near]);

			// We check the return value, because the clipper may fail to split the
			// polygon if it barely crosses the plane.
			ESideOf esf = clp_fog.esfSplitPolygon(rpoly, plh, b_perspective);
			if (esf == esfOUTSIDE)
				// Polygon is almost entirely outside (to near side) of plane.  We're done.
				break;

			// Otherwise, a new polygon was possibly created on the outside of the plane,
			// and the remaining polygon is on the inside, ready for the next band.

			i_fog_near++;
		}
	}

	//**********************************************************************************************
	forceinline void CFog::ApplyTerrainFog(CRenderPolygon& rpoly, CPipelineHeap& plh, bool b_perspective)
	{
		if (!rpoly.seterfFace[erfFOG])
		{
			rpoly.iFogBand = 0;
			return;
		}

		// If the polygon is inside the threshold for the first fog band, do nothing.
		// Only if fogging is enabled, calculate the fog bands of the closest and furthest vertex.
		TReal r_near, r_far;

		r_near = r_far = rpoly.paprvPolyVertices[0]->v3Cam.tY;

		for (int i = 1; i < rpoly.paprvPolyVertices.uLen; i++)
		{
			SetMin(r_near, rpoly.paprvPolyVertices[i]->v3Cam.tY);
			SetMax(r_far,  rpoly.paprvPolyVertices[i]->v3Cam.tY);
		}

		if (r_far <= fFogTerrainThreshold)
		{
			// No need to fog.
			rpoly.iFogBand = 0;
			return;
		}
		else if (iGetFogLevel(r_near) >= iNumBands-1)
		{
			// All in the last fog band.
			rpoly.iFogBand = 1;
			for (uint u_vertex = 0; u_vertex < rpoly.paprvPolyVertices.uLen; ++u_vertex)
			{
				SRenderVertex* prv = rpoly.paprvPolyVertices[u_vertex];
				prv->cvIntensity = iNumBands-1;
			}
			return;
		}
		else if (r_near >= fFogTerrainThreshold)
		{
			// No need to clip.
			rpoly.iFogBand = 1;
		}
		else
		{
			// Set the first polygon to be unfogged.
			rpoly.iFogBand = 0;

			// Construct a plane for splitting the polygon at this boundary.
			// The splitting plane leaves the input polygon on the inside of the plane.
			CClipPlaneT<CPlaneDefNegY> clp_fog(fFogTerrainThreshold);

			// We check the return value, because the clipper may fail to split the
			// polygon if it barely crosses the plane.
			ESideOf esf = clp_fog.esfSplitPolygon(rpoly, plh, b_perspective);
			if (esf != esfOUTSIDE)
			{
				// Set the second polygon to be fogged.
				rpoly.iFogBand = 1;
			}
		}

		// Compute per-vertex fogging values.
		for (uint u_vertex = 0; u_vertex < rpoly.paprvPolyVertices.uLen; ++u_vertex)
		{
			SRenderVertex* prv = rpoly.paprvPolyVertices[u_vertex];
			prv->cvIntensity = fGetFogLevel(prv->v3Cam.tY);
		}
	}


// Global fog objects.
extern CFog fogFog;
extern CFog fogTerrainFog;

#endif
