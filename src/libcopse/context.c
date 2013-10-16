/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <assert.h>

#include <libcork/core.h>

#include "valgrind/valgrind.h"

#include "copse/context.h"
#include "copse/cps.h"
#include "copse/fiber.h"

struct cps_context *
cps_context_new_from_sp(void *sp, size_t size, cps_context_f func);

struct cps_context *
cps_context_new(void *sp, size_t size, cps_context_f func)
{
    /* Register this new stack with Valgrind to suppress spurious undefined
     * value warnings. */
    (void) VALGRIND_STACK_REGISTER(sp, sp + size);

#if CPS_STACK_GROWS_DOWN
    /* The sp parameter always points to the bottom of the stack; switch it to
     * the top if stacks grow downward on this platform. */
    sp += size;
#endif

    return cps_context_new_from_sp(sp, size, func);
}
