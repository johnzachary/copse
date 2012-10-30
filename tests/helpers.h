/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef TESTS_HELPERS_H
#define TESTS_HELPERS_H

#include <stdlib.h>

#define DESCRIBE_TEST \
    fprintf(stderr, "--- %s\n", __func__);

#define container_of(field, struct_type, field_name) \
    ((struct_type *) (- offsetof(struct_type, field_name) + \
                      (void *) (field)))

#define fail_unless_equal(what, format, expected, actual) \
    (fail_unless((expected) == (actual), \
                 "%s not equal (expected " format \
                 ", got " format ")", \
                 (what), (expected), (actual)))


#endif /* TESTS_HELPERS_H */
