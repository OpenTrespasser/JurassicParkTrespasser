/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of CPalLookup.hpp
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/PalLookup.cpp                                                $
 * 
 * 2     1/08/97 7:14p Pkeet
 * Replaced the NonConst macro with a CColour& cast in the colour matching code.
 * 
 * 1     96/12/31 17:32 Speter
 * New file containing code moved from Clut.cpp.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "PalLookup.hpp"

#include <memory.h>

//******************************************************************************************
//
// CPaletteLookup Implementation.
//

	//******************************************************************************************
	CPaletteLookup::CPaletteLookup(CPal *ppal_target)
	{
		uint32 u4_index;
		TListElem le_cindex;
		TListElem* ale_list_lut;

		ppalTarget = ppal_target ;

		// Clear all pointers to NULL.
		memset(aleRList, 0, sizeof(aleRList));

		for (u4_index = 0; u4_index < ppalTarget->aclrPalette.uLen; u4_index++)
		{

			// Do R part.

			// Get R value.
			le_cindex = ppalTarget->aclrPalette[u4_index].u1Red;

			// Check to see if already allocated.
			if (aleRList[le_cindex] == NULL)
			{
				aleRList[le_cindex] = new TListElem[MAX_LIST];
				
				memset(aleRList[le_cindex] , NULL, MAX_LIST * sizeof(TListElem));
			}
			ale_list_lut = aleRList[le_cindex];

			if (ale_list_lut[0] < MAX_LIST - 2)
			{
				ale_list_lut[ale_list_lut[0] + 1] = u4_index;
				ale_list_lut[0]++;
			} else {
				Assert(0);
			}

			// Do G part.

			// Get G value.
			le_cindex = ppalTarget->aclrPalette[u4_index].u1Green;

			// Check to see if already allocated.
			if (aleGlist(le_cindex) == NULL)
			{
				aleGlist(le_cindex) = new TListElem[MAX_LIST];
				
				memset(aleGlist(le_cindex) , NULL, MAX_LIST * sizeof(TListElem));
			}
			ale_list_lut = aleGlist(le_cindex);

			if (ale_list_lut[0] < MAX_LIST - 2)
			{
				ale_list_lut[ale_list_lut[0] + 1] = u4_index;
				ale_list_lut[0]++;
			}
			else
			{
	  			Assert(0);
			}

			// Do u1BB part.

			// Get u1BB value.
			le_cindex = ppalTarget->aclrPalette[u4_index].u1Blue;

			// Check to see if already allocated.
			if (aleBList(le_cindex) == NULL)
			{
				aleBList(le_cindex) = new TListElem[MAX_LIST];
				
				memset(aleBList(le_cindex) , NULL, MAX_LIST * sizeof(TListElem));
			}
			ale_list_lut = aleBList(le_cindex);

			if (ale_list_lut[0] < MAX_LIST - 2)
			{
				ale_list_lut[ale_list_lut[0] + 1] = u4_index;
				ale_list_lut[0]++;
			} 
			else
			{
				Assert(0);
			}
		}
	}

	// Static variables used for quick communication.
	static uint8 u1RR, u1GG, u1BB;

	//******************************************************************************************
	//
	inline uint32 CPaletteLookup::u4RGBDiff
	(
		uint32 u4_index					//  CLUT index to compare.
	) const
	//
	// Returns:
	//  A difference in colour space between passed and preset colours.
	//
	//**********************************
	{
		register long i4_calc, i4_calc2, i4_r1, i4_g1, i4_b1;
		register long i4_my_diff, i4_my_diff1, i4_my_diff2, i4_my_diff3;

	#define iRED_B   299
	#define iGREEN_B 587
	#define iBLUE_B  114

		i4_r1 = ppalTarget->aclrPalette[u4_index].u1Red;
		i4_g1 = ppalTarget->aclrPalette[u4_index].u1Green;
		i4_b1 = ppalTarget->aclrPalette[u4_index].u1Blue;

		i4_calc  = ((iRED_B * i4_r1) + (iGREEN_B * i4_g1) + (iBLUE_B * i4_b1)) / 10;
		i4_calc2 = ((iRED_B * u1RR ) + (iGREEN_B * u1GG ) + (iBLUE_B * u1BB )) / 10;

		i4_my_diff  = ((i4_r1 - u1RR) * (i4_r1 - u1RR));
		i4_my_diff1 = ((i4_g1 - u1GG) * (i4_g1 - u1GG));
		i4_my_diff2 = ((i4_b1 - u1BB) * (i4_b1 - u1BB));

		i4_my_diff3 = ((i4_calc - i4_calc2) * (i4_calc - i4_calc2)) / 10000;

		return(uint32)((i4_my_diff3 + i4_my_diff + i4_my_diff1 + i4_my_diff2));
	}

	//******************************************************************************************
	uint8 CPaletteLookup::u1MatchColour(CColour clr_match) const
	{
		TListElem le_tempnum;
		uint16	u2_found;
		uint32	u4_index, u4_overflow;
		uint16	u2_pass;
		uint16	au2_found_before[256];
		uint32	u4_best_so_far;
		uint32	u4_difference;
		uint	u_best_index;

		// Assume that certian colors are always in the palette.
		if (clr_match.u4Value == 0)     return 0;
		if (clr_match.u4Value == 255*3) return 255;

//		if (clr_match.u4Value == clrLastMatch.u4Value) return u_best_index;

		// Let RGBDiff know the values.
		// I think it would generate a lot more code if u1RR were a CColour since its used often, 
		// this remains to be investigated..

		u1RR = clr_match.u1Red;
		u1GG = clr_match.u1Green;
		u1BB = clr_match.u1Blue;

		((CColour&)clrLastMatch).u4Value = clr_match.u4Value;

		u2_found = u4_overflow = u2_pass = 0;

		memset(au2_found_before, 0, sizeof(au2_found_before));

		u4_best_so_far = 999999;

	look_again:

		// u1RR Search.

		// Are we within the range ie above 0.
		if (u1RR - u2_pass >= 0)
		{
			// Is there an entry for this colour value.
			if (aleRList[u1RR - u2_pass] != NULL)
			{
				// There does exist an entry so loop for each in that list.
				for (u4_index = 0; u4_index <= aleRList[u1RR - u2_pass][0]; u4_index++)
				{
					// Get the palette u4_index value used.
					le_tempnum=aleRList[u1RR - u2_pass][u4_index + 1];
					
					// Have we found it before.
					if (au2_found_before[le_tempnum] == 0)
					{
						u4_difference = u4RGBDiff(le_tempnum);
						
						if (u4_difference < u4_best_so_far)
						{	
							u4_best_so_far = u4_difference;
							u_best_index = le_tempnum;
						}
						if (u4_difference<1)
						{
					 		return u_best_index;
						}

						u2_found++;

						// Mark as found already already.
						au2_found_before[le_tempnum] = 1;

					}
				}
			}
		}

		// This loop is the same as the previous one except for we`re looking in the other direction.
		if (u2_pass != 0)
		if (u1RR+u2_pass <= 255)
		{
			if (aleRList[u1RR + u2_pass] != NULL)
			{
				for (u4_index=0; u4_index <= aleRList[u1RR + u2_pass][0]; u4_index++)
				{
					if (au2_found_before[aleRList[u1RR + u2_pass][u4_index + 1]] == 0)
					{
						le_tempnum = aleRList[u1RR + u2_pass][u4_index + 1];
						u4_difference = u4RGBDiff(le_tempnum);

						if (u4_difference < u4_best_so_far)
						{	
							u4_best_so_far = u4_difference;
							u_best_index = le_tempnum;
						}
						if (u4_difference < 1)
						{
					 		return u_best_index ;
						}

						// Mark as found already already.
						au2_found_before[le_tempnum] = 1;
						
						u2_found ++;
					}
				}
			}
		}

	// End of u1RR Search.

	// u1GG Search.
		if (u1GG-u2_pass >= 0)
		{
			if (aleGlist(u1GG - u2_pass) != NULL)
			{
				for (u4_index = 0; u4_index <= aleGlist(u1GG - u2_pass)[0]; u4_index++)
				{
					if (au2_found_before[aleGlist(u1GG-u2_pass)[u4_index + 1]] == 0)
					{
						le_tempnum = aleGlist(u1GG - u2_pass)[u4_index + 1];
						u4_difference = u4RGBDiff(le_tempnum);

						if (u4_difference < u4_best_so_far)
						{	
							u4_best_so_far = u4_difference;
							u_best_index = le_tempnum;
						}
						if (u4_difference < 1)
						{
					 		return u_best_index ;
						}

						// Mark as found already already.
						au2_found_before[le_tempnum] = 1;

						u2_found ++;
					}
				}
			}
		}
		if (u2_pass != 0)
		if (u1GG + u2_pass <= 255)
		{
			if (aleGlist(u1GG + u2_pass)!=NULL)
			{
				for (u4_index = 0; u4_index <= aleGlist(u1GG + u2_pass)[0]; u4_index++)
				{
					if (au2_found_before[aleGlist(u1GG + u2_pass)[u4_index + 1]] == 0)
					{
						le_tempnum = aleGlist(u1GG + u2_pass)[u4_index + 1];
						u4_difference = u4RGBDiff(le_tempnum);
						if (u4_difference<u4_best_so_far)
						{	
							u4_best_so_far = u4_difference;
							u_best_index = le_tempnum;
						}
						if (u4_difference < 1)
						{
					 		return u_best_index ;
						}

						// Mark as found already already.
						au2_found_before[le_tempnum] = 1;

						u2_found ++;
					}
				}
			}
		}
	// End of u1GG Search.

	// u1BB Search.

		if (u1BB-u2_pass >= 0)
		{
			if (aleBList(u1BB - u2_pass) != NULL)
			{
				for (u4_index = 0; u4_index <= aleBList(u1BB - u2_pass)[0]; u4_index++)
				{
					if (au2_found_before[aleBList(u1BB - u2_pass)[u4_index + 1]] == 0)
					{
						le_tempnum = aleBList(u1BB - u2_pass)[u4_index + 1];
						u4_difference = u4RGBDiff(le_tempnum);
						if (u4_difference < u4_best_so_far)
						{	
							u4_best_so_far = u4_difference;
							u_best_index = le_tempnum;
						}
						if (u4_difference < 1)
						{
					 		return u_best_index;
						}

						// Mark as found already already.
						au2_found_before[le_tempnum] = 1;
						
						u2_found ++;
					}
				}
			}
		}
		if (u2_pass != 0)
		if (u1BB+u2_pass <= 255) 
		{
			if (aleBList(u1BB+u2_pass)!=NULL) 
			{
				for (u4_index = 0; u4_index <= aleBList(u1BB + u2_pass)[0]; u4_index++) 
				{
					if (au2_found_before[aleBList(u1BB + u2_pass)[u4_index + 1]] == 0) 
					{
						le_tempnum = aleBList(u1BB + u2_pass)[u4_index + 1];
						u4_difference = u4RGBDiff(le_tempnum);

						if (u4_difference < u4_best_so_far)
						{	
							u4_best_so_far = u4_difference;
							u_best_index = le_tempnum;
						}
						if (u4_difference < 1)
						{
					 		return u_best_index ;
						}
						u2_found ++;

						// Mark as found already already.
						au2_found_before[le_tempnum] = 1;
					} 
				}
			}
		}

	// End of u1BB Search.

		// Check to see if we found enough matches.

		if (u4_best_so_far > 1)
		{
			if (u2_found < 25)
			{
				u2_pass++;
				if (u2_pass > 255)
				{
					return u_best_index;		// Generally never hits here.
				}

				u4_overflow++;

				if (u4_overflow > 800)
				{
					return u_best_index;		// Or Here.
				}
				goto look_again;
			}
		}
		return u_best_index;
	}
