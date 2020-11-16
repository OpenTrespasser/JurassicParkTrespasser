/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of TerrainTest.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/TerrainTest.cpp                                                $
 * 
 * 56    9/23/98 7:41p Mlange
 * Wavelet quad tree nodes now have a single pointer to the first descendant and a pointer to
 * the next sibling, instead of four pointers for each of the descendants.
 * 
 * 55    9/09/98 11:30a Pkeet
 * Needed to add an include to be compatible with a debug switch in 'Texture.hpp.'
 * 
 * 54    8/30/98 4:49p Asouth
 * changes in usage rules; loop variable
 * 
 * 53    8/13/98 6:10p Mlange
 * Paint message now requires registration of the recipients.
 * 
 * 52    8/13/98 1:47p Mlange
 * The step message now requires registration of its recipients.
 * 
 **********************************************************************************************/

#include "StdAfx.h"
#include "gblinc/common.hpp"

#include "Lib/W95/WinInclude.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "TerrainTest.hpp"
#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeTForm.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeQuery.hpp"
#include "Lib/GeomDBase/WaveletStaticData.hpp"
#include "Lib/GeomDBase/WaveletDataFormat.hpp"
#include "Lib/GeomDBase/TerrainLoad.hpp"
#include "Lib/GeomDBase/TerrainTexture.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPaint.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/View/LineDraw.hpp"

//TODO:
//-Determine if getting rid of in influence check in projection is possible. Also determine if it generally speeds
// up evaluation times.
//-Fix edge of world terrain probs.
//-Allow negative terrain height values?
//-Calc vertex normals by averaging terrain tris.
//-Alloc mem for quad tree funcs should take # of bytes instead of # of nodes. Remove silly type ratio #defines.
//-Key of screen mode changed message and update pixel tolerance accordingly, e.g. tolerance should be size of
// projection on say a 15" monitor.
//-Test Coef projection code. Test w/ various screen modes and make sure it works when screen modes and camera
// properties change.
//-In terrain test code check for potential memory leak bugs by occasionally decimating the entire terrain
// for TIN and query quad tree. All memory should be freed at that point.
//-Determine sensible settings for max num of quad nodes etc.

//OPTIMISATIONS:
//-Consider initialising array of triangle pointers in texture branch function.
//-Consider constructing an array of pointers to vertices within view, instead of iterating trhough each triangle
// and then through each vertex of that triangle.
//-Consider storing quad node types on separate fast heaps for better cache efficiency.
//-Consider making the descendants link to each other instead of the parent linking all four descendants. This will
// save two pointers per node.
//-Do not recurse quad tree to find leaf combine/leaf nodes in evaluate func. Use linked list of tri info types
// instead. This might save about 1/2 ms per frame.


#ifdef __MWERKS__
 using namespace NMultiResolution;
#else
 using NMultiResolution::CQuadNodeTIN;
 using NMultiResolution::CQuadNodeQuery;
 using NMultiResolution::CQuadRootTForm;
 using NMultiResolution::CTransform;
 using NMultiResolution::CTransformedData;
 using NMultiResolution::CQuadVertexQuery;
 using NMultiResolution::CQueryRect;
#endif


