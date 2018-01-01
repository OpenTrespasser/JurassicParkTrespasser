// TestMaterialDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CollisionEditor.h"
#include "TestMaterialDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestMaterialDlg dialog


CTestMaterialDlg::CTestMaterialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestMaterialDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestMaterialDlg)
	velocity_string = _T("");
	//}}AFX_DATA_INIT
}


void CTestMaterialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestMaterialDlg)
	DDX_Control(pDX, IDC_VELOCITY_SLIDER, velocity_slider);
	DDX_Control(pDX, IDC_SURFACE_LIST, surface_list);
	DDX_Control(pDX, IDC_OBJECT_LIST, object_list);
	DDX_Text(pDX, IDC_VELOCITY, velocity_string);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestMaterialDlg, CDialog)
	//{{AFX_MSG_MAP(CTestMaterialDlg)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_PLAYCOL, OnPlaycol)
	ON_BN_CLICKED(IDC_PLAYOBJ, OnPlayobj)
	ON_BN_CLICKED(IDC_PLAYSURF, OnPlaysurf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestMaterialDlg message handlers

BOOL CTestMaterialDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Initialize list boxes.
	string* pstr;
	int idx;
	map<string, TMaterialProperties, less<string> >::iterator i;
	for (i = g_mdb->db.begin(); i != g_mdb->db.end(); i++)
	{
		if ((*i).second.reference_waves[0][0].length() > 0)
		{
			pstr = new string((*i).first);
			idx = object_list.AddString(pstr->c_str());
			object_list.SetItemDataPtr(idx, pstr);
			idx = surface_list.AddString(pstr->c_str());
			surface_list.SetItemDataPtr(idx, pstr);
		}
	}

	// Initialize velocity/slider.
	velocity_slider.SetRange(0, 100);
	velocity_slider.SetPos(0);
	velocity = 0.0f;
	velocity_string.Format("%2.2f", velocity);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTestMaterialDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData(TRUE);

	int pos = velocity_slider.GetPos();
	velocity = float(pos)/100.0f;
	velocity_string.Format("%2.2f", velocity);
	UpdateData(FALSE);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTestMaterialDlg::OnPlaycol() 
{
	int idx_object = object_list.GetCurSel();
	if (idx_object == LB_ERR) return;
	string* pstr_object = (string*)object_list.GetItemDataPtr(idx_object);
	ASSERT(pstr_object);

	int idx_surface = surface_list.GetCurSel();
	if (idx_surface == LB_ERR) return;
	string* pstr_surface = (string*)surface_list.GetItemDataPtr(idx_surface);
	ASSERT(pstr_surface);

	codee->collide(*pstr_object, *pstr_surface, velocity, CVector3<>(0,0,0));
}

void CTestMaterialDlg::OnPlayobj() 
{
	int idx_object = object_list.GetCurSel();
	if (idx_object == LB_ERR) return;
	string* pstr_object = (string*)object_list.GetItemDataPtr(idx_object);
	ASSERT(pstr_object);
}

void CTestMaterialDlg::OnPlaysurf() 
{
	int idx_surface = surface_list.GetCurSel();
	if (idx_surface == LB_ERR) return;
	string* pstr_surface = (string*)surface_list.GetItemDataPtr(idx_surface);
	ASSERT(pstr_surface);
}
