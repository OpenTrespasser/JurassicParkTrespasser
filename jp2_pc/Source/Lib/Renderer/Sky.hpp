/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Sky renderer CSkyRender
 *		Class pointer to the global sky.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Sky.hpp                                                  $
 * 
 * 17    98.08.19 6:01p Mmouni
 * Moved all quality setting values and multipliers into a single structure.
 * 
 * 16    7/21/98 5:20p Rwyatt
 * Set sky to use 128 colours
 * 
 * 15    7/20/98 10:27p Pkeet
 * Upgraded to Direct3D 4's interface.
 * 
 * 14    7/13/98 12:06a Pkeet
 * Added the 'ResetScreenSize' member function.
 * 
 * 13    98.06.26 7:40p Mmouni
 * Added sold fill mode to sky.
 * 
 * 12    4/22/98 12:36p Rwyatt
 * New text prop to set the flat shade colour
 * 
 * 11    4/21/98 2:56p Rwyatt
 * New default fog parameters for the sky.
 * 
 * 10    98.04.03 1:01p Mmouni
 * Added save/load methods to CSky.
 * 
 * 9     1/22/98 3:17p Pkeet
 * Added a purge function for Direct3D.
 * 
 * 8     1/05/98 6:26p Pkeet
 * Added the 'FillD3D' member function.
 * 
 * 7     1/02/98 2:35p Pkeet
 * Added structures and preliminary code to support Direct3D.
 * 
 * 6     10/22/97 5:07p Rwyatt
 * Added perspective textured and flat for non horizontal skies.
 * Optimized math and replaced constant expressions expressions
 * Made fog linear.
 * frustum vectors are now normalised direction vectors instead of the full length of the
 * frustum,
 * Sub division is variable.
 * 
 * 5     10/16/97 2:04p Agrant
 * Value table based constructor now allows instancing of CInfo-
 * not relevant for Sky, but here for uniformity.
 * 
 * 4     10/14/97 7:08p Rwyatt
 * New defaults
 * 
 * 3     10/14/97 4:26a Rwyatt
 * Added sky/clouds move speed in m/s
 * 
 * 2     10/13/97 8:14p Rwyatt
 * Formated for coding standards
 * 
 * 1     10/13/97 8:01p Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/

#ifndef LIB_RENDERER_SKY_HPP
#define LIB_RENDERER_SKY_HPP

#include "Lib/View/Raster.hpp"
#include "Lib/View/Colour.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"

//**********************************************************************************************
//
// Defaults for sky
//
#define iNUM_SKY_FOG_BANDS			64
#define iNUM_SKY_COLOURS			128

#define	fSKY_HORIZONTAL				0.02			// Tollerence before perspective is used
#define u4NUM_SKY_TRANSFORM_VERT	6				// number of vertices of the fustrum considered

#define	fDEFAULT_SKY_HEIGHT			1000.0f			// meters
#define fDEFAULT_SKY_NEAR_FOG		0.75f			// dot product
#define fDEFAULT_SKY_FAR_FOG		0.95f			// dot product
#define fDEFAULT_SKY_PIXEL_SCALE	0.055			// pixels pers meter
#define fDEFAULT_SKY_WIND_U			3.0f			// meters per second
#define fDEFAULT_SKY_WIND_V			1.8f			// meters per second
#define	u4DEFAULT_SKY_FOG_BITS		18				// fraction bits in fog values
//
//**************************************


//**********************************************************************************************
//
// The order of the vectors in the fustrum arrays
//
#define u4FRUST_TL					0
#define u4FRUST_TR					1
#define u4FRUST_BL					2
#define u4FRUST_BR					3
#define u4FRUST_TOP					4
#define u4FRUST_RIGHT				5
//
//**************************************


//**********************************************************************************************
//
#define fSKY_HORIZON_Z	0.05f


//**********************************************************************************************
//
#define u4SCREEN_DIVIDE_PIXELS	32





