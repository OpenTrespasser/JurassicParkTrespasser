project(StdTest)

list(APPEND StdTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/Novel/StdTest/CSetTest.cpp
)

add_common_options()

add_executable(${PROJECT_NAME} ${StdTest_Src})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tests/Novel)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
    ${CMAKE_SOURCE_DIR}/gtest/googletest/include
)

target_link_libraries( ${PROJECT_NAME}
    Std
    gtest
    gtest_main
)

enable_testing()
add_test(${PROJECT_NAME} ${PROJECT_NAME} "--gtest_output=xml:${PROJECT_NAME}_TestResults.xml")
