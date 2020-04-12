project(Game)

list(APPEND Game_Inc
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/Action.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/DesignDaemon/BloodSplat.hpp
    ${CMAKE_SOURCE_DIR}/source/Game/DesignDaemon/Daemon.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/DesignDaemon/DaemonScript.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/ExpressionEvaluate.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/GameActions.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/GameLoop.hpp
    ${CMAKE_SOURCE_DIR}/source/Game/DesignDaemon/Gun.hpp
    ${CMAKE_SOURCE_DIR}/source/Game/DesignDaemon/HitSpang.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/DesignDaemon/Player.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/DesignDaemon/PlayerSettings.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/TextOverlay.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/Trigger.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/DesignDaemon/Socket.hpp
)

list(APPEND Game_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/Action.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/DesignDaemon/BloodSplat.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/BooleanTrigger.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/CollisionTrigger.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/CreatureTrigger.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/DesignDaemon/Daemon.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/ExpressionEvaluate.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/GameActions.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/GameLoop.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/DesignDaemon/Gun.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/DesignDaemon/HitSpang.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/LocationTrigger.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/MagnetTrigger.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/MoreMassTrigger.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/ObjectTrigger.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/DesignDaemon/Player.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/SequenceTrigger.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/DesignDaemon/Socket.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/StartTrigger.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/TextOverlay.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/TimerTrigger.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/TriggerBase.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Trigger/VariableTrigger.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${Game_Inc} ${Game_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Game)
