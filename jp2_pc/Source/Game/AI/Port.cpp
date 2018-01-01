/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents: A set of null functions to allow the AI lib to be used outside the AI test app.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/Port.cpp                                                     $
 * 
 * 3     8/29/97 5:58p Agrant
 * using port.hpp
 * 
 * 2     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 1     3/19/97 2:39p Agrant
 * Initial revision
 * 
 *********************************************************************************************/

#include "Common.hpp"

#include "Port.hpp"

class CSilhouette;
class CFeeling;

void DrawSilhouette(const CSilhouette*){}

void DrawFeeling(const CFeeling &, const CVector2<>&){}

void LineColour(int,int,int){}

bool IsMenuItemChecked(unsigned int){return false;}

void Line(CVector2<>, CVector2<>){}

