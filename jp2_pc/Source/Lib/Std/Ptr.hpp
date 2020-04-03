/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: Classes used for pointer wrapping, including reference counting pointers.
 *
 *		ptr_const<>
 *		ptr<>
 *			aptr<>
 *		CRefObj
 *		rptr_const<>
 *		rptr<>
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/Ptr.hpp                                                       $
 * 
 * 19    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 18    8/26/98 3:25p Asouth
 * Removed const conversion; made non-const conversion a const function
 * 
 * 17    97/12/19 20:10 Speter
 * Replaced cumbersome casts with good ol' K&R style casts, because they work with namespaces.
 * 
 * 16    97/06/23 20:29 Speter
 * Made CRefObj::uRefs public; rptr classes now manipulate uRefs inline, rather than via funcs,
 * hopefully improving inlining.  Removed CRefPtr base class, moved functionality directly into
 * rptr_const.
 * Replaced CNullObj* with SNullObj variable for null object; it is now initialised staticly,
 * and not destroyed, fixing an obscure memory leak at program end (and now comparisions of
 * rptrs against Null are a bit faster).
 * In special init functions, changed bool param to special enums in overloaded functions,
 * avoiding comparison, and making it a little clearer.
 * 
 * 15    5/26/97 1:34p Agrant
 * LINT fixes.
 * 
 * 14    5/13/97 10:51p Agrant
 * Added ZeroRefs() to allow different behavior for a CRefObj
 * when its refcount reaches zero.  Useful for memory-managed and
 * instanced CRefObj's.
 * 
 * 13    97/01/29 18:49 Speter
 * Made rptr casting functions take and return references to the same rptr, to avoid rptr copy
 * overhead.
 * 
 * 12    97/01/26 19:56 Speter
 * Changed rptrs to point to a special null object rather than 0 when null.  This eliminates a
 * test for 0 when reference counting.
 * 
 * 11    97/01/07 11:06 Speter
 * Added const versions of pointer classes, and many comments.
 * 
 * 10    97/01/02 16:36 Speter
 * Moved rptr tracking functions to .cpp file.
 * Added many comments.
 * 
 * 9     96/12/31 17:04 Speter
 * Added ptGet() to ptr<>.
 * 
 * 8     96/12/31 16:35 Speter
 * Added working rptr, based on CRefObj.
 * 
 * 7     96/11/27 19:28 Speter
 * Added useful ptr and optr classes.  Changed CRPtr to rptr, inherited from ptr.
 * 
 * 6     96/10/18 18:22 Speter
 * Added AddRef and SubRef functions to CRefObj.
 * 
 * 5     96/10/04 16:58 Speter
 * Removed private operator & that disabled address taking.
 * That's not really desired.
 * 
 * 4     9/26/96 4:08p Blee
 * Delete Assert() from proGetRef().
 * 
 * 3     96/09/18 14:37 Speter
 * First working implementation.  Added CRefObj class for reference-counted objects.  Changed
 * CRefPtr to CRPtr.
 * 
 * 2     96/09/16 11:39 Speter
 * Made some minor changes.
 * 
 * 1     96/09/13 16:43 Speter
 * First implementation.
 * 
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_PTR_HPP
#define HEADER_LIB_STD_PTR_HPP

// 'delete' is ambiguous or inaccessible; memory will not be freed if initialization throws an exception
#pragma warning(disable: 4292)

#define VER_TRACK_RPTR		VER_DEBUG

