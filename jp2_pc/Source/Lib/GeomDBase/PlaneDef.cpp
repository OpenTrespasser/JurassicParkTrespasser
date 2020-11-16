/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Member functions of CPlaneDef.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/PlaneDef.cpp                                            $
 * 
 * 9     98.02.04 4:56p Mmouni
 * Inlined inverse square root estimation in asm version.
 * 
 * 8     97.12.10 3:01p Mmouni
 * Changed fSqrt, fInvSqrt to fSqrtEst, fInvSqrtEst to reflect changes in the functions.
 * 
 * 7     11/09/97 10:03p Gfavor
 * Merged FastMake and FastMake3DX.
 * 
 * 6     11/05/97 4:09p Gfavor
 * Tweaked FastMake3DX for predecode issues.
 * 
 * 6     97.11.05 3:30p Mmouni
 * Added #if to prevent assembly of 3DX instructions for non-3DX processor targets.
 * 
 * 5     11/04/97 7:02p Gfavor
 * Set up and enabled FastMake3DX
 * 
 * 4     10/29/97 8:07p Gfavor
 * Converted FastMake to 3DX.
 * 
 * 3     97/10/23 10:57a Pkeet
 * Added a K6 3D switch.
 * 
 * 2     97/06/24 2:48p Pkeet
 * The fast plane equation builder now uses the fast inverse sqrt function.
 * 
 * 1     97/05/23 5:52p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Plane.hpp"
#include "Lib/Math/FastSqrt.hpp"
#include "AsmSupport.hpp"


//
// Defines.
//

// Switch to use the fast inverse square root function.
#define bUSE_FAST_INVERSE_SQRT (1)


//
// Class implementations.
//

