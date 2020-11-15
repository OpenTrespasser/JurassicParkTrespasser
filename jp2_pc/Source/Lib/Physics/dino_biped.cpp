//	Dino_Biped.cpp
//	==============

//	Everything you need to draw two legged dinosaurs...
//	---------------------------------------------------

#include "common.hpp"
#include "pelvis_def.h"
#include "pelvis.h"
#include "futil.h"
#include <iostream>
#include "Lib/Sys/ConIO.hpp"
#include "Xob_bc.hpp"
#include "BioModel.h"
#include "BioStructure.h"
#include "Lib/Audio/SoundDefs.hpp"

#define NUM_BIO_PER_RAPTOR 2


//	Yikes!
extern float	Kontrol_Mouth[NUM_PELVISES];

 
float	Mangle = -.2;
float butley[3] = {0,0,0};
//	Here we create the objects and parameters that make a raptor model and stick them together...
void Make_a_Raptor( CInstance* pins, int object, Biped_Params& params, float init_state[7][3] )
{

float	parameters[PELVIS_PARAMETERS];		//Here is where we compute the joint params for the pelvic model...

	for (int p = 0; p < PELVIS_PARAMETERS; p++)
		parameters[p] = 0;

	Assert(object >= 0 && object < NUM_PELVISES);
	Assert(Pel_Usage[object] == 0);
	Pel_Usage[object] = epelRAPTOR;


float	mass = params.mass,
		leg_length = params.leg_length,
		hip_radius = params.hip_radius,
		tail_length = params.tail_length,
		neck_length = params.neck_length,
		neck_offset[3] = { params.neck_offset[0],
						   params.neck_offset[1],
						   params.neck_offset[2] },
		tail_offset[3] = { params.tail_offset[0],
						   params.tail_offset[1],
						   params.tail_offset[2] };


		conPhysics << "Mass: " << params.mass << ", leg: " << leg_length << "\n";

		mass = 20;//100;//mass;//20,
		if (leg_length>2.0) mass = 50;
		if (leg_length>3.0) mass = 100;
//		leg_length = .7*2,
//		hip_radius = .191,
//		tail_length = 1.8,
//		neck_length = 1,
//		neck_offset[3] = {0, .42, 0},
//		tail_offset[3] = {0, -.3, 0};


//	Calculate pelvis...
//	-------------------

//	Neck Length...
	parameters[48] = neck_length;

//	Inverse mus...
	parameters[31] = 1 / mass;                 //.05;
	parameters[32] = 1;
	parameters[33] = 1;

//	Thigh and Shin...
	//parameters[34] = .5*(1.5);
	//parameters[35] = .5*(.5);

	parameters[34] = .5*( .6666*leg_length );
	parameters[35] = .5*( .3333*leg_length );

//	Total ETA_max...
	parameters[14] = 2*(parameters[34] + parameters[35]);

//	Percentage rest...
//	parameters[21] = .9;
	//parameters[21] = .25;//.75;    //.70;//.75;

//	parameters[21] = .86;
	parameters[21] = 1.0;//

//	Toes...
	parameters[0] =.2;
	parameters[1] =.6;

//	Foot BC...
//	parameters[2] =.02;
//	parameters[3] = 1000;//50;//100;
//	parameters[4] = 500;//5;//50;
	parameters[2] =.09;
	parameters[3] = 200*parameters[32];//50;//100;
	//parameters[4] = 1.25*sqrt( parameters[3]/parameters[32]);//500;//5;//50;
	parameters[4] = 4.*sqrt( 2*parameters[3]/parameters[32]);//500;//5;//50;
 

//	Static mu...
	parameters[5] = 100;//500;
	parameters[6] = 0;//1;

	parameters[10] = 0;
	parameters[11] = 0;
	parameters[12] = 0;

//	Hip...
//	parameters[13] =.5;
	parameters[13] = hip_radius;//.191;

//	Leg kappa, delta...

	//parameters[15] =  1300;
	//parameters[16] =  .8*sqrt( 2*parameters[15]/parameters[31] );
	parameters[15] =  700;
	if (leg_length>2) parameters[15] *= 1.6;//1.25;
	parameters[16] =  .7*sqrt( 2*parameters[15]/parameters[31] );
	if (leg_length>2) parameters[16] *= .4;//1.25;

	
	//	Ankle alpha offset...
	parameters[20] = .7;
	//parameters[17] = 50;	//Kappa,
	//parameters[22] = 20;	//Delta...
	parameters[17] = 20;	//Kappa,
	parameters[22] = 2;		//Delta...

	parameters[18] = 200;	//Beta Kappa...
	parameters[23] = 200;	//Beta delta...

//	Gamma...
	parameters[19] = 100;
	parameters[24] = 10;   //50;

	//parameters[19] = 10;
	//parameters[24] = 5;   //50;

	parameters[25] =.1;//.01;
	parameters[26] =.1;//.01;
	parameters[27] =.10;// .25;//.01;


	parameters[28] = 0;
	parameters[29] = 0;
	parameters[30] = 0;


//	Foot inverse moi, abg...
	parameters[7] = .12/(parameters[32]*(parameters[0]*parameters[0] + parameters[1]*parameters[1] ) );
	parameters[8] = parameters[7];	//Symmetric for now, will this need to change?
	parameters[9] = parameters[7];

	//	Quad parameters...
	parameters[45] = 0;
	parameters[46] = 0;//1;//.5;


//	-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Here is where the rest of the initialization for the raptor would go, i.e. arms...
//	-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Shoulders...
	parameters[37] = 0;
	parameters[38] = 0;
	parameters[39] = 0;

//	Arms...
	parameters[40] = .46;

//	Hand...
	parameters[41] = .08;
	parameters[42] = .08;
	parameters[43] = .1;


	parameters[49] = 100;	//Health...

	parameters[50] = 0;
	parameters[51] = 0;
	parameters[52] = 0;

	parameters[53] = 0;
	parameters[54] = 0;

//	Manufacture the pelvic model...
float	boxy[10];
		Make_a_Pelvis( pins, object, parameters, init_state, boxy );

//float	box_extents[3] = {.15, .1, .25};
//float	box_extents[3] = {.2, .6, .4};
float	box_extents[3] = {.2, .8, .4};

int			BioIndex = NUM_BIO_PER_RAPTOR*object;
int			NumJoints = 5;
float		LenTail = tail_length;
//float		offset[3]  = {0,-.3,0};
//float		offset[3]  = {0,0,0};
//float		offseth[3] = {0,.42,0};

float	truss = 1.5;//.5;
		if (LenTail>2.5) truss = 3;

		Assert(BioModels[BioIndex+0] == 0);
		CBioModel* bio_tail = BioModels[BioIndex+0] = new CBioModel( object, LenTail, truss/*.5*/, 1, NumJoints, tail_offset );	//Make it and move it to the Butt!

		int i;
		for (i = 0; i < 3*NumJoints; i++)
		{
float		temp[3] = {-bio_tail->Field[i][0][0] + bio_tail->Offset[0],
					   -bio_tail->Field[i][1][0] + bio_tail->Offset[1],
					    bio_tail->Field[i][2][0] + bio_tail->Offset[2] };

			Local_to_World_Trans( temp, object );

			bio_tail->Field[i][0][0] = temp[0];
			bio_tail->Field[i][1][0] = temp[1];
			bio_tail->Field[i][2][0] = temp[2];
		}
		bio_tail->Field[i+0][0][0] *= -1;	//Rotate base member...
		bio_tail->Field[i+1][0][0] *= -1;
		bio_tail->Field[i+2][0][0] *= -1;

//int i;
float	LenHead = neck_length;
		NumJoints = 4;

		truss = .4;//.25;//.4;//.33;
		if (LenHead>1.5 )truss = 2.0;

		Assert(BioModels[BioIndex+1] == 0);
		CBioModel* bio_head = BioModels[BioIndex+1] = new CBioModel( object, LenHead, truss/*.33*/, 1, NumJoints, neck_offset );
		conPhysics << "After no. 2 bioindex: " << (BioIndex+1) << "\n";

		for (i = 0; i < 3*NumJoints; i++)
		{
float		temp[3] = { bio_head->Field[i][0][0] + bio_head->Offset[0],	
					    bio_head->Field[i][1][0] + bio_head->Offset[1],
					    bio_head->Field[i][2][0] + bio_head->Offset[2]};

			Local_to_World_Trans( temp, object );

			//if (i == 0) temp[1] += .5;

			bio_head->Field[i][0][0] = temp[0];
			bio_head->Field[i][1][0] = temp[1];
			bio_head->Field[i][2][0] = temp[2];
			
		}

}


