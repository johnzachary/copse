/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2013, RedJack, LLC.
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
#include "copse/round-robin.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Test continuations
 */

struct save_int {
    struct cps_cont  *cont;
    unsigned int  *dest;
    unsigned int  value;
    unsigned int  run_count;
};

static void
save_int__resume(void *user_data, struct cps_cont *next)
{
    struct save_int  *self = user_data;
    *self->dest = self->value;
    self->run_count++;
    cps_call(next);
}

static void
save_int_init(struct save_int *self, unsigned int *dest, unsigned int value)
{
    self->cont = cps_cont_new();
    cps_cont_set(self->cont, self, NULL, save_int__resume);
    self->dest = dest;
    self->value = value;
    self->run_count = 0;
}

static void
save_int_done(struct save_int *self)
{
    cps_cont_free(self->cont);
}

static void
save_int_verify(struct save_int *i)
{
    fail_unless_equal("Continuation result", "%u", i->value, *i->dest);
    fail_unless_equal("Continuation run count", "%u", 1, i->run_count);
}


struct save_int2 {
    struct cps_cont  *step1;
    unsigned int  *dest1;
    unsigned int  value1;

    struct cps_cont  *step2;
    unsigned int  *dest2;
    unsigned int  value2;

    unsigned int  run_count;
};

static void
save_int2__step1(void *user_data, struct cps_cont *next)
{
    struct save_int2  *self = user_data;
    *self->dest1 = self->value1;
    self->run_count++;
    cps_resume(next, self->step2);
}

static void
save_int2__step2(void *user_data, struct cps_cont *next)
{
    struct save_int2  *self = user_data;
    *self->dest2 = self->value2;
    self->run_count++;
    cps_call(next);
}

static void
save_int2_init(struct save_int2 *self,
               unsigned int *dest1, unsigned int value1,
               unsigned int *dest2, unsigned int value2)
{
    self->step1 = cps_cont_new();
    cps_cont_set(self->step1, self, NULL, save_int2__step1);
    self->step2 = cps_cont_new();
    cps_cont_set(self->step2, self, NULL, save_int2__step2);
    self->dest1 = dest1;
    self->value1 = value1;
    self->dest2 = dest2;
    self->value2 = value2;
    self->run_count = 0;
}

static void
save_int2_done(struct save_int2 *self)
{
    cps_cont_free(self->step1);
    cps_cont_free(self->step2);
}

static void
save_int2_verify1(struct save_int2 *i)
{
    fail_unless_equal("Continuation result #1", "%u", i->value1, *i->dest1);
    fail_unless_equal("Continuation run count", "%u", 1, i->run_count);
}

static void
save_int2_verify2(struct save_int2 *i)
{
    fail_unless_equal("Continuation result #1", "%u", i->value1, *i->dest1);
    fail_unless_equal("Continuation result #2", "%u", i->value2, *i->dest2);
    fail_unless_equal("Continuation run count", "%u", 2, i->run_count);
}


/*-----------------------------------------------------------------------
 * Simple continuation passing
 */

START_TEST(test_cps_01)
{
    DESCRIBE_TEST;
    unsigned int  result = 0;
    struct save_int  i;
    save_int_init(&i, &result, 10);
    fail_if_error(cps_run(i.cont));
    save_int_verify(&i);
    save_int_done(&i);
}
END_TEST

START_TEST(test_cps_02)
{
    DESCRIBE_TEST;
    unsigned int  result1 = 0;
    unsigned int  result2 = 0;
    struct save_int  i1;
    struct save_int  i2;
    save_int_init(&i1, &result1, 10);
    save_int_init(&i2, &result2, 20);
    cps_resume(i1.cont, i2.cont);
    save_int_verify(&i1);
    save_int_verify(&i2);
    save_int_done(&i1);
    save_int_done(&i2);
}
END_TEST

