# Microsoft Developer Studio Project File - Name="Physics" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Physics - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Physics.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Physics.mak" CFG="Physics - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Physics - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Physics - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Physics - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "Physics - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Physics - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Physics\Release"
# PROP BASE Intermediate_Dir ".\Physics\Release"
# PROP BASE Target_Dir ".\Physics"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\Physics"
# PROP Intermediate_Dir ".\Build\Release\Physics"
# PROP Target_Dir ".\Physics"
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

!ELSEIF  "$(CFG)" == "Physics - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Physics\Debug"
# PROP BASE Intermediate_Dir ".\Physics\Debug"
# PROP BASE Target_Dir ".\Physics"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\Physics"
# PROP Intermediate_Dir ".\Build\Debug\Physics"
# PROP Target_Dir ".\Physics"
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

!ELSEIF  "$(CFG)" == "Physics - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Physics\Final"
# PROP BASE Intermediate_Dir ".\Physics\Final"
# PROP BASE Target_Dir ".\Physics"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\Physics"
# PROP Intermediate_Dir ".\Build\Final\Physics"
# PROP Target_Dir ".\Physics"
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

!ELSEIF  "$(CFG)" == "Physics - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Physics\Final P6"
# PROP BASE Intermediate_Dir "Physics\Final P6"
# PROP BASE Target_Dir "Physics"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\Physics"
# PROP Intermediate_Dir ".\Build\Final_P6\Physics"
# PROP Target_Dir "Physics"
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

# Name "Physics - Win32 Release"
# Name "Physics - Win32 Debug"
# Name "Physics - Win32 Final"
# Name "Physics - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Lib\Physics\Arms.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\BioModel.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\dino_biped.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\dino_quad.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\foot.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\futil.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\Human.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\InfoBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\InfoCompound.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\InfoPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\InfoSkeleton.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\Magnet.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\Pelvis.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\PhysicsImport.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\PhysicsInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\PhysicsSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\WaterDisturbance.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\waves.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\Xob_bc.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\source\Lib\Physics\Arms.h
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\BioModel.h
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\BioStructure.h
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\dino_biped.h
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\futil.h
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\Human.h
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Physics\InfoBox.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Physics\InfoCompound.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Physics\InfoPlayer.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Physics\InfoSkeleton.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Physics\Magnet.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Physics\Pelvis.h
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\pelvis_def.h
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\PhysicsHelp.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Physics\PhysicsImport.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\PhysicsInfo.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\PhysicsStats.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Physics\PhysicsSystem.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Physics\WaterDisturbance.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Physics\Waves.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Physics\Xob_bc.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
