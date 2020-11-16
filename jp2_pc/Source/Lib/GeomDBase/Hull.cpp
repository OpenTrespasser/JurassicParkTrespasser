/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of Hull.hpp
 *
 * Purpose:  A fast approximating convex hull algorithm based on the QuickHull algorithm.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/Hull.cpp                                                $
 * 
 * 2     98.03.17 4:25p Mmouni
 * Removed object name being passed in for debugging purposes.
 * 
 * 1     98.02.26 9:47p Mmouni
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Lib/GeomDBase/Plane.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include <list>
#include <vector>
#include <algorithm>
#include <memory.h>

#include <stdio.h>
#include <string.h>

#define DODEBUG(x)


//
// Usefull constants.
//
const float f_outside_tolerance		= 0.001;
const float f_colinear_tolerance	= 0.0001;	// 0.25 degrees
const float f_coincident_tolerance	= 0.001;


//
// Type equaivalent to pv3.
//
typedef CVector3<>* Vertex;


//
// A Facet of the convex hull.
//
class Facet
{
public:
	enum FacetFlags { Flipped = 1, Degenerate = 2 };

	Vertex			verts[3];		// Vertices of the facet.
	std::list<Vertex>	outside;		// Vertices outside the facet.
	CPlane			plane;			// Plane equation for the facet.
	int				flags;			// Flags for this facet.

	Facet()
	{
		verts[0] = 0;
		verts[1] = 0;
		verts[2] = 0;
		flags = 0;
	}

	Facet(Vertex vtx0, Vertex vtx1, Vertex vtx2) : plane(*vtx0, *vtx1, *vtx2)
	{
		verts[0] = vtx0;
		verts[1] = vtx1;
		verts[2] = vtx2;
		flags = 0;
	}

	//
	// Set plane so that a point v3 is inside.
	//
	void Orient(const CVector3<> &v3)
	{
		if (plane.rDistance(v3) > 0.0f)
		{
			// reverse plane if point is outside.
			plane = -plane;
			flags |= Flipped;
		}
	}

};


//
//
//
class Edge
{
public:
	Vertex vtx0, vtx1;
	std::list<Facet>::iterator facet0, facet1;

	Edge()
	{
		vtx0 = 0;
		vtx1 = 0;
	}

	Edge(Vertex v0, Vertex v1)
	{
		if (v0 < v1)
		{
			vtx0 = v0;
			vtx1 = v1;
		}
		else
		{
			vtx0 = v1;
			vtx1 = v0;
		}
	}

	bool operator< (const Edge& edge)
	{
		return (vtx0 < edge.vtx0 || (vtx0 == edge.vtx0 && vtx1 < edge.vtx1));
	}

	bool operator== (const Edge& edge)
	{
		return (vtx0 == edge.vtx0 && vtx1 == edge.vtx1);
	}
};


//
// A list of facets that keeps a list of edges so that adjacent facets can
// be found quickly.
//
class FacetList : public std::list<Facet>
{
private:
	// A vector of edges.
	std::vector<Edge> edges;

	// Return the iterator pointing to an edge.
	std::vector<Edge>::iterator FindEdge(Vertex vtx0, Vertex vtx1)
	{
		// Binary search for edge.
		std::vector<Edge>::iterator at = lower_bound(edges.begin(), edges.end(), Edge(vtx0, vtx1));

		Assert(*at == Edge(vtx0, vtx1));

		return at;
	}

	// Add an edge if it doesn't already exist.
	void AddEdge(Vertex vtx0, Vertex vtx1, iterator facet)
	{
		// Binary search for edge.
		std::vector<Edge>::iterator at = lower_bound(edges.begin(), edges.end(), Edge(vtx0, vtx1));

		if (at != edges.end() && *at == Edge(vtx0, vtx1))
		{
			// Found, make the edge point to the facet.
			if (at->facet0 == end())
				at->facet0 = facet;
			else if (at->facet1 == end())
				at->facet1 = facet;
			else
				Assert(0);
		}
		else
		{
			// Insert edge (in sorted order).
			std::vector<Edge>::iterator new_edge = edges.insert(at, Edge(vtx0, vtx1));
			new_edge->facet0 = facet;
			new_edge->facet1 = end();
		}
	}

