/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Subsystem responsible for rendering the contents of the world database.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/RenderDB.hpp                                          $
 * 
 * 16    8/11/98 8:27p Mlange
 * Entities must now register themselves with each message type in which they are interested.
 * 
 * 15    8/11/98 2:15p Agrant
 * allow saving animated meshes
 * 
 * 14    7/28/98 4:23p Pkeet
 * Made the page frame delay static.
 * 
 * 13    98.06.25 8:45p Mmouni
 * Added stuff to support render quality setting.
 * 
 * 12    6/24/98 3:22p Rwyatt
 * New member function to disable threaded VM for a given number of frames
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_RENDERDB_HPP
#define HEADER_LIB_ENTITYDBASE_RENDERDB_HPP

#include "Lib/EntityDBase/Subsystem.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include <list>

class CMeshAnimating;

typedef std::list<CMeshAnimating*> LPMA;

//**********************************************************************************************
//
// Declarations for CRenderDB.
//
class CMessagePaint;

//**********************************************************************************************
//
class CRenderDB : public CSubsystem
//
// Subsystem for rendering the contents of the world database.
//
// Prefix: rdb
//
//**************************************
{
	const CWorld& wDBase;	//lint !e1725  // The world database this subsystem is contained in.

	char *pc_defaults;		// Buffer containing default values for settings.

	static uint32	u4NoPageFrames;	// Frames rendered before the RenderDBase enables VM paging.

public:
	LPMA lpmaAnimatedMeshes;	// A list of all animated meshes for savegame purposes.

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//
	CRenderDB(const CWorld& w = wWorld);

	~CRenderDB();

	//*****************************************************************************************
	static void SetNoPageFrames(uint32 u4_frames);

	//*****************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	void Process(const CMessagePaint& msgpaint) override;  //lint !e1411

	//******************************************************************************************
	void Process(const CMessageMove& msgmv) override;

	//******************************************************************************************
	void Process(const CMessageSystem& msg_system) override;

	//*****************************************************************************************
	char* pcSave(char*  pc) const override;

	//*****************************************************************************************
	const char* pcLoad(const char*  pc) override;

	//*****************************************************************************************
	//
	void SaveDefaults() override;
	//
	// Save the default values of the user modifiable settings.
	//
	//**********************************

	//*****************************************************************************************
	//
	void RestoreDefaults() override;
	//
	// Restore the default values of the user modifiable settings.
	//
	//**********************************
};


//
// Global variables.
//

// Flag for turning terrain off.
extern bool bRenderTerrain;

#endif
