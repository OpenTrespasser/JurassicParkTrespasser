project(File)

list(APPEND File_Inc
    ${CMAKE_SOURCE_DIR}/Source/Lib/File/Image.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/File/Section.hpp
)

list(APPEND File_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/File/Image.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/File/Section.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${File_Inc} ${File_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Util)
