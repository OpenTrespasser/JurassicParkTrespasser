/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of ScreenPolygon.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ScreenPolygon.cpp                                        $
 * 
 * 4     6/18/98 6:30p Pkeet
 * Added code for printing out debug information.
 * 
 * 3     97/04/25 2:18p Pkeet
 * Altered code to use the new implementation of CLine2D.
 * 
 * 2     97/04/24 11:46a Pkeet
 * Debugged function and added code for debugging.
 * 
 **********************************************************************************************/

//
// Defines.
//

// Switch for debug text output.
#define bOUTPUT_POLYINTERSECT (0)
#define bVALIDATE_POLYLIST    (0)
#define bOUTPUT_POLYLIST      (1)

//
// Includes.
//
#include <algorithm>
#include <vector>
//#include "TempBuf.cpp"
#include "Common.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "ScreenRender.hpp"
#include "DepthSortTools.hpp"
#include "Line2D.hpp"
#include "Line.hpp"

#if bOUTPUT_POLYINTERSECT || bOUTPUT_POLYLIST
	#include "Lib/Sys/TextOut.hpp"
	CConsoleBuffer conPolys(120, 80);
#endif


//
// Module defines.
//

// Maximum number of vertices per polygon.
#define iMAX_POLY_VERTICES (40)

// Definition of a line segment array.
typedef std::vector<CLine2D> TVLines;

bool bDumpPolylist = false;


//
// Module functions.
//

#if bOUTPUT_POLYINTERSECT

//******************************************************************************************
	//
	void WriteLine(CLine2D& line)
	//
	// 
	//
	//**********************************
	{
		conPolys.Print("%1.1f, %1.1f :: %1.1f, %1.1f\n", line.v2From.tX, line.v2From.tY, line.v2To.tX,
				  line.v2To.tY);
	}

	#endif // bOUTPUT_POLYINTERSECT

//**********************************************************************************************
//
bool bLineSegCompare
(
	const CLine2D& ls_a,
	const CLine2D& ls_b
)
//
// 
//
//**********************************
{
	return ls_a.fMinY() < ls_b.fMinY();
}



//
// Function implementations.
//

//**********************************************************************************************
void AddLineSegment
(
    TVLines&          rtvl_left,
	TVLines&          rtvl_right,
	const CVector3<>& v3_from,
	const CVector3<>& v3_to
)
{
	// Build the line segment.
	CLine2D line2d(v3_from, v3_to);

	// Ignore the line segment if it is horizontal.
	if (line2d.bHorizontal())
		return;

	// Push to appropriate list.
	if (line2d.bLeft())
		rtvl_left.push_back(line2d);
	else
		rtvl_right.push_back(line2d);
		
}

//**********************************************************************************************
void AddLineSegments
(
    TVLines&              rtvl_left,
	TVLines&              rtvl_right,
	const CRenderPolygon* prpoly
)
{
	// Add line segments to the respective left and right lists.
	for (int i_vert = 0; i_vert < prpoly->paprvPolyVertices.uLen - 1; i_vert++)
	{
		AddLineSegment
		(
			rtvl_left,
			rtvl_right,
			prpoly->paprvPolyVertices[i_vert]->v3Screen,
			prpoly->paprvPolyVertices[i_vert + 1]->v3Screen
		);
	}
	AddLineSegment
	(
		rtvl_left,
		rtvl_right,
		prpoly->paprvPolyVertices[prpoly->paprvPolyVertices.uLen - 1]->v3Screen,
		prpoly->paprvPolyVertices[0]->v3Screen
	);

	#if bOUTPUT_POLYINTERSECT
		conPolys.Print("\nUnsorted:\n");
		for (int i = 0; i < Max(rtvl_left.size(), rtvl_right.size()); i++)
		{
			if (i < rtvl_left.size())
				conPolys.Print("%1.1f, %1.1f :: ", rtvl_left[i].v2From.tY, rtvl_left[i].v2To.tY);
			else
				conPolys.Print("xxxxxxxxxxxx :: ");
			if (i < rtvl_right.size())
				conPolys.Print("%1.1f, %1.1f\n", rtvl_right[i].v2From.tY, rtvl_right[i].v2To.tY);
			else
				conPolys.Print("\n");
		}
		conPolys.Print("\nSorted:\n");
	#endif // bOUTPUT_POLYINTERSECT

	// Sort the left and right lists.
	stable_sort(rtvl_left.begin(), rtvl_left.end(), bLineSegCompare);
	stable_sort(rtvl_right.begin(), rtvl_right.end(), bLineSegCompare);

	#if bOUTPUT_POLYINTERSECT
		for (i = 0; i < Max(rtvl_left.size(), rtvl_right.size()); i++)
		{
			if (i < rtvl_left.size())
				conPolys.Print("%1.1f, %1.1f :: ", rtvl_left[i].v2From.tY, rtvl_left[i].v2To.tY);
			else
				conPolys.Print("xxxxxxxxxxxx :: ");
			if (i < rtvl_right.size())
				conPolys.Print("%1.1f, %1.1f\n", rtvl_right[i].v2From.tY, rtvl_right[i].v2To.tY);
			else
				conPolys.Print("\n");
		}
		conPolys.Print("\n");
	#endif
}


