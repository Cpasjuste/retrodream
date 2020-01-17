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
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")

#############
# code
#############
file(GLOB SOURCES src/*)
set(INCLUDES src)
set(CFLAGS
        -DVERSION_MAJOR="${VERSION_MAJOR}"
        -DVERSION_MINOR="${VERSION_MINOR}"
        -D__RETRODREAM__ -D_NO_VIDEO_H)
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
            src/ds_wrapper/exports_gcc.c)
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
            )
    list(APPEND INCLUDES
            src/ds/include
            src/ds/include/fatfs
            )
    list(APPEND LDFLAGS kosutils)
endif ()

#############
# executable
#############
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_compile_options(${PROJECT_NAME} PRIVATE ${CFLAGS})
target_link_libraries(${PROJECT_NAME} cross2d ${LDFLAGS})

#############
# targets
#############
include(${CMAKE_SOURCE_DIR}/libcross2d/cmake/targets.cmake)
