//	BC intensive stuff...
//	=====================

#include "Common.hpp"
#include "Xob_bc.hpp"

#include "PhysicsImport.hpp"
#include "PhysicsStats.hpp"
#include "Pelvis.h"

#include "Lib/Audio/SoundDefs.hpp"
#include "Lib/Std/Set.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include <memory.h>
#include <float.h>
#include <math.h>

#pragma warning( disable : 4244 )  

CXob	Xob[GUYS];								// The array of xobs.

float   fBreakage = 100000.0f;					//Min breakage constant 
												//(overrides Breakages and allows easy debug adjustment).
bool	bStoreFric = false;						//If this is the player's foot, we need to do extra work...
float	fFricStore[3] = {0,0,0};				//The place to store it...

int		iImmovCode = 0;							//Test for making thinkgs over the design limit unmovable by Anne...

float	BioTag[NUM_PELVISES][PELVIS_DOF];		//Applied inputs to BioTags...

float	vsq_store = 0;							//Temp storage for collision messages...
float	vsqt_store = 0;

float	current_timeslice = 0;					//Danger will robinson...


//		Latching NOT needed for some things...
bool	IntegratingNow = false;

//		Global parameters should NOT be object specific...
//		--------------------------------------------------
float	SleepL1	= Sqr(1.0),					// Coefficient for linear velocity.
		SleepL2	= Sqr(2.0 * MAX_TIMESTEP),	// Linear accel.
		SleepA1	= Sqr(1.0),					// Angular vel.
		SleepA2 = Sqr(2.0 * MAX_TIMESTEP);	// Angular accel.
float	PHY_sleep = Sqr(.02);				// Approx. vel squared at which sleep occcurs.
const int INFO_MAX = 3;

const float fSLEEP_DEFAULT	= PHY_sleep;	// Remember default value, because we modify it by number of boxes.

#define	GLOBAL_MU .005

float	PHY_number1 = 200;
float	PHY_number2 = 20;
float	BC_coarseness = .04,//.09,
		BC_bounce = .25,//     .4,//.5,//.5,

		IBC_coarseness = .09,
		IBC_bound = 0.4*IBC_coarseness,		// Current constant used to adjust internal extents.
		one_by_IBC = 1/IBC_coarseness,
		IBC_depth  = 1,//                               .5,//1.5,//1.25,//1.5,
		LIBC_kappa = 1.4,//    2.18,//1,//-8.76,//10,
		LIBC_damp  = 1.4,//.58,//1.4,//     .80,//.58,//.2,//-4.92,//.1,
		L_slide    = 1.0,//0,//.25,//0,//1,
		IBC_kappa  = 1.4,//    2.44,//1,//2.53,//2,
		IBC_damp   = 1.4,//2.44,//-1,//1.83,//-1;//-.3;
		IBC_slide  = -1;//-1;// -.26;

//		Collision thresholds for energy...
//		These are smaller values than audio triggers off of, but are neeeded for collision triggers, etc.
float	Collide_min_hit		= .01,
		Collide_min_slide	= .001;


float	OurKappa = 0,
		OurDelta = 0,
		OurMu    = 0;

//		For tuning to new timestamps...
float	L_Mult = .5,
		L_dot_mult =.1;


//		Jumping fix...
int		iJumpCode = -1;
float	jumptable[6][3] = { {0, 0, 1}, {0, 0,-1},
							{1, 0, 0}, {-1, 0, 0},
							{0, 1, 0}, {0,-1, 0} };
float	JTnorm[3] = {0,0,0};
bool	OKtoJUMP = false;

//		General floating point tolerances...
//		------------------------------------
float	ORTHO_TOLERANCE = .0001;
float	PARA_TOLERANCE  = .0001; 
float	DIV_TOL			= .0001;

extern CSet<uint32>				anne_this_terrain_bit;				//Collision latches...
extern CSet<uint32>				anne_last_terrain_bit;				//Collision latches...

struct SCollision
// Stored box collision event info.
{
	int			Element1, Element2;		// The sub-elements of the box collision.
	float		EnergyMax;				// Maximum normal collision power for any physics frame.
	float		EnergyHit;				// Total normal collision energy.
	float		EnergySlide;			// Total transverse (sliding) energy.
	float		Location[3];			// World position of collision (currently set only for terrain/water).
	CSet<int>	ElementCollide;			// Flag the collision of each element with other instances.
	CSet<int>	LastElementCollide;		// Flags in previous frame, for latching.
};

#define WATER			GUYS			// 1 entry for water.
#define TERRAIN			(GUYS+1)		// 8 entries: one per corner of colliding box.
#define COLLISIONS		(TERRAIN+8)		// Total entries.
#define COLLISION_DIM	(COLLISIONS+3)	// Round up to 32 for efficient access.

// Store collision info for each combination of boxes, plus water and terrain.
// Second dimension is rounded up to 32 for efficient access.
SCollision		BoxCollisions[GUYS][COLLISION_DIM];
CInstance*		WaterCollide[GUYS];		// Water entity this collided with.


bool bPhysicsIntegrating;				// Lock used for debugging.


//-----ACTUAL CODE STARTS-----



//	Does what it says, says what it does...
//	---------------------------------------
inline void minmax( float A, float B, float &min, float &max )
{
	if (A >= B) 
	{
		max = A;
		min = B;
	}
	else
	{
		max = B;
		min = A;
	}
}	//	What else do you want?



void CXob::Stuff_Terr_Moments(
						 float mine[3],						//My point of contact,
						 float BC_n[3],						//The normal of the BC vector,
						 float BC_val,						//The macgitude of the contact,
						 float ad,  float bd,  float gd		//My angle-converted first moments...
						 )
{

float	vel[3];

		float test = BC_n[0]*mine[0] + BC_n[1]*mine[1] + BC_n[2]*mine[2];
		if ( test < 0 )
		{

			BC_n[0] *= -1;
			BC_n[1] *= -1;
			BC_n[2] *= -1;
			BC_val = IBC_coarseness;
		}			


		vel[0] = - State[0][1];							//Should this me a minus?
		vel[1] = - State[1][1];							//This can be done outside the loop...
		vel[2] = - State[2][1];

		From( vel[0], vel[1], vel[2] );			//Local to me now...

		vel[2] -= (ad*mine[1] - bd*mine[0]);				//My rotation at your contact...
		vel[1] -= (gd*mine[0] - ad*mine[2]);
		vel[0] -= (bd*mine[2] - gd*mine[1]);

float	v_dotp = BC_n[0]*vel[0] + BC_n[1]*vel[1] + BC_n[2]*vel[2];

float	Vp[3]  = { v_dotp*BC_n[0], v_dotp*BC_n[1], v_dotp*BC_n[2] };
float	Vt[3] =  { vel[0] - Vp[0], vel[1] - Vp[1], vel[2] - Vp[2] };

float	lin_comp = LIBC_kappa*Data[1]*(IBC_coarseness - BC_val);
float	lin_damp = LIBC_damp*Data[2];
float	lin_slid = L_slide*Data[2];

		if (v_dotp > 0) lin_damp *= BC_bounce;

		vsq_store  = Vp[0]*Vp[0] + Vp[1]*Vp[1] + Vp[2]*Vp[2];	//For the collision message above...
		vsqt_store = Vt[0]*Vt[0] + Vt[1]*Vt[1] + Vt[2]*Vt[2];	//For the collision message above...

		if (vsqt_store + vsq_store < .001) lin_slid*=2;

		vsq_store  *= current_timeslice*lin_damp;
		vsqt_store *= current_timeslice*lin_slid;

float	Fuerzo[3]  = { lin_comp*BC_n[0] - lin_damp*Vp[0] - lin_slid*Vt[0],
					   lin_comp*BC_n[1] - lin_damp*Vp[1] - lin_slid*Vt[1],
					   lin_comp*BC_n[2] - lin_damp*Vp[2] - lin_slid*Vt[2] };

		Xin[1][0] -= mine[1]*Fuerzo[2] - mine[2]*Fuerzo[1];
		Xin[1][1] -= mine[2]*Fuerzo[0] - mine[0]*Fuerzo[2];
		Xin[1][0] -= mine[0]*Fuerzo[1] - mine[1]*Fuerzo[0];

		Back( Fuerzo[0], Fuerzo[1], Fuerzo[2] );

		//conPhysics << "Fuerzo: " << Fuerzo[0] << ", " << Fuerzo[1] << ", " << Fuerzo[2] << "\n";

		Xin[0][0] -= Fuerzo[0];
		Xin[0][1] -= Fuerzo[1];
		Xin[0][2] -= Fuerzo[2];
}


void CXob::scrub_state( float ad, float bd, float gd, float a0, float a1, float a2, float a3 )
{
	State[3][1] =-.5*( a1*ad + a2*bd + a3*gd );
	State[4][1] = .5*( a0*ad - a3*bd + a2*gd );
	State[5][1] = .5*( a3*ad + a0*bd - a1*gd );
	State[6][1] = .5*(-a2*ad + a1*bd + a0*gd );
	//conPhysics << "AAAAAAAAAAAAAAHHHHHHHHHHHHHHHHH!!!!!!!!!!!!!!!!!!!!\n";
}

void CXob::KillRVel( float V[3], float P[3] )
{


//	V in world, P in local...
//	-------------------------
float	R[3];
float	mag;

float	a0 = State[3][0],
		a1 = State[4][0],
		a2 = State[5][0],
		a3 = State[6][0];

float	d0 = State[3][1],
		d1 = State[4][1],
		d2 = State[5][1],
		d3 = State[6][1];

float	W[3] = { 2*( a0*d1 + a3*d2 - a2*d3 - a1*d0 ),
				 2*(-a3*d1 + a0*d2 + a1*d3 - a2*d0 ),
				 2*( a2*d1 - a1*d2 + a0*d3 - a3*d0 ) };

		From( V[0], V[1], V[2] );

		//Normalize( V, weird, science );
		Outer( P, V, R );
		mag = Inner( R, W );	//Component...

//		Do the work if needed...
		if (mag > 0)
		{
			conPhysics << "Scrub Rotational: " << mag << "\n";
			W[0] -= mag*R[0];
			W[1] -= mag*R[1];
			W[2] -= mag*R[2];
			scrub_state( W[0], W[1], W[2], State[3][0], State[4][0], State[5][0], State[6][0] );
		}
}

#define IBC_LIM .805
//	Some stuff...
void CXob::plane_interp( float Px, float Py, float Pz, float Vx, float Vy, float Vz, CXob& you )
{

	
			Back( Vx, Vy, Vz );

			float dp = Vx*State[0][1] + 
					   Vy*State[1][1] +
					   Vz*State[2][1];

			if (dp > 0)
			{
				State[0][1] -= dp*Vx;
				State[1][1] -= dp*Vy;
				State[2][1] -= dp*Vz;
			}

			dp = Vx*you.State[0][1] + 
				 Vy*you.State[1][1] +
				 Vz*you.State[2][1];

			if (dp < 0)
			{
				you.State[0][1] -= dp*Vx;
				you.State[1][1] -= dp*Vy;
				you.State[2][1] -= dp*Vz;
			}

//float		Vel[3] = { Vx, Vy, Vz },
//			Pos[3] = { Px, Py, Pz };

			//KillRVel( Vel, Pos );	//Have to do you also...

						
			//if (Anchored)  State[3][1]  = State[4][1]  = State[5][1]  = State[6][1]  = 0;
			//if (you.Anchored) you.State[3][1] = you.State[4][1] = you.State[5][1] = you.State[6][1] = 0;

			//if (!Anchored)  State[3][1]  = State[4][1]  = State[5][1]  = State[6][1]  = 0;
			//if (!you.Anchored) you.State[3][1] = you.State[4][1] = you.State[5][1] = you.State[6][1] = 0;

			conPhysics << "Planar!!!\n";

}



//	 And here we fill the external BC matricies...
// 	 ---------------------------------------------
void CXob::Stuff_Contacts( CXob& you,
					float Xdotdot, float Ydotdot, float Zdotdot,
					float V_x, float V_y, float V_z,
					float A, float B, float C,
					int superme, int superyou ) 
{


//		Test, needs cleanup, maybe...  Here NO FOOT INVOLVED...
		if ( !bStoreFric )
		{

float vect[3] = { V_y*Zdotdot - V_z*Ydotdot,
				  V_z*Xdotdot - V_x*Zdotdot,
				  V_x*Ydotdot - V_y*Xdotdot };

			TTotal[superme][0] -= vect[0];
			TTotal[superme][1] -= vect[1];
			TTotal[superme][2] -= vect[2];

			if (iImmovCode != 2)
			{
				Xin[1][0] -= vect[0];									//Add moments for ME...
				Xin[1][1] -= vect[1];
				Xin[1][2] -= vect[2];
			}

			Back( Xdotdot, Ydotdot, Zdotdot );				//Now global...
 
			if (iImmovCode != 2)
			{
				Xin[0][0]  -= Xdotdot;
				Xin[0][1]  -= Ydotdot;
				Xin[0][2]  -= Zdotdot;
			}

			if (iImmovCode != 1)
			{
				you.Xin[0][0] += Xdotdot;
				you.Xin[0][1] += Ydotdot;
				you.Xin[0][2] += Zdotdot;
			}

			you.From( Xdotdot, Ydotdot, Zdotdot );				//Back to you...

float vect1[3] = { B*Zdotdot - C*Ydotdot,
				   C*Xdotdot - A*Zdotdot,
				   A*Ydotdot - B*Xdotdot };

			you.TTotal[superyou][0] += vect1[0];
			you.TTotal[superyou][1] += vect1[1];
			you.TTotal[superyou][2] += vect1[2];

			if (iImmovCode != 1)
			{
				you.Xin[1][0] += vect1[0];									//Add moments for YOU...
				you.Xin[1][1] += vect1[1];
				you.Xin[1][2] += vect1[2];
			}

		}//FricStore test...
		else
		{

//	--------------------------------------------------------------
	//CheckForJump( superme );
	if (iJumpCode != 6)
	{
		JTnorm[0] = jumptable[iJumpCode][0];
		JTnorm[1] = jumptable[iJumpCode][1];
		JTnorm[2] = jumptable[iJumpCode][2];
	}

	Back_Element( JTnorm[0], JTnorm[1], JTnorm[2], superme );
	Back( JTnorm[0], JTnorm[1], JTnorm[2] );

	//conPhysics << JTnorm[0] << ", " << JTnorm[1] << ", " << JTnorm[2] << "\n";

	if ( JTnorm[2] > .7 ) OKtoJUMP = true;//conPhysics << "OK!\n";
	else conPhysics << "NO!\n";

	//Here I should have the world vector...
//	--------------------------------------------------------------


float		XdotdotP,
			YdotdotP,
			ZdotdotP;

			if (PelvisElem == RIGHT_FOOT)	//me is the foot...
			{
				XdotdotP = Xdotdot - fFricStore[0];
				YdotdotP = Ydotdot - fFricStore[1];
				ZdotdotP = Zdotdot - fFricStore[2];

				Zdotdot -= fFricStore[2];
			}
			else								//you is the foot...
			{
				XdotdotP = Xdotdot;
				YdotdotP = Ydotdot;
				ZdotdotP = Zdotdot;

				Xdotdot -= fFricStore[0];
				Ydotdot -= fFricStore[1];
				Zdotdot -= fFricStore[2];

				ZdotdotP-= fFricStore[2];

			}

			fFricStore[0] = fFricStore[1] = fFricStore[2] = 0;


float vect[3] = { V_y*Zdotdot - V_z*Ydotdot,
				  V_z*Xdotdot - V_x*Zdotdot,
				  V_x*Ydotdot - V_y*Xdotdot };

				TTotal[superme][0] -= vect[0];
				TTotal[superme][1] -= vect[1];
				TTotal[superme][2] -= vect[2];


				if (iImmovCode != 2)
				{
					Xin[1][0] -= vect[0];									//Add moments for ME...
					Xin[1][1] -= vect[1];
					Xin[1][2] -= vect[2];
				}

				Back( Xdotdot, Ydotdot, Zdotdot );				//Now global...
 
				if (iImmovCode != 2)
				{
					Xin[0][0]  -= Xdotdot;
					Xin[0][1]  -= Ydotdot;
					Xin[0][2]  -= Zdotdot;
				}

				Back( XdotdotP, YdotdotP, ZdotdotP );				//Now global...

				if (iImmovCode != 1)
				{
					you.Xin[0][0] += XdotdotP;
					you.Xin[0][1] += YdotdotP;
					you.Xin[0][2] += ZdotdotP;
				}

				you.From( XdotdotP, YdotdotP, ZdotdotP );				//Back to you...

float vect1[3] = { B*ZdotdotP - C*YdotdotP,
				   C*XdotdotP - A*ZdotdotP,
				   A*YdotdotP - B*XdotdotP };

				you.TTotal[superyou][0] += vect1[0];
				you.TTotal[superyou][1] += vect1[1];
				you.TTotal[superyou][2] += vect1[2];

				if (iImmovCode != 1)
				{
					you.Xin[1][0] += vect1[0];									//Add moments for YOU...
					you.Xin[1][1] += vect1[1];
					you.Xin[1][2] += vect1[2];
				}

				//Wz  -= Xin[0][2];
				//you.Wz -= you.Xin[0][2];

		}//End of foot positive case...

		// For each unique superbox pair, store only the largest collision per frame.
		int i_me = Index(),
			i_you = you.Index();

		if (i_me > i_you)
		{
			Swap(i_me, i_you);
			Swap(superme, superyou);
		}

SCollision*	pcoll = &BoxCollisions[i_me][i_you];

		if (vsq_store > 0)
		{
			// Track collisions for each element.
			BoxCollisions[i_me][i_you].ElementCollide[superme] = 1;
			BoxCollisions[i_you][i_me].ElementCollide[superyou] = 1;

			// Store hit info only if new for either element this frame.
			if (!BoxCollisions[i_me][i_you].LastElementCollide[superme] ||
				!BoxCollisions[i_you][i_me].LastElementCollide[superyou])
			{
				if (vsq_store > Max(Collide_min_hit, pcoll->EnergyMax))
				{
					// Largest normal collision between these boxes this frame.
					// Store normal velocity for this collision.
					pcoll->Element1 = superme;
					pcoll->Element2 = superyou;

					pcoll->EnergyMax = vsq_store;
				}
			}
		}

		if (!pcoll->EnergyHit)
		{
			// No new collision this frame so far.
			pcoll->Element1 = superme;
			pcoll->Element2 = superyou;
		}

		// Sum energies between all elements.
		pcoll->EnergyHit   += vsq_store;
		pcoll->EnergySlide += vsqt_store;

		bHitAnother = you.bHitAnother = true;
}


