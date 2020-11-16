/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents: Implementation of Bump Map Packing control dialog
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogBumpPack.cpp                                            $
 * 
 * 10    8/30/98 4:51p Asouth
 * loop variable re-scoped
 * 
 * 9     7/03/98 4:12p Rwyatt
 * Made a little friendlier
 * 
 * 8     7/02/98 7:11p Rwyatt
 * New VM allocation stratergy for curved bump maps
 * 
 * 7     4/21/98 2:56p Rwyatt
 * Added a button for a non-pageable bump map page
 * 
 * 6     2/24/98 6:36p Rwyatt
 * Modified to use the new texture packer
 * 
 * 5     2/11/98 4:35p Rwyatt
 * Can pack triangles on transparent backgrounds.
 * 
 * 4     1/29/98 7:25p Rwyatt
 * Fixed memory leaks
 * fixed some bug when there was no bump maps
 * Works with 16 bit textures
 * 
 * 3     1/19/98 7:30p Pkeet
 * Added support for 16 bit bumpmaps by adding lower colour resolution and a smaller size for
 * 'CBumpAnglePair.'
 * 
 * 2     1/14/98 6:21p Rwyatt
 * First full working version
 * 
 * 1     12/22/97 12:27a Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/


//
// Includes.
//
#include "StdAfx.h"
#include "DialogBumpPack.hpp"
#include "GUITools.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/View/Clut.hpp"
#include "Lib/Renderer/Primitives/FastBump.hpp"
#include "Lib/Sys/MemoryLog.hpp"


//*********************************************************************************************
//
// CDialogBumpPack implementation.
//
static int	i4SelectIconID[MAP_ICONS] = {IDC_MAP0, IDC_MAP1, IDC_MAP2, IDC_MAP3,
										IDC_MAP4, IDC_MAP5, IDC_MAP6, IDC_MAP7};

static bool bCheckDropPosition(rptr<CRaster> pras_pack,	rptr<CRaster> pras_src, int32 i4_xpos, int32 i4_ypos);


//*********************************************************************************************
//
// Message map for CDialogBumpPack.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogBumpPack, CDialog)
	//{{AFX_MSG_MAP(CDialogBumpPack)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelChange)
	ON_BN_CLICKED(IDC_RADIO1, OnRadioChange)
	ON_BN_CLICKED(IDC_RADIO2, OnRadioChange)
	ON_BN_CLICKED(IDC_NEW_SURFACE2, OnNewPage)
	ON_BN_CLICKED(IDC_NEW_SURFACE3, OnNewSmallPage)

	ON_BN_CLICKED(IDC_MAP0, OnSel0)
	ON_BN_CLICKED(IDC_MAP1, OnSel1)
	ON_BN_CLICKED(IDC_MAP2, OnSel2)
	ON_BN_CLICKED(IDC_MAP3, OnSel3)
	ON_BN_CLICKED(IDC_MAP4, OnSel4)
	ON_BN_CLICKED(IDC_MAP5, OnSel5)
	ON_BN_CLICKED(IDC_MAP6, OnSel6)
	ON_BN_CLICKED(IDC_MAP7, OnSel7)

	ON_BN_CLICKED(IDC_SET, OnSet)

	ON_BN_CLICKED(IDC_LEFT, OnLeft)
	ON_BN_CLICKED(IDC_RIGHT, OnRight)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CDialogBumpPack constructor.
//

//*********************************************************************************************
CDialogBumpPack::CDialogBumpPack(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogBumpPack::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogBumpPack)
	i4RadioPage = 0;
	strPackMip	= _T("-");
	strXSize	= _T("");
	strYSize	= _T("");
	strMipLevel	= _T("");
	strPacked	= _T("");
	strCurved	= _T("");
	strMinimum	= _T("");
	strMaximum	= _T("");
	//}}AFX_DATA_INIT
}

//*********************************************************************************************
CDialogBumpPack::~CDialogBumpPack()
{
	delete pgdibmpPack;
	delete pgdibmpSelected;
	for (int i = 0; i<MAP_ICONS; i++)
	{
		delete apgdibmpMaps[i];
	}
}


//*********************************************************************************************
//
// CDialogBumpPack member functions.
//

