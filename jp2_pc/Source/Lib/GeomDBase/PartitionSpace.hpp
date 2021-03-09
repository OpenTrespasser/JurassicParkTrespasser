/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		A pure spatial partitioning class:
 *
 *			CPartitionSpace
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/PartitionSpace.hpp                                     $
 * 
 * 20    7/03/98 9:21p Pkeet
 * Added code for finding duplicate partitions.
 * 
 * 19    6/03/98 8:14p Pkeet
 * Added the 'GetMinMaxDistance' member function.
 * 
 * 18    4/24/98 3:37p Pkeet
 * Added the 'fDistanceFromGlobalCameraSqr' member function.
 * 
 * 17    2/25/98 8:07p Pkeet
 * Added the 'GrowRandomly' and 'SetParent' member functions.
 * 
 * 16    98/02/18 10:09 Speter
 * Added get/set CPlacement functions to CPartition.
 * 
 * 15    98/02/04 14:47 Speter
 * Corrected include order.
 * 
 * 14    97/12/15 18:51 Speter
 * Changed CPresence3 arguments to references.
 * 
 * 13    12/01/97 3:32p Agrant
 * Partition loading completely reworked.
 * Partitions loaded/saved with save game.
 * 
 * 12    97/10/13 5:09p Pkeet
 * Added a class specific new and delete for 'CPartitionSpace.'
 * 
 * 11    10/02/97 6:10p Agrant
 * Added r3Rot() and SetRot() functions.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_PARTITIONSPACE_HPP
#define HEADER_LIB_GEOMDBASE_PARTITIONSPACE_HPP


//
// Required includes for this object.
//
#include "Partition.hpp"
#include "Lib/Renderer/GeomTypes.hpp"

#include <map>


//
// Types used.
//

// Type representing an associative container between a spatial partition and a value.
typedef std::map< uint, CPartition*, std::less<uint> > TPartSpaceMap;


//
// Constants.
//

// Version number for spatial partition files.
const uint32 u4SpatialParitionFileVersion = 0x1000AFBD;


//
// Class definitions.
//

