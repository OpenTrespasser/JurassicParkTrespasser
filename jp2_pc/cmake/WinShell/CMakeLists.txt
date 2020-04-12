project(WinShell)

list(APPEND WinShell_Inc
    ${CMAKE_SOURCE_DIR}/Source/Shell/AppEvent.hpp
    ${CMAKE_SOURCE_DIR}/Source/Shell/AppShell.hpp
    ${CMAKE_SOURCE_DIR}/Source/Shell/WinShell.hpp
    ${CMAKE_SOURCE_DIR}/Source/Shell/ShellResource.h
    ${CMAKE_SOURCE_DIR}/Source/Shell/Resource.h
    ${CMAKE_SOURCE_DIR}/Source/Shell/WinEvent.hpp
)

list(APPEND WinShell_Src
    ${CMAKE_SOURCE_DIR}/Source/Shell/AppEvent.cpp
    ${CMAKE_SOURCE_DIR}/Source/Shell/AppShell.cpp
    ${CMAKE_SOURCE_DIR}/Source/Shell/winshell.cpp
)

list(APPEND WinShell_Rsc
    ${CMAKE_SOURCE_DIR}/Source/Shell/WinShell.rc

)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${WinShell_Inc} ${WinShell_Src} ${WinShell_Rsc} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tests)
