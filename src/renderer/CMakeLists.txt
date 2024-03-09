cmake_minimum_required(VERSION 3.15)
project(renderer VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../ft2
    ../jpeg-6
    ../win32
    ..
    ../../cgame
    ../../game
    ../../qcommon
)

set(SOURCE_FILES
    ../ft2/ahangles.c
    ../ft2/ahglobal.c
    ../ft2/ahglyph.c
    ../ft2/ahhint.c
    ../ft2/ahmodule.c
    ../ft2/ahoptim.c
    ../ft2/ftcalc.c
    ../ft2/ftdebug.c
    ../ft2/ftextend.c
    ../ft2/ftglyph.c
    ../ft2/ftgrays.c
    ../ft2/ftinit.c
    ../ft2/ftlist.c
    ../ft2/ftmm.c
    ../ft2/ftnames.c
    ../ft2/ftobjs.c
    ../ft2/ftoutln.c
    ../ft2/ftraster.c
    ../ft2/ftrend1.c
    ../ft2/ftsmooth.c
    ../ft2/ftstream.c
    ../ft2/ftsystem.c
    ../jpeg-6/jcapimin.c
    ../jpeg-6/jccoefct.c
    ../jpeg-6/jccolor.c
    ../jpeg-6/jcdctmgr.c
    ../jpeg-6/jchuff.c
    ../jpeg-6/jcinit.c
    ../jpeg-6/jcmainct.c
    ../jpeg-6/jcmarker.c
    ../jpeg-6/jcmaster.c
    ../jpeg-6/jcomapi.c
    ../jpeg-6/jcparam.c
    ../jpeg-6/jcphuff.c
    ../jpeg-6/jcprepct.c
    ../jpeg-6/jcsample.c
    ../jpeg-6/jctrans.c
    ../jpeg-6/jdapimin.c
    ../jpeg-6/jdapistd.c
    ../jpeg-6/jdatadst.c
    ../jpeg-6/jdatasrc.c
    ../jpeg-6/jdcoefct.c
    ../jpeg-6/jdcolor.c
    ../jpeg-6/jddctmgr.c
    ../jpeg-6/jdhuff.c
    ../jpeg-6/jdinput.c
    ../jpeg-6/jdmainct.c
    ../jpeg-6/jdmarker.c
    ../jpeg-6/jdmaster.c
    ../jpeg-6/jdpostct.c
    ../jpeg-6/jdsample.c
    ../jpeg-6/jdtrans.c
    ../jpeg-6/jerror.c
    ../jpeg-6/jfdctflt.c
    ../jpeg-6/jidctflt.c
    ../jpeg-6/jmemmgr.c
    ../jpeg-6/jmemnobs.c
    ../jpeg-6/jutils.c
    ../win32/win_gamma.c
    ../win32/win_glimp.c
    ../win32/win_qgl.c
    tr_animation.c
    tr_backend.c
    tr_bsp.c
    tr_cmds.c
    tr_cmesh.c
    tr_curve.c
    tr_flares.c
    tr_font.c
    tr_image.c
    tr_init.c
    tr_light.c
    tr_main.c
    tr_marks.c
    tr_mesh.c
    tr_model.c
    tr_noise.c
    tr_scene.c
    tr_shade.c
    tr_shader.c
    tr_shade_calc.c
    tr_shadows.c
    tr_sky.c
    tr_surface.c
    tr_world.c
    # Additional source files here
)

add_library(renderer STATIC ${SOURCE_FILES})

set_target_properties(renderer PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
)