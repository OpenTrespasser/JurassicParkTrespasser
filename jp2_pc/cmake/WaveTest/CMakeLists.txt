project(WaveTest)

list(APPEND WaveTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/Physics/WaveTest.cpp
    ${CMAKE_SOURCE_DIR}/Source/Shell/WinShell.rc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${WaveTest_Src})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tests)
