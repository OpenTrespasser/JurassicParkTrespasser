/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Minimal pelvis definitions needed for other modules.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/pelvis_def.h                                              $
 * 
 * 15    8/29/98 3:59p Sblack
 * 
 * 14    8/27/98 9:25a Sblack
 * 
 * 13    8/19/98 2:45p Sblack
 * 
 * 12    8/03/98 10:54a Sblack
 * 
 * 11    5/23/98 10:42a Sblack
 * 
 * 10    5/21/98 8:59p Sblack
 * 
 * 9     5/12/98 12:09a Sblack
 * 
 * 8     98/05/11 17:35 Speter
 * Added pelvis type for quads.
 * 
 * 7     12/09/97 2:09p Sblack
 * 
 * 6     97/12/05 16:50 Speter
 * Added file header.  #include <bool.h>.  Added EPelvisType.
 * 
 **********************************************************************************************/

#ifndef _HEADER_PHYSICS_PELVIS_DEF_H
#define _HEADER_PHYSICS_PELVIS_DEF_H

//	Pelvis definitions...
//	---------------------

//	Quit your bitching!
#pragma warning(disable : 4244)

// Get the STL definition of the bool type and the true and false constants.
#include <bool.h>

//	Amount of shit...
#define	NUM_PELVISES 8
#define PELVIS_PARAMETERS 55
#define PELVIS_DOF 64

//	Definitions to aid in our journey...
#define	BODY 0
#define BODY_EULER 3
#define	RIGHT_FOOT 7
#define RIGHT_EULER 10
#define LEFT_FOOT 14
#define LEFT_EULER 17
#define MOUTH 21
#define RIGHT_PHI 22
#define	BALANCE_THETA 23
#define	RIGHT_HAND 25
#define RIGHT_HAND_EULER 28
#define FRIGHT_FOOT 32
#define FRIGHT_EULER 35
#define FLEFT_FOOT 39
#define FLEFT_EULER 42
#define HEAD_BC 46
#define DEAD_BC_EULER 49
#define TAIL_BC 53
#define TAIL_BC_EULER 56
#define HEAD_CONTROL 60


// Prefix: epel
enum EPelvisType
{
	epelEMPTY,			// Slot is empty.
	epelHUMAN,			// The most highly evolved animal.
	epelRAPTOR,			// An animal that no longer really exists.
	epelQUAD			// Two animals (OK, really just one) that no longer really exist.
};


#endif


//	Data contents:
//	Index:	|Contents:
//	--------|---------------
//	0		|Toe x
//	1		|Toe y
//	2		|Foot BC radius
//	3		|Foot BC kappa
//	4		|Foot BC delta
//	5		|Foot dynamic mu
//	6		|Foot static mu
//	7-9		|Foot inverse MOI, a, b, g
//	10-12	|Foot MOI, a, b, g
//	13		|Hip
//	14		|Leg extended length
//	15		|Leg kappa
//	16		|Leg delta
//	17		|Ankle alpha coupling 
//	18		|Ankle beta coupling 
//	19		|Ankle global gamma coupling
//	20		|Ankle alpha detent
//	21		|Percentage leg extention rest position...
//	22		|Ankle alpha delta
//	23		|Ankle beta delta
//	24		|Ankle gamma delta
//	25-27	|Pelvic inverse Ia, Ib, Ig
//	28-30	|Pelvic I's...
//	31		|inverse CG mass...
//	32,33	|Right and Left foot inverse masses...
//	34		|Thigh_l...
//	35		|Shin_l...
//	36		|Balance skill 0-1...
//	37,39	|Shoulder offset...
//	40		|Arm max length, assume equal joint lengths...
//	41		|Hand length...
//	42		|Hand width...
//	43		|Finger length...
//	44		|Heel offset...
//	45		|Front Hips...
//	47		|CM displacement..
//	48		|Neck length....
//	49		|HP...
//	50		|Length of front legs for quads...
//	51, 52	|Equiv of 34, 35 for front...
//	53		|Front foot Z offset...
//	54		|Rear foot Z offset...

