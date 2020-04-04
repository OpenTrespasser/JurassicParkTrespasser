project(WaveTest)

list(APPEND WaveTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/Physics/WaveTest.cpp
    ${CMAKE_SOURCE_DIR}/Source/Shell/WinShell.rc
)

add_common_options()

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
    ${CMAKE_SOURCE_DIR}/Inc
)

add_executable(${PROJECT_NAME} WIN32 ${WaveTest_Src})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tests)
