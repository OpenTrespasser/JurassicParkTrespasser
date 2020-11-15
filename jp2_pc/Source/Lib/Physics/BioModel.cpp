//	Everything needed for the constrained Field Bio model system.
//	-------------------------------------------------------------

//	NOTE: ML was right, look, it all fits into one file.

#include "common.hpp"
#include "BioModel.h"
#include "futil.h"
#include "PhysicsImport.hpp"
#include "PhysicsStats.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "AsmSupport.hpp"
#include "Lib/Math/FastSqrt.hpp"

#include <math.h>
#include <iostream>
#include <memory.h>
 
//	State...
//	--------

aptr<CBioModel>	BioModels[MAX_BIOMODELS];

//	-------------------------
//	Caution: Physics below...
//	-------------------------



//	Field parameters...
//	-------------------
////const float	kappa = 100,//50,
////			density = 1,
////			delta = .5*sqrt(2*density*kappa),
////			mass_scale = 1/density,
////			field_g = 10,
////			
////			BC_couple = 1000,
////			BC_damp = sqrt(2*density*BC_couple),
////			BC_coarseness = .05,
////			surface_mu = 50;//5;

//const float	kappa = 2000,//50,
//			density = 1,
//			//delta = 1.25*sqrt(2*density*kappa),
//			delta = 1.15*sqrt(2*density*kappa),
//			mass_scale = 1/density,
//			field_g = 0,//10,
//			
//			BC_couple = 1000,
//			BC_damp = sqrt(2*density*BC_couple),
//			BC_coarseness = .05,
//			surface_mu = 50;//5;

	 float	kappa = 2000,//500,//2000,        //50,
			density = 1,
			delta = 1.25*sqrt(2*density*kappa),
			mass_scale = 1/density,
			field_g = 0;//10,
			
//			BC_couple = 1000,
//			BC_damp = sqrt(2*density*BC_couple),
//			BC_coarseness = .05,
//			surface_mu = 50;//5;

//**********************************************************************************************
//
// CBioModel implementation.
//


//	 Internals of control...
//	 -----------------------
void CBioModel::UpdateControl()
{

#define STRENGTH 130*130//150*150                     //100*100//60*60

	if ( Control[4] > -1)
	{

		int guy = (int)Control[4];
		if ( Pel_Data[guy][49] > 0 )
		{

int			CP = Dof - 2;	//The last element Z...

float		target[3] = { Control[1], Control[2], Control[3] };

float		pos[3] = { Field[CP][0][0], Field[CP][1][0], Field[CP][2][0] },
			vel[3] = { Field[CP][0][1], Field[CP][1][1], Field[CP][2][1] };

			float ass[3];
			Global_Vel( ass, target, BODY, guy );
			vel[0] -= ass[0];
			vel[1] -= ass[1];
			vel[2] -= ass[2];
			float tempZ = target[2];
			Local_to_World_Trans( target, guy );	//Is there a God, or a pelvis?

//			For CONFIG TEST...
			if ( (Pel_Usage[guy] == epelRAPTOR) )
			{
				float spd = Pel[guy][0][1]*Pel[guy][0][1] + Pel[guy][1][1]*Pel[guy][1][1];

				if ( (Control[0] < 100) && (spd < 4*Pel_Data[guy][14]) )
				{
					float targ = NPhysImport::fPelTerrainHeight( guy, Pel[guy][0][0], Pel[guy][1][0] );
					if ( fabs(targ - Pel[guy][2][0]) < 1.25*Pel_Data[guy][14] )
					{
						target[2] = targ + .85*Pel_Data[guy][14];
						if (Pel_Data[guy][14]<3.0) target[2] += .5*Pel_Data[guy][14];
					}
				}
			}

			float mass = 2*density,
				  speed = .1,//.25,
				  max_squared = STRENGTH;

			float	R[3];

			Mass_Solution( target, pos, vel, mass, speed, max_squared, R );

			Field[CP][0][2] += Pel_Data[guy][48]*R[0];
			Field[CP][1][2] += Pel_Data[guy][48]*R[1];
			Field[CP][2][2] += 2*Pel_Data[guy][48]*R[2];

		}//Death check...
		
	}//	Bio check...

}


