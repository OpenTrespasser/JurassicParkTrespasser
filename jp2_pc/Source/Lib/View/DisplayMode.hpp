#pragma once

#include <Windows.h>

enum class WindowMode {
	UNDEFINED = 0,
	FRAMED = 1,
	BORDERLESS = 2,
	EXCLUSIVE = 3
};

WindowMode GetWindowModeConfigured();
void SetWindowModeConfigured(WindowMode mode);
WindowMode GetWindowModeActive();

int GetSystemBitDepth(HWND wnd);
int GetSystemBitDepth(HDC dc);

bool IsDisplayConfigurationValid(int colorDepth, bool d3dEnabled, WindowMode mode);
