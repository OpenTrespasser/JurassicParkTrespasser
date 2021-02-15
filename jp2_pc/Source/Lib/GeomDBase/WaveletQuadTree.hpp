/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		The TIN wavelet quad tree types.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTree.hpp                                     $
 * 
 * 103   10/01/98 8:28p Mlange
 * Optimised move message processing.
 * 
 * 102   98/09/15 0:44 Speter
 * Added count functions to PolyIterator, because only it knows how many polygons there
 * currently are.
 * 
 * 101   98/09/14 1:41 Speter
 * Added iNumVertexPointers(). iNumVertices() now returns actual #vertices, not 0.
 * bSharedVertices() is checked for that purpose.
 * 
 * 100   9/11/98 7:10p Mlange
 * Now uses quad node Z extents in texture node placement calculation. Fixed bug in wavelet
 * coeficient projection calculation - a squared term was missing.
 * 
 * 99    8/16/98 8:37p Mmouni
 * Fixed problem with returning a reference to a temporary.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_WAVELETQUADTREE_HPP
#define HEADER_LIB_GEOMDBASE_WAVELETQUADTREE_HPP

#include "Lib/Std/RangeVar.hpp"
#include "Lib/Std/BlockAllocator.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeBaseTri.hpp"
#include "Lib/GeomDBase/Shape.hpp"

//
// Forward declarations.
//
class CPartition;
class CCamera;
class CLightList;
class CConsoleBuffer;
class CFastHeap;
class CDraw;
class CMessageMove;
class CShapePresence;

namespace NMultiResolution
{
	//**********************************************************************************************
	//
	// Declarations for CQuadVertexTIN.
	//
	class CQuadNodeTIN;
	class CQuadRootTIN;

	//**********************************************************************************************
	//
	class CQuadVertexTIN : public CQuadVertexRecalc,
	                       public CBlockAllocator<CQuadVertexTIN>
	//
	// Vertex type for the TIN wavelet quad tree node.
	//
	// Prefix: qvt
	//
	//**************************************
	{
		friend class CBlockAllocator<CQuadVertexTIN>;
		static CBlockAllocator<CQuadVertexTIN>::SStore stStore; // Storage for the types.

		uint16 u2Handle;	// Storage for the block allocator handle.

		int16 i2LightVal;	// Lighting value, stored as an 8.8 fixed point number.

		//******************************************************************************************
		//
		// Constructors and destructor.
		//
	public:
		CQuadVertexTIN(int i_x, int i_y, int i_root_node_size, CCoef cf_root_scaling)
			: CQuadVertexRecalc(i_x, i_y, i_root_node_size, cf_root_scaling), i2LightVal(-1)
		{
		}


		CQuadVertexTIN(const CQuadVertexTIN* pqvt_a, const CQuadVertexTIN* pqvt_b)
			: CQuadVertexRecalc(pqvt_a, pqvt_b), i2LightVal(-1)
		{
		}


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		TClutVal cvGetLighting
		(
			const CQuadNodeTIN* pqntin,
			CLightList& ltl_lighting,
			const CClut& clut,
			const CQuadRootTIN* pqntin_root
		);
		//
		// Returns:
		//		The lighting value at this vertex.
		//
		//**************************************


		//******************************************************************************************
		//
		void InvalidateLighting()
		//
		// Invalidate the cached lighting value of this vertex.
		//
		//**************************************
		{
			i2LightVal = -1;
		}


		//******************************************************************************************
		//
		// Overides.
		//

		//******************************************************************************************
		int iGetHandle() const
		{
			return u2Handle;
		}

		//******************************************************************************************
		void SetHandle(int i_handle)
		{
			u2Handle = i_handle;

			// Ensure index is within numerical range.
			Assert(u2Handle == i_handle);
		}
	};



	//**********************************************************************************************
	//
	// Forward declarations for CTriangleTIN.
	//
	class CQuadNodeTIN;

