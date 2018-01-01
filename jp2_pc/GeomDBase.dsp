# Microsoft Developer Studio Project File - Name="GeomDBase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GeomDBase - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GeomDBase.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GeomDBase.mak" CFG="GeomDBase - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GeomDBase - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GeomDBase - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "GeomDBase - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "GeomDBase - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GeomDBase - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\GeomDBase\Release"
# PROP BASE Intermediate_Dir ".\GeomDBase\Release"
# PROP BASE Target_Dir ".\GeomDBase"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\GeomDBase"
# PROP Intermediate_Dir ".\Build\Release\GeomDBase"
# PROP Target_Dir ".\GeomDBase"
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

!ELSEIF  "$(CFG)" == "GeomDBase - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\GeomDBase\Debug"
# PROP BASE Intermediate_Dir ".\GeomDBase\Debug"
# PROP BASE Target_Dir ".\GeomDBase"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\GeomDBase"
# PROP Intermediate_Dir ".\Build\Debug\GeomDBase"
# PROP Target_Dir ".\GeomDBase"
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

!ELSEIF  "$(CFG)" == "GeomDBase - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\GeomDBase\Final"
# PROP BASE Intermediate_Dir ".\GeomDBase\Final"
# PROP BASE Target_Dir ".\GeomDBase"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\GeomDBase"
# PROP Intermediate_Dir ".\Build\Final\GeomDBase"
# PROP Target_Dir ".\GeomDBase"
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

!ELSEIF  "$(CFG)" == "GeomDBase - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GeomDBase\Final P6"
# PROP BASE Intermediate_Dir "GeomDBase\Final P6"
# PROP BASE Target_Dir "GeomDBase"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\GeomDBase"
# PROP Intermediate_Dir ".\Build\Final_P6\GeomDBase"
# PROP Target_Dir "GeomDBase"
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

# Name "GeomDBase - Win32 Release"
# Name "GeomDBase - Win32 Debug"
# Name "GeomDBase - Win32 Final"
# Name "GeomDBase - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\ClipMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\GeomTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\GeomTypesBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\GeomTypesCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\GeomTypesPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\GeomTypesPoly.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\GeomTypesSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\Hull.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\Mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Occlude.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\Partition.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\PartitionBuild.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\PartitionPriv.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\PartitionSpace.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\PlaneDef.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\PreDefinedShapes.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\RayCast.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\RenderType.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\Shape.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\Skeleton.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\Terrain.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\TerrainLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\TerrainObj.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\TerrainTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\TexturePageManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\WaterQuadTree.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\WaveletConv.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\WaveletDataFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\WaveletQuadTree.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\WaveletQuadTreeBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\WaveletQuadTreeBaseRecalc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\WaveletQuadTreeBaseTri.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\WaveletQuadTreeQuery.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\WaveletQuadTreeTForm.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\WaveletStaticData.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\ClipMesh.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Container.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\GeomTypes.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Renderer\Primitives\IndexT.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\Mesh.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\MeshIterator.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\Occlude.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\Partition.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\PartitionBuild.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\PartitionPriv.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\PartitionSpace.hpp
# End Source File
# Begin Source File

SOURCE=.\source\GUIApp\PredefinedShapes.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\RayCast.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Renderer\RenderType.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\Shape.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\Skeleton.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\GeomDBase\SkeletonIterator.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\Terrain.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\TerrainLoad.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\TerrainObj.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\TerrainTexture.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\TexturePageManager.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\WaterQuadTree.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\WaveletConv.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\WaveletDataFormat.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\WaveletQuadTree.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\WaveletQuadTreeBase.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\WaveletQuadTreeBaseRecalc.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\WaveletQuadTreeBaseTri.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\WaveletQuadTreeQuery.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\WaveletQuadTreeTForm.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\GeomDBase\WaveletStaticData.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
