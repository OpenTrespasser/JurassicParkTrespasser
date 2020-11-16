/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents: Implementation of 'TexturePack.hpp'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogTexturePack.cpp                                         $
 * 
 * 9     4/21/98 2:57p Rwyatt
 * Mip llevel is now shown in text form
 * 
 * 8     3/30/98 11:13p Rwyatt
 * Modified for new texture packer
 * 
 * 7     2/24/98 6:36p Rwyatt
 * Modified to use the new texture packer
 * 
 * 6     1/29/98 7:25p Rwyatt
 * Fixed mip level counter
 * 
 * 5     1/22/98 2:50p Pkeet
 * Added includes to accomodate the reduction in the number of includes in 'StdAfx.h.'
 * 
 * 4     1/14/98 6:27p Rwyatt
 * Now displays the mip level rather than the line number in the page mip level icon
 * 
 * 3     1/09/98 6:55p Rwyatt
 * Now shows the textures in the surface
 * 
 * 2     9/01/97 2:35p Rwyatt
 * Initial checkin after source safe was restored
 * 
 * 1     9/01/97 2:15p Rwyatt
 * First Implementation of a dialog to display packed surfaces
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "StdAfx.h"
#include "DialogTexturePack.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Loader/TextureManager.hpp"
#include "GUIApp.h"
#include "GUIAppDlg.h"
#include "GDIBitmap.hpp"


//*********************************************************************************************
//
// CDialogTexturePack implementation.
//
#define XORG1	16
#define YORG1	16

#define XORG2	16
#define YORG2	288


//*********************************************************************************************
//
// Message map for CDialogTexturePack.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogTexturePack, CDialog)
	//{{AFX_MSG_MAP(CDialogTexturePack)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_LBN_SELCHANGE(IDC_TEXTURE_BOX,		OnSelChangeList)
//	ON_BN_CLICKED(IDC_RADIO_LINEAR, OnRadioLinear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CDialogTexturePack constructor.
//

//*********************************************************************************************
CDialogTexturePack::CDialogTexturePack(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogTexturePack::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogTexturePack)
	i_Page = 0;
	//}}AFX_DATA_INIT

	gdibmpSurface1 = NULL;
	gdibmpSurface2 = NULL;
}


//*********************************************************************************************
//
// CDialogTexturePack member functions.
//

//*********************************************************************************************
//
void CDialogTexturePack::DoDataExchange(CDataExchange* pDX)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogTexturePack)
	DDX_Control(pDX, IDC_TEXTURE_BOX, m_TextureListBox);
	//}}AFX_DATA_MAP
}


//*********************************************************************************************
//
void CDialogTexturePack::OnShowWindow
(
	BOOL bShow,
	UINT nStatus
)
//
// Responds to the WM_SHOWWINDOW message by initializing dialog controls.
//
//**************************************
{
	CRenderTexturePackSurface**	aptps = gtxmTexMan.aptpsGetPageList();

	// Delete all strings in the list box because we are about to add them again...
	int i = m_TextureListBox.GetCount();

	int i_count = 0;

	while (i)
	{
		m_TextureListBox.DeleteString(i-1);
		i--;
	}

	for (i=0 ; i<MAX_TEXTURE_PAGES ; i++)
	{
		// this element in the texture page list is used so add it to the list box
		if (aptps[i])
		{
			char	buf[128];

			wsprintf(buf, "Texture Surface %d: %s-%d bit", i, aptps[i]->iGetPixelBits()==8?"Texture":"Bump", aptps[i]->iGetPixelBits() );
			int indx = m_TextureListBox.AddString(buf);

			// the item data of an entry is the index in the texture manager array
			m_TextureListBox.SetItemData(indx, i);

			i_count++;
		}
	}

	if (gdibmpSurface1)
		delete gdibmpSurface1;

	if (gdibmpSurface2)
		delete gdibmpSurface2;

	if (i_count)
	{
		char buf[64];

		m_TextureListBox.SetCurSel(0);
		i_Page = m_TextureListBox.GetItemData(0);

		gdibmpSurface1 = new CGDIBitmap(gtxmTexMan.aptpsGetPageList()[i_Page]->prasGetRaster(0));
		gdibmpSurface2 = new CGDIBitmap(gtxmTexMan.aptpsGetPageList()[i_Page]->prasGetRaster(1));

		wsprintf(buf,"%d",gtxmTexMan.aptpsGetPageList()[i_Page]->eptGetType());
		GetDlgItem(IDC_TEXT_MIP)->SetWindowText(buf);
	}
	else
	{
		i_Page = -1;
		gdibmpSurface1 = NULL;
		gdibmpSurface2 = NULL;
		GetDlgItem(IDC_TEXT_MIP)->SetWindowText("--");
	}

	// Call the base class member function.
	CDialog::OnShowWindow(bShow, nStatus);
}



