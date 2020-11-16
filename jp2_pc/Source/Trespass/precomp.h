//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1997.
//
//  File:       precomp.h
//
//  Contents:   Precompile Header File
//
//  Classes:
//
//  Functions:
//
//  History:    21-Oct-97   SHernd   Created
//
//---------------------------------------------------------------------------

#ifndef __PRECOMP_H__
#define __PRECOMP_H__

// We want to include the resource IDs for the OEM settings as well
#define OEMRESOURCE
#include "../Lib/W95/WinInclude.hpp"
#include <smacker/SMACK.H>
#include "../gblinc/common.hpp"
#include "../Lib/GeomDBase/PartitionPriv.hpp"
#include "../Lib/Std/MemLimits.hpp"
#include "../Lib/Sys/DebugConsole.hpp"
#include "../Lib/Sys/W95/Render.hpp"
#include "../Lib/Sys/ExePageModify.hpp"
#include "../Lib/Sys/PerformanceCount.hpp"
#include "../Lib/Sys/W95/Render.hpp"
#include "../Lib/Sys/ConIO.hpp"
#include "../Lib/Sys/ProcessorDetect.hpp"
#include "../Lib/W95/Direct3D.hpp"
#include "../Lib/W95/DD.hpp"
#include "../Lib/View/Video.hpp"
#include "../Lib/View/Raster.hpp"
#include "../Lib/View/RasterVid.hpp"
#include "../Lib/Physics/PhysicsSystem.hpp"
#include "../Lib/EntityDBase/GameLoop.hpp"
#include "../Lib/EntityDBase/MessageTypes/MsgControl.hpp"
#include "../Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "../Lib/EntityDBase/EntityLight.hpp"
#include "../Lib/EntityDBase/WorldDBase.hpp"
#include "../Lib/EntityDBase/Query/QRenderer.hpp"
#include "../Lib/EntityDBase/Query/QTerrain.hpp"
#include "../Lib/Audio/Audio.hpp"
#include "../Lib/Audio/Material.hpp"
#include "../Lib/Audio/AudioDaemon.hpp"
#include "../Lib/Audio/AudioLoader.hpp"
#include "../Lib/Loader/DataDaemon.hpp"
#include "../Lib/Loader/Loader.hpp"
#include "../Lib/Loader/ImageLoader.hpp"
#include "../Lib/Loader/LoadTexture.hpp"
#include "../Lib/Loader/SaveFile.hpp"
#include "../Lib/Control/Control.hpp"
#include "../Lib/Renderer/Camera.hpp"
#include "../Lib/Renderer/PipeLine.hpp"
#include "../Lib/Renderer/Light.hpp"
#include "../Lib/Renderer/LightBlend.hpp"
#include "../Lib/Physics/PhysicsSystem.hpp"
#include "../Lib/Trigger/GameActions.hpp"
#include "../Shell/WinRenderTools.hpp"
#include "../Game/AI/AIMain.hpp"
#include "../Game/DesignDaemon/Player.hpp"
#include "supportfn.hpp"
#include <windowsx.h>
#include <commctrl.h>

#ifndef IsKeyDown
#define IsKeyDown(x) (GetKeyState(x) < 0)
#endif

#endif // __PRECOMP_H__
