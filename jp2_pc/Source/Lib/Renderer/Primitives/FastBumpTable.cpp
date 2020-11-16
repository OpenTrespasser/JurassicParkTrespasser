/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of 'FastBumpTable.hpp.'
 *
 * To do:
 *		Add filters for preprocessing bumpmaps.
 *		Use curve fitting for height-field to bumpmap conversion.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/FastBumpTable.cpp                             $
 * 
 * 35    10/03/98 4:40a Rwyatt
 * Disabled translation primitive in bsetupBump()
 * 
 * 34    8/05/98 2:24p Pkeet
 * Fixed crash bug.
 * 
 * 33    98/07/22 14:43 Speter
 * Use new CMaterial and CClut functions for bump table. MakeTranslationTable algorithm
 * simplified, doesn't rely on old clut implementation.
 * 
 * 32    98/03/13 14:05 Speter
 * Removed bump-quantize test stuff.
 * 
 * 31    98/02/10 12:58 Speter
 * CBumpTable now requires just a CClu for conversions, not a CClut or CPalClut.
 * 
 * 30    1/19/98 7:30p Pkeet
 * Added support for 16 bit bumpmaps by adding lower colour resolution and a smaller size for
 * 'CBumpAnglePair.'
 * 
 * 29    97/10/12 20:39 Speter
 * Bump tables are now tailored to particular materials and cluts.  Translated bump mapping is
 * now used only for lighting changes.  Changed CBumpLookup to CBumpTable.  Now just one routine
 * is required for bump setup: bSetupBump.  Added Update() to force update.  Removed
 * au1BumpToCos primary table; now new primary table is calculated for each material.  Replaced
 * au1CosToIntensity with pau1IntensityTrans.  Removed au1BumpFirst.
 * 
 * 28    97/09/18 21:09 Speter
 * Bump-mapping now works with iPHI_B_BITS and iPHI_L_BITS of different resolution.  Use new
 * fGetPhiL() function.  Fixed specular code re light size.
 * 
 * 27    97/09/16 15:36 Speter
 * Incorporated simper SBumpLighting structure in bump-map setup.  Specular folding is now done
 * in lighting code.  Removed old code for separate specular bump-mapping.  Changed SetupBump to
 * return only 'translate' flag.  Added temporary iBumpQuantizeMask, for bump resolution
 * testing.
 * 
 * 26    97/09/11 17:38 Speter
 * Bump lighting ranges are now subject to fCombine() operation in here, rather than requiring
 * it be done outside.
 * 
 * 25    97/08/05 13:33 Speter
 * Added bSPECULAR_POWER flag (disabled).
 * 
 * 24    97/06/23 20:33 Speter
 * Gamma correction is now incorporated into bump-mapping.
 * 
 * 23    6/16/97 3:00p Mlange
 * Updated for fast float to integer conversion functions' name change.
 * 
 * 22    97-04-23 17:53 Speter
 * Fixed stupid bug.  Commented out old translation functions.
 * 
 * 21    97-04-23 14:40 Speter
 * Moved all bump lookup table setup into CBumpLookup::SetupBump(), from ScreenRenderDWI.  Now
 * fold specular lighting into translated bump-mapping, eliminating separate, slow specular
 * primitive.
 * Moved bangLight and bangSpecular variables here from MapT.hpp.
 * Removed code in disabled bREFLECTION_PRIMARY section.
 * 
 **********************************************************************************************/


//
// Includes.
//
#include "gblinc/common.hpp"
#include "FastBumpTable.hpp"

#include "Lib/W95/WinInclude.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include <math.h>

//
// Definitions.
//
#define fMAX_COSINE (float(iMAX_NUM_INTENSITIES) - 0.05f)


//
// Constants.
//

#define bMODULATE_SPECULAR	0
#define bSPECULAR_POWER		0

//*********************************************************************************************
//
// CBumpTable functions.
//

	//*********************************************************************************************
	//
	void FillBumpTable
	(
		uint8* au1_bump,				// Table to fill.
		const SBumpLighting& blt,		// Lighting conditions.
		const CClu& clu					// Output clu.
	)
	//
	// Fills the au1_bump table according to the parameters.
	//
	// Notes:
	//		For every combination of angles, the cosine between the bump and light angles is calculated.
	//		This is passed through the T functor, then converted to an integer and stored in the table.
	//
	//**********************************
	{
		Assert(au1_bump);

		dprintf("Making bump angle table..\n");

		CBumpAnglePair bang;			// Bumpmap lighting value.

		for (int i_phi_l = iMIN_PHI_L; i_phi_l <= iMAX_PHI_L; i_phi_l++)
		{
			// Set the phi for the light.
			bang.SetPhiL(i_phi_l);

			// Initialize phi for the light.
			float f_phi_l = bang.fGetPhiL();

			//
			// Get the sine and cosine of the light's phi value and store for use by all
			// the inner loops.
			//
			float f_sin_phi_l = sin(f_phi_l);
			float f_cos_phi_l = cos(f_phi_l);

			for (int i_phi_b = 0; i_phi_b <= iMAX_PHI_B; i_phi_b++)
			{
				bang.SetPhiB(i_phi_b);

				float f_phi_b   = bang.fGetPhiB();
				float f_cos_phi = f_cos_phi_l * cos(f_phi_b);
				float f_sin_phi = f_sin_phi_l * sin(f_phi_b);

				for (int i_theta = 0; i_theta <= (1 << (iTHETA_BITS-1)); i_theta++)
				{
					bang.SetTheta(uint(i_theta));

					//
					// Get the sine and cosine of the light's theta and store for use by all the inner
					// loops.
					//
					float f_theta_b     = bang.fGetTheta();
					float f_cos_theta_b = cos(f_theta_b);

					//
					// Obtain the dot product between the normals described by the two angle
					// pairs. This corresponds to the formula:
					//
					//		dot product = cos(Tb) * cos(Pb) * cos(Pl) + sin(Pb) * sin(Pl)
					//
					float f_dot = f_cos_theta_b * f_cos_phi + f_sin_phi;

					//
					// Statement for verifying the accuracy of this method of building the
					// table. Note that this statement is too slow even for debug mode, so
					// it was run once and then commented out.
					//
					//Assert(Fuzzy(f_dot) == bang.dir3MakeNormal() * b.dir3MakeNormal());

					// Clamp dot product values.
					Assert(bWithin(f_dot, -1.0, 1.0));
					SetMinMax(f_dot, 0.0f, 1.0f);

					// Set table entry.
					TLightVal lv;
					if (clu.pmatMaterial->bRefractive)
						lv = clu.pmatMaterial->fReflected(f_dot);
					else
						lv = blt.lvAmbient + f_dot * blt.lvStrength;

					// Get composite reflection value from material.
					TReflectVal rv = clu.pmatMaterial->rvSingle(lv);

					// Convert to integer clut index.
					int i_output = iPosFloatCast(clu.cvFromReflect(rv));

					Assert(bang.br < iBUMP_TABLE_SIZE);
					au1_bump[bang.br] = i_output;

					// Set the same value for negative theta.
					bang.SetTheta(uint(-i_theta));
					Assert(bang.br < iBUMP_TABLE_SIZE);
					au1_bump[bang.br] = i_output;
				}
			}
		}
	}

