/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CMesh
 *		CMeshRect
 *
 * Bugs:
 *
 * To do:
 *		Move the definition of CShapeRect to the water files. It is only ever used there.
 *
 *		Implement the copy function.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/Mesh.hpp                                                $
 * 
 * 108   98.09.22 12:26a Mmouni
 * Changed so that the unlit property is on a per mesh instead of per texture basis.
 * 
 * 107   98/09/14 1:39 Speter
 * Added iNumVertexPointers(), for pipeline heap.
 * 
 * 106   9/08/98 8:51p Rwyatt
 * New members for mesh fast heap
 * 
 * 105   98.09.08 2:34p Mmouni
 * Made changes to support creation of non CMesh objects for invisible geometry.
 * 
 * 104   98.08.21 7:15p Mmouni
 * Added some stuff to mesh heap.
 * 
 * 103   8/19/98 1:40p Rwyatt
 * The SPolygon object to texture matrix is now a CMatrix3<>* and nor a ptr<CMatrix3<>>. This
 * matrix is allocated from the image load fast heap and is only present in bump mapped
 * polygons, these are only created at load time so there is no need to delete the matrix.
 * 
 * 102   8/11/98 2:15p Agrant
 * allow saving animating meshes
 * 
 * 101   98.08.10 4:21p Mmouni
 * Cleaned up CMeshAnimating.  Added support for animating the texture on just a sub-material.
 * 
 * 100   98/08/06 16:25 Speter
 * Added v3Max to store strict bounding extents, for physics. Changed a couple fields to chars
 * for efficienty. Re-ordered a few fields for clarity.
 * 
 * 99    98.07.28 4:36p Mmouni
 * Bounding volume is now constructed from wrap points if possible.
 * 
 * 98    98/07/23 18:30 Speter
 * Added GetExtents() implementation.
 * 
 * 97    7/20/98 7:09p Agrant
 * added freeze frame to animated textures
 * 
 * 96    7/10/98 5:10p Mlange
 * Poly iterators now take a render context instead of a camera.
 * 
 * 95    7/03/98 3:44p Pkeet
 * Added the 'bAlwaysFaceMesh' flag.
 * 
 * 94    98/06/29 16:07 Speter
 * Rendering functions now take the CInstance* parameter. Made iNumTriangles() virtual.
 * 
 * 93    6/26/98 7:24p Agrant
 * Animating mesh enchancements
 * 
 * 92    6/03/98 8:08p Pkeet
 * Changed 'Render' to accept the 'CShapePresence' variable.
 * 
 * 91    98/06/01 18:51 Speter
 * Moved CPolyIterator to separate include file.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_MESH_HPP
#define HEADER_LIB_GEOMDBASE_MESH_HPP

#include "Shape.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/Sys/FastHeap.hpp"
#include "Lib/Sys/Timer.hpp"


//
// Forward declarations.
//
class CCamera;
class CShapePresence;

//
// Enum types.
//

//**********************************************************************************************
enum ENormal
// Type to define how the CMesh constructor should calculate the normal.
// Prefix: enl
{
	enlBEGIN=0,
	enlSPECIFIED=enlBEGIN,		// Normal was specifically set; don't recalculate.
	enlFLAT,					// Set to face normal (hard edge).
	enlCURVED,					// Set to average of point normals (curved).
	enlGUESS,					// Make flat or curved by inferring smoothing groups.
	enlEND
};

// The following constants control vertex normal sharing.
// They refer to the maximum angle allowed between faces in order to share vertex normals.
extern const CAngle angNORMAL_SHARE_DEFAULT;

//**********************************************************************************************
//
CAngle angNormalShare
(
	ENormal enl
);
//
// Returns:
//		The equivalent share angle for an ENormal type.
//
//**************************************

//
// Class definitions.
//

