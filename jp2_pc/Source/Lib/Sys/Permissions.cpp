#include "Permissions.hpp"
#include <atlbase.h>

//Based on https://github.com/microsoftarchive/msdn-code-gallery-microsoft/tree/master/OneCodeTeam/UAC%20self-elevation%20(CppUACSelfElevation)/%5BC%2B%2B%5D-UAC%20self-elevation%20(CppUACSelfElevation)/C%2B%2B

bool IsProcessElevated()
{
	ATL::CHandle hToken(INVALID_HANDLE_VALUE);

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken.m_h))
		return false;
	if (hToken == nullptr || hToken == INVALID_HANDLE_VALUE)
		return false;

	TOKEN_ELEVATION elevation = { 0 };
	DWORD dwSize = 0;
	if (!GetTokenInformation(hToken, TokenElevation, &elevation,
		sizeof(elevation), &dwSize))
		return false;

	return elevation.TokenIsElevated;
}

bool StartAsElevated(HWND wnd, HINSTANCE inst)
{
	TCHAR name[_MAX_PATH] = { '\0' };
	if (!GetModuleFileName(inst, name, sizeof(name)))
		return false;

	SHELLEXECUTEINFO sei = { 0 };
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.lpVerb = _T("runas");
	sei.lpFile = name;
	sei.hwnd = wnd;
	sei.nShow = SW_NORMAL;

	return ShellExecuteEx(&sei);
}