//	Control the Dino Head...
//	------------------------
void RaptorBodyControl( int dino,
					    float nose[3], float head_angle, float head_move_to,
						float wagamplitude, float wagfrequency,
						float mouth_angle,
						float headup )
{


int		BioIndex = NUM_BIO_PER_RAPTOR*dino;
float	butt, munch;


		butley[0] = nose[0];
		butley[1] = nose[1];
		butley[2] = nose[2];
		Local_to_World_Trans( butley, dino );


		//nose[0] = 0;
		//nose[1] = 1;
		//nose[2] = 1;
		//Local_to_World_Trans( nose, dino );

float	world_nose[3] = { nose[0], nose[1], nose[2] };
		SafeNormalize( nose, butt, munch );

float	you_are_a_shitty_compiler[3] = {0,0,0};

		float move[3] = {0,0,0}, move_speed = 0,
			  turn[3] = {0,0,0}, turn_speed = 0;

		CBioModel* bio_head = BioModels[BioIndex+1];

#define HEADMOVE
#ifdef HEADMOVE
		if (head_move_to > 0.0)
		{
			Local_to_World_Trans( world_nose, dino );

			int	CP = bio_head->Dof - 2;	//The last element Z...
			float offset[3] = {  bio_head->Offset[0],
								 bio_head->Offset[1],
								 bio_head->Offset[2] };
			Local_to_World_Trans( offset, dino );

			float delta[3] = { world_nose[0] - (bio_head->Field[CP][0][0]/* + offset[0]*/),
							   world_nose[1] - (bio_head->Field[CP][1][0]/* + offset[1]*/),
							   world_nose[2] - (bio_head->Field[CP][2][0]/* + offset[2]*/) };

			delta[2] = 0;

			SafeNormalize( delta, butt, munch );

			if ( butt <  .5 ) head_move_to *= butt;
			//if ( butt < .1 ) head_move_to *= .1;//butt*butt;
			//if ( butt < 1 ) head_move_to *= .1;//butt*butt;
			//if ( butt <.1 ) head_move_to *= butt;//butt*butt;

			World_to_Local( delta, dino );
			//delta[0] *= .15;	delta[2] *= .15;
			Local_to_World( delta, dino );


			move[0] = delta[0];	move[1] = delta[1]; move[2] = delta[2];
			move_speed = head_move_to;

			//conPhysics << "Delta: " << delta[0] << " : " << delta[1] << " : " << delta[2] << "\n";

		}
		//else Pelvis_Control( dino, you_are_a_shitty_compiler, 0, you_are_a_shitty_compiler, 0 );
#endif

		//conPhysics << "Mag nose command: " << butt << "\n";

		float critical = .1;
		//if (Pel_Usage[dino] == epelQUAD) critical = .8;

		if (nose[1] < critical)
		{
			turn_speed = 5*25*(critical - nose[1]);
			nose[1] = critical;
			turn[2] = -1;
			if (nose[0] < 0) turn[2] = 1;

			Normalize( nose, butt, munch );
			//turn_speed = 10;
		}

			//conPhysics << "Trun: " << turn[2] << ", " << fabs(nose[0]) << "\n";

			nose[0] *= Pel_Data[dino][48];
			nose[1] *= Pel_Data[dino][48];
			nose[2] *= 1.5*Pel_Data[dino][48];

float		temp[3] = { bio_head->Field[(bio_head->Dof+1)][0][0] + bio_head->Offset[0],
					    bio_head->Field[(bio_head->Dof+1)][1][0] + bio_head->Offset[1],
					    bio_head->Field[(bio_head->Dof+1)][2][0] + bio_head->Offset[2] };

			//Local_to_World_Trans( temp, dino );

			nose[0] += temp[0];
			nose[1] += temp[1];
			nose[2] += temp[2];


		if ( (move_speed>0) || (turn_speed>0) )
		{
			Pelvis_Control( dino, move, move_speed, turn, turn_speed );
			//conPhysics << move_speed << " in: " << move[0] << " : " << move[1] << " : " << move[2] << "\n";
		}

		bio_head->SetInput( nose, head_angle, headup, dino );	//-1 indicates world space...1

		Kontrol_Mouth[dino] = mouth_angle;
		
}




