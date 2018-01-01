# Microsoft Developer Studio Project File - Name="QuantizerTool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=QuantizerTool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "QuantizerTool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "QuantizerTool.mak" CFG="QuantizerTool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "QuantizerTool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "QuantizerTool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "QuantizerTool - Win32 Final" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "QuantizerTool - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\QuantizerTool\Debug"
# PROP BASE Intermediate_Dir ".\QuantizerTool\Debug"
# PROP BASE Target_Dir ".\QuantizerTool"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\QuantizerTool"
# PROP Intermediate_Dir ".\Build\Debug\QuantizerTool"
# PROP Target_Dir ".\QuantizerTool"
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "QuantizerTool - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\QuantizerTool\Release"
# PROP BASE Intermediate_Dir ".\QuantizerTool\Release"
# PROP BASE Target_Dir ".\QuantizerTool"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\QuantizerTool"
# PROP Intermediate_Dir ".\Build\Release\QuantizerTool"
# PROP Target_Dir ".\QuantizerTool"
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_RELEASE /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /profile /debug /machine:I386

!ELSEIF  "$(CFG)" == "QuantizerTool - Win32 Final"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\QuantizerTool\Final"
# PROP BASE Intermediate_Dir ".\QuantizerTool\Final"
# PROP BASE Target_Dir ".\QuantizerTool"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\QuantizerTool"
# PROP Intermediate_Dir ".\Build\Final\QuantizerTool"
# PROP Target_Dir ".\QuantizerTool"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /MD /W3 /GR /O2 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /profile /incremental:yes /debug

!ENDIF 

# Begin Target

# Name "QuantizerTool - Win32 Debug"
# Name "QuantizerTool - Win32 Release"
# Name "QuantizerTool - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Tools\QuantizerTool\QuantizerTool.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\QuantizerTool\QuantizerTool.rc
# ADD BASE RSC /l 0x409 /i "Source\Tools\QuantizerTool"
# ADD RSC /l 0x409 /i "Source\Tools\QuantizerTool" /i ".\Source\Tools\QuantizerTool"
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\QuantizerTool\QuantizerToolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\QuantizerTool\StdAfx.cpp

!IF  "$(CFG)" == "QuantizerTool - Win32 Debug"

# ADD CPP /Yc

!ELSEIF  "$(CFG)" == "QuantizerTool - Win32 Release"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "QuantizerTool - Win32 Final"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Source\Tools\QuantizerTool\QuantizerTool.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\QuantizerTool\QuantizerToolDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\QuantizerTool\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Source\Tools\QuantizerTool\res\QuantizerTool.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\QuantizerTool\res\QuantizerTool.rc2
# End Source File
# End Group
# End Target
# End Project
