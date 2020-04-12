project(AI)

list(APPEND AI_Inc
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Activity.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/ActivityAttack.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/ActivityCompound.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/ActivityPhased.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/ActivityVocal.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/AIGraph.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/AIInfo.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/AIMain.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/AITypes.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/AStar.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Brain.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/EmotionActivities.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/HeadActivities.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Influence.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/MentalState.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/MoveActivities.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/NodeHistory.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/NodeSource.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/PathAStar.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/PathAvoider.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Pathfinder.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Port.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Silhouette.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Synthesizer.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/TestActivities.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/WorldView.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/ActivityEnum.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Classes.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Feeling.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Graph.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Modifier.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/ActivityEnum.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Rating.hpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/PathFailure.hpp
)

list(APPEND AI_Src
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Activity.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/ActivityAttack.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/ActivityCompound.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/ActivityPhased.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/ActivityVocal.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/AIGraph.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/AIMain.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Brain.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/EmotionActivities.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/HeadActivities.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Influence.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/MentalState.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/MoveActivities.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/NodeHistory.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/NodeSource.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/PathAStar.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/PathAvoider.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Pathfinder.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Port.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Silhouette.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/Synthesizer.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/TestActivities.cpp
    ${CMAKE_SOURCE_DIR}/Source/Game/AI/WorldView.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${AI_Inc} ${AI_Src} )
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Game)
