# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011, RedJack, LLC.
# All rights reserved.
#
# Please see the COPYING file in this distribution for license details.
# ----------------------------------------------------------------------

include_directories(../include)

#-----------------------------------------------------------------------
# Build the library

set(LIBCOPSE_SRC
    libcopse/foo.c
)

add_library(libcopse SHARED ${LIBCOPSE_SRC})
set_target_properties(libcopse PROPERTIES
    OUTPUT_NAME copse
    SOVERSION 0.0.0)
target_link_libraries(libcopse
    ${CORK_LIBRARIES}
)

install(TARGETS libcopse DESTINATION lib)

#-----------------------------------------------------------------------
# Generate the pkg-config file

set(prefix ${CMAKE_INSTALL_PREFIX})
configure_file(copse.pc.in copse.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/copse.pc
        DESTINATION lib/pkgconfig)
