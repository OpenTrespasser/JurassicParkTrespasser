/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of TestMath.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/PolyTest.cpp                                                     $
 * 
 * 31    96/10/28 14:53 Speter
 * Changed AppShell:: to CAppShell::.
 * 
 * 30    8/23/96 10:38a Gstull
 * Changed prasvReadBMP() back to prasReadBMP().  Now creates a CRasterVid only if D3D support
 * exists.
 * 
 * 29    96/08/22 18:27 Speter
 * Changed, expanded render flag definitions.
 * 
 * 28    96/08/15 19:03 Speter
 * Call new prasvReadBMP function.
 * 
 * 27    7/31/96 3:25p Pkeet
 * Finished tests.
 * 
 * 26    7/31/96 12:40p Pkeet
 * Updated for new object 2d definitions.
 * 
 * 25    96/07/23 17:23 Speter
 * Fixed for new object structures.
 * 
 * 24    7/19/96 11:22a Pkeet
 * Fixed shell problem.
 * 
 * 23    96/07/18 17:19 Speter
 * Changes affecting zillions of files:
 * Changed SRenderCoord to SRenderVertex.
 * Added SRenderTriangle type, changed drawing primitives to accept it.
 * 
 * 22    96/07/18 16:56 Speter
 * 
 * 21    7/18/96 3:29p Pkeet
 * Converted to 'CRender'-style shell.
 * 
 * 20    96/07/15 19:19 Speter
 * Changed calls to DrawTriangle to new interface.
 * 
 * 19    7/15/96 7:12p Pkeet
 * More changes.
 * 
 * 18    96/07/12 17:44 Speter
 * Made preliminary changes to object structure and render flags.
 * 
 * 17    7/12/96 4:59p Pkeet
 * Finished debugging texture mapper and gouraud shader.
 * 
 * 16    7/09/96 3:28p Pkeet
 * Added test data for gouraud shading and linear texture mapping primitive.
 * 
 * 15    7/08/96 3:31p Pkeet
 * Changed for Scott's code review.
 * 
 * 14    96/07/03 13:02 Speter
 * Moved several files to new directories, and changed corresponding include statements.
 * 
 * 13    6/25/96 2:34p Pkeet
 * Finished tests.
 * 
 * 12    6/24/96 6:25p Pkeet
 * Finished series of tests.
 * 
 * 11    6/18/96 3:00p Pkeet
 * 
 * 10    6/17/96 6:48p Pkeet
 * Test conversion functions.
 * 
 * 9     6/13/96 3:10p Pkeet
 * 
 * 8     6/06/96 7:58p Pkeet
 * Changed to run test for DrawTriangle instead of DrawPolygon.
 * 
 * 7     96/05/31 12:48 Speter
 * Moved Clear from Winshell to Paint handlers.
 * 
 * 6     5/29/96 7:54p Pkeet
 * Fixed tear bug.
 * 
 * 5     5/29/96 4:42p Pkeet
 * 
 * 4     96/05/23 17:27 Speter
 * Removed erroneous include files.
 * 
 * 3     5/23/96 9:30a Pkeet
 * 
 * 2     5/17/96 10:22a Pkeet
 * 
 * 1     5/09/96 7:00p Pkeet
 * Empty initial version.
 * 
 **********************************************************************************************/

#include "Lib/W95/WinInclude.hpp"
#include "gblinc/common.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Shell/winshell.hpp"
#include "Shell/AppShell.hpp"
#include "Shell/WinEvent.hpp"
#include "Test/PolyData.hpp"
#include "Lib/View/ColourBase.hpp"
#include "Lib/Renderer/Texture.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "PolyTest.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Sys/W95/Render.hpp"

SRenderTriangle rtTestTriangle
(
	&arvTestVertices[0],
	&arvTestVertices[1],
	&arvTestVertices[2]
);

SRenderTriangle rtTestTriangle2
(
	&arvTestVertices[0],
	&arvTestVertices[2],
	&arvTestVertices[3]
);