//*********************************************************************************************
//
void CDialogBumpPack::DoDataExchange(CDataExchange* pDX)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogBumpPack)
	DDX_Control(pDX,IDC_LIST1,lbBumpList);
	DDX_Control(pDX,IDC_SCROLLBAR1,sbScrollMap);
	DDX_Radio(pDX,IDC_RADIO1, i4RadioPage);

	DDX_Text(pDX,IDC_TEXT_XSIZE2, strPackMip);
	DDX_Text(pDX,IDC_TEXT_XSIZE, strXSize);
	DDX_Text(pDX,IDC_TEXT_YSIZE, strYSize);
	DDX_Text(pDX,IDC_TEXT_MIPLEVEL, strMipLevel);
	DDX_Text(pDX,IDC_TEXT_PACKED, strPacked);
	DDX_Text(pDX,IDC_TEXT_CURVED, strCurved);
	DDX_Text(pDX,IDC_TEXT_MINIMUM, strMinimum);
	DDX_Text(pDX,IDC_TEXT_MAXIMUM, strMaximum);

	//}}AFX_DATA_MAP
}

//*********************************************************************************************
//
void CDialogBumpPack::OnHScroll
(
	UINT        nSBCode,
	UINT        nPos,
	CScrollBar* pScrollBar
)
//
// Responds to the WM_HSCROLL message by moving one or more scrollbars and updating colour
// information accordingly.
//
//**************************************
{
	char buf[64];

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	MoveScrollbar(nSBCode, nPos, pScrollBar);

	int32	i4_pos = sbScrollMap.GetScrollPos();
	int32	i4_diff = i4_pos - i4BaseScroll;			// the number of maps that need to be drawn

	int32	i4_off = i4_diff;

	i4_diff = abs(i4_diff);

	if (i4_diff > MAP_ICONS)
		i4_diff = MAP_ICONS;

	int32	i4_update;

	if (i4_off >0)
	{
		i4_update = i4_pos + MAP_ICONS - i4_diff;	// position to start updating for +ve
	}
	else
	{
		i4_update = i4BaseScroll - i4_diff;	// position to start updating for -ve
	}

	while (i4_diff>0)
	{
		CreateIconPicture(i4_update & 7,i4_update);
		i4_update++;
		i4_diff--;
	}

	i4BaseScroll = i4_pos;

	wsprintf(buf,"%d",i4BaseScroll);
	strMinimum = buf;

	wsprintf(buf,"%d",i4BaseScroll+MAP_ICONS-1);
	strMaximum = buf;

	if (i4Select<0)
	{
		Invalidate(false);
	}
	else
	{
		Invalidate();
		// delete the selected DDB
	}

	delete pgdibmpSelected;
	pgdibmpSelected = NULL;

	i4Select = -1;
	bBlit = false;
	bShowRect = false;

	ShowTextStats();
}