//*********************************************************************************************
//
void CDialogTexturePack::OnPaint()
{
	if ( (!IsIconic()) && (i_Page>=0) )
	{
		CRenderTexturePackSurface*	aptps = gtxmTexMan.aptpsGetPageList()[i_Page];
		CClientDC					dc(this);

		CDialog::OnPaint();

		if (gdibmpSurface1)
			gdibmpSurface1->Draw(dc,XORG1,YORG1);
		if (gdibmpSurface2)
			gdibmpSurface2->Draw(dc,XORG2,YORG2);

		DrawTree(dc,&aptps->ppckPackedRaster(0)->ptqtGetPackQuadTree()->tqnRoot,XORG1,YORG1);
		DrawTree(dc,&aptps->ppckPackedRaster(1)->ptqtGetPackQuadTree()->tqnRoot,XORG2,YORG2);
	}
	else
	{
		CDialog::OnPaint();
	}	
}


//*********************************************************************************************
//
void CDialogTexturePack::DrawTree(CDC& cdc,STextureQuadNode* ptqn_root,int i_xo,int i_yo)
{
	FillSubNodes(cdc, i_xo,i_yo,ptqn_root,8);
}


//*********************************************************************************************
//
void CDialogTexturePack::FillSubNodes(CDC& cdc, int i_xo, int i_yo, STextureQuadNode* ptqn,int i_level)
{
	if (ptqn==NULL)
		return;

	RECT		rect;

	if (ptqn->u1Texture == 0)
	{
		if (i_level >0 )
		{
			// now we need to check the children so long as we are not at the bottom level
			FillSubNodes(cdc, i_xo, i_yo, ptqn->ptqnSubNode[0], i_level-1);
			FillSubNodes(cdc, i_xo, i_yo, ptqn->ptqnSubNode[1], i_level-1);
			FillSubNodes(cdc, i_xo, i_yo, ptqn->ptqnSubNode[2], i_level-1);
			FillSubNodes(cdc, i_xo, i_yo, ptqn->ptqnSubNode[3], i_level-1);
		}
	}
	else
	{
		// the node is used so fill it in with colour
		COLORREF	rgb;

		switch(i_level)
		{
		case	0:
			rgb = RGB(0xFF,0,0);
			break;

		case	1:
			rgb = RGB(0,0xFF,0);
			break;

		case	2:
			rgb = RGB(0,0,0xFF);
			break;

		case	3:
			rgb = RGB(0,0xFF,0xFF);
			break;

		case	4:
			rgb = RGB(0xFF,0,0xFF);
			break;

		case	5:
			rgb = RGB(0xFF,0xFF,0);
			break;

		case	6:
			rgb = RGB(0xFF,0xFF,0xFF);
			break;

		case	7:
			rgb = RGB(0x80,0x80,0xFF);
			break;

		case	8:
			rgb = RGB(0x80,0x80,0x80);
			break;
		}

		CBrush		br_boxes(rgb);

		rect.left	= i_xo + ptqn->u1XOrg;
		rect.right	= i_xo + (ptqn->u1XOrg) + (ptqn->u1Size);
		rect.top	= i_yo + ptqn->u1YOrg;
		rect.bottom	= i_yo + (ptqn->u1YOrg) + (ptqn->u1Size);

		cdc.FrameRect(&rect,&br_boxes);
	}
}



//*********************************************************************************************
//
void CDialogTexturePack::OnSelChangeList()
{
	int idx = m_TextureListBox.GetCurSel();

	i_Page = m_TextureListBox.GetItemData(idx);

	if (gdibmpSurface1)
		delete gdibmpSurface1;

	if (gdibmpSurface1)
		delete gdibmpSurface2;

	gdibmpSurface1 = new CGDIBitmap(gtxmTexMan.aptpsGetPageList()[i_Page]->prasGetRaster(0));
	gdibmpSurface2 = new CGDIBitmap(gtxmTexMan.aptpsGetPageList()[i_Page]->prasGetRaster(1));

	InvalidateRect(NULL);

	char buf[64];
	char str_mip[64];

	uint32 u4_mip = gtxmTexMan.aptpsGetPageList()[i_Page]->eptGetType();

	switch (u4_mip)
	{
	case 0:
		strcpy(str_mip,"Top Level");
		break;

	case 1:
		strcpy(str_mip,"Mip 1");
		break;

	case 2:
		strcpy(str_mip,"Mip 2");
		break;

	case 3:
		strcpy(str_mip,"Mip 3");
		break;

	case 4:
		strcpy(str_mip,"Mip 4");
		break;

	case 5:
		strcpy(str_mip,"Mip 5");
		break;

	case 6:
		strcpy(str_mip,"Mip 6");
		break;

	case -1:
		strcpy(str_mip,"Smallest (Non-Pageable)");
		break;

	case -2:
		strcpy(str_mip,"Smallest User Pack (Non-Pageable)");
		break;

	case -3:
		strcpy(str_mip,"Don't Care");
		break;

	case -4:
		strcpy(str_mip,"User Pack");
		break;

	case -5:
		strcpy(str_mip,"Curved Parent");
		break;

	default:
		strcpy(str_mip,"Unknown");
		break;
	}

	wsprintf(buf,"%s",str_mip);
	GetDlgItem(IDC_TEXT_MIP)->SetWindowText(buf);
}
