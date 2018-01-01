/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Quad tree types specialised for efficient topology queries of the wavelet transformed
 *		data representation.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTreeQuery.hpp                                $
 * 
 * 37    98/09/08 13:06 Speter
 * Added esfEdgeIntersects().
 * 
 * 36    8/26/98 2:47p Rvande
 * Changed a constant double to a float.
 * 
 * 35    98/08/10 21:50 Speter
 * Cached pointers now cleared in SetEvaluateReq().
 * 
 * 34    98/08/10 12:51 Speter
 * Replaced Fuzzy with Furry equals in plane assert. Added CQueryRect::ptriLastTri for
 * optimisation, currently not used.
 * 
 * 33    98/08/02 16:44 Speter
 * Replaced silly bAsHighAs with a function that just returns the frickin max height.
 * 
 * 32    98/07/30 22:19 Speter
 * Added CTriangleQuery::rHeight() function, using cached inverse Z (not currently called).
 * Changed rHeight functions to return CPlane* rather than CPlane* (faster). Added ptriFind(),
 * bAsHighAs(), and cached pqnqLastNode and rZMax to CQueryRect, for faster terrain queries.
 * 
 * 31    6/29/98 3:40p Mlange
 * Terrain ray cast integrated with general ray cast system.
 * 
 * 30    6/24/98 12:29p Mlange
 * Added edge iterator to query rect.
 * 
 * 29    6/18/98 5:22p Mlange
 * Added ray-terrain intersect function.
 * 
 * 28    98/06/09 21:28 Speter
 * Added overload of rHeight() which also returns plane.
 * 
 * 27    98/05/25 15:34 Speter
 * Added rHeight() function. Made Evaluate() public.
 * 
 * 26    5/05/98 4:31p Mlange
 * Reorganised data members among the quad tree class hierarchy.
 * 
 * 25    4/23/98 4:35p Mlange
 * Wavelet quad tree data queries can now specify their frequence cutoff as a ratio of a node's
 * size.
 * 
 * 24    1/26/98 8:12p Mlange
 * CTriangle<> vertex pointers are now non-const.
 * 
 * 23    1/21/98 5:28p Mlange
 * Changed interface for allocating the fastheaps for the wavelet quad tree types. Improved some
 * stat printing.
 * 
 * 22    1/16/98 3:59p Mlange
 * Added support for highpass frequency filtering on wavelet data queries.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_WAVELETQUADTREEQUERY_HPP
#define HEADER_LIB_GEOMDBASE_WAVELETQUADTREEQUERY_HPP

#include <list.h>
#include "Lib/Std/BlockAllocator.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/GeomDBase/Plane.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeBaseTri.hpp"


class CBoundVol;
class CConsoleBuffer;
class CFastHeap;
class CBitBuffer;
struct SVolumeLoc;

namespace NMultiResolution
{
	//**********************************************************************************************
	//
	class CQuadVertexQuery : public CQuadVertexRecalc,
	                         public CBlockAllocator<CQuadVertexQuery>
	//
	// Vertex type for the query quad tree.
	//
	// Prefix: qvtq
	//
	//**************************************
	{
		friend class CBlockAllocator<CQuadVertexQuery>;
		static CBlockAllocator<CQuadVertexQuery>::SStore stStore; // Storage for the types.

	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//
		CQuadVertexQuery(int i_x, int i_y, int i_root_node_size, CCoef cf_root_scaling)
			: CQuadVertexRecalc(i_x, i_y, i_root_node_size, cf_root_scaling)
		{
		}


		CQuadVertexQuery(const CQuadVertexQuery* pqvt_a, const CQuadVertexQuery* pqvt_b)
			: CQuadVertexRecalc(pqvt_a, pqvt_b)
		{
		}
	};



