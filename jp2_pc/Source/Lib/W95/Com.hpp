/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CCom<T>
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/W95/Com.hpp                                                      $
 * 
 * 5     8/20/98 1:32p Pkeet
 * Added the 'SafeRelease' member function.
 * 
 * 4     96/12/31 17:15 Speter
 * Changed rAssign to Assign, with self-copy check.
 * 
 * 
 * 3     96/11/27 19:31 Speter
 * Changed CCom to inherit from ptr<>.
 * 
 * 2     96/08/14 12:19 Speter
 * Minor changes.
 * 
 * 1     96/08/09 10:55 Speter
 * Wrap COM functionality, w/ automatic AddRef/Release.
 * 
 **********************************************************************************************/

#ifndef HEADER_W95_COM_HPP
#define HEADER_W95_COM_HPP

void ComAddRef(void*);
void ComRelease(void*);

//**********************************************************************************************
//
template<class T> class CCom: public ptr<T>
//
// Prefix: com
//
// Encapsulate a COM object as a smart pointer, with automatic release.
// Identical in functionality to the rptr<> class, but uses COM functions for reference counting.
//
//**************************************
{
public:
	//**********************************************************************************************
	//
	// Constructors and destructor.
	//

	CCom()
	{
		ptPtr = 0;
	}

	CCom(T* pt)
	// Initialise with a newly allocated pointer.
	// Ensure that this was created with new(CRefCount), and that the count is only 1.
	{
		ptPtr = pt;
	}

	CCom(const CCom<T>& rp)
	// Copy a pointer, and increase the ref count.
	{
		ptPtr = rp.ptPtr;
		ComAddRef(ptPtr);
	}

	~CCom()
	// Decrement the reference count, and delete if 0.
	{
		ComRelease(ptPtr);
		ptPtr = 0;
	}

	//**********************************************************************************************
	//
	// Assignment operators.
	//

	CCom<T>& operator =(const CCom<T>& cp)
	{
		if (ptPtr != cp.ptPtr)
			Assign(*this, cp);
		return *this;
	}

	CCom<T>& operator =(T* pt)
	{
		if (ptPtr != pt)
			Assign(*this, pt);
		return *this;
	}

	//******************************************************************************************
	//
	// Conversion operators.
	//

	// Useful for passing to Create or QueryInterface functions.
	T** operator &()
	{
		return &ptPtr;
	}

	T* const* operator &() const
	{
		return &ptPtr;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SafeRelease
	(
	)
	//
	// Releases the com object, verifies that the reference count is one, and sets the pointer
	// to null.
	//
	//**************************************
	{
		if (ptPtr)
		{
			ptPtr->Release();
			ptPtr = 0;
		}
	}

};


#endif

