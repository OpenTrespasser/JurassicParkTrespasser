// CollisionEditorDlg.cpp : implementation file
//

#include "stdafx.h"

#include "CollisionEditor.h"
#include "CollisionEditorDlg.h"
#include <string>
#include <set>
#include "Database.h"
#include "EditMaterialDlg.h"
#include "SampleListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


bool bProcessLanguageFilename
(
	int iLang,
	CString& cstr_source, 
	CString& cstr_dest
);

bool bPartialLanguageFileName
(
	int iLang,
	CString& cstr_source, 
	CString& cstr_dest
);



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCollisionEditorDlg dialog

CCollisionEditorDlg::CCollisionEditorDlg(CWnd* pParent)
	: CDialog(CCollisionEditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCollisionEditorDlg)
	cstrAddMaterial = _T("");
	cstrEffectID = _T("");
	cstrEffectFile = _T("");
	iLang = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CCollisionEditorDlg::~CCollisionEditorDlg()
{
	delete CAudio::pcaAudio;
}


void CCollisionEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCollisionEditorDlg)
	DDX_Control(pDX, IDC_MATERIAL_LIST, lbMaterialList);
	DDX_Control(pDX, IDC_ADD_EDIT, eAddMaterial);
	DDX_Control(pDX, IDC_EFFECT_LIST, lcEffects);
	DDX_Radio(pDX, IDC_RADIO_ENG, iLang);
	DDX_Text(pDX, IDC_ADD_EDIT, cstrAddMaterial);
	DDX_Text(pDX, IDC_EFFECT_ID_EDIT, cstrEffectID);
	DDX_Text(pDX, IDC_EFFECT_FILE_EDIT, cstrEffectFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCollisionEditorDlg, CDialog)
	//{{AFX_MSG_MAP(CCollisionEditorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_BN_CLICKED(IDC_SAMPLE_PROP, OnSampleProp)
	ON_BN_CLICKED(IDC_LOADFILE, OnLoadfile)
	ON_BN_CLICKED(IDC_WRITEFILE, OnWritefile)
	ON_BN_CLICKED(IDC_NEW_EFFECT, OnAddEffect)
	ON_BN_CLICKED(IDC_RENAME_EFFECT, OnRenameEffect)
	ON_LBN_DBLCLK(IDC_MATERIAL_LIST, OnDblclkMaterialList)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowseSample)
	ON_BN_CLICKED(IDC_DELETE_EFFECT, OnDeleteSample)
	ON_BN_CLICKED(IDC_WRITEBINFILE, OnWriteBinary)
	ON_BN_CLICKED(IDC_TEST_EFFECT, OnTestEffect)
	ON_BN_CLICKED(IDC_STOP_EFFECT, OnStopEffect)
	ON_BN_CLICKED(IDC_CHECK_COLLISIONS, OnEnableCollisions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCollisionEditorDlg message handlers

//**********************************************************************************************
//
BOOL CCollisionEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	lcEffects.InsertColumn(0, "Identifier");
	lcEffects.InsertColumn(1, "Filename", LVCFMT_LEFT, 256, 1);

	lcEffects.SetColumnWidth(0,128);

	CString str_id = "MISSING";
	CString str_f = "missing.cau";
	edbEffects.bNewEffect(str_id,str_f);

	int idx = lcEffects.InsertItem(0, str_id);
	lcEffects.SetItemText(idx,1, str_f);


	SetEffectsModified(false);

	CAudio* pca_audio = new CAudio((void*)m_hWnd, true, true);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//**********************************************************************************************
//
void CCollisionEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

//**********************************************************************************************
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
//
void CCollisionEditorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}



