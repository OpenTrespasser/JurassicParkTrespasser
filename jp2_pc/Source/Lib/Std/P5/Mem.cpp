/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * P5 implementation of Mem.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/P5/Mem.cpp                                                    $
 * 
 * 17    10/02/98 3:51p Mmouni
 * Changed pu8MemSet64 to do not use FPU for stores under the metrowerks build.
 * 
 * 16    97/06/12 7:15p Pkeet
 * Added an assembly version of MemCopy32.
 * 
 * 15    97/06/12 7:03p Pkeet
 * Added a C++ version of 'MemCopy32.'
 * 
 * 14    97/06/10 15:37 Speter
 * Fixed bug in pu8MemSet64 which would fill with erroneous values on rare occasions.  Now check
 * to make sure FPU can handle value (a bit slower).
 * 
 * 13    5/08/97 11:29a Mlange
 * Fixed bug in pu8MemSet64. Added a few assert checks to this function.
 * 
 * 12    97/03/18 10:33a Pkeet
 * Fixed bug in mem16 routine.
 * 
 * 11    97/02/18 14:44 Speter
 * Created pu8MemSet64, which fills any 64-bit value.  Redid all memset routines so they all
 * call pu8MemSet64.  Got rid of stupid old versions that read before writing.
 * 
 * 10    97/01/27 22:06 Speter
 * Added important check for 0.
 * 
 * 9     97/01/10 17:44 Speter
 * Made memclr call memset8 when unaligned.  Removed test for odd address.
 * 
 * 8     12/20/96 4:27p Cwalla
 * P5 fpuclear
 * 
 * 7     12/13/96 9:15a Cwalla
 * Removed inline, as it didn`t work...
 * 
 * 6     12/13/96 7:04a Cwalla
 * Source Safe is playing up...
 * 
 * 1     12/13/96 7:01a Cwalla
 * 
 * 5     12/05/96 3:03p Cwalla
 * added memclr32 function.
 * 
 * 4     96/10/11 15:34 Speter
 * Fixed bad bugs in new mem routines.
 * 
 * 3     96/10/10 16:48 Speter
 * Sped up functions by testing each word before writing it.
 * 
 * 2     96/05/30 17:49 Speter
 * Sped up inner loops.
 * 
 * 1     96/05/23 16:51 Speter
 * New memset functions.
 * 
 **********************************************************************************************/


// no return value
#pragma warning(disable:4035)


//**********************************************************************************************
//
void MemCopy32
(
	void* pv_dest,		// Destination address.
	void* pv_source,	// Source address.
	int   i_num_bytes	// Number of bytes.
)
//
// Copies the number of bytes from the source address to the destination address.
//
//**************************************
{
	Assert(pv_dest);
	Assert(pv_source);
	Assert((i_num_bytes & 3) == 0);

	uint32* pu4_dest = (uint32*)pv_dest;
	uint32* pu4_source = (uint32*)pv_source;
	int     i_num_dwords = i_num_bytes >> 2;
	--pu4_dest;
	--pu4_source;

	__asm
	{
		mov edi, pu4_dest
		mov esi, pu4_source
		mov edx, i_num_dwords
	BEGIN_LOOP:
		mov eax, [esi + edx*4]
		mov [edi + edx*4], eax
		dec edx
		jnz short BEGIN_LOOP
	}
}


//**********************************************************************************************
//
static inline uint64* pu8MemSet64
(
	uint64* pu8_addr,					// The address to set.
	uint64 u8_val,						// The 64-bit value to set.
	uint u_count						// The number of 64-bit words to set.
)
//
// Returns:
//		pu8_addr + u_count.
//
//**************************************
{
	Assert((uint)pu8_addr % 8 == 0);

#if defined(__MWERKS__)
	if (u_count)
	_asm
	{
		mov		edi,[pu8_addr]
		mov 	eax,dword ptr [u8_val]
		mov 	edx,dword ptr [u8_val+4]
		mov		ecx,[u_count]

		align 16
top32:
		mov		[edi],eax
		mov		[edi+4],edx
		add		edi,8
	  	dec		ecx
		jnz		short top32
		
done:
		mov		[pu8_addr],edi
	}
	
	return pu8_addr;
#else
	uint64 u8_test;

	_asm
	{
		mov	edi, pu8_addr
		mov	ecx, u_count
		jecxz done
		fld	qword ptr [u8_val]

		// Test whether the float store will preserve the value.
		// If not, jump to the 32-bit sequence.
		fst qword ptr [u8_test]
		mov eax, dword ptr [u8_val]
		mov edx, dword ptr [u8_val+4]
		sub eax, dword ptr [u8_test]
		sub edx, dword ptr [u8_test+4]
		or  eax, edx
		jne do32

			align 16
		top:
				fst	qword ptr [edi]
				add	edi,8
	  			sub	ecx,1
				jnz	short top
			fstp qword ptr [u8_val]
			jmp done
	do32:
			fstp qword ptr [u8_test]
			mov eax, dword ptr [u8_val]
			mov edx, dword ptr [u8_val+4]
			align 16
		top32:
				mov [edi], eax
				mov [edi+4], edx
				add	edi,8
	  			sub	ecx,1
				jnz	short top32
	done:
		mov eax, edi				// Set return pointer value.
	}
#endif
}

//
// The following 3 routines invoke pu8MemSet64, but handle two additional aspects:
// They construct the 64-bit value out of repeated smaller values, and they handle the unaligned
// and odd-count words at the ends of the buffer.
//

//**********************************************************************************************
void memset32(uint32* pu4_addr, uint32 u4_val, uint u_count)
{
	Assert((uint)pu4_addr % 4 == 0);

	// Write any unaligned vals.
	if (u_count && uint(pu4_addr) % 8 != 0)
	{
		*pu4_addr++ = u4_val;
		u_count--;
	}

	union
	{
		uint64	u8_val;
		uint32	au4_vals[2];
	};
	au4_vals[0] = au4_vals[1] = u4_val;

	// Write the aligned vals.
	if (u_count >= 2)
		pu4_addr = (uint32*)pu8MemSet64((uint64*)pu4_addr, u8_val, u_count/2);

	// Write any odd vals.
	if (u_count % 2)
	{
		*pu4_addr = u4_val;
	}
}

//**********************************************************************************************
void memset16(uint16* pu2_addr, uint16 u2_val, uint u_count)
{
	Assert((int)pu2_addr % 2 == 0);

	// Write any unaligned vals.
	while (u_count && uint(pu2_addr) % 8 != 0)
	{
		*pu2_addr++ = u2_val;
		u_count--;
	}

	// If the job is done, return.
	if (u_count <= 0)
		return;

	union
	{
		uint64	u8_val;
		uint16	au2_vals[4];
	};
	au2_vals[0] = au2_vals[1] = au2_vals[2] = au2_vals[3] = u2_val;

	// Write the aligned vals.
	if (u_count >= 4)
		pu2_addr = (uint16*)pu8MemSet64((uint64*)pu2_addr, u8_val, u_count/4);

	// Write any odd vals.
	while (u_count % 4)
	{
		*pu2_addr++ = u2_val;
		u_count--;
	}
}

//**********************************************************************************************
void memset8(uint8* pu1_addr, uint8 u1_val, uint u_count)
{
	// Write any unaligned vals.
	while (u_count && uint(pu1_addr) % 8 != 0)
	{
		*pu1_addr++ = u1_val;
		u_count--;
	}

	union
	{
		uint64	u8_val;
		uint8	au1_vals[8];
	};
	au1_vals[0] = au1_vals[1] = au1_vals[2] = au1_vals[3] = 
	au1_vals[4] = au1_vals[5] = au1_vals[6] = au1_vals[7] = u1_val;

	// Write the aligned vals.
	if (u_count >= 8)
		pu1_addr = (uint8*)pu8MemSet64((uint64*)pu1_addr, u8_val, u_count/8);

	// Write any odd vals.
	while (u_count % 8)
	{
		*pu1_addr++ = u1_val;
		u_count--;
	}
}

//**********************************************************************************************
void memclr(void* pu_addr, uint u_count)
{
	memset8((uint8*)pu_addr, 0, u_count);
}

#if VER_DEBUG

#include "Lib/Std/Random.hpp"
#include <memory.h>

//**********************************************************************************************
class CInitMemTest
{
public:
	CInitMemTest()
	{
		union
		{
			uint8  au1_test[256];
			uint16 au2_test[128];
			uint32 au4_test[64];
		} au;
		union
		{
			uint8  u1_val;
			uint16 u2_val;
			uint32 u4_val;
		} u;
		int i;

		CRandom	rand;

		//
		// Validity test.
		//

		u.u4_val = 0x7FF00000;

		memset(au.au1_test, 0, 256);
		memset32(au.au4_test+1, u.u4_val, 64-2);
		for (i = 1; i < 63; i++)
			Assert(au.au4_test[i] == u.u4_val);
		Assert(au.au4_test[0] == 0);
		Assert(au.au4_test[63] == 0);

		u.u4_val = rand();

		memset(au.au1_test, 0, 256);
		memset16(au.au2_test+1, u.u2_val, 128-2);
		for (i = 1; i < 127; i++)
			Assert(au.au2_test[i] == u.u2_val);
		Assert(au.au2_test[0] == 0);
		Assert(au.au2_test[127] == 0);

		u.u4_val = rand();

		memset(au.au1_test, 0, 256);
		memset8(au.au1_test+1, u.u1_val, 256-2);
		for (i = 1; i < 255; i++)
			Assert(au.au1_test[i] == u.u1_val);
		Assert(au.au1_test[0] == 0);
		Assert(au.au1_test[255] == 0);
	}

};

static CInitMemTest InitMemTest;

#endif