//**********************************************************************************************
//
// CPlaneDef implementation.
//

	//******************************************************************************************
	CPlaneDef::CPlaneDef(const CVector3<>& v3_a, const CVector3<>& v3_b, const CVector3<>& v3_c)
	{
	#if VER_ASM

		struct SVecPlaneDef
		{
			float fX;
			float fY;
			float fZ;
			float fD;
		};
		float r;

		__asm
		{

			// CVector3<> v3;
			// CVector3<> v3a;
			// CVector3<> v3b;
			// v3a = v3_c - v3_b;
			// v3b = v3_a - v3_b;

			mov eax, [v3_a]
			mov ebx, [v3_b]
			mov ecx, [v3_c]

			fld   [ecx]SVecPlaneDef.fX
			fsub  [ebx]SVecPlaneDef.fX	// v3a.tX

			fld   [ecx]SVecPlaneDef.fY
			fsub  [ebx]SVecPlaneDef.fY	// v3a.tY, v3a.tX

			fld   [ecx]SVecPlaneDef.fZ
			fsub  [ebx]SVecPlaneDef.fZ	// v3a.tZ, v3a.tY, v3a.tX

			fld   [eax]SVecPlaneDef.fX
			fsub  [ebx]SVecPlaneDef.fX	// v3b.tX, v3a.tZ, v3a.tY, v3a.tX

			fld   [eax]SVecPlaneDef.fY
			fsub  [ebx]SVecPlaneDef.fY	// v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3a.tX

			fld   [eax]SVecPlaneDef.fZ
			fsub  [ebx]SVecPlaneDef.fZ	// v3b.tZ, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3a.tX
			fxch  st(5)					// v3a.tX, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3b.tZ
			
			// v3.tX = v3a.tY * v3b.tZ  -  v3a.tZ * v3b.tY; 
			// v3.tY = v3a.tZ * v3b.tX  -  v3a.tX * v3b.tZ;
			// v3.tZ = v3a.tX * v3b.tY  -  v3a.tY * v3b.tX;

			fld   st(0)			// v3a.tX, v3a.tX, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3b.tZ
			fmul  st(0), st(2)	// v3a.tX * v3b.tY, v3a.tX, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3b.tZ

			fxch  st(1)			// v3a.tX, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3b.tZ
			fmul  st(0), st(6)	// v3a.tX * v3b.tZ, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3b.tZ
			fxch  st(6)			// v3b.tZ, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3a.tX * v3b.tZ

			fmul  st(0), st(5)	// v3b.tZ * v3a.tY, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3a.tX * v3b.tZ
			fxch  st(5)	// v3a.tY, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3b.tZ * v3a.tY, v3a.tX * v3b.tZ
			fmul  st(0), st(3)	// v3a.tY * v3b.tX, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3b.tZ * v3a.tY, v3a.tX * v3b.tZ

			fxch  st(2)	// v3b.tY, v3a.tX * v3b.tY, v3a.tY * v3b.tX, v3b.tX, v3a.tZ,
						// v3b.tZ * v3a.tY, v3a.tX * v3b.tZ

			fmul  st(0), st(4)	// v3b.tY * v3a.tZ, v3a.tX * v3b.tY, v3a.tY * v3b.tX, v3b.tX, v3a.tZ,
								// v3b.tZ * v3a.tY, v3a.tX * v3b.tZ

			fxch  st(3)			// v3b.tX, v3a.tX * v3b.tY, v3a.tY * v3b.tX, v3b.tY * v3a.tZ, v3a.tZ,
								// v3b.tZ * v3a.tY, v3a.tX * v3b.tZ

			fmulp st(4), st(0)	// za, zb, xb, ya, xa, yb
			fxch  st(3)			// ya, zb, xb, za, xa, yb
			fsubrp st(5), st(0)	// zb, xb, za, xa, v3.tY
			fsubp st(2), st(0)	// xb, v3.tZ, xa, v3.tY
			fsubp st(2), st(0)	// v3.tZ, v3.tX, v3.tY

			// r = v3.tX * v3.tX + v3.tY * v3.tY + v3.tZ * v3.tZ;

			//fst   v3.tZ
			//fst   v3.tX
			//fst   v3.tY	// v3.tZ, v3.tX, v3.tY

			fld   st(2)			// v3.tY, v3.tZ, v3.tX, v3.tY
			fmul  st(0), st(3)	// y2, v3.tZ, v3.tX, v3.tY

			fld   st(2)			// v3.tX, y2, v3.tZ, v3.tX, v3.tY
			fmul  st(0), st(3)	// x2, y2, v3.tZ, v3.tX, v3.tY

			fld   st(2)			// v3.tZ, x2, y2, v3.tZ, v3.tX, v3.tY
			fmul  st(0), st(3)	// z2, x2, y2, v3.tZ, v3.tX, v3.tY

			fxch  st(2)			// y2, x2, z2, v3.tZ, v3.tX, v3.tY
			fadd
			fadd				// r, v3.tZ, v3.tX, v3.tY

			//r = (float)sqrt(r);
			fsqrt

			//r = 1.0f / r;
			fld1
			fdivrp st(1), st(0)		// r, v3.tZ, v3.tX, v3.tY
			mov   esi, this

			//v3 *= r;
			fmul  st(2), st(0)
			fmul  st(3), st(0)
			fmul
			fxch  st(2)			// y, x, z
			//fstp  r
			//fstp  v3.tY	
			//fstp  v3.tX
			//fstp  v3.tZ

			
			fld   [eax]SVecPlaneDef.fX	// ax, y, x, z
			fmul  st(0), st(2)			// xx, y, x, z
			fld   [eax]SVecPlaneDef.fY	// ay, xx, y, x, z
			fmul  st(0), st(2)			// yy, xx, y, x, z
			fld   [eax]SVecPlaneDef.fZ	// az, yy, xx, y, x, z
			fmul  st(0), st(5)			// zz, yy, xx, y, x, z
			fxch  st(2)					// xx, yy, zz, y, x, z

			fadd
			fadd 						// r, y, x, z
			fxch st(1)					// y, r, x, z

			//d3Normal = v3;
			mov   edx, 0x80000000
			fstp  [esi]SVecPlaneDef.fY
			fstp  r
			xor   edx, r
			fstp  [esi]SVecPlaneDef.fX
			fstp  [esi]SVecPlaneDef.fZ

			//rD = -r;
			mov   [esi]SVecPlaneDef.fD, edx
		}

	#else // VER_ASM

		d3Normal = (v3_c - v3_b) ^ (v3_a - v3_b);
		rD       = -(d3Normal * v3_a);

	#endif // VER_ASM
	}


	//******************************************************************************************
	void CPlaneDef::FastMake(const CVector3<>& v3_a, const CVector3<>& v3_b,
		                     const CVector3<>& v3_c)
	{
	#if (TARGET_PROCESSOR == PROCESSOR_K6_3D && VER_ASM)

		struct SVecPlaneDef
		{
			float fX;
			float fY;
			float fZ;
			float fD;
		};

		__asm
		{
			jmp		StartAsm

			align	16
			nop									;establish 10 byte starting code offset
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop

		StartAsm:		

			// femms							;caller responsible for this

			// CVector3<> v3;
			// CVector3<> v3a;
			// CVector3<> v3b;

			mov		ebx,[v3_b]
			mov		edx,[v3_c]
			mov		eax,[v3_a]

			;eax = v3_a ptr
			;ebx = v3_b ptr
			;ecx = This ptr
			;edx = v3_c ptr

			// v3a = v3_c - v3_b;
			// v3b = v3_a - v3_b;

			// v3.tX = v3a.tY * v3b.tZ  -  v3a.tZ * v3b.tY;
			// v3.tY = v3a.tZ * v3b.tX  -  v3a.tX * v3b.tZ;
			// v3.tZ = v3a.tX * v3b.tY  -  v3a.tY * v3b.tX;

			// r = v3.tX * v3.tX + v3.tY * v3.tY + v3.tZ * v3.tZ;

			movq	mm0,[ebx]SVecPlaneDef.fX	;m0= v3_b.Y | v3_b.X
			movq	mm1,[edx]SVecPlaneDef.fX	;m1= v3_c.Y | v3_c.X
			movq	mm2,[eax]SVecPlaneDef.fX	;m2= v3_a.Y | v3_a.X

			movd	mm3,[ebx]SVecPlaneDef.fZ	;m3=   0   | v3_b.Z
			pfsub	(m1,m0)						;m1= v3a.Y | v3a.X

			movd	mm4,[edx]SVecPlaneDef.fZ	;m4=   0   | v3_c.Z
			pfsub	(m2,m0)						;m2= v3b.Y | v3b.X

			movd	mm5,[eax]SVecPlaneDef.fZ	;m5=   0   | v3_a.Z
			movq	mm0,mm1						;m0= v3a.Y | v3a.X

			test	eax,eax						;2-byte NOOP to avoid 
												;  degraded predecode
			pfsub	(m4,m3)						;m4=   0   | v3a.Z
			punpckhdq mm0,mm0					;m0= v3a.Y | v3a.Y

			pfsub	(m5,m3)						;m5=   0   | v3b.Z
			movq	mm3,mm2						;m3= v3b.Y | v3b.X

			punpckldq mm0,mm4					;m0= v3a.Z | v3a.Y
			punpckhdq mm3,mm3					;m3= v3b.Y | v3b.Y

			pfmul	(m0,m2)						;m0= v3a.Z*v3b.Y | v3a.Y*v3b.X
			punpckldq mm3,mm5					;m3= v3b.Z | v3b.Y

			pfmul	(m3,m1)						;m3= v3b.Z*v3a.Y | v3b.Y*v3a.X
			movq	mm6,mm1						;m6= v3a.Y | v3a.X

			pfmul	(m6,m5)						;m6=   0   | v3a.X*v3b.Z
			movq	mm7,mm2						;m7= v3b.Y | v3b.X

			pfmul	(m7,m4)						;m7=   0   | v3b.X*v3a.Z
			pfsub	(m3,m0)						;m3= v3.X  | v3.Z


			pfsub	(m7,m6)						;m7=   0   | v3.Y
			movq	mm2,mm3						;m2= v3.X  | v3.Z

			pfmul	(m2,m2)						;m2= v3.X*v3.X | v3.Z*v3.Z

			movq	mm6,mm7						;m6=   0   | v3.Y

			pfmul	(m6,m6)						;m6=   0   | v3.Y*v3.Y
			pfacc	(m2,m2)						;m2= v3.X*v3.X + v3.Z*v3.Z


			pfadd	(m2,m6)						;m2= r

			// r = 1.0f / (float)sqrt(r);
			// v3 *= r;

			movd	mm4,[eax]SVecPlaneDef.fZ	;m4= 0 | v3_a.Z

			pfrsqrt	(m2,m2)						;m2= 1/sqrt(r) | ...
			movd	mm5,[eax]SVecPlaneDef.fX	;m5= 0 | v3_a.X

			test	eax,eax						;2-byte NOOP to avoid 
												;  degraded predecode

			pfmul	(m3,m2)						;m3= v3.X/sqrt(r) | v3.Z/sqrt(r)
			;									;  =  d3Normal.X  | d3Normal.Z
			movd	mm6,[eax]SVecPlaneDef.fY	;m6= 0 | v3_a.Y

			pfmul	(m7,m2)						;m7= 0 | v3.Y/sqrt(r) = d3Normal.Y
			punpckldq mm4,mm5					;m4= v3_a.X | v3_a.Z

			// d3Normal = v3;
			// rD = -(d3Normal * v3_a);

			pfmul	(m4,m3)						;m4= v3_a.X*d3Normal.X 
			;									;    | v3_a.Z*d3Normal.Z
			movd	[ecx]SVecPlaneDef.fZ,mm3	;store d3Normal.Z

			pfmul	(m6,m7)						;m6= v3_a.Y*d3Normal.Y 
			psrlq	mm3,32						;m3= d3Normal.X

			pfacc	(m4,m4)						;m4= ...X + ...Z
			movd	[ecx]SVecPlaneDef.fX,mm3	;store d3Normal.X


			pfadd	(m4,m6)						;m4= ...X + ...Z + ...Y = "rD"
			movd	[ecx]SVecPlaneDef.fY,mm7	;store d3Normal.Y

			pxor	mm5,mm5						;m5= 0.0
			test	eax,eax						;2-byte NOOP to avoid 
												;  degraded predecode
			pfsubr	(m4,m5)						;m4= -"rD" = rD
			movd	[ecx]SVecPlaneDef.fD,mm4	;store rD

			// femms							;caller responsible for this
		}

	#elif (VER_ASM)	// if (TARGET_PROCESSOR == PROCESSOR_K6_3D && VER_ASM)

		struct SVecPlaneDef
		{
			float fX;
			float fY;
			float fZ;
			float fD;
		};
		float r;

		__asm
		{

			// CVector3<> v3;
			// CVector3<> v3a;
			// CVector3<> v3b;
			// v3a = v3_c - v3_b;
			// v3b = v3_a - v3_b;

			mov eax, [v3_a]
			mov ebx, [v3_b]
			mov ecx, [v3_c]

			fld   [ecx]SVecPlaneDef.fX
			fsub  [ebx]SVecPlaneDef.fX	// v3a.tX

			fld   [ecx]SVecPlaneDef.fY
			fsub  [ebx]SVecPlaneDef.fY	// v3a.tY, v3a.tX

			fld   [ecx]SVecPlaneDef.fZ
			fsub  [ebx]SVecPlaneDef.fZ	// v3a.tZ, v3a.tY, v3a.tX

			fld   [eax]SVecPlaneDef.fX
			fsub  [ebx]SVecPlaneDef.fX	// v3b.tX, v3a.tZ, v3a.tY, v3a.tX

			fld   [eax]SVecPlaneDef.fY
			fsub  [ebx]SVecPlaneDef.fY	// v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3a.tX

			fld   [eax]SVecPlaneDef.fZ
			fsub  [ebx]SVecPlaneDef.fZ	// v3b.tZ, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3a.tX
			fxch  st(5)					// v3a.tX, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3b.tZ
			
			// v3.tX = v3a.tY * v3b.tZ  -  v3a.tZ * v3b.tY; 
			// v3.tY = v3a.tZ * v3b.tX  -  v3a.tX * v3b.tZ;
			// v3.tZ = v3a.tX * v3b.tY  -  v3a.tY * v3b.tX;

			fld   st(0)			// v3a.tX, v3a.tX, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3b.tZ
			fmul  st(0), st(2)	// v3a.tX * v3b.tY, v3a.tX, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3b.tZ

			fxch  st(1)			// v3a.tX, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3b.tZ
			fmul  st(0), st(6)	// v3a.tX * v3b.tZ, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3b.tZ
			fxch  st(6)			// v3b.tZ, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3a.tX * v3b.tZ

			fmul  st(0), st(5)	// v3b.tZ * v3a.tY, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3a.tY, v3a.tX * v3b.tZ
			fxch  st(5)	// v3a.tY, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3b.tZ * v3a.tY, v3a.tX * v3b.tZ
			fmul  st(0), st(3)	// v3a.tY * v3b.tX, v3a.tX * v3b.tY, v3b.tY, v3b.tX, v3a.tZ, v3b.tZ * v3a.tY, v3a.tX * v3b.tZ

			fxch  st(2)	// v3b.tY, v3a.tX * v3b.tY, v3a.tY * v3b.tX, v3b.tX, v3a.tZ,
						// v3b.tZ * v3a.tY, v3a.tX * v3b.tZ

			fmul  st(0), st(4)	// v3b.tY * v3a.tZ, v3a.tX * v3b.tY, v3a.tY * v3b.tX, v3b.tX, v3a.tZ,
								// v3b.tZ * v3a.tY, v3a.tX * v3b.tZ

			fxch  st(3)			// v3b.tX, v3a.tX * v3b.tY, v3a.tY * v3b.tX, v3b.tY * v3a.tZ, v3a.tZ,
								// v3b.tZ * v3a.tY, v3a.tX * v3b.tZ

			fmulp st(4), st(0)	// za, zb, xb, ya, xa, yb
			fxch  st(3)			// ya, zb, xb, za, xa, yb
			fsubrp st(5), st(0)	// zb, xb, za, xa, v3.tY
			fsubp st(2), st(0)	// xb, v3.tZ, xa, v3.tY
			fsubp st(2), st(0)	// v3.tZ, v3.tX, v3.tY

			// r = v3.tX * v3.tX + v3.tY * v3.tY + v3.tZ * v3.tZ;

			//fst   v3.tZ
			//fst   v3.tX
			//fst   v3.tY	// v3.tZ, v3.tX, v3.tY

			fld   st(2)			// v3.tY, v3.tZ, v3.tX, v3.tY
			fmul  st(0), st(3)	// y2, v3.tZ, v3.tX, v3.tY

			fld   st(2)			// v3.tX, y2, v3.tZ, v3.tX, v3.tY
			fmul  st(0), st(3)	// x2, y2, v3.tZ, v3.tX, v3.tY

			fld   st(2)			// v3.tZ, x2, y2, v3.tZ, v3.tX, v3.tY
			fmul  st(0), st(3)	// z2, x2, y2, v3.tZ, v3.tX, v3.tY

			fxch  st(2)			// y2, x2, z2, v3.tZ, v3.tX, v3.tY
			fadd
			fadd				// r, v3.tZ, v3.tX, v3.tY

	#if bUSE_FAST_INVERSE_SQRT

			fstp	[r]

			mov		edx,[r]

			// i4_table_index   = if_val & (i4FLOAT_MANT_MASK | BitMask(iFLOAT_EXP_BIT_START));
			mov		ebx,edx
			and		edx,0x00ffffff

			// i4_table_index >>= iFLOAT_MANT_BIT_WIDTH - iFAST_INV_SQRT_TABLE_SIZE_BITS + 1;
			shr		edx,iFLOAT_MANT_BIT_WIDTH-iFAST_INV_SQRT_TABLE_SIZE_BITS+1

			// if_val.i4Int &= i4FLOAT_EXP_MASK;
			and		ebx,0x7f800000

			// if_val = ((3 * iFLOAT_EXP_BIAS - 1) << (iFLOAT_EXP_BIT_START - 1)) - (if_val >> 1);
			shr		ebx,1
			mov		ecx,380 << 22

			sub		ecx,ebx

			// if_val.i4Int &= i4FLOAT_EXP_MASK;
			and		ecx,0x7f800000

			// if_val.i4Int |= ai4InvSqrtMantissaTable[i4_table_index];
			mov		edx,ai4InvSqrtMantissaTable[edx*4]
			or		ecx,edx

			mov		[r],ecx

			fld		[r]

	#else // if bUSE_FAST_INVERSE_SQRT

			fsqrt					// r = (float)sqrt(r);
			fld1					// r = 1.0f / r;
			fdivrp st(1), st(0)		// r, v3.tZ, v3.tX, v3.tY

	#endif // else

			mov   esi,this

			//v3 *= r;
			fmul  st(2), st(0)
			fmul  st(3), st(0)
			fmul
			fxch  st(2)			// y, x, z
			//fstp  r
			//fstp  v3.tY	
			//fstp  v3.tX
			//fstp  v3.tZ
			
			fld   [eax]SVecPlaneDef.fX	// ax, y, x, z
			fmul  st(0), st(2)			// xx, y, x, z
			fld   [eax]SVecPlaneDef.fY	// ay, xx, y, x, z
			fmul  st(0), st(2)			// yy, xx, y, x, z
			fld   [eax]SVecPlaneDef.fZ	// az, yy, xx, y, x, z
			fmul  st(0), st(5)			// zz, yy, xx, y, x, z
			fxch  st(2)					// xx, yy, zz, y, x, z

			fadd
			fadd 						// r, y, x, z
			fxch st(1)					// y, r, x, z

			//d3Normal = v3;
			mov   edx, 0x80000000
			fstp  [esi]SVecPlaneDef.fY
			fstp  r
			xor   edx, r
			fstp  [esi]SVecPlaneDef.fX
			fstp  [esi]SVecPlaneDef.fZ

			//rD = -r;
			mov   [esi]SVecPlaneDef.fD, edx
		}

	#else // elif VER_ASM

		d3Normal = (v3_c - v3_b) ^ (v3_a - v3_b);
		rD       = -(d3Normal * v3_a);

	#endif // else
	}
