project(QuantizerToolCLI)

list(APPEND QuantizerToolCLI_Src
    ${CMAKE_SOURCE_DIR}/Source/Tools/QuantizerTool/QuantMain.c
    ${CMAKE_SOURCE_DIR}/Source/Tools/QuantizerTool/NEUQUANT.C
)

add_common_options()

add_executable(${PROJECT_NAME} ${QuantizerToolCLI_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)
