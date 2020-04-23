project(RegToIniConverter)

list(APPEND RegToIniConverter_Src
    ${CMAKE_SOURCE_DIR}/Source/Tools/Novel/RegToIniConverter/RegToIniConverter.cpp
)

add_common_options()

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_executable(${PROJECT_NAME} ${RegToIniConverter_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools/Novel)

target_link_libraries(${PROJECT_NAME}
    System
)
