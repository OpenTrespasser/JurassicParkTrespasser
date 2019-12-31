// EditMaterialDlg.cpp : implementation file
//
#include "stdafx.h"
#include "Common.hpp"
#include "CollisionEditor.h"
#include "database.h"
#include "EditMaterialDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CEditMaterialDlg dialog


CSample*	CEditMaterialDlg::psamCol1 = NULL;
CSample*	CEditMaterialDlg::psamCol2 = NULL;
CSample*	CEditMaterialDlg::psamAudio = NULL;
CSample*	CEditMaterialDlg::psamTest = NULL;


CEditMaterialDlg::CEditMaterialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditMaterialDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditMaterialDlg)
	strWorldPos		= _T("");
	fTestVel		= 0.5f;
	fXWorldPos		= 0.0f;
	fYWorldPos		= 0.0f;
	fWorldScale		= 10.0f;
	u4Material		= 0;
	u8Collision		= 0;
	bValid			= false;
	bCollision		= false;
	bSlide			= false;
	bDual			= false;
	i4SampleSelect	= 0;
	fDelayTime		= 0.0f;
	//}}AFX_DATA_INIT
}


CEditMaterialDlg::~CEditMaterialDlg()
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


void CEditMaterialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditMaterialDlg)

	DDX_Control(pDX, IDC_MSET_LIST,				msetList);
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
	DDX_Control(pDX, IDC_SM_SLIDER15,			sliderMinVel);
	DDX_Control(pDX, IDC_COMBO1,				cmbSample1);
	DDX_Control(pDX, IDC_COMBO2,				cmbSample2);
	DDX_Control(pDX, IDC_COMBO3,				cmbSample3);
	DDX_Radio(pDX,	 IDC_SM_RADIO1,				i4SampleSelect);
	DDX_Text(pDX,	 IDC_DELAY_TIME,			fDelayTime);
	DDX_Text(pDX,	IDC_WORLD_POS_TEXT,			strWorldPos);

	DDV_MinMaxFloat(pDX, fDelayTime, 0.0, 60.0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditMaterialDlg, CDialog)
	//{{AFX_MSG_MAP(CEditMaterialDlg)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()

	ON_BN_CLICKED(IDC_REFWF_TEST_S1,	OnTestS1)
	ON_BN_CLICKED(IDC_REFWF_TEST_S2,	OnTestS2)
	ON_BN_CLICKED(IDC_REFWF_TEST_S3,	OnTestS3)
	
	ON_BN_CLICKED(IDC_DUAL_CHECK,		OnDualClick)
	ON_BN_CLICKED(IDC_STOP_COLLIDE,		OnStopSample)
	ON_BN_CLICKED(IDC_TEST_SAMPLE,		OnTestSample)
	ON_BN_CLICKED(IDC_COLLIDE,			OnCollide)
	ON_BN_CLICKED(IDC_EM_CENTER,		OnCenterSample)

	ON_BN_CLICKED(IDC_COLLISION_ENABLE,	OnEnableCollision)
	ON_BN_CLICKED(IDC_SLIDE_ENABLE,		OnEnableSlide)

	ON_BN_CLICKED(IDC_SM_RADIO1,		OnRadio)
	ON_BN_CLICKED(IDC_SM_RADIO2,		OnRadio)
	ON_BN_CLICKED(IDC_SM_RADIO4,		OnRadio)

	ON_LBN_SELCHANGE(IDC_MSET_LIST,		OnSelChangeList)
	ON_CBN_SELCHANGE(IDC_COMBO1,		OnSample1Change)
	ON_CBN_SELCHANGE(IDC_COMBO2,		OnSample2Change)
	ON_CBN_SELCHANGE(IDC_COMBO3,		OnSample3Change)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditMaterialDlg message handlers


// this will draw a graoh in local co-ordinates and scale it to fit within the
// rectangle bassed in...
void CEditMaterialDlg::DrawWorld(CDC& cdc,
				RECT& rect)
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

	float f_atten = 5.0f / pcsm->stTransfer.fAttenuate;

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



// this will draw a graoh in local co-ordinates and scale it to fit within the
// rectangle bassed in...
void CEditMaterialDlg::DrawGraph(CDC& cdc,
				RECT& rect,
				float f_xmin,
				float f_xmax,
				float f_ymin,
				float f_ymax,
				float f_grad,
				float f_ints,
				float f_fmin,
				float f_fmax,
				float f_vel,
				float f_minvel)

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



	RECT rc_vel;
	rc_vel.left		= GPX(0.0f);
	rc_vel.top		= GPY(f_ymax);
	rc_vel.right	= GPX(f_minvel);
	rc_vel.bottom	= GPY(f_ymin);

	CBrush brushvel( RGB( 64, 64, 64 ) );
	cdc.SelectObject(brushvel);
	cdc.Rectangle(&rc_vel);



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



