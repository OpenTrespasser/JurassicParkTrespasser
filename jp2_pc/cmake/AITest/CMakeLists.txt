project(AITest)

list(APPEND AITest_Inc
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/AITest.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/AITestShell.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/AStarTest.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/FakePhysics.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/FakeShape.hpp
    ${CMAKE_SOURCE_DIR}/Source/GUIApp/PredefinedShapes.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/TestAnimal.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/TestBrains.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/UIModes.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/AIResource.h
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/QueryTest.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/TestTree.hpp
)

list(APPEND AITest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/AITest.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/AITestShell.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/AStarTest.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/FakePhysics.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/FakeShape.cpp
    ${CMAKE_SOURCE_DIR}/Source/GUIApp/PreDefinedShapes.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/TestAnimal.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/TestBrains.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/UIModes.cpp
)

list(APPEND AITest_Rsc
    ${CMAKE_SOURCE_DIR}/Source/Test/AI/AI.rc
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${AITest_Inc} ${AITest_Src} ${AITest_Rsc} )
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

    dxguid
    dsound
    ddraw
)
