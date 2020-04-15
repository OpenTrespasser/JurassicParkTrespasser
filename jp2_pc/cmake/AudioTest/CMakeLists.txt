project(AudioTest)

list(APPEND AudioTest_Inc
    ${CMAKE_SOURCE_DIR}/Source/Test/AudioTest.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AudioTestDoc.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AudioTestView.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/MainFrm.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/AttribDlg.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/StdAfx.cpp
)

list(APPEND AudioTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/AudioTest.h
    ${CMAKE_SOURCE_DIR}/Source/Test/AudioTestDoc.h
    ${CMAKE_SOURCE_DIR}/Source/Test/AudioTestView.h
    ${CMAKE_SOURCE_DIR}/Source/Test/MainFrm.h
    ${CMAKE_SOURCE_DIR}/Source/Test/resource_audio.h
    ${CMAKE_SOURCE_DIR}/Source/Test/AttribDlg.h
    ${CMAKE_SOURCE_DIR}/Source/Test/StdAfx.h
)

list(APPEND AudioTest_Rsc
    ${CMAKE_SOURCE_DIR}/Source/Test/AudioTest.rc
    ${CMAKE_SOURCE_DIR}/Source/Test/res/AudioTest.ico
    ${CMAKE_SOURCE_DIR}/Source/Test/res/AudioTest.rc2
    ${CMAKE_SOURCE_DIR}/Source/Test/res/AudioTestDoc.ico
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${AudioTest_Inc} ${AudioTest_Src} ${AudioTest_Rsc} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tests)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Source/Test/StdAfx.h)

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
