/**********************************************************************
 *<
	FILE: acolor.h

	DESCRIPTION:  floating point color + alpha

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef _ACOLOR_H 

#define _ACOLOR_H

#include "point3.h"
#include "color.h"

class AColor {
public:
	float r,g,b,a;

	// Constructors
	AColor()  {}
	AColor(float R, float G, float B, float A=1.0f)  { r = R; g = G; b = B; a = A; }
	AColor(double R, double G, double B, double A=1.0) {
		 r = (float)R; g = (float)G; b = (float)B; a = (float)A; }
	AColor(int R, int G, int B, int A=0) { 
		r = (float)R; g = (float)G; b = (float)B; a = (float)A; }
	AColor(const AColor& c) { r = c.r; g = c.g; b = c.b; a = c.a; } 
	AColor(const Color& c, float alph=1.0f) { r = c.r; g = c.g; b = c.b; a = alph; } 
	AColor(DWORD rgb, float alph=1.0f);  // from Windows RGB value
	AColor(float af[4]) { r = af[0]; g = af[1]; b = af[2];a = af[3]; }
	
	void Black() { r = g = b = 0.0f; a = 1.0f; }
	void White() { r = g = b = 1.0f; a = 1.0f; }

	// Access operators
	float& operator[](int i) { return (&r)[i]; }     
	const float& operator[](int i) const { return (&r)[i]; }  

	// Conversion functions
	operator float*() { return(&r); }
	operator Color() { return Color(r,g,b); }

	// Convert to Windows RGB
	operator DWORD() { return RGB(FLto255(r),FLto255(g), FLto255(b)); }

	// Convert to Point3
	operator Point3() { return Point3(r,g,b); }

	// Unary operators
	AColor operator-() const { return (AColor(-r,-g,-b, -a)); } 
	AColor operator+() const { return *this; } 

	// Assignment operators
	inline AColor& operator-=(const AColor&);
    inline AColor& operator+=(const AColor&);
	inline AColor& operator*=(float); 
	inline AColor& operator/=(float);
	inline AColor& operator*=(const AColor&);	// element-by-element multiplg.

	// Test for equality
	int operator==(const AColor& p) const { return ((p.r==r)&&(p.g==g)&&(p.b==b)&&(p.a==a)); }
	int operator!=(const AColor& p) const { return ((p.r!=r)||(p.g!=g)||(p.b!=b)||(p.a!=a)); }

	// Binary operators
	inline AColor operator-(const AColor&) const;
	inline AColor operator+(const AColor&) const;
	inline AColor operator/(const AColor&) const;
    inline AColor operator*(const AColor&) const;   
	inline AColor operator^(const AColor&) const;   // CROSS PRODUCT
	};

int DllExport MaxComponent(const AColor&);  // the component with the maximum abs value
int DllExport MinComponent(const AColor&);  // the component with the minimum abs value

// Inlines:

inline AColor& AColor::operator-=(const AColor& c) {	
	r -= c.r;	g -= c.g;	b -= c.b;  a -= c.a;
	return *this;
	}

inline AColor& AColor::operator+=(const AColor& c) {
	r += c.r;	g += c.g;	b += c.b; a += c.a;
	return *this;
	}

inline AColor& AColor::operator*=(float f) {
	r *= f;   g *= f;	b *= f;  a *= f;
	return *this;
	}

inline AColor& AColor::operator/=(float f) { 
	r /= f;	g /= f;	b /= f;	a /= f;
	return *this; 
	}

inline AColor& AColor::operator*=(const AColor& c) { 
	r *= c.r;	g *= c.g;	b *= c.b;	a *= c.a;
	return *this; 
	}


inline AColor AColor::operator-(const AColor& c) const {
	return(AColor(r-c.r,g-c.g,b-c.b,a-c.a));
	}

inline AColor AColor::operator+(const AColor& c) const {
	return(AColor(r+c.r,g+c.g,b+c.b,a+c.a));
	}

inline AColor AColor::operator/(const AColor& c) const {
	return AColor(r/c.r,g/c.g,b/c.b,a/c.a);
	}

inline AColor AColor::operator*(const AColor& c) const {  
	return AColor(r*c.r, g*c.g, b*c.b, a*c.a);	
	}

inline AColor operator*(float f, const AColor& a) {
	return(AColor(a.r*f, a.g*f, a.b*f, a.a*f));
	}

inline AColor operator*(const AColor& a, float f) {
	return(AColor(a.r*f, a.g*f, a.b*f, a.a*f));
	}

// Composite  fg over bg, assuming associated alpha,
// i.e. pre-multiplied alpha for both fg and bg
inline AColor CompOver(const AColor &fg, const AColor& bg) {
	return  fg + (1.0f-fg.a)*bg;
	}

typedef AColor RGBA;

#endif