	//**********************************************************************************************
	//
	class CTriangleQuery : public CTriangleT<CTriangleQuery, CQuadVertexQuery>,
	                       public CBlockAllocator<CTriangleQuery>
	//
	// Definition of a triangle for the query wavelet quad tree.
	//
	//**************************************
	{
		friend class CBlockAllocator<CTriangleQuery>;
		static CBlockAllocator<CTriangleQuery>::SStore stStore; // Storage for the types.

		CPlane plPlane;				// The plane through this triangle, in world space.
		TReal rNegInvZ;				// The negative inverse Z component of the plane, 
									// for fast height computations.

		CVector3<> av3Corners[3];	// The three points that make up the triangle, in world space.

		//******************************************************************************************
		//
		// Constructors and destructor.
		//

	public:
		CTriangleQuery()
		{
		}


		CTriangleQuery
		(
			CQuadVertexQuery* pqvt_a, CQuadVertexQuery* pqvt_b, CQuadVertexQuery* pqvt_c,
			CTriangleQuery* pptri_link_after
		)
			: CTriangleT<CTriangleQuery, CQuadVertexQuery>(pqvt_a, pqvt_b, pqvt_c, pptri_link_after)
		{
			plPlane.d3Normal.tZ = -1;
		}

		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		void Init
		(
			const SMapping& mp
		)
		//
		// Initialise this triangle.
		//
		//**************************************
		{
			if (CIntFloat(plPlane.d3Normal.tZ).bSign())
			{
				av3Corners[0] = apqvtVertices[0]->v3World(mp);
				av3Corners[1] = apqvtVertices[1]->v3World(mp);
				av3Corners[2] = apqvtVertices[2]->v3World(mp);

				plPlane = CPlane(av3Corners[0], av3Corners[1], av3Corners[2]);
				rNegInvZ = -1.0f / plPlane.d3Normal.tZ;
			}

			#if VER_DEBUG
				CPlane pl_cmp(av3Corners[0], av3Corners[1], av3Corners[2]);
				Assert(Fuzzy(pl_cmp.d3Normal) == plPlane.d3Normal && bFurryEquals(pl_cmp.rD, plPlane.rD));
			#endif

			Assert(plPlane.d3Normal.tZ >= 0);
		}


		//******************************************************************************************
		//
		const CPlane& plGetPlaneWorld() const
		//
		// Returns:
		//		The plane through this triangle, in world space.
		//
		//**************************************
		{
			Assert(!CIntFloat(plPlane.d3Normal.tZ).bSign());
			return plPlane;
		}


		//******************************************************************************************
		//
		const CVector3<>& v3Get
		(
			uint u_vt_num
		) const
		//
		// Returns:
		//		The requested point of this triangle, in world space.
		//
		//**************************************
		{
			Assert(u_vt_num < 3);
			Assert(!CIntFloat(plPlane.d3Normal.tZ).bSign());
			return av3Corners[u_vt_num];
		}

		//******************************************************************************************
		//
		TReal rHeight
		(
			TReal r_x, TReal r_y				// X and Y location (any space).
		) const
		//
		// Returns:
		//		The Z value on this triangle at the given X and Y locations.
		//
		// Notes:
		//		This function does not check that r_x and r_y are within the triangle;
		//		it simply uses the triangle's plane to compute the height. Also, r_x and r_y
		//		can be in any space (world, quad, etc.), and the returned height will be in
		//		the same space.
		//
		//**************************************
		{
			Assert(!CIntFloat(plPlane.d3Normal.tZ).bSign());
			return (plPlane.d3Normal.tX * r_x + plPlane.d3Normal.tY * r_y + plPlane.rD) * rNegInvZ;
		}

		//******************************************************************************************
		//
		float fRayIntersects
		(
			CVector3<>& rv3_intersection,			// Initialised with the point of intersection.
			const CVector3<>& v3_ray_world_start,
			const CVector3<>& v3_ray_world_delta,
			const SMapping& mp
		);
		//
		// Determine if the given ray intersects this triangle.
		//
		// Returns:
		//		The fraction along the ray [0..1] where it intersects this triangle, or -1 if it
		//		does not intersect.
		//
		//**************************************


		//******************************************************************************************
		//
		// Overides.
		//

		//******************************************************************************************
		void UpdatedVertices()
		{
			plPlane.d3Normal.tZ = -1;
		}
	};



	//**********************************************************************************************
	//
	// Declarations for CQuadNodeQuery.
	//
	class CQuadNodeQuery;

