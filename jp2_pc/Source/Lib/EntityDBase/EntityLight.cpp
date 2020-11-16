/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of EntityLight.hpp
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/EntityLight.cpp                                       $
 * 
 * 10    98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 9     98/02/18 10:35 Speter
 * Removed CInstance::ppr3Presence() and replaced with inline pr3GetPresence() and
 * p3GetPlacement().
 * 
 * 8     11/10/97 6:08p Agrant
 * Placeholder save/load, intermediate check-in
 * 
 * 7     9/29/97 6:00p Agrant
 * Use the ppr3Presence() accessor function.
 * 
 * 6     97/07/07 14:24 Speter
 * UpdateShadows() now takes single top-level partition, rather than a bool and a bounding vol.
 * 
 * 5     97/07/01 11:15a Pkeet
 * Added a unique name for the class. Added flag to not include in partition building.
 * 
 * 4     6/18/97 2:57a Agrant
 * Directional lights loaded kill the global first light when they load, to avoid
 * having an unspecified light in the scene.
 * 
 * 3     5/30/97 11:08a Agrant
 * LINT tidying.
 * 
 * 2     97-05-12 11:14 Speter
 * Updated for CEntityAttached changes.
 * 
 * 1     97-04-03 19:02 Speter
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "EntityLight.hpp"
#include "Lib/Renderer/Light.hpp"

//*********************************************************************************************
//
// CEntityLight implementation.
//

CEntityLight*	petltLightDirectionalDefault = 0;
CInstance*		pinsLightDirectionalDefaultShape = 0;

	//******************************************************************************************
	CEntityLight::CEntityLight(rptr<CLight> plt, CInstance* pins_parent) :
		CEntityAttached
		(
			SInit(rptr_cast(CRenderType, plt)), 
			pins_parent
		)
	{
		// Point the light at our presence.
		plt->ppr3Presence = &pr3GetPresence();
	}

	//******************************************************************************************
	void CEntityLight::Process
	(
		const CMessageCreate& //msgcreate
	)
	{
		rptr<CLight> plt = pltLight();

		// Invalidate shadow buffer when objects added.
		plt->UpdateShadows(0);
	}

	//******************************************************************************************
	void CEntityLight::Move(const CPlacement3<>& p3_new, CEntity* pet_sender)
	{
		CEntityAttached::Move(p3_new, pet_sender);

		// Invalidate shadow buffer when I myself am moved.
		rptr<CLight> plt = pltLight();		
		plt->UpdateShadows(0);
	}
	
	//******************************************************************************************
	rptr<CLight> CEntityLight::pltLight() const
	{
		Assert(ptCastRenderType<CLight>(prdtGetRenderInfo()));

		rptr<CRenderType> rpdt = rptr_nonconst(prdtGetRenderInfo());
		
		rptr<CLight> rplt = rptr_static_cast(CLight, rpdt);

		return rplt;
			
//		return rptr_static_cast(CLight, rptr_nonconst(prdtGetRenderInfo()));
	}

	//*****************************************************************************************
	const char* CEntityLight::strPartType() const
	{
		return "Light";
	}

	//*****************************************************************************************
	char *CEntityLight::pcSave(char * pc_buffer) const
	{
		return CInstance::pcSave(pc_buffer);
	}

	//*****************************************************************************************
	const char*CEntityLight::pcLoad(const char* pc_buffer)
	{
		return CInstance::pcLoad(pc_buffer);
	}
