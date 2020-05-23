#pragma once

#include "Windows.h"

bool IsProcessElevated();
bool StartAsElevated(HWND hwnd, HINSTANCE hinst);
