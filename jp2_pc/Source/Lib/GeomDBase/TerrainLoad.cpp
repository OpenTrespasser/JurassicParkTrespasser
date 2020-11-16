/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Implementation of TerrainLoad.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/TerrainLoad.cpp                                         $
 * 
 * 37    9/23/98 7:41p Mlange
 * Wavelet quad tree nodes now have a single pointer to the first descendant and a pointer to
 * the next sibling, instead of four pointers for each of the descendants.
 * 
 * 36    8/25/98 8:38p Rvande
 * Stream usage changed for MSL (MW build)
 * 
 * 35    7/21/98 8:00p Mlange
 * Can now refine the exported terrain triangulation to the limit of the minimum terrain texture
 * node size.
 * 
 * 34    4/23/98 4:35p Mlange
 * Wavelet quad tree data queries can now specify their frequence cutoff as a ratio of a node's
 * size.
 * 
 * 33    4/07/98 7:45p Agrant
 * Better messages for missing .wtd assert
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "TerrainLoad.hpp"

#include <iomanip>
#include "Lib/Sys/Textout.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeTForm.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeQuery.hpp"
#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/GeomDBase/WaveletDataFormat.hpp"
#include "Lib/GeomDBase/WaveletStaticData.hpp"
#include "Lib/Renderer/GeomTypes.hpp"

#if VER_TEST
#include <stdio.h>
#endif

//**********************************************************************************************
NMultiResolution::CTransformedDataHeader* ptdhLoadTerrainData(const std::string& str_terrain_name)
{
	using NMultiResolution::CTransformedDataHeader;
	using NMultiResolution::CQuadRootTForm;

	// Attempt to open a matching file with a .wtd extension.
	std::string str_transformed_filename = str_terrain_name + ".wtd";

	std::ifstream stream_transformed(str_transformed_filename.c_str(), std::ios::in | std::ios::_Nocreate | std::ios::binary);

	if (stream_transformed.fail())
	{
		// Failed to find wtd file!
#if VER_TEST
		char str_buffer[1024];
		sprintf(str_buffer, "%s\n\nMissing .wtd file: %s", __FILE__, str_transformed_filename.c_str());

		if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
			DebuggerBreak();
#endif
		return 0;
	}
	else
		return new CTransformedDataHeader(str_transformed_filename.c_str());
}


//**********************************************************************************************
void ConvertTerrainData(const std::string& str_terrain_name, int i_quant_bits, CConsoleBuffer& rcon_text_out)
{
	using NMultiResolution::CTransformedDataHeader;
	using NMultiResolution::CQuadRootTForm;
	using NMultiResolution::CQuadNodeTForm;
	using NMultiResolution::CQuadVertexTForm;

	// No .wtd file exists, so create one.
	rcon_text_out.SetActive(true);

	if (!rcon_text_out.bFileSessionIsOpen())
		rcon_text_out.OpenFileSession((str_terrain_name + "Info.txt").c_str(), true);

	rcon_text_out.Print("Converting terrain data\n");
	rcon_text_out.Print("=======================\n\n");

	CTerrainExportedData ted_orig((str_terrain_name + ".trr").c_str(), i_quant_bits, rcon_text_out);


	//
	// Transform terrain data.
	//
	CTransformedDataHeader* ptdh_transformed;

	// Allocate and set the fastheap used for allocations of the transform quad tree classes.
	CQuadRootTForm::AllocMemory();

	{
		CQuadRootTForm qntr_convert(ted_orig.ediGetInfo(), rcon_text_out);

		qntr_convert.AddPoints(ted_orig);
		qntr_convert.Analyse();

		ptdh_transformed = new CTransformedDataHeader(qntr_convert);
	}

	CQuadRootTForm::FreeMemory();

	//
	// Save data.
	//
	std::string str_transformed_filename = str_terrain_name + ".wtd";

	rcon_text_out.Print("\nFinal data size: %4.1fKB\n", ptdh_transformed->uDataBitSize() / (1024.0 * 8.0));
	rcon_text_out.Print("Output file    : %s\n", str_transformed_filename.c_str());
	rcon_text_out.Show();

	ptdh_transformed->Save(str_transformed_filename.c_str());

	delete ptdh_transformed;
}


