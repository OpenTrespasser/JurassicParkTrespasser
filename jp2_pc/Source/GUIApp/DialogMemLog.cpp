/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents: Implementation of 'DialogMemLog.hpp'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogMemLog.cpp                                              $
 * 
 * 5     12/17/97 12:18a Rwyatt
 * New memory debugging buttons
 * 
 * 4     12/04/97 3:21p Rwyatt
 * Added more memory logs
 * 
 * 3     9/18/97 3:49p Rwyatt
 * Does a dummy read of system memory state to set the mem log conters.
 * 
 * 2     9/18/97 1:23p Rwyatt
 * Basic memory log view.
 * 
 * 1     9/18/97 12:54p Rwyatt
 * Implementation of dialog for memory logging
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "stdafx.h"
#include "DialogMemLog.hpp"
#include "GUIApp.h"
#include "GUIAppDlg.h"
#include "Lib/Std/MemLimits.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/Sys/DebugConsole.hpp"


//*********************************************************************************************
//
// Message map for CDialogMemLog.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogMemLog, CDialog)
	//{{AFX_MSG_MAP(CDialogMemLog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CLEAR, OnClearSelected)
	ON_BN_CLICKED(IDC_FRAGMENT, OnCalcFragmentation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CDialogMemLog constructor.
//

//*********************************************************************************************
CDialogMemLog::CDialogMemLog(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMemLog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogMemLog)
	//}}AFX_DATA_INIT
}


//*********************************************************************************************
//
// CDialogMemLog member functions.
//

//*********************************************************************************************
//
void CDialogMemLog::DoDataExchange(CDataExchange* pDX)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogMemLog)
	DDX_Control(pDX, IDC_LOG_LIST, m_ListLog);
	//}}AFX_DATA_MAP
}


//*********************************************************************************************
//
static char* strFormat(int i)
{
	static char	buf[32];
	
	wsprintf(buf, "%d", i);

	return buf;
}



//*********************************************************************************************
//
void CDialogMemLog::OnShowWindow
(
	BOOL bShow,
	UINT nStatus
)
//
// Responds to the WM_SHOWWINDOW message by initializing dialog controls.
//
//**************************************
{
	m_ListLog.InsertColumn(0, "Description");
	m_ListLog.InsertColumn(1, "Current Count", LVCFMT_LEFT, 128, 1);
	m_ListLog.InsertColumn(2, "Maximum Count", LVCFMT_LEFT, 128, 2);

	// get the width of the widest string
	int i_w = 0;
	for (int t = 0; t<emlEND; t++)
	{
		const char* str_log = MEMLOG_COUNTER_NAME(t);
		if (str_log == NULL)
			continue;
		int w = m_ListLog.GetStringWidth(str_log);
		if (w>i_w)
			i_w=w;
	}

	m_ListLog.SetColumnWidth(0,i_w+16);

	// calling these functions will cause the mem log counters to be updated...
	u4TotalPhysicalMemory();
	u4FreePhysicalMemory();
	u4FreePagefileMemory();
	u4FreeVirtualMemory();

	UpdateList();

	// Call the base class member function.
	CDialog::OnShowWindow(bShow, nStatus);
}


//*********************************************************************************************
//
void CDialogMemLog::OnClearSelected
(
)
//
//**************************************
{
	int sel = m_ListLog.GetSelectedCount();
	int next = -1;
	int e;

	if (sel>0)
	{
		do
		{
			next = m_ListLog.GetNextItem(next, LVNI_ALL|LVNI_SELECTED);

			if (next<0)
				break;

			e = m_ListLog.GetItemData(next);
			if (e!=0xffffffff)
			{
				MEMLOG_SET_COUNTER(e,0);
				m_ListLog.SetItemText(next,1,strFormat( ulGetMemoryLogCounter((EMemoryLog)e)) );
			}
		} while (next>=0);
	}
}


//*********************************************************************************************
//
void CDialogMemLog::UpdateList
(
)
//
//**************************************
{
	// delete all the items in the current list so the list control is empty
	m_ListLog.DeleteAllItems();

	// add all of our items.....
	// go through all memory log counters
	for (int t = 0; t<emlEND; t++)
	{
		int idx;
		const char* str_log = MEMLOG_COUNTER_NAME(t);
		if (str_log)
		{
			idx = m_ListLog.InsertItem(t, str_log);
			m_ListLog.SetItemText(t,1,strFormat( ulGetMemoryLogCounter((EMemoryLog)t)) );
			m_ListLog.SetItemText(t,2,strFormat( ulGetMaxMemoryLogCounter((EMemoryLog)t)) );
			m_ListLog.SetItemData(idx,t);
		}
		else
		{
			idx = m_ListLog.InsertItem(t, "");
			m_ListLog.SetItemData(idx,0xffffffff);
		}
	}
}


extern char* CalculateHeapFragmentationText();

//*********************************************************************************************
//
void CDialogMemLog::OnCalcFragmentation
(
)
//
//**************************************
{
	MessageBox(CalculateHeapFragmentationText(),"Memory fragmentation",MB_OK);
}
