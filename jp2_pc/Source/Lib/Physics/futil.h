#pragma once

//	Foot utility header...
//	======================

#include "pelvis.h"

//	Transform (in place!) from Global to Local (pelvic) space...
inline void World_to_Local( float vec[3], int model )
{

float	a0 = Pel[model][3][0],								//Again, for organizationl reasons...
		a1 = Pel[model][4][0],
		a2 = Pel[model][5][0],
		a3 = Pel[model][6][0];

float	V[3] = { vec[0], vec[1], vec[2] };

		vec[0] = V[0] * ( a0*a0 + a1*a1 - a2*a2 - a3*a3 )
			   + V[1] * 2*(a1*a2 + a0*a3)
			   + V[2] * 2*(a1*a3 - a0*a2);

		vec[1] = V[0] * 2*(a1*a2 - a0*a3)
			   + V[1] * (a0*a0 - a1*a1 + a2*a2 - a3*a3)
			   + V[2] * 2*(a2*a3 + a0*a1);

		vec[2] = V[0] * 2*(a0*a2 + a1*a3)
			   + V[1] * 2*(a2*a3 - a0*a1)
			   + V[2] * ( a0*a0 - a1*a1 - a2*a2 + a3*a3 );

}



//	Transform (in place!) from Local (pelvic) frame to Global (world) frame...
inline void Local_to_World( float vec[3], int model )
{

float	a0 = Pel[model][3][0],								//Again, for organizationl reasons...
		a1 = Pel[model][4][0],
		a2 = Pel[model][5][0],
		a3 = Pel[model][6][0];

float	V[3] = { vec[0], vec[1], vec[2] };

		vec[0] = V[0] * ( a0*a0 + a1*a1 - a2*a2 - a3*a3 )
			   + V[1] * 2*(a1*a2 - a0*a3)
			   + V[2] * 2*(a1*a3 + a0*a2);

		vec[1] = V[0] * 2*(a1*a2 + a0*a3)
			   + V[1] * (a0*a0 - a1*a1 + a2*a2 - a3*a3)
			   + V[2] * 2*(a2*a3 - a0*a1);

		vec[2] = V[0] * 2*(-a0*a2 + a1*a3)
			   + V[1] * 2*(a2*a3 + a0*a1)
			   + V[2] * ( a0*a0 - a1*a1 - a2*a2 + a3*a3 );

}

//	Does exactly what it looks like it does...
inline void Local_to_World_Trans( float vec[3], int model )
{

float	a0 = Pel[model][3][0],								//Again, for organizationl reasons...
		a1 = Pel[model][4][0],
		a2 = Pel[model][5][0],
		a3 = Pel[model][6][0];

float	V[3] = { vec[0], vec[1], vec[2] };

		vec[0] = V[0] * ( a0*a0 + a1*a1 - a2*a2 - a3*a3 )
			   + V[1] * 2*(a1*a2 - a0*a3)
			   + V[2] * 2*(a1*a3 + a0*a2);

		vec[1] = V[0] * 2*(a1*a2 + a0*a3)
			   + V[1] * (a0*a0 - a1*a1 + a2*a2 - a3*a3)
			   + V[2] * 2*(a2*a3 - a0*a1);

		vec[2] = V[0] * 2*(-a0*a2 + a1*a3)
			   + V[1] * 2*(a2*a3 + a0*a1)
			   + V[2] * ( a0*a0 - a1*a1 - a2*a2 + a3*a3 );

		vec[0] += Pel[model][0][0];		//Why yes, I am a brain surgeon.
		vec[1] += Pel[model][1][0];
		vec[2] += Pel[model][2][0];

}

//	Transform from foot space to global...
inline void Foot_to_World( float vec[3], int index, int model )
{

float	a0 = Pel[model][(index+3)][0],	//Again, for organizationl reasons...
		a1 = Pel[model][(index+4)][0],
		a2 = Pel[model][(index+5)][0],
		a3 = Pel[model][(index+6)][0];

float	V[3] = { vec[0], vec[1], vec[2] };

		vec[0] = V[0] * ( a0*a0 + a1*a1 - a2*a2 - a3*a3 )
			   + V[1] * 2*(a1*a2 - a0*a3)
			   + V[2] * 2*(a1*a3 + a0*a2);

		vec[1] = V[0] * 2*(a1*a2 + a0*a3)
			   + V[1] * (a0*a0 - a1*a1 + a2*a2 - a3*a3)
			   + V[2] * 2*(a2*a3 - a0*a1);

		vec[2] = V[0] * 2*(-a0*a2 + a1*a3)
			   + V[1] * 2*(a2*a3 + a0*a1)
			   + V[2] * ( a0*a0 - a1*a1 - a2*a2 + a3*a3 );

}

