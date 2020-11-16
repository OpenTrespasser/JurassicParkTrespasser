/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Functions importing world database functionality into simple physics library.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/PhysicsImport.hpp                                        $
 * 
 * 16    98/09/30 19:05 Speter
 * SetRotate().
 * 
 * 15    98/09/25 1:40 Speter
 * Removed dangerous and unused quad-tree-query height functions.
 * 
 * 14    98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 13    9/21/98 4:19p Agrant
 * added the hurtme function
 * 
 * 12    98/09/19 14:51 Speter
 * Made param order consistent; removed unneeded SendCollision overload.
 * 
 * 11    9/16/98 12:35a Agrant
 * added new energy calculations
 * 
 * 10    98/09/08 13:04 Speter
 * New format for iBoxTerrainEdges.
 * 
 * 9     98/08/26 2:17 Speter
 * Interfaced terrain edge query to physics.
 * 
 * 8     98/08/21 18:02 Speter
 * Updated for new collision parameters.
 * 
 * 7     98/08/10 21:51 Speter
 * Added pinsTerrain().
 * 
 * 6     98/08/02 16:47 Speter
 * Faster versions of fBoxTerrainHeight(), using max height. Replaced silly bBoxTerrainHighAs
 * with fBoxTerrainMaxHeight.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_PHYSICS_IMPORT_HPP
#define HEADER_LIB_PHYSICS_PHYSICS_IMPORT_HPP

class CInstance;
class CEntityWater;

#include "pelvis.h"
#include "Xob_bc.hpp"

//*****************************************************************************************
//
namespace NPhysImport
//
{
	//*****************************************************************************************
	void SetRotate
	(
		float aaf_rot[4][3],			// Destination quaternion array (sets zeroth deriv).
		float af_x[3], float af_y[3], float af_z[3]
										// Source matrix rows.
	);

	//*****************************************************************************************
	inline float fPelTerrainHeight
	(
		int i_pel,						// Which box to check.
		float f_x, float f_y			// Position at which to query height.
	)
	{
		return PelBox(i_pel, 0).fTerrainHeight(f_x, f_y);
	}

	//*****************************************************************************************
	inline float fPelTerrainHeight
	(
		int i_pel,						// Which pelvis to check.
		float f_x, float f_y,			// Position at which to query height.
		float af_normal[3]				// Returned terrain normal at this point.
	)
	{
		return PelBox(i_pel, 0).fTerrainHeight(f_x, f_y, af_normal);
	}

	//*****************************************************************************************
	float fTerrainFriction
	(
		float f_x, float f_y
	);

	//*****************************************************************************************
	CInstance* pinsTerrain();

	//*****************************************************************************************
	void SendCollision
	(
		CInstance* pins1, CInstance* pins2, 
		int i_mat1, int i_mat2,
		float af_loc[3],
		float f_energy_max, float f_energy_norm, float f_energy_trans,
		float f_delta_energy_1,
		float f_delta_energy_2
	);

	//*****************************************************************************************
	void CreateDisturbance
	(
		CEntityWater* petw, 
		float f_x, float f_y, 
		float f_radius, float f_height, 
		bool b_add
	);

	//*****************************************************************************************
	void RemoveMagnets
	(
		CInstance* pins, 
		bool b_remove_masters, bool b_remove_slaves
	);

	//*****************************************************************************************
	void ActivateIntersecting();
	
	//*****************************************************************************************
	bool bAllowSleep();

	//*****************************************************************************************
	void MakePlayerJumpNoise();

	//*****************************************************************************************
	void SendFootCollision(int i_pelvis, float af_location[3], float f_energy, float f_slide_energy);

	//*****************************************************************************************
	void HurtMe(int i_pelvis, float f_seconds);	// Punishes an animal for falling down a slope for a given amount of time.
};

#endif