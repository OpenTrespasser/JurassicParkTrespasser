/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998
 *
 * Contents: Implementation of EditSoundMaterial dialog
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogSoundMaterial.cpp                                       $
 * 
 * 3     98/05/18 19:43 Speter
 * Changed GetMaterialType to smatGetMaterialType.
 * 
 * 2     5/15/98 6:59p Rwyatt
 * A dialog the same as the collision editor available from GUIapp.
 * 
 * 1     5/14/98 2:18a Rwyatt
 * Initial implementation
 * 
 *********************************************************************************************/

//
// Includes.
//
#include "StdAfx.h"

#include "mmsystem.h"
#include "Lib/Audio/SoundTypes.hpp"
#include "Lib/Audio/Audio.hpp"

#include "DialogSoundMaterial.hpp"
#include "Lib/Audio/AudioDaemon.hpp"
#include "Lib/Audio/AudioLoader.hpp"
#include "Lib/Physics/InfoBox.hpp"

//*********************************************************************************************
//
// CDialogSoundMaterial implementation.
//
static uint64 u8CollisionHash(uint32 u4_mat1, uint32 u4_mat2);
const char* strFloatText(char* text,float val);


//*********************************************************************************************
//
// Message map for CDialogSoundMaterial.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogSoundMaterial, CDialog)
	//{{AFX_MSG_MAP(CDialogSoundMaterial)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_CLOSE()

	ON_LBN_SELCHANGE(IDC_MSET_LIST,		OnSelChangeList)
	ON_CBN_SELCHANGE(IDC_COMBO1,		OnSample1Change)
	ON_CBN_SELCHANGE(IDC_COMBO2,		OnSample2Change)
	ON_CBN_SELCHANGE(IDC_COMBO3,		OnSample3Change)

	ON_BN_CLICKED(IDC_COLLISION_ENABLE,	OnEnableCollision)
	ON_BN_CLICKED(IDC_SLIDE_ENABLE,		OnEnableSlide)
	ON_BN_CLICKED(IDC_SM_RADIO1,		OnRadio)
	ON_BN_CLICKED(IDC_SM_RADIO2,		OnRadio)
	ON_BN_CLICKED(IDC_SM_RADIO4,		OnRadio)
	ON_BN_CLICKED(IDC_EM_CENTER,		OnCenterSample)
	ON_BN_CLICKED(IDC_STOP_COLLIDE,		OnStopSample)
	ON_BN_CLICKED(IDC_REFWF_TEST_S1,	OnTestS1)
	ON_BN_CLICKED(IDC_REFWF_TEST_S2,	OnTestS2)
	ON_BN_CLICKED(IDC_REFWF_TEST_S3,	OnTestS3)
	ON_BN_CLICKED(IDC_DUAL_CHECK,		OnDualClick)
	ON_BN_CLICKED(IDC_TEST_SAMPLE,		OnTestSingle)
	ON_BN_CLICKED(IDC_COLLIDE,			OnTestDual)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CDialogSoundMaterial constructor.
//

//*********************************************************************************************
CDialogSoundMaterial::	CDialogSoundMaterial(CWnd* pParent /*= NULL*/)
	: CDialog(CDialogSoundMaterial::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSoundMaterial)
	strWorldPos		= _T("");
	//}}AFX_DATA_INIT

	 // Check if we are OK to init..
	bInit			= false;
	fWorldScale		= 10.0f;
	fTestVel		= 0.0f;
	fXWorldPos		= 0.0f;
	fYWorldPos		= 0.0f;

	psamCol1 = NULL;
	psamCol2 = NULL;
	psamAudio = NULL;
	psamTest = NULL;

	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();
	TFileSampleHash*	pfsh = padAudioDaemon->padatEffects->pfshGetSamples();

	if (pch && pfsh)
		bInit = true;
}


//*********************************************************************************************
//
// CDialogSoundMaterial member functions.
//

//*********************************************************************************************
//
void CDialogSoundMaterial::DoDataExchange(CDataExchange* pDX)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSoundMaterial)
	DDX_Control(pDX,		IDC_MSET_LIST,			msetList);
	DDX_Control(pDX,		IDC_COMBO1,				cmbSample1);
	DDX_Control(pDX,		IDC_COMBO2,				cmbSample2);
	DDX_Control(pDX,		IDC_COMBO3,				cmbSample3);
	DDX_Control(pDX, IDC_SM_SLIDER1,			sliderVolSlope);
	DDX_Control(pDX, IDC_SM_SLIDER2,			sliderVolIntersect);
	DDX_Control(pDX, IDC_SM_SLIDER3,			sliderVolMin);
	DDX_Control(pDX, IDC_SM_SLIDER4,			sliderVolMax);
	DDX_Control(pDX, IDC_SM_SLIDER5,			sliderPitSlope);
	DDX_Control(pDX, IDC_SM_SLIDER6,			sliderPitInt);
	DDX_Control(pDX, IDC_SM_SLIDER7,			sliderPitMin);
	DDX_Control(pDX, IDC_SM_SLIDER8,			sliderPitMax);
	DDX_Control(pDX, IDC_SM_SLIDER9,			sliderTestVel);
	DDX_Control(pDX, IDC_SM_SLIDER11,			sliderWorldY);
	DDX_Control(pDX, IDC_SM_SLIDER12,			sliderWorldX);
	DDX_Control(pDX, IDC_SM_SLIDER13,			sliderWorldScale);
	DDX_Control(pDX, IDC_SM_SLIDER14,			sliderAttenuate);
	DDX_Radio(pDX,			IDC_SM_RADIO1,			i4SampleSelect);

	DDX_Text(pDX,	IDC_WORLD_POS_TEXT,			strWorldPos);
	//}}AFX_DATA_MAP
}



//*********************************************************************************************
//
void CDialogSoundMaterial::OnShowWindow
(
	BOOL bShow,
	UINT nStatus
)
//
// Responds to the WM_SHOWWINDOW message by initializing dialog controls.
//
//**************************************
{
	UpdateData(false);
}


