# Microsoft Developer Studio Project File - Name="Game" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Game - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Game.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Game.mak" CFG="Game - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Game - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Game - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Game\Release"
# PROP BASE Intermediate_Dir ".\Game\Release"
# PROP BASE Target_Dir ".\Game"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\Game"
# PROP Intermediate_Dir ".\Build\Release\Game"
# PROP Target_Dir ".\Game"
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

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Game\Debug"
# PROP BASE Intermediate_Dir ".\Game\Debug"
# PROP BASE Target_Dir ".\Game"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\Game"
# PROP Intermediate_Dir ".\Build\Debug\Game"
# PROP Target_Dir ".\Game"
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

!ELSEIF  "$(CFG)" == "Game - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Game\Final"
# PROP BASE Intermediate_Dir ".\Game\Final"
# PROP BASE Target_Dir ".\Game"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\Game"
# PROP Intermediate_Dir ".\Build\Final\Game"
# PROP Target_Dir ".\Game"
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

!ELSEIF  "$(CFG)" == "Game - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Game\Final P6"
# PROP BASE Intermediate_Dir "Game\Final P6"
# PROP BASE Target_Dir "Game"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\Game"
# PROP Intermediate_Dir ".\Build\Final_P6\Game"
# PROP Target_Dir "Game"
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

# Name "Game - Win32 Release"
# Name "Game - Win32 Debug"
# Name "Game - Win32 Final"
# Name "Game - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Lib\Trigger\Action.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\DesignDaemon\BloodSplat.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\BooleanTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\CollisionTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\CreatureTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\DesignDaemon\Daemon.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\ExpressionEvaluate.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\GameActions.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\GameLoop.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\DesignDaemon\Gun.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\DesignDaemon\HitSpang.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\LocationTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\MagnetTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\MoreMassTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\ObjectTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\DesignDaemon\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\SequenceTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\DesignDaemon\Socket.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\StartTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\TextOverlay.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\TimerTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\TriggerBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\VariableTrigger.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Source\Lib\Trigger\Action.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\DesignDaemon\BloodSplat.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\DesignDaemon\Daemon.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\DesignDaemon\DaemonScript.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\ExpressionEvaluate.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\GameActions.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\GameLoop.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\DesignDaemon\Gun.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\DesignDaemon\HitSpang.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\DesignDaemon\Player.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\DesignDaemon\PlayerSettings.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\EntityDBase\TextOverlay.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Lib\Trigger\Trigger.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
