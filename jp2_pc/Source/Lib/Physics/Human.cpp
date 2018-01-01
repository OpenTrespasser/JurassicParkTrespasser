//	Human.cpp
//	=========

//	Tools for first person player models...
//	---------------------------------------


//	Includes...
#include "Common.hpp"
#include "Human.h"
#include "Pelvis_Def.h"
#include "Pelvis.h"
#include "futil.h"
#include "Arms.h"
#include "Xob_bc.hpp"
#include "BioModel.h"

#include "Lib/Audio/SoundDefs.hpp"
#include "Lib/Sys/ConIO.hpp"
 
//	Code...
//	-=-=-=-


//	Here we create the objects and parameters that make a human model and stick them together...
void Make_a_Human( CInstance* pins, int object, float init_state[7][3] )
{

float	parameters[PELVIS_PARAMETERS];		//Here is where we compute the joint params for the pelvic model...

	for (int p = 0; p < PELVIS_PARAMETERS; p++)
		parameters[p] = 0;

	conPhysics << "Make a HUMAN... -- ";

	Assert(object >= 0 && object < NUM_PELVISES);
	Assert(Pel_Usage[object] == 0);
	Pel_Usage[object] = epelHUMAN;

//	Calculate pelvis...
//	-------------------

//	Inverse mus...
	parameters[31] =.05;
	parameters[32] = 1;
	parameters[33] = 1;

//	Thigh and Shin...
//	parameters[34] = .5*(1.5);
//	parameters[35] = .5*(.5);
	parameters[34] = .5*(1);
	parameters[35] = .5*(1);
//	parameters[34] = .5*(.5);
//	parameters[35] = .5*(.5);

//	Total ETA_max...
//	parameters[14] = 2*(parameters[34] + parameters[35]);
	parameters[14] = (parameters[34] + parameters[35]);

//	Percentage rest...
	parameters[21] = .9;//.98;//.9;

//	Toes...
//	parameters[0] =.2;
//	parameters[1] =.6;
	parameters[0] =.075;
	parameters[1] =.25;
	parameters[44] = .1;

//	Foot BC...
//	parameters[2] =.02;
//	parameters[3] = 1000;//50;//100;
//	parameters[4] = 500;//5;//50;
	parameters[2] =.09;
	parameters[3] = parameters[32];//50;//100;
	parameters[4] =.5*sqrt( parameters[3]/parameters[32]);//500;//5;//50;
 

//	Static mu...
	parameters[5] = 500;
	parameters[6] = 0;//1;

	parameters[10] = 0;
	parameters[11] = 0;
	parameters[12] = 0;

//	Hip...
//	parameters[13] =.5;
	parameters[13] = .19;//.083;//.2;

//	Leg kappa, delta...
//	parameters[15] = 30 / parameters[31];//150;
//	parameters[16] = parameters[15] / 2;//50;
	parameters[15] = 100 / parameters[31];//30 / parameters[31];//150;
	//parameters[16] = 1.5*parameters[15] / 3;//parameters[15] / 3;//50;
	parameters[16] = 1.*parameters[15] / 3;//.75*parameters[15] / 3;//parameters[15] / 3;//50;
 	//parameters[16] = -.025*parameters[15] / 3;//.75*parameters[15] / 3;//parameters[15] / 3;//50;


	parameters[15] =
	parameters[16] = 0;


//	Ankle alpha offset...
//	parameters[20] = .5;
//	parameters[17] = 50;	//Kappa,
//	parameters[22] = 15;	//Delta...
	parameters[20] = .5;
	parameters[17] = 70;//50;	//Kappa,
	parameters[22] = 15;//5;		//Delta...

	parameters[18] = 200;	//Beta Kappa...
	parameters[23] = 200;	//Beta delta...

//	Gamma...
	parameters[19] = 100;
	parameters[24] = 50;

	parameters[25] =.03;
	parameters[26] = 0.001;//.003;
	parameters[27] =.275;

//	parameters[25] =.275;
//	parameters[26] =.275;
//	parameters[27] =.275;

	parameters[28] = 0;
	parameters[29] = 0;
	parameters[30] = 0;

//	Foot inverse moi, abg...
	parameters[7] = .12/(parameters[32]*(parameters[0]*parameters[0] + parameters[1]*parameters[1] ) );
	parameters[8] = parameters[7];	//Symmetric for now, will this need to change?
	parameters[9] = parameters[7];

//	Quad parameters...
	parameters[45] = parameters[46] = 0;

//	-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Here is where the rest of the initialization for the human would go, i.e. arms...
//	-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Shoulders...
	parameters[37] = 0;//.14;//0;
	parameters[38] = 0;
	parameters[39] =.438;//.9*parameters[34];	//Relate these based on average man...

//	Arms...
	parameters[40] = .778;//.52;//.53;//.46;//(1.8*parameters[34]);	//Again, wrist hangs just below hips...

//	Hand...
	parameters[41] = .149;//.125;	//Length from wrist to knuckles
	parameters[42] = .08;	//Width...
	parameters[43] = .08;	//Fingers...

	parameters[49] = 100;	//Health...

	//conPhysics << "Making a Human Pelvis...\n";

//	Manufacture the pelvic model...
float boxy[10];
	Make_a_Pelvis( pins, object, parameters, init_state, boxy );

	Pel[object][MOUTH][0] = Pel_Data[object][14] - .2;	//Initialize crotch...
}

