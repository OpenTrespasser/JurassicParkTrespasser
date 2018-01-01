/**********************************************************************
 *<
	FILE: quat.h

	DESCRIPTION: Class definitions for Quat

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/
#ifndef _QUAT_H 

#define _QUAT_H 

#include "matrix3.h"

class ostream;
class Quat;

class AngAxis {
public:
	Point3 axis;
	float angle;

	AngAxis() {}
	AngAxis(const Point3& axis,float angle) {this->axis=axis;this->angle=angle;}	
	DllExport AngAxis(const Quat &q);
	DllExport int GetNumRevs();
	DllExport void SetNumRevs(int num);
	};

class Quat {
public:
	float x,y,z,w;

	// Constructors
	Quat(){}
	Quat(float X, float Y, float Z, float W)  { x = X; y = Y; z = Z; w = W; }
	Quat(double X, double Y, double Z, double W)  { 
		x = (float)X; y = (float)Y; z = (float)Z; w = (float)W; 
		}
	Quat(const Quat& a) { x = a.x; y = a.y; z = a.z; w = a.w; } 
	Quat(float af[4]) { x = af[0]; y = af[1]; z = af[2]; w = af[3]; }
	DllExport Quat(const Matrix3& mat);
	DllExport Quat(const AngAxis& aa);

	// Access operators
	float& operator[](int i) { return (&x)[i]; }     
	const float& operator[](int i) const { return (&x)[i]; }  

	// Conversion function
	operator float*() { return(&x); }

	// Unary operators
	Quat operator-() const { return(Quat(-x,-y,-z,-w)); } 
	Quat operator+() const { return *this; } 

	// Assignment operators
	DllExport Quat& operator-=(const Quat&);
	DllExport Quat& operator+=(const Quat&);
	DllExport Quat& operator*=(const Quat&);
	DllExport Quat& operator*=(float);
	DllExport Quat& operator/=(float);

	DllExport Quat& MakeClosest(const Quat& qto);

	// Comparison
	DllExport int operator==(const Quat& a) const;

	void Identity() { x = y = z = (float)0.0; w = (float) 1.0; }
	DllExport int IsIdentity() const;
	DllExport void Normalize();  // normalize
	DllExport void MakeMatrix(Matrix3 &mat) const;

	// Binary operators
	DllExport Quat operator-(const Quat&) const;  //RB: Changed these to		// difference of two quaternions
	DllExport Quat operator+(const Quat&) const;  // duplicate * and /			// sum of two quaternions
	DllExport Quat operator*(const Quat&) const;  // product of two quaternions
	DllExport Quat operator/(const Quat&) const;  // ratio of two quaternions
	};

Quat DllExport operator*(float, const Quat&);	// multiply by scalar
Quat DllExport operator*(const Quat&, float);	// multiply by scalar
Quat DllExport operator/(const Quat&, float);	// divide by scalar
Quat DllExport Inverse(const Quat& q);  // Inverse of quaternion (1/q)
Quat DllExport Conjugate(const Quat& q); 
Quat DllExport LogN(const Quat& q);
Quat DllExport Exp(const Quat& q);
Quat DllExport Slerp(const Quat& p, const Quat& q, float t);
Quat DllExport LnDif(const Quat& p, const Quat& q);
Quat DllExport QCompA(const Quat& qprev,const Quat& q, const Quat& qnext);
Quat DllExport Squad(const Quat& p, const Quat& a, const Quat &b, const Quat& q, float t); 
Quat DllExport qorthog(const Quat& p, const Point3& axis);
Quat DllExport squadrev(
		float angle,	// angle of rotation 
		const Point3& axis,	// the axis of rotation 
		const Quat& p,		// start quaternion 
		const Quat& a, 		// start tangent quaternion 
		const Quat& b, 		// end tangent quaternion 
		const Quat& q,		// end quaternion 
		float t 		// parameter, in range [0.0,1.0] 
		);

void DllExport RotateMatrix(Matrix3& mat, const Quat& q);	  
void DllExport PreRotateMatrix(Matrix3& mat, const Quat& q);
Quat DllExport QFromAngAxis(float ang, const Point3& axis);
void DllExport AngAxisFromQ(const Quat& q, float *ang, Point3& axis);
float DllExport QangAxis(const Quat& p, const Quat& q, Point3& axis);
void DllExport DecomposeMatrix(const Matrix3& mat, Point3& p, Quat& q, Point3& s);
Quat DllExport TransformQuat(const Matrix3 &m, const Quat&q );
inline Quat IdentQuat() { return(Quat(0.0,0.0,0.0,1.0)); }

// Assumes Euler angles are of the form:
// RotateX(ang[0])
// RotateY(ang[1])
// RotateZ(ang[2])
//
void DllExport QuatToEuler(Quat &q, float *ang);
void DllExport EulerToQuat(float *ang, Quat &q);

ostream DllExport &operator<<(ostream&, const Quat&); 

#endif _QUAT_H 
