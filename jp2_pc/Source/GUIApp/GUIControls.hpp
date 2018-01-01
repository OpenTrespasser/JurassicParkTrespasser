/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		CControlSpin
 *			CControlSpinClamp
 *			CControlSpinRange
 *			CControlSpinVector2
 *			CControlSpinVector3
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/GUIControls.hpp                                               $
 * 
 * 3     98/05/12 18:32 Speter
 * Added PlayerSettings.bHandFollowHead option.
 * 
 * 2     98/04/22 12:22 Speter
 * Updated controls to interact with dialog better. Made .cpp file.
 * 
 * 1     98/04/06 10:44 Speter
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_GUICONTROLS_HPP
#define HEADER_GUIAPP_GUICONTROLS_HPP

#include "Lib/Transform/Vector.hpp"

#include <list.h>

#pragma warning(disable: 4355)

//
// Control classes.
//

class CController;

//**********************************************************************************************
//
class CConnection
//
// Prefix: conn
//
// Connection class for controls.
//
//**************************************
{
public:
	list<CController*> lsControls;

	//******************************************************************************************
	//
	void Add
	(
		CController* pctrl
	);
	//
	//**********************************

	//******************************************************************************************
	//
	virtual void Update
	(
		CController& ctrl				// Control doing the updating.
	)
	//
	// Updates environment based on new data.
	//
	//**********************************
	{
	}

	//******************************************************************************************
	//
	void DDX
	(
		CDataExchange* pDX
	);
	//
	// Sets up DDX with the given window id.
	//
	//**********************************

	//******************************************************************************************
	//
	void Display();
	//
	// Refreshes display of all controls. 
	//
	//**********************************
};

//**********************************************************************************************
//
class CController
//
// Prefix: ctlr
//
//**************************************
{
protected:
	CConnection*	pConnection;		// Handles updates, etc.
	int				iID;				// Dialog window ID.

public:

	//******************************************************************************************
	CController(CConnection* pconn, int i_id)
		: pConnection(pconn), iID(i_id)
	{
		Assert(pConnection);
		pConnection->Add(this);
	}


protected:

	//******************************************************************************************
	//
	virtual void DDX
	(
		CDataExchange* pDX
	)
	//
	// Sets up DDX with the given window id.
	//
	//**********************************
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

public:

	//******************************************************************************************
	int iGetID() const
	{
		return iID;
	}

	//******************************************************************************************
	//
	virtual void Display() const
	//
	// Display the current value.
	//
	//**********************************
	{
	}

	friend CConnection;
};

//**********************************************************************************************
//
class CControlSpinBase: public CController, public CSpinButtonCtrl
//
// Prefix: ctspin
//
//**************************************
{
	typedef CSpinButtonCtrl	TParent;

public:

	//******************************************************************************************
	CControlSpinBase(CConnection* pconn, int i_id)
		: CController(pconn, i_id)
	{
	}

protected:

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void DDX(CDataExchange* pDX)
	{
		::DDX_Control(pDX, iID, *this);
		Display();
	}
/*
	//******************************************************************************************
	virtual void PreSubclassWindow( )
	{
		// Empirically determined, this is the magic override in which the control's
		// initial value must be displayed.
		Display();
	}
 */
	//******************************************************************************************
	virtual BOOL OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult )
	{
		// Dispatch messages.
		if (message == WM_NOTIFY)
		{
			NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)lParam;
			if (pNMUpDown->hdr.code == UDN_DELTAPOS)
			{
				if (OnDeltaPos(pNMUpDown))
					return 1;
			}
		}

		return TParent::OnChildNotify(message, wParam, lParam, pLResult);
	}
  
	//******************************************************************************************
	virtual BOOL OnDeltaPos( NM_UPDOWN* pNMUpDown ) = 0;
  
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual void Display() const
	//
	// Display the current value.
	//
	//**********************************
	{
		// Get "buddy" window, assume it is some sort of text display, and set it.
		CWnd* wnd_buddy = GetBuddy();
		if (wnd_buddy)
		{
			char str_val[30];
			Format(str_val);
			wnd_buddy->SetWindowText(str_val);
		}
	}

	//******************************************************************************************
	//
	virtual void Format
	(
		char str_text[]
	) const = 0;
	//
	// Format the current value into str_text.
	//
	//**********************************
};