	//**********************************************************************************************
	//
	class CTriangleTIN : public CTriangleT<CTriangleTIN, CQuadVertexTIN>,
	                     public CBlockAllocator<CTriangleTIN>
	//
	// Triangle type for the TIN wavelet quad tree.
	//
	// Prefix: tri
	//
	//**************************************
	{
		friend class CBlockAllocator<CTriangleTIN>;
		static CBlockAllocator<CTriangleTIN>::SStore stStore; // Storage for the types.

		CPlane plPlane;					// The plane through this triangle, in quad space. A negative Z component of
										// the normal indicates this plane is invalid and must be recalculated.

		CQuadNodeTIN* pqntinTexNode;	// The quad node containing the texture for this triangle, null if none assigned yet.

		//******************************************************************************************
		//
		// Constructors and destructor.
		//

	public:
		CTriangleTIN()
		{
		}


		CTriangleTIN
		(
			CQuadVertexTIN* pqvt_a, CQuadVertexTIN* pqvt_b, CQuadVertexTIN* pqvt_c,
			CTriangleTIN* pptri_link_after
		)
			: CTriangleT<CTriangleTIN, CQuadVertexTIN>(pqvt_a, pqvt_b, pqvt_c, pptri_link_after),
			  pqntinTexNode(0)
		{
			plPlane.d3Normal.tZ = -1;
		}


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		const CPlane& plGetPlaneQuad() const
		//
		// Returns:
		//		The plane through this triangle, in quad tree space.
		//
		//**************************************
		{
			Assert(bHasValidPlane());

			return plPlane;
		}


		//******************************************************************************************
		//
		bool bHasValidPlane() const
		//
		// Returns:
		//		'true' if the plane through this triangle has been calculated.
		//
		//**************************************
		{
			return !CIntFloat(plPlane.d3Normal.tZ).bSign();
		}


		//******************************************************************************************
		//
		void CalcPlane(const SMapping& mp);
		//
		// If the plane through this triangle is invalid, calculate it.
		//
		//**************************************



		//******************************************************************************************
		//
		CQuadNodeTIN* pqntinGetTextureNode() const
		//
		// Returns:
		//		The texture node associated with this triangle, or null if none.
		//
		//**************************************
		{
			return pqntinTexNode;
		}


		//******************************************************************************************
		//
		void SetTextureReference
		(
			CQuadNodeTIN* pqntin = 0	// Default parameter invalidates current texture reference.
		)
		//
		// Set the texture referenced from this triangle.
		//
		//**************************************
		{
			pqntinTexNode = pqntin;
		}


		//******************************************************************************************
		//
		// Overides.
		//

		//******************************************************************************************
		void UpdatedVertices()
		{
			plPlane.d3Normal.tZ = -1;
			apqvtVertices[0]->InvalidateLighting();
			apqvtVertices[1]->InvalidateLighting();
			apqvtVertices[2]->InvalidateLighting();
		}

	};



	//**********************************************************************************************
	//
	class CTriNodeInfoTIN : public CTriNodeInfoT<CTriangleTIN, CQuadVertexTIN>,
	                        public CBlockAllocator<CTriNodeInfoTIN>
	//
	// Triangulated node info type, owned and managed by a TIN wavelet quad tree node type.
	//
	// Prefix: tinf
	//
	//**************************************
	{
		friend class CBlockAllocator<CTriNodeInfoTIN>;
		static CBlockAllocator<CTriNodeInfoTIN>::SStore stStore; // Storage for the types.

		int16 bValid;			// True if the triangles exist, have valid planes and valid texture references.

	public:
		int16 iDeferEvalCount;	// Deferred evaluation counter. Used by owning quad node.


		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		CTriNodeInfoTIN()
			: iDeferEvalCount(0), bValid(false)
		{
		}


		//******************************************************************************************
		//
		bool bIsValid() const
		//
		// Returns:
		//		'true' if this triangle node info type is valid i.e. it has been triangulated,
		//		the plane equation of each of the triangles has been calculated and each triangle
		//		has valid texture references.
		//
		//******************************
		{
			return bValid;
		}


		//******************************************************************************************
		//
		void Validate
		(
			const CQuadRootTIN* pqntin_root,	// Root node of tree.
			CQuadNodeTIN* pqntin_owner			// Owning quad node.
		);
		//
		// Validate this triangle node info type i.e. triangulate it if necessary and make sure the
		// the plane equation of each of the triangles has been calculated and each triangle has
		// a valid texture reference.
		//
		//******************************


		//******************************************************************************************
		//
		void InvalidateTextureReferences();
		//
		// Invalidate the texture node references of the triangles, if any.
		//
		//******************************


		//******************************************************************************************
		//
		// Overides.
		//

		//******************************************************************************************
		void ModifiedVertices()
		{
			CTriNodeInfoT<CTriangleTIN, CQuadVertexTIN>::ModifiedVertices();

			bValid = false;
		}
	};



