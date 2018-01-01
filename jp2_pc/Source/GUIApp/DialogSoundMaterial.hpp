/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998
 *
 * Contents:	Sound Material Dialog
 *
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogSoundMaterial.hpp                                       $
 * 
 * 2     5/15/98 6:59p Rwyatt
 * A dialog the same as the collision editor available from GUIapp.
 * 
 * 1     5/14/98 2:19a Rwyatt
 * Initial implementation
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGSOUNDMAT_HPP
#define HEADER_GUIAPP_DIALOGSOUNDMAT_HPP

#include "Lib\EntityDBase\Instance.hpp"

class CSample;
//
// Class definitions.
//

#define u4MAX_COLLISIONS	256

const float	fVOL_MIN_Y_AXIS=0.0f;
const float	fVOL_MAX_Y_AXIS=1.0f;
const float	fVOL_MIN_X_AXIS=0.0f;
const float	fVOL_MAX_X_AXIS=1.0f;

const float	fPIT_MIN_Y_AXIS=-1.0f;
const float	fPIT_MAX_Y_AXIS=1.0f;
const float	fPIT_MIN_X_AXIS=0.0f;
const float	fPIT_MAX_X_AXIS=1.0f;

const int	iVOL_MAX_SLOPE=10;
const int	iVOL_SLIDER_MAX_SLOPE=1000;
const int	iVOL_MAX_INTERSECT=10;
const int	iVOL_SLIDER_MAX_INTERSECT=1000;
const int	iVOL_SLIDER_MAX_REGION=1000;

const int	iPIT_MAX_SLOPE=10;
const int	iPIT_SLIDER_MAX_SLOPE=1000;
const int	iPIT_MAX_INTERSECT=10;
const int	iPIT_SLIDER_MAX_INTERSECT=1000;
const int	iPIT_SLIDER_MAX_REGION=1000;

const int	iTEST_VEL_SLIDER_MAX=1000;
const float	fWORLD_SCALE_SLIDER_MAX=1000;
const float	fMIN_WORLD_SCALE = 5.0f;
const float	fMAX_WORLD_SCALE = 100.0f;
const float fWORLD_UNITS = 100.0f;

const float fMAX_ATT = 7.0f;
const float fMIN_ATT = 0.001f;
const float	fATTENUATE_SLIDER_MAX = 500.0f;


//*********************************************************************************************
//
class CDialogSoundMaterial : public CDialog
//
// Fog settings dialog box.
//
//**************************************
{
public:
	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CDialogSoundMaterial(CWnd* pParent = NULL);

	CInstance*			pinsCurrent;
	bool				bInit;
	TSoundHandle		asndhndCollisions[u4MAX_COLLISIONS];
	uint32				u4Material;
	uint32				u4Collisions;
	uint32				u4Selected;
	uint64				u8Collision;
	bool				bSlide;
	bool				bDual;
	bool				bCollision;
	bool				bValid;
	SSoundTransfer*		pst;
	float				fWorldScale;
	float				fTestVel;
	float				fXWorldPos;
	float				fYWorldPos;

	CSample*		psamCol1;
	CSample*		psamCol2;
	CSample*		psamAudio;
	CSample*		psamTest;

	//*****************************************************************************************
	void SetComboBoxItem(CComboBox& cmb,uint32 u4_data);

	//*****************************************************************************************
	void AddIdentifiersToCombo(CComboBox& cmb);

	//*****************************************************************************************
	void UpdateSelectedGraph();

	//*****************************************************************************************
	void Enable(bool b_collision, bool b_slide, bool b_dual);

	//*****************************************************************************************
	void SetInstance(CInstance* pins);

	//*****************************************************************************************
	void SetCollisions();

	//*****************************************************************************************
	bool bInitOK()
	{
		return bInit;
	}

	//*****************************************************************************************
	void SetupCollision(uint32 u4_mat1, uint32 u4_mat2);

	//*****************************************************************************************
	void SetGraphs();

	//*****************************************************************************************
	void DrawWorld(CDC& cdc,RECT& rect);

	//*****************************************************************************************
	void DrawGraph
	(
		CDC& cdc,
		RECT& rect,
		float f_xmin,
		float f_xmax,
		float f_ymin,
		float f_ymax,
		float f_grad,
		float f_ints,
		float f_fmin,
		float f_fmax,
		float f_vel
	);

	//*****************************************************************************************
	void SetWorldPos();

	//*****************************************************************************************
	void WorldPosText();

	//*****************************************************************************************
	void PositionSamples();

	//*****************************************************************************************
	void AdjustSamples();

	//*****************************************************************************************
	void TestPlay(TSoundHandle sndhnd, bool b_loop);

	//*****************************************************************************************
	virtual void OnOK();

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CDialogSoundMaterial)
	enum { IDD = IDD_EDITMATERIAL_DIALOG };

  	CListBox		msetList;
	CComboBox		cmbSample1;
	CComboBox		cmbSample2;
	CComboBox		cmbSample3;
	int32			i4SampleSelect;
	CSliderCtrl		sliderWorldScale;
	CSliderCtrl		sliderTestVel;
	CSliderCtrl		sliderWorldX;
	CSliderCtrl		sliderWorldY;

	CSliderCtrl		sliderVolSlope;
	CSliderCtrl		sliderVolIntersect;
	CSliderCtrl		sliderVolMin;
	CSliderCtrl		sliderVolMax;
	CSliderCtrl		sliderPitSlope;
	CSliderCtrl		sliderPitInt;
	CSliderCtrl		sliderPitMin;
	CSliderCtrl		sliderPitMax;
	CSliderCtrl		sliderAttenuate;

	CString			strWorldPos;