//**********************************************************************************************
//
// CQueryTest implementation.
//
	//*****************************************************************************************
	CQueryTest::~CQueryTest()
	{
		delete pqrQuery;
	}


	//*****************************************************************************************
	void CQueryTest::Update(CQuadRootQuery* pqnq_root)
	{
		const uint  uLIFETIME_RANGE     = 40;	// Max number of steps a query lives or is dead.
		const TReal rCREATE_SIZE_RANGE  = 5;	// Max radius of a new query.
		const TReal rMOVE_PROB          = .8;	// Probability of an active query actually moving this step.
		const TReal rMOVE_DIST_RANGE    = 5;	// Max distance an active query moves (in xy) per step.

		const TReal rWORLD_EXTENT_INCREASE = 10;// Increment for the extents of world, so that queries are occasionally
												// performed off the edge of the world.

		static CRandom rnd;

		// If the current life counter is zero, this query either dies or is reborn.
		if (iLifeStepsRemaining == 0)
		{
			iLifeStepsRemaining = rnd(0u, uLIFETIME_RANGE);

			// If no query class was allocated, this query will be reborn.
			if (pqrQuery == 0)
			{
				// Construct a query new volume, its initial presence and movement direction.
				bvsVolume = CBoundVolSphere((TReal)Max(rnd(0, rCREATE_SIZE_RANGE), .001));

				CVector2<> v2_start = pqnq_root->mpConversions.rcWorldSpaceExtents.v2Start() - CVector2<>(rWORLD_EXTENT_INCREASE, rWORLD_EXTENT_INCREASE);
				CVector2<> v2_end   = pqnq_root->mpConversions.rcWorldSpaceExtents.v2End()   + CVector2<>(rWORLD_EXTENT_INCREASE, rWORLD_EXTENT_INCREASE);

				pr3Pos = CPresence3<>(CVector3<>(rnd(v2_start.tX, v2_end.tX), rnd(v2_start.tY, v2_end.tY), 0));

				d3MoveDir = CDir3<>(rnd(0.0, 1.0), rnd(0.0, 1.0), rnd(0.0, 1.0));

				pqrQuery = new CQueryRect(pqnq_root, bvsVolume, pr3Pos);
			}
			else
			{
				// A query was active, kill it now.
				delete pqrQuery;
				pqrQuery = 0;

				iLifeStepsRemaining = -iLifeStepsRemaining;
			}

		}
		// If the query is currently dead, update the counter until it is time to resurrect it.
		else if (iLifeStepsRemaining < 0)
		{
			Assert(pqrQuery == 0);

			++iLifeStepsRemaining;
		}
		else
		{
			Assert(pqrQuery != 0);

			if (rnd(0.0, 1.0) <= rMOVE_PROB)
			{
				// The query is living, move it by a random amount.
				TReal r_dist = rnd(0, rMOVE_DIST_RANGE);

				pr3Pos.v3Pos += d3MoveDir * r_dist;

				pqrQuery->SetVolume(bvsVolume, pr3Pos);
			}

			// Decrement the counter until it is time to kill this query.
			--iLifeStepsRemaining;
		}
	}



