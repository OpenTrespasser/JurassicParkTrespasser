#include "common.hpp"
#include "Lib/Loader/Loader.hpp"

//Global variables and functions declared elsewhere as extern
//needed by the libraries
bool bIsTrespasser = false;
bool bUseReplayFile = false;
bool bInvertMouse = false;
bool bUseOutputFiles = false;
unsigned int g_u4NotifyParam = 0;
unsigned int u4LookupResourceString(int, char*, unsigned int) { return 0; }
void LineColour(int, int, int) {}
PFNWORLDLOADNOTIFY g_pfnWorldLoadNotify = nullptr;
void ResetAppData() {}
void* hwndGetMainHwnd() { return nullptr; }
HINSTANCE hinstGetMainHInstance(){ return nullptr; }