//**********************************************************************************************
//
class CMesh: public CShape
//
// Prefix: msh
//
// An implementation of CShape.  Contains a polygonal mesh definition.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Static functions for memory allocation.
	//

	//*****************************************************************************************
	static CFastHeap	fhGlobalMesh;

	//*********************************************************************************************
	//
	static void ResetMeshHeap
	(
	)
	//*************************************
	{
		// Reset the load heap and remove all committed memory!
		fhGlobalMesh.Reset(0,0);
		MEMLOG_SET_COUNTER(emlMeshHeap,0);
	}

	//*********************************************************************************************
	//
	static bool bInMeshHeap
	(
		void* pv
	)
	//*************************************
	{
		return ( ((uint32)pv <= ((uint32)fhGlobalMesh.uNumBytesUsed() + (uint32)fhGlobalMesh.pvGetBase())) &&
		((uint32)pv >= (uint32)fhGlobalMesh.pvGetBase()) );
	}


	//******************************************************************************************
	// If this mesh is not in the mesh fast heap then delete its memory
	void operator delete(void* pv)
	{
		if (!bInMeshHeap(pv))
		{
			::delete pv;
		}
	}



	//**********************************************************************************************
	//
	// Nested types for CMesh.
	//

	class CHeap;

	//**********************************************************************************************
	//
	struct SSurface
	//
	// Prefix: sf
	//
	// Surface characteristics for a object polygon. 
	//
	//**************************************
	{
		rptr<CTexture>			ptexTexture;		// Colour/pattern info.

		//******************************************************************************************
		//
		// Constructors.
		//

		SSurface
		(
			rptr<CTexture> ptex = rptr<CTexture>()
		)
			: ptexTexture(ptex)
		{
		}
	};

	//**********************************************************************************************
	//
	struct SVertex
	//
	// Contains information for a unique vertex of a mesh.
	//
	// Prefix: mv
	//
	//**********************************
	{
		CVector3<>*	pv3Point;			// Point to shared coordinate.
		CDir3<>		d3Normal;			// The normal vector at this vertex.
		CTexCoord	tcTex;				// Texture coordinates at this vertex.

		// The following indices are needed for the pipeline interface.  They are stored here
		// redundantly to avoid the divide of a pointer subtraction.
		union
		{
			struct
			{
				uint		u4ShapeVertex;		// Index of this vertex in shape's array.
				uint		u4ShapePoint;		// Index of this vertex's point in shape's array.
			};
			bool			bSpecified;			// True if normal was explicitly given;
												// Otherwise, calculated automatically.
												// Used only during mesh construction.
		};

		//******************************************************************************************
		//
		// Constructors.  Provide them for all combinations of initialising values.
		//

		SVertex()
		//
		// Default constructor (useful for arrays).
		// Texture coordinates are uninitialised, so be sure to assign valid values.
		//
		{
		}

		SVertex(CVector3<>* pv3, CTexCoord tc = CTexCoord(0, 0))
		//
		// Construct with only a point pointer and optional texture coordinate. 
		//
			: pv3Point(pv3), tcTex(tc)
		{
			bSpecified = false;
		}

		SVertex(CVector3<>* pv3, const CDir3<>& d3, CTexCoord tc = CTexCoord(0, 0))
		//
		// Construct with only a point pointer and optional texture coordinate. 
		//
			: pv3Point(pv3), tcTex(tc), d3Normal(d3)
		{
			bSpecified = true;
		}

		SVertex(CHeap& mh, int i_point_index, CTexCoord tc)
			: pv3Point(&mh.mav3Points[i_point_index]), tcTex(tc)
		{
			bSpecified = false;
		}

		SVertex(CHeap& mh, int i_point_index, CTexCoord tc, const CDir3<>& d3)
			: pv3Point(&mh.mav3Points[i_point_index]), tcTex(tc), d3Normal(d3)
		{
			bSpecified = true;
		}

		SVertex(const SVertex& mv_a, const SVertex& mv_b, TReal r_t, CHeap& mh);
		// Interpolation constructor;
	};

	//**********************************************************************************************
	//
	struct SPolygon
	//
	// Polygon definition for a mesh.
	//
	// Prefix: mp
	//
	//**************************************
	{
	public:
		CPArray<SVertex*>	papmvVertices;	// Array of pointers to vertices in this mesh.
											// This array is a segment of the mesh's papmvVertices array.
		CPlane				plPlane;		// The definition of this face's plane.
		ptr<SSurface>		pSurface;		// Pointer to a shared surface.
		bool				bOcclude: 1,	// 'True' if the polygon is for occlusion only.
							bCache:   1,	// 'True' if the polygon is an image cache.
							bCurved:  1,	// Whether this polygon is "curved" (differing vertex normals).
							bHidden:  1;	// Whether this polygon is hidden in the mesh (used internally).
		CMatrix3<>*			pmx3ObjToTexture;	// Store transform for bump-mapped surfaces.
		TReal				rWorldArea;		// World area of the polygon.

		//******************************************************************************************
		//
		// Constructors.
		//

		// Default constructor (useful for arrays).
		SPolygon()
			: bOcclude(0), bCache(0), pmx3ObjToTexture(0)
		{
		}

		SPolygon(CPArray<SVertex*> papmv)
		// Takes an array of vertex pointers.
			: papmvVertices(papmv), bOcclude(0), bCache(0), pmx3ObjToTexture(0)
		{
		}

		SPolygon
		(
			CHeap& mh,					// Heap being used to build mesh.
			int i_vertex_start,			// Heap index of vertices for this polygon.
			int i_vertex_count			// Number of vertices.
		);
		// Allocates the array of vertex pointers from mh.

		SPolygon
		(
			CHeap& mh,					// Heap being used to build mesh.
			int i_vertex_0,				// Vertex indices defining a triangle.
			int i_vertex_1,
			int i_vertex_2
		);
		// Allocates the array of vertex pointers from mh.

		~SPolygon()
		{
			//delete pmx3ObjToTexture;
		}

		//
		// Convenient access functions.
		//

		//******************************************************************************************
		int iNumVertices() const
		{
			return papmvVertices.uLen;
		}

		//******************************************************************************************
		const CVector3<>& v3Point
		(
			int i_vertex
		) const
		//
		// Returns:
		//		The point for the given vertex.
		{
			return *papmvVertices[i_vertex]->pv3Point;
		}

		//******************************************************************************************
		const CDir3<>& d3Normal
		(
			int i_vertex
		) const
		//
		// Returns:
		//		The normal for the given vertex.
		{
			return papmvVertices[i_vertex]->d3Normal;
		}

		//******************************************************************************************
		const CTexCoord& tcTexCoord
		(
			int i_vertex
		) const
		//
		// Returns:
		//		The texture coordinate for the given vertex.
		{
			return papmvVertices[i_vertex]->tcTex;
		}

		//******************************************************************************************
		TReal rGetArea() const;

		//**************************************************************************************
		//
		CVector3<> v3GetCentre
		(
		) const;
		//
		// Returns the centre of the polygon.
		//
		//**********************************

	};

	//******************************************************************************************
	//
	class CHeap
	//
	// Prefix: mh
	//
	//**************************************
	{
	public:
		bool bOptimized;		// Heap has already been optimized.

		CVector3<>				v3Pivot;
		CDArray<SPolygon>		mampPolygons;
		CDArray<SVertex*>		mapmvVertices;
		CDArray<SVertex>		mamvVertices;
		CDArray< CVector3<> >	mav3Points;
		CDArray< CVector3<> >	mav3Wrap;
		CDArray<SSurface>		masfSurfaces;

	public:
		CHeap(int i_num_polygons);

		~CHeap();

		//**************************************************************************************
		//
		void Reset();
		//
		// Resets all sub-arrays.
		//
		//******************************
	};


	//******************************************************************************************
	//
	// Static flags controlling mesh behaviour features.
	//

	static bool bMergePolygons;
	static bool bSplitPolygons;
	static bool bQuashPolygons;
	static bool bCurveBumps;					// Override bump curve parameter if true.

	//******************************************************************************************
	//
	// Data members.
	//

