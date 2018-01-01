/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of BitBuffer.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/BitBuffer.cpp                                                 $
 * 
 * 3     8/28/98 11:57a Asouth
 * #ifdef for differences in STL versions
 * 
 * 2     10/24/97 4:28p Mlange
 * Operational.
 * 
 * 1     10/16/97 11:01a Mlange
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "GblInc/Common.hpp"
#include "BitBuffer.hpp"

#include <fstream.h>
#include <iomanip.h>


//**********************************************************************************************
//
// CBitBuffer implementation.
//
	//******************************************************************************************
	CBitBuffer::CBitBuffer(uint u_bit_size)
		: pmbBuffer(new SMemBuffer((u_bit_size + (bit_sizeof(TBufferUnit) - 1)) / bit_sizeof(TBufferUnit))),
		  uCurrIndex_(0)
	{
	}


	//******************************************************************************************
	CBitBuffer::CBitBuffer(const char* str_filename)
		: uCurrIndex_(0)
	{
		ifstream stream_load(str_filename, ios::in | ios::nocreate | ios::binary);

		AlwaysAssert(stream_load.is_open());

		// Determine the size of the file and round this up to the next buffer allocation unit multiple.
		stream_load.seekg(0, ios::end);
	#ifdef __MWERKS__
		int i_file_len = stream_load.tellg().offset();
	#else
		int i_file_len = stream_load.tellg();
	#endif

		uint u_num_buffer_units = uint(i_file_len + sizeof(TBufferUnit) - 1) / sizeof(TBufferUnit);

		pmbBuffer = new SMemBuffer(u_num_buffer_units);

		stream_load.seekg(0, ios::beg);
		stream_load.read(reinterpret_cast<char *>(&pmbBuffer->pabuData[0]), i_file_len);
	}


	//******************************************************************************************
	void CBitBuffer::Read(uint8* pu1_buffer, uint u_byte_count)
	{
		for (uint u_byte = 0; u_byte < u_byte_count; u_byte++)
			*pu1_buffer++ = uint8(uRead(bit_sizeof(uint8)));
	}


	//******************************************************************************************
	void CBitBuffer::Write(uint u_symbol, uint u_bit_count)
	{
		Assert(u_bit_count <= bit_sizeof(uint));

		for (uint u_bit = 0; u_bit < u_bit_count; u_bit++)
		{
			// Isolate bit to write from symbol and move it to the LSB position.
			uint u_sym_bit = (u_symbol >> (u_bit_count - u_bit - 1)) & 1;

			// Convert the buffer bit index into a buffer allocation unit index and a bit position in
			// that allocation unit.
			uint u_bu_index  = uCurrIndex_ / bit_sizeof(TBufferUnit);
			uint u_bit_index = bit_sizeof(TBufferUnit) - 1 - uCurrIndex_ % bit_sizeof(TBufferUnit);

			TBufferUnit bu_stream = pmbBuffer->pabuData[u_bu_index];

			// Write the bit into the stream.
			bu_stream &= ~(TBufferUnit(1) << u_bit_index);
			bu_stream |= TBufferUnit(u_sym_bit) << u_bit_index;

			pmbBuffer->pabuData[u_bu_index] = bu_stream;

			uCurrIndex_++;
		}
	}


	//******************************************************************************************
	void CBitBuffer::Write(const uint8* pu1_data, uint u_byte_count)
	{
		for (uint u_byte = 0; u_byte < u_byte_count; u_byte++)
			Write(*pu1_data++, bit_sizeof(uint8));
	}


	//******************************************************************************************
	void CBitBuffer::Save(const char* str_filename) const
	{
		ofstream stream_save(str_filename, ios::out | ios::trunc | ios::binary);

		AlwaysAssert(stream_save.is_open());

		stream_save.write
		(
			reinterpret_cast<const char *>(&pmbBuffer->pabuData[0]),
			pmbBuffer->pabuData.uLen * sizeof(TBufferUnit)
		);
	}
