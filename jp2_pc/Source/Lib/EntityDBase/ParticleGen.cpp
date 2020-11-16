/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of ParticleGen.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/ParticleGen.cpp                                      $
 * 
 * 17    10/07/98 4:04p Mlange
 * Changed sand parameters.
 * 
 * 16    98.09.30 10:41p Mmouni
 * Tweaked particle colours and added some new sound materials.
 * 
 * 15    98/09/27 18:02 Speter
 * Later particle mappings do not override earlier ones (so code effects take precedence over
 * text prop effects). Tweaked sparks.
 * 
 * 14    98/09/23 0:09 Speter
 * Changedness.
 * 
 * 13    98/09/19 14:44 Speter
 * Changed handling of particle parameter randomisation, for greater control. Added nice dust
 * defaults.
 * 
 *********************************************************************************************/

#include "common.hpp"
#include "ParticleGen.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/Audio/SoundDefs.hpp"

#include <list>
#include <map>

//*********************************************************************************************
//
// class CCreateParticleStatic implementation.
//

	//*********************************************************************************************
	CCreateParticleStatic::CCreateParticleStatic(const CCreateParticles& crt, TReal r_size)
		: CCreateParticles(crt)
	{
		// Override the parameters.
		ppMin.fNumParticles	= 1;
		ppMin.fVelocity		= 0;
		ppMin.sLifespan		= 0;
		ppMin.fSize			= r_size;

		ppMax = ppMin;
	}

//*********************************************************************************************
//
// class CParticleGenerator implementation.
//

	//*********************************************************************************************
	CParticleGenerator::CParticleGenerator(const CCreateParticles& crt, const CPlacement3<>& p3)
		: CCreateParticles(crt), p3Origin(p3)
	{
	}

	//*********************************************************************************************
	void CParticleGenerator::Step(TSec s_step)
	{
		Create(p3Origin.v3Pos, d3YAxis * p3Origin.r3Rot, s_step);
	}

	//*********************************************************************************************
	void CParticleGenerator::Step(TSec s_step, float f_interp)
	{
		Create(p3Origin.v3Pos, d3YAxis * p3Origin.r3Rot, s_step, f_interp);
	}

//*********************************************************************************************
//
// class CParticleCycle implementation.
//

	const float fTROUGH	= 0.25f;

	//*********************************************************************************************
	CParticleCycle::CParticleCycle(const CCreateParticlesCycle& crtc, float f_strength, const CPlacement3<>& p3)
		: CCreateParticlesCycle(crtc),
		  prtgenGenerator(crtc, p3),
		  fScale(Max(prtgenGenerator.fIntensity(f_strength), 0.0f)), 
		  sAge(sDuty)
	{
	}

	//*********************************************************************************************
	void CParticleCycle::Step(TSec s_time)
	{
		// Decay the scale.
		if (sAge > sPeriod)
			fScale *= exp(-fDecay * s_time);
		Assert(fScale >= 0.0f);

		// Modify the spurt according to age. To integrate over time,
		// find max and avg amplitude of cycle. Only deal with at most 1 period.
  		TSec s_phase0 = fmod(sAge, sPeriod);
		TSec s_phase1 = s_phase0 + s_time;

		if (s_phase1 > 1.0)
		{
			s_phase1 -= 1.0;
			s_phase0 -= 1.0;
			SetMax(s_phase0, 0.0);
		}

		sAge += s_time;

		float f_avg = 0, f_max = 0;

		if (s_phase0 <= sDuty * 0.5)
		{
			// Starts in up-ramping section.
			TSec s_end = Min(s_phase1, sDuty*0.5);

			// Integrate over this section.
			f_avg += (s_phase0 + s_end) / sDuty * (s_end - s_phase0);
			f_max = s_end / (sDuty * 0.5);
		}

		if (s_phase0 < sDuty && s_phase1 > sDuty*0.5)
		{
			// Exists in down-ramping section.
			TSec s_start = Max(s_phase0, sDuty*0.5);
			TSec s_end = Min(s_phase1, sDuty);

			// Integrate over this section.
			f_avg += (1.0 - (s_start + s_end - sDuty) / sDuty) * (s_end - s_start);
			SetMax(f_max, 1.0 - (s_start - sDuty*0.5) / (sDuty*0.5));
		}

		f_avg /= s_time;
		float f_min = Max(2 * f_avg - f_max, 0);

		// Modify generator by the separate min and max values.
		prtgenGenerator.ppMax = ppMin.ppInterpolate(ppMax, fScale);
		float f_size = prtgenGenerator.ppMax.fSize;

		prtgenGenerator.ppMax = prtgenGenerator.ppMax *	
			(fTROUGH + (1.0f - fTROUGH) * f_max);
		prtgenGenerator.ppMin = ppMin.ppInterpolate(ppMax, fScale) *
			(fTROUGH + (1.0f - fTROUGH) * f_min);

		// Now let generator go to work.
		prtgenGenerator.Step(s_time);

		// Create the per-wound static particle.
		// Must be re-created once pre frame.
		CCreateParticleStatic prtstat(*this, f_size);
		prtstat.Create(prtgenGenerator.p3GetPlacement().v3Pos, d3YAxis * prtgenGenerator.p3GetPlacement().r3Rot);
	}

