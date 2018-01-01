/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Direct3D implementation of ScreenRender.hpp
 *
 * Bugs:
 *		In ramp model, the specular colour shows up much too large...if the material has a
 *		specular component, then there's white when the diffuse lighting is large, even if
 *		there's no specular lighting.
 *
 * To do:
 *		DirectDraw driver must be selected w/regard to 3D caps.
 *		Main screen must be created under driver control.
 *		If driver can only work in video mem, we need to indicate that.
 *		Our rasteriser must be able to take a predeclared Z buffer of arbitrary depth.
 *		Figure out Z buffer format, figure out whether we can access surfaces on cards.
 *
 *		Experiment with D3D doing: Transform, Lighting, Clipping, Rasterising.
 *		Figure out how big to make ex buffer, how to manage it.
 *		Incorporate render options into trianges.
 *		Figure out how to change state optimally.
 *		
 *		Store specular shading correctly; fog also; store full RGB shading if supported.
 *		Determine proper culling order for renderer.
 *		Determine *all* driver caps, and integrate them with render options.
 *		Should alpha values be set???  Where is alpha turned on or off?
 *		Incorporate max execute buffer size/vertex count.
 *
 *		Change CD3DVidTextureList to a map or priority queue.
 *		Make D3DMaterials incorporate solid colours when solid rendering.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/D3d/ScreenRenderD3D.cpp                                  $
 * 
 * 28    97/06/23 20:31 Speter
 * Made gcfScreen a static member of CScreenRender.
 * 
 * 27    97-04-04 12:38 Speter
 * A big change: now pipeline uses CRenderPolygon rather than SRenderTriangle (currently always
 * as a triangle).  Changed associated variable names and comments.
 * 
 * 26    97-04-02 12:48 Speter
 * Updated for new render settings.
 * 
 * 25    97/02/21 15:21 Speter
 * Removed fZ from SRenderVertex.  Removed f_zadjust param from DrawTriangles().  Moved
 * prasScreen to CScreenRender from derived classes.
 * 
 * 24    97/01/20 11:50 Speter
 * Moved pixBackground and gcfScreen vars from CScreenRender to derived classes.  Added erfCOPY
 * to default settings.
 * 
 * 23    97/01/16 11:53 Speter
 * Many changes for new CScreenRender class.  Replaced seterfModify and seterfDefault values
 * with constant values and access functions.  
 * 
 * 22    97/01/10 17:59 Speter
 * Moved prasZBuffer from CScreenRender base class to derived classes that need it.
 * 
 * 21    1/08/97 7:45p Pkeet
 * Replaced the 'NonConst' macro. Used a reference cast in the new operator for 'D3DExBuf' to
 * prevent a compiler error under Visual C++ 5.0.
 * 
 * 20    97/01/07 11:17 Speter
 * Changed to new rptr_new macro, and new rptr_dynamic_cast functionality.
 * 
 * 
 * 19    96/12/31 16:58 Speter
 * Updated for rptr.
 * 
 * 18    96/12/06 15:16 Speter
 * BeginFrame() now clears background.
 * 
 * 17    96/12/02 17:50 Speter
 * Changed ValidateRenderState() call to CheckRenderState().
 * 
 * 16    96/11/27 19:32 Speter
 * Updated for change to CCom: ptCom member changed to ptPtr.
 * 
 * 15    11/19/96 7:02p Pkeet
 * Added the 'fZAdjust' parameter.
 * 
 * 14    96/10/22 11:12 Speter
 * Added include for <list.h>, necessary due to new Entity includes.
 * 
 * 13    96/10/14 15:41 Speter
 * Updated for change in CRaster.  ppalAttached is now in pxf.
 * 
 * 12    96/10/04 18:00 Speter
 * Removed CSet<ERenderFeature> param from DrawTriangles.
 * 
 * 11    96/09/27 11:31 Speter
 * Oops.  Changed <float> in geometry types to <>.
 * 
 * 10    96/09/23 17:09 Speter
 * Updated for replacement of two D3DDEVICEDESCs with one.
 * No longer lock raster surfaces in BeginFrame and EndFrame.
 * 
 * 9     96/09/12 16:26 Speter
 * Incorporated new TReflectVal usage.
 * 
 * 8     96/09/09 18:10 Speter
 * Made specular hilites work (at least for RGB model).
 * Added call to ValidateRenderState().
 * 
 * 7     96/09/05 11:51 Speter
 * Implemented seterfState and seterfModify handling.
 * Added checks to allow it to work w/o Z buffer.
 * Fixed bug by setting v2Scale to unity when no texture in VidTexture.
 * First pass at specular support.
 * 
 * 6     96/08/20 14:34 Speter
 * Created CD3DVidTexture class, and associated classes.  Now manage textures in a rudimentary
 * texture list.
 * 
 * 5     96/08/19 16:05 Speter
 * Now checks D3D.bValid before adding any drivers to the list.
 * 
 * 4     96/08/19 13:08 Speter
 * Added interface for CRenderDesc.
 * Changed prasScreen to prasvScreen.
 * Added prasvZBuffer member.
 * Added destructor, cleaning up objects.
 * Moved to do's etc. up to top of file.
 * Removed dummy code.
 * 
 * 3     96/08/15 18:39 Speter
 * Got textures working.
 * Moved Lock() and Unlock() into Begin/EndFrame().
 * Added more comments/questions.
 * 
 * 2     96/08/14 11:35 Speter
 * Added much functionality, now draws untextured polys.
 * Changed prefixes, added classes and functions, added many comments.
 * 
 * 1     96/08/09 11:02 Speter
 * New files which implement CScreenRender for D3D.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "D3D.hpp"

