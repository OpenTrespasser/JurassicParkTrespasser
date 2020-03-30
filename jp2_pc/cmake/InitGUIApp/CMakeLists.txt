project(InitGUIApp)

list(APPEND InitGUIApp_Inc    
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/DDDevice.hpp
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/resource.h
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/StdAfx.h
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/InitGUIApp.h
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/InitGUIAppDlg.h
)

list(APPEND InitGUIApp_Src
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/DDDevice.cpp
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/StdAfx.cpp
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/InitGUIApp.cpp
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/InitGUIAppDlg.cpp
)

list(APPEND InitGUIApp_Rsc
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/InitGUIApp.rc
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/res/InitGUIApp.rc2
    ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/res/InitGUIApp.ico
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${InitGUIApp_Inc} ${InitGUIApp_Src} ${InitGUIApp_Rsc} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Source/InitGUIApp/StdAfx.h)
