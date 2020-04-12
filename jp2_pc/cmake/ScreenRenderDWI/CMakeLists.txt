project(ScreenRenderDWI)

list(APPEND ScreenRenderDWI_Inc
    ${CMAKE_SOURCE_DIR}/Source/gblinc/AsmSupport.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Primitives/DrawSubTriangle.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Primitives/DrawTriangle.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Primitives/FastBump.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/P6/FastBumpEx.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/FastBumpMath.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Primitives/FastBumpTable.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/GouraudT.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/RenderCache.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/RenderCacheHelp.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/RenderCachePriv.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/P6/ScanlineAsmMacros.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/ScreenRenderAuxD3D.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/ScreenRenderAuxD3DBatch.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/screenrenderdwi.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/Walk.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/P6/WalkEx.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/RenderCacheInterface.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/RenderCacheLRUItem.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/ColLookupT.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/Edge.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/IndexPerspectiveT.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/LineBumpMake.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/MapT.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/ScanLine.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/TransparencyT.hpp
)

list(APPEND ScreenRenderDWI_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/DrawSubTriangle.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/DrawTriangle.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/DrawTriangleEx.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/FastBump.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/FastBumpMath.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/FastBumpTable.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/RenderCache.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/RenderCacheHelp.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/RenderCachePriv.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/ScreenRenderAuxD3D.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/ScreenRenderAuxD3DBatch.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/ScreenRenderDWI.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${ScreenRenderDWI_Inc} ${ScreenRenderDWI_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Render)
