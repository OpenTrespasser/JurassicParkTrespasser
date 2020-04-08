project(GroffExp)

list(APPEND GroffExp_Inc
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Bitmap.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/EasyString.hpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Export.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/FileIO.hpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Geometry.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/Groff.hpp
    ${CMAKE_SOURCE_DIR}/source/Tools/GroffExp/GUIInterface.hpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Mathematics.hpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/ObjectDef.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/ObjectHandle.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/SmartBuffer.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/Symtab.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/SysLog.hpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/GroffExp.h
)

list(APPEND GroffExp_Rsc
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/GroffExp.def
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/GroffExp.rc
)

list(APPEND GroffExp_Src
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Bitmap.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/EasyString.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Export.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/FileIO.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Geometry.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/Groff.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/GroffExp.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/GUIInterface.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Mathematics.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/ObjectDef.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/ObjectHandle.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/SmartBuffer.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/Symtab.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/SysLog.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
    ${CMAKE_SOURCE_DIR}/Inc/maxsdk
)

add_common_options()

add_library(${PROJECT_NAME} SHARED ${GroffExp_Inc} ${GroffExp_Src} ${GroffExp_Rsc} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dle") #Replace .dll with .dle

target_link_options(${PROJECT_NAME} PUBLIC "/SAFESEH:NO") #Needed for old 3DS Max SDK

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

    comctl32
    lz32
    winmm

    ddraw
    dsound
    dxguid

    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/BMM.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/CLIENT.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/CORE.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/EXPR.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/FLILIBD.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/FLILIBH.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/FLILIBR.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/FLT.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/GCOMM.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/GEOM.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/GFX.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/MESH.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/NURBS.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/PARTICLE.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/PATCH.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/RESMGR.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/UTIL.LIB
    ${CMAKE_SOURCE_DIR}/Lib/MaxSDK/VIEWFILE.LIB
)

