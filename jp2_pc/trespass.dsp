# Microsoft Developer Studio Project File - Name="trespass" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=trespass - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "trespass.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "trespass.mak" CFG="trespass - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "trespass - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "trespass - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "trespass - Win32 Final" (based on "Win32 (x86) Application")
!MESSAGE "trespass - Win32 Final P6" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "trespass - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\trespass\Release"
# PROP BASE Intermediate_Dir ".\trespass\Release"
# PROP BASE Target_Dir ".\trespass"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\build\release\trespass"
# PROP Intermediate_Dir ".\build\release\trespass"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\trespass"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\source" /I "\jp2_pc\inc\smacker" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "NDEBUG" /D BUILDVER_MODE=MODE_RELEASE /D "WIN32" /D "_WINDOWS" /D "TRESPASS" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 lib\smacker\smackw32.lib comctl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib lz32.lib version.lib /nologo /stack:0x100000 /subsystem:windows /incremental:yes /debug /debugtype:both /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libci" /SECTION:SelfMod,ERW
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "trespass - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\trespass\Debug"
# PROP BASE Intermediate_Dir ".\trespass\Debug"
# PROP BASE Target_Dir ".\trespass"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\build\debug\trespass"
# PROP Intermediate_Dir ".\build\debug\trespass"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\trespass"
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /Zi /Od /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\source" /I "\jp2_pc\inc\smacker" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "_DEBUG" /D BUILDVER_MODE=MODE_DEBUG /D "WIN32" /D "_WINDOWS" /D "TRESPASS" /YX"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 lib\smacker\smackw32.lib comctl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib lz32.lib version.lib /nologo /stack:0x100000 /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libcid" /SECTION:SelfMod,ERW
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "trespass - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\trespass\Final"
# PROP BASE Intermediate_Dir ".\trespass\Final"
# PROP BASE Target_Dir ".\trespass"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\build\final\trespass"
# PROP Intermediate_Dir ".\build\final\trespass"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\trespass"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\source" /I "\jp2_pc\inc\smacker" /I "\jp2_pc\inc\stl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D BUILDVER_MODE=MODE_FINAL /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\source" /I "\jp2_pc\inc\smacker" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "NDEBUG" /D BUILDVER_MODE=MODE_FINAL /D "WIN32" /D "_WINDOWS" /D "TRESPASS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 lib\smacker\smackw32.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libci"
# ADD LINK32 lib\smacker\smackw32.lib comctl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib lz32.lib version.lib /nologo /stack:0x100000 /subsystem:windows /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libci" /SECTION:SelfMod,ERW
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "trespass - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "trespass\Final P6"
# PROP BASE Intermediate_Dir "trespass\Final P6"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir "trespass"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\trespass"
# PROP Intermediate_Dir ".\Build\Final_P6\trespass"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "trespass"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\source" /I "\jp2_pc\inc\smacker" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "NDEBUG" /D BUILDVER_MODE=MODE_FINAL /D "WIN32" /D "_WINDOWS" /D "TRESPASS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G6 /MD /W3 /GR /O2 /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\source" /I "\jp2_pc\inc\smacker" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "TRESPASS" /D "_WINDOWS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /D TARGET_PROCESSOR=PROCESSOR_PENTIUMPRO /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 lib\smacker\smackw32.lib comctl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib lz32.lib version.lib /nologo /stack:0x100000 /subsystem:windows /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libci"
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 lib\smacker\smackw32.lib comctl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib lz32.lib version.lib /nologo /stack:0x100000 /subsystem:windows /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libci" /SECTION:SelfMod,ERW
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "trespass - Win32 Release"
# Name "trespass - Win32 Debug"
# Name "trespass - Win32 Final"
# Name "trespass - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\trespass\aaaa.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\Cdib.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\ctrls.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\DDDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\Dialogs.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\dlgrender.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\gamewnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\keyremap.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\main.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\mainwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\rasterdc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\saveload.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\supportfn.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\token.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\tpassglobals.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\trespass.rc
# ADD BASE RSC /l 0x409 /i "Source\trespass"
# ADD RSC /l 0x409 /i "Source\trespass" /i ".\Source\trespass"
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\uidlgs.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\uiwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\video.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Trespass\video.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Source\Trespass\Cdib.h
# End Source File
# Begin Source File

SOURCE=.\Source\Trespass\ctrls.h
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\DDDevice.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Trespass\dialogs.h
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\keyremap.h
# End Source File
# Begin Source File

SOURCE=.\Source\Trespass\main.h
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\precomp.h
# End Source File
# Begin Source File

SOURCE=.\Source\Trespass\rasterdc.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Trespass\supportfn.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Trespass\token.h
# End Source File
# Begin Source File

SOURCE=.\Source\Trespass\tpassglobals.h
# End Source File
# Begin Source File

SOURCE=.\Source\Trespass\uidlgs.h
# End Source File
# Begin Source File

SOURCE=.\Source\Trespass\uiwnd.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Source\trespass\res\icon_rap.ico
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\res\smallPal.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\res\USA_ctrls.rc2
# End Source File
# Begin Source File

SOURCE=.\Source\Trespass\res\USA_hints.rc2
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\res\USA_keyremap.rc2
# End Source File
# Begin Source File

SOURCE=.\Source\trespass\version.rc2
# End Source File
# End Group
# End Target
# End Project
