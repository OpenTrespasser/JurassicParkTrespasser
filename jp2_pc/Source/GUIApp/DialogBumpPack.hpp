/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Dialog box for controlling bump map packing
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogBumpPack.hpp                                            $
 * 
 * 6     8/30/98 4:50p Asouth
 * removed file scope
 * 
 * 5     7/03/98 4:12p Rwyatt
 * Made a little friendlier
 * 
 * 4     4/21/98 2:56p Rwyatt
 * Added a button for a non-pageable bump map page
 * 
 * 3     1/14/98 6:21p Rwyatt
 * First full working version
 * 
 * 2     1/09/98 6:55p Rwyatt
 * Checked in for safe keeping
 * 
 * 1     12/22/97 12:27a Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGBUMPPACK_HPP
#define HEADER_GUIAPP_DIALOGBUMPPACK_HPP

#include "afxwin.h"
#include "GDIBitmap.hpp"
#include "Lib/Loader/TextureManager.hpp"

#define MAP_ICONS	8

//
// Class definitions.
//

//*********************************************************************************************
//
class CDialogBumpPack : public CDialog
//
// Fog settings dialog box.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CDialogBumpPack(CWnd* pParent = NULL);
	~CDialogBumpPack();

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CDialogBumpPack)
	enum { IDD = IDD_DIALOG_BUMPPACK };
	CScrollBar	sbScrollMap;
	CListBox	lbBumpList;
	int32		i4RadioPage;		// radio button settings

	CString		strXSize;
	CString		strYSize;
	CString		strMipLevel;
	CString		strPacked;
	CString		strCurved;
	CString		strMinimum;
	CString		strMaximum;
	CString		strPackMip;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogBumpPack)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	int32		i4PageSelect;		// current selected bump map page or -1
	int32		i4Select;			// current selected map icon
	int32		i4BaseScroll;		// current scroll bar position
	uint32		u4MapCount;			// Number of maps in the vector
	TBumpList*	pblMaps;			// pointer to an STL vector
	CRect		rcIconPosition[MAP_ICONS];
	CRect		rcFrame;
	CRect		rcPack;
	CRect		rcState;
	CRect		rcShowRect;
	CGDIBitmap*	apgdibmpMaps[MAP_ICONS];
	CGDIBitmap*	pgdibmpPack;
	CGDIBitmap*	pgdibmpSelected;
	bool		bDropState;
	bool		bBlit;
	bool		bShowRect;
	bool		bDrop;
	int32		i4XBlit;
	int32		i4YBlit;
	rptr<CRaster> prasParent;
	rptr<CRaster> prasBump;
protected:

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_MSG(CDialogBumpPack)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnSel0();
	afx_msg void OnSel1();
	afx_msg void OnSel2();
	afx_msg void OnSel3();
	afx_msg void OnSel4();
	afx_msg void OnSel5();
	afx_msg void OnSel6();
	afx_msg void OnSel7();
	afx_msg void OnNewPage();
	afx_msg void OnNewSmallPage();
	afx_msg void OnSelChange();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnLeft();
	afx_msg void OnRight();
	afx_msg void OnRadioChange();
	afx_msg void ShowPacked();
	afx_msg void OnSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void NewBumpPage(ETexturePackTypes	ept);
	void SelectMap(int32 i4_map);
	void ShowTextStats();
	void SetPackMipText(ETexturePackTypes ept);
	void UpdateMapView(int32 i4_map);
	void UpdatePack();
	void InvalidatePackArea();
	void ClipSelected();
	void CreateIconPicture(int32 i4_icon, int32 i4_bump);
};


#endif
