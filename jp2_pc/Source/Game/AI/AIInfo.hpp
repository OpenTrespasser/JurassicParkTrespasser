/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *			CAIInfo
 *
 * Bugs:
 *
 * To do:
 *		Proper instancing.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/AIInfo.hpp                                                    $
 * 
 * 20    8/26/98 3:15a Agrant
 * protected AI mass
 * 
 * 19    8/10/98 5:23p Rwyatt
 * Removed iTotal and replaced it with a memory counter
 * 
 * 18    7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 17    6/18/98 4:46p Agrant
 * rtAIMass becomes fAIMass
 * 
 * 16    6/09/98 9:48p Agrant
 * removed old GetOpinion code
 * 
 * 15    6/08/98 5:47p Agrant
 * Fixed CAIInfo copy constructor
 * 
 * 14    6/07/98 4:03p Agrant
 * Use AIMass and MoveableMass for pathfinding.
 * 
 * 13    6/04/98 7:11p Agrant
 * major AI specification change
 * 
 * 12    5/16/98 10:00a Agrant
 * Pathfinding fixes for the AI bounding box vs the physics bounding box
 * Major pathfinding fix in general-  how did it ever work???
 * 
 * 11    5/12/98 3:53p Agrant
 * boolean bugs
 * 
 * 10    5/10/98 6:26p Agrant
 * AI's now can ignore selected tangible objects.
 * misc. changes
 * 
 * 9     9/09/97 9:07p Agrant
 * replaced bHackUnique with a set of flags
 * made a copy constructor to fix the -1 AIInfos bug
 * 
 * 8     7/27/97 2:24p Agrant
 * CAIInfo counter
 * 
 * 7     5/15/97 7:05p Agrant
 * new FindShared function for text properties
 * 
 * 6     5/13/97 10:52p Agrant
 * A way to instance AIInfos
 * 
 * 5     5/04/97 9:30p Agrant
 * Reworked the way activities are handled-  each now operates on an
 * InfluenceList.
 * 
 * 4     3/19/97 2:29p Agrant
 * Default to UNKNOWN instead of DONTCARE
 * 
 * 3     11/20/96 1:26p Agrant
 * Now using world database queries for perception.
 * Now using archetypes for default personality behaviors.
 * 
 * 2     11/14/96 4:50p Agrant
 * AI subsystem now in tune with the revised object hierarchy
 * 
 * 1     11/11/96 10:06p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_AIINFO_HPP
#define HEADER_LIB_ENTITYDBASE_AIINFO_HPP

#include "AITypes.hpp"
#include "AIMain.hpp"
#include "Feeling.hpp"
#include "Lib/sys/MemoryLog.hpp"


class CObjectValue;


enum EAIFlags
// Prefix: eaif
{
	eaifUNIQUE = 0,		// True when the AI Info is unique and not instanced.
//	eaifVALID_SURFACE,	// True when the thing can be walked on as a surface.
	eaifIGNORE,			// True if we can completely ignore this thing.
	eaifPATHFINDING,	// True if we use this guy for pathfinding.
	eaifCLIMB,
	eaifEND
};

//**********************************************************************************************
//
class CAIInfo : public CRefObj
//
// Prefix: aii
//
// Class holding shared AI info.
//
//	Notes:
//		Whenever an pointer to an AIInfo us declared as a very temporary variable, it may
//		a simple CAIInfo*.  Otherwise, rptr<CAIInfo> ought to be used.
//
//**************************************
{
public:

	EAIRefType	eaiRefType;		// A type field describing the associated instance
								// as seen by AI's.

	CSet<EAIFlags>	setFlags;	// A set of AI flags.

	CRating			rtDanger;	// A 0-1 rating of the danger of this thing.

protected:
	float			fAIMass;	// A 0-1 rating of the difficulty moving this thing.
								//  OR value of 2.0 or higher means we should take an educated guess 
public:

//	bool bHackUnique;			// True if the info is unique and never shared.
								// This is a hack that will go away when we do the 
								// correct thing about instancing/ref counting.


	//******************************************************************************************
	//
	// Constructors.
	//

	// default constructor
	CAIInfo(EAIRefType eai = eaiUNKNOWN, bool b_hack_unique = true);

	CAIInfo
	(
		const CGroffObjectName*		pgon,
		const ::CHandle&				h_obj,				// Handle to the object in the value table.
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				// the loader.
	);


	// Copy
	CAIInfo(const CAIInfo& aii)
	{
		*this = aii;

		MEMLOG_ADD_COUNTER(emlTotalAIInfo,1);
	}

	~CAIInfo()
	{
		MEMLOG_SUB_COUNTER(emlTotalAIInfo,1);
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	CAIInfo* paiiCopy
	(
	)
	//
	//	Copies this, returning a unique, non-instanced object outside of the instancing system.
	//
	//**************************
	{
		CAIInfo* paii_ret = new CAIInfo();
		*paii_ret = *this;
		paii_ret->setFlags[eaifUNIQUE] = true;
		return paii_ret;
	}


	//*****************************************************************************************
	//
	static const CAIInfo* paiiFindShared
	(
			const CAIInfo& aii
	);
	// Another version of FindShared which uses a base ai info to instance
	//
	//**************************

	//*****************************************************************************************
	//
	static const CAIInfo* paiiFindShared
	(
		const CGroffObjectName*			pgon,
		const ::CHandle&				h_obj,				// Handle to the object in the value table.
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				// the loader.
	);
	// Another version of FindShared which uses a value table entry.
	//
	//**************************

	bool operator< 
	(
		const CAIInfo& aii
	) const;


	//*************************************************************************************
	//
	EAIRefType eaiGetRefType
	(
	)
	//
	//	Gets the type of the entity, as perceived by animals' AI models.
	//
	//******************************
	{
		return eaiRefType;
	}

	//****************************************************************************
	//
	bool bIgnore() const
	//
	//	true if AI system should pay attention to this instance
	//
	//******************************
	{
		return setFlags[eaifIGNORE];
	}

	//*************************************************************************************
	//
	bool bPathfinding() const
	//
	//	true if instance can block passage
	//
	//******************************
	{
		return setFlags[eaifPATHFINDING];
	}


	//*************************************************************************************
	//
	bool bClimb() const
	//
	//	true if AI's can climb on instance
	//
	//******************************
	{
		return setFlags[eaifCLIMB];
	}

	//*************************************************************************************
	//
	const CBoundVol* pbvGetBoundVol
	(
		const CInstance* pins
	) const;
	//
	//	Returns the bound vol for AI purposes.
	//
	//*******************************************8

	//*************************************************************************************
	//
	CRating rtGetAIMass
	(
		const CInstance* pins
	) const;
	//
	//	Gets the AIMass of the entity, as perceived by animals' AI models.
	//
	//******************************

	//*************************************************************************************
	//
	void SetAIMass
	(
		float f
	)
	{
		Assert(f == 2.0f || (f >= 0.0f && f <= 1.0f));	
		fAIMass = f;
	}


#if VER_TEST
	//*****************************************************************************************
	//
	int iGetDescription
	(
		char *	pc_buffer,
		int		i_buffer_len
	) const;
	//
	//	Gets a text description of the AI attribs.
	//
	//*******************************************8
#endif


};



//#ifndef HEADER_LIB_ENTITYDBASE_AIINFO_HPP
#endif
