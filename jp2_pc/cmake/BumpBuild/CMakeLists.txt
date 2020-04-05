project(BumpBuild)

list(APPEND BumpBuild_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/BumpBuild/BumpBuild.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/BumpBuild/HeightBitmap.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/BumpBuild/HeightBitmap.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/BumpBuild/IOStuff.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/BumpBuild/IOStuff.hpp
)

add_common_options()

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_executable(${PROJECT_NAME} ${BumpBuild_Src})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tests)