//*********************************************************************************************
//
void CDialogSoundMaterial::OnHScroll
(
	UINT nSBCode, 
	UINT nPos, 
	CScrollBar* pScrollBar
)
//
// respond to WM_HSCROLL to move the slider bars.
//
//**************************************
{
	// get the ID of the control so we can switch on it
	int					iscr_id=pScrollBar->GetDlgCtrlID();
	CClientDC			dc(this);
	WINDOWPLACEMENT		wndpl;
	bool				b_drawvol=FALSE,b_drawpit=FALSE,b_drawworld=FALSE;

	// Call base class member function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// after calling the default handler set the slider position
	nPos=((CSliderCtrl*)pScrollBar)->GetPos();

	// which slider have we clicked on, we have the position so do not care
	// about the scroll code
	switch (iscr_id)
	{
	case IDC_SM_SLIDER1:
		pst->fVolSlope = slider_to_vol_slope(nPos);
		GetDlgItem(IDC_VOL_SLOPE_TEXT)->SetWindowText(strFloatText("Slope = %.3f",pst->fVolSlope));
		b_drawvol=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER2:
		pst->fVolInt = slider_to_vol_intersect(nPos);
		GetDlgItem(IDC_VOL_INTS_TEXT)->SetWindowText(strFloatText("Intersection = %.3f",pst->fVolInt));
		b_drawvol=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER3:
		pst->fVolMin = slider_to_vol_region(nPos);
		if (pst->fVolMin>pst->fVolMax-0.005)
		{
			pst->fVolMin=pst->fVolMax-0.005;
			sliderVolMin.SetPos(vol_region_to_slider(pst->fVolMin));
		}
		GetDlgItem(IDC_VOL_MIN_TEXT)->SetWindowText(strFloatText("Minimum Volume = %.3f",pst->fVolMin));
		b_drawvol=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER4:
		pst->fVolMax = slider_to_vol_region(nPos);
		if (pst->fVolMax<pst->fVolMin+0.005)
		{
			pst->fVolMax=pst->fVolMin+0.005;
			sliderVolMax.SetPos(vol_region_to_slider(pst->fVolMax));
		}
		GetDlgItem(IDC_VOL_MAX_TEXT)->SetWindowText(strFloatText("Maximum Volume = %.3f",pst->fVolMax));
		b_drawvol=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER5:
		pst->fPitchSlope = slider_to_pitch_slope(nPos);
		GetDlgItem(IDC_PIT_SLOPE_TEXT)->SetWindowText(strFloatText("Slope = %.3f",pst->fPitchSlope));
		b_drawpit=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER6:
		pst->fPitchInt = slider_to_pitch_intersect(nPos);
		GetDlgItem(IDC_PIT_INTS_TEXT)->SetWindowText(strFloatText("Intersection = %.3f",pst->fPitchInt));
		b_drawpit=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER7:
		pst->fPitchMin = slider_to_pitch_region(nPos);
		if (pst->fPitchMin>pst->fPitchMax-0.005)
		{
			pst->fPitchMin=pst->fPitchMax-0.005;
			sliderPitMin.SetPos(vol_region_to_slider(pst->fPitchMin));
		}
		GetDlgItem(IDC_PIT_MIN_TEXT)->SetWindowText(strFloatText("Minimum Pitch = %.3f",pst->fPitchMin));
		b_drawpit=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER8:
		pst->fPitchMax = slider_to_pitch_region(nPos);
		if (pst->fPitchMax<pst->fPitchMin+0.005)
		{
			pst->fPitchMax=pst->fPitchMin+0.005;
			sliderPitMax.SetPos(pitch_region_to_slider(pst->fPitchMax));
		}
		GetDlgItem(IDC_PIT_MAX_TEXT)->SetWindowText(strFloatText("Maximum Pitch = %.3f",pst->fPitchMax));
		b_drawpit=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER9:
		fTestVel = slider_to_vel(nPos);
		GetDlgItem(IDC_TEST_VELOCITY_TEXT)->SetWindowText(strFloatText("Test Velocity = %.3f",fTestVel));

		// draw both graphs for the velocit slider
		b_drawvol=TRUE;
		b_drawpit=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER12:
		fXWorldPos = slider_to_worldpos(nPos);
		WorldPosText();
		PositionSamples();
		b_drawworld = TRUE;
		break;

	case IDC_SM_SLIDER13:
		fWorldScale = slider_to_scale(nPos);
		GetDlgItem(IDC_WORLD_SCALE_TEXT)->SetWindowText(strFloatText("World Size = %.3f",fWorldScale));
		SetWorldPos();
		PositionSamples();
		b_drawworld = TRUE;
		break;

	case IDC_SM_SLIDER14:
		pst->fAttenuate = slider_to_attenuate(nPos);
		GetDlgItem(IDC_ATTENUATE_TEXT)->SetWindowText(strFloatText("Attenuation = -%.2f dB/m",pst->fAttenuate));
		b_drawworld = TRUE;
		AdjustSamples();
		break;
	}


	//
	// Draw any graph that is flagged for update
	//
	if (b_drawvol)
	{
		// draw the volume graph
		GetDlgItem(IDC_GRAPH_VOL)->GetWindowPlacement(&wndpl);
		DrawGraph(dc,wndpl.rcNormalPosition,
					fVOL_MIN_X_AXIS,fVOL_MAX_X_AXIS,fVOL_MIN_Y_AXIS,fVOL_MAX_Y_AXIS,
					pst->fVolSlope,pst->fVolInt,pst->fVolMin,pst->fVolMax,fTestVel);
	}


	if (b_drawpit)
	{
		// draw the pitch graph
		GetDlgItem(IDC_GRAPH_PIT)->GetWindowPlacement(&wndpl);
		DrawGraph(dc,wndpl.rcNormalPosition,
					fPIT_MIN_X_AXIS,fPIT_MAX_X_AXIS,fPIT_MIN_Y_AXIS,fPIT_MAX_Y_AXIS,
					pst->fPitchSlope,pst->fPitchInt,pst->fPitchMin,pst->fPitchMax,fTestVel);
	}

	if (b_drawworld)
	{
		GetDlgItem(IDC_GRAPH_WORLD)->GetWindowPlacement(&wndpl);
		DrawWorld(dc,wndpl.rcNormalPosition);
	}
}


//*********************************************************************************************
//
void CDialogSoundMaterial::OnVScroll
(
	UINT nSBCode, 
	UINT nPos, 
	CScrollBar* pScrollBar
)
//
// respond to WM_HSCROLL to move the slider bars.
//
//**************************************
{
	// get the ID of the control so we can switch on it
	int					iscr_id=pScrollBar->GetDlgCtrlID();
	CClientDC			dc(this);
	WINDOWPLACEMENT		wndpl;
	bool				b_drawworld=FALSE;

	// Call base class member function.
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

	// after calling the default handler set the slider position
	nPos=((CSliderCtrl*)pScrollBar)->GetPos();

	switch (iscr_id)
	{
	case IDC_SM_SLIDER11:
		fYWorldPos = -slider_to_worldpos(nPos);
		WorldPosText();
		PositionSamples();
		b_drawworld = TRUE;
		break;
	}

	if (b_drawworld)
	{
		GetDlgItem(IDC_GRAPH_WORLD)->GetWindowPlacement(&wndpl);
		DrawWorld(dc,wndpl.rcNormalPosition);
	}
}



//*********************************************************************************************
void CDialogSoundMaterial::SetInstance(CInstance* pins)
{
	pinsCurrent = pins;

	u4Collisions = 0;
	i4SampleSelect = 0;
	UpdateData(false);

	// Get the current material ID
	CPhysicsInfo* pphi = (CPhysicsInfo*)pins->pphiGetPhysicsInfo();
	if (pphi)
	{
		u4Material = pphi->smatGetMaterialType();
	}
	else
	{
		AlwaysAssert(0);
	}

	SetCollisions();

	fTestVel = 0.0f;
	sliderTestVel.SetRange(0,iTEST_VEL_SLIDER_MAX, TRUE );
	sliderTestVel.SetPos(vel_to_slider(fTestVel));
	GetDlgItem(IDC_TEST_VELOCITY_TEXT)->SetWindowText(strFloatText("Test Velocity = %.3f",fTestVel));

	fWorldScale = 10.0f;
	sliderWorldScale.SetRange(0,(int)fWORLD_SCALE_SLIDER_MAX, TRUE );
	sliderWorldScale.SetPos(scale_to_slider(fWorldScale));
	GetDlgItem(IDC_WORLD_SCALE_TEXT)->SetWindowText(strFloatText("World Size = %.3f",fWorldScale));

	fXWorldPos = 0.0f;
	fYWorldPos = 0.0f;
	SetWorldPos();

	Invalidate();
}


