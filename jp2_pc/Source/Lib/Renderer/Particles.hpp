/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * A particle system for blood and so on.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Particles.hpp                                            $
 * 
 * 22    98.09.30 10:37p Mmouni
 * Added bUseTerrainColour.
 * 
 * 21    9/24/98 1:45a Pkeet
 * Changed the colour of blood.
 * 
 * 20    98/09/19 14:47 Speter
 * Changed method of randomising particle params, for more control.
 * 
 * 19    98/09/17 3:38 Speter
 * Added particle priority, for resolving multiple effects.
 * 
 * 18    9/08/98 10:24p Pkeet
 * Added a dust particle type.
 * 
 * 17    98/08/28 19:49 Speter
 * Added CParticlesCreate::fIntensity().
 * 
 * 16    98/08/25 19:04 Speter
 * Replaced sStickTime with fLength parameter, which calculates former.
 * 
 * 15    8/21/98 11:06p Rwycko
 * Removed SParticlesParam text prop constructor; too much of a butt-pain with text-prop system.
 * Only CCreateParticles can be tp-constructed now.
 * 
 * 14    98/08/21 18:04 Speter
 * Added text prop loading. Moved fAccelStuck and fAccelUnstuck to SParticleParams. Added
 * fThreshold, bLogScale. Added default constructors.
 * 
 * 13    98/08/19 2:34 Speter
 * Changed iNumParticles to float; moved sStickTime and fSpreadRadians to SParticleParams, for
 * more general usage. Added scaling operators. Added Create() overload which takes an
 * interpolation and scale.
 * 
 * 12    8/18/98 1:31p Pkeet
 * Added 'SParticleParams' to this module and provided functions to interpolate parameters.
 * 
 * 11    8/18/98 12:36p Pkeet
 * Replace the create class in CParticles with 'CCreateParticles.'
 * 
 * 10    8/17/98 11:24p Pkeet
 * Reduced the specular blood colour.
 * 
 * 9     8/17/98 10:55p Pkeet
 * Added full support for solid particles.
 * 
 * 8     8/17/98 9:10p Pkeet
 * Virtualized the particle system. Added support for solid (non-fluid) particles.
 * 
 * 7     8/17/98 7:52p Pkeet
 * Changed the colour of blood.
 * 
 * 6     8/17/98 5:45p Pkeet
 * Added parameters for setting 'stick' time and acceleration while stuck and unstuck.
 * 
 * 5     8/17/98 4:51p Pkeet
 * Added specular values for fluids.
 * 
 * 4     8/16/98 9:47p Pkeet
 * Added in the full set of particle parameters.
 * 
 * 3     8/16/98 6:00p Pkeet
 * Added in the 'CCreateParticles' class and a derived class for blood.
 * 
 * 2     8/16/98 3:37p Pkeet
 * Added a reset for the particle system.
 * 
 * 1     8/15/98 9:43p Pkeet
 * Intial implementation.
 * 
 **********************************************************************************************/

#ifndef LIB_RENDERER_PARTICLES_HPP
#define LIB_RENDERER_PARTICLES_HPP


//
// Includes.
//
#include "Lib/Sys/Timer.hpp"
#include "Lib/Transform/Vector.hpp"
#include "Lib/View/Colour.hpp"


//
// Forward declarations.
//
class CParticleList;
class CPipelineHeap;
class CCamera;
class CTexture;
class CTextureColours;
class CParticleBase;
class CParticleSolid;
class CParticleFluid;

class CLoadWorld;
class CHandle;
class CValueTable;

//
// Constants.
//

// The colour of blood.
const CColour clrBlood = CColour(98, 2, 2);

// Specular value for blood.
const CColour clrBloodSpecular = CColour(122, 2, 2);

// The colour of water.
const CColour clrWater = CColour(160, 160, 228);

// Specular value for water.
const CColour clrWaterSpecular = CColour(208, 208, 240);


//
// Enumerations.
//

//*********************************************************************************************
//
enum EParticleType
//
// Type of particle.
//
// Prefix: ept
//
//**************************
{
	eptSolid,	// Ordinary solid particle.
	eptDust,	// Dust particles.
	eptFluid	// Fluid particle, for blood drops and water splashes.
};


//
// Structure definitions.
//
	