//	Legs...
//	--------
void Calculate_Raptor_Leg_Info( int pelvis, int foot, float points[4][3], float matricies[4][3][3] )
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

float	etamag, blo, me,
		Rat, ee0, ee1,
		hh0, hh1, kk0, kk1;

float	hip[3],
		knee[3],
		heel[3],
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
			StuffVec( 0, 1, 0, yf );			//Need yhatf in Local...
			Foot_to_Local( yf, foot, pelvis );
			Outer( yf, Eta_h, xk );
			SafeNormalize( xk, blo, me );		//Not clearly unit...
			if (blo < .1) { StuffVec( 1, 0, 0, xk ); Foot_to_Local( xk, foot, pelvis ); }	//Case yf || Eta
			Outer( Eta_h, xk, yk );				//There she is...

			Rat = Pel_Data[pelvis][34] / Pel_Data[pelvis][35];
			ee0 = etamag * Rat / (1 + Rat);
			ee1 = etamag - ee0;
			hh0 = .5*ee0 + ee1;
			hh1 = .5*ee1;

//			Hey, it could blow up here!
			if ( etamag < .999*Pel_Data[pelvis][14] )
			{
			kk0 = sqrt( Pel_Data[pelvis][34]*Pel_Data[pelvis][34] - .25*ee0*ee0 );
			kk1 = sqrt( Pel_Data[pelvis][35]*Pel_Data[pelvis][35] - .25*ee1*ee1 );
			} else kk0 = kk1 = 0;