//*********************************************************************************************
//
void CDialogBumpPack::OnShowWindow
(
	BOOL bShow,
	UINT nStatus
)
//
// Responds to the WM_SHOWWINDOW message by initializing dialog controls.
//
//**************************************
{
	char buf[64];
	// Call the base class member function.
	CDialog::OnShowWindow(bShow, nStatus);

	CRenderTexturePackSurface**	aptps = gtxmTexMan.aptpsGetPageList();

	prasParent = rptr0;
	prasBump = rptr0;

	i4PageSelect = -1;
	i4Select = -1;
	i4BaseScroll = 0;
	bShowRect = false;
	bBlit = false;
	pgdibmpSelected = NULL;
	bDropState = false;
	bDrop = false;
	GetDlgItem(IDC_SET)->EnableWindow(false);

	pblMaps = gtxmTexMan.pblGetBumpMaps();
	if (pblMaps)
	{
		u4MapCount = (*pblMaps).size();
		GetDlgItem(IDC_SCROLLBAR1)->EnableWindow(true);
		sbScrollMap.SetScrollRange(0,u4MapCount-MAP_ICONS);
	}
	else
	{
		u4MapCount = 0;
		GetDlgItem(IDC_SCROLLBAR1)->EnableWindow(false);
		GetDlgItem(IDC_SET)->EnableWindow(false);
		GetDlgItem(IDC_NEW_SURFACE2)->EnableWindow(false);
		GetDlgItem(IDC_NEW_SURFACE3)->EnableWindow(false);
	}

	// Delete all strings in the list box because we are about to add them again...
	int i = lbBumpList.GetCount();

	int i_count = 0;

	while (i)
	{
		lbBumpList.DeleteString(i-1);
		i--;
	}

	bool b_found = false;

	for (i=0 ; i<MAX_TEXTURE_PAGES ; i++)
	{
		// this element in the texture page list is used so add it to the list box
		if (aptps[i])
		{

			if (aptps[i]->iGetPixelBits() == iBUMPMAP_RESOLUTION)
			{
				wsprintf(buf, "Texture Surface %d", i );
				int indx = lbBumpList.AddString(buf);

				// the item data of an entry is the index in the texture manager array
				lbBumpList.SetItemData(indx, i);
				b_found = true;
			}
			i_count++;
		}
	}

	if (b_found)
	{
		lbBumpList.SetCurSel(0);
		i4PageSelect = lbBumpList.GetItemData(0);
		pgdibmpPack = new CGDIBitmap(aptps[ i4PageSelect ]->prasGetRaster(0));
		prasParent = aptps[ i4PageSelect ]->prasGetRaster(0);
		GetDlgItem(IDC_RADIO1)->EnableWindow(true);
		GetDlgItem(IDC_RADIO2)->EnableWindow(true);
		GetDlgItem(IDC_LIST1)->EnableWindow(true);

		SetPackMipText( aptps[ i4PageSelect ]->eptGetType() );
		//wsprintf(buf,"%d",aptps[ i4PageSelect ]->eptGetType( ) );
		//strPackMip = buf;
	}
	else
	{
		pgdibmpPack = NULL;
		GetDlgItem(IDC_RADIO1)->EnableWindow(false);
		GetDlgItem(IDC_RADIO2)->EnableWindow(false);
		GetDlgItem(IDC_LIST1)->EnableWindow(false);
	}

	//
	// Get the localtion of all important icons
	//
	WINDOWPLACEMENT	wndplc;
	GetDlgItem(IDC_PACK_FRAME)->GetWindowPlacement(&wndplc);
	rcFrame = wndplc.rcNormalPosition;

	rcPack.left = rcFrame.left + (((rcFrame.right - rcFrame.left) - 256)/2);
	rcPack.top = rcFrame.top + (((rcFrame.bottom - rcFrame.top) - 256)/2);

	GetDlgItem(IDC_STATE)->GetWindowPlacement(&wndplc);
	rcState = wndplc.rcNormalPosition;
	rcState.left+=2;
	rcState.top+=2;
	rcState.right-=2;
	rcState.bottom-=2;

	for (i = 0; i<MAP_ICONS; i++)
	{
		GetDlgItem(i4SelectIconID[i])->GetWindowPlacement(&wndplc);
		rcIconPosition[i] = wndplc.rcNormalPosition;
		rcIconPosition[i].left +=2;
		rcIconPosition[i].top +=2;
		rcIconPosition[i].right -=2;
		rcIconPosition[i].bottom -=2;
	}

	//
	// make the initial set of picture icons
	//
	for (i = 0; i<MAP_ICONS; i++)
	{
		if (i+i4BaseScroll>=(int32)u4MapCount)
		{
			apgdibmpMaps[i] = NULL;
		}
		else
		{
			apgdibmpMaps[i] = NULL;
			CreateIconPicture(i,i+i4BaseScroll);
		}
	}

	wsprintf(buf,"%d",i4BaseScroll);
	strMinimum = buf;

	if (i4BaseScroll+MAP_ICONS<=(int32)u4MapCount)
		wsprintf(buf,"%d",i4BaseScroll+MAP_ICONS-1);
	else
		wsprintf(buf,"%d",u4MapCount-1);

	strMaximum = buf;

	UpdateData(false);
}



