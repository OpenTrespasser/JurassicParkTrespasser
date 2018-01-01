/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * A file to allow porting of the world dbase library.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/WorldPort.cpp                                         $
 * 
 * 2     9/23/97 7:26p Agrant
 * Fixed compile bugs
 * 
 * 1     9/23/97 6:03p Agrant
 * initial rev
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "GblInc/Common.hpp"
#include "WorldDBase.hpp"

#include "Instance.hpp"



//
//	CWorld implementation.
//

	//*********************************************************************************************
	bool CWorld::bSelected(CInstance* pins)
	{
		return false;
	}

	//*********************************************************************************************
	int CWorld::iSelectedCount() const
	{
		return 0;
	}

	//*********************************************************************************************
	CInstance* CWorld::pinsLastSelected
	(
		int i_index 					// Which object to get from the list.
										// 0 means last selected, 1 means next last, etc.
	)
	{
		return 0;
	}

	//*********************************************************************************************
	void CWorld::Select
	(
		CInstance* pins,				// Object to select.
		bool b_augment					// Adds it to list; otherwise, selects exclusively.
	)
	{
	}

	//*****************************************************************************************
	//
	bool CWorld::bSelect
	(
		int i_x, int i_y,					// Position of mouse cursor on screen.
		bool b_augment						// As above.
	)
	{
		return false;		
	}

	//*****************************************************************************************
	int iDeleteSelected
	(
	)
	{
		return 0;	
	}
