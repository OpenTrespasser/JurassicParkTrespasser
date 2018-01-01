# Microsoft Developer Studio Project File - Name="Render3D" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Render3D - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Render3D.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Render3D.mak" CFG="Render3D - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Render3D - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Render3D - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Render3D - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "Render3D - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Render3D - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Render3D\Release"
# PROP BASE Intermediate_Dir ".\Render3D\Release"
# PROP BASE Target_Dir ".\Render3D"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\Render3D"
# PROP Intermediate_Dir ".\Build\Release\Render3D"
# PROP Target_Dir ".\Render3D"
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

!ELSEIF  "$(CFG)" == "Render3D - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Render3D\Debug"
# PROP BASE Intermediate_Dir ".\Render3D\Debug"
# PROP BASE Target_Dir ".\Render3D"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\Render3D"
# PROP Intermediate_Dir ".\Build\Debug\Render3D"
# PROP Target_Dir ".\Render3D"
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

!ELSEIF  "$(CFG)" == "Render3D - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Render3D\Final"
# PROP BASE Intermediate_Dir ".\Render3D\Final"
# PROP BASE Target_Dir ".\Render3D"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\Render3D"
# PROP Intermediate_Dir ".\Build\Final\Render3D"
# PROP Target_Dir ".\Render3D"
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

!ELSEIF  "$(CFG)" == "Render3D - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Render3D\Final P6"
# PROP BASE Intermediate_Dir "Render3D\Final P6"
# PROP BASE Target_Dir "Render3D"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\Render3D"
# PROP Intermediate_Dir ".\Build\Final_P6\Render3D"
# PROP Target_Dir "Render3D"
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

# Name "Render3D - Win32 Release"
# Name "Render3D - Win32 Debug"
# Name "Render3D - Win32 Final"
# Name "Render3D - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Clip.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\ClipRegion2D.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\DepthSort.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\DepthSortTools.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Fog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Light.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\LightBlend.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Line.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\LineSide2D.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Material.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Overlay.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Particles.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\PipeLine.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\PipeLineHeap.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\PipeLineHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\ScreenPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\ScreenRender.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\ScreenRenderShadow.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Shadow.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Sky.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Texture.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\source\Lib\Renderer\Camera.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Clip.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\ClipRegion2D.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\DepthSort.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\DepthSortTools.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Fog.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\lib\renderer\light.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\LightBlend.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Line.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\LineSide2D.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Material.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Particles.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Pipeline.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\PipeLineHeap.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\PipeLineHelp.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\ScreenRender.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\ScreenRenderShadow.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Shadow.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Sky.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Texture.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