void Get_Human_Hand_Data(int i_index, SHandData ahd[2])
{

	ahd->afSize[0] = .5*.1;//.08;
	ahd->afSize[1] = .5*.15;		//Faces down Y...
	ahd->afSize[2] = .5*.05;

	ahd->fMass = 1;

	ahd->afState[0] = Pel[i_index][RIGHT_HAND+0][0];
	ahd->afState[1] = Pel[i_index][RIGHT_HAND+1][0];
	ahd->afState[2] = Pel[i_index][RIGHT_HAND+2][0];

	ahd->afState[3] = Pel[i_index][RIGHT_HAND+3][0];
	ahd->afState[4] = Pel[i_index][RIGHT_HAND+4][0];
	ahd->afState[5] = Pel[i_index][RIGHT_HAND+5][0];
	ahd->afState[6] = Pel[i_index][RIGHT_HAND+6][0];

}



//	Calculate the knee info, and the foot...
void Calculate_Human_Leg_Info( int pelvis, int foot, float points[3][3], float matricies[3][3][3] )
{


float	Eta[3],
		Eta_h[3],
		Foot[3],
		yf[3],
		xk[3],
		yk[3],
		temp[3],
		Yl[3],
		Zl[3];

float	etamag, blo, me;

float	hip[3],
		knee[3],
		toe[3];

//			Get the point at the hip...
			if (foot == RIGHT_FOOT) StuffVec( Pel_Data[pelvis][13], 0, 0, hip );
			else                    StuffVec(-Pel_Data[pelvis][13], 0, 0, hip );
			Local_to_World_Trans( hip, pelvis );

//			Calculate Eta...
			StuffVec( Pel[pelvis][0][0] - Pel[pelvis][(foot+0)][0],			//Need this in l...
					  Pel[pelvis][1][0] - Pel[pelvis][(foot+1)][0],
					  Pel[pelvis][2][0] - Pel[pelvis][(foot+2)][0],
					  Eta );

			World_to_Local( Eta, pelvis );
			StuffVec( -Eta[0], -Eta[1], -Eta[2], Foot );		//Here is the local foot...

			if (foot == RIGHT_FOOT)	Eta[0] = Pel_Data[pelvis][13] + Eta[0];				//There it is...
			else					Eta[0] =-Pel_Data[pelvis][13] + Eta[0];
									Eta[1] =				      + Eta[1];
									Eta[2] =				      + Eta[2];
			CopyVec( Eta, Eta_h );
			SafeNormalize( Eta_h, etamag, me );
			StuffVec( 0, 1, 0, yf );		//Need yhatf in Local...
			Foot_to_Local( yf, foot, pelvis );
			Outer( yf, Eta_h, xk );
			SafeNormalize( xk, blo, me );		//Not clearly unit...
			Outer( Eta_h, xk, yk );			//There she is...


//			Here is the knee...
			blo = .5;
			if ( etamag < .9999*Pel_Data[pelvis][14] ) me = sqrt( Pel_Data[pelvis][34]*Pel_Data[pelvis][34] - .25*etamag*etamag );
			else me = 0;//	Check for singularity...
			StuffVec( Foot[0] + blo*Eta[0] + me*yk[0],
					  Foot[1] + blo*Eta[1] + me*yk[1],
					  Foot[2] + blo*Eta[2] + me*yk[2],
					  knee );
			Local_to_World_Trans( knee, pelvis );

//			And the toe...
			StuffVec( 0, 0, 0, toe );
			Foot_to_World_Trans( toe, foot, pelvis );



//			Now the matricies...
//			--------------------
			Local_to_World( xk, pelvis );

//			Thigh...
			StuffVec( knee[0] - hip[0],
					  knee[1] - hip[1],
					  knee[2] - hip[2],
					  Yl );
			Normalize( Yl, blo, me );			//Get it ready...
			Outer( xk, Yl, Zl );				//Here is Z...

			CopyVec( hip, points[0] );
			matricies[0][0][0] = xk[0];	matricies[0][1][0] = xk[1];	matricies[0][2][0] = xk[2];
			matricies[0][0][1] = Yl[0];	matricies[0][1][1] = Yl[1];	matricies[0][2][1] = Yl[2];
			matricies[0][0][2] = Zl[0];	matricies[0][1][2] = Zl[1];	matricies[0][2][2] = Zl[2];

//			Shin...
			StuffVec( toe[0] - knee[0],
					  toe[1] - knee[1],
					  toe[2] - knee[2],
					  Yl );
			Normalize( Yl, blo, me );			//Get it ready...
			Outer( xk, Yl, Zl );				//Here is Z...

			CopyVec( knee, points[1] );
			matricies[1][0][0] = xk[0];	matricies[1][1][0] = xk[1];	matricies[1][2][0] = xk[2];
			matricies[1][0][1] = Yl[0];	matricies[1][1][1] = Yl[1];	matricies[1][2][1] = Yl[2];
			matricies[1][0][2] = Zl[0];	matricies[1][1][2] = Zl[1];	matricies[1][2][2] = Zl[2];


//			Toes...
			CopyVec( toe, points[2] );

			StuffVec( 1, 0, 0, temp );
			Foot_to_World( temp, foot, pelvis );
			matricies[2][0][0] = temp[0];	matricies[2][1][0] = temp[1];	matricies[2][2][0] = temp[2];

			StuffVec( 0, 1, 0, temp );
			Foot_to_World( temp, foot, pelvis );
			matricies[2][0][1] = temp[0];	matricies[2][1][1] = temp[1];	matricies[2][2][1] = temp[2];

			StuffVec( 0, 0, 1, temp );
			Foot_to_World( temp, foot, pelvis );
			matricies[2][0][2] = temp[0];	matricies[2][1][2] = temp[1];	matricies[2][2][2] = temp[2];

//	Finally fucking done...
//	-----------------------

}