const char* strFloatText(char* text,float val)
{
	static char buf[128];

	sprintf(buf,text,val);

	return(buf);
}



void CEditMaterialDlg::OnPaint()
{
	CPaintDC			dc(this);
	WINDOWPLACEMENT		wndpl;

	if ((pcsm==NULL) || (bValid == false))
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
					pcsm->stTransfer.fVolSlope,pcsm->stTransfer.fVolInt,pcsm->stTransfer.fVolMin,pcsm->stTransfer.fVolMax,fTestVel,pcsm->stTransfer.fMinVelocity);
		GetDlgItem(IDC_GRAPH_PIT)->GetWindowPlacement(&wndpl);
		DrawGraph(dc,wndpl.rcNormalPosition,
					fPIT_MIN_X_AXIS,fPIT_MAX_X_AXIS,fPIT_MIN_Y_AXIS,fPIT_MAX_Y_AXIS,
					pcsm->stTransfer.fPitchSlope,pcsm->stTransfer.fPitchInt,pcsm->stTransfer.fPitchMin,pcsm->stTransfer.fPitchMax,fTestVel,pcsm->stTransfer.fMinVelocity);

		GetDlgItem(IDC_GRAPH_WORLD)->GetWindowPlacement(&wndpl);
		DrawWorld(dc,wndpl.rcNormalPosition);
	}
}


void CEditMaterialDlg::SetWorldPos()
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


void CEditMaterialDlg::WorldPosText()
{
	char buf[128];

	sprintf(buf,"(%.2f,%.2f)",fXWorldPos,fYWorldPos);
	strWorldPos = buf;
	UpdateData(FALSE);
}


void CEditMaterialDlg::SetGraphs()
{
	if (pcsm == NULL)
		return;

	if (!bValid)
		return;

	// Volume function
	sliderVolSlope.SetRange( -iVOL_SLIDER_MAX_SLOPE, iVOL_SLIDER_MAX_SLOPE, TRUE );
	sliderVolSlope.SetPos(vol_slope_to_slider(pcsm->stTransfer.fVolSlope));
	GetDlgItem(IDC_VOL_SLOPE_TEXT)->SetWindowText(strFloatText("Slope = %.3f",pcsm->stTransfer.fVolSlope));

	sliderVolIntersect.SetRange( -iVOL_SLIDER_MAX_INTERSECT, iVOL_SLIDER_MAX_INTERSECT, TRUE );
	sliderVolIntersect.SetPos(vol_intersect_to_slider(pcsm->stTransfer.fVolInt));
	GetDlgItem(IDC_VOL_INTS_TEXT)->SetWindowText(strFloatText("Intersection = %.3f",pcsm->stTransfer.fVolInt));

	sliderVolMin.SetRange( (fVOL_MIN_Y_AXIS*iVOL_SLIDER_MAX_REGION), (fVOL_MAX_Y_AXIS*iVOL_SLIDER_MAX_REGION), TRUE );
	sliderVolMin.SetPos(vol_region_to_slider(pcsm->stTransfer.fVolMin));
	GetDlgItem(IDC_VOL_MIN_TEXT)->SetWindowText(strFloatText("Minimum Volume = %.3f",pcsm->stTransfer.fVolMin));

	sliderVolMax.SetRange( (fVOL_MIN_Y_AXIS*iVOL_SLIDER_MAX_REGION), (fVOL_MAX_Y_AXIS*iVOL_SLIDER_MAX_REGION), TRUE );
	sliderVolMax.SetPos(vol_region_to_slider(pcsm->stTransfer.fVolMax));
	GetDlgItem(IDC_VOL_MAX_TEXT)->SetWindowText(strFloatText("Maximum Volume = %.3f",pcsm->stTransfer.fVolMax));

	// Pitch function
	sliderPitSlope.SetRange( -iPIT_SLIDER_MAX_SLOPE, iPIT_SLIDER_MAX_SLOPE, TRUE );
	sliderPitSlope.SetPos(pitch_slope_to_slider(pcsm->stTransfer.fPitchSlope));
	GetDlgItem(IDC_PIT_SLOPE_TEXT)->SetWindowText(strFloatText("Slope = %.3f",pcsm->stTransfer.fPitchSlope));

	sliderPitInt.SetRange( -iPIT_SLIDER_MAX_INTERSECT, iPIT_SLIDER_MAX_INTERSECT, TRUE );
	sliderPitInt.SetPos(pitch_intersect_to_slider(pcsm->stTransfer.fPitchInt));
	GetDlgItem(IDC_PIT_INTS_TEXT)->SetWindowText(strFloatText("Intersection = %.3f",pcsm->stTransfer.fPitchInt));

	sliderPitMin.SetRange( (fPIT_MIN_Y_AXIS*iPIT_SLIDER_MAX_REGION), (fPIT_MAX_Y_AXIS*iPIT_SLIDER_MAX_REGION), TRUE );
	sliderPitMin.SetPos(pitch_region_to_slider(pcsm->stTransfer.fPitchMin));
	GetDlgItem(IDC_PIT_MIN_TEXT)->SetWindowText(strFloatText("Minimum Pitch = %.3f",pcsm->stTransfer.fPitchMin));
	
	sliderPitMax.SetRange( (fPIT_MIN_Y_AXIS*iPIT_SLIDER_MAX_REGION), (fPIT_MAX_Y_AXIS*iPIT_SLIDER_MAX_REGION), TRUE );
	sliderPitMax.SetPos(pitch_region_to_slider(pcsm->stTransfer.fPitchMax));
	GetDlgItem(IDC_PIT_MAX_TEXT)->SetWindowText(strFloatText("Maximum Pitch = %.3f",pcsm->stTransfer.fPitchMax));

	sliderAttenuate.SetRange(0,(int)fATTENUATE_SLIDER_MAX, TRUE );
	sliderAttenuate.SetPos(attenuate_to_slider(pcsm->stTransfer.fAttenuate));
	GetDlgItem(IDC_ATTENUATE_TEXT)->SetWindowText(strFloatText("Attenuation = -%.2f dB/m",pcsm->stTransfer.fAttenuate));

	sliderMinVel.SetRange(0,(int)fVEL_SLIDER_UNITS, TRUE );
	sliderMinVel.SetPos(pcsm->stTransfer.fMinVelocity*fVEL_SLIDER_UNITS);
	GetDlgItem(IDC_MIN_VEL_TEXT)->SetWindowText(strFloatText("Minimum Collision = %.2f",pcsm->stTransfer.fMinVelocity));

	InvalidateRect(NULL, FALSE);
}


