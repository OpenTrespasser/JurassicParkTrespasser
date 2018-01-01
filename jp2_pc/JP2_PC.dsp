# Microsoft Developer Studio Project File - Name="AI Test" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AI Test - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "JP2_PC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "JP2_PC.mak" CFG="AI Test - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AI Test - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/JP2_PC", WUBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\AI Test\AI_Test_"
# PROP BASE Intermediate_Dir ".\AI Test\AI_Test_"
# PROP BASE Target_Dir ".\AI Test"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\Test"
# PROP Intermediate_Dir ".\Build\Debug\Test"
# PROP Target_Dir ".\AI Test"
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /GR /Zi /Od /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "_DEBUG" /D BUILDVER_MODE=MODE_DEBUG /D "WIN32" /D "_WINDOWS" /D "VC_EXTRALEAN" /D "VER_AI_TEST" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib odbc32.lib odbccp32.lib lz32.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "AI Test - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Test\Ai\AI.rc
# ADD BASE RSC /l 0x409 /i "Source\Test\Ai"
# ADD RSC /l 0x409 /i "Source\Test\Ai" /i ".\Source\Test\Ai"
# End Source File
# Begin Source File

SOURCE=.\Source\Test\Ai\AITest.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\Ai\AITestShell.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\Ai\AStarTest.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\AI\FakePhysics.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\AI\FakeShape.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GUIApp\PreDefinedShapes.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\Ai\TestAnimal.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\AI\TestBrains.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\Ai\UIModes.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\source\Test\AI\AITest.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Test\AI\AITestShell.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\AI\AStarTest.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\AI\FakePhysics.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\AI\FakeShape.hpp
# End Source File
# Begin Source File

SOURCE=.\source\GUIApp\PredefinedShapes.hpp
# End Source File
# Begin Source File

SOURCE=.\source\Test\AI\TestAnimal.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\AI\TestBrains.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\Test\AI\UIModes.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
