/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CScreenRender					Provides interface to rasterising functions.
 *		CRenderDesc						Describes and creates a CScreenRender.
 *		CArrayRenderDesc				Contains list of all CRenderDescs.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ScreenRender.hpp                                         $
 * 
 * 121   98.09.24 1:40a Mmouni
 * Shrunk CRenderPolygon by a large amount.
 * Multiple list depth sort stuff is now on a compile switch.
 * 
 * 120   98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 119   8/17/98 4:50p Pkeet
 * Added the 'ehwFluid' enumeration. The 'FastInit' member function of 'CRenderPolygon' now sets
 * the hardware enumeration data member for software by default.
 * 
 * 118   8/16/98 3:09p Pkeet
 * Added the 'bTargetMainScreen' member function.
 * 
 * 117   8/03/98 4:31p Pkeet
 * Added the 'bPrerasterized' flag.
 * 
 * 116   8/02/98 9:23p Pkeet
 * Added the 'u4D3DFog' to 'CRenderVertex.'
 * 
 * 115   8/02/98 8:37p Pkeet
 * Added the 'SetHardwareOut' member function.
 * 
 * 114   8/01/98 11:11p Pkeet
 * Added the 'fMaxLevel' data member to 'CRenderPolygon.'
 * 
 * 113   8/01/98 4:41p Pkeet
 * Added the 'pixSetBackground' member function.
 * 
 * 112   7/29/98 1:56p Pkeet
 * Added a parameter to the 'SetMipLevel' member function of 'CRenderPolygon' to reduce the
 * burden on the VM loader by making VM requests only when the Direct3D version is required and
 * is present.
 * 
 * 111   7/28/98 8:24p Pkeet
 * Added the 'EAddressMode' type.
 * 
 * 110   7/24/98 3:08p Pkeet
 * Added the 'pshOwningShape' backpointer to 'CRenderPolygon.'
 * 
 * 109   7/23/98 6:22p Pkeet
 * Added a member function to detect if the destination target is hardware. Added more hardware
 * polygon types.
 * 
 * 108   98.06.18 3:55p Mmouni
 * Added software alpha switch.
 * 
 * 107   6/14/98 2:48p Pkeet
 * Added the 'fGetFarthestZ' member function.
 * 
 * 106   6/09/98 3:52p Pkeet
 * Added a flag to 'SetD3DFlagForPolygons' to use Direct3D or not.
 * 
 * 105   6/08/98 8:08p Pkeet
 * Modified the 'SetD3DFlagForPolygons' parameters to work in the pipeline.
 * 
 * 104   6/02/98 11:12a Pkeet
 * Added the 'SetD3DFlagForPolygons' member function.
 * 
 * 103   98.05.21 11:31p Mmouni
 * Added validate function to CRenderPolygon.
 * 
 * 102   98/03/02 21:42 Speter
 * Combined SetAccept() into ReduceFeatures().
 * 
 * 101   98/02/26 13:52 Speter
 * Moved seterfFeatures to CTexture from SSurface, removing redundant flags.
 * 
 * 100   98/02/10 13:17 Speter
 * Changed original CSArray to CMSArray.
 * 
 * 99    98/02/04 14:44 Speter
 * Removed fGetRoughScreenArea().
 * 
 * 98    2/03/98 6:35p Mlange
 * Moved assertion failure avoidance to CRenderPolygon::InitFast().
 * 
 * 97    1/25/98 3:39p Pkeet
 * Added the 'SetAccept' and 'ReduceFeatures' member functions.
 * 
 * 96    1/23/98 5:40p Pkeet
 * Added the 'fArea' member variable and the 'SetArea' member function to the screen polygon
 * class.
 * 
 * 95    97/12/08 17:44 Speter
 * Made sole virtual function non-virtual, and added Assert for no vtbl (because of fast-init
 * polygons).
 * 
 * 94    1/23/96 4:37p Pkeet
 * Initializes the 'cvFace' data member to prevent later debug build asserts.
 * 
 * 93    1/23/96 2:00p Pkeet
 * Added an 'AlwaysAssert' in place of the pure virtual function 'DrawPolygon' so that an error
 * will return control to the debugger instead of crashing.
 * 
 * 92    97/11/19 18:10 Speter
 * Added CRenderPolygon::InitFast().
 * 
 * 91    11/09/97 10:07p Gfavor
 * Eliminated SetPlane3DX.
 * 
 * 90    97/11/08 4:48p Pkeet
 * Removed Z buffer parameters.
 * 
 **********************************************************************************************/