//**********************************************************************************************
void CEditMaterialDlg::AddIdentifiersToCombo(CComboBox& cmb)
{
	for (TEffectVector::iterator i = edbEffects.evEffects.begin(); i<edbEffects.evEffects.end(); ++i)
	{
		int idx = cmb.AddString( (*i).cstrID );
		cmb.SetItemData(idx, edbEffects.u4Hash( (*i).cstrID ) );
	}

	cmb.SetCurSel(0);
}


//**********************************************************************************************
void CEditMaterialDlg::AddMaterialsToList(CListBox& lb, uint32 u4_data)
{
	uint32 u4_set;

	// Add all the identifiers before we select one.
	// The list is sorted so we cannot be sure that the item we find will stay in place after 
	// new elements have been added
	for (TMaterialVector::iterator i = edbEffects.mvMaterials.begin(); i<edbEffects.mvMaterials.end(); ++i)
	{
		int idx = lb.AddString( (*i).cstrID );
		lb.SetItemData(idx, (*i).u4Hash);
	}

	for (int x = 0; x<lb.GetCount(); x++)
	{
		if (u4_data == lb.GetItemData(x))
		{
			u4_set = x;
		}
	}

	lb.SetCurSel(u4_set);
}


//**********************************************************************************************
void CEditMaterialDlg::SetComboBoxItem(CComboBox& cmb,uint32 u4_data)
{
	uint32 u4_count = (uint32)cmb.GetCount();
	uint32 u4_set   = 0;

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
void CEditMaterialDlg::SetupCollision(uint32 u4_mat1, uint32 u4_mat2)
{
	u8Collision = edbEffects.u8CollisionHash(u4_mat1, u4_mat2);

	if (edbEffects.bFindCollision(u8Collision))
	{
		bSlide		= edbEffects.cmCollisions[u8Collision].bSlide();
		bCollision	= edbEffects.cmCollisions[u8Collision].bCollision();
		bDual		= (edbEffects.cmCollisions[u8Collision].u4Samples() == 2);
		Enable(bCollision, bSlide, bDual);
	}
	else
	{
		Enable(false,false,false);
	}

	SMaterialListElement* pmle_mat1 = edbEffects.pmleFindMaterialID(u4_mat1);
	SMaterialListElement* pmle_mat2 = edbEffects.pmleFindMaterialID(u4_mat2);

	// set the window title
	CString	str_title("Edit Collision: ["); 
	str_title += pmle_mat1->cstrID;
	str_title += "/";
	str_title += pmle_mat2->cstrID;
	str_title += "]";
	
	SetWindowText(str_title);
}



//**********************************************************************************************
BOOL CEditMaterialDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	AddIdentifiersToCombo(cmbSample1);
	AddIdentifiersToCombo(cmbSample2);
	AddIdentifiersToCombo(cmbSample3);

	AddMaterialsToList(msetList, u4Material);

	SetupCollision(u4Material, u4Material);

	fTestVel = 0.5f;
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

	UpdateData(FALSE);
	return TRUE;
}


