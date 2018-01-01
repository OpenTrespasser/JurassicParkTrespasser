/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of Gamma.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Gamma.cpp                                                    $
 * 
 * 7     8/28/98 1:09p Asouth
 * loop variable re-scoped; added 'class' keyword to explicit template instantiation
 * 
 * 6     98/03/30 11:03 Speter
 * Gamma table now based on CFloatTable.
 * 
 * 5     97/12/11 16:51 Speter
 * CGamma interface changed.  Input now ranges up to rvMAX_WHITE.
 * 
 * 4     6/25/97 7:25p Mlange
 * Now uses the (fast) float to integer conversion functions.
 * 
 * 3     97/06/23 20:22 Speter
 * Now save gamma parameters as member variables, for inspection.   Sped up table construction
 * by avoiding repeated multiplies and divides.  Explicitly instantiate template.  
 * 
 * 2     97/01/26 15:50 Speter
 * Changed Assert to allow exponent of 0.
 * 
 * 1     96/12/17 13:23 Speter
 * New module.
 * 
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Gamma.hpp"

#include <math.h>

//*********************************************************************************************
//
// CGammaCorrection implementation.
//

	//*****************************************************************************************
	template<class T> CGammaCorrection<T>::CGammaCorrection(T t_output_range, float f_gamma)
	{
		Set(t_output_range, f_gamma);
	}

	//*****************************************************************************************
	template<class T> void CGammaCorrection<T>::Set(T t_output_range, float f_gamma)
	{
		Assert(t_output_range > T(0));
		Assert(f_gamma >= 0.0);

		tOutputRange	= t_output_range;
		fGamma			= f_gamma;

		int i_max_shaded = iPosFloatCast(iSize() * rvMAX_COLOURED / rvMAX_WHITE);

		float f_output_scale = float(tOutputRange) * 0.999;
		float f_shade_scale = f_output_scale * rvMAX_COLOURED / rvMAX_WHITE;
		float f_shade_step = 1.0 / (i_max_shaded-1);

		//
		// Build the table.
		//

		// Fill portion of table using gamma correction.
		float f_lighting = 0;
		int i;
		for (i = 0; i < i_max_shaded; i++)
		{
			float f_gamma_light = pow(f_lighting, f_gamma);
			satTable[i] = T(f_gamma_light * f_shade_scale);
			f_lighting += f_shade_step;
		}

		// Fill linear (specular) portion of table.
		if (iGAMMA_TABLE_SIZE > i_max_shaded)
		{
			float f_output_step = f_output_scale / (iGAMMA_TABLE_SIZE - 1 - i_max_shaded);
			f_lighting = f_output_scale * rvMAX_COLOURED / rvMAX_WHITE;
			for (; i < iGAMMA_TABLE_SIZE; i++)
			{
				f_lighting += f_shade_step;
				satTable[i] = f_lighting;
			}
		}
	}

	// Instantiate the particular templates we need.
	template class CGammaCorrection<float>;

