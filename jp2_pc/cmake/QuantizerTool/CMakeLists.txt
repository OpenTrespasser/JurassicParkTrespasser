project(QuantizerTool)

list(APPEND QuantizerTool_Src
    ${CMAKE_SOURCE_DIR}/Source/Tools/QuantizerTool/QuantizerTool.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/QuantizerTool/QuantizerToolDlg.cpp
)

list(APPEND QuantizerTool_Inc
    ${CMAKE_SOURCE_DIR}/Source/Tools/QuantizerTool/QuantizerTool.h
    ${CMAKE_SOURCE_DIR}/Source/Tools/QuantizerTool/QuantizerToolDlg.h
)

list(APPEND QuantizerTool_Res
    ${CMAKE_SOURCE_DIR}/Source/Tools/QuantizerTool/QuantizerTool.rc
    ${CMAKE_SOURCE_DIR}/Source/Tools/QuantizerTool/res/QuantizerTool.ico
    ${CMAKE_SOURCE_DIR}/Source/Tools/QuantizerTool/res/QuantizerTool.rc2
)

add_common_options()

add_executable(${PROJECT_NAME} WIN32 ${QuantizerTool_Src} ${QuantizerTool_Inc} ${QuantizerTool_Res} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)