//	Fill the entire transform stack for the Velociraptor...
//	-------------------------------------------------------
void Get_Human_Data( int model,
					 float loc[7],
					 float points[HUMAN_SKELETAL_ELEMENTS][3],
					 float matricies[HUMAN_SKELETAL_ELEMENTS][3][3],
					 int Am_I_Supported[HUMAN_SKELETAL_ELEMENTS] )
{

//		Indicates whether or not the renderer should care...
		for (int yme = 0; yme < HUMAN_SKELETAL_ELEMENTS; yme++) Am_I_Supported[yme] = 0;

float	Mr[3][3][3],
		Pr[3][3],
		Ml[3][3][3],
		Pl[3][3];

float	Ar[4][3][3],
		Pa[4][3];


//	conPhysics << "Get Human info!\n";
/*
	//Calculate_Human_Leg_Info( model, RIGHT_FOOT, Pr, Mr );
	CopyVec( Mr[0][0], matricies[1][0] );
	CopyVec( Mr[0][1], matricies[1][1] );
	CopyVec( Mr[0][2], matricies[1][2] );
	CopyVec( Pr[0], points[1] );
	Am_I_Supported[1] = 1;

	CopyVec( Mr[1][0], matricies[2][0] );
	CopyVec( Mr[1][1], matricies[2][1] );
	CopyVec( Mr[1][2], matricies[2][2] );
	CopyVec( Pr[1], points[2] );
	Am_I_Supported[2] = 1;

	CopyVec( Mr[2][0], matricies[3][0] );
	CopyVec( Mr[2][1], matricies[3][1] );
	CopyVec( Mr[2][2], matricies[3][2] );
	CopyVec( Pr[2], points[3] );
	Am_I_Supported[3] = 1;

	//Calculate_Human_Leg_Info( model, LEFT_FOOT,  Pl, Ml );
	CopyVec( Ml[0][0], matricies[5][0] );
	CopyVec( Ml[0][1], matricies[5][1] );
	CopyVec( Ml[0][2], matricies[5][2] );
	CopyVec( Pl[0], points[5] );
	Am_I_Supported[5] = 1;

	CopyVec( Ml[1][0], matricies[6][0] );
	CopyVec( Ml[1][1], matricies[6][1] );
	CopyVec( Ml[1][2], matricies[6][2] );
	CopyVec( Pl[1], points[6] );
	Am_I_Supported[6] = 1;

	CopyVec( Ml[2][0], matricies[7][0] );
	CopyVec( Ml[2][1], matricies[7][1] );
	CopyVec( Ml[2][2], matricies[7][2] );
	CopyVec( Pl[2], points[7] );
	Am_I_Supported[7] = 1;
*/
//	Now the arms!
	Calculate_Human_Arm_Info( model, RIGHT_HAND, Pa, Ar );
	CopyVec( Ar[0][0], matricies[10][0] );
	CopyVec( Ar[0][1], matricies[10][1] );
	CopyVec( Ar[0][2], matricies[10][2] );
	CopyVec( Pa[0], points[10] );
	Am_I_Supported[10] = 1;

	//conPhysics << "Body: " << Pel[model][0][0] << ", " << Pel[model][1][0] << ", " << Pel[model][2][0] << "\n";
	//conPhysics << "Shoulder: " << points[10][0] << ", " << points[10][1] << ", " << points[10][2] << "\n";

	CopyVec( Ar[1][0], matricies[11][0] );
	CopyVec( Ar[1][1], matricies[11][1] );
	CopyVec( Ar[1][2], matricies[11][2] );
	CopyVec( Pa[1], points[11] );
	Am_I_Supported[11] = 1;

	CopyVec( Ar[2][0], matricies[12][0] );
	CopyVec( Ar[2][1], matricies[12][1] );
	CopyVec( Ar[2][2], matricies[12][2] );
	CopyVec( Pa[2], points[12] );
	Am_I_Supported[12] = 1;

	CopyVec( Ar[3][0], matricies[13][0] );	//Fingers!
	CopyVec( Ar[3][1], matricies[13][1] );
	CopyVec( Ar[3][2], matricies[13][2] );
	CopyVec( Pa[3], points[13] );
	Am_I_Supported[13] = 1;


//	Do the CM...
	loc[0] = Pel[model][0][0];
	loc[1] = Pel[model][1][0];
	loc[2] = Pel[model][2][0];
	loc[3] = Pel[model][3][0];
	loc[4] = Pel[model][4][0];
	loc[5] = Pel[model][5][0];
	loc[6] = Pel[model][6][0];



}

