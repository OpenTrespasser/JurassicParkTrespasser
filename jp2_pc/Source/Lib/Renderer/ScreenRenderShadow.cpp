/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ScreenRenderShadow.cpp                                   $
 * 
 * 11    1/12/98 11:46a Pkeet
 * Added in the software lock request for the auxilary renderer.
 * 
 * 10    97/11/08 3:48p Pkeet
 * Removed Z template parameters and code.
 * 
 * 9     97/06/27 15:38 Speter
 * Changed CGouraudOff to CGouraudNone where appropriate. 
 * 
 * 8     6/19/97 2:53p Mlange
 * Moved Profile.hpp to Lib/Sys.
 * 
 * 7     97/06/17 2:03p Pkeet
 * Removed the reference counting pointer from prasScreen.
 * 
 * 6     97/05/23 17:35 Speter
 * Updated CScreenRenderShadow for new CDrawPolygon.
 * 
 * 5     97/05/13 13:40 Speter
 * Renamed SRenderTriangle::seterfTri to seterfFace.
 * 
 * 4     97-05-06 16:09 Speter
 * Updated, simplified, for new parallel-capable CCamera.
 * 
 * 3     97-04-04 12:38 Speter
 * A big change: now pipeline uses CRenderPolygon rather than SRenderTriangle (currently always
 * as a triangle).  Changed associated variable names and comments.
 * 
 * 2     97-04-02 19:12 Speter
 * Changed interface to match new DrawTriangles() taking array of pointers, fixing shadows.
 * 
 * 1     97-03-28 17:00 Speter
 * Rasteriser specialised/simplified for shadow buffering.
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "common.hpp"
#include "Primitives/DrawTriangle.hpp"
#include "ScreenRenderShadow.hpp"
#include "Shadow.hpp"

#include "Lib/Sys/Profile.hpp"

//*********************************************************************************************
//
//typedef CZBufferInvNoScreen<TShadowPixel> CZBufferShadow;
//
// The shadowing Z-buffering class; prevents screen writes.
//
//**************************************

//******************************************************************************************
//
class CScreenRenderShadow : public CScreenRender
//
// The base of an implementation of CScreenRender.  Provides common functions not
// specialised for pixel type.
//
//**************************************
{
public:

	//******************************************************************************************
	CScreenRenderShadow(SSettings* pscrenset, rptr<CRaster> pras_screen)
		: CScreenRender(pscrenset, pras_screen)
	{
		Assert(pras_screen->iPixelBytes() == sizeof(TShadowPixel));
	}

	//******************************************************************************************
	~CScreenRenderShadow()
	{
	}

private:

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void BeginFrame() override
	{
		Assert(prasScreen);

		prasScreen->Lock();

		if (pSettings->bClearBackground)
		{
			CCycleTimer ctmr;
			prasScreen->Clear(0);
			proProfile.psClearScreen.Add(ctmr());
		}
	}

	//******************************************************************************************
	virtual void EndFrame() override
	{
		prasScreen->Unlock();
	}

	//******************************************************************************************
	virtual void DrawPolygons(CPArray<CRenderPolygon*> paprpoly) override
	{
		//CZBufferShadow::SetConversion(fSHADOW_Z_MULTIPLIER, 0);

		CScreenRender::DrawPolygons(paprpoly);
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	/*
	//******************************************************************************************
	void DrawPolygon
	(
		CRenderPolygon& rp
	)
	{
		// Invoke the CDrawPolygon code, using the screen as both screen and Z buffer.
		if (rp.seterfFace[erfTRANSPARENT])
			CDrawPolygon< CScanline<TShadowPixel, CZBufferShadow, CGouraudNone, CTransparencyOn, CMap<uint8>, CIndexLinear, CColLookupOff> >
			(
				prasScreen,
				prasScreen,
				rp
			);
		else
			CDrawPolygon< CScanline<TShadowPixel, CZBufferShadow, CGouraudNone, CTransparencyOff, CMap<uint8>, CIndexNone, CColLookupOff> >
			(
				prasScreen,
				prasScreen,
				rp
			);
	}
	*/
};

//**********************************************************************************************
CScreenRender* pscrenShadow(CScreenRender::SSettings* pscrenset, rptr<CRaster> pras_screen)
{
	//return new CScreenRenderShadow(pscrenset, pras_screen);
	return 0;
}
