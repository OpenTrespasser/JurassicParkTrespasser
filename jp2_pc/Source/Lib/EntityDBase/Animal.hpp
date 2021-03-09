/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CAnimal
 *
 * Bugs:
 *
 * To do:
 *		Figure out how to use opaque types with CRPtr for reference counting.
 *		Remove accessor functions to 'pShapeInfo' in 'CAnimateShape.'
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Animal.hpp                                            $
 * 
 * 29    8/31/98 5:06p Agrant
 * Double wake up distance for dinos in view cone
 * 
 * 28    98/08/25 19:22 Speter
 *  Removed unused HandleColliision; added pins_me to HandleDamage. 
 * 
 * 27    8/20/98 11:04p Agrant
 * A tail position accessor function
 * 
 * 26    7/29/98 3:05p Agrant
 * handle damage function now includes an aggressor field
 * 
 * 25    7/20/98 10:13p Rwyatt
 * Removed description text in final mode
 * 
 * 24    6/20/98 2:42p Agrant
 * added placeholder pinsCopy functions
 * 
 * 23    6/09/98 9:33p Agrant
 * Dinos notice death
 * 
 * 22    6/04/98 11:04p Agrant
 * moved boundary conditions into CAnimate
 * 
 * 21    6/03/98 2:59p Agrant
 * Support for boundary condition instances
 * 
 * 20    5/22/98 3:43p Agrant
 * Revised the way we handle damage to animals and humans
 * 
 * 19    2/21/98 5:52p Rwyatt
 * Removed all traces of the audio response message. It is no longer required.
 * 
 * 18    2/05/98 5:06p Agrant
 * head position query
 * 
 * 17    1/16/98 8:45p Agrant
 * Added SetEmotions function to give outside systems control over the AI.
 * Probably should be message controlled.
 * 
 * 16    12/20/97 6:58p Agrant
 * iGetDescription and better CBrain constructor
 * 
 * 15    12/08/97 9:41p Agrant
 * Text prop constructors
 * 
 * 14    11/10/97 6:07p Agrant
 * Save/Load functions
 * 
 * 13    7/19/97 1:27p Agrant
 * Pass AudioResponse messages up to the brain.
 * 
 * 12    5/26/97 1:41p Agrant
 * LINT minor fixes.
 * 
 * 11    5/06/97 7:52p Agrant
 * Delete that brain.
 * 
 * 10    3/28/97 1:59p Agrant
 * Animals handle delete messages.
 * 
 * 9     3/19/97 2:25p Agrant
 * Moved constructors to .cpp file from .hpp file
 * 
 * 8     2/09/97 8:12p Agrant
 * Added move message handler.
 * 
 * 7     96/12/11 16:59 Speter
 * Replaced constructors taking individual Info pointers with single constructor taking
 * CInstance::SInit.
 * 
 * 6     11/21/96 4:21p Mlange
 * Now uses CRenderType as a replacement of CShapeInfo.
 * 
 * 5     11/18/96 4:07p Pkeet
 * Altered CAnimal to inherit from 'CAnimate.' Added the 'HandleCollision' function.
 * 
 * 4     11/16/96 4:11p Mlange
 * Updated identifier function and some comments.
 * 
 * 3     11/14/96 11:13p Agrant
 * Collision message processing.
 * Remove PRS Init from constructor.
 * 
 * 2     11/14/96 4:48p Agrant
 * Animals handle physicsrequest messages by passing them to their physics models
 * 
 * 1     11/11/96 9:49p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_ANIMAL_HPP
#define HEADER_LIB_ENTITYDBASE_ANIMAL_HPP

#include "Animate.hpp"

class CBrain;
class CFeeling;
class CMessageDelete;
class CMessageAudioResponse;

//**********************************************************************************************
//
class CAnimal: public CAnimate
//
// Prefix: ani
//
// Notes:
//		An entity that has an AI model attached.
//
//**************************************
{
public:
	CBrain*		pbrBrain;		// The AI model associated with the animal.

	//******************************************************************************************
	//
	// Constructors.
	//

	CAnimal();

	// The basic CInstance constructor for loading from a GROFF file.
	CAnimal
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CAnimal();
		
	//******************************************************************************************
	//
	// Functions.
	//

	//*****************************************************************************************
	//
	void SetEmotions
	(
		const CFeeling& feel
	);
	//
	//	Sets the animal's emotional state to "feel".  If a parameter in "feel" is negative, that 
	//	emotion is not set.
	//
	//***************************

	//******************************************************************************************
	CVector3<> v3GetHeadPos
	(
	) const;
	//
	//	Gets the position of the animal's head in world coords.  The position corresponds to the
	//	location which it would bite if it were to close its mouth.
	//
	//***************************

	//******************************************************************************************
	CVector3<> v3GetTailPos
	(
	) const;
	//
	//	Gets the position of the end of the animal's tail in world coords.
	//
	//***************************

	//******************************************************************************************
	//
	// Overides.
	//

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const override;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer) override;


	//
	// Message processing overrides.
	//

	//*****************************************************************************************
	virtual void PreRender
	(
		CRenderContext& renc			// Target, camera, settings, etc.
	) override;

	//******************************************************************************************
	virtual void Process
	(
		const CMessagePhysicsReq& msgpr
	) override;

	//lint -save -e1411
	//******************************************************************************************
	virtual void Process
	(
		const CMessageCollision& msgcoll
	) override;

	//******************************************************************************************
	virtual void Process
	(
		const CMessageMove& msgmv
	) override;

	//******************************************************************************************
	virtual void Process
	(
		const CMessageDelete& msgdel
	) override;

	//******************************************************************************************
	virtual void Process
	(
		const CMessageDeath& msgdeath
	) override;

	//*****************************************************************************************
	virtual void HandleDamage
	(
		float f_damage, const CInstance* pins_aggressor = 0, const CInstance* pins_me = 0
	) override;

	//*****************************************************************************************
	virtual CInstance* pinsCopy
	(
	) const override;		// Makes a copy of this

	//
	// Identifier functions.
	//

	//*****************************************************************************************
	virtual void Cast(CAnimal** ppani) override
	{
		*ppani = this;
	}
	//lint -restore

};

//#ifndef HEADER_LIB_ENTITYDBASE_ANIMAL_HPP
#endif
