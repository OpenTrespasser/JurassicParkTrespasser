// AudioTestView.cpp : implementation of the CAudioTestView class
//

#include "stdafx.h"

#include "AudioTest.h"

#include <math.h>

#include <assert.h>

#include <yvals.h>

#include "lib/audio/audio.hpp"
#include "Lib/Transform/Vector.hpp"

#include "AudioTestDoc.h"
#include "AudioTestView.h"

#include <list>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAudioTestView

IMPLEMENT_DYNCREATE(CAudioTestView, CScrollView)

BEGIN_MESSAGE_MAP(CAudioTestView, CScrollView)
	//{{AFX_MSG_MAP(CAudioTestView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioTestView construction/destruction

CAudioTestView::CAudioTestView()
{
	// TODO: add construction code here

}

CAudioTestView::~CAudioTestView()
{
}

BOOL CAudioTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAudioTestView drawing

void CAudioTestView::OnDraw(CDC* pDC)
{
	CAudioTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	
	CVector3<float> dir, up, pos;
	CAuScene * pscene = pDoc->GetScene ();

	CWinApp * pwin_app = AfxGetApp ();
	HICON	hicon;
	int x,y;
	
	// draw reference origin cross
	hicon = pwin_app->LoadIcon (IDI_CROSS);
	x = pDoc->x_displacement-6;  // 6 is to center of cross
	y = pDoc->y_displacement-6;
	pDC->DrawIcon (x, y, hicon);
	
	CPen pen_red,pen_blue,*old_pen;
	if (!pen_red.CreatePen (PS_SOLID,1,RGB (255,0,0))) Assert (0);
	if (!pen_blue.CreatePen (PS_SOLID,1,RGB (0,0,255))) Assert (0);
	old_pen = pDC->SelectObject (&pen_red);
	CBrush brush,*old_brush;
	LOGBRUSH logbrush = {BS_NULL,0,0};
	if (!brush.CreateBrushIndirect (&logbrush)) Assert (0);
	old_brush = pDC->SelectObject (&brush);
	
	// draw listener
	CAuListener * plistener;
	if (pscene && (plistener = pscene->GetListener ()) )
	{
		plistener->GetOrientation( &dir, &up );
		plistener->GetPosition( &pos );

		// draw dot
		x = (int)pos.tX*pDoc->scale + pDoc->x_displacement;
		y = (int)pos.tZ*pDoc->scale + pDoc->y_displacement;
		hicon = pwin_app->LoadIcon (IDI_REDDOT);
		pDC->DrawIcon (x-4, y-4, hicon); // 4 is to center of dot

		// draw nose (signifying direction)
		pDC->MoveTo (x,y);
		float norm = (float)sqrt ( dir.tX*dir.tX+dir.tZ*dir.tZ );
		dir.tX = (dir.tX/norm) * 16; // 16 units long
		dir.tZ = (dir.tZ/norm) * 16;
		if (dir.tX < 0) dir.tX-=0.5f; else dir.tX+=0.5f;
		if (dir.tZ < 0) dir.tZ-=0.5f; else dir.tZ+=0.5f;
		x += (int)dir.tX; y += (int)dir.tZ;
		pDC->LineTo (x,y);
	}

	std::list<CAuSound *> * plist_sounds = 0;
	if (pscene)
		plist_sounds = pscene->GetSoundList ();
	if (plist_sounds && !plist_sounds->empty ())
	{
		pDC->SelectObject (&pen_blue);

		SAuSoundModel sound_model;

		std::list<CAuSound*>::iterator i;
		for (i = plist_sounds->begin(); i != plist_sounds->end(); i++)
		{
			(*i)->GetOrientation (&dir);

			// ambient sound - don't draw
			if (dir.tX == 0.0f && dir.tY == 0.0f && dir.tZ == 0.0f)
			{
				continue;
			}
			
			(*i)->GetPosition (&pos);
			(*i)->GetModel (&sound_model);

			x = (int)pos.tX*pDoc->scale + pDoc->x_displacement;
			y = (int)pos.tZ*pDoc->scale + pDoc->y_displacement;
			
			// draw dot
			hicon = pwin_app->LoadIcon (IDI_BLUEDOT);
			pDC->DrawIcon (x-4, y-4, hicon); // 4 pixels to center of dot

			// draw ellipses
			float x_dir, y_dir, x_dir_perp, y_dir_perp;
			float norm = (float)sqrt (dir.tX*dir.tX+dir.tZ*dir.tZ);
			x_dir = dir.tX/norm;
			y_dir = dir.tZ/norm;
			x_dir_perp = -dir.tZ/norm;
			y_dir_perp = dir.tX/norm;

			sound_model.fMinBack*=pDoc->scale;
			sound_model.fMinFront*=pDoc->scale;
			sound_model.fMaxBack*=pDoc->scale;
			sound_model.fMaxFront*=pDoc->scale;
			float x1,y1,x2,y2;

#define VERT_HORZ_SCALE 0.30f

			// inner ellipse
			x1 = x + (-x_dir*sound_model.fMinBack);
			y1 = y + (-y_dir*sound_model.fMinBack);
			x1 += (x_dir_perp*(sound_model.fMinFront+sound_model.fMinBack)*VERT_HORZ_SCALE);
			y1 += (y_dir_perp*(sound_model.fMinFront+sound_model.fMinBack)*VERT_HORZ_SCALE);
			if (x1<0) x1-=0.5f; else x1+=0.5f;
			if (y1<0) y1-=0.5f; else y1+=0.5f;				
			pDC->MoveTo ((int)x1,(int)y1);
			x2 = x + (x_dir*sound_model.fMinFront);
			y2 = y + (y_dir*sound_model.fMinFront);
			x2 += (x_dir_perp*(sound_model.fMinFront+sound_model.fMinBack)*VERT_HORZ_SCALE);
			y2 += (y_dir_perp*(sound_model.fMinFront+sound_model.fMinBack)*VERT_HORZ_SCALE);
			if (x2<0) x2-=0.5f; else x2+=0.5f;
			if (y2<0) y2-=0.5f; else y2+=0.5f;
			pDC->LineTo ((int)x2,(int)y2);
			x2 = x + (x_dir*sound_model.fMinFront);
			y2 = y + (y_dir*sound_model.fMinFront);
			x2 += (-x_dir_perp*(sound_model.fMinFront+sound_model.fMinBack)*VERT_HORZ_SCALE);
			y2 += (-y_dir_perp*(sound_model.fMinFront+sound_model.fMinBack)*VERT_HORZ_SCALE);
			if (x2<0) x2-=0.5f; else x2+=0.5f;
			if (y2<0) y2-=0.5f; else y2+=0.5f;
			pDC->LineTo ((int)x2,(int)y2);
			x2 = x + (-x_dir*sound_model.fMinBack);
			y2 = y + (-y_dir*sound_model.fMinBack);
			x2 += (-x_dir_perp*(sound_model.fMinFront+sound_model.fMinBack)*VERT_HORZ_SCALE);
			y2 += (-y_dir_perp*(sound_model.fMinFront+sound_model.fMinBack)*VERT_HORZ_SCALE);
			if (x2<0) x2-=0.5f; else x2+=0.5f;
			if (y2<0) y2-=0.5f; else y2+=0.5f;
			pDC->LineTo ((int)x2,(int)y2);
			pDC->LineTo ((int)x1,(int)y1);
			
			// outer ellipse
			x1 = x + (-x_dir*sound_model.fMaxBack);
			y1 = y + (-y_dir*sound_model.fMaxBack);
			x1 += (x_dir_perp*(sound_model.fMaxFront+sound_model.fMaxBack)*VERT_HORZ_SCALE);
			y1 += (y_dir_perp*(sound_model.fMaxFront+sound_model.fMaxBack)*VERT_HORZ_SCALE);
			if (x1<0) x1-=0.5f; else x1+=0.5f;
			if (y1<0) y1-=0.5f; else y1+=0.5f;				
			pDC->MoveTo ((int)x1,(int)y1);
			x2 = x + (x_dir*sound_model.fMaxFront);
			y2 = y + (y_dir*sound_model.fMaxFront);
			x2 += (x_dir_perp*(sound_model.fMaxFront+sound_model.fMaxBack)*VERT_HORZ_SCALE);
			y2 += (y_dir_perp*(sound_model.fMaxFront+sound_model.fMaxBack)*VERT_HORZ_SCALE);
			if (x2<0) x2-=0.5f; else x2+=0.5f;
			if (y2<0) y2-=0.5f; else y2+=0.5f;
			pDC->LineTo ((int)x2,(int)y2);
			x2 = x + (x_dir*sound_model.fMaxFront);
			y2 = y + (y_dir*sound_model.fMaxFront);
			x2 += (-x_dir_perp*(sound_model.fMaxFront+sound_model.fMaxBack)*VERT_HORZ_SCALE);
			y2 += (-y_dir_perp*(sound_model.fMaxFront+sound_model.fMaxBack)*VERT_HORZ_SCALE);
			if (x2<0) x2-=0.5f; else x2+=0.5f;
			if (y2<0) y2-=0.5f; else y2+=0.5f;
			pDC->LineTo ((int)x2,(int)y2);
			x2 = x + (-x_dir*sound_model.fMaxBack);
			y2 = y + (-y_dir*sound_model.fMaxBack);
			x2 += (-x_dir_perp*(sound_model.fMaxFront+sound_model.fMaxBack)*VERT_HORZ_SCALE);
			y2 += (-y_dir_perp*(sound_model.fMaxFront+sound_model.fMaxBack)*VERT_HORZ_SCALE);
			if (x2<0) x2-=0.5f; else x2+=0.5f;
			if (y2<0) y2-=0.5f; else y2+=0.5f;
			pDC->LineTo ((int)x2,(int)y2);
			pDC->LineTo ((int)x1,(int)y1);
		}

	}

	pDC->SelectObject (old_pen);
	pDC->SelectObject (old_brush);
}

