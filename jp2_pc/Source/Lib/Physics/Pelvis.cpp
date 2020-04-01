//	Here is the Pelvic Wrapper...
//	=============================

#include <math.h>
#include "Common.hpp"
#include "Pelvis.h"
#include "futil.h"
#include "Arms.h"
#include "Xob_bc.hpp"
#include "BioModel.h"
#include "dino_biped.h"
#include "PhysicsStats.hpp"
#include "PhysicsImport.hpp"

#include "Lib/Sys/ConIO.hpp"
#include "Lib/Loader/SaveFile.hpp"

#define bCLAMP_PELVIS (1)

#if bCLAMP_PELVIS
	#include "Lib/Sys/DebugConsole.hpp"
#endif

#include <memory.h>
#include <iostream>
 
// The globals.
EPelvisType	Pel_Usage[ NUM_PELVISES ];					//Type of pelvis here, if any.
float	Pel[ NUM_PELVISES ][PELVIS_DOF][3];				//Body, right, left...
float	Pel_Data[ NUM_PELVISES ][ PELVIS_PARAMETERS ];	//As defined below...
int		Pel_Box_BC[ NUM_PELVISES ][ PELVIS_DOF ];		//Stores the box id's of the biotags...
float	History[ NUM_PELVISES ][ PELVIS_DOF ];
int		Hand_Drop_Flag[ NUM_PELVISES ];
float	angle = 0;

extern float Kontrol[NUM_PELVISES][6];
extern float Kontrol_Mouth[NUM_PELVISES];
extern bool	 bKontrol_Krouch[NUM_PELVISES];
extern int	 iKontrol_Jump[NUM_PELVISES];
extern CSet<int> asFootLatch[NUM_PELVISES];

#define bBIO_ORIENT		0

//	For renormalizing Dirac frames...
//	---------------------------------
#define	NUM_DIRAC 7
int		LM_Constrain[NUM_DIRAC] = { 3,
									RIGHT_EULER,
									LEFT_EULER,
									RIGHT_HAND_EULER,
									FRIGHT_EULER,
									FLEFT_EULER,
									HEAD_CONTROL};


//	 Used to detach a pelvis from boxes...
//	 -------------------------------------
void Remove_Pelvis_Linkage( int pelvisid, int elem, bool deactivate )
{
	int bcindex = Pel_Box_BC[pelvisid][elem];
	if (bcindex >= 0)
	{
		// Undo previous linkage.
		Xob[bcindex].PelvisModel = -1;
		Xob[bcindex].PelvisElem = -1;

		// Also deactivate box.
		if (deactivate)
		{
			Xob[bcindex].Instances[0] = 0;
			Xob[bcindex].Info = 0;
		}
	}
	Pel_Box_BC[pelvisid][elem] = -1;
}

//	 Used to detach a pelvis from boxes...
//	 -------------------------------------
void Remove_Pelvis_Linkage( int pelvisid, bool deactivate )
{
	// Undo all pelvis/box linkages.
	for (int i = 0; i < PELVIS_DOF; i++)
	{
		Remove_Pelvis_Linkage(pelvisid, i, deactivate);
	}
}

//	 Used to remove a pelvis from the system...
//	 ------------------------------------------
void Remove_Pelvis( int pelvisid )
{
	Remove_Pelvis_Linkage(pelvisid, true);
	Pel_Usage[pelvisid] = epelEMPTY;

	// Remove the biomodels.
	BioModels[2*pelvisid] = 0;
	BioModels[2*pelvisid + 1] = 0;
}