//*********************************************************************************************
//
void CDialogBumpPack::OnPaint
(
)
//
// Responds to the WM_PAINT
//
//**************************************
{
	char	buf[128];
	RECT	rc_update;
	int		i;

	if (!GetUpdateRect(&rc_update))
		return;

	CPaintDC	dc(this);
	CBrush		blue(RGB(0,0,255));
	CBrush		green(RGB(0,255,0));
	CPen		red(PS_SOLID, 2, RGB(255,0,0));

	//
	// Paint the pack surface
	//
	if (pgdibmpPack)
	{
		pgdibmpPack->Draw(dc,rcPack.left,rcPack.top);
	}

	if (bShowRect)
	{
		dc.FrameRect(&rcShowRect, &green );
	}

	if ((bBlit) && (pgdibmpPack))
	{
		if (pgdibmpSelected->Draw(dc,i4XBlit,i4YBlit,false) == edsNotClipped )
		{
			if (bCheckDropPosition(prasParent, prasBump,i4XBlit-rcPack.left, i4YBlit-rcPack.top))
			{
				dc.FillSolidRect(&rcState, RGB(0,255,0) );
			}
			else
			{
				dc.FillSolidRect(&rcState, RGB(255,0,0) );
			}
			bDropState = true;
			if (!bDrop)
			{
				GetDlgItem(IDC_SET)->EnableWindow(true);
				bDrop = true;
			}
		}
		else
		{
			dc.FillSolidRect(&rcState, RGB(255,0,0) );
			if (bDrop)
			{
				GetDlgItem(IDC_SET)->EnableWindow(false);
				bDrop = false;
			}
			bDropState = false;
		}

		wsprintf(buf,"(%d,%d)",i4XBlit-rcPack.left, i4YBlit-rcPack.top);
		GetDlgItem(IDC_TEXT_POSITION)->SetWindowText(buf);
	}
	else
	{
		dc.FillSolidRect(&rcState, RGB(128,128,128) );
		bDropState = false;
		GetDlgItem(IDC_TEXT_POSITION)->SetWindowText("");
	}

	i = 0;
	int32 i4_icon = i4BaseScroll & 7;

	if ((rc_update.top<=rcIconPosition[0].bottom) && (rc_update.bottom>=rcIconPosition[0].top))
	{
		while (i<MAP_ICONS)
		{
			if (apgdibmpMaps[i4_icon])
			{
				int32 i4_icon_w = rcIconPosition[i].right - rcIconPosition[i].left;
				int32 i4_icon_h = rcIconPosition[i].bottom - rcIconPosition[i].top;

				// if our bmp is smaller than the icon, fill the bakcground with gray
				if ((i4_icon_w > apgdibmpMaps[i4_icon]->i4Width) || (i4_icon_h > apgdibmpMaps[i4_icon]->i4Height))
				{
					dc.FillSolidRect(&rcIconPosition[i], RGB(128,128,128) );
				}

				int32 i4_xp = rcIconPosition[i].left + ((i4_icon_w - apgdibmpMaps[i4_icon]->i4Width)/2);
				int32 i4_yp = rcIconPosition[i].top + ((i4_icon_h - apgdibmpMaps[i4_icon]->i4Height)/2);

				apgdibmpMaps[i4_icon]->Draw(dc, i4_xp, i4_yp);

				// if this map is packed, cross it out
				if ( (*pblMaps)[i+i4BaseScroll].bPacked )
				{
					CPen* old = dc.SelectObject(&red);

					dc.MoveTo(rcIconPosition[i].left,rcIconPosition[i].top);
					dc.LineTo(rcIconPosition[i].right,rcIconPosition[i].bottom);
					dc.MoveTo(rcIconPosition[i].right,rcIconPosition[i].top);
					dc.LineTo(rcIconPosition[i].left,rcIconPosition[i].bottom);

					dc.SelectObject(old);
				}
			}
			else
			{
				dc.FillSolidRect(&rcIconPosition[i], RGB(0,0,0) );
			}

			i++;
			i4_icon++;
			i4_icon &= 7;
		}
	}

	if (i4Select>=0)
	{
		RECT	rc = rcIconPosition[i4Select];

		rc.left -= 5;
		rc.top -= 5;
		rc.right += 4;
		rc.bottom += 4;

		dc.FrameRect(&rc, &blue);
	}

	CDialog::OnPaint();
}



//*********************************************************************************************
//
void CDialogBumpPack::OnSelChange
(
)
//
// Responds to the list box selection being chnaged
//
//**************************************
{
	int32	i4_sel = lbBumpList.GetCurSel();
	CRenderTexturePackSurface**	aptps = gtxmTexMan.aptpsGetPageList();

	bShowRect = false;
	bBlit = false;
	InvalidateRect(&rcState,false);
	i4PageSelect = i4_sel;

	delete pgdibmpPack;
	pgdibmpPack = new CGDIBitmap(aptps[ lbBumpList.GetItemData(i4_sel) ]->prasGetRaster(i4RadioPage));

	prasParent = aptps[ lbBumpList.GetItemData(i4_sel) ]->prasGetRaster(i4RadioPage);

	SetPackMipText( aptps[ lbBumpList.GetItemData(i4_sel) ]->eptGetType() );
	//wsprintf(buf,"%d",aptps[ lbBumpList.GetItemData(i4_sel) ]->eptGetType( ) );
	//strPackMip = buf;

	InvalidatePackArea();
	UpdateData(false);
}


//*********************************************************************************************
//
void CDialogBumpPack::OnRadioChange
(
)
//
// Responds to the page radio buttons being clicked
//
//**************************************
{
	// get the radio state
	UpdateData(true);

	int32	i4_sel = lbBumpList.GetCurSel();
	CRenderTexturePackSurface**	aptps = gtxmTexMan.aptpsGetPageList();

	delete pgdibmpPack;
	pgdibmpPack = new CGDIBitmap(aptps[ lbBumpList.GetItemData(i4_sel) ]->prasGetRaster(i4RadioPage));
	prasParent = aptps[ lbBumpList.GetItemData(i4_sel) ]->prasGetRaster(i4RadioPage);

	bShowRect = false;
	bBlit = false;
	InvalidateRect(&rcState,false);
	InvalidatePackArea();
}