//	 Here compute the relative velocity at a point in me_local...
//	 ------------------------------------------------------------
void CXob::get_BC_moments( CXob& you,
					 float your[3],						//Your point of contact,
					 float mine[3],						//My point of contact,
					 float BC_n[3],						//The normal of the BC vector,
					 float BC_val,						//The macgitude of the contact,
					 float day, float dby, float dgy,	//Your angle-converted first moments,
					 float ad,  float bd,  float gd,	//My angle-converted first moments...
					 int superme, int superyou )
{

float	mvel[3];	//For temp...
float	vel[3];


		float test = BC_n[0]*mine[0] + BC_n[1]*mine[1] + BC_n[2]*mine[2];
		if ( test < 0 )
		{

			BC_n[0] *= -1;
			BC_n[1] *= -1;
			BC_n[2] *= -1;
			BC_val = -IBC_coarseness;//0;//IBC_coarseness;
		
			float Butt[3] = {BC_n[0],BC_n[1],BC_n[2]};
			Back( Butt[0], Butt[1], Butt[2] );

			float dp = Butt[0]*State[0][1] + 
					   Butt[1]*State[1][1] +
					   Butt[2]*State[2][1];

			if (dp > 0)
			{
				State[0][1] -= dp*Butt[0];
				State[1][1] -= dp*Butt[1];
				State[2][1] -= dp*Butt[2];
			}

			dp = Butt[0]*you.State[0][1] + 
				 Butt[1]*you.State[1][1] +
				 Butt[2]*you.State[2][1];

			if (dp < 0)
			{
				you.State[0][1] -= dp*Butt[0];
				you.State[1][1] -= dp*Butt[1];
				you.State[2][1] -= dp*Butt[2];
			}

			//if (Anchored[me])  State[me][3][1]  = State[me][4][1]  = State[me][5][1]  = State[me][6][1]  = 0;
			//if (Anchored[you]) State[you][3][1] = State[you][4][1] = State[you][5][1] = State[you][6][1] = 0;
			//if (!Anchored)  State[3][1]  = State[4][1]  = State[5][1]  = State[6][1]  = 0;
			//if (!you.Anchored) you.State[3][1] = you.State[4][1] = you.State[5][1] = you.State[6][1] = 0;
			//KillRVel( BC_n, mine );

			conPhysics << Index() << " : " << you.Index() << " -- Penetration!\n";
		}

//		Indicates a line normal...
		iJumpCode = 6;
		JTnorm[0] = BC_n[0];
		JTnorm[1] = BC_n[1];
		JTnorm[2] = BC_n[2];


		Back_Element( your[0], your[1], your[2], superme );	//SuperWasteOfTimeForImpreciseJoe...
		your[0] += SuperData[superme][3];
		your[1] += SuperData[superme][4];
		your[2] += SuperData[superme][5];

		Back_Element( mine[0], mine[1], mine[2], superme );
		mine[0] += SuperData[superme][3];
		mine[1] += SuperData[superme][4];
		mine[2] += SuperData[superme][5];

		Back_Element( BC_n[0], BC_n[1], BC_n[2], superme );
		Back( your[0], your[1], your[2] );		//Global...

		your[0] += State[0][0] - you.State[0][0];	
		your[1] += State[1][0] - you.State[1][0];
		your[2] += State[2][0] - you.State[2][0];

		you.From( your[0], your[1], your[2] );		//Local to YOU...

//		if (0)//test < 0)
//		{
//			float n_y[3] = { BC_n[0], BC_n[1], BC_n[2] };
//			Back_Element( n_y[0], n_y[1], n_y[2], superme );
//			Back( n_y[0], n_y[1], n_y[2] );
//			you.From( n_y[0], n_y[1], n_y[2] );
//
//			you.KillRVel( n_y, your );
//		}


		vel[2] = (day*your[1] - dby*your[0]);				//Local moments...
		vel[1] = (dgy*your[0] - day*your[2]);
		vel[0] = (dby*your[2] - dgy*your[1]);

		you.Back( vel[0], vel[1], vel[2] );			//Global for you...

		vel[0] += you.State[0][1] - State[0][1];		//Should this me a minus?
		vel[1] += you.State[1][1] - State[1][1];		//This can be done outside the loop...
		vel[2] += you.State[2][1] - State[2][1];

		From( vel[0], vel[1], vel[2] );			//Local to me now...

		mvel[2] = (ad*mine[1] - bd*mine[0]);				//My rotation at your contact...
		mvel[1] = (gd*mine[0] - ad*mine[2]);
		mvel[0] = (bd*mine[2] - gd*mine[1]);

		vel[0] -= mvel[0];									//Okay, there it is in local me space...
		vel[1] -= mvel[1];
		vel[2] -= mvel[2];



float	v_dotp = BC_n[0]*vel[0] + BC_n[1]*vel[1] + BC_n[2]*vel[2];

float	Vp[3]  = { v_dotp*BC_n[0], v_dotp*BC_n[1], v_dotp*BC_n[2] };
float	Vt[3] =  { vel[0] - Vp[0], vel[1] - Vp[1], vel[2] - Vp[2] };

float	lin_comp = LIBC_kappa*OurKappa*(IBC_coarseness - BC_val);
float	lin_damp = LIBC_damp*OurDelta;
float	lin_slid = L_slide*OurMu;

		if (v_dotp > 0) lin_damp *= BC_bounce;

		vsq_store  = Vp[0]*Vp[0] + Vp[1]*Vp[1] + Vp[2]*Vp[2];	//For the collision message above...
		vsqt_store = Vt[0]*Vt[0] + Vt[1]*Vt[1] + Vt[2]*Vt[2];	//For the collision message above...

		//if (vsqt_store + vsq_store < .001) lin_slid*=2;

		vsq_store  *= current_timeslice*lin_damp;
		vsqt_store *= current_timeslice*lin_slid;

float	Fuerzo[3]  = { lin_comp*BC_n[0] - lin_damp*Vp[0] - lin_slid*Vt[0],
					   lin_comp*BC_n[1] - lin_damp*Vp[1] - lin_slid*Vt[1],
					   lin_comp*BC_n[2] - lin_damp*Vp[2] - lin_slid*Vt[2] };

		Stuff_Contacts( you, Fuerzo[0], Fuerzo[1], Fuerzo[2],
						mine[0], mine[1], mine[2], your[0], your[1], your[2],
						superme, superyou );

//		For foot reaction force stuff...
		if (bStoreFric)
		{
			fFricStore[0] =-lin_slid*Vt[0];
			fFricStore[1] =-lin_slid*Vt[1];
			fFricStore[2] =-lin_slid*Vt[2];
		}

}


//	Stupid, stupid, stupid...
//	-------------------------
inline float fsign( float arg )
{
	return arg < 0.0f ? -1.0f : 1.0f;
}


