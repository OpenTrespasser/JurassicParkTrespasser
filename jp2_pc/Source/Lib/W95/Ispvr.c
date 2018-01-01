/*****************************************************************************
  Name : ISPVR.C v1.02
  Date : January 1998
  Platform : ANSI compatible

  * DESCRIPTION:
  Set of functions used to detect if a PowerVR board and its drivers are present
	
  * BUILD INFO:
  The sgl_versions and *psgl_versions structure declaration have been taken out
  of SGL.H, so that the developper does not need to include this header in his/her
  project. However, you can still include SGL.H if you need it : the structure
  declaration will not be duplicated.
  Library : version.lib

  * USEFUL COMMENTS:
  The main function is :

  int	IsPVR(pPowerVRInfo ppiPowerVRInfo);

  The function will return TRUE if the SGL drivers are installed on the machine.
  It will update the pPowerVRD3DInfo structure IF IT HAS BEEN SUBMITTED.
  That is, you can call the function with a NULL parameter.


  Email any comments to nthibieroz@videologic.com

  Copyright : 1998 by VideoLogic Limited. All rights reserved.
******************************************************************************/


/*************** Includes *************/
#include <windows.h>
#include <stdio.h>		/* For sprintf */
#include <winver.h>		/* For version information functions */
#include "Ispvr.h"		/* Internal header */


/*************** Defines *************/
/*************************************************************
** IMPORTANT : if a machine is a NEC PC-9800, define _NEC98 **
** This is because the NEC PC98 uses a different interrupt  **
** for the PCI BIOS and different op-codes.					**
*************************************************************/

/* define _NEC98 */		/* Commented out for normal PC */

#define LOADSGL (0)

#ifdef _NEC98	/* NEC PC98 PC */
#ifndef FIND_PCI_DEVICE_16_AX
#define FIND_PCI_DEVICE_16_AX	0xCC02
#endif
#ifndef PCI_INTERRUPT
#define PCI_INTERRUPT			0x1F
#endif

#else		/* Normal PC */

#ifndef FIND_PCI_DEVICE_16_AX
#define	FIND_PCI_DEVICE_16_AX	0xB102
#endif
#ifndef PCI_INTERRUPT
#define PCI_INTERRUPT			0x1A
#endif
#endif


#define MAX_MEM_IO_WINDOWS  6
/* From vsgl.h */
#define VSGL_GET_PHYSICAL_BOARD_INFO	27
#define VSGL_GPPORT						0x0058

#define MATROX_M3D_GPPORT_VALUE			0xF6F6


/*************** Typedefs *************/
/* From sgl.h */
#ifndef __SGL_H__		/* If SGL.H has not already been included */
typedef struct {
	char *library;
	char *required_header;
} sgl_versions, *psgl_versions;
#endif

/* Structures needed to retrieve board physical information from vsgl.vxd */
typedef enum
{
	PVR_STATUS_PENDING	= 0x00000000,
	PVR_STATUS_EOR		= 0x00000001
} PVR_STATUS;

typedef enum
{
	DISABLE_BOARD=0,
	MIDAS3,
	MIDAS4,
	MIDAS5,
	MIDAS5_003
} DEVICE_TYPE;

typedef struct BoardDataBlock
{
	DEVICE_TYPE	DeviceType;
	unsigned long 	PCIVendorID;
	unsigned long 	PCIDeviceID;
	unsigned long 	PCIBus;
	unsigned long 	PCIDev;
	unsigned long 	PCIFunc;
	unsigned long 	RevisionID;

	unsigned long 	IRQ;
	unsigned long 	nMemWindows;
	unsigned long 	PhysMemWindows   [MAX_MEM_IO_WINDOWS][2];
	unsigned long 	LinearMemWindows [MAX_MEM_IO_WINDOWS];
	unsigned long 	nIOWindows;
	unsigned long 	IOWindows        [MAX_MEM_IO_WINDOWS][2];

	PVR_STATUS volatile	*pStatus;
	
} BoardDataBlock;


/*************** Global variables **************/
static FARPROC		pfnsgl_getversions;		/* Pointer to sgl_getversions() function */