//*********************************************************************************************
//
void CDialogBumpPack::InvalidatePackArea
(
)
//
// Invlaidate the window in the area of the pakced surface
//
//**************************************
{
	InvalidateRect(&rcFrame,false);
}


//*********************************************************************************************
//
void CDialogBumpPack::OnNewPage
(
)
//
// Creates a new bump map pack page
//
//**************************************
{
	NewBumpPage(eptUSER_PACK);
}


//*********************************************************************************************
//
void CDialogBumpPack::OnNewSmallPage
(
)
//
// Creates a new bump map pack page that is not pageable
//
//**************************************
{
	NewBumpPage(eptSMALLEST_USER_PACK);
}


//*********************************************************************************************
//
void CDialogBumpPack::NewBumpPage
(
	ETexturePackTypes	ept
)
//
// Creates a new bump map pack page that is not pageable
//
//**************************************
{
	char	buf[64];
	int32	i4_page = gtxmTexMan.i4CreateNewBumpPage();
	CRenderTexturePackSurface**	aptps = gtxmTexMan.aptpsGetPageList();

	aptps[ i4_page ]->SetType( ept );

	wsprintf(buf, "Texture Surface %d", i4_page );
	int indx = lbBumpList.AddString(buf);

	if (i4PageSelect<0)
	{
		// we currently have no bump pages
		GetDlgItem(IDC_RADIO1)->EnableWindow(true);
		GetDlgItem(IDC_RADIO2)->EnableWindow(true);
		GetDlgItem(IDC_LIST1)->EnableWindow(true);

		SetPackMipText(aptps[ i4_page ]->eptGetType() );
		//wsprintf(buf,"%d",aptps[ i4_page ]->eptGetType( ) );
		//strPackMip = buf;
	}

	i4PageSelect = i4_page;


	// the item data of an entry is the index in the texture manager array
	lbBumpList.SetItemData(indx, i4_page);
	lbBumpList.SetCurSel(indx);
	OnSelChange();
}


//*********************************************************************************************
//
void CDialogBumpPack::OnSel0
(
)
//
// Seletcs a map icon
//
//**************************************
{
	SelectMap(0);
}

//*********************************************************************************************
//
void CDialogBumpPack::OnSel1
(
)
//
// Seletcs a map icon
//
//**************************************
{
	SelectMap(1);
}


//*********************************************************************************************
//
void CDialogBumpPack::OnSel2
(
)
//
// Seletcs a map icon
//
//**************************************
{
	SelectMap(2);
}


//*********************************************************************************************
//
void CDialogBumpPack::OnSel3
(
)
//
// Seletcs a map icon
//
//**************************************
{
	SelectMap(3);
}


//*********************************************************************************************
//
void CDialogBumpPack::OnSel4
(
)
//
// Seletcs a map icon
//
//**************************************
{
	SelectMap(4);
}


//*********************************************************************************************
//
void CDialogBumpPack::OnSel5
(
)
//
// Seletcs a map icon
//
//**************************************
{
	SelectMap(5);
}


//*********************************************************************************************
//
void CDialogBumpPack::OnSel6
(
)
//
// Seletcs a map icon
//
//**************************************
{
	SelectMap(6);
}


//*********************************************************************************************
//
void CDialogBumpPack::OnSel7
(
)
//
// Seletcs a map icon
//
//**************************************
{
	SelectMap(7);
}


//*********************************************************************************************
//
void CDialogBumpPack::OnLeft
(
)
//
// Move selected bump map around pack surface
//
//**************************************
{
	if (bBlit)
	{
		i4XBlit -= 1;
		InvalidatePackArea();
		InvalidateRect(&rcState,false);
	}
}


//*********************************************************************************************
//
void CDialogBumpPack::OnRight
(
)
//
// Move selected bump map around pack surface
//
//**************************************
{
	if (bBlit)
	{
		i4XBlit += 1;
		InvalidatePackArea();
		InvalidateRect(&rcState,false);
	}
}


//*********************************************************************************************
//
void CDialogBumpPack::OnUp
(
)
//
// Move selected bump map around pack surface
//
//**************************************
{
	if (bBlit)
	{
		i4YBlit -= 1;
		InvalidatePackArea();
		InvalidateRect(&rcState,false);
	}
}