//	Find the equation of motion and stuff from B/C...
//	-------------------------------------------------
void CXob::pipe_quad_BC() 
{


//		First off, set up the transform...
		Make_Transform();


//		Wz tests...
//		-----------
		Wz = 0;
		iJumpCode = -1;


//	Convert to something reasonable...
//	==================================
float	a0 = State[3][0],
		a1 = State[4][0],
		a2 = State[5][0],
		a3 = State[6][0];

float	d0 = State[3][1],
		d1 = State[4][1],
		d2 = State[5][1],
		d3 = State[6][1];

float	ad = 2*( a0*d1 + a3*d2 - a2*d3 - a1*d0 ),
		bd = 2*(-a3*d1 + a0*d2 + a1*d3 - a2*d0 ),
		gd = 2*( a2*d1 - a1*d2 + a0*d3 - a3*d0 );

float	alpha_total = 0,
		beta_total  = 0,
		gamma_total = 0;

float	Phi_X = 0,
		Phi_Y = 0,
		Phi_Z = 0;

//	Clean up in case of unrenormalizable values...
//	----------------------------------------------
#define MAX_RENORM 10
		if ( ad > MAX_RENORM ) { ad = MAX_RENORM; scrub_state( ad, bd, gd, a0, a1, a2, a3 ); }
		if ( bd > MAX_RENORM ) { bd = MAX_RENORM; scrub_state( ad, bd, gd, a0, a1, a2, a3 ); }
		if ( gd > MAX_RENORM ) { gd = MAX_RENORM; scrub_state( ad, bd, gd, a0, a1, a2, a3 ); }
		if ( ad <-MAX_RENORM ) { ad =-MAX_RENORM; scrub_state( ad, bd, gd, a0, a1, a2, a3 ); }
		if ( bd <-MAX_RENORM ) { bd =-MAX_RENORM; scrub_state( ad, bd, gd, a0, a1, a2, a3 ); }
		if ( gd <-MAX_RENORM ) { gd =-MAX_RENORM; scrub_state( ad, bd, gd, a0, a1, a2, a3 ); }


#define MAX_LINEAR 13*13
		float velcheck = ( State[0][1]*State[0][1] + State[1][1]*State[1][1] + State[2][1]*State[2][1] );
		if (velcheck > MAX_LINEAR)
		{
			float mul = sqrt(MAX_LINEAR / velcheck);
			State[0][1] *= mul;
			State[1][1] *= mul;
			State[2][1] *= mul;
			conPhysics << "MAXVEL = " << velcheck << "\n";
		}


//		End of influence from other objects...
//		======================================

//		Zeeeeeeero...
//		=============
		State[0][2] = State[1][2] = State[2][2] = 0;

//float	(*edges)[2][3];
int		num_edges = 0;

//		Now, in support of SUPERBOXES...
//		================================
		for (int super = 0; super < Data[11]; super++)
		{


float		X = SuperData[super][0],		//Half extents of element...
			Y = SuperData[super][1],
			Z = SuperData[super][2];

float		offsetX = SuperData[super][3],	//Offset from origin(CM) of element...
			offsetY = SuperData[super][4],
			offsetZ = SuperData[super][5];


float		work_mat[8][3] = { {-X, -Y, -Z},
							   { X, -Y, -Z},
							   { X,  Y, -Z},
							   {-X,  Y, -Z},
	
							   {-X, -Y,  Z},
							   { X, -Y,  Z},
							   { X,  Y,  Z},
							   {-X,  Y,  Z} };

//			An index to points in the geometry matricies...
const int	Lines[12][2] = { {0, 1}, {3, 2}, {4, 5}, {7, 6},	//X_local,
							 {0, 3}, {1, 2}, {4, 7}, {5, 6},	//Y_local,
							 {0, 4}, {1, 5}, {2, 6}, {3, 7} };	//Z_local...

//			Really, we don't need to calculate terrain interactions if we're immobile...
//			============================================================================
			if (Movable) 
			{
float			trr_height = fTerrainHeight( State[0][0], State[1][0] );
bool			do_terrain = trr_height > State[2][0] - Data[10];

				num_edges = 0;
float			trr_fric = 5;

				if (do_terrain)
				{
//					Get terrain friction for all subsequent interactions...
//					Disabled now until needed, and it can be made fast...
//					=====================================================
//					trr_fric = NPhysImport::fTerrainFriction( State[0][0], State[1][0] );
				}
				else
				{
//					Do terrain if we're near water as well...
//					=========================================
					do_terrain = fWaterHeight( State[0][0], State[1][0] ) > State[2][0] - Data[10];
				}

				if (do_terrain)
				{
//					Retrieve terrain edges...
//					=========================
					//TVectorPair* edges[100];
					//int num_edges = NPhysImport::iBoxTerrainEdges(me, edges, 100);

//					Okey dokey...
//					=============
					for (int count = 0; count < 8; count++)
					{

float					x_v = work_mat[count][0],
						y_v = work_mat[count][1],
						z_v = work_mat[count][2];
		
						Back_Element( x_v, y_v, z_v, super );	//To the element object itself...
						x_v += offsetX;	y_v += offsetY;	z_v += offsetZ;

float					A = x_v, 
						B = y_v,
						C = z_v;

						//conPhysics << "XYZ: " << A << ", " << B << ", " << C << "\n";

						Back( A, B, C );
				
						Assert(_finite(A) && _finite(B) && _finite(C));

						A += State[0][0];
						B += State[1][0];
						C += State[2][0];

						Assert(_finite(A) && _finite(B) && _finite(C));

//						Here is the call to get terrain info...
//						=======================================
float					trr_normal[3];
						float scratch = C - BC_coarseness - fTerrainHeight(A, B, C - BC_coarseness, trr_normal );
						CEntityWater* petw;
						float water_height = fWaterHeight(A, B, &petw);

#define BOY_OH_BOY .05

//						Bouyancy and water disturbance...
//						=================================
float					temporary = C - water_height;
						if ( temporary <= 0 && petw)
						{

							if ( temporary < -Data[10] ) temporary =-Data[10];

							// Inputs back into the box state go here.
float						xd = bd*z_v - gd*y_v,
							yd = gd*x_v - ad*z_v,
							zd = ad*y_v - bd*x_v;
				
							Back( xd, yd, zd );	//Now Glob...

							xd += State[0][1],
							yd += State[1][1],
							zd += State[2][1];

							// Does this object float?
							if (Floats)
							{
								// Yes!  It floats.
float							Fz = -BOY_OH_BOY*(Data[1]*temporary + /*1.5**/5*Data[2]*zd),
//float							Fz = -BOY_OH_BOY*(.5*Data[1]*temporary + /*1.5**/.5*Data[2]*zd),
								Fx = -BOY_OH_BOY*Data[3]*xd,
								Fy = -BOY_OH_BOY*Data[3]*yd;
							
								State[0][2] += Fx;	//Globally...
								State[1][2] += Fy;
								State[2][2] += Fz;

								From( Fx, Fy, Fz );
	
								alpha_total += y_v*Fz - z_v*Fy;
								beta_total  += z_v*Fx - x_v*Fz;
								gamma_total += x_v*Fy - y_v*Fx;

								//float vel = fabs(xd) + fabs(yd);
								//if (zd < 0) vel =-vel;
								//zd = vel;
							}

							// Create wave in water.
							//NPhysImport::CreateDisturbance(petw, A, B, /*1.5*.09*/1.5*BC_coarseness, -.01*zd, true);
							NPhysImport::CreateDisturbance(petw, A, B, /*1.5*.09*/1*BC_coarseness, -.015*zd, true);

							//
							// Update collision information. Use vertical velocity only; set sliding to 0.
							//
							int i_me = Index();
SCollision*					pcoll = &BoxCollisions[i_me][WATER];

							if (State[2][1] < 0)
							{
								// Track collisions for this.
								pcoll->ElementCollide[super] = 1;

float							energy = current_timeslice*Data[2]*zd*zd;

								// Store hit info only if new for this element this frame.
								if (!pcoll->LastElementCollide[super])
								{
									if (energy > Max(Collide_min_hit, pcoll->EnergyMax))
									{
										// Largest collision between box and water this frame.
										pcoll->Element1 = super;
										pcoll->Element2 = 0;

										pcoll->EnergyMax = energy;
										WaterCollide[i_me] = (CInstance*)petw;

										StuffVec( State[0][0], State[1][0], C, pcoll->Location );
									}
								}

								// Sum energies for elements which collided.
								if (!pcoll->EnergyMax)
									StuffVec( State[0][0], State[1][0], C, pcoll->Location );
								if (!pcoll->EnergyHit)
								{
									pcoll->Element1 = super;
									pcoll->Element2 = 0;
								}

								pcoll->EnergyHit += energy;
								WaterCollide[i_me] = (CInstance*)petw;
							}
						}

 

//						Do we need to do any work...
//						============================
						if ( scratch <= 0 )
						{

							if ( scratch < -2*BC_coarseness) 
							{
								scratch = -2*BC_coarseness;
							}
							//if ( scratch < -BC_coarseness) if (State[2][1] < 0) State[2][1] = 0;


float						zd = ad*y_v - bd*x_v,
							yd = gd*x_v - ad*z_v,
							xd = bd*z_v - gd*y_v;
			
							Back( xd, yd, zd );	//Now Glob...

							xd += State[0][1],
							yd += State[1][1],
							zd += State[2][1];

							if ((trr_normal[1]*trr_normal[1] + trr_normal[0]*trr_normal[0])<.3) {trr_normal[0] = trr_normal[1] = 0; trr_normal[2] = 1;}

							const float max_comp = .4f;//.6f;
							float t1, t2;
							if (trr_normal[0] > max_comp) { trr_normal[0] = max_comp; Normalize( trr_normal, t1, t2 );}
							if (trr_normal[1] > max_comp) { trr_normal[1] = max_comp; Normalize( trr_normal, t1, t2 );}
							if (trr_normal[0] <-max_comp) { trr_normal[0] =-max_comp; Normalize( trr_normal, t1, t2 );}
							if (trr_normal[1] <-max_comp) { trr_normal[1] =-max_comp; Normalize( trr_normal, t1, t2 );}

float						dot = xd*trr_normal[0] + yd*trr_normal[1] + zd*trr_normal[2];

							//if (dot > 0) dot = 0;

float						nf = (-Data[1]*scratch - Data[2]*dot);

float						store[3] = {xd, yd, zd};

							xd -= dot*trr_normal[0];
							yd -= dot*trr_normal[1];
							zd -= dot*trr_normal[2];


float						Fx = nf*trr_normal[0] - Data[2]*xd,
							Fy = nf*trr_normal[1] - Data[2]*yd,
							Fz = /*2*/nf*trr_normal[2] - Data[2]*zd;


							Wz -= trr_normal[2]*nf;

float						nrgt = current_timeslice*Data[2]*(xd*xd + yd*yd + zd*zd),
							nrgp = current_timeslice*Data[2]*dot*dot;//dot*nf;
							

							State[0][2] += Fx;	//Globally...
							State[1][2] += Fy;
							State[2][2] += Fz;

							//Oshit...
							if (Data[11] > 1)
							{
								comp_index += 1;
								//conPhysics << "++comp_index[" << iIndex() << "] = " << comp_index << "\n";

								// Inserted this to detect cause of crash.
								AlwaysAssert(comp_index < COMPOUND_OVERSPEC);

								SubInp[comp_index][0][0] = Fx;
								SubInp[comp_index][0][1] = Fy;
								SubInp[comp_index][0][2] = Fz;
							}

float						energy       = nrgp;
float						energy_trans = nrgt;

							From( Fx, Fy, Fz );
	
float						alpha_tmp = y_v*Fz - z_v*Fy,
							beta_tmp  = z_v*Fx - x_v*Fz,
							gamma_tmp = x_v*Fy - y_v*Fx;

							if (Data[11] > 1)
							{
								SubInp[comp_index][1][0] = alpha_tmp;
								SubInp[comp_index][1][1] = beta_tmp;
								SubInp[comp_index][1][2] = gamma_tmp;

								position[comp_index][0] = x_v;
								position[comp_index][1] = y_v;
								position[comp_index][2] = z_v;

								//conPhysics << "counting - " << comp_index << "\n";
							}

							//if (Data[11] == 2) conPhysics << "terrain: " << x_v << ", " << y_v << ", " << z_v << "\n";

							alpha_total += alpha_tmp;
							beta_total  += beta_tmp;
							gamma_total += gamma_tmp;

//							For breakage...
							TTotal[super][0] += alpha_tmp;
							TTotal[super][1] += beta_tmp;
							TTotal[super][2] += gamma_tmp;

							Assert(Movable);

//							Track collisions for each element and corner...
SCollision*					pcoll = &BoxCollisions[Index()][TERRAIN];
SCollision*					pcoll_corner = &BoxCollisions[Index()][TERRAIN+count];

							if (energy > 0)
							{
								pcoll_corner->ElementCollide[super] = 1;

//								Store collision info only if new for this corner this frame...
								if (!pcoll_corner->LastElementCollide[super])
								{
									if (energy > Max(Collide_min_hit, pcoll_corner->EnergyMax))
									{
										// Largest collision between this box and terrain this frame.
										pcoll_corner->Element1 = super;
										pcoll_corner->Element2 = 0;
										pcoll_corner->EnergyMax = energy;
									}
								}
							}

							//
							// Sum energies.  See notes under StuffContacts.
							//
							if (!pcoll->EnergyHit)
							{
								pcoll->Element1 = super;
								pcoll->Element2 = 0;
							}

							pcoll->EnergyHit   += energy;
							pcoll->EnergySlide += energy_trans;

							// Sum collision location vector, scaled by energy.
							// It will later be divided by total energy to yield average location.
							pcoll->Location[0] += A*energy;
							pcoll->Location[1] += B*energy;
							pcoll->Location[2] += C*energy;
						}//End of successful contact test...

					}//Iterate over extents...




//	+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_















//#define TERRAIN_EDGES
#ifdef TERRAIN_EDGES


#define MIN_EDGE_LEN 1.5
#define MIN_EDGE_CHECK .5//MIN_EDGE_LEN/2


	//conPhysics << "Num_Edges " << num_edges << "\n";

	Data[4] = 10;
	//conPhysics << "DM11: " << Data[11] << ", " << Data[4] << ", " << Data[5] << ", "  << Data[6] << "\n";


//	OK, check explicitly for edges from terrain...
//	==============================================
	if (	( Data[11] < 2)						//Non-compound...
		&&	( num_edges    > 0)						//There are in fact edges...
		&&	(										
				(Data[4] > MIN_EDGE_CHECK )		//It merits checking...
			||	(Data[5] > MIN_EDGE_CHECK )
			||	(Data[6] > MIN_EDGE_CHECK )
			)
		)
	{


float	A_me[3],
		B_me[3],
		A_you[3],
		B_you[3];

float	Mx = 0, Bx = 0,
		My = 0, By = 0,
		Mz = 0, Bz = 0;

float	rho = 0,
		delta = 0,
		tmin = 0,
		tmax = 0;

float	BC_vec[3],
		BC_n[3],
		BC_val = 0;


//		Now we go for X_me...
//		---------------------
		if ( Data[4] > MIN_EDGE_CHECK )
			for (int count = 0; count < 4; count ++)
			{
			A_me[0] = work_mat[(Lines[count][0])][0];// + IBC_coarseness;
			A_me[1] = work_mat[(Lines[count][0])][1];
			A_me[2] = work_mat[(Lines[count][0])][2];

			B_me[0] = work_mat[(Lines[count][1])][0];// - IBC_coarseness;
			//B_me[1] = work_mat[(Lines[count][1])][1];
			//B_me[2] = work_mat[(Lines[count][1])][2];

//			Lupe over YOUR lines...
//			-----------------------
			for (int yline = 0; yline < num_edges; yline++ )
			{
				A_you[0] = edges[yline][0][0];
				A_you[1] = edges[yline][0][1];
				A_you[2] = edges[yline][0][2];

				B_you[0] = edges[yline][1][0];
				B_you[1] = edges[yline][1][1];
				B_you[2] = edges[yline][1][2];


				A_you[0] -= State[0][0];	
				A_you[1] -= State[1][0];	
				A_you[2] -= State[2][0];	
				From( A_you[0], A_you[1], A_you[2] );

				B_you[0] -= State[0][0];	
				B_you[1] -= State[1][0];	
				B_you[2] -= State[2][0];	
				From( B_you[0], B_you[1], B_you[2] );

				//conPhysics << " Dist: " << sqrt( (A_you[0] - B_you[0])*(A_you[0] - B_you[0])
				//				  + (A_you[1] - B_you[1])*(A_you[1] - B_you[1])
				//				  + (A_you[2] - B_you[2])*(A_you[2] - B_you[2])
				//				  ) << "\n";

				//conPhysics << "Start...\n";

#define ATEST_F
#define BTEST_F
#define CTEST_F

#ifdef ATEST_F

				float epsilon_plus  = A_me[2] + IBC_coarseness;
				float epsilon_minus = A_me[2] - IBC_coarseness;
				if (   !((A_you[2]>epsilon_plus) && (B_you[2]>epsilon_plus))
					&& !((A_you[2]<epsilon_minus) && (B_you[2]<epsilon_minus))
				   )
#endif
				{

				//conPhysics << "Test 1...\n";

#ifdef ATEST_F
				epsilon_plus  = A_me[1] + IBC_coarseness;
				epsilon_minus = A_me[1] - IBC_coarseness;
				if (   !((A_you[1]>epsilon_plus) && (B_you[1]>epsilon_plus))
					&& !((A_you[1]<epsilon_minus) && (B_you[1]<epsilon_minus))
				   )

				if ( 1 )// ((A_you[0]>A_me[0]) && (A_you[0]<B_me[0])) || ((B_you[0]>A_me[0]) && (B_you[0]<B_me[0])) )
#endif
				{


				//conPhysics << "Test 2...\n";


				delta = A_you[0] - B_you[0];
				if ( fabs(delta) > ORTHO_TOLERANCE )				//Are you secretly ortho?
				{
					My =(A_you[1] - B_you[1])/delta;
					Mz =(A_you[2] - B_you[2])/delta;
					By = A_you[1] - A_me[1] - My*A_you[0];
					Bz = A_you[2] - A_me[2] - Mz*A_you[0];
					if ( Mz*Mz + My*My > PARA_TOLERANCE )			//How about parallel?
					{						
						rho =-(By*My + Bz*Mz) / (My*My + Mz*Mz);	//There's the solution...
						if ( (rho > A_me[0]) && (rho < B_me[0]) )
						{
							cout << "Passed 1...";
							minmax( A_you[0], B_you[0], tmin, tmax );
							if ( (rho > tmin) && (rho < tmax) )
							{
								cout << "Passed 2...";
								BC_vec[0] = 0;						//There.  I wrote it.
								BC_vec[1] = My*rho + By;
								BC_vec[2] = Mz*rho + Bz;
		
								BC_val = BC_vec[1]*BC_vec[1] + BC_vec[2]*BC_vec[2];
								if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
								{
									//conPhysics << "Rho: " << rho << "         RM - " << BC_vec[0] << " : " << BC_vec[1] << " : " << BC_vec[2] << "\n";
									//conPhysics << "T: " << yline << ", rho: " << rho << "\n";
float								mine[3] = { rho, A_me[1], A_me[2] };	//Find locations...
float								your[3] = { mine[0], mine[1], mine[2] };

									if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
									BC_val = sqrt( BC_val );				//Only if you HAVE to...
									BC_n[0] = 0;
									BC_n[1] = BC_vec[1] / BC_val;
									BC_n[2] = BC_vec[2] / BC_val;

									
									//conPhysics << "Point 1 with " << i_me << "\n";
									Stuff_Terr_Moments( me,						//Okay, do it...
														mine,
														BC_n,
														BC_val,
														ad, bd, gd
														);

								}//	Actual kontakt(tm) test...
							
							}// Final range test...

						}// First range test...

					}//	Para-tolerance...
					//else { cout << "Para!\n"; }	//Probably do nothing...
					//else conPhysics << "Para!\n";

				}//	Ortho-tolerance, are we orthogonal???
				else if ( (A_you[0] > A_me[0]) && (A_you[0] < B_me[0]) )	//Ok, now we're restricted to 2 DOF...
				{ 
					delta = A_you[1] - B_you[1];
					if ( fabs(delta) > ORTHO_TOLERANCE )					//We have to test here too...
					{
						Mz =(A_you[2] - B_you[2])/delta;
						Bz = A_you[2] - A_me[2] - Mz*A_you[1];

						rho =-( Bz*Mz )/(1 + Mz*Mz);						//Note that we'll not need a test here...
						minmax( A_you[1], B_you[1], tmin, tmax );
						if ( (rho > tmin) && (rho < tmax) )
						{
							BC_vec[0] = 0;
							BC_vec[1] = rho;
							BC_vec[2] = Mz*rho + Bz;

							BC_val = BC_vec[1]*BC_vec[1] + BC_vec[2]*BC_vec[2];
							if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
							{
								//cout << "O";	flush(cout);
float							mine[3] = { A_you[0], A_me[1], A_me[2] };	//Find locations...
float							your[3] = { mine[0], mine[1], mine[2] };

								if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
								BC_val = sqrt( BC_val );				//Only if you HAVE to...
								BC_n[0] = 0;
								BC_n[1] = BC_vec[1] / BC_val;
								BC_n[2] = BC_vec[2] / BC_val;

								//conPhysics << "Point 2\n";
								Stuff_Terr_Moments( me,						//Okay, do it...
													mine,
													BC_n,
													BC_val,
													ad, bd, gd
													);

							}//	Actual ortho Kontakt(tm)...

						}//	Test on ortho range...

					}//	Ortho case...
					else if ( fabs( delta = (A_you[1] - A_me[1])) <= IBC_coarseness )	//Failed Ortho - straight X-Z...
					{
						//cout << "del: " << delta << "\n";	flush( cout );

						if ( fabs(delta) < DIV_TOL ) delta = fsign(delta)*DIV_TOL;	//Only you etc...
						BC_vec[0] = BC_vec[2] = 0;
						BC_vec[1] = delta;
						
						BC_n[0] = BC_n[2] = 0;
						BC_n[1] = fsign(delta);//delta/fabs(delta);

						BC_val = fabs(delta);

float					mine[3] = { A_you[0], A_me[1], A_me[2] },
						your[3] = { mine[0], mine[1], mine[2] };

						//conPhysics << "Point 3\n";
						Stuff_Terr_Moments( me,						//Okay, do it...
											mine,
											BC_n,
											BC_val,
											ad, bd, gd
											);

					}//	Kontakt in the orthonormal case...

				}//	Are you ortho but in range?

			} }//Efficiency...

			}//	End of your line...

		}//	End of X_me line...


//#define OTHERS
#ifdef OTHERS
//		Now we go for Y_me...
//		---------------------
		for (count = 4; count < 8; count ++)
		{
			A_me[0] = work_mat[(Lines[count][0])][0];
			A_me[1] = work_mat[(Lines[count][0])][1];// + IBC_coarseness;
			A_me[2] = work_mat[(Lines[count][0])][2];

			//B_me[0] = work_mat[(Lines[count][1])][0];
			B_me[1] = work_mat[(Lines[count][1])][1];// - IBC_coarseness;
			//B_me[2] = work_mat[(Lines[count][1])][2];

//			Lupe over YOUR lines...
//			-----------------------
			for (int yline = 0; yline < 12; yline++ )
			{
				A_you[0] = you_mat[(Lines[yline][0])][0];
				A_you[1] = you_mat[(Lines[yline][0])][1];
				A_you[2] = you_mat[(Lines[yline][0])][2];

				B_you[0] = you_mat[(Lines[yline][1])][0];
				B_you[1] = you_mat[(Lines[yline][1])][1];
				B_you[2] = you_mat[(Lines[yline][1])][2];



#ifdef BTEST_F
				float epsilon_plus  = A_me[2] + IBC_coarseness;
				float epsilon_minus = A_me[2] - IBC_coarseness;
				if (   !((A_you[2]>epsilon_plus) && (B_you[2]>epsilon_plus))
					&& !((A_you[2]<epsilon_minus) && (B_you[2]<epsilon_minus))
				   )
#endif
				{

#ifdef BTEST_F
				epsilon_plus  = A_me[0] + IBC_coarseness;
				epsilon_minus = A_me[0] - IBC_coarseness;
				if (   !((A_you[0]>epsilon_plus) && (B_you[0]>epsilon_plus))
					&& !((A_you[0]<epsilon_minus) && (B_you[0]<epsilon_minus))
				   )

				if ( 1 )// ((A_you[1]>A_me[1]) && (A_you[1]<B_me[1])) || ((B_you[1]>A_me[1]) && (B_you[1]<B_me[1])) )
#endif
				{

				delta = A_you[1] - B_you[1];
				if ( fabs(delta) > ORTHO_TOLERANCE )				//Are you secretly ortho?
				{
					Mx =(A_you[0] - B_you[0])/delta;
					Mz =(A_you[2] - B_you[2])/delta;
					Bx = A_you[0] - A_me[0] - Mx*A_you[1];
					Bz = A_you[2] - A_me[2] - Mz*A_you[1];
					if ( Mz*Mz + Mx*Mx > PARA_TOLERANCE )			//How about parallel?
					{						
						//if ( (Mx*Mx < .01) || (Mz*Mz < .01) ) cout << "!"; flush( cout );
						rho =-(Bx*Mx + Bz*Mz) / (Mx*Mx + Mz*Mz);	//There's the solution...
						if ( (rho > A_me[1]) && (rho < B_me[1]) )
						{
							//cout << "Passed 1...";
							minmax( A_you[1], B_you[1], tmin, tmax );
							if ( (rho > tmin) && (rho < tmax) )
							{
								//cout << "Passed 2...";
								BC_vec[0] = Mx*rho + Bx;			//There.  I wrote it.
								BC_vec[1] = 0;
								BC_vec[2] = Mz*rho + Bz;
		
								BC_val = BC_vec[0]*BC_vec[0] + BC_vec[2]*BC_vec[2];
								if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
								{
									//cout << "Me"; flush(cout);
float								mine[3] = { A_me[0], rho, A_me[2] };	//Find locations...
float								your[3] = { mine[0], mine[1], mine[2] };

										if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
										BC_val = sqrt( BC_val );				//Only if you HAVE to...
										BC_n[0] = BC_vec[0] / BC_val;
										BC_n[1] = 0;
										BC_n[2] = BC_vec[2] / BC_val;

										get_BC_moments( me,						//Okay, do it...
														you,
														your,
														mine,
														BC_n,
														BC_val,
														day, dby, dgy,
														ad, bd, gd,
														super, supery );
									
								}//	Actual kontakt(tm) test...
							
							}// Final range test...

						}// First range test...

					}//	Para-tolerance...
					//else { cout << "Para!\n"; }	//Probably do nothing...

				}//	Ortho-tolerance, are we orthogonal???
				else if ( (A_you[1] > A_me[1]) && (A_you[1] < B_me[1]) )	//Ok, now we're restricted to 2 DOF...
				{ 
					delta = A_you[0] - B_you[0];
					if ( fabs(delta) > ORTHO_TOLERANCE )					//We have to test here too...
					{
						Mz =(A_you[2] - B_you[2])/delta;
						Bz = A_you[2] - A_me[2] - Mz*A_you[0];

						rho =-( Bz*Mz )/(1 + Mz*Mz);						//Note that we'll not need a test here...
						minmax( A_you[0], B_you[0], tmin, tmax );
						if ( (rho > tmin) && (rho < tmax) )
						{
							BC_vec[0] = rho;
							BC_vec[1] = 0;
							BC_vec[2] = Mz*rho + Bz;

							BC_val = BC_vec[0]*BC_vec[0] + BC_vec[2]*BC_vec[2];
							if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
							{
								//cout << "O";	flush(cout);
float							mine[3] = { A_me[0], A_you[1], A_me[2] };	//Find locations...
float							your[3] = { mine[0], mine[1], mine[2] };

									if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
									BC_val = sqrt( BC_val );				//Only if you HAVE to...
									BC_n[0] = BC_vec[0] / BC_val;
									BC_n[1] = 0;
									BC_n[2] = BC_vec[2] / BC_val;

									get_BC_moments( me,						//Okay, do it...
													you,
													your,
													mine,
													BC_n,
													BC_val,
													day, dby, dgy,
													ad, bd, gd,
													super, supery );
								
							}//	Actual ortho Kontakt(tm)...

						}//	Test on ortho range...

					}//	Ortho case...
					else if ( fabs( delta = (A_you[0] - A_me[0])) <= IBC_coarseness )	//Failed Ortho - straight Y-Z...
					{
						//cout << "del: " << delta << "\n";	flush( cout );

						if ( fabs(delta) < DIV_TOL ) delta = fsign(delta)*DIV_TOL;	//Only you etc...
						BC_vec[1] = BC_vec[2] = 0;
						BC_vec[0] = delta;
						
						BC_n[1] = BC_n[2] = 0;
						BC_n[0] = fsign(delta);//delta/fabs(delta);

						BC_val = fabs(delta);

float					mine[3] = { A_me[0], A_you[1], A_me[2] },
						your[3] = { mine[0], mine[1], mine[2] };

						get_BC_moments( me,						//Okay, do it...
						 				you,
										your,
										mine,
										BC_n,
										BC_val,
										day, dby, dgy,
										ad, bd, gd,
										super, supery );

					}//	Kontakt in the orthonormal case...

				}//	Are you ortho but in range?

			} }//Efficiency...

			}//	End of your line...

		}//	End of Y_me line...


//		Now, before E3, go for Z_me...
//		------------------------------
		for (count = 8; count < 12; count ++)
		{
			A_me[0] = work_mat[(Lines[count][0])][0];
			A_me[1] = work_mat[(Lines[count][0])][1];
			A_me[2] = work_mat[(Lines[count][0])][2];// + IBC_coarseness;

			//B_me[0] = work_mat[(Lines[count][1])][0] - IBC_coarseness;
			//B_me[1] = work_mat[(Lines[count][1])][1];
			B_me[2] = work_mat[(Lines[count][1])][2];// - IBC_coarseness;

//			Lupe over YOUR lines...
//			-----------------------
			for (int yline = 0; yline < 12; yline++ )
			{
				A_you[0] = you_mat[(Lines[yline][0])][0];
				A_you[1] = you_mat[(Lines[yline][0])][1];
				A_you[2] = you_mat[(Lines[yline][0])][2];

				B_you[0] = you_mat[(Lines[yline][1])][0];
				B_you[1] = you_mat[(Lines[yline][1])][1];
				B_you[2] = you_mat[(Lines[yline][1])][2];


#ifdef CTEST_F
				float epsilon_plus  = A_me[0] + IBC_coarseness;
				float epsilon_minus = A_me[0] - IBC_coarseness;
				if (   !((A_you[0]>epsilon_plus) && (B_you[0]>epsilon_plus))
					&& !((A_you[0]<epsilon_minus) && (B_you[0]<epsilon_minus))
				   )
#endif
				{

#ifdef CTEST_F
				epsilon_plus  = A_me[1] + IBC_coarseness;
				epsilon_minus = A_me[1] - IBC_coarseness;
				if (   !((A_you[1]>epsilon_plus) && (B_you[1]>epsilon_plus))
					&& !((A_you[1]<epsilon_minus) && (B_you[1]<epsilon_minus))
				   )
				if ( 1 )//((A_you[2]>A_me[2]) && (A_you[2]<B_me[2])) || ((B_you[2]>A_me[2]) && (B_you[2]<B_me[2])) )
#endif
				{

				delta = A_you[2] - B_you[2];
				if ( fabs(delta) > ORTHO_TOLERANCE )				//Are you secretly ortho?
				{
					Mx =(A_you[0] - B_you[0])/delta;
					My =(A_you[1] - B_you[1])/delta;
					Bx = A_you[0] - A_me[0] - Mx*A_you[2];
					By = A_you[1] - A_me[1] - My*A_you[2];
					if ( Mx*Mx + My*My > PARA_TOLERANCE )			//How about parallel?
					{						
						rho =-(Bx*Mx + By*My) / (Mx*Mx + My*My);	//There's the solution...
						//cout << "T: " << yline << ", rho: " << rho << "\n";
						if ( (rho > A_me[2]) && (rho < B_me[2]) )
						{
							//cout << "Passed 1...";
							minmax( A_you[2], B_you[2], tmin, tmax );
							if ( (rho > tmin) && (rho < tmax) )
							{
								//cout << "Passed 2...";
								BC_vec[0] = Mx*rho + Bx;			//There.  I wrote it.
								BC_vec[1] = My*rho + By;
								BC_vec[2] = 0;
		
								BC_val = BC_vec[0]*BC_vec[0] + BC_vec[1]*BC_vec[1];
								if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
								{
float								mine[3] = { A_me[0], A_me[1], rho };		//Find locations...
float								your[3] = { mine[0], mine[1], mine[2] };

									if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
									BC_val = sqrt( BC_val );				//Only if you HAVE to...
									BC_n[0] = BC_vec[0] / BC_val;
									BC_n[1] = BC_vec[1] / BC_val;
									BC_n[2] = 0;

									get_BC_moments( me,						//Okay, do it...
													you,
													your,
													mine,
													BC_n,
													BC_val,
													day, dby, dgy,
													ad, bd, gd,
													super, supery );

								}//	Actual kontakt(tm) test...
							
							}// Final range test...

						}// First range test...

					}//	Para-tolerance...
					//else { cout << "Para!\n"; }	//Probably do nothing...

				}//	Ortho-tolerance, are we orthogonal???
				else if ( (A_you[2] > A_me[2]) && (A_you[2] < B_me[2]) )	//Ok, now we're restricted to 2 DOF...
				{ 
					delta = A_you[0] - B_you[0];
					if ( fabs(delta) > ORTHO_TOLERANCE )					//We have to test here too...
					{
						My =(A_you[1] - B_you[1])/delta;
						By = A_you[1] - A_me[1] - My*A_you[0];

						rho =-( By*My )/(1 + My*My);						//Note that we'll not need a test here...
						minmax( A_you[0], B_you[0], tmin, tmax );
						if ( (rho > tmin) && (rho < tmax) )
						{
							BC_vec[0] = rho;
							BC_vec[1] = My*rho + By;
							BC_vec[2] = 0;

							BC_val = BC_vec[0]*BC_vec[0] + BC_vec[1]*BC_vec[1];
							if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
							{
								//cout << "O";	flush(cout);
float							mine[3] = { A_me[0], A_me[1], A_you[2] };	//Find locations...
float							your[3] = { mine[0], mine[1], mine[2] };

								if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
								BC_val = sqrt( BC_val );				//Only if you HAVE to...
								BC_n[0] = BC_vec[0] / BC_val;
								BC_n[1] = BC_vec[1] / BC_val;
								BC_n[2] = 0;

								get_BC_moments( me,						//Okay, do it...
												you,
												your,
												mine,
												BC_n,
												BC_val,
												day, dby, dgy,
												ad, bd, gd,
												super, supery );

							}//	Actual ortho Kontakt(tm)...

						}//	Test on ortho range...

					}//	Ortho case...
					else if ( fabs( delta = (A_you[0] - A_me[0])) <= IBC_coarseness )	//Failed Ortho - straight X-Z...
					{
						//cout << "del: " << delta << "\n";	flush( cout );

						if ( fabs(delta) < DIV_TOL ) delta = fsign(delta)*DIV_TOL;	//Only you etc...
						BC_vec[0] = BC_vec[1] = 0;
						BC_vec[2] = delta;
						
						BC_n[0] = BC_n[1] = 0;
						BC_n[2] = fsign(delta);//delta/fabs(delta);

						BC_val = fabs(delta);

float					mine[3] = { A_me[0], A_me[1], A_you[2] },
						your[3] = { mine[0], mine[1], mine[2] };

						get_BC_moments( me,						//Okay, do it...
										you,
										your,
										mine,
										BC_n,
										BC_val,
										day, dby, dgy,
										ad, bd, gd,
										super, supery );

					}//	Kontakt in the orthonormal case...

				}//	Are you ortho but in range?

			} }//Efficiency...
 
			}//	End of your line...

		}//	End of Z_me line...


#endif	//For OTHERS...

	}	//Is this a valid candidate...



#endif


//	+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_







				}//Test for height...

			}//Test for Movable...








//		Check the IBC list for other guys...
//		====================================
		int i_me = Index();
		for (int i_you = 0; i_you< GUYS; i_you++) if (Do_It[i_you])
		{
			CXob& you = Xob[i_you];

//			Set up the other guy, and make the mappings...
			you.Make_Transform();

float		Delta_x = you.State[0][0] - State[0][0],
			Delta_y = you.State[1][0] - State[1][0],
			Delta_z = you.State[2][0] - State[2][0];

float		a0y = you.State[3][0],
			a1y = you.State[4][0],
			a2y = you.State[5][0],
			a3y = you.State[6][0];

float		d0y = you.State[3][1],
			d1y = you.State[4][1],
			d2y = you.State[5][1],
			d3y = you.State[6][1];

float		day = 2*( a0y*d1y + a3y*d2y - a2y*d3y - a1y*d0y ),
			dby = 2*(-a3y*d1y + a0y*d2y + a1y*d3y - a2y*d0y ),
			dgy = 2*( a2y*d1y - a1y*d2y + a0y*d3y - a3y*d0y );

//			Here, unfortunately, we have to deal with the player standing on movable objects...
//			-----------------------------------------------------------------------------------
			if ( ((PelvisElem     == RIGHT_FOOT) && (Pel_Usage[PelvisModel]     == epelHUMAN))
			||   ((you.PelvisElem == RIGHT_FOOT) && (Pel_Usage[you.PelvisModel] == epelHUMAN)) )
			{
				bStoreFric = true;
				fFricStore[0] = fFricStore[1] = fFricStore[2] = 0;
			}
			else bStoreFric = false;

			//bStoreFric = false;


//			Here, unfortunately, we have to deal with the player standing on movable objects...
//			-----------------------------------------------------------------------------------
			iImmovCode = 0;
#define		MAX_MOVABLE 95//100

			bool bMeIsHuman = ( ((PelvisElem == RIGHT_FOOT) || (PelvisElem == BODY)) && (Pel_Usage[PelvisModel] == epelHUMAN) );
			bool bYouIsHuman = ( ((you.PelvisElem == RIGHT_FOOT) || (you.PelvisElem == BODY)) && (Pel_Usage[you.PelvisModel] == epelHUMAN) );

			if ( (bMeIsHuman) &&  (you.Data[19] >= MAX_MOVABLE) ) iImmovCode = 1;
			if ( (bYouIsHuman) && (Data[19] >= MAX_MOVABLE) )     iImmovCode = 2;

			if ( (!bYouIsHuman) && (PelvisElem == RIGHT_HAND) && (Pel_Usage[PelvisModel] == epelHUMAN) && (you.Data[19] >= MAX_MOVABLE)) iImmovCode = 1;
			if ( (!bMeIsHuman)  && (you.PelvisElem == RIGHT_HAND) && (Pel_Usage[you.PelvisModel] == epelHUMAN) && (Data[19] >= MAX_MOVABLE)) iImmovCode = 2;

			if (iImmovCode == 1) if ( Pel_Data[PelvisModel][49] <= 0     ) iImmovCode = 0;
			if (iImmovCode == 2) if ( Pel_Data[you.PelvisModel][49] <= 0 ) iImmovCode = 0;


//			Loop over your elements here...
//			-------------------------------
			for( int supery = 0; supery < you.Data[11]; supery++ ) {

float		Xy = you.SuperData[supery][0],			//Half extents of element...
			Yy = you.SuperData[supery][1],
			Zy = you.SuperData[supery][2];

float		offsetXy = you.SuperData[supery][3],	//Offset from origin(CM) of element...
			offsetYy = you.SuperData[supery][4],
			offsetZy = you.SuperData[supery][5];

float		you_mat[8][3] =	{ {-Xy, -Yy, -Zy},
							  { Xy, -Yy, -Zy},
							  { Xy,  Yy, -Zy},
							  {-Xy,  Yy, -Zy},

							  {-Xy, -Yy,  Zy},
							  { Xy, -Yy,  Zy},
							  { Xy,  Yy,  Zy},
							  {-Xy,  Yy,  Zy} };

//			Need to make this non-wasteful, also need to abstract so that we can handle other geometries...
//			===============================================================================================
			int count;
			for (count = 0; count < 8; count++) {

float			x_t = you_mat[count][0],
				y_t = you_mat[count][1],
				z_t = you_mat[count][2];

				you.Back_Element( x_t, y_t, z_t, supery );

float			A = x_t + offsetXy,						//Temps, will I need 'em?
				B = y_t + offsetYy,
				C = z_t + offsetZy;

float			V_x = A,								//First we'll get the local position of your vertex...
				V_y = B,
				V_z = C;

				you.Back( V_x, V_y, V_z );		//To global for you...

				V_x += Delta_x;
				V_y += Delta_y;
				V_z += Delta_z;

				From( V_x, V_y, V_z );		//Then to my local...

				//you_mat[count][0] = V_x;				//Save for edges...
				//you_mat[count][1] = V_y;
				//you_mat[count][2] = V_z;

//				Check here for extents!
//float			distal = 

//				Now moments...
float			dV_z = day*B - dby*A,					//Local moments...
				dV_y = dgy*A - day*C,
				dV_x = dby*C - dgy*B;

				you.Back( dV_x, dV_y, dV_z );	//Global for you...

				dV_x += you.State[0][1] - State[0][1];	//Should this me a minus?
				dV_y += you.State[1][1] - State[1][1];	//This can be done outside the loop...
				dV_z += you.State[2][1] - State[2][1];

				From( dV_x, dV_y, dV_z );		//Local to me now...

float			mdV_z = ad*V_y - bd*V_x,				//My rotation at your contact...
				mdV_y = gd*V_x - ad*V_z,
				mdV_x = bd*V_z - gd*V_y;

				dV_x -= mdV_x;							//Okay, there it is in local me space...
				dV_y -= mdV_y;
				dV_z -= mdV_z;

float			Xdotdot = 0,
				Ydotdot = 0,
				Zdotdot = 0;

float			atten;

				//V_x -= offsetX;
				//V_y -= offsetY;
				//V_z -= offsetZ;

float			VV_x = V_x - offsetX,		//For compound objects...
				VV_y = V_y - offsetY,
				VV_z = V_z - offsetZ;

float			dVV_x = dV_x,
				dVV_y = dV_y,
				dVV_z = dV_z;

				//From_Element( 
				From_Element(  VV_x,  VV_y,  VV_z, super );
				From_Element( dVV_x, dVV_y, dVV_z, super );

				you_mat[count][0] = VV_x;				//Save for edges...
				you_mat[count][1] = VV_y;
				you_mat[count][2] = VV_z;

//				Now we test for contact.  Probably, we should separate out the delta calculations until
//				After we're sure we'll need them...
//				===================================
float			comp_x = VV_x - X - IBC_coarseness,
				comp_y = VV_y - Y - IBC_coarseness,
				comp_z = VV_z - Z - IBC_coarseness;

CXob*			marker;
				if (you.Data[1] < Data[1]) 
					marker = &you;
				else
					marker = this;

				OurKappa = marker->Data[1];
				OurDelta = marker->Data[2];
				OurMu    = marker->Data[3];

				if ((Data[3] == 0) || (you.Data[3] == 0)) OurMu = 0;	//For Now...


#define FACES_ON
#ifdef FACES_ON

//				Z ...
//				=====

//				Even...
//				=======
				if ( (comp_z <= 0) && (comp_z >-IBC_depth*IBC_coarseness)
				  && (fabs(VV_x) < X + IBC_coarseness)
				  && (fabs(VV_y) < Y + IBC_coarseness) ) {

					if ( (comp_z <-IBC_coarseness) && ((fabs(VV_x) > X) || (fabs(VV_y) > Y)) ) break;

					if (comp_z <-IBC_LIM*IBC_coarseness)
					{	
						plane_interp( VV_x, VV_y, VV_z, 0, 0, 1, you);
						//comp_z *= 2;
					}

					atten = 1;
			
					if ( fabs(VV_x) > X ) atten *= 1 - ( fabs(VV_x) - X ) / IBC_coarseness;
					if ( fabs(VV_y) > Y ) atten *= 1 - ( fabs(VV_y) - Y ) / IBC_coarseness;
					//if (atten < 0) conPhysics << "ATTEN < 0 in IBC DOF\n";

					float test = OurDelta*dVV_z;
					if (dVV_z > 0) test *= BC_bounce;			//Bounciness again...

					Zdotdot = atten*(-IBC_kappa*OurKappa*comp_z - IBC_damp*test),	//Edge attenuation...
					Xdotdot = IBC_slide*atten*OurMu*dVV_x,
					Ydotdot = IBC_slide*atten*OurMu*dVV_y;

					if (bStoreFric)
					{
						fFricStore[0] = Xdotdot;
						fFricStore[1] = Ydotdot;
					}

					vsq_store  = current_timeslice*atten*IBC_damp*test*dVV_z;//dVV_z*dVV_z;
					vsqt_store =-current_timeslice*atten*OurMu*IBC_slide*(dVV_x*dVV_x + dVV_y*dVV_y);

					Back_Element( Xdotdot, Ydotdot, Zdotdot, super );
					iJumpCode = 0;
					Stuff_Contacts( you, Xdotdot, Ydotdot, Zdotdot,	V_x, V_y, V_z, A, B, C, super, supery );

				}	//Done with Z even...

#define OTHER_FACES
#ifdef OTHER_FACES

//				Odd...
//				======
				comp_z = - Z - IBC_coarseness - VV_z;

				if ( (comp_z <= 0) && (comp_z >-IBC_depth*IBC_coarseness)
				  && (fabs(VV_x) < X + IBC_coarseness)
				  && (fabs(VV_y) < Y + IBC_coarseness) ) {

					if ( (comp_z <-IBC_coarseness) && ((fabs(VV_x) > X) || (fabs(VV_y) > Y)) ) break;

					if (comp_z <-IBC_LIM*IBC_coarseness)
					{
						plane_interp( VV_x, VV_y, VV_z, 0, 0, -1, you);
						//comp_z *= 2;
					}

					atten = 1;
		
					if ( fabs(VV_x) > X ) atten *= 1 - ( fabs(VV_x) - X ) / IBC_coarseness;
					if ( fabs(VV_y) > Y ) atten *= 1 - ( fabs(VV_y) - Y ) / IBC_coarseness;
					//if (atten < 0) conPhysics << "ATTEN < 0 in IBC DOF\n";

					float test = OurDelta*dVV_z;
					if (dVV_z < 0) test *= BC_bounce;			//Bounciness again...

					Zdotdot = atten*( IBC_kappa*OurKappa*comp_z - IBC_damp*test),	//Edge attenuation...
					Xdotdot = IBC_slide*atten*OurMu*dVV_x,
					Ydotdot = IBC_slide*atten*OurMu*dVV_y;

					if (bStoreFric)
					{
						fFricStore[0] = Xdotdot;
						fFricStore[1] = Ydotdot;
					}

					vsq_store  = current_timeslice*atten*IBC_damp*test*dVV_z;//dVV_z*dVV_z;
					vsqt_store =-current_timeslice*atten*OurMu*IBC_slide*(dVV_x*dVV_x + dVV_y*dVV_y);

					Back_Element( Xdotdot, Ydotdot, Zdotdot, super );
					iJumpCode = 1;
					Stuff_Contacts( you, Xdotdot, Ydotdot, Zdotdot,	V_x, V_y, V_z, A, B, C, super, supery );

				}	//Done with Z odd...




//				X ...
//				=====

//				Even...
//				=======
				if ( (comp_x <= 0) && (comp_x >-IBC_depth*IBC_coarseness)
				  && (fabs(VV_z) < Z + IBC_coarseness)
				  && (fabs(VV_y) < Y + IBC_coarseness) ) {

					if ( (comp_x <-IBC_coarseness) && ((fabs(VV_z) > Z) || (fabs(VV_y) > Y)) ) break;

					if (comp_x <-IBC_LIM*IBC_coarseness)
					{
						plane_interp( VV_x, VV_y, VV_z, 1, 0, 0, you);
						//comp_x *= 2;
					}

					atten = 1;
				
					if ( fabs(VV_z) > Z ) atten *= 1 - ( fabs(VV_z) - Z ) / IBC_coarseness;
					if ( fabs(VV_y) > Y ) atten *= 1 - ( fabs(VV_y) - Y ) / IBC_coarseness;
					//if (atten < 0) conPhysics << "ATTEN < 0 in IBC DOF\n";

					float test = OurDelta*dVV_x;
					if (dVV_x > 0) test *= BC_bounce;			//Bounciness again...

					Xdotdot = atten*(-IBC_kappa*OurKappa*comp_x - IBC_damp*test),	//Edge attenuation...
					Zdotdot = IBC_slide*atten*OurMu*dVV_z,
					Ydotdot = IBC_slide*atten*OurMu*dVV_y;

					if (bStoreFric)
					{
						fFricStore[2] = Zdotdot;
						fFricStore[1] = Ydotdot;
					}

					vsq_store  = current_timeslice*atten*IBC_damp*test*dVV_x;
					vsqt_store =-current_timeslice*atten*OurMu*IBC_slide*(dVV_y*dVV_y + dVV_z*dVV_z);

					//vsq_store  = dVV_x*dVV_x;
					//vsqt_store = dVV_y*dVV_y + dVV_z*dVV_z;

					Back_Element( Xdotdot, Ydotdot, Zdotdot, super );
					iJumpCode = 2;
					Stuff_Contacts( you, Xdotdot, Ydotdot, Zdotdot,	V_x, V_y, V_z, A, B, C, super, supery );

				}	//Done with X even...


//				Odd...
//				======
				comp_x = - X - IBC_coarseness - VV_x;

				if ( (comp_x <= 0) && (comp_x >-IBC_depth*IBC_coarseness)
				  && (fabs(VV_z) < Z + IBC_coarseness)
				  && (fabs(VV_y) < Y + IBC_coarseness) ) {

					if ( (comp_x <-IBC_coarseness) && ((fabs(VV_z) > Z) || (fabs(VV_y) > Y)) ) break;

					if (comp_x <-IBC_LIM*IBC_coarseness)
					{
						plane_interp( VV_x, VV_y, VV_z,-1, 0, 0, you);
						//comp_x *= 2;
					}

					atten = 1;
		
					if ( fabs(VV_z) > Z ) atten *= 1 - ( fabs(VV_z) - Z ) / IBC_coarseness;
					if ( fabs(VV_y) > Y ) atten *= 1 - ( fabs(VV_y) - Y ) / IBC_coarseness;
					//if (atten < 0) conPhysics << "ATTEN < 0 in IBC DOF\n";

					float test = OurDelta*dVV_x;
					if (dVV_x < 0) test *= BC_bounce;			//Bounciness again...

					Xdotdot = atten*( IBC_kappa*OurKappa*comp_x - IBC_damp*test),	//Edge attenuation...
					Zdotdot = IBC_slide*atten*OurMu*dVV_z,
					Ydotdot = IBC_slide*atten*OurMu*dVV_y;

					if (bStoreFric)
					{
						fFricStore[2] = Zdotdot;
						fFricStore[1] = Ydotdot;
					}

					vsq_store  = current_timeslice*atten*IBC_damp*test*dVV_x;
					vsqt_store =-current_timeslice*atten*OurMu*IBC_slide*(dVV_y*dVV_y + dVV_z*dVV_z);

					//vsq_store  = dVV_x*dVV_x;
					//vsqt_store = dVV_y*dVV_y + dVV_z*dVV_z;

					Back_Element( Xdotdot, Ydotdot, Zdotdot, super );
					iJumpCode = 3;
					Stuff_Contacts( you, Xdotdot, Ydotdot, Zdotdot,	V_x, V_y, V_z, A, B, C, super, supery );

				}	//Done with X odd...


//				Y ...
//				=====

//				Even...
//				=======
				if ( (comp_y <= 0) && (comp_y >-IBC_depth*IBC_coarseness)
				  && (fabs(VV_x) < X + IBC_coarseness)
				  && (fabs(VV_z) < Z + IBC_coarseness) ) {

					if ( (comp_y <-IBC_coarseness) && ((fabs(VV_z) > Z) || (fabs(VV_x) > X)) ) break;

					if (comp_y <-IBC_LIM*IBC_coarseness)
					{
						plane_interp( VV_x, VV_y, VV_z, 0, 1, 0, you);
						//comp_y *= 2;
					}

					atten = 1;
				
					if ( fabs(VV_x) > X ) atten *= 1 - ( fabs(VV_x) - X ) / IBC_coarseness;
					if ( fabs(VV_z) > Z ) atten *= 1 - ( fabs(VV_z) - Z ) / IBC_coarseness;
					//if (atten < 0) conPhysics << "ATTEN < 0 in IBC DOF\n";

					float test = OurDelta*dVV_y;
					if (dVV_y > 0) test *= BC_bounce;			//Bounciness again...

					Ydotdot = atten*(-IBC_kappa*OurKappa*comp_y - IBC_damp*test),	//Edge attenuation...
					Xdotdot = IBC_slide*atten*OurMu*dVV_x,
					Zdotdot = IBC_slide*atten*OurMu*dVV_z;

					if (bStoreFric)
					{
						fFricStore[0] = Xdotdot;
						fFricStore[2] = Zdotdot;
					}

					vsq_store  = current_timeslice*atten*IBC_damp*test*dVV_y;
					vsqt_store =-current_timeslice*atten*OurMu*IBC_slide*(dVV_x*dVV_x + dVV_z*dVV_z);

					//vsq_store  = dVV_y*dVV_y;
					//vsqt_store = dVV_x*dVV_x + dVV_z*dVV_z;

					Back_Element( Xdotdot, Ydotdot, Zdotdot, super );
					iJumpCode = 4;
					Stuff_Contacts( you, Xdotdot, Ydotdot, Zdotdot,	V_x, V_y, V_z, A, B, C, super, supery );

				}	//Done with Y even...


//				Odd...
//				======
				comp_y = - Y - IBC_coarseness - VV_y;

				if ( (comp_y <= 0) && (comp_y >-IBC_depth*IBC_coarseness)
				  && (fabs(VV_x) < X + IBC_coarseness)
				  && (fabs(VV_z) < Z + IBC_coarseness) ) {

					if ( (comp_y <-IBC_coarseness) && ((fabs(VV_z) > Z) || (fabs(VV_x) > X)) ) break;

					if (comp_y <-IBC_LIM*IBC_coarseness)
					{
						plane_interp( VV_x, VV_y, VV_z, 0,-1, 0, you);
						//comp_y *= 2;
					}

					atten = 1;
		
					if ( fabs(VV_x) > X ) atten *= 1 - ( fabs(VV_x) - X ) / IBC_coarseness;
					if ( fabs(VV_z) > Z ) atten *= 1 - ( fabs(VV_z) - Z ) / IBC_coarseness;
					//if (atten < 0) conPhysics << "ATTEN < 0 in IBC DOF\n";

					float test = OurDelta*dVV_y;
					if (dVV_y < 0) test *= BC_bounce;			//Bounciness again...

					Ydotdot = atten*( IBC_kappa*OurKappa*comp_y - IBC_damp*test),	//Edge attenuation...
					Xdotdot = IBC_slide*atten*OurMu*dVV_x,
					Zdotdot = IBC_slide*atten*OurMu*dVV_z;

					if (bStoreFric)
					{
						fFricStore[0] = Xdotdot;
						fFricStore[2] = Zdotdot;
					}

					vsq_store  = current_timeslice*atten*IBC_damp*test*dVV_y;
					vsqt_store =-current_timeslice*atten*OurMu*IBC_slide*(dVV_x*dVV_x + dVV_z*dVV_z);

					//vsq_store  = dVV_y*dVV_y;
					//vsqt_store = dVV_x*dVV_x + dVV_z*dVV_z;

					Back_Element( Xdotdot, Ydotdot, Zdotdot, super );
					iJumpCode = 5;
					Stuff_Contacts( you, Xdotdot, Ydotdot, Zdotdot,	V_x, V_y, V_z, A, B, C, super, supery );

				}	//Done with Y odd...

#endif	//For OTHER_FACES

#endif	//For turning faces off...

			}	//End of loop...

#define LINES_ON
#ifdef LINES_ON

//		Here we make sure we are not duplicating calculations...
//		--------------------------------------------------------
		if ((BC_semaphore[i_you]<0) && (you.BC_semaphore[i_me]<0))
		{

		BC_semaphore[i_you] = you.BC_semaphore[i_me] = -1;


//		Now (GULP) check explicitly for edges and faces...
//		--------------------------------------------------

float	A_me[3],
		B_me[3],
		A_you[3],
		B_you[3];

float	Mx = 0, Bx = 0,
		My = 0, By = 0,
		Mz = 0, Bz = 0;

float	rho = 0,
		delta = 0,
		tmin = 0,
		tmax = 0;

float	BC_vec[3],
		BC_n[3],
		BC_val = 0;

//		Now we go for X_me...
//		---------------------
		for (count = 0; count < 4; count ++)
		{
			A_me[0] = work_mat[(Lines[count][0])][0];// + IBC_coarseness;
			A_me[1] = work_mat[(Lines[count][0])][1];
			A_me[2] = work_mat[(Lines[count][0])][2];

			B_me[0] = work_mat[(Lines[count][1])][0];// - IBC_coarseness;
			//B_me[1] = work_mat[(Lines[count][1])][1];
			//B_me[2] = work_mat[(Lines[count][1])][2];

//			Lupe over YOUR lines...
//			-----------------------
			for (int yline = 0; yline < 12; yline++ )
			{
				A_you[0] = you_mat[(Lines[yline][0])][0];
				A_you[1] = you_mat[(Lines[yline][0])][1];
				A_you[2] = you_mat[(Lines[yline][0])][2];

				B_you[0] = you_mat[(Lines[yline][1])][0];
				B_you[1] = you_mat[(Lines[yline][1])][1];
				B_you[2] = you_mat[(Lines[yline][1])][2];


#define ATEST_F
#define BTEST_F
#define CTEST_F

#ifdef ATEST_F

				float epsilon_plus  = A_me[2] + IBC_coarseness;
				float epsilon_minus = A_me[2] - IBC_coarseness;
				if (   !((A_you[2]>epsilon_plus) && (B_you[2]>epsilon_plus))
					&& !((A_you[2]<epsilon_minus) && (B_you[2]<epsilon_minus))
				   )
#endif
				{

#ifdef ATEST_F
				epsilon_plus  = A_me[1] + IBC_coarseness;
				epsilon_minus = A_me[1] - IBC_coarseness;
				if (   !((A_you[1]>epsilon_plus) && (B_you[1]>epsilon_plus))
					&& !((A_you[1]<epsilon_minus) && (B_you[1]<epsilon_minus))
				   )

				if ( 1 )// ((A_you[0]>A_me[0]) && (A_you[0]<B_me[0])) || ((B_you[0]>A_me[0]) && (B_you[0]<B_me[0])) )
#endif
				{


				delta = A_you[0] - B_you[0];
				if ( fabs(delta) > ORTHO_TOLERANCE )				//Are you secretly ortho?
				{
					My =(A_you[1] - B_you[1])/delta;
					Mz =(A_you[2] - B_you[2])/delta;
					By = A_you[1] - A_me[1] - My*A_you[0];
					Bz = A_you[2] - A_me[2] - Mz*A_you[0];
					if ( Mz*Mz + My*My > PARA_TOLERANCE )			//How about parallel?
					{						
						rho =-(By*My + Bz*Mz) / (My*My + Mz*Mz);	//There's the solution...
						if ( (rho > A_me[0]) && (rho < B_me[0]) )
						{
							//cout << "Passed 1...";
							minmax( A_you[0], B_you[0], tmin, tmax );
							if ( (rho > tmin) && (rho < tmax) )
							{
								//cout << "Passed 2...";
								BC_vec[0] = 0;						//There.  I wrote it.
								BC_vec[1] = My*rho + By;
								BC_vec[2] = Mz*rho + Bz;
		
								BC_val = BC_vec[1]*BC_vec[1] + BC_vec[2]*BC_vec[2];
								if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
								{
									//conPhysics << "Rho: " << rho << "         RM - " << BC_vec[0] << " : " << BC_vec[1] << " : " << BC_vec[2] << "\n";
									//conPhysics << "T: " << yline << ", rho: " << rho << "\n";
float								mine[3] = { rho, A_me[1], A_me[2] };	//Find locations...
float								your[3] = { mine[0], mine[1], mine[2] };

									if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
									BC_val = sqrt( BC_val );				//Only if you HAVE to...
									BC_n[0] = 0;
									BC_n[1] = BC_vec[1] / BC_val;
									BC_n[2] = BC_vec[2] / BC_val;

									//conPhysics << "Point 1 with " << me << " on " << you << "\n";
									get_BC_moments( you,
													your,
													mine,
													BC_n,
													BC_val,
													day, dby, dgy,
													ad, bd, gd,
													super, supery );

								}//	Actual kontakt(tm) test...
							
							}// Final range test...

						}// First range test...

					}//	Para-tolerance...
					//else { cout << "Para!\n"; }	//Probably do nothing...
					//else conPhysics << "Para!\n";

				}//	Ortho-tolerance, are we orthogonal???
				else if ( (A_you[0] > A_me[0]) && (A_you[0] < B_me[0]) )	//Ok, now we're restricted to 2 DOF...
				{ 
					delta = A_you[1] - B_you[1];
					if ( fabs(delta) > ORTHO_TOLERANCE )					//We have to test here too...
					{
						Mz =(A_you[2] - B_you[2])/delta;
						Bz = A_you[2] - A_me[2] - Mz*A_you[1];

						rho =-( Bz*Mz )/(1 + Mz*Mz);						//Note that we'll not need a test here...
						minmax( A_you[1], B_you[1], tmin, tmax );
						if ( (rho > tmin) && (rho < tmax) )
						{
							BC_vec[0] = 0;
							BC_vec[1] = rho;
							BC_vec[2] = Mz*rho + Bz;

							BC_val = BC_vec[1]*BC_vec[1] + BC_vec[2]*BC_vec[2];
							if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
							{
								//cout << "O";	flush(cout);
float							mine[3] = { A_you[0], A_me[1], A_me[2] };	//Find locations...
float							your[3] = { mine[0], mine[1], mine[2] };

								if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
								BC_val = sqrt( BC_val );				//Only if you HAVE to...
								BC_n[0] = 0;
								BC_n[1] = BC_vec[1] / BC_val;
								BC_n[2] = BC_vec[2] / BC_val;

								//conPhysics << "Point 2\n";
								get_BC_moments( you,
												your,
												mine,
												BC_n,
												BC_val,
												day, dby, dgy,
												ad, bd, gd,
												super, supery );

							}//	Actual ortho Kontakt(tm)...

						}//	Test on ortho range...

					}//	Ortho case...
					else if ( fabs( delta = (A_you[1] - A_me[1])) <= IBC_coarseness )	//Failed Ortho - straight X-Z...
					{
						//cout << "del: " << delta << "\n";	flush( cout );

						if ( fabs(delta) < DIV_TOL ) delta = fsign(delta)*DIV_TOL;	//Only you etc...
						BC_vec[0] = BC_vec[2] = 0;
						BC_vec[1] = delta;
						
						BC_n[0] = BC_n[2] = 0;
						BC_n[1] = fsign(delta);//delta/fabs(delta);

						BC_val = fabs(delta);

float					mine[3] = { A_you[0], A_me[1], A_me[2] },
						your[3] = { mine[0], mine[1], mine[2] };

						//conPhysics << "Point 3\n";
						get_BC_moments( you,
										your,
										mine,
										BC_n,
										BC_val,
										day, dby, dgy,
										ad, bd, gd,
										super, supery );

					}//	Kontakt in the orthonormal case...

				}//	Are you ortho but in range?

			} }//Efficiency...

			}//	End of your line...

		}//	End of X_me line...


#define OTHERS
#ifdef OTHERS
//		Now we go for Y_me...
//		---------------------
		for (count = 4; count < 8; count ++)
		{
			A_me[0] = work_mat[(Lines[count][0])][0];
			A_me[1] = work_mat[(Lines[count][0])][1];// + IBC_coarseness;
			A_me[2] = work_mat[(Lines[count][0])][2];

			//B_me[0] = work_mat[(Lines[count][1])][0];
			B_me[1] = work_mat[(Lines[count][1])][1];// - IBC_coarseness;
			//B_me[2] = work_mat[(Lines[count][1])][2];

//			Lupe over YOUR lines...
//			-----------------------
			for (int yline = 0; yline < 12; yline++ )
			{
				A_you[0] = you_mat[(Lines[yline][0])][0];
				A_you[1] = you_mat[(Lines[yline][0])][1];
				A_you[2] = you_mat[(Lines[yline][0])][2];

				B_you[0] = you_mat[(Lines[yline][1])][0];
				B_you[1] = you_mat[(Lines[yline][1])][1];
				B_you[2] = you_mat[(Lines[yline][1])][2];



#ifdef BTEST_F
				float epsilon_plus  = A_me[2] + IBC_coarseness;
				float epsilon_minus = A_me[2] - IBC_coarseness;
				if (   !((A_you[2]>epsilon_plus) && (B_you[2]>epsilon_plus))
					&& !((A_you[2]<epsilon_minus) && (B_you[2]<epsilon_minus))
				   )
#endif
				{

#ifdef BTEST_F
				epsilon_plus  = A_me[0] + IBC_coarseness;
				epsilon_minus = A_me[0] - IBC_coarseness;
				if (   !((A_you[0]>epsilon_plus) && (B_you[0]>epsilon_plus))
					&& !((A_you[0]<epsilon_minus) && (B_you[0]<epsilon_minus))
				   )

				if ( 1 )// ((A_you[1]>A_me[1]) && (A_you[1]<B_me[1])) || ((B_you[1]>A_me[1]) && (B_you[1]<B_me[1])) )
#endif
				{

				delta = A_you[1] - B_you[1];
				if ( fabs(delta) > ORTHO_TOLERANCE )				//Are you secretly ortho?
				{
					Mx =(A_you[0] - B_you[0])/delta;
					Mz =(A_you[2] - B_you[2])/delta;
					Bx = A_you[0] - A_me[0] - Mx*A_you[1];
					Bz = A_you[2] - A_me[2] - Mz*A_you[1];
					if ( Mz*Mz + Mx*Mx > PARA_TOLERANCE )			//How about parallel?
					{						
						//if ( (Mx*Mx < .01) || (Mz*Mz < .01) ) cout << "!"; flush( cout );
						rho =-(Bx*Mx + Bz*Mz) / (Mx*Mx + Mz*Mz);	//There's the solution...
						if ( (rho > A_me[1]) && (rho < B_me[1]) )
						{
							//cout << "Passed 1...";
							minmax( A_you[1], B_you[1], tmin, tmax );
							if ( (rho > tmin) && (rho < tmax) )
							{
								//cout << "Passed 2...";
								BC_vec[0] = Mx*rho + Bx;			//There.  I wrote it.
								BC_vec[1] = 0;
								BC_vec[2] = Mz*rho + Bz;
		
								BC_val = BC_vec[0]*BC_vec[0] + BC_vec[2]*BC_vec[2];
								if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
								{
									//cout << "Me"; flush(cout);
float								mine[3] = { A_me[0], rho, A_me[2] };	//Find locations...
float								your[3] = { mine[0], mine[1], mine[2] };

										if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
										BC_val = sqrt( BC_val );				//Only if you HAVE to...
										BC_n[0] = BC_vec[0] / BC_val;
										BC_n[1] = 0;
										BC_n[2] = BC_vec[2] / BC_val;

										get_BC_moments( you,
														your,
														mine,
														BC_n,
														BC_val,
														day, dby, dgy,
														ad, bd, gd,
														super, supery );
									
								}//	Actual kontakt(tm) test...
							
							}// Final range test...

						}// First range test...

					}//	Para-tolerance...
					//else { cout << "Para!\n"; }	//Probably do nothing...

				}//	Ortho-tolerance, are we orthogonal???
				else if ( (A_you[1] > A_me[1]) && (A_you[1] < B_me[1]) )	//Ok, now we're restricted to 2 DOF...
				{ 
					delta = A_you[0] - B_you[0];
					if ( fabs(delta) > ORTHO_TOLERANCE )					//We have to test here too...
					{
						Mz =(A_you[2] - B_you[2])/delta;
						Bz = A_you[2] - A_me[2] - Mz*A_you[0];

						rho =-( Bz*Mz )/(1 + Mz*Mz);						//Note that we'll not need a test here...
						minmax( A_you[0], B_you[0], tmin, tmax );
						if ( (rho > tmin) && (rho < tmax) )
						{
							BC_vec[0] = rho;
							BC_vec[1] = 0;
							BC_vec[2] = Mz*rho + Bz;

							BC_val = BC_vec[0]*BC_vec[0] + BC_vec[2]*BC_vec[2];
							if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
							{
								//cout << "O";	flush(cout);
float							mine[3] = { A_me[0], A_you[1], A_me[2] };	//Find locations...
float							your[3] = { mine[0], mine[1], mine[2] };

									if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
									BC_val = sqrt( BC_val );				//Only if you HAVE to...
									BC_n[0] = BC_vec[0] / BC_val;
									BC_n[1] = 0;
									BC_n[2] = BC_vec[2] / BC_val;

									get_BC_moments( you,
													your,
													mine,
													BC_n,
													BC_val,
													day, dby, dgy,
													ad, bd, gd,
													super, supery );
								
							}//	Actual ortho Kontakt(tm)...

						}//	Test on ortho range...

					}//	Ortho case...
					else if ( fabs( delta = (A_you[0] - A_me[0])) <= IBC_coarseness )	//Failed Ortho - straight Y-Z...
					{
						//cout << "del: " << delta << "\n";	flush( cout );

						if ( fabs(delta) < DIV_TOL ) delta = fsign(delta)*DIV_TOL;	//Only you etc...
						BC_vec[1] = BC_vec[2] = 0;
						BC_vec[0] = delta;
						
						BC_n[1] = BC_n[2] = 0;
						BC_n[0] = fsign(delta);//delta/fabs(delta);

						BC_val = fabs(delta);

float					mine[3] = { A_me[0], A_you[1], A_me[2] },
						your[3] = { mine[0], mine[1], mine[2] };

						get_BC_moments( you,
										your,
										mine,
										BC_n,
										BC_val,
										day, dby, dgy,
										ad, bd, gd,
										super, supery );

					}//	Kontakt in the orthonormal case...

				}//	Are you ortho but in range?

			} }//Efficiency...

			}//	End of your line...

		}//	End of Y_me line...


//		Now, before E3, go for Z_me...
//		------------------------------
		for (count = 8; count < 12; count ++)
		{
			A_me[0] = work_mat[(Lines[count][0])][0];
			A_me[1] = work_mat[(Lines[count][0])][1];
			A_me[2] = work_mat[(Lines[count][0])][2];// + IBC_coarseness;

			//B_me[0] = work_mat[(Lines[count][1])][0] - IBC_coarseness;
			//B_me[1] = work_mat[(Lines[count][1])][1];
			B_me[2] = work_mat[(Lines[count][1])][2];// - IBC_coarseness;

//			Lupe over YOUR lines...
//			-----------------------
			for (int yline = 0; yline < 12; yline++ )
			{
				A_you[0] = you_mat[(Lines[yline][0])][0];
				A_you[1] = you_mat[(Lines[yline][0])][1];
				A_you[2] = you_mat[(Lines[yline][0])][2];

				B_you[0] = you_mat[(Lines[yline][1])][0];
				B_you[1] = you_mat[(Lines[yline][1])][1];
				B_you[2] = you_mat[(Lines[yline][1])][2];


#ifdef CTEST_F
				float epsilon_plus  = A_me[0] + IBC_coarseness;
				float epsilon_minus = A_me[0] - IBC_coarseness;
				if (   !((A_you[0]>epsilon_plus) && (B_you[0]>epsilon_plus))
					&& !((A_you[0]<epsilon_minus) && (B_you[0]<epsilon_minus))
				   )
#endif
				{

#ifdef CTEST_F
				epsilon_plus  = A_me[1] + IBC_coarseness;
				epsilon_minus = A_me[1] - IBC_coarseness;
				if (   !((A_you[1]>epsilon_plus) && (B_you[1]>epsilon_plus))
					&& !((A_you[1]<epsilon_minus) && (B_you[1]<epsilon_minus))
				   )
				if ( 1 )//((A_you[2]>A_me[2]) && (A_you[2]<B_me[2])) || ((B_you[2]>A_me[2]) && (B_you[2]<B_me[2])) )
#endif
				{

				delta = A_you[2] - B_you[2];
				if ( fabs(delta) > ORTHO_TOLERANCE )				//Are you secretly ortho?
				{
					Mx =(A_you[0] - B_you[0])/delta;
					My =(A_you[1] - B_you[1])/delta;
					Bx = A_you[0] - A_me[0] - Mx*A_you[2];
					By = A_you[1] - A_me[1] - My*A_you[2];
					if ( Mx*Mx + My*My > PARA_TOLERANCE )			//How about parallel?
					{						
						rho =-(Bx*Mx + By*My) / (Mx*Mx + My*My);	//There's the solution...
						//cout << "T: " << yline << ", rho: " << rho << "\n";
						if ( (rho > A_me[2]) && (rho < B_me[2]) )
						{
							//cout << "Passed 1...";
							minmax( A_you[2], B_you[2], tmin, tmax );
							if ( (rho > tmin) && (rho < tmax) )
							{
								//cout << "Passed 2...";
								BC_vec[0] = Mx*rho + Bx;			//There.  I wrote it.
								BC_vec[1] = My*rho + By;
								BC_vec[2] = 0;
		
								BC_val = BC_vec[0]*BC_vec[0] + BC_vec[1]*BC_vec[1];
								if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
								{
float								mine[3] = { A_me[0], A_me[1], rho };		//Find locations...
float								your[3] = { mine[0], mine[1], mine[2] };

									if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
									BC_val = sqrt( BC_val );				//Only if you HAVE to...
									BC_n[0] = BC_vec[0] / BC_val;
									BC_n[1] = BC_vec[1] / BC_val;
									BC_n[2] = 0;

									get_BC_moments( you,
													your,
													mine,
													BC_n,
													BC_val,
													day, dby, dgy,
													ad, bd, gd,
													super, supery );

								}//	Actual kontakt(tm) test...
							
							}// Final range test...

						}// First range test...

					}//	Para-tolerance...
					//else { cout << "Para!\n"; }	//Probably do nothing...

				}//	Ortho-tolerance, are we orthogonal???
				else if ( (A_you[2] > A_me[2]) && (A_you[2] < B_me[2]) )	//Ok, now we're restricted to 2 DOF...
				{ 
					delta = A_you[0] - B_you[0];
					if ( fabs(delta) > ORTHO_TOLERANCE )					//We have to test here too...
					{
						My =(A_you[1] - B_you[1])/delta;
						By = A_you[1] - A_me[1] - My*A_you[0];

						rho =-( By*My )/(1 + My*My);						//Note that we'll not need a test here...
						minmax( A_you[0], B_you[0], tmin, tmax );
						if ( (rho > tmin) && (rho < tmax) )
						{
							BC_vec[0] = rho;
							BC_vec[1] = My*rho + By;
							BC_vec[2] = 0;

							BC_val = BC_vec[0]*BC_vec[0] + BC_vec[1]*BC_vec[1];
							if ( BC_val <= IBC_coarseness*IBC_coarseness )	//Ok, a valid kontakt(tm)...
							{
								//cout << "O";	flush(cout);
float							mine[3] = { A_me[0], A_me[1], A_you[2] };	//Find locations...
float							your[3] = { mine[0], mine[1], mine[2] };

								if (BC_val < DIV_TOL) BC_val = DIV_TOL;	//Only you can prevent singularities...
								BC_val = sqrt( BC_val );				//Only if you HAVE to...
								BC_n[0] = BC_vec[0] / BC_val;
								BC_n[1] = BC_vec[1] / BC_val;
								BC_n[2] = 0;

								get_BC_moments( you,
												your,
												mine,
												BC_n,
												BC_val,
												day, dby, dgy,
												ad, bd, gd,
												super, supery );

							}//	Actual ortho Kontakt(tm)...

						}//	Test on ortho range...

					}//	Ortho case...
					else if ( fabs( delta = (A_you[0] - A_me[0])) <= IBC_coarseness )	//Failed Ortho - straight X-Z...
					{
						//cout << "del: " << delta << "\n";	flush( cout );

						if ( fabs(delta) < DIV_TOL ) delta = fsign(delta)*DIV_TOL;	//Only you etc...
						BC_vec[0] = BC_vec[1] = 0;
						BC_vec[2] = delta;
						
						BC_n[0] = BC_n[1] = 0;
						BC_n[2] = fsign(delta);//delta/fabs(delta);

						BC_val = fabs(delta);

float					mine[3] = { A_me[0], A_me[1], A_you[2] },
						your[3] = { mine[0], mine[1], mine[2] };

						get_BC_moments( you,
										your,
										mine,
										BC_n,
										BC_val,
										day, dby, dgy,
										ad, bd, gd,
										super, supery );

					}//	Kontakt in the orthonormal case...

				}//	Are you ortho but in range?

			} }//Efficiency...
 
			}//	End of your line...

		}//	End of Z_me line...


#endif	//For OTHERS...

	}//BIG GAY BOB'S BIG GAY LINE TEST
#endif //For turning off lines...
		}	//End of supery...

		}	//End of IBC loop...

	}//End of loop for SooperBoxes...



//	Here is the idea that ROB SUGGESTED for semaphores...
	int i_me = Index();
	for (int you = 0; you < GUYS; you++) if (Do_It[you])
	{
		if ((BC_semaphore[you]==-1) && (Xob[you].BC_semaphore[i_me]==-1))
			BC_semaphore[you] = Xob[you].BC_semaphore[i_me] = 1;
	}



//	Add inertial couplings...
//	=========================
	//alpha_total += gd*bd*( 1/Data[9] = 1/Data[8] );
	//beta_total  += ad*gd*( 1/Data[7] = 1/Data[9] );
	//gamma_total += bd*ad*( 1/Data[8] = 1/Data[7] );


//	Needs to go AFTER all DOF are computed, but test on terrain for now...
/*
	if ((Data[11] > 1) && (Data[10] > .3))
	{
bool	bPulled[COMPOUND_OVERSPEC];
		for (int init = 0; init < 30; init++) bPulled[init] = false;

//		Now go for it...
		for( int a = 1; a <= comp_index; a++ )
		{
			if (!bPulled[a])
			{
				for ( int b = (a+1); b <= comp_index; b++ )
				{
					float test = (position[a][0] - position[b][0])*(position[a][0] - position[b][0])
							   + (position[a][1] - position[b][1])*(position[a][1] - position[b][1])
							   + (position[a][2] - position[b][2])*(position[a][2] - position[b][2]);

					//conPhysics << a << " on " << b << " = " << test << "\n";

					if ( test < .01 )//BC_coarseness*BC_coarseness )
					{
						bPulled[b] = true;
						//conPhysics << "failed: " << a << " on " << b << " d: " << test << "\n";

						State[0][2] -= SubInp[b][0][0];
						State[1][2] -= SubInp[b][0][1];
						State[2][2] -= SubInp[b][0][2];
	
						alpha_total -= SubInp[b][1][0];
						beta_total  -= SubInp[b][1][1];
						gamma_total -= SubInp[b][1][2];

					}	//Failed test..
				}	//Loop on second return...
			}	//rejection test...
		}	//loop on initial return...

//	conPhysics << "Who made it-- ";
//	for (init = 1; init <= comp_index; init++) if (!bPulled[init]) conPhysics << init << " ";
//	conPhysics << "\n";
	}	//test for compound...

*/

//	And check...
//	------------
	if (Data[11] > 1)
	{
		float tes = sqrt( State[0][2]*State[0][2] +
						  State[1][2]*State[1][2] +
						  State[2][2]*State[2][2] );

		float tmax = 800 / Data[0];
	
		if ( tes > tmax )
		{
			float mul = tmax / tes;
			State[0][2] *= mul;
			State[1][2] *= mul;
			State[2][2] *= mul;
			conPhysics << "Terr max!\n";
		}
	}


//	Now rescale...
//	==============
float	atemp = Tau[0] = Data[7]*alpha_total,
		btemp = Tau[1] = Data[8]*beta_total,
		gtemp = Tau[2] = Data[9]*gamma_total;

		if (!Ignorable_DOF[3]) atemp = 0;	//Test for ignorable axes...
		if (!Ignorable_DOF[4]) btemp = 0;
		if (!Ignorable_DOF[5]) gtemp = 0;

		//State[3][1] =-.5*( a1*ad + a2*bd + a3*gd );
		//State[4][1] = .5*( a0*ad - a3*bd + a2*gd );
		//State[5][1] = .5*( a3*ad + a0*bd - a1*gd );
		//State[6][1] = .5*(-a2*ad + a1*bd + a0*gd );

		//conPhysics << "T: " << Tau[0] << " : " << Tau[1] << " : " << Tau[2] << "\n";

	State[3][2] =-.5*( a1*atemp + a2*btemp + a3*gtemp
						 + d1*ad    + d2*bd    + d3*gd    );
	State[4][2] = .5*( a0*atemp - a3*btemp + a2*gtemp 
						 + d0*ad    - d3*bd    + d2*gd    );
	State[5][2] = .5*( a3*atemp + a0*btemp - a1*gtemp 
						 + d3*ad    + d0*bd    - d1*gd    );
	State[6][2] = .5*(-a2*atemp + a1*btemp + a0*gtemp 
						  -d2*ad    + d1*bd    + d0*gd    );





//	Correct our wayward ways...
//	===========================
float	Ld =  -2*( a0*d0 + a1*d1 + a2*d2 + a3*d3 );
float	L  = 1 - ( a0*a0 + a1*a1 + a2*a2 + a3*a3 );

		//conPhysics << "L: " << L << ", Ld: " << Ld << "\n";
		//conPhysics.Show();

		L  *= .5;//L_Mult;//.5;//50;
		Ld *= .1;//L_dot_mult;//.1;//5;;//50;//.05;

	State[3][2] += a0*Ld + d0*L;
	State[4][2] += a1*Ld + d1*L;
	State[5][2] += a2*Ld + d2*L;
	State[6][2] += a3*Ld + d3*L;

	State[3][1] += a0*L;
	State[4][1] += a1*L;
	State[5][1] += a2*L;
	State[6][1] += a3*L;


//	Linear...
//	=========
	State[0][2] *= Data[0];
	State[1][2] *= Data[0];
	State[2][2]  = Data[0]*State[2][2] - 10;//10 10 10 10 10 10 10 10 10 10 10

//	Done!
//	=====
}



