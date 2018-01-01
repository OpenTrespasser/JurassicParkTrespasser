/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/Waves.hpp                                                 $
 * 
 * 35    98/10/06 18:54 Speter
 * Got rid of problematic mMaxAmplitude maintenance. Made bElementActive() a virtual function
 * accessible by CSimWave2D.
 * 
 * 34    98/05/03 23:32 Speter
 * Added Transfer() functions.
 * 
 * 33    98/05/01 15:52 Speter
 * Added SetNeighbours().
 * 
 * 32    98/04/29 13:13 Speter
 * Added support for integrating across models of differing resolutions. Removed i_boundary from
 * water constructor.
 * 
 * 31    98/03/19 22:25 Speter
 * Fixed bug; water now had proper integration speed when switching resolutions.
 * 
 * 30    98/03/17 18:24 Speter
 * Added ability for water to integrate in different resolutions in X,Y.
 * 
 * 29    98/03/13 13:46 Speter
 * Separated basic wave functionality into CWaveData base class. Can now #if out all 1D wave
 * classes.
 * 
 * 28    98/03/10 21:12 Speter
 * Added constructors for converting an existing wave model to a new resolution. Removed
 * pa2mDepth from CSimWater2D; now set in constructor  or UpdateDepth().
 * 
 * 27    97/12/16 16:37 Speter
 * Added rMaxAmplitude field to base class.
 * 
 * 26    97/11/08 18:14 Speter
 * Now store iStride in CSimWave2D as well.
 * 
 * 25    97/10/10 15:07 Speter
 * Removed bCOPY_TO switch and code.  Added per-row limits of water integration, replacing
 * per-element test.
 * 
 * 24    97/10/02 12:23 Speter
 * Restored boundary condition parameter.  Added bElementActive() function.
 * 
 * 23    8/21/97 1:52p Agrant
 * another bones session- tail added
 * 
 * 22    97-05-06 15:54 Speter
 * Updated naming for new CPArray2 conventions: [y][x].
 * 
 * 21    4/21/97 11:57a Agrant
 * Raptors walking!
 * 
 * 20    97/02/15 21:00 Speter
 * Now both element lengths are specified, but only one physical length.
 * 
 * 19    97/02/14 13:47 Speter
 * Reorganised some code and parameters between classes.  CSimWater2D now has default parameters
 * for water damping and restoring.  Moved ApplyBoundary() call to Iterate().  Added
 * experimental bCOPY_TO flag, which implements alternate boundary condition strategy.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_WAVES_HPP
#define HEADER_LIB_PHYSICS_WAVES_HPP

#include "Lib/Std/Array2.hpp"
#include "Lib/Transform/TransLinear.hpp"
#include <math.h>

// Some sort of physical measurement.
typedef float		TMeasure;		// Prefix: mr

typedef TMeasure	TMetres;		// Prefix: m
typedef TMeasure	TKilograms;		// Prefix: kg
typedef TMeasure	TSeconds;		// Prefix: s

const TMeasure		mrGRAVITY = 9.8;

#define VER_TEST_OBJECTS	0		// Compile 1-D and other test code.

//**********************************************************************************************
//
class CWaveData
//
// Prefix: wvd
//
// Provides parameters and basic functionality for wave simulation classes.
//
//**************************************
{
public:
	TMeasure			mrVelocity;			// Velocity of propagation.
	TMeasure			mrDamping;			// Damping factor.
	TMeasure			mrRestoring;		// Restoring force factor.
	TMeasure			mrGravity;			// Acceleration due to gravity 
											// (in negative amplitude direction).
	int					iBoundary;			// Boundary condition.
	TMetres				mInterval;			// Length of each element.

protected:

	TSeconds			sStepMax;			// Maximum time step usable.
	TSeconds			sStepPrev;			// Time step used last time.

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	//
	CWaveData
	(
		int i_boundary,					// Boundary condition: which derivative is 0.
										//	0 means endpoints are zero.
										//	1 means first derivative at endpoints is 0.
										//  2 means second derivative at endpoints is 0.
		TMetres m_interval,				// Size of wave element (m).
		TMeasure mr_velocity,			// Velocity of propagation (m/s).
		TMeasure mr_damping = 0,		// Damping factor (1/s).
		TMeasure mr_restoring = 0,		// Leveling force factor (1/ss).
		TMeasure mr_gravity = 0			// Acceleration due to gravity (m/ss).
	);
	//
	// Initialises fields, allocates arrays, and sets elements to 0.
	//
	//**************************************

	//******************************************************************************************
	//
	CWaveData
	(
		const CWaveData& wave,
		TMetres m_interval				// Size of wave element (m).
	);
	//
	// Copies given wave data, except for m_interval.
	//
	//**************************************

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual void SwapPrev() = 0;
	//
	// Swaps the current wave-form with the previous one.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual void CopyPrev() = 0;
	//
	// Copies the current wave-form to the previous one, thus setting up a situation of
	// starting from rest.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual void Step
	(
		TSeconds s_step				// Size of time step.
	);
	//
	// Recalculates the wave values for the time step.
	//
	// Notes:
	//		This won't do anything unless some of the wave elements have been given non-zero values.
	//		This can be done directly, before calling Step().
	//
	//**********************************

	//******************************************************************************************
	//
	static void Step
	(
		CPArray<CWaveData*> pawvd,
		TSeconds s_step
	);
	//
	// Integrate all objects at once, in an interleaved fashion.
	// This allows neighbours to transfer their data between each integration.
	//
	//**********************************

protected:

	virtual void Iterate(TSeconds s_step_cur) = 0;

	virtual void ApplyBoundary() = 0;

	virtual void SetMaxStep();

	friend static void Step(CPArray<CWaveData*> pawvd, TSeconds s_step);
};

