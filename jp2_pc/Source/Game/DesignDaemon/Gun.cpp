/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *
 * Notes:
 *
 * To Do:
 *		Change Assert's into Assert's.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/Gun.cpp                                             $
 * 
 * 75    98/10/03 0:21 Speter
 * Create collision energy on tranq damage as well as regular damage.
 * 
 * 74    10/01/98 4:05p Mlange
 * Improved move message stat reporting.
 * 
 * 73    9/30/98 5:53p Rwyatt
 * Set defer load to flase for all gun samples
 * 
 * 72    9/27/98 10:46p Ksherr
 * Added unlimited ammo cheat. If it's on then Anne doesn't talk about the ammo.
 * 
 * 71    98/09/19 14:31 Speter
 * Upped the ad hoc gun colliison energy.
 * 
 * 70    9/17/98 10:47p Pkeet
 * Put in a fix for guns disappearing when muzzle flashes appear.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Gun.hpp"
#include "HitSpang.hpp"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/GeomDBase/RayCast.hpp"
#include "Lib/Physics/PhysicsSystem.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgAudio.hpp"
#include "Lib/Audio/AudioDaemon.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Std/Hash.hpp"

#include "Lib/EntityDBase/MessageTypes/MsgCollision.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Game/AI/AIMain.hpp"

#include "Lib/Std/Random.hpp"
#include "Lib/Renderer/Particles.hpp"


static CRandom rndGun;
bool    g_bUnlimitedAmmo = FALSE; //cheat can toggle this

// 
// Macros.
//

// Flag for enabling hit spangs.
#define bENABLE_SPANGS	(0)

// Flag for dumping gun collision information to a text file.
#define bREPORT_COLLISION (0)

// Flag for activating particles in place of hit spangs.
#define bUSE_PARTICLES_ALWAYS (0)


//
// Module specific class definitions.
//

//
// Class implementation.
//