	//**********************************************************************************************
	//
	class CTriNodeInfoQuery : public CTriNodeInfoT<CTriangleQuery, CQuadVertexQuery>,
	                          public CBlockAllocator<CTriNodeInfoQuery>
	//
	//
	//**************************************
	{
		friend class CBlockAllocator<CTriNodeInfoQuery>;
		static CBlockAllocator<CTriNodeInfoQuery>::SStore stStore; // Storage for the types.

		friend class CQuadNodeQuery;

		TReal rMinZ;
		TReal rMaxZ;

		//******************************************************************************************
		//
		// Constructors and destructor.
		//

	public:
		CTriNodeInfoQuery()
			: rMinZ(FLT_MAX), rMaxZ(FLT_MAX)
		{
		}


		//******************************************************************************************
		//
		// Overides.
		//

		//******************************************************************************************
		void ModifiedVertices()
		{
			CTriNodeInfoT<CTriangleQuery, CQuadVertexQuery>::ModifiedVertices();

			rMinZ = FLT_MAX;
			rMaxZ = FLT_MAX;
		}
	};




	//**********************************************************************************************
	//
	// Declarations for CQuadNodeQuery.
	//
	class CQueryRect;
	class CQuadRootQuery;

	//**********************************************************************************************
	//
	class CQuadNodeQuery : public CQuadNodeBaseTriT<CQuadNodeQuery, CQuadVertexQuery, CTriNodeInfoQuery, CTriangleQuery>,
	                       public CBlockAllocator<CQuadNodeQuery>
	//
	// Definition of a single node in the wavelet topology quad tree.
	//
	// Prefix: qnq
	//
	//**************************************
	{
		friend class CBlockAllocator<CQuadNodeQuery>;
		static CBlockAllocator<CQuadNodeQuery>::SStore stStore; // Storage for the types.

		friend class CQueryRect;

	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		CQuadNodeQuery()
		{
		}

		// Construct a new descendant. See CWaveletQuadTreeBaseT<> for a description.
		CQuadNodeQuery
		(
			const CQuadRootBaseRecalcT<CQuadNodeQuery, CQuadVertexQuery>* pqnr_root, const CQuadNodeQuery* ptqn_parent,
			int i_relation, CQuadNodeQuery* aptqn_subdiv_neighbours[4], CQuadVertexQuery* apqvt_dsc[5]
		)
			: CQuadNodeBaseTriT<CQuadNodeQuery, CQuadVertexQuery, CTriNodeInfoQuery, CTriangleQuery>(pqnr_root, ptqn_parent, i_relation, aptqn_subdiv_neighbours, apqvt_dsc)
		{
		}

		// Destructor.
		~CQuadNodeQuery()
		{
			Assert(!stState[estSIGNIFICANT]);
		}


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		pair<TReal, TReal> prrGetWorldZLimits
		(
			const CQuadRootQuery* pqnq_root
		);
		//
		// Returns:
		//
		//**************************************


	protected:
		struct SRayIterateContext;

		//******************************************************************************************
		//
		void RayEvaluateBranch
		(
			const SRayIterateContext& ric
		);
		//
		// Evaluate and, if necessary, refine this branch of the quad tree for the ray cast.
		//
		// Notes:
		//		Sets the significance bit for each quad node that contains wavelet coeficients that
		//		influence the data values in for the ray cast.
		//
		//**************************************


		//******************************************************************************************
		//
		bool bRayIntersectsBranch
		(
			SRayIterateContext& rric
		);
		//
		// Determine if a ray intersects this branch of the quad tree.
		//
		// Returns:
		//		'true' if the ray intersects.
		//
		//**************************************


	private:
		//******************************************************************************************
		//
		void AlignQueryExtent
		(
			CQueryRect& qr
		);
		//
		//**************************************


		//******************************************************************************************
		//
		void EvaluateQueryBranch
		(
			const CQueryRect& qr		// The extents of the region to evaluate.
		);
		//
		// Evaluate and, if necessary, refine this branch of the quad tree for the given region.
		//
		// Notes:
		//		Sets the significance bit for each quad node that contains wavelet coeficients that
		//		influence the data values in the given region.
		//
		//**************************************


		//******************************************************************************************
		//
		void IterateBranch
		(
			CQueryRect& qr
		) const;
		//
		//**************************************


	protected:
		//******************************************************************************************
		//
		bool bDecimateTree();
		//
		// Removes nodes that are insignificant from this branch of the quad tree, and deletes them.
		// Also clears the significance bit for *every* remaining node in this branch.
		//
		// Returns:
		//		'true' if insignificant nodes were removed from the tree.
		//
		//**************************************
	};



	//**********************************************************************************************
	//
	// Definitions for CQuadRootQuery.
	//
	#define uDEFAULT_ALLOC_QUERY_NODES	7000u