#include "Lib/View/RasterVid.hpp"
#include "../ScreenRender.hpp"

#include <list.h>

/*
	Class name:		CD3DDevice				int				void

	Current:
		Prefix:		d3ddev					i				
		Global:		d3ddevMain				iCount			Func()
		Class:		d3ddevDev				iCount			Func()
		Local:		d3ddev_cur				i_count
					d3ddev					i
	New:
		Prefix:		d3d_dev					i
		Global:		D3DDevMain				ICount			Func()
		Class:		D3DDev_Dev				I_Count			_Func()
					D3DDev					I
		Local:		d3d_dev_cur				i_count
					d3d_dev					i
*/

/*
class CD3DDevice: public CCom<IDirect3DDevice>
{
public:
	CD3DDevice()
	{
	}

	CD3DDevice(CDDSurface* pDDSurface, GUID* pGUID)
	{
		Init(pDDSurface, pGUID);
	}

	void Init(CDDSurface* pDDSurface, GUID* pGUID)
	{
		D3D.err = pDDSurface->QueryInterface(pGUID, (void**)&ptPtr);
		Assert(ptPtr);
	}
};

class CD3DViewport: public CCom<IDirect3DViewport>
{
public:
	CD3DViewport()
	{
		D3D.err = D3D_Main->CreateViewport(&ptPtr, 0);
		Assert(ptPtr);
	}
};

class CD3DTexture: public CCom<IDirect3DTexture>
{
public:
	CD3DTexture()
		: d3dhTex(0)
	{
	}

	CD3DTexture(CRasterVid& rasv)
	{
		Init(rasv);
	}

	void Init(CRasterVid& rasv)
	{
		// Initialise the COM object from the DD Surface.
		D3D.err = rasv.pddsDraw->QueryInterface(IID_IDirect3DTexture, (void**)&ptPtr);
	}
};
*/

void Convert
(
	D3DTLVERTEX* pd3d_tlvert, 
	const SRenderVertex& rv,
	CVector2<> v2_scale				// Texture coord scaling params.
)
{
	Assert(bWithin(rv.v3Screen.tZ, 0.0, 1.0));
	pd3d_tlvert->dvSX			= rv.v3Screen.tX;
	pd3d_tlvert->dvSY			= rv.v3Screen.tY;
	pd3d_tlvert->dvSZ			= 1 / rv.v3Screen.tZ;
	pd3d_tlvert->dvRHW			= rv.v3Screen.tZ;

	// Set the colour values.  Since we have only intensity, we set grey colours.
	// In D3D ramp mode, the blue component indicates intensity.
	int i_shade, i_hilite;

	if (rv.rvIntensity <= rvMAX_COLOURED)
	{
		i_shade = iFastPosFloatToInt(rv.rvIntensity / rvMAX_COLOURED * 255.9f);
		i_hilite = 0;
	}
	else
	{
		i_shade = 255;
		i_hilite = iFastPosFloatToInt((rv.rvIntensity - rvMAX_COLOURED) / (1 - rvMAX_COLOURED) * 255.9f);
	}

	pd3d_tlvert->dcColor		= RGBA_MAKE(i_shade, i_shade, i_shade, 0);
	pd3d_tlvert->dcSpecular		= RGBA_MAKE(i_hilite, i_hilite, i_hilite, 0);

	// Texture coordinates convert directly, as they are in [0,1] float range.
	pd3d_tlvert->dvTU			= rv.tcTex.tX * v2_scale.tX;
	pd3d_tlvert->dvTV			= rv.tcTex.tY * v2_scale.tY;

	Assert(bWithin(pd3d_tlvert->dvTU, 0.0f, 1.0f));
	Assert(bWithin(pd3d_tlvert->dvTV, 0.0f, 1.0f));
}

