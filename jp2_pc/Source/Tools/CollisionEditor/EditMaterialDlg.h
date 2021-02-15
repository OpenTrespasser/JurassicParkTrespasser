#ifndef _H_EDIT_MATERIAL_DIALOG
#define _H_EDIT_MATERIAL_DIALOG

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

const float	fVEL_SLIDER_MAX		= 1.0f;
const float	fVEL_SLIDER_UNITS	= 100.0f;

class CEditMaterialDlg : public CDialog
{

private:
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



// Construction
public:

	CEditMaterialDlg(CWnd* pParent = NULL);   // standard constructor
	~CEditMaterialDlg();

	void DrawGraph(CDC& cdc,				// device context
				RECT& rect,					// bounding rectange of graph
				float f_xmin,				// min X axis
				float f_xmax,				// max X axis
				float f_ymin,				// min Y axis
				float f_ymax,				// max Y axis
				float f_grad,				// gradient
				float f_ints,				// Y axis intersection
				float f_fmin,				// graph min y value (lower region)
				float f_fmax,				// graph max y value (upper region)
				float f_testvel,			// x position of the reference velocity
				float f_minvel);			// minimum velocity that collision can occur at

	// draws the world position icon			
	void DrawWorld(CDC& cdc,				// device context
					RECT& rect);			// bounding rectange of graph


	void SetWorldPos();
	void WorldPosText();
	void SetGraphs();
	void SetLocals();
	void SetSample2State(bool b_state);
	void AddIdentifiersToCombo(CComboBox& cmb);
	void AddMaterialsToList(CListBox& lb, uint32 u4_data);
	void SetComboBoxItem(CComboBox& cmb, uint32 u4_data);
	void SetupCollision(uint32 u4_mat1, uint32 u4_mat2);
	void Enable(bool b_collision, bool b_slide, bool b_dual);
	void UpdateSelectedGraph();
	static void TestPlay(const CString& sample, bool b_loop = false);
	void PositionSamples();
	void AdjustSamples();

	bool UpdateMinTime();

	static CSample*	psamCol1;
	static CSample*	psamCol2;
	static CSample*	psamAudio;
	static CSample*	psamTest;


// Dialog Data
	//{{AFX_DATA(CEditMaterialDlg)
	enum { IDD = IDD_EDITMATERIAL_DIALOG };

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
	CSliderCtrl		sliderMinVel;

	CListBox		msetList;

	CString			strWorldPos;

	CComboBox		cmbSample1;
	CComboBox		cmbSample2;
	CComboBox		cmbSample3;

	float			fTestVel;
	float			fDelayTime;

	// these two world pos variables acutally represnt X and Z in D3D space, 
	// Y is always zero.
	float			fXWorldPos;
	float			fYWorldPos;
	float			fWorldScale;

	uint32			u4Material;
	uint64			u8Collision;
	bool			bValid;
	bool			bCollision;
	bool			bSlide;
	bool			bDual;
	int32			i4SampleSelect;
	SCollisionSample*	pcsm;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditMaterialDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditMaterialDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg	void OnTestS1();
	afx_msg	void OnTestS2();
	afx_msg	void OnTestS3();
	afx_msg void OnCollide();

	afx_msg void OnRadioClick();
	afx_msg void OnDualClick();


	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	virtual void OnOK() override;
	afx_msg void OnSelChangeList();
	afx_msg void OnTestSample();
	afx_msg void OnStopSample();
	afx_msg void OnTestMaterial();
	afx_msg void OnCenterSample();
	afx_msg void OnEnableCollision();
	afx_msg void OnEnableSlide();
	afx_msg void OnSample1Change();
	afx_msg void OnSample2Change();
	afx_msg void OnSample3Change();

	afx_msg void OnRadio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