#ifndef LIB_RENDERER_SCREENRENDER_HPP
#define LIB_RENDERER_SCREENRENDER_HPP

#include "Texture.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/GeomDBase/Plane.hpp"
#include "Lib\Math\FastInverse.hpp"


//**********************************************************************************************
//
// Forward class declarations.
//
class CCamera;
class CSortList;
class CSortListNode;
class CShape;


//
// Constants.
//
extern float fMaxAreaCull;
extern float fMaxAreaCullTerrain;


//
// Enumerations.
//

// Enumeration of hardware rendering types.
enum EHWRenderType
{
	ehwSoftware,			// Non-hardware.
	ehwTerrain,				// Terrain.
	ehwCache,				// Cache hardware.
	ehwWater,				// Water.
	ehwAlphaCol,			// Alpha colour (e.g., alpha lights).
	ehwFill,				// Colour fill.
	ehwRegularOpaque,		// Regular opaque polygon.
	ehwRegularTransparent,	// Regular transparent polygon.
	ehwRegularAlpha,		// Regular alpha polygon.
	ehwFluid				// Vertex blended alpha.
};

// Enumeration of addressing modes for hardware.
enum EAddressMode
{
	eamTileNone,
	eamTileUV,
	eamTileU,
	eamTileV,
	eamTileDontCare
};


//**********************************************************************************************
//
class CSortKey
//
// Key used for depth sorting polygons.
//
// Prefix: key
//
//**************************************
{
public:

	uint32 u4SubKey;
	static uint32 u4UniqueSubKeyUp;
	static uint32 u4UniqueSubKeyDown;
	static uint32 u4UniqueSubKeyStep;

public:

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	static void Reset
	(
		uint32 u4_step  = 1 << 16,
		uint32 u4_start = 1 << 31
	)
	//
	// 
	//
	//**************************************
	{
		u4UniqueSubKeyStep = u4_step;
		u4UniqueSubKeyUp   = u4_start + u4_step;
		u4UniqueSubKeyDown = u4_start;
	}

	//******************************************************************************************
	//
	void SetKeyUp
	(
	)
	//
	// 
	//
	//**************************************
	{
		u4SubKey          = u4UniqueSubKeyUp;
		u4UniqueSubKeyUp += u4UniqueSubKeyStep;
	}

	//******************************************************************************************
	//
	void SetKeyDown
	(
	)
	//
	// 
	//
	//**************************************
	{
		u4SubKey            = u4UniqueSubKeyDown;
		u4UniqueSubKeyDown -= u4UniqueSubKeyStep;
	}

	//******************************************************************************************
	//
	bool bSetKeyMid
	(
		CSortKey key_0,
		CSortKey key_1
	)
	//
	// Sets the key value at the midpoint of the two provide key values, and returns 'true' if
	// the key is unique.
	//
	//**************************************
	{
		u4SubKey = (key_0.u4SubKey + key_1.u4SubKey) >> 1;
		return u4SubKey != key_0.u4SubKey && u4SubKey != key_1.u4SubKey;
	}

	//******************************************************************************************
	//
	void SetKeyMid
	(
		CSortKey key_0,
		CSortKey key_1
	)
	//
	// Sets the key value at the midpoint of the two provide key values, and returns 'true' if
	// the key is unique.
	//
	//**************************************
	{
		u4SubKey = (key_0.u4SubKey + key_1.u4SubKey) >> 1;
	}

	//**************************************************************************************
	//
	bool operator()(const CSortKey& key_0, const CSortKey& key_1) const
	//
	// Returns 'true' if the farthest (smallest) inverse Z value of the first polygon is
	// closer (larger) than the farthest inverse z value of the second polygon.
	//
	//**************************************
	{
		// Return the results of the comparision.
		return key_0.u4SubKey < key_1.u4SubKey;
	}

};