//	Find the equation of motion and stuff from B/C...
//	=================================================
void Pelvic_IDOF( int me, float timestep, float inputs[6] ) 
{

//	Test for now, inputs to the biomodels from the biotags...
//	---------------------------------------------------------

//	Here is where the footwork happens...
extern void Foot_IDOF( int pelvis, float delta_t, float res[PELVIS_DOF] ) ;

float	Moments[PELVIS_DOF];	//Results for each level of calculation,
								//NOTE: componoent 6 is not used, ADOF are Euler HERE ONLY!

	//Pel_Data[me][49] = 0;

	CCycleTimer ctmr;

//	Ok, we might as well get down to business...
	Foot_IDOF( me, timestep, Moments );


		if (Pel_Usage[me] != epelHUMAN)
		{
//		Linear...
		Pel[me][0][2] = Moments[0] + BioTag[me][(BODY+0)]; 
		Pel[me][1][2] = Moments[1] + BioTag[me][(BODY+1)];
		Pel[me][2][2] = Moments[2] + BioTag[me][(BODY+2)];

//		If you're dead, you fall down...
		if (Pel_Data[me][49] <= 0)//75)
		{
			Moments[3] += BioTag[me][(BODY+3)];
			Moments[4] += BioTag[me][(BODY+4)];
			Moments[5] += BioTag[me][(BODY+5)];
		}
		else Moments[5] += BioTag[me][(BODY+5)];

		Pel[me][(RIGHT_FOOT+0)][2] = Moments[(RIGHT_FOOT+0)];
		Pel[me][(RIGHT_FOOT+1)][2] = Moments[(RIGHT_FOOT+1)];
		Pel[me][(RIGHT_FOOT+2)][2] = Moments[(RIGHT_FOOT+2)];

		Pel[me][(LEFT_FOOT+0)][2] = Moments[(LEFT_FOOT+0)];
		Pel[me][(LEFT_FOOT+1)][2] = Moments[(LEFT_FOOT+1)];
		Pel[me][(LEFT_FOOT+2)][2] = Moments[(LEFT_FOOT+2)];
		}

		if (Pel_Usage[me] == epelQUAD)
		{
		Pel[me][(FRIGHT_FOOT+0)][2] = Moments[(FRIGHT_FOOT+0)];
		Pel[me][(FRIGHT_FOOT+1)][2] = Moments[(FRIGHT_FOOT+1)];
		Pel[me][(FRIGHT_FOOT+2)][2] = Moments[(FRIGHT_FOOT+2)];

		Pel[me][(FLEFT_FOOT+0)][2] = Moments[(FLEFT_FOOT+0)];
		Pel[me][(FLEFT_FOOT+1)][2] = Moments[(FLEFT_FOOT+1)];
		Pel[me][(FLEFT_FOOT+2)][2] = Moments[(FLEFT_FOOT+2)];
		}

		psPelFoot.Add(ctmr(), 1);


//		Update arms.  Human and Raptor are different...
//		-----------------------------------------------
float	input[6] = {0,0,0, 0,0,0};
		if ( Pel_Usage[me] == epelHUMAN )
		{


			//conPhysics << "BTbody[2]: " << BioTag[me][(BODY+2)] << "\n";


			Update_Right_Arm( me, timestep, input );

			Pel[me][(RIGHT_HAND+0)][2] = input[0] + BioTag[me][(RIGHT_HAND+0)];
			Pel[me][(RIGHT_HAND+1)][2] = input[1] + BioTag[me][(RIGHT_HAND+1)];
			Pel[me][(RIGHT_HAND+2)][2] = input[2] + BioTag[me][(RIGHT_HAND+2)] - 10;

			//conPhysics << "BT: " << BioTag[me][(RIGHT_HAND+3)] << ", " << BioTag[me][(RIGHT_HAND+4)] << ", " << BioTag[me][(RIGHT_HAND+5)] << "\n";

			Moments[(RIGHT_HAND_EULER+0)] = input[3] + BioTag[me][(RIGHT_HAND+3)];
			Moments[(RIGHT_HAND_EULER+1)] = input[4] + BioTag[me][(RIGHT_HAND+4)];
			Moments[(RIGHT_HAND_EULER+2)] = input[5] + BioTag[me][(RIGHT_HAND+5)];

int			bcindex = Pel_Box_BC[me][RIGHT_HAND];
			if (Xob[bcindex].Data[19]>100) Pel[me][(RIGHT_HAND+0)][2] = Pel[me][(RIGHT_HAND+1)][2] = Pel[me][(RIGHT_HAND+2)][2] = 
									   Moments[(RIGHT_HAND_EULER+0)] = Moments[(RIGHT_HAND_EULER+1)] = Moments[(RIGHT_HAND_EULER+2)] = 0;

extern float Pelvis_Set_Crouch;
float knees = Pel_Data[me][14] - .2;
float hat[3] = {0,0,1};

//			DEATH!!!!!!!!!
			if ( Pel_Data[me][49] > 0 )
			{


				Pel[me][0][2] -= Moments[0];
				Pel[me][1][2] -= Moments[1];
				Pel[me][2][2] -= Moments[2];


			//Crotch...
extern bool		 bKontrol_Krouch[NUM_PELVISES];

				int bodyblow = Pel_Box_BC[me][BODY];
				CXob& bodybox = Xob[bodyblow];
				bodybox.Data[6] = bodybox.SuperData[0][2] = .72*Pel_Data[me][14];
				bodybox.SuperData[0][5] = .14*bodybox.Data[6];

				if (Pel[me][MOUTH][0] < .5*Pel_Data[me][14])
				{
					bodybox.Data[6] = bodybox.SuperData[0][2] = .35*Pel_Data[me][14];
					bodybox.SuperData[0][5] = .7*bodybox.Data[6];
				}

				Local_to_World( hat, me );
				if (bKontrol_Krouch[me])
				{
					knees *= .1;
					////bodybox.Data[6] = bodybox.SuperData[0][2] = .45*Pel_Data[me][14];

					//bodybox.Data[6] = bodybox.SuperData[0][2] = .35*Pel_Data[me][14];
					//bodybox.SuperData[0][5] = .7*bodybox.Data[6];
				}

#define CSPRING 50
//#define CDAMP 7.5
#define CDAMP 10
	
				Pel[me][MOUTH][2] = CSPRING*(knees - Pel[me][MOUTH][0]) - CDAMP*Pel[me][MOUTH][1];
	
				if (Pel[me][MOUTH][0] < 0.09*(Pel_Data[me][14] - .2) ) {Pel[me][MOUTH][0] = .09*(Pel_Data[me][14] - .2); Pel[me][MOUTH][1] = 0;}
				if (Pel[me][MOUTH][0] > 1.1*(Pel_Data[me][14] - .2) ) {Pel[me][MOUTH][0] = 1.1*(Pel_Data[me][14] - .2); Pel[me][MOUTH][1] = 0;}

				hat[0] *= Pel[me][MOUTH][0];
				hat[1] *= Pel[me][MOUTH][0];
				hat[2] *= Pel[me][MOUTH][0];

				Pel[me][(BODY+0)][0] = Pel[me][(RIGHT_FOOT+0)][0] + hat[0];
				Pel[me][(BODY+1)][0] = Pel[me][(RIGHT_FOOT+1)][0] + hat[1];
				Pel[me][(BODY+2)][0] = Pel[me][(RIGHT_FOOT+2)][0] + hat[2];// + knees;//Pel_Data[me][14];

				Pel[me][(BODY+0)][1] = Pel[me][(RIGHT_FOOT+0)][1];
				Pel[me][(BODY+1)][1] = Pel[me][(RIGHT_FOOT+1)][1];
				Pel[me][(BODY+2)][1] = Pel[me][(RIGHT_FOOT+2)][1];// - Pel[me][MOUTH][1];

//			Coupling to the Pelvis...
#define		COUPLE 1
				if ( (Xob[(Pel_Box_BC[me][RIGHT_HAND])].Data[11] > 1) && (!Xob[(Pel_Box_BC[me][RIGHT_HAND])].Anchored) )
				{
					if (Xob[(Pel_Box_BC[me][RIGHT_HAND])].Data[19] > 10.0)//2.0)
					{
						Moments[(BODY+0)] -= COUPLE*input[0];
						Moments[(BODY+1)] -= COUPLE*input[1];
						Moments[(BODY+2)] -= COUPLE*input[2];
	
						//Pel[me][MOUTH][2] -= COUPLE*input[2];
					}
				}

				//if (BioTag[me][(BODY+2)] != 0)
				//{
				//	conPhysics << "Mouth: " << Pel[me][MOUTH][2] << ", " << BioTag[me][(BODY+2)] << "\n";
				//	Pel[me][MOUTH][2] *= .1;
				//	Pel[me][MOUTH][2] -= BioTag[me][(BODY+2)];
				//}

				Pel[me][(RIGHT_FOOT+0)][2] = BioTag[me][(RIGHT_FOOT+0)] + BioTag[me][(BODY+0)];
				Pel[me][(RIGHT_FOOT+1)][2] = BioTag[me][(RIGHT_FOOT+1)] + BioTag[me][(BODY+1)];
				Pel[me][(RIGHT_FOOT+2)][2] = BioTag[me][(RIGHT_FOOT+2)] + BioTag[me][(BODY+2)];

//				Let's stay out of trouble, shall we?
//				------------------------------------
int				fute = Pel_Box_BC[me][RIGHT_FOOT],
				budy = Pel_Box_BC[me][BODY];
				if ( (Xob[fute].bHitAnother) || (Xob[budy].bHitAnother) )	//Is there a contact...
				{

float				dir[3] = {Pel[me][(RIGHT_FOOT+0)][2], Pel[me][(RIGHT_FOOT+1)][2], 0};
float				monu, baby;
					SafeNormalize(dir, monu, baby);
			
					if (monu>0)	//Is there input...
					{

float					dp = Moments[0]*dir[0] +
							 Moments[1]*dir[1];

#define HIT_PER .4//1//.4//.4
float	hit_per = HIT_PER;

						if (dp < 0)
						{
							if (Xob[budy].bHitAnother) hit_per = 1;
							Moments[0] -= hit_per*dp*dir[0];
							Moments[1] -= hit_per*dp*dir[1];
						}
					}//Is there a control input?
				}//Is there a contact?

				Pel[me][(RIGHT_FOOT+0)][2] += Moments[0];
				Pel[me][(RIGHT_FOOT+1)][2] += Moments[1];
				Pel[me][(RIGHT_FOOT+2)][2] += Moments[2];

				Pel[me][MOUTH][2] -= BioTag[me][(RIGHT_FOOT+2)];
				if ( BioTag[me][(BODY+2)] < 0 )
				{
					if (Pel[me][MOUTH][1]>0) Pel[me][MOUTH][1] = 0;
					if (Pel[me][MOUTH][2]>0) Pel[me][MOUTH][2] = 0;
				}

				Pel[me][(BODY+0)][2] = Pel[me][(RIGHT_FOOT+0)][2];	//For sanity...
				Pel[me][(BODY+1)][2] = Pel[me][(RIGHT_FOOT+1)][2];
				Pel[me][(BODY+2)][2] = Pel[me][(RIGHT_FOOT+2)][2];

				//float Rates[3];
				//Get_Dirac_Vel( Rates, RIGHT_EULER, me );	//Get the rates in f...

				Moments[(RIGHT_EULER+0)] = BioTag[me][(RIGHT_EULER+0)];
				Moments[(RIGHT_EULER+1)] = BioTag[me][(RIGHT_EULER+1)];
				Moments[(RIGHT_EULER+2)] = BioTag[me][(RIGHT_EULER+2)];


				if ((Moments[(RIGHT_EULER+0)]==0)&&(Moments[(RIGHT_EULER+1)]==0)&&(Moments[(RIGHT_EULER+2)]==0))
				{
					float Rates[3];
					Get_Dirac_Vel( Rates, RIGHT_EULER, me );	//Get the rates in f...
					
					Moments[(RIGHT_EULER+0)] -= 1*Rates[0];
					Moments[(RIGHT_EULER+1)] -= 1*Rates[1];
					Moments[(RIGHT_EULER+2)] -= 1*Rates[2];
					//conPhysics << "Damp...\n";
				}
				//else if ( Xob[fute].Wz < -10 )
				else
				{
					float	norm[3];
					float	trr_height = NPhysImport::fPelTerrainHeight( me, Pel[me][0][0], Pel[me][1][0], norm );
					float	slohp = norm[0]*norm[0] + norm[1]*norm[1];

					if ( (slohp<.31)  || Xob[fute].bHitAnother )

					{
						float D[3] = {-Kontrol[me][0], -Kontrol[me][1], 0},
						Y[3] = {0,1,0},
						Z[3] = {0,0,1},
						RD[3];
						Local_to_World( Y, me );
						float dp = Y[0]*D[0] + Y[1]*D[1];
						D[0] = dp*Y[0];
						D[1] = dp*Y[1];

						World_to_Foot( D, RIGHT_FOOT, me );
						World_to_Foot( Z, RIGHT_FOOT, me );

						Outer( D, Z, RD );	//Save space...
						const float tau_mul = .75;//1;
						Moments[(RIGHT_EULER+0)] += tau_mul*RD[0];
						Moments[(RIGHT_EULER+1)] += tau_mul*RD[1];
						Moments[(RIGHT_EULER+2)] += tau_mul*RD[2];
					}
				}
				//float speed = sqrt( Pel[me][0][1]*Pel[me][0][1]
				//				  + Pel[me][1][1]*Pel[me][1][1]
				//				  + Pel[me][2][1]*Pel[me][2][1] );
				//conPhysics << "Speedo: " << speed << "\n";

			}//DEATH TEST!!!!!!!!!!!!!!!!!!!!!!!!!!!
			else //I am dead...
			{
				Pel[me][0][2] = BioTag[me][(BODY+0)];
				Pel[me][1][2] = BioTag[me][(BODY+1)];
				Pel[me][2][2] = BioTag[me][(BODY+2)] - 10;

				Moments[3] = BioTag[me][3];
				Moments[4] = BioTag[me][4];
				Moments[5] = BioTag[me][5];

				Pel[me][(RIGHT_FOOT+0)][0] = Pel[me][0][0];
				Pel[me][(RIGHT_FOOT+1)][0] = Pel[me][1][0];
				Pel[me][(RIGHT_FOOT+2)][0] = Pel[me][2][0] + .25;

				Pel[me][(RIGHT_EULER+0)][1] =
				Pel[me][(RIGHT_EULER+1)][1] =
				Pel[me][(RIGHT_EULER+2)][1] =
				Pel[me][(RIGHT_EULER+3)][1] = 0;

				Pel[me][(RIGHT_FOOT+0)][2] = Pel[me][(RIGHT_FOOT+0)][1] = 0;
				Pel[me][(RIGHT_FOOT+1)][2] = Pel[me][(RIGHT_FOOT+1)][1] = 0;
				Pel[me][(RIGHT_FOOT+2)][2] = Pel[me][(RIGHT_FOOT+2)][1] = 0;

				//Pel[me][(RIGHT_HAND+0)][2] -= BioTag[me][(RIGHT_HAND+0)];	//Hand will not vibrate against ground during death.
				//Pel[me][(RIGHT_HAND+1)][2] -= BioTag[me][(RIGHT_HAND+1)];
				//Pel[me][(RIGHT_HAND+2)][2] -= BioTag[me][(RIGHT_HAND+2)];

			}//End of death else


		}//End update arms for epelHUMAN





//		Dinosaurs...
//		------------
		if ( Pel_Usage[me] == epelRAPTOR || Pel_Usage[me] == epelQUAD )
		{


			if (Pel_Usage[me] == epelRAPTOR)
			{
				Pel[me][(RIGHT_FOOT+0)][2] += BioTag[me][(RIGHT_FOOT+0)];
				Pel[me][(RIGHT_FOOT+1)][2] += BioTag[me][(RIGHT_FOOT+1)];
				Pel[me][(RIGHT_FOOT+2)][2] += BioTag[me][(RIGHT_FOOT+2)];

				Pel[me][(LEFT_FOOT+0)][2] += BioTag[me][(LEFT_FOOT+0)];
				Pel[me][(LEFT_FOOT+1)][2] += BioTag[me][(LEFT_FOOT+1)];
				Pel[me][(LEFT_FOOT+2)][2] += BioTag[me][(LEFT_FOOT+2)];
			}



#define			NUM_BIO_PER_RAPTOR 2
int				BioIndex = NUM_BIO_PER_RAPTOR*me;
				CBioModel* bio_tail = BioModels[BioIndex+0];
				CBioModel* bio_head = BioModels[BioIndex+1];


#define SPRING 50
#define DAMP 7.17
	
			Pel[me][MOUTH][2] = SPRING*(Kontrol_Mouth[me] - Pel[me][MOUTH][0]) - DAMP*Pel[me][MOUTH][1];

			if (Pel[me][MOUTH][0] < 0) Pel[me][MOUTH][0] = 0;
			if (Pel[me][MOUTH][0] > 1) Pel[me][MOUTH][0] = 1;


//			TEMP FOR NOW...
			if ( Pel_Usage[me] == epelRAPTOR || Pel_Usage[me] == epelQUAD )
			{
int				CP = bio_head->Dof - 2;

#define MUL 1//3//5
float	mul = MUL;
		if (Pel_Data[me][49]<=0) mul = 1;


					Pel[me][(HEAD_BC+0)][1] = bio_head->Field[CP][0][1];
					Pel[me][(HEAD_BC+1)][1] = bio_head->Field[CP][1][1];
					Pel[me][(HEAD_BC+2)][1] = bio_head->Field[CP][2][1];

					if (Pel_Usage[me] != epelQUAD)
					{
						bio_head->FieldInputs[CP][0] = mul*BioTag[me][(HEAD_BC+0)];
						bio_head->FieldInputs[CP][1] = mul*BioTag[me][(HEAD_BC+1)];
						bio_head->FieldInputs[CP][2] = mul*BioTag[me][(HEAD_BC+2)];
					}


					CP = bio_tail->Dof - 5;

					Pel[me][(TAIL_BC+0)][1] = bio_tail->Field[CP][0][1];
					Pel[me][(TAIL_BC+1)][1] = bio_tail->Field[CP][1][1];
					Pel[me][(TAIL_BC+2)][1] = bio_tail->Field[CP][2][1];

					bio_tail->FieldInputs[CP][0] = mul*BioTag[me][(TAIL_BC+0)];
					bio_tail->FieldInputs[CP][1] = mul*BioTag[me][(TAIL_BC+1)];
					bio_tail->FieldInputs[CP][2] = mul*BioTag[me][(TAIL_BC+2)];
			
			

//		See if we can update the BCBOX orientations...
float center[MAX_JOINTS][3];
float X[MAX_JOINTS][3];
float Y[MAX_JOINTS][3];
float Z[MAX_JOINTS][3];

			bio_tail->GenerateFrames( center, X, Y, Z );
	
					Pel[me][(TAIL_BC+0)][0] = center[2][0];	//3rd joint...  NEED VELOCITY...
					Pel[me][(TAIL_BC+1)][0] = center[2][1];
					Pel[me][(TAIL_BC+2)][0] = center[2][2];

CXob&		tail = PelBox(me, TAIL_BC);

#if bBIO_ORIENT
					tail.SuperOrient[0][0][0] = X[2][0];	tail.SuperOrient[0][0][1] = Y[2][0];	tail.SuperOrient[0][0][2] = Z[2][0];
					tail.SuperOrient[0][1][0] = X[2][1];	tail.SuperOrient[0][1][1] = Y[2][1];	tail.SuperOrient[0][1][2] = Z[2][1];
					tail.SuperOrient[0][2][0] = X[2][2];	tail.SuperOrient[0][2][1] = Y[2][2];	tail.SuperOrient[0][2][2] = Z[2][2];
#else
					NPhysImport::SetRotate(&Pel[me][TAIL_BC+3], X[2], Y[2], Z[2]);
#endif

			bio_head->GenerateFrames( center, X, Y, Z );
	
					Pel[me][(HEAD_BC+0)][0] = center[2][0];	//3rd joint...  NEED VELOCITY...
					Pel[me][(HEAD_BC+1)][0] = center[2][1];
					Pel[me][(HEAD_BC+2)][0] = center[2][2];

CXob&		head = PelBox(me, HEAD_BC);

#if bBIO_ORIENT
					head.SuperOrient[0][0][0] = X[2][0];	head.SuperOrient[0][0][1] = Y[2][0];	head.SuperOrient[0][0][2] = Z[2][0];
					head.SuperOrient[0][1][0] = X[2][1];	head.SuperOrient[0][1][1] = Y[2][1];	head.SuperOrient[0][1][2] = Z[2][1];
					head.SuperOrient[0][2][0] = X[2][2];	head.SuperOrient[0][2][1] = Y[2][2];	head.SuperOrient[0][2][2] = Z[2][2];
#else
					NPhysImport::SetRotate(&Pel[me][HEAD_BC+3], X[2], Y[2], Z[2]);
#endif

//			HEAD...
//			-------
#define	HC_K 100
#define HC_D 10

float				tvec[3] = {0,0,1},
					tvel[3] = {0,0,0};

					Get_Dirac_Vel( tvel, (HEAD_CONTROL), me );
					Local_to_Foot( tvec, HEAD_CONTROL-3, me );

float				rollang = bio_head->Control[5];
					tvec[0] = cos( rollang )*tvec[0] + sin( rollang )*tvec[2];
					Moments[(HEAD_CONTROL+1)] += HC_K*tvec[0] - 1.4*HC_D*tvel[1];

					if (Pel_Data[me][49]>0)
					{
						tvec[0] = bio_head->Control[1];
						tvec[1] = bio_head->Control[2];
						tvec[2] = bio_head->Control[3];
						//conPhysics << "TVEC; " << tvec[0] << ", " << tvec[1] << ", " << tvec[2] << "\n";
					}
					else
					{
						tvec[0] = 0;
						tvec[1] = 1;
						tvec[2] = 0;
					}

float				t1, t2;
					SafeNormalize( tvec, t1, t2 );

					Local_to_Foot( tvec, HEAD_CONTROL-3, me );
					Moments[(HEAD_CONTROL+0)] += HC_K*tvec[2] - HC_D*tvel[0];
					Moments[(HEAD_CONTROL+2)] +=-HC_K*tvec[0] - HC_D*tvel[2];
					




					float mass_scale = .75;
					if (Pel_Data[me][49] > 0)
					{
						if (head.bHitAnother)
						{
							Pel[me][(BODY+0)][2] += mass_scale*BioTag[me][(HEAD_BC+0)];
							Pel[me][(BODY+1)][2] += mass_scale*BioTag[me][(HEAD_BC+1)];
							//Pel[me][(BODY+0)][2] += BioTag[me][(HEAD_BC+0)];
							//Pel[me][(BODY+1)][2] += BioTag[me][(HEAD_BC+1)];
						}

/*


float				dir[3] = {BioTag[me][(HEAD_BC+0)],
							  BioTag[me][(HEAD_BC+1)], 0};

//float				dir[3] = {BioTag[me][(HEAD_BC+0)] + BioTag[me][(TAIL_BC+0)],
//							  BioTag[me][(HEAD_BC+1)] + BioTag[me][(TAIL_BC+1)], 0};

float				monu, baby;
					SafeNormalize(dir, monu, baby);
			

float					dp = Moments[0]*dir[0] +
							 Moments[1]*dir[1];

#define PHIT_PER 1//.4//.4
float	phit_per = HIT_PER;

						if (dp < 0)
						{
							Pel[me][(BODY+0)][2] -= phit_per*dp*dir[0];
							Pel[me][(BODY+1)][2] -= phit_per*dp*dir[1];
						}

					
*/
						//if (tail.bHitAnother)
						//{
						//	Pel[me][(BODY+0)][2] += mass_scale*BioTag[me][(TAIL_BC+0)];
						//	Pel[me][(BODY+1)][2] += mass_scale*BioTag[me][(TAIL_BC+1)];
						//}
					
						if ( tail.bHitAnother)
						{
							mass_scale = .2;
							//Pel[me][(BODY+0)][2] += mass_scale*BioTag[me][(TAIL_BC+0)];
							//Pel[me][(BODY+1)][2] += mass_scale*BioTag[me][(TAIL_BC+1)];
	
							float v1[3] = { tail.State[0][0] - Pel[me][(BODY+0)][0],
											tail.State[1][0] - Pel[me][(BODY+1)][0],
											tail.State[2][0] - Pel[me][(BODY+2)][0] };
							
							World_to_Local( v1, me );
							float v2[3] = { mass_scale*BioTag[me][(TAIL_BC+0)],
											mass_scale*BioTag[me][(TAIL_BC+1)],
											mass_scale*BioTag[me][(TAIL_BC+2)] };
							//float v2[3] = { BioTag[me][(TAIL_BC+0)],
							//				BioTag[me][(TAIL_BC+1)],
							//				BioTag[me][(TAIL_BC+2)] };
							World_to_Local( v2, me );

							float result[3];
							Outer( v1, v2, result );
					
						    Moments[5] += Pel_Data[me][27]*result[2];
						    //Moments[5] = Pel_Data[me][27]*result[2];

						}
						
					}




			int i;
			for (i = 0; i < 3; i++)	//Update the root of the tail...
			{
float			temp[3] = {bio_tail->Field[(bio_tail->Dof+i)][0][0] + bio_tail->Offset[0],
						   bio_tail->Field[(bio_tail->Dof+i)][1][0] + bio_tail->Offset[1],
						   bio_tail->Field[(bio_tail->Dof+i)][2][0] + bio_tail->Offset[2] };

				//conPhysics << "T" << i << ": " << temp[0] << " : " << temp[1] << " : " << temp[2] << "\n";
float			b_vel[3];
				Global_Vel( b_vel, temp, BODY, me );

				bio_tail->Field[i][0][1] = b_vel[0];
				bio_tail->Field[i][1][1] = b_vel[1];
				bio_tail->Field[i][2][1] = b_vel[2];

				Local_to_World_Trans( temp, me );
	
				bio_tail->Field[i][0][0] = temp[0];
				bio_tail->Field[i][1][0] = temp[1];
				bio_tail->Field[i][2][0] = temp[2];
			}


			for (i = 0; i < 3; i++)	//Update the root of the head...
			{
float			temp[3] = { bio_head->Field[(bio_head->Dof+i)][0][0] + bio_head->Offset[0],
						    bio_head->Field[(bio_head->Dof+i)][1][0] + bio_head->Offset[1],
						    bio_head->Field[(bio_head->Dof+i)][2][0] + bio_head->Offset[2] };

//Global_Vel( float result[3], float coord[3], int index, int model )
float			b_vel[3];
				Global_Vel( b_vel, temp, BODY, me );

				bio_head->Field[i][0][1] = b_vel[0];
				bio_head->Field[i][1][1] = b_vel[1];
				bio_head->Field[i][2][1] = b_vel[2];

				//if (i != 1) temp[1] += .3;

				Local_to_World_Trans( temp, me );
	
				bio_head->Field[i][0][0] = temp[0];
				bio_head->Field[i][1][0] = temp[1];
				bio_head->Field[i][2][0] = temp[2];
			}


			head.State[0][2] = Pel[me][(HEAD_BC+0)][2];
			head.State[1][2] = Pel[me][(HEAD_BC+1)][2];
			head.State[2][2] = Pel[me][(HEAD_BC+2)][2];

			tail.State[0][2] = Pel[me][(TAIL_BC+0)][2];
			tail.State[1][2] = Pel[me][(TAIL_BC+1)][2];
			tail.State[2][2] = Pel[me][(TAIL_BC+2)][2];

			//conPhysics << "Head acc: " << Pel[me][(HEAD_BC+0)][2] << ", " << Pel[me][(HEAD_BC+1)][2] << ", " << Pel[me][(HEAD_BC+2)][2] << "\n"; 
			//conPhysics << "Boxy acc: " << State[head][0][2] << ", " << State[head][1][2] << ", " << State[head][2][2] << "\n"; 

			}//Raptor/Quad test for tail/head BC stuff...	






//		Here is where we zero all the BioTags...
		for (int bz = 0; bz < PELVIS_DOF; bz++)	BioTag[me][bz] = 0;














//	Here we pass a control input to an individual point in the Field...

	if ( 0 )//(Pel_Usage[me] == epelRAPTOR) && (Pel_Data[me][49]>0) ) 
	{
		angle += 3*timestep;
		if (angle > 2*3.1415) angle -= 2*3.1415;
		float target[3] = { .5*sin( angle ),-.7, .1 };
		bio_tail->SetInput( target, 0, .1/*1*/, me );
	}
	else if (0)//Pel_Usage[me] == epelQUAD)
	{
		float target[3] = { 0, -2.2, .1 };
		bio_tail->SetInput( target, 0, .1/*1*/, me );
	}

//extern float Kontrol[NUM_PELVISES][6];
//	float nose[3] = {Kontrol[me][0], Kontrol[me][1], .2};
//	World_to_Local( nose, me );
//	float butt, munch;
//	SafeNormalize( target1, butt, munch );
//	target[0] *= .75;
//	target[1] *= .75 + 1;
//	target[2] = .2;

//float	nose[3] = { sin( angle ), fabs( cos( angle ) ), 0 };//.8*sin(angle), .6, .2};
//		if (nose[2] < .15) nose[2] = .15;


//float	nose[3] = { .5, .3, 0 };//.8*sin(angle), .6, .2};
		//World_to_Local( nose, me );

	//RaptorBodyControl( me, nose, 0, 0, 0, fabs( 1.1*sin(angle) ) );

	//SetBioInput( (BioIndex+1), target1, 30, me );


		}

//#define NIOSY
#ifdef NIOSY
		for (int tt = 0; tt < 21 /*PELVIS_DOF*/; tt++ )
		{
			conPhysics << tt << ": " << Pel[me][tt][0] << ", " << Pel[me][tt][1] << ", " << Pel[me][tt][2] << "\n";
		}
#endif



//		Not clear that this is needed...
//		--------------------------------
//#ifdef ANAL_RETENTIVE
//		Deal with four parameters...
//		----------------------------
		for (int t = 0; t < NUM_DIRAC; t++ ) {

//		Don't do extra work if you don't need to...
		if ( 1 )//(t < 4) || (Pel_Usage[me] == epelQUAD) )
		{

int			index = LM_Constrain[t];

float		a0 = Pel[me][(index+0)][0],
			a1 = Pel[me][(index+1)][0],
			a2 = Pel[me][(index+2)][0],
			a3 = Pel[me][(index+3)][0];

float		d0 = Pel[me][(index+0)][1],
			d1 = Pel[me][(index+1)][1],
			d2 = Pel[me][(index+2)][1],
			d3 = Pel[me][(index+3)][1];

float		ad = 2*( a0*d1 + a3*d2 - a2*d3 - a1*d0 ),
			bd = 2*(-a3*d1 + a0*d2 + a1*d3 - a2*d0 ),
			gd = 2*( a2*d1 - a1*d2 + a0*d3 - a3*d0 );

//	Add inertial couplings...
//	-------------------------
//	alpha_total += gd*bd*( 1/Data[me][9] - 1/Data[me][8] );
//	beta_total  += ad*gd*( 1/Data[me][7] - 1/Data[me][9] );
//	gamma_total += bd*ad*( 1/Data[me][8] - 1/Data[me][7] );

//			Now rescale...
float		atemp = Moments[(index+0)],		//Here the index goes only to 3...
			btemp = Moments[(index+1)],
			gtemp = Moments[(index+2)];

//	For grabbing anchored and hinged objects...
			if ( index == RIGHT_HAND_EULER && Pel_Box_BC[me][RIGHT_HAND] >= 0)
			{

CXob&			handy = PelBox(me, RIGHT_HAND);

				if (handy.Anchored)
				{
					//State[i][0][2] = State[i][1][2] = State[i][2][2] = 0;
					Pel[me][(RIGHT_HAND+0)][2] = 
					Pel[me][(RIGHT_HAND+1)][2] = 					
					Pel[me][(RIGHT_HAND+2)][2] = 0;					


					if ( fabs(handy.Data[14]) > 0 )
					{
						gtemp += handy.Data[9]*handy.Data[14];
					} 

					//if (spring)
					//
					
					//Gravity Torque...
					if ( handy.Data[19] > 1.0 )
					{

						//conPhysics << "Mass as reported " << handy.Data[19] << "for " << i << ".\n";	
						float gtau[3] = { 0, 0, -5},//-10 ,
							  mom[3]  = { handy.SuperData[0][3],
										  handy.SuperData[0][4],
										  handy.SuperData[0][5] },
						      res[3]  = {0,0,0};
						World_to_Foot( gtau, RIGHT_HAND, me );
//						Make_Transform( i, TEMP_INDEX );
//						From( gtau[0], gtau[1], gtau[2], TEMP_INDEX );
						Outer( mom, gtau, res );
						atemp += handy.Data[7]*res[0];
						btemp += handy.Data[8]*res[1];
						gtemp += handy.Data[9]*res[2];
					}
	

					float damper = handy.Data[12];//.05;//.75;//.1;//.75;
					atemp -= handy.Data[7]*damper*ad;
					btemp -= handy.Data[8]*damper*bd;
					gtemp -= handy.Data[9]*damper*gd;

				}

				if (!handy.Ignorable_DOF[3]) atemp = 0;//Deal with ignorable coords...
				if (!handy.Ignorable_DOF[4]) btemp = 0;
				if (!handy.Ignorable_DOF[5]) gtemp = 0;

				//atemp = btemp = gtemp = 0;

			}


			Pel[me][(index+0)][2] =-.5*( a1*atemp + a2*btemp + a3*gtemp
							+ d1*ad    + d2*bd    + d3*gd    );
			Pel[me][(index+1)][2] = .5*( a0*atemp - a3*btemp + a2*gtemp 
							+ d0*ad    - d3*bd    + d2*gd    );
			Pel[me][(index+2)][2] = .5*( a3*atemp + a0*btemp - a1*gtemp 
							+ d3*ad    + d0*bd    - d1*gd    );
			Pel[me][(index+3)][2] = .5*(-a2*atemp + a1*btemp + a0*gtemp 
							- d2*ad    + d1*bd    + d0*gd    );

//			Correct our wayward ways...
//			---------------------------
float		Ld = -2*( a0*d0 + a1*d1 + a2*d2 + a3*d3 ),
			L = 1 - ( a0*a0 + a1*a1 + a2*a2 + a3*a3 );

//			L  *= 0;//50;
//			Ld *= 0;//5;//50;//.05;
			L  *= .5;
			Ld *= .1;//50;//.05;



#if 1
			Pel[me][(index+0)][2] += a0*Ld + d0*L;
			Pel[me][(index+1)][2] += a1*Ld + d1*L;
			Pel[me][(index+2)][2] += a2*Ld + d2*L;
			Pel[me][(index+3)][2] += a3*Ld + d3*L;
#endif
			Pel[me][(index+0)][1] += a0*L;
			Pel[me][(index+1)][1] += a1*L;
			Pel[me][(index+2)][1] += a2*L;
			Pel[me][(index+3)][1] += a3*L;

		}//Test for QUADs...

		}//Loop over four parameter DOF...
//#endif
		
//	Done!
//	-----
}