START_TEST(test_cps_03)
{
    DESCRIBE_TEST;
    unsigned int  result1 = 0;
    unsigned int  result2 = 0;
    unsigned int  result3 = 0;
    struct save_int  i1;
    struct save_int  i2;
    struct save_int  i3;
    struct cps_rr  *rr = cps_rr_new();
    save_int_init(&i1, &result1, 10);
    save_int_init(&i2, &result2, 20);
    save_int_init(&i3, &result3, 30);
    cps_rr_add(rr, i1.cont);
    cps_rr_add(rr, i2.cont);
    cps_rr_add(rr, i3.cont);
    fail_if_error(cps_rr_drain(rr));
    save_int_verify(&i1);
    save_int_verify(&i2);
    save_int_verify(&i3);
    cps_rr_free(rr);
    save_int_done(&i1);
    save_int_done(&i2);
    save_int_done(&i3);
}
END_TEST

START_TEST(test_cps_04)
{
    DESCRIBE_TEST;
    unsigned int  result1a = 0;
    unsigned int  result1b = 0;
    unsigned int  result2a = 0;
    unsigned int  result2b = 0;
    unsigned int  result3a = 0;
    unsigned int  result3b = 0;
    struct save_int2  i1;
    struct save_int2  i2;
    struct save_int2  i3;
    struct cps_rr  *rr = cps_rr_new();
    save_int2_init(&i1, &result1a, 10, &result1b, 15);
    save_int2_init(&i2, &result2a, 20, &result2b, 25);
    save_int2_init(&i3, &result3a, 30, &result3b, 35);
    cps_rr_add(rr, i1.step1);
    cps_rr_add(rr, i2.step1);
    cps_rr_add(rr, i3.step1);
    fail_if_error(cps_rr_drain(rr));
    save_int2_verify2(&i1);
    save_int2_verify2(&i2);
    save_int2_verify2(&i3);
    cps_rr_free(rr);
    save_int2_done(&i1);
    save_int2_done(&i2);
    save_int2_done(&i3);
}
END_TEST

START_TEST(test_cps_05)
{
    DESCRIBE_TEST;
    unsigned int  result1 = 0;
    unsigned int  result2a = 0;
    unsigned int  result2b = 0;
    unsigned int  result3 = 0;
    struct save_int  i1;
    struct save_int2  i2;
    struct save_int  i3;
    struct cps_rr  *rr = cps_rr_new();
    save_int_init(&i1, &result1, 10);
    save_int2_init(&i2, &result2a, 20, &result2b, 25);
    save_int_init(&i3, &result3, 30);
    cps_rr_add(rr, i1.cont);
    cps_rr_add(rr, i2.step1);
    cps_rr_add(rr, i3.cont);
    fail_if_error(cps_rr_drain(rr));
    save_int_verify(&i1);
    save_int2_verify2(&i2);
    save_int_verify(&i3);
    cps_rr_free(rr);
    save_int_done(&i1);
    save_int2_done(&i2);
    save_int_done(&i3);
}
END_TEST

START_TEST(test_cps_06)
{
    DESCRIBE_TEST;
    unsigned int  result1a = 0;
    unsigned int  result1b = 0;
    unsigned int  result2a = 0;
    unsigned int  result2b = 0;
    unsigned int  result3a = 0;
    unsigned int  result3b = 0;
    struct save_int2  i1;
    struct save_int2  i2;
    struct save_int2  i3;
    struct cps_rr  *rr = cps_rr_new();
    save_int2_init(&i1, &result1a, 10, &result1b, 15);
    save_int2_init(&i2, &result2a, 20, &result2b, 25);
    save_int2_init(&i3, &result3a, 30, &result3b, 35);
    cps_rr_add(rr, i1.step1);
    cps_rr_add(rr, i2.step1);
    cps_rr_add(rr, i3.step1);
    fail_if_error(cps_rr_run_one_lap(rr));
    save_int2_verify1(&i1);
    save_int2_verify1(&i2);
    save_int2_verify1(&i3);
    fail_if_error(cps_rr_run_one_lap(rr));
    save_int2_verify2(&i1);
    save_int2_verify2(&i2);
    save_int2_verify2(&i3);
    cps_rr_free(rr);
    save_int2_done(&i1);
    save_int2_done(&i2);
    save_int2_done(&i3);
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
    tcase_add_test(tc_cps, test_cps_03);
    tcase_add_test(tc_cps, test_cps_04);
    tcase_add_test(tc_cps, test_cps_05);
    tcase_add_test(tc_cps, test_cps_06);
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