//**********************************************************************************************
//
struct SRenderCoord
//
// The coordinate portion of a vertex type internal to the rendering pipeline. 
//
// Prefix: rnc
//
// Notes:
//		Currently the structure keeps the camera space coordinates for depth sorting and
//		occlusion.
//
//**************************************
{
	union
	{
		struct
		{
			CVector3<>	v3Cam;			// The position in normalised camera space.
		};
		struct
		{
			int			iYScr;			// Truncated screen coordinate (used in rasteriser only).
		};
	};
	CVector3<>		v3Screen;			// Screen X, Y and 1/Z coords.
	SRenderCoord() {}
};

//**********************************************************************************************
//
struct SRenderVertex: SRenderCoord
//
// A vertex type internal to the rendering pipeline. It combines the information from various
// stages in one type for quick access and manipulation.
//
// Prefix: rv
//
// Notes:
//		Currently the structure keeps the camera space coordinates for depth sorting and
//		occlusion.
//
//**************************************
{
	union
	{
		TClutVal cvIntensity;		// Clut index value at this point.
		struct
		{
			ptr< CDir3<> > pdir3Normal;	// Optional pointer to the vertex normal.
		};
	};
	CTexCoord tcTex;						// Texture coords, in range [0,1].
	uint32    u4D3DFog;						// Direct3D fog value.

	//******************************************************************************************
	//
	// Constructors.
	//

	SRenderVertex()
	{
	}

	// Interpolation constructor.
	SRenderVertex::SRenderVertex
	(
		const SRenderVertex& rv_0,		// Two vertices to interpolate.
		const SRenderVertex& rv_1, 
		TReal r_t,						// Interpolation parameter.
		bool b_perspective = true		// Whether to interpolate the screen coords as projected.
	);
};

//**********************************************************************************************
//
// Definitions for CRenderPolygon.
//

// The maximum number of sort lists a polygon can belong to.
#define iMAX_SORT_LISTS (1)