//	Initialize a single pelvic model, keeping it really simple for now...
//	---------------------------------------------------------------------
void Make_a_Pelvis( CInstance* pins,
				    int number,
				    float params[PELVIS_PARAMETERS],
					float init[7][3],
					float box_params[10] )	//Passed BACK...
{

int	counter = 0;
extern bool bIsLimp[NUM_PELVISES];

//	First, Zero the state and biotags...
//	------------------------------------
	for( counter = 0; counter < 6; counter++ ) Kontrol[number][counter] = 0;			//Inputs...
	Kontrol_Mouth[number] = 0;	//Mouth position...
	bKontrol_Krouch[number] = false;
	iKontrol_Jump[number] = 0;
	Hand_Drop_Flag[number] = 0;
	bIsLimp[number] = false;

	for( counter = 0; counter < PELVIS_DOF; counter++ )	Pel[number][counter][0] =		//State info...
														Pel[number][counter][1] =
														Pel[number][counter][2] =
														BioTag[number][counter] = 0;

//	Now, copy in the parameters...
//	------------------------------
	for ( counter = 0; counter < PELVIS_PARAMETERS; counter++ ) Pel_Data[number][counter] = params[counter];

	// For now, no linear velocity.
	int i;
	for (i = 0; i < 3; i++)
	{
		Pel[number][i][0] = init[i][0];
		Pel[number][i][1] = 0;
		Pel[number][i][2] = 0;
	}

	// Allow rotational velocity.
	for (; i < 7; i++)
	{
		Pel[number][i][0] = init[i][0];
		Pel[number][i][1] = init[i][1];
		Pel[number][i][2] = 0;
	}

		//Pel[number][28][0] = Pel[number][10][0] = Pel[number][17][0] = 1;	// Zero...
		//Pel[number][29][0] = Pel[number][11][0] = Pel[number][18][0] = 0;
		//Pel[number][30][0] = Pel[number][12][0] = Pel[number][19][0] = 0;
		//Pel[number][31][0] = Pel[number][13][0] = Pel[number][20][0] = 0;

		Pel[number][28][0] = Pel[number][10][0] = Pel[number][17][0] = Pel[number][3][0];
		Pel[number][29][0] = Pel[number][11][0] = Pel[number][18][0] = Pel[number][4][0];
		Pel[number][30][0] = Pel[number][12][0] = Pel[number][19][0] = Pel[number][5][0];
		Pel[number][31][0] = Pel[number][13][0] = Pel[number][20][0] = Pel[number][6][0];

		Pel[number][28][1] = Pel[number][10][1] = Pel[number][17][1] = 0;	//Probably best for initialization...
		Pel[number][29][1] = Pel[number][11][1] = Pel[number][18][1] = 0;
		Pel[number][30][1] = Pel[number][12][1] = Pel[number][19][1] = 0;
		Pel[number][31][1] = Pel[number][13][1] = Pel[number][20][1] = 0;

		Pel[number][35][0] = Pel[number][42][0] = Pel[number][3][0];
		Pel[number][36][0] = Pel[number][43][0] = Pel[number][4][0];
		Pel[number][37][0] = Pel[number][44][0] = Pel[number][5][0];
		Pel[number][38][0] = Pel[number][45][0] = Pel[number][6][0];

		Pel[number][35][1] = Pel[number][42][1] = 0;
		Pel[number][36][1] = Pel[number][43][1] = 0;
		Pel[number][37][1] = Pel[number][44][1] = 0;
		Pel[number][38][1] = Pel[number][45][1] = 0;

		Pel[number][(HEAD_CONTROL+0)][0] = Pel[number][(BODY_EULER+0)][0];
		Pel[number][(HEAD_CONTROL+1)][0] = Pel[number][(BODY_EULER+1)][0];
		Pel[number][(HEAD_CONTROL+2)][0] = Pel[number][(BODY_EULER+2)][0];
		Pel[number][(HEAD_CONTROL+3)][0] = Pel[number][(BODY_EULER+3)][0];


//	Place the feet in some reasonable position...
//	---------------------------------------------
float	trans[3];

	trans[0] =  + Pel_Data[number][13];
	trans[1] =  - Pel_Data[number][46];
	trans[2] =  -.75*Pel_Data[number][14];
	Local_to_World_Trans( trans, number );
	Pel[number][(RIGHT_FOOT+0)][0] = trans[0];
	Pel[number][(RIGHT_FOOT+1)][0] = trans[1];
	Pel[number][(RIGHT_FOOT+2)][0] = trans[2];

	trans[0] =  - Pel_Data[number][13];
	trans[1] =  - Pel_Data[number][46];
	trans[2] =  -.75*Pel_Data[number][14];
	Local_to_World_Trans( trans, number );
	Pel[number][(LEFT_FOOT+0)][0] = trans[0];
	Pel[number][(LEFT_FOOT+1)][0] = trans[1];
	Pel[number][(LEFT_FOOT+2)][0] = trans[2];

	trans[0] =  + Pel_Data[number][45];
	trans[1] =  + Pel_Data[number][46];
	trans[2] =  -.75*Pel_Data[number][50];
	Local_to_World_Trans( trans, number );
	Pel[number][(FRIGHT_FOOT+0)][0] = trans[0];
	Pel[number][(FRIGHT_FOOT+1)][0] = trans[1];
	Pel[number][(FRIGHT_FOOT+2)][0] = trans[2];

	trans[0] =  - Pel_Data[number][45];
	trans[1] =  + Pel_Data[number][46];
	trans[2] =  -.75*Pel_Data[number][50];
	Local_to_World_Trans( trans, number );
	Pel[number][(FLEFT_FOOT+0)][0] = trans[0];
	Pel[number][(FLEFT_FOOT+1)][0] = trans[1];
	Pel[number][(FLEFT_FOOT+2)][0] = trans[2];

//	Place the arms in an equally reasonable position...
	trans[0] =  + .4;
	trans[1] =  + .4;
	trans[2] =  + .5*Pel_Data[number][40];
	Local_to_World_Trans( trans, number );
	Pel[number][(RIGHT_HAND+0)][0] = trans[0];
	Pel[number][(RIGHT_HAND+1)][0] = trans[1];
	Pel[number][(RIGHT_HAND+2)][0] = trans[2];


/*
	Pel[number][(RIGHT_FOOT+0)][0] = Pel[number][0][0] + Pel_Data[number][13];
	Pel[number][(RIGHT_FOOT+1)][0] = Pel[number][1][0] - Pel_Data[number][46];
	Pel[number][(RIGHT_FOOT+2)][0] = Pel[number][2][0] - .75*Pel_Data[number][14];

	Pel[number][(LEFT_FOOT+0)][0] = Pel[number][0][0] - Pel_Data[number][13];
	Pel[number][(LEFT_FOOT+1)][0] = Pel[number][1][0] - Pel_Data[number][46];
	Pel[number][(LEFT_FOOT+2)][0] = Pel[number][2][0] - .75*Pel_Data[number][14];



	Pel[number][(FRIGHT_FOOT+0)][0] = Pel[number][0][0] + Pel_Data[number][45];
	Pel[number][(FRIGHT_FOOT+1)][0] = Pel[number][1][0] + Pel_Data[number][46];
	Pel[number][(FRIGHT_FOOT+2)][0] = Pel[number][2][0] - .75*Pel_Data[number][14];

	Pel[number][(FLEFT_FOOT+0)][0] = Pel[number][0][0] - Pel_Data[number][45];
	Pel[number][(FLEFT_FOOT+1)][0] = Pel[number][1][0] + Pel_Data[number][46];
	Pel[number][(FLEFT_FOOT+2)][0] = Pel[number][2][0] - .75*Pel_Data[number][14];




//	Place the arms in an equally reasonable position...
	Pel[number][(RIGHT_HAND+0)][0] = Pel[number][0][0] + .4;
	Pel[number][(RIGHT_HAND+1)][0] = Pel[number][1][0] + .4;
	Pel[number][(RIGHT_HAND+2)][0] = Pel[number][2][0] + .5*Pel_Data[number][40];
*/



//	Initialise the history...
	for (int tt = 0; tt < PELVIS_DOF; tt++ ) History[number][tt] = Pel[number][tt][0];

	asFootLatch[number] = set0;

//	Ok, start out the Foot with an interesting attitude...
//float	angles[3][2] = {{0,0}, {0,0}, {0,0}};
//		Set_Dirac_From_Euler( number, RIGHT_FOOT, angles );
//		Set_Dirac_From_Euler( number, LEFT_FOOT, angles );


//	Thank goodness that's over...
//	-----------------------------

}



