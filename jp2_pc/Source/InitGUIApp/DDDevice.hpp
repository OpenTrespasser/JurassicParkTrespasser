/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Enumerates DirectDraw devices.
 *
 * Notes:
 *		Other DirectX systems could implement device selection here.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/trespass/DDDevice.hpp                                                 $
 * 
 * 6     8/06/98 7:28p Pkeet
 * Added the 'bModulatedAlpha' flag.
 * 
 * 5     8/05/98 9:39p Pkeet
 * Moved the 'iZBufferBitDepth' data member to the Direct3D structure. Removed unnecessary code.
 * 
 * 4     8/05/98 9:09p Pkeet
 * Added the 'CEnumerateDevices' class.
 * 
 * 3     6/29/98 9:19p Pkeet
 * Simplified code to find GUIDs for DirectDraw and Direct3D.
 * 
 * 2     12/09/97 6:21p Pkeet
 * Added code to set and get D3D driver information.
 * 
 * 1     12/08/97 6:36p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef DDDEVICE_HPP
#define DDDEVICE_HPP


//
// Required includes.
//
#define D3D_OVERLOADS
#include <ddraw.h>
#include <d3d.h>

	
const int iMaxDevices = 20;


struct SDDDevice
{
	GUID guidID;
	char strName[256];
	char strDescription[1024];
};


struct SD3DDevice
{
	GUID guidID;
	char strName[256];
	char strDescription[1024];
	bool bSupported;
	int  iZBufferBitDepth;
	bool bModulatedAlpha;
};


struct SResolution
{
	int iWidth;
	int iHeight;
	int iRefreshHz;
	static int iResolutionsCount;
};

BOOL bFindResolutions(GUID& guid, SResolution* ar, int& ri_num_resolutions);

BOOL bFindD3D(GUID& guid, SD3DDevice* pd3d_device);

class CEnumerateDevices
{
public:

	struct SDevice
	{
		bool       bD3DDeviceFound;
		SDDDevice  ddDevice;
		SD3DDevice d3dDevice;
	};

private:

	int iNumDevices;
	int iSelectedDevice;
	SDevice Devices[iMaxDevices];

public:

	CEnumerateDevices();

	int iGetNumDevices() const
	{
		return iNumDevices;
	}

	SDevice devGetDevice(int i) const
	{
		return Devices[i];
	}

	SDevice devGetSelectedDevice() const
	{
		return Devices[iSelectedDevice];
	}

	void WriteSelectedDeviceToRegistry();

	void SelectDevice(char* str);

	void GetTitle(int i, char* str) const;

	void AddDDDevice(const SDDDevice& ddd);
	void RemoveDevice(int i);

};

extern CEnumerateDevices *penumdevDevices;



#endif // DDDEVICE_HPP