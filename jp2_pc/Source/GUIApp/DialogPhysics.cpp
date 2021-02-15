/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of 'DialogPhysics.hpp.'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogPhysics.cpp                                             $
 * 
 * 19    3/22/98 5:02p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 18    1/22/98 2:49p Pkeet
 * Added includes to accomodate the reduction in the number of includes in 'StdAfx.h.'
 * 
 * 17    11/10/97 1:56p Agrant
 * Fixed some memory leaks
 * 
 * 16    97/10/30 15:15 Speter
 * All instances now have PhysicsInfo.  
 * 
 * 15    10/13/97 3:37p Sblack
 * 
 * 14    97/10/07 18:24 Speter
 * Re-wrote and simplified, to use classes describing scrollbar-to-variable connection.  Added
 * Mass parameter, coupled to Density.  List of externs now specified only once.  Removed most
 * includes.
 * 
 * 13    97/09/29 2:22p Pkeet
 * Added use of the 'fGetScale' function to get the scale of an object instead of
 * 'pr3Presence().rScale.'
 * 
 * 12    97/09/26 16:47 Speter
 * Separated PhysicsInfo.hpp into additional InfoBox.hpp and InfoSkeleton.hpp.
 * 
 * 11    9/05/97 2:42p Pkeet
 * Upped the breakage range.
 * 
 * 10    9/05/97 1:35p Pkeet
 * Slider for fBreakage (magnet strength)
 * 
 * 9     9/05/97 11:45a Pkeet
 * Changed scrollbar range values for ' PARA_TOLERANCE' and 'DIV_TOL.'
 * 
 * 8     9/05/97 1:22a Sblack
 * 
 * 7     97-04-15 16:25 Speter
 * Removed unneeded include.
 * 
 * 6     3/11/97 3:21p Blee
 * Revamped trigger system.
 * 
 * 5     12/19/96 6:50p Agrant
 * Added new physics slider fields.
 * 
 * 4     12/18/96 3:59p Pkeet
 * Added a reset button.
 * 
 * 3     12/18/96 3:15p Pkeet
 * Added functions to place the CInstance name string in the titlebar. Fixed bugs.
 * 
 * 2     12/18/96 1:54p Pkeet
 * Added standard headers and a member function to get a CInstance object pointer and set a
 * local member variable.
 * 
 *********************************************************************************************/

#include "StdAfx.h"
#include "DialogPhysics.hpp"
#include "GUIAppDlg.h"

#include "Lib/Physics/InfoBox.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"
#include "Lib/Sys/Textout.hpp"

// Physics externs.  We're too lazy to make a header file.
extern float	PHY_sleep,
				PHY_number1,
				PHY_number2,

				BC_coarseness,
				IBC_coarseness,
				IBC_depth,
				IBC_kappa,
				IBC_damp,
				one_by_IBC,
				IBC_slide,

				BC_bounce,
				LIBC_kappa,
				LIBC_damp,
				L_slide,
				fBreakage,

				ORTHO_TOLERANCE,
				PARA_TOLERANCE,
				DIV_TOL;

// Switch to dump everything to a log file.
#define bDUMP_EVERYTHING (0)

//**********************************************************************************************
class CItemFloat
// Prefix: imf
{
public:
	const char* strName;					// Name of prop.
	float fMin, fMax;						// Range for sliders.

	//******************************************************************************************
	CItemFloat(const char* str_name, float f_min, float f_max)
		: strName(str_name), fMin(f_min), fMax(f_max)
	{
	}

	//******************************************************************************************
	virtual float fGet() const
	{
		return 0;
	}

	//******************************************************************************************
	virtual void Set(float f)
	{
	}
};

//**********************************************************************************************
class CItemFloatGlobal: public CItemFloat
// Prefix: imf
{
private:
	float* pfVar;

public:
	//******************************************************************************************
	CItemFloatGlobal(const char* str_name, float f_min, float f_max, float* pf)
		: CItemFloat(str_name, f_min, f_max), pfVar(pf)
	{
	}

	//******************************************************************************************
	virtual float fGet() const override
	{
		Assert(pfVar);
		return *pfVar;
	}

	//******************************************************************************************
	virtual void Set(float f) override
	{
		Assert(pfVar);
		*pfVar = f;
	}
};

