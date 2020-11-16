/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Dialog box for displaying packed surfaces
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogTexturePack.hpp                                         $
 * 
 * 3     1/09/98 6:55p Rwyatt
 * Now shows the textures in the surface
 * 
 * 2     9/01/97 2:35p Rwyatt
 * Initial checkin after source safe was restored
 * 
 * 1     9/01/97 2:16p Rwyatt
 * First Implementation of dialog to display packed surfaces
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGTEXPACK_HPP
#define HEADER_GUIAPP_DIALOGTEXPACK_HPP

#include "GDIBitmap.hpp"
#include "Lib/Loader/TexturePackSurface.hpp"

//
// Class definitions.
//

//*********************************************************************************************
//
class CDialogTexturePack : public CDialog
//
// Texture pack settings dialog box.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CDialogTexturePack(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CDialogTexturePack)
	enum { IDD = IDD_TEXTUREPACK };
	int			i_Page;
	CListBox	m_TextureListBox;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogTexturePack)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	CGDIBitmap*	gdibmpSurface1;
	CGDIBitmap*	gdibmpSurface2;

	//*********************************************************************************************
	//
	void DrawTree(CDC& cdc,STextureQuadNode* ptqn_root,int i_xo,int i_yo);

	//*********************************************************************************************
	//
	void FillSubNodes(CDC& cdc, int i_xo, int i_yo, STextureQuadNode* ptqn,int i_level);


	//{{AFX_MSG(CDialogTexturePack)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	afx_msg void OnSelChangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
