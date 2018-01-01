/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1997
 *
 * Contents: Implementation of class CMathematicsUtil.
 * 
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/Mathematics.cpp                                       $
 * 
 * 3     7/15/97 6:58p Gstull
 * Made organizational changes to make management of the exporter more reasonable.
 * 
 * 2     7/14/97 8:30p Gstull
 * Initial verison of the CMathematicalUtil class.
 * 
 * 1     7/14/97 3:55p Gstull
 * File to contain general purpose mathematical routines which are useful.
 * 
 * 
 *********************************************************************************************/

#include "groffexp.h"

#include "Max.h"
#include "decomp.h"

#include "Mathematics.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "Lib/Sys/SysLog.hpp"
#include "GUIInterface.hpp"


//**********************************************************************************************
//
CMathematicsUtil::CMathematicsUtil
(
	CGUIInterface& gui_interface,							// GUI environment interface class.
	CSysLog&	   sl_logfile								// Logfile interface class.
) : guiInterface(gui_interface), slLogfile(sl_logfile)
{
	// Setup local copies of commonly used formatting strings.
	estr_indent			  = guiInterface.strGetString(IDS_INDENT);
	estr_newline		  = guiInterface.strGetString(IDS_NEWLINE);
	estr_str_newline	  = guiInterface.strGetString(IDS_STR_CRLF);
	estr_fpoint3_newline  = guiInterface.strGetString(IDS_PT3_CRLF);
	estr_fvector3_newline = guiInterface.strGetString(IDS_FV3_CRLF);
}


//**********************************************************************************************
// Routine for determining whether a 3 x 3 matrix is "equal" to identity.
//
bool CMathematicsUtil::b3x3IsIndentity
(
	const Matrix3& m3_matrix
)
{
	// For each row of both matrices ...
	for (int i_i = 0; i_i < 3; i_i++)
	{
		// get a row ...
		Point3 p3_row = m3_matrix.GetRow(i_i);

		// then check each column for a match.
		for (int i_j = 0; i_j < 3; i_j++)
		{
			// Are we on the diagonal?
			float f_value = (float) (i_i == i_j);

			// Is the value within rangeYes! Is this value within the threshold?
			if (fabs(p3_row[i_j] - f_value) > fFUZZY_COMPARE_THRESHOLD)
			{
				// No! Return failure.
				return false;
			}
		}
	}

	// This matrix contains identity in the 3x3 submatrix.
	return true;
}


//**********************************************************************************************
// Routine for determining whether a 4 x 3 matrix is "equal" to identity.
//
bool CMathematicsUtil::b4x3IsIndentity
(
	const Matrix3& m3_matrix
)
{
	// For each row of both matrices ...
	for (int i_i = 0; i_i < 3; i_i++)
	{
		// get a row ...
		Point3 p3_row = m3_matrix.GetRow(i_i);

		// then check each column for a match.
		for (int i_j = 0; i_j < 3; i_j++)
		{
			// Are we on the diagonal?
			float f_value = (float) (i_i == i_j);

			// Is the value within rangeYes! Is this value within the threshold?
			if (fabs(p3_row[i_j] - f_value) > fFUZZY_COMPARE_THRESHOLD)
			{
				// No! Return failure.
				return false;
			}
		}
	}

	// Get the position vector.
	Point3 p3_point = m3_matrix.GetRow(3);

	// Is the translation vector equal to the origin?
	return bCompare(p3_point, p3ZeroPoint);
}


//**********************************************************************************************
// Compare to Matrix3 types for fuzzy equality.
//
bool CMathematicsUtil::bCompare
(
	const Point3& p3_point_a, 
	const Point3& p3_point_b
) const
{
	// then check each column for a match.
	for (int i_i = 0; i_i < 3; i_i++)
	{
		// Are these two matrices equal (fuzzy)?
		if (fabs(p3_point_a[i_i] - p3_point_b[i_i]) > fFUZZY_COMPARE_THRESHOLD)
		{
			// No! Return failure.
			return false;
		}
	}

	// They are 'equal', so return true.
	return true;
}


//**********************************************************************************************
// Compare to Matrix3 types for fuzzy equality.
//
bool CMathematicsUtil::bCompare
(
	const Matrix3& m3_matrix_a, 
	const Matrix3& m3_matrix_b
) const
{
	// For each row of both matrices ...
	for (int i_i = 0; i_i < 4; i_i++)
	{
		// get a row ...
		Point3 p3_point_a = m3_matrix_a.GetRow(i_i);
		Point3 p3_point_b = m3_matrix_b.GetRow(i_i);

		// then check each column for a match.
		for (int i_j = 0; i_j < 3; i_j++)
		{
			// Are these two matrices equal (fuzzy)?
			if (fabs(p3_point_a[i_j] - p3_point_b[i_j]) > fFUZZY_COMPARE_THRESHOLD)
			{
				// No! Return failure.
				return false;
			}
		}
	}

	// They are 'equal', so return true.
	return true;
}


//**********************************************************************************************
//
void CMathematicsUtil::Indent(int i_tabs)
{
	for (int i = 0; i < i_tabs; i++)
		slLogfile.Printf(estr_indent.strData());
}


//**********************************************************************************************
//
void CMathematicsUtil::DisplayNewline(int i_count)
{
	while(i_count--)
	{
		slLogfile.Printf(estr_newline.strData());
	}
}