//	RESIZE!
//	Takes a vector and resizes it for physics box use...
void CXob::XOBResize( float vec[3] )
{

#define PCT .1//1//.25

	if (vec[0] < PCT*IBC_coarseness) vec[0] = PCT*IBC_coarseness;
	if (vec[1] < PCT*IBC_coarseness) vec[1] = PCT*IBC_coarseness;
	if (vec[2] < PCT*IBC_coarseness) vec[2] = PCT*IBC_coarseness;
}



//	Initialize a guy...
//	-------------------
//	NOTE:  Superblocks will be initialized in the position of the zeroth element's cm.  The offset is
//		   returned based on that position.
void CXob::Create( 
				     int elements,						//The number of subelements...
					 float magnets[MAX_ELEMENTS][3],	//Magnet locations in frame 0...
					 float strength[MAX_ELEMENTS],		//Magnet breaking strength...
			  	     float extents[MAX_ELEMENTS][6],	//Element offsets and sizes...
					 float orient[MAX_ELEMENTS][3][3],	//Element orientations...
					 float mass[MAX_ELEMENTS],			//Element masses...
				     float slip[MAX_ELEMENTS],			//and slips...
					 int material[MAX_ELEMENTS],		//and materials...
					 bool movable,						//Whether it can move...
					 bool b_floats,						//Whether it floats...
					 bool live_DOF[6],					//1 for live, 0 for dead...
					 float hinge_strength,				//Restoring force, 0..10...
					 float hinge_damp,					//Hinge friction, 0..10...
					 float hinge_drive,					//Hinge drive value, 0..10...
					 bool hinge_limits,					//Whether hinge has limits; if so see below...
					 float hinge_xzero[3],				//Local x direction vector for zero-point.
					 float hinge_xmin[3],				//World limits of local x direction vector for rotation.
					 float hinge_xmax[3],	
				     float init[7][3],					//Initial state of element 0...
					 int pelvis,						//This box intimately, sensuously intertwined with bio model.
					 int pelvis_elem
					 )
{



//		OK.  Here is the massive dumpage...

		int i_me = Index();

		conPhysics << "Number: " << i_me << ", with " << elements << " elements\n=====================";
float	detonate( float a[3][3] );

		for (int print = 0; print < elements; print++ )
		{
			conPhysics << "element " << print << ":\n";
			conPhysics << "magnets[" << print << "][] = " << magnets[print][0] << ", " << magnets[print][1] << ", " << magnets[print][2] << "\n";
			conPhysics << "strength[" << print << "] = " << strength[print] << "\n";
			conPhysics << "extents[" << print << "][0-2] = " << extents[print][0] << ", " << extents[print][1] << ", " << extents[print][2] << "\n";
			conPhysics << "extents[" << print << "][3-5] = " << extents[print][3] << ", " << extents[print][4] << ", " << extents[print][5] << "\n";
			
			conPhysics << "O:\n";
			conPhysics << orient[print][0][0] << "	" << orient[print][0][1] << "	" << orient[print][0][2] << "\n";
			conPhysics << orient[print][1][0] << "	" << orient[print][1][1] << "	" << orient[print][1][2] << "\n";
			conPhysics << orient[print][2][0] << "	" << orient[print][2][1] << "	" << orient[print][2][2] << "\n";

			conPhysics << "Det: " << detonate( orient[print] ) << "\n\n";

			conPhysics << "mass[" << print << "] = " << mass[print] << "\n";
			conPhysics << "slip[" << print << "] = " << slip[print] << "\n";
			conPhysics << "=-=-=-=-=-=-=-=-=-=-=-=-=\n";
		}
			conPhysics << "movable = " << movable << "\n";
			conPhysics << "live_DOF[] = " << live_DOF[0] << ", " << live_DOF[1] << ", " << live_DOF[2] << "\n";

			conPhysics << "Init:\n";
			conPhysics << init[0][0] << ", " << init[0][1] << ", " << init[0][2] << "\n";
			conPhysics << init[1][0] << ", " << init[1][1] << ", " << init[1][2] << "\n";
			conPhysics << init[2][0] << ", " << init[2][1] << ", " << init[2][2] << "\n";

			conPhysics << init[3][0] << ", " << init[3][1] << ", " << init[3][2] << "\n";
			conPhysics << init[4][0] << ", " << init[4][1] << ", " << init[4][2] << "\n";
			conPhysics << init[5][0] << ", " << init[5][1] << ", " << init[5][2] << "\n";
			conPhysics << init[6][0] << ", " << init[6][1] << ", " << init[6][2] << "\n";

			conPhysics << "Pelvis = " << pelvis << ", " << pelvis_elem << "\n\n---------------------------\n\n";

//		Keeping tabs...
		Info = INFO_MAX;

//		Establish link to pelvis element.
		PelvisModel = pelvis;
		PelvisElem = pelvis_elem;

		if (pelvis >= 0)
		{
			Assert(pelvis_elem >= 0);
//			Set the reverse mapping too.
			Pel_Box_BC[pelvis][pelvis_elem] = i_me;
		}



//		Calculate CM...
//		---------------
float	cm[3] = {0,0,0};
float	M = 0;

int		index = 0;

//		Copy the orientation...
//		-----------------------
		for ( index = 0; index < elements; index++)
		{
//			Resize things for BC purposes...
			if(extents[index][0] > IBC_coarseness) extents[index][0] -= .4*IBC_coarseness;
			if(extents[index][1] > IBC_coarseness) extents[index][1] -= .4*IBC_coarseness;
			if(extents[index][2] > IBC_coarseness) extents[index][2] -= .4*IBC_coarseness;

#define RATIO .1//.5                    //1//.5
			//if(extents[index][0] < RATIO*IBC_coarseness) extents[index][0] = RATIO*IBC_coarseness;
			//if(extents[index][1] < RATIO*IBC_coarseness) extents[index][1] = RATIO*IBC_coarseness;
			//if(extents[index][2] < RATIO*IBC_coarseness) extents[index][2] = RATIO*IBC_coarseness;

			//extents[index][2] = 0;

			SuperOrient[index][0][0] = orient[index][0][0];
			SuperOrient[index][1][0] = orient[index][1][0];
			SuperOrient[index][2][0] = orient[index][2][0];
			SuperOrient[index][0][1] = orient[index][0][1];
			SuperOrient[index][1][1] = orient[index][1][1];
			SuperOrient[index][2][1] = orient[index][2][1];
			SuperOrient[index][0][2] = orient[index][0][2];
			SuperOrient[index][1][2] = orient[index][1][2];
			SuperOrient[index][2][2] = orient[index][2][2];

			// Store a flag for whether this element is rotated.
			Rotated[index] = 
				orient[index][0][0] != 1.0f ||
				orient[index][1][1] != 1.0f ||
				orient[index][2][2] != 1.0f;

//			Calculate total mass...
			M += mass[index];

			Breakages[index] = strength[index];

//			Copy materials...
			Sound[index] = material[index];
		}


//		Calculate CM and all its data and options...
//		============================================
		Anchored = movable && (!live_DOF[0] && !live_DOF[1] && !live_DOF[2]);

		if (Anchored)			// If there is no translational freedom, we anchor the object at its magnet pivot...
		{
			// Magnet 0 acts as CM.
			cm[0] = magnets[0][0];
			cm[1] = magnets[0][1];
			cm[2] = magnets[0][2];
		}
		else if (pelvis >= 0/* && elements == 1*/)
		{
//			For bio-boxes, respect the given origin...
			cm[0] = cm[1] = cm[2] = 0;
		}
		else
		{
//			Calculate CM...
			for ( index = 0; index < elements; index++)
			{
				cm[0] += mass[index]*extents[index][3];	//Individual cm offsetts...
				cm[1] += mass[index]*extents[index][4];
				cm[2] += mass[index]*extents[index][5];
			}
			cm[0] /= M;
			cm[1] /= M;
			cm[2] /= M;

			Assert(!Rotated[0]);
		}


//		Immobile objjects need to be maxed...
		if ( !movable ) M = 10000.0;


		Data[19] = M;
		conPhysics << "!!!!!!!!!!!!!!!!!!!Real Mass = " << M << "\n";

		//if ( pelvis_elem == RIGHT_FOOT ) M = 20;

//		Why is this here?
#define mMAX 10//20//100
		if (M > mMAX)
		{
			M = mMAX;
			//conPhysics << "Rescale from " << Data[19] << " to " << M << "\n";
			for (index = 0; index < elements; index++) mass[index] = M/(float)elements;
		}

		if (M < 1) M = 1;


//		Now, stuff the SuperData and Extents...
//		---------------------------------------
		Data[11] = elements;

		int i;
		for (i = 0; i < 6; i++)	Extents[i] = 0;

float	radius_sqr = 0;
float	II[3] = {0,0,0};

float	Imass = M / (12 * 8);

		for ( index = 0; index < elements; index++)
		{
float		X = SuperData[index][0] = extents[index][0],	//LOCAL XYZ extents...
			Y = SuperData[index][1] = extents[index][1],
			Z = SuperData[index][2] = extents[index][2];

float		offsetX = SuperData[index][3] = extents[index][3] - cm[0],
			offsetY = SuperData[index][4] = extents[index][4] - cm[1],
			offsetZ = SuperData[index][5] = extents[index][5] - cm[2];

//			Compute all local vertices, and get extents...
//			----------------------------------------------
float		work_mat[8][3] = { {-X, -Y, -Z},
							   { X, -Y, -Z},
							   { X,  Y, -Z},
							   {-X,  Y, -Z},
	
							   {-X, -Y,  Z},
							   { X, -Y,  Z},
							   { X,  Y,  Z},
							   {-X,  Y,  Z} };


			for (int count = 0; count < 8; count++)
			{
float			x_v = work_mat[count][0],
				y_v = work_mat[count][1],
				z_v = work_mat[count][2];

				Back_Element( x_v, y_v, z_v, index );	//To the element object itself...
				x_v += offsetX;	y_v += offsetY;	z_v += offsetZ;

				II[0] += (y_v*y_v + z_v*z_v);
				II[1] += (x_v*x_v + z_v*z_v);
				II[2] += (y_v*y_v + x_v*x_v);

//				Now find max extents...
//				-----------------------
				SetMin(Extents[0], x_v);
				SetMin(Extents[1], y_v);
				SetMin(Extents[2], z_v);

				SetMax(Extents[3], x_v);
				SetMax(Extents[4], y_v);
				SetMax(Extents[5], z_v);

				SetMax(radius_sqr, x_v*x_v + y_v*y_v + z_v*z_v);
			}
		}

		Data[7] = .1 / ( Imass*II[0] );
		Data[8] = .1 / ( Imass*II[1] );
		Data[9] = .1 / ( Imass*II[2] );



//		Adjust extents back to world size...
//		------------------------------------
		for (i = 0; i < 3; i++)
		{
			Extents[i] -= IBC_bound;
		}
		for (; i < 6; i++)
		{
			Extents[i] += IBC_bound;
		}

//		Compute radius here too; grow Data[][10] to provide safety...
//		--------------------------------------------------------------
		Radius = sqrt(radius_sqr) + IBC_bound;
		Data[10] = Radius + IBC_coarseness;

//		Compute max extent ratio...
//		---------------------------

float	ExtMin = Min(Extents[3], -Extents[0]);
		SetMin(ExtMin, Min(Extents[4], -Extents[1]));
		SetMin(ExtMin, Min(Extents[5], -Extents[2]));

		ExtentRatio = Radius / ExtMin;

//		Now, using the CM info, initialize the state...
//		-----------------------------------------------
		int ii;
		for (ii = 0; ii < 7; ii++ )	State[ii][2] = 0;

		State[3][0] = init[3][0];		State[3][1] = init[3][1];
		State[4][0] = init[4][0];		State[4][1] = init[4][1];
		State[5][0] = init[5][0];		State[5][1] = init[5][1];
		State[6][0] = init[6][0];		State[6][1] = init[6][1];

		Make_Transform();
		Back( cm[0], cm[1], cm[2] );

		State[0][0] = init[0][0] + cm[0];		State[0][1] = init[0][1];
		State[1][0] = init[1][0] + cm[1];		State[1][1] = init[1][1];
		State[2][0] = init[2][0] + cm[2];		State[2][1] = init[2][1];


/*
//		Calculate moments...
		if (elements == 1 && !Anchored)
		{

//float		X = Data[4] = .5;//extents[0][0];
//float		Y = Data[5] = .5;//extents[0][1];
//float		Z = Data[6] = .5;//extents[0][2];

float		X = Data[4] = extents[0][0];
float		Y = Data[5] = extents[0][1];
float		Z = Data[6] = extents[0][2];

			Data[7] = 1.2 / (M*( Y*Y + Z*Z ));	//Inverse MOI...
			Data[8] = 1.2 / (M*( X*X + Z*Z ));
			Data[9] = 1.2 / (M*( Y*Y + X*X ));

		
		}
		
		else
		{

			float II[3] = {0,0,0};
			for (index = 0; index < elements; index++)
			{

float		X = extents[index][0];
float		Y = extents[index][1];
float		Z = extents[index][2];
				
			II[0] += mass[index]*( Y*Y + Z*Z ) / 12;
			II[1] += mass[index]*( X*X + Z*Z ) / 12;
			II[2] += mass[index]*( Y*Y + X*X ) / 12;

			II[0] += (extents[index][5]*extents[index][5] + extents[index][4]*extents[index][4])*mass[index];
			II[1] += (extents[index][3]*extents[index][3] + extents[index][5]*extents[index][5])*mass[index];
			II[2] += (extents[index][3]*extents[index][3] + extents[index][4]*extents[index][4])*mass[index];

			conPhysics << "Is have it: " << II[0] << ", " << II[1] << ", " << II[2] << "\n";

			}
			Data[7] = .10 / II[0];
			Data[8] = .10 / II[1];
			Data[9] = .10 / II[2];

			//Data[7] = .50;
			//Data[8] = .50;
			//Data[9] = .50;

		}

*/

		conPhysics << "I: " << Data[7] << ", " << Data[8] << ", " << Data[9] << "\n";


//		Cap inertias so we can keep fast renormalisation...
//		---------------------------------------------------
#define MAX_DIAG 10//5         //10//5//.25                   //1//200//2//10
		if ( Data[7] > MAX_DIAG ) Data[7] = MAX_DIAG;
		if ( Data[8] > MAX_DIAG ) Data[8] = MAX_DIAG;
		if ( Data[9] > MAX_DIAG ) Data[9] = MAX_DIAG;


//		Now set object parameters...
//		============================
		Data[0] = 1 / M;
		//if (M > 50) M = 50;

		Data[1] = 200*M;// / (float)elements;// / (float)elements;
		//Data[1] = 400*M;// / (float)elements;

		//if (Data[1] > 10000) Data[1] = 10000;
		Data[2] = /*.5*/sqrt( /*2*/M*Data[1] );

//		For now, use slip[0] for all elements...
//		========================================
		if (slip[0] > 100) slip[0] = 100;
		Data[3] = M*slip[0];

		//conPhysics << "RelSlide: " << Data[2] << ", " << Data[3] << "\n";

//		Zero everything else...
		Xin[0][0] = Xin[0][1] = Xin[0][2] =			//BC inputs...
		Xin[1][0] = Xin[1][1] = Xin[1][2] = 0;
		for (ii = 0; ii < 6; ii++) Impulse_Queue[ii] = 0;			//Impulse inputs...

//		Zero the break accumulators...
		int j;
		for (j = 0; j < MAX_ELEMENTS; j++)
			TTotal[j][0] = TTotal[j][1] = TTotal[j][2] = 0;

//		Zero collisions too...
		for (j = 0; j < GUYS; j++)
		{
			BoxCollisions[i_me][j].EnergyMax = BoxCollisions[i_me][j].EnergyMax   = 0;
			BoxCollisions[i_me][j].EnergyHit = BoxCollisions[i_me][j].EnergySlide = 0;
			BoxCollisions[j][i_me].EnergyHit = BoxCollisions[j][i_me].EnergySlide = 0;

//			And reset the element latches...
			BoxCollisions[i_me][j].ElementCollide = BoxCollisions[i_me][j].LastElementCollide = set0;
			BoxCollisions[j][i_me].ElementCollide = BoxCollisions[j][i_me].LastElementCollide = set0;
		}

//		Do the water...
		BoxCollisions[i_me][WATER].EnergyMax = 0;
		BoxCollisions[i_me][WATER].EnergyHit = BoxCollisions[i_me][WATER].EnergySlide = 0;
		BoxCollisions[i_me][WATER].ElementCollide = BoxCollisions[i_me][WATER].LastElementCollide = set0;


//		For terrain, we set all corner flags on, to disable collisions just after an object awakes...
		for (j = TERRAIN; j < TERRAIN+8; j++)
		{
			BoxCollisions[i_me][j].EnergyMax = 0;
			BoxCollisions[i_me][j].EnergyHit = BoxCollisions[i_me][j].EnergySlide = 0;

			BoxCollisions[i_me][j].ElementCollide = set0;
			BoxCollisions[i_me][j].LastElementCollide = -CSet<int>();
		}


//		Am I immobile?
		Movable = movable;

//		Do I float?
		Floats	= b_floats;

//		For external use...
		Moved = 0;
		Bedtime = FLT_MAX;
		SleepThreshold = 1.0;

//		Zero the semaphores...
		for (j = 0; j < GUYS; j++)
			BC_semaphore[j] = Xob[j].BC_semaphore[i_me] = 0;

		if (pelvis_elem == RIGHT_HAND && elements == 1 && movable)
		{
			Data[0] = 1;
			Data[1] = 200;
			Data[2] = /*.5**/sqrt( 200. );
			Data[3] = slip[0];
			Data[7] = Data[8] = Data[9] = 1.;
			//conPhysics << "Hey!, I am a BCBOX...\n";
		}

//		if (   (pelvis_elem == RIGHT_FOOT)
//			|| (pelvis_elem == LEFT_FOOT)
//			|| (pelvis_elem == FRIGHT_FOOT)
//			|| (pelvis_elem == FLEFT_FOOT) )
//		{
//			//Data[3] = 230;
//
//		}

//		Finally, which axes are live?
		Ignorable_DOF[0] = live_DOF[0];
		Ignorable_DOF[1] = live_DOF[1];
		Ignorable_DOF[2] = live_DOF[2];

		Ignorable_DOF[3] = live_DOF[3];
		Ignorable_DOF[4] = live_DOF[4];
		Ignorable_DOF[5] = live_DOF[5];


//		Really, Finally, magnet hinge parameters...
		Data[12] = Data[14] = 0;
		if (Anchored)
		{
			if (hinge_damp == 0) Data[12] = 5;
			else Data[12] = hinge_damp;

			Data[14] = hinge_drive;

		}

//		Inertias...
		Data[15] = 1/Data[7];
		Data[16] = 1/Data[8];
		Data[17] = 1/Data[9];

//		Zero the compound overdefinition test...
		comp_index = 0;

//		Print out the parameter set...
//		------------------------------
#define NIOSY
#ifdef NIOSY		
		conPhysics << "Created id = " << i_me << ", elements = " << elements << "\n";

float	temp[3];
		StuffVec( SuperData[0][3], SuperData[0][4], SuperData[0][5], temp );
		Back( temp[0], temp[1], temp[2] );

		temp[0] += State[0][0];
		temp[1] += State[1][0];
		temp[2] += State[2][0];


		conPhysics << "0: " << temp[0] << ", " << temp[1] << ", " << temp[2] << "\n";

		conPhysics << "Data:  ----------------\n";
		for (index = 0; index < 12; index++ ) {
			conPhysics << index << ": " << Data[index] << "\n";
		}
		conPhysics << "Orientation:\n";
		for (index = 0; index < 3; index++)
		{
			conPhysics << orient[0][index][0] << " : " << orient[0][index][1] << " : " << orient[0][index][2] << "\n";
		}
		conPhysics << "+++++++++\n";
#endif


		// Last, update the initial energy.
		fInitialEnergy = fGetKineticEnergy();

//	Coooooool...
//	------------
}

