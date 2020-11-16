/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of WaveletDataFormat.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletDataFormat.cpp                                   $
 * 
 * 9     9/23/98 7:41p Mlange
 * Wavelet quad tree nodes now have a single pointer to the first descendant and a pointer to
 * the next sibling, instead of four pointers for each of the descendants.
 * 
 * 8     3/02/98 7:34p Mlange
 * Wavelet transform data file header now contains # of nodes and # of vertices data fields.
 * Updated version number.
 * 
 * 7     12/18/97 3:03p Mlange
 * Added version number.
 * 
 * 6     12/16/97 5:24p Mlange
 * Updated for changes to the wavelet quad tree vertex class interface.
 * 
 * 5     11/10/97 4:57p Mlange
 * Many simplifications and optimisations.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "WaveletDataFormat.hpp"

#include "Lib/GeomDBase/WaveletQuadTreeTForm.hpp"


namespace NMultiResolution
{

//**********************************************************************************************
//
// NMultiResolution::CTransformedDataHeader implementation.
//

	//******************************************************************************************
	CTransformedDataHeader::CTransformedDataHeader(const char* str_filename)
		: bbData(str_filename)
	{
		Read();
	}


	//******************************************************************************************
	CTransformedDataHeader::CTransformedDataHeader(const CQuadRootTForm& qntr)
		: bbData
		  (
			bit_sizeof(u2TRANSFORM_DATA_VERSION) +
			bit_sizeof(int) +
			bit_sizeof(int) +
			bit_sizeof(int) +
			bit_sizeof(SMapping) +
			qntr.uGetBranchDataBitSize()
		  )
	{
		Assert(qntr.bHasDescendants());

		// Write header.
		bbData.Write(u2TRANSFORM_DATA_VERSION, bit_sizeof(u2TRANSFORM_DATA_VERSION));
		bbData.Write(qntr.iGetNumSignificantNodes(), bit_sizeof(int));
		bbData.Write(qntr.iGetNumVertices(), bit_sizeof(int));
		bbData.Write(qntr.pqvtGetVertex(0)->cfScaling().iGet(), bit_sizeof(int));
		bbData.Write(reinterpret_cast<const uint8*>(&qntr.mpConversions), sizeof(SMapping));

		CTransformedData::WriteBranch(qntr, bbData);

		Read();
	}


	//******************************************************************************************
	void CTransformedDataHeader::Save(const char* str_filename) const
	{
		bbData.Save(str_filename);
	}


	//******************************************************************************************
	void CTransformedDataHeader::Read()
	{
		bbData.Reset();

		// Read header.
		u2Version = bbData.uRead(bit_sizeof(u2TRANSFORM_DATA_VERSION));

		iNumQuadNodes    = bbData.uRead(bit_sizeof(int));
		iNumQuadVertices = bbData.uRead(bit_sizeof(int));

		cfRoot = bbData.uRead(bit_sizeof(int));
		bbData.Read(reinterpret_cast<uint8*>(&mpConversions), sizeof(SMapping));

		// Make sure the terrain data file has the correct version number.
		AlwaysAssert(u2Version == u2TRANSFORM_DATA_VERSION);
	}



//**********************************************************************************************
//
// NMultiResolution::CTransformedData implementation.
//

	//******************************************************************************************
	CTransformedData::CTransformedData(const CBitBuffer& bb_data)
	{
		CBitBuffer bb_temp(bb_data);

		Read(bb_temp);
	}


	//******************************************************************************************
	CTransformedData::CTransformedData(const CTransformedData& td_parent, int i_dsc, const CBitBuffer& bb_data)
	{
		CBitBuffer bb_temp(bb_data);

		bb_temp.SeekBeg(td_parent.uDscBitBufferIndex);

		// Determine if the parent has data for this descendant.
		if (td_parent.cfMaxRegion != cfZERO)
		{
			// Seek to the descendant's location in the data stream.
			for (int i = 0; i < i_dsc; i++)
			{
				uint u_old_index = bb_temp.uCurrIndex();

				if (bb_temp.uReadBit() != 0)
				{
					uint u_branch_data_size = bb_temp.uRead(td_parent.uBranchDataSizeSigBits);

					// Determine how many bits of this node's wavelet data we have read.
					uint u_num_bits_read = bb_temp.uCurrIndex() - u_old_index;

					bb_temp.SeekRel(u_branch_data_size - u_num_bits_read);
				}
			}
		}

		Read(bb_temp, &td_parent);
	}


	//******************************************************************************************
	void CTransformedData::GetWavelets(CCoef acf_wvlts[3], const CBitBuffer& bb_data) const
	{
		if (cfMaxRegion != cfZERO)
		{
			CBitBuffer bb_temp(bb_data);

			// Seek back to the wavelet coeficients.
			uint u_num_wvlt_bits = uLog2(cfMaxRegion.iGet()) + 2;

			bb_temp.SeekBeg(uDscBitBufferIndex);
			bb_temp.SeekRel(-int(u_num_wvlt_bits * 3));

			CBitSignificance bsig_wvlt;

			bsig_wvlt.Set(bb_temp.uRead(u_num_wvlt_bits));
			acf_wvlts[0] = int(bsig_wvlt);

			bsig_wvlt.Set(bb_temp.uRead(u_num_wvlt_bits));
			acf_wvlts[1] = int(bsig_wvlt);

			bsig_wvlt.Set(bb_temp.uRead(u_num_wvlt_bits));
			acf_wvlts[2] = int(bsig_wvlt);
		}
		else
		{
			acf_wvlts[0] = cfZERO;
			acf_wvlts[1] = cfZERO;
			acf_wvlts[2] = cfZERO;
		}
	}


