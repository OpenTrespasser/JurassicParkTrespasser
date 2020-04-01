/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		CAudioADPCM - ADPCM audio loading and decompression class
 *
 *		This ADPCM class is based on the IMA algorithm as defined and put into the public
 *		domain by Intel.
 *		The documentation on this format can be found at www.ima.org
 *
 * Bugs:
 *
 * To do:
 *		8 bit decompression
 *
 * Notes:
 *		The code contained within this file should be 100% thread safe, this is because
 *		streamed audio will be loaded from within a thread.
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/AudioADPCM.cpp                                              $
 * 
 * 3     8/21/98 8:06p Rwyatt
 * 
 * 2     8/20/98 3:05p Rwyatt
 * Decompression support for 16bit ADPCM samples.
 * 
 * 1     8/10/98 5:16p Rwyatt
 * Initial Implementation
 * 
 ***********************************************************************************************/

#include "Audio.hpp"
#include "AudioADPCM.hpp"
#include <algorithm>

//**********************************************************************************************
// channel header
//
#define IMAADPCM_HEADER_LENGTH	4				// In bytes, per channel.


//**********************************************************************************************
static uint32 u4ADPCMDecodeM16
(
    uint8*	pu1_src,
    uint32	u4_src_length,
    uint8*	pu1_dst,
    uint32	u4_block_alignment
);


static uint32 u4ADPCMDecodeS16
(
    uint8*	pu1_src,
    uint32	u4_src_length,
    uint8*	pu1_dst,
    uint32	u4_block_alignment
);



//**********************************************************************************************
CAudioADPCM::CAudioADPCM
(
	SCAUHeader& pcau
)  : CCAULoad(pcau)
//**************************************
{
	u4SourceDataSize	= 0;
	pu1SourceData		= NULL;
}



//**********************************************************************************************
CAudioADPCM::CAudioADPCM
(
	CAudioDatabase*	padat,			// database holding the CAU file or NULL for a Win32 file
	HANDLE			h_file,			// file handle
	uint32			u4_fpos,		// offset of the header in the specified file
	SCAUHeader&		cau				// the header at the above offset
) : CCAULoad(padat, h_file, u4_fpos, cau)
//**************************************
{
	u4SourceDataSize	= 0;
	pu1SourceData		= NULL;

	//
	// Calculate the size of a decompressed block
	//
	if (cauheaderLocal.u1Bits == 8)
	{
		// 8 bit is not supported in Trespasser....
		_asm int 3;

		// MONO:	Block alignment of 1024 = 4 byte header + 1020 adpcm samples = (2040+1) samples = 2041 bytes.
		// STEREO:	Block alignment of 1024 = (4 byte header*2) + 1016 adpcm samples = (2032+2) samples = 2034 bytes.
		u4DecompressedBlockSize = ((cauheaderLocal.u4BlockAlignment - 
			(IMAADPCM_HEADER_LENGTH * cauheaderLocal.u1Channels))*2+cauheaderLocal.u1Channels);
	}
	else
	{
		// MONO:	Block alignment of 1024 = 4 byte header + 1020 adpcm samples = (2040+1) samples = 4082 bytes.
		// STEREO:	Block alignment of 1024 = (4 byte header*2) + 1016 adpcm samples = (2032+2) samples = 4068 bytes.
		u4DecompressedBlockSize = ((cauheaderLocal.u4BlockAlignment - 
			(IMAADPCM_HEADER_LENGTH * cauheaderLocal.u1Channels))*2+cauheaderLocal.u1Channels)*2;
	}

	pu1BlockBuffer = new uint8[u4DecompressedBlockSize+32];		// plus a little for good luck
	MEMLOG_ADD_COUNTER(emlSoundLoader,u4DecompressedBlockSize+32);

	u4FreshBytes = 0;
}



//**********************************************************************************************
CAudioADPCM::~CAudioADPCM()
{
	Assert (pu1BlockBuffer);

	MEMLOG_SUB_COUNTER(emlSoundLoader,u4DecompressedBlockSize+32);
	delete pu1BlockBuffer;
}



//******************************************************************************************
//
//	Decompression definitions, All audio compression classes must implement these functions
//
// Output Mono 8 Bit data, the u4_byte count will be a multiple of 1
//
uint32 CAudioADPCM::u4DecompressMono8bit
(
	uint8* pu1_dst, 
	uint32 u4_byte_count
)
//**************************************
{
	_asm int 3;
	return 0;
}



