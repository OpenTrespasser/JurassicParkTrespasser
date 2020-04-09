project(GroffBuild)

list(APPEND GroffBuild_Inc    
    ${CMAKE_SOURCE_DIR}/Source/GroffBuild/dialogs.h
    ${CMAKE_SOURCE_DIR}/Source/GroffBuild/gbuilder.h
    ${CMAKE_SOURCE_DIR}/Source/GroffBuild/main.h
    ${CMAKE_SOURCE_DIR}/Source/GroffBuild/utils.h
    ${CMAKE_SOURCE_DIR}/Source/GroffBuild/precomp.h
)

list(APPEND GroffBuild_Src
    ${CMAKE_SOURCE_DIR}/Source/GroffBuild/Dialogs.cpp
    ${CMAKE_SOURCE_DIR}/Source/GroffBuild/gbuilder.cpp
    ${CMAKE_SOURCE_DIR}/Source/GroffBuild/main.cpp
    ${CMAKE_SOURCE_DIR}/Source/GroffBuild/maindlg.cpp
    ${CMAKE_SOURCE_DIR}/Source/GroffBuild/Utils.cpp
)

list(APPEND GroffBuild_Rsc
    ${CMAKE_SOURCE_DIR}/Source/GroffBuild/GroffBuild.rc
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${GroffBuild_Inc} ${GroffBuild_Src} ${GroffBuild_Rsc} )

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Source/GroffBuild/precomp.h)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)

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
    version
    comctl32
    winmm

    dxguid
    dsound
    ddraw
)
