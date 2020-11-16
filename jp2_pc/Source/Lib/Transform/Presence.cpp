/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Presence.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/Presence.cpp                                            $
 * 
 * 14    9/24/98 5:46p Asouth
 * stupid programmer error
 * 
 * 13    98/08/26 19:28 Speter
 * Fixed broken build.
 * 
 * 12    8/26/98 6:18p Asouth
 * #ifdef'd usage of 'that' keyword; also added the keyword 'class' to explicit template
 * instantiation
 * 
 * 11    8/15/98 6:09p Mmouni
 * Added missing template parameters.
 * 
 * 10    8/14/98 8:10p Mmouni
 * Eliminated unreferenced variable.
 * 
 * 9     98/07/03 19:48 Speter
 * Added identity constants.
 * 
 * 8     98/02/10 12:57 Speter
 * Moved generic CPresence3<> * CTransform3<> to .cpp file.
 * 
 * 7     1/20/98 11:15a Mlange
 * Removed compiler warning.
 * 
 * 6     1/12/98 9:56p Gfavor
 * Converted float version of presence * transform operator to 3DX.
 * 
 * 5     97.12.11 7:09p Mmouni
 * Added check for unit quaterion in presence * transform operator.
 * 
 * 4     97.12.09 8:14p Mmouni
 * Added operator* for CPresence3*CTransform3.
 * 
 * 3     97/11/24 16:49 Speter
 * Make use of new 'that' macro for faster returns.
 * 
 * 2     97/11/19 18:09 Speter
 * Moved operator~ to .cpp file.
 * 
 * 1     97/10/27 15:31 Speter
 * First operator to be put in the .cpp file.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Presence.hpp"
#include "AsmSupport.hpp"

//**********************************************************************************************
//
// CPresence3<> implementation.
//

	//******************************************************************************************
	template<class TR> CPresence3<TR> CPresence3<TR>::operator ~() const
	{
		// The inverse of a composite operation S*T is ~T * ~S, which is CPresence3(~S, -T * ~S).
		Assert(rScale != 0);

		// Make use of the 'that' macro to construct the return value directly.
	#ifdef __MWERKS__
		CPresence3<TR> rv;	// Return Value
		rv.r3Rot  = ~r3Rot;
		rv.rScale = 1.0 / rScale;
		rv.v3Pos  = v3Pos * rv.r3Rot * -rv.rScale;
		return rv;
	#else
		that->r3Rot  = ~r3Rot;
		that->rScale = 1.0 / rScale;
		that->v3Pos  = v3Pos * that->r3Rot * -that->rScale;
		return *that;
	#endif
	}

	//******************************************************************************************
	template<class TR> CVector3<TR> CPresence3<TR>::v3Trans(const CVector3<TR>& v3) const
	{
		// Perform the rotation and scale, then add the translation.

		if (!r3Rot.bDoRotation())
		{
			// No rotation needed, just do scale and translation.
			return CVector3<TR>
			(
				v3.tX * rScale + v3Pos.tX,
				v3.tY * rScale + v3Pos.tY,
				v3.tZ * rScale + v3Pos.tZ
			);
		}

		// Duplicate most of v3Rotate() code, and incorporate scale and translation inline.

		TR t_cc = r3Rot.tC     * r3Rot.tC     - 0.5;
		TR t_cx = r3Rot.tC     * r3Rot.v3S.tX;
		TR t_cy = r3Rot.tC     * r3Rot.v3S.tY;
		TR t_cz = r3Rot.tC     * r3Rot.v3S.tZ;
		TR t_xy = r3Rot.v3S.tX * r3Rot.v3S.tY;
		TR t_xz = r3Rot.v3S.tX * r3Rot.v3S.tZ;
		TR t_yz = r3Rot.v3S.tY * r3Rot.v3S.tZ;

		TR t_s = 2 * rScale;
				
		return CVector3<TR>
		(
			t_s * (v3.tX * (t_cc + r3Rot.v3S.tX * r3Rot.v3S.tX) +  
			       v3.tY * (t_xy - t_cz) +
			       v3.tZ * (t_xz + t_cy)) +
			v3Pos.tX,

			t_s * (v3.tX * (t_xy + t_cz) + 
			       v3.tY * (t_cc + r3Rot.v3S.tY * r3Rot.v3S.tY) +
			       v3.tZ * (t_yz - t_cx)) +
			v3Pos.tY,

			t_s * (v3.tX * (t_xz - t_cy) +
			       v3.tY * (t_yz + t_cx) +
			       v3.tZ * (t_cc + r3Rot.v3S.tZ * r3Rot.v3S.tZ)) +
			v3Pos.tZ
		);
	}


	//
	// Generic presence * transform operator.
	//
	template<class TR> CTransform3<TR> operator *(const CPresence3<TR>& pr3, const CTransform3<TR>& tf3)
	{
		Assert(that != &tf3);

		// Convert quaterion to matrix (and scale it).
		float scale = pr3.rScale * 2.0f;
		float t_cc = pr3.r3Rot.tC * pr3.r3Rot.tC - 0.5f;

		float m_xx  = (pr3.r3Rot.v3S.tX * pr3.r3Rot.v3S.tX + t_cc) * scale;
		float m_xy  = (pr3.r3Rot.v3S.tX * pr3.r3Rot.v3S.tY + pr3.r3Rot.tC * pr3.r3Rot.v3S.tZ) * scale;
		float m_xz  = (pr3.r3Rot.v3S.tX * pr3.r3Rot.v3S.tZ - pr3.r3Rot.tC * pr3.r3Rot.v3S.tY) * scale;

		float m_yx  = (pr3.r3Rot.v3S.tX * pr3.r3Rot.v3S.tY - pr3.r3Rot.tC * pr3.r3Rot.v3S.tZ) * scale;
		float m_yy  = (pr3.r3Rot.v3S.tY * pr3.r3Rot.v3S.tY + t_cc) * scale;
		float m_yz  = (pr3.r3Rot.v3S.tY * pr3.r3Rot.v3S.tZ + pr3.r3Rot.tC * pr3.r3Rot.v3S.tX) * scale;

		float m_zx  = (pr3.r3Rot.v3S.tX * pr3.r3Rot.v3S.tZ + pr3.r3Rot.tC * pr3.r3Rot.v3S.tY) * scale;
		float m_zy  = (pr3.r3Rot.v3S.tY * pr3.r3Rot.v3S.tZ - pr3.r3Rot.tC * pr3.r3Rot.v3S.tX) * scale;
		float m_zz  = (pr3.r3Rot.v3S.tZ * pr3.r3Rot.v3S.tZ + t_cc) * scale;

		// Multiply by view matrix.
		that->mx3Mat.v3X.tX = m_xx * tf3.mx3Mat.v3X.tX + m_xy * tf3.mx3Mat.v3Y.tX + m_xz * tf3.mx3Mat.v3Z.tX;
		that->mx3Mat.v3X.tY = m_xx * tf3.mx3Mat.v3X.tY + m_xy * tf3.mx3Mat.v3Y.tY + m_xz * tf3.mx3Mat.v3Z.tY;
		that->mx3Mat.v3X.tZ = m_xx * tf3.mx3Mat.v3X.tZ + m_xy * tf3.mx3Mat.v3Y.tZ + m_xz * tf3.mx3Mat.v3Z.tZ;

		that->mx3Mat.v3Y.tX = m_yx * tf3.mx3Mat.v3X.tX + m_yy * tf3.mx3Mat.v3Y.tX + m_yz * tf3.mx3Mat.v3Z.tX;
		that->mx3Mat.v3Y.tY = m_yx * tf3.mx3Mat.v3X.tY + m_yy * tf3.mx3Mat.v3Y.tY + m_yz * tf3.mx3Mat.v3Z.tY;
		that->mx3Mat.v3Y.tZ = m_yx * tf3.mx3Mat.v3X.tZ + m_yy * tf3.mx3Mat.v3Y.tZ + m_yz * tf3.mx3Mat.v3Z.tZ;

		that->mx3Mat.v3Z.tX = m_zx * tf3.mx3Mat.v3X.tX + m_zy * tf3.mx3Mat.v3Y.tX + m_zz * tf3.mx3Mat.v3Z.tX;
		that->mx3Mat.v3Z.tY = m_zx * tf3.mx3Mat.v3X.tY + m_zy * tf3.mx3Mat.v3Y.tY + m_zz * tf3.mx3Mat.v3Z.tY;
		that->mx3Mat.v3Z.tZ = m_zx * tf3.mx3Mat.v3X.tZ + m_zy * tf3.mx3Mat.v3Y.tZ + m_zz * tf3.mx3Mat.v3Z.tZ;

		// Transform position.
		that->v3Pos.tX = pr3.v3Pos.tX * tf3.mx3Mat.v3X.tX + pr3.v3Pos.tY * tf3.mx3Mat.v3Y.tX +
									pr3.v3Pos.tZ * tf3.mx3Mat.v3Z.tX + tf3.v3Pos.tX;
		that->v3Pos.tY = pr3.v3Pos.tX * tf3.mx3Mat.v3X.tY + pr3.v3Pos.tY * tf3.mx3Mat.v3Y.tY +
									pr3.v3Pos.tZ * tf3.mx3Mat.v3Z.tY + tf3.v3Pos.tY;
		that->v3Pos.tZ = pr3.v3Pos.tX * tf3.mx3Mat.v3X.tZ + pr3.v3Pos.tY * tf3.mx3Mat.v3Y.tZ +
									pr3.v3Pos.tZ * tf3.mx3Mat.v3Z.tZ + tf3.v3Pos.tZ;

		return *that;
	}


