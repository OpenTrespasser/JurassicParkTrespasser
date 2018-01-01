/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1997
 *
 * Contents: Geometry class for validation and conversion of objects into a format consistent
 * with the renderer.
 * 
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/Geometry.cpp                                          $
 * 
 * 16    7/15/97 6:58p Gstull
 * Made organizational changes to make management of the exporter more reasonable.
 * 
 * 15    7/14/97 3:36p Gstull
 * Made substantial changes to support tasks for the August 18th milestone for attributes,
 * object representation, geometry checking, dialog interfacing, logfile support, string tables
 * and string support.
 * 
 * 14    6/18/97 7:33p Gstull
 * Added changes to support fast exporting.
 * 
 * 13    4/15/97 10:38p Gstull
 * Added changes to support export options specified in a dialog box at export time.
 * 
 * 12    4/14/97 7:19p Gstull
 * Added changes to support log file management.
 * 
 * 11    3/19/97 6:05p Gstull
 * Added fixes for Bug #18 and bug #19, which resolved the problems of exporting objects
 * without geometry and material ID range errors.
 * 
 * 10    2/25/97 7:51p Gstull
 * Resolved issues with proper object placement in the scene and object validation.
 * 
 *********************************************************************************************/

#include <math.h>

#include "StandardTypes.hpp"
#include "ObjectDef.hpp"
#include "Geometry.hpp"
#include "Mathematics.hpp"

#include "Lib/Sys/SysLog.hpp"
#include "Lib/Sys/SmartBuffer.hpp"
#include "Lib/Groff/FileIO.hpp"
#include "Lib/Groff/Groff.hpp"
#include "Tools/GroffExp/GUIInterface.hpp"

extern CGUIInterface guiInterface;


const float TOO_CLOSE = 0.000001f;


//******************************************************************************************
//
// Floating point comparision function for fuzzy equality checks.
//
bool bTooClose(fvector3& fv3_vector_1, fvector3& fv3_vector_2)
{
	// Determine if these points are ~ equal.
	if (fabs(fv3_vector_1.X - fv3_vector_2.X) > TOO_CLOSE)
	{
		// No! These points are not equal.
		return false;
	}

	// Determine if these points are ~ equal.
	if (fabs(fv3_vector_1.Y - fv3_vector_2.Y) > TOO_CLOSE)
	{
		// No! These points are not ~ equal.
		return false;
	}

	// Determine if these points are essentially equal.
	if (fabs(fv3_vector_1.Z - fv3_vector_2.Z) > TOO_CLOSE)
	{
		// No! These points are not ~ equal.
		return false;
	}

	// These points are essentially equal.
	return true;
}


//******************************************************************************************
//
// Function for looping through the object list and processing each object.
//
bool CGeometry::bCheckScene(CObjectDefList* podl_scene, const CEasyString& estr_export_filename)
{
	bool		b_result = true;
	int			i_index = 0;
	CObjectDef*	pod_node = podl_scene->podHead;	
	
	// Open the log file.
	char str_logfile[256];

	// Determine whether the file should be active or not.
	if (guiInterface.bGenerateLogfiles())
	{
		// Construct the proper path for the logfile.
		guiInterface.BuildPath(str_logfile, guiInterface.strGetLogfileDirPath(), "Geometry.log");
		slLogfile.Open(str_logfile);

		// Activate the logfile.
		slLogfile.Enable();
	}
	else
	{
		// Deactivate the logfile.
		slLogfile.Disable();
	}

	// Place a startup message in the logfile.
	slLogfile.Printf("Processing scene file: %s\n", estr_export_filename.strData());

	// Process the scene list
	do
	{
		// Print a message to the log
		slLogfile.Printf("\n***** Processing object(%d): %s *****\n", i_index++, pod_node->estrObjectName.strData());

		// Reset the statistics.
		ResetStatistics();

		// Analyze the placement and geometric information and put it in normal form.
		b_result &= bCheckGeometry(pod_node);

		// Check for degenerate polygons.
		b_result &= bCheckFaces(pod_node);

		// Check for legal texture coordinates.
		b_result &= bCheckTexCoords(pod_node);

		// Make sure the material indices are valid.  Were we successful?
		b_result &= bCheckMaterials(pod_node);

		// Log the results.
		PrintStatistics();

		// Move to the next object in the scene list.
		pod_node = pod_node->podNext;

	} while (pod_node != 0);
	

	// Close the logfile.
	slLogfile.Close();

	// Return a successful result
	return b_result;
}


