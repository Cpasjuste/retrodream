cmake_minimum_required(VERSION 3.0)
#set(CMAKE_VERBOSE_MAKEFILE ON)

# add libcross2d library
add_subdirectory(libcross2d)

#############
# RetroDream
#############
project(retrodream)

enable_language(ASM)

set(PROJECT_AUTHOR "Cpasjuste")
set(VERSION_MAJOR "3")
set(VERSION_MINOR "0")
set(VERSION_MICRO "0")

#############
# code
#############
file(GLOB SOURCES src/* src/flash/*)
set(INCLUDES src)
set(CFLAGS
        -DVERSION_MAJOR="${VERSION_MAJOR}"
        -DVERSION_MINOR="${VERSION_MINOR}"
        -DVERSION_MICRO="${VERSION_MICRO}"
        -D__RETRODREAM__ -D_NO_VIDEO_H -DMODULE_DEBUG)
set(LDFLAGS)

if (PLATFORM_DREAMCAST)
    # build ds exports
    add_custom_target(
            ${PROJECT_NAME}.exports ALL
            DEPENDS dummy_exports
    )
    add_custom_command(OUTPUT
            dummy_exports ${CMAKE_SOURCE_DIR}/src/ds_wrapper/exports.c ${CMAKE_SOURCE_DIR}/src/ds_wrapper/exports_gcc.c
            COMMAND ${KOS_BASE}/utils/genexports/genexports.sh ${CMAKE_SOURCE_DIR}/src/ds_wrapper/exports.txt ${CMAKE_SOURCE_DIR}/src/ds_wrapper/exports.c ds_symtab
            COMMAND ${KOS_BASE}/utils/genexports/genexports.sh ${CMAKE_SOURCE_DIR}/src/ds_wrapper/exports_gcc.txt ${CMAKE_SOURCE_DIR}/src/ds_wrapper/exports_gcc.c gcc_symtab
            )
    # retrodream ds wrapper
    list(APPEND SOURCES
            src/ds_wrapper/ds_wrapper.c
            src/ds_wrapper/exports.c
            src/ds_wrapper/exports_gcc.c
            )
    # ds vmu
    list(APPEND SOURCES
            src/ds/src/vmu/vmu.c
            )
    # ds fs
    list(APPEND SOURCES
            src/ds/src/drivers/rtc.c
            src/ds/src/drivers/spi.c
            src/ds/src/drivers/sd.c
            src/ds/src/fs/fs.c
            src/ds/src/fs/fat/dc.c
            src/ds/src/fs/fat/ff.c
            src/ds/src/fs/fat/utils.c
            src/ds/src/fs/fat/option/ccsbcs.c
            src/ds/src/fs/fat/option/syscall.c
            src/ds/src/utils/memcpy.S
            src/ds/src/utils/memmove.S
            src/ds/src/utils/memset.S
            )
    # ds loadable module support
    list(APPEND SOURCES
            src/ds/src/irq/exceptions.c
            src/ds/src/irq/setjmp.s
            src/ds/src/module.c
            src/ds/src/utils.c
            # isofs
            src/ds/lib/libcfg/src/cfg+.c
            src/ds/lib/libcfg/src/parse.c
            src/ds/lib/libcfg/src/props.c
            src/ds/lib/libcfg/src/cmdline.c
            src/ds/lib/libcfg/src/cfgfile.c
            src/ds/lib/libcfg/src/shared.c
            src/ds/lib/libcfg/src/platon/str/strplus.c
            src/ds/lib/libcfg/src/platon/str/strdyn.c
            src/ds/lib/libcfg/src/platon/str/strctype.c
            src/ds/lib/libcfg/src/platon/str/dynfgets.c
            )
    list(APPEND CFLAGS
            # isofs (libcfg)
            -DHAVE_LIMITS_H -DHAVE_FLOAT_H -DHAVE_STRING_H -DHAVE_CTYPE_H -DHAVE_MATH_H
            )
    # ds embedded modules (debug) - START
    list(APPEND SOURCES
            # minilzo
            src/ds/modules/minilzo/minilzo.c
            # isofs
            src/ds/modules/isofs/cdi.c
            src/ds/modules/isofs/ciso.c
            src/ds/modules/isofs/fs_iso9660.c
            src/ds/modules/isofs/gdi.c
            src/ds/modules/isofs/map2dev.c
            #src/ds/modules/isofs/module.c
            src/ds_wrapper/isofs_module.c
            # isoldr
            src/ds/modules/isoldr/exec.c
            src/ds/modules/isoldr/execasm.s
            src/ds/modules/isoldr/module.c
            # bflash
            src/ds/modules/bflash/bflash.c
            src/ds/modules/bflash/devices.c
            )
    list(APPEND CFLAGS
            -D__EMBEDDED_MODULE_DEBUG__
            # isofs
            -DVER_MAJOR=0 -DVER_MINOR=6 -DVER_MICRO=7 -DVER_BUILD=0
            )
    # ds embedded modules (debug) - END
    list(APPEND INCLUDES
            src/ds/include
            src/ds/include/minilzo
            src/ds/include/fatfs
            )
    list(APPEND LDFLAGS kallisti)
endif ()

#############
# executable
#############
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_compile_options(${PROJECT_NAME} PRIVATE ${CFLAGS})
target_link_libraries(${PROJECT_NAME} cross2d ${LDFLAGS})
if (PLATFORM_DREAMCAST)
    # for dcload-serial binary size
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS -s)
endif ()
#############
# targets
#############
include(${CMAKE_SOURCE_DIR}/libcross2d/cmake/targets.cmake)
