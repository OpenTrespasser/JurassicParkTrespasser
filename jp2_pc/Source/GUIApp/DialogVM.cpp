/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents: Implementation of 'DialogVM.hpp'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogVM.cpp                                                  $
 * 
 * 8     10/02/98 9:52p Rwyatt
 * Removed reference to the old async loader
 * 
 * 7     98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 6     4/21/98 2:57p Rwyatt
 * Modified for the new VM system
 * 
 * 5     2/25/98 3:16p Rwyatt
 * Added an extentsion to the swp and pid file so  multiples can exist for different versions
 * of the GUIApp
 * 
 * 4     2/18/98 1:06p Rwyatt
 * Added ImageLoader.hpp
 * 
 * 3     1/29/98 7:26p Rwyatt
 * New buttons to commit all, use conventional loader and commit on load.
 * 
 * 2     12/15/97 5:00p Rwyatt
 * AsyncLoader class chnaged
 * 
 * 1     12/04/97 3:20p Rwyatt
 * Initial implementation
 * 
 *********************************************************************************************/


//
// Includes.
//

//#include "stdafx.h"

//
// Warning::: Complete fucking bodge starts here...
//

//**********************************************************************************************
// Look at all the shit I had to go through to be able to include WinInclude after stdafx.h
// Why does stdafx.h include GUIAPP.h and GUIAPPDLG.h and GUITOOLS.h ???
// This section should be put into its own header file called stdafxwin.h or something like that.
//

#if (_MSC_VER >= 1100)
// MFC defines some comparison operators that differ from STL templated operators only
// by calling convertion resulting in this warning.
#pragma warning(disable: 4666)
#endif

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
#include "Common.hpp"

//**********************************************************************************************

// Now back to those includes

#include "GUIApp.h"
#include "GUIAppDlg.h"
#include "DialogVM.hpp"
#include "Lib/Sys/VirtualMem.hpp"
#include "Lib/sys/fileEx.hpp"
#include "Lib/Loader/TextureManager.hpp"
#include "Lib/Loader/ImageLoader.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"



//*********************************************************************************************
//
// CDialogVM implementation.
//


//*********************************************************************************************
//
// Message map for CDialogVM.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogVM, CDialog)
	//{{AFX_MSG_MAP(CDialogVM)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_GOVIRTUAL, OnGoVirtual)
	ON_BN_CLICKED(IDC_DECOMMIT, OnDecommit)
	ON_BN_CLICKED(IDC_RADIOA, OnRadio1)
	ON_BN_CLICKED(IDC_RADIOB, OnRadio2)
	ON_BN_CLICKED(IDC_RADIOC, OnRadio3)
	ON_BN_CLICKED(IDC_RADIOD, OnRadio4)
	ON_BN_CLICKED(IDC_RADIOE, OnRadio5)
	ON_BN_CLICKED(IDC_RADIOF, OnRadio6)
	ON_BN_CLICKED(IDC_RADIOG, OnRadio7)
	ON_BN_CLICKED(IDC_RADIOH, OnRadio8)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_BN_CLICKED(IDC_CHECK6, OnCheck6)
	ON_BN_CLICKED(IDC_COMMITALL, OnCommitAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CDialogVM constructor.
//

//*********************************************************************************************
CDialogVM::CDialogVM(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogVM::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogVM)
	radPageSize = 0;

	strPhysicalMem = _T("");
	//}}AFX_DATA_INIT
}


//*********************************************************************************************
//
// CDialogVM member functions.
//

//*********************************************************************************************
//
void CDialogVM::DoDataExchange(CDataExchange* pDX)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogVM)
	DDX_Control(pDX, IDC_SLIDER1, sliderPhysicalMem);
	DDX_Radio(pDX, IDC_RADIO1, radPageSize);
	DDX_Text(pDX, IDC_VM_COUNT, strPhysicalMem);
	//}}AFX_DATA_MAP
}



//*********************************************************************************************
//
void CDialogVM::OnHScroll
(
	UINT        nSBCode,
	UINT        nPos,
	CScrollBar* pScrollBar
)
//
//**************************************
{
	// get the ID of the control so we can switch on it
	char				buf[128];
	int					iscr_id=pScrollBar->GetDlgCtrlID();

	// Call base class member function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// after calling the default handler set the slider position
	nPos=((CSliderCtrl*)pScrollBar)->GetPos();

	switch (iscr_id)
	{
	case IDC_SLIDER1:
		wsprintf(buf, "%d Mb", nPos);
		strPhysicalMem = buf;
		gtxmTexMan.pvmeTextures->SetMaximumPhysical(nPos * 1024 * 1024);
		break;
	}

	UpdateData(false);
}