//	Integrator...
//	-------------
void CBioModel::Integrate( float delta_t ) 
{
	CTimeBlock tmb(&psIntBio);

int		i = 0,
		j = 0,
		active = 0;

//	Control...
//	----------
	//if (Control[0] > 0) UpdateControl();
extern bool bIsLimp[NUM_PELVISES];
	if ( !bIsLimp[ (int)Control[4] ] ) { if (Control[0] > 0){ UpdateControl(); field_g = 0;} }
	else field_g = 5;

	for (i = 0; i < Dof; i++) apply_bc_and_locate_field_tracers( i );

	for (i = 3; i < Dof; i++)
	{
		Field[i][0][2] -= .2*(Field[i][0][1]);// - Pel[Pelvis][0][1]);
		Field[i][1][2] -= .2*(Field[i][1][1]);// - Pel[Pelvis][1][1]);
		Field[i][2][2] -= .2*(Field[i][2][1]);// - Pel[Pelvis][2][1]);

		Field[i][0][2] *= 1;//Psi[i][0];
		Field[i][1][2] *= 1;//Psi[i][0];
		Field[i][2][2]  = /*Psi[i][0]*/Field[i][2][2] - field_g;

		for (j = 0; j < 3; j++)
		{
			Field[i][j][2] += FieldInputs[i][j];

			Field[i][j][0] += delta_t*Field[i][j][1];
			Field[i][j][1] += delta_t*Field[i][j][2];
			Field[i][j][2] = 0;

			//conPhysics << "FI: " << FieldInputs[i][j] << "\n";

			//FieldInputs[i][j] = 0;
		}
	}
}

void	integrate_field( float delta_t ) 
{
//	Loop over all the possible models...
	int active = 0;
	for( int model = 0; model < MAX_BIOMODELS; model++ )
	{
		if (BioModels[model])
		{
			active++;
			BioModels[model]->Integrate(delta_t);
		}
	}
	//conPhysics << "Active Biomodels: " << active << "\n";
}



//	The EQM...
//	----------
void	CBioModel::apply_bc_and_locate_field_tracers( int coord ) 
{
//	Working men...
//	--------------
int		index = 0,
		connect = 0;

float	mu[3],
		nu[3],
		hu[3];
float	scratch,
		rho,
		temp;

//	FSC...
//	------
	while ( (connect = Tensor[coord][index]) > -1 ) {

		mu[0] = Field[coord][0][0] - Field[connect][0][0];			//Zeroth...
		mu[1] = Field[coord][1][0] - Field[connect][1][0];
		mu[2] = Field[coord][2][0] - Field[connect][2][0];

		nu[0] = Field[coord][0][1] - Field[connect][0][1];			//First...
		nu[1] = Field[coord][1][1] - Field[connect][1][1];
		nu[2] = Field[coord][2][1] - Field[connect][2][1];
		
		scratch = sqrt( mu[0]*mu[0] + mu[1]*mu[1] + mu[2]*mu[2] );					//Nonzero always...		

		rho = Rho[coord][index] - scratch;

#define SAFETY .35
//		if (rho > SAFETY*Rho[coord][index]) rho = SAFETY*Rho[coord][index];
//		if (rho <-SAFETY*Rho[coord][index]) rho =-SAFETY*Rho[coord][index];

		scratch = 1/scratch;
		hu[0] = mu[0]*scratch;
		hu[1] = mu[1]*scratch;
		hu[2] = mu[2]*scratch;

		scratch = delta*( nu[0]*hu[0] + nu[1]*hu[1] + nu[2]*hu[2] );				//Projectoid...
		temp = kappa*rho - scratch;

		Field[coord][0][2]   += temp*hu[0];
		Field[coord][1][2]   += temp*hu[1];
		Field[coord][2][2]   += temp*hu[2];

		Field[connect][0][2] -= temp*hu[0];
		Field[connect][1][2] -= temp*hu[1];
		Field[connect][2][2] -= temp*hu[2];

		index += 1;
	}

//	Done...
//	-------

}





//	What it looks like...
//	---------------------
inline void BioNorm( float in[3] )
{
	float inv = fInvSqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]);
	in[0] *= inv;	in[1] *= inv;	in[2] *= inv;
}


//	Also does what it looks like it does...
//	---------------------------------------
inline void BioOut( float A[3], float B[3], float C[3] )
{
	C[0] = A[1]*B[2] - A[2]*B[1];
	C[1] = A[2]*B[0] - A[0]*B[2];
	C[2] = A[0]*B[1] - A[1]*B[0];
}


