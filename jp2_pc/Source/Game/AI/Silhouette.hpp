/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	CSilhouette class for AI library.
 *		
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/Silhouette.hpp                                                $
 * 
 * 11    9/16/98 9:33p Jpaque
 * Please clear silhouettes when constructing them
 * 
 * 10    9/02/98 5:20p Agrant
 * New silhouette simplification functions
 * 
 * 9     4/07/98 7:45p Agrant
 * Allowed large silhouettes.
 * 
 * 8     3/12/98 7:54p Agrant
 * New silhouette internals (array instead of vector)
 * 2d pathfinding handles compound physics objects
 * 
 * 7     7/14/97 12:54a Agrant
 * Corrected comment
 * 
 * 6     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 5     2/10/97 4:15p Agrant
 * Intermediate pathfinding check-in
 * 
 * 4     2/09/97 8:17p Agrant
 * Generalized silhouette code to take arbitrary boundvols-
 * only box is implemented, though.
 * 
 * 3     2/06/97 7:18p Agrant
 * Increased robustness of silhouette validation function.
 * 
 * 2     1/30/97 2:39p Agrant
 * Silhouettes added and pathfinder revision.
 * 
 * 1     1/28/97 6:41p Agrant
 * pathfinding files
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_SILHOUETTE_HPP
#define HEADER_LIB_AI_SILHOUETTE_HPP

#include <vector>
#include "Lib/Transform/Presence.hpp"
#include "Lib/Transform/Vector.hpp"
#include "Lib/EntityDBase/Container.hpp"
#include "Lib/GeomDBase/LineSegment.hpp"
//#include "Lib\Renderer\Geomtypes.hpp"


class CBoundVol;
class CBoundVolBox;

//*********************************************************************************************
//
class CSilhouette : public CMSArray<CVector2<> , 8>	
//
//	Prefix: sil
//
//	The CSilhouette class is a concave polygon used as a simplified representation
//  for an obstacle in the pathfinder.  
//
//	Notes:
//		Most of the real effort in the Silhouette is in the constructors, enabling the class
//		to generate good silhouettes quickly.  A Slihouette is usually a 2d projection of 
//		the physics bounding box of a 3d object onto the XY plane.
//
//		The points in a silhouette define a polygon and contain only unique corners of the 
//		polygon on the edge.  The points are either clockwise or counterclockwise, but never
//		completely random.
//
//*********************************************************************************************
{

//
//  Variable declarations
//

public:
	
//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	// default constructor, creates an invalid silhouette.
	CSilhouette() 
	{ 
		Clear();
	};


	// A constructor which projects a box onto the XY plane.
	CSilhouette
	(
		const CBoundVol*		pvb,	// The bounding volume to be projected.
		const CPresence3<>&		pr3,	// The orientation and scale of the bounding box.
		TReal					r_slop = 0	// The extra width to add to the box before determining silhouette.
	);


	//*****************************************************************************************
	//
	//	Member functions
	//


	//******************************************************************************************
	//
	void Init
	(
		const CBoundVol*		pvb,	// The bounding volume to be projected.
		const CPresence3<>&		pr3,	// The orientation and scale of the bounding box.
		TReal					r_pad	// An extra amount to add to the silhouette in all directions.
	);
	//
	// Initializes the silhouette. 
	//
	// Notes:
	//		Projects the bounding box to the xy axis, and calculates the silhouette as seen from above.
	//
	//**************************************

	//******************************************************************************************
	//
	void Init
	(
		const CBoundVolBox&		bvb,	// The bounding box to be projected.
		const CPresence3<>&		pr3 	// The orientation and scale of the bounding box.
	);
	//
	// Initializes the silhouette. 
	//
	// Notes:
	//		Projects the bounding box to the xy axis, and calculates the silhouette as seen from above.
	//
	//**************************************

	//******************************************************************************************
	//
	void Clear
	(
	)
	//
	// Removes all points from the silhouette.
	//
	// Notes:
	//		Until points are added, the silhouette is invalid.
	//
	//**************************************
	{
		// Set the array size to zero, effectively clearing the array.
		uLen = 0;
		//erase(begin(), end());
	}

	//******************************************************************************************
	//
	bool bIsValid
	(
	);
	//
	// Returns true if the silhouette has some substance.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bMerge
	(
		const CSilhouette* psil_A,
		const CSilhouette* psil_B
	);
	//
	// Merges silhouettes into this.  Returns true if successful, otherwise false.
	//
	//**************************************

	//******************************************************************************************
	//
	void Remove
	(
		int i_index
	)
	//
	// Remove a vertex.
	//
	//**************************************
	{
		AlwaysAssert(i_index >= 0);
		AlwaysAssert(i_index < size());

		for (int i = i_index; i < size() - 1; ++i)
		{
			(*this)[i] = (*this)[i+1];
		}

		uLen--;
	}

	//******************************************************************************************
	//
	void RemoveColinearPoints
	(
	);
	//
	// Removes intermediate colinear points in the silhouette.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bEdgeIntersects
	(
		const CLineSegment2<>& ls
	) const;
	//
	// Returns true if an edge of the silhouette intersects the given line segment.
	//
	//**************************************

	//******************************************************************************************
	//
	void push_back
	(
		const CVector2<>& v2
	) 
	//
	// Adds v2 to the silhouette.
	//
	//**************************************
	{
		*this << v2;
	}

	//******************************************************************************************
	//
	int size
	(
	) const
	//
	// returns the size of the silhouette
	//
	//**************************************
	{
		return uLen;
	}
	

};

// #ifndef HEADER_LIB_AI_SILHOUETTE_HPP
#endif
