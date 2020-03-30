project(Render3D)

list(APPEND Render3D_Inc
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Camera.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Clip.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/ClipRegion2D.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/DepthSort.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/DepthSortTools.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Fog.hpp
    ${CMAKE_SOURCE_DIR}/Source/lib/renderer/light.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/LightBlend.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Line.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/LineSide2D.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Material.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Particles.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Pipeline.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/PipeLineHeap.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/PipeLineHelp.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/ScreenRender.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/ScreenRenderShadow.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Shadow.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Sky.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Texture.hpp
)

list(APPEND Render3D_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Camera.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Clip.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/ClipRegion2D.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/DepthSort.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/DepthSortTools.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Fog.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Light.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/LightBlend.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Line.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/LineSide2D.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Material.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Overlay.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Particles.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/PipeLine.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/PipeLineHeap.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/PipeLineHelp.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/ScreenPolygon.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/ScreenRender.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/ScreenRenderShadow.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Shadow.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Sky.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Texture.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${Render3D_Inc} ${Render3D_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Render)
