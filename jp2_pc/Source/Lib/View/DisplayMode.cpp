#pragma once

#include "DisplayMode.hpp"
#include "Lib/Sys/RegInit.hpp"
#include "Lib/Sys/reg.h"

WindowMode GetWindowModeConfigured()
{
	int selection = GetRegValue("WindowMode", 0);
	if (selection < 0 || selection > static_cast<int>(WindowMode::EXCLUSIVE))
		selection = 0;
	return static_cast<WindowMode>(selection);
}

int GetSystemBitDepth(HWND wnd)
{
	return GetSystemBitDepth(GetDC(wnd));
}

int GetSystemBitDepth(HDC dc)
{
	//Gives 16bit when program is set to 16bit color depth compatibility mode
	return GetDeviceCaps(dc, BITSPIXEL);
}

bool IsDisplayConfigurationValid(int colorDepth, bool d3dEnabled, WindowMode mode)
{
	if (mode == WindowMode::UNDEFINED)
		return false;
	if (mode == WindowMode::EXCLUSIVE && d3dEnabled && colorDepth != 16)
		return false;

	return true;
}
