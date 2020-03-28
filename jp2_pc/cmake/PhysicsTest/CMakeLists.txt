project(PhysicsTest)

list(APPEND PhysicsTest_Inc
    ${CMAKE_SOURCE_DIR}/Source/Test/Physics/PhysicsTest.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/Physics/PhysicsTestShell.hpp
)

list(APPEND PhysicsTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/Physics/PhysicsTest.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/Physics/PhysicsTestShell.cpp
)

list(APPEND PhysicsTest_Rsc
    ${CMAKE_SOURCE_DIR}/Source/Shell/WinShell.rc
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
    ${CMAKE_SOURCE_DIR}
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${PhysicsTest_Inc} ${PhysicsTest_Src} ${PhysicsTest_Rsc} )

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
)
