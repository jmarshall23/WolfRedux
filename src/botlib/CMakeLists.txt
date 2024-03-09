cmake_minimum_required(VERSION 3.15)
project(botlib VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define preprocessor definitions
add_compile_definitions(
    $$<$$<CONFIG:Debug>:WIN32;_DEBUG;_LIB;BOTLIB;DEBUG;_CRT_SECURE_NO_WARNINGS>
    $$<$$<CONFIG:Release>:WIN32;NDEBUG;_LIB;BOTLIB;_CRT_SECURE_NO_WARNINGS>
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/../game
)

# Source files
set(SOURCE_FILES
    be_aas_bspq3.c
    be_aas_cluster.c
    be_aas_debug.c
    be_aas_entity.c
    be_aas_file.c
    be_aas_main.c
    be_aas_move.c
    be_aas_optimize.c
    be_aas_reach.c
    be_aas_route.c
    be_aas_routealt.c
    be_aas_routetable.c
    be_aas_sample.c
    be_ai_goal.c
    be_ai_move.c
    be_ea.c
    be_interface.c
)

# Header files
set(HEADER_FILES
    ../game/be_aas.h
    ../game/be_ai_gen.h
    ../game/be_ai_goal.h
    ../game/be_ai_move.h
    ../game/be_ea.h
    ../game/botlib.h
    ../game/q_shared.h
    ../game/surfaceflags.h
    aasfile.h
    be_aas_bsp.h
    be_aas_cluster.h
    be_aas_debug.h
    be_aas_def.h
    be_aas_entity.h
    be_aas_file.h
    be_aas_funcs.h
    be_aas_main.h
    be_aas_move.h
    be_aas_optimize.h
    be_aas_reach.h
    be_aas_route.h
    be_aas_routealt.h
    be_aas_routetable.h
    be_aas_sample.h
    be_interface.h
)

# Add static library target
add_library(botlib STATIC ${SOURCE_FILES} ${HEADER_FILES})

# Compiler options
target_compile_options(botlib PRIVATE
    $<$<CONFIG:Debug>:/MTd /W3 /Zi /Od /RTC1 /D_DEBUG>
    $<$<CONFIG:Release>:/MT /O2 /Ob2 /DNDEBUG>
)

# Set properties for different configurations
set_target_properties(botlib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
)