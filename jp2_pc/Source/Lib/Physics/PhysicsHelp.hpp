/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Definitions private to the physics system.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/PhysicsHelp.hpp                                           $
 * 
 * 15    10/01/98 12:34a Asouth
 * Changed syntax of reinterpret cast
 * 
 * 14    98/09/28 2:48 Speter
 * Forced these inline.
 * 
 * 13    98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 12    98/06/25 17:41 Speter
 * Removed old code.
 * 
 * 11    98/06/14 17:26 Speter
 * Added simple placement conversion.
 * 
 * 10    98/03/05 13:20 Speter
 * Removed unused function.
 * 
 * 9     98/02/10 13:19 Speter
 * Added switch for defining physics arrays via CSArray, for bounds checking.
 * 
 * 8     97/12/03 17:53 Speter
 * p3Convert no longer normalises quaternion (so velocity can work).
 * 
 * 7     97/11/12 17:12 Speter
 * Added p3Convert taking single-dim array.
 * 
 * 6     97/10/24 15:38 Speter
 * Added CVector3<> conversion functions accessing State array.
 * 
 * 5     97/10/15 21:56 Speter
 * Changed aaf_state[][] arg of Copy() to be [7][3], like State[][], so it's more generally
 * useful.
 * 
 * 4     97/10/10 15:18 Speter
 * Changed p3Convert to use non-renormalising CRotate3 constructor.
 * 
 * 3     97/10/08 14:56 Speter
 * Removed CQuaternion intermediate class, since CRotate3 can now be initialised directly with 4
 * floats.  Added CPlacement3 to float-array conversion.
 * 
 * 2     97/09/16 19:47 Speter
 * Added functions for CMatrix3 data conversions.
 * 
 * 1     97/09/05 0:39 Speter
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_PHYSICSHELP_HPP
#define HEADER_LIB_PHYSICS_PHYSICSHELP_HPP

#include "Lib/Transform/Transform.hpp"

//**********************************************************************************************
//
// Handy conversion functions between classes and arrays.
//

	//
	// Vector functions.
	//

	//**********************************************************************************************
	//
	forceinline float* afConvert
	(
		CVector3<>& v3
	)
	{
		return &v3.tX;
	}

	//**********************************************************************************************
	//
	forceinline CVector3<>& v3Convert
	(
		float af[3]
	)
	{
		return reinterpret_cast<CVector3<>&>(af[0]);
	}

	//**********************************************************************************************
	//
	forceinline const CVector3<>& v3Convert
	(
		const float af[3]
	)
	{
		return *(reinterpret_cast<const CVector3<>*>(af));
	}

	//******************************************************************************************
	//
	forceinline void Copy
	(
		float af[3],
		const CVector3<>& v3
	)
	{
		af[0] = v3.tX;
		af[1] = v3.tY;
		af[2] = v3.tZ;
	}

	//
	// Vector functions accessing State[][] array.
	//

	//**********************************************************************************************
	//
	forceinline CVector3<> v3Convert
	(
		const float aaf_state[7][3],		// Physics system's state info.
		int i_deriv
	)
	{
		return CVector3<>(aaf_state[0][i_deriv], aaf_state[1][i_deriv], aaf_state[2][i_deriv]);
	}

	//
	// Placement functions.
	//

	//*****************************************************************************************
	//
	forceinline const CPlacement3<>& p3Convert
	(
		const float	af_state[7]			// Physics system's state info.
	)
	//
	// Returns:
	//		The equivalent placement.
	//
	//**********************************
	{
		return *(reinterpret_cast<const CPlacement3<>*>(af_state));
	}

	//*****************************************************************************************
	//
	forceinline CPlacement3<> p3Convert
	(
		const float	aaf_state[7][3],	// Physics system's state info.
		int			i_deriv = 0			// Derivative to extract (second index).
	)
	//
	// Returns:
	//		The equivalent placement.
	//
	//**********************************
	{
		Assert(bWithin(i_deriv, 0, 2));

		return CPlacement3<>
		(
			// Set quaternion normalisation 'false'.
			CRotate3<>(aaf_state[3][i_deriv], aaf_state[4][i_deriv], aaf_state[5][i_deriv], aaf_state[6][i_deriv], false),
			CVector3<>(aaf_state[0][i_deriv], aaf_state[1][i_deriv], aaf_state[2][i_deriv])
		);
	}

	//*****************************************************************************************
	//
	forceinline void Copy
	(
		float	af_state[7],			// Physics system's state info.
		const	CPlacement3<>& p3		// World placement info.
	)
	//
	// Copies the placement information into the float array.
	//
	//**********************************
	{
		// Position.
		af_state[0] = p3.v3Pos.tX;
		af_state[1] = p3.v3Pos.tY;
		af_state[2] = p3.v3Pos.tZ;

		// Rotation.
		af_state[3] = p3.r3Rot.tC;
		af_state[4] = p3.r3Rot.v3S.tX;
		af_state[5] = p3.r3Rot.v3S.tY;
		af_state[6] = p3.r3Rot.v3S.tZ;
	}

	//*****************************************************************************************
	//
	forceinline void Copy
	(
		float	aaf_state[7][3],		// Physics system's state info.
		const	CPlacement3<>& p3,		// World placement info.
		int		i_deriv = 0				// Derivative to copy to (second index of aaf_state).
	)
	//
	// Copies the placement information into the float array.
	//
	//**********************************
	{
		Assert(bWithin(i_deriv, 0, 1));

		// Position.
		aaf_state[0][i_deriv] = p3.v3Pos.tX;
		aaf_state[1][i_deriv] = p3.v3Pos.tY;
		aaf_state[2][i_deriv] = p3.v3Pos.tZ;

		// Rotation.
		aaf_state[3][i_deriv] = p3.r3Rot.tC;
		aaf_state[4][i_deriv] = p3.r3Rot.v3S.tX;
		aaf_state[5][i_deriv] = p3.r3Rot.v3S.tY;
		aaf_state[6][i_deriv] = p3.r3Rot.v3S.tZ;
	}

