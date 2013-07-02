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
#include "copse/fiber.h"
#include "copse/round-robin.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Test continuations
 */

struct save_int {
    struct cps_cont  *cont;
    const char  *name;
    unsigned int  *dest;
    unsigned int  value;
    unsigned int  run_count;
};

static void
save_int__run(void *user_data, struct cps_fiber *fiber)
{
    struct save_int  *self = user_data;
    printf("[%s] Started\n", self->name);
    self->run_count++;
    printf("[%s] Yielding\n", self->name);
    cps_fiber_yield(fiber);
    printf("[%s] Back from yield\n", self->name);
    *self->dest = self->value;
    self->run_count++;
    printf("[%s] Finished\n", self->name);
}

static void
save_int_init(struct save_int *self, const char *name,
              unsigned int *dest, unsigned int value)
{
    struct cps_fiber  *fiber;
    self->name = name;
    self->dest = dest;
    self->value = value;
    self->run_count = 0;
    fiber = cps_fiber_new(self, NULL, save_int__run, 0);
    self->cont = cps_fiber_cont(fiber);
}

static void
save_int_done(struct save_int *self)
{
    cps_cont_free(self->cont);
}

#define save_int_verify(i, rc, v) \
    do { \
        printf("[%s] Verifying %u %u\n", (i)->name, (rc), (v)); \
        ck_assert_uint_eq((rc), (i)->run_count); \
        ck_assert_uint_eq((v), *(i)->dest); \
    } while (0)


struct save_int2 {
    struct cps_cont  *cont;
    const char  *name;
    unsigned int  *dest1;
    unsigned int  value1;
    unsigned int  *dest2;
    unsigned int  value2;
    unsigned int  run_count;
};

static void
save_int2__run(void *user_data, struct cps_fiber *fiber)
{
    struct save_int2  *self = user_data;
    printf("[%s] Started\n", self->name);
    self->run_count++;
    printf("[%s] Yielding once\n", self->name);
    cps_fiber_yield(fiber);
    printf("[%s] Back from first yield\n", self->name);
    *self->dest1 = self->value1;
    self->run_count++;
    printf("[%s] Yielding twice\n", self->name);
    cps_fiber_yield(fiber);
    printf("[%s] Back from second yield\n", self->name);
    *self->dest2 = self->value2;
    self->run_count++;
    printf("[%s] Finished\n", self->name);
}

static void
save_int2_init(struct save_int2 *self, const char *name,
               unsigned int *dest1, unsigned int value1,
               unsigned int *dest2, unsigned int value2)
{
    struct cps_fiber  *fiber;
    self->name = name;
    self->dest1 = dest1;
    self->value1 = value1;
    self->dest2 = dest2;
    self->value2 = value2;
    self->run_count = 0;
    fiber = cps_fiber_new(self, NULL, save_int2__run, 0);
    self->cont = cps_fiber_cont(fiber);
}

static void
save_int2_done(struct save_int2 *self)
{
    cps_cont_free(self->cont);
}

#define save_int2_verify(i, rc, v1, v2) \
    do { \
        printf("[%s] Verifying %u %u %u\n", (i)->name, (rc), (v1), (v2)); \
        ck_assert_uint_eq((rc), (i)->run_count); \
        ck_assert_uint_eq((v1), *(i)->dest1); \
        ck_assert_uint_eq((v2), *(i)->dest2); \
    } while (0)


/*-----------------------------------------------------------------------
 * Simple continuation passing
 */

START_TEST(test_fiber_01)
{
    DESCRIBE_TEST;
    unsigned int  result = 0;
    struct save_int  i;
    save_int_init(&i, "i", &result, 10);
    fail_if_error(cps_run(i.cont));
    save_int_verify(&i, 1, 0);
    fail_if_error(cps_run(i.cont));
    save_int_verify(&i, 2, 10);
    save_int_done(&i);
}
END_TEST

START_TEST(test_fiber_02)
{
    DESCRIBE_TEST;
    unsigned int  result1 = 0;
    unsigned int  result2 = 0;
    struct save_int  i1;
    struct save_int  i2;
    save_int_init(&i1, "i1", &result1, 10);
    save_int_init(&i2, "i2", &result2, 20);
    cps_resume(i1.cont, i2.cont);
    save_int_verify(&i1, 2, 10);
    save_int_verify(&i2, 2, 20);
    save_int_done(&i1);
    save_int_done(&i2);
}
END_TEST

