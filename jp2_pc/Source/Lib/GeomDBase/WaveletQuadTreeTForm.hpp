/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Quad tree types specialised for implementing the forward wavelet transform.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTreeTForm.hpp                                $
 * 
 * 12    5/05/98 4:31p Mlange
 * Reorganised data members among the quad tree class hierarchy.
 * 
 * 11    3/02/98 7:36p Mlange
 * Added CQuadRootTForm::iGetNumSignificantNodes() and CQuadRootTForm::iGetNumVertices().
 * 
 * 10    1/09/98 7:02p Mlange
 * Added convience functions for allocating and freeing the fastheaps used for the wavelet quad
 * tree transform classes.
 * 
 * 9     12/18/97 1:37p Mlange
 * Changed the wavelet transform algorithm to properly use the estDISC_DIAGONAL_1_3 state flag.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_WAVELETQUADTREETFORM_HPP
#define HEADER_LIB_GEOMDBASE_WAVELETQUADTREETFORM_HPP

#include "Lib/GeomDBase/WaveletQuadTreeBase.hpp"
#include "Lib/Std/BlockAllocator.hpp"


#define iTRANSFORM_BUFFER_SIZE_MB	64		// Amount of work memory to allocate for the wavelet transform.


class CTerrainExportedData;
class CRandom;
class CConsoleBuffer;
template<class T> class CRasterT;

namespace NMultiResolution
{
	//**********************************************************************************************
	//
	class CQuadVertexTForm : public CQuadVertex, public CBlockAllocator<CQuadVertexTForm>
	//
	// Vertex type for the wavelet transform quad tree.
	//
	// Prefix: qvtq
	//
	// Notes:
	//		The wavelet transform calculates wavelet coeficients from neighbouring scaling
	//		coeficients. If the wavelet coeficient has not been calculated, it is used to
	//		accumulate the 'lifting scalar' for the wavelet transform update stage.
	//
	//**************************************
	{
		friend class CBlockAllocator<CQuadVertexTForm>;
		static CBlockAllocator<CQuadVertexTForm>::SStore stStore; // Storage for the types.

		CCoef cfWavelet_;		// Wavelet coeficient value of this vertex, or cfZERO if it is not active.
		CCoef cfScaling_;		// Scaling coeficient value of this vertex, cfNAN if none specified.

	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//
		CQuadVertexTForm(int i_x, int i_y)
			: CQuadVertex(i_x, i_y), cfWavelet_(cfZERO), cfScaling_(cfNAN)
		{
		}

		CQuadVertexTForm(const CQuadVertex* pqvt_a, const CQuadVertex* pqvt_b)
			: CQuadVertex(pqvt_a, pqvt_b), cfWavelet_(cfZERO), cfScaling_(cfNAN)
		{
		}


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		bool bHasScaling() const
		//
		// Returns:
		//		'true' if the scaling coeficient has been initialised.
		//
		//**************************************
		{
			return cfScaling_ != cfNAN;
		}


		//******************************************************************************************
		//
		CCoef cfWavelet() const
		//
		// Returns:
		//		The wavelet coeficient of this vertex, or cfZERO if not 'activated'.
		//
		//**************************************
		{
			return cfWavelet_;
		}


		//******************************************************************************************
		//
		CCoef cfScaling() const
		//
		// Returns:
		//		The scaling coeficient of this vertex.
		//
		//**************************************
		{
			Assert(cfScaling_ != cfNAN);
			return cfScaling_;
		}


		//******************************************************************************************
		//
		void SetWavelet(CCoef cf)
		//
		// Set the value of the wavelet coeficient.
		//
		//**************************************
		{
			cfWavelet_ = cf;
		}


		//******************************************************************************************
		//
		void SetScaling(CCoef cf)
		//
		// Set the value of the scaling coeficient.
		//
		//**************************************
		{
			cfScaling_ = cf;
		}


		//******************************************************************************************
		//
		void InitWavelet
		(
			CCoef cf_wvlt	// Initial wavelet coeficient value.
		)
		//
		// Initialises this vertex as a wavelet coeficient and invalidates the scaling coeficient.
		//
		//**************************************
		{
			SetWavelet(cf_wvlt);
			SetScaling(cfNAN);
		}


		//******************************************************************************************
		//
		void SumWavelet
		(
			CCoef cf_wvlt
		)
		//
		// Accumulate the given wavelet for the subsequent lifting step.
		//
		//**************************************
		{
			SetWavelet(cfWavelet() + cf_wvlt);
		}


		//******************************************************************************************
		//
		void Lift()
		//
		// Update this vertex with the accumulated lifting scalar. Also zeroes the lifting
		// accumulator.
		//
		//**************************************
		{
			SetScaling(cfScaling() + cfWavelet().cfLift());
			SetWavelet(cfZERO);
		}
	};


	class CTransformedData;
	class CTransformedDataHeader;