	// Remove the pointer to this facet from this edge.
	void RemoveEdge(Vertex vtx0, Vertex vtx1, iterator facet)
	{
		// Binary search for edge.
		std::vector<Edge>::iterator at = lower_bound(edges.begin(), edges.end(), Edge(vtx0, vtx1));

		Assert(*at == Edge(vtx0, vtx1));

		// Remove the pointer to this facet.
		if (at->facet0 == facet)
			at->facet0 = end();
		else if (at->facet1 == facet)
			at->facet1 = end();
		else
			Assert(0);
	}

public:
	// Add a facet.
	void Add(Vertex vtx0, Vertex vtx1, Vertex vtx2)
	{
		// Add facet to list.
		push_front(Facet(vtx0, vtx1, vtx2));

		// Adjust edges for new facet.
		AddEdge(vtx0, vtx1, begin());
		AddEdge(vtx1, vtx2, begin());
		AddEdge(vtx2, vtx0, begin());
	}

	// Get adjacent facet indexed by index.
	iterator AdjacentFacet(iterator facet, int index1, int index2)
	{
		Assert(index1 >= 0 && index1 <= 2)
		Assert(index2 >= 0 && index2 <= 2)
		Assert(index1 != index2);

		// Lookup edge based on index.
		std::vector<Edge>::iterator edge = FindEdge((*facet).verts[index1], (*facet).verts[index2]);

		// Return the facet that is opposite this facet.
		if (edge->facet0 == facet)
		{
			Assert(edge->facet1 != end());
			return edge->facet1;
		}
		else
		{
			Assert(edge->facet0 != end());
			return edge->facet0;
		}

		// We should always have an adjacent facet.
		Assert(0);
		return end();
	}

	// Remove a facet.
	void Remove(iterator facet)
	{
		// Adjust edges for removed facet.
		RemoveEdge((*facet).verts[0], (*facet).verts[1], facet);
		RemoveEdge((*facet).verts[1], (*facet).verts[2], facet);
		RemoveEdge((*facet).verts[2], (*facet).verts[0], facet);

		// Remove from list.
		erase(facet);
	}

	// Reset the list.
	void reset()
	{
		erase(begin(), end());
		edges.erase(edges.begin(), edges.end());
	}
};


//
// A class to help finding points at each extreme.
//
class Extreme
{
public:
	float			fv;
	std::list<Vertex>	verts;

	Extreme()
	{
		fv = 0.0f;
	}

	Extreme(float f, Vertex v)
	{
		fv = f;
		verts.push_front(v);
	}

	void Set(float f, Vertex v)
	{
		fv = f;
		verts.push_front(v);
	}

	void SetMin(float f, Vertex v)
	{
		// Inside tolerance?
		if (f < fv + f_outside_tolerance)
		{
			// Way inside?
			if (f < fv - f_outside_tolerance)
			{
				fv = f;
				verts.erase(verts.begin(), verts.end());
			}

			verts.push_front(v);
		}
	}

	void SetMax(float f, Vertex v)
	{
		if (f > fv - f_outside_tolerance)
		{
			if (f > fv + f_outside_tolerance)
			{
				fv = f;
				verts.erase(verts.begin(), verts.end());
			}

			verts.push_front(v);
		}
	}
};


