//	Arms.h
//	======

#ifndef HEADER_LIB_PHYSICS_ARMS_HPP
#define HEADER_LIB_PHYSICS_ARMS_HPP

//	Argh.

//	Calculate the hand and arm...
void Calculate_Human_Arm_Info( int pelvis, int arm, float points[4][3], float matricies[4][3][3] );

//	Actual physical simulation...
void Update_Right_Arm( int pelvis, float timestep, float result[6] );

struct SControlArm
// Prefix: cta
{
	float	fPosUrgency;		// Whether to move.
	float	afPosition[3];		// In body space.
	float	fOrientUrgency;		// Whether to rotate.
	float	aafOrient[3][3];	// Matrix for hand orientation.
	float	fFingerCurl;		// In radians; should be negative.
	bool	bSwing;				// Whether arm is in swing mode.

	SControlArm()
		: fPosUrgency(0), fOrientUrgency(0), fFingerCurl(0), bSwing(false)
	{
	}
};

//	Process a trivial control event...
void Control_Arms( int pelvis, const SControlArm& cta );

#endif