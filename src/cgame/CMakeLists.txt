cmake_minimum_required(VERSION 3.15)
project(cgame VERSION 1.0 LANGUAGES C CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add definitions
add_compile_definitions($<$<CONFIG:DEBUG>:_DEBUG> WIN32 _WINDOWS CGAMEDLL _CRT_SECURE_NO_WARNINGS)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/../game ${CMAKE_SOURCE_DIR}/../ui ${CMAKE_SOURCE_DIR}/../../main/ui)

# Source files
set(SOURCE_FILES
    ../game/bg_animation.cpp
    ../game/bg_misc.cpp
    ../game/bg_pmove.cpp
    ../game/bg_slidemove.cpp
    ../game/q_math.c
    ../game/q_shared.c
    cg_consolecmds.cpp
    cg_draw.cpp
    cg_drawtools.cpp
    cg_effects.cpp
    cg_ents.cpp
    cg_event.cpp
    cg_flamethrower.cpp
    cg_info.cpp
    cg_localents.cpp
    cg_main.cpp
    cg_marks.cpp
    cg_particles.cpp
    cg_players.cpp
    cg_playerstate.cpp
    cg_predict.cpp
    cg_scoreboard.cpp
    cg_servercmds.cpp
    cg_snapshot.cpp
    cg_sound.cpp
    cg_syscalls.cpp
    cg_trails.cpp
    cg_view.cpp
    cg_weapons.cpp
    cgame.def
)

# Add the dynamic library target
add_library(cgame SHARED ${SOURCE_FILES})

# Link against additional libraries
target_link_libraries(cgame PRIVATE odbc32.lib odbccp32.lib)

# Set the output directory for the DLL and import library
set_target_properties(cgame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../../
)
