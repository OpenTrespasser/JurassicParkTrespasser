/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		A Direct3D driver.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/View/Direct3DRenderState.hpp                                      $
 * 
 * 13    9/22/98 10:39p Pkeet
 * Forced inline critical Direct3D functions.
 * 
 * 12    9/15/98 4:17p Pkeet
 * Added warnings for textures that do no have D3D rasters.
 * 
 * 11    9/01/98 12:43a Pkeet
 * Removed the unnecessary state changes in the 'SetZBuffer' member function.
 * 
 * 10    8/25/98 11:36p Pkeet
 * Moved 'SetAllowZBuffer' to the implementation file.
 * 
 * 9     8/25/98 11:01p Pkeet
 * Added a permission flag for Z buffering.
 * 
 * 8     8/20/98 4:43p Pkeet
 * Removed the Z buffer flag from the Direct3D object.
 * 
 * 7     8/11/98 6:17p Pkeet
 * Removed unnecessary include.
 * 
 * 6     8/07/98 11:45a Pkeet
 * Added code to make sure Z buffer states are always correct.
 * 
 * 5     7/29/98 8:24p Pkeet
 * Added code for Z buffering.
 * 
 * 4     7/29/98 11:45a Pkeet
 * The device changes addressing mode to wrap or clamp as required.
 * 
 * 3     7/23/98 10:28p Pkeet
 * Added the specular state.
 * 
 * 2     7/20/98 10:30p Pkeet
 * The Direct3D texture interface is now used instead of a handle.
 * 
 * 1     7/20/98 1:09p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_DIRECT3DRENDERSTATE_HPP
#define HEADER_LIB_VIEW_DIRECT3DRENDERSTATE_HPP


//
// Necessary includes.
//
#include "Lib/W95/WinInclude.hpp"
#include "Lib/W95/DD.hpp"
#define D3D_OVERLOADS
#include <DirectX/d3d.h>
#include "Lib/W95/Direct3D.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/ScreenRender.hpp"


//
// Macro definitions.
//

// Number of multi-texture stages.
#define iNUM_TEXTURE_STAGES (8)


//
// Class definitions.
//