//*********************************************************************************************
//
class CPartitionSpace : public CPartition
//
// Describes an abstraction of a spatial object hierarchy.
//
// Prefix: parts
//
// Notes:
//
//**************************
{
private:

	CVector3<> v3Position;	// Position of partition in the world.

	// Union providing storage for various bounding volume types.
	union
	{
		struct
		{
			CBoundVolInfinite bviInfinite;
		};
		struct
		{
			CBoundVolSphere   bvsSphere;
		};
		struct
		{
			CBoundVolBox      bvbBox;
		};
	};

public:

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	// Default constructor (uses an infinite bounding volume).
	CPartitionSpace
	(
	);

	// Constructor using an arbitrary bounding volume.
	CPartitionSpace
	(
		const CBoundVol& bv			// Pointer to the bounding volume.
	);

	// Construct from a file.
	CPartitionSpace
	(
	 	int i_handle,				// File handle.
		TPartSpaceMap& rpsm			// Associative map.
	);
	
	// Destructor.
	virtual ~CPartitionSpace();


	//Copy constructor
	CPartitionSpace(const CPartitionSpace& other)
		: CPartition(other),
		v3Position(other.v3Position),
		bvbBox(other.bvbBox)
	{
		//Copy only box, assuming it is the largest union member
		static_assert(sizeof(bviInfinite) <= sizeof(bvbBox) && sizeof(bvsSphere) <= sizeof(bvbBox));
	}

	//Copy assignment operator
	CPartitionSpace& operator=(const CPartitionSpace& other)
	{
		if (this == &other)
			return *this;

		CPartition::operator =(other); //Parent class
		v3Position = other.v3Position;
		
		//Copy only box, assuming it is the largest union member
		static_assert(sizeof(bviInfinite) <= sizeof(bvbBox) && sizeof(bvsSphere) <= sizeof(bvbBox));
		bvbBox = other.bvbBox;
		
		return *this;
	}
	
	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual void Cast(CPartitionSpace** ppparts) override
	//
	// Assigns a 'this' pointer for the CPartitionSpace or descendant object.
	//
	//**************************
	{
		*ppparts = this;
	}

	//*****************************************************************************************
	//
	virtual const CBoundVol* pbvBoundingVol
	(
	) const override;
	//
	// Returns the bounding volume for the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual const char* strPartType
	(
	) const override;
	//
	// Returns a partition type string.
	//
	//**************************

	//*****************************************************************************************
	//
	void* operator new
	(
		uint  u_size_type	// Amount of memory to allocate in bytes.
	);
	//
	// Returns pointer to memory allocated from the next position in the heap.
	//
	//**************************************

	//*****************************************************************************************
	//
	void operator delete
	(
		void *pv_mem
	);
	//
	// Calls the destructor on the object.
	//
	//**************************************

	//*****************************************************************************************
	//
	static void ResetHeap
	(
	);
	//
	// Resets the fast heap used for spatial partition memory allocations.
	//
	//**************************

	//*****************************************************************************************
	//
	void GrowRandomly
	(
	);
	//
	// Grows the partition's bounding volume very slightly using random dimensions.
	//
	//**************************

	//*****************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual float fDistanceFromGlobalCameraSqr() const override;

	//*****************************************************************************************
	virtual uint32 u4GetUniqueHandle() const override;

	//*****************************************************************************************
	virtual CPresence3<> pr3Presence() const override;

	//*****************************************************************************************
	virtual void SetPresence(const CPresence3<> &pr3) override;

	//*****************************************************************************************
	virtual CPlacement3<> p3Placement() const override;

	//*****************************************************************************************
	virtual void SetPlacement(const CPlacement3<>& p3) override;

	//*****************************************************************************************
	virtual CVector3<> v3Pos() const override;
	
	//*****************************************************************************************
	virtual void SetPos(const CVector3<>& v3_pos) override;

	//*****************************************************************************************
	virtual CRotate3<> r3Rot() const override;
	
	//*****************************************************************************************
	virtual void SetRot(const CRotate3<>& r3_rot) override;

	//*****************************************************************************************
	virtual float fGetScale() const override;

	//*****************************************************************************************
	virtual void SetScale(float f_new_scale) override;
	
	//*****************************************************************************************
	virtual int iSizeOf() const override;

	//*****************************************************************************************
	virtual bool bIsPureSpatial() const override;
	
	//*****************************************************************************************
	//virtual void Save(int i_handle) const;
	
	//*****************************************************************************************
	virtual char* pcSaveSpatial(char *pc_buffer) const override;

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const override;  // See instance.hpp

	//*****************************************************************************************
	virtual const char* pcLoad(const char *  pc_buffer) override;  // See instance.hpp

	//*****************************************************************************************
	virtual void SetParent(CPartition* ppart = 0) override;
	
	//*****************************************************************************************
	virtual void GetMinMaxDistance(float& rf_min, float& rf_max, CShapePresence& rsp) override;

	//*****************************************************************************************
	virtual bool bIsEqualToSpatial(const CPartition* ppart) const override;

};

//*********************************************************************************************
//
class CPartitionSpaceQuery : public CPartitionSpace
//
// Describes an abstraction of a spatial object hierarchy.
//
// Prefix: partsq
//
// Notes:
//
//**************************
{
private:
	CPresence3<>  pr3Pres;	// The basic positional information for the partition.
public:

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	// Constructor using a spherical bounding volume.
	CPartitionSpaceQuery
	(
		const CPresence3<>& pr3,	// Position of the centre of the sphere and its scale.
		TReal			r_radius	// Radius of the sphere.
	);

	// Constructor using an arbitrary bounding volume.
	CPartitionSpaceQuery
	(
		const CPresence3<>& pr3,	// Position of the centre of the volume and its scale.
		const CBoundVol&    bv		// The bounding volume.
	);

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	virtual CPresence3<> pr3Presence() const override;

	//*****************************************************************************************
	virtual void SetPresence(const CPresence3<> &pr3) override;

	//*****************************************************************************************
	virtual CPlacement3<> p3Placement() const override;

	//*****************************************************************************************
	virtual void SetPlacement(const CPlacement3<>& p3) override;

	//*****************************************************************************************
	virtual CVector3<> v3Pos() const override;
	
	//*****************************************************************************************
	virtual void SetPos(const CVector3<>& v3_pos) override;

};


#endif // HEADER_LIB_GEOMDBASE_PARTITIONSPACE_HPP