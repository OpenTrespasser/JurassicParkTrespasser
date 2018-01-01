/**********************************************************************
 *<
	FILE: point3.h

	DESCRIPTION: Class definitions for Point3

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef _POINT3_H 

#define _POINT3_H

class Point3 {
public:
	float x,y,z;

	// Constructors
	Point3(){}
	Point3(float X, float Y, float Z)  { x = X; y = Y; z = Z;  }
	Point3(double X, double Y, double Z) { x = (float)X; y = (float)Y; z = (float)Z; }
	Point3(int X, int Y, int Z) { x = (float)X; y = (float)Y; z = (float)Z; }
	Point3(const Point3& a) { x = a.x; y = a.y; z = a.z; } 
	Point3(float af[3]) { x = af[0]; y = af[1]; z = af[2]; }

	// Access operators
	float& operator[](int i) { return (&x)[i]; }     
	const float& operator[](int i) const { return (&x)[i]; }  

	// Conversion function
	operator float*() { return(&x); }

	// Unary operators
	Point3 operator-() const { return(Point3(-x,-y,-z)); } 
	Point3 operator+() const { return *this; } 

	// Assignment operators
	inline Point3& operator-=(const Point3&);
	inline Point3& operator+=(const Point3&);
	inline Point3& operator*=(float); 
	inline Point3& operator/=(float);
	inline Point3& operator*=(const Point3&);	// element-by-element multiply.

	// Test for equality
	int operator==(const Point3& p) const { return ((p.x==x)&&(p.y==y)&&(p.z==z)); }

	// Binary operators
	inline  Point3 operator-(const Point3&) const;
	inline  Point3 operator+(const Point3&) const;
	inline  Point3 operator/(const Point3&) const;
	inline  Point3 operator*(const Point3&) const;   

	DllExport Point3 operator^(const Point3&) const;	// CROSS PRODUCT
	};


float DllExport Length(const Point3&); 
float DllExport LengthSquared(const Point3&); 
int DllExport MaxComponent(const Point3&);  // the component with the maximum abs value
int DllExport MinComponent(const Point3&);  // the component with the minimum abs value
Point3 DllExport Normalize(const Point3&); // Return a unit vector.


// RB: moved this here from object.h
class Ray {
	public:
		Point3 p;   // point of origin
		Point3 dir; // unit vector
	};

 
// Inlines:

inline float Length(const Point3& v) {	
	return (float)sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
	}

inline float LengthSquared(const Point3& v) {	
	return (v.x*v.x+v.y*v.y+v.z*v.z);
	}

inline Point3& Point3::operator-=(const Point3& a) {	
	x -= a.x;	y -= a.y;	z -= a.z;
	return *this;
	}

inline Point3& Point3::operator+=(const Point3& a) {
	x += a.x;	y += a.y;	z += a.z;
	return *this;
	}

inline Point3& Point3::operator*=(float f) {
	x *= f;   y *= f;	z *= f;
	return *this;
	}

inline Point3& Point3::operator/=(float f) { 
	x /= f;	y /= f;	z /= f;	
	return *this; 
	}

inline Point3& Point3::operator*=(const Point3& a) { 
	x *= a.x;	y *= a.y;	z *= a.z;	
	return *this; 
	}

inline Point3 Point3::operator-(const Point3& b) const {
	return(Point3(x-b.x,y-b.y,z-b.z));
	}

inline Point3 Point3::operator+(const Point3& b) const {
	return(Point3(x+b.x,y+b.y,z+b.z));
	}

inline Point3 Point3::operator/(const Point3& b) const {
	return Point3(x/b.x,y/b.y,z/b.z);
	}

inline Point3 Point3::operator*(const Point3& b) const {  
	return Point3(x*b.x, y*b.y, z*b.z);	
	}


inline Point3 operator*(float f, const Point3& a) {
	return(Point3(a.x*f, a.y*f, a.z*f));
	}

inline Point3 operator*(const Point3& a, float f) {
	return(Point3(a.x*f, a.y*f, a.z*f));
	}

inline Point3 operator/(const Point3& a, float f) {
	return(Point3(a.x/f, a.y/f, a.z/f));
	}

inline Point3 operator+(const Point3& a, float f) {
	return(Point3(a.x+f, a.y+f, a.z+f));
	}

inline float DotProd(const Point3& a, const Point3& b) { 
	return(a.x*b.x+a.y*b.y+a.z*b.z);	
	}

Point3 DllExport CrossProd(const Point3& a, const Point3& b);	// CROSS PRODUCT


// Compress a normal vector from 12 to 4 bytes.
// the vector has to be <= 1.0 in length.
// The decompressed vector has absolute error <.001 in each
// component.
ULONG DllExport CompressNormal(Point3 p);
Point3 DllExport DeCompressNormal(ULONG n);

#endif

