# Microsoft Developer Studio Project File - Name="GUIApp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GUIApp - Win32 Final P6
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GUIApp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GUIApp.mak" CFG="GUIApp - Win32 Final P6"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GUIApp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "GUIApp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GUIApp - Win32 Final" (based on "Win32 (x86) Application")
!MESSAGE "GUIApp - Win32 Final P6" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GUIApp - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\GUIApp\Debug"
# PROP BASE Intermediate_Dir ".\GUIApp\Debug"
# PROP BASE Target_Dir ".\GUIApp"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\GUIApp"
# PROP Intermediate_Dir ".\Build\Debug\GUIApp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\GUIApp"
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D BUILDVER_MODE=MODE_DEBUG /D "_AFXDLL" /D "_MBCS" /Fo"Build\Debug\GUInterface\Debug/" /Fd"Build\GUInterface/" /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /Zi /Od /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "_DEBUG" /D BUILDVER_MODE=MODE_DEBUG /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "PROFILE_95" /FD /GM /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /pdb:"Build\Debug\GUInterface\Debug/GUIApp.pdb" /debug /machine:I386 /nodefaultlib:"libcd" /out:"Build\GUInterface/GUIApp.exe" /verbose:lib
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ole32.lib winmm.lib lz32.lib version.lib /nologo /stack:0x100000 /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd" /verbose:lib /SECTION:SelfMod,ERW
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "GUIApp - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\GUIApp\Release"
# PROP BASE Intermediate_Dir ".\GUIApp\Release"
# PROP BASE Target_Dir ".\GUIApp"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Release\GUIApp"
# PROP Intermediate_Dir ".\Build\Release\GUIApp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\GUIApp"
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D BUILDVER_MODE=MODE_DEBUG /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /Gm /GR /Zi /O2 /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "PROFILE_95" /D "_AFXDLL" /D "_MBCS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_RELEASE /FD /GM /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ole32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd" /verbose:lib
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ole32.lib winmm.lib lz32.lib version.lib /nologo /stack:0x100000 /subsystem:windows /profile /debug /machine:I386 /nodefaultlib:"libc" /verbose:lib /SECTION:SelfMod,ERW

!ELSEIF  "$(CFG)" == "GUIApp - Win32 Final"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\GUIApp\Final"
# PROP BASE Intermediate_Dir ".\GUIApp\Final"
# PROP BASE Target_Dir ".\GUIApp"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Final\GUIApp"
# PROP Intermediate_Dir ".\Build\Final\GUIApp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\GUIApp"
# ADD BASE CPP /nologo /G5 /MD /W3 /Gm /GR /Zi /O2 /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D "NDEBUG" /D BUILDVER_MODE=MODE_OPTIMISE /D "WIN32" /D "_WINDOWS" /D "PROFILE_95" /D "_AFXDLL" /D "_MBCS" /c
# SUBTRACT BASE CPP /Fr /YX /Yc /Yu
# ADD CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "PROFILE_95" /D "_AFXDLL" /D "_MBCS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /GM /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ole32.lib winmm.lib /nologo /subsystem:windows /profile /debug /machine:I386 /nodefaultlib:"libc" /verbose:lib /SECTION:Self_Modifying,ew
# ADD LINK32 ole32.lib winmm.lib lz32.lib version.lib /nologo /stack:0x100000 /subsystem:windows /incremental:no /machine:I386 /nodefaultlib:"libc" /verbose:lib /SECTION:SelfMod,ERW
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "GUIApp - Win32 Final P6"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GUIApp\Final P6"
# PROP BASE Intermediate_Dir "GUIApp\Final P6"
# PROP BASE Target_Dir "GUIApp"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Final_P6\GUIApp"
# PROP Intermediate_Dir ".\Build\Final_P6\GUIApp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "GUIApp"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D BUILDVER_MODE=MODE_OPTIMISE /D "_WINDOWS" /D "PROFILE_95" /D "_AFXDLL" /D "_MBCS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /GM /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /G6 /MD /W3 /GR /O2 /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D BUILDVER_MODE=MODE_OPTIMISE /D "PROFILE_95" /D "_AFXDLL" /D "_MBCS" /D "_WINDOWS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /D TARGET_PROCESSOR=PROCESSOR_PENTIUMPRO /FD /GM /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ole32.lib winmm.lib lz32.lib version.lib /nologo /stack:0x100000 /subsystem:windows /incremental:no /machine:I386 /nodefaultlib:"libc" /verbose:lib
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ole32.lib winmm.lib lz32.lib version.lib /nologo /stack:0x100000 /subsystem:windows /incremental:no /machine:I386 /nodefaultlib:"libc" /verbose:lib /SECTION:SelfMod,ERW
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "GUIApp - Win32 Debug"
# Name "GUIApp - Win32 Release"
# Name "GUIApp - Win32 Final"
# Name "GUIApp - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=".\Source\GUIApp\AI Dialogs2Dlg.cpp"
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\Background.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\CameraProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogAlphaColour.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogBumpPack.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogCulling.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogDepthSort.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogFog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogGamma.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogGore.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogGun.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogMagnet.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogMemLog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogMipmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogOcclusion.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogPartition.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogPhysics.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogRenderCache.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogScheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogSky.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogSoundMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogString.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogTeleport.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogTerrain.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogTexturePack.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogTexturePackOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogVM.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogWater.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\FindDuplicates.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\GDIBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\GUIApp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\GUIApp.rc
# ADD BASE RSC /l 0x409 /i "Source\GUIApp"
# ADD RSC /l 0x409 /i "Source\GUIApp" /i ".\Source\GUIApp"
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\GUIAppDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\GUIControls.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\GUIPipeline.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\GUITools.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\LightProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\Options.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\ParameterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\PerspectiveSubdivideDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\QualityDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\SoundPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\StdAfx.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\TerrainTest.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\Toolbar.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=".\Source\GUIApp\AI Dialogs2Dlg.h"
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\Background.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\CameraProperties.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogAlphaColour.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogBumpPack.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogCulling.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogDepthSort.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogFog.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogGamma.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogGore.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogGun.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogMagnet.h
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogMaterial.h
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogMemLog.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogMipmap.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogObject.h
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogOcclusion.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogPartition.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogPhysics.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogPlayer.h
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogRenderCache.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogScheduler.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogSky.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogSoundMaterial.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogString.hpp
# End Source File
# Begin Source File

SOURCE=.\DialogTeleport.h
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogTeleport.h
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogTerrain.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogTexturePack.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogTexturePackOptions.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogVM.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\DialogWater.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\GDIBitmap.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\GUIApp.h
# End Source File
# Begin Source File

SOURCE=.\source\GUIApp\GUIAppDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\GUIControls.hpp
# End Source File
# Begin Source File

SOURCE=.\source\GUIApp\GUIPipeLine.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\GUITools.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\LightProperties.hpp
# End Source File
# Begin Source File

SOURCE=.\source\GUIApp\ParameterDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\PerspectiveSubdivideDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\QualityDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\SoundPropertiesDlg.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\TerrainTest.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\Toolbar.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Source\GUIApp\res\GUIApp.ico
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\res\GUIApp.rc2
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\res\icon_rap.ico
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\res\Palette.bmp
# End Source File
# End Group
# End Target
# End Project
