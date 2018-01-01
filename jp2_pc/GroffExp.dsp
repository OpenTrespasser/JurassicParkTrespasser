# Microsoft Developer Studio Project File - Name="GroffExp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GroffExp - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GroffExp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GroffExp.mak" CFG="GroffExp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GroffExp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GroffExp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GroffExp - Win32 Final" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GroffExp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\GroffExp\Release"
# PROP BASE Intermediate_Dir ".\GroffExp\Release"
# PROP BASE Target_Dir ".\GroffExp"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\GroffExp\Release"
# PROP Intermediate_Dir ".\Build\GroffExp\Release"
# PROP Target_Dir ".\GroffExp"
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /I "\Jp2_pc\Source" /I "\Jp2_pc\Inc\MaxSDK" /I "\Jp2_pc\Source\Tools\GroffExp" /I "\Jp2_pc\Source\Lib\Sys" /I "\Jp2_pc\Source\Lib\Groff" /I "\Jp2_pc\Inc\Stl" /D "USE_MAX_TYPES" /D "_WINDOWS" /D "_MBCS" /D "PROFILE_95" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_RELEASE /FD /I /Jp2_pc/Source/Tools/GroffExp" /I /jp2_pc/Inc/MaxSDK" /I /jp2_pc/Source" /I /jp2_pc/Source/Tools/GroffExp" " " " " /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /out:".\Build\GroffExp\Release\GroffExp.dle"

!ELSEIF  "$(CFG)" == "GroffExp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\GroffExp\Debug"
# PROP BASE Intermediate_Dir ".\GroffExp\Debug"
# PROP BASE Target_Dir ".\GroffExp"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\GroffExp\Debug"
# PROP Intermediate_Dir ".\Build\GroffExp\Debug"
# PROP Target_Dir ".\GroffExp"
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /Zi /Od /I "\Jp2_pc\Source" /I "\Jp2_pc\Inc\MaxSDK" /I "\Jp2_pc\Source\Tools\GroffExp" /I "\Jp2_pc\Source\Lib\Sys" /I "\Jp2_pc\Source\Lib\Groff" /I "\Jp2_pc\Inc\Stl" /D "_DEBUG" /D "USE_MAX_TYPES" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DEBUG_MODE" /FD /I /Jp2_pc/Source/Tools/GroffExp" /I /jp2_pc/Inc/MaxSDK" /I /jp2_pc/Source" /I /jp2_pc/Source/Tools/GroffExp" " " " " /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Build\GroffExp\Debug\GroffExp.dle"

!ELSEIF  "$(CFG)" == "GroffExp - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\GroffExp\Final"
# PROP BASE Intermediate_Dir ".\GroffExp\Final"
# PROP BASE Target_Dir ".\GroffExp"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\GroffExp\Final"
# PROP Intermediate_Dir ".\Build\GroffExp\Final"
# PROP Target_Dir ".\GroffExp"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /O2 /I "\Jp2_pc\Source" /I "\Jp2_pc\Inc\MaxSDK" /I "\Jp2_pc\Source\Tools\GroffExp" /I "\Jp2_pc\Source\Lib\Sys" /I "\Jp2_pc\Source\Lib\Groff" /D "NDEBUG" /D "USE_MAX_TYPES" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "PROFILE_95" /I /Jp2_pc/Source/Tools/GroffExp" /I /jp2_pc/Inc/MaxSDK" /I /jp2_pc/Source" /I /jp2_pc/Source/Tools/GroffExp" " " " " /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /G5 /MD /W3 /GR /O2 /I "\Jp2_pc\Source" /I "\Jp2_pc\Inc\MaxSDK" /I "\Jp2_pc\Source\Tools\GroffExp" /I "\Jp2_pc\Source\Lib\Sys" /I "\Jp2_pc\Source\Lib\Groff" /I "\Jp2_pc\Inc\Stl" /D "USE_MAX_TYPES" /D "_WINDOWS" /D "_MBCS" /D "PROFILE_95" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /I /Jp2_pc/Source/Tools/GroffExp" /I /jp2_pc/Inc/MaxSDK" /I /jp2_pc/Source" /I /jp2_pc/Source/Tools/GroffExp" " " " " /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Build\Final\GroffExp/GroffExp.dle"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\Build\GroffExp\Final\GroffExp.dle"
# SUBTRACT LINK32 /profile /incremental:yes /debug

!ENDIF 

# Begin Target

# Name "GroffExp - Win32 Release"
# Name "GroffExp - Win32 Debug"
# Name "GroffExp - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\EasyString.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\Export.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\FileIO.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\Geometry.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\Groff.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\GroffExp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\GroffExp.def
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\GroffExp.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\GroffExp.rc
# ADD BASE RSC /l 0x409 /i "Source\Tools\GroffExp"
# ADD RSC /l 0x409 /i "Source\Tools\GroffExp" /i ".\Source\Tools\GroffExp"
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\GUIInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\Mathematics.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\ObjectDef.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\ObjectHandle.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\SmartBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\Symtab.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\SysLog.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\MaxSDK\BMM.LIB
# End Source File
# Begin Source File

SOURCE=.\lib\MaxSDK\CORE.LIB
# End Source File
# Begin Source File

SOURCE=.\lib\MaxSDK\GEOM.LIB
# End Source File
# Begin Source File

SOURCE=.\lib\MaxSDK\MESH.LIB
# End Source File
# Begin Source File

SOURCE=.\lib\MaxSDK\UTIL.LIB
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\Bitmap.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\EasyString.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\Export.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\FileIO.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\Geometry.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\Groff.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Tools\GroffExp\GUIInterface.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\Mathematics.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\GroffExp\ObjectDef.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\ObjectHandle.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\SmartBuffer.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\Symtab.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\SysLog.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
