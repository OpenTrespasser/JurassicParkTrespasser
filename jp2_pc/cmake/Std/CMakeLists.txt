project(Std)

list(APPEND Std_Inc
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/Array.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/Array2.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/ArrayAllocator.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/ArrayIO.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/BlockAllocator.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/BlockArray.hpp
    ${CMAKE_SOURCE_DIR}/Source/GblInc/buildver.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/CircularList.hpp
    ${CMAKE_SOURCE_DIR}/Source/GblInc/common.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/CRC.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Std/Hash.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Std/InitSys.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/LocalArray.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Std/Mem.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/MemLimits.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/PrivSelf.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Std/Ptr.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Std/Random.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/RangeVar.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/Set.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/Sort.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/SparseArray.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/StdLibEx.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Std/StringEx.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/TreeList.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/UAssert.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/UDefs.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/UTypes.hpp
    ${CMAKE_SOURCE_DIR}/Source/GblInc/Warnings.hpp
    ${CMAKE_SOURCE_DIR}/Source/GblInc/Version.hpp
    ${CMAKE_SOURCE_DIR}/Source/GblInc/VerBones.hpp
    ${CMAKE_SOURCE_DIR}/Source/GblInc/3DX.hpp
)

list(APPEND Std_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/Hash.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/InitSys.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/Mem.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/Ptr.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/Random.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Std/stringex.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${Std_Inc} ${Std_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Util)