	//**********************************************************************************************
	//
	class CQuadRootQuery : public CQuadRootBaseTriT<CQuadNodeQuery, CQuadVertexQuery, CTriNodeInfoQuery, CTriangleQuery>
	//
	// Definition of the root quad node for the wavelet topology quad tree.
	//
	// Prefix: qnq
	//
	// Notes:
	//		This class extends the functionality of the quad node base class for the purposes of
	//		efficient queries of the (world space) topology of the original data. By 'original data'
	//		we refer to the 2D rectangular array of data values used as input to the wavelet
	//		transform. In other words, for a query we must reconstruct the original data from its
	//		current wavelet-transformed (multiresolution) representation.
	//
	//		Note that, for efficiency and to conserve memory, we do not reconstruct *all* of the
	//		original data values. We assume that queries are done in a relatively small number
	//		of compact areas. So instead, the user must first specify a small rectangular bounding
	//		volume for the area to be queried. Only within this region are the original data
	//		values reconstructed.
	//
	//		To reconstruct the data values for a given region we must identify all of the wavelet
	//		coeficients that contribute to (e.g. modify) these data values. We refine the quad tree
	//		as necessary and add each of these wavelet coeficients to it. Then the quad tree
	//		vertices within the given region will have the same value as the original data values at
	//		that location.
	//
	//		Further, we assume that, generally, these regions are modified incrementally (i.e. move)
	//		and are queried many times. Thus, for efficiency we maintain the quad tree between
	//		successive queries (instead of refining the quad tree for each query and then
	//		immediately decimate it so to reclaim the memory). Also, it is likely that at least
	//		some of the query regions overlap and this scheme will effectively avoid refining the
	//		quad tree multiple times for the areas where the regions overlap.
	//
	//		In practice this is fairly straightforward to implement. First, the quad tree is
	//		evaluated for each of the query regions. This involves refining the quad tree where
	//		necessary and setting the significance status bit for each of quad nodes that contains
	//		wavelet coeficients that contribute to a region. Note that (re)setting the bit for each
	//		significant quad node is critical to maintain the quad tree in the subsequent decimate
	//		pass. In this decimate pass each quad node is evaluated; any insignificant nodes are
	//		deleted. The remaining nodes have their significance status bit cleared. So, unless
	//		these remaining nodes are still required for any query region(s), (in which case their
	//		significance bit will be re-set in the next iteration) they will be deleted in the next
	//		decimate pass.
	//
	//**************************************
	{
		friend class CQueryRect;

		static CProfileStat psUpdate;
		static CProfileStat psRayEvaluate;
		static CProfileStat psRayRefine;
		static CProfileStat psRayIntersect;

		list<CQueryRect*> ltpqrQueries;

	public:
		SMapping mpConversions;					// Conversions between quad - world space.

		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		// Construct the root node for the tree.
		CQuadRootQuery
		(
			const CTransformedDataHeader* ptdh	// Wavelet transformed data.
		);


		~CQuadRootQuery();

		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		static void AllocMemory
		(
			uint u_max_nodes = uDEFAULT_ALLOC_QUERY_NODES
		);
		//
		// Allocate fastheaps used to store wavelet quad tree query types.
		//
		// Notes:
		//		Must be called *before* any instances of this class are created.
		//
		//**************************************


		//******************************************************************************************
		//
		static void FreeMemory();
		//
		// Free fastheaps used to store wavelet quad tree query types.
		//
		// Notes:
		//		Must be called *after* all instances of this class are destructed.
		//
		//**************************************


		//******************************************************************************************
		//
		void RegisterQuery
		(
			CQueryRect* pqr
		)
		//
		//
		//**************************************
		{
			ltpqrQueries.push_back(pqr);
		}


		//******************************************************************************************
		//
		void UnregisterQuery
		(
			CQueryRect* pqr
		)
		//
		//
		//**************************************
		{
			ltpqrQueries.remove(pqr);
		}


		//******************************************************************************************
		//
		void Update();
		//
		// Update step for the quad tree after all queries have been done.
		//
		// Notes:
		//		Removes nodes that are insignificant from the quad tree, and deletes them.
		//
		//**************************************


		//******************************************************************************************
		//
		TReal rHeight
		(
			TReal r_x, TReal r_y		// World-space coordinates to query at.
		) const
		//
		// Returns:
		//		The Z value of the triangulation (in world space) at the quad point specified.
		//		Uses current quad-tree refinement, does not refine further.
		//		Returns 0 if the point is not contained in this branch of the tree.
		//
		//******************************
		{
			const CPlane* ppl;
			return rHeight(r_x, r_y, &ppl);
		}


		//******************************************************************************************
		//
		TReal rHeight
		(
			TReal r_x, TReal r_y,		// World-space coordinates to query at.
			const CPlane** pppl			// Return pointer to plane at this point.
		) const;
		//
		// Returns:
		//		Height above terrain, as above.
		//
		//******************************


		//******************************************************************************************
		//
		bool bRayIntersects
		(
			SVolumeLoc* pvl_init,
			const CVector3<>& v3_ray_world_start,
			const CVector3<>& v3_ray_world_end
		);
		//
		// Determine if a ray intersects a triangle in this quad tree.
		//
		// Returns:
		//		'true' if it intersects. The volume location is initialised with the nearest
		//		point to the start of the ray where it intersected.
		//
		//**************************************


		//******************************************************************************************
		//
		// Overides.
		//

		//******************************************************************************************
		void PrintStats(CConsoleBuffer& con) const;
	};