/////////////////////////////////////////////////////////////////////////////
// CAudioTestView diagnostics

#ifdef _DEBUG
void CAudioTestView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CAudioTestView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CAudioTestDoc* CAudioTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAudioTestDoc)));
	return (CAudioTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAudioTestView message handlers

void CAudioTestView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	SetScrollSizes (MM_TEXT, GetDocument()->size);
}


void CAudioTestView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	SetScrollSizes (MM_TEXT, GetDocument ()->size);
	
	InvalidateRect (NULL);
	
}

#define TOLERANCE 6 // radius tolerance for hit testing

void CAudioTestView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CAudioTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// need to change to logical coords
	CClientDC dc (this);
	OnPrepareDC (&dc);
	dc.DPtoLP (&point);

	CVector3<float> pos;
	CVector3<float> dir;
	CAuScene * pscene = pDoc->GetScene ();

	// hit test
	int found = 0;
	int x,y;
	// sounds first
	std::list<CAuSound *> * plist_sounds = 0;
	if (pscene)
		plist_sounds = pscene->GetSoundList ();
	if (plist_sounds && !plist_sounds->empty())
	{
		std::list<CAuSound *>::iterator i;
		for (i = plist_sounds->begin(); i != plist_sounds->end(); ++i)
		{
			(*i)->GetPosition (&pos);
			(*i)->GetOrientation (&dir);

			if (dir.tX == 0.0f && dir.tY == 0.0f && dir.tZ == 0.0f)
			{
				continue;
			}
	
			pDoc->convert_dlc (pos.tX, pos.tZ, &x, &y);
			
			if (point.x > x-TOLERANCE && point.x < x+TOLERANCE &&
				point.y > y-TOLERANCE && point.y < y+TOLERANCE)
				pDoc->control_sound (*i);
		}
	}

	// listener next
	CAuListener * plistener;
	if (!found &&
		pscene && (plistener = pscene->GetListener ()) )
	{
		plistener->GetPosition (&pos);
		
		pDoc->convert_dlc (pos.tX, pos.tZ, &x, &y);
		
		if (point.x > x-TOLERANCE && point.x < x+TOLERANCE &&
			point.y > y-TOLERANCE && point.y < y+TOLERANCE)
			pDoc->control_listener ();
	}
	
	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CAudioTestView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CAudioTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// need to change to logical coords
	CClientDC dc (this);
	OnPrepareDC (&dc);
	dc.DPtoLP (&point);

	CVector3<float> pos;
	CVector3<float> dir;
	CAuScene * pscene = pDoc->GetScene ();

	pos_obj_moving = 0;

	// hit test
	int x,y;
	std::list<CAuSound *> * plist_sounds = 0;
	if (pscene)
		plist_sounds = pscene->GetSoundList ();
	if (plist_sounds && !plist_sounds->empty())
	{
		std::list<CAuSound*>::iterator i;
		for (i = plist_sounds->begin(); i != plist_sounds->end(); ++i)
		{
			(*i)->GetPosition (&pos);
			(*i)->GetOrientation (&dir);

			if (dir.tX == 0.0f && dir.tY == 0.0f && dir.tZ == 0.0f)
			{
				continue;
			}

			pDoc->convert_dlc (pos.tX, pos.tZ, &x, &y);
			
			if (point.x > x-TOLERANCE && point.x < x+TOLERANCE &&
				point.y > y-TOLERANCE && point.y < y+TOLERANCE)
			{
				pos_obj_moving = (*i);
			}
		}
	}
	
	// listener next
	CAuListener * plistener;
	if (!pos_obj_moving &&
		pscene && (plistener = pscene->GetListener ()) )
	{
		plistener->GetPosition (&pos);
		
		pDoc->convert_dlc (pos.tX, pos.tZ, &x, &y);
		
		if (point.x > x-TOLERANCE && point.x < x+TOLERANCE &&
			point.y > y-TOLERANCE && point.y < y+TOLERANCE)
			pos_obj_moving = pscene->GetListener ();
	}

	if (pos_obj_moving)
		SetCapture ();
	
	
	CScrollView::OnLButtonDown(nFlags, point);
}

