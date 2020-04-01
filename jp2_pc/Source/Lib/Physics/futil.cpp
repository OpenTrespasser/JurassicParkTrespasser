//	Foot utilities...
//	=================

#include "Common.hpp"
#include "futil.h"
#include "PhysicsImport.hpp"

#include "Lib/Std/Set.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include <math.h>
#include <iostream>



// Some types...


CSet<uint32>		anne_this_terrain_bit;	//Collision latches...
CSet<uint32>		anne_last_terrain_bit;	//Collision latches...

//	Ok, here goes!
//	-=-=-=-=-=-=-=


//	+_+_+_+_+_+_+_+_+_
extern float Kontrol[NUM_PELVISES][6];
//	+_+_+_+_+_+_+_+_+_






//	Find the total global velocity of a point in a foot frame...
void Global_Vel( float result[3], float coord[3], int index, int model )
{

float	l_vel[3],
		r_vel[3];

//	This hurts worse that it looks...
	Get_Dirac_Vel( r_vel, (index + 3), model );		//Here is the foot local uncoupled velocity...
	Outer( r_vel, coord, l_vel );					//Ok, now it's linear, but we need it in Global...
	Foot_to_World( l_vel, index, model );			//Now global.
	
	result[0] = l_vel[0] + Pel[model][(index+0)][1];//Thank the maker these are globals!
	result[1] = l_vel[1] + Pel[model][(index+1)][1];
	result[2] = l_vel[2] + Pel[model][(index+2)][1];

}	

//	Find the global (no pelvic contribution) velocity of a point in a foot frame...
void Global_Vel_NoPel( float result[3], float coord[3], int index, int model )
{

float	l_vel[3],
		r_vel[3];

//	This hurts worse that it looks...
	Get_Dirac_Vel( r_vel, (index + 3), model );		//Here is the foot local uncoupled velocity...
	Outer( r_vel, coord, l_vel );					//Ok, now it's linear, but we need it in Global...
	Foot_to_World( l_vel, index, model );			//Now global.
	
	result[0] = l_vel[0];
	result[1] = l_vel[1];
	result[2] = l_vel[2];

}	

//	Here we handle requests for BC information.  For now, this is as stupid as it could possibly be!
//	------------------------------------------------------------------------------------------------
void Foot_BC( float location[3], float vel[3], float result[3], int model, int i_body_part )
{

#define MAX_MUL 10

//		Here is the call to get terrain info...
//		---------------------------------------
		CEntityWater* petw = 0;

		float water_height = PelBox(model, 0).fWaterHeight(location[0], location[1], &petw);
		float trr_height = PelBox(model, 0).fTerrainHeight(location[0], location[1]);




float	c = Pel_Data[model][2] - ( location[2] - trr_height );

		if ( c >= 0 )	//Contact!
		{
			if (c > MAX_MUL*Pel_Data[model][2]) c = MAX_MUL*Pel_Data[model][2];			//Safety wire!
			result[2] = Pel_Data[model][3]*c;
			float temp = - Pel_Data[model][4]*vel[2];
			if (temp < 0) temp = 0;
			result[2] += temp;
			result[0] =-Pel_Data[model][5]*vel[0];
			result[1] =-Pel_Data[model][5]*vel[1];


			// Sound collision here!
			if (!anne_this_terrain_bit[i_body_part] &&		// no collision this frame
				!anne_last_terrain_bit[i_body_part] &&		// no collision last frame
				model == 0)									// must be anne
			{
				NPhysImport::SendFootCollision
				(
					model,
					location,
					10.0,		// Energy.
					0.0			// Slide energy.
				);
//				dprintf("Boom! %d\n", i_body_part);
			}

			// Set this time's bit, please.  We have contact.
			anne_this_terrain_bit[i_body_part]  = true;
		}
		else result[0] = result[1] = result[2] = 0;


		//
		//  Water tests!
		//

float	f_foot_above_water = Pel_Data[model][2] - ( location[2] - water_height );

		// if  have water   &&  foot below water
		if ( petw && f_foot_above_water <= 0 ) 
		{
			// Create wave in water.
			float zd = 0.1f;
			NPhysImport::CreateDisturbance(petw, location[0], location[1], .5*.09/*BC_coarseness*/,-.015*zd, true);
		}
}