	//**********************************************************************************************
	//
	class CQuadNodeTForm : public CQuadNodeBaseT<CQuadNodeTForm, CQuadVertexTForm>,
	                       public CBlockAllocator<CQuadNodeTForm>
	//
	// Definition of a single node in the wavelet transform quad tree.
	//
	// Prefix: qnt
	//
	//**************************************
	{
		friend class CBlockAllocator<CQuadNodeTForm>;
		static CBlockAllocator<CQuadNodeTForm>::SStore stStore; // Storage for the types.

	protected:
		CCoef cfMaxRegion;			// The absolute maximum wavelet coeficient for this node and all its descendants.
		uint uBranchDataBitSize;	// Bit size of the encoded wavelet transform data for this node and all its descendants.

		//******************************************************************************************
		//
		// Constructors and destructor.
		//
	public:
		CQuadNodeTForm()
			: cfMaxRegion(cfZERO), uBranchDataBitSize(TypeMax(int))
		{
		}

		// Construct a new descendant. See CWaveletQuadTreeBaseT<> for a description.
		CQuadNodeTForm
		(
			const CQuadNodeTForm* ptqn_parent, int i_relation, CQuadNodeTForm* aptqn_subdiv_neighbours[4], CQuadVertexTForm* apqvt_dsc[5]
		);

	
		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		CCoef cfGetMaxRegion() const
		//
		// Returns:
		//		The absolute maximum wavelet coeficient value in this branch of the quad tree.
		//
		//**************************************
		{
			return cfMaxRegion;
		}


		//******************************************************************************************
		//
		uint uGetBranchDataBitSize() const
		//
		// Returns:
		//		The size, in bits, of the encoded wavelet transform data in this branch of the quad
		//		tree.
		//
		// Notes:
		//		The value returned by this function refers to the encoded size of the wavelet
		//		transformed data, i.e. not the size of the data structures associated with this
		//		branch of the quad tree. See also the wavelet transformed data format classes.
		//
		//**************************************
		{
			return uBranchDataBitSize;
		}


		//******************************************************************************************
		//
		void CalcBranchDataSize();
		//
		// Calculate the size of the encoded wavelet transform data in this branch of the quad tree.
		//
		// Notes:
		//		This function must be called after the analyse stage.
		//
		//**************************************


		//******************************************************************************************
		//
		void Grow();
		//
		// Grow this node e.g. add four descendants.
		//
		// Notes:
		//		This function ensures that the subdivision level of the neighbouring nodes never
		//		differs by more than one relative to the subdivision level of this node.
		//
		//		This is necessary to ensure that for each calculated wavelet coeficient, all the
		//		affected (e.g. lifted) scaling coeficients are defined in the quad tree.
		//
		//**************************************


	protected:
		//******************************************************************************************
		//
		void InterpolateUndefinedPoints
		(
			CConsoleBuffer& rcon_text_out,
			bool b_recursed = false
		);
		//
		// Interpolate the scaling coeficients in this branch of the quad tree that are unspecified
		// by any data points that have been added to the quad tree.
		//
		// Notes:
		//		Must be called after all points have been added to the quad tree.
		//
		//**************************************


		//******************************************************************************************
		//
		void AnalyseStep
		(
			int i_req_node_size		// Size of a node containing the wavelet coeficients to analyse
									// for the current decomposition level.
		);
		//
		// Analyse (e.g. calculate) the wavelet coeficients for a particular decomposition level.
		//
		// Notes:
		//		This function performs all the stages (split, predict and update) of the wavelet
		//		transform lifting scheme for the current decomposition level.
		//
		//		Level 0 defines the first decomposition stage. The level index is incremented after
		//		each analyse step. The analyse steps must be performed in order, starting with
		//		level 0.
		//
		//		This function will delete all nodes that contain wavelet transformed data that is
		//		insignificant.
		//
		//**************************************


	private:
		//******************************************************************************************
		//
		CCoef cfScalingReflect
		(
			int i_vt_num
		) const
		//
		// Returns:
		//		The scaling coeficient of the requested vertex, reflected with the maximum boundary
		//		of the quad tree.
		//
		//**************************************
		{
			i_vt_num &= 3;

			if (stState[estBOUNDARY_MAX_X])
				i_vt_num = (i_vt_num >> 1) | (i_vt_num & 2);

			if (stState[estBOUNDARY_MAX_Y])
				i_vt_num = (i_vt_num ^ (i_vt_num >> 1)) & 1;

			return pqvtGetVertex(i_vt_num)->cfScaling();
		}


	protected:
		//******************************************************************************************
		//
		void FillRaster
		(
			CRasterT<CCoef>* pras
		) const;
		//
		// Fills the given raster with the scaling coeficients in this branch of the quad tree.
		//
		// Notes:
		//		Used for debugging only. This function must be called before the analyse stage.
		//
		//**************************************


		//******************************************************************************************
		//
		void CheckWavelets
		(
			CRasterT<CCoef>* pras
		) const;
		//
		// Compares the wavelet coeficients in the given raster with the wavelet coeficients in this
		// branch of the quad tree and asserts for a mismatch.
		//
		// Notes:
		//		Used for debugging only. This function must be called after the analyse stage. For
		//		checked wavelet coeficient a zero value is written to raster.
		//
		//**************************************


		//******************************************************************************************
		//
		void CheckDataBranch
		(
			const CTransformedData& td,
			const CTransformedDataHeader& tdh
		) const;
		//
		// Compares the given transformed data stream with the wavelet coeficients in this branch of
		// the quad tree and asserts for a mismatch.
		//
		// Notes:
		//		Used for debugging only. This function must be called after the analyse stage.
		//
		//**************************************
	};


