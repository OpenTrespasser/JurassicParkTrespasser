/*
//   AAUDIO.H
//
//              INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//
//      Copyright (c) 1995 Intel Corporation. All Rights Reserved.
//
//  PVCS:
//      $Workfile: aaudio.h $
//      $Revision: 1 $
//      $Modtime: 5/31/96 8:57a $
//
//  PURPOSE:
//
//  CONTENTS:
//    
*/


#ifndef __AAUDIO_INCLUDED__
#define __AAUDIO_INCLUDED__
#ifdef _WIN32
#define COM_NO_WINDOWS_H
#include <objbase.h>
#else
#define IUnknown        void
#endif

#ifdef __cplusplus
extern "C" {
#endif



#ifdef _WIN32

// {2BA4CC21-0540-11cf-8963-444553540000}
DEFINE_GUID(CLSID_AAudio, 0x2ba4cc21, 0x540, 0x11cf, 0x89, 0x63, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);

DEFINE_GUID(IID_IAAudio, 0xce918a26, 0x308, 0x11cf, 0xb3, 0x0, 0x0, 0xaa, 0x0, 0x59, 0x48, 0xe9);
DEFINE_GUID(IID_IAAudioEmitter, 0xa6424a0, 0x317, 0x11cf, 0x9a, 0xae, 0x0, 0xaa, 0x0, 0x4b, 0x43, 0x67);
DEFINE_GUID(IID_IAAudioListener, 0x37b7b160, 0x3d0, 0x11cf, 0x89, 0x63, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);
#endif

struct IAAudio;
struct IAAudioEmitter;

typedef struct IAAudio                      FAR *LPAAUDIO;
typedef struct IAAudioEmitter               FAR *LPAAUDIOEMITTER;
typedef struct IAAudioListener              FAR *LPAAUDIOLISTENER;

typedef struct _AAEMITTERMODEL                  FAR *LPAAEMITTERMODEL;
typedef struct _AAEMITTERDESC                   FAR *LPAAEMITTERDESC;
typedef struct _AALISTENERDESC                  FAR *LPAALISTENERDESC;
typedef struct _AAENVIRONMENT                   FAR *LPAAENVIRONMENT;
typedef struct _VECTOR3D                        FAR *LPVECTOR3D;


// the extra 8 are to match Microsoft Internet URL max definitions
#define AA_MAX_NAME_LEN     (MAX_PATH + 8)


#ifdef _WIN32
#undef INTERFACE
#define INTERFACE IAAudio
DECLARE_INTERFACE_( IAAudio, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    
    /*** IAAudio methods ***/
    STDMETHOD(SetEnvironment)(THIS_ LPAAENVIRONMENT lpEnvAttr) PURE;
    STDMETHOD(CreateEmitter)(THIS_  LPAAEMITTERDESC lpEmitterAttr, LPAAUDIOEMITTER FAR *lpEmitterInterface, IUnknown FAR *reserved) PURE;    
    STDMETHOD(CreateListener)(THIS_  LPAALISTENERDESC lpListenerAttr, LPAAUDIOLISTENER FAR *lpListenerInterface, IUnknown FAR *reserved) PURE;

};
#endif


#ifdef _WIN32
#undef INTERFACE
#define INTERFACE IAAudioEmitter
DECLARE_INTERFACE_( IAAudioEmitter, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    
    /*** IAAudioEmitter methods ***/
    STDMETHOD(ControlMedia)(THIS_ DWORD dwControl) PURE;
	STDMETHOD(QueryMediaState)(THIS_ LPDWORD lpdwControl, PFLOAT pfSecondsPlayed, PFLOAT pfTotalTime) PURE;
    
	STDMETHOD(SetPosition)(THIS_ LPVECTOR3D lpPosition) PURE;
    STDMETHOD(SetOrientation)(THIS_ LPVECTOR3D lpOrientation) PURE;
    STDMETHOD(SetVelocity)(THIS_ LPVECTOR3D lpVelocity) PURE;
	STDMETHOD(SetModel)(THIS_ LPAAEMITTERMODEL lpEmitterModel) PURE;

    STDMETHOD(GetPosition)(THIS_ LPVECTOR3D lpPosition) PURE;
    STDMETHOD(GetOrientation)(THIS_ LPVECTOR3D lpOrientation) PURE;
    STDMETHOD(GetVelocity)(THIS_ LPVECTOR3D lpVelocity) PURE;
	STDMETHOD(GetModel)(THIS_ LPAAEMITTERMODEL lpEmitterModel) PURE;
	STDMETHOD(GetUserData)(THIS_ LPDWORD lpdwUser) PURE;

};
#endif

#ifdef _WIN32
#undef INTERFACE
#define INTERFACE IAAudioListener
DECLARE_INTERFACE_( IAAudioListener, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    
    /*** IAAudioListener methods ***/
    STDMETHOD(SetPosition)(THIS_ LPVECTOR3D lpPosition) PURE;
    STDMETHOD(SetOrientation)(THIS_ LPVECTOR3D lpDirection, LPVECTOR3D lpUp) PURE;
    STDMETHOD(SetVelocity)(THIS_ LPVECTOR3D lpVelocity) PURE;

    STDMETHOD(GetPosition)(THIS_ LPVECTOR3D lpPosition) PURE;
    STDMETHOD(GetOrientation)(THIS_ LPVECTOR3D lpDirection, LPVECTOR3D lpUp) PURE;
    STDMETHOD(GetVelocity)(THIS_ LPVECTOR3D lpVelocity) PURE;
	STDMETHOD(GetUserData)(THIS_ LPDWORD lpdwUser) PURE;

};
#endif


/*
// Enumerated Types
*/
enum AASoundType { AMBIENT, LOCALIZED };
enum AAUnitOfMeasure { METERS, FEET };
enum AAReverbType {NONE, MOUNTAIN_RANGE, VALLEY, CANYON, AMPHITHEATRE, STADIUM, 
    LECTURE_HALL, LOBBY, OFFICE, HOME, BATHROOM};
enum AAOutputDeviceType { HEADPHONES, STEREO_FRONT, QUAD_SURROUND, STEREO_PLUS_READ, DOLBY_AC3 };
enum AALocalizeCPU { CPU_NONE_LOCALIZE, CPU_LOW_LOCALIZE, CPU_MODERATE_LOCALIZE };


/*
// VECTOR3D
*/
typedef struct _VECTOR3D
{
    float          x;
    float          y;
    float          z;
    
} VECTOR3D;


/*
// AAEMITTERMODEL
*/
typedef struct _AAEMITTERMODEL {
	float			minBack;
	float			minFront;
	float			maxBack;
	float			maxFront;
	float			intensity;
} AAEMITTERMODEL;


/*
// AAEMITTERDESC
*/
typedef struct _AAEMITTERDESC
{
    char            szFilename[AA_MAX_NAME_LEN];
	DWORD			dwUser;
} AAEMITTERDESC;


/*
// AALISTENERDESC
*/
typedef struct _AALISTENERDESC
{
    enum AAOutputDeviceType     device;
    HWND                        hMainWnd;
	DWORD						dwUser;
} AALISTENERDESC;


/*
// AAENVIRONMENT
*/
typedef struct _AAENVIRONMENT
{
  enum AAReverbType     reverb;
  float                 speedOfSound;
  BOOL                  useRightHand;
  enum AALocalizeCPU	CPUBudget;

} AAENVIRONMENT;


/*
// Flags for ControlMedia
//
*/
#define AA_PLAYSINGLE       0x00000001
#define AA_PLAYLOOP         0x00000002
#define AA_PAUSE            0x00000003
#define AA_RESUME           0x00000004
#define AA_STOP				0x00000005

/*
// Error values
//
*/

// no sound driver installed
#define AAERR_NODRIVER			MAKE_HRESULT( 1, FACILITY_ITF, 10 )

// wave driver doesn't support format
#define AAERR_BADFORMAT			MAKE_HRESULT( 1, FACILITY_ITF, 20 )

// a zero length vector was specified for the orientation
#define AAERR_ZEROVECTOR		MAKE_HRESULT( 1, FACILITY_ITF, 30 )

// an error occurred opening the wave file specified
// when creating the emitter
#define AAERR_FILEOPEN			MAKE_HRESULT( 1, FACILITY_ITF, 40 )

// the wave file is corrupted(not valid)
#define AAERR_CORRUPTFILE		MAKE_HRESULT( 1, FACILITY_ITF, 50 )

// the listeners orienation vectors are parallel
#define AAERR_PARALLELVECTORS	MAKE_HRESULT( 1, FACILITY_ITF, 60 )

// sound resources are allocated or busy
#define AAERR_ALLOCATED			MAKE_HRESULT( 1, FACILITY_ITF, 70 )

// internet can't be initialized
#define AAERR_INET_INIT			MAKE_HRESULT( 1, FACILITY_ITF, 80 )

// error opening URL
#define AAERR_INET_URLERR		MAKE_HRESULT( 1, FACILITY_ITF, 90 )


#ifdef __cplusplus
};
#endif

#endif