//	Test to see if a toe is intersecting the ground...
//	--------------------------------------------------
int Toe_Contact_Test( float location[3], int model )
{
int		ReturnVal = 0;
float	c = Pel_Data[model][2] - ( location[2] - NPhysImport::fPelTerrainHeight( model, location[0], location[1] ) );

		if ( c >= 0 ) ReturnVal = 1;	
		return ReturnVal;

}


//	Set a Dirac frame in terms of Euler parameters...
//	-------------------------------------------------
void Set_Dirac_From_Euler( int pelvis, int frame, float init[3][2] )
{

float	cos_heading = cos( .5*init[2][0] ),	sin_heading = sin( .5*init[2][0] ),
		cos_pitch   = cos( .5*init[1][0] ),	sin_pitch   = sin( .5*init[1][0] ),
		cos_bank	= cos( .5*init[0][0] ),	sin_bank	= sin( .5*init[0][0] );

	Pel[pelvis][(frame+3)][0] = cos_heading*cos_pitch*cos_bank + sin_heading*sin_pitch*sin_bank;
	Pel[pelvis][(frame+4)][0] = cos_heading*cos_pitch*sin_bank - sin_heading*sin_pitch*cos_bank;
	Pel[pelvis][(frame+5)][0] = cos_heading*sin_pitch*cos_bank + sin_heading*cos_pitch*sin_bank;
	Pel[pelvis][(frame+6)][0] =-cos_heading*sin_pitch*sin_bank + sin_heading*cos_pitch*cos_bank;

float	a0 = Pel[pelvis][(frame+3)][0],
		a1 = Pel[pelvis][(frame+4)][0],
		a2 = Pel[pelvis][(frame+5)][0],
		a3 = Pel[pelvis][(frame+6)][0];

	Assert(bWithin(a0*a0 + a1*a1 + a2*a2 + a3*a3, 0.99, 1.01));

	Pel[pelvis][(frame+3)][1] =-.5*( a1*init[0][1] + a2*init[1][1] + a3*init[2][1] );
	Pel[pelvis][(frame+4)][1] = .5*( a0*init[0][1] - a3*init[1][1] + a2*init[2][1] );
	Pel[pelvis][(frame+5)][1] = .5*( a3*init[0][1] + a0*init[1][1] - a1*init[2][1] );
	Pel[pelvis][(frame+6)][1] = .5*(-a2*init[0][1] + a1*init[1][1] + a0*init[2][1] );

}




//	Here we will take input for control purposes!
//	=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Pelvis_Control( int model, float move_direction[3], float move_speed, 
								float turn_direction[3], float turn_speed )
{

	Kontrol[model][0] = move_speed * move_direction[0];
	Kontrol[model][1] = move_speed * move_direction[1];
	Kontrol[model][2] = move_speed * move_direction[2];

	Kontrol[model][3] = turn_speed * turn_direction[0];
	Kontrol[model][4] = turn_speed * turn_direction[1];
	Kontrol[model][5] = turn_speed * turn_direction[2];
	
}


#if (1 || !VER_ASM || TARGET_PROCESSOR != PROCESSOR_K6_3D)

//	Find the perfect 2nd derivative for moving a mass to a point...
//	---------------------------------------------------------------
float Mass_Solution( float target[3], float pos[3], float vel[3],
				    float mass, float speed, float max_squared,
					float result[3] )
{

float	kappa = 150*mass,					//As always...
		delta_p = 1*sqrt( mass*kappa ),		//For placement,
		delta_t = 2.5*delta_p;				//For control only...
	
		delta_p /= speed;

float	stu, pid,	//Temporary vars...
		vmag = 0;	//See below...

float	T[3],		//Direction vector...
		Th[3],		//hat...
		Vt[3],		//Transverse velocity...
		v[3];		//The rest of the velocity...

		T[0] = target[0] - pos[0];	//Root moment...
		T[1] = target[1] - pos[1];
		T[2] = target[2] - pos[2];

		CopyVec( T, Th );
		SafeNormalize( Th, stu, pid );

		vmag = Inner( vel, Th );	//There's the projected velocity...

		v[0] = vmag*Th[0];	v[1] = vmag*Th[1];	v[2] = vmag*Th[2];

		Vt[0] = vel[0] - v[0];		//Now compute the transverse velocity...
		Vt[1] = vel[1] - v[1];
		Vt[2] = vel[2] - v[2];

//		Assemble the solution...
		result[0] = kappa*T[0] - delta_p*v[0] - delta_t*Vt[0];
		result[1] = kappa*T[1] - delta_p*v[1] - delta_t*Vt[1];
		result[2] = kappa*T[2] - delta_p*v[2] - delta_t*Vt[2];// + 10*mass;

float	mag = result[0]*result[0] + result[1]*result[1] + result[2]*result[2];
		if ( mag > max_squared )
		{
			float rescale = sqrt( max_squared / mag );
			result[0] *= rescale;
			result[1] *= rescale;
			result[2] *= rescale;
		}

		return stu;
}


