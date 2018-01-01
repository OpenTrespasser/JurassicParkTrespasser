# Microsoft Developer Studio Project File - Name="CollisionEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CollisionEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CollisionEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CollisionEditor.mak" CFG="CollisionEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CollisionEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\CollisionEditor\Debug"
# PROP BASE Intermediate_Dir ".\CollisionEditor\Debug"
# PROP BASE Target_Dir ".\CollisionEditor"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\CollisionEditor"
# PROP Intermediate_Dir ".\Build\Debug\CollisionEditor"
# PROP Target_Dir ".\CollisionEditor"
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /Zi /Od /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D "_DEBUG" /D BUILDVER_MODE=MODE_DEBUG /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "PROFILE_95" /D "NOMINMAX" /D "COLLISION_EDITOR" /FD /c
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
# ADD LINK32 ole32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386
# Begin Target

# Name "CollisionEditor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\CollisionEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\CollisionEditor.rc
# ADD BASE RSC /l 0x409 /i "Source\Tools\CollisionEditor"
# ADD RSC /l 0x409 /i "Source\Tools\CollisionEditor" /i ".\Source\Tools\CollisionEditor"
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\CollisionEditorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\Database.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\EditMaterialDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\SampleListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\CollisionEditor.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\CollisionEditorDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\Database.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\EditMaterialDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\SampleListDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\res\CollisionEditor.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\CollisionEditor\res\CollisionEditor.rc2
# End Source File
# End Group
# End Target
# End Project