	//**********************************************************************************************
	//
	// Forward declarations for CQuadNodeTIN.
	//
	class CTextureNode;

	//**********************************************************************************************
	//
	class CQuadNodeTIN : public CQuadNodeBaseTriT<CQuadNodeTIN, CQuadVertexTIN, CTriNodeInfoTIN, CTriangleTIN>,
	                     public CBlockAllocator<CQuadNodeTIN>
	//
	// Definition of a single node in the wavelet TIN quad tree.
	//
	// Prefix: qntin
	//
	//**************************************
	{
		friend class CBlockAllocator<CQuadNodeTIN>;
		static CBlockAllocator<CQuadNodeTIN>::SStore stStore; // Storage for the types.

		class CPriv;
		friend class CPriv;
		friend class CQuadRootTIN;

		static CProfileStat psEvaluate;
		static CProfileStat psProject;
		static CProfileStat psEvalWavelets;
		static CProfileStat psRefine;
		static CProfileStat psDecimate;

		CTextureNode* ptxnTexture;	// Structure used by terrain texturing system.

		CCoef cfMin;				// Minimum and maximum scaling coeficient values in this branch of the quad tree,
		CCoef cfMax;				// or cfNAN if currently invalid.


		//******************************************************************************************
		//
		// Constructors and destructor.
		//
	public:
		CQuadNodeTIN()
			: ptxnTexture(0), cfMin(cfNAN), cfMax(cfNAN)
		{
		}

		// Construct a new descendant.
		CQuadNodeTIN
		(
			const CQuadRootBaseRecalcT<CQuadNodeTIN, CQuadVertexTIN>* pqnr_root, const CQuadNodeTIN* ptqn_parent,
			int i_relation, CQuadNodeTIN* aptqn_subdiv_neighbours[4], CQuadVertexTIN* apqvt_dsc[5]
		)
			: CQuadNodeBaseTriT<CQuadNodeTIN, CQuadVertexTIN, CTriNodeInfoTIN, CTriangleTIN>(pqnr_root, ptqn_parent, i_relation, aptqn_subdiv_neighbours, apqvt_dsc),
			  ptxnTexture(0), cfMin(cfNAN), cfMax(cfNAN)
		{
		}

		~CQuadNodeTIN();


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		const CTextureNode* ptxnGetTextureNode() const
		//
		// Returns:
		//		The texture node associated with this node, or null if none.
		//
		//**************************************
		{
			return ptxnTexture;
		}


		//******************************************************************************************
		//
		void EvaluateTexture
		(
			const CQuadRootTIN* pqntin_root
		);
		//
		// Evaluate the texture that this node contains.
		//
		//**************************************


		//******************************************************************************************
		//
		void GenerateLighting
		(
			rptr<CRaster> pras_dest,		// Composite 16-bit raster to light.
			CLightList& ltl_lighting,		// Lighting context for current scene.
			const CClut& clut,				// Clut for raster output.
			const CQuadRootTIN* pqntin_root	// Owning context.
		) const;
		//
		// Fill the lighting values (high byte) of the composite raster.
		// This overload is the public entry point; it calls the recursive version below.
		//
		//******************************



	protected:
		//******************************************************************************************
		//
		void ResetDeferredEvalBranch();
		//
		// Reset the deferred evaluation counts in this branch of the quad tree.
		//
		//**************************************


		//******************************************************************************************
		//
		int iEvaluateBranch
		(
			const CQuadRootTIN* pqntin_root
		);
		//
		// Evaluate this branch of the quad tree.
		//
		// Returns:
		//		The smallest size (in quad tree units) of all the nodes in this branch of the quad
		//		tree that are significant.
		//
		// Notes:
		//		Updates (e.g. refines and decimates) the geometry of the wavelet quad tree datat
		//		where necessary, driven by the position of the observer in world space, the
		//		various quality settings and the requirements of the texturing.
		//
		//**************************************


		//******************************************************************************************
		//
		void EvaluateTextureBranch
		(
			const CQuadRootTIN* pqntin_root
		);
		//
		// Refine this branch of the quad tree for texturing.
		//
		// Notes:
		//		Must be called AFTER iEvaluateBranch().
		//
		//******************************


		//******************************************************************************************
		//
		void AssignTexturesBranch
		(
			const CQuadRootTIN* pqntin_root,
			CTextureNode* ptxn_ancestor = 0
		);
		//
		// Assign textures in this branch of the quad tree.
		//
		// Notes:
		//		Must be called AFTER EvaluateTextureBranch().
		//
		//******************************


		//******************************************************************************************
		//
		void PurgeTexturesBranch
		(
			const CRectangle<>& rc_region	// Quad-space rectangle denoting region to update.
		);
		//
		// Invalidates (forces re-rendering) of any textures at or below this node, that intersect
		// the given region.
		//
		//**************************************


		//******************************************************************************************
		//
		void MovingObjShadowsBranch
		(
			const CRectangle<int>& rc_region_int,	// Quad-space rectangle denoting region to update.
			const CRectangle<>& rc_region,
			const CMessageMove& msgmv
		);
		//
		// Evaluates any textures for moving shadows at or below this node, that intersect
		// the given region.
		//
		//**************************************


		//******************************************************************************************
		//
		void CheckTexturesBranch() const;
		//
		// Checks the validity of the quad tree wrt the textured nodes.
		//
		// Notes:
		//		Does nothing in a non-debug build.
		//
		//******************************


		//******************************************************************************************
		//
		// Definitions for InitTriangleListBranch().
		//
		struct SIterateContext;

		//******************************************************************************************
		//
		void InitTriangleListBranch
		(
			const SIterateContext& itc,
			CClipRegion2D::CClipInfo cli_vis = CClipRegion2D::CClipInfo()
		);
		//
		// Initialise the given array with the triangles intersecting the given clipping region.
		//
		//**************************************


		//******************************************************************************************
		//
		// Overides.
		//

	public:
		//******************************************************************************************
		void ModifiedVerticesBranch();

		//******************************************************************************************
		void DrawQuadNode(CDraw& draw) const;

		//******************************************************************************************
		void SetColour(CDraw& draw, const CTriangleTIN* pttri) const;
	};




