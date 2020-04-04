/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of AI Test.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/AITest.cpp                                                   $
 * 
 * 76    9/23/98 10:18p Agrant
 * better query for test shapes
 * 
 * 75    9/19/98 1:39a Agrant
 * code rot
 * 
 * 74    9/16/98 12:36a Agrant
 * more code rot
 * 
 * 73    9/02/98 5:20p Agrant
 * keeping up with the Joneses
 * 
 * 72    8/27/98 5:32p Agrant
 * AI Test App is not Trespasser
 * 
 * 71    8/23/98 3:03a Agrant
 * AI Test compile fixes
 * 
 * 70    6/13/98 9:18p Agrant
 * More code hacking to handle GUIApp changes
 * 
 * 69    6/08/98 5:46p Agrant
 * Fixes for the new AI Type system
 * 
 * 68    5/17/98 8:03p Agrant
 * numerous AI test app interface improvements
 * 
 * 67    5/16/98 9:58a Agrant
 * viewport fixes to allow moving the view volume
 * 
 * 66    5/10/98 6:20p Agrant
 * allow loading scenes
 * 
 * 65    5/09/98 11:17a Agrant
 * #include commander
 * 
 * 64    98/04/28 15:30 Speter
 * Support for slide collisions. Removed ECollideType. 
 * 
 * 63    3/12/98 7:50p Agrant
 * Before you draw it, make sure the silhouette has at least 3 points.
 * 
 * 62    2/05/98 7:32p Agrant
 * World database handles its own initialization- don't duplicate that here!
 * 
 * 61    1/28/98 3:52p Agrant
 * Make sure we always have a player so the dinos can stay awake.
 * 
 * 60    1/13/98 9:33p Agrant
 * Only draw finite sized objects
 * 
 * 59    12/17/97 5:08p Agrant
 * More audio changes
 * 
 * 58    12/17/97 3:41p Agrant
 * AI Test code rot repairs
 * 
 * 57    97/12/15 18:55 Speter
 * Added collision types to CMessageCollision.
 * 
 * 56    97/10/24 15:48 Speter
 * Replaced Teleport() with Move().
 * 
 * 55    97/10/06 11:02 Speter
 * Updated for new CBoundVolBox interface.  Removed some but not all compile errors (is this
 * module still used?)
 * 
 * 54    9/15/97 7:26p Agrant
 * Better AI Test timing info.
 * 
 * 53    9/11/97 1:50p Agrant
 * Added graph links and jump links toggle on menu action
 * 
 * 52    9/09/97 8:53p Agrant
 * move object code
 * 
 * 51    7/27/97 2:14p Agrant
 * make sure the subroutines have access to the brain in debug draw.
 * 
 * 50    7/19/97 1:30p Agrant
 * Add audio daemon to AITest app.
 * Use global player.
 * 
 * 49    7/14/97 1:02a Agrant
 * Code Rot fixes.
 * Scaleable viewport.
 * 
 * 48    5/11/97 12:51p Agrant
 * Testing raptors and wolves with fullbrains instead of herdbrains.
 * 
 * 47    5/09/97 12:13p Agrant
 * Mouse player debug option
 * 
 * 46    5/04/97 9:41p Agrant
 * Minor update for CWDBQueryShapes changes
 * 
 * 45    5/01/97 3:47p Rwyatt
 * Renamed calls to ptCastInstance to ptCast. This is because it no longer casts from an
 * instance but from a partition. Renaming the function to ptCastPartition would have caused
 * more work later if another class is added under cPartition. All casts have base
 * implementations in CPartition rather than CInstance
 * 
 * 44    4/08/97 6:42p Agrant
 * Debugging improvements for AI Test app.
 * Allows test app treatment of GUIApp AI's.
 * Better tools for seeing what is going on in an AI's brain.
 * 
 * 43    3/31/97 3:18p Agrant
 * New AI debug printing scheme
 * 
 * 42    3/26/97 5:27p Agrant
 * Now running a real physics system in the AI test app.
 * 
 * 41    3/19/97 2:20p Agrant
 * Can now load GROFF files.  Now 0,0 is at center of screen.
 * 
 * 40    3/15/97 5:05p Agrant
 * Updated for spatial partition changes
 * 
 * 39    3/11/97 3:19p Blee
 * Revamped trigger system.
 * 
 * 38    97/03/04 5:11p Pkeet
 * Changed query interface to use partition objects instead of discrete presences and bounding
 * volumes.
 * 
 * 37    2/11/97 7:11p Agrant
 * Another pathfinding improvement.
 * 
 * 36    2/10/97 12:55p Agrant
 * added some consts
 * 
 * 35    2/06/97 7:17p Agrant
 * Test code for silhouetttes
 * 
 * 34    2/05/97 8:07p Agrant
 * Walls now block test graph.
 * 
 * 33    2/05/97 1:05p Agrant
 * Added a graph test mode for testing A* graph searches.
 * 
 * 32    2/04/97 2:10p Agrant
 * Updated for new query and message .hpp files
 * 
 * 31    1/30/97 2:35p Agrant
 * Added a silhouette drawign routine.
 * 
 * 30    1/23/97 2:23p Agrant
 * Added step mode to AI test app.
 * 
 * 29    1/07/97 6:50p Agrant
 * rptr's and other random structure changes integrated into test app.
 * 
 * 28    97/01/07 11:18 Speter
 * Put all CRenderTypes in rptr<>.
 * 
 * 27    12/10/96 7:41p Agrant
 * Fixing AI Test App for code rot.
 * World DBase changes, mostly.
 * 
 * 26    11/20/96 1:24p Agrant
 * CAISystem is now a CSubsystem, and processes step messages.
 * 
 * 25    11/17/96 5:24p Agrant
 * Fixed for new query class API.
 * 
 * 24    11/14/96 11:18p Agrant
 * Added club mode.
 * 
 * 23    11/14/96 4:49p Agrant
 * AI Test more in tune with the new object hierarchy
 * 
 * 22    11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 21    11/07/96 6:01p Agrant
 * now overriding the default menu with an AI specific menu.
 * 
 * 20    11/02/96 7:22p Agrant
 * Added Meat and Grass
 * Object selection works again
 * Added names to animate objects
 * 
 * 19    11/01/96 4:44p Agrant
 * Moved physics call in main loop to avoid destination line drawing errors.
 * 
 * 18    10/31/96 7:26p Agrant
 * changed over to world database as seen in the Game.  Yahoo.
 * 
 * 17    10/23/96 7:39p Agrant
 * more debugging
 * now using the dialog box for printing some text
 * first pass pathfinder
 * 
 * 16    10/22/96 4:33p Agrant
 * The dialog box is back, and ready for debugging info.
 * 
 * 15    10/15/96 9:17p Agrant
 * Fixed frame rate 
 * 
 * 14    10/14/96 12:21p Agrant
 * Added walls to test app.
 * 
 * 13    10/10/96 7:18p Agrant
 * Modified for code spec.
 * 
 * 12    10/02/96 8:10p Agrant
 * Added Sheep and Wolves.
 * Added Influence Debugging.
 * Added AI frame rate counter.
 * Unified all meter/pixel conversions.
 * 
 * 11    9/27/96 5:44p Agrant
 * Added Trees
 * Added some notion of acceleration to dino model.
 * 
 * 10    9/26/96 5:59p Agrant
 * AI system basic structure implemented
 * AI Test app has basic herding behavior based on two simple activities
 * TReal used as basic world coordinate type.
 * 
 * 9     9/23/96 2:53p Agrant
 * Added some AI system call functionality.
 * Faking inputs to dinos.
 * 
 * 8     9/19/96 1:23p Agrant
 * Use the actual game AI code for an AI cycle.
 * 
 * 7     9/03/96 7:54p Agrant
 * Added basic AI hooks.
 * Changed object list over to a list of pointers.
 * 
 * 6     8/30/96 4:51p Agrant
 * objects now have facing and velocity
 * 
 * 5     8/29/96 5:31p Agrant
 * Major revision
 * Mostly changed code to match coding spec
 * Turned the object list into a class
 * Incorporated a timer
 * Now using TDefReal instead of float
 * 
 * 4     8/28/96 4:12p Agrant
 * Basic UI modes added.
 * Paint() changed to remove silly line drawing
 * 
 * 3     8/27/96 4:37p Agrant
 *  Added object list, object creation, and object drawing
 * 
 * 2     8/21/96 3:46p Agrant
 * changed over to AI purposes
 * 
 * 1     8/20/96 5:22p Agrant
 * 
 **********************************************************************************************/

