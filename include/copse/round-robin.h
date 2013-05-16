/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef COPSE_ROUND_ROBIN_H
#define COPSE_ROUND_ROBIN_H


#include <copse/cps.h>


struct cps_rr;

struct cps_rr *
cps_rr_new(void);

void
cps_rr_free(struct cps_rr *rr);

/* Add a continuation to the end of the round-robin scheduler's work queue.
 * This can be safely called from a continuation that the scheduler started.
 * It's *not* thread-safe, though. */
void
cps_rr_add(struct cps_rr *rr, struct cps_cont *cont);

struct cps_cont *
cps_rr_get_yield(struct cps_rr *rr);

/* Add `next` to the work queue, then start executing continuations in the
 * queue.  This function will return when the first `cps_done` continuation is
 * encountered in the queue. */
int
cps_rr_resume(struct cps_rr *rr, struct cps_cont *next);

/* Execute all of the continuations that are currently in the round-robin
 * scheduler's work queue.  If these continuations add anything to the work
 * queue, those continuations won't be executed during this function call;
 * they'll still be in the queue, however, for later calls. */
int
cps_rr_run_one_lap(struct cps_rr *rr);

/* Execute all of the continuations that are in the round-robin scheduler's work
 * queue.  If these continuations add anything to the work queue, execute those
 * continuations as well.  Repeat until there is nothing left in the work queue,
 * then invoke the `next` continuation. */
int
cps_rr_drain(struct cps_rr *rr);


#endif /* COPSE_ROUND_ROBIN_H */
