/**********************************************************************
 *<
	FILE:  ref.h

	DESCRIPTION:  Defines Animatable, Reference Classes

	CREATED BY: Rolf Berteig & Dan Silva

	HISTORY: created 9 September 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef _REF_H_
#define _REF_H_

/* This module implements a first run at the concept of references.
 * Some other stuff is in here to like time and intervals, but
 * these are implemented only to get the reference classes working.
 */
typedef void *ReferenceData;
typedef unsigned int TargetParam;

typedef unsigned long PartID;

// Bits for PartID


// OBJECT STATE: SUB-PARTS
// These are the 4 sub-channels object in the object state.
// Dont change these defines  
// (See CHANNELS.H for define of TOPO_CHANNEL, etc.)
#define PART_TOPO			TOPO_CHANNEL   
#define PART_GEOM			GEOM_CHANNEL   
#define PART_TEXMAP			TEXMAP_CHANNEL 
#define PART_MTL			MTL_CHANNEL    
#define PART_SELECT			SELECT_CHANNEL 
#define PART_SUBSEL_TYPE 	SUBSEL_TYPE_CHANNEL
#define PART_DISPLAY    	DISP_ATTRIB_CHANNEL    

// These represent the TM and MTL parts of the object state
#define PART_TM_CHAN   		TM_CHANNEL		// (1<<7)
#define PART_MTL_CHAN		GLOBMTL_CHANNEL // (1<<8)

#define PART_OBJECT_TYPE   	(1<<10)      
#define PART_TM  			(1<<11)		// node TM
#define PART_OBJ  			(PART_TOPO|PART_GEOM)

#define PART_ALL			(ALL_CHANNELS|PART_TM)

// One of these is set as the PartID when notify dependents is
// called with REFMSG_FLAGDEPENDENTS
#define PART_PUT_IN_FG				(1<<0)
#define PART_SHOW_DEPENDENCIES		(1<<1)
#define PART_SHOWDEP_ON				(1<<2)

/* The message passed to notify and evaluate.
 */
typedef unsigned int RefMessage;

#define REFMSG_LOOPTEST 				0x00000010
#define REFMSG_TARGET_DELETED 			0x00000020
#define REFMSG_MODAPP_DELETING			0x00000021
#define REFMSG_EVAL  					0x00000030
#define REFMSG_RESET_ORIGIN 			0x00000040
#define REFMSG_CHANGE 					0x00000050
#define REFMSG_FLAGDEPENDENTS			0x00000070
#define REFMSG_TARGET_SELECTIONCHANGE	0x00000080
#define REFMSG_BEGIN_EDIT				0x00000090
#define REFMSG_END_EDIT					0x000000A0
#define REFMSG_DISABLE					0x000000B0
#define REFMSG_ENABLE					0x000000C0
#define REFMSG_TURNON					0x000000D0
#define REFMSG_TURNOFF					0x000000E0
#define REFMSG_LOOKAT_TARGET_DELETED  	0x000000F0 
#define REFMSG_INVALIDATE_IF_BG			0x000000F1
// turn on and off the a modifier display
#define REFMSG_MOD_DISPLAY_ON			0x000000F2
#define REFMSG_MOD_DISPLAY_OFF			0x000000F3

// Modifier uses this to tell Modapps to call their Eval() proc:
#define REFMSG_MOD_EVAL					0x000000F4

// Ask if its ok to change topology.  If any dependents have made 
//  topology-dependent mods, they should return REF_FAIL:
// a return of REF_SUCCEED means that the answer is YES
// a return of REF_FAIL means that the answer is NO
#define REFMSG_IS_OK_TO_CHANGE_TOPOLOGY	0x000000F5


// This main purpose of these notifications is to cause the tree
// view to update when one of these events takes place.

// Sent by a node when it has a child linked to it or unlinked from it.
#define REFMSG_NODE_LINK				0x000000F6

// Sent by a node when it's name has been changed.
#define REFMSG_NODE_NAMECHANGE			0x000000F7

