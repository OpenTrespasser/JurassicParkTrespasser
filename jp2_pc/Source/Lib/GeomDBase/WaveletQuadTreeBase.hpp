/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Template wavelet quad tree base classes.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTreeBase.hpp                                 $
 * 
 * 38    9/23/98 7:41p Mlange
 * Wavelet quad tree nodes now have a single pointer to the first descendant and a pointer to
 * the next sibling, instead of four pointers for each of the descendants.
 * 
 * 37    98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 36    5/05/98 4:31p Mlange
 * Reorganised data members among the quad tree class hierarchy.
 * 
 * 35    4/23/98 7:44p Mlange
 * Can now show terrain textures that are constrained wrt subdivision in stippled blue.
 * 
 * 34    4/16/98 6:26p Mlange
 * Changes to state flags.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_WAVELETQUADTREEBASE_HPP
#define HEADER_LIB_GEOMDBASE_WAVELETQUADTREEBASE_HPP


#include "Lib/Std/Set.hpp"
#include "Lib/GeomDBase/WaveletCoef.hpp"
#include "Lib/GeomDBase/WaveletConv.hpp"


#define dAVG_QUAD_NODE_TO_VERTEX_RATIO	.95		// Empirically derived constant, used only to determine the
												// upper limit of allocated memory buffers sizes.

class CConsoleBuffer;
template<class TD, class TVT> class CQuadRootBaseT;