//*********************************************************************************************
void CDialogSoundMaterial::SetCollisions()
{
	uint32						u4_count = 0;
	TCollisionHash::iterator	k;
	TCollisionHash*				pch = padAudioDaemon->padatEffects->pchGetCollisions();
	char						buf[256];

	AddIdentifiersToCombo(cmbSample1);
	AddIdentifiersToCombo(cmbSample2);
	AddIdentifiersToCombo(cmbSample3);

	uint32 u4_lbcount = (uint32)msetList.GetCount();

	msetList.ResetContent();

	// Go through the map element by element
	for (k = (*pch).begin(); k!=(*pch).end(); ++k)
	{
		bool b_found = false;
		if ( (uint32)(((*k).first) & 0x00000000ffffffff) == (uint32)u4Material)
		{
			// Bottom 32 bits of hash key match the current material so add the top 32 bits
			// as a collision ID material.
			asndhndCollisions[u4_count] = (uint32)((((*k).first) & 0xffffffff00000000)>>32);
			b_found = true;
		}
		else if ( (uint32)((((*k).first) & 0xffffffff00000000)>>32) == (uint32)u4Material)
		{
			// Top 32 bits of hash key match the current material so add the bottom 32 bits
			// as a collision ID material.
			asndhndCollisions[u4_count] = (uint32)(((*k).first) & 0x00000000ffffffff);
			b_found = true;
		}

		if (b_found)
		{
			wsprintf(buf,"%x",asndhndCollisions[u4_count]);
			msetList.InsertString(u4_count,buf);
			msetList.SetItemData(u4_count,asndhndCollisions[u4_count]);
			u4_count++;
		}
	}


	u4Collisions = u4_count;
	u4Selected = 0;

	if (u4_count == 0)
	{
		// disable the whole window
		wsprintf(buf,"No collisions for %x", u4Material);
		pst=NULL;
		bValid = false;
		Enable(false, false, false);
		GetDlgItem(IDC_MSET_LIST)->EnableWindow(false);
		GetDlgItem(IDC_COLLISION_ENABLE)->EnableWindow(false);
		GetDlgItem(IDC_SLIDE_ENABLE)->EnableWindow(false);
		GetDlgItem(IDC_STOP_COLLIDE)->EnableWindow(false);
	}
	else
	{
		// Select the top entry
		msetList.SetCurSel(0);
		SetupCollision(u4Material, asndhndCollisions[u4Selected]);
		GetDlgItem(IDC_MSET_LIST)->EnableWindow(true);
		GetDlgItem(IDC_COLLISION_ENABLE)->EnableWindow(true);
		GetDlgItem(IDC_SLIDE_ENABLE)->EnableWindow(true);
		GetDlgItem(IDC_STOP_COLLIDE)->EnableWindow(true);
	}
}


//*********************************************************************************************
//
void CDialogSoundMaterial::OnSelChangeList()
{
	int idx = msetList.GetCurSel();
	uint32 u4_hash = (uint32)msetList.GetItemData(idx);

	SetupCollision(u4Material, u4_hash);
}


//*********************************************************************************************
//
void CDialogSoundMaterial::SetupCollision(uint32 u4_mat1, uint32 u4_mat2)
{
	u8Collision =				u8CollisionHash(u4_mat1, u4_mat2);
	TCollisionHash*				pch = padAudioDaemon->padatEffects->pchGetCollisions();

	bSlide		= (*pch)[u8Collision]->bSlide();
	bCollision	= (*pch)[u8Collision]->bCollision();
	bDual		= (*pch)[u8Collision]->u4CollisionSamples() == 2;
	Enable(bCollision, bSlide, bDual);

	char buf[256];
	wsprintf(buf,"Adjust Collision [%x/%x]", u4_mat1, u4_mat2);
	SetWindowText(buf);
}


//*********************************************************************************************
//
void CDialogSoundMaterial::Enable(bool b_collision, bool b_slide, bool b_dual)
{
	bCollision	= b_collision;
	bSlide		= b_slide;
	bValid		= (b_collision|b_slide);
	bDual		= b_dual & b_collision;

	UpdateData(true);

	((CButton*)GetDlgItem(IDC_COLLISION_ENABLE))->SetCheck(b_collision);
	((CButton*)GetDlgItem(IDC_DUAL_CHECK))->SetCheck(b_dual);
	((CButton*)GetDlgItem(IDC_SLIDE_ENABLE))->SetCheck(b_slide);
	GetDlgItem(IDC_DUAL_CHECK)->EnableWindow(b_collision);
	GetDlgItem(IDC_COMBO1)->EnableWindow(b_collision);
	GetDlgItem(IDC_COMBO2)->EnableWindow(b_collision & b_dual);
	GetDlgItem(IDC_COMBO3)->EnableWindow(b_slide);
	GetDlgItem(IDC_SM_RADIO1)->EnableWindow(b_collision);
	GetDlgItem(IDC_SM_RADIO2)->EnableWindow(b_collision & b_dual);
	GetDlgItem(IDC_SM_RADIO4)->EnableWindow(b_slide);
	GetDlgItem(IDC_REFWF_TEST_S1)->EnableWindow(b_collision);
	GetDlgItem(IDC_REFWF_TEST_S2)->EnableWindow(b_collision & b_dual);
	GetDlgItem(IDC_REFWF_TEST_S3)->EnableWindow(b_slide);

	GetDlgItem(IDC_WORLD_SCALE_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_TEST_VELOCITY_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER1)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER2)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER3)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER4)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER5)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER6)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER7)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER8)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER9)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER11)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER12)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER13)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_SM_SLIDER14)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_EM_CENTER)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_COLLIDE)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_TEST_SAMPLE)->EnableWindow(b_collision|b_slide);

	GetDlgItem(IDC_VOL_SLOPE_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_VOL_MIN_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_VOL_MAX_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_VOL_INTS_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_ATTENUATE_TEXT)->EnableWindow(b_collision|b_slide);		

	GetDlgItem(IDC_PIT_SLOPE_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_PIT_MIN_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_PIT_MAX_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_PIT_INTS_TEXT)->EnableWindow(b_collision|b_slide);

	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();

	if (bSlide)
	{
		SetComboBoxItem(cmbSample3,(*pch)[u8Collision]->sndhndSamples[2]);
		if (!bCollision)
		{
			i4SampleSelect = 2;
		}
	}
	else
	{
		if (i4SampleSelect == 2)
		{
			if (bCollision)
			{
				i4SampleSelect = 0;
			}
		}
		SetComboBoxItem(cmbSample3,sndhndHashIdentifier("MISSING"));
	}

	if (bCollision)
	{
		SetComboBoxItem(cmbSample1,(*pch)[u8Collision]->sndhndSamples[0]);
		if (bDual)
		{
			SetComboBoxItem(cmbSample2,(*pch)[u8Collision]->sndhndSamples[1]);
		}
		else
		{
			SetComboBoxItem(cmbSample2,sndhndHashIdentifier("MISSING"));
		}

		if ((i4SampleSelect == 1) && (!b_dual))
		{
			i4SampleSelect = 0;
		}
	}
	else
	{
		if (i4SampleSelect<2)
		{
			if (bSlide)
			{
				i4SampleSelect = 2;
			}
		}
		SetComboBoxItem(cmbSample1,sndhndHashIdentifier("MISSING"));
		SetComboBoxItem(cmbSample2,sndhndHashIdentifier("MISSING"));
	}

	UpdateData(false);

	UpdateSelectedGraph();
}


