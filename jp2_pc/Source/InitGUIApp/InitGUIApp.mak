# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=InitGUIApp - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to InitGUIApp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "InitGUIApp - Win32 Release" && "$(CFG)" !=\
 "InitGUIApp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "InitGUIApp.mak" CFG="InitGUIApp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "InitGUIApp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "InitGUIApp - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "InitGUIApp - Win32 Debug"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "InitGUIApp - Win32 Release"

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

ALL : "$(OUTDIR)\InitGUIApp.exe" "$(OUTDIR)\InitGUIApp.pch"

CLEAN : 
	-@erase "$(INTDIR)\DDDevice.obj"
	-@erase "$(INTDIR)\Direct3DQuery.obj"
	-@erase "$(INTDIR)\InitGUIApp.obj"
	-@erase "$(INTDIR)\InitGUIApp.pch"
	-@erase "$(INTDIR)\InitGUIApp.res"
	-@erase "$(INTDIR)\InitGUIAppDlg.obj"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\RegInit.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\InitGUIApp.exe"

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/InitGUIApp.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InitGUIApp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/InitGUIApp.pdb" /machine:I386 /out:"$(OUTDIR)/InitGUIApp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DDDevice.obj" \
	"$(INTDIR)\Direct3DQuery.obj" \
	"$(INTDIR)\InitGUIApp.obj" \
	"$(INTDIR)\InitGUIApp.res" \
	"$(INTDIR)\InitGUIAppDlg.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\RegInit.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"..\..\lib\DirectX\ddraw.lib" \
	"..\..\lib\DirectX\dxguid.lib"

"$(OUTDIR)\InitGUIApp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "InitGUIApp - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\InitGUIApp.exe" "$(OUTDIR)\InitGUIApp.pch"

CLEAN : 
	-@erase "$(INTDIR)\DDDevice.obj"
	-@erase "$(INTDIR)\Direct3DQuery.obj"
	-@erase "$(INTDIR)\InitGUIApp.obj"
	-@erase "$(INTDIR)\InitGUIApp.pch"
	-@erase "$(INTDIR)\InitGUIApp.res"
	-@erase "$(INTDIR)\InitGUIAppDlg.obj"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\RegInit.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\InitGUIApp.exe"
	-@erase "$(OUTDIR)\InitGUIApp.ilk"
	-@erase "$(OUTDIR)\InitGUIApp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/InitGUIApp.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InitGUIApp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/InitGUIApp.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/InitGUIApp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DDDevice.obj" \
	"$(INTDIR)\Direct3DQuery.obj" \
	"$(INTDIR)\InitGUIApp.obj" \
	"$(INTDIR)\InitGUIApp.res" \
	"$(INTDIR)\InitGUIAppDlg.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\RegInit.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"..\..\lib\DirectX\ddraw.lib" \
	"..\..\lib\DirectX\dxguid.lib"

"$(OUTDIR)\InitGUIApp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "InitGUIApp - Win32 Release"
# Name "InitGUIApp - Win32 Debug"

!IF  "$(CFG)" == "InitGUIApp - Win32 Release"

!ELSEIF  "$(CFG)" == "InitGUIApp - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\InitGUIApp.cpp
DEP_CPP_INITG=\
	".\InitGUIApp.h"\
	".\InitGUIAppDlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\InitGUIApp.obj" : $(SOURCE) $(DEP_CPP_INITG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\InitGUIAppDlg.cpp
DEP_CPP_INITGU=\
	"..\..\..\MSDEV\INCLUDE\d3dcaps.h"\
	"..\Lib\Sys\Reg.h"\
	"..\Lib\Sys\RegInit.hpp"\
	".\DDDevice.hpp"\
	".\InitGUIApp.h"\
	".\InitGUIAppDlg.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\Lib\W95\Direct3DQuery.hpp"\
	
NODEP_CPP_INITGU=\
	"..\..\..\MSDEV\INCLUDE\d3dcom.h"\
	"..\..\..\MSDEV\INCLUDE\subwtype.h"\
	

"$(INTDIR)\InitGUIAppDlg.obj" : $(SOURCE) $(DEP_CPP_INITGU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "InitGUIApp - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/InitGUIApp.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InitGUIApp.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "InitGUIApp - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/InitGUIApp.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InitGUIApp.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\InitGUIApp.rc
DEP_RSC_INITGUI=\
	".\res\InitGUIApp.ico"\
	".\res\InitGUIApp.rc2"\
	

"$(INTDIR)\InitGUIApp.res" : $(SOURCE) $(DEP_RSC_INITGUI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\DDDevice.cpp
DEP_CPP_DDDEV=\
	"..\..\..\MSDEV\INCLUDE\d3dcaps.h"\
	"..\..\Inc\DirectX\d3d.h"\
	"..\..\Inc\DirectX\d3dcaps.h"\
	"..\..\Inc\DirectX\d3dtypes.h"\
	"..\..\Inc\DirectX\d3dvec.inl"\
	"..\..\Inc\DirectX\ddraw.h"\
	"..\Lib\Sys\RegInit.hpp"\
	".\DDDevice.hpp"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\Lib\W95\Direct3DQuery.hpp"\
	
NODEP_CPP_DDDEV=\
	"..\..\..\MSDEV\INCLUDE\d3dcom.h"\
	"..\..\..\MSDEV\INCLUDE\subwtype.h"\
	"..\..\Inc\DirectX\d3dcom.h"\
	"..\..\Inc\DirectX\subwtype.h"\
	".\precomp.h"\
	

"$(INTDIR)\DDDevice.obj" : $(SOURCE) $(DEP_CPP_DDDEV) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\Source\Lib\Sys\RegInit.cpp
DEP_CPP_REGIN=\
	"..\Lib\Sys\Reg.h"\
	"..\Lib\Sys\RegInit.hpp"\
	{$(INCLUDE)}"\Lib\W95\Direct3DQuery.hpp"\
	

"$(INTDIR)\RegInit.obj" : $(SOURCE) $(DEP_CPP_REGIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\Source\Lib\Sys\reg.cpp

"$(INTDIR)\reg.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\lib\DirectX\ddraw.lib

!IF  "$(CFG)" == "InitGUIApp - Win32 Release"

!ELSEIF  "$(CFG)" == "InitGUIApp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\lib\DirectX\dxguid.lib

!IF  "$(CFG)" == "InitGUIApp - Win32 Release"

!ELSEIF  "$(CFG)" == "InitGUIApp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\Source\Lib\W95\Direct3DQuery.cpp
DEP_CPP_DIREC=\
	"..\Lib\Sys\Reg.h"\
	"..\Lib\Sys\RegInit.hpp"\
	{$(INCLUDE)}"\Lib\Std\PrivSelf.hpp"\
	{$(INCLUDE)}"\Lib\W95\Direct3DQuery.hpp"\
	

"$(INTDIR)\Direct3DQuery.obj" : $(SOURCE) $(DEP_CPP_DIREC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
