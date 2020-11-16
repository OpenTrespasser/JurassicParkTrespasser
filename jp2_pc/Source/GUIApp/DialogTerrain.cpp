/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of "DialogTerrain.h."
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogTerrain.cpp                                             $
 * 
 * 52    6/09/98 5:36p Pkeet
 * Changed code so that region uploads are based on a driver setting.
 * 
 * 51    6/04/98 2:24p Mlange
 * Can now optionally disable conform constraint.
 * 
 * 50    6/03/98 2:54p Mlange
 * Implemented check box for texel project nearest.
 * 
 * 49    5/29/98 7:18p Mlange
 * Added slider for terrain dynamic texture disable distance.
 * 
 * 48    5/20/98 3:03p Pkeet
 * Added required include.
 * 
 * 47    5/19/98 4:54p Kmckis
 * changed the optimization type default. - bFreqAsRatio = FALSE
 * 
 * 46    5/12/98 7:17p Mlange
 * Added check box for terrain texture regional uploads.
 * 
 * 45    4/23/98 7:44p Mlange
 * Can now show terrain textures that are constrained wrt subdivision in stippled blue.
 * 
 * 44    4/23/98 6:42p Mlange
 * Implemented 'freeze' functionality for quad tree updates.
 * 
 * 43    4/23/98 4:36p Mlange
 * New check boxes.
 * 
 *********************************************************************************************/

#include "StdAfx.h"
#include "DialogTerrain.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "GUIApp/GUIAppDlg.h"

#include "Lib/GeomDBase/TerrainTexture.hpp"
#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/GeomDBase/TerrainLoad.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"

//*********************************************************************************************
//
// CDialogTerrain Implementation.
//

int   CDialogTerrain::iNumQuantisationBits = iDEFAULT_NUM_QUANTISATION_BITS;
float CDialogTerrain::fFreqCutoff          = fDEFAULT_FREQ_CUTOFF;
bool  CDialogTerrain::bFreqAsRatio         = FALSE;

//*********************************************************************************************
CDialogTerrain::CDialogTerrain(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogTerrain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogTerrain)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*********************************************************************************************
void CDialogTerrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogTerrain)
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_PIX_TOLERANCE_START, m_ScrollTerrainPixToleranceStart);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_PIX_TOLERANCE_END, m_ScrollTerrainPixToleranceEnd);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_DEFER_EVAL, m_ScrollTerrainDeferEval);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_QUANTISATION_BITS, m_ScrollTerrainQuantisationBits);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_FREQ_CUTOFF, m_ScrollTerrainFreqCutoff);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_WIREZOOM, m_ScrollTerrainWireZoom);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_MAX_DIST_RATIO, m_ScrollTerrainMaxDistRatio);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_MIN_NODE_SIZE, m_ScrollTerrainMinNodeSize);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_TRI_NODE_DIFF_SHIFT, m_ScrollTerrainTriNodeDiffShift);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_TEXEL_SCALE_START, m_ScrollTerrainTexelScaleStart);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_TEXEL_SCALE_END, m_ScrollTerrainTexelScaleEnd);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_DISABLE_SHADOW_DIST, m_ScrollTerrainDisableShadowDist);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_DISABLE_DYNAMIC_TEXTURE_DIST, m_ScrollTerrainDisableDynamicTextureDist);
	DDX_Control(pDX, IDC_SCROLL_TERRAIN_DISABLE_TEXTURE_DIST, m_ScrollTerrainDisableTextureDist);
	//}}AFX_DATA_MAP
}

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogTerrain, CDialog)
	//{{AFX_MSG_MAP(CDialogTerrain)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_TERRAIN_DYNAMIC, OnCheckTerrainDynamic)
	ON_BN_CLICKED(IDC_CHECK_TERRAIN_CLEARS, OnCheckTerrainClears)
	ON_BN_CLICKED(IDC_CHECK_TERRAIN_REGION_UPLOADS, OnCheckTerrainRegionUploads)
	ON_BN_CLICKED(IDC_CHECK_TERRAIN_SHOW_CONSTRAINED, OnCheckTerrainShowConstrained)
	ON_BN_CLICKED(IDC_CHECK_TERRAIN_FREQ_AS_RATIO, OnCheckTerrainFreqAsRatio)
	ON_BN_CLICKED(IDC_CHECK_TERRAIN_FREEZE, OnCheckTerrainFreeze)
	ON_BN_CLICKED(IDC_CHECK_TERRAIN_PROJ_NEAREST, OnCheckTerrainProjNearest)
	ON_BN_CLICKED(IDC_CHECK_TERRAIN_CONFORM_CONSTRAINT, OnCheckTerrainConformConstraint)
	ON_BN_CLICKED(IDC_CHECK_TERRAIN_WIRETRIS, OnCheckTerrainWireTris)
	ON_BN_CLICKED(IDC_CHECK_TERRAIN_WIREQUADS, OnCheckTerrainWireQuads)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*********************************************************************************************
