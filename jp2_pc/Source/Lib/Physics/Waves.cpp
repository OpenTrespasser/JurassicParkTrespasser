/***********************************************************************************************
 *
 * Implementation of Waves.cpp
 *
 * Bugs:
 *		Wave propagation in deep water is not wavelength-dependent.
 *		Fix border transition delay.
 *		Reproduce and fix water blowup due to border transitions (occurs when res differs).
 *		When bailing out of integration due to time-out, not all bodies have had equal
 *		 integration time.
 *
 * To do:
 *		Optimise up/down-sampling in CSimWave2D.  Do proper up-sampling, not simple lerp.
 *		Allow depth in UpdateDepth() to be up-sampled.
 *		Speed up and improve quality of CreateDisturbance. 
 *		Make a CreateDisturbance which takes an arbitrarily oriented box.
 *
 *		Change current/previous amplitude arrays to value/velocity.
 *		Use higher-order integration to reduce # iterations required.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/Waves.cpp                                                 $
 * 
 * 65    98/10/09 0:30 Speter
 * Clamp disturbance radius to abs world value.
 * 
 * 64    10/07/98 8:37a Jpaque
 * removed build breaking dout
 * 
 * 63    98/10/06 18:56 Speter
 * Fixed water blowup by more correctly determining max step size. Added a bunch of asserts. Got
 * rid of problematic mMaxAmplitude maintenance, replaced by actual check of current wave height
 * in CreateDisturbance. No longer add disturbances to inactive elements. Tweaked disturbance
 * appearance by enforcing a min radius.
 * 
 * 62    9/02/98 5:26p Asouth
 * removed 'short' from jle that was more than 128 bytes (223, under MW)
 * 
 * 61    98/08/31 19:04 Speter
 * Disabled "Disturb" stat.
 * 
 * 60    8/26/98 11:48p Asouth
 * loop vars moved out of loop scope into the block
 * 
 * 59    98/08/25 19:06 Speter
 * Clamp impulses more correctly.
 * 
 * 58    98/05/27 4:06 Speter
 * On second thought, make it 0.5 m.
 * 
 * 57    98/05/27 3:52 Speter
 * Do not apply disturbance if model amplitude is too high.
 * 
 * 56    5/13/98 4:43p Pkeet
 * Commented out asserts.
 * 
 * 55    98/05/03 23:34 Speter
 * Added Transfer() functions; constructors now call them. Sped up down-sampling. Added Asserts
 * for down-sampling ratios. Ignore corner elements in border transfers. Ignore borders in
 * UpdateDepth().
 * 
 * 54    98/05/01 15:55 Speter
 * Added SetNeighbours(). Updated to do's. 
 * 
 * 53    98/04/29 13:14 Speter
 * Added support for integrating across models of differing resolutions. Removed i_boundary from
 * water constructor.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Waves.hpp"

#include "Lib/Sys/Timer.hpp"
#include "Lib/Transform/Vector.hpp"
#include "Lib/Math/FastSqrt.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "AsmSupport.hpp"

#include <math.h>
#include <memory.h>

#define VER_LIMITS			1			// Track integration limits for each row.
#define VER_ANISOTROPIC_RES	0			// X and Y resolutions differ.

// To do: re-write ASM, then remove this!
#if VER_ANISOTROPIC_RES
	#define VER_ASM			0
#endif

const TMetres  mMAX_AMPLITUDE	= 0.4;	// Clamp the amplitude for disturbances.
const TSeconds sMAX_INTEGRATION	= 0.02;	// Maximum time we allow for integration, to clamp positive feedback.
const TMetres  mMIN_DISTURB_RADIUS	= 0.2;	// Minimum radius for disturbances to avoid checkerboards.


CProfileStat		psWaterStep("Water Step", &proProfile.psRender, Set(epfSEPARATE));
static CProfileStat		psWaterIntegration("Water Integ", &psWaterStep);
static CProfileStat		psDisturb("Disturb", &proProfile.psStep, Set(epfHIDDEN));

//
//	The behaviour of the wave is derived from the basic wave equation, to which we add
//	terms for gravity, and damping, and a volume restoring factor:
//
//		d²z/dt² = v² d²z/dx² - f dz/dt - r z - g
//
//			z = wave height, t = time, x = distance, v = wave velocity, 
//			f = damping factor, r = restoring factor, g = gravity acceleration.
//
//	The gravity term can be removed, because in combination with the restoring factor,
//	it produces only a constant offset to z.
//
//  Using the following derivative approximations (accurate to second order),
//	for step value dt:
//
//		dz/dt   = ( z - z(-dt) ) / dt
//		d²z/dt² = ( z(+dt) - 2 z + z(-dt) ) / dt²
//
//	We arrive at the following relations:
//
//		( z(+dt) - 2 z + z(-dt) ) / dt² = v² ( z(+dx) - 2 z + z(-dx) ) / dx²
//										- f ( z - z(-dt) ) / dt
//										- r z
//
//		z(+dt) - 2 z + z(-dt) = v² dt²( z(+dx) - 2 z + z(-dx) ) / dx²
//							  - f dt ( z - z(-dt) )
//							  - r z dt²
//
//		z(+dt) = v² dt²( z(+dx) - 2 z + z(-dx) ) / dx²
//			   - f dt ( z - z(-dt) )
//			   - r z dt²
//			   + 2 z - z(-dt) 
//
//			   = (2 - 2 v² dt² / dx² - f dt - r dt²) z
//			   + (v² dt² / dx²) ( z(+dx) + z(-dx) )
//			   + (f dt - 1) z(-dt)
//
//	Thus, to calculate the wave at any point requires the value of that point and its neighbours
//	one step ago, and the value of that point two steps ago.
//
//	By adding appropriate initial and boundary conditions, we're all set.
//
//	*** Refinement:
//
//	If, in order to support real-time integration, two different step values, ds and dt, 
//	are used for subsequent steps, then
//
//		dz/dt   = ( z - z(-ds) ) / ds
//		d²z/dt² = ( (z(+dt) - z) / dt - (z - z(-ds)) / ds ) / ((ds+dt)/2)
//				= -2 z/(ds dt)  +  2 z(+dt) / (dt(ds+dt))  +  2 z(-ds) / (ds(ds+dt))
//
//		- 2 z/(ds dt)  +  2 z(+dt)/(dt(ds+dt))  +  2 z(-ds) / (ds(ds+dt))
//										= v² ( z(+dx) - 2 z + z(-dx) ) / dx²
//										- f ( z - z(-ds) ) / ds
//										- r z
//
//		2 z(+dt) / (dt(ds+dt)) = v² ( z(+dx) - 2 z + z(-dx) ) / dx²
//							  - f ( z - z(-ds) ) / ds
//							  - r z
//							  + 2 z / (ds dt)  -  2 z(-ds) / (ds(ds+dt))
//
//							  = (2/(ds dt) - 2 v²/dx² - f/ds - r) z
//							  + v²/dx² ( z(+dx) + z(-dx) )
//							  + (f/ds - 2/(ds(ds+dt))) z(-ds)
//
//		z(+dt) = dt(ds+dt)/2 (
//					(2/(ds dt) - 2 v²/dx² - f/ds - r) z
//				   + v²/dx² ( z(+dx) + z(-dx) )
//				   + (f/ds - 2/(ds(ds+dt))) z(-ds)
//				 )
//
//	*** 2-dimensional version:
//
//		2 z(+dt) / (dt(ds+dt)) =   v² ( z(+dx) - 2 z + z(-dx) ) / dx²
//								 + v² ( z(+dy) - 2 z + z(-dy) ) / dy²
//							     - f (z - z(-ds)) / ds
//							     - r z
//							     + 2 z/(ds dt) - 2 z(-ds) / (ds(ds+dt))
//
//							   = (2/(ds dt) - 2 v²/dx² - 2 v²/dy² - f/ds - r) z
//							     + v²/dx² ( z(+dx) + z(-dx) )
//							     + v²/dy² ( z(+dy) + z(-dy) )
//							     + (f/ds - 2/(ds(ds+dt))) z(-ds)
//
//		z(+dt) = dt(ds+dt)/2 (
//					(2/(ds dt) - 2 v²/dx² - 2 v²/dy² - f/ds - r) z
//				   + v²/dx² ( z(+dx) + z(-dx) )
//				   + v²/dy² ( z(+dy) + z(-dy) )
//				   + (f/ds - 2/(ds(ds+dt))) z(-ds)
//				 )
//
//	*** Forward Euler method:
//
//		z\tt = v² (z\xx + z\yy) - f z\t - r z
//
//			 ~= v² ( (z(+dx) - 2 z + z(-dx)) / dx² + (z(+dy) - 2 z + z(-dy)) / dy² )
//			    - f z\t - r z
//			 = (-2v²/dx² -2v²/dy² - r) z
//			   + v²/dx² ( z(+dx) + z(-dx) )
//			   + v²/dy² ( z(+dy) + z(-dy) )
//			   - f z\t
//
//		z   += z\t dt
//		z\t += z\tt dt
//
//	*** A funny substitution:
//		z := u w, u := v², z\t = u w\t, z\tt = u w\tt
//		z\x = u w\x + u\x w
//		z\xx = u w\xx + 2 u\x w\x + u\xx w
//
//		The wave equation:
//
//		w\tt u = v² (w\xx u + 2 w\x u\x + w u\xx + w\yy u + 2 w\y u\y + w u\yy) - f w\t u - r w u
//		w\tt   =    (w\xx u + 2 w\x u\x + w u\xx + w\yy u + 2 w\y u\y + w u\yy) - f w\t - r w 
//
//		w\tt u = (-4v²/dx² - r) u w
//			     + v²/dx² u ( w(+dx) + w(-dx) + w(+dy) + w(-dy) )
//			     - f u w\t
//		w\tt u = (-4/dx² - r u) w
//			     + 1/dx² ( w(+dx) + w(-dx) + w(+dy) + w(-dy) )
//			     - f u w\t
//

	//******************************************************************************************
	//
	void CopyInterp
	(
		CPArray2<TReal> pa2_dest,
		CPArray2<TReal> pa2_source
	)
	//
	//
	//**********************************
	{
		Assert(pa2_dest.iWidth == 1);
		Assert(pa2_source.iWidth == 1);

		if (pa2_source.iHeight >= pa2_dest.iHeight)
		{
			// Down-convert. Perform integral averaging, ignoring any remainder.
			int i_avg = pa2_source.iHeight / pa2_dest.iHeight;
			Assert(i_avg > 0);

			// Make sure we aren't ignoring too many elements.
			Assert(pa2_source.iHeight % pa2_dest.iHeight < Max(pa2_source.iHeight/8, 8));

			TReal r_scale = 1.0 / i_avg;

			for (int i = 0; i < pa2_dest.iHeight; i++)
			{
				// Find avg values.
				TReal r_val = 0.0;
				for (int i_d = 0; i_d < i_avg; i_d++)
					r_val += pa2_source[i*i_avg + i_d][0];
				pa2_dest[i][0] = r_val * r_scale;
			}
		}
		else
		{
			// Up-convert. Perform linear interpolation.
			float f_terp = float(pa2_source.iHeight - 1) / (pa2_dest.iHeight - 1);

			pa2_dest[0][0] = pa2_source[0][0];
			for (int i = 1; i < pa2_dest.iHeight-1; i++)
			{
				float f_old = i * f_terp;
				int	  i_old = iPosFloatCast(f_old);
				float f_rem = f_old - float(i_old);

				pa2_dest[i][0] = 
					pa2_source[i_old][0] * (1.0 - f_rem) +
					pa2_source[i_old+1][0] * f_rem;
			}
			pa2_dest(-1)[0] = pa2_source(-1)[0];
		}
	}

	//******************************************************************************************
	inline bool bValidAmp(TMetres m_amp)
	{
		return Abs(m_amp) < 10.0f;
	}

//**********************************************************************************************
//
// class CWaveData implementation.
//

	//******************************************************************************************
	CWaveData::CWaveData
	(
		int i_boundary,
		TMetres m_interval,
		TMeasure mr_velocity,
		TMeasure mr_damping,
		TMeasure mr_restoring,
		TMeasure mr_gravity
	)
	{
		Assert(bWithin(i_boundary, 0, 2));
		Assert(m_interval > 0);
		Assert(mr_velocity > 0);
		Assert(mr_damping >= 0);
		Assert(mr_restoring >= 0);

		// Note: gravity is allowed to be negative or 0.

		iBoundary		= i_boundary;
		mInterval		= m_interval;
		mrVelocity		= mr_velocity;
		mrDamping		= mr_damping;
		mrRestoring		= mr_restoring;
		mrGravity		= mr_gravity;

		SetMaxStep();
	}

	//******************************************************************************************
	CWaveData::CWaveData
	(
		const CWaveData& wave,
		TMetres m_interval
	)
	{
		// Copy data, then override mInterval.
		*this = wave;

		Assert(m_interval > 0);
		mInterval = m_interval;

		SetMaxStep();
	}

	//******************************************************************************************
	void CWaveData::Step(TSeconds s_step)
	{
		CLArray(CWaveData*, papwvd, 1);
		papwvd[0] = this;
		Step(papwvd, s_step);
	}

	//******************************************************************************************
	void CWaveData::Step(CPArray<CWaveData*> papwvd, TSeconds s_step)
	{
		Assert(s_step >= 0);

		// Find min sStepMax of all nodes.
		TSec s_step_max = 100;
		forall (papwvd, CPArray<CWaveData*>, ppwvd)
		{
			SetMin(s_step_max, (*ppwvd)->sStepMax);
		}

		// Return if the step is too tiny to perform
		if (s_step < s_step_max / 1000)
			return;

		//
		// Since we have a maximum step size, figure out the number of iterations needed to
		// integrate over s_step.  Then, use the same step size for each iteration.
		//

		int i_iterations = iPosFloatCast(s_step / s_step_max) + 1;
		TMeasure s_step_cur = s_step / i_iterations;

		// Track actual time taken for integration, because we max it.
		CTimer tmr;

		CCycleTimer ctmr;

		// Also count actual time integrated for, in case we break out early.
		s_step = 0;

		for (int i = 0; i < i_iterations; i++)
		{
			CWaveData** ppwvd;
			for (ppwvd = papwvd; ppwvd < papwvd.end(); ++ppwvd)
			{
				CWaveData* pwvd = *ppwvd;
				//
				// See if this guy has to integrate this step.
				// Each model's integration step must be a multiple of the min step, to keep in sync.
				//
				int i_multiple = iPosFloatCast(pwvd->sStepMax / s_step_cur);
				Assert(i_multiple > 0);
				if (i % i_multiple == 0 || i == i_iterations-1)
				{
					// Do the border transfers.
					pwvd->ApplyBoundary();
				}
			}

			for (ppwvd = papwvd; ppwvd < papwvd.end(); ++ppwvd)
			{
				CWaveData* pwvd = *ppwvd;
				//
				// See if this guy has to integrate this step.
				// Each model's integration step must be a multiple of the min step, to keep in sync.
				//
				int i_multiple = iPosFloatCast(pwvd->sStepMax / s_step_cur);
				Assert(i_multiple > 0);
				if (i % i_multiple == 0 || i == i_iterations-1)
				{
					// It's time for you, man.
					int i_multiple_now = Min(i_multiple, i_iterations-i);
					TSec s_step_now = i_multiple_now * s_step_cur;

					psWaterIntegration.Add(0, 1);
					pwvd->Iterate(s_step_now);

					// Make PrevAmplitude the new current wave.
					pwvd->SwapPrev();
					pwvd->sStepPrev = s_step_now;
				}
			}

			s_step += s_step_cur;

			if (tmr.sTime() > sMAX_INTEGRATION)
				break;
		}

		psWaterIntegration.Add(ctmr());
	}

	//******************************************************************************************
	//
	void CWaveData::SetMaxStep()
	//
	// Set the maximum time step value based on the velocity of propagation.
	//
	//**********************************
	{
		sStepMax = sStepPrev = 0.7 * mInterval / mrVelocity;
	}

#if VER_TEST_OBJECTS

//**********************************************************************************************
//
// class CSimWave implementation.
//

	//******************************************************************************************
	CSimWave::CSimWave
	(
		int i_elements,
		int i_boundary,
		TMetres m_length,
		TMeasure mr_velocity,
		TMeasure mr_damping,
		TMeasure mr_restoring,
		TMeasure mr_gravity
	)
		: CWaveData(i_boundary, m_length / i_elements, mr_velocity, mr_damping, mr_restoring, mr_gravity),
		mLength(m_length),
		pamAmplitude(i_elements), pamAmplitudePrev(i_elements)
	{
		Assert(i_elements > 2);
		Assert(m_length > 0);

		pamAmplitude.Fill(0);
		pamAmplitudePrev.Fill(0);
	}

	//******************************************************************************************
	CSimWave::CSimWave(int i_elements, const CSimWave& wv)
		: CWaveData(wv, wv.mLength / i_elements),
		mLength(wv.mLength),
		pamAmplitude(i_elements), pamAmplitudePrev(i_elements)
	{
		Assert(i_elements > 2);
		Assert(mLength > 0);

		pamAmplitude.Fill(0);
		pamAmplitudePrev.Fill(0);
	}

	//******************************************************************************************
	void CSimWave::SwapPrev()
	{
		Swap(pamAmplitude.atArray, pamAmplitudePrev.atArray);
	}

	//******************************************************************************************
	void CSimWave::CopyPrev()
	{
		memcpy(pamAmplitudePrev, pamAmplitude, pamAmplitudePrev.uLen * sizeof(*pamAmplitudePrev));
	}

// protected:

	//******************************************************************************************
	//
	void CSimWave::Iterate
	(
		TSeconds s_step_cur
	)
	//
	// Perform a single iteration of the wave update, using values in pamAmplitude and 
	// pamAmplitudePrev, and placing new values in pamAmplitudePrev.
	//
	//**********************************
	{
		// Calculate coefficients for each iteration.
		TMeasure mr_factor	= mrVelocity / mInterval;
		mr_factor *= mr_factor;

		TMeasure s_step_sqr = s_step_cur * (s_step_cur + sStepPrev) / 2;

		// Coefficient for current amplitude.
		TMeasure mr_a	= 2.0 / s_step_cur / sStepPrev 
						- 2.0 * mr_factor 
						- mrDamping / sStepPrev
						- mrRestoring;
		mr_a *= s_step_sqr;
		Assert(mr_a > 0);

		// Coefficient for neighboring amplitudes.
		TMeasure mr_x	= mr_factor * s_step_sqr;

		// Coefficient for previous amplitude.
		TMeasure mr_t	= mrDamping * s_step_sqr / sStepPrev
						- s_step_cur / sStepPrev;

		// Constant term.
		TMeasure mr_c	= - mrGravity * s_step_sqr;

		// Iterate through the internal points.
		// We store the new wave values in pamAmplitudePrev, and then swap the arrays.
		for (uint u = 1; u < pamAmplitude.uLen-1; u++)
		{
			pamAmplitudePrev[u] = mr_a * pamAmplitude[u]
								 + mr_x * (pamAmplitude[u-1] + pamAmplitude[u+1])
								 + mr_t * pamAmplitudePrev[u]
								 + mr_c;
		}
	}

	//******************************************************************************************
	//
	void CSimWave::ApplyBoundary()
	//
	// Enforce the boundary condition on the endpoints of pamAmplitude.
	//
	//**********************************
	{
		switch (iBoundary)
		{
			case 0:
				// Fixed endpoints.
				break;
			case 1:
				// Zero first derivative at endpoints.
				pamAmplitude[0] = pamAmplitude[1];
				pamAmplitude(-1) = pamAmplitude(-2);
				break;
			case 2:
				// Zero second derivative at endpoints.
				pamAmplitude[0] = 2 * pamAmplitude[1] - pamAmplitude[2];
				pamAmplitude(-1) = 2 * pamAmplitude(-2) - pamAmplitude(-3);
				break;
			default:
				Assert(false);
		}
	}

//**********************************************************************************************
//
// class CSimWaveVar implementation.
//

	//******************************************************************************************
	CSimWaveVar::CSimWaveVar
	(
		int i_elements,					// Number of elements along length.
		int i_boundary,					// Boundary condition: which derivative is 0.
										//	0 means endpoints are zero.
										//	1 means first derivative at endpoints is 0.
										//  2 means second derivative at endpoints is 0.
		TMetres m_length,				// Length of wave medium (m).
		TMeasure mr_velocity,			// Default velocity of propagation (m/s).
		TMeasure mr_damping,			// Damping factor (1/s).
		TMeasure mr_restoring,			// Leveling force factor (1/ss).
		TMeasure mr_gravity				// Acceleration due to gravity (m/ss).
	)
		: CSimWave(i_elements, i_boundary, m_length, mr_velocity, mr_damping, mr_restoring, mr_gravity),
		pamrVelocity(i_elements)
	{
		pamrVelocity.Fill(mr_velocity);
	}

// protected:

	//******************************************************************************************
	void CSimWaveVar::Iterate(TSeconds s_step_cur)
	{
		// Calculate coefficients for each iteration.
		TMeasure s_step_sqr = s_step_cur * (s_step_cur + sStepPrev) / 2;

		// Coefficient for previous amplitude.
		TMeasure mr_t	= mrDamping * s_step_sqr / sStepPrev
						- s_step_cur / sStepPrev;

		// Constant term.
		TMeasure mr_c	= - mrGravity * s_step_sqr;

		// Iterate through the internal points.
		// We store the new wave values in pamAmplitudePrev, and then swap the arrays.
		for (uint u = 1; u < pamAmplitude.uLen-1; u++)
		{
			TMeasure mr_factor	= pamrVelocity[u] / mInterval;
			mr_factor *= mr_factor;

			// Coefficient for current amplitude.
			TMeasure mr_a	= 2.0 / s_step_cur / sStepPrev 
							- 2.0 * mr_factor 
							- mrDamping / sStepPrev
							- mrRestoring;
			mr_a *= s_step_sqr;
			Assert(mr_a > 0);

			// Coefficient for neighboring amplitudes.
			TMeasure mr_x	= mr_factor * s_step_sqr;

			pamAmplitudePrev[u] = mr_a * pamAmplitude[u]
								 + mr_x * (pamAmplitude[u-1] + pamAmplitude[u+1])
								 + mr_t * pamAmplitudePrev[u]
								 + mr_c;
		}
	}

	//******************************************************************************************
	void CSimWaveVar::UpdateVelocity()
	{
		// Set mrVelocity to maximum velocity.
		mrVelocity = 0;

		for (uint u = 0; u < pamrVelocity.uLen; u++)
		{
			Assert(pamrVelocity[u] > 0);
			SetMax(mrVelocity, pamrVelocity[u]);
		}

		// Recalculate sStepMax according to maximum velocity.
		SetMaxStep();
	}

//**********************************************************************************************
//
// class CSimWater implementation.
//

	//******************************************************************************************
	CSimWater::CSimWater
	(
		int i_elements,				// Number of elements along length.
		TMetres	 m_length,			// Length of wave medium (m).
		TMetres  m_depth,			// Default depth of water (m).
		TMeasure mr_damping,		// Damping factor (1/s).
		TMeasure mr_restoring,		// Leveling force factor (1/ss).
		TMeasure mr_gravity			// Acceleration due to gravity (m/ss).
	)
		: CSimWaveVar(i_elements, 1, m_length, 1.0, mr_damping, mr_restoring, mr_gravity),
		pamDepth(i_elements)
	{
		pamDepth.Fill(m_depth);
		UpdateDepth();
	}

	//******************************************************************************************
	void CSimWater::Iterate(TSeconds s_step_cur)
	{
		// Do the same thing as parent class.
		CSimWaveVar::Iterate(s_step_cur);

		// Then clamp the wave values to never go below the depth.
		for (uint u = 0; u < pamAmplitude.uLen; u++)
		{
			SetMax(pamAmplitudePrev[u], -pamDepth[u]);
		}
	}

	//******************************************************************************************
	void CSimWater::UpdateDepth()
	{
		for (uint u = 0; u < pamDepth.uLen; u++)
			pamrVelocity[u] = fSqrtEst(mrGRAVITY * Max(Min(pamDepth[u], mLength / 6.0), 0.001));
		UpdateVelocity();
	}

#endif // VER_TEST_OBJECTS

//**********************************************************************************************
//
// class CSimWave2D implementation.
//

	//******************************************************************************************
	CSimWave2D::CSimWave2D
	(
		int i_elements1, int i_elements2, int i_boundary, TMetres m_length, TMetres m_width,
		TMeasure mr_velocity, TMeasure mr_damping, TMeasure mr_restoring, TMeasure mr_gravity
	)
		//
		// Create the arrays with a manually specified stride equal to the width rounded up to
		// an even number.  This is optimal for 3DX.
		//
		: CWaveData(i_boundary, Min(m_length / i_elements1, m_width / i_elements2), 
					mr_velocity, mr_damping, mr_restoring, mr_gravity),
		mLength(m_length), mWidth(m_width),
		mIntervalY(m_length / i_elements1), mIntervalX(m_width / i_elements2),
		iHeight(i_elements1), iWidth(i_elements2), iStride((iWidth+1) & ~1),
		pa2mAmplitude(iHeight, iWidth, iStride),
		pa2mAmplitudePrev(iHeight, iWidth, iStride)
	{
		dout <<"Water create " <<iWidth <<"," <<iHeight <<"\n";
		pa2mAmplitude.Fill(0);
		pa2mAmplitudePrev.Fill(0);

		// No neighbours to start.
		for (int i_n = 0; i_n < 4; i_n++)
			apwv2Neighbours[i_n] = 0;
	}

	//******************************************************************************************
	CSimWave2D::CSimWave2D
	(
		int i_elements1, int i_elements2, const CSimWave2D& wave2
	)
		//
		// Create the arrays with a manually specified stride equal to the width rounded up to
		// an even number.  This is optimal for 3DX.
		//
		: CWaveData(wave2, Min(wave2.mLength / i_elements1, wave2.mWidth / i_elements2)),
		mLength(wave2.mLength), mWidth(wave2.mWidth),
		mIntervalY(mLength / i_elements1), mIntervalX(mWidth / i_elements2),
		iHeight(i_elements1), iWidth(i_elements2), iStride((i_elements2+1) & ~1),
		pa2mAmplitude(iHeight, iWidth, iStride),
		pa2mAmplitudePrev(iHeight, iWidth, iStride)
	{
		dout <<"Water convert " <<wave2.iWidth <<"," <<wave2.iHeight <<" to " <<iWidth <<"," <<iHeight <<"\n";

		// No neighbours to start.
		for (int i_n = 0; i_n < 4; i_n++)
			apwv2Neighbours[i_n] = 0;

		// Transfer data from wave2 to here.
		Transfer(wave2, 
			CRectangle<int>(1, 1, iWidth-2, iHeight-2), 
			CRectangle<int>(1, 1, wave2.iWidth-2, wave2.iHeight-2));

		// Now set borders correctly, in both arrays.
		SwapPrev();		ApplyBoundary();
		SwapPrev();		ApplyBoundary();
	}

	//******************************************************************************************
	void CSimWave2D::SwapPrev()
	{
		Swap(pa2mAmplitude.atArray, pa2mAmplitudePrev.atArray);
	}

	//******************************************************************************************
	void CSimWave2D::CopyPrev()
	{
		memcpy(pa2mAmplitudePrev, pa2mAmplitude, pa2mAmplitudePrev.iSize() * sizeof(*pa2mAmplitudePrev));
	}

	//******************************************************************************************
	void CSimWave2D::SetNeighbour(int i_nabe, CSimWave2D* pwv2)
	{
		Assert(bWithin(i_nabe, 0, 3));
		apwv2Neighbours[i_nabe] = pwv2;
	}

	//******************************************************************************************
	void CSimWave2D::SetNeighbours(CSimWave2D* pwv2_0, CSimWave2D* pwv2_1, CSimWave2D* pwv2_2, CSimWave2D* pwv2_3)
	{
		apwv2Neighbours[0] = pwv2_0;
		apwv2Neighbours[0] = pwv2_1;
		apwv2Neighbours[0] = pwv2_2;
		apwv2Neighbours[0] = pwv2_3;
	}

	//******************************************************************************************
	void CSimWave2D::Transfer(const CSimWave2D& wave2, CRectangle<int> rci_this, CRectangle<int> rci_it)
	{
		Assert(bWithin(rci_this.tX0(), 0, iWidth-1));
		Assert(bWithin(rci_this.tX1(), 0, iWidth));
		Assert(bWithin(rci_this.tY0(), 0, iHeight-1));
		Assert(bWithin(rci_this.tY1(), 0, iHeight));

		Assert(bWithin(rci_it.tX0(), 0, wave2.iWidth-1));
		Assert(bWithin(rci_it.tX1(), 0, wave2.iWidth));
		Assert(bWithin(rci_it.tY0(), 0, wave2.iHeight-1));
		Assert(bWithin(rci_it.tY1(), 0, wave2.iHeight));

		if (rci_it.tWidth() >= rci_this.tWidth())
		{
			// Down-convert. Perform integral averaging, ignoring any remainder.
			// Both dimensions must down-convert.
			int i_xavg = rci_it.tWidth()  / rci_this.tWidth();
			int i_yavg = rci_it.tHeight() / rci_this.tHeight();
			Assert(i_xavg > 0 && i_yavg > 0);

			TMeasure mr_scale = 1.0 / (i_xavg * i_yavg);

			// Index starting corner of source arrays.
			int i_srcy = rci_this.tY0() * wave2.iStride + rci_this.tX0();

			for (int i_y = rci_this.tY0(); i_y < rci_this.tY1(); i_y++)
			{
				int i_srcx = i_srcy;
				for (int i_x = rci_this.tX0(); i_x < rci_this.tX1(); i_x++)
				{
					// Find avg values.
					TMetres m_amp = 0.0, m_amp_prev = 0.0;
					int i_srca = i_srcx;
					for (int i_ya = 0; i_ya < i_yavg; i_ya++)
					{
						for (int i_xa = 0; i_xa < i_xavg; i_xa++)
						{
							m_amp      += wave2.pa2mAmplitude.tLinear(i_srca + i_xa);
							m_amp_prev += wave2.pa2mAmplitudePrev.tLinear(i_srca + i_xa);
						}
						i_srca += wave2.iStride;
					}
					pa2mAmplitude    [i_y][i_x] = m_amp      * mr_scale;
					pa2mAmplitudePrev[i_y][i_x] = m_amp_prev * mr_scale;

					Assert(bValidAmp(pa2mAmplitude[i_y][i_x]));
					Assert(bValidAmp(pa2mAmplitudePrev[i_y][i_x]));

					i_srcx += i_xavg;
				}
				i_srcy += wave2.iStride * i_yavg;
			}
		}
		else
		{
			// Up-convert. Perform linear interpolation.
			// Both dimensions must up-convert.
			float f_xterp = float(rci_it.tWidth()  - 1) / (rci_this.tWidth()  - 1);
			float f_yterp = float(rci_it.tHeight() - 1) / (rci_this.tHeight() - 1);
			Assert(f_xterp > 0 && f_yterp > 0);

			for (int i_y = rci_this.tY0(); i_y < rci_this.tY1(); i_y++)
			{
				float f_yit = (i_y - rci_this.tY0()) * f_yterp;
				int	  i_yit = rci_it.tY0() + iPosFloatCast(f_yit);
				float f_yrem = f_yit - float(i_yit);

				Assert(bWithin(i_yit, rci_it.tY0(), rci_it.tY1()-1));

				for (int i_x = rci_this.tX0(); i_x < rci_this.tX1(); i_x++)
				{
					float f_xit = (i_x - rci_this.tX0()) * f_xterp;
					int	  i_xit = rci_it.tX0() + iPosFloatCast(f_xit);
					float f_xrem = f_xit - float(i_xit);

					Assert(bWithin(i_xit, rci_it.tX0(), rci_it.tX1()-1));

					pa2mAmplitude[i_y][i_x] = 
					(
						wave2.pa2mAmplitude[i_yit][i_xit]   * (1.0 - f_xrem) +
						wave2.pa2mAmplitude[i_yit][i_xit+1] * f_xrem
					) * (1.0 - f_yrem) +
					(
						wave2.pa2mAmplitude[i_yit]  [i_xit]   * (1.0 - f_xrem) +
						wave2.pa2mAmplitude[i_yit+1][i_xit+1] * f_xrem
					) * f_yrem;

					pa2mAmplitudePrev[i_y][i_x] = 
					(
						wave2.pa2mAmplitudePrev[i_yit][i_xit]   * (1.0 - f_xrem) +
						wave2.pa2mAmplitudePrev[i_yit][i_xit+1] * f_xrem
					) * (1.0 - f_yrem) +
					(
						wave2.pa2mAmplitudePrev[i_yit]  [i_xit]   * (1.0 - f_xrem) +
						wave2.pa2mAmplitudePrev[i_yit+1][i_xit+1] * f_xrem
					) * f_yrem;

					Assert(bValidAmp(pa2mAmplitude[i_y][i_x]));
					Assert(bValidAmp(pa2mAmplitudePrev[i_y][i_x]));
				}
			}
		}
	}

	//******************************************************************************************
	void CSimWave2D::Transfer(const CSimWave2D& wave2, CRectangle<> rc_this, CRectangle<> rc_it)
	{
		// Convert float relative regions to integer absolute regions, ignoring borders.

		CRectangle<int> rci_this
		(
			rc_this.tX0() * (iWidth-2) + 1,
			rc_this.tY0() * (iHeight-2) + 1,
			rc_this.tWidth() * (iWidth-2),
			rc_this.tHeight() * (iHeight-2)
		);

		CRectangle<int> rci_it
		(
			rc_it.tX0() * (wave2.iWidth-2) + 1,
			rc_it.tY0() * (wave2.iHeight-2) + 1,
			rc_it.tWidth() * (wave2.iWidth-2),
			rc_it.tHeight() * (wave2.iHeight-2)
		);

		Transfer(wave2, rci_this, rci_it);
	}

	//******************************************************************************************
	void CSimWave2D::CreateDisturbance(TMetres m_x, TMetres m_y, TMetres m_radius, TMetres m_height,
		bool b_add)
	{
		CCycleTimer ctmr;

		if (b_add && m_height == 0.0)
			return;

 		TMetres m_radius_x = Max(m_radius, mMIN_DISTURB_RADIUS);
		TMetres m_radius_y = Max(m_radius, mMIN_DISTURB_RADIUS);

		// Find limits of the elements to alter.
		int i_x1 = iTrunc((m_x - m_radius_x) / mIntervalX);
		int i_x2 = iTrunc((m_x + m_radius_x) / mIntervalX);
		int i_y1 = iTrunc((m_y - m_radius_y) / mIntervalY);
		int i_y2 = iTrunc((m_y + m_radius_y) / mIntervalY);

		// Clamp them to the range of the model.
		SetMax(i_x1, 0);
		SetMin(i_x2, iHeight-1);
		SetMax(i_y1, 0);
		SetMin(i_y2, iWidth-1);

		// If current amplitude is too high, don't add to the problem.
		if (b_add)
		{
			// Find the current max amplitude in this region.
			TMetres m_max = 0;

			for (int i_x = i_x1; i_x <= i_x2; i_x++)
				for (int i_y = i_y1; i_y <= i_y2; i_y++)
					if (bElementActive(i_x, i_y))
						SetMax(m_max, Abs(pa2mAmplitude[i_x][i_y]));

			// Limit the disturbance amplitude so as not to exceed the max.
			TReal m_max_height = mMAX_AMPLITUDE - m_max;
			if (m_max_height <= 0)
				return;
			SetMinMax(m_height, -m_max_height, +m_max_height);
		}
		else
		{
			// Simply limit the disturbance value to the max.
			SetMinMax(m_height, -mMAX_AMPLITUDE, +mMAX_AMPLITUDE);
		}

		//
		// For each element, set to the integral of the disturbance function over its area, divided by the area.
		//
		// For a disturbance function d(x,y), that integral is:
		//
		//		I = (D(x,y) - D(x+dx,y) - D(x,y+dy) + D(x+dx,y+dy)) / (dx dy)
		//
		// where D is the double indefinite integral of d over x and y.
		//

#if 1
		//
		// For our disturbance function d = (cos(pi x) + 1) (cos(pi y) + 1) / 4
		//
		//		D = (sin(pi x)/pi + x) (sin(pi y)/pi + y) / 4
		//		I = ( (sin(pi x) - sin(pi(x+dx))) / pi - dx ) 
		//			( (sin(pi y) - sin(pi(y+dy))) / pi - dy ) / (4 dx dy)
		//

		// Compute coordinate increments to scale impulse radii to a circle of radius 1.
		TReal r_inc_x = mIntervalX / m_radius_x;
		TReal r_inc_y = mIntervalY / m_radius_y;

		TReal r_x = (i_x1 * mIntervalX - m_x) / m_radius_x;
		for (int i_x = i_x1; i_x <= i_x2; i_x++, r_x += r_inc_x)
		{
			TReal r_val_x = (sin(dPI*r_x) - sin(dPI*(r_x + r_inc_x))) / dPI - r_inc_x;
			r_val_x /= 4 * r_inc_x * r_inc_y;

			TReal r_y = (i_y1 * mIntervalY - m_y) / m_radius_y;
			for (int i_y = i_y1; i_y <= i_y2; i_y++, r_y += r_inc_y)
			{
				TReal r_val_y = (sin(dPI*r_y) - sin(dPI*(r_y + r_inc_y))) / dPI - r_inc_y;
				TReal r_val = r_val_x * r_val_y;
				if (r_val <= 0)
					// Outside radius.
					continue;

				if (!bElementActive(i_x, i_y))
					continue;

				TMetres m_disturb = r_val * m_height;
				if (b_add)
					pa2mAmplitude[i_x][i_y] += m_disturb;
				else
					pa2mAmplitude[i_x][i_y]  = m_disturb;
			}
		}
#else
		//
		// For our simple disturbance function  d = 1 - r² = 1 - x² - y²
		//
		//		D = xy - x³y/3 - xy³/3, and
		//		I = 1 - x² - y² - x dx - y dy - dx²/3 - dy²/3
		//

		// Compute coordinate increments to scale impulse radii to a circle of radius 1.
		TReal r_inc_x = mIntervalX / m_radius_x;
		TReal r_inc_y = mIntervalY / m_radius_y;

		TReal r_add_const = 1.0 - (r_inc_x*r_inc_x + r_inc_y*r_inc_y) / 3.0;

		TReal r_x = (i_x1 * mIntervalX - m_x) / m_radius_x;
		for (int i_x = i_x1; i_x <= i_x2; i_x++, r_x += r_inc_x)
		{
			TReal r_y = (i_y1 * mIntervalY - m_y) / m_radius_y;
			for (int i_y = i_y1; i_y <= i_y2; i_y++, r_y += r_inc_y)
			{
				TReal r_val = r_add_const - r_x*(r_x + r_inc_x) - r_y*(r_y + r_inc_y);
				if (r_val <= 0)
					// Outside radius.
					continue;

				if (!bElementActive(i_x, i_y))
					continue;

				TMetres m_disturb = r_val * m_height;
				if (b_add)
					pa2mAmplitude[i_x][i_y] += m_disturb;
				else
					pa2mAmplitude[i_x][i_y]  = m_disturb;
			}
		}
#endif
		psDisturb.Add(ctmr(), 1);
	}

// protected:

	//******************************************************************************************
	void CSimWave2D::Iterate(TSeconds s_step_cur)
	{
#if VER_TEST_OBJECTS
		// Calculate coefficients for each iteration.
		TMeasure mr_factor	= mrVelocity / mInterval;
		mr_factor *= mr_factor;

		TMeasure s_step_sqr = s_step_cur * (s_step_cur + sStepPrev) / 2;

		// Coefficient for current amplitude.
		TMeasure mr_a	= 2.0 / s_step_cur / sStepPrev 
						- 4.0 * mr_factor 
						- mrDamping / sStepPrev
						- mrRestoring;
		mr_a *= s_step_sqr;

		Assert(mr_a > 0);

		// Coefficient for neighboring amplitudes.
		TMeasure mr_x	= mr_factor * s_step_sqr;

		// Coefficient for previous amplitude.
		TMeasure mr_t	= mrDamping / sStepPrev * s_step_sqr
						- s_step_cur / sStepPrev;

		// Constant addend.
		TMeasure mr_c	= - mrGravity * s_step_sqr;

		// Iterate through the internal points.
		// We store the new wave values in pamAmplitudePrev, and then swap the arrays.
		for (int i_y = 1; i_y < iHeight-1; i_y++)
			for (int i_x = 1; i_x < iWidth-1; i_x++)
			{
				pa2mAmplitudePrev[i_y][i_x] = mr_a * pa2mAmplitude[i_y][i_x]
										   + mr_x * (pa2mAmplitude[i_y-1][i_x] + pa2mAmplitude[i_y+1][i_x])
										   + mr_x * (pa2mAmplitude[i_y][i_x-1] + pa2mAmplitude[i_y][i_x+1])
										   + mr_t * pa2mAmplitudePrev[i_y][i_x]
										   + mr_c;
			}
#else //VER_TEST_OBJECTS
		Assert(0);
#endif
	}

	//******************************************************************************************
	void CSimWave2D::ApplyBoundary()
	{
		//
		// Once upon a time, boundary conditions were enforced here, in an attempt to prevent
		// wave reflection behaviour at the borders. This never worked right, and is unnecessary
		// now anyway, as end borders will always be invisible (e.g. underground). Internal borders
		// are handled by the neighbours.
		//

		switch (iBoundary)
		{
			case 0:
			{
				// Zero boundary.
				if (!apwv2Neighbours[0])
					for (int i_x = 0; i_x < iWidth; i_x++)
						pa2mAmplitude[0][i_x] = 0;
				if (!apwv2Neighbours[1])
					for (int i_y = 0; i_y < iHeight; i_y++)
						pa2mAmplitude[i_y](-1) = 0;
				if (!apwv2Neighbours[2])
					for (int i_x = 0; i_x < iWidth; i_x++)
						pa2mAmplitude(-1)[i_x] = 0;
				if (!apwv2Neighbours[3])
					for (int i_y = 0; i_y < iHeight; i_y++)
						pa2mAmplitude[i_y][0] = 0;
				break;
			}
			case 1:
			{
				// Zero first derivative at boundary.
				for (int i_y = 0; i_y < iHeight; i_y++)
				{
					pa2mAmplitude[i_y][0] = pa2mAmplitude[i_y][1];
					pa2mAmplitude[i_y](-1) = pa2mAmplitude[i_y](-2);
				}
				for (int i_x = 0; i_x < iWidth; i_x++)
				{
					pa2mAmplitude[0][i_x] = pa2mAmplitude[1][i_x];
					pa2mAmplitude(-1)[i_x] = pa2mAmplitude(-2)[i_x];
				}
				break;
			}
			case 2:
			{
				// Zero second derivative at boundary.
				for (int i_y = 0; i_y < iHeight; i_y++)
				{
					pa2mAmplitude[i_y][0]  = 2*pa2mAmplitude[i_y][1]  - pa2mAmplitude[i_y][2];
					pa2mAmplitude[i_y](-1) = 2*pa2mAmplitude[i_y](-2) - pa2mAmplitude[i_y](-3);
				}
				for (int i_x = 0; i_x < iWidth; i_x++)
				{
					pa2mAmplitude[0][i_x]  = 2*pa2mAmplitude[1][i_x]  - pa2mAmplitude[2][i_x];
					pa2mAmplitude(-1)[i_x] = 2*pa2mAmplitude(-2)[i_x] - pa2mAmplitude(-3)[i_x];
				}
				break;
			}
			default:
				Assert(false);
		}

#if VER_DEBUG
		// Check.
		for (int i_y = 0; i_y < iHeight; i_y++)
		{
			Assert(bValidAmp(pa2mAmplitude[i_y][0]));
			Assert(bValidAmp(pa2mAmplitude[i_y](-1)));
		}
		for (int i_x = 0; i_x < iWidth; i_x++)
		{
			Assert(bValidAmp(pa2mAmplitude[0][i_x]));
			Assert(bValidAmp(pa2mAmplitude(-1)[i_x]));
		}
#endif

		//
		// Set boundaries from neighbours, copying real (not boundary) rows or columns
		// from the neighbour object.
		// (For now, only deal with boundaries of same world extent).
		//
		CSimWave2D* pwv2;
		if (pwv2 = apwv2Neighbours[0])
		{
			// My bottom boundary comes from your top.
			Assert(pwv2->mWidth == mWidth);

			CopyInterp
			(
				CPArray2<TReal>(&pa2mAmplitude[0][1], iWidth-2, 1),
				CPArray2<TReal>(&pwv2->pa2mAmplitude(-2)[1], pwv2->iWidth-2, 1)
			);
		}
		if (pwv2 = apwv2Neighbours[1])
		{
			// My right boundary comes from your left.
			Assert(pwv2->mLength == mLength);

			CopyInterp
			(
				CPArray2<TReal>(&pa2mAmplitude[1](-1), iHeight-2, 1, iStride),
				CPArray2<TReal>(&pwv2->pa2mAmplitude[1][1], pwv2->iHeight-2, 1, pwv2->iStride)
			);
		}
		if (pwv2 = apwv2Neighbours[2])
		{
			// My top boundary comes from your bottom.
			Assert(pwv2->mWidth == mWidth);

			CopyInterp
			(
				CPArray2<TReal>(&pa2mAmplitude(-1)[1], iWidth-2, 1),
				CPArray2<TReal>(&pwv2->pa2mAmplitude[1][1], pwv2->iWidth-2, 1)
			);
		}
		if (pwv2 = apwv2Neighbours[3])
		{
			// My left boundary comes from your right.
			Assert(pwv2->mLength == mLength);

			CopyInterp
			(
				CPArray2<TReal>(&pa2mAmplitude[1][0], iHeight-2, 1, iStride),
				CPArray2<TReal>(&pwv2->pa2mAmplitude[1](-2), pwv2->iHeight-2, 1, pwv2->iStride)
			);
		}

	}

	//******************************************************************************************
	void CSimWave2D::SetMaxStep()
	{
		//
		// Determine maximum stable step size in the 2D case.
		//
		// We require the coefficient of the current element height to be positive.
		//
		//		2/(ds dt) - 4 v²/dx² - f/ds - r > 0
		//
		// To find the maximum dt, substitute ds with dt, and multiply by -dt²:
		//
		//		2/dt² - 4 v²/dx² - f/dt - r > 0
		//		(4 v²/dx² + r) dt² + f dt - 2 = 0
		//
		// We can discount f, and arrive at:
		//
		//		dt = sqrt(2 / (4 v²/dx² + r))
		//
		// For some reason that I admit I don't understand, the integration in the 2D case is still
		// not stable unless you double the v²/dx² component, so this becomes:
		// 
		//		dt = sqrt(2 / (8 v²/dx² + r))
		//
		// Note to self:
		//		Why, oh why didn't I do this simple calculation once I had the full integration
		//		equation, instead of ignoring r, using dt = dx/v, and then multiplying
		//		by 0.4 for "safety"?  This wrong way of doing it caused blowups by choosing
		//		too large a step for low resolutions, and was slower than necessary by choosing
		//		too small a step for high resolutions.
		//

		// Set max step based on above equation, substituting 1.8 for 2.0 for safety.
		sStepMax = sStepPrev = fSqrt(1.8 / (8.0 * Sqr(mrVelocity/mInterval) + mrRestoring));
	}


//******************************************************************************************
//
// CSimWaveVar2D implementation.
//

	//******************************************************************************************
	CSimWaveVar2D::CSimWaveVar2D
	(
		int i_elements1, int i_elements2, int i_boundary, TMetres m_length, TMetres m_width,
		TMeasure mr_velocity, TMeasure mr_damping, TMeasure mr_restoring
	)
		: CSimWave2D(i_elements1, i_elements2, i_boundary, m_length, m_width,
			(mr_velocity > 0.0 ? mr_velocity : 1.0), mr_damping, mr_restoring, 0),
		pa2mrVelocityFactor(iHeight, iWidth, iStride),
		palmColLimits(iHeight)
	{
		if (mr_velocity > 0.0)
			// If mr_velocity is 0, we skip initialising velocity info.
			// Caller had better to it himself, or trouble will occur.
			SetVelocity(mr_velocity);
	}

	//******************************************************************************************
	CSimWaveVar2D::CSimWaveVar2D
	(
		int i_elements1, int i_elements2, const CSimWaveVar2D& wave2
	)
		: CSimWave2D(i_elements1, i_elements2, wave2),
		pa2mrVelocityFactor(iHeight, iWidth, iStride),
		palmColLimits(iHeight)
	{
		// Retain mrVelocity and sMaxStep from wave2; caller can update them if desired.
	}

	//******************************************************************************************
	void CSimWaveVar2D::SetVelocity(TMeasure mr_velocity)
	{
		Assert(mr_velocity > 0);

		TMeasure mr_velocity_factor = Square(mr_velocity / mInterval);

		pa2mrVelocityFactor.Fill(mr_velocity_factor);

		UpdateVelocity();
	}

	//******************************************************************************************
	inline void CSimWaveVar2D::SetVelocity(int i_1, int i_2, TMeasure mr_velocity)
	{
		Assert(mr_velocity >= 0);
		pa2mrVelocityFactor[i_1][i_2] = Square(mr_velocity / mInterval);
	}

	//******************************************************************************************
	void CSimWaveVar2D::UpdateVelocity()
	{
		// Enforce zero velocity at border.
		int i_x, i_y;
		for (i_y = 0; i_y < iHeight; i_y++)
			pa2mrVelocityFactor[i_y][0] = pa2mrVelocityFactor[i_y](-1) = 0;
		for (i_x = 0; i_x < iWidth; i_x++)
			pa2mrVelocityFactor[0][i_x] = pa2mrVelocityFactor(-1)[i_x] = 0;

		//
		// Search through velocity factor array, finding maximum.
		// Also find limits of array containing non-zero factors.
		// For 3DX, these limits should be rounded out to even boundaries.
		// This may result in the border elements being integrated, which is wasteful,
		// since they are always reset by ApplyBoundary().  But this will only happen
		// when the water is not on a natural terrain boundary.
		//
		// Note also that it's OK to access a column index 1 beyond the start or end,
		// as long as the row is not also the start or end. Since the row limits are always
		// at least 1 element inward, this holds.
		//

		TMeasure mr_velocity_factor = 0;

		lmRows.iStart = iHeight-2;
		lmRows.iEnd = 1;

		for (i_y = 0; i_y < iHeight; i_y++)
		{
			palmColLimits[i_y].iStart = iWidth-2;
			palmColLimits[i_y].iEnd = 1;

			for (i_x = 0; i_x < iWidth; i_x++)
			{
				Assert(pa2mrVelocityFactor[i_y][i_x] >= 0);

				if (pa2mrVelocityFactor[i_y][i_x])
				{
					SetMax(mr_velocity_factor, pa2mrVelocityFactor[i_y][i_x]);

					// Set iteration limits.
					SetMin(lmRows.iStart, i_y);
					lmRows.iEnd = i_y;

					SetMin(palmColLimits[i_y].iStart, i_x);
					palmColLimits[i_y].iEnd = i_x;
				}
			}

#if VER_LIMITS
			// Round iStart down to even, and iEnd up to odd.
			palmColLimits[i_y].iStart &= ~1;
			palmColLimits[i_y].iEnd   |= 1;
#else
			palmColLimits[i_y].iStart = 0;
			palmColLimits[i_y].iEnd   = iWidth-1;
#endif
		}

#if !VER_LIMITS
		lmRows.iStart = 0;
		lmRows.iEnd   = iHeight-1;
#endif

		// Now back-calculate max velocity, and reset max step.
		mrVelocity = fSqrt(mr_velocity_factor) * mInterval;
		SetMaxStep();
	}

// protected:

	//******************************************************************************************
	void CSimWaveVar2D::Iterate(TSeconds s_step_cur)
	{
#if (VER_ASM)

#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

		// Typedefs so that the assembler will accept offsets.
		typedef CSimWaveVar2D tdCSimWaveVar2D;
		typedef SLimits tdSLimits;

		// Assumptions made by this code.
		Assert(sizeof(SLimits) == 8);
		Assert((char *)&palmColLimits.atArray - (char *)&palmColLimits == 4);
		Assert((char *)&pa2mAmplitude.atArray - (char *)&pa2mAmplitude == 4);
		Assert((char *)&pa2mrVelocityFactor.atArray - (char *)&pa2mrVelocityFactor == 4);
		Assert((char *)&pa2mAmplitudePrev.atArray - (char *)&pa2mAmplitudePrev == 4);

	//	TMeasure s_step_sqr;
	//	TMeasure mr_a_const;
	//	TMeasure mr_t;

		int  i_stride;
		int  i_y;
		uint u_end;
	//	uint u_index;

	//	TMeasure mr_x;
	//	TMeasure mr_a;

		const float f_const_0_5 = 0.5f;
		const float f_const_2_0 = 2.0f;
		const float f_const_4_0 = 4.0f;

	//	double vf_4_0_step_sqr;
		#define vf_4_0_step_sqr		 esp+12		// to be manually allocated on 
		#define vf_4_0_step_sqr_loop esp+12+4	// 8B-aligned stack (with one 
												// immediately following push, 
	//	double vf_a_const;						// plus another within row loop 
		#define vf_a_const			 esp+4		// but outside of element loop)
		#define vf_a_const_loop		 esp+4+4

		uint i_stride_x4;

		__asm
		{
		mov		eax,esp						;grab copy of esp
		and		esp,0xFFFFFFF8				;re-align stack, as need be, to 8B boundary
		sub		esp,16						;allocate space for two vf_* variables
		push	eax							;save original esp for later restore

		// TMeasure s_step_sqr = ...;
		// TMeasure mr_a_const = ...;
		// TMeasure mr_t = ...;
		// int i_stride = ...;

		mov		ecx,[this]					;restore This ptr

		femms								;ensure fast switch if any

		nop									;(to avoid degraded predecode)
		movd	mm0,[ecx]CWaveData.sStepPrev	;m0= sStepPrev

		nop									;(to avoid degraded predecode)
		movd	mm2,[s_step_cur]			;m2= s_step_cur

		movd	mm7,[f_const_0_5]			;m7= 0.5
		movq	mm3,mm0						;m3= sStepPrev

		movd	mm5,[ecx]CWaveData.mrDamping	;m5= mrDamping
		pfadd	(m3,m2)						;m3= sStepPrev+s_step_cur

		movd	mm6,[ecx]CWaveData.mrRestoring ;m6= mrRestoring
		pfmul	(m7,m2)						  ;m7= s_step_cur/2

		movq	mm4,mm0						;m4= sStepPrev
		pfrcp	(m1,m0)						;m1= 1/sStepPrev

		mov		ebx,[ecx]tdCSimWaveVar2D.lmRows.iStart
		;									;ebx= i_y
		pfmul	(m3,m7)						;m3= s_step_sqr

		movd	mm7,[f_const_2_0]			;m7= 2.0
		pfmul	(m4,m2)						;m4= sStepPrev*s_step_cur

		movq	mm0,mm1						;m0= 1/sStepPrev
		pfmul	(m1,m5)						;m1= mrDamping/sStepPrev

		pfrcp	(m4,m4)						;m4= 1/(sStepPrev*s_step_cur)
		pfmul	(m5,m3)						;m5= mrDamping*s_step_sqr

		punpckldq mm3,mm3					;m3= s_step_sqr | ...
		pfadd	(m1,m6)						;m1= mrDamping/sStepPrev + mrRestoring

		pfmul	(m4,m7)						;m4= 2/(sStepPrev*s_step_cur)
		pfsub	(m5,m2)						;m5= mrDamping*s_step_sqr - s_step_cur

		movd	mm7,[f_const_4_0]			;m7= 4.0
		mov		[i_y],ebx					;save

		pfsub	(m4,m1)						;m4= 2/(...) - ... - ...
		pfmul	(m5,m0)						;m5= (...)/sStepPrev = mr_t

		pfmul	(m7,m3)						;m7= 4.0*s_step_sqr
		mov		eax,[ecx]CSimWave2D.iStride	;eax= i_stride

		pfmul	(m4,m3)						;m4= mr_a_const
		punpckldq mm5,mm5					;m5= mr_t | ...

		punpckldq mm7,mm7					;m7= 4.0*s_step_sqr | ...
		movq	[vf_4_0_step_sqr],mm7		;save
		
		lea		edx,[eax*4]					;edx= i_stride*4
		cmp		ebx,[ecx]tdCSimWaveVar2D.lmRows.iEnd
											;check if any rows to do

		punpckldq mm4,mm4					;m4= mr_a_const | ...
		movq	[vf_a_const],mm4			;save

		mov		[i_stride],eax				;save
		jg		IterDone					;skip if no rows

		mov		[i_stride_x4],edx			;save

		// for (int i_y = ...; i_y <= ...; i_y++)
		// {
		//     uint u_end = ...;


		;eax = i_stride
		;ebx = i_y
		;ecx = This ptr

		;mm3 = s_step_sqr | s_step_sqr
		;mm5 = mr_t | mr_t

		align	16

	RowLoop:
		mov		edx,[ecx+4]tdCSimWaveVar2D.palmColLimits
											;edx= palmColLimits.atArray ptr

		imul	eax,ebx						;eax= i_y*i_stride

		mov		esi,[edx+ebx*8]tdSLimits.iEnd
		;									;esi= palmColLimits[i_y].iEnd
		add		esi,eax						;esi= u_end
		nop									;(to avoid degraded predecode)
		add		eax,[edx+ebx*8]tdSLimits.iStart
		;									;eax= u_index
		mov		[u_end],esi					;save
		cmp		eax,esi						;check if any elements to do
		ja		NextRow						;skip if no elements

		mov		edx,[ecx+4]CSimWave2D.pa2mAmplitude
		;									;edx= pa2mAmplitude.atArray ptr
		push	ecx							;save for later restore

		mov		ebx,[ecx+4]tdCSimWaveVar2D.pa2mrVelocityFactor
		;									;ebx = pa2mrVelocityFactor.atArray ptr
		movq	mm4,[edx+eax*4-4]			;m4= pa2mAmplitude[u_index-1] | ...

		sub		edx,16						;offset ptr by 16
		mov		ecx,[ecx+4]CSimWave2D.pa2mAmplitudePrev
											;ecx= pa2mAmplitudePrev.atArray ptr

		mov		edi,edx
		sub		edi,[i_stride_x4]			;edi= (pa2mAmplitude-i_stride*4) ptr 
											;                          w/ offset

		mov		esi,edx
		add		esi,[i_stride_x4]			;esi= (pa2mAmplitude+i_stride*4) ptr 
											;                          w/ offset

		lea		ecx,[ecx-16]				;offset ptr by 16
		movq	mm7,[edi+eax*4+16]			;m7= pa2mAmplitude[u_index-i_stride] 
		;									;                              | ...

		lea		ebx,[ebx-16]				;offset ptr by 16
		movq	mm6,[esi+eax*4+16]			;m6= pa2mAmplitude[u_index+i_stride] 
		;									;                              | ...
		
		align	4							;(to avoid degraded predecode)

		pfadd	(m7,m4)						;m7= ...[u_index-i_stride] + 
		;									;              ...[u_index-1] | ...
		movq	mm4,[edx+eax*4+16+4]		;m4= pa2mAmplitude[u_index+1] | ...


		movq	mm0,[ebx+eax*4+16]			;m0= pa2mrVelocityFactor[u_index] 
		;									;                             | ...
		jmp		ElementLoop

		;eax = u_index
		;ebx = pa2mrVelocityFactor ptr w/ offset
		;ecx = pa2mAmplitudePrev ptr w/ offset
		;edx = pa2mAmplitude ptr w/ offset
		;esi = (pa2mAmplitude+i_stride*4) ptr w/ offset
		;edi = (pa2mAmplitude-i_stride*4) ptr w/ offset

		;mm0 = pa2mrVelocityFactor[u_index] | ...
		;mm3 = s_step_sqr | s_step_sqr
		;mm4 = pa2mAmplitude[u_index+1] | ...
		;      (also becomes pa2mAmplitude[u_index-1] | ...)
		;mm5 = mr_t | mr_t
		;mm6 = pa2mAmplitude[u_index+i_stride] | ...
		;mm7 = pa2mAmplitude[u_index-i_stride] + pa2mAmplitude[u_index-1] | ...

		// for (uint u_index = ...; u_index <= ...; u_index++)
		// {
		//     ...;
		// }
		// }

		align	16

	ElementLoop:
		movq	mm1,[vf_4_0_step_sqr_loop]	;m1= 4.0*s_step_sqr | ...
		pfadd	(m6,m4)						;m6= ...[u_index+i_stride] + 
		;									;              ...[u_index+1] | ...

		pfmul	(m1,m0)						;m1= 4.0*mr_x | ...
		pfadd	(m7,m6)						;m7= (...+...+...+...) | ...
											;  NOTE: above pair is before below 
											;  pair to avoid degraded predecode
		movq	mm2,[vf_a_const_loop]		;m2= mr_a_const | ...
		nop									;(filler to maintain decode pairing)

		pfmul	(m0,m3)						;m0= mr_x | ...
		movq	mm6,[edx+eax*4+16]			;m6= pa2mAmplitude[u_index] | ...

		pfsubr	(m1,m2)						;m1= mr_a | ...
		movq	mm2,[ecx+eax*4+16]			;m2= pa2mAmplitudePrev[u_index] |...

		pfmul	(m0,m7)						;m0= mr_x*(...) | ...
		add		eax,2						;advance to next pair of elements

		pfmul	(m1,m6)						;m1= mr_a*(...) | ...
		movq	mm7,[edi+eax*4+16]			;m7= next ...A[u_index-i_stride] |...

		pfmul	(m2,m5)						;m2= mr_t*(...) | ...
		movq	mm6,[esi+eax*4+16]			;m6= next ...A[u_index+i_stride] |...

		pfadd	(m1,m0)						;m1= mr_a...+mr_x... | ...
		cmp		eax,[u_end]					;check if any more elements to do

		pfadd	(m7,m4)						;m7= ...[u_index-i_stride] + 
		;									;              ...[u_index-1] | ...
		movq	mm4,[edx+eax*4+16+4]		;m4= next ...A[u_index+1] | ...

		pfadd	(m1,m2)						;m1= mr_a...+mr_x...+mr_t... | ...
		movq	[ecx+eax*4+16-8],mm1		;store to pa2mAmplitudePrev[u_index]

		movq	mm0,[ebx+eax*4+16]			;m0= next ...VF[u_index] | ...
		jle		ElementLoop					;continue if not done


		pop		ecx							;restore This ptr
	NextRow:
		mov		ebx,[i_y]					;ebx= i_y

		inc		ebx							;ebx= i_y+1
		mov		eax,[i_stride]				;eax= i_stride

		cmp		ebx,[ecx]tdCSimWaveVar2D.lmRows.iEnd
		;									;check if any more rows to do
		mov		[i_y],ebx					;update saved i_y

		jle		RowLoop						;continue if not done


	IterDone:
		pop		eax							;restore old esp before realignment 
		mov		esp,eax						;to 8B boundary

		femms								;clear MMX state and ensure fast 
											;  switch if any
		}

#else  // if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

		// Typedefs so that the assembler will accept offsets.
		typedef CSimWaveVar2D tdCSimWaveVar2D;
		typedef SLimits tdSLimits;

		// Assumptions made by this code.
		Assert(sizeof(SLimits) == 8);
		Assert((char *)&palmColLimits.atArray - (char *)&palmColLimits == 4);
		Assert((char *)&pa2mAmplitude.atArray - (char *)&pa2mAmplitude == 4);
		Assert((char *)&pa2mrVelocityFactor.atArray - (char *)&pa2mrVelocityFactor == 4);
		Assert((char *)&pa2mAmplitudePrev.atArray - (char *)&pa2mAmplitudePrev == 4);
		Assert(bWithin(sStepPrev, 0.0f, 1.0f));

		TMeasure s_step_sqr = s_step_cur * (s_step_cur + sStepPrev) * 0.5f;

		// Constant portion of coefficient for current amplitude.
		TMeasure mr_a_const = (2.0f / (s_step_cur * sStepPrev)
							   - mrDamping / sStepPrev 
							   - mrRestoring) * s_step_sqr;

		// Coefficient for previous amplitude.
		TMeasure mr_t = mrDamping / sStepPrev * s_step_sqr
						- s_step_cur / sStepPrev;

		// Iterate through the internal points.
		// We store the new wave values in pamAmplitudePrev, and then swap the arrays.
		const float fFour = 4.0f;
		int i_y = lmRows.iStart;
		int i_stride = pa2mAmplitude.iStride;
		int i_row = i_y * i_stride;
		uint u_end;

		__asm
		{
			mov		esi,this
			mov		ebx,i_y

			mov		ecx,i_row
			mov		edx,i_stride

			mov		eax,[esi]tdCSimWaveVar2D.lmRows.iEnd
			mov		edi,[esi+4]tdCSimWaveVar2D.palmColLimits

			jmp		END_OF_ROW_LOOP

			// eax = lmRows.iEnd
			// ebx = i_y
			// ecx = i_row
			// edx = i_stride
			// esi = this
			// edi = palmColLimits
ROW_LOOP:
			mov		eax,[edi + ebx*8]tdSLimits.iStart
			mov		ebx,[edi + ebx*8]tdSLimits.iEnd

			cmp		eax,ebx										// Test for zero size.
			jg		SKIP_INNER_LOOP

			add		eax,ecx										// u_index
			add		ebx,ecx										// u_end

			mov		edi,[esi+4]CSimWave2D.pa2mAmplitude
			mov		u_end,ebx									// save u_end

			shl		edx,2										// i_stride*4
			mov		ebx,[esi+4]CSimWave2D.pa2mAmplitude

			mov		ecx,edi										// copy of pa2mAmplitude
			sub		edi,edx										// pa2mAmplitude-i_stride

			add		edx,ecx										// pa2mAmplitude+i_stride
			mov		ecx,[esi+4]tdCSimWaveVar2D.pa2mrVelocityFactor

			// eax = u_index
			// ebx = pa2mAmplitude
			// ecx = pa2mrVelocityFactor,pa2mAmplitudePrev
			// edx = pa2mAmplitude+i_stride
			// esi = this
			// edi = pa2mAmplitude-i_stride
			fld		s_step_sqr
			fmul	dword ptr[ecx + eax*4]				// x
			fld		dword ptr[edi + eax*4]				// 
			fadd	dword ptr[edx + eax*4]				// a,x
			fld		fFour								// 
			fmul	st(0),st(2)							// c,a,x
			fld		dword ptr[ebx + eax*4 - 4]			//
			fadd	dword ptr[ebx + eax*4 + 4]			// b,c,a,x
			fxch	st(1)								// c,b,a,x
			fsubr	mr_a_const							// d,b,a,x
			fxch	st(1)								// b,d,a,x

			mov		ecx,[esi+4]CSimWave2D.pa2mAmplitudePrev

			faddp	st(2),st(0)							// d,e,x
			fmul	dword ptr[ebx + eax*4]				// f,e,x
			fld		mr_t								// mr_t,f,e,x
			fmul	dword ptr[ecx + eax*4]				// g,f,e,x

			mov		ecx,[esi+4]tdCSimWaveVar2D.pa2mrVelocityFactor

			fxch	st(2)								// e,f,g,x
			fmulp	st(3),st(0)							// f,g,h
			faddp	st(1),st(0)							// i,h

			inc		eax
			jmp		short END_OF_INNER_LOOP

INNER_LOOP:
			// x = pa2mrVelocityFactor[u_index] * s_step_sqr
			// a = pa2mAmplitude[u_index - i_stride] + pa2mAmplitude[u_index + i_stride]
			// b = pa2mAmplitude[u_index - 1] + pa2mAmplitude[u_index + 1]
			// c = 4.0 * x
			// d = mr_a_const - c
			// e = a + b
			// f = d * pa2mAmplitude[u_index]
			// g = mr_t * pa2mAmplitudePrev[u_index]
			// h = e * x
			// i = f + g 
			// j = i + h
			fld		s_step_sqr
			fmul	dword ptr[ecx + eax*4]				// x,h',i'
			fld		dword ptr[edi + eax*4]				// 
			fadd	dword ptr[edx + eax*4]				// a,x,h',i'
			fld		fFour								// 
			fmul	st(0),st(2)							// c,a,x,h',i'
			fld		dword ptr[ebx + eax*4 - 4]			//
			fadd	dword ptr[ebx + eax*4 + 4]			// b,c,a,x,h',i'
			fxch	st(1)								// c,b,a,x,h',i'
			fsubr	mr_a_const							// d,b,a,x,h',i'
			fxch	st(4)								// h',b,a,x,d,i'
			faddp	st(5),st(0)							// b,a,x,d,j'
			faddp	st(1),st(0)							// e,x,d,j'
			fxch	st(2)								// d,x,e,j'
			fmul	dword ptr[ebx + eax*4]				// f,x,e,j'

			mov		ecx,[esi+4]CSimWave2D.pa2mAmplitudePrev

			fld		mr_t								// mr_t,f,x,e,j'
			fmul	dword ptr[ecx + eax*4]				// g,f,x,e,j'
			fxch	st(4)								// j',f,x,e,g
			fstp	dword ptr[ecx + eax*4 - 4]			// f,x,e,g
			faddp	st(3),st(0)							// x,e,i
			fmulp	st(1),st(0)							// h,i

			inc		eax
			mov		ecx,[esi+4]tdCSimWaveVar2D.pa2mrVelocityFactor

END_OF_INNER_LOOP:
			cmp		eax,u_end
			jl		short INNER_LOOP

			faddp	st(1),st(0)							// j'

			mov		ebx,[esi+4]CSimWave2D.pa2mAmplitudePrev
			mov		edx,i_stride

			mov		ecx,i_row
			mov		edi,[esi+4]tdCSimWaveVar2D.palmColLimits

			fstp	dword ptr[ebx + eax*4 - 4]			// empty

SKIP_INNER_LOOP:
			mov		eax,[esi]tdCSimWaveVar2D.lmRows.iEnd
			mov		ebx,i_y

			inc		ebx									// i_y++
			add		ecx,edx								// i_row += i_stride

			mov		i_y,ebx
			mov		i_row,ecx
			
END_OF_ROW_LOOP:
			cmp		ebx,eax
			jle		ROW_LOOP
		}

#endif // else if (TARGET_PROCESSOR == PROCESSOR_K6_3D)
#else // if (VER_ASM)

		//
		//		z(+dt) = dt(ds+dt)/2 (
		//					(2/(ds dt) - 2 v²/dx² - 2 v²/dy² - f/ds - r) z
		//				   + v²/dx² ( z(+dx) + z(-dx) )
		//				   + v²/dy² ( z(+dy) + z(-dy) )
		//				   + (f/ds - 2/(ds(ds+dt))) z(-ds)
		//				 )
		//

		TMeasure s_step_sqr = s_step_cur * (s_step_cur + sStepPrev) / 2;

		// Constant portion of coefficient for current amplitude.
		TMeasure mr_a_const = (2.0 / (s_step_cur * sStepPrev)
							- mrDamping / sStepPrev 
							- mrRestoring)
							* s_step_sqr;

		// Assert that the "a" value for the maximum velocity element is positive.
		Assert(mr_a_const - 4.0 * Square(mrVelocity / mInterval) * s_step_sqr > 0.0);

		// Coefficient for previous amplitude.
		TMeasure mr_t	= mrDamping / sStepPrev * s_step_sqr
						- s_step_cur / sStepPrev;

#if VER_ANISOTROPIC_RES
		//
		// The stored velocity factor vf is v²/dm², where dm = mInterval.
		// We must convert to coefficients for other terms:
		//
		//	af = 2 v²/(1/dx² + 1/dy²)	= vf * 2 / (1/dx² + 1/dy²) / (1/dm²)
		//								= vf * 2 * dm² (dx² + dy²) / (dx² dy²)
		//	xf = v²/dx²					= vf * dm²/dx²
		//
		TMeasure mr_a_factor = 2.0 * (Square(mIntervalX) + Square(mIntervalY)) * 
							   Square(mInterval / mIntervalX / mIntervalY) * s_step_sqr;
		TMeasure mr_x_factor = Square(mInterval / mIntervalX) * s_step_sqr;
		TMeasure mr_y_factor = Square(mInterval / mIntervalY) * s_step_sqr;
#endif

		// Iterate through the internal points.
		// We store the new wave values in pamAmplitudePrev, and then swap the arrays.
		for (int i_y = lmRows.iStart; i_y <= lmRows.iEnd; i_y++)
		{
			uint u_index = i_y * iStride + palmColLimits[i_y].iStart;
			uint u_end   = i_y * iStride + palmColLimits[i_y].iEnd;

			Assert(((uint)&pa2mAmplitude.atArray[u_index] & 1) == 0);
			Assert(((uint)&pa2mAmplitudePrev.atArray[u_index] & 1) == 0);
			Assert(((uint)&pa2mrVelocityFactor.atArray[u_index] & 1) == 0);

			Assert((u_end - u_index) & 1);

			for (; u_index <= u_end; u_index++)
			{
#if VER_ANISOTROPIC_RES
				// Different x/y densities.

				TMeasure mr_vf = pa2mrVelocityFactor.atArray[u_index];

				// Coefficient for current amplitude.
				TMeasure mr_a = mr_a_const - mr_vf * mr_a_factor;
				Assert(mr_a > 0);

				TMeasure mr_x = mr_vf * mr_x_factor;
				TMeasure mr_y = mr_vf * mr_y_factor;

				pa2mAmplitudePrev.atArray[u_index] = 
					mr_a * pa2mAmplitude.atArray[u_index] +
					mr_y * (pa2mAmplitude.atArray[u_index - iStride] + 
						    pa2mAmplitude.atArray[u_index + iStride]) +
					mr_x * (pa2mAmplitude.atArray[u_index - 1] + 
							pa2mAmplitude.atArray[u_index + 1]) +
			        mr_t * pa2mAmplitudePrev.atArray[u_index];
#else
				// Calculate coefficients for each iteration.
				TMeasure mr_x = pa2mrVelocityFactor.atArray[u_index] * s_step_sqr;

				// Coefficient for current amplitude.
				TMeasure mr_a = mr_a_const - 4.0 * mr_x;
				Assert(mr_a > 0);

				pa2mAmplitudePrev.atArray[u_index] = 
					(mr_a * pa2mAmplitude.atArray[u_index] +
					 mr_x * (pa2mAmplitude.atArray[u_index - iStride] + 
						     pa2mAmplitude.atArray[u_index + iStride] +
							 pa2mAmplitude.atArray[u_index - 1] + 
							 pa2mAmplitude.atArray[u_index + 1])
					) +
			        mr_t * pa2mAmplitudePrev.atArray[u_index];
				Assert(bValidAmp(pa2mAmplitude.atArray[u_index]));
#endif
			}
		}
#endif  // (VER_ASM)
	}

//**********************************************************************************************
//
// class CSimWater2D implementation.
//

	//******************************************************************************************
	namespace
	{
		const TMetres mMAX_VELOCITY_DEPTH	= 0.3;
		const TMetres mMIN_VELOCITY_DEPTH	= 0;

		//
		// Fudge factors for depth.
		// If mDEPTH_ADD is positive, this allows waves to propagate, even a little bit on the shore.
		// If negative, it makes water act shallower than it is, so reflections are more obvious.
		//
		const float		fDEPTH_MUL			= 1.0;
		const TMetres	mDEPTH_ADD			= 0;

		//******************************************************************************************
		TMeasure mrVelocityFromDepth(TMetres m_depth)
		{
			// Translate depth to velocity.
			if (m_depth <= 0)
				// If depth is 0 or negative, velocity must be 0, so wave doesn't transmit.
				return 0;

			// However, any non-zero depth must have some velocity, to simulate wave washing up.
			m_depth = MinMax(m_depth * fDEPTH_MUL + mDEPTH_ADD, mMIN_VELOCITY_DEPTH, mMAX_VELOCITY_DEPTH);

			return fSqrtEst(mrGRAVITY * m_depth);
		}
	};

	//******************************************************************************************
	CSimWater2D::CSimWater2D
	(
		int i_elements1, int i_elements2, TMetres m_length, TMetres m_width,
		TMetres m_depth, TMeasure mr_damping, TMeasure mr_restoring
	)
		: CSimWaveVar2D(i_elements1, i_elements2, 0, m_length, m_width, m_depth, 
						mr_damping, mr_restoring)
	{
		UpdateDepth(m_depth);

		//CreateDisturbance(mWidth * 0.4, mLength * 0.4, mWidth * 0.05, mWidth * 0.02);
	}

	//******************************************************************************************
	CSimWater2D::CSimWater2D
	(
		int i_elements1, int i_elements2, TMetres m_length, TMetres m_width,
		CPArray2<TMetres> pa2m_depth, TMeasure mr_damping, TMeasure mr_restoring
	)
		// Specify m_depth = 0.0 to avoid velocity calculations.
		: CSimWaveVar2D(i_elements1, i_elements2, 0, m_length, m_width, 0.0, 
						mr_damping, mr_restoring)
	{
		UpdateDepth(pa2m_depth);

		//CreateDisturbance(mWidth * 0.4, mLength * 0.4, mWidth * 0.05, mWidth * 0.02);
	}

	//******************************************************************************************
	CSimWater2D::CSimWater2D
	(
		int i_elements1, int i_elements2, const CSimWater2D& water2,
		CPArray2<TMetres> pa2m_depth
	)
		: CSimWaveVar2D(i_elements1, i_elements2, water2)
	{
		UpdateDepth(pa2m_depth);

		//CreateDisturbance(mWidth * 0.4, mLength * 0.4, mWidth * 0.05, mWidth * 0.02);
	}

	//******************************************************************************************
	void CSimWater2D::UpdateDepth(TMetres m_depth)
	{
		SetVelocity(mrVelocityFromDepth(m_depth));

		// Now update model according to new velocities.
		UpdateVelocity();
	}

	//******************************************************************************************
	void CSimWater2D::UpdateDepth(CPArray2<TMetres> pa2m_depth)
	{
		//
		// Translate depth to velocity, with interpolation.
		// Perform integral interpolation, ignoring any remainder.
		//
		int i_xavg = pa2m_depth.iWidth / (iWidth-2);
		int i_yavg = pa2m_depth.iHeight / (iHeight-2);
		Assert(i_xavg > 0 && i_yavg > 0);

		// Make sure we aren't ignoring too many elements.
//		Assert(pa2m_depth.iWidth  % (iWidth -2) < Max(pa2m_depth.iWidth /8, 8));
//		Assert(pa2m_depth.iHeight % (iHeight-2) < Max(pa2m_depth.iHeight/8, 8));

		TMeasure mr_scale = 1.0 / (i_xavg * i_yavg);

		for (int i_y = 1; i_y < iHeight-1; i_y++)
		{
			// Get index for start of corresponding depth row.
			// Depth array has no borders; entire depth array maps to wave array minus borders.
			int i_depth = pa2m_depth.iIndex(0, (i_y - 1) * i_yavg);
			for (int i_x = 1; i_x < iWidth-1; i_x++)
			{
				// Find avg depth.
				TMetres m_depth = 0.0;
				int i_deptha = i_depth;
				for (int i_ya = 0; i_ya < i_yavg; i_ya++)
				{
					for (int i_xa = 0; i_xa < i_xavg; i_xa++)
						//m_depth += pa2m_depth.tLinear(i_deptha + i_xa);
						SetMax(m_depth, pa2m_depth.tLinear(i_deptha + i_xa));
					i_deptha += pa2m_depth.iStride;
				}
				//m_depth *= mr_scale;

				SetVelocity(i_y, i_x, mrVelocityFromDepth(m_depth));
				i_depth += i_xavg;
			}
		}

		// Now update model according to new velocities.
		UpdateVelocity();
	}
