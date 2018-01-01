/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Stats for physics system.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/PhysicsStats.hpp                                         $
 * 
 * 8     10/01/98 6:49p Mlange
 * Improved collision message stats.
 * 
 * 7     98/09/30 19:10 Speter
 * Phys stats.
 * 
 * 6     98/09/28 2:47 Speter
 * Added stat.
 * 
 * 5     98/09/08 13:04 Speter
 * Stat for edges.
 * 
 * 4     98/09/05 23:21 Speter
 * Added stat for biomodel.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_PHYSICSSTATS_HPP
#define HEADER_LIB_PHYSICS_PHYSICSSTATS_HPP

#include "Lib/Sys/Profile.hpp"

extern CProfileStat
	psIntegrate,
		psBoundary,
		psIntBox,
			psBoxContact,
			psBoxMoments,
		psIntPelvis,
			psPelTrans,
			psPelNorm,
			psPelFoot,
		psIntBio,
		psIntFinish,
		psQTrrHeight,
		psQTrrHeightPel,
			psQTrrHeightFind,
			psQTrrHeightInit,
			psQTrrHeightEvaluate,
			psQTrrHeightZ,
		psQTrrEdge,
		psQTrrFriction,
		psQWater,
	psIntersect,
		psRegion,
		psQuery,
		psTQuery,
		psEvaluate,
		psAwaken,
			psAwakenVol,
		psInteract,
			psInteractVol,
		psActivate,
	psIgnoramus,
	psTestIntersect,
	psReportIntersect,
	psUpdate;

#endif