/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: The implementation of DaemonScript.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/DaemonScript.cpp                                   $
 * 
 * 21    2/11/97 3:59p Blee
 * Change back to drive:filepath.  For some reason mmio does not like absolute filepaths
 * without the drive.  Investigate further.
 * 
 * 20    97/02/11 10:34 Speter
 * Changed all pathnames from "C:/etc" to "/etc".
 * 
 * 19    2/03/97 10:34p Agrant
 * Query.hpp and MessageTypes.hpp have been split into
 * myriad parts so that they may have friends.
 * Or rather, so compile times go down.
 * Look for your favorite query in Lib/EntityDBase/Query/
 * Look for messages in                Lib/EntityDBase/MessageTypes/
 * 
 * 18    1/31/97 7:56p Agrant
 * Increase fake ambient sound radius
 * 
 * 17    97/01/31 4:50p Pkeet
 * Added default ambient sound.
 * 
 * 16    97/01/31 12:42p Pkeet
 * Cleaned code up.
 * 
 * 15    20-12-96 1:06p Pkeet
 * changed final sample played from 34climate.wav  to 33suits.wav, for better effect.
 * 
 * 14    17-12-96 4:32p Pkeet
 * 
 * 13    17-12-96 4:21p Pkeet
 * corrected file reference to "35stepout.wav"
 * added player damage sounds
 * added player-hits-dino sounds
 * 
 * 12    17-12-96 1:44p Pkeet
 * 
 * 11    12/16/96 9:31p Pkeet
 * Added dino hits.
 * 
 * 10    16-12-96 5:53p Pkeet
 * added test function to see if voice samples play at all
 * 
 * 9     16-12-96 4:00p Pkeet
 * Added path names for sound clips.
 * 
 * 8     12/16/96 3:27p Pkeet
 * Changed the interface to the player hit function giving the number of hit points endured and
 * the number of hit points left.
 * 
 * 7     16-12-96 12:57p Pkeet
 * Added initial demo triggers.
 * 
 * 6     12/14/96 3:34p Pkeet
 * Disabled the windows example stuff.
 * 
 * 5     12/14/96 3:30p Pkeet
 * Added the 'RegisterTimeTrigger' functions with examples.
 * 
 * 4     11/18/96 4:06p Pkeet
 * Added the 'PlayerIsHit' function and sample code.
 * 
 * 3     11/18/96 1:52p Pkeet
 * Added a test for voice-overs.
 * 
 * 2     11/16/96 7:30p Pkeet
 * Changed the radius on the test object to 0.25.
 * 
 * 1     11/16/96 4:59p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgAudio.hpp"
#include "DaemonScript.hpp"
#include "Lib/EntityDBase/Instance.hpp"


//
// Function implementations.
//

//*********************************************************************************************
bool bDelay1()
{
	Say("Delay 1");
	//MessageBeep(0);
	return true;
}

CInstance inst;

//*********************************************************************************************
bool bStartAmbient()
{
	Say("Starting ambient sound.");
	CMessageAudio msg(CREATE_ENVIRONMENT, "c:/Jp2_pc/bindata/audiofiles/forest/ForestBG1.wav",
		&inst, 20.0, 20.0, 50.0, 50.0);
	msg.Dispatch();
	return true;
}

//*********************************************************************************************
bool bDelay0()
{
	Say("Delay 0");
	//MessageBeep(0);
	RegisterTimeTrigger("Delay 1", bDelay1, 10.0);
	return true;
}

//*********************************************************************************************
bool bMakeNoise()
{
	Say("******** MAKING NOISE YAHOO ********");
	VoiceOver("Explode.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx03()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/03wait.wav");
	return true;
}

//*********************************************************************************************

bool bTrigProx04()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/04needs.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx06()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/06canal.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx07()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/0721.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx08()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/08refine.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx10()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/10survey.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx11()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/11bill.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx12()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/12what.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx13()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/13great.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx14()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/14infra.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx17()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/17regroup.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx23()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/23loseit.wav");
	return true;
}

//*********************************************************************************************

bool bTrigProx24()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/24noway.wav");
	return true;
}





//*********************************************************************************************
bool bDelayTrig33()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/33suits.wav");

	return true;
}



//*********************************************************************************************
bool bDelayTrig26()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/26online.wav");
	RegisterTimeTrigger("TrigTime33", bDelayTrig33, 2.0);

	return true;
}



