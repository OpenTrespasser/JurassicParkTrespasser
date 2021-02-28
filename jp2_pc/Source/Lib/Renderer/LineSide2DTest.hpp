#pragma once

/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Functions to help debug LineSide2D functions.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:
 * 
 **********************************************************************************************/


//
// Required includes.
//
#include <math.h>
#include "Lib/Transform/Vector.hpp"
#include "Lib/Sys/Textout.hpp"

//
// Macros.
//

// Flag to indicate text file outputs should be used.
#define bOUTPUT_LINESIDE2D_DATA (0)

#if bOUTPUT_LINESIDE2D_DATA

	//******************************************************************************************
	//
	inline void PrintPos
	(
		const CVector2<>&  v2_centre,		// Point chosen as the centre of the set.
		const CVector2<>&  v2,				// Point to print angle and position of.
		CConsoleBuffer&    con,				// Console to write to.
		int                i = 0
	)
	//
	// Writes out a point's position and angle to the specified console.
	//
	//**************************************
	{
		float f_x = v2.tX - v2_centre.tX;
		float f_y = v2.tY - v2_centre.tY;
		float f_a = float(fmod(atan2(f_x, f_y) * 180.0 / 3.14159 + 720.0, 360.0));
		con.Print("%1.2f,%1.7f,%1.7f\n", f_a, v2.tX - v2_centre.tX, v2.tY - v2_centre.tY);
	}

	//******************************************************************************************
	//
	inline void VerifyRadialOrder
	(
		const CVector2<>&     v2_centre,	// Point chosen as the centre of the set.
		CPArray< CVector2<> > pav2,			// Array of points.
		CConsoleBuffer&       rcon			// Console to write to.
	)
	//
	// Writes out a point set to the specified console.
	//
	//**************************************
	{
		for (uint u = 0; u < pav2.uLen; ++u)
		{
			PrintPos(v2_centre, pav2[u], rcon, u);
		}
	}

	//******************************************************************************************
	//
	template<class T> void VerifyRadialOrder
	(
		const CVector2<>& v2_centre,		// Point chosen as the centre of the set.
		T&                cont,				// STL style container of points.
		CConsoleBuffer&   rcon				// Console to write to.
	)
	//
	// Writes out a point set to the specified console.
	//
	//**************************************
	{
		T::iterator it;	// Container iterator.

		// Write the angles.
		int i = 0;
		for (it = cont.begin(); ; ++i)
		{
			PrintPos(v2_centre, *it, rcon, i);
			++it;
			if (it == cont.end())
				break;
		}
	}

	//******************************************************************************************
	//
	template<class T> void VerifyRadialOrder
	(
		const CVector2<>&     v2_centre,	// Point chosen as the centre of the set.
		CPArray< CVector2<> > pav2,			// Array of points.
		T&                    cont,			// STL style container of points.
		const char*           str_filename	// File to write to.
	)
	//
	// Writes out a point set to the specified file.
	//
	//**************************************
	{
		T::iterator it;	// Container iterator.

		// Open a file to write to.
		CConsoleBuffer con(125, 80);
		con.OpenFileSession(str_filename);

		// Write the list.
		con.Print("\n\nList ***********************************************\n\n");
		VerifyRadialOrder(v2_centre, cont, con);

		// Separator.
		con.Print("\n\nArray **********************************************\n\n");

		// Write the array.
		VerifyRadialOrder(v2_centre, pav2, con);

		// End the file session.
		con.CloseFileSession();
	}

	//******************************************************************************************
	//
	template<class T> void VerifyRadialOrder
	(
		const CVector2<>&     v2_centre,	// Point chosen as the centre of the set.
		T&                    cont,			// STL style container of points.
		const char*           str_filename	// File to write to.
	)
	//
	// Writes out a point set to the specified file.
	//
	//**************************************
	{
		T::iterator it;	// Container iterator.

		// Open a file to write to.
		CConsoleBuffer con(125, 80);
		con.OpenFileSession(str_filename);

		// Write the list.
		con.Print("\n\nList ***********************************************\n\n");
		VerifyRadialOrder(v2_centre, cont, con);

		// End the file session.
		con.CloseFileSession();
	}

	//******************************************************************************************
	//
	inline void VerifyRadialOrder
	(
		const CVector2<>&     v2_centre,	// Point chosen as the centre of the set.
		CPArray< CVector2<> > pav2,			// Array of points.
		const char*           str_filename	// File to write to.
	)
	//
	// Writes out a point set to the specified file.
	//
	//**************************************
	{
		// Open a file to write to.
		CConsoleBuffer con(125, 80);
		con.OpenFileSession(str_filename);

		// Write the array.
		VerifyRadialOrder(v2_centre, pav2, con);

		// End the file session.
		con.CloseFileSession();
	}

#else // bOUTPUT_LINESIDE2D_DATA

	//******************************************************************************************
	//
	void PrintPos
	(
		const CVector2<>&  v2_centre,		// Point chosen as the centre of the set.
		const CVector2<>&  v2,				// Point to print angle and position of.
		CConsoleBuffer&    con,				// Console to write to.
		int                i = 0
	)
	//
	// Writes out a point's position and angle to the specified console.
	//
	//**************************************
	{
	}

	//******************************************************************************************
	//
	inline void VerifyRadialOrder
	(
		const CVector2<>&     v2_centre,	// Point chosen as the centre of the set.
		CPArray< CVector2<> > pav2,			// Array of points.
		CConsoleBuffer&       rcon			// Console to write to.
	)
	//
	// Writes out a point set to the specified console.
	//
	//**************************************
	{
	}

	//******************************************************************************************
	//
	template<class T> void VerifyRadialOrder
	(
		const CVector2<>& v2_centre,		// Point chosen as the centre of the set.
		T&                cont,				// STL style container of points.
		CConsoleBuffer&   rcon				// Console to write to.
	)
	//
	// Writes out a point set to the specified console.
	//
	//**************************************
	{
	}

	//******************************************************************************************
	//
	template<class T> void VerifyRadialOrder
	(
		const CVector2<>&     v2_centre,	// Point chosen as the centre of the set.
		CPArray< CVector2<> > pav2,			// Array of points.
		T&                    cont,			// STL style container of points.
		const char*           str_filename	// File to write to.
	)
	//
	// Writes out a point set to the specified file.
	//
	//**************************************
	{
	}

#endif // bOUTPUT_LINESIDE2D_DATA

//**********************************************************************************************
//**********************************************************************************************
//**********************************************************************************************
//
// For use in the event of emergencies.
//
//**********************************************************************************************
//**********************************************************************************************
//**********************************************************************************************
/*
class CRadial
{
public:
	float fAngle;
	CVector2<>* pv2Point;	// Pointer to the original vector.

	CRadial()
	{
	}

	CRadial(const CVector2<>& v2_centre, CVector2<>* pv2_pt)
	{
		pv2Point = pv2_pt;

		float f_x = pv2_pt->tX - v2_centre.tX;
		float f_y = pv2_pt->tY - v2_centre.tY;
		
		//fAngle = float(atan2(f_x, f_y));
		fAngle = float(fmod(atan2(f_x, f_y) * 180.0 / 3.14159 + 720.0, 360.0));
	}

	//**************************************************************************************
	//
	bool operator()(const CRadial& rad_0, const CRadial& rad_1) const
	//
	// Returns 'true' if the farthest (smallest) inverse Z value of the first polygon is
	// closer (larger) than the farthest inverse z value of the second polygon.
	//
	//**************************************
	{
		return rad_0.fAngle > rad_1.fAngle;
	}
};
*/