//*********************************************************************************************
//
void CEditMaterialDlg::OnSelChangeList()
{
	UpdateMinTime();

	int idx = msetList.GetCurSel();

	uint32 u4_hash = (uint32)msetList.GetItemData(idx);

	SetupCollision(u4Material, u4_hash);
}




//*********************************************************************************************
//
void CEditMaterialDlg::OnOK() 
{
	UpdateMinTime();
	CDialog::OnOK();
}



//*********************************************************************************************
//
void CEditMaterialDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
//
void CEditMaterialDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
		pcsm->stTransfer.fVolSlope = slider_to_vol_slope(nPos);
		GetDlgItem(IDC_VOL_SLOPE_TEXT)->SetWindowText(strFloatText("Slope = %.3f",pcsm->stTransfer.fVolSlope));
		b_drawvol=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER2:
		pcsm->stTransfer.fVolInt = slider_to_vol_intersect(nPos);
		GetDlgItem(IDC_VOL_INTS_TEXT)->SetWindowText(strFloatText("Intersection = %.3f",pcsm->stTransfer.fVolInt));
		b_drawvol=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER3:
		pcsm->stTransfer.fVolMin = slider_to_vol_region(nPos);
		if (pcsm->stTransfer.fVolMin>pcsm->stTransfer.fVolMax-0.005)
		{
			pcsm->stTransfer.fVolMin=pcsm->stTransfer.fVolMax-0.005;
			sliderVolMin.SetPos(vol_region_to_slider(pcsm->stTransfer.fVolMin));
		}
		GetDlgItem(IDC_VOL_MIN_TEXT)->SetWindowText(strFloatText("Minimum Volume = %.3f",pcsm->stTransfer.fVolMin));
		b_drawvol=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER4:
		pcsm->stTransfer.fVolMax = slider_to_vol_region(nPos);
		if (pcsm->stTransfer.fVolMax<pcsm->stTransfer.fVolMin+0.005)
		{
			pcsm->stTransfer.fVolMax=pcsm->stTransfer.fVolMin+0.005;
			sliderVolMax.SetPos(vol_region_to_slider(pcsm->stTransfer.fVolMax));
		}
		GetDlgItem(IDC_VOL_MAX_TEXT)->SetWindowText(strFloatText("Maximum Volume = %.3f",pcsm->stTransfer.fVolMax));
		b_drawvol=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER5:
		pcsm->stTransfer.fPitchSlope = slider_to_pitch_slope(nPos);
		GetDlgItem(IDC_PIT_SLOPE_TEXT)->SetWindowText(strFloatText("Slope = %.3f",pcsm->stTransfer.fPitchSlope));
		b_drawpit=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER6:
		pcsm->stTransfer.fPitchInt = slider_to_pitch_intersect(nPos);
		GetDlgItem(IDC_PIT_INTS_TEXT)->SetWindowText(strFloatText("Intersection = %.3f",pcsm->stTransfer.fPitchInt));
		b_drawpit=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER7:
		pcsm->stTransfer.fPitchMin = slider_to_pitch_region(nPos);
		if (pcsm->stTransfer.fPitchMin>pcsm->stTransfer.fPitchMax-0.005)
		{
			pcsm->stTransfer.fPitchMin=pcsm->stTransfer.fPitchMax-0.005;
			sliderPitMin.SetPos(vol_region_to_slider(pcsm->stTransfer.fPitchMin));
		}
		GetDlgItem(IDC_PIT_MIN_TEXT)->SetWindowText(strFloatText("Minimum Pitch = %.3f",pcsm->stTransfer.fPitchMin));
		b_drawpit=TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER8:
		pcsm->stTransfer.fPitchMax = slider_to_pitch_region(nPos);
		if (pcsm->stTransfer.fPitchMax<pcsm->stTransfer.fPitchMin+0.005)
		{
			pcsm->stTransfer.fPitchMax=pcsm->stTransfer.fPitchMin+0.005;
			sliderPitMax.SetPos(pitch_region_to_slider(pcsm->stTransfer.fPitchMax));
		}
		GetDlgItem(IDC_PIT_MAX_TEXT)->SetWindowText(strFloatText("Maximum Pitch = %.3f",pcsm->stTransfer.fPitchMax));
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
		pcsm->stTransfer.fAttenuate = slider_to_attenuate(nPos);
		GetDlgItem(IDC_ATTENUATE_TEXT)->SetWindowText(strFloatText("Attenuation = -%.2f dB/m",pcsm->stTransfer.fAttenuate));
		b_drawworld = TRUE;
		AdjustSamples();
		break;

	case IDC_SM_SLIDER15:
		pcsm->stTransfer.fMinVelocity = (float)nPos/fVEL_SLIDER_UNITS;
		GetDlgItem(IDC_MIN_VEL_TEXT)->SetWindowText(strFloatText("Minimum Collision = %.2f",pcsm->stTransfer.fMinVelocity));
		b_drawvol = TRUE;
		b_drawpit = TRUE;
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
					pcsm->stTransfer.fVolSlope,pcsm->stTransfer.fVolInt,pcsm->stTransfer.fVolMin,pcsm->stTransfer.fVolMax,fTestVel,pcsm->stTransfer.fMinVelocity);
	}


	if (b_drawpit)
	{
		// draw the pitch graph
		GetDlgItem(IDC_GRAPH_PIT)->GetWindowPlacement(&wndpl);
		DrawGraph(dc,wndpl.rcNormalPosition,
					fPIT_MIN_X_AXIS,fPIT_MAX_X_AXIS,fPIT_MIN_Y_AXIS,fPIT_MAX_Y_AXIS,
					pcsm->stTransfer.fPitchSlope,pcsm->stTransfer.fPitchInt,pcsm->stTransfer.fPitchMin,pcsm->stTransfer.fPitchMax,fTestVel,pcsm->stTransfer.fMinVelocity);
	}

	if (b_drawworld)
	{
		GetDlgItem(IDC_GRAPH_WORLD)->GetWindowPlacement(&wndpl);
		DrawWorld(dc,wndpl.rcNormalPosition);
	}
}