/*
	// For future use?
	inline void convert_hpb_vel_2_q_vel(float heading_vel, float pitch_vel, float bank_vel,
										const CQuaternion<>& pq_pos, CQuaternion<> *pq_vel)
	{
		pq_vel->tE0 =-.5*( pq_pos.tE1*heading_vel + pq_pos.tE2*pitch_vel + pq_pos.tE3*bank_vel );
		pq_vel->tE1 = .5*( pq_pos.tE0*heading_vel - pq_pos.tE3*pitch_vel + pq_pos.tE2*bank_vel );
		pq_vel->tE2 = .5*( pq_pos.tE3*heading_vel + pq_pos.tE0*pitch_vel - pq_pos.tE1*bank_vel );
		pq_vel->tE3 = .5*(-pq_pos.tE2*heading_vel + pq_pos.tE1*pitch_vel + pq_pos.tE0*bank_vel );
	}
*/


	//
	// Matrix functions.
	//

	//*****************************************************************************************
	//
	forceinline CMatrix3<> mx3Convert
	(
		const float	aaf_matrix[3][3]		// Matrix array; in [dest][source] format, reverse of CMatrix3.
	)
	//
	// Returns:
	//		The equivalent CMatrix3<>.
	//
	//**********************************
	{
		return CMatrix3<>
		(
			aaf_matrix[0][0], aaf_matrix[1][0], aaf_matrix[2][0], 
			aaf_matrix[0][1], aaf_matrix[1][1], aaf_matrix[2][1], 
			aaf_matrix[0][2], aaf_matrix[1][2], aaf_matrix[2][2]
		);
	}

	//*****************************************************************************************
	//
	forceinline void Copy
	(
		float	aaf_matrix[3][3],		// Matrix array; in [dest][source] format, reverse of CMatrix3.
		const	CMatrix3<>& mx3
	)
	//
	// Copies mx3 transform info to aaf_matrix, in the format physics expects.
	//
	//**********************************
	{
		aaf_matrix[0][0] = mx3.v3X.tX; 
		aaf_matrix[1][0] = mx3.v3X.tY;
		aaf_matrix[2][0] = mx3.v3X.tZ;

		aaf_matrix[0][1] = mx3.v3Y.tX; 
		aaf_matrix[1][1] = mx3.v3Y.tY;
		aaf_matrix[2][1] = mx3.v3Y.tZ;

		aaf_matrix[0][2] = mx3.v3Z.tX; 
		aaf_matrix[1][2] = mx3.v3Z.tY;
		aaf_matrix[2][2] = mx3.v3Z.tZ;
	}

#endif
