/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef COPSE_FIBER_H
#define COPSE_FIBER_H

#include <libcork/core.h>

#include <copse/context.h>


/*-----------------------------------------------------------------------
 * Fiber continuations
 */

struct cps_fiber;

typedef void
(*cps_fiber_f)(void *user_data, struct cps_fiber *fiber);

struct cps_fiber *
cps_fiber_new(void *user_data, cork_free_f free_user_data, cps_fiber_f func,
              size_t stack_size);

void
cps_fiber_free(struct cps_fiber *fiber);

struct cps_cont *
cps_fiber_cont(struct cps_fiber *fiber);

void
cps_fiber_yield(struct cps_fiber *fiber);


#endif /* COPSE_FIBER_H */
