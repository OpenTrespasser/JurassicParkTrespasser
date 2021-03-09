#pragma once

// AudioTestDoc.h : interface of the CAudioTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CAudioTestDoc : public CDocument
{
protected: // create from serialization only
	CAudioTestDoc();
	DECLARE_DYNCREATE(CAudioTestDoc)

// Attributes
public:

	int		scale;
	int		x_displacement, y_displacement;
	int		xmin,ymin,xmax,ymax; // bounding rectangle encompassing all positional objects
	SIZE	size; // logical size of needed screen in pixels
	
	void	update_logical_coords (int x, int y);
	void	update_logical_coords (float scale_factor);
	
	void	convert_dlc (float x, float y, int * px, int * py);
	void	convert_lc (int x, int y, float * px, float * py);
	
	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioTestDoc)
	public:
	virtual BOOL OnNewDocument() override;
	virtual void Serialize(CArchive& ar) override;
	virtual void OnCloseDocument() override;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAudioTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

	CAuScene * GetScene () { return pscene; }

	void control_listener ();
	void control_sound (CAuSound * psound);

protected:

	CAuScene * pscene;

	CAuEnvironmentalSound * env_loop;

	void	init_document ();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CAudioTestDoc)
	afx_msg void OnAudioCreatescene();
	afx_msg void OnAudioAddListener();
	afx_msg void OnUpdateAudioAddListener(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAudioDeleteScene(CCmdUI* pCmdUI);
	afx_msg void OnAudioDeleteScene();
	afx_msg void OnControlPlay();
	afx_msg void OnControlStop();
	afx_msg void OnUpdateControlPlay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateControlStop(CCmdUI* pCmdUI);
	afx_msg void OnControlZoomDown();
	afx_msg void OnControlZoomUp();
	afx_msg void OnAudioEnvironment();
	afx_msg void OnUpdateAudioEnvironment(CCmdUI* pCmdUI);
	afx_msg void OnAudioGameAddsound();
	afx_msg void OnUpdateAudioGameAddsound(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAudioCreatescene(CCmdUI* pCmdUI);
	afx_msg void OnAudioMusicAddsound();
	afx_msg void OnUpdateAudioMusicAddsound(CCmdUI* pCmdUI);
	afx_msg void OnAudioEnvironmentalloopsAddsound();
	afx_msg void OnUpdateAudioEnvironmentalloopsAddsound(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
