 /*********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents: options for GUIApp
 *
 **********************************************************************************************
 *
 * $Log: /JP2_PC/Source/GUIApp/Options.cpp $
 * 
 * 67    7/22/98 10:03p Agrant
 * Removed data daemon and pre-fetching
 * 
 * 66    7/21/98 8:58p Agrant
 * optional control of the load verbose flag
 * 
 * 65    98/07/08 0:57 Speter
 * Removed leash crap.
 * 
 * 64    98.06.26 11:37a Mmouni
 * Now adjusts desired far-clip distance.
 * 
 * 63    5/11/98 2:24p Mlange
 * Made water resolution range variable types.
 * 
 * 62    5/10/98 2:06p Pkeet
 * Removed unused render cache parameters.
 * 
 * 61    98/05/08 14:02 Speter
 * Added "Curve Bumps" override flag.
 * 
 * 60    98/03/24 21:44 Speter
 * New water resolution params.
 * 
 * 59    3/19/98 7:22p Agrant
 * added option to use the simple box shrink wrap.  Speeds up loading,
 * lowers frame rate slightly
 * 
 * 58    3/18/98 4:07p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 * 57    98/03/06 10:55 Speter
 * Moved bShowBones to CPhysicsSystem.
 * 
 *********************************************************************************************/
	
//
// Includes.
//

#include "StdAfx.h"
#include "common.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/Loader/DataDaemon.hpp"

#include "Lib/Sys/DebugConsole.hpp"

// For all those wacky flags.

#include "GUIAppDlg.h"
#include "GUIPipeLine.hpp"
#include "Lib/Physics/PhysicsSystem.hpp"
#include "Game/AI/AIMain.hpp"
#include "Game/DesignDaemon/Daemon.hpp"
#include "Lib/Renderer/RenderCacheInterface.hpp"
#include "Lib/Renderer/PipeLine.hpp"
#include "Lib/Renderer/RenderCacheInterface.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"
#include "Lib/Sys/StdDialog.hpp"
#include "resource.h"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Water.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"

#include "Lib/EntityDBase/MessageTypes/MsgControl.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"

#include "Lib/View/Clut.hpp"
#include "Lib/Renderer/LightBlend.hpp"
#include "Lib/Renderer/Fog.hpp"

#include "Lib/Loader/TextureManager.hpp"
#include "Lib/Loader/ImageLoader.hpp"

#include "stdio.h"
#include "string.h"


static int i_line_number = 0;

#define MAX_SYM_LENGTH 32
#define MAX_LINE_LENGTH 256

//
// Game menu symbols.
//

#define GAME_PLAY		"Play"
#define GAME_REPLAY_SAVE	"Replay Save File"
#define GAME_REPLAY_LOAD	"Replay Playback File"

#define GAME_PHYSICS	"Physics"
#define GAME_AI			"AI"

#define LOAD_SAVE_GAME	"Load SaveGame"

//
//  Edit menu
//

#define EDIT_TERRAIN_TEST	"Test Terrain"


//
// View menu symbols.
//

// Consoles
#define VIEW_CONSOLE_TERRAIN	"Terrain Console"
#define VIEW_CONSOLE_AI			"AI Console"
#define VIEW_CONSOLE_PHYSICS	"Physics Console"
#define VIEW_CONSOLE_SHADOWS	"Shadows Console"
#define VIEW_CONSOLE_DEPTH_SORT	"Depth Sort Console"
#define VIEW_CONSOLE_ART_STATS	"Art Stats Console"
#define VIEW_CONSOLE_OCCLUSION	"Occlusion Console"


// Stats.
#define VIEW_TOOLBAR		"Toolbar"
#define VIEW_STATS			"Stats"
#define VIEW_FPS			"FPS"
#define VIEW_CACHE_STATS	"Cache Stats"

#define VIEW_AVERAGE_STATS	"Average Stats"



// Wireframe
#define VIEW_HAIRS			"Hairs"
#define VIEW_HAIRS_VEGETATION  "Vegetation +"
#define VIEW_HAIRS_RADIUS      "Radius +"
#define VIEW_HAIRS_RADIUS_VALUE      "Radius + Distance"
#define VIEW_GLOBULES		"Globules"
#define VIEW_WIRES			"Wires"
#define VIEW_PINHEAD		"Pinhead"
#define VIEW_BONES			"Bones"

#define VIEW_TRIGGERS		"Triggers"

//
//  Action flags.
//
#define ACTION_CAMERA_FREE	"Camera Free"
#define ACTION_CAMERA_2M	"Camera 2m"
#define ACTION_TURN_WORLD_Z "Turn About World Z"

//
//  Options flags.
//

#define OPTIONS_RENDER_FLAGS "Render Flags"

#define CACHE_ON					"Cache"
#define CACHE_TEST					"Cache Test"
//	ERenderCacheModes erctMode;			// Render cache mode.

#define CACHE_INTERSECTING			"Cache Intersecting"
//	bool  bAddIntersectingObjects;		// If 'true' adds intersecting objects to the cache.

#define CACHE_PIXEL_RATIO			"Cache Pixel Ratio"
//	float fCacheToScreenMapping;		// Ratio between a cache pixel and a screen pixel.

#define CACHE_PERSPECTIVE_ERROR		"Cache Angular Error"
//	float fPerspectiveMaxError;			// Max error threshold for redrawing.

#define CACHE_MIN_PIXELS			"Cache Min Pixels"
//	int   iMinNumPixels;				// The minimum size for the render cache.

#define CACHE_AGE					"Cache Max Age"
//	int   iEuthanasiaAge;				// Number of frames to leave an unrendered cache
										// untouched.

#define CACHE_MAX_PROPORTIONAL_SIZE "Cache Max Rel Size"
//	TReal rMaxCacheSizeProportional;	// Maximum size of a cache proportional to the screen.

#define CACHE_MAX_SIZE				"Cache Max Abs Size"
//	TReal rMaxCacheSize;				// Absolute maximum size of a cache.


//
//	Colors.
//

#define BACKGROUND_COLOR "Background"
#define CLUT_START_COLOR "Clut Start Color"
#define CLUT_STOP_COLOR  "Clut Stop Color"
#define FOG_COLOR		 "Fog Color"
#define METAL_COLOR		 "Metal Color"
#define ALPHA_CHANNEL	 "Alpha Channel"

#define VERIFY_HASHING	 "Verify Hashing"

//
// Lights.
//

// Ambient light.
#define AMBIENT_INTENSITY			"Ambient"


//
// Player.
//

// Player position.
#define PLAYER_POSITION				"Player Position"


// Load options.
#define LOAD_AUTO_SELECT			"Load Auto-select"
extern int iMaxLoaderSelect;

