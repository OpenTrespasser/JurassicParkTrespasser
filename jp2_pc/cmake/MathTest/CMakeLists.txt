project(MathTest)

list(APPEND MathTest_Inc    
    ${CMAKE_SOURCE_DIR}/Source/Test/TestMath.hpp
)

list(APPEND MathTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/TestMath.cpp
)

list(APPEND MathTest_Rsc
    ${CMAKE_SOURCE_DIR}/Source/Shell/WinShell.rc

)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
    ${CMAKE_SOURCE_DIR}
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${MathTest_Inc} ${MathTest_Src} ${MathTest_Rsc} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tests)

target_link_libraries(${PROJECT_NAME}
    AI
    Audio
    EntityDBase
    File
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

    dxguid
    dsound
    ddraw
)
