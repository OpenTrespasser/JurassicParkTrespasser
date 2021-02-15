/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Terrain Dialog.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogTerrain.hpp                                             $
 * 
 * 28    6/04/98 2:24p Mlange
 * Can now optionally disable conform constraint.
 * 
 * 27    6/03/98 2:54p Mlange
 * Implemented check box for texel project nearest.
 * 
 * 26    5/29/98 7:18p Mlange
 * Added slider for terrain dynamic texture disable distance.
 * 
 * 25    5/12/98 7:17p Mlange
 * Added check box for terrain texture regional uploads.
 * 
 * 24    4/23/98 7:44p Mlange
 * Can now show terrain textures that are constrained wrt subdivision in stippled blue.
 * 
 * 23    4/23/98 4:36p Mlange
 * New check boxes.
 * 
 * 22    4/14/98 2:05p Mlange
 * Can now optionally clear terrain textures before rendering,
 * 
 * 21    3/31/98 11:08a Mlange
 * Can now create terrain textures as fogged, solid colour textures at some distance.
 * 
 * 20    3/16/98 5:45p Mlange
 * Can now disable terrain texture shadows beyond some distance.
 * 
 * 19    2/27/98 6:26p Mlange
 * Implemented the new sliders.
 * 
 *********************************************************************************************/


#include "DialogScrollbars.hpp"

#define iDEFAULT_NUM_QUANTISATION_BITS	16
#define fDEFAULT_FREQ_CUTOFF			0.0f


//*********************************************************************************************
//
class CDialogTerrain : public CDialog
//
// Terrain dialog.
//
//**************************************
{
public:
	static int   iNumQuantisationBits;
	static float fFreqCutoff;
	static bool  bFreqAsRatio;

	//*****************************************************************************************
	CDialogTerrain(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogTerrain)
	enum { IDD = IDD_TERRAIN };
	CScrollBar	m_ScrollTerrainPixToleranceStart;
	CScrollBar	m_ScrollTerrainPixToleranceEnd;
	CScrollBar	m_ScrollTerrainDeferEval;
	CScrollBar	m_ScrollTerrainQuantisationBits;
	CScrollBar	m_ScrollTerrainFreqCutoff;
	CScrollBar	m_ScrollTerrainWireZoom;
	CScrollBar	m_ScrollTerrainMaxDistRatio;
	CScrollBar	m_ScrollTerrainMinNodeSize;
	CScrollBar	m_ScrollTerrainTriNodeDiffShift;
	CScrollBar	m_ScrollTerrainTexelScaleStart;
	CScrollBar	m_ScrollTerrainTexelScaleEnd;
	CScrollBar	m_ScrollTerrainDisableShadowDist;
	CScrollBar	m_ScrollTerrainDisableDynamicTextureDist;
	CScrollBar	m_ScrollTerrainDisableTextureDist;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogTerrain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	//*****************************************************************************************
	void DisplayText();

	//*********************************************************************************************
	void Update();

	// Generated message map functions
	//{{AFX_MSG(CDialogTerrain)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCheckTerrainDynamic();
	afx_msg void OnCheckTerrainClears();
	afx_msg void OnCheckTerrainRegionUploads();
	afx_msg void OnCheckTerrainShowConstrained();
	afx_msg void OnCheckTerrainFreqAsRatio();
	afx_msg void OnCheckTerrainFreeze();
	afx_msg void OnCheckTerrainProjNearest();
	afx_msg void OnCheckTerrainConformConstraint();
	afx_msg void OnCheckTerrainWireTris();
	afx_msg void OnCheckTerrainWireQuads();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