//**********************************************************************************************
//
// CTerrainTest implementation.
//

	//*****************************************************************************************
	CTerrainTest::CTerrainTest()
		: iNumTestSteps(0), iNumCoefCheckedTIN(0), iNumCoefCheckedQuery(0),
		  ptdhData(0), prasQuadTreeVertices(0), prasTransformVertices(0), prasOrigVertices(0), prasOrigWavelets(0)
	{
		SetInstanceName("Terrain Test");
	    NMultiResolution::CTextureNode::SetTextureMemSize();
		NMultiResolution::AllocMemory();

		// Register this entity with the message types it needs to receive.
		 CMessageStep::RegisterRecipient(this);
		CMessagePaint::RegisterRecipient(this);
	}


	//*****************************************************************************************
	CTerrainTest::~CTerrainTest()
	{
		CMessagePaint::UnregisterRecipient(this);
		 CMessageStep::UnregisterRecipient(this);

		DeleteAll();

		NMultiResolution::FreeMemory();
		NMultiResolution::CTextureNode::SetTextureMemSize(0);
	}


	//*****************************************************************************************
	void CTerrainTest::Process(const CMessageStep&)
	{
		if (iNumTestSteps % 30 == 0)
			TestInitTransform();

		static CRandom rnd;

		if ((iNumTestSteps & 1) == 0)
		{
			//
			// Evaluate the wavelet quad tree with a random tolerance and observer location.
			//
			NMultiResolution::psWaveletTIN.Reset();

			// Set the dummy tolerances.
			pqntinRoot->rvarPixelTolerance.SetRange(0, 1700);
			pqntinRoot->rvarPixelToleranceFar.SetRange(0, 1700);

			const double d_min_tolerance = 100;
			const double d_max_tolerance = 200;

			TReal r_tolerance = rnd(d_min_tolerance, d_max_tolerance);
			pqntinRoot->rvarPixelTolerance    = r_tolerance;
			pqntinRoot->rvarPixelToleranceFar = r_tolerance;

			CVector3<> v3_pos
			(
				rnd( double(pqntinRoot->mpConversions.rcWorldSpaceExtents.tX0()), pqntinRoot->mpConversions.rcWorldSpaceExtents.tX1() ),
				rnd( double(pqntinRoot->mpConversions.rcWorldSpaceExtents.tY0()), pqntinRoot->mpConversions.rcWorldSpaceExtents.tY1() ),
				rnd(0.0, 255.0)
			);

			CWDbQueryActiveCamera().tGet()->SetPos(v3_pos);

			pqntinRoot->Update(CWDbQueryActiveCamera().tGet());

			pqntinRoot->CheckTriangulation();


			//
			// Ensure the vertices of the quad tree are valid.
			//
			 prasQuadTreeVertices->Clear(cfNAN);
			prasTransformVertices->Clear(cfZERO);

			prasQuadTreeVertices->tPix(0, 0) = pqntinRoot->pqvtGetVertex(0)->cfScaling();
			RecurseTree(pqntinRoot);

			// Synthesise the current quad tree transform.
			CTransform tf_synthesised
			(
				prasTransformVertices,
				ptdhData->cfRoot
			);

			tf_synthesised.Synthesise();


			// Compare the quadtree vertices with the synthesised wavelet transform data.
			iNumCoefCheckedTIN = 0;

			for (int i_y = 0; i_y < prasQuadTreeVertices->iHeight; i_y++)
			{
				for (int i_x = 0; i_x < prasQuadTreeVertices->iWidth; i_x++)
				{
					CCoef cf_quadtree_tin = prasQuadTreeVertices->tPix(i_x, i_y);

					if (cf_quadtree_tin != cfNAN)
					{
						CCoef cf_synthesised = prasTransformVertices->tPix(i_x, i_y);

						CCoef cf_abs_diff = (cf_quadtree_tin - cf_synthesised).cfAbs();

						AlwaysAssert(cf_abs_diff == cfZERO);

						iNumCoefCheckedTIN++;
					}
				}
			}
		}
		else
		{
			//
			// Update the height queries.
			//
			NMultiResolution::psWaveletQuery.Reset();

			if (iNumTestSteps % 3 == 0)
				pqnqRoot->Update();

			int i;
			for (i = 0; i < saqtQueries.uLen; i++)
				saqtQueries[i].Update(pqnqRoot);


			//
			// Ensure the height queries are valid.
			//

			iNumCoefCheckedQuery = 0;

			for (i = 0; i < saqtQueries.uLen; i++)
			{
				CQueryRect* pwqttop_curr = saqtQueries[i].pqrQuery;

				if (pwqttop_curr != 0)
				{
					CQueryRect::CIterator it = pwqttop_curr->itBegin();

					for (; it; ++it)
					{
						for (int i_corner = 0; i_corner < 3; i_corner++)
						{
							const NMultiResolution::CQuadVertexRecalc* pqvt = (*it)->pqvtGetVertex(i_corner);

							if (pqvt->iX() != prasOrigVertices->iWidth && pqvt->iY() != prasOrigVertices->iHeight)
							{
								CCoef cf_quadtree_query = pqvt->cfScaling();
								CCoef cf_original       = prasOrigVertices->tPix(pqvt->iX(), pqvt->iY());

								CCoef cf_abs_diff = (cf_quadtree_query - cf_original).cfAbs();

								AlwaysAssert(cf_abs_diff == cfZERO);

								iNumCoefCheckedQuery++;
							}
						}
					}
				}
			}
		}

		iNumTestSteps++;
	}



	//*****************************************************************************************
	void CTerrainTest::Process(const CMessagePaint& msgpaint)
	{
		if (pqntinRoot == 0)
			return;

		// Attempt to cast to a raster win.
		CRasterWin* pras_win = dynamic_cast<CRasterWin*>(msgpaint.renContext.pScreenRender->prasScreen);

		if (pras_win == 0)
			return;

		// Get camera origin, in quad space.
		const CCamera& cam = *CWDbQueryActiveCamera().tGet();

		CVector2<> v2_quad_pos = CVector2<>(cam.v3Pos()) * pqntinRoot->mpConversions.tlr2WorldToQuad;

		// Visualise state.
		if ((iNumTestSteps & 1) == 0)
			pqntinRoot->rcDrawWireframe(pras_win, CColour(1.0, 0.0, 0.0), true, false, v2_quad_pos);
		else
			pqnqRoot->rcDrawWireframe(pras_win, CColour(1.0, 1.0, 0.0), true, false, v2_quad_pos);

		// Update terrain console.
		conTerrain.ClearScreen();
		conTerrain.Print("Steps: %d, TIN coef checked: %d, Query coef checked: %d\n", iNumTestSteps, iNumCoefCheckedTIN, iNumCoefCheckedQuery);

/*
		double d_num_nodes = CQuadNodeTIN::uNumAlloc();

		double d_node_to_vert_ratio     =  NMultiResolution::CQuadVertexTIN::uNumAlloc() / d_num_nodes;
		double d_node_to_tri_ratio      =    NMultiResolution::CTriangleTIN::uNumAlloc() / d_num_nodes;
		double d_node_to_tri_info_ratio = NMultiResolution::CTriNodeInfoTIN::uNumAlloc() / d_num_nodes;

		double d_num_verts = NMultiResolution::CQuadVertexTIN::uNumAlloc() + NMultiResolution::CQuadVertexQuery::uNumAlloc();
		double d_vert_to_scaling_array_ratio = NMultiResolution::CQuadVertexRecalc::aaScalingAlloc.uNumAllocBytes() / d_num_verts;

		conTerrain.Print("%f, %f, %f, %f\n", d_node_to_vert_ratio, d_node_to_tri_ratio, d_node_to_tri_info_ratio, d_vert_to_scaling_array_ratio);
*/

		NMultiResolution::PrintProfileStats(conTerrain, pqntinRoot, pqnqRoot);
	}


	//******************************************************************************************
	void CTerrainTest::TestInitTransform()
	{
		DeleteAll();

		conTerrain.ClearScreen();

		static CRandom rnd;

		// Set up a random terrain info structure.
		SExportDataInfo edi_rnd;

		TReal r_res = .25 * (1 << rnd(0u, 4u));

		edi_rnd.iNumDataPoints       = rnd(1000u, 3000u);
		edi_rnd.v2Min                = CVector2<>(rnd(10u, 100u) * -r_res, rnd(10u, 100u) * -r_res);
		edi_rnd.rHighestRes          = r_res;
		edi_rnd.rMinHeight           = rnd(0.0, 100.0);
		edi_rnd.rMaxHeight           = edi_rnd.rMinHeight + rnd(0.0, 1000.0);
		edi_rnd.iMaxIndices          = NextPowerOfTwo(rnd(16u, 256u));
		edi_rnd.iNumQuantisationBits = 20;

		// Allocate and set the fastheap used for allocations of the transform quad tree classes.
		CQuadRootTForm::AllocMemory();

		{
			CQuadRootTForm qntr_test(edi_rnd, conTerrain);

			qntr_test.RandomisePoints(edi_rnd, rnd);

//			CTerrainExportedData ted("c:/jp2_pc/NewTerrainTest.trr", conTerrain);
//			CTerrainExportedData ted("c:/jp2_pc/PineValleyNF.trr", conTerrain);

//			CQuadRootTForm qntr_test(ted.ediGetInfo(), conTerrain);

//			qntr_test.AddPoints(ted);


			prasQuadTreeVertices  = new CRasterT<CCoef>(qntr_test.iGetSize(), qntr_test.iGetSize());
			prasTransformVertices = new CRasterT<CCoef>(qntr_test.iGetSize(), qntr_test.iGetSize());


			prasOrigWavelets = qntr_test.prasGetData();

			CTransform tf_analyse(prasOrigWavelets);
			tf_analyse.Analyse();

			prasOrigVertices = new CRasterT<CCoef>(qntr_test.iGetSize(), qntr_test.iGetSize());
			memcpy(*prasOrigVertices, *prasOrigWavelets, prasOrigVertices->iSize() * sizeof(CCoef));

			CTransform tf_synthesise(prasOrigVertices, prasOrigWavelets->tPix(0, 0));
			tf_synthesise.Synthesise();

			qntr_test.Analyse();

			qntr_test.CheckData(prasOrigWavelets);

			ptdhData = new CTransformedDataHeader(qntr_test);

			qntr_test.CheckData(*ptdhData);
		}

		CQuadRootTForm::FreeMemory();

		pqntinRoot = ::new CQuadRootTIN(  ptdhData);
		pqnqRoot   = ::new CQuadRootQuery(ptdhData);

		for (int i = 0; i < iMAX_QUERY_TESTS; i++)
			saqtQueries << CQueryTest();
	}


	//******************************************************************************************
	void CTerrainTest::DeleteAll()
	{
		for (int i = 0; i < saqtQueries.uLen; i++)
			delete saqtQueries[i].pqrQuery;

		saqtQueries.uLen = 0;

		delete ptdhData;
		delete prasQuadTreeVertices;
		delete prasTransformVertices;
		delete prasOrigVertices;
		delete prasOrigWavelets;

		::delete pqntinRoot;
		::delete pqnqRoot;
	}


	//******************************************************************************************
	void CTerrainTest::RecurseTree(const CQuadNodeTIN* pqntin)
	{
		if (pqntin->bHasDescendants())
		{

			for (int i = 0; i < 3; i++)
			{
				const NMultiResolution::CQuadVertexTIN* pqvt = pqntin->ptqnGetDescendant(0)->pqvtGetVertex(i + 1);

				Assert(prasOrigWavelets->tPix(pqvt->iX(), pqvt->iY()) == pqvt->cfWavelet());

				 prasQuadTreeVertices->tPix(pqvt->iX(), pqvt->iY()) = pqvt->cfScaling();
				prasTransformVertices->tPix(pqvt->iX(), pqvt->iY()) = pqvt->cfWavelet();
			}

			RecurseTree(pqntin->ptqnGetDescendant(0));
			RecurseTree(pqntin->ptqnGetDescendant(1));
			RecurseTree(pqntin->ptqnGetDescendant(2));
			RecurseTree(pqntin->ptqnGetDescendant(3));
		}
	}


