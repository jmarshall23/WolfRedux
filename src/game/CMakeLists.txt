cmake_minimum_required(VERSION 3.15)
project(game VERSION 1.0.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define sources and headers
set(CPP_SOURCE_FILES
    ai_cast.cpp
    ai_cast_characters.cpp
    ai_cast_debug.cpp
    ai_cast_dmq3.cpp
    ai_cast_events.cpp
    ai_cast_fight.cpp
    ai_cast_funcs.cpp
    ai_cast_func_attack.cpp
    ai_cast_func_boss1.cpp
    ai_cast_main.cpp
    ai_cast_script.cpp
    ai_cast_script_actions.cpp
    ai_cast_script_ents.cpp
    ai_cast_sight.cpp
    ai_cast_think.cpp
    ai_cast_util.cpp
    bg_animation.cpp
    bg_misc.cpp
    bg_pmove.cpp
    bg_slidemove.cpp
    g_active.cpp
    g_alarm.cpp
    g_bot.cpp
    g_client.cpp
    g_cmds.cpp
    g_combat.cpp
    g_items.cpp
    g_main.cpp
    g_mem.cpp
    g_misc.cpp
    g_missile.cpp
    g_mover.cpp
    g_props.cpp
    g_save.cpp
    g_script.cpp
    g_script_actions.cpp
    g_session.cpp
    g_spawn.cpp
    g_svcmds.cpp
    g_syscalls.cpp
    g_target.cpp
    g_team.cpp
    g_tramcar.cpp
    g_trigger.cpp
    g_utils.cpp
    g_weapon.cpp
    game.def
)

set(C_SOURCE_FILES
    q_math.c
    q_shared.c
)

# Include directories for header files
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/../botai
)

# Define the library target
add_library(${PROJECT_NAME} SHARED ${CPP_SOURCE_FILES} ${C_SOURCE_FILES})

# Set source files properties
set_source_files_properties(${C_SOURCE_FILES} PROPERTIES LANGUAGE C)

# Define preprocessor definitions and compile options
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    WIN32
    _WINDOWS
    GAMEDLL
    _CRT_SECURE_NO_WARNINGS
)

# Specify library dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE winmm.lib)

# Set output directory for binaries
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../../
)

