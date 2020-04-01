//	Foot BC for pelvic model...
//	===========================

#include <iostream>
#include "Common.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Pelvis.h"
#include "Futil.h"
#include "Xob_bc.hpp"
#include "PhysicsImport.hpp"
#include "Lib/Std/Set.hpp"

//-=-=-=-=-=-==-=-=
float	asshole[3] = {0,0,0};
float	asshole1[3] = {0,0,0};
float	asshole2[3] = {0,0,0};
float	asshole3[3] = {0,0,0};
float	Kontrol[NUM_PELVISES][6];
float	Kontrol_Mouth[NUM_PELVISES];
bool	bKontrol_Krouch[NUM_PELVISES];
int		iKontrol_Jump[NUM_PELVISES];
float	Pelvis_Set_Crouch = -100;
float	Pelvis_Jump[3];
bool	Pelvis_Jump_Voluntary;
bool	bIsLimp[NUM_PELVISES];
CSet<int> asFootLatch[NUM_PELVISES];

#define LEFT_FOOT_LATCH 1
#define FLEFT_FOOT_LATCH 2
#define RIGHT_FOOT_LATCH 3
#define FRIGHT_FOOT_LATCH 4

 
//-=--=-=-=-=-=-=-=

inline float arcsin(float s)
{
	return asin(MinMax(s, -1.0, 1.0));
}


//	Feet are stored in the pelvic coordinate system.

//	Models shown have over 18 degrees of freedom.  Records are on file in Pelvis_DEF.h...