//**********************************************************************************************
void CDialogSoundMaterial::SetComboBoxItem(CComboBox& cmb,uint32 u4_data)
{
	uint32 u4_count = (uint32)cmb.GetCount();
	uint32 u4_set = 0;

	for (uint32 u4=0; u4<u4_count; u4++)
	{
		if (cmb.GetItemData(u4) == u4_data)
		{
			u4_set = u4;
		}
	}

	cmb.SetCurSel(u4_set);
}


//**********************************************************************************************
void CDialogSoundMaterial::AddIdentifiersToCombo(CComboBox& cmb)
{
	char buf[1024];
	uint32 u4_count = (uint32)cmb.GetCount();
	uint32 u4_set = 1;

	TFileSampleHash*	pfsh = padAudioDaemon->padatEffects->pfshGetSamples();

	cmb.ResetContent();

	// Add the new identifiers
	for (TFileSampleHash::iterator i = (*pfsh).begin(); i!=(*pfsh).end(); ++i)
	{
		wsprintf(buf,"%d - %x",u4_set,(*i).first);
		int idx = cmb.AddString( buf );
		cmb.SetItemData(idx, (*i).first );
		u4_set++;
	}

	cmb.SetCurSel(0);
}



//*********************************************************************************************
void CDialogSoundMaterial::UpdateSelectedGraph()
{
	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();

	if (bCollision || bSlide)
	{
		switch (i4SampleSelect)
		{
		case 0:
			pst = &(*pch)[u8Collision]->stTransfer[0];
			GetDlgItem(IDC_COLLIDE)->EnableWindow(bDual);
			GetDlgItem(IDC_TEST_SAMPLE)->SetWindowText("Test Collision Sample 1");
			break;
		case 1:
			pst = &(*pch)[u8Collision]->stTransfer[1];
			GetDlgItem(IDC_COLLIDE)->EnableWindow(bDual);
			GetDlgItem(IDC_TEST_SAMPLE)->SetWindowText("Test Collision Sample 2");
			break;
		case 2:
			pst = &(*pch)[u8Collision]->stTransfer[2];
			GetDlgItem(IDC_COLLIDE)->EnableWindow(false);
			GetDlgItem(IDC_TEST_SAMPLE)->SetWindowText("Test Slide Sample");
			break;
		default:
			pst = NULL;
		}
	}
	else
	{
		pst = NULL;
	}

	SetGraphs();

	Invalidate();
}


//*********************************************************************************************
void CDialogSoundMaterial::SetGraphs()
{
	if (pst == NULL)
		return;

	if (!bValid)
		return;

	// Volume function
	sliderVolSlope.SetRange( -iVOL_SLIDER_MAX_SLOPE, iVOL_SLIDER_MAX_SLOPE, TRUE );
	sliderVolSlope.SetPos(vol_slope_to_slider(pst->fVolSlope));
	GetDlgItem(IDC_VOL_SLOPE_TEXT)->SetWindowText(strFloatText("Slope = %.3f",pst->fVolSlope));

	sliderVolIntersect.SetRange( -iVOL_SLIDER_MAX_INTERSECT, iVOL_SLIDER_MAX_INTERSECT, TRUE );
	sliderVolIntersect.SetPos(vol_intersect_to_slider(pst->fVolInt));
	GetDlgItem(IDC_VOL_INTS_TEXT)->SetWindowText(strFloatText("Intersection = %.3f",pst->fVolInt));

	sliderVolMin.SetRange( (fVOL_MIN_Y_AXIS*iVOL_SLIDER_MAX_REGION), (fVOL_MAX_Y_AXIS*iVOL_SLIDER_MAX_REGION), TRUE );
	sliderVolMin.SetPos(vol_region_to_slider(pst->fVolMin));
	GetDlgItem(IDC_VOL_MIN_TEXT)->SetWindowText(strFloatText("Minimum Volume = %.3f",pst->fVolMin));

	sliderVolMax.SetRange( (fVOL_MIN_Y_AXIS*iVOL_SLIDER_MAX_REGION), (fVOL_MAX_Y_AXIS*iVOL_SLIDER_MAX_REGION), TRUE );
	sliderVolMax.SetPos(vol_region_to_slider(pst->fVolMax));
	GetDlgItem(IDC_VOL_MAX_TEXT)->SetWindowText(strFloatText("Maximum Volume = %.3f",pst->fVolMax));

	// Pitch function
	sliderPitSlope.SetRange( -iPIT_SLIDER_MAX_SLOPE, iPIT_SLIDER_MAX_SLOPE, TRUE );
	sliderPitSlope.SetPos(pitch_slope_to_slider(pst->fPitchSlope));
	GetDlgItem(IDC_PIT_SLOPE_TEXT)->SetWindowText(strFloatText("Slope = %.3f",pst->fPitchSlope));

	sliderPitInt.SetRange( -iPIT_SLIDER_MAX_INTERSECT, iPIT_SLIDER_MAX_INTERSECT, TRUE );
	sliderPitInt.SetPos(pitch_intersect_to_slider(pst->fPitchInt));
	GetDlgItem(IDC_PIT_INTS_TEXT)->SetWindowText(strFloatText("Intersection = %.3f",pst->fPitchInt));

	sliderPitMin.SetRange( (fPIT_MIN_Y_AXIS*iPIT_SLIDER_MAX_REGION), (fPIT_MAX_Y_AXIS*iPIT_SLIDER_MAX_REGION), TRUE );
	sliderPitMin.SetPos(pitch_region_to_slider(pst->fPitchMin));
	GetDlgItem(IDC_PIT_MIN_TEXT)->SetWindowText(strFloatText("Minimum Pitch = %.3f",pst->fPitchMin));
	
	sliderPitMax.SetRange( (fPIT_MIN_Y_AXIS*iPIT_SLIDER_MAX_REGION), (fPIT_MAX_Y_AXIS*iPIT_SLIDER_MAX_REGION), TRUE );
	sliderPitMax.SetPos(pitch_region_to_slider(pst->fPitchMax));
	GetDlgItem(IDC_PIT_MAX_TEXT)->SetWindowText(strFloatText("Maximum Pitch = %.3f",pst->fPitchMax));

	sliderAttenuate.SetRange(0,(int)fATTENUATE_SLIDER_MAX, TRUE );
	sliderAttenuate.SetPos(attenuate_to_slider(pst->fAttenuate));
	GetDlgItem(IDC_ATTENUATE_TEXT)->SetWindowText(strFloatText("Attenuation = -%.2f dB/m",pst->fAttenuate));
	InvalidateRect(NULL, FALSE);
}