/*	CSliderCtrl		sliderSkyHeight;
	CSliderCtrl		sliderSkyScale;
	CSliderCtrl		sliderSkyNearFog;
	CSliderCtrl		sliderSkyFarFog;

	CSliderCtrl		sliderWindX;
	CSliderCtrl		sliderWindY;

	CSliderCtrl		sliderDivide;

	CString			strSkyHeight;
	CString			strSkyScale;
	CString			strSkyNearFog;
	CString			strSkyFarFog;

	CString			strWindX;
	CString			strWindY;

	CString			strDivide;*/


	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogSoundMaterial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_MSG(CDialogSoundMaterial)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelChangeList();
	afx_msg void OnPaint();
	afx_msg void OnRadio();
	afx_msg void OnCenterSample();
	afx_msg void OnSample1Change();
	afx_msg void OnSample2Change();
	afx_msg void OnSample3Change();
	afx_msg void OnEnableCollision();
	afx_msg void OnEnableSlide();
	afx_msg void OnClose();
	afx_msg void OnStopSample();
	afx_msg	void OnTestS1();
	afx_msg	void OnTestS2();
	afx_msg	void OnTestS3();
	afx_msg void OnDualClick();
	afx_msg void OnTestSingle();
	afx_msg void OnTestDual();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	// helper functions to convert button position into a slope and visa versa
	// a second set of functions is provided for the pitch slope
	//
	inline int vol_slope_to_slider(float slope)
	{
		return slope*(iVOL_SLIDER_MAX_SLOPE/iVOL_MAX_SLOPE);
	}

	inline float slider_to_vol_slope(int pos)
	{
		return ((float)pos) / ( (float)iVOL_SLIDER_MAX_SLOPE / (float)iVOL_MAX_SLOPE );
	}

	inline int pitch_slope_to_slider(float slope)
	{
		return slope*(iPIT_SLIDER_MAX_SLOPE/iPIT_MAX_SLOPE);
	}

	inline float slider_to_pitch_slope(int pos)
	{
		return ((float)pos) / ( (float)iPIT_SLIDER_MAX_SLOPE / (float)iPIT_MAX_SLOPE );
	}

	// helper functions to convert button position into a an intersect pos and visa versa
	// a second set of functions is provided for the pitch slope
	//
	inline int vol_intersect_to_slider(float ints)
	{
		return ints*(iVOL_SLIDER_MAX_INTERSECT/iVOL_MAX_INTERSECT);
	}

	inline float slider_to_vol_intersect(int pos)
	{
		return ((float)pos) / ( (float)iVOL_SLIDER_MAX_INTERSECT / (float)iVOL_MAX_INTERSECT );
	}

	inline int pitch_intersect_to_slider(float ints)
	{
		return ints*(iPIT_SLIDER_MAX_INTERSECT/iPIT_MAX_INTERSECT);
	}

	inline float slider_to_pitch_intersect(int pos)
	{
		return ((float)pos) / ( (float)iPIT_SLIDER_MAX_INTERSECT / (float)iPIT_MAX_INTERSECT );
	}

	// helper functions to convert button position into a region limit and visa versa
	// a second set of functions is provided for the pitch slope
	//
	inline int vol_region_to_slider(float reg)
	{
		return reg*(iVOL_SLIDER_MAX_REGION/fVOL_MAX_Y_AXIS);
	}

	inline float slider_to_vol_region(int pos)
	{
		return ((float)pos) / ( (float)iVOL_SLIDER_MAX_REGION / (float)fVOL_MAX_Y_AXIS );
	}

	inline int pitch_region_to_slider(float reg)
	{
		return reg*(iPIT_SLIDER_MAX_REGION/fPIT_MAX_Y_AXIS);
	}

	inline float slider_to_pitch_region(int pos)
	{
		return ((float)pos) / ( (float)iPIT_SLIDER_MAX_REGION / (float)fPIT_MAX_Y_AXIS );
	}


	inline int vel_to_slider(float vel)
	{
		return vel*iTEST_VEL_SLIDER_MAX;
	}

	inline float slider_to_vel(int pos)
	{
		return pos / (float)iTEST_VEL_SLIDER_MAX;
	}


	inline int scale_to_slider(float scale)
	{
		return ( (scale/(fMAX_WORLD_SCALE-fMIN_WORLD_SCALE))*fWORLD_SCALE_SLIDER_MAX);
	}


	inline float slider_to_scale(int pos)
	{
		return fMIN_WORLD_SCALE+((pos / fWORLD_SCALE_SLIDER_MAX) * (fMAX_WORLD_SCALE-fMIN_WORLD_SCALE));
	}

	inline int worldpos_to_slider(float pos)
	{
		return (pos*fWORLD_UNITS);
	}

	inline float slider_to_worldpos(int pos)
	{
		return ((float)pos/fWORLD_UNITS);
	}

	inline int attenuate_to_slider(float scale)
	{
		return ( (scale/(fMAX_ATT-fMIN_ATT))*fATTENUATE_SLIDER_MAX);
	}

	inline float slider_to_attenuate(int pos)
	{
		return fMIN_ATT+((pos / fATTENUATE_SLIDER_MAX) * (fMAX_ATT-fMIN_ATT));
	}
};


#endif