//*********************************************************************************************
//
bool bFileExist(LPCSTR fname)
{
	HANDLE	load;

	string str_fname = CAudio::pcaAudio->GetBasePathName();
	str_fname += fname;

	load=CreateFile(str_fname.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_READONLY|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if (load==INVALID_HANDLE_VALUE)
		return FALSE;
	CloseHandle(load);
	return TRUE;
}


//*********************************************************************************************
//
void CEditMaterialDlg::OnTestSample()
{
	CComboBox*	pcmb;
	bool		b_loop;
	UpdateData(true);


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

	CString cstr_id;
	pcmb->GetLBText(pcmb->GetCurSel(), cstr_id);

	SEffect* peff = edbEffects.peffFindEffectID(cstr_id);

	// check the file is real....
	if ( bFileExist((LPCSTR)peff->cstrFilename) == FALSE )
	{
		::MessageBox(NULL,"The selected identifier is invalid.","Trespasser Effects Editor", MB_OK);
		return;
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
	psamAudio = CAudio::psamCreateSample((LPCSTR)peff->cstrFilename,AU_CREATE_SPATIAL_3D | AU_CREATE_STATIC |
						AU_CREATE_CTRL_FREQUENCY | AU_CREATE_CTRL_VOLUME);
	if (psamAudio == NULL)
	{
		::MessageBox(NULL,"Cannot load the selected sample.","Trespasser Effects Editor", MB_OK);
		return;
	}

	CAudio::pcaAudio->PositionListener(0,0,0);
	CAudio::pcaAudio->OrientListener(0.0,0.0,1.0, 0.0,1.0,0.0);
	CAudio::pcaAudio->CommitSettings();

	if (pcsm->stTransfer.fMinVelocity<fTestVel)
		psamAudio->bPlay(pcsm->stTransfer,fTestVel,fXWorldPos,0.0f,fYWorldPos,b_loop?AU_PLAY_LOOPED:0);
}



//*********************************************************************************************
//
void CEditMaterialDlg::OnCenterSample()
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
//
void CEditMaterialDlg::TestPlay(const CString& sample, bool b_loop)
{
	// check the file is real....
	if ( bFileExist((LPCSTR)sample) == FALSE )
	{
		::MessageBox(NULL,"The selected identifier is invalid.","Trespasser Effects Editor", MB_OK);
		return;
	}

	if (psamTest)
		delete psamTest;

	psamTest = CAudio::psamCreateSample(sample,AU_CREATE_STEREO | AU_CREATE_STATIC);
	if (psamTest == NULL)
	{
		::MessageBox(NULL,"Cannot load the selected sample.","Trespasser Effects Editor", MB_OK);
		return;
	}

	psamTest->bPlay(b_loop?AU_PLAY_LOOPED:0);
}


//*********************************************************************************************
//
void CEditMaterialDlg::OnTestS1() 
{	
	CString cstr_id;
	cmbSample1.GetLBText(cmbSample1.GetCurSel(), cstr_id);

	SEffect* peff = edbEffects.peffFindEffectID(cstr_id);

	TestPlay(peff->cstrFilename);
}

//*********************************************************************************************
//
void CEditMaterialDlg::OnTestS2() 
{
	CString cstr_id;
	cmbSample2.GetLBText(cmbSample2.GetCurSel(), cstr_id);

	SEffect* peff = edbEffects.peffFindEffectID(cstr_id);

	TestPlay(peff->cstrFilename);
}

//*********************************************************************************************
//
void CEditMaterialDlg::OnTestS3() 
{
	CString cstr_id;
	cmbSample3.GetLBText(cmbSample3.GetCurSel(), cstr_id);

	SEffect* peff = edbEffects.peffFindEffectID(cstr_id);

	TestPlay(peff->cstrFilename, true);
}

//*********************************************************************************************
//
void CEditMaterialDlg::OnCollide() 
{
	CString	sample1;
	CString sample2;
	CString cstr_id;

	UpdateMinTime();

	cmbSample1.GetLBText(cmbSample1.GetCurSel(), cstr_id);
	SEffect* peff = edbEffects.peffFindEffectID(cstr_id);
	sample1 = peff->cstrFilename;

	cmbSample2.GetLBText(cmbSample2.GetCurSel(), cstr_id);
	peff = edbEffects.peffFindEffectID(cstr_id);
	sample2 = peff->cstrFilename;

	// check the file 1 is real....
	if ( bFileExist((LPCSTR)sample1) == FALSE )
	{
		MessageBox("Identifier for sample 1 is invalid.","Trespasser Effects Editor", MB_OK|MB_ICONINFORMATION);
		return;
	}

	// check file 2 is real....
	if ( bFileExist((LPCSTR)sample2) == FALSE )
	{
		MessageBox("Identifier for sample 1 is invalid.","Trespasser Effects Editor", MB_OK|MB_ICONINFORMATION);
		return;
	}

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

	psamCol1 = CAudio::psamCreateSample(sample1,AU_CREATE_SPATIAL_3D | AU_CREATE_STATIC | 
						AU_CREATE_CTRL_FREQUENCY | AU_CREATE_CTRL_VOLUME);
	if (psamCol1 == NULL)
	{
		MessageBox("Cannot load sample 1.","Trespasser Effects Editor", MB_OK|MB_ICONINFORMATION);
		return;
	}

	psamCol2 = CAudio::psamCreateSample(sample2,AU_CREATE_SPATIAL_3D | AU_CREATE_STATIC |
					AU_CREATE_CTRL_FREQUENCY | AU_CREATE_CTRL_VOLUME);
	if (psamCol2 == NULL)
	{
		MessageBox("Cannot load sample 2.","Trespasser Effects Editor", MB_OK|MB_ICONINFORMATION);
		return;
	}

	CAudio::pcaAudio->PositionListener(0,0,0);
	CAudio::pcaAudio->OrientListener(0.0,0.0,1.0, 0.0,1.0,0.0);
	CAudio::pcaAudio->CommitSettings();

	if (edbEffects.bFindCollision(u8Collision))
	{
		if (edbEffects.cmCollisions[u8Collision].csmList[0].stTransfer.fMinVelocity<fTestVel)
		{
			psamCol1->bPlay(edbEffects.cmCollisions[u8Collision].csmList[0].stTransfer,
				fTestVel,fXWorldPos,0.0f,fYWorldPos);
		}

		if (edbEffects.cmCollisions[u8Collision].csmList[1].stTransfer.fMinVelocity<fTestVel)
		{
			psamCol2->bPlay(edbEffects.cmCollisions[u8Collision].csmList[1].stTransfer,
				fTestVel,fXWorldPos,0.0f,fYWorldPos);
		}
	}
}


//*********************************************************************************************
//
void CEditMaterialDlg::OnDualClick() 
{
	bool b_enable = ((CButton*)GetDlgItem(IDC_DUAL_CHECK))->GetCheck();

	UpdateMinTime();

	if (b_enable)
	{
		edbEffects.cmCollisions[u8Collision].SetCollisionDefaults(1, edbEffects.u4Hash("MISSING"));
		edbEffects.cmCollisions[u8Collision].SetNumSamples(2);
		Enable(bCollision, bSlide, true);
	}
	else
	{
		if (MessageBox("Are you sure you want to remove the second sample?", "Trespasses Effects Editor", MB_YESNO|MB_ICONQUESTION) == IDNO)
		{
			((CButton*)GetDlgItem(IDC_DUAL_CHECK))->SetCheck(true);
			return;
		}
		edbEffects.cmCollisions[u8Collision].SetCollisionDefaults(1, 0);
		edbEffects.cmCollisions[u8Collision].SetNumSamples(1);
		Enable(bCollision, bSlide, false);
	}
}


//*********************************************************************************************
void CEditMaterialDlg::Enable(bool b_collision, bool b_slide, bool b_dual) 
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
	GetDlgItem(IDC_SM_SLIDER15)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_EM_CENTER)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_COLLIDE)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_TEST_SAMPLE)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_DELAY_TIME)->EnableWindow(b_collision);

	GetDlgItem(IDC_VOL_SLOPE_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_VOL_MIN_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_VOL_MAX_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_VOL_INTS_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_ATTENUATE_TEXT)->EnableWindow(b_collision|b_slide);		

	GetDlgItem(IDC_PIT_SLOPE_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_PIT_MIN_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_PIT_MAX_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_PIT_INTS_TEXT)->EnableWindow(b_collision|b_slide);
	GetDlgItem(IDC_MIN_VEL_TEXT)->EnableWindow(b_collision|b_slide);

	if (bSlide)
	{
		SetComboBoxItem(cmbSample3,edbEffects.cmCollisions[u8Collision].csmSlide.u4SampleID);
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
		SetComboBoxItem(cmbSample3,edbEffects.u4Hash("MISSING"));
	}

	if (bCollision)
	{
		SetComboBoxItem(cmbSample1,edbEffects.cmCollisions[u8Collision].csmList[0].u4SampleID);
		if (bDual)
		{
			SetComboBoxItem(cmbSample2,edbEffects.cmCollisions[u8Collision].csmList[1].u4SampleID);
		}
		else
		{
			SetComboBoxItem(cmbSample2,edbEffects.u4Hash("MISSING"));
		}

		if ((i4SampleSelect == 1) && (!b_dual))
		{
			i4SampleSelect = 0;
		}

		fDelayTime	= edbEffects.cmCollisions[u8Collision].fGetMinDelay();
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
		SetComboBoxItem(cmbSample1,edbEffects.u4Hash("MISSING"));
		SetComboBoxItem(cmbSample2,edbEffects.u4Hash("MISSING"));

		fDelayTime	= 0.0f;
	}

	UpdateData(false);

	UpdateSelectedGraph();
}