//*********************************************************************************************
void CDialogSoundMaterial::OnPaint()
{
	CPaintDC			dc(this);
	WINDOWPLACEMENT		wndpl;

	if ((pst==NULL) || (bValid == false))
	{
			// draw the volume graph
		CBrush brush( RGB( 0, 0, 0 ) );
		dc.SelectObject(brush);

		GetDlgItem(IDC_GRAPH_VOL)->GetWindowPlacement(&wndpl);
		dc.Rectangle(&wndpl.rcNormalPosition);

		GetDlgItem(IDC_GRAPH_PIT)->GetWindowPlacement(&wndpl);
		dc.Rectangle(&wndpl.rcNormalPosition);

		GetDlgItem(IDC_GRAPH_WORLD)->GetWindowPlacement(&wndpl);
		dc.Rectangle(&wndpl.rcNormalPosition);
	}
	else
	{
		GetDlgItem(IDC_GRAPH_VOL)->GetWindowPlacement(&wndpl);
		DrawGraph(dc,wndpl.rcNormalPosition,
					fVOL_MIN_X_AXIS,fVOL_MAX_X_AXIS,fVOL_MIN_Y_AXIS,fVOL_MAX_Y_AXIS,
					pst->fVolSlope,pst->fVolInt,pst->fVolMin,pst->fVolMax,fTestVel);
		GetDlgItem(IDC_GRAPH_PIT)->GetWindowPlacement(&wndpl);
		DrawGraph(dc,wndpl.rcNormalPosition,
					fPIT_MIN_X_AXIS,fPIT_MAX_X_AXIS,fPIT_MIN_Y_AXIS,fPIT_MAX_Y_AXIS,
					pst->fPitchSlope,pst->fPitchInt,pst->fPitchMin,pst->fPitchMax,fTestVel);

		GetDlgItem(IDC_GRAPH_WORLD)->GetWindowPlacement(&wndpl);
		DrawWorld(dc,wndpl.rcNormalPosition);
	}
}



//*********************************************************************************************
void CDialogSoundMaterial::DrawWorld(CDC& cdc,RECT& rect)
{
#define fWORLD_SCALE		1000.0f
	// reset the mapping mode to normal GDI (pixel=logical)
	cdc.SetMapMode(MM_TEXT);
	cdc.SetViewportOrg( 0, 0 );

	CRgn	rgnWorld;
	rgnWorld.CreateRectRgnIndirect(&rect);
	cdc.SelectClipRgn(&rgnWorld,RGN_COPY);

	//
	// clear the back ground while still in MM_TEXT mode, the rectangle passed in 
	// is the position of our graph.
	//
	CBrush brush( RGB( 0, 0, 0 ) );
	cdc.SelectObject(brush);
	cdc.Rectangle(&rect);

	if (!bValid)
		return;

	//
	// use the rectange passed in to create a local co-ordinate system
	//
	cdc.SetViewportOrg( rect.left+((rect.right-rect.left)/2),rect.top+((rect.bottom-rect.top)/2) );
	cdc.SetMapMode(MM_ANISOTROPIC);
	cdc.SetViewportExt(rect.right-rect.left,-(rect.bottom-rect.top));
	cdc.SetWindowExt((fWorldScale)*fWORLD_SCALE*2,(fWorldScale)*fWORLD_SCALE*2);


	//
	// Our local co-ordinate system is the bottom left hand position of the rectangle passed in.
	// +ve x is to the left and +ve y is up, just like a normal graph.
	// the size of our device is specifed on the SetWindowExt line above
	//

	float f_atten = 5.0f / pst->fAttenuate;

	CBrush b4( RGB( 32, 32, 32 ) );
	RECT r4={(fXWorldPos-f_atten*8)*fWORLD_SCALE,(fYWorldPos-f_atten*8)*fWORLD_SCALE,(fXWorldPos+f_atten*8)*fWORLD_SCALE,(fYWorldPos+f_atten*8)*fWORLD_SCALE};
	cdc.SelectObject(b4);
	cdc.Ellipse(&r4);

	// draw our minimum distance
	CBrush b3( RGB( 64, 64, 64 ) );
	RECT r3={(fXWorldPos-f_atten*4)*fWORLD_SCALE,(fYWorldPos-f_atten*4)*fWORLD_SCALE,(fXWorldPos+f_atten*4)*fWORLD_SCALE,(fYWorldPos+f_atten*4)*fWORLD_SCALE};
	cdc.SelectObject(b3);
	cdc.Ellipse(&r3);

	CBrush b2( RGB( 128, 128, 128 ) );
	RECT r2={(fXWorldPos-f_atten*2)*fWORLD_SCALE,(fYWorldPos-f_atten*2)*fWORLD_SCALE,(fXWorldPos+f_atten*2)*fWORLD_SCALE,(fYWorldPos+f_atten*2)*fWORLD_SCALE};
	cdc.SelectObject(b2);
	cdc.Ellipse(&r2);

	CBrush b1( RGB( 192, 192, 192 ) );
	RECT r1={(fXWorldPos-f_atten)*fWORLD_SCALE,(fYWorldPos-f_atten)*fWORLD_SCALE,(fXWorldPos+f_atten)*fWORLD_SCALE,(fYWorldPos+f_atten)*fWORLD_SCALE};
	cdc.SelectObject(b1);
	cdc.Ellipse(&r1);


	CPen pen_axis(PS_SOLID,1,RGB(0xff,0,0) );
	cdc.SelectObject(&pen_axis);

	cdc.MoveTo(-fWorldScale*fWORLD_SCALE, 0);
	cdc.LineTo(+fWorldScale*fWORLD_SCALE, 0);
	cdc.MoveTo(0,-fWorldScale*fWORLD_SCALE);
	cdc.LineTo(0,+fWorldScale*fWORLD_SCALE);

	CPen pen_pos(PS_SOLID,1,RGB(0xff,0xff,0) );
	cdc.SelectObject(&pen_pos);

	cdc.MoveTo(fXWorldPos*fWORLD_SCALE, -fWorldScale*fWORLD_SCALE);
	cdc.LineTo(fXWorldPos*fWORLD_SCALE, fWorldScale*fWORLD_SCALE);
	cdc.MoveTo(-fWorldScale*fWORLD_SCALE, fYWorldPos*fWORLD_SCALE);
	cdc.LineTo(fWorldScale*fWORLD_SCALE, fYWorldPos*fWORLD_SCALE);

}


//*********************************************************************************************
void CDialogSoundMaterial::DrawGraph(CDC& cdc,
				RECT& rect,
				float f_xmin,
				float f_xmax,
				float f_ymin,
				float f_ymax,
				float f_grad,
				float f_ints,
				float f_fmin,
				float f_fmax,
				float f_vel)

