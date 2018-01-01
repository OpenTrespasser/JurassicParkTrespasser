/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	   Dummy function decls for compatability with AI test.
 *		
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/Port.hpp                                                      $
 * 
 * 1     8/29/97 5:57p Agrant
 * initial rev
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_PORT_HPP
#define HEADER_GAME_AI_PORT_HPP

// Forward declarations for AI Test functions that have dummy implementations in the library.
#include "Lib/Transform/Vector.hpp"

class CSilhouette;
class CFeeling;

void DrawSilhouette(const CSilhouette*);

void DrawFeeling(const CFeeling &, const CVector2<>&);

void LineColour(int,int,int);

bool IsMenuItemChecked(unsigned int);

void Line(CVector2<>, CVector2<>);




#endif