	//**********************************************************************************************
	//
	class CQuadRootTForm : public CQuadRootBaseT<CQuadNodeTForm, CQuadVertexTForm>
	//
	// Definition of the root quad node for the wavelet transform quad tree.
	//
	// Prefix: qntr
	//
	// Notes:
	//		The analysing wavelet transform type is defined by a 0-disc wavelet and the filter
	//		specified by the CCoef class.
	//
	//**************************************
	{
		int   iNumPointsAdded;			// Count of the number of data points added to the quad tree.
		TReal rMaxQuantisationError;	// Maximum data point quantisation error.

		int iLevel;						// Current wavelet transform decomposition level.

		CConsoleBuffer& rconTextOut;

	public:
		SMapping mpConversions;			// Conversions between quad - world space.

		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		// Construct the root node for the tree.
		CQuadRootTForm(const SExportDataInfo& edi, CConsoleBuffer& rcon_text_out);

		~CQuadRootTForm();

		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		static void AllocMemory();
		//
		// Allocate fastheaps used to store wavelet quad tree transform types.
		//
		// Notes:
		//		Must be called *before* any instances of this class are created.
		//
		//**************************************


		//******************************************************************************************
		//
		static void FreeMemory();
		//
		// Free fastheaps used to store wavelet quad tree transform types.
		//
		// Notes:
		//		Must be called *after* all instances of this class are destructed.
		//
		//**************************************


		//******************************************************************************************
		//
		int iGetNumSignificantNodes() const;
		//
		// Returns:
		//		The number of significant nodes in the quad tree, including this root node.
		//
		//**************************************


		//******************************************************************************************
		//
		int iGetNumVertices() const;
		//
		// Returns:
		//		The number of data points in the quad tree.
		//
		//**************************************


		//******************************************************************************************
		//
		void AddPoints
		(
			CTerrainExportedData& rted
		);
		//
		// Add data points from an exported terrain file to the quad tree.
		//
		// Notes:
		//		Quad tree scaling coeficients undefined by any data points are interpolated.
		//
		//**************************************


		//******************************************************************************************
		//
		void RandomisePoints
		(
			const SExportDataInfo& edi,	// Dummy exported data info defining bounds and count of
										// randomised points.
			CRandom& rrnd				// Randomiser to use.
		);
		//
		// Add a set of randomised points to the quad tree.
		//
		// Notes:
		//		Used for debugging only.
		//
		//**************************************


		//******************************************************************************************
		//
		void Analyse();
		//
		// Perform analysing wavelet transform.
		//
		// Notes:
		//		Must be called after all the data points have been added to the quad tree.
		//
		//**************************************


		//******************************************************************************************
		//
		CRasterT<CCoef>* prasGetData() const;
		//
		// Returns:
		//		Dynamically allocated raster containing the scaling coeficients in the quad tree.
		//		The returned raster will contain 'cfNAN' for all undefined scaling coeficients in the
		//		quad tree.
		//
		// Notes:
		//		Used for debugging only. This function must be called before the analyse stage. The
		//		calling function is responsible for deleting the raster when done.
		//
		//**************************************


		//******************************************************************************************
		//
		void CheckData
		(
			const CRasterT<CCoef>* pras_cmp
		) const;
		//
		// Compares the wavelet coeficients in the given raster with the wavelet coeficients in the
		// quad tree and asserts for a mismatch.
		//
		// Notes:
		//		Used for debugging only. This function must be called after the analyse stage.
		//
		//**************************************


		//******************************************************************************************
		//
		void CheckData
		(
			const CTransformedDataHeader& tdh
		) const;
		//
		// Compares the given transformed data stream with the wavelet coeficients in the quad tree
		// and asserts for a mismatch.
		//
		// Notes:
		//		Used for debugging only. This function must be called after the analyse stage.
		//
		//**************************************

	private:
		//******************************************************************************************
		//
		void AddPoint
		(
			const CVector3<>& v3_world_pos,	// Definition of point to add in world space.
			int i_pt_num,					// Point's number.
			int i_total_num_points			// Total # points to add.
		);
		//
		// Add a single point to the wavelet quad tree. The quad tree is refined to accomodate the
		// point if necessary.
		//
		//**************************************
	};
};

#endif
