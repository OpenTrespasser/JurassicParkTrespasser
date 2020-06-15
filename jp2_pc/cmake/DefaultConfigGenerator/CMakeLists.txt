project(DefaultConfigGenerator)

list(APPEND DefaultConfigGenerator_Src
    ${CMAKE_SOURCE_DIR}/Source/Tools/Novel/DefaultConfigGenerator/DefaultConfigGenerator.cpp
)

add_common_options()

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_executable(${PROJECT_NAME} ${DefaultConfigGenerator_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools/Novel)

target_link_libraries(${PROJECT_NAME}
    System
)