//*****************************************************************************************
//
// Function to validate the objects geometry.
//
bool CGeometry::bCheckGeometry
( 
	CObjectDef* pod_object
)
{
	//
	// Determine what coordinate system the objects geometry exists in.
	//

	//
	// Make sure the object is being uniformly scaled on an axis boundry.  If not, report this
	// condition as an error.
	//

	// Decompose the matrix.

	// Display the uniform scale value.
	slLogfile.Printf("Object found to be using uniform scaling: %f\n", pod_object->fGetScale());

	return true;
}


/*
{
	bool	b_result = true;
	uint	u_i;
	float	f_min_x =  100000000.0f;
	float	f_max_x = -100000000.0f;
	float	f_min_y =  100000000.0f;
	float	f_max_y = -100000000.0f;
	float	f_min_z =  100000000.0f;
	float	f_max_z = -100000000.0f;
//	float	f_delta_x;
//	float	f_delta_y;
//	float	f_delta_z;
	float	f_trans_x;
	float	f_trans_y;
	float	f_trans_z;
	float	f_scale = 1.0f;


	// Adjust the object position vector against the pivot offset.
	Point3 p3_position = pod_object->p3GetPosition();
	Point3 p3_pivot_offset = pod_object->p3GetPivotOffset();

	// Display the uniform scale value.
	slLogfile.Printf("Adjusting position by pivot offset: %f %f %f\n", p3_pivot_offset.x, p3_pivot_offset.y, p3_pivot_offset.z);

	// Subtract the pivot offset from the position.
	p3_position.x -= p3_pivot_offset.x;
	p3_position.y-= p3_pivot_offset.y;
	p3_position.z -= p3_pivot_offset.z;

	// Save the updated position.
	pod_object->Position(p3_position);

	// Loop through all the vertices
	for (u_i = 0; u_i < pod_object->uVertexCount; u_i++)
	{
		//
		// Adjust each point to reflect the center of mass.
		//

		// Get each vertex and adjust it against the pivot offset if it is non-zero.
		pod_object->afv3Vertex[u_i].X += p3_pivot_offset.x;
		pod_object->afv3Vertex[u_i].Y += p3_pivot_offset.y;
		pod_object->afv3Vertex[u_i].Z += p3_pivot_offset.z;

		//
		// Is this coordinate either the smallest we have seen?
		if (pod_object->afv3Vertex[u_i].X < f_min_x)
		{
			// Replace it
			f_min_x = pod_object->afv3Vertex[u_i].X;
		}

		// Is this coordinate either the largest we have seen?
		if (pod_object->afv3Vertex[u_i].X > f_max_x)
		{
			// Replace it
			f_max_x = pod_object->afv3Vertex[u_i].X;
		}

		// Is this coordinate either the smallest we have seen?
		if (pod_object->afv3Vertex[u_i].Y < f_min_y)
		{
			// Replace it
			f_min_y = pod_object->afv3Vertex[u_i].Y;
		}

		// Is this coordinate either the largest we have seen?
		if (pod_object->afv3Vertex[u_i].Y > f_max_y)
		{
			// Replace it
			f_max_y = pod_object->afv3Vertex[u_i].Y;
		}

		// Is this coordinate either the smallest we have seen?
		if (pod_object->afv3Vertex[u_i].Z < f_min_z)
		{
			// Replace it
			f_min_z = pod_object->afv3Vertex[u_i].Z;
		}

		// Is this coordinate either the largest we have seen?
		if (pod_object->afv3Vertex[u_i].Z > f_max_z)
		{
			// Replace it
			f_max_z = pod_object->afv3Vertex[u_i].Z;
		}
	}

	// Calculate the threshold for 
	// Is the origin of the object contained by it's extent?
	if (f_min_x <= 0.00001f && f_max_x >= 0.00001f && f_min_y <= 0.00001f && f_max_x >= 0.00001f && f_min_z <= 0.00001f && f_max_z >= 0.00001f)
	{
		slLogfile.Printf("No object recentering required.\n"); 

		//
		// Yes! Then only scaling of the object into unit space is needed.  Determine
		// this by finding the largest relative offset and using that value for the
		// scaling constant.
		//

		// Calculate the range of each of the coordinates on the respective axis
		float f_range = -f_min_x;

		if (f_max_x > f_min_x)
			f_range = f_max_x;
		if (-f_min_y > f_range)
			f_range = -f_min_y;
		if (f_max_y > f_range)
			f_range = f_max_y;
		if (-f_min_z > f_range)
			f_range = -f_min_z;
		if (f_max_z > f_range)
			f_range = f_max_z;

		f_trans_x = 0.0f;
		f_trans_y = 0.0f;
		f_trans_z = 0.0f;

		// Adjust the scale for the unit cube
		f_scale = 1.0f / f_range;

		// Log the results.
		slLogfile.Printf("Object space definition contains the origin.\n");
		slLogfile.Printf("Object space scaling factor is: %f\n", f_scale);
		slLogfile.Printf("Translation: x -> 0.0, y -> 0.0, z -> 0.0\n\n");

		// Loop through all the vertices, translate them to the object center of mass then
		// scale the object into the unit cube.
		for (u_i = 0; u_i < pod_object->uVertexCount; u_i++)
		{
			// Adjust the coordinates
			pod_object->afv3Vertex[u_i].X /= f_range;
			pod_object->afv3Vertex[u_i].Y /= f_range;
			pod_object->afv3Vertex[u_i].Z /= f_range;

			// Log the results
			slLogfile.Printf("World Vertex %3d: %f %f %f\n", u_i, 
				pod_object->afv3Vertex[u_i].X, 
				pod_object->afv3Vertex[u_i].Y, 
				pod_object->afv3Vertex[u_i].Z); 
		}

		// Place the object scale in the object so it can be restored.
		pod_object->Scale(pod_object->fGetScale() / f_scale);
	}
	else
	{
		//
		// No! The object needs to be recentered and scaled into the unit cube.
		//
		slLogfile.Printf("Recentering object since origin is not within object's extent.\n"); 

		// Calculate the range of each of the coordinates on the respective axis
		f_trans_x = ((f_min_x + f_max_x) / 2.0f);
		f_trans_y = ((f_min_y + f_max_y) / 2.0f);
		f_trans_z = ((f_min_z + f_max_z) / 2.0f);

		// Log the results.
		slLogfile.Printf("Object space definition does not contain the origin.\n");
//			slLogfile.Printf("Range values: x -> %f, y -> %f, z -> %f\n", f_delta_x, f_delta_y, f_delta_z);
		slLogfile.Printf("Translation: x -> %f, y -> %f, z -> %f\n\n", f_trans_x, f_trans_y, f_trans_z);
		slLogfile.Printf("Object space scaling factor is: %f\n", f_scale);

		// Loop through all the vertices, translate them to the object center of mass then
		// scale the object into the unit cube.
		for (u_i = 0; u_i < pod_object->uVertexCount; u_i++)
		{
			// Adjust the coordinates
			pod_object->afv3Vertex[u_i].X -= f_trans_x;
			pod_object->afv3Vertex[u_i].Y -= f_trans_y;
			pod_object->afv3Vertex[u_i].Z -= f_trans_z;

			// Log the results
			slLogfile.Printf("Vertex %3d: %7.2f, %7.2f, %7.2f\n", u_i, pod_object->afv3Vertex[u_i].X, pod_object->afv3Vertex[u_i].Y, 
				pod_object->afv3Vertex[u_i].Z); 
		}

		// Relocate the object to the origin by getting the previous position and updating it with respect to the origin.
		Point3 p3_old_position = pod_object->p3GetPosition();
		Point3 p3_new_position(p3_old_position.x+f_trans_x, p3_old_position.y+f_trans_y, p3_old_position.z+f_trans_z);

		// Update the position.
		pod_object->Position(p3_new_position);
	}
/*		else
	{
		//
		// No! The object needs to be recentered and scaled into the unit cube.
		//

		// Calculate the range of each of the coordinates on the respective axis
		f_delta_x = (f_max_x - f_min_x);
		f_delta_y = (f_max_y - f_min_y);
		f_delta_z = (f_max_z - f_min_z);

		f_trans_x = ((f_min_x + f_max_x) / 2.0f);
		f_trans_y = ((f_min_y + f_max_y) / 2.0f);
		f_trans_z = ((f_min_z + f_max_z) / 2.0f);

		// Find the largest delta.  Is this the largest?
		f_scale = f_delta_x;
		if (f_delta_y > f_scale)
		{
			// Replace it
			f_scale = f_delta_y;
		}

		// Find the largest delta.  Is this the largest?
		if (f_delta_z > f_scale)
		{
			// Replace it
			f_scale = f_delta_z;
		}

		// Adjust the scale for the unit cube
		f_scale /= 2.0f;

		// Log the results.
		slLogfile.Printf("Object space definition does not contain the origin.\n");
		slLogfile.Printf("Range values: x -> %f, y -> %f, z -> %f\n", f_delta_x, f_delta_y, f_delta_z);
		slLogfile.Printf("Translation: x -> %f, y -> %f, z -> %f\n\n", f_trans_x, f_trans_y, f_trans_z);
		slLogfile.Printf("Object space scaling factor is: %f\n", f_scale);

		// Find the largest range since we can only use uniform scaling.  Is this the largest?
		f_scale = f_delta_x;
		if (f_delta_y > f_scale)
		{
			// Replace it.
			f_scale = f_delta_y;
		}

		// Find the largest delta.  Is this the largest?
		if (f_delta_z > f_scale)
		{
			// Replace it
			f_scale = f_delta_z;
		}

		// Adjust the scale for the unit cube
		f_scale /= 2.0f;

		// Loop through all the vertices, translate them to the object center of mass then
		// scale the object into the unit cube.
		for (u_i = 0; u_i < pod_object->uVertexCount; u_i++)
		{
			// Adjust the coordinates
			pod_object->afv3Vertex[u_i].X = (pod_object->afv3Vertex[u_i].X - f_trans_x) / f_scale;
			pod_object->afv3Vertex[u_i].Y = (pod_object->afv3Vertex[u_i].Y - f_trans_y) / f_scale;
			pod_object->afv3Vertex[u_i].Z = (pod_object->afv3Vertex[u_i].Z - f_trans_z) / f_scale;

			// Log the results
	//		slLogfile.Printf("Vertex %3d: %7.2f, %7.2f, %7.2f\n", u_i, pod_object->afv3Vertex[u_i].X, pod_object->afv3Vertex[u_i].Y, 
	//			pod_object->afv3Vertex[u_i].Z); 
		}

		// Relocate the object to the origin.
		float f_old_x;
		float f_old_y;
		float f_old_z;

		// Get the previous position.
		pod_object->GetPosition(f_old_x, f_old_y, f_old_z);

		// Update the translation of the object to the newly centered position.
		pod_object->Position(f_old_x + (f_trans_x * pod_object->fGetScale()), 
							 f_old_y + (f_trans_y * pod_object->fGetScale()), 
							 f_old_z + (f_trans_z * pod_object->fGetScale()));

		// Place the object scale in the object so it can be restored.
		pod_object->Scale(f_scale * pod_object->fGetScale());

		// Loop through all the vertices, translate them to the object center of mass then
		// scale the object into the unit cube.
		for (u_i = 0; u_i < pod_object->uVertexCount; u_i++)
		{
			// Adjust the coordinates
			pod_object->afv3Vertex[u_i].X /= f_scale;
			pod_object->afv3Vertex[u_i].Y /= f_scale;
			pod_object->afv3Vertex[u_i].Z /= f_scale;

			// Log the results
			slLogfile.Printf("Vertex %3d: %7.2f, %7.2f, %7.2f\n", u_i, pod_object->afv3Vertex[u_i].X, pod_object->afv3Vertex[u_i].Y, 
				pod_object->afv3Vertex[u_i].Z); 
		}
	}

	slLogfile.Printf("\n");


	// Return the result to the caller.
	return b_result;
}
*/