{
	// reset the mapping mode to normal GDI (pixel=logical)
	cdc.SetMapMode(MM_TEXT);
	cdc.SetViewportOrg( 0, 0 );

	CRgn	rgnWorld;
	rgnWorld.CreateRectRgnIndirect(&rect);
	cdc.SelectClipRgn(&rgnWorld,RGN_COPY);

	//
	// clear the back ground while still in MM_TEXT mode, the rectangle passed in 
	// is the position of our graph.
	//
	CBrush brush( RGB( 0, 0, 0 ) );
	cdc.SelectObject(brush);
	cdc.Rectangle(&rect);

	if (!bValid)
		return;

	//
	// use the rectange passed in to create a local co-ordinate system
	//
	cdc.SetViewportOrg(rect.left,rect.bottom);
	cdc.SetMapMode(MM_ANISOTROPIC);
	cdc.SetViewportExt(rect.right-rect.left,-(rect.bottom-rect.top));
	cdc.SetWindowExt( 1000, 1000);

	//
	// Our local co-ordinate system is the bottom left hand position of the rectangle passed in.
	// +ve x is to the left and +ve y is up, just like a normal graph.
	// the size of our device is specifed on the SetWindowExt line above
	//


#define GPX(xp) (f_x+((xp)*f_scalex))
#define GPY(yp) (f_y+((yp)*f_scaley))


	float f_y,f_x,f_scalex,f_scaley;

	f_scalex=(950.0f-50.0f)/(f_xmax-f_xmin);
	f_x=50.0f-(f_xmin*f_scalex);
	f_scaley=(950.0f-50.0f)/(f_ymax-f_ymin);
	f_y=50.0f-(f_ymin*f_scaley);



	CPen pen_guide(PS_SOLID,1,RGB(0,0,0xff) );
	cdc.SelectObject(&pen_guide);

	cdc.MoveTo(GPX(f_xmin),GPY(f_fmin) );
	cdc.LineTo(GPX(f_xmax),GPY(f_fmin) );
	cdc.MoveTo(GPX(f_xmin),GPY(f_fmax) );
	cdc.LineTo(GPX(f_xmax),GPY(f_fmax) );

	CPen pen_axis(PS_SOLID,16,RGB(0xff,0,0) );
	cdc.SelectObject(&pen_axis);

	cdc.MoveTo( f_x, 950);
	cdc.LineTo( f_x, 50);
	cdc.MoveTo( 50, f_y);
	cdc.LineTo( 950,f_y);


	// select the graph pen
	CPen pen_graph(PS_SOLID,10,RGB(0,0xff,0) );
	cdc.SelectObject(&pen_graph);

	if ( (f_grad>-0.001) && (f_grad<0.001) )
	{
		// the graph is a straight horizontal line.......
		// make sure that its Y value is ot outside the specifed range
		float y;

		if (f_ints<f_fmin)
		{
			// intersection is less than lower region
			y=f_fmin;
		}
		else if (f_ints>f_fmax)
		{
			// intersection is greater than upper region
			y=f_fmax;
		}
		else
		{
			y=f_ints;
		}
		cdc.MoveTo(GPX(f_xmin),GPY(y) );
		cdc.LineTo(GPX(f_xmax),GPY(y) );
	}
	else
	{
		// first get the Y value of the xmin intersection
		float	f_y_xmin=(f_grad*f_xmin)+f_ints;
		float	f_x_xmin=f_xmin;
		bool	b_line=TRUE;				// the slope line
		bool	b_startline=FALSE;			// the clip startline
		bool	b_endline=FALSE;

		// if the Y value outside the region f_fmin -> f_fmax
		if (f_y_xmin>f_fmax)
		{
			// off the top
			f_y_xmin=f_fmax;
			f_x_xmin=(f_fmax-f_ints)/f_grad;
			b_startline=TRUE;
		}
		else if (f_y_xmin<f_fmin)
		{
			// off the bottom
			f_y_xmin=f_fmin;
			f_x_xmin=(f_fmin-f_ints)/f_grad;
			b_startline=TRUE;
		}

		// first get the Y value of the xmax intersection
		float	f_y_xmax=(f_grad*f_xmax)+f_ints;
		float	f_x_xmax=f_xmax;


		// if the Y value outside the region f_fmin -> f_fmax
		if (f_y_xmax>f_fmax)
		{
			// off the top
			f_y_xmax=f_fmax;
			f_x_xmax=(f_fmax-f_ints)/f_grad;
			b_endline=TRUE;
		}
		else if (f_y_xmax<f_fmin)
		{
			// off the bottom
			f_y_xmax=f_fmin;
			f_x_xmax=(f_fmin-f_ints)/f_grad;
			b_endline=TRUE;
		}

		if (f_x_xmin>f_xmax)
		{
			f_x_xmin=f_xmax;
			b_endline=FALSE;
			b_line=FALSE;
		}

		if (f_x_xmin<f_xmin)
		{
			f_x_xmin=f_xmin;
			b_startline=FALSE;
			b_line=FALSE;
		}


		if (f_x_xmax<f_xmin)
		{
			f_x_xmax=f_xmin;
			b_startline=FALSE;
			b_line=FALSE;
		}


		if (b_startline)
		{
			cdc.MoveTo(GPX(f_xmin),GPY(f_y_xmin) );
			cdc.LineTo(GPX(f_x_xmin),GPY(f_y_xmin) );
		}
		else
		{
			cdc.MoveTo(GPX(f_x_xmin),GPY(f_y_xmin) );
		}

		if (b_line)
		{
			cdc.LineTo(GPX(f_x_xmax),GPY(f_y_xmax) );
		}

		if (b_endline)
		{
			cdc.LineTo(GPX(f_xmax),GPY(f_y_xmax) );
		}


	}

	// draw the reference velocity
	CPen pen_vel(PS_SOLID,1,RGB(0xff,0xff,0) );
	cdc.SelectObject(&pen_vel);

	cdc.MoveTo(GPX(f_vel),GPY(f_ymin) );
	cdc.LineTo(GPX(f_vel),GPY(f_ymax) );


	// select some other pen/brush before we delete ours!!!
	cdc.SelectStockObject(WHITE_BRUSH);
	cdc.SelectStockObject(BLACK_PEN);

	// our pens and brushes get freed when the function goes out
	// of scope, ie here....
}


//*********************************************************************************************
void CDialogSoundMaterial::SetWorldPos()
{
	if (fXWorldPos>=fWorldScale)
		fXWorldPos=fWorldScale;
	if (fXWorldPos<=-fWorldScale)
		fXWorldPos=-fWorldScale;

	if (fYWorldPos>=fWorldScale)
		fYWorldPos=fWorldScale;
	if (fYWorldPos<=-fWorldScale)
		fYWorldPos=-fWorldScale;

	sliderWorldX.SetRange((int)-fWorldScale*fWORLD_UNITS,(int)fWorldScale*fWORLD_UNITS, TRUE );
	sliderWorldX.SetPos(worldpos_to_slider(fXWorldPos));
	sliderWorldY.SetRange((int)-fWorldScale*fWORLD_UNITS,(int)fWorldScale*fWORLD_UNITS, TRUE );
	sliderWorldY.SetPos(-worldpos_to_slider(fYWorldPos));
	WorldPosText();
}


//*********************************************************************************************
void CDialogSoundMaterial::WorldPosText()
{
	char buf[128];

	sprintf(buf,"(%.2f,%.2f)",fXWorldPos,fYWorldPos);
	strWorldPos = buf;
	UpdateData(FALSE);
}


//*********************************************************************************************
void CDialogSoundMaterial::OnRadio()
{
	UpdateData(true);
	UpdateSelectedGraph();
}


//*********************************************************************************************
void CDialogSoundMaterial::PositionSamples()
{
	if (psamAudio)
	{
		psamAudio->SetPosition(fXWorldPos,0.0f,fYWorldPos,false);
	}

	CAudio::pcaAudio->CommitSettings();
}