#else // if (!VER_ASM || TARGET_PROCESSOR != PROCESSOR_K6_3D)


//	Find the perfect 2nd derivative for moving a mass to a point...
//	---------------------------------------------------------------
void Mass1_Solution( float target[3], float pos[3], float vel[3],
				    float mass, float speed, float max_squared,
					float result[3] )
{
	float	kappa,			//As always...
			delta_p,		//For placement,
			delta_t;		//For control only...

	float	T[3];			//Direction vector...

	float	f_150 = 150.0f;
	float	f_2_5 =   2.5f;
	float	f_0_000001 = 0.000001f;
	float	f_1_0 =   1.0f;

	__asm
	{
		jmp		StartAsm

		align 16
		nop										;establish 2 byte starting code offset
		nop

	StartAsm:
		femms									;ensure fast switch

		movd	mm4,[mass]						;m4= mass

		movd	mm7,[f_150]						;m7= 150.0

		movd	mm5,[speed]						;m5= speed

		pfmul	(m7,m4)							;m7= kappa
		mov		eax,[target]					;eax= ptr to target[]

		movd	[kappa],mm7
		mov		ebx,[pos]						;eax= ptr to pos[]

		pfmul	(m7,m4)							;m7= mass*kappa
		movq	mm0,[eax]						;m0= target[1] | target[0]

		pfrcp	(m5,m5)							;m5= 1/speed
		movd	mm1,[eax+8]						;m1= target[2]

		pfrsqrt	(m4,m7)							;m4= 1/sqrt(mass*kappa)
		movd	mm6,[f_2_5]						;m6= 2.5

		movq	mm2,[ebx]						;m2= pos[1] | pos[0]

		pfmul	(m4,m7)							;m4= delta_p = sqrt(mass*kappa)
		movd	mm3,[ebx+8]						;m3= pos[2]

		pfsub	(m0,m2)							;m0= T[1] | T[0]
		movq	[T],mm0

		pfmul	(m6,m4)							;m6= delta_t = 2.5*delta_p
		movd	[delta_t],mm6

		pfmul	(m4,m5)							;m4= delta_p = delta_p/speed
		movd	[delta_p],mm4

		pfsub	(m1,m3)							;m1= T[2]
		movd	[T+8],mm1

		movq	mm2,mm0							;m2=      T[1] | T[0]
		pfmul	(m0,m0)							;m0= T[1]*T[1] | T[0]*T[0]

		mov		ebx,[vel]						;ebx= ptr to vel[]
		movq	mm3,mm1							;m3= T[2]

		pfmul	(m1,m1)							;m1= T[2]*T[2]
		pfacc	(m0,m0)							;m0= T[0]*T[0]+T[1]*T[1]

		movd	mm7,[f_0_000001]				;m7= 0.000001

		movq	mm4,[ebx]						;m4= vel[1] | vel[0]
		pfadd	(m0,m1)							;m0= T[0]*T[0]+T[1]*T[1]+T[2]*T[2]

		movd	mm5,[ebx+8]						;m5= vel[2]
		punpckldq mm7,mm7						;m7= 0.000001 | 0.000001

		pfcmpgt	(m7,m0)							;m7= (0.000001 > lensqr(T)) ? 1's : 0's
		nop										;1-byte NOOP to avoid degraded predecode

		pfrsqrt	(m0,m0)							;m0= 1/len(T) | 1/len(T)


		pand	mm0,mm7							;m0= force 1/len(T) to 0.0 if len(T) is too small
		test	ebx,ebx							;2-byte NOOP to avoid degraded predecode

		pfmul	(m2,m0)							;m2= Th[1] | Th[0]
		movq	mm6,mm4							;m6= vel[1] | vel[0]

		pfmul	(m3,m0)							;m3= Th[2]
		movq	mm7,mm5							;m7= vel[2]

		pfmul	(m4,m2)							;m4= vel[1]*Th[1] | vel[0]*Th[0]
		nop										;1-byte NOOP to avoid degraded predecode

		pfmul	(m5,m3)							;m5= vel[2]*Th[2]
		nop										;1-byte NOOP to avoid degraded predecode

		pfacc	(m4,m4)							;m4= vel[0]*Th[0]+vel[1]*Th[1]
		movd	mm0,[delta_p]					;m0= delta_p

		movd	mm1,[delta_t]					;m1= delta_t
		pfadd	(m4,m5)							;m4= vmag

		punpckldq mm0,mm0						;m0= delta_p | delta_p
		punpckldq mm1,mm1						;m1= delta_t | delta_t

		pfmul	(m3,m4)							;m3= v[2]
		punpckldq mm4,mm4						;m4= vmag | vmag

		pfmul	(m2,m4)							;m2= v[1] | v[0]
		cmp		ebx,0							;3-byte NOOP to avoid degraded predecode

		pfsub	(m7,m3)							;m7= Vt[2]
		pfmul	(m3,m0)							;m3= delta_p*v[2]

		pfsub	(m6,m2)							;m6= Vt[1] | Vt[0]
		pfmul	(m2,m0)							;m2= delta_p*v[1] | delta_p*v[0]

		movd	mm0,[kappa]						;m0= kappa
		pfmul	(m7,m1)							;m7= delta_t*Vt[2]

		movd	mm5,[T+8]						;m5= T[2]
		pfmul	(m6,m1)							;m6= delta_t*Vt[1] | delta_t*Vt[0]

		movq	mm4,[T]							;m4= T[1] | T[0]
		punpckldq mm0,mm0						;m0= kappa | kappa

		pfadd	(m7,m3)							;m7= delta_p*v[2]+delta_t*Vt[2]
		pfmul	(m5,m0)							;m5= kappa*T[2]

		pfadd	(m6,m2)							;m6= delta_p*v[1]+delta_t*Vt[1] | delta_p*v[0]+delta_t*Vt[0]
		pfmul	(m4,m0)							;m4= kappa*T[1] | kappa+T[0]

		pfsub	(m5,m7)							;m5= result[2]
		movd	mm1,[speed]						;m1= speed

		pfsub	(m4,m6)							;m4= result[1] | result[0]
		nop										;1-byte NOOP to avoid degraded predecode

		pfmul	(m5,m1)							;m5= speed*result[2]
		punpckldq mm1,mm1						;m1= speed | speed

		pfmul	(m4,m1)							;m4= speed*result[1] | speed*result[0]

		movq	mm7,mm5							;m7= result'[2]
		pfmul	(m5,m5)							;m5= result'[2]*result'[2]

		movq	mm6,mm4							;m6= result'[1] | result'[0]
		pfmul	(m4,m4)							;m4= result'[1]*result'[1] | result'[0]*result'[0]

		test	ebx,ebx							;2-byte NOOP to avoid degraded predecode
		movd	mm0,[max_squared]				;m0= max_squared

		pfacc	(m4,m4)							;m4= result'[0]*result'[0]+result'[1]*result'[1]

		pfrcp	(m1,m0)							;m1= 1/max_squared

		pfadd	(m4,m5)							;m4= mag
		movd	mm2,[f_1_0]						;m2= 1.0


		pfmul	(m4,m1)							;m4= mag/max_squared
		nop										;1-byte NOOP to avoid degraded predecode


		pfrsqrt	(m4,m4)							;m4= 1/sqrt(mag/max_squared) | ...
												;  = rescale | rescale
		punpckldq mm2,mm2						;m2= 1.0 | 1.0
		mov		ebx,[result]					;ebx= ptr to result[]

		pfmin	(m4,m2)
		test	ebx,ebx							;2-byte NOOP to avoid degraded predecode


		pfmul	(m6,m4)							;m6= result''[1] | result''[0]
		movq	[ebx],mm6

		pfmul	(m7,m4)							;m7= result''[2]
		movd	[ebx+8],mm7

		femms									;empty MMX state and ensure fast switch
	}
}

#endif // else
