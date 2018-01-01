/*---------------------------------------------------------------------
 *
 * ia3d.h
 *
 *---------------------------------------------------------------------
 *
 * $Id: ia3d.h%v 1.1 1996/09/02 10:50:35 mike Exp mike $
 *
 *---------------------------------------------------------------------
 *
 * ia3d header file.  It's the part the outside world needs to see.
 *
 *---------------------------------------------------------------------
 *
 * AUREAL SEMICONDUCTOR, INC. PROPRIETARY AND CONFIDENTIAL
 * Copyright (c) 1996 Aureal Semiconductor, Inc. - All rights 
 * reserved. 
 *
 *---------------------------------------------------------------------
 */


#ifndef _IA3D_H_
#define _IA3D_H_

#ifdef __cplusplus
extern "C" {
#endif


// A3d Class ID! {D8F1EEE0-F634-11cf-8700-00A0245D918B}
DEFINE_GUID(CLSID_A3d, 
0xd8f1eee0, 0xf634, 0x11cf, 0x87, 0x0, 0x0, 0xa0, 0x24, 0x5d, 0x91, 0x8b);

// A3d Interface ID! {D8F1EEE1-F634-11cf-8700-00A0245D918B}
DEFINE_GUID(IID_IA3d,
0xd8f1eee1, 0xf634, 0x11cf, 0x87, 0x0, 0x0, 0xa0, 0x24, 0x5d, 0x91, 0x8b);


// Bits for manipulating output modes

// Values for bOutputMode
#define OUTPUT_MODE_STEREO	0x00000001
#define OUTPUT_MODE_QUAD	0x00000002

// Values for FrontXtalkMode and bRearXtalkMode
#define OUTPUT_HEADPHONES		0x00000001    // headphones
#define OUTPUT_SPEAKERS_WIDE    0x00000002
#define OUTPUT_SPEAKERS_NARROW	0x00000003

// Values for Resource Management Mode
#define A3D_RESOURCE_MODE_OFF		     0x00000000
#define A3D_RESOURCE_MODE_NOTIFY	     0x00000001
#define A3D_RESOURCE_MODE_DYNAMIC     	 0x00000002

// Declare the IA3d Interface.  It's not very complex at all.

#undef INTERFACE
#define INTERFACE IA3d

typedef struct IA3d *LPIA3D;

DECLARE_INTERFACE_(IA3d, IUnknown)
{
    // IUnknown 
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)  (THIS) PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // IA3d
    STDMETHOD(SetOutputMode)(THIS_ DWORD    dwFrontXtalkMode, DWORD    dwBackXtalkMode, DWORD    dwQuadMode) PURE;
    STDMETHOD(GetOutputMode)(THIS_ DWORD *lpdwFrontXtalkMode, DWORD *lpdwBackXtalkMode, DWORD *lpdwQuadMode) PURE;

	STDMETHOD(SetResourceManagerMode) (THIS_ DWORD  ) PURE;
	STDMETHOD(GetResourceManagerMode) (THIS_ DWORD *) PURE;

	STDMETHOD(SetHFAbsorbFactor)(THIS_ FLOAT  ) PURE;
    STDMETHOD(GetHFAbsorbFactor)(THIS_ FLOAT *) PURE;

};




// The library function that gets things going.  It returns an interface
// pointer to DirectSound.

#define A3D_OK			1	// A3dCreate returns this upon detection of A3D enabled hardware.

__declspec (dllexport) HRESULT WINAPI
A3dCreate(GUID * lpGUID, LPDIRECTSOUND * ppDS, IUnknown FAR *pUnkOuter );

// Usefull Macros for C folks.

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IA3d_QueryInterface(p,a,b)			(p)->lpVtbl->QueryInterface(p,a,b)
#define IA3d_AddRef(p)						(p)->lpVtbl->AddRef(p)
#define IA3d_Release(p)						(p)->lpVtbl->Release(p)
#define IA3d_SetOutputMode(p,a,b,c)			(p)->lpVtbl->SetOutputMode(p,a,b,c)
#define IA3d_GetOutputMode(p,a,b,c)			(p)->lpVtbl->GetOutputMode(p,a,b,c)
#define IA3d_SetResourceManagerMode(p,a)	(p)->lpVtbl->SetResourceManagerMode(p,a)
#define IA3d_GetResourceManagerMode(p,a)	(p)->lpVtbl->GetResourceManagerMode(p,a)
#define IA3d_SetHFAbsorbFactor(p,a)			(p)->lpVtbl->SetHFAbsorbFactor(p,a)
#define IA3d_GetHFAbsorbFactor(p,a)			(p)->lpVtbl->GetHFAbsorbFactor(p,a)

    
#else										
#define IA3d_QueryInterface(p,a,b)			(p)->QueryInterface(a,b)
#define IA3d_AddRef(p)						(p)->AddRef()
#define IA3d_Release(p)						(p)->Release()
#define IA3d_SetOutputMode(p,a,b,c)			(p)->SetOutputMode(a,b,c)
#define IA3d_GetOutputMode(p,a,b,c)			(p)->GetOutputMode(a,b,c)
#define IA3d_SetResourceManagerMode(p,a)	(p)->SetResourceManagerMode(a)
#define IA3d_GetResourceManagerMode(p,a)	(p)->GetResourceManagerMode(a)
#define IA3d_SetHFAbsorbFactor(p,a)			(p)->SetHFAbsorbFactor(a)
#define IA3d_GetHFAbsorbFactor(p,a)			(p)->GetHFAbsorbFactor(a)

#endif



#ifdef __cplusplus
};
#endif

				  

#endif // _IA3D_H_
