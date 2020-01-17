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
set(CUSTOM_CFLAGS
        -DVERSION_MAJOR="${VERSION_MAJOR}"
        -DVERSION_MINOR="${VERSION_MINOR}"
        -D__RETRODREAM__ -D_NO_VIDEO_H)
set(CUSTOM_LDFLAGS)

if (PLATFORM_DREAMCAST)
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
            src/ds/src/irq/exceptions.c
            src/ds/src/irq/setjmp.s
            src/ds/src/utils/debug_console.c
            src/ds/src/utils/memcpy.S
            src/ds/src/utils/memmove.S
            src/ds/src/utils/memset.S
            src/ds/src/module.c
            )
    list(APPEND INCLUDES src/ds/include src/ds/include/fatfs)
endif ()

#############
# executable
#############
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_compile_options(${PROJECT_NAME} PRIVATE ${CUSTOM_CFLAGS})
target_link_libraries(${PROJECT_NAME} cross2d ${CUSTOM_LDFLAGS})

#############
# targets
#############
include(${CMAKE_SOURCE_DIR}/libcross2d/cmake/targets.cmake)