inline void CXob::IntegrateInit()
{
//	0.) Zero out some variables...
//	------------------------------

//	Zero the external influence...
//	------------------------------
	Xin[0][0] = Xin[0][1] = Xin[0][2] = 
	Xin[1][0] = Xin[1][1] = Xin[1][2] = 0; 

//	Zero a stupid flag for anne knowing she can walk on objects...
//	--------------------------------------------------------------
	bHitAnother = false;

//	Zero the break accumulators...
//	------------------------------
int	j;
	for (j = 0; j < MAX_ELEMENTS; j++) 
		TTotal[j][0] = TTotal[j][1] = TTotal[j][2] = 0;

	for (j = 0; j < GUYS; j++)
		BC_semaphore[j] = -100;

	comp_index = 0;

//	YIKES!
	OKtoJUMP = false;
}

inline void CXob::ToBio()
{
//	2.) Run Pelvis B/C with coupling from hands...
//	----------------------------------------------	
	if (Info > 0 && PelvisModel >= 0)
	{
		Assert(PelvisElem >= 0);
		for ( int ii = 0; ii < 7; ii++ )		//Copy the Box BC...
		{
			Pel[(PelvisModel)][(PelvisElem+ii)][0] = State[ii][0];
			Pel[(PelvisModel)][(PelvisElem+ii)][1] = State[ii][1];
		}


		BioTag[(PelvisModel)][(PelvisElem+0)] = Data[0]*( Xin[0][0] + Impulse_Queue[0] ) + State[0][2];
		BioTag[(PelvisModel)][(PelvisElem+1)] = Data[0]*( Xin[0][1] + Impulse_Queue[1] ) + State[1][2];
		BioTag[(PelvisModel)][(PelvisElem+2)] = Data[0]*( Xin[0][2] + Impulse_Queue[2] ) + State[2][2] + 10;
		State[2][2] += 10;

		BioTag[(PelvisModel)][(PelvisElem+3)] = Data[7]*( Xin[1][0] + Impulse_Queue[3] ) + Tau[0];
		BioTag[(PelvisModel)][(PelvisElem+4)] = Data[8]*( Xin[1][1] + Impulse_Queue[4] ) + Tau[1];
		BioTag[(PelvisModel)][(PelvisElem+5)] = Data[9]*( Xin[1][2] + Impulse_Queue[5] ) + Tau[2];
	}
}