//**********************************************************************************************
//
template<class T> class ptr_const
//
// Prefix: p...
//
// A pointer wrapper class that automatically initialises the pointer to 0, and Asserts for 
// not 0 upon access.
//
// Notes:
//		Does *not* do any automatic deletion, or reference-counting.  That is handled by aptr 
//		and rptr respectively.
//
//		This class provides const access only to the object.  
//		The similar ptr<T> class provides non-const access.  
//
// Implementation notes on constness:
//
//		Separate ptr and ptr_const classes are necessary, because ptr<const T> does not work 
//		for implementing a const pointer.  The compiler does not really distinguish "const T" 
//		as a separate template type from "T", and does not respect its constness.  I don't know 
//		if this is a compiler bug or not.
//
//		ptr<T> does not inherit from ptr_const<T>.  That would be useless, because ptr<T> 
//		cannot inherit ptr_const<T>'s const access functions (e.g. const T* operator ->()).  
//		That is because when ptr<T> defines its (non-const) T* operator ->(), that name hides
//		the inherited version and makes it unavailable.  So ptr<T> would still have to redefine
//		the const T* operator->().
//
//		These classes "propagate" constness in a more complete and useful manner than straight
//		pointers.  As with straight pointers, ptr<T> provides read-write access to T, while
//		ptr_const<T> provides read-only access.  However, const ptr<T>, besides prohibiting
//		modification of T*, also prohibits modification of T.  That is, const ptr<T> is
//		equivalent to const ptr_const<T>.  Thus, if an entire structure is const, then all
//		the objects the structure points to (via ptr<T> members) will also be const.  The
//		same is not true for straight pointers.
//
//		All of these notes apply equally to the rptr and rptr_const classes below.
//
//**************************************
{
protected:
	const T* ptPtr;
		
public:

	//**********************************************************************************************
	//
	// Constructors and destructor.
	//

	forceinline ptr_const(const T* pt = 0)
		: ptPtr(pt)
	{
	}

	//**********************************************************************************************
	//
	// Assignment operators.
	//

	forceinline ptr_const<T>& operator =(const T* pt)
	{
		ptPtr = pt;
		return *this;
	}

	//**********************************************************************************************
	//
	// Conversions.
	//

	//
	// Provide operators which let class act like a pointer.
	// 

	forceinline operator const T*() const
	{
		return ptPtr;
	}

	//**********************************************************************************************
	//
	// Operators.
	//

	forceinline const T* operator ->() const
	{
		Assert(ptPtr);
		return ptPtr;
	}

	forceinline const T& operator *() const
	{
		Assert(ptPtr);
		return *ptPtr;
	}

	//**********************************************************************************************
	//
	// Member functions.
	//

	// Provide explicit access to pointer, for cases where compiler fails to do implicit conversion.

	forceinline const T* ptGet() const
	{
		return ptPtr;
	}
};

//**********************************************************************************************
//
template<class T> class ptr
//
// Prefix: p...
//
//**************************************
{
protected:
	T*	ptPtr;

public:
	//**********************************************************************************************
	//
	// Constructors and destructor.
	//

	forceinline ptr(T* pt = 0)
		: ptPtr(pt)
	{
	}

	//**********************************************************************************************
	//
	// Assignment operators.
	//

	forceinline ptr<T>& operator =(T* pt)
	{
		ptPtr = pt;
		return *this;
	}

	//**********************************************************************************************
	//
	// Conversions.
	//

	forceinline operator ptr_const<T>() const
	{
		return ptr_const<T>(ptPtr);
	}

/*	operator const T*() const
	{
		return ptPtr;
	} */

	forceinline operator T*() const
	{
		return ptPtr;
	}

	//**********************************************************************************************
	//
	// Operators.
	//

	forceinline const T* operator ->() const
	{
		Assert(ptPtr);
		return ptPtr;
	}

	forceinline T* operator ->()
	{
		Assert(ptPtr);
		return ptPtr;
	}

	forceinline const T& operator *() const
	{
		Assert(ptPtr);
		return *ptPtr;
	}

	forceinline T& operator *()
	{
		Assert(ptPtr);
		return *ptPtr;
	}

	//**********************************************************************************************
	//
	// Member functions.
	//

	forceinline const T* ptGet() const
	{
		return ptPtr;
	}

	forceinline T* ptGet()
	{
		return ptPtr;
	}
};