//	Integrate with Lagrange constraints...
//	======================================
void	Pelvis_integrate( float delta_t, float inputs[6] ) {

int		i = 0,
		j = 0;

int	active = 0;


//	Now the pelvises...
	for (i = 0; i < NUM_PELVISES; i++)
	if (Pel_Usage[i])
	{

float	store[7];
 
	if (Pel_Usage[i] == epelHUMAN)
	{
		store[0] = Pel[i][0][0];
		store[1] = Pel[i][1][0];
		store[2] = Pel[i][2][0];

		store[3] = Pel[i][3][0];
		store[4] = Pel[i][4][0];
		store[5] = Pel[i][5][0];
		store[6] = Pel[i][6][0];
	}

//			Here goes...
			Pelvic_IDOF( i, delta_t, inputs );
			active++;

//			Maximally stupid integration...
//			-------------------------------
			for (j = 0; j < (RIGHT_PHI+0); j++) {
				Pel[i][j][0] += delta_t*( Pel[i][j][1] );
				Pel[i][j][1] += delta_t*Pel[i][j][2];
			}
		
//		Now do some hand stuff...
//		-------------------------
		if (Pel_Usage[i] == epelHUMAN)
		{
			CXob& handy = PelBox(i, RIGHT_HAND);
			if ( handy.Movable )
			{
				for (j = RIGHT_HAND; j < (FRIGHT_FOOT); j++)
				{
					Pel[i][j][0] += delta_t*( Pel[i][j][1] );
					Pel[i][j][1] += delta_t*Pel[i][j][2];
				}
			}
		}

//		Now if you are a QUAD do the rest of the DOF...
//		-----------------------------------------------
			if ( Pel_Usage[i] == epelQUAD )
			{
				for (j = (FRIGHT_FOOT); j < (HEAD_BC); j++) {
					Pel[i][j][0] += delta_t*( Pel[i][j][1] );
					Pel[i][j][1] += delta_t*Pel[i][j][2];
				}
			}

//		Finally, independant head control...
			if ( Pel_Usage[i] != epelHUMAN )
			{
				for (j = (HEAD_CONTROL); j < (PELVIS_DOF); j++) {
					Pel[i][j][0] += delta_t*( Pel[i][j][1] );
					Pel[i][j][1] += delta_t*Pel[i][j][2];
				}
			}

//			Now apply all that constraint shit...
//			-------------------------------------
//float	lmul = .15;//.01;//.5;//0.1;//.01;//delta_t;
			for (int t = 0; t < NUM_DIRAC; t++ ) {
	
int				index = LM_Constrain[t];

float			a0 = Pel[i][(index+0)][0],
				a1 = Pel[i][(index+1)][0],
				a2 = Pel[i][(index+2)][0],
				a3 = Pel[i][(index+3)][0];


float			L = 1 - ( a0*a0 + a1*a1 + a2*a2 + a3*a3 );

const float		lag_mult = .02;
const float		lag_max = .001;

int				count_dracula = 0;

				while (1) 
				{

					if ( fabs(L) < lag_max ) break;
					count_dracula++;

					a0 = Pel[i][(index+0)][0];
					a1 = Pel[i][(index+1)][0];
					a2 = Pel[i][(index+2)][0];
					a3 = Pel[i][(index+3)][0];


//					float m = sqrt(a0*a0 + a1*a1 + a2*a2 + a3*a3);
//					Pel[i][(index+0)][0] /= m;
//					Pel[i][(index+1)][0] /= m;
//					Pel[i][(index+2)][0] /= m;
//					Pel[i][(index+3)][0] /= m;

					Pel[i][(index+0)][0] += lag_mult*a0*L;
					Pel[i][(index+1)][0] += lag_mult*a1*L;
					Pel[i][(index+2)][0] += lag_mult*a2*L;
					Pel[i][(index+3)][0] += lag_mult*a3*L;

					L = 1 - ( a0*a0 + a1*a1 + a2*a2 + a3*a3 );

				}//Lagrange work...
			//conPhysics  << "Pel LP: " << L << "\n";

			//conPhysics << "L: " << count_dracula << "\n";

			}//Loop over constrained DOF...


	#if bCLAMP_PELVIS
		if (Pel_Usage[i] == epelHUMAN)
		{
/*
			// Calculate differences in position and rotation.
			float	lin_sqr = ( (store[0] - Pel[i][0][0])*(store[0] - Pel[i][0][0])
							  + (store[1] - Pel[i][1][0])*(store[1] - Pel[i][1][0])
  							  + (store[2] - Pel[i][2][0])*(store[2] - Pel[i][2][0]) );

			// Clamp motion.
			if (lin_sqr < 0.0001f * 0.0001f)
			{
				Pel[i][0][0] = store[0];
				Pel[i][1][0] = store[1];
				Pel[i][2][0] = store[2];

				Pel[i][0][1] = Pel[i][1][1] = Pel[i][2][1] = 0;
			}
*/
			// Only clamp rotation if positional vel is very small.
			if (Sqr(Pel[i][0][1]) + Sqr(Pel[i][1][1]) + Sqr(Pel[i][2][1]) < Sqr(0.001f))
			{
				// Now rotational vel
				float f_vel_sqr =		Pel[i][3][1] * Pel[i][3][1]
							  + Pel[i][4][1] * Pel[i][4][1]
							  + Pel[i][5][1] * Pel[i][5][1]
							  + Pel[i][6][1] * Pel[i][6][1];

	#define LOW_ROT_VELOCITY 0.01f
				if (f_vel_sqr < LOW_ROT_VELOCITY * LOW_ROT_VELOCITY)
				{
					Pel[i][3][0] = store[3];
					Pel[i][4][0] = store[4];
					Pel[i][5][0] = store[5];
					Pel[i][6][0] = store[6];
					
					Pel[i][3][1] = 0.0f;
					Pel[i][4][1] = 0.0f;
					Pel[i][5][1] = 0.0f;
					Pel[i][6][1] = 0.0f;

				}
			}
		}
	#endif

	}//Loop over pelvises...

//			Maximally Stoopid integration of biomodels...
#define MIN .003//.003//.004//.004                    //.003//.0025
float		ttime = delta_t;
			while (ttime > MIN)
			{
				integrate_field( MIN );
				ttime -= MIN;
			}
			//integrate_field( ttime );
	//conPhysics << "Active Pelvises: " << active << "\n";
}


