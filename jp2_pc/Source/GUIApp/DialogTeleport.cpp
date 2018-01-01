// DialogTeleport.cpp : implementation file
//

#include "stdafx.h"
#include "guiapp.h"
#include "DialogTeleport.h"

#include "..\Lib\GeomDBase\PartitionPriv.hpp"
#include "..\Lib\Renderer\Camera.hpp"
#include "..\Lib\EntityDBase\WorldDBase.hpp"
#include "..\Lib\EntityDBase\Query\QRenderer.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogTeleport dialog


CDialogTeleport::CDialogTeleport(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogTeleport::IDD, pParent)
{
    char            sz[50];
	CCamera* pcam = CWDbQueryActiveCamera().tGet();
    CPlacement3<>   place = pcam->pr3Presence();

    sprintf(sz, "%.1f", place.v3Pos.tX);
	m_csCurrX = sz;

    sprintf(sz, "%.1f", place.v3Pos.tY);
	m_csCurrY = sz;

    sprintf(sz, "%.1f", place.v3Pos.tZ);
    m_csCurrZ = sz;

	//{{AFX_DATA_INIT(CDialogTeleport)
	//}}AFX_DATA_INIT
}


void CDialogTeleport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogTeleport)
	DDX_Control(pDX, IDC_TELE_Z, m_editTeleZ);
	DDX_Control(pDX, IDC_TELE_Y, m_editTeleY);
	DDX_Control(pDX, IDC_TELE_X, m_editTeleX);
	DDX_Text(pDX, IDC_CURR_X, m_csCurrX);
	DDX_Text(pDX, IDC_CURR_Y, m_csCurrY);
	DDX_Text(pDX, IDC_CURR_Z, m_csCurrZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogTeleport, CDialog)
	//{{AFX_MSG_MAP(CDialogTeleport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogTeleport message handlers

void CDialogTeleport::OnOK() 
{
    float       fX;
    float       fY;
    float       fZ;
    char        sz[50];

    sz[m_editTeleX.GetLine(0, sz, sizeof(sz))] = '\0';
    sscanf(sz, "%f", &fX);

    sz[m_editTeleY.GetLine(0, sz, sizeof(sz))] = '\0';
    sscanf(sz, "%f", &fY);

    sz[m_editTeleZ.GetLine(0, sz, sizeof(sz))] = '\0';
    sscanf(sz, "%f", &fZ);

	extern void PlayerTeleportToXYZ(float fX, float fY, float fZ);

    PlayerTeleportToXYZ(fX, fY, fZ);
	
	CDialog::OnOK();
}

BOOL CDialogTeleport::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}