//*********************************************************************************************
void CEditMaterialDlg::OnEnableCollision() 
{
	bool b_enable = ((CButton*)GetDlgItem(IDC_COLLISION_ENABLE))->GetCheck();

	if (b_enable)
	{
		if (!edbEffects.bFindCollision(u8Collision))
		{
			edbEffects.u8NewCollision(u8Collision);
		}

		edbEffects.cmCollisions[u8Collision].SetCollisionDefaults(0, edbEffects.u4Hash("MISSING"));
		edbEffects.cmCollisions[u8Collision].SetCollisionDefaults(1, edbEffects.u4Hash("MISSING"));
		edbEffects.cmCollisions[u8Collision].SetNumSamples(1);

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

		if (bSlide)
		{
			// there is still the slide information, so do not delete the database entry
			edbEffects.cmCollisions[u8Collision].RemoveCollision();
		}
		else
		{
			// there is no slide info so lets delete the entry from the database
			edbEffects.bDeleteCollision(u8Collision);
		}
	}
}


//*********************************************************************************************
void CEditMaterialDlg::OnEnableSlide() 
{
	bool b_enable = ((CButton*)GetDlgItem(IDC_SLIDE_ENABLE))->GetCheck();

	if (b_enable)
	{
		if (!edbEffects.bFindCollision(u8Collision))
		{
			edbEffects.u8NewCollision(u8Collision);
		}

		edbEffects.cmCollisions[u8Collision].EnableSlide();
		edbEffects.cmCollisions[u8Collision].SetSlideDefaults(edbEffects.u4Hash("MISSING"));

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

		if (bCollision)
		{
			// there is still the collision information, so do not delete the database entry
			edbEffects.cmCollisions[u8Collision].RemoveSlide();
		}
		else
		{
			// there is no collision info so lets delete the entry from the database
			edbEffects.bDeleteCollision(u8Collision);
		}
	}
}

