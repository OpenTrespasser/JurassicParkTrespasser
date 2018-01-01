/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		CDialogMaterial
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogMaterial.h                                              $
 * 
 * 1     98/04/22 12:31 Speter
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGMATERIAL_HPP
#define HEADER_GUIAPP_DIALOGMATERIAL_HPP

#include "GUIControls.hpp"
#include "Lib/Renderer/Material.hpp"
#include "Lib/Math/FastTrig.hpp"

class CInstance;
class CTexture;

//**********************************************************************************************
//
class CControlSpinAngleWidth: public CControlSpin<float>
//
// Prefix: ctspinangw
//
// Specialises CControlSpin<float> to provide translation between angle and cosine.
//
//**************************************
{
	typedef float	T;
	typedef CControlSpin<float> TParent;

public:

	//******************************************************************************************
	CControlSpinAngleWidth(CConnection* pconn, int i_id, T* pt_var, T  t_min, T t_max, T t_inc, int i_prec = 0)
		// Use a display factor of 1 (we convert from degrees ourselves).
		: CControlSpin<float>(pconn, i_id, pt_var, t_min, t_max, t_inc, 1, i_prec)
	{
	}

protected:

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void SetVal(T val)
	//
	// Converts the angle (in degrees) to a cosine.
	//
	{
		TParent::SetVal(cos(val * dDEGREES));
	}

	//******************************************************************************************
	virtual T tGetVal() const
	//
	// Converts the cosine to an angle (in degrees).
	//
	{
		return acos(*ptVar) / dDEGREES;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CDialogMaterial dialog

class CDialogMaterial : public CDialog, public CConnection
{
// Construction
public:

	//**********************************************************************************************
	CDialogMaterial(CWnd* pParent = 0);   // standard constructor

	//**********************************************************************************************
	void SetMaterial(CInstance* pins, int i_index);


// Dialog Data
	//{{AFX_DATA(CDialogMaterial)
	enum { IDD = IDD_MATERIAL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CInstance*				pinsEdit;			// The instance being edited.
	int						iMaterialIndex,		// The index of the material being edited.
							iLastIndex;			// Previous one.
	rptr<CTexture>			ptexEdit;			// The current texture being edited.
	CMaterial				matLocal;			// The local copy of the material being edited.

	// Controls in this dialog.
	CControlSpin<int>		ctspiniIndex;
	CControlSpin<>			ctspinDiffuse;
	CControlSpin<>			ctspinSpecular;
	CControlSpinAngleWidth	ctspinangwSpecular;
	CControlSpin<>			ctspinEmissive;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogMaterial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogMaterial)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//******************************************************************************************
	//
	void GetMaterial();
	//
	//**********************************

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void Update(CController& ctrl);
};


//**********************************************************************************************
//
CDialog* newCDialogMaterial(CInstance* pins);
//
// Named constructor.
//
//**************************************

#endif