# Microsoft Developer Studio Project File - Name="ScreenRenderDWI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ScreenRenderDWI - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ScreenRenderDWI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ScreenRenderDWI.mak" CFG="ScreenRenderDWI - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ScreenRenderDWI - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ScreenRenderDWI - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ScreenRenderDWI - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "ScreenRenderDWI - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ScreenRenderDWI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\ScreenRenderDWI\Release"
# PROP BASE Intermediate_Dir ".\ScreenRenderDWI\Release"
# PROP BASE Target_Dir ".\ScreenRenderDWI"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\ScreenRenderDWI"
# PROP Intermediate_Dir ".\Build\Release\ScreenRenderDWI"
# PROP Target_Dir ".\ScreenRenderDWI"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_RELEASE /FD /GM /Zm200 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ScreenRenderDWI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\ScreenRenderDWI\Debug"
# PROP BASE Intermediate_Dir ".\ScreenRenderDWI\Debug"
# PROP BASE Target_Dir ".\ScreenRenderDWI"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\ScreenRenderDWI"
# PROP Intermediate_Dir ".\Build\Debug\ScreenRenderDWI"
# PROP Target_Dir ".\ScreenRenderDWI"
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /GR /Zi /Od /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "_DEBUG" /D BUILDVER_MODE=MODE_DEBUG /D "WIN32" /D "_WINDOWS" /D "VC_EXTRALEAN" /D "_MBCS" /FD /Zm200 /GM /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ScreenRenderDWI - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\ScreenRenderDWI\Final"
# PROP BASE Intermediate_Dir ".\ScreenRenderDWI\Final"
# PROP BASE Target_Dir ".\ScreenRenderDWI"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\ScreenRenderDWI"
# PROP Intermediate_Dir ".\Build\Final\ScreenRenderDWI"
# PROP Target_Dir ".\ScreenRenderDWI"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D "NDEBUG" /D BUILDVER_MODE=MODE_OPTIMISE /D "WIN32" /D "_WINDOWS" /D "VC_EXTRALEAN" /Fo"Build\Final\ScreenRenderDWI\Final/" /Fd"Build\ScreenRenderDWI/" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /GM /Zm200 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ScreenRenderDWI - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ScreenRenderDWI\Final P6"
# PROP BASE Intermediate_Dir "ScreenRenderDWI\Final P6"
# PROP BASE Target_Dir "ScreenRenderDWI"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\ScreenRenderDWI"
# PROP Intermediate_Dir ".\Build\Final_P6\ScreenRenderDWI"
# PROP Target_Dir "ScreenRenderDWI"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /GM /Zm200 /c
# ADD CPP /nologo /G6 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "VC_EXTRALEAN" /D "_WINDOWS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /D TARGET_PROCESSOR=PROCESSOR_PENTIUMPRO /FD /GM /Zm200 /c
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

# Name "ScreenRenderDWI - Win32 Release"
# Name "ScreenRenderDWI - Win32 Debug"
# Name "ScreenRenderDWI - Win32 Final"
# Name "ScreenRenderDWI - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\DrawSubTriangle.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\DrawTriangle.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\DrawTriangleEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\FastBump.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\FastBumpMath.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\FastBumpTable.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\RenderCache.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\RenderCacheHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\RenderCachePriv.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\ScreenRenderAuxD3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\ScreenRenderAuxD3DBatch.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\ScreenRenderDWI.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Source\gblinc\AsmSupport.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Primitives\DrawSubTriangle.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Primitives\DrawTriangle.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Primitives\FastBump.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\P6\FastBumpEx.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\FastBumpMath.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Primitives\FastBumpTable.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\GouraudT.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\RenderCache.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\RenderCacheHelp.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\RenderCachePriv.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\P6\ScanlineAsmMacros.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\ScreenRenderAuxD3D.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\ScreenRenderAuxD3DBatch.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\screenrenderdwi.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\Walk.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\P6\WalkEx.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
