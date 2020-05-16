project(ProcessorDetect)

list(APPEND ProcessorDetect_Inc
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/Processor.hpp
)

list(APPEND ProcessorDetect_Src
    "${CMAKE_SOURCE_DIR}/Source/Tools/Processor Detect/Detect.cpp"
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} SHARED ${ProcessorDetect_Inc} ${ProcessorDetect_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Processor")

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Other)