void CDialogTerrain::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Move scrollbar.
	MoveScrollbar(nSBCode, nPos, pScrollBar);

	// Call the base class member function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	NMultiResolution::CQuadRootTIN::rvarPixelTolerance.SetFraction(m_ScrollTerrainPixToleranceStart.GetScrollPos() / 100.0);
	NMultiResolution::CQuadRootTIN::rvarPixelToleranceFar.SetFraction(m_ScrollTerrainPixToleranceEnd.GetScrollPos() / 100.0);
	NMultiResolution::CQuadRootTIN::rvarDeferMultiplier.SetFraction(m_ScrollTerrainDeferEval.GetScrollPos() / 100.0);

	NMultiResolution::CQuadRootTIN::rvarMaxDistRatio.SetFraction(m_ScrollTerrainMaxDistRatio.GetScrollPos() / 100.0);
	NMultiResolution::CQuadRootTIN::rvarMinDistRatioNodeSize.Set(m_ScrollTerrainMinNodeSize.GetScrollPos());
	NMultiResolution::CQuadRootTIN::rvarSigNodeDiffShift.Set(m_ScrollTerrainTriNodeDiffShift.GetScrollPos());

	NMultiResolution::CQuadRootTIN::rvarTexelScale.SetFraction(m_ScrollTerrainTexelScaleStart.GetScrollPos() / 100.0);
	NMultiResolution::CQuadRootTIN::rvarTexelScaleFar.SetFraction(m_ScrollTerrainTexelScaleEnd.GetScrollPos() / 100.0);
	NMultiResolution::CQuadRootTIN::rvarDisableShadowDistance.SetFraction(m_ScrollTerrainDisableShadowDist.GetScrollPos() / 100.0);
	NMultiResolution::CQuadRootTIN::rvarDisableDynamicTextureDistance.SetFraction(m_ScrollTerrainDisableDynamicTextureDist.GetScrollPos() / 100.0);
	NMultiResolution::CQuadRootTIN::rvarDisableTextureDistance.SetFraction(m_ScrollTerrainDisableTextureDist.GetScrollPos() / 100.0);

	CTerrain::rvarWireZoom.SetFraction(m_ScrollTerrainWireZoom.GetScrollPos() / 100.0);


	CDialogTerrain::iNumQuantisationBits = m_ScrollTerrainQuantisationBits.GetScrollPos();
	CDialogTerrain::fFreqCutoff          = m_ScrollTerrainFreqCutoff.GetScrollPos() / 100.0;

	// Display new values.
	DisplayText();

	// Re-evaluate the terrain.
	Update();
}

//*********************************************************************************************
void CDialogTerrain::Update()
{
	// Re-evaluate the terrain.
	if (CWDbQueryTerrain().tGet() != 0)
		CWDbQueryTerrain().tGet()->Rebuild(true);

	// Update main screen.
	GetParent()->Invalidate();
}