float fMoveBy()
{
	if (GetAsyncKeyState(VK_SHIFT) == 0)
		return 8.0F;
	else
		return 0.3F;
}

bool bShow[5] = { true, false, false, false, false };

bool bShowPal = false;

int iChangeVertex = 1;//4;

int iStyle = 2;

void DoKey(uint u_key_code)
{
	int i_temp;

	switch (u_key_code)
	{
		case '1':
			bShow[0] = !bShow[0];
			break;
		case '2':
			bShow[1] = !bShow[1];
			break;
		case '3':
			bShow[2] = !bShow[2];
			break;
		case '4':
			bShow[3] = !bShow[3];
			break;
		case '5':
			bShow[4] = !bShow[4];
			break;
		case VK_UP:
			arvTestVertices[iChangeVertex].v3Screen.tY += fMoveBy();
			break;
		case VK_DOWN:
			arvTestVertices[iChangeVertex].v3Screen.tY -= fMoveBy();
			break;
		case VK_LEFT:
			arvTestVertices[iChangeVertex].v3Screen.tX += fMoveBy();
			break;
		case VK_RIGHT:
			arvTestVertices[iChangeVertex].v3Screen.tX -= fMoveBy();
			break;

		case VK_F5:
			iStyle = (iStyle + 1) % 4;
			break;

		case VK_F6:
			TerminateShell(false);
			break;
		case VK_F7:
			i_temp = aiPoly3[0];
			aiPoly3[0] = aiPoly3[1];
			aiPoly3[1] = aiPoly3[2];
			aiPoly3[2] = i_temp;
			break;
		case VK_F8:
			iChangeVertex = (iChangeVertex + 1) % 6;
			break;
		case VK_F11:
			RollTestVertices();
			break;
		
		case VK_F3:
			arvTestVertices[6].fInvZ *= 1.08f;
			break;
		case VK_F4:
			arvTestVertices[6].fInvZ /= 1.08f;
			break;
			
		case VK_F2:
			return;

		case VK_F1:
			bShowPal = !bShowPal;
			break;
		default:
			return;
	}
	CAppShell::Repaint();
}

uint32 GetColour(CRaster* pras, int i_red, int i_green, int i_blue)
{
	switch (pras->iPixelBits)
	{
		case 8:
			Assert(pcdbMain.pceMainPalClut);
			Assert(pcdbMain.pceMainPalClut->ppalPalette);
			return pcdbMain.pceMainPalClut->ppalPalette->u1MatchEntry(CColour(i_red, i_green, i_blue));
			break;
		case 16:
			return ((i_red >> 3) << 10) | ((i_green >> 3) << 5) | (i_blue >> 3);
			break;
		case 24:
			return (i_red << 16) | (i_green << 8) | (i_blue);
			break;
		default:
			Assert(false);
			return 0;
	}
}

uint32 u4MakeColour(int i_r, int i_g, int i_b, int i_depth = 0)
{
	Assert(psrMain);
	Assert(psrMain->prasScreen);

	int i_pal_index;
	CPalClut* ppce = pcdbMain.ppceFindColourMatch(CColour(i_r, i_g, i_b), i_pal_index);

	Assert(ppce);
	Assert(ppce->pclutClut);

	uint32 u4_ret_data = ppce->pclutClut->u4GetGouraudAddress(i_pal_index, i_depth);
	Assert(u4_ret_data);
	return u4_ret_data;
}


