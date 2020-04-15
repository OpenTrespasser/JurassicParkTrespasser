/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 ***********************************************************************************************
 *
 * $Log: /JP2_PC/Source/Test/BumpBuild/IOStuff.cpp $
 * 
 * 1     9/16/96 2:00p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include "IOStuff.hpp"

DWORD TickCount;
FILE* fl_file_stream;
BOOL  DoFileOutput;

void DisplayMemory()
{
	MEMORYSTATUS MemStatus;

	GlobalMemoryStatus(&MemStatus);
	
	print("");
    print("Available Physical Memory (kb)", MemStatus.dwAvailPhys >> 10);     // DWORD free physical memory bytes 
    print("Available Page File (Mb)",       MemStatus.dwAvailPageFile >> 20); // DWORD free bytes of paging file 
    print("Available Virtual (Mb)",         MemStatus.dwAvailVirtual >> 20);  // DWORD free user bytes 

}

void Begin(BOOL FileOutput)
{
	char FileName[256] = "bumpbuild.txt";
	time_t tm_timestamp;
	
	DoFileOutput = FileOutput;

	if (DoFileOutput)
	{
		//printf("Enter name of shadow file (no extension) > ");
		//scanf("%s", FileName);
		strcat(FileName, ".txt");
		fl_file_stream = freopen(FileName, "w", stderr);
		time(&tm_timestamp);
		fprintf(fl_file_stream, "------------------------------------------------------------\n\n");
		fprintf(fl_file_stream, "File Name:     %s\n", FileName);
		fprintf(fl_file_stream, "Date and Time: %s\n", ctime(&tm_timestamp));
		fprintf(fl_file_stream, "------------------------------------------------------------\n\n");
	}
	//WaitToContinue();
}

void print(LPSTR lpstr)
{
	printf("%s\n", lpstr);
	if (DoFileOutput) fprintf(fl_file_stream, "%s\n", lpstr);
}

void print(LPSTR lpstr, LPSTR str2)
{
	printf("%30s : %s\n", lpstr, str2);
	if (DoFileOutput) fprintf(fl_file_stream, "%30s : %s\n", lpstr, str2);
}

void print4(LPSTR lpstr, double d)
{
	printf("%30s : %1.20f\n", lpstr, d);
	if (DoFileOutput) fprintf(fl_file_stream, "%30s : %1.20f\n", lpstr, d);
}

void print2(LPSTR lpstr, DWORD dw, DWORD dw2)
{
	printf("%30s : ", lpstr);
	if (DoFileOutput) fprintf(fl_file_stream, "%30s : ", lpstr);
	printf("%ld\t", dw);
	if (DoFileOutput) fprintf(fl_file_stream, "%ld\t", dw);
	printf("%ld", dw2);
	if (DoFileOutput) fprintf(fl_file_stream, "%ld", dw2);
	printf("\n");
	if (DoFileOutput) fprintf(fl_file_stream, "\n");
}


void print3(LPSTR lpstr, DWORD dw, DWORD dw2, DWORD dw3)
{
	printf("%30s : ", lpstr);
	if (DoFileOutput) fprintf(fl_file_stream, "%30s : ", lpstr);

	printf("%ld\t", dw);
	if (DoFileOutput) fprintf(fl_file_stream, "%ld\t", dw);

	printf("%ld\t", dw2);
	if (DoFileOutput) fprintf(fl_file_stream, "%ld\t", dw2);

	printf("%ld", dw3);
	if (DoFileOutput) fprintf(fl_file_stream, "%ld", dw3);

	printf("\n");
	if (DoFileOutput) fprintf(fl_file_stream, "\n");
}

void print(LPSTR lpstr, DWORD dw, DWORD format)
{
	printf("%30s : ", lpstr);
	if (DoFileOutput) fprintf(fl_file_stream, "%30s : ", lpstr);
	switch (format)
	{
		case DATA_INT:
			printf("%ld", dw);
			if (DoFileOutput) fprintf(fl_file_stream, "%ld", dw);
			break;
		case DATA_HEX:
			printf("%X", dw);
			if (DoFileOutput) fprintf(fl_file_stream, "%X", dw);
			break;
		default:;
	}
	printf("\n");
	if (DoFileOutput) fprintf(fl_file_stream, "\n");
}

void StartTimer()
{
	TickCount = GetTickCount();
}

void StopTimer(DWORD& u4_time)
{
	u4_time = GetTickCount() - TickCount;
}

void StopTimer()
{
	TickCount = GetTickCount() - TickCount;
	print("");
	print("Time in milliseconds", TickCount);
	print("");
}

void OutputError()
{
	print("Error", GetLastError(), DATA_HEX);
}

void DisplaySystemInfo()
{
	LPVOID       lpv = NULL;
	SYSTEM_INFO  SysInfo;
	MEMORYSTATUS MemStatus;

	GetSystemInfo(&SysInfo);

	print("System Info:\n");
	print("dwOemId",                     SysInfo.dwOemId);
	print("dwPageSize",                  SysInfo.dwPageSize);
	print("lpMinimumApplicationAddress", (DWORD)SysInfo.lpMinimumApplicationAddress, DATA_HEX);
	print("lpMaximumApplicationAddress", (DWORD)SysInfo.lpMaximumApplicationAddress, DATA_HEX);
	print("dwActiveProcessorMask",       SysInfo.dwActiveProcessorMask);
	print("dwNumberOfProcessors",        SysInfo.dwNumberOfProcessors);
	print("dwProcessorType",             SysInfo.dwProcessorType);
	print("dwAllocationGranularity",     SysInfo.dwAllocationGranularity);
	print("wProcessorLevel",             SysInfo.wProcessorLevel);
	print("wProcessorRevision",          SysInfo.wProcessorRevision);

	GlobalMemoryStatus(&MemStatus);
	
	print("");
    print("Memory Load %",                  MemStatus.dwMemoryLoad);    // DWORD percent of memory in use 
    print("Total Physical Memory (kb)",     MemStatus.dwTotalPhys >> 10);     // DWORD bytes of physical memory 
    print("Available Physical Memory (kb)", MemStatus.dwAvailPhys >> 10);     // DWORD free physical memory bytes 
    print("Total Page File (Mb)",           MemStatus.dwTotalPageFile >> 20); // DWORD bytes of paging file 
    print("Available Page File (Mb)",       MemStatus.dwAvailPageFile >> 20); // DWORD free bytes of paging file 
    print("Total Virtual (Mb)",             MemStatus.dwTotalVirtual >> 20);  // DWORD user bytes of address space 
    print("Available Virtual (Mb)",         MemStatus.dwAvailVirtual >> 20);  // DWORD free user bytes 

}

void WaitToContinue()
{
	print("\nPress any key to continue...\n");
	while(!_kbhit());
	_getch();
}

void End()
{
	print("\nPress any key to exit...");
	while(!_kbhit());
	_getch();
	if (DoFileOutput) fclose(fl_file_stream);
}