//protected:
	// These should not be accessed if it is possible to avoid it.
public:
	CPArray<SPolygon>		pampPolygons;		// The polygons defining the mesh.
	CPArray<SVertex*>		papmvVertices;		// Storage for the vertex pointer arrays.
	CPArray<SVertex>		pamvVertices;		// Shared vertices for the polygons.
	CPArray< CVector3<> >	pav3Points;			// Shared points for the polygons in this mesh.
	CPArray<SSurface>		pasfSurfaces;		// The surfaces referenced by the polygons.

	CPArray< CVector3<> >	pav3Wrap;			// Vertices that shrink wrap this shape.
	CPArray<SPolygon*>		papmpOcclude;		// Pointers to occlusion polygons.

	CBoundVolBox	 		bvbVolume;			// The bounding box for this mesh.

	// The following 2 fields are used in constructing physics infos from meshes.
	// They are not needed otherwise.
	CVector3<>				v3Max;				// The maximum extents for this mesh.
												// bvbVolume may extend beyond the mesh due to wrap point
												// perturbation, but physics will need to access v3Max.
	CVector3<>				v3Pivot;			// Offset (in local space) to point object should pivot about.

	// Store the following adjacent fields as chars to save space.
	char					bCacheableMesh;		// Set to 'true' if the mesh is cacheable.
	char					bAlwaysFaceMesh;	// Set to 'true' if the mesh always faces the camera.
	char					bUnlit;				// Set to 'true' if the mesh does not get lighting.
	char					cLastPrefetch;		// Enumerates the last prefetch to touch the mesh.
												// This field is likely to change without warning,
												// even if the mesh is const.  It speed up prefetching.
