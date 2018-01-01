# Microsoft Developer Studio Project File - Name="Math" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Math - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Math.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Math.mak" CFG="Math - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Math - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Math - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Math\Release"
# PROP BASE Intermediate_Dir ".\Math\Release"
# PROP BASE Target_Dir ".\Math"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\Math"
# PROP Intermediate_Dir ".\Build\Release\Math"
# PROP Target_Dir ".\Math"
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

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Math\Debug"
# PROP BASE Intermediate_Dir ".\Math\Debug"
# PROP BASE Target_Dir ".\Math"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\Math"
# PROP Intermediate_Dir ".\Build\Debug\Math"
# PROP Target_Dir ".\Math"
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

!ELSEIF  "$(CFG)" == "Math - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Math\Final"
# PROP BASE Intermediate_Dir ".\Math\Final"
# PROP BASE Target_Dir ".\Math"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\Math"
# PROP Intermediate_Dir ".\Build\Final\Math"
# PROP Target_Dir ".\Math"
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

!ELSEIF  "$(CFG)" == "Math - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Math\Final P6"
# PROP BASE Intermediate_Dir "Math\Final P6"
# PROP BASE Target_Dir "Math"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\Math"
# PROP Intermediate_Dir ".\Build\Final_P6\Math"
# PROP Target_Dir "Math"
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

# Name "Math - Win32 Release"
# Name "Math - Win32 Debug"
# Name "Math - Win32 Final"
# Name "Math - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Lib\Math\FastInverse.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Math\FastSqrt.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Math\FastTrig.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\Presence.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\Rotate.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\source\Lib\Math\FastInverse.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Math\FastSqrt.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Math\FastTrig.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Math\FloatDef.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Math\FloatTable.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Math\MathUtil.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Transform\Matrix.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\Matrix2.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Transform\Presence.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Transform\Rotate.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\Scale.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\Shear.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\Transform.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\TransformIO.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\Translate.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\TransLinear.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\Vector.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Transform\VectorRange.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