//*********************************************************************************************
void CDialogSoundMaterial::AdjustSamples()
{
	if (pst == NULL)
		return;

	float	f_pitch	= pst->fCalculatePitch(fTestVel);

	// scale the pitch to a multiplying factor that can be applied to the frequency
	if (f_pitch < 0.0f)
	{
		f_pitch = 1.0f + (f_pitch * 0.75f);
	}
	else
	{
		f_pitch = (f_pitch * 3.0f) + 1.0f;
	}

	if (psamAudio)
	{
		psamAudio->SetFrequency(f_pitch);
		psamAudio->SetVolume( -50.0f + (pst->fCalculateVolume(fTestVel)*50.0f) );
		psamAudio->SetAttenuation(pst->fAttenuate);
	}

	CAudio::pcaAudio->CommitSettings();
}


//*********************************************************************************************
//
void CDialogSoundMaterial::OnCenterSample()
{
	CClientDC			dc(this);
	WINDOWPLACEMENT		wndpl;

	fXWorldPos=0.0f;
	fYWorldPos=0.0f;

	GetDlgItem(IDC_GRAPH_WORLD)->GetWindowPlacement(&wndpl);
	DrawWorld(dc,wndpl.rcNormalPosition);
	SetWorldPos();
	PositionSamples();
}


//*********************************************************************************************
void CDialogSoundMaterial::OnSample1Change()
{
	if (!bCollision)
		return;

	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();
	(*pch)[u8Collision]->sndhndSamples[0] = cmbSample1.GetItemData(cmbSample1.GetCurSel());
}

//*********************************************************************************************
void CDialogSoundMaterial::OnSample2Change()
{
	if (!bCollision)
		return;

	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();
	(*pch)[u8Collision]->sndhndSamples[0] = cmbSample1.GetItemData(cmbSample1.GetCurSel());
}


//*********************************************************************************************
void CDialogSoundMaterial::OnSample3Change()
{
	if (!bSlide)
		return;


	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();
	(*pch)[u8Collision]->sndhndSamples[0] = cmbSample1.GetItemData(cmbSample1.GetCurSel());
}


//*********************************************************************************************
void CDialogSoundMaterial::OnEnableCollision() 
{
	bool b_enable = ((CButton*)GetDlgItem(IDC_COLLISION_ENABLE))->GetCheck();
	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();

	if (b_enable)
	{
		(*pch)[u8Collision]->sndhndSamples[0] = sndhndHashIdentifier("MISSING");
		(*pch)[u8Collision]->sndhndSamples[1] = sndhndHashIdentifier("MISSING");

		(*pch)[u8Collision]->u4SampleFlags &= 0xfffffffc;
		(*pch)[u8Collision]->u4SampleFlags |= 1;

		(*pch)[u8Collision]->stTransfer[0].fVolMax = 1.0f;
		(*pch)[u8Collision]->stTransfer[0].fVolMin = 0.0f;
		(*pch)[u8Collision]->stTransfer[0].fVolSlope = 0.0f;
		(*pch)[u8Collision]->stTransfer[0].fVolInt = 1.0f;
		(*pch)[u8Collision]->stTransfer[0].fPitchMax = 1.0f;
		(*pch)[u8Collision]->stTransfer[0].fPitchMin = -1.0f;
		(*pch)[u8Collision]->stTransfer[0].fPitchSlope = 0.0f;
		(*pch)[u8Collision]->stTransfer[0].fPitchInt = 0.0f;
		(*pch)[u8Collision]->stTransfer[0].fAttenuate = 1.0f;


		(*pch)[u8Collision]->stTransfer[1].fVolMax = 1.0f;
		(*pch)[u8Collision]->stTransfer[1].fVolMin = 0.0f;
		(*pch)[u8Collision]->stTransfer[1].fVolSlope = 0.0f;
		(*pch)[u8Collision]->stTransfer[1].fVolInt = 1.0f;
		(*pch)[u8Collision]->stTransfer[1].fPitchMax = 1.0f;
		(*pch)[u8Collision]->stTransfer[1].fPitchMin = -1.0f;
		(*pch)[u8Collision]->stTransfer[1].fPitchSlope = 0.0f;
		(*pch)[u8Collision]->stTransfer[1].fPitchInt = 0.0f;
		(*pch)[u8Collision]->stTransfer[1].fAttenuate = 1.0f;

		// we are enabling this collision
		Enable(b_enable, bSlide, false);
	}
	else	
	{
		if (MessageBox("Are you sure you want to remove this collision? All collision information for this material pair will be lost.", "Trespasser Effects Editor", MB_YESNO|MB_ICONQUESTION) == IDNO)
		{
			((CButton*)GetDlgItem(IDC_COLLISION_ENABLE))->SetCheck(true);
			return;
		}

		// we are disabling this collision
		Enable(b_enable, bSlide, false);

		(*pch)[u8Collision]->u4SampleFlags &= 0xfffffffc;
	}
}


//*********************************************************************************************
void CDialogSoundMaterial::OnEnableSlide() 
{
	bool b_enable = ((CButton*)GetDlgItem(IDC_SLIDE_ENABLE))->GetCheck();
	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();

	if (b_enable)
	{
		(*pch)[u8Collision]->sndhndSamples[2] = sndhndHashIdentifier("MISSING");
		(*pch)[u8Collision]->u4SampleFlags |= 0x80;


		(*pch)[u8Collision]->stTransfer[2].fVolMax = 1.0f;
		(*pch)[u8Collision]->stTransfer[2].fVolMin = 0.0f;
		(*pch)[u8Collision]->stTransfer[2].fVolSlope = 0.0f;
		(*pch)[u8Collision]->stTransfer[2].fVolInt = 1.0f;
		(*pch)[u8Collision]->stTransfer[2].fPitchMax = 1.0f;
		(*pch)[u8Collision]->stTransfer[2].fPitchMin = -1.0f;
		(*pch)[u8Collision]->stTransfer[2].fPitchSlope = 0.0f;
		(*pch)[u8Collision]->stTransfer[2].fPitchInt = 0.0f;
		(*pch)[u8Collision]->stTransfer[2].fAttenuate = 1.0f;

		// we are enabling slide for this collision
		Enable(bCollision, b_enable, bDual);
	}
	else	
	{
		if (MessageBox("Are you sure you want to remove the sliding information for this collision? All slide information for this material pair will be lost.", "Trespasser Effects Editor", MB_YESNO|MB_ICONQUESTION) == IDNO)
		{
			((CButton*)GetDlgItem(IDC_SLIDE_ENABLE))->SetCheck(true);
			return;
		}

		// we are disabling the slide information
		Enable(bCollision, b_enable, bDual);
		(*pch)[u8Collision]->u4SampleFlags &= ~0x80;
	}
}


//*********************************************************************************************
void CDialogSoundMaterial::OnClose() 
{
	OnStopSample();
	CDialog::OnClose();
}

//*********************************************************************************************
void CDialogSoundMaterial::OnOK() 
{
	OnStopSample();
	CDialog::OnOK();
}


//*********************************************************************************************
void CDialogSoundMaterial::OnStopSample()
{
	delete psamCol1;
	delete psamCol2;
	delete psamAudio;
	delete psamTest;

	psamCol1 = NULL;
	psamCol2 = NULL;
	psamAudio = NULL;
	psamTest = NULL;
}


