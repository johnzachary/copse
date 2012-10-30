/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef COPSE_CPS_H
#define COPSE_CPS_H


struct cps_cont {
    int
    (*resume)(struct cps_cont *cont, struct cps_cont *next);
};

#define cps_resume(c, n) ((c)->resume((c), (n)))


extern struct cps_cont  cps_done_cont;

#define cps_done(n)  (cps_resume((n), &cps_done_cont))
#define cps_run(n)   (cps_resume((n), &cps_done_cont))


#endif /* COPSE_CPS_H */
