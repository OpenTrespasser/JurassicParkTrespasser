//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       gbuilder.cpp
//
//  Contents:   Groff Builder Part
//
//  Classes:    
//
//  Functions:
//
//  History:    02-Feb-98   SHernd   Created
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop


#include "gbuilder.h"

PFNGBUILDPUMP   g_pfnGBuildPump = NULL;
PFNGBUILDINFO   g_pfnGBuildInfo = NULL;
LPARAM          g_lGBuildlParam = 0;


void GBuild_SetupCallbacks(PFNGBUILDPUMP pfnPump, 
                           PFNGBUILDINFO pfnInfo,
                           LPARAM lParam)
{
    g_pfnGBuildPump = pfnPump;
    g_pfnGBuildInfo = pfnInfo;
    g_lGBuildlParam = lParam;
}


#define GROFF_BUFF_COPY     (64 * 1024)


BOOL GBuild_Build(LPSTR pszDstGroffName, int icItems, GBUILD * paGBuild)
{
    char            sz[1024];
    int             i;
    CFileIO         fioFile;
    BOOL            bRet;
    DWORD           dwSize;
    DWORD           dwToRead;
    DWORD           dwRead;
    BYTE            ab[GROFF_BUFF_COPY];
    HANDLE          hfile = INVALID_HANDLE_VALUE;
    int             icErrors = 0;
    TSectionHandle  seh;
    int             iWritten;

    // Open Groff File
    if (g_pfnGBuildInfo)
    {
        wsprintf(sz, "Opening Groff %s", pszDstGroffName);
        (g_pfnGBuildInfo)(sz, 0, g_lGBuildlParam);
    }

    if (!fioFile.bOpen(pszDstGroffName, eWrite))
    {
        if (g_pfnGBuildInfo)
        {
            wsprintf(sz, "Error Opening Groff %s", pszDstGroffName);
            (g_pfnGBuildInfo)(sz, 0, g_lGBuildlParam);
        }

        goto Error;
    }

    // iterate through GBuild Info
    for (i = 0; i < icItems; i++)
    {
        if (g_pfnGBuildInfo)
        {
            wsprintf(sz, "Adding %s", paGBuild[i].pszSection);
            (g_pfnGBuildInfo)(sz, 0, g_lGBuildlParam);
        }

        // Open the source file
        hfile = CreateFile(paGBuild[i].pszSrcName,
                           GENERIC_READ,
                           FILE_SHARE_READ,
                           NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);
        if (hfile == INVALID_HANDLE_VALUE)
        {
            icErrors++;
            if (g_pfnGBuildInfo)
            {
                wsprintf(sz, "Error Opening %s", paGBuild[i].pszSrcName);
                (g_pfnGBuildInfo)(sz, 0, g_lGBuildlParam);
            }

            continue;
        }

        // Create The section for this file in the Groff File
		seh = fioFile.sehCreate(paGBuild[i].pszSection, gSPECIAL);

        dwSize = GetFileSize(hfile, NULL);
        while (dwSize > 0)
        {
            if (dwSize > GROFF_BUFF_COPY)
            {
                dwToRead = GROFF_BUFF_COPY;
            }
            else
            {
                dwToRead = dwSize;
            }

            bRet = ReadFile(hfile, ab, dwToRead, &dwRead, NULL);
            if (!bRet || dwToRead != dwRead)
            {
                icErrors++;
                if (g_pfnGBuildInfo)
                {
                    wsprintf(sz, "Error Reading File");
                    (g_pfnGBuildInfo)(sz, 0, g_lGBuildlParam);
                }

                dwSize = 0;
            }
            else
            {
                iWritten = fioFile.iWrite(seh, ab, dwToRead);
                dwSize -= dwToRead;
            }
        }

        CloseHandle(hfile);
        hfile = INVALID_HANDLE_VALUE;

        if (g_pfnGBuildPump)
        {
            (g_pfnGBuildPump)();
        }
    }

    fioFile.bWriteImage();
    fioFile.bClose();

    // Close Groff File
    if (g_pfnGBuildInfo)
    {
        wsprintf(sz, "Closing Groff %s", pszDstGroffName);
        g_pfnGBuildInfo(sz, 0, g_lGBuildlParam);
    }

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    bRet = FALSE;
    goto Cleanup;
}