//*********************************************************************************************
//
// CGun implementation.
//

	//*****************************************************************************************
	CGun::CGun()
	{
		// Register this entity with the message types it needs to receive.
		CMessageStep::RegisterRecipient(this);
		CMessageMove::RegisterRecipient(this);
	}

		
	//*****************************************************************************************
	CGun::CGun
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CEntity(pgon, pload, h_object, pvtable, pinfo)
	{
		iMaxAmmo			= 100;
		iAmmo				= iMaxAmmo;			
		v3BarrelOffset		= CVector3<>(0,0,0);
		sCanShoot			= 0.0f;
		bAutoFire			= false;
		bAltAmmoCount		= false;
		sndhndLastSample	= 0;
		rWakeUp				= 100.0f;

		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_object)
		{
			bFILL_INT(iAmmo, esAmmo);
			bFILL_INT(iMaxAmmo, esMaxAmmo);

			// Clamp ammo.
			if (iAmmo > iMaxAmmo)
				iAmmo = iMaxAmmo;

			float fROF = 1.0f;
			bFILL_FLOAT(fROF, esROF);
			Assert(fROF > 0.01f);
			sTimeBetweenShots = 1.0f / fROF;

			bFILL_BOOL(bAutoFire, esAutoFire);

			bFILL_BOOL(bAltAmmoCount, esAltAmmoCount);

			bFILL_FLOAT(gdGunData.fDamage, esDamage);

			bFILL_FLOAT(gdGunData.fTranqDamage, esTranqDamage);

			bFILL_FLOAT(gdGunData.fExtension, esRange);

			bFILL_FLOAT(gdGunData.fImpulse, esPush);

			const CEasyString* pestr = 0;
			bFILL_pEASYSTRING(pestr, esBarrel);
			if (pestr)
			{
				// Locate barrel object.
				CGroffObjectName* pgon = pload->goiInfo.pgonFindObject(pestr->strData());

				if (pgon)
				{
					// Barrel in world space
					CVector3<> v3_barrel_world = ::pr3Presence(*pgon).v3Pos;

					// Find the local space barrel location.
					v3BarrelOffset = v3_barrel_world / pr3GetPresence();
				}
				else
				{
					#if VER_TEST
						char str_buffer[256];
						sprintf(str_buffer, 
								"%s\n\nGun references barrel '%s' that does not exist in GRF.\n", 
								__FILE__,
								pestr->strData() );

						if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
							DebuggerBreak();
					#endif
				}
			}

			pestr = 0;

			bFILL_pEASYSTRING(pestr, esTActionSample);
			if (pestr)
			{
				sndhndGunSample = sndhndHashIdentifier(pestr->strData());
			}
			else
			{
				sndhndGunSample = 0;
			}

			pestr = 0;
			bFILL_pEASYSTRING(pestr, esEmptyClipSample);
			if (pestr)
			{
				sndhndEmptySample = sndhndHashIdentifier(pestr->strData());
			}
			else
			{
				sndhndEmptySample = 0;
			}

			pestr = 0;
			bFILL_pEASYSTRING(pestr, esRingSample);
			if (pestr)
			{
				// The sample must be an autofire sample before we can have a ring sample...
				Assert(bAutoFire);

				sndhndRingSample = sndhndHashIdentifier(pestr->strData());
			}
			else
			{
				sndhndRingSample = 0;
			}

			//
			// recoild Force
			// A zero force means no recoil at all
			//
			fRecoilForce = 0.0f;
			bFILL_FLOAT(fRecoilForce, esRecoilForce);

			bFILL_FLOAT(rWakeUp, esWakeUp);

			//
			// Recoil impulse 
			//
			pestr = 0;
			bFILL_pEASYSTRING(pestr, esRecoilOrg);
			if (pestr)
			{
				// Locate recoil object.
				CGroffObjectName* pgon = pload->goiInfo.pgonFindObject(pestr->strData());

				if (pgon)
				{
					// world space presence of the recoil object
					CPresence3<> pr3 = ::pr3Presence(*pgon);

					// local presence of the recoil object
					pr3 /= pr3GetPresence();

					// get the position of the recoil object in local space
					v3RecoilOffset = pr3.v3Pos;

					// Rotate a forward direction vector by the recoil object rotation to get a local
					// impulse vector. The Y axis of the recoil object is the direction in which the
					// impulse is applied.
					v3RecoilDir = d3YAxis * pr3.r3Rot;
				}
				else
				{
					#if VER_TEST
						char str_buffer[256];
						sprintf(str_buffer, 
								"%s\n\nGun references recoil origin '%s' that does not exist in GRF.\n", 
								__FILE__,
								pestr->strData() );

						if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
							DebuggerBreak();
					#endif
				}
			}
			else
			{
				fRecoilForce = 0.0f;
			}

			// zero out the flash mesh array
			memset(pmuzflMeshes, 0, sizeof(pmuzflMeshes));

			//
			// Muzzle flash meshes
			//

			u4MeshCount = 0;
			while (u4MeshCount<iMAX_MUZZLE_FLASH)
			{
				pestr = 0;
				bFILL_pEASYSTRING(pestr, (ESymbol)((uint32)esMFlashObject0 + u4MeshCount));

				// if we have not found this mesh string then stop looking
				if (!pestr)
					break;

				pmuzflMeshes[u4MeshCount] = CMuzzleFlash::pmuzflFindNamedMuzzleFlash( pestr->strData() );

#if VER_TEST
				if (pmuzflMeshes[u4MeshCount] == 0)
				{
					char str_buffer[1024];
					sprintf(str_buffer, 
							"%s\n\nGun references muzzle object '%s' that is not of type CMuzzleFlash.\n", 
							__FILE__,
							pestr->strData() );

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}
#endif

				u4MeshCount++;
			}

			//
			// Muzzle flash location
			//
			pestr = 0;
			bFILL_pEASYSTRING(pestr, esMFlashOrg);
			if (pestr)
			{
				// Locate the muzzle flash origin object.
				CGroffObjectName* pgon = pload->goiInfo.pgonFindObject(pestr->strData());

				if (pgon)
				{
					// Barrel in world space
					CVector3<> v3_flash_world = ::pr3Presence(*pgon).v3Pos;

					// Find the local space barrel location.
					v3FlashOffset = v3_flash_world / pr3GetPresence();
				}
				else
				{
					#if VER_TEST
						char str_buffer[256];
						sprintf(str_buffer, 
								"%s\n\nGun references muzzle flash origin '%s' that does not exist in GRF.\n", 
								__FILE__,
								pestr->strData() );

						if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
							DebuggerBreak();
					#endif
				}
			}
			else
			{
				// if there is no flah object, set the mesh count to zero to stop the muzzle flash being drawn
				u4MeshCount = 0;
			}

			u4NextMesh = 0;
			pmuzflCurrentFlash = NULL;

			sFlashDuration = 0.0f;
			bFILL_FLOAT(sFlashDuration, esMFlashDuration);

			bRandomRotate = false;
			bFILL_BOOL(bRandomRotate, esRandomRotate);
//			InitHandle(esKick, "Kick");
		}
		END_OBJECT_HANDLE;
		END_TEXT_PROCESSING;

		// Register this entity with the message types it needs to receive.
		CMessageStep::RegisterRecipient(this);
		CMessageMove::RegisterRecipient(this);
	}

	//*****************************************************************************************
	CGun::~CGun()
	{
		CMessageMove::UnregisterRecipient(this);
		CMessageStep::UnregisterRecipient(this);
	}


	//*****************************************************************************************
	bool CGun::bUse(bool b_repeat)
	{
		// Return false when out of ammo or when first experiencing a click, but only after some time has passed
		// after our last fire attempt.
		bool b_swing_the_gun = iAmmo < 0 && CMessageStep::sStaticTotal > sCanShoot;

		// Fire the gun!
   		if (CMessageStep::sStaticTotal > sCanShoot)
		{
			if (iAmmo > 0)
			{
				if (sndhndGunSample)
				{
					CMessageAudio	msg
					(
						sndhndGunSample,
						bAutoFire?eamATTACHED_EFFECT:eamPOSITIONED_EFFECT,
						NULL,						// sender (NULL if using play function)
						padAudioDaemon,				// receiver (NULL if using play function)
						this,						// parent of the effect (can be NULL for stereo)
						0.0f,						// volume 0dBs
						0.5f,						// attenuation (only for pseudo/real 3D samples)
						AU_SPATIAL3D,				// spatial type
						360.0f,						// fustrum angle (real 3D only)
						-15.0f,						// outside volume (real 3D only)
						bAutoFire,					// looped
						0,							// loop count
						10000.0f					// distance before sound is stopped (pseudo/real 3D only)
					);

					// when starting a new looped gun sample we need to set the ring sample for when the
					// loop stops.
					if (bAutoFire)
					{
						msg.SetStopSample(sndhndRingSample);
					}

					msg.SetDeferLoad(false);

					bImmediateSoundEffect(msg,NULL);

					sndhndLastSample = sndhndGunSample;
				}

				// Adjust the presence to account for the bullet ejection point (barrel offset)
				CPresence3<> pr3 = pr3GetPresence();
				pr3.v3Pos = pr3.v3Pos + (v3BarrelOffset * pr3.r3Rot) * pr3.rScale;

				// Fire from the adjusted world position.
				CInstance* pins_hit = gdGunData.pinsFire(this, pr3, false, this);
				if (pins_hit)
				{
					dout << "Gun hit " << pins_hit->strGetInstanceName() << "\n";
				}
				else
				{
					dout << "Missed.\n";
				}

				//
				// Apply an impulse to the gun in the hand
				//

				if (fRecoilForce>0)
				{
					pphiGetPhysicsInfo()->ApplyImpulse
					(
						this, 0,
						v3RecoilOffset * pr3GetPresence(),
						(v3RecoilDir * pr3GetPresence().r3Rot) * (fRecoilForce * 0.1)
					);
				}

				// Let dinos know.
				gaiSystem.AlertAnimals(gpPlayer->apbbBoundaryBoxes[(int)ebbBODY], rWakeUp);
				gaiSystem.Handle3DStartle(v3Pos(), rWakeUp * 0.5f);

				//
				// If there are muzzle flash meshes then display one
				//
				if (u4MeshCount>0)
				{
					CPlacement3<>	p3;
					CPresence3<>	pr3 = pr3GetPresence();

					// If we have a muzzle flash on the screen, remove it before setting up another
					if (pmuzflCurrentFlash)
					{
						pmuzflCurrentFlash->bVisible = false;
						wWorld.Remove(pmuzflCurrentFlash);
						pmuzflCurrentFlash = NULL;
					}
					// add our flash mesh to the world database
					wWorld.Add(pmuzflMeshes[u4NextMesh], true);
					pmuzflMeshes[u4NextMesh]->bVisible = true;

					// position the flash on the end of the gun

					p3.v3Pos = pr3.v3Pos + (v3FlashOffset * pr3.r3Rot) * pr3.rScale;

					if (bRandomRotate)
						p3.r3Rot = CRotate3<>( CDir3<>(0.0f,1.0f,0.0f), CAngle(rndGun(0,2*dPI))) * pr3.r3Rot;
					else
						p3.r3Rot = pr3.r3Rot;

					//p3.v3Pos = v3_world_hit_pos;
					//p3.r3Rot = CRotate3<>( CDir3<>(0.0f,1.0f,0.0f), d3_hit_normal);
					//pinsMeshes[u4NextMesh]->SetScale(2.0f);

					pmuzflMeshes[u4NextMesh]->Move(p3);

					// rememeber the current flash so we can remove it later
					pmuzflCurrentFlash = pmuzflMeshes[u4NextMesh];
					sFlashRemove = CMessageStep::sStaticTotal + sFlashDuration;

					// cycle through the available flash meshes.
					u4NextMesh++;
					if (u4NextMesh>=u4MeshCount)
						u4NextMesh = 0;
				}

				// Decrement the ammo count unless cheat is on.
                if (!g_bUnlimitedAmmo)
                {
				    --iAmmo;
                }

				// Make sure we don't fire until we've reloaded.
				sCanShoot = CMessageStep::sStaticTotal + sTimeBetweenShots;

                //let stop Anne's ceaseless chatter with unlimited ammo
                if (!g_bUnlimitedAmmo)
                {
				    if (bAltAmmoCount)
				    {
					    // If firing just brought us to 50% or below, then say so.
					    if (iAmmo <= iMaxAmmo*0.5f && (iAmmo+1) > iMaxAmmo*0.5f)
					    {
						    gpPlayer->TalkAboutApproximateAmmo(1);
					    }
					    // If firing just brought us to 10% or below, then say so.
					    else if (iAmmo <= iMaxAmmo*0.1f && (iAmmo+1) > iMaxAmmo*0.1f)
					    {
						    gpPlayer->TalkAboutApproximateAmmo(0);
					    }
				    }
				    else
				    {
					    // Let Anne wax philosophical about ammunition.
					    gpPlayer->MaybeTalkAboutAmmo(iAmmo, iMaxAmmo);
				    }
                }
			}
			else if (iAmmo == 0)		// No ammo!
			{
				// Out of ammo, so play the empty clip sample.
				if (sndhndEmptySample)
				{
					CMessageAudio	msg
					(
						sndhndEmptySample,
						bAutoFire?eamATTACHED_EFFECT:eamPOSITIONED_EFFECT,
						NULL,						// sender (NULL if using play function)
						padAudioDaemon,				// receiver (NULL if using play function)
						this,						// parent of the effect (can be NULL for stereo)
						0.0f,						// volume 0dBs
						0.5f,						// attenuation (only for pseudo/real 3D samples)
						AU_SPATIAL3D,				// spatial type
						360.0f,						// fustrum angle (real 3D only)
						-15.0f,						// outside volume (real 3D only)
						bAutoFire,					// looped
						0,							// loop count (pseudo/real 3D only)
						10000.0f					// distance before sound is stopped (pseudo/real 3D only)
					);

					msg.SetDeferLoad(false);

					bImmediateSoundEffect(msg,NULL);

					sndhndLastSample = sndhndEmptySample;
				}

				// Once out of ammo, do not start winging the gun for a short time-  indicated by sCanShoot.
				sCanShoot = CMessageStep::sStaticTotal + 1.0f;

				// Let Anne wax philosophical about ammunition.
				gpPlayer->MaybeTalkAboutAmmo(iAmmo, iMaxAmmo);

				// Set ammo negative so we know that we've hit the failed sample.
				iAmmo = -1;
			}
		}
		else
		{
			//
			// It is not yet time to shoot again, but if we have an automatic weapon it must send a message
			// to the audio daemon to keep it going. Therefore, send an audio message with the ID of the last
			// sample played. This will either loop the gun sample or will loop the empty clip.
			// NOTE: THE SAMPLES (BOTH FIRE AND EMPTY) MUST BE LOOPABLE SAMPLES.
			//
			if ((bAutoFire) && (sndhndLastSample))
			{
				CMessageAudio	msg
				(
					sndhndLastSample,
					eamATTACHED_EFFECT,			// type of sound
					NULL,						// sender (NULL if using play function)
					padAudioDaemon,				// receiver (NULL if using play function)
					this,						// parent of the effect (can be NULL for stereo)
					0.0f,						// volume 0dBs
					0.5f,						// attenuation (only for pseudo/real 3D samples)
					AU_SPATIAL3D,				// spatial type
					360.0f,						// fustrum angle (real 3D only)
					-15.0f,						// outside volume (real 3D only)
					true,						// looped
					0,							// loop count
					10000.0f					// distance before sound is stopped (pseudo/real 3D only)
				);

				msg.SetDeferLoad(false);

				//
				// When sending consecutive messages to keep a sample looping you do not have to set
				// the ring sample. The ring sample only has to be set when the first message is sent
				// but setting it every time will do no harm.
				//
				bImmediateSoundEffect(msg,NULL);
			}
		}

		// There was a use action.
		return !b_swing_the_gun;
	}

	//*****************************************************************************************
	void CGun::PickedUp()
	{
		// The player should say something about how much ammo we have.
		gpPlayer->TalkAboutAmmoOnPickup(iAmmo, iMaxAmmo, bAltAmmoCount);
	}

	//*****************************************************************************************
	// Step function is required to remove the muzzle flash
	void CGun::Process(const CMessageStep& msg)
	{
		if (pmuzflCurrentFlash)
		{
			if (CMessageStep::sStaticTotal>sFlashRemove)
			{
				//
				// If our time for this flash has expired then remove the muzzle flash mesh
				//
				pmuzflCurrentFlash->bVisible = false;
				wWorld.Remove(pmuzflCurrentFlash);
				pmuzflCurrentFlash = NULL;
			}
		}

		CHitSpang::ProcessStep(msg);
	}

	
	extern CProfileStat psMoveMsgGun;
	
	//*****************************************************************************************
	void CGun::Process(const CMessageMove& msg)
	{
		CTimeBlock tmb(&psMoveMsgGun);

		// Has some other system moved then gun?? Like the physics??
		// If so we must move the muzzle flash if it is present

		if ((pmuzflCurrentFlash) && (msg.pinsMover == this) && (msg.etType == CMessageMove::etMOVED))
		{
			CPlacement3<>	p3;
			CPresence3<>	pr3 = pr3GetPresence();

			// position the flash on the end of the gun
			p3.v3Pos = pr3.v3Pos + (v3FlashOffset * pr3.r3Rot) * pr3.rScale;
			p3.r3Rot = pr3.r3Rot;
			pmuzflCurrentFlash->Move(p3);
		}
	}


