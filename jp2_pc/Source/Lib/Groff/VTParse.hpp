/***********************************************************************************************
 *
 * Copyright (c) 1997 DreamWorks Interactive
 *
 * Contents: Functions for the quick and easy parsing of value tables.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/VTParse.hpp                                                 $
 * 
 * 10    98.08.22 11:31p Mmouni
 * Fixed problem with lookup object value.
 * 
 * 9     98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 8     7/23/98 9:34p Agrant
 * Fail gracefully when you get an int when you expect a float
 * 
 * 7     98.06.04 10:32p Mmouni
 * Changed IF_SETUP_OBJECT_SYMBOL() macro to work with nested objects.
 * 
 * 6     11/18/97 4:27p Agrant
 * better set of macros for parsing text props, part II
 * 
 * 5     10/28/97 7:16p Agrant
 * Added include for loader.hpp
 * 
 * 4     10/22/97 9:33p Agrant
 * Improved handling of non-nested objects.
 * 
 * 3     10/22/97 5:48p Agrant
 * Hacks to handle non-nested objects in text props nicely.
 * 
 * 2     10/21/97 11:55p Gstull
 * changed cFillChar to bFillChar
 * added "bool" to declarations of all bFillXXX functions
 * 
 * 1     10/14/97 9:12p Agrant
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GROFF_VTPARSE_HPP
#define HEADER_LIB_GROFF_VTPARSE_HPP

#include "ValueTable.hpp"

#include "Lib/Loader/Loader.hpp"


// A quick and dirty time-saver.
inline bool bMatches(const CBaseValue* pbv, const char *str_prop_name, CValueTable* pvt)
{
	return	(((CStringValue*)&(*pvt)[pbv->hSymbol()])->estrValue() == CEasyString(str_prop_name));
}


// Quick ways to get data from a value class.
// There may well be better ways to do this, but this is fast and safe.

inline bool bGetBool	(const CBaseValue *pbv) 
{
	Assert(pbv->evtType() == evtBOOL);
	return ((CBoolValue*)pbv)->bValue();
}

inline char cGetChar	(const CBaseValue *pbv) 
{
	Assert(pbv->evtType() == evtCHAR);
	return ((CCharValue*)pbv)->cValue();
}

inline int iGetInt	(const CBaseValue *pbv) 
{
	Assert(pbv->evtType() == evtINT);
	return ((CIntValue*)pbv)->iValue();
}

inline float fGetFloat	(const CBaseValue *pbv) 
{
	Assert(pbv->evtType() == evtFLOAT);
	return ((CFloatValue*)pbv)->fValue();
}

inline const CEasyString& estrGetString	(const CBaseValue *pbv) 
{
	Assert(pbv->evtType() == evtSTRING);
	return ((CStringValue*)pbv)->estrValue();
}

inline CObjectValue* povalCast(CBaseValue* pbv)
{
	Assert(pbv->evtType() == evtOBJECT);
	return (CObjectValue*)pbv;
}


inline CBaseValue* pbvLookupValue
(
	const CHandle&		h_symbol,
	CObjectValue*		poval_parent,
	CValueTable*		pvtable,
	int					i_start
)
{
	if (!poval_parent)
		return false;

	// Don't start off the end of the array.
	if (i_start >= poval_parent->uCount())
		i_start = 0;

	CBaseValue* pbv;
	int i = i_start;

	do 
	{
		// Check to see if we've found the guy we want.
		pbv = &(*pvtable)[poval_parent->hElementHandle(i)];

		if (h_symbol == pbv->hSymbol())
		{
			return pbv;
		}
		else
		{
			// If not, try the next guy.
			i++;

			// Don't read off the end of the array.
			if (i >= poval_parent->uCount())
				i = 0;
		}
	}
	while (i_start != i);

	// Looped all the way, found nothing.
	return 0;
}


// A bunch of inline functions to snag properties.
inline bool bFillBool
(
	bool *pb_value,						// The value to set.
	const CHandle& h_symbol,			// The symbol to find.
	CObjectValue* poval_parent,	// The object containing the value.
	CValueTable* pvtable,			// The vtable to use.	
	int *pi_best_guess_index			// The best place to start looking for the symbol.
)
//  Returns true if successful, else false.  If false, does not modify "*b"
{
	if (!poval_parent)
		return false;

	CBaseValue* pbv = pbvLookupValue(h_symbol, poval_parent, pvtable, *pi_best_guess_index);

	if (pbv && pbv->evtType() == evtBOOL)
	{
		// Found the symbol!

		// Stuff the return value.
		// We are expecting a bool.
		*pb_value = bGetBool(pbv);

		// Look at the next property next time.
		(*pi_best_guess_index)++;

		// Let the caller know that we have succeeded.
		return true;
	}
	else
		return false;
}

inline bool bFillChar
(
	char *pc_value,						// The value to set.
	const CHandle& h_symbol,			// The symbol to find.
	CObjectValue* poval_parent,	// The object containing the value.
	CValueTable* pvtable,			// The vtable to use.	
	int *pi_best_guess_index			// The best place to start looking for the symbol.
)
//  Returns true if successful, else false.  If false, does not modify "*b"
{
	if (!poval_parent)
		return false;

	CBaseValue* pbv = pbvLookupValue(h_symbol, poval_parent, pvtable, *pi_best_guess_index);

	if (pbv && pbv->evtType() == evtCHAR)
	{
		// Found the symbol!

		// Stuff the return value.
		// We are expecting a bool.
		*pc_value = cGetChar(pbv);

		// Look at the next property next time.
		(*pi_best_guess_index)++;

		// Let the caller know that we have succeeded.
		return true;
	}
	else
		return false;
}

inline bool bFillInt
(
	int *pi_value,						// The value to set.
	const CHandle& h_symbol,			// The symbol to find.
	CObjectValue* poval_parent,	// The object containing the value.
	CValueTable* pvtable,			// The vtable to use.	
	int *pi_best_guess_index			// The best place to start looking for the symbol.
)
//  Returns true if successful, else false.  If false, does not modify "*b"
{
	if (!poval_parent)
		return false;

	CBaseValue* pbv = pbvLookupValue(h_symbol, poval_parent, pvtable, *pi_best_guess_index);

	if (pbv && pbv->evtType() == evtINT)
	{
		// Found the symbol!

		// Stuff the return value.
		// We are expecting a bool.
		*pi_value = iGetInt(pbv);

		// Look at the next property next time.
		(*pi_best_guess_index)++;

		// Let the caller know that we have succeeded.
		return true;
	}
	else
		return false;
}

inline bool bFillFloat
(
	float *pf_value,						// The value to set.
	const CHandle& h_symbol,			// The symbol to find.
	CObjectValue* poval_parent,	// The object containing the value.
	CValueTable* pvtable,			// The vtable to use.	
	int *pi_best_guess_index			// The best place to start looking for the symbol.
)
//  Returns true if successful, else false.  If false, does not modify "*b"
{
	if (!poval_parent)
		return false;

	CBaseValue* pbv = pbvLookupValue(h_symbol, poval_parent, pvtable, *pi_best_guess_index);

	if (pbv && pbv->evtType() == evtFLOAT)
	{
		// Found the symbol!

		// Stuff the return value.
		// We are expecting a bool.
		*pf_value = fGetFloat(pbv);

		// Look at the next property next time.
		(*pi_best_guess_index)++;

		// Let the caller know that we have succeeded.
		return true;
	}
	else
		return false;
}

inline bool bFillString
(
	const CEasyString **ppestr_value,						// The value to set.
	const CHandle& h_symbol,			// The symbol to find.
	CObjectValue* poval_parent,	// The object containing the value.
	CValueTable* pvtable,			// The vtable to use.	
	int *pi_best_guess_index			// The best place to start looking for the symbol.
)
//  Returns true if successful, else false.  If false, does not modify "*b"
{
	if (!poval_parent)
		return false;

	CBaseValue* pbv = pbvLookupValue(h_symbol, poval_parent, pvtable, *pi_best_guess_index);

	if (pbv && pbv->evtType() == evtSTRING)
	{
		// Found the symbol!

		// Stuff the return value.
		// We are expecting a bool.
		*ppestr_value = &estrGetString(pbv);

		// Look at the next property next time.
		(*pi_best_guess_index)++;

		// Let the caller know that we have succeeded.
		return true;
	}
	else
		return false;
}

bool bGetObject
(
	CObjectValue **ppoval_value,			// The value to set.
	const CHandle& h_symbol,			// The symbol to find.
	CObjectValue* poval_parent,	// The object containing the value.
	CValueTable* pvtable,			// The vtable to use.	
	int *pi_best_guess_index			// The best place to start looking for the symbol.
);


#define bFILL_OBJECT(varname, symbol)\
	bGetObject(&varname, __pload->hSymbol(symbol), __pov, __pvt, &__i)

#define bFILL_pEASYSTRING(varname, symbol)\
	bFillString(&varname, __pload->hSymbol(symbol), __pov, __pvt, &__i)

#define bFILL_INT(varname, symbol)\
	bFillInt(&varname, __pload->hSymbol(symbol), __pov, __pvt, &__i)

#define bFILL_BOOL(varname, symbol)\
	bFillBool(&varname, __pload->hSymbol(symbol), __pov, __pvt, &__i)

#define bFILL_FLOAT(varname, symbol)\
	bFillFloat(&varname, __pload->hSymbol(symbol), __pov, __pvt, &__i)




//
//
//   Each macro has several forms:  SETUP_foo			// always executes enclosed block
//									IF_SETUP_foo		// only executes enclosed block if setup works
//									END_foo				// ends the block
//
//

// Sets up the text processing macros.  Must always be used to allow text processing.
#define SETUP_TEXT_PROCESSING(pvtable, pload)\
{\
	Assert(pvtable);  Assert(pload);\
	CValueTable*	__pvt	= pvtable;\
	CLoadWorld*		__pload = pload;\
	CObjectValue*	__pov	= 0;

// Cannot fail text processing setup.
#define IF_SETUP_TEXT_PROCESSING(pvtable, pload)  SETUP_TEXT_PROCESSING(pvtable, pload)  

// End the text processing block.
#define END_TEXT_PROCESSING  }



// Sets up an already existing object value pointer.
#define SETUP_OBJECT_POINTER(pov)\
	{\
		CObjectValue* __pov_parent = __pov;\
		int __i = 0;\
		__pov = pov;\
		if (!__pov)\
			__pov = __pov_parent;
// The IF statement here is for faking nested objects.  Take it out when they exist and are used.

// Sets up an already existing object value pointer.
#define IF_SETUP_OBJECT_POINTER(pov)\
	if (pov)\
	{\
		CObjectValue* __pov_parent = __pov;\
		int __i = 0;\
		__pov = pov;\
// The last IF statement here is for faking nested objects.  Take it out when they exist and are used.

// Ends any object block.
#define END_OBJECT   __pov = __pov_parent;  }
#define END_OBJECT_POINTER   END_OBJECT__pov = __pov_parent;  }


// Sets up the object with the specified handle.
#define SETUP_OBJECT_HANDLE(handle)\
	{\
		CObjectValue* __pov_parent = __pov;\
		if (handle != 0) __pov = povalCast(&(*__pvt)[handle]);\
		int __i = 0;

#define IF_SETUP_OBJECT_HANDLE(handle)\
	if (handle != 0)\
	{\
		CObjectValue* __pov_parent = __pov;\
		__pov = povalCast(&(*__pvt)[handle]);\
		int __i = 0;

#define END_OBJECT_HANDLE END_OBJECT



// Continues into the bracketed area even if no object is found.
#define SETUP_OBJECT_SYMBOL(symbol)\
		{\
			int __i = 0;\
			CObjectValue* __pov_parent = __pov;\
			if (!bFILL_OBJECT(__pov, symbol))\
				__pov = __pov_parent;

//  Gets the object with "symbol" as its symbol.  Sets it up for use.
#define IF_SETUP_OBJECT_SYMBOL(symbol)\
		{\
			int __i = 0;\
			CObjectValue* __pov_parent = __pov;\
			if (bFILL_OBJECT(__pov, symbol))\

#define END_OBJECT_SYMBOL END_OBJECT






#endif