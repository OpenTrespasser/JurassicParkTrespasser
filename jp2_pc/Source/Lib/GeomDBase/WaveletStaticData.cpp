/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of WaveletStaticData.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletStaticData.cpp                                   $
 * 
 * 33    9/09/98 11:31a Pkeet
 * Needed to add an include to be compatible with a debug switch in 'Texture.hpp.'
 * 
 * 32    9/05/98 10:34p Agrant
 * get rid of symbol length warnings
 * 
 * 31    7/23/98 6:29p Mlange
 * Updated stat printing.
 * 
 * 30    6/18/98 5:22p Mlange
 * Added profile stats.
 * 
 * 29    6/10/98 3:20p Mlange
 * Changes to profile stat.
 * 
 * 28    6/03/98 2:55p Mlange
 * Added stat printing for terrain textures.
 * 
 * 27    4/03/98 4:07p Mlange
 * Improved stat printing.
 *
 **********************************************************************************************/

#include "GblInc/Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "WaveletStaticData.hpp"

#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeQuery.hpp"
#include "Lib/GeomDBase/TerrainTexture.hpp"
#include "Lib/Sys/TextOut.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/FastHeap.hpp"

#pragma warning(disable: 4786)

namespace NMultiResolution
{
	//**********************************************************************************************
	CProfileStatMain psWaveletTIN;
	CProfileStatMain psWaveletQuery;

	bool bAverageStats = false;

	//**********************************************************************************************
	void AllocMemory()
	{
		  CQuadRootTIN::AllocMemory();
		CQuadRootQuery::AllocMemory();

		uint u_size = (uDEFAULT_ALLOC_TIN_NODES + uDEFAULT_ALLOC_QUERY_NODES) * dAVG_QUAD_NODE_TO_VERTEX_RATIO *
		                                                                        dAVG_QUAD_VERTEX_TO_SCALING_ARRAY_RATIO;
		CFastHeap* pfh_allocations = new CFastHeap(u_size);
		CQuadVertexRecalc::aaScalingAlloc.SetFastHeap(pfh_allocations);
	}


	//**********************************************************************************************
	void FreeMemory()
	{
		  CQuadRootTIN::FreeMemory();
		CQuadRootQuery::FreeMemory();

		CFastHeap* pfh_allocations = CQuadVertexRecalc::aaScalingAlloc.pfhGetFastHeap();
		CQuadVertexRecalc::aaScalingAlloc.SetFastHeap(0);
		delete pfh_allocations;
	}


	//**********************************************************************************************
	void PrintProfileStats(CConsoleBuffer& con, const CQuadRootTIN* pqntin_root, const CQuadRootQuery* pqnq_root)
	{
		Assert(pqntin_root != 0 && pqnq_root != 0);

		con.Print("Recalc quad tree   Curr    Max\n");
		con.Print
		(
			" Scaling array:   %3dKB  %3dKB\n",
			(CQuadVertexRecalc::aaScalingAlloc.uNumAllocBytes() + 512) / 1024,
			(CQuadVertexRecalc::aaScalingAlloc.uMaxAllocBytes() + 512) / 1024
		);

		con.Print
		(
			" Wavelet data :   %3dKB\n",
			(((pqntin_root->ptdhGetWaveletData()->uDataBitSize() + 7) / 8) + 512) / 1024
		);

		con.Print
		(
			"Mem commit: %dKB, free: %dKB, reserved: %dKB\n",
			(CQuadVertexRecalc::aaScalingAlloc.pfhGetFastHeap()->uNumBytesUsed() + 512) / 1024,
		    (CQuadVertexRecalc::aaScalingAlloc.pfhGetFastHeap()->uNumBytesFree() + 512) / 1024,
			(CQuadVertexRecalc::aaScalingAlloc.pfhGetFastHeap()->uGetSize()      + 512) / 1024
		);

		con.Print("\n");
		CTextureNode::PrintStats(con);

		con.Print("\nTerrain textures:\n");
		CTextureNode::ptexmTexturePages->PrintStats(con);

		con.Print("\nTerrain shadow buffers:\n");
		CTextureNode::ptexmTextureStaticShadowPages->PrintStats(con);

		con.Print("\n");
		pqntin_root->PrintStats(con);

		con.Print("\n");
		pqnq_root->PrintStats(con);

		con.Print("\n");
		psWaveletTIN.WriteToConsole(con);

		con.Print("\n");
		psWaveletQuery.WriteToConsole(con);
	}

};



CProfileStat NMultiResolution::CQuadRootTIN::psUpdate("Update TIN qt", &NMultiResolution::psWaveletTIN, Set(epfMASTER));

	CProfileStat NMultiResolution::CQuadRootTIN::psEvaluateBranch("Eval branch", &NMultiResolution::CQuadRootTIN::psUpdate);

		CProfileStat NMultiResolution::CQuadNodeTIN::psEvaluate("Eval node", &NMultiResolution::CQuadRootTIN::psEvaluateBranch);

			CProfileStat NMultiResolution::CQuadNodeTIN::psProject(     "Project node", &NMultiResolution::CQuadRootTIN::psEvaluate);
			CProfileStat NMultiResolution::CQuadNodeTIN::psEvalWavelets("Eval wvlts",   &NMultiResolution::CQuadRootTIN::psEvaluate);
			CProfileStat NMultiResolution::CQuadNodeTIN::psRefine(      "Refine",       &NMultiResolution::CQuadRootTIN::psEvaluate);
			CProfileStat NMultiResolution::CQuadNodeTIN::psDecimate(    "Decimate",     &NMultiResolution::CQuadRootTIN::psEvaluate);

	CProfileStat NMultiResolution::CQuadRootTIN::psTextureBranch("Texture branch", &NMultiResolution::CQuadRootTIN::psUpdate);

CProfileStat NMultiResolution::CQuadRootTINShape::psEvaluateTexture("Eval textures", &NMultiResolution::psWaveletTIN);

CProfileStat NMultiResolution::CQueryRect::psEvaluate("Evaluate", &NMultiResolution::psWaveletQuery, Set(epfMASTER));

	CProfileStat NMultiResolution::CQueryRect::psRefine( "Refine",  &NMultiResolution::CQueryRect::psEvaluate);
	CProfileStat NMultiResolution::CQueryRect::psIterate("Iterate", &NMultiResolution::CQueryRect::psEvaluate);


CProfileStat NMultiResolution::CQuadRootQuery::psRayEvaluate("Ray cast", &NMultiResolution::psWaveletQuery, Set(epfMASTER));

	CProfileStat NMultiResolution::CQuadRootQuery::psRayRefine(   "Refine",    &NMultiResolution::CQuadRootQuery::psRayEvaluate);
	CProfileStat NMultiResolution::CQuadRootQuery::psRayIntersect("Intersect", &NMultiResolution::CQuadRootQuery::psRayEvaluate);


CProfileStat NMultiResolution::CQuadRootQuery::psUpdate("Update query qt", &NMultiResolution::psWaveletQuery);

