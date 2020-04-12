project(GeomDBase)

list(APPEND GeomDBase_Inc
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/ClipMesh.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/EntityDBase/Container.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/GeomTypes.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Primitives/IndexT.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/Mesh.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/MeshIterator.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/Occlude.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/Partition.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/PartitionBuild.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/PartitionPriv.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/PartitionSpace.hpp
    ${CMAKE_SOURCE_DIR}/source/GUIApp/PredefinedShapes.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/RayCast.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/RenderType.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/Shape.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/Skeleton.hpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/SkeletonIterator.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/Terrain.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/TerrainLoad.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/TerrainObj.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/TerrainTexture.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/TexturePageManager.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/WaterQuadTree.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/WaveletConv.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/WaveletDataFormat.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/WaveletQuadTree.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/WaveletQuadTreeBase.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/WaveletQuadTreeBaseRecalc.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/WaveletQuadTreeBaseTri.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/WaveletQuadTreeQuery.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/WaveletQuadTreeTForm.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/WaveletStaticData.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/InvisibleShape.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/LightShape.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/LineSegment.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/PartitionBuildBase.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/PartitionPrivClass.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/Plane.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/PlaneAxis.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/GeomDBase/WaveletCoef.hpp
    ${CMAKE_SOURCE_DIR}/source/Lib/Renderer/GeomTypesPriv.hpp
)

list(APPEND GeomDBase_Src
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/ClipMesh.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/GeomTypes.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/GeomTypesBox.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/GeomTypesCamera.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/GeomTypesPlane.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/GeomTypesPoly.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/GeomTypesSphere.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/Hull.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/Occlude.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/Partition.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/PartitionBuild.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/PartitionPriv.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/PartitionSpace.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/PlaneDef.cpp
    ${CMAKE_SOURCE_DIR}/Source/GUIApp/PreDefinedShapes.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/RayCast.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/Renderer/RenderType.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/Shape.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/Skeleton.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/Terrain.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/TerrainLoad.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/TerrainObj.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/TerrainTexture.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/TexturePageManager.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/WaterQuadTree.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/WaveletConv.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/WaveletDataFormat.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/WaveletQuadTree.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/WaveletQuadTreeBase.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/WaveletQuadTreeBaseRecalc.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/WaveletQuadTreeBaseTri.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/WaveletQuadTreeQuery.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/WaveletQuadTreeTForm.cpp
    ${CMAKE_SOURCE_DIR}/Source/Lib/GeomDBase/WaveletStaticData.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Source/gblinc
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${GeomDBase_Inc} ${GeomDBase_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Game)
