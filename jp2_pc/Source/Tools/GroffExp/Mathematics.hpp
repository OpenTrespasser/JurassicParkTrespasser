/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1996-1997
 *
 * Contents: File containing generic mathematical functions and output formatting routines.
 * 
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/Mathematics.hpp                                       $
 * 
 * 2     7/15/97 6:58p Gstull
 * Made organizational changes to make management of the exporter more reasonable.
 * 
 * 1     7/14/97 8:29p Gstull
 * Class to provide mathematical routines which support formatted output and general purpose
 * mathematical support.
 *
 *********************************************************************************************/

#ifndef HEADER_TOOLS_GROFFEXP_MATHEMATICS_HPP
#define HEADER_TOOLS_GROFFEXP_MATHEMATICS_HPP

#include "Max.h"

#include "Lib/Groff/EasyString.hpp"
#include "Lib/Sys/SysLog.hpp"
#include "GUIInterface.hpp"


// Define a threshold constant to be used for fuzzy comparing of values.
static const float fFUZZY_COMPARE_THRESHOLD = 0.00001f;

// Define an identity matrix.
static const Matrix3 m3Indetity(1);

// Define a zero point at the origin.
static const Point3 p3ZeroPoint(0, 0, 0);


//**********************************************************************************************
// Class to provide general purpose matrix routines and formatted output support.
// 
class CMathematicsUtil
//
// Prefix: mutil
//
{
	CEasyString		estr_indent;			// String to keep track of the indent formatting string.
	CEasyString		estr_newline;			// String to keep track of the newline formatting string.
	CEasyString		estr_str_newline;		// String to keep track of the string formatting string.
	CEasyString		estr_fpoint3_newline;	// String to keep track of the Point3 formatting string.
	CEasyString		estr_fvector3_newline;	// String to keep track of the fvector3 formatting string.
	
public:

	CGUIInterface&	guiInterface;			// Interface to the GUI environment.

	CSysLog&		slLogfile;				// Logfile to place output into.


	//******************************************************************************************
	//
	// Constructors and destructors.
	//

	//**********************************************************************************************
	//
	CMathematicsUtil
	(
		CGUIInterface& gui_interface,		// GUI environment interface class.
		CSysLog&	   sl_logfile			// Logfile interface class.
	);


	//******************************************************************************************
	//
	~CMathematicsUtil
	(
	)
	{
	};


	//******************************************************************************************
	//
	// Member functions.
	//


	//******************************************************************************************
	// Routine for determining whether a matrix is "equal" to identity.
	//
	bool b3x3IsIndentity
	(
		const Matrix3& m3_matrix
	);


	//******************************************************************************************
	// Routine for determining whether a matrix is "equal" to identity.
	//
	bool b4x3IsIndentity
	(
		const Matrix3& m3_matrix
	);


	//******************************************************************************************
	// Routine which compares two Point3 types for fuzzy equality.
	//
	bool bCompare
	(
		const Point3& p3_point_a, 
		const Point3& p3_point_b
	) const;

	
	//******************************************************************************************
	// Compare to Matrix3 types for fuzzy equality.
	//
	bool bCompare
	(
		const Matrix3& m3_matrix_a, 
		const Matrix3& m3_matrix_b
	) const;

	
	//******************************************************************************************
	// Routine for formatted indenting.
	//
	void Indent
	(
		int i_tabs
	);


	//******************************************************************************************
	// Routine for formatted newlines.
	//
	void DisplayNewline
	(
		int i_count=1
	);


	//******************************************************************************************
	// Routine for formatted strings.
	//
	void DisplayPrompt
	(
		const char* str_prompt,
		int			i_count=1
	);


	//******************************************************************************************
	// Routine for formatted points.
	//
	void DisplayPoint
	(
		const Point3& p3_point,
		int			  i_tabs=1
	);


	//******************************************************************************************
	// Routine for formatted vectors.
	//
	void DisplayVector
	(
		const Point3& p3_point,
		int			  i_tabs=1
	);
	

	//******************************************************************************************
	// Routine for formatted quaternions.
	//
	void DisplayQuat
	(
		const char* str_prompt, 
		const Quat&	q_quat,
		int			i_tabs=1
	);


	//******************************************************************************************
	// Routine for formatted matrices.
	//
	void DisplayMatrix
	(
		const char*	   str_prompt, 
		const Matrix3& m3_matrix,
		int			   i_tabs=1
	);


	//******************************************************************************************
	// Routine for displaying bounding boxes.
	//
	void DisplayBox
	(
		const char*	str_prompt, 
		const Box3&	bx3_box,
		int			i_tabs=1
	);


	//******************************************************************************************
	// Routine for decomposing and displaying matrix information.
	//
	void DisplayDecomposedMatrix
	(
		const char*	   str_prompt, 
		const Matrix3& m3_matrix,
		int			   i_tabs=1
	);
};

#endif