inline void CXob::IntegrateBox( float delta_t )
{
	if (Info > 0)
	{
		State[0][2] += Data[0]*(Xin[0][0] + Impulse_Queue[0]);	//Include collision info...
		State[1][2] += Data[0]*(Xin[0][1] + Impulse_Queue[1]);
		State[2][2] += Data[0]*(Xin[0][2] + Impulse_Queue[2]);

		TTotal[0][0] += 1000*Impulse_Queue[3];
		TTotal[0][1] += 1000*Impulse_Queue[4];
		TTotal[0][2] += 1000*Impulse_Queue[5];

		if (!Ignorable_DOF[0]) State[0][2] = 0;
		if (!Ignorable_DOF[1]) State[1][2] = 0;
		if (!Ignorable_DOF[2]) State[2][2] = 0;

float	a0 = State[3][0],
		a1 = State[4][0],
		a2 = State[5][0],
		a3 = State[6][0];

float	atemp = Data[7]*(Xin[1][0] + Impulse_Queue[3]),
		btemp = Data[8]*(Xin[1][1] + Impulse_Queue[4]),
		gtemp = Data[9]*(Xin[1][2] + Impulse_Queue[5]);

		Impulse_Queue[0] = Impulse_Queue[1] = Impulse_Queue[2] = 
		Impulse_Queue[3] = Impulse_Queue[4] = Impulse_Queue[5] = 0;

//				For all sorts of stuff...
		float	d0 = State[3][1],
				d1 = State[4][1],
				d2 = State[5][1],
				d3 = State[6][1];

		float	ad = 2*( a0*d1 + a3*d2 - a2*d3 - a1*d0 ),
				bd = 2*(-a3*d1 + a0*d2 + a1*d3 - a2*d0 ),
				gd = 2*( a2*d1 - a1*d2 + a0*d3 - a3*d0 );

		if (Anchored)
		{
			State[0][2] = State[1][2] = State[2][2] = 0;

			//Data[14] = 3 / Data[9];
			//Data[12] = 30;

			if ( fabs(Data[14]) > 0 )
			{
				gtemp += Data[9]*Data[14];
			} 

			//if (spring)
			//
			
			//Gravity Torque...
			if ( Data[19] > 1.0 )
			{

				//conPhysics << "Mass as reported " << Data[19] << "for " << i << ".\n";	
				float gtau[3] = { 0, 0, -5},//-10 ,
					  mom[3]  = { SuperData[0][3],
								  SuperData[0][4],
								  SuperData[0][5] },
					  res[3]  = {0,0,0};

				Make_Transform();
				From( gtau[0], gtau[1], gtau[2] );
				Outer( mom, gtau, res );
				atemp += Data[7]*res[0];
				btemp += Data[8]*res[1];
				gtemp += Data[9]*res[2];
			}


			float damper = Data[12];//.05;//.75;//.1;//.75;
			atemp -= Data[7]*damper*ad;
			btemp -= Data[8]*damper*bd;
			gtemp -= Data[9]*damper*gd;
			

		}	//End of anchored test...

		//atemp = btemp = gtemp = 0;

		if (!Ignorable_DOF[3]) atemp = 0;//Deal with ignorable coords...
		if (!Ignorable_DOF[4]) btemp = 0;
		if (!Ignorable_DOF[5]) gtemp = 0;

		State[3][2] +=-.5*( a1*atemp + a2*btemp + a3*gtemp );
		State[4][2] += .5*( a0*atemp - a3*btemp + a2*gtemp );
		State[5][2] += .5*( a3*atemp + a0*btemp - a1*gtemp );
		State[6][2] += .5*(-a2*atemp + a1*btemp + a0*gtemp );


		if (Movable)							//Integrate everything that can move...
		{
			if (PelvisModel >= 0 && GetBioHealth(PelvisModel) > 0.0f)
			{
				// Never sleep living bio-elements.
				Bedtime = 10000;
			}
			else
			{
				// Calculate a bedtime parameter.
				Bedtime = SleepL1*( State[0][1]*State[0][1]
								+ State[1][1]*State[1][1]
								+ State[2][1]*State[2][1] );

				Bedtime += SleepL2*( State[0][2]*State[0][2]
								   + State[1][2]*State[1][2] );

				if (PelvisModel < 0)
					// Count vertical accleration only for non-bio elements.
					// Dead dino box sleep should skip this, as at this point in the integration,
					// it's not correct.
					Bedtime += SleepL2*( State[2][2]*State[2][2] );

				Bedtime += SleepA1*( ad*ad + bd*bd + gd*gd );
				if (Anchored)
					Bedtime += SleepA2*(atemp*atemp + btemp*btemp + gtemp*gtemp);
			}

			if ( Bedtime >= PHY_sleep * SleepThreshold )
			{
				for (int j = 0; j < 7; j++)
				{
					State[j][0] += delta_t*( State[j][1] );
					State[j][1] += delta_t*State[j][2];
				}

				// Reassert this box's full activeness.
				Info = INFO_MAX;

				// Mark it as moved this frame, for external checking.
				Moved |= 1;
			}
			else
			{
				// Put sleepyheads to sleep. Joe will keep bio-boxes awake for us, if required.
				if (NPhysImport::bAllowSleep())
					// Give it several steps before going to sleep,
					// so we ensure object is really resting.
					Info--;
			}

			a0 = State[3][0];				//Grab stuff for renormalization...
			a1 = State[4][0];
			a2 = State[5][0];
			a3 = State[6][0];

float			L = 1 - ( a0*a0 + a1*a1 + a2*a2 + a3*a3 );

float			Lorig = L;
//const float	lag_mult = .01;
const float		lag_mult = .1;
const float		lag_max = .003;//.001;
int count = 0;
			while (1) {
				if (!std::isfinite(L))
				{
					dout << "FINITY BAILOUT IN XOB_BC FOR L: " << L << std::endl;
					//This is an incomplete workaround
					
					//Set to valid, normalised vector
					State[3][0] = 0.25f;
					State[4][0] = 0.25f;
					State[5][0] = 0.25f;
					State[6][0] = 0.25f;
					
					break;
				}

				if ( fabs(L) < lag_max ) break;

				count++;

				a0 = State[3][0];
				a1 = State[4][0];
				a2 = State[5][0];
				a3 = State[6][0];

				//float m = sqrt( a0*a0 + a1*a1 + a2*a2 + a3*a3 );
				//State[3][0] /= m;
				//State[4][0] /= m;
				//State[5][0] /= m;
				//State[6][0] /= m;

				State[3][0] += lag_mult*a0*L;
				State[4][0] += lag_mult*a1*L;
				State[5][0] += lag_mult*a2*L;
				State[6][0] += lag_mult*a3*L;

				L = 1 - ( a0*a0 + a1*a1 + a2*a2 + a3*a3 );

			}	//End of renorm loop...

		//conPhysics << i << "'s L: " << Lorig << " with " << count << "iterations, now = " << L << ".\n";
		//conPhysics << "C: " << count << "\n";

		}	//End of moveable object loop...
		else
		{
			for (int tt = 0; tt < 3; tt++ ) State[tt][1] = State[tt][2] = 0;	//You should now sleep if you're not touched...

			// Start deactivating immovable objects.
			if (NPhysImport::bAllowSleep())
				// Give it several steps in which to retire.  
				Info--;
		}

	}	//End of integration loop...
	else if (Instances[0])
	{
		// Continue the countdown on sleepy objects.
		if (NPhysImport::bAllowSleep())
			Info--;
	}
}


