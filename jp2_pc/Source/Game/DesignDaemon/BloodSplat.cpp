/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of BloodSplat.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/BloodSplat.cpp                                      $
 * 
 * 6     10/02/98 11:09p Mlange
 * Blood splats are now always placed above all other terrain texture objects.
 * 
 * 5     9/28/98 10:21p Mlange
 * Positions and sizes of blood splats are now saved in the scene file.
 * 
 * 4     9/25/98 9:04p Mlange
 * Reworked blood splat system. It now incorporates the slope of the terrain in the placement
 * calculation to correctly handle bloodsplats on steep slopes.
 * 
 * 3     98/08/28 11:52 Speter
 * Only reports splat count one time on load.
 * 
 * 2     8/26/98 2:11p Mlange
 * Operational.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "BloodSplat.hpp"

#include "Game/DesignDaemon/Player.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/GeomDBase/TerrainObj.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/Math/FastSqrt.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Lib/Loader/SaveFile.hpp"

#include <string.h>
#include <stdio.h>

namespace
{
	// Minimum and maximum dimensions of blood splats, in metres.
	const TReal rMinSplatDimension = .2f;
	const TReal rMaxSplatDimension = 2.0f;

	// Minimum and maximum dimensions of blood streaks, in metres.
	const TReal rMinStreakDimension = .2f;
	const TReal rMaxStreakDimension = 2.0f;

	// Overlap ratios for blood splats reuse.
	const TReal rMinSplatOverlapReuse = .4f;
	const TReal rMaxSplatOverlapReuse = 1.5f;

	// Distance and orientation angle for blood streak reuse.
	const TReal rMinStreakReuseDist = .3f;
	const TReal rMinStreakReuseDotp = .85f;

	const TReal rMinOverlapDimensionMultiplier = 1.1f;

	// Relative importance of distance vs scale for reuse selection of blood splat.
	const TReal rDistReuseImportance = 2.0f;

	// Relative scale adjustment for terrain slope.
	const TReal rSlopeAdjustFactor = .5f;

	// Random number generator for this module.
	CRandom rndModule;

	// High bit of blood splat terrain object heights.
	const int iTrrObjBaseHeightBit = 29;
}


//**********************************************************************************************
//
class CBloodSplats::CPriv : public CBloodSplats
//
// Private implementation of CBloodSplats.
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	static SBloodSplatObj* pbsoFindClosest
	(
		TContainer& rcont,
		TReal r_world_x,
		TReal r_world_y
	);
	//
	// Find the closest blood splat or streak to the given location.
	//
	//**************************************


	//******************************************************************************************
	//
	static SBloodSplatObj* pbsoFindReuse
	(
		TContainer& rcont,
		TReal r_rel_dimension
	);
	//
	// Find the 'best' blood splat or streak to reuse.
	//
	//**************************************


	//******************************************************************************************
	//
	void SetBloodObjPlacement
	(
		SBloodSplatObj* pbso,
		const CPlacement3<>& p3,
		TReal r_scale
	);
	//
	// Set position and scale of blood splat or streak. Adjusts the scale of the given blood
	// splat or streak for the Z slope of the terrain.
	//
	//**************************************
};


