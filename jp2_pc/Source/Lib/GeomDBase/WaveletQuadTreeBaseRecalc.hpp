/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Template quad tree base classes for the synthesising wavelet transform.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTreeBaseRecalc.hpp                           $
 * 
 * 11    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 10    5/05/98 4:31p Mlange
 * Reorganised data members among the quad tree class hierarchy.
 * 
 * 9     4/03/98 4:08p Mlange
 * Added CQuadRootRecalcT::ptdhGetWaveletData().
 * 
 * 8     2/04/98 3:26p Mlange
 * Added CQuadVertexRecalc::cfNegUnliftedScaling().
 * 
 * 7     98/01/22 14:47 Speter
 * Added rWorldSize() and rcWorldRectangle().
 * 
 * 6     1/21/98 5:26p Mlange
 * Added constant.
 * 
 * 5     1/21/98 1:34p Mlange
 * Removed unused CQuadVertexRecalc::cfUnliftedScaling().
 * 
 * 4     1/13/98 1:54p Mlange
 * Renamed CQuadNodeBaseRecalcT<>::UpdateDscScalingCoef() to RecalcDscScalingCoef(). Renamed
 * CQuadNodeBaseRecalcT<>::UpdatedVertices() to ModifiedVerticesBranch(). Fixed bug in
 * Recalculate() function; sometimes it would not call the ModifiedVerticesBranch() function
 * when required.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_WAVELETQUADTREEBASERECALC_HPP
#define HEADER_LIB_GEOMDBASE_WAVELETQUADTREEBASERECALC_HPP

#include "Lib/GeomDBase/WaveletQuadTreeBase.hpp"
#include "Lib/GeomDBase/WaveletDataFormat.hpp"
#include "Lib/Std/ArrayAllocator.hpp"


#define dAVG_QUAD_VERTEX_TO_SCALING_ARRAY_RATIO	26.0	// Empirically derived constant, used only to determine the
														// upper limit of allocated memory buffers sizes.


