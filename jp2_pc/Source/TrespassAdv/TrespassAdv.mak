# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=TrespassAdv - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to TrespassAdv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "TrespassAdv - Win32 Release" && "$(CFG)" !=\
 "TrespassAdv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "TrespassAdv.mak" CFG="TrespassAdv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TrespassAdv - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TrespassAdv - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "TrespassAdv - Win32 Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "TrespassAdv - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\TweakTrespass.exe" "$(OUTDIR)\TrespassAdv.pch"

CLEAN : 
	-@erase "$(INTDIR)\DDDevice.obj"
	-@erase "$(INTDIR)\DialogName.obj"
	-@erase "$(INTDIR)\Direct3DQuery.obj"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\RegInit.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TrespassAdv.obj"
	-@erase "$(INTDIR)\TrespassAdv.pch"
	-@erase "$(INTDIR)\TrespassAdv.res"
	-@erase "$(INTDIR)\TrespassAdvDlg.obj"
	-@erase "$(OUTDIR)\TweakTrespass.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/TrespassAdv.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/TrespassAdv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /pdb:"Release/TrespassAdv.pdb" /machine:I386 /out:"Release/TweakTrespass.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/TrespassAdv.pdb" /machine:I386\
 /out:"$(OUTDIR)/TweakTrespass.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DDDevice.obj" \
	"$(INTDIR)\DialogName.obj" \
	"$(INTDIR)\Direct3DQuery.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\RegInit.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TrespassAdv.obj" \
	"$(INTDIR)\TrespassAdv.res" \
	"$(INTDIR)\TrespassAdvDlg.obj" \
	"..\..\lib\DirectX\ddraw.lib" \
	"..\..\lib\DirectX\dxguid.lib"

"$(OUTDIR)\TweakTrespass.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TrespassAdv - Win32 Debug"

# PROP BASE Use_MFC 5
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

ALL : "$(OUTDIR)\TweakTrespass.exe" "$(OUTDIR)\TrespassAdv.pch"

CLEAN : 
	-@erase "$(INTDIR)\DDDevice.obj"
	-@erase "$(INTDIR)\DialogName.obj"
	-@erase "$(INTDIR)\Direct3DQuery.obj"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\RegInit.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TrespassAdv.obj"
	-@erase "$(INTDIR)\TrespassAdv.pch"
	-@erase "$(INTDIR)\TrespassAdv.res"
	-@erase "$(INTDIR)\TrespassAdvDlg.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\TrespassAdv.pdb"
	-@erase "$(OUTDIR)\TweakTrespass.exe"
	-@erase "$(OUTDIR)\TweakTrespass.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/TrespassAdv.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/TrespassAdv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /pdb:"Debug/TrespassAdv.pdb" /debug /machine:I386 /out:"Debug/TweakTrespass.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/TrespassAdv.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/TweakTrespass.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DDDevice.obj" \
	"$(INTDIR)\DialogName.obj" \
	"$(INTDIR)\Direct3DQuery.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\RegInit.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TrespassAdv.obj" \
	"$(INTDIR)\TrespassAdv.res" \
	"$(INTDIR)\TrespassAdvDlg.obj" \
	"..\..\lib\DirectX\ddraw.lib" \
	"..\..\lib\DirectX\dxguid.lib"

"$(OUTDIR)\TweakTrespass.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "TrespassAdv - Win32 Release"
# Name "TrespassAdv - Win32 Debug"

!IF  "$(CFG)" == "TrespassAdv - Win32 Release"

!ELSEIF  "$(CFG)" == "TrespassAdv - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "TrespassAdv - Win32 Release"

!ELSEIF  "$(CFG)" == "TrespassAdv - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TrespassAdv.cpp
DEP_CPP_TRESP=\
	".\StdAfx.h"\
	".\TrespassAdv.h"\
	".\TrespassAdvDlg.h"\
	