	//**********************************************************************************************
	//
	// Definitions for CQueryRect
	//
	#define uDEFAULT_MAX_NODES_IN_QUERY		1000u

	//**********************************************************************************************
	//
	class CQueryRect
	//
	// Defines the rectangular area in which the quad tree is evaluated for subsequent topology
	// queries.
	//
	// Prefix: qr
	//
	//**************************************
	{
	public:
		//**********************************************************************************************
		//
		class CIterator
		//
		// Iterate the contents of the query for triangles.
		//
		// Prefix: it
		//
		//**************************************
		{
			friend class CQueryRect;

			uint uCurrNodeIndex;		// Index into array of quad node pointers for the current node being iterated.
			CTriangleQuery* ptriCurr;	// Current triangle (of current quad node) being iterated.

		public:
			CQueryRect* pqrContainer;	// Pointer to owning query.


			//******************************************************************************************
			//
			// Constructors and destructor.
			//
		private:
			CIterator(CQueryRect* pqr)
				: pqrContainer(pqr)
			{
				pqrContainer->IterateLock();
				Reset();
			}

		public:
			CIterator(const CIterator& it)
				: pqrContainer(it.pqrContainer), uCurrNodeIndex(it.uCurrNodeIndex), ptriCurr(it.ptriCurr)
			{
				pqrContainer->IterateLock();
			}


			~CIterator()
			{
				pqrContainer->IterateUnlock();
			}


			//*****************************************************************************************
			//
			// Member functions.
			//

			//*****************************************************************************************
			//
			void Reset()
			//
			// Reset the iteration back to the first triangle.
			//
			//**************************
			{
				uCurrNodeIndex = 0;
				ptriCurr = pqrContainer->dapqnqIterateStore[0]->ptriGetFirst();
			}


			//*****************************************************************************************
			//
			void NextQuadNode()
			//
			// Advance the iteration to the next quad node.
			//
			//**************************
			{
				Assert(uCurrNodeIndex < pqrContainer->dapqnqIterateStore.uLen);

				uCurrNodeIndex++;

				if (uCurrNodeIndex < pqrContainer->dapqnqIterateStore.uLen)
					ptriCurr = pqrContainer->dapqnqIterateStore[uCurrNodeIndex]->ptriGetFirst();
				else
					ptriCurr = 0;
			}


			//*****************************************************************************************
			//
			CQuadNodeQuery* pqnqGetNode() const
			//
			// Returns:
			//		The current node being iterated.
			//
			//**************************
			{
				return pqrContainer->dapqnqIterateStore[uCurrNodeIndex];
			}


			//*****************************************************************************************
			//
			// Operators.
			//

			//*****************************************************************************************
			operator bool() const
			{
				return ptriCurr != 0;
			}

			//*****************************************************************************************
			void operator ++()
			{
				Assert(ptriCurr != 0 && uCurrNodeIndex < pqrContainer->dapqnqIterateStore.uLen);

				ptriCurr = ptriCurr->ptriGetNext();

				if (ptriCurr == 0)
					NextQuadNode();
			}

			//*****************************************************************************************
			const CTriangleQuery* operator *() const
			{
				Assert(ptriCurr != 0);
				ptriCurr->Init(pqrContainer->pqnqRoot->mpConversions);
				return ptriCurr;
			}
		};


		//**********************************************************************************************
		//
		class CEdgeIterator
		//
		// Iterate the contents of the query for edges.
		//
		// Prefix: eit
		//
		// Notes:
		//		Each edge is iterated once only.
		//
		//		Edges coincident with the left and top boundaries of the query rect are *not* iterated.
		//
		//**************************************
		{
			friend class CQueryRect;

			uint uCurrNodeIndex;		// Index into array of quad node pointers for the current node being iterated.
			uint uCurrEdgeIndex;

			bool bInternal;				// 'true' if iterating internal node edges.
			bool bTriFan;				// 'true' if current node is a triangle fan, otherwise it's a tri strip.
			bool bNodeDisc1_3;			// Configuration of tri strip.

			CTriangleQuery* ptriCurr;	// Current triangle (of current quad node) being iterated.

		public:
			CQueryRect* pqrContainer;	// Pointer to owning query.


			//******************************************************************************************
			//
			// Constructors and destructor.
			//
		private:
			CEdgeIterator(CQueryRect* pqr)
				: pqrContainer(pqr)
			{
				pqrContainer->IterateLock();
				Reset();
			}

		public:
			CEdgeIterator(const CEdgeIterator& it)
				: pqrContainer(it.pqrContainer),
				  uCurrNodeIndex(it.uCurrNodeIndex), ptriCurr(it.ptriCurr), uCurrEdgeIndex(it.uCurrEdgeIndex),
				  bInternal(it.bInternal), bTriFan(it.bTriFan), bNodeDisc1_3(it.bNodeDisc1_3)
			{
				pqrContainer->IterateLock();
			}


			~CEdgeIterator()
			{
				pqrContainer->IterateUnlock();
			}


			//*****************************************************************************************
			//
			// Member functions.
			//

			//*****************************************************************************************
			//
			void Reset();
			//
			// Reset the iteration back to the first edge.
			//
			//**************************


			//*****************************************************************************************
			//
			void NextQuadNode();
			//
			// Advance the iteration to the next quad node.
			//
			//**************************


			//*****************************************************************************************
			//
			CQuadNodeQuery* pqnqGetNode() const
			//
			// Returns:
			//		The current node being iterated.
			//
			//**************************
			{
				return pqrContainer->dapqnqIterateStore[uCurrNodeIndex];
			}


			//*****************************************************************************************
			//
			void GetEdge
			(
				CVector3<>& rv3_a,	// Initialised with endpoints of edge.
				CVector3<>& rv3_b
			) const
			//
			// Accessor for current edge being iterated.
			//
			//**************************
			{
				ptriCurr->Init(pqrContainer->pqnqRoot->mpConversions);

				rv3_a = ptriCurr->v3Get(uCurrEdgeIndex);
				rv3_b = ptriCurr->v3Get(uCurrEdgeIndex == 2 ? 0 : uCurrEdgeIndex + 1);
			}


			//*****************************************************************************************
			//
			ESideOf esfEdgeIntersects() const;
			//
			// Returns:
			//		The intersection flag of the current edge with the original query rectangle.
			//
			//**************************

			//*****************************************************************************************
			//
			// Operators.
			//

			//*****************************************************************************************
			operator bool() const
			{
				return ptriCurr != 0;
			}

			//*****************************************************************************************
			void operator ++();
		};

	private:
		friend class CIterator;
		friend class CEdgeIterator;
		friend class CQuadNodeQuery;

		static CProfileStat psEvaluate;
		static CProfileStat psRefine;
		static CProfileStat psIterate;

		CDArray<CQuadNodeQuery*> dapqnqIterateStore;
		CQuadNodeQuery* pqnqLastNode;	// Cached pointer to last node checked for triangle.
		CTriangleQuery* ptriLastTri;	// Cached pointer to last triangle height query fell within.

		CRectangle<>    rcExtentQuery;	// Extent (in XY plane) of this query rectangle, in quad tree units.
		CRectangle<int> rcExtent;		// Extent rounded out to integer units.
		CRectangle<int> rcRefinedExtent;// Extent of this query rectangle, grown to contain the (maximum refined) quad
										// nodes intersecting the above rectangle.
		TReal rZMax;					// Largest Z value within this rect.

		CCoef cfMaxRegionHighPass;
		bool bFilterAsRatio;

		bool bEvaluateReq;				// 'true' if the contents of this query must be re-evaluated before iteration.

		int iIterateLockCount;			// Number of active iterations on contents.

	public:
		CQuadRootQuery* pqnqRoot;		// The root node of the quad tree this query is associated with.


		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		// Initialiser constructor.
		CQueryRect
		(
			CQuadRootQuery* pqnq_root,
			const CBoundVol& bv,
			const CPresence3<>& pr3,
			TReal r_freq_highpass = 0,
			bool b_filter_as_ratio = false,
			uint u_max_nodes = uDEFAULT_MAX_NODES_IN_QUERY
		);

		~CQueryRect();


		//******************************************************************************************
		//
		// Member functions.
		//

		//*****************************************************************************************
		//
		CIterator itBegin()
		//
		//
		//**************************
		{
			Evaluate();
			return CIterator(this);
		}


		//*****************************************************************************************
		//
		CEdgeIterator eitBegin()
		//
		//
		//**************************
		{
			Evaluate();
			return CEdgeIterator(this);
		}


		//******************************************************************************************
		//
		const CRectangle<int>& rcGetQuadExtent() const
		//
		// Returns:
		//
		//**************************************
		{
			return rcExtent;
		}


		//******************************************************************************************
		//
		void SetVolume
		(
			const CBoundVol& bv,
			const CPresence3<>& pr3,
			TReal r_freq_highpass = 0,	// Lower limit (in meters or as a ratio of a node's size) on
										// wavelet coeficients driving further refinement of the quad
										// tree.
			bool b_filter_as_ratio = false
										// Whether the highpass frequency should be expressed
										// as a ratio of a node's size.
		);
		//
		// Calculate the extents of this query rectangle from the given bounding volume and its
		// presence.
		//
		// Notes:
		//		The resulting rectangular area is clipped against the extents of the
		//		multiresolution data.
		//
		//**************************************


		//******************************************************************************************
		//
		CTriangleQuery* ptriFind
		(
			TReal r_x, TReal r_y				// World location.
		);
		//
		// Returns:
		//		The triangle containing the given point within this query, or 0 if outside.
		//
		//**************************************

		//******************************************************************************************
		//
		TReal rHeight
		(
			TReal r_x, TReal r_y,	// The world location to sample the height at within the current 
									// query volume.
			const CPlane** pppl		// Return pointer to plane at this point.
		);
		//
		// Query topology for the height at the given location.
		//
		// Returns:
		//		The height at the given (x,y) world location, or zero if no (height) data exists at
		//		the given location.
		//
		//**************************************


		//******************************************************************************************
		//
		TReal rHeight
		(
			TReal r_x, TReal r_y	// The world location to sample the height at within the current 
									// query volume.
		)
		//
		// Query topology for the height at the given location.
		//
		// Returns:
		//		The height at the given (x,y) world location, or zero if no (height) data exists at
		//		the given location.
		//
		//**************************************
		{
			const CPlane* ppl;
			return rHeight(r_x, r_y, &ppl);
		}

		//******************************************************************************************
		//
		TReal rMaxHeight() const
		//
		// Returns:
		//		The maximum terrain height in this region.
		//
		//**************************************
		{
			return rZMax;
		}

		//******************************************************************************************
		//
		void SetEvaluateReq()
		//
		//**************************************
		{
			Assert(iIterateLockCount == 0);

			bEvaluateReq = true;
			dapqnqIterateStore.Reset();

			// Reset cached pointers.
			pqnqLastNode = 0;
			ptriLastTri = 0;
		}


		//******************************************************************************************
		//
		void Evaluate();
		//
		// Refine and iterate the query quad tree for this query region, if necessary.
		//
		// Notes:
		//		This function will only evaluate the quad tree if a decimate pass has been done
		//		since the last time it was called. This avoids redundant updates for multiple
		//		queries between quad tree decimate passes.
		//
		//**************************************

	private:
		//******************************************************************************************
		//
		void IterateLock()
		//
		//**************************************
		{
			Assert(iIterateLockCount >= 0);

			iIterateLockCount++;
		}


		//******************************************************************************************
		//
		void IterateUnlock()
		//
		//
		//**************************************
		{
			Assert(iIterateLockCount > 0);

			iIterateLockCount--;
		}
	};
};

#endif
