project(Audio)

list(APPEND Audio_Inc
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/Audio.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/AudioADPCM.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/AudioDaemon.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/AudioLoader.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/AudioPCM.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/AudioVOICE.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/SoundDefs.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/SoundTypes.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/eax.h
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/Ia3d.h
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/Material.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/Subtitle.hpp
)

list(APPEND Audio_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/Audio.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/AudioADPCM.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/AudioDaemon.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/AudioLoad.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/Material.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Audio/Sample.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${Audio_Inc} ${Audio_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Game)
