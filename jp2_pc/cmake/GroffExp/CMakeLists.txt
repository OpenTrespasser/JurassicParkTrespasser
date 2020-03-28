project(GroffExp)

list(APPEND GroffExp_Inc
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Bitmap.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/EasyString.hpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Export.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/FileIO.hpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Geometry.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/Groff.hpp
    ${CMAKE_SOURCE_DIR}/source/Tools/GroffExp/GUIInterface.hpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Mathematics.hpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/ObjectDef.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/ObjectHandle.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/SmartBuffer.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/Symtab.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/SysLog.hpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/GroffExp.h
)

list(APPEND GroffExp_Rsc
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/GroffExp.def
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/GroffExp.rc
)

list(APPEND GroffExp_Src
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Bitmap.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/EasyString.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Export.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/FileIO.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Geometry.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/Groff.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/GroffExp.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/GUIInterface.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/Mathematics.cpp
    ${CMAKE_SOURCE_DIR}/Source/Tools/GroffExp/ObjectDef.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/ObjectHandle.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/SmartBuffer.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/Symtab.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/SysLog.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
    ${CMAKE_SOURCE_DIR}/Inc/maxsdk
)

add_common_options()

add_library(${PROJECT_NAME} SHARED ${GroffExp_Inc} ${GroffExp_Src} ${GroffExp_Rsc} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Tools)

#target_link_libraries(${PROJECT_NAME}
#TODO add dependencies here
#)