namespace
{
	void ConformTriangulation(const NMultiResolution::CQuadRootQuery* pqnq_root, NMultiResolution::CQuadNodeQuery* pqnq_node, int i_min_size)
	{
		if (!pqnq_node->bHasDescendants() && pqnq_node->bSubdivideConforms())
		{
			int i_size_cmp;

			if (pqnq_node->stState[NMultiResolution::CQuadNodeQuery::estSIGNIFICANT])
				i_size_cmp = pqnq_node->ptqnGetTriangulate()->iGetSize();
			else
				i_size_cmp = pqnq_node->iGetSize() << 1;

			if (pqnq_node->iGetSize() > 1 && i_size_cmp > i_min_size)
				pqnq_node->Subdivide(pqnq_root);
		}

		if (pqnq_node->bHasDescendants())
		{
			NMultiResolution::CQuadNodeQuery* pqnq_dsc = pqnq_node->ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				ConformTriangulation(pqnq_root, pqnq_dsc, i_min_size);

				pqnq_dsc = pqnq_dsc->ptqnGetSibling();
			}
		}
	}
}

//**********************************************************************************************
void SaveTerrainTriangulation(const std::string& str_terrain_name, TReal r_freq_highpass, bool b_freq_as_ratio, bool b_conform, CConsoleBuffer& rcon_text_out)
{
	rcon_text_out.SetActive(true);

	if (!rcon_text_out.bFileSessionIsOpen())
		rcon_text_out.OpenFileSession((str_terrain_name + "TriInfo.txt").c_str(), true);

	rcon_text_out.Print("Saving triangulation data\n");
	rcon_text_out.Print("=========================\n\n");
	rcon_text_out.Print("Highpass cutoff %.1fcm\n\n", r_freq_highpass * 100.0);
	rcon_text_out.Show();

	using NMultiResolution::CQuadRootQuery;
	using NMultiResolution::CQuadNodeQuery;
	using NMultiResolution::CQuadVertexQuery;
	using NMultiResolution::CQuadVertexRecalc;
	using NMultiResolution::CQueryRect;

	// Load .wtd file.
	rcon_text_out.Print("Input file : %s\n", (str_terrain_name + ".wtd").c_str());
	rcon_text_out.Show();

	NMultiResolution::CTransformedDataHeader* ptdh_data = ptdhLoadTerrainData(str_terrain_name);


	// Allocate memory for the quad tree types.
	uint u_max_query_nodes = ptdh_data->iNumQuadNodes;
	uint u_max_query_verts = ptdh_data->iNumQuadVertices;

	CQuadRootQuery::AllocMemory(u_max_query_nodes * 2);

	CFastHeap* pfh_scaling_array = new CFastHeap(u_max_query_verts * dAVG_QUAD_VERTEX_TO_SCALING_ARRAY_RATIO);
	CQuadVertexRecalc::aaScalingAlloc.SetFastHeap(pfh_scaling_array);

	{
		//
		// Query the entire terrain.
		//
		CQuadRootQuery qnq_root(ptdh_data);

		// Set up a sufficiently large bounding sphere to contain the entire world.
		TReal r_radius = sqrt(Sqr(qnq_root.mpConversions.rcWorldSpaceExtents.tWidth()  / 1.5) +
							  Sqr(qnq_root.mpConversions.rcWorldSpaceExtents.tHeight() / 1.5)  );

		CBoundVolSphere bvs_query(r_radius);
		CPresence3<>    pr3_query(qnq_root.mpConversions.rcWorldSpaceExtents.v2Mid());

		CQueryRect qr_query(&qnq_root, bvs_query, pr3_query, r_freq_highpass, b_freq_as_ratio, u_max_query_nodes);

		if (b_conform)
		{
			ConformTriangulation
			(
				&qnq_root,
				&qnq_root,
				Max(1, iPosFloatCast(NMultiResolution::CQuadRootTIN::rvarMinDistRatioNodeSize * qnq_root.mpConversions.tlr2WorldToQuad.tlrX.tScale))
			);
		}

		//
		// Write the triangulation to a file.
		//

		std::string str_triangles_filename = str_terrain_name + ".tri";

		std::ofstream stream_tri(str_triangles_filename.c_str(), std::ios::out | std::ios::trunc);

		if (stream_tri.fail())
		{
			AlwaysAssert(false);
			return;
		}

		rcon_text_out.Print("Output file: %s\n\n", str_triangles_filename.c_str());
		rcon_text_out.Show();

		// Count the number of triangles.
		CQueryRect::CIterator it = qr_query.itBegin();

		uint u_num_tri = 0;

		for (; it; ++it)
			u_num_tri++;


		CDArray<const CQuadVertexQuery*> dapqvtq_used_verts(CQuadVertexQuery::uNumAlloc());

		// Write the triangles to the file.
		rcon_text_out.Print("Writing %d triangles\n", u_num_tri);
		rcon_text_out.Show();

		stream_tri << u_num_tri << std::endl;

		for (it.Reset(); it; ++it)
		{
			uint au_vert_indices[3];

			// Find corner vertex indices.
			for (int i_corner = 0; i_corner < 3; i_corner++)
			{
				// Determine if this corner vertex has already been added to the array of vertex pointers.
				const CQuadVertexQuery* pqvtq_curr = (*it)->pqvtGetVertex(i_corner);

				for (au_vert_indices[i_corner] = 0; au_vert_indices[i_corner] < dapqvtq_used_verts.uLen; au_vert_indices[i_corner]++)
					if (pqvtq_curr == dapqvtq_used_verts[au_vert_indices[i_corner]])
						break;

				// This is a new vertex, add it to the pointer array.
				if (au_vert_indices[i_corner] == dapqvtq_used_verts.uLen)
					dapqvtq_used_verts << pqvtq_curr;
			}

			stream_tri << au_vert_indices[0] << ' ';
			stream_tri << au_vert_indices[1] << ' ';
			stream_tri << au_vert_indices[2] << std::endl;
		}


		// Write the vertices to the file.
		rcon_text_out.Print("Writing %d vertices\n", dapqvtq_used_verts.uLen);
		rcon_text_out.Show();

		stream_tri << dapqvtq_used_verts.uLen << std::endl;
		stream_tri << std::setprecision(12);

		for (uint u_vt = 0; u_vt < dapqvtq_used_verts.uLen; u_vt++)
		{
			const CQuadVertexQuery* pqvt_curr = dapqvtq_used_verts[u_vt];

			stream_tri << pqvt_curr->v3World(qnq_root.mpConversions).tX << ' ';
			stream_tri << pqvt_curr->v3World(qnq_root.mpConversions).tY << ' ';
			stream_tri << pqvt_curr->v3World(qnq_root.mpConversions).tZ << std::endl;
		}
	}


	CQuadRootQuery::FreeMemory();
	CQuadVertexRecalc::aaScalingAlloc.SetFastHeap(0);

	delete pfh_scaling_array;

	delete ptdh_data;
}


