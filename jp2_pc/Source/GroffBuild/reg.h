

void OpenKey ();
void CloseKey();
void DeleteValue(LPCSTR lpszVal);
void SetRegValue (LPCSTR lpszVal, int nVal);
int  GetRegValue (LPCSTR lpszVal, int nDefault);
void SetRegString (LPCSTR lpszVal, LPCSTR lpszString);
int  GetRegString (LPCSTR lpszVal, LPSTR lpszString, int nSize, LPCSTR lpszDefault);
void SetRegData (LPCSTR lpszVal, LPBYTE lpszData, int nSize);
int  GetRegData (LPCSTR lpszVal, LPBYTE lpszData, int nSize);