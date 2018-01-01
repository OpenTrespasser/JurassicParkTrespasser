//	Boxy physics...
//	===============
#include <math.h>
#include <iostream.h>

//	State...
//	--------




//	-------------------------
//	Caution: Physics below...
//	-------------------------




//	Field parameters...
//	-------------------
const float	kappa = 100.0f,//50.0f,
			density = 1.0f,
			delta = .5f*sqrt(2.0f*density*kappa),
			mass_scale = 1.0f/density,
			field_g = 10.0f,
			
			BC_couple = 1000.0f,
			BC_damp = sqrt(2.0f*density*BC_couple),
			BC_coarseness = .05f,
			surface_mu = 50.0f;//5.0f;



