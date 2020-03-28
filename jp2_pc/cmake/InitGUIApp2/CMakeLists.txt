project(InitGUIApp2)

list(APPEND InitGUIApp2_Inc    
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp2/resource.h
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp2/StdAfx.h
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp2/InitGUIApp2.h
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp2/InitGUIApp2Dlg.h
)

list(APPEND InitGUIApp2_Src
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp2/StdAfx.cpp
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp2/InitGUIApp2.cpp
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp2/InitGUIApp2Dlg.cpp
)

list(APPEND InitGUIApp2_Rsc
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp2/InitGUIApp2.rc
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp2/res/InitGUIApp2.rc2
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp2/res/InitGUIApp2.ico
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${InitGUIApp2_Inc} ${InitGUIApp2_Src} ${InitGUIApp2_Rsc} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Source/InitGUIApp2/StdAfx.h)

target_link_libraries(${PROJECT_NAME}
    System
    GeomDBase
)