// Sent by a node (or derived object) when the object it references changes.
#define REFMSG_OBREF_CHANGE				0x000000F8

// Sent by a derived object when a modifier is a added or deleted.
#define REFMSG_MODIFIER_ADDED			0x000000F9

// Sent when an animatable switches controllers for one of it's parameters.
#define REFMSG_CONTROLREF_CHANGE		0x000000FA

// A parameter block sends the message to it's client when it needs the anim
// name of the ith parameter.
// partID is set to a pointer to a GetParamName structure defined in iparamb.h
#define REFMSG_GET_PARAM_NAME			0x000000FB

// A parameter block sends this message to it's client when it needs to know
// the dimension type of the ith parameter.
// partID is set to a pointer to a GetParamDim structure  defined in iparamb.h
#define REFMSG_GET_PARAM_DIM			0x000000FC

// A controller can send this to it's client to get it's param dimension.
// It should set partID to a ParamDimension **
#define REFMSG_GET_CONTROL_DIM			0x000000FD

// This message is sent by a node when it's TM has changed because it was evaluated
// at a different time. Normally this isn't neccessary - anyone depending on the
// node's TM would have a validity interval that reflected the validity of the
// TM. The axis system doesn't store a validity interval (it probably should) so this
// message is needed for it.
#define REFMSG_TM_CHANGE 				0x000000FE

// A node sends this message when it's anim range changes.
#define REFMSG_RANGE_CHANGE				0x000000FF

// Sent to the tree view when an animatable's line height changes.
#define REFMSG_LINEHEIGHT_CHANGE		0x00000100

// A controller should send this message to the treeview when it becomes animated.
// If the user has the animated only filter on then the tree view will display this item.
#define REFMSG_BECOMING_ANIMATED		0x00000101

// This is intended mainly for the TrackView to tell it to re-generate
// view below the message sender's level.
#define REFMSG_SUBANIM_STRUCTURE_CHANGED 0x00000102

// A target has had a ref deleted: Mtl's use this to tell mtlLib, in case 
// the # of node refs has gone to zero.
#define REFMSG_REF_DELETED			0x00000103

// A target has had a ref Added: Mtl's use this to tell mtlLib, in case 
// the # of node refs has become non-zero.
#define REFMSG_REF_ADDED			0x00000104

// Sent by an object that provides branching in the history to notify that
// the structure of the branches changed.
#define REFMSG_BRANCHED_HISTORY_CHANGED	0x00000105

// The selection set sends this notification when it receives a REFMSG_CHANGE
// from an item in the selection set. The selection set doesn't propogate the
// REFMSG_CHANGE message.
#define REFMSG_NODEINSELSET_CHANGED	0x00000106

// Used to test dependencies
#define REFMSG_TEST_DEPENDENCY	0x00000107

// A Param block sends this to its client to ask if it should display
// a distinct "Parameters" level in the track view hierarchy. 
// A pointer to a boolan is passed in for PartID: set this to the desired answer.
// The default is NO -- in this case the message doesn't need
// to be responded to.
#define REFMSG_WANT_SHOWPARAMLEVEL 	0x00000108

// Theser are sent before and after a paste has been done: Sent as partID is 
// a pointer to a data structure containing three RefTargetHandle's: the ref maker,
// the old target,  and the new target. The message is sent to the ref maker initially.
#define REFMSG_BEFORE_PASTE 	0x00000109
#define REFMSG_NOTIFY_PASTE 	0x0000010A

// Sent when a UV Generator changes symmetry, so interactive texture display 
// updates.
#define REFMSG_UV_SYM_CHANGE    0x0000010B

// The first node that gets this message will fill in the
// TSTR which partID points to with its name and stop the
// message from propogating.
#define REFMSG_GET_NODE_NAME			0x0000010C

// Sent by the selection set whenever it has just deleted nodes
#define REFMSG_SEL_NODES_DELETED		0x0000010D

// Sent *before* a reference target is pasted. Sent by the target
// about to be replaced.
#define REFMSG_PRENOTIFY_PASTE 	0x0000010E

