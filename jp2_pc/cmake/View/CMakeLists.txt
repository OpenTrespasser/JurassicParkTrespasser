project(View)

list(APPEND View_Inc
    ${CMAKE_SOURCE_DIR}/source/Lib/View/AGPTextureMemManager.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/Clut.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/Colour.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/ColourBase.hpp
    ${CMAKE_SOURCE_DIR}/Source/lib/w95/dd.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/W95/DDFont.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/W95/Direct3D.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/W95/Direct3DQuery.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/Direct3DRenderState.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/Gamma.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/Grab.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/W95/Ispvr.h
    ${CMAKE_SOURCE_DIR}/source/Lib/View/LineDraw.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/Palette.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/pallookup.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/Pixel.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/Raster.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/RasterComposite.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/RasterConvertD3D.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/RasterD3D.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/RasterFile.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/RasterPool.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/RasterVid.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/View/Viewport.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/ColourT.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/Video.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/W95//Direct3DCards.hpp
)

list(APPEND View_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/AGPTextureMemManager.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/Clut.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/Colour.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/ColourBase.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/W95/dd.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/W95/DDFont.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/W95/Direct3D.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/W95/Direct3DCards.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/W95/Direct3DQuery.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/Direct3DRenderState.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/Gamma.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/Grab.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/LineDraw.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/Palette.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/PalLookup.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/Pixel.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/Raster.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/RasterComposite.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/RasterConvertD3D.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/RasterD3D.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/RasterFile.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/RasterPool.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/W95/RasterVid.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/W95/Video.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/View/Viewport.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${View_Inc} ${View_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Render)
