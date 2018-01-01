/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Support for private implementation classes.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/PrivSelf.hpp                                                  $
 * 
 * 1     98/02/20 18:58 Speter
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_PRIVSELF_HPP
#define HEADER_LIB_STD_PRIVSELF_HPP

//**********************************************************************************************
//
// Declarations needed to simplify use of private implementation classes.
//

#define priv_self		static_cast<CPriv&>(*this)

//#define priv_selfc	static_cast<const CPriv&>(*this)
//
// The above version breaks under MSVC 4.2, so I must use the less-safe one below.
//
#define priv_selfc		((const CPriv&)*this)

//
// Notes:
//		This is part of a strategy for keeping private implementation details out of header files.
//		One way to do this is to make the interface class a "pure" abstract interface: all functions
//		virtual, no member data.  The implementation is then provided by derived classes.  However,
//		for many heavy-use classes, this is unacceptable in terms of performance.  That's why we're
//		still using C++ instead of Java. For performance reasons, we still need classes that have 
//		non-virtual functions; and for classes with possible local allocation, the instance data 
//		(even private) must still be declared in the interface.  
//
//		However, we would like to remove private member functions, both regular and static, and
//		private static data from the header file.  These items do not affect the external use of
//		the class, yet C++ requires that they be declared in the class.  One often-used strategy
//		is to move these items out of the class, and make them global items private to the 
//		implementation file.  But this has the undesirable result of removing the "connection"
//		from the class, in terms of scoping, access rules, and the convenience of the "this" pointer.
//
//		The solution, via example:
//
//			// X.hpp.
//
//				class CX
//				{
//				public:
//					// Data and functions.
//					void Example();
//					int iGetTotal() const;
//
//				private:
//					// Instance data.
//
//					class CPriv;
//					friend class CPriv;
//
//					//
//					// CPriv is defined in the implementation file, and contains CX's private member 
//					// functions and static data.  It is derived from CX so that it has access to CX's 
//					// facilities via the convenient "this" pointer. Deriving from CX also allows
//					// the above macros to work, and reduces global namespace clutter.
//					//
//					// It is a friend so it can access CX's private instance data.
//					// This is unnecessary if CX's instance data is protected rather than private.
//					//
//				};
//
//			// X.cpp.
//
//				class CX::CPriv: CX				// Don't need public derivation.
//				{
//					friend class CX;			// Let CX access our stuff, but leave everything private
//												// so no one else in the same file can.
//
//					// Declare functions and static data for CX's implementation.
//					void Helper()
//					{
//						...
//					}
//
//					static int iTotal;
//				};
//
//				//
//				// Implement CX's public functions.
//				// When referencing a private member from CPriv, prefix it with priv_self or priv_selfc.
//				//
//
//				void CX::Example()
//				{
//					...
//					priv_self.Helper();				// Call a private function.
//					...
//				}
//
//				int CX::iGetTotal() const
//				{
//					return priv_selfc.iTotal;		// Reference private static data.
//				}
//
//		Note: While ideally it would be possible to use a single "priv_self" symbol which invokes 
//		overloaded functions for both const and non-const this, in VC 4.2 it fails due to the 
//		compiler's faulty overload resolution with templates.
//				
//
//**************************************


#endif