//**********************************************************************************************
class CDualPolyIt
{
public:
	int i_a_left;
	int i_a_right;
	int i_b_left;
	int i_b_right;

	TVLines vline_a_left;
	TVLines vline_a_right;
	TVLines vline_b_left;
	TVLines vline_b_right;

	float fMinY;
	float fMaxY;
	float fNextY;
	float fStartY;

	//******************************************************************************************
	CDualPolyIt()
	{
		i_a_left  = 0;
		i_a_right = 0;
		i_b_left  = 0;
		i_b_right = 0;
	}

	//******************************************************************************************
	bool bDone()
	{
		if (i_a_left >= vline_a_left.size() || i_a_right >= vline_a_right.size() ||
			i_b_left >= vline_b_left.size() || i_b_right >= vline_b_right.size())
			return true;
		if (!bOverlap())
			return true;
		return false;
	}

	//******************************************************************************************
	float fMid()
	{
		return (fNextY - fStartY) * 0.0001f + fStartY;
	}

	//******************************************************************************************
	void FindMinY()
	{
		fMinY = Min(fFindMinYa(), fFindMinYb());
	}

	//******************************************************************************************
	void FindMaxY()
	{
		fMaxY = Max(fFindMaxYa(), fFindMaxYb());
	}

	//******************************************************************************************
	float fFindMinYa()
	{
		return Min(vline_a_left[i_a_left].fMinY(), vline_a_right[i_a_right].fMinY());
	}

	//******************************************************************************************
	float fFindMinYb()
	{
		return Min(vline_b_left[i_b_left].fMinY(), vline_b_right[i_b_right].fMinY());
	}

	//******************************************************************************************
	float fFindMaxYa()
	{
		return Max(vline_a_left[i_a_left].fMaxY(), vline_a_right[i_a_right].fMaxY());
	}

	//******************************************************************************************
	float fFindMaxYb()
	{
		return Max(vline_b_left[i_b_left].fMaxY(), vline_b_right[i_b_right].fMaxY());
	}
	
	//******************************************************************************************
	void FindStartY()
	{
		fStartY = Max(Max(vline_a_left[i_a_left].fMinY(), vline_a_right[i_a_right].fMinY()),
			          Max(vline_b_left[i_b_left].fMinY(), vline_b_right[i_b_right].fMinY()));
	}

