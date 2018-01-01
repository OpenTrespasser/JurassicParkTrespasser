/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Node History classes for AI library.
 *		
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/NodeHistory.hpp                                               $
 * 
 * 13    8/23/98 3:34a Agrant
 * Random pathfinding nodes.
 * 
 * 12    8/11/98 2:19p Agrant
 * much hacking to keep nodes current with influences
 * 
 * 11    9/09/97 9:04p Agrant
 * added validation process to nodes
 * removed unused old influence nodes and old nodesource nodes
 * improved behavior of moving influences
 * 
 * 10    7/31/97 4:41p Agrant
 * Pathfinding over objects begins
 * 
 * 9     7/14/97 12:57a Agrant
 * Added NodeSource histories.
 * Support unknown nodes.
 * 
 * 8     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 7     4/01/97 1:14p Agrant
 * Better influence forgetting code
 * 
 * 6     3/28/97 2:00p Agrant
 * Brain now points to graph, mentalstate, and worldview
 * Pathfinding more efficient
 * 
 * 5     3/26/97 5:31p Agrant
 * Added the invalid node history to describe invalid nodes.
 * 
 * 4     2/16/97 5:37p Agrant
 * Even better pathfinding.
 * 
 * 3     2/12/97 7:32p Agrant
 * Major pathfinding revision complete- animals construct a set of 
 * nodes which they use for pathfinding, and update the set interactively
 * based on the physical geometry of the objects around them.
 * 
 * 2     2/11/97 7:14p Agrant
 * 
 * 1     2/11/97 7:11p Agrant
 * History classes for nodes in the pathfinding graph.
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_NODEHISTORY_HPP
#define HEADER_GAME_AI_NODEHISTORY_HPP

#include "Lib/Sys/Timer.hpp"
#include "Lib/Std/Set.hpp"


class CNodeHistory	;
class CNHStart		;
class CNHStop		;
class CNHInfluence	;
class CNHInfluenceOld;
class CNHInfluenceSurface;
class CNHRandom	;
class CNHDesigner;
class CNHUnknown;
class CNHInvalid;
class CNHNodeSource	;
class CNHNodeSourceOld;

class CAIGraphNode;
class CAIGraph;
class CInfluence;
class CInstance;
class CNodeSource;

#define fBAD 100000.0f

enum ENodeFlags 
// Prefix: enf
{
	enfVALIDATED = 0,	// True once the node has been validated.  Some nodes must gather
						// extra data after creation.
	enfDISCARDABLE,		// True if the node ought to be discarded.
	enfEND
};


//lint -save -e1712   // its okay that these guys have no default constructor.

//*********************************************************************************************
//
class CNodeHistory
//
//	Prefix: nh
//
//	Base class for all node history classes.
//
//	Notes:
//		No destructor for a NodeHistory is ever called.  Do not rely on a destructor!
//		Any pointers to a NodeHistory may change without notice as the nodes themselves are
//		managed by STL.
//		See CAIGraphNode.
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//
	CSet<ENodeFlags> setFlags;		// Flags that every node shares.