START_TEST(test_fiber_03)
{
    DESCRIBE_TEST;
    unsigned int  result1 = 0;
    unsigned int  result2 = 0;
    unsigned int  result3 = 0;
    struct save_int  i1;
    struct save_int  i2;
    struct save_int  i3;
    struct cps_rr  *rr = cps_rr_new();
    save_int_init(&i1, "i1", &result1, 10);
    save_int_init(&i2, "i2", &result2, 20);
    save_int_init(&i3, "i3", &result3, 30);
    cps_rr_add(rr, i1.cont);
    cps_rr_add(rr, i2.cont);
    cps_rr_add(rr, i3.cont);
    fail_if_error(cps_rr_drain(rr));
    save_int_verify(&i1, 2, 10);
    save_int_verify(&i2, 2, 20);
    save_int_verify(&i3, 2, 30);
    cps_rr_free(rr);
    save_int_done(&i1);
    save_int_done(&i2);
    save_int_done(&i3);
}
END_TEST

START_TEST(test_fiber_04)
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
    save_int2_init(&i1, "i1", &result1a, 10, &result1b, 15);
    save_int2_init(&i2, "i2", &result2a, 20, &result2b, 25);
    save_int2_init(&i3, "i3", &result3a, 30, &result3b, 35);
    cps_rr_add(rr, i1.cont);
    cps_rr_add(rr, i2.cont);
    cps_rr_add(rr, i3.cont);
    fail_if_error(cps_rr_drain(rr));
    save_int2_verify(&i1, 3, 10, 15);
    save_int2_verify(&i2, 3, 20, 25);
    save_int2_verify(&i3, 3, 30, 35);
    cps_rr_free(rr);
    save_int2_done(&i1);
    save_int2_done(&i2);
    save_int2_done(&i3);
}
END_TEST

START_TEST(test_fiber_05)
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
    save_int_init(&i1, "i1", &result1, 10);
    save_int2_init(&i2, "i2", &result2a, 20, &result2b, 25);
    save_int_init(&i3, "i3", &result3, 30);
    cps_rr_add(rr, i1.cont);
    cps_rr_add(rr, i2.cont);
    cps_rr_add(rr, i3.cont);
    fail_if_error(cps_rr_drain(rr));
    save_int_verify(&i1, 2, 10);
    save_int2_verify(&i2, 3, 20, 25);
    save_int_verify(&i3, 2, 30);
    cps_rr_free(rr);
    save_int_done(&i1);
    save_int2_done(&i2);
    save_int_done(&i3);
}
END_TEST

START_TEST(test_fiber_06)
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
    save_int2_init(&i1, "i1", &result1a, 10, &result1b, 15);
    save_int2_init(&i2, "i2", &result2a, 20, &result2b, 25);
    save_int2_init(&i3, "i3", &result3a, 30, &result3b, 35);
    cps_rr_add(rr, i1.cont);
    cps_rr_add(rr, i2.cont);
    cps_rr_add(rr, i3.cont);
    fail_if_error(cps_rr_run_one_lap(rr));
    save_int2_verify(&i1, 1, 0, 0);
    save_int2_verify(&i2, 1, 0, 0);
    save_int2_verify(&i3, 1, 0, 0);
    fail_if_error(cps_rr_run_one_lap(rr));
    save_int2_verify(&i1, 2, 10, 0);
    save_int2_verify(&i2, 2, 20, 0);
    save_int2_verify(&i3, 2, 30, 0);
    fail_if_error(cps_rr_run_one_lap(rr));
    save_int2_verify(&i1, 3, 10, 15);
    save_int2_verify(&i2, 3, 20, 25);
    save_int2_verify(&i3, 3, 30, 35);
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
    Suite  *s = suite_create("fiber");

    TCase  *tc_cps = tcase_create("fiber");
    tcase_add_test(tc_cps, test_fiber_01);
    tcase_add_test(tc_cps, test_fiber_02);
    tcase_add_test(tc_cps, test_fiber_03);
    tcase_add_test(tc_cps, test_fiber_04);
    tcase_add_test(tc_cps, test_fiber_05);
    tcase_add_test(tc_cps, test_fiber_06);
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