//			Here is the knee...
			blo = hh0;
			me = kk0;
			StuffVec( Foot[0] + blo*Eta_h[0] + me*yk[0],
					  Foot[1] + blo*Eta_h[1] + me*yk[1],
					  Foot[2] + blo*Eta_h[2] + me*yk[2],
					  knee );
			Local_to_World_Trans( knee, pelvis );

//			And the heel...
			blo = hh1;
			me =-kk1;
			StuffVec( Foot[0] + blo*Eta_h[0] + me*yk[0],
					  Foot[1] + blo*Eta_h[1] + me*yk[1],
					  Foot[2] + blo*Eta_h[2] + me*yk[2],
					  heel );
			Local_to_World_Trans( heel, pelvis );


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
			StuffVec( heel[0] - knee[0],
					  heel[1] - knee[1],
					  heel[2] - knee[2],
					  Yl );
			Normalize( Yl, blo, me );			//Get it ready...
			Outer( xk, Yl, Zl );				//Here is Z...

			CopyVec( knee, points[1] );
			matricies[1][0][0] = xk[0];	matricies[1][1][0] = xk[1];	matricies[1][2][0] = xk[2];
			matricies[1][0][1] = Yl[0];	matricies[1][1][1] = Yl[1];	matricies[1][2][1] = Yl[2];
			matricies[1][0][2] = Zl[0];	matricies[1][1][2] = Zl[1];	matricies[1][2][2] = Zl[2];

//			Foot...
			StuffVec( toe[0] - heel[0],
					  toe[1] - heel[1],
					  toe[2] - heel[2],
					  Yl );
			Normalize( Yl, blo, me );			//Get it ready...
			Outer( xk, Yl, Zl );				//Here is Z...

			CopyVec( heel, points[2] );
			matricies[2][0][0] = xk[0];	matricies[2][1][0] = xk[1];	matricies[2][2][0] = xk[2];
			matricies[2][0][1] = Yl[0];	matricies[2][1][1] = Yl[1];	matricies[2][2][1] = Yl[2];
			matricies[2][0][2] = Zl[0];	matricies[2][1][2] = Zl[1];	matricies[2][2][2] = Zl[2];