char* pcSavePelvis(char* pc, int i_pel)
{
	// Save the 2 bio-field states.
	for (int i_bio = 2*i_pel; i_bio < 2*i_pel+2; i_bio++)
	{
		if (BioModels[i_bio])
		{
			memcpy(pc, BioModels[i_bio]->Field, sizeof(BioModels[i_bio]->Field));
			pc += sizeof(BioModels[i_bio]->Field);
		}
	}
	return pc;
}

const char* pcLoadPelvis(const char* pc, int i_pel)
{
	// Load the 2 bio-field states.
	for (int i_bio = 2*i_pel; i_bio < 2*i_pel+2; i_bio++)
	{
		if (BioModels[i_bio])
		{
			memcpy(BioModels[i_bio]->Field, pc, sizeof(BioModels[i_bio]->Field));
			pc += sizeof(BioModels[i_bio]->Field);
		}
		else if (CSaveFile::iCurrentVersion < 12)
			// Advance past bogus bio info.
			pc += sizeof(BioModels[i_bio]->Field);
	}
	return pc;
}

void PelReset()
{
	memset(Pel_Usage, 0, sizeof(Pel_Usage));
	memset(Pel_Box_BC, -1, sizeof(Pel_Box_BC));

	for (int i = 0; i < MAX_BIOMODELS; i++)
		BioModels[i] = 0;
}

