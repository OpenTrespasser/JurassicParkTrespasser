# Microsoft Developer Studio Project File - Name="System" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=System - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "System.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "System.mak" CFG="System - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "System - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "System - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "System - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "System - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "System - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\System\Release"
# PROP BASE Intermediate_Dir ".\System\Release"
# PROP BASE Target_Dir ".\System"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\System"
# PROP Intermediate_Dir ".\Build\Release\System"
# PROP Target_Dir ".\System"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_RELEASE /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "System - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\System\Debug"
# PROP BASE Intermediate_Dir ".\System\Debug"
# PROP BASE Target_Dir ".\System"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\System"
# PROP Intermediate_Dir ".\Build\Debug\System"
# PROP Target_Dir ".\System"
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /GR /Zi /Od /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "_DEBUG" /D BUILDVER_MODE=MODE_DEBUG /D "WIN32" /D "_WINDOWS" /D "VC_EXTRALEAN" /D "_MBCS" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "System - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\System\Final"
# PROP BASE Intermediate_Dir ".\System\Final"
# PROP BASE Target_Dir ".\System"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\System"
# PROP Intermediate_Dir ".\Build\Final\System"
# PROP Target_Dir ".\System"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D "NDEBUG" /D BUILDVER_MODE=MODE_OPTIMISE /D "WIN32" /D "_WINDOWS" /D "VC_EXTRALEAN" /c
# ADD CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "System - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "System\Final P6"
# PROP BASE Intermediate_Dir "System\Final P6"
# PROP BASE Target_Dir "System"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\System"
# PROP Intermediate_Dir ".\Build\Final_P6\System"
# PROP Target_Dir "System"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "VC_EXTRALEAN" /D "_WINDOWS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /D TARGET_PROCESSOR=PROCESSOR_PENTIUMPRO /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "System - Win32 Release"
# Name "System - Win32 Debug"
# Name "System - Win32 Final"
# Name "System - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Lib\Sys\BitBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\W95\Com.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\ConIO.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\control\Control.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\DebugConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\W95\errors.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\W95\ExePageModify.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\W95\FastHeap.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\W95\FileEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\FileMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\FixedHeap.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\LRU.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\MemoryLog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\PerformanceCount.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\ProcessorDetect.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\Profile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\reg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\RegInit.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\W95\Render.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\Scheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\StdDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\W95\textout.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\ThreadControl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\W95\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\VirtualMem.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Shell\WinRenderTools.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\DirectX\dinput.lib
# End Source File
# Begin Source File

SOURCE=.\lib\DirectX\dxguid.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\source\Lib\Sys\BitBuffer.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\W95\Com.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GblInc\Config.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\ConIO.hpp
# End Source File
# Begin Source File

SOURCE=.\source\lib\control\Control.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\DebugConsole.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\Errors.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\ExePageModify.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\FastHeap.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\FileMapping.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\FixedHeap.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\LRU.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\MemoryLog.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\lib\sys\performancecount.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\lib\sys\ProcessorDetect.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\Profile.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\reg.h
# End Source File
# Begin Source File

SOURCE=.\Source\lib\sys\reginit.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\lib\sys\w95\render.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\Scheduler.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\StdDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\ThreadControl.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\VirtualMem.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\shell\winrendertools.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
