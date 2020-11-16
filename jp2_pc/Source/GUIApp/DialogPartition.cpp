/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Implementation of DialogPartition.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogPartition.cpp                                           $
 * 
 * 9     1/30/98 4:34p Pkeet
 * Removed the fast partition build.
 * 
 * 8     1/22/98 2:49p Pkeet
 * Added includes to accomodate the reduction in the number of includes in 'StdAfx.h.'
 * 
 * 7     1/20/98 5:18p Pkeet
 * Added a fast create button.
 * 
 * 6     1/20/98 3:25p Agrant
 * Now allows user to load the partition parts of a .scn file
 * 
 * 5     12/01/97 3:29p Agrant
 * Hacked out the old partition loading.  Must revisit soon.
 * 
 * 4     97/11/13 12:29p Pkeet
 * Fixed division by zero bug.
 * 
 * 3     97/09/25 1:24p Pkeet
 * Made the load and save features operable.
 * 
 * 2     97/09/25 11:42a Pkeet
 * Added a callback to the 'BuildOptimalPartitions' function for reporting progress.
 * 
 * 1     97/09/24 10:55a Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#include "StdAfx.h"
#include "GUIApp.h"
#include "DialogPartition.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"

CDialogPartition* pdlgPartition = 0;

void PartitionBuildCallback(int i_count);

/////////////////////////////////////////////////////////////////////////////
// CDialogPartition dialog


CDialogPartition::CDialogPartition(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPartition::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogPartition)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogPartition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogPartition)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogPartition, CDialog)
	//{{AFX_MSG_MAP(CDialogPartition)
	ON_BN_CLICKED(ID_CREATE, OnCreate)
	ON_BN_CLICKED(ID_LOAD, OnLoad)
	ON_BN_CLICKED(ID_SAVE, OnSave)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int iEstimateNumNodes(int i)
{
	//if (i <= 3)
		return i;
	//return i + iEstimateNumNodes(i >> 1);
}

void CDialogPartition::Update(int i_count)
{
	static int i_num_done;
	static int i_num_estimated;

	switch (i_count)
	{
		case 0:
			break;
		case 1:
			++i_num_done;
			if ((i_num_done & 3) == 0)
			{
				int i_remaining = Max(i_num_estimated - i_num_done, 0);
				::SetDlgItemInt(m_hWnd, IDC_REMAINING, i_remaining, TRUE);
				if (i_num_estimated == 0)
					i_num_estimated = 1;
				int i_percent = (Max(i_num_done, 0) * 100) / i_num_estimated;
				::SetDlgItemInt(m_hWnd, IDC_PERCENT_DONE, i_percent, TRUE);
			}
			break;
		case -1:
			::SetDlgItemText(m_hWnd, IDC_ACTION, "Flattening");
			break;
		case -2:
			::SetDlgItemText(m_hWnd, IDC_ACTION, "Stuffing");
			break;
		case -3:
			::SetDlgItemText(m_hWnd, IDC_ACTION, "Generating");
			break;
		case -4:
			::SetDlgItemText(m_hWnd, IDC_ACTION, "Initializing");
			break;
		default:
			i_num_done = 0;
			i_num_estimated = iEstimateNumNodes(i_count);
			::SetDlgItemInt(m_hWnd, IDC_REMAINING, i_num_estimated, TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDialogPartition message handlers

void CDialogPartition::OnCreate() 
{
	wWorld.BuildOptimalPartitions(PartitionBuildCallback);
	::SetDlgItemText(m_hWnd, IDC_ACTION, "Done");
}

void CDialogPartition::OnLoad() 
{
	char str_filename[1024];

	// Call the file dialog to get file name.
	bool b_found = bGetFilename
	(
		m_hWnd,
		str_filename,
		sizeof(str_filename),
		"Load Spatial Partitions",
		"Scene (*.scn)\0*.scn\0All (*.*)\0*.*\0"
	);
	
	// Indicate if action cancelled.
	if (!b_found)
	{
		MessageBox("Action cancelled.", "Partitions File", MB_OK | MB_ICONHAND);
		return;
	}
	Assert(*str_filename);

	// Load the partitions in the file into the world.
	Verify(wWorld.bLoadPartitionFromScene(str_filename));
//	Assert(0);
}

void CDialogPartition::OnSave() 
{
	char str_filename[1024];

	// Call the file dialog to get file name.
	bool b_found = bGetFilename
	(
		m_hWnd,
		str_filename,
		sizeof(str_filename),
		"Save Spatial Partitions",
		"Partition (*.prt)\0*.prt\0All (*.*)\0*.*\0"
	);
	
	// Indicate if action cancelled.
	if (!b_found)
	{
		MessageBox("Action cancelled.", "Partitions File", MB_OK | MB_ICONHAND);
		return;
	}
	Assert(*str_filename);

	// Save partitions to the file.
//	wWorld.SaveSpatialPartitions(str_filename);	
	Assert(0);
}

void CDialogPartition::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	pdlgPartition = this;	
	::SetDlgItemText(m_hWnd, IDC_ACTION, "None");
	::SetDlgItemInt(m_hWnd, IDC_REMAINING, 0, TRUE);
	::SetDlgItemInt(m_hWnd, IDC_PERCENT_DONE, 0, TRUE);
}

void PartitionBuildCallback(int i_count)
{
	Assert(pdlgPartition);
	pdlgPartition->Update(i_count);
}