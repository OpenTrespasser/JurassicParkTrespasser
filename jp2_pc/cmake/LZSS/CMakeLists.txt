project(LZSS)

list(APPEND LZSS_Src
    ${CMAKE_SOURCE_DIR}/Source/Tools/LZSS/LZSS.cpp
)

add_common_options()

add_executable(${PROJECT_NAME} ${LZSS_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)
