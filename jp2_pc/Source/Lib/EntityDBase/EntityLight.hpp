/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * CEntityLight
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/EntityLight.hpp                                       $
 * 
 * 10    98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 9     11/10/97 6:07p Agrant
 * Save/Load functions
 * 
 * 8     97/09/08 17:27 Speter
 * CEntityLight can no longer have children.
 * 
 * 7     97/07/03 17:15 Speter
 * Made bIncludeInBuildPart() const. 
 * 
 * 6     97/07/01 11:15a Pkeet
 * Added a unique name for the class. Added flag to not include in partition building.
 * 
 * 5     6/18/97 2:57a Agrant
 * Directional lights loaded kill the global first light when they load, to avoid
 * having an unspecified light in the scene.
 * 
 * 4     5/30/97 11:08a Agrant
 * LINT tidying.
 * 
 * 3     97-05-12 11:14 Speter
 * Updated for CEntityAttached changes.
 * 
 * 2     97-04-04 12:38 Speter
 * A big change: now pipeline uses CRenderPolygon rather than SRenderTriangle (currently always
 * as a triangle).  Changed associated variable names and comments.
 * 
 * 1     97-04-03 19:51 Speter
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_ENTITYLIGHT_HPP
#define HEADER_LIB_ENTITYDBASE_ENTITYLIGHT_HPP

#include "Lib/EntityDBase/Entity.hpp"
// #include "Lib/EntityDBase/Message.hpp"


//*********************************************************************************************
//
// Opaque type declarations for CEntityLight.
//

class CLight;

//*********************************************************************************************
//
class CEntityLight: public CEntityAttached
//
// Points to a light, responds to world events.
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors.
	//

	CEntityLight
	(
		rptr<CLight> plt,					// The light object for this entity.
		CInstance* pins_parent = 0			// The controlling parent instance.
	);

	//******************************************************************************************
	//
	// Overrides.
	//

	// Invalidate shadow buffer when objects added.
	void Process(const CMessageCreate& msgcreate) override;  //lint !e1411  //Yes, this had the same name as other functions.

	// Invalidate shadow buffer when I myself am moved.
	void Move(const CPlacement3<>& p3_new, CEntity* pet_sender = 0) override;

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	rptr<CLight> pltLight() const;
	//
	// Returns:
	//		The pointer to the light render type, without all the casting hassle.
	//
	//**********************************


	//******************************************************************************************
	//
	// Overrides
	//

	//*****************************************************************************************
	virtual bool bIncludeInBuildPart() const override
	{
		return false;
	}

	//*****************************************************************************************
	virtual bool bCanHaveChildren() override
	{
		return false;
	}

	//*****************************************************************************************
	virtual const char* strPartType() const override;

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const override;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer) override;


};

//  Keep this so we can alter it if necessary.
extern CEntityLight*	petltLightDirectionalDefault;
extern CInstance*		pinsLightDirectionalDefaultShape;


#endif

