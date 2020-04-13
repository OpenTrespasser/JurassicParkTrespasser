cmake_minimum_required(VERSION 3.17)
project(JP2_PC)

set(CMAKE_SUPPRESS_REGENERATION true) #Suppress zero-check project
set(CMake_MSVC_PARALLEL true GLOBAL)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT trespass)
string(APPEND CMAKE_CXX_FLAGS " /MP /GF")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

#Linker flag required by the self-modifying assembly code in DrawSubTriangle in project ScreenRenderDWI
#Can be removed after assembly code has been replaced
#ERW = Execute, read, write
string(APPEND CMAKE_EXE_LINKER_FLAGS " /SECTION:SelfMod,ERW")
string(APPEND CMAKE_SHARED_LINKER_FLAGS " /SECTION:SelfMod,ERW")


set(USE_TRESPASSER_DIRECTORY TRUE CACHE BOOL INTERNAL)

get_filename_component(TRESPASS_INSTALL_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\DreamWorks Interactive\\Trespasser;Installed Directory]" ABSOLUTE)

if ( (NOT TRESPASS_INSTALL_DIR STREQUAL "/registry") AND USE_TRESPASSER_DIRECTORY )  #Found and read the registry entry, and it is valid
    set(FOUND_TRESPASS_DIR TRUE)
    set(CMAKE_INSTALL_PREFIX ${TRESPASS_INSTALL_DIR} CACHE STRING INTERNAL FORCE)
else()
    set(FOUND_TRESPASS_DIR FALSE)
endif()

set(CMAKE_CONFIGURATION_TYPES Debug Release Final CACHE STRING INTERNAL FORCE) 

include(cmake/CMakeCommon.cmake)

add_subdirectory(cmake/AI)
add_subdirectory(cmake/AITest)
add_subdirectory(cmake/Audio)
add_subdirectory(cmake/Bugs)
add_subdirectory(cmake/CollisionEditor)
add_subdirectory(cmake/EntityDBase)
add_subdirectory(cmake/Examples)
add_subdirectory(cmake/File)
add_subdirectory(cmake/FileTest)
add_subdirectory(cmake/Game)
add_subdirectory(cmake/GeomDBase)
add_subdirectory(cmake/GroffBuild)
add_subdirectory(cmake/GroffExp)
add_subdirectory(cmake/GUIApp)
add_subdirectory(cmake/InitGUIApp)
add_subdirectory(cmake/InitGUIApp2)
add_subdirectory(cmake/LZSS)
add_subdirectory(cmake/Loader)
add_subdirectory(cmake/Math)
add_subdirectory(cmake/MathTest)
add_subdirectory(cmake/Physics)
add_subdirectory(cmake/PhysicsTest)
add_subdirectory(cmake/PipelineTest)
add_subdirectory(cmake/ProcessorDetect)
add_subdirectory(cmake/QuantizerTool)
add_subdirectory(cmake/QuantizerToolCLI)
add_subdirectory(cmake/Render3D)
add_subdirectory(cmake/ScreenRenderDWI)
add_subdirectory(cmake/Std)
add_subdirectory(cmake/System)
add_subdirectory(cmake/trespass)
add_subdirectory(cmake/TweakNvidia128)
add_subdirectory(cmake/TweakTrespass)
add_subdirectory(cmake/View)
add_subdirectory(cmake/WaveTest)
add_subdirectory(cmake/WinShell)


option(INSTALL_GTEST OFF)
add_subdirectory(gtest)
set_target_properties(gtest PROPERTIES FOLDER "Tests/gtest")
set_target_properties(gtest_main PROPERTIES FOLDER "Tests/gtest")
set_target_properties(gmock PROPERTIES FOLDER "Tests/gtest")
set_target_properties(gmock_main PROPERTIES FOLDER "Tests/gtest")



set(DEACTIVATE_BROKEN_PROJECTS TRUE CACHE BOOL INTERNAL)

if (DEACTIVATE_BROKEN_PROJECTS)
    list(APPEND BROKEN_PROJECTS
        Bugs
        CollisionEditor
        Examples
        InitGUIApp
        ProcessorDetect
        PipelineTest
        QuantizerToolCLI
        TweakTrespass
        WaveTest
    )
    
    foreach(broken ${BROKEN_PROJECTS})
        set_target_properties(${broken} PROPERTIES EXCLUDE_FROM_ALL true)             #When building the ALL_BUILD project
        set_target_properties(${broken} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD true)   #When building the entire solution
    endforeach()
endif()
