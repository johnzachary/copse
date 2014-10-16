/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef TESTS_HELPERS_H
#define TESTS_HELPERS_H

#include <stdio.h>
#include <libcork/core.h>

#define DESCRIBE_TEST \
    fprintf(stderr, "--- %s\n", __func__);


/*-----------------------------------------------------------------------
 * Allocators
 */

/* Use a custom allocator that debugs every allocation. */

static void
setup_allocator(void)
{
    struct cork_alloc  *debug = cork_debug_alloc_new(cork_allocator);
    cork_set_allocator(debug);
}


/*-----------------------------------------------------------------------
 * Error reporting
 */

#define fail_if_error(call) \
    do { \
        call; \
        if (cork_error_occurred()) { \
            fail("%s", cork_error_message()); \
        } \
    } while (0)

#define fail_unless_error(call, ...) \
    do { \
        call; \
        if (!cork_error_occurred()) { \
            fail(__VA_ARGS__); \
        } else { \
            print_expected_failure(); \
        } \
        cork_error_clear(); \
    } while (0)

#define fail_unless_equal(what, format, expected, actual) \
    (fail_unless((expected) == (actual), \
                 "%s not equal (expected " format \
                 ", got " format ")", \
                 (what), (expected), (actual)))


#endif /* TESTS_HELPERS_H */
