project(EntityDBase)

list(APPEND EntityDBase_Inc
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Animal.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Animate.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/AnimationScript.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/CameraPrime.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Entity.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/EntityLight.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/FrameHeap.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Instance.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Message.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/MessageLog.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/MovementPrediction.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgAudio.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgCollision.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgControl.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgCreate.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgDelete.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgMagnet.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgMove.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgPaint.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgPhysicsReq.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgStep.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgSystem.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/MsgTrigger.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/ParticleGen.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query/QAI.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query/QMessage.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query/QPhysics.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query/QRenderer.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query/QSolidObject.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Query/QSubsystem.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query/QTerrain.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query/QTerrainObj.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query/QTriggers.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/QualitySettings.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Query.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/QueueMessage.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query/QWater.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/RenderDB.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Replay.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Teleport.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Water.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/WorldDBase.hpp    
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/FilterIterator.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/MessageTypes.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Instancer.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/Subsystem.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/WorldPriv.hpp
)

list(APPEND EntityDBase_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Animal.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Animate.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/AnimationScript.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/CameraPrime.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Entity.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/EntityLight.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/FrameHeap.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Instance.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Message.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageLog.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MessageTypes.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/MovementPrediction.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/ParticleGen.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query/QSubsystem.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query/QTerrain.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/QualitySettings.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Query.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/QueueMessage.cpp
    ${CMAKE_SOURCE_DIR}/source/Lib/EntityDBase/RenderDB.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Replay.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Teleport.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Water.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/WorldDBase.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/WorldPort.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${EntityDBase_Inc} ${EntityDBase_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Game)