//*********************************************************************************************
//
void CDialogBumpPack::OnDown
(
)
//
// Move selected bump map around pack surface
//
//**************************************
{
	if (bBlit)
	{
		i4YBlit += 1;
		InvalidatePackArea();
		InvalidateRect(&rcState,false);
	}
}


//*********************************************************************************************
//
void CDialogBumpPack::OnSet
(
)
//
// set the current pack position
//
//**************************************
{
	if ((pgdibmpSelected == NULL) || (i4Select<0) || (!bBlit))
		return;

	CRenderTexturePackSurface**	aptps = gtxmTexMan.aptpsGetPageList();

	if ( (*pblMaps)[i4BaseScroll + i4Select].eptMipLevel == eptSMALLEST )
	{
		// If the texture is a smallest mip the page must also be of type eptSMALLEST
		if ( ( aptps[ lbBumpList.GetItemData(lbBumpList.GetCurSel()) ]->eptGetType() != eptSMALLEST ) &&
			( aptps[ lbBumpList.GetItemData(lbBumpList.GetCurSel()) ]->eptGetType() != eptSMALLEST_USER_PACK ))
		{
			MessageBox("The Smallest mip maps must be in non-pageable texture pages.", "Pack Error", MB_OK);
			return;
		}
	}

	rptr<CRaster>	pras_pack = aptps[ lbBumpList.GetItemData(lbBumpList.GetCurSel())]->prasGetRaster(i4RadioPage);

	SRect rc(i4XBlit-rcPack.left, i4YBlit-rcPack.top, (*pblMaps)[i4Select+i4BaseScroll].prasBumpMap->iWidth, (*pblMaps)[i4Select+i4BaseScroll].prasBumpMap->iHeight);

	MEMLOG_SUB_COUNTER(emlBumpNoPack,1);
	MEMLOG_SUB_COUNTER(emlBumpNoPackMem,
				(*pblMaps)[i4Select+i4BaseScroll].prasBumpMap->iLineBytes()*(*pblMaps)[i4Select+i4BaseScroll].prasBumpMap->iHeight);


	rptr<CRasterMem> prasmem = rptr_cast(CRasterMem, rptr_new CRasterMem
												(
													pras_pack,
													rc,
													aptps[ lbBumpList.GetItemData(lbBumpList.GetCurSel()) ]->ppckPackedRaster(i4RadioPage), 
													&(*pblMaps)[i4Select+i4BaseScroll].prasBumpMap->pxf) 
												);

	(*pblMaps)[i4Select+i4BaseScroll].prasBumpMap->ChangeRaster( prasmem, true );

	// mark this element as packed.
	(*pblMaps)[i4Select+i4BaseScroll].bPacked = true;

	bBlit = false;
	delete pgdibmpSelected;
	pgdibmpSelected = NULL;

	UpdateData(false);
	OnSelChange();
	SelectMap(i4Select);
	Invalidate();
}


//*********************************************************************************************
//
void CDialogBumpPack::OnMouseMove( UINT nFlags, CPoint pos)
//
// Mouse has moved
//
//**************************************
{
	if (!rcFrame.PtInRect(pos))
		return;

	if (pgdibmpSelected == NULL)
		return;

	if (pgdibmpPack == NULL)
		return;

	if (nFlags & MK_LBUTTON)
	{
		bBlit = true;
		i4XBlit = pos.x - (pgdibmpSelected->i4Width/2);
		i4YBlit = pos.y - (pgdibmpSelected->i4Height/2);;
		InvalidatePackArea();
		InvalidateRect(&rcState,false);
	}
}


