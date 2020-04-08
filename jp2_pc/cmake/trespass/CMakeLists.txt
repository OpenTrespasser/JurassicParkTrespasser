cmake_minimum_required(VERSION 3.5)
project(trespass)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND trespass_Inc
${CMAKE_SOURCE_DIR}/Source/Trespass/video.h
${CMAKE_SOURCE_DIR}/Source/Trespass/Cdib.h
${CMAKE_SOURCE_DIR}/Source/Trespass/ctrls.h
${CMAKE_SOURCE_DIR}/Source/trespass/DDDevice.hpp
${CMAKE_SOURCE_DIR}/Source/Trespass/dialogs.h
${CMAKE_SOURCE_DIR}/Source/trespass/keyremap.h
${CMAKE_SOURCE_DIR}/Source/Trespass/main.h
${CMAKE_SOURCE_DIR}/Source/trespass/precomp.h
${CMAKE_SOURCE_DIR}/Source/Trespass/rasterdc.hpp
${CMAKE_SOURCE_DIR}/Source/Trespass/supportfn.hpp
${CMAKE_SOURCE_DIR}/Source/Trespass/token.h
${CMAKE_SOURCE_DIR}/Source/Trespass/tpassglobals.h
${CMAKE_SOURCE_DIR}/Source/Trespass/uidlgs.h
${CMAKE_SOURCE_DIR}/Source/Trespass/uiwnd.h
)

list(APPEND trespass_Src    
${CMAKE_SOURCE_DIR}/Source/trespass/aaaa.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/Cdib.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/ctrls.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/DDDevice.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/Dialogs.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/dlgrender.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/gamewnd.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/keyremap.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/main.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/mainwnd.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/rasterdc.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/saveload.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/supportfn.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/token.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/tpassglobals.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/uidlgs.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/uiwnd.cpp
${CMAKE_SOURCE_DIR}/Source/trespass/video.cpp
)

list(APPEND trespass_Rsc
${CMAKE_SOURCE_DIR}/Source/trespass/res/icon_rap.ico
${CMAKE_SOURCE_DIR}/Source/trespass/res/smallPal.bmp
${CMAKE_SOURCE_DIR}/Source/trespass/res/USA_ctrls.rc2
${CMAKE_SOURCE_DIR}/Source/Trespass/res/USA_hints.rc2
${CMAKE_SOURCE_DIR}/Source/trespass/res/USA_keyremap.rc2
${CMAKE_SOURCE_DIR}/Source/trespass/version.rc2
${CMAKE_SOURCE_DIR}/Source/trespass/trespass.rc

)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
    ${CMAKE_SOURCE_DIR}/Inc
)

add_compile_definitions(TRESPASS)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${trespass_Inc} ${trespass_Src} ${trespass_Rsc} )

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

    comctl32
    lz32    
    winmm

    ddraw
    dsound
    dxguid

    ${CMAKE_SOURCE_DIR}/lib/smacker/SMACKW32.LIB
)

target_link_options(${PROJECT_NAME} PUBLIC "/SAFESEH:NO") #Needed only for old Smacker lib, remove when it gets replaced

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Source/trespass/precomp.h)

if (FOUND_TRESPASS_DIR AND USE_TRESPASSER_DIRECTORY)
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX})

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_INSTALL_PREFIX}/$<TARGET_FILE_NAME:${PROJECT_NAME}>)
endif()
