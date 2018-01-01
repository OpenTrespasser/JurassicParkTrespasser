/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CPArray2<T>			Handles pre-allocated 2D array.
 *			CAArray2<T>		2D array that is released upon destruction.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/Array2.hpp                                                    $
 * 
 * 8     98/03/10 21:16 Speter
 * Added default constructor, and CAArray2 = CPArray2 assignment operator.
 * 
 * 7     97/11/10 17:28 Speter
 * Now allow 0-sized arrays.
 * 
 * 6     97/08/14 10:28 Speter
 * Allow for negative stride (for Y-downward rasters).
 * 
 * 5     97/08/12 23:40 Speter
 * Fixed bug in pa2Rect().
 * 
 * 4     97/07/23 17:57 Speter
 * Fixed bug in constructors taking specified memory block; didn't set the memory!  Removed
 * Set() functions, as regular assignment does the job.  
 * 
 * 3     5/26/97 1:35p Agrant
 * LINT does not like the chained [][] notation, I think because of our use
 * of temporaries.  Use the function call for 2d array elements to avoid
 * LINT complaints.
 * 
 * 2     97-05-06 15:48 Speter
 * Made CPArray2 usable as base class for raster.  Renamed dimensions to iHeight and iWidth,
 * changed argument names to reflect accessing in [y][x] order.  Added Fill() functions, one
 * taking an SRect.  Moved SRect here from Raster.hpp.  Added other functionality.  
 * 
 * 1     96/11/05 20:47 Speter
 * 
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_ARRAY2_HPP
#define HEADER_LIB_STD_ARRAY2_HPP

#include "Array.hpp"

//**********************************************************************************************
//
struct SRect
//
// Simple rectangle structure, used in CPArray2.
//
// Prefix: rect
//
//**************************************
{
	int		iX, iY;
	int		iWidth, iHeight;

	SRect(int i_x, int i_y, int i_w, int i_h)
		: iX(i_x), iY(i_y), iWidth(i_w), iHeight(i_h)
	{
	}
};