//**********************************************************************************************
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
//
HCURSOR CCollisionEditorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnAdd() 
{
	if (edbEffects.u4TotalEffects() == 0)
	{
		MessageBox("Materials cannot be added while there are no effects.","Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
		return;
	}

	UpdateData(TRUE);

	if (cstrAddMaterial.IsEmpty())
	{
		MessageBox("You cannot add a sound material with no name.","Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		if (edbEffects.bFindMaterialID( cstrAddMaterial ))
		{
			MessageBox("A material with that name already exists.","Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
			return;
		}

		if (edbEffects.bNewMaterial(cstrAddMaterial))
		{
			// Add to dialog listbox, we locate our sound material by its hash value.
			// at this point the unique ID for the material is unique
			int idx = lbMaterialList.AddString(cstrAddMaterial);
			lbMaterialList.SetItemData(idx,edbEffects.u4Hash(cstrAddMaterial));
			eAddMaterial.SetWindowText("");
			SetEffectsModified(true);
		}
		else
		{
			MessageBox("Failed to add to material database.","Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
		}
	}
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnDelete() 
{
	int idx = lbMaterialList.GetCurSel();
	
	if (idx == LB_ERR)
		return;

	if ( MessageBox("Are you sure you want to delete the selected material?","Delete Sound Material",MB_YESNO|MB_ICONQUESTION) == IDNO)
		return;

	// get the hash value
	uint32 u4_hash = (uint32)lbMaterialList.GetItemDataPtr(idx);
	ASSERT(u4_hash != 0);

	edbEffects.DeleteMaterial(u4_hash);
	lbMaterialList.DeleteString(idx);
	SetEffectsModified(true);
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnRename() 
{
	if (edbEffects.u4TotalMaterials() == 0)
	{
		return;
	}

	UpdateData(TRUE);

	if (cstrAddMaterial.IsEmpty())
	{
		MessageBox("First enter the new name.","Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		if (edbEffects.bFindMaterialID( cstrAddMaterial ))
		{
			MessageBox("A material with that name already exists.","Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
			return;
		}
	}

	int idx = lbMaterialList.GetCurSel();

	if (idx == LB_ERR) 
		return;

	edbEffects.RenameMaterial( lbMaterialList.GetItemData(idx), cstrAddMaterial );

	lbMaterialList.DeleteString(idx);
	idx = lbMaterialList.InsertString(idx, cstrAddMaterial);
	lbMaterialList.SetItemData(idx, edbEffects.u4Hash(cstrAddMaterial) );

	SetEffectsModified(true);
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnEdit() 
{
	int idx = lbMaterialList.GetCurSel();

	if (idx == LB_ERR) 
		return;

	// create a dialog box and set the material ID that we want to edit
	CEditMaterialDlg dlg;
	dlg.u4Material = lbMaterialList.GetItemData(idx);

	dlg.DoModal();
	SetEffectsModified(true);
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnCopy() 
{
	int idx = lbMaterialList.GetCurSel();

	if (idx == LB_ERR) 
	{
		MessageBox("Please first select a source material","Trespasser Sound Material Copy",MB_OK|MB_ICONINFORMATION);
	}

	uint32 u4_hash_source = lbMaterialList.GetItemData(idx);
	

	if (edbEffects.u4TotalMaterials() == 0)
	{
		return;
	}

	UpdateData(TRUE);

	if (cstrAddMaterial.IsEmpty())
	{
		MessageBox("Enter the name of the new material.","Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		if (edbEffects.bFindMaterialID( cstrAddMaterial ))
		{
			MessageBox("A material with that name already exists.","Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
			return;
		}

		if (edbEffects.bNewMaterial(cstrAddMaterial))
		{
			// Add to dialog listbox, we locate our sound material by its hash value.
			// at this point the unique ID for the material is unique
			int i = lbMaterialList.AddString(cstrAddMaterial);
			lbMaterialList.SetItemData(i,edbEffects.u4Hash(cstrAddMaterial));

			edbEffects.DuplicateMaterial(edbEffects.u4Hash(cstrAddMaterial), u4_hash_source);
			eAddMaterial.SetWindowText("");
			SetEffectsModified(true);
		}
		else
		{
			MessageBox("Failed to add to material database.","Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
		}
	}
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnDblclkMaterialList() 
{
	OnEdit();
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnLoadfile() 
{
	if (bModified)
	{
		if (MessageBox("Are you sure you want to load? The current database has been modified.", 
			"Trespasser Sound Effect Editor", MB_YESNO|MB_ICONQUESTION) == IDNO)
		{
			return;
		}
	}

	CFileDialog fdlg (TRUE, _T ("eel"), _T ("*.eel"),
		OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
		_T ("Effects Editor Files (*.eel)|*.eel|"), this);

	fdlg.m_ofn.lpstrTitle = "Load Effects File";

	if (fdlg.DoModal () == IDOK)
	{
		if (edbEffects.LoadText(fdlg.m_ofn.lpstrFile))
		{
			lcEffects.DeleteAllItems();
			lbMaterialList.ResetContent();

			for (TEffectVector::iterator i = edbEffects.evEffects.begin(); i<edbEffects.evEffects.end(); ++i)
			{
				int idx = lcEffects.InsertItem(0, (*i).cstrID);
				lcEffects.SetItemText(idx,1, (*i).cstrFilename);
			}

			if (edbEffects.u4TotalMaterials() == 0)
			{
				((CButton*)GetDlgItem(IDC_CHECK_COLLISIONS))->SetCheck(false);
				EnableCollisionWindow(false);
			}
			else
			{
				((CButton*)GetDlgItem(IDC_CHECK_COLLISIONS))->SetCheck(true);
				EnableCollisionWindow(true);
				for (TMaterialVector::iterator j = edbEffects.mvMaterials.begin(); j<edbEffects.mvMaterials.end(); ++j)
				{
					int idx = lbMaterialList.AddString( (*j).cstrID );
					lbMaterialList.SetItemData(idx, edbEffects.u4Hash( (*j).cstrID ) );
				}
	
				eAddMaterial.SetWindowText("");
			}
		}

		SetEffectsModified(false);
	}
}



//**********************************************************************************************
//
void CCollisionEditorDlg::OnWritefile() 
{
	CFileDialog fdlg (FALSE, _T ("eel"), _T ("*.eel"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		_T ("Effects Editor Files (*.eel)|*.eel|"), this);

	fdlg.m_ofn.lpstrTitle = "Save Effects Editor Files";
	if (fdlg.DoModal () == IDOK)
	{
		edbEffects.SaveText(fdlg.m_ofn.lpstrFile);
		SetEffectsModified(false);
	}
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnAddEffect()
{
	int idx;
	UpdateData(true);

	if ((cstrEffectID == "") || (cstrEffectFile == ""))
	{
		MessageBox("Samples must have a valid ID and filename.","Trespasser Sound Effect Editor",MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (edbEffects.bFindEffectID(cstrEffectID))
	{
		MessageBox("Identifier already exists.","Trespasser Sound Effect Editor",MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (!bFileExists(cstrEffectFile))
	{
		MessageBox("File does not exist.","Trespasser Sound Effect Editor",MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (!bFileExtension(cstrEffectFile, "CAU"))
	{
		MessageBox("File is not a CAU audio file.","Trespasser Sound Effect Editor",MB_OK|MB_ICONINFORMATION);
		return;
	}


	if(edbEffects.bNewEffect(cstrEffectID, cstrEffectFile))
	{
		idx = lcEffects.InsertItem(0, cstrEffectID);
		lcEffects.SetItemText(idx, 1, cstrEffectFile);
		SetEffectsModified(true);
	}
	else
	{
		MessageBox("Failed to add to effect database.","Sound Effect Edit",MB_OK|MB_ICONINFORMATION);
	}

	cstrEffectID = "";
	cstrEffectFile = "";

	UpdateData(false);
}

//**********************************************************************************************
//
void CCollisionEditorDlg::OnRenameEffect()
{
	int idx = lcEffects.GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);

	UpdateData(true);

	if (idx == -1)
	{
		MessageBox("Please first select a sample","Trespasser Sound Sample Adjust",MB_OK|MB_ICONINFORMATION);
		return;
	}

	if ((cstrEffectID == "") && (cstrEffectFile == ""))
	{
		MessageBox("To adjust enter an Id or filename.","Trespasser Sound Sample Adjust",MB_OK|MB_ICONINFORMATION);
		return;
	}

	CString	cstr_sel = lcEffects.GetItemText(idx,0);
	CString cstr_msg;

	if (cstrEffectID!="")
	{
		if (edbEffects.u4Hash("MISSING") == edbEffects.u4Hash(cstr_sel))
		{
			MessageBox("Cannot rename the 'MISSING' identifier","Trespasser Sound Sample Delete",MB_OK|MB_ICONINFORMATION);
			return;
		}

		if (edbEffects.bFindEffectID(cstrEffectID))
		{
			MessageBox("Identifier already exists.","Trespasser Sample Adjust",MB_OK|MB_ICONINFORMATION);
			return;
		}
	}

	if (cstrEffectFile!="")
	{
		if (!bFileExists(cstrEffectFile))
		{
			MessageBox("File does not exist.","Trespasser Sound Sample Adjust",MB_OK|MB_ICONINFORMATION);
			return;
		}

		if (!bFileExtension(cstrEffectFile, "CAU"))
		{
			MessageBox("File is not a CAU audio file.","Trespasser Sound Sample Adjust",MB_OK|MB_ICONINFORMATION);
			return;
		}
	}

	if (cstrEffectFile == "")
	{
		cstr_msg = "Are you sure you want to rename identifier '";
		cstr_msg+=cstr_sel;
		cstr_msg+="' to '";
		cstr_msg+=cstrEffectID;
		cstr_msg+="' ?";
	}
	else if (cstrEffectID == "")
	{
		cstr_msg = "Are you sure you want to change the file for identifier '";
		cstr_msg+=cstr_sel;
		cstr_msg+="' to '";
		cstr_msg+=cstrEffectFile;
		cstr_msg+="' ?";
	}
	else
	{
		cstr_msg = "Are you sure you want to rename identifier '";
		cstr_msg+=cstr_sel;
		cstr_msg+="' to '";
		cstr_msg+=cstrEffectID;
		cstr_msg+="' and change the filename?";
	}

	if (MessageBox(cstr_msg,"Sound Effect Adjust",MB_YESNO|MB_ICONQUESTION) == IDNO)
		return;

	SEffect* peff = edbEffects.peffFindEffectID(cstr_sel);

	if ( peff == NULL)
	{
		MessageBox("Cannot find effect address","Trespasser Sound Sample Adjust",MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (cstrEffectID!="")
	{
		uint32 u4_old = peff->u4Hash;

		peff->cstrID = cstrEffectID;
		peff->u4Hash = edbEffects.u4Hash(cstrEffectID);

		// rename all the sample Ids in the collision database
		edbEffects.AdjustMaterialCollisions(u4_old,peff->u4Hash);
	}

	if (cstrEffectFile!="")
	{
		peff->cstrFilename = cstrEffectFile;
	}

	lcEffects.DeleteItem(idx);
	idx = lcEffects.InsertItem(idx, peff->cstrID);
	lcEffects.SetItemText(idx,1, peff->cstrFilename);

	SetEffectsModified(true);

	cstrEffectID = "";
	cstrEffectFile = "";

	UpdateData(false);
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnBrowseSample()
{
	UpdateData(true);
	CFileDialog fdlg (TRUE, _T ("cau"), _T ("*.cau"),
		OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_NOCHANGEDIR,
		_T ("Trespasser Audio Files (*.cau)|*.cau|"), this);

	fdlg.m_ofn.lpstrTitle = "Browse Trespasser Audio Files";
	if (fdlg.DoModal() == IDOK)
	{
		cstrEffectFile = fdlg.m_ofn.lpstrFile;
		UpdateData(false);
	}
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnDeleteSample()
{
	int idx = lcEffects.GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);

	if (idx == -1)
	{
		MessageBox("Please first select a sample to delete","Trespasser Sound Sample Delete",MB_OK|MB_ICONINFORMATION);
		return;
	}

	CString	cstr_sel = lcEffects.GetItemText(idx,0);

	if (edbEffects.u4Hash("MISSING") == edbEffects.u4Hash(cstr_sel))
	{
		MessageBox("Cannot delete the 'MISSING' identifier","Trespasser Sound Sample Delete",MB_OK|MB_ICONINFORMATION);
		return;
	}

	CString	cstr_msg = "Are you sure you want to delete sample '";
	cstr_msg+=cstr_sel;
	cstr_msg+="' ?";

	if (MessageBox(cstr_msg,"Trespasser Sound Sample Delete",MB_YESNO|MB_ICONQUESTION) == IDNO)
		return;


	if (edbEffects.bEffectIDUsed(cstr_sel))
	{
		MessageBox("This effect is used in a collision so cannot be deleted","Delete Sound Material",MB_OK|MB_ICONINFORMATION);
		return;
	}
	else
	{
		if (!edbEffects.bDeleteEffect(cstr_sel))
		{
			MessageBox("Sample not deleted.","Trespasser Sound Sample Delete",MB_OK|MB_ICONINFORMATION);
			return;
		}

		SetEffectsModified(true);
		lcEffects.DeleteItem(idx);
	}
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnWriteBinary() 
{
	UpdateData(true);

	CFileDialog fdlg (FALSE, _T ("tpa"), _T ("*.tpa"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		_T ("Trespasser Packed Audio Files (*.tpa)|*.tpa|"), this);

	fdlg.m_ofn.lpstrTitle = "Save Trespasser Packed Audio File";
	if (fdlg.DoModal () == IDOK)
	{
		HCURSOR csr_wait = AfxGetApp()->LoadStandardCursor(IDC_WAIT);
		HCURSOR csr_old = ::SetCursor(csr_wait);

		edbEffects.SaveBinary(fdlg.m_ofn.lpstrFile,iLang);

		::SetCursor(csr_old);
	}
}


//**********************************************************************************************
//
void CCollisionEditorDlg::SetEffectsModified(bool b_mod)
{
	bModified = b_mod;
	if (bModified)
	{
		SetWindowText("Trespasser Effects Editor [*]");
	}
	else
	{
		SetWindowText("Trespasser Effects Editor");
	}
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnCancel() 
{
	if (bModified)
	{
		if (MessageBox("Are you sure you want to exit? The database has been modified.", 
			"Trespasser Sound Effect Editor", MB_YESNO|MB_ICONQUESTION) == IDNO)
		{
			return;
		}
	}

	delete CEditMaterialDlg::psamTest;
	delete CEditMaterialDlg::psamAudio;
	delete CEditMaterialDlg::psamCol1;
	delete CEditMaterialDlg::psamCol2;

	CEditMaterialDlg::psamTest = NULL;
	CEditMaterialDlg::psamAudio = NULL;
	CEditMaterialDlg::psamCol1 = NULL;
	CEditMaterialDlg::psamCol2 = NULL;

	EndDialog(IDCANCEL);
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnTestEffect() 
{
	UpdateData(true);

	int idx = lcEffects.GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);

	if (idx == -1)
	{
		MessageBox("Please first select a sample","Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
		return;
	}

	CString	cstr_id = lcEffects.GetItemText(idx,0);
	SEffect* peff = edbEffects.peffFindEffectID(cstr_id);

	HCURSOR csr_wait = AfxGetApp()->LoadStandardCursor(IDC_WAIT);
	HCURSOR csr_old = ::SetCursor(csr_wait);

	CString cstr_filename;
	bool b_lang = bProcessLanguageFilename(iLang, peff->cstrFilename, cstr_filename);

	if (b_lang)
	{
		// We have processed the file path, make sure it exists
		if (!bFileExists(cstr_filename))
		{
			CString cstr_rel;
			bPartialLanguageFileName(iLang,peff->cstrFilename,cstr_rel);

			CString	cstr_text;
			cstr_text.Format("File '%s' does not exist, defaulting to English.", (const char*)cstr_rel);

			MessageBox(cstr_text, "Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
			cstr_filename = peff->cstrFilename;
		}
	}

	TRACE2("ID = %s\nFilename = %s\n",(const char*)cstr_id,(const char*)cstr_filename);

	CEditMaterialDlg::TestPlay((const char*)cstr_filename);

	::SetCursor(csr_old);
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnStopEffect() 
{
	delete CEditMaterialDlg::psamTest;
	CEditMaterialDlg::psamTest = NULL;
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnEnableCollisions()
{
	bool b_enabled = (bool)(((CButton*)GetDlgItem(IDC_CHECK_COLLISIONS))->GetCheck());

	if (!b_enabled)
	{
		if (edbEffects.u4TotalMaterials()>0)
		{
			if (MessageBox("Disabling effects will delete all materials. Do you wish to continue?",
				"Trespasser Effects Editor",MB_YESNO|MB_ICONQUESTION) == IDNO)
			{
				((CButton*)GetDlgItem(IDC_CHECK_COLLISIONS))->SetCheck(true);
				return;
			}
		}
		edbEffects.DeleteAllMaterials();
		lbMaterialList.ResetContent();
		SetEffectsModified(true);
	}

	EnableCollisionWindow(b_enabled);
}


//**********************************************************************************************
//
void CCollisionEditorDlg::EnableCollisionWindow
(
	bool	b_enable
)
{
	GetDlgItem(IDC_TEXT_NEW_MAT)->EnableWindow(b_enable);
	GetDlgItem(IDC_TEXT_CUR_MAT)->EnableWindow(b_enable);
	GetDlgItem(IDC_ADD_EDIT)->EnableWindow(b_enable);
	GetDlgItem(IDC_MATERIAL_LIST)->EnableWindow(b_enable);
	GetDlgItem(IDC_ADD)->EnableWindow(b_enable);
	GetDlgItem(IDC_EDIT)->EnableWindow(b_enable);
	GetDlgItem(IDC_COPY)->EnableWindow(b_enable);
	GetDlgItem(IDC_DELETE)->EnableWindow(b_enable);
	GetDlgItem(IDC_RENAME)->EnableWindow(b_enable);
}


//**********************************************************************************************
//
void CCollisionEditorDlg::OnSampleProp
(
)
{
	int idx = lcEffects.GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);

	if (idx == -1)
	{
		MessageBox("Please first select a sample","Trespasser Effects Editor",MB_OK|MB_ICONINFORMATION);
		return;
	}

	CString	cstr_id = lcEffects.GetItemText(idx,0);
	SEffect* peff = edbEffects.peffFindEffectID(cstr_id);

	if (peff == NULL)
		return;

	CSampleListDlg	dlg_prop(peff);
	dlg_prop.DoModal();
}



//**********************************************************************************************
// Language extentions..
//**********************************************************************************************
CString acstrLang[7] = {"finalsamples","finalsamples-frn","finalsamples-ger","finalsamples-spn","finalsamples-prt","finalsamples-jap","finalsamples-chn"};

//**********************************************************************************************
//
bool bProcessLanguageFilename
(
	int i_lang,
	CString& cstr_source, 
	CString& cstr_dest
)
//*************************************
{
	CString cstr_src(cstr_source);
	cstr_src.MakeLower();

	if ((i_lang<1) || (i_lang>6))
	{
		// Copy the source path and return a translated sting that is the same as the source
		cstr_dest = cstr_src;
		return false;
	}

	int i_pos = cstr_src.Find(acstrLang[0]);
	if (i_pos == -1)
	{
		// we have not found the base lang directory so return false
		cstr_dest = cstr_src;
		return false;
	}

	int i_remain = acstrLang[0].GetLength()-1;
	i_remain += i_pos;
	cstr_dest = cstr_src.Left(i_pos);
	cstr_dest += acstrLang[i_lang];
	cstr_dest += cstr_src.Right(cstr_src.GetLength()-i_remain-1);

	return true;
}


//**********************************************************************************************
//
bool bPartialLanguageFileName
(
	int i_lang,
	CString& cstr_source, 
	CString& cstr_dest
)
//*************************************
{
	CString cstr_src(cstr_source);
	cstr_src.MakeLower();

	if ((i_lang<1) || (i_lang>6))
	{
		// Copy the source path and return a translated sting that is the same as the source
		cstr_dest = cstr_src;
		return false;
	}

	int i_pos = cstr_src.Find(acstrLang[0]);
	if (i_pos == -1)
	{
		// we have not found the base lang directory so return false
		cstr_dest = cstr_src;
		return false;
	}

	int i_remain = acstrLang[0].GetLength()-1;
	i_remain += i_pos;
	cstr_dest = "...\\";
	cstr_dest += acstrLang[i_lang];
	cstr_dest += cstr_src.Right(cstr_src.GetLength()-i_remain-1);

	return true;
}