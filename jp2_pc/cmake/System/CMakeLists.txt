project(System)

list(APPEND System_Inc
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/BitBuffer.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/W95/Com.hpp
    ${CMAKE_SOURCE_DIR}/Source/GblInc/Config.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/ConIO.hpp
    ${CMAKE_SOURCE_DIR}/source/lib/control/Control.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/DebugConsole.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/Errors.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/ExePageModify.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/FastHeap.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/FileMapping.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/FixedHeap.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/LRU.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/MemoryLog.hpp
    ${CMAKE_SOURCE_DIR}/Source/lib/sys/performancecount.hpp
    ${CMAKE_SOURCE_DIR}/Source/lib/sys/ProcessorDetect.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/Profile.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/reg.h
    ${CMAKE_SOURCE_DIR}/Source/lib/sys/reginit.hpp
    ${CMAKE_SOURCE_DIR}/Source/lib/sys/w95/render.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/Scheduler.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/StdDialog.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/ThreadControl.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Sys/VirtualMem.hpp
    ${CMAKE_SOURCE_DIR}/Source/shell/winrendertools.hpp
)

list(APPEND System_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/BitBuffer.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/W95/Com.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/ConIO.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/control/Control.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/DebugConsole.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/W95/errors.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/W95/ExePageModify.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/W95/FastHeap.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/W95/FileEx.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/FileMapping.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/FixedHeap.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/LRU.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/MemoryLog.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/PerformanceCount.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/ProcessorDetect.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/Profile.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/reg.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/RegInit.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/W95/Render.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/Scheduler.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/StdDialog.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/W95/textout.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/ThreadControl.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/W95/timer.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Sys/VirtualMem.cpp
    ${CMAKE_SOURCE_DIR}/Source/Shell/WinRenderTools.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${System_Inc} ${System_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Util)
