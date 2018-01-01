/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CActivity child classes for head movement behaviors.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/HeadActivities.hpp                                            $
 * 
 * 16    9/06/98 8:15p Agrant
 * cock head activity
 * 
 * 15    9/06/98 7:49p Agrant
 * added some head cock 
 * 
 * 14    8/26/98 7:14p Agrant
 * Sniff rework
 * 
 * 13    8/23/98 2:22p Agrant
 * do not glare at something to close
 * 
 * 12    8/22/98 7:18p Agrant
 * lookaround responds to startle flag
 * 
 * 11    8/21/98 12:43a Agrant
 * sniff air implementation
 * 
 * 10    7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 9     5/19/98 9:17p Agrant
 * The LookAround activity
 * 
 * 8     5/17/98 9:31p Agrant
 * moved constructors to .cpp file
 * 
 * 7     5/16/98 11:50p Agrant
 * many tweaks
 * 
 * 6     2/12/98 1:11p Agrant
 * Added descriptions.
 * 
 * 5     2/03/98 4:33p Agrant
 * Updated some dino resource requirements.
 * 
 * 4     1/30/98 6:19p Agrant
 * Resources adjusted for LookAt
 * 
 * 3     1/08/98 6:06p Agrant
 * pseudocode implementations of head activities
 * 
 * 2     1/07/98 5:12p Agrant
 * Shell of future activities
 * 
 * 1     12/22/97 6:09p Agrant
 * initial rev
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_HEADACTIVITIES_HPP
#define HEADER_GAME_AI_HEADACTIVITIES_HPP

//#include "Lib\Std\Random.hpp"

#include "Activity.hpp"
#include "ActivityVocal.hpp"

//*********************************************************************************************
//
class CActivityLookAt : public CActivity
//
//	Prefix: ala
//
//*********************************************************************************************
{

public:
		TReal rHeadCock;			// The head cock angle.

	//
	//  Member function definitions.
	//

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

	CActivityLookAt();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};

//*********************************************************************************************
//
class CActivitySniffAir : public CActivityVocal
//
//	Prefix: ala
//
//*********************************************************************************************
{
	CVector3<> v3StartPosition;	// Where the head started out.
	TReal rHeadCock;			// The head cock angle.

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivitySniffAir(char *pc_name = "Sniff Air");
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};



//*********************************************************************************************
//
class CActivityTaste : public CActivity
//
//	Prefix: ala
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

	CActivityTaste();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};


//*********************************************************************************************
//
class CActivitySniffTarget : public CActivitySniffAir
//
//	Prefix: ast
//
//*********************************************************************************************
{
public:
	TSec sStartSniffing;	// When did we begin our exploratory sniff?


	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivitySniffTarget();
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};


//*********************************************************************************************
//
class CActivityCockHead : public CActivity
//
//	Prefix: ala
//
//*********************************************************************************************
{

public:
		TReal rHeadCock;			// The head cock angle.


	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityCockHead();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf);
		
		//*********************************************************************************
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};


//*********************************************************************************************
//
class CActivityGlare : public CActivity
//
//	Prefix: ala
//
//*********************************************************************************************
{

public:
		TReal rHeadCock;			// The head cock angle.
//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityGlare();
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf);
		
		//*********************************************************************************
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};


//*********************************************************************************************
//
class CActivityLookAround : public CActivity
//
//	Prefix: ala
//
//*********************************************************************************************
{
	CVector3<> v3Target;	// Where the head started out.
	TReal rHeadCock;			// The head cock angle.

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityLookAround();
	

	//*****************************************************************************************
	//
	//	Overrides.
	//

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf);

		//*********************************************************************************
		virtual void Act(CRating	rt_importance, CInfluence* pinf);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};


//#ifndef HEADER_GAME_AI_HEADACTIVITIES_HPP
#endif