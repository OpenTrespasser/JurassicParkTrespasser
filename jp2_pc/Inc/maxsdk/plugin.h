
#ifndef PLUGIN_H_DEFINED
#define PLUGIN_H_DEFINED

/*******************************************************************
 *
 *    DESCRIPTION: DLL Plugin classes
 *
 *    AUTHOR: Dan Silva
 *
 *    HISTORY: 11/30/94 Started coding
 *
 *******************************************************************/

typedef unsigned long ulong;

#include <plugapi.h>
//---------------------------------------------------------
// This is the interface to a DLL
// Each DLL may implement any number of classes. 
//
class DllDesc {
public:
	HINSTANCE		handle;
	TSTR fname;
	const TCHAR*	(*vDescription)();   // copy of this string can be stored in the DLL dir
	int				(*vNumClasses)();    // How many classes does this DLL implement?
    ClassDesc*		(*vClassDesc)(int i); // Hands back a ClassDesc for the ith class
	CoreExport	 	DllDesc(); 
	CoreExport void		 	Free() { FreeLibrary(handle); }
	CoreExport const TCHAR*	Description() { return (*vDescription)(); };
	CoreExport int 			NumberOfClasses() { return (*vNumClasses)(); };
	ClassDesc*		operator[](int i) { return (*vClassDesc)(i); };
	int operator==( const DllDesc& dd ) const { return 0; }
	};


/*-----------------------------------------------------------------
 We will build one big DllDir on startup.
 As we do this, we will build a set of lists, one for each SuperClass.
 For a given super class, we want to 
	(a) Enumerate all public subclasses 
	(b) Enumerate all subclasses
	(c) Find the subClass for a given subClass ID.

--------------------------------------------------------------*/

class ClassEntry {
	int dllNumber;  // index into the master Dll list
	int classNum;  	// index of the class within the DLL
	int isPublic;
	int useCount;  // used for enumerating classes in summary info
	Class_ID classID;
	TSTR name;
	TSTR category;
	ClassDesc *cd;	
	
	// The following are use to store the last rollup state for a
	// given class. 'scroll' is the scroll amount of the command panel
	// and pageState is the open close state for up to 32 rollup pages. (when the bit is set that means the rollup page is closed)
	int scroll;
	DWORD pageState;
	
	public:
		CoreExport ClassEntry();
		CoreExport ClassEntry(const ClassEntry &ce);
		CoreExport ClassEntry(ClassDesc *cld, int dllN, int index);
		CoreExport void Set(ClassDesc *cld, int dllN, int index);
		int DllNumber() { return dllNumber; }
		int IsPublic() { return isPublic; }
		Class_ID ClassID() { return classID; }
		TSTR &ClassName() { return name; }
		TSTR &Category() { return category; }
		int GetScroll() {return scroll;}
		void SetScroll(int s) {scroll = s;}
		BOOL GetPageState(int i) {return (pageState&(1<<i))?TRUE:FALSE;}
		void SetPageState(int i,BOOL state) {if (state) pageState |= (1<<i); else pageState &= ~(1<<i);}
		int UseCount() { return useCount; }
		void IncUseCount () { useCount++; }
	    void SetUseCount(int i) { useCount = i; }
		ClassDesc *CD() { return cd; }
		CoreExport int ClassEntry::IsAccType(int accType);
		CoreExport ClassEntry& operator=( const ClassEntry &ce ); 
		int operator==( const ClassEntry &ce ) const { return 0; }
	};

// access type.
#define ACC_PUBLIC 1
#define ACC_PRIVATE 2
#define ACC_ALL (ACC_PUBLIC|ACC_PRIVATE)

typedef ClassEntry* PClassEntry;
typedef Tab<PClassEntry> SubClassTab;


class SubClassList {
		int iEnum;
		ulong superID;
		SubClassTab subClList;
    public:
		CoreExport ~SubClassList();
		SubClassList(ulong sid=0) { superID = sid; }
		ClassEntry& operator[](int i){ return(*subClList[i]);}
		CoreExport int FindClass(Class_ID subClassID);	  // Get a class by its class ID
		CoreExport int FindClass(const TCHAR *name);    // Get a class by its class name
		CoreExport void AddClass(ClassDesc *cld, int dllNum, int index); 
		CoreExport int  Count(int accType);
		ulong SuperID() { return superID; }
		int operator==(const SubClassList &sl) {return(0);}

		// Enumerate.
		CoreExport int GetFirst(int accType);	 // get first ClassDesc of accType
		CoreExport int GetNext(int accType);	 // get next ClassDesc of accType (NULL for End)

		int operator==( const SubClassList& lst ) const { return 0; }
	};


typedef SubClassList* PSubClassList;
typedef Tab<PSubClassList> ListOfClassLists;

/* ClassDirectory: A list of SubClassLists, one for each pluggable
   super class */
class ClassDirectory {
	ListOfClassLists cl;
	public:				  
		CoreExport ~ClassDirectory();
		CoreExport SubClassList* GetClassList(SClass_ID superClassID);	
		CoreExport ClassDesc* FindClass(SClass_ID superClassID, Class_ID subClassID);
		CoreExport ClassEntry *FindClassEntry(SClass_ID superClassID, Class_ID subClassID);
		SubClassList& operator[](int i){ return(*cl[i]);}
		CoreExport void AddSuperClass(SClass_ID superClassID);
		// returns 0 if class already exists
		// returns -1 if superclass was unknown
		CoreExport int  AddClass(ClassDesc *cdesc, int dllNum, int index);
		int Count() { return cl.Count(); }	
	};

/* DllDirectory: A list DllDescriptors, one for every DLL loaded 
   Also contains the ClassDirectory of all classes implemented in these
   DLL's */

typedef DllDesc *PDllDesc;

class DllDir {
	Tab<PDllDesc> dll;    // list of Descriptors for all the loaded DLL's
	ClassDirectory classDir; // All the classes implemented in these DLL's 
	int  AddDll(const DllDesc *dllDesc);
	public:
		CoreExport ~DllDir();
		CoreExport void UnloadAllDlls();
		int Count() { return dll.Count(); }
		DllDesc& operator[](int i) { return(*dll[i]); }
		CoreExport int LoadDllsFromDir(TCHAR *directory, TCHAR *  wildcard, HWND hwnd=NULL);
		ClassDirectory& ClassDir() { return classDir; }
	};

#endif // PLUGIN_H_DEFINED