"$(INTDIR)\TrespassAdv.obj" : $(SOURCE) $(DEP_CPP_TRESP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TrespassAdvDlg.cpp
DEP_CPP_TRESPA=\
	"..\..\..\MSDEV\INCLUDE\d3dcaps.h"\
	"..\..\..\MSDEV\INCLUDE\d3dtypes.h"\
	".\DialogName.hpp"\
	".\StdAfx.h"\
	".\TrespassAdv.h"\
	".\TrespassAdvDlg.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\Lib\Sys\reg.h"\
	{$(INCLUDE)}"\Lib\Sys\RegInit.hpp"\
	{$(INCLUDE)}"\Lib\W95\Direct3DQuery.hpp"\
	{$(INCLUDE)}"\Trespass\DDDevice.hpp"\
	
NODEP_CPP_TRESPA=\
	"..\..\..\MSDEV\INCLUDE\d3dcom.h"\
	"..\..\..\MSDEV\INCLUDE\subwtype.h"\
	

"$(INTDIR)\TrespassAdvDlg.obj" : $(SOURCE) $(DEP_CPP_TRESPA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "TrespassAdv - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /Fp"$(INTDIR)/TrespassAdv.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\TrespassAdv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "TrespassAdv - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/TrespassAdv.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\TrespassAdv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TrespassAdv.rc
DEP_RSC_TRESPAS=\
	".\res\TrespassAdv.ico"\
	".\res\TrespassAdv.rc2"\
	

"$(INTDIR)\TrespassAdv.res" : $(SOURCE) $(DEP_RSC_TRESPAS) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\Source\Lib\W95\Direct3DQuery.cpp
DEP_CPP_DIREC=\
	{$(INCLUDE)}"\Lib\Std\PrivSelf.hpp"\
	{$(INCLUDE)}"\Lib\Sys\reg.h"\
	{$(INCLUDE)}"\Lib\Sys\RegInit.hpp"\
	{$(INCLUDE)}"\Lib\W95\Direct3DQuery.hpp"\
	

"$(INTDIR)\Direct3DQuery.obj" : $(SOURCE) $(DEP_CPP_DIREC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\Source\Lib\Sys\RegInit.cpp
DEP_CPP_REGIN=\
	"..\lib\w95\wininclude.hpp"\
	{$(INCLUDE)}"\Lib\Sys\reg.h"\
	{$(INCLUDE)}"\Lib\Sys\RegInit.hpp"\
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

SOURCE=.\DialogName.cpp
DEP_CPP_DIALO=\
	".\DialogName.hpp"\
	".\StdAfx.h"\
	".\TrespassAdv.h"\
	{$(INCLUDE)}"\Lib\W95\Direct3DQuery.hpp"\
	

"$(INTDIR)\DialogName.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\Source\Trespass\DDDevice.cpp

!IF  "$(CFG)" == "TrespassAdv - Win32 Release"

DEP_CPP_DDDEV=\
	"..\..\..\MSDEV\INCLUDE\d3dcaps.h"\
	"..\..\..\MSDEV\INCLUDE\d3dtypes.h"\
	"..\..\Inc\DirectX\d3d.h"\
	"..\..\Inc\DirectX\d3dcaps.h"\
	"..\..\Inc\DirectX\d3dtypes.h"\
	"..\..\Inc\DirectX\d3dvec.inl"\
	"..\..\Inc\DirectX\ddraw.h"\
	"..\Game\AI\AIMain.hpp"\
	"..\Game\AI\Classes.hpp"\
	"..\Game\AI\Feeling.hpp"\
	"..\Game\AI\Rating.hpp"\
	"..\Game\DesignDaemon\Player.hpp"\
	"..\gblinc\common.hpp"\
	"..\Lib\Audio\Audio.hpp"\
	"..\Lib\Audio\AudioDaemon.hpp"\
	"..\Lib\Audio\AudioLoader.hpp"\
	"..\Lib\Audio\Ia3d.h"\
	"..\Lib\Audio\Material.hpp"\
	"..\Lib\Audio\SoundTypes.hpp"\
	"..\Lib\Audio\Subtitle.hpp"\
	"..\Lib\Control\Control.hpp"\
	"..\Lib\EntityDBase\EntityLight.hpp"\
	"..\Lib\EntityDBase\FrameHeap.hpp"\
	"..\Lib\EntityDBase\GameLoop.hpp"\
	"..\Lib\EntityDBase\MessageTypes\MsgControl.hpp"\
	"..\Lib\EntityDBase\MessageTypes\MsgSystem.hpp"\
	"..\Lib\EntityDBase\Query\QRenderer.hpp"\
	"..\Lib\EntityDBase\WorldDBase.hpp"\
	"..\Lib\GeomDBase\PartitionPriv.hpp"\
	"..\Lib\GeomDBase\Shape.hpp"\
	"..\Lib\Loader\DataDaemon.hpp"\
	"..\lib\loader\loader.hpp"\
	"..\lib\loader\loadtexture.hpp"\
	"..\Lib\Physics\PhysicsSystem.hpp"\
	"..\Lib\Renderer\Camera.hpp"\
	"..\lib\renderer\light.hpp"\
	"..\Lib\Renderer\LightBlend.hpp"\
	"..\Lib\Renderer\PipeLine.hpp"\
	"..\Lib\Renderer\RenderDefs.hpp"\
	"..\lib\renderer\Shadow.hpp"\
	"..\Lib\Std\Mem.hpp"\
	"..\lib\std\memlimits.hpp"\
	"..\Lib\Sys\ConIO.hpp"\
	"..\lib\sys\debugconsole.hpp"\
	"..\lib\sys\exepagemodify.hpp"\
	"..\lib\sys\performancecount.hpp"\
	"..\lib\sys\Processor.hpp"\
	"..\lib\sys\ProcessorDetect.hpp"\
	"..\lib\sys\w95\render.hpp"\
	"..\Lib\Transform\Matrix.hpp"\
	"..\Lib\Transform\Scale.hpp"\
	"..\Lib\Transform\Shear.hpp"\
	"..\Lib\Transform\Translate.hpp"\
	"..\Lib\View\Gamma.hpp"\
	"..\lib\view\rastervid.hpp"\
	"..\lib\view\video.hpp"\
	"..\lib\w95\dd.hpp"\
	"..\Lib\W95\Direct3D.hpp"\
	"..\lib\w95\wininclude.hpp"\
	"..\shell\winrendertools.hpp"\
	"..\Trespass\precomp.h"\
	"..\Trespass\supportfn.hpp"\
	{$(INCLUDE)}"\algobase.h"\
	{$(INCLUDE)}"\bool.h"\
	{$(INCLUDE)}"\bstring.h"\
	{$(INCLUDE)}"\Config.hpp"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\defalloc.h"\
	{$(INCLUDE)}"\deque.h"\
	{$(INCLUDE)}"\function.h"\
	{$(INCLUDE)}"\Game\AI\AITypes.hpp"\
	{$(INCLUDE)}"\GblInc\BuildVer.hpp"\
	{$(INCLUDE)}"\GblInc\Warnings.hpp"\
	{$(INCLUDE)}"\heap.h"\
	{$(INCLUDE)}"\iterator.h"\
	{$(INCLUDE)}"\Lib\Audio\SoundDefs.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Animate.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Container.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Entity.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Instance.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Message.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\MessageTypes\MsgAudio.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\MessageTypes\MsgStep.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Query.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\QueueMessage.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Subsystem.hpp"\
	{$(INCLUDE)}"\Lib\GeomDBase\Mesh.hpp"\
	{$(INCLUDE)}"\Lib\GeomDBase\Partition.hpp"\
	{$(INCLUDE)}"\Lib\GeomDBase\PartitionSpace.hpp"\
	{$(INCLUDE)}"\Lib\GeomDBase\Plane.hpp"\
	{$(INCLUDE)}"\Lib\Groff\EasyString.hpp"\
	{$(INCLUDE)}"\Lib\Groff\FileIO.hpp"\
	{$(INCLUDE)}"\Lib\Groff\Groff.hpp"\
	{$(INCLUDE)}"\Lib\Groff\GroffIO.hpp"\
	{$(INCLUDE)}"\Lib\Groff\GroffLoader.hpp"\
	{$(INCLUDE)}"\Lib\Groff\ObjectHandle.hpp"\
	{$(INCLUDE)}"\Lib\Groff\SymbolTable.hpp"\
	{$(INCLUDE)}"\Lib\Groff\ValueTable.hpp"\
	{$(INCLUDE)}"\Lib\Loader\Fetchable.hpp"\
	{$(INCLUDE)}"\Lib\Math\FastSqrt.hpp"\
	{$(INCLUDE)}"\Lib\Math\FastTrig.hpp"\
	{$(INCLUDE)}"\Lib\Math\FloatDef.hpp"\
	{$(INCLUDE)}"\Lib\Math\FloatTable.hpp"\
	{$(INCLUDE)}"\Lib\Renderer\GeomTypes.hpp"\
	{$(INCLUDE)}"\Lib\Renderer\Material.hpp"\
	{$(INCLUDE)}"\Lib\Renderer\Primitives\FastBump.hpp"\
	{$(INCLUDE)}"\Lib\Renderer\RenderType.hpp"\
	{$(INCLUDE)}"\Lib\Renderer\ScreenRender.hpp"\
	{$(INCLUDE)}"\Lib\Renderer\Texture.hpp"\
	{$(INCLUDE)}"\Lib\Std\Array.hpp"\
	{$(INCLUDE)}"\Lib\Std\Array2.hpp"\
	{$(INCLUDE)}"\Lib\Std\InitSys.hpp"\
	{$(INCLUDE)}"\Lib\Std\Ptr.hpp"\
	{$(INCLUDE)}"\Lib\Std\Random.hpp"\
	{$(INCLUDE)}"\Lib\Std\Set.hpp"\
	{$(INCLUDE)}"\Lib\Std\StdLibEx.hpp"\
	{$(INCLUDE)}"\Lib\Std\StringEx.hpp"\
	{$(INCLUDE)}"\Lib\Std\UAssert.hpp"\
	{$(INCLUDE)}"\Lib\Std\UDefs.hpp"\
	{$(INCLUDE)}"\Lib\Std\UTypes.hpp"\
	{$(INCLUDE)}"\Lib\Sys\Errors.hpp"\
	{$(INCLUDE)}"\Lib\Sys\FastHeap.hpp"\
	{$(INCLUDE)}"\Lib\Sys\MemoryLog.hpp"\
	{$(INCLUDE)}"\Lib\Sys\P5\Msr.hpp"\
	{$(INCLUDE)}"\Lib\Sys\Profile.hpp"\
	{$(INCLUDE)}"\Lib\Sys\RegInit.hpp"\
	{$(INCLUDE)}"\Lib\Sys\Scheduler.hpp"\
	{$(INCLUDE)}"\Lib\Sys\SmartBuffer.hpp"\
	{$(INCLUDE)}"\Lib\Sys\SysLog.hpp"\
	{$(INCLUDE)}"\Lib\Sys\Textout.hpp"\
	{$(INCLUDE)}"\Lib\Sys\Timer.hpp"\
	{$(INCLUDE)}"\Lib\Transform\Presence.hpp"\
	{$(INCLUDE)}"\Lib\Transform\Rotate.hpp"\
	{$(INCLUDE)}"\Lib\Transform\Transform.hpp"\
	{$(INCLUDE)}"\Lib\Transform\TransLinear.hpp"\
	{$(INCLUDE)}"\Lib\Transform\Vector.hpp"\
	{$(INCLUDE)}"\Lib\View\Clut.hpp"\
	{$(INCLUDE)}"\Lib\View\Colour.hpp"\
	{$(INCLUDE)}"\Lib\View\ColourBase.hpp"\
	{$(INCLUDE)}"\Lib\View\Palette.hpp"\
	{$(INCLUDE)}"\Lib\View\Pixel.hpp"\
	{$(INCLUDE)}"\lib\view\Raster.hpp"\
	{$(INCLUDE)}"\Lib\View\Viewport.hpp"\
	{$(INCLUDE)}"\Lib\W95\Com.hpp"\
	{$(INCLUDE)}"\Lib\W95\Direct3DQuery.hpp"\
	{$(INCLUDE)}"\Lib\W95\WinAlias.hpp"\
	{$(INCLUDE)}"\List.h"\
	{$(INCLUDE)}"\map.h"\
	{$(INCLUDE)}"\pair.h"\
	{$(INCLUDE)}"\projectn.h"\
	{$(INCLUDE)}"\set.h"\
	{$(INCLUDE)}"\stack.h"\
	{$(INCLUDE)}"\tree.h"\
	{$(INCLUDE)}"\Trespass\DDDevice.hpp"\
	{$(INCLUDE)}"\vector.h"\
	
NODEP_CPP_DDDEV=\
	"..\..\..\MSDEV\INCLUDE\d3dcom.h"\
	"..\..\..\MSDEV\INCLUDE\subwtype.h"\
	"..\..\Inc\DirectX\d3dcom.h"\
	"..\..\Inc\DirectX\subwtype.h"\
	"..\Lib\Audio\DirectX\d3drmwin.h"\
	"..\Lib\Audio\DirectX\DSound.h"\
	"..\Lib\Groff\StandardTypes.hpp"\
	"..\Lib\Sys\StandardTypes.hpp"\
	

"$(INTDIR)\DDDevice.obj" : $(SOURCE) $(DEP_CPP_DDDEV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TrespassAdv - Win32 Debug"

DEP_CPP_DDDEV=\
	"..\..\..\MSDEV\INCLUDE\d3dcaps.h"\
	"..\..\..\MSDEV\INCLUDE\d3dtypes.h"\
	"..\..\Inc\DirectX\d3d.h"\
	"..\..\Inc\DirectX\d3dcaps.h"\
	"..\..\Inc\DirectX\d3dtypes.h"\
	"..\..\Inc\DirectX\d3dvec.inl"\
	"..\..\Inc\DirectX\ddraw.h"\
	"..\gblinc\common.hpp"\
	"..\Lib\Audio\Audio.hpp"\
	"..\Lib\Control\Control.hpp"\
	"..\Lib\EntityDBase\EntityLight.hpp"\
	"..\Lib\EntityDBase\FrameHeap.hpp"\
	"..\Lib\EntityDBase\GameLoop.hpp"\
	"..\Lib\EntityDBase\MessageTypes\MsgControl.hpp"\
	"..\Lib\EntityDBase\MessageTypes\MsgSystem.hpp"\
	"..\Lib\EntityDBase\Query\QRenderer.hpp"\
	"..\Lib\EntityDBase\WorldDBase.hpp"\
	"..\Lib\GeomDBase\PartitionPriv.hpp"\
	"..\Lib\Physics\PhysicsSystem.hpp"\
	"..\Lib\Renderer\PipeLine.hpp"\
	"..\Lib\Renderer\RenderDefs.hpp"\
	"..\Lib\Std\Mem.hpp"\
	"..\lib\std\memlimits.hpp"\
	"..\Lib\Sys\ConIO.hpp"\
	"..\lib\sys\debugconsole.hpp"\
	"..\lib\sys\exepagemodify.hpp"\
	"..\lib\sys\performancecount.hpp"\
	"..\lib\sys\Processor.hpp"\
	"..\lib\sys\ProcessorDetect.hpp"\
	"..\lib\sys\w95\render.hpp"\
	"..\Lib\Transform\Matrix.hpp"\
	"..\Lib\Transform\Scale.hpp"\
	"..\Lib\Transform\Shear.hpp"\
	"..\Lib\Transform\Translate.hpp"\
	"..\Lib\View\Gamma.hpp"\
	"..\lib\view\rastervid.hpp"\
	"..\lib\view\video.hpp"\
	"..\lib\w95\dd.hpp"\
	"..\Lib\W95\Direct3D.hpp"\
	"..\lib\w95\wininclude.hpp"\
	"..\Trespass\precomp.h"\
	{$(INCLUDE)}"\algobase.h"\
	{$(INCLUDE)}"\bool.h"\
	{$(INCLUDE)}"\bstring.h"\
	{$(INCLUDE)}"\Config.hpp"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\defalloc.h"\
	{$(INCLUDE)}"\deque.h"\
	{$(INCLUDE)}"\function.h"\
	{$(INCLUDE)}"\GblInc\BuildVer.hpp"\
	{$(INCLUDE)}"\GblInc\Warnings.hpp"\
	{$(INCLUDE)}"\heap.h"\
	{$(INCLUDE)}"\iterator.h"\
	{$(INCLUDE)}"\Lib\EntityDBase\Container.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Entity.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Instance.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Message.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Query.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\QueueMessage.hpp"\
	{$(INCLUDE)}"\Lib\EntityDBase\Subsystem.hpp"\
	{$(INCLUDE)}"\Lib\GeomDBase\Partition.hpp"\
	{$(INCLUDE)}"\Lib\GeomDBase\PartitionSpace.hpp"\
	{$(INCLUDE)}"\Lib\GeomDBase\Plane.hpp"\
	{$(INCLUDE)}"\Lib\Loader\Fetchable.hpp"\
	{$(INCLUDE)}"\Lib\Math\FastSqrt.hpp"\
	{$(INCLUDE)}"\Lib\Math\FastTrig.hpp"\
	{$(INCLUDE)}"\Lib\Math\FloatDef.hpp"\
	{$(INCLUDE)}"\Lib\Math\FloatTable.hpp"\
	{$(INCLUDE)}"\Lib\Renderer\GeomTypes.hpp"\
	{$(INCLUDE)}"\Lib\Renderer\Material.hpp"\
	{$(INCLUDE)}"\Lib\Renderer\ScreenRender.hpp"\
	{$(INCLUDE)}"\Lib\Renderer\Texture.hpp"\
	{$(INCLUDE)}"\Lib\Std\Array.hpp"\
	{$(INCLUDE)}"\Lib\Std\Array2.hpp"\
	{$(INCLUDE)}"\Lib\Std\InitSys.hpp"\
	{$(INCLUDE)}"\Lib\Std\Ptr.hpp"\
	{$(INCLUDE)}"\Lib\Std\Set.hpp"\
	{$(INCLUDE)}"\Lib\Std\StdLibEx.hpp"\
	{$(INCLUDE)}"\Lib\Std\UAssert.hpp"\
	{$(INCLUDE)}"\Lib\Std\UDefs.hpp"\
	{$(INCLUDE)}"\Lib\Std\UTypes.hpp"\
	{$(INCLUDE)}"\Lib\Sys\Errors.hpp"\
	{$(INCLUDE)}"\Lib\Sys\FastHeap.hpp"\
	{$(INCLUDE)}"\Lib\Sys\MemoryLog.hpp"\
	{$(INCLUDE)}"\Lib\Sys\RegInit.hpp"\
	{$(INCLUDE)}"\Lib\Sys\Textout.hpp"\
	{$(INCLUDE)}"\Lib\Sys\Timer.hpp"\
	{$(INCLUDE)}"\Lib\Transform\Presence.hpp"\
	{$(INCLUDE)}"\Lib\Transform\Rotate.hpp"\
	{$(INCLUDE)}"\Lib\Transform\Transform.hpp"\
	{$(INCLUDE)}"\Lib\Transform\TransLinear.hpp"\
	{$(INCLUDE)}"\Lib\Transform\Vector.hpp"\
	{$(INCLUDE)}"\Lib\View\Clut.hpp"\
	{$(INCLUDE)}"\Lib\View\Colour.hpp"\
	{$(INCLUDE)}"\Lib\View\ColourBase.hpp"\
	{$(INCLUDE)}"\Lib\View\Palette.hpp"\
	{$(INCLUDE)}"\Lib\View\Pixel.hpp"\
	{$(INCLUDE)}"\lib\view\Raster.hpp"\
	{$(INCLUDE)}"\Lib\W95\Com.hpp"\
	{$(INCLUDE)}"\Lib\W95\Direct3DQuery.hpp"\
	{$(INCLUDE)}"\Lib\W95\WinAlias.hpp"\
	{$(INCLUDE)}"\List.h"\
	{$(INCLUDE)}"\map.h"\
	{$(INCLUDE)}"\pair.h"\
	{$(INCLUDE)}"\projectn.h"\
	{$(INCLUDE)}"\stack.h"\
	{$(INCLUDE)}"\tree.h"\
	{$(INCLUDE)}"\Trespass\DDDevice.hpp"\
	{$(INCLUDE)}"\vector.h"\
	
NODEP_CPP_DDDEV=\
	"..\..\..\MSDEV\INCLUDE\d3dcom.h"\
	"..\..\..\MSDEV\INCLUDE\subwtype.h"\
	"..\..\Inc\DirectX\d3dcom.h"\
	"..\..\Inc\DirectX\subwtype.h"\
	

"$(INTDIR)\DDDevice.obj" : $(SOURCE) $(DEP_CPP_DDDEV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\lib\DirectX\ddraw.lib

!IF  "$(CFG)" == "TrespassAdv - Win32 Release"

!ELSEIF  "$(CFG)" == "TrespassAdv - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\jp2_pc\lib\DirectX\dxguid.lib

!IF  "$(CFG)" == "TrespassAdv - Win32 Release"

!ELSEIF  "$(CFG)" == "TrespassAdv - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
