/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Terrain query classes for the world database.
 *
 * Bugs:
 *
 * To do:
 *		Change the query base class to allow non-container query objects to lock and unlock the
 *		world database.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QTerrain.hpp                                    $
 * 
 * 12    4/23/98 4:35p Mlange
 * Wavelet quad tree data queries can now specify their frequence cutoff as a ratio of a node's
 * size.
 * 
 * 11    1/16/98 3:57p Mlange
 * Updated for changes to the CQueryRect base class constructor.
 * 
 * 10    1/09/98 3:36p Mlange
 * Moved all terrain query functionality to the base class.
 * 
 * 9     10/14/97 2:30p Mlange
 * Updated for the new multiresolution wavelet transform classes.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QTERRAIN_HPP
#define HEADER_LIB_ENTITYDBASE_QUERY_QTERRAIN_HPP

#include "Lib/EntityDBase/Query.hpp"
#include "Lib/GeomDBase/Terrain.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeQuery.hpp"
#include "Lib/Transform/Presence.hpp"


//*********************************************************************************************
//
// Definitions for CWDbQueryTerrainTopology.
//
class CBoundVol;


//*********************************************************************************************
//
class CWDbQueryTerrainTopology : public NMultiResolution::CQueryRect
//
// Terrain height query.
//
// Prefix: wqttop
//
// Notes:
//		Each instance of this class defines a region (in the xy plane) in which topology of the
//		terrain must be calculated so that it can be subsequently iterated.
//
//**************************************
{
public:
	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	// Initialiser constructor. Queries given world database for the required multiresolution type.
	CWDbQueryTerrainTopology
	(
		const CBoundVol& bv,			// Volume to query.
		const CPresence3<>& pr3,		// World presence of volume.
		TReal r_freq_highpass = 0,		// Lower limit (in meters or as ratio) on wavelet coeficients.
		bool  b_freq_as_ratio = false,	// Whether the frequency is expressed as a ratio of a node's size.
        const CWorld& w = wWorld
	);
};



//*********************************************************************************************
//
class CWDbQueryTerrainMesh
//
// World database query container.
//
// Prefix: wqtmsh
//
// Notes:
//		Calling the constructor for this object will instantiate it with a CInstance type
//		whose renderer type refers to the TIN mesh of the terrain.
//
//		There is only ever one terrain mesh, so this query class does not need to inherit
//		from the CContainer type. This class does contain the tGet() member function, so in
//		use it behaves similar to the other query container classes.
//
//**************************************
{
	CInstance* pinsTIN;

public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryTerrainMesh(const CWorld& w = wWorld)
		: pinsTIN(w.ptrrTerrain)
	{
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	CInstance* tGet()
	//
	// Returns:
	//		The value from this container.
	//
	// Notes:
	//		Will return a null pointer if the terrain mesh does not currently exist!
	//
	//**************************************
	{
		// Return the value.
		return pinsTIN;
	}
};



//*********************************************************************************************
//
class CWDbQueryTerrain
//
// World database query container.
//
// Prefix: wqtrr
//
// Notes:
//		Calling the constructor for this object will instantiate it with a CTerrain type.
//
//		There is only ever one terrain, so this query class does not need to inherit
//		from the CContainer type. This class does contain the tGet() member function, so in
//		use it behaves similar to the other query container classes.
//
//**************************************
{
	CTerrain* ptrrTerrain;

public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryTerrain(const CWorld& w = wWorld)
		: ptrrTerrain(w.ptrrTerrain)
	{
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	CTerrain* tGet()
	//
	// Returns:
	//		The value from this container.
	//
	// Notes:
	//		Will return a null pointer if the terrain does not currently exist!
	//
	//**************************************
	{
		// Return the value.
		return ptrrTerrain;
	}
};



#endif