#include "GblInc/Common.hpp"
#include "Lib\W95\WinInclude.hpp"


#include "Lib\GeomDBase\PartitionPriv.hpp"
#include "AITest.hpp"


#include "Shell/WinShell.hpp"
#include "Lib/Sys/Timer.hpp"
#include "Game\AI\AIMain.hpp"

#include "Lib\Sys\Textout.hpp"
#include "Lib\EntityDBase\WorldDBase.hpp"
#include "Lib\EntityDBase\Query\QAI.hpp"
#include "Lib\EntityDBase\MessageTypes\MsgCollision.hpp"
#include "Lib\EntityDBase\GameLoop.hpp"


#include "TestBrains.hpp"
#include "AITestShell.hpp"
#include "UIModes.hpp"
#include "TestAnimal.hpp"
#include "TestTree.hpp"
#include "airesource.h"
#include "QueryTest.hpp"
#include "Lib\Loader\Loader.hpp"

#include "Game/AI/AIGraph.hpp"
#include "Game/AI/AStar.hpp"
#include "AStarTest.hpp"

#include "Game/DesignDaemon/DaemonScript.hpp"
#include "Game/DesignDaemon/Player.hpp"

#include "Lib/Audio/AudioDaemon.hpp"
#include "Lib/Audio/Audio.hpp"

#include "Lib\Sys\Profile.hpp"

CTimerLocal		tmrSimTimer;	// A timer to track simulation time.
CTimerLocal		tmrAITimer;		// A profile timer for the entire AI cycle.

CInstance*		pinsModeBasedObject = 0;   
								// Used to reference an object needed for a particular UI mode.