//	Transform from global space to foot frame...
inline void World_to_Foot( float vec[3], int index, int model )
{

float	a0 = Pel[model][(index+3)][0],	//Again, for organizationl reasons...
		a1 = Pel[model][(index+4)][0],
		a2 = Pel[model][(index+5)][0],
		a3 = Pel[model][(index+6)][0];

float	V[3] = { vec[0], vec[1], vec[2] };

		vec[0] = V[0] * ( a0*a0 + a1*a1 - a2*a2 - a3*a3 )
			   + V[1] * 2*(a1*a2 + a0*a3)
			   + V[2] * 2*(a1*a3 - a0*a2);

		vec[1] = V[0] * 2*(a1*a2 - a0*a3)
			   + V[1] * (a0*a0 - a1*a1 + a2*a2 - a3*a3)
			   + V[2] * 2*(a2*a3 + a0*a1);

		vec[2] = V[0] * 2*(a0*a2 + a1*a3)
			   + V[1] * 2*(a2*a3 - a0*a1)
			   + V[2] * ( a0*a0 - a1*a1 - a2*a2 + a3*a3 );

}

//	Transform (IN PLACE!) from local frame to foot frame...
inline void Local_to_Foot( float vec[3], int index, int model )
{
	Local_to_World( vec, model );		//No, I'm really a Rocket Scientist!
	World_to_Foot( vec, index, model );
}


//	Transform (IN PLACE!) from foot to local (pelvic) frame...
inline void Foot_to_Local( float vec[3], int index, int model )
{
	Foot_to_World( vec, index, model );	//Actually, a Lawyer...
	World_to_Local( vec, model );
}

//	Just like Foot_to_World, but with a translation from the mother frame...
inline void Foot_to_World_Trans( float vec[3], int index, int model )
{
	Foot_to_World( vec, index, model );	//This does not take a lot of brains...

	vec[0] += Pel[model][(index+0)][0];
	vec[1] += Pel[model][(index+1)][0];
	vec[2] += Pel[model][(index+2)][0];

//	vec[0] += Pel[model][0][0];			//And this would be the translation part...
//	vec[1] += Pel[model][1][0];
//	vec[2] += Pel[model][2][0];

}

//	Calculate Euler velocities in Dirac frames...
inline void Get_Dirac_Vel( float result[3], int index, int model )
{

float	a0 = Pel[model][(index+0)][0],						//Again, for organizationl reasons...
		d0 = Pel[model][(index+0)][1],

		a1 = Pel[model][(index+1)][0],
		d1 = Pel[model][(index+1)][1],
		
		a2 = Pel[model][(index+2)][0],
		d2 = Pel[model][(index+2)][1],
		
		a3 = Pel[model][(index+3)][0],
		d3 = Pel[model][(index+3)][1];

		result[0] = 2*( a0*d1 + a3*d2 - a2*d3 - a1*d0 );	//alpha,
		result[1] = 2*(-a3*d1 + a0*d2 + a1*d3 - a2*d0 );	//beta,
		result[2] = 2*( a2*d1 - a1*d2 + a0*d3 - a3*d0 );	//gamma!

}

//	Find the total global velocity of a point in a foot frame...
void Global_Vel( float result[3], float coord[3], int index, int model );

//	Find the global (no pelvic contribution) velocity of a point in a foot frame...
void Global_Vel_NoPel( float result[3], float coord[3], int index, int model );

//	Here we handle requests for BC information.  For now, this is as stupid as it could possibly be!
void Foot_BC( float location[3], float vel[3], float result[3], int model, int i_body_part_index );

//	Test to see if a toe is intersecting the ground...
int Toe_Contact_Test( float location[3], int model );

//	Set a Dirac frame in terms of Euler parameters...
void Set_Dirac_From_Euler( int pelvis, int frame, float init[3][2] );

//	Here we will take input for control purposes!
void Pelvis_Control( int model, float move_direction[3], float move_speed, 
								float turn_direction[3], float turn_speed );

//	Find the perfect 2nd derivative for moving a mass to a point...
//	Return the length of target...
float Mass_Solution( float target[3], float pos[3], float vel[3],
				    float mass, float speed, float max_squared,
					float result[3] );