//*********************************************************************************************
void CDialogTerrain::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);	

	// Set to default position.
	int i_x  = 512 + 2 * GetSystemMetrics(SM_CXFRAME) + iGUIAPP_DEFAULT_OFFSET;
	SetWindowPos(&wndTop, i_x, iGUIAPP_DEFAULT_OFFSET, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);

	//
	// Setup scrollbars.
	//

	// Setup the constant scrollbar.
	m_ScrollTerrainPixToleranceStart.SetScrollRange(0, 100, FALSE);
	m_ScrollTerrainPixToleranceEnd.SetScrollRange(0, 100, FALSE);
	m_ScrollTerrainDeferEval.SetScrollRange(0, 100, FALSE);
	m_ScrollTerrainQuantisationBits.SetScrollRange(4, 28, FALSE);
	m_ScrollTerrainFreqCutoff.SetScrollRange(0, 100, FALSE);
	m_ScrollTerrainMaxDistRatio.SetScrollRange(0, 100, FALSE);
	m_ScrollTerrainTexelScaleStart.SetScrollRange(0, 100, FALSE);
	m_ScrollTerrainTexelScaleEnd.SetScrollRange(0, 100, FALSE);
	m_ScrollTerrainDisableShadowDist.SetScrollRange(0, 100, FALSE);
	m_ScrollTerrainDisableDynamicTextureDist.SetScrollRange(0, 100, FALSE);
	m_ScrollTerrainDisableTextureDist.SetScrollRange(0, 100, FALSE);
	m_ScrollTerrainWireZoom.SetScrollRange(0, 100, FALSE);

	int i_tol_start            = int(NMultiResolution::CQuadRootTIN::rvarPixelTolerance.fGetFraction() * 100 + .499);
	int i_tol_end              = int(NMultiResolution::CQuadRootTIN::rvarPixelToleranceFar.fGetFraction() * 100 + .499);
	int i_defer_eval           = int(NMultiResolution::CQuadRootTIN::rvarDeferMultiplier.fGetFraction() * 100 + .499);
	int i_max_dist_ratio       = int(NMultiResolution::CQuadRootTIN::rvarMaxDistRatio.fGetFraction() * 100 + .499);
	int i_min_node_size        = int(NMultiResolution::CQuadRootTIN::rvarMinDistRatioNodeSize);
	int i_tri_node_diff_shift  = int(NMultiResolution::CQuadRootTIN::rvarSigNodeDiffShift);
	int i_tex_scale_start      = int(NMultiResolution::CQuadRootTIN::rvarTexelScale.fGetFraction() * 100 + .499);
	int i_tex_scale_end        = int(NMultiResolution::CQuadRootTIN::rvarTexelScaleFar.fGetFraction() * 100 + .499);
	int i_disable_shadow_dist  = int(NMultiResolution::CQuadRootTIN::rvarDisableShadowDistance.fGetFraction() * 100 + .499);
	int i_disable_dyn_tex_dist = int(NMultiResolution::CQuadRootTIN::rvarDisableDynamicTextureDistance.fGetFraction() * 100 + .499);
	int i_disable_texture_dist = int(NMultiResolution::CQuadRootTIN::rvarDisableTextureDistance.fGetFraction() * 100 + .499);
	int i_wirezoom             = int(CTerrain::rvarWireZoom.fGetFraction() * 100 + .499);

	m_ScrollTerrainMinNodeSize.SetScrollRange(NMultiResolution::CQuadRootTIN::rvarMinDistRatioNodeSize.tGetMin(),
		                                      NMultiResolution::CQuadRootTIN::rvarMinDistRatioNodeSize.tGetMax(), FALSE);

	m_ScrollTerrainTriNodeDiffShift.SetScrollRange(NMultiResolution::CQuadRootTIN::rvarSigNodeDiffShift.tGetMin(),
		                                           NMultiResolution::CQuadRootTIN::rvarSigNodeDiffShift.tGetMax(), FALSE);

	m_ScrollTerrainPixToleranceStart.SetScrollPos(i_tol_start);
	m_ScrollTerrainPixToleranceEnd.SetScrollPos(i_tol_end);
	m_ScrollTerrainDeferEval.SetScrollPos(i_defer_eval);
	m_ScrollTerrainQuantisationBits.SetScrollPos(CDialogTerrain::iNumQuantisationBits);
	m_ScrollTerrainFreqCutoff.SetScrollPos(CDialogTerrain::fFreqCutoff * 100 + .499);
	m_ScrollTerrainMaxDistRatio.SetScrollPos(i_max_dist_ratio);
	m_ScrollTerrainMinNodeSize.SetScrollPos(i_min_node_size);
	m_ScrollTerrainTriNodeDiffShift.SetScrollPos(i_tri_node_diff_shift);
	m_ScrollTerrainTexelScaleStart.SetScrollPos(i_tex_scale_start);
	m_ScrollTerrainTexelScaleEnd.SetScrollPos(i_tex_scale_end);
	m_ScrollTerrainDisableShadowDist.SetScrollPos(i_disable_shadow_dist);
	m_ScrollTerrainDisableDynamicTextureDist.SetScrollPos(i_disable_dyn_tex_dist);
	m_ScrollTerrainDisableTextureDist.SetScrollPos(i_disable_texture_dist);
	m_ScrollTerrainWireZoom.SetScrollPos(i_wirezoom);

	// Texturing setup.
	CheckDlgButton(IDC_CHECK_TERRAIN_DYNAMIC,            NMultiResolution::CTextureNode::bDisableTextures);
	CheckDlgButton(IDC_CHECK_TERRAIN_CLEARS,             NMultiResolution::CTextureNode::bClears);
	CheckDlgButton(IDC_CHECK_TERRAIN_SHOW_CONSTRAINED,   NMultiResolution::CTextureNode::bShowConstrained);
	CheckDlgButton(IDC_CHECK_TERRAIN_FREQ_AS_RATIO,      CDialogTerrain::bFreqAsRatio);
	CheckDlgButton(IDC_CHECK_TERRAIN_FREEZE,             NMultiResolution::CQuadRootTIN::bFreeze);
	CheckDlgButton(IDC_CHECK_TERRAIN_PROJ_NEAREST,       NMultiResolution::CQuadRootTIN::bTexelProjNearest);
	CheckDlgButton(IDC_CHECK_TERRAIN_CONFORM_CONSTRAINT, NMultiResolution::CQuadRootTIN::bConform);
	CheckDlgButton(IDC_CHECK_TERRAIN_WIRETRIS,           CTerrain::bShowWireframe);
	CheckDlgButton(IDC_CHECK_TERRAIN_WIREQUADS,          CTerrain::bShowQuadtree);

	//
	// Setup static text.
	//
	DisplayText();
}

