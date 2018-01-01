/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Definitions for pelvis model lib.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/pelvis.h                                                  $
 * 
 * 20    98/10/03 22:32 Speter
 * Inlined health functions.
 * 
 * 19    98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 18    8/26/98 10:53p Asouth
 * added expanded forward decl of ostream
 * 
 * 17    8/25/98 6:26p Rvande
 * Removed forward declaration that clashed with MSL
 * 
 * 16    98/07/24 14:04 Speter
 * Added pelvis state load/save functions.
 * 
 * 15    98/07/20 13:13 Speter
 * PelReset(), DumpPelState().
 * 
 * 14    98/07/08 1:11 Speter
 * Removed SetBoxID; CreateBoxModel now performs this.
 * 
 * 13    98/06/14 23:54 Speter
 * Added UpdateBioBox function.
 * 
 * 12    6/04/98 10:41a Sblack
 * 
 * 11    98/03/05 13:16 Speter
 * Modified Make_a_Pelvis and related functions to take opaque CInstance param, and init state
 * in quaternions rather than angles.  Pel_Box_BC expanded to include linkages for all joints.
 * SetBoxID changed accordingly.
 * 
 * 10    3/02/98 10:32a Gfavor
 * Fixed bug in 3DX version of SafeNormalize.
 * 
 * 9     3/01/98 7:50p Gfavor
 * Converted Normalize and SafeNormalize to 3DX.
 * 
 * 8     98/02/16 14:36 Speter
 * Added HandDrop() function.
 * 
 * 7     98/01/28 17:48 Speter
 * Made all "PelTools" functions inline.
 * 
 * 6     12/18/97 2:47p Sblack
 * 
 * 5     97/12/05 16:51 Speter
 * Added file header.  Added Pel_Usage[] array.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_PELVIS_HPP
#define HEADER_LIB_PHYSICS_PELVIS_HPP

#include <math.h>
#include "Xob_bc.hpp"
#include "Pelvis_Def.h"
#include "AsmSupport.hpp"

class CInstance;
#ifdef __MWERKS__
// The MW build uses templated stream classes;
// ostream
 namespace std {
  template<class charT, class traits> class basic_ostream;
  template<class charT> struct char_traits;
  typedef basic_ostream<char,char_traits<char> > ostream; 
 };
#else
 class ostream;
#endif

//	The shit itself...
extern EPelvisType	Pel_Usage[ NUM_PELVISES ];					//Type of pelvis here, if any.
extern float	Pel[ NUM_PELVISES ][PELVIS_DOF][3];				//Body, right, left...
extern float	Pel_Data[ NUM_PELVISES ][ PELVIS_PARAMETERS ];	//As defined below...
extern int		Pel_Box_BC[ NUM_PELVISES ][ PELVIS_DOF ];		//For box handled B/C...-1 if no box.
extern float	History[ NUM_PELVISES ][ PELVIS_DOF ];
extern int		Hand_Drop_Flag[ NUM_PELVISES ];					//Flag set by physics to inform
extern float	Pelvis_Jump[3];
extern bool		Pelvis_Jump_Voluntary;
																//control to drop object.

//	Things that pelvises do... Actually the functions in Pelvis.cpp...
//	------------------------------------------------------------------

//	Find the equation of motion and stuff from B/C...
void Pelvic_IDOF( int me, float timestep );

//	Initialize a single pelvic model, keeping it really simple for now...
void Make_a_Pelvis( CInstance* pins, int number, float params[PELVIS_PARAMETERS], float init[7][3], float box_params[10] );

//	Used to remove a pelvis from the system.
void Remove_Pelvis( int pelvisid );

//	Integrate with Lagrange constraints...
void Pelvis_integrate( float delta_t );

//	Sets this box to its linked bio-state.
void UpdateBioBox(int bcindex);

//	Returns a pelvis' health, 1-100...
inline float GetBioHealth( int index )
{
	Assert(index >= 0);
	return Pel_Data[index][49];
}

//	Sets a pelvis' health...
inline void SetBioHealth( int index, float value )
{
	Assert(index >= 0);
	Pel_Data[index][49] = value;
}


inline void HandDrop( int pelvisid )
{
	Hand_Drop_Flag[pelvisid] = 1;
}

//*****************************************************************************************
inline CXob& PelBox(int i_pel, int i_elem)
{
	Assert(i_pel >= 0 && i_elem >= 0);
	Assert(Pel_Box_BC[i_pel][i_elem] >= 0);
	return Xob[ Pel_Box_BC[i_pel][i_elem] ];
}

//	Loads and saves pelvis state...
char* pcSavePelvis(char* pc, int i_pel);

const char* pcLoadPelvis(const char* pc, int i_pel);

//	Resets the pelvis arrays...
void PelReset();

//	Dumps the state to a text file...
void DumpPelState( ostream& os );

//	Tools
//	-----
//	D = ( A, B, C )
inline void StuffVec( float A, float B, float C, float D[3] )
{
	D[0] = A;
	D[1] = B;
	D[2] = C;
}

//	C = A + B
inline void AddVec( float A[3], float B[3], float C[3] )
{
	C[0] = A[0] + B[0];
	C[1] = A[1] + B[1];
	C[2] = A[2] + B[2];
}

//	B -> A
inline void CopyVec( float A[3], float B[3] )
{
	B[0] = A[0];
	B[1] = A[1];
	B[2] = A[2];
}

