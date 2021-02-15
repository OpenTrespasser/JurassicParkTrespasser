/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CShape
 *
 * Bugs:
 *
 * To do:
 *		Pass view volume info to iterator constructor.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/Shape.hpp                                               $
 * 
 * 72    98.09.30 10:34p Mmouni
 * Made pshShadow protected instead of private.
 * 
 * 71    98.09.22 12:26a Mmouni
 * Changed so that the unlit property is on a per mesh instead of per texture basis.
 * 
 * 70    98/07/23 18:29 Speter
 * Added GetExtents() virtual.
 * 
 * 69    7/10/98 5:10p Mlange
 * Poly iterators now take a render context instead of a camera.
 * 
 * 68    98/06/29 16:06 Speter
 * Rendering functions now take the CInstance* parameter. Make iNumTriangles() virtual.
 * 
 * 67    6/03/98 8:08p Pkeet
 * Changed 'Render' to accept the 'CShapePresence' variable.
 * 
 * 66    98/06/01 18:51 Speter
 * Added bBackface() function. Changed virtual iterator functions to always return objects, for
 * flexibility in derived classes. Inline versions can still return references for simplicity.
 * 
 * 65    4/16/98 3:32p Pkeet
 * Added the 'bSimpleShape' member function.
 * 
 * 64    3/03/98 3:26p Pkeet
 * Added the 'pshShadow' data member and the 'pshGetTerrainShape' and 'SetShadowShape' member
 * functions.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_SHAPE_HPP
#define HEADER_LIB_GEOMDBASE_SHAPE_HPP



#include "Plane.hpp"
#include "Lib/Renderer/Texture.hpp"
#include "Lib/Renderer/RenderType.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/Std/Set.hpp"

#include "Lib/Transform/Transform.hpp"


//
// Forward declarations.
//
class COcclude;
class CRenderContext;
class CRenderPolygon;
class CCamera;
class CShapePresence;
class CInstance;

//**********************************************************************************************
//
struct SClipPoint
//
// Prefix: clpt
//
// Utility class used by pipeline and shape for storing transformed points.
//
//**************************************
{
	CSet<EOutCode>	seteocOut;
	CVector3<>		v3Point;
};