//******************************************************************************************
// Output Mono 16 Bit data, the u4_byte count will be a multiple of 2
// Returns the number of bytes put into the output buffer
//
uint32 CAudioADPCM::u4DecompressMono16bit
(
	uint8* pu1_dst, 
	uint32 u4_byte_count
)
//**************************************
{
	uint32 u4_count;
	uint32 u4_total = 0;

	while ((u4_byte_count) && (u4SourceDataSize>0))
	{
		if (u4FreshBytes == 0)
		{
			// decompress 1 block into the decomp buffer
			u4FreshBytes = u4ADPCMDecodeM16(pu1SourceData,u4BlockSize,
				pu1BlockBuffer,cauheaderLocal.u4BlockAlignment);

			Assert(u4FreshBytes <= u4DecompressedBlockSize);
			u4SourceDataSize -= cauheaderLocal.u4BlockAlignment;
			pu1SourceData += cauheaderLocal.u4BlockAlignment;

			pu1Next = pu1BlockBuffer;
		}
		
		u4_count = std::min(u4_byte_count,u4FreshBytes);

		memcpy(pu1_dst,pu1Next,u4_count);
		pu1_dst+=u4_count;
		pu1Next+=u4_count;

		u4FreshBytes -= u4_count;
		u4_byte_count-=u4_count;

		u4_total += u4_count;
	}

	return u4_total;
}



//******************************************************************************************
// Output Stereo 8 Bit data, the u4_byte count will be a multiple of 2
// Returns the number of bytes put into the output buffer
//
uint32 CAudioADPCM::u4DecompressStereo8bit
(
	uint8* pu1_dst, 
	uint32 u4_byte_count
)
//**************************************
{
	_asm int 3;
	return 0;
}



//******************************************************************************************
// Output Stereo 16 Bit data, the u4_byte count will be a multiple of 4
// Returns the number of bytes put into the output buffer
//
uint32 CAudioADPCM::u4DecompressStereo16bit
(
	uint8* pu1_dst, 
	uint32 u4_byte_count
)
//**************************************
{
	uint32 u4_count;
	uint32 u4_total = 0;

	while ((u4_byte_count) && (u4SourceDataSize>0))
	{
		if (u4FreshBytes == 0)
		{
			// decompress 1 block into the decomp buffer
			u4FreshBytes = u4ADPCMDecodeS16(pu1SourceData,u4BlockSize,
				pu1BlockBuffer,cauheaderLocal.u4BlockAlignment);

			Assert(u4FreshBytes <= u4DecompressedBlockSize);
			u4SourceDataSize -= cauheaderLocal.u4BlockAlignment;
			pu1SourceData += cauheaderLocal.u4BlockAlignment;

			pu1Next = pu1BlockBuffer;
		}
		
		u4_count = std::min(u4_byte_count,u4FreshBytes);

		memcpy(pu1_dst,pu1Next,u4_count);
		pu1_dst+=u4_count;
		pu1Next+=u4_count;

		u4FreshBytes -= u4_count;
		u4_byte_count-=u4_count;

		u4_total += u4_count;
	}

	return u4_total;
}



//******************************************************************************************
// Set the base address and size of the current block that is avaliable for decompressing
// Returns the number of bytes put into the output buffer
//
void CAudioADPCM::SetSampleSource
(
	uint8* pu1_src, 
	uint32 u4_src_len
)
//**************************************
{
	// Our source len must be a multiple of the compressed block length
	Assert( ((u4_src_len % cauheaderLocal.u4BlockAlignment) == 0) || (u4_src_len <cauheaderLocal.u4BlockAlignment));

	pu1SourceData		= pu1_src;
	u4SourceDataSize	= u4_src_len;

	// Block size is the number of bytes that each decompression step should decode, it
	// is either a multiple of the block length or a number of bytes less than the block
	// length, This is only used for the last block.
	u4BlockSize			= std::min(u4_src_len,cauheaderLocal.u4BlockAlignment);
}


//******************************************************************************************
// get the number of bytes remaining in the current block, or 0 if none.
//
uint32 CAudioADPCM::u4GetRemainingData
(
)
//**************************************
{
	return u4SourceDataSize;
}








//**********************************************************************************************
//
// ADPCM Decode functions.......
//
//**********************************************************************************************
//
// stack offsets for local variables
//
#define asm_u4_block_length		esp+4
#define asm_u4_src_len			esp+8
#define asm_u4_alignment		esp+12



