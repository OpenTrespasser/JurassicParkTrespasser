/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Implementation of Direct3DRenderState.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Direct3DRenderState.cpp                                      $
 * 
 * 15    9/16/98 2:15p Pkeet
 * Turned colour key off by default.
 * 
 * 14    9/01/98 12:43a Pkeet
 * Set the default to allow z buffering, and z buffering on.
 * 
 * 13    8/25/98 11:36p Pkeet
 * Moved 'SetAllowZBuffer' to the implementation file. Fixed bug in setting state.
 * 
 * 12    8/25/98 10:10p Pkeet
 * Removed modex allocations.
 * 
 * 11    8/20/98 4:43p Pkeet
 * Removed the Z buffer flag from the Direct3D object.
 * 
 * 10    8/04/98 6:20p Pkeet
 * Enabled Z writes by default.
 * 
 * 9     8/03/98 4:24p Pkeet
 * The Z bias value is set to zero by default.
 * 
 * 8     8/01/98 4:45p Pkeet
 * Changed transparency default setup.
 * 
 * 7     7/30/98 4:08p Pkeet
 * Made the Z buffer draw a pixel on greater than or equals instead of on just greater than.
 * 
 * 6     7/29/98 8:24p Pkeet
 * Added code for Z buffering.
 * 
 * 5     7/29/98 11:45a Pkeet
 * The device changes addressing mode to wrap or clamp as required.
 * 
 * 4     7/23/98 10:28p Pkeet
 * Added the specular state.
 * 
 * 3     7/23/98 10:03p Pkeet
 * Inverted alpha for hardware.
 * 
 * 2     7/21/98 12:35p Pkeet
 * Fixed alpha textures and fills.
 * 
 * 1     7/20/98 1:10p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Direct3DRenderState.hpp"


//*********************************************************************************************
//
// Class CDirect3DRenderState implementation.
//

	//*********************************************************************************************
	CDirect3DRenderState::CDirect3DRenderState()
	{
	}

	//*********************************************************************************************
	void CDirect3DRenderState::SetDefault()
	{
		// Set the defaults for each stage.
		for (int i_stage = 0; i_stage < iNUM_TEXTURE_STAGES; ++i_stage)
		{
			SStageState& sts = astsStates[i_stage];

			// Texturing.
			sts.pd3dtexCurrent = 0;
			d3dDriver.err = d3dDriver.pGetDevice()->SetTexture(i_stage, 0);

			// Texture addressing.
			sts.bWrapU = false;
			sts.bWrapV = false;
			d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);

			// Filtering.
			sts.bFiltering = false;
			d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_MAGFILTER, D3DTFG_POINT);
			d3dDriver.err = d3dDriver.pGetDevice()->SetTextureStageState(i_stage, D3DTSS_MINFILTER, D3DTFG_POINT);
		}

		// Alpha blending.
		bAlpha = false;
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);

		// Fogging.
		bFog = false;
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);

		// Shading.
		bShaded = false;
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);

		// Specular.
		bSpecular = false;
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);

		// Transparency.
		bTransparent = false;
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ALPHAREF,  0x00001000);
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER);

		// ZBuffer state.
		bAllowZBuffer = true;
		bZBuffer = true;
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_GREATEREQUAL);
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ZBIAS, 0);
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
		
		// Dither if enabled.
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_DITHERENABLE, d3dDriver.bUseDithering());

		// Global uncached states.
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_FILLMODE,        D3DFILL_SOLID);
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_CULLMODE,        D3DCULL_NONE);
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_SUBPIXEL,        TRUE);
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
		
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_SRCALPHA);
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);

		// Turn off colour key.
		d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
	}

	//*********************************************************************************************
	void CDirect3DRenderState::SetAllowZBuffer(bool b_state)
	{
		// Do nothing if the state is correct.
		if (bAllowZBuffer == b_state || !d3dDriver.pGetDevice())
			return;

		if (b_state)
		{
			bAllowZBuffer = true;
			SetZBuffer(true);
			d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
		}
		else
		{
			SetZBuffer(false);
			bAllowZBuffer = false;
			d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
		}
	}


//
// Global variables.
//
CDirect3DRenderState d3dstState;