//*********************************************************************************************
//
void CDialogTerrain::DisplayText
(
)
//
// Set the text in all related static controls.
//
//**************************************
{
	float f_tol_start            = NMultiResolution::CQuadRootTIN::rvarPixelTolerance;
	float f_tol_end              = NMultiResolution::CQuadRootTIN::rvarPixelToleranceFar;
	float f_defer_eval           = NMultiResolution::CQuadRootTIN::rvarDeferMultiplier;
	float f_max_dist_ratio       = NMultiResolution::CQuadRootTIN::rvarMaxDistRatio;
	int   i_min_node_size        = NMultiResolution::CQuadRootTIN::rvarMinDistRatioNodeSize;
	int   i_tri_node_diff_shift  = NMultiResolution::CQuadRootTIN::rvarSigNodeDiffShift;
	float f_tex_scale_start      = NMultiResolution::CQuadRootTIN::rvarTexelScale;
	float f_tex_scale_end        = NMultiResolution::CQuadRootTIN::rvarTexelScaleFar;
	float f_disable_shadow_dist  = NMultiResolution::CQuadRootTIN::rvarDisableShadowDistance;
	float f_disable_dyn_tex_dist = NMultiResolution::CQuadRootTIN::rvarDisableDynamicTextureDistance;
	float f_disable_texture_dist = NMultiResolution::CQuadRootTIN::rvarDisableTextureDistance;
	float f_wirezoom             = CTerrain::rvarWireZoom;

	SetDlgItemFloat(this, IDC_STATIC_TERRAIN_PIX_TOLERANCE_START, f_tol_start, 2);
	SetDlgItemFloat(this, IDC_STATIC_TERRAIN_PIX_TOLERANCE_END, f_tol_end, 2);
	SetDlgItemFloat(this, IDC_STATIC_TERRAIN_DEFER_EVAL, f_defer_eval, 2);

	SetDlgItemInt(IDC_STATIC_TERRAIN_QUANTISATION_BITS, CDialogTerrain::iNumQuantisationBits);
	SetDlgItemFloat(this, IDC_STATIC_TERRAIN_FREQ_CUTOFF, CDialogTerrain::fFreqCutoff, 2);

	SetDlgItemFloat(this, IDC_STATIC_TERRAIN_MAX_DIST_RATIO, f_max_dist_ratio, 2);
	SetDlgItemInt(IDC_STATIC_TERRAIN_MIN_NODE_SIZE, i_min_node_size);
	SetDlgItemInt(IDC_STATIC_TERRAIN_TRI_NODE_DIFF_SHIFT, i_tri_node_diff_shift);

	SetDlgItemFloat(this, IDC_STATIC_TERRAIN_TEXEL_SCALE_START, f_tex_scale_start, 2);
	SetDlgItemFloat(this, IDC_STATIC_TERRAIN_TEXEL_SCALE_END, f_tex_scale_end, 2);
	SetDlgItemFloat(this, IDC_STATIC_TERRAIN_DISABLE_SHADOW_DIST, f_disable_shadow_dist, 2);
	SetDlgItemFloat(this, IDC_STATIC_TERRAIN_DISABLE_DYNAMIC_TEXTURE_DIST, f_disable_dyn_tex_dist, 2);
	SetDlgItemFloat(this, IDC_STATIC_TERRAIN_DISABLE_TEXTURE_DIST, f_disable_texture_dist, 2);

	SetDlgItemFloat(this, IDC_STATIC_TERRAIN_WIREZOOM, f_wirezoom, 2);
}

