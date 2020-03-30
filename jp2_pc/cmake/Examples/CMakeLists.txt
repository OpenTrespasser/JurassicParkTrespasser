project(Examples)

list(APPEND Examples_Src
    ${CMAKE_SOURCE_DIR}/Source/Examples/Class.hpp
    ${CMAKE_SOURCE_DIR}/Source/Examples/Examples.hpp
    ${CMAKE_SOURCE_DIR}/Source/Examples/File.cpp
    ${CMAKE_SOURCE_DIR}/Source/Examples/File.hpp
    ${CMAKE_SOURCE_DIR}/Source/Examples/Function.hpp
    ${CMAKE_SOURCE_DIR}/Source/Examples/Namespace.hpp
    ${CMAKE_SOURCE_DIR}/Source/Examples/Section.hpp
)

add_common_options()

add_executable(${PROJECT_NAME} ${Examples_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Other)
