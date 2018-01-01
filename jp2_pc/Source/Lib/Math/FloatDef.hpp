/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Classes that enable access to the individual bits in the floating point types.
 *		Global functions for fast operations on floating point types.
 *
 * Bugs:
 *
 * To do:
 *		CIntFloat and CIntDouble should really be the same class! The float or double type
 *		should then be specified by a template parameter. Also, the various constants required
 *		by these classes should be encapsulated in a 'trait' class.
 *
 *		Move bPosFloatLessThan() and other assembly functions to a P5/ subdirectory.
 *
 *		Determine if the current implementations of bGetSign() are optimal (e.g. just return:
 *		fFloat < 0 is more optimal?).
 *
 *		Ensure the constructor/assignment operator of CIntFloat and CIntDouble with a floating
 *		point CONSTANT do not go through the floating point pipeline in a release build. If this
 *		is the case, all instances where this is used must be optimised some other way!
 *
 *		Rewrite 'iIntFromPosFloat' and the 'iIntFromFloat' functions in assembly.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Math/FloatDef.hpp                                                 $
 * 
 * 33    8/26/98 5:31p Asouth
 * Converted binary constants to hex constants
 * 
 * 32    8/20/98 5:34p Mmouni
 * iPosFloatCast and iFloatCast now put their return value in a temp before returning it.
 * 
 * 31    98.07.24 1:43p Mmouni
 * Added exception stuff to CFPUState.
 * 
 * 30    6/18/98 5:21p Mlange
 * Removed CIntFloat::iSigned(). It didn't actually work, but luckily wasn't used anyway.
 * 
 * 29    97/10/02 12:08 Speter
 * Added iTrunc and iPosRound functions.  Made iRound work correctly (and slowly).
 * 
 * 28    9/22/97 1:40p Mlange
 * Added CIntFloat::iGetFixedpointLSB().
 * 
 * 27    6/26/97 9:58a Mlange
 * Added CIntFloat::iSigned().
 * 
 * 26    97/06/24 2:00p Pkeet
 * Added the fAbs and the bEqualsZero function.
 * 
 * 25    97/06/23 18:47 Speter
 * Moved iRound here from StdLibEx.hpp.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_MATH_FLOATDEF_HPP
#define HEADER_LIB_MATH_FLOATDEF_HPP



//**********************************************************************************************
//
// Constants for class CIntFloat.
//

// The position of the sign bit.
#define iFLOAT_SIGN_BIT			31

// The position of the first bit of the exponent and the number of bits in the exponent.
#define iFLOAT_EXP_BIT_START	23
#define iFLOAT_EXP_BIT_WIDTH	8

// The position of the first bit of the mantissa and the number of bits in the mantissa (excluding
// the implicit or hidden MSB).
#define iFLOAT_MANT_BIT_START	0
#define iFLOAT_MANT_BIT_WIDTH	23

// Masks to isolate the sign, exponent and mantissa of a floating point number.
#define i4FLOAT_SIGN_BIT_MASK	BitMask((int32)iFLOAT_SIGN_BIT)
#define i4FLOAT_EXP_MASK		BitMaskRange((int32)iFLOAT_EXP_BIT_START,  (int32)iFLOAT_EXP_BIT_WIDTH)
#define i4FLOAT_MANT_MASK		BitMaskRange((int32)iFLOAT_MANT_BIT_START, (int32)iFLOAT_MANT_BIT_WIDTH)

// The exponent bias. Exponents are stored as: e + EXP_BIAS.
#define iFLOAT_EXP_BIAS			127

// Range of the exponent.
#define iFLOAT_EXP_MIN			-126
#define iFLOAT_EXP_MAX			127

// Assembly implementations of CIntFloat::u4CastPos() and CIntFloat::i4Cast().
// These are implemented as macros because otherwise the compiler will *not* inline them
// in many cases.
// On entry, eax should contain the floating point value to convert. On return eax will
// contain the integer result.
#define ASM_POS_FLOAT_CAST							\
	__asm mov ecx, eax								\
													\
	__asm or  eax, 1 << iFLOAT_MANT_BIT_WIDTH		\
	__asm shr ecx, iFLOAT_EXP_BIT_START				\
													\
	__asm neg ecx									\
													\
	__asm shl eax, 32 - (iFLOAT_MANT_BIT_WIDTH + 1)	\
													\
	__asm add ecx, iFLOAT_EXP_BIAS - 1				\
	__asm and eax, ecx								\
													\
	__asm shr eax, cl


