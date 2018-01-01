/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Base class building optimal spatial partitions.
 *
 * To do:
 *
 * Notes:
 *		
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/PartitionBuildBase.hpp                                 $
 * 
 * 4     97/10/13 4:20p Pkeet
 * Added an optional parameter to the partition new function to provide memory instead of
 * calling the global new.
 * 
 * 3     97/07/01 11:24a Pkeet
 * Removed memory leaks.
 * 
 * 2     97/06/30 3:16p Pkeet
 * Added the 'ppartSpace' member variable and the 'ppartGetNewPart' member function.
 * 
 * 1     97/03/14 12:25p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_PARTITIONBUILDBASE_HPP
#define HEADER_LIB_GEOMDBASE_PARTITIONBUILDBASE_HPP


//*********************************************************************************************
//
template<int N> class CPartitionSet
//
// Base class for pairing partitions for spatial partitioning hierarchy.
//
// Prefix: pair
//
//**************************
{
private:

	CPartition* ppartParts[N];
	int         iSetPosition;

protected:

	float       fScore;

public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CPartitionSet()
	{
		Reset();
		SetScore();
	}
	

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Reset
	(
	)
	//
	// Resets the set position.
	//
	//**************************
	{
		iSetPosition = 0;
	}

	//*****************************************************************************************
	//
	void Back
	(
	)
	//
	// Removes the last 'Set' partition.
	//
	//**************************
	{
		--iSetPosition;
		Assert(iSetPosition >= 0);
	}

	//*****************************************************************************************
	//
	void Set
	(
		CPartition* ppart
	)
	//
	// Sets the next parition value.
	//
	//**************************
	{
		Assert(ppart);
		Assert(iSetPosition < N);

		ppartParts[iSetPosition++] = ppart;
	}

	//*****************************************************************************************
	//
	CPartition* ppartGet
	(
		int i_part	// Index of the partition to get.
	)
	//
	// Returns the indexed partition.
	//
	//**************************
	{
		Assert(i_part >= 0);
		Assert(i_part < iSetPosition);
		Assert(ppartParts[i_part]);

		return ppartParts[i_part];
	}

	//*****************************************************************************************
	//
	int iNumNodes
	(
	) const
	//
	// Returns the number of nodes in this partition list.
	//
	//**************************
	{
		return iSetPosition;
	}

	//*****************************************************************************************
	//
	bool bAlwaysMax
	(
	) const
	//
	// Returns 'true' to indicate that partitions should always try to contain the maximum
	// number of nodes, or 'false' to allow the maximum or less number of partitions.
	//
	//**************************
	{
		return true;
	}

	//*****************************************************************************************
	//
	static int iNumNodesMax
	(
	)
	//
	// Returns the maximum number of nodes in this partition list.
	//
	//**************************
	{
		return N;
	}

	//*****************************************************************************************
	//
	void SetScore()
	//
	// Returns a fitness score for this pair of partitions. The lower the score, the better
	// the fit.
	//
	//**************************
	{
		fScore = FLT_MAX;
	}

	//*****************************************************************************************
	//
	float fGetScore() const
	//
	// Returns a fitness score for this pair of partitions. The lower the score, the better
	// the fit.
	//
	//**************************
	{
		return fScore;
	}

	//*****************************************************************************************
	//
	bool operator <(const CPartitionSet<N>& partset) const
	//
	// Returns a fitness score for this pair of partitions. The lower the score, the better
	// the fit.
	//
	//**************************
	{
		return fGetScore() < partset.fGetScore();
	}

	//*****************************************************************************************
	//
	CPartition* ppartNewPart
	(
		CPartition* ppart = 0
	)
	//
	// Creates a new spatial partition to bound two existing partitions.
	//
	//**************************
	{
		Assert(0);
		return 0;
	}

};


#endif // HEADER_LIB_GEOMDBASE_PARTITIONBUILDBASE_HPP
