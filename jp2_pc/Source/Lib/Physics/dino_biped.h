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
 * $Log:: /JP2_PC/Source/Lib/Physics/dino_biped.h                                              $
 * 
 * 11    8/23/98 12:49p Sblack
 * 
 * 10    6/09/98 5:03p Sblack
 * 
 * 9     6/03/98 5:16p Sblack
 * 
 * 8     5/11/98 11:37p Sblack
 * 
 * 7     4/27/98 6:35p Sblack
 * 
 * 6     4/26/98 1:28p Sblack
 * 
 * 5     98/03/05 13:14 Speter
 * Modified Make_a_Pelvis and related functions to take opaque CInstance param, and init state
 * in quaternions rather than angles.
 * 
 * 4     12/18/97 2:03p Sblack
 * 
 * 3     12/17/97 2:04p Sblack
 * 
 * 2     97/12/08 22:35 Speter
 * In GetData functions, replaced matrix with quaternion, avoiding conversions on both ends.
 * 
 * 1     97/12/05 17:50 Speter
 * 
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_DINO_BIPED_HPP
#define HEADER_LIB_PHYSICS_DINO_BIPED_HPP

class CInstance;

#define RAPTOR_SKELETAL_ELEMENTS 19
#define QUAD_SKELETAL_ELEMENTS 28

//	Data read from imported skeletons...
//	====================================
#include "BioStructure.h"

//	Bipeds:
//	=======
//	Here we create the objects and parameters that make a biped model and stick them together...
void Make_a_Raptor( CInstance* pins, int object, Biped_Params& params, float init_state[7][3] );

//	Calcualtes everything...
void Get_Raptor_Data( int model,
					 float loc[7],
					 float points[RAPTOR_SKELETAL_ELEMENTS][3],
					 float matricies[RAPTOR_SKELETAL_ELEMENTS][3][3],
					 int Am_I_Supported[RAPTOR_SKELETAL_ELEMENTS] );

//	Calculate the knee info, and the foot...
void Calculate_Raptor_Leg_Info( int pelvis, int foot, float points[3][3], float matricies[3][3][3] );


//	Quads:
//	======
//	Here we create the objects and parameters that make a QUAD model and stick them together...
void Make_a_Quad( CInstance* pins, int object, Quad_Params& params, float init_state[7][3] );

//	Calcualtes everything...
void Get_Quad_Data( int model,
				    float loc[7],
				    float points[QUAD_SKELETAL_ELEMENTS][3],
				    float matricies[QUAD_SKELETAL_ELEMENTS][3][3],
				    int Am_I_Supported[QUAD_SKELETAL_ELEMENTS] );

//	Calculate the knee info, and the foot...
void Calculate_Quad_Leg_Info( int pelvis, int foot, float points[3][3], float matricies[3][3][3] );


//	Both...
//	=======

//	First random stab at skeletal AI control...
void RaptorBodyControl( int dino,
					    float nose[3], float head_angle, float head_move_to,
						float wagamplitude, float wagfrequency,
						float mouth_angle,
						float headup );

#endif
 