/*//*********************************************************************************************
//
BOOL CDialogVM::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	UpdateData(false);

	return TRUE;  // return TRUE  unless you set the focus to a control
}*/


//*********************************************************************************************
//
void CDialogVM::OnShowWindow
(
	BOOL bShow,
	UINT nStatus
)
//
// Responds to the WM_SHOWWINDOW message by initializing dialog controls.
//
//**************************************
{
	// Call the base class member function.
	CDialog::OnShowWindow(bShow, nStatus);

	uint32 u4_page = gtxmTexMan.pvmeTextures->u4GetLoadPageSize();
	uint32 u4_mem = gtxmTexMan.pvmeTextures->u4GetMaxPhysicalMem() / (1024*1024);

	char buf[128];

	switch (u4_page)
	{
	case 4*1024:
		radPageSize = 0;
		break;

	case 8*1024:
		radPageSize = 1;
		break;

	case 16*1024:
		radPageSize = 2;
		break;

	case 32*1024:
		radPageSize = 3;
		break;

	case 64*1024:
		radPageSize = 4;
		break;

	case 128*1024:
		radPageSize = 5;
		break;

	case 256*1024:
		radPageSize = 6;
		break;

	case 512*1024:
		radPageSize = 7;
		break;
	}

	// set the range of the slider (in Mb)
	sliderPhysicalMem.SetRange(gtxmTexMan.u4MinimumTextureMemory()/(1024*1024),192,false);
	sliderPhysicalMem.SetPos(u4_mem);

	wsprintf(buf, "%d Mb", u4_mem);
	strPhysicalMem = buf;

	GetDlgItem(IDC_GOVIRTUAL)->EnableWindow(  (gtxmTexMan.pvmeTextures->bAnythingAllocated()) && (!gtxmTexMan.pvmeTextures->bFileBased()) );
	GetDlgItem(IDC_DECOMMIT)->EnableWindow( gtxmTexMan.pvmeTextures->bFileBased() );
	GetDlgItem(IDC_COMMITALL)->EnableWindow( gtxmTexMan.pvmeTextures->bFileBased() );

	if ( (gtxmTexMan.pvmeTextures->bFileBased())
			|| (gtxmTexMan.pvmeTextures->bAnythingAllocated()) )
	{
		GetDlgItem(IDC_CHECK1)->EnableWindow(false);
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck( true );
		// disable the image loader, just to be safe
		CLoadImageDirectory::Enable(false);
	}
	else
	{
		GetDlgItem(IDC_CHECK1)->EnableWindow(true);
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck( (!CLoadImageDirectory::bEnabled()) );
	}

	((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck( CLoadImageDirectory::bAutoCommit );


	((CButton*)GetDlgItem(IDC_CHECK6))->SetCheck( gtxmTexMan.pvmeTextures->bVMLoadThreadSuspended() );
	GetDlgItem(IDC_CHECK6)->EnableWindow( gtxmTexMan.pvmeTextures->bFileBased() );

	UpdateData(false);
}


//*********************************************************************************************
// This is where the shit hits the fan!
//
void CDialogVM::OnGoVirtual
(
)
//**************************************
{
	char	str_save[MAX_PATH];

	// get the name of the first groff loaded.
	const char* str_grf_name = wWorld.strGetGroff(0);

	// WE MUST SAVE THE DIRECTORY FILE BEFORE WE SAVE THE SWAP FILE BECAUSE THE SAVE DIRECTORY
	// FUNCTION CAN MODIFY THE SWAP FILE...
	strcpy(str_save,str_grf_name);
	strcpy(str_save + strlen(str_save) - 4, strPID_FILE_EXTENTSION);
	gtxmTexMan.CreatePackedImageDirectory(str_save);

	strcpy(str_save,str_grf_name);
	strcpy(str_save + strlen(str_save) - 4, strSWP_FILE_EXTENTSION);
	if (!gtxmTexMan.pvmeTextures->bBeginPaging(str_save, gtxmTexMan.pvmsaSmallTextures->u4AllocatorLength(), gtxmTexMan.pvmsaSmallTextures->u4MemoryAllocated(), true))
	{
		MessageBox("Swap file failed to save. SWP and PID files are invalid", "Swap File Error", MB_OK);
	}

	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(true);
	GetDlgItem(IDC_CHECK1)->EnableWindow(false);
	GetDlgItem(IDC_GOVIRTUAL)->EnableWindow(false);
	GetDlgItem(IDC_DECOMMIT)->EnableWindow(true);
	GetDlgItem(IDC_COMMITALL)->EnableWindow(true);

	// disable the image loader
	CLoadImageDirectory::Enable(false);

	((CButton*)GetDlgItem(IDC_CHECK6))->SetCheck( gtxmTexMan.pvmeTextures->bVMLoadThreadSuspended() );
	GetDlgItem(IDC_CHECK6)->EnableWindow(true);
}


//*********************************************************************************************
//
void CDialogVM::OnRadio1
(
)
//**************************************
{
	gtxmTexMan.pvmeTextures->SetLoadPageSize(4*1024);
	radPageSize = 0;
	UpdateData(false);
}

//*********************************************************************************************
//
void CDialogVM::OnRadio2
(
)
//**************************************
{
	gtxmTexMan.pvmeTextures->SetLoadPageSize(8*1024);
	radPageSize = 1;
	UpdateData(false);
}

//*********************************************************************************************
//
void CDialogVM::OnRadio3
(
)
//**************************************
{
	gtxmTexMan.pvmeTextures->SetLoadPageSize(16*1024);
	radPageSize = 2;
	UpdateData(false);
}

//*********************************************************************************************
//
void CDialogVM::OnRadio4
(
)
//**************************************
{
	gtxmTexMan.pvmeTextures->SetLoadPageSize(32*1024);
	radPageSize = 3;
	UpdateData(false);
}

//*********************************************************************************************
//
void CDialogVM::OnRadio5
(
)
//**************************************
{
	gtxmTexMan.pvmeTextures->SetLoadPageSize(64*1024);
	radPageSize = 4;
	UpdateData(false);
}

//*********************************************************************************************
//
void CDialogVM::OnRadio6
(
)
//**************************************
{
	gtxmTexMan.pvmeTextures->SetLoadPageSize(128*1024);
	radPageSize = 5;
	UpdateData(false);
}

//*********************************************************************************************
//
void CDialogVM::OnRadio7
(
)
//**************************************
{
	gtxmTexMan.pvmeTextures->SetLoadPageSize(256*1024);
	radPageSize = 6;
	UpdateData(false);
}

//*********************************************************************************************
//
void CDialogVM::OnRadio8
(
)
//**************************************
{
	gtxmTexMan.pvmeTextures->SetLoadPageSize(512*1024);
	radPageSize = 7;
	UpdateData(false);
}


//*********************************************************************************************
//
void CDialogVM::OnDecommit
(
)
//**************************************
{
	gtxmTexMan.pvmeTextures->DecommitAll();
	((CButton*)GetDlgItem(IDC_CHECK6))->SetCheck( gtxmTexMan.pvmeTextures->bVMLoadThreadSuspended() );
}


//*********************************************************************************************
//
void CDialogVM::OnCheck1
(
)
//**************************************
{
	bool b_button = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
	CLoadImageDirectory::Enable(!b_button);
}

//*********************************************************************************************
//
void CDialogVM::OnCheck2
(
)
//**************************************
{
	CLoadImageDirectory::bAutoCommit = ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck();
}


//*********************************************************************************************
//
void CDialogVM::OnCheck6
(
)
//**************************************
{
	bool b_suspend = ((CButton*)GetDlgItem(IDC_CHECK6))->GetCheck();

	if (b_suspend)
	{
		gtxmTexMan.pvmeTextures->SuspendVMLoadThread();
	}
	else
	{
		gtxmTexMan.pvmeTextures->ResumeVMLoadThread();
	}
}



//*********************************************************************************************
//
void CDialogVM::OnCommitAll
(
)
//**************************************
{
	// commit the whole page file to memory
	gtxmTexMan.pvmeTextures->CommitAll();

	// find out how much memory is commited
	uint32	u4_mem = (gtxmTexMan.pvmeTextures->u4GetMaxPhysicalMem()+512*1024) / (1024*1024);
	char	buf[128];

	wsprintf(buf, "%d Mb", u4_mem);
	strPhysicalMem = buf;
	sliderPhysicalMem.SetPos(u4_mem);
	((CButton*)GetDlgItem(IDC_CHECK6))->SetCheck(true);		// thread is suspended
	UpdateData(false);
}