//	Get the moments on the feet of pelvis...
//	----------------------------------------
void Foot_IDOF( int pelvis, float delta_t, float Moments[PELVIS_DOF] ) 
{

float	FootPrint[3],	//Temps...
		FootVel[3],
		Result[3],
		Temp[3],
		TempT[3],
		Eta[3],
		EtaHat[3],
		EtaHat_f[3],
		Rates[3];

float	etamag,			//Mag of eta,
		ietamag,		
		speed,			//For EtaF calculations,
		phi,
		phid,
		fsign;
//		temp1,
//		temp2;

int		iJumpTest = 0;
float	fFrontTau, fBackTau;

float	rdelta  = 0;
float	rdelta1 = 0;

float DefaultZ = -.95;//-.86;//-.78;
	if (Pel_Usage[pelvis] == epelQUAD) DefaultZ = -.86;

//	Danger, code below!
//	=-=-=-=-=-=-=-=-=-=

//	Zero all the results...
	ZeroVecDim( Moments, PELVIS_DOF );

//	For all to use...
	Get_Dirac_Vel( Rates, BODY_EULER, pelvis );

	if (1)//Pel_Data[pelvis][49]>0)
	{

//	First, process BC on the FEET:
//	==============================
		if (  (Pel_Usage[pelvis] == epelRAPTOR) || (Pel_Usage[pelvis] == epelQUAD)  )	
		{


//		Ok.  First, take care of some aesthetic issues...
float	fTotalSpeed2 = Pel[pelvis][0][1]*Pel[pelvis][0][1] + Pel[pelvis][1][1]*Pel[pelvis][1][1];
float	LegBounce = .5;//.25;//.1;
float	AMul = 2;
#define	HACK_FRIC 20//40
float	hackie = HACK_FRIC;
		if (Pel_Data[pelvis][14] > 1.5) hackie *= 2;  

		float Leg_K = Pel_Data[pelvis][15];
		float Leg_D = Pel_Data[pelvis][16];
		if ( (Pel_Usage[pelvis] == epelRAPTOR) && (fTotalSpeed2 > 6*Pel_Data[pelvis][14]*Pel_Data[pelvis][14]) )
		{
			Leg_K *= 1.3;
			//LegBounce = 0;
			//AMul = 1;
		}

//		Crouch and Jump (Bipeds only????)...
		if ( iKontrol_Jump[pelvis] != 0 )
		{
			if ( (fTotalSpeed2 > 4*Pel_Data[pelvis][14]*Pel_Data[pelvis][14]) )
			{
				Leg_D *= .1;
				Leg_K *= 2;
				LegBounce = 0;//.1;
			}
			else
			{
				if ( 0 )//iKontrol_Jump[pelvis] == 1 )	//Crouch...
				{
					Leg_K *= .5;//.22;
					//conPhysics << "Jump = 1\n";
				}
				else								//And jump! (Ostensibly iKontrol... == 2)
				{
					Leg_K *= 1;
					Leg_K *= 2;
					LegBounce = 0;
				}
			}
		}
		else if (bKontrol_Krouch[pelvis]) Leg_K *= .22;	//No crouching whilst jump is active...

		if (Pel_Usage[pelvis] == epelQUAD)
		{
//	RIGHT (positive x_l):
//		heel...
		StuffVec( 0, -.1*Pel_Data[pelvis][1], 0, FootPrint );
		Global_Vel( FootVel, FootPrint, RIGHT_FOOT, pelvis );		//Grabs velocoties!
		Foot_to_World_Trans( FootPrint, RIGHT_FOOT, pelvis );		//Global...
		Foot_BC( FootPrint, FootVel, Result, pelvis, RIGHT_FOOT );	//Get the moments...
		Moments[(RIGHT_FOOT+0)] += Result[0];						//Linear...
		Moments[(RIGHT_FOOT+1)] += Result[1];
		Moments[(RIGHT_FOOT+2)] += Result[2];


//	LEFT (negative x_l):
//		heel...
		StuffVec( 0, -.1*Pel_Data[pelvis][1], 0, FootPrint );
		Global_Vel( FootVel, FootPrint, LEFT_FOOT, pelvis );		//Grabs velocoties!
		Foot_to_World_Trans( FootPrint, LEFT_FOOT, pelvis );		//Global...
		Foot_BC( FootPrint, FootVel, Result, pelvis, LEFT_FOOT );	//Get the moments...
		Moments[(LEFT_FOOT+0)] += Result[0];						//Linear...
		Moments[(LEFT_FOOT+1)] += Result[1];
		Moments[(LEFT_FOOT+2)] += Result[2];

//	FRIGHT (positive x_l):
//		heel...
		StuffVec( 0, -.1*Pel_Data[pelvis][1], 0, FootPrint );
		Global_Vel( FootVel, FootPrint, FRIGHT_FOOT, pelvis );		//Grabs velocoties!
		Foot_to_World_Trans( FootPrint, FRIGHT_FOOT, pelvis );		//Global...
		Foot_BC( FootPrint, FootVel, Result, pelvis, FRIGHT_FOOT );	//Get the moments...
		Moments[(FRIGHT_FOOT+0)] += Result[0];						//Linear...
		Moments[(FRIGHT_FOOT+1)] += Result[1];
		Moments[(FRIGHT_FOOT+2)] += Result[2];


//	FLEFT (negative x_l):
//		heel...
		StuffVec( 0, -.1*Pel_Data[pelvis][1], 0, FootPrint );
		Global_Vel( FootVel, FootPrint, FLEFT_FOOT, pelvis );		//Grabs velocoties!
		Foot_to_World_Trans( FootPrint, FLEFT_FOOT, pelvis );		//Global...
		Foot_BC( FootPrint, FootVel, Result, pelvis, FLEFT_FOOT );	//Get the moments...
		Moments[(FLEFT_FOOT+0)] += Result[0];						//Linear...
		Moments[(FLEFT_FOOT+1)] += Result[1];
		Moments[(FLEFT_FOOT+2)] += Result[2];
		}

//	Now, calculate legs and foot-leg interactions...
//	================================================

//	RIGHT:

//		Calculate Eta...
//		----------------
		StuffVec( Pel[pelvis][0][0] - Pel[pelvis][(RIGHT_FOOT+0)][0],			//Need this in l...
				  Pel[pelvis][1][0] - Pel[pelvis][(RIGHT_FOOT+1)][0],
				  Pel[pelvis][2][0] - Pel[pelvis][(RIGHT_FOOT+2)][0],
				  Eta );

		World_to_Local( Eta, pelvis );

		Eta[0] = Pel_Data[pelvis][13] + Eta[0];				//There it is...
		Eta[1] =-Pel_Data[pelvis][46] + Eta[1];
		Eta[2] = Pel_Data[pelvis][54] + Eta[2];

//		Sanitize Eta...
//		---------------
		CopyVec( Eta, EtaHat );
		Normalize( EtaHat, etamag, ietamag );


//		No Blo Up GI...
//		---------------
#define LEGDIST 1.4//1.25
		if (etamag > LEGDIST*Pel_Data[pelvis][14])
		{
			float buttley = LEGDIST*Pel_Data[pelvis][14];
			float temp[3] = { buttley*EtaHat[0], buttley*EtaHat[1], buttley*EtaHat[2] };
		
			temp[0] -= Pel_Data[pelvis][13];
			temp[1] += Pel_Data[pelvis][46];
			temp[2] -= Pel_Data[pelvis][54];

			temp[0] *= -1;
			temp[1] *= -1;
			temp[2] *= -1;

			Local_to_World_Trans( temp, pelvis );

			Pel[pelvis][(RIGHT_FOOT+0)][0] = temp[0];
			Pel[pelvis][(RIGHT_FOOT+1)][0] = temp[1];
			Pel[pelvis][(RIGHT_FOOT+2)][0] = temp[2];

			etamag = buttley;

			CopyVec( EtaHat, temp );
			float sVel[3] = { Pel[pelvis][(RIGHT_FOOT+0)][1], Pel[pelvis][(RIGHT_FOOT+1)][1], Pel[pelvis][(RIGHT_FOOT+2)][1] };
			buttley = Inner( temp, sVel );
			
			Pel[pelvis][(RIGHT_FOOT+0)][1] -= buttley*temp[0];
			Pel[pelvis][(RIGHT_FOOT+1)][1] -= buttley*temp[1];
			Pel[pelvis][(RIGHT_FOOT+2)][1] -= buttley*temp[2];

		}



//		If we are jump-crouching and we are low enough set jump-jumping...
		if (iKontrol_Jump[pelvis] == 1)
		{
			if (etamag < .4*Pel_Data[pelvis][14])
			{
				iKontrol_Jump[pelvis] = 2;
				conPhysics << "Jump change to state 2...\n";
				}
		}


//		Find local f frame couplings to (now sanitized) Eta...
//		------------------------------------------------------
		CopyVec( EtaHat, EtaHat_f );							//(will be) Local to f...
		Local_to_Foot( EtaHat_f, RIGHT_FOOT, pelvis );

		StuffVec( 0, 1, 0, Temp );								//Foot forward...
		Local_to_Foot( Temp, RIGHT_FOOT, pelvis );				//OPTIMIZE: do this
																//computation in l space after you're converted...
		float fRates[3];
		Get_Dirac_Vel( fRates, RIGHT_EULER, pelvis );			//Get the rates in f...

//		TempT[0] =-Pel_Data[pelvis][17]*( asin( EtaHat_f[1] ) + Pel_Data[pelvis][20] )	//Offset...
//				 - Pel_Data[pelvis][22]*fRates[0];

//		TempT[1] = Pel_Data[pelvis][18]*asin( EtaHat_f[0] )		//Offset,
//				 - Pel_Data[pelvis][23]*fRates[1];				//Damping...

		TempT[2] =-Pel_Data[pelvis][19]*( asin( Temp[0] ) )		//Yada yada...
				 - Pel_Data[pelvis][24]*fRates[2];

//		YOU COULD ADD GRAVITY HERE YOU MORON!

		//Moments[(RIGHT_EULER+0)] += TempT[0];
		//Moments[(RIGHT_EULER+1)] += TempT[1];
		Moments[(RIGHT_EULER+2)] += TempT[2];


//		Now calculate the leg extension, apply up and down...
//		-----------------------------------------------------
		phi =-(etamag - Pel_Data[pelvis][21]*Pel_Data[pelvis][14])*Leg_K;
		StuffVec( Pel[pelvis][(RIGHT_FOOT+0)][1] - Pel[pelvis][0][1],
				  Pel[pelvis][(RIGHT_FOOT+1)][1] - Pel[pelvis][1][1],
				  Pel[pelvis][(RIGHT_FOOT+2)][1] - Pel[pelvis][2][1],
				  FootVel );
		World_to_Local( FootVel, pelvis );						//Ok, here 'tis...
		speed = Inner( FootVel, EtaHat );
		phid  = speed*Leg_D;									//This is WITH muscle motion...
		if (speed < 0) phid *= LegBounce;						//This is AGAINST!

		if (phi < 0) phi = 0;									//This is AGAINST!

		if (Pel_Data[pelvis][49]<50) phi *= .1;	//Damage liming...
		phi += phid;

		//float nrg = phid*speed*delta_t;
		float nrg = delta_t*Pel_Data[pelvis][16]*Pel[pelvis][(RIGHT_FOOT+2)][1]*Pel[pelvis][(RIGHT_FOOT+2)][1];
		StuffVec( phi*EtaHat[0], phi*EtaHat[1], phi*EtaHat[2], Result );

/*		CopyVec( Result, Temp );
		Local_to_World( Temp, pelvis );
		Moments[(RIGHT_FOOT+0)] -= Temp[0];					//Eta on foot!
		Moments[(RIGHT_FOOT+1)] -= Temp[1];
		Moments[(RIGHT_FOOT+2)] -= Temp[2];
*/



//	OK, foot control...
//	=-=-=-=-=-=-=-=-=-=
#define Y_RADIUS_COUPLING .4//.5
#define X_RADIUS_COUPLING .3//.5                                        //.5

#define	MAX_RADIUS_RATIO  .6//.4//.5//                              .4//                 .26            //.25
#define PI 3.1415
#define OFFSET_UN .5


		float FT[3] = {Pel_Data[pelvis][13], -Pel_Data[pelvis][46], Pel_Data[pelvis][54] };
		Local_to_World_Trans( FT, pelvis );
		float testt = NPhysImport::fPelTerrainHeight( pelvis, FT[0], FT[1] );

		testt -= FT[2];//Pel[pelvis][2][0];

		FT[0] = Pel_Data[pelvis][13]; FT[1] =-Pel_Data[pelvis][46]; FT[2] = 0; 
		Local_to_World_Trans( FT, pelvis );
		fBackTau = NPhysImport::fPelTerrainHeight( pelvis, FT[0], FT[1] ) - FT[2];

		FT[0] = Pel_Data[pelvis][13]; FT[1] = Pel_Data[pelvis][46]; FT[2] = 0; 
		Local_to_World_Trans( FT, pelvis );
		fFrontTau = NPhysImport::fPelTerrainHeight( pelvis, FT[0], FT[1] ) - FT[2];

		if ( testt < DefaultZ*Pel_Data[pelvis][14] ) testt = DefaultZ*Pel_Data[pelvis][14];
		
		StuffVec( Pel_Data[pelvis][13], -Pel_Data[pelvis][46], Pel_Data[pelvis][54] + testt, FootPrint );

float   tempo[3];

		CopyVec( FootPrint, tempo );
		Local_to_World_Trans( tempo, pelvis );

float	delta[3] = {tempo[0] - History[pelvis][0],
					tempo[1] - History[pelvis][1],
					tempo[2] - History[pelvis][2] };

		History[pelvis][0] = tempo[0];
		History[pelvis][1] = tempo[1];
		History[pelvis][2] = tempo[2];

		World_to_Local( delta, pelvis );

		Global_Vel( FootVel, FootPrint, BODY, pelvis );		//Grabs velocoties!
		World_to_Local( FootVel, pelvis );
		fsign = FootVel[1];

float	R  = Y_RADIUS_COUPLING * fabs( FootVel[1] );
float	Rl = X_RADIUS_COUPLING * fabs( FootVel[0] );
		if (R < .1)	R += .5*fabs(Rates[2]);

		if (Pel_Usage[pelvis] == epelRAPTOR)
		{
			FootPrint[0] -= .05*R;
			if (FootPrint[0] < 0) FootPrint[0] = 0;
		}
 
		if ( R > MAX_RADIUS_RATIO * Pel_Data[pelvis][14] ) R = MAX_RADIUS_RATIO * Pel_Data[pelvis][14];
		if ( Rl> MAX_RADIUS_RATIO * Pel_Data[pelvis][14] ) Rl= MAX_RADIUS_RATIO * Pel_Data[pelvis][14];
		//if ( Rl> 1.8*Pel_Data[pelvis][13] ) Rl = 1.8*Pel_Data[pelvis][13];
		
//		if (fabs(delta[1]) < .003) delta[1] *= 2;
//		if (fabs(delta[0]) < .003) delta[0] *= 2;
		
float	happy = delta[1];

		if ( R > .01 ) rdelta  = -AMul*happy / R;
		if ( Rl >.01 ) rdelta1 = -2*delta[0] / Rl;

		//rdelta  *= 2;//1.25;
		//rdelta1 *= 2;//1.25;

		Pel[pelvis][(RIGHT_PHI+0)][0] += rdelta;
		Pel[pelvis][(RIGHT_PHI+1)][0] += rdelta1;

float	FootDesire[3];
float	z_comp = R*sin(  Pel[pelvis][(RIGHT_PHI+0)][0] ) + Rl*sin( Pel[pelvis][(RIGHT_PHI+1)][0] );
		if (z_comp < 0) z_comp *= .2;
		if (z_comp > .6*Pel_Data[pelvis][14]) z_comp = .6*Pel_Data[pelvis][14];
		StuffVec( FootPrint[0] + Rl*cos( Pel[pelvis][(RIGHT_PHI+1)][0] ),
				  FootPrint[1] + R*cos(  Pel[pelvis][(RIGHT_PHI+0)][0] ),
				  FootPrint[2] + z_comp,
				  FootDesire );
		//if (FootDesire[0] < 0) FootDesire[0] = 0;
		Local_to_World_Trans( FootDesire, pelvis );

#define REAL_FOOT_CONTROL
#ifdef REAL_FOOT_CONTROL

#define FOOT_SPEED 1//2                            //.75//1
#define FOOT_STRENGTH 100*100
			float rbutt[3] = { Pel[pelvis][(RIGHT_FOOT+0)][0], Pel[pelvis][(RIGHT_FOOT+1)][0], Pel[pelvis][(RIGHT_FOOT+2)][0] };
			float rvel[3] = { Pel[pelvis][(RIGHT_FOOT+0)][1], Pel[pelvis][(RIGHT_FOOT+1)][1], Pel[pelvis][(RIGHT_FOOT+2)][1] };
			rvel[0] -= Pel[pelvis][(BODY+0)][1];
			rvel[1] -= Pel[pelvis][(BODY+1)][1];
			rvel[2] -= Pel[pelvis][(BODY+2)][1];
			float rK[3];
			Mass_Solution( FootDesire, rbutt, rvel, 1/Pel_Data[pelvis][32], FOOT_SPEED, FOOT_STRENGTH, rK );

			Moments[(RIGHT_FOOT+0)] += rK[0];
			Moments[(RIGHT_FOOT+1)] += rK[1];
			Moments[(RIGHT_FOOT+2)] += rK[2];

			//Moments[0] -= rK[0];
			//Moments[1] -= rK[1];
			//Moments[2] -= rK[2];

#else

		Pel[pelvis][(RIGHT_FOOT+2)][0] = FootDesire[2];
		Pel[pelvis][(RIGHT_FOOT+0)][0] = FootDesire[0];
		Pel[pelvis][(RIGHT_FOOT+1)][0] = FootDesire[1];

#endif

		StuffVec( 0, 0/*Pel_Data[pelvis][1]*/, 0, Temp );
		Foot_to_World( Temp, RIGHT_FOOT, pelvis );
		UnaryAddVec( FootDesire, Temp );


bool	testme = false;

	if (Pel_Usage[pelvis] == epelQUAD) testme = ( (Toe_Contact_Test( Temp, pelvis ) == 0)  || (etamag > Pel_Data[pelvis][14]) );
	else testme = ( ((BioTag[pelvis][(RIGHT_FOOT+2)] == 0) || (etamag > Pel_Data[pelvis][14])) && ( !Xob[ (Pel_Box_BC[pelvis][RIGHT_FOOT])].bHitAnother ) );

	//if ( ((Toe_Contact_Test( Temp, pelvis ) == 0) || (etamag > Pel_Data[pelvis][14])) && ( !Xob[ (Pel_Box_BC[pelvis][RIGHT_FOOT]) ].bHitAnother )   )
	//if ( (Toe_Contact_Test( Temp, pelvis ) == 0)  || (etamag > Pel_Data[pelvis][14]) )
	if (testme)
	{
		asFootLatch[pelvis][RIGHT_FOOT_LATCH] = false;
	}
	else
	{
		//	Finally, calculate binding to the pelvis...
		//	===========================================

		//		RIGHT FOOT!
				//Moments[3] -= Pel_Data[pelvis][46]*Result[2];
		//		Moments[4] +=-Pel_Data[pelvis][13]*Result[2];			//Store angular...
		//		Moments[5] += Pel_Data[pelvis][13]*Result[1];

		if (Pel_Usage[pelvis] == epelRAPTOR)
		{
			Moments[(RIGHT_FOOT+0)] -= hackie*Pel[pelvis][(RIGHT_FOOT+0)][1];
			Moments[(RIGHT_FOOT+1)] -= hackie*Pel[pelvis][(RIGHT_FOOT+1)][1];
		}

		Local_to_World( Result, pelvis );
		//Moments[0] += Result[0];								//Store spatial...
		//Moments[1] += Result[1];
		Moments[2] += Result[2];
		Moments[(RIGHT_EULER+2)] -= 8*fRates[2];

		//Moments[(RIGHT_EULER+2)] -= 2*Pel_Data[pelvis][24]*Rates[2];

		iJumpTest++;

		// Is this a new foot collision?
		if (!asFootLatch[pelvis][RIGHT_FOOT_LATCH])
		{
			// Yes!  Just starting.  
			// Latch
			asFootLatch[pelvis][RIGHT_FOOT_LATCH] = true;
			NPhysImport::SendFootCollision(pelvis, FootDesire, nrg, 0);
		}
	}

//	LEFT:
//	=-=-=
//		Calculate Eta...
//		----------------
		StuffVec( Pel[pelvis][0][0] - Pel[pelvis][(LEFT_FOOT+0)][0],			//Need this in l...
				  Pel[pelvis][1][0] - Pel[pelvis][(LEFT_FOOT+1)][0],
				  Pel[pelvis][2][0] - Pel[pelvis][(LEFT_FOOT+2)][0],
				  Eta );

		World_to_Local( Eta, pelvis );

		Eta[0] =-Pel_Data[pelvis][13] + Eta[0];				//There it is...
		Eta[1] =-Pel_Data[pelvis][46] + Eta[1];
		Eta[2] = Pel_Data[pelvis][54] + Eta[2];

//		Sanitize Eta...
//		---------------
		CopyVec( Eta, EtaHat );
		Normalize( EtaHat, etamag, ietamag );

		if (etamag > LEGDIST*Pel_Data[pelvis][14])
		{
			float buttley = LEGDIST*Pel_Data[pelvis][14];
			float temp[3] = { buttley*EtaHat[0], buttley*EtaHat[1], buttley*EtaHat[2] };
		
			temp[0] += Pel_Data[pelvis][13];
			temp[1] += Pel_Data[pelvis][46];
			temp[2] -= Pel_Data[pelvis][54];

			temp[0] *= -1;
			temp[1] *= -1;
			temp[2] *= -1;

			Local_to_World_Trans( temp, pelvis );

			Pel[pelvis][(LEFT_FOOT+0)][0] = temp[0];
			Pel[pelvis][(LEFT_FOOT+1)][0] = temp[1];
			Pel[pelvis][(LEFT_FOOT+2)][0] = temp[2];

			etamag = buttley;

			CopyVec( EtaHat, temp );
			float sVel[3] = { Pel[pelvis][(LEFT_FOOT+0)][1], Pel[pelvis][(LEFT_FOOT+1)][1], Pel[pelvis][(LEFT_FOOT+2)][1] };
			buttley = Inner( temp, sVel );
			
			Pel[pelvis][(LEFT_FOOT+0)][1] -= buttley*temp[0];
			Pel[pelvis][(LEFT_FOOT+1)][1] -= buttley*temp[1];
			Pel[pelvis][(LEFT_FOOT+2)][1] -= buttley*temp[2];

		}

//		Find local f frame couplings to (now sanitized) Eta...
//		------------------------------------------------------
		CopyVec( EtaHat, EtaHat_f );							//(will be) Local to f...
		Local_to_Foot( EtaHat_f, LEFT_FOOT, pelvis );

		StuffVec( 0, 1, 0, Temp );								//Foot forward...
		Local_to_Foot( Temp, LEFT_FOOT, pelvis );				//OPTIMIZE: do this
																//computation in l space after you're converted...
		Get_Dirac_Vel( fRates, LEFT_EULER, pelvis );				//Get the rates in f...

//		TempT[0] =-Pel_Data[pelvis][17]*( asin( EtaHat_f[1] ) + Pel_Data[pelvis][20] )	//Offset...
//				 - Pel_Data[pelvis][22]*fRates[0];

//		TempT[1] = Pel_Data[pelvis][18]*asin( EtaHat_f[0] )		//Offset,
//				 - Pel_Data[pelvis][23]*fRates[1];				//Damping...

		TempT[2] =-Pel_Data[pelvis][19]*( asin( Temp[0] ) )		//Yada yada...
				 - Pel_Data[pelvis][24]*fRates[2];

//		YOU COULD ADD GRAVITY HERE YOU MORON!

		//Moments[(LEFT_EULER+0)] += TempT[0];
		//Moments[(LEFT_EULER+1)] += TempT[1];
		Moments[(LEFT_EULER+2)] += TempT[2];


//		Now calculate the leg extension, apply up and down...
//		-----------------------------------------------------
		phi =-(etamag - Pel_Data[pelvis][21]*Pel_Data[pelvis][14])*Leg_K;
		StuffVec( Pel[pelvis][(LEFT_FOOT+0)][1] - Pel[pelvis][0][1],
				  Pel[pelvis][(LEFT_FOOT+1)][1] - Pel[pelvis][1][1],
 				  Pel[pelvis][(LEFT_FOOT+2)][1] - Pel[pelvis][2][1],
				  FootVel );
		World_to_Local( FootVel, pelvis );						//Ok, here 'tis...
		speed = Inner( FootVel, EtaHat );
		phid  = speed*Leg_D;									//This is WITH muscle motion...
		if (speed < 0) phid *= LegBounce;						//This is AGAINST!

		if (phi < 0) phi = 0;									//This is AGAINST!

		//if (Pel_Data[pelvis][49]<50) phi *= .1;	//Damage liming...
		phi += phid;

		//nrg = phid*speed*delta_t;
		nrg = delta_t*Pel_Data[pelvis][16]*Pel[pelvis][(LEFT_FOOT+2)][1]*Pel[pelvis][(LEFT_FOOT+2)][1];
		StuffVec( phi*EtaHat[0], phi*EtaHat[1], phi*EtaHat[2], Result );


/*		CopyVec( Result, Temp );
		Local_to_World( Temp, pelvis );
		Moments[(LEFT_FOOT+0)] -= Temp[0];					//Eta on foot!
		Moments[(LEFT_FOOT+1)] -= Temp[1];
		Moments[(LEFT_FOOT+2)] -= Temp[2];
*/

//	OK, foot control...
//	=-=-=-=-=-=-=-=-=-=

		float lFT[3] = {-Pel_Data[pelvis][13], -Pel_Data[pelvis][46], Pel_Data[pelvis][54] };
		Local_to_World_Trans( lFT, pelvis );
		testt = NPhysImport::fPelTerrainHeight( pelvis, lFT[0], lFT[1] );

		testt -= lFT[2];//Pel[pelvis][2][0];
		//if (-.75*Pel_Data[pelvis][14] > testt) testt = -.75*Pel_Data[pelvis][14];
		if (DefaultZ*Pel_Data[pelvis][14] > testt) testt = DefaultZ*Pel_Data[pelvis][14];

		StuffVec( -Pel_Data[pelvis][13], -Pel_Data[pelvis][46], Pel_Data[pelvis][54] + testt, FootPrint );

		//StuffVec( -Pel_Data[pelvis][13], 0, -.75*Pel_Data[pelvis][14], FootPrint );
	
		Global_Vel( FootVel, FootPrint, BODY, pelvis );		//Grabs velocoties!
		World_to_Local( FootVel, pelvis );

		//FootVel[1] = -3;	FootVel[0] = FootVel[2] = 0;

		R  = Y_RADIUS_COUPLING * fabs( FootVel[1] );
		Rl = X_RADIUS_COUPLING * fabs( FootVel[0] );
		if (R < .1)	R += .15*fabs(Rates[2]);

		if (Pel_Usage[pelvis] == epelRAPTOR)
		{
			FootPrint[0] += .05*R;
			if (FootPrint[0] > 0) FootPrint[0] = 0;
		}

		if ( R > MAX_RADIUS_RATIO * Pel_Data[pelvis][14] ) R = MAX_RADIUS_RATIO * Pel_Data[pelvis][14];
		if ( Rl> MAX_RADIUS_RATIO * Pel_Data[pelvis][14] ) Rl= MAX_RADIUS_RATIO * Pel_Data[pelvis][14];
		//if ( Rl> 1.8*Pel_Data[pelvis][13] ) Rl =1.8*Pel_Data[pelvis][13];

float	xsgn = 1, ysgn = 1;
		if (FootVel[1]*fsign < 0) ysgn *= -1;
		//if (FootVel[0] < 0) xsgn *= -1;

//		if (Pel_Data[pelvis][49] <= 0) R = Rl = 0;

		z_comp = R*sin( Pel[pelvis][(RIGHT_PHI+0)][0] + PI ) + Rl*sin( Pel[pelvis][(RIGHT_PHI+1)][0] + PI );
		if (z_comp < 0) z_comp *= .2;
		if (z_comp > .6*Pel_Data[pelvis][14]) z_comp = .6*Pel_Data[pelvis][14];

		StuffVec( FootPrint[0] + xsgn*Rl*cos( Pel[pelvis][(RIGHT_PHI+1)][0] + PI ),
				  FootPrint[1] + ysgn*R*cos( Pel[pelvis][(RIGHT_PHI+0)][0] + PI ),
				  FootPrint[2] + z_comp,
				  FootDesire );
		Local_to_World_Trans( FootDesire, pelvis );

		//Pel[pelvis][(LEFT_FOOT+0)][0] = FootDesire[0];
		//Pel[pelvis][(LEFT_FOOT+1)][0] = FootDesire[1];

#ifdef REAL_FOOT_CONTROL

			float lbutt[3] = { Pel[pelvis][(LEFT_FOOT+0)][0], Pel[pelvis][(LEFT_FOOT+1)][0], Pel[pelvis][(LEFT_FOOT+2)][0] };
			float lvel[3] = { Pel[pelvis][(LEFT_FOOT+0)][1], Pel[pelvis][(LEFT_FOOT+1)][1], Pel[pelvis][(LEFT_FOOT+2)][1] };
			lvel[0] -= Pel[pelvis][(BODY+0)][1];
			lvel[1] -= Pel[pelvis][(BODY+1)][1];
			lvel[2] -= Pel[pelvis][(BODY+2)][1];
			float lK[3];
			Mass_Solution( FootDesire, lbutt, lvel, 1/Pel_Data[pelvis][32], FOOT_SPEED, FOOT_STRENGTH, lK );

			Moments[(LEFT_FOOT+0)] += lK[0];
			Moments[(LEFT_FOOT+1)] += lK[1];
			Moments[(LEFT_FOOT+2)] += lK[2];

#else

		Pel[pelvis][(LEFT_FOOT+2)][0] = FootDesire[2];
		Pel[pelvis][(LEFT_FOOT+0)][0] = FootDesire[0];
		Pel[pelvis][(LEFT_FOOT+1)][0] = FootDesire[1];

#endif

		StuffVec( 0, 0/*Pel_Data[pelvis][1]*/, 0, Temp );
		Foot_to_World( Temp, LEFT_FOOT, pelvis );
		UnaryAddVec( FootDesire, Temp );


	if (Pel_Usage[pelvis] == epelQUAD)	testme = ( (Toe_Contact_Test( Temp, pelvis ) == 0)  || (etamag > Pel_Data[pelvis][14]) );
	else testme = ( ((BioTag[pelvis][(LEFT_FOOT+2)] == 0) || (etamag > Pel_Data[pelvis][14])) && ( !Xob[ (Pel_Box_BC[pelvis][LEFT_FOOT])].bHitAnother ) );

	//if ( ((BioTag[pelvis][(LEFT_FOOT+2)] == 0) || (etamag > Pel_Data[pelvis][14])) && ( !Xob[ (Pel_Box_BC[pelvis][LEFT_FOOT]) ].bHitAnother )   )
	//if ( ((Toe_Contact_Test( Temp, pelvis ) == 0) || (etamag > Pel_Data[pelvis][14])) && ( !Xob[ (Pel_Box_BC[pelvis][LEFT_FOOT]) ].bHitAnother )   )
	if (testme)
	{
		// No longer contacting the ground.
		asFootLatch[pelvis][LEFT_FOOT_LATCH] = false;
	}

	else  
	{

		//	Finally, calculate binding to the pelvis...
		//	===========================================
		//		LEFT FOOT!
				//Moments[3] -= Pel_Data[pelvis][46]*Result[2];
		//		Moments[4] += Pel_Data[pelvis][13]*Result[2];			//Store angular...
		//		Moments[5] +=-Pel_Data[pelvis][13]*Result[1];

		if (Pel_Usage[pelvis] == epelRAPTOR)
		{
			Moments[(LEFT_FOOT+0)] -= hackie*Pel[pelvis][(LEFT_FOOT+0)][1];
			Moments[(LEFT_FOOT+1)] -= hackie*Pel[pelvis][(LEFT_FOOT+1)][1];
		}


		Local_to_World( Result, pelvis );
		//Moments[0] += Result[0];								//Store spatial...
		//Moments[1] += Result[1];
		Moments[2] += Result[2];
		Moments[(LEFT_EULER+2)] -= 8*fRates[2];

		iJumpTest++;

		// Is this a new foot collision?
		if (!asFootLatch[pelvis][LEFT_FOOT_LATCH])
		{
			// Yes!  Just starting.  

			// Latch
			asFootLatch[pelvis][LEFT_FOOT_LATCH] = true;
			NPhysImport::SendFootCollision(pelvis, FootDesire, nrg, 0);
		}
	}






//	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	QUADS
//	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	


#define QUADROPHENIA
#ifdef QUADROPHENIA

	if (Pel_Usage[pelvis] == epelQUAD)
	{

//		Front RIGHT:

//		Calculate Eta...
//		----------------
		StuffVec( Pel[pelvis][0][0] - Pel[pelvis][(FRIGHT_FOOT+0)][0],			//Need this in l...
				  Pel[pelvis][1][0] - Pel[pelvis][(FRIGHT_FOOT+1)][0],
				  Pel[pelvis][2][0] - Pel[pelvis][(FRIGHT_FOOT+2)][0],
				  Eta );

		World_to_Local( Eta, pelvis );

		Eta[0] = Pel_Data[pelvis][45] + Eta[0];				//There it is...
		Eta[1] = Pel_Data[pelvis][46] + Eta[1];
		Eta[2] = Pel_Data[pelvis][53] + Eta[2];

//		Sanitize Eta...
//		---------------
		CopyVec( Eta, EtaHat );
		Normalize( EtaHat, etamag, ietamag );


		//conPhysics << "Eta - " << etamag << ", " << Pel_Data[pelvis][50] << "\n";


//		Find local f frame couplings to (now sanitized) Eta...
//		------------------------------------------------------
		CopyVec( EtaHat, EtaHat_f );							//(will be) Local to f...
		Local_to_Foot( EtaHat_f, FRIGHT_FOOT, pelvis );

		StuffVec( 0, 1, 0, Temp );								//Foot forward...
		Local_to_Foot( Temp, FRIGHT_FOOT, pelvis );				//OPTIMIZE: do this
																//computation in l space after you're converted...
		Get_Dirac_Vel( fRates, FRIGHT_EULER, pelvis );			//Get the rates in f...

//		TempT[0] =-Pel_Data[pelvis][17]*( asin( EtaHat_f[1] ) + Pel_Data[pelvis][20] )	//Offset...
//				 - Pel_Data[pelvis][22]*fRates[0];

//		TempT[1] = Pel_Data[pelvis][18]*asin( EtaHat_f[0] )		//Offset,
//				 - Pel_Data[pelvis][23]*fRates[1];				//Damping...

		TempT[2] =-Pel_Data[pelvis][19]*( asin( Temp[0] ) )		//Yada yada...
				 - Pel_Data[pelvis][24]*fRates[2];

//		YOU COULD ADD GRAVITY HERE YOU MORON!

		//Moments[(FRIGHT_EULER+0)] += TempT[0];
		//Moments[(FRIGHT_EULER+1)] += TempT[1];
		Moments[(FRIGHT_EULER+2)] += TempT[2];


//		Now calculate the leg extension, apply up and down...
//		-----------------------------------------------------
		phi =-(etamag - Pel_Data[pelvis][21]*Pel_Data[pelvis][50])*Leg_K;
		StuffVec( Pel[pelvis][(FRIGHT_FOOT+0)][1] - Pel[pelvis][0][1],
				  Pel[pelvis][(FRIGHT_FOOT+1)][1] - Pel[pelvis][1][1],
				  Pel[pelvis][(FRIGHT_FOOT+2)][1] - Pel[pelvis][2][1],
				  FootVel );
		World_to_Local( FootVel, pelvis );						//Ok, here 'tis...
		speed = Inner( FootVel, EtaHat );
		phid  = speed*Leg_D;									//This is WITH muscle motion...
		if (speed < 0) phid *= LegBounce;						//This is AGAINST!

		if (phi < 0) phi = 0;									//This is AGAINST!

		//if (Pel_Data[pelvis][49]<50) phi *= .1;	//Damage liming...
		phi += phid;

		//nrg = phid*speed*delta_t;
		float nrg = delta_t*Pel_Data[pelvis][16]*Pel[pelvis][(FRIGHT_FOOT+2)][1]*Pel[pelvis][(FRIGHT_FOOT+2)][1];
		StuffVec( phi*EtaHat[0], phi*EtaHat[1], phi*EtaHat[2], Result );

/*		CopyVec( Result, Temp );
		Local_to_World( Temp, pelvis );
		Moments[(FRIGHT_FOOT+0)] -= Temp[0];					//Eta on foot!
		Moments[(FRIGHT_FOOT+1)] -= Temp[1];
		Moments[(FRIGHT_FOOT+2)] -= Temp[2];
*/


//	OK, foot control...
//	=-=-=-=-=-=-=-=-=-=
		float rat_tail = 1;//Pel_Data[pelvis][50]/Pel_Data[pelvis][14];

		FT[0] = Pel_Data[pelvis][45]; FT[1] = Pel_Data[pelvis][46]; FT[2] = Pel_Data[pelvis][53];
		Local_to_World_Trans( FT, pelvis );
		testt = NPhysImport::fPelTerrainHeight( pelvis, FT[0], FT[1] );


		//fFrontTau = testt;
		testt -= FT[2];//Pel[pelvis][2][0];

		if (DefaultZ*Pel_Data[pelvis][50] > testt) testt = DefaultZ*Pel_Data[pelvis][50];
		
		StuffVec( Pel_Data[pelvis][45], Pel_Data[pelvis][46], Pel_Data[pelvis][53] + testt, FootPrint );

		Global_Vel( FootVel, FootPrint, BODY, pelvis );		//Grabs velocoties!
		World_to_Local( FootVel, pelvis );

		R  = rat_tail*Y_RADIUS_COUPLING * fabs( FootVel[1] );
		Rl = rat_tail*X_RADIUS_COUPLING * fabs( FootVel[0] );
		//R  = Y_RADIUS_COUPLING * fabs( FootVel[1] );
		//Rl = X_RADIUS_COUPLING * fabs( FootVel[0] );
		//if (R < .1)	R += .5*fabs(Rates[2]);

		if ( R > MAX_RADIUS_RATIO * Pel_Data[pelvis][50] ) R = MAX_RADIUS_RATIO * Pel_Data[pelvis][50];
		if ( Rl> 1.8*Pel_Data[pelvis][45] ) Rl = 1.8*Pel_Data[pelvis][45];
		
		z_comp = R*sin(  Pel[pelvis][(RIGHT_PHI+0)][0] + PI + OFFSET_UN) + Rl*sin( Pel[pelvis][(RIGHT_PHI+1)][0] + PI + OFFSET_UN);
		if (z_comp < 0) z_comp *= .2;
		if (z_comp > .7*Pel_Data[pelvis][50]) z_comp = .7*Pel_Data[pelvis][50];
		StuffVec( FootPrint[0] + Rl*cos( Pel[pelvis][(RIGHT_PHI+1)][0] + PI + OFFSET_UN),
				  FootPrint[1] + R*cos(  Pel[pelvis][(RIGHT_PHI+0)][0] + PI + OFFSET_UN),
  	  			  //FootPrint[2] + R*sin(  Pel[pelvis][(RIGHT_PHI+0)][0] + PI + OFFSET_UN) + Rl*sin( Pel[pelvis][(RIGHT_PHI+1)][0] + PI + OFFSET_UN),
				  FootPrint[2] + z_comp,
				  FootDesire );
		Local_to_World_Trans( FootDesire, pelvis );

		StuffVec( 0, 0/*Pel_Data[pelvis][1]*/, 0, Temp );
		Foot_to_World( Temp, FRIGHT_FOOT, pelvis );
		UnaryAddVec( FootDesire, Temp );

		//Pel[pelvis][(FRIGHT_FOOT+0)][0] = FootDesire[0];
		//Pel[pelvis][(FRIGHT_FOOT+1)][0] = FootDesire[1];

			float frbutt[3] = { Pel[pelvis][(FRIGHT_FOOT+0)][0], Pel[pelvis][(FRIGHT_FOOT+1)][0], Pel[pelvis][(FRIGHT_FOOT+2)][0] };
			float frvel[3]  = { Pel[pelvis][(FRIGHT_FOOT+0)][1], Pel[pelvis][(FRIGHT_FOOT+1)][1], Pel[pelvis][(FRIGHT_FOOT+2)][1] };
			frvel[0] -= Pel[pelvis][(BODY+0)][1];
			frvel[1] -= Pel[pelvis][(BODY+1)][1];
			frvel[2] -= Pel[pelvis][(BODY+2)][1];
			float frK[3];
			Mass_Solution( FootDesire, frbutt, frvel, 1/Pel_Data[pelvis][32], FOOT_SPEED, FOOT_STRENGTH, frK );

			Moments[(FRIGHT_FOOT+0)] += frK[0];
			Moments[(FRIGHT_FOOT+1)] += frK[1];
			Moments[(FRIGHT_FOOT+2)] += frK[2];
	
	if ( (Toe_Contact_Test( Temp, pelvis ) == 0) || (etamag > Pel_Data[pelvis][50]) )
	{
		asFootLatch[pelvis][FRIGHT_FOOT_LATCH] = false;
	}
	else
	{
		//	Finally, calculate binding to the pelvis...
		//	===========================================

		//	FRIGHT FOOT!
			//Moments[3] += Pel_Data[pelvis][46]*Result[2];
		//	Moments[4] +=-Pel_Data[pelvis][13]*Result[2];			//Store angular...
		//	Moments[5] += Pel_Data[pelvis][13]*Result[1];

		Local_to_World( Result, pelvis );
		//Moments[0] += Result[0];								//Store spatial...
		//Moments[1] += Result[1];
		Moments[2] += Result[2];

		iJumpTest++;

		// Is this a new foot collision?
		if (!asFootLatch[pelvis][FRIGHT_FOOT_LATCH])
		{
			// Yes!  Just starting.  
			// Latch
			asFootLatch[pelvis][FRIGHT_FOOT_LATCH] = true;
			NPhysImport::SendFootCollision(pelvis, FootDesire, nrg, 0);
		}
	}


//	LEFT:
//	=-=-=
//		Calculate Eta...
//		----------------
		StuffVec( Pel[pelvis][0][0] - Pel[pelvis][(FLEFT_FOOT+0)][0],			//Need this in l...
				  Pel[pelvis][1][0] - Pel[pelvis][(FLEFT_FOOT+1)][0],
				  Pel[pelvis][2][0] - Pel[pelvis][(FLEFT_FOOT+2)][0],
				  Eta );

		World_to_Local( Eta, pelvis );

		Eta[0] =-Pel_Data[pelvis][45] + Eta[0];				//There it is...
		Eta[1] = Pel_Data[pelvis][46] + Eta[1];
		Eta[2] = Pel_Data[pelvis][53] + Eta[2];

//		Sanitize Eta...
//		---------------
		CopyVec( Eta, EtaHat );
		Normalize( EtaHat, etamag, ietamag );

//		Find local f frame couplings to (now sanitized) Eta...
//		------------------------------------------------------
		CopyVec( EtaHat, EtaHat_f );							//(will be) Local to f...
		Local_to_Foot( EtaHat_f, FLEFT_FOOT, pelvis );

		StuffVec( 0, 1, 0, Temp );								//Foot forward...
		Local_to_Foot( Temp, FLEFT_FOOT, pelvis );				//OPTIMIZE: do this
																//computation in l space after you're converted...
		Get_Dirac_Vel( fRates, FLEFT_EULER, pelvis );				//Get the rates in f...

//		TempT[0] =-Pel_Data[pelvis][17]*( asin( EtaHat_f[1] ) + Pel_Data[pelvis][20] )	//Offset...
//				 - Pel_Data[pelvis][22]*fRates[0];

//		TempT[1] = Pel_Data[pelvis][18]*asin( EtaHat_f[0] )		//Offset,
//				 - Pel_Data[pelvis][23]*fRates[1];				//Damping...

		TempT[2] =-Pel_Data[pelvis][19]*( asin( Temp[0] ) )		//Yada yada...
				 - Pel_Data[pelvis][24]*fRates[2];

//		YOU COULD ADD GRAVITY HERE YOU MORON!

		//Moments[(FLEFT_EULER+0)] += TempT[0];
		//Moments[(FLEFT_EULER+1)] += TempT[1];
		Moments[(FLEFT_EULER+2)] += TempT[2];


//		Now calculate the leg extension, apply up and down...
//		-----------------------------------------------------
		phi =-(etamag - Pel_Data[pelvis][21]*Pel_Data[pelvis][50])*Leg_K;
		StuffVec( Pel[pelvis][(FLEFT_FOOT+0)][1] - Pel[pelvis][0][1],
				  Pel[pelvis][(FLEFT_FOOT+1)][1] - Pel[pelvis][1][1],
 				  Pel[pelvis][(FLEFT_FOOT+2)][1] - Pel[pelvis][2][1],
				  FootVel );
		World_to_Local( FootVel, pelvis );						//Ok, here 'tis...
		speed = Inner( FootVel, EtaHat );
		phid  = speed*Leg_D;									//This is WITH muscle motion...
		if (speed < 0) phid *= LegBounce;						//This is AGAINST!

		if (phi < 0) phi = 0;									//This is AGAINST!

		if (Pel_Data[pelvis][49]<50) phi *= .1;	//Damage liming...
		phi += phid;

		//nrg = phid*speed*delta_t;
		nrg = delta_t*Pel_Data[pelvis][16]*Pel[pelvis][(FLEFT_FOOT+2)][1]*Pel[pelvis][(FLEFT_FOOT+2)][1];
		StuffVec( phi*EtaHat[0], phi*EtaHat[1], phi*EtaHat[2], Result );

/*		CopyVec( Result, Temp );
		Local_to_World( Temp, pelvis );
		Moments[(FLEFT_FOOT+0)] -= Temp[0];					//Eta on foot!
		Moments[(FLEFT_FOOT+1)] -= Temp[1];
		Moments[(FLEFT_FOOT+2)] -= Temp[2];
*/

//	OK, foot control...
//	=-=-=-=-=-=-=-=-=-=

		lFT[0] =-Pel_Data[pelvis][45];	lFT[1] = Pel_Data[pelvis][46];	lFT[2] = Pel_Data[pelvis][53];
		Local_to_World_Trans( lFT, pelvis );
		testt = NPhysImport::fPelTerrainHeight( pelvis, lFT[0], lFT[1] );

		testt -= FT[2];//Pel[pelvis][2][0];
		if (DefaultZ*Pel_Data[pelvis][50] > testt) testt = DefaultZ*Pel_Data[pelvis][50];

		StuffVec( -Pel_Data[pelvis][45], Pel_Data[pelvis][46], testt + Pel_Data[pelvis][53], FootPrint );
		
		Global_Vel( FootVel, FootPrint, BODY, pelvis );		//Grabs velocoties!
		World_to_Local( FootVel, pelvis );

		R  = rat_tail*Y_RADIUS_COUPLING * fabs( FootVel[1] );
		Rl = rat_tail*X_RADIUS_COUPLING * fabs( FootVel[0] );
		//R  = Y_RADIUS_COUPLING * fabs( FootVel[1] );
		//Rl = X_RADIUS_COUPLING * fabs( FootVel[0] );
		//if (R < .1)	R += .15*fabs(Rates[2]);


		//if ( R > MAX_RADIUS_RATIO * Pel_Data[pelvis][50] ) R = MAX_RADIUS_RATIO * Pel_Data[pelvis][50];
		if ( R > MAX_RADIUS_RATIO * Pel_Data[pelvis][50] ) R = MAX_RADIUS_RATIO * Pel_Data[pelvis][50];
		if ( Rl> 1.8*Pel_Data[pelvis][45] ) Rl = 1.8*Pel_Data[pelvis][45];


float	xsgn = 1, ysgn = 1;
		//if (FootVel[1] < 0) ysgn *= -1;
		//if (FootVel[0] < 0) xsgn *= -1;

		z_comp = R*sin( Pel[pelvis][(RIGHT_PHI+0)][0] + OFFSET_UN ) + Rl*sin( Pel[pelvis][(RIGHT_PHI+1)][0] + OFFSET_UN );
		if (z_comp < 0) z_comp *= .2;
		if (z_comp > .7*Pel_Data[pelvis][50]) z_comp = .7*Pel_Data[pelvis][50];
		StuffVec( FootPrint[0] + xsgn*Rl*cos( Pel[pelvis][(RIGHT_PHI+1)][0] + OFFSET_UN),
				  FootPrint[1] + ysgn*R*cos(  Pel[pelvis][(RIGHT_PHI+0)][0] + OFFSET_UN ),
 				  FootPrint[2] + z_comp,
 				  //FootPrint[2] + R*sin( Pel[pelvis][(RIGHT_PHI+0)][0] ) + Rl*sin( Pel[pelvis][(RIGHT_PHI+1)][0] ),
				  FootDesire );
		Local_to_World_Trans( FootDesire, pelvis );

		StuffVec( 0, 0/*Pel_Data[pelvis][1]*/, 0, Temp );
		Foot_to_World( Temp, FLEFT_FOOT, pelvis );
		UnaryAddVec( FootDesire, Temp );

		//Pel[pelvis][(FLEFT_FOOT+0)][0] = FootDesire[0];
		//Pel[pelvis][(FLEFT_FOOT+1)][0] = FootDesire[1];

			float flbutt[3] = { Pel[pelvis][(FLEFT_FOOT+0)][0], Pel[pelvis][(FLEFT_FOOT+1)][0], Pel[pelvis][(FLEFT_FOOT+2)][0] };
			float flvel[3]  = { Pel[pelvis][(FLEFT_FOOT+0)][1], Pel[pelvis][(FLEFT_FOOT+1)][1], Pel[pelvis][(FLEFT_FOOT+2)][1] };
			flvel[0] -= Pel[pelvis][(BODY+0)][1];
			flvel[1] -= Pel[pelvis][(BODY+1)][1];
			flvel[2] -= Pel[pelvis][(BODY+2)][1];
			float flK[3];
			Mass_Solution( FootDesire, flbutt, flvel, 1/Pel_Data[pelvis][32], FOOT_SPEED, FOOT_STRENGTH, flK );

			Moments[(FLEFT_FOOT+0)] += flK[0];
			Moments[(FLEFT_FOOT+1)] += flK[1];
			Moments[(FLEFT_FOOT+2)] += flK[2];

	if ( (Toe_Contact_Test( Temp, pelvis ) == 0) || (etamag > Pel_Data[pelvis][50]) )
	{
		// Unlatch foot collision
		asFootLatch[pelvis][FLEFT_FOOT_LATCH] = false;
	}
	else
	{
		//	Finally, calculate binding to the pelvis...
		//	===========================================


		//	FLEFT FOOT!
			//Moments[3] += Pel_Data[pelvis][46]*Result[2];
		//	Moments[4] += Pel_Data[pelvis][13]*Result[2];			//Store angular...
		//	Moments[5] +=-Pel_Data[pelvis][13]*Result[1];


		Local_to_World( Result, pelvis );
		//Moments[0] += Result[0];								//Store spatial...
		//Moments[1] += Result[1];
		Moments[2] += Result[2];

		iJumpTest++;

		// Is this a new foot collision?
		if (!asFootLatch[pelvis][FLEFT_FOOT_LATCH])
		{
			// Yes!  Just starting.  
			// Latch
			asFootLatch[pelvis][FLEFT_FOOT_LATCH] = true;
			NPhysImport::SendFootCollision(pelvis, FootDesire, nrg, 0);
		}
	}





	}//Test for Quads...

//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	QUADS
//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif


//	Turn off jump if no feet are touching...
	if ( (iJumpTest == 0) && (iKontrol_Jump[pelvis] == 2) )	iKontrol_Jump[pelvis] = 0;
	if ( iJumpTest == 0 ) {
		Pel[pelvis][(RIGHT_PHI+0)][0] -= .75*rdelta;
		Pel[pelvis][(RIGHT_PHI+1)][0] -= .75*rdelta1;
	}


	}//Test to see if its a dino to save time...



//	Some pelvic games...
//	--------------------
//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++BULLSHIT CODE MUST DIE
#define	LINEAR_DAMP 50//50
#define ANGULAR_REST 50
#define ANGULAR_DAMP 15
float	adamp = ANGULAR_DAMP;

//#define CATCH_DAMP 40// 50//70                 //50//150// 0 //150
//#define CATCH_REST 800 //1000// 0 //1000
#define CATCH_DAMP 0//50//150
#define CATCH_REST 0//1000

#define MOVE_MULT 10
#define JUMP_FACTOR 2.1
#define	CROUCH_PERCENT .4//.6//.5
#define	CONTROL_CROUCH .5

#define CONTACT_VAL 0//-10       //-5

//		Here we drive the model...
float	Drive[3] = {0,0,0};

		Drive[0] = Kontrol[pelvis][0]*MOVE_MULT;							//MOVE:  0-10
		Drive[1] = Kontrol[pelvis][1]*MOVE_MULT;

//		Here is the new...
float	height_z = .85*Pel_Data[pelvis][14];
float	damp_z	 = CATCH_DAMP;


		if (Pel_Usage[pelvis] == epelRAPTOR || Pel_Usage[pelvis] == epelQUAD)	
		{

			Drive[0] *= 3;
			Drive[1] *= 3;


			World_to_Local( Drive, pelvis );

			//conPhysics << Drive[0] << ", " << Drive[1] << "\n";

#define STEP_SPEED 30
#define	SIDESTEP_SPEED 30//10

			if (Drive[0] > SIDESTEP_SPEED) Drive[0] = SIDESTEP_SPEED;
			if (Drive[0] <-SIDESTEP_SPEED) Drive[0] =-SIDESTEP_SPEED;

			if (Drive[1] > STEP_SPEED) Drive[1] = STEP_SPEED;
			if (Drive[1] <-STEP_SPEED) Drive[1] =-STEP_SPEED;

			//Drive[0] = 10;
			//Drive[1] =  0;


			Local_to_World( Drive, pelvis );

			//Drive[0] *= MOVE_MULT;
			//Drive[1] *= MOVE_MULT;

//	float temp[3] = {0, 30, 0};
//	Local_to_World( temp, pelvis );
//	Drive[0] = temp[0]*MOVE_MULT;
//	Drive[1] = temp[1]*MOVE_MULT;
	//Moments[5] =-5;

			float comp = Pel_Data[pelvis][14];
				  if (comp < 1.3) comp = 1.3;
				  if (comp > 2.0) comp = 2.0;

//			Drive[0] *= comp*1.2*4;
//			Drive[1] *= comp*1.2*4;
			Drive[0] *= 1.1*comp*1.2*4;
			Drive[1] *= 1.1*comp*1.2*4;

			//For CONFIG TEST ONLY.
			if ( Pel_Usage[pelvis] == epelQUAD )
			{
				Drive[0] *= .4;
				Drive[1] *= .4;
				adamp *= 2;
			}

		}



//	Only for Humans for Now...
		if (Pel_Usage[pelvis] == epelHUMAN)
		{
			height_z = Pel_Data[pelvis][14];
			damp_z = 150;//150;
			Drive[0] *= 2.45;//3;//2.45;//3;               //3.5;
			Drive[1] *= 2.45;//3;//2.45;//3;               //3.5;

//			Check for crouching...
			//if (Pelvis_Set_Crouch > 0)
			if (bKontrol_Krouch[pelvis])
			{
				height_z *= CROUCH_PERCENT;
				Drive[0] *= CONTROL_CROUCH;
				Drive[1] *= CONTROL_CROUCH;
			}

		}

float	slohp = 0;
float	testmul = 100;

int	feetwet = Pel_Box_BC[pelvis][RIGHT_FOOT];



		if (Pel_Usage[pelvis] == epelHUMAN) testmul *= 2;
//		if (test > 0) testmul = .1;




		if (Pel_Usage[pelvis] == epelRAPTOR || Pel_Usage[pelvis] == epelQUAD)	
		{
			Drive[0] += -.5*testmul*Pel[pelvis][0][1];
			Drive[1] += -.5*testmul*Pel[pelvis][1][1];
		}
		else	//Human...
		{



		//Get data...
float		norm[3];
float		trr_height = NPhysImport::fPelTerrainHeight( pelvis, Pel[pelvis][0][0], Pel[pelvis][1][0], norm );
			slohp = norm[0]*norm[0] + norm[1]*norm[1];
int			feetwet = Pel_Box_BC[pelvis][RIGHT_FOOT];

float		TMoments[2] = {0,0};

		//Is there pressure...
		if ( BioTag[pelvis][(RIGHT_FOOT+2)] > CONTACT_VAL )
		{


			//if CI
			if ( (fabs(Kontrol[pelvis][0]) > 0) || (fabs(Kontrol[pelvis][1]) > 0) )
			{
				float butley[3] = {Kontrol[pelvis][0], Kontrol[pelvis][1], 0};
				float yet, again;

				Normalize( butley, yet, again );
				float weenie[3] = {Pel[pelvis][0][1], Pel[pelvis][1][1], 0};
				yet = Inner( butley, weenie );
				float shot[3] = {yet*butley[0], yet*butley[1], 0};  //Trans Comp...
				float dead[3] = {weenie[0] - shot[0], weenie[1] - shot[1], 0};

				TMoments[0] += .5*Drive[0] - testmul*( 2*dead[0] + shot[0] );
				TMoments[1] += .5*Drive[1] - testmul*( 2*dead[1] + shot[1] );

				//conPhysics << "Adding Kontrol inputs...\n";
				//TMoments[0] += .5*Drive[0] - .75*testmul*Pel[pelvis][(BODY+0)][1];
				//TMoments[1] += .5*Drive[1] - .75*testmul*Pel[pelvis][(BODY+1)][1];

			}

			//else HD
			else if ( (slohp < .31) || (Xob[feetwet].bHitAnother) )	//Normal Drag...
			{
				Moments[(BODY+0)] += -5*testmul*Pel[pelvis][(BODY+0)][1];
				Moments[(BODY+1)] += -5*testmul*Pel[pelvis][(BODY+1)][1];
				//conPhysics << "NoControl but pressure...\n";
			}


		}//End of pressure plate test...
		else
		{
			TMoments[0] += .05*Drive[0] - .05*testmul*Pel[pelvis][(BODY+0)][1];
			TMoments[1] += .05*Drive[1] - .05*testmul*Pel[pelvis][(BODY+1)][1];

			//conPhysics << "!!!!!!!!!!No pressure, allowed slope...\n";
		}

		//Deal with slopes
		if ( (slohp >= .31) && !Xob[feetwet].bHitAnother )//(Pel[pelvis][(RIGHT_FOOT+2)][0] - trr_height < .4) )
		{

			//conPhysics << "Slohp: " << slohp << "\n";
			norm[2] = 0;
			float poo, bah;
			Normalize( norm, poo, bah );

			poo = norm[0]*TMoments[0] + norm[1]*TMoments[1];
			if (poo < 0)
			{
				//conPhysics << "Subtracting Due to Slope...\n";
				TMoments[0] -= poo*norm[0];
				TMoments[1] -= poo*norm[1];
			}
		}

		Moments[(BODY+0)] += TMoments[0];
		Moments[(BODY+1)] += TMoments[1];



		}//Humam...

		
   
//	Only for Humans for Now...
extern bool	OKtoJUMP;

		if (Pel_Usage[pelvis] == epelHUMAN)
		{

			if ( (Pelvis_Jump[2]) && (Pel_Data[pelvis][49]>0) )
			{
				//if ( (Xob[feetwet].Wz != 0) && ((slohp < .31) || ( Xob[feetwet].bHitAnother )) )
				//if ( (Xob[feetwet].bHitAnother) && (fabs( rat ) > 1) )

				if (   ( (Xob[feetwet].Wz != 0) && (slohp < .31) )
					|| ( OKtoJUMP )
				   )
				
				{
					Pel[pelvis][(RIGHT_FOOT+0)][1] = Pel[pelvis][(BODY+0)][1] += 4 * Pelvis_Jump[0];
					Pel[pelvis][(RIGHT_FOOT+1)][1] = Pel[pelvis][(BODY+1)][1] += 4 * Pelvis_Jump[1];
					Pel[pelvis][(RIGHT_FOOT+2)][1] = Pel[pelvis][(BODY+2)][1]  = 4 * Pelvis_Jump[2];
					Pelvis_Jump[0] = Pelvis_Jump[1] = Pelvis_Jump[2] = 0;
					if (Pelvis_Jump_Voluntary)
						NPhysImport::MakePlayerJumpNoise();
					BioTag[pelvis][(RIGHT_FOOT+2)] = 0;
				}
				else conPhysics << "JumpFailure: " << Xob[feetwet].Wz << " and slope: " << slohp << "\n";
			}
		}




		if (Pel_Usage[pelvis] == epelRAPTOR || Pel_Usage[pelvis] == epelQUAD)	
		{

	
			
		//float temp = Pel[pelvis][2][0] - fake_terrain( Pel[pelvis][0][0], Pel[pelvis][1][0] );

float	trr_normal[3];
float	trr_height = NPhysImport::fPelTerrainHeight( pelvis, Pel[pelvis][0][0], Pel[pelvis][1][0], trr_normal );
		float temp = Pel[pelvis][2][0] - trr_height;


		//float cheat = CATCH_REST*(height_z - temp) - damp_z*Pel[pelvis][2][1];

			//if ( ((height_z - temp) > 0) && (Pel_Data[pelvis][49] > 0) ) 
			if ( iJumpTest != 0 ) 
			{
				//Moments[2] += cheat;
				//Ok, control inputs finished, apply to pelvis...

				//if (trr_normal[0]*trr_normal[0] + trr_normal[1]*trr_normal[1]<.31)
				{
					float t0 = cos( Pel[pelvis][(RIGHT_PHI+1)][0] )*cos( Pel[pelvis][(RIGHT_PHI+1)][0] );
					float t1 = cos( Pel[pelvis][(RIGHT_PHI+0)][0] )*cos( Pel[pelvis][(RIGHT_PHI+0)][0] );

					Moments[(BODY+0)] += t0*Drive[0];
					Moments[(BODY+1)] += t1*Drive[1];
				}

			}
		}



		if (Pel_Usage[pelvis] == epelHUMAN) Moments[5] -= 16*Rates[2];
		else
		{
			if (Pel_Data[pelvis][14] > 1.5) adamp *= 3; 
			Moments[5] -= adamp*Rates[2];

			if (Pel_Usage[pelvis] == epelQUAD)
			{
				Moments[5] -= adamp*Rates[2];

				Moments[3] += 1000*( (fFrontTau) - (fBackTau) );
				Moments[3] -= 20*adamp*Rates[0];

				StuffVec( 0, 0, 1, Temp );
				World_to_Local( Temp, pelvis );
				Moments[4] += 3*ANGULAR_REST*Temp[0] - adamp*Rates[1];
			}

		}
		if (Pel_Usage[pelvis] == epelHUMAN) Moments[5] -= 16*Rates[2];
		else Moments[5] -= adamp*Rates[2];


//		Here is the "Turn to Direction" control...
//		------------------------------------------
#define TURN
#ifdef TURN
float	ass, wipe, direction[3] = { Kontrol[pelvis][3], Kontrol[pelvis][4], 0 };

#define FACTOR 60                          //30
float factor = FACTOR;

		if (Pel_Usage[pelvis] == epelRAPTOR || Pel_Usage[pelvis] == epelQUAD)	
		{
			direction[0] = Kontrol[pelvis][0];
			direction[1] = Kontrol[pelvis][1];
			if (Pel_Data[pelvis][14] > 2.3) 
			{
				factor *= .5;
				//Kontrol[pelvis][5] *= .5;
			}
		}


		if (Pel_Usage[pelvis] != epelHUMAN) factor = 60;//200;

		SafeNormalize( direction, ass, wipe );

		//conPhysics << "Ass: " << ass << "\n";
		//if (ass < 5 ) factor *= .2*ass;
		if ( ass > .01 ) {
			float turn = 0;

			World_to_Local( direction, pelvis );
			if ( direction[1] >= 0 )
				turn = -factor * arcsin( direction[0] );
			//else if ( (Pel_Usage[pelvis] == epelHUMAN) && (direction[1] > -.1) )
			else if ( (direction[1] > -.1) )
			//else if ( (direction[1] > -.5) )
			{
				if ( direction[0] > 0 ) turn = -factor*1.5707;
				else turn = factor*1.5707;
//				conPhysics << "Max turn " << turn << "\n";
			}
			
			Moments[(BODY_EULER+2)] += turn;
			//if (Pel_Usage[pelvis] == epelQUAD) conPhysics << "QUAD turn: " << turn << "\n";
		}

		// For CONFIG TEST only...
		//if ( Pel_Usage[pelvis] == epelQUAD ) Moments[(BODY_EULER+2)] += .5*Kontrol[pelvis][5];
		//else Moments[(BODY_EULER+2)] += Kontrol[pelvis][5];

		Moments[(BODY_EULER+2)] += Kontrol[pelvis][5];

//		conPhysics << "Moments: " << Moments[0] << ' ' << Moments[1] << ' ' << Moments[2] << ' '
//								  << Moments[3] << ' ' << Moments[4] << ' ' << Moments[5] << '\n';
			

#endif
//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++BULLSHIT CODE MUST DIE


//	Finish: 
//	=======
	

//		Moments[0] = Moments[1] = 0;
//		Moments[3] = Moments[4] = 0;
//		Moments[5] = 1;

	if (Pel_Usage[pelvis] != epelHUMAN)
	{

		//Moments[0] = Moments[1] = 0;
		//Moments[3] = Moments[4] = 0;
		//Moments[5] = 1;


//		RIGHT FOOT...
		Moments[(RIGHT_EULER+0)] *= Pel_Data[pelvis][7];		//Scaled!
		Moments[(RIGHT_EULER+1)] *= Pel_Data[pelvis][8];		//Couplings are bullshit!
		Moments[(RIGHT_EULER+2)] *= Pel_Data[pelvis][9];
		Moments[(RIGHT_FOOT+0)]  *= Pel_Data[pelvis][32];		//Mass scaling, before gravity!
		Moments[(RIGHT_FOOT+1)]  *= Pel_Data[pelvis][32];
		Moments[(RIGHT_FOOT+2)]  =  Pel_Data[pelvis][32]*Moments[(RIGHT_FOOT+2)] - 10;

		//conPhysics << "After: " << Moments[(RIGHT_FOOT+0)] << ", " << Moments[(RIGHT_FOOT+1)] << ", " << Moments[(RIGHT_FOOT+2)] << "\n";

//		LEFT FOOT...
		Moments[(LEFT_EULER+0)] *= Pel_Data[pelvis][7];			//Scaled!
		Moments[(LEFT_EULER+1)] *= Pel_Data[pelvis][8];			//Couplings are bullshit!
		Moments[(LEFT_EULER+2)] *= Pel_Data[pelvis][9];
	  	Moments[(LEFT_FOOT+0)]  *= Pel_Data[pelvis][32];		//Mass scaling, before gravity!
		Moments[(LEFT_FOOT+1)]  *= Pel_Data[pelvis][32];
		Moments[(LEFT_FOOT+2)]  =  Pel_Data[pelvis][32]*Moments[(LEFT_FOOT+2)] - 10;


		if (Pel_Usage[pelvis] == epelQUAD)
		{
			//		FRIGHT FOOT...
			Moments[(FRIGHT_EULER+0)] *= Pel_Data[pelvis][7];		//Scaled!
			Moments[(FRIGHT_EULER+1)] *= Pel_Data[pelvis][8];		//Couplings are bullshit!
			Moments[(FRIGHT_EULER+2)] *= Pel_Data[pelvis][9];
			Moments[(FRIGHT_FOOT+0)]  *= Pel_Data[pelvis][32];		//Mass scaling, before gravity!
			Moments[(FRIGHT_FOOT+1)]  *= Pel_Data[pelvis][32];
			Moments[(FRIGHT_FOOT+2)]  =  Pel_Data[pelvis][32]*Moments[(FRIGHT_FOOT+2)] - 10;

			//		FLEFT FOOT...
			Moments[(FLEFT_EULER+0)] *= Pel_Data[pelvis][7];			//Scaled!
			Moments[(FLEFT_EULER+1)] *= Pel_Data[pelvis][8];			//Couplings are bullshit!
			Moments[(FLEFT_EULER+2)] *= Pel_Data[pelvis][9];
			Moments[(FLEFT_FOOT+0)]  *= Pel_Data[pelvis][32];		//Mass scaling, before gravity!
			Moments[(FLEFT_FOOT+1)]  *= Pel_Data[pelvis][32];
			Moments[(FLEFT_FOOT+2)]  =  Pel_Data[pelvis][32]*Moments[(FLEFT_FOOT+2)] - 10;

		}

	}

		}//Test for death...

		if (Pel_Data[pelvis][49] <= 0)
		{
			for (int z = 0; z < 7; z++) Moments[z] = 0;
			if (Pel_Usage[pelvis] == epelHUMAN) Moments[(RIGHT_FOOT+0)] =
												Moments[(RIGHT_FOOT+1)] = 
												Moments[(RIGHT_FOOT+2)] = 
												Moments[(RIGHT_EULER+0)] = 
												Moments[(RIGHT_EULER+1)] = 
												Moments[(RIGHT_EULER+2)] = 0;
		}

		//if (Pel_Data[pelvis][49]<0) Moments[5] = Moments[0] = Moments[1] = Moments[2] = 0;


//		PELVIS...
//#define TEST
#ifdef TEST
		Moments[(BODY_EULER+0)] *= 0;//Pel_Data[pelvis][25];		//Scaled!
		Moments[(BODY_EULER+1)] *= 0;//Pel_Data[pelvis][26];		//Couplings are bullshit!
		Moments[(BODY_EULER+2)] *= 0;//Pel_Data[pelvis][27];
		Moments[(BODY+0)]        = 0;//Pel_Data[pelvis][31]*Moments[0];
		Moments[(BODY+1)]        = 0;//Pel_Data[pelvis][31]*Moments[1];	//Mass scaling, before gravity!
		Moments[(BODY+2)]        = 0;//Pel_Data[pelvis][31]*Moments[2] - 10;
#else
		//Moments[5] += 15;
		Moments[(BODY_EULER+0)] *= Pel_Data[pelvis][25];		//Scaled!
		Moments[(BODY_EULER+1)] *= Pel_Data[pelvis][26];		//Couplings are bullshit!
		Moments[(BODY_EULER+2)] *= Pel_Data[pelvis][27];
		Moments[(BODY+0)]        = Pel_Data[pelvis][31]*Moments[0];
		Moments[(BODY+1)]        = Pel_Data[pelvis][31]*Moments[1];	//Mass scaling, before gravity!
		Moments[(BODY+2)]        = Pel_Data[pelvis][31]*Moments[2] - 10;
#endif

		if (Pel_Usage[pelvis] != epelQUAD ) Moments[3] = Moments[4] = 0;//Moments[5] = 0;
		//Moments[3] = Moments[4] = 0;

}