/*************** Prototypes ***********/
static int PCIScan(DWORD nVendor, DWORD nDevice);
static int IsM3D();
static int GetSGLDLLVersion(pPowerVRInfo ppiPowerVRInfo);
static int LoadSGLDLL(pPowerVRInfo ppiPowerVRInfo);
int	IsPVR(pPowerVRInfo ppiPowerVRInfo);


/***************************************
 *************** Functions *************
 **************************************/


/*******************************************************************************
 * Function Name  : PCIScan
 * Inputs         : nVendor, nDevice
 * Outputs        : None
 * Returns        : TRUE or FALSE
 * Global Used    : None
 * Description    : Perform a PCI scan to test if a PowerVR board is physically
 *					present.
 *					We assume there is only one PowerVR board in the machine 
 *					(nIndex = 0 -> detects first PCI board corresponding to the 
 *					device ID and manufacturer ID submitted.
 *					Be sure to modify the FIND_PCI_DEVICE_16_AX and 
 *					PCI_INTERRUPT defines from the top of the file if the
 *					computer is one of the PC98 series.
 *					
 *******************************************************************************/
static int PCIScan(DWORD nVendor, DWORD nDevice)
{
	int		nIndex=0;	
	int		nBoardDetection=FALSE;
	
	__asm
		{
			mov		ecx, nDevice;
			mov		edx, nVendor;
			mov		esi, nIndex;
			mov		eax, FIND_PCI_DEVICE_16_AX;
			int		PCI_INTERRUPT;
			jc		Board_Continue;		/* Carry flag set = error in detection (not detected) */
		
			mov		nBoardDetection, TRUE;	/* Carry flag clear = PCI board detected */
			Board_Continue:	
		}
	
	return(nBoardDetection);
}


/*******************************************************************************
 * Function Name  : IsM3D
 * Returns        : TRUE or FALSE
 * Global Used    : None
 * Description    : Detect if the PowerVR board is a Matrox board (m3D) or a
 *					generic board.
 *					To perform the detection, the VSGL.VXD driver is asked with
 *					a code of operation (VSGL_GET_PHYSICAL_BOARD_INFO) and
 *					relevant information is returned.
 *					Returns TRUE if a Matrox m3D has been detected.
 *					
 *******************************************************************************/
static int IsM3D()
{
	DWORD			*pRegs=NULL;
	DWORD			*pGPPORT;
	HANDLE			hVxD;
	DWORD			PhysBoardID=0;		
	BoardDataBlock	BDB;
	char			pszTmp[300];
	int				nM3DDetected=FALSE;

	/* Load hardware information from the VSGL.VXD device driver */
	hVxD=CreateFile("\\\\.\\VSGL.VXD", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL);
	if (hVxD==INVALID_HANDLE_VALUE)
	{
		/* vsgl.vxd was not found or not loaded correctly */
		OutputDebugString("Failed to open VxD\n");
		hVxD=NULL;
		return FALSE;		
	}

	/* Sends a control code to the VSGL.VXD device driver */
	if (DeviceIoControl(hVxD, VSGL_GET_PHYSICAL_BOARD_INFO,
					    &PhysBoardID, sizeof(PhysBoardID), 
						&BDB, sizeof(BDB), NULL, NULL))
	{
		/* pRegs is the linear address of PCX2 registers */
		pRegs=(DWORD *)BDB.LinearMemWindows[0];

		/* Retrieve GPPORT address */
		pGPPORT=(DWORD *)((char *)pRegs+VSGL_GPPORT);

		// Set for input
		*pGPPORT=0x00000000;

		/* Needed if optimization is ON */
		Sleep(10);	

		/* Read GPPORT value
		   For the M3D, Matrox has connected resistors to this port, thus
		   obtaining a value (MATROX_M3D_GPPORT_VALUE) when reading that port */
		if ( (*pGPPORT & 0x0000FFFF)==MATROX_M3D_GPPORT_VALUE )
		{
			/* Matrox M3D detected */
			nM3DDetected=TRUE;
			OutputDebugString("Matrox M3D detected\n");
		}
		
		/* Display info in debug screen */
		sprintf(pszTmp, "GP PORT address=0x%X\nGP PORT value=0x%X\n", pGPPORT, *pGPPORT);
		OutputDebugString(pszTmp);
	}
	else	/* If DeviceIoControl has failed */
	{
		sprintf(pszTmp, "Error no %d\n", GetLastError());
		OutputDebugString(pszTmp);
		OutputDebugString("Unable to get physical board info\n");
	}
	
	/* Close device driver handle */
	CloseHandle(hVxD);
	hVxD=NULL;

	return (nM3DDetected);
}