//
//  Preload settings
//
#define PRELOAD_UPDATE_DISTANCE		"Preload Update Distance"
#define PRELOAD_BEYOND_CLIP			"Preload Beyond Clip"
#define PRELOAD_INTERVAL			"Preload Time Interval"
#define PRELOAD_THREADED			"Preload Threaded"
#define EAT_MEM						"Eat Memory"
#define PRELOAD_WASTED_MEM			"Preload Wasted Memory"

#define VIRTUAL_LOADER				"Virtual Loader"

#define BREAK_ALLOC					"_CrtSetBreakAlloc"



#define WINDOW_SIZE		 "Window Size"
#define FULL_SCREEN		 "Full Screen"
#define DEBUG_MOVEMENT	 "Debug Movement"
#define WANDER_DURATION	 "Wander Duration"
#define WANDER_DURATION_IN_SECS	 "Wander Time Secs"

#define SYSTEM_MEM		 "System Memory"

// Camera settings
#define NEAR_CLIP		"Near Clipping Plane"
#define FAR_CLIP		"Far Clipping Plane"
#define VIEW_ANGLE		"View Angle"
#define ZOOM			"Zoom"

// Fog Settings			
#define FOG_Z_HALF		"Fog Z-half"
#define FOG_EXPONENENT	"Fog Exponent"
#define FOG_LINEAR		"Fog Linear"

// Water settings
#define WATER_SCREEN_RES	"Water Screen Res"
#define WATER_WORLD_RES		"Water World Res"

// Load options
#define LOAD_FILE		 "GROFF"
#define LOAD_BUMPS		"Load Bumpmaps"
#define LOAD_SHRINK		"Load Shrinkwrap"
#define LOAD_REBUILD_PARTITIONS  "Rebuild Partitions"
#define LOAD_MESH_SPLITTING	"Mesh Splitting"
#define LOAD_MESH_CURVING	"Curve Bumps"
#define LOAD_VERBOSE		"Load Verbose"

#define MENU			"Menu"

#define OVERLAY_FILE	"Overlay File"
#define OVERLAY_PERIOD	"Overlay Period"

#define STEP_DISTANCE	"Step Distance"
#define TURN_ANGLE		"Turn Angle"


#define DEFAULT_BUMPINESS	"Bumpiness"
extern float fDefaultBumpiness;

extern CGUIAppDlg* pappdlgMainWindow;

#define pGUI pappdlgMainWindow

extern TSec sWanderStop;
extern bool bWanderDurationInSecs;

// Sunbeam table.
extern CLightBlend lbAlphaConstant;


void Complain(char *str_complaint)
{
	dprintf(str_complaint);
}


void LineComplain(char *str_complaint)
{
	char str[32];
	sprintf(str,"Line %d:  ", i_line_number);

	Complain(str);
	Complain(str_complaint);
}



#define bSameString	!strcmp


bool bParseRGB(int *i_r, int *i_g, int *i_b, char *str_rgb)
{
	int i_red  = 0;
	int i_green = 0;
	int i_blue = 0;

	if (sscanf(str_rgb, "%d %d %d", &i_red, &i_green, &i_blue) != 3)
	{
		LineComplain("Expected 3 Numbers, \"R G B\"");
		return false;
	}

	if (i_red < 0 || i_green < 0 || i_blue < 0)
	{
		LineComplain("R,G,B must all be greater than or equal to zero.");
		return false;
	}

	if (i_red >= 256 || i_green >= 256 || i_blue >= 256)
	{
		LineComplain("R,G,B must all be less than 256.");
		return false;
	}

	*i_r = i_red;
	*i_g = i_green;
	*i_b = i_blue;

	return true;

}

bool bParseBoolean(char *str_boolean)
{
	// True values
	//  on true yes
	if (bSameString(str_boolean, "on")	||
		bSameString(str_boolean, "true") ||
		bSameString(str_boolean, "yes")    )
	{
		return true;
	}
	
	// False value
	// off false no
	if (bSameString(str_boolean, "off")		||
		bSameString(str_boolean, "false")	||
		bSameString(str_boolean, "no")		)
	{
		return false;
	}

	// Finally, complain about the value.
	{
		char str[256];

		sprintf(str, "Expected boolean value, got \"%s\".\n", str_boolean);

		LineComplain(str);
		return false;
	}
}

float fParseFloat(char *str_float)
{
	float f = 0;

	if (1 == sscanf(str_float, "%f",&f))
		return f;
	else
	{
		char str[256];
		sprintf(str, "Expected floating point value, got \"%s\".\n", str_float);
		LineComplain(str);
		return 0.0f;
	}
}

int iParseInt(char *str_int)
{
	int i = 0;

	if (1 == sscanf(str_int, "%d",&i))
		return i;
	else
	{
		char str[256];
		sprintf(str, "Expected integer value, got \"%s\".\n", str_int);
		LineComplain(str);
		return false;
	}
}



void WriteBoolSymbol(FILE* pfile_options, char* str_symbol, bool b)
{
	if (b)
		fprintf(pfile_options, "%s: true\n", str_symbol);
	else
		fprintf(pfile_options, "%s: false\n", str_symbol);
}

void WriteFloatSymbol(FILE* pfile_options, char* str_symbol, float f)
{
	fprintf(pfile_options, "%s: %f\n", str_symbol,f);
}

void WriteIntSymbol(FILE* pfile_options, char* str_symbol, int i)
{
	fprintf(pfile_options, "%s: %d\n", str_symbol,i);
}

void GetOneWord(char *str_long_string, char *str_return_buffer)
{
	sscanf(str_long_string,"%s", str_return_buffer);
}


void RemoveTrailingWhiteSpace(char *str_data)
{
	// Time to remove the trailing whitespace.
	for (int i = strlen(str_data) - 1; i >= 0; i--)
	{
		if (str_data[i] == '\n' ||
			str_data[i] == '\t' ||
			str_data[i] == ' ')
		{
			str_data[i] = '\0';
		}
		else
		{
			break;
		}
	}
}



#define CASE(str)  if (bSameString(str_symbol, str))