//	Here, generate all the data needed by Joe to move the models...
//	---------------------------------------------------------------
void CBioModel::GenerateFrames( 
					    float center[MAX_JOINTS][3],
					    float X[MAX_JOINTS][3],
					    float Y[MAX_JOINTS][3],
					    float Z[MAX_JOINTS][3] )
{

int		i = 0;
//int		joints = (int)(Dof * (1.0 / 3.0));
int			joints = Dof / 3;

//	Calculate interjoint lengths...
	for ( i = 0; i < joints; i++ )		//Pass 1...
	{
		int index = 3*i;

		center[i][0] = ( Field[(index+0)][0][0] + Field[(index+1)][0][0] + Field[(index+2)][0][0] ) * (1.0 / 3.0);
		center[i][1] = ( Field[(index+0)][1][0] + Field[(index+1)][1][0] + Field[(index+2)][1][0] ) * (1.0 / 3.0);
		center[i][2] = ( Field[(index+0)][2][0] + Field[(index+1)][2][0] + Field[(index+2)][2][0] ) * (1.0 / 3.0);

	}

//	Now the frames for all but the last joint...
	for ( i = 0; i < (joints-1); i++ )		//Pass 2...
	{
		int index = 3*i;

		Y[i][0] = center[(i+1)][0] - center[i][0];
		Y[i][1] = center[(i+1)][1] - center[i][1];
		Y[i][2] = center[(i+1)][2] - center[i][2];

		BioNorm( Y[i] );

		Z[i][0] = Field[(index+1)][0][0] - center[i][0];	//#2 is the local top...
		Z[i][1] = Field[(index+1)][1][0] - center[i][1];
		Z[i][2] = Field[(index+1)][2][0] - center[i][2];

		BioOut( Y[i], Z[i], X[i] );
		BioNorm( X[i] );
		BioOut( X[i], Y[i], Z[i] );
		BioNorm( Z[i] );
	}

//	Now do the last joint...
	i = (joints - 1);
	int index = 3*i;
	Z[i][0] = Field[(index+1)][0][0] - center[i][0];
	Z[i][1] = Field[(index+1)][1][0] - center[i][1];
	Z[i][2] = Field[(index+1)][2][0] - center[i][2];

	X[i][0] = Field[(index+0)][0][0] - Field[(index+2)][0][0];
	X[i][1] = Field[(index+0)][1][0] - Field[(index+2)][1][0];
	X[i][2] = Field[(index+0)][2][0] - Field[(index+2)][2][0];

	BioNorm( X[i] );

	BioOut( Z[i], X[i], Y[i] );
	BioNorm( Y[i] );

	BioOut( X[i], Y[i], Z[i] );
	BioNorm( Z[i] );

	/*
	conPhysics << "Points:\n";
	conPhysics << Field[0][0][0] << ", " << Field[0][1][0] << ", " << Field[0][2][0] << "\n";
	conPhysics << Field[1][0][0] << ", " << Field[1][1][0] << ", " << Field[1][2][0] << "\n";
	conPhysics << Field[2][0][0] << ", " << Field[2][1][0] << ", " << Field[2][2][0] << "\n";

	conPhysics << "XYZ\n";
	conPhysics << X[0][0] << ", " << X[0][1] << ", " << X[0][2] << "\n";
	conPhysics << Y[0][0] << ", " << Y[0][1] << ", " << Y[0][2] << "\n";
	conPhysics << Z[0][0] << ", " << Z[0][1] << ", " << Z[0][2] << "\n";
	*/

}


//#define SAFE
#ifdef SAFE

