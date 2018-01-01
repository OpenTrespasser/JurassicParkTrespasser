/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CreateParticle specialisations, and persistent particle generators.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/ParticleGen.hpp                                      $
 * 
 * 7     98/09/19 14:38 Speter
 * Changed handling of particle parameter randomisation, for greater control.
 * 
 * 6     98/09/17 3:31 Speter
 * pcrtFind now takes an alternate sound material for lookup.
 * 
 * 5     98/08/30 21:18 Speter
 * Made CParticleCycle simpler, w/o MI.
 * 
 * 4     98/08/28 19:51 Speter
 * Added default constructors. 
 * 
 * 3     98/08/21 18:01 Speter
 * Updated for new particle capabilities.
 * 
 * 2     98/08/19 2:38 Speter
 * Updated for many CParticle changes.
 * 
 * 1     98/08/17 17:31 Speter
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_PARTICLEGEN_HPP
#define HEADER_LIB_ENTITYDBASE_PARTICLEGEN_HPP

#include "Lib/Renderer/Particles.hpp"
#include "Lib/Transform/Transform.hpp"

//*********************************************************************************************
//
class CCreateParticleStatic: public CCreateParticles
//
// Prefix: crts
//
// Specifies a single static particle with a lifetime of one frame. Must be redrawn each frame.
//
//**************************************
{
public:

	//*********************************************************************************************
	CCreateParticleStatic
	(
		const CCreateParticles& crt,
		TReal r_size
	);
};

//*********************************************************************************************
//
class CParticleGenerator: public CCreateParticles
//
// Prefix: prtgen
//
// A persistent source of particles. Keeps on generating them; parameters changeable at will.
//
//**************************************
{
protected:

	CPlacement3<> p3Origin;				// The world origin of the generator.

public:

	//*********************************************************************************************
	CParticleGenerator()
	{
	}

	//*********************************************************************************************
	CParticleGenerator
	(
		const CCreateParticles& crt,
		const CPlacement3<>& p3
	);

	//*********************************************************************************************
	//
	void SetPlacement
	(
		const CPlacement3<>& p3
	)
	//
	//**********************************
	{
		p3Origin = p3;
	}

	//*********************************************************************************************
	//
	const CPlacement3<>& p3GetPlacement() const
	//
	//**********************************
	{
		return p3Origin;
	}

	//*********************************************************************************************
	//
	void Step
	(
		TSec s_step
	);
	//
	// Generates particles for a new time-step, using the min and max params as the ranges for
	// generated particles.
	//
	//**********************************

	//*********************************************************************************************
	//
	void Step
	(
		TSec s_step,
		float f_interp					// Interpolation value, normally 0..1.
	);
	//
	// Generates particles for a new time-step. Interpolates between the min and max params
	// by the scale parameter to produce the range max of particle params. Applies a standard
	// scale factor to produce the 
	//
	//**********************************
};

//*********************************************************************************************
//
class CCreateParticlesCycle: public CCreateParticles
//
// Prefix: crtc
//
// A specification of a cycling spurting source of particles.
// In this class, the min and max values of each parameter specify the max values at the
// limits of the spurting cycle. The randomness is applied by a constant variance for each 
// parameter.
//
//**************************************
{
public:
	TSec	sPeriod;					// Period of spurting.
	TSec	sDuty;						// Duration of actual spurt cycle.
										// range is (param * fMinRatio) .. param.

	float	fDecay;						// Exponential decay rate of bleeding per second.
	TSec	sLifetime;					// How long this spurter lasts.

public:

	//*********************************************************************************************
	CCreateParticlesCycle()
	{
	}

	//*********************************************************************************************
	CCreateParticlesCycle
	(
		const CCreateParticles& crt
	)
		: CCreateParticles(crt)
	{
	}
};

//*********************************************************************************************
//
class CParticleCycle: public CCreateParticlesCycle
//
// Prefix: pcl
//
// An persistent instance of a cycling spurting source of particles.
//
//**************************************
{
protected:

	CParticleGenerator	prtgenGenerator;	// The generator used for each frame of the cycle.
	TSec sAge;								// How long this generator has been alive.
	float fScale;							// Current scaling value for all params.

public:

	//*********************************************************************************************
	CParticleCycle()
	{
	}

	//*********************************************************************************************
	CParticleCycle
	(
		const CCreateParticlesCycle& crtc,
		float f_strength = 0.0f,
		const CPlacement3<>& p3 = p3Null
	);

	//*********************************************************************************************
	//
	bool bIsDone() const
	//
	//
	//**********************************
	{
		return sAge >= sLifetime;
	}

	//*********************************************************************************************
	//
	void SetPlacement(const CPlacement3<>& p3)
	//
	//**********************************
	{
		prtgenGenerator.SetPlacement(p3);
	}

	//*********************************************************************************************
	//
	void Step(TSec s_time);
	//
	//**********************************
};

typedef uint32 TSoundMaterial;

//******************************************************************************************
//
namespace NParticleMap
//
// Maintains a list of particle effects, and maps them to object materials.
// 
//**************************************
{
	//******************************************************************************************
	//
	void Reset();
	// 
	// Clears all entries.
	//
	//**********************************

	//******************************************************************************************
	//
	CCreateParticles* pcrtAddParticle
	(
		const CCreateParticles& crt
	);
	//
	// Adds a copy of this effect to the map, and returns the pointer to it.
	//
	//**********************************

	//******************************************************************************************
	//
	void AddMapping
	(
		CCreateParticles* pcrt, 
		TSoundMaterial smat
	);
	//
	// Adds an association between the effect and the material.
	// If smat == 0, the effect is set to the default effect.
	//
	//**********************************

	//******************************************************************************************
	//
	CCreateParticles* pcrtFind
	(
		TSoundMaterial smat,			// Material to search for.
		TSoundMaterial smat_alt = 0		// Alternate material to search, if not found.
	);
	//
	// Returns:
	//		The associated effect for the material, or the alternate material,
	//		or the default. In any case, may be 0.
	//
	//**********************************
};

#endif