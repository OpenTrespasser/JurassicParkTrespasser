/**********************************************************************
 *<
	FILE: mtl.h

	DESCRIPTION: Material and texture class definitions

	CREATED BY: Don Brittain

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#if !defined(_MTL_H_)

#define _MTL_H_

// main material class definition
class  Material {
public:
	DllExport Material();
	DllExport ~Material();
	
    Point3		Ka;
    Point3		Kd;
    Point3		Ks;
    float		shininess;
    float		shinStrength;
    float		opacity;
	float		selfIllum;
	int			dblSided;
	int			shadeLimit;
	int			useTex;
	int			faceMap;
	DWORD 		textHandle;  // texture handle
	Matrix3 	textTM;  // texture transform
};

#endif // _MTL_H_
