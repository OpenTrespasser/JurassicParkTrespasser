/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: The implementation of PlatonicInstance.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/PlatonicInstance.cpp                                      $
 * 
 * 6     8/26/98 4:51p Asouth
 * MSL's map requires the first item in the pair to be const
 * 
 * 5     8/19/98 1:37p Rwyatt
 * Platonic instances now use hash values instead of names
 * 
 * 4     8/10/98 9:28p Rwyatt
 * Addec counter for unique meshes
 * 
 * 3     10/27/97 4:35p Agrant
 * Added the bIsPlatonicIdealName function 
 * 
 * 2     10/16/97 1:56p Agrant
 * Now instancing all CInfo on object name.  Object-00 is always the base object from which we
 * instance.
 * 
 * 1     10/14/97 9:12p Agrant
 * 
 *********************************************************************************************/

#include "Common.hpp"
#include "PlatonicInstance.hpp"
#include "Lib/Std/Hash.hpp"
#include "Lib/sys/MemoryLog.hpp"

//
// Module specific variables.
//

TMapHashPins tmPlatonicIdeal;


//*****************************************************************************************
bool bGetPlatonicIdealName
(
   const char* ac_source,			// The name to be analyzed.
   char* ac_ideal_dest		// The return buffer to be filled with an ideal name (zero if not desired).
)
{

	int i_len = strlen(ac_source);
	strcpy(ac_ideal_dest, ac_source);

	for (int i = i_len - 1; i >= 0; --i)
	{
		// Scan to the last '-'
		if (ac_source[i] == '-')
		{
			if (i == i_len - 3)
			{
				// Then we have "xxx-XX"
				if (ac_source[i+1] == '0' &&
					ac_source[i+2] == '0')
				{
					// We have an ideal name already.
					return true;					
				}
			}

			if (i <= i_len - 3)
			{
				ac_ideal_dest[i+1] = '0';
				ac_ideal_dest[i+2] = '0';
				ac_ideal_dest[i+3] = '\0';			

				// Not ideal, but one exists.
				return false;
			}
		}
	}

	// No dash found.
	// No ideal name exists.
	ac_ideal_dest[0] = '\0';
	return false;
}


//*****************************************************************************************
bool bIsPlatonicIdealName
(
   const char* ac_source			// The name to be analyzed.
)
{

	int i_len = strlen(ac_source);

	// Is there a dash 3 from the end?
	if (ac_source[i_len - 3] == '-')
	{
		// Yes!  Are there two zeroes after it?
		if (ac_source[i_len - 2] == '0' && ac_source[i_len - 1] == '0')
			// Yes!  It is an ideal name.
			return true;
	}

	return false;
}


//*****************************************************************************************
void AddToIdealList
(
   const uint32		u4_hash,
   const CInstance*	pins			// The ideal instance.
)
{
#ifdef __MWERKS__
	tmPlatonicIdeal.insert(pair<const uint32, const CInstance*>(u4_hash, pins));
#else
	tmPlatonicIdeal.insert(pair<uint32, const CInstance*>(u4_hash, pins));
#endif
	MEMLOG_SET_COUNTER(emlUniqueInstances, tmPlatonicIdeal.size() );
}


//*****************************************************************************************
void RemoveFromIdealList
(
   const uint32 u4_hash
)
{
	TMapHashPins::iterator i = tmPlatonicIdeal.find( u4_hash );

	tmPlatonicIdeal.erase(i);
	MEMLOG_SET_COUNTER(emlUniqueInstances, tmPlatonicIdeal.size() );
}