//**********************************************************************************************
class CRenderPolygon
//
// Prefix: rpoly
//
// A general polygon in renderable format.  Used by CScreenRender below. 
// It can have an arbitrary number of SRenderVertexes, referenced by paprvPolyVertices.  
//
//**************************************
{
public:
	CPArray<SRenderVertex*> paprvPolyVertices;	// Points to a list of vertex pointers.
												// This list must be maintained elsewhere.
	CSet<ERenderFeature>	seterfFace;			// Rendering features enabled for this face.

	ptr_const<CTexture>		ptexTexture;		// The texture data.

	TClutVal				cvFace;				// Lighting value for entire face, if not shading.
	SBumpLighting			Bump;
	int						iFogBand;			// Fog band triangle falls in.
	int						iMipLevel;			// Mip level for the polygon.
	float					fArea;				// Area of the polygon.

	//
	// Depth sorting variables.
	//
	CPlane		  plPlane;						// Triangle's plane.
	TReal		  rPlaneTolerance;				// Tolerance for clipping to the plane.
	CVector2<int> v2ScreenMinInt;				// Minimum screen coordinates as an integer.
	CVector2<int> v2ScreenMaxInt;				// Maximum screen coordinates as an integer.
	float         fMinZ;						// Minimum Z value for the polygon.
	float         fMaxZ;						// Maximum Z value for the polygon.
	TReal         rMaxLevel;					// The maximum Z value for this point in the
												// z-sorted list.
												// moved in the depth sort list.
	CSortKey      keySort;						// Key value for depth sorting.
	void*         pvNodeItMain;					// Pointer to the node iterator.
	bool          bMovedInList;					// Flag to indicate if the polygon has been moved.
	bool          bAccept;						// Flag to accept or cull polygon.

	//
	// Hardware rasterizaion support.
	//
	bool          bFullHardware;				// Flag for using full hardware acceleration.
	bool          bPrerasterized;				// Indicates a polygon already rasterized in hardware.
	EHWRenderType ehwHardwareFlags;				// Flags for hardware rendering.
	EAddressMode  eamAddressMode;				// Address mode for filtering.

	//
	// Stuff used for the partitoning depht sort.
	//
#if (iMAX_SORT_LISTS > 1)
	int           iNumSortLists;				// Number of sort lists the polygon belongs to.
	CSortList*    pslMemberOf[iMAX_SORT_LISTS];	// Sort lists the polygon is a member of.
	void*         pvNodeIt[iMAX_SORT_LISTS];	// Pointer to the node iterator.
#endif

	//
	// Stuff that is helpfull for debugging.
	//
#if (VER_DEBUG)
	bool          bClippedByDepthSort;			// Flag to indicate polygon was clipped by the
												// depth sort routine.
	int           iClipped;						// The number of times this polygon has been
												// clipped in the depthsort algorithm.
	CShape*       pshOwningShape;				// Pointer to the shape that owns the polygon.
#endif

	//******************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CRenderPolygon();

	//******************************************************************************************
	CRenderPolygon& operator =(const CRenderPolygon& rpoly)
	{
		new(this)CRenderPolygon(rpoly);
		return *this;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void InitFast()
	//
	// Initialise the bare minimum necessary for this polygon.
	//
	// This can be used after creating an uninitialised polygon (i.e. one that hasn't had
	// its default constructor invoked).  The various stages of the pipeline are then responsible
	// for setting those values necessary for the particular rendering features enabled.
	//
	//**********************************
	{
#if VER_DEBUG
		// Set everything to a probably invalid value, so we can be sure that anything
		// needing initialising is explicitly initialised.
		memset(this, 0xFF, sizeof(*this));

		// Make sure we don't have a vtbl, by checking the address of the first member.
		Assert((char*)&paprvPolyVertices == (char*)this);

		// Avoid an assertion failure on the copy constructor. We must make sure we have
		// a valid (normalised) plane normal before the copy.
		plPlane = CPlane(CDir3<>(1, 0, 0), 1);
#endif

		// Prevent an undefined lighting value.
		cvFace = 0.0f;
		
		// This must be initialised, or the CDir3<> copy constructor will Assert.
		Bump.d3Light = d3ZAxis;

		// Software polygon by default.
		ehwHardwareFlags = ehwSoftware;
	}

	//******************************************************************************************
	//
	void SetPlane
	(
	);
	//
	// Sets the plane of the triangle and its tolerance.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bCalculateScreenExtents
	(
		bool b_cull_tiny = true			// Whether to reject tiny polygons.
	);
	//
	// Initializes the screen space bounding box, and related members.
	//
	// Returns 'true' if the polygon spans at least a single screen pixel.
	//
	//**********************************

	//******************************************************************************************
	//
	bool bScreenNoOverlap
	(
		CRenderPolygon* prpoly	// Pointer to the polygon that should lay to the front.
	) const
	//
	// Returns 'true' if the screen space bounding rectangles do not overlap, or if this
	// polygon lay entirely behind the polygon given by the argument.
	//
	// Notes:
	//		This test incorporates the first two ambiguity resolution steps for depth sorting.
	//
	//**********************************
	{
		Assert(prpoly);
		Assert(fMinZ >= 0.0f);
		Assert(prpoly->fMaxZ >= 0.0f);

		// X Overlap.
		if (v2ScreenMinInt.tX >= prpoly->v2ScreenMaxInt.tX)
			return true;
		if (v2ScreenMaxInt.tX <= prpoly->v2ScreenMinInt.tX)
			return true;

		// Y Overlap.
		if (v2ScreenMinInt.tY >= prpoly->v2ScreenMaxInt.tY)
			return true;
		if (v2ScreenMaxInt.tY <= prpoly->v2ScreenMinInt.tY)
			return true;

		// Z Overlap.
		if (u4FromFloat(fMinZ) >= u4FromFloat(prpoly->fMaxZ))
			return true;

		// All attempts at trivial rejection failed, return 'true.'
		return false;
	}

	//******************************************************************************************
	//
	bool bBehind
	(
		const CRenderPolygon* prpoly	// Pointer to the polygon that should lay to the front.
	) const;
	//
	// Returns 'true' if this polygon is behind the polygon referred to by the argument. This
	// test assumes that a trivial Z extent test has been done, and only uses the planes of
	// the polygons to determine order.
	//
	// Notes:
	//		This test incorporates the third and fourth ambiguity resolution steps for depth
	//		sorting. As a slight optimization, the fourth test is done before the third.
	//
	//**********************************

	//******************************************************************************************
	//
	bool bInFrontOf
	(
		const CRenderPolygon* ppoly		// Pointer to the polygon that should lay to the back.
	) const;
	//
	// Returns 'true' if this polygon is entirely in front of the polygon referred to by the
	// argument.
	//
	// Notes:
	//		This test represents the reversed step three and four test for ambiguity resolution
	//		for depth sorting.
	//
	//**********************************

	//******************************************************************************************
	//
	void SetArea
	(
	);
	//
	// Sets the area of the polygon.
	//
	//**************************************

	//******************************************************************************************
	//
	void ReduceFeatures
	(
	);
	//
	// Turns features off wherever possible.  Also sets bAccept and fArea.
	//
	//**************************************

	//**********************************************************************************************
	//
	void FindBestSlope
	(
		int& ri_best_vert_a,			// Index of first vertex.
		int& ri_best_vert_b				// Index of second index.
	) const;
	//
	// Finds the pair of vertices that subdivides the polygon with the least horizontal
	// slope.
	//
	//**********************************

	//**********************************************************************************************
	//
	void BuildVertexArray
	(
		CRenderPolygon& rpoly_new,				// Polygon to fill with vertices.
		int i_from,								// Index of first vertex.
		int i_to,								// Index of second index.
		CPArray<SRenderVertex*> paprv_scratch	// Array usable for rpoly's vertices.
	) const;
	
	//******************************************************************************************
	//
	float fGetAverageZ
	(
	) const;
	//
	// Returns the average z value for the polygon.
	//
	//**********************************
	
	//******************************************************************************************
	//
	float fGetFarthestZ
	(
	) const;
	//
	// Returns the average z value for the polygon.
	//
	//**********************************
	
	//******************************************************************************************
	//
	void SetMipLevel
	(
		bool b_target_hardware
	);
	//
	// Determines the mip level for this polygon by setting 'iMipLevel.'
	//
	//**************************************

	//******************************************************************************************
	//
	void Validate
	(
	);
	//
	// Make sure that this polygon is ready to render.
	//
	//**************************************

	//******************************************************************************************
	//
	void Grow
	(
		float f_amount	// Amount to grow outward by.
	);
	//
	// Grows the polygon outward by a specified amount.
	//
	//**************************************
};


//******************************************************************************************
//
// Inlined for speed.
//

	//******************************************************************************************
	forceinline float CRenderPolygon::fGetAverageZ() const
	{
		float f_average_z = 0.0f;

		// Sum the z values of the vertices.
		for (int i = 0; i < paprvPolyVertices.size(); ++i)
			f_average_z += paprvPolyVertices[i]->v3Cam.tY;

		// Return the average value.
		return f_average_z * fUnsignedInverseInt(paprvPolyVertices.size());
	}

	//******************************************************************************************
	forceinline void CRenderPolygon::SetArea()
	{
		Assert(paprvPolyVertices.uLen >= 3);
		fArea = 0.0f;

		//
		// Break the polygon into triangles and sum their area.
		//
		for (uint u_3 = 2; u_3 < paprvPolyVertices.uLen; ++u_3)
		{
			uint u_1 = 0;
			uint u_2 = u_3 - 1;

			//
			// Formula:
			//
			//		Area = (x2 - x1) * (x3 - x1) - (y3 - y1) * (y2 - y1)
			//             ---------------------------------------------
			//                                   2
			//
			// Note the reverse order of the screen vertices. Also note that the CVector2D cross
			// product could be used here.
			//
			float f_sub_area = (paprvPolyVertices[u_2]->v3Screen.tX - paprvPolyVertices[u_3]->v3Screen.tX) *
				               (paprvPolyVertices[u_1]->v3Screen.tY - paprvPolyVertices[u_3]->v3Screen.tY) -
					           (paprvPolyVertices[u_1]->v3Screen.tX - paprvPolyVertices[u_3]->v3Screen.tX) *
							   (paprvPolyVertices[u_2]->v3Screen.tY - paprvPolyVertices[u_3]->v3Screen.tY);

			// Sum the areas.
			fArea += fabs(f_sub_area);
		}

		// Divide by two.
		fArea *= 0.5f;
	}


//**********************************************************************************************
//
class CScreenRender
//
// Prefix: scren
//
// An interface class for the rasteriser.
// Implemented by a derived class, e.g. our native rasteriser, or Direct3D.
//
//**************************************
{
public:

	//**********************************************************************************************
	//
	struct SSettings
	//
	// Prefix: screnset
	//
	// Screen renderer settings.
	//
	//**************************************
	{
		// Publicly changeable settings.
		CSet<ERenderFeature>	seterfState;		// Which features currently active.
		bool					bClearBackground;	// Whether to clear the background in BeginFrame.
		bool					bDrawSky;			// The renderer has the sky
		CColour					clrBackground;		// The background colour to clear to.
													// Also used as sky colour for water rendering.
		bool					bTestGamma;			// Flag to draw gamma test pattern on background.
		bool					bHalfScanlines;		// Flag to render only the even scanlines.
		bool					bDoubleVertical;	// Flag to double the pixels vertically.
		bool					bSoftwareAlpha;		// Flag to do software alpha blending.

		// Scene info.
		SBumpLighting			bltPrimary;			// Bump lighting info for primary lighting.

		// Supply default constructor only.

		//******************************************************************************************
		SSettings();
	};

	CRaster*					prasScreen;			// Target raster associated with this renderer.
	ptr<SSettings>				pSettings;			// Render settings used by this renderer
													// (changeable).

public:

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	//******************************************************************************************
	CScreenRender
	(
		SSettings*			pscrenset,				// Render settings to use.
		rptr<CRaster>		pras_screen,			// Raster for render output.
		rptr<CRaster>		pras_zbuffer = rptr0	// Raster for z-buffer output.
	);
	//
	// Constructs an object that renders to the given screen.  If pras_zbuffer is 0,
	// an appropriate Z-buffer is constructed internally.
	//
	// Note that some implementations of this class may have restrictions such as not allowing
	// a Z buffer to be passed, or requiring pras_screen to be a CRasterWin.
	//

	//******************************************************************************************
	virtual ~CScreenRender()
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	virtual void UpdateSettings()
	//
	// Handles any changes to the settings.
	//
	{
	}

	//******************************************************************************************
	virtual CSet<ERenderFeature> seterfModify()
	//
	// Returns:
	//		Which features are changeable in this driver.
	//
	{
		return set0;
	}

	//******************************************************************************************
	virtual void BeginFrame()
	//
	// Perform frame setup activities before rendering.
	//
	{
	}

	//******************************************************************************************
	//
	virtual bool bTargetHardware
	(
	) const
	//
	// Returns 'true' if the target renderer supports Direct3D.
	//
	//**************************************
	{
		return false;
	}

	//******************************************************************************************
	//
	virtual bool bTargetMainScreen
	(
	) const
	//
	// Returns 'true' if the target renderer is the main screen.
	//
	//**************************************
	{
		return false;
	}

	//******************************************************************************************
	virtual void EndFrame()
	//
	// Perform frame cleanup activities after rendering, including flipping the screen.
	//
	{
	}

	//******************************************************************************************
	virtual void ClearMemSurfaces()
	//
	// Clears the raster memory.
	//
	{
	}

	//******************************************************************************************
	virtual void DrawPolygons
	(
		CPArray<CRenderPolygon*> parpoly	// Array of polygons to draw.
	);
	//
	// Render these here polys.
	// Default version calls DrawPolygon for each one.
	//

	//******************************************************************************************
	//
	virtual void SetD3DFlagForPolygons
	(
		CPArray<CRenderPolygon> parpoly,	// Array of polygons to draw.
		bool b_d3d
	);
	//
	// Sets software and hardware flag as required.
	//
	//**************************************

	//******************************************************************************************
	virtual void DrawPolygon
	(
		CRenderPolygon& rpoly				// Polygon to render.
	)
	//
	// Render this here poly.
	// // Disabled default version splits into triangles, calls DrawTriangle.
	//
	{
		AlwaysAssert(0);
	}

	//******************************************************************************************
	//
	virtual CScreenRender* psrCreateCompatible
	(
		rptr<CRaster> pras_screen	// Raster for render output.
	)
	//
	// Calls appropriate CScreenRender() constructor.
	//
	//**********************************
	{
		return 0;
	}

	//******************************************************************************************
	//
	virtual TPixel pixSetBackground
	(
		TPixel pix_background	// The new background colour.
	);
	//
	// Sets the background colour.
	//
	// Returns the value of the current background colour.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual void SetHardwareOut
	(
		bool b_allow_hardware
	);
	//
	// Sets the hardware out flag for this device.
	//
	//**********************************


protected:

	//******************************************************************************************
	virtual CSet<ERenderFeature> seterfDefault()
	//
	// Returns:
	//		Which features are changeable in this driver.
	//
	{
		return set0;
	}

	//******************************************************************************************
	void CorrectRenderState
	(
		CSet<ERenderFeature>&	seterf				// Input render state.
	);
	//
	// Corrects the render state, enforcing internal consistency, and setting any unmodifiable
	// flags to their default state.
	//

	//**********************************************************************************************
	//
	virtual void ValidatePolygon
	(
		const CRenderPolygon& rpoly		// Polygon to check.
	);
	//
	// Makes sure it's kosher.
	//
	//**********************************

	//**********************************************************************************************
	//
	virtual void ValidateVertex
	(
		const SRenderVertex& rv			// Vertex to check.
	);
	//
	// Makes sure it's kosher.
	//
	//**********************************

};

//**********************************************************************************************
//
class CRenderDesc
//
// Prefix: srendesc
//
// Used to describe a particular ScreenRenderer to the main program.  Contains a name for the
// renderer, and a function to create it.
//
//**************************************
{
public:
	char*	strDesc;					// Description of the renderer.

	//******************************************************************************************
	//
	// Constructor.
	//

	CRenderDesc(char* str)
		: strDesc(str)
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual CScreenRender* pScreenRenderCreate
	(
		CScreenRender::SSettings* pscrenset,	// Render settings to use.
		rptr<CRaster>		      pras_screen	// Raster for render output.
	) = 0;
	//
	// Calls appropriate CScreenRender() constructor.
	//
	//**********************************
};

//**********************************************************************************************
//
class CArrayRenderDesc: public CMSArray<CRenderDesc*, 5>
//
// Prefix: sapRenderDesc (like parent)
//
// A class which is basically just an array of RenderDesc*, with special code to initialise it.
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructor.
	//

	// When initialised, it fills itself with all known CRenderDescs.
	CArrayRenderDesc();
};


//******************************************************************************************
//
// Global variables.
//

// The app looks here to find the list of CRenderDescs, and create CScreenRenderers.
extern CArrayRenderDesc		sapRenderDesc;

#endif