#if VER_TEST_OBJECTS

//**********************************************************************************************
//
class CSimWave: public CWaveData
//
// Prefix: wv
//
//	A class for simulating a 1-dimensional wave.
//
//**************************************
{
public:
	TMetres				mLength;			// Length of 1-dimensional wave medium.

	CAArray<TMetres>	pamAmplitude;		// Array of amplitude values for wave.
											// The length of the array is the number of discrete
											// values for which the amplitude is maintained.
protected:

	CAArray<TMetres>	pamAmplitudePrev;	// Previous array of height values.

public:

	//******************************************************************************************
	//
	CSimWave
	(
		int i_elements,					// Number of elements along length.
		int i_boundary,					// Boundary condition: which derivative is 0.
										//	0 means endpoints are zero.
										//	1 means first derivative at endpoints is 0.
										//  2 means second derivative at endpoints is 0.
		TMetres m_length,				// Length of wave medium (m).
		TMeasure mr_velocity,			// Velocity of propagation (m/s).
		TMeasure mr_damping = 0,		// Damping factor (1/s).
		TMeasure mr_restoring = 0,		// Leveling force factor (1/ss).
		TMeasure mr_gravity = 0			// Acceleration due to gravity (m/ss).
	);
	//
	// Initialises fields, allocates arrays, and sets elements to 0.
	//
	//**************************************

	//******************************************************************************************
	//
	CSimWave
	(
		int i_elements,					// Number of elements along length.
		const CSimWave& wv				// Object to copy settings from.
	);
	//
	// Copies settings from wv, constructs at new resolution.
	//
	//**************************************

protected:

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual void SwapPrev();

	virtual void CopyPrev();

	virtual void Iterate(TSeconds s_step_cur);

	virtual void ApplyBoundary();
};

//**********************************************************************************************
//
class CSimWaveVar: public CSimWave
//
// Prefix: wvv
//
//	A class for simulating a 1-dimensional wave, with variable velocity.
//
//
//**************************************
{
public:
	CAArray<TMeasure>	pamrVelocity;	// Wave velocity at each element.


	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	//
	CSimWaveVar
	(
		int i_elements,					// Number of elements along length.
		int i_boundary,					// Boundary condition: which derivative is 0.
										//	0 means endpoints are zero.
										//	1 means first derivative at endpoints is 0.
										//  2 means second derivative at endpoints is 0.
		TMetres m_length,				// Length of wave medium (m).
		TMeasure mr_velocity,			// Default velocity of propagation (m/s).
		TMeasure mr_damping = 0,		// Damping factor (1/s).
		TMeasure mr_restoring = 0,		// Leveling force factor (1/ss).
		TMeasure mr_gravity = 0			// Acceleration due to gravity (m/ss).
	);
	//
	// Sets up as per CSimWave, and initialises all pamrVelocity elements to mr_velocity.
	//
	//**************************************

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void UpdateVelocity();
	//
	// Updates the simulation for new velocity values.
	// Must be called after changing the pamrVelocity array.
	//
	//**********************************

protected:

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual void Iterate(TSeconds s_step_cur);

};

//**********************************************************************************************
//
class CSimWater: public CSimWaveVar
//
// Prefix: wat
//
//	A class for simulating a 1-dimensional water wave, with variable depth.
//
//**************************************
{
public:
	CAArray<TMetres>	pamDepth;		// Water depth at each element.


	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	//
	CSimWater
	(
		int i_elements,					// Number of elements along length.
		TMetres	 m_length,				// Length of wave medium (m).
		TMetres  m_depth = 0,			// Default depth of water (m).
		TMeasure mr_damping = 0,		// Damping factor (1/s).
		TMeasure mr_restoring = 0,		// Leveling force factor (1/ss).
		TMeasure mr_gravity = mrGRAVITY	// Acceleration due to gravity (m/ss).
	);
	//
	//**************************************

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void UpdateDepth();
	//
	// Updates the simulation for new depth values.
	// Must be called after changing the pamDepth array.
	//
	//**********************************

protected:

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual void Iterate(TSeconds s_step_cur);
};