//**********************************************************************************************
//
// CBloodSplats implementation.
//

	//******************************************************************************************
	CBloodSplats::CBloodSplats()
	{
		SetInstanceName("CBloodSplats");

		CMessageSystem::RegisterRecipient(this);
	}

	//******************************************************************************************
	CBloodSplats::~CBloodSplats()
	{
		CMessageSystem::UnregisterRecipient(this);
	}

	//******************************************************************************************
	void CBloodSplats::CreateSplat(TReal r_world_x, TReal r_world_y, TReal r_dimension)
	{
		// Cannot create a blood splat if we have no terrain texture objects for it.
		if (contSplats.size() == 0)
			return;

		// Reject blood splats that are too small.
		if (r_dimension < rMinSplatDimension)
			return;

		// Find the bloodplat that is nearest the requested location.
		SBloodSplatObj* pbso_splat_nearest = priv_self.pbsoFindClosest(contSplats, r_world_x, r_world_y);

		// Determine the dimension required for the nearest blood splat to encompass the requested one.
		TReal r_dim_req = ((CVector2<>(pbso_splat_nearest->ptrobjShape->v3Pos()) -
		                    CVector2<>(r_world_x, r_world_y)                      ).tLen() + r_dimension * .5f) * 2;

		// Determine the overlap ratio between the closest bloodsplat and the requested one.
		TReal r_overlap = r_dim_req / pbso_splat_nearest->rDimension;

		if (r_overlap < rMinSplatOverlapReuse)
			return;


		SBloodSplatObj* pbso_splat;
		CPlacement3<> p3_splat;

		if (r_overlap > rMaxSplatOverlapReuse || r_dim_req > rMaxSplatDimension)
		{
			// We need to add a new bloodsplat. Find the smallest and furthest to reuse.
			pbso_splat = priv_self.pbsoFindReuse(contSplats, rDefaultSplatDimension);

			// Rotate blood splat around its local z axis by random amount.
			CRotate3<> r3_splat(CDir3<>(0, 0, 1), CAngle(rndModule(0.0, d2_PI)));

			p3_splat = CPlacement3<>(r3_splat, CVector3<>(r_world_x, r_world_y, 0));

			#if VER_TEST
				dout << "New blood splat " << r_dimension << "m.\n";
			#endif
		}
		else
		{
			// Do not add a new splat, grow the nearest one instead.
			pbso_splat = pbso_splat_nearest;

			if (r_overlap > 1)
				r_dimension = r_dim_req;
			else
				// The requested blood splat is entirely within the existing one, grow it slightly.
				r_dimension = pbso_splat->rDimension * rMinOverlapDimensionMultiplier;

			p3_splat = pbso_splat->ptrobjShape->p3Placement();

			#if VER_TEST
				dout << "Scaled existing blood splat to " << r_dimension << "m.\n";
			#endif
		}

		// Clamp to maximum dimension.
		pbso_splat->rDimension = Min(r_dimension, rMaxSplatDimension);

		priv_self.SetBloodObjPlacement(pbso_splat, p3_splat, pbso_splat->rDimension / rDefaultSplatDimension);
	}


	//******************************************************************************************
	void CBloodSplats::CreateStreak(TReal r_world_x, TReal r_world_y, TReal r_dimension, const CDir2<>& d2_direction)
	{
		// Cannot create a blood streak if we have no terrain texture objects for it.
		if (contStreaks.size() == 0)
			return;

		// Reject blood streaks that are too small.
		if (r_dimension < rMinStreakDimension)
			return;

		// Find the blood streak that is nearest the requested location.
		SBloodSplatObj* pbso_streak_nearest = priv_self.pbsoFindClosest(contStreaks, r_world_x, r_world_y);

		// Determine its orientation in the xy plane.
		CDir2<> d2_nearest_orient = CDir3<>(0, 1, 0) * pbso_streak_nearest->ptrobjShape->r3Rot();

		TReal r_nearest_dist_sqr = (CVector2<>(pbso_streak_nearest->ptrobjShape->v3Pos()) -
		                            CVector2<>(r_world_x, r_world_y)                       ).tLenSqr();

		TReal r_nearest_dotp = d2_nearest_orient * d2_direction;


		SBloodSplatObj* pbso_streak;
		CPlacement3<> p3_streak;

		if (r_nearest_dist_sqr > Sqr(rMinStreakReuseDist) || r_nearest_dotp < rMinStreakReuseDotp)
		{
			// We need to add a new blood streak. Find the smallest and furthest to reuse.
			pbso_streak = priv_self.pbsoFindReuse(contStreaks, rDefaultStreakDimension);

			CRotate3<> r3_streak(CDir3<>(0, 1, 0), CDir3<>(d2_direction.tX, d2_direction.tY, 0));

			p3_streak = CPlacement3<>(r3_streak, CVector3<>(r_world_x, r_world_y, 0));

			#if VER_TEST
				dout << "New blood streak " << r_dimension << "m.\n";
			#endif
		}
		else
		{
			// Do not add a new streak, grow the nearest one instead.
			pbso_streak = pbso_streak_nearest;

			r_dimension = Max(r_dimension, pbso_streak->rDimension * rMinOverlapDimensionMultiplier);

			p3_streak = pbso_streak->ptrobjShape->p3Placement();


			#if VER_TEST
				dout << "Scaled existing blood streak to " << r_dimension << "m.\n";
			#endif
		}

		// Clamp to maximum dimension.
		pbso_streak->rDimension = Min(r_dimension, rMaxStreakDimension);

		priv_self.SetBloodObjPlacement(pbso_streak, p3_streak, pbso_streak->rDimension / rDefaultStreakDimension);
	}


	//******************************************************************************************
	void CBloodSplats::Process(const CMessageSystem& ms)
	{
		if (ms.escCode == escGROFF_LOADED)
		{
			// Reset blood height counter.
			iCurrentTrrObjHeight = 0;

			// A new GROFF file has been loaded, find all blood splat terrain texture objects that
			// it contains.
			std::string str_base_splat_name("TrnObj_BloodSplat-");
			std::string str_base_streak_name("TrnObj_BloodStreak-");

			for (int i_instance = 0; ; i_instance++)
			{
				char str_instanced[3];
				sprintf(str_instanced, "%.2d", i_instance);

				CInstance* pins_splat  = pwWorld->ppartTerrainPartitionList()->pinsFindInstance(str_base_splat_name  + str_instanced);
				CInstance* pins_streak = pwWorld->ppartTerrainPartitionList()->pinsFindInstance(str_base_streak_name + str_instanced);

				if (!pins_splat && !pins_streak)
					break;

				if (pins_splat)
				{
					CTerrainObj* ptrobj_splat = ptCast<CTerrainObj>(pins_splat);

					// Found a blood splat that is not a terrain texture object!
					AlwaysAssert(ptrobj_splat != 0);

					// Set initial 'height'.
					ptrobj_splat->iHeight = 1 << iTrrObjBaseHeightBit;

					contSplats.push_back(SBloodSplatObj(ptrobj_splat));
				}

				if (pins_streak)
				{
					CTerrainObj* ptrobj_streak = ptCast<CTerrainObj>(pins_streak);

					// Found a blood streak that is not a terrain texture object!
					AlwaysAssert(ptrobj_streak != 0);

					// Set initial 'height'.
					ptrobj_streak->iHeight = 1 << iTrrObjBaseHeightBit;

					contStreaks.push_back(SBloodSplatObj(ptrobj_streak));
				}
			}

			if (contSplats.size() != 0)
			{
				// Determine the 'base' size of the blood splat objects from the first instanced blood splat.
				CTerrainObj* ptrobj_splat = (*contSplats.begin()).ptrobjShape;

				CVector3<> v3_min, v3_max;
				ptrobj_splat->pbvBoundingVol()->GetWorldExtents(CPresence3<>(), v3_min, v3_max);

				rDefaultSplatDimension = Max(v3_max.tX - v3_min.tX, v3_max.tY - v3_min.tY);
			}

			if (contStreaks.size() != 0)
			{
				// Determine the 'base' size of the blood streak objects from the first instanced blood streak.
				CTerrainObj* ptrobj_streak = (*contStreaks.begin()).ptrobjShape;

				CVector3<> v3_min, v3_max;
				ptrobj_streak->pbvBoundingVol()->GetWorldExtents(CPresence3<>(), v3_min, v3_max);

				rDefaultStreakDimension = Max(v3_max.tX - v3_min.tX, v3_max.tY - v3_min.tY);
			}

			#if VER_TEST
				dout << "Found " << contSplats.size()  << " blood splat objects.\n";
				dout << "Found " << contStreaks.size() << " blood streak objects.\n";
			#endif
		}
	}

	//*****************************************************************************************
	char * CBloodSplats::pcSave(char *  pc_buffer) const
	{
		TContainer::const_iterator it;

		// Save blood splats positions and dimensions.
		for (it = contSplats.begin(); it != contSplats.end(); ++it)
		{
			pc_buffer = pcSaveT(pc_buffer, (*it).ptrobjShape->p3Placement());
			pc_buffer = pcSaveT(pc_buffer, (*it).rDimension);
		}

		// Save blood streaks positions and dimensions.
		for (it = contStreaks.begin(); it != contStreaks.end(); ++it)
		{
			pc_buffer = pcSaveT(pc_buffer, (*it).ptrobjShape->p3Placement());
			pc_buffer = pcSaveT(pc_buffer, (*it).rDimension);
		}

		return pc_buffer;
	}

	//*****************************************************************************************
	const char * CBloodSplats::pcLoad(const char *  pc_buffer)
	{
		if (CSaveFile::iCurrentVersion >= 19)
		{
			TContainer::iterator it;

			// Load blood splats positions and dimensions.
			for (it = contSplats.begin(); it != contSplats.end(); ++it)
			{
				CPlacement3<> p3;
				TReal r_dim;

				pc_buffer = pcLoadT(pc_buffer, &p3);
				pc_buffer = pcLoadT(pc_buffer, &r_dim);

				priv_self.SetBloodObjPlacement(&(*it), p3, r_dim / rDefaultSplatDimension);
			}

			// Load blood streaks positions and dimensions.
			for (it = contStreaks.begin(); it != contStreaks.end(); ++it)
			{
				CPlacement3<> p3;
				TReal r_dim;

				pc_buffer = pcLoadT(pc_buffer, &p3);
				pc_buffer = pcLoadT(pc_buffer, &r_dim);

				priv_self.SetBloodObjPlacement(&(*it), p3, r_dim / rDefaultStreakDimension);
			}
		}

		return pc_buffer;
	}