	//******************************************************************************************
	uint CTransformedData::uEncodedBitSize(const CQuadNodeTForm& qnt)
	{
		uint u_bit_size = 1;

		if (qnt.stGetState()[CQuadNodeTForm::estSIGNIFICANT])
		{
			if (qnt.ptqnGetParent() != 0)
			{
				u_bit_size += (uLog2(qnt.ptqnGetParent()->uGetBranchDataBitSize()) + 1);
				u_bit_size += (uLog2(qnt.ptqnGetParent()->cfGetMaxRegion().iGet()) + 1);
			}
			else
				u_bit_size += iMAX_ENCODE_SIG_BITS * 2;

			u_bit_size += (uLog2(qnt.cfGetMaxRegion().iGet()) + 2) * 3;
		}

		return u_bit_size;
	}


	//******************************************************************************************
	void CTransformedData::WriteBranch(const CQuadNodeTForm& qnt, CBitBuffer& rbb_write)
	{
		//
		// Very efficient encoding (e.g. compression) of the wavelet transformed data into a serial stream
		// can be achieved by observing that simple correlations in the data exist. Specifically, the
		// magnitude of various data elements encoded in the stream define an upper limit for like elements
		// encoded after it. For example, it is clear that the 'branch size' data element of a particular
		// node must always be less than that of its parent.
		//
		// We exploit these correlations by reducing the number of (significant) bits written into stream
		// for a particular element, based on the number of bits required to encode the previous like element.
		//

		// First, write the 'node significance' data element, which is encoded as a single bit.
		bool b_has_data = qnt.stGetState()[CQuadNodeTForm::estSIGNIFICANT] != 0;

		rbb_write.Write(b_has_data, 1);

		if (b_has_data)
		{
			// Next, write the 'branch size' and 'max region coef' data elements of this node into the stream.
			// These elements are always of a lesser magnitude than those of the parent node. We determine
			// how many significant bits were required to encode the parent's data and use that information
			// to limit the number of bits required to encode this node's data. Note also that both elements
			// are unsigned quantities, so we do not need to encode a sign bit.
			if (qnt.ptqnGetParent() != 0)
			{
				rbb_write.Write(qnt.uGetBranchDataBitSize(), uLog2(qnt.ptqnGetParent()->uGetBranchDataBitSize()) + 1);
				rbb_write.Write(qnt.cfGetMaxRegion().iGet(), uLog2(qnt.ptqnGetParent()->cfGetMaxRegion().iGet()) + 1);
			}
			else
			{
				rbb_write.Write(qnt.uGetBranchDataBitSize(), iMAX_ENCODE_SIG_BITS);
				rbb_write.Write(qnt.cfGetMaxRegion().iGet(), iMAX_ENCODE_SIG_BITS);
			}

			// Finally, write the wavelet coeficient values of this node into the stream. The absolute of
			// these wavelet coeficients must always be less than the 'max region coef' data element of the
			// same node. We use this information to limit the number of bits like we did above. Note that
			// the wavelet coeficients are signed quantities, so we need to reserve an extra bit to encode the
			// sign.
			for (int i_wvlt = 0; i_wvlt < 3; i_wvlt++)
			{
				CCoef cf_wvlt = qnt.ptqnGetFirstDescendant()->pqvtGetVertex(i_wvlt + 1)->cfWavelet();

				CBitSignificance bsig_wvlt(cf_wvlt.iGet());

				Assert(uLog2(bsig_wvlt.uGet()) <= uLog2(qnt.cfGetMaxRegion().iGet()) + 1);

				rbb_write.Write(bsig_wvlt.uGet(), uLog2(qnt.cfGetMaxRegion().iGet()) + 2);
			}

			const CQuadNodeTForm* pqnt_dsc = qnt.ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				CTransformedData::WriteBranch(*pqnt_dsc, rbb_write);

				pqnt_dsc = pqnt_dsc->ptqnGetSibling();
			}
		}
	}


	//******************************************************************************************
	void CTransformedData::Read(CBitBuffer& rbb_data, const CTransformedData* ptd_parent)
	{
		uint u_num_data_size_bits  = iMAX_ENCODE_SIG_BITS;
		uint u_num_max_region_bits = iMAX_ENCODE_SIG_BITS;

		if (ptd_parent != 0)
		{
			u_num_data_size_bits  = ptd_parent->uBranchDataSizeSigBits;
			u_num_max_region_bits = uLog2(ptd_parent->cfMaxRegion.iGet()) + 1;
		}

		// First make sure there is data after the parent node, then read the significance bit for
		// this node.
		if (u_num_data_size_bits != 0 && rbb_data.uReadBit() != 0)
		{
			uint u_branch_data_size = rbb_data.uRead(u_num_data_size_bits);
			uBranchDataSizeSigBits  = uLog2(u_branch_data_size) + 1;

			cfMaxRegion = rbb_data.uRead(u_num_max_region_bits);

			// Seek past the wavelet coeficients.
			rbb_data.SeekRel((uLog2(cfMaxRegion.iGet()) + 2) * 3);
		}
		else
		{
			uBranchDataSizeSigBits = 0;
			cfMaxRegion            = cfZERO;
		}

		uDscBitBufferIndex = rbb_data.uCurrIndex();

		// Make sure the index value did not exceed the numerical range of the bitfield reserved for it.
		Assert(uDscBitBufferIndex == rbb_data.uCurrIndex());
	}
};
