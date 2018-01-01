/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of "DialogMaterial.h."
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogMaterial.cpp                                            $
 * 
 * 3     98/07/22 14:47 Speter
 * Now instance material when modified. Prevents clut errors with shared materials.
 * 
 * 2     98/04/22 17:04 Speter
 * Fixed a windowing bug.
 * 
 * 1     98/04/22 12:26 Speter
 * 
 *********************************************************************************************/

#include "stdafx.h"
#include "guiapp.h"
#include "DialogMaterial.h"

#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/View/Raster.hpp"

/////////////////////////////////////////////////////////////////////////////
// CDialogMaterial dialog


CDialogMaterial::CDialogMaterial(CWnd* pParent)
	: CDialog(CDialogMaterial::IDD, pParent),

	// Control init.
	ctspiniIndex		(this, IDC_SPIN_MATERIAL_INDEX,	&iMaterialIndex,		0, 20, 1),
	ctspinDiffuse		(this, IDC_SPIN_DIFFUSE,		&matLocal.rvDiffuse,	0.0, 2.0, 0.05),
	ctspinSpecular		(this, IDC_SPIN_SPECULAR,		&matLocal.rvSpecular,	0.0, 2.0, 0.05),
	ctspinangwSpecular	(this, IDC_SPIN_ANGLE,			&matLocal.angwSpecular,	0, 90, 0.5, 1),
	ctspinEmissive		(this, IDC_SPIN_EMISSIVE,		&matLocal.rvEmissive,	0.0, 2.0, 0.05)
{
	//{{AFX_DATA_INIT(CDialogMaterial)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDialogMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogMaterial)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	// Update our controls.
	DDX(pDX);

	GetMaterial();
}

//**********************************************************************************************
void CDialogMaterial::SetMaterial(CInstance* pins, int i_index)
{
	pinsEdit = pins;
	iMaterialIndex = i_index;

	GetMaterial();
}

//**********************************************************************************************
void CDialogMaterial::GetMaterial()
{
	// To do: Make material access should a general property of CShape.
	// For now, use explicit access for CMesh only.
	rptr_const<CMesh> pmsh = rptr_const_dynamic_cast(CMesh, pinsEdit->prdtGetRenderInfo());
	if (!pmsh)
		return;

	SetMinMax(iMaterialIndex, 0, pmsh->pasfSurfaces.size()-1);

	ptexEdit = pmsh->pasfSurfaces[iMaterialIndex].ptexTexture;
	matLocal = *ptexEdit->ppcePalClut->pmatMaterial;

	if (::IsWindow(m_hWnd))
	{
		// Set window titles.
		char str_title[100];

		const char* str_name = pinsEdit->strGetInstanceName();
		if (str_name)
		{
			sprintf(str_title, "Material Properties for %s", str_name);
			SetWindowText(str_title);
		}

		// Set text label.
		sprintf(str_title, "of %d", pmsh->pasfSurfaces.size());
		SetDlgItemText(IDC_STATIC_TEX_COUNT, str_title);

		rptr<CRaster> pras_0 = ptexEdit->prasGetTexture(0);
		if (pras_0)
		{
			sprintf(str_title, "%d x %d, %d levels", pras_0->iWidth, pras_0->iHeight, ptexEdit->iGetNumMipLevels());
			SetDlgItemText(IDC_EDIT_TEXTURE, str_title);
		}
		else
			SetDlgItemText(IDC_EDIT_TEXTURE, "None");

		// Display all values.
		Display();

		CheckDlgButton(IDC_CHECK_TEXTURE,		ptexEdit->seterfFeatures[erfTEXTURE]);
		CheckDlgButton(IDC_CHECK_TRANSPARENT,	ptexEdit->seterfFeatures[erfTRANSPARENT]);
		CheckDlgButton(IDC_CHECK_BUMP,			ptexEdit->seterfFeatures[erfBUMP]);
		CheckDlgButton(IDC_CHECK_ALPHA,			ptexEdit->seterfFeatures[erfALPHA_COLOUR]);
		CheckDlgButton(IDC_CHECK_COPY,			ptexEdit->seterfFeatures[erfCOPY]);
	}
}

//******************************************************************************************
void CDialogMaterial::Update(CController& ctrl)
{
	if (ctrl.iGetID() == IDC_SPIN_MATERIAL_INDEX)
	{
		// Set up for the new material.
		GetMaterial();
	}
	else
	{
		CPalClut& pce = *ptexEdit->ppcePalClut;

//		// Copy changed material, brazenly violating its constness.
//		*const_cast<CMaterial*>(pce.pmatMaterial) = matLocal;

		// Create new material, brazenly wasting memory.
		pce.pmatMaterial = CMaterial::pmatFindShared(&matLocal);

		// Re-create the pal clut and any associated bump tables.
		pce.UpdateClut(pce.pclutClut->pxfDest);

		// Update any texture flags.
		ptexEdit->UpdateFeatures();

		//
		// Update main window display.
		//
		GetParent()->Invalidate();
	}
}

BEGIN_MESSAGE_MAP(CDialogMaterial, CDialog)
	//{{AFX_MSG_MAP(CDialogMaterial)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogMaterial message handlers


void CDialogMaterial::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	GetMaterial();
}

//**********************************************************************************************
CDialog* newCDialogMaterial(CInstance* pins)
{
	static CDialogMaterial* pdlgMaterial = 0;
	if (!pdlgMaterial)
		pdlgMaterial = new CDialogMaterial;
	pdlgMaterial->SetMaterial(pins, 0);
	return pdlgMaterial;
}
