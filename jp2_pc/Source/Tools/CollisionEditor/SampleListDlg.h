#pragma once

/////////////////////////////////////////////////////////////////////////////
// CSampleListDlg dialog used for App About
#include "resource.h"

class CSampleListDlg : public CDialog
{
public:
	CSampleListDlg(SEffect* peff);

// Dialog Data
	//{{AFX_DATA(CSampleListDlg)
	enum { IDD = IDD_SAMPLE_PROP };

	CSliderCtrl	sliderMasterVolume;
	CSliderCtrl	sliderAttenuation;

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SEffect*	peffSample;

	//{{AFX_MSG(CSampleListDlg)

	virtual BOOL OnInitDialog() override;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