//******************************************************************************************
//
// Function for validating the geometry of an object.
//
bool CGeometry::bCheckFaces(CObjectDef* pod_object)
{
	bool b_error = true;				// Success.

	//
	// Start out by checking all the faces for degenerate polygons, then verify that the
	// face normals are valid.  This will occur by checking the following:
	//

	// Loop through all the vertices
	for (uint u_i = 0; u_i < pod_object->uFaceCount; u_i++)
	{
		// All all the vertex indices in range?
		if (pod_object->auv3Face[u_i].X >= pod_object->uVertexCount ||
		    pod_object->auv3Face[u_i].Y >= pod_object->uVertexCount ||
			pod_object->auv3Face[u_i].Z >= pod_object->uVertexCount)
		{
			// Flag the error.
			b_error = false;

			// No! Log the error in the file.
			slLogfile.Printf("Error: One or more vertex indices is out of range (0..%d): %d %d %d\n", pod_object->uVertexCount-1,
				pod_object->auv3Face[u_i].X, pod_object->auv3Face[u_i].Y, pod_object->auv3Face[u_i].Z);
		}

		// Is the polygon degenerate due to vertex indices?
		if (pod_object->auv3Face[u_i].X == pod_object->auv3Face[u_i].Y ||
		    pod_object->auv3Face[u_i].Y == pod_object->auv3Face[u_i].Z ||
			pod_object->auv3Face[u_i].Z == pod_object->auv3Face[u_i].X)
		{
			// Flag the error.
			b_error = false;

			// No! Log the error in the file.
			slLogfile.Printf("Error: Degenerate polygon detected: Polygon (%d): %d %d %d\n", pod_object->uFaceCount,
				pod_object->auv3Face[u_i].X, pod_object->auv3Face[u_i].Y, pod_object->auv3Face[u_i].Z);
		}

		// Is the polygon degenerate due to vertices being to close together?
		fvector3 fv3_vertex_0 = pod_object->afv3Vertex[pod_object->auv3Face[u_i].X];
		fvector3 fv3_vertex_1 = pod_object->afv3Vertex[pod_object->auv3Face[u_i].Y];
		fvector3 fv3_vertex_2 = pod_object->afv3Vertex[pod_object->auv3Face[u_i].Z];

		// Is vertex 0 too close to vertex 1?
		if (bTooClose(fv3_vertex_0, fv3_vertex_1))
		{
			// Flag the error.
			b_error = false;

			// No! Log the error in the file.
			slLogfile.Printf("Error: Vertex proximity error: Polygon (%d): Vertices: 0:(%d) (%.3f %.3f %.3f), 1:(%d) (%.3f %.3f %.3f)\n",
				u_i, 
				pod_object->auv3Face[u_i].X, fv3_vertex_0.X, fv3_vertex_0.Y, fv3_vertex_0.Z, 
				pod_object->auv3Face[u_i].Y, fv3_vertex_1.X, fv3_vertex_1.Y, fv3_vertex_1.Z);
		}

		// Is vertex 1 too close to vertex 2?
		if (bTooClose(fv3_vertex_1, fv3_vertex_2)) 
		{
			// Flag the error.
			b_error = false;

			// No! Log the error in the file.
			slLogfile.Printf("Error: Vertex proximity error: Polygon (%d): Vertices: 1:(%d) (%.3f %.3f %.3f), 2:(%d) (%.3f %.3f %.3f)\n",
				u_i, 
				pod_object->auv3Face[u_i].Y, fv3_vertex_1.X, fv3_vertex_1.Y, fv3_vertex_1.Z, 
				pod_object->auv3Face[u_i].Z, fv3_vertex_2.X, fv3_vertex_2.Y, fv3_vertex_2.Z);
		}

		// Is vertex 0 too close to vertex 2?
		if (bTooClose(fv3_vertex_0, fv3_vertex_2))
		{
			// Flag the error.
			b_error = false;

			// No! Log the error in the file.
			slLogfile.Printf("Error: Vertex proximity error: Polygon (%d): Vertices: 0:(%d) (%.3f %.3f %.3f), 2:(%d) (%.3f %.3f %.3f)\n",
				u_i, 
				pod_object->auv3Face[u_i].X, fv3_vertex_0.X, fv3_vertex_0.Y, fv3_vertex_0.Z, 
				pod_object->auv3Face[u_i].Z, fv3_vertex_2.X, fv3_vertex_2.Y, fv3_vertex_2.Z);
		}

		// Is the face normal valid?
		if (fabs(pod_object->afv3FaceNormal[u_i].X) < TOO_CLOSE &&
			fabs(pod_object->afv3FaceNormal[u_i].Y) < TOO_CLOSE &&
			fabs(pod_object->afv3FaceNormal[u_i].Z) < TOO_CLOSE)
		{
			// Flag the error.
			b_error = false;

			// No! Log the error in the file.
			slLogfile.Printf("Error: Degenerate normal detected on polygon %d\n", u_i);
		}

		//
		// Are the vertex normals valid?
		//
		for (uint u_j = 0; u_j < 3; u_j++)
		{
			// Is the face normal valid?
			if (fabs(pod_object->afv3VertexNormal[u_i*3+u_j].X) < TOO_CLOSE &&
				fabs(pod_object->afv3VertexNormal[u_i*3+u_j].Y) < TOO_CLOSE &&
				fabs(pod_object->afv3VertexNormal[u_i*3+u_j].Z) < TOO_CLOSE)
			{
				// Flag the error.
				b_error = false;

				// No! Log the error in the file.
				slLogfile.Printf("Error: Degenerate vertex normal detected on polygon %d, vertex %d: (0.0, 0.0, 0.0)\n", u_i, u_j);
			}
		}
	}

	// Were any errors detected?
	if (!b_error)
	{
		char str_msg[256];
		// Yes!  Throw up a dialog box reporting that something went wrong.
		sprintf(str_msg, "Geometry errors detected when processing object: %s.  See Geometry.log for details.\n", 
			pod_object->estrObjectName.strData());

		guiInterface.bErrorMsg(str_msg);
	}


	// Return the result.
	return b_error;
}



