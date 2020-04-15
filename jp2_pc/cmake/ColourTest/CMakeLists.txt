project(ColourTest)

list(APPEND ColourTest_Inc
    ${CMAKE_SOURCE_DIR}/Source/Test/ColourTest.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/ColourDrawTest.cpp
)

list(APPEND ColourTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/ColourTest.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/ColourDrawTest.hpp
)
include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${ColourTest_Inc} ${ColourTest_Src} )

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
