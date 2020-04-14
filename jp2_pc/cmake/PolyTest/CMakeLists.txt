project(PolyTest)

list(APPEND PolyTest_Inc
    ${CMAKE_SOURCE_DIR}/Source/Test/PolyData.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/PolyTest.cpp
)

list(APPEND PolyTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/PolyData.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/PolyTest.hpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${PolyTest_Inc} ${PolyTest_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tests)

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
    WinShell

    lz32
    winmm

    ddraw
    dxguid
    dsound
)
