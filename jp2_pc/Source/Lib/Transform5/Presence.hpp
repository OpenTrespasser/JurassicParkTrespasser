/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Definition of CPresence, a class for defining position, orientation and scale.
 *
 * Bugs:
 *
 * To do:
 *		Make either CPlacement3 or CPresence an abstract type in terms of transforms.
 *		E.g. Encode identity transform.
 *
 *		For the future, the plan is to have classes external to the renderer (e.g. the world
 *		database) overload the methods defined by the presence. This way, these classes could
 *		implement a data structure that defines the transform in some other way, along with
 *		the appropriate member access functions. This could be useful to, say, define a
 *		space efficient transform for specialised entities that require just a translation and
 *		no rotation. However, it is currently unclear how this might actually be implemented.
 *
 *		Note that the member functions and overloaded operators for the presence class only
 *		define access functions for the transform. For example, there is no function or
 *		overloaded operator that would implement moving the presence by some vector. We are
 *		relying on the methods of the transform class to implement these. This will obviously
 *		work but might have performance implementations if the compiler converts the specific
 *		transform to a generic transform before applying it to the presence. This needs to be
 *		investigated.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform5/Presence.hpp                                           $
 * 
 * 4     97-04-24 18:40 Speter
 * Folded new changes from 4.2 version into this 5.0 specific version.
 * 
 * 4     97-03-31 22:26 Speter
 * CPresence3<> now inherits from CPlacement3<> for easier conversion.
 * 
 * 3     97/03/24 14:54 Speter
 * Removed constructors of CDirs and CRotates from ASCII chars; use d3ZAxis etc. constants.
 * When we need optimisation for axes, we'll use derived classes which don't need a switch
 * statement.
 * 
 * 2     96/12/10 11:05 Speter
 * Fixed inversion operator.
 * 
 * 1     96/12/04 20:23 Speter
 * Made CPresence into CPresence3<>, a proper transform with operators.
 * Moved to Transform/ directory.
 * 
 * 6     11/23/96 5:40p Mlange
 * Updated the default constructor to also initialise the data members.
 * 
 * 5     11/21/96 4:17p Mlange
 * Made scale public for now.
 * 
 * 4     11/21/96 12:03p Mlange
 * Updated some comments. Removed redundant includes. Removed destructor and made tf3WorldGet()
 * non-virtual. Added scale factor. Renamed some functions.
 * 
 * 3     11/13/96 12:38p Agrant
 * removed hierarchy info from CPresence, leaving nothing but a placement.
 * 
 * 2     10/31/96 2:38p Agrant
 * Added Accessor functions for world location setting and getting
 * 
 * 1     96/10/22 11:07 Speter
 * First version of Entity under new guise as Presence.
 * 
 * 11    96/10/04 17:51 Speter
 * Added tf3WorldGet() function.
 * 
 * 10    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 9     96/08/21 18:51 Speter
 * Removed d3XAxis, etc.  Now use CDir3(char) conversion.
 * 
 * 8     96/08/06 18:05 Speter
 * Made CEntity a node in a hierarchy, adding appropriate member functions and constructors.
 * Added p3WorldGet() function.
 * Added SInit type for simple constructors.
 * CEntity type can now be instantiated, as it can be a node in a Placement tree.
 * Removed implicit conversion operators to Placement.
 * Added CObjPlacement3 typedef.
 * 
 * 7     7/25/96 4:30p Mlange
 * Renamed CPoint and CNormal to CObjPoint and CObjNormal. Removed the various transform
 * typedefs such as CDir, CVector, etc. Updated the todo list.
 * 
 * 6     96/07/22 15:25 Speter
 * Added CTransform typedef.
 * 
 * 5     96/07/18 17:45 Speter
 * Changed CVertex to CPoint.
 * 
 * 4     96/07/08 12:42 Speter
 * Moved CVertex and CNormal here from ObjDef3D.
 * Added other typedefs for template classes of TObjReal.
 * 
 * 3     96/06/28 15:20 Speter
 * Changed Entity transform type from Transform to Placement.  Adjusted associated pipeline
 * operations.
 * 
 * 2     96/06/26 15:10 Speter
 * Changed to conform to new transform modules.
 * 
 * 1     96/06/18 10:03a Mlange
 * Definition of CEntity and TObjReal.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_PRESENCE_HPP
