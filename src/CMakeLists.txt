cmake_minimum_required(VERSION 3.15)
project(WolfSP VERSION 1.0 LANGUAGES C)

# Use the specified Windows target platform version
set(CMAKE_SYSTEM_VERSION 10.0)

# Add referenced projects
add_subdirectory(botlib)
add_subdirectory(botutil)
add_subdirectory(cgame)
add_subdirectory(game)
add_subdirectory(renderer)
add_subdirectory(splines)
add_subdirectory(ui)

# Define source files for the main application
set(SOURCE_FILES
    client/cl_cgame.c
    client/cl_cin.c
    client/cl_console.c
    client/cl_input.c
    client/cl_keys.c
    client/cl_main.c
    client/cl_net_chan.c
    client/cl_parse.c
    client/cl_scrn.c
    client/cl_ui.c
    client/snd_adpcm.c
    client/snd_dma.c
    client/snd_mem.c
    client/snd_mix.c
    client/snd_wavelet.c
    game/q_math.c
    game/q_shared.c
    qcommon/cmd.c
    qcommon/cm_load.c
    qcommon/cm_patch.c
    qcommon/cm_polylib.c
    qcommon/cm_test.c
    qcommon/cm_trace.c
    qcommon/common.c
    qcommon/cvar.c
    qcommon/files.c
    qcommon/huffman.c
    qcommon/md4.c
    qcommon/msg.c
    qcommon/net_chan.c
    qcommon/unzip.c
    server/sv_bot.c
    server/sv_ccmds.c
    server/sv_client.c
    server/sv_game.c
    server/sv_init.c
    server/sv_main.c
    server/sv_net_chan.c
    server/sv_snapshot.c
    server/sv_world.c
    win32/win_input.c
    win32/win_main.c
    win32/win_net.c
    win32/win_shared.c
    win32/win_snd.c
    win32/win_syscon.c
    win32/win_wndproc.c
    ft2/ahangles.c
    ft2/ahglobal.c
    ft2/ahglyph.c
    ft2/ahhint.c
    ft2/ahmodule.c
    ft2/ahoptim.c
    ft2/ftcalc.c
    ft2/ftdebug.c
    ft2/ftextend.c
    ft2/ftglyph.c
    ft2/ftgrays.c
    ft2/ftinit.c
    ft2/ftlist.c
    ft2/ftmm.c
    ft2/ftnames.c
    ft2/ftobjs.c
    ft2/ftoutln.c
    ft2/ftraster.c
    ft2/ftrend1.c
    ft2/ftsmooth.c
    ft2/ftstream.c
    ft2/ftsystem.c
    ft2/sfdriver.c
    ft2/sfobjs.c
    ft2/ttcmap.c
    ft2/ttdriver.c
    ft2/ttgload.c
    ft2/ttinterp.c
    ft2/ttload.c
    ft2/ttobjs.c
    ft2/ttpload.c
    ft2/ttpost.c
    ft2/ttsbit.c

    # Add more files as needed
)

# Create executable
add_executable(WolfSP WIN32 ${SOURCE_FILES})

# Link against Windows libraries and referenced projects
target_link_libraries(WolfSP PRIVATE winmm.lib wsock32.lib opengl32.lib botlib botutil cgame game renderer splines ui)

# Set properties for different configurations
set_target_properties(WolfSP PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../../
)