//**********************************************************************************************
//
//  This array is used by imaadpcmNextStepIndex to determine the next step
//  index to use.  The step index is an index to the step[] array, below.
//
static const int32 i4NextStep[256] =
{
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
};



//**********************************************************************************************
//
//  This array contains the array of step sizes used to encode the ADPCM
//  samples.  The step index in each ADPCM block is an index to this array.
//
static const int i4Step[89] =
{
        7,     8,     9,    10,    11,    12,    13,
       14,    16,    17,    19,    21,    23,    25,
       28,    31,    34,    37,    41,    45,    50,
       55,    60,    66,    73,    80,    88,    97,
      107,   118,   130,   143,   157,   173,   190,
      209,   230,   253,   279,   307,   337,   371,
      408,   449,   494,   544,   598,   658,   724,
      796,   876,   963,  1060,  1166,  1282,  1411,
     1552,  1707,  1878,  2066,  2272,  2499,  2749,
     3024,  3327,  3660,  4026,  4428,  4871,  5358,
     5894,  6484,  7132,  7845,  8630,  9493, 10442,
    11487, 12635, 13899, 15289, 16818, 18500, 20350,
    22385, 24623, 27086, 29794, 32767
};



//**********************************************************************************************
// Decoding 16bit is idetical to decoding 8bit except 16bit is signed not unsigned like 8bit and
// of course we write out 16bits per sample and not 8.
//
// Each pass of the loop takes 1 byte (2 sample nibbles) and converts it into 2 16 bit samples.
//
// Block alignment of 1024 = 4 byte header + 1020 adpcm samples = (2040+1) samples = 4082 bytes.
// Compression ratio = 25.08%
//
#pragma warning(disable:4035)
static uint32 u4ADPCMDecodeM16
(
    uint8*	pu1_src,
    uint32	u4_src_length,
    uint8*	pu1_dst,
    uint32	u4_block_alignment
)
{
	_asm
	{
		push	ebp
		sub		esp,32

		//
		// Copy locals to a known stack address, we cannot access the locals vis ESP because we
		// have no idea how many bytes the compiler has put onto the stack. This is why we make
		// a copy
		//
		mov		eax, [u4_src_length]
		mov		[asm_u4_src_len], eax
		mov		eax, [u4_block_alignment]
		mov		[asm_u4_alignment], eax

		mov		edi, [pu1_dst]
		mov		esi, [pu1_src]

NEXT_BLOCK:
		mov		eax, [asm_u4_src_len]
		mov		ebx, [asm_u4_alignment]
		cmp		eax, IMAADPCM_HEADER_LENGTH * 1	//  1 = number of channels.
		jl		FINISH_SAMPLE

		mov		edx,[esi]							// get the header byte

		cmp		ebx,eax
		jl		short MIN
		mov		ebx,eax								// block length
MIN:
		mov		ebp,edx
		sub		eax,ebx								// number of btes remaining...
		shl		edx,16								// sign extend bottom 16bits of edx
		shr		ebp,16
		sub		ebx,IMAADPCM_HEADER_LENGTH * 1		// remove header bytes frm block length
		sar		edx,16								// edx is the block start sample
		and		ebp,0xff							// ebp is the start index
		mov		[asm_u4_block_length],ebx					// number of bytes in a block
		mov		[asm_u4_src_len], eax					// length remaining after this block
		add		esi,4
		mov		WORD PTR[edi],dx

		//**************************************************************************************
		// if the jump below is taken then we have a header with no data....
		// therefore because edi is not going to be used again (the loop will exit)
		// it is safe to adjust it after the compare...
		cmp		DWORD PTR [asm_u4_block_length],0
		jz		NEXT_BLOCK

		add		edi,2
NEXT_SAMPLE:
		xor		ebx,ebx

		//**************************************************************************************
		//
		//		SAMPLE 1		-	Low Nibble
		//
		//**************************************************************************************


		//**************************************************************************************
		//
		//  calculate difference:
		//
		//      lDifference = (nEncodedSample + 1/2) * nStepSize / 4
		//
		mov		eax,DWORD PTR[i4Step + ebp*4]
		mov		bl, [esi]
		mov		ecx,eax
		inc		esi
		shr		eax,3
		test	ebx,4
		jz		short S1_NB4
		add		eax,ecx
S1_NB4:
		shr		ecx,1
		test	ebx,2
		jz		short S1_NB2
		add		eax,ecx
S1_NB2:
		shr		ecx,1
		test	ebx,1
		jz		short S1_NB1
		add		eax,ecx
S1_NB1:
		test	ebx,8
		jz		short S1_NO_SIGN
		neg		eax
S1_NO_SIGN:

		//**************************************************************************************
		//
		//  adjust predicted sample based on calculated difference
		//
		add		edx,eax
		//
		// Clamp to a signed 16bit value
		movsx	eax,dx
		cmp		eax,edx
		je		short S1_DONE
		// we need to clamp....
		cmp		edx,-32768
		jg		short S1_NCL
		mov		edx,-32768
		jmp		short S1_DONE
S1_NCL:
		mov		edx,32768
S1_DONE:
		//		EDX IS THE CONVERTED REAL SAMPLE VALUE

		mov		eax,DWORD PTR[i4NextStep + ebx*4]

		mov		WORD PTR[edi],dx

		// calculate the next step index
		add		ebp, eax			
		jge		short S1_GZ
		xor		ebp,ebp
S1_GZ:
		cmp		ebp,88
		jle		short S1_LE88
		mov		ebp,88
S1_LE88:

		//**************************************************************************************
		//
		//		SAMPLE 2	-	High nibble
		//
		//**************************************************************************************

		//**************************************************************************************
		//
		//  calculate difference:
		//
		shr		ebx,4
		mov		eax,DWORD PTR[i4Step + ebp*4]

		add		edi,2
		mov		ecx,eax
		shr		eax,3
		test	ebx,4
		jz		short S2_NB4
		add		eax,ecx
S2_NB4:
		shr		ecx,1
		test	ebx,2
		jz		short S2_NB2
		add		eax,ecx
S2_NB2:
		shr		ecx,1
		test	ebx,1
		jz		short S2_NB1
		add		eax,ecx
S2_NB1:
		test	ebx,8
		jz		short S2_NO_SIGN
		neg		eax
S2_NO_SIGN:

		//**************************************************************************************
		//
		//  adjust predicted sample based on calculated difference
		//
		add		edx,eax
		//
		// Clamp to a signed 16bit value
		movsx	eax,dx
		cmp		eax,edx
		je		short S2_DONE
		// we need to clamp....

		cmp		edx,-32768
		jg		short S2_NCL
		mov		edx,-32768
		jmp		short S2_DONE
S2_NCL:
		mov		edx,32768
S2_DONE:
		//		EDX IS THE CONVERTED REAL SAMPLE VALUE
		mov		eax,DWORD PTR[i4NextStep + ebx*4]
		mov		[edi],dx

		// calculate the next step index
		add		ebp, eax			
		jge		short S2_GZ
		xor		ebp,ebp
S2_GZ:
		cmp		ebp,88
		jle		short S2_LE88
		mov		ebp,88
S2_LE88:

		add		edi,2

		dec		DWORD PTR [asm_u4_block_length]			// block length in samples NOT bytes
		jnz		NEXT_SAMPLE
		jmp		NEXT_BLOCK

FINISH_SAMPLE:
		add		esp,32
		pop		ebp
		//
		//  We return the number of bytes used in the destination.  This is
		//  simply the difference in bytes from where we started.
		//
		mov		eax,edi
		sub		eax,[pu1_dst]
	}
}
#pragma warning(default:4035)






