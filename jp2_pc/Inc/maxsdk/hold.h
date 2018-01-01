/**********************************************************************
 *<
	FILE: hold.h

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __HOLD__H__
#define __HOLD__H__

class HoldStore;
class RestoreObj {
	friend class HoldStore;
		RestoreObj *next,*prev;
	public:
		RestoreObj() { next = prev = NULL; }
   		virtual ~RestoreObj() {};
		virtual void Restore(int isUndo)=0;
		virtual void Redo()=0;
		virtual int Size() { return 1; }
		virtual void EndHold() { }
		virtual TSTR Description() { return TSTR(_T("---")); }
	};

class Hold {
  	HoldStore *holdStore, *holdList;
	int undoEnabled;
	int superLevel;
	HoldStore *ResetStore();
	void Init();
	public:
		CoreExport Hold();
		CoreExport ~Hold();
		CoreExport void Put(RestoreObj *rob);
		CoreExport void Begin();
		CoreExport void Suspend();	  // temporarly suspend holding
		CoreExport void Resume();    // resume holding
		CoreExport int	Holding();  // are we holding?
		CoreExport void DisableUndo();  // prevents Undo when Accept is called.
		CoreExport void EnableUndo();
		CoreExport void Restore();  // Restore changes from holdStore. 
		CoreExport void Release();  // Tosses out holdStore. 

		// 3 ways to terminate the Begin()...
		CoreExport void End();  // toss holdStore.
		CoreExport void Accept(int nameID); // record Undo (if enabled), End();
		CoreExport void Accept(TCHAR *name);
		CoreExport void Cancel();   // Restore changes, End() 

		//		
		// Group several Begin-End lists into a single Super-group.
		CoreExport void SuperBegin();
		CoreExport void SuperAccept(int nameID);
		CoreExport void SuperAccept(TCHAR *name);
		CoreExport void SuperCancel();
	};



extern CoreExport Hold theHold;

void CoreExport EnableUndoDebugPrintout(BOOL onoff);


#endif //__HOLD__H__