//**********************************************************************************************
//
// CBloodSplats::CPriv implementation.
//

	//******************************************************************************************
	CBloodSplats::SBloodSplatObj* CBloodSplats::CPriv::pbsoFindClosest(TContainer& rcont, TReal r_world_x, TReal r_world_y)
	{
		//
		// Find the bloodplat that is closest to the requested location.
		//
		TReal r_min_dist_sqr = FLT_MAX;
		SBloodSplatObj* pbso_nearest  = 0;

		for (TContainer::iterator it = rcont.begin(); it != rcont.end(); ++it)
		{
			TReal r_dist_sqr = (CVector2<>((*it).ptrobjShape->v3Pos()) - CVector2<>(r_world_x, r_world_y)).tLenSqr();
			if (r_dist_sqr < r_min_dist_sqr)
			{
				r_min_dist_sqr = r_dist_sqr;
				pbso_nearest = &(*it);
			}
		}

		return pbso_nearest;
	}

	//******************************************************************************************
	CBloodSplats::SBloodSplatObj* CBloodSplats::CPriv::pbsoFindReuse(TContainer& rcont, TReal r_rel_dimension)
	{
		//
		// Find the smallest and furthest to reuse.
		//

		// Use player as observer if loaded, otherwise use camera.
		CVector2<> v2_observer = gpPlayer ? gpPlayer->v3Pos() : pwWorld->pcamGetActiveCamera()->v3Pos();

		TReal r_best_score = -1;
		SBloodSplatObj* pbso = 0;

		for (TContainer::iterator it = rcont.begin(); it != rcont.end(); ++it)
		{
			// Score is a simple linear combination of distance from observer and relative dimension of blood splat/streak.
			TReal r_dist_sqr = (v2_observer - CVector2<>((*it).ptrobjShape->v3Pos())).tLenSqr();
			TReal r_score    = (r_dist_sqr * rDistReuseImportance) / Sqr((*it).rDimension / r_rel_dimension);

			if (r_score > r_best_score)
			{
				r_best_score = r_score;
				pbso = &(*it);
			}
		}

		return pbso;
	}

	//******************************************************************************************
	void CBloodSplats::CPriv::SetBloodObjPlacement(SBloodSplatObj* pbso, const CPlacement3<>& p3, TReal r_scale)
	{
		CTerrain* ptrr = CWDbQueryTerrain().tGet();

		if (!ptrr)
			return;

		// Query the terrain height at various points on the bounding volume's projected rectangle.
		CVector3<> v3_min, v3_max;
		pbso->ptrobjShape->pbvBoundingVol()->GetWorldExtents(CPresence3<>(p3, r_scale), v3_min, v3_max);

		TReal r_height_min_x = ptrr->rHeight(v3_min.tX, (v3_min.tY + v3_max.tY) * .5f);
		TReal r_height_max_x = ptrr->rHeight(v3_max.tX, (v3_min.tY + v3_max.tY) * .5f);

		TReal r_height_min_y = ptrr->rHeight((v3_min.tX + v3_max.tX) * .5f, v3_min.tY);
		TReal r_height_max_y = ptrr->rHeight((v3_min.tX + v3_max.tX) * .5f, v3_max.tY);

		TReal r_ratio_x = (v3_max.tX - v3_min.tX) / fSqrtEst(Sqr(r_height_max_x - r_height_min_x) + Sqr(v3_max.tX - v3_min.tX));
		TReal r_ratio_y = (v3_max.tY - v3_min.tY) / fSqrtEst(Sqr(r_height_max_y - r_height_min_y) + Sqr(v3_max.tY - v3_min.tY));

		TReal r_slope_adjust = 1.0f - ((1.0f - Min(r_ratio_x, r_ratio_y)) * rSlopeAdjustFactor);

		r_scale *= r_slope_adjust;

		pbso->ptrobjShape->SetScale(r_scale);
		pbso->ptrobjShape->Move(p3);

		// Place this blood splat above all others.
		iCurrentTrrObjHeight++;
		iCurrentTrrObjHeight &= (1 << iTrrObjBaseHeightBit) - 1;

		pbso->ptrobjShape->iHeight = (1 << iTrrObjBaseHeightBit) | iCurrentTrrObjHeight;
	}

//******************************************************************************************
CBloodSplats* pBloodSplats;