//**********************************************************************************************
class CItemFloatBox: public CItemFloat
// Prefix: imf
{
private:
	float CPhysicsInfoBox::* pfVar;

public:
	static CInstance* pinsObject;
	static CPhysicsInfoBox* pphibBox;

	//******************************************************************************************
	CItemFloatBox(const char* str_name, float f_min, float f_max, float CPhysicsInfoBox::* pf)
		: CItemFloat(str_name, f_min, f_max), pfVar(pf)
	{
	}

	//******************************************************************************************
	virtual float fGet() const override
	{
		Assert(pfVar);
		return pphibBox ? pphibBox->*pfVar : 0.0;
	}

	//******************************************************************************************
	virtual void Set(float f) override
	{
		Assert(pfVar);
		if (pphibBox)
			pphibBox->*pfVar = f;
	}
};

CInstance* CItemFloatBox::pinsObject;
CPhysicsInfoBox* CItemFloatBox::pphibBox;

//**********************************************************************************************
class CItemMass: public CItemFloat
// Prefix: imf
// Calculates mass from box density.
{
public:

	//******************************************************************************************
	CItemMass(const char* str_name, float f_min, float f_max)
		: CItemFloat(str_name, f_min, f_max)
	{
	}

	//******************************************************************************************
	virtual float fGet() const override
	{
		// Return calculated value.
		if (CItemFloatBox::pphibBox)
		{
			Assert(CItemFloatBox::pinsObject);
			return CItemFloatBox::pphibBox->fMass(CItemFloatBox::pinsObject);
		}
		else
			return 0.0;
	}

	//******************************************************************************************
	virtual void Set(float f) override
	{
		if (CItemFloatBox::pphibBox)
		{
			// Set density instead (converting from mks to cgs).
			CItemFloatBox::pphibBox->fDensity = 
				f * 0.001 / CItemFloatBox::pphibBox->fVolume(CItemFloatBox::pinsObject);
		}
	}
};

//**********************************************************************************************
// The vars.

// Box members.
static CItemFloatBox itemDensity( "Density",		0.01, 10.0,	&CPhysicsInfoBox::fDensity );
static CItemMass	  itemMass( "Mass",			0.1, 1000.0 );
static CItemFloatBox itemFriction( "Friction",		0.01, 50.0,	&CPhysicsInfoBox::fFriction );
static CItemFloatBox itemElasticity( "Elasticity",	0.01, 10.0,	&CPhysicsInfoBox::fElasticity);

// Physics globals.
static CItemFloatGlobal itemSleep( "Sleep",			0, .001,		&PHY_sleep );
static CItemFloatGlobal itemIBC_slide( "IBC_slide",		-5,  5,			&IBC_slide );

// Apparently not used.
//	new CItemFloatGlobal( "DELTA",			0,  100,		&PHY_number2 ),
//	new CItemFloatGlobal( "KAPPA",			0,  1000,		&PHY_number1 ),

static CItemFloatGlobal itemL_slide( "L_slide",		-10,  10,		&L_slide );
static CItemFloatGlobal itemBreakage( "Breakage",		1000, 100000,	&fBreakage );
static CItemFloatGlobal itemORTHO( "ORTHO",			0, .01,			&ORTHO_TOLERANCE );
static CItemFloatGlobal itemPARA( "PARA",			0, .1,			&PARA_TOLERANCE );
static CItemFloatGlobal itemDIV( "DIV",			0, .1,			&DIV_TOL );

static CItemFloatGlobal itemBounce( "Bounce",			0,  1.0,		&BC_bounce );
static CItemFloatGlobal itemL_kappa( "L_kappa",		-20.0, 20.0,	&LIBC_kappa );
static CItemFloatGlobal itemL_damp( "L_damp",			-20.0,  20.0,	&LIBC_damp );
static CItemFloatGlobal itemBC_coarse( "BC_coarse",		0,   .1,		&BC_coarseness );
static CItemFloatGlobal itemIBC_coarse( "IBC_coarse",		0,   .1,		&IBC_coarseness );
static CItemFloatGlobal itemIBC_depth( "IBC_depth",		0,  5.0,		&IBC_depth );
static CItemFloatGlobal itemIBC_kappa( "IBC_kappa",		-5.0,  5.0,		&IBC_kappa );
static CItemFloatGlobal itemIBC_damp( "IBC_damp",		-5.0,  5.0,		&IBC_damp );