//
// Explicit instantiation of used template types.
//

template class CPresence3<>;


//
// Presence * transform operator (float version).
//
CTransform3<> operator *(const CPresence3<>& pr3, const CTransform3<>& tf3)
{
	const float fTwo = 2.0f;
	const float fOneHalf = 0.5f;
	typedef CPlacement3<> Placement3;
	typedef CPresence3<> Presence3;
	typedef CTransform3<> Transform3;
	float m_xx, m_xy, m_xz, m_yx, m_yy, m_yz, m_zx, m_zy, m_zz;

#ifdef __MWERKS__
	CTransform3<> rv;	// Return Value

	if (pr3.r3Rot.tC == 1.0f)
	{
		if (pr3.rScale == 1.0f)
		{
			// No rotation or scale, so just copy matrix.
			rv.mx3Mat = tf3.mx3Mat;
		}
		else
		{
			// No rotation, so just copy and scale matrix.
			rv.mx3Mat.v3X.tX = tf3.mx3Mat.v3X.tX * pr3.rScale;
			rv.mx3Mat.v3X.tY = tf3.mx3Mat.v3X.tY * pr3.rScale;
			rv.mx3Mat.v3X.tZ = tf3.mx3Mat.v3X.tZ * pr3.rScale;
			rv.mx3Mat.v3Y.tX = tf3.mx3Mat.v3Y.tX * pr3.rScale;
			rv.mx3Mat.v3Y.tY = tf3.mx3Mat.v3Y.tY * pr3.rScale;
			rv.mx3Mat.v3Y.tZ = tf3.mx3Mat.v3Y.tZ * pr3.rScale;
			rv.mx3Mat.v3Z.tX = tf3.mx3Mat.v3Z.tX * pr3.rScale;
			rv.mx3Mat.v3Z.tY = tf3.mx3Mat.v3Z.tY * pr3.rScale;
			rv.mx3Mat.v3Z.tZ = tf3.mx3Mat.v3Z.tZ * pr3.rScale;
		}

		// Transform position.
		rv.v3Pos.tX = pr3.v3Pos.tX * tf3.mx3Mat.v3X.tX + pr3.v3Pos.tY * tf3.mx3Mat.v3Y.tX +
									pr3.v3Pos.tZ * tf3.mx3Mat.v3Z.tX + tf3.v3Pos.tX;
		rv.v3Pos.tY = pr3.v3Pos.tX * tf3.mx3Mat.v3X.tY + pr3.v3Pos.tY * tf3.mx3Mat.v3Y.tY +
									pr3.v3Pos.tZ * tf3.mx3Mat.v3Z.tY + tf3.v3Pos.tY;
		rv.v3Pos.tZ = pr3.v3Pos.tX * tf3.mx3Mat.v3X.tZ + pr3.v3Pos.tY * tf3.mx3Mat.v3Y.tZ +
									pr3.v3Pos.tZ * tf3.mx3Mat.v3Z.tZ + tf3.v3Pos.tZ;
	}
#else
	Assert(that != &tf3);

	if (pr3.r3Rot.tC == 1.0f)
	{
		if (pr3.rScale == 1.0f)
		{
			// No rotation or scale, so just copy matrix.
			that->mx3Mat = tf3.mx3Mat;
		}
		else
		{
			// No rotation, so just copy and scale matrix.
			that->mx3Mat.v3X.tX = tf3.mx3Mat.v3X.tX * pr3.rScale;
			that->mx3Mat.v3X.tY = tf3.mx3Mat.v3X.tY * pr3.rScale;
			that->mx3Mat.v3X.tZ = tf3.mx3Mat.v3X.tZ * pr3.rScale;
			that->mx3Mat.v3Y.tX = tf3.mx3Mat.v3Y.tX * pr3.rScale;
			that->mx3Mat.v3Y.tY = tf3.mx3Mat.v3Y.tY * pr3.rScale;
			that->mx3Mat.v3Y.tZ = tf3.mx3Mat.v3Y.tZ * pr3.rScale;
			that->mx3Mat.v3Z.tX = tf3.mx3Mat.v3Z.tX * pr3.rScale;
			that->mx3Mat.v3Z.tY = tf3.mx3Mat.v3Z.tY * pr3.rScale;
			that->mx3Mat.v3Z.tZ = tf3.mx3Mat.v3Z.tZ * pr3.rScale;
		}

		// Transform position.
		that->v3Pos.tX = pr3.v3Pos.tX * tf3.mx3Mat.v3X.tX + pr3.v3Pos.tY * tf3.mx3Mat.v3Y.tX +
									pr3.v3Pos.tZ * tf3.mx3Mat.v3Z.tX + tf3.v3Pos.tX;
		that->v3Pos.tY = pr3.v3Pos.tX * tf3.mx3Mat.v3X.tY + pr3.v3Pos.tY * tf3.mx3Mat.v3Y.tY +
									pr3.v3Pos.tZ * tf3.mx3Mat.v3Z.tY + tf3.v3Pos.tY;
		that->v3Pos.tZ = pr3.v3Pos.tX * tf3.mx3Mat.v3X.tZ + pr3.v3Pos.tY * tf3.mx3Mat.v3Y.tZ +
									pr3.v3Pos.tZ * tf3.mx3Mat.v3Z.tZ + tf3.v3Pos.tZ;
	}
#endif
	else
	{
#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
		float scale;

		__asm 
		{
		mov		eax,[pr3]
		jmp		StartAsm

		align 16
		nop											;establish 2 byte starting code offset
		nop

	StartAsm:
		mov		ebx,[tf3]
#ifdef __MWERKS__
		lea		ecx,[rv]
#else
		mov		ecx,that
#endif

		femms										;ensure fast switch

		// Convert quaternion to matrix.

		// scale = rScale * 2.0f;
		// t_cc = tC * tC - 0.5f;
		// m_xx  = (tX * tX + t_cc) * scale;		// NOTE: scale is calculated and factored 
		// m_xy  = (tX * tY + tC * tZ) * scale;		// in later when m_** are used (i.e. m_** 
		// m_xz  = (tX * tZ - tC * tY) * scale;		// in memory don't include "* scale")

		movd	mm4,[eax]Placement3.r3Rot.tC		;m4= tC
		test	ecx,ecx								;2-byte NOOP to maintain decode pairing 
													;  and avoid degraded predecode
		movd	mm5,[fOneHalf]						;m5= 0.5
		nop											;1-byte NOOP to maintain decode pairing
													;  and avoid degraded predecode
		movd	mm1,[eax]Placement3.r3Rot.v3S.tX	;m1= tX
		movq	mm0,mm4								;m0= tC

		movd	mm2,[eax]Placement3.r3Rot.v3S.tY	;m2= tY
		pfmul	(m4,m4)								;m4= tC*tC

		test	ecx,ecx								;2-byte NOOP to avoid degraded predecode
		movd	mm3,[eax]Placement3.r3Rot.v3S.tZ	;m3= tZ

		// MM0 = tC
		// MM1 = tX
		// MM2 = tY
		// MM3 = tZ
		// MM4 = t_cc  (after following pfsub)

		movq	mm7,mm1								;m7= tX
		pfsub	(m4,m5)								;m4= t_cc

		pfmul	(m7,m3)								;m7= tX*tZ
		movq	mm6,mm0								;m6= tC

		movq	mm5,mm1								;m5= tX
		pfmul	(m6,m2)								;m6= tC*tY

		nop											;1-byte NOOP to avoid degraded predecode
		;											;  and maintain decode pairing
		pfmul	(m5,m2)								;m5= tX*tY

		pfsubr	(m6,m7)								;m6= m_xz
		movq	mm7,mm0								;m7= tC

		test	ecx,ecx								;2-byte NOOP to avoid degraded predecode
		;											;  and maintain decode pairing
		pfmul	(m7,m3)								;m7= tC*tZ

		movd	[m_xz],mm6
		movq	mm6,mm1								;m6= tX

		pfmul	(m6,m6)								;m6= tX*tX
		pfadd	(m5,m7)								;m5= m_xy

		movq	mm7,mm1								;m7= tX
		movd	[m_xy],mm5

		pfadd	(m6,m4)								;m6= m_xx
		movd	[m_xx],mm6

		// m_yx  = (tX * tY - tC * tZ) * scale;		// NOTE: scale is calculated and factored 
		// m_yy  = (tY * tY + t_cc) * scale;		// in later when m_** are used (i.e. m_** 
		// m_yz  = (tY * tZ + tC * tX) * scale;		// in memory don't include "* scale"

		nop											;1-byte NOOP to avoid degraded predecode
		;											;  and maintain decode pairing
		test	ecx,ecx								;2-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		pfmul	(m7,m2)								;m7= tX*tY
		movq	mm6,mm0								;m6= tC

		movq	mm5,mm2								;m5= tY
		pfmul	(m6,m3)								;m6= tC*tZ

		nop											;1-byte NOOP to avoid degraded predecode
		;											;  and maintain decode pairing
		pfmul	(m5,m3)								;m5= tY*tZ

		pfsubr	(m6,m7)								;m6= m_yx
		movq	mm7,mm0								;m7= tC

		movd	[m_yx],mm6
		pfmul	(m7,m1)								;m7= tC*tX

		movq	mm6,mm2								;m6= tY
		test	ecx,ecx								;2-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		pfmul	(m6,m6)								;m6= tY*tY
		pfadd	(m5,m7)								;m5= m_yz

		movq	mm7,mm2								;m7= tY
		movd	[m_yz],mm5

		pfadd	(m6,m4)								;m6= m_yy
		movd	[m_yy],mm6

		// m_zx  = (tX * tZ + tC * tY) * scale;		// NOTE: scale is calculated and factored 
		// m_zy  = (tY * tZ - tC * tX) * scale;		// in later when m_** are used (i.e. m_** 
		// m_zz  = (tZ * tZ + t_cc) * scale;		// in memory don't include "* scale"

		pfmul	(m7,m3)								;m7= tY*tZ
		movq	mm6,mm0								;m6= tC

		movq	mm5,mm1								;m5= tX
		pfmul	(m6,m1)								;m6= tC*tX

		cmp		ecx,0								;3-byte NOOP to avoid degraded predecode
		;											;  and maintain decode pairing
		pfmul	(m5,m3)								;m5= tX*tZ

		pfsubr	(m6,m7)								;m6= m_zy
		movq	mm7,mm0								;m7= tC

		movd	[m_zy],mm6
		pfmul	(m7,m2)								;m7= tC*tY

		movq	mm6,mm3								;m6= tZ
		nop											;1-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		pfmul	(m6,m6)								;m6= tZ*tZ
		pfadd	(m5,m7)								;m5= m_zx

		movd	mm7,[eax]Presence3.rScale			;m7= rScale
		movd	[m_zx],mm5

		pfadd	(m6,m4)								;m6= m_zz
		movd	[m_zz],mm6

		// scale = rScale * 2.0f;

		movd	mm0,[m_xx]							;m0= m_xx
		pfadd	(m7,m7)								;m7= scale

		movd	mm1,[m_xy]							;m1= m_xy
		movd	[scale],mm7

		// MM7 = scale

		// Multiply by view matrix.
		// that->mx3Mat.v3X.tX = m_xx * mx3Mat.v3X.tX + m_xy * mx3Mat.v3Y.tX + m_xz * mx3Mat.v3Z.tX;
		// that->mx3Mat.v3X.tY = m_xx * mx3Mat.v3X.tY + m_xy * mx3Mat.v3Y.tY + m_xz * mx3Mat.v3Z.tY;
		// that->mx3Mat.v3X.tZ = m_xx * mx3Mat.v3X.tZ + m_xy * mx3Mat.v3Y.tZ + m_xz * mx3Mat.v3Z.tZ;

		movd	mm2,[m_xz]							;m2= m_xz
		pfmul	(m0,m7)								;m0= m_xx*scale

		movd	mm3,[ebx]Transform3.mx3Mat.v3X.tX	;m3= vX.tX
		pfmul	(m1,m7)								;m1= m_xy*scale

		nop											;1-byte NOOP to avoid degraded predecode
		;											;  and maintain decode pairing
		nop											;1-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		movd	mm4,[ebx]Transform3.mx3Mat.v3Y.tX	;m4= vY.tX
		pfmul	(m2,m7)								;m2= m_xz*scale

		movd	mm5,[ebx]Transform3.mx3Mat.v3Z.tX	;m5= vZ.tX
		pfmul	(m3,m0)								;m3= m_xx*vX.tX

		movd	mm6,[ebx]Transform3.mx3Mat.v3X.tY	;m6= vX.tY
		pfmul	(m4,m1)								;m4= m_xy*vY.tX

		movd	mm7,[ebx]Transform3.mx3Mat.v3Y.tY	;m7= vY.tY
		pfmul	(m5,m2)								;m5= m_xz*vZ.tX

		pfadd	(m4,m3)								;m4= m_xx*vX.tX + m_xy*vY.tX
		pfmul	(m6,m0)								;m6= m_xx*vX.tY

		movd	mm3,[ebx]Transform3.mx3Mat.v3Z.tY	;m3= vZ.tY
		pfmul	(m7,m1)								;m7= m_xy*vY.tY

		pfadd	(m5,m4)								;m5= that->mat.vX.tX
		movd	[ecx]Transform3.mx3Mat.v3X.tX,mm5

		pfmul	(m3,m2)								;m3= m_xz*vZ.tY
		pfadd	(m7,m6)								;m7= m_xx*vX.tY + m_xy*vY.tY

		movd	mm4,[ebx]Transform3.mx3Mat.v3X.tZ	;m4= vX.tZ
		nop											;1-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		movd	mm5,[ebx]Transform3.mx3Mat.v3Y.tZ	;m5= vY.tZ
		pfadd	(m3,m7)								;m3= that->mat.vX.tY

		movd	mm6,[ebx]Transform3.mx3Mat.v3Z.tZ	;m6= vZ.tZ
		pfmul	(m4,m0)								;m4= m_xx*vX.tZ

		movd	[ecx]Transform3.mx3Mat.v3X.tY,mm3
		pfmul	(m5,m1)								;m5= m_xy*vY.tZ

		movd	mm7,[scale]							;m7= scale
		pfmul	(m6,m2)								;m6= m_xz*vZ.tZ

		movd	mm0,[m_yx]							;m0= m_yx
		pfadd	(m5,m4)								;m5= m_xx*vX.tZ + m_xy*vY.tZ

		movd	mm1,[m_yy]							;m1= m_yy
		cmp		ecx,0								;3-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		pfadd	(m6,m5)								;m6= that->mat.vX.tZ
		movd	[ecx]Transform3.mx3Mat.v3X.tZ,mm6

		// that->mx3Mat.v3Y.tX = m_yx * mx3Mat.v3X.tX + m_yy * mx3Mat.v3Y.tX + m_yz * mx3Mat.v3Z.tX;
		// that->mx3Mat.v3Y.tY = m_yx * mx3Mat.v3X.tY + m_yy * mx3Mat.v3Y.tY + m_yz * mx3Mat.v3Z.tY;
		// that->mx3Mat.v3Y.tZ = m_yx * mx3Mat.v3X.tZ + m_yy * mx3Mat.v3Y.tZ + m_yz * mx3Mat.v3Z.tZ;

		movd	mm2,[m_yz]							;m2= m_yz
		pfmul	(m0,m7)								;m0= m_yx*scale

		movd	mm3,[ebx]Transform3.mx3Mat.v3X.tX	;m3= vX.tX
		pfmul	(m1,m7)								;m1= m_yy*scale

		test	ecx,ecx								;2-byte NOOP to avoid degraded predecode
		;											;  and maintain decode pairing
		cmp		ecx,0								;3-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		movd	mm4,[ebx]Transform3.mx3Mat.v3Y.tX	;m4= vY.tX
		pfmul	(m2,m7)								;m2= m_yz*scale

		movd	mm5,[ebx]Transform3.mx3Mat.v3Z.tX	;m5= vZ.tX
		pfmul	(m3,m0)								;m3= m_yx*vX.tX

		movd	mm6,[ebx]Transform3.mx3Mat.v3X.tY	;m6= vX.tY
		pfmul	(m4,m1)								;m4= m_yy*vY.tX

		movd	mm7,[ebx]Transform3.mx3Mat.v3Y.tY	;m7= vY.tY
		pfmul	(m5,m2)								;m5= m_yz*vZ.tX

		pfadd	(m4,m3)								;m4= m_yx*vX.tX + m_yy*vY.tX
		pfmul	(m6,m0)								;m6= m_yx*vX.tY

		movd	mm3,[ebx]Transform3.mx3Mat.v3Z.tY	;m3= vZ.tY
		pfmul	(m7,m1)								;m7= m_yy*vY.tY

		pfadd	(m5,m4)								;m5= that->mat.vY.tX
		movd	[ecx]Transform3.mx3Mat.v3Y.tX,mm5

		pfmul	(m3,m2)								;m3= m_yz*vZ.tY
		pfadd	(m7,m6)								;m7= m_yx*vX.tY + m_yy*vY.tY

		movd	mm4,[ebx]Transform3.mx3Mat.v3X.tZ	;m4= vX.tZ
		nop											;1-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		movd	mm5,[ebx]Transform3.mx3Mat.v3Y.tZ	;m5= vY.tZ
		pfadd	(m3,m7)								;m3= that->mat.vY.tY

		movd	mm6,[ebx]Transform3.mx3Mat.v3Z.tZ	;m6= vZ.tZ
		pfmul	(m4,m0)								;m4= m_yx*vX.tZ

		movd	[ecx]Transform3.mx3Mat.v3Y.tY,mm3
		pfmul	(m5,m1)								;m5= m_yy*vY.tZ

		movd	mm7,[scale]							;m7= scale
		pfmul	(m6,m2)								;m6= m_yz*vZ.tZ

		movd	mm0,[m_zx]							;m0= m_zx
		pfadd	(m5,m4)								;m5= m_yx*vX.tZ + m_yy*vY.tZ

		movd	mm1,[m_zy]							;m1= m_zy
		cmp		ecx,0								;3-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		pfadd	(m6,m5)								;m6= that->mat.vY.tZ
		movd	[ecx]Transform3.mx3Mat.v3Y.tZ,mm6

		// that->mx3Mat.v3Z.tX = m_zx * mx3Mat.v3X.tX + m_zy * mx3Mat.v3Y.tX + m_zz * mx3Mat.v3Z.tX;
		// that->mx3Mat.v3Z.tY = m_zx * mx3Mat.v3X.tY + m_zy * mx3Mat.v3Y.tY + m_zz * mx3Mat.v3Z.tY;
		// that->mx3Mat.v3Z.tZ = m_zx * mx3Mat.v3X.tZ + m_zy * mx3Mat.v3Y.tZ + m_zz * mx3Mat.v3Z.tZ;

		movd	mm2,[m_zz]							;m2= m_zz
		pfmul	(m0,m7)								;m0= m_zx*scale

		movd	mm3,[ebx]Transform3.mx3Mat.v3X.tX	;m3= vX.tX
		pfmul	(m1,m7)								;m1= m_zy*scale

		test	ecx,ecx								;2-byte NOOP to avoid degraded predecode
		;											;  and maintain decode pairing
		cmp		ecx,0								;3-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		movd	mm4,[ebx]Transform3.mx3Mat.v3Y.tX	;m4= vY.tX
		pfmul	(m2,m7)								;m2= m_zz*scale

		movd	mm5,[ebx]Transform3.mx3Mat.v3Z.tX	;m5= vZ.tX
		pfmul	(m3,m0)								;m3= m_zx*vX.tX

		movd	mm6,[ebx]Transform3.mx3Mat.v3X.tY	;m6= vX.tY
		pfmul	(m4,m1)								;m4= m_zy*vY.tX

		movd	mm7,[ebx]Transform3.mx3Mat.v3Y.tY	;m7= vY.tY
		pfmul	(m5,m2)								;m5= m_zz*vZ.tX

		pfadd	(m4,m3)								;m4= m_zx*vX.tX + m_zy*vY.tX
		pfmul	(m6,m0)								;m6= m_zx*vX.tY

		movd	mm3,[ebx]Transform3.mx3Mat.v3Z.tY	;m3= vZ.tY
		pfmul	(m7,m1)								;m7= m_zy*vY.tY

		pfadd	(m5,m4)								;m5= that->mat.vZ.tX
		movd	[ecx]Transform3.mx3Mat.v3Z.tX,mm5

		pfmul	(m3,m2)								;m3= m_zz*vZ.tY
		pfadd	(m7,m6)								;m7= m_zx*vX.tY + m_zy*vY.tY

		movd	mm4,[ebx]Transform3.mx3Mat.v3X.tZ	;m4= vX.tZ
		nop											;1-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		movd	mm5,[ebx]Transform3.mx3Mat.v3Y.tZ	;m5= vY.tZ
		pfadd	(m3,m7)								;m3= that->mat.vZ.tY

		movd	mm6,[ebx]Transform3.mx3Mat.v3Z.tZ	;m6= vZ.tZ
		pfmul	(m4,m0)								;m4= m_zx*vX.tZ

		movd	[ecx]Transform3.mx3Mat.v3Z.tY,mm3
		pfmul	(m5,m1)								;m5= m_zy*vY.tZ

		movd	mm0,[eax]Placement3.v3Pos.tX		;m0= pX
		pfmul	(m6,m2)								;m6= m_zz*vZ.tZ

		movd	mm1,[eax]Placement3.v3Pos.tY		;m1= pY
		pfadd	(m5,m4)								;m5= m_zx*vX.tZ + m_zy*vY.tZ

		movd	mm2,[eax]Placement3.v3Pos.tZ		;m2= pZ
		punpckldq mm0,mm0							;m0= pX | pX

		pfadd	(m6,m5)								;m6= that->mat.vZ.tZ
		movd	[ecx]Transform3.mx3Mat.v3Z.tZ,mm6

		// Transform position by view transform.
		// that->v3Pos.tX = pr3.Pos.tX * mx3Mat.v3X.tX + pr3.Pos.tY * mx3Mat.v3Y.tX +
		//					pr3.Pos.tZ * mx3Mat.v3Z.tX + v3Pos.tX;
		// that->v3Pos.tY = pr3.Pos.tX * mx3Mat.v3X.tY + pr3.Pos.tY * mx3Mat.v3Y.tY +
		//					pr3.Pos.tZ * mx3Mat.v3Z.tY + v3Pos.tY;
		// that->v3Pos.tZ = pr3.Pos.tX * mx3Mat.v3X.tZ + pr3.Pos.tY * mx3Mat.v3Y.tZ +
		//					pr3.Pos.tZ * mx3Mat.v3Z.tZ + v3Pos.tZ;

		movd	mm3,[ebx]Transform3.mx3Mat.v3X.tZ	;m3= vX.tZ
		nop											;1-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		punpckldq mm1,mm1							;m1= pY | pY
		cmp		ecx,0								;3-byte NOOP to avoid degraded predecode
													;  and maintain decode pairing
		movd	mm4,[ebx]Transform3.mx3Mat.v3Y.tZ	;m4= vY.tZ
		punpckldq mm2,mm2							;m2= pZ | pZ

		movd	mm5,[ebx]Transform3.mx3Mat.v3Z.tZ	;m5= vZ.tZ
		pfmul	(m3,m0)								;m3= vX.tZ*pX

		movd	mm6,[ebx]Transform3.v3Pos.tZ		;m6= vP.tZ
		pfmul	(m4,m1)								;m4= vY.tZ*pY

		movq	mm7,[ebx]Transform3.mx3Mat.v3X.tX	;m7=    vX.tY | vX.tX
		pfmul	(m5,m2)								;m5= vZ.tZ*pZ

		pfadd	(m4,m3)								;m4= vX.tZ*pX + vY.tZ*pY
		movq	mm3,[ebx]Transform3.mx3Mat.v3Y.tX	;m3=    vY.tY | vY.tX

		pfadd	(m6,m5)								;m6= vZ.tZ*pZ + vP.tZ
		pfmul	(m7,m0)								;m7= vX.tY*pX | vX.tX*pX

		movq	mm5,[ebx]Transform3.mx3Mat.v3Z.tX	;m5=    vZ.tY | vZ.tX
		pfmul	(m3,m1)								;m3= vY.tY*pY | vY.tX*pY

		pfadd	(m6,m4)								;m6= that->Pos.tZ
		movq	mm4,[ebx]Transform3.v3Pos.tX		;m4=    vP.tY | vP.tX

		pfmul	(m5,m2)								;m5= vZ.tY*pZ | vZ.tX*pZ
		pfadd	(m7,m3)								;m7= vX.tY*pX + vY.tY*pY | vX.tX*pX + vY.tX*pY

		movd	[ecx]Transform3.v3Pos.tZ,mm6
		pfadd	(m5,m4)								;m5= vZ.tY*pZ + vP.tY    | vZ.tX*pZ + vP.tX


		pfadd	(m7,m5)								;m7= that->Pos.tY | that->Pos.tX
		movq	[ecx]Transform3.v3Pos.tX,mm7

		femms										;empty MMX state and ensure fast switch
		}

#else // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		__asm 
		{
		mov		eax,pr3
		mov		ebx,tf3
#ifdef __MWERKS__
		lea		ecx,[rv]
#else
		mov		ecx,that
#endif
		
		// Convert quaternion to matrix.

		// scale = rScale * 2.0f;
		// t_cc = tC * tC - 0.5f;
		// m_xx  = (tX * tX + t_cc) * scale;
		// m_xy  = (tX * tY + tC * tZ) * scale;
		// m_xz  = (tX * tZ - tC * tY) * scale;
		fld		fTwo
		fmul	[eax]Presence3.rScale				// scale
		fld		[eax]Placement3.r3Rot.tC
		fmul	[eax]Placement3.r3Rot.tC			// cc,scale
		fld		[eax]Placement3.r3Rot.v3S.tX
		fmul	[eax]Placement3.r3Rot.v3S.tX		// xx,cc,scale
		fxch	st(1)								// cc,xx,scale
		fsub	fOneHalf							// cc,xx,scale
		fxch	st(1)								// xx,cc,scale
		fld		[eax]Placement3.r3Rot.v3S.tX
		fmul	[eax]Placement3.r3Rot.v3S.tY		// xy,xx,cc,scale
		fld		[eax]Placement3.r3Rot.tC
		fmul	[eax]Placement3.r3Rot.v3S.tZ		// cz,xy,xx,cc,scale
		fld		[eax]Placement3.r3Rot.v3S.tX
		fmul	[eax]Placement3.r3Rot.v3S.tZ		// xz,cz,xy,xx,cc,scale
		fld		[eax]Placement3.r3Rot.tC
		fmul	[eax]Placement3.r3Rot.v3S.tY		// cy,xz,cz,xy,xx,cc,scale
		fxch	st(4)								// xx,xz,cz,xy,cy,cc,scale
		fadd	st(0),st(5)							// m_xx,xz,cz,xy,cy,cc,scale
		fxch	st(3)								// xy,xz,cz,m_xx,cy,cc,scale
		faddp	st(2),st(0)							// xz,m_xy,m_xx,cy,cc,scale
		fsubrp	st(3),st(0)							// m_xy,m_xx,m_xz,cc,scale
		fxch	st(1)								// m_xx,m_xy,m_xz,cc,scale
		fmul	st(0),st(4) 
		fxch	st(1)								// m_xy,m_xx,m_xz,cc,scale
		fmul	st(0),st(4)
		fxch	st(2)								// m_xz,m_xx,m_xy,cc,scale
		fmul	st(0),st(4)
		fxch	st(1)								// m_xx,m_xz,m_xy,cc,scale
		fstp	m_xx
		fstp	m_xz
		fstp	m_xy

		// m_yx  = (tX * tY - tC * tZ) * scale;
		// m_yy  = (tY * tY + t_cc) * scale;
		// m_yz  = (tY * tZ + tC * tX) * scale;
		fld		[eax]Placement3.r3Rot.v3S.tY
		fmul	[eax]Placement3.r3Rot.v3S.tY		// yy,cc,scale
		fld		[eax]Placement3.r3Rot.v3S.tY
		fmul	[eax]Placement3.r3Rot.v3S.tX		// yx,yy,cc,scale
		fld		[eax]Placement3.r3Rot.tC
		fmul	[eax]Placement3.r3Rot.v3S.tZ		// cz,yx,yy,cc,scale
		fld		[eax]Placement3.r3Rot.v3S.tY
		fmul	[eax]Placement3.r3Rot.v3S.tZ		// yz,cz,yx,yy,cc,scale
		fld		[eax]Placement3.r3Rot.tC
		fmul	[eax]Placement3.r3Rot.v3S.tX		// cx,yz,cz,yx,yy,cc,scale
		fxch	st(4)								// yy,yz,cz,yx,cx,cc,scale
		fadd	st(0),st(5)							// m_yy,yz,cz,yx,cx,cc,scale
		fxch	st(3)								// xy,yz,cz,m_yy,cx,cc,scale
		fsubrp	st(2),st(0)							// yz,m_yx,m_yy,cx,cc,scale
		faddp	st(3),st(0)							// m_yx,m_yy,m_yz,cc,scale
		fxch	st(1)								// m_yy,m_yx,m_yz,cc,scale
		fmul	st(0),st(4)
		fxch	st(1)								// m_yx,m_yy,m_yz,cc,scale
		fmul	st(0),st(4)
		fxch	st(2)								// m_yz,m_yy,m_yx,cc,scale
		fmul	st(0),st(4)
		fxch	st(1)								// m_yy,m_yz,m_yx,cc,scale
		fstp	m_yy
		fstp	m_yz
		fstp	m_yx

		// m_zx  = (tX * tZ + tC * tY) * scale;
		// m_zy  = (tY * tZ - tC * tX) * scale;
		// m_zz  = (tZ * tZ + t_cc) * scale;
		fld		[eax]Placement3.r3Rot.v3S.tX
		fmul	[eax]Placement3.r3Rot.v3S.tZ		// xz,cc,scale
		fld		[eax]Placement3.r3Rot.tC
		fmul	[eax]Placement3.r3Rot.v3S.tY		// cy,xz,cc,scale
		fld		[eax]Placement3.r3Rot.v3S.tY
		fmul	[eax]Placement3.r3Rot.v3S.tZ		// yz,cy,xz,cc,scale
		fld		[eax]Placement3.r3Rot.tC
		fmul	[eax]Placement3.r3Rot.v3S.tX		// cx,yz,cy,xz,cc,scale
		fld		[eax]Placement3.r3Rot.v3S.tZ
		fmul	[eax]Placement3.r3Rot.v3S.tZ		// zz,cx,yz,cy,xz,cc,scale
		fxch	st(4)								// xz,cx,yz,cy,zz,cc,scale
		faddp	st(3),st(0)							// cx,yz,m_zx,zz,cc,scale
		fsubp	st(1),st(0)							// m_yz,m_zx,zz,cc,scale
		fxch	st(2)								// zz,m_zx,m_zy,cc,scale
		faddp	st(3),st(0)							// m_zx,m_zy,m_zz,scale
		fmul	st(0),st(3)							
		fxch	st(1)								// m_zy,m_zx,m_zz,scale	
		fmul	st(0),st(3)
		fxch	st(2)								// m_zz,m_zx,m_zy,scale	
		fmulp	st(3),st(0)							// m_zx,m_zy,m_zz
		fstp	m_zx
		fstp	m_zy
		fstp	m_zz

		// Multiply by view matrix.
		fld		m_xx
		fmul	[ebx]Transform3.mx3Mat.v3X.tX		// A1
		fld		m_xy
		fmul	[ebx]Transform3.mx3Mat.v3Y.tX		// A1 B1
		fld		m_xz
		fmul	[ebx]Transform3.mx3Mat.v3Z.tX		// A1 B1 C1
		fxch	st(2)								// C1 B1 A1
		faddp	st(1),st(0)							// C1 A1+B1
		fld		m_xx										
		fmul	[ebx]Transform3.mx3Mat.v3X.tY		// C1 A1+B1 A2
		fxch	st(2)								// A2 A1+B1 C1
		faddp	st(1),st(0)							// A2 A1+B1+C1
		fld		m_xy
		fmul	[ebx]Transform3.mx3Mat.v3Y.tY		// A2 A1+B1+C1 B2
		fld		m_xz
		fmul	[ebx]Transform3.mx3Mat.v3Z.tY		// A2 A1+B1+C1 B2 C2
		fxch	st(2)								// A2 C2 B2 A1+B1+C1
		fstp	[ecx]Transform3.mx3Mat.v3X.tX		// A2 C2 B2
		faddp	st(1),st(0)							// A2 B2+C2
		fld		m_xx
		fmul	[ebx]Transform3.mx3Mat.v3X.tZ		// A2 B2+C2 A3
		fxch	st(2)								// A3 B2+C2 A2
		faddp	st(1),st(0)							// A3 A2+B2+C2
		fld		m_xy										
		fmul	[ebx]Transform3.mx3Mat.v3Y.tZ		// A3 A2+B2+C2 B3
		fld		m_xz
		fmul	[ebx]Transform3.mx3Mat.v3Z.tZ		// A3 A2+B2+C2 B3 C3
		fxch	st(2)								// A3 C3 B3 A2+B2+C2
		fstp	[ecx]Transform3.mx3Mat.v3X.tY		// A3 C3 B3
		faddp	st(1),st(0)							// A3 B3+C3
		fld		m_yx
		fmul	[ebx]Transform3.mx3Mat.v3X.tX		// A3 B3+C3 A1
		fxch	st(2)								// A1 B3+C3 A3
		faddp	st(1),st(0)							// A1 A3+B3+C3
		fld		m_yy
		fmul	[ebx]Transform3.mx3Mat.v3Y.tX		// A1 A3+B3+C3 B1
		fld		m_yz
		fmul	[ebx]Transform3.mx3Mat.v3Z.tX		// A1 A3+B3+C3 B1 C1
		fxch	st(2)								// A1 C1 B1 A3+B3+C3
		fstp	[ecx]Transform3.mx3Mat.v3X.tZ		// A1 C1 B1
		faddp	st(1),st(0)							// C1 A1+B1
		fld		m_yx										
		fmul	[ebx]Transform3.mx3Mat.v3X.tY		// C1 A1+B1 A2
		fxch	st(2)								// A2 A1+B1 C1
		faddp	st(1),st(0)							// A2 A1+B1+C1
		fld		m_yy
		fmul	[ebx]Transform3.mx3Mat.v3Y.tY		// A2 A1+B1+C1 B2
		fld		m_yz
		fmul	[ebx]Transform3.mx3Mat.v3Z.tY		// A2 A1+B1+C1 B2 C2
		fxch	st(2)								// A2 C2 B2 A1+B1+C1
		fstp	[ecx]Transform3.mx3Mat.v3Y.tX		// A2 C2 B2
		faddp	st(1),st(0)							// A2 B2+C2
		fld		m_yx
		fmul	[ebx]Transform3.mx3Mat.v3X.tZ		// A2 B2+C2 A3
		fxch	st(2)								// A3 B2+C2 A2
		faddp	st(1),st(0)							// A3 A2+B2+C2
		fld		m_yy										
		fmul	[ebx]Transform3.mx3Mat.v3Y.tZ		// A3 A2+B2+C2 B3
		fld		m_yz
		fmul	[ebx]Transform3.mx3Mat.v3Z.tZ		// A3 A2+B2+C2 B3 C3
		fxch	st(2)								// A3 C3 B3 A2+B2+C2
		fstp	[ecx]Transform3.mx3Mat.v3Y.tY		// A3 C3 B3
		faddp	st(1),st(0)							// A3 B3+C3
		fld		m_zx
		fmul	[ebx]Transform3.mx3Mat.v3X.tX		// A3 B3+C3 A1
		fxch	st(2)								// A1 B3+C3 A3
		faddp	st(1),st(0)							// A1 B3+C3+A3
		fld		m_zy
		fmul	[ebx]Transform3.mx3Mat.v3Y.tX		// A1 B3+C3+A3 B1
		fld		m_zz
		fmul	[ebx]Transform3.mx3Mat.v3Z.tX		// A1 B3+C3+A3 B1 C1
		fxch	st(2)								// A1 C1 B1 B3+C3+A3
		fstp	[ecx]Transform3.mx3Mat.v3Y.tZ		// A1 C1 B1
		faddp	st(1),st(0)							// C1 A1+B1
		fld		m_zx										
		fmul	[ebx]Transform3.mx3Mat.v3X.tY		// C1 A1+B1 A2
		fxch	st(2)								// A2 A1+B1 C1
		faddp	st(1),st(0)							// A2 A1+B1+C1
		fld		m_zy
		fmul	[ebx]Transform3.mx3Mat.v3Y.tY		// A2 A1+B1+C1 B2
		fld		m_zz
		fmul	[ebx]Transform3.mx3Mat.v3Z.tY		// A2 A1+B1+C1 B2 C2
		fxch	st(2)								// A2 C2 B2 A1+B1+C1
		fstp	[ecx]Transform3.mx3Mat.v3Z.tX		// A2 C2 B2
		faddp	st(1),st(0)							// A2 B2+C2
		fld		m_zx
		fmul	[ebx]Transform3.mx3Mat.v3X.tZ		// A2 B2+C2 A3
		fxch	st(2)								// A3 B2+C2 A2
		faddp	st(1),st(0)							// A3 A2+B2+C2
		fld		m_zy										
		fmul	[ebx]Transform3.mx3Mat.v3Y.tZ		// A3 A2+B2+C2 B3
		fld		m_zz
		fmul	[ebx]Transform3.mx3Mat.v3Z.tZ		// A3 A2+B2+C2 B3 C3
		fxch	st(2)								// A3 C3 B3 A2+B2+C2
		fstp	[ecx]Transform3.mx3Mat.v3Z.tY		// A3 C3 B3
		faddp	st(1),st(0)							// A3 B3+C3
		fld		[eax]Placement3.v3Pos.tX					
		fmul	[ebx]Transform3.mx3Mat.v3X.tX		// A3 B3+C3 A1
		fxch	st(2)								// A1 B3+C3 A3
		faddp	st(1),st(0)							// A1 B3+C3+A3
		fld		[eax]Placement3.v3Pos.tY
		fmul	[ebx]Transform3.mx3Mat.v3Y.tX		// A1 B3+C3+A3 B1
		fld		[eax]Placement3.v3Pos.tZ
		fmul	[ebx]Transform3.mx3Mat.v3Z.tX		// A1 B3+C3+A3 B1 C1
		fxch	st(2)								// A1 C1 B1 B3+C3+A3
		fstp	[ecx]Transform3.mx3Mat.v3Z.tZ		// A1 C1 B1

		// Transform position by view transform.
		faddp	st(1),st(0)							// A1 C1+B1
		fld		[eax]Placement3.v3Pos.tX
		fmul	[ebx]Transform3.mx3Mat.v3X.tY		// A1 C1+B1 A2
		fxch	st(2)								// A2 C1+B1 A1
		faddp	st(1),st(0)							// A2 C1+B1+A1
		fld		[eax]Placement3.v3Pos.tY
		fmul	[ebx]Transform3.mx3Mat.v3Y.tY		// A2 C1+B1+A1 B2
		fxch	st(1)								// A2 B2 C1+B1+A1
		fadd	[ebx]Transform3.v3Pos.tX			// A2 B2 C1+B1+A1+D1
		fxch	st(1)								// A2 C1+B1+A1+D1 B2 
		fld		[eax]Placement3.v3Pos.tZ
		fmul	[ebx]Transform3.mx3Mat.v3Z.tY		// A2 C1+B1+A1+D1 B2 C2
		fxch	st(3)								// C2 C1+B1+A1+D1 B2 A2
		faddp	st(1),st(0)							// C2 C1+B1+A1+D1 B2+A2
		fxch	st(1)								// C2 B2+A2 C1+B1+A1+D1
		fstp	[ecx]Transform3.v3Pos.tX			// C2 B2+A2
		faddp	st(1),st(0)							// C2+B2+A2
		fld		[eax]Placement3.v3Pos.tX
		fmul	[ebx]Transform3.mx3Mat.v3X.tZ		// C2+B2+A2 A3
		fxch	st(1)								// A3 C2+B2+A2
		fadd	[ebx]Transform3.v3Pos.tY			// A3 C2+B2+A2+D2
		fld		[eax]Placement3.v3Pos.tY			// A3 C2+B2+A2+D2 B3
		fmul	[ebx]Transform3.mx3Mat.v3Y.tZ
		fld		[eax]Placement3.v3Pos.tZ
		fmul	[ebx]Transform3.mx3Mat.v3Z.tZ		// A3 C2+B2+A2+D2 B3 C3
		fxch	st(3)								// C3 C2+B2+A2+D2 B3 A3
		faddp	st(1),st(0)							// C3 C2+B2+A2+D2 B3+A3
		fxch	st(2)								// B3+A3 C2+B2+A2+D2 C3
		// stall(1)
		fadd	[ebx]Transform3.v3Pos.tZ			// B3+A3 C2+B2+A2+D2 C3+D3
		fxch	st(1)								// B3+A3 C3+D3 C2+B2+A2+D2 
		fstp	[ecx]Transform3.v3Pos.tY			// B3+A3 C3+D3
		faddp	st(1),st(0)							// B3+A3+C3+D3
		// stall(3)
		fstp	[ecx]Transform3.v3Pos.tZ
		}

#endif // else(TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

	}

#ifdef __MWERKS__
	return rv;
#else
	return *that;
#endif
}

const CPresence3<> pr3Null;