cmake_minimum_required(VERSION 3.9)
project(kiss-ocl LANGUAGES CXX VERSION "1.0.0" DESCRIPTION "Make OpenCL more simple to use!")
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
    set(KISSOCL_BUILD_SAMPLES ON)
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(include)


add_subdirectory(external/SDL2)
include_directories(external/SDL2/include)

add_subdirectory(external/miss-ocl)
include_directories(external/miss-ocl/include)

add_subdirectory(src)

if (${KISSOCL_BUILD_SAMPLES})
    add_subdirectory(app)
endif ()