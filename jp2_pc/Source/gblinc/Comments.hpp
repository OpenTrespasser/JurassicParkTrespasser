/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Comments on programming or project issues of a general nature.
 *		NOT included in any source file.  Here just for documentation purposes.
 *		Identify each section with an appropriate title that can be referred to in actual
 *		source comments.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GblInc/Comments.hpp                                                   $
 * 
 * 1     97-05-07 17:29 Speter
 * Nice repository for general programming comments.
 * 
 **********************************************************************************************/

#error Don't include or compile me.

/*
	Optimal virtual function calls.
	-------------------------------

	The following technique allows virtual function calls to work the way they should: if a
	class's implementation of a virtual function has no overrides from subclasses, then a call
	to that function from an object explicitly of that class should be non-virtual, and inline
	if possible.

	Example:
		
		class CBase
		{
		public:
			virtual int iF() = 0;
		};

		class CDerived: public CBase
		{
		public:
			int iF() { return 3; }
		};

		CDerived d;
		CBase* pb = &d;

		int i = pb->iF();			// Called virtually.
		int i = d.iF();				// Called virtually, but should be called inline,
									// as CDerived::iF is not declared virtual.

	The solution is for every class to have a public non-virtual function which does the real work,
	and a private virtual function (of the same name with an appended underscore), which allows
	virtual function calls to work properly.

		class CBase
		{
		public:
			int iF() { return iF_(); }			// CBase::iF() is the publicly callable function.
												// In the base class, it always invokes the virtual version.
		private:
			virtual int iF_() = 0;				// Derived classes must override iF_().
		};

		class CDerived: public CBase
		{
		public:
			int iF() { return 3; }				// Publicly callable CDerived::iF() is inline.
		private:
			virtual int iF_() { return iF(); }	// Privately implement virtual iF_(), so it works
												// when called via a CBase.
		};


*/