//*********************************************************************************************
//
void CDialogBumpPack::ShowPacked
(
)
//
// shows a pack position
//
//**************************************
{
	int32	i4_bump = i4BaseScroll + i4Select;

	// if the current bump is not packed, we cannot select it
	if ( !(*pblMaps)[i4_bump].bPacked )
		return;

	// if the stride is not 512 pixels then the map is not packed in a page, it is just in VM
	if ( (*pblMaps)[i4_bump].prasBumpMap->iLinePixels != 512)
		return;

	rptr<CRasterMem> prasmem_bump = rptr_static_cast(CRasterMem, (*pblMaps)[i4_bump].prasBumpMap );


	// get the parent packed raster of this bump map
	rptr<CPackedRaster>	ppck = prasmem_bump->ppckrParent;
	CRenderTexturePackSurface**	aptps = gtxmTexMan.aptpsGetPageList();

	bool b_found = false;
	uint32	u4_page;

	int i_surface;
	for (i_surface=0 ; i_surface<MAX_TEXTURE_PAGES ; i_surface++)
	{
		if (aptps[i_surface]->ppckPackedRaster(0) == ppck)
		{
			u4_page = 0;
			b_found = true;
			break;
		}

		if (aptps[i_surface]->ppckPackedRaster(1) == ppck)
		{
			u4_page = 1;
			b_found = true;
			break;
		}
	}

	// i_surface is the element in the global pack list of our surface.
	Assert(b_found);

	// all packed surfaces must have a parent.
	Assert(ppck);

	void*	p_parent = (void*) prasmem_bump->prasReferenced->pSurface;
	void*	p_tex    = (void*) prasmem_bump->pSurface;

	Assert(p_parent);
	Assert(p_tex);

	void*	p_surface0 = aptps[ i_surface ]->prasGetRaster(0)->pSurface;
	void*	p_surface1 = aptps[ i_surface ]->prasGetRaster(1)->pSurface;
	uint32	u4_bytes   = aptps[ i_surface ]->prasGetRaster(0)->iPixelBits/8;

	uint32	u4_texture = ((uint8*)p_tex) - ((uint8*)p_parent);
	u4_texture /= u4_bytes;

	int i_map_count = lbBumpList.GetCount();
	int i_element;
	while (i_map_count)
	{
		i_element = lbBumpList.GetItemData(i_map_count-1);
		if (i_element == i_surface)
			break;
		i_map_count--;
	}

	lbBumpList.SetCurSel(i_map_count-1);
	i4RadioPage = (int)u4_page;
	OnSelChange();

	rcShowRect.left		= rcPack.left + (u4_texture & 255);
	rcShowRect.top		= rcPack.top + (u4_texture >> 9);
	rcShowRect.right	= rcShowRect.left + prasmem_bump->iWidth;
	rcShowRect.bottom	= rcShowRect.top + prasmem_bump->iHeight;

	bShowRect = true;
	InvalidatePackArea();

	// delete the selected DDB
	delete pgdibmpSelected;
	pgdibmpSelected = NULL;
}



//*********************************************************************************************
//
void CDialogBumpPack::SelectMap
(
	int32	i4_map
)
//
// Selects the specifed map icon
//
//**************************************
{
	RECT	rc;

	if (i4_map + i4BaseScroll >= (int32)u4MapCount)
		return;

	if (bShowRect)
	{
		bShowRect = false;
		InvalidatePackArea();
	}

	int32 i4_old = i4Select;

	i4Select = i4_map;
	ShowTextStats();

	if (i4_old>=0)
	{
		// deselect the old map
		rc = rcIconPosition[i4_old];
		rc.left -=8;
		rc.top -=8;
		rc.right +=8;
		rc.bottom +=8;
		InvalidateRect(&rc,true);
	}

	rc = rcIconPosition[i4Select];
	rc.left -=8;
	rc.top -=8;
	rc.right +=8;
	rc.bottom +=8;
	InvalidateRect(&rc,false);

	bBlit = false;

	prasBump = (*pblMaps)[i4Select+i4BaseScroll].prasBumpMap;

	if ((*pblMaps)[i4Select+i4BaseScroll].bPacked)
	{
		ShowPacked();
	}
	else
	{
		// map is not packed so make a DDB for this icon
		delete pgdibmpSelected;
		pgdibmpSelected = new CGDIBitmap( prasBump );
		pgdibmpSelected->SetClip(rcPack.left,rcPack.top,256,256);
		i4XBlit = rcPack.left;
		i4YBlit = rcPack.top;

		InvalidatePackArea();
	}

	InvalidateRect(&rcState,false);
}

//*********************************************************************************************
//
void CDialogBumpPack::ShowTextStats
(
)
//
// Shows the stats for the current bump
//
//**************************************
{
	char	buf[128];
	int32	i4 = i4BaseScroll + i4Select;

	if (i4Select<0)
	{
		strXSize	= _T("");
		strYSize	= _T("");
		strMipLevel	= _T("");
		strPacked	= _T("");
		strCurved	= _T("");
		UpdateData(false);
	}
	else
	{
		wsprintf(buf,"%d", (*pblMaps)[i4].prasBumpMap->iWidth);
		strXSize = buf;
		wsprintf(buf,"%d", (*pblMaps)[i4].prasBumpMap->iHeight);
		strYSize = buf;

		switch ( (*pblMaps)[i4].eptMipLevel )
		{
		case 0:
			strcpy(buf,"Top Level");
			break;

		case 1:
			strcpy(buf,"Mip 1");
			break;

		case 2:
			strcpy(buf,"Mip 2");
			break;

		case 3:
			strcpy(buf,"Mip 3");
			break;

		case 4:
			strcpy(buf,"Mip 4");
			break;

		case 5:
			strcpy(buf,"Mip 5");
			break;

		case 6:
			strcpy(buf,"Mip 6");
			break;

		case -1:
			strcpy(buf,"Smallest (Non-Pageable)");
			break;

		case -2:
			strcpy(buf,"Smallest User Pack (Non-Pageable)");
			break;

		case -3:
			strcpy(buf,"Don't Care");
			break;

		case -4:
			strcpy(buf,"User Pack");
			break;

		case -5:
			strcpy(buf,"Curved Parent");
			break;

		default:
			strcpy(buf,"Unknown");
			break;
		}


		strMipLevel = buf;
		if ((*pblMaps)[i4].bPacked)
		{
			if ((*pblMaps)[i4].prasBumpMap->iLinePixels == 512)
				strPacked = "Yes";
			else
				strPacked = "Copied";
		}
		else
		{
			strPacked = "No";
		}

		if ((*pblMaps)[i4].bCurved)
		{
			strCurved = "Yes";
		}
		else
		{
			strCurved = "No";
		}

		UpdateData(false);
	}
}

