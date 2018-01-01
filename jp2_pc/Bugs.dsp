# Microsoft Developer Studio Project File - Name="Bugs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Bugs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Bugs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Bugs.mak" CFG="Bugs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Bugs - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP BASE Output_Dir ".\Bugs\Bugs___W"
# PROP BASE Intermediate_Dir ".\Bugs\Bugs___W"
# PROP BASE Target_Dir ".\Bugs"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Build\Debug\Bugs"
# PROP Intermediate_Dir ".\Build\Debug\Bugs"
# PROP Target_Dir ".\Bugs"
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /GR /Zi /Od /I "\jp2_pc\source" /I "\jp2_pc\source\gblinc" /I "\jp2_pc\inc" /I "\jp2_pc\inc\stl" /I "\jp2_pc\inc\directx" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# Begin Target

# Name "Bugs - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Bugs\BogusNestedType.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\BogusTemplateDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\BogusTemplateType.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\Complaints.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\DynamicCastConst.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\FriendOperator.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\FriendSpecialise.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\NoConvert.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\PartialSpecialise.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\TemplateAmbiguity.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\TemplateDefaultError.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\TemplateDefaultRedef.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bugs\TemplateNested.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