	//******************************************************************************************
	void FindNextY()
	{
		fNextY = Min(Min(vline_a_left[i_a_left].fMaxY(), vline_a_right[i_a_right].fMaxY()),
			         Min(vline_b_left[i_b_left].fMaxY(), vline_b_right[i_b_right].fMaxY()));

		//
		// Look for the smallest Y intersection point in the region bounded by fStartY and
		// fNextY.
		//
		if (vline_a_left[i_a_left].bDoesIntersect(vline_b_left[i_b_left]))
		{
			float f_y_int = vline_a_left[i_a_left].v2GetIntersection().tY;
			if (f_y_int > fStartY)
				fNextY = Min(fNextY, f_y_int);
		}

		if (vline_a_left[i_a_left].bDoesIntersect(vline_b_right[i_b_right]))
		{
			float f_y_int = vline_a_left[i_a_left].v2GetIntersection().tY;
			if (f_y_int > fStartY)
				fNextY = Min(fNextY, f_y_int);
		}

		if (vline_a_right[i_a_right].bDoesIntersect(vline_b_left[i_b_left]))
		{
			float f_y_int = vline_a_right[i_a_right].v2GetIntersection().tY;
			if (f_y_int > fStartY)
				fNextY = Min(fNextY, f_y_int);
		}

		if (vline_a_right[i_a_right].bDoesIntersect(vline_b_right[i_b_right]))
		{
			float f_y_int = vline_a_right[i_a_right].v2GetIntersection().tY;
			if (f_y_int > fStartY)
				fNextY = Min(fNextY, f_y_int);
		}
	}

	//******************************************************************************************
	bool bOverlap()
	{
		if (fFindMinYa() >= fFindMaxYb())
			return false;
		if (fFindMinYb() >= fFindMaxYa())
			return false;
		return true;
	}

	//******************************************************************************************
	void IncrementPastNextY()
	{
		while (vline_a_left[i_a_left].fMaxY()   <= fNextY && i_a_left  < vline_a_left.size())
			i_a_left++;

		while (vline_a_right[i_a_right].fMaxY() <= fNextY && i_a_right < vline_a_right.size())
			i_a_right++;

		while (vline_b_left[i_b_left].fMaxY()   <= fNextY && i_b_left  < vline_b_left.size())
			i_b_left++;

		while (vline_b_right[i_b_right].fMaxY() <= fNextY && i_b_right < vline_b_right.size())
			i_b_right++;
	}

	//******************************************************************************************
	void IncrementToStartY()
	{
		while (vline_a_left[i_a_left].fMaxY()   <= fStartY && i_a_left  < vline_a_left.size())
			i_a_left++;

		while (vline_a_right[i_a_right].fMaxY() <= fStartY && i_a_right < vline_a_right.size())
			i_a_right++;

		while (vline_b_left[i_b_left].fMaxY()   <= fStartY && i_b_left  < vline_b_left.size())
			i_b_left++;

		while (vline_b_right[i_b_right].fMaxY() <= fStartY && i_b_right < vline_b_right.size())
			i_b_right++;
	}
};

//**********************************************************************************************
float fAreaTrapezoid(CLine2D& line_left, CLine2D& line_right, float f_y_start, float f_y_end)
{
	float f_area_sum = 0.0f;
	float f_area_tri = 0.0f;
	float f_dy          = f_y_end - f_y_start;
	float f_x_top_left  = line_left.fGetIntercept(f_y_start);
	float f_x_bot_left  = line_left.fGetIntercept(f_y_end);
	float f_x_top_right = line_right.fGetIntercept(f_y_start);
	float f_x_bot_right = line_right.fGetIntercept(f_y_end);

	#if bOUTPUT_POLYINTERSECT
		// Write the left and right lines.
		conPolys.Print("\nHeight: %1.1f\n", f_dy);
		conPolys.Print("Left: %1.1f, %1.1f\n", f_x_top_left, f_x_bot_left);
		conPolys.Print("Right: %1.1f, %1.1f\n", f_x_top_right, f_x_bot_right);
	#endif

	// Add the area of the left side.
	f_area_tri += Abs(f_x_top_left - f_x_bot_left) * f_dy * 0.5f;

	// Add the area of the right side.
	f_area_tri += Abs(f_x_top_right - f_x_bot_right) * f_dy * 0.5f;

	// Add the area of the rectangle.
	float f_x0 = Min(f_x_top_left, f_x_bot_left);
	float f_x1 = Max(f_x_top_right, f_x_bot_right);
	f_area_sum = (f_x1 - f_x0) * f_dy - f_area_tri;

	#if bOUTPUT_POLYINTERSECT
		conPolys.Print("Area: %1.1f\n", f_area_sum);
	#endif

	// Return the computed area.
	return f_area_sum;
}

