project(TweakNvidia128)

list(APPEND TweakNvidia128_Src
    ${CMAKE_SOURCE_DIR}/Source/TweakNVidia128/TweakNVidia128.cpp
    ${CMAKE_SOURCE_DIR}/Source/TweakNVidia128/TweakNVidia128Dlg.cpp
    ${CMAKE_SOURCE_DIR}/Source/TweakNVidia128/StdAfx.cpp
)

list(APPEND TweakNvidia128_Inc
    ${CMAKE_SOURCE_DIR}/Source/TweakNVidia128/TweakNVidia128.h
    ${CMAKE_SOURCE_DIR}/Source/TweakNVidia128/TweakNVidia128Dlg.h
    ${CMAKE_SOURCE_DIR}/Source/TweakNVidia128/Resource.h
    ${CMAKE_SOURCE_DIR}/Source/TweakNVidia128/StdAfx.h
)

list(APPEND TweakNvidia128_Res
    ${CMAKE_SOURCE_DIR}/Source/TweakNVidia128/TweakNVidia128.rc
    ${CMAKE_SOURCE_DIR}/Source/TweakNVidia128/res/TweakNVidia128.rc2
    ${CMAKE_SOURCE_DIR}/Source/TweakNVidia128/res/TweakNVidia128.ico
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${TweakNvidia128_Src} ${TweakNvidia128_Inc} ${TweakNvidia128_Res} )

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Source/TweakNVidia128/StdAfx.h)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)

target_link_libraries(${PROJECT_NAME}
    ddraw
    dxguid
)
