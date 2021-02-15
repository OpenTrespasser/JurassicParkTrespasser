/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		Prime camera for image caches and terrain textures.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/CameraPrime.hpp                                       $
 * 
 * 3     10/03/98 4:40a Rwyatt
 * Added new member definition
 * 
 * 2     9/16/98 3:49p Mlange
 * Now delays camera priming until next paint message to ensure screen surfaces have been
 * created.
 * 
 * 1     9/14/98 2:07p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_CAMERAPRIME_HPP
#define HEADER_LIB_ENTITYDBASE_CAMERAPRIME_HPP

#include "Lib/EntityDBase/Subsystem.hpp"

class CMessageSys;
class CMessagePaint;

//**********************************************************************************************
//
class CCameraPrime : public CSubsystem
//
// Prime camera for image caches and terrain textures.
//
// Prefix: camp
//
//**************************************
{
	bool bPrimeOnPaint;

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
public:
	CCameraPrime();

	~CCameraPrime();


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Prime() const;
	//
	// Primes the camera.
	//
	//**************************************

	//*****************************************************************************************
	void BuildInitialBumpTables();
	//
	// Builds bump tables based on the current light position, if the light changes then these
	// tables are rebuilt on the fly.
	//
	//**************************************


	//******************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual void Process(const CMessagePaint&) override;
	virtual void Process(const CMessageSystem&) override;
};

// Single global instance.
extern CCameraPrime* pCameraPrime;

#endif