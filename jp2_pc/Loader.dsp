# Microsoft Developer Studio Project File - Name="Loader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Loader - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Loader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Loader.mak" CFG="Loader - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Loader - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Loader - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Loader - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "Loader - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Loader - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Loader\Release"
# PROP BASE Intermediate_Dir ".\Loader\Release"
# PROP BASE Target_Dir ".\Loader"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\Loader"
# PROP Intermediate_Dir ".\Build\Release\Loader"
# PROP Target_Dir ".\Loader"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_RELEASE /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Loader\Debug"
# PROP BASE Intermediate_Dir ".\Loader\Debug"
# PROP BASE Target_Dir ".\Loader"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\Loader"
# PROP Intermediate_Dir ".\Build\Debug\Loader"
# PROP Target_Dir ".\Loader"
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /GR /Zi /Od /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "_DEBUG" /D BUILDVER_MODE=MODE_DEBUG /D "WIN32" /D "_WINDOWS" /D "VC_EXTRALEAN" /D "_MBCS" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Loader - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Loader\Final"
# PROP BASE Intermediate_Dir ".\Loader\Final"
# PROP BASE Target_Dir ".\Loader"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\Loader"
# PROP Intermediate_Dir ".\Build\Final\Loader"
# PROP Target_Dir ".\Loader"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D "NDEBUG" /D BUILDVER_MODE=MODE_OPTIMISE /D "WIN32" /D "_WINDOWS" /D "VC_EXTRALEAN" /c
# ADD CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Loader - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Loader\Final P6"
# PROP BASE Intermediate_Dir "Loader\Final P6"
# PROP BASE Target_Dir "Loader"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\Loader"
# PROP Intermediate_Dir ".\Build\Final_P6\Loader"
# PROP Target_Dir "Loader"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "VC_EXTRALEAN" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /O2 /I "\jp2_pc\inc\directx" /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D BUILDVER_MODE=MODE_OPTIMISE /D "VC_EXTRALEAN" /D "_WINDOWS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /D TARGET_PROCESSOR=PROCESSOR_PENTIUMPRO /FD /c
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

# Name "Loader - Win32 Release"
# Name "Loader - Win32 Debug"
# Name "Loader - Win32 Final"
# Name "Loader - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Lib\Loader\AsyncLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Loader\DataDaemon.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\EasyString.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\FileIO.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\Groff.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\GroffIO.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\GroffLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Loader\ImageLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Loader\Loader.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Loader\LoadTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\ObjectHandle.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Loader\PlatonicInstance.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Loader\PVA.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Loader\SaveFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\SmartBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\SymbolTable.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\Symtab.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Sys\SysLog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Loader\TextureManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Loader\TexturePackSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Groff\ValueTable.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\source\Lib\Loader\ASyncLoader.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Loader\DataDaemon.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\EasyString.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Loader\Fetchable.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\FileIO.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\Groff.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\GroffIO.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\GroffLoader.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Loader\ImageLoader.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Loader\Loader.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\lib\loader\loadtexture.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\ObjectHandle.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Loader\PlatonicInstance.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Loader\PVA.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Loader\SaveFile.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\SmartBuffer.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\SymbolTable.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\Symtab.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Sys\SysLog.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Loader\TextureManager.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Loader\TexturePackSurface.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\Groff\ValueTable.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
