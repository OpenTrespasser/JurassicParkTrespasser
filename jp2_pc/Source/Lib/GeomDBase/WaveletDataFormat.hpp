/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Classes describing the format of the binary wavelet transformed data.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletDataFormat.hpp                                   $
 * 
 * 7     3/02/98 7:35p Mlange
 * Wavelet transform data file header now contains # of nodes and # of vertices data fields.
 * Updated version number.
 * 
 * 6     12/18/97 3:04p Mlange
 * Added version number.
 * 
 * 5     11/10/97 4:57p Mlange
 * Many simplifications and optimisations.
 * 
 * 4     10/24/97 4:34p Mlange
 * Implemented compression of wavelet transformed data. Added many comments.
 * 
 * 3     10/16/97 1:44p Mlange
 * Moved implementation of SMapping structure to a separate translation unit to reduce include
 * bloat.
 * 
 * 2     10/16/97 11:02a Mlange
 * Moved CBitBuffer implementation to a separate translation unit.
 * 
 * 1     10/14/97 2:21p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_WAVELETDATAFORMAT_HPP
#define HEADER_LIB_GEOMDBASE_WAVELETDATAFORMAT_HPP

#include "Lib/Sys/BitBuffer.hpp"
#include "Lib/GeomDBase/WaveletCoef.hpp"
#include "Lib/GeomDBase/WaveletConv.hpp"



namespace NMultiResolution
{
	#define u2TRANSFORM_DATA_VERSION	uint16(1001)

	#define iMAX_TRANSFORM_LEVELS	15		// Defines the maximum number of subdivision levels in the wavelet data.
	#define iMAX_TRANSFORM_INDICES	(1 << iMAX_TRANSFORM_LEVELS)

	#define iMAX_ENCODE_SIG_BITS	30		// Maximum number of bits allowed to encode a wavelet transform datum.


	class CQuadRootTForm;

	//**********************************************************************************************
	//
	class CTransformedDataHeader
	//
	// Wavelet transformed data stream header.
	//
	// Prefix: tdh
	//
	//**************************************
	{
		CBitBuffer bbData;

	public:
		uint16   u2Version;			// Version number of terrain data.

		int iNumQuadNodes;			// Number of wavelet quad tree nodes
		int iNumQuadVertices;		// Number of wavelet quad tree vertices

		CCoef    cfRoot;			// Root scaling vertex of the wavelet transformed data.
		SMapping mpConversions;		// World <-> quad space conversions.


		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		// Load from a file.
		CTransformedDataHeader(const char* str_filename);

		// Construct from the wavelet transform data quad tree.
		CTransformedDataHeader(const CQuadRootTForm& qntr);


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		uint uDataBitSize() const
		//
		// Returns:
		//		The number of bits used to hold all the wavelet transformed data, including the header.
		//
		//**************************************
		{
			return bbData.uSize();
		}


		//******************************************************************************************
		//
		const CBitBuffer& bbGetData() const
		//
		// Returns:
		//		The bit buffer that holds the wavelet transformed data. The bit buffer current index
		//		references the start of the wavelet data for the root node.
		//
		//**************************************
		{
			return bbData;
		}


		//******************************************************************************************
		//
		void Save
		(
			const char* str_filename
		) const;
		//
		// Save the wavelet transformed data to disk.
		//
		//**************************************

	private:
		//******************************************************************************************
		//
		void Read();
		//
		// Initialise this header with data read from the bit buffer stream.
		//
		// Notes:
		//		Must be called by all constructors.
		//
		//**************************************
	};




	class CQuadNodeTForm;

	//**********************************************************************************************
	//
	class CTransformedData
	//
	// The wavelet transformed data for a single quad node.
	//
	// Prefix: td
	//
	//**************************************
	{
		uint uDscBitBufferIndex : 27;	// The index into the wavelet transformed data stream for the data of the
										// first descendant.

		uint uBranchDataSizeSigBits : 5;// The number of significant bits required to encode the value of the wavelet
										// transformed data bit size associated with this branch of the quad tree.

		CCoef cfMaxRegion;				// Absolute max wavelet coeficient value of this node and all its descendants.
										// Zero if this node has no wavelet coeficients (e.g. is a leaf node).

		//******************************************************************************************
		//
		// Constructors and destructor.
		//

	public:
		CTransformedData()
		{
		}


		// Initialise for the root node.
		CTransformedData(const CBitBuffer& bb_data);


		// Construct a stream for a descendant.
		CTransformedData(const CTransformedData& td_parent, int i_dsc, const CBitBuffer& bb_data);


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		CCoef cfGetMaxRegion() const
		//
		// Returns:
		//		Absolute max wavelet coeficient value of this node and all its descendants or cfZERO
		//		if this node has no wavelet coeficients (e.g. is a leaf node).
		//
		//**************************************
		{
			return cfMaxRegion;
		}


		//******************************************************************************************
		//
		void GetWavelets
		(
			CCoef acf_wvlts[3],
			const CBitBuffer& bb_data
		) const;
		//
		// Initialises the given array with the wavelet coeficients of this node, or all cfZERO
		// if this node has no wavelets.
		//
		//**************************************


		//******************************************************************************************
		//
		static uint uEncodedBitSize
		(
			const CQuadNodeTForm& qnt
		);
		//
		// Returns:
		//		The number of bits required to hold the wavelet transformed data for the given
		//		quad node.
		//
		//**************************************


		//******************************************************************************************
		//
		static void WriteBranch
		(
			const CQuadNodeTForm& qnt,
			CBitBuffer& rbb_write
		);
		//
		// Write the encoded wavelet transformed data associated with the given branch of the quad
		// tree to the bit buffer.
		//
		//**************************************

	private:
		//******************************************************************************************
		//
		void Read
		(
			CBitBuffer& rbb_data,
			const CTransformedData* ptd_parent = 0
		);
		//
		// Initialise this with data read from the bit buffer stream.
		//
		//**************************************
	};
};

#endif
