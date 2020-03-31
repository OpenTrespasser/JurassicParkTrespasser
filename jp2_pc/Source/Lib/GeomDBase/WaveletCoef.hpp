/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Classes for representing wavelet coeficients.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletCoef.hpp                                         $
 * 
 * 9     98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 8     8/25/98 8:41p Rvande
 * added explicit #include of file that defines relative operators
 * 
 * 7     8/25/98 11:38a Rvande
 * Relative operators are no longer part of the standard
 * 
 * 6     11/25/97 5:36p Mlange
 * Added CCoef::cfUnlift().
 * 
 * 5     11/05/97 5:57p Mlange
 * Added CCoef::cfMinAbs().
 * 
 * 4     10/24/97 4:31p Mlange
 * Added definition for cfMAX. Provided direct access to the actual integer type used to encode
 * the wavelet coeficient.
 * 
 * 3     10/14/97 2:31p Mlange
 * Moved SMapping structure to the new wavelet data format source files. Added overloaded <
 * operator to the CCoef class.
 * 
 * 2     9/19/97 5:27p Mlange
 * Moved SMapping definition to this header file.
 * 
 * 1     9/19/97 3:19p Mlange
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_WAVELETCOEF_HPP
#define HEADER_LIB_GEOMDBASE_WAVELETCOEF_HPP

#include "Lib/Math/FloatDef.hpp"
#include "Lib/Transform/Vector.hpp"

#include <functional>

#ifdef __MWERKS__
 // for <= if only given < and ==
 #include <utility>
 using namespace std::rel_ops;
#endif

namespace NMultiResolution
{
	//**********************************************************************************************
	//
	class CCoef0
	//
	// A dummy class used for concisely representing zero coeficients.
	//
	// Prefix: cf
	//
	//**************************************
	{
	public:
		CCoef0() {};
	};


	//**********************************************************************************************
	//
	class CCoefNAN
	//
	// A dummy class used for concisely representing an illegal coeficient value.
	//
	// Prefix: cf
	//
	//**************************************
	{
	public:
		CCoefNAN() {};
	};


	//**********************************************************************************************
	//
	// Definitions for CCoef.
	//

	#define iMAX_COEF_BITS	(sizeof(int) * 8 - 4)	// Maximum number of significant bits for a coeficient type. This is
													// determined by the size of the type used to encode a coeficient whilst
													// reserving bits for magnitude of intermediate calculations.

	#define cfZERO	NMultiResolution::CCoef(NMultiResolution::CCoef0())
	#define cfNAN	NMultiResolution::CCoef(NMultiResolution::CCoefNAN())
	#define cfMAX	NMultiResolution::CCoef(1 << iMAX_COEF_BITS)