	//**********************************************************************************************
	//
	// Declarations for CQuadRootTIN.
	//
	#define uDEFAULT_ALLOC_TIN_NODES	15000u

	struct SEvalInfo;

	//**********************************************************************************************
	//
	class CQuadRootTIN : public CQuadRootBaseTriT<CQuadNodeTIN, CQuadVertexTIN, CTriNodeInfoTIN, CTriangleTIN>
	//
	//
	// Prefix:
	//
	//**************************************
	{
	public:
		static CRangeVar<float> rvarPixelTolerance;
		static CRangeVar<float> rvarPixelToleranceFar;
		static CRangeVar<float> rvarDeferMultiplier;

		static CRangeVar<float> rvarMaxDistRatio;
		static CRangeVar<int>   rvarMinDistRatioNodeSize;
		static CRangeVar<int>   rvarSigNodeDiffShift;

		static CRangeVar<float> rvarTexelScale;
		static CRangeVar<float> rvarTexelScaleFar;
		static CRangeVar<float> rvarTextureTolerance;
		static CRangeVar<float> rvarDisableShadowDistance;
		static CRangeVar<float> rvarDisableDynamicTextureDistance;
		static CRangeVar<float> rvarDisableTextureDistance;

		static bool bFreeze;					// 'true' if terrain should be frozen, e.g. no updates occour.
		static bool bConform;
		static bool bTexelProjNearest;

	private:
		static CProfileStat psUpdate;
		static CProfileStat psEvaluateBranch;
		static CProfileStat psTextureBranch;

		friend class CQuadNodeTIN;
		friend class CQuadNodeTIN::CPriv;

		SEvalInfo* peiInfo;

		int iLastQualitySetting;

	public:
		SMapping mpConversions;					// Conversions between quad - world space.


		//******************************************************************************************
		//
		// Constructors and destructor.
		//
		CQuadRootTIN
		(
			const CTransformedDataHeader* ptdh	// Wavelet transformed data.
		);

		~CQuadRootTIN();


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		static void AllocMemory
		(
			uint u_max_nodes = uDEFAULT_ALLOC_TIN_NODES
		);
		//
		// Allocate fastheaps used to store wavelet quad tree TIN types.
		//
		// Notes:
		//		Must be called *before* any instances of this class are created.
		//
		//**************************************


		//******************************************************************************************
		//
		static void FreeMemory();
		//
		// Free fastheaps used to store wavelet quad tree TIN types.
		//
		// Notes:
		//		Must be called *after* all instances of this class are destructed.
		//
		//**************************************


		//******************************************************************************************
		//
		void Update
		(
			const CCamera* pcam_view,	// Camera specifying the observer.
			bool b_force_eval = false	// If true, forces re-evaluation of the terrain geometry,
										// ignoring any deferred evaluation counts.
		);
		//
		// Update the wavelet quad tree for geometry and textures.
		//
		//**************************************


		//******************************************************************************************
		//
		void UpdateMovingObjShadows
		(
			const CMessageMove& msgmv
		);
		//
		// Updates the terrain texture moving shadows.
		//
		//**************************************


		//******************************************************************************************
		//
		void UpdateTextures
		(
			const CPartition* ppart_region,	// Region to invalidate.
			const CPlacement3<>& p3_prev	// Previous presence of the region.
		);
		//
		// Invalidates the textures that intersect the region at its current or previous location.
		//
		//**************************************


	
		//******************************************************************************************
		//
		TReal rGetWorldZ
		(
			TReal r_quad_x, TReal r_quad_y		// Quad-space coordinates to query at.
		) const;
		//
		// Returns:
		//		The Z value of the triangulation (in world space) at the quad point specified.
		//		Uses current quad-tree refinement, does not refine further.
		//		Returns 0 if the point is not contained in this branch of the tree.
		//
		//******************************


		//******************************************************************************************
		//
		CDir3<> d3GetNormal
		(
			const CQuadVertexTIN* pqvt,		// Vertex at which to find normal.
			const CQuadNodeTIN* ptqn = 0	// Optionally, the node owning this vertex.
											// If 0, will search for it.
		) const;
		//
		// Returns:
		//		The surface normal at vertex pqvt.
		//
		//******************************


	protected:
		//******************************************************************************************
		//
		void InitTriangleList
		(
			CDArray<CTriangleTIN*>& daptri,	// Array of triangle pointers to initialise.
			const CClipRegion2D& clip2d,	// Camera clipping region.
			const CVector3<>& v3_pos,		// Position of camera in object space.
			TReal r_pl_thickness			// Thickness of plane through a poly for purposes of
											// backface culling.
		);
		//
		// Initialise the given array with the triangles intersecting the given clipping region.
		//
		//**************************************


		//******************************************************************************************
		//
		// Overides.
		//

	public:
		//******************************************************************************************
		void PrintStats(CConsoleBuffer& con) const;
	};



