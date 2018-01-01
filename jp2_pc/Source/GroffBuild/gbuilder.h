//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       gbuilder.h
//
//  Contents:   Groff Builder Header
//
//  Classes:    
//
//  Functions:
//
//  History:    02-Feb-98   SHernd   Created
//
//---------------------------------------------------------------------------


#ifndef __GBUILDER_H__
#define __GBUILDER_H__


typedef BOOL (FAR PASCAL * PFNGBUILDPUMP)();
typedef BOOL (FAR PASCAL * PFNGBUILDINFO)(LPSTR pszInformation, int iInfoStyle, LPARAM lParam);


typedef struct tagGBUILD
{
    LPSTR   pszSection;
    LPSTR   pszSrcName;
} GBUILD;


void GBuild_SetupCallbacks(PFNGBUILDPUMP pfnPump, PFNGBUILDINFO fpnInfo, LPARAM lParam);
BOOL GBuild_Build(LPSTR pszDstGroffName, int icItems, GBUILD * paGBuild);


#endif // __GBUILDER_H__