namespace NMultiResolution
{
	//**********************************************************************************************
	//
	class CQuadVertex
	//
	// Base vertex type for the wavelet quad tree.
	//
	// Prefix: qvt
	//
	// Notes:
	//		This class is the base class for all quad vertex types.
	//
	//**************************************
	{
		uint16 u2X;				// Coordinates of vertex.
		uint16 u2Y;

	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		// Construct one of the corner vertices for the root quad node.
		CQuadVertex(int i_x, int i_y)
			: u2X(i_x), u2Y(i_y)
		{
			// Ensure the given points are within numerical range.
			Assert(u2X == i_x && u2Y == i_y);
		}

		// Construct a new vertex at the midpoint of an edge.
		CQuadVertex(const CQuadVertex* pqvt_a, const CQuadVertex* pqvt_b)
			: u2X((pqvt_a->u2X + pqvt_b->u2X) >> 1), u2Y((pqvt_a->u2Y + pqvt_b->u2Y) >> 1)
		{
			Assert(!(u2X == pqvt_a->u2X && u2Y == pqvt_a->u2Y));
		}


		//******************************************************************************************
		//
		int iX() const
		//
		// Returns:
		//		The X coordinate of this vertex.
		//
		//**************************************
		{
			return u2X;
		}


		//******************************************************************************************
		//
		int iY() const
		//
		// Returns:
		//		The Y coordinate of this vertex.
		//
		//**************************************
		{
			return u2Y;
		}
	};

	
	//**********************************************************************************************
	//
	template<class TD, class TVT> class CQuadNodeBaseT
	//
	// Base class definition of a single node in the wavelet quad tree.
	//
	// Prefix: qnb
	//
	// Notes:
	//		This class is the base class for all quad node types. Note that even though this is a
	//		base class, none of the member functions are declared virtual. This is deliberate, so to
	//		avoid the vtbl size overhead. By passing the derived class' type as a template parameter
	//		and casting the 'this' pointer type to that of the derived class, it is still possible to
	//		overide certain functions in the derived class and get the expected behaviour.
	//
	//		Template type 'TD' defines the type of the derived class. The template type 'TVT'
	//		defines the vertex type.
	//
	//		The corner vertices and descendants of a node are maintained and numbered in a counter-
	//		clockwise order, starting with the bottom left corner. Similarly, the eight neighbours of
	//		a node are numbered in a counter-clockwise order, starting at the neighbouring node
	//		directly below.
	//
	//		Vertex 0 and descendant 0 are referred to as the 'base vertex' and 'base descendant' of
	//		a quad node, respectively. The odd numbered neighbours are referred to as the 'diagnonal'
	//		neigbours and the even numbered neighbours are referred to as the 'transverse' neighbours.
	//
	//		Within each quad node, there are three wavelet coefficients, located at the base vertex of
	//		descendants 1, 2 and 3. Therefore, a quad node can only have wavelet coeficients if it
	//		also has descendants.
	//										+-----+-----+-----+
	//			vt3           vt2			|     |     |     |
	//			  +-----+-----+				|nghbr|nghbr|nghbr|
	//			  |     |     |				|  5  |  4  |  3  |
	//			  | dsc | dsc |				+-----+-----+-----+
	//			  |  3  |  2  |				|     |     |     |
	//			 wt2---wt1----+				|nghbr|     |nghbr|
	//			  |     |     |				|  6  |     |  2  |
	//			  | dsc | dsc |				+-----+-----+-----+
	//			  |  0  |  1  |				|     |     |     |
	//			  +----wt0----+				|nghbr|nghbr|nghbr|
	//			vt0           vt1			|  7  |  0  |  1  |
	//										+-----+-----+-----+
	//
	//**************************************
	{
	public:
		enum EState
		// Prefix: est
		// The status bits for a node. Used with the TState type.
		{
			estBOUNDARY_MIN_Y,			// Set of flags that indicate that this node's extent coincides with the
			estBOUNDARY_MAX_X,			// boundary of thequad tree.
			estBOUNDARY_MAX_Y,
			estBOUNDARY_MIN_X,

			estHAS_NEIGHBOUR_0,			// Set of flags that indicate whether this node has a neighbour. Note
			estHAS_NEIGHBOUR_2,			// that only four of the eight possible neighbours have a status bit.
			estHAS_NEIGHBOUR_4,
			estHAS_NEIGHBOUR_6,

			estDISC_DIAGONAL_1_3,		// Defines the 0-disc wavelet topology relative to this node. If set, it
										// indicates that wavelet 1 is calculated from a combination of vertices 1
										// and. If clear, it is calculated from vertices 0 and 2.

			estLEAF,					// Indicates that this is a leaf node, e.g. a node with no descendants.
			estLEAF_COMBINE,			// Indicates that this is a 'leaf combine' node, e.g. a node whose
										// descendants are leaf nodes.

			estSIGNIFICANT,				// Node significance flag. Used by the derived classes.
			estWAVELET_0_SIGNIFICANT,	// Wavelet coeficient significance flag. Used by the derived classes.
			estWAVELET_1_SIGNIFICANT,
			estWAVELET_2_SIGNIFICANT,

			estTEXTURE_SUBDIVIDE,		// Flag indicating node must be subdivided for texturing. Used by the derived classes.
			estDIFF_SIG_LEVEL_EXCEEDED,	// Determines if the subdivision level of this node is such that it cannot
										// contain a texture.
			estTEX_SUBDIV_CONSTRAINED	// Flag indicating node could not be subdivided for texturing. Used by the derived classes.
		};

		// Prefix: st
		typedef CSet<EState> TState;

	public:
		TState stState;				// The current state of this node.

	private:
		friend class CQuadRootBaseT<TD, TVT>;

		TD* ptqnParent;				// Parent to this node, or null if this node is the root node of the tree.

		TD* ptqnDescendant;			// First Descendant of this node. Each node has either none or all four of its
									// descendants constructed. If there are no descendants, this pointer is null.

		TD* ptqnSibling;			// Next sibling, or null if none. Siblings are maintained in a circular linked
									// list.

		CSCArray<TVT*, 4> csapqvtVertices;
									// The corner vertices of this node. Note that vertices are shared between
									// neighbouring nodes.

		//******************************************************************************************
		//
		// Constructors and destructor.
		//

	protected:
		CQuadNodeBaseT()
			: ptqnParent(0), ptqnDescendant(0), ptqnSibling(0), csapqvtVertices(0)
		{
		}


		// Construct a new descendant.
		CQuadNodeBaseT
		(
			const TD* ptqn_parent,	// The parent of the new descendant.
			int i_relation,			// Relation of this node to its parent.
			TD*  aptqn_subdiv_neighbours[4],
									// Array of pointers to the four neighbours of this node that are subdivided, or
									// null if that neighbour does not exist or is not subdivided.
			TVT* apqvt_dsc[5]		// Pointers to the four vertices along the mid point of the parent node's edges
									// and a pointer to the vertex at the centre of the parent node.
		);


		// Destructor.
		~CQuadNodeBaseT()
		{
			Assert(!bHasDescendants());
		}


		//******************************************************************************************
		//
		// Member functions.
		//

	public:
		//******************************************************************************************
		//
		TState stGetState() const
		//
		// Returns:
		//		The current state of this node.
		//
		//**************************************
		{
			return stState;
		}


		//******************************************************************************************
		//
		int iGetSize() const
		//
		// Returns:
		//		The size of this node, in quad tree units.
		//
		//**************************************
		{
			// Make sure this node has a valid (non-zero) size.
			Assert(pqvtGetVertex(1)->iX() - iBaseX() > 0);

			return pqvtGetVertex(1)->iX() - iBaseX();
		}


		//******************************************************************************************
		//
		TD* ptqnGetParent() const
		//
		// Returns:
		//		The parent of this node, or the null pointer if this is the root node.
		//
		//**************************************
		{
			return ptqnParent;
		}


		//******************************************************************************************
		//
		bool bHasDescendants() const
		//
		//	Returns:
		//		'true' if this node has any descendants, 'false' otherwise.
		//
		//**************************************
		{
			// Make sure state flag matches.
			Assert((ptqnDescendant == 0) == (stState[estLEAF] != 0));

			return ptqnDescendant != 0;
		}


		//******************************************************************************************
		//
		TD* ptqnGetFirstDescendant() const
		//
		// Returns:
		//		A pointer to the first descendant, or null if none exists.
		//
		//**************************************
		{
			return ptqnDescendant;
		}


		//******************************************************************************************
		//
		TD* ptqnGetSibling() const
		//
		// Returns:
		//		A pointer to the next sibling, or null if none exists.
		//
		//**************************************
		{
			return ptqnSibling;
		}


		//******************************************************************************************
		//
		void SetSibling
		(
			TD* ptqn
		)
		//
		// Set the link to the next sibling.
		//
		//**************************************
		{
			Assert(ptqnSibling == 0 && ptqn != 0);
			ptqnSibling = ptqn;
		}


		//******************************************************************************************
		//
		TD* ptqnGetDescendant
		(
			int i_dsc_num	// The index of the requested descendant.
		) const
		//
		// Returns:
		//		A pointer to the descendant associated with the given index.
		//
		// Notes:
		//		To enable convienent iteration, the given index is taken mod 4.
		//
		//**************************************
		{
			Assert(ptqnDescendant != 0);

			TD* ptqn_ret = ptqnDescendant;

			for (i_dsc_num &= 3; i_dsc_num != 0; --i_dsc_num)
			{
				ptqn_ret = ptqn_ret->ptqnGetSibling();
				Assert(ptqn_ret != 0);
			}

			return ptqn_ret;
		}


		//******************************************************************************************
		//
		TD* ptqnGetNeighbour
		(
			int i_n		// The number of the requested neighbour (see class header).
		) const
		//
		// Returns:
		//		The requested neighbour, or null if none exists.
		//
		// Notes:
		//		To enable convienent iteration, the requested neighbour is taken mod 8.
		//
		//**************************************
		{
			if ((i_n & 1) == 0 && stState[estGetNeighbourBit(i_n)] == 0)
				return 0;

			TD* ptqn_neighbour = ptqnGetNearestNeighbour(i_n);

			if (ptqn_neighbour != 0 && ptqn_neighbour->iGetSize() == iGetSize())
				return ptqn_neighbour;
			else
				return 0;
		}


		//******************************************************************************************
		//
		TVT* pqvtGetVertex
		(
			int i_vertex_num	// The index of the requested vertex.
		) const
		//
		// Returns:
		//		A pointer to the vertex associated with the given index.
		//
		// Notes:
		//		To enable convienent iteration, the given index is taken mod 4.
		//
		//**************************************
		{
			Assert(csapqvtVertices[i_vertex_num] != 0);

			return csapqvtVertices[i_vertex_num];
		}


		//******************************************************************************************
		//
		int iBaseX() const
		//
		// Returns:
		//		The x coordinate (in quad tree units) of the base vertex of this node.
		//
		//**************************************
		{
			return pqvtGetVertex(0)->iX();
		}


		//******************************************************************************************
		//
		int iBaseY() const
		//
		// Returns:
		//		The y coordinate (in quad tree units) of the base vertex of this node.
		//
		//**************************************
		{
			return pqvtGetVertex(0)->iY();
		}


		//******************************************************************************************
		//
		CRectangle<> rcGetRectangle() const
		//
		// Returns:
		//		The real rectangle, in quad space, defining this node.
		//
		//******************************
		{
			return CRectangle<>(iBaseX(), iBaseY(), iGetSize(), iGetSize());
		}


		//******************************************************************************************
		//
		bool bContains
		(
			int i_x, int i_y	// The x/y coordinates of the point to test, in quad tree units.
		) const
		//
		// Returns:
		//		'true' if the given point is contained in this node (considering the extents of this
		//		node in the x/y plane only), 'false' otherwise.
		//
		// Notes:
		//		A point is considered to be contained by a quad node if it lies within the range
		//		defined by (and including) the base coordinates up to, but not including, its extents.
		//
		//**************************************
		{
			return i_x >= iBaseX() && i_x < pqvtGetVertex(2)->iX() &&
				   i_y >= iBaseY() && i_y < pqvtGetVertex(2)->iY();
		}


		//******************************************************************************************
		//
		bool bContains
		(
			const CRectangle<>& rc
		) const
		//
		// Returns:
		//		'true' if the given rectangle is contained in this node (considering the extents of this
		//		node in the x/y plane only), 'false' otherwise.
		//
		//**************************************
		{
			return rc.tX0() >= iBaseX() && rc.tX1() <= pqvtGetVertex(2)->iX() &&
				   rc.tY0() >= iBaseY() && rc.tY1() <= pqvtGetVertex(2)->iY();
		}


		//******************************************************************************************
		//
		TD* ptqnGetNearestNeighbour
		(
			int i_n		// The number of the requested neighbour (see class header).
		) const;
		//
		// Returns:
		//		The nearest neighbour that exists, or null if a neighbour outside the bounds of the
		//		quad tree was requested..
		//
		// Notes:
		//		The nearest neighbour is the smallest node in tree that has a greater or equal size
		//		and adjoins this node.
		//
		//		To enable convienent iteration, the requested neighbour is taken mod 8.
		//
		//**************************************


	protected:
		//******************************************************************************************
		//
		void AccumulateWavelets
		(
			CCoef acf_wvlt[3],		// The values of the wavelet coeficients.
			CCoef acf_corner[4]		// Array to hold accumulated wavelets.
		) const;
		//
		// For each corner vertex of this node, accumulate the wavelet coeficients from this node
		// that influence it.
		//
		//**************************************


		//******************************************************************************************
		//
		void AllocateDscVertices
		(
			TVT* apqvt_dsc[5],				// Array to to hold the returned vertices.
			TD*  aptqn_subdiv_neighbours[4]	// Array of pointers to the four neighbours of this
											// node that are subdivided, or null if that neighbour
											// does not exist or is not subdivided.
		) const;
		//
		// Find or allocate the vertices required to subdivide this node.
		//
		// Notes:
		//		Before a node can be subdivided we must obtain the five vertices required for the
		//		corners of the new descendants. This function will either find those vertices from
		//		existing (neighbouring) nodes or allocate them if they do not yet exist.
		//
		//**************************************



		//******************************************************************************************
		//
		void InitSubdivide
		(
			TD* ptqn_dsc_0, TD* ptqn_dsc_1,
			TD* ptqn_dsc_2, TD* ptqn_dsc_3
		);
		//
		// Subdivide this node, e.g. add the given descendant nodes.
		//
		// Notes:
		//		The estLEAF and estLEAF_COMBINE flags for the affected nodes are updated.
		//
		//**************************************



		//******************************************************************************************
		//
		void Decimate();
		//
		// Decimate this node, e.g. delete its descendant nodes and any resulting orphaned vertices.
		//
		// Notes:
		//		This must be a leaf combine node, otherwise use the decimate branch function below.
		//		The estLEAF and estLEAF_COMBINE flags for the affected nodes are updated.
		//
		//		The descendant nodes are deleted before any vertices so that the corner vertices of a
		//		quad node are still valid in the quad node destructor,
		//
		//		This function can be overiden by the derived class.
		//
		//**************************************


	public:
		//******************************************************************************************
		//
		void DecimateBranch()
		//
		// Decimate this branch of the quad tree.
		//
		// Notes:
		//		This function can be overiden by the derived class.
		//
		//**************************************
		{
			if (!bHasDescendants())
				return;

			TD* ptqn_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				ptqn_dsc->DecimateBranch();

				ptqn_dsc = ptqn_dsc->ptqnGetSibling();
			}

			static_cast<TD*>(this)->Decimate();
		}