//
// for all unvisited neighbors N of facets in V
//    if p is above N
//      add N to V
//
bool find_visble_neighbors
(
	const FacetList::iterator &facet, 
	FacetList &facets, 
	Vertex vtx_p, 
	std::list<FacetList::iterator> &visible
)
{
	bool bIsVisible = 0;

	// Check if vertex is visible from this facet.
	if (((*facet).flags & Facet::Degenerate))
	{
		// Degenerate facet is visisble if either of it's neighors are.
		FacetList::iterator it_adj;
		std::list<FacetList::iterator>::iterator itit_this;

		// Add facet iterator to visible list.
		visible.push_front(facet);
		itit_this = visible.begin();

		// Recursively look at each neighboring facet, skipping facets already in the 
		// visible list.
		it_adj = facets.AdjacentFacet(facet, 0, 1);
		if (find(visible.begin(), visible.end(), it_adj) == visible.end())
		{
			if (find_visble_neighbors(it_adj, facets, vtx_p, visible))
				bIsVisible = 1;
		}

		it_adj = facets.AdjacentFacet(facet, 1, 2);
		if (find(visible.begin(), visible.end(), it_adj) == visible.end())
		{
			if (find_visble_neighbors(it_adj, facets, vtx_p, visible))
				bIsVisible = 1;
		}

		it_adj = facets.AdjacentFacet(facet, 2, 0);
		if (find(visible.begin(), visible.end(), it_adj) == visible.end())
		{
			if (find_visble_neighbors(it_adj, facets, vtx_p, visible))
				bIsVisible = 1;
		}

		if (!bIsVisible)
		{
			// Remove this facet from the list.
			visible.erase(itit_this);
		}
	}
	else if ((*facet).plane.rDistance(*vtx_p) > f_outside_tolerance)
	{
		FacetList::iterator it_adj;

		// Add facet iterator to visible list.
		visible.push_front(facet);
		bIsVisible = 1;

		// Recursively look at each neighboring facet, skipping facets already in the 
		// visible list.
		it_adj = facets.AdjacentFacet(facet, 0, 1);
		if (find(visible.begin(), visible.end(), it_adj) == visible.end())
			find_visble_neighbors(it_adj, facets, vtx_p, visible);

		it_adj = facets.AdjacentFacet(facet, 1, 2);
		if (find(visible.begin(), visible.end(), it_adj) == visible.end())
			find_visble_neighbors(it_adj, facets, vtx_p, visible);

		it_adj = facets.AdjacentFacet(facet, 2, 0);
		if (find(visible.begin(), visible.end(), it_adj) == visible.end())
			find_visble_neighbors(it_adj, facets, vtx_p, visible);
	}

	return bIsVisible;
}


//******************************************************************************************
bool bGoodPlane
(
	const Vertex vtx_a,			// The three vertices.
	const Vertex vtx_b, 
	const Vertex vtx_c
)
//
// Returns:
//		1 if the points form a plane.
//		0 if they are co-incident or co-linear.
//
//**********************************
{
	if (Fuzzy(*vtx_a, f_coincident_tolerance) == *vtx_b)
		return 0;

	if (Fuzzy(*vtx_a, f_coincident_tolerance) == *vtx_c)
		return 0;

	if (Fuzzy(*vtx_b, f_coincident_tolerance) == *vtx_c)
		return 0;

	// Check for co-linear points.
	float f_dot = CDir3<>(*vtx_c - *vtx_a) * CDir3<>(*vtx_c - *vtx_b);

	if (f_dot >= 1.0f - f_colinear_tolerance || f_dot <= -1.0f + f_colinear_tolerance)
		return 0;

	return 1;
}


//
// Writes the hull data out to a simple 3d file.
//
void DumpHullSob(char *str_name, CVector3<>* av3_pts, int i_num_pts, FacetList& facets)
{
	char str_buf[256];

	strcpy(str_buf, "C:\\Test_Data\\");
	strcat(str_buf, str_name);
	strcat(str_buf, ".sob");

	FILE *fp = fopen(str_buf, "w");

	if (fp)
	{
		fprintf(fp, "%s\n", str_name);

		fprintf(fp, "%d\n", i_num_pts);
		fprintf(fp, "%d\n", facets.size());

		for (int i = 0; i < i_num_pts; i++)
		{
			fprintf(fp, "%f %f %f\n", av3_pts[i].tX, av3_pts[i].tY, av3_pts[i].tZ);
		}

		for (FacetList::iterator it_fac = facets.begin(); it_fac != facets.end(); it_fac++)
		{
			if ((*it_fac).flags & Facet::Flipped)
				fprintf(fp, "%d %d %d\n", (*it_fac).verts[0] - av3_pts, 
										  (*it_fac).verts[1] - av3_pts, 
										  (*it_fac).verts[2] - av3_pts);
			else
				fprintf(fp, "%d %d %d\n", (*it_fac).verts[2] - av3_pts, 
										  (*it_fac).verts[1] - av3_pts, 
										  (*it_fac).verts[0] - av3_pts);
		}

		fclose(fp);
	}
}