//**********************************************************************************************
//
template<class T> class CPArray2: public CPArray<T>
//
// Prefix: pa2
//
// A 2-dimensional array, similar to CPArray.  
//
// Can be accessed like a regular C 2D array: pa2[i_a][i_b].
//
// In order to facilitate use as a video raster, the naming conventions used herein reflects the 
// [i_y][i_x] indexing order.  Thus, the first dimension is named iHeight, and the second iWidth.
//
//**************************************
{
public:
	int		iHeight;					// Dimension of first index (y).
	int		iWidth;						// Dimension of second index (x).
	int		iStride;					// Actual distance between adjacent y's.
										// (Defaults to iWidth).

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CPArray2()
		: iHeight(0), iWidth(0), iStride(0)
	{
	}

	// Initialise with a preallocated array, and size.
	CPArray2(T* at_array, int i_height, int i_width, int i_stride = 0)
		: iHeight(i_height), iWidth(i_width), iStride(i_stride)
	{
		if (iStride == 0)
			iStride = iWidth;
		Assert(iWidth >= 0 && iHeight >= 0);
		Assert(Abs(iStride) >= iWidth);
		uLen = iHeight * Abs(iStride);
		atArray = at_array;
	}

	// Allocate with a specified size.
	CPArray2(int i_height, int i_width, int i_stride = 0)
		: iHeight(i_height), iWidth(i_width), iStride(i_stride)
	{
		if (iStride == 0)
			iStride = iWidth;
		Assert(iWidth >= 0 && iHeight >= 0);
		Assert(Abs(iStride) >= iWidth);
		uLen = iHeight * Abs(iStride);
		atArray = new T[uLen];
		if (iStride < 0)
			atArray += -iStride * (iHeight-1);
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// The subscript operator returns a CPArray ready for the second subscripting.
	CPArray<T> operator [](int i)
	{
		Assert(atArray && i >= 0 && i < iHeight);
		return CPArray<T>(iWidth, atArray + i * iStride);
	}

	const CPArray<T> operator [](int i) const
	{
		Assert(atArray && i >= 0 && i < iHeight);
		return CPArray<T>(iWidth, atArray + i * iStride);
	}

	// Parenthesis operator indexes from end of array.
	CPArray<T> operator ()(int i)
	{
		return (*this)[iHeight + i];
	}

	const CPArray<T> operator ()(int i) const
	{
		return (*this)[iHeight + i];
	}

	// Parenthesis operator taking 2 indexes in (x,y) order.
	T& operator ()(int i_x, int i_y)
	{
		Assert(bWithin(i_x, 0, iWidth-1));
		Assert(bWithin(i_y, 0, iHeight-1));
		return atArray[i_y * iStride + i_x];
	}

	const T& operator ()(int i_x, int i_y) const
	{
		Assert(bWithin(i_x, 0, iWidth-1));
		Assert(bWithin(i_y, 0, iHeight-1));
		return atArray[i_y * iStride + i_x];
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	int iSize() const
	//
	// Returns:
	//		Total number of pixels.
	//
	//**********************************
	{
		return uLen;
	}

	//******************************************************************************************
	//
	int iIndex
	(
		int i_x, int i_y				// Indices for indexing.
										// Note that order is (x, y), the opposite of the array
										// indexing order [y][x]
	) const
	//
	// Returns:
	//		Linear index into this array.
	//
	//**********************************
	{
		Assert(bWithin(i_x, 0, iWidth-1));
		Assert(bWithin(i_y, 0, iHeight-1));
		return i_y * iStride + i_x;
	}

	//******************************************************************************************
	//
	T& tLinear
	(
		int i_index
	)
	//
	// Returns:
	//		An element, accessed linearly.
	//
	//**********************************
	{
		Assert(atArray);
		Assert(bWithin(i_index, 0, iSize()-1));
		return atArray[i_index];
	}

	//******************************************************************************************
	const T& tLinear(int i_index) const
	{
		Assert(atArray);
		Assert(bWithin(i_index, 0, iSize()-1));
		return atArray[i_index];
	}

	//******************************************************************************************
	//
	CPArray<T>& paLinear()
	//
	// Returns:
	//		Single-dimensional array, for linear access.
	//
	//**********************************
	{
		return static_cast<CPArray<T>&>(*this);
	}

	//******************************************************************************************
	const CPArray<T> paLinear() const
	{
		return static_cast<const CPArray<T>&>(NonConst(*this));
	}

	//******************************************************************************************
	//
	CPArray2<T> pa2Dup() const
	//
	// Returns:
	//		Another array, with a new copy of the data.
	//
	//**********************************
	{
		// First create dup of linear array.
		CPArray<T> pa = paDup();

		return CPArray2<T>(pa.atArray, iHeight, iWidth, iStride);
	}

	//******************************************************************************************
	//
	CPArray2<T> pa2Rect
	(
		const SRect& rect
	)
	//
	// Returns:
	//		An array referencing a rectangular portion of this array.
	//
	//**********************************
	{
		Assert(rect.iX + rect.iWidth  <= iWidth);
		Assert(rect.iY + rect.iHeight <= iHeight);

		return CPArray2<T>(&atArray[rect.iY * iStride + rect.iX], rect.iHeight, rect.iWidth, iStride);
	}

	//******************************************************************************************
	//
	void Fill
	(
		const SRect& rect,				// Rectangular region to fill.
		T t								// Value to write.
	)
	//
	// Fill a rectangular region with the given value.
	//
	//**********************************
	{
		// Find the index of the upper left element.
		int i_index = iIndex(rect.iX, rect.iY);

		for (int i_y = 0; i_y < rect.iHeight; i_y++)
		{
			// Call linear Fill() for each line.
			CPArray<T>::Fill(i_index, rect.iWidth, t);
			i_index += iStride;
		}
	}

	//******************************************************************************************
	//
	void Fill
	(
		T t
	)
	//
	// Fill the entire array with the given value.
	//
	//**********************************
	{
		if (iWidth == iStride)
		{
			// Can fill the whole thing at once.
			CPArray<T>::Fill(t);
		}
		else
		{
			// Fill as a rectangle.
			Fill(SRect(0, 0, iWidth, iHeight), t);
		}
	}
};

//**********************************************************************************************
//
template<class T> CPArray2<T> PArray2
(
	const CPArray2<T>& pa2, 
	const SRect& rect
)
{
	return CPArray2<T>(pa2, rect);
}

//**********************************************************************************************
//
template<class T> class CAArray2: public CPArray2<T>
//
// A CPArray2 that is automatically allocated and deleted by its creator.
//
// Prefix: pa2
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CAArray2()
	{
	}

	CAArray2(const CPArray2<T>& pa2)
		: CPArray2<T>(pa2)
	{
	}

	// Allocate with a specified size.
	CAArray2(int i_height, int i_width, int i_stride = 0)
		: CPArray2<T>(i_height, i_width, i_stride)
	{
	}

	~CAArray2()
	{
		delete[] atArray;
	}

	//******************************************************************************************
	//
	// Assignment operators.
	//

	CAArray2<T>& operator =(const CPArray2<T>& pa2)
	{
		if (atArray != pa2.atArray)
		{
			delete[] atArray;
			new(this) CAArray2<T>(pa2);
		}
		return *this;
	}

private:

	// Prevent copying, because we don't want to give an array owned by one object
	// to another.

	CAArray2(const CAArray2<T>&);

	CAArray2<T>& operator =(const CAArray2<T>&);
};


#endif