public:

	//******************************************************************************************
	//
	static CHeap& mhGetBuildHeap();
	//
	// Returns:
	//		The mesh heap to use to build mesh data.
	//
	// This is the heap on which mesh data must be placed for construction.
	//
	//**********************************


	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	CMesh
	(
		CHeap& mh_data,					// Heap containing initially created data.
		CAngle ang_max_vertex_share = angNORMAL_SHARE_DEFAULT,	
										// Controls vertex normal calculation.
		bool b_merge_polygons = true,
		bool b_split_polygons = false,
		bool b_curved = false
	);
	//
	// Initialiser constructor. This constructor will actually make the mesh hierarchy consistent.
	// It calculates polygon plane equations and vertex normals that are unspecified.  It also
	// optimises the mesh by merging polygons if so told, and eliminating duplicate vertices.
	//

	//******************************************************************************************
	CMesh
	(
		CHeap& mh_data,					// Heap containing initially created data.
		ENormal enl,					// Controls vertex normal calculation.
		bool b_merge_polygons = true,
		bool b_split_polygons = false,
		bool b_curved = false
	);
	//
	// Initialiser constructor. This constructor will actually make the mesh hierarchy consistent.
	// It calculates polygon plane equations and vertex normals that are unspecified.  It also
	// optimises the mesh by merging polygons if so told, and eliminating duplicate vertices.
	//

	virtual ~CMesh();

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	bool bIsCacheable() const
	{
		return bCacheableMesh;
	}

	//******************************************************************************************
	bool bIsUnlit() const
	{
		return bUnlit;
	}

	//******************************************************************************************
	int iNumPoints() const override
	{
		return pav3Points.uLen;
	}

	//******************************************************************************************
	int iNumVertices() const override
	{
		return pamvVertices.uLen;
	}

	//******************************************************************************************
	int iNumVertexPointers() const
	{
		return papmvVertices.uLen;
	}

	//******************************************************************************************
	int iNumPolygons() const override
	{
		return pampPolygons.uLen;
	}

	//******************************************************************************************
	int iNumTriangles() const override;


	//******************************************************************************************
	const CVector3<>& v3Point
	(
		int i_point
	) const
	{
		return pav3Points[i_point];
	}

	//******************************************************************************************
	virtual void Render
	(
		const CInstance*			pins,				// The instance owning this shape.
		CRenderContext&				renc,				// The rendering context.
		CShapePresence&				rsp,				// The shape-to-world transform.
		const CTransform3<>&		tf3_shape_camera,	// The shape to camera transform.
		const CPArray<COcclude*>&	papoc,				// Array of occluding objects.
		ESideOf						esf_view			// Shape's relation to the view volume
														// (for trivial acceptance).
	) const override;
	//
	// Defined in Pipeline.cpp.
	//

	//******************************************************************************************
	virtual const CBoundVol& bvGet() const override
	{
		return bvbVolume;
	}

	//*****************************************************************************************
	virtual CVector3<> v3GetPhysicsBox() const override
	{
		return v3Max;
	}

	//*****************************************************************************************
	virtual CVector3<> v3GetPivot() const override
	{
		return v3Pivot;
	}

	//******************************************************************************************
	virtual void GetExtents(CInstance* pins, const CTransform3<>& tf3_shape,
		CVector3<>& rv3_min, CVector3<>& rv3_max) const override;

	//******************************************************************************************
	void AdjustBoundingVolumeForDetailReducedVersions();
	//
	// Adjust the bounding volume of the this Mesh so that in consists of the union of
	// its own bounding volume and the bounding volumes of its detail reduced meshes.
	//
	//******************************************************************************************


	//******************************************************************************************
	void Rescale
	(
		TReal r_scale		// Scale factor.
	);
	//
	// Scale the meshed points and bounding volume by a scale factor.
	//
	//******************************************************************************************


	//******************************************************************************************
	virtual rptr<CRenderType> prdtCopy() override
	{
		Assert(false);
		return rptr0;
	}

	//******************************************************************************************
	virtual CPArray< CVector3<> > pav3GetWrap() const override;

	//******************************************************************************************
	virtual void CreateWrap() override;

	//******************************************************************************************
	virtual void CreateWrapBox() override;

	//******************************************************************************************
	virtual void CreateMipMaps
	(
		uint32 u4_smallest = 0xffffffff			// Generate all mips by default
	);
	//******************************************************************************************
	virtual bool bSimpleShape() override;

	//
	// CFetchable overrides.  (for prefetching)
	//
	//*****************************************************************************************
	virtual void OnPrefetch(bool b_in_thread) const;

	//*****************************************************************************************
	virtual void OnFetch();

	//*****************************************************************************************
	virtual void OnUnfetch();

	//*****************************************************************************************
	virtual int iSize() const
	{
		return sizeof(CMesh);
	}