//**********************************************************************************************
//
class CD3DVidTexture
//
// Prefix: d3dvtex
//
// Contains all structures needed by D3D to render textures.
// Also works with textureless (solid) materials.
//
//**************************************
{
public:
	rptr<CRasterVid>		prasvSource;			// Source texture raster.
	CCom<IDirect3DTexture>	d3dTextureSource;		// D3D Texture interface for it.
	rptr<CRasterVid>		prasvVid;				// Video memory texture raster.
	CCom<IDirect3DTexture>	d3dTextureVid;			// Texture interface for it.
	D3DTEXTUREHANDLE		d3dhTexture;			// Handle for video texture.
	CD3DMaterial			d3dmatTexture;			// The material associated with the texture.
	bool					bLoaded;				// Whether it's currently loaded.

	CVector2<>				v2Scale;				// Scaling required for texture coordinates,
													// due to possible difference in raster and 
													// surface sizes.

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	CD3DVidTexture(rptr<CRasterVid> prasv_source, CCom<IDirect3DDevice> d3ddev)
	{
		prasvSource = prasv_source;

		if (prasvSource)
		{
			// We have a texture raster.

			Assert(prasvSource->pddsDraw);

			// Create the source texture interface.
			D3D.err = prasvSource->pddsDraw->QueryInterface(IID_IDirect3DTexture, (void**)&d3dTextureSource);

			// Create the video texture.
			prasvVid = rptr_new CRasterVid(prasvSource->iWidth, prasvSource->iHeight, prasvSource->iPixelBits, 
				Set(erasTexture) + erasVideoMem);

			// Create an empty palette for the raster.
			prasvVid->AttachPalette(prasvSource->pxf.ppalAttached);

			// Create the video texture interface.
			D3D.err = prasvVid->pddsDraw->QueryInterface(IID_IDirect3DTexture, (void**)&d3dTextureVid);

			// Retrieve the video texture handle.
			D3D.err = d3dTextureVid->GetHandle(d3ddev, &d3dhTexture);

			//
			// Set the scaling values.
			//
			CDDSize<DDSURFACEDESC>	ddsd;

			D3D.err = prasvVid->pddsDraw->GetSurfaceDesc(&ddsd);			
			v2Scale.tX = (float) prasvSource->iWidth  / ddsd.dwWidth;
			v2Scale.tY = (float) prasvSource->iHeight / ddsd.dwHeight;

			bLoaded = false;
		}
		else
		{
			// Set null structure values.
			d3dhTexture = 0;
			bLoaded = true;
			v2Scale.tX = v2Scale.tY = 1.0f;
		}
			
		// Create the material.
		CDDSize<D3DMATERIAL>	d3dmat_desc;

		//
		// D3D always modulates vertex colours with material colour.
		// Therefore, since we've already calculated the *real* colour the vertex should be,
		// we need a material with a white diffuse and specular colour.
		//
		d3dmat_desc.dwRampSize		= 32;
		d3dmat_desc.dcvDiffuse.dvR	=
		d3dmat_desc.dcvDiffuse.dvG	=
		d3dmat_desc.dcvDiffuse.dvB	= (D3DVALUE)1;
		d3dmat_desc.dcvSpecular		= d3dmat_desc.dcvDiffuse;
		d3dmat_desc.hTexture		= d3dhTexture;

		d3dmatTexture.Init(d3dmat_desc, d3ddev);
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	bool bLoad()
	{
		if (!bLoaded)
			//
			// Load the source texture into the dest.
			// You would think that the return value would indicate a specific error condition,
			// such as being out of video memory, as in other DirectX return codes, but no.  
			// In Direct3D, return values are things like D3DERR_TEXTURE_LOAD_FAILED, which is 
			// useless.  So we must simply return a sucess code from here, and assume that failure
			// is due to lack of video memory.
			//
			bLoaded = d3dTextureVid->Load(d3dTextureSource) == 0;
		return bLoaded;
	}

	bool bUnload()
	{
		if (!bLoaded || !d3dTextureVid)
			return false;
		D3D.err = d3dTextureVid->Unload();
		bLoaded = false;
		return true;
	}
};

//**********************************************************************************************
//
class CD3DVidTextureList: public list<CD3DVidTexture*>
//
// lsd3dvtex
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Destructor.
	//

	~CD3DVidTextureList()
	{
		// Delete all our VidTextures.
		for (iterator it = begin(); it != end(); it++)
			delete (*it);
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	CD3DVidTexture* pd3dvtexLoad(rptr<CRasterVid> prasv_source, CCom<IDirect3DDevice> d3ddev)
	{
		CD3DVidTexture*	pd3dvtex = 0;

		// Search through the list a texture already using prasv_source.
		for (iterator it = begin(); it != end(); it++)
		{
			if ((*it)->prasvSource == prasv_source)
			{
				pd3dvtex = *it;
				break;
			}
		}

		if (!pd3dvtex)
		{
			// Must add a vid texture to the list.
			pd3dvtex = new CD3DVidTexture(prasv_source, d3ddev);
			push_back(pd3dvtex);
		}

		// Now attempt to Load it.
		while (!pd3dvtex->bLoad())
		{
			// We must unload something from the front of the list.
			for (iterator it = begin(); it != end(); it++)
			{
				if ((*it)->bUnload())
					break;
			}
			if (it == end())
				// We reached end of list w/o unloading anything.
				// I guess we can't load this texture.
				return pd3dvtex;
		}

		return pd3dvtex;
	}
};

	

//**********************************************************************************************
//
class CScreenRenderD3D: public CScreenRender
//
// Prefix: scrend3d
//
// Implementation of CScreenRender interface for Direct3D.
//
//**************************************
{
private:
	rptr<CRasterVid>		prasvScreen;	// CRasterVid version of the main screen.
	rptr<CRasterVid>		prasvZBuffer;	// CRasterVid version of the z-buffer.
	CCom<IDirect3DDevice>	D3DDevice;		// The attached D3D device (driver).
	CDirect3D::SDeviceDesc*	pDeviceDesc;	// The description of the device.
	CCom<IDirect3DViewport>	D3DViewport;	// The D3D viewport.
	CD3DExBuf				D3DExBuf;		// The execute buffer.
	CD3DVidTextureList		D3DVidTextureList;		// The list of textures loaded.

	CSet<ERenderFeature>	seterf_Modify;	// Which flags are modifiable.
	CSet<ERenderFeature>	seterf_Default;	// Which flags are on by default.
	CSet<ERenderFeature>	seterfLast;		// The last render state encountered.

	// Settings derived from pSettings.
	TPixel					pixBackground;		// The pixel corresponding to clrBackground.

public:

	//******************************************************************************************
	CScreenRenderD3D(SSettings* pscrenset, rptr<CRasterVid> prasv_screen, uint u_driver)
		: CScreenRender(pscrenset, rptr_cast(CRaster, prasv_screen))
	{
		prasvScreen = prasv_screen;

		pDeviceDesc = &D3D.sadevdList[u_driver];

		// Create a D3DDevice attached to the surface.
		D3D.err = prasvScreen->pddsDraw->QueryInterface
		(
			pDeviceDesc->Guid,				// The device ID.
			(void**)&D3DDevice				// Put the device here.
		);
		Assert(D3DDevice);

		// Create the Z buffer if supported.

		uint u_depths = pDeviceDesc->d3ddevcap.dwDeviceZBufferBitDepth;
		if (u_depths)
		{
			// Find the Z buffer bit depth from this driver's D3D device description
			int i_depth;

			if (u_depths & DDBD_32)
				i_depth = 32;
			else if (u_depths & DDBD_24)
				i_depth = 24;
			else if (u_depths & DDBD_16)
				i_depth = 16;
			else if (u_depths & DDBD_8)
				i_depth = 8;
			else
				Assert(false);

			prasvZBuffer = rptr_new CRasterVid
			(
				prasvScreen->iWidth, prasvScreen->iHeight,	// Same dimensions as screen.
				i_depth,									// The required depth.
				Set(erasZBuffer) +							// Set the Z buffer flag.
					// For hardware drivers, the Z-Buffer MUST be in video memory.  
					// (Otherwise, it MUST be in system memory).
					Set(erasVideoMem) * pDeviceDesc->bIsHW
			);

			// Attach the Z-buffer to the draw buffer so D3D will find it.
			D3D.err = prasvScreen->pddsDraw->AddAttachedSurface(prasvZBuffer->pddsDraw);
		}

		// Create the viewport.
		D3D.err = D3D->CreateViewport(&D3DViewport, 0);
		Assert(D3DViewport);

		// Attach it to the device.
		D3D.err = D3DDevice->AddViewport(D3DViewport);

		// Initialise it.
		CDDSize<D3DVIEWPORT> d3dvpdesc;
		d3dvpdesc.dwWidth  = prasvScreen->iWidth;
		d3dvpdesc.dwHeight = prasvScreen->iHeight;

		// Scaling does not apply to TLVERTICES, but set them to 1.0 just in case.
		d3dvpdesc.dvScaleX	= 1.0f;
		d3dvpdesc.dvScaleY	= 1.0f;
		d3dvpdesc.dvMaxX	= (D3DVALUE)(d3dvpdesc.dwWidth - 1);
		d3dvpdesc.dvMaxY	= (D3DVALUE)(d3dvpdesc.dwHeight - 1);

		D3D.err = D3DViewport->SetViewport(&d3dvpdesc);

/*
		// Create the background material for the viewport.
		CCom<IDirect3DMaterial>	d3d_material_bg;
		CDDSize<D3DMATERIAL>	d3d_material_desc;
		D3DMATERIALHANDLE		d3dhmat;

		// Set ramp size to 1, and all colours 0.  This makes everything black.
		d3d_material_desc.dwRampSize = 1;

		D3D.err = D3D->CreateMaterial(&d3d_material_bg, 0);
		D3D.err = d3d_material_bg->SetMaterial(&d3d_material_desc);
		D3D.err = d3d_material_bg->GetHandle(D3DDevice, &d3dhmat);
		D3D.err = D3DViewport->SetBackground(d3dhmat);
*/
		// Allocate the execute buffer.  Make up some size for now.
		D3DExBuf.Allocate(D3DDevice, 32*1024);

		// Set seterfState to default.
		seterf_Default[erfRASTER_CLIP]		= 1;

		// If culling is supported, assume it's on by default.
		seterf_Default[erfRASTER_CULL]		= 
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwMiscCaps & 
			(D3DPMISCCAPS_CULLCCW | D3DPMISCCAPS_CULLCW);

		seterf_Default[erfZ_BUFFER]
				   [erfZ_WRITE]
				   [erfZ_TEST]					= prasvZBuffer != 0;

		seterf_Default[erfCOPY]				= 0;
		seterf_Default[erfLIGHT]			= 1;
		seterf_Default[erfLIGHT_SHADE]		= 1;
		seterf_Default[erfFOG]				= 0;
		seterf_Default[erfFOG_SHADE]		= 0;
		seterf_Default[erfCOLOURED_LIGHTS]	= pDeviceDesc->d3ddevcap.dcmColorModel == D3DCOLOR_RGB;

		seterf_Default[erfTEXTURE]			= 1;
		seterf_Default[erfTRANSPARENT]		= 0;
		seterf_Default[erfBUMP]				= 0;

		seterf_Default[erfSUBPIXEL]			= 0;
		seterf_Default[erfPERSPECTIVE]		= 0;
		seterf_Default[erfDITHER]			= 0;
		seterf_Default[erfFILTER]			= 0;
		seterf_Default[erfFILTER_EDGES]		= 0;

		// Set seterf_Modify based on caps fields.
		seterf_Modify[erfRASTER_CLIP]		= 			
			pDeviceDesc->d3ddevcap.dtcTransformCaps.dwCaps & D3DTRANSFORMCAPS_CLIP;
//			&& pDeviceDesc->d3ddevcap.bClipping;

		// Set cull modifiable if both CULLNONE and another culling option are supported.
		seterf_Modify[erfRASTER_CULL]		= 
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwMiscCaps & 
			(D3DPMISCCAPS_CULLCCW | D3DPMISCCAPS_CULLCW)	&&
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwMiscCaps & 
			D3DPMISCCAPS_CULLNONE;

		Assert(pDeviceDesc->d3ddevcap.dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_LESSEQUAL);

		seterf_Modify[erfZ_BUFFER]			= prasvZBuffer != 0;
		seterf_Modify[erfZ_WRITE]			=
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwMiscCaps & D3DPMISCCAPS_MASKZ;
		seterf_Modify[erfZ_TEST]				=
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZTEST;

		seterf_Modify[erfCOPY]				= 
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwTextureBlendCaps &
				D3DPTBLENDCAPS_COPY;
		seterf_Modify[erfLIGHT]				= 
			(pDeviceDesc->d3ddevcap.dpcTriCaps.dwTextureBlendCaps & D3DPTBLENDCAPS_MODULATE) &&
			(pDeviceDesc->d3ddevcap.dpcTriCaps.dwShadeCaps &
				(D3DPSHADECAPS_COLORFLATMONO | D3DPSHADECAPS_COLORFLATRGB));
		seterf_Modify[erfLIGHT_SHADE]		= 
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwShadeCaps &
				(D3DPSHADECAPS_COLORGOURAUDMONO | D3DPSHADECAPS_COLORGOURAUDRGB);

		seterf_Modify[erfFOG]				=
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_FOGFLAT;
		seterf_Modify[erfFOG_SHADE]			=
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_FOGGOURAUD;
		seterf_Modify[erfCOLOURED_LIGHTS]	= 
			(pDeviceDesc->d3ddevcap.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDMONO) &&
			(pDeviceDesc->d3ddevcap.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB);
		seterf_Modify[erfSPECULAR]			=
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwShadeCaps &
				(D3DPSHADECAPS_SPECULARGOURAUDMONO | D3DPSHADECAPS_SPECULARGOURAUDRGB);

		seterf_Modify[erfTEXTURE]			=
			pDeviceDesc->d3ddevcap.dwDevCaps & 
				(D3DDEVCAPS_TEXTURESYSTEMMEMORY | D3DDEVCAPS_TEXTUREVIDEOMEMORY);
		seterf_Modify[erfTRANSPARENT]		=
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_TRANSPARENCY;

		seterf_Modify[erfBUMP]				= 0;

		seterf_Modify[erfSUBPIXEL]			= 
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_SUBPIXEL;
		seterf_Modify[erfPERSPECTIVE]		= 
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE;
		seterf_Modify[erfDITHER]				=
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_DITHER;
		seterf_Modify[erfFILTER]				= 
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEAR;
		seterf_Modify[erfFILTER_EDGES]		= 
			pDeviceDesc->d3ddevcap.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_BORDER;

	}

	//******************************************************************************************
	~CScreenRenderD3D()
	{
		// Remove the attached Z-buffer.
		if (prasvZBuffer)
		{
			D3D.err = prasvScreen->pddsDraw->DeleteAttachedSurface(0, prasvZBuffer->pddsDraw);
		}
	}

private:

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	void FlushBuffer(uint u_vertices = 0)
	{
		if (D3DExBuf.uLen == 0)
			// Empty.
			return;

		// Flush the execute buffer commands to the device
		// (because we're done with the frame, or it's full).
		D3DExBuf.Finish(u_vertices);
		D3DExBuf.Execute(D3DViewport, D3DEXECUTE_CLIPPED);

		// Check out the results.
		CDDSize<D3DEXECUTEDATA> d3d_exdata;
		D3DExBuf->GetExecuteData(&d3d_exdata);
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual CSet<ERenderFeature> seterfModify()
	{
		return seterf_Modify;
	}

	//******************************************************************************************
	virtual CSet<ERenderFeature> seterfDefault()
	{
		return seterf_Default;
	}

	//******************************************************************************************
	virtual void BeginFrame()
	{
		CorrectRenderState(pSettings->seterfState);

		// Clear the Z buffer (only).
		if (pSettings->seterfState[erfZ_BUFFER])
		{
			Assert(prasvZBuffer);

			D3DRECT d3drect;
			d3drect.x1 = d3drect.y1 = 0;
			d3drect.x2 = prasvZBuffer->iWidth;
			d3drect.y2 = prasvZBuffer->iHeight;
			D3D.err = D3DViewport->Clear(1, &d3drect, D3DCLEAR_ZBUFFER);

//			prasvZBuffer->Lock();
		}

		// Clear the screen.
		if (pSettings->bClearBackground)
		{
			Assert(prasvScreen);
			Assert(prasvScreen->pixFromColour(pSettings->clrBackground) == pixBackground);
			prasvScreen->Clear(pixBackground);
		}

//		prasvScreen->Lock();
		D3D.err = D3DDevice->BeginScene();
	}

	//******************************************************************************************
	virtual void EndFrame()
	{
		D3D.err = D3DDevice->EndScene();
/*
		prasvScreen->Unlock();
		if (prasvZBuffer)
			prasvZBuffer->Unlock();
*/
	}

	//******************************************************************************************
	virtual void SetRenderState(CSet<ERenderFeature> seterf)
	// Add state instructions to execute buffer.
	{
		// If there is no D3D Viewport, we must return because it is not required.
		if (!D3DViewport)
			return;


		// To do: Light states.

		// Put a STATERENDER op in the buffer.
		// Remember its position, so we can fill in the count later.
		SD3DInsStates* pd3dins = new((CMArray<char>&)D3DExBuf) SD3DInsStates(D3DOP_STATERENDER, 0);

		D3DExBuf << SD3DState(D3DRENDERSTATE_CULLMODE,
			seterf[erfRASTER_CULL] ? 
				// If we want software culling, then set CW if supported, else CCW.
				(pDeviceDesc->d3ddevcap.dpcTriCaps.dwMiscCaps & D3DPMISCCAPS_CULLCW ?
					D3DCULL_CCW : D3DCULL_CCW)
				: D3DCULL_NONE);

		// Z-buffer.
		// To do: Figure out how to do Z write-only, if possible.
/*
		D3DExBuf << SD3DState(D3DRENDERSTATE_ZFUNC, 
			D3DCMP_LESSEQUAL);
*/
		D3DExBuf << SD3DState(D3DRENDERSTATE_ZENABLE, 
			!!seterf[erfZ_BUFFER]);

		D3DExBuf << SD3DState(D3DRENDERSTATE_ZWRITEENABLE, 
			!!seterf[erfZ_WRITE]);

		// Shading.
		
		// To do: make first vertex contain average lighting.
		D3DExBuf << SD3DState(D3DRENDERSTATE_SHADEMODE,
			seterf[erfLIGHT_SHADE] ? D3DSHADE_GOURAUD : D3DSHADE_FLAT);

		// To do: work out interaction w/ FOGFLAT, FOGGOURAUD caps.
		D3DExBuf << SD3DState(D3DRENDERSTATE_FOGENABLE, 
			!!seterf[erfFOG]);

		// To do:
		// Figure out start and end.
		// D3DExBuf << SD3DState(D3DRENDERSTATE_FOGCOLOR, 

		// Use default fog mode, start, and end.
/*
		D3DExBuf << SD3DState(D3DRENDERSTATE_FOGTABLEMODE, 
			seterfState[erfFOG] ? D3DFOG_LINEAR : D3DFOG_NONE);
		D3DExBuf << SD3DState(D3DRENDERSTATE_FOGTABLESTART, 
			1.0f);
		D3DExBuf << SD3DState(D3DRENDERSTATE_FOGTABLEEND, 
			100.0f);
*/

		// To do: coloured support in lighting.
		D3DExBuf << SD3DState(D3DRENDERSTATE_MONOENABLE, 
			!seterf[erfCOLOURED_LIGHTS]);

		D3DExBuf << SD3DState(D3DRENDERSTATE_SPECULARENABLE, 
			!seterf[erfSPECULAR]);

		// Texture.

		// To do: disable textures (enable solid colouring).

		// To do: test transparencies.  MODULATEMASK isn't it.

		D3DExBuf << SD3DState(D3DRENDERSTATE_TEXTUREMAPBLEND,
			seterf[erfCOPY] ? 
				D3DTBLEND_COPY
			: seterf[erfTRANSPARENT] ?
				D3DTBLEND_MODULATEMASK
			:
				D3DTBLEND_MODULATE);

		D3DExBuf << SD3DState(D3DRENDERSTATE_SUBPIXEL,
			!!seterf[erfSUBPIXEL]);
		D3DExBuf << SD3DState(D3DRENDERSTATE_SUBPIXELX,
			!!seterf[erfSUBPIXEL]);

		D3DExBuf << SD3DState(D3DRENDERSTATE_TEXTUREPERSPECTIVE,
			!!seterf[erfPERSPECTIVE]);

		D3DExBuf << SD3DState(D3DRENDERSTATE_DITHERENABLE,
			!!seterf[erfDITHER]);

		// To do: add mip-mapping.
		D3DExBuf << SD3DState(D3DRENDERSTATE_TEXTUREMIN,
			seterf[erfFILTER] ? D3DFILTER_LINEAR : D3DFILTER_NEAREST);
		D3DExBuf << SD3DState(D3DRENDERSTATE_TEXTUREMAG,
			seterf[erfFILTER] ? D3DFILTER_LINEAR : D3DFILTER_NEAREST);

		// To do: test this, see what the hell it does.
		D3DExBuf << SD3DState(D3DRENDERSTATE_ANTIALIAS,
			!!seterf[erfFILTER_EDGES]);

		// Set the bloody count;
		pd3dins->wCount = (SD3DState*) D3DExBuf.ptEnd() - (SD3DState*) (pd3dins+1);

		// To do: create fill mode.
	}

	//******************************************************************************************
	virtual void DrawPolygons
	(
		CPArray<CRenderPolygon> parpoly
	)
	{
		if (parpoly.uLen == 0)
			return;

		// For now, assume one texture per object.
		rptr<CRasterVid> prasv;
		CD3DVidTexture*	pd3dvtex = 0;

		if (parpoly[0U].ptexTexture && parpoly[0U].ptexTexture->prasTexture)
			prasv = rptr_dynamic_cast(CRasterVid, rptr_nonconst(parpoly[0U].ptexTexture->prasTexture));

		// Load the material.
		pd3dvtex = D3DVidTextureList.pd3dvtexLoad(prasv, D3DDevice);
	
		// Start filling the Execute buffer.
		D3DExBuf.Reset();

		// First store the vertices.
		D3DTLVERTEX*	ad3dvertex = new((CMArray<char>&)D3DExBuf) D3DTLVERTEX[parv_vertices.uLen];
		for (uint u = 0; u < parv_vertices.uLen; u++)
			Convert(&ad3dvertex[u], parv_vertices[u], pd3dvtex->v2Scale);

		//
		// Now store all the instructions.
		//

		D3DExBuf << SD3DInsStates(D3DOP_STATELIGHT, 1);
		D3DExBuf << SD3DState(D3DLIGHTSTATE_MATERIAL, pd3dvtex->d3dmatTexture.d3dHandle);

		// The D3DOP_PROCESSVERTICES instruction is required, even though it just performs a COPY.
		D3DExBuf << SD3DInsProcessVertices(
			D3DPROCESSVERTICES_COPY | D3DPROCESSVERTICES_UPDATEEXTENTS, 
			0, parv_vertices.uLen);

		// Validate state if necessary.
		CorrectRenderState(pSettings->seterfState);

		if (pSettings->seterfState ^ seterfLast)
		{
			SetRenderState(pSettings->seterfState);
			seterfLast = pSettings->seterfState;
		}

		D3DExBuf << SD3DInsStates(D3DOP_STATERENDER, 1);
		D3DExBuf << SD3DState(D3DRENDERSTATE_TEXTUREHANDLE,	
			pSettings->seterfState[erfTEXTURE] ? pd3dvtex->d3dhTexture : 0);

		// A single triangle instruction, with the number of trianges we're drawing.
		D3DExBuf.PutInsTriangles(parpoly.uLen);
		for (CRenderPolygon* prpoly = parpoly; prpoly < parpoly.ptEnd(); prpoly++)
		{
			Assert(prpoly->ptexTexture == parpoly[0U].ptexTexture);
			D3DExBuf << SD3DTriangle
			(
				// Convert RenderVertex pointers to indices.
				// Pass in clockwise order.
				prpoly->aprvVertices[2] - parv_vertices,
				prpoly->aprvVertices[1] - parv_vertices,
				prpoly->aprvVertices[0] - parv_vertices
			);
		}

		D3DExBuf.Finish(parv_vertices.uLen);

/*
		prasvScreen->Unlock();
		if (prasvZBuffer)
			prasvZBuffer->Unlock();
*/

		D3DExBuf.Execute(D3DViewport, D3DEXECUTE_CLIPPED);

/*
		prasvScreen->Lock();
		if (prasvZBuffer)
			prasvZBuffer->Lock();
*/
	}
};

//**********************************************************************************************
//
class CRenderDescD3D: public CRenderDesc
//
//**************************************
{
private:
	uint	uDriver;

public:

	//**********************************************************************************************
	//
	// Constructor.
	//

	//**********************************************************************************************
	CRenderDescD3D(uint u_driver)
		: CRenderDesc(D3D.sadevdList[u_driver].strName), uDriver(u_driver)
	{
		Assert(D3D.bValid);
	}

	//**********************************************************************************************
	virtual CScreenRender* pScreenRenderCreate(SSettings* pscrenset, rptr<CRaster> pras_screen, rptr<CRaster> pras_zbuffer)
	{
		Assert(D3D.bValid);

		// Not allowed to pass in an explicit z-buffer.  And screen must be a CRasterVid.
		Assert(!pras_zbuffer);
		return new CScreenRenderD3D(pscrenset, rptr_dynamic_cast(CRasterVid, pras_screen), uDriver);
	}
};


//**********************************************************************************************
//
void AddRenderDescD3D()
//
// Add RenderDescs for all available D3D drivers to sapRenderDesc.
//
//**************************************
{
	// Ensure that the D3D var has been initialised.
	InitPtr(pD3D);

	if (!D3D.bValid)
	{
		//
		// There is no Direct3D support.  
		// Don't add any CRenderDescs, and that should be the end of the story.  
		// Since all access to Direct3D is through the CRenderDesc and CScreenRender interfaces, 
		// we won't have to worry about anyone else thinking Direct3D exists.
		//
		return;
	}

	// Add all the drivers.
	for (uint u = 0; u < D3D.sadevdList.uLen; u++)
		sapRenderDesc << new CRenderDescD3D(u);
}