	private:
		//******************************************************************************************
		//
		static EState estGetNeighbourBit
		(
			int i_n		// The number of the requested transverse neighbour.
		)
		//
		// Returns:
		//		The bit in the status bitset that indicates the existance (e.g. whether that node
		//		has been added to the quad tree) of that neighbour.
		//
		// Notes:
		//		This function can only be used to determine the existance of a transverse neighbour.
		//		Note also that this refers to the existance of the actual neighbour, e.g. excluding
		//		any 'nearest' neigbours.
		//
		//		To enable convienent iteration, the requested neighbour is taken mod 8.
		//
		//**************************************
		{
			// The status bitset only records the existance of a transverse neighbour.
			Assert((i_n & 1) == 0);

			// Ensure the enumerated constants match the index calculation.
			Assert(int(estHAS_NEIGHBOUR_2) - int(estHAS_NEIGHBOUR_0) == 1 &&
			       int(estHAS_NEIGHBOUR_4) - int(estHAS_NEIGHBOUR_0) == 2 && int(estHAS_NEIGHBOUR_6) - int(estHAS_NEIGHBOUR_0) == 3);

			return EState( ((i_n & 7) >> 1) + estHAS_NEIGHBOUR_0 );
		}
	};




	//**********************************************************************************************
	//
	template<class TD, class TVT> class CQuadRootBaseT : public TD
	//
	// Base class definition of the root node in the wavelet quad tree.
	//
	// Prefix: qnr
	//
	// Notes:
	//		The size of the root node defines the extents of the quad tree and therefore the
	//		extents of the wavelet transformed data set.
	//
	//		Currently, this also means that the wavelet transformed data set MUST be square and its
	//		size must be an integer power of two.
	//
	//**************************************
	{

		//******************************************************************************************
		//
		// Constructors and destructor.
		//
	protected:
		CQuadRootBaseT(TVT* pqvt_0, TVT* pqvt_1, TVT* pqvt_2, TVT* pqvt_3);

		~CQuadRootBaseT();

	public:
		//******************************************************************************************
		//
		TD* ptqnFindLeaf
		(
			int i_x, int i_y	// The x/y coordinates of a point in quad tree units that is
								// used to locate the leaf node.
		) const;
		//
		// Searches down the tree until the leaf node that contains the given point is found.
		//
		// Returns:
		//		The leaf node that contains the given point, or null if the tree does not contain
		//		the point.
		//
		//**************************************


		//******************************************************************************************
		//
		void PrintStats
		(
			CConsoleBuffer& con
		) const;
		//
		//
		// Print the stats to the given console.
		//
		// Notes:
		//		This function can be overiden by the derived class.
		//		This function does not clear the console before printing.
		//
		//**************************************


		//******************************************************************************************
		//
		// Overloaded operators.
		//

		//******************************************************************************************
		void* operator new(size_t i_size)
		{
			return ::operator new(i_size);
		}

		//******************************************************************************************
		void operator delete(void* pv)
		{
			::delete pv;
		}
	};
};


#endif