#include "Arms.h"
#include "Lib/Std/ArrayIO.hpp"
#include "Lib/Std/Set.hpp"

extern SControlArm	ControlArm;
extern bool	ArmUnderControl;
extern float Pelvis_Set_Crouch;

void DumpPelState(std::ostream& os)
{
#if VER_TEST

	int i, j, k;

	for (i = 0; i < NUM_PELVISES; i++)
	if (Pel_Usage[i])
	{
		os	<<"Pelvis " <<i <<" Type=" <<(int)Pel_Usage[i]
			<< std::endl;

		os	<<"  Box_BC=" <<CPArray<int>(PELVIS_DOF, Pel_Box_BC[i]) << std::endl;

		os	<<"  State=";
		for (j = 0; j < PELVIS_DOF; j++)
		{
//			os <<endl <<"    " <<CPArray<float>(3, Pel[i][j]);
			os << std::endl <<"    ";
			for (k = 0; k < 3; k++)
				os <<double(Pel[i][j][k]) <<" ";
		}

		os	<<"  History=" <<CPArray<float>(PELVIS_DOF, History[i]) << std::endl;

		os	<<"  BioTag=" <<CPArray<float>(PELVIS_DOF, BioTag[i]) << std::endl;

		os	<<"  Data=" <<CPArray<float>(PELVIS_PARAMETERS, Pel_Data[i]) << std::endl;

		os	<<"  Hand_Drop_Flag="	<<Hand_Drop_Flag[i]
			<<" Kontrol="			<<CPArray<float>(6, Kontrol[i])
			<<" Kontrol_Mouth="		<<Kontrol_Mouth[i]
			<<" bKontrol_Krouch="	<<bKontrol_Krouch[i]
			<<" iKontrol_Jump="		<<iKontrol_Jump[i]
			<<" asFootLatch="		<<(int)asFootLatch[i]
			<< std::endl;
		os	<< std::endl;
	}
	os	<< std::endl;

	// Misc stuff.
	os	<<"ControlArm: PosUrge=" <<ControlArm.fPosUrgency
		<<" OrientUrge=" <<ControlArm.fOrientUrgency
		<<" Curl=" <<ControlArm.fFingerCurl
		<<" UnderControl=" <<ArmUnderControl
		<< std::endl;

	os	<<"Pelvis_Set_Crouch="	<<Pelvis_Set_Crouch
		<<" Pelvis_Jump="	<<CPArray<float>(3, Pelvis_Jump)
		<< std::endl;

	os	<< std::endl;

#endif
}