//
// Writes the hull data out to a geo file.
//
void DumpHullGeo(char *str_name, CVector3<>* av3_pts, int i_num_pts, FacetList& facets)
{
	char str_buf[256];

	strcpy(str_buf, "C:\\Test_Data\\");
	strcat(str_buf, str_name);
	strcat(str_buf, ".geo");

	FILE *fp = fopen(str_buf, "w");

	if (fp)
	{
		fprintf(fp, "3DG1\n");

		fprintf(fp, "%d\n", i_num_pts);

		for (int i = 0; i < i_num_pts; i++)
		{
			fprintf(fp, "%f %f %f\n", av3_pts[i].tX, av3_pts[i].tY, av3_pts[i].tZ);
		}

		for (FacetList::iterator it_fac = facets.begin(); it_fac != facets.end(); it_fac++)
		{
			if ((*it_fac).flags & Facet::Flipped)
				fprintf(fp, "3 %d %d %d 15\n", (*it_fac).verts[2] - av3_pts, 
										  (*it_fac).verts[1] - av3_pts, 
										  (*it_fac).verts[0] - av3_pts);
			else
				fprintf(fp, "3 %d %d %d 15\n", (*it_fac).verts[0] - av3_pts, 
										  (*it_fac).verts[1] - av3_pts, 
										  (*it_fac).verts[2] - av3_pts);
		}

		fclose(fp);
	}
}


//
// Iterate through a set of points removing co-incident ones.
//
void RemoveDuplicatePoints
(
	CVector3<>* av3_pts, 
	int &i_num_pts
)
{
	int i = 0;
	for (;;)
	{
		// Do nothing if there is only one point in the set.
		if (i_num_pts <= 1)
			return;

		// Is the next point close to any of the remaining points.
		bool b_close = false;
		for (int i_rem = i + 1; i_rem < i_num_pts; i_rem++)
		{
			if (Fuzzy(av3_pts[i], 0.0125) == av3_pts[i_rem])
			{
				b_close = true;
				break;
			}
		}

		// If the point is close, replace it with the end point.
		if (b_close)
		{
			av3_pts[i] = av3_pts[i_num_pts - 1];
			--i_num_pts;
		}
		else
		{
			// Otherwise increment the current point.
			++i;
		}
		if (i >= int(i_num_pts) - 2)
			return;
	}
}