public:

	// Forward nested class declaration; include "MeshIterator.hpp" for definition.
	class CPolyIterator;

	//******************************************************************************************
	virtual CShape::CPolyIterator* pPolyIterator(const CInstance* pins, const CRenderContext* prenc) const override;

	//******************************************************************************************
	//
	// Member functions.
	//

public:

	//******************************************************************************************
	//
	uint uSize() const;
	//
	// Returns:
	//		Size of mesh in bytes (not counting textures).
	//
	//**********************************

	//******************************************************************************************
	//
	void MakeVerticesUnique();
	//
	// Makes each polygon have unique vertex information.
	//
	//**********************************

	//******************************************************************************************
	//
	void MakeSurfacesUnique();
	//
	// Makes each polygon have unique surface information.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual void MakeNoTexture
	(
	) override;
	//
	// Makes the surface apply flat-shading.
	//
	//**************************************

	//******************************************************************************************
	//
	void SetPolygonAsCache();
	//
	// Sets the flag in the mesh polygons to indicate the mesh is an image cache. 
	//
	//**********************************

	//******************************************************************************************
	virtual void Validate() const override;

protected:

	// Null constructor for use by descendents.
	CMesh()
	{
	}

	//******************************************************************************************
	void CalculateFaceNormals
	(
		CHeap& mh
	);

	//******************************************************************************************
	void CalculateVertexNormals
	(
		CHeap& mh,
		CAngle ang_max_vertex_share
	);

	//******************************************************************************************
	void MergeVertices
	(
		CHeap& mh
	);

	//******************************************************************************************
	void MergePolygons
	(
		CHeap& mh
	);

	//******************************************************************************************
	void IdentifyOccludingPolygons();

	//******************************************************************************************
	void SplitIntersectingPolygons
	(
		CHeap& mh
	);

	//******************************************************************************************
	void MakePolygonsPlanar
	(
		CHeap& mh
	);

	//******************************************************************************************
	void AllocateFinalData
	(
		CHeap& mh
	);

public:
	//  Cast override:

	//*****************************************************************************************
	virtual void Cast(rptr_const<CMesh>* ppmesh) const override
	{
		*ppmesh = rptr_const_this(this);
	}

	friend class CPolyIterator;

protected:
	const CVector3<>& v3Point_
	(
		int i_point
	) const
	{
		return v3Point(i_point);
	}
};