//******************************************************************************************
//
namespace NParticleMap
//
// Implementation.
// 
//**************************************
{
	std::list<CCreateParticles> lspcrtEffects;	// The list of effects.

	typedef std::map< TSoundMaterial, CCreateParticles*, std::less<TSoundMaterial> > TParticleMap;
	// Prefix: prtm

	TParticleMap	prtmEffectMap;			// The mapping to materials.

	//******************************************************************************************
	CCreateParticles* pcrtAddParticle(const CCreateParticles& crt)
	{
		// Just duplicate and add to list. Do not check for duplicates.
		std::list<CCreateParticles>::iterator it = lspcrtEffects.insert(lspcrtEffects.end(), crt);
		return &*it;
	}

	//******************************************************************************************
	void AddMapping(CCreateParticles* pcrt, TSoundMaterial smat)
	{
		// Do not allow overrides.
		TParticleMap::iterator it = prtmEffectMap.find(smat);
		if (it == prtmEffectMap.end())
		{
			// No current effect.
			prtmEffectMap[smat] = pcrt;
		}
	}

	//*********************************************************************************************
	void AddDefaults()
	{
		//
		// Add some hard-coded default particle effects to the map.
		//

		CCreateParticles crt;
		CCreateParticles* pcrt;

		// Blood hit.
		crt = CCreateParticles(clrBlood);
		crt.ppMax.fThreshold	= 60.0;		crt.ppMin.fThreshold	= 0.0;
		crt.ppMax.fNumParticles	= 60;		crt.ppMin.fNumParticles	= 20;
		crt.ppMax.fVelocity		= 3.0;		crt.ppMin.fVelocity		= 0.0;
		crt.ppMax.fSize			= 0.02;		crt.ppMin.fSize			= 0.01;
		crt.ppMax.fLength		= 0.1;		crt.ppMin.fLength		= 0.05;
		crt.ppMax.sLifespan		= 2.0;		crt.ppMin.sLifespan		= 0.5;

		pcrt = pcrtAddParticle(crt);
		AddMapping(pcrt, matHashIdentifier("Blood"));

		// Bleeding.
		crt = CCreateParticles(clrBlood);
		crt.fTopMin				= 0.5f;
		crt.ppMax.fThreshold	= 60.0;		crt.ppMin.fThreshold	= 0.0;
		crt.ppMax.fNumParticles	= 60;		crt.ppMin.fNumParticles	= 20;
		crt.ppMax.fVelocity		= 6.5;		crt.ppMin.fVelocity		= 2.5;
		crt.ppMax.fSize			= 0.02;		crt.ppMin.fSize			= 0.01;
		crt.ppMax.fLength		= 0.3;		crt.ppMin.fLength		= 0.1;
		crt.ppMax.sLifespan		= 2.0;		crt.ppMin.sLifespan		= 0.5;
		crt.ppMax.fSpreadRadians=			crt.ppMin.fSpreadRadians= 5 * dDEGREES;

		pcrt = pcrtAddParticle(crt);
		AddMapping(pcrt, matHashIdentifier("Bleed"));

		// Water.
		crt = CCreateParticles(clrWaterSpecular);
		crt.iPriority			= 5;
		crt.bLogScale			= true;
		crt.ppMax.fThreshold	= 1.0;		crt.ppMin.fThreshold	= 0.2;
		crt.ppMax.fNumParticles	= 30;		crt.ppMin.fNumParticles	= 0;
		crt.ppMax.fVelocity		= 6;		crt.ppMin.fVelocity		= 0;
		crt.ppMax.fSize			= 0.03;		crt.ppMin.fSize			= 0.005;
		crt.ppMax.fLength		= 0.10;		crt.ppMin.fLength		= 0.02;
		crt.ppMax.sLifespan		= 3;		crt.ppMin.sLifespan		= 0.5;
		crt.ppMax.fSpreadRadians= 60 * dDEGREES;
		pcrt = pcrtAddParticle(crt);
		AddMapping(pcrt, matHashIdentifier("Water"));

		// Terrain default dust; has small collision threshold.
		crt = CCreateParticles(CColour(0.5, 0.4, 0.3));
		crt.iPriority			= 4;
		crt.eptParticleType		= eptDust;
		crt.ppMax.fThreshold	= 1.0;		crt.ppMin.fThreshold	= 0.3;
		crt.ppMax.fNumParticles	= 60;		crt.ppMin.fNumParticles	= 20;
		crt.ppMax.fSize			= 2;		crt.ppMin.fSize			= 2;		// Pixels.
		crt.ppMax.fVelocity		= 10;		crt.ppMin.fVelocity		= 0;
		crt.ppMax.fAccelStuck	= 40;		crt.ppMin.fAccelStuck	= 40;
		crt.ppMax.fAccelUnstuck	= 0;		crt.ppMin.fAccelUnstuck	= 0;
		crt.ppMax.sLifespan		= 2;		crt.ppMin.sLifespan		= 2;
		crt.ppMax.fSpinRadPerSec= 360 * dDEGREES;	
											crt.ppMin.fSpinRadPerSec= 0;
		crt.ppMax.fSpreadRadians=			crt.ppMin.fSpreadRadians= 90 * dDEGREES;

		pcrt = pcrtAddParticle(crt);
		AddMapping(pcrt, matHashIdentifier("Terrain"));

		// Sand. Smaller threshold, more particles, with some gravity.
		crt = CCreateParticles(CColour(0.8, 0.7, 0.5));
		crt.iPriority			= 4;
		crt.eptParticleType		= eptDust;
		crt.ppMax.fThreshold	= 1.0;		crt.ppMin.fThreshold	= 0.1;
		crt.ppMax.fNumParticles	= 80;		crt.ppMin.fNumParticles	= 40;
		crt.ppMax.fSize			= 2;		crt.ppMin.fSize			= 2;		// Pixels.
		crt.ppMax.fVelocity		= 8;		crt.ppMin.fVelocity		= 3;
		crt.ppMax.fAccelStuck	= 10;		crt.ppMin.fAccelStuck	= 40;
		crt.ppMax.fAccelUnstuck	= 5;		crt.ppMin.fAccelUnstuck	= 0;
		crt.ppMax.sLifespan		= 2;		crt.ppMin.sLifespan		= 2;
		crt.ppMax.fSpinRadPerSec= 360 * dDEGREES;	
											crt.ppMin.fSpinRadPerSec= 0;
		crt.ppMax.fSpreadRadians=			crt.ppMin.fSpreadRadians= 90 * dDEGREES;

		pcrt = pcrtAddParticle(crt);
		AddMapping(pcrt, matHashIdentifier("Terrain - Sand"));

		// Default dust; occurs only on large collisions.
		crt = CCreateParticles(CColour(0.35, 0.35, 0.35));
		crt.iPriority			= 3;
		crt.eptParticleType		= eptDust;
		crt.ppMax.fThreshold	= 1.0;		crt.ppMin.fThreshold	= 0.4;
		crt.ppMax.fNumParticles	= 40;		crt.ppMin.fNumParticles	= 15;
		crt.ppMax.fSize			= 2;		crt.ppMin.fSize			= 2;
		crt.ppMax.fVelocity		= 5;		crt.ppMin.fVelocity		= 0;
		crt.ppMax.fAccelStuck	= 40;		crt.ppMin.fAccelStuck	= 20;
		crt.ppMax.fAccelUnstuck	= 1;		crt.ppMin.fAccelUnstuck	= 0;
		crt.ppMax.sLifespan		= 2;		crt.ppMin.sLifespan		= 0.5;
		crt.ppMax.fSpinRadPerSec= 360 * dDEGREES;	
											crt.ppMin.fSpinRadPerSec= 0;
		crt.ppMax.fSpreadRadians=			crt.ppMin.fSpreadRadians= 90 * dDEGREES;

		// Set as default.
		pcrt = pcrtAddParticle(crt);
		AddMapping(pcrt, 0);

		// Turn off collision particles for animates; that's what blood is for.
		AddMapping(0, matHashIdentifier("Animate"));

		// Turn off collision particels for concrete.
		AddMapping(0, matHashIdentifier("Terrain - Concrete"));

		// Sparks.
		crt = CCreateParticles(CColour(1.0, 0.75, 0.25));
		crt.iPriority			= 2;
		crt.ppMax.fThreshold	= 1.0;		crt.ppMin.fThreshold	= 0.5;
		crt.eptParticleType		= eptFluid;
		crt.ppMax.fNumParticles	= 30;		crt.ppMin.fNumParticles	= 0;
		crt.ppMax.fSize			= 0.005;	crt.ppMin.fSize			= 0.003;
		crt.ppMax.fLength		= 0.09;		crt.ppMin.fLength		= 0.02;
		crt.ppMax.fVelocity		= 5;		crt.ppMin.fVelocity		= 2;
		crt.ppMax.sLifespan		= 0.3;		crt.ppMin.sLifespan		= 0.25;
		crt.ppMax.fSpreadRadians= 90 * dDEGREES;	
											crt.ppMin.fSpreadRadians= 45 * dDEGREES;
		crt.ppMax.fAccelStuck	=			crt.ppMin.fAccelStuck	= 0;
		crt.ppMax.fAccelUnstuck	=			crt.ppMin.fAccelUnstuck	= 0;

		pcrt = pcrtAddParticle(crt);
		AddMapping(pcrt, matHashIdentifier("Metal Hard 1"));
		AddMapping(pcrt, matHashIdentifier("Metal Dull 2"));
		AddMapping(pcrt, matHashIdentifier("Oil Barrel 1"));
		AddMapping(pcrt, matHashIdentifier("Terrain - Metal"));

/*
		// Wood chips.
		CCreateParticles crt(CColour(0.5, 0.25, 0.0));
		crt.eptParticleType = eptSolid;
		crt.ppMax.fThreshold		= 1.0;		crt.ppMin.fThreshold		= 0.4;
		crt.ppMax.fNumParticles	= 20;		crt.ppMin.fNumParticles	= 5;
		crt.ppMax.fVelocity		= 1.5;		crt.ppMin.fVelocity		= 0.25;
		crt.ppMax.fSize			= 0.02;		crt.ppMin.fSize			= 0.01;
		crt.ppMax.fSpinRadPerSec	= 360 * dDEGREES;	crt.ppMin.fSpinRadPerSec	= 60 * dDEGREES;
		crt.ppMax.sLifespan		= 2;		crt.ppMin.sLifespan		= 0.25;
		crt.ppMax.fSpreadRadians	= 60 * dDEGREES;	crt.ppMin.fSpreadRadians = 90 * dDEGREES;

		// Set as default.
		pcrt = pcrtAddParticle(crt);
		AddMapping(pcrt, matHashIdentifier("Wood"));

		// Sap.
		CCreateParticles crt_sap(CColour(0.2, 0.1, 0.1));
		crt_sap.ppMax.fNumParticles	=			crt_sap.ppMin.fNumParticles	= 1;
		crt_sap.ppMax.fVelocity		=			crt_sap.ppMin.fVelocity		= 0;
		crt_sap.ppMax.fSize			= 0.02;		crt_sap.ppMin.fSize			= 0.005;
		crt_sap.ppMax.fLength		= 0.10;		crt_sap.ppMin.fLength		= 0.05;
		crt_sap.ppMax.sLifespan		= 5;		crt_sap.ppMin.sLifespan		= 5;
		crt_sap.ppMax.fAccelStuck	= 0.05;		crt_sap.ppMin.fAccelStuck	= .01;
		crt_sap.ppMax.fAccelUnstuck	=			crt_sap.ppMin.fAccelUnstuck	= 10;
		crt_sap.ppMax.fThreshold	= 1.0;		crt_sap.ppMin.fThreshold	= 0.01;

		// Add as default!
		pcrt = pcrtAddParticle(crt_sap);
		AddMapping(pcrt, 0);

*/
	}

	//******************************************************************************************
	// This is called at the start and before any new level is loaded.
	void Reset()
	{
		erase_all(lspcrtEffects);
		erase_all(prtmEffectMap);
		
		AddDefaults();
	}

	//******************************************************************************************
	CCreateParticles* pcrtFind(TSoundMaterial smat, TSoundMaterial smat_alt)
	{
		if (smat)
		{
			TParticleMap::iterator it = prtmEffectMap.find(smat);
			if (it != prtmEffectMap.end())
				return (*it).second;
		}

		// Not found; search for alternate.
		if (smat_alt)
		{
			TParticleMap::iterator it = prtmEffectMap.find(smat_alt);
			if (it != prtmEffectMap.end())
				return (*it).second;
		}

		// Return default, if any.
		return prtmEffectMap[0];
	}

	//*********************************************************************************************
	class CInit
	{
	public:
		CInit()
		{
			AddDefaults();
		}
	};

	static CInit init;

};