//**********************************************************************************************
//
template<class T> class aptr: public ptr<T>
//
// Prefix: p...
//
// A pointer wrapper which owns the pointed-to object, and deletes it upon destruction.
//
// Notes:
//		This pointer can be used in place of reference-counting for simple situations, such as
//		global pointers, or classes which contain pointers to objects that are *not shared*.
//		Unlike rptr, the target of aptr need not be derived from CRefObj.
//
//		Since this is an owned ptr (and is not reference-counted), it is not possible to copy objects 
//		of this class.  That would result in two aptrs, each of which would try to delete the same
//		ptr upon destruction.  (You can copy the _pointer_ value in and out, you just can't copy the
//		aptr object.)  To enforce this, the copy constructor and operator are private.
//
//**************************************
{
public:
	//**********************************************************************************************
	//
	// Constructors and destructor.
	//

	forceinline aptr(T* pt = 0)
		: ptr<T>(pt)
	{
	}

	forceinline ~aptr()
	{
		delete ptPtr;
	}

	//**********************************************************************************************
	//
	// Assignment operators.
	//

	forceinline aptr<T>& operator =(T* pt)
	{
		// Delete current object, assign new one.
		if (ptPtr != pt)
		{
			delete ptPtr;
			ptPtr = pt;
		}
		return *this;
	}

private:

	// Disable copy constructor and operator.
	aptr(const aptr<T>&);

	void operator =(const aptr<T>&);
};

//**********************************************************************************************
//
// Classes for rptr<>.
//


#if VER_TRACK_RPTR
	#define FN_TRACK_RPTR		;
#else
	#define FN_TRACK_RPTR		{}
#endif


