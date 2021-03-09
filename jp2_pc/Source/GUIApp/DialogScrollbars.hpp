/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Scrollbars for dialogs.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogScrollbars.hpp                                          $
 * 
 * 3     98/01/15 13:52 Speter
 * Added ability to update static text display automatically.
 * 
 * 2     12/05/97 3:56p Pkeet
 * Added default values for the 'Set' member function of the logarithmic scale. Added some
 * asserts.
 * 
 * 1     12/05/97 12:11p Pkeet
 * Initial implementation. Moved the basic floating-point scrollbar here from the physics
 * header file.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGSCROLLBARS_HPP
#define HEADER_GUIAPP_DIALOGSCROLLBARS_HPP


//
// Required includes.
//
#include <math.h>


//
// Macros.
//

// Floating point log function.
#define fLog(X) float(log(double(X)))

// Floating point exponent function.
#define fExp(X) float(exp(double(X)))


//
// Class definitions.
//

//*********************************************************************************************
//
class CScrollFloat : public CScrollBar
//
// Floating point scrollbar.
//
//**************************************
{
protected:

	float fMin;				// Minimum value for the scrollbar.
	float fValuePerStep;	// Step value for the scrollbar.
	float fStartValue;		// Value when the set member function was last called.
	int	  iTextID;			// Associated static text control for display.

public:

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	// Default constructor.
	CScrollFloat()
		: CScrollBar()
	{
		// Set the class variables.
		fMin          = 0.0f;
		fValuePerStep = 0.0f;
		fStartValue   = 0.0f;
		iTextID		  = 0;
	}


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual void Set
	(
		float f_current_value,	// Current value of the parameter.
		float f_min,			// Minimum value the scrollbar can represent.
		float f_max,			// Maximum value the scrollbar can represent.
		int   i_steps = 100,	// Number of discrete steps the scrollbar uses.
		int	  i_text_id = 0,	// Associated static text control for display.
		CWnd* pwnd_dialog = 0	// Containing dialog.
	)
	//
	// Sets the scrollbar's internal values.
	//
	//**********************************
	{
		Assert(i_steps > 1);

		// Set the class variables.
		fMin          = f_min;
		float f_range = f_max - f_min;
		fValuePerStep = f_range / float(i_steps-1);
		fStartValue   = f_current_value;
		iTextID		  = i_text_id;

		Assert(fValuePerStep != 0.0f);

		// Initialize the scrollbar.
		SetScrollRange(0, i_steps - 1, false);
		SetPos(fStartValue, pwnd_dialog);
	}

	//*****************************************************************************************
	//
	virtual void SetPos
	(
		float f_current_value,	// Current value of the parameter.
		CWnd* pwnd_dialog = 0	// Containing dialog.
	)
	//
	// Sets the scrollbar to the specified value.
	//
	//**********************************
	{
		// Get the scrollbar position.
		int i_position = int((f_current_value - fMin) / fValuePerStep + 0.5f);

		// Set the scrollbar position.
		SetScrollPos(i_position);
		if (pwnd_dialog && iTextID)
			SetDlgItemFloat(pwnd_dialog, iTextID, f_current_value, 2);
	}

	//*****************************************************************************************
	//
	virtual void Reset
	(
	)
	//
	// Resets the scrollbar to the initial value.
	//
	//**********************************
	{
		SetPos(fStartValue);
	}

	//*****************************************************************************************
	//
	virtual float fGet
	(
		CWnd* pwnd_dialog = 0			// Containing dialog.
	)
	//
	// Gets the value specified by the scrollbar.
	//
	//**********************************
	{
		float f_val = float(GetScrollPos()) * fValuePerStep + fMin;
		if (pwnd_dialog && iTextID)
			SetDlgItemFloat(pwnd_dialog, iTextID, f_val, 2);
		return f_val;
	}
};

//*********************************************************************************************
//
class CScrollFloatLog : public CScrollFloat
//
// Floating point scrollbar using a logarithmic scale.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	// Default constructor.
	CScrollFloatLog()
		: CScrollFloat()
	{
	}


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual void Set
	(
		float f_current_value,	// Current value of the parameter.
		float f_min   = 0.01f,	// Minimum value the scrollbar can represent.
		float f_max   = 100.0f,	// Maximum value the scrollbar can represent.
		int   i_steps = 100,	// Number of discrete steps the scrollbar uses.
		int	  i_text_id = 0,	// Associated static text control for display.
		CWnd* pwnd_dialog = 0	// Containing dialog.
	) override
	//
	// Sets the scrollbar's internal values.
	//
	//**********************************
	{
		Assert(f_current_value > 0.0f);
		Assert(f_min > 0.0f);
		Assert(f_max > 0.0f);
		Assert(i_steps > 1);

		// Set the class variables.
		fMin          = fLog(f_min);
		float f_range = fLog(f_max) - fMin;
		fValuePerStep = f_range / float(i_steps-1);
		fStartValue   = fLog(f_current_value);
		iTextID		  = i_text_id;

		Assert(fValuePerStep != 0.0f);

		// Initialize the scrollbar.
		SetScrollRange(0, i_steps - 1, false);
		SetPos(f_current_value, pwnd_dialog);
	}

	//*****************************************************************************************
	//
	virtual void SetPos
	(
		float f_current_value,	// Current value of the parameter.
		CWnd* pwnd_dialog = 0	// Containing dialog.
	) override
	//
	// Sets the scrollbar to the specified value.
	//
	//**********************************
	{
		Assert(f_current_value > 0.0f);

		// Get the scrollbar position.
		int i_position = int((fLog(f_current_value) - fMin) / fValuePerStep + 0.5f);

		// Set the scrollbar position.
		SetScrollPos(i_position);
		if (pwnd_dialog && iTextID)
			SetDlgItemFloat(pwnd_dialog, iTextID, f_current_value, 2);
	}

	//*****************************************************************************************
	//
	virtual float fGet
	(
		CWnd* pwnd_dialog = 0			// Containing dialog.
	) override
	//
	// Gets the value specified by the scrollbar.
	//
	//**********************************
	{
		float f_val = fExp((GetScrollPos()) * fValuePerStep + fMin);
		if (pwnd_dialog && iTextID)
			SetDlgItemFloat(pwnd_dialog, iTextID, f_val, 2);
		return f_val;
	}
};


#endif