extern	void Pelvis_integrate( float a, float b[6] );			//Now, finally, the pelvises...

inline void CXob::FromBio()
{
//	4.) Copy pelvis biobox state to box...
//	----------------------------------
	if (Info > 0 && PelvisModel >= 0)
	{
		Assert(PelvisElem >= 0);
		for ( int ii = 0; ii < 7; ii++ )				//Copy the Box BC...
		{
			State[ii][0] = Pel[ PelvisModel ][ PelvisElem+ii ][0];
			State[ii][1] = Pel[ PelvisModel ][ PelvisElem+ii ][1];
		}
	}
}


inline void CXob::IntegrateFinish()
{
//	Now check for bookKeeping stuff...

	if (Info)
	{
		// Check for breakage...
		for (int super = 0; super < Data[11]; super++)
		{
			//conPhysics << "Brakages[super]: " << Breakages[super] << "\n";
			if (Breakages[super] < FLT_MAX)
			{

				float mag = TTotal[super][0]*TTotal[super][0]
						  + TTotal[super][1]*TTotal[super][1]
						  + TTotal[super][2]*TTotal[super][2];
				conPhysics << "TT: " << mag << "\n";

				if ( mag > fBreakage && mag > Breakages[super]) 
				{
					//
					// Break the instance for this element from its master.
					// If the element is 0, it has no masters, so break from all its slaves.
					//
					NPhysImport::RemoveMagnets(Instances[super], true, super == 0);
				}//Break test itself...
			}//Check active...
		}//Loop over elements...

		if (PelvisElem == 0)
		{
			// Perform bookkeeping on pelvic models.
			CheckPelvis();
		}
	}
}