#define ASM_FLOAT_CAST			\
	__asm mov ebx, eax			\
	__asm and eax, 0x7fffffff	\
	__asm sar ebx, 31			\
								\
	ASM_POS_FLOAT_CAST			\
								\
	__asm xor eax, ebx			\
	__asm sub eax, ebx			\


//**********************************************************************************************
//
class CIntFloat
//
// Class providing access to the individual bits in a floating point type.
//
// Prefix: if
//
//**************************************
{
public:
	union
	{
		int32 i4Int;
		float fFloat;
	};


	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CIntFloat()
	{
	}

	CIntFloat(float f)
	{
		*this = f;
	}

	CIntFloat(int32 i4)
	{
		*this = i4;
	}
	

	//******************************************************************************************
	//
	// Assignment operators.
	//
	CIntFloat& operator =(float f)
	{
		fFloat = f;
		return *this;
	}

	CIntFloat& operator =(int32 i4)
	{
		i4Int = i4;
		return *this;
	}


	//******************************************************************************************
	//
	// Conversion operators.
	//
	operator float() const
	{
		return fFloat;
	}

	operator int32() const
	{
		return i4Int;
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	bool bSign
	(
	) const
	//
	// Get the sign.
	//
	// Returns:
	//		'true' if this is negative, 'false' if it is positive.
	//
	//**************************************
	{
		return bool((uint32)i4Int >> iFLOAT_SIGN_BIT);
	}


	//******************************************************************************************
	//
	CIntFloat ifAbs
	(
	) const
	//
	// Get the absolute of this.
	//
	// Returns:
	//		The absolute of this int/float value.
	//
	//**************************************
	{
		return *this & (i4FLOAT_EXP_MASK | i4FLOAT_MANT_MASK);
	}


	//******************************************************************************************
	//
	CIntFloat& SetExponent
	(
		int32 i4_exp	// Value for the exponent.
	)
	//
	// Set the exponent.
	//
	// Notes:
	//		The bias is added to the exponent. The mantissa and sign bit are set to zero.
	//
	//**************************************
	{
		// Ensure the exponent is within range.
		Assert(bWithin(i4_exp, iFLOAT_EXP_MIN, iFLOAT_EXP_MAX));

		i4Int = (i4_exp + iFLOAT_EXP_BIAS) << iFLOAT_EXP_BIT_START;

		return *this;
	}


	//******************************************************************************************
	//
	int32 i4GetExponent
	(
	) const
	//
	// Get the exponent.
	//
	// Returns:
	//		The de-normalised exponent of the floating point value.
	//
	//**************************************
	{
		return ((i4Int & i4FLOAT_EXP_MASK) >> iFLOAT_EXP_BIT_START) - iFLOAT_EXP_BIAS;
	}


	//******************************************************************************************
	//
	CIntFloat& SetMantissa
	(
		int32 i4_mant	// Value for the mantissa, excluding the implicit MSB.
	)
	//
	// Set the mantissa.
	//
	// Notes:
	//		The exponent and sign bit are set to zero.
	//
	//**************************************
	{
		// Ensure the mantissa is within range.
		Assert((i4_mant & ~i4FLOAT_MANT_MASK) == 0);

		i4Int = i4_mant;

		return *this;
	}


	//******************************************************************************************
	//
	int32 i4GetMantissa
	(
	) const
	//
	// Get the mantissa.
	//
	// Returns:
	//		The mantissa of the floating point value, EXCLUDING the implicit MSB or sign.
	//
	//**************************************
	{
		return i4Int & i4FLOAT_MANT_MASK;
	}



	//******************************************************************************************
	//
	int iGetFixedpointLSB
	(
	) const
	//
	// Returns:
	//		The LSB of the value contained in this, expressed as a 'normalised' fixed point
	//		representation. In that fixed point representation, the fixed point is at bit 0, the
	//		integer part has positive bit numbers and the fractional part has negative bit
	//		numbers.
	//
	//		Returns zero if the value contained in this is zero also.
	//
	//**************************************
	{
		if (i4Int == 0)
			return 0;

		// Find the LSB that is set in the mantissa.
		int32 i4_mantissa = i4GetMantissa();

		int i_lsb;

		for (i_lsb = -iFLOAT_MANT_BIT_WIDTH; i_lsb != 0; i_lsb++)
		{
			if ((i4_mantissa & 1) != 0)
				break;

			i4_mantissa >>= 1;
		}

		// Find the corresponding LSB in the fixed point representation.
		i_lsb += i4GetExponent();

		return i_lsb;
	}



	//******************************************************************************************
	//
	uint32 u4CastPos
	(
	) const
	//
	// Fast conversion of this *positive* floating point value to an integer value, using
	// truncation.
	//
	// Returns:
	//		The truncated integer representation of the float value. The result of this function
	//		is undefined for floating point numbers outside the numerical range of the return type.
	//
	// Notes:
	//		This floating point value is assumed to be positive by this function. This function
	//		executes faster than the code the compiler generates. Note also, that it is entirely
	//		integer math.
	//
	//**************************************
	{
		// Only works for positive floating point values.
		Assert(!bSign());

		uint32 u4_int_val;

#if VER_ASM
		int i_int_this = i4Int;

		__asm mov eax, i_int_this
		ASM_POS_FLOAT_CAST
		__asm mov u4_int_val, eax

#else
		// Extract the mantissa and add the hidden bit. Then shift the result up to the MSB. We now have
		// the mantissa's value at the most significant bit locations.
		u4_int_val   = i4Int | (1 << iFLOAT_MANT_BIT_WIDTH);
		u4_int_val <<= 32 - (iFLOAT_MANT_BIT_WIDTH + 1);

		// Now extract the exponent. To handle the cases where the floating point number is less than
		// one efficiently, we actually obtain the (denormalised) negated exponent, less one. When the
		// floating point number is less than one, this function must return integer zero. Note that in
		// this case the negated exponent contains all zeros at the bit locations of the (shifted)
		// mantissa obtained above, otherwise it contains all ones.
		int32 i4_neg_exp_less_one = iFLOAT_EXP_BIAS - 1 - (i4Int >> iFLOAT_EXP_BIT_START);

		// Now zero the mantissa in the case of a floating point number less than one.
		u4_int_val &= uint32(i4_neg_exp_less_one);

		// Finally, shift the mantissa's value to the bit location determined by the exponent.
		// Note that on the Intel architecture, the +32 is redundant.
		u4_int_val >>= i4_neg_exp_less_one + 32;

// VER_ASM
#endif

		Assert(fFloat > TypeMax(int) || u4_int_val == uint(fFloat));
		return u4_int_val;
	}


	//******************************************************************************************
	//
	int32 i4Cast
	(
	) const
	//
	// Fast conversion of this floating point value to an integer value, using truncation.
	//
	// Returns:
	//		The truncated integer representation of the float value. The result of this function
	//		is undefined for floating point numbers outside the numerical range of the return type.
	//
	// Notes:
	//		This function executes faster than the code the compiler generates. Note also, that
	//		this function is entirely integer math.
	//
	//**************************************
	{
		int32 i4_int_val;

#if VER_ASM
		int i_int_this = i4Int;

		__asm mov eax, i_int_this
		ASM_FLOAT_CAST
		__asm mov i4_int_val, eax

#else
		// Construct a bitmask, that is either zero if the floating point value is positive or
		// that has all bits set if it is negative.
		uint u4_mask = uint32(i4Int >> (sizeof(int32) * 8 - 1));

		// Take the absolute floating point value and convert it to an integer representation.
		i4_int_val = ifAbs().u4CastPos();

		// First use the mask to obtain a ones-complement integer representation of the floating point value.
		// Then, if the floating point value is negative, add one to convert to a twos-complement representation.
		i4_int_val ^= u4_mask;
		i4_int_val += u4_mask & 1;

// VER_ASM
#endif

		Assert(fFloat > TypeMax(int) || fFloat < TypeMin(int) || i4_int_val == int(fFloat));
		return i4_int_val;
	}
};



//******************************************************************************************
//
inline int iPosFloatCast
(
	float f
)
//
// Convienent global function equivalent of CIntFloat::u4CastPos().
//
// Returns:
//		The truncated integer representation of the float value.
//
//**************************************
{
#if VER_ASM
	int i_result;

	__asm mov eax,[f]
	ASM_POS_FLOAT_CAST
	__asm mov [i_result],eax

	Assert(i_result == CIntFloat(f).u4CastPos());

	return i_result;
#else
	return CIntFloat(f).u4CastPos();
#endif

}


//******************************************************************************************
//
inline int iFloatCast
(
	float f
)
//
// Convienent global function equivalent of CIntFloat::i4Cast().
//
// Returns:
//		The truncated integer (toward zero) representation of the float value.
//
//**************************************
{
#if VER_ASM
	int i_result;

	__asm mov eax,[f]
	ASM_FLOAT_CAST
	__asm mov [i_result],eax

	Assert(i_result == CIntFloat(f).i4Cast());

	return i_result;
#else
	return CIntFloat(f).i4Cast();
#endif
}


//******************************************************************************************
//
inline float fAbs
(
	float f
)
//
// Convienent global function equivalent of CIntFloat::i4Cast().
//
// Returns:
//		The absolute floating point value.
//
//**************************************
{
	return CIntFloat(f).ifAbs();
}



//******************************************************************************************
// 
inline int iTrunc
(
	float f					// Number to truncate.
)
//
// Returns:
//		The next lowest integer.
//
// Notes:
//		iFloatCast is not the same, as it truncates toward zero.
//		For positive numbers, iPosFloatCast is faster.
//
//**************************
{
	// To do: optimise.
	return f >= 0 ? iPosFloatCast(f) : 
		-iPosFloatCast(-f + (1.0f - 1.192092896e-07f));
}


//******************************************************************************************
// 
inline int iPosRound
(
	float f					// Positive number to round.
)
//
// Returns:
//		The nearest integer.
//
//**************************
{
	return iPosFloatCast(f + 0.5f);
}


//******************************************************************************************
// 
inline int iRound
(
	float f					// Number to round.
)
//
// Returns:
//		The nearest integer.
//
//**************************
{
	// To do: optimise.
	return iTrunc(f + 0.5f);
}


//**********************************************************************************************
//
// Constants for class CIntDouble.
//

// The position of the sign bit.
#define iDOUBLE_SIGN_BIT		63

// The position of the first bit of the exponent and the number of bits in the exponent.
#define iDOUBLE_EXP_BIT_START	52
#define iDOUBLE_EXP_BIT_WIDTH	11

// The position of the first bit of the mantissa and the number of bits in the mantissa (excluding
// the implicit or hidden MSB).
#define iDOUBLE_MANT_BIT_START	0
#define iDOUBLE_MANT_BIT_WIDTH	52

// Masks to isolate the sign, exponent and mantissa of a floating point number.
#define i8DOUBLE_SIGN_BIT_MASK	BitMask((int64)iDOUBLE_SIGN_BIT)
#define i8DOUBLE_EXP_MASK		BitMaskRange((int64)iDOUBLE_EXP_BIT_START,  (int64)iDOUBLE_EXP_BIT_WIDTH)
#define i8DOUBLE_MANT_MASK		BitMaskRange((int64)iDOUBLE_MANT_BIT_START, (int64)iDOUBLE_MANT_BIT_WIDTH)


// The exponent bias. Exponents are stored as: e + EXP_BIAS.
#define iDOUBLE_EXP_BIAS		1023

// Range of the exponent.
#define iDOUBLE_EXP_MIN			-1022
#define iDOUBLE_EXP_MAX			1023



//**********************************************************************************************
//
class CIntDouble
//
// Class providing access to the individual bits in a double floating point type.
//
// Prefix: id
//
//**************************************
{
public:
	union
	{
		int64  i8Int;
		double dFloat;
	};


	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CIntDouble()
	{
	}

	CIntDouble(double d)
	{
		*this = d;
	}

	CIntDouble(int64 i8)
	{
		*this = i8;
	}
	

	//******************************************************************************************
	//
	// Assignment operators.
	//
	CIntDouble& operator =(double d)
	{
		dFloat = d;
		return *this;
	}

	CIntDouble& operator =(int64 i8)
	{
		i8Int = i8;
		return *this;
	}


	//******************************************************************************************
	//
	// Conversion operators.
	//
	operator double() const
	{
		return dFloat;
	}

	operator int64() const
	{
		return i8Int;
	}


	//******************************************************************************************
	//
	// Member functions.
	//


	//******************************************************************************************
	//
	bool bSign
	(
	) const
	//
	// Get the sign.
	//
	// Returns:
	//		'true' if this is negative, 'false' if it is positive.
	//
	//**************************************
	{
		return bool((uint64)i8Int >> iDOUBLE_SIGN_BIT);
	}


	//******************************************************************************************
	//
	CIntDouble idAbs
	(
	) const
	//
	// Get the absolute of this.
	//
	// Returns:
	//		The absolute of this int/float value.
	//
	//**************************************
	{
		return *this & (i8DOUBLE_EXP_MASK | i8DOUBLE_MANT_MASK);
	}


	//******************************************************************************************
	//
	CIntDouble& SetExponent
	(
		int64 i8_exp	// Value for the exponent.
	)
	//
	// Set the exponent.
	//
	// Notes:
	//		The bias is added to the exponent. The mantissa and sign bit are set to zero.
	//
	//**************************************
	{
		// Ensure the exponent is within range.
		Assert(bWithin(i8_exp, iDOUBLE_EXP_MIN, iDOUBLE_EXP_MAX));

		i8Int = (i8_exp + iDOUBLE_EXP_BIAS) << iDOUBLE_EXP_BIT_START;

		return *this;
	}


	//******************************************************************************************
	//
	int64 i8GetExponent
	(
	) const
	//
	// Get the exponent.
	//
	// Returns:
	//		The de-normalised exponent of the double floating point value.
	//
	//**************************************
	{
		return ((i8Int & i8DOUBLE_EXP_MASK) >> iDOUBLE_EXP_BIT_START) - iDOUBLE_EXP_BIAS;
	}


	//******************************************************************************************
	//
	CIntDouble& SetMantissa
	(
		int64 i8_mant	// Value for the mantissa, excluding the implicit MSB.
	)
	//
	// Set the mantissa.
	//
	// Notes:
	//		The exponent and sign bit are set to zero.
	//
	//**************************************
	{
		// Ensure the mantissa is within range.
		Assert((i8_mant & ~i8DOUBLE_MANT_MASK) == 0);

		i8Int = i8_mant;

		return *this;
	}


	//******************************************************************************************
	//
	int64 i8GetMantissa
	(
	) const
	//
	// Get the mantissa.
	//
	// Returns:
	//		The mantissa of the floating point value, EXCLUDING the implicit MSB or sign.
	//
	//**************************************
	{
		return i8Int & i8DOUBLE_MANT_MASK;
	}
};



//******************************************************************************************
//
inline bool bPosFloatLessThan
(
	float f_a,
	float f_b
)
//
// Performs a less than comparison for positive floating point values. Note that the
// comparison of floating point values on the stack generates a lot of floating point code,
// and the results of the compare must be moved to the integer registers for processing;
// for this reason this function is much faster.
//
// Returns:
//		The boolean result of the comparison f_a < f_b.
//
//**************************************
{
	int32 i4_retval;

	#if defined(_MSC_VER)
		__asm
		{
			xor  eax, eax

			mov  ebx, f_a
			sub  ebx, f_b
			adc  eax, 0

			mov  i4_retval, eax
		}
	#else
		i4_retval = int32(f_a < f_b);
	#endif

	return bool(i4_retval);
}



//******************************************************************************************
//
inline void PosFloatSetMinMax
(
	float& f,
	float  f_min,
	float  f_max
)
//
// Performs a less than comparison for positive floating point values. Note that the
// comparison of floating point values on the stack generates a lot of floating point code,
// and the results of the compare must be moved to the integer registers for processing;
// for this reason this function is much faster.
//
// Returns:
//		The boolean result of the comparison f_a < f_b.
//
//**************************************
{
	float f_r = f;

	//
	// eax	min
	// ebx	max
	//

	__asm
	{
		push eax
		push ebx

		mov  eax, f_min		// If the float is less than min, set to the min value.
		mov  ebx, f_max

		cmp  eax, [f_r]
		jl short NOT_LESS

		mov  [f_r], eax
		jmp short FINISH_POS_FLOAT_SETMINMAX
NOT_LESS:

		cmp ebx, [f_r]
		jge short FINISH_POS_FLOAT_SETMINMAX

		mov  [f_r], ebx

FINISH_POS_FLOAT_SETMINMAX:
		pop ebx
		pop eax
	}

	f = f_r;
}


//******************************************************************************************
//
inline bool bEqualsZero
(
	float f
)
//
//
// Returns 'true' if the value equals zero.
//
//**************************************
{
	return u4FromFloat(f) == 0;
}


//******************************************************************************************
//
inline int Fist(float f)
//
// Returns:
//		The result of using the 'fist' instruction on 'f.'
//
//**************************************
{
	int i_ret_val;

	#if VER_ASM
		__asm
		{
			fld   [f]
			fistp float ptr[i_ret_val]
		}
	#else // VER_ASM
		i_ret_val = int(f);
	#endif // VER_ASM
	return i_ret_val;
}


//**********************************************************************************************
//
class CFPUState
//
// Object to manipulate the FPU state.
//
// Prefix: fpus
//
//**************************************
{
private:

	bool   bAltered;		// Flag indicating that the FPU state has been modified.
	uint16 u2OldFPUState;	// Storage for the old state of the FPU.

public:

	//******************************************************************************************
	//
	// FPU exception flags.
	//
	enum eFlags	
	{
		fpuINVALID_OPERATION = 0x0001, 
		fpuDENORMALIZED_OPERAND = 0x0002, 
		fpuZERO_DIVIDE = 0x0004, 
		fpuOVERFLOW = 0x0008, 
		fpuUNDERFLOW = 0x0010, 
		fpuPRECISION = 0x0020 
	};


	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	// Constructor.
	CFPUState()
	{
		uint16 u2_fpu_oldstate;

		// Store the fpu state.
		__asm fstcw [u2_fpu_oldstate]
		u2OldFPUState = u2_fpu_oldstate;

		// Indicate the fpu state has not been altered.
		bAltered = false;
	}

	// Destructor.
	~CFPUState()
	{
		Restore();
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SetLowRes
	(
	)
	//
	// Sets the FPU into a low-resolution, fast state.
	//
	//**************************************
	{
		uint16 u2_fpu_currentstate;
		uint16 u2_fpu_newstate;

		__asm 
		{
			fstcw [u2_fpu_currentstate]
			mov   ax, [u2_fpu_currentstate]
			and   eax, NOT 0x300	// 11 0000 0000B
			mov   [u2_fpu_newstate], ax
			fldcw [u2_fpu_newstate]
		}
		bAltered = true;
	}

	//******************************************************************************************
	//
	void SetNormalRes
	(
	)
	//
	// Sets the FPU into a normal resolution (the default state).
	//
	//**************************************
	{
		uint16 u2_fpu_currentstate;
		uint16 u2_fpu_newstate;

		__asm 
		{
			fstcw [u2_fpu_currentstate]
			mov   ax, [u2_fpu_currentstate]
			and   eax, NOT 0x0300	// 11 0000 0000B
			or    eax, 0x0200 		// 10 0000 0000B
			mov   [u2_fpu_newstate], ax
			fldcw [u2_fpu_newstate]
		}
		bAltered = true;
	}

	//******************************************************************************************
	//
	void SetTruncate
	(
	)
	//
	// Sets the FPU into truncation mode.
	//
	//**************************************
	{
		uint16 u2_fpu_currentstate;
		uint16 u2_fpu_newstate;

		__asm 
		{
			fstcw [u2_fpu_currentstate]
			mov   ax, [u2_fpu_currentstate]
			or    eax, 0x0c00	// 1100 0000 0000B
			mov   [u2_fpu_newstate], ax
			fldcw [u2_fpu_newstate]
		}
		bAltered = true;
	}

	//******************************************************************************************
	//
	void EnableExceptions
	(
		uint16 u2_flags
	)
	//
	// Enables the specified FPU exceptions.
	//
	//**************************************
	{
		uint16 u2_old_state;
		uint16 u2_new_state;

		__asm 
		{
			// Clear exceptions.
			fnclex

			// Enable exceptions.
			fstcw	[u2_old_state]
			mov		ax,[u2_old_state]
			mov		bx,[u2_flags]
			not		bx
			and		ax,bx
			mov		[u2_new_state],ax
			fldcw	[u2_new_state]
		}
	}

	//******************************************************************************************
	//
	void Restore
	(
	)
	//
	// Restores the original state of the FPU.
	//
	//**************************************
	{
		if (bAltered)
		{
			uint16 u2_fpu_oldstate = u2OldFPUState;

			__asm fldcw [u2_fpu_oldstate]
			bAltered = false;
		}
	}
};


#endif

