/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <check.h>

#include "copse/cps.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Test continuations
 */

struct save_int {
    struct cps_cont  parent;
    unsigned int  *dest;
    unsigned int  value;
};

static int
save_int__resume(struct cps_cont *cont, struct cps_cont *next)
{
    struct save_int  *self = container_of(cont, struct save_int, parent);
    *self->dest = self->value;
    return cps_done(next);
}

#define SAVE_INT_INIT(d, v)  { { save_int__resume }, (d), (v) }


/*-----------------------------------------------------------------------
 * Simple continuation passing
 */

START_TEST(test_cps_01)
{
    DESCRIBE_TEST;
    unsigned int  result = 0;
    struct save_int  i = SAVE_INT_INIT(&result, 10);

    fail_if(cps_run(&i.parent), "Error running continuations");
    fail_unless_equal("Continuation result", "%u", 10, result);
}
END_TEST

START_TEST(test_cps_02)
{
    DESCRIBE_TEST;
    unsigned int  result1 = 0;
    unsigned int  result2 = 0;
    struct save_int  i1 = SAVE_INT_INIT(&result1, 10);
    struct save_int  i2 = SAVE_INT_INIT(&result2, 20);

    fail_if(cps_resume(&i1.parent, &i2.parent), "Error running continuations");
    fail_unless_equal("Continuation #1 result", "%u", 10, result1);
    fail_unless_equal("Continuation #2 result", "%u", 20, result2);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("cps");

    TCase  *tc_cps = tcase_create("cps");
    tcase_add_test(tc_cps, test_cps_01);
    tcase_add_test(tc_cps, test_cps_02);
    suite_add_tcase(s, tc_cps);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = test_suite();
    SRunner  *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}

