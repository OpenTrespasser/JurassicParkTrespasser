project(PipelineTest)

list(APPEND PipelineTest_Inc
    ${CMAKE_SOURCE_DIR}/Source/Test/Test3DObjs.hpp
    ${CMAKE_SOURCE_DIR}/Source/Test/TestPipeLine.hpp
)

list(APPEND PipelineTest_Src
    ${CMAKE_SOURCE_DIR}/Source/Test/Test3DObjs.cpp
    ${CMAKE_SOURCE_DIR}/Source/Test/TestPipeLine.cpp
)

list(APPEND PipelineTest_Rsc
    ${CMAKE_SOURCE_DIR}/Source/Shell/WinShell.rc
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${PipelineTest_Inc} ${PipelineTest_Src} ${PipelineTest_Rsc} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tests)