// Sent when a shape enters a state where it'll be changing a lot and it
// would be a good idea for anybody using it for mesh generation to suppress
// updates.
#define REFMSG_SHAPE_START_CHANGE	0x0000010F

// Sent to terminate the above state
#define REFMSG_SHAPE_END_CHANGE		0x00000110

// A texture map has been removed: tells medit to remove
// it from the viewport if it is active.
#define REFMSG_TEXMAP_REMOVED	0x00000111

// Sent by an unselected node to see if any selected nodes depend on it.
// The partID param points to a boolean. If a selected node receives this 
// message it should set the boolean to true and return REF_STOP.
#define REFMSG_FLAG_NODES_WITH_SEL_DEPENDENTS	0x00000112

// The following are sent by objects which contain shapes when the selection,
// position, or both change:
#define REFMSG_CONTAINED_SHAPE_POS_CHANGE 0x00000120
#define REFMSG_CONTAINED_SHAPE_SEL_CHANGE 0x00000121
#define REFMSG_CONTAINED_SHAPE_GENERAL_CHANGE 0x00000122

/* Message numbers above this value can be defined for use
 * by sub-classes, below are reserved.
 */
#define REFMSG_USER		0x00010000


/* Some return codes for references...
 * There would probably be a bunch of these.
 */
enum RefResult {
	REF_FAIL,
	REF_SUCCEED,
	REF_DONTCARE,
	REF_STOP,
	REF_INVALID	
	};


// Use this to disable ref counting on objects.
#define MakeFakeHandleClass( className, handleClassName ) 	\
	class className; typedef className* handleClassName		

MakeFakeHandleClass( ReferenceTarget, RefTargetHandle );
MakeFakeHandleClass( ReferenceMaker, RefMakerHandle );

// This replaces the .Valid() method of handles.

#define VALID(x) (x)

// If this super class is passed to NotifyDependents() all dependents will be notified
#define NOTIFY_ALL		0xfffffff0

// For remapping references during a Clone.
class RemapDir {
	public:
		virtual	RefTargetHandle FindMapping(RefTargetHandle from)=0;
		virtual void AddEntry(RefTargetHandle hfrom, RefTargetHandle hto)=0;
		virtual RefTargetHandle CloneRef(RefTargetHandle oldTarg);
		virtual void PatchPointer(RefTargetHandle* patchThis, RefTargetHandle oldTarg)=0;
		virtual void Backpatch()=0;
		virtual void Clear()=0;
		virtual	void DeleteThis()=0;
	};


/* Reference list link-node
 */
class CoreExport RefListItem {
	
	public:
		RefMakerHandle  maker;
		RefListItem		*next;
		/* Constructors:
		 */		
		RefListItem( RefMakerHandle hmaker, RefListItem *list ) { 
			next = list;
			maker = hmaker;
			}
	};

class CoreExport RefList {
	public:
		RefListItem* first;								  
		RefList() { first = NULL;}
		RefListItem*  FirstItem() { return first; }
		RefResult DeleteItem(RefMakerHandle hmaker, int eval);
		RefResult AddItem(RefMakerHandle hmaker );
		void Cleanup();  // remove null entries
	};

class DeleteRefRestore;
class MakeRefRestore;
class ParamBlock;
class ISave;
class ILoad;
class DependentIterator;

class DependentEnumProc {
	public:
		// proc should return 1 when it wants enumeration to halt.
		virtual	int proc(ReferenceMaker *rmaker)=0;
	};

class SaveEnumProc {
	public:
		virtual	void proc(ReferenceMaker *rmaker)=0;
		virtual int terminate(ReferenceMaker *rmaker)=0; 
	};

CoreExport RemapDir& NoRemap();

// Here's a pointer to a RemapDir: you must delete it when done.
CoreExport RemapDir* NewRemapDir(); 

/* Everything can be a reference maker. We might want to keep this
 * class separate from Animatable just for clarity.
 */
class  ReferenceMaker : public Animatable {
	friend class DeleteRefRestore;
	friend class MakeRefRestore;
	friend class ReferenceTarget;
	friend class ParamBlock;