void ParseData(char *str_symbol, char *str_data)
{
	// Now we're ready to actually load & set options.
	// Happy day!

	char str_one_word[256];

	//  Strip off preceding blanks.
	while(*str_data == ' ' ||
		  *str_data == '\t')
	{
		str_data++;
	}

	GetOneWord(str_data, str_one_word);


	// Menu options.

	{
		//
		// Game menu
		//


		CASE(BREAK_ALLOC)
		{
			int i = iParseInt(str_one_word);
			_CrtSetBreakAlloc(i);
			return;
		}


		CASE(LOAD_SAVE_GAME)
		{
			RemoveTrailingWhiteSpace(str_data);
			wWorld.bLoadWorld(str_data, false);
			return;
		}

		CASE(GAME_PLAY)
		{
			// Toggle debug/play if F8 pressed.
			if (bParseBoolean(str_one_word))
			{
				pGUI->SendMessage(WM_COMMAND, MENU_PLAY, 0);
			}
			else
				pGUI->SendMessage(WM_COMMAND, MENU_DEBUG, 0);
			return;
		}
		CASE(GAME_PHYSICS)
		{
			// Physics on or off
			Assert(pphSystem);
			pphSystem->bActive = bParseBoolean(str_one_word);
			return;
		}
		CASE(GAME_AI)
		{
			// AI on or off
			gaiSystem.bActive = bParseBoolean(str_one_word);
			return;
		}
	}


	
	// Edit menu.
	CASE(EDIT_TERRAIN_TEST)
	{
		// Only toggle if true, since toggling instantiates some terrain.
		if (bParseBoolean(str_one_word))
		{

			pGUI->bTerrainTest = false;
			pGUI->ToggleTerrainTest();
		}
		return;
	}

	{
		//
		// View menu.
		//
		CASE(VIEW_CONSOLE_SHADOWS)
		{
			if (conShadows.bIsActive() != bParseBoolean(str_one_word))
			{
				// The two flags are different.  Toggle to make them the same.
				pGUI->ToggleConShadows();
			}
			return;
		}
		CASE(VIEW_CONSOLE_AI)
		{
			if (conAI.bIsActive() != bParseBoolean(str_one_word))
			{
				// The two flags are different.  Toggle to make them the same.
				pGUI->ToggleConAI();
			}
			return;
		}
		CASE(VIEW_CONSOLE_PHYSICS)
		{
			if (conPhysics.bIsActive() != bParseBoolean(str_one_word))
			{
				// The two flags are different.  Toggle to make them the same.
				pGUI->ToggleConPhysics();
			}
			return;
		}
		CASE(VIEW_CONSOLE_TERRAIN)
		{
			if (conTerrain.bIsActive() != bParseBoolean(str_one_word))
			{
				// The two flags are different.  Toggle to make them the same.
				pGUI->ToggleConTerrain();
			}
			return;
		}
		CASE(VIEW_CONSOLE_DEPTH_SORT)
		{
			if (conDepthSort.bIsActive() != bParseBoolean(str_one_word))
			{
				// The two flags are different.  Toggle to make them the same.
				pGUI->ToggleConDepthSort();
			}
			return;
		}
		CASE(VIEW_CONSOLE_ART_STATS)
		{
			if (conArtStats.bIsActive() != bParseBoolean(str_one_word))
			{
				// The two flags are different.  Toggle to make them the same.
				pGUI->OnArtStats();
			}
			return;
		}
		CASE(VIEW_CONSOLE_OCCLUSION)
		{
			if (conOcclusion.bIsActive() != bParseBoolean(str_one_word))
			{
				// The two flags are different.  Toggle to make them the same.
				pGUI->ToggleOcclusionConsole();
			}
			return;
		}


		CASE(VIEW_TOOLBAR)
		{
			pGUI->bShowToolbar = bParseBoolean(str_one_word);
			return;
		}
		CASE(VIEW_STATS)
		{
			// Stats on or off
			pGUI->bShowStats = bParseBoolean(str_one_word);
			return;
		}
		CASE(VIEW_CACHE_STATS)
		{
			// Cache stats on or off
			// Toggle render cache stats flag.
			rcstCacheStats.bKeepStats = !bParseBoolean(str_one_word);

			// Now toggle.
			pGUI->OnShowCacheStats();
			return;
		}
		CASE(VIEW_FPS)
		{
			// Set the FPS flag.
			pGUI->bShowFPS  = bParseBoolean(str_one_word);
			return;
		}
		CASE(VIEW_AVERAGE_STATS)
		{
			pGUI->bAvgStats = bParseBoolean(str_one_word);
			return;	
		}

		CASE(VIEW_HAIRS)
		{
			pGUI->bShowHairs = bParseBoolean(str_one_word);
			return;
		}
		CASE(VIEW_HAIRS_VEGETATION)
		{
			pGUI->bCrosshairVegetation = bParseBoolean(str_one_word);
			return;
		}
		CASE(VIEW_HAIRS_RADIUS)
		{
			pGUI->bCrosshairRadius = bParseBoolean(str_one_word);
			return;
		}
		CASE(VIEW_HAIRS_RADIUS_VALUE)
		{
			pGUI->rCrosshairRadius = fParseFloat(str_one_word);
			return;
		}
		CASE(VIEW_GLOBULES)
		{
			pGUI->bShowSpheres = bParseBoolean(str_one_word);
			return;
		}
		CASE(VIEW_WIRES)
		{
			pGUI->bShowWire = bParseBoolean(str_one_word);
			return;
		}
		CASE(VIEW_PINHEAD)
		{
			pGUI->bShowPinhead = bParseBoolean(str_one_word);
			return;
		}
		CASE(VIEW_BONES)
		{
			pphSystem->bShowBones = bParseBoolean(str_one_word);
			return;
		}
	}

	CASE(VIEW_TRIGGERS)
	{
		CRenderContext::bRenderTriggers = bParseBoolean(str_one_word);
		return;
	}

	CASE(LOAD_AUTO_SELECT)
	{
		// Set the number of objects auto-selected on load.
		iMaxLoaderSelect = iParseInt(str_one_word);
		return;
	}

	CASE(VIRTUAL_LOADER)
	{
		// Cannot enable virtual loader if bump-curve override is on.
		CLoadImageDirectory::Enable(bParseBoolean(str_one_word) && !CMesh::bCurveBumps);
		return;
	}


	{	
		//
		// Action flags.
		//
		CASE(ACTION_CAMERA_FREE)
		{
			if (bParseBoolean(str_one_word))
			{
				// Free it!
				pcamGetCamera()->SetAttached(0);
			}
			return;
		}

		CASE(ACTION_CAMERA_2M)
		{
			Assert(pappdlgMainWindow);
			Assert(pcamGetCamera());

			pappdlgMainWindow->bCameraFloating = bParseBoolean(str_one_word);

			if (pappdlgMainWindow->bCameraFloating)
				pcamGetCamera()->SetAttached(0);
			return;
		}

		CASE(ACTION_TURN_WORLD_Z)
		{
			pGUI->bRotateAboutWorldZ = bParseBoolean(str_one_word);
		}
	}

	//
	// Options flags
	//

	// Render Flags.
	CASE(OPTIONS_RENDER_FLAGS)
	{
		// Render flags are kept in a set.  There ought to be a way to set
		// them individually, but for now it'll be group.
		uint u_flags = 0, u_version = 0;
		if (sscanf(str_data, "%o %o", &u_flags, &u_version) != 2)
		{
			LineComplain(OPTIONS_RENDER_FLAGS ": Expected octal flags and version number.\n");
			return;
		}
		if (u_version != erfVERSION)
		{
			LineComplain(OPTIONS_RENDER_FLAGS ": Invalid version number.\n");
			return;
		}

		prenMain->pSettings->seterfState = CSet<ERenderFeature>(u_flags, 1);
		return;
	}

	{
		//
		// Image caching
		//
		CASE(CACHE_ON)
		{
			bool b = bParseBoolean(str_one_word);
			if (b)
				rcsRenderCacheSettings.erctMode = ercmCACHE_ON;
			else
				rcsRenderCacheSettings.erctMode = ercmCACHE_OFF;
			return;
		}

		CASE(CACHE_INTERSECTING)
		{
			rcsRenderCacheSettings.bAddIntersectingObjects = bParseBoolean(str_one_word);
			return;
		}
		
		CASE(CACHE_PIXEL_RATIO)
		{
			return;
		}

		CASE(CACHE_MIN_PIXELS)
		{
			return;
		}

		CASE(CACHE_AGE)
		{
			rcsRenderCacheSettings.iEuthanasiaAge = iParseInt(str_one_word);
			return;
		}
	
	}

#ifdef USING_FETCH
	if (pddDataDaemon)
	{
		//
		//	Preload options
		//

		CASE(PRELOAD_UPDATE_DISTANCE)
		{
			float f = fParseFloat(str_one_word);
			if (f <= 0)
			{
				LineComplain("Bad Update Distance!\n");	
				f = 10.0f;
			}
			pddDataDaemon->rUpdateDistSqr = f;
			return;
		}

		CASE(PRELOAD_BEYOND_CLIP)
		{
			float f = fParseFloat(str_one_word);
			if (f <= 0)
			{
				LineComplain("Bad Beyond Clip Distance!\n");	
				f = 10.0f;
			}
			pddDataDaemon->rDistancePastFarClip = f;
			return;
		}

		CASE(PRELOAD_INTERVAL)
		{
			float f = fParseFloat(str_one_word);
			if (f <= 0)
			{
				LineComplain("Bad Time Interval\n");	
				f = 1.0f;
			}
			pddDataDaemon->sPrefetchInterval = f;
			return;
		}
		
		CASE(PRELOAD_THREADED)
		{
			pddDataDaemon->bThreadedLoad = bParseBoolean(str_one_word);
			return;
		}

	}
#endif



	CASE(BACKGROUND_COLOR)
	{
		int i_red  = 0;
		int i_green = 0;
		int i_blue = 0;

		if (bParseRGB(&i_red, &i_green, &i_blue, str_data))
		{
			prenMain->pSettings->bClearBackground = true;
			pGUI->clrBackground = CColour(i_red, i_green, i_blue);
			prenMain->pSettings->clrBackground = pGUI->clrBackground;
			prenMain->UpdateSettings();
		}
		return;
	}

	CASE(CLUT_START_COLOR)
	{
		int i_red  = 0;
		int i_green = 0;
		int i_blue = 0;

		if (bParseRGB(&i_red, &i_green, &i_blue, str_data))
		{
			clrDefStartRamp = CColour(i_red, i_green, i_blue);
		}
		return;
	}

	CASE(CLUT_STOP_COLOR)
	{
		int i_red  = 0;
		int i_green = 0;
		int i_blue = 0;

		if (bParseRGB(&i_red, &i_green, &i_blue, str_data))
		{
			clrDefEndRamp = CColour(i_red, i_green, i_blue);
		}
		return;
	}

	CASE(FOG_COLOR)
	{
		int i_red  = 0;
		int i_green = 0;
		int i_blue = 0;

		if (bParseRGB(&i_red, &i_green, &i_blue, str_data))
		{
			clrDefEndDepth = CColour(i_red, i_green, i_blue);
		}
		return;
	}

	CASE(METAL_COLOR)
	{
		// Metal reflection color
		int i_red  = 0;
		int i_green = 0;
		int i_blue = 0;

		if (bParseRGB(&i_red, &i_green, &i_blue, str_data))
		{
			clrDefReflection = CColour(i_red, i_green, i_blue);
		}
		return;
	}


	CASE(ALPHA_CHANNEL)
	{
		int i_alpha = -1;
		int i_red = 0;
		int i_green = 0;
		int i_blue = 0;
		float f_alpha = 0.0f;
		
		int i_ret = sscanf(str_data,"%d %d %d %d %f", 
							&i_alpha,
							&i_red,
							&i_green,
							&i_blue,
							&f_alpha);
		
		if (i_ret != 5)
		{
			LineComplain("Bad Number of Alpha arguments.\n");
			return;
		} 
		else if (i_alpha < 0 || i_alpha >= 16)
		{
			LineComplain("Bad Alpha channel index.\n");
			return;
		}
		else if (i_red < 0 || i_green < 0 || i_blue < 0)
		{
			LineComplain("Negative RGB value.\n");
			return;
		} 
		else if (i_red > 255 || i_green > 255 || i_blue > 255)
		{
			LineComplain("RGB value bigger than 255.\n");
			return;
		} 
		else if (f_alpha < 0 || f_alpha > 1)
		{
			LineComplain("Alpha value must be between 0 and 1.\n");
			return;
		}

		CLightBlend::SLightBlendSettings* plbs = lbAlphaConstant.lpsSettings + i_alpha;
		plbs->clrBlendColour = CColour(i_red, i_green, i_blue);
		plbs->fAlpha = f_alpha;

		return;
	}



	CASE(VERIFY_HASHING)
	{
		extern bool bVerifyHashing;
		bVerifyHashing = bParseBoolean(str_one_word);
		return;
	}

	CASE(LOAD_BUMPS)
	{
		CLoadWorld::bBumps = bParseBoolean(str_one_word);
		return;
	}

	CASE(LOAD_SHRINK)
	{
		CLoadWorld::bCreateWrap = bParseBoolean(str_one_word);
		return;
	}

	CASE(LOAD_REBUILD_PARTITIONS)
	{
		CLoadWorld::bRebuildPartitions = bParseBoolean(str_one_word);
		return;
	}

	CASE(LOAD_MESH_SPLITTING)
	{
		CMesh::bSplitPolygons = bParseBoolean(str_one_word);
		return;
	}
	
	CASE(LOAD_MESH_CURVING)
	{
		CMesh::bCurveBumps = bParseBoolean(str_one_word);
		if (CMesh::bCurveBumps)
			// Must disable virtual loading if this flag is set.
			CLoadImageDirectory::Enable(false);
		return;
	}

	CASE(LOAD_VERBOSE)
	{
		CLoadWorld::bVerbose = bParseBoolean(str_one_word);
		return;
	}


	CASE(FULL_SCREEN)
	{
		int i_index;

		int i_width;
		int i_height;
		int i_depth;

		if (sscanf(str_data,  "%d %d %d", &i_width, &i_height, &i_depth) != 3)
		{
			char str_error[256];
			sprintf(str_error, "Expected iWIDE iHIGH iBITS, got \"%s\".\n", str_data);
			LineComplain(str_error);
			return;
		}

		bool b_got_it = false;
		for (i_index = 0; i_index <= Video::iModes-1; i_index++)
		{
			if (i_width		== Video::ascrmdList[i_index].iW		&&
				i_height	== Video::ascrmdList[i_index].iH		&&
				i_depth		== Video::ascrmdList[i_index].iBits)
			{
				// Found it!
				b_got_it = true;
				break;
			}
		}

		if (b_got_it)
		{
			// Yahoo.  Send a change request message.
			CMessageNewRaster msgnewr(i_index, pGUI->bSystemMem, false);
			msgnewr.Dispatch();
		}
		else
		{
			LineComplain("Requested Full Screen Video mode not supported,\n");
		}

		return;
	}

	CASE(WINDOW_SIZE)
	{
		int i_x = 0;
		int i_y = 0;

		if (sscanf(str_data,  "%d %d", &i_x, &i_y) != 2)
		{
			LineComplain("Expected width and height, two integers.");
			return;
		}

		if (i_x < 25 || i_y < 25) 
		{
			LineComplain("Width and height must be at least 25.");
		}


		int i_width  = 2 * GetSystemMetrics(SM_CXFRAME);
		int i_height = 2 * GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
			   GetSystemMetrics(SM_CYMENU);

		// Set the window size.
		pGUI->SetWindowPos(&pGUI->wndTop, 0, 0, i_width + i_x, i_height + i_y, SWP_SHOWWINDOW | SWP_NOMOVE);

		return;
	}


	// Load options.
	CASE(LOAD_FILE)
	{
		// Want to load a GROFF file.
		// Time to remove the trailing whitespace.

		for (int i = strlen(str_data) - 1; i >= 0; i--)
		{
			if (str_data[i] == '\n' ||
				str_data[i] == '\t' ||
				str_data[i] == ' ')
			{
				str_data[i] = '\0';
			}
			else
			{
				break;
			}
		}


		CLoadWorld lw(str_data);
		return;
	}

	CASE(OVERLAY_FILE)
	{
		// Add an overlay to the list.
		SOverlay ovl;

		// Scan for a filename, followed optionally by 2 positions.
		if (sscanf(str_data, "%s %d %d", 
			str_one_word,
			&ovl.v2iOverlayPos.tX,
			&ovl.v2iOverlayPos.tY
		) < 1)
		{
			LineComplain("Overlay specification invalid.\n");
			return;
		}

		ovl.prasOverlay = prasReadBMP(str_one_word, true);
		if (!ovl.prasOverlay)
		{
			LineComplain("Overlay file not found.\n");
			return;
		}
	
		// Add overlay to the list.
		pVideoOverlay->AddOverlay(ovl);
		return;
	}

	CASE(OVERLAY_PERIOD)
	{
		int ms;
		if (sscanf(str_data, "%d", &ms) != 1)
			LineComplain("Overlay period invalid.\n");
		pVideoOverlay->SetPeriod(ms);
		return;
	}

	CASE(MENU)
	{
		// A way to turn off the GUIApp menu.

		// Doesn't work.
		if (!bParseBoolean(str_one_word))
		{
			CMenu* pm = pGUI->GetMenu();
			CMenu* pm_new = new CMenu();
			pm_new->LoadMenu(MENU_E3);
			pGUI->SetMenu(pm_new);
//			pm->Detach();
//			pm->LoadMenu(MENU_E3);
		}

		return;
	}

// Camera
	{
		CASE(NEAR_CLIP)
		{
			TReal r = fParseFloat(str_one_word);

			if (r <= 0.0)
			{
				LineComplain("Near clipping plane to close to camera.\n");
			}
			else
			{
				CCamera::SProperties camprop = pcamGetCamera()->campropGetProperties();

				camprop.rNearClipPlaneDist  = r;

				pcamGetCamera()->SetProperties(camprop);
			}
			return;
		}

		CASE(FAR_CLIP)
		{
			TReal r = fParseFloat(str_one_word);

			if (r <= 1.0)
			{
				LineComplain("Far clipping plane to close to camera.\n");
			}
			else
			{
				CCamera::SProperties camprop = pcamGetCamera()->campropGetProperties();

				camprop.rDesiredFarClipPlaneDist  = r;
				camprop.SetFarClipFromDesired();
	
				pcamGetCamera()->SetProperties(camprop);
			}
			return;
		}

		CASE(VIEW_ANGLE)
		{
			float f_angle = fParseFloat(str_one_word);

			CCamera::SProperties camprop = pcamGetCamera()->campropGetProperties();

			camprop.rViewWidth = tan(dDegreesToRadians(f_angle) * .5f);

			pcamGetCamera()->SetProperties(camprop);

			return;
		}

		CASE(ZOOM)
		{
			TReal r = fParseFloat(str_one_word);

			CCamera::SProperties camprop = pcamGetCamera()->campropGetProperties();

			camprop.fZoomFactor = r;

			pcamGetCamera()->SetProperties(camprop);

			return;
		}
	}


	{	//
		// Fog Settings			
		CFog::SProperties fogprop = fogFog.fogpropGetProperties();

		CASE(FOG_Z_HALF)
		{
			fogprop.rHalfFogY = fParseFloat(str_one_word);
			fogFog.SetProperties(fogprop);
			fogTerrainFog.SetProperties(fogprop);
			return;
		}

		CASE(FOG_EXPONENENT)
		{
			fogprop.rPower = fParseFloat(str_one_word);
			fogFog.SetProperties(fogprop);
			fogTerrainFog.SetProperties(fogprop);
			return;
		}

		CASE(FOG_LINEAR)
		{
			if (bParseBoolean(str_one_word))
				fogprop.efogFunction = CFog::efogLINEAR;
			else
				fogprop.efogFunction = CFog::efogEXPONENTIAL;
			fogFog.SetProperties(fogprop);
			fogTerrainFog.SetProperties(fogprop);
			return;
		}
	}	// End fog settings.


	{	//
		// Water settings.
		CASE(WATER_SCREEN_RES)
		{
			CEntityWater::rvarMaxScreenRes = fParseFloat(str_one_word);
			return;
		}

		CASE(WATER_WORLD_RES)
		{
			CEntityWater::rvarMaxWorldRes = fParseFloat(str_one_word);
			return;
		}
	}

	CASE(DEBUG_MOVEMENT)
	{
		pGUI->bAllowDebugMovement = bParseBoolean(str_one_word);
		return;
	}

	CASE(STEP_DISTANCE)
	{
		extern TReal rDebugStepDistance;
		
		float f_value = fParseFloat(str_one_word);

		if (f_value < .05)
		{
			LineComplain("Distance too small.\n");
			return;
		}
		else
			rDebugStepDistance	= f_value;

		return;
	}

	CASE(TURN_ANGLE)
	{
		extern TReal rDebugTurnAngle;

		float f_value = fParseFloat(str_one_word);

		if (f_value < 1.0f)
		{
			LineComplain("Angle too small.\n");
			return;
		}
		else
			rDebugTurnAngle	= f_value;

		return;
	}

	
	CASE(SYSTEM_MEM)
	{
		if (bParseBoolean(str_one_word))
		{
			pGUI->bSystemMem = false;
			// Then toggle to make it true.
			pGUI->OnSystemMem();
		}
		else
		{
			pGUI->bSystemMem = true;
			// Then toggle to make it false.
			pGUI->OnSystemMem();
		}
		return;
	}

	CASE(AMBIENT_INTENSITY)
	{
		//
		// Ambient light.
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
		plta->lvIntensity = fParseFloat(str_one_word);

		return;
	}

	CASE(PLAYER_POSITION)
	{
		Assert(gpPlayer);
		Assert(pcamGetCamera());

		// Player position.
		CVector3<> v3_pos = gpPlayer->v3Pos();
		sscanf
		(
			str_data,
			"%f %f %f",
			&v3_pos.tX,
			&v3_pos.tY,
			&v3_pos.tZ
		);
		gpPlayer->SetPos(v3_pos);

		// Make sure the camera is in the correct position.
		pcamGetCamera()->SetPos(gpPlayer->v3Pos());
		return;
	}

	CASE(DEFAULT_BUMPINESS)
	{
		float f = fParseFloat(str_one_word);

		if (f <= 0)
		{
			LineComplain("Bumpiness must be greater than zero.\n");
			return;
		}

		fDefaultBumpiness = f;

		return;
	}

	CASE(WANDER_DURATION)
	{
		float f = fParseFloat(str_one_word);

		if (f <= 0)
		{
			LineComplain("Wander duration ignored (<= 0).\n");
			return;
		}

		sWanderStop = f + CMessageStep::sStaticTotal;
		pGUI->bWander = true;

		return;
	}


	CASE(WANDER_DURATION_IN_SECS)
	{
		bWanderDurationInSecs = bParseBoolean(str_one_word);
		return;
	}


	// Default case:
	char str[256];
	sprintf(str,"Could not match symbol \"%s\"\n", str_symbol);

	LineComplain(str);
}


