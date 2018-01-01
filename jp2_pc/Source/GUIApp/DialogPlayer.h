/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		CDialogPlayer
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogPlayer.h                                                $
 * 
 * 11    98/08/05 20:39 Speter
 * Added neck offset.
 * 
 * 10    98/06/11 23:21 Speter
 * More player options.
 * 
 * 9     98/05/22 13:11 Speter
 * Added support for flail mode.
 * 
 * 8     98/05/12 18:32 Speter
 * Added PlayerSettings.bHandFollowHead option.
 * 
 * 7     98/05/06 18:59 Speter
 * Separate auto-crouch options for normal and special boxes.
 * 
 * 6     98/04/22 12:23 Speter
 * Updated spin controls to interact with dialogs better. Added auto-crouch option.
 * 
 * 5     98/04/17 13:08 Speter
 * Changed v3HandToPhys to v3HandToPalm, reversed sense.
 * 
 * 4     98/04/14 20:14 Speter
 * Added button for bHoldRelativeShoulder. 
 * 
 * 3     98/04/08 21:22 Speter
 * Added Player settings.
 * 
 * 2     98/04/07 23:46 Speter
 * Added centring options.
 * 
 * 1     98/04/05 20:37 Speter
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGPLAYER_HPP
#define HEADER_GUIAPP_DIALOGPLAYER_HPP

#include "GUIControls.hpp"

/////////////////////////////////////////////////////////////////////////////
// CDialogPlayer dialog

class CDialogPlayer : public CDialog, public CConnection
{
// Construction
public:
	CDialogPlayer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogPlayer)
	enum { IDD = IDD_PLAYER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Controls in this dialog.
	CControlSpinVector3<>	ctspinv3HeadOffset;
	CControlSpinVector3<>	ctspinv3NeckOffset;
	CControlSpinVector3<>	ctspinv3HandToPalm;

	CControlSpinRange<>		ctspinHandDist;

	CControlSpin<>			ctspinHandReachDist;
	CControlSpin<>			ctspinHandReachWidth;
	CControlSpin<>			ctspinHandDistPickup;

	CControlSpinRange<>		ctspinHandViewX;
	CControlSpinRange<>		ctspinHandViewY;

	CControlSpinRange<>		ctspinHandRotX;
	CControlSpinRange<>		ctspinHandRotY;

	CControlSpin<>			ctspinFlailVelocity;
	CControlSpin<>			ctspinThrowTorque;

	CControlSpin<>			ctspinHeadMoveFrac;

	CControlSpinVector2<>	ctspinv2HandMax;
	CControlSpinVector2<>	ctspinv2HeadViewMax;
	CControlSpin<>			ctspinHeadTurnX;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPlayer)
	protected:
	//}}AFX_VIRTUAL

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogPlayer)
	afx_msg void OnCheckHandActual();
	afx_msg void OnCheckHeadActual();
	afx_msg void OnCheckCentreEmpty();
	afx_msg void OnCheckCentreHolding();
	afx_msg void OnCheckAllowDrop();
	afx_msg void OnCheckThrowReverse();
	afx_msg void OnCheckHoldShoulder();
	afx_msg void OnCheckAutoCrotchNormal();
	afx_msg void OnCheckAutoCrotchSpecial();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCheckHeadFollowHand();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