//**********************************************************************************************
//
class CMeshPolygon: public CMesh
//
// Prefix: mshp
//
// A mesh containing a single polygon.
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	CMeshPolygon
	(
		CPArray< CVector3<> > pav3_points,		// The points in this polygon.
												// This array becomes owned by the mesh.
		CPArray<CTexCoord> patc_tex,			// The array of corresponding texture coordinates.
												// Must be same length as pav3_points, and it
												// becomes owned by the mesh.
		SSurface sf = SSurface()				// The surface for this polygon.
	);

	//******************************************************************************************
	CMeshPolygon
	(
		TReal r_x, TReal r_y,					// Dimensions for rectangular polygon (total width).
		bool b_y_downward = false,				// Texture coordinates are automatically assigned.
												// This flag determines whether texture Y is 0
												// at maximum point Y.
		SSurface sf = SSurface()				// The surface for this polygon.
	);
};


//**********************************************************************************************
//
class CMeshCache: public CShape
//
// Prefix: mshc
//
// A mesh simplified for an image cache.
//
//**************************************
{
public:
	struct SVertex
	//
	// Contains information for a each vertex of the cache mesh.
	//
	// Prefix: mv
	//
	//**********************************
	{
		CVector3<>	v3Point;			// Co-ordinate of this verteix.
		CTexCoord	tcTex;				// Texture coordinates at this vertex.
	};

	CAArray<SVertex>		pamvVertices;		// Vertices of the polygon.
	CBoundVolBox	 		bvbVolume;			// The bounding box for this mesh.
	rptr<CTexture>			ptexTexture;		// The texture for the polygon.
	CPlane					plPlane;			// Plane of the polygon.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	CMeshCache
	(
		CPArray< CVector3<> > pav3_points,		// The points in this polygon.
												// This array becomes owned by the mesh.
		CPArray<CTexCoord> patc_tex,			// The array of corresponding texture coordinates.
												// Must be same length as pav3_points, and it
												// becomes owned by the mesh.
		rptr<CTexture> ptex = rptr0				// The texture for this polygon.
	);

	//******************************************************************************************
	virtual const CBoundVol& bvGet() const override
	{
		return bvbVolume;
	}

	//******************************************************************************************
	virtual rptr<CRenderType> prdtCopy() override
	{
		Assert(false);
		return rptr0;
	}

	//******************************************************************************************
	int iNumPoints() const override
	{
		return pamvVertices.uLen;
	}

	//******************************************************************************************
	int iNumVertices() const override
	{
		return pamvVertices.uLen;
	}

	//******************************************************************************************
	int iNumVertexPointers() const
	{
		return pamvVertices.uLen;
	}

	//******************************************************************************************
	int iNumPolygons() const override
	{
		return 1;
	}

	//******************************************************************************************
	int iNumTriangles() const override
	{
		return 1;
	}

	//******************************************************************************************
	virtual CShape::CPolyIterator* pPolyIterator(const CInstance* pins, const CRenderContext* prenc) const override
	{
		Assert(false);
		return NULL;
	}
};


//*********************************************************************************************
//
class CMeshBox: public CMesh
//
// Helper class that constructs a simple box mesh.
//
//**************************************
{
public:

	CMeshBox
	(
		const CVector3<>& v3_extents = CVector3<>(1.0, 1.0, 1.0),	
										// The box extents.
		SSurface sf = SSurface()		// The surface for all faces.
	);
};

