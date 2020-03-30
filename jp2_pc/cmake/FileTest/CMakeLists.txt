project(FileTest)

list(APPEND FileTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/File/FileTest.cpp
)

add_common_options()

add_executable(${PROJECT_NAME} ${FileTest_Src})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tests)

target_link_libraries( ${PROJECT_NAME}
    File
)