CSpatialGraph3	sg3TestGraph;			// Used to test the graph class.
CGraphPath		gpTestPath;						// Used to store a path.
int				iTestGraphStart = -1;	// Where do we start?
int				iTestGraphStop	= -1;	// Where do we stop?


CBoundVolBox	bvbTestBox(1, 2, 4);
CSilhouette*	psilTestBoxSilhouette;		


bool    bUseOutputFiles = false;
bool    bUseReplayFile	= false;
bool	bInvertMouse	= false;
bool	bIsTrespasser = false;

extern TReal	rScale			;		//	pixels per meter
extern int		iHeight			;		//  height of screen in pixels
extern int		iWidth			;		//  height of screen in pixels
extern int		iXOffset		;		//  screen offset from world coords (in pixels)
extern int		iYOffset		;			
extern int		iBorderWidth	;		// A border left empty for clarity.
extern CVector2<>  v2WorldMax;				// The point with the biggest possible world coords
extern CVector2<>  v2WorldMin;				// The point with the smallest possible world coords


	//*********************************************************************************************
	void Create()
	{
//		gpaiSystem = new CAISystem();

		gaiSystem.pconDebugConsole->SetActive(true);

		// Init the timer.
		tmrAITimer = CTimerLocal();
	
		// Set the view scale with 10 pixels per meter
		iXOffset = - iWidth / 2;
		iYOffset = - iHeight / 2;
		SetViewScale(7.0f);

		// Initialize the mode system
		InitMode();

		// Set the initial mode 
		SetMode(euiSELECTOBJECT);

//		MakeDialogText(pwnsWindowsShell->hinsAppInstance, pwnsWindowsShell->hwndMain);

		// Override the default menu.
		HMENU hmenu = LoadMenu
		(
			pwnsWindowsShell->hinsAppInstance,
			MAKEINTRESOURCE(AI_MAIN_MENU)
		);

		SetMenu(pwnsWindowsShell->hwndMain, hmenu);
		SetWindowText(pwnsWindowsShell->hwndMain, "AI Test App");


		// Turn off normal physics.
		//pphSystem->bActive = false;
	}
	
	//*********************************************************************************************
	void Step()
	{
		if (!IsMenuItemChecked(ID_DEBUG_STEPMODE))
			bStepRequested = TRUE;

		if (bStepRequested)
		{
			// Find the sim time since the last step.
			TMSec ms_elapsed	= tmrSimTimer.msElapsed();

			// Turn milliseconds into Seconds
			TSec sec			= sSeconds(ms_elapsed);

			// Phaux Physics
			{
				// To be implemented through the step message function.
				gmlGameLoop.Step();
			}



			// Make sure that the cursor is right.
			TReal r_x, r_y;
			GetMousePosition(r_x, r_y);
			extern CVector2<>  v2WorldMax;	// The point with the biggest possible world coords
			extern CVector2<>  v2WorldMin;	// The point with the smallest possible world coords

			// If cursor is in window, set it again.
			if (r_x < v2WorldMax.tX &&
				r_y < v2WorldMax.tY &&
				r_x > v2WorldMin.tX &&
				r_y > v2WorldMin.tY)
				SetMode(euiGetMode());


			// Draw the AI time.
//			extern CProfileStat psSynthesizer("Synthesizer", &proProfile.psAI);
//			extern CProfileStat psAnalyzer("Analyzer", &proProfile.psAI);
//			extern CProfileStat psPathfinder("Pathfinder", &psSynthesizer);



			// Test code for Silhouettes.
//			void TestSilhouetteStep();
//			TestSilhouetteStep();

			// Update dialog.
//			InvalidateRect(hwndDialogTextout, NULL, true);
			gaiSystem.pconDebugConsole->Show();
			gaiSystem.pconDebugConsole->ClearScreen();

			// Show the frame time, in the next frame.
			proProfile.psAI.WriteToConsole(*gaiSystem.pconDebugConsole);


			bStepRequested = FALSE;
		}  // end bStepRequested.

		// Mode specific details
		switch (euiGetMode())
		{
			// The user is changing a facing.
			case euiCURRENTLYFACINGOBJECT:
			{
				extern CVector2<>  v2WorldMax;	// The point with the biggest possible world coords
				extern CVector2<>  v2WorldMin;	// The point with the smallest possible world coords

				// For now, the world and the viewport are the same
				CVector2<>	v2_viewport_center = (v2WorldMax + v2WorldMin) / 2;

				// Find a direction based on mouse position
				CVector2<>  v2_new_facing;
				GetMousePosition(v2_new_facing.tX, v2_new_facing.tY);
				v2_new_facing = v2_new_facing - v2_viewport_center;

				// Normalise for front vector use
				v2_new_facing.Normalise();

				// Set facing of selected objects
//				colObjects.SetFrontSelected(v2_new_facing);
				Assert(0);
			}
			// The user is drawing a wall.
			case euiCURRENTLYADDINGWALL:
			{
				TReal r_x, r_y;

				GetMousePosition(r_x, r_y);

				// Set the second point.
				((CTestWall*)pinsModeBasedObject)->SetSecondPoint(r_x, r_y);

				MaybePerformSearch();
			}
			// The user is moving an object.
			case euiMOVING:
			{
				TReal r_x, r_y;

				GetMousePosition(r_x, r_y);

				// Set the second point.
				if (pinsModeBasedObject)
				{
					CPlacement3<> p3 = pinsModeBasedObject->pr3Presence();
					p3.v3Pos.tX = r_x;
					p3.v3Pos.tY = r_y;

					pinsModeBasedObject->Move(p3);
				}
			}
			case euiMOUSEPLAYER:
			{
				// Create a player if need be,
				if (!gpPlayer)
				{
					gpPlayer = CPlayer::pplayNew();
				}

				// Move the player object to the mouse position.
				TReal r_x, r_y;

				GetMousePosition(r_x, r_y);

				gpPlayer->Move( CPlacement3<>(CRotate3<>(), CVector3<>(r_x, r_y, 0)));
			}
			// All other modes
			default:
				// Do nothing.
				//  Kill the player if it exists.
//				if (pplay)
//				{
//					pplay->Kill();
//					pplay = 0;
//				}
				break;
		}

		// Don't count AI time against the sim clock to make debugging easier.
		tmrSimTimer.msElapsed();
 	}
	
	//*********************************************************************************************
	void AITestDraw(CInstance* pins)
	{
		rptr_const<CRenderType> pshi = pins->prdtGetRenderInfo();
		rptr_const<CTestShapeInfo> ptshi = rptr_const_dynamic_cast(CTestShapeInfo, pshi);
		if (ptshi)
		{
			ptshi->Draw(pins);
		}
		else if (pshi)
		{
			const CPhysicsInfo *pphi	= pins->pphiGetPhysicsInfo();

			if (pphi)
			{
				LineColour(128,255,255);

				// Overrrides the color to bright red if pins is currently selected.
				if (gaiSystem.bIsSelected(pins))
					LineColour(255,0,0);

				if (pphi->pbvGetBoundVol())
				{
					if (pphi->pbvGetBoundVol()->pbviCast())
						return;	

					// Draw an arbitrary object, please.
					CSilhouette sil(pphi->pbvGetBoundVol(), pins->pr3Presence());

					DrawSilhouette(&sil);
				}
			}
		}

	}

	//*********************************************************************************************
	void Paint()
	{
		if (IsMenuItemChecked(ID_RENDER_SELECTEDONLY))
		{
			if (gaiSystem.pinsSelected)
				AITestDraw(gaiSystem.pinsSelected);
		}
		else
		{
			CVector3<> v3_extent = v2WorldMax - v2WorldMin;
			v3_extent.tZ = 10000.0f;

			CBoundVolBox bvb(v3_extent);
			CBoundVol* pbv = &bvb;
			CPartitionSpace ps(*pbv);
			ps.SetPlacement(CPlacement3<>((v2WorldMax + v2WorldMin) * 0.5f));

			//  draw each object in the global list
			CWDbQueryRenderTypes cqs(&ps);
			//CWDbQueryShapes cqs(&ps);

			foreach(cqs)
			{
				CInstance *pins;

				cqs.tGet().ppart->Cast(&pins);

				AITestDraw(pins);

			}

			// Then draw debug for each object.
			// This makes sure that nodes are drawn afterwards.
			foreach(cqs)
			{
				CInstance *pins;

				cqs.tGet().ppart->Cast(&pins);

				const CAnimal* pani = ptCast<CAnimal>(pins);

				if (pani)
				{
					gaiSystem.SetCurrentBrain(pani->pbrBrain);
					pani->pbrBrain->DrawDebug();
					gaiSystem.ClearCurrentBrain();
				}
			}

		}




		
		// Draw the test graph!
		{
			sg3TestGraph.Draw(false);

			// Highlight the start and stop nodes of the graph.
			LineColour(0,255,0);
			sg3TestGraph.HighlightNode(iTestGraphStart);

			// Highlight the start and stop nodes of the graph.
			LineColour(255,0,0);
			sg3TestGraph.HighlightNode(iTestGraphStop);

			// Draw the path!
			sg3TestGraph.DrawPath(gpTestPath);

			// Draw the test silhouette if appropriate.
			if (psilTestBoxSilhouette)
				DrawSilhouette(psilTestBoxSilhouette);
		}
	}


	//*********************************************************************************************
	void Destroy()
	{
		wWorld.Purge();
	}


	//*********************************************************************************************
	void LeftButtonClicked(TReal r_x, TReal r_y)
	{
		// Left click actions depend on the current UI mode
		switch (euiGetMode())
		{

			// Add dinosaur.
			case euiADDDINOSAUR:
			{
				// Create a dinosaur.
				CTestDinosaur*	ptd = new CTestDinosaur(r_x, r_y);

				// Give it a brain.
				new CFullBrain(ptd, earCARNIVORE, "Dino");

				// Add the object to the world.
				AddObject(ptd);
				break;
			}
			// Add Wolf.
			case euiADDWOLF:
			{
				CTestWolf *ptw = new CTestWolf(r_x, r_y);

				// Give it a brain.
				new CFullBrain(ptw,earCARNIVORE, "Wolf");

				// Add the object to the world.
				AddObject(ptw);
			}
			break;
			// Add Sheep.
			case euiADDSHEEP:
			{
				CTestSheep *pts = new CTestSheep(r_x, r_y);

				// Give it a brain.
				new CHerdBrain(pts,earCARNIVORE, "Sheep");

				// Add the object to the world.
				AddObject(pts);
			}
			break;
			// Place first point for a new wall.
			case euiADDWALL:
			{
				// Create the wall and save a reference to it.
				pinsModeBasedObject = new CTestWall(r_x, r_y);

				// Add the object to the world.
				AddObject(pinsModeBasedObject);

				// Wait for button release to finish drawing wall.
				SetMode(euiCURRENTLYADDINGWALL);
			}
			break;
			// Add Tree.
			case euiADDTREE:
			{
				CTestTree *ptt = new CTestTree(r_x, r_y);

				// Add the object to the world.
				AddObject(ptt);
			}
			break;
			// Add Hint.
			case euiADDHINT:
			{
				CInstance* pins = 
					new CInstance(CInstance::SInit( CVector3<>(r_x, r_y,0),
													rptr0,
													0,
													new CAIInfo(eaiNODEHINT),
													"Hint"));

				// Add the object to the world.
				AddObject(pins);
			}
			break;
			// Add Meat.
			case euiADDMEAT:
			{
//				AddObject(new CTestInanimate<eaiMEAT, CShapeMeat>(r_x, r_y));
			}
			break;
			// Add Grass.
			case euiADDGRASS:
			{
//				AddObject(new CTestInanimate<eaiGRASS, CShapeGrass>(r_x, r_y));
			}
			break;
			// Select object.
			case euiSELECTOBJECT:
			{
				CInstance* pins = pinsGetNearest(CVector2<>(r_x,r_y));

				// Toggle select bit
				if (pins)
				{
					if (gaiSystem.bIsSelected(pins))
						gaiSystem.pinsSelected = 0;
					else
					{
						gaiSystem.pinsSelected = pins;

						if (pins->paniGetOwner())
						{
							gaiSystem.pinsSelected = (CInstance*)pins->paniGetOwner();
						}
					}
				}
			}
			break;
			// Remove Object.
			case euiREMOVEOBJECT:
			{
				// Find closest object
				CInstance* pins = pinsGetNearest(CVector2<>(r_x,r_y));

				if (pins)
					pins->Kill();
			}
			
			// Waiting for user to start the facing action
			case euiBEGINFACINGOBJECT:
				SetMode(euiCURRENTLYFACINGOBJECT);
				break;

			// Change facing of selected objects by dragging mouse
			case euiCURRENTLYFACINGOBJECT:
				// This shouldn't ever happen, since you enter this mode with 
				// a left click and exit with a release or a right click
				Assert(0);
				break;
			case euiCLUB:
			{
				/*
				CInstance* pins = pinsGetNearest(CVector2<>(r_x,r_y));
				if (pins)
				{
					CMessageCollision msgcoll(pins, pins, 10.0, 10.0, CVector3<>(0,0,0));
					msgcoll.Dispatch();
				}
				*/
			}
				break;
			// The user is ading nodes to the test graph.
			case euiADDTESTNODE:
			{
				TReal r_x, r_y;
				GetMousePosition(r_x, r_y);

				sg3TestGraph.AddNode(CVecNode3<>(r_x, r_y, 0));
				MaybePerformSearch();
			}
				break;
			// The user is selecting the start node of the test graph.
			case euiSETSTARTNODE:
			{
				TReal r_x, r_y;
				GetMousePosition(r_x, r_y);

				iTestGraphStart = sg3TestGraph.iNearestNode(CVector3<>(r_x, r_y, 0));

				MaybePerformSearch();
			}
				break;
			// The user is selecting the stop node of the test graph.
			case euiSETSTOPNODE:
			{
				TReal r_x, r_y;
				GetMousePosition(r_x, r_y);

				iTestGraphStop = sg3TestGraph.iNearestNode(CVector3<>(r_x, r_y, 0));
				MaybePerformSearch();
			}
				break;
			case euiSTARTMOVE:
			{	
				pinsModeBasedObject = pinsGetNearest(CVector2<>(r_x,r_y));
				if (pinsModeBasedObject)
					SetMode(euiMOVING);
			}
				break;

			case euiMOVING:
			case euiCURRENTLYADDINGWALL:
			case euiMOUSEPLAYER:
				break;

			// Unknown mode
			default:
				Assert(0);
				break;
		}
	}
	
	//*********************************************************************************************
	void LeftButtonReleased(TReal r_x, TReal r_y)
	{
		switch (euiGetMode())
		{
			case euiCURRENTLYFACINGOBJECT:
			{
				// User has just finished facing the selected objects.
				SetMode(euiBEGINFACINGOBJECT);
				Assert(0);
	//			colObjects.SetVelocityToFrontAll();
			}
				break;
			case euiCURRENTLYADDINGWALL:
			{
				// User has placed first wall point, and is now releasing the button.
				SetMode(euiADDWALL);

				// Set the second point.
				((CTestWall*)pinsModeBasedObject)->SetSecondPoint(r_x, r_y);
				pinsModeBasedObject = 0;
			}
				break;

			case euiMOVING:
			{
				SetMode(euiSTARTMOVE);
				pinsModeBasedObject = 0;
			}
				break;
			default:
				break;
		}
	}

	//*********************************************************************************************
	void RightButtonClicked(TReal r_x, TReal r_y)
	{

		// Right click actions depend on the curremt UI mode
		switch (euiGetMode())
		{
		
			// The user is selecting the start node of the test graph.
			case euiSETSTARTNODE:
			{
				TReal r_x, r_y;
				GetMousePosition(r_x, r_y);

				// The right button selects the other end of the path.
				iTestGraphStop = sg3TestGraph.iNearestNode(CVector3<>(r_x, r_y, 0));
				MaybePerformSearch();
			}
				break;
			// The user is selecting the stop node of the test graph.
			case euiSETSTOPNODE:
			{
				TReal r_x, r_y;
				GetMousePosition(r_x, r_y);

				// The right button selects the other end of the path.
				iTestGraphStart = sg3TestGraph.iNearestNode(CVector3<>(r_x, r_y, 0));
				MaybePerformSearch();
			}
			break;
			default:
			{
				// Resets mode.
				SetMode(euiSELECTOBJECT);

				// Unselect all.
				gaiSystem.pinsSelected = 0;

//				CWDbQueryTest cqt;

//				foreach(cqt)
//				{
//					cqt.tGet()->bSelected = false;
//				}	
			}
		}

	}

	//*********************************************************************************************
	void Command(int i_id)
	{
		// find out which command has been activated
		switch (i_id)
		{
			// Add dinosaur.
			case ID_ADD_DINO:
				SetMode(euiADDDINOSAUR);
				break;
			// Add Tree.
			case ID_ADD_TREE:
				SetMode(euiADDTREE);
				break;
			// Add Wolf.
			case ID_ADD_WOLF:
				SetMode(euiADDWOLF);
				break;
			// Add Sheep.
			case ID_ADD_SHEEP:
				SetMode(euiADDSHEEP);
				break;
			case ID_ADD_WALL:
				SetMode(euiADDWALL);
				break;
			case ID_ADD_MEAT:
				SetMode(euiADDMEAT);
				break;
			case ID_ADD_GRASS:
				SetMode(euiADDGRASS);
				break;
			case ID_ACTION_CLUB:
				SetMode(euiCLUB);
				break;
			case ID_ACTION_GOTO:
				SetMode(euiGOTO);
				break;
			case ID_ACTION_MOVE:
				SetMode(euiSTARTMOVE);
				break;
			// Remove object.
			case ID_OBJECT_REMOVE:
				// Set the mode.
				SetMode(euiREMOVEOBJECT);

				// Remove all currently selected objects.
//				colObjects.RemoveSelected();
				Assert(0);
				break;
			// Select object.
			case ID_OBJECT_SELECT:
				SetMode(euiSELECTOBJECT);
				break;
			// Change facing of selected objects
			case ID_OBJECT_FACING:
				SetMode(euiBEGINFACINGOBJECT);
				break;			
			// Add node
			case ID_GRAPH_PLACETESTNODE:
				SetMode(euiADDTESTNODE);
				break;			
			// Remove node
			case ID_GRAPH_DELETENODE:
				SetMode(euiREMOVETESTNODE);
				break;			
			// Select start node
			case ID_GRAPH_SELECTSTARTNODE:
				SetMode(euiSETSTARTNODE);
				break;			
			// Add node
			case ID_GRAPH_SELECTSTOPNODE:
				SetMode(euiSETSTOPNODE);
				break;			
			// Toggle debug flags.
			case ID_DEBUG_INFLUENCES:
			case ID_DEBUG_DESTINATIONS:
			case ID_DEBUG_STRINGS:
			case ID_DEBUG_INVULNERABOBLE:
			case ID_DEBUG_STEPMODE:
			case ID_DEBUG_GRAPH:
			case ID_RENDER_SELECTEDONLY:
				ToggleMenuCheck(i_id);
				break;
			case ID_GRAPH_SHOWALLLINKS:
				gaiSystem.bDrawAllGraphLinks = !gaiSystem.bDrawAllGraphLinks;
				SetMenuCheck(i_id, gaiSystem.bDrawAllGraphLinks);
				break;
			case ID_GRAPH_JUMPLINKS:
				gaiSystem.bDrawJumpGraphLinks = !gaiSystem.bDrawJumpGraphLinks;
				SetMenuCheck(i_id, gaiSystem.bDrawAllGraphLinks);
				break;
			case ID_DEBUG_MOUSEPLAYER:
				SetMode(euiMOUSEPLAYER);
				break;
			case ID_ACTION_STEP:
				bStepRequested = TRUE;
				break;
			case ID_FILE_LOADGROFF:
				{
					char str_file_name[512];		// Storage for the file name.


					BOOL bGetFilename
					(
						HWND  hwnd,			// Window handle of parent.
						char* str_filename,	// Pointer to string to put filename.
						int   i_max_chars	// Maximum number of characters for the filename.
					);

					// Get a filename from the file dialog box.
					if (!bGetFilename(pwnsWindowsShell->hwndMain, str_file_name, sizeof(str_file_name)))
					{
						// If no file is selected, then return.
						return;
					}

					// Create the load world object.
					CLoadWorld lw(str_file_name);
				}
				break;
			case ID_FILE_SCENE:
				{
					char str_file_name[512];		// Storage for the file name.


					BOOL bGetFilename
					(
						HWND  hwnd,			// Window handle of parent.
						char* str_filename,	// Pointer to string to put filename.
						int   i_max_chars	// Maximum number of characters for the filename.
					);

					// Get a filename from the file dialog box.
					if (!bGetFilename(pwnsWindowsShell->hwndMain, str_file_name, sizeof(str_file_name)))
					{
						// If no file is selected, then return.
						return;
					}

					// Create the load world object.
					wWorld.bLoadScene(str_file_name);
					break;
				}

			default:
				// All unhandled commands ought to show up here.
				//Assert(0);
				break;
		}
	}
		


	//*********************************************************************************************