static CItemFloat* apimfVars[] =
{
	// Box members.
	&itemDensity,
	&itemMass,
	&itemFriction,
	&itemElasticity,

	// Physics globals.
	&itemSleep,
	&itemIBC_slide,

	// Apparently not used.
//	new CItemFloatGlobal( "DELTA",			0,  100,		&PHY_number2 ),
//	new CItemFloatGlobal( "KAPPA",			0,  1000,		&PHY_number1 ),

	&itemL_slide,
	&itemBreakage,
	&itemORTHO,
	&itemPARA,
	&itemDIV,

	&itemBounce,
	&itemL_kappa,
	&itemL_damp,
	&itemBC_coarse,
	&itemIBC_coarse,
	&itemIBC_depth,
	&itemIBC_kappa,
	&itemIBC_damp,
};

	// Example.
//	{ "Scale", 0, &pinsObject->fGetScale(), 0.01, 10.0 },

#if bDUMP_EVERYTHING

CConsoleBuffer con;	// Buffer to write everything to.

//*********************************************************************************************
//
void DumpEverything
(
	CConsoleBuffer& con,
	CPhysicsInfoBox* pphib
)
//
// Writes all physics info to the console buffer.
//
//**************************************
{
	for (int i = 0; i < iNumArrayElements(apimfVars); i++)
	{
		con.Print("%-18s %1.20f\n", apimfVars[i]->strName, apimfVars[i]->fGet());
	}
}

#endif // bDUMP_EVERYTHING


