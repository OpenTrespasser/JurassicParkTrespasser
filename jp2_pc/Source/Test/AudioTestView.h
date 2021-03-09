#pragma once

// AudioTestView.h : interface of the CAudioTestView class
//
/////////////////////////////////////////////////////////////////////////////

class CAudioTestView : public CScrollView
{
protected: // create from serialization only
	CAudioTestView();
	DECLARE_DYNCREATE(CAudioTestView)

// Attributes
public:
	CAudioTestDoc* GetDocument();

	CAuPositionalObject *		pos_obj_moving;
	
	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioTestView)
	public:
	virtual void OnDraw(CDC* pDC) override;  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual void OnInitialUpdate() override;
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) override;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAudioTestView();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAudioTestView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AudioTestView.cpp
inline CAudioTestDoc* CAudioTestView::GetDocument()
   { return (CAudioTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
