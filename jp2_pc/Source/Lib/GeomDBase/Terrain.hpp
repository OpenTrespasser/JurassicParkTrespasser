/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Main terrain definition classes.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/Terrain.hpp                                             $
 * 
 * 71    9/27/98 8:35p Mlange
 * Added clrGetBaseTextureColour() function.
 * 
 * 70    6/29/98 3:40p Mlange
 * Terrain ray cast integrated with general ray cast system.
 * 
 * 69    6/18/98 5:22p Mlange
 * Added ray-terrain intersect function.
 * 
 * 68    98/05/25 15:34 Speter
 * Added pqnqGetQueryRoot() accessor.
 * 
 * 67    98.05.14 7:01p Mmouni
 * Added save/restore of default settings.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_TERRAIN_HPP
#define HEADER_LIB_GEOMDBASE_TERRAIN_HPP

#include "WaveletStaticData.hpp"
#include "Lib/Transform/Vector.hpp"
#include "Lib/EntityDBase/Subsystem.hpp"
#include "Lib/Std/RangeVar.hpp"



//**********************************************************************************************
//
// Definitions for CTerrain.
//
namespace NMultiResolution
{
	class CQuadRootTINShape;
	class CQuadRootQuery;
	class CTransformedDataHeader;
};

class CWDbQueryTerrainTopology;
class CColour;

//**********************************************************************************************
//
class CTerrain: public CSubsystem
//
// The terrain subsystem.
//
// Prefix: trr
//
// Notes:
//		The CInstance base class of this subsystem references a valid shape. This shape is a
//		TIN mesh that represents the terrain.
//
//**************************************
{
public:
	static CRangeVar<float> rvarWireZoom;
	static bool bShowWireframe;				// 'true' if an additional wireframe version of the TIN mesh should be rendered.
	static bool bShowQuadtree;				// 'true' if an outline version of the wavelet quadtree should be rendered.

private:
	friend class CWDbQueryTerrainTopology;

	NMultiResolution::CTransformedDataHeader* ptdhData;

	rptr<NMultiResolution::CQuadRootTINShape> pqntinRoot;
	     NMultiResolution::CQuadRootQuery*    pqnqRoot;

	char *pc_defaults;		// Buffer containing default values for settings.

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//
public:
	CTerrain(const SInit& initins);

	~CTerrain();


	//*****************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SetStatAveraging(bool b_on)
	//
	// Enable/disable profile stat averaging.
	//
	//**************************************
	{
		NMultiResolution::bAverageStats = b_on;
	}


	//******************************************************************************************
	//
	NMultiResolution::CQuadRootQuery* pqnqGetQueryRoot() const
	//
	// Returns:
	//		The quadtree to use for physical terrain height queries.
	//
	//**************************************
	{
		return pqnqRoot;
	}

	//******************************************************************************************
	//
	TReal rHeight
	(
		TReal r_x,	// The world location to sample the height at.
		TReal r_y
	);
	//
	// Query terrain topology for its height at the given location.
	//
	// Returns:
	//		The height of the terrain at the given (x,y) world location.
	//
	//	Notes:
	//		May force a refinement of the quadtree.  Potentially very slow.
	//
	//**************************************


	//******************************************************************************************
	//
	TReal rHeightTIN
	(
		TReal r_x,	// The world location to sample the height at.
		TReal r_y
	);
	//
	// Returns:
	//		The height of the current TIN quad-tree at the given (x,y) world location
	//
	//	Notes:
	//		This version does not force a refinement, but uses the current viewer-dependent
	//		refinement of the TIN quad-tree.  Much faster, ideal for shadow queries.
	//
	//**************************************


	//******************************************************************************************
	//
	CColour clrGetBaseTextureColour
	(
		TReal r_world_x,
		TReal r_world_y
	) const;
	//
	// Returns:
	//		The colour of the base terrain texture at the given world x/y location, or black
	//		(colour 0, 0, 0) if given location is outside of bounds of the terrain.
	//
	//**************************************


	//******************************************************************************************
	//
	void FrameBegin
	(
		const CCamera* pcam_view	// Camera specifying the observer.
	);
	//
	// Must be called before terrain is rendered.
	//
	// Notes:
	//		Updates the terrain mesh for geometry and textures.
	//
	//**************************************


	//******************************************************************************************
	//
	void FrameEnd();
	//
	// Must be called after terrain has been rendered.
	//
	//**************************************


	//******************************************************************************************
	//
	void Rebuild
	(
		bool b_eval_geometry	// If true, forces re-evaluation of the terrain geometry,
								// ignoring any deferred evaluation counts.
	);
	//
	// Force a rebuild of all the terrain textures and, optionally, the entire terrain TIN mesh.
	//
	//**************************************


	//*****************************************************************************************
	//
	// Overrides.
	//

	void Cast(CTerrain** ptrr)
	{
		*ptrr = this;
	}

	bool bGetWorldExtents(CVector3<>& v3_min, CVector3<>& v3_max) const;

	void Process(const CMessageMove& msgmv);
	void Process(const CMessagePaint& msgpaint);

	//*****************************************************************************************
	char* pcSave(char* pc) const;

	//*****************************************************************************************
	const char* pcLoad(const char* pc);

	//*****************************************************************************************
	//
	void SaveDefaults();
	//
	// Save the default values of the user modifiable settings.
	//
	//**********************************

	//*****************************************************************************************
	//
	void RestoreDefaults();
	//
	// Restore the default values of the user modifiable settings.
	//
	//**********************************
};

#endif