//******************************************************************************************
//
// Function for checking the texture coordinate information of an object.
//
bool CGeometry::bCheckTexCoords(CObjectDef* pod_object)
{
	// Does this object have any textured materials?
	if (pod_object->uTextureVertexCount > 0)
	{
		// Loop through all the texture vertices and make sure it falls in [0..1]
		for (uint u_i = 0; u_i < pod_object->uTextureVertexCount; u_i++)
		{
			// Is the U coordinate < 0?
			if (pod_object->afv2TextureVertex[u_i].X < -0.000001f)
			{
				// Log this to the file
				slLogfile.Printf("Clamping u coordinate (%d) from %f to 0.0\n", u_i, pod_object->afv2TextureVertex[u_i].X);

				// Clamp to 0.0
				pod_object->afv2TextureVertex[u_i].X = 0.0f;

				// Increment the UV clamp counter
				iClampUVCoords++;
			}

			// Is the U coordinate > 1.0?
			if (pod_object->afv2TextureVertex[u_i].X > 1.000001f)
			{
				// Log this to the file
				slLogfile.Printf("Clamping u coordinate (%d) from %f to 1.0\n", u_i, pod_object->afv2TextureVertex[u_i].X);

				// Clamp to 1.0
				pod_object->afv2TextureVertex[u_i].X = 1.0f;

				// Increment the UV clamp counter
				iClampUVCoords++;
			}

			float y = (float) fabs(pod_object->afv2TextureVertex[u_i].Y);

			// Is the V coordinate < 0?
			if (pod_object->afv2TextureVertex[u_i].Y < -0.000001f)
			{
				// Log this to the file
				slLogfile.Printf("Clamping V coordinate (%d) from %f to 0.0\n", u_i, pod_object->afv2TextureVertex[u_i].Y);

				// Clamp to 0.0
				pod_object->afv2TextureVertex[u_i].Y = 0.0f;

				// Increment the UV clamp counter
				iClampUVCoords++;
			}

			// Is the V coordinate > 1.0?
			if (pod_object->afv2TextureVertex[u_i].Y > 1.000001f)
			{
				// Log this to the file
				slLogfile.Printf("Clamping V coordinate (%d) from %f to 1.0\n", u_i, pod_object->afv2TextureVertex[u_i].Y);

				// Clamp to 1.0
				pod_object->afv2TextureVertex[u_i].Y = 1.0f;

				// Increment the UV clamp counter
				iClampUVCoords++;
			}
		}
	}
	else
	{
		// No texture vertices.  This is definitely an error.
		return false;
	}

	// Were any errors detected?
	if (iClampUVCoords > 0)
	{	
		// Yes!  Throw up a dialog box reporting that something went wrong.
		slLogfile.Printf("Texture coordinate errors detected when processing: %s.  See Geometry.log for details.\n", 
			pod_object->estrObjectName.strData());
	}

	// Advance the log file.
	slLogfile.Printf("\n");


	// Return a successful result
	return true;
}