//**********************************************************************************************
//
class CMeshAnimating: public CMesh
//
// Prefix: ma
//
// A mesh that changes textures and/or texture co-ordinates over time.
//
//	Notes:
//		Specifically, the texture changes.  For now, requires that there be a single texture map 
//		for the entire mesh, and that the texture coords never change.  All this could change fairly 
//		easily.
//
//**************************************
{
protected:
	int		iCurrentIndex;				// The texture we are currently using.
	int		iTrackTwo;					// The first index of track two.
	int		iAnimateSurface;			// Surface number of the texture to animate.

	bool	bAnimateMaps;				// Automagically change the texture.
	TSec	sTimeStep;					// The length of time between frame changes.
	TSec	sTimeToChange;				// The time to change the texture.
	int		iFreezeFrame;				// An optional frame to stop on.  -1 if none.

	bool		bAnimateCoords;			// Texture co-ordinate animation.
	CVector2<>	v2TexCoordDelta;		// Amount to increment texture coords per second.

public:
	CMAArray<rptr<CTexture> > aptexTextures;	// The array of textures to use.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	CMeshAnimating
	(
		CHeap& mh_data,					// Heap containing initially created data.
		CAngle ang_max_vertex_share = angNORMAL_SHARE_DEFAULT,	
										// Controls vertex normal calculation.
		bool b_merge_polygons = true,
		bool b_split_polygons = false,
		bool b_curved = false
	);

	//******************************************************************************************
	CMeshAnimating
	(
		CHeap& mh_data,					// Heap containing initially created data.
		ENormal enl,					// Controls vertex normal calculation.
		bool b_merge_polygons = true,
		bool b_split_polygons = false,
		bool b_curved = false
	);
	
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	void SetFrame
	(
		int i_current_frame				// Texture frame.
	);
	//
	// Sets the animated textures to frame "i_current_frame"
	//
	//******************************

	//******************************************************************************************
	void SetInterval
	(
		float f_time_step
	);
	//
	// Sets the interval the animation changed on.
	//
	//******************************

	//******************************************************************************************
	void SetSurface
	(
		int i_surface_no
	);
	//
	// Sets the surface that the texture is animated for (default is all).
	//
	//******************************

	//******************************************************************************************
	void SetCoordDeltas
	(
		float f_dx, float f_dy
	);
	//
	// Sets the co-ordinate deltas.
	//
	//******************************

	//******************************************************************************************
	void SetTrackTwo
	(
		int i_track_two
	);
	//
	// Sets the start of the second track for two track animations.
	//
	//******************************

	//******************************************************************************************
	void SetFreezeFrame
	(
		int i_freeze_frame
	);
	//
	// Sets a frame the animation stops at.
	//
	//******************************

	//******************************************************************************************
	//
	void AddTexture
	(
		rptr<CTexture> ptex		// The texture to add to the cycle.
	);
	//
	// Adds a texture to the list of animating textures.
	//
	//******************************

	//******************************************************************************************
	//
	virtual bool bIsAnimated
	(
	) const override
	//
	// Returns 'true' if the shape is animated (e.g., is an animating mesh).
	//
	//**********************************
	{
		return true;
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	char* pcSave(char*  pc) const;

	//*****************************************************************************************
	const char* pcLoad(const char*  pc);

	//******************************************************************************************
	virtual void Render
	(
		const CInstance*			pins,				// The instance owning this shape.
		CRenderContext&				renc,				// The rendering context.
		CShapePresence&				rsp,				// The shape-to-world transform.
		const CTransform3<>&		tf3_shape_camera,	// The shape to camera transform.
		const CPArray<COcclude*>&	papoc,				// Array of occluding objects.
		ESideOf						esf_view			// Shape's relation to the view volume
														// (for trivial acceptance).
	) const override;
	//
	//	Overridden to enable the mesh to decide when to animate itself.
	//
	//	Notes:
	//		Modifies "this" in spite of constness, changing the current texture info.
	//
	//******************************
};

//**********************************************************************************************
//
class CMeshPlanted: public CMesh
//
// Prefix: mp
//
// A mesh that has verts welded to the terrain.
//
//**************************************
{
public:

	CAArray<int> aiPlantedPoints;	// Indexes of points to plant on the ground at render time.
	CAArray<CVector3<> > av3PlantedPoints;	// Original mesh positions of planted points.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	CMeshPlanted
	(
		CHeap& mh_data,					// Heap containing initially created data.
		const CPresence3<>& pr3,		// The presence used to determine which points are planted.
		TReal r_plant_distance, 
		CAngle ang_max_vertex_share = angNORMAL_SHARE_DEFAULT,	
										// Controls vertex normal calculation.
		bool b_merge_polygons = true,
		bool b_split_polygons = false,
		bool b_curved = false
	);

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void Render
	(
		const CInstance*			pins,				// The instance owning this shape.
		CRenderContext&				renc,				// The rendering context.
		CShapePresence&				rsp,				// The shape-to-world transform.
		const CTransform3<>&		tf3_shape_camera,	// The shape to camera transform.
		const CPArray<COcclude*>&	papoc,				// Array of occluding objects.
		ESideOf						esf_view			// Shape's relation to the view volume
														// (for trivial acceptance).
	) const override;
	//
	//	Overridden to enable the mesh to decide when to animate itself.
	//
	//	Notes:
	//		Modifies "this" in spite of constness, changing the current texture info.
	//
	//******************************

	//******************************************************************************************
	//
	// Member functions.
	//
};


#endif