#if VER_TEST
	//*****************************************************************************************
	int CGun::iGetDescription(char* pc_buffer, int i_buffer_len)
	{
		CInstance::iGetDescription(pc_buffer, i_buffer_len);

		char str_buffer[512];

		sprintf(str_buffer,"iAmmo\t%d\n", iAmmo);
		strcat(pc_buffer, str_buffer);
	
		sprintf(str_buffer,"v3BarrelOffset\t%2.2f,%2.2f,%2.2f\n", v3BarrelOffset.tX, v3BarrelOffset.tY, v3BarrelOffset.tZ);
		strcat(pc_buffer, str_buffer);

		sprintf(str_buffer,"Barrel (world units)\t%2.2f,%2.2f,%2.2f\n", 
			fGetScale() * v3BarrelOffset.tX, 
			fGetScale() * v3BarrelOffset.tY, 
			fGetScale() * v3BarrelOffset.tZ);
		strcat(pc_buffer, str_buffer);

		sprintf(str_buffer,"v3RecoilOffset\t%2.2f,%2.2f,%2.2f\n", v3RecoilOffset.tX, v3RecoilOffset.tY, v3RecoilOffset.tZ);
		strcat(pc_buffer, str_buffer);

		sprintf(str_buffer,"v3RecoilDir\t%2.2f,%2.2f,%2.2f\n", v3RecoilDir.tX, v3RecoilDir.tY, v3RecoilDir.tZ);
		strcat(pc_buffer, str_buffer);

		sprintf(str_buffer,"sTimeBetweenShots\t%f\n", sTimeBetweenShots);
		strcat(pc_buffer, str_buffer);

		sprintf(str_buffer,"sCanShoot\t%f\n", sCanShoot);
		strcat(pc_buffer, str_buffer);

		sprintf(str_buffer,"fDamage\t%f\n", gdGunData.fDamage);
		strcat(pc_buffer, str_buffer);
		
		sprintf(str_buffer,"fTranqDamage\t%f\n", gdGunData.fTranqDamage);
		strcat(pc_buffer, str_buffer);

		int i_len = strlen(pc_buffer);
		Assert(i_len < i_buffer_len);
		return i_len;
	}
