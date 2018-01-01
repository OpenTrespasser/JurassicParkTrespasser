/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of "DialogPlayer.h."
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogPlayer.cpp                                              $
 * 
 * 15    98/10/03 4:42 Speter
 * New player settings.
 * 
 * 14    98/09/10 1:10 Speter
 * Better wrist rotation stuff.
 * 
 * 13    98/09/06 3:16 Speter
 * Updated/renamed hand orientation functions for new player command space.
 * 
 * 12    98/09/04 22:02 Speter
 * Moved PlayerSettings to separate file.
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
 * Added settings, changed ranges.
 * 
 * 2     98/04/07 23:46 Speter
 * Added centring options.
 * 
 * 1     98/04/05 19:52 Speter
 * 
 *********************************************************************************************/

// DialogPlayer.cpp : implementation file
//

#include "stdafx.h"
#include "DialogPlayer.h"
#include "Game/DesignDaemon/PlayerSettings.hpp"

/////////////////////////////////////////////////////////////////////////////
// CDialogPlayer dialog


CDialogPlayer::CDialogPlayer(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPlayer::IDD, pParent),

	// Control init.
	ctspinv3HeadOffset	(this, IDC_SPIN_HEAD_X, IDC_SPIN_HEAD_Y, IDC_SPIN_HEAD_Z, 
								&PlayerSettings.v3HeadOffset, -1.0, 1.0, 0.01),
	ctspinv3NeckOffset	(this, IDC_SPIN_NECK_X, IDC_SPIN_NECK_Y, IDC_SPIN_NECK_Z, 
								&PlayerSettings.v3NeckOffset, -1.0, 1.0, 0.01),
	ctspinv3HandToPalm	(this, IDC_SPIN_HAND_OFFSET_X, IDC_SPIN_HAND_OFFSET_Y, IDC_SPIN_HAND_OFFSET_Z, 
								&PlayerSettings.v3WristToPalmNormal, -1.0, 1.0, 0.01),

	ctspinHandDist		(this, IDC_SPIN_HAND_RANGE_MIN, IDC_SPIN_HAND_RANGE_MAX, 
								&PlayerSettings.fHandDistMin, 
								&PlayerSettings.fHandDistMax, 0.0, 2.0, 0.01),

	ctspinHandReachDist	(this, IDC_SPIN_HAND_REACH_DIST, 
								&PlayerSettings.fHandReachSpecial, 0.0, 2.0, 0.01),
	ctspinHandReachWidth(this, IDC_SPIN_HAND_REACH_WIDTH, 
								&PlayerSettings.fHandReachWidth, 0.0, 1.0, 0.01),
	ctspinHandDistPickup(this, IDC_SPIN_HAND_PICKUP_DIST, 
								&PlayerSettings.fHandDistPickup, 0.0, 1.0, 0.005),

	ctspinv2HandMax		(this, IDC_SPIN_HAND_BODY_MAX_X, IDC_SPIN_HAND_BODY_MAX_Y, 
								&PlayerSettings.ang2HandMax, 0, 180, 1, 1/dDEGREES, 0),
	ctspinHandViewX		(this, IDC_SPIN_HAND_VIEW_X_MIN, IDC_SPIN_HAND_VIEW_X_MAX, 
								&PlayerSettings.ang2HandViewMin.tX, 
								&PlayerSettings.ang2HandViewMax.tX, -90, 90, 1, 1/dDEGREES, 0),
	ctspinHandViewY		(this, IDC_SPIN_HAND_VIEW_Y_MIN, IDC_SPIN_HAND_VIEW_Y_MAX, 
								&PlayerSettings.ang2HandViewMin.tY, 
								&PlayerSettings.ang2HandViewMax.tY, -90, 90, 1, 1/dDEGREES, 0),
	ctspinHandRotX		(this, IDC_SPIN_HAND_ROT_X_MIN, IDC_SPIN_HAND_ROT_X_MAX, 
								&PlayerSettings.ang3HandRotMin.tX, 
								&PlayerSettings.ang3HandRotMax.tX, -180, 180, 5, 1/dDEGREES, 0),
	ctspinHandRotY		(this, IDC_SPIN_HAND_ROT_Y_MIN, IDC_SPIN_HAND_ROT_Y_MAX, 
								&PlayerSettings.ang3HandRotMin.tY, 
								&PlayerSettings.ang3HandRotMax.tY, -180, 180, 5, 1/dDEGREES, 0),

	ctspinFlailVelocity	(this, IDC_SPIN_FLAIL_VELOCITY, 
								&PlayerSettings.angSwingPull, 0, 3600, 30, 1/dDEGREES, 0),
	ctspinThrowTorque	(this, IDC_SPIN_THROW_TORQUE, 
								&PlayerSettings.fThrowTorque, -2.0, 2.0, 0.1),

	ctspinHeadMoveFrac	(this, IDC_SPIN_HEAD_FOLLOW, &PlayerSettings.fHeadMoveFrac, 0.0, 1.0, 0.05),

	ctspinv2HeadViewMax	(this, IDC_SPIN_HEAD_BODY_X_MAX, IDC_SPIN_HEAD_BODY_Y_MAX, 
								&PlayerSettings.ang2HeadViewMax, 0, 90, 1, 1/dDEGREES, 0),
	ctspinHeadTurnX		(this, IDC_SPIN_HEAD_TURN_X, 
								&PlayerSettings.fAngHeadTurnX, 0, 90, 1, 1/dDEGREES, 0)
{
	//{{AFX_DATA_INIT(CDialogPlayer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDialogPlayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogPlayer)
		// NOTE: the ClassWizard will add DDX and DDV calls here

	//}}AFX_DATA_MAP
	DDX(pDX);
}


