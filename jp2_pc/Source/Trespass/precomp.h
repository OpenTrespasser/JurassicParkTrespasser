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
#include "..\lib\w95\wininclude.hpp"
#include "..\gblinc\common.hpp"
#include "..\Lib\GeomDBase\PartitionPriv.hpp"
#include "..\lib\std\memlimits.hpp"
#include "..\lib\sys\debugconsole.hpp"
#include "..\Lib\Sys\W95\Render.hpp"
#include "..\lib\sys\exepagemodify.hpp"
#include "..\lib\sys\performancecount.hpp"
#include "..\lib\sys\w95\render.hpp"
#include "..\Lib\Sys\ConIO.hpp"
#include "..\lib\sys\ProcessorDetect.hpp"
#include "..\Lib\W95\Direct3D.hpp"
#include "..\lib\w95\dd.hpp"
#include "..\lib\view\video.hpp"
#include "..\lib\view\raster.hpp"
#include "..\lib\view\rastervid.hpp"
#include "..\lib\physics\physicssystem.hpp"
#include "..\Lib\EntityDBase\GameLoop.hpp"
#include "..\Lib\EntityDBase\MessageTypes\MsgControl.hpp"
#include "..\Lib\EntityDBase\MessageTypes\MsgSystem.hpp"
#include "..\Lib\EntityDBase\EntityLight.hpp"
#include "..\Lib\EntityDBase\WorldDBase.hpp"
#include "..\Lib\EntityDBase\Query\QRenderer.hpp"
#include "..\Lib\EntityDBase\Query\QTerrain.hpp"
#include "..\Lib\Audio\Audio.hpp"
#include "..\Lib\Audio\Material.hpp"
#include "..\Lib\Audio\AudioDaemon.hpp"
#include "..\Lib\Audio\AudioLoader.hpp"
#include "..\Lib\Loader\DataDaemon.hpp"
#include "..\lib\loader\loader.hpp"
#include "..\lib\loader\imageloader.hpp"
#include "..\lib\loader\loadtexture.hpp"
#include "..\lib\loader\savefile.hpp"
#include "..\Lib\Control\Control.hpp"
#include "..\Lib\Renderer\Camera.hpp"
#include "..\Lib\Renderer\PipeLine.hpp"
#include "..\lib\renderer\light.hpp"
#include "..\Lib\Renderer\LightBlend.hpp"
#include "..\Lib\Physics\PhysicsSystem.hpp"
#include "..\lib\trigger\GameActions.hpp"
#include "..\shell\winrendertools.hpp"
#include "..\Game\AI\AIMain.hpp"
#include "..\Game\DesignDaemon\Player.hpp"
#include "supportfn.hpp"
#include <windowsx.h>
#include <commctrl.h>

#ifndef IsKeyDown
#define IsKeyDown(x) (GetKeyState(x) < 0)
#endif

#endif // __PRECOMP_H__
