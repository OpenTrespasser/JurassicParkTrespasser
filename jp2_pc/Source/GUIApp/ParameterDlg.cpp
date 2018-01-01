// ParameterDlg.cpp : implementation file
//

// Make sure this constant hasn't already been defined.
#ifndef VC_EXTRALEAN
// Exclude rarely-used stuff from Windows headers.
#define VC_EXTRALEAN		
#endif
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#undef min
#undef max

#undef STRICT

// Project includes.
#include "resource.h"

#include "common.hpp"
//#include "AI Dialogs2.h"
#include "GUIApp/ParameterDlg.h"
#include "Game/AI/Feeling.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParameterDlg dialog


CParameterDlg::CParameterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParameterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParameterDlg)
	m_Param0 = 0.0f;
	m_Param1 = 0.0f;
	m_Param2 = 0.0f;
	m_Param3 = 0.0f;
	m_Param4 = 0.0f;
	m_Param5 = 0.0f;
	m_Param6 = 0.0f;
	m_Param7 = 0.0f;
	m_Param8 = 0.0f;
	//}}AFX_DATA_INIT

	pFeeling = 0;
	bDidInit = false;
}


void CParameterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParameterDlg)
	DDX_Control(pDX, IDC_PARAMETER8, m_ParamSlider8);
	DDX_Control(pDX, IDC_PARAMETER7, m_ParamSlider7);
	DDX_Control(pDX, IDC_PARAMETER6, m_ParamSlider6);
	DDX_Control(pDX, IDC_PARAMETER5, m_ParamSlider5);
	DDX_Control(pDX, IDC_PARAMETER4, m_ParamSlider4);
	DDX_Control(pDX, IDC_PARAMETER3, m_ParamSlider3);
	DDX_Control(pDX, IDC_PARAMETER2, m_ParamSlider2);
	DDX_Control(pDX, IDC_PARAMETER1, m_ParamSlider1);
	DDX_Control(pDX, IDC_PARAMETER0, m_ParamSlider0);
	DDX_Text(pDX, IDC_EDIT_PARAMETER0, m_Param0);
	DDX_Text(pDX, IDC_EDIT_PARAMETER1, m_Param1);
	DDX_Text(pDX, IDC_EDIT_PARAMETER2, m_Param2);
	DDX_Text(pDX, IDC_EDIT_PARAMETER3, m_Param3);
	DDX_Text(pDX, IDC_EDIT_PARAMETER4, m_Param4);
	DDX_Text(pDX, IDC_EDIT_PARAMETER5, m_Param5);
	DDX_Text(pDX, IDC_EDIT_PARAMETER6, m_Param6);
	DDX_Text(pDX, IDC_EDIT_PARAMETER7, m_Param7);
	DDX_Text(pDX, IDC_EDIT_PARAMETER8, m_Param8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParameterDlg, CDialog)
	//{{AFX_MSG_MAP(CParameterDlg)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_PARAMETER0, OnChangeEditParameter)
	ON_EN_CHANGE(IDC_EDIT_PARAMETER1, OnChangeEditParameter)
	ON_EN_CHANGE(IDC_EDIT_PARAMETER2, OnChangeEditParameter)
	ON_EN_CHANGE(IDC_EDIT_PARAMETER3, OnChangeEditParameter)
	ON_EN_CHANGE(IDC_EDIT_PARAMETER4, OnChangeEditParameter)
	ON_EN_CHANGE(IDC_EDIT_PARAMETER5, OnChangeEditParameter)
	ON_EN_CHANGE(IDC_EDIT_PARAMETER6, OnChangeEditParameter)
	ON_EN_CHANGE(IDC_EDIT_PARAMETER7, OnChangeEditParameter)
	ON_EN_CHANGE(IDC_EDIT_PARAMETER8, OnChangeEditParameter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParameterDlg message handlers

BOOL CParameterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CParameterDlg::OnOK() 
{
	// Set the feeling to the param values!
	if (pFeeling)
	{
		UpdateData(true);

		(*pFeeling)[0] = m_Param0;
		(*pFeeling)[1] = m_Param1;
		(*pFeeling)[2] = m_Param2;
		(*pFeeling)[3] = m_Param3;
		(*pFeeling)[4] = m_Param4;
		(*pFeeling)[5] = m_Param5;
		(*pFeeling)[6] = m_Param6;
		(*pFeeling)[7] = m_Param7;
		(*pFeeling)[8] = m_Param8;
	}

	pFeeling = 0;

	CDialog::OnOK();
}

void CParameterDlg::UpdateEditFromScroll()
{
	m_Param0 = 0.1f * (float) m_ParamSlider0.GetPos();
	m_Param1 = 0.1f * (float) m_ParamSlider1.GetPos();
	m_Param2 = 0.1f * (float) m_ParamSlider2.GetPos();
	m_Param3 = 0.1f * (float) m_ParamSlider3.GetPos();
	m_Param4 = 0.1f * (float) m_ParamSlider4.GetPos();
	m_Param5 = 0.1f * (float) m_ParamSlider5.GetPos();
	m_Param6 = 0.1f * (float) m_ParamSlider6.GetPos();
	m_Param7 = 0.1f * (float) m_ParamSlider7.GetPos();
	m_Param8 = 0.1f * (float) m_ParamSlider8.GetPos();
	
	UpdateData(FALSE);
}

void CParameterDlg::UpdateScrollFromEdit()
{
	UpdateData(TRUE);

	m_ParamSlider0.SetPos(m_Param0 * 10.0f);
	m_ParamSlider1.SetPos(m_Param1 * 10.0f);
	m_ParamSlider2.SetPos(m_Param2 * 10.0f);
	m_ParamSlider3.SetPos(m_Param3 * 10.0f);
	m_ParamSlider4.SetPos(m_Param4 * 10.0f);
	m_ParamSlider5.SetPos(m_Param5 * 10.0f);
	m_ParamSlider6.SetPos(m_Param6 * 10.0f);
	m_ParamSlider7.SetPos(m_Param7 * 10.0f);
	m_ParamSlider8.SetPos(m_Param8 * 10.0f);
}


void CParameterDlg::SetFeeling(CFeeling* pfeel, float f_low, float f_high)
{
	Assert(pfeel);

	f_low = f_low * 10; 
	f_high = f_high * 10;
	
	m_ParamSlider0.SetRange(f_low, f_high, FALSE);
	m_ParamSlider1.SetRange(f_low, f_high, FALSE);
	m_ParamSlider2.SetRange(f_low, f_high, FALSE);
	m_ParamSlider3.SetRange(f_low, f_high, FALSE);
	m_ParamSlider4.SetRange(f_low, f_high, FALSE);
	m_ParamSlider5.SetRange(f_low, f_high, FALSE);
	m_ParamSlider6.SetRange(f_low, f_high, FALSE);
	m_ParamSlider7.SetRange(f_low, f_high, FALSE);
	m_ParamSlider8.SetRange(f_low, f_high, FALSE);

	pFeeling = pfeel;

	bDidInit = false;
} 

void CParameterDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CParameterDlg::OnPaint() 
{
	// Set the params to the feeling  values!
	if (!bDidInit && pFeeling)
	{
		m_Param0 = (*pFeeling)[0];
		m_Param1 = (*pFeeling)[1];
		m_Param2 = (*pFeeling)[2];
		m_Param3 = (*pFeeling)[3];
		m_Param4 = (*pFeeling)[4];
		m_Param5 = (*pFeeling)[5];
		m_Param6 = (*pFeeling)[6];
		m_Param7 = (*pFeeling)[7];
		m_Param8 = (*pFeeling)[8];

		m_ParamSlider0.SetPos(m_Param0 * 10);
		m_ParamSlider1.SetPos(m_Param1 * 10);
		m_ParamSlider2.SetPos(m_Param2 * 10);
		m_ParamSlider3.SetPos(m_Param3 * 10);
		m_ParamSlider4.SetPos(m_Param4 * 10);
		m_ParamSlider5.SetPos(m_Param5 * 10);
		m_ParamSlider6.SetPos(m_Param6 * 10);
		m_ParamSlider7.SetPos(m_Param7 * 10);
		m_ParamSlider8.SetPos(m_Param8 * 10);

		UpdateData(false);

		bDidInit = true;
	}




	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CParameterDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateEditFromScroll();
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CParameterDlg::OnChangeEditParameter() 
{
	UpdateScrollFromEdit();
}