//*********************************************************************************************
//
// CBumpTable implementation.
//

	CBumpAnglePair CBumpTable::bangLight;

	//*********************************************************************************************
	CBumpTable::CBumpTable(const CClu* pclu)
		: pcluCLU(pclu)
	{
		// Initialise tracking vars to invalid values, so tables are always built first time.
		bltMain.lvStrength = -1;
		bltTrans.lvStrength = -1;

		Assert(pcluCLU);
		fLightTolerance = (pcluCLU) ? (1.0 / pcluCLU->iNumRampValues) : ( 1.0f / 32.0f);
	}

	//*********************************************************************************************
	void CBumpTable::MakeTranslationTable(const SBumpLighting& blt)
	{
		if (!pau1IntensityTrans)
			pau1IntensityTrans = CPArray<uint8>(pcluCLU->iNumRampValues);

		// Pretty simple scheme. Just linearly translate the clut values as
		// the light ranges transform. Add 0.5 for automatic rounding.
		TClutVal cv_max = pcluCLU->iNumRampValues;
		CTransLinear<> tlr_clut
		(
			bltMain.lvAmbient * cv_max, bltMain.lvStrength * cv_max, 
			blt.lvAmbient * cv_max + 0.5, blt.lvStrength * cv_max
		);
	
		// Translate'em.
		for (int i = 0; i < pcluCLU->iNumRampValues; i++)
		{
			int i_new = iFloatCast((float)i * tlr_clut);
			pau1IntensityTrans[i] = MinMax(i_new, 0, pcluCLU->iNumRampValues-1);
		}
	}

	//*********************************************************************************************
	bool CBumpTable::bSetupBump(const SBumpLighting& blt_main, const SBumpLighting& blt_poly)
	{
		bangLight.SetBump(blt_poly.d3Light, true);

		Assert(bWithin(blt_main.lvAmbient, 0, 1));
		Assert(Fuzzy(blt_main.lvStrength).bWithin(0, 1 - blt_main.lvAmbient));

		if (Fuzzy((float)bltMain.lvAmbient, fLightTolerance) != blt_main.lvAmbient ||
			Fuzzy((float)bltMain.lvStrength, fLightTolerance) != blt_main.lvStrength)
		{
			// Fill the bumpmap table.
			bltMain = blt_main;
			FillBumpTable(au1BumpToIntensity, bltMain, *pcluCLU);
		}

		Assert(bWithin(blt_poly.lvAmbient, 0, 1));
		Assert(Fuzzy(blt_poly.lvStrength).bWithin(0, 1 - blt_poly.lvAmbient));

		//
		// Assert that we are not going to try to use the translated bump maps as the translation
		// code is commented out below!
		//
		Assert ((Fuzzy((float)bltMain.lvAmbient, fLightTolerance) != blt_poly.lvAmbient ||
			Fuzzy((float)bltMain.lvStrength, fLightTolerance) != blt_poly.lvStrength) == 0);

		// Use translated bump-mapping if the material or lighting differ from defaults.
/*		if (Fuzzy((float)bltMain.lvAmbient, fLightTolerance) != blt_poly.lvAmbient ||
			Fuzzy((float)bltMain.lvStrength, fLightTolerance) != blt_poly.lvStrength)
		{
			// The bump data for this triangle is different than the primary data.
			// We must use slow table bumping.
			// Compare with current params, and return if equivalent.
			if (Fuzzy((float)bltTrans.lvAmbient,  fLightTolerance) != blt_poly.lvAmbient ||
				Fuzzy((float)bltTrans.lvStrength, fLightTolerance) != blt_poly.lvStrength)
			{
				// Rebuild the translation table.
				bltTrans = blt_poly;
				MakeTranslationTable(bltTrans);
			}
			return true;
		}
		else
			return false;*/

		return false;
	}

	//*********************************************************************************************
	void CBumpTable::Update()
	{
		// Initialise tracking vars to invalid values, so tables are built next time.
		bltMain.lvStrength = -1;
		bltTrans.lvStrength = -1;
	}