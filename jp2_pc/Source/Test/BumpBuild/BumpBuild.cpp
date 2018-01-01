/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 ***********************************************************************************************
 *
 * $Log: /JP2_PC/Source/Test/BumpBuild/BumpBuild.cpp $
 * 
 * 2     9/24/96 3:00p Pkeet
 * Not working, but didn't want them hanging around.
 * 
 * 1     9/16/96 2:01p Pkeet
 * Initial implementation (empty).
 * 
 **********************************************************************************************/

#include <math.h>
#include "IOStuff.hpp"
#include "HeightBitmap.hpp"

char strOutputFile[1024] = { "bw.bmp" };

int iDistance(int i_x, int i_y, int i_z = 0)
{
	double d_x = (double)i_x;
	double d_y = (double)i_y;
	double d_z = (double)i_z;

	return (int)sqrt(d_x * d_x + d_y * d_y + d_z * d_z);
}

int iGetZ(int i_distance, int i_x, int i_y)
{
	double d_d = (double)i_distance;
	double d_x = (double)i_x;
	double d_y = (double)i_y;

	d_x /= d_d;
	d_y /= d_d;

	double d_z = 1.0 - (d_x * d_x + d_y * d_y);

	return (int)(sqrt(d_z) * 15.95);
}

void SimpleHemisphere(CGreyBitmap* pgbmp)
{
	print("Building a simple hemisphere...");

	int i_size   = pgbmp->bmiInfoHeader.biWidth;
	int i_radius = iDistance(i_size / 2, i_size / 2);

	for (int i = 0; i < i_size; i++)
		for (int j = 0; j < i_size; j++)
			//pgbmp->SetPixel(i, j, i * j);
			//pgbmp->SetPixel(i, j, iDistance(i - i_size / 2, j - i_size / 2));
			pgbmp->SetPixel(i, j, iGetZ(i_radius, i - i_size / 2, j - i_size / 2));
}

int main()
{
	Begin();

	// Get the name of the output file.
	//printf("Enter the output filename > ");
	//scanf("%s", strOutputFile);
	CGreyBitmap* pgbmp = new CGreyBitmap(128, 128);
	print( "Bitmap name: ", strOutputFile);
	print2("Bitmap size: ", pgbmp->bmiInfoHeader.biWidth, pgbmp->bmiInfoHeader.biHeight);
	print("");

	SimpleHemisphere(pgbmp);

	pgbmp->WriteToFile(strOutputFile);
	delete pgbmp;

	End();
	return 0;
}