//**********************************************************************************************
//
// CTerrainExportedData implementation.
//

	// Minimum XY grid resolution of terrain data in meters.
	static const TReal rMIN_XY_RESOLUTION = TReal(1.0);

	// Highest XY grid resolution allowed in terrain data in meters. Data points not at these grid
	// locations are snapped to the next lower grid location.
	static const TReal rMAX_XY_RESOLUTION = TReal(.125);

	//******************************************************************************************
	const char* const CTerrainExportedData::astrErrorStrings[] =
	{
		"Cannot open file",
		"No data in file",
		"Old terrain format",
		"Negative data value found",
		"Lower left point not defined",
		"Terrain resolution too high",
		"Attempt to read past end of file",
		"Error reading terrain file",
		"Point snapped to min grid resolution",
		"Extent forced to power of two",
		"Unknown error",
	};


	//******************************************************************************************
	CTerrainExportedData::CTerrainExportedData(const char* str_filename, int i_quant_bits, CConsoleBuffer& rcon_text_out)
	{
		for (int i = 0; i < eerrEND; i++)
			abErrorDialogDone[i] = false;

		rcon_text_out.Print("Input file: %s\n\n", str_filename);
		rcon_text_out.Show();

		streamFile.open(str_filename, std::ios::in | std::ios::_Nocreate);

		if (!streamFile)
		{
			Error(eerrCANNOT_OPEN_FILE);
			return;
		}


		Reset();

		if (streamFile.eof())
		{
			Error(eerrNO_DATA_IN_FILE);
			return;
		}

		if (dRead() == -1)
		{
			Error(eerrOLD_TERRAIN_FORMAT);
			return;
		}

		Reset();

		InitInfo(i_quant_bits, rcon_text_out);
	}

	//******************************************************************************************
	CTerrainExportedData::~CTerrainExportedData()
	{
		streamFile.close();
	}

	//******************************************************************************************
	void CTerrainExportedData::Reset()
	{
		streamFile.seekg(0);
		streamFile.clear();
	#ifndef __MWERKS__
		// MSL's stream classes do not need this behavior,
		// and the function isn't implemented.
		streamFile.ipfx(0);
	#endif
	}

	//******************************************************************************************
	CVector3<> CTerrainExportedData::v3Read()
	{
		double d_x = dRead();
		double d_y = dRead();
		double d_z = dRead();

		// Snap to the minimum grid resolution.
		double d_x_mod = fmod(d_x, double(rMAX_XY_RESOLUTION));
		double d_y_mod = fmod(d_y, double(rMAX_XY_RESOLUTION));

		if (d_x_mod != 0 || d_y_mod != 0)
		{
			Error(eerrSNAPPED_TO_GRID);
			d_x -= d_x_mod;
			d_y -= d_y_mod;
		}

		if (d_z < 0)
			Error(eerrNEG_DATA_VALUE);

		return CVector3<>(d_x, d_y, d_z);
	}


	//******************************************************************************************
	void CTerrainExportedData::InitInfo(int i_quant_bits, CConsoleBuffer& rcon_text_out)
	{
		const int iUPDATE_DELAY = 547;	// Specifies the console buffer update rate.


		//
		// Pass 1.
		//
		// Determine the actual number of data points in the file and the various minimum and
		// maximum values of these data points.
		//
		rcon_text_out.Print("Terrain data info\n");
		rcon_text_out.Print(" Pass 1\n");

		ediInfo.iNumQuantisationBits = i_quant_bits;

		Reset();

		CVector2<> v2_max(-FLT_MAX, -FLT_MAX);

		do
		{
			CVector3<> v3_val = v3Read();

			ediInfo.v2Min.tX   = Min(ediInfo.v2Min.tX,   v3_val.tX);
			ediInfo.v2Min.tY   = Min(ediInfo.v2Min.tY,   v3_val.tY);

			ediInfo.rMinHeight = Min(ediInfo.rMinHeight, v3_val.tZ);
			ediInfo.rMaxHeight = Max(ediInfo.rMaxHeight, v3_val.tZ);

			v2_max.tX = Max(v2_max.tX, v3_val.tX);
			v2_max.tY = Max(v2_max.tY, v3_val.tY);

			ediInfo.iNumDataPoints++;

			if ((ediInfo.iNumDataPoints - 1) % iUPDATE_DELAY == 0 || streamFile.eof())
			{
				if (ediInfo.iNumDataPoints != 1)
					rcon_text_out.MoveCursor(0, -5);

				rcon_text_out.Print("  Num data points    : %d\n", ediInfo.iNumDataPoints);
				rcon_text_out.Print("  Min world XY       : %gm, %gm    \n", ediInfo.v2Min.tX,   ediInfo.v2Min.tY);
				rcon_text_out.Print("  Min/max heights    : %gm, %gm    \n", ediInfo.rMinHeight, ediInfo.rMaxHeight);
				rcon_text_out.Print("  Max quantise error : %2.2fcm    \n", (ediInfo.rMaxHeight / (1 << ediInfo.iNumQuantisationBits)) / 2 * 100.0);
				rcon_text_out.Print("  World extent XY    : %gm, %gm    \n", (v2_max - ediInfo.v2Min).tX, (v2_max - ediInfo.v2Min).tY);
				rcon_text_out.Show();
			}
		}
		while (!streamFile.eof());

		//
		// Pass 2.
		//
		// Determine the maximum resolution of the terrain data in the XY plane, relative to the
		// minimum XY extent of the terrain data.
		//
		Reset();

		// The file must contain an actual data point at the minimum XY extent. The wavelet transform
		// requires this to be able to interpolate the 'missing' data points.
		bool b_lower_left_found = false;

		int i_lsb_min = TypeMax(int);

		for (int i_pt = 0; i_pt < ediInfo.iNumDataPoints; i_pt++)
		{
			CVector2<> v2_val = v3Read() - ediInfo.v2Min;

			b_lower_left_found |= v2_val.tX == 0 && v2_val.tY == 0;

			if (v2_val.tX != 0)
				i_lsb_min = Min(CIntFloat(v2_val.tX).iGetFixedpointLSB(), i_lsb_min);

			if (v2_val.tY != 0)
				i_lsb_min = Min(CIntFloat(v2_val.tY).iGetFixedpointLSB(), i_lsb_min);

			CVector2<> v2_extent = v2_max - ediInfo.v2Min;

			ediInfo.rHighestRes = Min(rMIN_XY_RESOLUTION, pow(2.0, i_lsb_min));
			ediInfo.iMaxIndices = NextPowerOfTwo(iRound(Max(v2_extent.tX, v2_extent.tY) / ediInfo.rHighestRes));

			if ((i_pt % iUPDATE_DELAY == 0) || (i_pt == ediInfo.iNumDataPoints - 1))
			{
				if (i_pt != 0)
					rcon_text_out.MoveCursor(0, -4);

				rcon_text_out.Print(" Pass 2  (%d)     \n", ediInfo.iNumDataPoints - i_pt - 1);
				rcon_text_out.Print("  Max resolution     : %.4gm    \n", ediInfo.rHighestRes);
				rcon_text_out.Print("  Max ^2 world extent: %gm    \n",   ediInfo.iMaxIndices * ediInfo.rHighestRes);
				rcon_text_out.Print("  Max indices        : %d\n",  ediInfo.iMaxIndices);
				rcon_text_out.Show();
			}
		}

		if ((v2_max - ediInfo.v2Min).tX != ediInfo.iMaxIndices * ediInfo.rHighestRes ||
		    (v2_max - ediInfo.v2Min).tY != ediInfo.iMaxIndices * ediInfo.rHighestRes   )
			Error(eerrFORCED_EXTENT_POWER_TWO);

		if (!b_lower_left_found)
			Error(eerrNO_LOWER_LEFT_POINT);

		if (ediInfo.iMaxIndices > iMAX_TRANSFORM_INDICES)
			Error(eerrRESOLUTION_TOO_HIGH);

		rcon_text_out.Print("\n");
		rcon_text_out.Show();
	}

	//******************************************************************************************
	double CTerrainExportedData::dRead()
	{
		if (streamFile.eof())
		{
			Error(eerrREAD_PAST_END);
			return FLT_MAX;
		}

		// Read value and skip whitespace to the next value in the stream.
		double d_val = FLT_MAX;

		streamFile >> d_val;
	#ifndef __MWERKS__
		streamFile.ipfx(0);
	#endif

		if (streamFile.fail() && !streamFile.eof())
		{
			Error(eerrERROR_READING_FILE);
			return FLT_MAX;
		}

		return d_val;
	}


	//******************************************************************************************
	void CTerrainExportedData::Error(EError eerr)
	{
		if (eerr >= eerrEND)
			eerr = eerrUNKNOWN;

		if (!abErrorDialogDone[eerr])
		{
			Warning(astrErrorStrings[eerr], "Terrain Error");
			abErrorDialogDone[eerr] = true;
		}
	}