//**********************************************************************************************
float fAreaIntersection(const CRenderPolygon* prpoly_a, const CRenderPolygon* prpoly_b)
{
	Assert(prpoly_a);
	Assert(prpoly_b);

	CDualPolyIt dpi;
	float       f_area = 0.0f;	// Sum of the sub areas.

	//
	// Create and sort the arrays of line segments.
	//
	AddLineSegments(dpi.vline_a_left, dpi.vline_a_right, prpoly_a);
	AddLineSegments(dpi.vline_b_left, dpi.vline_b_right, prpoly_b);

	//
	// The area is zero if one side of either polygon is missing or the polygons no
	// longer overlap.
	//
	if (dpi.bDone())
		return f_area;
	dpi.FindStartY();
	dpi.IncrementToStartY();
	dpi.FindNextY();
	for (;;)
	{
		if (dpi.bDone())
			return f_area;

		#if bOUTPUT_POLYINTERSECT
			// Debug output.
			conPolys.Print("\n\nY Segment: %1.5f, %1.5f\n", dpi.fStartY, dpi.fNextY);
		#endif // bOUTPUT_POLYINTERSECT

		//
		// Get the area of the seperate and combined trapezoids.
		//
		float f_area_a = fAreaTrapezoid
		(
			dpi.vline_a_left[dpi.i_a_left],
			dpi.vline_a_right[dpi.i_a_right],
			dpi.fStartY,
			dpi.fNextY
		);
		float f_area_b = fAreaTrapezoid
		(
			dpi.vline_b_left[dpi.i_b_left],
			dpi.vline_b_right[dpi.i_b_right],
			dpi.fStartY,
			dpi.fNextY
		);

		float f_max_area = Max(f_area_a, f_area_b);

		f_max_area = Max(f_max_area, fAreaTrapezoid
		(
			dpi.vline_a_left[dpi.i_a_left],
			dpi.vline_b_right[dpi.i_b_right],
			dpi.fStartY,
			dpi.fNextY
		));

		f_max_area = Max(f_max_area, fAreaTrapezoid
		(
			dpi.vline_b_left[dpi.i_b_left],
			dpi.vline_a_right[dpi.i_a_right],
			dpi.fStartY,
			dpi.fNextY
		));

		// Get the area of intersection (or negative area of non-intersection).
		float f_area_intersection = f_area_a + f_area_b - f_max_area;

		#if bOUTPUT_POLYINTERSECT
			conPolys.Print("Intersection Area: %1.1f\n", f_area_intersection);
		#endif // bOUTPUT_POLYINTERSECT

		// If there is an area of intersection, add it to the cumulative value.
		if (f_area_intersection > 0.0f)
			f_area += f_area_intersection;

		dpi.IncrementPastNextY();
		dpi.fStartY = dpi.fNextY;
		dpi.FindNextY();
	}

	// Return the area.
	return f_area;
}

#if bOUTPUT_POLYINTERSECT
	class TestPoly
	{
	public:
		//**************************************************************************************
		TestPoly()
		{
			conPolys.OpenFileSession("TestPoly.txt");
			OpenDepthSortDebug();

			// Create two dummy polygons.
			SRenderVertex* aprv_a[20];
			SRenderVertex* aprv_b[20];
			SRenderVertex  rv_a[20];
			SRenderVertex  rv_b[20];
			CRenderPolygon poly_a;
			CRenderPolygon poly_b;

			// Build the vertex arrays.
			for (int i_vert = 0; i_vert < 20; i_vert++)
			{
				aprv_a[i_vert] = &rv_a[i_vert];
				aprv_b[i_vert] = &rv_b[i_vert];
			}

			// Add the arrays to the polygon.
			poly_a.paprvPolyVertices.atArray = aprv_a;
			poly_b.paprvPolyVertices.atArray = aprv_b;
			poly_a.paprvPolyVertices.uLen    = 3;
			poly_b.paprvPolyVertices.uLen    = 4;

			// Set the values for polygon a.
			rv_a[0].v3Screen = CVector3<>(49.44099, 212.22241, 0.0f);
			rv_a[1].v3Screen = CVector3<>(20.76502, 197.86124, 0.0f);
			rv_a[2].v3Screen = CVector3<>(20.76502, 223.01630, 0.0f);
			//rv_a[3].v3Screen = CVector3<>(136.50629, 205.71118, 0.0f);

			// Set the values for polygon b.
			rv_b[0].v3Screen = CVector3<>(43.07402, 213.65799, 0.0f);
			rv_b[1].v3Screen = CVector3<>(0.49998, 223.25742, 0.0f);
			rv_b[2].v3Screen = CVector3<>(0.49998, 230.64423, 0.0f);
			rv_b[3].v3Screen = CVector3<>(20.76491, 223.01633, 0.0f);

			// Output the polygon values.
			WritePolygon(&poly_a);
			WritePolygon(&poly_b);

			// Find the area of intersection.
			float f_area_intersection = fAreaIntersection(&poly_a, &poly_b);
			conPolys.Print("\n\nThe area of intersection is: %1.20f\n\n", f_area_intersection);
			CloseDepthSortDebug();
			conPolys.CloseFileSession();
		}
	};

	TestPoly testpoly;
