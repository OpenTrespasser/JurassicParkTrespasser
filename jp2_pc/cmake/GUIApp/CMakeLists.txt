project(GUIApp)

list(APPEND GUIApp_Inc    
"${CMAKE_SOURCE_DIR}/Source/GUIApp/AI Dialogs2Dlg.h"
${CMAKE_SOURCE_DIR}/Source/GUIApp/Background.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/CameraProperties.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogAlphaColour.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogBumpPack.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogCulling.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogDepthSort.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogFog.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogGamma.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogGore.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogGun.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogMagnet.h
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogMaterial.h
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogMemLog.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogMipmap.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogObject.h
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogOcclusion.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogPartition.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogPhysics.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogPlayer.h
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogRenderCache.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogScheduler.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogSky.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogSoundMaterial.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogString.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogTeleport.h
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogTerrain.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogTexturePack.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogTexturePackOptions.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogVM.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogWater.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/GDIBitmap.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/GUIApp.h
${CMAKE_SOURCE_DIR}/source/GUIApp/GUIAppDlg.h
${CMAKE_SOURCE_DIR}/Source/GUIApp/GUIControls.hpp
${CMAKE_SOURCE_DIR}/source/GUIApp/GUIPipeLine.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/GUITools.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/LightProperties.hpp
${CMAKE_SOURCE_DIR}/source/GUIApp/ParameterDlg.h
${CMAKE_SOURCE_DIR}/Source/GUIApp/PerspectiveSubdivideDialog.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/QualityDialog.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/SoundPropertiesDlg.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/StdAfx.h
${CMAKE_SOURCE_DIR}/Source/GUIApp/TerrainTest.hpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/Toolbar.hpp
)

list(APPEND GUIApp_Src
"${CMAKE_SOURCE_DIR}/Source/GUIApp/AI Dialogs2Dlg.cpp"
${CMAKE_SOURCE_DIR}/Source/GUIApp/Background.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/CameraProperties.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogAlphaColour.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogBumpPack.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogCulling.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogDepthSort.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogFog.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogGamma.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogGore.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogGun.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogMagnet.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogMaterial.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogMemLog.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogMipmap.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogObject.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogOcclusion.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogPartition.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogPhysics.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogPlayer.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogRenderCache.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogScheduler.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogSky.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogSoundMaterial.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogString.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogTeleport.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogTerrain.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogTexturePack.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogTexturePackOptions.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogVM.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/DialogWater.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/FindDuplicates.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/GDIBitmap.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/GUIApp.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/GUIApp.rc
${CMAKE_SOURCE_DIR}/Source/GUIApp/GUIAppDlg.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/GUIControls.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/GUIPipeline.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/GUITools.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/LightProperties.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/Options.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/ParameterDlg.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/PerspectiveSubdivideDialog.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/QualityDialog.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/SoundPropertiesDlg.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/StdAfx.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/TerrainTest.cpp
${CMAKE_SOURCE_DIR}/Source/GUIApp/Toolbar.cpp

)

list(APPEND GUIApp_Rsc
${CMAKE_SOURCE_DIR}/Source/GUIApp/res/GUIApp.ico
${CMAKE_SOURCE_DIR}/Source/GUIApp/res/GUIApp.rc2
${CMAKE_SOURCE_DIR}/Source/GUIApp/res/icon1.ico
${CMAKE_SOURCE_DIR}/Source/GUIApp/res/icon_rap.ico
${CMAKE_SOURCE_DIR}/Source/GUIApp/res/Palette.bmp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
    ${CMAKE_SOURCE_DIR}
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${GUIApp_Inc} ${GUIApp_Src} ${GUIApp_Rsc} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Source/GUIApp/StdAfx.h)

target_link_libraries(${PROJECT_NAME}
    AI
    Audio
    EntityDBase
    Game
    GeomDBase
    Loader
    Math
    Physics
    Render3D
    ScreenRenderDWI
    Std
    System
    View

    lz32
)
