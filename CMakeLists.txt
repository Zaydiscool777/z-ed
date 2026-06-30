cmake_minimum_required(VERSION 3.10)
project(z-ed C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_library(comm SHARED src/comm.c #[[src/lib.c]])

add_executable(main src/commproc.c src/lib.c)
# add_executable(main src/test.c src/lib.c)

find_library(DL_LIBRARY dl)
if(DL_LIBRARY)
    target_link_libraries(main PRIVATE ${DL_LIBRARY})
endif()

set_target_properties(comm main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
