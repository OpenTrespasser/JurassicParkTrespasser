#pragma once

enum class WindowMode {
	UNDEFINED = 0,
	FRAMED = 1,
	BORDERLESS = 2,
	EXCLUSIVE = 3
};

WindowMode GetWindowModeConfigured();
