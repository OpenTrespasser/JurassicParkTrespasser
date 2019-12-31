/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of TestPipeLine.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/TestPipeLine.cpp                                                 $
 * 
 * 82    97-04-03 19:16 Speter
 * Updated for new CEntityLight.
 * 
 * 81    97-03-28 16:37 Speter
 * Renamed preMain to prnshMain, psrMain to prenMain.
 * 
 * 
 * 80    97/03/24 15:12 Speter
 * Changed camera CInstance to CCamera.
 * 
 * 79    97/02/05 19:44 Speter
 * Fixed for new Message.hpp headers.
 * 
 * 78    97/01/27 17:52 Speter
 * Updated for new CScreenRender structure.
 * 
 * 77    97/01/16 11:57 Speter
 * Replaced defunct psrMain->prasvScreen with prasMainScreen.
 * 
 * 76    97/01/07 13:12 Speter
 * Made all CRenderTypes use rptr<>.
 * 
 * 75    12/30/96 6:33p Mlange
 * Updated for changes to the base library.
 * 
 * 74    96/12/09 16:09 Speter
 * Removed CTimer calls.
 * 
 * 73    96/12/04 20:28 Speter
 * A big change: Changed all CInstance placement accessors to pr3Presence().  Changed v3T to
 * v3Pos, r3T to r3Rot.
 * 
 * 72    11/13/96 5:02p Pkeet
 * Replaced CEntityCamera and its include with CCamera and its include.
 * 
 * 71    10/31/96 5:55p Pkeet
 * Changed 'prscWorld' to 'wWorld' and references to 'TPrsContainer' to 'CWorld.'
 * 
 * 70    10/31/96 4:43p Pkeet
 * Replaced references to the pShapeInfo member variable of CEntityShape to use the
 * 'pshiGetShapeInfo()' function for access.
 * 
 * 69    10/28/96 7:32p Pkeet
 * Removed the member variable camera and the global viewport.
 * 
 * 68    96/10/28 15:01 Speter
 * Changed CEventHandler to CAppShell.
 * 
 * 67    96/10/25 14:31 Speter
 * Updated for changes in CShape and CMesh.
 * 
 * 66    10/24/96 6:36p Pkeet
 * Removed the base world object.
 * 
 * 65    96/10/23 15:15 Speter
 * Removed timing stats.
 * Changed references of prasMainScreen to psrMain->prasvScreen.
 * 
 * 64    96/10/22 11:27 Speter
 * Changed CPresenceCamera to CCamera, prscam prefix to cam.
 * 
 * 63    96/10/22 11:11 Speter
 * Changed CEntity to CPresence in appropriate contexts.
 * 
 * 62    96/10/18 18:30 Speter
 * Updated for changes in CViewport.
 * 
 * 61    96/10/14 15:40 Speter
 * Removed screen format info from stat output (it's now in ConShowStats()).
 * 
 * 
 * 60    96/10/04 18:10 Speter
 * Removed defunct ObjDef3D.hpp file, added new Mesh.hpp.
 * Removed now-unnecessary call to LoadTexture.
 * Moved stat printout code to WinRenderTools.cpp.
 * 
 * 59    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 58    9/20/96 9:38a Cwalla
 * added clut viewer..
 * 
 * 57    96/09/11 13:48 Speter
 * Added state descriptions to console output.
 * 
 * 56    96/09/05 17:12 Speter
 * Fixed conflicts.
 * 
 * 55    96/09/05 11:57 Speter
 * Changed global seterfRendering to CScreenRender::seterfState.
 * 
 * 54    96/08/23 9:47 Speter
 * Changed erfCLIP_PERSPECTIVE flag to erfPERSPECTIVE.
 * 
 * 53    96/08/22 18:27 Speter
 * Changed, expanded render flag definitions.
 * 
 * 52    8/20/96 4:44p Pkeet
 * Added fogging.
 * 
 * 51    96/08/15 18:38 Speter
 * Fixed TestDrawLine bug.
 * Removed calls to Lock() and Unlock().
 * 
 * 50    96/08/14 14:04 Speter
 * Removed calls to Lock, Unlock, ResetZBuffer (handled by ScreenRender class).
 * Added timing stats.
 * 
 * 49    96/08/07 20:50 Speter
 * Updated for gamma parameters now in CLight rather than CMaterial.
 * 
 * 48    96/08/06 18:26 Speter
 * Changes for hierarchical Entities.
 * 
 * 47    96/08/05 12:28 Speter
 * Now print out description of all DDSurface flags.
 * 
 * 46    96/08/02 11:57 Speter
 * Added 'H' key to show menu in console; off by default.
 * 'G' key now toggles gamma correction.
 * 
 * 45    96/07/31 15:53 Speter
 * Added timing stats.
 * Moved ResetZBuffer() here from Render.cpp.
 * 
 * 44    96/07/29 14:07 Speter
 * 
 * 43    96/07/29 11:37 Speter
 * 
 * 42    7/26/96 6:37p Mlange
 * Updated for mesh scale member change.
 * 
 * 41    7/25/96 4:33p Mlange
 * Updated for CObjPoint name change. Updated for deleted types such as CPlacement etc.
 * 
 * 40    96/07/24 15:00 Speter
 * Added gamma adjustment key.
 * 
 * 39    96/07/23 17:01 Speter
 * Changed instructions too.
 * 
 * 38    96/07/23 17:00 Speter
 * Changed '+' and '-' to VK_ADD and VK_SUBTRACT, so it now works.
 * 
 * 37    96/07/23 16:10 Speter
 * Added new erfCLIP_PERSPECTIVE render flag, to control whether clipper interpolates in 3D or
 * 2D.
 * 
 * 36    96/07/23 10:55 Speter
 * Added casts to some array index operations to resolve ambiguities.
 * Added printout of quaternion denormalisation.
 * 
 * 35    96/07/22 17:07 Speter
 * Fixed signed/unsigned mismatch.
 * 
 * 34    96/07/22 15:52 Speter
 * Changes for new render object format:
 * Made several changes to test UI:
 * Now takes arbitrary number of objects and lights.
 * All objects and a light can be moved, rotated, etc.
 * Keyboard interface is simpler, and uses arrow keys for movement.
 * 
 * 33    7/19/96 10:57a Pkeet
 * Changed include files for shell files moved to 'Shell.'
 * 
 * 32    7/18/96 6:59p Mlange
 * Updated for CVertex to CPoint name change. 
 * 
 * 31    7/18/96 5:46p Pkeet
 * Got rid of the arbitrary Viewport modifications during the pipeline test initialization.
 * 
 * 30    7/18/96 5:28p Mlange
 * Updated for changes to bounding box.
 * 
 * 29    7/18/96 3:45p Pkeet
 * Removed references to 'WinShell.'
 * 
 * 28    7/18/96 3:31p Mlange
 * Added test code for bounding box.
 * 
 * 27    7/17/96 6:12p Mlange
 * Updated the (commented-out) test code.
 * 
 * 26    7/16/96 1:27p Mlange
 * Fixed memory leak.
 * 
 * 25    7/11/96 6:01p Mlange
 * Added pyramid.
 * 
 * 24    7/10/96 12:43p Mlange
 * Added (now commented out) test code for the 3d clipping.
 * 
 * 23    7/08/96 8:49p Mlange
 * Test code for clipping module.
 * 
 * 22    96/07/03 13:02 Speter
 * Moved several files to new directories, and changed corresponding include statements.
 * 
 * 21    96/06/28 15:20 Speter
 * Changed Entity transform type from Transform to Placement.  Adjusted associated pipeline
 * operations.
 * 
 * 20    6/28/96 3:16p Mlange
 * Renamed global shape instances. Now draws the viewport's border and clears the screen before
 * rendering the scene.
 * 
 * 19    96/06/26 15:10 Speter
 * Changed to conform to new transform modules.
 * 
 * 18    6/26/96 12:31p Mlange
 * Updated for changes to pipeline interface and object definitions.
 * 
 * 17    96/06/18 8:12p Mlange
 * Updated for changes to camera class' data members.
 * 
 * 16    96/06/18 10:08a Mlange
 * Added UI for camera move and rotate. Now interfaces with RenderScene() instead of hard coding
 * the rendering pipeline in this module.
 * 
 * 15    6/06/96 1:40p Pkeet
 * Made pyramid and cube solid.
 * 
 * 14    6-06-96 11:41a Mlange
 * Updated for change in coordinate system.
 * 
 * 13    5-06-96 1:11p Mlange
 * Added cube.
 * 
 * 12    5-06-96 12:01p Mlange
 * Updated for triangle based rendering.
 * 
 * 11    4-06-96 4:24p Mlange
 * Added code (now commented out) to calculate the plane equation of the polygon directly.
 * 
 * 10    3-06-96 5:59p Mlange
 * Updated for changes to angle class.
 * 
 * 9     3-06-96 5:18p Mlange
 * 
 * 8     31-05-96 10:11p Mlange
 * Added a second static rotating object to test Z-buffering.
 * 
 * 7     96/05/31 12:48 Speter
 * Moved Clear from Winshell to Paint handlers.
 * 
 * 6     31-05-96 11:17a Mlange
 * Changed the initial position and scale of the object.
 * 
 * 5     96/05/31 10:51 Speter
 * Now uses CColour values, and converts them to pixels, rather than specifically using pixel
 * values.  Now works in all video modes and palettes.
 * Moved rotation code from Paint function to Step function.
 * 
 * 4     29-05-96 5:45p Mlange
 * Temporary version! Integrated with the polygon rendering primitives.
 * 
 * 3     29-05-96 4:58p Mlange
 * Added backface culling.
 * 
 * 2     29-05-96 11:23a Mlange
 * Added object rotation.
 * 
 * 1     28-05-96 8:30p Mlange
 * Testbed for the rendering pipeline.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "TestPipeLine.hpp"

#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/Sys/TextOut.hpp"
#include "Lib/Renderer/Fog.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Shell/WinRenderTools.hpp"
#include "Shell/AppShell.hpp"
#include "Test/Test3DObjs.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"
#include "Lib/EntityDBase/EntityLight.hpp"


#define angFIELD_OF_VIEW_INCR	(CAngle)dDegreesToRadians(2)
#define fZOOM_INCR				.1f
#define rCLIP_PLANE_INCR		.01
#define dDISP_WIN_INCR			2.0
#define dCOP_INCR				0.01
#define rENTITY_MOVE_INCR		.005
#define angENTITY_ROT_INCR_INCR	(CAngle)dDegreesToRadians(1)
#define rOBJ_SCALE_INCR			1.1

enum EAction
{
	eNO_ACTION,

	eMODIFY_CAMERA,
	eMOVE_CAMERA,
	eROTATE_CAMERA,
	eADJUST_FIELD_OF_VIEW,
	eADJUST_ZOOM,
	eMOVE_FAR_CLIP_PLANE,
	eMOVE_NEAR_CLIP_PLANE,

	eTOGGLE_ZBUFFER,
	eTOGGLE_SHADING,
	eTOGGLE_TEXTURING,

	eMODIFY_VIEWPORT,
	eMOVE_VIEWPORT,
	eRESIZE_VIEWPORT,
	eMOVE_VIEWPORT_CENTRE,

	eMODIFY_OBJECT,
	eMODIFY_LIGHT,
};



const CColour clrBLUE	(0.0f, 0.0f, 1.0f);
const CColour clrRED	(1.0f, 0.0f, 0.0f);
const CColour clrGREEN	(0.0f, 1.0f, 0.0f);
const CColour clrYELLOW	(1.0f, 1.0f, 0.0f);
const CColour clrBROWN	(0.5f, 0.5f, 0.0f);
const CColour clrWHITE	(1.0f, 1.0f, 1.0f);

// Flag to ensure initialization is called only once.
bool bIsInitialized = false;

class CAppShellTestPipeLine: public CAppShell
{
	EAction eacCurr;
	bool bShowMenu;
	uint uStatLine;

	CEntityTest* pettCurr;

	SMakeObjects* pmkobjObjs;

public:

	CAppShellTestPipeLine()
	{
		bShowMenu = false;
	}

	//******************************************************************************************
	void Init()
	{
		// Set flag to initialize only once.
		if (bIsInitialized)
			return;
		bIsInitialized = true;

		SetContinuous(1);

		pmkobjObjs = new SMakeObjects();

		//
		// Push the objects onto the world database.
		//
		wWorld.Add(pmkobjObjs->pettCube);
		wWorld.Add(pmkobjObjs->pettPyramid);
		wWorld.Add(pmkobjObjs->pettCone);



		//
		// Push the lights onto the world database.
		//
		rptr<CLightAmbient>     plta_light = rptr_new CLightAmbient(0.2);
		rptr<CLightDirectional> pltd_light = rptr_new CLightDirectional(0.8);

		wWorld.Add(new CEntityTest(rptr_cast(CRenderType, plta_light)));

		wWorld.Add(pmkobjObjs->pettLight);

		// Create an entity to contain it, with the camera as its controlling parent.
		wWorld.Add(new CEntityLight(rptr_cast(CLight, pltd_light), pmkobjObjs->pettLight);

		//
		// Push the camera onto the world database.
		//
//		wWorld.Add(new CEntityTest(new CCamera()));

		// Print the first menu.
		eacCurr = eNO_ACTION;
		KeyPress(0);
	}


	//******************************************************************************************
	void Step()
	{
		// Print general render stats.
		conStd.SetCursorPosition(0, uStatLine);

		conStd.Print("ZBuf %d", 
			!!prenMain->pSettings->seterfState[erfZ_BUFFER]);
		conStd.Print(", Shade %d", 
			!!prenMain->pSettings->seterfState[erfLIGHT_SHADE]);
		conStd.Print(", Texture %d", 
			!!prenMain->pSettings->seterfState[erfTEXTURE]);
		conStd.Print(", Perspect %d", 
			!!prenMain->pSettings->seterfState[erfPERSPECTIVE]);
//		conStd.Print(", Gamma %d:%f",
//			CLight::bGammaCorrect, CLight::fInvMonitorGamma);
		conStd.Print("\n");

//		ConShowStats();

		gmlGameLoop.Step();

		conStd.Show();

		Repaint();
	}


	//******************************************************************************************
	void Paint()
	{
		prasMainScreen->Clear(0);

		//
		// Draw the border of the viewport.
		//
		TPixel pix = prasMainScreen->pixFromColour(clrWHITE);

		// Get camera properties.
		CCamera* pcam = CWDbQueryActiveCamera().tGet();
		const CCamera::SProperties& camprop = pcam->campropGetProperties();

		double d_tl_x, d_tl_y;		// Top left coordinates.
		double d_br_x, d_br_y;		// Bottom right coordinates.

		d_tl_x = camprop.vpViewport.scPositionX;
		d_tl_y = camprop.vpViewport.scPositionY;

		d_br_x = camprop.vpViewport.scPositionX + camprop.vpViewport.scWidth;
		d_br_y = camprop.vpViewport.scPositionY + camprop.vpViewport.scHeight;

		d_br_y -= 1;
		d_br_x -= 1;

		// Top, bottom, left, right.
		TestDrawLine(d_tl_x, d_tl_y, d_br_x, d_tl_y, pix);
		TestDrawLine(d_tl_x, d_br_y, d_br_x, d_br_y, pix);
		TestDrawLine(d_tl_x, d_tl_y, d_tl_x, d_br_y, pix);
		TestDrawLine(d_br_x, d_tl_y, d_br_x, d_br_y, pix);


		// Render the scene.
		wWorld.Render(*psrMain);

		//
		// Draw the centre of the viewport (centre of projection).
		//
		double d_cop_x, d_cop_y;	// Bottom left coordinates.

		d_cop_x = camprop.vpViewport.scScreenX(0.0f);
		d_cop_y = camprop.vpViewport.scScreenY(0.0f);

		TestDrawLine(d_cop_x, d_cop_y - 5, d_cop_x, d_cop_y + 5, pix);
		TestDrawLine(d_cop_x - 5, d_cop_y, d_cop_x + 5, d_cop_y, pix);
	}

	//******************************************************************************************
	void KeyPress(int c_key_code)
	{
/*
		// Get a pointer to the main camera.
		rptr<CCamera> pcam = &petcamGetPrimaryCamera(wWorld)->camCamera;

		if (c_key_code == 'H')
		{
			bShowMenu = !bShowMenu;
			return;
		}

		switch(eacCurr)
		{
			case eNO_ACTION :
				if (vpettShapes.size() && bWithin(c_key_code-'1', 0, (int)vpettShapes.size()-1))
				{
					eacCurr = eMODIFY_OBJECT;
					pettCurr = vpettShapes[c_key_code - '1'];
				}
				else switch(c_key_code)
				{
					case 'C' :
						eacCurr = eMODIFY_CAMERA;
						break;

					case 'V' :
						eacCurr = eMODIFY_VIEWPORT;
						break;

					case 'Z':
						psrMain->seterfState ^= erfZ_BUFFER;
						break;

					case 'I':
						psrMain->seterfState ^= erfLIGHT_SHADE;
						StatDB.Reset();
						break;

					case 'T':
						psrMain->seterfState ^= erfTEXTURE;
						StatDB.Reset();
						break;

					case 'P':
						psrMain->seterfState ^= erfPERSPECTIVE;
						StatDB.Reset();
						break;

					case 'G':
						CLight::bGammaCorrect ^= 1;
						if (CLight::bGammaCorrect)
						{
							if (CLight::fInvMonitorGamma > 0.3f)
								CLight::fInvMonitorGamma -= 0.1f;
							else
								CLight::fInvMonitorGamma = 1.0f;
						}
						StatDB.Reset();
						break;

					default :
						break;
				}
				break;


			case eMODIFY_CAMERA :
				switch(c_key_code)
				{
					case 'M' :
						eacCurr = eMOVE_CAMERA;
						break;

					case 'R' :
						eacCurr = eROTATE_CAMERA;
						break;

					case 'A' :
						eacCurr = eADJUST_FIELD_OF_VIEW;
						break;

					case 'Z' :
						eacCurr = eADJUST_ZOOM;
						break;

					case 'F' :
						eacCurr = eMOVE_FAR_CLIP_PLANE;
						break;

					case 'N' :
						eacCurr = eMOVE_NEAR_CLIP_PLANE;
						break;

					case 'Q' :
						eacCurr = eNO_ACTION;
						break;
							
					default :
						break;
				}
				break;

			case eMOVE_CAMERA :
				switch(c_key_code)
				{
					case 'U' :
						v3CamPos.tZ += rENTITY_MOVE_INCR;
						break;

					case 'D' :
						v3CamPos.tZ -= rENTITY_MOVE_INCR;
						break;

					case 'L' :
						v3CamPos.tX -= rENTITY_MOVE_INCR;
						break;
						
					case 'R' :
						v3CamPos.tX += rENTITY_MOVE_INCR;
						break;
							
					case 'I' :
						v3CamPos.tY += rENTITY_MOVE_INCR;
						break;
						
					case 'O' :
						v3CamPos.tY -= rENTITY_MOVE_INCR;
						break;
							
					case 'Q' :
						eacCurr = eMODIFY_CAMERA;
						break;
							
					default :
						break;
				}
				break;

			case eROTATE_CAMERA :
				switch(c_key_code)
				{
					case 'X' :
						angCamXRot += angENTITY_ROT_INCR_INCR;
						break;
							
					case 'U' :
						angCamXRot -= angENTITY_ROT_INCR_INCR;
						break;
							
					case 'Y' :
						angCamYRot += angENTITY_ROT_INCR_INCR;
						break;
							
					case 'V' :
						angCamYRot -= angENTITY_ROT_INCR_INCR;
						break;
							
					case 'Z' :
						angCamZRot += angENTITY_ROT_INCR_INCR;
						break;
							
					case 'W' :
						angCamZRot -= angENTITY_ROT_INCR_INCR;
						break;
							
					case 'R' :
						angCamXRot = 0;
						angCamYRot = 0;
						angCamZRot = 0;
						break;
							
					case 'Q' :
						eacCurr = eMODIFY_CAMERA;
						break;
							
					default :
						break;
				}
				break;

			case eADJUST_FIELD_OF_VIEW :
				switch(c_key_code)
				{
					case 'U' :
						pcamGetCamera()->angFieldOfView += angFIELD_OF_VIEW_INCR;
						break;
						
					case 'D' :
						pcamGetCamera()->angFieldOfView -= angFIELD_OF_VIEW_INCR;
						break;
							
					case 'Q' :
						eacCurr = eMODIFY_CAMERA;
						break;
							
					default :
						break;
				}
				break;

			case eADJUST_ZOOM :
				switch(c_key_code)
				{
					case 'U' :
						pcamGetCamera()->fZoomFactor += fZOOM_INCR;
						break;
						
					case 'D' :
						pcamGetCamera()->fZoomFactor -= fZOOM_INCR;
						break;
							
					case 'R' :
						pcamGetCamera()->fZoomFactor = 1.0f;
						break;
							
					case 'Q' :
						eacCurr = eMODIFY_CAMERA;
						break;
							
					default :
						break;
				}
				break;

			case eMOVE_FAR_CLIP_PLANE :
				switch(c_key_code)
				{
					case 'U' :
						pcamGetCamera()->rFarClipPlaneDist += rCLIP_PLANE_INCR;
						break;
						
					case 'D' :
						pcamGetCamera()->rFarClipPlaneDist -= rCLIP_PLANE_INCR;
						break;
							
					case 'Q' :
						eacCurr = eMODIFY_CAMERA;
						break;
							
					default :
						break;
				}
				break;

			case eMOVE_NEAR_CLIP_PLANE :
				switch(c_key_code)
				{
					case 'U' :
						pcamGetCamera()->rNearClipPlaneDist += rCLIP_PLANE_INCR;
						break;
						
					case 'D' :
						pcamGetCamera()->rNearClipPlaneDist -= rCLIP_PLANE_INCR;
						break;
							
					case 'Q' :
						eacCurr = eMODIFY_CAMERA;
						break;
							
					default :
						break;
				}
				break;


			case eMODIFY_VIEWPORT :
				switch(c_key_code)
				{
					case 'M' :
						eacCurr = eMOVE_VIEWPORT;
						break;

					case 'R' :
						eacCurr = eRESIZE_VIEWPORT;
						break;

					case 'C' :
						eacCurr = eMOVE_VIEWPORT_CENTRE;
						break;

					case 'Q' :
						eacCurr = eNO_ACTION;
						break;
							
					default :
						break;
				}
				break;

			case eMOVE_VIEWPORT :
				switch(c_key_code)
				{
					case 'U' :
						pcam->vpViewport.MovePosition(0.0, -dDISP_WIN_INCR);
						break;
						
					case 'D' :
						pcam->vpViewport.MovePosition(0.0, dDISP_WIN_INCR);
						break;
							
					case 'L' :
						pcam->vpViewport.MovePosition(-dDISP_WIN_INCR, 0.0);
						break;
						
					case 'R' :
						pcam->vpViewport.MovePosition(dDISP_WIN_INCR, 0.0);
						break;
							
					case 'Q' :
						eacCurr = eMODIFY_VIEWPORT;
						break;
							
					default :
						break;
				}
				break;

			case eRESIZE_VIEWPORT :
				switch(c_key_code)
				{
					case 'U' :
						pcam->vpViewport.ReSize(0.0, -dDISP_WIN_INCR);
						break;
						
					case 'D' :
						pcam->vpViewport.ReSize(0.0, dDISP_WIN_INCR);
						break;
							
					case 'L' :
						pcam->vpViewport.ReSize(-dDISP_WIN_INCR, 0.0);
						break;
						
					case 'R' :
						pcam->vpViewport.ReSize(dDISP_WIN_INCR, 0.0);
						break;
							
					case 'Q' :
						eacCurr = eMODIFY_VIEWPORT;
						break;
							
					default :
						break;
				}
				break;

			case eMOVE_VIEWPORT_CENTRE :
				switch(c_key_code)
				{
					case 'U' :
						pcam->vpViewport.MoveVirtualOrigin(0.0, -dCOP_INCR);
						break;
						
					case 'D' :
						pcam->vpViewport.MoveVirtualOrigin(0.0, dCOP_INCR);
						break;
							
					case 'L' :
						pcam->vpViewport.MoveVirtualOrigin(-dCOP_INCR, 0.0);
						break;
						
					case 'R' :
						pcam->vpViewport.MoveVirtualOrigin(dCOP_INCR, 0.0);
						break;
							
					case 'C' :
						pcam->vpViewport.SetVirtualOrigin(0.5, 0.5);
						break;
							
					case 'Q' :
						eacCurr = eMODIFY_VIEWPORT;
						break;
							
					default :
						break;
				}
				break;


			case eMODIFY_OBJECT :
				switch(c_key_code)
				{
					//
					// Move object.
					//
												
					case VK_UP:
						pettCurr->p3Ref().v3Pos.tZ += rENTITY_MOVE_INCR;
						break;

					case VK_DOWN:
						pettCurr->p3Ref().v3Pos.tZ -= rENTITY_MOVE_INCR;
						break;

					case VK_LEFT:
						pettCurr->p3Ref().v3Pos.tX -= rENTITY_MOVE_INCR;
						break;
						
					case VK_RIGHT:
						pettCurr->p3Ref().v3Pos.tX += rENTITY_MOVE_INCR;
						break;
							
					case VK_PGUP:
						pettCurr->p3Ref().v3Pos.tY += rENTITY_MOVE_INCR;
						break;
						
					case VK_PGDN:
						pettCurr->p3Ref().v3Pos.tY -= rENTITY_MOVE_INCR;
						break;
							
					//
					// Rotate object.
					//

					case 'X' :
						pettCurr->v3RotateVelocity.tX += (float)angENTITY_ROT_INCR_INCR;
						break;
							
					case 'U' :
						pettCurr->v3RotateVelocity.tX -= (float)angENTITY_ROT_INCR_INCR;
						break;
							
					case 'Y' :
						pettCurr->v3RotateVelocity.tY += (float)angENTITY_ROT_INCR_INCR;
						break;
							
					case 'V' :
						pettCurr->v3RotateVelocity.tY -= (float)angENTITY_ROT_INCR_INCR;
						break;
							
					case 'Z' :
						pettCurr->v3RotateVelocity.tZ += (float)angENTITY_ROT_INCR_INCR;
						break;
							
					case 'W' :
						pettCurr->v3RotateVelocity.tZ -= (float)angENTITY_ROT_INCR_INCR;
						break;
							
					case 'R' :
						// Stop the current object's rotation.
						pettCurr->v3RotateVelocity = CVector3<>(0.0f, 0.0f, 0.0f);
						break;
					
					//
					// Scale object.
					//
							
					case VK_ADD :
						pettCurr->pshiGetShapeInfo()->rScale *= rOBJ_SCALE_INCR;
						break;
						
					case VK_SUBTRACT :
						pettCurr->pshiGetShapeInfo()->rScale /= rOBJ_SCALE_INCR;
						break;
							
					case 'Q' :
						eacCurr = eNO_ACTION;
						break;

					default :
						break;
				}
				break;

		}

		ShowMenuAndStats();
*/
	}

	void NewRaster()
	{
		ShowMenuAndStats();

		Init();
	}

	void ShowMenuAndStats()
	{
		//
		// Print the current menu.
		//
		conStd.ClearScreen();

/*
		if (bShowMenu)
		{
			switch(eacCurr)
			{
				case eNO_ACTION :
				{
					conStd.Print("No action selected.\n\n");
					conStd.Print("V  Modify viewport parameters.\n");
					conStd.Print("C  Modify camera parameters.\n");
					conStd.Print("L  Modify light parameters.\n");

					for (uint u = 0; u < vpettShapes.size(); u++)
						conStd.Print("%c  Modify %s parameters.\n", 
								 (char)('1' + u), vpettShapes[u]->strDesc);

					conStd.Print("\n");
					conStd.Print("Z  Toggle Z-buffer (%d)\n", 
						!!psrMain->seterfState[erfZ_BUFFER]);
					conStd.Print("I  Toggle intensity shading (%d)\n", 
						!!psrMain->seterfState[erfLIGHT_SHADE]);
					conStd.Print("T  Toggle texturing (%d)\n", 
						!!psrMain->seterfState[erfTEXTURE]);
					conStd.Print("P  Toggle clipping perspective (%d)\n", 
						!!psrMain->seterfState[erfPERSPECTIVE]);
					conStd.Print("G  Toggle gamma adjustment (%d:%f)\n",
						CLight::bGammaCorrect, CLight::fInvMonitorGamma);
					break;
				}

				case eMODIFY_CAMERA :
					conStd.Print("Modify camera.\n\n");
					conStd.Print("M  Move camera.\n");
					conStd.Print("R  Rotate camera.\n");
					conStd.Print("A  Adjust field of view.\n");
					conStd.Print("Z  Set zoom factor.\n");
					conStd.Print("F  Move far clipping plane.\n");
					conStd.Print("N  Move near clipping plane.\n");
					conStd.Print("Q  Previous menu.\n");
					break;

				case eMOVE_CAMERA :
					conStd.Print("Move camera.\n");
					conStd.Print("X: %.4f\n",   v3CamPos.tX);
					conStd.Print("Y: %.4f\n",   v3CamPos.tY);
					conStd.Print("Z: %.4f\n\n", v3CamPos.tZ);
					conStd.Print("U/D/L/R/I/O  Move.\n");
					conStd.Print("Q            Previous menu.\n");
					break;

				case eROTATE_CAMERA :
					conStd.Print("Rotate camera.\n");
					conStd.Print("X: %f\n",   dRadiansToDegrees(angCamXRot));
					conStd.Print("Y: %f\n",   dRadiansToDegrees(angCamYRot));
					conStd.Print("Z: %f\n\n", dRadiansToDegrees(angCamZRot));
					conStd.Print("X/Y/Z  Increase X/Y/Z rotation.\n");
					conStd.Print("U/V/W  Decrease X/Y/Z rotation.\n");
					conStd.Print("R      Reset rotation.\n");
					conStd.Print("Q      Previous menu.\n");
					break;

				case eADJUST_FIELD_OF_VIEW :
					conStd.Print("Adjust field of view: %f\n\n", dRadiansToDegrees(pcamGetCamera()->angFieldOfView));
					conStd.Print("U/D  Increase/decrease angle of view.\n");
					conStd.Print("Q    Previous menu.\n");
					break;

				case eADJUST_ZOOM :
					conStd.Print("Adjust zoom: %.4f\n\n", (double)pcamGetCamera()->fZoomFactor);
					conStd.Print("U/D  Increase/decrease zoom.\n");
					conStd.Print("R    Reset value.\n");
					conStd.Print("Q    Previous menu.\n");
					break;

				case eMOVE_FAR_CLIP_PLANE :
					conStd.Print("Move far clipping plane: %.4f\n\n", (double)pcamGetCamera()->rFarClipPlaneDist);
					conStd.Print("U/D  Increase/decrease far clipping plane.\n");
					conStd.Print("Q    Previous menu.\n");
					break;

				case eMOVE_NEAR_CLIP_PLANE :
					conStd.Print("Move near clipping plane: %.4f\n\n", (double)pcamGetCamera()->rNearClipPlaneDist);
					conStd.Print("U/D  Increase/decrease near clipping plane.\n");
					conStd.Print("Q    Previous menu.\n");
					break;



				case eMODIFY_VIEWPORT :
					conStd.Print("Modify viewport.\n\n");
					conStd.Print("M  Move viewport.\n");
					conStd.Print("R  Resize viewport.\n");
					conStd.Print("C  Move centre of viewport.\n");
					conStd.Print("Q  Previous menu.\n");
					break;

				case eMOVE_VIEWPORT :
					conStd.Print("Move viewport.\n\n");
					conStd.Print("U/D/L/R  Move.\n");
					conStd.Print("Q        Previous menu.\n");
					break;

				case eRESIZE_VIEWPORT :
					conStd.Print("Resize viewport.\n\n");
					conStd.Print("U/D  Adjust height.\n");
					conStd.Print("L/R  Adjust width.\n");
					conStd.Print("Q    Previous menu.\n");
					break;

				case eMOVE_VIEWPORT_CENTRE :
					conStd.Print("Move centre of viewport.\n\n");
					conStd.Print("U/D/L/R  Move.\n");
					conStd.Print("C        Reset centre.\n");
					conStd.Print("Q        Previous menu.\n");
					break;


				case eMODIFY_OBJECT :
					conStd.Print("Modify object (%s).\n\n", pettCurr->strDesc);
					conStd.Print("Move object.\n");
					conStd.Print(" Left/Right:  X: %.4f\n", pettCurr->p3Get().v3Pos.tX);
					conStd.Print(" PgUp/PgDown: Y: %.4f\n", pettCurr->p3Get().v3Pos.tY);
					conStd.Print(" Up/Down:     Z: %.4f\n", pettCurr->p3Get().v3Pos.tZ);
					conStd.Print("\n");
					conStd.Print("Rotate object.\n");
					conStd.Print(" X/U:  X': %f\n",   dRadiansToDegrees(pettCurr->v3RotateVelocity.tX));
					conStd.Print(" Y/V:  Y': %f\n",   dRadiansToDegrees(pettCurr->v3RotateVelocity.tY));
					conStd.Print(" Z/W:  Z': %f\n\n", dRadiansToDegrees(pettCurr->v3RotateVelocity.tZ));
					conStd.Print("\n");
					conStd.Print("+/-: Scale object: %.4f\n\n", pettCurr->pshiGetShapeInfo()->rScale);
					conStd.Print("Q    Previous menu.\n");
					break;
			}

			conStd.Print("H:   Hide menu.\n");
			conStd.Print("\n");
		}
		else
			conStd.Print("Press H for menu.\n");

		uStatLine = conStd.uCursorY;
		conStd.Show();
*/
	}
}
TestPipeLine;

CAppShell* pappMain = &TestPipeLine;


//**********************************************************************************************
void TestDrawLine(double d_xa, double d_ya, double d_xb, double d_yb, TPixel pix)
{
	double d_dx, d_dy;

	d_dx = d_xb - d_xa;
	d_dy = d_yb - d_ya;

	if(Abs(d_dx) > Abs(d_dy))
	{
		if (d_xa > d_xb)
		{
			Swap(d_xa, d_xb);
			Swap(d_ya, d_yb);
		}

		double d_y_incr;
		d_y_incr = d_dy / d_dx;

		for ( ;d_xa <= d_xb; d_xa += 1)
		{
			if (d_xa >= 0 && d_ya >= 0 && d_xa < prasMainScreen->iWidth && d_ya < prasMainScreen->iHeight)
				prasMainScreen->PutPixel((int)d_xa, (int)d_ya, pix);

			d_ya += d_y_incr;
		}
	}
	else
	{
		if (d_ya > d_yb)
		{
			Swap(d_xa, d_xb);
			Swap(d_ya, d_yb);
		}

		double d_x_incr;
		d_x_incr = d_dx / d_dy;

		for ( ;d_ya <= d_yb; d_ya += 1)
		{
			if (d_xa >= 0 && d_ya >= 0 && d_xa < prasMainScreen->iWidth && d_ya < prasMainScreen->iHeight)
				prasMainScreen->PutPixel((int)d_xa, (int)d_ya, pix);

			d_xa += d_x_incr;
		}
	}
};



	