	//**********************************************************************************************
	//
	class CQuadRootTINShape : public CShape, public CQuadRootTIN
	//
	// Prefix:
	//
	// Extends CQuadRootTIN by providing a CShape implementation, to render the terrain.
	//
	//**************************************
	{
		static CProfileStat psEvaluateTexture;

	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		CQuadRootTINShape
		(
			const CTransformedDataHeader* ptdh	// Wavelet transformed data.
		);

		~CQuadRootTINShape();

		// Resolve which version of delete we inherit.
		void operator delete(void* pv)
		{
			CQuadRootTIN::operator delete(pv);
		}

		//******************************************************************************************
		//
		// Overrides.
		//

		//******************************************************************************************
		virtual void Render
		(
			const CInstance*		  pins,				// Instance owning this shape.
			CRenderContext&		      renc,				// The rendering context.
			CShapePresence&           rsp,				// The shape-to-world transform.
			const CTransform3<>&      tf3_shape_camera,	// The shape to camera transform.
			const CPArray<COcclude*>& papoc,			// Array of occluding objects.
			ESideOf				      esf_view			// Shape's relation to the view volume
														// (for trivial acceptance).
		) const override;
		//
		// Defined in Pipeline.cpp.
		//

		virtual const CBoundVol& bvGet() const override
		{
			//
			// The terrain basically encompasses the entire world, so just return infinity.
			// Returning a CBoundVolBox wouldn't be accurate, because those boxes are now
			// centred on the origin, and the terrain isn't.  For an accurate volume, use
			// CTerrain.GetWorldExtents().
			//
			static CBoundVolInfinite bvi;
			return bvi;
		}

		virtual rptr<CRenderType> prdtCopy() override
		{
			Assert(false);
			return rptr0;
		}

		int iNumPolygons() const override
		{
			// Return an upper bound on the triangle count.
			return iNumPoints() * 2;
		}

		int iNumTriangles() const override
		{
			return iNumPolygons();
		}

		int iNumPoints() const override
		{
			return CQuadVertexTIN::uMaxAlloc();
		}

		int iNumVertices() const override
		{
			// We do not share vertices, so there are 3 per polygon.
			return iNumPolygons() * 3;
		}

		int iNumVertexPointers() const
		{
			// There are always 3 vertices per polygon.
			return iNumPolygons() * 3;
		}

		virtual TReal rPolyPlaneThickness() const override;


		//******************************************************************************************
		bool bBumpmap() const
		{
			return false;
		}

		//******************************************************************************************
		bool bSharedVertices() const
		{
			// We cannot share vertices, because triangles do not in general share textures.
			return false;
		}

		//******************************************************************************************
		bool bIsCacheable() const
		{
			return false;
		}
		
		//******************************************************************************************
		bool bTerrain() const
		{
			return true;
		}
		

		//******************************************************************************************
		//
		class CPolyIterator: public CShape::CPolyIterator
		//
		// Implementation of CShape::CPolyIterator for CQuadRootTINShape.
		//
		//**************************************
		{
		protected:
			CQuadRootTINShape& qntinshShape;	// Shape we're iterating through.

			const CRenderContext* prencContext;

			CDArray<CTriangleTIN*> daptriCurrentTris;// Holds triangles visible in current view.

			CTriangleTIN** pptriCurrent;		// Position in iterator.
			CTriangleTIN*  ptriCurrent;			// The current triangle pointer in iteration.

			CTexCoord	atcCoords[3];			// Pre-stored texture coords.

		public:

			//******************************************************************************************
			//
			// Constructor and destructor.
			//

			CPolyIterator(const CQuadRootTINShape& qntinsh, const CInstance* pins, const CRenderContext* prenc);

			~CPolyIterator();

			//******************************************************************************************
			//
			// Overrides.
			//

			int iNumPolygons() const
			{
				// Return the actual number in this iterator.
				return daptriCurrentTris.size();
			}

			int iNumPoints() const
			{
				// This must be an upper bound on the point index,
				// so return the quad tree's liberal estimate.
				return qntinshShape.iNumPoints();
			}

			int iNumShapeVertices() const
			{
				// We do not share vertices, so there are 3 per polygon.
				return iNumPolygons() * 3;
			}

			int iNumVertexPointers() const
			{
				// There are always 3 vertices per polygon.
				return iNumPolygons() * 3;
			}

			//******************************************************************************************
			void TransformPoints
			(
				const CTransform3<>&	tf3_shape_camera,
				const CCamera&			cam,
				CPArray<SClipPoint>		paclpt_points,
				bool					b_outcodes
			) override;

			//******************************************************************************************
			bool bNext()
			{
				pptriCurrent++;
				if (pptriCurrent >= daptriCurrentTris.end())
					return false;

				ptriCurrent = *pptriCurrent;
				return true;
			}

			//******************************************************************************************
			const CPlane& plPlane()
			{
				return ptriCurrent->plGetPlaneQuad();
			}

			//******************************************************************************************
			const CVector3<> v3Point()
			{
				return v3Point(0);
			}

			//******************************************************************************************
			const CDir3<>& d3Normal()
			{
				return plPlane().d3Normal;
			}

			//******************************************************************************************
			bool bCurved()
			{
				return true;
			}

			//******************************************************************************************
			const CMatrix3<> mx3ObjToTexture()
			{
				Assert(0);
				return CMatrix3<>();
			}

			//******************************************************************************************
			const CTexture* ptexTexture();

			//******************************************************************************************
			int iNumVertices()
			{
				return 3;
			}

			//******************************************************************************************
			CVector3<> v3Point(int i_vertex)
			{
				Assert(i_vertex < 3);
				Assert(ptriCurrent);
				return ptriCurrent->pqvtGetVertex(i_vertex)->v3Quad(qntinshShape.mpConversions);
			}

			//******************************************************************************************
			CDir3<> d3Normal(int i_vertex)
			{
				Assert(i_vertex < 3);
				Assert(ptriCurrent);

				return qntinshShape.d3GetNormal(ptriCurrent->pqvtGetVertex(i_vertex));
			}

			//******************************************************************************************
			const CTexCoord& tcTexCoord(int i_vertex)
			{
				// Return previously stored value.
				Assert(i_vertex < 3);
				return atcCoords[i_vertex];
			}

			//******************************************************************************************
			int iShapePoint(int i_vertex)
			{
				Assert(ptriCurrent);

				const CQuadVertexTIN* pqvt = ptriCurrent->pqvtGetVertex(i_vertex);
				return pqvt->iGetHandle();
			}

			//******************************************************************************************
			int iShapeVertex(int i_vertex)
			{
				// Should not be called, because pipeline was told we had unique vertices.
				return Verify(0);
			}

		protected:

			//******************************************************************************************
			//
			// Virtual overrides.
			//

			void Reset_() override
			{
				Reset();
			}

			bool bNext_() override
			{
				return bNext();
			}

			CPlane plPlane_() override
			{
				return plPlane();
			}

			CVector3<> v3Point_() override
			{
				return v3Point();
			}

			CDir3<> d3Normal_()
			{
				return d3Normal();
			}

			bool bCurved_() override
			{
				return bCurved();
			}

			CMatrix3<> mx3ObjToTexture_() override
			{
				return mx3ObjToTexture();
			}

			const CTexture* ptexTexture_() override
			{
				return ptexTexture();
			}

			int iNumVertices_() override
			{
				return iNumVertices();
			}

			CVector3<> v3Point_(int i_poly_vertex) override
			{
				return v3Point(i_poly_vertex);
			}

			CDir3<> d3Normal_(int i_poly_vertex) override
			{
				return d3Normal(i_poly_vertex);
			}

			CTexCoord tcTexCoord_(int i_poly_vertex) override
			{
				return tcTexCoord(i_poly_vertex);
			}

			int iShapeVertex_(int i_poly_vertex) override
			{
				return iShapeVertex(i_poly_vertex);
			}

			int iShapePoint_(int i_poly_vertex) override
			{
				return iShapePoint(i_poly_vertex);
			}
		};

		//******************************************************************************************
		CShape::CPolyIterator* pPolyIterator(const CInstance* pins, const CRenderContext* prenc) const override
		{
			return new CPolyIterator(*this, pins, prenc);
		}

		friend class CPolyIterator;
	};

};


#endif