//*********************************************************************************************
//
class CDirect3DRenderState
//
// Encapsulates D3D functionality
//
// Prefix: d3dst
//
// Notes:
//		This object caches the following states for Direct3D stages:
//
//			SetTexture
//			SetFiltering
//
//		And the following general render states:
//
//			SetAlpha
//			SetFog
//			SetTransparency (through setting alpha compares)
//			SetShading (sets gouraud or flat shading).
//			
//
//**************************************
{
private:

	//******************************************************************************************
	//
	struct SStageState
	//
	// Caches Direct3D state values for each stage of the Direct3D multitexturing pipeline.
	//
	// Prefix: sts
	//
	//**************************************
	{
		bool               bWrapU;			// Wrap for u texture coordinates.
		bool               bWrapV;			// Wrap for v texture coordinates.
		bool               bFiltering;		// Filtering state.
		LPDIRECT3DTEXTURE2 pd3dtexCurrent;	// Current active texture.
	};

	// Global Direct3D states.
	bool        bAlpha;								// Alpha blending state.
	bool        bFog;								// Fog state for vertex fogging.
	bool        bTransparent;						// Transparency state.
	bool        bShaded;							// Shading state.
	bool        bSpecular;							// Specular state.
	bool        bZBuffer;							// Z buffer state.
	SStageState astsStates[iNUM_TEXTURE_STAGES];	// Stage states.

	// Global Permissions.
	bool        bAllowZBuffer;						// Enables or disables Z buffering in general.

public:
	
	//******************************************************************************************
	//
	// Constructor.
	//

	// Default constructor.
	CDirect3DRenderState();

	
	//******************************************************************************************
	//
	// Member functions.
	//

	//*********************************************************************************************
	//
	forceinline void SetAddressing
	(
		EAddressMode eam = eamTileNone,	// Addressing mode to set stage state to.
		int          i_stage = 0		// Stage to set state for.
	)
	//
	// Sets the transparency state.
	//
	//**************************************
	{
		SStageState& sts = astsStates[i_stage];

		// Select addressing mode.
		switch (eam)
		{
			case eamTileNone:
				if (sts.bWrapU || sts.bWrapV)
				{
					sts.bWrapU = false;
					sts.bWrapV = false;
					d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);
				}
				break;

			case eamTileUV:
				if (!sts.bWrapU || !sts.bWrapV)
				{
					sts.bWrapU = true;
					sts.bWrapV = true;
					d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_ADDRESS, D3DTADDRESS_WRAP);
				}
				break;

			case eamTileU:
				if (!sts.bWrapU)
				{
					sts.bWrapU = true;
					d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
				}
				if (sts.bWrapV)
				{
					sts.bWrapV = false;
					d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
				}
				break;

			case eamTileV:
				if (sts.bWrapU)
				{
					sts.bWrapU = false;
					d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
				}
				if (!sts.bWrapV)
				{
					sts.bWrapV = true;
					d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
				}
				break;
		}
	}

	//******************************************************************************************
	//
	forceinline bool bFogState
	(
	)
	//
	// Returns the vertex fogging state.
	//
	//**************************************
	{
		return bFog;
	}

	//*********************************************************************************************
	//
	void SetDefault
	(
	);
	//
	// Sets the default values for render states for all render stages.
	//
	//**************************************

	//******************************************************************************************
	//
	forceinline void SetAlpha
	(
		bool b_state = false
	)
	//
	// Sets the alpha state.
	//
	//**************************************
	{
		// Do nothing if the correct alpha state is already set.
		if (bAlpha == b_state)
			return;
		bAlpha = b_state;

		// Set to the desired alpha state.
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, bAlpha);
	}

	//*********************************************************************************************
	//
	forceinline void SetFiltering
	(
		bool b_state = false,
		int  i_stage = 0
	)
	//
	// Sets the filtering state.
	//
	//**************************************
	{
		// Do nothing if the state is correct.
		if (astsStates[i_stage].bFiltering == b_state)
			return;

		astsStates[i_stage].bFiltering = b_state;
		if (b_state)
		{
			d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
			d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_MINFILTER, D3DTFG_LINEAR);
		}
		else
		{
			d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_MAGFILTER, D3DTFG_POINT);
			d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_MINFILTER, D3DTFG_POINT);
		}
	}

	//******************************************************************************************
	//
	forceinline void SetFog
	(
		bool b_state = false
	)
	//
	// Sets the vertex fogging state.
	//
	//**************************************
	{
		// Do nothing if the state is already set.
		if (bFog == b_state)
			return;

		bFog = b_state;
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_FOGENABLE, bFog);
	}

	//*********************************************************************************************
	//
	forceinline void SetTexture
	(
		CRaster* pras,
		int      i_stage = 0
	)
	//
	// Sets the texture inteface for a given texture stage.
	//
	//**************************************
	{
		// Check for null textures.
		if (!pras)
		{
			// Do not set texture if not required.
			if (!astsStates[i_stage].pd3dtexCurrent)
			{
				PrintD3D("D3D Warning: D3D texture not found.\n");
				return;
			}

			// Set a null texture for stage.
			d3dDriver.err = d3dDriver.pGetDevice()->SetTexture(i_stage, 0);
			astsStates[i_stage].pd3dtexCurrent = 0;
			PrintD3D("D3D Warning: D3D texture not found.\n");
			return;
		}

		// Get the texture interface.
		LPDIRECT3DTEXTURE2 pd3dtex = (LPDIRECT3DTEXTURE2)pras->pd3dtexGet();
		AlwaysAssert(pd3dtex);

		// Do not apply the texture interface if it does matches the current one.
		if (astsStates[i_stage].pd3dtexCurrent == pd3dtex)
			return;

		// Apply the texture interface.
		d3dDriver.err = d3dDriver.pGetDevice()->SetTexture(i_stage, pd3dtex);
		astsStates[i_stage].pd3dtexCurrent = pd3dtex;
	}

	//*********************************************************************************************
	//
	forceinline void SetTextureNull
	(
		int i_stage = 0
	)
	//
	// Sets the texture inteface to null for a given texture stage.
	//
	//**************************************
	{
		// Do not apply the texture interface if it does matches the current one.
		if (astsStates[i_stage].pd3dtexCurrent == 0)
			return;

		// Apply the texture interface.
		d3dDriver.err = d3dDriver.pGetDevice()->SetTexture(i_stage, 0);
		astsStates[i_stage].pd3dtexCurrent = 0;
	}

	//******************************************************************************************
	//
	forceinline void SetShading
	(
		bool b_state = false
	)
	//
	// Sets the shading state.
	//
	//**************************************
	{
		// Do nothing if the state is already set.
		if (bShaded == b_state)
			return;

		bShaded = b_state;
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState
		(
			D3DRENDERSTATE_SHADEMODE,
			(bShaded) ? (D3DSHADE_GOURAUD) : (D3DSHADE_FLAT)
		);
	}

	//******************************************************************************************
	//
	forceinline void SetSpecular
	(
		bool b_state = false
	)
	//
	// Sets the specular state.
	//
	//**************************************
	{
		// Do nothing if the correct alpha state is already set.
		if (bSpecular == b_state)
			return;
		bSpecular = b_state;

		// Set to the desired alpha state.
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, bSpecular);
	}

	//*********************************************************************************************
	//
	forceinline void SetTransparency
	(
		bool b_state = false
	)
	//
	// Sets the transparency state.
	//
	//**************************************
	{
		// Do nothing if the state is correct.
		if (bTransparent == b_state)
			return;

		bTransparent = b_state;
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, bTransparent);
	}

	//*********************************************************************************************
	//
	forceinline void SetZBuffer
	(
		bool b_state = false
	)
	//
	// Sets the Z buffer state.
	//
	//**************************************
	{
		// Do nothing if the state is correct.
		if (bZBuffer == b_state || !bAllowZBuffer)
			return;

		bZBuffer = b_state;
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ZENABLE, bZBuffer);
	}

	//*********************************************************************************************
	//
	void SetAllowZBuffer
	(
		bool b_state = false
	);
	//
	// Sets the Z buffer permission state.
	//
	//**************************************

	//*********************************************************************************************
	//
	forceinline bool bGetAllowZBuffer
	(
	) const
	//
	// Returns the Z buffer permission state.
	//
	//**************************************
	{
		return bAllowZBuffer;
	}

};


//
// Global variables.
//

// Global Direct3D state object.
extern CDirect3DRenderState d3dstState;


#endif // HEADER_LIB_VIEW_DIRECT3DRENDERSTATE_HPP