//*********************************************************************************************
//
void CDialogSoundMaterial::TestPlay(TSoundHandle sndhnd, bool b_loop)
{
	if (psamTest)
		delete psamTest;

	psamTest = CAudio::psamCreateSample(sndhnd,padAudioDaemon->padatEffects,AU_CREATE_STEREO | AU_CREATE_STATIC);
	if (psamTest == NULL)
	{
		::MessageBox(NULL,"Cannot load the selected sample.","Trespasser Effects Editor", MB_OK);
		return;
	}

	psamTest->bPlay(b_loop?AU_PLAY_LOOPED:0);
}


//*********************************************************************************************
void CDialogSoundMaterial::OnTestS1() 
{	
	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();
	TestPlay(cmbSample1.GetItemData(cmbSample1.GetCurSel()), false);
}


//*********************************************************************************************
void CDialogSoundMaterial::OnTestS2() 
{
	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();
	TestPlay(cmbSample2.GetItemData(cmbSample2.GetCurSel()), false);
}


//*********************************************************************************************
void CDialogSoundMaterial::OnTestS3() 
{
	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();
	TestPlay(cmbSample3.GetItemData(cmbSample3.GetCurSel()), true);
}


//*********************************************************************************************
void CDialogSoundMaterial::OnDualClick() 
{
	bool b_enable = ((CButton*)GetDlgItem(IDC_DUAL_CHECK))->GetCheck();
	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();

	if (b_enable)
	{
		(*pch)[u8Collision]->sndhndSamples[1] = sndhndHashIdentifier("MISSING");

		(*pch)[u8Collision]->u4SampleFlags &= 0xfffffffc;
		(*pch)[u8Collision]->u4SampleFlags |= 2;

		(*pch)[u8Collision]->stTransfer[1].fVolMax = 1.0f;
		(*pch)[u8Collision]->stTransfer[1].fVolMin = 0.0f;
		(*pch)[u8Collision]->stTransfer[1].fVolSlope = 0.0f;
		(*pch)[u8Collision]->stTransfer[1].fVolInt = 1.0f;
		(*pch)[u8Collision]->stTransfer[1].fPitchMax = 1.0f;
		(*pch)[u8Collision]->stTransfer[1].fPitchMin = -1.0f;
		(*pch)[u8Collision]->stTransfer[1].fPitchSlope = 0.0f;
		(*pch)[u8Collision]->stTransfer[1].fPitchInt = 0.0f;
		(*pch)[u8Collision]->stTransfer[1].fAttenuate = 1.0f;

		Enable(bCollision, bSlide, true);
	}
	else
	{
		if (MessageBox("Are you sure you want to remove the second sample?", "Trespasses Effects Editor", MB_YESNO|MB_ICONQUESTION) == IDNO)
		{
			((CButton*)GetDlgItem(IDC_DUAL_CHECK))->SetCheck(true);
			return;
		}
		(*pch)[u8Collision]->u4SampleFlags &= 0xfffffffc;
		(*pch)[u8Collision]->u4SampleFlags |= 1;

		Enable(bCollision, bSlide, false);
	}
}


//*********************************************************************************************
//
void CDialogSoundMaterial::OnTestSingle()
{
	CComboBox*	pcmb;
	bool		b_loop;
	UpdateData(true);

	if (pst == NULL)
		return;

	switch (i4SampleSelect)
	{
	case 0:
		pcmb = &cmbSample1;
		b_loop = false;
		break;
	case 1:
		pcmb = &cmbSample2;
		b_loop = false;
		break;
	case 2:
		pcmb = &cmbSample3;
		b_loop = true;
		break;
	default:
		Assert(0);
	}
	
	if (psamTest)
	{
		delete psamTest;
		psamTest = NULL;
	}

	if (psamAudio)
		delete psamAudio;

	//
	// Create a 3D sample
	//
	psamAudio = CAudio::psamCreateSample(pcmb->GetItemData(pcmb->GetCurSel()), padAudioDaemon->padatEffects, 
						AU_CREATE_SPATIAL_3D | AU_CREATE_STATIC | AU_CREATE_CTRL_FREQUENCY | AU_CREATE_CTRL_VOLUME);
	if (psamAudio == NULL)
	{
		::MessageBox(NULL,"Cannot load the selected sample.","Trespasser Effects Editor", MB_OK);
		return;
	}

	CAudio::pcaAudio->PositionListener(0,0,0);
	CAudio::pcaAudio->OrientListener(0.0,0.0,1.0, 0.0,1.0,0.0);
	CAudio::pcaAudio->CommitSettings();

	psamAudio->bPlay(*pst,fTestVel,fXWorldPos,0.0f,fYWorldPos,b_loop?AU_PLAY_LOOPED:0);
}


//*********************************************************************************************
void CDialogSoundMaterial::OnTestDual() 
{

	TCollisionHash*		pch = padAudioDaemon->padatEffects->pchGetCollisions();
	uint32 u4_id1			= cmbSample1.GetItemData(cmbSample1.GetCurSel());
	uint32 u4_id2			= cmbSample2.GetItemData(cmbSample2.GetCurSel());

	if (psamTest)
	{
		delete psamTest;
		psamTest = NULL;
	}

	if (psamAudio)
	{
		delete psamAudio;
		psamAudio = NULL;
	}


	// delete the existing samples..
	delete psamCol1;
	delete psamCol2;

	psamCol1 = NULL;
	psamCol2 = NULL;

	psamCol1 = CAudio::psamCreateSample(u4_id1,padAudioDaemon->padatEffects,AU_CREATE_SPATIAL_3D | AU_CREATE_STATIC | 
						AU_CREATE_CTRL_FREQUENCY | AU_CREATE_CTRL_VOLUME);
	if (psamCol1 == NULL)
	{
		MessageBox("Cannot load sample 1.","Trespasser Effects Editor", MB_OK|MB_ICONINFORMATION);
		return;
	}

	psamCol2 = CAudio::psamCreateSample(u4_id2,padAudioDaemon->padatEffects,AU_CREATE_SPATIAL_3D | AU_CREATE_STATIC |
					AU_CREATE_CTRL_FREQUENCY | AU_CREATE_CTRL_VOLUME);
	if (psamCol2 == NULL)
	{
		MessageBox("Cannot load sample 2.","Trespasser Effects Editor", MB_OK|MB_ICONINFORMATION);
		return;
	}

	CAudio::pcaAudio->PositionListener(0,0,0);
	CAudio::pcaAudio->OrientListener(0.0,0.0,1.0, 0.0,1.0,0.0);
	CAudio::pcaAudio->CommitSettings();

	psamCol1->bPlay((*pch)[u8Collision]->stTransfer[0],fTestVel,fXWorldPos,0.0f,fYWorldPos);

	psamCol2->bPlay((*pch)[u8Collision]->stTransfer[1],fTestVel,fXWorldPos,0.0f,fYWorldPos);
}



//*********************************************************************************************
static uint64 u8CollisionHash(uint32 u4_mat1, uint32 u4_mat2)
{
	if (u4_mat1<u4_mat2)
	{
		return ((uint64)u4_mat1) | (((uint64)u4_mat2)<<32);
	}
	else
	{
		return ((uint64)u4_mat2) | (((uint64)u4_mat1)<<32);
	}	
}


//*********************************************************************************************
const char* strFloatText(char* text,float val)
{
	static char buf[128];

	sprintf(buf,text,val);

	return(buf);
}
