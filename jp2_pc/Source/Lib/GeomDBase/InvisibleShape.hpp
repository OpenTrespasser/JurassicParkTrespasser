/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CInvisibleShape - A shape that just contains info needed to make physics boxes.
 *
 * Bugs:
 *
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/InvisibleShape.hpp                                      $
 * 
 * 1     98.09.08 2:32p Mmouni
 * Initial version.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_INVISIBLESHAPE_HPP
#define HEADER_LIB_GEOMDBASE_INVISIBLESHAPE_HPP


#include "Lib/Renderer/RenderType.hpp"


//**********************************************************************************************
//
class CInvisibleShape: public CRenderType
//
// Prefix: invsh
//
// A class the holds info needed to construct physics boxes.
//
//**************************************
{
private:
	CVector3<> v3Pivot;
	CBoundVolBox bvbVolume;
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CInvisibleShape(const CDArray< CVector3<> >& mav3_points, const CVector3<>& v3_pivot) :
		bvbVolume(mav3_points), v3Pivot(v3_pivot)
	{
		// All handled in the initializers.
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual CVector3<> v3GetPhysicsBox() const override
	{
		return bvbVolume.v3GetMax();
	}

	//*****************************************************************************************
	virtual CVector3<> v3GetPivot() const override
	{
		return v3Pivot;
	}

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
};

#endif
