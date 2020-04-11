project(Math)

list(APPEND Math_Inc
    ${CMAKE_SOURCE_DIR}/source/Lib/Math/FastInverse.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Math/FastSqrt.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Math/FastTrig.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Math/FloatDef.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Math/FloatTable.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Math/MathUtil.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Transform/Matrix.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/Matrix2.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Transform/Presence.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Transform/Rotate.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/Scale.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/Shear.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/Transform.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/TransformIO.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/Translate.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/TransLinear.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/Vector.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/VectorRange.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Types/BigFixed.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Types/FixedP.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Types/P5/FixedPEx.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Math/FloatShort.hpp
)

list(APPEND Math_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/Math/FastInverse.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Math/FastSqrt.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Math/FastTrig.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/Matrix.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/Presence.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Transform/Rotate.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
    ${CMAKE_SOURCE_DIR}
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${Math_Inc} ${Math_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Util)
