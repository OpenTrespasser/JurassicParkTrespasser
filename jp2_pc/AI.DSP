# Microsoft Developer Studio Project File - Name="AI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AI - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AI.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AI.MAK" CFG="AI - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AI - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AI - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AI - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "AI - Win32 Final P6" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\AI\Release"
# PROP BASE Intermediate_Dir ".\AI\Release"
# PROP BASE Target_Dir ".\AI"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Release\AI"
# PROP Intermediate_Dir ".\Build\Release\AI"
# PROP Target_Dir ".\AI"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GR /Zi /O2 /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D BUILDVER_MODE=MODE_OPTIMISE /D "_AFXDLL" /D "_WINDOWS" /D "_MBCS" /D "PROFILE_95" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_RELEASE /FD /GM /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\AI\AI___Win"
# PROP BASE Intermediate_Dir ".\AI\AI___Win"
# PROP BASE Target_Dir ".\AI"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\AI"
# PROP Intermediate_Dir ".\Build\Debug\AI"
# PROP Target_Dir ".\AI"
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /GR /Z7 /Od /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "_DEBUG" /D BUILDVER_MODE=MODE_DEBUG /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "PROFILE_95" /FD /GM /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AI - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\AI\Final"
# PROP BASE Intermediate_Dir ".\AI\Final"
# PROP BASE Target_Dir ".\AI"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final\AI"
# PROP Intermediate_Dir ".\Build\Final\AI"
# PROP Target_Dir ".\AI"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /D "NDEBUG" /D BUILDVER_MODE=MODE_OPTIMISE /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "PROFILE_95" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D BUILDVER_MODE=MODE_OPTIMISE /D "_AFXDLL" /D "_WINDOWS" /D "_MBCS" /D "PROFILE_95" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /GM /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AI - Win32 Final P6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AI\Final P6"
# PROP BASE Intermediate_Dir "AI\Final P6"
# PROP BASE Target_Dir "AI"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Build\Final_P6\AI"
# PROP Intermediate_Dir ".\Build\Final_P6\AI"
# PROP Target_Dir "AI"
# ADD BASE CPP /nologo /G5 /MD /W3 /GR /O2 /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D BUILDVER_MODE=MODE_OPTIMISE /D "_AFXDLL" /D "_WINDOWS" /D "_MBCS" /D "PROFILE_95" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /FD /GM /c
# ADD CPP /nologo /G6 /MD /W3 /GR /O2 /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D BUILDVER_MODE=MODE_OPTIMISE /D "_AFXDLL" /D "_MBCS" /D "PROFILE_95" /D "_WINDOWS" /D "NDEBUG" /D "WIN32" /D BUILDVER_MODE=MODE_FINAL /D TARGET_PROCESSOR=PROCESSOR_PENTIUMPRO /FD /GM /c
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

# Name "AI - Win32 Release"
# Name "AI - Win32 Debug"
# Name "AI - Win32 Final"
# Name "AI - Win32 Final P6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Game\Ai\Activity.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\ActivityAttack.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\ActivityCompound.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\ActivityPhased.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\ActivityVocal.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\AIGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\AIMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\Brain.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\EmotionActivities.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\HeadActivities.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\Influence.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\MentalState.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\MoveActivities.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\NodeHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\NodeSource.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\PathAStar.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\PathAvoider.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\Pathfinder.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\Port.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\Silhouette.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\Synthesizer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\TestActivities.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\WorldView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\source\Game\AI\Activity.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\AI\ActivityAttack.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\AI\ActivityCompound.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\ActivityPhased.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\AI\ActivityVocal.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\AI\AIGraph.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\AIInfo.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\AIMain.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\AITypes.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\AStar.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\AI\Brain.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\AI\EmotionActivities.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\AI\HeadActivities.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\Influence.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\AI\MentalState.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\AI\MoveActivities.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\NodeHistory.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\NodeSource.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\PathAStar.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\PathAvoider.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\Pathfinder.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\Port.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\AI\Silhouette.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\AI\Synthesizer.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Game\AI\TestActivities.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Game\Ai\WorldView.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
