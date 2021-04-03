project(LibSmacker)

list(APPEND LibSmacker_Inc
    ${CMAKE_SOURCE_DIR}/lib/libsmacker-1.1.1/smacker.h
    ${CMAKE_SOURCE_DIR}/lib/libsmacker-1.1.1/smk_bitstream.h
    ${CMAKE_SOURCE_DIR}/lib/libsmacker-1.1.1/smk_hufftree.h
    ${CMAKE_SOURCE_DIR}/lib/libsmacker-1.1.1/smk_malloc.h
    )

list(APPEND LibSmacker_Src
    ${CMAKE_SOURCE_DIR}/lib/libsmacker-1.1.1/smacker.c
    ${CMAKE_SOURCE_DIR}/lib/libsmacker-1.1.1/smk_bitstream.c
    ${CMAKE_SOURCE_DIR}/lib/libsmacker-1.1.1/smk_hufftree.c
)

add_common_options()

add_library(${PROJECT_NAME} STATIC ${LibSmacker_Inc} ${LibSmacker_Src} )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Lib/Game)