void WindowsEvent(uint u_message, WPARAM wp_param, LPARAM lp_param)
{
	switch (u_message)
	{
		case WM_DESTROY:
			// Destroy the test bitmap.
			// ptexTexture is a const pointer and should be deleted elsewhere.
//			delete rtTestTriangle.ptexTexture;
			break;

		case AM_NEWRASTER:
			CAppShell::SetContinuous(false);
			if (rtTestTriangle.ptexTexture == 0)
			{
				if (*strCommandArg)
				{
					rtTestTriangle.ptexTexture = new CTexture(prasReadBMP(strCommandArg));
				}
				else
				{
					rtTestTriangle.ptexTexture = new CTexture(prasReadBMP("BinData\\Palettes\\Standard.bmp"));
				}
			}
			if (rtTestTriangle2.ptexTexture == 0)
			{
				if (*strCommandArg)
				{
					rtTestTriangle2.ptexTexture = new CTexture(prasReadBMP(strCommandArg));
				}
				else
				{
					rtTestTriangle2.ptexTexture = new CTexture(prasReadBMP("BinData\\Palettes\\Standard.bmp"));
				}
			}
			Assert(prasMainScreen);
			Assert(rtTestTriangle.ptexTexture);
			break;
		case WM_KEYDOWN:
			DoKey(wp_param);
			break;
		case AM_PAINT:
			Assert(psrMain);
			Assert(rtTestTriangle.ptexTexture);
			Assert(rtTestTriangle.ptexTexture->prasTexture);
			Assert(rtTestTriangle.ptexTexture->ppcePalClut);
			Assert(rtTestTriangle2.ptexTexture);
			Assert(rtTestTriangle2.ptexTexture->prasTexture);
			Assert(rtTestTriangle2.ptexTexture->ppcePalClut);

			if (rtTestTriangle.ptexTexture->ppcePalClut->pclutClut == 0)
				pcdbMain.UpdateCluts(prasMainScreen);
			if (rtTestTriangle2.ptexTexture->ppcePalClut->pclutClut == 0)
				pcdbMain.UpdateCluts(prasMainScreen);

			conStd.OpenFileSession("x.txt");
			conStd.ClearScreen();

			prasMainScreen->Clear(0);
			psrMain->ResetZBuffer();

			//
			// Draw test polygons on the screen.
			//
/*
			psrMain->DrawTriangle
			(
				&arvTestVertices[5],
				&arvTestVertices[6],
				&arvTestVertices[7],
				GetColour(prasMainScreen, 0, 255, 0),
				eFLAT_SHADED_Z
			);
*/
			switch (iStyle)
			{
				case 0:
					if (bShow[0])
					{
						((CTexture*)rtTestTriangle.ptexTexture)->tpSolid
							= rtTestTriangle.ptexTexture->prasTexture->pixFromColour(CColour(0, 255, 0));
						psrMain->DrawTriangle
						(
							rtTestTriangle,
							Set(erfZ_BUFFER)
						);
					}
					if (bShow[0])
					{
						((CTexture*)rtTestTriangle2.ptexTexture)->tpSolid
							= rtTestTriangle.ptexTexture->prasTexture->pixFromColour(CColour(255, 255, 255));
						psrMain->DrawTriangle
						(
							rtTestTriangle2,
							Set(erfZ_BUFFER)
						);
					}
					break;
				case 1:
					if (bShow[0])
					{
						((CTexture*)rtTestTriangle.ptexTexture)->tpSolid
							= rtTestTriangle.ptexTexture->prasTexture->pixFromColour(CColour(255, 255, 255));
						psrMain->DrawTriangle
						(
							rtTestTriangle,
							Set(erfZ_BUFFER) + erfLIGHT_SHADE
						);
					}
					break;
				case 2:
					if (bShow[0])
						psrMain->DrawTriangle
						(
							rtTestTriangle,
							Set(erfZ_BUFFER) + erfTEXTURE
						);
					break;
				case 3:
					if (bShow[0])
						psrMain->DrawTriangle
						(
							rtTestTriangle,
							Set(erfZ_BUFFER) + erfTEXTURE + erfLIGHT_SHADE
						);
					break;
				default:
					Assert(0);
					break;
			}
			
			if (bShowPal)
			{
				psrMain->DrawPalette();
			}

			conStd.Show();
			conStd.CloseFileSession();
			break;
	}
};

class CCl
{
public:
	~CCl()
	{
		conStd.CloseFileSession();
	};
};

CCl Cl;

