/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Implementation of Overlay.hpp.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Overlay.cpp                                              $
 * 
 * 3     98.09.24 1:40a Mmouni
 * Shrunk CRenderPolygon by a large amount.
 * Multiple list depth sort stuff is now on a compile switch.
 * 
 * 2     98.09.14 12:05p Mmouni
 * Now sets area of overlay polygon to prevent assert.
 * 
 * 1     98.08.31 9:31p Mmouni
 * Initial version.
 * 
 **********************************************************************************************/

//
// Defines and pragmas.
//
#include <map>
#include <math.h>
#include <memory>
#include "Common.hpp"
#include "Overlay.hpp"

#include "Lib/W95/Direct3D.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "D3DTypes.h"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "PipeLine.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Std/PrivSelf.hpp"

#include "RenderDefs.hpp"
#include "Camera.hpp"
#include "ScreenRender.hpp"
#include "PipeLineHeap.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/Transform/Matrix2.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3DUtilities.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"
#include "Lib/View/Direct3DRenderState.hpp"


//
// Constants.
//
const CColour clrAlphaFlash = CColour(128, 128, 128);
const CColour clrStippleFlashBase = CColour(95, 95, 95);
const CColour clrStippleFlashRange = CColour(160, 160, 160);


//*********************************************************************************************
//
// Class COverlay implementation.
//

	//*****************************************************************************************
	//
	// COverlay constructor and destructor.
	//

	//*****************************************************************************************
	COverlay::COverlay()
	{
		bEnabled = false;

		// Our texture (actually just color and alpha).
		ptexTexture = new CTexture();
	}

	//*****************************************************************************************
	COverlay::~COverlay()
	{
		delete ptexTexture;
	}

	//*****************************************************************************************
	//
	// COverlay member function.
	//
	
	//*****************************************************************************************
	void COverlay::Enable(float f_percent)
	{
		bEnabled = true;
		fPercent = f_percent;

		if (fPercent < 0.0f)
			fPercent = 0.0f;
		else if (fPercent > 1.0f)
			fPercent = 1.0f;
	}

	//*****************************************************************************************
	void COverlay::Disable()
	{
		bEnabled = false;
	}

	//*****************************************************************************************
	void COverlay::Add(const CCamera& cam, CPipelineHeap& rplhHeap)
	{
		static SRenderVertex  rvVertices[4];
		static SRenderVertex* prvVertices[4];

		if (!bEnabled)
			return;

		Assert(prasMainScreen.ptGet());

		if (d3dDriver.bUseD3D() && d3dDriver.bUseAlpha())
		{
			// Adjust alpha value only.
			*ptexTexture = CTexture(clrAlphaFlash.u1Red, clrAlphaFlash.u1Green, 
									clrAlphaFlash.u1Blue, int(fPercent * 255.0f));
		}
		else
		{
			// Adjust colour since we are using a 50% stipple for alpha.
			int i_red   = clrStippleFlashBase.u1Red   + fPercent * clrStippleFlashRange.u1Red;
			int i_green = clrStippleFlashBase.u1Green + fPercent * clrStippleFlashRange.u1Green;
			int i_blue  = clrStippleFlashBase.u1Blue  + fPercent * clrStippleFlashRange.u1Blue;

			*ptexTexture = CTexture(i_red, i_green, i_blue, 128);
		}

		// Set the width and the height of the particle screen.
		float fScreenWidth  = prasMainScreen->iWidth  + 0.5f;
		float fScreenHeight = prasMainScreen->iHeight + 0.5f;

		// Add the overlay polygon.
		CRenderPolygon& rpoly = *rplhHeap.darpolyPolygons.paAlloc(1);

		rvVertices[0].v3Screen.tX = 0.5f;
		rvVertices[0].v3Screen.tY = 0.5f;
		rvVertices[0].v3Screen.tZ = fInvScale;
		rvVertices[0].v3Cam.tY	  = fInvScale;
		
		rvVertices[1].v3Screen.tX = 0.5f;
		rvVertices[1].v3Screen.tY = fScreenHeight;
		rvVertices[1].v3Screen.tZ = fInvScale;
		rvVertices[1].v3Cam.tY	  = fInvScale;
		
		rvVertices[2].v3Screen.tX = fScreenWidth;
		rvVertices[2].v3Screen.tY = fScreenHeight;
		rvVertices[2].v3Screen.tZ = fInvScale;
		rvVertices[2].v3Cam.tY	  = fInvScale;
		
		rvVertices[3].v3Screen.tX = fScreenWidth;
		rvVertices[3].v3Screen.tY = 0.5f;
		rvVertices[3].v3Screen.tZ = fInvScale;
		rvVertices[3].v3Cam.tY	  = fInvScale;

		prvVertices[0] = &rvVertices[0];
		prvVertices[1] = &rvVertices[1];
		prvVertices[2] = &rvVertices[2];
		prvVertices[3] = &rvVertices[3];

		rpoly.paprvPolyVertices.atArray = prvVertices;
		rpoly.paprvPolyVertices.uLen    = 4;
		rpoly.seterfFace                = Set(erfTRAPEZOIDS) + erfALPHA_COLOUR;
		rpoly.ptexTexture               = ptexTexture;
		rpoly.cvFace                    = 1.0f;
		rpoly.iFogBand                  = 0;
		rpoly.rPlaneTolerance           = 0.001;
		rpoly.iMipLevel                 = 0;
		rpoly.bAccept                   = true;
		rpoly.eamAddressMode            = eamTileNone;
		rpoly.bPrerasterized            = false;
		rpoly.bPrerasterized            = false;
		rpoly.fArea			            = (fScreenWidth - 0.5f) * (fScreenHeight - 0.5f);

#if (VER_DEBUG)
		rpoly.pshOwningShape            = 0;
#endif


		if (d3dDriver.bUseD3D() && d3dDriver.bUseAlpha())
		{
			rpoly.ehwHardwareFlags = ehwAlphaCol;
			rpoly.bFullHardware    = true;

			// Draw D3D poly immediately.
			d3dstState.SetAllowZBuffer(false);
			srd3dRenderer.bDrawPolygon(rpoly);
			d3dstState.SetAllowZBuffer(true);
		}
		else
		{
			rpoly.ehwHardwareFlags = ehwSoftware;
			rpoly.bFullHardware    = false;

			// Since we add the overlay after sorting we need to add ourselves to the list.
			rplhHeap.darppolyPolygons << &rpoly;
		}
	}


//
// Global variables.
//
COverlay Overlay;
