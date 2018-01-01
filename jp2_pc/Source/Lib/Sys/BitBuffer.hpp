/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Classes for managing memory mapped 'stream buffers' addressable at the bit level.
 *
 * Bugs:
 *
 * To do:
 *		Writing to the bit stream can be optimised. It currently writes a single bit at a time,
 *		instead of entire buffer allocation units, like the read functions do.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/BitBuffer.hpp                                                 $
 * 
 * 4     3/16/98 1:03p Pkeet
 * Fixed memory leak.
 * 
 * 3     11/10/97 4:56p Mlange
 * Fixed memory leak in bit buffer destructor. Added CBitBuffer::SeekBeg() and
 * CBitBuffer::uReadBit().
 * 
 * 2     10/24/97 4:28p Mlange
 * Operational.
 * 
 * 1     10/16/97 11:01a Mlange
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_SYS_BITBUFFER_HPP
#define HEADER_LIB_SYS_BITBUFFER_HPP

#include "Lib/Std/Array.hpp"


//**********************************************************************************************
//
// Convienient macro to determine the size of a type in bits.
//
#define bit_sizeof(t)	(sizeof(t) * 8)


//**********************************************************************************************
//
class CBitSignificance
//
// Conversions of integers between a twos-complement and a 'bit-significance' representation.
//
// Prefix: bsig
//
// Notes:
//		Bit-significance is a sign-magnitude representation where the sign bit, rather than
//		proceeding the MSB, is encoded after the LSB.
//
//		In a bit-significance representation, the position of the first non-zero magnitude bit
//		defines the MSB of that number. Thus, bit-significance representations written to an
//		embedded bit-stream can be 'truncated' if the MSB can be determined by other means.
//
//**************************************
{
	uint uVal;

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CBitSignificance()
	{
	}


	CBitSignificance(int i_2c)
	{
		uVal = Abs(i_2c) << 1;

		// Isolate the sign bit, then place it after the LSB.
		uVal |= uint(i_2c) >> (bit_sizeof(int) - 1);
	}


	//******************************************************************************************
	//
	// Conversions.
	//
	operator int() const
	{
		// Obtain the absolute twos-complement.
		int i_2c = uVal >> 1;

		// Create a mask of all ones if the sign bit is set, or all zeros otherwise.
		uint u_sign_mask = (~uVal & 1) - 1;

		// Negate the absolute twos-complement if the sign bit is set.
		i_2c ^= u_sign_mask;
		i_2c -= int(u_sign_mask);

		return i_2c;
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	uint uGet() const
	//
	// Returns:
	//		The bit-significance representation of this, expressed as an unsigned integer. The
	//		LSB denotes the sign, the remaining bits encode the magnitude.
	//
	//**************************************
	{
		return uVal;
	}

	//******************************************************************************************
	//
	void Set
	(
		uint u_val
	)
	//
	// Set the bit-significance representation of this.
	//
	//**************************************
	{
		uVal = u_val;
	}
};



//**********************************************************************************************
//
class CBitBuffer
//
// Manages a memory mapped 'stream buffer' addressable at the bit level.
//
// Prefix: bb
//
// Notes:
//		Copying bit buffers always results in a shallow copy, i.e. the copy references the same
//		memory block as the original. The bit buffer's associated memory block is reference
//		counted.
//
//**************************************
{
	// Prefix: bu
	// The bit buffer allocation unit.
	typedef uint TBufferUnit;

	struct SMemBuffer
	// Prefix: mb
	{
		int iRefCount;					// Current reference count on the buffer.
		CPArray<TBufferUnit> pabuData;	// The buffer to hold the data.

		SMemBuffer(int i_size)
			: pabuData(i_size), iRefCount(1)
		{
		}

		~SMemBuffer()
		{
			delete[] pabuData.atArray;
			pabuData.atArray = 0;
			pabuData.uLen    = 0;
		}
	};


	SMemBuffer* pmbBuffer;	// Memory buffer holding the data.

	uint uCurrIndex_;		// The current bit index into the data buffer.

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Construct buffer with a specified bit size, rounded to the next multiple of the buffer allocation unit.
	CBitBuffer(uint u_bit_size);

	// Copy a bit buffer.
	CBitBuffer(const CBitBuffer& bb)
		: pmbBuffer(bb.pmbBuffer), uCurrIndex_(bb.uCurrIndex_)
	{
		pmbBuffer->iRefCount++;
	}

	// Load from a file.
	CBitBuffer(const char* str_filename);

	// Destructor.
	~CBitBuffer()
	{
		pmbBuffer->iRefCount--;

		if (pmbBuffer->iRefCount == 0)
			delete pmbBuffer;
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	uint uSize() const
	//
	// Returns:
	//		The size of the buffer, in bits.
	//
	//**************************************
	{
		return pmbBuffer->pabuData.uLen * bit_sizeof(TBufferUnit);
	}


	//******************************************************************************************
	//
	uint uCurrIndex() const
	//
	// Returns:
	//		The current bit index (e.g. bit offset from the start of the buffer).
	//
	//**************************************
	{
		return uCurrIndex_;
	}


	//******************************************************************************************
	//
	void Reset()
	//
	// Resets the bit index to the beginning of the buffer.
	//
	//**************************************
	{
		uCurrIndex_ = 0;
	}


	//******************************************************************************************
	//
	void SeekBeg
	(
		int i_offset		// Amount to seek, in bits.
	)
	//
	// Seeks (e.g. moves) the bit index by the specified amount, relative to the start of the
	// buffer.
	//
	//**************************************
	{
		Assert(i_offset >= 0 && i_offset <= uSize());

		uCurrIndex_ = i_offset;
	}


	//******************************************************************************************
	//
	void SeekRel
	(
		int i_count		// Amount to seek, in bits.
	)
	//
	// Seeks (e.g. moves) the bit index by the specified amount, relative to its current
	// position. Negative amounts seek towards the start of the buffer.
	//
	//**************************************
	{
		uCurrIndex_ += i_count;

		Assert(uCurrIndex_ <= uSize());
	}


	//******************************************************************************************
	//
	uint uRemainingBits() const
	//
	// Returns:
	//		The number of unused bits in the buffer.
	//
	//**************************************
	{
		return pmbBuffer->pabuData.uLen * bit_sizeof(TBufferUnit) - uCurrIndex_;
	}


	//******************************************************************************************
	//
	uint uReadBit()
	//
	// Returns:
	//		A single bit from the bit stream at its current position.
	//
	// Notes:
	//		Advances the bit index by 1.
	//
	//**************************************
	{
		// Convert the buffer bit index into a buffer allocation unit index and a bit position in
		// that allocation unit.
		uint u_bu_index  = uCurrIndex_ / bit_sizeof(TBufferUnit);
		uint u_bit_index = bit_sizeof(TBufferUnit) - 1 - uCurrIndex_ % bit_sizeof(TBufferUnit);

		TBufferUnit bu_stream = pmbBuffer->pabuData[u_bu_index];

		bu_stream >>= u_bit_index;
		bu_stream &=  1;

		uCurrIndex_++;

		return bu_stream;
	}


	//******************************************************************************************
	//
	uint uRead
	(
		uint u_bit_count	// The number of bits to read from the stream.
	)
	//
	// Read a symbol from the bit stream at its current position.
	//
	// Returns:
	//		The read symbol expressed as an unsigned integer. Buffer bits are 'streamed' into
	//		the returned symbol at the LSB; unused bits, if any, are zeroed.
	//
	// Notes:
	//		Advances the bit index by the number of bits read from the stream.
	//
	//**************************************
	{
		uint u_ret_val = 0;

		do
		{
			Assert(u_bit_count != 0 && u_bit_count <= bit_sizeof(uint));

			// Convert the buffer bit index into a buffer allocation unit index and a bit position in
			// that allocation unit.
			uint u_bu_index  = uCurrIndex_ / bit_sizeof(TBufferUnit);
			uint u_bit_index = bit_sizeof(TBufferUnit) - 1 - uCurrIndex_ % bit_sizeof(TBufferUnit);

			// Determine the number of bits to read from the current buffer allocation unit. This is
			// the number of bits remaining in the allocation unit, up to the limit of bits left to read.
			uint u_stream_bits = Min(u_bit_count, u_bit_index + 1);

			// Read the buffer allocation unit from the stream, and shift the required bits to the LSB position.
			// Construct a mask to isolate the required bits.
			TBufferUnit bu_stream = pmbBuffer->pabuData[u_bu_index] >> (u_bit_index + 1 - u_stream_bits);
			TBufferUnit bu_mask   = ~TBufferUnit(0) >> TBufferUnit(bit_sizeof(TBufferUnit) - u_stream_bits);

			// Add the new bits to the return type.
			u_ret_val <<= u_stream_bits;
			u_ret_val |=  bu_stream & bu_mask;

			u_bit_count -= u_stream_bits;
			uCurrIndex_ += u_stream_bits;
		}
		while (u_bit_count != 0);

		return u_ret_val;
	}




	//******************************************************************************************
	//
	void Read
	(
		uint8* pu1_buffer,	// Byte buffer to fill.
		uint u_byte_count	// Number of bytes to read.
	);
	//
	// Reads a speficied number of bytes from the stream into the given buffer.
	//
	// Notes:
	//		Advances the bit index by the number of bits read from the stream.
	//
	//**************************************


	//******************************************************************************************
	//
	void Write
	(
		uint u_symbol,		// Symbol to write.
		uint u_bit_count	// The number of bits to write to the stream.
	);
	//
	// Write a symbol to the bit stream at its current position.
	//
	// Notes:
	//		Bits are written from the given symbol in MSB to LSB order, starting at the MSB
	//		position specified by the number of bits to write.
	//
	//		Advances the bit index by the number of bits written to the stream.
	//
	//**************************************


	//******************************************************************************************
	//
	void Write
	(
		const uint8* pu1_data,	// Byte buffer to read from.
		uint u_byte_count		// Number of bytes to write.
	);
	//
	// Writes a specified number of bytes from the given buffer into the stream.
	//
	// Notes:
	//		Advances the bit index by the number of bits written to the stream.
	//
	//**************************************


	//******************************************************************************************
	//
	void Save
	(
		const char* str_filename
	) const;
	//
	// Save the contents of this buffer to a file.
	//
	//**************************************
};



#endif
