//	Data for the care and feeding of dino models...
//	===============================================

#ifndef HEADER_LIB_PHYSICS_BIOSTRUCTURE_H
#define HEADER_LIB_PHYSICS_BIOSTRUCTURE_H

//	Parameters for Bipeds...
//	------------------------
struct Biped_Params
{

float	mass,
		hip_radius,
		hip_height,
		leg_length,
		neck_length,
		tail_length;
float	neck_offset[3];		// Offset of first neck joint from origin.
float	tail_offset[3];		// Offset of first tail joint from origin.
float	neck_dir[3];		// Direction of first-to-last neck joint.
};


//	Parameters for Quads...
//	-----------------------
struct Quad_Params: public Biped_Params
{
float	leg_offset,
		front_leg_length,
		front_hip_radius,
		front_hip_height;
};

#endif