#endif

//**********************************************************************************************
//
class CSimWave2D: public CWaveData
//
// Prefix: wv2
//
//	A class for simulating a 2-dimensional wave, similar to CSimWave.
//
//**************************************
{
public:
	TMetres				mLength, mWidth;	// Length of waves in 2 dimensions.
	TMetres				mIntervalX, mIntervalY;
											// Separate intervals for optional
											// variant resolution.
	int					iHeight, iWidth,	// Dimensions of the simulation 
						iStride;			// (shared by all arrays herein).
	CAArray2<TMetres>	pa2mAmplitude;		// Array of amplitude values for wave.

protected:
	CAArray2<TMetres>	pa2mAmplitudePrev;	// Previous array of height values.
	CSimWave2D*			apwv2Neighbours[4];	// Neighbouring sims, if any.
											// Boundary conditions are set from these.
											// Index convention is same as for terrain:
											// 0: bottom, 1: right, 2: top, 3: left.

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	//
	CSimWave2D
	(
		int i_elements1,				// Number of elements along length and width.
		int i_elements2,
		int i_boundary,					// Boundary condition: which derivative is 0.
										//	0 means endpoints are zero.
										//	1 means first derivative at endpoints is 0.
		TMetres m_length,				// Size of wave medium (m).  
		TMetres m_width,
		TMeasure mr_velocity,			// Velocity of propagation (m/s).
		TMeasure mr_damping = 0,		// Damping factor (1/s).
		TMeasure mr_restoring = 0,		// Leveling force factor (1/ss).
		TMeasure mr_gravity = 0			// Acceleration due to gravity (m/ss).
	);
	//
	// Initialises fields, allocates arrays, and sets elements to 0.
	//
	//**************************************

	//******************************************************************************************
	//
	CSimWave2D
	(
		int i_elements1,				// Number of elements along length and width.
		int i_elements2,
		const CSimWave2D& wv2			// Object to copy settings and waves from.
	);
	//
	// Copies settings and waves from wv2, constructs with new resolution.
	//
	//**************************************

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SetNeighbour
	(
		int i_nabe,						// Which neighbour.
		CSimWave2D* pwv2				// Neighbouring sim, or 0 if none.
	);
	//
	// Sets it.
	//
	//**********************************

	//******************************************************************************************
	//
	void SetNeighbours
	(
		CSimWave2D* pwv2_0,				// Neighbouring sims for each side, or 0 if none.
		CSimWave2D* pwv2_1,
		CSimWave2D* pwv2_2,
		CSimWave2D* pwv2_3
	);
	//
	// Sets 'em.
	//
	//**********************************

	//******************************************************************************************
	//
	void Transfer
	(
		const CSimWave2D& wave2,		// Wave sim to transfer from.
		CRectangle<int> rci_this,		// Integer region of this wave to fill:
										// denotes starting and one-past-ending coords.
		CRectangle<int> rci_it			// Integer region of wave2 to transfer from.
	);
	//
	// Transfers the wave data, averaging and interpolating as needed.
	//
	//**********************************

	//******************************************************************************************
	//
	void Transfer
	(
		const CSimWave2D& wave2, 		// Wave sim to transfer from.
		CRectangle<> rc_this,			// Relative region of this wave to fill:
										// 0.0 refers to start of wave, 1.0 to end, both minus border.
		CRectangle<> rc_it				// Relative region of wave2 to transfer from.
	);
	//
	// Transfers the wave data, averaging and interpolating as needed.
	//
	//**********************************

	//******************************************************************************************
	//
	void CreateDisturbance
	(
		TMetres m_x, TMetres m_y,		// Centre of bump to make, in sim coords.
		TMetres m_radius,				// Radius of the bump.
		TMetres m_height,				// Height of the bump.
		bool b_add = false				// Whether to add disturbance (else set it).
	);
	//
	// Makes a hemispherical bump in the wave surface.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual bool bElementActive
	(
		int i_y, int i_x
	) const
	//
	// Returns:
	//		Whether there is an active element at this location.
	//
	//**********************************
	{
		return true;
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual void SwapPrev();

	virtual void CopyPrev();

protected:

	virtual void Iterate(TSeconds s_step_cur);

	virtual void ApplyBoundary();

	virtual void SetMaxStep();
};


//**********************************************************************************************
//
class CSimWaveVar2D: public CSimWave2D
//
// Prefix: wvv2
//
// A class for simulating a 2-dimensional wave, with variable velocity.
// Note: Gravity as a force acting on the elements is not supported in this class, 
// as it is being used to simulate incompressible fluids (CSimWater2D derived class).
// Also, the only boundary condition supported is 1.
//
//**************************************
{
protected:
	CAArray2<TMeasure>	pa2mrVelocityFactor;	// Wave velocity at each element.

public:
	struct SLimits
	// Prefix: lm
	{
		int iStart, iEnd;						// First and last (not last+1) elements to iterate.
	};

	SLimits lmRows;								// Limits of rows to iterate.
	CAArray<SLimits> palmColLimits;				// The limits of iteration for each row,
												// determined by pa2mrVelocityFactor.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	//
	CSimWaveVar2D
	(
		int i_elements1,				// Number of elements along length and width.
		int i_elements2,
		int i_boundary,					// Boundary condition: which derivative is 0.
		TMetres m_length,				// Size of wave medium (m).
		TMetres m_width,
		TMeasure mr_velocity,			// Default velocity of propagation (m/s).
		TMeasure mr_damping = 0,		// Damping factor (1/s).
		TMeasure mr_restoring = 0		// Leveling force factor (1/ss).
	);
	//
	// Sets up as per CSimWave2D, and sets velocity to mr_velocity everywhere.
	//
	//**************************************

	//******************************************************************************************
	//
	CSimWaveVar2D
	(
		int i_elements1,				// Number of elements along length and width.
		int i_elements2,
		const CSimWaveVar2D& wvv2		// Object to copy settings and waves from.
	);
	//
	// Copies settings and current wave values from wat2d, constructs object with new resolution
	// and depth.
	//
	//**************************************

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SetVelocity
	(
		TMeasure mr_velocity			// Velocity for entire sim.
	);
	//
	// Set all velocities to mr_velocity, and calls UpdateVelocity().
	//
	//**********************************

	//******************************************************************************************
	//
	void SetVelocity
	(
		int i_y, int i_x,				// Element to set.
		TMeasure mr_velocity			// Velocity at element.
	);
	//
	// Set the velocity value at i_y, i_x.
	//
	// Notes:
	//		Requires calling UpdateVelocity() after all velocities have been set.
	//
	//**********************************

	//******************************************************************************************
	//
	void UpdateVelocity();
	//
	// Updates the sim for new velocity values that have been set via SetVelocity().
	//
	//**********************************

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual void Iterate(TSeconds s_step_cur);

	virtual bool bElementActive(int i_y, int i_x) const
	{
		return pa2mrVelocityFactor[i_y][i_x] > 0;
	}

};

//**********************************************************************************************
//
class CSimWater2D: public CSimWaveVar2D
//
// Prefix: wat2d
//
//	A class for simulating a 2-dimensional water wave, with variable depth.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	//
	CSimWater2D
	(
		int i_elements1,				// Number of elements along length and width.
		int i_elements2,
		TMetres	m_length,				// Size of wave medium (m).
		TMetres m_width,
		TMetres m_depth = 0,			// Default depth of water (m).
		TMeasure mr_damping = 0.1,		// Damping factor (1/s).
		TMeasure mr_restoring = 25		// Leveling force factor (1/ss).
	);
	//
	//**************************************

	//******************************************************************************************
	//
	CSimWater2D
	(
		int i_elements1,				// Number of elements along length and width.
		int i_elements2,
		TMetres	m_length,				// Size of wave medium (m).
		TMetres m_width,
		CPArray2<TMetres> pa2m_depth,	// Array of depth values, spanning entire water's extent.
										// Must be at least as large as water array (minus 2 for borders);
										// values will be interpolated.
		TMeasure mr_damping = 0.1,		// Damping factor (1/s).
		TMeasure mr_restoring = 25		// Leveling force factor (1/ss).
	);
	//
	//**************************************

	//******************************************************************************************
	//
	CSimWater2D
	(
		int i_elements1,				// Number of elements along length and width.
		int i_elements2,
		const CSimWater2D& wat2d,		// Object to copy existing info from.
		CPArray2<TMetres> pa2m_depth	// Array of depth values.
										// Must be at least as large as water array;
										// values will be interpolated.
	);
	//
	// Copies settings and current wave values from wat2d, constructs object with new resolution
	// and depth.
	//
	//**************************************

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void UpdateDepth
	(
		TMetres m_depth					// Constant depth for water.
	);
	//
	// Updates the simulation for new depth values.
	//
	//**********************************

	//******************************************************************************************
	//
	void UpdateDepth
	(
		CPArray2<TMetres> pa2m_depth	// Array of depth values, spanning entire water's extent.
										// Must be at least as large as water array (minus 2 for borders);
										// values will be interpolated.
	);
	//
	// Updates the simulation for new depth values.
	//
	//**********************************
};

#endif
