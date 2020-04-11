project(Loader)

list(APPEND Loader_Inc
    ${CMAKE_SOURCE_DIR}/source/Lib/Loader/ASyncLoader.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/DataDaemon.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/EasyString.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/Fetchable.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/FileIO.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/Groff.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/GroffIO.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/GroffLoader.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Loader/ImageLoader.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Loader/Loader.hpp
    ${CMAKE_SOURCE_DIR}/Source/lib/loader/loadtexture.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/ObjectHandle.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Loader/PlatonicInstance.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Loader/PVA.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Loader/SaveFile.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/SmartBuffer.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/SymbolTable.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/Symtab.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/SysLog.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Loader/TextureManager.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Loader/TexturePackSurface.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/ValueTable.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Loader/SaveBuffer.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Groff/VTParse.hpp
)

list(APPEND Loader_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/AsyncLoader.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/DataDaemon.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/EasyString.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/FileIO.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/Groff.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/GroffIO.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/GroffLoader.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/ImageLoader.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/Loader.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/LoadTexture.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/ObjectHandle.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/PlatonicInstance.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/PVA.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/SaveFile.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/SmartBuffer.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/SymbolTable.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/Symtab.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/SysLog.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/TextureManager.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Loader/TexturePackSurface.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Groff/ValueTable.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${Loader_Inc} ${Loader_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Util)
