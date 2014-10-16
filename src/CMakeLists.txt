# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011, RedJack, LLC.
# All rights reserved.
#
# Please see the COPYING file in this distribution for license details.
# ----------------------------------------------------------------------

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#-----------------------------------------------------------------------
# Context implementations

include(CheckCSourceCompiles)
include(CheckTypeSize)

# Address model

if (NOT COPSE_CONTEXT)
    message(STATUS "Detecting context implementation")
    try_run(
        COPSE_CONTEXT_RAN COPSE_CONTEXT_COMPILED
        ${CMAKE_CURRENT_BINARY_DIR}/detect/context
        ${CMAKE_CURRENT_SOURCE_DIR}/detect/context.c
        CMAKE_FLAGS -DINCLUDE_DIRECTORIES:STRING=${CMAKE_SOURCE_DIR}/include
        COMPILE_OUTPUT_VARIABLE COPSE_CONTEXT_COMPILE_RESULT
        RUN_OUTPUT_VARIABLE COPSE_CONTEXT
    )
    if (NOT COPSE_CONTEXT_COMPILED)
        message(
            FATAL_ERROR
            "Could not detect context implementation: ${COPSE_CONTEXT_COMPILE_RESULT}"
        )
    endif (NOT COPSE_CONTEXT_COMPILED)
endif (NOT COPSE_CONTEXT)

enable_language(ASM)
message(STATUS "Using ${COPSE_CONTEXT} context implementation")
set(LIBCOPSE_CONTEXT_SRC
    libcopse/context/jump_${COPSE_CONTEXT}
    libcopse/context/make_${COPSE_CONTEXT}
)

# Work around CMake 2.6's experimental assembly support
if ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" STREQUAL "2.6")
    if (CMAKE_ASM_COMPILER MATCHES "as$")
        set(CMAKE_ASM${ASM_DIALECT}_COMPILE_OBJECT
            "<CMAKE_ASM${ASM_DIALECT}_COMPILER> -o <OBJECT> <SOURCE>")
    else (CMAKE_ASM_COMPILER MATCHES "as$")
        set(CMAKE_ASM${ASM_DIALECT}_COMPILE_OBJECT
            "<CMAKE_ASM${ASM_DIALECT}_COMPILER> -o <OBJECT> -c <SOURCE>")
    endif (CMAKE_ASM_COMPILER MATCHES "as$")
endif ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" STREQUAL "2.6")


#-----------------------------------------------------------------------
# Build the library

set(LIBCOPSE_SRC
    libcopse/context.c
    libcopse/cps.c
    libcopse/fiber.c
    libcopse/round-robin.c
    ${LIBCOPSE_CONTEXT_SRC}
)

# Update the VERSION and SOVERSION properties below according to the following
# rules (taken from [1]):
#
# VERSION = current.revision.age
#
#   1. Start with a VERSION of `0.0.0` for each shared library.
#   2. Update VERSION only immediately before a public release of your software.
#      More frequent updates are unnecessary, and only guarantee that the
#      current interface number gets larger faster.
#   3. If the library source code has changed at all since the last update, then
#      increment `revision` (`c.r.a` becomes `c.r+1.a`).
#   4. If any interfaces have been added, removed, or changed since the last
#      update, increment `current`, and set `revision` to 0.
#   5. If any interfaces have been added since the last public release, then
#      increment `age`.
#   6. If any interfaces have been removed or changed since the last public
#      release, then set `age` to 0.
#
# SOVERSION should always equal `current`.
#
# Note that changing `current` means that you are releasing a new
# backwards-incompatible version of the library.  This has implications on
# packaging, so once an API has stabilized, these should be a rare occurrence.
#
# [1] http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html#Updating-version-info

add_library(libcopse SHARED ${LIBCOPSE_SRC})
set_target_properties(libcopse PROPERTIES
    OUTPUT_NAME copse
    VERSION 0.1.0
    SOVERSION 0)
target_link_libraries(libcopse
    ${CORK_LIBRARIES}
)

install(TARGETS libcopse DESTINATION ${CMAKE_INSTALL_LIBDIR})

#-----------------------------------------------------------------------
# Generate the pkg-config file

set(prefix ${CMAKE_INSTALL_PREFIX})
configure_file(copse.pc.in ${CMAKE_CURRENT_BINARY_DIR}/copse.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/copse.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
