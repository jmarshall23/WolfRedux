cmake_minimum_required(VERSION 3.15)
project(ui VERSION 1.0 LANGUAGES C CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/../game
    ${CMAKE_SOURCE_DIR}/../../main/ui
    ${CMAKE_SOURCE_DIR}
)

# Source and header files
set(SOURCE_FILES
    ../game/q_math.c
    ../game/q_shared.c
    ui_main.cpp
)

# Create the DLL target with the DEF file
add_library(ui SHARED ${SOURCE_FILES} ui.def )

# Add preprocessor definitions
target_compile_definitions(ui PRIVATE
    $<$<CONFIG:Debug>:_DEBUG;_USRDLL;UI_EXPORTS>
    WIN32
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS
)

# Set properties for different configurations
set_target_properties(ui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../../
)

# Link against required libraries
target_link_libraries(ui PRIVATE odbc32.lib odbccp32.lib opengl32.lib)