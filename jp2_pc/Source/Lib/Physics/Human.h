/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/Human.h                                                   $
 * 
 * 4     98/03/05 13:15 Speter
 * Modified Make_a_Pelvis and related functions to take opaque CInstance param, and init state
 * in quaternions rather than angles.
 * 
 * 3     97/12/08 22:33 Speter
 * In GetData functions, replaced matrix with quaternion, avoiding conversions on both ends.
 * 
 * 2     97/12/05 17:06 Speter
 * Added file header.  Moved stuff from Human_Functions.h here.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_HUMAN_HPP
#define HEADER_LIB_PHYSICS_HUMAN_HPP

class CInstance;

#define HUMAN_SKELETAL_ELEMENTS 19

//	Here we create the objects and parameters that make a human model and stick them together...
void Make_a_Human( CInstance* pins, int object, float init_state[7][3] );

//	Calculate the knee info, and the foot...
void Calculate_Human_Leg_Info( int pelvis, int foot, float points[3][3], float matricies[3][3][3] );

//	Calcualtes everything...
void Get_Human_Data( int model,
					 float loc[7],
					 float points[HUMAN_SKELETAL_ELEMENTS][3],
					 float matricies[HUMAN_SKELETAL_ELEMENTS][3][3],
					 int Am_I_Supported[HUMAN_SKELETAL_ELEMENTS] );

struct SHandData
// Passed from physics to Joe to construct hand box models.
{
	float afSize[3];
	float fMass;
	float afState[7];			// Position/Quaternion.
};

//  Give human model the info to create box hands.
void Get_Human_Hand_Data(int i_index, SHandData ahd[2]);

#endif