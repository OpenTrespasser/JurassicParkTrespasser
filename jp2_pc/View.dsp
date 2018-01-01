# Microsoft Developer Studio Project File - Name="View" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=View - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "View.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "View.mak" CFG="View - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "View - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "View - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "View - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "View - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "View - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\View\Release"
# PROP BASE Intermediate_Dir ".\View\Release"
# PROP BASE Target_Dir ".\View"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\View"
# PROP Intermediate_Dir ".\Build\Release\View"
# PROP Target_Dir ".\View"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_RELEASE /FD /GM /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "View - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\View\Debug"
# PROP BASE Intermediate_Dir ".\View\Debug"
# PROP BASE Target_Dir ".\View"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\View"
# PROP Intermediate_Dir ".\Build\Debug\View"
# PROP Target_Dir ".\View"
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /GR /Zi /Od /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "_DEBUG" /D BUILDVER_MODE=MODE_DEBUG /D "WIN32" /D "_WINDOWS" /D "VC_EXTRALEAN" /D "_MBCS" /FD /GM /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "View - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\View\Final"
# PROP BASE Intermediate_Dir ".\View\Final"
# PROP BASE Target_Dir ".\View"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\View"
# PROP Intermediate_Dir ".\Build\Final\View"
# PROP Target_Dir ".\View"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D "NDEBUG" /D BUILDVER_MODE=MODE_OPTIMISE /D "WIN32" /D "_WINDOWS" /D "VC_EXTRALEAN" /c
# ADD CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /GM /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "View - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "View\Final P6"
# PROP BASE Intermediate_Dir "View\Final P6"
# PROP BASE Target_Dir "View"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\View"
# PROP Intermediate_Dir ".\Build\Final_P6\View"
# PROP Target_Dir "View"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /GM /c
# ADD CPP /nologo /G6 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "VC_EXTRALEAN" /D "_WINDOWS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /D TARGET_PROCESSOR=PROCESSOR_PENTIUMPRO /FD /GM /c
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

# Name "View - Win32 Release"
# Name "View - Win32 Debug"
# Name "View - Win32 Final"
# Name "View - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Lib\View\AGPTextureMemManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\Clut.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\Colour.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\ColourBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\W95\dd.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\W95\DDFont.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\W95\Direct3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\W95\Direct3DCards.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\W95\Direct3DQuery.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\Direct3DRenderState.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\Gamma.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\Grab.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\LineDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\Palette.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\PalLookup.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\Pixel.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\Raster.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\RasterComposite.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\RasterConvertD3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\RasterD3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\RasterFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\RasterPool.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\W95\RasterVid.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\W95\Video.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\Viewport.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\DirectX\ddraw.lib
# End Source File
# Begin Source File

SOURCE=.\lib\DirectX\dxguid.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\source\Lib\View\AGPTextureMemManager.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\Clut.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\Colour.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\ColourBase.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\lib\w95\dd.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\W95\DDFont.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\W95\Direct3D.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\W95\Direct3DQuery.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\Direct3DRenderState.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\Gamma.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\Grab.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\W95\Ispvr.h
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\LineDraw.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\Palette.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\pallookup.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\Pixel.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\Raster.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\RasterComposite.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\RasterConvertD3D.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\RasterD3D.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\RasterFile.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\RasterPool.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\View\RasterVid.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\View\Viewport.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
