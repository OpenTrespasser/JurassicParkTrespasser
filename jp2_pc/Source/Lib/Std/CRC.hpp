/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	 	CCRC - Generic CRC class, will generate any CRC for which the polynomials are known.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/CRC.hpp                                                      $
 * 
 * 2     3/09/98 10:48p Rwyatt
 * New function to CRC a string without first getting its length
 * 
 * 1     1/26/98 5:23p Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/
#ifndef HEADER_LIB_STD_CRC_HPP
#define HEADER_LIB_STD_CRC_HPP


//**********************************************************************************************
// This controls if we use a logical or table driven implementation.
// Table driven is significantly faster.
//
#define TABLE_DRIVE



//**********************************************************************************************
//
class CCRC
// prefix: crc
//*************************************
{
public:

	//******************************************************************************************
	// Constructor...
	//
	CCRC
	(
		uint32	u4_width,		// width of CRC register, either 16 or 32
		uint32	u4_poly,		// the CRC binary polynomial
		uint32	u4_init,		// initial/reset value of the CRC register
		uint32	u4_xor_out,		// value to XOR the output with
		bool	b_reflect		// reflect/reverse the CRC bit direction
	)
	//*************************************
	{
		Assert ( (u4_width == 16) || (u4_width == 32) );

		u4Width			= u4_width;
		u4Poly			= u4_poly;
		u4Init			= u4_init;
		u4XorOut		= u4_xor_out;
		bReflect		= b_reflect;

		// set the mask to represent u4_width bits..
		u4Mask			= (u4_width==16)?0x0000ffff:0xffffffff;

		Assert( (u4_poly & u4Mask) == u4_poly);
		Assert( (u4_init & u4Mask) == u4_init);
		Assert( (u4_xor_out & u4Mask) == u4_xor_out);

#ifdef TABLE_DRIVE
		//
		// Build the CRC byte table
		//
		for (uint32 u4=0; u4<256; u4++)
		{
			uint32	u4_topbit = 1<<(u4_width-1);
			uint32	u4_inbyte = u4;

			// if we are reflecting the input, switch the input bits around
			if (bReflect) 
			{
				u4_inbyte = u4Reflect(u4_inbyte,8);
			}

			uint32 u4_crc = u4_inbyte << (u4_width-8);
			for (uint32 u4_bit_count=0; u4_bit_count<8; u4_bit_count++)
			{
				if (u4_crc & u4_topbit)
					u4_crc = (u4_crc << 1) ^ u4_poly;
				else
					u4_crc <<= 1;
			}

			// again, if we are reflecting the input we need to switch the whole
			// output crc value around
			if (bReflect) 
			{
				u4_crc = u4Reflect(u4_crc,u4Width);
			}

			u4CrcTable[u4] = u4_crc & u4Mask;
		}
#endif
		// prepare the CRC for use
		Reset();
	}


	//******************************************************************************************
	// Reset the CRC register to its initial state.
	// Call this if the CRC class is to be used again.
	//
	void Reset
	(
	)
	//*************************************
	{
		u4Crc = u4Init;
	}


	//******************************************************************************************
	// CRC the specified block using the CRC register in the current state.
	//
	void CRCBlock
	(
		const void*		pv_block,
		uint32			u4_count
	)
	//*************************************
	{
		while (u4_count)
		{
			*this += ( *((uint8*)pv_block) );
			pv_block = ((uint8*)pv_block) + 1;
			u4_count--;
		}
	}


	//******************************************************************************************
	// CRC the specified string using the CRC register in the current state.
	// This is identical to the above function exception it expects a zero terminated block
	// and therefore does not require the length
	//
	void CRCString
	(
		const char*		str
	)
	//*************************************
	{
		Assert(str);

		while (*str)
		{
			*this += (*str);
			str++;
		}
	}


	//******************************************************************************************
	// return the current CRC register
	//
	uint32 operator()
	(
	)
	//*************************************
	{
#ifdef TABLE_DRIVE
		return (u4XorOut ^ u4Crc) & u4Mask;
#else
		if (bReflect)
			return u4XorOut ^ u4Reflect(u4Crc,u4Width);
		else
			return u4XorOut ^ u4Crc;
#endif
	}


	//******************************************************************************************
	// Update the CRC by 1 character
	//
	void operator+=
	(
		uint8 u1_byte
	)
	//*************************************
	{
#ifdef TABLE_DRIVE
		//**************************************************************************************
		// Table driven version of the CRC algorithm for 16 or 32 bit CRCs
		//
		if (bReflect)
		{
			u4Crc = u4CrcTable[(u4Crc ^ u1_byte) & 0xFF] ^ (u4Crc >> 8);
		}
		else
		{
			u4Crc = u4CrcTable[((u4Crc>>(u4Width-8)) ^ u1_byte) & 0xFF] ^ (u4Crc << 8);
		}
#else
		//**************************************************************************************
		// CRC Implementation through straight logic. If in doubt of the table method then use
		// this method. 
		// This method does a CRC calculation per bit of input data.
		// 
		uint32	u4_input	= (uint32) u1_byte;
		uint32	u4_topbit	= 1 << (u4Width-1);

		if (bReflect) 
		{
			u4_input = u4Reflect(u4_input,8);
		}

		u4Crc ^= (u4_input << (u4Width-8));

		for (uint32 u4=0; u4<8; u4++)
		{
			if (u4Crc & u4_topbit)
			{
				// top bit is set
				u4Crc = (u4Crc << 1) ^ u4Poly;
			}
			else
			{
				// top bit is not set
				u4Crc <<= 1;
			}

			u4Crc &= u4Mask;
		}
#endif
	}


	//******************************************************************************************
	//
	//	Support functions to create well know CRCs
	//
	//******************************************************************************************

	//******************************************************************************************
	// CCITT 32bit CRC is used by PKZIP, AUTODIN II, Ethernet and FDDI. CCITT32 CRC is also
	// known as the CRC32 algorithm.
	// Its output can be verified by the standard CRC test string of '123456789', the output
	// from the CRC should be: 0xCBF43926
	//
	static CCRC* pcrcCreateCCITT32
	(
	)
	//*************************************
	{
		return new CCRC(32,0x04C11DB7,0xffffffff,0xffffffff,true);
	}


	//******************************************************************************************
	// CCITT 16bit (forward) CRC is mainly used within the X25 communication protocol, but is
	// also used by ADCCP and SDLC/HDLC.
	// Its output can be verified by the standard CRC test string of '123456789', the output
	// from the CRC should be: 0x29B1
	//
	static CCRC* pcrcCreateCCITT16
	(
	)
	//*************************************
	{
		return new CCRC(16,0x1021,0xffff,0x0000,false);
	}


	//******************************************************************************************
	// Standard CRC16 algorithm, this is different from the CCITT 16Bit CRC. This is used in the
	// early PKZip files, now the CCITT32 algorithm is used. This is still used within ARC files.
	// Its output can be verified by the standard CRC test string of '123456789', the output
	// from the CRC should be: 0xBB3D
	//
	static CCRC* pcrcCreateCRC16
	(
	)
	//*************************************
	{
		return new CCRC(16,0x8005,0x0000,0x0000,true);
	}


	//******************************************************************************************
	// XModem CRC - unsed only within the XModem file transfer protocol.
	// Its output can be verified by the standard CRC test string of '123456789', the output
	// from the CRC should be: 0x0C73
	//
	static CCRC* pcrcCreateXModem
	(
	)
	//*************************************
	{
		return new CCRC(16,0x8408,0x0000,0x0000,true);
	}


protected:
	//******************************************************************************************
	// Returns the value v with the bottom u4_bits reflected.
	//
	uint32 u4Reflect
	(
		uint32	u4_value,
		uint32	u4_bits
	)
	//*************************************
	{
		uint32	u4_temp = u4_value;

		for (uint32 u4=0; u4 < u4_bits; u4++)
		{
			if (u4_temp & 0x00000001)
			{
				u4_value |=  (1 << ((u4_bits-1)-u4) );
			}
			else
			{
				u4_value &= ~(1 << ((u4_bits-1)-u4) );
			}

			u4_temp >>= 1;
		}

		return u4_value;
	}


	//******************************************************************************************
#ifdef TABLE_DRIVE
	uint32	u4CrcTable[256];	// CRC lookup table
#endif

	uint32  u4Width;			// width in bits [8,32].
	uint32	u4Mask;				// 2^u4Width -1
	uint32	u4Poly;				// the polynomial.
	uint32	u4Init;				// initial CRC register value.
	uint32	bReflect;			// Reflect input bytes?
	uint32	u4XorOut;			// XOR this to output CRC
	
	uint32	u4Crc;				// CRC during execution.
};


#endif