#endif

	//*****************************************************************************************
	bool CGun::bValidateGunProperties
	(
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				pload		// The loader.
	)
	//*********************************
	{
		const CEasyString*	pestr = NULL;
		uint32				u4_count = 0;

		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_object)
		{
			while (u4_count<iMAX_MUZZLE_FLASH)
			{
				pestr = 0;
				bFILL_pEASYSTRING(pestr, (ESymbol)((uint32)esMFlashObject0 + u4_count));

				// if we do not find the next text prop then stop looking
				if (!pestr)
					break;


				//
				// Make sure the muzzle flash exists in the world
				//
#if VER_TEST
				CGroffObjectName* pgon = pload->goiInfo.pgonFindObject( pestr->strData() );

				if (pgon == 0)
				{
					char str_buffer[1024];
					sprintf(str_buffer, 
							"%s\n\nGun references muzzle flash '%s' that does not exist in GRF.\n", 
							__FILE__,
							pestr->strData() );

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}
#endif

				// look for the specified object in the world, if it does not exist we cannot
				// load this gun until the next pass.
				CMuzzleFlash* pmuzfl = CMuzzleFlash::pmuzflFindNamedMuzzleFlash( pestr->strData() );
				if (!pmuzfl)
					return false;

				u4_count++;
			}
		}
		END_OBJECT_HANDLE;
		END_TEXT_PROCESSING;

		return true;
	}


	//*****************************************************************************************
	char * CGun::pcSave(char * pc) const
	{
		// Save the instance location data.
		pc = CInstance::pcSave(pc);

		// Gun specific data.
		pc = pcSaveT(pc, iAmmo);
		pc = pcSaveT(pc, sCanShoot);

		return pc;
	}

	//*****************************************************************************************
	const char *CGun::pcLoad(const char * pc)
	{
		// Load the instance location data.
		pc = CInstance::pcLoad(pc);

		// Gun specific data.
		pc = pcLoadT(pc, &iAmmo);
		pc = pcLoadT(pc, &sCanShoot);
		// remove any existing muzzle flash, the instance for any visible muzzle flash will be
		// remove within the next step message.
		sFlashRemove = 0.0f;
		return pc;
	}