void CAudioTestView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (GetCapture () != this)
		return;

	ReleaseCapture ();
	
	CScrollView::OnLButtonUp(nFlags, point);
}

void CAudioTestView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetCapture () != this)
		return;

	CAudioTestDoc *	doc = GetDocument ();
	
	RECT rect;
	rect.left = point.x-15;
	rect.top = point.y-15;
	rect.right = point.x+15;
	rect.bottom = point.y+15;

	// need to change to logical coords
	CClientDC dc (this);
	OnPrepareDC (&dc);
	dc.DPtoLP (&point);

	CVector3<float> pos,old_pos;
	CAuScene * scene = doc->GetScene ();

	pos_obj_moving->GetPosition (&pos);
	old_pos = pos;
	doc->convert_lc (point.x, point.y, &pos.tX, &pos.tZ);
	pos_obj_moving->SetPosition (pos);

	//if (old_pos.tX-pos.tX<0) rect.left-=((long)(pos.tX-old_pos.tX)*doc->scale);
	//else rect.right += ((long)(old_pos.tX-pos.tX)*doc->scale);
	//if (old_pos.tZ-pos.tZ<0) rect.top-=((long)(pos.tZ-old_pos.tZ)*doc->scale);
	//else rect.bottom += ((long)(old_pos.tZ-pos.tZ)*doc->scale);
	
	//if (pos.tX < doc->xmin || pos.tZ < doc->ymin)
	//	InvalidateRect (NULL);

	doc->update_logical_coords ((int)pos.tX, (int)pos.tZ);
	SetScrollSizes (MM_TEXT, GetDocument ()->size);
	//InvalidateRect (&rect, TRUE);
	InvalidateRect (NULL);
	
	CScrollView::OnMouseMove(nFlags, point);
}

