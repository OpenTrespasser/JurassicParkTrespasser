//	Arms.cpp
//	========

//	Calculates the arm dynamics for the player model, and perhaps the dinoes...

//	Includes...
#include "common.hpp"
#include "Arms.h"
#include "pelvis_def.h"
#include "pelvis.h"
#include "futil.h"
#include "Xob_bc.hpp"

#include "Lib/Sys/ConIO.hpp"
#include <iostream>

//	Bad, but OK for now...
//	-=-=-=-=-=-=-=-=-=-=-=
SControlArm	ControlArm;
bool	ArmUnderControl = false;
float	ttime = 0;
float	tvar = 0;

float			multip_G = 80;	//teST ONLY!
float			multipD_G = 2.5;//1;

//	-=-=-=-=-=-=-=-=-=-=-=

//	Calculate the hand and arm...
void Calculate_Human_Arm_Info( int pelvis, int arm, float points[4][3], float matricies[4][3][3] )
{


float	Eta[3],
		Eta_h[3],
		Arm[3],
		yf[3],
		xk[3],
		yk[3],
		temp[3],
		Yl[3],
		Zl[3];

float	wtemp[3];

float	etamag, blo, me;

float	shoulder[3],
		elbow[3],
		wrist[3];


//			Get the point at the shoulder...
			if (arm == RIGHT_HAND) StuffVec( Pel_Data[pelvis][13] + Pel_Data[pelvis][37],	//NOTE: assume shoulder_w = hip_w...
											 Pel_Data[pelvis][38],
											 Pel_Data[pelvis][39], shoulder );

			else				   StuffVec(-Pel_Data[pelvis][13] + Pel_Data[pelvis][37],
											 Pel_Data[pelvis][38],
											 Pel_Data[pelvis][39], shoulder );
			Local_to_World_Trans( shoulder, pelvis );

//			If we are holding something, then the hand is part of a compound object!
//			------------------------------------------------------------------------
int			bcindex = Pel_Box_BC[pelvis][RIGHT_HAND];
CXob&		xob = Xob[bcindex];

//			The wrist...
			StuffVec( Pel[pelvis][(arm+0)][0], Pel[pelvis][(arm+1)][0], Pel[pelvis][(arm+2)][0], wrist );

			if ( xob.Anchored /*xob.Data[11] > 1*/ )	//Are you magneted to another pivot???
			{
				wtemp[0] = xob.SuperData[0][3];
				wtemp[1] = xob.SuperData[0][4];
				wtemp[2] = xob.SuperData[0][5];
				Foot_to_World( wtemp, arm, pelvis );

				wrist[0] += wtemp[0];
				wrist[1] += wtemp[1];
				wrist[2] += wtemp[2];
			}

//			Calculate Eta...
			StuffVec( Pel[pelvis][0][0] - /*Pel[pelvis][(arm+0)][0]*/wrist[0],			//Need this in l...
					  Pel[pelvis][1][0] - /*Pel[pelvis][(arm+1)][0]*/wrist[1],
					  Pel[pelvis][2][0] - /*Pel[pelvis][(arm+2)][0]*/wrist[2],
					  Eta );

			World_to_Local( Eta, pelvis );
			StuffVec( -Eta[0], -Eta[1], -Eta[2], Arm );						//Here is the local foot...

			if (arm == RIGHT_HAND)	Eta[0] = Pel_Data[pelvis][13] + Pel_Data[pelvis][37] + Eta[0];	//There it is...
			else					Eta[0] =-Pel_Data[pelvis][13] + Pel_Data[pelvis][37] + Eta[0];
									Eta[1] = Pel_Data[pelvis][38] + Eta[1];
									Eta[2] = Pel_Data[pelvis][39] + Eta[2];
			CopyVec( Eta, Eta_h );
			Normalize( Eta_h, etamag, me );



//			StuffVec( 1, 0, 0, yf );		//Need yhatf in Local...
			StuffVec( 0, 0, 1, yf );		//Need yhatf in Local...
			Foot_to_Local( yf, arm, pelvis );

			//conPhysics << "yf: " << yf[0] << ", " << yf[1] << ", " << yf[2] << "\n";
			if ( yf[2] < .25 ) 
			{
				yf[2] = .25;
				float butt, plug;
				Normalize( yf, butt, plug );
				//conPhysics << "!!!\n";
			}

			//if ( yf[2] > .95 ) {
			//{
			//	yf[2] = .95;
			//	float butt, plug;
			//	Normalize( yf, butt, plug );
			//	conPhysics << "!!!\n";
			//}

			Outer( Eta_h, yf, xk );
			//if ( xk[0] < 0 ) {
			//	xk[0] = 0;
			//	conPhysics << "How does this happen???\n";
			//}


			Normalize( xk, blo, me );		//Not clearly unit...
			Outer( xk, Eta_h, yk );			//There she is...
			Normalize( yk, blo, me );

//			Here is the elbow...
			blo = .5;
			if ( etamag < .9999*Pel_Data[pelvis][40] ) me =-.5*sqrt( Pel_Data[pelvis][40]*Pel_Data[pelvis][40] - etamag*etamag );
			else me = 0;//	Check for singularity...
			StuffVec( Arm[0] + blo*Eta[0] + me*xk[0],
					  Arm[1] + blo*Eta[1] + me*xk[1],
					  Arm[2] + blo*Eta[2] + me*xk[2],
					  elbow );
//			StuffVec( Arm[0] + blo*Eta[0] - me*yk[0],
//					  Arm[1] + blo*Eta[1] - me*yk[1],
//					  Arm[2] + blo*Eta[2] - me*yk[2],
//					  elbow );
			Local_to_World_Trans( elbow, pelvis );

//			Now the matricies...
//			--------------------
			Local_to_World( yk, pelvis );

//			Upper arm...
			StuffVec( elbow[0] - shoulder[0],
					  elbow[1] - shoulder[1],
					  elbow[2] - shoulder[2],
					  Yl );
			Normalize( Yl, blo, me );			//Get it ready...
			Outer( yk, Yl, Zl );				//Here is Z...
//			Outer( xk, Yl, Zl );				//Here is Z...

			CopyVec( shoulder, points[0] );
			matricies[0][0][0] = yk[0];	matricies[0][1][0] = yk[1];	matricies[0][2][0] = yk[2];
			matricies[0][0][1] = Yl[0];	matricies[0][1][1] = Yl[1];	matricies[0][2][1] = Yl[2];
			matricies[0][0][2] = Zl[0];	matricies[0][1][2] = Zl[1];	matricies[0][2][2] = Zl[2];

//			matricies[0][0][0] =-Zl[0];	matricies[0][1][0] =-Zl[1];	matricies[0][2][0] =-Zl[2];
//			matricies[0][0][1] = Yl[0];	matricies[0][1][1] = Yl[1];	matricies[0][2][1] = Yl[2];
//			matricies[0][0][2] =-xk[0];	matricies[0][1][2] =-xk[1];	matricies[0][2][2] =-xk[2];

//			Forearm...
			StuffVec( wrist[0] - elbow[0],
					  wrist[1] - elbow[1],
					  wrist[2] - elbow[2],
					  Yl );
			Normalize( Yl, blo, me );			//Get it ready...
			Outer( yk, Yl, Zl );				//Here is Z...
//			Outer( xk, Yl, Zl );				//Here is Z...

			CopyVec( elbow, points[1] );
			matricies[1][0][0] = yk[0];	matricies[1][1][0] = yk[1];	matricies[1][2][0] = yk[2];
			matricies[1][0][1] = Yl[0];	matricies[1][1][1] = Yl[1];	matricies[1][2][1] = Yl[2];
			matricies[1][0][2] = Zl[0];	matricies[1][1][2] = Zl[1];	matricies[1][2][2] = Zl[2];

//			matricies[1][0][0] =-Zl[0];	matricies[1][1][0] =-Zl[1];	matricies[1][2][0] =-Zl[2];
//			matricies[1][0][1] = Yl[0];	matricies[1][1][1] = Yl[1];	matricies[1][2][1] = Yl[2];
//			matricies[1][0][2] =-xk[0];	matricies[1][1][2] =-xk[1];	matricies[1][2][2] =-xk[2];

//			Hand...
			CopyVec( wrist, points[2] );

			StuffVec( 1, 0, 0, temp );
			xob.Back_Element( temp[0], temp[1], temp[2], 0);
			Foot_to_World( temp, arm, pelvis );
			matricies[2][0][0] = temp[0];	matricies[2][1][0] = temp[1];	matricies[2][2][0] = temp[2];

			StuffVec( 0, 1, 0, temp );
			xob.Back_Element( temp[0], temp[1], temp[2], 0);
			Foot_to_World( temp, arm, pelvis );
			matricies[2][0][1] = temp[0];	matricies[2][1][1] = temp[1];	matricies[2][2][1] = temp[2];


			StuffVec( 0, 0, 1, temp );
			xob.Back_Element( temp[0], temp[1], temp[2], 0);
			Foot_to_World( temp, arm, pelvis );
			matricies[2][0][2] = temp[0];	matricies[2][1][2] = temp[1];	matricies[2][2][2] = temp[2];
			//conPhysics << temp[0] << " : " << temp[1] << " : " << temp[2] << "\n";

//			Fingers...
			StuffVec( 0, Pel_Data[pelvis][41], 0, temp );
			xob.Back_Element( temp[0], temp[1], temp[2], 0);
			Foot_to_World( temp, arm, pelvis );
			AddVec( temp, wrist, temp );
			CopyVec( temp, points[3] );

float		ca = cos( ControlArm.fFingerCurl ),
			sa = sin( ControlArm.fFingerCurl );

			matricies[3][0][0] = matricies[2][0][0];	matricies[3][0][1] = matricies[2][0][1]*ca + matricies[2][0][2]*sa;	matricies[3][0][2] = matricies[2][0][2]*ca - matricies[2][0][1]*sa;
			matricies[3][1][0] = matricies[2][1][0];	matricies[3][1][1] = matricies[2][1][1]*ca + matricies[2][1][2]*sa;	matricies[3][1][2] = matricies[2][1][2]*ca - matricies[2][1][1]*sa;
			matricies[3][2][0] = matricies[2][2][0];	matricies[3][2][1] = matricies[2][2][1]*ca + matricies[2][2][2]*sa;	matricies[3][2][2] = matricies[2][2][2]*ca - matricies[2][2][1]*sa;



//	Finally fucking done...
//	-----------------------

}







