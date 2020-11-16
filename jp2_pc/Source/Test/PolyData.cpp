/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of TestMath.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/PolyData.cpp                                                     $
 * 
 * 15    96/09/09 18:35 Speter
 * Made compatible with change of fIntensity to lvIntensity.
 * 
 * 14    7/31/96 3:25p Pkeet
 * Finished tests.
 * 
 * 13    7/31/96 12:40p Pkeet
 * Updated for new object 2d definitions.
 * 
 * 12    96/07/23 17:23 Speter
 * Fixed for new object structures.
 * 
 * 11    7/19/96 11:22a Pkeet
 * Fixed shell problem.
 * 
 * 10    96/07/18 17:19 Speter
 * Changes affecting zillions of files:
 * Changed SRenderCoord to SRenderVertex.
 * Added SRenderTriangle type, changed drawing primitives to accept it.
 * 
 * 9     7/12/96 4:59p Pkeet
 * Finished debugging texture mapper and gouraud shader.
 * 
 * 8     7/08/96 4:39p Pkeet
 * Removed SourceSafe conflicts.
 * 
 * 7     7/08/96 3:33p Pkeet
 * 
 * 6     96/07/03 13:02 Speter
 * Moved several files to new directories, and changed corresponding include statements.
 * 
 * 5     6/25/96 2:34p Pkeet
 * Finished tests.
 * 
 * 4     5/29/96 4:36p Pkeet
 * Initial implementation.
 * 
 * 3     5/23/96 9:30a Pkeet
 * 
 * 2     5/17/96 10:22a Pkeet
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Test/PolyData.hpp"

const uint uMaxNoVertices = 4;

const float fPlane0 = 1.0f / 2.0f;
const float fPlane1 = 1.0f / 1.0f;
const float fPlane2 = 1.0f / 3.0f;

SRenderVertex arvTestVertices[3];

class CInitVertices
{
public:
	CInitVertices()
	{
		arvTestVertices[0].fInvZ       = 0.5f;
		arvTestVertices[1].fInvZ       = 0.5f;
		arvTestVertices[2].fInvZ       = 0.5f;

		arvTestVertices[0].lvIntensity = 0.9f;
		arvTestVertices[1].lvIntensity = 0.9f;
		arvTestVertices[2].lvIntensity = 0.9f;

		arvTestVertices[0].v3Screen.tX = 42.1559f;
		arvTestVertices[0].v3Screen.tY = 318.134f;
		arvTestVertices[0].tcTex.tX    = 0.0f;
		arvTestVertices[0].tcTex.tY    = 0.0f;

		arvTestVertices[1].v3Screen.tX = 101.625f;
		arvTestVertices[1].v3Screen.tY = 132.588f;
		arvTestVertices[1].tcTex.tX    = 1.0f;
		arvTestVertices[1].tcTex.tY    = 0.5f;

		arvTestVertices[2].v3Screen.tX = 68.0313f;
		arvTestVertices[2].v3Screen.tY = 237.271f;
		arvTestVertices[2].tcTex.tX    = 1.0f;
		arvTestVertices[2].tcTex.tY    = 0.0f;
	};
};

CInitVertices InitVertices;
/*
SRenderVertex arvTestVertices[9] =
{
	{  20.3f,  20.3f, 15.00f, 0.0f, 0.0f },	// 0
	{  90.3f, 290.3f,  0.00f, 0.0f,   15.0f },	// 1
	{ 284.3f, 160.3f, 31.00f,  15.0f,   5.0f },	// 2

	{ 310.3f,  20.3f, 31.00f,   6.0f, 64.0f },	// 3
	{ 110.2f, 100.2f,  0.00f,  32.0f,  32.0f },	// 4

	{ 140.0f,  40.0f, 31.00f,  5.0f,  5.0f },	// 5
	{ 140.0f, 190.0f, 31.00f,  5.0f,  5.0f },	// 6
	{ 280.0f, 120.0f, 31.00f,  5.0f,  5.0f },	// 7

	{ 400.0f,  40.0f,  0.00f,  5.0f,  5.0f }
};
*/

int aiPoly0[3] =
{
	0 * sizeof(SRenderVertex),
	1 * sizeof(SRenderVertex),
	2 * sizeof(SRenderVertex)
	//4 * sizeof(SRenderVertex)
};

int aiPoly1[3] =
{
	0 * sizeof(SRenderVertex),
	1 * sizeof(SRenderVertex),
	//2 * sizeof(SRenderVertex),
	3 * sizeof(SRenderVertex)
	//4 * sizeof(SRenderVertex)
};

int aiPoly2[3] =
{
	2 * sizeof(SRenderVertex),
	3 * sizeof(SRenderVertex),
	4 * sizeof(SRenderVertex)
};

int aiPoly3[3] =
{
	3 * sizeof(SRenderVertex),
	0 * sizeof(SRenderVertex),
	4 * sizeof(SRenderVertex)
};

int aiPoly4[4] =
{
	5 * sizeof(SRenderVertex),
	6 * sizeof(SRenderVertex),
	7 * sizeof(SRenderVertex),
	8 * sizeof(SRenderVertex)
};

void RollTestVertices()
{
	SRenderVertex rv;

	rv = arvTestVertices[uMaxNoVertices - 1];
	for (int i = uMaxNoVertices - 1; i >= 1; i--)
		arvTestVertices[i] = arvTestVertices[i - 1];
	arvTestVertices[0] = rv;
}