//
//  Member function definitions
//

	//*************************************************************************************
	//
	virtual float fRate
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	//
	//	Examines the node to determine its importance.
	//
	//	Returns:
	//		A rating of the node's importance.  0 is vital, higher numbers are less vital.
	//
	//
	//******************************
	{
		Assert(false);

		if (!setFlags[enfVALIDATED])
			Validate(paig, i_node_index);
		
		return fBAD;
	}

	//*************************************************************************************
	//
	virtual void WasUsed
	(
		CAIGraphNode*,	//paig,		// The node to update.
		TSec			//s_when_used // The time the node was used.
	);
	//
	//	Updates this and paig to allow for its recent use.
	//
	//	Notes:
	//		Updates the last time used data and updates any other data particular to a
	//		a particular NodeHistory.
	//
	//******************************

	//*************************************************************************************
	//
	virtual void Delete
	(
		CAIGraph*,	//paig,			// The graph the node is in.
		int			//i_node_index	// The index of the node.
	);
	//
	//	Updates any external data structs that need it when a node is deleted.
	//	Updates the graph to which the node belongs.
	//
	//	Notes:
	//		This function is necessary, because node histories are never actually deconstructed
	//		or "delete" deleted.
	//
	//******************************

	//*************************************************************************************
	//
	virtual bool bReferences
	(
		const CInstance*	//pins
	) const
	//
	//	Returns true if the node relies upon pins, else false
	//
	//
	//******************************
	{
		return false;
	}

	//*************************************************************************************
	//
	virtual CNodeSource* pnsReferences
	(
	)
	//
	//	Returns the node source that the node relies upon.  Returns NULL if none.
	//
	//******************************
	{
		return 0;
	}

	//*************************************************************************************
	//
	virtual void Validate
	(
		CAIGraph*,	//paig,			// The graph the node is in.
		int			//i_node_index	// The index of the node.
	);
	//
	//	Validates the node.  Called by fRate the first time fRate is called.
	//
	//  Notes:
	//		Sets the enfVALIDATED flag.
	//
	//******************************

	//
	//  Cast functions.
	//
	
	virtual CNHStart*			pCastNHStart()			{ return 0; };
	virtual CNHStop*			pCastNHStop()			{ return 0; };
	virtual CNHInfluence*		pCastNHInfluence()		{ return 0; };
	virtual CNHInfluenceOld*	pCastNHInfluenceOld()	{ return 0; };
	virtual CNHInfluenceSurface*pCastNHInfluenceSurface()	{ return 0; };
	virtual CNHDesigner*		pCastNHDesigner()		{ return 0; };
	virtual CNHRandom*			pCastNHRandom()			{ return 0; };
	virtual CNHUnknown*			pCastNHUnknown()		{ return 0; };
	virtual CNHInvalid*			pCastNHInvalid()		{ return 0; };
	virtual CNHNodeSource*		pCastNHNodeSource()		{ return 0; };
	virtual CNHNodeSourceOld*	pCastNHNodeSourceOld()	{ return 0; };
};

//*********************************************************************************************
//
class CNHStart : public CNodeHistory
//
//	Prefix: nhsn
//
//	The node representing the animal's current location.
//
//	Notes:
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//

//
//  Member function definitions
//

//
//  Overrrides.
//

	//*************************************************************************************
	//
	virtual float fRate
	(
		CAIGraph*,	//paig,			// The graph the node is in.
		int			//i_node_index	// The index of the node.
	)
	//
	//******************************
	{
		return -1;
	}

	//*************************************************************************************
	//
	virtual void WasUsed
	(
		CAIGraphNode*,	//paig,		// The node to update.
		TSec			//s_when_used // The time the node was used.
	)
	//
	//******************************
	{
	}

//
//  Cast functions.
//
	virtual CNHStart*			pCastNHStart()			{ return this; };
};

//*********************************************************************************************
//
class CNHStop : public CNodeHistory
//
//	Prefix: nhsn
//
//	The node representing the animal's destination.
//
//	Notes:
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//

//
//  Member function definitions
//

//
//  Overrrides.
//

	//*************************************************************************************
	//
	virtual float fRate
	(
		CAIGraph*,	//paig,			// The graph the node is in.
		int			//i_node_index	// The index of the node.
	)
	//
	//	Examines aign to determine its importance.
	//
	//	Returns:
	//		A rating of aign's importance.  0 is vital, higher numbers are less vital.
	//
	//
	//******************************
	{
		return -1;
	}

	//*************************************************************************************
	//
	virtual void WasUsed
	(
		CAIGraphNode*,	//paig,		// The node to update.
		TSec			//s_when_used // The time the node was used.
	)
	//
	//******************************
	{
	}

//
//  Cast functions.
//

	virtual CNHStop*			pCastNHStop()			{ return this; };

};

//*********************************************************************************************
//
class CNHNodeSource : public CNodeHistory
//
//	Prefix: nhns
//
//	A node created by analyzing the silhouette of a node source.
//
//	Notes:
//		The node source has not changed since the creation of the node.
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//
	CNodeSource*	pnsNodeSource;	// The source used to generate the node.

//
//  Member function definitions
//

	CNHNodeSource(CNodeSource *pns);