	public:		
		/* Constructor:
		 */
		ReferenceMaker() { ClearAFlag(A_EVALUATING); } 

		// This routine is used when cloning reference makers, to delete old 
		// reference and make a new one.
		CoreExport RefResult ReplaceReference(int which, RefTargetHandle newtarg, BOOL delOld=TRUE);
		
		// new format
		CoreExport RefResult MakeRefByID(Interval refInterval,int which,RefTargetHandle htarget);

		// Deletes all references from this RefMaker.
		CoreExport RefResult DeleteAllRefsFromMe();

		// Deletes all refs to this RefMaker/RefTarget.
		// also sends REFMSG_TARGET_DELETED to all dependents.
		virtual RefResult DeleteAllRefsToMe() { return REF_SUCCEED; }

		// Deletes all references to AND from this guy.
		CoreExport RefResult DeleteAllRefs();

		// This deletes all reference to and from, sends REFMSG_TARGET_DELETED 
		// messages, handles UNDO, and deletes the object.	
		CoreExport void DeleteMe();

		// Enumerator to search back in the dependency network.
		CoreExport virtual	int EnumDependents(DependentEnumProc* dep);

		CoreExport virtual IOResult Save(ISave *isave);
		CoreExport virtual IOResult Load(ILoad *iload);

	   	// Access of refs. -- These functions must be implemented
		// by ALL classes that make refs.
		CoreExport virtual	int NumRefs();
		CoreExport virtual RefTargetHandle GetReference(int i);
		CoreExport virtual void SetReference(int i, RefTargetHandle rtarg);

		// A maker can choose not to let TransferReferences() affect it. Note that plug-ins probably should not use this
		// it is used by certain system objects that have references.
		virtual BOOL CanTransferReference(int i) {return TRUE;}

		//-- default Save enumeration.
		CoreExport virtual void SaveEnum(SaveEnumProc& sep, BOOL isNodeCall=0);

		CoreExport virtual RefResult 
			NotifyDependents(Interval changeInt, PartID partID, 
				RefMessage message, SClass_ID sclass=NOTIFY_ALL,
				BOOL propagate=TRUE, RefTargetHandle hTarg=NULL );

		// Enumerate auxiliary files (e.g. bitmaps)
		// The default implementation just calls itself on all references.
		// Entities which actually need to load aux files must implement this,
		// possibly calling ReferenceMaker::EnumAuxFiles also to recurse.
		CoreExport virtual void EnumAuxFiles(NameEnumCallback& nameEnum, DWORD flags);

		CoreExport RefResult DeleteReference( int i);
		CoreExport int FindRef(RefTargetHandle rtarg);

		// When a reference target's last "real" reference is deleted
		// the target is deleted. Any leftover "non-real" reference makers
		// will receive a REFMSG_TARGET_DELETED notification.
		virtual BOOL IsRealDependency(ReferenceTarget *rtarg) {return TRUE;}

	protected:
		void BlockEval() { SetAFlag(A_EVALUATING); }
		void UnblockEval() { ClearAFlag(A_EVALUATING); }		
		int Evaluating() { return TestAFlag(A_EVALUATING); }		
		
		CoreExport RefResult StdNotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
		   PartID partID, RefMessage message, BOOL propagate=TRUE);		

	private:
		

		// StdNotifyRefChanged calls this, which can change the partID to new value 
		// If it doesnt depend on the particular message& partID, it should return
		// REF_DONTCARE
	    virtual RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	         PartID& partID,  RefMessage message)=0;		
	
	};



/* Not everything is a reference target...
 */
class  ReferenceTarget : public ReferenceMaker {
	friend class DependentIterator;
	friend class DeleteRefRestore;
	friend class MakeRefRestore;
	//friend class Remap;
	
	public:
		ReferenceTarget() { ClearAFlag(A_LOCK_TARGET); } 

		// old format: will go away
		CoreExport RefResult MakeReference(Interval refInterval,RefMakerHandle hmaker,int whichRef=-1);