BEGIN_MESSAGE_MAP(CDialogPlayer, CDialog)
	//{{AFX_MSG_MAP(CDialogPlayer)
	ON_BN_CLICKED(IDC_CHECK_HAND_ACTUAL, OnCheckHandActual)
	ON_BN_CLICKED(IDC_CHECK_HEAD_ACTUAL, OnCheckHeadActual)
	ON_BN_CLICKED(IDC_CHECK_CENTRE_EMPTY, OnCheckCentreEmpty)
	ON_BN_CLICKED(IDC_CHECK_CENTRE_HOLDING, OnCheckCentreHolding)
	ON_BN_CLICKED(IDC_CHECK_ALLOW_DROP, OnCheckAllowDrop)
	ON_BN_CLICKED(IDC_CHECK_REVERSE_THROW, OnCheckThrowReverse)
	ON_BN_CLICKED(IDC_CHECK_AUTO_CROTCH_NORMAL, OnCheckAutoCrotchNormal)
	ON_BN_CLICKED(IDC_CHECK_AUTO_CROTCH_SPECIAL, OnCheckAutoCrotchSpecial)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_HEAD_FOLLOW_HAND, OnCheckHeadFollowHand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogPlayer message handlers

void CDialogPlayer::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CheckDlgButton(IDC_CHECK_HAND_ACTUAL,		PlayerSettings.bMoveHandActual);
	CheckDlgButton(IDC_CHECK_HEAD_ACTUAL,		PlayerSettings.bHeadFollowActual);
	CheckDlgButton(IDC_CHECK_CENTRE_EMPTY,		PlayerSettings.bActivateCentreEmpty);
	CheckDlgButton(IDC_CHECK_CENTRE_HOLDING,	PlayerSettings.bActivateCentreHolding);
	CheckDlgButton(IDC_CHECK_ALLOW_DROP,		PlayerSettings.bAllowDrop);
	CheckDlgButton(IDC_CHECK_REVERSE_THROW,		PlayerSettings.bThrowReverse);
//	CheckDlgButton(IDC_CHECK_HOLD_SHOULDER,		PlayerSettings.bHoldRelativeShoulder);
	CheckDlgButton(IDC_CHECK_AUTO_CROTCH_NORMAL,	PlayerSettings.bAutoCrouchNormal);
	CheckDlgButton(IDC_CHECK_AUTO_CROTCH_SPECIAL,	PlayerSettings.bAutoCrouchSpecial);

	CDialog::OnShowWindow(bShow, nStatus);
}

void CDialogPlayer::OnCheckHeadFollowHand() 
{
	PlayerSettings.bHandFollowHead = !PlayerSettings.bHandFollowHead;
}

void CDialogPlayer::OnCheckHandActual() 
{
	PlayerSettings.bMoveHandActual = !PlayerSettings.bMoveHandActual;
}

void CDialogPlayer::OnCheckHeadActual() 
{
	PlayerSettings.bHeadFollowActual = !PlayerSettings.bHeadFollowActual;
}

void CDialogPlayer::OnCheckCentreEmpty() 
{
	PlayerSettings.bActivateCentreEmpty = !PlayerSettings.bActivateCentreEmpty;
}

void CDialogPlayer::OnCheckCentreHolding() 
{
	PlayerSettings.bActivateCentreHolding = !PlayerSettings.bActivateCentreHolding;
}

void CDialogPlayer::OnCheckAllowDrop() 
{
	PlayerSettings.bAllowDrop = !PlayerSettings.bAllowDrop;
}

void CDialogPlayer::OnCheckThrowReverse() 
{
	PlayerSettings.bThrowReverse = !PlayerSettings.bThrowReverse;
}

void CDialogPlayer::OnCheckHoldShoulder() 
{
	PlayerSettings.bHoldRelativeShoulder = !PlayerSettings.bHoldRelativeShoulder;
}

void CDialogPlayer::OnCheckAutoCrotchNormal() 
{
	PlayerSettings.bAutoCrouchNormal = !PlayerSettings.bAutoCrouchNormal;
}

void CDialogPlayer::OnCheckAutoCrotchSpecial() 
{
	PlayerSettings.bAutoCrouchSpecial = !PlayerSettings.bAutoCrouchSpecial;
}

CDialog* newCDialogPlayer()
{
	return new CDialogPlayer();
}