namespace NMultiResolution
{


//**********************************************************************************************
//
// NMultiResolution::CTransform implementation.
//
	//******************************************************************************************
	CTransform::CTransform(CRasterT<CCoef>* pras_analyse)
		: prasTransformed(pras_analyse), uSize(pras_analyse->iWidth), iLevel(0)
	{
		Assert(pras_analyse != 0);
		InterpolateMissingData();
	}

	//******************************************************************************************
	CTransform::CTransform(CRasterT<CCoef>* pras_synthesise, CCoef cf_root)
		: prasTransformed(pras_synthesise), uSize(pras_synthesise->iWidth), iLevel(-int(uLog2(pras_synthesise->iWidth)))
	{
		Assert(prasTransformed->iWidth == prasTransformed->iHeight);
		Assert(bPowerOfTwo(uSize));
		Assert(iDimScaling() == 1);

		prasTransformed->tPix(0, 0) = cf_root;
	}


	//******************************************************************************************
	void CTransform::InterpolateMissingData()
	{
		iLevel = -int(uLog2(uSize));
			
		do
		{
			iLevel++;

			int i_stride = iScalingStride();

			bool b_x_odd_node = false;
			bool b_y_odd_node = false;

			for (int i_y = 0; i_y < uSize; i_y += i_stride * 2)
			{
				for (int i_x = 0; i_x < uSize; i_x += i_stride * 2)
				{
					CCoef cf_predict;

					if (prasTransformed->tPix(i_x + i_stride, i_y) == cfNAN)
						prasTransformed->tPix(i_x + i_stride, i_y) = cfPredict(i_x + i_stride, i_y);

					if (prasTransformed->tPix(i_x, i_y + i_stride) == cfNAN)
						prasTransformed->tPix(i_x, i_y + i_stride) = cfPredict(i_x, i_y + i_stride);

					if (prasTransformed->tPix(i_x + i_stride, i_y + i_stride) == cfNAN)
					{
						if (b_x_odd_node == b_y_odd_node)
						{
							cf_predict  = cfReadReflected(i_x + i_stride, i_y + i_stride,  i_stride,  i_stride);
							cf_predict += cfReadReflected(i_x + i_stride, i_y + i_stride, -i_stride, -i_stride);
						}
						else
						{
							cf_predict  = cfReadReflected(i_x + i_stride, i_y + i_stride,  i_stride, -i_stride);
							cf_predict += cfReadReflected(i_x + i_stride, i_y + i_stride, -i_stride,  i_stride);
						}
	
						prasTransformed->tPix(i_x + i_stride, i_y + i_stride) = cf_predict.cfPredict();
					}

					b_x_odd_node = !b_x_odd_node;
				}

				b_y_odd_node = !b_y_odd_node;
			}
		}
		while (iLevel != 0);
	}


