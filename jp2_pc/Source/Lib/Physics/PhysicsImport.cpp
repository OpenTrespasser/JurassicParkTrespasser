/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of 'PhysicsImport.hpp.'
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/PhysicsImport.cpp                                        $
 * 
 * 24    98/10/02 17:20 Speter
 * Removed slow fTerrainFriction; new version in CXob.
 * 
 * 23    98/09/30 19:07 Speter
 * SetRotate().
 * 
 * 22    98/09/25 1:40 Speter
 * Removed dangerous and unused quad-tree-query height functions.
 * 
 * 21    98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 20    9/23/98 12:39a Agrant
 * yet another damage hack.
 * Will the madness never stop?
 * 
 * 19    9/22/98 1:16p Agrant
 * do not take sideways falling damage
 * 
 * 18    9/21/98 4:19p Agrant
 * added the hurtme function
 * 
 * 17    98/09/19 14:51 Speter
 * Made param order consistent.
 * 
 * 16    9/18/98 7:40p Bbell
 * removed assert
 * 
 * 15    9/16/98 12:35a Agrant
 * added new energy calculations
 * 
 * 14    98/09/15 4:02 Speter
 * Update for CPlayer.
 * 
 * 13    98/09/11 2:52 Speter
 * Tweaked message a bit.
 * 
 *********************************************************************************************/

#include "common.hpp"
#include "PhysicsImport.hpp"

#include "PhysicsSystem.hpp"
#include "Xob_bc.hpp"
#include "InfoSkeleton.hpp"
#include "PhysicsHelp.hpp"
#include "PhysicsStats.hpp"
#include "Magnet.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/EntityDBase/Query/QWater.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgCollision.hpp"
#include "Lib/GeomDBase/TerrainObj.hpp"
#include "Lib/EntityDBase/Water.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/Sys/ConIO.hpp"

//*****************************************************************************************
//
// class CXob implementation. The functions that require world database interfacing are
// defined here.
//

	//*****************************************************************************************
	const char* CXob::strInstanceName(int i_elem)
	{
		return Instances[i_elem]->strGetInstanceName();
	}

//*****************************************************************************************
//
namespace NPhysImport
//
{
	//*****************************************************************************************
	void SetRotate(float aaf_rot[4][3], float af_x[3], float af_y[3], float af_z[3])
	{
		CMatrix3<> mx3
		(
			//af_x[0], af_y[0], af_z[0],
			//af_x[1], af_y[1], af_z[1],
			//af_x[2], af_y[2], af_z[2]
			v3Convert(af_x), v3Convert(af_y), v3Convert(af_z)
		);

		// Horrible slow conversion.
		CRotate3<> r3(mx3, true);

		aaf_rot[0][0] = r3.tC;
		aaf_rot[1][0] = r3.v3S.tX;
		aaf_rot[2][0] = r3.v3S.tY;
		aaf_rot[3][0] = r3.v3S.tZ;
	}

	//*****************************************************************************************
	void CreateDisturbance(CEntityWater* petw, float f_x, float f_y, float f_radius, float f_height, bool b_add)
	{
		Assert(petw);
		if (petw)
			petw->CreateDisturbance(CVector3<>(f_x, f_y, 0), f_radius, f_height, b_add);
	}

	//*****************************************************************************************
	CInstance* pinsTerrain()
	{
		return CWDbQueryTerrain().tGet();
	}

	//*****************************************************************************************
	void SendCollision(CInstance* pins1, CInstance* pins2, int i_mat1, int i_mat2,
		float af_loc[3], float f_energy_max, float f_energy_norm, float f_energy_trans, 
		float f_energy_delta_1, float f_energy_delta_2)
	{
		conShadows <<CMessageStep::u4Frame <<": " <<pins1->strGetInstanceName() 
			<<" " <<(pins2 ? pins2->strGetInstanceName() : "");
		if (f_energy_max)
			conShadows <<" M=" <<f_energy_max;
		if (f_energy_norm)
			conShadows <<" H=" <<f_energy_norm;
		if (f_energy_trans)
			conShadows <<" S=" <<f_energy_trans;
		conShadows <<"\n";

		CMessageCollision
		(
			pins1, pins2,
			i_mat1, i_mat2,
			f_energy_max, f_energy_norm, f_energy_trans,
			v3Convert(af_loc),
			f_energy_delta_1, f_energy_delta_2,
			pphSystem
		).Dispatch();
	}

	//*****************************************************************************************
	void SendFootCollision(int i_pelvis, float af_location[3], float f_energy, float f_slide_energy)
	{
		CInstance* pins_pelvis = CPhysicsInfoSkeleton::paniSkeleton(i_pelvis);

		SendCollision
		(
			pins_pelvis, 
			CWDbQueryTerrain().tGet(),		// Terrain!
			pins_pelvis->pphiGetPhysicsInfo()->smatGetMaterialType(),
			0,		// Material 2 (grab from terrain)
			af_location,
			f_energy,
			f_energy, 
			f_slide_energy, 
			0.0f,
			0.0f
		);
	}

	//*****************************************************************************************
	void MakePlayerJumpNoise()
	{
		gpPlayer->JumpSound();
	}

	//*****************************************************************************************
	void RemoveMagnets(CInstance* pins, bool b_remove_masters, bool b_remove_slaves)
	{
		NMagnetSystem::RemoveMagnets(pins, b_remove_masters, b_remove_slaves);
	}

	//*****************************************************************************************
	void ActivateIntersecting()
	{
		pphSystem->ActivateIntersecting();
	}
	
	//*****************************************************************************************
	bool bAllowSleep()
	{
		return pphSystem->bAllowSleep;
	}

	//*****************************************************************************************
	void HurtMe(int i_pelvis, float f_seconds)
	{
		CInstance* pins_pelvis = CPhysicsInfoSkeleton::paniSkeleton(i_pelvis);
		AlwaysAssert(pins_pelvis);

		// Make this into a CAnimal cast instead of CAnimate to exempt Anne.
		CAnimate* pani = ptCast<CAnimate>(pins_pelvis);
		if (pani)
		{
			AlwaysAssert(f_seconds > 0.0f);

			// One second of fall will kill you.
			float f_damage = f_seconds * pani->fMaxHitPoints;
			pani->fHitPoints -= f_damage;
			pani->HandleDamage(f_damage);
		}
	}
};