//			Toes...
			CopyVec( toe, points[3] );

			StuffVec( 1, 0, 0, temp );
			Foot_to_World( temp, foot, pelvis );
			matricies[3][0][0] = temp[0];	matricies[3][1][0] = temp[1];	matricies[3][2][0] = temp[2];

			StuffVec( 0, 1, 0, temp );
			Foot_to_World( temp, foot, pelvis );
			matricies[3][0][1] = temp[0];	matricies[3][1][1] = temp[1];	matricies[3][2][1] = temp[2];

			StuffVec( 0, 0, 1, temp );
			Foot_to_World( temp, foot, pelvis );
			matricies[3][0][2] = temp[0];	matricies[3][1][2] = temp[1];	matricies[3][2][2] = temp[2];

//	Finally fucking done...
//	-----------------------

}



//	Fill the entire transform stack for the Raptor...
//	-------------------------------------------------------
void Get_Raptor_Data( int model,
							float loc[7],
							float points[20][3], float matricies[20][3][3],
							int Am_I_Supported[20] )
{

//		Indicates whether or not the renderer should care...
		for (int yme = 0; yme < 20; yme++) Am_I_Supported[yme] = 0;

float	Mr[4][3][3],
		Pr[4][3],
		Ml[4][3][3],
		Pl[4][3];

#define LEGS
#ifdef LEGS
	Calculate_Raptor_Leg_Info( model, RIGHT_FOOT, Pr, Mr );
	CopyVec( Mr[0][0], matricies[12][0] );
	CopyVec( Mr[0][1], matricies[12][1] );
	CopyVec( Mr[0][2], matricies[12][2] );
	CopyVec( Pr[0], points[12] );
	Am_I_Supported[12] = 1;

	CopyVec( Mr[1][0], matricies[13][0] );
	CopyVec( Mr[1][1], matricies[13][1] );
	CopyVec( Mr[1][2], matricies[13][2] );
	CopyVec( Pr[1], points[13] );
	Am_I_Supported[13] = 1;

	CopyVec( Mr[2][0], matricies[14][0] );
	CopyVec( Mr[2][1], matricies[14][1] );
	CopyVec( Mr[2][2], matricies[14][2] );
	CopyVec( Pr[2], points[14] );
	Am_I_Supported[14] = 1;

	CopyVec( Mr[3][0], matricies[15][0] );
	CopyVec( Mr[3][1], matricies[15][1] );
	CopyVec( Mr[3][2], matricies[15][2] );
	CopyVec( Pr[3], points[15] );
	Am_I_Supported[15] = 1;


	Calculate_Raptor_Leg_Info( model, LEFT_FOOT,  Pl, Ml );
	CopyVec( Ml[0][0], matricies[16][0] );
	CopyVec( Ml[0][1], matricies[16][1] );
	CopyVec( Ml[0][2], matricies[16][2] );
	CopyVec( Pl[0], points[16] );
	Am_I_Supported[16] = 1;

	CopyVec( Ml[1][0], matricies[17][0] );
	CopyVec( Ml[1][1], matricies[17][1] );
	CopyVec( Ml[1][2], matricies[17][2] );
	CopyVec( Pl[1], points[17] );
	Am_I_Supported[17] = 1;

	CopyVec( Ml[2][0], matricies[18][0] );
	CopyVec( Ml[2][1], matricies[18][1] );
	CopyVec( Ml[2][2], matricies[18][2] );
	CopyVec( Pl[2], points[18] );
	Am_I_Supported[18] = 1;

	CopyVec( Ml[3][0], matricies[19][0] );
	CopyVec( Ml[3][1], matricies[19][1] );
	CopyVec( Ml[3][2], matricies[19][2] );
	CopyVec( Pl[3], points[19] );
	Am_I_Supported[19] = 1;

#endif


//	Do the CM...
	loc[0] = Pel[model][0][0];
	loc[1] = Pel[model][1][0];
	loc[2] = Pel[model][2][0];
	loc[3] = Pel[model][3][0];
	loc[4] = Pel[model][4][0];
	loc[5] = Pel[model][5][0];
	loc[6] = Pel[model][6][0];

//		Now, heh heh heh, extract the bio info...
int		BioIndex = NUM_BIO_PER_RAPTOR*model;
float	center[MAX_JOINTS][3],
		X[MAX_JOINTS][3],
		Y[MAX_JOINTS][3],
		Z[MAX_JOINTS][3];


//		Tail...
		BioModels[BioIndex+0]->GenerateFrames( center, X, Y, Z );
//		for (int i = 1; i < 5; i++ )
		int i;
		for (i = 0; i < 5; i++ )
		{

//			int j = i-1;
			int j = i;
	
			Am_I_Supported[i] = 1;
			points[i][0] = center[j][0];
			points[i][1] = center[j][1];
			points[i][2] = center[j][2];

			matricies[i][0][0] = X[j][0];	matricies[i][0][1] = Y[j][0];	matricies[i][0][2] = Z[j][0];
			matricies[i][1][0] = X[j][1];	matricies[i][1][1] = Y[j][1];	matricies[i][1][2] = Z[j][1];
			matricies[i][2][0] = X[j][2];	matricies[i][2][1] = Y[j][2];	matricies[i][2][2] = Z[j][2];

		}

//		Head...
		BioModels[BioIndex+1]->GenerateFrames( center, X, Y, Z );
		for ( i = 5; i < 9; i++ )
		{

			int j = i-5;

			Am_I_Supported[i] = 1;
			points[i][0] = center[j][0];
			points[i][1] = center[j][1];
			points[i][2] = center[j][2];
	

			matricies[i][0][0] = X[j][0];	matricies[i][0][1] = Y[j][0];	matricies[i][0][2] = Z[j][0];
			matricies[i][1][0] = X[j][1];	matricies[i][1][1] = Y[j][1];	matricies[i][1][2] = Z[j][1];
			matricies[i][2][0] = X[j][2];	matricies[i][2][1] = Y[j][2];	matricies[i][2][2] = Z[j][2];

		}


		float temp[3] = {1,0,0};
		Foot_to_World( temp, HEAD_CONTROL-3, model );
		matricies[8][0][0] = temp[0];		matricies[8][1][0] = temp[1];		matricies[8][2][0] = temp[2];

		temp[0] = 0;	temp[1] = 1;	temp[2] = 0;
		Foot_to_World( temp, HEAD_CONTROL-3, model );
		matricies[8][0][1] = temp[0];		matricies[8][1][1] = temp[1];		matricies[8][2][1] = temp[2];

		temp[0] = 0;	temp[1] = 0;	temp[2] = 1;
		Foot_to_World( temp, HEAD_CONTROL-3, model );
		matricies[8][0][2] = temp[0];		matricies[8][1][2] = temp[1];		matricies[8][2][2] = temp[2];


//	Jaw!
//	----
float		alpha =-Pel[model][MOUTH][0];
float		ca = cos( alpha ),
			sa = sin( alpha );

			points[9][0] = points[8][0];
			points[9][1] = points[8][1];
			points[9][2] = points[8][2];

//extern float asshole[3];
//			points[9][0] = asshole[0];
//			points[9][1] = asshole[1];
//			points[9][2] = asshole[2];


			Am_I_Supported[9] = 1;

			matricies[9][0][0] = matricies[8][0][0];	matricies[9][0][1] = matricies[8][0][1]*ca + matricies[8][0][2]*sa;	matricies[9][0][2] = matricies[8][0][2]*ca - matricies[8][0][1]*sa;
			matricies[9][1][0] = matricies[8][1][0];	matricies[9][1][1] = matricies[8][1][1]*ca + matricies[8][1][2]*sa;	matricies[9][1][2] = matricies[8][1][2]*ca - matricies[8][1][1]*sa;
			matricies[9][2][0] = matricies[8][2][0];	matricies[9][2][1] = matricies[8][2][1]*ca + matricies[8][2][2]*sa;	matricies[9][2][2] = matricies[8][2][2]*ca - matricies[8][2][1]*sa;

}