//*********************************************************************************************
void CEditMaterialDlg::OnSample1Change()
{
	if (!bCollision)
		return;

	CString cstr_id;
	cmbSample1.GetLBText(cmbSample1.GetCurSel(), cstr_id);
	SEffect* peff = edbEffects.peffFindEffectID(cstr_id);
	edbEffects.cmCollisions[u8Collision].csmList[0].u4SampleID = edbEffects.u4Hash(peff->cstrID);
}

//*********************************************************************************************
void CEditMaterialDlg::OnSample2Change()
{
	if (!bCollision)
		return;

	CString cstr_id;
	cmbSample2.GetLBText(cmbSample2.GetCurSel(), cstr_id);
	SEffect* peff = edbEffects.peffFindEffectID(cstr_id);
	edbEffects.cmCollisions[u8Collision].csmList[1].u4SampleID = edbEffects.u4Hash(peff->cstrID);
}


//*********************************************************************************************
void CEditMaterialDlg::OnSample3Change()
{
	if (!bSlide)
		return;

	CString cstr_id;
	cmbSample3.GetLBText(cmbSample3.GetCurSel(), cstr_id);
	SEffect* peff = edbEffects.peffFindEffectID(cstr_id);
	edbEffects.cmCollisions[u8Collision].csmSlide.u4SampleID = edbEffects.u4Hash(peff->cstrID);
}