/*	void AITestCycle()
	{
		Assert(0);


		// Stop clock so that debugging sessions do not give stuff time to move.
		tmrSimTimer.Pause(1);	

		// Call the AI system.
//		gaiSystem.Cycle();

		// Start clcok.
		tmrSimTimer.Pause(0);	
	}
*/	

	//*********************************************************************************************
	void AddObject(CInstance* pins)
	{

		
		/*
		CAnimal*	pan = ptCast<CAnimal>(pins);

		// Is the new object an animal?
		if (pan)
		{
			// Iterator for the world object list.
			CWDbQueryShape cqs(bvsBigSphere);
		
			foreach(cqs)
			{
				pan->pbrBrain->msState.Notice(cqs.tGet());
			}
		}


		// Let each dino in the world know about the new object.
		CWDbQueryAnimal cqa(bvsBigSphere);

		foreach(cqa)
		{
			cqa.tGet()->pbrBrain->msState.Notice(pins);
		}
		*/
		
		// Add it to the object list.
		wWorld.Add(pins);
	}

	//*********************************************************************************************
	void KeyPressed
	(
		int virtkey // virtual key code of pressed key
	)
	{
		// The world scale factor for the viewport.
		extern TReal rScale;

		switch (virtkey)
		{
			case VK_SPACE:
				// spacebar steps siulation in step mode
				bStepRequested = TRUE;
				break;
			case VK_DOWN:
				iYOffset -= rScale * 10;
				SetViewScale(rScale); 
				break;
			case VK_UP:
				iYOffset += rScale * 10;
				SetViewScale(rScale);
				break;
			case VK_LEFT:
				iXOffset -= rScale * 10;
				SetViewScale(rScale);
				break;
			case VK_RIGHT:
				iXOffset += rScale * 10;
				SetViewScale(rScale);
				break;
			case VK_NEXT:
				{
					CVector2<> v2_center = v2WorldMin + v2WorldMax;
					v2_center *= 0.5;
					SetViewScale(rScale * 1.1);
					SetViewCenter(v2_center.tX, v2_center.tY);
				}
				break;
			case VK_PRIOR:
				{
					CVector2<> v2_center = v2WorldMin + v2WorldMax;
					v2_center *= 0.5;
					SetViewScale(rScale * (1.0 / 1.1));
					SetViewCenter(v2_center.tX, v2_center.tY);
				}
				break;
			case VK_HOME:
				// Teleport to a dinosaur.
				{
					static int i_last_dino = iMAX_ACTIVE_ANIMALS;

					bool b_found_one = false;
					CVector3<> v3_animal;


					i_last_dino--;

					if (i_last_dino < 0)
						i_last_dino = iMAX_ACTIVE_ANIMALS - 1;

					for ( ; i_last_dino >= 0; --i_last_dino)
					{
						if (gaiSystem.apaniActiveAnimals[i_last_dino])
						{	
							CAnimal *pani = gaiSystem.apaniActiveAnimals[i_last_dino];
							v3_animal = pani->v3Pos();

							b_found_one = true;
						}
					}

					if (!b_found_one)
					{
						// If we get here, we failed to find any active dinos.
						// Target an inactive one.
						if (gaiSystem.lpaniInactiveAnimals.begin() != gaiSystem.lpaniInactiveAnimals.end())
						{
							CAnimal *pani = (CAnimal*)*gaiSystem.lpaniInactiveAnimals.begin();
 							v3_animal = pani->v3Pos();
							b_found_one = true;
						}
					}

					if (b_found_one)
					{
						SetViewCenter(v3_animal.tX, v3_animal.tY);
					}
					break;
				}
			default:
				break;
		}
	}

	//*********************************************************************************************
	void DrawSilhouette
	(
		const CSilhouette* psil
	)
	{
		Assert(psil->uLen > 2);

		for (int i = psil->size() - 2; i >= 0; i--)
		{
			Line((*psil)[i+1], (*psil)[i]);
		}
			
		Line((*psil)[psil->size()-1], (*psil)[0]);
	}

	//*********************************************************************************************
	void MaybePerformSearch()
	{
		// if we have good start and stop nodes, perform a search!
		if (sg3TestGraph.bIsValidIndex(iTestGraphStart) &&
			sg3TestGraph.bIsValidIndex(iTestGraphStop))
		{
//			CAStar< CVecNode3<>, TReal > astar(&sg3TestGraph, iTestGraphStart, iTestGraphStop);
			CAStarTest astar(&sg3TestGraph, iTestGraphStart, iTestGraphStop);			

			astar.Search();

			gpTestPath.Clear();
			astar.GetPath(&gpTestPath);
		}
	}


	