//*********************************************************************************************
//
// CDialogPhysics implementation.
//

	//*****************************************************************************************
	//
	// CDialogPhysics standard functions.
	//

	//*****************************************************************************************
	CDialogPhysics::CDialogPhysics(CWnd* pParent /*=NULL*/)
		: CDialog(CDialogPhysics::IDD, pParent)
	{
		//{{AFX_DATA_INIT(CDialogPhysics)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}

	//*****************************************************************************************
	void CDialogPhysics::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CDialogPhysics)
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_0, m_Scroll[0]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_1, m_Scroll[1]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_2, m_Scroll[2]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_3, m_Scroll[3]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_4, m_Scroll[4]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_5, m_Scroll[5]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_6, m_Scroll[6]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_7, m_Scroll[7]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_8, m_Scroll[8]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_9, m_Scroll[9]);
		
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_10, m_Scroll[10]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_11, m_Scroll[11]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_12, m_Scroll[12]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_13, m_Scroll[13]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_14, m_Scroll[14]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_15, m_Scroll[15]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_16, m_Scroll[16]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_17, m_Scroll[17]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_18, m_Scroll[18]);
		DDX_Control(pDX, IDC_SCROLL_PHYSICS_19, m_Scroll[19]);
		//}}AFX_DATA_MAP
	}

	//*****************************************************************************************
	BEGIN_MESSAGE_MAP(CDialogPhysics, CDialog)
		//{{AFX_MSG_MAP(CDialogPhysics)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDRESET, OnReset)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	//*****************************************************************************************
	//
	// CDialogPhysics member functions.
	//

	//*****************************************************************************************
	void CDialogPhysics::SetInstance(CInstance* pins)
	{
		Assert(pins);

		pinsObject = pins;

		// Only display box data.
		CItemFloatBox::pinsObject = pinsObject;
		CItemFloatBox::pphibBox = const_cast<CPhysicsInfoBox*>(pinsObject->pphiGetPhysicsInfo()->ppibCast());
	}

	//*****************************************************************************************
	void CDialogPhysics::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		CDialog::OnShowWindow(bShow, nStatus);
		Assert(pinsObject);

	#if bDUMP_EVERYTHING
		if (!con.bIsActive())
			con.SetActive(true);
		if (!con.bFileSessionIsOpen())
			con.OpenFileSession("PhysicsDialogValues.txt");
		con.Print("\nOpening dialog:\n\n");
		DumpEverything(con);
	#endif

		SetupScrollbars();
		DisplayText();
		DisplayValues();
	}

	//*****************************************************************************************
	void CDialogPhysics::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

		// Move the scrollbar in question.
		MoveScrollbar(nSBCode, nPos, pScrollBar);

		// Set the parameter of this object only.
		int i_scroll = static_cast<CScrollFloat*>(pScrollBar) - m_Scroll;
		if (bWithin(i_scroll, 0, iNumArrayElements(apimfVars)))
			apimfVars[i_scroll]->Set(m_Scroll[i_scroll].fGet());

		// Show the scrollbar and text values.
		DisplayValues();

	#if bDUMP_EVERYTHING
		con.Print("\nAfter Scroll:\n\n");
		DumpEverything(con);
	#endif

		// Repaint the main window.
		Update();
	}

	//*****************************************************************************************
	void CDialogPhysics::OnOK() 
	{
		// Get the latest from the scrollbars.
		Update();

		CDialog::OnOK();
	}

	//*****************************************************************************************
	void CDialogPhysics::OnReset() 
	{
		for (int i_scroll = 0; i_scroll < iNumArrayElements(apimfVars); i_scroll++)
		{
			Assert(i_scroll < iNUM_SCROLLS);
			m_Scroll[i_scroll].Reset();
			apimfVars[i_scroll]->Set(m_Scroll[i_scroll].fGet());
		}

		DisplayValues();
		Update();
	}

	//*****************************************************************************************
	void CDialogPhysics::OnCancel() 
	{
		// Reset the values.
		OnReset();
		
		CDialog::OnCancel();
	}

	//*********************************************************************************************
	//
	void CDialogPhysics::Update
	(
	)
	//
	//**************************************
	{
		// Do nothing if the simulation is running.
		if (gmlGameLoop.bCanStep())
			return;

		// Repaint the main window.
		CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());
		Assert(pappdlg);
		pappdlg->Invalidate();
	}

	//*****************************************************************************************
	//
	void CDialogPhysics::DisplayText
	(
	)
	//
	// Sets the text in the title bar.
	//
	//**************************************
	{
		Assert(pinsObject);

		if (pinsObject->strGetInstanceName() == NULL)
			return;

		char str_title[1024];

		// Make the title string.
		strcpy(str_title, "Physics for '");
		strcat(str_title, (const char*)pinsObject->strGetInstanceName() );
		strcat(str_title, "'");

		// Set the dialog title.
		SetWindowText(str_title);
	}

	//*****************************************************************************************
	//
	void CDialogPhysics::DisplayValues
	(
	)
	//
	// Sets the text in static controls to reflect object values.
	//
	//**************************************
	{
		for (int i_scroll = 0; i_scroll < iNumArrayElements(apimfVars); i_scroll++)
		{
			Assert(i_scroll < iNUM_SCROLLS);

			// Get the floating point value.
			float f_value = apimfVars[i_scroll]->fGet();

			// Set the floating point value to the textbox.
			SetDlgItemFloat(this, IDC_STATIC_PHYSICS_VALUE_0 + i_scroll, f_value);

			// Set the scrollbar for the value.
			m_Scroll[i_scroll].SetPos(apimfVars[i_scroll]->fGet());
		}
	}

	//*****************************************************************************************
	//
	// CDialogPhysics ALL YOU NEED IS HERE!!!
	//

	//*****************************************************************************************
	//
	void CDialogPhysics::SetupScrollbars
	(
	)
	//
	// Sets the text in static controls to reflect object values.
	//
	//**************************************
	{
		for (int i = 0; i < iNumArrayElements(apimfVars); i++)
		{
			Assert(i < iNUM_SCROLLS);

			CItemFloat* pimf = apimfVars[i];

			// Set the name of the value.
			SetDlgItemText(IDC_STATIC_PHYSICS_LABEL_0 + i, pimf->strName);
			
			// Set the scrollbar for the value.
			m_Scroll[i].Set(pimf->fGet(), pimf->fMin, pimf->fMax, 1000);
		}
	}

	//*********************************************************************************************
	//
	void CDialogPhysics::GetValuesFromScrollbars
	(
	)
	//
	// Gets the values from the scrollbars and places them in their object's parameter list.
	//
	//**************************************
	{
		for (int i = 0; i < iNumArrayElements(apimfVars); i++)
		{
			Assert(i < iNUM_SCROLLS);
			apimfVars[i]->Set(m_Scroll[i].fGet());
		}
	}
