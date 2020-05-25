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

WindowMode GetWindowModeActual()
{
	if (GetRegValue(strFLAG_D3D, DEFAULT_D3D))
		return WindowMode::EXCLUSIVE;
	else
		return GetWindowModeConfigured();
}