/*******************************************************************************
 * Function Name  : GetSGDLLVersion
 * Inputs/Outputs : ppiPowerVRInfo
 * Returns        : TRUE or FALSE
 * Global Used    : None
 * Description    : Test if SGL.DLL exists
 *					Return TRUE if it does, FALSE otherwise
 *					If ppiPowerVRInfo exists(non NULL), then version information
 *					is returned.
 *					
 *******************************************************************************/
static int GetSGLDLLVersion(pPowerVRInfo ppiPowerVRInfo)
{
	LPVOID	lpvVersionInfo;
	DWORD	dwVerInfoSize;
	DWORD	dwHandle;					/* Version Info Handle */
	LPVOID	lpFixedVersionInfo;
	LPVOID	lpFixedVersionInfoLength;
	VS_FIXEDFILEINFO *pFixedInfo;
	HGLOBAL hMem;
	LPSTR   lpstrVffInfo;
	LPSTR   lpVersion;					/* String pointer to 'version' text */
	UINT	*puVersionLen=(UINT *)calloc(1, sizeof(UINT));
	CHAR	pszFileName[70];
	
	/* Copy "SGL.DLL" in pszFileName string */
	strcpy(pszFileName, "SGL.DLL");
	
	/* Take structure size */
	dwVerInfoSize = GetFileVersionInfoSize(pszFileName, &dwHandle);
    
	/* If dwVerInfoSize exists, continue, else return FALSE */
	if (!dwVerInfoSize)
    {
		OutputDebugString("GetFileVersionInfoSize() failed in GetSGLDLLVersion()\n");
		return FALSE;
	}

    /* If PowerVRInfo structure is supplied */
	if (ppiPowerVRInfo)	
	{
		/* First extract version numbers */

		/* Allocate memory to receive Version Information structure */
		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		if (hMem==NULL)
		{
			OutputDebugString("Memory could not be allocated\n");
			return FALSE;
		}
			
		/* Lock memory */
		lpvVersionInfo=GlobalLock(hMem);
		if (lpvVersionInfo==NULL)
		{
			OutputDebugString("GlobalLock() failed in GetSGLDLLVersion()\n");
			GlobalFree(hMem);
			return FALSE;
		}

		/* Get file version info */
		if (!GetFileVersionInfo(pszFileName, dwHandle, dwVerInfoSize, lpvVersionInfo))
		{
			OutputDebugString("GetFileVersionInfo() failed in GetSGLDLLVersion()\n");
			GlobalUnlock(hMem);
			GlobalFree(hMem);
			return FALSE;
		}
			
		/* Load fixed File Information */
		if (!VerQueryValue (lpvVersionInfo,
							TEXT("\\"),
							&lpFixedVersionInfo,
							(PUINT) &lpFixedVersionInfoLength))
		{
			OutputDebugString("VerQueryValue() failed in GetSGLDLLVersion()\n");
			GlobalUnlock(hMem);
			GlobalFree(hMem);
			return FALSE;
		}
			
		/* Retrieve information */
		pFixedInfo = (VS_FIXEDFILEINFO *)lpFixedVersionInfo;

		/* Write version numbers in PowerVRInfo SGL structure */
		ppiPowerVRInfo->nSGLMajorVersion=HIWORD(pFixedInfo->dwFileVersionMS);
		ppiPowerVRInfo->nSGLMinorVersion=LOWORD(pFixedInfo->dwFileVersionMS);
		ppiPowerVRInfo->nSGLBugVersion=  HIWORD(pFixedInfo->dwFileVersionLS);
		ppiPowerVRInfo->nSGLBuildVersion=LOWORD(pFixedInfo->dwFileVersionLS);
										
		/* Unlock memory */
		GlobalUnlock(hMem);
		/* Free memory */
		GlobalFree (hMem);		
	
		/* Then extract version string */
		
		/* Allocate memory to receive a string */
		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		if (hMem==NULL)
		{
			OutputDebugString("Memory could not be allocated\n");
			return FALSE;
		}

		/* Lock memory */
		lpstrVffInfo = (char *)GlobalLock(hMem);
		if (lpstrVffInfo==NULL)
		{
			OutputDebugString("GlobalLock() failed in GetSGLDLLVersion()\n");
			GlobalFree(hMem);
			return FALSE;
		}

		/* Get file version info */
		if (!GetFileVersionInfo(pszFileName, dwHandle, dwVerInfoSize, lpstrVffInfo))
		{
			OutputDebugString("GetFileVersionInfo() failed in GetSGLDLLVersion()\n");
			GlobalUnlock(hMem);
			GlobalFree(hMem);
			return FALSE;
		}

		/* File Version */
		if (!VerQueryValue((LPVOID)lpstrVffInfo,
						TEXT("\\StringFileInfo\\040904E4\\FileVersion"),
						(LPVOID *)&lpVersion,puVersionLen))
		{
			OutputDebugString("VerQueryValue() failed in GetSGLDLLVersion()\n");
			GlobalUnlock(hMem);
			GlobalFree(hMem);
			return FALSE;
		}
			
		/* Copy version string into PowerVRInfo SGL structure */
		strcpy(ppiPowerVRInfo->szSGLResourceVersion, lpVersion);
							
		/* Unlock memory */
		GlobalUnlock (hMem);
		/* Free memory */
		GlobalFree (hMem);		
	}	/* End if ppiPowerVRInfo */
	
	return(TRUE);
}