//**********************************************************************************************
//
class CSkyRender
// Prefix: sky
//
//**************************************
{
public:
	rptr<CRaster> prasD3DSky;
	bool          bUseD3D;

	//******************************************************************************************
	CSkyRender(rptr<CRaster> pras_sky, rptr<CRaster> pras_screen);

	//******************************************************************************************
	void DrawSkyToHorizon();


	//******************************************************************************************
	void SetWorkingConstants
	(
		float			f_pixels_per_meter,
		float			f_sky_height,
		float			f_fog_near,
		float			f_fog_far,
		uint32			u4_sub_division
	);

	//******************************************************************************************
	void ResetScreenSize();

	//******************************************************************************************
	void RemoveRenderSurface()
	{
		prasRenderSurface = rptr0;
	}

	//******************************************************************************************
	void NewRenderSurface(rptr<CRaster> pras_screen);

	//******************************************************************************************
	rptr<CRaster> prasGetSkyTexture()
	{
		return prasSkyTexture;
	}

	//******************************************************************************************
	float	fGetPixelsPerMeter()
	{
		return	fPixelsPerMeter;
	}

	//******************************************************************************************
	float	fGetFogNear()
	{
		return	fFogNear;
	}

	//******************************************************************************************
	float	fGetFogFar()
	{
		return	fFogFar;
	}

	//******************************************************************************************
	float	fGetHeight()
	{
		return	fSkyHeight;
	}

	//******************************************************************************************
	uint32 u4GetDivisionLength()
	{
		return u4DivisionLength;
	}


	//******************************************************************************************
	void SetSkyWind
	(
		float f_u_speed, 
		float f_v_speed
	)
	{
		fSkyOffsetdU = f_u_speed*65536.0f;
		fSkyOffsetdV = f_v_speed*65536.0f;
	}


	//******************************************************************************************
	void GetSkyWind
	(
		float& f_u_speed, 
		float& f_v_speed
	)
	{
		f_u_speed = fSkyOffsetdU / 65536.0f;
		f_v_speed = fSkyOffsetdV / 65536.0f;
	}


	//******************************************************************************************
	void SetSkyFogTable(CColour	clr_fog);

	//******************************************************************************************
	enum SkyDrawMode 
	{ 
		sdmFill,			// Solid color.
		sdmGradient,		// Solid gradient (fog).
		sdmTextured			// Textured.
	};

	//******************************************************************************************
	void SetDrawMode(SkyDrawMode sdm_mode)
	{
		sdmMode = sdm_mode;
	}

	//******************************************************************************************
	bool bSkyTextured()
	{
		return sdmMode == sdmTextured;
	}

	//******************************************************************************************
	void SetFilled(bool b_fill)
	{
		bFill = b_fill;
	}

	//******************************************************************************************
	bool bSkyFill()
	{
		return bFill;
	}

	//******************************************************************************************
	void SetFlatShadeColour(int32 i4_flat_colour)
	{
		u4FlatColourIndex = (uint32)i4_flat_colour;
	}


	//******************************************************************************************
	//
	// Helper functions to create the global sky and extract text properties
	//
	static void CreateSky
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	//******************************************************************************************
	//
	void PurgeD3D
	(
	);
	//
	// Purges the Direct3D version of the sky.
	//
	//**************************************

	//******************************************************************************************
	//
	void InitializeForD3D
	(
	);
	//
	// Sets up structrures for using the sky with Direct3D.
	//
	//**************************************

	//******************************************************************************************
	static void RemoveSky();

	//*****************************************************************************************
	//
	char* pcSave
	(
		char* pc
	) const;
	//
	// Saves the sky settings to a buffer.
	//
	//**************************************

	//*****************************************************************************************
	//
	const char* pcLoad
	(
		const char* pc
	);
	//
	// Loads the sky settings from a buffer.
	//
	//**************************************

protected:

	//******************************************************************************************
	void FillD3D(float f_offset_x, float f_offset_y);

	//******************************************************************************************
	void FillD3DBackdropFog(float f_offset_x, float f_offset_y);

	//******************************************************************************************
	void StartD3DSceneForSky();

	//******************************************************************************************
	void DrawD3D();

	//******************************************************************************************
	void DrawSkyTexturedLinear();

	//******************************************************************************************
	void DrawSkyFlatLinear();

	//******************************************************************************************
	void DrawSkyTexturedPerspective();

	//******************************************************************************************
	void DrawSkyFlatPerspective();

	//******************************************************************************************
	void SetTransformedCameraCorners();

	//******************************************************************************************
	void FillSky
	(
		uint16*	pu2_dst,			// pointer to the dst surface
		int32	i4_width,			// width in pixels to store
		int32	i4_pitch_adj,		// value to add to pointer at end of scan line to get tp the next
		uint32	u4_lines,			// number of lines to do
		uint32	u4_col				// colour packed into a DWORD
	);


	//******************************************************************************************
	//
	// Data Members
	//
	rptr<CRaster>	prasSkyTexture;			// Texture for the sky
	rptr<CRaster>	prasRenderSurface;		// Destination raster

	uint16*			pu2Raster;				// pointer to the 16bit destination
	int32			i4Pitch;				// pitch of the destination raster
	uint32			u4DivisionLength;		// number of pixels per division (must be even)..
	float			fDivisionsPerScanRep;	// 1.0 / number of divisions per scan line
	float			fDivisionLengthRepScale;// 1.0 / number of division pixel length * 65536.0
	float			fPixelsPerMeter;		// sky pixels per meter..
	float			fScale;					// Bitmap scale
	float			fFogFar;				// distance to far fog (100% fog)
	float			fFogNear;				// distance to minimum fog (before this is 0% fog)
	float			fFogScale;				// Scale from 0..1 to fog bands * 65536.0f
	float			fScreenWidthRep;		// 1/screen width
	float			fScreenHeightRep;		// 1/screen height
	float			fScreenWidthScaleRep;	// scales dU,dV into a per horizontal pixel quantity.
	float			fSkyHeight;				// height of sky in meters
	float			fSkyOffsetU;			// position offset of sky texture in U
	float			fSkyOffsetV;			// position offset of sky texture in V
	float			fSkyOffsetdU;			// how much the sky texture moves each frame * 65536.0
	float			fSkyOffsetdV;			// how much the sky texture moves each frame * 65536.0
	TSec			sFrameTime;				// used to calculate cloud/texture movement

	uint32			u4ScreenWidth;			// width of destination raster
	uint32			u4ScreenHeight;			// height of destination raster

	SkyDrawMode		sdmMode;				// How to draw the sky.
	bool			bFill;					// Fill screen with sky
		
	// colour table of size iNUM_SKY_FOG_BANDS*iNUM_SKY_FOG_COLOURS*2 bytes per entry
	uint16			u2FogTable[iNUM_SKY_FOG_BANDS][iNUM_SKY_COLOURS];
	uint32			u4Fog;					// Dword containing 2 fog pixels
	uint32			u4FlatColourIndex;		// Palette index of the flat shaded colour

	//******************************************************************************************
	static CVector3<>	av3Frustum[u4NUM_SKY_TRANSFORM_VERT];

	//******************************************************************************************
	static CVector3<>	av3FrustTran[u4NUM_SKY_TRANSFORM_VERT];

	//******************************************************************************************
	static CVector3<>	v3Camera;
};




//**********************************************************************************************
//
extern CSkyRender*	gpskyRender;
//
//**************************************


#endif