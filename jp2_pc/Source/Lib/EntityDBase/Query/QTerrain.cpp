/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of QTerrain.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QTerrain.cpp                                    $
 * 
 * 8     4/23/98 4:35p Mlange
 * Wavelet quad tree data queries can now specify their frequence cutoff as a ratio of a node's
 * size.
 * 
 * 7     1/16/98 3:57p Mlange
 * Updated for changes to the CQueryRect base class constructor.
 * 
 * 6     1/09/98 3:36p Mlange
 * Moved all terrain query functionality to the base class.
 * 
 * 5     10/14/97 2:30p Mlange
 * Updated for the new multiresolution wavelet transform classes.
 * 
 * 4     97/10/02 12:33 Speter
 * Changed iFloatCast() to more correct iTrunc().
 * 
 **********************************************************************************************/

#include "GblInc/Common.hpp"
#include "QTerrain.hpp"

#include "Lib/Math/FloatDef.hpp"
#include "Lib/GeomDBase/Plane.hpp"


//**********************************************************************************************
//
// CWDbQueryTerrainTopology implementation.
//

	//******************************************************************************************
	CWDbQueryTerrainTopology::CWDbQueryTerrainTopology(const CBoundVol& bv, const CPresence3<>& pr3, TReal r_freq_highpass, bool b_freq_as_ratio, const CWorld& w)
		: NMultiResolution::CQueryRect(w.ptrrTerrain->pqnqRoot, bv, pr3, r_freq_highpass, b_freq_as_ratio)
	{
	}

