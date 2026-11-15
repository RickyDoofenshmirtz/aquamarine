file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(aquamarine_src_main ${SRC_FILES})

target_link_libraries(aquamarine_src_main PRIVATE lib_aqua)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(aquamarine_src_main PRIVATE stdc++exp)
    message(STATUS "using g++, linking stdc++exp")
endif()