//**********************************************************************************************
//
template<class T = TReal> class CControlSpin: public CControlSpinBase
//
// Prefix: ctspin
//
//**************************************
{
	typedef CSpinButtonCtrl	TParent;

protected:
	T*	ptVar;							// Variable this controls.
	T	tMin, tMax, tInc;				// Range and increments for values.
	T	tDisplayFactor;					// Factor translating actual value to display.
	int iPrecision;						// Display precision.

public:

	//******************************************************************************************
	CControlSpin(CConnection* pconn, int i_id, T* pt_var, T  t_min, T t_max, T t_inc, T t_factor = (T)1, int i_prec = 2)
		: 
		CControlSpinBase(pconn, i_id), ptVar(pt_var), 
		tMin(t_min), tMax(t_max), tInc(t_inc), 
		tDisplayFactor(t_factor),
		iPrecision(i_prec)
	{
		Assert(ptVar);
		Assert(tMax >= tMin);
		Assert(tInc >= T(0));
		Assert(iPrecision >= 0);
	}

protected:

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual BOOL OnDeltaPos( NM_UPDOWN* pNMUpDown )
	{
		//
		// Handle spinner control.
		//

		// We totally ignore the spinner's idea of range and current position,
		// and just perform use the delta to increment ourselves.
		// Also, we negate iDelta, because it's backwards... +1 is down!
		SetVal(MinMax(tGetVal() - pNMUpDown->iDelta * tInc, tMin, tMax));

		return 0;
	}
  
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual void SetVal
	(
		T val							// Value to set.
	)
	//
	// Sets the variable to the given value.
	// Derived classes may incorporate some kind of translation.
	//
	//**********************************
	{
		*ptVar = val / tDisplayFactor;

		// Display new value.
		Display();

		if (pConnection)
			pConnection->Update(*this);
	}

	//******************************************************************************************
	//
	virtual T tGetVal() const
	//
	// Returns:
	//		The value of the variable.
	//
	// Derived classes may incorporate some kind of translation.
	//
	//**********************************
	{
		return *ptVar * tDisplayFactor;
	}

	//******************************************************************************************
	virtual void Format(char str_text[]) const;
};

	//******************************************************************************************
	//
	// Specialisations.
	//

	//******************************************************************************************
	void CControlSpin<float>::Format(char str_text[]) const;

	//******************************************************************************************
	void CControlSpin<int>::Format(char str_text[]) const;


//**********************************************************************************************
enum EMinMax {emmMIN, emmMAX};
// Prefix: emm

//**********************************************************************************************
//
template<class T = TReal> class CControlSpinClamp: public CControlSpin<T>
//
// Prefix: ctspinc
//
//**************************************
{
	typedef CControlSpinClamp<T> TThis;
	typedef CControlSpin<T> TParent;

protected:
	T* ptVarClamp;						// Companion variable which clamps this one.
	EMinMax MinMax;						// Which end of the range this var clamps.

public:

	//******************************************************************************************
	CControlSpinClamp(CConnection* pconn, int i_id, T* pt_var, T t_min, T t_max, T t_inc, T t_factor, int i_prec, T* pt_var_clamp, EMinMax emm) :
		CControlSpin<T>(pconn, i_id, pt_var, t_min, t_max, t_inc, t_factor, i_prec),
		ptVarClamp(pt_var_clamp), MinMax(emm)
	{
	}

protected:

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual BOOL OnDeltaPos( NM_UPDOWN* pNMUpDown )
	{
		if (MinMax == emmMIN)
			tMin = *ptVarClamp * tDisplayFactor;
		else
			tMax = *ptVarClamp * tDisplayFactor;

		return TParent::OnDeltaPos(pNMUpDown);
	}
};

//**********************************************************************************************
//
template<class T = TReal> class CControlSpinRange
//
// Prefix: ctspinr
//
//**************************************
{
	CControlSpinClamp<T>	ctspincMin, ctspincMax;

public:

	//******************************************************************************************
	CControlSpinRange(CConnection* pconn, int i_id_min, int i_id_max, 
					  T* pt_var_min, T* pt_var_max, 
					  T t_min, T t_max, T t_inc, T t_factor = T(1), int i_prec = 2) :
		ctspincMin(pconn, i_id_min, pt_var_min, t_min, t_max, t_inc, t_factor, i_prec, pt_var_max, emmMAX),
		ctspincMax(pconn, i_id_max, pt_var_max, t_min, t_max, t_inc, t_factor, i_prec, pt_var_min, emmMIN)
	{
	}
};

//**********************************************************************************************
//
template<class T = TReal> class CControlSpinVector2
//
// Prefix: ctspinv2
//
//**************************************
{
	CControlSpin<T>	ctspinX, ctspinY;

public:

	//******************************************************************************************
	CControlSpinVector2(CConnection* pconn, int i_id_x, int i_id_y, CVector2<T>* pv2, 
						T t_min, T t_max, T t_inc, T t_factor = T(1), int i_prec = 2) :
		ctspinX(pconn, i_id_x, &pv2->tX, t_min, t_max, t_inc, t_factor, i_prec),
		ctspinY(pconn, i_id_y, &pv2->tY, t_min, t_max, t_inc, t_factor, i_prec)
	{
	}
};

//**********************************************************************************************
//
template<class T = TReal> class CControlSpinVector3
//
// Prefix: ctspinv3
//
//**************************************
{
	CControlSpin<T>	ctspinX, ctspinY, ctspinZ;

public:

	//******************************************************************************************
	CControlSpinVector3(CConnection* pconn, int i_id_x, int i_id_y, int i_id_z, CVector3<>* pv3, 
					    T t_min, T t_max, T t_inc, T t_factor = T(1), int i_prec = 2) :
		ctspinX(pconn, i_id_x, &pv3->tX, t_min, t_max, t_inc, t_factor, i_prec),
		ctspinY(pconn, i_id_y, &pv3->tY, t_min, t_max, t_inc, t_factor, i_prec),
		ctspinZ(pconn, i_id_z, &pv3->tZ, t_min, t_max, t_inc, t_factor, i_prec)
	{
	}
};


template<class TX> class MicrosoftHasShitForBrains;

#endif
