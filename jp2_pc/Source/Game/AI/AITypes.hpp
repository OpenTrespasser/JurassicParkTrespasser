/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Non-class type definitions for AI library.
 *		
 *
 * Bugs:
 *
 * To do:
 *			
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/AITypes.hpp                                                   $
 * 
 * 24    8/27/98 1:44a Agrant
 * added tear sound
 * 
 * 23    8/13/98 6:56p Agrant
 * code to allow dinos to ignore influences that are unattainable
 * 
 * 22    8/09/98 7:58p Rwycko
 * Added new Dinosaur type for Albertosaur
 * 
 * 21    6/26/98 7:23p Agrant
 * Added eating and drinking sounds to vocal types.
 * 
 * 20    6/18/98 4:47p Agrant
 * Added bite vocal type
 * 
 * 19    6/14/98 8:40p Agrant
 * added an attack vocal type, for use during attacks
 * 
 * 18    6/04/98 7:11p Agrant
 * major AI specification change
 * 
 * 17    5/29/98 3:22p Agrant
 * more vocals, dinos, archetypes
 * 
 * 16    5/08/98 1:02a Agrant
 * Vocals specified in data.
 * 
 * 15    1/30/98 6:19p Agrant
 * fixed indexing for EDinoResources
 * 
 * 14    12/20/97 7:02p Agrant
 * Moved activity enum to another file
 * added the Vegetable AI type (does nothing)
 * 
 * 13    12/18/97 7:36p Agrant
 * New subbrains, test and emotion
 * 
 * 12    7/19/97 1:28p Agrant
 * Added the voice dino resource.
 * 
 * 11    4/29/97 6:42p Agrant
 * CFeeling now float-based instead of CRating-based.
 * Activities now rate/act based on a list of influences.
 * Better rating functions for activities.
 * Debugging tools for activity processing.
 * 
 * 10    4/08/97 6:42p Agrant
 * Debugging improvements for AI Test app.
 * Allows test app treatment of GUIApp AI's.
 * Better tools for seeing what is going on in an AI's brain.
 * 
 * 9     12/11/96 1:28p Agrant
 * added humans to ai ref type
 * 
 * 8     11/20/96 1:26p Agrant
 * Now using world database queries for perception.
 * Now using archetypes for default personality behaviors.
 * 
 * 7     11/02/96 7:14p Agrant
 * added attack sub brain.
 * 
 * 6     10/15/96 9:20p Agrant
 * Synthesizer reworked to be better, stronger, smarter, faster than before.
 * Activity Rate and Act and Register functions now have better defined roles.
 * Added some accessor functions to smooth future changes.
 * 
 * 5     10/10/96 7:18p Agrant
 * Modified code for code specs.
 * 
 * 4     9/30/96 3:05p Agrant
 * Modified to fit coding spec.
 * 
 * 3     9/26/96 5:58p Agrant
 * basic AI structure done
 * AI system adapted to TReal for world coords
 * AI Test App exhibits very basic herding with two very simple actvities.
 * 
 * 2     9/23/96 2:59p Agrant
 * Added dino resources, physics primitives, and sub-brains.
 * 
 * 1     9/03/96 8:12p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_AITYPES_HPP
#define HEADER_LIB_AI_AITYPES_HPP

enum EParameterType
//	Prefix: ept
//  Indexes the parameters for reference purposes.
//
//	Notes:
//		The meanings of each parameter vary slightly depending upon context.  For more detail,
//		see Feeling.hpp.
{
	eptFEAR=0,		// How afraid is the animal?
	eptLOVE,		// How friendly is it feeling?
	eptANGER,		// How angry is it?
	eptCURIOSITY,	// How curious is it?
	eptHUNGER,		// How hungry is it?
	eptTHIRST,		// How thirsty is it?
	eptFATIGUE,		// How tired is it?
	eptPAIN,		// How much pain/injury is it feeling?
	eptSOLIDITY,	// How worried is it about running into solid objects?
					//	This one doesn't make sense except in an Influence.
	eptEND
};


enum EPerceptionType
//	Prefix: epct
//  Indexes the perceptions for reference purposes.
{
	epctINJURED=0,	// When the animal gets hurt.
	epctEND
};


enum EDinoResource
//	Prefix: edr
//
//  Indexes various dinosaur Resources
//
{
	edrBEGIN=0,
	edrHEAD = edrBEGIN,	// Head and neck of the dinosaur.
	edrTAIL,			// The tail.
	edrFORELEGS,		// The forelegs, and torso to some extent.
	edrHINDLEGS,		// The hind legs and pelvis.
	edrVOICE,			// The vocal chords.
	edrEND
};

enum EPhysicsPrimitive
//	Prefix: epp
//  Indexes various dinosaur actions as seen from a physics point of view.
{
	eppNONE=0,
	eppMOVE,	// Tell the animal to walk/run in a direction.
	eppEND
};

enum ESubBrainType
//	Prefix: esbt
//
//  Indexes sub brains by the type of activity they promote
//
{
	esbtMOVEMENT=0,	// The sub-brain handling simple movement.
	esbtATTACK,
	esbtTEST,
	esbtEMOTION,
	esbtEND
};

enum EArchetype
//	Prefix: ear
//  Personality styles for AI models.
{
	earBEGIN=0,
	earCARNIVORE=earBEGIN,		
	earHERBIVORE,			

	earEND				// End index.
};

enum EAIRefType
//	Prefix: eai
//  Type of an entity, from an animal's point of view.
{
	eaiDONTCARE=0,	// Entity is ignored.
	eaiUNKNOWN,		// Mysterious entity.
	eaiANIMAL,
	eaiMEAT,		// Piece of meat.
	eaiGRASS,		// A morsel of yummy grass.
	eaiHUMAN,		// A human
	eaiNODEHINT,	// A good spot for a pathfinding node.
	eaiWATER,

	eaiEND
};

enum EDinoType
//  Prefix: edt
//  Physical type of dinosaur.
{
	edtRAPTOR=0,
	edtTREX,
	edtPARA,
	edtTRIKE,
	edtSTEG,
	edtBRONTO,
	edtALBERT,
	edtEND
};

enum EVocalType
//  Prefix: evt
//  Type of dinosaur vocal.
{
	evtOUCH=0,	// Vocalize pain
	evtHELP,	// Vocalize fear
	evtHOWL,	// Boredom/frustration vocalization
	evtSNARL,	// Angry vocalization
	evtCROON,	// Hunting vocalization
	evtWHIMPER,
	evtDIE,
	evtATTACK,	// A threatening sound made while initiating an attack
	evtBITE,	// The actual sounds of biting (but not necessarily connecting)
	evtEAT,
	evtDRINK,
	evtSWALLOW,
	evtSNIFF,
	evtTEAR,
	evtEND
};


	
#endif  // #ifndef HEADER_LIB_AI_AITYPES_HPP