//	A BioTruss, with field tracers...
//	---------------------------------
void CBioModel::CBioModel( int pelvis, float length, float base, float taper, float joints, float offset[3] )
{
		memset(this, 0, sizeof(*this));

//		First, zero everything...
//		-------------------------
		Pelvis = pelvis;

float	a = base,
		dist = length / (joints - 1);
int		counter = 0;

		taper = .5;

float	delta = taper * ( a/(joints) );
float	l = dist;//length / (joints-1);

float	separator = 0;

int		numJoints = (int)joints;

//		Set the model to have numJoints degrees of freedom!
		Dof = 3*numJoints;


//		For safety...
		Control[0] = Control[1] = 
		Control[2] = Control[3] = 0;
		Control[4] =-1;

		for (int j = 0; j < FIELD_DIMENSION; j++)								//Dof...
		{
			Field[j][0][0] = Field[j][0][1] = Field[j][0][2] = 
			Field[j][1][0] = Field[j][1][1] = Field[j][1][2] = 
			Field[j][2][0] = Field[j][2][1] = Field[j][2][2] = 0;

			Psi[j][0] = 1;
		}


//		Offset from the origin...
		Offset[0] = offset[0];
		Offset[1] = offset[1];
		Offset[2] = offset[2];
			

//		First, manufacture the zeroth set...
//		------------------------------------
float	alpha = base,
		beta  = alpha - delta,
		gamma = alpha + delta;

float	h = .288*alpha,
		U =  2*h,
		b = .5*alpha;

float	crossb = sqrt( l*l + .25*(alpha+beta)*(alpha+beta) ),
		crossg,
		straight = sqrt( l*l + .25*delta*delta );

		Field[0][0][0] = b;	Field[1][0][0] = 0;	Field[2][0][0] =-b;
		Field[0][1][0] = 0;	Field[1][1][0] = 0;	Field[2][1][0] = 0;
		Field[0][2][0] =-h;	Field[1][2][0] = U;	Field[2][2][0] =-h;

		Tensor[0][0] = 1;	Rho[0][0] = alpha;
		Tensor[0][1] = 2;	Rho[0][1] = alpha;
		Tensor[0][2] = 3;	Rho[0][2] = straight;
		Tensor[0][3] = 4;	Rho[0][3] = crossb;
		Tensor[0][4] = 5;	Rho[0][4] = crossb;
		Tensor[0][5] = -1;

		Tensor[1][0] = 0;	Rho[1][0] = alpha;
		Tensor[1][1] = 2;	Rho[1][1] = alpha;
		Tensor[1][2] = 3;	Rho[1][2] = crossb;
		Tensor[1][3] = 4;	Rho[1][3] = straight;
		Tensor[1][4] = 5;	Rho[1][4] = crossb;
		Tensor[1][5] = -1;

		Tensor[2][0] = 0;	Rho[2][0] = alpha;
		Tensor[2][1] = 1;	Rho[2][1] = alpha;
		Tensor[2][2] = 3;	Rho[2][2] = crossb;
		Tensor[2][3] = 4;	Rho[2][3] = crossb;
		Tensor[2][4] = 5;	Rho[2][4] = straight;
		Tensor[2][5] = -1;


//		conPhysics << "Huzzah!: " << sqrt(	(Field[0][0][0] - Field[1][0][0])*(Field[0][0][0] - Field[1][0][0])
//										  + (Field[0][1][0] - Field[1][1][0])*(Field[0][1][0] - Field[1][1][0])
//										  + (Field[0][2][0] - Field[1][2][0])*(Field[0][2][0] - Field[1][2][0]) ) << "\n";




//		Now, manufacture the rest of the joints, and the bindings...
//		------------------------------------------------------------
		for ( int i = 1; i < ( (int)joints-1 ); i++ )
		{
			int index = i*3;

			separator += dist;
			
//			alpha = base - (float)i*delta;
			beta  = alpha - delta;
			gamma = alpha + delta;

			h = .288*alpha;
			U =  2*h;
			b = .5*alpha;

			crossb   = sqrt( l*l + .25*(alpha+beta)*(alpha+beta) );
			crossg   = sqrt( l*l + .25*(alpha+gamma)*(alpha+gamma) );
			straight = sqrt( l*l + .25*delta*delta );

			Field[(index+0)][0][0] = b;
			Field[(index+0)][1][0] = separator;
			Field[(index+0)][2][0] =-h;

				Tensor[(index+0)][0] = (index+1);	Rho[(index+0)][0] = alpha;		//Amongst ourselves...
				Tensor[(index+0)][1] = (index+2);	Rho[(index+0)][1] = alpha;

				Tensor[(index+0)][2] = (index-3);	Rho[(index+0)][2] = straight;	//Back...
				Tensor[(index+0)][3] = (index-2);	Rho[(index+0)][3] = crossg;
				Tensor[(index+0)][4] = (index-1);	Rho[(index+0)][4] = crossg;

				Tensor[(index+0)][5] = (index+3);	Rho[(index+0)][5] = straight;	//Forward...
				Tensor[(index+0)][6] = (index+4);	Rho[(index+0)][6] = crossb;
				Tensor[(index+0)][7] = (index+5);	Rho[(index+0)][7] = crossb;
				Tensor[(index+0)][8] = -1;


			Field[(index+1)][0][0] = 0;
			Field[(index+1)][1][0] = separator;
			Field[(index+1)][2][0] = U;

				Tensor[(index+1)][0] = (index+0);	Rho[(index+1)][0] = alpha;
				Tensor[(index+1)][1] = (index+2);	Rho[(index+1)][1] = alpha;

				Tensor[(index+1)][2] = (index-3);	Rho[(index+1)][2] = crossg;
				Tensor[(index+1)][3] = (index-2);	Rho[(index+1)][3] = straight;
				Tensor[(index+1)][4] = (index-1);	Rho[(index+1)][4] = crossg;

				Tensor[(index+1)][5] = (index+3);	Rho[(index+1)][5] = crossb;
				Tensor[(index+1)][6] = (index+4);	Rho[(index+1)][6] = straight;
				Tensor[(index+1)][7] = (index+5);	Rho[(index+1)][7] = crossb;
				Tensor[(index+1)][8] = -1;

			Field[(index+2)][0][0] =-b;
			Field[(index+2)][1][0] = separator;
			Field[(index+2)][2][0] =-h;

				Tensor[(index+2)][0] = (index+0);	Rho[(index+2)][0] = alpha;		//Amongst ourselves...
				Tensor[(index+2)][1] = (index+1);	Rho[(index+2)][1] = alpha;

				Tensor[(index+2)][2] = (index-3);	Rho[(index+2)][2] = crossg;	//Back...
				Tensor[(index+2)][3] = (index-2);	Rho[(index+2)][3] = crossg;
				Tensor[(index+2)][4] = (index-1);	Rho[(index+2)][4] = straight;

				Tensor[(index+2)][5] = (index+3);	Rho[(index+2)][5] = crossb;	//Forward...
				Tensor[(index+2)][6] = (index+4);	Rho[(index+2)][6] = crossb;
				Tensor[(index+2)][7] = (index+5);	Rho[(index+2)][7] = straight;
				Tensor[(index+2)][8] = -1;


		}

//		Finally the last element...
//		---------------------------
		counter = Dof - 3;

			separator += dist;

//			alpha -= delta;
			beta   = alpha - delta;
			gamma  = alpha + delta;

			h = .288*alpha;
			U =  2*h;
			b = .5*alpha;

			crossb   = sqrt( l*l + .25*(alpha+beta)*(alpha+beta) );
			crossg   = sqrt( l*l + .25*(alpha+gamma)*(alpha+gamma) );
			straight = sqrt( l*l + .25*delta*delta );


		Field[(counter+0)][0][0] = b;
		Field[(counter+0)][1][0] = separator;
		Field[(counter+0)][2][0] =-h;

			Tensor[(counter+0)][0] = (counter+1);	Rho[(counter+0)][0] = alpha;		//Amongst ourselves...
			Tensor[(counter+0)][1] = (counter+2);	Rho[(counter+0)][1] = alpha;

			Tensor[(counter+0)][2] = (counter-3);	Rho[(counter+0)][2] = straight;	//Back...
			Tensor[(counter+0)][3] = (counter-2);	Rho[(counter+0)][3] = crossg;
			Tensor[(counter+0)][4] = (counter-1);	Rho[(counter+0)][4] = crossg;
			Tensor[(counter+0)][5] = -1;

		Field[(counter+1)][0][0] = 0;
		Field[(counter+1)][1][0] = separator;
		Field[(counter+1)][2][0] = U;

			Tensor[(counter+1)][0] = (counter+0);	Rho[(counter+1)][0] = alpha;
			Tensor[(counter+1)][1] = (counter+2);	Rho[(counter+1)][1] = alpha;

			Tensor[(counter+1)][2] = (counter-3);	Rho[(counter+1)][2] = crossg;
			Tensor[(counter+1)][3] = (counter-2);	Rho[(counter+1)][3] = straight;
			Tensor[(counter+1)][4] = (counter-1);	Rho[(counter+1)][4] = crossg;
			Tensor[(counter+1)][5] = -1;

		Field[(counter+2)][0][0] =-b;
		Field[(counter+2)][1][0] = separator;
		Field[(counter+2)][2][0] =-h;

			Tensor[(counter+2)][0] = (counter+0);	Rho[(counter+2)][0] = alpha;		//Amongst ourselves...
			Tensor[(counter+2)][1] = (counter+1);	Rho[(counter+2)][1] = alpha;

			Tensor[(counter+2)][2] = (counter-3);	Rho[(counter+2)][2] = crossg;	//Back...
			Tensor[(counter+2)][3] = (counter-2);	Rho[(counter+2)][3] = crossg;
			Tensor[(counter+2)][4] = (counter-1);	Rho[(counter+2)][4] = straight;
			Tensor[(counter+2)][5] = -1;



		Field[(counter+3)][0][0] = Field[0][0][0];
		Field[(counter+3)][1][0] = Field[0][1][0];
		Field[(counter+3)][2][0] = Field[0][2][0];

		Field[(counter+4)][0][0] = Field[1][0][0];
		Field[(counter+4)][1][0] = Field[1][1][0];
		Field[(counter+4)][2][0] = Field[1][2][0];

		Field[(counter+5)][0][0] = Field[2][0][0];
		Field[(counter+5)][1][0] = Field[2][1][0];
		Field[(counter+5)][2][0] = Field[2][2][0];

//	Field[3][2][0] -= .5;

				conPhysics << "model: " << model << " with " << Dof << " dof...\n";
				for (int t = 0; t < Dof; t++)
				{
					conPhysics << t << ": " << Field[t][0][0] << ", " << Field[t][1][0] << ", " << Field[t][2][0] << "\n";
				}


}