void SaveMenuSettings(char *str_filename)
{
	// It's so sad, really, as this will overwrite any default settings 
	// and any hand-editing you may have done.
	Assert(str_filename);
	Assert(*str_filename);

	//
	// Open the file for reading.
	//
	FILE *pfile_options = fopen(str_filename, "w");

	Assert(pfile_options);


	void WriteBoolSymbol(FILE* pfile_options, char* str_symbol, bool b);


	// Other non-saveable options.
	fprintf(pfile_options,"//    Boolean values can be on, true, yes, off, false, no.\n\n");
	fprintf(pfile_options,"// To load a groff file, can use multiple times:\n//%s: FILENAME\n", LOAD_FILE);
	fprintf(pfile_options,"// To load a save file:\n//%s: FILENAME\n", LOAD_SAVE_GAME);
	
	fprintf(pfile_options,"\n// To set window size:\n//%s: WIDTH HEIGHT\n", WINDOW_SIZE);

	// Bump map loading.
	fprintf(pfile_options,"\n\n// Bumpmap loading.\n");
	WriteBoolSymbol(pfile_options, LOAD_BUMPS, CLoadWorld::bBumps);

	// Shrink wrap loading.
	fprintf(pfile_options,"\n\n// Shrink wrap loading.  If false, will use a simple box shrinkwrap.\n");
	WriteBoolSymbol(pfile_options, LOAD_SHRINK, CLoadWorld::bCreateWrap);

	// partition rebuild.
	fprintf(pfile_options,"\n\n// Partition rebuild on load.\n");
	WriteBoolSymbol(pfile_options, LOAD_REBUILD_PARTITIONS, CLoadWorld::bRebuildPartitions);

	// partition rebuild.
	fprintf(pfile_options,"\n\n// Extra douts on load.\n");
	WriteBoolSymbol(pfile_options, LOAD_VERBOSE, CLoadWorld::bVerbose);

	// Mesh Splitting.
	fprintf(pfile_options,"\n\n// True if mesh polygons are split on load.\n");
	WriteBoolSymbol(pfile_options, LOAD_MESH_SPLITTING, CMesh::bSplitPolygons);
	WriteBoolSymbol(pfile_options, LOAD_MESH_CURVING, CMesh::bCurveBumps);

	fprintf(pfile_options,"\n// %s: false\n", MENU);
	fprintf(pfile_options,"\n// %s: false\n", DEBUG_MOVEMENT);
	WriteBoolSymbol(pfile_options, SYSTEM_MEM, pGUI->bSystemMem);


	fprintf(pfile_options, "\n// Camera Settings.\n");
	WriteFloatSymbol(pfile_options, NEAR_CLIP, pcamGetCamera()->campropGetProperties().rNearClipPlaneDist);
	WriteFloatSymbol(pfile_options, FAR_CLIP, pcamGetCamera()->campropGetProperties().rFarClipPlaneDist);
	float f_angle = 2.0 * atan(pcamGetCamera()->campropGetProperties().rViewWidth);
	f_angle = dRadiansToDegrees(f_angle);
	WriteFloatSymbol(pfile_options, VIEW_ANGLE, f_angle);
	WriteFloatSymbol(pfile_options, ZOOM, pcamGetCamera()->campropGetProperties().fZoomFactor);


	////////////////////////////////////////////
	//
	// Fog Settings			
	fprintf(pfile_options, "\n// Fog Settings.\n");

	CFog::SProperties fogprop = fogFog.fogpropGetProperties();
	WriteFloatSymbol(pfile_options, FOG_Z_HALF, fogprop.rHalfFogY);
	WriteFloatSymbol(pfile_options, FOG_EXPONENENT, fogprop.rPower);
	WriteBoolSymbol(pfile_options, FOG_LINEAR, fogprop.efogFunction == CFog::efogLINEAR);

	// Water Settings			
	fprintf(pfile_options, "\n// Water Settings.\n");
	WriteFloatSymbol(pfile_options, WATER_SCREEN_RES, CEntityWater::rvarMaxScreenRes);
	WriteFloatSymbol(pfile_options, WATER_WORLD_RES, CEntityWater::rvarMaxWorldRes);

	// Background color.
	if (prenMain)
	{
		fprintf(pfile_options,"\n\n// Background color, R G B each 0-255\n");
		CColour c = prenMain->pSettings->clrBackground;
		fprintf(pfile_options,"%s: %d %d %d\n", BACKGROUND_COLOR, (int)c.u1Red,(int)c.u1Green,(int)c.u1Blue);

		// Render flags.
		fprintf(pfile_options,"\n\n//  Render flags (octal).\n");
		fprintf(pfile_options,"%s: %o %o\n", OPTIONS_RENDER_FLAGS, 
			(uint)prenMain->pSettings->seterfState, erfVERSION);
	}



	fprintf(pfile_options,"\n\n// Colors, R G B each 0-255\n");
	CColour c = clrDefStartRamp;
	fprintf(pfile_options,"%s: %d %d %d\n", CLUT_START_COLOR, (int)c.u1Red,(int)c.u1Green,(int)c.u1Blue);
	c = clrDefEndRamp;
	fprintf(pfile_options,"%s: %d %d %d\n", CLUT_STOP_COLOR, (int)c.u1Red,(int)c.u1Green,(int)c.u1Blue);
	c = clrDefEndDepth;
	fprintf(pfile_options,"%s: %d %d %d\n", FOG_COLOR, (int)c.u1Red,(int)c.u1Green,(int)c.u1Blue);
	c = clrDefReflection;
	fprintf(pfile_options,"%s: %d %d %d\n", METAL_COLOR, (int)c.u1Red,(int)c.u1Green,(int)c.u1Blue);


	fprintf(pfile_options, "\n\n// Alpha Channels-  channel R G B opacity\n");
	for (int i_alpha = 0; i_alpha < 16; i_alpha++)
	{
		CLightBlend::SLightBlendSettings* plbs = lbAlphaConstant.lpsSettings + i_alpha;
		CColour c = plbs->clrBlendColour;
		float f_alpha = plbs->fAlpha;

		fprintf(pfile_options,"%s:\t%d\t%d\t%d\t%d\t%f\n", 
			ALPHA_CHANNEL, 
			i_alpha,
			(int)c.u1Red,(int)c.u1Green,(int)c.u1Blue,
			f_alpha);
	}


	extern bool bVerifyHashing;
	fprintf(pfile_options, "// TRUE when doing asset checking and official area release testing\n");
	WriteBoolSymbol(pfile_options, VERIFY_HASHING, bVerifyHashing);

	//
	// Ambient light.
	//
	// Iterate through the world database and find the first ambient light.
	// The ambient light is always the first one in the list.
	//
	CWDbQueryLights wqlt;

	// Return the light.
	rptr<CLight> plt = ptCastRenderType<CLight>(wqlt.tGet()->prdtGetRenderInfo());
	rptr<CLightAmbient> plta = rptr_dynamic_cast(CLightAmbient, plt);
	fprintf(pfile_options,"\n");
	WriteFloatSymbol(pfile_options, AMBIENT_INTENSITY, plta->lvIntensity);

	// Player position.
	fprintf(pfile_options,"\n");
	fprintf(pfile_options,"//Load player position. ***** NOTE ***** this might get \n");
	fprintf(pfile_options,"//confusing if Anne is loaded.\n");
	fprintf
	(
		pfile_options,
		"%s: %f %f %f\n", PLAYER_POSITION,
		gpPlayer->v3Pos().tX,
		gpPlayer->v3Pos().tY,
		gpPlayer->v3Pos().tZ
	);

	fprintf(pfile_options,"\n//  Num objects selected when loading.\n");
	WriteIntSymbol(pfile_options, LOAD_AUTO_SELECT, iMaxLoaderSelect);

	fprintf(pfile_options,"\n//  Debug Movement parameters.\n");

	extern TReal rDebugStepDistance;
	extern TReal rDebugTurnAngle;
	WriteFloatSymbol(pfile_options, STEP_DISTANCE, rDebugStepDistance);
	WriteFloatSymbol(pfile_options, TURN_ANGLE, rDebugTurnAngle);


	// Full screen.
	fprintf(pfile_options,"\n//%s: 640 480 16\n", FULL_SCREEN);


	// Game menu.
	fprintf(pfile_options,"\n\n// GAME menu flag settings.\n");
	WriteBoolSymbol(pfile_options, GAME_PLAY, gmlGameLoop.egmGameMode == egmPLAY);
	WriteBoolSymbol(pfile_options, GAME_PHYSICS, pphSystem->bActive);
	WriteBoolSymbol(pfile_options, GAME_AI, gaiSystem.bActive);

	// Edit menu.
	fprintf(pfile_options,"\n\n// EDIT menu flag settings.\n");
	WriteBoolSymbol(pfile_options, EDIT_TERRAIN_TEST, pGUI->bTerrainTest);

	// View menu.
	fprintf(pfile_options,"\n\n// VIEW menu flag settings.\n");

	WriteBoolSymbol(pfile_options, VIEW_CONSOLE_AI, conAI.bIsActive());
	WriteBoolSymbol(pfile_options, VIEW_CONSOLE_PHYSICS, conPhysics.bIsActive());
	WriteBoolSymbol(pfile_options, VIEW_CONSOLE_TERRAIN, conTerrain.bIsActive());
	WriteBoolSymbol(pfile_options, VIEW_CONSOLE_DEPTH_SORT, conDepthSort.bIsActive());
	WriteBoolSymbol(pfile_options, VIEW_CONSOLE_SHADOWS, conShadows.bIsActive());
	WriteBoolSymbol(pfile_options, VIEW_CONSOLE_ART_STATS, conArtStats.bIsActive());
	WriteBoolSymbol(pfile_options, VIEW_CONSOLE_OCCLUSION, conOcclusion.bIsActive());

	fprintf(pfile_options,"\n//%s: off\n", VIEW_TOOLBAR);
	WriteBoolSymbol(pfile_options, VIEW_STATS, pGUI->bShowStats);
	WriteBoolSymbol(pfile_options, VIEW_CACHE_STATS, rcstCacheStats.bKeepStats);
	WriteBoolSymbol(pfile_options, VIEW_FPS, pGUI->bShowFPS);
	WriteBoolSymbol(pfile_options, VIEW_AVERAGE_STATS, pGUI->bAvgStats);

	WriteBoolSymbol(pfile_options, VIEW_HAIRS, pGUI->bShowHairs);
	WriteBoolSymbol(pfile_options, VIEW_HAIRS_VEGETATION, pGUI->bCrosshairVegetation);
	WriteBoolSymbol(pfile_options, VIEW_HAIRS_RADIUS, pGUI->bCrosshairRadius);
	WriteFloatSymbol(pfile_options, VIEW_HAIRS_RADIUS_VALUE, pGUI->rCrosshairRadius);

	WriteBoolSymbol(pfile_options, VIEW_GLOBULES, pGUI->bShowSpheres);
	WriteBoolSymbol(pfile_options, VIEW_WIRES, pGUI->bShowWire);
	WriteBoolSymbol(pfile_options, VIEW_PINHEAD, pGUI->bShowPinhead);
	WriteBoolSymbol(pfile_options, VIEW_BONES, pphSystem->bShowBones);

	WriteBoolSymbol(pfile_options, VIEW_TRIGGERS, CRenderContext::bRenderTriggers);


	fprintf(pfile_options,"\n\n// ACTION menu flag settings.\n");

	WriteBoolSymbol(pfile_options, ACTION_CAMERA_FREE, !pcamGetCamera()->pinsAttached());
	
	Assert(pappdlgMainWindow);
	WriteBoolSymbol(pfile_options, ACTION_CAMERA_2M, pappdlgMainWindow->bCameraFloating);

	WriteBoolSymbol(pfile_options, ACTION_TURN_WORLD_Z, pGUI->bRotateAboutWorldZ);

	// Image cache settings.
	fprintf(pfile_options,"\n\n// Image caching flags.\n");

	// Set the render cache menus.
	bool b_on = false;
	bool b_test = false;
	switch (rcsRenderCacheSettings.erctMode)
	{
		case ercmCACHE_OFF:
			b_on = false;
			b_test = false;
			break;
		case ercmCACHE_ON:
			b_on = true;
			b_test = false;
			break;
		default:
			Assert(0);
	}
	WriteBoolSymbol(pfile_options, CACHE_ON, b_on);
	WriteBoolSymbol(pfile_options, CACHE_INTERSECTING,  rcsRenderCacheSettings.bAddIntersectingObjects);

	WriteIntSymbol  (pfile_options, CACHE_AGE, rcsRenderCacheSettings.iEuthanasiaAge);

	// These guys are harder, maybe Paul ought to do them.
//	WriteFloatSymbol(pfile_options, CACHE_MAX_PROPORTIONAL_SIZE, rcsRenderCacheSettings.rMaxCacheSizeProportional);
//	WriteFloatSymbol(pfile_options, CACHE_MAX_SIZE, rcsRenderCacheSettings.rMaxCacheSize);



		//
		//	Preload options
		//
#ifdef USING_FETCH
	if (pddDataDaemon)
	{
		fprintf(pfile_options, "\n\n//   Preload Options!\n");
		WriteFloatSymbol(pfile_options, PRELOAD_UPDATE_DISTANCE, sqrt(pddDataDaemon->rUpdateDistSqr));
		WriteFloatSymbol(pfile_options, PRELOAD_INTERVAL, pddDataDaemon->sPrefetchInterval);
		WriteBoolSymbol(pfile_options, PRELOAD_THREADED, pddDataDaemon->bThreadedLoad);
	}
#endif

	fprintf(pfile_options,"\n\n// Virtual loader flags.\n");
	WriteBoolSymbol(pfile_options, VIRTUAL_LOADER, CLoadImageDirectory::bEnabled());


	// Default bumpiness.
	fprintf(pfile_options,"\n\n// How bumpy are the unspecified bumpmaps?\n");
	WriteFloatSymbol(pfile_options, DEFAULT_BUMPINESS,	fDefaultBumpiness);

	fprintf(pfile_options,"\n\n// In seconds, greater than zero if wandering desired.\n");
	WriteFloatSymbol(pfile_options, WANDER_DURATION, -1.0f);

	fprintf(pfile_options,"\n\n// 'true' if wandering duration is in seconds, otherwise in # of frames.\n");
	WriteBoolSymbol(pfile_options, WANDER_DURATION_IN_SECS, bWanderDurationInSecs);

	fprintf(pfile_options, "\n\n// To set a breakpoint on a particular memory allocation-\n//%s: 666\n", BREAK_ALLOC);

	fclose(pfile_options);
}