//*********************************************************************************************
//

void CDialogTerrain::OnCheckTerrainDynamic() 
{
	NMultiResolution::CTextureNode::bDisableTextures = IsDlgButtonChecked(IDC_CHECK_TERRAIN_DYNAMIC);
	Update();  
}

void CDialogTerrain::OnCheckTerrainClears() 
{
	NMultiResolution::CTextureNode::bClears = IsDlgButtonChecked(IDC_CHECK_TERRAIN_CLEARS);
	Update();  
}

void CDialogTerrain::OnCheckTerrainRegionUploads() 
{
	Update();  
}

void CDialogTerrain::OnCheckTerrainShowConstrained() 
{
	NMultiResolution::CTextureNode::bShowConstrained = IsDlgButtonChecked(IDC_CHECK_TERRAIN_SHOW_CONSTRAINED);
	Update();  
}

void CDialogTerrain::OnCheckTerrainFreqAsRatio() 
{
	CDialogTerrain::bFreqAsRatio = IsDlgButtonChecked(IDC_CHECK_TERRAIN_FREQ_AS_RATIO);
	Update();  
}

void CDialogTerrain::OnCheckTerrainFreeze() 
{
	NMultiResolution::CQuadRootTIN::bFreeze = IsDlgButtonChecked(IDC_CHECK_TERRAIN_FREEZE);
}

void CDialogTerrain::OnCheckTerrainProjNearest() 
{
	NMultiResolution::CQuadRootTIN::bTexelProjNearest = IsDlgButtonChecked(IDC_CHECK_TERRAIN_PROJ_NEAREST);
}

void CDialogTerrain::OnCheckTerrainConformConstraint()
{
	NMultiResolution::CQuadRootTIN::bConform = IsDlgButtonChecked(IDC_CHECK_TERRAIN_CONFORM_CONSTRAINT);
}

void CDialogTerrain::OnCheckTerrainWireTris() 
{
	CTerrain::bShowWireframe = IsDlgButtonChecked(IDC_CHECK_TERRAIN_WIRETRIS);

	// Update main screen.
	GetParent()->Invalidate();
}

void CDialogTerrain::OnCheckTerrainWireQuads() 
{
	CTerrain::bShowQuadtree = IsDlgButtonChecked(IDC_CHECK_TERRAIN_WIREQUADS);

	// Update main screen.
	GetParent()->Invalidate();
}