//**********************************************************************************************
//
int iQuickHull
(
	CVector3<>* av3_pts, 
	int i_num_pts
)
//
// Find the convex hull of a set of vertices using the quick hull algorithm.
//
// Returns the number of vertices in the reduced array or zero if the method
// fails to compute a convex hull.
//
//**********************************
{
	int i;
	FacetList facets;
	Extreme extremes[6];

	RemoveDuplicatePoints(av3_pts, i_num_pts);

	//
	// Get a list of points at each extreme.
	//
	extremes[0].Set(av3_pts[0].tX, &av3_pts[0]);
	extremes[1].Set(av3_pts[0].tY, &av3_pts[0]);
	extremes[2].Set(av3_pts[0].tZ, &av3_pts[0]);
	extremes[3].Set(av3_pts[0].tX, &av3_pts[0]);
	extremes[4].Set(av3_pts[0].tY, &av3_pts[0]);
	extremes[5].Set(av3_pts[0].tZ, &av3_pts[0]);

	for (i = 1; i < i_num_pts; i++)
	{
		extremes[0].SetMin(av3_pts[i].tX, &av3_pts[i]);
		extremes[1].SetMin(av3_pts[i].tY, &av3_pts[i]);
		extremes[2].SetMin(av3_pts[i].tZ, &av3_pts[i]);
		extremes[3].SetMax(av3_pts[i].tX, &av3_pts[i]);
		extremes[4].SetMax(av3_pts[i].tY, &av3_pts[i]);
		extremes[5].SetMax(av3_pts[i].tZ, &av3_pts[i]);
	}

	//
	// Create a four point simplex out of the six points with min/max co-ordinates.
	//
	Vertex vtx_simplex[4];
	std::list<Vertex> lvtx_dont_use;

	// Choose four different vertices.
	int i_use_extreme = 0;
	int i_simplex_cnt = 0;
	std::list<Vertex>::iterator it_vtx_ext = extremes[i_use_extreme].verts.begin();

	// Find vertices to use for the simplex.
	while (i_simplex_cnt < 4 && i_use_extreme < 6)
	{
		// Pick a point from the current extreme.
		vtx_simplex[i_simplex_cnt] = *it_vtx_ext;

		// Make sure it isn't in the don't use list.
		if (find(lvtx_dont_use.begin(), lvtx_dont_use.end(), *it_vtx_ext) == lvtx_dont_use.end())
		{
			// Keep this point.
			i_simplex_cnt++;

			// Add the points from this extreme to the don't use list.
			lvtx_dont_use.splice(lvtx_dont_use.end(), extremes[i_use_extreme].verts);

			// Go on to the next extreme.
			i_use_extreme++;

			if (i_use_extreme < 6)
				it_vtx_ext = extremes[i_use_extreme].verts.begin();
		}
		else
		{
			// Try the next point in this extreme.
			it_vtx_ext++;

			// If there is no next point...
			if (it_vtx_ext == extremes[i_use_extreme].verts.end())
			{
				// Go on to the next extreme.
				i_use_extreme++;

				if (i_use_extreme < 6)
					it_vtx_ext = extremes[i_use_extreme].verts.begin();
			}
		}
	}

	if (i_simplex_cnt < 4)
	{
		// Degenerate convex hull, either a triangle, line or point.
		CVector3<> av3_temp[3];

		// Not enough points to make a good initial hull.
		DODEBUG(dprintf("QuickHull Failed: not enough points for simplex\n"));

		return(0);
	}

	// Make faces out of all combinations of the four vertices.
	FacetList::iterator F;

	// add facet, reverse plane if "other" point is outside.
	facets.Add(vtx_simplex[0], vtx_simplex[1], vtx_simplex[2]);
	F = facets.begin();
	(*F).Orient(*vtx_simplex[3]);

	// add facet, reverse plane if "other" point is outside.
	facets.Add(vtx_simplex[0], vtx_simplex[1], vtx_simplex[3]);
	F = facets.begin();
	(*F).Orient(*vtx_simplex[2]);

	// add facet, reverse plane if "other" point is outside.
	facets.Add(vtx_simplex[0], vtx_simplex[2], vtx_simplex[3]);
	F = facets.begin();
	(*F).Orient(*vtx_simplex[1]);

	// add facet, reverse plane if "other" point is outside.
	facets.Add(vtx_simplex[1], vtx_simplex[2], vtx_simplex[3]);
	F = facets.begin();
	(*F).Orient(*vtx_simplex[0]);

	//
	// Setup the outside list for the simplex.
	//	
	char* ac_used = (char*)_alloca(sizeof(char) * i_num_pts);
	memset(ac_used, 0, sizeof(char) * i_num_pts);

	// Mark points of simplex as used.
	ac_used[vtx_simplex[0]  - av3_pts] = 1;
	ac_used[vtx_simplex[1]  - av3_pts] = 1;
	ac_used[vtx_simplex[2]  - av3_pts] = 1;
	ac_used[vtx_simplex[3]  - av3_pts] = 1;

#if (1)
	// for each facet F
	for (F = facets.begin(); F != facets.end(); F++)
	{
		float f_max_dist = 0.0f;

		// for each unassigned point p
		for (i = 0; i < i_num_pts; i++)
		{
			if (!ac_used[i])
			{
				// if p is above F
				float f_dist = (*F).plane.rDistance(av3_pts[i]);
				if (f_dist > f_outside_tolerance)
				{
					// assign p to F's outside set
					if (f_dist > f_max_dist)
					{
						// keep max point at the front of the list
						f_max_dist = f_dist;
						(*F).outside.push_front(&av3_pts[i]);
					}
					else
					{
						(*F).outside.push_back(&av3_pts[i]);
					}

					// mark point as used
					ac_used[i] = 1;
				}
			}
		}
	}
#else
	// for each unassigned point p
	for (i = 0; i < i_num_pts; i++)
	{
		if (!ac_used[i])
		{
			// Farthest distance this point is from any facet.
			float f_max_dist = f_outside_tolerance;

			// Facet that point is farthest outside.
			FacetList::iterator it_max_facet = facets.end();

			// for each facet F
			for (F = facets.begin(); F != facets.end(); F++)
			{
				// if p is above F
				float f_dist = (*F).plane.rDistance(av3_pts[i]);
				if (f_dist > f_max_dist)
				{
					// keep track of this facet.
					f_max_dist = f_dist;
					it_max_facet = F;
				}
			}

			if (it_max_facet != facets.end())
			{
				// mark point as used
				ac_used[i] = 1;

				// assign p to it_max_facet's outside set
				if (!(*it_max_facet).outside.empty())
				{
					float f_cur_max_dist = (*it_max_facet).plane.rDistance(*(*it_max_facet).outside.front());
					if (f_max_dist > f_cur_max_dist)
					{
						// keep max point at the front of the list
						(*it_max_facet).outside.push_front(&av3_pts[i]);
					}
					else
					{
						// put non-max points at end.
						(*it_max_facet).outside.push_back(&av3_pts[i]);
					}
				}
				else
				{
					// first point in outside list.
					(*it_max_facet).outside.push_front(&av3_pts[i]);
				}
			}
		}
	}
#endif

	//
	// Construct the convex hull by adding outside points.
	//
	int done = 0;
	while (!done)
	{
		done = 1;

		for (F = facets.begin(); F != facets.end(); F++)
		{
			// if F has a non-empty outside set
			if (!(*F).outside.empty())
			{
				done = 0;

				// select the furthest point p of F's outside set
				Vertex vtx_p = (*F).outside.front();
				(*F).outside.pop_front();

				DODEBUG(dprintf("QuickHull: add point %d to facet %d,%d,%d\n", vtx_p - av3_pts,
						(*F).verts[0] - av3_pts, (*F).verts[1] - av3_pts, (*F).verts[2] - av3_pts));
				
				// initialize the visible set V to F
				std::list<FacetList::iterator> V;
	
				// Find all the neighbors of F that P is visible from.
				find_visble_neighbors(F, facets, vtx_p, V);

				Assert(!V.empty())

				// the boundary of V is the set of horizon ridges H
				std::list<FacetList::iterator>::iterator itit_fac;
				std::list<Edge> H;

				for (itit_fac = V.begin(); itit_fac != V.end(); itit_fac++)
				{
					FacetList::iterator it_adj;

					// Find which adjacent facet(s) is(are) not in V.
					it_adj = facets.AdjacentFacet(*itit_fac, 0, 1);
					if (find(V.begin(), V.end(), it_adj) == V.end())
					{
						H.push_front(Edge((**itit_fac).verts[0], (**itit_fac).verts[1]));
					}

					it_adj = facets.AdjacentFacet(*itit_fac, 1, 2);
					if (find(V.begin(), V.end(), it_adj) == V.end())
					{
						H.push_front(Edge((**itit_fac).verts[1], (**itit_fac).verts[2]));
					}

					it_adj = facets.AdjacentFacet(*itit_fac, 2, 0);
					if (find(V.begin(), V.end(), it_adj) == V.end())
					{
						H.push_front(Edge((**itit_fac).verts[2], (**itit_fac).verts[0]));
					}
				}

#if (0)
				DODEBUG(dprintf("Horizon Poly:"));

				// Order horizon edges to form a polygon.
				list<Edge>::iterator it_edge_cur = H.begin();
				while (it_edge_cur != H.end())
				{
					list<Edge>::iterator it_edge_next = it_edge_cur;
					it_edge_next++;

					// find matching edge for it_edge_cur.
					for (list<Edge>::iterator it_edge = it_edge_next; it_edge != H.end(); it_edge++)
					{
						if ((*it_edge_cur).vtx1 == (*it_edge).vtx0)
						{
							// Swap it_edge and it_edge_next.
							iter_swap(it_edge, it_edge_next);
							break;
						}
						else if ((*it_edge_cur).vtx1 == (*it_edge).vtx1)
						{
							// Reverse edge.
							swap((*it_edge).vtx0, (*it_edge).vtx1);

							// Swap it_edge and it_edge_next.
							iter_swap(it_edge, it_edge_next);
							break;
						}
					}

					DODEBUG(dprintf(" %d", (*it_edge_cur).vtx0 - av3_pts));
					it_edge_cur++;
				}

				DODEBUG(dprintf("\n"));
#endif

				// delete the facets in V  
				// get all the points outside facets in V
				// do it now so that new faces get proper edges
				std::list<Vertex> outside;

				for (itit_fac = V.begin(); itit_fac != V.end(); itit_fac++)
				{
					// Move vertices outside facet into combined outside list.
					outside.splice(outside.begin(), (**itit_fac).outside);

					// Remove from facets list.
					facets.Remove(*itit_fac);
				}

				V.erase(V.begin(), V.end());

				// Compute pseudo centroid of polygon formed by horizon ridges H.
				std::list<Edge>::iterator R = H.begin();
				i = 1;

				CVector3<> v3_avg = *(*R).vtx0;
				v3_avg += *(*R).vtx1;

				R++;
				while (R != H.end())
				{
					v3_avg += *(*R).vtx0;
					v3_avg += *(*R).vtx1;
					R++;
					i++;
				}

				v3_avg /= (float)(i*2);

				// for each ridge R in H
				for (R = H.begin(); R != H.end(); R++)
				{
					FacetList::iterator it_new;

					// create a new facet from R and p
					facets.Add((*R).vtx0, (*R).vtx1, vtx_p);

					it_new = facets.begin();

					// orient facet correctly
					(*it_new).Orient(v3_avg);

					if (!bGoodPlane((*R).vtx0, (*R).vtx1, vtx_p))
					{
						// bad facet, flag it.
						(*it_new).flags |= Facet::Degenerate;

						DODEBUG(dprintf("Degenerate Facet (%d,%d,%d)\n", 
										(*F).verts[0] - av3_pts, 
										(*F).verts[1] - av3_pts, 
										(*F).verts[2] - av3_pts));
					}

					// keep new facet in a list.
					V.push_front(it_new);
				}

#if (0)
				// Re-compute planes for degenerate facets.
				for (itit_fac = V.begin(); itit_fac != V.end(); itit_fac++)
				{
					if (((**itit_fac).flags & Facet::Degenerate))
					{
						// Normal is average of adjacent normals.
						FacetList::iterator it_adj;
						CVector3<> v3_normal;

						it_adj = facets.AdjacentFacet(*itit_fac, 0, 1);
						v3_normal = (*it_adj).plane.d3Normal;

						it_adj = facets.AdjacentFacet(*itit_fac, 1, 2);
						v3_normal += (*it_adj).plane.d3Normal;

						it_adj = facets.AdjacentFacet(*itit_fac, 2, 0);
						v3_normal += (*it_adj).plane.d3Normal;

						v3_normal *= (1.0f/3.0f);
						
						// Use any point for the center of the plane. 
						(**itit_fac).plane.d3Normal = CDir3<>(v3_normal);
						(**itit_fac).plane.rD = -(*(**itit_fac).verts[0] * (**itit_fac).plane.d3Normal);
					}
				}
#endif

#if (1)
				// for each new facet F'
				for (itit_fac = V.begin(); itit_fac != V.end(); itit_fac++)
				{
					std::list<Vertex>::iterator it_vtx_next;
					float f_max_dist = 0.0f;

					// for each unassigned point q in an outside set of a facet V
					for (std::list<Vertex>::iterator it_vtx = outside.begin(); it_vtx != outside.end(); it_vtx = it_vtx_next)
					{
						it_vtx_next = it_vtx;
						it_vtx_next++;

						// if q is above F'
						float f_dist = (**itit_fac).plane.rDistance(**it_vtx);
						if (f_dist > f_outside_tolerance && 
							!((**itit_fac).flags & Facet::Degenerate))
						{
							if (f_dist > f_max_dist)
							{
								// assign q to F's outside set (at front of list).
								(**itit_fac).outside.splice((**itit_fac).outside.begin(), outside, it_vtx);

								f_max_dist = f_dist;
							}
							else
							{
								// assign q to F's outside set (at end of list).
								(**itit_fac).outside.splice((**itit_fac).outside.end(), outside, it_vtx);
							}
						}
					}
				}
#else
				// for each unassigned point q in an outside set of a facet in V
				list<Vertex>::iterator it_vtx_next;
				for (list<Vertex>::iterator it_vtx = outside.begin(); it_vtx != outside.end(); it_vtx = it_vtx_next)
				{
					it_vtx_next = it_vtx;
					it_vtx_next++;

					float f_max_dist = f_outside_tolerance;
					FacetList::iterator it_max_facet = facets.end();

					// for each new facet F'
					for (itit_fac = V.begin(); itit_fac != V.end(); itit_fac++)
					{
						// if q is above F'
						float f_dist = (**itit_fac).plane.rDistance(**it_vtx);
						if (f_dist > f_max_dist && 
							!((**itit_fac).flags & Facet::Degenerate))
						{
							// keep track of this facet.
							f_max_dist = f_dist;
							it_max_facet = *itit_fac;
						}
					}

					if (it_max_facet != facets.end())
					{
						if (!(*it_max_facet).outside.empty())
						{
							float f_cur_max_dist = (*it_max_facet).plane.rDistance(*(*it_max_facet).outside.front());
							if (f_max_dist > f_cur_max_dist)
							{
								// assign q to it_max_facet's outside set (at front of list).
								(*it_max_facet).outside.splice((*it_max_facet).outside.begin(), outside, it_vtx);
							}
							else
							{
								// assign q to it_max_facet's outside set (at end of list).
								(*it_max_facet).outside.splice((*it_max_facet).outside.end(), outside, it_vtx);
							}
						}
						else
						{
							// assign q to it_max_facet's outside set (at front of list).
							(*it_max_facet).outside.splice((*it_max_facet).outside.begin(), outside, it_vtx);
						}
					}
				}
#endif
				break;
			}
		}
	}

	// Dump information for the hull.
	DODEBUG(DumpHullGeo("Hull", av3_pts, i_num_pts, facets));

	//
	// Pack the vertices in the convex hull.
	//
	memset(ac_used, 0, sizeof(char) * i_num_pts);

	// Mark used vertices.
	for (F = facets.begin(); F != facets.end(); F++)
	{
		ac_used[(*F).verts[0] - av3_pts] = 1;
		ac_used[(*F).verts[1] - av3_pts] = 1;
		ac_used[(*F).verts[2] - av3_pts] = 1;
	}

	//
	// Test to make sure the Hull encloses all points.
	//
	for (F = facets.begin(); F != facets.end(); F++)
	{
		for (i = 0; i < i_num_pts; i++)
		{
			if ((*F).plane.rDistance(av3_pts[i]) > 10.0f * f_outside_tolerance &&
				!((*F).flags & Facet::Degenerate))
			{
				DODEBUG(dprintf("QuickHull Failed: point %d not enclosed by facet (%d,%d,%d), %f out\n", 
								i, (*F).verts[0] - av3_pts, (*F).verts[1] - av3_pts, (*F).verts[2] - av3_pts,
								(*F).plane.rDistance(av3_pts[i]) - f_outside_tolerance));

				// Oh well.
				return(0);
			}
		}
	}

	// Pack used vertices.
	int i_num_used = 0;
	for (i = 0; i < i_num_pts; i++)
	{
		if (ac_used[i])
			av3_pts[i_num_used++] = av3_pts[i];
	}

	DODEBUG(dprintf("QuickHull: points = %d\n", i_num_used));

	return(i_num_used);
}