//*********************************************************************************************
void CEditMaterialDlg::UpdateSelectedGraph()
{
	if ( (edbEffects.bFindCollision(u8Collision)) && ((bCollision) || (bSlide)) )
	{
		switch (i4SampleSelect)
		{
		case 0:
			pcsm = &edbEffects.cmCollisions[u8Collision].csmList[0];
			GetDlgItem(IDC_COLLIDE)->EnableWindow(bDual);
			GetDlgItem(IDC_TEST_SAMPLE)->SetWindowText("Test Collision Sample 1");
			break;
		case 1:
			pcsm = &edbEffects.cmCollisions[u8Collision].csmList[1];
			GetDlgItem(IDC_COLLIDE)->EnableWindow(bDual);
			GetDlgItem(IDC_TEST_SAMPLE)->SetWindowText("Test Collision Sample 2");
			break;
		case 2:
			pcsm = &edbEffects.cmCollisions[u8Collision].csmSlide;
			GetDlgItem(IDC_COLLIDE)->EnableWindow(false);
			GetDlgItem(IDC_TEST_SAMPLE)->SetWindowText("Test Slide Sample");
			break;
		default:
			pcsm = NULL;
		}
	}
	else
	{
		pcsm = NULL;
	}

	SetGraphs();

	Invalidate();
}


//*********************************************************************************************
void CEditMaterialDlg::OnRadio()
{
	UpdateData(true);
	UpdateSelectedGraph();
}


//*********************************************************************************************
void CEditMaterialDlg::OnStopSample()
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
void CEditMaterialDlg::PositionSamples()
{
	if (psamAudio)
	{
		psamAudio->SetPosition(fXWorldPos,0.0f,fYWorldPos,false);
	}

	CAudio::pcaAudio->CommitSettings();
}



//*********************************************************************************************
void CEditMaterialDlg::AdjustSamples()
{
	if (pcsm == NULL)
		return;

	float	f_pitch	= pcsm->stTransfer.fCalculatePitch(fTestVel);

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
		if (pcsm->stTransfer.fMinVelocity<fTestVel)
		{
			psamAudio->SetFrequency(f_pitch);
			psamAudio->SetVolume( -50.0f + (pcsm->stTransfer.fCalculateVolume(fTestVel)*50.0f) );
			psamAudio->SetAttenuation(pcsm->stTransfer.fAttenuate);
		}
		else
		{
			psamAudio->SetVolume( -100.0f);
		}
	}

	CAudio::pcaAudio->CommitSettings();
}


//*********************************************************************************************
bool CEditMaterialDlg::UpdateMinTime()
{
	if (edbEffects.bFindCollision(u8Collision))
	{
		if (!UpdateData(true))
		{
			fDelayTime = 0.0f;
			MessageBox("Minimum delay time has been set to zero","Trespasser Effects Error",MB_OK|MB_ICONINFORMATION);
			UpdateData(false);
		}

		edbEffects.cmCollisions[u8Collision].SetMinimumDelay(fDelayTime);
	}

	return true;
}