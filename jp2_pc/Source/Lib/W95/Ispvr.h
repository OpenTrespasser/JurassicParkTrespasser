/*****************************************************************************
  Name : IsPVR.H
  Date : October 1997
  Platform : ANSI compatible

  Header file used to call functions from IsPVR.C
  
  Copyright : 1997 by VideoLogic Limited. All rights reserved.
******************************************************************************/

#ifndef _ISPVR_H_
#define _ISPVR_H_

/*************************
**		  Defines 		**
*************************/
/*	From sgl.h.	*/
#ifndef NEC_VENDOR_ID		/* If not already included from PVRD3D.H */
#define NEC_VENDOR_ID					0x1033  /* PCI Vendor ID */
#define MIDAS3_PCI_BRIDGE_DEVICE_ID		0x001F  /* PCI bridge ID */
#define PCX1_DEVICE_ID					0x002A  /* PCX-1 device ID */
#define PCX2_DEVICE_ID					0x0046  /* PCX-2 device ID */
#endif

/* Added ARC-1 device id */
#ifndef ARC1_DEVICE_ID
#define ARC1_DEVICE_ID					0x0061	/* ARC-1 device ID */
#endif

/* Useful macros */
#ifndef TRUE
  #define TRUE	1
#endif
#ifndef FALSE
  #define FALSE	0
#endif

/*************************
**		   Enum 		**
*************************/
enum
{
	STATUS_SGL_BEGIN=0,
	STATUS_BOARD_PRESENT,
	STATUS_SGL_PRESENT,
	STATUS_SGL_LOADED
};	

/*************************
**		 Typedefs		**
*************************/
typedef struct _PowerVRInfo {
	INT		nStatus;							/* Detection Status */
	
	DWORD	dwChipID;							/* Board chip ID */
	DWORD	dwManufacturer;						/* Manufacturer ID */
	CHAR	szOEM[64];							/* OEM string (Matrox or Generic) */

	INT		nSGLMajorVersion;					/* Taken from SGL.DLL version info */
	INT		nSGLMinorVersion;					/* Taken from SGL.DLL version info */
	INT		nSGLBugVersion;						/* Taken from SGL.DLL version info */
	INT		nSGLBuildVersion;					/* Taken from SGL.DLL version info */
	CHAR	szSGLResourceVersion[64];			/* Taken from SGL.DLL version info */

	CHAR	szSGLInternalVersion[64];			/* Taken from sgl_get_version call in SGL.DLL */
	CHAR	szSGLInternalRequiredHeader[64];	/* Taken from sgl_get_version call in SGL.DLL */
	
	CHAR	szReserved[64];						/* Reserved for future use */
} PowerVRInfo, *pPowerVRInfo;



/*************************
** Function declaration **
*************************/

#ifdef __cplusplus		/* C++ compiling ? */
extern "C" {
#endif

int	IsPVR(pPowerVRInfo ppiPowerVRInfo);

#ifdef __cplusplus
}
#endif

#endif		/* End if _ISPVR_H_ */