//	Integrate with Lagrange constraints...
//	--------------------------------------
void	CXob::PD_Integrator_Internals( float delta_t ) 
{
		CCycleTimer	ctmr;

int		i;

//		-1). Wake up everybody near current objs.
		NPhysImport::ActivateIntersecting();

//		0.) Zero out some variables...
//		------------------------------
		for (i = 0; i < GUYS; i++) 
			Xob[i].IntegrateInit();


//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		ctmr.Reset();

//		1.) Run Box model B/C, get XIN for hands...
//		-------------------------------------------
int		i_num_active_obj = 0;							//Count the total number of active objects.

		bPhysicsIntegrating = true;
		for (i = 0; (i < GUYS); i++) 
		{
			if (Xob[i].Info > 0)
			{
				current_timeslice = delta_t;
				Xob[i].pipe_quad_BC(); 
				i_num_active_obj++;
			}
		}
		bPhysicsIntegrating = false;
		//conPhysics << "with: " << i_num_active_obj << "\n";


//	Compound Object Fix...
	for (i = 0; (i < GUYS); i++) 
	{
		if ( (Xob[i].Data[11] > 1) && ( Xob[i].PelvisElem != RIGHT_HAND) && !Xob[i].Anchored )
		{
			if (Xob[i].Info > 0)
			{

float		mag0 = sqrt(Xob[i].Xin[0][0]*Xob[i].Xin[0][0] + Xob[i].Xin[0][1]*Xob[i].Xin[0][1] + Xob[i].Xin[0][2]*Xob[i].Xin[0][2]);
float		mag1 = sqrt(Xob[i].Xin[1][0]*Xob[i].Xin[1][0] + Xob[i].Xin[1][1]*Xob[i].Xin[1][1] + Xob[i].Xin[1][2]*Xob[i].Xin[1][2]);

float		max0 = 25 / Xob[i].Data[0];
float		max1 = 50;//00;
			//conPhysics << "Mag1: " << mag1 << "\n";


				if ( mag0 > max0 )
				{
					float mul = max0 / mag0;
					Xob[i].Xin[0][0] *= mul;
					Xob[i].Xin[0][1] *= mul;
					Xob[i].Xin[0][2] *= mul;

					//conPhysics << "LinearOuch!\n";
				}
				if ( mag1 > max1 )
				{
					float mul = max1 / mag1;
					Xob[i].Xin[1][0] *= mul;
					Xob[i].Xin[1][1] *= mul;
					Xob[i].Xin[1][2] *= mul;

					//conPhysics << "TorqueOuch!\n";
				}
			}
		} 
	}

		psBoundary.Add(ctmr(), i_num_active_obj);


		//conPhysics << "Objects Running: " << i_num_active_obj << "\n";

//		2.) Run Pelvis B/C with coupling from hands...
//		----------------------------------------------	
		for (i = 0; i < GUYS; i++)
			if (Xob[i].Info > 0 && Xob[i].PelvisModel >= 0)
				Xob[i].ToBio();

//		3.) integrate both. (do NOT integrate hand box??)...
//		----------------------------------------------------
		for (i = 0; (i < GUYS); i++)												//Now the true integration...
			Xob[i].IntegrateBox(delta_t);

		psIntBox.Add(ctmr(), 1);


//extern	void Pelvis_integrate( float a, float b[6] );			//Now, finally, the pelvises...
float	inputs[6] = {0,0,0, 0,0,0};
		Pelvis_integrate( delta_t, inputs );					//Do what Nike says...

//		4.) Copy pelvis biobox state to box...
//		----------------------------------
		for (i = 0; i < GUYS; i++)
			if (Xob[i].Info > 0 && Xob[i].PelvisModel >= 0)
				Xob[i].FromBio();

		psIntPelvis.Add(ctmr(), 1);


//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//	Now check for bookKeeping stuff...

//		5.) Finish up...
//		----------------------------------
		for (i = 0; i < GUYS; i++)
			if (Xob[i].Info)
				Xob[i].IntegrateFinish();

		psIntFinish.Add(ctmr(), 1);
}



extern CProfileStat psCollisionMsg;



float	MAX_TIMESTEP = .01;//.008
#define TIMESTORE_PCT .25
float	timestore = 0;
float	fLastFrame = .1;

//	Integrate...
//	============
void	CXob::PD_Integrate( float timestep )
{
	CCycleTimer ctmr;

	//conPhysics << "Actual: " << timestep << ", Last: " << fLastFrame;

//	Stupid asymptotic filter that fails all stability citeria but might still work!
/*	if (timestep > fLastFrame)
	{
		float temp = fLastFrame;
		fLastFrame += .5*(timestep - fLastFrame);
		timestep = temp;
	}
	else fLastFrame = timestep;

	conPhysics << ", and as passed: " << timestep << "\n";
*/

	timestep += timestore;	//For repeatability and speed...
	timestore = 0.0f;

//	Divide into chunks of maximum stable deltas...
	while ( timestep > MAX_TIMESTEP ) 
	{
		timestep -= MAX_TIMESTEP;
		PD_Integrator_Internals( MAX_TIMESTEP );
	}

//	Now get the rest...
	if (timestep > TIMESTORE_PCT*MAX_TIMESTEP /*.001*/ )
	{
		PD_Integrator_Internals( timestep );
	}
	else
	{
		timestore = timestep;
		//conPhysics << "TimeStore active...\n";
	}

	psIntegrate.Add(ctmr());

	// Frame latching allows two boxes to only make a collision sound once per frame rather than
	// once per integration step.

	for (int me = 0; me < GUYS; me++) 
	if (Xob[me].Info > 0)
	{
		int you;

		// Sum max terrain collisions for each corner into element 0.
		for (you = TERRAIN+1; you < TERRAIN+8; you++)
		{
			BoxCollisions[me][TERRAIN].EnergyMax += BoxCollisions[me][you].EnergyMax;
			BoxCollisions[me][you].EnergyMax = 0;
		}

		// Send collision messages for any found.
		for (you = 0; you < COLLISIONS; you++)
		{
			SCollision* pcoll = &BoxCollisions[me][you];

			// Check for minimum significance.
			if (pcoll->EnergyMax < Collide_min_hit)
				pcoll->EnergyMax = 0;
			if (pcoll->EnergyHit < Collide_min_hit)
				pcoll->EnergyHit = 0;
			if (pcoll->EnergySlide < Collide_min_slide)
				pcoll->EnergySlide = 0;
				
			if (pcoll->EnergyMax + pcoll->EnergyHit + pcoll->EnergySlide > 0)
			{
				// These boxes collided or slid this frame. Send a message.

				//
				// Compute collision location as weighted centre point between objects.
				// Since individual point collisions are combined into this event anyway, 
				// there's no point is specifying a more precise location.
				//

				float loc[3];
				if (you < GUYS)
				{
					// Point between objects, weighted by their radii.
					float me_frac = Xob[you].Data[10] / (Xob[you].Data[10] + Xob[me].Data[10]);
					for (int ii = 0; ii < 3; ii++)
						loc[ii] = Xob[me].State[ii][0] * me_frac + Xob[you].State[ii][0] * (1.0 - me_frac);
				}
				else 
				{
					// For terrain or water collisions, we already have the correct positions.
					loc[0] = pcoll->Location[0];
					loc[1] = pcoll->Location[1];
					loc[2] = pcoll->Location[2];

					// Terrain collisions were summed scaled by hit energy, 
					// so to produce average collision, we must divide by total energy.
					if (you == TERRAIN && pcoll->EnergyHit)
					{
						loc[0] /= pcoll->EnergyHit;
						loc[1] /= pcoll->EnergyHit;
						loc[2] /= pcoll->EnergyHit;

						Assert(loc[0] != 0.0f || loc[1] != 0.0f || loc[2] != 0.0f);
					}
				}

				psIntegrate.Add(ctmr());

				// Scale energy to correspond to original masses.
				float mass_adjust;
				if (you >= GUYS || !Xob[you].Movable)
					mass_adjust = Xob[me].Data[19] * Xob[me].Data[0];
				else if (!Xob[me].Movable)
					mass_adjust = Xob[you].Data[19] * Xob[you].Data[0];
				else
					mass_adjust = (Xob[me].Data[19] * Xob[you].Data[19]) /
								  (Xob[me].Data[19] + Xob[you].Data[19]) *
								  (Xob[me].Data[0]  + Xob[you].Data[0]);


				NPhysImport::SendCollision
				(
					Xob[me].Instances[pcoll->Element1], 

					// If box number is TERRAIN, it's the terrain.
					(you == WATER ? WaterCollide[me] : 
					 you == TERRAIN ? NPhysImport::pinsTerrain() : 
					 Xob[you].Instances[pcoll->Element2]),

					Xob[me].Sound[pcoll->Element1],

					// If terrain, use box function to retrieve sound material.
					// If water, specify no sound so it will be inferred.
					(you < GUYS ? Xob[you].Sound[pcoll->Element2] :
					 you == TERRAIN ? Xob[me].iTerrainSound(loc[0], loc[1]) :
					 0),

					loc,

					pcoll->EnergyMax	* mass_adjust,
					pcoll->EnergyHit	* mass_adjust,
					pcoll->EnergySlide	* mass_adjust,
					Xob[me].fGetKineticEnergy() - Xob[me].fInitialEnergy,

					(you < GUYS ?
						Xob[you].fGetKineticEnergy() - Xob[you].fInitialEnergy :
						0.0f)
				);

				psCollisionMsg.Add(ctmr(), 1);
			}
		
			// Copy flags for next time, to enable frame latching.
			pcoll->LastElementCollide = pcoll->ElementCollide;
			pcoll->ElementCollide = set0;

			// Also reset velocities and location.
			pcoll->EnergyMax = pcoll->EnergyHit = pcoll->EnergySlide = 0;
			pcoll->Location[0] = pcoll->Location[1] = pcoll->Location[2] = 0;
		}
	}

	// Reset the latch bits for terrain/anne interactions.
	anne_last_terrain_bit = anne_this_terrain_bit;
	anne_this_terrain_bit = CSet<uint32>(0,0);

	psIntegrate.Add(ctmr());
}


float detonate( float a[3][3] )
{

	return -a[0][2]*a[1][1]*a[2][0]
		  + a[0][1]*a[1][2]*a[2][0]
		  + a[0][2]*a[1][0]*a[2][1]
		  - a[0][0]*a[1][2]*a[2][1]
		  - a[0][1]*a[1][0]*a[2][2]
		  + a[0][0]*a[1][1]*a[2][2];



}


//	Make the transform for object and store in index...
//	---------------------------------------------------
void CXob::Make_Transform()
{

float	e0 = State[3][0],
		e1 = State[4][0],
		e2 = State[5][0],
		e3 = State[6][0];

		Transform[0] = ( e0*e0 + e1*e1 - e2*e2 - e3*e3 );
		Transform[1] = 2*( e1*e2 + e0*e3 );
		Transform[2] = 2*( e1*e3 - e0*e2 );

		Transform[3] = 2*( e1*e2 - e0*e3 );
		Transform[4] = ( e0*e0 - e1*e1 + e2*e2 - e3*e3 );
		Transform[5] = 2*( e2*e3 + e0*e1 );

		Transform[6] = 2*( e0*e2 + e1*e3 );
		Transform[7] = 2*( e2*e3 - e0*e1 );
		Transform[8] = ( e0*e0 - e1*e1 - e2*e2 + e3*e3 );

}

void CXob::Assert_Transform() const
{

float	e0 = State[3][0],
		e1 = State[4][0],
		e2 = State[5][0],
		e3 = State[6][0];

float	transform[9];

		transform[0] = ( e0*e0 + e1*e1 - e2*e2 - e3*e3 );
		transform[1] = 2*( e1*e2 + e0*e3 );
		transform[2] = 2*( e1*e3 - e0*e2 );

		transform[3] = 2*( e1*e2 - e0*e3 );
		transform[4] = ( e0*e0 - e1*e1 + e2*e2 - e3*e3 );
		transform[5] = 2*( e2*e3 + e0*e1 );

		transform[6] = 2*( e0*e2 + e1*e3 );
		transform[7] = 2*( e2*e3 - e0*e1 );
		transform[8] = ( e0*e0 - e1*e1 - e2*e2 + e3*e3 );

		for (int i = 0; i < 9; i++)
			Assert(transform[i] == Transform[i]);
}

//	Apply an impulse to model M, at world coordinate W[3], with vector strength V[3]...
//	--------------------------------------------------------------------------------------------------
void CXob::ApplyImpulse( float W[3], float V[3] )
{
	int type = Sound[0];
	float mul = 1;

	V[0] *= mul/MAX_TIMESTEP;			//Convert to force for 1 timestep.
	V[1] *= mul/MAX_TIMESTEP;
	V[2] *= mul/MAX_TIMESTEP;

	Impulse_Queue[0] = V[0];			//Total impulse in world...
	Impulse_Queue[1] = V[1];
	Impulse_Queue[2] = V[2];

	W[0] -= State[0][0];				//Ready to become LOCAL...
	W[1] -= State[1][0];
	W[2] -= State[2][0];

	Make_Transform();	//Get local...
	From( W[0], W[1], W[2] );
	From( V[0], V[1], V[2] );

	Impulse_Queue[3] = W[1]*V[2] - W[2]*V[1];
	Impulse_Queue[4] = W[2]*V[0] - W[0]*V[2];
	Impulse_Queue[5] = W[0]*V[1] - W[1]*V[0];
}

void CXob::Reset()
{
	// Clear out important elements.
	for (int me = 0; me < GUYS; me++)
	{
		memset(Xob[me].Instances, 0, sizeof(Xob[me].Instances));
		Xob[me].Info = 0;

		// For good measure.
		memset(Xob[me].Tau, 0, sizeof(Xob[me].Tau));

		Xob[me].PelvisModel = Xob[me].PelvisElem = -1;
	}

	// Absolutely must reset this. Fiend!
	timestore = 0;

	// May not need to reset the following.
	current_timeslice = 0;
	OurKappa = OurDelta = OurMu = 0;
}

#include "Lib/Std/ArrayIO.hpp"

std::ostream& operator <<(std::ostream& os, const SCollision& coll)
{
	os	<<(int)coll.ElementCollide
		<<" " <<(int)coll.LastElementCollide
		<<" " <<coll.EnergyMax
		<<" " <<coll.EnergyHit
		<<" " <<coll.EnergySlide;

	return os;
}

void CXob::DumpState(std::ostream& os)
{
#if VER_TEST
	if (Instances[0])
	{
		int j, k, l;

		// Superbox data.
		os	<< "Box " << Index()
			<< " Elems=" << Data[11]
			<< " Moved=" << Moved << " bHitAnother=" << bHitAnother
			<< " Pelvis=" << PelvisModel << ' ' << PelvisElem
			<< std::endl;

		os	<< "  Info=" << Info
			<< " Movable=" << Movable
			<< " Anchored=" << Anchored
			<< " Ignorable_DOF=" << Ignorable_DOF[0] << ' ' << Ignorable_DOF[1] << ' ' << Ignorable_DOF[2]
			<< std::endl;

		os	<< "  State=";
		for (j = 0; j < 3; j++)
		{
			os << "( ";
			for (k = 0; k < 7; k++)
				os	<< double(State[k][j]) << ' ';
			os << ") ";
		}
		os	<< std::endl;

		os	<< "  Data=" << CPArray<float>(20, Data) << std::endl;

		os	<< "  Extents=" << CPArray<float>(6, Extents)
			<< "Radius=" <<Radius <<" ExtentRatio=" <<ExtentRatio
			<< std::endl;

		os	<< "  Tau=" << CPArray<float>(3, Tau) << std::endl;

		os	<< "  Xin=";
		for (j = 0; j < 2; j++)
			for (k = 0; k < 3; k++)
				os << Xin[j][k] << ' ';
		os	<< std::endl;

		os	<< "  Impulse=" << CPArray<float>(6, Impulse_Queue) << std::endl;

		os	<< "  BC_semaphore=" << CPArray<int8>(GUYS, BC_semaphore) << std::endl;

		for (j = 0; j < COLLISIONS; j++)
			if ((int)BoxCollisions[Index()][j].ElementCollide || (int)BoxCollisions[Index()][j].LastElementCollide)
			os	<<"  Coll " <<j <<": " <<BoxCollisions[Index()][j] << std::endl;
		os	<< std::endl;

		// Element data.
		for (j = 0; Instances[j]; j++)
		{
			os	<< std::endl;
			os	<< "  Elem " << j;
			if (Instances[j])
				os	<< " '" << strInstanceName(j) << "'";
			os	<< std::endl;

			os	<< "    SuperData=";
			for (k = 0; k < 6; k++)
				os	<< SuperData[j][k] << ' ';
			os	<< std::endl;

			os	<< "    SuperOrient=";
			for (k = 0; k < 3; ++k)
			{
				os	<< "( ";
				for (l = 0; l < 3; ++l)
					os	<< SuperOrient[j][k][l] << ' ';
				os	<< ")";
			}
			os	<< std::endl;

			os	<< "    Breakage=" << Breakages[j]
				<< " TTotal=" << TTotal[j][0] << ' ' << TTotal[j][1] << ' ' << TTotal[j][2]
				<< std::endl;
		}

		os	<< std::endl;
	}
#endif
}

void CXob::DumpStateAll(std::ostream& os)
{
#if VER_TEST
	int i;

	//
	// Boxes.
	//

	os	<<"Last time=" <<current_timeslice << std::endl;

	// Active box data
	for (i = 0; i < iMAX_PHYSICS_OBJECTS; ++i)
		Xob[i].DumpState(os);

	os	<<"OurKappa=" <<OurKappa
		<<" OurDelta=" <<OurDelta
		<<" OurMu=" <<OurMu
		<< std::endl;
	os	<<"anne_this_terrain_bit=" <<(int)anne_this_terrain_bit
		<<" anne_last_terrain_bit=" <<(int)anne_last_terrain_bit
		<< std::endl << std::endl;

	os	<< std::endl;
#endif
}

//	What does it mean?
//	==================
//	0	-	1/mass
//	1	-	surface_kappa
//	2	-	surface_delta
//	3	-	friction;
//	4	-	X/2
//	5	-	Y/2
//	6	-	Z/2
//	7-9	-	1/Ia, 1/Ib, 1/Ig.
//	10	-	biggest dimension,
//	11  -   number of sooperboxes
//	12	-	damp for magnets...
//	13	-	spring for magnets...
//	14	-	motor strength for magnets...
//	15-17 - inertias...