//	B += A
inline void UnaryAddVec( float A[3], float B[3] )
{
	B[0] += A[0];
	B[1] += A[1];
	B[2] += A[2];
}

//	A[i] = 0
inline void ZeroVecDim( float A[], int i )
{
	int	arse;
	for (arse = 0; arse < i; arse++) A[arse] = 0;
}

//	V -> V^, mag = |V|, imag = 1/|V|
inline void Normalize( float V[3], float &mag, float &imag )
{
#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

	__asm
	{
		mov		eax,[V]						;eax= ptr to V[]

		#ifndef COMPILED_SCALAR_3DX
			femms							;ensure fast switch when not called from 
		#endif								;  code compiled into scalar 3DX

		align	16

		movq	mm0,[eax]					;m0= V[1] | V[0]
		test	eax,eax						;2-byte NOOP to avoid degraded predecode

		movd	mm1,[eax+8]					;m1= V[2]

		movq	mm2,mm0						;m2= V[1] | V[0]
		pfmul	(m0,m0)						;m0= V[1]*V[1] | V[0]*V[0]

		movq	mm3,mm1						;m3= V[2]
		pfmul	(m1,m1)						;m1= V[2]*V[2]

		pfacc	(m0,m0)						;m0= V[0]*V[0]+V[1]*V[1]
		mov		ebx,[imag]					;ebx= ptr to imag

		pfadd	(m0,m1)						;m0= V[0]*V[0]+V[1]*V[1]+V[2]*V[2]

		pfrsqrt	(m1,m0)						;m1= 1/len | 1/len
		movd	[ebx],mm1					;update imag

		mov		ebx,[mag]					;ebx= ptr to mag

		pfmul	(m2,m1)						;m2= V[1]/len | V[0]/len
		movq	[eax],mm2

		pfmul	(m3,m1)						;m3= V[2]/len
		movd	[eax+8],mm3

		pfmul	(m0,m1)						;m0= len
		movd	[ebx],mm0					;update mag

		#ifndef COMPILED_SCALAR_3DX
			femms							;clear MMX state and ensure fast switch when not 
		#endif								;  called from code compiled into scalar 3DX
	}

#else // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

	mag = sqrt( V[0]*V[0] + V[1]*V[1] + V[2]*V[2] );
	imag = 1/mag;
	V[0] *= imag;
	V[1] *= imag;
	V[2] *= imag;

#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
}

//	V -> V^, mag = |V|, imag = 1/|V| IFF mag > 0, else mag = 0, etc...
inline void SafeNormalize( float V[3], float &mag, float &imag )
{
#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

	__asm
	{
		mov		eax,[V]						;eax= ptr to V[]

		#ifndef COMPILED_SCALAR_3DX
			femms							;ensure fast switch when not called from 
		#endif								;  code compiled into scalar 3DX

		align	16

		movq	mm0,[eax]					;m0= V[1] | V[0]
		test	eax,eax						;2-byte NOOP to avoid degraded predecode

		movd	mm1,[eax+8]					;m1= V[2]

		movq	mm2,mm0						;m2= V[1] | V[0]
		pfmul	(m0,m0)						;m0= V[1]*V[1] | V[0]*V[0]

		movq	mm3,mm1						;m3= V[2]
		pfmul	(m1,m1)						;m1= V[2]*V[2]

		pfacc	(m0,m0)						;m0= V[0]*V[0]+V[1]*V[1]
		mov		ebx,[imag]					;ebx= ptr to imag

		pfadd	(m0,m1)						;m0= V[0]*V[0]+V[1]*V[1]+V[2]*V[2]

		pfrsqrt	(m1,m0)						;m1= 1/len | 1/len
		movd	[ebx],mm1					;update imag

		mov		ebx,[mag]					;ebx= ptr to mag

		pfmul	(m2,m1)						;m2= V[1]/len | V[0]/len
		movq	[eax],mm2

		pfmul	(m3,m1)						;m3= V[2]/len
		movd	[eax+8],mm3

		pfmul	(m0,m1)						;m0= len
		movd	[ebx],mm0					;update mag

		#ifndef COMPILED_SCALAR_3DX
			femms							;clear MMX state and ensure fast switch when not 
		#endif								;  called from code compiled into scalar 3DX
	}

	if ( imag > 1000 )
	{
		imag = 0;
		mag = 0;
		V[0] = 0;
		V[1] = 0;
		V[2] = 0;
	}

#else // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

	mag = sqrt( V[0]*V[0] + V[1]*V[1] + V[2]*V[2] );
	if ( mag > 0.001 )
	{
		imag = 1/mag;
		V[0] *= imag;
		V[1] *= imag;
		V[2] *= imag;
	}
	else imag = mag = V[0] = V[1] = V[2] = 0;

#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
}


//	returns AB inner product
inline float Inner( float A[3], float B[3] ) 
{ 
	return A[0]*B[0] + A[1]*B[1] + A[2]*B[2]; 
}

//	returns C = outer(A, B)...
inline void Outer( float A[3], float B[3], float C[3] )
{
	C[0] = A[1]*B[2] - A[2]*B[1];
	C[1] = A[2]*B[0] - A[0]*B[2];
	C[2] = A[0]*B[1] - A[1]*B[0];
}

#endif