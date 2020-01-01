/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Base class for renderer's types that are within a scene.
 *
 * Bugs:
 *
 * To do:
 *		Replace virtual rptr_const<CBioMesh>* rpbmCast() const with the right thing.
 *		Move prdtFindShared from Loader.cpp to some more reasonable place.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/RenderType.hpp                                           $
 * 
 * 14    98.09.08 2:35p Mmouni
 * Made changes to support creation of non CMesh objects for invisible geometry.
 * 
 * 13    5/14/98 8:06p Agrant
 * Removed the defunct poval_renderer argument form  rendertype constructor
 * 
 * 12    10/02/97 6:13p Agrant
 * CRenderType descends frim CFetchable
 * 
 * 11    97/08/18 16:18 Speter
 * Removed useless statement in ptCastRenderType.
 * 
 * 10    97/06/23 19:25 Speter
 * Changed wad of shit to rptr_const_static_cast.
 * 
 * 9     5/15/97 7:05p Agrant
 * Improved cast functionality
 * a FindShared function for rendertype loading with text properties.
 * 
 * 8     4/16/97 2:24p Agrant
 * Hacked biomesh cast function
 * 
 * 7     97/03/24 15:10 Speter
 * Made camera a CInstance rather than a CRenderType.
 * 
 * 6     97/01/26 19:52 Speter
 * Changed ptGet() to ptPtrRaw() in rptr casting functions.
 * 
 * 5     97/01/07 11:27 Speter
 * Put all CRenderTypes in rptr<>.
 * 
 * 4     11/26/96 6:28p Mlange
 * The bounding volume function now returns a CBoundVol instead of a CBoundVolCompound.
 * 
 * 3     11/23/96 5:48p Mlange
 * Made the bvcGet(0 function const. The 'cast' class now also handles null pointers.
 * 
 * 2     11/21/96 4:18p Mlange
 * Added copy function. Made various things const.
 * 
 * 1     11/21/96 12:37p Mlange
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_RENDERTYPE_HPP
#define HEADER_LIB_RENDERER_RENDERTYPE_HPP


#include "Lib/Loader/Fetchable.hpp"
#include "Lib/Transform/Vector.hpp"

//*********************************************************************************************
//
// Forward declarations for CRenderType.
//
class CLight;
class CShape;
class CMesh;
class CBioMesh;
class CBoundVol;


//  For loading/creating render types in the loader....
class CGroffObjectName;
class CLoadWorld;
class CHandle;
class CObjectValue;
class CValueTable;



//*********************************************************************************************
//
class CRenderType: public CRefObj, public CFetchable
//
// Base class for the rendering types that are the elements of a scene.
//
// Prefix: rdt
//
// Notes:
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CRenderType()
	{
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual const CBoundVol& bvGet
	(
	) const = 0;
	//
	// Obtain the extents.
	//
	// Returns:
	//		A bounding volume that describes the extents of this class.
	//
	//**********************************


	//*****************************************************************************************
	//
	virtual rptr<CRenderType> prdtCopy
	(
	) = 0;
	//
	// Copies this, returning a unique, non-instanced object outside of the instancing system.
	//
	// Returns:
	//		A new (unique) copy of this.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual CVector3<> v3GetPhysicsBox
	(
	) const;
	//
	// Obtain the extents of the physics box for this thing.
	//
	// Returns:
	//		A vector that specifies the extents of the physics box.
	//
	//**********************************

	//*****************************************************************************************
	//
	virtual CVector3<> v3GetPivot
	(
	) const;
	//
	// Obtain the pivot point for this thing.
	//
	// Returns:
	//		A vector that specifies the pivot point in local space.
	//
	//**********************************

	//
	// Identifier functions.
	//

	//*****************************************************************************************
	virtual void Cast(rptr_const<CRenderType>* pprdt) const
	{
		*pprdt = rptr_const_this(this);
	}

	//*****************************************************************************************
	virtual void Cast(rptr_const<CLight>* pplt) const
	{
		*pplt = rptr0;
	}

	//*****************************************************************************************
	virtual void Cast(rptr_const<CShape>* ppsh) const
	{
		*ppsh = rptr0;
	}

	//*****************************************************************************************
	virtual void Cast(rptr_const<CMesh>* ppsh) const
	{
		*ppsh = rptr0;
	}

	//*****************************************************************************************
	virtual void Cast(rptr_const<CBioMesh>* ppsh) const
	{
		*ppsh = rptr0;
	}

	//*****************************************************************************************
	//  HACK HACK HACK
	//
	//		Can't figure out how to avoid 
	//
	//      error C2664: 'Cast' : cannot convert parameter 1 from 'class rptr_const<class CBioMesh> *' to 'class rptr_const<class CShape> *'
	//
	virtual rptr_const<CBioMesh> rpbmCast() const
	{
		return rptr0;
	}

public:
	//*****************************************************************************************
	//
	static const rptr<CRenderType> prdtFindShared
	(
		const CGroffObjectName*	pgon,				// Pointer to GROFF name.
		CLoadWorld*				pload,				// Pointer to loader.
		const ::CHandle&			h_obj,				// Handle to the base object in the value table.
		CValueTable*			pvtable				// Pointer to the value table.
	);
	// Obtain a RenderType that has the requested data.
	//
	//	Notes:
	//		Uses the value table entries to determine what kind of render type to create.
	//		Instances and shares data wherever possible.
	//
	//**************************
};



//*********************************************************************************************
//
template<class T_TYPE> class ptCastRenderType
//
// Class that behaves like the dynamic_cast<> operator for classes derived from CRenderType.
//
// Prefix: N/A
//
// Example:
//		rptr<CShape> psh;
//
//		rptr<CRenderType> prdt = psh;
//
//		rptr<CShape> psh = ptCastRenderType<CShape>(psh);
//
//**************************
{
	rptr_const<T_TYPE> ptVal;	// The return value.

public:
	//*****************************************************************************************
	//
	// Constructors and destructors.
	//
	ptCastRenderType(rptr_const<CRenderType> prdt)
	{
		// Skip the cast function call if a null pointer is passed.
		if (prdt != 0)
			prdt->Cast(&ptVal);
	}

	//*****************************************************************************************
	//
	// Overloaded operators.
	//
	operator rptr<T_TYPE>() const
	{
		return rptr_nonconst(ptVal);
	}

	operator rptr_const<T_TYPE>() const
	{
		return ptVal;
	}

	operator bool() const
	{
		return ptVal;
	}
};




#endif
