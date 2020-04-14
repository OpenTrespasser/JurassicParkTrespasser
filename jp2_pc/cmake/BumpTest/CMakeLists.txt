project(BumpTest)

list(APPEND BumpTest_Inc
    ${CMAKE_SOURCE_DIR}/Source/Test/TestBump.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/scan.cpp
)

list(APPEND BumpTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/scan.hpp
)
include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${BumpTest_Inc} ${BumpTest_Src} )

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
