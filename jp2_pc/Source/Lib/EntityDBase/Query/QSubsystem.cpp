/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of QSubsystem.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QSubsystem.cpp                                  $
 * 
 * 4     3/16/98 1:03p Pkeet
 * Removed STL includes from the header file through the use of opaque declarations.
 * 
 * 3     5/08/97 4:08p Mlange
 * Added missing #include.
 * 
 * 2     5/01/97 3:47p Rwyatt
 * Renamed calls to ptCastInstance to ptCast. This is because it no longer casts from an
 * instance but from a partition. Renaming the function to ptCastPartition would have caused
 * more work later if another class is added under cPartition. All casts have base
 * implementations in CPartition rather than CInstance
 * 
 * 1     3/14/97 5:42p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "QSubsystem.hpp"
#include "Lib/EntityDBase/Subsystem.hpp"
#include "Lib/EntityDBase/WorldPriv.hpp"

//**********************************************************************************************
//
// CWDbQuerySubsystem implementation.
//

	//******************************************************************************************
	CWDbQuerySubsystem::CWDbQuerySubsystem(const CWorld& w)
		: CWDbQuery< list<CSubsystem*> >(w)
	{
		TListInstance::iterator it_w;

		for (it_w = w.psubList->begin(); it_w != w.psubList->end(); it_w++)
		{
			CSubsystem* psub = ptCast<CSubsystem>(*it_w);

			Assert(psub != 0);

			push_back(psub);
		}

		Begin();
	}