	//******************************************************************************************
	void CTransform::Analyse()
	{
		Assert(iDimScaling() == uSize);

		do
		{
			int i_stride = iScalingStride();
			int i_x, i_y;

			bool b_x_odd_node = false;
			bool b_y_odd_node = false;

			for (i_y = 0; i_y < uSize; i_y += i_stride * 2)
			{
				for (i_x = 0; i_x < uSize; i_x += i_stride * 2)
				{
					CCoef cf_predict;

					// Horizontal.
					cf_predict = cfPredict(i_x + i_stride, i_y);
					prasTransformed->tPix(i_x + i_stride, i_y) -= cf_predict;

					// Vertical.
					cf_predict = cfPredict(i_x, i_y + i_stride);
					prasTransformed->tPix(i_x, i_y + i_stride) -= cf_predict;

					// Diagonal.
					if (b_x_odd_node == b_y_odd_node)
					{
						cf_predict  = cfReadReflected(i_x + i_stride, i_y + i_stride,  i_stride,  i_stride);
						cf_predict += cfReadReflected(i_x + i_stride, i_y + i_stride, -i_stride, -i_stride);
					}
					else
					{
						cf_predict  = cfReadReflected(i_x + i_stride, i_y + i_stride,  i_stride, -i_stride);
						cf_predict += cfReadReflected(i_x + i_stride, i_y + i_stride, -i_stride,  i_stride);
					}

					cf_predict = cf_predict.cfPredict();

					prasTransformed->tPix(i_x + i_stride, i_y + i_stride) -= cf_predict;

					b_x_odd_node = !b_x_odd_node;
				}

				b_y_odd_node = !b_y_odd_node;
			}


			b_x_odd_node = false;
			b_y_odd_node = false;

			for (i_y = 0; i_y < uSize; i_y += i_stride * 2)
			{
				for (i_x = 0; i_x < uSize; i_x += i_stride * 2)
				{
					if (b_x_odd_node == b_y_odd_node)
						prasTransformed->tPix(i_x, i_y) += cfLift8(i_x, i_y);
					else
						prasTransformed->tPix(i_x, i_y) += cfLift4(i_x, i_y);

					b_x_odd_node = !b_x_odd_node;
				}

				b_y_odd_node = !b_y_odd_node;
			}

			--iLevel;
		}
		while (iDimScaling() != 1);

	}