	//**********************************************************************************************
	//
	class CCoef
	//
	// Defines the coeficient type (used to represent wavelet and scaling coeficients) and the
	// integer 2D transform type (e.g. filter) for the wavelet transform.
	//
	// Prefix: cf
	//
	// Notes:
	//		This class implements a 0-disc 2D variation of the 1D 2-2 biorthogonal wavelet transform
	//		of Cohen, Daubechies and Feauveau using the lifting scheme.
	//
	//		This class defines an integer, perfect reconstruction wavelet transform. Conversions to
	//		and from this type require an appropriate (uniform) quantisation scalar.
	//
	//**************************************
	{
		int iCoefVal;

		//******************************************************************************************
		//
		// Constructors and destructor.
		//
	public:
		forceinline CCoef()
		{
			#if VER_DEBUG
				iCoefVal = TypeMax(int);
			#endif
		}

		// Construct and quantise from a floating point value.
		forceinline CCoef(TReal r_val, TReal r_world_to_quantised)
			: iCoefVal(iRound(r_val * r_world_to_quantised))
		{
			Assert(cfAbs() <= cfMAX);
		}


		forceinline CCoef(int i)
			: iCoefVal(i)
		{
		}


		forceinline CCoef(CCoef0)
			: iCoefVal(0)
		{
		}


		forceinline CCoef(CCoefNAN)
			: iCoefVal(TypeMax(int))
		{
		}


	public:
		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		forceinline int iGet() const
		//
		// Returns:
		//		The integer quantised value of this coeficient.
		//
		//**************************************
		{
			return iCoefVal;
		}


		//******************************************************************************************
		//
		forceinline TReal rGet
		(
			TReal r_from_quantised		// Reciprocal of quantisation scalar.
		) const
		//
		// Returns:
		//		The value of this coeficient.
		//
		//**************************************
		{
			return iCoefVal * r_from_quantised;
		}


		//******************************************************************************************
		//
		forceinline TReal rGetSignificance
		(
			TReal r_from_quantised,		// Reciprocal of quantisation scalar.
			int i_support,				// Length of support of this coeficient in quad tree units.
			TReal r_freq_response = 1	// Additional scaling applied to integral.
		) const
		//
		// Returns:
		//		An approximation of the 1D integral of this wavelet coeficient.
		//
		//**************************************
		{
			return iCoefVal * r_from_quantised * i_support * r_freq_response;
		}


		//******************************************************************************************
		//
		forceinline CCoef cfAbs() const
		//
		// Returns:
		//		The absolute of this coeficient.
		//
		//**************************************
		{
			return CCoef(Abs(iCoefVal));
		}


		//******************************************************************************************
		//
		forceinline CCoef cfMaxAbs
		(
			CCoef cf
		) const
		//
		// Returns:
		//		The maximum of the absolutes of this and the given coeficient.
		//
		//**************************************
		{
			return CCoef(Max(Abs(iCoefVal), Abs(cf.iCoefVal)));
		}


		//******************************************************************************************
		//
		forceinline CCoef cfMinAbs
		(
			CCoef cf
		) const
		//
		// Returns:
		//		The minimum of the absolutes of this and the given coeficient.
		//
		//**************************************
		{
			return CCoef(Min(Abs(iCoefVal), Abs(cf.iCoefVal)));
		}


		//******************************************************************************************
		//
		forceinline CCoef cfPredict() const
		//
		// Calculates the 'predict' step of the lifting scheme.
		//
		//**************************************
		{
			return CCoef((iCoefVal + 1) >> 1);
		}


		//******************************************************************************************
		//
		forceinline CCoef cfLift() const
		//
		// Calculates, after all the wavelets have been accumulated in this coeficient, the quantity
		// for the 'update' stage of the lifting scheme.
		//
		//**************************************
		{
			return CCoef((iCoefVal + 4) >> 3);
		}


		//******************************************************************************************
		//
		forceinline CCoef cfUnlift() const
		//
		// The inverse of the 'update' stage of the lifting scheme.
		//
		//**************************************
		{
			return CCoef(iCoefVal << 3);
		}


		//******************************************************************************************
		//
		// Overloaded operators.
		//
		forceinline bool operator ==(CCoef cf) const
		{
			return iCoefVal == cf.iCoefVal;
		}


		forceinline bool operator <(CCoef cf) const
		{
			return iCoefVal < cf.iCoefVal;
		}


		forceinline CCoef operator -() const
		{
			return CCoef(-iCoefVal);
		}


		forceinline CCoef operator +(CCoef cf) const
		{
			return CCoef(iCoefVal + cf.iCoefVal);
		}


		forceinline CCoef operator -(CCoef cf) const
		{
			return CCoef(iCoefVal - cf.iCoefVal);
		}


		forceinline CCoef& operator +=(CCoef cf)
		{
			iCoefVal += cf.iCoefVal;

			return *this;
		}


		forceinline CCoef& operator -=(CCoef cf)
		{
			iCoefVal -= cf.iCoefVal;

			return *this;
		}
	};

};

#endif
