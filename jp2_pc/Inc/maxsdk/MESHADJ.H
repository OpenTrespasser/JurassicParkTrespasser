/**********************************************************************
 *<
	FILE: meshadj.h

	DESCRIPTION: Adjacency list for meshes.

	CREATED BY: Rolf Berteig

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __MESHADJ__
#define __MESHADJ__

#include "export.h"

#define UNDEFINED	0xffffffff

class MEdge {
	public:
		DWORD f[2];
		DWORD v[2];		
	};

class AdjEdgeList {
	public:
		DWORDTab *list; 	// 1 DWORDTab per vertex. The Tab is a list of indices into the edge list, 1 for each edge adjacent to the vertex
		Tab<MEdge> edges;	// Table of edges
		int nverts;			// size of 'list'.

		DllExport AdjEdgeList(Mesh& amesh);
		DllExport ~AdjEdgeList();
		AdjEdgeList& operator=(AdjEdgeList& a) {assert(0);return *this;}
		DllExport void AddEdge( DWORD fi, DWORD v1, DWORD v2 );
		DWORDTab& operator[](int i) { return list[i]; }
		DllExport int FindEdge(DWORD v0, DWORD v1);
		DllExport int FindEdge(DWORDTab& vmap,DWORD v0, DWORD v1);		
		DllExport void TransferEdges(DWORD from,DWORD to,DWORD except1,DWORD except2,DWORD del);
		DllExport void RemoveEdge(DWORD from,DWORD e);
	};

class AdjFace {
	public:
		DWORD f[3];
		AdjFace() {f[0] = f[1] = f[2] = UNDEFINED;}
	};

class AdjFaceList {
	public:
		Tab<AdjFace> list;

		AdjFace& operator[](int i) {return list[i];}
		DllExport AdjFaceList(Mesh& mesh,AdjEdgeList& el);
	};

class FaceElementList {
	public:
		// For each face, which element is it in
		DWORDTab elem;
		DWORD count;
		DllExport FaceElementList(Mesh &mesh,AdjFaceList& af);
		DWORD operator[](int i) {return elem[i];}
	};

class FaceClusterList {
	public:
		// Cluster #, one for each face - non-selected faces have UNDEFINED for their id.
		DWORDTab clust;
		DWORD count;
	
		// This version separates cluster also using a minimum angle and optionally the selection set
		DllExport FaceClusterList(Mesh *mesh, AdjFaceList& adj,float angle,BOOL useSel=TRUE);		

		// Uses selection set
		DllExport FaceClusterList(BitArray& fsel, AdjFaceList& adj);		
				
		DWORD operator[](int i) { return clust[i]; }		
	};

#endif //__MESHADJ__
