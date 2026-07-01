cmake_minimum_required(VERSION 3.10)
project(z-ed C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${PROJECT_SOURCE_DIR}/src)

find_library(DL_LIBRARY dl)

add_library(lib SHARED src/lib.c)
if(DL_LIBRARY)
    target_link_libraries(lib PRIVATE ${DL_LIBRARY})
endif()

add_library(comm SHARED src/comm.c)
target_link_libraries(comm PRIVATE lib)

add_executable(main src/commproc.c)
target_link_libraries(main PRIVATE lib)
if(DL_LIBRARY)
    target_link_libraries(main PRIVATE ${DL_LIBRARY})
endif()

set_target_properties(lib comm main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    BUILD_RPATH "$ORIGIN"
)
