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
 * 9     9/23/98 4:47p Pkeet
 * Boxes and buttons that are not used by the software renderer are disabled.
 * 
 * 8     8/29/98 3:52p Pkeet
 * Added data members to detect cards that should use smaller textures.
 * 
 * 7     8/23/98 3:30p Pkeet
 * Application defaults to 320x240 resolution. If that resolution is not available, it chooses
 * the next lowest resolution. If there are no lower resolutions, it selects the next highest
 * resolution.
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
#include "../Lib//W95/Direct3DCards.hpp"
#define D3D_OVERLOADS
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
	EVideoCard evcVideoCard;
	bool bSmallMemory;
};


struct SResolution
{
	int iWidth;
	int iHeight;
	int iRefreshHz;
	static int iResolutionsCount;

	SResolution()
		: iWidth(0), iHeight(0), iRefreshHz(0)
	{
	}

	SResolution(int i_width, int i_height, int i_hz = 0)
		: iWidth(i_width), iHeight(i_height), iRefreshHz(i_hz)
	{
	}

	//******************************************************************************************
	//
	// Comparison operators.
	//
	// Notes:
	//		These operators are to be used for matching dimensions only.
	//

	bool operator ==(const SResolution& res) const
	{
		return iWidth == res.iWidth && iHeight == res.iHeight;
	}

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

	bool bSoftwareSelected()
	{
		if (iNumDevices < 1)
			return true;
		return iSelectedDevice == iNumDevices - 1;
	}

};

extern CEnumerateDevices *penumdevDevices;



#endif // DDDEVICE_HPP