//*********************************************************************************************
//
void CDialogBumpPack::SetPackMipText
(
	ETexturePackTypes	ept
)
//**************************************
{
	char buf[1024];

	switch ( (uint32)ept )
	{
	case 0:
		strcpy(buf,"Top Level");
		break;

	case 1:
		strcpy(buf,"Mip 1");
		break;

	case 2:
		strcpy(buf,"Mip 2");
		break;

	case 3:
		strcpy(buf,"Mip 3");
		break;

	case 4:
		strcpy(buf,"Mip 4");
		break;

	case 5:
		strcpy(buf,"Mip 5");
		break;

	case 6:
		strcpy(buf,"Mip 6");
		break;

	case -1:
		strcpy(buf,"Smallest (Non-Pageable)");
		break;

	case -2:
		strcpy(buf,"Smallest User (Non-Pageable)");
		break;

	case -3:
		strcpy(buf,"Don't Care");
		break;

	case -4:
		strcpy(buf,"User (Pageable)");
		break;

	case -5:
		strcpy(buf,"Curved Parent");
		break;

	default:
		strcpy(buf,"Unknown");
		break;
	}


	strPackMip = buf;
}


//*********************************************************************************************
//
void CDialogBumpPack::CreateIconPicture
(
	int32	i4_icon,
	int32	i4_bump
)
//
// Creates a new DDB for the given icon and bump map
//
//**************************************
{
	int32 i4_icon_w = rcIconPosition[i4_icon].right - rcIconPosition[i4_icon].left;
	int32 i4_icon_h = rcIconPosition[i4_icon].bottom - rcIconPosition[i4_icon].top;
	int32 i4_w,i4_h;

	if ((*pblMaps)[i4_bump].prasBumpMap->iWidth>=i4_icon_w)
	{
		i4_w = i4_icon_w;
	}
	else
	{
		i4_w = (*pblMaps)[i4_bump].prasBumpMap->iWidth;
	}

	if ((*pblMaps)[i4_bump].prasBumpMap->iHeight>=i4_icon_h)
	{
		i4_h = i4_icon_h;
	}
	else
	{
		i4_h = (*pblMaps)[i4_bump].prasBumpMap->iHeight;
	}

	delete apgdibmpMaps[i4_icon];
	apgdibmpMaps[i4_icon] = new CGDIBitmap((*pblMaps)[i4_bump].prasBumpMap, i4_w, i4_h);
}


//*********************************************************************************************
//
static bool bCheckDropPosition
(
	rptr<CRaster>	pras_pack,
	rptr<CRaster>	pras_src,
	int32			i4_xpos,
	int32			i4_ypos
)
//
// Creates a new DDB for the given icon and bump map
//
//**************************************
{
	CBumpAnglePair	bang_src;
	CBumpAnglePair	bang_pack;

	int32			i4_w = pras_src->iWidth;
	int32			i4_h = pras_src->iHeight;
	int32			i4_xloc = i4_xpos;
	int32			i4_yloc = i4_ypos;

	for (int32 i4_y = 0; i4_y<i4_h ;i4_y++)
	{
		for (int32 i4_x = 0; i4_x<i4_w ;i4_x++)
		{
			bang_src.br = pras_src->pixGet(i4_x, i4_y);

			// if this pixel is zero, ignore it
			if (bang_src.u1GetColour() == 0)
				continue;

			bang_pack.br = pras_pack->pixGet(i4_x + i4_xloc, i4_y + i4_yloc);
			if (bang_pack.u1GetColour() != 0)
				return false;
		}
	}

	return true;
}