//**********************************************************************************************
// This routine decodes a single ADPCM sample.  
//  
__inline static int32 i4SampleDecode
(
    int32	i4_encoded_sample,
    int32	i4_predicted_sample,
    int32	i4_step_size
)
{
    int32	i4_difference;
    int32	i4_new_sample;

    //
    //  calculate difference:
    //
    //      lDifference = (nEncodedSample + 1/2) * nStepSize / 4
    //
    i4_difference = i4_step_size>>3;

    if (i4_encoded_sample & 4) 
        i4_difference += i4_step_size;

    if (i4_encoded_sample & 2) 
        i4_difference += i4_step_size>>1;

    if (i4_encoded_sample & 1) 
        i4_difference += i4_step_size>>2;

    //
    //  If the 'sign bit' of the encoded nibble is set, then the
    //  difference is negative...
    //
    if (i4_encoded_sample & 8)
        i4_difference = -i4_difference;

    //
    //  adjust predicted sample based on calculated difference
    //
    i4_new_sample = i4_predicted_sample + i4_difference;

    //
    //  check for overflow and clamp if necessary to a 16 signed sample.
    //  Note that this is optimized for the most common case, when we
    //  don't have to clamp.
    //
    if( (int32)(int16)i4_new_sample == i4_new_sample )
    {
        return (int32)i4_new_sample;
    }

    //
    //  Clamp.
    //
    if( i4_new_sample < -32768 )
        return (int32)-32768;
    else
        return (int32)32767;
}