	//******************************************************************************************
	void CTransform::Synthesise()
	{
		Assert(iDimScaling() == 1);

		do
		{
			++iLevel;

			int i_stride = iScalingStride();
			int i_x, i_y;

			bool b_x_odd_node = false;
			bool b_y_odd_node = false;

			for (i_y = 0; i_y < uSize; i_y += i_stride * 2)
			{
				for (i_x = 0; i_x < uSize; i_x += i_stride * 2)
				{
					if (b_x_odd_node == b_y_odd_node)
						prasTransformed->tPix(i_x, i_y) -= cfLift8(i_x, i_y);
					else
						prasTransformed->tPix(i_x, i_y) -= cfLift4(i_x, i_y);

					b_x_odd_node = !b_x_odd_node;
				}

				b_y_odd_node = !b_y_odd_node;
			}

			b_x_odd_node = false;
			b_y_odd_node = false;

			for (i_y = 0; i_y < uSize; i_y += i_stride * 2)
			{
				for (i_x = 0; i_x < uSize; i_x += i_stride * 2)
				{
					CCoef cf_predict;

					// Horizontal.
					cf_predict = cfPredict(i_x + i_stride, i_y);
					prasTransformed->tPix(i_x + i_stride, i_y) += cf_predict;

					// Vertical.
					cf_predict = cfPredict(i_x, i_y + i_stride);
					prasTransformed->tPix(i_x, i_y + i_stride) += cf_predict;

					// Diagonal.
					if (b_x_odd_node == b_y_odd_node)
					{
						cf_predict  = cfReadReflected(i_x + i_stride, i_y + i_stride,  i_stride,  i_stride);
						cf_predict += cfReadReflected(i_x + i_stride, i_y + i_stride, -i_stride, -i_stride);
					}
					else
					{
						cf_predict  = cfReadReflected(i_x + i_stride, i_y + i_stride,  i_stride, -i_stride);
						cf_predict += cfReadReflected(i_x + i_stride, i_y + i_stride, -i_stride,  i_stride);
					}

					cf_predict = cf_predict.cfPredict();

					prasTransformed->tPix(i_x + i_stride, i_y + i_stride) += cf_predict;

					b_x_odd_node = !b_x_odd_node;
				}

				b_y_odd_node = !b_y_odd_node;
			}
		}
		while (iDimScaling() != uSize);
	}