namespace NMultiResolution
{
	//**********************************************************************************************
	//
	class CQuadVertexRecalc : public CQuadVertex
	//
	// Base vertex type for the synthesising wavelet quad tree.
	//
	// Prefix: qvtr
	//
	// Notes:
	//		This class maintains the scaling coeficient value for a vertex at each subdivision
	//		level of the quad tree. To ensure we maintain full precision and the correct rounding
	//		behaviour at each stage (predict/update) of the wavelet synthesising transform, the scaling
	//		coeficients are negated and stored unlifted in the array. Storing the coeficients unlifted
	//		allows us to accumulate wavelet coeficients without losing precision due to rounding. The
	//		need for negating the coeficients is not so obvious. Note that the following relations
	//		hold: S + lift(W) = lift(unlift(S) + W), but: S - lift(W) != lift(unlift(S) - W). Note
	//		also that the wavelet synthesising transform calculates the value of the scaling
	//		coeficients 'S' at each level 'l' from the scaling coeficients at the previous level and
	//		the wavelet coeficients 'W' according to:
	//
	//								S    = S  - lift(sum(W )).
	//								 l+1    l             l
	//
	//		Therefore:
	//
	//				-S    = -S  + lift(sum(W ))  =>  S    = -lift(unlift(-S ) + sum(W )).
	//				  l+1     l             l         l+1                  l         l
	//
	//
	//**************************************
	{
	public:
		static CArrayAllocator<CCoef, iMAX_TRANSFORM_LEVELS> aaScalingAlloc;

	private:
		uint8 u1RootLevel;		// Subdivision level vertex was created at.
		uint8 u1CurrLevel;		// Max subdivision level of vertex.

		uint16 u2ArraySize;		// Size of the array.
		CCoef* pacfScaling;		// Pointer to the array of scaling coeficients, one for each subdivision
								// level in the quad tree.
		CCoef cfWavelet_;		// Wavelet coeficient value of this vertex, or cfZERO if it is not active.

		//******************************************************************************************
		//
		// Constructors and destructor.
		//
	public:
		// Construct vertex for root node.
		CQuadVertexRecalc(int i_x, int i_y, int i_root_node_size, CCoef cf_root_scaling)
			: CQuadVertex(i_x, i_y),
			  u2ArraySize(uLog2(i_root_node_size) + 1), u1RootLevel(0), u1CurrLevel(0), cfWavelet_(cfZERO)
		{
			pacfScaling = aaScalingAlloc.patAlloc(u2ArraySize);

			pacfScaling[0] = -cf_root_scaling.cfUnlift();
		}


		// Construct a new vertex at the midpoint of an edge.
		CQuadVertexRecalc(const CQuadVertexRecalc* pqvt_a, const CQuadVertexRecalc* pqvt_b)
			: CQuadVertex(pqvt_a, pqvt_b), cfWavelet_(cfZERO)
		{
			u2ArraySize = Min(pqvt_a->u2ArraySize, pqvt_b->u2ArraySize) - 1;
			u1RootLevel = Max(pqvt_a->u1RootLevel, pqvt_b->u1RootLevel) + 1;
			u1CurrLevel = u1RootLevel;

			pacfScaling = aaScalingAlloc.patAlloc(u2ArraySize);

			// Predict root scaling coeficient along the midpoint of the edge.
			CCoef cf_predict = (pqvt_a->cfGet(u1RootLevel) + pqvt_b->cfGet(u1RootLevel)).cfPredict();
			pacfScaling[0] = -cf_predict.cfUnlift();
		}


		~CQuadVertexRecalc()
		{
			aaScalingAlloc.Free(pacfScaling, u2ArraySize);
		}


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		uint uGetRootLevel() const
		//
		// Returns:
		//		The root level of this vertex (e.g the quad tree subdivision level at which this
		//		vertex was created).
		//
		//**************************************
		{
			return u1RootLevel;
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
		forceinline CCoef cfScaling() const
		//
		// Returns:
		//		The scaling coeficient of this vertex at its highest subdivision level.
		//
		//**************************************
		{
			return cfGet(u1CurrLevel);
		}


		//******************************************************************************************
		//
		CCoef cfNegUnliftedScaling() const
		//
		// Returns:
		//		The negated, unlifted scaling coeficient of this vertex at its highest subdivision
		//		level.
		//
		// Notes:
		//		Provides fast, immediate access to the internal representation of the scaling
		//		coeficient.
		//
		//**************************************
		{
			return pacfScaling[uGetIndex(u1CurrLevel)];
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
		TReal rZQuad(const SMapping& mp) const
		//
		// Returns:
		//		The value of the scaling coeficient, scaled to quad tree units.
		//
		//**************************************
		{
			return cfScaling().rGet(mp.rCoefToQuad);
		}


		//******************************************************************************************
		//
		TReal rZWorld(const SMapping& mp) const
		//
		// Returns:
		//		The value of the scaling coeficient, scaled to world space units.
		//
		//**************************************
		{
			return cfScaling().rGet(mp.rCoefToWorld);
		}


		//******************************************************************************************
		//
		CVector3<> v3World(const SMapping& mp) const
		//
		// Returns:
		//		The position of this vertex in world space.
		//
		//**************************************
		{
			return CVector3<>
			(
				TReal(iX()) * mp.tlr2QuadToWorld.tlrX,
				TReal(iY()) * mp.tlr2QuadToWorld.tlrY,
				cfScaling().rGet(mp.rCoefToWorld)
			);
		}


		//******************************************************************************************
		//
		CVector3<> v3Quad(const SMapping& mp) const
		//
		// Returns:
		//		The position of this vertex in quad tree units.
		//
		//**************************************
		{
			return CVector3<>(iX(), iY(), cfScaling().rGet(mp.rCoefToQuad));
		}



		//******************************************************************************************
		//
		void SetRootScaling
		(
			const CQuadVertexRecalc* pqvt_a,
			const CQuadVertexRecalc* pqvt_b
		)
		//
		// Calculate the scaling coeficient at the root subdivision level from the two endpoint
		// vertices of the edge along which it lies.
		//
		//**************************************
		{
			Assert(pqvt_a != 0 && pqvt_b != 0);

			// Predict root scaling coeficient along the midpoint of the given edge.
			CCoef cf_predict = (pqvt_a->cfGet(u1RootLevel) + pqvt_b->cfGet(u1RootLevel)).cfPredict();

			CCoef cf_old = pacfScaling[0];

			pacfScaling[0] = -(cf_predict + cfWavelet()).cfUnlift();

			// Determine the amount of change from the last value of the root scaling coeficient and
			// update the scaling coeficients at the higher levels accordingly.
			CCoef cf_diff = pacfScaling[0] - cf_old;

			if (cf_diff != cfZERO)
			{
				for (uint u_index = 1; u_index <= u1CurrLevel - u1RootLevel; u_index++)
					pacfScaling[u_index] += cf_diff;
			}
		}


		//******************************************************************************************
		//
		CCoef cfUpdate
		(
			CCoef cf_wvlt_sum,
			uint u_level
		)
		//
		// Accumulate the given summed wavelet coeficients to the scaling coeficient at the given
		// subdivision level.
		//
		// Returns:
		//		The amount of change (lifted) from last value of scaling coeficient.
		//
		//**************************************
		{
			uint u_index = uGetIndex(u_level);

			CCoef cf_old = pacfScaling[u_index];

			pacfScaling[u_index] += cf_wvlt_sum;

			// Determine the amount of change from the last value of the scaling coeficient at the
			// given level and update the scaling coeficients at the higher levels accordingly.
			CCoef cf_diff = (pacfScaling[u_index].cfLift() - cf_old.cfLift()).cfUnlift();

			if (cf_diff != cfZERO)
			{
				for (u_index++; u_index <= u1CurrLevel - u1RootLevel; u_index++)
					pacfScaling[u_index] += cf_diff;
			}

			return cf_diff;
		}


		//******************************************************************************************
		//
		void DupFrom
		(
			const CQuadVertexRecalc* pqvt,	// Vertex to copy from.
			uint u_level					// Subdivision level to duplicate.
		)
		//
		// Duplicate the scaling coeficient value at the given subdivision level.
		//
		//**************************************
		{
			pacfScaling[uGetIndex(u_level)] = pqvt->pacfScaling[pqvt->uGetIndex(u_level)];
		}


		//******************************************************************************************
		//
		void MakeLevel
		(
			uint u_level
		)
		//
		// Make sure a scaling coeficient is defined at the given subdivision level.
		//
		//**************************************
		{
			if (u_level > u1CurrLevel)
			{
				Assert(u_level - u1CurrLevel == 1);

				// Copy the last scaling coeficient to the next subdivision level, performing the
				// required rounding.
				CCoef cf_last = pacfScaling[u1CurrLevel - u1RootLevel].cfLift();

				u1CurrLevel++;

				pacfScaling[u1CurrLevel - u1RootLevel] = cf_last.cfUnlift();

				Assert(u1CurrLevel - u1RootLevel <= u2ArraySize);
			}
		}

	private:
		//******************************************************************************************
		//
		forceinline CCoef cfGet
		(
			uint u_level
		) const
		//
		// Returns:
		//		The lifted scaling coeficient at the requested subdivision level.
		//
		//**************************************
		{
			return -pacfScaling[uGetIndex(u_level)].cfLift();
		}


		//******************************************************************************************
		//
		forceinline uint uGetIndex
		(
			uint u_level
		) const
		//
		// Returns:
		//		The scaling coeficient array index from the given subdivision level.
		//
		//**************************************
		{
			Assert(u_level <= u1CurrLevel);

			u_level -= u1RootLevel;

			return u_level;
		}
	};



	template<class TD, class TVT> class CQuadRootBaseRecalcT;

	//**********************************************************************************************
	//
	template<class TD, class TVT> class CQuadNodeBaseRecalcT : public CQuadNodeBaseT<TD, TVT>
	//
	// Base node type for the synthesising wavelet quad tree.
	//
	// Prefix: qnr
	//
	// Notes:
	//		Note that even though this is a base class, none of the member functions are declared
	//		virtual. This is deliberate, so to avoid the vtbl size overhead. By passing the derived
	//		class' type as a template parameter and casting the 'this' pointer type to that of the
	//		derived class, it is still possible to overide certain functions in the derived class and
	//		get the expected behaviour.
	//
	//		Ideally, we'd declare all the functions and data as either protected or private.
	//		However, MS compiler gets very confused if we do, so some are necessarily public.
	//
	//**************************************
	{
	public:
		CTransformedData tdData;			// Wavelet transformed data for this node.


		//******************************************************************************************
		//
		// Constructors and destructor.
		//

	protected:
		CQuadNodeBaseRecalcT()
		{
		}

		// Construct a new descendant.
		CQuadNodeBaseRecalcT
		(
			const CQuadRootBaseRecalcT<TD, TVT>* pqnr_root,
			const TD* ptqn_parent, int i_relation, TD* aptqn_subdiv_neighbours[4], TVT* apqvt_dsc[5]
		);


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		void Subdivide
		(
			const CQuadRootBaseRecalcT<TD, TVT>* pqnr_root
		);
		//
		// Subdivide this node, e.g. allocate and initialise four new descendant nodes.
		//
		// Notes:
		//		Initialises the wavelet coeficients of this node and recalculates the necessary
		//		scaling coeficients of the quad tree.
		//
		//**************************************


	private:
		//******************************************************************************************
		//
		void Recalculate
		(
			CCoef acf_wvlts[3]
		);
		//
		// Recalculate the scaling coeficients of all the descendant nodes in this branch of the
		// quad tree.
		//
		//**************************************


	public:
		//******************************************************************************************
		//
		void RecalcDscScalingCoef();
		//
		// Recalculate the scaling coeficients of all the descendant nodes in this branch of the
		// quad tree.
		//
		// Cross reference:
		//		Called by Recalculate.
		//
		//**************************************


		//******************************************************************************************
		//
		void ModifiedVerticesBranch()
		//
		// This function is called after any vertices in this branch of the tree have been
		// recalculated (e.g. modified).
		//
		// Notes:
		//		This function can be overidden by the derived class.
		//
		//**************************************
		{
		}

		//******************************************************************************************
		//
		TReal rWorldSize(const SMapping& mp) const
		//
		// Returns:
		//		The size of this node, in world units.
		//
		//**************************************
		{
			return iGetSize() * mp.tlr2QuadToWorld.tlrX.tScale;
		}

		//******************************************************************************************
		//
		CRectangle<> rcWorldRectangle(const SMapping& mp) const
		//
		// Returns:
		//		The real rectangle, in world space, defining this node.
		//
		//******************************
		{
			return rcGetRectangle() * mp.tlr2QuadToWorld;
		}


		//******************************************************************************************
		//
		// Overides.
		//

		//******************************************************************************************
		void Decimate();
	};



	//**********************************************************************************************
	//
	template<class TD, class TVT> class CQuadRootBaseRecalcT : public CQuadRootBaseT<TD, TVT>
	//
	// Base root node type for the synthesising wavelet quad tree.
	//
	// Prefix: qnrr
	//
	//**************************************
	{
		friend class CQuadNodeBaseRecalcT<TD, TVT>;

		const CTransformedDataHeader* ptdhData;

		//******************************************************************************************
		//
		// Constructors and destructor.
		//
	protected:
		CQuadRootBaseRecalcT(const CTransformedDataHeader* ptdh, TVT* pqvt_0, TVT* pqvt_1, TVT* pqvt_2, TVT* pqvt_3);


	public:
		//******************************************************************************************
		//
		const CTransformedDataHeader* ptdhGetWaveletData() const
		//
		// Returns:
		//		Wavelet data bitstream.
		//
		//******************************
		{
			return ptdhData;
		}
	};
};

#endif
