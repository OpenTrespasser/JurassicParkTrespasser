/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: Simple IO stuff.
 *
 ***********************************************************************************************
 *
 * $Log: /JP2_PC/Source/Test/BumpBuild/IOStuff.hpp $
 * 
 * 1     9/16/96 2:01p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef IO_STUFF
#define IO_STUFF

#include <Windows.h>
#include <stdio.h>

#define DATA_INT    0
#define DATA_HEX    1

template<class T>
void print_binary(T t)
{
	for (int n = sizeof(T) * 8 - 1; n >= 0; n--)
	{
		if ((n % 8) == 7)
			printf(" ");
		if (((T)1 << n) & t)
			printf("1");
		else
			printf("0");
	}
	printf("\n");
}

extern void Begin(BOOL FileOutput = TRUE);
extern void print(LPSTR lpstr);
extern void print(LPSTR lpstr, LPSTR str2);
extern void print(LPSTR lpstr, DWORD dw, DWORD format = DATA_INT);
extern void print2(LPSTR lpstr, DWORD dw, DWORD dw2);
extern void print3(LPSTR lpstr, DWORD dw, DWORD dw2, DWORD dw3);
extern void print4(LPSTR lpstr, double d);
extern void End();

extern void DisplayMemory();
extern void WaitToContinue();
extern void DisplaySystemInfo();
extern void StartTimer();
extern void StopTimer();
extern void StopTimer(DWORD& u4_time);
extern void OutputError();

#endif