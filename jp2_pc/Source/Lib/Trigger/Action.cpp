/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CAction::CreateTriggerAction
 *		CVoiceOverAction
 *
 * Bugs:
 *
 * To do:
 *		VoiceOvers/Ambient/Music triggers all use exaclty the same init/start code, this ia
 *		a waste as the code is quite big.
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/Action.cpp                                               $
 * 
 * 100   10/08/98 7:34p Rwycko
 * allow player as an emitter
 * 
 * 99    9/30/98 8:09p Agrant
 * handle damage in set animate triggers
 * 
 * 98    9/24/98 11:18a Rwyatt
 * sequence the text overlays
 * 
 * 97    9/23/98 11:25p Agrant
 * Do not speak while you are dead.
 * 
 * 96    9/21/98 1:06a Agrant
 * added water disturbance action
 * 
 * 95    9/18/98 2:03a Rwyatt
 * Text Action has a time period.
 * Texr Action default colour is now white
 * 
 * 94    9/17/98 3:52p Rwyatt
 * New action for text overlay
 * 
 * 93    9/10/98 12:03a Agrant
 * added player comtrol action and set ai system action
 * 
 * 92    9/08/98 4:26p Mlange
 * Some extra teleport action text prop parsing asserts.
 * 
 * 91    9/06/98 6:16p Rwycko
 * changed validatation of emitters
 * 
 * 90    98.09.03 5:03p Mmouni
 * Added end game action.
 * 
 * 89    9/02/98 2:22p Rwyatt
 * Fixed effect action and added loop support
 * 
 * 88    9/01/98 2:54p Rwyatt
 * Added props for attaching/positioning sound effects with instances.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Action.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "GameActions.hpp"
#include "Trigger.hpp"

#include "Lib\View\Raster.hpp"
#include "Lib/Loader/LoadTexture.hpp"
#include "Game\DesignDaemon\Daemon.hpp"
#include "Game\DesignDaemon\Player.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgAudio.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/EntityDBase/Query/QPhysics.hpp"
#include "Lib/EntityDBase/AnimationScript.hpp"
#include "Lib\GeomDBase\Partition.hpp"
#include "Lib/Audio/AudioDaemon.hpp"
#include "Lib/Audio/AudioLoader.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib\Renderer\Fog.hpp"
#include "Lib/Renderer/Sky.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib\Renderer\RenderCacheInterface.hpp"
#include "Lib/Renderer/DepthSort.hpp"
#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib\GeomDBase\TerrainTexture.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Renderer/LightBlend.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/EntityDBase/Water.hpp"
#include "Lib/EntityDBase/Animate.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/EntityDBase/TextOverlay.hpp"
#include "Lib/Std/Hash.hpp"

// The following mess is for the perspective settings.
#include "Lib/Types/FixedP.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"
#include "Lib/Renderer/Primitives/Walk.hpp"
#include "Lib/Renderer/Primitives/IndexT.hpp"
#include "Lib/Renderer/Primitives/IndexPerspectiveT.hpp"
#include "Lib/Audio/Audio.hpp"

