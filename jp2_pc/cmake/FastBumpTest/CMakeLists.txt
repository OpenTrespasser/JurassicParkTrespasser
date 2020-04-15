project(FastBumpTest)

list(APPEND FastBumpTest_Inc
    ${CMAKE_SOURCE_DIR}/Source/Test/FastBumpTest.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/FastBumpTestShell.cpp
)

list(APPEND FastBumpTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/FastBumpTest.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/FastBumpTestShell.hpp
)
include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${FastBumpTest_Inc} ${FastBumpTest_Src} )

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
