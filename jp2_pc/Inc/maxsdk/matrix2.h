/**********************************************************************
 *<
	FILE: matrix2.h

	DESCRIPTION: Class definitions for Matrix2

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __MATRIX2__ 

#define __MATRIX2_

#include "point2.h"

class Matrix2 {
public:
	float m[3][2];

	// Constructors
	Matrix2(){}	 // NO INITIALIZATION done in this constructor!! (can use Zero or IdentityMatrix)
	DllExport Matrix2(float (*fp)[2]); 
	
	// Assignment operators
	DllExport Matrix2& operator-=( const Matrix2& M);
	DllExport Matrix2& operator+=( const Matrix2& M); 
	DllExport Matrix2& operator*=( const Matrix2& M);  	// Matrix multiplication

 	// Conversion function
	operator float*() { return(&m[0][0]); }

	// Initialize matrix
	DllExport void IdentityMatrix(); 	// Set to the Identity Matrix
	DllExport void Zero();		// Set all elements to 0

	// Apply Incremental transformations to this matrix
	DllExport void Translate(const Point2& p);
	DllExport void Rotate(float angle);  
	DllExport void Scale(const Point2& s);
		
	// Binary operators		
	DllExport Matrix2 Matrix2::operator*(const Matrix2& B) const;	
	DllExport Matrix2 Matrix2::operator+(const Matrix2& B) const;
	DllExport Matrix2 Matrix2::operator-(const Matrix2& B) const;
	};

// Build new matrices for transformations
Matrix2 DllExport RotateMatrix(float angle);   
Matrix2 DllExport TransMatrix(const Point2& p);
Matrix2 DllExport ScaleMatrix(const Point2& s);
 
Matrix2 DllExport Inverse(const Matrix2& M);

// Transform point with matrix:
Point2 DllExport operator*(const Matrix2& A, const Point2& V);
Point2 DllExport operator*( const Point2& V, const Matrix2& A);
Point2 DllExport VectorTransform(const Matrix2& M, const Point2& V);

// Printout
ostream DllExport &operator<< (ostream& s, const Matrix2& A); 

#endif
