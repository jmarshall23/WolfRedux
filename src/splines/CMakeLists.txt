cmake_minimum_required(VERSION 3.15)
project(splines VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Define source files for the static library
set(SOURCE_FILES
    math_angles.cpp
    math_matrix.cpp
    math_quaternion.cpp
    math_vector.cpp
    q_parse.cpp
    q_shared.cpp
    splines.cpp
    util_str.cpp
)

# Add static library target
add_library(splines STATIC ${SOURCE_FILES})

# Specify preprocessor definitions and other compiler options per configuration
target_compile_definitions(splines PRIVATE
    $<$<CONFIG:Debug>:WIN32;_DEBUG;_LIB;_CRT_SECURE_NO_WARNINGS>
    $<$<CONFIG:Release>:WIN32;NDEBUG;_LIB;_CRT_SECURE_NO_WARNINGS>
)

target_compile_options(splines PRIVATE
    $<$<CONFIG:Debug>:/MTd /Od /Zi>
    $<$<CONFIG:Release>:/MT /O2>
)

# Set properties for different configurations
set_target_properties(splines PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
)