#endif // bOUTPUT_POLYINTERSECT


#if bVALIDATE_POLYLIST
	
	//******************************************************************************************
	void ValidatePolylist(CPArray<CRenderPolygon*>& parpoly)
	{
		if (d3dDriver.bUseD3D())
		{
			for (uint u = 0; u < parpoly.uLen; ++u)
			{
				AlwaysAssert(parpoly[u]);
				if (parpoly[u]->seterfFace[erfSOURCE_TERRAIN])
					AlwaysAssert(parpoly[u]->ehwHardwareFlags == ehwTerrain);
			}
		}
		else
		{
			for (uint u = 0; u < parpoly.uLen; ++u)
			{
				AlwaysAssert(parpoly[u]);
				AlwaysAssert(parpoly[u]->ehwHardwareFlags == ehwSoftware);
			}
		}
	}

#else // bVALIDATE_POLYLIST
	
	//******************************************************************************************
	void ValidatePolylist(CPArray<CRenderPolygon*>& parpoly)
	{
	}

#endif // bVALIDATE_POLYLIST

#if bOUTPUT_POLYLIST
	
	//******************************************************************************************
	void WriteCacheFlags(CSet<ERenderFeature> seterf)
	{
		if (seterf[erfCOPY])
			conPolys.Print("Copy ");
		if (seterf[erfLIGHT_SHADE])
			conPolys.Print("LS ");
		if (seterf[erfTEXTURE])
			conPolys.Print("Tex ");
		if (seterf[erfTRANSPARENT])
			conPolys.Print("Trans ");
		if (seterf[erfBUMP])
			conPolys.Print("Bump ");
		if (seterf[erfDRAW_CLIP])
			conPolys.Print("Clip ");
		if (seterf[erfALPHA_COLOUR])
			conPolys.Print("Alpha Col ");
		if (seterf[erfPERSPECTIVE])
			conPolys.Print("Persp ");
		if (seterf[erfFILTER])
			conPolys.Print("Filter ");
		if (seterf[erfFILTER_EDGES])
			conPolys.Print("Filter Edges ");
		if (seterf[erfSUBPIXEL])
			conPolys.Print("Sub Pix ");
		if (seterf[erfDITHER])
			conPolys.Print("Dither ");
		if (seterf[erfZ_BUFFER])
			conPolys.Print("Z Buffer ");
		if (seterf[erfTRAPEZOIDS])
			conPolys.Print("Trap ");
		if (seterf[erfRASTER_CLIP])
			conPolys.Print("RClip ");
		if (seterf[erfRASTER_CULL])
			conPolys.Print("RCull ");
		if (seterf[erfLIGHT])
			conPolys.Print("Lt ");
		if (seterf[erfFOG])
			conPolys.Print("Fog ");
		if (seterf[erfFOG_SHADE])
			conPolys.Print("Fog Shd ");
		if (seterf[erfSPECULAR])
			conPolys.Print("Spec ");
		if (seterf[erfCOLOURED_LIGHTS])
			conPolys.Print("Col Lights ");
		if (seterf[erfWIRE])
			conPolys.Print("Wire ");
		if (seterf[erfSOURCE_TERRAIN])
			conPolys.Print("Terr ");
		if (seterf[erfSOURCE_WATER])
			conPolys.Print("Water ");
		if (seterf[erfOCCLUDE])
			conPolys.Print("Occlude ");
		if (seterf[erfMIPMAP])
			conPolys.Print("Mip ");
		if (seterf[erfCURVED])
			conPolys.Print("Curved ");
		if (seterf[erfD3D_CACHE])
			conPolys.Print("D3D Cache ");
	}
	
	//******************************************************************************************
	void DumpPolylist(CPArray<CRenderPolygon*>& parpoly)
	{
		if (!bDumpPolylist)
			return;
		conPolys.OpenFileSession("PolyDump.txt");

		int i_locks = 0;
		int i_locks_sw_cache = 0;
		int i_hw = 0;
		int i_sw = 0;
		bool b_software_mode = parpoly[0]->ehwHardwareFlags == ehwSoftware;

		if (d3dDriver.bUseD3D())
		{
			for (uint u = 0; u < parpoly.uLen; ++u)
			{
				AlwaysAssert(parpoly[u]);

				bool b_sw_cache = false;

				switch (parpoly[u]->ehwHardwareFlags)
				{
					case ehwSoftware:
						{
							++i_sw;
							if (parpoly[u]->seterfFace[erfCOPY])
							{
								conPolys.Print("SW Cache     ");
								b_sw_cache = true;
							}
							else
								conPolys.Print("SW           ");
						}
						break;
					case ehwTerrain:
						++i_hw;
						conPolys.Print("HW Terrain   ");
						break;
					case ehwCache:
						++i_hw;
						conPolys.Print("HW Cache     ");
						break;
					case ehwWater:
						++i_hw;
						conPolys.Print("HW Water     ");
						break;
					case ehwAlphaCol:
						++i_hw;
						conPolys.Print("HW Alpha Col ");
						break;
					case ehwFill:
						++i_hw;
						conPolys.Print("HW Fill      ");
						break;
					default:
						conPolys.Print("************UNKNOWN POLYGON TYPE*************");
				}

				{
					// Texture size:
					rptr<CRaster> pras = parpoly[u]->ptexTexture->prasGetTexture();
					if (pras)
					{
						int i_tex_size = pras->iWidth * pras->iHeight;
						conPolys.Print("Txls: %ld \t", i_tex_size);
					}
					else
					{
						conPolys.Print("No Texture \t");
						if (parpoly[u]->ehwHardwareFlags == ehwSoftware)
							WriteCacheFlags(parpoly[u]->seterfFace);
					}
				}

				// Count mode switches.
				if (b_software_mode)
				{
					if (parpoly[u]->ehwHardwareFlags != ehwSoftware)
					{
						b_software_mode = false;
					}
				}
				else
				{
					if (parpoly[u]->ehwHardwareFlags == ehwSoftware)
					{
						++i_locks;
						b_software_mode = true;
						if (b_sw_cache)
						{
							++i_locks_sw_cache;
							conPolys.Print("*** Lock Due To SW Cache ***");
						}
						else
						{
							conPolys.Print("*** Lock ***");
						}
					}
				}

				conPolys.Print("\n");
			}

			{
				int i_lock_percent = 0;
				if (i_locks)
					i_lock_percent = (i_locks_sw_cache * 100) / i_locks;

				conPolys.Print("\n\n");
				conPolys.Print("Total Polygons:               %ld\n", parpoly.uLen);
				conPolys.Print("Locks Taken:                  %ld\n", i_locks);
				conPolys.Print("Locks Taken due to SW caches: %ld, %ld%%\n", i_locks_sw_cache, i_lock_percent);
			}
		}
		else
		{
			for (uint u = 0; u < parpoly.uLen; ++u)
			{
				AlwaysAssert(parpoly[u]);
				AlwaysAssert(parpoly[u]->ehwHardwareFlags == ehwSoftware);
				conPolys.Print("\n");
			}
		}
		
		conPolys.CloseFileSession();
		bDumpPolylist = false;
	}

#else // bOUTPUT_POLYLIST

	//******************************************************************************************
	void DumpPolylist(CPArray<CRenderPolygon*>& parpoly)
	{
	}

#endif // bOUTPUT_POLYLIST