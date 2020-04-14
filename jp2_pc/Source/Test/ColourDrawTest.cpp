/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of ColourDrawTest.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/ColourDrawTest.cpp                                              $
 * 
 * 12    7/18/96 3:30p Pkeet
 * Converted to 'CRender' style shell.
 * 
 * 11    7/14/96 10:41a Pkeet
 * 
 * 10    7/13/96 11:55p Pkeet
 * 
 * 9     7/13/96 11:32p Pkeet
 * 
 * 8     7/13/96 11:19p Pkeet
 * 
 * 7     7/13/96 11:07p Pkeet
 * Finished test.
 * 
 * 6     7/13/96 10:59p Pkeet
 * Finished test.
 * 
 * 5     7/13/96 9:55p Pkeet
 * Test finished.
 * 
 * 4     7/13/96 5:51p Pkeet
 * Speedbump test.
 * 
 * 3     6/21/96 2:36p Pkeet
 * Finished the initial tests on the clut.
 * 
 * 2     6/19/96 8:37p Pkeet
 * Initial implementation.
 * 
 * 1     6/18/96 4:23p Pkeet
 * 
 **********************************************************************************************/

#include <math.h>
#include "Common.hpp"
#include "ColourDrawTest.hpp"
#include "Lib/Types/FixedP.hpp"
#include "Lib/Sys/TextOut.hpp"
#include "Lib/View/Raster.hpp"


//int iIncidence  = 230;
int iIncidence  = 255;
int iBearing    = 0;
int iConeHeight = 88;

const int    iNoAnglesShift = 5;
const int    iNoAngles      = 1 << iNoAnglesShift;
const int    iShift         = 8 - iNoAnglesShift;
const double dNoAngles      = (double)iNoAngles;

const double dAmbient = 0.1;



int iSubtractAngle(int i_ang_a, int i_ang_b)
{
	i_ang_a += 256;
	i_ang_a -= i_ang_b;
	return i_ang_a & 0xFF;
}

double Convert(int i)
{
	return ((double)i) * 2.0 * 3.14159 / dNoAngles;
}

uint8 u1Scale(int t, int u, int v)
{
	double a = Convert(t);
	double b = Convert(u);
	double c = Convert(v);

	double x0 = 0.0;
	double y0 = cos(b);
	double z0 = sin(b);

	double x1 = 0.0;
	double y1 = cos(c);
	double z1 = sin(c);

	// Transform x1 and y1:
	double tsin = sin(a);
	double tcos = cos(a);
	double x = x1;
	double y = y1;
	x1 = tcos * x - tsin * y;
	y1 = tsin * x + tcos * y;

	// Dot product:
	double d = x0 * x1 + y0 * y1 + z0 * z1;
	//d = d * 0.5 + 0.5;

	if (d < 0.0)
		d = dAmbient;
	else
		d = (d + dAmbient) / (dAmbient + 1);

	if (d > 0.00001)
		d = sqrt(d);

	int i_ret = (int)(d * 255.0);
	Assert(i_ret >= 0);
	Assert(i_ret < 256);
	return (uint8)(i_ret);
}

class CBumpTable
{
public:
	uint8 u1Table[iNoAngles][iNoAngles][iNoAngles];
public:
	CBumpTable()
	{
		for (int x = 0; x < iNoAngles; x++)
			for (int y = 0; y < iNoAngles; y++)
				for (int z = 0; z< iNoAngles; z++)
				{
					u1Table[x][y][z] = u1Scale(x, y, z);
				}
	}
	inline uint32 u4GetIntensity(int x, int y, int z)
	{
		return (uint32)u1Table[x >> iShift][y >> iShift][z >> iShift];
	}
};

CBumpTable btTable;


int iMiddle(int i)
{
	return 255 - Abs(i - 128);
}

int iDistance(int i, int j, int len)
{
	float d = (float)len;
	float x = (float)i / d;
	float y = (float)j / d;
	x -= 0.5f;
	y -= 0.5f;
	float f = x * x + y * y;
	f = (float)sqrt(f);
	f *= d * 0.7071f;
	
	return (int)f;
}

double sign(double d)
{
	if (d >= 0.0)
		return 1.0;
	return -1.0;
}

int iDirection(int i, int j, int len)
{
	float X, Y, A;

	len >>= 1;
	X = (float)(i - len);
	Y = (float)(j - len);
	if (Y == 0.0)
	{
		if (X >= 0.0f) A = 1.5708f;
		else A = -1.5708f;
	}
	else
	{
		A = (float)atan(X / Y);
		if (Y < 0.0) A -= 3.14159f;
	}
	if (A < 0.0) A = A + 6.28319f;
	if (A > 6.28319) A = A - 6.28319f;
	A *= 255.0f / 6.28319f;
	return (int)A;
}

class CBumpmap
{

public:

	uint8 u1Incidence[iBumpmapLen][iBumpmapLen];
	uint8 u1Bearing[iBumpmapLen][iBumpmapLen];

public:

	void MakeCone()
	{
		for (int i = 0; i < iBumpmapLen; i++)
			for (int j = 0; j < iBumpmapLen; j++)
		{
			// Get distance.
			if (iDistance(i, j, iBumpmapLen) < 40)
			{
				u1Incidence[i][j] = iConeHeight;

				// Get bearing.
				u1Bearing[i][j] = (uint8)(iDirection(i, j, iBumpmapLen));
			}
			else
			{
				u1Incidence[i][j] = 64;
				u1Bearing[i][j]   = 0;
			}
		}
	}

	void MakeHemisphere()
	{
		for (int i = 0; i < iBumpmapLen; i++)
			for (int j = 0; j < iBumpmapLen; j++)
			{
				// Get distance.
				int d = iDistance(i, j, iBumpmapLen);

				if (d < 40)
				{
					u1Incidence[i][j] = 64 + d;

					// Get bearing.
					u1Bearing[i][j] = (uint8)(iDirection(i, j, iBumpmapLen));
				}
				else
				{
					//u1Incidence[i][j] = 255;
					u1Incidence[i][j] = 64;
					u1Bearing[i][j]   = 0;
				}
			}
	}

	void MakeRipple()
	{
		for (int i = 0; i < iBumpmapLen; i++)
			for (int j = 0; j < iBumpmapLen; j++)
		{
			// Get distance.
			int d = iDistance(i, j, iBumpmapLen);
			if (d <= 48)
			{
				u1Incidence[i][j] = 64 + (12 - Abs((d % 24) - 12)) * 4;

				// Get bearing.
				u1Bearing[i][j] = (uint8)(iDirection(i, j, iBumpmapLen));
			}
			else
			{
				u1Incidence[i][j] = 64;
				u1Bearing[i][j]   = 0;
			}
		}
	}

	void Smooth()
	{
		uint8 t[iBumpmapLen][iBumpmapLen];

		for (int i = 1; i < iBumpmapLen-1; i++)
			for (int j = 1; j < iBumpmapLen-1; j++)
		{
			int sum;
			sum  = u1Incidence[i-1][j-1];
			sum += u1Incidence[i-1][j];
			sum += u1Incidence[i-1][j+1];
			sum += u1Incidence[i][j+1];
			sum += u1Incidence[i][j-1];
			sum += u1Incidence[i+1][j-1];
			sum += u1Incidence[i+1][j];
			sum += u1Incidence[i+1][j+1];
			sum >>= 3;
			t[i][j] = sum;
		}
		
		for (int i = 1; i < iBumpmapLen-1; i++)
			for (int j = 1; j < iBumpmapLen-1; j++)
				u1Incidence[i][j] = t[i][j];
	}

public:

	void Make(EBumpmap ebm)
	{
		switch (ebm)
		{
			case cone:
				MakeCone();
				break;
			case hemi:
				MakeHemisphere();
				break;
			case ripple:
				MakeRipple();
				break;
			default:
				Assert(false);
				break;
		}
	}

	uint8 u1GetIntensity(int i, int j)
	{
		return (uint8)btTable.u4GetIntensity
		(
			iSubtractAngle(u1Bearing[i][j], iBearing),
			u1Incidence[i][j],
			iIncidence
		);
	}
};

CBumpmap bump;

void MakeBumpmap(EBumpmap ebm)
{
	bump.Make(ebm);
}

//*********************************************************************************************
void DrawSpeedBump(CRaster* prasDest, int i_xshift, int i_yshift)
{
	int i, j;

	i_xshift *= iBumpmapLen;
	i_yshift *= iBumpmapLen;
	for (i = 0; i < iBumpmapLen; i++)
	{
		for (j = 0; j < iBumpmapLen; j++)
		{
			//if (bump.u1Incidence[i][j] != 255)
				prasDest->PutPixel(i + i_xshift, j + i_yshift, bump.u1GetIntensity(i, j));
		}
	}
}

class CSin
{
public:
	uint8 u1Convert[256];
public:
	CSin()
	{
		for (int i = 0; i < 256; i++)
		{
			double s = (double)(iNoAngles - 1) * (sin((double)i * 3.14159 / 128.0) / 2.0 + 0.5);
			u1Convert[i] = (uint8)s;
		}
	}
	uint8 GetSin(int i)
	{
		return u1Convert[i & 255];
	}
};

CSin Sin;

int iAmp = 0;

//*********************************************************************************************
void DrawWater(CRaster* prasDest)
{
	int i, j;
	int i_width  = prasDest->iWidth;//Min(prasDest->iWidth, 320);
	int i_height = prasDest->iHeight;//Min(prasDest->iHeight, 160);


	for (i = 0; i < i_width; i++)
	{
		for (j = 0; j < i_height; j++)
		{
			int d0 = iDistance(i, j, i_height) * 10;
			int d1 = iDistance(i_width - i, j, i_height) * 10;
			int i_pix = Sin.GetSin(d0 + iAmp) + Sin.GetSin(d1 + iAmp);
			i_pix *= 4;

			/*
			uint8 u1_pixel = (uint8)btTable.u4GetIntensity
			(
				iSubtractAngle(i_direction, iBearing),
				i_incidence,
				iIncidence
			);
			//prasDest->PutPixel(i, j, u1_pixel);
			*/
			prasDest->PutPixel(i, j, i_pix);
		}
	}
	iAmp += 4;
}