//*********************************************************************************************
// Static function that create any action from text props, the action class needs to be
// de-referenced by the calling function, only text props within the specifed class will be 
// processed. The parent class should be an action array element.
//
void CAction::CreateTriggerAction
(
	CTrigger*			ptr_trig,
	const CGroffObjectName*	pgon,			// Object to load.
	CObjectValue*		poval_action,		// Pointer to the actual action class
	CValueTable*		pvtable,			// Pointer to the value table.
	CLoadWorld*			pload				// the loader.
)
//	
//**************************************
{
	rptr<CAction>	pac_action = rptr0;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		int i_action_type;

		if (bFILL_INT(i_action_type, esTActionType))
		{
			switch (i_action_type)
			{
			case eaACTION_VOICEOVER:
				pac_action = rptr_cast(CAction, rptr_new CVoiceOverAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_AMBIENT:
				// Ambients add themselves to triggers
				pac_action = rptr_cast(CAction, rptr_new CAmbientAction(ptr_trig, pgon, poval_action, pvtable, pload));
				pac_action = rptr0;
				break;

			case eaACTION_MUSIC:
				pac_action = rptr_cast(CAction, rptr_new CMusicAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_FADE_MUSIC:
				pac_action = rptr_cast(CAction, rptr_new CFadeMusicAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SHOW_OVERLAY:
				pac_action = rptr_cast(CAction, rptr_new CShowOverlayAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_FOG:
				pac_action = rptr_cast(CAction, rptr_new CSetFogAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_RENDERER:
				pac_action = rptr_cast(CAction, rptr_new CSetRendererAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_TERRAIN:
				pac_action = rptr_cast(CAction, rptr_new CSetTerrainAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_IMAGECACHE:
				pac_action = rptr_cast(CAction, rptr_new CSetImageCacheAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_AI:
				pac_action = rptr_cast(CAction, rptr_new CSetAIAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_PHYSICS:
				pac_action = rptr_cast(CAction, rptr_new CSetPhysicsAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SUBSTITUTE_MESH:
				pac_action = rptr_cast(CAction, rptr_new CSubstituteMeshAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_DEPTHSORT:
				pac_action = rptr_cast(CAction, rptr_new CSetSortAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_SKY:
				pac_action = rptr_cast(CAction, rptr_new CSetSkyAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_ALPHA_WATER:
				pac_action = rptr_cast(CAction, rptr_new CSetAlphaWaterAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_ENABLE_WATER:
				pac_action = rptr_cast(CAction, rptr_new CWaterOnOffAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_LOAD_LEVEL:
				pac_action = rptr_cast(CAction, rptr_new CLoadLevelAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_ANIMATE_PROPERTIES:
				pac_action = rptr_cast(CAction, rptr_new CSetAnimatePropertiesAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_TELEPORT:
				pac_action = rptr_cast(CAction, rptr_new CTeleportAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SAVE_LEVEL:
				pac_action = rptr_cast(CAction, rptr_new CSaveLevelAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_MAGNET:
				pac_action = rptr_cast(CAction, rptr_new CMagnetAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_ANIMATE_TEXTURE:
				pac_action = rptr_cast(CAction, rptr_new CAnimateTextureAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_HIDESHOW:
				pac_action = rptr_cast(CAction, rptr_new CHideShowAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SOUND_EFFECT:
				pac_action = rptr_cast(CAction, rptr_new CSoundEffectAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_WAKE_AI:
				pac_action = rptr_cast(CAction, rptr_new CWakeAIAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_DELAY:
				pac_action = rptr_cast(CAction, rptr_new CDelayAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SCRIPTED_ANIMATION:
				pac_action = rptr_cast(CAction, rptr_new CScriptedAnimationAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_VARIABLE_TRIGGER:
				pac_action = rptr_cast(CAction, rptr_new CSetVariableTriggerAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SET_HINT:
				pac_action = rptr_cast(CAction, rptr_new CSetHintAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_AUDIO_ENVIRONMENT:
				pac_action = rptr_cast(CAction, rptr_new CAudioEnvironmentAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;

			case eaACTION_SUBSTITUTE_AI:
				pac_action = rptr_cast(CAction, rptr_new CSubstituteAIAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;
			
			case eaACTION_END_GAME:
				pac_action = rptr_cast(CAction, rptr_new CEndGameAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;
			case eaACTION_CONTROL_PLAYER:
				pac_action = rptr_cast(CAction, rptr_new CControlPlayerAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;
			case eaACTION_AI_SYSTEM:
				pac_action = rptr_cast(CAction, rptr_new CAISystemAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;
			case eaACTION_TEXT:
				pac_action = rptr_cast(CAction, rptr_new CTextAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;
			case eaACTION_WATER_DISTURBANCE:
				pac_action = rptr_cast(CAction, rptr_new CWaterDisturbanceAction(ptr_trig, pgon, poval_action, pvtable, pload));
				break;
			default:
				// Invalid action type!
				Assert(false);
				break;
			}

			// add the action to the parent trigger
			if (pac_action)
				ptr_trig->Add(pac_action);
		}
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;
}



//*********************************************************************************************
// Validate the action passed in is valid.
//
// NOTE: This function is only present in VER_TEST builds so the trigger should not call it in 
// final builds.
//
bool CAction::bValidateTriggerAction
(
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	const CEasyString* pestr = 0;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		int i_action_type;

		//
		// Perform any action specific validation...
		// 
		if (bFILL_INT(i_action_type, esTActionType))
		{
			switch(i_action_type)
			{
			case eaACTION_SET_AI:
				if (bFILL_pEASYSTRING(pestr, esStayNearTarget))
				{
					if (!bGroffObjectLoaded(pload,pestr->strData()))
						return false;
				}

				if (bFILL_pEASYSTRING(pestr, esStayAwayTarget))
				{
					if (!bGroffObjectLoaded(pload,pestr->strData()))
						return false;
				}
				break;

			case eaACTION_TELEPORT:
				if (bFILL_pEASYSTRING(pestr, esTeleportDestObjectName))
				{
					if (!bInGroff(pload,pestr->strData()))
					{
#if VER_TEST
						char str_buffer[256];
						sprintf(str_buffer, 
								"%s\n\nMissing teleport dest in GROFF:\n%s needs %s", 
								__FILE__, 
								pgon->strObjectName,
								pestr->strData());

						if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
							DebuggerBreak();
#endif
						return false;
					}
				}
				break;

			case eaACTION_MAGNET:
				if (bFILL_pEASYSTRING(pestr, esMasterObject))
				{
					if (!bGroffObjectLoaded(pload,pestr->strData()))
						return false;
				}

				if (bFILL_pEASYSTRING(pestr, esSlaveObject))
				{
					if (!bGroffObjectLoaded(pload,pestr->strData()))
						return false;
				}
				break;				
			}
		}


		//
		// The properties below apply to multiple actions but they only need
		// to be validated if they are present.
		//

		//
		// look for an emitter, if present check it exists
		//
		if (bFILL_pEASYSTRING(pestr, esTActionEmitter))
		{
			if (*pestr != "Player")
			{

				// This is a in groff check instead of a "has been loaded" check because of
				//  the impulse actions which allow $objects as emitters.
				if (!bInGroff(pload,pestr->strData()))
				{
					// Emitter object not found in groff file!
					AlwaysAssert(false);
					return false;
				}
			}
		}


		//
		// check the Target text prop...
		//
		if (bFILL_pEASYSTRING(pestr, esTarget))
		{
			if (!bGroffObjectLoaded(pload,pestr->strData()))
				return false;
		}

		//
		// check the ObjectName text prop...
		//
		if (bFILL_pEASYSTRING(pestr, esObjectName))
		{
			if (!bGroffObjectLoaded(pload,pestr->strData()))
				return false;
		}
	}
	END_OBJECT;
	END_TEXT_PROCESSING;

	return true;
}



//*********************************************************************************************
// VOICE OVER ACTION
//*********************************************************************************************
// Create a voiceover action straight from text props
//
CVoiceOverAction::CVoiceOverAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,			// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	// these props must be present..
	const CEasyString*		pestr_sample= NULL;

	// these props have defaults...
	float				f_volume	= 0.0f;
	float				f_atten		= 1.0f;
	float				f_frustum	= 360.0f;
	float				f_out_vol	= -15.0f;
	int32				i4_spatial	= 0;	// STEREO
	CInstance*			pins_parent = ptr_trig;
	const CEasyString*	pestr_emmit = 0;


	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		// Do we have a sample
		if (!bFILL_pEASYSTRING(pestr_sample, esTActionSample))
		{
			// there is no sample specified
			Assert(0);
		}

		// fill all of the optional data...
		bFILL_INT(i4_spatial, esTActionSpatial);
		bFILL_FLOAT(f_volume, esTActionVolume);
		bFILL_FLOAT(f_atten,  esTActionAtten);
		bFILL_FLOAT(f_out_vol,esTActionOutsideVol);
		bFILL_FLOAT(f_frustum,esTActionFrustum);

		// get the name of the emmiter object, if there is one.
		if (bFILL_pEASYSTRING(pestr_emmit, esTActionEmitter))
		{
			// there is an emmiter so gets its instance
			CInstance* pins = wWorld.ppartPartitionList()->
								pinsFindNamedInstance(pestr_emmit->strData());

			// did not find the object
			Assert(pins);

			// only change the emmiter if we found a valid object
			if (pins)
				pins_parent = pins;
		}
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	// fill the action data with what we got from the text props
	if (pestr_sample)
	{
		sndhndVoiceSample = sndhndHashIdentifier( pestr_sample->strData() );
	}
	else
	{
		// no sample, use the missing sample
		sndhndVoiceSample = sndhndHashIdentifier( "MISSING" );
	}

	fVolume = f_volume;
	u4SpatialType = (uint32)i4_spatial;
	fAtten = f_atten;
	pinsLocation = pins_parent;
	fOutsideVolume = f_out_vol;
	fFrustumAngle = f_frustum;

	// validate the range of all the action text props...
	Assert ((fVolume<=0.0f) && (fVolume>=-100.0f));
	Assert (u4SpatialType<=2);
	Assert (fAtten<10.0f);
	Assert ((fOutsideVolume<=0.0f) && (fOutsideVolume>=-100.0f));
	Assert ((fFrustumAngle>=0.0f) && (fFrustumAngle<=360.0f));
}



//*********************************************************************************************
// The voice over action start function.
//
void CVoiceOverAction::Start
(
)
//	
//**************************************
{
//	dprintf("ACTION: VoiceOverStart [%s]\n",estrSample.strData());

	// Do not play a voiceover if the player is dead.
	if (gpPlayer->bDead())
		return;

	CMessageAudio	maVoice
					(
						sndhndVoiceSample,
						eamVOICEOVER,			// type fo sound
						padAudioDaemon,			// destination of message
						0,						// sender
						pinsLocation,			// parent
						fVolume,				// play volume
						fAtten,					// volume attenuation
						u4SpatialType,			// type of sample positioning
						fFrustumAngle,			// sound frustum angle
						fOutsideVolume,			// outside the frustum volume
						false,					// looped
						-1,						// loop count
						100000.0f				// maximum distance (not used)
					);
	maVoice.Dispatch();
}






//*********************************************************************************************
// AMBIENT LOOP ACTION
//*********************************************************************************************
// ambient message constructor
//
CAmbientAction::CAmbientAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,			// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	// these props must be present..
	const CEasyString*		pestr_sample= NULL;

	// these props have defaults...
	float				f_volume		= 0.0f;
	float				f_atten			= 1.0f;
	float				f_frustum		= 360.0f;
	float				f_max_vol_dist	= 0.0f;
	float				f_out_vol		= -15.0f;
	int32				i4_spatial		= 1;	// PSEUDO_3D
	CInstance*			pins_parent		= ptr_trig;
	const CEasyString*	pestr_emmit		= 0;
	bool				b_loop			= false;
	int32				i4_loop_count	= -1;
	float				f_maxdist		= 1.0f;
	float				f_boundary_vol	= -40.0f;

	fMasterVolumeMin = 0.0f;
	fMasterVolumeMax = 0.0f;

	// By default all ambient sounds can be muted
	bMute = true;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		// fill all of the optional data...
		bFILL_INT(i4_spatial, esTActionSpatial);

		if (i4_spatial == 1)
		{
			// pseudo 3D sounds start at minimum volume
			f_volume = -100.0f;
		}
		else
		{
			// other types of sound
			bFILL_FLOAT(f_volume, esTActionVolume);
		}

		bFILL_FLOAT(f_out_vol,esTActionOutsideVol);
		bFILL_FLOAT(f_frustum,esTActionFrustum);

		// get the loop state
		bFILL_BOOL(b_loop, esTActionSampleLoop);

		// get the mute state
		bFILL_BOOL(bMute, esTActionMute);

		// fill the loop count 
		bFILL_INT(i4_loop_count, esTActionLoopCount);

		// get the name of the emmiter object, if there is one.
		if (bFILL_pEASYSTRING(pestr_emmit, esTActionEmitter))
		{
			// there is an emmiter so gets its instance
			CInstance* pins = wWorld.ppartPartitionList()->
								pinsFindNamedInstance(pestr_emmit->strData());

			// did not find the object
			Assert(pins);

			// only change the emmiter if we found a valid object
			if (pins)
				pins_parent = pins;
		}

		//
		// Get the max distance as a percentage (0.0 being the default so attenuation starts immediately)
		//
		bFILL_FLOAT(f_max_vol_dist,  esTActionMaxVolDist);
		Assert ((f_max_vol_dist>=0.0f) && (f_max_vol_dist<=1.0f));

		if (!bFILL_FLOAT(f_atten,  esTActionAtten))
		{
			bFILL_FLOAT(f_boundary_vol,  esTActionBoundaryVol);

			Assert ((f_boundary_vol<=0.0f) && (f_boundary_vol>=-100.0f));
			// attenuation was not specified so calculate it based
			// on the scale factor of the model and the size of the max volume area
			CPresence3<> pr3 = pins_parent->pr3Presence();
			float f_scale = pr3.rScale;

			f_atten = -f_boundary_vol / ((1.0f-f_max_vol_dist)*f_scale);
		}

		// get the cuttoff distance of the sample
		if (!bFILL_FLOAT(f_maxdist,  esTActionMaxDist))
		{
			// cutoff was not specified so calculate it based
			// on the scale factor of the model
			CPresence3<> pr3 = pins_parent->pr3Presence();
			float f_scale = pr3.rScale;

			// make the cutoff distance 5% bigger than the size
			f_maxdist = f_scale*1.05f;
		}

		bFILL_FLOAT(fMasterVolumeMin,  esTMasterVolumeMin);
		bFILL_FLOAT(fMasterVolumeMax,  esTMasterVolumeMax);

		//
		// These master volume settings are in dBs so must be negative
		//
		Assert(fMasterVolumeMin<=0.0f);
		Assert(fMasterVolumeMax<=0.0f);
		Assert(fMasterVolumeMin>=-100.0f);
		Assert(fMasterVolumeMax>=-100.0f);
		Assert(fMasterVolumeMax>=fMasterVolumeMin);

		fVolume = f_volume;
		u4SpatialType = (uint32)i4_spatial;
		fAtten = f_atten;
		pinsLocation = pins_parent;
		fOutsideVolume = f_out_vol;
		fFrustumAngle = f_frustum;
		bLooped = b_loop;
		i4LoopCount = i4_loop_count;
		fMaxDistance = f_maxdist;
		fMaxVolDistance = f_max_vol_dist * pins_parent->pr3Presence().rScale;

		// validate the range of all the action text props...
		Assert ((fVolume<=0.0f) && (fVolume>=-100.0f));
		Assert (u4SpatialType<=2);
		Assert (fAtten>=0.0f);
		Assert ((fOutsideVolume<=0.0f) && (fOutsideVolume>=-100.0f));
		Assert ((fFrustumAngle>=0.0f) && (fFrustumAngle<=360.0f));
		Assert (fMaxDistance>=0.0f);
		Assert (i4LoopCount!=0);


		TSoundHandle	sndhnd;

		// Do we have a sample
		if (!bFILL_pEASYSTRING(pestr_sample, esTActionSample))
		{
			uint32			u4_count = 0;

			// there is no sample specified is there a sample array??
			while ( bFILL_pEASYSTRING(pestr_sample, (ESymbol)((uint32)esA00+u4_count)) )
			{
				// The first array element is the same as the sample text prop
				if (pestr_sample)
				{
					sndhnd = sndhndHashIdentifier( pestr_sample->strData() );
				}
				else
				{
					// no sample, use the missing sample
					sndhnd = sndhndHashIdentifier( "MISSING" );
				}

				// Add a copy of this action to the trigger
				ptr_trig->Add( rptr_cast(CAction, rptr_new CAmbientAction((*this), sndhnd)) );
				u4_count++;
			}

			// Did we get an array element??
			Assert(u4_count>0);
		}
		else
		{
			if (pestr_sample)
			{
				sndhnd = sndhndHashIdentifier( pestr_sample->strData() );
			}
			else
			{
				// no sample, use the missing sample
				sndhnd = sndhndHashIdentifier( "MISSING" );
			}

			// Add a copy of this action to the trigger
			ptr_trig->Add( rptr_cast(CAction, rptr_new CAmbientAction((*this), sndhnd)) );
		}

	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;
}


//*********************************************************************************************
// ambient action copy constructor
//
CAmbientAction::CAmbientAction
(
	CAmbientAction&		ambact,
	TSoundHandle		sndhnd
)
//	
//**************************************
{
	// This constructor is private and is only called from the constructor above. This constructor
	// will copy the ambient action constructed above. Therefore the ref count of either ambient
	// has not been incremented, iRefCount = 0, because the class has not be assigned to a variable.
	Assert(ambact.uRefs == 0);		// Source ref count must be 0
	Assert(uRefs == 0);				// our ref count must be 0

	memcpy(this,&ambact,sizeof(CAmbientAction));
	sndhndAmbientSample = sndhnd;

	AlwaysAssert(padAudioDaemon);
	if (padAudioDaemon->padatAmbient)
	{
		sActionLength = padAudioDaemon->padatAmbient->fGetSampleLength(sndhndAmbientSample);
	}
	else
	{
		sActionLength = 0.0f;
	}
}



//*********************************************************************************************
// A trigger has fired with an ambient action.
//
void CAmbientAction::Start
(
)
//	
//**************************************
{
	//dprintf("ACTION: AmbientStart [%x]\n",sndhndAmbientSample);
	float f_mvol;

	if (fMasterVolumeMin == fMasterVolumeMax)
		f_mvol = fMasterVolumeMin;
	else
		f_mvol = CTrigger::rndRand(fMasterVolumeMin, fMasterVolumeMax);

	CMessageAudio	maVoice
					(
						sndhndAmbientSample,
						bMute?eamAMBIENT:eamPERMAMBIENT,// type of sound
						padAudioDaemon,			// destination of message
						0,						// sender
						pinsLocation,			// parent
						fVolume,				// play volume
						fAtten,					// volume attenuation
						u4SpatialType,			// type of sample positioning
						fFrustumAngle,			// sound frustum angle
						fOutsideVolume,			// outside the frustum volume
						bLooped,				// looped
						i4LoopCount,			// number of times to loop or -1 for continous
						fMaxDistance,			// maximum distance
						fMaxVolDistance,		// distance of no attenuation
						f_mvol					// master volume
					);
	maVoice.Dispatch();
}






//*********************************************************************************************
// STREAMED MUSIC ACTION
//*********************************************************************************************
// music message constructor
//
CMusicAction::CMusicAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,			// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	// these props must be present..
	const CEasyString*		pestr_sample= NULL;

	// these props have defaults...
	float				f_volume	= 0.0f;
	float				f_atten		= 1.0f;
	float				f_frustum	= 360.0f;
	float				f_out_vol	= -15.0f;
	int32				i4_spatial	= 0;	// STEREO
	CInstance*			pins_parent = ptr_trig;
	const CEasyString*	pestr_emmit = 0;


	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		// Do we have a sample
		if (!bFILL_pEASYSTRING(pestr_sample, esTActionSample))
		{
			// there is no sample specified
			Assert(0);
		}

		// fill all of the optional data...
		bFILL_INT(i4_spatial, esTActionSpatial);
		bFILL_FLOAT(f_volume, esTActionVolume);
		bFILL_FLOAT(f_atten,  esTActionAtten);
		bFILL_FLOAT(f_out_vol,esTActionOutsideVol);
		bFILL_FLOAT(f_frustum,esTActionFrustum);

		// get the name of the emmiter object, if there is one.
		if (bFILL_pEASYSTRING(pestr_emmit, esTActionEmitter))
		{
			// there is an emmiter so gets its instance
			CInstance* pins = wWorld.ppartPartitionList()->
								pinsFindNamedInstance(pestr_emmit->strData());

			// did not find the object
			Assert(pins);

			// only change the emmiter if we found a valid object
			if (pins)
				pins_parent = pins;
		}
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	// fill the action data with what we got from the text props
	if (pestr_sample)
	{
		sndhndMusicSample = sndhndHashIdentifier( pestr_sample->strData() );
	}
	else
	{
		// no sample, use the missing sample
		sndhndMusicSample = sndhndHashIdentifier( "MISSING" );
	}

	fVolume = f_volume;
	u4SpatialType = (uint32)i4_spatial;
	fAtten = f_atten;
	pinsLocation = pins_parent;
	fOutsideVolume = f_out_vol;
	fFrustumAngle = f_frustum;

	// validate the range of all the action text props...
	Assert ((fVolume<=0.0f) && (fVolume>=-100.0f));
	Assert (u4SpatialType<=2);
	Assert ((fAtten<10.0f) && (fAtten>=0.0f));
	Assert ((fOutsideVolume<=0.0f) && (fOutsideVolume>=-100.0f));
	Assert ((fFrustumAngle>=0.0f) && (fFrustumAngle<=360.0f));
}





//*********************************************************************************************
// A trigger has fired with a play music action
//
void CMusicAction::Start
(
)
//	
//**************************************
{
//	dprintf("ACTION: Music Start [%s]\n",estrSample.strData());
	CMessageAudio	maVoice
					(
						sndhndMusicSample,
						eamMUSIC,				// type fo sound
						padAudioDaemon,			// destination of message
						0,						// sender
						pinsLocation,			// parent
						fVolume,				// play volume
						fAtten,					// volume attenuation
						u4SpatialType,			// type of sample positioning
						fFrustumAngle,			// sound frustum angle
						fOutsideVolume,			// outside the frustum volume
						false,					// looped
						-1,						// loop count
						100000.0f				// maximum distance (not used)
					);
	maVoice.Dispatch();
}




//*********************************************************************************************
// FADE MUSIC VOLUME ACTION
//*********************************************************************************************
// music message constructor
//
CFadeMusicAction::CFadeMusicAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,			// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	// the volume decay must be speicfed..
	float				f_decay;

	bStop = true;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		if (!bFILL_FLOAT(f_decay, esTActionVolFade))
		{
			// must specify a volume decay rate
			Assert(0);
		}

		bFILL_BOOL(bStop, esTActionStopAfterFade);
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	fDeltaVolume = f_decay/10.0f;

	// validate the range of all the action text props...
	Assert ((fDeltaVolume<=100.0f) && (fDeltaVolume>=-100.0f));
}





//*********************************************************************************************
// A trigger has fired with a volume fade, send a message to the audio daemon
//
void CFadeMusicAction::Start
(
)
//	
//**************************************
{
//	dprintf("ACTION: Music Fade Start \n");
	CMessageAudio	ma_music_fade
					(
						padAudioDaemon,			// destination of message
						0						// sender
					);

	ma_music_fade.SetMessageType(eamCTRL_FADEMUSIC);
	ma_music_fade.fDeltaVolume = fDeltaVolume;
	ma_music_fade.bStopAfterFade = bStop;

	ma_music_fade.Dispatch();
}






//*********************************************************************************************
// SHOW OVERLAYED DECAL (TEMP ACTION)
//*********************************************************************************************
//
CShowOverlayAction::CShowOverlayAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,			// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	const CEasyString*	pestr_bitmap;
	bool				b_center = true;
	bool				b_discard = false;
	int32				i4_xp = 0;
	int32				i4_yp = 0;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		if (!bFILL_pEASYSTRING(pestr_bitmap, esTActionBitmap))
		{
			// there is no bitmap specified
			Assert(0);
		}

		bFILL_BOOL(b_center, esTActionCenter);
		bFILL_BOOL(b_discard, esTActionDiscard);
		bFILL_INT(i4_xp, esTActionXP);
		bFILL_INT(i4_yp, esTActionYP);
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	if (pestr_bitmap)
	{
		estrBitmap = *pestr_bitmap;
	}
	else
	{
		estrBitmap = "";
	}

	bDiscard = b_discard;
	bCenter = b_center;
	i4XPos = i4_xp;
	i4YPos = i4_yp;
}



//*********************************************************************************************
void CShowOverlayAction::Start
(
)
//	
//**************************************
{
//	dprintf("ACTION: Show Overlay \n");

	SOverlay ovl;

	ovl.v2iOverlayPos.tX = i4XPos;
	ovl.v2iOverlayPos.tY = i4YPos;
	ovl.prasOverlay = prasReadBMP(estrBitmap.strData(), true);

	ovl.bThrowaway = bDiscard;
	ovl.bCentered = bCenter;

	// Add overlay to the list.
	pVideoOverlay->AddOverlay(ovl);
}




//*********************************************************************************************
// SET THE FOG ACTION
//*********************************************************************************************
//
CSetFogAction::CSetFogAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,			// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	int32	u4_r;
	int32	u4_g;
	int32	u4_b;
	int32	i4_col_count = 0;
	int32	i4_type;
	uint32	u4_flags = 0;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		i4_col_count += bFILL_INT(u4_r, esR)?1:0;
		i4_col_count += bFILL_INT(u4_g, esG)?1:0;
		i4_col_count += bFILL_INT(u4_b, esB)?1:0;

		// either all 3 colours have to be set or none of them
		Assert ( (i4_col_count==3) || (i4_col_count==0) );

		// check the range of the colours
		Assert (u4_r<256);
		Assert (u4_g<256);
		Assert (u4_b<256);

		if (i4_col_count)
		{
			u4_flags |= ACTION_FOG_FLAG_SET_COLOUR;
			clrFog = CColour(u4_r, u4_g, u4_b);
		}

		if (bFILL_INT(i4_type, esTActionFogType))
		{
			u4_flags |= ACTION_FOG_FLAG_SET_TYPE;

			if (i4_type == ACTION_FOG_TYPE_LINEAR)
			{
				u4_flags |= ACTION_FOG_FLAG_SET_LINEAR;
			}
			else if (i4_type == ACTION_FOG_TYPE_EXPO)
			{
				u4_flags |= ACTION_FOG_FLAG_SET_EXPONENTIAL;
			}
			else
			{
				// unknown fog type
				Assert(0);
			}
		}
		
		if (bFILL_FLOAT(fFogPower, esTActionFogPower))
		{
			u4_flags |= ACTION_FOG_FLAG_SET_POWER;
			Assert(fFogPower>0.0f);
		}

		if (bFILL_FLOAT(fFogHalf, esTActionFogHalf))
		{
			u4_flags |= ACTION_FOG_FLAG_SET_HALF;
			Assert(fFogHalf>0.0f);
		}
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	// if we have no flags set the the action is not going to do anything
	Assert(u4_flags);

	u4SetFlags = u4_flags;
}



//*********************************************************************************************
void CSetFogAction::Start
(
)
//	
//**************************************
{
//	dprintf("ACTION: Set Fog \n");

	CFog::SProperties fogprop = fogFog.fogpropGetProperties();


	//
	// Is the change colour flag set??
	//
	if (u4SetFlags & ACTION_FOG_FLAG_SET_COLOUR)
	{
		// set the fog colour
		clrDefEndDepth = clrFog;

		// set the sky fade colour
		if (gpskyRender)
		{
			gpskyRender->SetSkyFogTable(clrFog);
		}
	}


	//
	// Is the change fog type flag set?
	//
	if (u4SetFlags & ACTION_FOG_FLAG_SET_TYPE)
	{
		if (u4SetFlags & ACTION_FOG_FLAG_SET_LINEAR)
		{
			// Set fog type to linear.
			fogprop.efogFunction = CFog::efogLINEAR;
		}

		if (u4SetFlags & ACTION_FOG_FLAG_SET_EXPONENTIAL)
		{
			// Set fog type to exponential.
			fogprop.efogFunction = CFog::efogEXPONENTIAL;
		}
	}

	//
	// Is the change fog power flag set
	//
	if (u4SetFlags & ACTION_FOG_FLAG_SET_POWER)
	{
		fogprop.rPower = fFogPower;
	}

	//
	// Is the change fog Y Half flag set
	//
	if (u4SetFlags & ACTION_FOG_FLAG_SET_HALF)
	{
		fogprop.rHalfFogY = fFogHalf;
	}

	//
	// Set the new fog properties
	// 
	fogFog.SetProperties(fogprop);
	fogTerrainFog.SetProperties(fogprop);	

	if (u4SetFlags & ACTION_FOG_FLAG_SET_COLOUR)
	{
		// Update CLUTS.
		pcdbMain.UpdateCluts();

		// Re-create the terrain blend for the new fog colour.
		lbAlphaTerrain.CreateBlend(prasMainScreen.ptPtrRaw(), clrDefEndDepth);
	}

	//
	// Redraw the terrain textures to account for the new fog
	//
	if (CWDbQueryTerrain().tGet() != 0)
		CWDbQueryTerrain().tGet()->Rebuild(false);
}





//*********************************************************************************************
// SET TERRAIN PARAMETERS ACTION
//*********************************************************************************************
//
CSetTerrainAction::CSetTerrainAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	uint32	u4_flags = 0;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		if (bFILL_FLOAT(fPixelTol, esTActionTrrPixelTol))
			u4_flags |= ACTION_TERRAIN_SET_TrrPixelTol;

		if (bFILL_FLOAT(fPixelTolFar, esTActionTrrPixelTolFar))
			u4_flags |= ACTION_TERRAIN_SET_TrrPixelTolFar;

		if (bFILL_FLOAT(fTrrNoShadowDist,	esTrrNoShadowDist))
			u4_flags |= ACTION_TERRAIN_SET_TrrNoShadowDist;

		if (bFILL_FLOAT(fTrrNoTextureDist,	esTrrNoTextureDist))	
			u4_flags |= ACTION_TERRAIN_SET_TrrNoTextureDist;

		if (bFILL_FLOAT(fTrrNoDynTextureDist,esTrrNoDynTextureDist))	
			u4_flags |= ACTION_TERRAIN_SET_TrrNoDynTextureDist;

		if (bFILL_BOOL(bTrrMovingShadows,	esTrrMovingShadows))	
			u4_flags |= ACTION_TERRAIN_SET_TrrMovingShadows;
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	// if we have no flags set the the action is not going to do anything
	Assert(u4_flags);

	u4SetFlags = u4_flags;
}



//*********************************************************************************************
void CSetTerrainAction::Start
(
)
//	
//**************************************
{
	// Set terrain settings.
	if (u4SetFlags & ACTION_TERRAIN_SET_TrrPixelTol)
		NMultiResolution::CQuadRootTIN::rvarPixelTolerance.Set(fPixelTol);

	if (u4SetFlags & ACTION_TERRAIN_SET_TrrPixelTolFar)
		NMultiResolution::CQuadRootTIN::rvarPixelToleranceFar.Set(fPixelTolFar);

	if (u4SetFlags & ACTION_TERRAIN_SET_TrrNoShadowDist)
		NMultiResolution::CQuadRootTIN::rvarDisableShadowDistance.Set(fTrrNoShadowDist);

	if (u4SetFlags & ACTION_TERRAIN_SET_TrrNoTextureDist)
		NMultiResolution::CQuadRootTIN::rvarDisableTextureDistance.Set(fTrrNoTextureDist);	

	if (u4SetFlags & ACTION_TERRAIN_SET_TrrNoDynTextureDist)
		NMultiResolution::CQuadRootTIN::rvarDisableDynamicTextureDistance.Set(fTrrNoDynTextureDist);	

	if (u4SetFlags & ACTION_TERRAIN_SET_TrrMovingShadows)
		NMultiResolution::CTextureNode::bEnableMovingShadows = bTrrMovingShadows;	
}



//*********************************************************************************************
// SET SORT PARAMETERS ACTION
//*********************************************************************************************
//
CSetSortAction::CSetSortAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	uint32	u4_flags = 0;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		if (bFILL_FLOAT(fNearTolerance, esTActionNearTolerance))
		{
			u4_flags |= ACTION_SORT_SET_NearTolerance;
			Assert(fNearTolerance > 0.0f && fNearTolerance <= 1.0f);
		}

		if (bFILL_FLOAT(fFarTolerance, esTActionFarTolerance))
		{
			u4_flags |= ACTION_SORT_SET_FarTolerance;
			Assert(fFarTolerance > 0.0f && fFarTolerance <= 1.0f);
		}

		if (bFILL_FLOAT(fNearZ, esTActionNearZ))
		{
			u4_flags |= ACTION_SORT_SET_NearZ;
			Assert(fNearZ > 0.0f && fNearZ <= 1.0f);
		}

		if (bFILL_FLOAT(fFarZNo, esTActionFarZNo))
		{
			u4_flags |= ACTION_SORT_SET_FarZNo;
			Assert(fFarZNo > 0.0f && fFarZNo <= 1.0f);
		}

		if (bFILL_FLOAT(fSortPixelTol, esTActionSortPixelTol))
		{
			u4_flags |= ACTION_SORT_SET_SortPixelTol;
			Assert(fSortPixelTol >= 0.0f && fSortPixelTol <= 2.0f);
		}

		if (bFILL_INT(iMaxNumToSort, esTActionMaxNumToSort))
		{
			u4_flags |= ACTION_SORT_SET_MaxNumToSort;
			Assert(iMaxNumToSort >= 100 && iMaxNumToSort <= 2000);
		}

		if (bFILL_INT(iSort2PartAt, esTActionSort2PartAt))
		{
			u4_flags |= ACTION_SORT_SET_Sort2PartAt;
			Assert(iSort2PartAt >= 100 && iSort2PartAt <= 2000);
		}

		if (bFILL_INT(iSort4PartAt, esTActionSort4PartAt))
		{
			u4_flags |= ACTION_SORT_SET_Sort2PartAt;
			Assert(iSort4PartAt >= 100 && iSort4PartAt <= 2000);
		}

		if (bFILL_FLOAT(fTerrNearTolerance, esTActionTerrNearTolerance))
		{
			u4_flags |= ACTION_SORT_SET_TerrNearTolerance;
			Assert(fTerrNearTolerance > 0.0f && fTerrNearTolerance <= 1.0f);
		}

		if (bFILL_FLOAT(fTerrFarTolerance, esTActionTerrFarTolerance))
		{
			u4_flags |= ACTION_SORT_SET_TerrFarTolerance;
			Assert(fTerrFarTolerance > 0.0f && fTerrFarTolerance <= 1.0f);
		}

		if (bFILL_FLOAT(fTerrNearZ, esTActionTerrNearZ))
		{
			u4_flags |= ACTION_SORT_SET_TerrNearZ;
			Assert(fTerrNearZ > 0.0f && fTerrNearZ <= 1.0f);
		}

		if (bFILL_FLOAT(fTerrFarZ, esTActionTerrFarZ))
		{
			u4_flags |= ACTION_SORT_SET_TerrFarZ;
			Assert(fTerrFarZ > 0.0f && fTerrFarZ <= 1.0f);
		}

		if (bFILL_BOOL(bUseSeperateTol, esTActionUseSeperateTol))
		{
			u4_flags |= ACTION_SORT_SET_UseSeperateTol;
		}
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	// if we have no flags set the the action is not going to do anything
	Assert(u4_flags);

	u4SetFlags = u4_flags;
}


//*********************************************************************************************
void CSetSortAction::Start
(
)
//	
//**************************************
{
	if (u4SetFlags & ACTION_SORT_SET_NearTolerance)
		ptsTolerances.rNearZScale = fNearTolerance;

	if (u4SetFlags & ACTION_SORT_SET_FarTolerance)
		ptsTolerances.rFarZScale = fFarTolerance;

	if (u4SetFlags & ACTION_SORT_SET_NearZ)
		ptsTolerances.rNearZ = fNearZ;

	if (u4SetFlags & ACTION_SORT_SET_FarZ)
		ptsTolerances.rFarZ = fFarZ;

	if (u4SetFlags & ACTION_SORT_SET_FarZNo)
		ptsTolerances.rFarZNoDepthSort = fFarZNo;

	if (u4SetFlags & ACTION_SORT_SET_SortPixelTol)
		ptsTolerances.fPixelBuffer = fSortPixelTol;

	if (u4SetFlags & ACTION_SORT_SET_MaxNumToSort)
		ptsTolerances.iMaxToDepthsort = iMaxNumToSort;

	if (u4SetFlags & ACTION_SORT_SET_Sort2PartAt)
		ptsTolerances.iBinaryPartitionAt = iSort2PartAt;

	if (u4SetFlags & ACTION_SORT_SET_Sort4PartAt)
		ptsTolerances.iQuadPartitionAt = iSort4PartAt;

	if (u4SetFlags & ACTION_SORT_SET_TerrNearTolerance)
		ptsTolerances.rNearZScaleTerrain = fTerrNearTolerance;

	if (u4SetFlags & ACTION_SORT_SET_TerrFarTolerance)
		ptsTolerances.rFarZScaleTerrain = fTerrFarTolerance;

	if (u4SetFlags & ACTION_SORT_SET_TerrNearZ)
		ptsTolerances.rNearZTerrain = fTerrNearZ;

	if (u4SetFlags & ACTION_SORT_SET_TerrFarZ)
		ptsTolerances.rFarZTerrain = fTerrFarZ;

	if (u4SetFlags & ACTION_SORT_SET_UseSeperateTol)
		ptsTolerances.bUseSeparateTolerances = bUseSeperateTol;
}



//*********************************************************************************************
// SET RENDERER PARAMETERS ACTION
//*********************************************************************************************
//
CSetRendererAction::CSetRendererAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	uint32	u4_flags = 0;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		//
		// Lighting & camera settings.
		//
		if (bFILL_FLOAT(fAmbientLight, esTActionAmbientLight))
		{
			u4_flags |= ACTION_RENDERER_SET_AMBIENT;
			Assert( (fAmbientLight>=0.0f) && (fAmbientLight<=1.0f) );
		}

		if (bFILL_FLOAT(fCameraFOV, esCameraFOV))
		{
			u4_flags |= ACTION_RENDERER_SET_CAMERA_FOV;
			Assert( fCameraFOV > 0.0f && fCameraFOV <= 170.0f );
		}

		if (bFILL_FLOAT(fNearClipPlane, esTActionNearClipPlane))
		{
			u4_flags |= ACTION_RENDERER_SET_NEAR_CLIP;
			Assert(fNearClipPlane>0.0f);
		}

		if (bFILL_FLOAT(fFarClipPlane, esTActionFarClipPlane))
		{
			u4_flags |= ACTION_RENDERER_SET_FAR_CLIP;
			Assert(fFarClipPlane>0.0f);
		}

		//
		// New distance culling stuff for the load trigger.
		//
		if (bFILL_FLOAT(fCullMaxDist, esCullingMaxDist))
		{
			u4_flags |= ACTION_RENDERER_SET_CULLMAXDIST;
			Assert(fCullMaxDist > 0.0f);
		}
		if (bFILL_FLOAT(fCullMaxRadius, esCullingMaxRadius))
		{
			u4_flags |= ACTION_RENDERER_SET_CULLMAXRADIUS;
			Assert(fCullMaxRadius > 0.0f);
		}
		if (bFILL_FLOAT(fCullMaxDistShadow, esCullingMaxDistShadow))
		{
			u4_flags |= ACTION_RENDERER_SET_CULLMAXDISTSHADOW;
			Assert(fCullMaxDistShadow > 0.0f);
		}
		if (bFILL_FLOAT(fCullMaxRadiusShadow, esCullingMaxRadiusShadow))
		{
			u4_flags |= ACTION_RENDERER_SET_CULLMAXRADIUSSHADOW;
			Assert(fCullMaxRadiusShadow > 0.0f);
		}

		//
		// Perspective correction settings.
		//
		if (bFILL_FLOAT(fPixelError, esPixelError))
		{
			u4_flags |= ACTION_RENDERER_SET_PixelError;
		}

		if (bFILL_INT(iSubdivisionLen, esSubdivisionLen))
		{
			u4_flags |= ACTION_RENDERER_SET_SubdivisionLen;
			Assert(iSubdivisionLen > 1 && (iSubdivisionLen & 1) == 0);
		}

		if (bFILL_FLOAT(fAltPixelError, esAltPixelError))
		{
			u4_flags |= ACTION_RENDERER_SET_AltPixelError;
		}

		if (bFILL_INT(iAltSubdivisionLen, esAltSubdivisionLen))
		{
			u4_flags |= ACTION_RENDERER_SET_AltSubdivisionLen;
			Assert(iAltSubdivisionLen > 1 && (iAltSubdivisionLen  & 1) == 0);
		}
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	// if we have no flags set the the action is not going to do anything
	Assert(u4_flags);

	u4SetFlags = u4_flags;
}


//*********************************************************************************************
void CSetRendererAction::Start
(
)
//	
//**************************************
{
	//dprintf("ACTION: Set Renderer \n");

	CCamera* pcam = NULL;
	CCamera::SProperties camprop;
	bool b_reinitialize_partitions = false;

	//
	// Lighting & camera settings.
	//
	if (u4SetFlags & ACTION_RENDERER_SET_CAMERA_FOV)
	{
		if (pcam == NULL)
		{
			CWDbQueryActiveCamera wqcam(wWorld);
			pcam = wqcam.tGet();
			Assert(pcam);
			camprop = pcam->campropGetProperties();
		}

		camprop.SetAngleOfView(dDegreesToRadians(fCameraFOV));
	}

	if (u4SetFlags & ACTION_RENDERER_SET_NEAR_CLIP)
	{
		if (pcam == NULL)
		{
			CWDbQueryActiveCamera wqcam(wWorld);
			pcam = wqcam.tGet();
			Assert(pcam);
			camprop = pcam->campropGetProperties();
		}

		camprop.rNearClipPlaneDist  = fNearClipPlane;		
	}

	if (u4SetFlags & ACTION_RENDERER_SET_FAR_CLIP)
	{
		if (pcam == NULL)
		{
			CWDbQueryActiveCamera wqcam(wWorld);
			pcam = wqcam.tGet();
			Assert(pcam);
			camprop = pcam->campropGetProperties();
		}

		camprop.rDesiredFarClipPlaneDist  = fFarClipPlane;
		camprop.SetFarClipFromDesired();
	}

	//
	// New distance cull load trigger stuff.
	//
	if (u4SetFlags & ACTION_RENDERER_SET_CULLMAXDIST)
	{
		SPartitionSettings::SetCullMaxAtDistance(fCullMaxDist);
		b_reinitialize_partitions = true;
	}
	if (u4SetFlags & ACTION_RENDERER_SET_CULLMAXRADIUS)
	{
		SPartitionSettings::SetMaxRadius(fCullMaxRadius);
		b_reinitialize_partitions = true;
	}
	if (u4SetFlags & ACTION_RENDERER_SET_CULLMAXDISTSHADOW)
	{
		SPartitionSettings::SetCullMaxAtDistanceShadow(fCullMaxDistShadow);
		b_reinitialize_partitions = true;
	}
	if (u4SetFlags & ACTION_RENDERER_SET_CULLMAXRADIUSSHADOW)
	{
		SPartitionSettings::SetMaxRadiusShadow(fCullMaxRadiusShadow);
		b_reinitialize_partitions = true;
	}

	//
	// if we queried for the camera we must have chnaged something so
	// set the camera properties..
	//
	if (pcam!=NULL)
	{
		pcam->SetProperties(camprop);
	}

	/*
	 * Ignored, these are now set based on the global quality setting.
	 *
	// Perspective settings (all hacked up).
	extern float fPerspectivePixelError;
	extern float fAltPerspectivePixelError;
	extern CPerspectiveSettings persetSettings;

	//
	// Perspective correction settings.
	//
	if (u4SetFlags & ACTION_RENDERER_SET_PixelError)
	{
		fPerspectivePixelError = fPixelError;
	}

	if (u4SetFlags & ACTION_RENDERER_SET_SubdivisionLen)
	{
		persetSettings.iMinSubdivision = iSubdivisionLen;
	}

	if (u4SetFlags & ACTION_RENDERER_SET_AltPixelError)
	{
		fAltPerspectivePixelError = fAltPixelError;
	}

	if (u4SetFlags & ACTION_RENDERER_SET_AltSubdivisionLen)
	{
		persetSettings.iAltMinSubdivision = iAltSubdivisionLen;
	}
	*/

	//
	// Set the ambient light if speicifed
	//
	if (u4SetFlags & ACTION_RENDERER_SET_AMBIENT)
	{
		//
		// Iterate through the world database and find the first ambient light.
		// The ambient light is always the first one in the list.
		//
		CWDbQueryLights wqlt;

		// Return the light.
		Assert(wqlt.tGet());
		rptr<CLight> plt = ptCastRenderType<CLight>(wqlt.tGet()->prdtGetRenderInfo());

		Assert(plt);
		rptr<CLightAmbient> plta = rptr_dynamic_cast(CLightAmbient, plt);
		
		Assert(plta);
		plta->lvIntensity = fAmbientLight;
	}

	// Reinitialize the partitions if required.
	if (b_reinitialize_partitions && pwWorld)
		pwWorld->InitializePartitions();
}



//*********************************************************************************************
// SET IMAGE CACHE PARAMETERS ACTION
//*********************************************************************************************
//
CSetImageCacheAction::CSetImageCacheAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	uint32	u4_flags = 0;
	bool	b_cache_active;
	bool	b_cache_intersect;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		if (bFILL_BOOL(b_cache_active, esTActionCacheActive))
		{
			if (b_cache_active)
			{
				u4_flags |= ACTION_IMGCACHE_SET_CACHE_ON;
			}
			else
			{
				u4_flags |= ACTION_IMGCACHE_SET_CACHE_OFF;
			}
		}

		if (bFILL_BOOL(b_cache_intersect, esTActionCacheIntersect))
		{
			if (b_cache_intersect)
			{
				u4_flags |= ACTION_IMGCACHE_SET_INTERSECT_ON;
			}
			else
			{
				u4_flags |= ACTION_IMGCACHE_SET_INTERSECT_OFF;
			}
		}

		if (bFILL_FLOAT(fPixelRatio, esTActionPixelRatio))
		{
			u4_flags |= ACTION_IMGCACHE_SET_PIXEL_RATIO;
		}

		if (bFILL_INT(i4MinPixels, esTActionMinPixels))
		{
			u4_flags |= ACTION_IMGCACHE_SET_MIN_PIXELS;
		}

		if (bFILL_INT(i4CacheAge, esTActionCacheAge))
		{
			u4_flags |= ACTION_IMGCACHE_SET_CACHE_AGE;
		}
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	// if we have no flags set the the action is not going to do anything
	Assert(u4_flags);

	u4SetFlags = u4_flags;
}



//*********************************************************************************************
void CSetImageCacheAction::Start
(
)
//**************************************
{
//	dprintf("ACTION: Set Image cache \n");

	if (u4SetFlags & ACTION_IMGCACHE_SET_CACHE_ON)
	{
		rcsRenderCacheSettings.erctMode = ercmCACHE_ON;
	}

	if (u4SetFlags & ACTION_IMGCACHE_SET_CACHE_OFF)
	{
		rcsRenderCacheSettings.erctMode = ercmCACHE_OFF;
	}

	if (u4SetFlags & ACTION_IMGCACHE_SET_INTERSECT_ON)
	{
		rcsRenderCacheSettings.bAddIntersectingObjects = true;
	}

	if (u4SetFlags & ACTION_IMGCACHE_SET_INTERSECT_OFF)
	{
		rcsRenderCacheSettings.bAddIntersectingObjects = false;
	}

	if (u4SetFlags & ACTION_IMGCACHE_SET_PIXEL_RATIO)
	{
	}
	
	if (u4SetFlags & ACTION_IMGCACHE_SET_MIN_PIXELS)
	{
	}

	if (u4SetFlags & ACTION_IMGCACHE_SET_CACHE_AGE)
	{
		rcsRenderCacheSettings.iEuthanasiaAge = i4CacheAge;
	}
}


//*********************************************************************************************
// SET SKY PARAMETERS ACTION
//*********************************************************************************************
//
CSetSkyAction::CSetSkyAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//**************************************
{
	uint32	u4_flags = 0;
	bool	b_sky_texture = true;
	bool	b_sky_fill = true;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		if (bFILL_BOOL(b_sky_fill, esFillScreen))
		{
			if (b_sky_fill)
			{
				u4_flags |= ACTION_SKY_SET_FILLSCREEN_ON;
			}
			else
			{
				u4_flags |= ACTION_SKY_SET_FILLSCREEN_OFF;
			}
		}

		if (bFILL_BOOL(b_sky_texture, esTexture))
		{
			if (b_sky_texture)
			{
				u4_flags |= ACTION_SKY_SET_TEXTURE_ON;
			}
			else
			{
				u4_flags |= ACTION_SKY_SET_TEXTURE_OFF;
			}
		}

		if (bFILL_FLOAT(fScale, esScale))
		{
			u4_flags |= ACTION_SKY_SET_SCALE;
			Assert(fScale>=0.001);
			Assert(fScale<=0.2);
		}

		if (bFILL_FLOAT(fHeight, esHeight))
		{
			u4_flags |= ACTION_SKY_SET_HEIGHT;
			Assert(fHeight>=500);
			Assert(fHeight<=2500);
		}

		if (bFILL_FLOAT(fNearFog, esFogNear))
		{
			u4_flags |= ACTION_SKY_SET_NEAR_FOG;
			Assert(fNearFog>=0.0);
			Assert(fNearFog<=1.0);
		}

		if (bFILL_FLOAT(fFarFog, esFogFar))
		{
			u4_flags |= ACTION_SKY_SET_FAR_FOG;
			Assert(fFarFog>=0.0);
			Assert(fFarFog<=1.0);
		}

		if (bFILL_FLOAT(fWindX, esWindSpeedX))
		{
			u4_flags |= ACTION_SKY_SET_WIND_SPEED_X;
			Assert(fWindX>=0.0);
		}

		if (bFILL_FLOAT(fWindY, esWindSpeedY))
		{
			u4_flags |= ACTION_SKY_SET_WIND_SPEED_Y;
			Assert(fWindX>=0.0);
		}


		if (bFILL_INT(i4Division, esSubDivision))
		{
			u4_flags |= ACTION_SKY_SET_SUBDIVISIONS;
			Assert(i4Division>=2);				// Minimum of 2 pixels per division
			Assert((i4Division%2) == 0);		// Division length must be even
		}

	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	// if we have no flags set the the action is not going to do anything
	Assert(u4_flags);

	Assert(fNearFog<fFarFog);

	u4SetFlags = u4_flags;
}


//*********************************************************************************************
void CSetSkyAction::Start
(
)
//**************************************
{
	if (gpskyRender == NULL)		// To be safe in release mode do nothing if we have no sky.
	{
		Assert(0);					// We have no sky but we do have a sky action.
		return;
	}

	//
	// Get the current sky settings
	//
	float				f_sky_height = gpskyRender->fGetHeight();
	float				f_sky_scale  = gpskyRender->fGetPixelsPerMeter();
	float				f_sky_near	 = gpskyRender->fGetFogNear();
	float				f_sky_far	 = gpskyRender->fGetFogFar();
	float				f_x, f_y;
	uint32				u4_division	 = gpskyRender->u4GetDivisionLength();

	gpskyRender->GetSkyWind(f_x,f_y);

	if (u4SetFlags & ACTION_SKY_SET_SCALE)
	{
		f_sky_scale = fScale;
	}

	if (u4SetFlags & ACTION_SKY_SET_HEIGHT)
	{
		f_sky_height = fHeight;
	}

	if (u4SetFlags & ACTION_SKY_SET_NEAR_FOG)
	{
		f_sky_near = fNearFog;
	}

	if (u4SetFlags & ACTION_SKY_SET_FAR_FOG)
	{
		f_sky_far = fFarFog;
	}

	if (u4SetFlags & ACTION_SKY_SET_WIND_SPEED_X)
	{
		f_x = fWindX;
	}

	if (u4SetFlags & ACTION_SKY_SET_WIND_SPEED_Y)
	{
		f_y = fWindY;
	}

	if (u4SetFlags & ACTION_SKY_SET_SUBDIVISIONS)
	{
		u4_division = uint32(i4Division);
	}

	/*
	 * Ignored, this is now set based on the global quality setting.
	 *
	if (u4SetFlags & ACTION_SKY_SET_TEXTURE_ON)
	{
		gpskyRender->SetTextured(true);
	}

	if (u4SetFlags & ACTION_SKY_SET_TEXTURE_OFF)
	{
		gpskyRender->SetTextured(false);
	}
	*/

	if (u4SetFlags & ACTION_SKY_SET_FILLSCREEN_ON)
	{
		gpskyRender->SetFilled(true);
	}

	if (u4SetFlags & ACTION_SKY_SET_FILLSCREEN_OFF)
	{
		gpskyRender->SetFilled(false);
	}
		
	//
	// Put the modified sky parameters back
	//
	gpskyRender->SetWorkingConstants
	(
		f_sky_scale,
		f_sky_height,
		f_sky_near,
		f_sky_far,
		u4_division
	);
	
	gpskyRender->SetSkyWind(f_x,f_y);
}


//*********************************************************************************************
//
CSetAlphaWaterAction::CSetAlphaWaterAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//
//**************************************
{
	/*
	object AlphaWaterProperties =
	{ 
		object A00 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A01 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A02 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A03 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A04 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A05 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A06 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A07 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A08 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A09 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A10 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A11 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A12 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A13 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A14 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A15 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
		object A16 = { int R = 100; int G = 100; int B = 100; float Alpha = 1.0f  };
	}
	*/

	// Clear flags.
	u4SetFlags = 0;

	// Get values from text-props.
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		IF_SETUP_OBJECT_SYMBOL(esAlphaWaterProperties)
		{
			u4SetFlags |= ACTION_WATER_SET_ALHPA_VALUES;

			for (int i = 0; i < 16; i++)
			{
				IF_SETUP_OBJECT_SYMBOL(ESymbol(esA00 + i))
				{
					int i_red = 0;
					int i_green = 0;
					int i_blue = 0;

					bFILL_INT(i_red, esR);
					bFILL_INT(i_green, esG);
					bFILL_INT(i_blue, esB);

					clrColour[i] = CColour(i_red, i_green, i_blue);

					fAlpha[i] = 0.0f;
					bFILL_FLOAT(fAlpha[i], esAlpha);
				}
				END_OBJECT;
			}
		}
		END_OBJECT;

		IF_SETUP_OBJECT_SYMBOL(esNonAlphaWaterProperties)
		{
			u4SetFlags |= ACTION_WATER_SET_NONALHPA_VALUES;

			for (int i = 0; i < 32; i++)
			{
				IF_SETUP_OBJECT_SYMBOL(ESymbol(esA00 + i))
				{
					int i_red = 0;
					int i_green = 0;
					int i_blue = 0;

					bFILL_INT(i_red, esR);
					bFILL_INT(i_green, esG);
					bFILL_INT(i_blue, esB);

					clrAltColour[i] = CColour(i_red, i_green, i_blue);

					fAltAlpha[i] = 0.0f;
					bFILL_FLOAT(fAltAlpha[i], esAlpha);
				}
				END_OBJECT;
			}
		}
		END_OBJECT;
	}
	END_OBJECT;
	END_TEXT_PROCESSING;

	// We want to do something.
	Assert(u4SetFlags);
}


//*********************************************************************************************
//
void CSetAlphaWaterAction::Start()
//
//**************************************
{
	// Global defualt water alhpa settings.
	extern CLightBlend::SLightBlendSettings albsWater[];
	extern CLightBlend::SLightBlendSettings lbsNonAlphaWater[];
	
	if (u4SetFlags & ACTION_WATER_SET_ALHPA_VALUES)
	{
		for (int i = 0; i < 16; i++)
		{
			albsWater[i] = CLightBlend::SLightBlendSettings(clrColour[i], fAlpha[i]);
		}
	}

	if (u4SetFlags & ACTION_WATER_SET_NONALHPA_VALUES)
	{
		for (int i = 0; i < 32; i++)
		{
			lbsNonAlphaWater[i] = CLightBlend::SLightBlendSettings(clrAltColour[i], fAltAlpha[i]);
		}
	}

	if (u4SetFlags)
	{
		// Re-do the light blend.
		lbAlphaWater.CreateBlendForWater(prasMainScreen.ptPtrRaw());
	}
}


//*********************************************************************************************
//
CWaterOnOffAction::CWaterOnOffAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//
//**************************************
{
	u4Handle = 0;
	bEnable = false;

	// Get values from text-props.
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		const CEasyString* pestr_object = 0;
		if (bFILL_pEASYSTRING(pestr_object, esObjectName))
		{
			u4Handle = u4Hash((const char *)pestr_object->strData());
		}

		bFILL_BOOL(bEnable, esEnable);
	}
	END_OBJECT;
	END_TEXT_PROCESSING;

	// We at least want a water object.
	Assert(u4Handle);
}


//*********************************************************************************************
//
void CWaterOnOffAction::Start()
//
//**************************************
{
	// Find the water object.
	CInstance* pins = wWorld.ppartPartitionList()->pinsFindInstance(u4Handle);

	// Cast it to the correct type.
	CEntityWater* petw = ptCast<CEntityWater>(pins);

	if (petw)
	{
		// Enable/Disable it.
		petw->Enable(bEnable);
	}
}


//*********************************************************************************************
//
CLoadLevelAction::CLoadLevelAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//
//**************************************
{
	// Get values from text-props.
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		const CEasyString* pestr_object = 0;
		if (bFILL_pEASYSTRING(pestr_object, esLevelName))
		{
			// Make a copy of the string.
			estrLevelName = *pestr_object;
		}
		else
		{
			// Must have the name of the level to load.
			Assert(0);
		}
	}
	END_OBJECT;
	END_TEXT_PROCESSING;
}

//*********************************************************************************************
//
void CLoadLevelAction::Start()
//
//**************************************
{
	//
	// Load the level.  This must be deferred until later since int will delete
	// the world.
	//
	pwWorld->DeferredLoad(estrLevelName.strData());
}


//*********************************************************************************************
//
CSaveLevelAction::CSaveLevelAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//
//**************************************
{
	// Get values from text-props.
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		const CEasyString* pestr_object = 0;
		if (bFILL_pEASYSTRING(pestr_object, esLevelName))
		{
			// Make a copy of the string.
			estrLevelName = *pestr_object;
		}
		else
		{
			// Default name to save.
			estrLevelName = "AutoSave.scn";
		}
	}
	END_OBJECT;
	END_TEXT_PROCESSING;
}

//*********************************************************************************************
//
void CSaveLevelAction::Start()
//
//**************************************
{
	//
	// Save the level.  This must be deferred until later so that the shell can
	// show some indication that something is going on.
	//
	pwWorld->DeferredSave(estrLevelName.strData());
}



//*********************************************************************************************
//
// CSetAnimatePropertiesAction implementation.
//

	//*****************************************************************************************
	CSetAnimatePropertiesAction::CSetAnimatePropertiesAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	)
	{
		u4Handle	= 0;
		u4SetFlags	= 0;

		// Get values from text-props.
		SETUP_TEXT_PROCESSING(pvtable, pload);
		IF_SETUP_OBJECT_POINTER(poval_action)
		{
			const CEasyString* pestr_object = 0;
			if (bFILL_pEASYSTRING(pestr_object, esObjectName))
			{
				u4Handle = u4Hash((const char *)pestr_object->strData());
			}

			if (bFILL_FLOAT(fHitPoints, esHitPoints))
				u4SetFlags |= ACTION_ANIMATE_PROP_SET_HITPOINTS;

			if (bFILL_FLOAT(fMaxHitPoints, esMaxHitPoints))
				u4SetFlags |= ACTION_ANIMATE_PROP_SET_MAX_HITPOINTS;

			if (bFILL_FLOAT(fRegenerationRate, esRegeneration))
				u4SetFlags |= ACTION_ANIMATE_PROP_SET_REGENERATION_RATE;

			if (bFILL_FLOAT(fReallyDead, esReallyDie))
				u4SetFlags |= ACTION_ANIMATE_PROP_SET_REALLY_DEAD;

			if (bFILL_FLOAT(fDieRate, esDieRate))
				u4SetFlags |= ACTION_ANIMATE_PROP_SET_DIE_RATE;

			if (bFILL_FLOAT(fCriticalHit, esCriticalHit))
				u4SetFlags |= ACTION_ANIMATE_PROP_SET_CRITICAL_HIT;

			if (bFILL_FLOAT(fDamage, esDamage))
				u4SetFlags |= ACTION_ANIMATE_PROP_SET_DAMAGE;
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		// Must have an object to work on.
		Assert(u4Handle != 0);
	}

	//*****************************************************************************************
	void CSetAnimatePropertiesAction::Start()
	{
		if (u4Handle != 0)
		{
			CInstance* pins = pwWorld->pinsFindInstance(u4Handle);
			Assert(pins != 0);

			CAnimate* pant = ptCast<CAnimate>(pins);
			Assert(pant != 0);

			if (u4SetFlags & ACTION_ANIMATE_PROP_SET_HITPOINTS)
			{
				float f_damage = pant->fHitPoints - fHitPoints;
				pant->fHitPoints = fHitPoints;

				if (f_damage > 0.0f)
					pant->HandleDamage(f_damage);
			}

			if (u4SetFlags & ACTION_ANIMATE_PROP_SET_MAX_HITPOINTS)
				pant->fMaxHitPoints = fMaxHitPoints;

			if (u4SetFlags & ACTION_ANIMATE_PROP_SET_REGENERATION_RATE)
				pant->fRegenerationRate = fRegenerationRate;

			if (u4SetFlags & ACTION_ANIMATE_PROP_SET_REALLY_DEAD)
				pant->fReallyDead = fReallyDead;

			if (u4SetFlags & ACTION_ANIMATE_PROP_SET_DIE_RATE)
				pant->fDieRate = fDieRate;

			if (u4SetFlags & ACTION_ANIMATE_PROP_SET_CRITICAL_HIT)
				pant->fCriticalHit = fCriticalHit;

			if (u4SetFlags & ACTION_ANIMATE_PROP_SET_DAMAGE)
			{
				pant->fHitPoints -= fDamage;
				pant->HandleDamage(fDamage);
			}
		}
	}


//*********************************************************************************************
//
// CTeleportAction implementation.
//

	//*****************************************************************************************
	CTeleportAction::CTeleportAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	)
	{
		u4Handle        = 0;
		bHeightRelative = true;
		bSetPosition    = true;
		bSetOrientation = true;
		bOnTerrain		= true;

		// Get values from text-props.
		SETUP_TEXT_PROCESSING(pvtable, pload);
		IF_SETUP_OBJECT_POINTER(poval_action)
		{
			const CEasyString* pestr_object = 0;
			if (bFILL_pEASYSTRING(pestr_object, esObjectName))
			{
				u4Handle = u4Hash((const char *)pestr_object->strData());

#if VER_TEST
				// Make sure target object was found.
				CGroffObjectName* pgon_target = pload->goiInfo.pgonFindObject(pestr_object->strData());

				if (pgon == 0)
				{
					char str_buffer[512];
					sprintf(str_buffer, 
							"%s\n\nMissing teleport target:\n%s needs %s", 
							__FILE__, 
							pgon->strObjectName,
							pgon_target->strObjectName);

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}
#endif
			}
			else
			{
				// ObjectName text prop not specified.
				AlwaysAssert(false);
			}

			const CEasyString* pestr_target = 0;
			if (bFILL_pEASYSTRING(pestr_target, esTeleportDestObjectName))
			{
				CGroffObjectName* pgon_target = pload->goiInfo.pgonFindObject(pestr_target->strData());

#if VER_TEST
				// Make sure target object was found.
				if (pgon == 0)
				{
					char str_buffer[512];
					sprintf(str_buffer, 
							"%s\n\nMissing teleport destination:\n%s needs %s", 
							__FILE__, 
							pgon->strObjectName,
							pgon_target->strObjectName);

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}
#endif

				p3Destination = ::pr3Presence(*pgon_target).p3Placement();
			}
			else
			{
				// TeleportDestObjectName text prop not specified.
				AlwaysAssert(false);
			}

			bFILL_BOOL(bHeightRelative, esHeightRelative);
			bFILL_BOOL(bSetPosition,    esSetPosition);
			bFILL_BOOL(bSetOrientation, esSetOrientation);
			bFILL_BOOL(bOnTerrain,		esOnTerrain);
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		// Must have an object to work on.
		Assert(u4Handle != 0);
	}


	//*****************************************************************************************
	void CTeleportAction::Start()
	{
		if (u4Handle != 0)
		{
			CInstance* pins = pwWorld->pinsFindInstance(u4Handle);

#if VER_DEBUG
			AlwaysAssert(pins != 0);
			if (!pins)
				return;
#endif
			// Drop anything if we are teleporting it.
			CVector3<> v3_drop_velocity(0,0,0);
			if (pins == gpPlayer->pinsHeldObject())
			{
				gpPlayer->Drop(v3_drop_velocity);
			}

			// Construct a presence for the target teleport destination.
			CPresence3<> pr3_dest = pins->pr3Presence();

			if (bSetPosition)
				pr3_dest.v3Pos = p3Destination.v3Pos;

			if (bSetOrientation)
				pr3_dest.r3Rot = p3Destination.r3Rot;

			if (bHeightRelative)
			{
				CVector3<> v3_min, v3_max;

				const CBoundVol* pbv_ins;

				pbv_ins = pins->pphiGetPhysicsInfo()->pbvGetBoundVol();

				// Calculate the z delta from the instance's centre to its lowest z (at the teleport target orientation).
				pbv_ins->GetWorldExtents(pr3_dest, v3_min, v3_max);

				float f_pins_min_z_delta = pr3_dest.v3Pos.tZ - v3_min.tZ;

				// Should we put the object on the terrain?
				if (bOnTerrain)
				{
					// Yes! Determine the height of the terrain in the target area, and place the instance above it.
					float f_terrain_z = 0;

					if (CWDbQueryTerrain().tGet() != 0)
						f_terrain_z = CWDbQueryTerrain().tGet()->rHeight(pr3_dest.v3Pos.tX, pr3_dest.v3Pos.tY);

					pr3_dest.v3Pos.tZ = f_terrain_z + f_pins_min_z_delta + .1f;
				}

				// Bump scale for conservative physics queries.
				pr3_dest.rScale *= 1.1;

				bool b_intersect = true;

				for (int i = 0; i < 20 && b_intersect; i++)
				{
					// Determine the highest point of all physics objects in the teleport target area.
					CWDbQueryPhysics wqph(*pbv_ins, pr3_dest);

					float f_max_phys_z = -.1f;

					foreach(wqph)
					{
						const CInstance* pins_phys = wqph.tGet();
						const CBoundVol* pbv_ins_phys;

						// Don't pay attention to the teleporting object itself.
						if (pins_phys == pins)
							continue;

						// Ignore bio boundary boxes, as the animal itself will be caught here.
						// (Note cast due to incorrect constness of ptCast.)
						if (ptCast<CBoundaryBox>(const_cast<CInstance*>(pins_phys)))
							continue;

						pbv_ins_phys = pins_phys->pphiGetPhysicsInfo()->pbvGetBoundVol();

						pbv_ins_phys->GetWorldExtents(pins_phys->pr3Presence(), v3_min, v3_max);

						if (v3_max.tZ > f_max_phys_z)
							f_max_phys_z = v3_max.tZ;
					}

					// Determine the lowest z of the instance at the teleport target location.
					float f_ins_low_z = pr3_dest.v3Pos.tZ - f_pins_min_z_delta;

					// Will the instance be above all physics objects when teleported?
					if (f_ins_low_z > f_max_phys_z)
						b_intersect = false;
					else
						// Bump height and try again.
						pr3_dest.v3Pos.tZ = f_max_phys_z + f_pins_min_z_delta + .1f;
				}

				Assert(!b_intersect);

				if (b_intersect)
					return;
			}

			pins->PhysicsDeactivate();
			pins->Move(pr3_dest.p3Placement());
			pins->PhysicsActivate();
		}
	}




#pragma optimize("",off)
//*********************************************************************************************
//
// CSoundEffectAction implementation.
//

	//*****************************************************************************************
	CSoundEffectAction::CSoundEffectAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	)
	{
		// Get values from text-props.
		SETUP_TEXT_PROCESSING(pvtable, pload);
		IF_SETUP_OBJECT_POINTER(poval_action)
		{
			const CEasyString* pestr_sample = 0;
			if (bFILL_pEASYSTRING(pestr_sample, esTActionSample))
			{
				sndhndEffectSample = sndhndHashIdentifier( pestr_sample->strData() );
			}
			else
			{
				sndhndEffectSample = sndhndHashIdentifier( "MISSING" );
			}

			u4Handle = 0;
			fVolume = 0.0f;
			fAtten	= 1.0f;
			bAttach = false;
			fFrustum = 360.0f;
			fOutsideVol = -15.0f;
			bLoop = false;
			u4Handle = ptr_trig->u4GetUniqueHandle();

			const CEasyString*	pestr_emit = 0;

			// there may be an emitter specified, if so use this instead of the parent trigger
			if (bFILL_pEASYSTRING(pestr_emit, esTActionEmitter))
			{
				Assert(pestr_emit);

				u4Handle = u4Hash((const char *)pestr_emit->strData());
			}

			// get the attach flag
			bFILL_BOOL(bAttach, esTActionAttach);

			bFILL_FLOAT(fVolume, esTActionVolume);
			bFILL_FLOAT(fAtten,  esTActionAtten);
			bFILL_FLOAT(fFrustum, esTActionFrustum);
			bFILL_FLOAT(fOutsideVol,  esTActionOutsideVol);
			bFILL_BOOL(bLoop, esTActionSampleLoop);

		}
		END_OBJECT;
		END_TEXT_PROCESSING;
	}


	//*****************************************************************************************
	void CSoundEffectAction::Start()
	{
		CInstance*	pins = wWorld.ppartTriggerPartitionList()->pinsFindInstance(u4Handle);

		// if we did not find the object in the trigger partition, look in the main partition.
		if (pins == NULL)
		{
			pins = wWorld.ppartPartitionList()->pinsFindInstance(u4Handle);

#if VER_TEST
			// the instance we get from the above must be valid
			AlwaysAssert(pins);
#endif

		}

		CMessageAudio	msg
		(
			sndhndEffectSample,
			bAttach?eamATTACHED_EFFECT:eamPOSITIONED_EFFECT,		// type of sound
			NULL,						// sender (NULL if using play function)
			NULL,						// receiver (NULL if using play function)
			pins,						// parent of the effect (can be NULL for stereo)
			fVolume,					// volume 0dBs
			fAtten,						// attenuation (only for pseudo/real 3D samples)
			AU_SPATIAL3D,				// spatial type
			fFrustum,					// fustrum angle (real 3D only)
			fOutsideVol,				// outside volume (real 3D only)
			bLoop,						// looped
			0,							// loop count
			10000.0f,					// distance before sound is stopped (pseudo/real 3D only)
			0,
			0
		);

		bImmediateSoundEffect(msg,NULL);
	}


//*********************************************************************************************
//
// CDelayAction implementation.
//
	CDelayAction::CDelayAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	)
	{
		// Get delay value from text-props
		SETUP_TEXT_PROCESSING(pvtable, pload);
		IF_SETUP_OBJECT_POINTER(poval_action)
		{
			bFILL_FLOAT(sDelay, esDelay);
		}
		END_OBJECT;
		END_TEXT_PROCESSING;
	}


//*********************************************************************************************
//
// CScriptedAnimationAction implementation.
//

	//*****************************************************************************************
	CScriptedAnimationAction::CScriptedAnimationAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	)
	{
		pansAnim = 0;

		// Get values from text-props.
		SETUP_TEXT_PROCESSING(pvtable, pload);
		IF_SETUP_OBJECT_POINTER(poval_action)
		{
			const CEasyString* pestr = 0;
			if (bFILL_pEASYSTRING(pestr, esAnimationName))
			{
				// Determine directory from which we're loading this GROFF.
				string str_name = pwWorld->strGetGroff(0);

				int i_last_slash = Min(str_name.find_last_of('/'), str_name.find_last_of('\\'));
				string str_dir = str_name.substr(0, i_last_slash);

				pansAnim = new CAnimationScript(pestr->strData(), str_dir.c_str());
				pAnimations->Add(pansAnim);
			}
			else
			{
				// Must have name of animation script.
				Assert(false);
			}
		}
		END_OBJECT;
		END_TEXT_PROCESSING;
	}

	//*****************************************************************************************
	CScriptedAnimationAction::~CScriptedAnimationAction()
	{
		pAnimations->Remove(pansAnim);
	}

	//*****************************************************************************************
	void CScriptedAnimationAction::Start()
	{
		pansAnim->Rewind();
		pansAnim->Start();
	}


//*********************************************************************************************
//
// CSetVariableTriggerAction implementation.
//

	//*****************************************************************************************
	CSetVariableTriggerAction::CSetVariableTriggerAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	)
	{
		// Get values from text-props.
		SETUP_TEXT_PROCESSING(pvtable, pload);
		IF_SETUP_OBJECT_POINTER(poval_action)
		{
			const CEasyString* pestr = 0;
			if (bFILL_pEASYSTRING(pestr, esTriggerName))
			{
				u4Handle = u4Hash((const char *)pestr->strData());
			}
			else
			{
				// Target must be specified.
				AlwaysAssert(false);
			}

			bToggle = false;
			bValue = true;

			bFILL_BOOL(bToggle, esToggle);
			bFILL_BOOL(bValue, esValue);
		}
		END_OBJECT;
		END_TEXT_PROCESSING;
	}

	//*****************************************************************************************
	void CSetVariableTriggerAction::Start()
	{
		CInstance* pins = wWorld.ppartTriggerPartitionList()->pinsFindInstance(u4Handle);
		AlwaysAssert(pins);

		CVariableTrigger* pvt_trigger = dynamic_cast<CVariableTrigger*>(pins);
		AlwaysAssert(pvt_trigger);

		if (bToggle)
			pvt_trigger->Invert();
		else
			pvt_trigger->Set(bValue);
	}


	//*****************************************************************************************
	CAudioEnvironmentAction::CAudioEnvironmentAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	)
	{
		i4Environment	= 0;
		fVolume			= -1.0f;
		fDecay			= -1.0f;		// all set to illegal values
		fDamping		= -1.0f;

		// Get values from text-props.
		SETUP_TEXT_PROCESSING(pvtable, pload);
		IF_SETUP_OBJECT_POINTER(poval_action)
		{
			if (!bFILL_INT( i4Environment, esAudioEnvironment))
			{
				// Must specify an environment....
				Assert(0);
			}
			
			// These parameters are optional..
			bFILL_FLOAT(fVolume, esReverbVolume);			// Reverb Volume
			bFILL_FLOAT(fDecay, esReverbDecay);			// Reverb Decay
			bFILL_FLOAT(fDamping, esReverbDamping);			// Reverb Damping
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		Assert( (fVolume == -1.0f) || ((fVolume >= 0.0f) && (fVolume <= 1.0f)) );
		Assert( (fDecay == -1.0f) || ((fDecay >= 0.1f) && (fVolume <= 20.0f)) );
		Assert( (fDamping == -1.0f) || ((fDamping >= 0.0f) && (fDamping <= 2.0f)) );
	}

	//*****************************************************************************************
	void CAudioEnvironmentAction::Start()
	{
		// Always set the environment
		CAudio::pcaAudio->SetEAXEnvironment((uint32)i4Environment);

		// Only set the other parameters if they are specified
		if (fVolume>=0.0f)
		{
			CAudio::pcaAudio->SetEAXReverbVolume(fVolume);
		}

		if (fDecay>=0.0f)
		{
			CAudio::pcaAudio->SetEAXReverbDecay(fDecay);
		}

		if (fDamping>=0.0f)
		{
			CAudio::pcaAudio->SetEAXReverbDamping(fDamping);
		}
	}


	//*****************************************************************************************
	CTextAction::CTextAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	)
	{
		int32	i4_position	= 1;		// center.		
		bool	b_top = true;
		float	f_r = 1.0f;
		float	f_g = 1.0f;
		float	f_b = 1.0f;
		const	CEasyString* pestr = 0;

		fTime		= 3.0f;		// 3 seconds is the default
		u4Flags		= 0;
		strRawData	= NULL;

		// Get values from text-props.
		SETUP_TEXT_PROCESSING(pvtable, pload);
		IF_SETUP_OBJECT_POINTER(poval_action)
		{
			bFILL_INT( i4_position, esTextPosition);
			bFILL_FLOAT(fTime, esTextDisplayTime);

			bFILL_FLOAT( f_r, esR);
			bFILL_FLOAT( f_g, esG);
			bFILL_FLOAT( f_b, esB);

			bFILL_BOOL(b_top, esTextAtTop);

			if (bFILL_INT(i4ResID, esResourceID) == false)
			{
				i4ResID = 0;

				// if we do not have a resource ID, look for a raw string
				if (bFILL_pEASYSTRING(pestr, esOverlayText) == false)
				{
					AlwaysAssert(0);
				}

				// now copy the easy string data into an allocated buffer
				strRawData = new char[pestr->uLength()+1];

				// This will copy the string and process any /r sequences
				wsprintf(strRawData,pestr->strData());
			}
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		if (b_top)
		{
			u4Flags |= TEXT_FORMAT_TOP;
		}
		else
		{
			u4Flags |= TEXT_FORMAT_TOP;
		}

		switch (i4_position)
		{
		case 0:
			u4Flags |= TEXT_FORMAT_LEFT;
			break;

		case 1:
			u4Flags |= TEXT_FORMAT_CENTER;
			break;

		case 2:
			u4Flags |= TEXT_FORMAT_RIGHT;
			break;

		default:
			AlwaysAssert(0);
			break;
		}

		// make the text colour
		clrText = CColour(f_r, f_g, f_b);
	}


	//*****************************************************************************************
	void CTextAction::Start()
	{
		char*	str_string;
		char	str_buf[256];

		if (i4ResID)
		{
			u4LookupResourceString(i4ResID,str_buf,256);
			str_string = str_buf;
		}
		else
		{
			str_string = strRawData;
		}

		uint32 u4_prev = CTextOverlay::ptovTextSystem->u4FindSequenceEnd(ettTUTORIAL);
		// Send the text to the text system, the text system copies the text so it can be
		// in a local buffer
		CTextOverlay::ptovTextSystem->u4DisplayFormattedString(str_string, fTime, u4Flags, clrText,u4_prev,ettTUTORIAL);
	}


