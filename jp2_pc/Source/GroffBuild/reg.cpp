
#include "precomp.h"
#pragma hdrstop

#define REGKEYPARENT HKEY_CURRENT_USER
#define REGLOCATION "Software\\DreamWorks\\GroffBuild"

HKEY    g_hKey = NULL;


void CloseKey()
{
	if (g_hKey)
		RegCloseKey (g_hKey);
}

void OpenKey ()
{
	if (RegOpenKeyEx (REGKEYPARENT,
				REGLOCATION,
				0,
				KEY_ALL_ACCESS,
				&g_hKey)!=ERROR_SUCCESS)
	{
		RegCreateKey (REGKEYPARENT,
				REGLOCATION,
				&g_hKey);
	}
}

void SetRegValue (LPCSTR lpszVal, int nVal)
{
	RegSetValueEx (g_hKey,
		lpszVal,
		NULL,
		REG_DWORD,
		(LPBYTE)&nVal,
		sizeof(nVal));
}


int GetRegValue (LPCSTR lpszVal, int nDefault)
{
	DWORD dwType,dwSize;
	DWORD nRes;
	dwSize = sizeof(nRes);
	if (RegQueryValueEx (g_hKey,
		lpszVal,
		NULL,
		&dwType,
		(LPBYTE)&nRes,
		&dwSize)==ERROR_SUCCESS)
	{
		if (dwType==REG_DWORD)
			return (int)nRes;
	}
	return nDefault;
}

void SetRegString (LPCSTR lpszVal, LPCSTR lpszString)
{
	RegSetValueEx (g_hKey,
		lpszVal,
		NULL,
		REG_SZ,
		(LPBYTE)lpszString,
		lstrlen(lpszString)+1);
}

// returns length
int GetRegString (LPCSTR lpszVal, LPSTR lpszString, int nSize, LPCSTR lpszDefault)
{
	DWORD dwType,dwSize;
	dwSize = nSize;
	if (RegQueryValueEx (g_hKey,
		lpszVal,
		NULL,
		&dwType,
		(LPBYTE)lpszString,
		&dwSize)!=ERROR_SUCCESS)
	{
		if (lpszDefault)
		{
			lstrcpy (lpszString, lpszDefault);
			dwSize = lstrlen(lpszDefault);
		}
		else
			dwSize = 0;
	}
	return dwSize;
}

void SetRegData (LPCSTR lpszVal, LPBYTE lpszData, int nSize)
{
	RegSetValueEx (g_hKey,
		lpszVal,
		NULL,
		REG_BINARY,
		lpszData,
		nSize);
}

// returns length
int GetRegData (LPCSTR lpszVal, LPBYTE lpszData, int nSize)
{
	DWORD dwType,dwSize;
	dwSize = nSize;
	if (RegQueryValueEx (g_hKey,
		lpszVal,
		NULL,
		&dwType,
		lpszData,
		&dwSize)!=ERROR_SUCCESS)
		dwSize = 0;
	return dwSize;
}

void DeleteValue(LPCSTR lpszVal)
{
	RegDeleteValue (g_hKey, lpszVal);
}