#else



//	A BioTruss, with field tracers...
//	---------------------------------
CBioModel::CBioModel( int pelvis, float length, float base, float taper, float joints, float offset[3] )
{

//		First, zero everything...
//		-------------------------
		memset(this, 0, sizeof(*this));

		Pelvis = pelvis;

float	a = base,
		dist = length / (joints - 1);
int		counter = 0;

		taper = .5;//.3;

float	delta = taper * ( a/(joints) );
float	l = length / (joints-1);

float	separator = 0;

int		numJoints = (int)joints;

//		Set the model to have numJoints degrees of freedom!
		Dof = 3*numJoints;


//		For safety...
		Control[0] = Control[1] = 
		Control[2] = Control[3] = 0;
		Control[4] =-1;

		for (int j = 0; j < FIELD_DIMENSION; j++)								//Dof...
		{
			Field[j][0][0] = Field[j][0][1] = Field[j][0][2] = 
			Field[j][1][0] = Field[j][1][1] = Field[j][1][2] = 
			Field[j][2][0] = Field[j][2][1] = Field[j][2][2] = 0;

			Psi[j][0] = 1;
		}


//		Offset from the origin...
		Offset[0] = offset[0];
		Offset[1] = offset[1];
		Offset[2] = offset[2];
			

//		First, manufacture the zeroth set...
//		------------------------------------
float	alpha = base,
		beta  = alpha - delta,
		gamma = alpha + delta;

float	h = .288*alpha,
		U =  2*h,
		b = .5*alpha;

float	crossb = sqrt( l*l + .25*(alpha+beta)*(alpha+beta) ),
		crossg,
		straight = sqrt( l*l + .25*delta*delta );

		Field[0][0][0] = b;	Field[1][0][0] = 0;	Field[2][0][0] =-b;
		Field[0][1][0] = 0;	Field[1][1][0] = 0;	Field[2][1][0] = 0;
		Field[0][2][0] =-h;	Field[1][2][0] = U;	Field[2][2][0] =-h;

		Tensor[0][0] = 1;	Rho[0][0] = alpha;
		Tensor[0][1] = 2;	Rho[0][1] = alpha;
		Tensor[0][2] = 3;	Rho[0][2] = straight;
		Tensor[0][3] = 4;	Rho[0][3] = crossb;
		Tensor[0][4] = 5;	Rho[0][4] = crossb;
		Tensor[0][5] = -1;

		//Tensor[1][0] = 0;	Rho[1][0] = alpha;
		Tensor[1][0] = 2;	Rho[1][0] = alpha;
		Tensor[1][1] = 3;	Rho[1][1] = crossb;
		Tensor[1][2] = 4;	Rho[1][2] = straight;
		Tensor[1][3] = 5;	Rho[1][3] = crossb;
		Tensor[1][4] = -1;

		//Tensor[2][0] = 0;	Rho[2][0] = alpha;
		//Tensor[2][1] = 1;	Rho[2][1] = alpha;
		Tensor[2][0] = 3;	Rho[2][0] = crossb;
		Tensor[2][1] = 4;	Rho[2][1] = crossb;
		Tensor[2][2] = 5;	Rho[2][2] = straight;
		Tensor[2][3] = -1;


//		conPhysics << "Huzzah!: " << sqrt(	(Field[0][0][0] - Field[1][0][0])*(Field[0][0][0] - Field[1][0][0])
//										  + (Field[0][1][0] - Field[1][1][0])*(Field[0][1][0] - Field[1][1][0])
//										  + (Field[0][2][0] - Field[1][2][0])*(Field[0][2][0] - Field[1][2][0]) ) << "\n";




//		Now, manufacture the rest of the joints, and the bindings...
//		------------------------------------------------------------
		for ( int i = 1; i < ( (int)joints-1 ); i++ )
		{
			int index = i*3;

			separator += dist;
			
			alpha = base - (float)i*delta;
			beta  = alpha - delta;
			gamma = alpha + delta;

			h = .288*alpha;
			U =  2*h;
			b = .5*alpha;

			crossb   = sqrt( l*l + .25*(alpha+beta)*(alpha+beta) );
			crossg   = sqrt( l*l + .25*(alpha+gamma)*(alpha+gamma) );
			straight = sqrt( l*l + .25*delta*delta );

			Field[(index+0)][0][0] = b;
			Field[(index+0)][1][0] = separator;
			Field[(index+0)][2][0] =-h;

				Tensor[(index+0)][0] = (index+1);	Rho[(index+0)][0] = alpha;		//Amongst ourselves...
				Tensor[(index+0)][1] = (index+2);	Rho[(index+0)][1] = alpha;

				//Tensor[(index+0)][2] = (index-3);	Rho[(index+0)][2] = straight;	//Back...
				//Tensor[(index+0)][3] = (index-2);	Rho[(index+0)][3] = crossg;
				//Tensor[(index+0)][4] = (index-1);	Rho[(index+0)][4] = crossg;

				Tensor[(index+0)][2] = (index+3);	Rho[(index+0)][2] = straight;	//Forward...
				Tensor[(index+0)][3] = (index+4);	Rho[(index+0)][3] = crossb;
				Tensor[(index+0)][4] = (index+5);	Rho[(index+0)][4] = crossb;
				Tensor[(index+0)][5] = -1;


			Field[(index+1)][0][0] = 0;
			Field[(index+1)][1][0] = separator;
			Field[(index+1)][2][0] = U;

				//Tensor[(index+1)][0] = (index+0);	Rho[(index+1)][0] = alpha;
				Tensor[(index+1)][0] = (index+2);	Rho[(index+1)][0] = alpha;

				//Tensor[(index+1)][2] = (index-3);	Rho[(index+1)][2] = crossg;
				//Tensor[(index+1)][3] = (index-2);	Rho[(index+1)][3] = straight;
				//Tensor[(index+1)][4] = (index-1);	Rho[(index+1)][4] = crossg;

				Tensor[(index+1)][1] = (index+3);	Rho[(index+1)][1] = crossb;
				Tensor[(index+1)][2] = (index+4);	Rho[(index+1)][2] = straight;
				Tensor[(index+1)][3] = (index+5);	Rho[(index+1)][3] = crossb;
				Tensor[(index+1)][4] = -1;

			Field[(index+2)][0][0] =-b;
			Field[(index+2)][1][0] = separator;
			Field[(index+2)][2][0] =-h;

				//Tensor[(index+2)][0] = (index+0);	Rho[(index+2)][0] = alpha;		//Amongst ourselves...
				//Tensor[(index+2)][1] = (index+1);	Rho[(index+2)][1] = alpha;

				//Tensor[(index+2)][2] = (index-3);	Rho[(index+2)][2] = crossg;	//Back...
				//Tensor[(index+2)][3] = (index-2);	Rho[(index+2)][3] = crossg;
				//Tensor[(index+2)][4] = (index-1);	Rho[(index+2)][4] = straight;

				Tensor[(index+2)][0] = (index+3);	Rho[(index+2)][0] = crossb;	//Forward...
				Tensor[(index+2)][1] = (index+4);	Rho[(index+2)][1] = crossb;
				Tensor[(index+2)][2] = (index+5);	Rho[(index+2)][2] = straight;
				Tensor[(index+2)][3] = -1;


		}



//		Finally the last element...
//		---------------------------
		counter = Dof - 3;

			separator += dist;

			alpha -= delta;
			beta   = alpha - delta;
			gamma  = alpha + delta;

			h = .288*alpha;
			U =  2*h;
			b = .5*alpha;

			crossb   = sqrt( l*l + .25*(alpha+beta)*(alpha+beta) );
			crossg   = sqrt( l*l + .25*(alpha+gamma)*(alpha+gamma) );
			straight = sqrt( l*l + .25*delta*delta );


		Field[(counter+0)][0][0] = b;
		Field[(counter+0)][1][0] = separator;
		Field[(counter+0)][2][0] =-h;

			Tensor[(counter+0)][0] = (counter+1);	Rho[(counter+0)][0] = alpha;		//Amongst ourselves...
			Tensor[(counter+0)][1] = (counter+2);	Rho[(counter+0)][1] = alpha;

			//Tensor[(counter+0)][2] = (counter-3);	Rho[(counter+0)][2] = straight;	//Back...
			//Tensor[(counter+0)][3] = (counter-2);	Rho[(counter+0)][3] = crossg;
			//Tensor[(counter+0)][4] = (counter-1);	Rho[(counter+0)][4] = crossg;
			Tensor[(counter+0)][2] = -1;

		Field[(counter+1)][0][0] = 0;
		Field[(counter+1)][1][0] = separator;
		Field[(counter+1)][2][0] = U;

			//Tensor[(counter+1)][0] = (counter+0);	Rho[(counter+1)][0] = alpha;
			Tensor[(counter+1)][0] = (counter+2);	Rho[(counter+1)][0] = alpha;

			//Tensor[(counter+1)][2] = (counter-3);	Rho[(counter+1)][2] = crossg;
			//Tensor[(counter+1)][3] = (counter-2);	Rho[(counter+1)][3] = straight;
			//Tensor[(counter+1)][4] = (counter-1);	Rho[(counter+1)][4] = crossg;
			Tensor[(counter+1)][1] = -1;

		Field[(counter+2)][0][0] =-b;
		Field[(counter+2)][1][0] = separator;
		Field[(counter+2)][2][0] =-h;

			//Tensor[(counter+2)][0] = (counter+0);	Rho[(counter+2)][0] = alpha;		//Amongst ourselves...
			//Tensor[(counter+2)][1] = (counter+1);	Rho[(counter+2)][1] = alpha;

			//Tensor[(counter+2)][2] = (counter-3);	Rho[(counter+2)][2] = crossg;	//Back...
			//Tensor[(counter+2)][3] = (counter-2);	Rho[(counter+2)][3] = crossg;
			//Tensor[(counter+2)][4] = (counter-1);	Rho[(counter+2)][4] = straight;
			Tensor[(counter+2)][0] = -1;



		Field[(counter+3)][0][0] = Field[0][0][0];
		Field[(counter+3)][1][0] = Field[0][1][0];
		Field[(counter+3)][2][0] = Field[0][2][0];

		Field[(counter+4)][0][0] = Field[1][0][0];
		Field[(counter+4)][1][0] = Field[1][1][0];
		Field[(counter+4)][2][0] = Field[1][2][0];

		Field[(counter+5)][0][0] = Field[2][0][0];
		Field[(counter+5)][1][0] = Field[2][1][0];
		Field[(counter+5)][2][0] = Field[2][2][0];

//	Field[3][2][0] -= .5;


		conPhysics << "New biomodel with " << Dof << " dof...\n";
		for (int t = 0; t < Dof; t++)
		{
			conPhysics << t << ": " << Field[t][0][0] << ", " << Field[t][1][0] << ", " << Field[t][2][0] << "\n";
		}


}



