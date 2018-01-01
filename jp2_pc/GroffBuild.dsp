# Microsoft Developer Studio Project File - Name="GroffBuild" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GroffBuild - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GroffBuild.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GroffBuild.mak" CFG="GroffBuild - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GroffBuild - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GroffBuild - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "GroffBuild - Win32 Final" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GroffBuild - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\GroffBuild\Release"
# PROP BASE Intermediate_Dir ".\GroffBuild\Release"
# PROP BASE Target_Dir ".\GroffBuild"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\build\release\GroffBuild"
# PROP Intermediate_Dir ".\build\release\GroffBuild"
# PROP Target_Dir ".\GroffBuild"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\source" /I "\jp2_pc\inc\stl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D BUILDVER_MODE=MODE_FINAL /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib lz32.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "GroffBuild - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\GroffBuild\Debug"
# PROP BASE Intermediate_Dir ".\GroffBuild\Debug"
# PROP BASE Target_Dir ".\GroffBuild"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\build\debug\GroffBuild"
# PROP Intermediate_Dir ".\build\debug\GroffBuild"
# PROP Target_Dir ".\GroffBuild"
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\source" /I "\jp2_pc\inc\stl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D BUILDVER_MODE=MODE_DEBUG /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 winmm.lib lz32.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "GroffBuild - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\GroffBuild\Final"
# PROP BASE Intermediate_Dir ".\GroffBuild\Final"
# PROP BASE Target_Dir ".\GroffBuild"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\GroffBuild\Final"
# PROP Intermediate_Dir ".\GroffBuild\Final"
# PROP Target_Dir ".\GroffBuild"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\source" /I "\jp2_pc\inc\stl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D BUILDVER_MODE=MODE_FINAL /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib lz32.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386

!ENDIF 

# Begin Target

# Name "GroffBuild - Win32 Release"
# Name "GroffBuild - Win32 Debug"
# Name "GroffBuild - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\GroffBuild\Dialogs.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GroffBuild\gbuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GroffBuild\GroffBuild.rc
# ADD BASE RSC /l 0x409 /i "Source\GroffBuild"
# ADD RSC /l 0x409 /i "Source\GroffBuild" /i ".\Source\GroffBuild"
# End Source File
# Begin Source File

SOURCE=.\Source\GroffBuild\main.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GroffBuild\maindlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GroffBuild\reg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GroffBuild\Utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Source\GroffBuild\dialogs.h
# End Source File
# Begin Source File

SOURCE=.\Source\GroffBuild\gbuilder.h
# End Source File
# Begin Source File

SOURCE=.\Source\GroffBuild\main.h
# End Source File
# Begin Source File

SOURCE=.\Source\GroffBuild\reg.h
# End Source File
# Begin Source File

SOURCE=.\Source\GroffBuild\utils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