/*******************************************************************************
 * Function Name  : LoadSGLDLL
 * Input/Output	  : ppiPowerVRInfo	(can be NULL)
 * Returns        : TRUE or FALSE
 * Global Used    : None
 * Description    : Test if SGL.DLL can be loaded
 *					Return TRUE if it can, FALSE otherwise
 *					If ppiPowerVRInfo exists(non NULL), then internal version
 *					information is returned by calling sgl_get_versions() from
 *					SGL.DLL.
 *					
 *******************************************************************************/
static int LoadSGLDLL(pPowerVRInfo ppiPowerVRInfo)
{
	HINSTANCE hInstanceSGL;
	sgl_versions *pSGL_Info;

	/*
		Try to load SGL.DLL
		Assign hInstanceSGL to SGL.DLL
		Will look for sgl.dll in
		1. The directory from which the application loaded. 
		2. The current directory. 
		3. The Windows system directory.
		4. The Windows directory.
		5. The directories that are listed in the PATH environment variable.  
	*/
	
	/* Load the SGL.DLL library */
	hInstanceSGL=LoadLibrary("sgl.dll");		
	
	/* Test value of hInstance ; if NULL then sgl.dll could not be loaded->return FALSE */
	if (hInstanceSGL!=NULL)
	{
		/* If we get this point, then SGL.DLL was loaded properly */

		/* if Version_Info is not NULL, load SGL version informations */
		if (ppiPowerVRInfo!=NULL) 
		{
			/* Load address of sgl_get_versions from SGL.DLL */
			pfnsgl_getversions=GetProcAddress(hInstanceSGL, "sgl_get_versions");
			
			/* Call pfnsgl_get_versions from SGL.DLL */
			pSGL_Info=(sgl_versions *)pfnsgl_getversions();
		
			/* Copy string contents into returned structure	*/
			strcpy(ppiPowerVRInfo->szSGLInternalVersion, pSGL_Info->library);
			strcpy(ppiPowerVRInfo->szSGLInternalRequiredHeader, pSGL_Info->required_header);
		}

		/* Free library handle */
		FreeLibrary(hInstanceSGL);

		/* Return TRUE : SGL.DLL has been loaded and version informations extracted */
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}


/*******************************************************************************
 * Function Name  : IsPVR
 * Input/Output	  : ppiPowerVRInfo (can be NULL)
 * Returns        : TRUE or FALSE
 * Global Used    : None
 * Description    : Perform an incremental detection process of a PowerVR board.
 *					The steps are :
 *					1. Test if a PowerVR board is physically present with a PCI scan.
 *					2. Test if SGL.DLL exist and return version information.
 *					3. Load SGL.DLL and call sgl_get_version() to obtain internal
 *					   version information.
 *					nStatus in the pPowerVRInfo structure indicates how far the
 *					detection process went.
 *					
 *******************************************************************************/
int IsPVR(pPowerVRInfo ppiPowerVRInfo)
{
	int Returned_Value=FALSE;

	/***************************************************
	** Detect the (most recent) board with a PCI scan **
	***************************************************/
	if (PCIScan(NEC_VENDOR_ID, ARC1_DEVICE_ID))
	{
		if (ppiPowerVRInfo)
		{
			ppiPowerVRInfo->nStatus=STATUS_BOARD_PRESENT;
			ppiPowerVRInfo->dwManufacturer=NEC_VENDOR_ID;
			ppiPowerVRInfo->dwChipID=ARC1_DEVICE_ID;
		}
	}
	else
	if (PCIScan(NEC_VENDOR_ID, PCX2_DEVICE_ID))
	{
		if (ppiPowerVRInfo)
		{
			ppiPowerVRInfo->nStatus=STATUS_BOARD_PRESENT;
			ppiPowerVRInfo->dwManufacturer=NEC_VENDOR_ID;
			ppiPowerVRInfo->dwChipID=PCX2_DEVICE_ID;
		}
	}
	else
	if (PCIScan(NEC_VENDOR_ID, PCX1_DEVICE_ID))
	{
		if (ppiPowerVRInfo)
		{
			ppiPowerVRInfo->nStatus=STATUS_BOARD_PRESENT;
			ppiPowerVRInfo->dwManufacturer=NEC_VENDOR_ID;
			ppiPowerVRInfo->dwChipID=PCX1_DEVICE_ID;
		}
	}
	else
	if (PCIScan(NEC_VENDOR_ID, MIDAS3_PCI_BRIDGE_DEVICE_ID))
	{
		if (ppiPowerVRInfo)
		{
			ppiPowerVRInfo->nStatus=STATUS_BOARD_PRESENT;
			ppiPowerVRInfo->dwManufacturer=NEC_VENDOR_ID;
			ppiPowerVRInfo->dwChipID=MIDAS3_PCI_BRIDGE_DEVICE_ID;
		}
	}
	else
	{
		return(FALSE);
	}

#if LOADSGL

	/*************************************************************************
	** If a PCX2 is detected, then perform OEM detection (Matrox or Generic) *
	*************************************************************************/	
	/* Since we already know that a PowerVR board exists in the system, we
	   only run this test if the user has supplied a pPowerVRInfo structure */
	if (ppiPowerVRInfo )
	{
		if (ppiPowerVRInfo->dwChipID==PCX2_DEVICE_ID && IsM3D())
		{
			strcpy(ppiPowerVRInfo->szOEM, "Matrox");
		}
		else
		{
			strcpy(ppiPowerVRInfo->szOEM, "Generic");
		}
	}

	/************************************************************************
	** Detect the file existance and get the version resource from SGL.DLL **
	*************************************************************************/	
	if (!GetSGLDLLVersion(ppiPowerVRInfo))
	{
		return (FALSE);
	}
	
	/* If user supplied a pPowerVRInfo structure */
	if (ppiPowerVRInfo) 
	{
		ppiPowerVRInfo->nStatus=STATUS_SGL_PRESENT;
	}

	/*******************************************************************
	** Attempt to load SGL.DLL and to call sgl_get_versions() from it **
	*******************************************************************/	
	if (!LoadSGLDLL(ppiPowerVRInfo))
	{
		return(FALSE);
	}
	
	/* If user supplied a pPowerVRInfo structure */
	if (ppiPowerVRInfo)
	{
		ppiPowerVRInfo->nStatus=STATUS_SGL_LOADED;
	}

#endif LOADSGL

	return(TRUE);	/* All tests have been successfully passed */
}

