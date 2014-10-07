/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013-2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <assert.h>

#include <libcork/core.h>

#include "copse/context.h"
#include "copse/cps.h"
#include "copse/fiber.h"


/*-----------------------------------------------------------------------
 * Fiber continuations
 */

#define CPS_DEFAULT_STACK_SIZE  (1 << 20)  /* 1 MB */

enum cps_fiber_state {
    CPS_FIBER_FINISHED,
    CPS_FIBER_RUNNING,
    CPS_FIBER_PAUSED
};

struct cps_fiber {
    void  *user_data;
    cork_free_f  free_user_data;
    cps_fiber_f  func;
    struct cps_cont  *cont;
    struct cps_context  *context;
    struct cps_context  ret;
    void  *stack;
    size_t  stack_size;
    enum cps_fiber_state  state;
};

static void
cps_fiber__jump_into(void *user_data)
{
    struct cps_fiber  *fiber = user_data;
    fiber->state = CPS_FIBER_RUNNING;
    fiber->func(fiber->user_data, fiber);
    fiber->state = CPS_FIBER_FINISHED;
    cps_context_jump(fiber->context, &fiber->ret, NULL, true);
}

static void
cps_fiber__resume(void *user_data, struct cps_cont *next)
{
    struct cps_fiber  *fiber = user_data;

    /* We can only resume a paused fiber. */
    assert(fiber->state == CPS_FIBER_PAUSED);

    /* Jump into the fiber's function (not necessarily for the first time). */
    cps_context_jump(&fiber->ret, fiber->context, fiber, true);

    /* When we return, the fiber will either have yielded, or the fiber's
     * function will have returned. */
    if (fiber->state == CPS_FIBER_FINISHED) {
        /* If the fiber's function finished, then we don't need to return back
         * to this continuation later on. */
        cps_call(next);
    } else {
        cps_resume(next, fiber->cont);
    }
}

static void
cps_fiber__free(void *user_data)
{
    struct cps_fiber  *fiber = user_data;
    cork_free(fiber->stack, fiber->stack_size);
    cork_free_user_data(fiber);
    cork_delete(struct cps_fiber, fiber);
}

struct cps_fiber *
cps_fiber_new(void *user_data, cork_free_f free_user_data, cps_fiber_f func,
              size_t stack_size)
{
    struct cps_fiber  *fiber = cork_new(struct cps_fiber);
    fiber->user_data = user_data;
    fiber->free_user_data = free_user_data;
    fiber->func = func;
    if (stack_size == 0) {
        stack_size = CPS_DEFAULT_STACK_SIZE;
    }
    fiber->state = CPS_FIBER_PAUSED;
    fiber->stack = cork_malloc(stack_size);
    fiber->stack_size = stack_size;
    fiber->context =
        cps_context_new(fiber->stack, stack_size, cps_fiber__jump_into);
    fiber->cont = cps_cont_new();
    cps_cont_set(fiber->cont, fiber, cps_fiber__free, cps_fiber__resume);
    return fiber;
}

void
cps_fiber_free(struct cps_fiber *fiber)
{
    cps_cont_free(fiber->cont);
}

struct cps_cont *
cps_fiber_cont(struct cps_fiber *fiber)
{
    return fiber->cont;
}

void
cps_fiber_yield(struct cps_fiber *fiber)
{
    /* Should be called from within fiber */
    assert(fiber->state == CPS_FIBER_RUNNING);

    /* Jump back to the context that yielded to us most recently.  This should
     * jump us back into the cps_fiber__resume method, returning from its
     * cps_context_jump call. */
    fiber->state = CPS_FIBER_PAUSED;
    cps_context_jump(fiber->context, &fiber->ret, NULL, true);

    /* When we return, someone else will have resumed this fiber's continuation,
     * leading to a new call to cps_fiber__resume.  Its cps_context_jump call
     * will lead here; we return back to the fiber function. */
    fiber->state = CPS_FIBER_RUNNING;
}