#define HEADER_LIB_TRANSFORM_PRESENCE_HPP

#include "Rotate.hpp"
#include "Scale.hpp"


//**********************************************************************************************
//
template<class TR = TReal> class CPresence3: public CPlacement3<>
//
// Prefix: pr3
//
// Adds scale to CPlacement3.  We use containment rather than inheritance to avoid exploding
// ambiguities.
//
//**************************************
{
public:
	TR				rScale;				// Scale factor.

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CPresence3()
		: rScale(1.0)
	{
	}

	CPresence3(CVector3<TR> v3)
		: CPlacement3<TR>(v3), rScale(1.0)
	{
	}

	CPresence3(CPlacement3<TR> p3, TR r_scale = 1.0)
		: CPlacement3<TR>(p3), rScale(r_scale)
	{
	}


	CPresence3(const CRotate3<TR>& r3, TR r_scale = 1.0)
		: CPlacement3<TR>(r3), rScale(r_scale)
	{
	}

	CPresence3(const CRotate3<TR>& r3, TR r_scale, const CVector3<TR>& v3)
		: CPlacement3<TR>(r3, v3), rScale(r_scale)
	{
	}


	//******************************************************************************************
	//
	// Conversion operators.
	//

	operator CTransform3<TR> () const
	{
		// Concatenate the scale and rotation, then add the translation.
		return CTransform3<TR>(CScaleI3<TR>(rScale) * CMatrix3<TR>(r3Rot), v3Pos);
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Return the inverse of the transform.
	CPresence3<TR> operator ~() const
	{
		// The inverse of a composite operation S*T is ~T * ~S, which is CPresence3(~S, -T * ~S).
		Assert(rScale != 0);

		// First, invert the components.
		CRotate3<TR> r3_inv = ~r3Rot;
		TR r_scale_inv = 1.0 / rScale;

		// Construct the inverted presence from these, and their effects on the translation.
		return CPresence3<TR>(r3_inv, r_scale_inv, v3Pos * r3_inv * -r_scale_inv);
	}

	//
	// Concatenate with another CPresence3.
	//

	CPresence3<TR> operator *(const CPresence3<TR>& pr3) const;

	CPresence3<TR>& operator *=(const CPresence3<TR>& pr3);

	//******************************************************************************************
	//
	// Member functions.
	//

	CPlacement3<TR> p3Placement() const
	{
		return CPlacement3<TR>(r3Rot, v3Pos);
	}
};

//******************************************************************************************
//
// Global operators for CPresence3<>.
//

	//
	// Operate on a vector.
	//

	template<class TR> inline CVector3<TR> operator *(const CVector3<TR>& v3, const CPresence3<TR>& pr3)
	{
		// Perform the rotation and scale, then add the translation.
		return v3 * pr3.r3Rot * pr3.rScale + pr3.v3Pos;
	}

	template<class TR> inline CVector3<TR>& operator *=(CVector3<TR>& v3, const CPresence3<TR>& pr3)
	{
		return v3 = v3 * pr3;
	}

	//
	// Operate on a direction by skipping the translation and scale steps.
	//

	template<class TR> inline CDir3<TR> operator *(const CDir3<TR>& d3, const CPresence3<TR>& pr3)
	{
		return d3 * pr3.r3Rot;
	}

	template<class TR> inline CDir3<TR>& operator *=(CDir3<TR>& d3, const CPresence3<TR>& pr3)
	{
		return d3 *= pr3.r3Rot;
	}

//**********************************************************************************************
//
// CPresence3<> implementation.
//

	//******************************************************************************************
	CPresence3<TR> CPresence3<TR>::operator *(const CPresence3<TR>& pr3) const
	{
		return CPresence3<TR>
		(
			r3Rot   * pr3.r3Rot,	// Rotation is concatenation of rotations.
			rScale  * pr3.rScale,	// Scale is product of scales.
			v3Pos   * pr3			// Translation is left-hand translation times right-hand presence.
		);
	}

	//******************************************************************************************
	CPresence3<TR>& CPresence3<TR>operator *=(const CPresence3<TR>& pr3)
	{
		// Concatenate base transform and translation separately.
		r3Rot   *= pr3.r3Rot;
		rScale  *= pr3.rScale;
		v3Pos   *= pr3;
		return *this;
	}

#endif

