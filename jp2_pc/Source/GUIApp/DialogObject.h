/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Dialog for information about objects in the world (CInstance's).
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogObject.h                                                $
 * 
 * 6     5/01/98 4:42p Rwyatt
 * Added mass to the dialog
 * 
 * 5     97/10/30 15:14 Speter
 * CPhysicsInfo::pbvGetBoundVol() now returns const CBoundVol*.
 * 
 * 4     97-05-05 11:52 Speter
 * Changed display of all class info to use typeid().name() function.
 * 
 * 3     12/13/96 6:06p Pkeet
 * Cleaned things up a bit.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGOBJECT_HPP
#define HEADER_GUIAPP_DIALOGOBJECT_HPP

#include "Lib/EntityDBase/PhysicsInfo.hpp"

//*********************************************************************************************
//
class CDialogObject : public CDialog
//
// Object information dialog box.
//
//**************************************
{
protected:

	CInstance* pinsObject;

public:

	CDialogObject(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	void SetInstance
	(
		CInstance* pins	// Pointer to the instance of the object to look at.
	);
	//
	// Sets the pointer to an instance of an object for display purposes.
	//
	//**********************************

// Dialog Data
	//{{AFX_DATA(CDialogObject)
	enum { IDD = IDD_OBJECT_PROPERTIES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogObject)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//*****************************************************************************************
	void DisplayType();
	
	//*****************************************************************************************
	void DisplayPhysicsBoundingBox(const CBoundVolBox* pbvb);

	//*****************************************************************************************
	void DisplayPhysicsBoundingVolume(const CBoundVol* pbv);

	//*****************************************************************************************
	void DisplayPhysicsInfo(const CPhysicsInfo* pphi);

	//*****************************************************************************************
	void DisplayPhysicsInfoBox(const CPhysicsInfo* pphi);

	//*****************************************************************************************
	void DisplayPresenceInfo(const CPresence3<>& pr3);

	//*****************************************************************************************
	void DisplayText();

	// Generated message map functions
	//{{AFX_MSG(CDialogObject)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
