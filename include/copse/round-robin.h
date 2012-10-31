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


struct cps_rr {
    struct cps_cont  start;
    struct cps_cont  yield;
};


struct cps_rr *
cps_rr_new(void);

void
cps_rr_free(struct cps_rr *sched);

void
cps_rr_add(struct cps_rr *sched, struct cps_cont *cont);

#define cps_rr_resume(s, n)  (cps_run(&(s)->start, (n)))
#define cps_rr_run(s)        (cps_run(&(s)->start))


#endif /* COPSE_ROUND_ROBIN_H */
