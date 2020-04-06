/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of GUIControls.hpp
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/GUIControls.cpp                                               $
 * 
 * 1     98/04/22 12:30 Speter
 * 
 * 1     98/04/06 10:44 Speter
 * 
 *********************************************************************************************/

#include "stdafx.h"
#include "guiapp.h"
#include "GUIControls.hpp"

//**********************************************************************************************
//
// class CConnection implementation.
//

	//******************************************************************************************
	void CConnection::Add(CController* pctrl)
	{
		lsControls.push_back(pctrl);
	}

	//******************************************************************************************
	void CConnection::DDX(CDataExchange* pDX)
	{
		for (std::list<CController*>::iterator itls = lsControls.begin(); itls != lsControls.end(); ++itls)
			(*itls)->DDX(pDX);
	}

	//******************************************************************************************
	void CConnection::Display()
	{
		for (std::list<CController*>::iterator itls = lsControls.begin(); itls != lsControls.end(); ++itls)
			(*itls)->Display();
	}

//**********************************************************************************************
//
// class CControlSpin<> implementation.
//

	//******************************************************************************************
	void CControlSpin<float>::Format(char str_text[]) const
	{
		sprintf(str_text, "%.*f", iPrecision, tGetVal());
	}

	//******************************************************************************************
	void CControlSpin<int>::Format(char str_text[]) const
	{
		// Override for ints.
		sprintf(str_text, "%d", tGetVal());
	}

