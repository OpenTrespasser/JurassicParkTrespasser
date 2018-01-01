/*********************************************************************
 *<
	FILE: maxapi.h

	DESCRIPTION: These are functions that are exported from the 
	             3DS MAX executable.

	CREATED BY:	Rolf Berteig

	HISTORY: Created 28 November 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __JAGAPI__
#define __JAGAPI__

#include "cmdmode.h"
#include "sceneapi.h"
#include "rtclick.h"
#include "evuser.h"
#include "maxcom.h"

class ModContext;
class HitData;
class HitLog;
class CtrlHitLog;
class MtlBase;
class PickNodeCallback;
class Renderer;
class RendParams;
class RendProgressCallback;
class Bitmap;
class BitmapInfo;
class Texmap;
class SoundObj;
class MtlBaseLib;

// RB: I didn't want to include render.h here
#ifndef _REND_TYPE_DEFINED
#define _REND_TYPE_DEFINED
enum RendType { 
	RENDTYPE_NORMAL,
	RENDTYPE_REGION,
	RENDTYPE_BLOWUP,
	RENDTYPE_SELECT
	};
#endif

class NameMaker {
	public:
		virtual void MakeUniqueName(TSTR &name)=0;
	};

// This class provides functions that expose the portions of View3D
// that are exported for use by plug-ins.

class ViewExp {
	public:
		virtual Point3 GetPointOnCP(const IPoint2 &ps)=0;
		virtual Point3 SnapPoint(const IPoint2 &in, IPoint2 &out, Matrix3 *plane2d = NULL, DWORD flags = 0)=0;
		virtual float SnapLength(float in)=0;
		virtual float GetCPDisp(const Point3 base, const Point3& dir, 
                        const IPoint2& sp1, const IPoint2& sp2 )=0;
		virtual GraphicsWindow *getGW()=0;
		virtual int IsWire()=0;
		virtual Rect GetDammageRect()=0;		

		virtual Point3 MapScreenToView( IPoint2& sp, float depth )=0;
		virtual void MapScreenToWorldRay(float sx, float sy, Ray& ray)=0;
		virtual void GetAffineTM( Matrix3& tm )=0;
		virtual float GetScreenScaleFactor(const Point3 worldPoint)=0;

		// return the viewPort screen width factor in world space at 
		// a point in world space
		virtual float GetVPWorldWidth(const Point3 wPoint)=0;
		virtual Point3 MapCPToWorld(const Point3 cpPoint)=0;
		virtual void GetConstructionTM( Matrix3 &tm )=0;
		virtual void SetGridSize( float size )=0;
		virtual float GetGridSize()=0;
		virtual BOOL IsGridVisible()=0;

		// Get the camera if there is one.
		virtual INode *GetViewCamera()=0;

		// node level hit-testing
		virtual void ClearHitList()=0;
		virtual INode *GetClosestHit()=0;
		virtual int HitCount()=0;
		// subobject level hit-testing
		virtual	void LogHit(INode *nr, ModContext *mc, DWORD dist, ulong info, HitData *hitdat = NULL)=0;		
		virtual HitLog&	GetSubObjHitList()=0;
		virtual void ClearSubObjHitList()=0;
		virtual int NumSubObjHits()=0;

		// For controller apparatus hit testing
		virtual void CtrlLogHit(INode *nr,DWORD dist,ulong info,DWORD infoExtra)=0;
		virtual CtrlHitLog&	GetCtrlHitList()=0;
		virtual void ClearCtrlHitList()=0;
		
		virtual float NonScalingObjectSize()=0;  // 1.0 is "default"

		// Turn on and off image background display
		virtual BOOL setBkgImageDsp(BOOL onOff)=0;
		virtual int	getBkgImageDsp(void)=0;		

		// Turn on and off safe frame display
		virtual void setSFDisplay(int onOff)=0;
		virtual int getSFDisplay(void)=0;

		// This is the window handle of the viewport. This is the
		// same window handle past to GetViewport() to get a ViewExp*
		virtual HWND GetHWnd()=0;
	};


// return values for CreateMouseCallBack
#define CREATE_CONTINUE 1
#define CREATE_STOP	0	    // creation terminated normally
#define CREATE_ABORT -1		// delete the created object and node


// This is a specific call-back proc for handling the creation process
// of a plug-in object.
// The vpt passed in will have had setTransform called with the 
// transform of the current construction plane.
class CreateMouseCallBack {
	public:
		virtual int proc( 
			ViewExp *vpt,
			int msg, 
			int point, 
			int flags, 
			IPoint2 m,
			Matrix3& mat
			)=0;
		virtual int override(int mode) { return mode; }	// Defaults to mode supplied
	};


class Object;
class ConstObject;
class MouseCallBack;
class IObjCreate;
class IObjParam;
class ModContext;
class ModContextList;
class INodeTab;

// Passed to getBkgFrameRange()
#define VIEWPORT_BKG_START			0
#define VIEWPORT_BKG_END			1

// Passed to setBkgORType().
#define VIEWPORT_BKG_BLANK			0
#define VIEWPORT_BKG_HOLD			1
#define VIEWPORT_BKG_LOOP			2

// Passed to setBkgImageAspect()
#define VIEWPORT_BKG_ASPECT_VIEW	0
#define VIEWPORT_BKG_ASPECT_BITMAP	1
#define VIEWPORT_BKG_ASPECT_OUTPUT	2


// Identify the transform tool buttons
#define MOVE_BUTTON		1
#define ROTATE_BUTTON	2
#define NUSCALE_BUTTON	3
#define USCALE_BUTTON	4
#define SQUASH_BUTTON	5
#define SELECT_BUTTON	6

// Axis constraints.
#define AXIS_XY		2
#define AXIS_ZX		1
#define AXIS_YZ		0
#define AXIS_X		3
#define AXIS_Y		4
#define AXIS_Z		5

// Origin modes		
#define ORIGIN_LOCAL		0	// Object's pivot
#define ORIGIN_SELECTION	1	// Center of selection set (or center of individual object for local or parent space)
#define ORIGIN_SYSTEM		2	// Center of the reference coord. system

// Reference coordinate system
#define COORDS_HYBRID	0
#define COORDS_SCREEN	1
#define COORDS_WORLD	2
#define COORDS_PARENT	3
#define COORDS_LOCAL	4
#define COORDS_OBJECT	5


// Max cursors
#define SYSCUR_MOVE			1
#define SYSCUR_ROTATE		2
#define SYSCUR_USCALE		3
#define SYSCUR_NUSCALE		4
#define SYSCUR_SQUASH		5
#define SYSCUR_SELECT		6
#define SYSCUR_DEFARROW		7

// flags to pass to RedrawViews
#define REDRAW_BEGIN		(1<<0)
#define REDRAW_INTERACTIVE	(1<<1)
#define REDRAW_END			(1<<2)
#define REDRAW_NORMAL		(1<<3)

// Return values for GetNumAxis()
#define NUMAXIS_ZERO		0 	// Nothing to transform
#define NUMAXIS_ALL			1	// Use only one axis.
#define NUMAXIS_INDIVIDUAL	2	// Do all, one at a time

// MAX Directories - update APP_TOTAL_DIRS (in app.h) if this changes!
#define APP_FONT_DIR		0
#define APP_SCENE_DIR		1
#define APP_IMPORT_DIR		2
#define APP_EXPORT_DIR		3
#define APP_HELP_DIR		4
#define APP_EXPRESSION_DIR	5
#define APP_PREVIEW_DIR		6
#define APP_IMAGE_DIR		7
#define APP_SOUND_DIR		8
#define APP_PLUGCFG_DIR		9
#define APP_MAXSTART_DIR	10
#define APP_VPOST_DIR		11
#define APP_MAXROOT_DIR		12


// Types for status numbers
#define STATUS_UNIVERSE		1
#define STATUS_SCALE		2
#define STATUS_ANGLE		3
#define STATUS_OTHER		4

// Extended display modes
#define EXT_DISP_NONE				0
#define EXT_DISP_SELECTED			(1<<0)		// object is selected
#define EXT_DISP_TARGET_SELECTED	(1<<1)		// object's target is selected
#define EXT_DISP_LOOKAT_SELECTED	(1<<2)		// object's lookat node is selected
#define EXT_DISP_ONLY_SELECTED		(1<<3)		// object is only thing selected
#define EXT_DISP_DRAGGING			(1<<4)		// object is being "dragged"
#define EXT_DISP_ZOOM_EXT			(1<<5)		// object is being tested for zoom ext

// Render time types passed to SetRendTimeType()
#define REND_TIMESINGLE		0
#define REND_TIMESEGMENT	1
#define REND_TIMERANGE		2
#define REND_TIMEPICKUP		3

// Flag bits for hide by category.
#define HIDE_OBJECTS	0x0001
#define HIDE_SHAPES		0x0002
#define HIDE_LIGHTS		0x0004
#define HIDE_CAMERAS	0x0008
#define HIDE_HELPERS	0x0010
#define HIDE_WSMS		0x0020
#define HIDE_SYSTEMS	0x0040
#define HIDE_ALL		0xffff
#define HIDE_NONE		0



// viewport layout configuration
//   VP_LAYOUT_ LEGEND
//		# is number of viewports (total) in view panel
//		V = vertical split
//		H = horizontal split
//		L/R	= left/right placement
//		T/B = top/bottom placement
//   CONSTANT LEGEND
//		bottom nibble is total number of views
#define VP_LAYOUT_1			0x0001
#define VP_LAYOUT_2V		0x0012
#define VP_LAYOUT_2H		0x0022
#define VP_LAYOUT_2HT		0x0032
#define VP_LAYOUT_2HB		0x0042
#define VP_LAYOUT_3VL		0x0033
#define VP_LAYOUT_3VR		0x0043
#define VP_LAYOUT_3HT		0x0053
#define VP_LAYOUT_3HB		0x0063
#define VP_LAYOUT_4			0x0074
#define VP_LAYOUT_4VL		0x0084
#define VP_LAYOUT_4VR		0x0094
#define VP_LAYOUT_4HT		0x00a4
#define VP_LAYOUT_4HB		0x00b4
#define VP_LAYOUT_1C		0x00c1
#define VP_NUM_VIEWS_MASK	0x000f



class DWORDTab : public Tab<DWORD> {};


// A callback object passed to RegisterTimeChangeCallback()
class TimeChangeCallback {
	public:
		virtual void TimeChanged(TimeValue t)=0;
	};


// A callback object passed to RegisterCommandModeChangeCallback()
class CommandModeChangedCallback {
	public:
		virtual void ModeChanged(CommandMode *oldM, CommandMode *newM)=0;
	};

// A call back object to filter selection in the track view.
class TrackViewFilter {
	public:
		// Return TRUE to accept the anim as selectable.
		virtual BOOL proc(Animatable *anim, Animatable *client,int subNum)=0;
	};

// Stores the result of a track view pick
class TrackViewPick {
	public:
		ReferenceTarget *anim;
		ReferenceTarget *client;
		int subNum;

		TrackViewPick() {anim=NULL;client=NULL;subNum=0;}
	};

// A callback object passed to SetPickMode()
class PickModeCallback {
	public:
		// Called when ever the pick mode needs to hit test. Return TRUE if something was hit
		virtual BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)=0;
		
		// Called when the user picks something. The vpt should have the result of the hit test in it.
		// return TRUE to end the pick mode.
		virtual BOOL Pick(IObjParam *ip,ViewExp *vpt)=0;

		// Called when the user right-clicks or presses ESC
		// return TRUE to end the pick mode, FALSE to continue picking
		virtual BOOL RightClick(IObjParam *ip,ViewExp *vpt)	{ return FALSE; }

		// Called when the mode is entered and exited.
		virtual void EnterMode(IObjParam *ip) {}
		virtual void ExitMode(IObjParam *ip) {}

		virtual HCURSOR GetDefCursor(IObjParam *ip) {return NULL;}
		virtual HCURSOR GetHitCursor(IObjParam *ip) {return NULL;}

		// If the user hits the H key while in your pick mode, you
		// can provide a filter to filter the name list.
		virtual PickNodeCallback *GetFilter() {return NULL;}
	};

// Not to be confused with a PickMODEcallback...
// Used to filter node's during a hit test (PickNode)
class PickNodeCallback {
	public:
		// Return TRUE if this is an acceptable hit, FALSE otherwise.
		virtual BOOL Filter(INode *node)=0;
	};

// Used with DoHitByNameDialog();
class HitByNameDlgCallback {
public:
	virtual TCHAR *dialogTitle()	{ return _T(""); }
	virtual TCHAR *buttonText() 	{ return _T(""); }
	virtual BOOL singleSelect()		{ return FALSE; }
	virtual BOOL useFilter()		{ return TRUE; }
	virtual int filter(INode *node)	{ return TRUE; }
	virtual BOOL useProc()			{ return TRUE; }
	virtual void proc(INodeTab &nodeTab) {}
	virtual BOOL doCustomHilite()	{ return FALSE; }
	virtual BOOL doHilite(INode *node)	{ return FALSE; }
};


class Interface;

// A callback used with RegisterRedrawViewsCallback()
class RedrawViewsCallback {
	public:
		// this will be called after all the viewport have completed drawing.
		virtual void proc(Interface *ip)=0;
	};

// A callback used with RegisterAxisChangeCallback()
class AxisChangeCallback {
	public:
		// this will be called when the axis system is changed
		virtual void proc(Interface *ip)=0;
	};


// Generic interface into Jaguar
class Interface {
	public:
		virtual HFONT GetAppHFont()=0;
		virtual void RedrawViews(TimeValue t,DWORD vpFlags=REDRAW_NORMAL, ReferenceTarget *change=NULL)=0;		
		virtual BOOL SetActiveViewport(HWND hwnd)=0;
		virtual ViewExp *GetActiveViewport()=0; // remember to release ViewExp* with ReleaseViewport()
		virtual IObjCreate *GetIObjCreate()=0;
		virtual IObjParam *GetIObjParam()=0;
		virtual HWND GetMAXHWnd()=0;

		// This will cause all viewports to be completely redrawn.
		// This is extremely heavy handed and should only be used when
		// necessary.
		virtual void ForceCompleteRedraw()=0;

		// Register a call back object that gets called evrytime
		// the viewports are redrawn.
		virtual void RegisterRedrawViewsCallback(RedrawViewsCallback *cb)=0;
		virtual void UnRegisterRedrawViewsCallback(RedrawViewsCallback *cb)=0;

		// Execute a track view pick dialog.
		virtual BOOL TrackViewPickDlg(HWND hParent, TrackViewPick *res, TrackViewFilter *filter=NULL, DWORD flags=0)=0;

		// Command mode methods
		virtual void PushCommandMode( CommandMode *m )=0;
		virtual void SetCommandMode( CommandMode *m )=0;
		virtual void PopCommandMode()=0;		
		virtual CommandMode* GetCommandMode()=0;
		virtual void SetStdCommandMode( int cid )=0;
		virtual void PushStdCommandMode( int cid )=0;		
		virtual void RemoveMode( CommandMode *m )=0;
		virtual void DeleteMode( CommandMode *m )=0;

		// This will set the command mode to a standard pick mode.
		// The callback implements hit testing and a method that is
		// called when the user actually pick an item.
		virtual void SetPickMode(PickModeCallback *pc)=0;
		
		// makes sure no pick modes are in the command stack.
		virtual void ClearPickMode()=0;

		// Puts up a hit by name dialog. If the callback is NULL it 
		// just does a standard select by name.
		// returns TRUE if the user OKs the dialog, FALSE otherwise.
		virtual BOOL DoHitByNameDialog(HitByNameDlgCallback *hbncb=NULL)=0;

		// status panel prompt stuff
		virtual void PushPrompt( TCHAR *s )=0;
		virtual void PopPrompt()=0;
		virtual void ReplacePrompt( TCHAR *s )=0;
		virtual void DisplayTempPrompt( TCHAR *s, int msec=1000)=0;
		virtual void RemoveTempPrompt()=0;

		// status panel progress bar
		virtual BOOL ProgressStart(TCHAR *title, BOOL dispBar, LPTHREAD_START_ROUTINE fn, LPVOID arg)=0;
		virtual void ProgressUpdate(int pct, BOOL showPct = TRUE, TCHAR *title = NULL)=0;
		virtual void ProgressEnd()=0;
		virtual BOOL GetCancel()=0;
		virtual void SetCancel(BOOL sw)=0;
		
		// Some info about the current grid settings
		virtual float GetGridSpacing()=0;
		virtual int GetGridMajorLines()=0;

		// Write values to x,y,z status boxes. Before doing this, mouse
		// tracking must be disabled. Typically a plug-in would disable
		// mouse tracking on mouse down and enable it on mouse up.		
		virtual void DisableStatusXYZ()=0;
		virtual void EnableStatusXYZ()=0;
		virtual void SetStatusXYZ(Point3 xyz,int type)=0;
		virtual void SetStatusXYZ(AngAxis aa)=0; // this will convert the aa for status display

		// Extended display modes (such as camera cones that only appear when dragging a camera)
		virtual void SetExtendedDisplayMode(int flags)=0;
		virtual int GetExtendedDisplayMode()=0;

		// UI flyoff timing
		virtual void SetFlyOffTime(int msecs)=0;
		virtual int  GetFlyOffTime()=0;

		// Get standard Jaguar cursors.
		virtual HCURSOR GetSysCursor( int id )=0;

		// Turn on or off a cross hair cursor which draws horizontal and vertical
		// lines the size of the viewport's width and height and intersect at
		// the mouse position.
		virtual void SetCrossHairCur(BOOL onOff)=0;
		virtual BOOL GetCrossHairCur()=0;

		// This pops all modes above the create or modify mode.
		// NOTE: This is obsolete with the new modifiy panel design.
		virtual void RealizeParamPanel()=0;

		// Snap an angle value (in radians)
		virtual float SnapAngle(float angleIn)=0;

		// Snap a percentage value (1.0 = 100%)
		virtual float SnapPercent(float percentIn)=0;

		// Get the snap switch state
		virtual BOOL GetSnapState()=0;

		// Get the snap type -- Absolute or Relative (grid.h)
		virtual int GetSnapMode()=0;

		// Set the snap mode -- Set to absolute will fail if not in screen space
		// Returns TRUE if succeeded
		virtual BOOL SetSnapMode(int mode)=0;

		// Hit tests the screen position for nodes and returns a 
		// INode pointer if one is hit, NULL otherwise.
		virtual INode *PickNode(HWND hWnd,IPoint2 pt,PickNodeCallback *filt=NULL)=0;

		//----- Modify-related Methods--------------------------

		// Registers the sub-object types for a given plug-in object type.
		virtual void RegisterSubObjectTypes( const TCHAR **types, int count )=0;

		// Add sub-object named selection sets the named selection set drop down.
		// This should be done whenever the selection level changes.
		virtual void AppendSubObjectNamedSelSet(const TCHAR *set)=0;

		// Clear the named selections from the drop down.
		virtual void ClearSubObjectNamedSelSets()=0;

		// Clears the edit field of the named selection set drop down
		virtual void ClearCurNamedSelSet()=0;

		// Returns the state of the sub object drop-down. 0 is object level
		// and >= 1 refer to the levels registered by the object.
		virtual int GetSubObjectLevel()=0;
		
		// Sets the sub-object drop down. This will cause the object being edited
		// to receive a notification that the current subobject level has changed.
		virtual void SetSubObjectLevel(int level)=0;

		// Enables or disables sub object selection. Note that it
		// will already be disabled if there are no subobject levels
		// registered. In this case, it can not be enabled.
		virtual void EnableSubObjectSelection(BOOL enable)=0;
		virtual BOOL IsSubObjectSelectionEnabled()=0;

		// Notifies the system that the selection level in the pipeline has chaned.
		virtual void PipeSelLevelChanged()=0;

		// Returns the sub-object selection level at the point in the
		// pipeline  just before the current place in the history.
		virtual void GetPipelineSubObjLevel(DWORDTab &levels)=0;

		// Get's all instance contexts for the modifier at the current
		// place in the history.
		virtual void GetModContexts(ModContextList& list, INodeTab& nodes)=0;

		// Hit tests the object currently being edited at the sub object level.
		virtual int SubObHitTest(TimeValue t, int type, int crossing, 
			int flags, IPoint2 *p, ViewExp *vpt)=0;

		// Is the selection set frozen?
		virtual BOOL SelectionFrozen()=0;
		virtual void FreezeSelection()=0;
		virtual void ThawSelection()=0;

		// Nodes in the current selection set.
		virtual INode *GetSelNode(int i)=0;
		virtual int GetSelNodeCount()=0;

		// This is the color that selected sub-object entities are to be drawn in.
		virtual Point3 GetSubSelColor()=0;

		// Enable/disable show end result. 
		virtual void EnableShowEndResult(BOOL enabled)=0;

		// Returns the state of the 'crossing' preference for hit testing.
		virtual BOOL GetCrossing()=0;

		// Sets the state of one of the transform tool buttons.
		// TRUE indecates pressed, FALSE is not pressed.
		virtual void SetToolButtonState(int button, BOOL state )=0;
		virtual BOOL GetToolButtonState(int button)=0;

		// Finds the vpt given the HWND
		virtual ViewExp *GetViewport( HWND hwnd )=0;		
		virtual void ReleaseViewport( ViewExp *vpt )=0;		

		// Disables/Enables animate button
		virtual void EnableAnimateButton(BOOL enable)=0;
		virtual BOOL IsAnimateEnabled()=0;

		// Turns the animate button on or off
		virtual void SetAnimateButtonState(BOOL onOff)=0;

		// Registers a callback that gets called whenever the axis
		// system is changed.
		virtual void RegisterAxisChangeCallback(AxisChangeCallback *cb)=0;
		virtual void UnRegisterAxisChangeCallback(AxisChangeCallback *cb)=0;
		 
		// Gets/Sets the state of the axis constraints.
		virtual int GetAxisConstraints()=0;
		virtual void SetAxisConstraints(int c)=0;
		virtual void EnableAxisConstraints(int c,BOOL enabled)=0;

		// Gets/Sets the state of the coordinate system center
		virtual int GetCoordCenter()=0;
		virtual void SetCoordCenter(int c)=0;
		virtual void EnableCoordCenter(BOOL enabled)=0;

		// Gets/Sets the reference coordinate systems
		virtual int GetRefCoordSys()=0;
		virtual void SetRefCoordSys(int c)=0;
		virtual void EnableRefCoordSys(BOOL enabled)=0;

		// Gets the axis which define the space in which transforms should
		// take place. 
		// The node and subIndex refer to the object and sub object which the axis
		// system should be based on (this should be the thing the user clicked on)
		// If 'local' is not NULL, it will be set to TRUE if the center of the axis
		// is the pivot point of the node, FALSE otherwise.
		virtual Matrix3 GetTransformAxis(INode *node,int subIndex,BOOL* local = NULL)=0;

		// This returns the number of axis tripods in the scene. When transforming
		// multiple sub-objects, in some cases each sub-object is transformed in
		// a different space.
		// Return Values:
		// NUMAXIS_ZERO			- Nothing to transform
		// NUMAXIS_ALL			- Use only one axis.
		// NUMAXIS_INDIVIDUAL	- Do all, one at a time
		virtual int GetNumAxis()=0;

		// Locks axis tripods so that they will not be updated.
		virtual void LockAxisTripods(BOOL onOff)=0;
		virtual BOOL AxisTripodLocked()=0;

		// Registers a dialog window so IsDlgMesage() gets called for it.
		virtual void RegisterDlgWnd( HWND hDlg )=0;
		virtual int UnRegisterDlgWnd( HWND hDlg )=0;

		// Registers a keyboard accelerator table
		virtual void RegisterAccelTable( HWND hWnd, HACCEL hAccel )=0;
		virtual int UnRegisterAccelTable( HWND hWnd, HACCEL hAccel )=0;

		// Adds rollup pages to the command panel. Returns the window
		// handle of the dialog that makes up the page.
		virtual HWND AddRollupPage( HINSTANCE hInst, TCHAR *dlgTemplate, 
				DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0 )=0;
		
		// Removes a rollup page and destroys it.
		virtual void DeleteRollupPage( HWND hRollup )=0;

		// When the user mouses down in dead area, the plug-in should pass
		// mouse messages to this function which will pass them on to the rollup.
		virtual void RollupMouseMessage( HWND hDlg, UINT message, 
				WPARAM wParam, LPARAM lParam )=0;

		// get/set the current time.
		virtual TimeValue GetTime()=0;	
		virtual void SetTime(TimeValue t,BOOL redraw=TRUE)=0;

		// get/set the anim interval.
		virtual Interval GetAnimRange()=0;
		virtual void SetAnimRange(Interval range)=0;

		// Register a callback object that will get called every time the
		// user changes the frame slider.
		virtual void RegisterTimeChangeCallback(TimeChangeCallback *tc)=0;
		virtual void UnRegisterTimeChangeCallback(TimeChangeCallback *tc)=0;

		// Register a callback object that will get called when the user
		// causes the command mode to change
		virtual void RegisterCommandModeChangedCallback(CommandModeChangedCallback *cb)=0;
		virtual void UnRegisterCommandModeChangedCallback(CommandModeChangedCallback *cb)=0;

		virtual RightClickMenuManager* GetRightClickMenuManager()=0;

		// Delete key notitfication
		virtual void RegisterDeleteUser(EventUser *user)=0;		// Register & Activate
		virtual void UnRegisterDeleteUser(EventUser *user)=0;	// Deactivate & UnRegister

		//----- Creation-related Methods--------------------------
		
		virtual void MakeNameUnique(TSTR &name)=0;
		virtual INode *CreateObjectNode( Object *obj)=0;		
		virtual void *CreateInstance(SClass_ID superID, Class_ID classID)=0;
		virtual int BindToTarget(INode *laNode, INode *targNode)=0;
		virtual int IsCPEdgeOnInView()=0;		
		virtual void DeleteNode(INode *node)=0;
		virtual INode *GetRootNode()=0;
		virtual void NodeInvalidateRect( INode *node )=0;
		virtual void SelectNode( INode *node, int clearSel = 1)=0;
		virtual void AddLightToScene(INode *node)=0; 
		virtual void SetNodeTMRelConstPlane(INode *node, Matrix3& mat)=0;
		virtual void SetActiveGrid(INode *node)=0;

		// When a plug-in object implements it's own BeginCreate()/EndCreate()
		// it can cause EndCreate() to be called by calling this method.
		virtual void StopCreating()=0;

		// This creates a new object/node with out going throught the usual
		// create mouse proc sequence.
		// The matrix is relative to the contruction plane.
		virtual void NonMouseCreate(Matrix3 tm)=0;

		// directories
		virtual TCHAR *GetDir(int which)=0;		// which = APP_XXX_DIR
		virtual int	GetPlugInEntryCount()=0;	// # of entries in PLUGIN.INI
		virtual TCHAR *GetPlugInDesc(int i)=0;	// ith description
		virtual TCHAR *GetPlugInDir(int i)=0;	// ith directory

		// bitmap path
		virtual int GetMapDirCount()=0;			// number of dirs in path
		virtual TCHAR *GetMapDir(int i)=0;		// i'th dir of path
		virtual BOOL AddMapDir(TCHAR *dir)=0;	// add a path to the list

		virtual float GetLightConeConstraint()=0;

		// for light exclusion/inclusion lists
		virtual int DoExclusionListDialog(NameTab *nl, BOOL doShadows=TRUE)=0;
		
		virtual MtlBase *DoMaterialBrowseDlg(HWND hParent,DWORD flags,BOOL &newMat,BOOL &cancel)=0;

		virtual void PutMtlToMtlEditor(MtlBase *mb)=0;
		
		// Access names of current files
		virtual TSTR &GetCurFileName()=0;
		virtual TSTR &GetCurFilePath()=0;
		virtual TCHAR *GetMatLibFileName()=0;

		// These may bring up file requesters
		virtual void FileOpen()=0;
		virtual BOOL FileSave()=0;
		virtual BOOL FileSaveAs()=0;
		virtual void FileSaveSelected()=0;
		virtual void FileReset(BOOL noPrompt=FALSE)=0;
		virtual void FileMerge()=0;
		virtual void FileHold()=0;
		virtual void FileFetch()=0;
		virtual void FileOpenMatLib(HWND hWnd)=0;  // Window handle is parent window
		virtual void FileSaveMatLib(HWND hWnd)=0;
		virtual void FileSaveAsMatLib(HWND hWnd)=0;
		
		// This loads 3dsmax.mat (if it exists
		virtual void LoadDefaultMatLib()=0;

		// These do not bring up file requesters
		virtual int LoadFromFile(const TCHAR *name)=0;
		virtual int SaveToFile(const TCHAR *fname)=0;		
		virtual int LoadMaterialLib(const TCHAR *name)=0;
		virtual int SaveMaterialLib(const TCHAR *name)=0;
		virtual int MergeFromFile(const TCHAR *name, BOOL mergeAll=FALSE,BOOL selMerged=FALSE)=0;

		// Returns TRUE if this instance of MAX is in slave mode
		virtual BOOL InSlaveMode()=0;

		// Brings up the object color picker. Returns TRUE if the user
		// picks a color and FALSE if the user cancels the dialog.
		// If the user picks a color than 'col' will be set to the color.
		virtual BOOL NodeColorPicker(HWND hWnd,DWORD &col)=0;

		
		// The following gourping functions will operate on the table
		// of nodes passed in or the current selection set if the table is NULL
		
		// If name is NULL a dialog box will prompt the user to select a name. 
		// If sel group is TRUE, the group node will be selected after the operation completes.
		// returns a pointer to the group node created.
		virtual INode *GroupNodes(INodeTab *nodes=NULL,TSTR *name=NULL,BOOL selGroup=TRUE)=0;
		virtual void UngroupNodes(INodeTab *nodes=NULL)=0;
		virtual void ExplodeNodes(INodeTab *nodes=NULL)=0;
		virtual void OpenGroup(INodeTab *nodes=NULL,BOOL clearSel=TRUE)=0;
		virtual void CloseGroup(INodeTab *nodes=NULL,BOOL selGroup=TRUE)=0;

		// If a plug-in needs to do a PeekMessage() and wants to actually remove the
		// message from the queue, it can use this method to have the message
		// translated and dispatched.
		virtual void TranslateAndDispatchMAXMessage(MSG &msg)=0;
		
		// This will go into a PeekMessage loop until there are no more
		// messages left. If this method returns FALSE then the user
		// is attempting to quit MAX and the caller should return.
		virtual BOOL CheckMAXMessages()=0;

		// Access viewport background image settings.
		virtual BOOL		setBkgImageName(TCHAR *name)=0;
		virtual TCHAR *		getBkgImageName(void)=0;
		virtual void		setBkgImageAspect(int t)=0;
		virtual int			getBkgImageAspect()=0;
		virtual void		setBkgImageAnimate(BOOL onOff)=0;
		virtual int			getBkgImageAnimate(void)=0;
		virtual void		setBkgFrameRange(int start, int end, int step=1)=0;
		virtual int			getBkgFrameRangeVal(int which)=0;
		virtual void		setBkgORType(int which, int type)=0; // which=0 => before start, which=1 =>	after end
		virtual int			getBkgORType(int which)=0;
		virtual void		setBkgStartTime(TimeValue t)=0;
		virtual TimeValue	getBkgStartTime()=0;
		virtual void		setBkgSyncFrame(int f)=0;
		virtual int			getBkgSyncFrame()=0;
		virtual int			getBkgFrameNum(TimeValue t)=0;

		// Gets the state of the real-time animation playback toggle.
		virtual BOOL GetRealTimePlayback()=0;
		virtual void SetRealTimePlayback(BOOL realTime)=0;
		virtual BOOL GetPlayActiveOnly()=0;
		virtual void SetPlayActiveOnly(BOOL playActive)=0;
		virtual void StartAnimPlayback(int selOnly=FALSE)=0;
		virtual void EndAnimPlayback()=0;
		virtual BOOL IsAnimPlaying()=0;
		
		// The following APIs provide a simplistic method to call
		// the renderer and render frames. The renderer just uses the
		// current user specified parameters.
		// Note that the renderer uses the width, height, and aspect
		// of the specified bitmap so the caller can control the size
		// of the rendered image rendered.

		// Renderer must be opened before frames can be rendered.
		// Either camNode or view must be non-NULL but not both.
		// 
		// Returns the result of the open call on the current renderer.
		// 0 is fail and 1 is succeed.
		virtual int OpenCurRenderer(INode *camNode,ViewExp *view)=0;

		// The renderer must be closed when you are done with it.
		virtual void CloseCurRenderer()=0;

		// Renders a frame to the given bitmap.
		// The RendProgressCallback is an optional callback (the base class is
		// defined in render.h).
		//
		// Returns the result of the render call on the current renderer.
		// 0 is fail and 1 is succeed.
		virtual int CurRendererRenderFrame(TimeValue t,Bitmap *bm,RendProgressCallback *prog=NULL, float frameDur = 1.0f)=0;


		// To get more control over the renderer, the renderer can
		// be called dircetly. The following methods give access to
		// the current renderer and the the user's current rendering settings.

		// Retreives a pointer the renderer currently set to be the
		// active renderer. 
		virtual Renderer *GetCurrentRenderer()=0;
		
		// Fills in a RendParams structure that can be passed to the
		// renderer with the user's current rendering settings.
		// A vpt pointer only needs to be passed in if the RendType
		// is RENDTYPE_REGION or RENDTYPE_BLOWUP. In these cases it will
		// set up the RendParams regxmin,regxmax,regymin,regymax from
		// values stored in the viewport.
		virtual void SetupRendParams(RendParams &rp, ViewExp *vpt, RendType t = RENDTYPE_NORMAL)=0;

		// These give access to individual user specified render parameters
		// These are either parameters that the user specifies in the
		// render dialog or the renderer page of the preferences dialog.
		virtual int GetRendTimeType()=0;
		virtual void SetRendTimeType(int type)=0;
		virtual TimeValue GetRendStart()=0;
		virtual void SetRendStart(TimeValue start)=0;
		virtual TimeValue GetRendEnd()=0;
		virtual void SetRendEnd(TimeValue end)=0;
		virtual int GetRendNThFrame()=0;
		virtual void SetRendNThFrame(int n)=0;
		virtual BOOL GetRendShowVFB()=0;
		virtual void SetRendShowVFB(BOOL onOff)=0;
		virtual BOOL GetRendSaveFile()=0;
		virtual void SetRendSaveFile(BOOL onOff)=0;
		virtual BOOL GetRendUseDevice()=0;
		virtual void SetRendUseDevice(BOOL onOff)=0;
		virtual BOOL GetRendUseNet()=0;
		virtual void SetRendUseNet(BOOL onOff)=0;
		virtual BitmapInfo& GetRendFileBI()=0;
		virtual BitmapInfo& GetRendDeviceBI()=0;
		virtual int GetRendWidth()=0;
		virtual void SetRendWidth(int w)=0;
		virtual int GetRendHeight()=0;
		virtual void SetRendHeight(int h)=0;
		virtual float GetRendApect()=0;
		virtual void SetRendAspect(float a)=0;
		virtual BOOL GetRendFieldRender()=0;
		virtual void SetRendFieldRender(BOOL onOff)=0;
		virtual BOOL GetRendColorCheck()=0;
		virtual void SetRendColorCheck(BOOL onOff)=0;
		virtual BOOL GetRendSuperBlack()=0;
		virtual void SetRendSuperBlack(BOOL onOff)=0;
		virtual BOOL GetRendHidden()=0;
		virtual void SetRendHidden(BOOL onOff)=0;
		virtual BOOL GetRendForce2Side()=0;
		virtual void SetRendForce2Side(BOOL onOff)=0;
		virtual BOOL GetRendAtmosphere()=0;
		virtual void SetRendAtmosphere(BOOL onOff)=0;
		virtual TSTR& GetRendPickFramesString()=0;
		virtual BOOL GetRendDitherTrue()=0;
		virtual void SetRendDitherTrue(BOOL onOff)=0;
		virtual BOOL GetRendDither256()=0;
		virtual void SetRendDither256(BOOL onOff)=0;
		virtual BOOL GetRendMultiThread()=0;
		virtual void SetRendMultiThread(BOOL onOff)=0;
		virtual BOOL GetRendNThSerial()=0;
		virtual void SetRendNThSerial(BOOL onOff)=0;
		virtual int GetRendVidCorrectMethod()=0; // 0->FLAG, 1->SCALE_LUMA 2->SCALE_SAT
		virtual void SetRendVidCorrectMethod(int m)=0;
		virtual int GetRendFieldOrder()=0; // 0->even, 1-> odd
		virtual void SetRendFieldOrder(int fo)=0;
		virtual int GetRendNTSC_PAL()=0; // 0 ->NTSC,  1 ->PAL
		virtual void SetRendNTSC_PAL(int np)=0;
		virtual int GetRendSuperBlackThresh()=0;
		virtual void SetRendSuperBlackThresh(int sb)=0;
		virtual float GetRendMaxPixelSize()=0;
		virtual void SetRendMaxPixelSize(float s)=0;

		virtual DWORD GetHideByCategoryFlags()=0;
		virtual void SetHideByCategoryFlags(DWORD f)=0;

		virtual int GetViewportLayout()=0;
		virtual BOOL IsViewportMaxed()=0;
		virtual void SetViewportMax(BOOL max)=0;

		// Zoom extents the active viewport, or all
		virtual void ViewportZoomExtents(BOOL doAll, BOOL skipPersp=FALSE)=0;

		// Gets the world space bounding box of the selection.
		virtual void GetSelectionWorldBox(TimeValue t,Box3 &box)=0;
		
		// Find an INode with the given name
		virtual INode *GetINodeByName(const TCHAR *name)=0;

		// Executes a MAX command. See maxcom.h for available commands
		virtual void ExecuteMAXCommand(int id)=0;

		// Returns a class used for efficiently creating unique names
		virtual NameMaker* NewNameMaker()=0;

		// Get set the viewport background color.
		virtual void SetViewportBGColor(const Point3 &color)=0;
		virtual Point3 GetViewportBGColor()=0;

		// Get/Set the environment texture map.
		virtual Texmap *GetEnvironmentMap()=0;
		virtual void SetEnvironmentMap(Texmap *map)=0;

		// Get/Set the current sound object.
		virtual SoundObj *GetSoundObject()=0;
		virtual void SetSoundObject(SoundObj *snd)=0;

		// Access the current mat lib loaded.
		virtual MtlBaseLib& GetMaterialLibrary()=0;

		// Network error logging
		// for writing multiple lines, use NetOpenLog(), NetCloseLog().
		// For a single line, just call NetLog: it will automatically do
		// an Open and Close.
		virtual int NetOpenLog()=0; // returns 1 if is network server and open succeeded
		virtual void NetLog(const TCHAR *format,...)=0; // noop unless network server
		virtual void NetCloseLog()=0;
		virtual BOOL IsNetServer()=0; // returns 1 iff is network server
	};

class IObjParam: public Interface{};
class IObjCreate: public IObjParam{};


#endif // __JAGAPI__