		CoreExport RefResult TestForLoop( Interval refInterval, RefMakerHandle hmaker);

	    CoreExport BOOL HasDependents();
	    CoreExport BOOL HasRealDependents();  // not counting tree view
		
		// Deletes all references to this refTarget.
		CoreExport RefResult DeleteAllRefsToMe();

		RefList& GetRefList() { return refs; }

		// This function is called when a targets last ref is deleted
		// Most subclasses will not need to override this. If you don't want to be
		// deleted when the last ref is deleted, plug in a noop.
		CoreExport virtual RefResult AutoDelete();

		// This is called after deleting a reference to a ref target,
		// in the case that the target was not deleted. If target needs
		// to know, it should override this method.
		virtual void RefDeleted() {}  

		// This is called after making a reference-- If the
		// target needs to know it can override it.
		virtual void RefAdded(RefMakerHandle rm) {}  

		// Transfer all the references from oldTarget to this 
		CoreExport RefResult TransferReferences(RefTargetHandle oldTarget, BOOL delOld=FALSE);

		// Enumerator to search back in the dependency network.
		CoreExport virtual	int EnumDependents(DependentEnumProc* dep);

		CoreExport virtual RefTargetHandle Clone(RemapDir &remap = NoRemap());

		// Checks if there are no references, and if object is deletable, deletes it.
		CoreExport RefResult MaybeAutoDelete();


		// This sends the REFMSG_FLAGDEPENDENTS message up the pipeline.
		// There are two reasons to flag dependents:
		// 1) To put the node in the FG plane. (PART_PUT_IN_FG)
		// 2) To set the node's mesh color to green to indicate it is a
		//    dependent. (PART_SHOW_DEPENDENCIES)
		//    If the PART_SHOWDEP_ON bit is set, the dependency display is
		//    turned on, otherwise it is turned off.
		void FlagDependents( TimeValue t, PartID which=PART_PUT_IN_FG ) 
			{ 
			NotifyDependents( 
				Interval(t,t), 
				which,
				REFMSG_FLAGDEPENDENTS );
			}	

		// This method is called to flag dependents into the FG. 
		// (Note that the above method is obsolete)
		// The default implementation just sends out the notification.
		// In particular, a slave controller could override this method
		// and call its masters version of this method.
		virtual void NotifyForeground(TimeValue t) 
			{NotifyDependents(Interval(t,t),PART_PUT_IN_FG,REFMSG_FLAGDEPENDENTS);}

		// To see if this reference target depends on something:
		// first call BeginDependencyTest()
		// then call NotifyDependents() on the thing with the REFMSG_TEST_DEPENDENCY
		// if EndDependencyTest() returns TRUE this target is dependent on the thing. 
		void BeginDependencyTest() {ClearAFlag(A_DEPENDENCY_TEST);}
		BOOL EndDependencyTest() {return TestAFlag(A_DEPENDENCY_TEST);}
			
		// Notify all dependent RefMakers concerened with the message 
		CoreExport virtual RefResult 
			NotifyDependents(Interval changeInt, PartID partID, 
				RefMessage message, SClass_ID sclass=NOTIFY_ALL,
				BOOL propagate=TRUE, RefTargetHandle hTarg=NULL);
	private:		
		
		// This is the list of active references that refer to us.
		RefList	refs;
	};

class DependentIterator {
	ReferenceTarget *rt;
	RefListItem *next;
	public:
	DependentIterator(ReferenceTarget *rtarg) { rt = rtarg; next = rt->refs.first; }
	CoreExport ReferenceMaker *Next();
	void Reset() { next = rt->refs.first; }
	};

class DeletedRestore: public RestoreObj {
	RefMakerHandle anim, svanim;
	public:
		CoreExport DeletedRestore();
		CoreExport DeletedRestore(RefMakerHandle an);
		CoreExport ~DeletedRestore();
		CoreExport void Restore(int isUndo);
		CoreExport void Redo();
		CoreExport TSTR Description();
	};


#endif