#endif








//	 Here we pass a control input to an individual point in the Field...
void CBioModel::SetInput( float position[3], float roll, float intensity, int pelvis_number )
{

	Control[0] = intensity;		//1 = head point, 100 = head move...

	Control[1] = position[0];		//Coordinates in pelvis frame that we want to point at...
	Control[2] = position[1];
	Control[3] = position[2];
	Control[5] = roll;

	Control[4] = (float)pelvis_number;

	if (Pel_Data[pelvis_number][48] > 5.0f) Control[0] = 0;
}






























#include "Lib/Std/ArrayIO.hpp"
#include <stdio.h>

//******************************************************************************************
std::ostream& operator <<(std::ostream& os, CPArray<float> pa)
{
	for (int i = 0; i < pa.size(); i++)
	{
		char str_float[32];
		sprintf(str_float, "%.7g ", pa[i]);
		os << str_float;
	}
	return os;
}

void CBioModel::DumpState(std::ostream& os)
{
#if VER_TEST
	int j, k;

	os	<<" DOF=" <<Dof
		<< std::endl;

	os	<<"  Field=";
	for (j = 0; j < FIELD_DIMENSION; j++)
	{
		os	<<"    ";
		for (k = 0; k < 3; k++)
			os	<<CPArray<float>(3, Field[j][k]) <<" ";
		os	<< std::endl;
	}
	os	<< std::endl;

	os	<<"  FieldInputs=";
	for (j = 0; j < FIELD_DIMENSION; j++)
		os	<<"    " <<CPArray<float>(3, FieldInputs[j])
			<< std::endl;
	os	<< std::endl;

	os	<<"  Tensor=";
	for (j = 0; j < FIELD_DIMENSION; j++)
	{
		os	<<"    ";
		for (k = 0; k < FIELD_DIMENSION; k++)
			os	<<int(Tensor[j][k]) <<" ";
		os	<< std::endl;
	}
	os	<< std::endl;

	os	<<"  Rho=";
	for (j = 0; j < FIELD_DIMENSION; j++)
		os	<<"    " <<CPArray<float>(FIELD_DIMENSION, Rho[j])
			<< std::endl;
	os	<< std::endl;

	os	<<"  Psi=";
	for (j = 0; j < FIELD_DIMENSION; j++)
		os	<<"    " <<CPArray<float>(5, Psi[j])
			<< std::endl;
	os	<< std::endl;

	os	<<"  Offset=" <<CPArray<float>(3, Offset)
		<< std::endl;

	os	<<"  Control=" <<CPArray<float>(5, Control)
		<< std::endl;

	os	<< std::endl;
#endif
}

void DumpBioState(std::ostream& os)
{
#if VER_TEST
	int i;

	for (i = 0; i < MAX_BIOMODELS; i++)
		if (BioModels[i])
		{
			os	<<"BioModel" <<i << std::endl;
			BioModels[i]->DumpState(os);
		}
	os	<< std::endl;
#endif
}
