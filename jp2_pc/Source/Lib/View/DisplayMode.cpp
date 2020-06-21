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
	//In some situations it can be helpful to override the configured mode.
	//For example, during some development stages it is possible that not
	//all combinations of window modes and renderers work together. In such
	//a situation an enforcement of a specific window mode is needed.
	//
	//At the same time it should remain possible to query the configured window mode.
	//A possible use case is displaying the value in a settings menu.

	return GetWindowModeConfigured();
}
