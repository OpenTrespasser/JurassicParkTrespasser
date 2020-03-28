project(TweakTrespass)

list(APPEND TweakTrespass_Inc    
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/DialogName.hpp
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/resource.h
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/StdAfx.h
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/TrespassAdv.h
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/TrespassAdvDlg.h
)

list(APPEND TweakTrespass_Src
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/DialogName.cpp
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/StdAfx.cpp
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/TrespassAdv.cpp
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/TrespassAdvDlg.cpp
)

list(APPEND TweakTrespass_Rsc
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/TrespassAdv.rc
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/res/TrespassAdv.rc2
    ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/res/TrespassAdv.ico
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${TweakTrespass_Inc} ${TweakTrespass_Src} ${TweakTrespass_Rsc} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Source/TrespassAdv/StdAfx.h)
