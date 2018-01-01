/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		class InitSys
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/InitSys.hpp                                                   $
 * 
 * 5     97-05-06 15:45 Speter
 * Added destructor to CInitPtr, which deletes pointer.
 * 
 * 4     96/08/19 13:04 Speter
 * Added InitPtr function.
 * 
 * 3     96/07/31 15:34 Speter
 * Added CInitPtr template class.
 * 
 * 2     96/05/29 17:10 Speter
 * Changed init classes to have a per-file static instance rather than use inheritance to
 * enforce order.
 * 
 * 1     96/05/13 18:43 Speter
 * New module, provide system-level init code.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_INITSYS_HPP
#define HEADER_LIB_STD_INITSYS_HPP

//**********************************************************************************************
//
class CInitSys
//
// The standard system-level init class.
//
// Explanation of init classes:
//		An init class uses C++ in a nifty way to provide for automatic invokation of 
//		initialisation (and termination) code for a module, without a main program needing to 
//		call it.  It also provides a way to specify order dependencies between init classes.
//
//		Automatic initialisation and termination is provided by declaring a class with a
//		constructor and destructor (and no other data or function members), and then 
//		declaring a static instance of that class.
//
//		Order dependency is provided by declaring a static instance of the class in the header 
//		file, so that every module which includes it will have its own copy.  A module's global
//		variables are constructed in the order declared, so the order of include files will
//		correspond to the order of initialisation of modules.  The destructors will run in the
//		reverse order.
//		
//		Since there will be multiple instances of an init class, but we only want the 
//		initialisation (and termination) code to run once, the constructors and destructors must 
//		contain static flags to ensure they execute only once.
//
//		"InitSys.hpp" is included by "Common.hpp", the first user include file of every module,
//		and therefore this module will be initialised before all others.  Anyone depending on
//		a particular module being initialised before its own static initialisation should include
//		that module's header file.
//
//		All init classes should be called CInit(something), and should be constructed along the
//		model of CInitSys.
//
//		Currently, CInitSys does the following:
//			Calls _set_new_handler to call TerminalError() when operator new fails.
//			Calls _set_new_mode(1) to cause malloc() to call _new_handler() when it fails.
//
//**************************
{
public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CInitSys();
	~CInitSys();
};


// The per-file instance of this init class.
static CInitSys initSys;

//**********************************************************************************************
//
template<class T> inline void InitPtr
(
	T*&	pt								// A pointer needing initialising.
)
//
//	Allocates the pointer if it has not been already.
//
//	Notes:
//		This function invokes only the default constructor for the variable.
//
//**************************************
{
	if (!pt)
		pt = new T;
}

//**********************************************************************************************
//
template<class T> class CInitPtr
//
// An init class to init a global pointer variable.
//
// Example:
//		// Declare a global pointer variable needing initialising.
//		extern CExample* pexVar;
//
//		// Declare a per-file instance of the class to initialise it.
//		static CInitPtr<CExample> initPtr(pexVar);
//
// Notes:
//		This class only invokes the default constructor for the pointer.
//
//**************************************
{
private:
	T*&		ptPointer;					// The pointer to initialise.

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CInitPtr
	(
		T*& pt							// Pointer to a variable needing initialising.
	)
		: ptPointer(pt)
	{
		InitPtr(pt);
	}

	~CInitPtr()
	{
		// Release the pointer, and set it to 0, so this can occur multiple times.
		delete ptPointer;
		ptPointer = 0;
	}
};

#endif