//**********************************************************************************************
//
void CMathematicsUtil::DisplayPrompt(const char* str_prompt, int i_tabs)
{
	Indent(i_tabs);
	slLogfile.Printf(estr_str_newline.strData(), str_prompt);
}


//**********************************************************************************************
//
void CMathematicsUtil::DisplayPoint(const Point3& p3_point, int i_tabs)
{
	Indent(i_tabs);
	slLogfile.Printf(estr_fpoint3_newline.strData(), p3_point[0], p3_point[1], p3_point[2]);
} 


//**********************************************************************************************
//
void CMathematicsUtil::DisplayVector(const Point3& p3_point, int i_tabs)
{
	Indent(i_tabs);
	slLogfile.Printf(estr_fvector3_newline.strData(), p3_point[0], p3_point[1], p3_point[2]);
}


//**********************************************************************************************
//
void CMathematicsUtil::DisplayQuat(const char* str_prompt, const Quat& q_quat, int i_tabs)
{
	// Display the quaternion in mathematical, as opposed to graphics notation.
	DisplayPrompt(str_prompt, i_tabs);
	
	Indent(i_tabs+1);
	slLogfile.Printf(guiInterface.strGetString(IDS_QUAT), q_quat[3], q_quat[0], q_quat[1], q_quat[2]);

	// Display the quaternion in angle/axis format.
	Point3 p3_axis;
	float  f_angle;

	// Convert the quaternion into angle-axis form.
	AngAxisFromQ(q_quat, &f_angle, p3_axis);

	// Display the quaternion in angle-axis form.
	Indent(i_tabs+1);
	slLogfile.Printf(guiInterface.strGetString(IDS_ANGLE_AXIS), f_angle * 180.0/3.1415926, 
		p3_axis[0], p3_axis[1], p3_axis[2]);
}


//**********************************************************************************************
//
void CMathematicsUtil::DisplayMatrix(const char* str_prompt, const Matrix3& m3_matrix, int i_tabs)
{
	DisplayNewline();

	// Was a custom user prompt supplied?
	if (str_prompt)
	{
		// Yes! Then display it.
		DisplayPrompt(str_prompt, i_tabs);
	}

	// Get the rows from the matrix and print them out.
	DisplayVector((Point3) m3_matrix.GetRow(0), i_tabs+1);
	DisplayVector((Point3) m3_matrix.GetRow(1), i_tabs+1);
	DisplayVector((Point3) m3_matrix.GetRow(2), i_tabs+1);
	DisplayPrompt(guiInterface.strGetString(IDS_MAT_SEPARATOR), i_tabs+1);
	DisplayVector((Point3) m3_matrix.GetRow(3), i_tabs+1);
}


//**********************************************************************************************
//
void CMathematicsUtil::DisplayBox(const char* str_prompt, const Box3& bx3_box, int i_tabs)
{
	DisplayNewline();

	// Was a custom user prompt supplied?
	if (str_prompt)
	{
		// Yes! Then display it.
		DisplayPrompt(str_prompt, i_tabs);
	}

	// Display the title banner.
	DisplayPrompt(guiInterface.strGetString(IDS_BBOX_TITLE), i_tabs);

	// Get the lower, upper and center coordinate of the bounding box and display it.
	Point3 p3_min = bx3_box.Min();	
	DisplayPrompt(guiInterface.strGetString(IDS_BBOX_LOWER_COORD), i_tabs);
	DisplayVector(p3_min, i_tabs+1);
	
	Point3 p3_max = bx3_box.Max();
	DisplayPrompt(guiInterface.strGetString(IDS_BBOX_UPPER_COORD), i_tabs);
	DisplayVector(p3_max, i_tabs+1);

	Point3 p3_center = bx3_box.Center();
	DisplayPrompt(guiInterface.strGetString(IDS_BBOX_CENTER_COORD), i_tabs);
	DisplayVector(p3_center, i_tabs+1);
}


//**********************************************************************************************
//
void CMathematicsUtil::DisplayDecomposedMatrix(const char* str_prompt, const Matrix3& m3_matrix, int i_tabs)
{
	AffineParts parts;
	decomp_affine(m3_matrix, &parts);

	DisplayNewline();

	// Was a custom user prompt supplied?
	if (str_prompt)
	{
		// Yes! Then display it.
		DisplayPrompt(str_prompt, i_tabs);
	}

	// Display the title banner.
	DisplayPrompt(guiInterface.strGetString(IDS_DECOMP_TITLE1), i_tabs+1);

	// Display the translation component.
	DisplayPrompt(guiInterface.strGetString(IDS_TRANSLATION), i_tabs+1);
	DisplayVector(parts.t, i_tabs+2);

	// Display the rotation component.
	DisplayNewline();
	DisplayQuat(guiInterface.strGetString(IDS_ROTATION), parts.q, i_tabs+1);

	// Display the scaling axis.
	DisplayNewline();
	DisplayQuat(guiInterface.strGetString(IDS_SCALE_ROTATION), parts.u, i_tabs+1);

	// Display the scaling components.
	DisplayNewline();
	DisplayPrompt(guiInterface.strGetString(IDS_SCALE_FACTOR), i_tabs+1);
	DisplayVector(parts.k, i_tabs+2);

	// Display the determinant sign.
	DisplayNewline();
	Indent(i_tabs+1);
	slLogfile.Printf(guiInterface.strGetString(IDS_DETERMINANT_SIGN), parts.f);
	
	// Display the end of the banner.
	DisplayPrompt(guiInterface.strGetString(IDS_DECOMP_TITLE2), i_tabs+1);
}
