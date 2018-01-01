# Microsoft Developer Studio Project File - Name="EntityDBase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=EntityDBase - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EntityDBase.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EntityDBase.mak" CFG="EntityDBase - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EntityDBase - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "EntityDBase - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "EntityDBase - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "EntityDBase - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EntityDBase - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\EntityDBase\Release"
# PROP BASE Intermediate_Dir ".\EntityDBase\Release"
# PROP BASE Target_Dir ".\EntityDBase"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\EntityDBase"
# PROP Intermediate_Dir ".\Build\Release\EntityDBase"
# PROP Target_Dir ".\EntityDBase"
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

!ELSEIF  "$(CFG)" == "EntityDBase - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\EntityDBase\Debug"
# PROP BASE Intermediate_Dir ".\EntityDBase\Debug"
# PROP BASE Target_Dir ".\EntityDBase"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\EntityDBase"
# PROP Intermediate_Dir ".\Build\Debug\EntityDBase"
# PROP Target_Dir ".\EntityDBase"
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

!ELSEIF  "$(CFG)" == "EntityDBase - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\EntityDBase\Final"
# PROP BASE Intermediate_Dir ".\EntityDBase\Final"
# PROP BASE Target_Dir ".\EntityDBase"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\EntityDBase"
# PROP Intermediate_Dir ".\Build\Final\EntityDBase"
# PROP Target_Dir ".\EntityDBase"
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

!ELSEIF  "$(CFG)" == "EntityDBase - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "EntityDBase\Final P6"
# PROP BASE Intermediate_Dir "EntityDBase\Final P6"
# PROP BASE Target_Dir "EntityDBase"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\EntityDBase"
# PROP Intermediate_Dir ".\Build\Final_P6\EntityDBase"
# PROP Target_Dir "EntityDBase"
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

# Name "EntityDBase - Win32 Release"
# Name "EntityDBase - Win32 Debug"
# Name "EntityDBase - Win32 Final"
# Name "EntityDBase - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Animal.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Animate.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\AnimationScript.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\CameraPrime.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Entity.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\EntityLight.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\FrameHeap.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Instance.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Message.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageLog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MovementPrediction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\ParticleGen.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query\QSubsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query\QTerrain.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\QualitySettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\QueueMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\RenderDB.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Replay.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Teleport.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Water.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\WorldDBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\WorldPort.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\Animal.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\Animate.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\AnimationScript.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\CameraPrime.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\Entity.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\EntityLight.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\FrameHeap.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\Instance.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\Message.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\MessageLog.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\MovementPrediction.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgAudio.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgCollision.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgControl.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgCreate.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgDelete.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgMagnet.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgMove.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgPaint.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgPhysicsReq.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgStep.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgSystem.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\MsgTrigger.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\ParticleGen.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query\QAI.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query\QMessage.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query\QPhysics.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query\QRenderer.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query\QSolidObject.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\Query\QSubsystem.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query\QTerrain.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query\QTerrainObj.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query\QTriggers.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\QualitySettings.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\Query.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\QueueMessage.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\Query\QWater.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\MessageTypes\RegisteredMsg.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\RenderDB.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\Replay.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\Teleport.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\Water.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Lib\EntityDBase\WorldDBase.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