//*********************************************************************************************
bool bTrigProx25()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/25reactor.wav");
	RegisterTimeTrigger("TrigTime26", bDelayTrig26, 1.0);
	return true;
}


//*********************************************************************************************

bool bTrigProx29()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/29jeep.wav");
	return true;
}


//*********************************************************************************************

bool bTrigProx30()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/30stepout.wav");
	return true;
}



//*********************************************************************************************

bool bTrigProx31()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/31south.wav");
	return true;
}

//*********************************************************************************************

bool bTrigProx35()
{
	VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Hammond/35evac.wav");
	return true;
}


//
// Register function.
//

//*********************************************************************************************
void RegisterScript(TPFMap& rpfmap)
{
	//
	// Register your function with a character string here!
	//	
	RegisterPlayerTrigger(rpfmap, "Make Noise", bMakeNoise, 0.25, true);
	Register(rpfmap, "TrigProx03", bTrigProx03);
	Register(rpfmap, "TrigProx04", bTrigProx04);
	Register(rpfmap, "TrigProx06", bTrigProx06);
	Register(rpfmap, "TrigProx07", bTrigProx07);
	Register(rpfmap, "TrigProx08", bTrigProx08);
	Register(rpfmap, "TrigProx10", bTrigProx10);
	Register(rpfmap, "TrigProx12", bTrigProx12);
	Register(rpfmap, "TrigProx13", bTrigProx13);
	Register(rpfmap, "TrigProx14", bTrigProx14);
	Register(rpfmap, "TrigProx17", bTrigProx17);
	Register(rpfmap, "TrigProx23", bTrigProx23);
	Register(rpfmap, "TrigProx24", bTrigProx24);
	Register(rpfmap, "TrigProx25", bTrigProx25);
	Register(rpfmap, "TrigProx29", bTrigProx29);
	Register(rpfmap, "TrigProx31", bTrigProx31);
	Register(rpfmap, "TrigProx35", bTrigProx35);
	
	// Example of registering a time trigger.
	//RegisterTimeTrigger(rpfmap, "Delay 0", bDelay0, 2.5);
	//VoiceOver("/Jp2_pc/bindata/audiofiles/voice/Hammond/28crates.wav");

	// Initialize ambient sound.
	RegisterTimeTrigger(rpfmap, "Start ambient", bStartAmbient, 0.1);
}


//
// Player functions.
//

//*********************************************************************************************
void PlayerIsHit(float fHitPointsLeft, float fHitPoints)
{
	//minor hit
	if (bBetween(fHitPoints, 0.0, 12.0))
	{	
		switch(iRandom (1, 5))
		{
			case 1:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/small_pain/spgrunt2.wav");
				break;
			case 2:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/small_pain/spow.wav");
				break;
			case 3:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/small_pain/spgrunt3.wav");
				break;
			case 4:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/small_pain/spgrunt1.wav");
				break;
			case 5:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/small_pain/spgrunt4.wav");
				break;
		}
	}

	// medium hit
	if (bBetween(fHitPoints, 3.0, 6.0))
	{	
		switch(iRandom (1, 9))
		{
			case 1:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/small_pain/spgrunt5.wav");
				break;
			case 2:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/small_pain/spbast.wav");
				break;
			case 3:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/small_pain/spdamn.wav");
				break;
		}
	}

	// Big hit.
	if (bBetween(fHitPoints, 6.0, 8.0))
	{	
		switch(iRandom (1, 10))
		{
			case 1:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/big_pain/bpgrunt1.wav");
				break;
			case 2:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/big_pain/bpgrunt2.wav");
				break;
			case 3:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/big_pain/bpson.wav");
				break;
			case 4:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/big_pain/bpjesus.wav");
				break;
		}
	}

	// Dead.
	if (fHitPointsLeft == 0.0)
	{
	}
}

//*********************************************************************************************
void DinoIsHit(float fHitPointsLeft, float fHitPoints)
{
	if (fHitPoints >= 5.0)
	{
		switch(iRandom (1, 10))
		{
			case 1:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/dino_damage/ddback1.wav");
				break;
			case 2:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/dino_damage/ddcome.wav");
				break;
			case 3:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/dino_damage/ddback2.wav");
				break;
			case 4:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/dino_damage/ddgoon.wav");
				break;
			case 5:
				VoiceOver("c:/jp2_pc/bindata/audiofiles/voice/Anne/Battling/dino_damage/ddyeah.wav");
				break;
		}
	}
}