//*****************************************************************************************
//
struct SParticleParams
//
// Parameters for particles.
//
// Prefix: pp
//
//**************************
{
	float fNumParticles;	// Number of particles to generate per 'Create' call.
	float fSpinRadPerSec;	// Spin in radians per second.
	float fSize;			// Minimum size of a particle's cylindrical radius (metres).
	float fLength;			// Approximate length of fluid particles.
	float fVelocity;		// The lowest velocity for a particle upon creation (m/s).
	TSec  sLifespan;		// Lifespan in seconds.
	float fAccelStuck;		// Acceleration while the particle is stuck.
	float fAccelUnstuck;	// Acceleration after the particle becomes unstuck.
	float fSpreadRadians;	// The angle formed by the axis of the cone and the side
							// of the cone.

	float fThreshold;		// An associated strength associated with this set of params.
							// Not used by particle system, but used elsewhere to map
							// input energy, etc. to particle params.


	//*************************************************************************************
	//
	// Constructors.
	//

	//*************************************************************************************
	SParticleParams();

	//*************************************************************************************
	//
	// Member functions.
	//

	//*************************************************************************************
	//
	SParticleParams ppInterpolate
	(
		const SParticleParams& pp,	// Parameters to interpolate to.
		float f_t
	) const;
	//
	// Interpolates between this set of parameters and the parameters specified by the object.
	//
	//**************************

	//*************************************************************************************
	//
	SParticleParams operator *(float f_t) const;
	//
	// Returns a 'pp' object whose params are scaled by the value.
	//
	//**************************

	//*************************************************************************************
	//
	SParticleParams ppRandom
	(
		const SParticleParams& pp	// Parameters to interpolate to.
	) const;
	//
	// Returns random values between this set of parameters and the parameters specified by
	// the object.
	//
	//**************************

};


//
// Class definitions.
//
	
//*****************************************************************************************
//
class CCreateParticles
//
// Prefix: crp
//
// Particle creation specification and activator.
//
// Notes:
//		Descendants of this class should always use this class's data members only.
//
//**************************
{
public:

	EParticleType	eptParticleType;	// Particle type.
	CColour			clrColour;			// Colour of the particle.
	bool			bUseTerrainColour;	// Use the terrain base colour for the particle.
	bool			bLogScale;			// Whether scaling of effect is logarithmic.
	float			fTopMin;			// Max strength of effect for min input.
										// Max strength of effect for max input is always 1.
										// Min strength for either is always 0.
	int				iPriority;			// Precedence given when multiple effects possible
										// (higher = more likely).
	SParticleParams ppMin;
	SParticleParams ppMax;

protected:

	CTexture* ptexTexture;		// Texture to be used by generated particles.
	TSec      sBeginAge;		// Starting age of the particle.

public:

	//*************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CCreateParticles(CColour clr = clrBlack);

	//*************************************************************************************
	CCreateParticles
	(
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable		// Value Table.
	);

	//*************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Create
	(
		CVector3<> v3_origin,
		CDir3<> d3_dir,
		TSec s_period = 1.0		// Time scaling for number of particles created.
								// Directly scales the number of particles created.
								// To simply create a number between the ranges specified,
								// use the default argument.
	);
	//
	// Creates the specified number and type of particles.
	//
	//**************************

	//*****************************************************************************************
	//
	void Create
	(
		CVector3<> v3_origin,
		CDir3<> d3_dir,
		TSec s_period,
		float f_strength			// Strength of event (relative to fThreshold params).
	);
	//
	// Creates the specified number and type of particles.
	// Interpolates between ppMin and ppMax to produce max values of particle params,
	// according to f_strength and fThreshold.
	//
	//**************************

	//*************************************************************************************
	//
	float fIntensity
	(
		float f_strength		// Strength of event.
	) const;
	//
	// Returns the intensity (interpolation param) to use for the given strength, based
	// on the fThreshold params.
	//
	//**************************

	//*************************************************************************************
	//
	CCreateParticles crtInterpolate
	(
		const CCreateParticles& crt,	// Parameters to interpolate to.
		float f_t
	) const;
	//
	// Returns a 'crt' object representing an interpolation from this object to the object
	// specified in the parameters.
	//
	//**************************

	//*************************************************************************************
	//
	CCreateParticles operator *(float f_t) const;
	//
	// Returns a 'crt' object whose min and max params are scaled by the value.
	//
	//**************************

private:

	class CPriv;
	friend class CPriv;
	friend class CParticles;
	friend class CParticleBase;
	friend class CParticleFluid;
	friend class CParticleSolid;
	friend class CParticleDust;

};


//*********************************************************************************************
//
class CParticles
//
// Object encapsulates all the functionality of maintaining lists of moving particles.
//
// Prefix: prts
//
//**************************
{
private:

	CParticleList*   pprtParticleList;	// List of active particles.
	CTextureColours* ptexcolTextures;	// List of colours used.
	TSec             sPreviousStep;		// Time of the last step.

public:

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	// Default constructor.
	CParticles();

	// Destructor.
	~CParticles();


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Reset
	(
	);
	//
	// Clears the particle system of particles.
	//
	//**************************

	//*****************************************************************************************
	//
	void Add
	(
		const CCamera& cam,
		CPipelineHeap& rplhHeap
	);
	//
	// Adds particles to the renderer.
	//
	//**************************

	//*****************************************************************************************
	//
	void Update
	(
		TSec sec
	);
	//
	// Updates the particle system based on a unit of time.
	//
	//**************************

private:

	friend class CParticleList;
	friend class CCreateParticles;

};


//
// Global variable declarations.
//

// A global instance of a particle system.
extern CParticles Particles;


#endif LIB_RENDERER_PARTICLES_HPP