//******************************************************************************************
//
// Function for checking the material information associated with an object.
//
bool CGeometry::bCheckMaterials(CObjectDef* pod_object)
{
	char str_message[256];
	uint u_i;
	bool b_result = true;
	bool b_processing_error = false;
	uint u_material_count = pod_object->uMaterialCount;


	// Does this object have any materials?
	if (u_material_count > 0)
	{
		// Make sure all the material slots have at least a diffuse map.
		for (u_i = 0; u_i < u_material_count; u_i++)
		{
			// Does this slot have a material?
			if (pod_object->astrTextureMap[u_i] == 0)
			{
				// No! This is definitely an error so report it.
				sprintf(str_message, "Error: Object %s has %d materials and slot %d is empty.\n", 
					pod_object->estrObjectName.strData(), u_material_count, u_i);
				slLogfile.Printf(str_message);

				// Display the error in a dialog box to the user.
				guiInterface.bErrorMsg(str_message);

				// Signal the fact that there was an error.
				b_result = false;
			}
		}

		// Loop through all the texture vertices and make sure it falls in [0..1]
		for (u_i = 0; u_i < pod_object->uFaceCount; u_i++)
		{
			// Is the material ID in range?
			if (pod_object->auFaceMaterialIndex[u_i] >= u_material_count)
			{
				//
				// No!  Determine if this is a single material object since single material objects
				// can have multiple material ID's but only one material.
				//
				if (u_material_count == 1)
				{
					slLogfile.Printf("Single-material requires ID's to be clamped to 0 from (%d).\n", pod_object->auFaceMaterialIndex[u_i]);
					// Clamp the material ID to 0.
					pod_object->auFaceMaterialIndex[u_i] = 0;

					iClampMaterialIDs++;
				}
				else
				{
					sprintf(str_message, "Multi-material ID (%d) is out of range (0..%d).\n",  pod_object->auFaceMaterialIndex[u_i], u_material_count-1);

					// Set the error flag.
					b_result = false;
				}
			}
		}
	}
	else
	{
		//
		// There are no materials for this node, return an error.  At some point it would be nice if a 'default' material
		// representing a visual error message, could be mapped onto the object so it could be rendered flat.
		//
		slLogfile.Printf("Error: No materials are present for object: %s\n", pod_object->estrObjectName.strData());

		return false;
	}

	if (iClampMaterialIDs)
	{
		// Advance the log file.
		slLogfile.Printf("\n");
	}

	// Return the result
	return b_result;
}


//******************************************************************************************
//
// Function for reseting the export statistics.
//
void CGeometry::ResetStatistics()
{
	// Reset these before processing each object
	iDegenerateFaces  = 0;
	iRemovedVertices  = 0;
	iClampUVCoords    = 0;
	iClampMaterialIDs = 0;
}


//******************************************************************************************
//
// Function for printing the statistics.
//
void CGeometry::PrintStatistics()
{
	// Reset these before processing each object
	slLogfile.Printf("\nProcessing complete.\n");
	slLogfile.Printf("   Degenerate polygons    : %d\n", iDegenerateFaces);
	slLogfile.Printf("   Dangling vertices      : %d\n", iRemovedVertices);
	slLogfile.Printf("   Clamped UV coordinates : %d\n", iClampUVCoords);
	slLogfile.Printf("   Clamped Material ID's  : %d\n\n", iClampMaterialIDs);
}