void TestSilhouetteStep()
{

// The idea is to make a box and rotate it a lot and make sure that it looks good.

	static int i_stage = 0;
	static int i_substage = 0;
	static int i_subsubstage = 0;

	if (psilTestBoxSilhouette)
		delete psilTestBoxSilhouette;
	psilTestBoxSilhouette = 0;

	// 0 to 2pi
#define RAND2PI fRandom(-2*dPI,2*dPI)

	// -pi to pi
#define RANDPI fRandom(-dPI, dPI)


#define ROTVEC(x,y,z) CRotate3<>(CVector3<>(x,y,z))

#define TRANS(rot) CPresence3<> pr3(rot,5,CVector3<>(30,30,0))

#define TEST()  psilTestBoxSilhouette = new CSilhouette(&bvbTestBox, pr3); 
		//		Assert(psilTestBoxSilhouette->bIsValid());


	if (i_stage == 0)
	{
		// Go through the vertical axis cases.
		switch (i_substage)
		{
			case 0:
				{
				// Perform a test around Z
				TRANS(ROTVEC(0,0, RAND2PI));
				TEST();
				}
				break;
			case 1:
				// Perform a test around Y
				{
				TRANS(ROTVEC(dPI / 2,0, 0) * ROTVEC(0,0,RAND2PI));
				TEST();
				}
				break;
			case 2:
				// Perform a test around X
				{
				TRANS(ROTVEC(0,-dPI / 2, 0) * ROTVEC(0,0,RAND2PI));
				TEST();
				}
				break;
			case 3:
				// Perform a test around -Z
				{
				TRANS(ROTVEC(0,dPI, 0) * ROTVEC(0,0,RAND2PI));
				TEST();
				}
				break;
			case 4:
				// Perform a test around -X
				{
				TRANS(ROTVEC(0,dPI / 2, 0) * ROTVEC(0,0,RAND2PI));
				TEST();
				}
				break;
			case 5:
				// Perform a test around -Y
				{
				TRANS(ROTVEC(-dPI / 2,0, 0) * ROTVEC(0,0,RAND2PI));
				TEST();
				}
				break;

		}

		i_subsubstage++;
		if (i_subsubstage > 20)
		{
			i_substage++;
			i_subsubstage = 0;
		}
		if (i_substage > 5)
		{
			i_stage++;
			i_substage = 0;
		}
	}
	else if (i_stage == 1)
	{
		// Go through the 1 zero sign axis cases.
		switch (i_substage)
		{
			case 0:
				{
				// Perform a test where X is zero
				TRANS(ROTVEC(RAND2PI, 0, 0) * ROTVEC(0,0, RAND2PI));
				TEST();
				}
				break;
			case 1:
				// Perform a test where Y is zero
				{
				TRANS(ROTVEC(0,RAND2PI,0) * ROTVEC(0,0,RAND2PI));
				TEST();
				}
				break;
			case 2:
				// Perform a test where Z is zero
				{
				TRANS(ROTVEC(0,dPI / 2, 0) * ROTVEC(RAND2PI,0,0) * ROTVEC(0,0,RAND2PI));
				TEST();
				}
				break;

		}

		i_subsubstage++;
		if (i_subsubstage > 40)
		{
			i_substage++;
			i_subsubstage = 0;
		}
		if (i_substage > 2)
		{
			i_stage++;
			i_substage = 0;
		}
	}
	else
	{
		// Go through the all non-zero sign axis cases.
		TRANS(ROTVEC(RANDPI, RANDPI, RANDPI));
		TEST();
	}
}



//**********************************************************************************************
BOOL bGetFilename(HWND hwnd, char* str_filename, int i_max_chars)
{
	OPENFILENAME ofn;

	// Null the string.
	*str_filename = 0;
			
	// Fill structure.
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hwnd;
	ofn.lpstrFilter       = "GROFF (*.grf)\0*.grf\0SCENE (*.scn)\0*.scn\0All (*.*)\0*.*\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = str_filename;
	ofn.nMaxFile          = i_max_chars;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = "Load 3D File";
	ofn.lpstrFileTitle    = NULL;
	ofn.lpstrDefExt       = NULL;
	ofn.Flags             = OFN_HIDEREADONLY;

	// Get the file name.
	if (GetOpenFileName((LPOPENFILENAME)&ofn))
	{
		return TRUE;
	}
	return FALSE;
}



struct goof {

	void foobar(){};
};

void whoopee2()
{
	goof g;

	g.foobar();
}

