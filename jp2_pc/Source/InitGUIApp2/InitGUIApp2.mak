# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=InitGUIApp2 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to InitGUIApp2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "InitGUIApp2 - Win32 Release" && "$(CFG)" !=\
 "InitGUIApp2 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "InitGUIApp2.mak" CFG="InitGUIApp2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "InitGUIApp2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "InitGUIApp2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "InitGUIApp2 - Win32 Debug"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "InitGUIApp2 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\InitGUIApp2.exe"

CLEAN : 
	-@erase "$(INTDIR)\InitGUIApp2.obj"
	-@erase "$(INTDIR)\InitGUIApp2.pch"
	-@erase "$(INTDIR)\InitGUIApp2.res"
	-@erase "$(INTDIR)\InitGUIApp2Dlg.obj"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\RegInit.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\InitGUIApp2.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/InitGUIApp2.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InitGUIApp2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/InitGUIApp2.pdb" /machine:I386 /out:"$(OUTDIR)/InitGUIApp2.exe"\
 
LINK32_OBJS= \
	"$(INTDIR)\InitGUIApp2.obj" \
	"$(INTDIR)\InitGUIApp2.res" \
	"$(INTDIR)\InitGUIApp2Dlg.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\RegInit.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\InitGUIApp2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "InitGUIApp2 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\InitGUIApp2.exe"

CLEAN : 
	-@erase "$(INTDIR)\InitGUIApp2.obj"
	-@erase "$(INTDIR)\InitGUIApp2.pch"
	-@erase "$(INTDIR)\InitGUIApp2.res"
	-@erase "$(INTDIR)\InitGUIApp2Dlg.obj"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\RegInit.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\InitGUIApp2.exe"
	-@erase "$(OUTDIR)\InitGUIApp2.ilk"
	-@erase "$(OUTDIR)\InitGUIApp2.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/InitGUIApp2.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InitGUIApp2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/InitGUIApp2.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/InitGUIApp2.exe" 
LINK32_OBJS= \
	"$(INTDIR)\InitGUIApp2.obj" \
	"$(INTDIR)\InitGUIApp2.res" \
	"$(INTDIR)\InitGUIApp2Dlg.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\RegInit.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\InitGUIApp2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "InitGUIApp2 - Win32 Release"
# Name "InitGUIApp2 - Win32 Debug"

!IF  "$(CFG)" == "InitGUIApp2 - Win32 Release"

!ELSEIF  "$(CFG)" == "InitGUIApp2 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\InitGUIApp2.cpp
DEP_CPP_INITG=\
	".\InitGUIApp2.h"\
	".\InitGUIApp2Dlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\InitGUIApp2.obj" : $(SOURCE) $(DEP_CPP_INITG) "$(INTDIR)"\
 "$(INTDIR)\InitGUIApp2.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\InitGUIApp2Dlg.cpp
DEP_CPP_INITGU=\
	"..\Lib\Sys\Reg.h"\
	"..\Lib\Sys\RegInit.hpp"\
	".\InitGUIApp2.h"\
	".\InitGUIApp2Dlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\InitGUIApp2Dlg.obj" : $(SOURCE) $(DEP_CPP_INITGU) "$(INTDIR)"\
 "$(INTDIR)\InitGUIApp2.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "InitGUIApp2 - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/InitGUIApp2.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InitGUIApp2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "InitGUIApp2 - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/InitGUIApp2.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InitGUIApp2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\InitGUIApp2.rc
DEP_RSC_INITGUI=\
	".\res\InitGUIApp2.ico"\
	".\res\InitGUIApp2.rc2"\
	

"$(INTDIR)\InitGUIApp2.res" : $(SOURCE) $(DEP_RSC_INITGUI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\Source\Lib\Sys\reg.cpp

"$(INTDIR)\reg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\InitGUIApp2.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\Source\Lib\Sys\RegInit.cpp
DEP_CPP_REGIN=\
	"..\Lib\Sys\Reg.h"\
	"..\Lib\Sys\RegInit.hpp"\
	

"$(INTDIR)\RegInit.obj" : $(SOURCE) $(DEP_CPP_REGIN) "$(INTDIR)"\
 "$(INTDIR)\InitGUIApp2.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