//**********************************************************************************************
//
static uint32 u4ADPCMDecodeS16
(
    uint8*	pu1_src,
    uint32	u4_src_length,
    uint8*	pu1_dst,
    uint32	u4_block_alignment
)
{
    uint8*	pu1_dst_start;
    int32	i4_block_length;
    int32	i4_step_size;
    uint32	u4_header;
    uint32	u4_left;
    uint32	u4_right;
    int32	i4;

    int32	i4_enc_sampleL;
    int32	i4_pred_sampleL;
    int32	i4_step_indexL;

    int32	i4_enc_sampleR;
    int32	i4_pred_sampleR;
    int32	i4_step_indexR;

    pu1_dst_start = pu1_dst;

    while(u4_src_length!=0)
    {
        i4_block_length  = std::min((uint32)u4_block_alignment,(uint32)u4_src_length);
        u4_src_length   -= i4_block_length;
        i4_block_length -= IMAADPCM_HEADER_LENGTH * 2;

        //
        //  LEFT channel header
        //
        u4_header		= *(uint32*)pu1_src;
        pu1_src			+= sizeof(uint32);
        i4_pred_sampleL	= (int32)(int16)LOWORD(u4_header);
        i4_step_indexL  = (int32)(uint8)HIWORD(u4_header);

        Assert(i4_step_indexL >= 0 && i4_step_indexL <= 88);
        
        //
        //  RIGHT channel header
        //
        u4_header = *(uint32*)pu1_src;
        pu1_src   += sizeof(uint32);
        i4_pred_sampleR = (int32)(int16)LOWORD(u4_header);
        i4_step_indexR  = (int32)(uint8)HIWORD(u4_header);

        Assert(i4_step_indexR >= 0 && i4_step_indexR <= 88);
        
        //
        //  write out first sample
        //
        *(uint32*)pu1_dst = MAKELONG(i4_pred_sampleL, i4_pred_sampleR);
        pu1_dst += sizeof(uint32);

        //
        //  The first DWORD contains 4 left samples, the second DWORD
        //  contains 4 right samples.  We process the source in 8-byte
        //  chunks to make it easy to interleave the output correctly.
        //
		while( (i4_block_length-=8)>=0 )
        {
            //cbBlockLength -= 8;

            u4_left		= *(uint32*)pu1_src;
            pu1_src		+= sizeof(uint32);
            u4_right	= *(uint32*)pu1_src;
            pu1_src		+= sizeof(uint32);

            for( i4=8; i4>0; i4-- )
            {
                //
                //  LEFT channel
                //
                i4_enc_sampleL  = (u4_left & 0x0F);
                i4_step_size    = i4Step[i4_step_indexL];
                i4_pred_sampleL = i4SampleDecode(i4_enc_sampleL, i4_pred_sampleL, i4_step_size);
				i4_step_indexL += i4NextStep[i4_enc_sampleL];
				if (i4_step_indexL < 0)
					i4_step_indexL = 0;
				else if (i4_step_indexL > 88)
					i4_step_indexL = 88;

                //
                //  RIGHT channel
                //
                i4_enc_sampleR	= (u4_right & 0x0F);
                i4_step_size	= i4Step[i4_step_indexR];
                i4_pred_sampleR	= i4SampleDecode(i4_enc_sampleR, i4_pred_sampleR, i4_step_size);
				i4_step_indexR += i4NextStep[i4_enc_sampleR];
				if (i4_step_indexR < 0)
					i4_step_indexR = 0;
				else if (i4_step_indexR > 88)
					i4_step_indexR = 88;

                //
                //  write out sample
                //
                *(uint32*)pu1_dst = MAKELONG(i4_pred_sampleL, i4_pred_sampleR);
                pu1_dst += sizeof(uint32);

                //
                //  Shift the next input sample into the low-order 4 bits.
                //
                u4_left  >>= 4;
                u4_right >>= 4;
            }
        }
    }

    //
    //  We return the number of bytes used in the destination.  This is
    //  simply the difference in bytes from where we started.
    //
    return (uint32)(pu1_dst - pu1_dst_start);
}