#define		CONST_ALPHA 50
#define		CONST_BETA  50
#define		CONST_GAMMA 50

#define		DAMP_ALPHA  17//2
#define		DAMP_BETA   17//2
#define		DAMP_GAMMA  17//2


float	G_alpha_K = CONST_ALPHA;
float	G_alpha_D = DAMP_ALPHA;

float	G_beta_K = CONST_BETA;
float	G_beta_D = DAMP_BETA;

float	G_gamma_K = CONST_GAMMA;
float	G_gamma_D = DAMP_GAMMA;

bool	hack_roll = false;



#define MASS 1



//	Well, calculate the arm information!
//	------------------------------------
void Update_Right_Arm( int pelvis, float timestep, float result[6] )
{


float	strength = 150*150;								//100*100;               //20*20;//5*5;//1;//5*5;
float	ArmDesire[3];
float	intensity = 5;//1;//.4;//.4;//07;

float	wrist[3],
		offset[3],
		vel[3];


//	Set the positions...
	if ( (ControlArm.fPosUrgency > 0) && (Pel_Data[pelvis][49]>0) )
	{
		intensity = .75;
//		Do not increase for swing. Default speed is better...
		//if (ControlArm.bSwing) intensity = 1;

		ArmDesire[0] = ControlArm.afPosition[0];
		ArmDesire[1] = ControlArm.afPosition[1];
		ArmDesire[2] = ControlArm.afPosition[2];

	}
	else {

		intensity = .7;//.4;              //.2;//.3;//.4;//07;
		ArmDesire[0] = Pel_Data[pelvis][13] + .21;
		ArmDesire[1] = .15;
		ArmDesire[2] = -.35;//-.28;//-.085;//-.05;

	}


//		OK, compute the mass solution...
//		--------------------------------
int		bcindex = Pel_Box_BC[pelvis][RIGHT_HAND];	//Used all throughout...
CXob&	xob = Xob[bcindex];

		StuffVec( Pel[pelvis][(RIGHT_HAND+0)][0],
				  Pel[pelvis][(RIGHT_HAND+1)][0],
				  Pel[pelvis][(RIGHT_HAND+2)][0], wrist );	//Wrist [position w/o offset...

		StuffVec( xob.SuperData[0][3],
				  xob.SuperData[0][4],
				  xob.SuperData[0][5], offset );		//The offset...

		//Global_Vel( vel, offset, RIGHT_HAND, pelvis );		//BEFORE transforming offset, get vel...
		vel[0] = Pel[pelvis][(RIGHT_HAND+0)][1];
		vel[1] = Pel[pelvis][(RIGHT_HAND+1)][1];
		vel[2] = Pel[pelvis][(RIGHT_HAND+2)][1];

		Foot_to_World( offset, RIGHT_HAND, pelvis );
		wrist[0] += offset[0];								//NOW we have the real wrist position...
		wrist[1] += offset[1];
		wrist[2] += offset[2];

		vel[0] -= 1.25*Pel[pelvis][0][1];	//Help to stay centered!
		vel[1] -= 1.25*Pel[pelvis][1][1];
		vel[2] -= 1.25*Pel[pelvis][2][1];
		//if (Pel[pelvis][MOUTH][0] < .8*Pel_Data[pelvis][14]) vel[2] -= 2*Pel[pelvis][MOUTH][1];
		//vel[2] -= Pel[pelvis][MOUTH][1];
		if ( (ControlArm.fPosUrgency > 0) && (Pel_Data[pelvis][49]>0) ) vel[2] += 1.25*Pel[pelvis][2][1];


//		Now apply the control...
		Local_to_World_Trans( ArmDesire, pelvis );

		float R[3];
		float mass = 3;		//1;//5;//2;

		if (xob.Anchored)
		{
			Get_Dirac_Vel( vel, (RIGHT_HAND+3), pelvis );
			StuffVec( xob.SuperData[0][3],
					  xob.SuperData[0][4],
					  xob.SuperData[0][5], wrist );
			Foot_to_World_Trans( wrist, RIGHT_HAND, pelvis );
			//intensity = 1;
		}

		float norm = Mass_Solution( ArmDesire, wrist, vel, mass, intensity, strength, R );
		result[0] = R[0];
		result[1] = R[1];
		result[2] = R[2];

float factor = 1.0;
		//if (norm > .05) factor = .5;

//		Design checks...
		if (xob.Data[19]>10)  result[2] -= 10;
		if (xob.Data[19]>30)  result[2]  = 0;

		if (xob.Data[19]>100) result[2]  = result[1] = result[0] = 0;




//	Now, calculate Eta and Rho, then apply hand rotations based on them...
//	----------------------------------------------------------------------
float		Arm[3],
			shoulder[3],
			Eta[3],
			Eta_h[3],
//			yk[3],
			Vel[3];
//			temp[3];

float		me,
			etamag;
	
//			Get the point at the shoulder...
			StuffVec( Pel_Data[pelvis][13] + Pel_Data[pelvis][37],			//NOTE: assume shoulder_w = hip_w...
					  Pel_Data[pelvis][38],
					  Pel_Data[pelvis][39], shoulder );
			Local_to_World_Trans( shoulder, pelvis );

//			Calculate Eta...
			StuffVec( Pel[pelvis][0][0] - wrist[0],							//Need this in l...
					  Pel[pelvis][1][0] - wrist[1],
					  Pel[pelvis][2][0] - wrist[2],
					  Eta );

			World_to_Local( Eta, pelvis );
			StuffVec( -Eta[0], -Eta[1], -Eta[2], Arm );						//Here is the local foot...

			Eta[0] = Pel_Data[pelvis][13] + Pel_Data[pelvis][37] + Eta[0];	//There it is...
			Eta[1] = Pel_Data[pelvis][38] + Eta[1];
			Eta[2] = Pel_Data[pelvis][39] + Eta[2];

			CopyVec( Eta, Eta_h );
			Normalize( Eta_h, etamag, me );

#define MUL_FAC 1.5
			if (etamag > MUL_FAC*Pel_Data[pelvis][40])	//Is the hand too far away???
			{
				HandDrop( pelvis );
			}


float new_X[3] = {  ControlArm.aafOrient[0][0], ControlArm.aafOrient[1][0], ControlArm.aafOrient[2][0] };
float new_Y[3] = {  ControlArm.aafOrient[0][1], ControlArm.aafOrient[1][1], ControlArm.aafOrient[2][1] };
float new_Z[3] = {  ControlArm.aafOrient[0][2], ControlArm.aafOrient[1][2], ControlArm.aafOrient[2][2] };

#define PI 3.141592
float	mee, too;

/*				if (new_Z[2] < .02)
				{

					if (new_Z[0] < -.6)
					{
						new_Z[2] = .02;
						Normalize( new_Z, mee, too );
					}
					else if (new_Z[0] < .02)
					{
						new_Z[0] = .02;
						Normalize( new_Z, mee, too );
					}


				}
*/
				if (new_Z[0] < 0)
				{
					new_Z[0] = 0;
					Normalize( new_Z, mee, too );

					Outer( new_Y, new_Z, new_X );
				}





/*				float badVec[3] = { new_Z[0], new_Z[2], 0 };
				float v1, v2;
				SafeNormalize( badVec, v1, v2 );

				float ang_test = asin( badVec[0] );
				//if (new_Z[2] >= 0) ang_test = ang_test;
				//else ang_test = -ang_test;


				ang_test = asin( badVec[0] );
				//conPhysics << "BV: " << badVec[0] << ", " << badVec[1] << ", " << badVec[2] << "\n";
				conPhysics << "angle: " << ang_test << "\n";
				//conPhysics << "angle: " << ang_test << "\n";
				//conPhysics << "Angle: " << ang_test << "NZ2: " << new_Z[2] << " normalized: " << badVec[1] << "\n";
			//	if ( ang_test < -.5*3.1415 ) conPhysics << "Hey!!!!!!!!!!!!!!!!!!!\n";
*/

				if ( ControlArm.fOrientUrgency == 0 )
				{
					new_Y[0] =-Eta_h[0];
					new_Y[1] =-Eta_h[1];
					new_Y[2] =-Eta_h[2];

					float tq[3] = {1,0,0};
					Outer( tq, new_Y, new_Z );
				}

				Local_to_Foot( new_X, RIGHT_HAND, pelvis );
				Local_to_Foot( new_Y, RIGHT_HAND, pelvis );
				Local_to_Foot( new_Z, RIGHT_HAND, pelvis );

#define MIN_ZX -.8
//				if ( testroll > .95 ) testroll = .95;	//Positive limit...
//				if ( hack_roll && (ControlArm.aafOrient[0][0]>=.5) ) hack_roll = false;
//				if ( testroll <= MIN_ZX ) hack_roll = true;
	

				Get_Dirac_Vel( Vel, (RIGHT_HAND_EULER), pelvis );

//				Design checks...
float			design_scale = 1;

				if (xob.Data[19]>10)  design_scale = .05;
				if (xob.Data[19]>30)  design_scale = .005;
				if (xob.Data[19]>100) design_scale = 0;

				//conPhysics << xob.Data[19] << "\n";

//				Are we Swingin'?
				//conPhysics << "CA.BS: " << ControlArm.bSwing << "\n";
				bool fuck_you = ControlArm.bSwing;
				if ( fuck_you )
				{
					multip_G  = 80;
					multipD_G = .9;
				}
				else
				{
					///multip_G = 80;
					///multipD_G = 2.5;

					//multip_G = 40;
					//multipD_G = 1.5;

					multip_G = 80;
					multipD_G = 2.0f;//1.5;
				}


				if ( xob.Data[19] < 3 )
				{
					multip_G = 100;
					multipD_G = 1.55;
				}

				G_alpha_K = design_scale*multip_G*xob.Data[15];
				G_beta_K  = design_scale*multip_G*xob.Data[16];
				G_gamma_K = design_scale*multip_G*xob.Data[17];


				//multipD_G = .25;
				G_alpha_D = multipD_G*factor*sqrt( xob.Data[15]*G_alpha_K );
				G_beta_D  = multipD_G*factor*sqrt( xob.Data[16]*G_beta_K );
				G_gamma_D = multipD_G*factor*sqrt( xob.Data[17]*G_gamma_K );

				//if (xob.Data[11] > 1) G_alpha_K = G_beta_K = G_gamma_K = 0;

				result[3]  = G_alpha_K*( new_Y[2] ) - G_alpha_D*Vel[0];
				result[5]  = G_gamma_K*(-new_Y[0] ) - G_gamma_D*Vel[2];


				if ( 1 )//!hack_roll)
				{
					//result[4]  = G_beta_K *(Rho[0]   - testroll)  - G_beta_D *Vel[1];

					//result[4]  = G_beta_K *(new_Z[0])  - G_beta_D *Vel[1];
					result[4]  = G_beta_K *(-new_X[2])  - G_beta_D *Vel[1];

					//conPhysics << new_X[2] << "--\n";
				}
				//else
				//{
				//float temp = ControlArm.aafOrient[0][0];
				//if (temp < -.15) temp = -.15;
				//result[4]  = G_beta_K *(yk[0] - ControlArm.aafOrient[0][0])  - G_beta_D *Vel[1];

				//conPhysics << ControlArm.aafOrient[0][0] << "\n";
				//}
				//conPhysics << "result[4]: " << HANDROLL + ControlArm.aafOrient[2][0] << "\n";


float temp[3];



			if ( xob.Data[11] > 1 )
			{
				//result[3] = result[4] = result[5] = 0;

				StuffVec( xob.SuperData[1][3],	//free
						  xob.SuperData[1][4],
						  xob.SuperData[1][5], offset );

				//StuffVec( xob.SuperData[0][3],	//anchored
				//		  xob.SuperData[0][4],
				//		  xob.SuperData[0][5], offset );

				if (xob.Anchored)
				{
					result[0] *=-.4;//-.3;
					result[1] *=-.4;//-.3;
					result[2] *=-.25;//-.3;

					const float c_d = 20;
					//result[3] = result[4] = result[5] = 0;

					result[3] = - c_d*Vel[0]; 
					result[4] = - c_d*Vel[1]; 
					result[5] = - c_d*Vel[2]; 

					StuffVec( xob.SuperData[0][3],	//anchored
							  xob.SuperData[0][4],
							  xob.SuperData[0][5], offset );

				} 
				//conPhysics << "O:" << offset[0] << ", " << offset[1] << ", " << offset[2] << "\n";

			float fornow[3] = {-result[0],
							   -result[1],
							   -result[2] };

			World_to_Foot( fornow, RIGHT_HAND, pelvis );

			Outer( offset, fornow, temp );

			result[3] += temp[0];
			result[4] += temp[1];
			result[5] += temp[2];

			}


			//if (xob.Data[19]>100) result[0] = result[1] = result[2] = result[3] = result[4] = result[5] = 0;

//	Safeties...
//	-----------
	if (1)//xob.bHitAnother)
	{

//		Linear...
		float	temp[3] = {BioTag[pelvis][(RIGHT_HAND+0)],
						   BioTag[pelvis][(RIGHT_HAND+1)],
						   BioTag[pelvis][(RIGHT_HAND+2)]},
				mag, ma;

		SafeNormalize( temp, mag, ma );
		ma = temp[0]*result[0] + temp[1]*result[1] + temp[2]*result[2];

		if (ma < 0)
		{
			result[0] -= ma*temp[0];
			result[1] -= ma*temp[1];
			result[2] -= ma*temp[2];
			//conPhysics << "Linear Hand Boundary\n";
			if (ControlArm.bSwing) 
				result[3] = result[4] = result[5] = 0;
		}

//		And rotational...
/*		float	Ttemp[3] = {BioTag[pelvis][(RIGHT_HAND+3)],
						    BioTag[pelvis][(RIGHT_HAND+4)],
						    BioTag[pelvis][(RIGHT_HAND+5)]};

		SafeNormalize( Ttemp, mag, ma );
		//ma = Ttemp[0]*result[3] + Ttemp[1]*result[1] + Ttemp[2]*result[2];
		ma = Ttemp[0]*result[3] + Ttemp[1]*result[4] + Ttemp[2]*result[5];

		if (ma < 0)
		{
			result[3] -= ma*temp[3];
			result[4] -= ma*temp[4];
			result[5] -= ma*temp[5];
			conPhysics << "Rotational Hand Boundary\n";
		}

*/			//if (xob.Data[19]>100) result[0] = result[1] = result[2] = result[3] = result[4] = result[5] = 0;

	}



//		Rescale...
//		----------
		result[0] *= xob.Data[0];
		result[1] *= xob.Data[0];
		result[2] *= xob.Data[0];
		
		result[3] *= xob.Data[7];
		result[4] *= xob.Data[8];
		result[5] *= xob.Data[9];



}




//	Process a trivial control event...
void Control_Arms( int pelvis, const SControlArm& cta )
{
	ControlArm = cta;

	if ( ControlArm.fPosUrgency > 0 ) ArmUnderControl = true;
	else ArmUnderControl = false;
}