void ParseLine(char *str_line)
{
	// Check for the comment symbol "//"
	if (str_line[0] == '/' && str_line[1] == '/')
	{
		return;
	}

	char str_symbol[MAX_SYM_LENGTH+2];
	int i;

	char *str_value = 0;
	int max_len = strlen(str_line);
	if (max_len > MAX_SYM_LENGTH)
		max_len = MAX_SYM_LENGTH;

	bool b_has_non_whitespace = false;
	bool b_found_symbol = false;


	//  Verify that the line has a symbol and a value.
	{
		// Scan for ':' 
		for (i = 0; i < max_len; i++)
		{
			if (str_line[i] == '\n')
			{
				// Found return before ':'- failed.
				break;
			}

			if (str_line[i] != '\t' &&			
				str_line[i] != ' ')
			{
				b_has_non_whitespace = true;
			}

			if (str_line[i] == ':')
			{
				str_symbol[i] = '\0';
				b_found_symbol = true;

				// Save the rest of the line for the value.
				str_value = str_line + i + 1;
				break;
			}
			else
				str_symbol[i] = str_line[i];
		}
	}

	// Signal an error if not.
	if (!b_found_symbol)
	{
		str_symbol[i] = '\0';

		// Failed to find a symbol.
		if (b_has_non_whitespace)
		{
			char str[256];
			sprintf(str, "Failed to find symbol in line %d, beginning \"%s\"\n", i_line_number, str_symbol);
			Complain(str);
		}

		return;
	}

	// Otherwise process the info.
	ParseData(str_symbol, str_value);
}

void ParseOptionFile(char *str_filename)
{
	Assert(str_filename);
	Assert(*str_filename);

	//
	// Open the file for reading.
	//
	FILE *pfile_options = fopen(str_filename, "r");

	// If not present, skip.
	if (!pfile_options)
		return;

	//
	// Iterate through the lines in the file and pass them to the line parser.
	//

	i_line_number = 0;
	char str_line[MAX_LINE_LENGTH];

	while(1)
	{
		char *str;
		str = fgets(str_line, MAX_LINE_LENGTH, pfile_options);

		if (str)
		{
			ParseLine(str_line);
			i_line_number++;
		}
		else
			break;
		
	}

	fclose(pfile_options);

}
