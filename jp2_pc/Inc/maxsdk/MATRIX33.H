/**********************************************************************
 *<
	FILE: matrix33.h

	DESCRIPTION: Class definitions for Matrix33

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/
#ifndef _MATRIX33_H 

#define _MATRIX33_H

#include "point3.h"

class Matrix33 {
public:
	float m[4][3];

	// Constructors
	Matrix33(){}	 // NO INITIALIZATION done in this constructor!! 
				 // (can use IdentityMatrix)
	DllExport Matrix33(float (*fp)[3]); 
	
	// Assignment operators
	DllExport Matrix33& operator-=( const Matrix33& M);
	DllExport Matrix33& operator+=( const Matrix33& M); 
	DllExport Matrix33& operator*=( const Matrix33& M);  	// Matrix multiplication

  	// Conversion function
	operator float*() { return(&m[0][0]); }

	// Operations on matrix
	DllExport void IdentityMatrix(); 		// Make into the Identity Matrix
	DllExport void Zero();		// set all elements to 0
	
	// null out the rotation part;
	DllExport void NoRot();
	// null out the scale part;
	DllExport void NoScale();
	
	// Access i-th row as Point3 for read or assignment:
	Point3& operator[](int i) { return((Point3&)(*m[i])); }
	Point3& operator[](int i) const { return((Point3&)(*m[i])); }

	// Apply Incremental transformations to this matrix
	DllExport void RotateX(float angle);  
	DllExport void RotateY(float angle);
	DllExport void RotateZ(float angle);
	DllExport void Scale(const Point3& s);
		
	DllExport Matrix33 operator*(const Matrix33&) const;
	DllExport Matrix33 operator+(const Matrix33&) const;
	DllExport Matrix33 operator-(const Matrix33&) const;
		
	};


// Build new matrices for transformations
 
Matrix33 DllExport Inverse(const Matrix33& M);  // return Inverse of matrix

// These two versions of transforming a point with a matrix do the same thing,
// regardless of the order of operands (linear algebra rules notwithstanding).
Point3 DllExport operator*(const Matrix33& A, const Point3& V); // Transform Point with matrix
Point3 DllExport operator*(const Point3& V, const Matrix33& A); // Transform Point with matrix

ostream DllExport &operator<< (ostream& s, const Matrix33& A); 

#endif // define _MATRIX33_H