//*********************************************************************************************
//
// CGunData implementation.
//

	//*****************************************************************************************
	//
	// Constructors.
	//

	//*****************************************************************************************
	CGunData::CGunData()
		: fExtension(50.0f), fImpulse(100.0f), fExtensionPull(50.0f), fImpulsePull(100.0f), fDamage(25.0f)
	{
		
	}

	//*****************************************************************************************
	CInstance* CGunData::pinsFire
	(
		CGun* pgun,
		const CPresence3<>& pr3_gun, 
		bool b_pull, 
		const CInstance* pins_immune
	)
	//*********************************
	{
		// Use the raycast to find the first object of collision.
		float f_distance = (b_pull) ? (fExtensionPull) : (fExtension);

		//
		// With the modified ray check for objects
		//
		CRayCast rc(pr3_gun, 0.001f, f_distance);

		// Iterate through the instances.
		SObjectLoc* pobl;
		while (pobl = rc.poblNextIntersection())
		{
			Assert(pobl->pinsObject);

			// Skip if it's immune.
			if (pins_immune == pobl->pinsObject)
				continue;
						
			// Skip if it is the player.
			if (ptCast<CPlayer>(pobl->pinsObject))
				continue;

			//
			// Found something.
			//

			//
			// Now tell the physics system about this object.
			// If pulling, reverse the impulse direction, and use special magnitude.
			//

			// Randomise the collision strength, from 50% to 100% max.
			// Apply hacky scale, as old impulse values were in a different scale.
			// These are actual units of momentum.
			float f_impulse	 = (b_pull ? -fImpulsePull : fImpulse) * rndGun(0.05, 0.10);

			// Apply hacky scale, as old impulse values were in a different scale.
			CVector3<> v3_impulse = CVector3<>(0, f_impulse, 0) * pr3_gun.r3Rot;

			pobl->pinsObject->pphiGetPhysicsInfo()->ApplyImpulse
			(
				pobl->pinsObject, pobl->iSubObject, pobl->v3Location, v3_impulse
			);

			//
			// Send a colliison message to the physics system.
			//
		#if !bUSE_PARTICLES_ALWAYS

			// Ad hoc computation of energy. Multiply impulse by damage, and apply an extra scale.
			float f_energy = f_impulse * Max(fDamage, fTranqDamage) * 10.0;
			CMessageCollision msgcoll
			(
				pobl->pinsObject, 
				0,						// Bullet.
				f_energy,				// Max energy.
				f_energy,				// Hit energy.
				0.0,					// Slide energy.
				pobl->v3Location,
				0.0f,
				0.0f,
				pgun,					// Sender.
				&rc
			);
			msgcoll.Dispatch();
		#endif !bUSE_PARTICLES_ALWAYS

		#if bUSE_PARTICLES_ALWAYS
			{
				v3_impulse.Normalise();
				v3_impulse *= -1.0f;
				v3_impulse = CVector3<>(0.0f, 0.0f, 1.0f);

				CCreateParticles crt;
		
				crt.eptParticleType      = eptDust;
				crt.ppMin.fNumParticles  = crt.ppMax.fNumParticles = 25;
				crt.ppMin.fSpinRadPerSec = dDegreesToRadians(30.0f);
				crt.ppMax.fSpinRadPerSec = dDegreesToRadians(360.0f);
				crt.clrColour            = CColour(255, 255, 0);
				crt.ppMin.fVelocity      = 0.5f;
				crt.ppMax.fVelocity      = 1.5f;
				crt.ppMin.sLifespan      = 3.0f;
				crt.ppMax.sLifespan      = 10.0f;
				crt.ppMin.fLength        = 0.0f;
				crt.ppMax.fLength        = 0.0f;
				crt.ppMin.fSize          = 0.010f;
				crt.ppMax.fSize          = 0.020f;
				crt.ppMin.fAccelStuck    = 2.0f;
				crt.ppMax.fAccelStuck    = 2.0f;
				crt.ppMin.fAccelUnstuck  = 2.0f;
				crt.ppMax.fAccelUnstuck  = 2.0f;
				crt.ppMin.fSpreadRadians = dDegreesToRadians(15.0f);
				crt.ppMax.fSpreadRadians = dDegreesToRadians(30.0f);

				crt.Create(pobl->v3Location, v3_impulse);
			}
		#else
			#if bENABLE_SPANGS
				// Call the hit spang class with the collision object
				CHitSpang::ProcessHit(pobl);
			#endif // bUSE_PARTICLES_ALWAYS
		#endif // bUSE_PARTICLES_ALWAYS

			// Exit if collision was successful.
			return pobl->pinsObject;
		}

		// we have hit nothing..
		return 0;
	}


	//*****************************************************************************************
	CInstance* CGun::pinsCopy() const
	{
		CGun *pgun_copy = new CGun();	

		*pgun_copy = *this;

		// And instance pickup magnets or similar add-ons.
		pgun_copy->CopyExternalData(this);

		return pgun_copy;
	}




