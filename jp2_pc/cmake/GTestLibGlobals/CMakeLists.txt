project(GTestLibGlobals)

list(APPEND GTestLibGlobals_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/Novel/GTestLibGlobals/GTestLibGlobals.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${GTestLibGlobals_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tests/gtest)