//**********************************************************************************************
//
class CRefObj
//
// Prefix: ro
//
// Base class for all reference-counted objects.  
//
// Any class which wants to benefit from reference counting must have CRefObj as a base class.
// CRefObj works together with rptr<> to manage references.  See rptr<> for details.
//
//**************************************
{
private:

public:
	uint	uRefs;						// The number of rptrs to this object.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CRefObj()
		: uRefs(0)
	{
	}

	// Copying a CRefObj preserves the count; thus initialiation sets uRefs to 0.
	CRefObj(const CRefObj&)
		: uRefs(0)
	{
	}

	// Copying a CRefObj preserves the count, and is thus a null operation.
	CRefObj& operator =(const CRefObj&)
	{
		return *this;
	}

	// Virtual destructor, so that we can delete the object without knowing its exact type.
	virtual ~CRefObj()
	{
		Assert(uRefs == 0);
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	uint uNumRefs() const
	// 
	//**********************************
	{
		return uRefs;
	}

	//******************************************************************************************
	//
	virtual void ZeroRefs()
	// 
	//	Called when the refcount reaches zero.
	//
	//	Notes:
	//		Default is to delete the refobj, just like you'd expect.
	//		Can be overridden in cases where such deletion would be bad, as in
	//		memory-managed ref-objs and the like.
	//
	//**********************************
	{
		delete this;
	}

	//
	// These functions maintain a database of pointers, for rigorous checking in debug mode.
	//

	//******************************************************************************************
	//
	void CheckNewPointer()
	//
	// Adds this pointer to the database, making sure it is not already there.
	//
	//**********************************
		FN_TRACK_RPTR

	//******************************************************************************************
	//
	void CheckExistingPointer()
	//
	// Ensures this pointer is already in the database.
	//
	//**********************************
		FN_TRACK_RPTR

	//******************************************************************************************
	//
	void RemovePointer()
	//
	// Removes the pointer from the database, ensuring it was already there.
	//
	//**********************************
		FN_TRACK_RPTR

/*
protected:

	// delete is unavailable to the general public, because you should never delete these objects!
	// These objects delete themselves, through the rptr.
	void operator delete(void* pv)
	{
		::operator delete(pv);
	}

	//Unfortunately, CRefObjs are often deleted anyway,
	//especially when not used in an rptr context.
	//The compiler also complains when CRefObjs are used with
	//rptr_new and rptr_cast.

	//As a workaround until all that is sorted out,
	//using the delete operator as public is permitted.
*/
};

#if VER_TRACK_RPTR

//**********************************************************************************************
//
class CInitPtrTracker
//
// A class to initialise the CRefObj database before anyone uses it.
//
//**************************************
{
public:
	CInitPtrTracker();

	~CInitPtrTracker();
};

// Declare a static version of CInitPtrTracker in every module.
static CInitPtrTracker initPtrTracker;

// #if VER_TRACK_RPTR
#endif

//**********************************************************************************************
//
struct SNullObj
//
// This struct is a hack, but needed.  It mirrors a CRefObj without actually being one.  This is
// so that we can create a static SNullObj which is automatically initialised before any rptrs.
// Only if it's a simple struct, which is initialised with simple data, and without constructor
// calls, will this be so.
//
// An alternative would be to make it a CRefObj* which is initialised with a CPtrInit object.
// However, this caused problems, as on program exit, CPtrInit set the CRefObj* to 0 BEFORE other
// objects which relied on the pNullObj value destructed.
//
//**************************************
{
	void*	pVTbl;						// Match the vtbl in CRefObj.
	uint	uRefs;						// The ref count; initialised to a huge value in Ptr.cpp.
};

// Object used as the actual address of null pointers.
extern SNullObj		NullObj;
#define pNullObj	reinterpret_cast<CRefObj*>(&NullObj)

//**********************************************************************************************
//
class CRPtr0
//
// A dummy class used solely to represent a null rptr.
//
//**************************************
{
public:
	CRPtr0(){};  // this makes it clear to LINT that we do not need to init rptr0.
};

// The object used to assign null rptrs.
const CRPtr0	rptr0;

//**********************************************************************************************
//
template<class T> class rptr_const
//
// Prefix: p...
//
// A pointer wrapper class which counts references, automatically deleting objects when
// all pointers to it are destroyed.
//
// Notes:
//		The rptr_const class contains constant pointers.  The rptr derived class contains 
//		non-constant pointers.  Note, however, that rptr_const will still delete the pointed-
//		to object when the references go to zero.  Reference adjustment and deletion are
//		permitted on const objects.
//
//		The notes here otherwise apply to both classes.
//
//		All targets of rptr must inherit from CRefObj.  
//
//			The advantages of this are that:
//				Allows virtual destruction when using opaque pointers (see below).
//				Prevents explicit deletion of pointers (by protecting the delete function).
//					Without CRefObj, we could detect deletion of ref-allocated pointers only
//					at run-time, by overloading global delete to check against an rptr database
//					in debug mode.
//				Allows diagnostics to use RTTI to identify type name.
//
//			However, there are disadvantages:
//				Intrusive; requires reference-counted objects to inherit from CRefObj.
//				Requires all such objects to be always reference-counted 
//				(because deletion is protected).
//				Won't work for reference-counted arrays; need different method.
//
//		Opaque pointers.
//
//			We clearly must allow rptr to work with opaque pointers.  The solution is that
//			*all* rptrs are opaque.  Internally, they cast their pointer to a simple CRefObj*.
//
//			Advantages:
//
//				Much easier and more natural than having a separate opq_rptr type for opaque 
//				pointers, and converting between opq_rptr and rptr.
//
//			Disadvantages:
//
//				Every target of rptr<> must be blindly convertible to CRefObj; that is, it must
//				have CRefObj as its first base class.  This disallows multiple inheritance where 
//				rptr points to the second inherited type.
//
//				The rptr class cannot check this, as it must work without the definition of the type.
//				So to check this, all assignment of new pointers must be via the rptr_new helper 
//				function.
//
//		Conversions to regular pointers:
//			
//			This is allowed only via explicit member function call: ptPtr().  This is to minimise
//			inadvertant leaks or dangling pointers.
//
//			However, conversion must be allowed, because the rptr copy overhead is significant
//			in time-critical code (e.g. triangle).  Sorry, I tried to minimise it, but the compiler
//			constructs temporary rptrs on every function parameter pass, and insists on not inlining 
//			the constructor and destructor.  Even inlined, it wouldn't be very fast.
//
//			Please use this conversion only for temporary pointer copies, such as function params, 
//			or in structures that have a known temporary lifetime, and only when speed is critical.  
//			Do *not* convert an rptr to a raw pointer and then save it in some structure that sticks 
//			around.
//
//		Conversion from regular pointers:
//
//			This is not allowed in general; who knows where the pointer has been?  However,
//			note the following special functions.
//
//		Special functions:
//
//			rptr_new
//
//				A pseudo-keyword that must be used in place of "new" when creating new objects 
//				that go into an rptr.  Example:
//
//					rptr<T> pt = rptr_new T(args);
//
//				This is necessary to ensure that T is really derived from CRefObj (the rptr
//				class itself cannot do this, as it deals with types opaquely), and also to
//				check that the reference count is really 0.
//
//				rptr_new is implemented via a macro.
//
//			rptr_this(this)
//			rptr_const_this(this)
//
//				Converts the "this" pointer to an rptr of the same type.  (One of the few 
//				times it's valid to convert a regular pointer to an rptr).
//					
//			rptr_cast(T, rp)
//
//				Allows conversion between different rptrs, as this does not happen implicitly
//				(until we get member templates).  Example:
//
//					rptr<S> ps;
//					rptr<T> pt = rptr_cast(T, ps);
//					rptr<T> pt = rptr_cast(T, rptr_new S(args));
//
//				This works only if S* can be implicitly converted to T*.
//
//			rptr_static_cast(T, rp)
//			rptr_const_static_cast(T, rp)
//
//				Converts rp to an rptr<T> or rptr_const<T> via a static_cast.
//
//			rptr_dynamic_cast(T, rp)
//			rptr_const_dynamic_cast(T, rp)
//
//				Converts rp to an rptr<T> or rptr_const<T> via a dynamic_cast.
//
//			rptr_nonconst(rcp)
//
//				Converts an rptr_const<T> to rptr<T>.
//
//		Other notes:
//
//			You cannot assign 0 to an rptr.  Therefore, do not initialise the rptr to 0; its default
//			constructor does that.  To specifically assign or return 0, use the rptr0 constant.
//
//			You cannot explicitly delete an rptr (or any pointer to an object derived from CRefObj).
//			You can, however, assign rptr<T>() to an rptr, thus removing a reference.
//
//			Because implicit conversion to pointers is not allowed, operators are provided for 
//			common expressions using pointers: bool, ==
//
//**************************************
{
protected:
	CRefObj*		pRefObj;				// The pointer to the reference-counted object.

public:

	//**********************************************************************************************
	enum ENew	{ eNEW };
	enum EExist	{ eEXIST };

	//**********************************************************************************************
	//
	// Constructors and destructor.
	//

	// Constructors which initialise the pointer to pNullObj.
	forceinline rptr_const()
		: pRefObj(pNullObj)
	{
	}

	forceinline rptr_const(const CRPtr0&)
		: pRefObj(pNullObj)
	{
	}

	forceinline rptr_const(const rptr_const<T>& rp)
		: pRefObj(rp.pRefObj)
	//
	// Copy a pointer, and increase the ref count.
	//
	{
		// Assert that this object previously existed.
		Assert(pRefObj);
		pRefObj->CheckExistingPointer();

		// Add a reference.
		pRefObj->uRefs++;
	}

	forceinline rptr_const(ENew, const T* pt)
	//
	// Constructor used by friend functions.
	// Construct from a raw pointer, ensuring this is a new pointer.
	//
		: pRefObj((CRefObj*)pt)
	{
		// Assert that this object is new.
		Assert(pRefObj);
		pRefObj->CheckNewPointer();

		// Add a reference.
		pRefObj->uRefs++;
	}

	forceinline rptr_const(EExist, const T* pt)
	//
	// Constructor used by friend functions.
	// Construct from a raw pointer, ensuring this is an old pointer.
	//
		: pRefObj((CRefObj*)pt)
	{
		// Assert that this object previously existed.
		Assert(pRefObj);
		pRefObj->CheckExistingPointer();

		// Add a reference.
		pRefObj->uRefs++;
	}

	forceinline ~rptr_const()
	// Decrement the reference count, and delete if 0.  Replace with null pointer.
	{
		// Assert that this object previously existed.
		Assert(pRefObj);
		Assert(pRefObj->uRefs > 0);

		// Dereference the object.
		if (--pRefObj->uRefs == 0)
		{
			pRefObj->RemovePointer();
			pRefObj->ZeroRefs();
		}
		pRefObj = pNullObj;
	}

	//**********************************************************************************************
	//
	// Assignment operators.
	//

	forceinline rptr_const<T>& operator =(const rptr_const<T>& rp)
	{
		//
		// First increment the source reference, then decrement the target reference,
		// then copy the pointers.  This sequence is necessary to avoid deletion in
		// self-assignment.
		//

		// Assert that this object previously existed.
		Assert(rp.pRefObj);
		rp.pRefObj->CheckExistingPointer();
		rp.pRefObj->uRefs++;

		// Dereference the object.
		Assert(pRefObj);
		Assert(pRefObj->uRefs > 0);
		if (--pRefObj->uRefs == 0)
		{
			pRefObj->RemovePointer();
			pRefObj->ZeroRefs();
		}

		// Now copy.
		pRefObj = rp.pRefObj;
		return *this;
	}

	//******************************************************************************************
	//
	// Conversions.
	//

	//
	// Provide common conversions, since there is no explicit conversion to pointer.
	//

	forceinline operator bool () const
	{
		return pRefObj != pNullObj;
	}

	//******************************************************************************************
	//
	// Operators.
	//

	forceinline bool operator == (const rptr_const<T>& rp) const
	{
		return pRefObj == rp.pRefObj;
	}

	//
	// Duplicate ptr<> functionality.
	//

	forceinline const T* operator ->() const
	{
		Assert(bool(*this));
		return ptPtrRaw();
	}

	forceinline const T& operator *() const
	{
		Assert(bool(*this));
		return *ptPtrRaw();
	}

	//**********************************************************************************************
	//
	// Member functions.
	//

	// Provide explicit access to pointer.

	forceinline const T* ptPtrRaw() const
	// Return raw pointer, cast to T*.
	{
		Assert(pRefObj);
		return reinterpret_cast<const T*>((const CRefObj*)pRefObj);
	}

	forceinline const T* ptGet() const
	// Return pointer, cast to T*, and converted to 0 if pNullObj.
	{
		return bool(*this) ? ptPtrRaw() : 0;
	}
};

//**********************************************************************************************
//
// Global functions for rptr_const.
//

	//******************************************************************************************
	//
	template<class T> inline rptr_const<T> rptr_const_this
	(
		const T* pt_this							// A "this" pointer of type T.
	)
	//
	// Returns:
	//		An rptr<T> initialised with pt_this.
	//
	// Notes:
	//		This function is for those rare cases when a "this" pointer must be passed to a
	//		function expecting an rptr.
	//
	//**************************************
	{
		Assert(pt_this);
		return rptr_const<T>(rptr_const<T>::eEXIST, pt_this);
	}

	//******************************************************************************************
	//
	template<class T, class S> inline rptr_const<T>& rptr_const_static_cast_fn
	(
		T* pt,								// Dummy pointer of type T.
		const rptr_const<S>& rp				// rptr to cast.
	)
	//
	// Returns:
	//		A reference to the same const_rptr, cast to type T.
	//
	// Notes:
	//		This function takes and returns references to avoid rptr copy overhead.
	//		It only works if type S* can be staticly converted to T*.
	//
	//**************************************
	{
		// Check type compatibility in debug mode.
		Assert(static_cast<const T*>(rp.ptPtrRaw()));

		// Pretend the rptr is something else.
		return (rptr_const<T>&) rp;
	}

	#define rptr_const_static_cast(T, rp)	rptr_const_static_cast_fn((T*)0, rp)

	//******************************************************************************************
	//
	template<class T, class S> rptr_const<T> rptr_const_dynamic_cast_fn
	(
		T* pt,								// Dummy type param.
		const rptr_const<S>& rps			// rptr_const to convert.
	)
	//
	// Returns:
	//		An rptr_const<T> initialised with the dynamic_cast value of rps.
	//
	//**************************************
	{
		// Note const_cast necessary because of 4.2 compiler bug.
		pt = dynamic_cast<T*>(const_cast<S*>(rps.ptPtrRaw()));
		return rptr_const<T>(rptr_const<T>::eEXIST, pt ? pt : reinterpret_cast<T*>(pNullObj));
	}

	#define rptr_const_dynamic_cast(T, rp)	rptr_const_dynamic_cast_fn((T*)0, rp)

//**********************************************************************************************
//
template<class T> class rptr: public rptr_const<T>
//
// Prefix: p...
//
// A non-const version of rptr_const<>.
//
//**************************************
{
public:

	//**********************************************************************************************
	//
	// Duplicate constructors.
	//

	forceinline rptr()
	{
	}

	forceinline rptr(const CRPtr0&)
	{
	}

	forceinline rptr(const rptr<T>& rp)
		: rptr_const<T>(rp)
	{
	}

	forceinline rptr(EExist, T* pt)
		: rptr_const<T>(eEXIST, pt)
	{
	}

	forceinline rptr(ENew, T* pt)
		: rptr_const<T>(eNEW, pt)
	{
	}

	//**********************************************************************************************
	//
	// Operators.
	//

	//
	// Add non-constant access.
	// Note that we must duplicate the functions inherited from rptr_const as well, because declaring
	// the non-const versions hides access to the inherited versions.
	//

	forceinline const T* operator ->() const
	{
		Assert(bool(*this));
		return ptPtrRaw();
	}

	forceinline T* operator ->()
	{
		Assert(bool(*this));
		return ptPtrRaw();
	}

	forceinline const T& operator *() const
	{
		Assert(bool(*this));
		return *ptPtrRaw();
	}

	forceinline T& operator *()
	{
		Assert(bool(*this));
		return *ptPtrRaw();
	}

	//**********************************************************************************************
	//
	// Member functions.
	//

	// Provide explicit access to pointer.

	forceinline const T* ptPtrRaw() const
	// Return raw pointer, cast to T*.
	{
		Assert(pRefObj);
		return (const T*)pRefObj;
	}

	forceinline T* ptPtrRaw()
	// Return raw pointer, cast to T*.
	{
		Assert(pRefObj);
		return (T*)pRefObj;
	}

	forceinline const T* ptGet() const
	{
		return bool(*this) ? ptPtrRaw() : 0;
	}

	forceinline T* ptGet()
	{
		return bool(*this) ? ptPtrRaw() : 0;
	}
};

//**********************************************************************************************
//
// Global functions for rptr.
//

	//******************************************************************************************
	//
	template<class T> inline rptr<T> rptr_this
	(
		T* pt_this							// A "this" pointer of type T.
	)
	//
	// Returns:
	//		An rptr<T> initialised with pt_this.
	//
	// Notes:
	//		This function is for those rare cases when a "this" pointer must be passed to a
	//		function expecting an rptr.
	//
	//**************************************
	{
		Assert(pt_this);
		return rptr<T>(rptr<T>::eEXIST, pt_this);
	}

	//******************************************************************************************
	//
	struct SRPtrNew
	//
	//**********************************
	{
	};

	//******************************************************************************************
	//
	template<class T> inline rptr<T> operator *
	(
		SRPtrNew,							// Dummy object.
		T* pt								// Newly allocated pointer.
	)
	//
	// Returns:
	//		An rptr<T> initialised with pt.
	//
	// Notes:
	//		This function provides the only way to create an rptr<T> containing a new pointer.
	//		The purpose of this function is to ensure that T has CRefObj as its first base class.
	//		The reason this function is a friend rather than a constructor of rptr is that rptr
	//		is designed to work with opaque T*, and this function will not.
	//
	//**************************************
	{
		Assert(pt);
	#if VER_DEBUG
		//	Test whether pt is truly derived from CRefobj, and exists at the same address.
		//	Assert if not.
		CRefObj* pro = pt;
		Assert((void*)pro == (void*)pt);
	#endif
		return rptr<T>(rptr<T>::eNEW, pt);
	}

	#define rptr_new					SRPtrNew() * new

	//******************************************************************************************
	//
	template<class T, class S> inline rptr<T>& rptr_cast_fn
	(
		const T* pt,						// Dummy pointer of type T.
		const rptr<S>& rp					// rptr to cast.
	)
	//
	// Returns:
	//		A reference to the same rptr, cast to type T.
	//
	// Notes:
	//		This function takes and returns references to avoid rptr copy overhead.
	//		It only works if type S* can be implicitly converted to T*.
	//
	//**************************************
	{
		// Check type compatibility in debug mode.
		Assert(pt = rp.ptPtrRaw());

		// Pretend the rptr is something else.
		return (rptr<T>&) rp;
	}

	#define rptr_cast(T, rp)			rptr_cast_fn((const T*)0, rp)

	//******************************************************************************************
	//
	template<class T, class S> inline rptr<T>& rptr_static_cast_fn
	(
		T* pt,								// Dummy pointer of type T.
		const rptr<S>& rp					// rptr to cast.
	)
	//
	// Returns:
	//		A reference to the same rptr, cast to type T.
	//
	// Notes:
	//		This function takes and returns references to avoid rptr copy overhead.
	//		It only works if type S* can be staticly converted to T*.
	//
	//**************************************
	{
		// Check type compatibility in debug mode.
		Assert(static_cast<const T*>(rp.ptPtrRaw()));

		// Pretend the rptr is something else.
		return (rptr<T>&) rp;
	}

	#define rptr_static_cast(T, rp)		rptr_static_cast_fn((T*)0, rp)

	//******************************************************************************************
	//
	template<class T, class S> rptr<T> rptr_dynamic_cast_fn
	(
		T* pt,								// Dummy pointer of type T.
		rptr<S>& rp							// rptr to cast.
	)
	//
	// Returns:
	//		A copy of rp, dynamicly cast to rptr<T>.
	//
	// Notes:
	//		As per the rptr convention, if the dynamic_cast returns 0, the rptr is
	//		assigned a value of pNullObj.
	//
	//**************************************
	{
		pt = dynamic_cast<T*>(rp.ptPtrRaw());
		return rptr<T>(rptr<T>::eEXIST, pt ? pt : reinterpret_cast<T*>(pNullObj));
	}

	#define rptr_dynamic_cast(T, rp)	rptr_dynamic_cast_fn((T*)0, rp)

	//******************************************************************************************
	//
	template<class T> inline rptr<T>& rptr_nonconst
	(
		const rptr_const<T>& rp
	)
	//
	// Returns:
	//		A reference to the same const_rptr, cast to rptr.
	//
	// Notes:
	//		This function takes and returns references to avoid rptr copy overhead.
	//		As with regular const_cast, use of this conversion is discouraged.
	//
	//**************************************
	{
		return (rptr<T>&) rp;
	}

#endif