//*********************************************************************************************
//
// CMuzzleFlash implementation.
//
TMuzzleFlashList	CMuzzleFlash::mfMeshList;


	//*****************************************************************************************
	//
	// Constructors.
	//
	CMuzzleFlash::CMuzzleFlash
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CInstance(pgon, pload, h_object, pvtable, pinfo)
	//*********************************
	{
		// add all muzzle flash meshes to the list of available muzzle flash
		mfMeshList.push_back( this );
		bVisible = false;
		SetFlagShadow(false);
	}


	//*****************************************************************************************
	//
	// Destructors.
	//
	CMuzzleFlash::~CMuzzleFlash
	(
	)
	//*********************************
	{
		// This mesh must not be visible
		Assert( bVisible == false);

		// when a muzzle flash is deleted remove it from our list
		for (TMuzzleFlashList::iterator i = mfMeshList.begin(); i!=mfMeshList.end(); ++i)
		{
			// Remove the muzzle flash that is being deleted from the global list
			if ( (*i) == this)
			{
				mfMeshList.erase(i);
				break;
			}
		}
	}

	//*****************************************************************************************
	CMuzzleFlash* CMuzzleFlash::pmuzflFindNamedMuzzleFlash
	(
		const char* str
	)
	//*********************************
	{
		uint32 u4_hash = u4Hash(str);

		for (TMuzzleFlashList::iterator i = mfMeshList.begin(); i!=mfMeshList.end(); ++i)
		{
			if ( (*i)->u4GetUniqueHandle() == u4_hash)
			{
				return (*i);
			}
		}

		return NULL;
	}

	//*****************************************************************************************
	void CMuzzleFlash::ClearMuzzleFlashList
	(
	)
	//*********************************
	{
		bool b_done = false;

		while (!b_done)
		{
			b_done = true;

			// when a muzzle flash is deleted remove it from our list
			for (TMuzzleFlashList::iterator i = mfMeshList.begin(); i!=mfMeshList.end(); ++i)
			{
				if ( (*i)->bVisible)
				{
					// Remove this guy from the world before we delete it...
					wWorld.Remove( (*i) );
					(*i)->bVisible = false;
				}

				// delete the muzzle flash class
				delete (*i);
				b_done = false;
				break;
			}
		}

		//
		// Now we have deleted all muzzle flashes the list should be empty
		//
		Assert( mfMeshList.size() == 0);
	}

	//*****************************************************************************************
	void CMuzzleFlash::ResetMuzzleFlash
	(
	)
	//*********************************
	{
		// Ensure that there is nothing left from a previous game
		Assert( mfMeshList.size() == 0);
	}
	
	//*****************************************************************************************
	bool CMuzzleFlash::bCanHaveChildren()
	{
		return false;
	}


//
// Global variables.
//
CGunData gdGlobalGunData;