//
//  Overrides.
//

	//*************************************************************************************
	//
	virtual float fRate
	(
		CAIGraph*,	//paig,			// The graph the node is in.
		int			//i_node_index	// The index of the node.
	);
	//******************************

	//*************************************************************************************
	//
	virtual void Delete
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	);
	//******************************

	//*************************************************************************************
	//
	virtual void WasUsed
	(
		CAIGraphNode*	paig,		// The node to update.
		TSec			s_when_used // The time the node was used.
	);
	//******************************

	//*************************************************************************************
	virtual CNodeSource* pnsReferences();

//
//  Cast functions.
//
	virtual CNHNodeSource*		pCastNHNodeSource()		{ return this; };
};

#ifdef TRACK_OLD_NODES
//*********************************************************************************************
//
class CNHNodeSourceOld : public CNHNodeSource
//
//	Prefix: nhnso
//
//	A node which was once a node source silhouette node, but the source has moved since the node was
//		created.
//
//	Notes:
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//

//
//  Member function definitions
//

	CNHNodeSourceOld(CNodeSource *pinf);
	
	//*************************************************************************************
	//
	virtual float fRate
	(
		CAIGraph*,	//paig,			// The graph the node is in.
		int			//i_node_index	// The index of the node.
	);
	//******************************

	//*************************************************************************************
	//
	virtual void Delete
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	);
	//******************************

	//*************************************************************************************
	//
	virtual void WasUsed
	(
		CAIGraphNode*	paig,		// The node to update.
		TSec			s_when_used // The time the node was used.
	);
	//******************************

	//*************************************************************************************
	virtual CNodeSource* pnsReferences();

//
//  Cast functions.
//
	virtual CNHNodeSourceOld*	pCastNHSilhouetteOld()	{ return this; };
};
#endif


//*********************************************************************************************
//
class CNHInfluence : public CNodeHistory
//
//	Prefix: nhi
//
//	A node created by analyzing the silhouette of an influence.
//
//	Notes:
//		The influence has not moved since the creation of the node.
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//
	CInfluence*	pinfInfluence;	// The influence used to generate the node.

//
//  Member function definitions
//

	CNHInfluence(CInfluence *pinf);

//
//  Overrides.
//

	//*************************************************************************************
	//
	virtual float fRate
	(
		CAIGraph*,	//paig,			// The graph the node is in.
		int			//i_node_index	// The index of the node.
	);
	//******************************

	//*************************************************************************************
	//
	virtual void Delete
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	);
	//******************************

	//*************************************************************************************
	//
	virtual void WasUsed
	(
		CAIGraphNode*	paig,		// The node to update.
		TSec			s_when_used // The time the node was used.
	);
	//******************************

	//*************************************************************************************
	//
	virtual bool bReferences
	(
		const CInstance*	pins
	) const;
	//******************************

	//*************************************************************************************
	virtual CNodeSource* pnsReferences();
//
//  Cast functions.
//
	virtual CNHInfluence*		pCastNHInfluence()		{ return this; };
};

#ifdef TRACK_OLD_NODES
//*********************************************************************************************
//
class CNHInfluenceOld : public CNodeHistory
//
//	Prefix: nhio
//
//	A node which was once a silhouette node, but the influence has moved since the node was
//		created.
//
//	Notes:
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//

	CInfluence*	pinfInfluence;	// The influence used to generate the node.

//
//  Member function definitions
//

	CNHInfluenceOld(CInfluence *pinf);
	
	//*************************************************************************************
	//
	virtual float fRate
	(
		CAIGraph*,	//paig,			// The graph the node is in.
		int			//i_node_index	// The index of the node.
	);
	//******************************

	//*************************************************************************************
	//
	virtual void Delete
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	);
	//******************************

	//*************************************************************************************
	//
	virtual void WasUsed
	(
		CAIGraphNode*	paig,		// The node to update.
		TSec			s_when_used // The time the node was used.
	);
	//******************************

	//*************************************************************************************
	//
	virtual bool bReferences
	(
		const CInstance*	pins
	) const;
	//******************************

	//*************************************************************************************
	virtual CNodeSource* pnsReferences();
//
//  Cast functions.
//
	virtual CNHInfluenceOld*	pCastNHSilhouetteOld()	{ return this; };
};
#endif