	//******************************************************************************************
	CCoef CTransform::cfReadReflected(int i_x, int i_y, int i_x_off, int i_y_off) const
	{
		uint u_r_x = uint(i_x + i_x_off);
		uint u_r_y = uint(i_y + i_y_off);

		if ((u_r_x & (uSize - 1)) != u_r_x)
			i_x_off = -i_x_off;
		
		if ((u_r_y & (uSize - 1)) != u_r_y)
			i_y_off = -i_y_off;

		CCoef cf_val = prasTransformed->tPix(i_x + i_x_off, i_y + i_y_off);

		Assert(cf_val != cfNAN);

		return cf_val;
	}


	//******************************************************************************************
	CCoef CTransform::cfPredict(int i_x, int i_y) const
	{
		int i_x_mod = i_x & iScalingStride();
		int i_y_mod = i_y & iScalingStride();

		Assert(i_x_mod != 0 || i_y_mod != 0);

		return (cfReadReflected(i_x, i_y,  i_x_mod,  i_y_mod) +
		        cfReadReflected(i_x, i_y, -i_x_mod, -i_y_mod)   ).cfPredict();
	}


	//******************************************************************************************
	CCoef CTransform::cfLift4(int i_x, int i_y) const
	{
		int i_pos_off = iScalingStride();

		CCoef cf_lift = cfZERO;

		cf_lift += cfReadReflected(i_x, i_y, 0,  i_pos_off);
		cf_lift += cfReadReflected(i_x, i_y, 0, -i_pos_off);

		cf_lift += cfReadReflected(i_x, i_y,  i_pos_off, 0);
		cf_lift += cfReadReflected(i_x, i_y, -i_pos_off, 0);

		return cf_lift.cfLift();
	}


	//******************************************************************************************
	CCoef CTransform::cfLift8(int i_x, int i_y) const
	{
		int i_pos_off = iScalingStride();

		CCoef cf_lift = cfZERO;

		cf_lift += cfReadReflected(i_x, i_y, 0,  i_pos_off);
		cf_lift += cfReadReflected(i_x, i_y, 0, -i_pos_off);

		cf_lift += cfReadReflected(i_x, i_y,  i_pos_off, 0);
		cf_lift += cfReadReflected(i_x, i_y, -i_pos_off, 0);

		cf_lift += cfReadReflected(i_x, i_y,  i_pos_off,  i_pos_off);
		cf_lift += cfReadReflected(i_x, i_y, -i_pos_off, -i_pos_off);

		cf_lift += cfReadReflected(i_x, i_y,  i_pos_off, -i_pos_off);
		cf_lift += cfReadReflected(i_x, i_y, -i_pos_off,  i_pos_off);

		return cf_lift.cfLift();
	}
};