//**********************************************************************************************
//
class CShape: public CRenderType
//
// Prefix: sh
//
// An abstract class providing shape information to the rendering pipeline.
//
//**************************************
{
protected:
	rptr<CShape> pshShadow;		// The shape used for rendering shadows to the terrain.

	// Constants for the thickness of the plane through a polygon.
	static TReal rPolyPlaneThicknessDefault;
	static TReal rPolyPlaneThicknessD3D;
	
public:
	//
	// Some public members which implement a simple multi-level shape scheme.
	// The shape pointed to by an instance is always the highest-level shape.
	// Lower levels, if any, are pointed to in a chain here, and a function is provided
	// for finding the appropriate shape for any given camera.
	//
	float fPRadius;				// Replacement radius for a mesh of this size.
	rptr<CShape> pshCoarser;	// The next coarser shape, if any.
	TReal rMinScreenArea;		// The minimum screen size this shape is usefully rendered as.
								// Below this size, the coarser shape should be used.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CShape()
		: fPRadius(0.0f)
	{
	}

	virtual ~CShape()
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	rptr_const<CShape> pshGetProperShape
	(
		TReal r_screen_area	// Approximate screen area in square screen pixels.
	) const;
	//
	// Returns:
	//		The appropriate shape for the given screen area.
	//
	//**********************************

	//******************************************************************************************
	//
	rptr_const<CShape> pshGetTerrainShape
	(
	) const;
	//
	// Returns the appropriate shape for terrain shadowing.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual int iNumPoints() const = 0;
	//
	// Returns:
	//		An upper limit on the number of unique points in the shape.
	//
	// Notes:
	//		This number must be larger than the highest value returned by CPolyIterator::iShapePoint().
	//		It is used in rendering to allocate an array holding transformed points, and to ensure
	//		that shared points are transformed only once.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual int iNumVertices() const = 0;
	//
	// Returns:
	//		An upper limit on the number of unique vertices in the shape.
	//
	// Notes:
	//		This number must be larger than the highest value returned by CPolyIterator::iShapeVertex().
	//		It is used in rendering to allocate an array holding lit vertices, and to ensure
	//		that shared vertices are lit only once.
	//
	//		The shape must keep track of how many unique vertices it has (each having
	//		a unique value for v3Point(), d3Normal(), and tcTexCoord()).  Furthermore, the shape 
	//		must associate each vertex of each polygon with a vertex index.  
	//		(See CShape::CPolyIterator::iShapeVertex()).
	//
	//**********************************

	//******************************************************************************************
	//
	virtual int iNumPolygons() const = 0;
	//
	// Returns:
	//		The number of polygons in the shape.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual int iNumTriangles() const = 0;
	//
	// Returns:
	//		The number of triangles in the shape (may be greater than number of polygons).
	//
	//**********************************

	//******************************************************************************************
	//
	virtual TReal rPolyPlaneThickness() const;
	//
	// Returns:
	//		The thickness of the plane through a polygon. Used to avoid problems with numerical
	//		accuracy when back-face culling.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual CPArray< CVector3<> > pav3GetWrap() const;
	//
	// Returns:
	//		A pointer to an array of vertices that shrink wrap this shape.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual void CreateWrap();
	//
	// Creates a wrapper for this shape.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual void CreateWrapBox();
	//
	// Creates a wrapper for this shape that is just a bounding box.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual void CreateMipMaps();
	//
	// Creates mipmap levels for textures associated with the object. If mipmaps are already
	// present, this function does nothing.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual bool bSimpleShape
	(
	)
	//
	// Returns 'true' if the shape contains two or less polygons.
	//
	//**********************************
	{
		return false;
	}

	//******************************************************************************************
	//
	virtual bool bIsAnimated
	(
	) const
	//
	// Returns 'true' if the shape is animated (e.g., is an animating mesh).
	//
	//**********************************
	{
		return false;
	}

	//******************************************************************************************
	//
	virtual void SetShadowShape
	(
		rptr<CShape>& rpsh
	);
	//
	// Sets the 'pshShadow' data member with the shape appropriate for use for rendering shadows
	// into the terrain.
	//
	//**********************************

	//
	// Several inline functions used during rendering.
	//

	//******************************************************************************************
	//
	bool bBumpmap() const
	//
	// Returns:
	//		Whether this shape potentially has any bumpmaps.
	//
	//**********************************
	{
		return true;
	}

	//******************************************************************************************
	//
	bool bSharedVertices() const
	//
	// Returns:
	//		Whether this shape has shared vertex values.
	//
	//**********************************
	{
		return true;
	}

	//******************************************************************************************
	//
	bool bIsCacheable() const
	//
	// Returns:
	//		Whether this shape can be cached.
	//
	//**********************************
	{
		return true;
	}

	//******************************************************************************************
	//
	bool bIsUnlit() const
	//
	// Returns:
	//		Whether this shape should be lit.
	//
	//**********************************
	{
		return false;
	}
	
	//******************************************************************************************
	//
	bool bTerrain() const
	//
	// Returns:
	//		Whether this shape is terrain (used for clipping).
	//
	//**********************************
	{
		return false;
	}

	//******************************************************************************************
	//
	virtual void MakeNoTexture
	(
	)
	//
	// Makes the surface apply flat-shading.
	//
	//**************************************
	{
	}
	
	//******************************************************************************************
	//
	virtual void GetExtents
	(
		CInstance* pins,					// Owning instance.
		const CTransform3<>& tf3_shape,		// Transform from shape into requested space.
		CVector3<>& rv3_min, CVector3<>& rv3_max
											// Returned extents.
	) const
	//
	// Gets the 3D extents of the shape, as transformed into the requested space.
	//
	//**********************************
	{
		// Default assumes point shape.
		rv3_min = rv3_max = tf3_shape.v3Pos;
	}

	//******************************************************************************************
	//
	class CPolyIterator
	//
	// Nested class responsible for providing iteration through the shape's polygons.
	// Created by the CShape's pPolyIterator() function (see below).
	// Descendents of CShape must have their own iterator class which inherits and implements
	// CShape::CPolyIterator, and which is returned by their pPolyIterator() function.
	//
	// This class uses the Optimal Virtual Function Call technique (see GblInc/Comments.hpp).
	//
	// Examples:
	//		// There are two ways of iterating through polygons.  The most general method
	//		// relies only on a CShape*, and uses its pPolyIterator() function to obtain an
	//		// iterator pointer.  In this method, the iterator must be deleted when finished.
	//
	//		CShape* psh;
	//		for (CShape::CPolyIterator* pti = psh->pPolyIterator(); pti->bNext(); )
	//		{
	//			// Operate on current polygon.
	//			CDir3<> d3_normal = pti->d3Normal();
	//			...
	//		}
	//		delete pti;
	//
	//		// If the specific CShape derived class is known, then the iterator can be declared
	//		// directly as a local variable, rather than as a pointer through pPolyIterator().
	//
	//		CShapeX* psh;
	//		for (CShapeX::CPolyIterator pi(*psh); pi.bNext(); )
	//		{
	//			// Operate on current polygon.
	//			CDir3<> d3_normal = pi.d3Normal();
	//			...
	//		}
	//
	//
	//**********************************
	{
	public:

		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		virtual ~CPolyIterator()
		{
		}

		//******************************************************************************************
		//
		virtual void TransformPoints
		(
			const CTransform3<>&	tf3_shape_camera,	// Transform to use.
			const CCamera&			cam,				// Camera to use for clipping.
			CPArray<SClipPoint>		paclpt_points,		// Destination array.
			bool					b_outcodes			// Whether outcodes need to be calculated.
		) = 0;
		//
		// Transform [and outcode] all unique points in the shape.
		//
		// Notes:
		//		Called just after iterator creation.
		//		Has no non-virtual counterpart.
		//
		//**********************************

		//******************************************************************************************
		//
		// Functions for iterating (see Example above).
		// In the base class, these invoke the private virtual functions.
		//

		//******************************************************************************************
		//
		void Reset()
		//
		// Reset for first polygon (done automatically upon construction).
		//
		//******************************
		{
			Reset_();
		}

		//******************************************************************************************
		//
		bool bNext()
		//
		// Advance to next polygon.
		//
		// Returns:
		//		Whether there is a next polygon; false if done.
		//
		//******************************
		{
			return bNext_();
		}

		//
		// Functions for retrieving polygon data.
		//

		//******************************************************************************************
		//
		bool bBackface
		(
			const CVector3<>& v3_cam		// The camera position.
		)
		//
		// Returns:
		//		Whether this polygon is facing away from the camera.
		//
		//**********************************
		{
			return bBackface_(v3_cam);
		}

		//******************************************************************************************
		//
		CPlane plPlane()
		//
		// Returns:
		//		The plane equation for the current polygon.
		//
		//**********************************
		{
			return plPlane_();
		}

		//******************************************************************************************
		//
		CDir3<> d3Normal()
		//
		// Returns:
		//		The normal for the current polygon.
		//
		//**********************************
		{
			return plPlane_().d3Normal;
		}

		//******************************************************************************************
		//
		CVector3<> v3Point()
		//
		// Returns:
		//		The point representative of the location of the polygon.
		//
		// Notes:
		//		This point is used for certain lighting calculations.  It need not be the centre of
		//		the polygon.  It may, for example, be the centre of a polygonal surface of which
		//		the polygon is a part.
		//
		//**********************************
		{
			return v3Point_();
		}

		//******************************************************************************************
		//
		bool bCurved()
		//
		// Returns:
		//		Whether this polygon represents a curved segment of the shape.
		//
		// Notes:
		//		This should be true when any of the polygon's vertex normals are different from
		//		its face normal; false if they are equal.
		//
		//**********************************
		{
			return bCurved_();
		}

		//******************************************************************************************
		//
		bool bOcclude()
		//
		// Returns:
		//		Whether this polygon represents an occluding polygon.
		//
		//**********************************
		{
			return false;
		}

		//******************************************************************************************
		//
		bool bCache()
		{
			return false;
		}
		//
		// Returns 'true' if the polygon is for image caching.
		//
		//**********************************

		//******************************************************************************************
		//
		CMatrix3<> mx3ObjToTexture()
		//
		// Returns:
		//		The matrix converting object-to-texture space, including the rotation of the texture
		//		on the polygon.
		//
		// Cross-references:
		//		Used by bumpmap lighting code.
		//
		//******************************
		{
			return mx3ObjToTexture_();
		}

		//******************************************************************************************
		//
		const CTexture* ptexTexture()
		//
		// Returns:
		//		A texture describing the polygon's surface.
		//
		// Notes:
		//		This returns a raw texture pointer for efficiency, as opposed to an rptr
		//		referencing the stored texture.  The assumption is that the pointer returned
		//		will be used only temporarily in rendering, and not copied to a persistent
		//		location.
		//
		//**********************************
		{
			return ptexTexture_();
		}

		//
		// Functions for retrieving vertex data.
		//

		//******************************************************************************************
		//
		int iNumVertices()
		//
		// Returns:
		//		The number of vertices in the current polygon.
		//
		//**********************************
		{
			return iNumVertices_();
		}

		//******************************************************************************************
		//
		CVector3<> v3Point
		(
			int i_poly_vertex				// Vertex in the current polygon.
		)
		//
		// Returns:
		//		The point corresponding to i_poly_vertex.
		//
		//**********************************
		{
			return v3Point_(i_poly_vertex);
		}

		//******************************************************************************************
		//
		CDir3<> d3Normal
		(
			int i_poly_vertex				// Vertex in the current polygon.
		)
		//
		// Returns:
		//		The normal corresponding to i_poly_vertex.
		//
		//**********************************
		{
			return d3Normal_(i_poly_vertex);
		}

		//******************************************************************************************
		//
		CTexCoord tcTexCoord
		(
			int i_poly_vertex				// Vertex in the current polygon.
		)
		//
		// Returns:
		//		The texture coords corresponding to i_poly_vertex.
		//
		//**********************************
		{
			return tcTexCoord_(i_poly_vertex);
		}

		//******************************************************************************************
		//
		int iShapeVertex
		(
			int i_poly_vertex				// Vertex in the current polygon.
		)
		//
		// Returns:
		//		The index number identifying which unique shape vertex this vertex corresponds to.
		//
		// Notes:
		//		The caller uses this index number to avoid processing identical vertices more than
		//		once.  The shape must keep track of how many unique vertices it has (each having
		//		a unique value for v3Point(), d3Normal(), and tcTexCoord()).  (This count is returned
		//		by CShape::uNumVertices().  And the shape must associate each vertex of each polygon
		//		with one of these unique vertices, via this function.
		//
		//**************************************
		{
			return iShapeVertex_(i_poly_vertex);
		}

		//******************************************************************************************
		//
		int iShapePoint
		(
			int i_poly_vertex				// Point in the current polygon.
		)
		//
		// Returns:
		//		The index number identifying which unique shape point this vertex corresponds to.
		//
		//**************************************
		{
			return iShapePoint_(i_poly_vertex);
		}

	protected:

		//******************************************************************************************
		//
		// Constructors.
		//

		CPolyIterator()
		{
		}

		// Create an iterator by passing the shape.  This is an alternative to creating the iterator
		// via CShape::pPolyIterator(), if the particular shape class is known.

		CPolyIterator(CShape& sh, const CInstance* pins, const CRenderContext* prenc)
		{
		}

		//******************************************************************************************
		//
		// Actual virtual functions.
		//

		virtual void Reset_() = 0;

		virtual bool bNext_() = 0;

		virtual bool bBackface_(const CVector3<>& v3_cam)
		{
			return false;
		}

		virtual CPlane plPlane_() = 0;

		virtual CVector3<> v3Point_()
		{
			// Default implementation just returns first point.
			return v3Point(0);
		}

		virtual const CTexture* ptexTexture_() = 0;

		virtual bool bCurved_()
		{
			// Default implementation returns true.
			return true;
		}

		virtual CMatrix3<> mx3ObjToTexture_() = 0;

		virtual int iNumVertices_() = 0;

		virtual CVector3<> v3Point_
		(
			int i_poly_vertex				// Vertex in the current polygon.
		) = 0;

		virtual CDir3<> d3Normal_
		(
			int i_poly_vertex				// Vertex in the current polygon.
		) = 0;

		virtual CTexCoord tcTexCoord_
		(
			int i_poly_vertex				// Vertex in the current polygon.
		) = 0;

		virtual int iShapeVertex_
		(
			int i_poly_vertex				// Vertex in the current polygon.
		) = 0;

		virtual int iShapePoint_
		(
			int i_poly_vertex				// Point in the current polygon.
		) = 0;
	};

	//******************************************************************************************
	//
	virtual CPolyIterator* pPolyIterator
	(
		const CInstance* pins,			// Instance owning this shape.
		const CRenderContext* prenc = 0	// Render context.
	) const = 0;
	//
	// Returns:
	//		An initialised CPolyIterator*, ready for iteration through the shape's polygons.
	//		This function is guaranteed *not* to return 0.
	//
	// Notes:
	//		This is the only way to access a CShape's iterator when the particular CShape class
	//		is unknown.  The caller is responsible for deleting the CIterator* when done.
	//
	// Example:
	//		CShape* psh;
	//		for (CShape::CPolyIterator* pti = psh->pPolyIterator(); *pti; ++*pti)
	//		{
	//			// Operate on current polygon.
	//			CDir3<> d3_normal = pti->d3Normal(0);
	//			...
	//		}
	//
	//**********************************

	//******************************************************************************************
	//
	virtual void Render
	(
		const CInstance*			pins,				// The instance owning this shape.
		CRenderContext&				renc,				// The rendering context.
		CShapePresence&				rsp,				// The shape-to-world transform.
		const CTransform3<>&		tf3_shape_camera,	// The shape to camera transform.
		const CPArray<COcclude*>&	papoc,				// Array of occluding objects.
		ESideOf						esf_view			// Shape's relation to the view volume
														// (for trivial acceptance).
	) const = 0;
	//
	//	A virtual function to speed up switching on rendering type.
	//
	//	Notes:
	//		Usually defined in Pipeline.cpp for each Shape type.
	//
	//**********************************

	//******************************************************************************************
	//
	// Overrides.
	//
	virtual void Cast(rptr_const<CShape>* ppsh) const override
	{
		*ppsh = rptr_const_this(this);
	}

	//******************************************************************************************
	//
	virtual void Validate
	(
	) const;
	//
	// Performs assertions to validate the object.
	//
	//**************************************

private:
	
	//
	// Disable copying of shapes.
	//

	CShape(const CShape&);

	CShape& operator =(const CShape&);
};

#endif