//*********************************************************************************************
//
class CNHInfluenceSurface : public CNHInfluence
//
//	Prefix: nhis
//
//	A node created by analyzing the surface of an influence.
//
//	Notes:
//		The influence has not moved since the creation of the node.
//
//*********************************************************************************************
{
public:
	
	//  If the surface of the node is the same as the influence itself, 
	//  the node is physically on the influence.

	//  If the surface is not the same as the influence, then the node was added explcitly 
	//  for the purpose of providing access to the surface of the influence.  If it is not doing
	//  that, then it is most likely a wasted node.

	CNHInfluenceSurface(CInfluence *pinf);


	//
	//  Overrides.
	//

	//*************************************************************************************
	//
	virtual void Validate
	(
		CAIGraph*,	//paig,			// The graph the node is in.
		int			//i_node_index	// The index of the node.
	);
	//******************************

	//*************************************************************************************
	//
	virtual void Delete
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	);
	//******************************

	virtual CNHInfluenceSurface*pCastNHInfluenceSurface()	{ return this; };

};

//*********************************************************************************************
//
class CNHRandom : public CNodeHistory
//
//	Prefix: nhrn
//
//	A node created randomly.
//
//	Notes:
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//

//
//  Member function definitions
//

//
//  Overrides.
//

	//*************************************************************************************
	virtual float fRate(CAIGraph*	paig, int i_node_index);

	//*************************************************************************************
	virtual void Delete(CAIGraph*	paig, int i_node_index);

//
//  Cast functions.
//
	virtual CNHRandom*			pCastNHRandom()			{ return this; };
};

//*********************************************************************************************
//
class CNHDesigner : public CNodeHistory
//
//	Prefix: nhd
//
//	A node handed down by God.
//
//	Notes:
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//

//
//  Member function definitions
//

//
//  Overrides.
//

	//*************************************************************************************
	//
	virtual float fRate
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	//
	//	Examines aign to determine its importance.
	//
	//	Returns:
	//		A rating of aign's importance.  0 is vital, higher numbers are less vital.
	//
	//
	//******************************
	{
		if (setFlags[enfVALIDATED] == false)
			Validate(paig, i_node_index);

		return fBAD;
	}

	//*************************************************************************************
	//
	virtual void Delete
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	);
	//
	//	Updates any external data structs that need it when a node is deleted.
	//
	//	Notes:
	//
	//******************************

//
//  Cast functions.
//
	virtual CNHDesigner*		pCastNHDesigner()		{ return this; };
};

//*********************************************************************************************
//
class CNHUnknown : public CNodeHistory
//
//	Prefix: nhu
//
//	No one knows where this node came from.
//
//	Notes:
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//

//
//  Member function definitions
//

	//*************************************************************************************
	//
	virtual float fRate
	(
		CAIGraph*,	//paig,			// The graph the node is in.
		int			//i_node_index	// The index of the node.
	);
	//******************************

//
//  Cast functions.
//
	
	virtual CNHUnknown*			pCastNHUnknown()		{ return this; };
};

//*********************************************************************************************
//
class CNHInvalid : public CNodeHistory
//
//	Prefix: nhinv
//
//	This is a bogus node.
//
//	Notes:
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//
	uint32 u4Bits;		// Clear these bits to zero for safety.

//
//  Member function definitions
//
	CNHInvalid()
	{
		u4Bits = 0;
	}
//
//  Cast functions.
//
	
	virtual CNHInvalid*			pCastNHInvalid()		{ return this; };
};

#define ENTRY(type, name) char name [sizeof(type)]

// A union used to guarantee that we have an amount of memory greater than the size
// of the largest NodeHistory child.
union UNodeHistory {
char nh		[sizeof(CNodeHistory)];
char nhsta	[sizeof(CNHStart)];
ENTRY(CNHStop,nhsto);
ENTRY(CNHInfluence,nhsil);
ENTRY(CNHInfluenceSurface,nhsils);
ENTRY(CNHNodeSource, nhns);
ENTRY(CNHRandom,nhr);
ENTRY(CNHDesigner,			nhd);
ENTRY(CNHUnknown,			nhu);

#ifdef TRACK_OLD_NODES
ENTRY(CNHNodeSourceOld, nhnso);
ENTRY(CNHInfluenceOld,nhsilo);
#endif
};

#undef ENTRY


//lint -restore  // for 1712
		
//#ifndef HEADER_GAME_AI_NODEHISTORY_HPP
#endif
