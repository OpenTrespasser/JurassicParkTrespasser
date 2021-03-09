#pragma once

// DialogRenderCache.hpp : header file
//


//
// Required includes.
//
#include "DialogScrollbars.hpp"

/////////////////////////////////////////////////////////////////////////////
// CDialogRenderCache dialog

class CDialogRenderCache : public CDialog
{
// Construction
public:
	CDialogRenderCache(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogRenderCache)
	enum { IDD = IDD_RENDERCACHE };
	CScrollFloatLog	m_ParallaxShear;
	CScrollFloatLog	m_AreaEfficiency;
	CScrollFloatLog	m_Distortion;
	CScrollBar	    m_CylinderDeg;
	CScrollBar	    m_MaxObjects;
	CScrollBar      m_DetailReduce;
	CScrollBar	    m_PolyArea;
	CScrollBar	    m_Line;
	CScrollBar	    m_Area;
	CButton	        m_ForceCaching;
	CButton	        m_FasterPhysics;
	CButton	        m_CheckPrediction;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogRenderCache)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//*****************************************************************************************
	void DisplayText();

	//*****************************************************************************************
	void Redraw();

	// Generated message map functions
	//{{AFX_MSG(CDialogRenderCache)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCheckPrediction();
	afx_msg void OnCheckFasterPhysics();
	afx_msg void OnCheckForceCaching();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
