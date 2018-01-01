# Microsoft Developer Studio Project File - Name="Std" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Std - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Std.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Std.mak" CFG="Std - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Std - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Std - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Std - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "Std - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Std - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Std\Release"
# PROP BASE Intermediate_Dir ".\Std\Release"
# PROP BASE Target_Dir ".\Std"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\Std"
# PROP Intermediate_Dir ".\Build\Release\Std"
# PROP Target_Dir ".\Std"
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

!ELSEIF  "$(CFG)" == "Std - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Std\Debug"
# PROP BASE Intermediate_Dir ".\Std\Debug"
# PROP BASE Target_Dir ".\Std"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\Std"
# PROP Intermediate_Dir ".\Build\Debug\Std"
# PROP Target_Dir ".\Std"
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

!ELSEIF  "$(CFG)" == "Std - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Std\Final"
# PROP BASE Intermediate_Dir ".\Std\Final"
# PROP BASE Target_Dir ".\Std"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\Std"
# PROP Intermediate_Dir ".\Build\Final\Std"
# PROP Target_Dir ".\Std"
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

!ELSEIF  "$(CFG)" == "Std - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Std\Final P6"
# PROP BASE Intermediate_Dir "Std\Final P6"
# PROP BASE Target_Dir "Std"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\Std"
# PROP Intermediate_Dir ".\Build\Final_P6\Std"
# PROP Target_Dir "Std"
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

# Name "Std - Win32 Release"
# Name "Std - Win32 Debug"
# Name "Std - Win32 Final"
# Name "Std - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Lib\Std\Hash.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\InitSys.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\Mem.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\Ptr.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\Random.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\stringex.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Source\Lib\Std\Array.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\Array2.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\ArrayAllocator.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\ArrayIO.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\BlockAllocator.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\BlockArray.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GblInc\buildver.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\CircularList.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GblInc\common.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\CRC.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Std\Hash.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Std\InitSys.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\LocalArray.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Std\Mem.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\MemLimits.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\PrivSelf.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Std\Ptr.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Std\Random.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\RangeVar.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\Set.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\Sort.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\SparseArray.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\StdLibEx.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Std\StringEx.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\TreeList.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\UAssert.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\UDefs.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Std\UTypes.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
