project(CollisionEditor)

list(APPEND CollisionEditor_Inc
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/CollisionEditor.h
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/CollisionEditorDlg.h
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/Database.h
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/EditMaterialDlg.h
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/resource.h
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/SampleListDlg.h
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/stdafx.h
)

list(APPEND CollisionEditor_Src
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/CollisionEditor.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/CollisionEditorDlg.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/Database.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/EditMaterialDlg.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/SampleListDlg.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/StdAfx.cpp
)

list(APPEND CollisionEditor_Rsc
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/CollisionEditor.rc
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/res/CollisionEditor.rc2
    ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/res/CollisionEditor.ico
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${CollisionEditor_Inc} ${CollisionEditor_Src} ${CollisionEditor_Rsc} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Source/Tools/CollisionEditor/stdafx.h)

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

    lz32
)
